#ifndef	_RWMacCGPageComposer_h_
# define	_RWMacCGPageComposer_h_

# include	"RWMacPageComposer.h"

// Macintosh CoreGraphics specific drawing/printing
// Printing using kPMGraphicsContextCoreGraphics
// Line/Rect/Oval
// Pictures - PICT/CGImage/CGPDFDocument
// screen support

class	RWMacCGPageComposer
	:	public	RWMacPageComposer
{
public:
								RWMacCGPageComposer (unsigned long inFlags, CText &inDst, CText &inPrinter);	//mbs 25072011	printer
	virtual						~RWMacCGPageComposer (void);

	virtual		bool			IsQD (void) const;

	virtual		void			GetPageBounds (const CText inOrientation, const CText inSize, SRect &outRect);

	virtual		void			DrawLine (float top, float left, float bottom, float right, float inThickness, SRGBColor inLineColor, float inLineLen = 0, float inSpaceLen = 0);
	virtual		void			DrawRect (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
											bool inFill, SRGBColor inFillColor, float inLineLen = 0, float inSpaceLen = 0);
	virtual		void			DrawOval (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
											bool inFill, SRGBColor inFillColor, float inLineLen = 0, float inSpaceLen = 0);

	virtual		void			GetPictBounds (SRect &ioRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, bool inGrow);
	virtual		void			DrawPict (SRect &inRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, double inRotation, float alfa);
//	virtual		void			FreePict (RWPictData **cd);
//	static		void			GetPictureFromRef (const RWPicture &inPicture, RWPicture &outPicture);
//	static		void			GetPictureRefFromPicture (const RWPicture &inPicture, RWPicture &outPicture);
	
								// screen drawing
	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect);
	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect, const SRect &inExcludeRect);
	virtual		void			RestoreClip (RWClipInfoRef &ioClipInfo);
	virtual		void	*		GetContext (void) const;
	virtual		void			SetContext (void *inContext);
	virtual		RWNativePageComposer*	CreateComposerForPrinting (void) const;

	virtual		void			SaveContext (RWContextInfoRef &outContext)		{ if (mGC) CGContextSaveGState (mGC); }
	virtual		void			RestoreContext (RWContextInfoRef &ioContext)	{ if (mGC) CGContextRestoreGState (mGC); }
virtual	const RWPrintContextRef	GetPrintContext (void)							{ fPageBottom = mPageRect.bottom; return reinterpret_cast <const RWPrintContextRef> (&fPageBottom); }
	virtual		void			ApplyTransform (CGAffineTransform &inMatrix)	{ if (mGC) CGContextConcatCTM (mGC, inMatrix); }
    
	virtual		CGContextRef *	GetGContext (void);
	virtual		void			ReleaseGContext (void);
	
protected:
	virtual		OSStatus		OpenSessionSelf (void);
	virtual		void			OpenNewPageSelf (void);
	virtual		void			ClosePageSelf (bool inFinally);
	virtual		CGContextRef	DrawTextBoxBegin (const RWStyle *inStyle, const SRect &inRect);
	virtual		void			DrawTextBoxEnd (CGContextRef inRef);
#if	TARGET_DEBUG && __MACH__
				void			DrawNativeRect (const CGRect &inRect, float inThickness, SRGBColor inFrameColor, float inLineLen, float inSpaceLen);
#endif

//private:
								RWMacCGPageComposer (const RWMacCGPageComposer &inOriginal);
		RWMacCGPageComposer&	operator = (const RWMacCGPageComposer &inOriginal);

protected:
	float			fPageBottom;
	CGContextRef	mGC;
};

inline	bool			RWMacCGPageComposer::IsQD (void) const					{ return false; }
inline	void	*		RWMacCGPageComposer::GetContext (void) const			{ return mGC; }
inline	void			RWMacCGPageComposer::SetContext (void *inContext)		{ if ((mFlags & eDestinationMask) == eDestinationScreen) { mGC = reinterpret_cast <CGContextRef> (inContext); mDocIsOpen = mPageIsOpen = (mGC != NULL); }}
//inline	CGContextRef	RWMacCGPageComposer::GetGC (void) const					{ return mGC; }
//inline	void			RWMacCGPageComposer::SetGC (CGContextRef inGC)			{ SetContext (inGC); }
inline	CGContextRef *	RWMacCGPageComposer::GetGContext (void)					{ return &mGC; }
inline	void			RWMacCGPageComposer::ReleaseGContext (void)				{ }

#endif
