#!/bin/bash
# Runs the ReportWriter 4D tests headless (phase 10).
#
#   tests/4D/run_4d_tests.sh <plugin bundle> <arm64|x86_64> <output folder> [method] [4D.app]
#
# The project is copied to <output folder>/project with the plugin in its Plugins folder;
# results are written to <output folder>/results. The old plugin (x86_64 only) needs x86_64.
set -uo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
BUNDLE="${1:?plugin bundle}"
ARCH="${2:?arm64 or x86_64}"
OUT="${3:?output folder}"
METHOD="${4:-RWT_Run}"
APP="${5:-/Applications/4D 20.9/4D.app}"
TIMEOUT="${TIMEOUT:-600}"

mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"
rm -rf "${OUT:?}/project" "${OUT:?}/results"
mkdir -p "$OUT/results"
cp -R "$ROOT/RWTest" "$OUT/project"
mkdir -p "$OUT/project/Plugins"
cp -R "$BUNDLE" "$OUT/project/Plugins/"

"/usr/bin/arch" "-$ARCH" "$APP/Contents/MacOS/4D" --project "$OUT/project/Project/RWTest.4DProject" \
	--opening-mode interpreted --headless --dataless --startup-method "$METHOD" \
	--user-param "$OUT/results/" > "$OUT/4d_stdout.txt" 2>&1 &
PID=$!
for ((i = 0; i < TIMEOUT; i++)); do
	kill -0 $PID 2> /dev/null || break
	sleep 1
done
if kill -0 $PID 2> /dev/null; then
	echo "4D did not quit within ${TIMEOUT}s - killed"
	kill -9 $PID
	exit 2
fi
wait $PID
echo "4D exit code $? (stdout: $OUT/4d_stdout.txt)"
cp "$OUT"/project/Data/Logs/4DDiagnosticLog*.txt "$OUT/results/" 2> /dev/null
ls "$OUT/results"
