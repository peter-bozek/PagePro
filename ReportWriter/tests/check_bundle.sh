#!/bin/bash
# Checks a built plugin bundle before it goes to 4D:
#   - the executable is universal (arm64 + x86_64)
#   - each architecture exports FourDPackex (4D's entry point), and nothing else
#   - Info.plist names the executable, manifest.json is in Resources and parses
#   - code signature, extended attributes (codesign rejects Finder info)
#
#   tests/check_bundle.sh [path/to/ReportWriter.bundle]
set -u

BUNDLE="${1:-$(cd "$(dirname "$0")/.." && pwd)/test/Plugins/ReportWriter.bundle}"
problems=0
ok ()   { echo "  ok    $*"; }
fail () { echo "  FAIL  $*"; problems=$((problems + 1)); }

echo "$BUNDLE"
if [ ! -f "$BUNDLE/Contents/Info.plist" ]; then
	fail "no Contents/Info.plist - not a built bundle"
	exit 1
fi
EXEC_NAME=$(/usr/libexec/PlistBuddy -c "Print :CFBundleExecutable" "$BUNDLE/Contents/Info.plist" 2> /dev/null)
EXEC="$BUNDLE/Contents/MacOS/$EXEC_NAME"
if [ -z "$EXEC_NAME" ] || [ ! -f "$EXEC" ]; then
	fail "Info.plist CFBundleExecutable '$EXEC_NAME' does not name a file in Contents/MacOS"
	exit 1
fi
ok "executable $EXEC_NAME (built $(stat -f %Sm "$EXEC"))"

ARCHS=$(lipo -archs "$EXEC" 2> /dev/null)
for arch in arm64 x86_64; do
	if [[ " $ARCHS " == *" $arch "* ]]; then
		ok "architecture $arch"
		exports=$(nm -gU -arch $arch "$EXEC" 2> /dev/null | awk '{print $3}')
		if [ "$exports" = "_FourDPackex" ]; then
			ok "  $arch exports _FourDPackex only"
		elif echo "$exports" | grep -qx "_FourDPackex"; then
			ok "  $arch exports _FourDPackex ($(echo "$exports" | wc -l | tr -d ' ') symbols in total)"
		else
			fail "  $arch does not export _FourDPackex"
		fi
	else
		fail "architecture $arch missing (has: $ARCHS) - Debug builds from the Xcode window build only the Mac's own architecture"
	fi
done

if [ -f "$BUNDLE/Contents/Resources/manifest.json" ] && python3 -m json.tool "$BUNDLE/Contents/Resources/manifest.json" > /dev/null 2>&1; then
	ok "manifest.json ($(python3 -c "import json,sys; print(len(json.load(open(sys.argv[1]))['commands']))" "$BUNDLE/Contents/Resources/manifest.json") commands)"
else
	fail "Contents/Resources/manifest.json missing or not valid JSON"
fi

if codesign --verify --deep --strict "$BUNDLE" 2> /dev/null; then
	ok "signature: $(codesign -dv "$BUNDLE" 2>&1 | grep -m1 '^Authority=' || echo 'ad hoc')"
else
	fail "not signed, or the signature does not verify (codesign --verify --deep --strict)"
fi

if [ -n "$(xattr -r "$BUNDLE" 2> /dev/null)" ]; then
	fail "extended attributes present (codesign refuses Finder info): xattr -cr \"$BUNDLE\""
	xattr -r "$BUNDLE" | sed 's/^/        /' | head -5
else
	ok "no extended attributes"
fi

[ $problems -eq 0 ] && echo "bundle OK" || echo "$problems problem(s)"
exit $problems
