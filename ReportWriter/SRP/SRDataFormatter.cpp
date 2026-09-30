# include	"SRDataFormatter.h"
# include	"4DPluginAPI.h"
# include   "RWBaseTypes.h"
# include	"RWString4D.h"

#include <ctime>
#include <string>

// using namespace	FourDAPIEx;

namespace	SRDataFormatter	{


// ---------------------------------------------------------------------------
// Format4D																[local]
// ---------------------------------------------------------------------------
// 4D writes at most 255 characters into the result buffer

namespace
{
	const	size_t	k4DResultSize = 256;

	RWString	FormatLongint (long inValue, const RWString &inFormat)
	{
		PA_Unichar	result [k4DResultSize] = { 0 };
		PA_FormatLongint ((PA_long32) inValue, RWStr::ToPA (inFormat), result);
		return RWStr::FromPA (result);
	}

	RWString	FormatReal (double inValue, const RWString &inFormat)
	{
		PA_Unichar	result [k4DResultSize] = { 0 };
		PA_FormatReal (inValue, RWStr::ToPA (inFormat), result);
		return RWStr::FromPA (result);
	}

	RWString	FormatDate (long inDate, short inFormat)
	{
		// day: 0 - 31 ==> 5 bits
		// month: 0 - 12 ==> 4 bits
		// day | (month << 5) | (year << 9)
		PA_Unichar	result [k4DResultSize] = { 0 };
		PA_FormatDate (short (inDate & 0x1F), short ((inDate >> 5) & 0xF), short (inDate >> 9), inFormat, result);
		return RWStr::FromPA (result);
	}

	RWString	FormatTime (long inTime, short inFormat)
	{
		PA_Unichar	result [k4DResultSize] = { 0 };
		PA_FormatTime ((PA_long32) inTime, inFormat, result);	// HH:MM:SS
		return RWStr::FromPA (result);
	}
}


// ---------------------------------------------------------------------------
// FormatVariable
// ---------------------------------------------------------------------------

RWString	FormatVariable (const RWValue &inVar, RWStringView inFormat)
{
	RWString	buf;
	RWString	format (inFormat);

	switch (inVar.GetKind())
	{
		case RWValue::eValue_Undefined:
			break;	// pB 2010-12 ###Undefined Value###

		case RWValue::eValue_Boolean:
		{
			// "text if true;text if false"
			buf = format.empty() ? RWString (u"True;False") : format;
			size_t	pos = buf.find (u';');
			if (pos == RWString::npos)
				pos = buf.length();
			if (inVar.GetBoolean())
				buf.erase (pos);
			else
				buf.erase (0, pos + 1);
			break;
		}

		case RWValue::eValue_Integer:
			buf = FormatLongint (inVar.GetInteger(), format);
			break;

		case RWValue::eValue_Real:
			buf = FormatReal (inVar.GetReal(), format);
			break;

		case RWValue::eValue_Text:
			if (!format.empty())
			{
				// PA_FormatString cannot format text values - implement only trivial formatting:
				// every '#' takes the next character of the text, the rest is appended
				const RWString	&source = inVar.GetText();
				size_t			used = 0;
				for (char16_t ch : format)
				{
					if (ch != u'#')
						buf.push_back (ch);
					else if (used < source.size())
						buf.push_back (source[used++]);
				}
				buf.append (source, used, RWString::npos);
			}
			else
				buf = inVar.GetText();
			break;

//••• TODO •••	parse textual string formats into numbers? Should be done probably on SRP conversion...
		case RWValue::eValue_DateTime:
		{
			time_t		tim = (time_t) inVar.GetInteger();
			struct tm	lt;
#if	VERSIONWIN
			localtime_s (&lt, &tim);
#else
			localtime_r (&tim, &lt);
#endif
			char		cbuf [128];
			std::string	fmt = format.empty() ? std::string ("%Y-%m-%d %H:%M:%S %Z") : RWStr::ToUTF8 (format);
			size_t		len = strftime (cbuf, sizeof (cbuf), fmt.c_str(), &lt);
			buf = RWStr::FromUTF8 (std::string_view (cbuf, len));
			break;
		}

//••• TODO •••	parse textual string formats into numbers? Should be done probably on SRP conversion...
		case RWValue::eValue_Date:
			buf = FormatDate (inVar.GetInteger(), (short) RWStr::ToInteger (format).value_or (0));	// 0 = short
			break;

		case RWValue::eValue_Time:
			buf = FormatTime (inVar.GetInteger(), (short) RWStr::ToInteger (format).value_or (0));
			break;

		case RWValue::eValue_BLOB:
			buf = u"###BLOB Value###";
			break;

		case RWValue::eValue_PictRefScreen:
		case RWValue::eValue_PictRefPrint:
		case RWValue::eValue_PicturePICT:
		case RWValue::eValue_PicturePDF:
		case RWValue::eValue_PictureJPG:
		case RWValue::eValue_PicturePNG:
		case RWValue::eValue_PictureTIFF:
		case RWValue::eValue_PictureEMF:
			buf = u"###Picture Value###";
			break;

		default:
			buf = u"###Unknown Data Type###";
			break;
	}

	return buf;
}


}	// namespace	SRDataFormatter
