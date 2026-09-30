/*
 *  RWString.cpp
 *  ReportWriter
 *
 *  Internal string type and std based string utilities.
 */

# include	"RWString.h"

# include	<algorithm>
# include	<cctype>
# include	<charconv>
# include	<climits>
# include	<cstdio>
# include	<cstdlib>
# include	<clocale>

#if	defined(_WIN32)
# include	<locale.h>
#else
# include	<xlocale.h>
#endif


namespace
{
	const	char32_t	kMaxCodePoint	=	0x10FFFF;

	inline	bool	IsSurrogate (char32_t inCP)			{ return inCP >= 0xD800 && inCP <= 0xDFFF; }
	inline	bool	IsHighSurrogate (char32_t inCP)		{ return inCP >= 0xD800 && inCP <= 0xDBFF; }
	inline	bool	IsLowSurrogate (char32_t inCP)		{ return inCP >= 0xDC00 && inCP <= 0xDFFF; }
	inline	bool	IsASCIISpace (char16_t inChar)		{ return inChar == u' ' || inChar == u'\t' || inChar == u'\r' || inChar == u'\n'; }
	inline	char16_t	FoldASCII (char16_t inChar)		{ return (inChar >= u'A' && inChar <= u'Z') ? char16_t (inChar + 32) : inChar; }


	// ---------------------------------------------------------------------------
	// AppendCodePoint
	// ---------------------------------------------------------------------------
	// Append one valid code point to UTF-16 text.

	void
	AppendCodePoint (RWString &ioText, char32_t inCP)
	{
		if (inCP < 0x10000)
			ioText.push_back (char16_t (inCP));
		else
		{
			inCP -= 0x10000;
			ioText.push_back (char16_t (0xD800 + (inCP >> 10)));
			ioText.push_back (char16_t (0xDC00 + (inCP & 0x3FF)));
		}
	}


	// ---------------------------------------------------------------------------
	// NextCodePoint
	// ---------------------------------------------------------------------------
	// Decode one code point from UTF-16 text starting at ioIndex, advancing ioIndex.
	// Lone surrogates decode as U+FFFD.

	char32_t
	NextCodePoint (RWStringView inText, size_t &ioIndex)
	{
		char32_t	cp = inText[ioIndex++];

		if (IsHighSurrogate (cp))
		{
			if (ioIndex < inText.size() && IsLowSurrogate (inText[ioIndex]))
				return 0x10000 + ((cp - 0xD800) << 10) + (inText[ioIndex++] - 0xDC00);
			return RWStr::kReplacementChar;
		}
		if (IsLowSurrogate (cp))
			return RWStr::kReplacementChar;

		return cp;
	}


	// ---------------------------------------------------------------------------
	// Locale independent number conversion
	// ---------------------------------------------------------------------------

#if	defined(_WIN32)
	_locale_t
	CLocale (void)
	{
		static	_locale_t	sLocale = _create_locale (LC_NUMERIC, "C");
		return sLocale;
	}
#else
	locale_t
	CLocale (void)
	{
		static	locale_t	sLocale = newlocale (LC_ALL_MASK, "C", (locale_t) 0);
		return sLocale;
	}
#endif


	// ---------------------------------------------------------------------------
	// NumberPrefix
	// ---------------------------------------------------------------------------
	// Skip leading white space and return the following ASCII characters -
	// numbers never contain anything else.

	std::string
	NumberPrefix (RWStringView inText)
	{
		size_t	i = 0;
		while (i < inText.size() && IsASCIISpace (inText[i]))
			i++;

		std::string	result;
		for (; i < inText.size() && inText[i] < 0x80 && !IsASCIISpace (inText[i]); i++)
			result.push_back (char (inText[i]));

		return result;
	}
}


// ---------------------------------------------------------------------------
// FromWide / ToWide
// ---------------------------------------------------------------------------
// wchar_t is UTF-16 on Windows (plain copy) and UTF-32 elsewhere.

RWString
RWStr::FromWide (std::wstring_view inText)
{
	if constexpr (sizeof (wchar_t) == sizeof (char16_t))
	{
		return RWString (reinterpret_cast <const char16_t*> (inText.data()), inText.size());
	}
	else
	{
		RWString	result;
		result.reserve (inText.size());
		for (wchar_t ch : inText)
		{
			char32_t	cp = char32_t (ch);
			if (cp > kMaxCodePoint || IsSurrogate (cp))
				cp = kReplacementChar;
			AppendCodePoint (result, cp);
		}
		return result;
	}
}

std::wstring
RWStr::ToWide (RWStringView inText)
{
	if constexpr (sizeof (wchar_t) == sizeof (char16_t))
	{
		return std::wstring (reinterpret_cast <const wchar_t*> (inText.data()), inText.size());
	}
	else
	{
		std::wstring	result;
		result.reserve (inText.size());
		for (size_t i = 0; i < inText.size(); )
			result.push_back (wchar_t (NextCodePoint (inText, i)));
		return result;
	}
}


// ---------------------------------------------------------------------------
// FromUTF8
// ---------------------------------------------------------------------------
// Malformed sequences (overlong forms, surrogates, truncated sequences,
// values above U+10FFFF) are replaced by U+FFFD.

RWString
RWStr::FromUTF8 (std::string_view inText)
{
	RWString	result;
	result.reserve (inText.size());

	size_t	i = 0;
	while (i < inText.size())
	{
		unsigned char	ch = (unsigned char) inText[i];
		if (ch < 0x80)
		{
			result.push_back (ch);
			i++;
			continue;
		}

		size_t		len;
		char32_t	cp, minCP;
		if (ch >= 0xC2 && ch <= 0xDF)		{ len = 2; cp = ch & 0x1F; minCP = 0x80; }
		else if (ch >= 0xE0 && ch <= 0xEF)	{ len = 3; cp = ch & 0x0F; minCP = 0x800; }
		else if (ch >= 0xF0 && ch <= 0xF4)	{ len = 4; cp = ch & 0x07; minCP = 0x10000; }
		else
		{
			result.push_back (kReplacementChar);
			i++;
			continue;
		}

		size_t	j = 1;
		for (; j < len && i + j < inText.size(); j++)
		{
			unsigned char	cc = (unsigned char) inText[i + j];
			if ((cc & 0xC0) != 0x80)
				break;
			cp = (cp << 6) | (cc & 0x3F);
		}

		if (j < len || cp < minCP || cp > kMaxCodePoint || IsSurrogate (cp))
			result.push_back (kReplacementChar);
		else
			AppendCodePoint (result, cp);
		i += j;
	}

	return result;
}


// ---------------------------------------------------------------------------
// ToUTF8
// ---------------------------------------------------------------------------

std::string
RWStr::ToUTF8 (RWStringView inText)
{
	std::string	result;
	result.reserve (inText.size());

	for (size_t i = 0; i < inText.size(); )
	{
		char32_t	cp = NextCodePoint (inText, i);
		if (cp < 0x80)
			result.push_back (char (cp));
		else if (cp < 0x800)
		{
			result.push_back (char (0xC0 | (cp >> 6)));
			result.push_back (char (0x80 | (cp & 0x3F)));
		}
		else if (cp < 0x10000)
		{
			result.push_back (char (0xE0 | (cp >> 12)));
			result.push_back (char (0x80 | ((cp >> 6) & 0x3F)));
			result.push_back (char (0x80 | (cp & 0x3F)));
		}
		else
		{
			result.push_back (char (0xF0 | (cp >> 18)));
			result.push_back (char (0x80 | ((cp >> 12) & 0x3F)));
			result.push_back (char (0x80 | ((cp >> 6) & 0x3F)));
			result.push_back (char (0x80 | (cp & 0x3F)));
		}
	}

	return result;
}


// ---------------------------------------------------------------------------
// FromASCII / ToASCII
// ---------------------------------------------------------------------------

RWString
RWStr::FromASCII (std::string_view inText)
{
	RWString	result;
	result.reserve (inText.size());
	for (char ch : inText)
		result.push_back ((unsigned char) ch < 0x80 ? char16_t (ch) : kReplacementChar);
	return result;
}

std::string
RWStr::ToASCII (RWStringView inText, char inReplacement)
{
	std::string	result;
	result.reserve (inText.size());
	for (char16_t ch : inText)
		result.push_back (ch < 0x80 ? char (ch) : inReplacement);
	return result;
}


// ---------------------------------------------------------------------------
// Comparison
// ---------------------------------------------------------------------------

bool
RWStr::Equals (RWStringView inText, std::string_view inASCII)
{
	if (inText.size() != inASCII.size())
		return false;
	for (size_t i = 0; i < inText.size(); i++)
		if (inText[i] != char16_t ((unsigned char) inASCII[i]))
			return false;
	return true;
}

bool
RWStr::EqualsNoCase (RWStringView inText1, RWStringView inText2)
{
	return inText1.size() == inText2.size() && CompareNoCase (inText1, inText2) == 0;
}

bool
RWStr::EqualsNoCase (RWStringView inText, std::string_view inASCII)
{
	if (inText.size() != inASCII.size())
		return false;
	for (size_t i = 0; i < inText.size(); i++)
		if (FoldASCII (inText[i]) != FoldASCII (char16_t ((unsigned char) inASCII[i])))
			return false;
	return true;
}

int
RWStr::CompareNoCase (RWStringView inText1, RWStringView inText2)
{
	size_t	len = std::min (inText1.size(), inText2.size());
	for (size_t i = 0; i < len; i++)
	{
		char16_t	c1 = FoldASCII (inText1[i]);
		char16_t	c2 = FoldASCII (inText2[i]);
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
	}
	if (inText1.size() == inText2.size())
		return 0;
	return inText1.size() < inText2.size() ? -1 : 1;
}

bool
RWStr::StartsWith (RWStringView inText, RWStringView inPrefix)
{
	return inText.substr (0, inPrefix.size()) == inPrefix;
}

bool
RWStr::StartsWithNoCase (RWStringView inText, RWStringView inPrefix)
{
	return inText.size() >= inPrefix.size() && EqualsNoCase (inText.substr (0, inPrefix.size()), inPrefix);
}

bool
RWStr::EndsWith (RWStringView inText, RWStringView inSuffix)
{
	return inText.size() >= inSuffix.size() && inText.substr (inText.size() - inSuffix.size()) == inSuffix;
}

bool
RWStr::Contains (RWStringView inText, RWStringView inPart)
{
	return inText.find (inPart) != RWStringView::npos;
}


// ---------------------------------------------------------------------------
// Transformations
// ---------------------------------------------------------------------------

RWString
RWStr::ToLowerASCII (RWStringView inText)
{
	RWString	result (inText);
	for (char16_t &ch : result)
		ch = FoldASCII (ch);
	return result;
}

RWString
RWStr::ToUpperASCII (RWStringView inText)
{
	RWString	result (inText);
	for (char16_t &ch : result)
		if (ch >= u'a' && ch <= u'z')
			ch = char16_t (ch - 32);
	return result;
}

RWStringView
RWStr::Trim (RWStringView inText)
{
	size_t	start = 0;
	size_t	end = inText.size();
	while (start < end && IsASCIISpace (inText[start]))
		start++;
	while (end > start && IsASCIISpace (inText[end - 1]))
		end--;
	return inText.substr (start, end - start);
}

std::vector<RWString>
RWStr::Split (RWStringView inText, char16_t inSeparator)
{
	std::vector<RWString>	result;
	size_t					start = 0;
	for (;;)
	{
		size_t	pos = inText.find (inSeparator, start);
		result.emplace_back (inText.substr (start, pos == RWStringView::npos ? RWStringView::npos : pos - start));
		if (pos == RWStringView::npos)
			break;
		start = pos + 1;
	}
	return result;
}

RWString
RWStr::ReplaceAll (RWStringView inText, RWStringView inFrom, RWStringView inTo)
{
	if (inFrom.empty())
		return RWString (inText);

	RWString	result;
	size_t		start = 0;
	for (;;)
	{
		size_t	pos = inText.find (inFrom, start);
		if (pos == RWStringView::npos)
			break;
		result.append (inText.substr (start, pos - start));
		result.append (inTo);
		start = pos + inFrom.size();
	}
	result.append (inText.substr (start));
	return result;
}


// ---------------------------------------------------------------------------
// ToInteger
// ---------------------------------------------------------------------------

std::optional<long long>
RWStr::ToInteger (RWStringView inText)
{
	std::string	s = NumberPrefix (inText);
	const char	*p = s.data();
	const char	*end = p + s.size();

	bool	negative = false;
	if (p < end && (*p == '+' || *p == '-'))
		negative = (*p++ == '-');

	int	base = 10;
	if (end - p > 2 && p[0] == '0' && (p[1] == 'x' || p[1] == 'X') && std::isxdigit ((unsigned char) p[2]))
	{
		base = 16;
		p += 2;
	}

	unsigned long long	magnitude = 0;
	auto	[next, ec] = std::from_chars (p, end, magnitude, base);
	if (ec != std::errc() || next == p)
		return std::nullopt;

	if (negative)
	{
		if (magnitude > (unsigned long long) LLONG_MAX + 1)
			return std::nullopt;
		return (long long) (0 - magnitude);
	}
	if (magnitude > (unsigned long long) LLONG_MAX)
		return std::nullopt;
	return (long long) magnitude;
}


// ---------------------------------------------------------------------------
// ToDouble
// ---------------------------------------------------------------------------

std::optional<double>
RWStr::ToDouble (RWStringView inText)
{
	std::string	s = NumberPrefix (inText);
	char		*next = NULL;

#if	defined(_WIN32)
	double	value = _strtod_l (s.c_str(), &next, CLocale());
#else
	double	value = strtod_l (s.c_str(), &next, CLocale());
#endif

	if (next == s.c_str())
		return std::nullopt;
	return value;
}


// ---------------------------------------------------------------------------
// FromInteger / FromDouble
// ---------------------------------------------------------------------------

RWString
RWStr::FromInteger (long long inValue)
{
	char	buf[32];
	auto	[end, ec] = std::to_chars (buf, buf + sizeof (buf), inValue);
	(void) ec;	// 32 characters always suffice
	return FromASCII (std::string_view (buf, end - buf));
}

RWString
RWStr::FromDouble (double inValue, const char *inPrintfFormat)
{
	char	buf[512];	// enough for "%f" of DBL_MAX

#if	defined(_WIN32)
	int		len = _snprintf_l (buf, sizeof (buf) - 1, inPrintfFormat, CLocale(), inValue);
	buf[sizeof (buf) - 1] = 0;
#else
	int		len = snprintf_l (buf, sizeof (buf), CLocale(), inPrintfFormat, inValue);
#endif

	if (len < 0)
		return RWString();
	return FromASCII (std::string_view (buf, std::min (size_t (len), sizeof (buf) - 1)));
}
