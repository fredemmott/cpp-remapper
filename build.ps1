<#

.SYNPOSIS

  Legacy/convenience build tool

.EXAMPLE

  ./build.ps1
  ./build.ps1 -Config Debug
  ./build.ps1 -Clang -Config Debug
#>

[cmdletbinding()]
param(
  [string] $Config = "RelWithDebInfo",
  [switch] $Clang
)

$Compiler = "cl"
$CMakeArgs = @();

if ($Clang) {
  $Compiler = "clang";
  $CMakeArgs += @("-T", "ClangCL")
}

$BuildDir = "build/${Compiler}"
$InstallDir = "$(Get-Location)/out/${Config}-${Compiler}"
$InstallAliasDir = "$(Get-Location)/out"

cmake -S . -B $BuildDir -DBUILD_LEGACY_PROFILES=ON @CMakeArgs
cmake --build $BuildDir --config $Config --parallel
cmake --install $BuildDir --config $Config --prefix $InstallDir
cmake --install $BuildDir --config $Config --prefix $InstallAliasDir
