#ifndef	_RWWinPageComposer_h_
# define	_RWWinPageComposer_h_

# include	"RWPageComposer.h"
# include	<Windows.h>
# include	<commdlg.h>

# include	<map>

namespace Gdiplus
{
	class	Graphics;
	class	Font;
	class	Image;
	class	Metafile;
	class	Bitmap;
}
//mbs 29062011	RWStyle, not RWStyle*
//# define	RWStyleToFontMap	map <RWStyle*, Gdiplus::Font*>
# include	"RWStyle.h"
struct RWStyleLess
{
	bool operator()(const RWStyle& x, const RWStyle& y) const { return x.less (y); }
};
typedef	std::map <RWStyle, Gdiplus::Font*, RWStyleLess>	RWStyleToFontMap;


class	RWWinPageComposer
	:	public	RWPageComposer
{
friend class	RWWinPrintText;
friend class	RWPageComposer;

public:
								RWWinPageComposer (unsigned long inFlags, RWString &inDst, RWString &inPrinter);	//mbs 25072011	printer
	virtual						~RWWinPageComposer (void);

	virtual		void			ParseReport (RWXmlNode inReport) override;
	virtual		void*			FinishReport (size_t &outSize) override;

	virtual		void			GetPageBounds (const RWString inOrientation, const RWString inSize, SRect &outRect) override;
	virtual		bool			GetPageMetrics (float &outPageWidth, float &outPageHeight, SRect &outMargins) override;
	virtual		bool			GetPageMetrics (SRect &outPageRect, SRect &outPaperRect, SRect &outMargins) override;
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
//	virtual		void			FreePict (RWPictData **cd);
//	static		void			GetPictureFromRef (const RWPicture &inPicture, RWPicture &outPicture);
//	static		void			GetPictureRefFromPicture (const RWPicture &inPicture, RWPicture &outPicture);

	virtual		void			DrawTextBox (const RWString inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText) override;
	virtual		double			MeasureText (const RWString inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText) override;

	virtual		bool			GetDPI (float &x, float &y, void* inWindow) override;

		const	HGLOBAL			GetDevMode (void) const;
//				HGLOBAL			GetDevMode (bool inDetachSignature);
				void			SetDevMode (HGLOBAL inDevMode, bool inTakeOwnership = false);
				void			SetDevMode (const SBlob &inDevMode);
		const	HGLOBAL			GetDeviceNames (void) const;
//				HGLOBAL			GetDeviceNames (bool inDetachSignature);
				void			SetDeviceNames (HGLOBAL inDeviceNames, bool inTakeOwnership = false);
				void			SetDeviceNames (const SBlob &inDeviceNames);
				PAGESETUPDLGW*	GetPageSetupDialog (void) const;
				void			SetPageSetupDialog (const PAGESETUPDLGW *inPageSetupDlg);
				void			SetPageSetupDialog (const SBlob &inPageSetupDlg);
				PRINTDLGW	*	GetPrintDialog (void) const;
				void			SetPrintDialog (const PRINTDLGW *inPrintDlg);
				void			SetPrintDialog (const SBlob &inPrintDlg);

								// screen drawing
	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect) override;
	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect, const SRect &inExcludeRect) override;
	virtual		void			RestoreClip (RWClipInfoRef &ioClipInfo) override;
	virtual		void			StyleChanged (RWStyle *inStyle) override;
	virtual		void	*		GetContext (void) const override;
	virtual		void			SetContext (void *inContext) override;
	virtual		RWNativePageComposer*	CreateComposerForPrinting (void) const override;

	virtual		void			SaveContext (RWContextInfoRef &outContext) override;
	virtual		void			RestoreContext (RWContextInfoRef &ioContext) override;
//virtual	const RWPrintContextRef	GetPrintContext (void);
	virtual		void			ApplyTransform (CGAffineTransform &inMatrix) override;
	virtual		double			MeasureWord (const RWString inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading) override;
	virtual		void			DrawWord (const RWString inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle) override;
	virtual		double			GetNativeRotation (double inRotation) override	{ return -inRotation; }

				void			SetHWNDContext (HWND inHWND);
			Gdiplus::Graphics*	GetGDI (void) const;

protected:
# if	_4D_Package_
				void			Adopt4DSetting (void);
# endif
				long			AdoptDefSetting (bool inOrientation);

	virtual		OSStatus		OpenSession (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation) override;
	virtual		void			CloseSession (bool inRelease) override;
				Gdiplus::Font*	MapStyle (RWStyle *inStyle);
//	virtual		HDC				DrawTextBoxBegin (const RWStyle *inStyle, const SRect &inRect);
//	virtual		void			DrawTextBoxEnd (HDC inRef);

//private:
								RWWinPageComposer (const RWWinPageComposer &inOriginal);
			RWWinPageComposer&	operator = (const RWWinPageComposer &inOriginal);

protected:
	bool					mDocIsOpen;
	SRect					mTruePageRect;
	float					mOffsetX;
	float					mOffsetY;

//	RWString				mPrinterName;
	HGLOBAL					mDevMode;	// DEVMODEW
	HGLOBAL					mDevNames;	// DEVNAMES
	PAGESETUPDLGW		*	mPageSetupDlg;
	PRINTDLGW			*	mPrintDlg;

	HDC						mDC;
	HANDLE					mPrinter;
	Gdiplus::Graphics*		mGraphics;
	RWStyleToFontMap		mStyleMap;
};

inline	void	*			RWWinPageComposer::GetContext (void) const			{ return mDC; }
inline	Gdiplus::Graphics*	RWWinPageComposer::GetGDI (void) const				{ return mGraphics; }

#endif
