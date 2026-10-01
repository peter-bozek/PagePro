# 4D tests (phase 10)

A headless 4D project (`RWTest`) that drives the plugin through its commands and writes every
result to files; `compare.py` compares two runs, e.g. the production plugin (TinyXML) against
the new one.

```sh
# the new plugin, native and under Rosetta
tests/4D/run_4d_tests.sh <build>/ReportWriter.bundle arm64  /tmp/rw/new-arm64
tests/4D/run_4d_tests.sh <build>/ReportWriter.bundle x86_64 /tmp/rw/new-x86
# baseline: the production plugin (x86_64 only, 1.6.0b10)
tests/4D/run_4d_tests.sh <path>/RW.bundle x86_64 /tmp/rw/old

swiftc -O -o /tmp/rw/pdftext tests/4D/pdftext.swift
tests/4D/compare.py /tmp/rw/old/results /tmp/rw/new-arm64/results /tmp/rw/pdftext
```

`run_4d_tests.sh <bundle> <arch> <output folder> [method] [4D.app]` copies the project to the
output folder with the plugin in `Plugins`, runs 4D (default `/Applications/4D 20.9`) with
`--headless --dataless --startup-method RWT_Run` and a timeout (`TIMEOUT`, seconds). Results:
`results/results.txt` (one `name = value` line per check), the report XML / JSON, processed
report, exports and PDFs, `log.txt`, and 4D's diagnostic log (4D errors are logged, they do
not stop the run).

What `RWT_Run` covers: building a report through the editor API (sections, text, variable,
shapes, group, properties), reading it back (`RW_GetProperties`, `RW_GetObjectXML`,
`RW_FindObjectByID`, `RW_GetObjects`), XML round trips (text and files, also in a folder with
non-ASCII characters), styled text commands, processing (`RW_Process_RW`) with 4D variables,
text / HTML / XML / JSON exports, PDF output (single report, from `.rwxml`, a session with two
reports), the JSON and 4D object commands, deleting objects.

Commands only the new plugin has are called with `EXECUTE FORMULA` (their result lines end in
`(new)`), so the same test runs with older plugins. The delete step runs last: the production
plugin crashes there (NULL area in `DMReport::RemoveObject`, fixed).

Not covered (needs a person in front of 4D): the editor area (`%ReportWriter`: drawing, mouse,
keyboard, undo / redo, `RW_GetEditorRect`), printing to a printer, print preview, page / job
setup dialogs (`RW_PrintSettings`).
