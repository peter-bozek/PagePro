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
								RWWinPageComposer (unsigned long inFlags, UString &inDst, UString &inPrinter);	//mbs 25072011	printer
	virtual						~RWWinPageComposer (void);

	virtual		void			ParseReport (const TiXmlElement *inReport);
	virtual		void*			FinishReport (size_t &outSize);

	virtual		void			GetPageBounds (ConstCText inOrientation, ConstCText inSize, SRect &outRect);
	virtual		bool			GetPageMetrics (float &outPageWidth, float &outPageHeight, SRect &outMargins);
	virtual		bool			GetPageMetrics (SRect &outPageRect, SRect &outPaperRect, SRect &outMargins);
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

	virtual		void			DrawTextBox (ConstCText inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText);
	virtual		double			MeasureText (ConstCText inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText);

	virtual		bool			GetDPI (float &x, float &y, void* inWindow);

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
	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect);
	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect, const SRect &inExcludeRect);
	virtual		void			RestoreClip (RWClipInfoRef &ioClipInfo);
	virtual		void			StyleChanged (RWStyle *inStyle);
	virtual		void	*		GetContext (void) const;
	virtual		void			SetContext (void *inContext);
	virtual		RWNativePageComposer*	CreateComposerForPrinting (void) const;

	virtual		void			SaveContext (RWContextInfoRef &outContext);
	virtual		void			RestoreContext (RWContextInfoRef &ioContext);
//virtual	const RWPrintContextRef	GetPrintContext (void);
	virtual		void			ApplyTransform (CGAffineTransform &inMatrix);
	virtual		double			MeasureWord (ConstCText inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading);
	virtual		void			DrawWord (ConstCText inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle);
	virtual		double			GetNativeRotation (double inRotation)	{ return -inRotation; }

				void			SetHWNDContext (HWND inHWND);
			Gdiplus::Graphics*	GetGDI (void) const;

protected:
# if	_4D_Package_
				void			Adopt4DSetting (void);
# endif
				long			AdoptDefSetting (bool inOrientation);

	virtual		OSStatus		OpenSession (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation);
	virtual		void			CloseSession (bool inRelease);
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

//	RWTextValue				mPrinterName;
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
