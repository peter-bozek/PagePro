/*
 *  RWFontsMac.cpp
 *  ReportWriter
 *
 *  Mac font selection (RWFontsMac.h) and the platform part of the PDF font
 *  lookup (RWPdfFonts.h): the sfnt tables of the CoreText font.
 */

# include	"RWFontsMac.h"
# include	"RWPdfFonts.h"
# include	"RWStringCF.h"
# include	"RWStyle.h"
# include	<cstdio>


// ---------------------------------------------------------------------------
// RWCreateCTFont
// ---------------------------------------------------------------------------

CTFontRef
RWCreateCTFont (RWStringView name, float inSize, int style)
{
	CFStringRef	fontName = RWStr::CreateCFString (name);
	CTFontRef	font = CTFontCreateWithName (fontName, inSize, NULL);
	if (font == NULL)
	{
		printf ("RWCreateCTFont: font '%s' not found!\n", RWStr::ToUTF8 (name).c_str());
		::CFRelease (fontName);
		fontName = CFStringCreateWithCString (kCFAllocatorDefault, RWStyle::cDefFontName, kCFStringEncodingUTF8);
		font = CTFontCreateWithName (fontName, inSize, NULL);
		if (font == NULL)
		{
			::CFRelease (fontName);
			fontName = CFStringCreateWithCString (kCFAllocatorDefault, RWStyle::cDefFontName, kCFStringEncodingUTF8);
			font = CTFontCreateWithName (fontName, inSize, NULL);
		}
	}
	::CFRelease (fontName);
	if (font != NULL && (style & (RWStyle::st_bold | RWStyle::st_italic)) != 0)
	{
		// font variation - bold/italic
		CTFontSymbolicTraits	fstyle = 0;
		if (style & RWStyle::st_bold)
			fstyle |= kCTFontBoldTrait;
		if (style & RWStyle::st_italic)
			fstyle |= kCTFontItalicTrait;
		CTFontRef	fontA = CTFontCreateCopyWithSymbolicTraits (font, 0.0, NULL, fstyle, kCTFontItalicTrait | kCTFontBoldTrait);
		if (fontA == NULL && fstyle == (kCTFontBoldTrait | kCTFontItalicTrait))
		{
			fontA = CTFontCreateCopyWithSymbolicTraits (font, 0.0, NULL, kCTFontBoldTrait, kCTFontItalicTrait | kCTFontBoldTrait);
			if (fontA == NULL)
				fontA = CTFontCreateCopyWithSymbolicTraits (font, 0.0, NULL, kCTFontItalicTrait, kCTFontItalicTrait | kCTFontBoldTrait);
		}
		if (fontA != NULL)
		{
			::CFRelease (font);
			font = fontA;
		}
	}
	return font;
}


// ---------------------------------------------------------------------------
// RWPdfFonts::CopyFontTables
// ---------------------------------------------------------------------------

bool
RWPdfFonts::CopyFontTables (RWStringView inFamily, int inStyle, RWFontTables &outTables)
{
	outTables.clear();

	CTFontRef	font = RWCreateCTFont (inFamily, 12, inStyle);
	if (font == NULL)
		return false;

	if (CFArrayRef tags = CTFontCopyAvailableTables (font, kCTFontTableOptionNoOptions))
	{
		const CFIndex	count = CFArrayGetCount (tags);
		for (CFIndex i = 0; i < count; i++)
		{
			// the array holds the tags as integers, not as CF objects
			const CTFontTableTag	tag = CTFontTableTag (uintptr_t (CFArrayGetValueAtIndex (tags, i)));
			if (CFDataRef data = CTFontCopyTable (font, tag, kCTFontTableOptionNoOptions))
			{
				outTables.emplace_back (uint32_t (tag), std::string (reinterpret_cast <const char*> (CFDataGetBytePtr (data)), size_t (CFDataGetLength (data))));
				CFRelease (data);
			}
		}
		CFRelease (tags);
	}
	CFRelease (font);

	return !outTables.empty();
}
