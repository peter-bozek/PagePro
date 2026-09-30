# include	"RWBaseTypes.h"
# include	"RWStyle.h"
# include	"RWString4D.h"
# include	<cctype>
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


// ---------------------------------------------------------------------------
// Text form of SPoint / SRect
// ---------------------------------------------------------------------------

namespace
{
	// Read up to inCount numbers separated by inSeparator into outValues, in order;
	// returns how many were read (like the sscanf calls it replaces).
	int
	ReadNumbers (RWStringView inText, char16_t inSeparator, double *outValues, int inCount)
	{
		std::vector<RWString>	parts = RWStr::Split (inText, inSeparator);
		int						n = 0;
		for ( ; n < inCount && n < (int) parts.size(); n++)
		{
			std::optional<double>	value = RWStr::ToDouble (parts[n]);
			if (!value)
				break;
			outValues[n] = *value;
		}
		return n;
	}

	// "a;b;c" as written, older files may use "a,b,c"
	void
	ReadNumberList (RWStringView inText, double *outValues, int inCount)
	{
		for (int i = 0; i < inCount; i++)
			outValues[i] = 0;
		if (ReadNumbers (inText, u';', outValues, inCount) != inCount)
			ReadNumbers (inText, u',', outValues, inCount);
	}
}


SPoint&
SPoint::operator = (RWStringView inValue)
{
	double	values[2];
	ReadNumberList (inValue, values, 2);
	h = values[0];
	v = values[1];
	return *this;
}


RWString
SPoint::ToString (void) const
{
	return RWStr::Format ("%g;%g", h, v);
}


SRect&
SRect::operator = (RWStringView inValue)
{
	double	values[4];
	ReadNumberList (inValue, values, 4);
	left = values[0];
	top = values[1];
	right = values[2];
	bottom = values[3];
	return *this;
}


RWString
SRect::ToString (void) const
{
	return RWStr::Format ("%g;%g;%g;%g", left, top, right, bottom);
}


// ---------------------------------------------------------------------------
// Text form of SRGBColor
// ---------------------------------------------------------------------------

SRGBColor&
SRGBColor::operator = (RWStringView inValue)
{
	red = green = blue = 0;
	alpha = 0xFFFF;

	RWStringView	value = RWStr::Trim (inValue);
	if (value.empty())
		return *this;

	if (value[0] == u'#')								// #AARRGGBB, #RRGGBB
	{
		size_t	digits = 1;
		while (digits < value.size() && std::isxdigit (value[digits] < 0x80 ? int (value[digits]) : 0))
			digits++;
		digits--;
		if (digits >= 1 && digits <= 8)
		{
			std::optional<long long>	argb = RWStr::ToInteger (u"0x" + RWString (value.substr (1, digits)));
			*this = SRGBColor ((unsigned long) argb.value_or (0));
			if (digits < 7)
				alpha = 0xFFFF;
		}
	}
	else if ((value[0] >= u'a' && value[0] <= u'z') || (value[0] >= u'A' && value[0] <= u'Z'))
	{
		static	const	struct	{ const char16_t *name; const SRGBColor *color; }	sNames[] =
		{
			{ u"red", &cRedColor },			{ u"green", &cGreenColor },		{ u"blue", &cBlueColor },
			{ u"white", &cWhiteColor },		{ u"gray", &cGrayColor },		{ u"lightgray", &cLightGrayColor },
			{ u"transparent", &cEmptyColor },	{ u"cyan", &cCyanColor },	{ u"magenta", &cMagentaColor },
			{ u"yellow", &cYellowColor },	{ u"brown", &cBrownColor },		{ u"orange", &cOrangeColor },
			{ u"purple", &cPurpleColor }
		};

		*this = cBlackColor;							// unknown names, "black"
		for (const auto &entry : sNames)
		{
			if (RWStr::StartsWith (value, entry.name))
			{
				*this = *entry.color;
				break;
			}
		}
	}
	else if (value.find (u'.') != RWStringView::npos)	// r,g,b[,a] as 0..1 reals
	{
		double	c[4] = { 0, 0, 0, 1 };
		if (ReadNumbers (value, u',', c, 4) >= 3
		 &&	c[0] >= 0 && c[0] <= 1 && c[1] >= 0 && c[1] <= 1 && c[2] >= 0 && c[2] <= 1)
		{
			red = (unsigned short) (c[0] * 65535);
			green = (unsigned short) (c[1] * 65535);
			blue = (unsigned short) (c[2] * 65535);
			alpha = (unsigned short) (c[3] * 65535);
		}
	}
	else												// ARGB number, or r,g,b[,a] as 16 bit values
	{
		std::vector<RWString>	parts = RWStr::Split (value, u',');
		unsigned short			*components[4] = { &red, &green, &blue, &alpha };
		size_t					count = 0;
		for ( ; count < 4 && count < parts.size(); count++)
		{
			std::optional<long long>	number = RWStr::ToInteger (parts[count]);
			if (!number)
				break;
			*components[count] = (unsigned short) *number;
		}
		if (count == 1)
			*this = SRGBColor ((unsigned long) RWStr::ToInteger (parts[0]).value_or (0));
	}

	return *this;
}


RWString
SRGBColor::ToString (void) const
{
	return RWStr::Format ("#%08lx", (unsigned long) *this);
}


// ---------------------------------------------------------------------------
// RWValue
// ---------------------------------------------------------------------------

const char *	RWValue::sPictFormats[] = { "BLOB", "PICT", "PDF", "JPG", "PNG", "TIFF", "EMF", NULL };


RWValue::RWValue (EValue_Kind inKind, void* inData, size_t inSize)
	:	fKind (inKind),
		fOwn (false),
		fBlob ()
{
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
		fOwn (false),
		fBlob ()
{
	Clone (inOriginal);
}


void
RWValue::Free (void)
{
	if (fOwn)
	{
		fOwn = false;
		if (fKind == eValue_PictRefScreen)
		{
			if (fRef)
#if	VERSIONMAC
				::CFRelease (fRef);
#else
				delete reinterpret_cast <RWScreenPict> (fRef);
#endif
		}
		else if (fKind == eValue_PictRefPrint)
		{
			if (fRef)
#if	VERSIONMAC
				::CFRelease (fRef);
#else
				delete reinterpret_cast <RWPrintPict> (fRef);
#endif
		}
		else if (fKind >= eValue_BLOB)
		{
			if (fBlob.fData != NULL)
				free (fBlob.fData);
		}
	}
	fText.clear();
	fBlob.Init();
	fKind = eValue_Undefined;

	return;
}


// ---------------------------------------------------------------------------
// Attach
// ---------------------------------------------------------------------------
// share the original's data without owning it

RWValue&
RWValue::Attach (const RWValue &inOriginal)
{
	if (this != &inOriginal)
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

			case eValue_PictRefScreen:
			case eValue_PictRefPrint:
				fRef = inOriginal.fRef;
				break;

			case eValue_Real:
				fReal = inOriginal.fReal;
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
				fBlob = inOriginal.fBlob;
				break;

			default:
				fKind = eValue_Undefined;
				break;
		}
	}

	return *this;
}


// ---------------------------------------------------------------------------
// Detach
// ---------------------------------------------------------------------------
// take over the original's data, including ownership

RWValue&
RWValue::Detach (RWValue &inOriginal)
{
	if (this != &inOriginal)
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
				fInteger = inOriginal.fInteger;
				break;

			case eValue_PictRefScreen:
			case eValue_PictRefPrint:
				fRef = inOriginal.fRef;
				break;

			case eValue_Real:
				fReal = inOriginal.fReal;
				break;

			case eValue_Text:
				fText = std::move (inOriginal.fText);
				inOriginal.fText.clear();
				break;

			case eValue_BLOB:
			case eValue_PicturePICT:
			case eValue_PicturePDF:
			case eValue_PictureJPG:
			case eValue_PicturePNG:
			case eValue_PictureTIFF:
			case eValue_PictureEMF:
				fBlob = inOriginal.fBlob;
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


// ---------------------------------------------------------------------------
// Clone
// ---------------------------------------------------------------------------
// deep copy

RWValue&
RWValue::Clone (const RWValue &inOriginal)
{
	if (this != &inOriginal)
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

			case eValue_Text:
				fText = inOriginal.fText;
				break;

			case eValue_PictRefScreen:
			case eValue_PictRefPrint:
				fRef = inOriginal.fRef;
				if (fRef)
				{
#if	VERSIONMAC
					::CFRetain (fRef);
#else
					fRef = reinterpret_cast <Gdiplus::Image*> (fRef)->Clone();
#endif
					fOwn = true;
				}
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
				equal = (fInteger == inCompare.fInteger);
				break;

			case eValue_PictRefScreen:
			case eValue_PictRefPrint:
				equal = (fRef == inCompare.fRef);
				break;

			case eValue_Real:
				equal = (fReal == inCompare.fReal);
				break;

			case eValue_Text:
				equal = (fText == inCompare.fText);
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


bool
RWValue::IsEmpty (void)
const
{
	switch (fKind)
	{
		case eValue_Undefined:
			return true;

		case eValue_Text:
			return fText.empty();

		case eValue_PictRefScreen:
		case eValue_PictRefPrint:
			return fRef == NULL;

		default:
			if (fKind >= eValue_BLOB)
				return fBlob.fSize == 0 || fBlob.fData == NULL;
			return false;
	}
}


// ---------------------------------------------------------------------------
// GetTextValue
// ---------------------------------------------------------------------------
// fmt is a printf format for numbers, a replacement label for BLOBs and pictures.
// outValue is left unchanged for an empty label.

void
RWValue::GetTextValue (RWString &outValue, const char* fmt)
const
{
	const char	*label = NULL;

	switch (fKind)
	{
		case eValue_Undefined:
			label = "<NULL>";
			break;

		case eValue_Boolean:
			label = fInteger ? "1" : "0";
			break;

		case eValue_Integer:
			outValue = RWStr::Format (fmt ? fmt : "%ld", fInteger);
			break;

		case eValue_DateTime:
		{
			time_t		tim = (time_t) fInteger;
			struct tm	lt;
			char		buf[64];
#if	VERSIONWIN
			localtime_s (&lt, &tim);
#else
			localtime_r (&tim, &lt);
#endif
			size_t		len = strftime (buf, sizeof (buf), "%Y-%m-%dT%H:%M:%S%Z", &lt);
			outValue = RWStr::FromUTF8 (std::string_view (buf, len));
			break;
		}

		case eValue_Date:
			// day: 0 - 31 ==> 5 bits
			// month: 0 - 12 ==> 4 bits
			// day | (month << 5) | (year << 9)
			outValue = RWStr::Format ("%04ld-%02ld-%02ld", fInteger >> 9, (fInteger >> 5) & 0xF, fInteger & 0x1F);
			break;

		case eValue_Time:
			outValue = RWStr::Format ("%02ld.%02ld.%02ld", fInteger / 3600, fInteger / 60 % 60, fInteger % 60);
			break;

		case eValue_Real:
			outValue = RWStr::Format (fmt ? fmt : "%lg", fReal);
			break;

		case eValue_Text:
			outValue = fText;
			break;

		case eValue_BLOB:
			label = fmt ? fmt : "<BLOB>";
			break;

		case eValue_PictRefScreen:
#if	VERSIONMAC
			label = fmt ? fmt : "<CGImageRef>";
#else
			label = fmt ? fmt : "<Gdiplus::Bitmap*>";
#endif
			break;

		case eValue_PictRefPrint:
#if	VERSIONMAC
			label = fmt ? fmt : "<CGPDFDocumentRef>";
#else
			label = fmt ? fmt : "<Gdiplus::Metafile*>";
#endif
			break;

		case eValue_PicturePICT:	label = fmt ? fmt : "<IMAGE_PICT>";	break;
		case eValue_PicturePDF:		label = fmt ? fmt : "<IMAGE_PDF>";	break;
		case eValue_PictureJPG:		label = fmt ? fmt : "<IMAGE_JPG>";	break;
		case eValue_PicturePNG:		label = fmt ? fmt : "<IMAGE_PNG>";	break;
		case eValue_PictureTIFF:	label = fmt ? fmt : "<IMAGE_TIFF>";	break;
		case eValue_PictureEMF:		label = fmt ? fmt : "<IMAGE_EMF>";	break;
	}

	if (label && *label)
		outValue = RWStr::FromUTF8 (label);
	return;
}


// ---------------------------------------------------------------------------
// CoerceValue
// ---------------------------------------------------------------------------

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
			else if (fKind == eValue_Text)
				SetBoolean (!fText.empty() && !RWStr::Equals (fText, "0"));
			else
				return false;
			break;

		case eValue_Integer:
			if (fKind == eValue_Boolean)
				SetInteger (GetBoolean());
			else if (fKind == eValue_Real)
				SetInteger ((long) GetReal());
			else if (fKind == eValue_Text)
			{
				RWStringView	text = RWStr::Trim (fText);
				if (text.empty())
					SetInteger (0);
				else if (text[0] == u'#')		//mbs 12072010	special case for color
					SetInteger ((long) (unsigned long) SRGBColor (text));
				else if (std::optional<long long> value = RWStr::ToInteger (text))
					SetInteger ((long) *value);
				else
					return false;
			}
			else
				return false;
			break;

		case eValue_Real:
			if (fKind == eValue_Boolean)
				SetReal (GetBoolean());
			else if (fKind == eValue_Integer)
				SetReal (GetInteger());
			else if (fKind == eValue_Text)
			{
				// text that is not a number stays text (as before)
				if (std::optional<double> value = RWStr::ToDouble (fText))
					SetReal (*value);
			}
			else
				return false;
			break;

		case eValue_PictRefScreen:
		case eValue_PictRefPrint:
		case eValue_BLOB:
		case eValue_PicturePICT:
		case eValue_PicturePDF:
		case eValue_PictureJPG:
		case eValue_PicturePNG:
		case eValue_PictureTIFF:
		case eValue_PictureEMF:
			//••• TODO •••	convert image format...
			return false;

		default:
			return false;
	}

	return true;
}


// ---------------------------------------------------------------------------
// ParseIntoText													  [static][public]
// ---------------------------------------------------------------------------
// text content of an element: text nodes, nested <Data> elements, <NL/> as CR

RWString
RWTools::ParseIntoText (RWXmlNode inNode)
{
	RWString	result;

	for (RWXmlNode node = inNode.FirstChild(); node; node = node.NextSibling())
	{
		if (node.IsText())
			result += node.Value();
		else if (node.NameIs ("Data"))
			result += ParseIntoText (node);
		else if (node.NameIs ("NL"))
			result.push_back (u'\r');
	}

	return result;
}


// ---------------------------------------------------------------------------
// WriteText														  [static][public]
// ---------------------------------------------------------------------------
// XML parsers turn a CR in text into LF, so CR is written as <NL/>
// (read back by ParseIntoText) - 4D uses CR as line separator.

void
RWTools::WriteText (RWXmlNode inParent, RWStringView inText)
{
	size_t	start = 0;
	while (start < inText.size())
	{
		size_t	cr = inText.find (u'\r', start);
		if (cr == RWStringView::npos)
			cr = inText.size();
		if (cr > start)
			inParent.AppendText (inText.substr (start, cr - start));
		if (cr < inText.size())
			inParent.Append (u"NL");
		start = cr + 1;
	}

	return;
}


// ---------------------------------------------------------------------------
// ReadData / WriteData												  [static][public]
// ---------------------------------------------------------------------------
// BLOB as Base64 text content, in groups of 128 characters

void
RWTools::ReadData (RWXmlNode inNode, SBlob &outData)
{
	outData.Free();

	RWString	encoded;
	for (RWXmlNode node = inNode.FirstChild(); node; node = node.NextSibling())
		if (node.IsText())
			encoded += node.Value();

	std::vector<unsigned char>	data = RWStr::Base64Decode (encoded);
	if (!data.empty())
	{
		outData.fData = malloc (data.size());
		if (outData.fData != NULL)
		{
			memcpy (outData.fData, data.data(), data.size());
			outData.fSize = data.size();
		}
	}

	return;
}


void
RWTools::WriteData (RWXmlNode inParent, const SBlob &inData)
{
	if (inData.fData != NULL && inData.fSize != 0)
		inParent.AppendText (RWStr::Base64Encode (inData.fData, inData.fSize, 128));

	return;
}


// ---------------------------------------------------------------------------
// FindInList														  [static][public]
// ---------------------------------------------------------------------------

long
RWTools::FindInList (RWStringView inValue, const char** inList)
{
	if (!inValue.empty() && inList)
	{
		for (long index = 0; inList[index]; index++)
			if (RWStr::Equals (inValue, inList[index]))
				return index;
	}
	return -1;
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
// EscapeAttributedString											  [static][public]
// ---------------------------------------------------------------------------
// used to convert non-attribute text to attributed text

RWString
RWTools::EscapeAttributedString (RWStringView inText)
{
	RWString	result;
	result.reserve (inText.size());
	for (char16_t ch : inText)
	{
		switch (ch)
		{
			case u'<':	result.append (u"&lt;");	break;
			case u'>':	result.append (u"&gt;");	break;
			case u'&':	result.append (u"&amp;");	break;
			case u'"':	result.append (u"&quot;");	break;
			default:	result.push_back (ch);		break;
		}
	}
	return result;
}


// ---------------------------------------------------------------------------
// SplitAttributedString											  [static][public]
// ---------------------------------------------------------------------------
// Removes the tags and resolves the entities of attributed text.
// outAttributes: [0] = array size, [1] = length of the result, then pairs
// <position of the tag text in the source (first char after '<'), position in the result>

RWString
RWTools::SplitAttributedString (RWStringView inAttributedString, std::vector<long> *outAttributes)
{
	RWString			as (inAttributedString);
	std::vector<long>	v;
	long				aPosOffset = 0;
	auto				at = [&as] (long inIndex) -> char16_t { return inIndex >= 0 && size_t (inIndex) < as.size() ? as[inIndex] : 0; };

	for (long i = 0; size_t (i) < as.size(); i++)
	{
		if (as[i] == u'<')	// start of a tag
		{
			// can happen if text is switched from standard to attributed
			if (at (i + 1) == u'%')	// <%variable; format%>
			{
				size_t	j = as.find (u"%>", i + 2);
				if (j == RWString::npos)
					break;
				i = long (j);
				continue;
			}

			size_t	pos = as.find (u'>', i + 1);
			if (pos == RWString::npos)
				break;
			long	j = long (pos);

			if (as[j - 1] == u'/')	//mbs 19052010	support <BR/>
			{
				bool	isBR = ((at (i + 1) == u'B' && at (i + 2) == u'R') || (at (i + 1) == u'b' && at (i + 2) == u'r'))
							&& (at (i + 3) == u'/' || at (i + 3) == u' ' || at (i + 3) == u'\t');
				if (isBR)
				{
					aPosOffset += j - i;
					as.erase (i, j - i);
					as[i] = 0x000D;	// CR
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
		else if (as[i] == u'&')
		{
			// parse entity
			if (at (i + 1) == u'#' && at (i + 2) == u'x' && at (i + 5) == u';')
			{
				auto		hex = [] (char16_t c) -> unsigned { return c > u'9' ? 9 + (c & 0x07) : (c & 0x0F); };
				char16_t	value = char16_t (((hex (at (i + 3)) << 4) | hex (at (i + 4))) & 0xFF);
				as.erase (i, 5);
				as[i] = value;
				aPosOffset += 5;
			}
			else if (at (i + 1) == u'a' && at (i + 2) == u'm' && at (i + 3) == u'p' && at (i + 4) == u';')
			{
				as.erase (i + 1, 4);	//mbs 29072011	+1
				aPosOffset += 4;
			}
			else if (at (i + 1) == u'l' && at (i + 2) == u't' && at (i + 3) == u';')
			{
				as.erase (i, 3);
				as[i] = u'<';
				aPosOffset += 3;
			}
			else if (at (i + 1) == u'g' && at (i + 2) == u't' && at (i + 3) == u';')
			{
				as.erase (i, 3);
				as[i] = u'>';
				aPosOffset += 3;
			}
			else if (at (i + 1) == u'q' && at (i + 2) == u'u' && at (i + 3) == u'o' && at (i + 4) == u't' && at (i + 5) == u';')
			{
				as.erase (i, 5);
				as[i] = u'"';
				aPosOffset += 5;
			}
			else if (at (i + 1) == u'a' && at (i + 2) == u'p' && at (i + 3) == u'o' && at (i + 4) == u's' && at (i + 5) == u';')
			{
				as.erase (i, 5);
				as[i] = u'\'';
				aPosOffset += 5;
			}
		}
	}

	if (outAttributes)
	{
		outAttributes->clear();
		outAttributes->reserve (v.size() + 2);
		outAttributes->push_back (long (v.size() + 2));
		outAttributes->push_back (long (as.length()));
		outAttributes->insert (outAttributes->end(), v.begin(), v.end());
	}
	return as;
}


// ---------------------------------------------------------------------------
// Tag / style text scanning
// ---------------------------------------------------------------------------

namespace
{
	const	char16_t	CR   = 0x000D;	// ASCII carriage return
	const	char16_t	LF   = 0x000A;	// ASCII newline
	const	char16_t	SP   = 0x0020;	// ASCII space
	const	char16_t	NBSP = 0x00A0;	// Unicode non-breaking space
	const	char16_t	LSEP = 0x2028;	// Unicode line separator
	const	char16_t	PSEP = 0x2029;	// Unicode paragraph separator

	bool
	IsSpace (char16_t c)
	{
		return c == SP || c == NBSP || c == CR || c == LF || c == LSEP || c == PSEP;
	}

	unsigned
	HexValue (char16_t c)
	{
		if (c >= u'0' && c <= u'9')	return c - u'0';
		if (c >= u'a' && c <= u'f')	return c - u'a' + 10;
		if (c >= u'A' && c <= u'F')	return c - u'A' + 10;
		return 0;
	}

	bool
	IsHexDigit (char16_t c)
	{
		return (c >= u'0' && c <= u'9') || (c >= u'a' && c <= u'f') || (c >= u'A' && c <= u'F');
	}

	// read position in a text; an embedded NUL ends the text, as in the C string code it replaces
	struct	Cursor
	{
		RWStringView	text;
		size_t			pos = 0;

		explicit		Cursor (RWStringView inText, size_t inPos = 0) : text (inText), pos (inPos) {}

		char16_t		Peek (size_t inOffset = 0) const	{ return pos + inOffset < text.size() ? text[pos + inOffset] : 0; }
		bool			AtEnd (void) const					{ return Peek() == 0; }
		bool			StartsWith (RWStringView inPart) const	{ return pos <= text.size() && RWStr::StartsWith (text.substr (pos), inPart); }
	};

	void
	SkipWhiteSpace (Cursor &ioCursor)
	{
		while (!ioCursor.AtEnd() && IsSpace (ioCursor.Peek()))
			ioCursor.pos++;
	}

	// Names start with a letter or underscore, then letters, underscores or hyphens.
	bool
	ReadName (Cursor &ioCursor, RWString &outName)
	{
		char16_t	c = ioCursor.Peek();
		if (!((c >= u'a' && c <= u'z') || (c >= u'A' && c <= u'Z') || c == u'_'))
			return false;

		for (c = ioCursor.Peek(); (c >= u'a' && c <= u'z') || (c >= u'A' && c <= u'Z') || c == u'_' || c == u'-'; c = ioCursor.Peek())
		{
			outName.push_back (c);
			ioCursor.pos++;
		}
		return true;
	}

	// one character, resolving &#xHH; and the five XML entities
	char16_t
	GetChar (Cursor &ioCursor)
	{
		static	const	struct	{ const char16_t *entity; char16_t chr; }	sEntities[] =
		{
			{ u"&amp;", u'&' }, { u"&lt;", u'<' }, { u"&gt;", u'>' }, { u"&quot;", u'"' }, { u"&apos;", u'\'' }
		};

		char16_t	c = ioCursor.Peek();
		if (c == u'&')
		{
			if (ioCursor.Peek (1) == u'#' && ioCursor.Peek (2) == u'x'
			 &&	IsHexDigit (ioCursor.Peek (3)) && IsHexDigit (ioCursor.Peek (4)) && ioCursor.Peek (5) == u';')
			{
				c = char16_t ((HexValue (ioCursor.Peek (3)) << 4) | HexValue (ioCursor.Peek (4)));
				ioCursor.pos += 6;
				return c;
			}
			for (const auto &e : sEntities)
			{
				if (ioCursor.StartsWith (e.entity))
				{
					ioCursor.pos += std::char_traits<char16_t>::length (e.entity);
					return e.chr;
				}
			}
		}
		ioCursor.pos++;
		return c;
	}

	// read up to inEnd (resolving entities) and skip it; false if inEnd was not found
	bool
	ReadText (Cursor &ioCursor, RWString &outText, char16_t inEnd)
	{
		while (!ioCursor.AtEnd() && ioCursor.Peek() != inEnd)
			outText.push_back (GetChar (ioCursor));
		if (ioCursor.AtEnd())
			return false;
		ioCursor.pos++;
		return true;
	}

	bool
	ReadText (Cursor &ioCursor, RWString &outText, RWStringView inEnd)
	{
		while (!ioCursor.AtEnd() && !ioCursor.StartsWith (inEnd))
			outText.push_back (GetChar (ioCursor));
		if (ioCursor.AtEnd())
			return false;
		ioCursor.pos += inEnd.size();
		return true;
	}

	// a style value: quoted, or up to white space / ';' / '/' / '>'
	RWString
	ReadValue (Cursor &ioCursor)
	{
		RWString	value;
		char16_t	quote = ioCursor.Peek();
		if (quote == u'\'' || quote == u'"')
		{
			ioCursor.pos++;
			ReadText (ioCursor, value, quote);
		}
		else
		{
			for (char16_t c = ioCursor.Peek(); c != 0 && !IsSpace (c) && c != u';' && c != u'/' && c != u'>'; c = ioCursor.Peek())
			{
				value.push_back (c);
				ioCursor.pos++;
			}
		}
		return value;
	}

	// "[+|-] 12.5" - size in points, optional relative sign
	void
	ReadSize (Cursor &ioCursor, double &outSize, int &outSizeSign)
	{
		outSize = 0;
		outSizeSign = 0;
		if (ioCursor.Peek() == u'+' || ioCursor.Peek() == u'-')
		{
			outSizeSign = ioCursor.Peek();
			ioCursor.pos++;
		}
		SkipWhiteSpace (ioCursor);
		for ( ; ioCursor.Peek() >= u'0' && ioCursor.Peek() <= u'9'; ioCursor.pos++)
			outSize = outSize * 10 + (ioCursor.Peek() - u'0');
		if (ioCursor.Peek() == u'.')
		{
			double	base = 10;
			for (ioCursor.pos++; ioCursor.Peek() >= u'0' && ioCursor.Peek() <= u'9'; ioCursor.pos++)
			{
				outSize += (ioCursor.Peek() - u'0') / base;
				base *= 10;
			}
		}
	}
}


// ---------------------------------------------------------------------------
// ParseAttributedStringAttribute									  [static][public]
// ---------------------------------------------------------------------------
//mbs 22032010
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
RWTools::ParseAttributedStringAttribute (RWStringView inAttributedString, double &outSize, int &outSizeSign, int &outStyle, SRGBColor &outColor, RWString &outFont)
{
	Cursor		p (inAttributedString);
	RWString	name;

	SkipWhiteSpace (p);
	if (p.AtEnd() || p.Peek() == u'/')
		return 0;
	if (!ReadName (p, name))
		return 0;
	SkipWhiteSpace (p);
	if (p.AtEnd())
		return 0;

	if (RWStr::Equals (name, "b"))			// bold
	{
		outStyle = RWStyle::st_bold;
		return 'B';
	}
	else if (RWStr::Equals (name, "i"))		// italic
	{
		outStyle = RWStyle::st_italic;
		return 'I';
	}
	else if (RWStr::Equals (name, "u"))		// underline
	{
		outStyle = RWStyle::st_underline;
		return 'U';
	}
	else if (RWStr::Equals (name, "s"))		// size in points
	{
		ReadSize (p, outSize, outSizeSign);
		return 'S';
	}
	else if (RWStr::Equals (name, "c"))		// color
	{
		outColor = SRGBColor (p.text.substr (p.pos));
		return 'C';
	}
	else if (RWStr::Equals (name, "f"))		// font
	{
		outFont = ReadValue (p);
		return 'F';
	}
	else if (RWStr::Equals (name, "SPAN"))	// 4D v12 <SPAN STYLE="...">
	{
		name.clear();
		if (!ReadName (p, name) || p.AtEnd() || !RWStr::Equals (name, "STYLE"))
			return 0;
		SkipWhiteSpace (p);
		if (p.Peek() != u'=')
			return 0;
		p.pos++;
		SkipWhiteSpace (p);

		// read the STYLE attribute
		char16_t	quote = p.Peek();
		if (quote != u'\'' && quote != u'"')
			return 0;
		p.pos++;
		RWString	style;
		if (!ReadText (p, style, quote) || p.AtEnd())
			return 0;

		int		result = 256;	//mbs 25052010
		Cursor	s (style);
		for (;;)
		{
			SkipWhiteSpace (s);
			name.clear();
			if (s.AtEnd() || !ReadName (s, name) || s.AtEnd())
				return result;
			SkipWhiteSpace (s);
			if (s.Peek() != u':')
				return result;
			s.pos++;	// skip ':'
			SkipWhiteSpace (s);
			if (s.AtEnd())
				return result;

			RWString	value = ReadValue (s);

			if (RWStr::Equals (name, "font-family"))				// font-family : 'Arial'
			{
				outFont = value;
				result |= 1;
			}
			else if (RWStr::Equals (name, "font-size"))				// font-size : 24pt
			{
				Cursor	q (value);
				ReadSize (q, outSize, outSizeSign);
				result |= 2;
			}
			else if (RWStr::Equals (name, "font-weight"))			// font-weight : bold
			{
				if (RWStr::Equals (value, "bold"))
					outStyle |= RWStyle::st_bold;
				else if (RWStr::Equals (value, "normal"))
					outStyle &= ~RWStyle::st_bold;
				result |= 4;
			}
			else if (RWStr::Equals (name, "font-style"))			// font-style : italic
			{
				if (RWStr::Equals (value, "italic"))
					outStyle |= RWStyle::st_italic;
				else if (RWStr::Equals (value, "normal"))
					outStyle &= ~RWStyle::st_italic;
				result |= 8;
			}
			else if (RWStr::Equals (name, "text-decoration"))		// text-decoration : underline
			{
				if (RWStr::Equals (value, "underline"))
					outStyle |= RWStyle::st_underline;
				else if (RWStr::Equals (value, "none"))
					outStyle &= ~RWStyle::st_underline;
				else if (RWStr::Equals (value, "line-through"))
					outStyle |= RWStyle::st_strikethrough;
				result |= 16;
			}
			else if (RWStr::Equals (name, "color"))					// color : #000000
			{
				outColor = SRGBColor (value);
				result |= 32;
			}
			// text-align, background-color: ignored

			SkipWhiteSpace (s);
			if (s.Peek() != u';')
				return result;
			s.pos++;
		}
	}

	return 0;
}


// ---------------------------------------------------------------------------
// ParseTextForXLIFF												  [static][public]
// ---------------------------------------------------------------------------
// ":xliff:resname" (up to '<') is replaced by the localized string

bool
RWTools::ParseTextForXLIFF (RWStringView inString, long, RWString &outText)
{
	size_t	startPos = inString.find (u":xliff:");
	if (startPos == RWStringView::npos)
		return false;

	RWString	name;
	Cursor		p (inString, startPos + 7);
	ReadText (p, name, u'<');

	PA_Unistring	translated = PA_LocaliseString (RWStr::ToPA (name), 0);
	outText = RWStr::FromPA (&translated);
	PA_DisposeUnistring (&translated);
	return true;
}


// ---------------------------------------------------------------------------
// ParseTextForVar													  [static][public]
// ---------------------------------------------------------------------------
// Finds the next <%name;format%> (or &lt;%name;format%&gt; in attributed text)
// at or after ioStart and before inTextLen. On success ioStart / outEnd delimit
// the whole reference, outFormat is empty when there is no format.

bool
RWTools::ParseTextForVar (bool inAttributed, RWStringView inString, long inTextLen, long &ioStart, long &outEnd, RWString &outVarName, RWString &outFormat)
{
	size_t	limit = std::min (size_t (std::max (inTextLen, 0L)), inString.size());
	size_t	curPos = size_t (std::max (ioStart, 0L));

	if (inAttributed)
	{
		for (size_t startPos; (startPos = inString.find (u"&lt;%", curPos)) != RWStringView::npos && startPos < limit; curPos = startPos)
		{
			RWString	xmlText;
			Cursor		p (inString, startPos + 5);
			if (!ReadText (p, xmlText, u"%&gt;"))
				break;			// not terminated

			size_t	endPos = p.pos;
			if (!xmlText.empty())
			{
				size_t	fmtPos = xmlText.find (u';');
				outFormat = fmtPos != RWString::npos ? xmlText.substr (fmtPos + 1) : RWString();
				outVarName = xmlText.substr (0, fmtPos);	//mbs 19052010	not -1
				ioStart = long (startPos);
				outEnd = long (endPos);
				return true;
			}
			startPos = endPos;
		}
	}
	else
	{
		for (size_t startPos; (startPos = inString.find (u"<%", curPos)) != RWStringView::npos && startPos < limit; curPos = startPos)
		{
			size_t	close = inString.find (u"%>", startPos + 2);
			if (close == RWStringView::npos)
				break;

			size_t	endPos = close + 2;
			if (close > startPos + 2)
			{
				RWStringView	content = inString.substr (startPos + 2, close - startPos - 2);
				size_t			fmtPos = content.find (u';');
				outFormat = fmtPos != RWStringView::npos ? RWString (content.substr (fmtPos + 1)) : RWString();
				outVarName = RWString (content.substr (0, fmtPos));
				ioStart = long (startPos);
				outEnd = long (endPos);
				return true;
			}
			startPos = endPos;
		}
	}

	return false;
}
