# include	"SRDataFormatter.h"
# include	"4DPluginAPI.h"
# include   "RWBaseTypes.h"

#include <sstream>
#include <string>

using namespace std;
// using namespace	FourDAPIEx;

namespace	SRDataFormatter	{


RWTextValue	FormatVariable (const RWValue &inVar, const CText inFormat)
{
    CText					buf;
	UTF16Char				empty[1] = { '\0' };
	RWValue::EValue_Kind	kind = inVar.GetKind();
	CText					format = (inFormat);

/*
	UTF16Char				*format = NULL;
	long					fmtSize = 0;
	long					textSize = 0;
	if (	inFormat
		&&	*inFormat
		&&	(fmtSize = CText::StrLength ((const CText) inFormat)) <= 255
		&&	(	kind == RWValue::eValue_Boolean
			||	kind == RWValue::eValue_Integer
			||	kind == RWValue::eValue_Real
			||	(	kind == RWValue::eValue_Text
				&&	not inVar.IsEmpty()
				&&	(textSize = CText::StrLength ((const CText) inVar.GetText())) <= 80
				)
			||	kind == RWValue::eValue_Date
			||	kind == RWValue::eValue_Time
			)
	)
	{
		CText	fmt (inFormat, CText::_nullTerminated_);	// convert UTF8/UTF16 to UniChar
		format = fmt.CopyCStr (inEncoding);					// convert UniChar to MacRoman for use by 4D
	}
*/

	switch (kind)
	{
		case RWValue::eValue_Undefined:
            buf.assign ("", 0, 1);  // pB 2010-12 ###Undefined Value###");
			break;

		case RWValue::eValue_Boolean:
		{
#if	0
			const UTF16Char	boolFmt[] = { 'T', 'r', 'u', 'e', ';', 'F', 'a', 'l', 's', 'e', 0 };
			PA_FormatLongint (inVar.GetInteger() - 1, format ? format : const_cast <UTF16Char*> (boolFmt), buf.Get());
#else
			buf = format;
			if (buf.length() == 0)
				buf.assign ("True;False");
			long	pos = buf.find (';', 0);
			if (pos ==  string::npos)
				pos = buf.length();
			if (inVar.atoi())
				buf.erase (pos);
			else
				buf.erase (0, pos + 1);
#endif
			break;
		}

		case RWValue::eValue_Integer:
//			snprintf (result = buf, sizeof (buf), "%ld", inVar.GetInteger());
			PA_FormatLongint (inVar.GetInteger(), format ? format : empty, buf.c_str());
			break;

		case RWValue::eValue_Real:
//			snprintf (result = buf, sizeof (buf), "%.15lg", inVar.GetReal());
			PA_FormatReal (inVar.GetReal(), format ? format : empty, buf.c_str());
			break;

		case RWValue::eValue_Text:
//			result = inVar.fText;
			if (format.length() > 0)
				// PA_FormatString (const_cast <CText> (inVar.GetText()), format, buf.Get());
				// PA_FormatString cannot format text values - implement only trivial formatting
			{
				int stringIdx = 0;
				CText	sourceStr (inVar.GetText());
                CText formatting (format);
				int stringLen = sourceStr.length();
				int formatIdx;
				for (formatIdx = 0; formatIdx < formatting.length(); formatIdx++)
				{
					if (formatting[formatIdx] == '#') {
						if(stringIdx < stringLen)
						{
							buf.append(sourceStr[stringIdx]);
							stringIdx++;
						}
					}
					else {
						buf.append(formatting[formatIdx]);
					}
				}
				if (stringIdx < stringLen) {
                    buf.append(sourceStr.substr(stringIdx, stringLen - stringIdx));
				}
			}
			else
				buf = inVar.GetText();
			break;

//••• TODO •••	parse textual string formats into numbers? Should be done probably on SRP conversion...
		case RWValue::eValue_DateTime:
		{
			time_t	tim = (time_t) inVar.GetInteger();
			struct	tm *lt = localtime (&tim);
#if	0
			PA_FormatDate (lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900, 8, buf.Get());	// DateTime
			buf.UpdateLength();
			PA_FormatTime (lt->tm_hour * 3600L + lt->tm_min * 60L + lt->tm_sec, 6, buf.Get() + buf.StrLength());	// DateTime
#else
			char	cbuf [128];
			if (format.length() > 0)
			{
				buf = format;
				strftime (cbuf, sizeof (cbuf), (const char*) buf.GetUTF8(), lt);
			}
			else
				strftime (cbuf, sizeof (cbuf), "%Y-%m-%d %T %Z", lt);
#if	WINVER
			buf.AssignAscii (cbuf);
#else
			buf.AssignUTF8 ((const UTF8Char*) cbuf);
#endif
#endif
			break;
		}

//••• TODO •••	parse textual string formats into numbers? Should be done probably on SRP conversion...
		case RWValue::eValue_Date:
		{	// day: 0 - 31 ==> 5 bits
			// month: 0 - 12 ==> 4 bits
			// day | (month << 5) | (year << 9)
//			snprintf (result = buf, sizeof (buf), "%04d-%02d-%02d", inVar.GetInteger() >> 9, (inVar.GetInteger() >> 5) & 0xF, inVar.GetInteger() & 0x1F);
			int	fmt = 0;
			// if (format && *format >= '1' && *format <= '9' && format[1] == 0)
			//	fmt = *format - '0';
			if (format.length() > 0) {
				CText s  = format;
				fmt = atoi(s.c_str());
			}
			PA_FormatDate (inVar.GetInteger() & 0x1F, (inVar.GetInteger() >> 5) & 0xF, inVar.GetInteger() >> 9, fmt, buf.Get());	// Short
			break;
		}

		case RWValue::eValue_Time:
		{
//			snprintf (result = buf, sizeof (buf), "%02d.%02d.%02d", inVar.GetInteger() / 3600, inVar.GetInteger() / 60 % 60, inVar.GetInteger() % 60);
			int	fmt = 0;
			if (format.length() > 0) {
				CText s  = format;
				fmt = atoi(s.GetCStr());
			}
			PA_FormatTime (inVar.GetInteger(), fmt, buf.Get());	// HH:MM:SS
			break;
		}

		case RWValue::eValue_BLOB:
			buf.AssignAscii ("###BLOB Value###");
			break;

		case RWValue::eValue_PictRefScreen:
		case RWValue::eValue_PictRefPrint:
		case RWValue::eValue_PicturePICT:
		case RWValue::eValue_PicturePDF:
		case RWValue::eValue_PictureJPG:
		case RWValue::eValue_PicturePNG:
		case RWValue::eValue_PictureTIFF:
		case RWValue::eValue_PictureEMF:
			buf.AssignAscii ("###Picture Value###");
			break;

		default:
			buf.AssignAscii ("###Unknown Data Type###");
			break;
	}

	RWTextValue	re (buf.Release());
	return re;
}


}	// namespace	SRDataFormatter
