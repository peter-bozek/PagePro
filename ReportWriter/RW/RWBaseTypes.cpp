# include	"RWBaseTypes.h"
# include	"RWStyle.h"
# include	<memory>

#if	WINVER
# include	<OLE2.h>
# include	<gdiplus.h>
#endif

# include	"4DPluginAPI.h"
// using namespace    FourDAPI;

const SRGBColor	cBlackColor		( 0, 0, 0, 65535 );
const SRGBColor	cGrayColor		( 32768, 32768, 32768, 65535 );
const SRGBColor	cLightGrayColor	( 0xeeee, 0xeeee, 0xeeee, 0xffff );
const SRGBColor	cWhiteColor		( 65535, 65535, 65535, 65535 );
const SRGBColor	cWhite50Color	( 65535, 65535, 65535, 32768 );
const SRGBColor	cRedColor		( 65535, 0, 0, 65535 );
const SRGBColor	cDarkRedColor	( 0xcccc, 0, 0, 65535 );
const SRGBColor	cGreenColor		( 0, 65535, 0, 65535 );
const SRGBColor	cBlueColor		( 0, 0, 65535, 65535 );
const SRGBColor	cDarkBlueColor	( 0, 0, 32768, 65535 );
const SRGBColor	cCyanColor		( 0, 65535, 65535, 65535 );
const SRGBColor	cMagentaColor	( 65535, 0, 65535, 65535 );
const SRGBColor	cYellowColor	( 65535, 65535, 0, 65535 );
const SRGBColor	cBrownColor		( 39321, 26214, 13107, 65535 );	// 60% 40% 20%
const SRGBColor	cOrangeColor	( 65535, 32768, 0, 65535 );	// 100% 50% 0%
const SRGBColor	cDarkOrangeColor	( 0xcccc, 0x6666, 0, 65535 );	// 100% 50% 0%
const SRGBColor	cPurpleColor	( 32768, 0, 32768, 65535 );	// 50% 0% 50%
const SRGBColor	cHighlightColor		( 0, 0, 32768, 32768 );
const SRGBColor	cEmptyColor		( 0, 0, 0, 0 );

#ifndef	M_PI
# define	M_PI		3.14159265358979323846
#endif

static	float	Round (float f, float dist)
{
	float	i;
	if (f < 0)
		i = floorf (f + dist);
	else
		i = ceilf (f - dist);
	
	return i;
}

short	RoundToShort (double f)
{
	/* short	i;
	if (f < 0)
		i = floorf (f + 0.5);
	else
		i = ceilf (f - 0.5); */ // pB

	return (short) lround (f);
}


#if	USE_MAC_TYPES

SPoint::operator Point (void) const
{
	Point	pt;
	pt.h = RoundToShort (h);
	pt.v = RoundToShort (v);
	return pt;
}


SRect::operator Rect (void) const
{
	Rect	rect;
	rect.top = RoundToShort (top);
	rect.left = RoundToShort  (left);
	rect.bottom = RoundToShort (bottom);
	rect.right = RoundToShort (right);

	return rect;
}

#endif

SPoint&
SPoint::operator = (const char *inValue)
{
	h = v = 0;
	if (inValue && *inValue)	//mbs 19052010
	{
		if (sscanf (inValue, "%lg;%lg", &h, &v) != 2)
			sscanf (inValue, "%lg,%lg", &h, &v);
	}
	return *this;
}


SPoint::operator const char* (void) const
{
	static	char buf [2][64];
	static	int cur = 0;
	cur = 1 - cur;
	snprintf (buf[cur], sizeof (buf[0]), "%g;%g", h, v);
	return buf[cur];
}

SRect&
SRect::operator = (const char *inValue)
{
	top = left = bottom = right = 0;
	if (inValue && *inValue)	//mbs 19052010
	{
//		sscanf (inValue, "%g,%g,%g,%g", &top, &left, &bottom, &right);
		if (sscanf (inValue, "%lg;%lg;%lg;%lg", &left, &top, &right, &bottom) != 4)
			sscanf (inValue, "%lg,%lg,%lg,%lg", &left, &top, &right, &bottom);
	}
	return *this;
}


SRect::operator const char* (void) const
{
	static	char buf [2][64];
	static	int cur = 0;
	cur = 1 - cur;
//	snprintf (buf[cur], sizeof (buf[0]), "%g,%g,%g,%g", top, left, bottom, right);
	snprintf (buf[cur], sizeof (buf[0]), "%lg;%lg;%lg;%lg", left, top, right, bottom);
	return buf [cur];
}


SRGBColor&
SRGBColor::operator = (const CText inValue)
{
	red = green = blue = 0;
	alpha = ~0;
	if (inValue.length() > 0)
	{
        CText value = inValue;
		unsigned long	l = 0;
        
		while (l < value.length() &&
               (value[l] == ' ' || value[l] == '\t' || value[l] == '\r' || value[l] == '\n'))
			l++;
            
        
        if (l > 0) value = inValue.substr(l);
               
        if (value.length())
        {
            if (value[0] == '#')
            {
                const UniChar *p = value.c_str() + 1;
                const UniChar *in = value.c_str();
                for (; (*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F'); p++)
                    ;
                if (p > in + 1 && p < in + 10)
                {
                    l = 0;
                    sscanf ((char *)(in + 1), "%lx", &l);
                    operator = (l);
                    if (p < in + 8)
                        alpha = ~0;
                }
            }
            else if ((value[0] >= 'a' && value[0] <= 'z') || (value[0] >= 'A' && value[0] <= 'Z'))
            {
                if (TEXT_STARTS_WITH (value, "red"))
                    *this = cRedColor;
                else if (TEXT_STARTS_WITH (value, "green"))
                    *this = cGreenColor;
                else if (TEXT_STARTS_WITH (value, "blue"))
                    *this = cBlueColor;
                else if (TEXT_STARTS_WITH (value, "white"))
                    *this = cWhiteColor;
                else if (TEXT_STARTS_WITH (value, "gray"))
                    *this = cGrayColor;
                else if (TEXT_STARTS_WITH (value, "lightgray"))
                    *this = cLightGrayColor;
                else if (TEXT_STARTS_WITH (value, "transparent"))
                    red = green = blue = alpha = 0;
                else if (TEXT_STARTS_WITH (value, "cyan"))
                    *this = cCyanColor;
                else if (TEXT_STARTS_WITH (value, "magenta"))
                    *this = cMagentaColor;
                else if (TEXT_STARTS_WITH (value, "yellow"))
                    *this = cYellowColor;
                else if (TEXT_STARTS_WITH (value, "brown"))
                    *this = cBrownColor;
                else if (TEXT_STARTS_WITH (value, "orange"))
                    *this = cOrangeColor;
                else if (TEXT_STARTS_WITH (value, "purple"))
                    *this = cPurpleColor;
                else // if (STR_STARTS_WITH (inValue, "black"))
                    *this = cBlackColor;
            }
            else if (strchr ((char *)value.c_str(), '.') != NULL)	//mbs 04042010
            {
                float	r, g, b, a = 1;
                if (sscanf ((char *)value.c_str(), "%g,%g,%g,%g", &r, &g, &b, &a) >= 3)
                {
                    if (r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1)
                    {
                        red = (unsigned short) (r * 65535);
                        green = (unsigned short) (g * 65535);
                        blue = (unsigned short) (b * 65535);
                        alpha = (unsigned short) (a * 65535);
                    }
                }
            }
            else if (sscanf ((char *)value.c_str(), "%hi,%hi,%hi,%hi", &red, &green, &blue, &alpha) == 1)
            {
                if (sscanf ((char *)value.c_str(), "%li", &l) == 1)
                    operator = (l);
            }
        }
	}
	return *this;
}




SRGBColor::operator CText (void) const
{
	static char buf [64];
//	snprintf (buf[cur], sizeof (buf[0]), "%#x,%#x,%#x,%#x", red, green, blue, alpha);
//mbs 04042010	float as default
//	snprintf (buf[cur], sizeof (buf[0]), "%g,%g,%g,%g", red/65535., green/65535., blue/65535., alpha/65535.);
//	if (strchr (buf[cur], '.') == NULL)
//		strcat (buf[cur], ".");
//mbs 27042010	ARGB as default
	snprintf ( (char *)buf, sizeof (buf[0]), "#%08lx", (unsigned long) *this);
	return RWTextValue::UTF_8_to_UTF16(buf);
}



RWTextValue&
RWTextValue::Copy (const char *value)
{
	Free();
	if (value && *value)
	{
        data_ = RWTextValue::UTF_8_to_UTF16(value);
	}

	return *this;
}



RWTextValue&
RWTextValue::Copy (const CText &value)
{
	Free();
    data_ = value;
	
	return *this;
}


CXMLText
UTF_16_to_UTF8(const CText value)
{
    CXMLText out;
    unsigned int codepoint = 0;
    auto it = value.cbegin();
    while (*it != 0)
     {
         if (*it >= 0xd800 && *it <= 0xdbff)
             codepoint = ((*it - 0xd800) << 10) + 0x10000;
         else
         {
             if (*it >= 0xdc00 && *it <= 0xdfff)
                 codepoint |= *it - 0xdc00;
             else
                 codepoint = *it;

             if (codepoint <= 0x7f)
                 out.append(1, static_cast<uint8_t>(codepoint));
             else if (codepoint <= 0x7ff)
             {
                 out.append(1, static_cast<uint8_t>(0xc0 | ((codepoint >> 6) & 0x1f)));
                 out.append(1, static_cast<uint8_t>(0x80 | (codepoint & 0x3f)));
             }
             else if (codepoint <= 0xffff)
             {
                 out.append(1, static_cast<uint8_t>(0xe0 | ((codepoint >> 12) & 0x0f)));
                 out.append(1, static_cast<uint8_t>(0x80 | ((codepoint >> 6) & 0x3f)));
                 out.append(1, static_cast<uint8_t>(0x80 | (codepoint & 0x3f)));
             }
             else
             {
                 out.append(1, static_cast<uint8_t>(0xf0 | ((codepoint >> 18) & 0x07)));
                 out.append(1, static_cast<uint8_t>(0x80 | ((codepoint >> 12) & 0x3f)));
                 out.append(1, static_cast<uint8_t>(0x80 | ((codepoint >> 6) & 0x3f)));
                 out.append(1, static_cast<uint8_t>(0x80 | (codepoint & 0x3f)));
             }
             codepoint = 0;
         }
     }
     return out;
}

CText
UTF_8_to_UTF16(const CXMLText &value)
{
    CText out;
    unsigned int codepoint;
    auto it = value.cbegin();
    while (*it != 0)
     {
         uint8_t ch = *it;
         if (ch <= 0x7f)
             codepoint = ch;
         else if (ch <= 0xbf)
             codepoint = (codepoint << 6) | (ch & 0x3f);
         else if (ch <= 0xdf)
             codepoint = ch & 0x1f;
         else if (ch <= 0xef)
             codepoint = ch & 0x0f;
         else
             codepoint = ch & 0x07;
         ++it;
         if (((*it & 0xc0) != 0x80) && (codepoint <= 0x10ffff))
         {
             if (codepoint > 0xffff)
             {
                 codepoint -= 0x10000;
                 out.append(1, static_cast<UTF16Char>(0xd800 + (codepoint >> 10)));
                 out.append(1, static_cast<UTF16Char>(0xdc00 + (codepoint & 0x03ff)));
             }
             else if (codepoint < 0xd800 || codepoint >= 0xe000)
                 out.append(1, static_cast<UTF16Char>(codepoint));
         }
     }
     return out;

}

/*
RWValue::RWValue (const CText inText)
	:	fKind (eValue_Text),
		fOwn (false),
		fText (0)
{
	if (inText)
	{
		fText = reinterpret_cast <CText> (strdup (reinterpret_cast <const char*> (inText));
		fOwn = true;
	}
	return;
}
*/


const char *	RWValue::sPictFormats[] = { "BLOB", "PICT", "PDF", "JPG", "PNG", "TIFF", "EMF", NULL };


RWValue::RWValue (EValue_Kind inKind, void* inData, size_t inSize)
	:	fKind (inKind),
		fOwn (false)
{
	fBlob.fSize = 0;
	fBlob.fData = NULL;

	if (inSize != 0 && inData != NULL)
	{
		fBlob.fData = malloc (inSize);
		if (fBlob.fData != NULL)
		{
			fOwn = true;
			fBlob.fSize = inSize;
			memcpy (fBlob.fData, inData, inSize);
		}
	}
	return;
}


RWValue::RWValue (const RWValue &inOriginal)
	:	fKind (eValue_Undefined),
		fOwn (false)
{
	Clone (inOriginal);
}

/*
RWValue&
RWValue::operator = (const RWValue &inOriginal)
{
	return Attach (inOriginal);
}


RWValue&
RWValue::operator = (RWValue &inOriginal)
{
	return Detach (inOriginal);
}
*/

void
RWValue::Free (void)
{
	if (fOwn)
	{
		fOwn = false;
		if (fKind == eValue_XMLText)
		{
            fXMLText.clear();  //free (const_cast <CXMLText> (fXMLText));
		}
		else if (fKind == eValue_Text)
		{
            fText.clear();  //free (fText);
		}
		else if (fKind == eValue_PictRefScreen)
		{
#if	MACVER
			if (fInteger)
				::CFRelease ((RWScreenPict) fInteger);
#else
			if (fInteger)
				delete reinterpret_cast <RWScreenPict> (fInteger);
#endif
		}
		else if (fKind == eValue_PictRefPrint)
		{
#if	MACVER
			if (fInteger)
				::CFRelease ((RWPrintPict) fInteger);
#else
			if (fInteger)
				delete reinterpret_cast <RWPrintPict> (fInteger);
#endif
		}
		else if (fKind >= eValue_BLOB)
		{
			if (fBlob.fSize != 0 && fBlob.fData != NULL)
			{
				free (fBlob.fData);
				fBlob.fSize = 0;
				fBlob.fData = NULL;
			}
		}
	}
	fKind = eValue_Undefined;

	return;
}


RWValue&
RWValue::Attach (const RWValue &inOriginal)
{
	if (static_cast<const void*> (this) != static_cast<const void*> (&inOriginal))
	{
		Free();
		
		fKind = inOriginal.fKind;
//		fOwn = false;
		switch (fKind)
		{
			case eValue_Undefined:
				break;

			case eValue_Boolean:
			case eValue_Integer:
			case eValue_DateTime:
			case eValue_Date:
			case eValue_Time:
			case eValue_PictRefScreen:
			case eValue_PictRefPrint:
				fInteger = inOriginal.fInteger;
				break;
				
			case eValue_Real:
				fReal = inOriginal.fReal;
				break;
				
			case eValue_XMLText:
				fXMLText = inOriginal.fXMLText;
				break;

			case eValue_Text:
				fText = inOriginal.fText;
				break;
				
			case eValue_BLOB:
			case eValue_PicturePICT:
			case eValue_PicturePDF:
			case eValue_PictureJPG:
			case eValue_PicturePNG:
			case eValue_PictureTIFF:
			case eValue_PictureEMF:
				fBlob.fSize = inOriginal.fBlob.fSize;
				fBlob.fData = inOriginal.fBlob.fData;
				break;
				
			default:
				fKind = eValue_Undefined;
				break;
		}
	}
	
	return *this;
}


RWValue&
RWValue::Detach (RWValue &inOriginal)
{
	if (static_cast<void*> (this) != static_cast<void*> (&inOriginal))
	{
		Free();
		
		fKind = inOriginal.fKind;
		fOwn = inOriginal.fOwn;
		inOriginal.fOwn = false;
		switch (fKind)
		{
			case eValue_Undefined:
				break;

			case eValue_Boolean:
			case eValue_Integer:
			case eValue_DateTime:
			case eValue_Date:
			case eValue_Time:
			case eValue_PictRefScreen:
			case eValue_PictRefPrint:
				fInteger = inOriginal.fInteger;
				break;
				
			case eValue_Real:
				fReal = inOriginal.fReal;
				break;
				
			case eValue_XMLText:
				fXMLText = inOriginal.fXMLText;
				break;
				
			case eValue_Text:
				fText = inOriginal.fText;
				break;
				
			case eValue_BLOB:
			case eValue_PicturePICT:
			case eValue_PicturePDF:
			case eValue_PictureJPG:
			case eValue_PicturePNG:
			case eValue_PictureTIFF:
			case eValue_PictureEMF:
				fBlob.fSize = inOriginal.fBlob.fSize;
				fBlob.fData = inOriginal.fBlob.fData;
				break;
				
			default:
				inOriginal.fOwn = fOwn;
				fOwn = false;
				fKind = eValue_Undefined;
				break;
		}
	}
	
	return *this;
}


RWValue&
RWValue::Clone (const RWValue &inOriginal)
{
	if (static_cast<const void*> (this) != static_cast<const void*> (&inOriginal))
	{
		Free();

		fKind = inOriginal.fKind;
		switch (fKind)
		{
			case eValue_Undefined:
				break;

			case eValue_Boolean:
			case eValue_Integer:
			case eValue_DateTime:
			case eValue_Date:
			case eValue_Time:
				fInteger = inOriginal.fInteger;
				break;

			case eValue_Real:
				fReal = inOriginal.fReal;
				break;

			case eValue_XMLText:
				if (!inOriginal.fXMLText.empty())
				{
					fXMLText =  inOriginal.fXMLText;
					fOwn = true;
				}
				else
					fXMLText.clear();
				break;

			case eValue_Text:
				if (!inOriginal.fText.empty())
				{
					// RWTextValue	t ((const CText) inOriginal.fText);
                    fText = inOriginal.fText;  // t.Detach();
					fOwn = true;
				}
				else
					fText.clear();
				break;

			case eValue_PictRefScreen:
				fInteger = inOriginal.fInteger;
#if	MACVER
				if (fInteger)
					::CFRetain ((RWScreenPict) fInteger);
#else
				if (fInteger)
					fInteger = (long) new RWScreenPict (reinterpret_cast <RWScreenPict> (fInteger));
#endif
				break;

			case eValue_PictRefPrint:
				fInteger = inOriginal.fInteger;
#if	MACVER
				if (fInteger)
					::CFRetain ((RWPrintPict) fInteger);
#else
				if (fInteger)
					fInteger = (long) new RWPrintPict (reinterpret_cast <RWPrintPict> (fInteger));
#endif
				break;

			case eValue_BLOB:
			case eValue_PicturePICT:
			case eValue_PicturePDF:
			case eValue_PictureJPG:
			case eValue_PicturePNG:
			case eValue_PictureTIFF:
			case eValue_PictureEMF:
				if (inOriginal.fBlob.fSize > 0 && inOriginal.fBlob.fData != NULL)
				{
					fBlob.fData = malloc (inOriginal.fBlob.fSize);
					if (fBlob.fData != NULL)
					{
						fOwn = true;
						fBlob.fSize = inOriginal.fBlob.fSize;
						memcpy (fBlob.fData, inOriginal.fBlob.fData, inOriginal.fBlob.fSize);
					}
				}
				else
				{
					fBlob.fSize = 0;
					fBlob.fData = NULL;
				}
				break;

			default:
				fKind = eValue_Undefined;
				break;
		}
	}

	return *this;
}



bool
RWValue::operator == (const RWValue &inCompare)
const
{
	bool	equal = false;

	if (fKind == inCompare.fKind)
	{
		switch (fKind)
		{
			case eValue_Undefined:
				equal = true;
				break;
				
			case eValue_Boolean:
			case eValue_Integer:
			case eValue_DateTime:
			case eValue_Date:
			case eValue_Time:
			case eValue_PictRefScreen:
			case eValue_PictRefPrint:
				equal = (fInteger == inCompare.fInteger);
				break;

			case eValue_Real:
				equal = (fReal == inCompare.fReal);
				break;
				
			case eValue_XMLText:
                equal = fXMLText.compare(inCompare.fXMLText) == 0;
				break;
				
			case eValue_Text:
                equal = fText.compare(inCompare.fText) == 0;
                break;

			case eValue_BLOB:
			case eValue_PicturePICT:
			case eValue_PicturePDF:
			case eValue_PictureJPG:
			case eValue_PicturePNG:
			case eValue_PictureTIFF:
			case eValue_PictureEMF:
				if (fBlob.fSize != inCompare.fBlob.fSize)
					;	// size must match
				else if (fBlob.fData == inCompare.fBlob.fData)
					equal = true;	// both are nil or point to the same data
				else if (fBlob.fSize == 0)
					equal = true;	// no data
				else if (
						fBlob.fData != NULL
					&&	inCompare.fBlob.fData != NULL
					&&	memcmp (fBlob.fData, inCompare.fBlob.fData, fBlob.fSize) == 0
				)
					equal = true;
				break;
		}
	}

	return equal;
}




void
RWValue::GetTextValue (RWTextValue &outValue, const char* fmt)
const
{
	char		buf [64];
	const char	*p = buf;
	switch (fKind)
	{
		case eValue_Undefined:
			p = "<NULL>";
			break;

		case eValue_Boolean:
			p = fInteger ? "1" : "0";
			break;

		case eValue_Integer:
			snprintf (buf, sizeof (buf), fmt? fmt: "%ld", fInteger);
			break;

		case RWValue::eValue_DateTime:
		{
			time_t	tim = (time_t) fInteger;
			struct	tm *lt = localtime (&tim);
			strftime (buf, sizeof (buf), "%Y-%m-%dT%T%Z", lt);
			break;
		}
			
		case RWValue::eValue_Date:
			// day: 0 - 31 ==> 5 bits
			// month: 0 - 12 ==> 4 bits
			// day | (month << 5) | (year << 9)
			snprintf (buf, sizeof (buf), "%04ld-%02ld-%02ld", fInteger >> 9, (fInteger >> 5) & 0xF, fInteger & 0x1F);
			break;
			
		case RWValue::eValue_Time:
			snprintf (buf, sizeof (buf), "%02ld.%02ld.%02ld", fInteger / 3600, fInteger / 60 % 60, fInteger % 60);
			break;
			
		case eValue_Real:
			snprintf (buf, sizeof (buf), fmt? fmt: "%lg", fReal);
			break;

		case eValue_XMLText:
		{
            outValue = RWTextValue::UTF_8_to_UTF16 (fXMLText.c_str());
			p = NULL;
			break;
		}

		case eValue_Text:
			outValue = fText;
			p = NULL;
			break;

		case eValue_BLOB:
			p = fmt? fmt: "<BLOB>";
			break;

		case eValue_PictRefScreen:
#if	MACVER
			p = fmt? fmt: "<CGImageRef>";
#else
			p = fmt? fmt: "<Gdiplus::Bitmap*>";
#endif
			break;
		case eValue_PictRefPrint:
#if	MACVER
			p = fmt? fmt: "<CGPDFDocumentRef>";
#else
			p = fmt? fmt: "<Gdiplus::Metafile*>";
#endif
			break;
		case eValue_PicturePICT:
			p = fmt? fmt: "<IMAGE_PICT>";
			break;
		case eValue_PicturePDF:
			p = fmt? fmt: "<IMAGE_PDF>";
			break;
		case eValue_PictureJPG:
			p = fmt? fmt: "<IMAGE_JPG>";
			break;
		case eValue_PicturePNG:
			p = fmt? fmt: "<IMAGE_PNG>";
			break;
		case eValue_PictureTIFF:
			p = fmt? fmt: "<IMAGE_TIFF>";
			break;
		case eValue_PictureEMF:
			p = fmt? fmt: "<IMAGE_EMF>";
			break;
	}

	if (p && *p)
		outValue = (const char *) p;
	return;
}


bool
RWValue::CoerceValue (EValue_Kind inKind)
{
	if (inKind == fKind)
		return true;

	switch (inKind)
	{
		case eValue_Boolean:
			if (fKind == eValue_Integer)
				SetBoolean (GetInteger() > 0);
			else if (fKind == eValue_Real)
				SetBoolean (GetReal() > 0);
			else if (fKind == eValue_XMLText)
				SetBoolean (!GetXMLText().empty() && !TEXT_EQUALS (GetXMLText(), "0"));
			else if (fKind == eValue_Text)
				SetBoolean (!GetText().empty() && !TEXT_EQUALS (GetText(), "0"));
			else
				return false;
			break;

		case eValue_Integer:
		{
			long	lVal;
			if (fKind == eValue_Boolean)
				SetInteger (GetBoolean());
			else if (fKind == eValue_Real)
				SetInteger (GetReal());
			else if (fKind == eValue_XMLText)
			{
				if (!GetXMLText().empty())
				{
					//mbs 12072010	special case for color
                    if (GetXMLText()[0] == '#')
					{
						SRGBColor	c (GetXMLText().c_str());
						SetInteger ((unsigned long) c);
					}
					else if (sscanf ((char *)GetXMLText().c_str(), "%li", &lVal) == 1)
						SetInteger (lVal);
					else
						return false;
				}
				else
					SetInteger (0);
			}
			else if (fKind == eValue_Text)
			{
				CoerceValue (eValue_XMLText);
				if (!GetXMLText().empty())
				{
					if (sscanf ((char *)GetXMLText().c_str(), "%li", &lVal) == 1)
						SetInteger (lVal);
					else
						return false;
				}
				else
					SetInteger (0);
			}
			else
				return false;
			break;
		}

		case eValue_Real:
		{
			double	fVal;
			if (fKind == eValue_Boolean)
				SetReal (GetBoolean());
			else if (fKind == eValue_Integer)
				SetReal (GetInteger());
			else if (fKind == eValue_XMLText)
			{
				if (!GetXMLText().empty() && sscanf ((char *)GetXMLText().c_str(), "%lg", &fVal) == 1)
					SetReal (fVal);
			}
			else if (fKind == eValue_Text)
			{
				CoerceValue (eValue_XMLText);
				if (!GetXMLText().empty() && sscanf ((char *)GetXMLText().c_str(), "%lg", &fVal) == 1)
					SetReal (fVal);
			}
			else
				return false;
			break;
		}

		case eValue_XMLText:
			if (fKind == eValue_Text)
			{
					fKind = eValue_XMLText;
					fXMLText = RWTextValue::UTF_16_to_UTF8 (fText);
			}
			else
				return false;
			break;

		case eValue_Text:
			if (fKind == eValue_XMLText)
			{
					fKind = eValue_Text;
					fText = RWTextValue::UTF_8_to_UTF16(fXMLText);
			}
			else
				return false;
			break;

//		case eValue_DateTime:
//		case eValue_Date:
//		case eValue_Time:
//			return false;

		case eValue_PictRefScreen:
		case eValue_PictRefPrint:
			if (fKind > eValue_BLOB)
			{
				//••• TODO •••	convert image format...
				return false;
			}
			else
				return false;
			break;

		case eValue_BLOB:
			return false;
			break;

		case eValue_PicturePICT:
		case eValue_PicturePDF:
		case eValue_PictureJPG:
		case eValue_PicturePNG:
		case eValue_PictureTIFF:
		case eValue_PictureEMF:
			if (fKind >= eValue_PictRefScreen)
			{
				//••• TODO •••	convert image format...
				return false;
			}
			else
				return false;
			break;

		default:
			return false;
			break;
	}

	return true;
}



// ---------------------------------------------------------------------------
// WriteText												  [static][public]
// ---------------------------------------------------------------------------

void
RWTools::WriteText (FILE *fd, const CXMLText inText)
{
    if (!inText.empty())
	{
		fprintf (fd, "%s", EscapeAttributedString(inText).c_str());
	}

	return;
}

void
RWTools::WriteText (FILE *fd, const CText inText)
{
    if (!inText.empty())
    {
        fprintf (fd, "%s", EscapeAttributedString( RWTextValue::UTF_16_to_UTF8(inText)).c_str());
    }

    return;
}


void
RWTools::WriteText (XMLElement *inParent, const CXMLText inText)
{
	if (!inText.empty())
	{
        XMLText * myText = inParent->ToDocument()->NewText((const char *)inText.c_str());
		inParent->InsertEndChild (myText);
	}

	return;
}


// ---------------------------------------------------------------------------
// ParseIntoText											  [static][public]
// ---------------------------------------------------------------------------

CText
RWTools::ParseIntoText (const XMLElement *inNode, bool)
{
    CText	result;

	const XMLNode		*node;
	for ( node = inNode->FirstChild(); node; node = node->NextSibling() )
	{
		const XMLText	*text = node->ToText();

		if (text)
		{
			result.append(RWTextValue::UTF_8_to_UTF16( text->Value()));
		}
		else
		{
            const XMLElement	*elem = node->ToElement();
			if (elem)
			{
				const CText	name = RWTextValue::UTF_8_to_UTF16 (elem->Value());
				if (TEXT_EQUALS (name, "Data"))
				{
                    CText	nested (ParseIntoText (elem, true));
					result.append(nested);
				}
				else if (TEXT_EQUALS (name, "NL"))
				{
					result.append((UniChar *)'\r');
				}
			}
		}
	}

	return result;
}

CText
RWTools::ParseIntoText (const XMLElement *inNode)
{
    return ParseIntoText(inNode, true);
}



/*--- function HTUU_encode -----------------------------------------------
 *
 *   Encode a single line of binary data to a standard format that
 *   uses only printing ASCII characters (but takes up 33% more bytes).
 *
 *    Entry    bufin    points to a buffer of bytes
 *             nbytes   is the number of bytes in that buffer.
 *             bufcoded points to an output buffer.  Be sure that this
 *                      can hold at least 1 + (4*nbytes)/3 characters.
 *
 *    Exit     bufcoded contains the coded line.  The first 4*nbytes/3 bytes
 *                      contain printing ASCII characters representing
 *                      those binary bytes. This may include one or
 *                      two '=' characters used as padding at the end.
 *                      The last byte is a zero byte.
 *             Returns the number of ASCII characters in "bufcoded".
 */
static	const unsigned char six2pr[64] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '0','1','2','3','4','5','6','7','8','9','+','/'
};

static	long	MyEncode (const unsigned char *bufin, long nbytes, char *bufcoded)
{
/* ENC is the basic 1 character encoding function to make a char printing */
#define ENC(c) six2pr[c]

	char				*outptr = bufcoded;
	long				i;

	if (nbytes % 3)
		nbytes -= 3;	//mbs 07072010	ensure we do not access buffer beyond the end!
	for (i=0; i<nbytes; i += 3)
	{
		*(outptr++) = ENC(*bufin >> 2);            /* c1 */
		*(outptr++) = ENC(((*bufin << 4) & 060) | ((bufin[1] >> 4) & 017)); /*c2*/
		*(outptr++) = ENC(((bufin[1] << 2) & 074) | ((bufin[2] >> 6) & 03));/*c3*/
		*(outptr++) = ENC(bufin[2] & 077);         /* c4 */
		bufin += 3;
	}
	if (nbytes % 3)
		nbytes += 3;
	if (i < nbytes)	//mbs 07072010	ensure we do not access buffer beyond the end!
	{
		unsigned char	buf [3] = { 0 };
		memcpy (buf, bufin, nbytes - i);
		*(outptr++) = ENC(*buf >> 2);            /* c1 */
		*(outptr++) = ENC(((*buf << 4) & 060) | ((buf[1] >> 4) & 017)); /*c2*/
		*(outptr++) = ENC(((buf[1] << 2) & 074) | ((buf[2] >> 6) & 03));/*c3*/
		*(outptr++) = ENC(buf[2] & 077);         /* c4 */
		bufin += 3;
		i += 3;
	}
		

	/* If nbytes was not a multiple of 3, then we have encoded too
	* many characters.  Adjust appropriately.
	*/
	if(i == nbytes+1)		// There were only 2 bytes in that last group
	{
		outptr[-1] = '=';
	}
	else if(i == nbytes+2)	// There was only 1 byte in that last group
	{
		outptr[-1] = '=';
		outptr[-2] = '=';
	}
	*outptr = '\0';

	return outptr - bufcoded;
}

#define	chunkRawSize		96	// 32*3
#define	chunkEncodedSize	128	// 32*4

// ---------------------------------------------------------------------------
// WriteData														  [public]
// ---------------------------------------------------------------------------

void
RWTools::WriteData (FILE *fd, const SBlob &inData)
{
	const unsigned char *data = reinterpret_cast <const unsigned char*> (inData.fData);
	size_t				size = inData.fSize;
	char				buf [chunkEncodedSize + 2];
	int					chunk;

	for ( ; size > 0; data += chunkRawSize, size -= chunk)
	{
		chunk = size > chunkRawSize ? chunkRawSize : size;
		MyEncode (data, chunk, buf);
		fprintf (fd, "\t%s\r\n", buf);
	}

	return;
}


// ---------------------------------------------------------------------------
// WriteData														  [public]
// ---------------------------------------------------------------------------

void
RWTools::WriteData (XMLNode *inParent, const SBlob &inData)
{
	const unsigned char *data = reinterpret_cast <const unsigned char*> (inData.fData);
	size_t				size = inData.fSize;


	char        	buf [size * 4 / 3 + size / chunkRawSize + 5];
	char			*dst = buf;
	int				chunk;
	for ( ; size > 0; data += chunkRawSize, dst += chunkEncodedSize, size -= chunk)
	{
		if (dst != buf)
			*dst++ = ' ';
		chunk = size > chunkRawSize ? chunkRawSize : size;
		MyEncode (data, chunk, dst);
	}
	XMLText	*elem = inParent->ToDocument()->NewText(buf);
	inParent->InsertEndChild (elem);
	
	return;
}


// ---------------------------------------------------------------------------
// FindInList														  [public]
// ---------------------------------------------------------------------------

long
RWTools::FindInList (const CXMLText inValue, const char** inList)
{
	if (!inValue.empty() && inList)
	{
		long	index = 0;
		while (*inList)
		{
			if (TEXT_EQUALS (inValue, *inList))
				return index;
			index++;
			inList++;
		}
	}
	return -1;
}



/*--- function HTUU_decode ------------------------------------------------
 *
 *  Decode an ASCII-encoded buffer back to its original binary form.
 *
 *    Entry    bufcoded    points to a uuencoded string.  It is
 *                         terminated by any character not in
 *                         the printable character table six2pr, but
 *                         leading whitespace is stripped.
 *             bufplain    points to the output buffer; must be big
 *                         enough to hold the decoded string (generally
 *                         shorter than the encoded string) plus
 *                         as many as two extra bytes used during
 *                         the decoding process.
 *             outbufsize  is the maximum number of bytes that
 *                         can fit in bufplain.
 *
 *    Exit     Returns the number of binary bytes decoded.
 *             bufplain    contains these bytes.
 */

static	unsigned char	pr2six[256];

static int MyDecode (const unsigned char *&bufcoded, unsigned char *bufplain, long outbufsize)
{
/* single character decode */
#define DEC(c) pr2six[(int)c]
#define MAXVAL 63

	static int	first = 1;

	long		nbytesdecoded, j = (outbufsize*4)/3;

	/* If this is the first call, initialize the mapping table.
	* This code should work even on non-ASCII machines.
	*/
	if (first)
	{
		first = 0;
		for (j=0; j<256; j++)
			pr2six[j] = MAXVAL+1;

		for (j=0; j<64; j++)
			pr2six[(int)six2pr[j]] = (unsigned char) j;
	}

	/* Strip leading whitespace. */
	while (*bufcoded==' ' || *bufcoded == '\t' || *bufcoded == '\r' || *bufcoded == '\n')
		bufcoded++;

	/* Figure out how many characters are in the input buffer.
	* If this would decode into more bytes than would fit into
	* the output buffer, adjust the number of input bytes downwards.
	*/
	const unsigned char *bufin = bufcoded;
	long		nprbytes;

	j = (outbufsize*4)/3;
	while(j > 0 && pr2six[(int)*(bufin++)] <= MAXVAL)
		j--;
	nprbytes = bufin - bufcoded;
	if (j > 0)
		nprbytes--;
	nbytesdecoded = ((nprbytes+3)/4) * 3;
	if (nbytesdecoded > outbufsize)
		nprbytes = (outbufsize*4)/3;

	bufin = bufcoded;

	if (bufplain)
	{
		unsigned char *bufout = bufplain;
		while (nprbytes > 0)
		{
			*(bufout++) = (unsigned char) (DEC(*bufin) << 2 | DEC(bufin[1]) >> 4);
			*(bufout++) = (unsigned char) (DEC(bufin[1]) << 4 | DEC(bufin[2]) >> 2);
			*(bufout++) = (unsigned char) (DEC(bufin[2]) << 6 | DEC(bufin[3]));
			bufin += 4;
			nprbytes -= 4;
		}
	}
	else
	{
		bufin += ((nprbytes+3)/4) * 4;
		nprbytes -= ((nprbytes+3)/4) * 4;
	}

	if (nprbytes & 03)
	{
		if (pr2six[(int)bufin[-2]] > MAXVAL)
		{
			nbytesdecoded -= 2;
			bufin -= 2;
		}
		else
		{
			nbytesdecoded -= 1;
			bufin--;
		}
	}

	bufcoded = bufin;
	return (nbytesdecoded);
}


// ---------------------------------------------------------------------------
// ReadData															  [public]
// ---------------------------------------------------------------------------

void
RWTools::ReadData (const XMLElement *inNode, SBlob &outData)
{
	outData.Free();

	const XMLNode	*node;
	const XMLText	*text;
    const unsigned char* str, *cur;
	size_t			outSize = 0;
	int				size;
    const unsigned char *s1;
    
	for (node = inNode->FirstChild(); node != NULL; node = node->NextSibling())
	{
		text = node->ToText();
		if (text)
		{
            // ss = reinterpret_cast<const unsigned char *>(text->Value());
			str = (unsigned char *)text->Value();
            if (str && *str)
			{
				cur = str;

				for ( ; ; )
				{
					size = MyDecode ( cur, NULL, 64*1024*1024); //cur.c_str()
					if (size < 1)
						break;
					outSize += size;
				}
			}
		}
	}

	if (outSize != 0)
	{
		unsigned char	*data = reinterpret_cast <unsigned char*> (malloc (outSize + 2));
		if (data != NULL)
		{
			unsigned char	*dst = data;
			size_t			real_size = outSize;

			for (node = inNode->FirstChild(); outSize > 0 && node != NULL; node = node->NextSibling())
			{
				text = node->ToText();
				if (text)
				{
					str = (unsigned char *)(text->Value());
					if (str && *str)
					{
						for ( cur = str; outSize > 0; )
						{
                            size = MyDecode (cur, dst, outSize);
							if (size < 1)
								break;
							outSize -= size;
							dst += size;
						}
					}
				}
			}

			real_size -= outSize;
//			outVar.SetBlob (data, real_size, true);
			outData.fData = data;
			outData.fSize = real_size;
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// MakeMatrixFromUserRect											  [public]
// ---------------------------------------------------------------------------

#if	MACVER

struct	RWMacPrintTextContext
{
	float			fPageBottom;
	CGContextRef	fCGContext;
};

CGAffineTransform
RWTools::MakeMatrixFromUserRect (const RWPrintContextRef inContext, SRect &ioRect, float inAngle, float inWidth, float inHeight)
{
    RWMacPrintTextContext	*gc = reinterpret_cast <RWMacPrintTextContext*> (inContext);
	float	bottom = gc? gc->fPageBottom : 0.0;
	return MakeMatrixFromUserRect (ioRect, inAngle, bottom, inWidth, inHeight);
}

#else

CGAffineTransform
RWTools::MakeMatrixFromUserRect (const RWPrintContextRef inContext, SRect &ioRect, float inAngle, float inWidth, float inHeight)
{
	return MakeMatrixFromUserRect (ioRect, inAngle, 0, inWidth, inHeight);
}

#endif


CGAffineTransform
RWTools::MakeMatrixFromUserRect (SRect &ioRect, float inAngle, float inBottom, float inWidth, float inHeight)
{
#if	WINVER
//	inAngle = -inAngle;		//mbs 21052010	Gdiplus	//mbs 29062011	use RWPageComposer::GetNativeRotation()
#endif

	float	angle = Round (inAngle, RW_EPSILON);
	float	w = ioRect.Width();
	float	h = ioRect.Height();
	double	c = cos (inAngle * M_PI / 180);
	double	s = sin (inAngle * M_PI / 180);
	double	px = ioRect.left + w / 2;
#if	WINVER
	double	py = ioRect.top + h / 2;
#else
	double	py = (inBottom - ioRect.bottom) + h / 2;
#endif
	float	w1, h1;

//	if (180 - fabs (inAngle) < RW_EPSILON)				// inAngle == 180 || inAngle == -180)
	if ((int (fabs (angle)) % 180) == 0)				// inAngle == 180 || inAngle == -180)
	{
		w1 = w;
		h1 = h;
	}
//	else if (fabs (90 - fabs (inAngle)) < RW_EPSILON)	// inAngle == 90 || inAngle == -90
	else if ((int (fabs (angle)) % 90) == 0)			// inAngle == 90 || inAngle == -90)
	{
		w1 = h;
		h1 = w;
	}
	else
	{
		double	a = w / 2;
		double	b = h / 2;
		double	a1 = fabs (c*a - s*b);
		double	a2 = fabs (c*a + s*b);
		double	k1 = max (a1, a2) / a;
		double	b1 = fabs (s*a - c*b);
		double	b2 = fabs (s*a + c*b);
		double	k2 = max (b1, b2) / b;
		double	k = max (k1, k2);
		w1 = w / k;
		h1 = h / k;
	}

//printf ("RWTools::MakeMatrixFromUserRect ({%g,%g,%g,%g}, %g, %g) -> h=%g, w=%g; h1=%g, w1=%g\n",
//		ioRect.top, ioRect.left, ioRect.bottom, ioRect.right, inAngle, inBottom, h, w, h1, w1);

	ioRect.top += (h - h1) / 2;
	ioRect.left += (w - w1) / 2;
	if (false && inWidth != 0 && inHeight != 0)		//mbs 11062010	aj tak to nefunguje (pre ine ako 90*) a robi to bordel pri centrovani vertikalne...
	{
//		ioRect.top += (h - inHeight) / 2;
//		ioRect.left += (w - inWidth) / 2;
		ioRect.bottom = ioRect.top + inHeight;
		ioRect.right = ioRect.left + inWidth;
	}
	else
	{
		ioRect.bottom = ioRect.top + h1;
		ioRect.right = ioRect.left + w1;
	}
	CGAffineTransform	t = CGAffineTransformMake (c, s, -s, c, (1 - c)*px + s*py, -s*px + (1 - c)*py);
	return t;
}


// ---------------------------------------------------------------------------
// MakeUserRectFromText												  [public]
// ---------------------------------------------------------------------------

void
RWTools::MakeUserRectFromText (SRect &ioRect, float inAngle)
{
#if	WINVER
//	inAngle = -inAngle;		//mbs 21052010	Gdiplus	//mbs 29062011	use RWPageComposer::GetNativeRotation()
#endif

	float	angle = Round (-inAngle, RW_EPSILON);
	float	w = ioRect.Width();
	float	h = ioRect.Height();
	float	w1, h1;

	if ((int (fabs (angle)) % 180) == 0)				// inAngle == 180 || inAngle == -180)
	{
		w1 = w;
		h1 = h;
	}
	else if ((int (fabs (angle)) % 90) == 0)			// inAngle == 90 || inAngle == -90)
	{
		w1 = h;
		h1 = w;
	}
	else
	{
		double	c = cos (-inAngle * M_PI / 180);
		double	s = sin (-inAngle * M_PI / 180);
		double	a1 = fabs (c*w - s*h);
		double	a2 = fabs (c*w + s*h);
		w1 = max (a1, a2);
		double	b1 = fabs (s*w - c*h);
		double	b2 = fabs (s*w + c*h);
		h1 = max (b1, b2);
	}

//printf ("RWTools::MakeUserRectFromText ({%g,%g,%g,%g}, %g, %g) -> h=%g, w=%g; h1=%g, w1=%g\n",
//		ioRect.top, ioRect.left, ioRect.bottom, ioRect.right, inAngle, inBottom, h, w, h1, w1);

	ioRect.bottom = ioRect.top + h1;
	ioRect.right = ioRect.left + w1;

	return;
}


// ---------------------------------------------------------------------------
// EscapeAttributedString											  [public]
// ---------------------------------------------------------------------------
// used to convert non-attribute text to attributed text

CXMLText
RWTools::EscapeAttributedString (const CXMLText inAttributedString)
{
    CText ss = RWTextValue::UTF_8_to_UTF16(inAttributedString);
    return RWTextValue::UTF_16_to_UTF8(EscapeAttributedString(ss));
}

CText
RWTools::EscapeAttributedString (const CText inAttributedString)
{
	CText			as (inAttributedString);
	long			i;
	
	if (as.length() > 0)
	{
		for (i = 0; i < as.length(); i++)
		{
			switch (as[i]) {
				case '<':
				{
                    UTF16Char	et[] = { '&', 'l', 't', ';', 0 };
					as = as.erase(i, 1);
					as = as.insert(i, et);
					break;
				}
				case '>':
				{	
                    UTF16Char	et[] = { '&', 'g', 't', ';', 0 };
					as = as.erase(i, 1);
					as = as.insert(i, et);
					break;
				}
				case '&':
				{
                    UTF16Char	et[] = { '&', 'a', 'm', 'p', ';', 0 };
					as = as.erase(i, 1);
					as = as.insert(i, et);
					break;
				}
				case '"':
				{
                    UTF16Char	et[] = { '&', 'q', 'u', 'o', 't', ';', 0 };
					as = as.erase(i, 1);
					as = as.insert(i, et);
					break;
				}
				default:
					break;
			}
		}
	}
	return as;
}
// ---------------------------------------------------------------------------
// SplitAttributedString											  [public]
// ---------------------------------------------------------------------------
// on return, outAttributes contains array of pairs of long position <attribute string in source, text in destination>
// first pair is <size of array, length of destination text>

CText
RWTools::SplitAttributedString (const CText inAttributedString, unique_ptr<long> *outAttributes)
{
	CText			as (inAttributedString);
	vector<long>	v;
	long			i, j, aPosOffset = 0;

	if (as.length() > 0)
	{
		for (i = 0; i < as.length(); i++)
		{
			if (as[i] == '<')	// start of a tag
			{
		// can happen if text is switched from standard to attributed
				if (as[i + 1] == '%')	// <%variable; format%>
				{
                    UniChar	et[] = { '%', '>', 0 };
					j = as.find (et, i + 2);
					if (j == string::npos)
						break;
					i = j;
					continue;
				}

                j = as.find ('>', i + 1);
				if (j == string::npos)
					break;
				if (as [j - 1] == '/')	//mbs 19052010	support <BR/>
				{
					if (as [i+1] == 'B' && as [i+2] == 'R' && (as [i+3] == '/' || as [i+3] == ' ' || as [i+3] == '\t'))
					{
						aPosOffset += j - i;
						as.erase (i, j - i);
						as [i] = 0x000D;	// CR
					}
					else if (as [i+1] == 'b' && as [i+2] == 'r' && (as [i+3] == '/' || as [i+3] == ' ' || as [i+3] == '\t'))
					{
						aPosOffset += j - i;
						as.erase (i, j - i);
						as [i] = 0x000D;	// CR
					}
					else
					{
						aPosOffset += j - i + 1;
						as.erase (i, j - i + 1);
						i--;
					}
				}
				else
				{
					if (outAttributes)
					{
						v.push_back (i + aPosOffset + 1);	// first char after '<'
						v.push_back (i);
					}
					aPosOffset += j - i + 1;
					as.erase (i, j - i + 1);
					i--;
				}
			}
			else if (as[i] == '&')
			{
				// parse entity
				if (as [i+1] == '#' && as [i+2] == 'x' && i + 5 < as.length() && as [i+5] == ';')
				{
					unsigned char	value = 0;
					if (as [i+3] > '9')
						value = (9 + (as [i+3] & 0x07)) << 4;
					else
						value = (as [i+3] & 0x0F) << 4;
					if (as [i+4] > '9')
						value |= (9 + (as [i+4] & 0x07));
					else
						value |= (as [i+4] & 0x0F);
					as.erase (i, 5);
					as [i] = value;
					aPosOffset += 5;
				}
				else if (as [i+1] == 'a' && as [i+2] == 'm' && as [i+3] == 'p' && as [i+4] == ';')
				{
					as.erase (i + 1, 4);	//mbs 29072011	+1
//					as [i] = '&';
					aPosOffset += 4;
				}
				else if (as [i+1] == 'l' && as [i+2] == 't' && as [i+3] == ';')
				{
					as.erase (i, 3);
					as [i] = '<';
					aPosOffset += 3;
				}
				else if (as [i+1] == 'g' && as [i+2] == 't' && as [i+3] == ';')
				{
					as.erase (i, 3);
					as [i] = '>';
					aPosOffset += 3;
				}
				else if (as [i+1] == 'q' && as [i+2] == 'u' && as [i+3] == 'o' && as [i+4] == 't' && as [i+5] == ';')
				{
					as.erase (i, 5);
					as [i] = '\"';
					aPosOffset += 5;
				}
				else if (as [i+1] == 'a' && as [i+2] == 'p' && as [i+3] == 'o' && as [i+4] == 's' && as [i+5] == ';')
				{
					as.erase (i, 5);
					as [i] = '\'';
					aPosOffset += 5;
				}
			}
		}
	}

	if (outAttributes)
	{
		long	*l = static_cast <long*> (::operator new ((v.size() + 2) * sizeof (long)));
		outAttributes->reset (l);
		l [0] = v.size() + 2;
		l [1] = as.length();
		for (i = 0, j = v.size(); i < j; i++)
			l [i + 2] = v [i];
	}
	return as;
}



// ---------------------------------------------------------------------------
// ParseAttributedStringAttribute									  [public]
// ---------------------------------------------------------------------------
//mbs 22032010

const    UniChar         CR   = 0x000D;  // ASCII carrige return  '\r'
const    UniChar         LF   = 0x000A;  // ASCII newline         '\n'
const    UniChar         SP   = 0x0020;  // ASCII space         ' '
const    UniChar         NBSP = 0x00A0;  // Unicode non-breaking space
const    UniChar         LSEP = 0x2028;  // Unicode line separator
const    UniChar         PSEP = 0x2029;  // Unicode paragraph separator
static	bool	IsSpace (const UniChar c)
{
	return (c == SP || c == NBSP || c == CR || c == LF || c == LSEP || c == PSEP);
}

static	const UniChar* SkipWhiteSpace( const UniChar* p )
{
	if ( !p || !*p )
	{
		return 0;
	}
	while ( p && *p )
	{
		if ( IsSpace( *p ) )
			++p;
		else
			break;
	}

	return p;
}

static	const UniChar* ReadName( const UniChar* p, CText &name )
{
//	*name = "";
	// Names start with letters or underscores.
	// After that, they can be letters, underscores, numbers,
	// hyphens, or colons. (Colons are valid only for namespaces,
	// but tinyxml can't tell namespaces from names.)
	if (    p && *p
		 && ( ( *p >= 'a' && *p <= 'z' ) || ( *p >= 'A' && *p <= 'Z' ) || *p == '_' ) )
	{
		while(		p && *p
				&&	(		( *p >= 'a' && *p <= 'z' )
						 || ( *p >= 'A' && *p <= 'Z' )
						 || *p == '_'
						 || *p == '-' ) )
		{
			name += *p;
			++p;
		}
		return p;
	}
	return 0;
}

static	const UniChar	sHexa []	= {	'&', '#', 'x', 0	};
static	const UniChar	sAmp []		= {	'&', 'a', 'm', 'p', ';', 0	};
static	const UniChar	sLt []		= {	'&', 'l', 't', ';', 0	};
static	const UniChar	sGt []		= {	'&', 'g', 't', ';', 0	};
static	const UniChar	sQuot []	= {	'&', 'q', 'u', 'o', 't', ';', 0	};
static	const UniChar	sApos []	= {	'&', 'a', 'p', 'o', 's', ';', 0	};
static	struct	uEntity 	
{
	const UniChar*	str;
	unsigned int	strLength;
	UniChar			chr;
}	sEntity[] =
{
	{ sAmp, 	5, '&' },
	{ sLt,		4, '<' },
	{ sGt,		4, '>' },
	{ sQuot,	6, '\"' },
	{ sApos,	6, '\'' }
};

static	const UniChar* GetEntity( const UniChar* p, UniChar* value )
{
	// Presume an entity, and pull it out.
	int i;

	// Ignore the &#x entities.
	if (p [1] == '#' && p [2] == 'x' && p[3] && p[4] && p [5] == ';')
	{
        UniChar	value = 0;
		if (p [3] > '9')
			value = (9 + (p [3] & 0x07)) << 4;
		else
			value = (p [3] & 0x0F) << 4;
		if (p [4] > '9')
			value |= (9 + (p [4] & 0x07));
		else
			value |= (p [4] & 0x0F);
		return p+6;
	}

	// Now try to match it.
	for( i=0; i< (int) (sizeof (sEntity) / sizeof (sEntity[0])); ++i )
	{
		if ( memcmp( sEntity[i].str, p, sEntity[i].strLength * sizeof (UniChar) ) == 0 )
		{
			*value = sEntity[i].chr;
			return ( p + sEntity[i].strLength * sizeof (UniChar));
		}
	}

	// So it wasn't an entity, its unrecognized, or something like that.
	*value = *p;	// Don't put back the last one, since we return it!
	return p+1;
}

// Get a character, while interpreting entities.
inline static const UniChar* GetChar( const UniChar* p, UniChar* value )
{
	if ( *p == '&' )
	{
		return GetEntity( p, value );
	}
	else
	{
		*value = *p;
		return p+1;
	}
}

static	const UniChar* ReadText(	const UniChar* p,
									CText& text,
									const UniChar endTag)
{
	while (p && *p && *p != endTag)
	{
		UniChar	c;
		p = GetChar( p, &c );
        text += c;
	}
	if (p && *p)
		return p + 1;
	return p;
}

static	const UniChar* ReadText(	const UniChar* p,
									CText& text,
									const char* endTag)
{
    
	while (p && *p && not TEXT_STARTS_WITH (p, endTag))
	{
		UniChar	c;
		p = GetChar( p, &c );
        text += c;
	}
	if (p && *p)
		return p + strlen (endTag);
	return p;
}


// return value:
//	unhandled (e.g. </xxx>:	0
// for simple cases, the letter in uppercase:
//	B = outStyle is bold
//	I = outStyle is italic
//	U = outStyle is underline
//	S = outSize and outSizeSign contain size change information (size in points, sign is '+' or '-' for relative change, 0 for absolute one
//	C = outColor contains foreground color
//	F = outFont contains font name
// for 4D v12 <SPAN STYLE=...>, result is a bitfield:
// 256 = SPAN
//	1 = outFont contains 'font-family'
//	2 = outSize and outSizeSign contain 'font-size'
//	4 = outStyle's 'bold' contains 'font-weight' (bold or normal)
//	8 = outStyle's 'italic' contains 'font-style' (italic or normal)
//	16 = outStyle's 'underline' (and new unimplemented strike-through) contains 'text-decoration' (underline, none or line-through)
//	32 = outColor contains foreground 'color'
//	'text-align' and 'background-color' are ignored

int
RWTools::ParseAttributedStringAttribute (const CText inAttributedString, double &outSize, int &outSizeSign, int &outStyle, SRGBColor &outColor, CText &outFont)
{
    const UniChar * p = inAttributedString.c_str();

    if (!p || !*p)
		return false;
	p = SkipWhiteSpace (p);
    if (!p || !*p || *p == '/')
		return false;
    CText	name;
	p = ReadName (p, name);
    if (!p || !*p)
		return false;
	p = SkipWhiteSpace(p);
    if (!p || !*p)
		return false;
	
//	outSize = 0;
//	outSizeSign = 0;
//	outStyle = 0;
//	outColor = SRGBColor();
//	outFont.Free();

	if (TEXT_EQUALS(name, "b"))		// bold
	{
		outStyle = RWStyle::st_bold;
		return 'B';
	}
	else if (TEXT_EQUALS(name, "i"))	// italic
	{
		outStyle = RWStyle::st_italic;
		return 'I';
	}
	else if (TEXT_EQUALS(name, "u"))	// underline
	{
		outStyle = RWStyle::st_underline;
		return 'U';
	}
	else if (TEXT_EQUALS(name, "s"))	// size in points
	{
		outSize = 0;
		outSizeSign = 0;
		if (*p == '+' || *p == '-')
		{
			outSizeSign = *p;
			p++;
		}
		p = SkipWhiteSpace (p);
		if (!p || !*p)
			return false;
		for ( ; *p >= '0' && *p <= '9'; p++)
			outSize = outSize * 10 + (*p - '0');
		if (*p == '.')
		{
			float	base = 10;
			for (p++; *p >= '0' && *p <= '9'; p++)
			{
				outSize = outSize + (*p - '0') / base;
				base /= 10;
			}
		}
		return 'S';
	}
	else if (TEXT_EQUALS(name, "c"))	// color
	{
		outColor = SRGBColor (p);
		return 'C';
	}
	else if (TEXT_EQUALS(name, "f"))	// font
	{
		UniChar	end;
		CText	value;
		if ( *p == '\'' )
		{
			++p;
			end = '\'';
			p = ReadText( p, value, end );
		}
		else if ( *p == '"' )
		{
			++p;
			end = '\"';
			p = ReadText( p, value, end );
		}
		else
		{
//			value = "";
			while (    p && *p										// existence
					&& !IsSpace( *p ) && *p != '\n' && *p != '\r'	// whitespace
					&& *p != ';' && *p != '/' && *p != '>' )		// tag end
			{
				value += *p;
				++p;
			}
		}
		outFont = value;
		return 'F';
	}
	else if (TEXT_EQUALS(name, "SPAN"))	// 4D v12 <SPAN STYLE="...">
	{
		name.clear();
		p = ReadName (p, name);
		if (!p || !*p)
			return false;
		if (!TEXT_EQUALS(name, "STYLE"))
			return false;
		p = SkipWhiteSpace( p );
		if ( !p || *p != '=' )
			return false;
		++p;	// skip '='
		p = SkipWhiteSpace( p );
		if ( !p || !*p )
			return false;
		// read the STYLE attribute
		UniChar	end;
		if ( *p == '\'' )
			end = '\'';
		else if ( *p == '"' )
			end = '\"';
		else
			return false;
		++p;
		CText	avalue;
		p = ReadText( p, avalue, end );
		if ( !p || !*p )
			return false;

		int	result = 256;	//mbs 25052010
		//now parse the STYLE attributes
		p = avalue.c_str();
		do
		{
			p = SkipWhiteSpace (p);
			if (!p || !*p)
				return result;
			name.clear();
			p = ReadName (p, name);
			if (!p || !*p)
				return result;
			p = SkipWhiteSpace( p );
			if ( !p || *p != ':' )
				return result;
			++p;	// skip ':'
			p = SkipWhiteSpace( p );
			if ( !p || !*p )
				return result;

			CText	value;
			if ( *p == '\'' )
			{
				++p;
				end = '\'';
				p = ReadText( p, value, end );
			}
			else if ( *p == '"' )
			{
				++p;
				end = '\"';
				p = ReadText( p, value, end );
			}
			else
			{
//				value = "";
				while (    p && *p										// existence
						&& !IsSpace( *p ) && *p != '\n' && *p != '\r'	// whitespace
						&& *p != ';' && *p != '/' && *p != '>' )		// tag end
				{
					value += *p;
					++p;
				}
			}

			// now interpret the attribute
			if (TEXT_EQUALS(name, "font-family"))				// font-family : 'Arial'
			{
				outFont = value;
				result |= 1;
			}
			else if (TEXT_EQUALS(name, "font-size"))			// font-size : 24pt
			{
				const UniChar *	q = value.c_str();
				outSize = 0;
				outSizeSign = 0;
				if (*q == '+' || *q == '-')
				{
					outSizeSign = *q;
					q++;
				}
				q = SkipWhiteSpace (q);
				if (!q || !*q)
					return result;
				for ( ; *q >= '0' && *q <= '9'; q++)
					outSize = outSize * 10 + (*q - '0');
				if (*q == '.')
				{
					float	base = 10;
					for (q++; *q >= '0' && *q <= '9'; q++)
					{
						outSize = outSize + (*q - '0') / base;
						base /= 10;
					}
				}
				result |= 2;
			}
//			else if (name.IsEqualTo ("text-align"))			// text-align : left
//				;	// ignored
			else if (TEXT_EQUALS(name, "font-weight"))		// font-weight : bold
			{
				if (TEXT_EQUALS(name, "bold"))
					outStyle |= RWStyle::st_bold;
				else if (TEXT_EQUALS(name, "normal"))
					outStyle &= ~RWStyle::st_bold;
				result |= 4;
			}
			else if (TEXT_EQUALS(name, "font-style"))			// font-style : italic
			{
				if (TEXT_EQUALS(name, "italic"))
					outStyle |= RWStyle::st_italic;
				else if (TEXT_EQUALS(name, "normal"))
					outStyle &= ~RWStyle::st_italic;
				result |= 8;
			}
			else if (TEXT_EQUALS(name, "text-decoration"))	// text-decoration : underline
			{
				if (TEXT_EQUALS(name, "underline"))
					outStyle |= RWStyle::st_underline;
				else if (TEXT_EQUALS(name, "none"))
					outStyle &= ~RWStyle::st_underline;
				else if (TEXT_EQUALS(name, "line-through"))
					outStyle |= RWStyle::st_strikethrough;
				result |= 16;
			}
			else if (TEXT_EQUALS(name, "color"))				// color : #000000
			{
				outColor = SRGBColor (value);
				result |= 32;
			}
//			else if (name.IsEqualTo ("background-color"))	// background-color : #FFFFFF
//				;	// ignored
			if ( !p || !*p )
				return result;
			if (*p == ';')
				++p;
			else
				return result;
		} while (true);
	}
	else
		return false;
}

bool
RWTools::ParseTextForXLIFF (const CText inString, long inTextLen, CText &outText)
{
	CText		content (inString);
	CText		name;
		
	long			startPos;
	
	startPos = TEXT_STR(content, ":xliff:");
	if (startPos != string::npos)
	{
		const UniChar*	p = ReadText (content.c_str() + startPos + 7, name, "<");
		PA_Unistring	forTranslation = PA_CreateUnistring((PA_Unichar *)name.c_str());
		PA_Unistring	translated = PA_LocaliseString(PA_GetUnistring(&forTranslation), 0);
		outText.assign (PA_GetUnistring (&translated), PA_GetUnistringLength (&translated));
		PA_DisposeUnistring (&forTranslation);
		PA_DisposeUnistring (&translated);
		return true;

	}
	return false;
}

bool
RWTools::ParseTextForVar (bool inAttributed, const CText inString, long inTextLen, long &ioStart, long &outEnd, CText &outVarName, CText &outFormat)
{
	long	curPos, startPos, endPos, nameLen, fmtPos;

	if (inAttributed)
	{
		for (curPos = ioStart; (startPos = TEXT_STR (inString.c_str() + curPos, "&lt;%")) != string::npos && startPos < inTextLen; curPos = startPos)
		{
			startPos += curPos;
			CText	xmlText;
			const	UniChar*	p = ReadText (inString.c_str() + startPos + 5, xmlText, "%&gt;");
			if (p)
			{
				endPos = p - inString.c_str();
				nameLen = xmlText.length();
				if (nameLen > 0)
				{
					fmtPos = TEXT_STR (xmlText, ";");
					if (fmtPos != string::npos)
					{
						outFormat.assign (xmlText.c_str() + fmtPos + 1, nameLen - fmtPos - 1);
						nameLen = fmtPos;	//mbs 19052010	not -1
					}
					outVarName.assign (xmlText.c_str(), nameLen);
					ioStart = startPos;
					outEnd = endPos;
					return true;
				}
				else
					startPos = endPos;
			}
			else
				break;
		}
	}
	else
	{
		for (curPos = ioStart; (startPos = TEXT_STR (inString.c_str() + curPos, "<%")) != string::npos && startPos < inTextLen; curPos = startPos)
		{
			startPos += curPos;
			long	endPos = TEXT_STR (inString.c_str() + startPos + 2, "%>");
			if (endPos != string::npos)
			{
				endPos += startPos + 4;
				nameLen = endPos - startPos - 4;
				if (nameLen > 0)
				{
					for (fmtPos = startPos + 2; fmtPos < endPos - 1 && inString [fmtPos] != ';'; fmtPos++)
						;
					if (fmtPos < endPos - 1)
					{
						outFormat.assign (inString.c_str() + fmtPos + 1, endPos - fmtPos - 3);
						nameLen = fmtPos - startPos - 2;
					}
					outVarName.assign (inString.c_str() + startPos + 2, nameLen);
					ioStart = startPos;
					outEnd = endPos;
					return true;
				}
				else
					startPos = endPos + 2;
			}
			else
				break;
		}
	}

	return false;
}


#if	0
static string TestEncode (const UInt8 *ptr, long size)
{
	StPointerBlock	buf (size * 4 / 3 + 4);
	MyEncode (ptr, size, reinterpret_cast <char*> (buf.Get()));
	return string (reinterpret_cast <char*> (buf.Get()));
}

void RWTools_TestBase64 (void)
{
# define	kTestSize	16*1024*1024
	StPointerBlock	buf (kTestSize);
	StPointerBlock	decoded (kTestSize + 4);
	long	i;
	for (i = 0; i < kTestSize; i++)
		buf [i] = (UInt8) i;
	string	s;
	for (i = kTestSize - 256; i < kTestSize; i++)
	{
		s = TestEncode (buf, i+1);
		const char *cur = s.c_str();
		long	l = MyDecode (cur, NULL, 64*1024*1024);
		if (l != i + 1)
			printf ("MyDecode (NULL, %ld): %ld\n", i+1, l);
		cur = s.c_str();
		l = MyDecode (cur, decoded, i+1);
		if (l != i + 1)
			printf ("MyDecode (buf, %ld): %ld\n", i+1, l);
		if (memcmp (buf, decoded, i+1) != 0)
			printf ("memcmp (buf, decoded, %ld) failed\n", i+1);
	}
}
#endif
