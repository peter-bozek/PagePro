#
#  build_win.ps1
#  ReportWriter
#
#  Builds PoDoFo 1.0 as a static x64 library for the Windows plugin, with the same
#  PoDoFo version and options as build_mac.sh:
#
#	podofo\win64\include	PoDoFo headers
#	podofo\win64\lib		podofo.lib podofo_private.lib podofo_3rdparty.lib and the
#							dependency libraries from vcpkg (freetype, OpenSSL, libpng,
#							libjpeg-turbo, zlib, libxml2)
#
#  Dependencies come from vcpkg (triplet x64-windows-static-md: static libraries,
#  dynamic CRT like the plugin DLL). PoDoFo itself is built from the tagged source.
#
#  Usage (Developer PowerShell for Visual Studio 2022):
#		podofo\build_win.ps1 [-Vcpkg C:\vcpkg]
#
#  Requirements: Visual Studio 2022 (C++17), CMake >= 3.23, git, vcpkg.
#
#  Not run yet (written on the Mac): check on the first Windows build.

param (
	[string] $Vcpkg = $env:VCPKG_ROOT
)

$ErrorActionPreference = "Stop"

$PodofoTag = "1.0.4"
$Triplet = "x64-windows-static-md"

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Work = Join-Path $Root ".build-win"
$Out = Join-Path $Root "win64"

if (-not $Vcpkg -or -not (Test-Path (Join-Path $Vcpkg "vcpkg.exe"))) {
	throw "vcpkg not found: pass -Vcpkg <dir> or set VCPKG_ROOT"
}

Write-Host "Installing dependencies with vcpkg ($Triplet)"
& (Join-Path $Vcpkg "vcpkg.exe") install --triplet $Triplet "freetype[core,zlib]" openssl libpng libjpeg-turbo zlib "libxml2[core,zlib]"
if ($LASTEXITCODE -ne 0) { throw "vcpkg install failed" }
$Installed = Join-Path $Vcpkg "installed\$Triplet"

New-Item -ItemType Directory -Force -Path $Work | Out-Null
$Src = Join-Path $Work "podofo"
if (-not (Test-Path $Src)) {
	git -c advice.detachedHead=false clone --quiet --depth 1 --branch $PodofoTag https://github.com/podofo/podofo.git $Src
	if ($LASTEXITCODE -ne 0) { throw "git clone failed" }
}

Write-Host "Building PoDoFo $PodofoTag"
$Build = Join-Path $Work "build"
$Prefix = Join-Path $Work "prefix"
cmake -S $Src -B $Build -G "Visual Studio 17 2022" -A x64 `
	"-DCMAKE_TOOLCHAIN_FILE=$Vcpkg\scripts\buildsystems\vcpkg.cmake" `
	"-DVCPKG_TARGET_TRIPLET=$Triplet" `
	"-DCMAKE_INSTALL_PREFIX=$Prefix" `
	"-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded`$<`$<CONFIG:Debug>:Debug>DLL" `
	-DPODOFO_BUILD_STATIC=ON -DPODOFO_BUILD_LIB_ONLY=ON `
	-DCMAKE_DISABLE_FIND_PACKAGE_TIFF=ON `
	-DCMAKE_DISABLE_FIND_PACKAGE_Fontconfig=ON `
	-DCMAKE_DISABLE_FIND_PACKAGE_LCMS2=ON
if ($LASTEXITCODE -ne 0) { throw "cmake configure failed" }
cmake --build $Build --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw "cmake build failed" }
cmake --install $Build --config Release
if ($LASTEXITCODE -ne 0) { throw "cmake install failed" }

Write-Host "Copying to $Out"
if (Test-Path $Out) { Remove-Item -Recurse -Force $Out }
New-Item -ItemType Directory -Force -Path (Join-Path $Out "include"), (Join-Path $Out "lib") | Out-Null
Copy-Item -Recurse (Join-Path $Prefix "include\podofo") (Join-Path $Out "include")
Copy-Item (Join-Path $Prefix "lib\*.lib") (Join-Path $Out "lib")
foreach ($lib in "freetype.lib", "libssl.lib", "libcrypto.lib", "libpng16.lib", "jpeg.lib", "zlib.lib", "libxml2.lib") {
	$path = Join-Path $Installed "lib\$lib"
	if (Test-Path $path) { Copy-Item $path (Join-Path $Out "lib") }
	else { Write-Warning "$lib not found in $Installed\lib (check the vcpkg library names)" }
}

Get-ChildItem (Join-Path $Out "lib")
Write-Host "Done. ReportWriter.vcxproj links these libraries (plus ws2_32, crypt32, bcrypt) and defines PODOFO_STATIC."
