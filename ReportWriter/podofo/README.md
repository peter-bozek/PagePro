# PoDoFo for the PDF output

`RWPoDoFoPageComposer` writes PDF files with [PoDoFo](https://github.com/podofo/podofo) 1.0
on macOS and Windows. The libraries are built from tagged sources by the scripts in this
folder; the results are not in the repository.

| Script | Output | Platform |
|---|---|---|
| `build_mac.sh` | `mac/include`, `mac/lib` (static, universal arm64 + x86_64, macOS 10.15+) | Xcode project |
| `build_win.ps1` | `win64/include`, `win64/lib` (static x64, dependencies from vcpkg) | Visual Studio project |

Versions (both scripts): PoDoFo 1.0.4, FreeType 2.14.3, OpenSSL 3.5.9 (LTS), libpng 1.6.59,
libjpeg-turbo 3.1.4 (vcpkg's current versions on Windows). zlib and libxml2 come from the
macOS SDK on the Mac. TIFF, fontconfig and lcms2 support are disabled.

Link order: `podofo podofo_private podofo_3rdparty freetype png16 jpeg ssl crypto z xml2`,
and compile with `PODOFO_STATIC` defined.

## Licences

- PoDoFo: LGPL 2.0 or later. It is linked statically (decision 2026-10-01); the plugin's
  distribution provides what the LGPL requires for that (recipients can relink the plugin
  with a modified PoDoFo, e.g. object files on request).
- FreeType: FreeType License (BSD style, credit in the documentation).
- OpenSSL 3: Apache 2.0. libpng: libpng licence. libjpeg-turbo: IJG + BSD.
- AFDKO (bundled in PoDoFo, CFF subsetting): Apache 2.0.
