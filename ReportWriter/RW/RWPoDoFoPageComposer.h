/*
 *  RWPoDoFoPageComposer.h
 *  ReportWriter
 *
 *  PDF output with PoDoFo 1.0, the same on macOS and Windows (4D desktop and
 *  4D Server). Fonts are the ones the native composers use for a style
 *  (RWPdfFonts), embedded as subsets; JPEG pictures are embedded unchanged,
 *  other pictures and all content streams are Flate compressed.
 */

#ifndef	_RWPoDoFoPageComposer_h_
# define	_RWPoDoFoPageComposer_h_

# include	"RWPageComposer.h"
# include	<map>
# include	<memory>
# include	<utility>

namespace	PoDoFo
{
	class	PdfMemDocument;
	class	PdfPage;
	class	PdfPainter;
	class	PdfFont;
}


class	RWPoDoFoPageComposer
	:	public	RWPageComposer
{
public:
								RWPoDoFoPageComposer (unsigned long inFlags, RWString &inDst, RWString &inPrinter);
	virtual						~RWPoDoFoPageComposer (void);

	virtual		void			ParseReport (RWXmlNode inReport) override;
	virtual		void*			FinishReport (size_t &outSize) override;

	virtual		void			GetPageBounds (const RWString inOrientation, const RWString inSize, SRect &outRect) override;
	virtual		void			OpenNewPage (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages) override;
	virtual		void			ClosePage (void) override;
	virtual		SRect			GetTruePageRect (void) const override;

	virtual		void			DrawLine (float top, float left, float bottom, float right, float inThickness, SRGBColor inLineColor, float inLineLen = 0, float inSpaceLen = 0) override;
	virtual		void			DrawRect (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
										  bool inFill, SRGBColor inFillColor, float inLineLen = 0, float inSpaceLen = 0) override;
	virtual		void			DrawOval (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
										  bool inFill, SRGBColor inFillColor, float inLineLen = 0, float inSpaceLen = 0) override;

	virtual		void			GetPictBounds (SRect &ioRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, bool inGrow) override;
	virtual		void			DrawPict (SRect &inRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, double inRotation, float alfa) override;

	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect) override;
	virtual		void			RestoreClip (RWClipInfoRef &ioClipInfo) override;
	virtual		void			StyleChanged (RWStyle *inStyle) override;

	virtual	const RWPrintContextRef	GetPrintContext (void) override				{ return reinterpret_cast <const RWPrintContextRef> (&mPageRect.bottom); }
	virtual		void			ApplyTransform (CGAffineTransform &inMatrix) override;
	virtual		double			MeasureWord (const RWString inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading) override;
	virtual		void			DrawWord (const RWString inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle) override;

				// the finished PDF of the last session (empty while a session is open)
				const std::string&	GetPDFData (void) const						{ return mPDFData; }

protected:
	virtual		OSStatus		OpenSession (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation) override;
	virtual		void			CloseSession (bool inRelease) override;

	struct	FontEntry
	{
		PoDoFo::PdfFont		*font = nullptr;
		bool				syntheticBold = false;
		bool				syntheticItalic = false;
		double				ascent = 0;			// em units, 0: ask PoDoFo (standard 14 font)
		double				descent = 0;
		double				lineGap = 0;
	};
				const FontEntry&	MapStyle (RWStyle *inStyle);
				bool			EnsureDocument (void);
				void			SetFillColor (SRGBColor inColor);
				void			SetStrokeColor (SRGBColor inColor);
				void			SetDash (float inLineLen, float inSpaceLen);
				double			PdfX (double inX) const			{ return inX + mOffset.h; }
				double			PdfY (double inY) const			{ return mPageRect.bottom - inY + mOffset.v; }

private:
								RWPoDoFoPageComposer (const RWPoDoFoPageComposer &inOriginal) = delete;
		RWPoDoFoPageComposer&	operator = (const RWPoDoFoPageComposer &inOriginal) = delete;

protected:
	typedef	std::pair<RWString, int>			FontKey;		// family, st_bold | st_italic

	SRect									mTruePageRect;
	SPoint									mOffset;
	std::unique_ptr<PoDoFo::PdfMemDocument>	mPDF;
	PoDoFo::PdfPage							*mPage;
	std::unique_ptr<PoDoFo::PdfPainter>		mPainter;
	std::map<FontKey, FontEntry>			mFonts;			// fonts of the open document
	std::string								mPDFData;
	unsigned long							mDocumentSerial;	// identifies the open document for cached pictures
	bool									mSaveFailed;
	static	unsigned long					sDocumentCounter;
};

#endif
