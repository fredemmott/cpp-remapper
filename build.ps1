<#

.SYNOPSIS

    Build a profile or utility

.PARAMETER Target

    The executable to build

.EXAMPLE

  ./build.ps1 example
  ./example.exe

.EXAMPLE

  ./build.ps1 list-devices
  ./list-devices.exe

#>

[cmdletbinding()]
param(
  [Parameter(Mandatory=$true)]
  [string] $Target
)

$VSPATH=vswhere -property installationPath -version 16
Import-Module (Join-Path $VSPATH "Common7\Tools\Microsoft.VisualStudio.DevShell.dll")
Enter-VsDevShell -VsInstallPath $VSPATH -SkipAutomaticLocation

$Name=Split-Path "$Target" -Leaf

cl "/Fe$Name.exe" /I. /ISDK/inc /Ilib /EHsc /vmg /std:c++17 lib/*.cpp "$Target.cpp"
