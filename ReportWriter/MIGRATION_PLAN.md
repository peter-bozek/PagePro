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
  - **Transitional shims**: `CText` / `CXMLText` = `RWString`, `CChar`, `TEXT_*` / `STR_*` helpers, `RWTextValue` (an `RWString` with the old method names), `RWValue::SetXMLText/GetXMLText`, `#include "tinyxml2.h"` + `using namespace tinyxml2`. All removed in phase 8.
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
  - **Removed (decision 2026-10-01)**: `RW_ColorPicker`. It predates 4D's own `Select RGB color`; the Mac version used Carbon's `NPickColor`, which does not exist in 64 bit. The command slot stays reserved (`eColorPicker_Removed`) so later command numbers do not change; the phase 9 manifest needs a placeholder entry at that position.
  - Observation: license validation is commented out in `SRLicense.cpp`; any non-empty license string is accepted.

- [x] **6. ET module + JSON.** ET compiles and is covered by `tests/ETExportTests.cpp` (a processed report exported to all formats).
  - `ETReportData` reads the SR document directly (`RWXmlDocument*`); no print and re-parse.
  - Output is built in memory and written once as UTF-8 by `bool ETReport::ReportToFile (path)`: text and HTML in an `RWString`, XML in an `RWXmlDocument` (same elements and attributes as before, `<?xml … encoding="utf-8" standalone="yes"?>`), JSON in an `RWJsonDocument`. Files are written with `RWStr::WriteFile` (`std::filesystem`, Unicode paths on both platforms).
  - New flag `eo_json = 0x8000`; format precedence text, HTML, XML, JSON (as before, JSON added last). JSON layout: `{ "version", "name", "sections": [ { "type", "id", "items": [ { "type": "text" | "variable", "name", "id", "class", "value", "styled" } ] } ] }`; `value` is plain text, `styled` holds the 4D markup when the text is attributed.
  - HTML escaping via `RWStr::EscapeXML` (the old `ToXMLEscaped` duplicated text).
  - **Fixes**:
    - `RW_VarNamesRWCount` was `RW_VarName + 1`, but `HORPAGE` / `HORPAGES` were added after it: `RWInitReportVariable` wrote two strings past `mVarNames` in `RWReportWriter` and `ETReport` (memory corruption on every print and export), and SRP counted the two as SR-only variables. Now counted, with a `static_assert`.
    - Exports never contained the report name (`ETReport::mName` was never set).
    - Attributed text in variables was entity-*decoded* instead of encoded (tinyxml2 `StrPair` misuse); same fix as RW / SRP.
    - `basic_string (str, pos)` misuse in `ETObject` text substitution.

- [x] **7. DM module.** Every DM file compiles (`DMArea.cpp` as Objective-C++); with SRP, RW and ET this leaves only files outside the target (`RW/main.cpp`, `RWDemoDataSource`, `RWPaper`, `RWll`: phase 8), the PDF composers (phase 7b) and `RWWinPageComposer` (Windows, phase 9).
  - Property tables (`PSObjProps`) keep ASCII names. Possible later optimization: `std::string_view` name overloads on `RWXmlNode` if the per-call conversion shows up in profiles.
  - Undo snapshots (`DMUndo`) are UTF-16 XML strings (`RWXmlDocument::SaveString` / `LoadString`) instead of `strdup`ed UTF-8.
  - `DMReport::GetReport (RWXmlDocument&)` adds `<?xml version="1.0" encoding="utf-8" standalone="yes"?>`, as the TinyXML 1 code did. Built-in styles are UTF-16 literals parsed with `LoadString`.
  - Overrides that renamed the element written by the base class (`td`, `Col`, section type, `Vertical` / `Horizontal`) use the new `RWXmlNode::SetName`.
  - Section factories take `RWStringView inType` (empty means "use the element name"); `DMSection::mType` is an `RWString` (was a `strdup`ed pointer).
  - `UIScrollBar` (Mac) no longer uses the 32-bit-only Carbon control API. 64-bit scroll bars were already no-ops and still are. `DMArea.cpp` includes `<Carbon/Carbon.h>` itself for the HIToolbox calls that still exist in 64 bit (key modifier constants, `GetCurrentKeyModifiers`, `HIThemeDrawFocusRect`), since `RWBaseTypes.h` no longer pulls in Carbon.
  - **Fixes**: `auto_ptr<SRect>` over raw `operator new` storage replaced by `std::vector<SRect>`; `DMText::ParseData` had the same empty-result `basic_string (str, pos)` bug as SRP; the table row measurement string was mis-decoded Mac Roman (`"ROW √ög"`), now `u"ROW Úg"`; `DMVariable` compared its script object to `NULL`, now `!mScript.IsEmpty()`.
- [x] **7b. PDF output (decision 2026-10-01).** PoDoFo 1.0.4 composer on macOS and Windows; covered on the Mac by `tests/PdfComposerTests.cpp` (35 checks: text in several styles, shapes, a JPEG and a PNG with alpha, two pages; the file is reloaded with PoDoFo and checked).
  - Decision: PDF files are produced by one PoDoFo 1.0 composer on macOS **and** Windows, in 4D desktop and 4D Server, independent of installed printers. Printing and on-screen preview stay native (CoreText / GDI+). `RWPageComposer::CreatePrinterComposer` returns `RWPoDoFoPageComposer` for `eDestinationPDF` with a destination path on both platforms (the Mac used the CoreText composer before).
  - Windows preview prints to "Microsoft Print to PDF" (was "Microsoft XPS Document Writer"), port `PORTPROMPT:` with the file from `DOCINFO::lpszOutput`, default file `RW_Preview.pdf`; without that printer the preview is a PoDoFo PDF. Untested until the phase 9 Windows build.
  - **Libraries**: `podofo/build_mac.sh` builds PoDoFo 1.0.4, FreeType, OpenSSL 3.5 LTS, libpng and libjpeg-turbo from tagged sources as static universal (arm64 + x86_64) libraries in `podofo/mac` (zlib, libxml2 from the SDK); `podofo/build_win.ps1` does the same for x64 with vcpkg dependencies (written, not run yet). Build products are not committed (`podofo/.gitignore`, 51 MB on the Mac). The Xcode project has the include / library paths, `PODOFO_STATIC` and the link flags. See `podofo/README.md`.
  - **Licence (decision 2026-10-01)**: PoDoFo stays statically linked; the plugin's distribution is adjusted to the LGPL (recipients can relink with a modified PoDoFo).
  - **Fonts** (`RWPdfFonts`): the PDF uses the font the native composer uses for a style. The platform part copies its sfnt tables (`RWFontsMac.cpp`: CoreText, via the font selection shared with `RWCTPageComposer` in `RWCreateCTFont`; `RWFontsWin.cpp`: GDI `GetFontData`), the common part rebuilds a standalone TrueType / OpenType file (also works for faces from `.ttc` collections) that PoDoFo embeds as a **subset** (TrueType and CFF). `ePDFDontEmbedFonts` still skips embedding. The standard 14 fonts are no longer used unembedded (WinAnsi cannot encode Central European letters); Helvetica remains the last fallback.
    - Missing bold / italic faces are synthesized (outline stroke, 0.2 skew), detected from `OS/2` / `head` style bits.
    - Line metrics come from `hhea` like CoreText (PoDoFo's own ascent is up to 20 % smaller), so line breaks and baselines match the screen and printed output; word widths are identical to CoreText.
    - No glyph fallback: characters missing in the font are not drawn (CoreText substitutes another font); logged by `MeasureWord`. Accepted (decision 2026-10-01): users know that not every font has every character; mention it in the user documentation.
  - **Pictures**: JPEG embedded unchanged (DCTDecode); PNG decoded and Flate compressed, alpha as a soft mask. `SR4DData::GetPictureFrom4D` for PDF now accepts JPEG and PNG and converts other formats to PNG (was JPEG: lossy, transparency lost). Content streams and fonts are Flate compressed; the test page with three subset fonts and two pictures is 35 KB.
  - The PDF is built in memory and written with `RWStr::WriteFile` (Unicode paths, HFS paths from 4D converted on the Mac). In a batch (`RW_OpenSession`) the document stays open until the session closes.
  - Removed: the PDFlib composer (`RWPDFPageComposer`, `RWll.cpp`) and the PoDoFo 0.9 code (memory `FILE*` adapter for MSL / `funopen`).
  - **Fix**: `RWPageComposer::DrawTextBox` cast every composer to `RWMacPageComposer` on the Mac to call `GetGContext` / `ReleaseGContext` (undefined behaviour for any other composer, i.e. PDF output); now a checked `dynamic_cast`.
  - **Fix**: `RWTFPrintText::Draw` clipped each text box at its top edge with the first baseline at top + ascent, so with fonts whose `hhea` ascent is small (Helvetica: 0.77 em) the accents of capitals on the first line (Ž, Š, Č, Ď) were cut, in the PDF and the native Mac output alike. The clip now leaves 35 % of the first line's ascent above the box; the layout is unchanged.
  - `theVersion.h` copied into the repository (project root) from `new SRP current_16/Resources`; the Xcode project referenced it there.

- [x] **8. Removal.** Every file in the target compiles without TinyXML and without the transitional names; all four test programs pass.
  - Deleted: `tinyXML/` (incl. `xmltest.cpp`, which was in the target's sources with its own `main`), jsoncpp (`include/json`, `a/libjsoncpp.a`, `lib/windows*/jsoncpp.lib`, `support/4DPlugin-JSON.*`), and the files outside the target (`RW/main.cpp`, `RWDemoDataSource`, `RWPaper`). The Xcode project no longer searches `include/**`, `lib`, `a`.
  - Shims removed: `CText` / `CXMLText` / `CChar` → `RWString` / `char16_t`; `RWTextValue` → `RWString` (its old methods replaced at the positions the compiler reported: `IsEmpty` → `empty`, `Free` → `clear`, `StrLength` → `size`, `Attach (x.Detach())` → `std::exchange (x, RWString())`, `Copy` / `FromXML` → assignment); `STR_EQUALS` / `TEXT_EQUALS` / `STR_STARTS_WITH` → `RWStr::EqualsNoCase` / `Equals` / `StartsWithNoCase` (new ASCII overloads for `StartsWith`, `StartsWithNoCase`, `CompareNoCase`); `TEXT_STR (..) == STR_NOTFOUND` → `!RWStr::Contains`; `SetXMLText` → `SetText`; the `#if CChar_Size == 1` branches.
  - Dead code using removed APIs: 7 `#if 0` blocks and the commented-out TinyXML declarations. Kept: the `#if 0` licence validation in `SRLicense.cpp` (may be re-enabled).
  - Windows-only branches checked by a scan for the removed names (they do not compile on the Mac); only `RWWinPageComposer` still uses the old string class — ported in phase 9.
  - **Fix** (Windows): reading the Mac page format of a report on Windows (`DMReport::SetProperty (PSObjPropPageFormat)`) looped forever on an unknown key in the ticket dictionary and read the `PMRect` arrays as x / y / width / height instead of top / left / bottom / right, so the page size was wrong. Now `RWTools::ParseMacPageFormat` (RWXml, compiled and tested on both platforms).
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
