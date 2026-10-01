/*
 *  RWFontsWin.cpp
 *  ReportWriter
 *
 *  Platform part of the PDF font lookup on Windows (RWPdfFonts.h): the sfnt tables
 *  of the GDI font for a family and style, read with GetFontData. Same family and
 *  style mapping as RWWinPageComposer::MapStyle (GDI+ Font), Arial as the fallback.
 */

# include	"RWPdfFonts.h"
# include	"RWStyle.h"

#if	defined (_WIN32)

# include	<windows.h>
# include	<algorithm>

namespace
{
	// GetFontData takes the tag with the bytes in file order
	inline	DWORD	GDITag (uint32_t inTag)
	{
		return ((inTag & 0xFF) << 24) | ((inTag & 0xFF00) << 8) | ((inTag >> 8) & 0xFF00) | (inTag >> 24);
	}

	bool	ReadTable (HDC inDC, uint32_t inTag, std::string &outData)
	{
		const DWORD	size = ::GetFontData (inDC, GDITag (inTag), 0, NULL, 0);
		if (size == GDI_ERROR || size == 0)
			return false;
		outData.resize (size);
		return ::GetFontData (inDC, GDITag (inTag), 0, &outData [0], size) == size;
	}

	// tags of the selected face: the table directory for a single font file, a probe
	// of the tables a PDF font program can use for a face from a collection (GDI
	// does not tell which face of the collection is selected)
	std::vector<uint32_t>	TableTags (HDC inDC)
	{
		std::vector<uint32_t>	tags;
		unsigned char			header [12];
		if (::GetFontData (inDC, 0, 0, header, sizeof (header)) == sizeof (header)
			&& !(header [0] == 't' && header [1] == 't' && header [2] == 'c' && header [3] == 'f'))
		{
			const unsigned	count = (unsigned (header [4]) << 8) | header [5];
			std::string		directory (size_t (count) * 16, '\0');
			if (::GetFontData (inDC, 0, 12, &directory [0], DWORD (directory.size())) == directory.size())
				for (unsigned i = 0; i < count; i++)
				{
					const unsigned char	*entry = reinterpret_cast <const unsigned char*> (directory.data()) + i * 16;
					tags.push_back ((uint32_t (entry [0]) << 24) | (uint32_t (entry [1]) << 16) | (uint32_t (entry [2]) << 8) | entry [3]);
				}
		}
		if (tags.empty())
		{
			using RWPdfFonts::Tag;
			tags = {	Tag ('c','m','a','p'), Tag ('h','e','a','d'), Tag ('h','h','e','a'), Tag ('h','m','t','x'),
						Tag ('m','a','x','p'), Tag ('n','a','m','e'), Tag ('O','S','/','2'), Tag ('p','o','s','t'),
						Tag ('g','l','y','f'), Tag ('l','o','c','a'), Tag ('c','v','t',' '), Tag ('f','p','g','m'),
						Tag ('p','r','e','p'), Tag ('g','a','s','p'), Tag ('C','F','F',' '), Tag ('C','F','F','2'),
						Tag ('V','O','R','G'), Tag ('v','h','e','a'), Tag ('v','m','t','x'), Tag ('k','e','r','n'),
						Tag ('G','D','E','F'), Tag ('G','P','O','S'), Tag ('G','S','U','B') };
		}
		return tags;
	}

	HFONT	CreateGDIFont (RWStringView inFamily, int inStyle)
	{
		LOGFONTW	lf = { };
		lf.lfHeight = -1000;	// em size, the size does not change the font data
		lf.lfWeight = (inStyle & RWStyle::st_bold) ? FW_BOLD : FW_NORMAL;
		lf.lfItalic = (inStyle & RWStyle::st_italic) ? TRUE : FALSE;
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
		lf.lfQuality = DEFAULT_QUALITY;
		const size_t	length = std::min (inFamily.size(), size_t (LF_FACESIZE - 1));
		std::copy (inFamily.begin(), inFamily.begin() + length, lf.lfFaceName);
		lf.lfFaceName [length] = 0;
		return ::CreateFontIndirectW (&lf);
	}
}


bool
RWPdfFonts::CopyFontTables (RWStringView inFamily, int inStyle, RWFontTables &outTables)
{
	outTables.clear();

	HDC		dc = ::CreateCompatibleDC (NULL);
	if (dc == NULL)
		return false;

	const RWString	fallback = RWStr::FromASCII (RWStyle::cDefFontName);
	for (RWStringView family : { inFamily, RWStringView (fallback) })
	{
		HFONT	font = CreateGDIFont (family, inStyle);
		if (font == NULL)
			continue;
		HGDIOBJ	old = ::SelectObject (dc, font);

		// GDI substitutes silently: accept the font only if the face name matches
		wchar_t	face [LF_FACESIZE] = { };
		::GetTextFaceW (dc, LF_FACESIZE, face);
		const bool	matches = RWStr::EqualsNoCase (RWStr::FromWide (face), family) || family == RWStringView (fallback);
		if (matches)
			for (uint32_t tag : TableTags (dc))
			{
				std::string	data;
				if (ReadTable (dc, tag, data))
					outTables.emplace_back (tag, std::move (data));
			}

		::SelectObject (dc, old);
		::DeleteObject (font);
		if (!outTables.empty())
			break;
	}

	::DeleteDC (dc);
	return !outTables.empty();
}

#endif
