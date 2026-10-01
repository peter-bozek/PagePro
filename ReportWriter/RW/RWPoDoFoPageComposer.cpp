/*
 *  RWPoDoFoPageComposer.cpp
 *  ReportWriter
 *
 *  PDF output with PoDoFo 1.0 (see RWPoDoFoPageComposer.h).
 *
 *  Coordinates: the report uses a top-left origin, PDF a bottom-left one;
 *  PdfX / PdfY convert, mOffset shifts by the report margins when the paper
 *  margins are not used (same geometry as the PoDoFo 0.9 composer).
 */

# include	"RWPoDoFoPageComposer.h"
# include	"RWPdfFonts.h"
# include	"RWStyle.h"
# include	"theVersion.h"
#if	MACVER
# include	"RWStringCF.h"
#endif

#undef	CreateFont
#undef	DrawText
# include	<podofo/podofo.h>

# include	<cmath>
# include	<cstdio>

using namespace PoDoFo;

namespace
{
	const double	kSyntheticItalicSkew = 0.2;		// tan (about 11°)
	const double	kSyntheticBoldStroke = 0.03;		// outline width, fraction of the font size

	inline	PdfColor	ToPdfColor (SRGBColor inColor)
	{
		return PdfColor (inColor.red / 65535., inColor.green / 65535., inColor.blue / 65535.);
	}

	void	LogPdfError (const char *inWhere, const PdfError &inError)
	{
		printf ("RWPoDoFoPageComposer::%s: %s\n", inWhere, inError.what());
		fflush (stdout);
	}

	RWString	FileSystemPath (const RWString &inDestination)
	{
#if	MACVER
		return RWStr::POSIXPathFromHFS (inDestination);	// 4D passes HFS paths on the Mac
#else
		return inDestination;
#endif
	}
}


unsigned long	RWPoDoFoPageComposer::sDocumentCounter = 0;


// Composer specific object for picture rendering
struct	RWPdfPictData	:	public	RWPictData
{
	std::unique_ptr<PdfImage>	fImage;
	unsigned long				fDocument = 0;			// the image belongs to this document (RWPoDoFoPageComposer::mDocumentSerial)
};


// ---------------------------------------------------------------------------
// RWPoDoFoPageComposer						Constructor				  [public]
// ---------------------------------------------------------------------------

RWPoDoFoPageComposer::RWPoDoFoPageComposer (unsigned long inFlags, CText &inDst, CText &inPrinter)
	:	RWPageComposer (inFlags, inDst, inPrinter),
		mPage (nullptr),
		mDocumentSerial (0),
		mSaveFailed (false)
{
}


// ---------------------------------------------------------------------------
// ~RWPoDoFoPageComposer					Destructor				  [public]
// ---------------------------------------------------------------------------

RWPoDoFoPageComposer::~RWPoDoFoPageComposer (void)
{
	mBatchLevel = 0;
	CloseSession (true);
}


// ---------------------------------------------------------------------------
// StyleChanged														  [public]
// ---------------------------------------------------------------------------
// Fonts are cached per family and style for the open document, nothing depends
// on other style attributes.

void
RWPoDoFoPageComposer::StyleChanged (RWStyle *)
{
}


// ---------------------------------------------------------------------------
// MapStyle														   [protected]
// ---------------------------------------------------------------------------
// The font the native composer uses for the style, embedded as a subset (or not
// embedded with ePDFDontEmbedFonts). Falls back to the default font, then to the
// standard Helvetica (not embedded, Latin only).

const RWPoDoFoPageComposer::FontEntry&
RWPoDoFoPageComposer::MapStyle (RWStyle *inStyle)
{
	const int		style = inStyle->GetStyle() & (RWStyle::st_bold | RWStyle::st_italic);
	const FontKey	key (inStyle->GetFName(), style);

	auto	found = mFonts.find (key);
	if (found != mFonts.end())
		return found->second;

	FontEntry			entry;
	PdfFontCreateParams	params;
	if ((mFlags & ePDFDontEmbedFonts) != 0)
		params.Flags = PdfFontCreateFlags::DontEmbed;

	const RWString	defaultFamily = RWStr::FromASCII (RWStyle::cDefFontName);
	for (RWStringView family : { RWStringView (key.first), RWStringView (defaultFamily) })
	{
		RWPdfFontProgram	program;
		if (!RWPdfFonts::LoadFont (family, style, program))
		{
			printf ("RWPoDoFoPageComposer::MapStyle: font '%s' not found\n", RWStr::ToUTF8 (family).c_str());
			continue;
		}
		try
		{
			entry.font = &mPDF->GetFonts().GetOrCreateFontFromBuffer (bufferview (program.data.data(), program.data.size()), params);
			entry.syntheticBold = program.syntheticBold;
			entry.syntheticItalic = program.syntheticItalic;
			entry.ascent = program.ascent;
			entry.descent = program.descent;
			entry.lineGap = program.lineGap;
			break;
		}
		catch (const PdfError &e)
		{
			LogPdfError ("MapStyle", e);
		}
	}

	if (entry.font == nullptr)
	{
		PdfStandard14FontType	type = PdfStandard14FontType::Helvetica;
		switch (style)
		{
			case RWStyle::st_bold:							type = PdfStandard14FontType::HelveticaBold; break;
			case RWStyle::st_italic:						type = PdfStandard14FontType::HelveticaOblique; break;
			case RWStyle::st_bold | RWStyle::st_italic:		type = PdfStandard14FontType::HelveticaBoldOblique; break;
		}
		entry.font = &mPDF->GetFonts().GetStandard14Font (type);
	}

	return mFonts.emplace (key, entry).first->second;
}


// ---------------------------------------------------------------------------
// EnsureDocument												   [protected]
// ---------------------------------------------------------------------------
// Text measurement and pictures need the document (fonts, images) before the
// first page is opened.

bool
RWPoDoFoPageComposer::EnsureDocument (void)
{
	if (mPDF == nullptr)
		OpenSession (false, false, 0, 0, TEXT_EQUALS (mPageOrientation, "Landscape"));
	return mPDF != nullptr;
}


// ---------------------------------------------------------------------------
// OpenSession													   [protected]
// ---------------------------------------------------------------------------

OSStatus
RWPoDoFoPageComposer::OpenSession (bool, bool, unsigned long, unsigned long, bool)
{
	if (mPDF != nullptr)
		return noErr;

	try
	{
		mPDF.reset (new PdfMemDocument());
		mPainter.reset (new PdfPainter());
		mDocumentSerial = ++sDocumentCounter;
		mFonts.clear();
		mPDFData.clear();
		mSaveFailed = false;

		PdfMetadata	&info = mPDF->GetMetadata();
		info.SetCreator (PdfString (kProductNameString " " kVersionString));
		info.SetKeywords ({ "report" });
		if (!mJobName.empty())
			info.SetTitle (PdfString (RWStr::ToUTF8 (mJobName)));
	}
	catch (const PdfError &e)
	{
		LogPdfError ("OpenSession", e);
		mPainter.reset();
		mPDF.reset();
		return -1;
	}

	return noErr;
}


// ---------------------------------------------------------------------------
// CloseSession													   [protected]
// ---------------------------------------------------------------------------
// Saves the document (fonts are subset here) and writes it to the destination.
// Inside a batch (RW_OpenSession) the document stays open for the next report.

void
RWPoDoFoPageComposer::CloseSession (bool inRelease)
{
	ClosePage();

	if (mBatchLevel != 0 || !inRelease || mPDF == nullptr)
		return;

	try
	{
		if (mPDF->GetPages().GetCount() == 0)
			mPDF->GetPages().CreatePage (PoDoFo::Rect (0, 0, mPaperRect.Width(), mPaperRect.Height()));	// a PDF needs a page

		StringStreamDevice	device (mPDFData);
		mPDF->Save (device);
	}
	catch (const PdfError &e)
	{
		LogPdfError ("CloseSession", e);
		mPDFData.clear();
		mSaveFailed = true;
	}

	if (!mPDFData.empty() && !mDestination.empty())
		if (!RWStr::WriteFile (FileSystemPath (mDestination), mPDFData))
		{
			printf ("RWPoDoFoPageComposer::CloseSession: could not write '%s'\n", RWStr::ToUTF8 (mDestination).c_str());
			mSaveFailed = true;
		}

	mFonts.clear();
	mPainter.reset();
	mPDF.reset();
}


// ---------------------------------------------------------------------------
// FinishReport														  [public]
// ---------------------------------------------------------------------------

void*
RWPoDoFoPageComposer::FinishReport (size_t &outSize)
{
	CloseSession (true);
	outSize = 0;
	return NULL;
}


// ---------------------------------------------------------------------------
// ParseReport														  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::ParseReport (RWXmlNode inReport)
{
	mTruePageRect.SetRect (0, 0, 0, 0);
	RWPageComposer::ParseReport (inReport);
}


// ---------------------------------------------------------------------------
// GetPageBounds													  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::GetPageBounds (const CText inOrientation, const CText inSize, SRect &outRect)
{
	if (mPageRect.Width() == 0)
		RWPageComposer::GetPageBounds (inOrientation, inSize, mPageRect);

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
RWPoDoFoPageComposer::OpenNewPage (const SRect &, unsigned long inCurPage, unsigned long)
{
	ClosePage();

	if (inCurPage < mFirstPage || inCurPage > mLastPage)
		return;
	if (!EnsureDocument())
		return;

	try
	{
		mPage = &mPDF->GetPages().CreatePage (PoDoFo::Rect (0, 0, mPaperRect.Width(), mPaperRect.Height()));
		mPainter->SetCanvas (*mPage);
		mPageIsOpen = true;
	}
	catch (const PdfError &e)
	{
		LogPdfError ("OpenNewPage", e);
		mPage = nullptr;
	}
}


// ---------------------------------------------------------------------------
// ClosePage														  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::ClosePage (void)
{
	if (mPageIsOpen)
	{
		try
		{
			mPainter->FinishDrawing();
		}
		catch (const PdfError &e)
		{
			LogPdfError ("ClosePage", e);
		}
		mPage = nullptr;
		mPageIsOpen = false;
	}
}


// ---------------------------------------------------------------------------
// ClipToRect / RestoreClip											  [public]
// ---------------------------------------------------------------------------

RWClipInfoRef
RWPoDoFoPageComposer::ClipToRect (const SRect &inRect)
{
	if (!mPageIsOpen)
		return (RWClipInfoRef) 0;

	mPainter->Save();
	mPainter->SetClipRect (PdfX (inRect.left), PdfY (inRect.bottom), inRect.Width(), inRect.Height());
	return (RWClipInfoRef) 1;
}


void
RWPoDoFoPageComposer::RestoreClip (RWClipInfoRef &ioClipInfo)
{
	if (ioClipInfo && mPageIsOpen)
		mPainter->Restore();
	ioClipInfo = 0;
}


// ---------------------------------------------------------------------------
// Drawing state helpers										   [protected]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::SetFillColor (SRGBColor inColor)
{
	mPainter->GraphicsState.SetNonStrokingColor (ToPdfColor (inColor));
}

void
RWPoDoFoPageComposer::SetStrokeColor (SRGBColor inColor)
{
	mPainter->GraphicsState.SetStrokingColor (ToPdfColor (inColor));
}

void
RWPoDoFoPageComposer::SetDash (float inLineLen, float inSpaceLen)
{
	if (inLineLen > 0 && inSpaceLen > 0)
	{
		const double	dash [] = { inLineLen, inSpaceLen };
		mPainter->SetStrokeStyle (cspan<double> (dash, 2), 0);
	}
}


// ---------------------------------------------------------------------------
// DrawLine															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::DrawLine (float top, float left, float bottom, float right, float inThickness, SRGBColor inLineColor, float inLineLen, float inSpaceLen)
{
	if (!mPageIsOpen)
		return;

	mPainter->Save();
	mPainter->GraphicsState.SetLineWidth (inThickness);
	SetStrokeColor (inLineColor);
	SetDash (inLineLen, inSpaceLen);

	// compensate for the thickness - QuickDraw drew below / right of the line
	const float	half = inThickness / 2;
	if (left == right)
		left = right = left + half;
	else if (top == bottom)
		top = bottom = top + half;
	mPainter->DrawLine (PdfX (left), PdfY (top), PdfX (right), PdfY (bottom));

	mPainter->Restore();
}


// ---------------------------------------------------------------------------
// DrawRect															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::DrawRect (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
								bool inFill, SRGBColor inFillColor, float inLineLen, float inSpaceLen)
{
	if (!mPageIsOpen || (!inFrame && !inFill))
		return;

	mPainter->Save();
	mPainter->GraphicsState.SetLineWidth (inThickness);
	if (inFill)
		SetFillColor (inFillColor);
	if (inFrame)
		SetStrokeColor (inFrameColor);
	SetDash (inLineLen, inSpaceLen);

	const PdfPathDrawMode	mode = inFill ? (inFrame ? PdfPathDrawMode::StrokeFill : PdfPathDrawMode::Fill) : PdfPathDrawMode::Stroke;
	mPainter->DrawRectangle (PdfX (inRect.left + inThickness / 2), PdfY (inRect.bottom - inThickness / 2),
							 inRect.Width() - inThickness, inRect.Height() - inThickness, mode);

	mPainter->Restore();
}


// ---------------------------------------------------------------------------
// DrawOval															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::DrawOval (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
								bool inFill, SRGBColor inFillColor, float inLineLen, float inSpaceLen)
{
	if (!mPageIsOpen || (!inFrame && !inFill))
		return;

	mPainter->Save();
	mPainter->GraphicsState.SetLineWidth (inThickness);
	if (inFill)
		SetFillColor (inFillColor);
	if (inFrame)
		SetStrokeColor (inFrameColor);
	SetDash (inLineLen, inSpaceLen);

	const PdfPathDrawMode	mode = inFill ? (inFrame ? PdfPathDrawMode::StrokeFill : PdfPathDrawMode::Fill) : PdfPathDrawMode::Stroke;
	mPainter->DrawEllipse (PdfX (inRect.left + inThickness / 2), PdfY (inRect.bottom - inThickness / 2),
						   inRect.Width() - inThickness, inRect.Height() - inThickness, mode);

	mPainter->Restore();
}


// ---------------------------------------------------------------------------
// GetPictBounds													  [public]
// ---------------------------------------------------------------------------
// JPEG is embedded as it is (DCTDecode), PNG is decoded and Flate compressed,
// its alpha channel becomes a soft mask. RW_ConvertPictureForPrinting delivers
// one of the two (SR4DData::GetPictureFrom4D).

void
RWPoDoFoPageComposer::GetPictBounds (SRect &ioRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, bool inGrow)
{
	RWPdfPictData	*pd = static_cast <RWPdfPictData*> (*cd);
	if (pd == nullptr)
	{
		pd = new RWPdfPictData;
		*cd = pd;
	}

	if (EnsureDocument() && pd->fDocument != mDocumentSerial)
	{
		// first use, or cached for a document that was closed
		pd->fImage.reset();
		pd->fWidth = pd->fHeight = 0;
		pd->fDocument = mDocumentSerial;

		RWValue	convertedPict;
		convertedPict.Attach (inPicture);
		if (convertedPict.GetKind() != RWValue::eValue_PictureJPG && convertedPict.GetKind() != RWValue::eValue_PicturePNG)
			RW_ConvertPictureForPrinting (convertedPict, true);

		if (convertedPict.GetBlobSize() > 0)
			try
			{
				std::unique_ptr<PdfImage>	image = mPDF->CreateImage();
				image->LoadFromBuffer (bufferview (static_cast <const char*> (convertedPict.GetBlobData()), convertedPict.GetBlobSize()));
				pd->fWidth = float (image->GetWidth());
				pd->fHeight = float (image->GetHeight());
				pd->fImage = std::move (image);
			}
			catch (const PdfError &e)
			{
				LogPdfError ("GetPictBounds", e);
			}
	}

	if (inGrow && pd->fImage != nullptr)	// can shrink / expand
	{
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
			{
				double	scalingFactor = ioRect.Width() / pd->fWidth;
				if (scalingFactor > ioRect.Height() / pd->fHeight)
					scalingFactor = ioRect.Height() / pd->fHeight;
				if (scalingFactor > 1.0)		// don't enlarge a picture
					scalingFactor = 1.0;
				ioRect.bottom = ioRect.top + pd->fHeight * scalingFactor;
				ioRect.right = ioRect.left + pd->fWidth * scalingFactor;
				break;
			}
		}
	}
}


// ---------------------------------------------------------------------------
// DrawPict															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::DrawPict (SRect &inRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, double inRotation, float)
{
	if (!mPageIsOpen)
		return;

	{
		SRect	r (inRect);
		GetPictBounds (r, inPicture, inSizing, cd, false);
	}

	RWPdfPictData	*pd = static_cast <RWPdfPictData*> (*cd);
	if (pd == nullptr || pd->fImage == nullptr || pd->fWidth <= 0 || pd->fHeight <= 0)
		return;

	double	x, y;							// top left corner, report coordinates
	double	hscale = 1, vscale = 1;
	switch (inSizing)
	{
		case ePictFormat_Centered:				// Truncated (centered)
			x = inRect.left + (inRect.Width() - pd->fWidth) / 2;
			y = inRect.top + (inRect.Height() - pd->fHeight) / 2;
			break;

		case ePictFormat_ScaledToFit:			// Scaled to fit
			hscale = inRect.Width() / pd->fWidth;
			vscale = inRect.Height() / pd->fHeight;
			x = inRect.left;
			y = inRect.top;
			break;

		case ePictFormat_ScaledProp:			// Scaled to fit (proportional)
		case ePictFormat_ScaledPropCentered:	// Scaled to fit centered (prop.)
			hscale = inRect.Width() / pd->fWidth;
			if (hscale > inRect.Height() / pd->fHeight)
				hscale = inRect.Height() / pd->fHeight;
			if (hscale > 1.0)					// don't enlarge a picture
				hscale = 1.0;
			vscale = hscale;
			if (inSizing == ePictFormat_ScaledPropCentered)
			{
				x = inRect.left + (inRect.Width() - pd->fWidth * hscale) / 2;
				y = inRect.top + (inRect.Height() - pd->fHeight * vscale) / 2;
			}
			else
			{
				x = inRect.left;
				y = inRect.top;
			}
			break;

		case ePictFormat_Normal:				// Truncated (non-centered)
		default:
			x = inRect.left;
			y = inRect.top;
			break;
	}

	mPainter->Save();
	if (inRotation != 0)
	{
		CGAffineTransform	t = RWTools::MakeMatrixFromUserRect (GetPrintContext(), inRect, GetNativeRotation (inRotation), 0., 0.);
		ApplyTransform (t);
	}
	{
		StClipToRect	clip (this, inRect);
		// PDF images are placed by their bottom left corner
		mPainter->DrawImage (*pd->fImage, PdfX (x), PdfY (y + pd->fHeight * vscale), hscale, vscale);
	}
	mPainter->Restore();
}


// ---------------------------------------------------------------------------
// ApplyTransform													  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::ApplyTransform (CGAffineTransform &inMatrix)
{
	if (mPageIsOpen)
		mPainter->GraphicsState.ConcatenateTransformationMatrix (Matrix (inMatrix.a, inMatrix.b, inMatrix.c, inMatrix.d, inMatrix.tx, inMatrix.ty));
}


// ---------------------------------------------------------------------------
// MeasureWord														  [public]
// ---------------------------------------------------------------------------
// Width without the horizontal scale (RWTextFormatter applies it), like the
// native composers.

double
RWPoDoFoPageComposer::MeasureWord (const CText inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading)
{
	outAscent = outDescent = outLeading = 0;
	if (!EnsureDocument())
		return 0;

	const FontEntry		&entry = MapStyle (inStyle);
	PdfTextState		state;
	state.Font = entry.font;
	state.FontSize = inStyle->GetSize();

	const std::string	text = RWStr::ToUTF8 (RWStringView (inText).substr (0, size_t (std::max (inTextLength, 0))));
	double				width = 0;
	if (!entry.font->TryGetStringLength (text, state, width))
		printf ("RWPoDoFoPageComposer::MeasureWord: characters missing in font '%s'\n", RWStr::ToUTF8 (inStyle->GetFName()).c_str());

	if (entry.ascent > 0)		// same line metrics as CoreText ('hhea')
	{
		outAscent = entry.ascent * state.FontSize;
		outDescent = entry.descent * state.FontSize;
		outLeading = entry.lineGap * state.FontSize;
	}
	else
	{
		outAscent = entry.font->GetAscent (state);
		outDescent = -entry.font->GetDescent (state);	// PDF descent is negative
		outLeading = std::max (0.0, entry.font->GetLineSpacing (state) - outAscent - outDescent);
	}
	return width;
}


// ---------------------------------------------------------------------------
// DrawWord															  [public]
// ---------------------------------------------------------------------------

void
RWPoDoFoPageComposer::DrawWord (const CText inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle)
{
	if (!mPageIsOpen)
		return;

	const FontEntry		&entry = MapStyle (inStyle);
	const double		size = inStyle->GetSize();
	const SRGBColor		color = inStyle->GetTextColor();
	const double		x = PdfX (inX), y = PdfY (inBaseLine);

	PdfDrawTextStyle	decoration = PdfDrawTextStyle::Regular;
	if (inStyle->GetStyle() & RWStyle::st_underline)
		decoration |= PdfDrawTextStyle::Underline;
	if (inStyle->GetStyle() & RWStyle::st_strikethrough)
		decoration |= PdfDrawTextStyle::StrikeThrough;

	mPainter->Save();
	try
	{
		SetFillColor (color);
		mPainter->TextState.SetFont (*entry.font, size);
		const float	hScale = inStyle->GetHorizontalScale();
		if (hScale > 0 && hScale != 1)
			mPainter->TextState.SetFontScale (hScale);
		if (entry.syntheticBold)
		{
			SetStrokeColor (color);
			mPainter->GraphicsState.SetLineWidth (size * kSyntheticBoldStroke);
			mPainter->TextState.SetRenderingMode (PdfTextRenderingMode::FillStroke);
		}
		if (entry.syntheticItalic)		// skew around the baseline
			mPainter->GraphicsState.ConcatenateTransformationMatrix (Matrix (1, 0, kSyntheticItalicSkew, 1, -kSyntheticItalicSkew * y, 0));

		const std::string	text = RWStr::ToUTF8 (RWStringView (inText).substr (0, size_t (std::max (inTextLength, 0))));
		mPainter->DrawText (text, x, y, decoration);
	}
	catch (const PdfError &e)
	{
		LogPdfError ("DrawWord", e);
	}
	mPainter->Restore();
}
