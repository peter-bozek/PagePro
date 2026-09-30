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
