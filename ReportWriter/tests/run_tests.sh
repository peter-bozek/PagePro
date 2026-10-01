#!/bin/bash
# Build and run the RWString / RWXml / RWJson tests with the plugin's compiler settings.
# Usage: tests/run_tests.sh [build-dir]
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${1:-${TMPDIR:-/tmp}/reportwriter-tests}"
mkdir -p "$OUT"

FLAGS=(-std=c++17 -mmacosx-version-min=11.0 -g
       -I "$ROOT/RW" -I "$ROOT/pugixml" -I "$ROOT/rapidjson/include" -I "$ROOT/4D Plugin API")

# third party code: no -Werror
clang++ "${FLAGS[@]}" -c "$ROOT/pugixml/pugixml.cpp" -o "$OUT/pugixml.o"

clang++ "${FLAGS[@]}" -Wall -Wextra -Werror -fsanitize=address,undefined \
	"$ROOT/RW/RWString.cpp" "$ROOT/RW/RWXml.cpp" "$ROOT/tests/RWFoundationTests.cpp" \
	"$OUT/pugixml.o" -o "$OUT/RWFoundationTests"

"$OUT/RWFoundationTests" "$OUT"

# RWBaseTypes core, linked with the 4D plugin API (never called)
clang -std=gnu99 -mmacosx-version-min=11.0 -g -w -Wno-int-conversion -I "$ROOT/4D Plugin API" -c "$ROOT/4D Plugin API/4DPluginAPI.c" -o "$OUT/4DPluginAPI.o"
clang++ "${FLAGS[@]}" -I "$ROOT/tinyXML" -I "$ROOT/DM" -I "$ROOT/SRP" -Wall -Wextra -Wno-comma -Wno-unused-value -fsanitize=address,undefined \
	"$ROOT/RW/RWBaseTypes.cpp" "$ROOT/RW/RWDataProvider.cpp" "$ROOT/SRP/RW4DText.cpp" "$ROOT/RW/RWString.cpp" "$ROOT/RW/RWXml.cpp" "$ROOT/tests/RWBaseTypesTests.cpp" \
	"$OUT/pugixml.o" "$OUT/4DPluginAPI.o" -framework CoreFoundation -framework CoreGraphics -o "$OUT/RWBaseTypesTests"

"$OUT/RWBaseTypesTests"

# export (ET): processed report to text, HTML, XML and JSON
clang++ "${FLAGS[@]}" -I "$ROOT/tinyXML" -I "$ROOT/DM" -I "$ROOT/SRP" -I "$ROOT/ET" -w -fsanitize=address,undefined \
	"$ROOT"/ET/*.cpp "$ROOT/RW/RWBaseTypes.cpp" "$ROOT/RW/RWString.cpp" "$ROOT/RW/RWXml.cpp" "$ROOT/RW/RWDataProvider.cpp" \
	"$ROOT/RW/RWDataSource.cpp" "$ROOT/RW/RWDataSourceProvider.cpp" "$ROOT/RW/RWCalculator.cpp" "$ROOT/RW/RWStyle.cpp" \
	"$ROOT/DM/PSObject.cpp" "$ROOT/SRP/ExtendedExecute.cpp" "$ROOT/tests/ETExportTests.cpp" \
	"$OUT/pugixml.o" "$OUT/4DPluginAPI.o" -framework CoreFoundation -framework CoreGraphics -o "$OUT/ETExportTests"

mkdir -p "$OUT/export"
"$OUT/ETExportTests" "$OUT/export"
