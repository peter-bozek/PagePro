/*
 *  RWPdfFonts.cpp
 *  ReportWriter
 *
 *  Platform independent part of the PDF font lookup (see RWPdfFonts.h).
 */

# include	"RWPdfFonts.h"
# include	"RWStyle.h"
# include	<algorithm>

namespace
{
	inline	uint16_t	Read16 (const std::string &inData, size_t inOffset)
	{
		if (inOffset + 2 > inData.size())
			return 0;
		return uint16_t ((uint8_t (inData [inOffset]) << 8) | uint8_t (inData [inOffset + 1]));
	}

	inline	void		Write16 (std::vector<char> &ioData, uint16_t inValue)
	{
		ioData.push_back (char (inValue >> 8));
		ioData.push_back (char (inValue));
	}

	inline	void		Write32 (std::vector<char> &ioData, uint32_t inValue)
	{
		Write16 (ioData, uint16_t (inValue >> 16));
		Write16 (ioData, uint16_t (inValue));
	}

	// sfnt table checksum: sum of big endian 32 bit words, data zero padded
	uint32_t			Checksum (const std::string &inData)
	{
		uint32_t	sum = 0;
		for (size_t i = 0; i < inData.size(); i += 4)
		{
			uint32_t	word = 0;
			for (size_t j = 0; j < 4; j++)
				word = (word << 8) | (i + j < inData.size() ? uint8_t (inData [i + j]) : 0);
			sum += word;
		}
		return sum;
	}

	const std::string*	FindTable (const RWFontTables &inTables, uint32_t inTag)
	{
		for (const auto &table : inTables)
			if (table.first == inTag)
				return &table.second;
		return nullptr;
	}
}


// ---------------------------------------------------------------------------
// BuildSfnt
// ---------------------------------------------------------------------------
// Offset table + table directory (sorted by tag) + 4 byte aligned tables. The
// 'head' checkSumAdjustment is recomputed for the new file.

std::vector<char>
RWPdfFonts::BuildSfnt (const RWFontTables &inTables)
{
	std::vector<char>	result;

	RWFontTables	tables;
	for (const auto &table : inTables)
		if (!table.second.empty() && table.first != Tag ('D', 'S', 'I', 'G'))	// a signature does not survive the rebuild
			tables.push_back (table);
	if (tables.empty())
		return result;
	std::sort (tables.begin(), tables.end(), [] (const auto &a, const auto &b) { return a.first < b.first; });

	const bool		cff = FindTable (tables, Tag ('C', 'F', 'F', ' ')) || FindTable (tables, Tag ('C', 'F', 'F', '2'));
	const uint16_t	numTables = uint16_t (tables.size());
	uint16_t		entrySelector = 0;
	while ((2u << entrySelector) <= numTables)
		entrySelector++;
	const uint16_t	searchRange = uint16_t ((1u << entrySelector) * 16);

	// 'head' with checkSumAdjustment cleared (offset 8)
	for (auto &table : tables)
		if (table.first == Tag ('h', 'e', 'a', 'd') && table.second.size() >= 12)
			std::fill (table.second.begin() + 8, table.second.begin() + 12, '\0');

	Write32 (result, cff ? Tag ('O', 'T', 'T', 'O') : 0x00010000);
	Write16 (result, numTables);
	Write16 (result, searchRange);
	Write16 (result, entrySelector);
	Write16 (result, uint16_t (numTables * 16 - searchRange));

	uint32_t	offset = uint32_t (12 + 16 * numTables);
	for (const auto &table : tables)
	{
		Write32 (result, table.first);
		Write32 (result, Checksum (table.second));
		Write32 (result, offset);
		Write32 (result, uint32_t (table.second.size()));
		offset += uint32_t ((table.second.size() + 3) & ~size_t (3));
	}

	size_t	headOffset = 0;
	for (const auto &table : tables)
	{
		if (table.first == Tag ('h', 'e', 'a', 'd'))
			headOffset = result.size();
		result.insert (result.end(), table.second.begin(), table.second.end());
		result.resize ((result.size() + 3) & ~size_t (3), '\0');
	}

	if (headOffset != 0)
	{
		const uint32_t	adjustment = 0xB1B0AFBA - Checksum (std::string (result.begin(), result.end()));
		for (int i = 0; i < 4; i++)
			result [headOffset + 8 + i] = char (adjustment >> (24 - 8 * i));
	}

	return result;
}


// ---------------------------------------------------------------------------
// HasBoldFace / HasItalicFace
// ---------------------------------------------------------------------------
// OS/2 fsSelection (offset 62: bit 0 italic, bit 5 bold), OS/2 usWeightClass
// (offset 4) and head macStyle (offset 44: bit 0 bold, bit 1 italic).

bool
RWPdfFonts::HasBoldFace (const RWFontTables &inTables)
{
	if (const std::string *os2 = FindTable (inTables, Tag ('O', 'S', '/', '2')))
		if ((Read16 (*os2, 62) & 0x20) != 0 || Read16 (*os2, 4) >= 600)
			return true;
	if (const std::string *head = FindTable (inTables, Tag ('h', 'e', 'a', 'd')))
		if ((Read16 (*head, 44) & 0x01) != 0)
			return true;
	return false;
}

bool
RWPdfFonts::HasItalicFace (const RWFontTables &inTables)
{
	if (const std::string *os2 = FindTable (inTables, Tag ('O', 'S', '/', '2')))
		if ((Read16 (*os2, 62) & 0x01) != 0)
			return true;
	if (const std::string *head = FindTable (inTables, Tag ('h', 'e', 'a', 'd')))
		if ((Read16 (*head, 44) & 0x02) != 0)
			return true;
	return false;
}


// ---------------------------------------------------------------------------
// GetLineMetrics
// ---------------------------------------------------------------------------
// head unitsPerEm (offset 18), hhea ascender / descender / lineGap (offsets 4, 6, 8,
// signed), as CoreText's CTFontGetAscent / Descent / Leading.

bool
RWPdfFonts::GetLineMetrics (const RWFontTables &inTables, double &outAscent, double &outDescent, double &outLineGap)
{
	const std::string	*head = FindTable (inTables, Tag ('h', 'e', 'a', 'd'));
	const std::string	*hhea = FindTable (inTables, Tag ('h', 'h', 'e', 'a'));
	if (head == nullptr || hhea == nullptr || hhea->size() < 10)
		return false;
	const double	unitsPerEm = Read16 (*head, 18);
	if (unitsPerEm < 16)
		return false;

	outAscent = int16_t (Read16 (*hhea, 4)) / unitsPerEm;
	outDescent = -int16_t (Read16 (*hhea, 6)) / unitsPerEm;
	outLineGap = std::max (0, int (int16_t (Read16 (*hhea, 8)))) / unitsPerEm;
	return outAscent > 0;
}


// ---------------------------------------------------------------------------
// LoadFont
// ---------------------------------------------------------------------------

bool
RWPdfFonts::LoadFont (RWStringView inFamily, int inStyle, RWPdfFontProgram &outFont)
{
	RWFontTables	tables;
	if (!CopyFontTables (inFamily, inStyle, tables))
		return false;

	outFont.data = BuildSfnt (tables);
	outFont.syntheticBold = (inStyle & RWStyle::st_bold) != 0 && !HasBoldFace (tables);
	outFont.syntheticItalic = (inStyle & RWStyle::st_italic) != 0 && !HasItalicFace (tables);
	(void) GetLineMetrics (tables, outFont.ascent, outFont.descent, outFont.lineGap);
	return !outFont.data.empty();
}
