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

cl "/Fe$Target.exe" /I. /ISDK/inc /Ilib /EHsc /vmg /std:c++17 lib/*.cpp "$Target.cpp"
