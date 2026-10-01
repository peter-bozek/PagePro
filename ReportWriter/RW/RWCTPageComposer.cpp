/*
 *  RWCTPageComposer.cpp
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 17.01.2010.
 *  Copyright 2010 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */

#include "RWCTPageComposer.h"
#include "RWStyle.h"
#include "RWStringCF.h"
#include "RWFontsMac.h"

struct	RWMacPrintTextContext
{
	CGContextRef	fCGContext;
	float			fPageBottom;
};

// Composer specific object for text rendering
class	RWCTPrintText	: public	RWPrintText
{
public:
								RWCTPrintText (RWStyle *inStyle);
	virtual						~RWCTPrintText (void);
	virtual		void			Reset (void);

				void			Init (RWCTPageComposer &inComposer, const CText inText, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit);
				void			GetBounds (RWCTPageComposer &inComposer, SRect &ioRect, bool inFit);
				int				Draw (RWCTPageComposer &inComposer, SRect &ioRect, bool inFit, void *inContext);

protected:
				void			Free();
				void			ApplyAttributes (const CText inText, CTFontRef font, long *attributes, long start, long end);

private:
	// defensive programming - not implemented
								RWCTPrintText (const RWCTPrintText &inOriginal);
				RWCTPrintText&	operator = (const RWCTPrintText &inOriginal);
	
protected:
	CFIndex							mTextLength;
	CFMutableAttributedStringRef	mCFText;
	CTFramesetterRef				mCTFrames;
	CFRange							mCFRange;
};



RWNativePageComposer*
RWCTPageComposer::CreateComposerForPrinting (void)
const
{

    RWCTPageComposer	*pc = new RWCTPageComposer (*this);
	pc->mFlags = (GetFlags() & eUserFlagsMask) | eDestinationPrinter;
	return static_cast<RWNativePageComposer*> (pc);
}


// ---------------------------------------------------------------------------
// RWCTPageComposer							Constructor				  [public]
// ---------------------------------------------------------------------------

RWCTPageComposer::RWCTPageComposer (unsigned long inFlags, CText &inDst, CText &inPrinter)	//mbs 25072011	printer
	:	RWMacCGPageComposer (inFlags, inDst, inPrinter)	//mbs 25072011	printer
{
	return;
}


// ---------------------------------------------------------------------------
// ~RWCTPageComposer						Destructor				  [public]
// ---------------------------------------------------------------------------

RWCTPageComposer::~RWCTPageComposer (void)
{
	StyleChanged (NULL);
	return;
}




// ---------------------------------------------------------------------------
// StyleChanged														  [public]
// ---------------------------------------------------------------------------

void
RWCTPageComposer::StyleChanged (RWStyle *inStyle)
{
	if (inStyle == NULL)
	{
		RWStyleToCFDictionaryMap::const_iterator	it;
		
		for (it = mStyleMap.begin(); it != mStyleMap.end(); it++)
		{
			CFMutableDictionaryRef	styleDict = (*it).second;
			::CFRelease (styleDict);
		}
		mStyleMap.clear();
	}
	else
	{
		RWStyleToCFDictionaryMap::key_type v (*inStyle);
		RWStyleToCFDictionaryMap::iterator	it = mStyleMap.find (v);
		if (it != mStyleMap.end())
		{
			CFMutableDictionaryRef	styleDict = (*it).second;
			::CFRelease (styleDict);
			mStyleMap.erase (it);
		}
	}
}


// ---------------------------------------------------------------------------
// CreateFont												 [static] [public]
// ---------------------------------------------------------------------------

CTFontRef
RWCTPageComposer::CreateFont (const CText inName, long inNameLength, float inSize, int style)
{
	return RWCreateCTFont (RWStringView (inName).substr (0, size_t (std::max (inNameLength, 0L))), inSize, style);
}


// ---------------------------------------------------------------------------
// MapStyle														   [protected]
// ---------------------------------------------------------------------------

CFDictionaryRef
RWCTPageComposer::MapStyle (RWStyle *inStyle)
{
	CFMutableDictionaryRef	styleDict = NULL;
	
	{
		RWStyleToCFDictionaryMap::key_type v (*inStyle);
		RWStyleToCFDictionaryMap::iterator	it = mStyleMap.find (v);
		if (it != mStyleMap.end())
		{
			styleDict = (*it).second;
		}

		/* for (it = mStyleMap.begin(); it != mStyleMap.end(); it++)
		{
			if (inStyle == (*it).first)
			{
				styleDict = (*it).second;
				break;
			}
		} */
	}
	
	if (styleDict == NULL)
	{
		styleDict = CFDictionaryCreateMutable (kCFAllocatorDefault, 4, &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		// require (styleDict != NULL, CFDictionaryCreateMutable);

		CText		fontName = inStyle->GetFName();
		CTFontRef	font = CreateFont (fontName, (long) fontName.size(), inStyle->GetSize(), inStyle->GetStyle());
		CFDictionaryAddValue (styleDict, kCTFontAttributeName, font);
		::CFRelease (font);

		// text color
		CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
        SRGBColor textColor = inStyle->GetTextColor();
        CGFloat		f [4] = {textColor.red / 65535., textColor.green / 65535.,textColor.blue / 65535., textColor.alpha / 65535.};
		CGColorRef cgcolor = CGColorCreate (rgbColorSpace, f);
		::CGColorSpaceRelease (rgbColorSpace);
		CFDictionaryAddValue (styleDict, kCTForegroundColorAttributeName, cgcolor);
		::CGColorRelease (cgcolor);

		// horizontal alignment/word wrap/line spacing
		CTTextAlignment	align;
		CTLineBreakMode	lineBreak = inStyle->ShouldWrap()? kCTLineBreakByWordWrapping : kCTLineBreakByClipping;
//		float leading = 1;
		switch (inStyle->GetJustification())
		{
			case RWStyle::st_default:		align = kCTNaturalTextAlignment; break;
			case RWStyle::st_left:			align = kCTLeftTextAlignment; break;
			case RWStyle::st_right:			align = kCTRightTextAlignment; break;
			case RWStyle::st_center:		align = kCTCenterTextAlignment; break;
			case RWStyle::st_justify:		align = kCTJustifiedTextAlignment; break;
			case RWStyle::st_fulljustify:	align = kCTJustifiedTextAlignment; break;
		}
		CTParagraphStyleSetting	paraSet [] = {
			{ kCTParagraphStyleSpecifierAlignment, sizeof (align), &align },
			{ kCTParagraphStyleSpecifierLineBreakMode, sizeof (lineBreak), &lineBreak }
//			{ kCTParagraphStyleSpecifierLineSpacing, sizeof (float), &leading }
		};
		CTParagraphStyleRef	para = CTParagraphStyleCreate (paraSet, sizeof (paraSet)/sizeof (paraSet[0]));
		CFDictionaryAddValue (styleDict, kCTParagraphStyleAttributeName, para);
		::CFRelease (para);

		// font variation - underline
		if (inStyle->GetStyle() & RWStyle::st_underline)
		{
			SInt32 l = kCTUnderlineStyleSingle | kCTUnderlinePatternSolid;
			CFNumberRef underline = CFNumberCreate (kCFAllocatorDefault, kCFNumberSInt32Type, &l);	
			CFDictionaryAddValue (styleDict, kCTUnderlineStyleAttributeName, underline);
			::CFRelease (underline);
		}
		
		mStyleMap.insert (RWStyleToCFDictionaryMap::value_type (*inStyle, styleDict));
	}

CFDictionaryCreateMutable:

	return styleDict;
}




void
RWCTPageComposer::DrawTextBox (CText inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText)
{
if (sUseTF)
	RWPageComposer::DrawTextBox (inText, inStyle, inRect, inWrap, inAttributed, inFit, ioPrintText);
else
{
	SRect	r (inRect);
	if (ioPrintText != NULL && *ioPrintText != NULL)
		inText = (*ioPrintText)->GetText();
	
	if (mPageIsOpen)
	{
		RWMacPrintTextContext	ctx;
		ctx.fCGContext = DrawTextBoxBegin (inStyle, inRect);
		ctx.fPageBottom = mPageRect.bottom;
		
		try
		{
			if (!inText.empty())
			{
				if (ioPrintText == NULL)	// RWTable support
				{
					RWCTPrintText	txt (inStyle);
					txt.Init (*this, inText, r, inWrap, inAttributed, inFit);
					r = inRect;
					txt.Draw (*this, r, inFit, &ctx);
				}
				else
				{
					if (*ioPrintText == NULL)
					{
						*ioPrintText = new RWCTPrintText (inStyle);
						static_cast <RWCTPrintText*> (*ioPrintText)->Init (*this, inText, r, inWrap, inAttributed, inFit);
						r = inRect;
					}
					static_cast <RWCTPrintText*> (*ioPrintText)->Draw (*this, r, inFit, &ctx);
				}
			}
			DrawTextBoxEnd (ctx.fCGContext);
		}
		catch (...)
		{
			DrawTextBoxEnd (ctx.fCGContext);
			throw;
		}
	}
	else if (ioPrintText != NULL && *ioPrintText != NULL)	// nothing to do if we don't draw and ioPrintText is empty
	{
		if (!inText.empty())
			static_cast <RWCTPrintText*> (*ioPrintText)->Draw (*this, r, inFit, NULL);
	}
}
}


double
RWCTPageComposer::MeasureText (const CText inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText)
{
if (sUseTF)
	return RWPageComposer::MeasureText (inText, inStyle, ioRect, inWrap, inAttributed, inFit, ioPrintText);
else
{
	if (!inText.empty())
	{
		if (ioPrintText == NULL)	// RWTable support
		{
			RWCTPrintText	txt (inStyle);
			txt.Init (*this, inText, ioRect, inWrap, inAttributed, inFit);
//			txt.GetBounds (*this, ioRect, inFit, false);
		}
		else
		{
			if (*ioPrintText == NULL)
			{
				*ioPrintText = new RWCTPrintText (inStyle);
				static_cast <RWCTPrintText*> (*ioPrintText)->Init (*this, inText, ioRect, inWrap, inAttributed, inFit);
			}
			else
				static_cast <RWCTPrintText*> (*ioPrintText)->GetBounds (*this, ioRect, inFit);
		}
	}
	else
	{
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
	}
	
	return ioRect.Width();
}
}

#pragma	mark -

RWCTPrintText::RWCTPrintText (RWStyle *inStyle)
	:	RWPrintText (inStyle),
		mTextLength (0),
		mCFText (NULL),
		mCTFrames (NULL)
{
}


RWCTPrintText::~RWCTPrintText (void)
{
	Free();
}


void
RWCTPrintText::Free (void)
{
	mTextLength = 0;
	if (mCFText)
	{
		::CFRelease (mCFText);
		mCFText = NULL;
	}
	if (mCTFrames)
	{
		::CFRelease (mCTFrames);
		mCTFrames = NULL;
	}
	mCFRange = CFRangeMake (0, 0);
	return;
}


void
RWCTPrintText::ApplyAttributes (const CText inText, CTFontRef font, long *attributes, long start, long end)
{
	long		i, j, r = 1;
	CTFontRef	fontA;
	for (i = start; i < end; i += 2)
	{
		const char16_t	*as = inText.c_str() + attributes[i];
		if (*as == '/')
			continue;
		for (j = i + 2; j < end; j += 2)
		{
			const char16_t	*ae = inText.c_str() + attributes[j];
			if (*ae != '/')
			{
				if (*ae == *as)
					r++;
				continue;
			}
			if (ae[1] == *as)
			{
				if (--r == 0)
					break;
			}
		}
		if (j < attributes[0])
			r = attributes[j+1];
		else
			r = attributes[1];
		mCFRange = CFRangeMake (attributes[i+1], r - attributes[i+1]);

		double		size = 0;
		int			sizeSign = 0;
		int			style = 0;
		SRGBColor	color;
		CText		fontName;
		int	rv = RWTools::ParseAttributedStringAttribute (as, size, sizeSign, style, color, fontName);

		switch (rv)
		{
//			case 'b':
			case 'B':
				fontA = CTFontCreateCopyWithSymbolicTraits (font, 0.0, NULL, kCTFontBoldTrait, kCTFontBoldTrait);
				if (fontA == 0)
				{
					if (j > i + 4)
						ApplyAttributes (inText, font, attributes, i+2, j-2);
				}
				else
				{
					CFAttributedStringSetAttribute (mCFText, mCFRange, kCTFontAttributeName, fontA);
					if (j > i + 4)
						ApplyAttributes (inText, fontA, attributes, i+2, j-2);
					::CFRelease (fontA);
				}
				break;

//			case 'i':
			case 'I':
				fontA = CTFontCreateCopyWithSymbolicTraits (font, 0.0, NULL, kCTFontItalicTrait, kCTFontItalicTrait);
				if (fontA == 0)
				{
					if (j > i + 4)
						ApplyAttributes (inText, font, attributes, i+2, j-2);
				}
				else
				{
					CFAttributedStringSetAttribute (mCFText, mCFRange, kCTFontAttributeName, fontA);
					if (j > i + 4)
						ApplyAttributes (inText, fontA, attributes, i+2, j-2);
					::CFRelease (fontA);
				}
				break;

//			case 'u':
			case 'U':
			{
				SInt32 l = kCTUnderlineStyleSingle | kCTUnderlinePatternSolid;	//•••
				CFNumberRef underline = CFNumberCreate (kCFAllocatorDefault, kCFNumberSInt32Type, &l);	
				CFAttributedStringSetAttribute (mCFText, mCFRange, kCTUnderlineStyleAttributeName, underline);
				::CFRelease (underline);
				if (j > i + 4)
					ApplyAttributes (inText, font, attributes, i+2, j-2);
				break;
			}

//			case 'c':
			case 'C':	// color
			{
				CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
                CGFloat		f [4] = {color.red / 65535., color.green / 65535.,color.blue / 65535., color.alpha / 65535.};
				CGColorRef cgcolor = CGColorCreate (rgbColorSpace, f);
				::CGColorSpaceRelease (rgbColorSpace);
				if (cgcolor != 0)
				{
					CFAttributedStringSetAttribute (mCFText, mCFRange, kCTForegroundColorAttributeName, cgcolor);
					::CGColorRelease (cgcolor);
				}
				if (j > i + 4)
					ApplyAttributes (inText, font, attributes, i+2, j-2);
				break;
			}

//			case 'f':
			case 'F':	// font face
			{
				CTFontSymbolicTraits	sstyle = CTFontGetSymbolicTraits (font);
				style = 0;
				if (sstyle & kCTFontBoldTrait)
					style |= RWStyle::st_bold;
				if (sstyle & kCTFontItalicTrait)
					style |= RWStyle::st_italic;
				fontA = RWCTPageComposer::CreateFont (fontName, (long) fontName.size(), CTFontGetSize (font), style);
				//mbs 25052010	CFAttributedStringSetAttribute will crash if the font is the same!
				if (fontA != 0)
				{
					CFStringRef	fn1 = CTFontCopyFamilyName (font);
					CFStringRef	fn2 = CTFontCopyFamilyName (fontA);
					if (CFStringCompare (fn1, fn2, 0) == kCFCompareEqualTo)
					{
						::CFRelease (fontA);
						fontA = 0;
					}
					::CFRelease (fn1);
					::CFRelease (fn2);
				}
				if (fontA == 0)
				{
					if (j > i + 4)
						ApplyAttributes (inText, font, attributes, i+2, j-2);
				}
				else
				{
					CFAttributedStringSetAttribute (mCFText, mCFRange, kCTFontAttributeName, fontA);
					if (j > i + 4)
						ApplyAttributes (inText, fontA, attributes, i+2, j-2);
					::CFRelease (fontA);
				}
				break;
			}

//			case 's':
			case 'S':	// font size
			{
				CGFloat	fSize = CTFontGetSize (font);
				if (sizeSign == '+')
					size = fSize + (fSize / 4) * size;
				else if (sizeSign == '-')
				{
					size = fSize - (fSize / 4) * size;
					if (size < 5)
						size = 5;
				}
				if (size > 4 && size != fSize)
				{
					CFMutableDictionaryRef dict = CFDictionaryCreateMutable (kCFAllocatorDefault, 0, &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
					CTFontDescriptorRef	fdesc = CTFontDescriptorCreateWithAttributes (dict);
					::CFRelease (dict);
					fontA = CTFontCreateCopyWithAttributes (font, size, NULL, fdesc);
					::CFRelease (fdesc);
				}
				else
					fontA = NULL;
				if (fontA == 0)
				{
					if (j > i + 4)
						ApplyAttributes (inText, font, attributes, i+2, j-2);
				}
				else
				{
					CFAttributedStringSetAttribute (mCFText, mCFRange, kCTFontAttributeName, fontA);
					if (j > i + 4)
						ApplyAttributes (inText, fontA, attributes, i+2, j-2);
					::CFRelease (fontA);
				}
				break;
			}

			default:	// SPAN STYLE attributes
				if (rv & 256)
				{
					// rv & 1	font
					// rv & 2	size
					// rv & 4	bold
					// rv & 8	italic
					// rv & 16	underline
					// rv & 32	color
					CGFloat	fSize = CTFontGetSize (font);
					if (rv & 2)
					{
						if (sizeSign == '+')
							size = fSize + (fSize / 4) * size;
						else if (sizeSign == '-')
						{
							size = fSize - (fSize / 4) * size;
							if (size < 5)
								size = 5;
						}
					}
					else
						size = fSize;

					int	curStyle = 0;
					CTFontSymbolicTraits	cstyle = 0, sstyle = CTFontGetSymbolicTraits (font);
					if (sstyle & kCTFontBoldTrait)
						curStyle |= RWStyle::st_bold;
					if (sstyle & kCTFontItalicTrait)
						curStyle |= RWStyle::st_italic;
					if (rv & 4)
					{
						cstyle |= kCTFontBoldTrait;
						if (style & RWStyle::st_bold)
						{
							sstyle |= kCTFontBoldTrait;
							curStyle |= RWStyle::st_bold;
						}
						else
							curStyle &= ~RWStyle::st_bold;
					}
					if (rv & 8)
					{
						cstyle |= kCTFontItalicTrait;
						if (style & RWStyle::st_italic)
						{
							sstyle |= kCTFontItalicTrait;
							curStyle |= RWStyle::st_italic;
						}
						else
							curStyle &= ~RWStyle::st_italic;
					}

					if (rv & 1)
					{
						fontA = RWCTPageComposer::CreateFont (fontName, (long) fontName.size(), size, curStyle);	//mbs 10062010	curStyle instead of sstyle!!!
						//mbs 25052010	CFAttributedStringSetAttribute will crash if the font is the same!
						if (fontA != 0)
						{
							CFStringRef	fn1 = CTFontCopyFamilyName (font);
							CFStringRef	fn2 = CTFontCopyFamilyName (fontA);
							if (CFStringCompare (fn1, fn2, 0) == kCFCompareEqualTo)
							{
								::CFRelease (fontA);
								fontA = 0;
							}
							::CFRelease (fn1);
							::CFRelease (fn2);
						}
					}
					else
						fontA = NULL;

					if (fontA == NULL && cstyle != 0)
					{
						fontA = CTFontCreateCopyWithSymbolicTraits (font, size, NULL, sstyle, cstyle);
						if (fontA == NULL && sstyle == (kCTFontBoldTrait | kCTFontItalicTrait))
						{
							fontA = CTFontCreateCopyWithSymbolicTraits (font, size, NULL, kCTFontBoldTrait, kCTFontItalicTrait | kCTFontBoldTrait);
							if (fontA == NULL)
								fontA = CTFontCreateCopyWithSymbolicTraits (font, size, NULL, kCTFontItalicTrait, kCTFontItalicTrait | kCTFontBoldTrait);
						}
					}

					if (fontA == NULL && size != fSize)
					{
						CFMutableDictionaryRef dict = CFDictionaryCreateMutable (kCFAllocatorDefault, 0, &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
						CTFontDescriptorRef	fdesc = CTFontDescriptorCreateWithAttributes (dict);
						::CFRelease (dict);
						fontA = CTFontCreateCopyWithAttributes (font, size, NULL, fdesc);
						::CFRelease (fdesc);
					}

					if (fontA == NULL)
					{
						if (rv & 16)
						{
							SInt32 l = kCTUnderlineStyleNone;
							if (style & RWStyle::st_underline)
								l = kCTUnderlineStyleSingle | kCTUnderlinePatternSolid;	//•••
							CFNumberRef underline = CFNumberCreate (kCFAllocatorDefault, kCFNumberSInt32Type, &l);	
							CFAttributedStringSetAttribute (mCFText, mCFRange, kCTUnderlineStyleAttributeName, underline);
							::CFRelease (underline);
						}
						if (rv & 32)
						{
							CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
                            CGFloat		f [4] = {color.red / 65535., color.green / 65535.,color.blue / 65535., color.alpha / 65535.};
                            CGColorRef cgcolor = CGColorCreate (rgbColorSpace, f);
							::CGColorSpaceRelease (rgbColorSpace);
							if (cgcolor != 0)
							{
								CFAttributedStringSetAttribute (mCFText, mCFRange, kCTForegroundColorAttributeName, cgcolor);
								::CGColorRelease (cgcolor);
							}
						}
						if (j > i + 4)
							ApplyAttributes (inText, font, attributes, i+2, j-2);
					}
					else
					{
						CFAttributedStringSetAttribute (mCFText, mCFRange, kCTFontAttributeName, fontA);
						if (rv & 16)
						{
							SInt32 l = kCTUnderlineStyleNone;
							if (style & RWStyle::st_underline)
								l = kCTUnderlineStyleSingle | kCTUnderlinePatternSolid;	//•••
							CFNumberRef underline = CFNumberCreate (kCFAllocatorDefault, kCFNumberSInt32Type, &l);	
							CFAttributedStringSetAttribute (mCFText, mCFRange, kCTUnderlineStyleAttributeName, underline);
							::CFRelease (underline);
						}
						if (rv & 32)
						{
							CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
                            CGFloat		f [4] = {color.red / 65535., color.green / 65535.,color.blue / 65535., color.alpha / 65535.};
                            CGColorRef cgcolor = CGColorCreate (rgbColorSpace, f);
							::CGColorSpaceRelease (rgbColorSpace);
							if (cgcolor != 0)
							{
								CFAttributedStringSetAttribute (mCFText, mCFRange, kCTForegroundColorAttributeName, cgcolor);
								::CGColorRelease (cgcolor);
							}
						}
						if (j > i + 4)
							ApplyAttributes (inText, fontA, attributes, i+2, j-2);
						::CFRelease (fontA);
					}
					break;
				}
				// fall through

			case 0:	// unknown/unsupported
				break;
		}
		i = j;
		r = 1;
	}
}


void
RWCTPrintText::Init (RWCTPageComposer &inComposer, const CText inText, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit)
{
	Free();
	std::vector<long>	aattributes;
	long	*attributes = NULL;
	if (inAttributed)
	{
		mText.Attach (RWTools::SplitAttributedString (inText, &aattributes));
		attributes = aattributes.data();
	}
	else
		mText = inText;
	mTextLength = 0;
	mWidth = ioRect.Width();
	mLineHeight = 0;
	mHeight = 0;
	mPrintedHeight = 0;
	mNumLines = -1;
	mPrintedLines = 0;
//	mWrap = inWrap;

	if (not mText.IsEmpty())
	{
		mTextLength = mText.size();
		CFDictionaryRef		styleDict = static_cast <RWCTPageComposer&> (inComposer).MapStyle (mStyle);
		CFStringRef			cfText = RWStr::CreateCFString (mText);
		mCFText = (CFMutableAttributedStringRef) CFAttributedStringCreate (kCFAllocatorDefault, cfText, styleDict);
		::CFRelease (cfText);
		if (inAttributed && attributes && attributes[0] > 2)
		{
			CFMutableAttributedStringRef	mas = CFAttributedStringCreateMutableCopy (kCFAllocatorDefault, mTextLength, mCFText);
			::CFRelease (mCFText);
			mCFText = mas;

			CTFontRef	font = reinterpret_cast <CTFontRef> (CFDictionaryGetValue (styleDict, kCTFontAttributeName));
			ApplyAttributes (inText, font, attributes, 2, attributes[0]);
		}

		SPoint	origTL (ioRect.TopLeft());
		if (mStyle->GetRotation() != 0)
			RWTools::MakeMatrixFromUserRect (ioRect, mStyle->GetRotation(), 0, 0, 0);

		mCTFrames = CTFramesetterCreateWithAttributedString (mCFText);
		CGRect				rect = CGRectMake (0, 0, ioRect.Width() == 0? 4000: ioRect.Width(), inFit? ioRect.Height(): 4000);
		CGMutablePathRef	path = CGPathCreateMutable();
		CGPathAddRect (path, NULL, rect);
		mCFRange = CFRangeMake (0, 0);
		CTFrameRef			frameRef = CTFramesetterCreateFrame (mCTFrames, mCFRange, path, NULL);
		::CFRelease (path);
		CFArrayRef			lineArray = CTFrameGetLines (frameRef);
		CFIndex				j, lineCount = CFArrayGetCount (lineArray);
		CGFloat				ascent, descent, leading;
		for (j = 0; j < lineCount; j++)
		{
			CTLineRef	currentLine = (CTLineRef) CFArrayGetValueAtIndex (lineArray, j);
			double		width = CTLineGetTypographicBounds (currentLine, &ascent, &descent, &leading);
			if (j == 0)
			{
				mLineHeight = ascent + descent;
				mHeight = mLineHeight + leading;
				mWidth = width;
				mNumLines = lineCount;
			}
			else
			{
				if (width > mWidth)
					mWidth = width;
				mHeight += mLineHeight + leading;
			}
		}
		/*
		 When using the origins to calculate measurements for a frame's
		 contents, remember that line origins do not always correspond to
		 line metrics; paragraph style settings can affect line origins,
		 for one. The overall typographic bounds of a frame may generally
		 be calculated as the difference between the top of the frame and
		 the descent of the last line. This will obviously exclude any
		 spacing following the last line, but such spacing has no effect
		 on framesetting in the first place.
		 
		 */
		if (lineCount > 0)
		{
			CGPoint	last;
			CTFrameGetLineOrigins (frameRef, CFRangeMake (lineCount - 1, 1), &last);
			mHeight = rect.size.height - last.y + descent + 0.05;
		}
		::CFRelease (frameRef);

		ioRect.bottom = ioRect.top + mHeight;
		ioRect.right = ioRect.left + mWidth;
		if (mStyle->GetRotation() != 0)
		{
			RWTools::MakeUserRectFromText (ioRect, mStyle->GetRotation());
			ioRect.bottom = origTL.v + ioRect.Height();
			ioRect.right = origTL.h + ioRect.Width();
			ioRect.top = origTL.v;
			ioRect.left = origTL.h;
		}
	}
	else
	{
		mWidth = 0;
		mNumLines = 0;
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
	}

	return;
}


void
RWCTPrintText::Reset (void)
{
	mPrintedHeight = 0;
	mPrintedLines = 0;
	mCFRange = CFRangeMake (0, 0);
}


void
RWCTPrintText::GetBounds (RWCTPageComposer &inComposer, SRect &ioRect, bool inFit)
{
	if (mNumLines > 0)
	{
		ioRect.bottom = ioRect.top + mHeight;	// - mPrintedHeight;
		ioRect.right = ioRect.left + mWidth;
		if (mStyle->GetRotation() != 0)
		{
			SRect	r (ioRect);
			RWTools::MakeUserRectFromText (r, mStyle->GetRotation());
			ioRect.bottom = ioRect.top + r.Height();
			ioRect.right = ioRect.left + r.Width();
		}
	}
	else
	{
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
	}

	return;
}


int
RWCTPrintText::Draw (RWCTPageComposer &inComposer, SRect &ioRect, bool inFit, void *inContext)
{
	if (mNumLines > mPrintedLines)
	{
		RWMacPrintTextContext *ctx = reinterpret_cast <RWMacPrintTextContext*> (inContext);
		if (ctx && ctx->fCGContext)
			CGContextSaveGState (ctx->fCGContext);
		CGRect				rect = CGRectMake (ioRect.left, (ctx? ctx->fPageBottom: 0) - ioRect.bottom, ioRect.Width(), ioRect.Height());
		SPoint				origTL (ioRect.TopLeft());
		if (not inFit && ctx && ctx->fCGContext)
			CGContextClipToRect (ctx->fCGContext, rect);	// if we have to fit, CT will take care, otherwise we clip

		if (mStyle->GetRotation() != 0)
		{
			CGAffineTransform	t = RWTools::MakeMatrixFromUserRect (ioRect, mStyle->GetRotation(), ctx? ctx->fPageBottom: 0, mWidth, mHeight);
#if	0 && TARGET_DEBUG
			const SRGBColor	color	( 65535, 0, 65535, 65535 );
			inComposer.DrawRect (ioRect, 0.25, true, color, false, cWhiteColor, 1, 2);
#endif
			if (ctx && ctx->fCGContext)
				CGContextConcatCTM (ctx->fCGContext, t);
#if	0 && TARGET_DEBUG
			inComposer.DrawRect (ioRect, 0.25, true, cBlueColor, false, cWhiteColor, 1, 2);
#endif
		}

		// vertical alignment
		if (ioRect.Height() >= mHeight)
		{
			if (mStyle->GetVerticalJustification() == RWStyle::st_top)
			{
				ioRect.bottom -= ioRect.Height() - mHeight;
			}
			else if (mStyle->GetVerticalJustification() == RWStyle::st_bottom)
			{
				ioRect.top += ioRect.Height() - mHeight;
			}
			else if (mStyle->GetVerticalJustification() == RWStyle::st_center)
			{
				float	delta = (ioRect.Height() - mHeight) / 2;
				ioRect.top += delta;
				ioRect.bottom -= delta;
			}
#if	0 && TARGET_DEBUG
			inComposer.DrawRect (ioRect, 0.25, true, cBlackColor, false, cWhiteColor, .75, 3);
#endif
		}

		rect = CGRectMake (ioRect.left, (ctx? ctx->fPageBottom: 0) - ioRect.bottom, ioRect.Width(), ioRect.Height());
		if (inFit)
		{
			rect.origin.y -= mLineHeight / 2;
			rect.size.height += mLineHeight / 2;
		}
		else
		{
			rect.origin.y -= mLineHeight - 0.05;
			rect.size.height += mLineHeight - 0.05;
		}
		CGMutablePathRef	path = CGPathCreateMutable();
		CGPathAddRect (path, NULL, rect);
		CTFrameRef			frameRef = CTFramesetterCreateFrame (mCTFrames, mCFRange, path, NULL);
		::CFRelease (path);
		if (frameRef)
		{
			if (ctx && ctx->fCGContext)
				CTFrameDraw (frameRef, ctx->fCGContext);
			mCFRange = CTFrameGetVisibleStringRange (frameRef);
			mCFRange.location += mCFRange.length;
			mCFRange.length = 0;
			::CFRelease (frameRef);
			if (mCFRange.location == mTextLength)
				mPrintedLines = mNumLines;
		}

#if	0 && TARGET_DEBUG
		inComposer.DrawRect (ioRect, 0.5, true, cGreenColor, false, cWhiteColor, 0.5, 1);
#endif
		if (ctx && ctx->fCGContext)
			CGContextRestoreGState (ctx->fCGContext);
		
		if (mStyle->GetRotation() != 0)
		{
			RWTools::MakeUserRectFromText (ioRect, mStyle->GetRotation());
			ioRect.bottom = origTL.v + ioRect.Height();
			ioRect.right = origTL.h + ioRect.Width();
			ioRect.top = origTL.v;
			ioRect.left = origTL.h;
		}
#if	0 && TARGET_DEBUG
		inComposer.DrawRect (ioRect, 0.5, true, cRedColor, false, cWhiteColor, 0.5, 0.5);
#endif
		return 1;
	}
	return 0;
}


double	RWCTPageComposer::MeasureWord (const CText inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading)
{
	CFDictionaryRef			styleDict = MapStyle (inStyle);
	CFStringRef				cfText = RWStr::CreateCFString (RWStringView (inText).substr (0, size_t (std::max (inTextLength, 0))));
	CFAttributedStringRef	attrString = (CFMutableAttributedStringRef) CFAttributedStringCreate (kCFAllocatorDefault, cfText, styleDict);
	CFRelease (cfText);
	CTLineRef				line = CTLineCreateWithAttributedString (attrString); 
	CFRelease (attrString);
	CGFloat ascent, descent, leading;
	double width = CTLineGetTypographicBounds (line, &ascent, &descent, &leading);
	outAscent = ascent;
	outDescent = descent;
	outLeading = leading;
	CFRelease (line); 
	return width;
}


void	RWCTPageComposer::DrawWord (const CText inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle)
{
	if (mPageIsOpen)
	{
		CFDictionaryRef			styleDict = MapStyle (inStyle);
		CFStringRef				cfText = RWStr::CreateCFString (RWStringView (inText).substr (0, size_t (std::max (inTextLength, 0))));
		CFAttributedStringRef	attrString = (CFMutableAttributedStringRef) CFAttributedStringCreate (kCFAllocatorDefault, cfText, styleDict);
		CFRelease (cfText);
		CTLineRef				line = CTLineCreateWithAttributedString (attrString); 
		CFRelease (attrString);
		CGContextSaveGState (mGC);

		float hScale = inStyle->GetHorizontalScale();
		if (hScale != 1)
		{
			CGAffineTransform	matrix = CGAffineTransformMake(hScale, 0, 0, 1, 0, 0);
			CGContextConcatCTM (mGC, matrix);
			CGContextSetTextPosition (mGC, inX / hScale, mPageRect.bottom - inBaseLine); 
		}
		else
			CGContextSetTextPosition (mGC, inX, mPageRect.bottom - inBaseLine); 

        CTLineDraw (line, mGC);
		
		CFRelease (line); 
		CGContextRestoreGState (mGC);
	}
}

bool	RWCTPageComposer::IsUnicodeFont (RWStyle *inStyle)
{
	CText font = inStyle->GetFName();
	if (font == u"Zapf Dingbats")
		return false;
	
	return true;
}
