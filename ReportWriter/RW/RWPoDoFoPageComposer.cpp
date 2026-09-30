#undef	CreateFont
#undef	DrawText
# include	"RWPoDoFoPageComposer.h"
# include	"RWStyle.h"
//# include	<string.h>
//# include	<ctype.h>
# include	<math.h>
# include	"podofo/podofo.h"
# include	"ft2build.h"
# include	"freetype/freetype.h"

# include	<stdio.h>
# include	"theVersion.h"	//mbs 20072011	for SetCreator()

FILE * memfopen (void *mem, long size);

typedef unsigned char  Byte;  /* 8 bits */

#ifdef PODOFO_HAVE_TIFF_LIB
extern "C" {
#  include "tiffio.h"
#  ifdef _WIN32		// Collision between tiff and jpeg-headers
#    ifndef XMD_H
#    define XMD_H
#    endif
//#    undef FAR
//#    define FAR
#  endif
}
#endif // PODOFO_HAVE_TIFF_LIB

#ifdef PODOFO_HAVE_JPEG_LIB
extern "C" {
#  ifndef XMD_H
#    define XMD_H
#  endif
#  include "jpeglib.h"
}
#endif // PODOFO_HAVE_JPEG_LIB

#ifdef PODOFO_HAVE_PNG_LIB
#include <png.h>
#endif /// PODOFO_HAVE_PNG_LIB

using namespace PoDoFo;

# define	PDF_Stroke		"S"
# define	PDF_Fill		"f"
# define	PDF_StrokeFill	"B"

# define	PDF_EMBEDD_FONT		1		// 0 = don't embedd, 1 = embedd if not PDF font, 2 = always


# if PDF_EMBEDD_FONT == 1
const char	*	RWPoDoFoPageComposer::PDFFonts[] =
{
	"Courier",
	"Courier-Bold",
	"Courier-BoldOblique",
	"Courier-Oblique",
	"Helvetica",
	"Helvetica-Bold",
	"Helvetica-BoldOblique",
	"Helvetica-Oblique",
	"Symbol",
	"Times-Bold",
	"Times-BoldItalic",
	"Times-Italic",
	"Times-Roman",
	"ZapfDingbats",
	0
};
# endif


// Composer specific object for picture rendering
struct	RWPdfPictData	:	public	RWPictData
{
public:
	inline					RWPdfPictData (void);
							~RWPdfPictData (void);

	PdfImage	*fImage;
//	double		fScale;	//mbs 20072011
};


inline	RWPdfPictData::RWPdfPictData (void)
	:	RWPictData(),
		fImage (0)
//		fScale (1)
{
}

RWPdfPictData::~RWPdfPictData (void)
{
	if (fImage)
		delete fImage;
}



// ---------------------------------------------------------------------------
// RWPoDoFoPageComposer						Default Constructor		  [public]
// ---------------------------------------------------------------------------

RWPoDoFoPageComposer::RWPoDoFoPageComposer (unsigned long inFlags, CText &inDst, CText &inPrinter)	//mbs 25072011	printer
	:	RWPageComposer (inFlags, inDst, inPrinter),	//mbs 25072011	printer
		mPDF (0),
		mPage (0),
		mPainter (0)
{
	return;
}


/*
// ---------------------------------------------------------------------------
// RWPoDoFoPageComposer						Constructor			   [protected]
// ---------------------------------------------------------------------------

RWPoDoFoPageComposer::RWPoDoFoPageComposer (const RWPoDoFoPageComposer &inOriginal)
	:	RWPageComposer (inOriginal),
		mPDF (0),
		mPage (0),
		mPainter (0)
{
	*this = inOriginal;
	return;
}


// ---------------------------------------------------------------------------
// operator =													   [protected]
// ---------------------------------------------------------------------------

RWPoDoFoPageComposer&
RWPoDoFoPageComposer::operator = (const RWPoDoFoPageComposer &inOriginal)
{
	CloseSession (true);
	StyleChanged (NULL);
	RWPageComposer::operator = (inOriginal);

	return *this;
}
*/


// ---------------------------------------------------------------------------
// ~RWPoDoFoPageComposer					Destructor				  [public]
// ---------------------------------------------------------------------------

RWPoDoFoPageComposer::~RWPoDoFoPageComposer (void)
{
	CloseSession (true);
    StyleChanged (NULL);

	return;
}

// ---------------------------------------------------------------------------
// StyleChanged                                                          [public]
// ---------------------------------------------------------------------------

void
RWCTPageComposer::StyleChanged (RWStyle *inStyle)
{
    if (inStyle == NULL)
    {
        RWStyleToPdfFontMap::const_iterator    it;
        
        for (it = mStyleMap.begin(); it != mStyleMap.end(); it++)
        {
            PdfFont*    font = (*it).second;
//            delete (font); // owned by document
        }
        mStyleMap.clear();
    }
    else
    {
        RWStyleToPdfFontMap::key_type v (*inStyle);
        RWStyleToPdfFontMap::iterator    it = mStyleMap.find (v);
        if (it != mStyleMap.end())
        {
            PdfFont*    font = (*it).second;
//            delete (font);
            mStyleMap.erase (it);
        }
    }
}



// ---------------------------------------------------------------------------
// MapStyle														   [protected]
// ---------------------------------------------------------------------------
PoDoFo::PdfFont*
RWPoDoFoPageComposer::MapStyle (RWStyle *inStyle)
{
	int	embedd = PDF_EMBEDD_FONT == 2;

# if PDF_EMBEDD_FONT == 1
	const char	**p;
	for (p = PDFFonts; *p; p++)
		if (TEXT_EQUALS (inStyle->GetFName(), *p))
			break;
	embedd = (*p == 0 ? 1 : 0);
# endif

	if ((mFlags & ePDFDontEmbedFonts) != 0)	//mbs 30062010	set this bit if fonts have to be NOT embedded
		embedd = false;

	PdfFont*	font = NULL;

    
    RWStyleToPdfFontMap::key_type v (*inStyle);
    RWStyleToPdfFontMap::iterator    it = mStyleMap.find (v);
    if (it != mStyleMap.end())
    {
        font = (*it).second;
    }
        
    if (font == NULL)
    {
		font = mPDF->CreateFont ((const wchar_t*) inStyle->GetFName(),
			(inStyle->GetStyle() & RWStyle::st_bold) != 0,
			(inStyle->GetStyle() & RWStyle::st_italic) != 0,
			new PdfIdentityEncoding(), embedd);
		if (font == NULL)
		{
			CText	us ((const wchar_t*) inStyle->GetFName());
			printf ("RWPoDoFoPageComposer::MapStyle: CreateFont: font '%s' not found!\n", us.GetUTF8());
			font = mPDF->CreateFont (L"Arial",
				(inStyle->GetStyle() & RWStyle::st_bold) != 0,
				(inStyle->GetStyle() & RWStyle::st_italic) != 0,
				new PdfIdentityEncoding(), embedd);
		}
		font->SetUnderlined (inStyle->GetStyle() & RWStyle::st_underline);
		font->SetStrikeOut (inStyle->GetStyle() & RWStyle::st_strikethrough);
	
        mStyleMap.insert (RWStyleToPdfFontMap::value_type (*inStyle, font));
    }

	return font;
}


// ---------------------------------------------------------------------------
// OpenSession													   [protected]
// ---------------------------------------------------------------------------

long
RWPoDoFoPageComposer::OpenSession (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation)
{
	long		status = noErr;

	if (mBatchLevel == 0)
	{
#if	WINVER
//		CText	name (mDestination, CText::_nullTerminated_, true);
		mPDF = new PdfStreamedDocument (mDestination.GetWStr());	// name.GetWStr());
#else
		mPDF = new PdfStreamedDocument (mDestination.GetFSName());
#endif
		mPainter = new PdfPainter;

		if (mPDF != NULL)
		{
			PdfInfo	*info = mPDF->GetInfo();
			info->SetCreator (kProductNameString " " kVersionString);	//mbs 20072011	was "Report Writer 1.0"
			info->SetKeywords ("report");

			if (mJobName)
			{
				PdfString	s (reinterpret_cast <const wchar_t*> ((const CText) mJobName));
				info->SetTitle (s);
			}
		}
	}
/*
	else
	{
		mPDF = static_cast <RWPoDoFoPageComposer*> (mBatch)->mPDF;
		mPainter = static_cast <RWPoDoFoPageComposer*> (mBatch)->mPainter;
	}
*/

	return status;
}


// ---------------------------------------------------------------------------
// CloseSession													   [protected]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::CloseSession (bool inRelease)
{
	ClosePage();	//mbs 08102010	always close page

	if (mBatchLevel == 0)
	{
		if (inRelease)
		{
			if (mPainter != NULL)
			{
				delete mPainter;
				mPainter = NULL;
			}
			if (mPDF != NULL)
			{
				mPDF->Close();
				delete mPDF;
				mPDF = NULL;
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// FinishReport													   [protected]
// ---------------------------------------------------------------------------
// Close the page, close the PDF

void*
RWPoDoFoPageComposer::FinishReport (size_t &outSize)
{
	CloseSession (true);

	outSize = 0;

	return NULL;
}


// ---------------------------------------------------------------------------
// ParseReport													   [protected]
// ---------------------------------------------------------------------------
// Get default page size, page orientation and encoding
// Initialize PDF (title, creator, ...)

void
RWPoDoFoPageComposer::ParseReport (RWXmlNode inReport)
{
//	if (mBatchLevel == 0)
	{
		mTruePageRect.SetRect (0, 0, 0, 0);
		RWPageComposer::ParseReport (inReport);

//		mEncoding = inReport->Attribute ("Encoding");
//		if (mEncoding == NULL)
//			mEncoding = cDefEncoding;
	}

	return;
}


// ---------------------------------------------------------------------------
// GetPageBounds													  [public]
// ---------------------------------------------------------------------------
// Get default page size, page orientation and encoding

void
RWPoDoFoPageComposer::GetPageBounds (const CText inOrientation, const CText inSize, SRect &outRect)
{
	if (mPageRect.Width() == 0)
	{
		RWPageComposer::GetPageBounds (inOrientation, inSize, mPageRect);
//		mPageRect -= mPageRect.TopLeft();
	}

	if (mTruePageRect.Width() == 0)
	{
		mTruePageRect = mPageRect;
		if (mUseReportMargins)
		{
			mOffset = SPoint (0, 0);
			mPageRect.bottom = mTruePageRect.top + mPaperRect.Height();
			mPageRect.right = mTruePageRect.left + mPaperRect.Width();
		}
		else
			mOffset = SPoint (mReportPageMargins.left, mReportPageMargins.top);
	}

	outRect = mPageRect;

	return;
}


SRect
RWPoDoFoPageComposer::GetTruePageRect (void) const
{
	if (mUseReportMargins)
		return mTruePageRect + SPoint (mTruePageRect.left - mPaperRect.left, mTruePageRect.top - mPaperRect.top);
	return mTruePageRect;
}


// ---------------------------------------------------------------------------
// OpenNewPage														  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::OpenNewPage (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages)
{
	ClosePage();

	if (inCurPage < mFirstPage)
		return;
	if (inCurPage > mLastPage)
		return;

	if (mPDF == NULL)
		OpenSession (false, true, inCurPage, inNumPages, TEXT_EQUALS (mPageOrientation, "Landscape"));

	if (mPDF != NULL)
	{
		PdfRect	r (0, 0, mPaperRect.Width(), mPaperRect.Height());
		mPage = mPDF->CreatePage (r);
		mPainter->SetPage (mPage);
		mPageIsOpen = true;

#if	TARGET_DEBUG
		SRect		rect (mTruePageRect);
//		rect *= 1;
		if (mUseReportMargins)
			rect -= mPaperRect.TopLeft();
		DrawRect (rect, 0.25, true, cRedColor, false, cRedColor, 2, 1);
		rect = mPaperRect;
//		rect *= 1;
		if (mUseReportMargins)
			rect -= mPaperRect.TopLeft();
		DrawRect (rect, 0.25, true, cBlueColor, false, cBlueColor);

#if	0
		PdfImage image( mPDF );
		image.LoadFromFile( "C://Test/lena.jpg" );
		mPainter->DrawImage( 0.0, mPage->GetPageSize().GetHeight() - image.GetHeight(), &image );
#endif
#endif
	}

	return;
}


// ---------------------------------------------------------------------------
// ClosePage														  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::ClosePage (void)
{
	if (mPageIsOpen)
	{
		mPainter->FinishPage();
		mPage = NULL;
		mPageIsOpen = false;
	}

	return;
}


// ---------------------------------------------------------------------------
// ClipToRect														  [public]
// ---------------------------------------------------------------------------

RWClipInfoRef
RWPoDoFoPageComposer::ClipToRect (const SRect &inRect)
{
	if (mPageIsOpen)
	{
		mPainter->Save();
		mPainter->SetClipRect (inRect.left + mOffset.h, mPageRect.bottom - inRect.bottom + mOffset.v, inRect.Width(), inRect.Height());
		return (RWClipInfoRef) 1;
	}
	return (RWClipInfoRef) 0;
}


// ---------------------------------------------------------------------------
// RestoreClip														  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::RestoreClip (RWClipInfoRef &ioClipInfo)
{
	if (ioClipInfo)
	{
		mPainter->Restore();
		ioClipInfo = 0;
	}
	return;
}


// ---------------------------------------------------------------------------
// DrawLine															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::DrawLine (float top, float left, float bottom, float right, float inThickness, SRGBColor inLineColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
		float	half = inThickness / 2;
		mPainter->Save();
		mPainter->SetStrokeWidth (inThickness);
		mPainter->SetStrokingColor (inLineColor.red / 65535., inLineColor.green / 65535., inLineColor.blue / 65535. /*, inLineColor.alpha / 65535. */);
		if (inLineLen > 0 && inSpaceLen > 0)
		{
			char	buf [32];
			snprintf (buf, sizeof (buf), "[%g %g] 0", inLineLen, inSpaceLen);	// TODO
			mPainter->SetStrokeStyle (ePdfStrokeStyle_Custom, buf);
		}

#if 1	// compensate for thickness - QD draws differently than CG
		if (left == right)
		{
			left += half;
			mPainter->MoveTo (left + mOffset.h, mPageRect.bottom - top + mOffset.v);
			mPainter->LineTo (left + mOffset.h, mPageRect.bottom - bottom + mOffset.v);
		}
		else if (top == bottom)
		{
			top += half;
			mPainter->MoveTo (left + mOffset.h, mPageRect.bottom - top + mOffset.v);
			mPainter->LineTo (right + mOffset.h, mPageRect.bottom - top + mOffset.v);
		}
		else
		{
//			left += half;
//			top += half;
			mPainter->MoveTo (left + mOffset.h, mPageRect.bottom - top + mOffset.v);
			mPainter->LineTo (right + mOffset.h, mPageRect.bottom - bottom + mOffset.v);
		}
#else
		mPainter->MoveTo (left, mPageRect.bottom - top);
		mPainter->LineTo (right, mPageRect.bottom - bottom);
#endif
		
		mPainter->Stroke();
		mPainter->Restore();
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawRect															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::DrawRect (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
								bool inFill, SRGBColor inFillColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
		mPainter->Save();
		mPainter->SetStrokeWidth (inThickness);
		if (inFill)
			mPainter->SetColor (inFillColor.red / 65535., inFillColor.green / 65535., inFillColor.blue / 65535. /*, inFillColor.alpha / 65535. */);
		if (inFrame)
			mPainter->SetStrokingColor (inFrameColor.red / 65535., inFrameColor.green / 65535., inFrameColor.blue / 65535. /*, inFrameColor.alpha / 65535. */);
		if (inLineLen > 0 && inSpaceLen > 0)
		{
			char	buf [32];
			snprintf (buf, sizeof (buf), "[%g %g] 0", inLineLen, inSpaceLen);	// TODO
			mPainter->SetStrokeStyle (ePdfStrokeStyle_Custom, buf);
		}
		
		if (inFill)
			if (inFrame)
				mPainter->AddRect (inRect.left + inThickness/2 + mOffset.h, mPageRect.bottom - inRect.bottom + inThickness/2 + mOffset.v,
					inRect.Width() - inThickness, inRect.Height() - inThickness, 0, 0, PDF_StrokeFill);
			else
				mPainter->FillRect (inRect.left + inThickness/2 + mOffset.h, mPageRect.bottom - inRect.bottom + inThickness/2 + mOffset.v,
					inRect.Width() - inThickness, inRect.Height() - inThickness);
		else
			mPainter->DrawRect (inRect.left + inThickness/2 + mOffset.h, mPageRect.bottom - inRect.bottom + inThickness/2 + mOffset.v,
					inRect.Width() - inThickness, inRect.Height() - inThickness);
		mPainter->Restore();
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawOval															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::DrawOval (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
								bool inFill, SRGBColor inFillColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
		mPainter->Save();
		mPainter->SetStrokeWidth (inThickness);
		if (inFill)
			mPainter->SetColor (inFillColor.red / 65535., inFillColor.green / 65535., inFillColor.blue / 65535. /*, inFillColor.alpha / 65535. */);
		if (inFrame)
			mPainter->SetStrokingColor (inFrameColor.red / 65535., inFrameColor.green / 65535., inFrameColor.blue / 65535. /*, inFrameColor.alpha / 65535. */);
		if (inLineLen > 0 && inSpaceLen > 0)
		{
			char	buf [32];
			snprintf (buf, sizeof (buf), "[%g %g] 0", inLineLen, inSpaceLen);	// TODO
			mPainter->SetStrokeStyle (ePdfStrokeStyle_Custom, buf);
		}
		
		if (inFill)
			if (inFrame)
				mPainter->AddEllipse (inRect.left + inThickness/2 + mOffset.h, mPageRect.bottom - inRect.bottom + inThickness/2 + mOffset.v,
					inRect.Width() - inThickness, inRect.Height() - inThickness, PDF_StrokeFill);
			else
				mPainter->FillEllipse (inRect.left + inThickness/2 + mOffset.h, mPageRect.bottom - inRect.bottom + inThickness/2 + mOffset.v,
					inRect.Width() - inThickness, inRect.Height() - inThickness);
		else
			mPainter->DrawEllipse (inRect.left + inThickness/2 + mOffset.h, mPageRect.bottom - inRect.bottom + inThickness/2 + mOffset.v,
					inRect.Width() - inThickness, inRect.Height() - inThickness);
		mPainter->Restore();
	}

	return;
}


// ---------------------------------------------------------------------------
// GetPictBounds													  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::GetPictBounds (SRect &ioRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, bool inGrow)
{
	RWPdfPictData	*pd = NULL;
	if (*cd == NULL)
	{
		//mbs 11082010
		if (mPDF == NULL)
			OpenSession (false, false, 0, 0,  TEXT_EQUALS (mPageOrientation, "Landscape"));

		pd = new RWPdfPictData;
		*cd = pd;
		RWValue	convertedPict;

		convertedPict.Attach (inPicture);
		if (convertedPict.GetKind() < RWValue::eValue_PictureJPG || convertedPict.GetKind() > RWValue::eValue_PictureTIFF)
		{
			RW_ConvertPictureForPrinting (convertedPict, true);
		}

		FILE*	file = memfopen (convertedPict.GetBlobData(), convertedPict.GetBlobSize());

		try
		{
			switch (convertedPict.GetKind())
			{
				case RWValue::eValue_PictureJPG:
					pd->fImage = new PdfImage (mPDF);
					pd->fImage->LoadFromJpegFile (file);
					break;

				case RWValue::eValue_PicturePNG:
					pd->fImage = new PdfImage (mPDF);
					pd->fImage->LoadFromPngFile (file);
					break;

				case RWValue::eValue_PictureTIFF:
					pd->fImage = new PdfImage (mPDF);
					pd->fImage->LoadFromTiffFile (file, "<none>");
					break;
				
				default:	// to shut up compiler
//					fclose (file);
					throw -1L;	// unsupported
					break;
			}
			pd->fWidth = pd->fImage->GetWidth();
			pd->fHeight = pd->fImage->GetHeight();
		}
		catch (...)
		{
			fclose (file);
			delete pd->fImage;
			pd->fImage = NULL;
		}
	}
	else
		pd = static_cast <RWPdfPictData*> (*cd);

	if (inGrow)	// can shrink/expand
	{
//		if (pd->fWidth > ioRect.Width())	// but can't expand horizontally
		{
			float	scalingFactor;	//mbs 20072011
			switch (inSizing)
			{
				case ePictFormat_Centered:				// Truncated (centered)
				case ePictFormat_Normal:				// Truncated (non-centered)
				default:
					ioRect.bottom = ioRect.top + pd->fHeight;
					break;

				case ePictFormat_ScaledToFit:			// Scaled to fit
				case ePictFormat_ScaledProp:			// Scaled to fit (proportional)
				case ePictFormat_ScaledPropCentered:	// Scaled to fit centered (prop.)
					// scale
#if	0	//mbs 06082010
					if ((pd->fWidth - ioRect.Width()) > (pd->fHeight - ioRect.Height()))
						scalingFactor = (double) ioRect.Width() / pd->fWidth;
					else
						scalingFactor = (double) ioRect.Height() / pd->fHeight;
#else
					scalingFactor = (double) ioRect.Width() / pd->fWidth;
					if (scalingFactor > (double) ioRect.Height() / pd->fHeight)
						scalingFactor = (double) ioRect.Height() / pd->fHeight;
#endif

					if (scalingFactor > 1.0)	// don't enlarge a pict
						scalingFactor = 1.0;

					ioRect.bottom = ioRect.top + pd->fHeight * scalingFactor;
					ioRect.right = ioRect.left + pd->fWidth * scalingFactor;
					break;
			} //switch
		}
/*
		else
		{
			ioRect.bottom = ioRect.top + pd->fHeight;
			ioRect.right = ioRect.left + pd->fWidth;
		}
*/
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawPict															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::DrawPict (SRect &inRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, double inRotation, float alfa)
{
	if (mPageIsOpen && *cd == NULL)
	{
		SRect	r (inRect);
		GetPictBounds (r, inPicture, inSizing, cd, false);
	}

	if (mPageIsOpen && *cd)
	{
		RWPdfPictData	*pd = static_cast <RWPdfPictData*> (*cd);
		if (pd->fImage != NULL)
		{
			double		x, y;
			double		hscale = 1, vscale = 1;	//mbs 20072011

            mPainter->Save();

            if (inRotation != 0)
            {
                CGAffineTransform	t = RWTools::MakeMatrixFromUserRect (GetPrintContext(), inRect, GetNativeRotation (inRotation), 0., 0.);	//mbs 29062011
                ApplyTransform (t);
                
            }
            
			switch (inSizing)
			{
				case ePictFormat_Centered:				// Truncated (centered)
					x = inRect.left + (inRect.Width() - pd->fWidth) / 2;
					y = inRect.top + pd->fHeight + (inRect.Height() - pd->fHeight) / 2;
					break;

				case ePictFormat_ScaledToFit:			// Scaled to fit
					hscale = inRect.Width() / pd->fWidth;
					vscale = inRect.Height() / pd->fHeight;
					x = inRect.left;
					y = inRect.bottom;
					break;

				case ePictFormat_ScaledProp:			// Scaled to fit (proportional)
				case ePictFormat_ScaledPropCentered:	// Scaled to fit centered (prop.)
					hscale = inRect.Width() / pd->fWidth;
					if (hscale > inRect.Height() / pd->fHeight)
						hscale = inRect.Height() / pd->fHeight;
					if (hscale > 1.0)	// don't enlarge a pict
						hscale = 1.0;
					vscale = hscale;

					if (inSizing == ePictFormat_ScaledPropCentered)
					{
						// center
						x = inRect.left + (inRect.Width() - pd->fWidth * hscale) / 2;
						y = inRect.bottom - (inRect.Height() - pd->fHeight * vscale) / 2;
					}
					else
					{
						x = inRect.left;
						y = inRect.top + pd->fHeight * vscale;
					}
					break;

				case ePictFormat_Normal:				// Truncated (non-centered)
				default:
					x = inRect.left;
					y = inRect.top + pd->fHeight;
					break;
			} //switch

			StClipToRect	clip (this, inRect);
			mPainter->DrawImage (x + mOffset.h, mPageRect.bottom - y + mOffset.v, pd->fImage, hscale, vscale);
            
            mPainter->Restore();

		}
	}

	return;
}


/*
// ---------------------------------------------------------------------------
// FreePict															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::FreePict (void **cd)
{
	return;
}
*/



// ---------------------------------------------------------------------------
// ApplyTransform													  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::ApplyTransform (CGAffineTransform &inMatrix)
{
	if (mPageIsOpen)
	{
		mPainter->SetTransformationMatrix (inMatrix.a, inMatrix.b, inMatrix.c, inMatrix.d, inMatrix.tx, inMatrix.ty);
	}
}


// ---------------------------------------------------------------------------
// MeasureWord														  [public]
// ---------------------------------------------------------------------------

double
RWPoDoFoPageComposer::MeasureWord (const CText inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading)
{
	if (mPDF == NULL)
		OpenSession (false, false, 0, 0, TEXT_EQUALS (mPageOrientation, "Landscape"));

	if (mPDF != NULL)
	{
		PdfFont			*font = MapStyle (inStyle);
		font->SetFontSize (inStyle->GetSize());
		PdfFontMetrics	*fm = font->GetFontMetrics2();
		double	width = fm->StringWidth (reinterpret_cast <const wchar_t*> (inText), inTextLength);
		outAscent = fm->GetAscent();
		outDescent = -fm->GetDescent();
		outLeading = 0;
//		width *= fm->GetFontSize() / fm->GetFace()->units_per_EM;
		return width;
	}
	return 0;
}


// ---------------------------------------------------------------------------
// DrawWord															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::DrawWord (const CText inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle)
{
	if (mPageIsOpen)
	{
		SRGBColor				textColor = inStyle->GetTextColor();
		PdfFont					*font = MapStyle (inStyle);
		font->SetFontSize (inStyle->GetSize());
		mPainter->Save();
		mPainter->SetColor (textColor.red / 65535., textColor.green / 65535., textColor.blue / 65535. /*, textColor.alpha / 65535. */);
		mPainter->SetFont (font);
		PdfString	text (reinterpret_cast <const wchar_t*> (inText), inTextLength);
		mPainter->DrawText (inX + mOffset.h, mPageRect.bottom - inBaseLine + mOffset.v, text);
		mPainter->Restore();
	}

	return;
}



/*
// ---------------------------------------------------------------------------
// RWPoDoFoFilePageComposer					Default Constructor		  [public]
// ---------------------------------------------------------------------------

RWPoDoFoFilePageComposer::RWPoDoFoFilePageComposer (const char *inFileName)
{
	mPDF = PDF_new2 (PDFErrorHandler, NULL, NULL, NULL, this);
	if (mPDF != NULL)
		if (PDF_open_file (mPDF, inFileName) == -1)
			throw 1L;

	return;
}


// ---------------------------------------------------------------------------
// ~RWPoDoFoFilePageComposer				Destructor				  [public]
// ---------------------------------------------------------------------------

RWPoDoFoFilePageComposer::~RWPoDoFoFilePageComposer (void)
{
	return;
}




// ---------------------------------------------------------------------------
// RWPoDoFoBlobPageComposer					Default Constructor		  [public]
// ---------------------------------------------------------------------------

RWPoDoFoBlobPageComposer::RWPoDoFoBlobPageComposer (void)
{
	mPDF = PDF_new2 (PDFErrorHandler, NULL, NULL, NULL, this);
	if (mPDF != NULL)
		PDF_open_mem (mPDF, RWPoDoFoBlobWriteProc);

	return;
}


// ---------------------------------------------------------------------------
// ~RWPoDoFoBlobPageComposer				Destructor				  [public]
// ---------------------------------------------------------------------------

RWPoDoFoBlobPageComposer::~RWPoDoFoBlobPageComposer (void)
{
	return;
}


// ---------------------------------------------------------------------------
// FinishReport													   [protected]
// ---------------------------------------------------------------------------

void*
RWPoDoFoBlobPageComposer::FinishReport (size_t &outSize)
{
	RWPoDoFoPageComposer::FinishReport (outSize);

	void	*buf = ll.concatenate (outSize);

	return buf;
}
*/




typedef	struct	_Handle
{
	UInt8	*mem;
	fpos_t	position;
	fpos_t	size;
}	_Handle;


# ifdef	_MSL_STDIO_H
#  include	<ansi_files.h>
#  include	<file_io.h>

static	int __read_blob (__file_handle handle, unsigned char * buffer, size_t * count, __ref_con idle_proc);
static	int __write_blob (__file_handle handle, unsigned char * buffer, size_t * count, __ref_con idle_proc);
static	int __position_blob (__file_handle handle, unsigned long * position, int mode, __ref_con idle_proc);
static	int __close_blob (__file_handle handle);


FILE * memfopen (void *mem, long size)
{
	__file_modes	mode;
	_Handle			*blob;
	FILE *			file = NULL;

	if (__get_file_modes ("rb", &mode))
	{
		blob = (_Handle*) malloc (sizeof (_Handle));
		if (blob)
		{
			blob->mem = (UInt8*) mem;
			blob->position = 0;
			blob->size = size;

			file = __handle_open ((__file_handle) blob, "rb");
			if (file)
			{
				file->position_proc    = __position_blob;
				file->read_proc        = __read_blob;
				file->write_proc       = __write_blob;
				file->close_proc       = __close_blob;
				setvbuf (file, NULL, _IONBF, 0);
			}
			else
				free (blob);
		}
	}

	return file;
}


int __read_blob (__file_handle handle, unsigned char * buffer, size_t * count, __ref_con /* idle_proc */)
{
	if (handle)
	{
		_Handle			*blob = (_Handle*) handle;

		if (blob->position + *count > blob->size)
			*count = blob->size - blob->position;

		if (*count > 0)
		{
			memcpy (buffer, blob->mem + blob->position, *count);
			blob->position += *count;
			return (__no_io_error);
		}
		else
			return (__io_EOF);
	}
	return (__io_error);
}


int __write_blob (__file_handle handle, unsigned char * buffer, size_t * count, __ref_con /* idle_proc */)
{
	if (handle)
	{
		if (*count > 0)
		{
			_Handle			*blob = (_Handle*) handle;

			if (blob->position + *count > blob->size)
				return (__io_error);

			memcpy (blob->mem + blob->position, buffer, *count);
			blob->position += *count;
		}

		return (__no_io_error);
	}
	return (__io_error);
}


int __position_blob (__file_handle handle, unsigned long * position, int mode, __ref_con /* idle_proc */)
{
	if (handle)
	{
		_Handle	*blob = (_Handle*) handle;
		long	absPos;

		switch (mode)
		{
			case SEEK_END:
				absPos = blob->size + *((signed long *) position);
				break;

			case SEEK_CUR:
				absPos = blob->position + *((signed long *) position);
				break;

			case SEEK_SET:
				absPos = *((signed long *) position);
				break;

			default:
				return (__io_error);
		}

		if (absPos < 0)
			return (__io_error);

		if (absPos > blob->size)
			return (__io_error);
	  	blob->position = absPos;

		return (__no_io_error);
	}
	return (__io_error);
}


int __close_blob (__file_handle handle)
{
	_Handle			*blob = (_Handle*) handle;

	if (blob)
	{
		free (blob);
	}

	return (__no_io_error);
}

# else	// def	_MSL_STDIO_H

// # include	<errno.h>

static int __read_blob (void * handle, char * buffer, int count);
static int __write_blob (void * handle, const char * buffer, int count);
static fpos_t __position_blob (void * handle, fpos_t position, int mode);
static int __close_blob (void * handle);


FILE * memfopen (void *mem, long size)
{
	_Handle			*blob;
	FILE *			file = NULL;

	if (hdl)
	{
		blob = (_Handle*) malloc (sizeof (_Handle));
		if (blob)
		{
			blob->mem = (UInt8*) mem;
			blob->position = 0;
			blob->size = size;

			file = funopen (blob, __read_blob, __write_blob, __position_blob, __close_blob);
			if (file == NULL)
				free (blob);
			else
				setvbuf (file, NULL, _IONBF, 0);
		}
	}

	return file;
}


int __read_blob (void * handle, char * buffer, int count)
{
	if (handle)
	{
		_Handle			*blob = (_Handle*) handle;

		if (blob->position + count > blob->size)
			count = blob->size - blob->position;

		if (count > 0)
		{
			memcpy (buffer, blob->mem + blob->position, count);
			blob->position += count;
			return (count);
		}
		else
			return (-1);
	}
	return (-1);
}


int __write_blob (void * handle, const char * buffer, int count)
{
	if (handle)
	{
		if (count > 0)
		{
			_Handle			*blob = (_Handle*) handle;

			if (blob->position + count > blob->size)
				return (-1);

			memcpy (blob->mem + blob->position, buffer, count);
			blob->position += count;
		}

		return (count);
	}
	return (-1);
}


fpos_t __position_blob (void * handle, fpos_t position, int mode)
{
	if (handle)
	{
		_Handle	*blob = (_Handle*) handle;
		off_t	absPos;

		switch (mode)
		{
			case SEEK_END:
				absPos = blob->size + (off_t) position;
				break;

			case SEEK_CUR:
				absPos = blob->position + (off_t) position;
				break;

			case SEEK_SET:
				absPos = position;
				break;

			default:
				return (-1);
		}

		if (absPos < 0)
			return (-1);

		if (absPos > blob->size)
			return (-1);
	  	blob->position = absPos;

		return (absPos);
	}
	return (-1);
}


int __close_blob (void * handle)
{
	_Handle			*blob = (_Handle*) handle;

	if (blob)
	{
		free (blob);
	}

	return (0);
}

# endif
