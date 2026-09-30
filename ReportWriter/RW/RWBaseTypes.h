#ifndef	_RWBaseTypes_h_
# define	_RWBaseTypes_h_

#if	!defined(USE_MAC_TYPES) && VERSIONMAC
# define	USE_MAC_TYPES	1
#endif

# include   "4DPluginAPI.h"
# include	"tinyxml2.h"
# include	<math.h>
# include   <string>         // std::string
# include   <format>

#if	WINVER
namespace	Gdiplus
{
	class	Metafile;
	class	Bitmap;
}
#endif

#if	USE_MAC_TYPES
# include	<Carbon/Carbon.h>
#endif

#if	VERSIONWIN
struct ATSURGBAlphaColor {
  float               red;
  float               green;
  float               blue;
  float               alpha;
};
#endif

typedef	unsigned long	TextEncoding;

struct QDPoint {
  short               v;
  short               h;
};


# include	<time.h>
# include	<vector>
# include	<map>
# include	<iterator>

# define	RWList		std::vector
# define	RWMap		std::map

// SBLOB definition
struct	_SBlob
{
	void		*fData;
	size_t		fSize;
};
struct	SBlob	:	public	_SBlob
{
	inline			void			Init (void);
	inline			void			Free (void);
	inline							operator bool (void) const;
};

inline	void	SBlob::Init (void)					{ fData = 0; fSize = 0; }
inline	void	SBlob::Free (void)					{ if (fData) { free (fData); Init(); } }
inline			SBlob::operator bool (void) const	{ return fData != 0 && fSize != 0; }

// RWValue definition
class	RWValue;
typedef	RWValue				RWPicture;
typedef	unsigned long		RWDataID;

enum	EPictFormat
{
	ePictFormat_First = 0,

	ePictFormat_Normal				=	0,	// 4 = Truncated (non-centered)
	ePictFormat_Centered			=	1,	// 1 = Truncated (centered)
	ePictFormat_ScaledToFit			=	2,	// 2 = Scaled to fit
	ePictFormat_ScaledProp			=	3,	// 5 = Scaled to fit (proportional)
	ePictFormat_ScaledPropCentered	=	4,	// 6 = Scaled to fit centered (prop.)

	ePictFormat_Last
};

#if	VERSIONMAC
    #import "CoreGraphics/CoreGraphics.h"
	typedef	CGImageRef				    RWScreenPict;
	typedef	CGPDFDocumentRef		    RWPrintPict;
#else
	 typedef	Gdiplus::Bitmap*		RWScreenPict;
	 typedef	Gdiplus::Metafile*		RWPrintPict;
#endif


//encoding of strings used by RW
// currenty we use UTF-16 encoded strings
typedef std::basic_string<PA_Unichar>           CUTF16String;
typedef std::string                             CUTF8String;

typedef	UniChar			                        CChar;
typedef	std::basic_string<UniChar>              CText;
#define	CChar_Size			                    2

//encoding of strings used by XML
// currently we use UTF-8 thru TiniXml ==> char
typedef std::string                              CXMLText;

inline    bool    TEXT_EQUALS (const CXMLText s1, const char *s2)         {return s1.compare(s2) == 0; }
inline    bool    TEXT_STARTS_WITH (const CXMLText s1, const char *s2)    { return s1.find(s2) == 0; }
inline    long    TEXT_STR (const CXMLText s1, const char *s2)            { return s1.find(s2); }

#define	STR_NOTFOUND	(-1L)

# define    STR_COMPARE(s1,s2)            strcasecmp (s1.c_str(), s2)
# define    STR_EQUALS(s1,s2)            (strcasecmp (s1.c_str(), s2) == 0)
# define    STR_STARTS_WITH(s1,s2)        (strncasecmp (s1.c_str(), s2, strlen (s2)) == 0)

class	RWTextValue
{
public:
inline						RWTextValue (void);
inline						RWTextValue (const RWTextValue &inRhs);
inline						RWTextValue (const char *value);		// ASCII
// inline						RWTextValue (const UTF8Char *value);	// UTF8

inline						RWTextValue (const CText value);			// UTF16
inline						~RWTextValue (void);

inline						operator const CText (void) const;
inline						operator CText (void);
    
    
    
inline	RWTextValue&		operator = (const char *value);
// inline	RWTextValue&		operator = (const UTF8Char *value);

inline	RWTextValue&		operator = (const CText value);

inline	RWTextValue&		operator = (const RWTextValue &value);

inline	bool				IsEmpty (void) const;
inline	void				Free (void);
inline	void				Allocate (size_t len);
inline	size_t				StrLength (void) const;
inline  bool                equal (const CText string2) const;
inline  int                 compare (const CText string2) const;
inline  bool                empty () const;
    
		RWTextValue&		Copy (const char *value);
//		RWTextValue&		Copy (const UTF8Char *value);
        RWTextValue&		Copy (const CText &value);
    
// inline	RWTextValue&		Attach (CText &value);
inline	RWTextValue&		Attach (CText value);
inline	CText				Detach (void);
    
inline	RWTextValue&		FromXML (const std::string &value);
// inline	RWTextValue&		FromXML (const CXMLText value);
        CXMLText			ToXML (void) const;
        CXMLText			ToXMLEscaped (void) const;
    
static	void				FreeXML (CXMLText value);
    
    
    // static utility methods
    static  CXMLText        UTF_16_to_UTF8(const CText value);
    static  CText           UTF_8_to_UTF16(const CXMLText value);

protected:
	CText			data_;
};


inline	RWTextValue::RWTextValue (void)
	:	data_ (0)
{
}

inline	RWTextValue::RWTextValue (const RWTextValue &inRhs)
{
    data_ = inRhs.data_;
}

inline	RWTextValue::RWTextValue (const char *value)
{
	Copy (value);
}

/*inline	RWTextValue::RWTextValue (const UTF8Char *value)
{
	Copy (value);
}*/

inline	RWTextValue::RWTextValue (const CText value)
{
    data_ = value;
}

inline	RWTextValue::~RWTextValue (void)
{
	Free();
}

inline	RWTextValue::operator const CText (void)
const
{
	return data_;
}

inline	RWTextValue::operator CText (void)
{
	return data_;
}

inline	RWTextValue&	RWTextValue::operator = (const char *value)
{
	return Copy (value);
}

/*inline	RWTextValue&	RWTextValue::operator = (const UTF8Char *value)
{
	return Copy (value);
}*/


inline	RWTextValue&	RWTextValue::operator = (const CText value)
{
    data_ = value;
    return *this;
}

inline	RWTextValue&	RWTextValue::operator = (const RWTextValue &value)
{
	return Copy (value.data_);
}

inline	bool	RWTextValue::IsEmpty (void) const
{
	return data_.empty();
}

inline	void	RWTextValue::Free (void)
{
	if (!data_.empty())
	{
		data_.clear();
	}
	return;
}

inline	void	RWTextValue::Allocate (size_t len)
{
	Free();
    data_.resize(len);
	return;
}

inline	size_t	RWTextValue::StrLength (void) const
{
        return (data_).length();
}

inline    bool    RWTextValue::equal (const CText string2) const
{
        return data_.compare(string2) == 0;
}

inline    int    RWTextValue::compare (const CText string2) const
{
        return data_.compare(string2);
}

inline    bool    RWTextValue::empty () const
{
        return data_.empty();
}

/*inline	RWTextValue&	RWTextValue::Attach (CText &value)
{
	Free();
	data_ = value;
    value.clear();
	return *this;
}*/

inline	RWTextValue&	RWTextValue::Attach (CText value)
{
	Free();
	data_ = value;
	return *this;
}

inline	CText	RWTextValue::Detach (void)
{
	CText	value = data_;
	data_.clear();
	return value;
}

inline	RWTextValue&	RWTextValue::FromXML (const CXMLText &value)
{
	return Copy (value.c_str());
}

/*
inline	RWTextValue&	RWTextValue::FromXML (const CXMLText value)
{
	return Copy (reinterpret_cast <const UTF8Char*> (value.c_str()));
}
*/

inline    bool    TEXT_EQUALS (const UniChar* p, const char *s2)
{
    std::basic_string<UniChar> s1 = p;
    std::basic_string<UniChar> s3 = RWTextValue::UTF_8_to_UTF16(s2);
    return s1.compare(s3) == 0;
}

inline    bool    TEXT_STARTS_WITH (const UniChar* p, const char *s2)
{
    std::basic_string<UniChar> s1 = p;
    std::basic_string<UniChar> s3 = RWTextValue::UTF_8_to_UTF16(s2);
    return s1.compare(0, s3.length(), s3) == 0;
    
}
inline    long    TEXT_STR (const UniChar* p, const char *s2)
{
    std::basic_string<UniChar> s1 = p;
    std::basic_string<UniChar> s3 = RWTextValue::UTF_8_to_UTF16(s2);
    return s1.find(s3);
}

inline    bool    TEXT_EQUALS (const CText s1, const char *s2)
{
    std::basic_string<UniChar> s3 = RWTextValue::UTF_8_to_UTF16(s2);
    return s1.compare(s3) == 0;
}

inline    bool    TEXT_STARTS_WITH (const CText s1, const char *s2)
{
    std::basic_string<UniChar> s3 = RWTextValue::UTF_8_to_UTF16(s2);
    return s1.compare(0, s3.length(), s3) == 0;
    
}
inline    long    TEXT_STR (const CText s1, const char *s2)
{
    std::basic_string<UniChar> s3 = RWTextValue::UTF_8_to_UTF16(s2);
    return s1.find(s3);
}

inline	CXMLText	RWTextValue::ToXML (void) const
{
    return   RWTextValue::UTF_16_to_UTF8 (data_);
}

inline	CXMLText	RWTextValue::ToXMLEscaped (void) const
{
    if (!data_.empty())
    {
        CXMLText escaped;
        CXMLText sUTF8 =  RWTextValue::UTF_16_to_UTF8 (data_);
        int i = 0;
        while (sUTF8[i]) {
            switch (sUTF8[i]) {
                case '&':
                    escaped.append("&amp;");
                    break;
                case '>':
                    escaped.append("&gt;");
                    break;
                case '<':
                    escaped.append("&lt;");
                    break;
                case '"':
                    escaped.append("&quot;");
                    break;
                case '\'':
                    escaped.append("&apos;");
                    break;
                
                default:
                    escaped.append(&sUTF8[i]);
                    break;
            }
            i++;
        }
        return escaped;
    }
    return NULL;
}

inline	void	RWTextValue::FreeXML (CXMLText value)
{
	value.erase();
    return;
}



# define	RW_EPSILON			1e-5

short	RoundToShort (double f);		// round f to the nearest short

enum	RWLine_Flags
{
	RWLine_Horizontal	= 0x00,
	RWLine_Vertical,	// 1
	RWLine_TopLeft,		// 2
	RWLine_BottomLeft,	// 3
	RWLine_Full			// 4
};

enum	RWRect_Flags	// it is really a bitfield...
{
	RWRect_Top		= 0x01,
	RWRect_Left		= 0x02,
	RWRect_Bottom	= 0x04,
	RWRect_Right	= 0x08,
	RWRect_Full		= 0x0F
};


// point (point 0,0 is at top/left)
struct	_SPoint
{
	double		h;
	double		v;
};


inline	bool operator == (const _SPoint inLh, const _SPoint inRh)
{
	return fabs (inLh.h - inRh.h) < RW_EPSILON && fabs (inLh.v - inRh.v) < RW_EPSILON;
}

inline	bool operator != (const _SPoint inLh, const _SPoint inRh)
{
	return not operator == (inLh, inRh);
}


// rectangle (point 0,0 is at top/left)
struct	_SRect
{
	double		top;
	double		left;
	double		bottom;
	double		right;
};


inline	bool operator == (const _SRect inLh, const _SRect inRh)
{
	return fabs (inLh.top - inRh.top) < RW_EPSILON && fabs (inLh.left - inRh.left) < RW_EPSILON && fabs (inLh.bottom - inRh.bottom) < RW_EPSILON && fabs (inLh.right - inRh.right) < RW_EPSILON;
}

inline	bool operator != (const _SRect inLh, const _SRect inRh)
{
	return not operator == (inLh, inRh);
}


struct	SPoint	:	public	_SPoint
{
	inline				SPoint (void);
	inline				SPoint (double x, double y);
	inline				SPoint (int x, int y);
	inline		SPoint	operator + (const _SPoint rh);
	inline		SPoint	operator - (const _SPoint rh);
	inline		bool	IsContained (const _SRect &rh) const;

//#if	USE_MAC_TYPES
	inline				SPoint (QDPoint p);
						operator QDPoint (void) const;
//#endif
				SPoint&	operator = (const char *inValue);
						operator const char* (void) const;
};

inline	SPoint::SPoint (void)
{
}

inline	SPoint::SPoint (double x, double y)
{
	h = x;
	v = y;
}

inline	SPoint::SPoint (int x, int y)
{
	h = x;
	v = y;
}

inline	SPoint::SPoint (const QDPoint p)
{
	h = p.h;
	v = p.v;
}

inline		SPoint	SPoint::operator + (const _SPoint rh)
{
	SPoint	offp (h + rh.h, v + rh.v);
	return offp;
}

inline		SPoint	SPoint::operator - (const _SPoint rh)
{
	SPoint	offp (h - rh.h, v - rh.v);
	return offp;
}


// rectangle (point 0,0 is at top/left)
struct	SRect	:	public	_SRect
{
inline				SRect (void);
inline				SRect (double t, double l, double b, double r);
inline				SRect (int t, int l, int b, int r);
inline		SRect	operator + (const _SPoint rh) const;	// offset
inline		SRect&	operator += (const _SPoint rh);			// offset
inline		SRect	operator - (const _SPoint rh) const;	// offset
inline		SRect&	operator -= (const _SPoint rh);			// offset
inline		SRect&	operator *= (const _SRect &rh);			// offset with different values
//inline		SRect&	operator += (const SRect &rh);		// offsets only top/left
inline		bool	operator & (const _SRect &rh) const;	// intersection
inline		SRect&	operator &= (const _SRect &rh);			// intersection
inline		SRect&	operator /= (const _SRect &rh);			// intersection without top
inline		SRect&	operator *= (double inset);				// inset
inline		bool	IsEmpty (void) const;
inline		bool	Contains (const _SRect &rh) const;
inline		double	Width (void) const;
inline		double	Height (void) const;
inline		SPoint	TopLeft (void) const;
inline		SPoint	BottomRight (void) const;
inline		void	SetRect (double t, double l, double b, double r);
inline		void	SetRect (int t, int l, int b, int r);

/* inline				SRect (const Rect &r);
					operator Rect (void) const; */
    SRect&    operator = (const char *inValue);
    SRect&    operator = (const CXMLText *inValue);
					operator const char* (void) const;
};


inline		bool	SPoint::IsContained (const _SRect &rh) const
{
	return rh.top <= v && rh.left <= h && rh.bottom >= v && rh.right >= h;
}

inline	SRect::SRect (void)
{
}

inline	SRect::SRect (double t, double l, double b, double r)
{
	top = t;
	left = l;
	bottom = b;
	right = r;
}

inline	SRect::SRect (int t, int l, int b, int r)
{
	top = t;
	left = l;
	bottom = b;
	right = r;
}

#if	USE_MAC_TYPES
inline	SRect::SRect (const Rect &r)
{
	top = r.top;
	left = r.left;
	bottom = r.bottom;
	right = r.right;
}
#endif

inline		SRect	SRect::operator + (const _SPoint rh)
const
{
	SRect	offr (top + rh.v, left + rh.h, bottom + rh.v, right + rh.h);
	return offr;
}

inline		SRect&	SRect::operator += (const _SPoint rh)
{
	top += rh.v;
	left += rh.h;
	bottom += rh.v;
	right += rh.h;
	return *this;
}

inline		SRect	SRect::operator - (const _SPoint rh)
const
{
	SRect	offr (top - rh.v, left - rh.h, bottom - rh.v, right - rh.h);
	return offr;
}

inline		SRect&	SRect::operator -= (const _SPoint rh)
{
	top -= rh.v;
	left -= rh.h;
	bottom -= rh.v;
	right -= rh.h;
	return *this;
}

/*
 inline		SRect&	SRect::operator += (const _SRect &rh)
{
	top += rh.top;
	left += rh.left;
//	bottom += rh.bottom;
//	right += rh.right;
	return *this;
}
*/

// intersection
inline		bool	SRect::operator & (const _SRect &rh)
const
{
	return (rh.top < bottom && rh.left < right && rh.bottom > top && rh.right > left);
}

// intersection
inline		SRect&	SRect::operator &= (const _SRect &rh)
{
	
	if (rh.bottom < top) {
		bottom = top;
		return *this;
	}
	
	if (rh.top > top)
		top = rh.top;
	if (rh.left > left)
		left = rh.left;
	 
	if (rh.bottom < bottom)
		bottom = rh.bottom;
	if (rh.right < right)
		right = rh.right;
	if (top > bottom)   // pB 2010-9
		top = bottom = top;
	if (left > right)
		 right = left;
	 
	return *this;
}

// intersection without top
inline		SRect&	SRect::operator /= (const _SRect &rh)
{
	if (rh.left > left)
		left = rh.left;
	if (rh.bottom < bottom)
		bottom = rh.bottom;
	if (rh.right < right)
		right = rh.right;
	if (top > bottom)   // pB 2010-9
		top = bottom = 0;
	if (left > right)
		left = right = 0;
	return *this;
}

// inset
inline		SRect&	SRect::operator *= (double inset)
{
	top += inset;
	left += inset;
	bottom -= inset;
	right -= inset;
	return *this;
}

inline		SRect&	SRect::operator *= (const _SRect &rh)
{
	top += rh.top;
	left += rh.left;
	bottom -= rh.bottom;
	right -= rh.right;
	return *this;
}

inline		bool	SRect::IsEmpty (void) const
{
//	return top == bottom && right == left;
	return bottom - top < RW_EPSILON && right - left < RW_EPSILON;
}

inline		bool	SRect::Contains (const _SRect &rh) const
{
	return rh.top >= top && rh.left >= left && rh.bottom <= bottom && rh.right <= right;
}

inline		double	SRect::Width (void) const
{
	return right - left;
}

inline		double	SRect::Height (void) const
{
	return bottom - top;
}

inline		SPoint	SRect::TopLeft (void) const
{
	SPoint	pt (left, top);
	return pt;
}

inline		SPoint	SRect::BottomRight (void) const
{
	SPoint	pt (right, bottom);
	return pt;
}

inline		void	SRect::SetRect (double t, double l, double b, double r)
{
	top = t;
	left = l;
	bottom = b;
	right = r;
}

inline		void	SRect::SetRect (int t, int l, int b, int r)
{
	top = t;
	left = l;
	bottom = b;
	right = r;
}

/*inline	SRect::operator Rect (void) const
{
	Rect	rect = { top, left, bottom, right };
	return rect;
}*/


// RGBA color
struct	_SRGBColor
{
	unsigned short	red;
	unsigned short	green;
	unsigned short	blue;
	unsigned short	alpha;
};
struct	SRGBColor	:	public	_SRGBColor
{
			inline	SRGBColor (void);
			inline	SRGBColor (unsigned short r, unsigned short g, unsigned short b, unsigned short a);
			inline	SRGBColor (unsigned long argb);
			inline	SRGBColor (const char *inValue);
			inline	SRGBColor (const CText inValue);
#if	USE_MAC_TYPES
			inline	SRGBColor (const RGBColor inValue);
			inline	SRGBColor (const ATSURGBAlphaColor inValue);
	inline	operator RGBColor (void) const;
	inline	operator ATSURGBAlphaColor (void) const;
#endif
	SRGBColor&		operator = (const CText inValue);
	inline			operator unsigned long (void) const;
					operator CText (void) const;
                    operator char * (void) const;
};

inline	SRGBColor::SRGBColor (void)
{
}

inline	SRGBColor::SRGBColor (unsigned short r, unsigned short g, unsigned short b, unsigned short a)
{
	red = r;
	green = g;
	blue = b;
	alpha = a;
}

inline	SRGBColor::SRGBColor (unsigned long argb)
{
	blue = argb & 0xFF;
	argb >>= 8;
	green = argb & 0xFF;
	argb >>= 8;
	red = argb & 0xFF;
	argb >>= 8;
	alpha = argb & 0xFF;

	red |= red << 8;
	green |= green << 8;
	blue |= blue << 8;
	alpha |= alpha << 8;
}

inline	SRGBColor::SRGBColor (const char *inValue)
{
	operator = (inValue);
    

}

inline	SRGBColor::SRGBColor (const CText inValue)
{
	operator = (inValue);
}

#if	USE_MAC_TYPES
inline	SRGBColor::SRGBColor (const RGBColor inValue)
{
	red = inValue.red;
	green = inValue.green;
	blue = inValue.blue;
	alpha = ~0;
}

inline	SRGBColor::SRGBColor (const ATSURGBAlphaColor inValue)
{
	red = inValue.red * 65535;
	green = inValue.green * 65535;
	blue = inValue.blue * 65535;
	alpha = inValue.alpha * 65535;
}

inline	SRGBColor::operator RGBColor (void) const
{
	RGBColor	rgb = { red, green, blue };
	return rgb;
}

inline	SRGBColor::operator ATSURGBAlphaColor (void) const
{
	ATSURGBAlphaColor	argb = { (float)(red / 65535.), (float)(green / 65535.), (float)(blue / 65535.), (float)(alpha / 65535.) };
	return argb;
}
#endif

inline	SRGBColor::operator unsigned long (void) const
{
	unsigned long	argb = ((alpha & 0xFF00L) << 16) | ((red & 0xFF00L) << 8) | (green & 0xFF00) | ((blue & 0xFF00) >> 8);
	return argb;
}

inline    SRGBColor::operator CText (void) const
{
    unsigned long    argb = ((alpha & 0xFF00L) << 16) | ((red & 0xFF00L) << 8) | (green & 0xFF00) | ((blue & 0xFF00) >> 8);
    char             buffer[16];
    snprintf(buffer, 16, "%#x", argb);
    return RWTextValue::UTF_8_to_UTF16(buffer) ;
}

inline    SRGBColor::operator char * (void) const
{
    unsigned long    argb = ((alpha & 0xFF00L) << 16) | ((red & 0xFF00L) << 8) | (green & 0xFF00) | ((blue & 0xFF00) >> 8);
    char             buffer[16];
    snprintf(buffer, 16, "%#x", argb);
    return buffer ;
}

extern	const SRGBColor	cBlackColor;
extern	const SRGBColor	cGrayColor;
extern	const SRGBColor	cLightGrayColor;
extern	const SRGBColor	cWhiteColor;
extern	const SRGBColor	cWhite50Color;
extern	const SRGBColor	cRedColor;
extern	const SRGBColor	cDarkRedColor;
extern	const SRGBColor	cGreenColor;
extern	const SRGBColor	cBlueColor;
extern	const SRGBColor	cDarkBlueColor;
extern	const SRGBColor	cCyanColor;
extern	const SRGBColor	cMagentaColor;
extern	const SRGBColor	cYellowColor;
extern	const SRGBColor	cBrownColor;
extern	const SRGBColor	cOrangeColor;
extern	const SRGBColor	cDarkOrangeColor;
extern	const SRGBColor	cPurpleColor;
extern	const SRGBColor	cHighlightColor;
extern	const SRGBColor	cEmptyColor;


inline	bool operator == (const _SRGBColor inLh, const _SRGBColor inRh)
{
	return inLh.red == inRh.red && inLh.green == inRh.green && inLh.blue == inRh.blue && inLh.alpha == inRh.alpha;
}

inline	bool operator != (const _SRGBColor inLh, const _SRGBColor inRh)
{
	return inLh.red != inRh.red || inLh.green != inRh.green || inLh.blue != inRh.blue || inLh.alpha != inRh.alpha;
}

// variable
class	RWValue
{
public:
	enum	EValue_Kind
	{
		eValue_Undefined = 0,
		eValue_Boolean,
		eValue_Integer,
		eValue_Real,
		eValue_XMLText,
		eValue_Text,
		eValue_DateTime,
		eValue_Date,
		eValue_Time,
		eValue_PictRefScreen,		// CGImageRef / Gdiplus::Bitmap*
		eValue_PictRefPrint,		// CGPDFDocumentRef / Gdiplus::Metafile*
		eValue_BLOB,
		eValue_PicturePICT,
		eValue_PicturePDF,
		eValue_PictureJPG,
		eValue_PicturePNG,
		eValue_PictureTIFF,
		eValue_PictureEMF
	};
	
typedef	enum
	{														
		eVK_Real			= 1,	// Variable declared using C_REAL
		eVK_PictureACI		= 3,	// Variable declared using C_GRAPH ;-)
		eVK_Date			= 4,	// Variable declared using C_DATE
		eVK_Undefined		= 5,	// Undefined variable
		eVK_Boolean			= 6,	// variable declared using C_BOOLEAN
		eVK_Integer			= 8,	// variable declared using C_INTEGER
		eVK_Longint			= 9,	// Variable declared using C_LONGINT
		eVK_Picture			= 10,	// Variable declared using C_PICTURE
		eVK_Time			= 11,	// Variable declared using C_TIME
		eVK_ArrayOfArray	= 13,	// Any two-dimensional array
		eVK_ArrayReal		= 14,	// One dimension array declared using ARRAY REAL
		eVK_ArrayInteger	= 15,	// One dimension array declared using ARRAY INTEGER
		eVK_ArrayLongint	= 16,	// One dimension array declared using ARRAY LONGINT
		eVK_ArrayDate		= 17,	// One dimension array declared using ARRAY DATE
		eVK_ArrayPicture	= 19,	// One dimension array declared using ARRAY PICTURE
		eVK_ArrayPointer	= 20,	// One dimension array declared using ARRAY POINTER
		eVK_ArrayBoolean	= 22,	// One dimension array declared using ARRAY BOOLEAN
		eVK_Pointer			= 23,	// Variable declared using C_POINTER
		eVK_Blob			= 30,	// Variable declared using C_BLOB
		eVK_Unistring		= 33,	// Variable declared using C_STRING or C_TEXT
		eVK_ArrayUnicode	= 34	// One Dimension array declared using ARRAY STRING or ARRAY TEXT
	} PA_VariableKind;
	

			inline					RWValue (void);
			inline					RWValue (long inInteger);
			inline					RWValue (double inDouble);
//			inline					RWValue (time_t inDateTime);		time_t is actually long...
			inline					RWValue (long inValue, EValue_Kind inKind);
			inline					RWValue (EValue_Kind inKind);
			inline					RWValue (PA_VariableKind inKind);
//									RWValue (SConstText inText);
									RWValue (EValue_Kind inKind, void* inData, size_t inSize);
									RWValue (const RWValue &inOriginal);
			inline					~RWValue (void);
public:
					void			Free (void);
					RWValue	&		Attach (const RWValue &inOriginal);	// original is owner, we have same pointers
					RWValue	&		Detach (RWValue &inOriginal);		// original is not owner anymore, we have same pointers
					RWValue	&		Clone (const RWValue &inOriginal);	// make a copy of original

					bool			operator == (const RWValue &inCompare) const;
			inline	bool			operator != (const RWValue &inCompare) const;

			inline	EValue_Kind		GetKind (void) const;
			inline	bool			IsEmpty (void) const;
			inline	long			GetInteger (void) const;
			inline	void			SetInteger (long value, EValue_Kind inKind = eValue_Integer);
			inline	bool			GetBoolean (void) const;
			inline	void			SetBoolean (int value);
			inline	void			SetBoolean (bool value);
			inline	double			GetReal (void) const;
			inline	void			SetReal (double value);
			inline	void			SetXMLText (const CXMLText value);
			inline	void			SetXMLText (CXMLText value, bool takeOwnership);
    
            inline  CXMLText        GetXMLText (void) const;
			inline	CText		    GetText (void) const;
			inline	void			SetText (const CText value);
//					void			SetText (const char* value);
//					void			SetText (const UTF8Char* value);
			inline	void			SetText (CText value, bool takeOwnership);
			inline	size_t			GetBlobSize (void) const;
			inline	void*			GetBlobData (void) const;
			inline	const SBlob&	GetBlob (void) const;
			inline	void			SetBlob (void* value, size_t size, bool takeOwnership);
			inline	void			SetBlob (const SBlob& value, bool takeOwnership);
			inline	void*			GetPictureRef (void) const;
			inline	void			SetPictureRef (void* value, bool inScreen);
			inline	void			SetPicture (EValue_Kind inKind, void* value, size_t size, bool takeOwnership);
			inline	void			SetPicture (EValue_Kind inKind, const SBlob& value, bool takeOwnership);
					void			GetTextValue (RWTextValue &outValue, const char* fmt) const;
	static	inline	const char**	GetPictFormats (void);
					bool			CoerceValue (EValue_Kind inKind);

private:
	// not implemented - dangerous behavior - just a const difference...
					RWValue	&		operator = (const RWValue &inOriginal);	// clone - inOriginal is stil owner
//					RWValue	&		operator = (RWValue &inOriginal);		// grab - inOriginal is not owner anymore
	
protected:
static const char *		sPictFormats[];
	EValue_Kind			fKind;		// variable kind
	mutable bool		fOwn;		// data is owned by this instance

	union
	{
		long			fInteger;
		double			fReal;
		CXMLText    	fXMLText;
		CText	 		fText;
		SBlob			fBlob;
	};
};


inline	RWValue::RWValue (void)
	:	fKind (eValue_Undefined),
		fOwn (false),
		fText (0)
{
}


inline	RWValue::RWValue (long inInteger)
	:	fKind (eValue_Integer),
		fOwn (false),
		fInteger (inInteger)
{
}


inline	RWValue::RWValue (double inDouble)
	:	fKind (eValue_Real),
		fOwn (false),
		fReal (inDouble)
{
}


/*		time_t is actually long...
inline	RWValue::RWValue (time_t inDateTime)
	:	fKind (eValue_DateTime),
		fOwn (false),
		fDateTime (inDateTime)
{
}
*/

inline	RWValue::RWValue (long inValue, EValue_Kind inKind)
	:	fKind (eValue_Undefined),
		fOwn (false),
		fInteger (inValue)
{
	if (inKind == eValue_Boolean || inKind == eValue_Date || inKind == eValue_Time || inKind == eValue_DateTime)
		fKind = inKind;
	return;
}

inline	RWValue::RWValue (EValue_Kind inKind)
	:	fKind (inKind),
	fOwn (false)
{
}

inline	RWValue::RWValue (PA_VariableKind inKind)
:	fKind (eValue_Undefined),
	fOwn (false)
{
	switch (inKind) {
		case eVK_Real:
			fKind = eValue_Real;
			break;
		case	eVK_Date:
			fKind = eValue_Date;
			break;
		case	eVK_Undefined:
			fKind = eValue_Undefined;
			break;
		case	eVK_Boolean:
			fKind = eValue_Boolean;
			break;
		case	eVK_Integer:
			fKind = eValue_Integer;
			break;
		case	eVK_Longint:
			fKind = eValue_Integer;
			break;
		case	eVK_Picture:
			fKind = eValue_Undefined;
			break;
		case	eVK_Time:
			fKind = eValue_Time;
			break;
			
		default:
			fKind = eValue_Undefined;
			break;
	}
}

inline	RWValue::~RWValue (void)
{
	Free();
}

inline
bool
RWValue::operator != (const RWValue &inCompare)
const
{
	return not operator == (inCompare);
}

inline	RWValue::EValue_Kind	RWValue::GetKind (void) const		{ return fKind; }
inline	bool					RWValue::IsEmpty (void) const		{ return fText.empty(); }
inline	long					RWValue::GetInteger (void) const	{ return fInteger; }
inline	void					RWValue::SetInteger (long value, EValue_Kind inKind)	{ Free(); fKind = inKind; fInteger = value; }
inline	bool					RWValue::GetBoolean (void) const	{ return fInteger != 0; }
inline	void					RWValue::SetBoolean (int value)		{ Free(); fKind = eValue_Boolean; fInteger = (value != 0); }
inline	void					RWValue::SetBoolean (bool value)	{ Free(); fKind = eValue_Boolean; fInteger = value; }
inline	double					RWValue::GetReal (void) const		{ return fReal; }
inline	void					RWValue::SetReal (double value)		{ Free(); fKind = eValue_Real; fReal = value; }
inline	CXMLText			    RWValue::GetXMLText (void) const				{ return fXMLText; }
inline	void					RWValue::SetXMLText (const CXMLText value)		{ Free(); fKind = eValue_XMLText; fXMLText = value; }
inline	void					RWValue::SetXMLText (CXMLText value, bool takeOwnership)		{ Free(); fKind = eValue_XMLText; fXMLText = value; fOwn = takeOwnership; }
inline	CText				    RWValue::GetText (void) const		{ return fText; }
inline	void					RWValue::SetText (const CText value)
{
	Free();
	fKind = eValue_Text;
//	fOwn = false;
	fText = value;
}
inline	void					RWValue::SetText (CText value, bool takeOwnership)
{
	Free();
	fKind = eValue_Text;
	fOwn = takeOwnership;
	fText = value;
}
inline	size_t					RWValue::GetBlobSize (void) const	{ return fKind >= eValue_BLOB? fBlob.fSize: 0; }
inline	void*					RWValue::GetBlobData (void) const	{ return fBlob.fData; }
inline	const SBlob	&			RWValue::GetBlob (void) const		{ return fBlob; }
inline	void					RWValue::SetBlob (void* value, size_t size, bool takeOwnership)
{
	Free();
	fKind = eValue_BLOB;
	fOwn = takeOwnership;
	fBlob.fData = value;
	fBlob.fSize = size;
}
inline	void					RWValue::SetBlob (const SBlob& value, bool takeOwnership)	{ SetBlob (value.fData, value.fSize, takeOwnership); }
inline	void*					RWValue::GetPictureRef (void) const	{	return (void*) fInteger; }
//inline	void					RWValue::SetPictureRef (void* value)	{ Free(); if ((fInteger = (long) value) != 0) fKind = eValue_PictureRef; }
inline	void					RWValue::SetPictureRef (void* value, bool inScreen)	{ Free(); fInteger = (long) value; fKind = inScreen? eValue_PictRefScreen: eValue_PictRefPrint; fOwn = true; }
inline	void					RWValue::SetPicture (EValue_Kind inKind, void* value, size_t size, bool takeOwnership)		{ SetBlob (value, size, takeOwnership); fKind = inKind; }
inline	void					RWValue::SetPicture (EValue_Kind inKind, const SBlob& value, bool takeOwnership)			{ SetBlob (value.fData, value.fSize, takeOwnership); fKind = inKind; }
inline	const char**			RWValue::GetPictFormats (void)			{ return sPictFormats; }


typedef	RWMap<CText, RWValue*>	RWVarMap;



enum	ECalcType
{
	ECalcType_None = 0,
	ECalcType_Total,
	ECalcType_Min,
	ECalcType_Average,
	ECalcType_Max,
	ECalcType_Count,
	ECalcType_StdVar,
	ECalcType_StdDev,
	ECalcType_Last,

	ECalcType_CurrentValue = 0,
	ECalcType_OldValue = -1
};


enum	RW_VarNames
{
	// RWReportWriter variables
	RW_VarNotFound = -1,
	RW_VarPage,
	RW_VarPages,
	RW_VarSubPage,
	RW_VarSubPages,
	RW_VarFrame,
	RW_VarFrames,
	RW_VarDateTime,
	RW_VarDate,
	RW_VarTime,
	RW_VarName,
	RW_VarHorPage,
	RW_VarHorPages,

	// SRReportWriter variables
//	RW_VarSRPage,
//	RW_VarSRDate,
//	RW_VarSRTime,
//	RW_VarSRRecord,
	RW_VarRWDate,
	RW_VarRWTime,
	RW_VarRWRecord,

	RW_VarNamesSRCount,
	RW_VarNamesRWCount = RW_VarName + 1
};



#if	USE_MAC_TYPES == 0 && WINVER
typedef UInt16                          EventModifiers;
enum {
                                        /* modifiers */
  activeFlagBit                 = 0,    /* activate? (activateEvt and mouseDown)*/
  btnStateBit                   = 7,    /* state of button?*/
  cmdKeyBit                     = 8,    /* command key down?*/
  shiftKeyBit                   = 9,    /* shift key down?*/
  alphaLockBit                  = 10,   /* alpha lock down?*/
  optionKeyBit                  = 11,   /* option key down?*/
  controlKeyBit                 = 12,   /* control key down?*/
  rightShiftKeyBit              = 13,   /* right shift key down?*/
  rightOptionKeyBit             = 14,   /* right Option key down?*/
  rightControlKeyBit            = 15    /* right Control key down?*/
};

enum {
  activeFlag                    = 1 << activeFlagBit,
  btnState                      = 1 << btnStateBit,
  cmdKey                        = 1 << cmdKeyBit,
  shiftKey                      = 1 << shiftKeyBit,
  alphaLock                     = 1 << alphaLockBit,
  optionKey                     = 1 << optionKeyBit,
  controlKey                    = 1 << controlKeyBit,
  rightShiftKey                 = 1 << rightShiftKeyBit,
  rightOptionKey                = 1 << rightOptionKeyBit,
  rightControlKey               = 1 << rightControlKeyBit
};
struct CGPoint {
    float x;
    float y;
};
struct CGSize {
    float width;
    float height;
};
struct CGAffineTransform {
    float a, b, c, d;
    float tx, ty;
};
CGAffineTransform	CGAffineTransformMake (float a, float b, float c, float d, float tx, float ty);
CGAffineTransform	CGAffineTransformMakeTranslation (float tx, float ty);
CGAffineTransform	CGAffineTransformMakeScale (float sx, float sy);
CGAffineTransform	CGAffineTransformMakeRotation (float angle);
CGAffineTransform	CGAffineTransformTranslate (CGAffineTransform t, float tx, float ty);
CGAffineTransform	CGAffineTransformScale (CGAffineTransform t, float sx, float sy);
CGAffineTransform	CGAffineTransformRotate (CGAffineTransform t, float angle);
CGAffineTransform	CGAffineTransformInvert (CGAffineTransform t);
CGAffineTransform	CGAffineTransformConcat (CGAffineTransform t1, CGAffineTransform t2);
CGPoint				CGPointApplyAffineTransform (CGPoint point, CGAffineTransform t);
CGSize				CGSizeApplyAffineTransform (CGSize size, CGAffineTransform t);
#if !defined(CG_INLINE)
#  if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#    define CG_INLINE static inline
#  elif defined(__MWERKS__) || defined(__cplusplus)
#    define CG_INLINE static inline
#  elif defined(__GNUC__)
#    define CG_INLINE static __inline__
#  else
#    define CG_INLINE static    
#  endif
#endif
CG_INLINE CGPoint __CGPointMake(float x, float y)
{
   CGPoint p; p.x = x; p.y = y; return p;
}
#define CGPointMake __CGPointMake

CG_INLINE CGSize __CGSizeMake(float width, float height)
{
   CGSize size; size.width = width; size.height = height; return size;
}
#define CGSizeMake __CGSizeMake
CG_INLINE CGAffineTransform
__CGAffineTransformMake(float a, float b, float c, float d, float tx, float ty)
{
    CGAffineTransform t;
	
    t.a = a; t.b = b; t.c = c; t.d = d; t.tx = tx; t.ty = ty;
    return t;
}
#define CGAffineTransformMake __CGAffineTransformMake

CG_INLINE CGPoint
__CGPointApplyAffineTransform(CGPoint point, CGAffineTransform t)
{
    CGPoint p;
	
    p.x = t.a * point.x + t.c * point.y + t.tx;
    p.y = t.b * point.x + t.d * point.y + t.ty;
    return p;
}
#define CGPointApplyAffineTransform __CGPointApplyAffineTransform

CG_INLINE CGSize
__CGSizeApplyAffineTransform(CGSize size, CGAffineTransform t)
{
    CGSize s;
	
    s.width = t.a * size.width + t.c * size.height;
    s.height = t.b * size.width + t.d * size.height;
    return s;
}
#define CGSizeApplyAffineTransform __CGSizeApplyAffineTransform
#endif

using namespace std;

typedef	struct	RWPrintContext*	RWPrintContextRef;	// for coordinate transform - bottom + CG on Mac / bottom for PDF / nothing otherwise

using namespace tinyxml2;

class	RWTools
{
public:
	static		CText           	ParseIntoText (const XMLElement *inNode, bool);
	static		CText				ParseIntoText (const XMLElement *inNode);
    static      void                WriteText (FILE *fd, const CXMLText inText);
	static		void				WriteText (XMLElement *inParent, const CXMLText inText);
	static		void				WriteText (FILE *fd, const CText inText);
	static		void				WriteText (XMLElement *inParent, const CText inText);


	static		void				ReadData (const XMLElement *inParent, SBlob &outData);
	static		void				WriteData (FILE *fd, const SBlob &inData);
	static		void				WriteData (XMLNode *inParent, const SBlob &inData);
	static		long				FindInList (const CXMLText inValue, const char** inList);

	static		CGAffineTransform	MakeMatrixFromUserRect (SRect &ioRect, float inAngle, float inBottom, float inWidth, float inHeight);
	static		CGAffineTransform	MakeMatrixFromUserRect (const RWPrintContextRef inContext, SRect &ioRect, float inAngle, float inWidth, float inHeight);
	static		void				MakeUserRectFromText (SRect &ioRect, float inAngle);

	static		CText				SplitAttributedString (const CText inAttributedString, unique_ptr<long> *outAttributes);
    static      CText               EscapeAttributedString (const CText inAttributedString);
    static      CXMLText            EscapeAttributedString (const CXMLText inAttributedString);
	static		int					ParseAttributedStringAttribute (const CText inAttributedString, double &outSize, int &outSizeSign, int &outStyle, SRGBColor &outColor, CText &outFont);
	static		bool				ParseTextForVar (bool inAttributed, const CText inString, long inTextLen, long &ioStart, long &outEnd, CText &outVarName, CText &outFormat);
	static		bool				ParseTextForXLIFF (const CText inString, long inTextLen, CText &outText);
};


template <class _Tp, typename _Alloc = allocator<_Tp> >
class RWArray : public vector<_Tp, _Alloc>
{
public:
	typedef 	vector<_Tp, _Alloc>	_RWVector;
							RWArray (void)
								:	_RWVector () {}
	virtual					~RWArray (void)
	{
		clear();
	}

public:
	void clear()
	{
		{
			typename _RWVector::const_iterator	it;

			for (it = _RWVector::begin(); it != _RWVector::end(); it++)
			{
				_Tp	t = *it;
				delete t;
			}
		}
		_RWVector::clear();
	}

private:
	// defensive programming - not implemented
//							RWArray (const RWArray &inOriginal);
//				RWArray	&	operator = (const RWArray &inOriginal);
};

typedef	void (*RW_CBFunction) (void *inData);
void	RW_RunInMainThread (RW_CBFunction inFunction, void *inData);	// client app has to implement...
void	RW_ConvertPictureForPrinting (RWValue &ioPict, bool inForPDF);
#endif
