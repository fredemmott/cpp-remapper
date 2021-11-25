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
  [string[]] $Profiles,
  [ValidateSet('Release', 'Debug')]
  [string] $BuildMode = 'Release',
  [ValidateSet('x86', 'x64')]
  [string] $Platform = 'x64',
  [switch] $ForceRebuild,
  [switch] $GuessIntentForIDE,
  [ValidateSet('clang', 'cl')]
  [string] $Compiler = 'cl',
  [switch] $ShowExeSuffix
)

$AllProfiles = (Get-Item *.cpp).Name
if (Test-Path profiles) {
  $AllProfiles += Get-Item profiles\*.cpp | ForEach-Object { "profiles\$($_.Name)" }
}
if (($Profiles -eq @("all")) -or ($null -eq $Profiles)) {
  $Profiles = $AllProfiles
}

if ($GuessIntentForIDE) {
  $IDEFile = $Profiles[0];
  if ((Get-Item $IDEFile).Directory.FullName -eq (Get-Item profiles).FullName) {
    $Profiles = @($IDEFile)
  } elseif ((Get-Item $IDEFile).Directory.FullName -eq (Get-Item .).FullName -and $IDEFile.endsWith('.cpp')) {
    $Profiles = $($IDEFile)
  } else {
    $Profiles = $AllProfiles
  }
}

$ExeSuffix = ''
$CWD = (Get-Item .).FullName
$IntermediateDir = "$CWD\build\${Platform}-${Compiler}-${BuildMode}"
New-Item -Force -Path $IntermediateDir -ItemType Directory | Out-Null

$LINKFlags = @()
$CLFlags = @(
  "/nologo",
  "/I.",
  "/IViGEmClient\include",
  "/ISDK/inc",
  "/Ilib",
  "/EHsc",
  "/vmg",
  "/Zc:__cplusplus",
  "/std:c++20",
  "/FC",
  "/DNOMINMAX",
  "/DProjectDirLength=$((Get-Location).Path.Length)" # Needed by HidHideCli
)

switch($Platform) {
  "x86" {
    $VCVarsBat = "vcvars32.bat"
    $ExeSuffix += "-x86"
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

switch($BuildMode) {
  "Debug" {
    $CLFlags += @("/Zi", "/MTd")
    $LINKFlags += @("/DEBUG:FULL")

    if ($Compiler -eq "cl") {
      $CLFlags += "/fsanitize=address";
    }
    $ExeSuffix += "-Debug"
  }
  "Release" {
    $CLFlags += @("/O2", "/MT")
  }

  default {
    Write-Output "Unsupported BuildMode '$BuildMode'"
    exit 1
  }
}


$CL = 'cl.exe'
if ($Compiler -eq "clang") {
  $CL = 'clang-cl.exe'
  $CLFlags += @(
    "/D_CLANG_CL",
    # Extra flags for HidHideCLI
    "-Wno-format",
    "-Wno-pragma-pack",
    "-Wno-pragma-once-outside-header"
  )
  $ExeSuffix += "-clang"
}

if ($ShowExeSuffix) {
  Write-Output $ExeSuffix
  exit
}

$LibViGEmClient="ViGEmClient\lib\$BuildMode\$Platform\ViGEmClient.lib"

$ErrorActionPreference = "Stop"
function Invoke-Exe-Checked-Raw{
  param ( [scriptblock] $Block )
  &$Block
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }
}

# Using vcvars[64].bat as Enter-VsDevShell does not reliably get an x64 environment
$VSPATH=vswhere -property installationPath -version '16' -latest
$VCVars=New-TemporaryFile
Invoke-Exe-Checked-Raw { cmd.exe /c "call `"$VSPATH\VC\Auxiliary\Build\$VCVarsBat`" && set > $VCVars" }

# With MSVC Env
function Invoke-Exe-Checked {
  param ( [scriptblock] $Block )

  $SavedEnv = Get-Item -Path 'Env:'
  Get-Content $VCVars | ForEach-Object {
    $Var, $Value = $_.split("=")
    Set-Item -Path "Env:$Var" -Value $Value
  }

  &$Block
  foreach ($Item in $SavedEnv.GetEnumerator()) {
    Set-Item -Path "Env:$($Item.Key)" -Value $Item.Value
  }
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }
}

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
    if (!(Test-Path $Source)) {
      &$Wrap
      return;
    }
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
  } elseif ($Directory.ToLower().StartsWith("$CWD\".ToLower())) {
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
    [string] $Cpp
  )

  $Headers = @()

  $Cpp = (Get-Relative-Name $Cpp)

  if ($Compiler -eq "cl") {
    $DepsPath = (Get-Relative-Name "$IntermediateDir\$Cpp.deps.json")
    $DepsCLFlags = @("/sourceDependencies", $DepsPath)
    if (Test-Path $DepsPath) {
      $Headers = Get-Content $DepsPath | Out-String | ConvertFrom-Json | % { $_.Data.Includes }
    }
  }
  if ($Compiler -eq "clang") {
    $DepsPath = (Get-Relative-Name "$IntermediateDir\$Cpp.deps.txt")
    $DepsCLFlags = @("/clang:-MD", "/clang:-MF$DepsPath")
    if (Test-Path $DepsPath) {
      # Make-style foo.o: foo.h bar.h
      $Str = (Get-Content $DepsPath).Trim() | Out-String
      # Skip 'foo.o':
      $Str = $Str.Substring($Str.IndexOf(':') + 2);
      $Headers = (
        $Str `
        -Replace " \\`r`n", "`r`n" `
        -Replace "([^\\]) ", "`$1`r`n" `
        -Replace "\\ ", " "
      ).Trim() -Split "`r`n" | ForEach-Object { $_.Trim() }
    }
  }

  Rebuild-If-Outdated -Target $Target -Sources (@($Cpp) + $Headers) -Impl {
    Write-Output "  CXX ($Compiler): ${Target}: $Cpp"
    Invoke-Exe-Checked {
      & $CL $CLFlags $DepsCLFlags /c "/Fo$Target" (Get-Item $Cpp).FullName
    }
  }
}

function Cpp-StaticLib-Rule {
  param (
    [Parameter(Mandatory=$true)]
    [string] $Target,
    [Parameter(Mandatory=$true)]
    [string[]] $Sources
  )

  foreach($Source in $Sources) {
    Cpp-Obj-Rule -Target (Get-Cpp-Obj-Name $Source) -Cpp $Source
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

$LibCppRemapper = Get-Relative-Name "$IntermediateDir\cpp-remapper.lib"
$LibHidHide = Get-Relative-Name "$IntermediateDir\hidhide.lib"

Cpp-StaticLib-Rule `
  -Target $LibHidHide `
  -Sources (Get-Item lib/HidHideCLI/*.cpp | ForEach-Object { Get-Relative-Name $_.FullName })
Cpp-StaticLib-Rule `
  -Target $LibCppRemapper `
  -Sources (Get-Item lib/*.cpp | ForEach-Object { Get-Relative-Name $_.FullName })

if (!$Profiles) {
  return;
}

foreach ($ProfileSource in $Profiles) {
  $ProfileObj=Get-Cpp-Obj-Name $ProfileSource

  Cpp-Obj-Rule -Target $ProfileObj -Cpp $ProfileSource

  Objs-Exe-Rule `
    -Target "$((Get-Item $ProfileSource).BaseName)$ExeSuffix.exe" `
    -Objects @($ProfileObj, $LibCppRemapper, $LibHidHide, $LibViGEmClient)
}
