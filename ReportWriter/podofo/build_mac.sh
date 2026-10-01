#!/bin/bash
#
#  build_mac.sh
#  ReportWriter
#
#  Builds PoDoFo 1.0 and the libraries it needs as static, universal (arm64 + x86_64)
#  libraries for the plugin:
#
#	podofo/mac/include	PoDoFo headers (its public headers include none of the dependencies)
#	podofo/mac/lib		libpodofo.a libpodofo_private.a libpodofo_3rdparty.a
#						libfreetype.a libssl.a libcrypto.a libpng16.a libjpeg.a
#
#  zlib and libxml2 come from the macOS SDK (link -lz -lxml2).
#  Sources are cloned at fixed tags into podofo/.build (not committed).
#
#  Usage:	podofo/build_mac.sh				build everything
#			KEEP_BUILD=1 podofo/build_mac.sh	keep podofo/.build for incremental rebuilds
#
#  Requirements: Xcode command line tools, CMake >= 3.23, git, perl.

set -euo pipefail

PODOFO_TAG=1.0.4
FREETYPE_TAG=VER-2-14-3
OPENSSL_TAG=openssl-3.5.9
LIBPNG_TAG=v1.6.59
LIBJPEG_TAG=3.1.4

ARCHS="arm64 x86_64"
export MACOSX_DEPLOYMENT_TARGET=10.15	# std::filesystem (RWStr::WriteFile) needs 10.15

ROOT=$(cd "$(dirname "$0")" && pwd)
WORK="$ROOT/.build"
OUT="$ROOT/mac"
JOBS=$(sysctl -n hw.ncpu)

SDK=$(xcrun --sdk macosx --show-sdk-path)

# keep Homebrew / MacPorts copies out of every find_package
CMAKE_COMMON=(
	-DCMAKE_BUILD_TYPE=Release
	-DCMAKE_OSX_SYSROOT="$SDK"
	-DCMAKE_OSX_DEPLOYMENT_TARGET="$MACOSX_DEPLOYMENT_TARGET"
	-DCMAKE_POSITION_INDEPENDENT_CODE=ON
	-DCMAKE_IGNORE_PREFIX_PATH="/usr/local;/opt/homebrew;/opt/local"
	-DCMAKE_FIND_FRAMEWORK=LAST
	-DBUILD_SHARED_LIBS=OFF
)

fetch ()	# url tag dir
{
	if [ ! -d "$WORK/src/$3" ]; then
		git -c advice.detachedHead=false clone --quiet --depth 1 --branch "$2" "$1" "$WORK/src/$3"
	fi
}

cmake_build ()	# source-dir build-dir prefix arch [options...]
{
	local src=$1 bld=$2 prefix=$3 arch=$4
	shift 4
	cmake -S "$src" -B "$bld" "${CMAKE_COMMON[@]}" \
		-DCMAKE_OSX_ARCHITECTURES="$arch" \
		-DCMAKE_INSTALL_PREFIX="$prefix" \
		-DCMAKE_PREFIX_PATH="$prefix" \
		"$@" > "$bld.log" 2>&1 || { tail -40 "$bld.log"; exit 1; }
	cmake --build "$bld" --config Release --parallel "$JOBS" >> "$bld.log" 2>&1 || { tail -40 "$bld.log"; exit 1; }
	cmake --install "$bld" --config Release >> "$bld.log" 2>&1 || { tail -40 "$bld.log"; exit 1; }
}

[ "${KEEP_BUILD:-0}" = 1 ] || rm -rf "$WORK"
mkdir -p "$WORK/src"

echo "Fetching sources"
fetch https://github.com/podofo/podofo.git				"$PODOFO_TAG"	podofo
fetch https://github.com/freetype/freetype.git			"$FREETYPE_TAG"	freetype
fetch https://github.com/openssl/openssl.git			"$OPENSSL_TAG"	openssl
fetch https://github.com/pnggroup/libpng.git			"$LIBPNG_TAG"	libpng
fetch https://github.com/libjpeg-turbo/libjpeg-turbo.git	"$LIBJPEG_TAG"	libjpeg

for arch in $ARCHS; do
	prefix="$WORK/prefix-$arch"
	bld="$WORK/build-$arch"
	mkdir -p "$prefix" "$bld"

	echo "[$arch] OpenSSL"
	if [ ! -f "$prefix/lib/libcrypto.a" ]; then
		rm -rf "$bld/openssl"
		cp -R "$WORK/src/openssl" "$bld/openssl"
		( cd "$bld/openssl" &&
		  ./Configure "darwin64-$arch-cc" no-shared no-tests no-apps no-docs no-module \
			--prefix="$prefix" --libdir=lib -mmacosx-version-min="$MACOSX_DEPLOYMENT_TARGET" > ../openssl.log 2>&1 &&
		  make -j"$JOBS" build_libs >> ../openssl.log 2>&1 &&
		  make install_dev >> ../openssl.log 2>&1 ) || { tail -40 "$bld/openssl.log"; exit 1; }
	fi

	echo "[$arch] FreeType"
	cmake_build "$WORK/src/freetype" "$bld/freetype" "$prefix" "$arch" \
		-DFT_DISABLE_BZIP2=ON -DFT_DISABLE_PNG=ON -DFT_DISABLE_HARFBUZZ=ON -DFT_DISABLE_BROTLI=ON \
		-DFT_REQUIRE_ZLIB=ON

	echo "[$arch] libpng"
	cmake_build "$WORK/src/libpng" "$bld/libpng" "$prefix" "$arch" \
		-DPNG_SHARED=OFF -DPNG_STATIC=ON -DPNG_TESTS=OFF -DPNG_TOOLS=OFF -DPNG_FRAMEWORK=OFF \
		-DPNG_HARDWARE_OPTIMIZATIONS=ON

	echo "[$arch] libjpeg-turbo"
	simd=OFF
	if [ "$arch" = arm64 ] || command -v nasm > /dev/null; then simd=ON; fi
	cmake_build "$WORK/src/libjpeg" "$bld/libjpeg" "$prefix" "$arch" \
		-DENABLE_SHARED=OFF -DENABLE_STATIC=ON -DWITH_TURBOJPEG=OFF -DWITH_SIMD=$simd \
		-DCMAKE_SYSTEM_PROCESSOR="$arch"

	echo "[$arch] PoDoFo"
	cmake_build "$WORK/src/podofo" "$bld/podofo" "$prefix" "$arch" \
		-DPODOFO_BUILD_STATIC=ON -DPODOFO_BUILD_LIB_ONLY=ON \
		-DCMAKE_DISABLE_FIND_PACKAGE_TIFF=ON \
		-DCMAKE_DISABLE_FIND_PACKAGE_Fontconfig=ON \
		-DCMAKE_DISABLE_FIND_PACKAGE_LCMS2=ON \
		-DOPENSSL_ROOT_DIR="$prefix" -DOPENSSL_USE_STATIC_LIBS=ON \
		-DFREETYPE_LIBRARY="$prefix/lib/libfreetype.a" -DFREETYPE_INCLUDE_DIRS="$prefix/include/freetype2" \
		-DPNG_PNG_INCLUDE_DIR="$prefix/include" -DPNG_LIBRARY="$prefix/lib/libpng16.a" \
		-DJPEG_INCLUDE_DIR="$prefix/include" -DJPEG_LIBRARY="$prefix/lib/libjpeg.a" \
		-DZLIB_INCLUDE_DIR="$SDK/usr/include" -DZLIB_LIBRARY="$SDK/usr/lib/libz.tbd" \
		-DLIBXML2_INCLUDE_DIR="$SDK/usr/include/libxml2" -DLIBXML2_LIBRARY="$SDK/usr/lib/libxml2.tbd"
done

echo "Creating universal libraries in $OUT"
rm -rf "$OUT"
mkdir -p "$OUT/lib" "$OUT/include"
first=$(echo $ARCHS | cut -d' ' -f1)
for lib in "$WORK/prefix-$first"/lib/*.a; do
	[ -L "$lib" ] && continue		# libpng.a -> libpng16.a
	name=$(basename "$lib")
	inputs=()
	for arch in $ARCHS; do inputs+=("$WORK/prefix-$arch/lib/$name"); done
	lipo -create "${inputs[@]}" -output "$OUT/lib/$name"
done
cp -R "$WORK/prefix-$first/include/podofo" "$OUT/include/"

# a header that differs per architecture would need an #if __arm64__ wrapper
for arch in $ARCHS; do
	[ "$arch" = "$first" ] && continue
	diff -rq "$WORK/prefix-$first/include/podofo" "$WORK/prefix-$arch/include/podofo" \
		|| { echo "error: PoDoFo headers differ between $first and $arch"; exit 1; }
done

[ "${KEEP_BUILD:-0}" = 1 ] || rm -rf "$WORK"

ls -l "$OUT/lib"
echo "Done."
