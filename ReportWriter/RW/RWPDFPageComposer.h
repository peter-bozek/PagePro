#ifndef	_RWPDFPageComposer_h_
# define	_RWPDFPageComposer_h_

# include	"RWPageComposer.h"
# include	"RWll.h"

typedef	struct	PDF_s	PDF;


class	RWPDFPageComposer
	:	public	RWPageComposer
{
public:
	virtual						~RWPDFPageComposer (void);

	virtual		void			GetPageBounds (const CText inOrientation, const CText inSize, SRect &outRect);
	virtual		void			OpenNewPage (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages);
	virtual		void			ClosePage (void);

	virtual		void			DrawLine (float top, float left, float bottom, float right, float inThickness, SRGBColor inLineColor, float inLineLen = 0, float inSpaceLen = 0);
	virtual		void			DrawRect (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
										  bool inFill, SRGBColor inFillColor, float inLineLen = 0, float inSpaceLen = 0);
	virtual		void			DrawOval (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
											bool inFill, SRGBColor inFillColor, float inLineLen = 0, float inSpaceLen = 0);

	virtual		void			GetPictBounds (SRect &ioRect, const RWPicture &inPicture, EPictFormat inSizing, void **cd, bool inGrow);
	virtual		void			DrawPict (const SRect &inRect, const RWPicture &inPicture, EPictFormat inSizing, void **cd, double inRotation);
	virtual		void			FreePict (void **cd);

	virtual		void			DrawTextBox (const CText inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, RWPrintText **ioPrintText);
	virtual		float			MeasureText (const CText inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, RWPrintText **ioPrintText);

protected:
								RWPDFPageComposer (void);
	virtual		void			ParseReport (RWXmlNode inReport) override;
	virtual		void*			FinishReport (size_t &outSize);

				int				FindFont (const char *inFontName);
static			void			PDFErrorHandler (PDF *inPDF, int inType, const char* inShortMsg);
static			size_t			RWPDFDummyWriteProc (PDF *p, void *data, size_t size);

private:
			// defensive programming - not implemented
								RWPDFPageComposer (const RWPDFPageComposer &inOriginal);
			RWPDFPageComposer&	operator = (const RWPDFPageComposer &inOriginal);

protected:
static const char	*PDFErrorNames[];
static const char	*PDFFonts[];
	const char		*mEncoding;
	PDF				*mPDF;
	PDF				*mAuxPDF;
	SRect			mPageRect;
};


class	RWPDFFilePageComposer
	:	public	RWPDFPageComposer
{
public:
								RWPDFFilePageComposer (const char *inFileName);
	virtual						~RWPDFFilePageComposer (void);

private:
			// defensive programming - not implemented
								RWPDFFilePageComposer (const RWPDFFilePageComposer &inOriginal);
		RWPDFFilePageComposer&	operator = (const RWPDFFilePageComposer &inOriginal);
};


class	RWPDFBlobPageComposer
	:	public	RWPDFPageComposer
{
public:
								RWPDFBlobPageComposer (void);
	virtual						~RWPDFBlobPageComposer (void);

protected:
	virtual		void		*	FinishReport (size_t &outSize);
	static		size_t			RWPDFBlobWriteProc (PDF *p, void *data, size_t size);

private:
			// defensive programming - not implemented
								RWPDFBlobPageComposer (const RWPDFBlobPageComposer &inOriginal);
		RWPDFBlobPageComposer&	operator = (const RWPDFBlobPageComposer &inOriginal);

protected:
	RWll		ll;
};

#endif
