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
  [Parameter(Mandatory=$true)]
  [string] $ProfileSource,
  [ValidateSet('Release', 'Debug')]
  [string] $BuildMode = 'Release',
  [switch] $ForceRebuild
)

switch($BuildMode) {
  "Debug" {
    $CLFlags=@("/Zi")
    $LINKFlags=@("/DEBUG:FULL")
  }
  "Release" {
    $CLFlags=@("/O2")
    $LINKFlags=@()
  }

  default {
    Write-Output "Unsupported BuildMode '$BuildMode'"
    exit 1
  }
}

$ErrorActionPreference = "Stop"

$VSPATH=vswhere -property installationPath -version 16
Import-Module (Join-Path $VSPATH "Common7\Tools\Microsoft.VisualStudio.DevShell.dll")
Enter-VsDevShell -VsInstallPath $VSPATH -SkipAutomaticLocation

# Needed by HidHideCli
$ProjectDirLength=(Get-Location).Path.Length

$CWD = (Get-Item .).FullName

function Rebuild-If-Outdated {
  param (
    [Parameter(Mandatory=$true)]
    [string] $Target,
    [Parameter(Mandatory=$true)]
    [string[]] $Sources,
    [Parameter(Mandatory=$true)]
    [scriptblock] $Impl
  )

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

function Invoke-Exe-Checked {
  param ( [scriptblock] $Block )
  &$Block
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }
}

function Get-Relative-Directory{
  param (
    [Parameter(Mandatory=$true)]
    [string] $FilePath
  )
  $File=(Get-Item $FilePath);
  $Directory=($File.Directory.FullName);
  if ($Directory -eq $CWD) {
    return "";
  } elseif ($Directory.StartsWith("$CWD\")) {
     return $Directory.Substring($CWD.Length + 1) + "/";
  }
  return $Directory;
}

function Get-Relative-Name {
  param (
    [Parameter(Mandatory=$true)]
    [string] $FilePath
  )

  $File=(Get-Item $FilePath)
  $Directory=(Get-Relative-Directory $File.FullName)
  return "$Directory$($File.Name)"
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

function Cpp-Obj-Rule {
  param (
    [Parameter(Mandatory=$true)]
    [string] $Cpp,
    [Parameter(Mandatory=$true)]
    [string[]] $Headers
  )

  $Target=Swap-Extension -Path $Cpp -NewExtension "obj"

  Rebuild-If-Outdated -Target $Target -Sources (@($Cpp) + $Headers) -Impl {
    Write-Output "  CL: ${Target}: $Cpp"
    Invoke-Exe-Checked { CL.exe /nologo /c "/Fo$Target" /I. /ISDK/inc /Ilib /EHsc /vmg /std:c++17 /DProjectDirLength=$ProjectDirLength $CLFlags $Cpp }
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
    Cpp-Obj-Rule -Cpp $Source -Headers $Headers
  }

  $Objs=$Sources | ForEach-Object { Swap-Extension -Path $_ -NewExtension "obj" }

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
	  Invoke-Exe-Checked { LINK.exe /nologo "/Out:$Target" $LINKFlags $Objects }
  }

}

$ProfileObj=(Swap-Extension -Path $ProfileSource -NewExtension "obj")

$CppRemapperHeaders=(Get-Item lib/*.h).FullName
$HidHideHeaders=(Get-Item lib/HidHideCLI/*.h).FullName

Cpp-StaticLib-Rule `
  -Target hidhide.lib `
  -Sources (Get-Item lib/HidHideCLI/*.cpp | ForEach-Object { Get-Relative-Name $_.FullName }) `
  -Headers $HidHideHeaders
Cpp-StaticLib-Rule `
  -Target cpp-remapper.lib `
  -Sources (Get-Item lib/*.cpp | ForEach-Object { Get-Relative-Name $_.FullName }) `
  -Headers ($CppRemapperHeaders + $HidHideHeaders)

Cpp-Obj-Rule -Cpp $ProfileSource -Headers $CppRemapperHeaders

Objs-Exe-Rule `
  -Target "$((Get-Item $ProfileSource).BaseName).exe" `
  -Objects @($ProfileObj, "cpp-remapper.lib", "hidhide.lib")
