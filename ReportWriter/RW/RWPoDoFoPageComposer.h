#ifndef	_RWPoDoFoPageComposer_h_
# define	_RWPoDoFoPageComposer_h_

# include	"RWPageComposer.h"

namespace	PoDoFo
{
	class	PdfStreamedDocument;
	class	PdfPage;
	class	PdfPainter;
	class	PdfFont;
}
# define	RWStyleToPdfFontMap	std::map <RWStyle*, PoDoFo::PdfFont*>


class	RWPoDoFoPageComposer
	:	public	RWPageComposer
{
public:
								RWPoDoFoPageComposer (unsigned long inFlags, CText &inDst, CText &inPrinter);	//mbs 25072011	printer
	virtual						~RWPoDoFoPageComposer (void);

	virtual		void			ParseReport (RWXmlNode inReport) override;
	virtual		void*			FinishReport (size_t &outSize);

	virtual		void			GetPageBounds (const CText inOrientation, const CText inSize, SRect &outRect);
//	virtual		bool			GetPageMetrics (float &outPageWidth, float &outPageHeight, SRect &outMargins);
//	virtual		bool			GetPageMetrics (SRect &outPageRect, SRect &outPaperRect, SRect &outMargins);
	virtual		void			OpenNewPage (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages);
	virtual		void			ClosePage (void);
	virtual		SRect			GetTruePageRect (void) const;

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

//	virtual		void			DrawTextBox (const CText inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText);
//	virtual		float			MeasureText (const CText inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText);

								// screen drawing
	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect);
//	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect, const SRect &inExcludeRect);
	virtual		void			RestoreClip (RWClipInfoRef &ioClipInfo);
    virtual     void            StyleChanged (RWStyle *inStyle);
//	virtual		void	*		GetContext (void) const;
//	virtual		void			SetContext (void *inContext);
//	virtual		RWNativePageComposer*	CreateComposerForPrinting (void) const;

//	virtual		void			SaveContext (RWContextInfoRef &outContext);
//	virtual		void			RestoreContext (RWContextInfoRef &ioContext);
virtual	const RWPrintContextRef	GetPrintContext (void)							{ return reinterpret_cast <const RWPrintContextRef> (&mPageRect.bottom); }
	virtual		void			ApplyTransform (CGAffineTransform &inMatrix);
	virtual		double			MeasureWord (const CText inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading);
	virtual		void			DrawWord (const CText inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle);

protected:
	virtual		OSStatus		OpenSession (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation);
	virtual		void			CloseSession (bool inRelease);
			PoDoFo::PdfFont*	MapStyle (RWStyle *inStyle);

private:
								RWPoDoFoPageComposer (const RWPoDoFoPageComposer &inOriginal);
		RWPoDoFoPageComposer&	operator = (const RWPoDoFoPageComposer &inOriginal);

protected:
	static const char	*PDFFonts[];

	SRect						mTruePageRect;
	SPoint						mOffset;
	PoDoFo::PdfStreamedDocument	*mPDF;
	PoDoFo::PdfPage				*mPage;
	PoDoFo::PdfPainter			*mPainter;
	RWStyleToPdfFontMap			mStyleMap;
};

#endif
