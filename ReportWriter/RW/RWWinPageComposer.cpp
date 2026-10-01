# include	"RWWinPageComposer.h"
# include	"RWStyle.h"
# include	<math.h>

# include	<Windows.h>
# include	<WinSpool.h>
# include	<OLE2.h>
# include	<io.h>
# include	<Shlobj.h>		//mbs 10052011	default name for preview
# include	<fcntl.h>
# include	<algorithm>
# include	<climits>

# include	<gdiplus.h>
using namespace Gdiplus;


// GDI+ colour from a 16 bit per channel RGBA colour
static	Color	ToColor (SRGBColor inColor)
{
	return Color (BYTE (inColor.alpha >> 8), BYTE (inColor.red >> 8), BYTE (inColor.green >> 8), BYTE (inColor.blue >> 8));
}


// DEVNAMES (wide) for a driver, device and output port
static	HGLOBAL	CreateDevNames (RWStringView inDriver, RWStringView inDevice, RWStringView inOutput, WORD inDefault = 0)
{
	const size_t	chars = inDriver.size() + inDevice.size() + inOutput.size() + 3;
	HGLOBAL			handle = GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof (DEVNAMES) + chars * sizeof (WCHAR));
	if (handle == NULL)
		return NULL;
	DEVNAMES		*names = (DEVNAMES*) GlobalLock (handle);
	WCHAR			*base = (WCHAR*) names;
	WORD			offset = WORD (sizeof (DEVNAMES) / sizeof (WCHAR));
	names->wDefault = inDefault;
	for (int i = 0; i < 3; i++)
	{
		RWStringView	text = i == 0 ? inDriver : i == 1 ? inDevice : inOutput;
		(i == 0 ? names->wDriverOffset : i == 1 ? names->wDeviceOffset : names->wOutputOffset) = offset;
		std::copy (text.begin(), text.end(), base + offset);
		base [offset + text.size()] = 0;
		offset = WORD (offset + text.size() + 1);
	}
	GlobalUnlock (handle);
	return handle;
}


// a string of a DEVNAMES structure, which 4D may hand over in ANSI or UTF-16
static	RWString	DevNamesString (const DEVNAMES *inNames, WORD inOffset, bool inWide)
{
	if (inWide)
		return RWStr::FromWide (((const WCHAR*) inNames) + inOffset);
	const char	*text = ((const char*) inNames) + inOffset;
	const int	length = MultiByteToWideChar (CP_ACP, 0, text, -1, NULL, 0);
	std::wstring	wide (length > 0 ? length - 1 : 0, L'\0');
	if (length > 1)
		MultiByteToWideChar (CP_ACP, 0, text, -1, &wide [0], length);
	return RWStr::FromWide (wide);
}


// Scaling factors for various unit conversions
static const double mm2inches = (1/25.4);
static const double inches2mm = (25.4);

static const double mm2twips = (1/25.4*1440);
static const double twips2mm = (1/(1/25.4*1440));

static const double mm2pt = (1/25.4*72);
static const double pt2mm = (1/(1/25.4*72));

static const double inches2pt = (72);
static const double pt2inches = (1/72.);

inline	LONG	PtToPrinter (DWORD Flags, float pt)
{
	if (Flags & PSD_INHUNDREDTHSOFMILLIMETERS)
		return pt * pt2mm * 100;
	return pt * pt2inches * 1000;	// PSD_INTHOUSANDTHSOFINCHES
}

inline	float	PrinterToPt (DWORD Flags, LONG pt)
{
	if (Flags & PSD_INHUNDREDTHSOFMILLIMETERS)
		return pt * mm2pt / 100;
	return pt * inches2pt / 1000;	// PSD_INTHOUSANDTHSOFINCHES
}


// Composer specific object for picture rendering
struct	RWWinPictData	:	public	RWPictData
{
public:
	inline					RWWinPictData (void);
							~RWWinPictData (void);

	Gdiplus::Image*			fImage;
	RWValue					fConvertedPict;
};


inline	RWWinPictData::RWWinPictData (void)
	:	RWPictData(),
		fImage (0)
{
}

RWWinPictData::~RWWinPictData (void)
{
/*	if (fImage)
		delete fImage;
 */
}


#if	0
//#include "DrawUnicodeString.h"
// #include "TextUtilities.h"

# include	"UString.h"
# define	STRING_ENCODING	kCFStringEncodingUnicode	// kTextEncodingUnicodeDefault + kTextEncodingDefaultFormat (aka kUnicode16BitFormat)

typedef	unsigned short	UniChar;
typedef	unsigned long	UniCharArrayOffset;
typedef	unsigned long	UniCharCount;
#endif


// Composer specific object for text rendering
class	RWWinPrintText	: public	RWPrintText
{
public:
								RWWinPrintText (RWWinPageComposer &inComposer, const RWString inText, RWStyle *inStyle, bool inWrap, bool inAttributed, bool inFit);
	virtual						~RWWinPrintText (void);
	virtual		void			Reset (void);

				void			GetBounds (RWWinPageComposer &inComposer, SRect &ioRect, bool inFit);
				int				Draw (RWWinPageComposer &inComposer, SRect &ioRect, bool inFit, bool inMeasure, bool inDoDraw);

protected:
				void			Free();
//				void			ApplyAttributes (const RWString inText, CFMutableAttributedStringRef text, CTFontRef font, long *attributes, long start, long end);

protected:
	long				mTextLength;
	long				mCharsPrinted;
	bool				mWrap;
	StringFormat		mStringFormat;
};


//#define	ROUND_UP(x)	(x)
static	int	ROUND_UP (float x)
{
	long	l = ceil (x);
	return (int) l;
}


void
RWWinPageComposer::SetContext (void *inContext)
{
	if ((mFlags & eDestinationMask) == eDestinationScreen)
	{
#if	1
		if (mGraphics && (mDC != reinterpret_cast <HDC> (inContext)))
		{
			delete mGraphics;
			mGraphics = NULL;
		}
		mDC = reinterpret_cast <HDC> (inContext);
		mDocIsOpen = mPageIsOpen = (mDC != NULL);
		if (mDC != NULL && mGraphics == NULL)
		{
			mGraphics = new Graphics (mDC);
//			mGraphics->SetPageUnit (UnitPoint);
		}
#else
		if (mGraphics != NULL)
		{
			mGraphics->ReleaseHDC (mDC);
			delete mGraphics;
			mGraphics = NULL;
			mDC = NULL;
		}
		if (inContext != NULL)
		{
			mGraphics = new Graphics (reinterpret_cast <HWND> (inContext));
//			mGraphics->SetPageUnit (UnitPoint);
			mDC = mGraphics->GetHDC();
		}
#endif
	}
}


void
RWWinPageComposer::SetHWNDContext (HWND inHWND)
{
	if ((mFlags & eDestinationMask) == eDestinationScreen)
	{
		if (mGraphics != NULL)
		{
			delete mGraphics;
			mGraphics = NULL;
		}
		mDocIsOpen = mPageIsOpen = (inHWND != NULL);
		if (inHWND != NULL)
		{
			mGraphics = new Graphics (inHWND);
//			mGraphics->SetPageUnit (UnitPoint);
		}
	}
}


RWNativePageComposer*
RWWinPageComposer::CreateComposerForPrinting (void)
const
{
//	RWWinPageComposer	*pc = new RWWinPageComposer ((GetFlags() & eUserFlagsMask) | eDestinationPrinter, mDestination);
	RWWinPageComposer	*pc = new RWWinPageComposer (*this);
	pc->mFlags = (GetFlags() & eUserFlagsMask) | eDestinationPrinter;
	return pc;
}


// ---------------------------------------------------------------------------
// RWWinPageComposer						Constructor				  [public]
// ---------------------------------------------------------------------------

RWWinPageComposer::RWWinPageComposer (unsigned long inFlags, RWString &inDst, RWString &inPrinter)	//mbs 25072011	printer
	:	RWPageComposer (inFlags, inDst, inPrinter),	//mbs 25072011	printer
		mDocIsOpen (false),
		mTruePageRect (0, 0, 0, 0),
		mOffsetX (0),
		mOffsetY (0),
		mDevMode (0),
		mDevNames (0),
		mPageSetupDlg (0),
		mPrintDlg (0),
		mDC (0),
		mPrinter (0),
		mGraphics (0)
{
	return;
}


// ---------------------------------------------------------------------------
// RWWinPageComposer						Constructor			   [protected]
// ---------------------------------------------------------------------------

RWWinPageComposer::RWWinPageComposer (const RWWinPageComposer &inOriginal)
	:	RWPageComposer (inOriginal),
		mDocIsOpen (false),
		mTruePageRect (0, 0, 0, 0),
		mOffsetX (0),
		mOffsetY (0),
		mDevMode (0),
		mDevNames (0),
		mPageSetupDlg (0),
		mPrintDlg (0),
		mDC (0),
		mPrinter (0),
		mGraphics (0)
{
	*this = inOriginal;
}


// ---------------------------------------------------------------------------
// operator =													   [protected]
// ---------------------------------------------------------------------------

RWWinPageComposer&
RWWinPageComposer::operator = (const RWWinPageComposer &inOriginal)
{
	CloseSession (true);
	StyleChanged (NULL);
	RWPageComposer::operator = (inOriginal);
	
//	mPageIsOpen = false;
//	mDocIsOpen = false;
	mTruePageRect = inOriginal.mTruePageRect;
	mOffsetX = inOriginal.mOffsetX;
	mOffsetY = inOriginal.mOffsetY;

//	mPrinterName = inOriginal.mPrinterName;
	SetDevMode (inOriginal.mDevMode, false);
	SetDeviceNames (inOriginal.mDevNames, false);
	SetPageSetupDialog (inOriginal.mPageSetupDlg);
	SetPrintDialog (inOriginal.mPrintDlg);

	return *this;
}



// ---------------------------------------------------------------------------
// ~RWWinPageComposer						Destructor				  [public]
// ---------------------------------------------------------------------------

RWWinPageComposer::~RWWinPageComposer (void)
{
	CloseSession (true);
	StyleChanged (NULL);

	return;
}


// ---------------------------------------------------------------------------
// GetDevMode														  [public]
// ---------------------------------------------------------------------------

const	HGLOBAL
RWWinPageComposer::GetDevMode (void)
const
{
	return mDevMode;
}

/*
HGLOBAL
RWWinPageComposer::GetDevMode (bool inDetachSignature)
{
	HGLOBAL	hDevMode = mDevMode;
	mDevMode = NULL;
	return hDevMode;
}
*/


// ---------------------------------------------------------------------------
// SetDevMode														  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::SetDevMode (HGLOBAL inDevMode, bool inTakeOwnership)
{
	mTruePageRect.SetRect (0, 0, 0, 0);
	if (mDevMode != inDevMode)
	{
		if (mDevMode != NULL)
		{
			GlobalFree (mDevMode);
			mDevMode = NULL;
		}
		if (inTakeOwnership)
			mDevMode = inDevMode;
		else if (inDevMode != NULL)
		{
			SIZE_T	size = GlobalSize (inDevMode);
			mDevMode = GlobalAlloc (GMEM_MOVEABLE, size);
			LPDEVMODEW srcDevMode = (LPDEVMODEW) GlobalLock (inDevMode);
			LPDEVMODEW dstDevMode = (LPDEVMODEW) GlobalLock (mDevMode);
			memcpy (dstDevMode, srcDevMode, size);
			GlobalUnlock (mDevMode);
			GlobalUnlock (inDevMode);
		}
	}
}

void
RWWinPageComposer::SetDevMode (const SBlob &inDevMode)
{
	mTruePageRect.SetRect (0, 0, 0, 0);
	if (inDevMode)
	{
		HGLOBAL	hDevMode = GlobalAlloc (GMEM_MOVEABLE, inDevMode.fSize);
		void* dstDevMode = GlobalLock (hDevMode);
		memcpy (dstDevMode, inDevMode.fData, inDevMode.fSize);
		GlobalUnlock (hDevMode);
		SetDevMode (hDevMode, true);
	}
	else
		SetDevMode (NULL, true);
}


// ---------------------------------------------------------------------------
// GetDeviceNames													  [public]
// ---------------------------------------------------------------------------

const	HGLOBAL
RWWinPageComposer::GetDeviceNames (void)
const
{
	return mDevNames;
}

/*
HGLOBAL
RWWinPageComposer::GetDeviceNames (bool inDetachSignature)
{
	HGLOBAL	hDevNames = mDevNames;
	mDevNames = NULL;
	return hDevNames;
}
*/


// ---------------------------------------------------------------------------
// SetDeviceNames													  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::SetDeviceNames (HGLOBAL inDeviceNames, bool inTakeOwnership)
{
	if (mDevNames != inDeviceNames)
	{
		if (mDevNames != NULL)
		{
			GlobalFree (mDevNames);
			mDevNames = NULL;
		}
		if (inTakeOwnership)
			mDevNames = inDeviceNames;
		else if (inDeviceNames != NULL)
		{
			SIZE_T	size = GlobalSize (inDeviceNames);
			mDevNames = GlobalAlloc (GMEM_MOVEABLE, size);
			LPDEVNAMES srcDevNames = (LPDEVNAMES) GlobalLock (inDeviceNames);
			LPDEVNAMES dstDevNames = (LPDEVNAMES) GlobalLock (mDevNames);
			memcpy (dstDevNames, srcDevNames, size);
			GlobalUnlock (mDevNames);
			GlobalUnlock (inDeviceNames);
		}
		//mbs 11082010
		if (mDevNames != NULL && mDestination.size() > 4)
		{
			DEVNAMES		*lpDevNames = (DEVNAMES*) GlobalLock (mDevNames);
			const RWString	devn = RWStr::ToUpperASCII (RWStr::FromWide (((WCHAR*) lpDevNames) + lpDevNames->wDeviceOffset));
			GlobalUnlock (mDevNames);

			const char16_t	*suffix = NULL;
			if (RWStr::Contains (devn, u"XPS"))
				suffix = u".XPS";
			else if (RWStr::Contains (devn, u"PDF"))
				suffix = u".PDF";
			if (suffix != NULL)
			{
				// replace an extension of 3 characters, as before
				const RWString	sub = mDestination.substr (mDestination.size() - 4);
				if (!RWStr::EqualsNoCase (sub, RWStringView (suffix)))
				{
					if (sub [0] == u'.')
						mDestination.erase (mDestination.size() - 4);
					mDestination += suffix;
				}
			}
		}
	}
}

void
RWWinPageComposer::SetDeviceNames (const SBlob &inDeviceNames)
{
	if (inDeviceNames)
	{
		HGLOBAL	hDevNames = GlobalAlloc (GMEM_MOVEABLE, inDeviceNames.fSize);
		void* dstDevNames = GlobalLock (hDevNames);
		memcpy (dstDevNames, inDeviceNames.fData, inDeviceNames.fSize);
		GlobalUnlock (hDevNames);
		SetDeviceNames (hDevNames, true);
	}
	else
		SetDeviceNames (NULL, true);
}


// ---------------------------------------------------------------------------
// GetPageSetupDialog												  [public]
// ---------------------------------------------------------------------------

PAGESETUPDLGW	*
RWWinPageComposer::GetPageSetupDialog (void)
const
{
	return mPageSetupDlg;
}


// ---------------------------------------------------------------------------
// SetPageSetupDialog												  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::SetPageSetupDialog (const PAGESETUPDLGW *inPageSetupDlg)
{
	if (mPageSetupDlg != NULL && inPageSetupDlg == NULL)
	{
		delete mPageSetupDlg;
		mPageSetupDlg = NULL;		// was "mPrintDlg = NULL", leaving a dangling pointer
	}
	if (inPageSetupDlg != NULL)
	{
		if (mPageSetupDlg == NULL)
			mPageSetupDlg = new PAGESETUPDLGW;
		memset (mPageSetupDlg, 0, sizeof (PAGESETUPDLGW));
		mPageSetupDlg->lStructSize = sizeof (PAGESETUPDLGW);
		mPageSetupDlg->Flags = inPageSetupDlg->Flags;
		mPageSetupDlg->ptPaperSize = inPageSetupDlg->ptPaperSize;
		mPageSetupDlg->rtMinMargin = inPageSetupDlg->rtMinMargin;
		mPageSetupDlg->rtMargin = inPageSetupDlg->rtMargin;
		if (mPageSetupDlg->Flags & PSD_INHUNDREDTHSOFMILLIMETERS)
		{
			mPageWidth = mPageSetupDlg->ptPaperSize.x * mm2pt / 100;
			mPageHeight = mPageSetupDlg->ptPaperSize.y * mm2pt / 100;
			mReportPageMargins.left = mPageSetupDlg->rtMargin.left * mm2pt / 100;
			mReportPageMargins.top = mPageSetupDlg->rtMargin.top * mm2pt / 100;
			mReportPageMargins.right = mPageSetupDlg->rtMargin.right * mm2pt / 100;
			mReportPageMargins.bottom = mPageSetupDlg->rtMargin.bottom * mm2pt / 100;
		}
		else
		{
			mPageWidth = mPageSetupDlg->ptPaperSize.x * inches2pt / 1000;
			mPageHeight = mPageSetupDlg->ptPaperSize.y * inches2pt / 1000;
			mReportPageMargins.left = mPageSetupDlg->rtMargin.left * inches2pt / 1000;
			mReportPageMargins.top = mPageSetupDlg->rtMargin.top * inches2pt / 1000;
			mReportPageMargins.right = mPageSetupDlg->rtMargin.right * inches2pt / 1000;
			mReportPageMargins.bottom = mPageSetupDlg->rtMargin.bottom * inches2pt / 1000;
		}
		mPageWidth -= mReportPageMargins.left + mReportPageMargins.right;
		mPageHeight -= mReportPageMargins.top + mReportPageMargins.bottom;
	}
}


void
RWWinPageComposer::SetPageSetupDialog (const SBlob &inPageSetupDlg)
{
	if (inPageSetupDlg.fSize >= sizeof (PAGESETUPDLGW))
		SetPageSetupDialog (reinterpret_cast <PAGESETUPDLGW*> (inPageSetupDlg.fData));
	else
		SetPageSetupDialog ((PAGESETUPDLGW*) NULL);
}


// ---------------------------------------------------------------------------
// GetPrintDialog													  [public]
// ---------------------------------------------------------------------------

PRINTDLGW	*
RWWinPageComposer::GetPrintDialog (void)
const
{
	return mPrintDlg;
}


// ---------------------------------------------------------------------------
// SetPrintDialog													  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::SetPrintDialog (const PRINTDLGW *inPrintDlg)
{
	if (mPrintDlg != NULL && inPrintDlg == NULL)
	{
		delete mPrintDlg;
		mPrintDlg = NULL;
	}
	if (inPrintDlg)
	{
		if (mPrintDlg == NULL)
			mPrintDlg = new PRINTDLGW;
		memset (mPrintDlg, 0, sizeof (PRINTDLGW));
		mPrintDlg->lStructSize = sizeof (PRINTDLGW);
		mPrintDlg->Flags = inPrintDlg->Flags;
		mPrintDlg->nFromPage = inPrintDlg->nFromPage;
		mPrintDlg->nToPage = inPrintDlg->nToPage;
		mPrintDlg->nMinPage = inPrintDlg->nMinPage;
		mPrintDlg->nMaxPage = inPrintDlg->nMaxPage;
		mPrintDlg->nCopies = inPrintDlg->nCopies;
	}
}


void
RWWinPageComposer::SetPrintDialog (const SBlob &inPrintDlg)
{
	if (inPrintDlg.fSize >= sizeof (PRINTDLGW))
		SetPrintDialog (reinterpret_cast <PRINTDLGW*> (inPrintDlg.fData));
	else
		SetPrintDialog ((PRINTDLGW*) NULL);
}


# if	_4D_Package_
// 4D's own print settings (eUse4DPageSetup / eUse4DJobSetup). The structures may be
// ANSI or UTF-16 (4D is a Unicode application): DEVMODE is told apart by dmSize,
// DEVNAMES by the driver name ("winspool" in UTF-16 has a zero second byte).
void
RWWinPageComposer::Adopt4DSetting (void)
{
	if ((mFlags & (eUse4DPageSetup | eUse4DJobSetup)) == 0)
		return;
	mFlags &= ~(eUse4DPageSetup | eUse4DJobSetup);

	HGLOBAL	dlg4d = (HGLOBAL) PA_GetWindowsPRINTDLG();
	PRINTDLGW	*dlg = dlg4d ? (PRINTDLGW*) GlobalLock (dlg4d) : NULL;	// handle members are the same in PRINTDLGA
	if (dlg == NULL)
		return;

	if (dlg->hDevNames)
	{
		const DEVNAMES	*names = (const DEVNAMES*) GlobalLock (dlg->hDevNames);
		if (names)
		{
			const bool	wide = ((const char*) names) [names->wDriverOffset + 1] == 0;
			HGLOBAL		hDevNames = CreateDevNames (DevNamesString (names, names->wDriverOffset, wide),
													DevNamesString (names, names->wDeviceOffset, wide),
													DevNamesString (names, names->wOutputOffset, wide), names->wDefault);
			GlobalUnlock (dlg->hDevNames);
			if (hDevNames)
				SetDeviceNames (hDevNames, true);
		}
	}

	if (dlg->hDevMode)
	{
		const SIZE_T	size = GlobalSize (dlg->hDevMode);
		const void		*mode = GlobalLock (dlg->hDevMode);
		if (mode && size >= sizeof (DEVMODEW) && ((const DEVMODEW*) mode)->dmSize == sizeof (DEVMODEW))
			SetDevMode (dlg->hDevMode, false);		// already UTF-16: copied
		else if (mode && size >= sizeof (DEVMODEA) && ((const DEVMODEA*) mode)->dmSize == sizeof (DEVMODEA))
		{
			const DEVMODEA	*src = (const DEVMODEA*) mode;
			const SIZE_T	extra = src->dmDriverExtra;
			HGLOBAL			hDevMode = GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof (DEVMODEW) + extra);
			DEVMODEW		*dst = hDevMode ? (DEVMODEW*) GlobalLock (hDevMode) : NULL;
			if (dst)
			{
				MultiByteToWideChar (CP_ACP, 0, (const char*) src->dmDeviceName, CCHDEVICENAME, dst->dmDeviceName, CCHDEVICENAME);
				dst->dmDeviceName [CCHDEVICENAME - 1] = 0;
				memcpy (&dst->dmSpecVersion, &src->dmSpecVersion, offsetof (DEVMODEA, dmFormName) - offsetof (DEVMODEA, dmSpecVersion));
				dst->dmSize = sizeof (DEVMODEW);	// was written into 4D's ANSI structure
				MultiByteToWideChar (CP_ACP, 0, (const char*) src->dmFormName, CCHFORMNAME, dst->dmFormName, CCHFORMNAME);
				dst->dmFormName [CCHFORMNAME - 1] = 0;
				memcpy (&dst->dmLogPixels, &src->dmLogPixels, sizeof (DEVMODEA) - offsetof (DEVMODEA, dmLogPixels) + extra);
				GlobalUnlock (hDevMode);
				SetDevMode (hDevMode, true);		// the converted DEVMODE was never used before
			}
			else if (hDevMode)
				GlobalFree (hDevMode);
		}
		GlobalUnlock (dlg->hDevMode);
	}

	GlobalUnlock (dlg4d);
}
# endif

long
RWWinPageComposer::AdoptDefSetting (bool inOrientation)
{
	long		status = noErr;

	// get default printer
	PRINTDLGW	pd;
	memset (&pd, 0, sizeof (PRINTDLGW));
	pd.lStructSize = sizeof (PRINTDLGW);
	pd.Flags = PD_RETURNDEFAULT;
	if (!PrintDlgW (&pd))
	{
		status = ::CommDlgExtendedError();
		if ( pd.hDevMode )
			::GlobalFree(pd.hDevMode);
		if ( pd.hDevNames )
			::GlobalFree(pd.hDevNames);
	}
	else
	{
		//mbs 29062011	don't use default printer on Windows - separate DevMode & DevNames
		
		// if orientation set,
		if (pd.hDevMode) 
		{
			if (inOrientation) {
				LPDEVMODEW srcDevMode = (LPDEVMODEW) GlobalLock (pd.hDevMode);
				srcDevMode->dmOrientation = DMORIENT_LANDSCAPE;
				srcDevMode->dmFields = srcDevMode->dmFields | DM_ORIENTATION;
				GlobalUnlock (pd.hDevMode);

			}
		}
		if (mDevMode == NULL || (mFlags & (eUseDefPageSetup | eUseDefJobSetup)) == (eUseDefPageSetup | eUseDefJobSetup))
		{
			SetDevMode (pd.hDevMode, true);
		}
		else	// either eUseDefPageSetup or eUseDefJobSetup is true
		{
	        if ( pd.hDevMode )
	            ::GlobalFree (pd.hDevMode);
		}
		if (mDevNames == NULL || (mFlags & eNoDefPrinter) == 0)
		{
			SetDeviceNames (pd.hDevNames, true);
		}
		else
		{
	        if ( pd.hDevNames )
	            ::GlobalFree (pd.hDevNames);
		}
	}

	//mbs 11082010	Preview
	if (status == noErr && ((mFlags & eDestinationMask) == eDestinationPreview))
	{
		//mbs 10052011	default name for preview
		if (mDestination.empty())
		{
			// "RW_Preview.pdf" in My Documents, numbered if it exists
			wchar_t	folder [MAX_PATH];
			if (SHGetFolderPathW (NULL, CSIDL_MYDOCUMENTS | CSIDL_FLAG_CREATE, NULL, 0 /* SHGFP_TYPE_CURRENT */, folder) == S_OK)
			{
				for (int i = 0; i < 65536; i++)
				{
					wchar_t	name [MAX_PATH + 32];
					if (i == 0)
						swprintf (name, MAX_PATH + 32, L"%ls\\RW_Preview.pdf", folder);
					else
						swprintf (name, MAX_PATH + 32, L"%ls\\RW_Preview %d.pdf", folder, i);
					int	fh = _wopen (name, O_WRONLY | O_BINARY | O_CREAT | O_EXCL, _S_IREAD | _S_IWRITE);
					if (fh != -1)
					{
						_close (fh);
						mDestination = RWStr::FromWide (name);
						break;
					}
				}
			}
		}

		// the file name comes from DOCINFO::lpszOutput (was "Microsoft XPS Document Writer" on "XPSPort:")
		HGLOBAL	hDevNames = CreateDevNames (u"winspool", u"Microsoft Print to PDF", u"PORTPROMPT:");
		if (hDevNames)
			SetDeviceNames (hDevNames, true);
	}

	else if (status == noErr && ((mFlags & eDestinationMask) == eDestinationPrinter) && !mPrinterName.empty())	//mbs 25072011	printer
	{
		HGLOBAL	hDevNames = CreateDevNames (u"winspool", mPrinterName, u"");
		if (hDevNames)
			SetDeviceNames (hDevNames, true);
	}

	if (status == noErr && (mDevMode == NULL || mDevNames == NULL))
		status = nilHandleErr;

	return status;
}


// ---------------------------------------------------------------------------
// StyleChanged														  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::StyleChanged (RWStyle *inStyle)
{
	if (inStyle == NULL)
	{
		RWStyleToFontMap::const_iterator	it;
		
		for (it = mStyleMap.begin(); it != mStyleMap.end(); it++)
		{
			Font*	font = (*it).second;
			delete font;
		}
		mStyleMap.clear();
	}
	else
	{
//		RWStyleToFontMap::key_type v (inStyle);
		RWStyleToFontMap::iterator	it = mStyleMap.find (*inStyle);	//mbs 29062011
		if (it != mStyleMap.end())
		{
			Font*	font = (*it).second;
			delete font;
			mStyleMap.erase (it);
		}
	}
}


// ---------------------------------------------------------------------------
// MapStyle														   [protected]
// ---------------------------------------------------------------------------

Gdiplus::Font*
RWWinPageComposer::MapStyle (RWStyle *inStyle)
{
	Font*	font = NULL;

	{
		//mbs 29062011	RWStyle, not RWStyle*
		RWStyleToFontMap::const_iterator	it = mStyleMap.find (*inStyle);
		if (it != mStyleMap.end())
			font = (*it).second;
/*		
		for (it = mStyleMap.begin(); it != mStyleMap.end(); it++)
		{
			if (inStyle == (*it).first)
			{
				font = reinterpret_cast <Gdiplus::Font*> ((*it).second);
				break;
			}
		}
*/
	}

	if (font == NULL)
	{
//		FontFamily	fontFamily ((const wchar_t*) inStyle->GetFName());
//		font = new Font (&fontFamily, inStyle->GetSize(), FontStyle (inStyle->GetStyle() & 0x0F), UnitPoint, NULL);
		Unit	unit = GetDestination() == RWPageComposer::eDestinationScreen? UnitPixel: UnitPoint;
		font = new Font (RWStr::ToWide (inStyle->GetFName()).c_str(), inStyle->GetSize(), FontStyle (inStyle->GetStyle() & 0x0F), unit, NULL);
		if (not font->IsAvailable())
		{
			delete font;
			font = new Font (L"Arial", inStyle->GetSize(), FontStyle (inStyle->GetStyle() & 0x0F), unit, NULL);
		}
		mStyleMap.insert (RWStyleToFontMap::value_type (*inStyle, font));
	}

	return font;
}


// ---------------------------------------------------------------------------
// OpenSession													   [protected]
// ---------------------------------------------------------------------------

long
RWWinPageComposer::OpenSession (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation)
{
	long		status = noErr;

	if (mGraphics != NULL)
	{
		delete mGraphics;
		mGraphics = NULL;
	}

	if (mBatchLevel == 0)
	{
		if (mDC != NULL)
		{
			::DeleteDC (mDC);
			mDC = NULL;
		}

# if	_4D_Package_
		Adopt4DSetting();
#endif

		status = AdoptDefSetting(inOrientation);

		if (status == noErr && inDoPageSetup && AskPageSetup())
		{
			PAGESETUPDLGW	psd;
/*
			if (mPageSetupDlg != NULL)
			{
				psd = *mPageSetupDlg;
				psd.Flags = (psd.Flags & (PSD_INTHOUSANDTHSOFINCHES | PSD_INHUNDREDTHSOFMILLIMETERS)) | PSD_MARGINS;
			}
			else
*/
			{
				memset (&psd, 0, sizeof (PAGESETUPDLGW));
				psd.lStructSize = sizeof (PAGESETUPDLGW);
				psd.Flags = PSD_DEFAULTMINMARGINS | PSD_INHUNDREDTHSOFMILLIMETERS | PSD_MARGINS;
				psd.ptPaperSize.x = round ((mPageWidth + mReportPageMargins.left + mReportPageMargins.right) * 100 * pt2mm);
				psd.ptPaperSize.y = round ((mPageHeight + mReportPageMargins.top + mReportPageMargins.bottom) * 100 * pt2mm);
				psd.rtMargin.left = round (mReportPageMargins.left * 100 * pt2mm);
				psd.rtMargin.top = round (mReportPageMargins.top * 100 * pt2mm);
				psd.rtMargin.right = round (mReportPageMargins.right * 100 * pt2mm);
				psd.rtMargin.bottom = round (mReportPageMargins.bottom * 100 * pt2mm);
				
			}
			psd.hDevMode = mDevMode;
			psd.hDevNames = mDevNames;
			if (!PageSetupDlgW (&psd))
			{
				status = ::CommDlgExtendedError();
				if (ERROR_SUCCESS == status)
					status = -128;	// errUserCanceled;
				SetDevMode (psd.hDevMode, true);
				SetDeviceNames (psd.hDevNames, true);
			}
			else
			{
				SetDevMode (psd.hDevMode, true);
				SetDeviceNames (psd.hDevNames, true);
				SetPageSetupDialog (&psd);
			}
		}

		PRINTDLGW	pd;
		if (status == noErr && inDoJobSetup && AskJobSetup())
		{
			if (mPrintDlg != NULL)
			{
				pd = *mPrintDlg;
				pd.Flags = PD_ALLPAGES | PD_NOSELECTION | PD_NOCURRENTPAGE | PD_USEDEVMODECOPIESANDCOLLATE;
			}
			else
			{
				memset (&pd, 0, sizeof (PRINTDLGW));
				pd.lStructSize = sizeof (PRINTDLGW);
				pd.Flags = PD_ALLPAGES | PD_NOSELECTION | PD_NOCURRENTPAGE | PD_USEDEVMODECOPIESANDCOLLATE;
			}
			if (inCurPage == 0)
				pd.Flags |= PD_NOPAGENUMS;
			else
			{
				pd.nMinPage = pd.nFromPage = inCurPage;
				pd.nMaxPage = pd.nToPage = (inNumPages > USHRT_MAX? USHRT_MAX: inNumPages);
			}
			pd.hDC = 0;
			pd.Flags |= PD_RETURNDC;
			pd.hDevMode = mDevMode;
			pd.hDevNames = mDevNames;
			if (!PrintDlgW (&pd))
			{
				status = ::CommDlgExtendedError();
				if (ERROR_SUCCESS == status)
					status = -128;	// errUserCanceled;
				SetDevMode (pd.hDevMode, true);
				SetDeviceNames (pd.hDevNames, true);
			}
			else
			{
				SetDevMode (pd.hDevMode, true);
				SetDeviceNames (pd.hDevNames, true);
				SetPrintDialog (&pd);
			}
			mDC = pd.hDC;
		}

		if (status == noErr)
		{
#if	0
			if (mPrintDlg != NULL)
			{
				pd = *mPrintDlg;
			}
			else
			{
				memset (&pd, 0, sizeof (PRINTDLGW));
				pd.lStructSize = sizeof (PRINTDLGW);
			}
			pd.Flags = PD_RETURNDC;
			pd.hDevMode = mDevMode;
			pd.hDevNames = mDevNames;
			if (!PrintDlgW (&pd))
			{
				status = ::CommDlgExtendedError();
				if (ERROR_SUCCESS == status)
					status = -128;	// errUserCanceled;
				SetDevMode (pd.hDevMode, true);
				SetDeviceNames (pd.hDevNames, true);
			}
			else
			{
				SetDevMode (pd.hDevMode, true);
				SetDeviceNames (pd.hDevNames, true);
			}
			mDC = pd.hDC;
#else
			{
				DEVNAMES	*lpDevNames = (DEVNAMES*) GlobalLock (mDevNames);
				DEVMODEW	*lpDevMode = (DEVMODEW*) GlobalLock (mDevMode);
				::OpenPrinterW ((LPWSTR) (((WCHAR*) lpDevNames) + lpDevNames->wDeviceOffset), &mPrinter, NULL);
				if (mDC == NULL)
				{
				    mDC = ::CreateDCW ( (LPWSTR) ((WCHAR*) lpDevNames) + lpDevNames->wDriverOffset,
				    					(LPWSTR) ((WCHAR*) lpDevNames) + lpDevNames->wDeviceOffset,
				    					(LPWSTR) ((WCHAR*) lpDevNames) + lpDevNames->wOutputOffset,
				    					lpDevMode);
/*
				    mDC = ::CreateDCW ( NULL,	// L"WINSPOOL",
				    					(LPWSTR) ((WCHAR*) lpDevNames) + lpDevNames->wDeviceOffset,
				    					NULL,
				    					lpDevMode);
*/
				    if (mDC == NULL)
				    	status = GetLastError();
				}
				GlobalUnlock (mDevMode);
				GlobalUnlock (mDevNames);

				//mbs 08072010
				if (status == noErr)
				{
					if (inDoPageSetup)
						mFlags &= ~eAskPageSetup;
					if (inDoJobSetup && AskJobSetup())
					{
						mFlags |= eRanJobSetup;
						mFlags &= ~eAskJobSetup;
					}
				}
			}
#endif
		}
	}
/*
	else
	{
		mDC = static_cast <RWNativePageComposer*> (mBatch)->mDC;
		mPrinter = static_cast <RWNativePageComposer*> (mBatch)->mPrinter;
	}
*/
			
	if (status == noErr && mDC != NULL)
	{
		if (mPrinter)
			mGraphics = new Graphics (mDC, mPrinter);
		else
			mGraphics = new Graphics (mDC);
		mGraphics->SetPageUnit (UnitPoint);
	}

	if (status == noErr && (mDC == NULL || mGraphics == NULL))
		status = -128;	// errUserCanceled;
	
//	if (status == noErr && mBatch)
//		mDocIsOpen = true;

	return status;
}


// ---------------------------------------------------------------------------
// CloseSession													   [protected]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::CloseSession (bool inRelease)
{
	ClosePage();	//mbs 08102010	always close page
	StyleChanged (NULL);	//mbs 08102010	clear cache

	if (mBatchLevel == 0)
	{
		if (mDocIsOpen)
		{
			if ((mFlags & eDestinationMask) != eDestinationScreen)
				::EndDoc (mDC);
			mDocIsOpen = false;
		}

		if (inRelease)
		{
			if (mGraphics != NULL)
			{
				delete mGraphics;
				mGraphics = NULL;
			}
			if (mPrinter != NULL)
			{
				::ClosePrinter (mPrinter);
				mPrinter = NULL;
			}
			if (mDC != NULL)
			{
				::DeleteDC (mDC);
				mDC = NULL;
			}
			if (mDevMode != NULL)
			{
				GlobalFree (mDevMode);
				mDevMode = NULL;
			}
			if (mDevNames != NULL)
			{
				GlobalFree (mDevNames);
				mDevNames = NULL;
			}
			if (mPageSetupDlg != NULL)
			{
				delete mPageSetupDlg;
				mPageSetupDlg = NULL;
			}
			if (mPrintDlg != NULL)
			{
				delete mPrintDlg;
				mPrintDlg = NULL;
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// FinishReport														  [public]
// ---------------------------------------------------------------------------
// Close the page, close the session

void*
RWWinPageComposer::FinishReport (size_t &outSize)
{
	CloseSession (true);

	outSize = 0;

	return NULL;
}


// ---------------------------------------------------------------------------
// ParseReport														  [public]
// ---------------------------------------------------------------------------
// Get default page size & page orientation

void
RWWinPageComposer::ParseReport (RWXmlNode inReport)
{
/*
	if (mBatch)
	{
		*this = *static_cast <RWNativePageComposer*> (mBatch);
	}
*/
	if (mBatchLevel != 0)
		;
	else if ((mFlags & (eUseDefPageSetup | eUse4DPageSetup | eUseDefJobSetup | eUse4DJobSetup)) == 0)
	{
		RWPageComposer::ParseReport (inReport);

		SBlob		blob;
		blob.Init();
		try
		{
			if (RWXmlNode elem = inReport.Child (u"DevMode"))
			{
				RWTools::ReadData (elem, blob);
				SetDevMode (blob);
				blob.Free();
			}

			if ((mFlags & eNoDefPrinter) != 0)	//mbs 29062011	don't use default printer on Windows
				if (RWXmlNode elem = inReport.Child (u"DeviceNames"))
				{
					RWTools::ReadData (elem, blob);
					SetDeviceNames (blob);
					blob.Free();
				}

			if (RWXmlNode elem = inReport.Child (u"PageSetupDlg"))
			{
				RWTools::ReadData (elem, blob);
				SetPageSetupDialog (blob);
				blob.Free();
			}

			if (RWXmlNode elem = inReport.Child (u"PrintDlg"))
			{
				RWTools::ReadData (elem, blob);
				SetPrintDialog (blob);
				blob.Free();
			}
		}
		catch (...)
		{
		}
		if (blob)
			blob.Free();
	}

	return;
}


// ---------------------------------------------------------------------------
// GetPageBounds													  [public]
// ---------------------------------------------------------------------------
// Get default page size, page orientation and encoding

void
RWWinPageComposer::GetPageBounds (const RWString inOrientation, const RWString inSize, SRect &outRect)
{
	bool		changed = false;
	OSStatus	status;

	//mbs 24122009	always set the rectangles/margins!
	if (mPageRect.Width() == 0 || mTruePageRect.Width() == 0)
	{
		if (GetPageMetrics (mTruePageRect, mPaperRect, mReportPageMargins))
			if (mUsePhysical) {
				mPageRect = mPaperRect;	
			} else {
				mPageRect = mTruePageRect;
			}
	}

	if (mPageRect.Width() == 0 || mTruePageRect.Width() == 0 || AskPageSetup() /* || mGraphics == NULL */)	//mbs 19012011	added mGraphics condition //mbs 25012011	not good for multipage single job
	{
		status = OpenSession (true, false, 1, 1, RWStr::Equals (mPageOrientation, "Landscape")); // pB 2012 we need paper size of destination, not default
		CloseSession (false);
		if (status != noErr)
		{
			printf ("RWWinPageComposer::GetPageBounds: OpenSession: status != noErr\n");
			throw status;
		}
		changed = true;
	}

	//mbs 24122009	always set the rectangles/margins!
	if (changed)
	{
		if (GetPageMetrics (mTruePageRect, mPaperRect, mReportPageMargins))
			if (mUsePhysical) {
				mPageRect = mPaperRect;	
			} else {
				mPageRect = mTruePageRect;
			}
	}
	else if (mBatchLevel > 0 && mGraphics == NULL && mDC != NULL)	//mbs 25012011	added - we need mGraphics for session (from 2-nd report on)
	{
		if (mPrinter)
			mGraphics = new Graphics (mDC, mPrinter);
		else
			mGraphics = new Graphics (mDC);
		mGraphics->SetPageUnit (UnitPoint);
	}

	if (mUsePhysical) {
		outRect = mPaperRect;	
	} else {
		
		if (mPageRect.Width() == 0)
			RWPageComposer::GetPageBounds (inOrientation, inSize, outRect);
		else
		{
	 // pB 2012-1
			if (!mUseReportMargins)
			{
				mOffsetX = 0;
				mOffsetY = 0;
	//			mPageRect.bottom += mReportPageMargins.top;
	//			mPageRect.right += mReportPageMargins.left;
	//			mPageRect.bottom = mTruePageRect.top + mPaperRect.Height();
	//			mPageRect.right = mTruePageRect.left + mPaperRect.Width();
			}

			outRect = mPageRect;
		}
	}

	return;
}


SRect
RWWinPageComposer::GetTruePageRect (void) const
{
	if (mUseReportMargins)
		return mTruePageRect + SPoint (mTruePageRect.left - mPaperRect.left, mTruePageRect.top - mPaperRect.top);
	return mTruePageRect;
}


// ---------------------------------------------------------------------------
// GetPageMetrics													  [public]
// ---------------------------------------------------------------------------
// Get page size and margins

bool
RWWinPageComposer::GetPageMetrics (float &outPageWidth, float &outPageHeight, SRect &outMargins)
{
	SRect pageRect, paperRect;
	if (GetPageMetrics (pageRect, paperRect, outMargins))
	{
		outPageWidth = pageRect.Width();
		outPageHeight = pageRect.Height();
		return true;
	}
	return false;
}


// ---------------------------------------------------------------------------
// GetPageMetrics													  [public]
// ---------------------------------------------------------------------------
// Get page size and margins

bool
RWWinPageComposer::GetPageMetrics (SRect &outPageRect, SRect &outPaperRect, SRect &outMargins)
{
	if (mDevMode != NULL && mDC != NULL)
	{
		int nLogPixelsX      = GetDeviceCaps( mDC, LOGPIXELSX      );
		int nLogPixelsY      = GetDeviceCaps( mDC, LOGPIXELSY      );
		int nPhysicalOffsetX = GetDeviceCaps( mDC, PHYSICALOFFSETX );
		int nPhysicalOffsetY = GetDeviceCaps( mDC, PHYSICALOFFSETY );
		int nPhysicalWidth   = GetDeviceCaps( mDC, PHYSICALWIDTH   );
		int nPhysicalHeight  = GetDeviceCaps( mDC, PHYSICALHEIGHT  );
		int	pixel_width		 = GetDeviceCaps( mDC, HORZRES         );
		int	pixel_height	 = GetDeviceCaps( mDC, VERTRES         );

		// user - entered values recalculated to pixels
		float nLeftMargin   = mReportPageMargins.left * pt2inches * nLogPixelsX;
		float nTopMargin    = mReportPageMargins.top * pt2inches * nLogPixelsY;
		float nRightMargin  = mReportPageMargins.right * pt2inches * nLogPixelsX;
		float nBottomMargin = mReportPageMargins.bottom * pt2inches * nLogPixelsY;

		// Adjust to physical offsets:
		nLeftMargin   = nPhysicalOffsetX; // __max( nPhysicalOffsetX, nLeftMargin   ); pB manual margins need to be rethought
		nTopMargin    = nPhysicalOffsetY; //__max( nPhysicalOffsetY, nTopMargin    );
		
		int nPhysicalOffsetRight = nPhysicalWidth - pixel_width - nPhysicalOffsetX;
		int nPhysicalOffsetBottom = nPhysicalHeight - pixel_height - nPhysicalOffsetY;

		nRightMargin  =  nPhysicalOffsetRight; //__max( nPhysicalOffsetRight, nRightMargin  );
		nBottomMargin =  nPhysicalOffsetBottom; //__max( nPhysicalOffsetBottom, nBottomMargin );

		float	iWidth  = nPhysicalWidth -  (nLeftMargin + nRightMargin) ;		// / nLogPixelsX;
		float	iHeight = nPhysicalHeight -  (nTopMargin + nBottomMargin) ;	// / nLogPixelsY;

		outPaperRect.SetRect (0.0, 0.0, inches2pt * nPhysicalHeight / nLogPixelsY, inches2pt * nPhysicalWidth / nLogPixelsX);
		outPageRect.SetRect (0.0, 0.0, inches2pt * iHeight / nLogPixelsY, inches2pt * iWidth / nLogPixelsX);
		outMargins.SetRect (inches2pt * nTopMargin / nLogPixelsY, inches2pt * nLeftMargin / nLogPixelsX, inches2pt * nBottomMargin / nLogPixelsY, inches2pt * nRightMargin / nLogPixelsX);
		SPoint	offset (-nLeftMargin * inches2pt / nLogPixelsX, -nTopMargin * inches2pt / nLogPixelsY);
		outPaperRect += offset;
		
		/* pB changed 2012-02
		SPoint	offset (nLeftMargin * inches2pt / nLogPixelsX, nTopMargin * inches2pt / nLogPixelsY);
		outPaperRect += offset;
		outMargins.SetRect (outPageRect.top - outPaperRect.top, outPageRect.left - outPaperRect.left, outPaperRect.bottom - outPageRect.bottom, outPaperRect.right - outPageRect.right);
		*/
		
		if (mUseReportMargins) 
		{
			mOffsetX = nLeftMargin * inches2pt / nLogPixelsX;
			mOffsetY = nTopMargin * inches2pt / nLogPixelsY;			
		}

		return true;
	}
	return false;
}


// ---------------------------------------------------------------------------
// OpenNewPage														  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::OpenNewPage (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages)
{
	long	status = 0;
	if (not mDocIsOpen)
	{
		status = OpenSession (false, true, inCurPage, inNumPages, RWStr::Equals (mPageOrientation, "Landscape"));
		if (status != noErr || mDC == 0)
		{
			printf ("RWWinPageComposer::OpenNewPage: OpenSession: status != noErr || mDC == nil\n");
			throw status;
		}

		// pB 2012 - paper size could not changed, but margins yes
		if (GetPageMetrics (mTruePageRect, mPaperRect, mReportPageMargins))
			if (mUsePhysical) {
				mPageRect = mPaperRect;	
			} else {
				mPageRect = mTruePageRect;
			}

		// kept alive until StartDocW has run
		const std::wstring	docName = RWStr::ToWide (mJobName);
		const std::wstring	outName = RWStr::ToWide (mDestination);
		DOCINFOW	docinfo;
		docinfo.cbSize = sizeof (DOCINFOW);
		docinfo.lpszDocName = docName.c_str();

		if ((mFlags & eDestinationMask) == eDestinationPrinter)	//mbs 11082010
			docinfo.lpszOutput = NULL;
		else
		{
//			UString	outName;
//			if (mDestination && *mDestination)
//				outName.AssignUTF8 (reinterpret_cast <const UTF8Char*> (mDestination));
//			docinfo.lpszOutput = outName.GetWStr();	//••• TODO •••	needs this to persist?!?
			docinfo.lpszOutput = outName.empty() ? NULL : outName.c_str();
		}
		docinfo.lpszDatatype = NULL;
		docinfo.fwType = 0;
		int ret = ::StartDocW (mDC, &docinfo);
		if (ret <= 0)
		{
			status = GetLastError();
			printf ("RWWinPageComposer::OpenNewPage: OpenSession: StartDocW() failed\n");
			throw status;
		}
		mDocIsOpen = true;
	}
	else
		ClosePage();

	if (inCurPage < mFirstPage)
		return;
	if (inCurPage > mLastPage)
		return;

	::StartPage (mDC);
	mPageIsOpen = true;

//mbs 19012011	delete already created mGraphics
	if (mGraphics != NULL)
		delete mGraphics;

	if (mPrinter)
		mGraphics = new Graphics (mDC, mPrinter);
	else
		mGraphics = new Graphics (mDC);
	mGraphics->SetPageUnit (UnitPoint);

	Matrix	matrix (1, 0, 0, 1, -mOffsetX, -mOffsetY);
	mGraphics->SetTransform (&matrix);

#if	TARGET_DEBUG
	SRect		rect (mTruePageRect);
	rect *= 1;
	if (mUseReportMargins)
		rect -= mPaperRect.TopLeft();
	DrawRect (rect, 0.25, true, cRedColor, false, cRedColor, 2, 1);
	rect = mPaperRect;
	rect *= 1;
	if (mUseReportMargins)
		rect -= mPaperRect.TopLeft();
	DrawRect (rect, 0.25, true, cBlueColor, false, cBlueColor);
#endif

	return;
}


// ---------------------------------------------------------------------------
// ClosePage														  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::ClosePage (void)
{
	if (mPageIsOpen)
	{
		if (mGraphics != NULL)
		{
			delete mGraphics;
			mGraphics = NULL;
		}
		if ((mFlags & eDestinationMask) != eDestinationScreen)
			::EndPage (mDC);
		mPageIsOpen = false;
	}

	return;
}


// ---------------------------------------------------------------------------
// ClipToRect														  [public]
// ---------------------------------------------------------------------------

RWClipInfoRef
RWWinPageComposer::ClipToRect (const SRect &inRect)
{
//	GraphicsContainer	gc = mGraphics->BeginContainer();
	RWContextInfoRef	gc = 0;
	SaveContext (gc);

	if (gc)	//mbs 19012011
	{
		RectF	r (inRect.left, inRect.top, inRect.Width(), inRect.Height());
		mGraphics->SetClip (r, CombineModeIntersect);
	}
	return (RWClipInfoRef) gc;
}


// ---------------------------------------------------------------------------
// ClipToRect														  [public]
// ---------------------------------------------------------------------------

RWClipInfoRef
RWWinPageComposer::ClipToRect (const SRect &inRect, const SRect &inExcludeRect)
{
//	GraphicsContainer	gc = mGraphics->BeginContainer();
	RWContextInfoRef	gc = 0;
	SaveContext (gc);

	if (gc)	//mbs 19012011
	{
		RectF	r (inRect.left, inRect.top, inRect.Width(), inRect.Height());
		mGraphics->SetClip (r, CombineModeIntersect);
		if (not inExcludeRect.IsEmpty())
		{
			RectF	re (inExcludeRect.left, inExcludeRect.top, inExcludeRect.Width(), inExcludeRect.Height());
			mGraphics->SetClip (re, CombineModeExclude);
		}
	}
	return (RWClipInfoRef) gc;
}


// ---------------------------------------------------------------------------
// RestoreClip														  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::RestoreClip (RWClipInfoRef &ioClipInfo)
{
	if (ioClipInfo)
	{
		mGraphics->EndContainer ((GraphicsContainer) ioClipInfo);
		ioClipInfo = 0;
	}
	return;
}



void	RWWinPageComposer::SaveContext (RWContextInfoRef &outContext)
{
	if (mGraphics)
	{
		GraphicsContainer	gc = mGraphics->BeginContainer();
		outContext = reinterpret_cast <RWContextInfoRef> (gc);
		if (GetDestination() != RWPageComposer::eDestinationScreen) 
			mGraphics->SetPageUnit (UnitPoint);
	}
	else
		outContext = 0;
}

void	RWWinPageComposer::RestoreContext (RWContextInfoRef &ioContext)
{
	if (mGraphics && ioContext)
	{
		mGraphics->EndContainer ((GraphicsContainer) ioContext);
		ioContext = 0;
	}
}

void	RWWinPageComposer::ApplyTransform (CGAffineTransform &inMatrix)
{
	if (mGraphics)
	{
		Matrix	matrix (inMatrix.a, inMatrix.b, inMatrix.c, inMatrix.d, inMatrix.tx, inMatrix.ty);
		mGraphics->SetTransform (&matrix);
	}
}

double	RWWinPageComposer::MeasureWord (const RWString inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading)
{
#if	TARGET_DEBUG__
	static	bool	sAdjustDescent = false;
#endif

	float width = 0;
	if (mGraphics)
	{
		RectF	r (0, 0, 0, 0);
		RectF	bbox (0, 0, 0, 0);
		Font	*font = MapStyle (inStyle);
		StringFormat	format (StringFormat::GenericTypographic());
		format.SetFormatFlags (StringFormatFlagsMeasureTrailingSpaces);
		const std::wstring	text = RWStr::ToWide (RWStringView (inText).substr (0, size_t (std::max (inTextLength, 0))));
		mGraphics->MeasureString (text.c_str(), INT (text.size()), font, r, &format, &bbox);
		width = bbox.Width;
//		outHeight = bbox.Height;
		FontFamily	ff;
		font->GetFamily (&ff);
		REAL	fontSize = font->GetSize();
		INT		fontStyle = font->GetStyle();
		REAL	emHeight = ff.GetEmHeight (fontStyle);
		outAscent = fontSize / emHeight * ff.GetCellAscent (fontStyle);
		outDescent = fontSize / emHeight * ff.GetCellDescent (fontStyle);
		outLeading = fontSize / emHeight * ff.GetLineSpacing (fontStyle) - outAscent - outDescent;

		//mbs 30062010	TODO: FIX ME!!!!
#if	TARGET_DEBUG__
		if (sAdjustDescent)
			outDescent = bbox.Height - outAscent;
#endif
	}
	else {
		// attemtp to measure text before Graphics is open - there is some unitinialized text?
		width = 0.7 * inStyle->GetSize() * inTextLength;
		outAscent = inStyle->GetSize();
		outDescent = inStyle->GetSize() * 0.2;
		outLeading = 0;

	}

	return width;
}

void	RWWinPageComposer::DrawWord (const RWString inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle)
{
#if	TARGET_DEBUG__
	static	bool	sFrameText = false;
#endif

	if (mPageIsOpen)
	{
		Font		*font = MapStyle (inStyle);
		FontFamily	ff;
		font->GetFamily (&ff);
		REAL		fontSize = font->GetSize();
		INT			fontStyle = font->GetStyle();
		REAL		emHeight = ff.GetEmHeight (fontStyle);
		REAL		ascent = fontSize / emHeight * ff.GetCellAscent (fontStyle);
		//if (GetDestination() == RWPageComposer::eDestinationScreen)
		//	ascent *= mGraphics->GetDpiY() / 72;		//pB we have same unit on screen

		Color		color = ToColor (inStyle->GetTextColor());
		SolidBrush	solidBrush (color);
		RectF		r (inX, inBaseLine - ascent, 0, 0);

		float		hScale = inStyle->GetHorizontalScale();
		RWContextInfoRef	gs = 0;
		if (hScale != 1)
		{
			SaveContext (gs);
			Matrix	matrix (hScale, 0, 0, 1, 0, 0);
			mGraphics->SetTransform (&matrix);
		}
			
		const std::wstring	text = RWStr::ToWide (RWStringView (inText).substr (0, size_t (std::max (inTextLength, 0))));
		mGraphics->DrawString (text.c_str(), INT (text.size()), font, r, StringFormat::GenericTypographic(), &solidBrush);
#if	TARGET_DEBUG__
		if (sFrameText)
		{
			mGraphics->MeasureString (text.c_str(), INT (text.size()), font, r, StringFormat::GenericTypographic(), &r);
			Pen		pen (color, 0.25);
			mGraphics->DrawRectangle (&pen, r);
		}
#endif
		if (gs)
			RestoreContext (gs);
	}
}



// ---------------------------------------------------------------------------
// DrawLine															  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::DrawLine (float top, float left, float bottom, float right, float inThickness, SRGBColor inLineColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
		Color	color = ToColor (inLineColor);
		Pen		pen (color, inThickness);
		if (inLineLen > 0 && inSpaceLen > 0)
		{
			float	pattern [2];
			pattern[0] = inLineLen;
			pattern[1] = inSpaceLen;
			pen.SetDashPattern (pattern, 2);
		}
		float	half = inThickness / 2;
		if (left == right)
			mGraphics->DrawLine (&pen, left + half, top, right + half, bottom);
		else if (top == bottom)
			mGraphics->DrawLine (&pen, left, top + half, right, bottom + half);
		else
			mGraphics->DrawLine (&pen, left, top, right, bottom);
	}
}


// ---------------------------------------------------------------------------
// DrawRect															  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::DrawRect (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
							   bool inFill, SRGBColor inFillColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
		if (inFill)
		{
			Color		color = ToColor (inFillColor);
			SolidBrush	brush (color);
			mGraphics->FillRectangle (&brush, (float)inRect.left, (float)inRect.top, (float)inRect.Width(), (float)inRect.Height());
		}
		if (inFrame)
		{
			Color	color = ToColor (inFrameColor);
			Pen		pen (color, inThickness);
			if (inLineLen > 0 && inSpaceLen > 0)
			{
				float	pattern [2];
				pattern[0] = inLineLen;
				pattern[1] = inSpaceLen;
				pen.SetDashPattern (pattern, 2);
			}
			mGraphics->DrawRectangle (&pen, (float)(inRect.left + inThickness/2), (float)(inRect.top + inThickness/2), (float)(inRect.Width() - inThickness), (float)(inRect.Height() - inThickness));
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawOval															  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::DrawOval (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
								bool inFill, SRGBColor inFillColor, float inLineLen, float inSpaceLen)
{
	if (mPageIsOpen)
	{
		if (inFill)
		{
			Color		color = ToColor (inFillColor);
			SolidBrush	brush (color);
			mGraphics->FillEllipse (&brush, (float)inRect.left, (float)(float)inRect.top, inRect.Width(), (float)inRect.Height());
		}
		if (inFrame)
		{
			Color	color = ToColor (inFrameColor);
			Pen		pen (color, inThickness);
			if (inLineLen > 0 && inSpaceLen > 0)
			{
				float	pattern [2];
				pattern[0] = inLineLen;
				pattern[1] = inSpaceLen;
				pen.SetDashPattern (pattern, 2);
			}
			mGraphics->DrawEllipse (&pen, (float)(inRect.left + inThickness/2), (float)(inRect.top + inThickness/2), (float)(inRect.Width() - inThickness/2), (float)(inRect.Height() - inThickness/2));
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// GetPictBounds													  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::GetPictBounds (SRect &ioRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, bool inGrow)
{
	RWWinPictData	*pd = NULL;
	if (*cd == NULL)
	{
		pd = new RWWinPictData;
		*cd = pd;


		pd->fConvertedPict.Attach (inPicture);
		if (pd->fConvertedPict.GetKind() >= RWValue::eValue_BLOB && pd->fConvertedPict.GetKind() <= RWValue::eValue_PictureEMF)
		{
			RW_ConvertPictureForPrinting (pd->fConvertedPict, false);
		}


/*		pd->fImage.Attach (inPicture);
		if (pd->fImage.GetKind() >= RWValue::eValue_BLOB && pd->fImage.GetKind() <= RWValue::eValue_PictureEMF)
		{
			RW_ConvertPictureForPrinting (pd->fImage, false);
		}
*/

		switch (pd->fConvertedPict.GetKind())
		{
			case RWValue::eValue_PictRefScreen:
			{
				void *sp = pd->fConvertedPict.GetPictureRef();
				if (sp)
				{
					pd->fImage = reinterpret_cast <RWScreenPict> (sp);
					pd->fWidth = pd->fImage->GetWidth();
					pd->fHeight = pd->fImage->GetHeight();
				}
				break;
			}
			case RWValue::eValue_PictRefPrint:
			{
				void *pp = pd->fConvertedPict.GetPictureRef();
				if (pp)
				{
					pd->fImage = reinterpret_cast <RWPrintPict> (pp);
					pd->fWidth = pd->fImage->GetWidth();
					pd->fHeight = pd->fImage->GetHeight();
				}
				break;
			}

#if	0
			case RWValue::eValue_BLOB:
			case RWValue::eValue_PicturePICT:
			case RWValue::eValue_PicturePDF:
			case RWValue::eValue_PictureJPG:
			case RWValue::eValue_PicturePNG:
			case RWValue::eValue_PictureTIFF:
			case RWValue::eValue_PictureEMF:
				break;
#endif

			default:	// to shut up compiler
				break;
		}
	}
	else
		pd = static_cast <RWWinPictData*> (*cd);

	if (inGrow)	// can shrink/expand
	{
		//mbs 29062011	support vertical grow!
		if (ioRect.top == ioRect.bottom)
			ioRect.bottom = ioRect.top + pd->fHeight;
		
//		if (pd->fWidth > ioRect.Width())	// but can't expand horizontally
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
					// scale
					float	scalingFactor;
#if	0	//mbs 06082010
					if ((pd->fWidth - ioRect.Width()) > (pd->fHeight - ioRect.Height()))
						scalingFactor = ioRect.Width() / pd->fWidth;
					else
						scalingFactor = ioRect.Height() / pd->fHeight;
#else
					scalingFactor = ioRect.Width() / pd->fWidth;
					if (scalingFactor > ioRect.Height() / pd->fHeight)
						scalingFactor = ioRect.Height() / pd->fHeight;
#endif

					if (scalingFactor > 1.0)	// don't enlarge a pict
						scalingFactor = 1.0;

					ioRect.bottom = ioRect.top + pd->fHeight * scalingFactor;
					ioRect.right = ioRect.left + pd->fWidth * scalingFactor;
					break;
				}
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
RWWinPageComposer::DrawPict (SRect &inRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, double inRotation, float alfa)
{
	if (mPageIsOpen && *cd == NULL)
	{
		SRect	r (inRect);
		GetPictBounds (r, inPicture, inSizing, cd, false);
	}

	if (mPageIsOpen && *cd)
	{
		RWWinPictData	*pd = static_cast <RWWinPictData*> (*cd);
		if (pd->fImage != NULL)
		{
			float		h, v;
			float		scalingFactor;
            RWContextInfoRef	gc = 0;

            if (inRotation != 0)
            {
                CGAffineTransform	t = RWTools::MakeMatrixFromUserRect (inRect, GetNativeRotation (inRotation), 0, 0., 0.);	//mbs 29062011
                SaveContext (gc);
                ApplyTransform (t);
                
            }
            RectF		pictRect (inRect.left, inRect.top, pd->fWidth, pd->fHeight);
            
            ColorMatrix colorMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1 - alfa, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
            // Create an ImageAttributes object and set its color matrix.
            ImageAttributes imageAtt;
            imageAtt.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault,
                                    ColorAdjustTypeBitmap);

			switch (inSizing)
			{
				case ePictFormat_Centered:				// Truncated (centered)
					if (inRect.Width() >= pd->fWidth)
					{
						pictRect.X += (inRect.Width() - pd->fWidth) / 2;
						h = 0;
					}
					else
					{
						pictRect.Width = inRect.Width();
						h = (pd->fWidth - inRect.Width()) / 2;
					}
					if (inRect.Height() >= pd->fHeight)
					{
						pictRect.Y += (inRect.Height() - pd->fHeight) / 2;
						v = 0;
					}
					else
					{
						pictRect.Height = inRect.Height();
						v = (pd->fHeight - inRect.Height()) / 2;
					}
					mGraphics->DrawImage (pd->fImage, pictRect, h, v, pictRect.Width, pictRect.Height, UnitPixel, &imageAtt);	//mbs 20072011
					break;

				case ePictFormat_ScaledToFit:			// Scaled to fit
					pictRect.Width = inRect.Width();
					pictRect.Height = inRect.Height();
					mGraphics->DrawImage (pd->fImage, pictRect,  0., 0., pd->fWidth, pd->fHeight, UnitPixel, &imageAtt);
					break;

				case ePictFormat_ScaledProp:			// Scaled to fit (proportional)
				case ePictFormat_ScaledPropCentered:	// Scaled to fit centered (prop.)
					// scale
#if	0	//mbs 06082010
					if (pd->fWidth - inRect.Width() > pd->fHeight - inRect.Height())
						scalingFactor = inRect.Width() / pd->fWidth;
					else
						scalingFactor = inRect.Height() / pd->fHeight;
#else
					scalingFactor = inRect.Width() / pd->fWidth;
					if (scalingFactor > inRect.Height() / pd->fHeight)
						scalingFactor = inRect.Height() / pd->fHeight;
#endif

					if (scalingFactor > 1.0)	// don't enlarge a pict
						scalingFactor = 1.0;

					pictRect.Width *= scalingFactor;
					pictRect.Height *= scalingFactor;

					if (inSizing == ePictFormat_ScaledPropCentered)
					{
						// center
						h = (inRect.Width() - pictRect.Width) / 2;
						v = (inRect.Height() - pictRect.Height) / 2;
						pictRect.X += h;
						pictRect.Y += v;
					}
					mGraphics->DrawImage (pd->fImage, pictRect, 0., 0., pd->fWidth, pd->fHeight,UnitPixel, &imageAtt);
					break;

				case ePictFormat_Normal:				// Truncated (non-centered)
				default:
					if (inRect.Width() >= pd->fWidth)
						h = pd->fWidth;
					else
						h = inRect.Width();
					if (inRect.Height() >= pd->fHeight)
						v = pd->fHeight;
					else
						v = inRect.Height();
					mGraphics->DrawImage (pd->fImage, pictRect, 0., 0., h, v, UnitPixel, &imageAtt);	//mbs 20072011
					break;
			} //switch
            if (gc)
                RestoreContext (gc);

		}
	}

	return;
}


/*
// ---------------------------------------------------------------------------
// FreePict															  [public]
// ---------------------------------------------------------------------------

void
RWWinPageComposer::FreePict (RWPictData **cd)
{
	if (*cd)
	{
		RWWinPictData	*pd = static_cast <RWWinPictData*> (*cd);
		if (pd->fImage)
			delete pd->fImage;
		delete pd;
		*cd = 0;
	}
	return;
}
*/


void
RWWinPageComposer::DrawTextBox (RWString inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText)
{
if (sUseTF)
	RWPageComposer::DrawTextBox (inText, inStyle, inRect, inWrap, inAttributed, inFit, ioPrintText);
else
{
	SRect	r (inRect);
	if (ioPrintText != NULL && *ioPrintText != NULL)
		inText = (*ioPrintText)->GetText();

	if (mPageIsOpen)
	{
		if (inStyle->GetBackColor().alpha != 0)	// (inStyle->GetBackColor() != cWhiteColor)
			DrawRect (inRect, 0, false, cWhiteColor, true, inStyle->GetBackColor());

		try
		{
			if (!inText.empty())
			{
				if (ioPrintText == NULL)	// RWTable support
				{
					RWWinPrintText	txt (*this, inText, inStyle, inWrap, inAttributed, inFit);
					txt.Draw (*this, r, inFit, true, false);
					r = inRect;
					txt.Draw (*this, r, inFit, false, true);
				}
				else
				{
					if (*ioPrintText == NULL)
					{
						*ioPrintText = new RWWinPrintText (*this, inText, inStyle, inWrap, inAttributed, inFit);
						static_cast <RWWinPrintText*> (*ioPrintText)->Draw (*this, r, inFit, true, false);
						r = inRect;
					}
					static_cast <RWWinPrintText*> (*ioPrintText)->Draw (*this, r, inFit, false, true);
				}
			}
		}
		catch (...)
		{
			throw;
		}
	}
	else if (ioPrintText != NULL && *ioPrintText != NULL)	// nothing to do if we don't draw and ioPrintText is empty
	{
		if (!inText.empty())
			static_cast <RWWinPrintText*> (*ioPrintText)->Draw (*this, r, inFit, false, false);
	}
}
}


double
RWWinPageComposer::MeasureText (const RWString inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText)
{
if (sUseTF)
	return RWPageComposer::MeasureText (inText, inStyle, ioRect, inWrap, inAttributed, inFit, ioPrintText);
else
{
	if (!inText.empty())
	{
		if (ioPrintText == NULL)	// RWTable support
		{
			RWWinPrintText	txt (*this, inText, inStyle, inWrap, inAttributed, inFit);
			txt.Draw (*this, ioRect, inFit, true, false);
//			txt.GetBounds (*this, ioRect, inFit, false);
		}
		else
		{
			if (*ioPrintText == NULL)
			{
				*ioPrintText = new RWWinPrintText (*this, inText, inStyle, inWrap, inAttributed, inFit);
				static_cast <RWWinPrintText*> (*ioPrintText)->Draw (*this, ioRect, inFit, true, false);
			}
			else
				static_cast <RWWinPrintText*> (*ioPrintText)->GetBounds (*this, ioRect, inFit);
		}
	}
	else
	{
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
	}

	return ioRect.Width();
}
}



RWWinPrintText::RWWinPrintText (RWWinPageComposer &inComposer, const RWString inText, RWStyle *inStyle, bool inWrap, bool inAttributed, bool inFit)
	:	RWPrintText (inText, inStyle, inAttributed),
		mTextLength (0),
		mCharsPrinted (0),
		mWrap (inWrap)
{
	if (not mText.empty())
	{
		mTextLength = long (mText.size());
		switch (mStyle->GetJustification())
		{
			case RWStyle::st_default:		break;
			case RWStyle::st_left:			mStringFormat.SetAlignment(StringAlignmentNear); break;
			case RWStyle::st_right:			mStringFormat.SetAlignment(StringAlignmentFar); break;
			case RWStyle::st_center:		mStringFormat.SetAlignment(StringAlignmentCenter); break;
			case RWStyle::st_justify:		break;
			case RWStyle::st_fulljustify:	break;
		}

		if (mStyle->GetVerticalJustification() == RWStyle::st_top)
			mStringFormat.SetLineAlignment (StringAlignmentNear);
		else if (mStyle->GetVerticalJustification() == RWStyle::st_bottom)
			mStringFormat.SetLineAlignment (StringAlignmentFar);
		else if (mStyle->GetVerticalJustification() == RWStyle::st_center)
			mStringFormat.SetLineAlignment (StringAlignmentCenter);

		INT	flags = 0;
		if (inFit)
			flags |= StringFormatFlagsLineLimit;
		if (not inWrap)
			flags |= StringFormatFlagsNoWrap;
		if (flags != 0)
			mStringFormat.SetFormatFlags (flags);
	}
	else
	{
		mWidth = 0;
		mNumLines = 0;
	}
}


RWWinPrintText::~RWWinPrintText (void)
{
	Free();
}


void
RWWinPrintText::Free (void)
{
	mText.clear();
	mTextLength = 0;

	return;
}


void
RWWinPrintText::Reset (void)
{
	mPrintedHeight = 0;
	mPrintedLines = 0;
	mCharsPrinted = 0;
}


void
RWWinPrintText::GetBounds (RWWinPageComposer &inComposer, SRect &ioRect, bool inFit)
{
	if (mNumLines > 0)
	{
		ioRect.bottom = ioRect.top + mHeight;	// - mPrintedHeight;
		ioRect.right = ioRect.left + mWidth;
		if (mStyle->GetRotation() != 0)
		{
			SRect	r (ioRect);
			RWTools::MakeUserRectFromText (r, mStyle->GetRotation());
			ioRect.bottom = ioRect.top + r.Height();
			ioRect.right = ioRect.left + r.Width();
		}
	}
	else
	{
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
	}

	return;
}


int
RWWinPrintText::Draw (RWWinPageComposer &inComposer, SRect &ioRect, bool inFit, bool inMeasure, bool inDoDraw)
{
	if (inMeasure || mNumLines > mPrintedLines)
	{
		Graphics			*g = inComposer.GetGDI();
		SPoint				origTL (ioRect.TopLeft());
//		GraphicsContainer	gc = 0;
		RWContextInfoRef	gc = 0;

		bool		measureWidth = (ioRect.Width() == 0);
# define	kMeasureWidth		4000

		if (measureWidth && not inDoDraw)
			mWidth = 0;
		if (measureWidth)
			ioRect.right = ioRect.left + kMeasureWidth;

		if (mStyle->GetRotation() != 0)
		{
			if (measureWidth)
				ioRect.bottom = ioRect.top + kMeasureWidth;
			CGAffineTransform	t = RWTools::MakeMatrixFromUserRect (ioRect, mStyle->GetRotation(), 0, mWidth, mHeight);
			if (inDoDraw)
			{
#if	0 && TARGET_DEBUG
				const SRGBColor	color	( 65535, 0, 65535, 65535 );
				inComposer.DrawRect (ioRect, 0.25, true, color, false, cWhiteColor, 1, 2);
#endif
//				Matrix	world;
//				g->GetTransform (&world);	// there could be a transform to honor page origin
//				REAL	elements[6];
//				world.GetElements (elements);
				Matrix	matrix (t.a, t.b, t.c, t.d, t.tx, t.ty);
//				world.Multiply (&matrix, MatrixOrderAppend);
				inComposer.SaveContext (gc);
				g->SetTransform (&matrix);
#if	0 && TARGET_DEBUG
				if (inDoDraw)
					inComposer.DrawRect (ioRect, 0.25, true, cBlueColor, false, cWhiteColor, 1, 2);
#endif
			}
		}

		if (not inDoDraw && not inFit)
			ioRect.bottom = ioRect.top + 2e10;

		RectF	r (ioRect.left, ioRect.top, ioRect.Width(), ioRect.Height());
		RectF	bbox;
		INT		chars = 0;
		INT		lines = 0;

		if (inDoDraw)
		{
			Color			color = ToColor (mStyle->GetTextColor());
			SolidBrush		solidBrush (color);
			const std::wstring	text = RWStr::ToWide (RWStringView (mText).substr (size_t (mCharsPrinted)));
			g->DrawString (text.c_str(), INT (text.size()), inComposer.MapStyle (mStyle), r, &mStringFormat, &solidBrush);
		}

		const std::wstring	remaining = RWStr::ToWide (RWStringView (mText).substr (size_t (mCharsPrinted)));
		g->MeasureString (remaining.c_str(), INT (remaining.size()), inComposer.MapStyle (mStyle), r, &mStringFormat, &bbox, &chars, &lines);
		if (inMeasure)
		{
			ioRect.SetRect (bbox.Y, bbox.X, bbox.Y + bbox.Height, bbox.X + bbox.Width);
			if (mNumLines == -1)
			{
				mNumLines = lines;
				mHeight = bbox.Height;	//mbs 28052010
				mWidth = bbox.Width;
			}
		}
		else	// if (not inMeasure)	//mbs 14052010	if (inDoDraw)
		{
			mCharsPrinted += chars;
			mPrintedLines += lines;
			mPrintedHeight += bbox.Height;
		}

		if (mStyle->GetRotation() != 0)
		{
			RWTools::MakeUserRectFromText (ioRect, mStyle->GetRotation());
			ioRect.bottom = origTL.v + ioRect.Height();
			ioRect.right = origTL.h + ioRect.Width();
			ioRect.top = origTL.v;
			ioRect.left = origTL.h;
		}
#if	0 && TARGET_DEBUG
		if (inDoDraw)
			inComposer.DrawRect (ioRect, 0.5, true, cRedColor, false, cWhiteColor, 0.5, 0.5);
#endif
		if (gc)
//			g->EndContainer (gc);
			inComposer.RestoreContext (gc);
		return mPrintedLines;
	}
	return 0;
}


bool	RWWinPageComposer::GetDPI (float &x, float &y, void* inWindow)
{
	if (mGraphics)
	{
		x = mGraphics->GetDpiX();
		y = mGraphics->GetDpiY();
		return true;
	}

	if (inWindow && IsWindow ((HWND) inWindow))
	{
		Graphics	gr ((HWND) inWindow);
		x = gr.GetDpiX();
		y = gr.GetDpiY();
		return true;
	}

	return false;
}
