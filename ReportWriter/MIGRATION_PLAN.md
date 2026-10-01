# ReportWriter: TinyXML → pugixml, UTF-16 strings, JSON export

## Decisions

| Topic | Decision |
|---|---|
| Internal text type | `RWString` = `std::u16string` (UTF-16, same as 4D `PA_Unichar`, CoreFoundation `UniChar`, Win32 `wchar_t`) |
| XML library | pugixml 1.16 in `wchar_t` mode (`PUGIXML_WCHAR_MODE` in `pugixml/pugiconfig.hpp`) |
| XML access | only through `RW/RWXml.h` (`RWXmlDocument`, `RWXmlNode`); it converts `wchar_t` ↔ `RWString` (copy on Windows, UTF-32 ↔ UTF-16 on Mac) |
| JSON library | RapidJSON (commit 24b5e7a) with `UTF16<char16_t>` values, only through `RW/RWJson.h` |
| Writing XML | build the whole tree in memory, write it once (`SaveFile` / `SaveUTF8` / `SaveString`). No `FILE*` writers. |
| Files on disk | UTF-8 with declaration, tab indented; existing `.srxml` / `.rwxml` stay compatible |
| Parse options | `parse_default \| parse_ws_pcdata_single`: white-space-only text is dropped like TinyXML did, except when it is the only content (`<Data> </Data>`). Switch to `parse_ws_pcdata` only if the regression corpus needs it. |
| Numbers in text | `RWStr::ToInteger` / `ToDouble` / `FromDouble`: "C" locale always, prefix parsing like `sscanf` |
| Booleans in XML | written as `1` / `0`; read as number, `true/false`, `yes/no` |
| 4D boundary | `RW/RWString4D.h` (`FromPA`, `ToPA`, `CreatePA`, `SetPA`), pointer casts only |

Rules for new and ported code:
- Never include `pugixml.hpp` or `rapidjson/*` directly; use `RWXml.h` / `RWJson.h`.
- No `char*` text, `strcpy`, `sprintf`, `sscanf`, `atoi`, `strcasecmp`; use `RWString` + `RWStr::` helpers. `char` / `std::string` only for UTF-8 bytes at OS, file and BLOB boundaries.
- Source files are UTF-8 (MSVC needs `/utf-8` for `u"…"` literals with non-ASCII characters).
- Third-party headers get their own non-recursive search path. The existing recursive `include/**` path exposed RapidJSON's MSVC `msinttypes/stdint.h` to the whole project, so RapidJSON lives in `rapidjson/include`.

## Phases

- [ ] **0. Baseline.** ~~Put the project under version control~~ (done: branch `pugixml-migration`, commit `c024911`). Collect a regression corpus of real `.srxml` / `.rwxml` files plus the exported XML/HTML/CSV they produce. Decide whether the Carbon composer (`RWMacPageComposer`, ATSUI, `PMPrintSession`) and `DMArea` Carbon code stay in the target. They are a separate porting problem.
- [x] **1. Libraries.** `pugixml/` (compiled into the target) and `rapidjson/include/` (header-only). Header search paths added in all three Xcode configurations.
- [x] **2. Foundation.** `RW/RWString.{h,cpp}`, `RW/RWString4D.h`, `RW/RWXml.{h,cpp}`, `RW/RWJson.h`. Tests in `tests/RWFoundationTests.cpp`; run `tests/run_tests.sh` (ASan + UBSan, macOS 11 deployment target).
- [x] **3. Core types (`RW/RWBaseTypes`).** Done; covered by `tests/RWBaseTypesTests.cpp`.
  - `RWValue`: text is an `RWString` member outside the union (it used to be a `std::string` / `std::u16string` inside an anonymous union, never constructed or destroyed properly). `eValue_XMLText` is gone, and `eValue_Text` stays 5. Picture references are `void*` (they were stored in `long`, which truncates on 64-bit Windows). `IsEmpty` is defined for every kind.
  - `SPoint` / `SRect` / `SRGBColor`: `operator = (RWStringView)` and `ToString()` replace `const char*` conversions, which returned static or dangling buffers.
  - `RWTools`: `ParseIntoText`, `WriteText`, `ReadData`, `WriteData` take `RWXmlNode`; `FILE*` overloads removed; Base64 via `RWStr::Base64Encode/Decode` (the old writer put the whole encoded BLOB in a stack VLA). `SplitAttributedString` returns its attributes in a `std::vector<long>` (was `unique_ptr<long>` over `operator new` memory).
  - `RWString` gained `Format` (C locale printf), `EscapeXML`, `Base64Encode/Decode`.
  - `RWBaseTypes.h` now defines `MACVER` from `VERSIONMAC`. It used to come from an unused prefix header, so every `#if MACVER` compiled its Windows branch on the Mac. `USE_MAC_TYPES` is explicitly 0 (it never was on, see phase 0).
  - **Transitional shims** (remove in phase 8): `CText` / `CXMLText` = `RWString`, `CChar`, `TEXT_*` / `STR_*` helpers, `RWTextValue` (an `RWString` with the old method names), `RWValue::SetXMLText/GetXMLText`, `#include "tinyxml2.h"` + `using namespace tinyxml2`.
  - **Behaviour changes**:
    - A CR in element text is written as `<NL/>`. XML parsers turn a raw CR into LF, so 4D line breaks were lost on every save/load.
    - `<SPAN STYLE>` `font-weight`, `font-style` and `text-decoration` now apply; the old code compared the property name instead of its value.
    - Entities in attributed text: `&#xHH;` is decoded, and named entities no longer skip twice their length.
    - `ParseTextForVar` clears the format of a variable without one (the previous variable's format leaked), ignores unterminated `<%…`, and no longer skips 2 characters after an empty `<%%>`.
    - Colours are written as `#aarrggbb`; the old writer produced an empty string.
    - Text → integer coercion also understands `#colour` for all text (it used to be UTF-8 text only).
    - Numbers are always formatted and parsed in the C locale.
  - Windows-only code in `RWValue::Clone` (`Gdiplus::Image::Clone`) is not compiled or tested yet (phase 9).

  Per-file compile errors after phase 3 (C++ syntax check, excluding the Cocoa noise in `DMArea` / `RWMacPageComposer`). These are the work lists for phases 4–7; almost all come from TinyXML types, `char*` text and `UniChar*` ↔ `char16_t*` at the 4D boundary:
  RW 526 (RWCTPageComposer 163, RWObject 60, RWTable 58, RWReportWriter 39, RWSection 37, RWReportData 31, RWPageComposer 30, RWDataProvider 28), SRP 340 (SRObject 72, SRTable 51, SRReportData 29, SRSection 22, ExtendedExecute 22, SRReportWriter 20), ET 101, DM 285 (DMReport 100, DMObject 77, PSObject 46).
- [x] **4. RW module.** Every RW file in the target compiles, plus `DM/PSObject` (the property engine RW styles, SRP and DM objects all share). Covered by `tests/RWBaseTypesTests.cpp` (data provider round trip).
  - XML reading uses `RWXmlNode`. Attribute loops became `for (auto &[name, value] : node.Attributes())`, and `sscanf` became `RWStr::ReadNumber`, which also leaves the variable unchanged on failure.
  - Virtual signatures changed across modules, with `override` on every derived declaration so a mismatch is a compile error: `ParseReport` (data sources incl. `SRDataSource`, page composers incl. Windows), and `LoadXML` / `WriteXML` / `LoadXMLObjects` / `WriteXMLObjects` (all `PSObject` descendants in DM and SRP). The DM / SRP bodies are ported in their phases.
  - `RWDataProvider` writes and reads the unchanged `<Objects>` / `<Tables>` format; `FILE*` writers deleted; kind 4 (old UTF-8 text) is read as text.
  - Mac: `RWStringCF.h` (`RWString` ↔ `CFString`). `RWMacPageComposer.cpp` and `DMArea.cpp` import AppKit, so their Xcode file type is now Objective-C++ (they could not compile as C++). `RWMacCGPageComposer.cpp`, the base class of the CoreText composer, was missing from the target and is added. `RWMacPageComposer.h` includes ApplicationServices (PrintCore, ATSUI, CoreText). The `TextEncoding` typedef in `RWBaseTypes.h` is Windows-only now (it clashed with CoreServices).
  - Legacy Mac Roman sources converted to UTF-8 / LF in separate commits (no code changes).
  - **Bugs fixed** (all from the earlier partial port unless noted):
    - `RWStyle`: every style's font was replaced by the default (`!mFontName.empty()`); `operator ==` reported styles equal only when the fonts differed; `GetFName` returned `NULL` into a string.
    - `RWPageComposer::ParseReport`: a report's `Size` was always replaced by A4; `usePhysical` was a pointer assigned to `bool` (always true).
    - `RWObject::Parse`: the `r` (position) attribute was read with the wrong order and separator, so only one coordinate was set.
    - `std::basic_string (str, n)` means "from position n", not "first n characters": text-variable substitution in `RWText` produced empty text. Same pattern still in SRP / DM / ET: `SRObject.cpp:1994,2022,2060`, `SRTable.cpp:463`, `DMObject.cpp:4564,4596`, `ETObject.cpp:572,579`.
    - `RWDataProvider::Parse` iterated an uninitialised pointer and skipped exactly the `<v>` cells; `WriteValue` wrote the empty string for numbers and vice versa.
    - `RWDataProvider::tableData`: `--end()` on an empty column (original code), NULL column, `new [-1]` when `cols` is missing.
    - `RWObject.h`: position / order comparators had lost their `template` line, so `std::sort` of objects did not compile.
    - `RWMacPageComposer::ParseReport` dereferenced a missing `PageFormat` / `PrintSettings` child.
    - `RWTable::ParseHeading` counted comment nodes as heading rows.
  - `RWPDFPageComposer.cpp` (PDFlib, unused) and `RWPoDoFoPageComposer.cpp` (PoDoFo 0.9) do not compile; decided in phase 7b.
  Per-file compile errors after phase 4 (the next phases' work lists): RW 24, all in files outside the target or needing missing libraries (RWDemoDataSource 11, RWPaper 9, RWll / RWPDFPageComposer / RWPoDoFoPageComposer / RWWinPageComposer 1 each). SRP 368 (RW4DText 85, SRObject 84, SRTable 60, SRReportData 26, SRDataSource 24, SRSection 23, ExtendedExecute 22, SRDataFormatter 17, SR4DData 15, SRReportWriter 6). DM 289 (DMReport 128, DMObject 82, DMArea 54 as Objective-C++, DMUndo 15, UIScrollBar 10). ET 59.
- [x] **5. SRP module.** Every SRP file compiles; `SRPlugin.cpp`'s only remaining errors come from DM headers (phase 7).
  - All `FILE*` writers deleted (about 900 lines); the tree writers use `RWXmlNode`. `RWXmlNode::SetAttribute` is a typed setter that writes `bool` as `1` / `0`. tinyxml2 wrote `true`, which the numeric readers parse as 0, so e.g. header `firstPage` / `lastPage` settings were lost.
  - `SRReportWriter::ReportToXML()` returns the document it builds (`std::unique_ptr<RWXmlDocument>`); the old version always returned NULL.
  - `SRPlugin.cpp` was still the original pre-port code (old `XF::CText` toolbox). 4D parameters go through `RWString4D.h`. File paths go through `RWStr::NativePath`: HFS to POSIX on the Mac, like the original `UString::GetFSName`, native UTF-16 on Windows (the original converted to the ANSI code page). Reports are loaded by one `LoadSourceXML` helper (a file path or XML text straight from 4D).
  - Interfaces declared for later phases: `ETReportData (RWXmlDocument*)`, `bool ETReport::ReportToFile (path)`, `DMReport (RWXmlDocument*)`, `SetReport` / `GetReport` / `CreateObject` with `RWXmlDocument` / `RWXmlNode`.
  - `SRDataFormatter 2.cpp` (a naive rewrite that lost the boolean formats and duplicated the symbol) removed.
  - **Fixes**: empty text after variable substitution and XLIFF localization (`basic_string (str, pos)`), `RW4DText::RemoveSpan` not shortening split spans, script callback passing a truncated 64-bit pointer (now the internal ID via `PA_ExecuteMethodByID`), `RW_SetLicense` declared and defined with different types, NULL base style in the style export, `auto_ptr` over `new[]`, null dereference in `RW_NewObject`.
  - **Not functional on the Mac, decision postponed (2026-10-01)**: `RW_ColorPicker` used Carbon's `NPickColor`, which does not exist in 64 bit; it now returns "no color chosen". Replace with `NSColorPanel` (Objective-C++) or 4D's `Select RGB color`. Windows is unchanged.
  - Observation: license validation is commented out in `SRLicense.cpp`; any non-empty license string is accepted.

- [ ] **6. ET module + JSON.**
  - `ETReportData` takes the SR document directly (no print + re-parse).
  - Output layer: XML into an `RWXmlDocument`, HTML / text / CSV into an `RWString`, new `eo_json = 0x8000` into an `RWJsonDocument` (or streaming `RWJsonUTF8Writer` if exports get very large). Write once as UTF-8.
  - 4D command / flag to request JSON.
- [ ] **7. DM module.** `DMReport`, `DMObject`, `PSObject`, `DMUndo`. Property tables (`PSObjProps`) keep ASCII names. Add `std::string_view` name overloads to `RWXmlNode` if the per-call conversion shows up in profiles.
- [ ] **7b. PDF output (decision 2026-10-01).**
  - PDF files are produced by one PoDoFo 1.0 composer on macOS **and** Windows, in 4D desktop and 4D Server. The output is identical on both platforms and does not depend on installed printers. Printing and on-screen preview stay native (CoreText / GDI+).
  - Windows preview uses "Microsoft Print to PDF" instead of "Microsoft XPS Document Writer" (`RWPageComposer::CreatePrinterComposer`).
  - Reports use standard paper sizes and PDF file size is not a concern (full font embedding is acceptable).
  - Rewrite `RWPoDoFoPageComposer` (written for PoDoFo 0.9) against the 1.0 API; font lookup and embedding per platform (CoreText on the Mac, GDI font data on Windows), so text measurement and output use the same metrics.
  - Build PoDoFo and its dependencies (FreeType, zlib, OpenSSL, image libraries; confirm against the 1.0 release) for macOS arm64 + x86_64 and Windows x64. Check the licence (LGPL) against the distribution model (dynamic linking or relinkable objects).
  - Remove the PDFlib composer (`RWPDFPageComposer`, `RWll`); nothing creates it.
- [ ] **8. Removal.** `tinyXML/` (incl. `xmltest.cpp`, which has its own `main`), `SRP/SRDataFormatter 2.cpp`, jsoncpp (`include/json`, `a/libjsoncpp.a`, `lib/windows*/jsoncpp.lib`, `support/4DPlugin-JSON.*`, which nothing includes).
- [ ] **9. Plugin shell.** Replace the wizard stub (`4DPlugin-ReportWriter.cpp`, `manifest.json`) with the real command table from `SRPlugin.cpp`. Update `ReportWriter.vcxproj`: sources, include paths `pugixml;rapidjson/include`, C++17, `/utf-8`.
- [ ] **10. Verification.** Corpus round trip (load → save → diff, whitespace-normalised) against the TinyXML build output; 4D test database on macOS arm64 + x86_64 and Windows x64.

## Reference: the original code

`../../new SRP current_16` (next to this project) is the original code base before the partial port: TinyXML 1, the `XF::CText` / `UString` toolbox (`SourceLib/Framework`), `Resources/theVersion.h` (referenced by the Xcode project from there), and `lib_paint/RWll.h`, the header `RWPDFPageComposer` needs. PoDoFo sources are in `../../podofo`. Use it to check what code did before the partial port; `ReportWriter.zip` (Dec 2023) already contains the partial port.

## Known defects in the current code (fixed by the phases above)

- `RW/RWBaseTypes.cpp:263`: `UTF_16_to_UTF8` never advances its iterator (endless loop); both UTF helpers are free functions, not the declared `RWTextValue::` statics; `UTF_8_to_UTF16` reads past the end and uses an uninitialised code point.
- `RW/RWBaseTypes.cpp:1060`: `result.append ((UniChar *) '\r')`, a character cast to a pointer.
- `RW/RWBaseTypes.h:383/390`: `ToXMLEscaped` appends the rest of the string per character; `return NULL` into `std::string`.
- `RW/RWBaseTypes.cpp:232`: `snprintf (buf, sizeof (buf[0]), …)` writes an empty colour string.
- `SRP/SRReportWriter.cpp:389`: `ReportToXML` always returns `NULL` (and leaks), so ET export and the `.rwxml` dump never run.
- `SRP/SRPlugin.cpp:745`: `strncat (s3, ".rwxml", sizeof (s3))` can overflow.
- Fixed in phase 3: the `RWBaseTypes` items above, plus the `RWValue` union, `SRGBColor::operator char*` (returned a stack buffer), `GetEntity`, SPAN style values, Base64 stack VLA, missing `MACVER`.
- `CText` = `std::basic_string<unsigned short>` relies on `char_traits<unsigned short>`, deprecated in current libc++ and scheduled for removal.
