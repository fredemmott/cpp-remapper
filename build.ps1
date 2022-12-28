<#

.SYNPOSIS

  Legacy/convenience build tool

.EXAMPLE

  ./build.ps1
  ./build.ps1 -Debug
#>

[cmdletbinding()]
param(
  [string] $Config = "RelWithDebInfo"
)

$Compiler = "cl"

$BuildDir = "build/${Compiler}"
$InstallDir = "$(Get-Location)/out/${Config}-${Compiler}"
$InstallAliasDir = "$(Get-Location)/out"

cmake -S . -B $BuildDir -DBUILD_LEGACY_PROFILES=ON 
cmake --build $BuildDir --config $Config --parallel
cmake --install $BuildDir --config $Config --prefix $InstallDir
cmake --install $BuildDir --config $Config --prefix $InstallAliasDir
