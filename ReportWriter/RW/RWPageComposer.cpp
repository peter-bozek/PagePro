# include	"RWPageComposer.h"
# include	"RWPoDoFoPageComposer.h"
#if	WINVER
# include	<Windows.h>
# include	<WinSpool.h>
#endif

bool	RWPageComposer::sUseTF = true;
long    RWPageComposer::mSessionCounter = 1024;

RWPictData::~RWPictData (void)
{
}


RWPrintText::RWPrintText (RWStyle *inStyle)
	:	mStyle (inStyle),
		mWidth (0),
		mLineHeight (0),
		mHeight (0),
		mPrintedHeight (0),
		mNumLines (-1),
		mPrintedLines (0)
{
}


RWPrintText::RWPrintText (const RWString inText, RWStyle *inStyle, bool inAttributed)
	:	mStyle (inStyle),
		mWidth (0),
		mLineHeight (0),
		mHeight (0),
		mPrintedHeight (0),
		mNumLines (-1),
		mPrintedLines (0)
{
    if (!inText.empty()) {
		if (inAttributed)
			mText = RWTools::SplitAttributedString (inText, NULL);
		else
			mText = inText;
    }
}


RWPrintText::~RWPrintText (void)
{
}


# pragma	mark	-

static struct	SRWPageSizes
{
	const char	*	name;
	float			width;
	float			height;
}	RWPageSizes [] =
{
	{ "A4",		595,	842		},
	{ "A0",		2380,	3368	},
	{ "A1",		1684,	2380	},
	{ "A2",		1190,	1684	},
	{ "A3",		842,	1190	},
//	{ "A4",		595,	842		},
	{ "A5",		421,	595		},
	{ "A6",		297,	421		},
	{ "B5",		501,	709		},
	{ "Letter",	612,	792		},
	{ "Legal",	612,	1008	},
	{ "Ledger",	1224,	792		},
	{ "p11x17",	792,	1224	},
	{ NULL,		0,		0		}
};


// ---------------------------------------------------------------------------
// RWPageComposer							Constructor			   [protected]
// ---------------------------------------------------------------------------

RWPageComposer::RWPageComposer (unsigned long inFlags, RWString &inDst, RWString &inPrinter)	//mbs 25072011	printer
	:	mFlags (inFlags),
		mDestination (inDst),
		mPrinterName (inPrinter),	//mbs 25072011	printer
		mPageWidth (0),
		mPageHeight (0),
		mReportPageMargins (0, 0, 0, 0),
		mPaperRect (0, 0, 0, 0),
		mPageRect (0, 0, 0, 0),
		mUseReportMargins (false),
		mUsePhysical (false),
		mFirstPage (1),
		mLastPage (~0),
		mBatchLevel (0),
		mPageIsOpen (false),
        mReportRotation(false),
        mReportMirror(false),
        mInternalID(mSessionCounter++)
{
# if	!_4D_Package_
	mFlags &= ~ (eUse4DPageSetup | eUse4DJobSetup);
# endif
    sSessions[mInternalID] = this;
}


// ---------------------------------------------------------------------------
// RWPageComposer							Constructor			   [protected]
// ---------------------------------------------------------------------------

RWPageComposer::RWPageComposer (unsigned long inFlags)
	:	mFlags (inFlags),
		mPageWidth (0),
		mPageHeight (0),
		mReportPageMargins (0, 0, 0, 0),
		mPaperRect (0, 0, 0, 0),
		mPageRect (0, 0, 0, 0),
		mUseReportMargins (false),
		mUsePhysical (false),
		mFirstPage (1),
		mLastPage (~0),
		mBatchLevel (0),
		mPageIsOpen (false),
        mReportRotation(false),
        mReportMirror(false),
        mInternalID(mSessionCounter++)
{
# if	!_4D_Package_
	mFlags &= ~ (eUse4DPageSetup | eUse4DJobSetup);
# endif
    sSessions[mInternalID] = this;

}


// ---------------------------------------------------------------------------
// RWPageComposer							Constructor			   [protected]
// ---------------------------------------------------------------------------

RWPageComposer::RWPageComposer (const RWPageComposer &inOriginal)
{
	*this = inOriginal;
}


// ---------------------------------------------------------------------------
// operator =													   [protected]
// ---------------------------------------------------------------------------

RWPageComposer&
RWPageComposer::operator = (const RWPageComposer &inOriginal)
{
	mFlags = inOriginal.mFlags;
	mDestination = inOriginal.mDestination;
	mPageSize = inOriginal.mPageSize;
	mPageOrientation = inOriginal.mPageOrientation;
	mPageWidth = inOriginal.mPageWidth;
	mPageHeight = inOriginal.mPageHeight;
	mReportPageMargins = inOriginal.mReportPageMargins;
	mPaperRect = inOriginal.mPaperRect;
	mPageRect = inOriginal.mPageRect;
	mUseReportMargins = inOriginal.mUseReportMargins;
	mUsePhysical  = inOriginal.mUsePhysical;
	mFirstPage = inOriginal.mFirstPage;
	mLastPage = inOriginal.mLastPage;
    mReportRotation = inOriginal.mReportRotation;
    mReportMirror = inOriginal.mReportMirror;
    mInternalID = mSessionCounter++;

    sSessions[mInternalID] = this;

	return *this;
}


// ---------------------------------------------------------------------------
// RWPageComposer							Destructor			 	  [public]
// ---------------------------------------------------------------------------

RWPageComposer::~RWPageComposer (void)
{
    sSessions.erase(mInternalID);
}


// ---------------------------------------------------------------------------
// ParseReport														  [public]
// ---------------------------------------------------------------------------
// Get default page size, page orientation and encoding
// Initialize PDF (title, creator, ...)

void
RWPageComposer::ParseReport (RWXmlNode inReport)
{
	if (mBatchLevel == 0)
	{
		RWString	value = inReport.Attr (u"Size");
		if (value.empty())		// was "!value.empty()", which ignored every report's page size
			value = RWStr::FromASCII (RWPageSizes [0].name);	// "A4";
		mPageSize = value;

		if (inReport.HasAttr (u"pageWidth"))
		{
			mPageWidth = (float) inReport.AttrDouble (u"pageWidth", 0);
			mPageHeight = (float) inReport.AttrDouble (u"pageHeight", 0);
			if (mPageWidth < 100 || mPageHeight < 100)
				mPageWidth = mPageHeight = 0;
			else
				mPageRect = SRect (0.0, 0.0, mPageHeight, mPageWidth);
		}

		// pB 1.3.32
		value = inReport.Attr (u"Orientation");
		if (!value.empty())
			mPageOrientation = value;
		else if (mPageWidth > mPageHeight)
			mPageOrientation = u"Landscape";
		else
			mPageOrientation = u"Portrait";

		value = inReport.Attr (u"pageMargins");
		if (!value.empty())
		{
			mReportPageMargins = value;
//			mUseReportMargins = (mReportPageMargins.top != 0 || mReportPageMargins.left != 0 || mReportPageMargins.bottom != 0 || mReportPageMargins.right != 0);
		}
		if (inReport.HasAttr (u"usePhysical"))
		{
			mUsePhysical = inReport.AttrBool (u"usePhysical");	// was a pointer assigned to bool, i.e. always true
			mUseReportMargins = (!mUsePhysical);
		}

		mReportRotation = inReport.HasAttr (u"rotation");
		mReportMirror = inReport.HasAttr (u"mirror");

		//mbs 18062010
		mPaperRect.SetRect (-mReportPageMargins.top, -mReportPageMargins.left, mPageHeight - mReportPageMargins.bottom, mPageWidth - mReportPageMargins.right);
		mPageRect.SetRect (0.0f, 0.0f, mPageHeight - mReportPageMargins.top - mReportPageMargins.bottom, mPageWidth - mReportPageMargins.left - mReportPageMargins.right);
	}
	
	return;
}


// ---------------------------------------------------------------------------
// GetPageBounds													  [public]
// ---------------------------------------------------------------------------
// Get page size according to orientation

void
RWPageComposer::GetPageBounds ( RWString inOrientation,  RWString inSize, SRect &outRect)
{
	if (mPageRect.Width() < 100)
	{
		if (inSize.empty())
			inSize = mPageSize;

		if (inOrientation.empty())
			inOrientation = mPageOrientation;

		SRWPageSizes	*p;
		for (p = RWPageSizes; p->name != NULL && !RWStr::Equals (inSize, p->name); p++)
			;
		if (p->name == NULL)
			p = RWPageSizes;

		mPageRect.top = 0;
		mPageRect.left = 0;

		if (RWStr::Equals (inOrientation, "Landscape"))
		{
			mPageWidth = mPageRect.bottom = p->width;
			mPageHeight = mPageRect.right = p->height;
		}
		else
		{
			mPageHeight = mPageRect.bottom = p->height;
			mPageWidth = mPageRect.right = p->width;
		}

		//mbs 21062010
		mPaperRect.SetRect (-mReportPageMargins.top, -mReportPageMargins.left, mPageHeight - mReportPageMargins.bottom, mPageWidth - mReportPageMargins.right);
		mPageRect.SetRect (0.0f, 0.0f, mPageHeight - mReportPageMargins.top - mReportPageMargins.bottom, mPageWidth - mReportPageMargins.left - mReportPageMargins.right);

		if (mUsePhysical) {
			outRect = mPaperRect;
		} else {
			if (not mUseReportMargins)	//mbs 06052010	page is smaller than physical paper ==> not & substitution
			{
				//mbs 20062010	left at 0,0
	//			outRect.top += mReportPageMargins.top;
	//			outRect.left += mReportPageMargins.left;
	//			outRect.bottom -= mReportPageMargins.bottom;
	//			outRect.right -= mReportPageMargins.right;
				outRect = mPageRect;
			}
			else
				outRect = mPaperRect - mPaperRect.TopLeft(); 
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// GetPageMetrics													  [public]
// ---------------------------------------------------------------------------
// Get page size and margins

bool
RWPageComposer::GetPageMetrics (float &outPageWidth, float &outPageHeight, SRect &outMargins)
{
	SRect	r;
	GetPageBounds (NULL, NULL, r);
	outPageWidth = r.Width();
	outPageHeight = r.Height();
	outMargins = mReportPageMargins;
	return true;
}


// ---------------------------------------------------------------------------
// GetPageMetrics													  [public]
// ---------------------------------------------------------------------------
// Get page size and margins

bool
RWPageComposer::GetPageMetrics (SRect &outPageRect, SRect &outPaperRect, SRect &outMargins)
{
	GetPageBounds (NULL, NULL, outPageRect);
	outPaperRect.SetRect (float (outPageRect.top - mReportPageMargins.top), outPageRect.left - mReportPageMargins.left,
						  outPageRect.bottom + mReportPageMargins.bottom, outPageRect.right + mReportPageMargins.right);
	outMargins = mReportPageMargins;
	return true;
}


// ---------------------------------------------------------------------------
// GetPaperMetrics													  [public]
// ---------------------------------------------------------------------------
// Get paper size and margins

bool
RWPageComposer::GetPaperMetrics (float &outPaperWidth, float &outPaperHeight, SRect &outMargins)
{
	SRect	r;
	GetPageBounds (NULL, NULL, r);
	outPaperWidth = r.Width() + mReportPageMargins.left + mReportPageMargins.right;
	outPaperHeight = r.Height() + mReportPageMargins.top + mReportPageMargins.bottom;
	outMargins = mReportPageMargins;
	return true;
}


// ---------------------------------------------------------------------------
// PrintSettings													  [public]
// ---------------------------------------------------------------------------

long
RWPageComposer::PrintSettings (float &outPaperWidth, float &outPaperHeight, SRect &outMargins)
{
	try
	{
		mFlags |= eResetMargins;
		long	result = OpenSession (true, true, 1, ~0, RWStr::Equals (mPageOrientation, "Landscape"));
		CloseSession (false);
		if (result == 0)	//mbs 12082011
		{
			GetPaperMetrics (outPaperWidth, outPaperHeight, outMargins);
			mFlags &= ~eResetMargins;
		}
		return result;
	}
	catch (...)
	{
		mFlags &= ~eResetMargins;
		throw;
	}
}


// ---------------------------------------------------------------------------
// DrawLine															  [public]
// ---------------------------------------------------------------------------

void
RWPageComposer::DrawLine (const SRect &inRect, float inThickness, SRGBColor inLineColor, UInt8 inFlags, float inLineLen, float inSpaceLen)
{
	switch (inFlags)
	{
		default:
		case RWLine_Horizontal:
			DrawLine (inRect.top, inRect.left, inRect.top, inRect.right, inThickness, inLineColor, inLineLen, inSpaceLen);
			break;
		case RWLine_Vertical:
			DrawLine (inRect.top, inRect.left, inRect.bottom, inRect.left, inThickness, inLineColor, inLineLen, inSpaceLen);
			break;
		case RWLine_TopLeft:
			DrawLine (inRect.top, inRect.left, inRect.bottom, inRect.right, inThickness, inLineColor, inLineLen, inSpaceLen);
			break;
		case RWLine_BottomLeft:
			DrawLine (inRect.bottom, inRect.left, inRect.top, inRect.right, inThickness, inLineColor, inLineLen, inSpaceLen);
			break;
		case RWLine_Full:
			DrawLine (inRect.top, inRect.left, inRect.bottom, inRect.right, inThickness, inLineColor, inLineLen, inSpaceLen);
			DrawLine (inRect.bottom, inRect.left, inRect.top, inRect.right, inThickness, inLineColor, inLineLen, inSpaceLen);
			break;
	}
/*
	if (inRect.bottom - inRect.top < inRect.right - inRect.left)	// horizontal
		DrawLine (inRect.top, inRect.left, inRect.top, inRect.right, inThickness, inLineColor, inLineLen, inSpaceLen);
	else
		DrawLine (inRect.top, inRect.left, inRect.bottom, inRect.left, inThickness, inLineColor, inLineLen, inSpaceLen);
*/

	return;
}


// ---------------------------------------------------------------------------
// DrawRect															[public]
// ---------------------------------------------------------------------------

void
RWPageComposer::DrawRect (const SRect &inParent, const SRect &inRect, float inThickness, SRGBColor inFrameColor,
						  bool inFill, SRGBColor inFillColor, long inRows, long inCols, UInt8 inFlags)
{
	SRect	sect (inParent);
	sect &= inRect;
	if (not sect.IsEmpty())
	{
		SRect	r (inRect);
		if (inFlags == RWRect_Full)
			DrawRect (r, inThickness, true, inFrameColor, inFill, inFillColor);
		else if (inFill)
			DrawRect (r, 0, false, cWhiteColor, inFill, inFillColor);
		
		if (inFlags != 0 && inFlags != RWRect_Full)
		{
			if (inFlags & RWRect_Top)
			{
				r.top = r.bottom = inRect.top;
				r.left = inRect.left;	// + inThickness;
				r.right = inRect.right;	// - inThickness;
//				if (sect & r)
					DrawLine (r, inThickness, inFrameColor, RWLine_Horizontal);
			}
			if (inFlags & RWRect_Left)
			{
				r.top = inRect.top;	// + inThickness;
				r.bottom = inRect.bottom;	// - inThickness;
				r.left = r.right = inRect.left;
//				if (sect & r)
					DrawLine (r, inThickness, inFrameColor, RWLine_Vertical);
			}
			if (inFlags & RWRect_Right)
			{
				r.top = inRect.top;	// + inThickness;
				r.bottom = inRect.bottom;	// - inThickness;
				r.left = r.right = inRect.right - inThickness;
//				if (sect & r)
					DrawLine (r, inThickness, inFrameColor, RWLine_Vertical);
			}
			if (inFlags & RWRect_Bottom)
			{
				r.top = r.bottom = inRect.bottom - inThickness;
				r.left = inRect.left;	// + inThickness;
				r.right = inRect.right;	// - inThickness;
//				if (sect & r)
					DrawLine (r, inThickness, inFrameColor, RWLine_Horizontal);
			}
		}
		
		if (inRows > 1 || inCols > 1)
		{
			double	i, delta;
			long	n;
			
			if (inCols > 1)
			{
				delta = (inRect.Width() - inThickness) / inCols;
				
				r.top = inRect.top;	// + inThickness;
				r.bottom = inRect.bottom;	// - inThickness;
				for (n = inCols - 1, i = inRect.left + delta; n > 0 && i < sect.right; i += delta, n--)
				{
					if (i >= sect.left)
					{
						r.left = r.right = i;
						DrawLine (r, inThickness, inFrameColor, RWLine_Vertical);
					}
				}
			}
			
			if (inRows > 1)
			{
				delta = (inRect.Height() - inThickness) / inRows;
				r.left = inRect.left;	// + inThickness;
				r.right = inRect.right;	// - inThickness;
				for (n = inRows - 1, i = inRect.top + delta; n > 0 && i < sect.bottom; i += delta, n--)
				{
					if (i >= sect.top)
					{
						r.top = r.bottom = i;
						DrawLine (r, inThickness, inFrameColor, RWLine_Horizontal);
					}
				}
			}
		}
	}
	return;
}


// ---------------------------------------------------------------------------
// CreateScreenComposer						[static]				  [public]
// ---------------------------------------------------------------------------

#if	WINVER
# include	"RWWinPageComposer.h"
#else
# if 0	// MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_5
#  include	"RWMacQDPageComposer.h"		// QuickDraw + ATSUI on CG
#  include	"RWMacCGPageComposer.h"		// CoreGraphics + ATSUI
static	int	sWhichComposer = 0;
# endif
#  include	"RWCTPageComposer.h"		// CoreGraphics + CoreText
#endif

RWNativePageComposer*
RWPageComposer::CreateScreenComposer (void)
{
	RWString	empty;
#if	WINVER
	return new RWWinPageComposer (eDestinationScreen, empty, empty);
#else
# if	MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_5
	if (sWhichComposer == 1)
		return new RWMacQDPageComposer (eDestinationScreen, empty, empty);
	if (sWhichComposer == 2 || (void*) CTFrameGetLineOrigins == 0)
		return new RWMacCGPageComposer (eDestinationScreen, empty, empty);
# endif
	return new RWCTPageComposer (eDestinationScreen, empty, empty);
#endif
}


// ---------------------------------------------------------------------------
// CreatePrinterComposer					[static]				  [public]
// ---------------------------------------------------------------------------

RWPageComposer*
RWPageComposer::CreatePrinterComposer (unsigned long inFlags, RWString &inDst, RWString &inPrinter)	//mbs 25072011	printer
{
	RWPageComposer	*obj = NULL;

	// PDF files: PoDoFo on both platforms, the same output on macOS and Windows
	if ((inFlags & eDestinationMask) == eDestinationPDF && !inDst.empty())
		return new RWPoDoFoPageComposer (inFlags & ~eDestinationScreen, inDst, inPrinter);

#if	WINVER
	// preview prints to "Microsoft Print to PDF" (was the XPS Document Writer);
	// without that printer the preview is a PoDoFo PDF
	if ((inFlags & eDestinationMask) == eDestinationPreview)
	{
		HANDLE	printer = NULL;
		::OpenPrinterW (const_cast <LPWSTR> (L"Microsoft Print to PDF"), &printer, NULL);
		if (printer != NULL)
			::ClosePrinter (printer);
		else if (!inDst.empty())
			return new RWPoDoFoPageComposer ((inFlags & ~(eDestinationMask | eDestinationScreen)) | eDestinationPDF, inDst, inPrinter);
	}
	obj = new RWWinPageComposer (inFlags & ~eDestinationScreen, inDst, inPrinter);	//mbs 25072011	printer
#else
	obj = (RWPageComposer *)new RWCTPageComposer (inFlags & ~eDestinationScreen, inDst, inPrinter);	//mbs 25072011	printer
#endif

	return obj;
}


/*
//mbs 08102010	unused
void
RWPageComposer::SetJobName (const UTF8Char *inName)
{
	if (mBatchLevel == 0 || mJobName.IsEmpty())	//mbs 08102010
{
	if (inName && *inName)
		mJobName = inName;
	else
		mJobName.Free();
}
}
*/


void
RWPageComposer::SetJobName (const RWString inName)
{
	if (mBatchLevel == 0 || mJobName.empty())	//mbs 08102010
	{
	mJobName = inName;
	}
}

/*
void
RWPageComposer::SetJobName (const RWString &inName)
{
	if (mBatchLevel == 0 || mJobName.IsEmpty())	//mbs 08102010
	{
		if (inName.StrLength() > 0)
			mJobName.Attach (inName.CopyU16Str());
		else
			mJobName.Free();
	}
}
*/ // duplicite definition

void
RWPageComposer::IncreaseBatchLevel (void)
{
	mBatchLevel++;
}


void
RWPageComposer::DecreaseBatchLevel (void)
{
	mBatchLevel--;
}




PSesList	RWPageComposer::sSessions;


// ---------------------------------------------------------------------------
// GetSessionObject											 [static] [public]
// ---------------------------------------------------------------------------

RWPageComposer *
RWPageComposer::GetSessionObject (long inObject)
{
	if (inObject == 0L)
		return NULL;

//	RWPageComposer				*obj = reinterpret_cast <RWPageComposer*> (inObject);
	PSesList::const_iterator	iter = sSessions.find (inObject);
	if (iter == sSessions.end())
		return NULL;
	return reinterpret_cast <RWPageComposer*>(iter->second);
}


// ---------------------------------------------------------------------------
// OpenSession												 [static] [public]
// ---------------------------------------------------------------------------

long
RWPageComposer::OpenSession (RWPageComposer* &outSession, unsigned long inFlags, RWString &inTemplate, RWString &inDst, RWString &inJobName, RWString &inPrinter)	//mbs 25072011	printer
{
	RWPageComposer	*session = RWPageComposer::CreatePrinterComposer (inFlags, inDst, inPrinter);	//mbs 25072011	printer
	long			result = -1;
	if (session)
	{
		//mbs 08102010	needs to parse report template BEFORE opening the session!!!
		if (inTemplate.length() > 0)
		{
			RWXmlDocument	xml;
			RWXmlResult		parsed = xml.LoadString (inTemplate);
			if (!parsed)
			{
				printf ("Could not load XML. Error='%s'.\n", RWStr::ToUTF8 (parsed.description).c_str());
				fflush (stdout);
				result = 1;	// errCantLoadXML;
			}
			else
				session->ParseReport (xml.Root());
		}

		//mbs 19012011	needs to set job name BEFORE opening the session!!!
		session->SetJobName (inJobName);

		result = session->OpenSession (true, true, 0, ~0, RWStr::Equals (session->mPageOrientation, "Landscape"));
		if (result != noErr)
		{
			session->CloseSession (true);
			delete session;
			session = NULL;
		}
		else
            sSessions[session->mInternalID] = session;
	}
	outSession = session;
	return result;
}


// ---------------------------------------------------------------------------
// CloseSession												 [static] [public]
// ---------------------------------------------------------------------------

void
RWPageComposer::CloseSession (RWPageComposer *inSession)
{
	if (inSession)
	{
		inSession->CloseSession (true);
		PSesList::iterator	iter = sSessions.find (inSession->mInternalID);
		if (iter != sSessions.end())
			sSessions.erase (inSession->mInternalID);
		delete inSession;
	}
}



bool	RWPageComposer::GetDPI (float &x, float &y, void* inWindow)
{
	return false;
}



# include	"RWTextFormatter.h"

void
RWPageComposer::DrawTextBox (RWString inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText)
{
	SRect	r (inRect);
	if (ioPrintText != NULL && *ioPrintText != NULL)
		inText = (*ioPrintText)->GetText();
	
	if (mPageIsOpen)
	{
#if	MACVER
		// only the Quartz composers have a graphics context (not RWPoDoFoPageComposer)
		RWMacPageComposer	*macComposer = dynamic_cast <RWMacPageComposer*> (this);
		if (macComposer)
			macComposer->GetGContext();
#endif
		if (inStyle->GetBackColor().alpha != 0)	// (inStyle->GetBackColor() != cWhiteColor)
			DrawRect (inRect, 0, false, cWhiteColor, true, inStyle->GetBackColor());
		try
		{
			if (!inText.empty())
			{
				if (ioPrintText == NULL)	// RWTable support
				{
					RWTFPrintText	txt (inStyle);
					txt.Init (*this, inText, r, inWrap, inAttributed, inFit);
					r = inRect;
					txt.Draw (*this, r, inFit, true);
				}
				else
				{
					if (*ioPrintText == NULL)
					{
						*ioPrintText = new RWTFPrintText (inStyle);
						static_cast <RWTFPrintText*> (*ioPrintText)->Init (*this, inText, r, inWrap, inAttributed, inFit);
						r = inRect;
					}
					static_cast <RWTFPrintText*> (*ioPrintText)->Draw (*this, r, inFit, true);
				}
			}
#if	MACVER
			if (macComposer)
				macComposer->ReleaseGContext();
#endif
		}
		catch (...)
		{
#if	MACVER
			if (macComposer)
				macComposer->ReleaseGContext();
#endif
			throw;
		}
	}
	else if (ioPrintText != NULL && *ioPrintText != NULL)	// nothing to do if we don't draw and ioPrintText is empty
	{
		if (!inText.empty())
			static_cast <RWTFPrintText*> (*ioPrintText)->Draw (*this, r, inFit, true);
	}
	
	return;
}


double
RWPageComposer::MeasureText (const RWString inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText)
{
    if (!inText.empty())
	{
		if (ioPrintText == NULL)	// RWTable support
		{
			RWTFPrintText	txt (inStyle);
			txt.Init (*this, inText, ioRect, inWrap, inAttributed, inFit);
//			txt.Draw (*this, ioRect, inFit, false, NULL);
		}
		else
		{
			if (*ioPrintText == NULL)
			{
				*ioPrintText = new RWTFPrintText (inStyle);
				static_cast <RWTFPrintText*> (*ioPrintText)->Init (*this, inText, ioRect, inWrap, inAttributed, inFit);
			}
			else
				static_cast <RWTFPrintText*> (*ioPrintText)->Draw (*this, ioRect, inFit, false);
		}
	}
	else
	{
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
	}

	return ioRect.Width();
}

// ---------------------------------------------------------------------------
// InitPagePosition													 [public]
// ---------------------------------------------------------------------------

void
RWPageComposer::InitPagePosition (void)
{
    if (mReportRotation) {
        CGAffineTransform	t = CGAffineTransformMake (-1, 0, 0, -1, mPageRect.Width(), mPageRect.Height());
        ApplyTransform(t);
        
    }
    if (mReportMirror) {
        CGAffineTransform	t = CGAffineTransformMake (-1, 0, 0, 1, mPageRect.Width(), 0);
        ApplyTransform(t);
    }
    
}

