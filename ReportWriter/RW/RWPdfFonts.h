/*
 *  RWPdfFonts.h
 *  ReportWriter
 *
 *  Font programs for the PDF composer. The PDF uses the same font the screen and
 *  print composers use for a style (CoreText on the Mac, GDI on Windows): the
 *  platform part copies the font's sfnt tables, the common part rebuilds a
 *  standalone TrueType / OpenType file from them, which PoDoFo embeds as a subset.
 *  Copying tables instead of reading font files also works for collections
 *  (.ttc / .otc) and fonts without a file of their own.
 */

#ifndef	_RWPdfFonts_h_
# define	_RWPdfFonts_h_

# include	"RWString.h"
# include	<cstdint>
# include	<string>
# include	<utility>
# include	<vector>

// sfnt tables: tag ('glyf', 'CFF ', ...) and raw table data
typedef	std::vector<std::pair<uint32_t, std::string>>	RWFontTables;

struct	RWPdfFontProgram
{
	std::vector<char>	data;					// standalone sfnt (TrueType or OpenType/CFF)
	bool				syntheticBold = false;	// bold requested, the font has no bold face
	bool				syntheticItalic = false;	// italic requested, the font has no italic face

	// line metrics in em units from 'hhea' / 'head', the values CoreText reports
	// (PoDoFo's own ascent / descent come from other tables and differ by up to 20 %)
	double				ascent = 0.8;
	double				descent = 0.2;				// positive, below the baseline
	double				lineGap = 0;
};

namespace	RWPdfFonts
{
	// RWStyle::st_bold / st_italic in inStyle; false if no usable font was found
	bool				LoadFont (RWStringView inFamily, int inStyle, RWPdfFontProgram &outFont);

	// platform part (RWFontsMac.cpp / RWFontsWin.cpp): tables of the font the
	// native composer uses for the family and style, falling back to the default font
	bool				CopyFontTables (RWStringView inFamily, int inStyle, RWFontTables &outTables);

	// common part
	std::vector<char>	BuildSfnt (const RWFontTables &inTables);
	bool				HasBoldFace (const RWFontTables &inTables);
	bool				HasItalicFace (const RWFontTables &inTables);
	bool				GetLineMetrics (const RWFontTables &inTables, double &outAscent, double &outDescent, double &outLineGap);

	constexpr uint32_t	Tag (char a, char b, char c, char d)
	{
		return (uint32_t ((unsigned char) a) << 24) | (uint32_t ((unsigned char) b) << 16) | (uint32_t ((unsigned char) c) << 8) | uint32_t ((unsigned char) d);
	}
}

#endif
