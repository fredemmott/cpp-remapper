<#

.SYNOPSIS

    Build a profile or utility

.PARAMETER ProfileSource

    The C++ profile source to build

.EXAMPLE

  ./build.ps1 example.cpp
  ./example.exe

.EXAMPLE

  ./build.ps1 list-devices.cpp
  ./list-devices.exe

#>

[cmdletbinding()]
param(
  [string] $ProfileSource,
  [string] $IDEBuildFile,
  [ValidateSet('Release', 'Debug')]
  [string] $BuildMode = 'Release',
  [ValidateSet('x86', 'x64')]
  [string] $Platform = 'x64',
  [switch] $ForceRebuild
)

if ($IDEBuildFile) {
  if ((Get-Item $IDEBuildFile).Directory.FullName -eq (Get-Item profiles).FullName) {
    $ProfileSource = $IDEBuildFile
  } elseif ((Get-Item $IDEBuildFile).Directory.FullName -eq (Get-Item .).FullName -and $IDEBuildFile.endsWith('.cpp')) {
    $ProfileSource = $IDEBuildFile
  } else {
    $ProfileSource = "test.cpp"
  }
}

$CWD = (Get-Item .).FullName
$IntermediateDir = "$CWD\build\${Platform}-${BuildMode}"
$ExeSuffix = "-${BuildMode}";
New-Item -Force -Path $IntermediateDir -ItemType Directory | Out-Null

switch($BuildMode) {
  "Debug" {
    $CLFlags=@("/Zi", "/MTd", "/fsanitize=address")
    $LINKFlags=@("/DEBUG:FULL")
  }
  "Release" {
    $CLFlags=@("/O2", "/MT")
    $LINKFlags=@()
    $ExeSuffix=''
  }

  default {
    Write-Output "Unsupported BuildMode '$BuildMode'"
    exit 1
  }
}

switch($Platform) {
  "x86" {
    $VCVarsBat = "vcvars32.bat"
    $ExeSuffix = "$ExeSuffix-x86"
    # VJoy SDK
    $LINKFlags += "/LIBPATH:SDK\lib"
  }
  "x64" {
    $VCVarsBat = "vcvars64.bat"
    # VJoy SDK
    $LINKFlags += "/LIBPATH:SDK\lib\amd64"
  }

  default {
    Write-Output "Unsupported platform '$Platform'"
    exit 1
  }
}
$LibViGEmClient="ViGEmClient\lib\$BuildMode\$Platform\ViGEmClient.lib"

$ErrorActionPreference = "Stop"
function Invoke-Exe-Checked {
	param ( [scriptblock] $Block )
	&$Block
	if ($LASTEXITCODE -ne 0) {
		exit $LASTEXITCODE
	}
}


# Using vcvars[64].bat as Enter-VsDevShell does not reliably get an x64 environment
$VSPATH=vswhere -property installationPath -version 16
$VCVars=New-TemporaryFile
Invoke-Exe-Checked { cmd.exe /c "call `"$VSPATH\VC\Auxiliary\Build\$VCVarsBat`" && set > $VCVars" }
Get-Content $VCVars | ForEach-Object {
  $Var, $Value = $_.split("=")
  Set-Item -Path "Env:$Var" -Value $Value
}

# Needed by HidHideCli
$ProjectDirLength=(Get-Location).Path.Length

function Rebuild-If-Outdated {
  param (
    [Parameter(Mandatory=$true)]
    [string] $Target,
    [Parameter(Mandatory=$true)]
    [string[]] $Sources,
    [Parameter(Mandatory=$true)]
    [scriptblock] $Impl
  )
  if (Split-Path $Target -Parent) {
    New-Item -Force -Path (Split-Path $Target -Parent) -ItemType Directory | Out-Null
  }

  $Wrap = {
    Write-Output "[$Target]: Out of date, rebuilding."
    &$Impl
  }

  if ($ForceRebuild -or !(Test-Path $Target)) {
    &$Wrap
    return
  }

  $TargetMTime=$(Get-Item $Target).LastWriteTime
  foreach ($Source in $Sources) {
    if ((Get-Item "$Source").LastWriteTime -gt $TargetMTime) {
      &$Wrap
      return
    }
  }

  Write-Output "[$Target] Up to date."
}

function Get-Relative-Directory{
  param (
    [Parameter(Mandatory=$true)]
    [string] $FilePath
  )
  $Directory=Split-Path $FilePath -Parent
  if ($Directory -eq $CWD -or $Directory -eq "") {
    return "";
  } elseif ($Directory.StartsWith("$CWD\")) {
     return $Directory.Substring($CWD.Length + 1) + "\";
  }
  return $Directory+"\";
}

function Get-Relative-Name {
  param (
    [Parameter(Mandatory=$true)]
    [string] $FilePath
  )

  $Directory=(Get-Relative-Directory $FilePath)
  return "$Directory$(Split-Path $FilePath -Leaf)"
}

function Swap-Extension {
  param (
    [Parameter(Mandatory=$true)]
    [string] $Path,
    [Parameter(Mandatory=$true)]
    [string] $NewExtension
  )
  $File = (Get-Item $Path)
  $Directory = (Get-Relative-Directory $File.FullName)
  return "$Directory$($File.BaseName).$NewExtension"
}

function Get-Cpp-Obj-Name {
  param (
    [Parameter(Mandatory=$true)]
    [string] $Cpp
  )

  return Get-Relative-Name "$IntermediateDir\$(Swap-Extension -Path (Get-Relative-Name $Cpp) -NewExtension "obj")"
}

function Cpp-Obj-Rule {
  param (
    [Parameter(Mandatory=$true)]
    [string] $Target,
    [Parameter(Mandatory=$true)]
    [string] $Cpp,
    [Parameter(Mandatory=$true)]
    [string[]] $Headers
  )

  Rebuild-If-Outdated -Target $Target -Sources (@($Cpp) + $Headers) -Impl {
    Write-Output "  CL: ${Target}: $Cpp"
    Invoke-Exe-Checked {
      CL.exe /nologo /c "/Fo$Target" /I. /IViGEmClient\include /ISDK/inc /Ilib /EHsc /vmg /Zc:__cplusplus /std:c++17 /DProjectDirLength=$ProjectDirLength $CLFlags $Cpp
    }
  }
}

function Cpp-StaticLib-Rule {
  param (
    [Parameter(Mandatory=$true)]
    [string] $Target,
    [Parameter(Mandatory=$true)]
    [string[]] $Sources,
    [Parameter(Mandatory=$true)]
    [string[]] $Headers
  )

  foreach($Source in $Sources) {
    Cpp-Obj-Rule -Target (Get-Cpp-Obj-Name $Source) -Cpp $Source -Headers $Headers
  }

  $Objs=$Sources | ForEach-Object { Get-Cpp-Obj-Name $_ }

  Rebuild-If-Outdated -Target $Target -Sources $Objs -Impl {
    Write-Output "  LIB: ${Target}: $Objs "
    Invoke-Exe-Checked { LIB.exe /nologo "/Out:$Target" $Objs }
  }
}

function Objs-Exe-Rule {
  param (
    [Parameter(Mandatory=$true)]
    [string] $Target,
    [Parameter(Mandatory=$true)]
    [string[]] $Objects
  )

  Rebuild-If-Outdated -Target $Target -Sources $Objects -Impl {
    Write-Output "  LINK: ${Target}: $Objects $LINKFlags"
    Invoke-Exe-Checked { LINK.exe /nologo "/Out:$Target" /LTCG $LINKFlags $Objects }
  }

}
$ViGEmClientSources=(Get-ChildItem -Path ViGEmClient -Recurse -File -Include @("*.cpp", "*.h")).FullName

Rebuild-If-Outdated -Target $LibViGEmClient -Sources $ViGEmClientSources -Impl {
  Write-Output "  MSBUILD: ViGEmClient ${BuildMode}_LIB"
  Push-Location ViGEmClient
  Invoke-Exe-Checked { msbuild -noLogo "-p:Configuration=${BuildMode}_LIB" "-p:Platform=$Platform" }
  Pop-Location
}

$CppRemapperHeaders=(Get-Item lib/*.h).FullName
$HidHideHeaders=(Get-Item lib/HidHideCLI/*.h).FullName

$LibCppRemapper = Get-Relative-Name "$IntermediateDir\cpp-remapper.lib"
$LibHidHide = Get-Relative-Name "$IntermediateDir\hidhide.lib"

Cpp-StaticLib-Rule `
  -Target $LibHidHide `
  -Sources (Get-Item lib/HidHideCLI/*.cpp | ForEach-Object { Get-Relative-Name $_.FullName }) `
  -Headers $HidHideHeaders
Cpp-StaticLib-Rule `
  -Target $LibCppRemapper `
  -Sources (Get-Item lib/*.cpp | ForEach-Object { Get-Relative-Name $_.FullName }) `
  -Headers ($CppRemapperHeaders + $HidHideHeaders)

if (!$ProfileSource) {
  return;
}

Write-Output "Profile: $ProfileSource"
$ProfileObj=Get-Cpp-Obj-Name $ProfileSource

Cpp-Obj-Rule -Target $ProfileObj -Cpp $ProfileSource -Headers $CppRemapperHeaders

(Get-Command cl.exe).Path
Objs-Exe-Rule `
  -Target "$((Get-Item $ProfileSource).BaseName)$ExeSuffix.exe" `
  -Objects @($ProfileObj, $LibCppRemapper, $LibHidHide, $LibViGEmClient)
