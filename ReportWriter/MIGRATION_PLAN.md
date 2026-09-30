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

- [ ] **0. Baseline.** Put the project under version control: only `ET/` and `en.lproj` are tracked in git today. Collect a regression corpus of real `.srxml` / `.rwxml` files plus the exported XML/HTML/CSV they produce. Decide whether the Carbon composer (`RWMacPageComposer`, ATSUI, `PMPrintSession`) and `DMArea` Carbon code stay in the target. They are a separate porting problem.
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
- [ ] **4. RW module.** `RWReportData`, `RWSection`, `RWStyle`, `RWObject`, `RWDataProvider` (delete `Write (FILE*)` / `WriteValue (FILE*)`), XML use in the page composers.
- [ ] **5. SRP module.**
  - Delete every `Write (FILE*)`, `WriteSelf (FILE*)`, `WriteSection (FILE*)`, `WriteSpecial (FILE*)`, `WritePage (FILE*)` and `SRDataSource::Write*/WriteReportData (FILE*)`. They are unreachable: `SRReportWriter::Report` already builds a tree.
  - Port the tree writers to `RWXmlNode`.
  - `SRReportWriter` returns an `RWXmlDocument` (`std::unique_ptr`). This fixes `ReportToXML`, which always returns `NULL` today.
  - `SRPlugin.cpp`: load / save via `RWXmlDocument` + `RWString4D.h`; `auto_ptr` → `unique_ptr`; no `fopen` / `strncat` path building.
- [ ] **6. ET module + JSON.**
  - `ETReportData` takes the SR document directly (no print + re-parse).
  - Output layer: XML into an `RWXmlDocument`, HTML / text / CSV into an `RWString`, new `eo_json = 0x8000` into an `RWJsonDocument` (or streaming `RWJsonUTF8Writer` if exports get very large). Write once as UTF-8.
  - 4D command / flag to request JSON.
- [ ] **7. DM module.** `DMReport`, `DMObject`, `PSObject`, `DMUndo`. Property tables (`PSObjProps`) keep ASCII names. Add `std::string_view` name overloads to `RWXmlNode` if the per-call conversion shows up in profiles.
- [ ] **8. Removal.** `tinyXML/` (incl. `xmltest.cpp`, which has its own `main`), `SRP/SRDataFormatter 2.cpp`, jsoncpp (`include/json`, `a/libjsoncpp.a`, `lib/windows*/jsoncpp.lib`, `support/4DPlugin-JSON.*`, which nothing includes).
- [ ] **9. Plugin shell.** Replace the wizard stub (`4DPlugin-ReportWriter.cpp`, `manifest.json`) with the real command table from `SRPlugin.cpp`. Update `ReportWriter.vcxproj`: sources, include paths `pugixml;rapidjson/include`, C++17, `/utf-8`.
- [ ] **10. Verification.** Corpus round trip (load → save → diff, whitespace-normalised) against the TinyXML build output; 4D test database on macOS arm64 + x86_64 and Windows x64.

## Known defects in the current code (fixed by the phases above)

- `RW/RWBaseTypes.cpp:263`: `UTF_16_to_UTF8` never advances its iterator (endless loop); both UTF helpers are free functions, not the declared `RWTextValue::` statics; `UTF_8_to_UTF16` reads past the end and uses an uninitialised code point.
- `RW/RWBaseTypes.cpp:1060`: `result.append ((UniChar *) '\r')`, a character cast to a pointer.
- `RW/RWBaseTypes.h:383/390`: `ToXMLEscaped` appends the rest of the string per character; `return NULL` into `std::string`.
- `RW/RWBaseTypes.cpp:232`: `snprintf (buf, sizeof (buf[0]), …)` writes an empty colour string.
- `SRP/SRReportWriter.cpp:389`: `ReportToXML` always returns `NULL` (and leaks), so ET export and the `.rwxml` dump never run.
- `SRP/SRPlugin.cpp:745`: `strncat (s3, ".rwxml", sizeof (s3))` can overflow.
- Fixed in phase 3: the `RWBaseTypes` items above, plus the `RWValue` union, `SRGBColor::operator char*` (returned a stack buffer), `GetEntity`, SPAN style values, Base64 stack VLA, missing `MACVER`.
- `CText` = `std::basic_string<unsigned short>` relies on `char_traits<unsigned short>`, deprecated in current libc++ and scheduled for removal.
