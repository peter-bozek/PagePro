# include	"RWMacCGPageComposer.h"
# include	"RWStyle.h"


// Composer specific object for picture rendering
struct	RWMacCGPictData	:	public	RWPictData
{
public:
	inline					RWMacCGPictData (void);
							~RWMacCGPictData (void);

		enum	{
			eKind_None,
			eKind_PICT,
			eKind_CGI,
			eKind_PDF
		}						fKind;
		union
		{
			RWScreenPict		fCGIRef;
			RWPrintPict			fPDFRef;
            // pB			QDPictRef			fQDPRef;
		};
		CGRect					fRect;
		RWValue					fConvertedPict;
};


inline	RWMacCGPictData::RWMacCGPictData (void)
	:	RWPictData(),
		fKind (RWMacCGPictData::eKind_None)
// pB		fQDPRef (0)
{
	fRect = CGRectMake (0, 0, 0, 0);
}

RWMacCGPictData::~RWMacCGPictData (void)
{
	switch (fKind)
	{
//		case RWMacCGPictData::eKind_PICT:
//			QDPictRelease (fQDPRef);
//			break;
		case RWMacCGPictData::eKind_CGI:
			CFRelease (fCGIRef);
			break;
		case RWMacCGPictData::eKind_PDF:
			CFRelease (fPDFRef);
			break;
        case RWMacCGPictData::eKind_None:	// to shut up compiler
            break;
        case RWMacCGPictData::eKind_PICT:	// to shut up compiler
            break;
	}
}


#define	kGenericRGBProfilePathStr       "/System/Library/ColorSync/Profiles/Generic RGB Profile.icc"
/*
    This function locates, opens, and returns the profile reference for the calibrated 
    Generic RGB color space. It is up to the caller to call CMCloseProfile when done
    with the profile reference this function returns.
*/
static	CMProfileRef OpenGenericProfile(void)
{
    static CMProfileRef cachedRGBProfileRef = NULL;

//    // we only create the profile reference once
//    if (cachedRGBProfileRef == NULL)
//    {
//		OSStatus 			err;
//		CMProfileLocation 	loc;
//
//		loc.locType = cmPathBasedProfile;
//		strcpy(loc.u.pathLoc.path, kGenericRGBProfilePathStr);
//
//		err = CMOpenProfile(&cachedRGBProfileRef, &loc);
//
//		if (err != noErr)
//		{
//			cachedRGBProfileRef = NULL;
//			// log a message to the console
//			fprintf(stderr, "couldn't open generic profile due to error %d\n", (int)err);
//		}
//    }
//
//    if (cachedRGBProfileRef)
//    {
//		// clone the profile reference so that the caller has their own reference, not our cached one
//		CMCloneProfileRef(cachedRGBProfileRef);   
//    }

    return cachedRGBProfileRef;
}


/*
    Return the generic RGB color space. This is a 'get' function and the caller should
    not release the returned value unless the caller retains it first. Usually callers
    of this routine will immediately use the returned colorspace with CoreGraphics
    so they typically do not need to retain it themselves.

    This function creates the generic RGB color space once and hangs onto it so it can
    return it whenever this function is called.
*/
static	CGColorSpaceRef GetGenericRGBColorSpace (void)
{
    static CGColorSpaceRef genericRGBColorSpace = NULL;

//	if (genericRGBColorSpace == NULL)
//	{
//		CMProfileRef genericRGBProfile = OpenGenericProfile();
//
//		if (genericRGBProfile)
//		{
//			genericRGBColorSpace = CGColorSpaceCreateWithPlatformColorSpace(genericRGBProfile);
//			if (genericRGBColorSpace == NULL)
//				fprintf(stderr, "couldn't create the generic RGB color space\n");
//
//			// we opened the profile so it is up to us to close it
//			CMCloseProfile(genericRGBProfile); 
//		}
//	}
    return genericRGBColorSpace;
}


// ---------------------------------------------------------------------------
// RWMacCGPageComposer						Constructor				  [public]
// ---------------------------------------------------------------------------

RWMacCGPageComposer::RWMacCGPageComposer (unsigned long inFlags, CText &inDst, CText &inPrinter)	//mbs 25072011	printer
	:	RWMacPageComposer (inFlags, inDst, inPrinter),	//mbs 25072011	printer
		mGC (0)
{
	return;
}


// ---------------------------------------------------------------------------
// RWMacCGPageComposer						Constructor			   [protected]
// ---------------------------------------------------------------------------

RWMacCGPageComposer::RWMacCGPageComposer (const RWMacCGPageComposer &inOriginal)
	:	RWMacPageComposer (inOriginal),
		mGC (0)
{
}


// ---------------------------------------------------------------------------
// operator =													   [protected]
// ---------------------------------------------------------------------------

RWMacCGPageComposer&
RWMacCGPageComposer::operator = (const RWMacCGPageComposer &inOriginal)
{
	RWMacPageComposer::operator = (inOriginal);
	mGC = NULL;
	return *this;
}


// ---------------------------------------------------------------------------
// ~RWMacCGPageComposer						Destructor				  [public]
// ---------------------------------------------------------------------------

RWMacCGPageComposer::~RWMacCGPageComposer (void)
{
	if ((mFlags & eDestinationMask) != eDestinationScreen)
		CloseSession (true);	// parent is unable to call our virtual ClosePageSelf()
	return;
}


// ---------------------------------------------------------------------------
// GetPageBounds													  [public]
// ---------------------------------------------------------------------------
// Get default page size, page orientation and encoding

void
RWMacCGPageComposer::GetPageBounds (const CText inOrientation, const CText inSize, SRect &outRect)
{
	RWMacPageComposer::GetPageBounds (inOrientation, inSize, outRect);

	if (mUsePhysical) {
		outRect = mPageRect; // pB is a paper rect at this moment
		mOffsetX = 0; //- mReportPageMargins.left;
		mOffsetY = 0; //- mReportPageMargins.bottom;
	} else {
		if (mTruePageRect.Width() != 0)
		{
			//mbs 24122009	always set the transformation!
			// adjust paper to page rect
			if (mUseReportMargins)
			{
				mOffsetX = 0;	// -mReportPageMargins.left;
				mOffsetY = 0;	// -mReportPageMargins.bottom;
				mPageRect.bottom = mTruePageRect.top + mPaperRect.Height();
				mPageRect.right = mTruePageRect.left + mPaperRect.Width();
			}
			else
			{
				//mbs 12082010	use report margins, not printer's
				//			mOffsetX = mTruePageRect.left - mPaperRect.left;
				//			mOffsetY = mPaperRect.bottom - mTruePageRect.Height();
				mOffsetX = mReportPageMargins.left;
				mOffsetY = mReportPageMargins.bottom;
				mPageRect.bottom = mPaperRect.Height() - mReportPageMargins.bottom - mReportPageMargins.top;
				mPageRect.right = mPaperRect.Width() - mReportPageMargins.right - mReportPageMargins.left;
			}
			outRect = mPageRect;
		}
		
	}

	return;
}


// ---------------------------------------------------------------------------
// OpenSessionSelf												   [protected]
// ---------------------------------------------------------------------------

OSStatus
RWMacCGPageComposer::OpenSessionSelf (void)
{
	//see Apple's Technical Q&A: QA1216

	OSStatus			status = noErr;
// pB	CFStringRef         strings[1];
//    CFArrayRef          ourGraphicsContextsArray;

//	strings[0] = kPMGraphicsContextCoreGraphics; // This is important!
//	ourGraphicsContextsArray = CFArrayCreate (kCFAllocatorDefault,
//									(const void **)strings,
//									1, &kCFTypeArrayCallBacks);
//	if (ourGraphicsContextsArray != NULL)
//	{
//		status = PMSessionSetDocumentFormatGeneration (mPrintSession,
//									kPMDocumentFormatPDF,
//									ourGraphicsContextsArray, NULL);
//		CFRelease (ourGraphicsContextsArray);
//	}

	if (status == noErr)
	{
		PMDestinationType	dstType = kPMDestinationInvalid;
		CFURLRef			dstURL = NULL;
		if (mFlags & (eRanJobSetup | eNoDefPrinter))	//mbs 07042010	honor user's interactive settings	//mbs 25072011	honor printer stored in report
		{
			PMSessionGetDestinationType (mPrintSession, mPrintSettings, &dstType);
			CFStringRef		format = NULL;
			PMSessionCopyDestinationFormat (mPrintSession, mPrintSettings, &format);
			PMSessionCopyDestinationLocation (mPrintSession, mPrintSettings, &dstURL);
			PMSessionSetDestination (mPrintSession, mPrintSettings, dstType, format, dstURL);
			if (format)
				::CFRelease (format);
		}
		else
		{
			dstType = GetDestinationType();
			dstURL = GetDestinationURL();
// CFShow (dstURL);
			status = PMSessionSetDestination (mPrintSession, mPrintSettings, dstType, dstType == kPMDestinationFile? kPMDocumentFormatPDF: NULL, dstURL);
		}
		if (dstURL)
			::CFRelease (dstURL);
	}

    return status;
}


// ---------------------------------------------------------------------------
// OpenNewPageSelf												   [protected]
// ---------------------------------------------------------------------------

void
RWMacCGPageComposer::OpenNewPageSelf (void)
{
	OSStatus	status = PMSessionGetCGGraphicsContext (
								mPrintSession,
								(CGContextRef*) &mGC );
	if (status != 0 || mGC == nil)
		DebugStr ("\pRWMacCGPageComposer::OpenNewPageSelf: mGC is nil!");
	if (status != noErr || mGC == nil)
	{
		printf ("RWMacCGPageComposer::OpenNewPageSelf: status != noErr || mGC == nil\n");
		throw status;
	}

	//mbs 24042006	we learned something...
	CGColorSpaceRef genericColorSpace = GetGenericRGBColorSpace();
//	CGRect	pageBounds = CGRectMake (mPageRect.left, mPageRect.bottom, mPageRect.Width(), mPageRect.Height());
//	CGContextBeginPage (mGC, &pageBounds);
	// ensure that we are drawing in the correct color space, a calibrated color space
	CGContextSetFillColorSpace (mGC, genericColorSpace); 
	CGContextSetStrokeColorSpace (mGC, genericColorSpace); 

#if	TARGET_DEBUG && __MACH__
	// true page rect
	CGRect	rect = CGRectMake (mTruePageRect.left - mPaperRect.left, mPaperRect.bottom - mTruePageRect.bottom, mTruePageRect.Width(), mTruePageRect.Height());
	DrawNativeRect (rect, .25, cRedColor, .75, .75);

//	if (mUseReportMargins)
	{
#if 0
		// original design page rect (using width/height)
		rect = CGRectMake (
			mReportPageMargins.left,
			mReportPageMargins.bottom + mPaperRect.Height() - mPageHeight,
			mPageWidth - mReportPageMargins.left - mReportPageMargins.right,
			mPageHeight - mReportPageMargins.top - mReportPageMargins.bottom);
		DrawNativeRect (rect, 0.25, cGreenColor, 2, 1);
#endif
		// original design page rect (using margins)
		rect = CGRectMake (
			mReportPageMargins.left,
			mReportPageMargins.bottom,
			mPaperRect.Width() - mReportPageMargins.left - mReportPageMargins.right,
			mPaperRect.Height() - mReportPageMargins.top - mReportPageMargins.bottom);
		DrawNativeRect (rect, 0.25, cBlueColor, 1, 1);
	}
#endif

	CGContextTranslateCTM (mGC, mOffsetX, mOffsetY);

	return;
}


// ---------------------------------------------------------------------------
// ClosePageSelf												   [protected]
// ---------------------------------------------------------------------------

void
RWMacCGPageComposer::ClosePageSelf (bool inFinally)
{
	if (inFinally)
		mGC = nil;
//	else
//		CGContextEndPage (mGC);

	return;
}


// ---------------------------------------------------------------------------
// DrawTextBoxBegin												   [protected]
// ---------------------------------------------------------------------------

CGContextRef
RWMacCGPageComposer::DrawTextBoxBegin (const RWStyle *inStyle, const SRect &inRect)
{
	if (inStyle->GetBackColor().alpha != 0)	// (inStyle->GetBackColor() != cWhiteColor)
		DrawRect (inRect, 0, false, cWhiteColor, true, inStyle->GetBackColor());

	return mGC;
}


// ---------------------------------------------------------------------------
// DrawTextBoxEnd												   [protected]
// ---------------------------------------------------------------------------

void
RWMacCGPageComposer::DrawTextBoxEnd (CGContextRef inRef)
{
	return;
}


// ---------------------------------------------------------------------------
// ClipToRect														  [public]
// ---------------------------------------------------------------------------

RWClipInfoRef
RWMacCGPageComposer::ClipToRect (const SRect &inRect)
{
	if (mGC)
	{
		CGContextSaveGState (mGC);
		CGRect	rect = CGRectMake (inRect.left, mPageRect.bottom - inRect.bottom, inRect.Width(), inRect.Height());
		CGContextClipToRect (mGC, rect);
		return (RWClipInfoRef) 1;
	}
	return (RWClipInfoRef) 0;
}


// ---------------------------------------------------------------------------
// ClipToRect														  [public]
// ---------------------------------------------------------------------------

RWClipInfoRef
RWMacCGPageComposer::ClipToRect (const SRect &inRect, const SRect &inExcludeRect)
{
	if (inExcludeRect.IsEmpty())
		return ClipToRect (inRect);
	CGContextSaveGState (mGC);
	CGRect	rect = CGRectMake (inRect.left, mPageRect.bottom - inRect.bottom, inRect.Width(), inRect.Height());
	CGContextClipToRect (mGC, rect);
	SRect	r (inRect);
	r &= inExcludeRect;
	CGRect	rs [4];
	rs [0] = CGRectMake (inRect.left, mPageRect.bottom - r.top, inRect.Width(), r.top - inRect.top);				// top over r
	rs [1] = CGRectMake (inRect.left, mPageRect.bottom - inRect.bottom, inRect.Width(), inRect.bottom - r.bottom);	// bottom under r
	rs [2] = CGRectMake (inRect.left, mPageRect.bottom - inRect.bottom, r.left - inRect.left, inRect.Height());		// left from r
	rs [3] = CGRectMake (r.right, mPageRect.bottom - inRect.bottom, inRect.right - r.right, inRect.Height());		// right from r
	CGContextClipToRects (mGC, rs, 4);
	return (RWClipInfoRef) 2;
}


// ---------------------------------------------------------------------------
// RestoreClip														  [public]
// ---------------------------------------------------------------------------

void
RWMacCGPageComposer::RestoreClip (RWClipInfoRef &ioClipInfo)
{
	if (ioClipInfo)
	{
		CGContextRestoreGState (mGC);
		ioClipInfo = 0;
	}
	return;
}


// ---------------------------------------------------------------------------
// DrawLine															  [public]
// ---------------------------------------------------------------------------

void
RWMacCGPageComposer::DrawLine (float top, float left, float bottom, float right, float inThickness, SRGBColor inLineColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
		float	half = inThickness / 2;
#if	__MACH__
		CGContextSaveGState (mGC);
		//	CGContextBeginPath (mGC);
		CGContextSetLineWidth (mGC, inThickness);	//  > 0 ? inThickness : 0.25);
        CGContextSetRGBStrokeColor (mGC, inLineColor.red / 65535., inLineColor.green / 65535., inLineColor.blue / 65535., inLineColor.alpha / 65535.);
		if (inLineLen > 0 && inSpaceLen > 0)
		{
			CGFloat	pattern [2];
			pattern[0] = inLineLen;
			pattern[1] = inSpaceLen;
			CGContextSetLineDash (mGC, 0, pattern, 2);
		}

#if 1	// compensate for thickness - QD draws differently than CG
		if (left == right)
		{
			left += half;
			CGContextMoveToPoint (mGC, left, mPageRect.bottom - top);
			CGContextAddLineToPoint (mGC, left, mPageRect.bottom - bottom);
		}
		else if (top == bottom)
		{
			top += half;
			CGContextMoveToPoint (mGC, left, mPageRect.bottom - top);
			CGContextAddLineToPoint (mGC, right, mPageRect.bottom - top);
		}
		else
		{
//			left += half;
//			top += half;
			CGContextMoveToPoint (mGC, left, mPageRect.bottom - top);
			CGContextAddLineToPoint (mGC, right, mPageRect.bottom - bottom);
		}
#else
		CGContextMoveToPoint (mGC, left, mPageRect.bottom - top);
		CGContextAddLineToPoint (mGC, right, mPageRect.bottom - bottom);
#endif
		
		CGContextStrokePath (mGC);
		CGContextRestoreGState (mGC);
#endif
	}
}


// ---------------------------------------------------------------------------
// DrawRect															  [public]
// ---------------------------------------------------------------------------

void
RWMacCGPageComposer::DrawRect (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
							   bool inFill, SRGBColor inFillColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
#if	__MACH__
		CGContextSaveGState (mGC);
		if (inLineLen > 0 && inSpaceLen > 0)
		{
			CGFloat	pattern [2];
			pattern[0] = inLineLen;
			pattern[1] = inSpaceLen;
			CGContextSetLineDash (mGC, 0, pattern, 2);
		}
		CGContextBeginPath (mGC);
		CGContextSetLineWidth (mGC, inThickness);	//  > 0 ? inThickness : 0.25);
		if (inFill)
			CGContextSetRGBFillColor (mGC, inFillColor.red / 65535., inFillColor.green / 65535., inFillColor.blue / 65535., inFillColor.alpha / 65535.);
//		if (inThickness > 0)
		CGContextSetRGBStrokeColor (mGC, inFrameColor.red / 65535., inFrameColor.green / 65535., inFrameColor.blue / 65535., inFrameColor.alpha / 65535.);
		CGRect	rect = CGRectMake (inRect.left + inThickness/2, mPageRect.bottom - inRect.bottom + inThickness/2, inRect.Width() - inThickness, inRect.Height() - inThickness);
		CGContextAddRect (mGC, rect);
		CGContextClosePath (mGC);
		if (inFill)
//			CGContextFillStrokePath (mGC);
//			if (inThickness > 0)
				CGContextDrawPath (mGC, kCGPathFillStroke);
//			else
//				CGContextFillPath (mGC);
		else
			CGContextStrokePath (mGC);
		CGContextRestoreGState (mGC);
#endif
	}
	
	return;
}


#if	TARGET_DEBUG && __MACH__
// ---------------------------------------------------------------------------
// DrawNativeRect												   [protected]
// ---------------------------------------------------------------------------

void
RWMacCGPageComposer::DrawNativeRect (const CGRect &inRect, float inThickness, SRGBColor inFrameColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
#if	__MACH__
		CGContextSaveGState (mGC);
		if (inLineLen > 0 && inSpaceLen > 0)
		{
			CGFloat	pattern [2];
			pattern[0] = inLineLen;
			pattern[1] = inSpaceLen;
			CGContextSetLineDash (mGC, 0, pattern, 2);
		}
		CGContextBeginPath (mGC);
		CGContextSetLineWidth (mGC, inThickness);	//  > 0 ? inThickness : 0.25);
		CGContextSetRGBStrokeColor (mGC, inFrameColor.red / 65535., inFrameColor.green / 65535., inFrameColor.blue / 65535., inFrameColor.alpha / 65535.);
		CGContextAddRect (mGC, inRect);
		CGContextClosePath (mGC);
		CGContextStrokePath (mGC);
		CGContextRestoreGState (mGC);
#endif
	}

	return;
}
#endif


// ---------------------------------------------------------------------------
// DrawOval															  [public]
// ---------------------------------------------------------------------------

void
RWMacCGPageComposer::DrawOval (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
								bool inFill, SRGBColor inFillColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
#if	__MACH__
// TODO	adjust for line thickness/deformation...
		const float TWOPI   = 2*pi;	// 6.283185307
		float   halfWidth   = 0.5 * inRect.Width();
		float   halfHeight  = 0.5 * inRect.Height();
		float   centerX, centerY, radius;
		float   scaleX, scaleY;

		if( (halfWidth > 0) && (halfHeight > 0) )
		{
			if (halfWidth < halfHeight)
			{
				radius = halfWidth;
				scaleX = 1.0;
				scaleY = halfHeight / halfWidth;
				centerX = inRect.left + halfWidth;
				centerY = (mPageRect.bottom - inRect.bottom + halfHeight) / scaleY;
			}
			else
			{
				radius = halfHeight;
				scaleX = halfWidth / halfHeight;
				scaleY = 1.0;
				centerX = (inRect.left + halfWidth) / scaleX;
				centerY = mPageRect.bottom - inRect.bottom + halfHeight;
			}

			CGContextSaveGState(mGC);
			if (inLineLen > 0 && inSpaceLen > 0)
			{
				CGFloat	pattern [2];
				pattern[0] = inLineLen;
				pattern[1] = inSpaceLen;
				CGContextSetLineDash (mGC, 0, pattern, 2);
			}
			CGContextSetLineWidth (mGC, inThickness);	//  > 0 ? inThickness : 0.25);
			if (inFill)
				CGContextSetRGBFillColor (mGC, inFillColor.red / 65535., inFillColor.green / 65535., inFillColor.blue / 65535., inFillColor.alpha / 65535.);
			CGContextSetRGBStrokeColor (mGC, inFrameColor.red / 65535., inFrameColor.green / 65535., inFrameColor.blue / 65535., inFrameColor.alpha / 65535.);

	//		CGContextTranslateCTM (mGC, inRect.left + inRect.Width() / 2, mPageRect.bottom - inRect.bottom + inRect.Height() / 2);
	//		CGContextScaleCTM (mGC, inRect.Width() / 2, inRect.Height() / 2);
	//		CGContextBeginPath (mGC);
	//		CGContextAddArc (mGC, 0, 0, 1, 0, 2*pi, true);

			CGContextScaleCTM (mGC, scaleX, scaleY);
			CGContextBeginPath (mGC);
			CGContextAddArc (mGC, centerX, centerY, radius, 0.0, TWOPI, false);
			CGContextClosePath (mGC);
			if (inFill)
	//			CGContextFillStrokePath (mGC);
				CGContextDrawPath (mGC, kCGPathFillStroke);
			else
				CGContextStrokePath (mGC);
			CGContextRestoreGState (mGC);
		}
#endif
	}

	return;
}


// ---------------------------------------------------------------------------
// GetPictBounds													  [public]
// ---------------------------------------------------------------------------

void
RWMacCGPageComposer::GetPictBounds (SRect &ioRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, bool inGrow)
{
	RWMacCGPictData	*pd = NULL;
	if (*cd == NULL)
	{
		pd = new RWMacCGPictData;
		*cd = pd;

		pd->fConvertedPict.Attach (inPicture);
		if (pd->fConvertedPict.GetKind() >= RWValue::eValue_BLOB && pd->fConvertedPict.GetKind() <= RWValue::eValue_PictureEMF)
		{
			RW_ConvertPictureForPrinting (pd->fConvertedPict, false);
		}

		switch (pd->fConvertedPict.GetKind())
		{
			case RWValue::eValue_PictRefScreen:
			{
				void *sp = pd->fConvertedPict.GetPictureRef();
				if (sp)
				{
					if (CFGetTypeID ((CFTypeRef) sp) == CGImageGetTypeID())
					{
						pd->fKind = RWMacCGPictData::eKind_CGI;
						pd->fCGIRef = reinterpret_cast <RWScreenPict> (sp);
						CFRetain (pd->fCGIRef);
						pd->fRect.size.width = CGImageGetWidth (pd->fCGIRef);
						pd->fRect.size.height = CGImageGetHeight (pd->fCGIRef);
					}
				}
				break;
			}
			case RWValue::eValue_PictRefPrint:
			{
				void *pp = pd->fConvertedPict.GetPictureRef();
				if (pp)
				{
					if (CFGetTypeID ((CFTypeRef) pp) == CGPDFDocumentGetTypeID())
					{
						pd->fKind = RWMacCGPictData::eKind_PDF;
						pd->fPDFRef = reinterpret_cast <RWPrintPict> (pp);
						CFRetain (pd->fPDFRef);
						CGPDFPageRef	pdfPage = CGPDFDocumentGetPage (pd->fPDFRef, 1);
						pd->fRect = CGPDFPageGetBoxRect (pdfPage, kCGPDFMediaBox);
					}
				}
				break;
			}

//			case RWValue::eValue_PicturePICT:
//				if (not pd->fConvertedPict.IsEmpty())
//				{
//					CGDataProviderRef	dp = CGDataProviderCreateWithData (NULL, pd->fConvertedPict.GetBlobData(), pd->fConvertedPict.GetBlobSize(), NULL);
//					if (dp)
//					{
//						pd->fKind = RWMacCGPictData::eKind_PICT;
//						pd->fQDPRef = QDPictCreateWithProvider (dp);
//						CGDataProviderRelease (dp);
//						if (pd->fQDPRef)
//							pd->fRect = QDPictGetBounds (pd->fQDPRef);
//						else
//							pd->fKind = RWMacCGPictData::eKind_None;
//					}
//				}
//				break;
				
			case RWValue::eValue_PicturePNG:
			{
				pd->fKind = RWMacCGPictData::eKind_CGI;
				CGDataProviderRef	provider = CGDataProviderCreateWithData (NULL, pd->fConvertedPict.GetBlobData(), pd->fConvertedPict.GetBlobSize(), NULL);
				pd->fCGIRef = CGImageCreateWithPNGDataProvider (provider, NULL, false, kCGRenderingIntentDefault);
				CFRelease (provider);
				pd->fRect.size.width = CGImageGetWidth (pd->fCGIRef);
				pd->fRect.size.height = CGImageGetHeight (pd->fCGIRef);
				break;
			}

			case RWValue::eValue_PictureJPG:
			{
				pd->fKind = RWMacCGPictData::eKind_CGI;
				CGDataProviderRef	provider = CGDataProviderCreateWithData (NULL, pd->fConvertedPict.GetBlobData(), pd->fConvertedPict.GetBlobSize(), NULL);
				pd->fCGIRef = CGImageCreateWithJPEGDataProvider (provider, NULL, false, kCGRenderingIntentDefault);
				CFRelease (provider);
				pd->fRect.size.width = CGImageGetWidth (pd->fCGIRef);
				pd->fRect.size.height = CGImageGetHeight (pd->fCGIRef);
				break;
			}
				
			case RWValue::eValue_PictureTIFF:
			{
				pd->fKind = RWMacCGPictData::eKind_CGI;
				CGDataProviderRef	provider = CGDataProviderCreateWithData (NULL, pd->fConvertedPict.GetBlobData(), pd->fConvertedPict.GetBlobSize(), NULL);
				CGImageSourceRef	source = CGImageSourceCreateWithDataProvider (provider, NULL);
				CFRelease (provider);
				pd->fCGIRef = CGImageSourceCreateImageAtIndex (source, 0, NULL);
				CFRelease (source);
				pd->fRect.size.width = CGImageGetWidth (pd->fCGIRef);
				pd->fRect.size.height = CGImageGetHeight (pd->fCGIRef);
				break;
			}
				
			case RWValue::eValue_PicturePDF:
			case RWValue::eValue_PictureEMF:
				//••• TODO•••	convert image format
//				CGDataProviderRef CGDataProviderCreateWithData (void *info, const void *data, size_t size, CGDataProviderReleaseDataCallback releaseData);
//				CGImageSourceRef CGImageSourceCreateWithDataProvider(CGDataProviderRef provider, CFDictionaryRef options);
//				CGImageRef CGImageCreateWithJPEGDataProvider(CGDataProviderRef source, const float decode[], bool shouldInterpolate, CGColorRenderingIntent intent);
				break;

			default:	// to shut up compiler
				break;
		}
	}
	else
		pd = static_cast <RWMacCGPictData*> (*cd);

	pd->fWidth = pd->fRect.size.width;
	pd->fHeight = pd->fRect.size.height;

	if (inGrow)	// can shrink/expand
	{
//		if (pd->fRect.size.width > ioRect.Width())	// but can't expand horizontally
		{
			switch (inSizing)
			{
				case ePictFormat_Centered:				// Truncated (centered)
				case ePictFormat_Normal:				// Truncated (non-centered)
				default:
					ioRect.bottom = ioRect.top + pd->fRect.size.height;
					break;

				case ePictFormat_ScaledToFit:			// Scaled to fit
				case ePictFormat_ScaledProp:			// Scaled to fit (proportional)
				case ePictFormat_ScaledPropCentered:	// Scaled to fit centered (prop.)
					// scale
					float	scalingFactor;
#if	0	//mbs 06082010
					if ((pd->fRect.size.width - ioRect.Width()) > (pd->fRect.size.height - ioRect.Height()))
						scalingFactor = ioRect.Width() / pd->fRect.size.width;
					else
						scalingFactor = ioRect.Height() / pd->fRect.size.height;
#else
					scalingFactor = ioRect.Width() / pd->fRect.size.width;
					if (scalingFactor > ioRect.Height() / pd->fRect.size.height)
						scalingFactor = ioRect.Height() / pd->fRect.size.height;
#endif

					if (scalingFactor > 1.0)	// don't enlarge a pict
						scalingFactor = 1.0;

					ioRect.bottom = ioRect.top + pd->fRect.size.height * scalingFactor;
					ioRect.right = ioRect.left + pd->fRect.size.width * scalingFactor;
					break;
			} //switch
		}
/*
		else
		{
			ioRect.bottom = ioRect.top + pd->fRect.size.height;
			ioRect.right = ioRect.left + pd->fRect.size.width;
		}
*/
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawPict															  [public]
// ---------------------------------------------------------------------------

void
RWMacCGPageComposer::DrawPict (SRect &inRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, double inRotation, float alfa)
{
	if (mPageIsOpen && *cd == NULL)
	{
		SRect	r (inRect);
		GetPictBounds (r, inPicture, inSizing, cd, false);
	}

	if (mPageIsOpen && *cd)
	{
		RWMacCGPictData	*pd = static_cast <RWMacCGPictData*> (*cd);
		if (pd->fKind != RWMacCGPictData::eKind_None)
		{

#if	__MACH__
			CGRect		pictRect = pd->fRect;
			float		h, v;
			float		scalingFactor;
            SRect       scaleRect = inRect; //(pictRect.origin.y, pictRect.origin.x, pictRect.origin.y + pictRect.size.height, pictRect.origin.x + pictRect.size.width);
            StClipToRect	clip (this, inRect);

            if (inRotation != 0)
            {
                CGAffineTransform	t = RWTools::MakeMatrixFromUserRect (GetPrintContext(), scaleRect, GetNativeRotation (inRotation), 0., 0.);	//mbs 29062011
                ApplyTransform (t);
                
            }

			switch (inSizing)
			{
				case ePictFormat_Centered:				// Truncated (centered)
					h = (scaleRect.Width() - pd->fRect.size.width) / 2;
					v = (scaleRect.Height() - pd->fRect.size.height) / 2;
					pictRect.origin.x = scaleRect.left + h;
					pictRect.origin.y = scaleRect.top + v;
					break;

				case ePictFormat_ScaledToFit:			// Scaled to fit
					pictRect = CGRectMake (scaleRect.left, scaleRect.top, scaleRect.Width(), scaleRect.Height());
					break;

				case ePictFormat_ScaledProp:			// Scaled to fit (proportional)
				case ePictFormat_ScaledPropCentered:	// Scaled to fit centered (prop.)
					// scale
#if	0	//mbs 06082010
					if (pictRect.size.width - scaleRect.Width() > pictRect.size.height - scaleRect.Height())
						scalingFactor = scaleRect.Width() / pictRect.size.width;
					else
						scalingFactor = scaleRect.Height() / pictRect.size.height;
#else
					scalingFactor = scaleRect.Width() / pictRect.size.width;
					if (scalingFactor > scaleRect.Height() / pictRect.size.height)
						scalingFactor = scaleRect.Height() / pictRect.size.height;
#endif

					if (scalingFactor > 1.0)	// don't enlarge a pict
						scalingFactor = 1.0;

					pictRect.origin.x = scaleRect.left;
					pictRect.origin.y = scaleRect.top;
					pictRect.size.width *= scalingFactor;
					pictRect.size.height *= scalingFactor;

					if (inSizing == ePictFormat_ScaledPropCentered)
					{
						// center
						h = (scaleRect.Width() - pictRect.size.width) / 2;
						v = (scaleRect.Height() - pictRect.size.height) / 2;
						pictRect.origin.x += h;
						pictRect.origin.y += v;
					}
					break;

				case ePictFormat_Normal:				// Truncated (non-centered)
				default:
					pictRect.origin.x = scaleRect.left;
					pictRect.origin.y = scaleRect.top;
					break;
			} //switch

			CGContextSaveGState (mGC);
			CGRect	tRect = CGRectMake (scaleRect.left, mPageRect.bottom - scaleRect.bottom, scaleRect.Width(), scaleRect.Height());
			pictRect.origin.y = mPageRect.bottom - pictRect.origin.y - pictRect.size.height;
#if 0 && TARGET_DEBUG
//			CGContextSetRGBStrokeColor (mGC, 1.0, 0.0, 0.0, 0.5);
//			CGContextStrokeRect (mGC, tRect);
//			CGContextSetRGBStrokeColor (mGC, 0.0, 0.0, 1.0, 0.5);
//			CGContextStrokeRect (mGC, pictRect);
			DrawNativeRect (tRect, 0.5, cRedColor, .75, .75);
			DrawNativeRect (pictRect, 0.5, cBlueColor, 1.25, 1.25);
#endif
            CGContextSetAlpha(mGC, 1 - alfa);
        
			CGContextClipToRect (mGC, tRect);
			switch (pd->fKind)
			{
//				case RWMacCGPictData::eKind_PICT:
//					/* OSStatus	err = */ QDPictDrawToCGContext (mGC, pictRect, pd->fQDPRef);
//					break;
				case RWMacCGPictData::eKind_CGI:
					CGContextDrawImage (mGC, pictRect, pd->fCGIRef);
					break;
				case RWMacCGPictData::eKind_PDF:
				{
					CGPDFPageRef	pdfPage = CGPDFDocumentGetPage (pd->fPDFRef, 1);
					CGRect			sourceRect = CGPDFPageGetBoxRect (pdfPage, kCGPDFArtBox);
					if (sourceRect.size.height > 0 && sourceRect.size.width > 0)
					{
						CGAffineTransform m;
						m = CGAffineTransformMake(1, 0, 0, 1, 0, 0);
						m = CGAffineTransformTranslate(m, pictRect.origin.x - sourceRect.origin.x, pictRect.origin.y - sourceRect.origin.y);
						m = CGAffineTransformScale(m, pictRect.size.width / sourceRect.size.width, pictRect.size.height / sourceRect.size.height);

						// mComp = CGPDFPageGetDrawingTransform (pdfPage,  kCGPDFArtBox, pictRect, 0, false);
						CGContextConcatCTM (mGC, m);
						
						SRect bRect (pictRect.origin.y + pictRect.size.height, pictRect.origin.x, pictRect.origin.y, pictRect.origin.x + pictRect.size.width);
						CGAffineTransform	t = RWTools::MakeMatrixFromUserRect (GetPrintContext(), bRect, 0, pictRect.size.width, pictRect.size.height);
						CGContextConcatCTM (mGC, t);

						CGContextDrawPDFPage (mGC, pdfPage);
					}
					break;
				}
                case RWMacCGPictData::eKind_None:	// to shut up compiler
                    break;
                case RWMacCGPictData::eKind_PICT:	// to shut up compiler
                    break;
			}

			CGContextRestoreGState (mGC);
#endif
		}
	}

	return;
}


