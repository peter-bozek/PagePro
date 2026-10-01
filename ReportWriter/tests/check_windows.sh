#!/bin/bash
# Compiles and links the Windows plugin with llvm-mingw on a Mac (no Visual Studio needed),
# with the defines of ReportWriter.vcxproj (Debug and Release). Catches Windows-only
# compile errors and missing symbols; PoDoFo is not linked (podofo/build_win.ps1 builds
# it for Visual Studio), so only its symbols may remain undefined.
#
#   tests/check_windows.sh <llvm-mingw directory> [build directory]
#
# llvm-mingw: https://github.com/mstorsjo/llvm-mingw/releases (…-ucrt-macos-universal.tar.xz)
set -uo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TOOLS="${1:?usage: tests/check_windows.sh <llvm-mingw directory> [build directory]}/bin"
OUT="${2:-${TMPDIR:-/tmp}/reportwriter-windows}"
CXX="$TOOLS/x86_64-w64-mingw32-clang++"
cd "$ROOT"

SOURCES=$(ls DM/*.cpp SRP/*.cpp RW/*.cpp ET/*.cpp | grep -v -e RWCTPageComposer -e RWMacCGPageComposer -e RWMacPageComposer -e RWFontsMac -e SRGetFontsCG)
SOURCES="$SOURCES pugixml/pugixml.cpp"
status=0

for cfg in Debug Release; do
	obj="$OUT/$cfg"
	mkdir -p "$obj"
	find "$obj" -name "*.o" -delete
	if [ $cfg = Debug ]; then defs="-D_DEBUG"; else defs="-DNDEBUG"; fi
	for f in $SOURCES "4D Plugin API/4DPluginAPI.c"; do
		o="$obj/$(basename "$f" | sed 's/\.[a-z]*$/.o/')"
		"$CXX" -x c++ -std=c++17 -w -fms-extensions -DWIN32 -DWIN64 -D_WINDOWS -D_USRDLL -DUNICODE -D_UNICODE -DPODOFO_STATIC $defs \
			-include RW/RWWinPrefix.h -I RW -I DM -I SRP -I ET -I . -I pugixml -I rapidjson/include -I "4D Plugin API" -I podofo/mac/include \
			-c "$f" -o "$o" 2> "$obj/errors.txt" || { echo "[$cfg] compile error in $f"; grep " error" "$obj/errors.txt" | head -10; status=1; }
	done
	missing=$("$CXX" -shared -o "$obj/ReportWriter.4DX" "$obj"/*.o -lgdiplus -lcomdlg32 -lwinspool -lshell32 -lole32 -lwinmm -lgdi32 -luser32 2>&1 \
		| grep -o "undefined symbol: .*" | sort -u | grep -v PoDoFo)
	if [ -n "$missing" ]; then echo "[$cfg] undefined symbols:"; echo "$missing"; status=1; else echo "[$cfg] compiles and links (PoDoFo excluded)"; fi
done
exit $status
