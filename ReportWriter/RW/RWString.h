/*
 *  RWString.h
 *  ReportWriter
 *
 *  Internal string type and std based string utilities.
 *
 *  All text inside ReportWriter is UTF-16 held in std::u16string (RWString),
 *  the same encoding 4D uses (PA_Unichar). Conversions exist only for the
 *  boundaries:
 *		- pugixml (wchar_t: UTF-32 on Mac, UTF-16 on Windows)	FromWide / ToWide
 *		- files, JSON output, OS APIs taking UTF-8				FromUTF8 / ToUTF8
 *		- 4D plugin API											RWString4D.h
 *
 *  Invalid input (lone surrogates, malformed UTF-8, out of range code points)
 *  is replaced by U+FFFD, conversions never fail.
 *
 *  Number parsing / formatting is locale independent ("C" locale).
 */

#ifndef	_RWString_h_
# define	_RWString_h_

# include	<string>
# include	<string_view>
# include	<optional>
# include	<vector>

typedef	std::u16string			RWString;
typedef	std::u16string_view		RWStringView;

namespace	RWStr
{
	const	char16_t	kReplacementChar	=	0xFFFD;

	// encoding conversions
	RWString				FromWide (std::wstring_view inText);
	std::wstring			ToWide (RWStringView inText);
	RWString				FromUTF8 (std::string_view inText);
	std::string				ToUTF8 (RWStringView inText);
	RWString				FromASCII (std::string_view inText);		// widening copy, bytes >= 0x80 become U+FFFD
	std::string				ToASCII (RWStringView inText, char inReplacement = '?');

	// comparison - case folding is ASCII only (element / attribute names, keywords)
	bool					Equals (RWStringView inText, std::string_view inASCII);
	bool					EqualsNoCase (RWStringView inText1, RWStringView inText2);
	bool					EqualsNoCase (RWStringView inText, std::string_view inASCII);
	int						CompareNoCase (RWStringView inText1, RWStringView inText2);
	bool					StartsWith (RWStringView inText, RWStringView inPrefix);
	bool					StartsWithNoCase (RWStringView inText, RWStringView inPrefix);
	bool					EndsWith (RWStringView inText, RWStringView inSuffix);
	bool					Contains (RWStringView inText, RWStringView inPart);

	// transformations
	RWString				ToLowerASCII (RWStringView inText);
	RWString				ToUpperASCII (RWStringView inText);
	RWStringView			Trim (RWStringView inText);				// strips ASCII white space (space, tab, CR, LF)
	std::vector<RWString>	Split (RWStringView inText, char16_t inSeparator);
	RWString				ReplaceAll (RWStringView inText, RWStringView inFrom, RWStringView inTo);
	RWString				EscapeXML (RWStringView inText);		// & < > " ' as entities, for hand-written HTML / XML

	// Base64 (standard alphabet, '=' padding). Encoding inserts a space every inGroupLength
	// characters (0 = none); decoding skips white space and stops at the first invalid character.
	RWString				Base64Encode (const void *inData, size_t inSize, size_t inGroupLength = 0);
	std::vector<unsigned char>	Base64Decode (RWStringView inText);

	// numbers
	// Parsing skips leading white space and reads the longest valid prefix, like strtol / strtod
	// (and the sscanf calls it replaces); an empty result means no number was found.
	// Integers accept an optional sign and a "0x" prefix for hexadecimal.
	std::optional<long long>	ToInteger (RWStringView inText);
	std::optional<double>		ToDouble (RWStringView inText);
	RWString					FromInteger (long long inValue);
	RWString					FromDouble (double inValue, const char *inPrintfFormat = "%g");

	// printf style formatting in the "C" locale; the result is expected to be ASCII / UTF-8
	RWString					Format (const char *inPrintfFormat, ...)
#if	defined(__GNUC__) || defined(__clang__)
									__attribute__ ((format (printf, 1, 2)))
#endif
									;
}

#endif
