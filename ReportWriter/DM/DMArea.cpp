/*
 *  DMArea.cpp
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 01.11.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o. All rights reserved.
 *
 */

# include	"DMArea.h"
# include	"SRPlugin.h"
# include	"PSObjProps.h"
# include	"RWString4D.h"
# include	"theVersion.h"

// using namespace    FourDAPIEx;

#if	MACVER
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
// HIToolbox key modifiers (cmdKey, ...), GetCurrentKeyModifiers, HIThemeDrawFocusRect - still in the 64 bit SDK
# include	<Carbon/Carbon.h>
#endif
extern	"C"		void Yield4D (void);

float	DMArea::sRoundUI = 10;
float	DMArea::sSnapUI = 2.5;
float	DMArea::sScrollUI = 10;

inline	float DMArea::RoundUI (float f)
{
	return round (f * sRoundUI) / sRoundUI;
}

# define	dragHighlite	SRGBColor(0x80EEEEEE)

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)


#if	WINVER

#if	kUSE_FAKE_AREA
bool	DMArea::sClassRegistered	= false;
#define	kDMAreaClassName	L"RW_Area"
#endif

extern	HINSTANCE	gMyInstance;
//# include	<Events.h>
# include	<OLE2.h>
# include	<gdiplus.h>
# include 	<Mmsystem.h>
#endif


# define	kFocusInset	3	// for focus rect
#if	WINVER
# define	kScrollSize	16
#else
# define	kScrollSize	16
#endif

static	float	sFocusInset = kFocusInset;
static	float	sScrollSize = kScrollSize;


const DMBase::UserProps	DMArea::sUserProperties[] = {
{ PSObjPropKind,			false	},
{ PSObjPropID,				true	},
//{ PSObjPropVisible,			true	},	// REMOVE!!!
//{ PSObjPropLocked,			true	},	// REMOVE???
//{ PSObjPropSelected,		true	},
{ PSObjPropVersion,			false	},
{ PSObjPropName,			true	},
{ PSObjPropDynamic,			true	},
//{ PSObjPropDrawingRect,		false	},	// REMOVE!!!
//{ PSObjPropRect,			true	},
{ PSObjPropWidth,			true	},
{ PSObjPropHeight,			true	},
{ PSObjPropPaper,			true	},
{ PSObjPropMargins,			true	},
	
{ PSObjPropObjectRotation,	true	},
{ PSObjPropMirror,			true	},


//{ PSObjPropPageSetup,		true	},
{ PSObjPropPageFormat,		true	},
{ PSObjPropPrintSettings,	true	},
{ PSObjPropDevMode,			true	},
{ PSObjPropDeviceNames,		true	},
{ PSObjPropPageSetupDlg,	true	},
{ PSObjPropPrintDlg,		true	},

{ PSObjPropShowMargins,		true	},
{ PSObjPropShowRuler,		true	},
{ PSObjPropRulerUnits,		true	},
{ PSObjPropGridSize,		true	},
{ PSObjPropShowGrid,		true	},
{ PSObjPropSnapToGrid,		true	},
{ PSObjPropShowGuides,		true	},
{ PSObjPropLockGuides,		true	},
{ PSObjPropSnapToGuides,	true	},
//{ PSObjPropShowSections,	true	},
//{ PSObjPropLockSections,	true	},
{ PSObjPropShowObjBorders,	true	},
{ PSObjPropScale,			true	},

//{ 'isEA',		false	},
//{ 'hite',		false	},
//{ 'hito',		false	},
//{ 'asel',		false	},
{ 'drmo',		true	},
{ 'scrl',		true	},
{ 'scrt',		true	},
{ 'tool',		true	},
//{ '4Der',		true	},	// REMOVE!!!
//{ 'dpiX',		false	},	// REMOVE!!!
//{ 'dpiY',		false	},	// REMOVE!!!
{ 'rund',		true	},	// REMOVE?
{ 'snap',		true	},	// REMOVE?
{ 'scrw',		true	},	// REMOVE?
{ 0, 						false	}
};

long	DMArea::GetUserProperties (const UserProps* &outProps) const
{
    outProps = sUserProperties;
    return (sizeof (sUserProperties) / sizeof (sUserProperties[0])) - 1;
}

static Boolean MouseDown (void)
{
#if    TARGET_API_MAC_CARBON
    NSUInteger buttons = [NSEvent pressedMouseButtons];
    if (buttons & 0x0001) return true;
#else
    SHORT  ks = GetAsyncKeyState(VK_LBUTTON);
    if (ks & 0x8000) return true;
#endif
    return false;
}

static UInt32 GetModifiers (void)
{
#if	TARGET_API_MAC_CARBON
	return ::GetCurrentKeyModifiers();
#elif	USE_MAC_API
    return GetEventModifiers(void);
#else
	SHORT	ks = GetAsyncKeyState (VK_SHIFT);
	UInt32	modifiers = 0;
	if (ks & 0x8000)
		modifiers |= shiftKey;
	ks = GetAsyncKeyState (VK_CONTROL);
	if (ks & 0x8000)
		modifiers |= cmdKey;
	ks = GetAsyncKeyState (VK_MENU);
	if (ks & 0x8000)
		modifiers |= optionKey;
	ks = GetAsyncKeyState (VK_ESCAPE);
	if (ks & 0x8000)
		modifiers |= activeFlag;
   
	return modifiers;
#endif
}

// ---------------------------------------------------------------------------
// DMArea									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

DMArea::DMArea (void)
	:	DMReport (NULL),
		mScreen (0),
#if	kUSE_FAKE_AREA
		mArea (0),
#endif
		mNoHitTest (0, 0, 0, 0),
		mScrollH (0),
		mScrollV (0),
		mScrollPos (0, 0),
		mScrollPosScaled (0, 0),
		mDrawMode (eDraw_Normal),
		mAreaSelected (false),
		mDirtyIdle (false),
		mEventLevel (1),
		mCurParams (0),
		mMenuEvent (eAE_Idle),
		mLastEventHit (eHit_None),
		mLastObjectHit (0),
		mLastObjectCreated (0),
		mLastEventTime (0),
		mLastEventKey (0),
		mCurCharPos (0),
		mDoubleClick (false),
		mRulerAbsolute (true),
        mInterfaceEvent(eAE_Idle),
        mRequestUpdate(false)
{
	mAreaProperties.fVersion = -1;
	mLastEventChar [0] = 0;
	mToolI = eTool_Select;
    mTrackSelect.SetRect (0, 0, 0, 0);
//	ScaleChanged();

#if	kUSE_FAKE_AREA
	if (not sClassRegistered)
	{
		WNDCLASSW	wndclass;
		wndclass.style = CS_NOCLOSE;
		wndclass.lpfnWndProc = (WNDPROC) AreaWndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = gMyInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = NULL; //LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH) ::GetStockObject (NULL_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = kDMAreaClassName;
		sClassRegistered = ::RegisterClassW (&wndclass) ? true: false;
		sClassRegistered = true;	//mbs 30062011	RegisterClass will fail when re-opening DB
	}
#endif
	return;
}


// ---------------------------------------------------------------------------
// ~DMArea									Destructor				  [public]
// ---------------------------------------------------------------------------

DMArea::~DMArea (void)
{
// ••• TODO •••	Report should be destroyed first! (pictures)
//	DMReport::~DMReport();
//	delete mScreen;

	delete mScrollH;
	delete mScrollV;

#if	kUSE_FAKE_AREA
	if (mArea)
		::DestroyWindow (mArea);
#endif

	return;
}

// ---------------------------------------------------------------------------
// HandleEvent												 [static] [public]
// ---------------------------------------------------------------------------

void
DMArea::HandleEvent (PA_PluginParameters params)
{
	DMArea	*area = NULL;
	PA_AreaEvent	event = PA_GetAreaEvent (params);
	switch (event)
	{
		case eAE_DesignInit:
		case eAE_GetMenuIcon:
			PA_DontTakeEvent (params);
			break;
		case eAE_DesignUpdate:
			DrawDesign (params);	//mbs 30062011
			break;

		case eAE_AreAdvancedPropertiesEditable:
			PA_SetAdvancedPropertiesEditable (params, false);
			break;

		case eAE_InitArea:
		case eAE_InitAdvancedProperties:
		{
			area = new DMArea;
			PA_SetAreaReference (params, (void *)area->mInternalID);
			PA_Unistring	*n = PA_GetAreaName (params);
			area->mAreaName = RWStr::FromPA (n);
			PA_GetPluginProperties (params, &area->mAreaProperties);

			area->mCurParams = params;
			PA_Rect			r = PA_GetAreaRect (params);
			area->mAreaFullRect =
			area->mNewAreaRect = SRect (r.fTop, r.fLeft, r.fBottom, r.fRight);
			area->mCurEvent = event;
			area->mLastEventModifiers = GetEventModifiers();
			area->HandleEvent();
			area->mCurParams = 0;
			area->mEventLevel--;
			break;
		}

		default:
		{
			DMReport	*rep = DMReport::GetReportObject ((long) PA_GetAreaReference (params));
			if (rep == NULL)
				throw (long) errInvalidReportRef;
			area = dynamic_cast <DMArea*> (rep);
			if (area == NULL)
				throw (long) errInvalidReportRef;

			PA_PluginParameters	savedParams = area->mCurParams;	//mbs 05032010	reentrancy in composited mode
			PA_AreaEvent		savedEvent = area->mCurEvent;
			area->mEventLevel++;
			area->mCurParams = params;
			PA_Rect			r = PA_GetAreaRect (params);
			area->mNewAreaRect = SRect (r.fTop, r.fLeft, r.fBottom, r.fRight);
			area->mCurEvent = event;
			area->mLastEventModifiers = GetEventModifiers();
			area->HandleEvent();
			area->mCurParams = savedParams;
			if (--area->mEventLevel > 0)	//mbs 04042010	otherwise it will be always 602...
				area->mCurEvent = savedEvent;
			if (event == eAE_Deinit)
			{
				RWPageComposer	*screen = area->GetPageComposer();
				delete area;
				delete screen;
				PA_SetAreaReference (params, 0);
			}
			break;
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawDesign												  [static][public]
// ---------------------------------------------------------------------------

void
DMArea::DrawDesign (PA_PluginParameters params)
{
	
	/*
	std::unique_ptr	<RWNativePageComposer> screen (RWPageComposer::CreateScreenComposer());
#if	MACVER
	CGrafPtr		port;
	Rect			portRect;
	::GetPort (&port);
	::GetPortBounds (port, &portRect);
	CGContextRef	cg = 0;
	if (not (*screen).IsQD())
	{
		::QDBeginCGContext (port, &cg);
		if (cg == 0)	//mbs!!!
		{
			PA_DontTakeEvent (params);
			return;
		}
	}
	SRect			window (portRect);
	(*screen).SetPageRect (window);
	if ((*screen).IsQD())
		(*screen).SetContext (port);
	else
		(*screen).SetContext (cg);

	RgnHandle		rgn = NewRgn();
	GetClip (rgn);
	ClipCGContextToRegion (cg, &portRect, rgn);	//mbs 22062011	mc4
	DisposeRgn (rgn);
#else
	HDC				dc = (HDC) PA_GetUpdateHDC();
	(*screen).SetContext (dc);
#endif
*/
	

	std::unique_ptr <RWNativePageComposer> screen (RWPageComposer::CreateScreenComposer());
#if	MACVER
    CGContextRef	cg = 0;
    
	// does not work if fVersionSupported is not set to 0x1400 during initialization!
    PA_PluginProperties	props;
    PA_GetPluginProperties (params, &props);
    cg = (CGContextRef) props.fMacPort;
    PA_Rect	portBounds = PA_GetAreaPortBounds (params);
    CGContextScaleCTM (cg, 1.0, -1.0);
    CGContextTranslateCTM (cg, 0, -(portBounds.fBottom - portBounds.fTop));
#else
	HDC     dc = (HDC) PA_GetHDC (params);
	(*screen).SetContext (dc);
#endif
	
	RWStyle			style (NULL, RWXmlNode());
	{
		RWValue	size (12.0);
		style.SetProperty (PSObjPropSize, size);
	}
	RWString		u = RWStr::FromPA (PA_GetAreaName (params));
	char			buf [32];
	PA_Rect			ar = PA_GetAreaRect (params);
	snprintf (buf, sizeof (buf), " w: %d h: %d\rReportWriter v", ar.fRight - ar.fLeft, ar.fBottom - ar.fTop);
	u += RWStr::FromASCII (buf);
	u += RWStr::FromASCII (kVersionString);
#ifdef	TARGET_STR
	u += u' ';
	u += RWStr::FromASCII (TARGET_STR);
#endif
	u += u'\r';
	u += RWStr::FromUTF8 (QUOTEME (kProductCopyright));

	SRect			r (ar.fTop, ar.fLeft, ar.fBottom, ar.fRight);
	(*screen).DrawRect (r, 1.0, true, cBlackColor, true, cWhiteColor);
	r *= 2;
	(*screen).DrawTextBox (u, &style, r, true, false, false, NULL);
	(*screen).StyleChanged (&style);
	(*screen).SetContext (NULL);
	PA_CustomizeDesignMode (params);
}


// ---------------------------------------------------------------------------
// SetReport														  [public]
// ---------------------------------------------------------------------------

void
DMArea::SetReport (RWXmlDocument *inXML)
{
	mScreen->SetContext (NULL); // v 1.2.4
	mUndoBuffer.Clear();
	DMReport::SetReport (inXML);
	mScreen->StyleChanged (NULL);	// clear cache
	ScaleChanged();
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMArea::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case 'isEA':				outValue.SetBoolean (true); break;
		case 'evtT':				if (mInterfaceEvent == eAE_RedoCommand)
										outValue.SetInteger (31); 
									else if (mInterfaceEvent >= eAE_UndoCommand)
											 outValue.SetInteger (mInterfaceEvent - 13);
									else 
										outValue.SetInteger (mInterfaceEvent);
									break;
		case 'evtH':				outValue.SetReal (mLastEventPos.h); break;
		case 'evtV':				outValue.SetReal (mLastEventPos.v); break;
		case 'evtM':				outValue.SetInteger (mLastEventModifiers); break;
		case 'evtK':				outValue.SetInteger (mLastEventKey); break;
		case 'evtC':				outValue.SetText (RWStr::FromPA (mLastEventChar)); break;
		case 'evtD':				outValue.SetBoolean (mDoubleClick); break;
		case 'hite':				outValue.SetInteger (mLastEventHit); break;
        case 'hito':				outValue.SetInteger (IsValidObject (mLastObjectHit) ? mLastObjectHit->GetInternalID() : 0); break;
        case 'creo':				outValue.SetInteger (IsValidObject(mLastObjectCreated) ? mLastObjectCreated->GetInternalID() : 0); break;
		case 'asel':				outValue.SetBoolean (mAreaSelected); break;

		case 'drmo':				outValue.SetInteger (mDrawMode); break;
		case 'scrl':				outValue.SetReal (mScrollPos.h); break;
		case 'scrt':				outValue.SetReal (mScrollPos.v); break;
		case 'tool':				outValue.SetInteger (mToolI); break;

		case '4Der':				outValue.SetText (mNoHitTest.ToString()); break;

		case 'dpiX':
		case 'dpiY':
		{
			float	dpiX = 0., dpiY = 0.;
#if	WINVER
			if (not mScreen->GetDPI (dpiX, dpiY, mAreaProperties.fWinHWND))
#else
			if (not mScreen->GetDPI (dpiX, dpiY, mAreaProperties.fMacWindow))
#endif
				return false;
			if (id == 'dpiX')
				outValue.SetReal (dpiX);
			else
				outValue.SetReal (dpiY);
			break;
		}

		case 'rund':				outValue.SetReal (sRoundUI); break;
		case 'snap':				outValue.SetReal (sSnapUI); break;
		case 'scrw':				outValue.SetReal (sScrollUI); break;
		case 'drpX':				outValue.SetReal(mLastDragPos.h); break;
		case 'drpY':				outValue.SetReal(mLastDragPos.v); break;
        case 'drgo':				outValue.SetInteger ((mLastDragObject == NULL) ? 0 : mLastDragObject->GetInternalID()); break;

		default:					return DMReport::GetProperty (id, outValue);
	}
	
	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
DMArea::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropVisible:
		{
			bool	oldVisible = mVisible;
			if (DMReport::SetProperty (id, inValue))
			{
				if (oldVisible != mVisible)
				{
					if (not mVisible)
					{
						if (mScrollH)
							mScrollH->SetVisible (false);
						if (mScrollV)
							mScrollV->SetVisible (false);
#if	kUSE_FAKE_AREA
						if (mArea)
							::ShowWindow (mArea, SW_HIDE);
#endif
					}
				}
				return true;
			}
			break;
		}

		case PSObjPropWidth:
			if (DMReport::SetProperty (id, inValue))
			{
				CalculatePosition();
				return true;
			}
			break;

		case PSObjPropHeight:
			if (DMReport::SetProperty (id, inValue))
			{
				CalculatePosition();
				return true;
			}
			break;

		case PSObjPropPaper:
			if (DMReport::SetProperty (id, inValue))
			{
				CalculatePosition();
				return true;
			}
			break;

		case PSObjPropMargins:
			if (DMReport::SetProperty (id, inValue))
			{
				CalculatePosition();
				return true;
			}
			break;

		case PSObjPropShowRuler:
			if (DMReport::SetProperty (id, inValue))
			{
				CalcRectangles();
				ScaleChanged();
				return true;
			}
			break;

		case PSObjPropRulerUnits:
			if (DMReport::SetProperty (id, inValue))
			{
				mDirty = true;
				return true;
			}
			break;

		case 'creo':
			mLastObjectCreated = NULL;
			return true;

		case 'drmo':
		{
			long	lVal;
			if (SetIntegerProperty (inValue, lVal, eDraw_Normal, eDraw_Last - 1))
			{
				if (mDrawMode != EDrawDM (lVal))
				{
					if (mDataSource != NULL)
						mDataSource->ClearDataSource();
					mDrawMode = EDrawDM (lVal);
					if (mDrawMode == eDraw_Resource)
						DMReport::ParseData (mScreen);
					mDirty = true;
				}
				return true;
			}
			break;
		}
		case 'scrl':
		case 'scrt':
		{
			double	dVal = 0;
			SetRealProperty (inValue, dVal, 0, 4096);
			if (id == 'scrl')
			{
				if (mScrollH)
				{
					mScrollH->SetValue (dVal);
					mScrollPos.h = mScrollH->GetValue();
				}
				else
					mScrollPos.h = dVal;
			}
			else
			{
				if (mScrollV)
				{
					mScrollV->SetValue (dVal);
					mScrollPos.v = mScrollV->GetValue();
				}
				else
					mScrollPos.v = dVal;
			}
			ScaleChanged();
			return true;
		}
		case 'tool':				return SetIntegerProperty (inValue, mToolI, eTool_Select, eTool_last - 1);

		case '4Der':				return SetRectProperty (inValue, mNoHitTest);

		case 'rund':				return SetRealProperty (inValue, sRoundUI, 0.1, 10000);
		case 'snap':				return SetRealProperty (inValue, sSnapUI, 0.05, 32);
		case 'scrw':				return SetRealProperty (inValue, sScrollUI, 1, 1000);

		default:					return DMReport::SetProperty (id, inValue);
	}
	
	return false;
}


// ---------------------------------------------------------------------------
// MapToArea														  [public]
// ---------------------------------------------------------------------------

SPoint
DMArea::MapToArea (const SPoint inPt)
const
{
#if	MACVER
	CGPoint	p = CGPointMake (inPt.h, mPortRect.bottom - inPt.v);
	p = CGPointApplyAffineTransform (p, mScaleClick);
	return SPoint (p.x, mPortRect.bottom - p.y);
#else
	CGPoint	p = CGPointMake (inPt.h, inPt.v);
	p = CGPointApplyAffineTransform (p, mScaleClick);
	return SPoint (p.x, p.y);
#endif
}

SRect
DMArea::MapToArea (const SRect inRect)
const
{
	SRect	r;
	SPoint	p = MapToArea (inRect.TopLeft());
	r.top = p.v;
	r.left = p.h;
	p = MapToArea (inRect.BottomRight());
	r.bottom = p.v;
	r.right = p.h;
	return r;
}


// ---------------------------------------------------------------------------
// MapFromArea														  [public]
// ---------------------------------------------------------------------------

SPoint
DMArea::MapFromArea (const SPoint inPt)
const
{
#if	MACVER
	CGPoint	p = CGPointMake (inPt.h, mPortRect.bottom - inPt.v);
	p = CGPointApplyAffineTransform (p, mScaleDraw);
	return SPoint (p.x, mPortRect.bottom - p.y);
#else
	CGPoint	p = CGPointMake (inPt.h, inPt.v);
	p = CGPointApplyAffineTransform (p, mScaleDraw);
	return SPoint (p.x, p.y);
#endif
}

SRect
DMArea::MapFromArea (const SRect inRect)
const
{
	SRect	r;
	SPoint	p = MapFromArea (inRect.TopLeft());
	r.top = p.v;
	r.left = p.h;
	p = MapFromArea (inRect.BottomRight());
	r.bottom = p.v;
	r.right = p.h;
	return r;
}

SRect
DMArea::MapFromAreaRotated (const DMBase *inObject, const SRect inRect)
const
{
	SRect	r (inRect);
	DMStyle	*s;
    float rotation = 0;
	switch (inObject->GetKind())
	{
		case eObject_Text:		s = GetStyle (static_cast <const DMText*> (inObject)->GetStyleID()); break;
		case eObject_Var:		s = GetStyle (static_cast <const DMVariable*> (inObject)->GetStyleID()); break;
		case eObject_Fld:		s = GetStyle (static_cast <const DMField*> (inObject)->GetStyleID()); break;
		case eObject_TblHdr:	s = GetStyle (static_cast <const DMHeader*> (inObject)->GetStyleID()); break;
        case eObject_TblCol:	s = GetStyle (static_cast <const DMColumn*> (inObject)->GetStyleID()); break;
        case eObject_Pict:      s = NULL; rotation = (static_cast <const DMPict*> (inObject))->GetRotation(); break;
		default:				s = NULL; break;
	}
	if (s != NULL)
        rotation =s->GetRotation();
    if (rotation != 0)
#if	MACVER
		RWTools::MakeMatrixFromUserRect (r, rotation, mPortRect.bottom, 0, 0);
#else
		RWTools::MakeMatrixFromUserRect (r, rotation, 0, 0, 0);
#endif
	return MapFromArea (r);
}


// ---------------------------------------------------------------------------
// ScaleChanged													   [protected]
// ---------------------------------------------------------------------------

void
DMArea::ScaleChanged (void)
{
#if	MACVER
	mScaleDraw = CGAffineTransformMake (mScale, 0, 0, mScale, mEditorRect.left * (1 - mScale), (mPortRect.bottom - mEditorRect.top) * (1 - mScale));
	mScaleClick = CGAffineTransformMake (1/mScale, 0, 0, 1/mScale, mEditorRect.left * (1 - 1/mScale), (mPortRect.bottom - mEditorRect.top) * (1 - 1/mScale));
#else
	mScaleDraw = CGAffineTransformMake (mScale, 0, 0, mScale, mEditorRect.left * (1 - mScale), mEditorRect.top * (1 - mScale));
	mScaleClick = CGAffineTransformMake (1/mScale, 0, 0, 1/mScale, mEditorRect.left * (1 - 1/mScale), mEditorRect.top * (1 - 1/mScale));
#endif

	if (mScrollH)
	{
		long	lMax = max (0., mPosition.Width() * mScale - mEditorRect.Width());
		mScrollH->SetValues (mScrollPos.h, 0, lMax, 32, mEditorRect.Width());
		mScrollPos.h = mScrollH->GetValue();
	}
	if (mScrollV)
	{
		long	lMax = max (0., mPosition.Height() * mScale - mEditorRect.Height());
		mScrollV->SetValues (mScrollPos.v, 0, lMax, 32, mEditorRect.Height());
		mScrollPos.v = mScrollV->GetValue();
	}

	mScrollPosScaled.h = mScrollPos.h / -mScale;
	mScrollPosScaled.v = mScrollPos.v / -mScale;
	AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
	mDirty = true;
}


// ---------------------------------------------------------------------------
// CalculatePosition											   [protected]
// ---------------------------------------------------------------------------

void
DMArea::CalculatePosition (void)
{
	DMReport::CalculatePosition();
	CalcRectangles();
	ScaleChanged();
	return;
}


// ---------------------------------------------------------------------------
// CalcRectangles												   [protected]
// ---------------------------------------------------------------------------

void
DMArea::CalcRectangles (void)
{
	mToolbarRect.SetRect (mAreaFullRect.top + sFocusInset, mAreaFullRect.left + sFocusInset, mAreaFullRect.top + sFocusInset, mAreaFullRect.right - sFocusInset);	// empty for now
	mEditorRect.SetRect (mToolbarRect.bottom, mToolbarRect.left,
		mAreaFullRect.bottom - 16 - sScrollSize - sFocusInset,	// 16 = ruler height, kScrollSize = scrollbar height
		mToolbarRect.right - 32 - sScrollSize	// 32 = ruler width, kScrollSize = scrollbar width
	);
	mRulerRectH.SetRect (mEditorRect.bottom, mEditorRect.left,
						 mEditorRect.bottom + 16,
						 mEditorRect.right
						 );
	mRulerRectV.SetRect (mToolbarRect.bottom, mEditorRect.right,
						 mEditorRect.bottom,
						 mEditorRect.right + 32
						 );
	if (not mShowRuler)
	{
		mEditorRect.bottom += 16;
		mEditorRect.right += 32;
		mRulerRectH.top += 16;
		mRulerRectV.left += 32;
	}
	mScrollRectH.SetRect (mRulerRectH.bottom, mRulerRectH.left,
						 mRulerRectH.bottom + sScrollSize,
						 mToolbarRect.right - sScrollSize
						 );
	mScrollRectV.SetRect (mRulerRectV.top, mRulerRectV.right,
						  mScrollRectH.top,
						  mRulerRectV.right + sScrollSize
						  );
#if	kUSE_FAKE_AREA
	if (mArea)
	{
		UINT flags = SWP_NOACTIVATE | SWP_NOZORDER;	// | SWP_NOREDRAW;
		::SetWindowPos (mArea, NULL, mAreaFullRect.left, mAreaFullRect.top, mAreaFullRect.Width(), mAreaFullRect.Height(), flags);
		if (mScrollH)
			mScrollH->ParentPosChanged();
		if (mScrollV)
			mScrollV->ParentPosChanged();
	}
#endif

	if (mScrollH)
		mScrollH->Update (&mScrollRectH);
	if (mScrollV)
		mScrollV->Update (&mScrollRectV);
}


// ---------------------------------------------------------------------------
// HandleEvent													   [protected]
// ---------------------------------------------------------------------------

void
DMArea::HandleEvent (void)
{
	switch (mCurEvent)
	{
		case eAE_InitArea:
		case eAE_InitAdvancedProperties:
			mScreen = RWPageComposer::CreateScreenComposer();
			CalcRectangles();
			ScaleChanged();
#if	MACVER
			WindowAttributes 	attr;
			OSStatus			err;
//			err = ::GetWindowAttributes ((WindowRef) mAreaProperties.fMacWindow, &attr);
//			mComposite = ((attr & kWindowCompositingAttribute) != 0);
            mComposite = false; // HIWindowTestAttribute  ((WindowRef) mAreaProperties.fMacWindow, kWindowCompositingAttribute);
#else
#if	kUSE_FAKE_AREA
			if (sClassRegistered)
			{
				DWORD	exStyle = 0;	// WS_EX_TRANSPARENT
				mArea = ::CreateWindowExW (exStyle, kDMAreaClassName, L"", WS_CHILD | WS_CLIPCHILDREN,	//mbs 30062011	WS_CLIPCHILDREN
					mAreaFullRect.left, mAreaFullRect.top, mAreaFullRect.Width(), mAreaFullRect.Height(),
					(HWND) mAreaProperties.fWinHWND, NULL, gMyInstance, NULL);
				if (::IsWindow (mArea))
					::SetWindowLongPtrW (mArea, GWLP_USERDATA, (LONG_PTR) this);
				mScreen->SetHWNDContext (mArea);	//mbs 13052010
			}
#else
			RegisterProc();
			mScreen->SetContext ((void*) mAreaProperties.fWinHDC);	//mbs 13052010
#endif
#endif
			break;

		case eAE_Deinit:
#if	WINVER && !kUSE_FAKE_AREA
			UnRegisterProc();
#endif
			break;


		case eAE_IsFocusable:
#if	kUSE_CLIPMODE
			PA_SetPluginAreaClipMode ((PA_PluginRef) PA_GetAreaReference (mCurParams), 1);	//mbs 13052010 - the area reference (mInternalID), was "this"
#endif
			PA_SetAreaFocusable (mCurParams, true);
			break;

		case eAE_Select:
//			if (not mAreaSelected)
				mDirty = true;
			mAreaSelected = mVisible;
			PA_AcceptSelect (mCurParams, mAreaSelected);
			break;

		case eAE_Deselect:
//			if (mAreaSelected)
				mDirty = true;
			mAreaSelected = false;
			PA_AcceptDeselect (mCurParams, true);
			break;


		case eAE_Scroll:
			if (mAreaFullRect != mNewAreaRect)
			{
				mAreaFullRect = mNewAreaRect;
#if	MACVER
//				CGrafPtr	port;
//				::GetPort (&port);
//				::GetPortBounds (port, &mPortRect);
#endif
				CalcRectangles();
				ScaleChanged();
			}
			else
				PA_DontTakeEvent (mCurParams);
			break;

		case eAE_KeyDown:
		case eAE_AutoKey:
            mInterfaceEvent = mCurEvent;
			if (mVisible)
			{
				PA_KeyCode	keycode;
				if (PA_GetKey (mCurParams, mLastEventChar + mCurCharPos++, &keycode, NULL, NULL, NULL))
				{
					mLastEventKey = keycode;
					mLastEventChar [mCurCharPos] = 0;
					mCurCharPos = 0;
//					if ((mLastEventModifiers & cmdKey) == 0 || keycode == KEY_LEFT || keycode == KEY_RIGHT || keycode == KEY_UP || keycode == KEY_DOWN)
					if ((mLastEventModifiers & cmdKey) == 0 || (keycode < KEY_A || (keycode > KEY_Z && keycode <= KEY_DOWN)))
						PA_CallPluginAreaMethod (mCurParams);
					else
						PA_DontTakeEvent (mCurParams);
				}
			}
			else
				PA_DontTakeEvent (mCurParams);
			break;

		case eAE_MouseWheel:
			if (mVisible)
			{
//				bool	isHorizontal = (mLastEventModifiers & shiftKey) != 0;
//				short	delta = PA_GetMouseWheelIncrement (mCurParams);
				short	delta = PA_GetMouseWheelIncrement (mCurParams);
				mLastEventModifiers = GetModifiers();	//mbs 02082010	get modifiers (mLastEventModifiers is empty), adjust by sScrollUI
				const bool	isHorizontal = (mLastEventModifiers & shiftKey) != 0;	// was never set (read uninitialized)
				if ((mLastEventModifiers & (controlKey | cmdKey)) == 0)
					delta *= sScrollUI;
				if ((mLastEventModifiers & optionKey) != 0)
					delta *= 10;
				if (isHorizontal)
				{
					if (mScrollH) {
						mScrollH->SetValue (mScrollH->GetValue() - delta);
						mScrollPos.h = mScrollH->GetValue();
					} else {
						mScrollPos.h -= delta;
					}
				}
				else
				{
					if (mScrollV) {
						mScrollV->SetValue (mScrollV->GetValue() - delta);
						mScrollPos.v = mScrollV->GetValue();
					} else {
						mScrollPos.v -= delta;
					}

				}
				ScaleChanged();
				HandleUpdate (true);
			}
			else
				PA_DontTakeEvent (mCurParams);
			break;

        case eAE_Cursor:
        case eAE_MouseUp:
		case eAE_MouseDown:
            mInterfaceEvent = mCurEvent;
			if (mVisible)
				HandleMouse();
			else
				PA_DontTakeEvent (mCurParams);
			break;

		case eAE_Update:
			if (mVisible)
			{
				mDirty = false;
#if	TARGET_DEBUG
				if (mAreaFullRect != mNewAreaRect)
				{
					mAreaFullRect = mNewAreaRect;
					CalcRectangles();
					ScaleChanged();
				}
#endif
#if	kUSE_FAKE_AREA
				if (mArea)
					::ShowWindow (mArea, SW_SHOWDEFAULT);
#endif
				HandleUpdate (false);
//				if (mDirty)
//					AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
//				mDirtyIdle = true;
			}
			else
				PA_DontTakeEvent (mCurParams);
			break;

		case eAE_PageChange:
		{
            mInterfaceEvent = mCurEvent;
			short	from, to;
			PA_GetPageChange (mCurParams, &from, &to);

			if (mAreaProperties.fPage == from && mAreaProperties.fPage != 0)
			{
				if (mScrollH)
					mScrollH->SetVisible (false);
				if (mScrollV)
					mScrollV->SetVisible (false);
#if	kUSE_FAKE_AREA
				if (mArea)
					::ShowWindow (mArea, SW_HIDE);
#endif
			}
			else if (mAreaProperties.fPage == to || mAreaProperties.fPage == 0)
			{
				//	we have to adjust our size - user could resize a window while this area was hidden...
				if (mAreaFullRect != mNewAreaRect)
				{
					mAreaFullRect = mNewAreaRect;
#if	MACVER
//					CGrafPtr	port;
//					::GetPort (&port);
//					::GetPortBounds (port, &mPortRect);
#else
//#if	kUSE_FAKE_AREA
//					if (mArea)
//						::ShowWindow (mArea, SW_SHOWDEFAULT);
//#endif
#endif
					CalcRectangles();
					ScaleChanged();
//					if (mScrollH)
//						mScrollH->SetVisible (true);
//					if (mScrollV)
//						mScrollV->SetVisible (true);
				}
				mDirty = mVisible;
			}
			break;
		}

		case eAE_ShowHide:
		{
            mInterfaceEvent = mCurEvent;
			bool vis = PA_IsAreaVisible(mCurParams);
			if (mScrollH)
				mScrollH->SetVisible (vis);
			if (mScrollV)
				mScrollV->SetVisible (vis);

			break;
		}
		case eAE_Idle:
			if (mDirtyIdle)
			{
				if (mVisible)
				{
					if (mScrollH)
						mScrollH->Update (&mScrollRectH);
					if (mScrollV)
						mScrollV->Update (&mScrollRectV);
				}
				mDirtyIdle = false;
			}
            if(mRequestUpdate){
                PA_Rect areaRect = PA_GetAreaPortBounds (mCurParams);
                PA_RedrawArea ( mCurParams, char(1), &areaRect);
                mRequestUpdate = false;
            }
			//mbs 25052010
			if (mMenuEvent != eAE_Idle)
			{
				mCurEvent = mMenuEvent;
				mMenuEvent = eAE_Idle;
				PA_CallPluginAreaMethod (mCurParams);
			}
			if (mCallScriptInIdle)
			{
				mCallScriptInIdle = false;
				PA_CallPluginAreaMethod (mCurParams);
			}
			break;

		case eAE_EndExecutionCycle:
			if (mDirty &&  mVisible)
			{
//				mPosition = SRect (0., 0., mEditorRect.Height(), mEditorRect.Width());
				AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
				PA_RequestRedraw (mCurParams);
				mDirty = false;
			}
			break;
			
		//mbs 25052010
		case eAE_UpdateEditCommands:
		{
			SInt32	canUndo, canRedo;
			/* SInt32	operation = */ GetUndoRedoState (canUndo, canRedo);
			// ••• TODO •••	undo/redo string from operation
			// anyway, the string is ignored by 4D...
			// PA_UpdateEditMenu:	undoString, undo, redo, cut, copy, paste, clear, selectAll
			// PA_UpdateEditMenu v11.3:	undo, redo, cut, copy, paste, clear, selectAll
			PA_UpdateEditMenu (canUndo != 0, canRedo != 0, true, true, true, true, true);
			break;
		}

		//mbs 25052010
		case eAE_UndoCommand:
            mInterfaceEvent = mCurEvent;
			DoEventUndo();
			mMenuEvent = mCurEvent;
			break;
		case eAE_RedoCommand:
            mInterfaceEvent = mCurEvent;
			DoEventRedo();
			mMenuEvent = mCurEvent;
			break;
		case eAE_CutCommand:
		case eAE_CopyCommand:
		case eAE_PasteCommand:
		case eAE_ClearCommand:
		case eAE_SelectAllCommand:
            mInterfaceEvent = mCurEvent;
			mMenuEvent = mCurEvent;
//			PA_DontTakeEvent (mCurParams);
			break;
		
		case eAE_BeginDrag :
            mInterfaceEvent = mCurEvent;
			OnBeginDrag();	// drag from this area has started - put data into pasteboard
			break;
			
		case eAE_AllowDrop:
            mInterfaceEvent = mCurEvent;
			OnAllowDrop();
			break;
			
		case eAE_Drag :	// dragging 4D object allowed in previous eAE_AllowDrop
			OnDrag();
			break;
			
		case eAE_Drop :	// dropping object allowed in previous eAE_AllowDrop
            mInterfaceEvent = mCurEvent;
			OnDrop();
			break;
			
		default:
			PA_DontTakeEvent (mCurParams);
			break;
	}
}


// ---------------------------------------------------------------------------
// HandleUpdate													   [protected]
// ---------------------------------------------------------------------------

#if	MACVER

void
DMArea::HandleUpdate (bool inWindow)
{

    if(mCurEvent != eAE_Update)
    {
             mRequestUpdate = true;  // we can be in event where there is no crrent params from which we can get area
    }
    else
    {
        CGContextRef	cg = 0;
        cg = (CGContextRef) (( (PA_Event**) mCurParams->fParameters)[0])->fMessage;
        PA_Rect	portBounds = PA_GetAreaPortBounds (mCurParams);
        mPortRect.SetRect (portBounds.fTop, portBounds.fLeft, portBounds.fBottom, portBounds.fRight);
        CGContextScaleCTM (cg, 1.0, -1.0);
        CGContextTranslateCTM (cg, 0, -(portBounds.fBottom - portBounds.fTop));
            
        
        SRect	window (mPortRect);
        mScreen->SetPageRect (window);
        mScreen->SetContext (cg);
        
        PA_Rect			r = PA_GetAreaRect (mCurParams);
        mAreaFullRect = SRect (r.fTop, r.fLeft, r.fBottom, r.fRight);
        SRect	er (mAreaFullRect);
        er.bottom = mScrollRectH.top;
        er.right = mScrollRectV.left;
        mScreen->DrawRect (er, 0.0, false, cLightGrayColor, true, cLightGrayColor);
        
        mScreen->DrawRect (mAreaFullRect, sFocusInset, true, cLightGrayColor, false, cLightGrayColor);
        er.SetRect (mScrollRectH.top, mScrollRectH.right, mScrollRectH.bottom, mScrollRectV.right);
        mScreen->DrawRect (er, 0, false, cLightGrayColor, true, cLightGrayColor);
        if (mAreaSelected)
        {
            if (cg)
            {
                CGRect	r = CGRectMake (mAreaFullRect.left + sFocusInset, window.bottom - mAreaFullRect.bottom + sFocusInset, mAreaFullRect.Width() - 2*sFocusInset, mAreaFullRect.Height() - 2*sFocusInset);
                ::HIThemeDrawFocusRect (&r, true, cg, kHIThemeOrientationInverted);
            }
        }
        
        if (mShowRuler)
            DrawRulers();

        if (mScale != 1.0)
        {
            CGContextConcatCTM (cg, mScaleDraw);
            SRect	er = MapToArea (mEditorRect);
            DMReport::Draw (mScreen, er, mDrawMode);
            if (not mTrackSelect.IsEmpty())
            {
                er = MapToArea (mTrackSelect);
                mScreen->DrawRect (er, 0.5, true, cBlackColor, false, cBlackColor, 2, 2);
            }
        }
        else
        {
            DMReport::Draw (mScreen, mEditorRect, mDrawMode);
            if (not mTrackSelect.IsEmpty())
                mScreen->DrawRect (mTrackSelect, 0.5, true, cBlackColor, false, cBlackColor, 2, 2);
        }
        mScreen->SetContext (NULL);

        
        if (mScrollH == NULL)
        {
            mScrollH = new UIScrollBar (this, (WindowRef) mAreaProperties.fMacWindow, mScrollRectH);
            long	lMax = max (0., mPosition.Width() * mScale - mEditorRect.Width());
            mScrollH->SetValues (mScrollPos.h, 0, 0, 32, mEditorRect.Width());
            mScrollH->SetValues (mScrollPos.h, 0, lMax, 32, mEditorRect.Width());
            mScrollPos.h = mScrollH->GetValue();
            mScrollPosScaled.h = mScrollPos.h / -mScale;
        }
        if (mScrollV == NULL)
        {
            mScrollV = new UIScrollBar (this, (WindowRef) mAreaProperties.fMacWindow, mScrollRectV);
            long	lMax = max (0., mPosition.Height() * mScale - mEditorRect.Height());
            mScrollV->SetValues (mScrollPos.v, 0, 0, 32, mEditorRect.Height());
            mScrollV->SetValues (mScrollPos.v, 0, lMax, 32, mEditorRect.Height());
            mScrollPos.v = mScrollV->GetValue();
            mScrollPosScaled.v = mScrollPos.v / -mScale;
        }
        if (mScrollH)
            mScrollH->Update (&mScrollRectH);
        if (mScrollV)
            mScrollV->Update (&mScrollRectV);
    }
}

#elif	kUSE_FAKE_AREA

void
DMArea::HandleUpdate (bool inWindow)
{
#if	kUSE_CLIPMODE
	inWindow = true;	// we used PA_SetPluginAreaClipMode (area, true)
#endif

	Gdiplus::Matrix	transform (1.0, 0.0, 0.0, 1.0, -mAreaFullRect.left, -mAreaFullRect.top);

#if	kUSE_OFFSCREEN
	// HDC		dc = (HDC) (( (PA_Event**) mCurParams->fParameters)[0])->fMessage;
	HDC		dc = GetDC (mArea);
	HDC		newDC = ::CreateCompatibleDC (dc);
	const	int	l = 0;	// mAreaFullRect.left;
	const	int	t = 0;	// mAreaFullRect.top;
	const	int	w = mScrollRectV.left - mAreaFullRect.left;	// mScrollRectV.left;	// - l;
	const	int	h = mScrollRectH.top - mAreaFullRect.top;	// mScrollRectH.top;	// - t;
	HBITMAP	newBitmap = ::CreateCompatibleBitmap (dc, w, h);
	HBITMAP	oldBitmap = (HBITMAP) ::SelectObject (newDC, newBitmap);
	mScreen->SetContext ((void*) newDC);
#else
	mScreen->SetHWNDContext (mArea);
#endif
	mScreen->GetGDI()->SetTransform (&transform);


//	mScreen->DrawRect (mAreaFullRect, 0.0, false, cLightGrayColor, true, cLightGrayColor);
#if	kUSE_OFFSCREEN
	if (mAreaSelected)
		mScreen->DrawRect (mAreaFullRect, 2, true, cBlueColor, true, cLightGrayColor);
	else
		mScreen->DrawRect (mAreaFullRect, 0, false, cLightGrayColor, true, cLightGrayColor);
#else
	mScreen->DrawRect (mEditorRect, 0, false, cLightGrayColor, true, cLightGrayColor);
#endif
//	if (mShowToolbar)
//		mScreen->DrawRect (mToolbarRect, 1.0, true, cBlackColor, true, cGrayColor);
	if (mShowRuler)
		DrawRulers();

	{
		StClipToRect	clip (mScreen, mEditorRect, mNoHitTest);
//		mScreen->DrawRect(mEditorRect, 0, false, cBlackColor, true, cGrayColor);
		if (mScale != 1.0)
		{
#if	1
			Gdiplus::Matrix	matrix (mScale, 0, 0, mScale, mEditorRect.left * (1 - mScale), mEditorRect.top * (1 - mScale));
			SRect	er = MapToArea (mEditorRect);
			mScreen->GetGDI()->SetTransform (&matrix);
			DMReport::Draw (mScreen, er, mDrawMode);
			if (not mTrackSelect.IsEmpty())
			{
				er = MapToArea (mTrackSelect);
				mScreen->DrawRect (er, 0.5, true, cBlackColor, false, cBlackColor, 2, 2);
			}
#else

#if	kUSE_OFFSCREEN
//			Gdiplus::Matrix	matrix (mScale, 0, 0, mScale, mAreaFullRect.left * (1 - mScale), mAreaFullRect.top * (1 - mScale));
			Gdiplus::Matrix	matrix (mScale, 0, 0, mScale, 0, 0);
			AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() - mAreaFullRect.TopLeft() + mScrollPosScaled);
			SRect	er (mEditorRect);
			er -= mAreaFullRect.TopLeft();
			er = MapToArea (er);
#else
			Gdiplus::Matrix	matrix (mScale, 0, 0, mScale, -mAreaFullRect.left, -mAreaFullRect.top);
			SRect	er = MapToArea (mEditorRect);
#endif
			mScreen->GetGDI()->SetTransform (&matrix);
			DMReport::Draw (mScreen, er, mDrawMode);
			if (not mTrackSelect.IsEmpty())
			{
				er = MapToArea (mTrackSelect);
				mScreen->DrawRect (er, 0.5, true, cBlackColor, false, cBlackColor, 2, 2);
			}
#if	kUSE_OFFSCREEN
			AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
#endif

#endif
		}
		else
		{
			DMReport::Draw (mScreen, mEditorRect, mDrawMode);
			if (not mTrackSelect.IsEmpty())
				mScreen->DrawRect (mTrackSelect, 0.5, true, cBlackColor, false, cBlackColor, 2, 2);
		}
	}
	mScreen->SetContext (NULL);

#if	kUSE_OFFSCREEN
	if (mNoHitTest.IsEmpty())
		::BitBlt (dc, l, t, w, h, newDC, l, t, SRCCOPY);
	else
	{
		::BitBlt (dc, l, t, w, mNoHitTest.top, newDC, l, t, SRCCOPY);	// top
		::BitBlt (dc, l, t + mNoHitTest.top, mNoHitTest.left, h - mNoHitTest.top, newDC, l, t + mNoHitTest.top, SRCCOPY);	// left
		::BitBlt (dc, l + mNoHitTest.right, mNoHitTest.top, w - mNoHitTest.right, h - mNoHitTest.top, newDC, l + mNoHitTest.right, mNoHitTest.top, SRCCOPY);	// right
		::BitBlt (dc, l + mNoHitTest.left, mNoHitTest.bottom, mNoHitTest.Width(), h - mNoHitTest.bottom, newDC, l + mNoHitTest.left, mNoHitTest.bottom, SRCCOPY);	// bottom
	}
	::SelectObject (newDC, oldBitmap);
	::DeleteObject (newBitmap);
	::DeleteDC (newDC);
	::DeleteDC (dc);
#endif

	mScreen->SetHWNDContext (mArea);
	mScreen->GetGDI()->SetTransform (&transform);

	SRect	r (mAreaFullRect);
	if (mAreaSelected)
		mScreen->DrawRect (r, 2, true, cBlueColor, false, cLightGrayColor);
	else
		mScreen->DrawRect (r, 2, true, cLightGrayColor, false, cLightGrayColor);
	if (sFocusInset > 2)
	{
		r *= 2;
		mScreen->DrawRect (r, sFocusInset - 2, true, cLightGrayColor, false, cLightGrayColor);
	}
	r.SetRect (mScrollRectH.top, mScrollRectH.right, mScrollRectH.bottom, mScrollRectV.right);
	mScreen->DrawRect (r, 0, false, cLightGrayColor, true, cLightGrayColor);

	if (mScrollH == NULL)
	{
		mScrollH = new UIScrollBar (this, mArea, mScrollRectH);
		long	lMax = max (0., mPosition.Width() * mScale - mEditorRect.Width());
		mScrollH->SetValues (mScrollPos.h, 0, 0, 32, mEditorRect.Width());
		mScrollH->SetValues (mScrollPos.h, 0, lMax, 32, mEditorRect.Width());
		mScrollPos.h = mScrollH->GetValue();
		mScrollPosScaled.h = mScrollPos.h / -mScale;
	}
	if (mScrollV == NULL)
	{
		mScrollV = new UIScrollBar (this, mArea, mScrollRectV);
		long	lMax = max (0., mPosition.Height() * mScale - mEditorRect.Height());
		mScrollV->SetValues (mScrollPos.v, 0, 0, 32, mEditorRect.Height());
		mScrollV->SetValues (mScrollPos.v, 0, lMax, 32, mEditorRect.Height());
		mScrollPos.v = mScrollV->GetValue();
		mScrollPosScaled.v = mScrollPos.v / -mScale;
	}
	if (mScrollH)
		mScrollH->Update (&mScrollRectH);
	if (mScrollV)
		mScrollV->Update (&mScrollRectV);
}

#else	// WINVER && !kUSE_FAKE_AREA

void
DMArea::HandleUpdate (bool inWindow)
{
#if	kUSE_CLIPMODE
	inWindow = true;	// we used PA_SetPluginAreaClipMode (area, true)
#endif
    Gdiplus::Matrix	transform (1.0, 0.0, 0.0, 1.0, -mAreaFullRect.left, -mAreaFullRect.top);

	 HDC		dc = (HDC) PA_GetUpdateHDC();
#if	kUSE_OFFSCREEN
//    HDC		dc = GetDC (mArea);
    HDC		newDC = ::CreateCompatibleDC (dc);
    const	int	l = 0; // mAreaFullRect.left;
    const	int	t = 0; //mAreaFullRect.top;
	const	int	w = mScrollRectV.left - mAreaFullRect.left;;	// - l;
	const	int	h = mScrollRectH.top - mAreaFullRect.top;	// - t;
	HBITMAP	newBitmap = ::CreateCompatibleBitmap (dc, w, h);
	HBITMAP	oldBitmap = (HBITMAP) ::SelectObject (newDC, newBitmap);
	mScreen->SetContext ((void*) newDC);
#else
	mScreen->SetContext (dc);
//    mScreen->SetHWNDContext (mArea);
#endif
    mScreen->GetGDI()->SetTransform (&transform);

    
//	mScreen->DrawRect (mAreaFullRect, 0.0, false, cLightGrayColor, true, cLightGrayColor);
#if	kUSE_OFFSCREEN
	if (mAreaSelected)
		mScreen->DrawRect (mAreaFullRect, 2, true, cBlueColor, true, cLightGrayColor);
	else
		mScreen->DrawRect (mAreaFullRect, 0.0, false, cLightGrayColor, true, cLightGrayColor);
#else
	mScreen->DrawRect (mEditorRect, 0.0, false, cLightGrayColor, true, cLightGrayColor);
#endif
//	if (mShowToolbar)
//		mScreen->DrawRect (mToolbarRect, 1.0, true, cBlackColor, true, cGrayColor);
	if (mShowRuler)
		DrawRulers();

	{
		StClipToRect	clip (mScreen, mEditorRect, mNoHitTest);
//		mScreen->DrawRect(mEditorRect, 0, false, cBlackColor, true, cGrayColor);
		if (mScale != 1.0)
		{
			Gdiplus::Matrix	matrix (mScaleDraw.a, mScaleDraw.b, mScaleDraw.c, mScaleDraw.d, mScaleDraw.tx, mScaleDraw.ty);
			mScreen->GetGDI()->SetTransform (&matrix);
			SRect	er = MapToArea (mEditorRect);
			DMReport::Draw (mScreen, er, mDrawMode);
			if (not mTrackSelect.IsEmpty())
			{
				er = MapToArea (mTrackSelect);
				mScreen->DrawRect (er, 0.5, true, cBlackColor, false, cBlackColor, 2, 2);
			}
		}
		else
		{
			DMReport::Draw (mScreen, mEditorRect, mDrawMode);
			if (not mTrackSelect.IsEmpty())
				mScreen->DrawRect (mTrackSelect, 0.5, true, cBlackColor, false, cBlackColor, 2, 2);
		}
	}
	mScreen->SetContext (NULL);

#if	kUSE_OFFSCREEN
	if (mNoHitTest.IsEmpty())
		::BitBlt (dc, l, t, w, h, newDC, l, t, SRCCOPY);
	else
	{
		::BitBlt (dc, l, t, w, mNoHitTest.top, newDC, l, t, SRCCOPY);	// top
		::BitBlt (dc, l, t + mNoHitTest.top, mNoHitTest.left, h - mNoHitTest.top, newDC, l, t + mNoHitTest.top, SRCCOPY);	// left
		::BitBlt (dc, l + mNoHitTest.right, mNoHitTest.top, w - mNoHitTest.right, h - mNoHitTest.top, newDC, l + mNoHitTest.right, mNoHitTest.top, SRCCOPY);	// right
		::BitBlt (dc, l + mNoHitTest.left, mNoHitTest.bottom, mNoHitTest.Width(), h - mNoHitTest.bottom, newDC, l + mNoHitTest.left, mNoHitTest.bottom, SRCCOPY);	// bottom
	}
	::SelectObject (newDC, oldBitmap);
	::DeleteObject (newBitmap);
	::DeleteDC (newDC);
#endif

	mScreen->SetContext ((HDC) mAreaProperties.fWinHDC);
	SRect	r (mAreaFullRect);
	if (mAreaSelected)
		mScreen->DrawRect (r, 2, true, cBlueColor, false, cLightGrayColor);
	else
		mScreen->DrawRect (r, 2, true, cLightGrayColor, false, cLightGrayColor);
	if (sFocusInset > 2)
	{
		r *= 2;
		mScreen->DrawRect (r, sFocusInset - 2, true, cLightGrayColor, false, cLightGrayColor);
	}
	r.SetRect (mScrollRectH.top, mScrollRectH.right, mScrollRectH.bottom, mScrollRectV.right);
	mScreen->DrawRect (r, 0, false, cLightGrayColor, true, cLightGrayColor);

	if (mScrollH == NULL)
	{
		mScrollH = new UIScrollBar (this, (HWND) mAreaProperties.fWinHWND, mScrollRectH);
		long	lMax = max (0., mPosition.Width() * mScale - mEditorRect.Width());
		mScrollH->SetValues (mScrollPos.h, 0, 0, 32, mEditorRect.Width());
		mScrollH->SetValues (mScrollPos.h, 0, lMax, 32, mEditorRect.Width());
		mScrollPos.h = mScrollH->GetValue();
		mScrollPosScaled.h = mScrollPos.h / -mScale;
	}
	if (mScrollV == NULL)
	{
		mScrollV = new UIScrollBar (this, (HWND) mAreaProperties.fWinHWND, mScrollRectV);
		long	lMax = max (0., mPosition.Height() * mScale - mEditorRect.Height());
		mScrollV->SetValues (mScrollPos.v, 0, 0, 32, mEditorRect.Height());
		mScrollV->SetValues (mScrollPos.v, 0, lMax, 32, mEditorRect.Height());
		mScrollPos.v = mScrollV->GetValue();
		mScrollPosScaled.v = mScrollPos.v / -mScale;
	}
#if	!kUSE_CLIPMODE
	if (not inWindow)
	{
		if (mScrollH)
			mScrollH->Update (&mScrollRectH, (HDC) PA_GetUpdateHDC());
		if (mScrollV)
			mScrollV->Update (&mScrollRectV, (HDC) PA_GetUpdateHDC());
	}
	else
#endif
	if (mScrollH)
		mScrollH->Update (&mScrollRectH);
	if (mScrollV)
		mScrollV->Update (&mScrollRectV);
}

#endif


// ---------------------------------------------------------------------------
// DrawRulers													   [protected]
// ---------------------------------------------------------------------------

void
DMArea::DrawRulers (void)
{
	SRGBColor	color ( 0xe9e9, 0xe9e9, 0xe9e9, 0xffff );

	SRect	r (mRulerRectH.top, mRulerRectV.left, mRulerRectH.bottom, mRulerRectV.right);
	mScreen->DrawRect (r, 0, false, cBlackColor, true, color);
	mScreen->DrawRect (mRulerRectH, 1.0, true, cBlackColor, true, color);
	if (mRulerAbsolute)
		mScreen->DrawRect (mRulerRectV, 1.0, true, cBlackColor, true, color);

	int		valueEvery, valueInc, valueStep = 1;
	double	ptPerMark;
	RWString	o;
	switch (mRulerUnits)
	{
		default:// [pt]
//			o.AssignAscii ("[pt]");
			valueEvery = 5;
			valueInc = 50;
			ptPerMark = 10 * mScale;
			if (ptPerMark > 50)
			{
				valueInc /= 10;
				ptPerMark /= 10;
			}
			valueStep = 1;
			break;
		case 2:	// [mm]
//			o.AssignAscii ("[mm]");
			valueEvery = 4;
			valueInc = 10;
			ptPerMark = 0.25 * 72. / 2.54 * mScale;
			if (ptPerMark > 50)
			{
				valueInc /= 10;
				ptPerMark /= 10;
			}
			valueStep = 2;
			break;
		case 3:	// [in]
//			o.AssignAscii ("[in]");
			valueEvery = 8;
			valueInc = 1;
			ptPerMark = 9 * mScale;
			valueStep = 2;
			break;
	}
	while (ptPerMark < 5)
	{
		valueInc *= 2;
		ptPerMark *= 2;
	}
	while (ptPerMark * valueEvery < 40)
	{
		valueInc *= 2;
		ptPerMark *= 2;
	}
	
	//pB a bit of normalisation
	switch (valueInc) {
		case 40:
			valueInc = 50;
			valueEvery = 5;
			ptPerMark *= 1.25;
			valueStep = 1;
			break;
		case 80:
			valueInc = 100;
			valueEvery = 4;
			ptPerMark *= 1.25;
			valueStep = 2;
			break;
		case 4:
			valueInc = 5;
			valueEvery = 1;
			ptPerMark *= 1.25;
			valueStep = 1;
			break;
		case 8:
			valueInc = 10;
			valueEvery = 4;
			ptPerMark *= 1.25;
			valueStep = 2;
			break;
			
		default:
			break;
	}

	RWStyle	*style = GetStyle (-1);
	int		startMark, endMark, i;
	double	where;
	char	str [16];

//	mScreen->DrawTextBox (o, style, r, false, false, false, NULL);
	color.red = color.green = color.blue = 17476;

	// horizontal ruler
	{
		r = mRulerRectH;
		r *= 1;
		startMark = mScrollPos.h / ptPerMark;
		endMark = startMark + (mEditorRect.Width() / ptPerMark) + 1;
		where = mEditorRect.left - mScrollPos.h + startMark * ptPerMark;

		StClipToRect	clip (mScreen, r);
		for (i = startMark; i <= endMark; i++, where += ptPerMark)
		{
			r.left = r.right = where;
			if ((i % valueEvery) == 0)
			{
				r.bottom = r.top + 5;
				static_cast <RWPageComposer*> (mScreen)->DrawLine (r, 1.0f, color, RWLine_Vertical);
				snprintf (str, sizeof (str), "%d", valueInc * (i / valueEvery));
				o = RWStr::FromASCII (str);
				r.left += 1;
				r.right = r.left + 100;
				r.bottom = r.top + 30;
				mScreen->DrawTextBox (o, style, r, false, false, false, NULL);
			}
			else
			{
				if ((i % valueStep) == 0)
					r.bottom = r.top + 5;			
				else
					r.bottom = r.top + 2;
				static_cast <RWPageComposer*> (mScreen)->DrawLine (r, 0.5f, color, RWLine_Vertical);
			}
		}
	}

	// vertical ruler
	if (mRulerAbsolute)
	{
		r = mRulerRectV;
		r *= 1;
		startMark = mScrollPos.v / ptPerMark;
		endMark = startMark + (mEditorRect.Height() / ptPerMark) + 1;
		where = mEditorRect.top - mScrollPos.v + startMark * ptPerMark;
		
		StClipToRect	clip (mScreen, r);
		for (i = startMark; i <= endMark; i++, where += ptPerMark)
		{
			r.top = r.bottom = where;
			if (i % valueEvery == 0)
			{
				r.right = r.left + 5;
				static_cast <RWPageComposer*> (mScreen)->DrawLine (r, 1.0f, color, RWLine_Horizontal);
				snprintf (str, sizeof (str), "%d", valueInc * (i / valueEvery));
				o = RWStr::FromASCII (str);
				r.top -= 8;
				r.left += 5;
				r.right = r.left + 100;
				r.bottom = r.bottom + 10;
				mScreen->DrawTextBox (o, style, r, false, false, false, NULL);
				r.left = mRulerRectV.left + 1;
			}
			else
			{
				if ((i % valueStep) == 0)
					r.right = r.left + 5;
				else
					r.right = r.left + 2;
				static_cast <RWPageComposer*> (mScreen)->DrawLine (r, 0.5f, color, RWLine_Horizontal);
			}
		}
	}
}

// ---------------------------------------------------------------------------
// GetEventModifiers                                                       [public]
// ---------------------------------------------------------------------------

UInt32
DMArea::GetEventModifiers(void)
{
#if    WINVER
    UInt32          result = 0;
    if(GetKeyState(VK_CONTROL)) result += controlKey;
    if(GetKeyState(VK_SHIFT)) result += shiftKey;
    if(GetKeyState(VK_MENU)) result += optionKey;
    if(GetKeyState(VK_SHIFT)) result += shiftKey;

    return result;
#else
    unsigned long   flags;
    UInt32          result = 0;
    flags = [NSEvent modifierFlags];
    if(flags & NSEventModifierFlagCommand) result += cmdKey;
    if(flags & NSEventModifierFlagShift) result += shiftKey;
    if(flags & NSEventModifierFlagControl) result += controlKey;
    if(flags & NSEventModifierFlagOption) result += optionKey;
    if(flags & NSEventModifierFlagCapsLock) result += alphaLock;
    return result;
    // return GetCurrentKeyModifiers();
#endif
}
// ---------------------------------------------------------------------------
// AdjustCursor													   [protected]
// ---------------------------------------------------------------------------

#if	WINVER
extern "C" 
{
#define MacGetCursor		GetCurso
#define MacSetCursor		SetCurso
#if	!USE_MAC_API
typedef	struct	_MacCursor	*_MacCursPtr, **_MacCursHandle;
void			__stdcall	MacSetCursor (const _MacCursor * crsr);
_MacCursHandle	__stdcall	MacGetCursor(short cursorID);
#endif
}
#endif

void
DMArea::AdjustCursor (int inHit)
{
#if	1	// MACVER
	int	cursor;

	switch (inHit)
	{
		case eHit_None: cursor = 355; break;
		case eHit_Object:	mCurEvent == eAE_MouseDown? cursor = 556: cursor = 355; break;
		case eHit_TLH:	cursor = 9005; break;
		case eHit_TCH:	cursor = 9004; break;
		case eHit_TRH:	cursor = 9006; break;
		case eHit_RCH:	cursor = 9003; break;
		case eHit_BRH:	cursor = 9005; break;
		case eHit_BCH:	cursor = 9004; break;
		case eHit_BLH:	cursor = 9006; break;
		case eHit_LCH:	cursor = 9003; break;
		case eHit_ResizeH:	cursor = 9008; break;
		case eHit_ResizeV:	cursor = 9009; break;

		case -1: cursor = 15009; break;		// drag select
		case -2: cursor = 15000; break;		// drag create guide
	}
//pB	MacSetCursor (*MacGetCursor (cursor));
    
    PA_Variable    vars[2];
    //        vars [0].fType = eVK_Undefined;            // no retVal
    vars[0] = PA_CreateVariable(eVK_Longint);
    vars[1] = PA_CreateVariable(eVK_Longint);
    PA_SetLongintVariable (&vars [0], (long) cursor);
    PA_SetLongintVariable (&vars [1], (long) cursor);
    (void) PA_ExecuteCommandByID (469, vars, 1);
    //                result = PA_GetLastError();
    PA_ClearVariable (&vars [0]);
    PA_ClearVariable (&vars [1]);
    
#endif
}



SInt32
DMArea::SaveUndo (SInt32 inOperation, bool inSelection)
{
	SInt32	result = paramErr;
	switch (inOperation)
	{
		case -1:	// pause
			if (mUndoBuffer.IsRecording())
			{
				mUndoBuffer.PauseRecording (true);
				result = noErr;
			}
			break;

		case -2:	// resume
			if (mUndoBuffer.IsRecordingPaused())
			{
				mUndoBuffer.PauseRecording (false);
				result = noErr;
			}
			break;

		case 0:		// stop
			if (mUndoBuffer.IsRecording() || mUndoBuffer.IsRecordingPaused())
			{
				mUndoBuffer.StopRecording();
				result = noErr;
			}
			break;
			
		default:	// start
			mUndoBuffer.StartRecording (this, inOperation, inSelection);;
			result = noErr;
			break;
	}
	return result;
}


#if	0
SInt32
DMArea::DoEventCut (void)
{
	SInt32	err = DoEventCopy();
	if (err == noErr)
		err = DoEventClear();
	return err;
}


SInt32
DMArea::DoEventCopy (void)
{
	SInt32		err = unimpErr;
	return err;
}


SInt32
DMArea::DoEventPaste (void)
{
	SInt32		err = unimpErr;
	return err;
}


SInt32
DMArea::DoEventClear (void)
{
	SInt32		err = unimpErr;
	return err;
}


SInt32
DMArea::DoEventSelectAll (void)
{
	SInt32		err = unimpErr;
	return err;
}
#endif

#if __LP64__
// ---------------------------------------------------------------------------
// HandleMouse	for 64 bit build												   [protected]
// ---------------------------------------------------------------------------

void
DMArea::HandleMouse (void)
{
    QDPoint	wPt;
    PA_GetClick (mCurParams, &wPt.h, &wPt.v);
    SPoint	where (wPt);
    int		hit = eHit_None;

    double doubleClickTime = [NSEvent doubleClickInterval];    // from seconds to ticks
    double when = [NSDate timeIntervalSinceReferenceDate];

    if (mCurEvent == eAE_MouseDown)
    {
        mIsDrag = true;
        // PA_Event* ev = (( (PA_Event**) mCurParams->fParameters)[0]);
        mDoubleClick = ((when - doubleClickTime) <= mLastEventTime && fabs (where.h - mLastEventPos.h) < 2 && fabs (where.v - mLastEventPos.v) < 2);
        mLastEventTime = when;
        mLastEventPos = where;
        mTrackEvent = 0;
        mTrackHit = 0;
        
        if (where.IsContained (mEditorRect))
        {
            where = MapToArea (wPt);
            mLastAreaPos = where;
            DMBase *	hobj = NULL;
            
            if (mToolI == eTool_Select)
            {
                if (mCurEvent == eAE_Cursor)
                    mLastEventModifiers = GetModifiers();	//mbs 10052010
                if (mLastEventModifiers & optionKey)
                {
                    hit = -1;	// track select
                    AdjustCursor (hit);
                    DeselectAll();
                    PA_CallPluginAreaMethod (mCurParams);
                    mTrackEvent = 1;
                }
                else
                {
                    hit = HitTest (where, hobj);
                    mLastEventHit = hit;
                    mLastObjectHit = hobj;
                    
                    bool	wasSelected = (hobj != NULL && hobj->GetSelected());
                    if (hit != eHit_Object || (mLastEventModifiers & (shiftKey | cmdKey)) == 0)
                        if (not wasSelected)
                                DeselectAll();
                    if (hit != eHit_None && hobj != NULL && (mLastEventModifiers & shiftKey) != 0)
                        hobj->SetSelected (true);
                    else if (hit == eHit_Object && hobj != NULL && (mLastEventModifiers & cmdKey) != 0)
                        hobj->SetSelected (not wasSelected);
                    else if (hit != eHit_None && hobj != NULL)
                            hobj->SetSelected (true);
                        
                    hit = HitTest (where, hobj);
                    if (hobj != NULL && hobj != mLastObjectHit)
                    {
                        hobj = mLastObjectHit;
                        hit = mLastEventHit;
                    }
                    bool	canTrack = (hobj != NULL && hobj->GetSelected() && not hobj->IsLocked());
                    if (mSelectedObjects.size() == 0)
                    {
                        SetSelected (true);	// select report
                    }
                    else if (canTrack && mSelectedObjects.size() > 0)	//mbs 02082010	can't track even single header cell ;-)
                    {
                        // ••• TODO •••	multiple selection must not contain different "levels" of objects e.g. Guide with Section and Object...
                        PSObjList::iterator	iter;
                        EObject_Kind		kind;
                        for (iter = mSelectedObjects.begin(); iter != mSelectedObjects.end(); iter++)
                        {
                            hobj = static_cast <DMBase*> (*iter);
                            if (hobj->IsLocked())
                            {
                                canTrack = false;
                                break;
                            }
                            if (iter == mSelectedObjects.begin())
                            {
                                kind = hobj->GetKind();
                                if (kind <= eObject_Table)
                                    kind = eObject_Group;
                            }
                            else
                            {
                                EObject_Kind	thisKind = hobj->GetKind();
                                if (thisKind <= eObject_Table)
                                    thisKind = eObject_Group;
                                if (kind != thisKind)
                                {
                                    canTrack = false;
                                    break;
                                }
                            }
                        }
                        //mbs 10012010	can't drag section/header/column
                        if (canTrack && (kind == eObject_Section || kind == eObject_TblCol || kind == eObject_TblHdr))
                            if (hit == eHit_Object)
                                canTrack = false;
                    }
                        
                    mDirty = true;
                    HandleUpdate (true);
                    if (canTrack)
                    {
                        AdjustCursor (hit);
                        mTrackEvent = 2;
                        mTrackHit = hit; // pas to mouse handler
                        mLastDragObject = hobj;
                        TrackObjectStart();
                    }
                    mDirty = true;
                    PA_CallPluginAreaMethod (mCurParams);
                    hit = HitTest (where, hobj);
                }
                mCurEvent = eAE_Cursor;
                AdjustCursor (hit);
                mCurEvent = PA_GetAreaEvent (mCurParams);
            }
       
            else
            {
                if (mCurEvent == eAE_MouseDown)
                {
                    SaveUndo (10001, true);	// eUndo_NewObject
                    mDirty = true;
                    DeselectAll();
                    
                    DMBase	*parent = GetParentAt (where);
                    if (parent)
                    {
                        PSObjListD	*objects = NULL;
                        parent->GetObjects (PSObjPropObjects, objects);
                        hobj = NULL;	//mbs 18062010
                        
                        switch (mToolI)
                        {
                                //mbs 18062010	use CreateObject to generate mID...
                            case eTool_Select: break;	// to shut up compiler
                            case eTool_CreateGroup:	hobj = CreateObject (PSObjPropOGroup, parent, RWXmlNode()); break;	// DMGroup::Create (parent, NULL); break;
                            case eTool_CreateLine:	hobj = CreateObject (PSObjPropOLine, parent, RWXmlNode()); break;	// DMLine::Create (parent, NULL); break;
                            case eTool_CreateRect:	hobj = CreateObject (PSObjPropORect, parent, RWXmlNode()); break;	// DMRect::Create (parent, NULL); break;
                            case eTool_CreateOval:	hobj = CreateObject (PSObjPropOOval, parent, RWXmlNode()); break;	// DMOval::Create (parent, NULL); break;
                            case eTool_CreatePict:	hobj = CreateObject (PSObjPropOPict, parent, RWXmlNode()); break;	// DMPict::Create (parent, NULL); break;
                            case eTool_CreateText:	hobj = CreateObject (PSObjPropOText, parent, RWXmlNode()); break;	// DMText::Create (parent, NULL); break;
                            case eTool_CreateVar:	hobj = CreateObject (PSObjPropOVar, parent, RWXmlNode()); break;	// DMVariable::Create (parent, NULL); break;
                            case eTool_CreateField:	hobj = CreateObject (PSObjPropOFld, parent, RWXmlNode()); break;	// DMField::Create (parent, NULL); break;
                            case eTool_CreateTable:	hobj = CreateObject (PSObjPropOTable, parent, RWXmlNode()); break;	// DMTable::Create (parent, NULL); break;
                            case eTool_last: break;	// to shut up compiler
                        }
                        if (hobj)
                        {
                            //						objects->push_back (hobj);
                            hobj->SetOrder (objects->size());
                            hobj->SetSelected (true);
                            SRect	pos (parent->GetDrawPosition());
                            pos.top = RoundUI (where.v - pos.top);		// - mScrollPosScaled.v);
                            pos.left = RoundUI (where.h - pos.left);	// - mScrollPosScaled.h);
                            pos.right = pos.left + 48;
                            pos.bottom = pos.top + 20;
                            if (mToolI == eTool_CreateLine)
                            {
                                pos.bottom = pos.top;
                                hit = eHit_RCH;
                            }
                            else
                                hit = eHit_BRH;
                            SnapRect (hobj, pos, eHit_None);
                            hobj->SetPosition (pos);
                            
                            HandleUpdate (true);
                            AdjustCursor (hit);
                            mTrackHit = hit; // pas to mouse handler
                            mLastDragObject = hobj;

                            TrackNewObjectStart();
                      }
                        mDirty = true;
                    }
                    ////			else	// no section/group!
                    //					mToolI = eTool_Select;
                    SaveUndo (0);
                    if (hobj)
                        PA_CallPluginAreaMethod (mCurParams);
                }
                hit = HitTest (where, hobj);
                AdjustCursor (hit);
            }
        }
    } else if (mCurEvent == eAE_MouseUp) {
        if (mTrackEvent == 1) // select
        {
            mTrackSelect.SetRect (0, 0, 0, 0);
            mTrackEvent = 0;
            HandleUpdate (true);

        }
    } else if (mCurEvent == eAE_Cursor) {
        if (mTrackEvent == 1) // select
        {
            if (MouseDown())
            {
                if (where.h >= mLastEventPos.h)
                {
                    mTrackSelect.left = mLastEventPos.h;
                    mTrackSelect.right = where.h;
                }
                else
                {
                    mTrackSelect.left = where.h;
                    mTrackSelect.right = mLastEventPos.h;
                }
                if (where.v >= wPt.v)
                {
                    mTrackSelect.top = mLastEventPos.v;
                    mTrackSelect.bottom = where.v;
                }
                else
                {
                    mTrackSelect.top = where.v;
                    mTrackSelect.bottom = mLastEventPos.v;
                }
                SRect    sr (mTrackSelect); // PB - mScrollPosScaled);
                sr = MapToArea (sr);
                HandleTrackSelect (sr, (mLastEventModifiers & optionKey) == 0);
                HandleUpdate (true);
            } else
            {
                mTrackSelect.SetRect (0, 0, 0, 0);
                mTrackEvent = 0;
                HandleUpdate (true);
                mDirty = true;
                mCallScriptInIdle = true; // cannot call script from cursor event
                mInterfaceEvent = eAE_MouseUp;
            }
            
        } else if (mTrackEvent == 2) // resize
        {
            if (MouseDown())
            {
                if(WaitMouseMoved (where, when))
                {
                    TrackObjectBody(where, when);
                    HandleUpdate (true);
                }
            } else
            {
                TrackObjectEnd(0);
                mTrackEvent = 0;
                HandleUpdate (true);
                mDirty = true;
                mCallScriptInIdle = true; // cannot call script from cursor event
                mInterfaceEvent = eAE_MouseUp;
           }
        } else if (mTrackEvent == 3) // create
        {
            if (MouseDown())
            {
                if(WaitMouseMoved (where, when))
                {
                    TrackNewObjectBody(where, when);
                    HandleUpdate (true);
                }
            } else
            {
                TrackNewObjectEnd(0);
                mTrackEvent = 0;
                HandleUpdate (true);
                mDirty = true;
                mCallScriptInIdle = true; // cannot call script from cursor event
                mInterfaceEvent = eAE_MouseUp;
            }
        }
    }

}

Boolean
DMArea::WaitMouseMoved (SPoint where, double when)
{
#define kDragMinDist        4
    double doubleClickTime = [NSEvent doubleClickInterval];    // from seconds to ticks
    
    if (((when - doubleClickTime) > mLastEventTime || fabs (where.h - mLastEventPos.h) > kDragMinDist || fabs (where.v - mLastEventPos.v) > kDragMinDist))
        return true;
    
    return false;    // mouse release without dragging
#undef kDragMinDist
}

#else

// ---------------------------------------------------------------------------
// HandleMouse for 32 bit apps													   [protected]
// ---------------------------------------------------------------------------

void
DMArea::HandleMouse (void)
{
	QDPoint	wPt;
	PA_GetClick (mCurParams, &wPt.h, &wPt.v);
	SPoint	where (wPt);
	int		hit = eHit_None;

	if (mCurEvent == eAE_MouseDown)
	{
#if	MACVER
		//  pB 2012 4D v12 does not pass fWhen
		 PA_Event* ev = ( (PA_Event**) mCurParams->fParameters )[ 0 ];
		 mDoubleClick = ((ev->fWhen - ::GetDblTime()) <= mLastEventTime && fabs (where.h - mLastEventPos.h) < 2 && fabs (where.v - mLastEventPos.v) < 2);
		 mLastEventTime = ev->fWhen;
        
#else
		mDoubleClick = ((::timeGetTime() - ::GetDoubleClickTime()) <= mLastEventTime && fabs (where.h - mLastEventPos.h) < 2 && fabs (where.v - mLastEventPos.v) < 2);
		mLastEventTime = ::timeGetTime();
#endif
		mLastEventPos = where;
	}

	if (where.IsContained (mScrollRectH))
	{
		if (mCurEvent == eAE_MouseDown)
			mScrollH->Track (wPt);
		else
			AdjustCursor (eHit_None);
	}
	else if (where.IsContained (mScrollRectV))
	{
		if (mCurEvent == eAE_MouseDown)
			mScrollV->Track (wPt);
		else
			AdjustCursor (eHit_None);
	}
	else if (where.IsContained (mRulerRectH))
	{
		if (mCurEvent == eAE_MouseDown)
			TrackNewGuide (wPt, true);
		else
		{
			hit = -2;
			AdjustCursor (hit);
		}
	}
	else if (where.IsContained (mRulerRectV))
	{
		if (mCurEvent == eAE_MouseDown)
			TrackNewGuide (wPt, false);
		else
		{
			hit = -2;
			AdjustCursor (hit);
		}
	}
	else if (where.IsContained (mEditorRect))
	{
		where = MapToArea (wPt);
		DMBase *	hobj = NULL;

		if (mToolI == eTool_Select)
		{
			if (mCurEvent == eAE_Cursor)
				mLastEventModifiers = GetModifiers();	//mbs 10052010
			if (mLastEventModifiers & optionKey)
			{
				hit = -1;	// track select
				AdjustCursor (hit);
				if (mCurEvent == eAE_MouseDown)
				{
					DeselectAll();
					if (TrackSelect (wPt))
						;	//nothing to do
				}
				PA_CallPluginAreaMethod (mCurParams);
			}
			else
			{
				hit = HitTest (where, hobj);
				mLastEventHit = hit;
				mLastObjectHit = hobj;
				
				if (mCurEvent == eAE_MouseDown)
				{
					bool	wasSelected = (hobj != NULL && hobj->GetSelected());
					if (hit != eHit_Object || (mLastEventModifiers & (shiftKey | cmdKey)) == 0)
						if (not wasSelected)
							DeselectAll();
					if (hit != eHit_None && hobj != NULL && (mLastEventModifiers & shiftKey) != 0)
						hobj->SetSelected (true);
					else if (hit == eHit_Object && hobj != NULL && (mLastEventModifiers & cmdKey) != 0)
						hobj->SetSelected (not wasSelected);
					else if (hit != eHit_None && hobj != NULL)
						hobj->SetSelected (true);

					hit = HitTest (where, hobj);
					if (hobj != NULL && hobj != mLastObjectHit)
					{
						hobj = mLastObjectHit;
						hit = mLastEventHit;
					}
					bool	canTrack = (hobj != NULL && hobj->GetSelected() && not hobj->IsLocked());
					if (mSelectedObjects.size() == 0)
					{
						SetSelected (true);	// select report
					}
					else if (canTrack && mSelectedObjects.size() > 0)	//mbs 02082010	can't track even single header cell ;-)
					{
						// ••• TODO •••	multiple selection must not contain different "levels" of objects e.g. Guide with Section and Object...
						PSObjList::iterator	iter;
						EObject_Kind		kind;
						for (iter = mSelectedObjects.begin(); iter != mSelectedObjects.end(); iter++)
						{
							hobj = static_cast <DMBase*> (*iter);
							if (hobj->IsLocked())
							{
								canTrack = false;
								break;
							}
							if (iter == mSelectedObjects.begin())
							{
								kind = hobj->GetKind();
								if (kind <= eObject_Table)
									kind = eObject_Group;
							}
							else
							{
								EObject_Kind	thisKind = hobj->GetKind();
								if (thisKind <= eObject_Table)
									thisKind = eObject_Group;
								if (kind != thisKind)
								{
									canTrack = false;
									break;
								}
							}
						}
						//mbs 10012010	can't drag section/header/column
						if (canTrack && (kind == eObject_Section || kind == eObject_TblCol || kind == eObject_TblHdr))
							if (hit == eHit_Object)
								canTrack = false;
					}

					mDirty = true;
					HandleUpdate (true);
					if (canTrack)
					{
						AdjustCursor (hit);
						if (TrackObject (hobj, wPt, hit))
							;	//nothing to do
					}
					mDirty = true;
					PA_CallPluginAreaMethod (mCurParams);
					hit = HitTest (where, hobj);
				}
			}
			mCurEvent = eAE_Cursor;
			AdjustCursor (hit);
			mCurEvent = PA_GetAreaEvent (mCurParams);
		}
		else
		{
			if (mCurEvent == eAE_MouseDown)
			{
				SaveUndo (10001, true);	// eUndo_NewObject
				mDirty = true;
				DeselectAll();
				
				DMBase	*parent = GetParentAt (where);
				if (parent)
				{
					PSObjListD	*objects = NULL;
					parent->GetObjects (PSObjPropObjects, objects);
					hobj = NULL;	//mbs 18062010
					
					switch (mToolI)
					{
						//mbs 18062010	use CreateObject to generate mID...
						case eTool_Select: break;	// to shut up compiler
						case eTool_CreateGroup:	hobj = CreateObject (PSObjPropOGroup, parent, RWXmlNode()); break;	// DMGroup::Create (parent, NULL); break;
						case eTool_CreateLine:	hobj = CreateObject (PSObjPropOLine, parent, RWXmlNode()); break;	// DMLine::Create (parent, NULL); break;
						case eTool_CreateRect:	hobj = CreateObject (PSObjPropORect, parent, RWXmlNode()); break;	// DMRect::Create (parent, NULL); break;
						case eTool_CreateOval:	hobj = CreateObject (PSObjPropOOval, parent, RWXmlNode()); break;	// DMOval::Create (parent, NULL); break;
						case eTool_CreatePict:	hobj = CreateObject (PSObjPropOPict, parent, RWXmlNode()); break;	// DMPict::Create (parent, NULL); break;
						case eTool_CreateText:	hobj = CreateObject (PSObjPropOText, parent, RWXmlNode()); break;	// DMText::Create (parent, NULL); break;
						case eTool_CreateVar:	hobj = CreateObject (PSObjPropOVar, parent, RWXmlNode()); break;	// DMVariable::Create (parent, NULL); break;
						case eTool_CreateField:	hobj = CreateObject (PSObjPropOFld, parent, RWXmlNode()); break;	// DMField::Create (parent, NULL); break;
						case eTool_CreateTable:	hobj = CreateObject (PSObjPropOTable, parent, RWXmlNode()); break;	// DMTable::Create (parent, NULL); break;
						case eTool_last: break;	// to shut up compiler
					}
					if (hobj)
					{
//						objects->push_back (hobj);
						hobj->SetOrder (objects->size());
						hobj->SetSelected (true);
						SRect	pos (parent->GetDrawPosition());
						pos.top = RoundUI (where.v - pos.top);		// - mScrollPosScaled.v);
						pos.left = RoundUI (where.h - pos.left);	// - mScrollPosScaled.h);
						pos.right = pos.left + 48;
						pos.bottom = pos.top + 20;
						if (mToolI == eTool_CreateLine)
						{
							pos.bottom = pos.top;
							hit = eHit_RCH;
						}
						else
							hit = eHit_BRH;
						SnapRect (hobj, pos, eHit_None);
						hobj->SetPosition (pos);

						HandleUpdate (true);
						if (TrackNewObject (hobj, wPt))	// object created
						{
							AddUndoCreate (hobj);
							mLastObjectHit = hobj;	//mbs 10012010
							mLastObjectCreated = hobj;	//mbs 12052010
						}
						else
						{
							//mbs 18062010	clean up!
							hobj->SetSelected (false);
							PSObjList::iterator	iter = std::find (objects->begin(), objects->end(), static_cast <PSObject*> (hobj));
							if (iter != objects->end())
								objects->erase (iter);
							parent->AdjustOrder (eOrder_Deleted, 0);

							delete hobj;
							hobj = NULL;
							HandleUpdate (true);
						}
					}
					mDirty = true;
				}
////			else	// no section/group!
//					mToolI = eTool_Select;
				SaveUndo (0);
				if (hobj)
					PA_CallPluginAreaMethod (mCurParams);
			}
			hit = HitTest (where, hobj);
			AdjustCursor (hit);
		}
	}
}

#endif


#if	WINVER
extern "C" 
{
#if	!USE_MAC_API
typedef UInt16		MouseTrackingResult;
enum {
  kMouseTrackingMouseDown       = 1,
  kMouseTrackingMouseUp         = 2,
  kMouseTrackingMouseExited     = 3,
  kMouseTrackingMouseEntered    = 4,
  kMouseTrackingMouseDragged    = 5,
  kMouseTrackingKeyModifiersChanged = 6,
  kMouseTrackingUserCancelled   = 7,
  kMouseTrackingTimedOut        = 8,
  kMouseTrackingMouseMoved      = 9
};
typedef UInt32		OptionBits;
typedef double		EventTime;
typedef EventTime	EventTimeout;

// the (primary) mouse button is still pressed
static	Boolean	StillDown (void)
{
	const int	button = ::GetSystemMetrics (SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
	return (::GetAsyncKeyState (button) & 0x8000) != 0;
}
#endif


OSStatus __stdcall
TrackMouseLocationWithOptions(
  HWND                   inPort,             /* GrafPtr - can be NULL */
  OptionBits             inOptions,
  long					 inTimeout,			// not double in seconds, but long in ticks
  QDPoint *              outPt,
  UInt32 *               outModifiers,       /* can be NULL */
  MouseTrackingResult *  outResult);
Boolean		__stdcall	WaitMouseMoved (HWND inHwnd, QDPoint wPt);
}

static inline long Abs (long a)            
{
	return (((a) < 0) ? (-(a)) : (a));
}

Boolean	__stdcall WaitMouseMoved (HWND inHwnd, QDPoint startPt)
{
#define kDragMinDist		3
	long		startTime = ::timeGetTime();
	long		currentTime;
	POINT		currentPt;

	while (StillDown())
	{
		::GetCursorPos (&currentPt);
		::ScreenToClient (inHwnd, &currentPt);
		
		// distance threshold exceeded
		if (Abs (currentPt.x - startPt.h) > kDragMinDist || Abs (currentPt.y - startPt.v) > kDragMinDist)
			return true;
		
		currentTime = ::timeGetTime();
		
		// time threshold exceeded
		if (currentTime - startTime > ::GetDoubleClickTime())
			return true;
	}
	
	return false;	// mouse release without dragging
#undef kDragMinDist
}

OSStatus	__stdcall TrackMouseLocationWithOptions (
  HWND                   inPort,             /* GrafPtr - can be NULL */
  OptionBits             inOptions,
  long					 inTimeout,
  QDPoint *              outPt,
  UInt32 *               outModifiers,       /* can be NULL */
  MouseTrackingResult *  outResult)
{
	long		startTime = ::timeGetTime();
	long		currentTime;
	POINT		startPt, currentPt;

	UInt32	modifiers = GetModifiers();
	*outResult = kMouseTrackingMouseUp;
	::GetCursorPos (&startPt);
	while (StillDown())
	{
		::GetCursorPos (&currentPt);
		
		if (Abs (currentPt.x - startPt.x) > 0 || Abs (currentPt.y - startPt.y) > 0)
		{
			*outResult = kMouseTrackingMouseDragged;
			break;
		}
		UInt32	curModifiers = GetModifiers();
		if (curModifiers & activeFlag)
		{
			*outResult = kMouseTrackingUserCancelled;
			break;
		}
		if (modifiers != curModifiers)
		{
			*outResult = kMouseTrackingKeyModifiersChanged;
			break;
		}
		
		currentTime = ::timeGetTime();
		// time threshold exceeded
		if (currentTime - startTime > (inTimeout * 14))  // pB 2011 time are in ms, intimeout in ticks
		{
			*outResult = kMouseTrackingTimedOut;
			break;
		}
	}
	::GetCursorPos (&currentPt);	//mbs 29062011
	if (inPort)
		::ScreenToClient (inPort, &currentPt);
	outPt->h = currentPt.x;
	outPt->v = currentPt.y;
	if (outModifiers)
	{
		*outModifiers = 0;
		SHORT	ks = GetAsyncKeyState (VK_SHIFT);
		if (ks & 0x8000)
			*outModifiers |= shiftKey;
		ks = GetAsyncKeyState (VK_CONTROL);
		if (ks & 0x8000)
			*outModifiers |= controlKey;
		ks = GetAsyncKeyState (VK_MENU);
		if (ks & 0x8000)
			*outModifiers |= optionKey;
	}
	return noErr;
}
#endif


// ---------------------------------------------------------------------------
// TrackSelect													   [protected]
// ---------------------------------------------------------------------------

#if !__LP64__
bool
DMArea::TrackSelect (QDPoint wPt)
{
//	SPoint				where = MapToArea (wPt);
	QDPoint				pt;
	UInt32				modifiers;
	MouseTrackingResult	result;
	OSStatus			err = noErr;

	while (err == noErr)
	{
#if	MACVER
		err = ::TrackMouseLocationWithOptions (NULL, 0, 0.5, &pt, &modifiers, &result);
#else
		err = ::TrackMouseLocationWithOptions ((HWND) mAreaProperties.fWinHWND, 0, 30, &pt, &modifiers, &result);
#endif
		if (err != noErr || result == kMouseTrackingMouseUp || result == kMouseTrackingUserCancelled)
			break;
//		if (result == kMouseTrackingMouseDragged)
		{
			SPoint	now (GlobalToLocal(pt.h, pt.v));

			if (now.h >= wPt.h)
			{
				mTrackSelect.left = wPt.h;
				mTrackSelect.right = now.h;
			}
			else
			{
				mTrackSelect.left = now.h;
				mTrackSelect.right = wPt.h;
			}
			if (now.v >= wPt.v)
			{
				mTrackSelect.top = wPt.v;
				mTrackSelect.bottom = now.v;
			}
			else
			{
				mTrackSelect.top = now.v;
				mTrackSelect.bottom = wPt.v;
			}
			SRect	sr (mTrackSelect); // PB - mScrollPosScaled);
			sr = MapToArea (sr);
			HandleTrackSelect (sr, (modifiers & optionKey) == 0);
			Yield4D();
			HandleUpdate (true);
		}
	}

	mTrackSelect.SetRect (0, 0, 0, 0);
	mDirty = true;
	return (err == noErr && result == kMouseTrackingMouseUp);
}


// ---------------------------------------------------------------------------
// TrackNewObject												   [protected]
// ---------------------------------------------------------------------------

bool
DMArea::TrackNewObject (DMBase *inObj, QDPoint wPt)
{
//	bool	isSection = (inObj->GetKind() == eObject_Section);
//	int		isGuide = (inObj->GetKind() == eObject_Guide);
//	if (isGuide)
//		if (static_cast <DMGuide*> (inObj)->IsVertical())
//			isGuide++;
	SRect				opos = inObj->GetPosition();
	SRect				pos = opos;
	SPoint				where = MapToArea (wPt);
	QDPoint				pt;
	UInt32				modifiers;
	MouseTrackingResult	result;
	OSStatus			err = noErr;

	while (err == noErr)
	{
		if (mToolI == eTool_CreateLine)
		{
			long	lineFlags;
			if (pos.Width() > 1)
			{
				if (pos.Height() > 1)
					if ( ((pt.v >= wPt.v) && (pt.h >= wPt.h))
						|| ((pt.v < wPt.v) && (pt.h < wPt.h)) )
						lineFlags = RWLine_TopLeft;
					else
						lineFlags = RWLine_BottomLeft;
				else
					lineFlags = RWLine_Horizontal;
			}
			else
				lineFlags = RWLine_Vertical;
			RWValue	flg (lineFlags);
			inObj->SetProperty (PSObjPropFlags, flg);
		}
		inObj->SetPosition (pos);
		AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
		HandleUpdate (true);

#if	MACVER
		err = ::TrackMouseLocationWithOptions (NULL, 0, 0.5, &pt, &modifiers, &result);
#else
		err = ::TrackMouseLocationWithOptions ((HWND) mAreaProperties.fWinHWND, 0, 30, &pt, &modifiers, &result);
#endif
		if (err != noErr || result == kMouseTrackingMouseUp || result == kMouseTrackingUserCancelled)
			break;
//		if (result == kMouseTrackingMouseDragged)
		{
			SPoint	now (MapToArea (GlobalToLocal(pt.h, pt.v)));

			//mbs 22062010	snap
			SPoint	nowR (RoundUI (opos.left + now.h - where.h), RoundUI (opos.top + now.v - where.v));
			SnapPoint (inObj, nowR, false, false);
			if (nowR.h < opos.left)
			{
				pos.left = nowR.h;
				pos.right = opos.left;
			}
			else
				pos.right = nowR.h;
			if (nowR.v < opos.top)
			{
				pos.top = nowR.v;
				pos.bottom = opos.top;
			}
			else
				pos.bottom = nowR.v;

			/*
			if (pt.h >= wPt.h)
				pos.right = RoundUI (pos.left + now.h - where.h);
			else
				pos.right = pos.left;
			if (pt.v >= wPt.v)
				pos.bottom = RoundUI (pos.top + now.v - where.v);
			else if (mToolI == eTool_CreateLine)
			{
				pos.top = RoundUI (opos.top + now.v - where.v);
				pos.bottom = opos.top;
			}
			else
				pos.bottom = pos.top;
			 */

			if (modifiers & shiftKey)
			{
				// ••• TODO •••	make it a square
				// pB
				if((mToolI == eTool_CreateLine) && ((pos.right - pos.left) < ((pos.bottom - pos.top)/2)))
				{
					pos.right = pos.left;
				}
				else if ((pos.right - pos.left) < (pos.bottom - pos.top))
				{
					pos.bottom = pos.top + (pos.right - pos.left);	
				}
				else if ((mToolI == eTool_CreateLine) && ((pos.bottom - pos.top) < ((pos.right - pos.left)/2)))
				{
					pos.bottom = pos.top;
				}
				else 
				{
					pos.right = pos.left + (pos.bottom - pos.top);
				}

			}
			
			Yield4D();
		}
	}

	return (err == noErr && result == kMouseTrackingMouseUp);
}


void
DMArea::TrackMouse4Obj (DMBase *inObj, QDPoint wPt)
{
    SRect				opos = inObj->GetPosition();
    SRect				pos = opos;
    SPoint				where = MapToArea (wPt);
    QDPoint				pt;
    UInt32				modifiers;
    MouseTrackingResult	result;
    OSStatus			err = noErr;
    
    while (err == noErr)
    {
        if (mToolI == eTool_CreateLine)
        {
            long	lineFlags;
            if (pos.Width() > 1)
            {
                if (pos.Height() > 1)
                    if ( ((pt.v >= wPt.v) && (pt.h >= wPt.h))
                        || ((pt.v < wPt.v) && (pt.h < wPt.h)) )
                        lineFlags = RWLine_TopLeft;
                    else
                        lineFlags = RWLine_BottomLeft;
                    else
                        lineFlags = RWLine_Horizontal;
            }
            else
                lineFlags = RWLine_Vertical;
            RWValue	flg (lineFlags);
            inObj->SetProperty (PSObjPropFlags, flg);
        }
        inObj->SetPosition (pos);
        AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
        HandleUpdate (true);
        
#if	MACVER
        err = ::TrackMouseLocationWithOptions (NULL, 0, 0.5, &pt, &modifiers, &result);
#else
        err = ::TrackMouseLocationWithOptions ((HWND) mAreaProperties.fWinHWND, 0, 30, &pt, &modifiers, &result);
#endif
        if (err != noErr || result == kMouseTrackingMouseUp || result == kMouseTrackingUserCancelled)
            break;
        //		if (result == kMouseTrackingMouseDragged)
        {
            SPoint	now (MapToArea (GlobalToLocal(pt.h, pt.v)));
            
            //mbs 22062010	snap
            SPoint	nowR (RoundUI (opos.left + now.h - where.h), RoundUI (opos.top + now.v - where.v));
            SnapPoint (inObj, nowR, false, false);
            if (nowR.h < opos.left)
            {
                pos.left = nowR.h;
                pos.right = opos.left;
            }
            else
                pos.right = nowR.h;
            if (nowR.v < opos.top)
            {
                pos.top = nowR.v;
                pos.bottom = opos.top;
            }
            else
                pos.bottom = nowR.v;
            
            /*
             if (pt.h >= wPt.h)
             pos.right = RoundUI (pos.left + now.h - where.h);
             else
             pos.right = pos.left;
             if (pt.v >= wPt.v)
             pos.bottom = RoundUI (pos.top + now.v - where.v);
             else if (mToolI == eTool_CreateLine)
             {
             pos.top = RoundUI (opos.top + now.v - where.v);
             pos.bottom = opos.top;
             }
             else
             pos.bottom = pos.top;
             */
            
            if (modifiers & shiftKey)
            {
                // ••• TODO •••	make it a square
                // pB
                if((mToolI == eTool_CreateLine) && ((pos.right - pos.left) < ((pos.bottom - pos.top)/2)))
                {
                    pos.right = pos.left;
                }
                else if ((pos.right - pos.left) < (pos.bottom - pos.top))
                {
                    pos.bottom = pos.top + (pos.right - pos.left);	
                }
                else if ((mToolI == eTool_CreateLine) && ((pos.bottom - pos.top) < ((pos.right - pos.left)/2)))
                {
                    pos.bottom = pos.top;
                }
                else 
                {
                    pos.right = pos.left + (pos.bottom - pos.top);
                }
                
            }
            
            Yield4D();
        }
    }
    
}

#endif
// ---------------------------------------------------------------------------
// MakeProportional													   [protected]
// ---------------------------------------------------------------------------

void						
DMArea::MakeProportional (SRect &iRect, SPoint &ioPoint)
{
	if (iRect.Width() == 0)
	{
		ioPoint.v = 0;
		return;
	}
		
	if (iRect.Height() == 0) 
	{
		ioPoint.h = 0;
		return;

	}
	
	float	scale = iRect.Width() / iRect.Height();
	float	newHeight = (iRect.Height() + ioPoint.v);
	float	newWidth = (iRect.Width() + ioPoint.h);
	
	if ( fabs(newHeight * scale) < fabs (newWidth)) 
		newHeight = newWidth / scale;
	else 
		newWidth = newHeight * scale;
	
	ioPoint.v = newHeight - iRect.Height();
	ioPoint.h = newWidth - iRect.Width();
	
	return;
	
}

// ---------------------------------------------------------------------------
// TrackObjectXxx                                                       [protected]
// ---------------------------------------------------------------------------
#if __LP64__
void
DMArea::TrackObjectStart ()
{
    DMBase    *inObj = mLastDragObject;
    int       i, c = mSelectedObjects.size();

    //mbs 02082010    support undo for column/header resize
    bool    isTableCell = (inObj->GetKind() == eObject_TblHdr || inObj->GetKind() == eObject_TblCol);
    vector <DMBase*>    tables;
    if (isTableCell)
    {
        SaveUndo (10002, false);    // eUndo_MoveResizeObject
        for (i = 0; i < c; i++)
        {
            inObj = static_cast <DMBase*> (mSelectedObjects [i])->GetParent();
            if (tables.size() == 0 || std::find (tables.begin(), tables.end(), inObj) == tables.end())
            {
                AddUndoSnapshot (inObj, true);
                tables.push_back (inObj);
            }
        }
        SaveUndo (-1);    // eUndo_Pause
    }
    
}

void
DMArea::TrackObjectBody (SPoint inNow, double when)
{
    Boolean                 proportional;
    
    UInt32                  modifiers = mLastEventModifiers;
    int                     inHit = mTrackHit;
    DMBase                  *inObj = mLastDragObject;
    SPoint                  where = mLastAreaPos;
    int                     i, c = mSelectedObjects.size();
    bool                    isTableCell = (inObj->GetKind() == eObject_TblHdr || inObj->GetKind() == eObject_TblCol);

    bool                    isSection = (inObj->GetKind() == eObject_Section);
    int                     isGuide = (inObj->GetKind() == eObject_Guide);
    
    if (isGuide)
        if (static_cast <DMGuide*> (inObj)->IsVertical())
            isGuide++;
    
    std::vector <SRect>        opos (c);
    
    for (i = 0; i < c; i++)
    {
        inObj = static_cast <DMBase*> (mSelectedObjects [i]);
        opos [i] = inObj->GetPosition();
    }
            
    
    AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
                // HandleUpdate (true);
                
    SPoint    now = MapToArea (inNow);
    SPoint    delta (RoundUI (now.h - where.h), RoundUI (now.v - where.v));
    mLastAreaPos = now;
    
    if (isSection || isGuide == 1)
        delta.h = 0;
    else if (isGuide)
        delta.v = 0;
    
    proportional = false;
    if (modifiers & shiftKey)
    {
        // ••• TODO •••    make it a square
        // pB
        float deltaH = fabs ((float) delta.h);
        float deltaV = fabs ((float) delta.v);
        
        if (inHit == eHit_Object)
        {
            if (deltaV < deltaH)
                delta.v = 0;
            else
                delta.h = 0;
        } else if (inHit == eHit_BRH) //resize
            proportional = true;
    }
    
    for (i = 0; i < c; i++)
    {
        inObj = static_cast <DMBase*> (mSelectedObjects [i]);
        SRect    pos = opos [i];
        switch (inHit)
        {
            default:        break;    // shut up compiler
            case eHit_Object:
                pos += delta;
                break;
            case eHit_TLH:
                pos.top += delta.v;
                if (pos.top > pos.bottom)
                    pos.top = pos.bottom;
                pos.left += delta.h;
                if (pos.left > pos.right)
                    pos.left = pos.right;
                break;
            case eHit_TCH:
                pos.top += delta.v;
                if (pos.top > pos.bottom)
                    pos.top = pos.bottom;
                break;
            case eHit_TRH:
                pos.top += delta.v;
                if (pos.top > pos.bottom)
                    pos.top = pos.bottom;
                pos.right += delta.h;
                if (pos.left > pos.right)
                    pos.right = pos.left;
                break;
            case eHit_LCH:
                pos.left += delta.h;
                if (pos.left > pos.right)
                    pos.left = pos.right;
                break;
            case eHit_RCH:
                pos.right += delta.h;
                if (pos.left > pos.right)
                    pos.right = pos.left;
                break;
            case eHit_BLH:
                pos.bottom += delta.v;
                if (pos.top > pos.bottom)
                    pos.bottom = pos.top;
                pos.left += delta.h;
                if (pos.left > pos.right)
                    pos.left = pos.right;
                break;
            case eHit_BCH:
                pos.bottom += delta.v;
                if (pos.top > pos.bottom)
                    pos.bottom = pos.top;
                break;
            case eHit_BRH:
                if (proportional)
                    MakeProportional (pos, delta);
                pos.bottom += delta.v;
                if (pos.top > pos.bottom)
                    pos.bottom = pos.top;
                pos.right += delta.h;
                if (pos.left > pos.right)
                    pos.right = pos.left;
                break;
            case eHit_ResizeH:
                pos.right += delta.h;
                if (isGuide)
                    pos.left += delta.h;
                else if (pos.left > pos.right)
                    pos.right = pos.left;
                break;
            case eHit_ResizeV:
                pos.bottom += delta.v;
                if (isGuide)
                    pos.top += delta.v;
                else if (pos.top > pos.bottom)
                    pos.bottom = pos.top;
                break;
        }
        if (not isGuide && not isSection)
        {
            if ((modifiers & optionKey) == 0)
                SnapRect (inObj, pos, inHit);
            
        }
        // pB v 1.4.2 round coordinates after drag
        pos.top = RoundUI(pos.top);
        pos.left = RoundUI(pos.left);
        pos.bottom = RoundUI(pos.bottom);
        pos.right = RoundUI(pos.right);
        
        if (isTableCell)
        {
            RWValue    nv;
            if (inHit == eHit_ResizeH)    //mbs 04082010
            {
                nv.SetReal (pos.Width());
                inObj->SetProperty (PSObjPropPosWidth, nv);
            }
            else    // if (inHit == eHit_ResizeV)    //mbs 04082010
            {
                nv.SetReal (pos.Height());
                inObj->SetProperty (PSObjPropPosHeight, nv);
            }
        }
        else
            inObj->SetPosition (pos);
    }
}
                
    
void
DMArea::TrackObjectEnd (OSStatus err)
{
    DMBase                  *inObj = mLastDragObject;
    int                     i, c = mSelectedObjects.size();
    bool                    isTableCell = (inObj->GetKind() == eObject_TblHdr || inObj->GetKind() == eObject_TblCol);
    
    bool                    isSection = (inObj->GetKind() == eObject_Section);
    int                     isGuide = (inObj->GetKind() == eObject_Guide);
    
    if (isGuide)
        if (static_cast <DMGuide*> (inObj)->IsVertical())
            isGuide++;
    
    std::vector <SRect>        opos (c);

    if (err != noErr)
    {
        for (i = 0; i < c; i++)
        {
            inObj = static_cast <DMBase*> (mSelectedObjects [i]);
            inObj->SetPosition (opos [i]);
        }
        if (isTableCell)
        {
            SaveUndo (0);
            mUndoBuffer.Undo (this, true);
        }
    }
    else
    {
        {
            SaveUndo (10002, true);    // eUndo_MoveResizeObject
            RWValue    v;
            for (i = 0; i < c; i++)
            {
                inObj = static_cast <DMBase*> (mSelectedObjects [i]);
                SRect    r = inObj->GetPosition();
                v.SetText (r.ToString());
                RWValue    ov;
                ov.SetText (opos [i].ToString());
                AddUndoProperty (inObj, PSObjPropRect, ov, v);
                inObj->SetProperty (PSObjPropRect, v);
            }
            SaveUndo (0);
        }
    }
 }

// ---------------------------------------------------------------------------
// TrackNewObjectXxx                                                   [protected]
// ---------------------------------------------------------------------------
void
DMArea::TrackNewObjectStart ()
{
    mTrackEvent = 3;
}
void
DMArea::TrackNewObjectBody (SPoint inNow, double when)
{
  
    DMBase                  *inObj = mLastDragObject;
    SRect                   opos = inObj->GetPosition();
    SRect                   pos = opos;
    UInt32                  modifiers = mLastEventModifiers;
    int                     inHit = mTrackHit;
    SPoint                  where = mLastAreaPos;
    SPoint                  now = MapToArea (inNow);

    if (mToolI == eTool_CreateLine)
    {
        long    lineFlags;
        if (pos.Width() > 1)
        {
            if (pos.Height() > 1)
                if ( ((where.v >= now.v) && (where.h >= now.h))
                    || ((where.v < now.v) && (where.h < now.h)) )
                    lineFlags = RWLine_TopLeft;
                else
                    lineFlags = RWLine_BottomLeft;
                else
                    lineFlags = RWLine_Horizontal;
        }
        else
            lineFlags = RWLine_Vertical;
        RWValue    flg (lineFlags);
        inObj->SetProperty (PSObjPropFlags, flg);
    }
    inObj->SetPosition (pos);
    AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
    //  HandleUpdate (true);
    
    
    //mbs 22062010    snap
    SPoint    nowR (RoundUI (opos.left + now.h - where.h), RoundUI (opos.top + now.v - where.v));
    SnapPoint (inObj, nowR, false, false);
    if (nowR.h < opos.left)
    {
        pos.left = nowR.h;
        pos.right = opos.left;
    }
    else
        pos.right = nowR.h;
    
    if (nowR.v < opos.top)
    {
        pos.top = nowR.v;
        pos.bottom = opos.top;
    }
    else
        pos.bottom = nowR.v;
    
    if (modifiers & shiftKey)
    {
        // ••• TODO •••    make it a square
        // pB
        if((mToolI == eTool_CreateLine) && ((pos.right - pos.left) < ((pos.bottom - pos.top)/2)))
        {
            pos.right = pos.left;
        }
        else if ((pos.right - pos.left) < (pos.bottom - pos.top))
        {
            pos.bottom = pos.top + (pos.right - pos.left);
        }
        else if ((mToolI == eTool_CreateLine) && ((pos.bottom - pos.top) < ((pos.right - pos.left)/2)))
        {
            pos.bottom = pos.top;
        }
        else
        {
            pos.right = pos.left + (pos.bottom - pos.top);
        }
        
    }
    inObj->SetPosition (pos);
    AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
    HandleUpdate (true);

}

void
DMArea::TrackNewObjectEnd(int err)
{
    DMBase                  *hobj = mLastDragObject;
    SPoint                  where = mLastAreaPos;

      if (err == 0)    // object created
      {
          AddUndoCreate (hobj);
          mLastObjectHit = hobj;    //mbs 10012010
          mLastObjectCreated = hobj;    //mbs 12052010
      }
      else
      {
      //mbs 18062010    clean up!
          DMBase    *parent = GetParentAt (where);
          if (parent)
          {
              PSObjListD    *objects = NULL;
              parent->GetObjects (PSObjPropObjects, objects);
          
              hobj->SetSelected (false);
              PSObjList::iterator    iter = std::find (objects->begin(), objects->end(), static_cast <PSObject*> (hobj));
              if (iter != objects->end())
              objects->erase (iter);
              parent->AdjustOrder (eOrder_Deleted, 0);
          }
          delete hobj;
          hobj = NULL;
          HandleUpdate (true);
      }
}
#endif

// ---------------------------------------------------------------------------
// TrackObject													   [protected]
// ---------------------------------------------------------------------------
#if !__LP64__
bool
DMArea::TrackObject (DMBase *inObj, QDPoint wPt, int inHit)
{
#if	MACVER
	if (inHit != eHit_None && ::WaitMouseMoved (wPt))
#else
	if (inHit != eHit_None && ::WaitMouseMoved ((HWND) mAreaProperties.fWinHWND, wPt))
#endif
	{
		bool	isSection = (inObj->GetKind() == eObject_Section);
		int		isGuide = (inObj->GetKind() == eObject_Guide);
		if (isGuide)
			if (static_cast <DMGuide*> (inObj)->IsVertical())
				isGuide++;
		SPoint	where = MapToArea (wPt);
		int		i, c = mSelectedObjects.size();
		std::vector <SRect>	opos (c);
		for (i = 0; i < c; i++)
		{
			inObj = static_cast <DMBase*> (mSelectedObjects [i]);
			opos [i] = inObj->GetPosition();
		}

		//mbs 02082010	support undo for column/header resize
		vector <DMBase*>	tables;
		bool	isTableCell = (inObj->GetKind() == eObject_TblHdr || inObj->GetKind() == eObject_TblCol);
		if (isTableCell)
		{
			SaveUndo (10002, false);	// eUndo_MoveResizeObject
			for (i = 0; i < c; i++)
			{
				inObj = static_cast <DMBase*> (mSelectedObjects [i])->GetParent();
				if (tables.size() == 0 || std::find (tables.begin(), tables.end(), inObj) == tables.end())
				{
					AddUndoSnapshot (inObj, true);
					tables.push_back (inObj);
				}
			}
			SaveUndo (-1);	// eUndo_Pause
		}

		QDPoint				pt;
		UInt32				modifiers;
		MouseTrackingResult	result;
		OSStatus			err = noErr;
		Boolean				proportional;
		
		while (err == noErr)
		{
			AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
			HandleUpdate (true);
			
#if	MACVER
			err = ::TrackMouseLocationWithOptions (NULL, 0, 0.5, &pt, &modifiers, &result);
#else
			err = ::TrackMouseLocationWithOptions ((HWND) mAreaProperties.fWinHWND, 0, 30, &pt, &modifiers, &result);
#endif
			if (err != noErr || result == kMouseTrackingMouseUp || result == kMouseTrackingUserCancelled)
				break;
//			if (result == kMouseTrackingMouseDragged)
			{
				SPoint	now (MapToArea (GlobalToLocal(pt.h, pt.v)));
 				SPoint	delta (RoundUI (now.h - where.h), RoundUI (now.v - where.v));

				if (isSection || isGuide == 1)
					delta.h = 0;
				else if (isGuide)
					delta.v = 0;
				
				proportional = false;
				if (modifiers & shiftKey)
				{
					// ••• TODO •••	make it a square
					// pB
					float deltaH = fabs ((float) delta.h);
					float deltaV = fabs ((float) delta.v);
					
					if (inHit == eHit_Object)
					{
						if (deltaV < deltaH)
							delta.v = 0;	
						else 
							delta.h = 0;
					} else if (inHit == eHit_BRH) //resize
						proportional = true;				
				}
				
				for (i = 0; i < c; i++)
				{
					inObj = static_cast <DMBase*> (mSelectedObjects [i]);
					SRect	pos = opos [i];
					switch (inHit)
					{
						default:		break;	// shut up compiler
						case eHit_Object:
							pos += delta;
							break;
						case eHit_TLH:
							pos.top += delta.v;
							if (pos.top > pos.bottom)
								pos.top = pos.bottom;
							pos.left += delta.h;
							if (pos.left > pos.right)
								pos.left = pos.right;
							break;
						case eHit_TCH:
							pos.top += delta.v;
							if (pos.top > pos.bottom)
								pos.top = pos.bottom;
							break;
						case eHit_TRH:
							pos.top += delta.v;
							if (pos.top > pos.bottom)
								pos.top = pos.bottom;
							pos.right += delta.h;
							if (pos.left > pos.right)
								pos.right = pos.left;
							break;
						case eHit_LCH:
							pos.left += delta.h;
							if (pos.left > pos.right)
								pos.left = pos.right;
							break;
						case eHit_RCH:
							pos.right += delta.h;
							if (pos.left > pos.right)
								pos.right = pos.left;
							break;
						case eHit_BLH:
							pos.bottom += delta.v;
							if (pos.top > pos.bottom)
								pos.bottom = pos.top;
							pos.left += delta.h;
							if (pos.left > pos.right)
								pos.left = pos.right;
							break;
						case eHit_BCH:
							pos.bottom += delta.v;
							if (pos.top > pos.bottom)
								pos.bottom = pos.top;
							break;
						case eHit_BRH:
							if (proportional)
								MakeProportional (pos, delta);
							pos.bottom += delta.v;
							if (pos.top > pos.bottom)
								pos.bottom = pos.top;
							pos.right += delta.h;
							if (pos.left > pos.right)
								pos.right = pos.left;
							break;
						case eHit_ResizeH:
							pos.right += delta.h;
							if (isGuide)
								pos.left += delta.h;
							else if (pos.left > pos.right)
								pos.right = pos.left;
							break;
						case eHit_ResizeV:
							pos.bottom += delta.v;
							if (isGuide)
								pos.top += delta.v;
							else if (pos.top > pos.bottom)
								pos.bottom = pos.top;
							break;
					}
					if (not isGuide && not isSection)
					{
						if ((modifiers & optionKey) == 0)
							SnapRect (inObj, pos, inHit);
						
					}
                    // pB v 1.4.2 round coordinates after drag
                    pos.top = RoundUI(pos.top);
                    pos.left = RoundUI(pos.left);
                    pos.bottom = RoundUI(pos.bottom);
                    pos.right = RoundUI(pos.right);
                    
					if (isTableCell)
					{
						RWValue	nv;
						if (inHit == eHit_ResizeH)	//mbs 04082010
						{
							nv.SetReal (pos.Width());
							inObj->SetProperty (PSObjPropPosWidth, nv);
						}
						else	// if (inHit == eHit_ResizeV)	//mbs 04082010
						{
							nv.SetReal (pos.Height());
							inObj->SetProperty (PSObjPropPosHeight, nv);
						}
					}
					else
						inObj->SetPosition (pos);
				}
				Yield4D();
			}

		}

		if (err != noErr || result != kMouseTrackingMouseUp)
		{
			for (i = 0; i < c; i++)
			{   
				inObj = static_cast <DMBase*> (mSelectedObjects [i]);
				inObj->SetPosition (opos [i]);
			}
			if (isTableCell)
			{
				SaveUndo (0);
				mUndoBuffer.Undo (this, true);
			}
			return false;
		}
		else
		{
			if (isTableCell)
			{
				SaveUndo (-2);
				for (i = tables.size() - 1; i >= 0; i--)
				{
					inObj = tables [i];
					AddUndoSnapshot (inObj, false);
				}
				SaveUndo (0);
			}
			else
			{
				SaveUndo (10002, true);	// eUndo_MoveResizeObject
				RWValue	v;
				for (i = 0; i < c; i++)
				{
					inObj = static_cast <DMBase*> (mSelectedObjects [i]);
					SRect	r = inObj->GetPosition();
					v.SetText (r.ToString());
					RWValue	ov;
					ov.SetText (opos [i].ToString());
					AddUndoProperty (inObj, PSObjPropRect, ov, v);
					inObj->SetProperty (PSObjPropRect, v);
				}
				SaveUndo (0);
			}
		}
		return true;	// (err == noErr && result == kMouseTrackingMouseUp);
	}
	
	return false;
}

// ---------------------------------------------------------------------------
// TrackNewGuide												   [protected]
// ---------------------------------------------------------------------------
void
DMArea::TrackNewGuide (QDPoint wPt, bool inVertical)
{
	SPoint		where = MapToArea (wPt);
	DMBase	 	*hobj = NULL;
	OSStatus	err = noErr;

	SaveUndo (10001, true);	// eUndo_NewObject
	mDirty = true;
//	DeselectAll();
				
	hobj = CreateObject (inVertical? PSObjPropOGuideV: PSObjPropOGuideH, (DMBase*) 0, RWXmlNode());
	if (hobj)
	{
		mShowGuides = true;
		hobj->SetOrder (mGuides.size());
		SPoint	pos (RoundUI (where.h - mEditorRect.left - mScrollPosScaled.h), RoundUI (where.v - mEditorRect.top - mScrollPosScaled.v));
		SPoint	opos (pos);

		MouseTrackingResult	result;
		while (err == noErr)
		{
			QDPoint		pt;
			UInt32		modifiers;
			RWValue		value (inVertical? pos.h: pos.v);
			hobj->SetProperty (PSObjPropData, value);
			AdjustDrawingPosition (mScreen, mEditorRect.TopLeft() + mScrollPosScaled);
			HandleUpdate (true);

#if	MACVER
			err = ::TrackMouseLocationWithOptions (NULL, 0, 0.5, &pt, &modifiers, &result);
#else
			err = ::TrackMouseLocationWithOptions ((HWND) mAreaProperties.fWinHWND, 0, 30, &pt, &modifiers, &result);
#endif
			if (err != noErr || result == kMouseTrackingMouseUp || result == kMouseTrackingUserCancelled)
				break;
//			if (result == kMouseTrackingMouseDragged)
			{
				SPoint	now (MapToArea (GlobalToLocal(pt.h, pt.v)));

				pos.h = RoundUI (opos.h + now.h - where.h);
				pos.v = RoundUI (opos.v + now.v - where.v);
				
				Yield4D();
			}
		}

		if (err == noErr && result == kMouseTrackingMouseUp)
		{
			AddUndoCreate (hobj);
			mLastObjectHit = hobj;
			mLastObjectCreated = hobj;
		}
		else
		{
			// clean up!
			hobj->SetSelected (false);
			PSObjList::iterator	iter = std::find (mGuides.begin(), mGuides.end(), static_cast <PSObject*> (hobj));
			if (iter != mGuides.end())
				mGuides.erase (iter);

			delete hobj;
			hobj = NULL;
			HandleUpdate (true);
		}
		mDirty = true;

		SaveUndo (0);
		if (hobj)
			PA_CallPluginAreaMethod (mCurParams);
	}
	AdjustCursor (HitTest (where, hobj));

}
#endif

// ---------------------------------------------------------------------------
// SnapPoint													   [protected]
// ---------------------------------------------------------------------------

void
DMArea::SnapPoint (DMBase *inObj, SPoint &ioPoint, bool h, bool v)
const
{
	SRect	pos;
	float	p;
//	bool	h = false;
//	bool	v = false;

	//mbs 05082010	at first, convert that point to report coordinates
	SPoint	delta (inObj->GetParent()->GetDrawPosition().TopLeft() - mEditorRect.TopLeft() + mScrollPosScaled);
	ioPoint = ioPoint + delta;

	// first snap to guides
	if (mSnapToGuide /* && mShowGuides */ && mGuides.size() > 0)
	{
		PSObjList::const_iterator	iter;
		for (iter = mGuides.begin(); iter != mGuides.end(); iter++)
		{
			DMGuide *guide = static_cast <DMGuide*> (*iter);
			pos = guide->GetPosition();
			if (guide->IsVertical())
			{
				p = ioPoint.h - pos.left;
				if (fabs (p) < sSnapUI)
				{
					h = true;
					ioPoint.h = pos.left;
				}
			}
			else
			{
				p = ioPoint.v - pos.top;
				if (fabs (p) < sSnapUI)
				{
					v = true;
					ioPoint.v = pos.top;
				}
			}
		}
	}

	// snap to grid
	if (mSnapToGrid /* && mShowGrid */ && mGridSize > 1 && (not h || not v))
	{
		SPoint	pt (ioPoint);
		if (not h)
		{
			p = long (pt.h / mGridSize) * mGridSize;
			if (pt.h - p < sSnapUI)
			{
				pt.h = p;
				h = true;
			}
			else if (p - pt.h + mGridSize < sSnapUI)
			{
				pt.h = p + mGridSize;
				h = true;
			}
		}
		if (not v)
		{
			p = long (pt.v / mGridSize) * mGridSize;
			if (pt.v - p < sSnapUI)
			{
				pt.v = p;
				v = true;
			}
			else if (p - pt.v + mGridSize < sSnapUI)
			{
				pt.v = p + mGridSize;
				v = true;
			}
		}
		if (h && v)
			ioPoint = pt;
	}

	ioPoint = ioPoint - delta;
	return;
}


// ---------------------------------------------------------------------------
// SnapRect														   [protected]
// ---------------------------------------------------------------------------

void
DMArea::SnapRect (DMBase *inObj, SRect &ioRect, int inHit)
const
{
	if (mSnapToGuide || mSnapToGrid)
	{
		bool	top = false, left = false, bottom = false, right = false;
		switch (inHit)
		{
			default:			break;	// shut up compiler
			case eHit_None:		top = left = bottom = right = true; break;	// object creation
			case eHit_Object:	top = left /* = bottom = right */ = true; break;
			case eHit_TLH:		top = left = true; break;
			case eHit_TCH:		top = true; break;
//			case eHit_TRH:		top = right = true; break;
			case eHit_LCH:		left = true; break;
			case eHit_RCH:		right = true; break;
//			case eHit_BLH:		left = bottom = true; break;
			case eHit_BCH:		bottom = true; break;
			case eHit_BRH:		bottom = right = true; break;
			case eHit_ResizeH:	right = true; break;
			case eHit_ResizeV:	bottom = true; break;
		}
		if (inHit == eHit_TRH)
		{
			SPoint	tr (ioRect.right, ioRect.top);
			SnapPoint (inObj, tr, false, false);
			ioRect.right = tr.h;
			ioRect.top = tr.v;
		}
		else if (inHit == eHit_BLH)
		{
			SPoint	bl (ioRect.left, ioRect.bottom);
			SnapPoint (inObj, bl, false, false);
			ioRect.left = bl.h;
			ioRect.bottom = bl.v;
		}
		else
		{
			if (top || left)
			{
				SPoint	tl (ioRect.TopLeft());
				SnapPoint (inObj, tl, not left, not top);
				if (left)
				{
					if (inHit == eHit_Object)	// prevent resizing
						ioRect.right += tl.h - ioRect.left;
					ioRect.left = tl.h;
				}
				if (top)
				{
					if (inHit == eHit_Object)	// prevent resizing
						ioRect.bottom += tl.v - ioRect.top;
					ioRect.top = tl.v;
				}
			}
			if (bottom || right)
			{
				SPoint br (ioRect.BottomRight());
				SnapPoint (inObj, br, not right, not bottom);
				if (right)
					ioRect.right = br.h;
				if (bottom)
					ioRect.bottom = br.v;
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// HandleScroll														  [public]
// ---------------------------------------------------------------------------
// called from scrollbar handler

void
DMArea::HandleScroll (UIScrollBar *which, SInt32 amount)
{
	if (amount == 0)
		return;

	if (mScrollH)
		mScrollPos.h = mScrollH->GetValue();
	if (mScrollV)
		mScrollPos.v = mScrollV->GetValue();
	ScaleChanged();
    mCurEvent = eAE_MouseDown;
    HandleUpdate (true);

	return;
}

// ---------------------------------------------------------------------------
// OnBeginDrag													   [protected]
// ---------------------------------------------------------------------------

void
DMArea::OnBeginDrag (void)
{
//	PA_DragContextRef	dragContext = PA_GetDragAndDropContext (mCurParams);
//	PA_PasteboardRef	pasteBoard = PA_GetDragAndDropPasteboard (dragContext);
}


// ---------------------------------------------------------------------------
// OnAllowDrop													   [protected]
// ---------------------------------------------------------------------------

void
DMArea::OnAllowDrop (void)
{
	PA_AllowDrop (mCurParams, true);
}


// ---------------------------------------------------------------------------
// OnDrag														   [protected]
// ---------------------------------------------------------------------------

void
DMArea::OnDrag (void)
{
	PA_Rect rect;
	short	posX, posY;
	int		hit;
	SPoint	pt;
	
	PA_GetDragPositions (mCurParams, &rect, &posX, &posY);
	PA_CustomizeDragOver (mCurParams);
	pt = GlobalToLocal (posX, posY);
				
	if (pt.IsContained (mEditorRect))
	{
		SPoint		where = MapToArea (pt);
		DMBase *	hobj = NULL;
	
		hit = HitTest (where, hobj);
		mLastDragObject = hobj;
		mLastDragPos = where - mDrawRect.TopLeft();
#if MACVER
		mLastDragTick = ::TickCount ();
#else
		mLastDragTick = ::GetTickCount();
#endif
		mIsDrag = true;
	}
	// HandleUpdate (true);
}


// ---------------------------------------------------------------------------
// OnDrop														   [protected]
// ---------------------------------------------------------------------------

void
DMArea::OnDrop (void)
{
	// all variables are set from previous eAE_Drag - 
	mIsDrag = false;
}

// ---------------------------------------------------------------------------
// GlobalToLocal													  [public]
// ---------------------------------------------------------------------------

SPoint
DMArea::GlobalToLocal (short inX, short inY)
const
{
#if	MACVER 
#if !__LP64__
	//	Point	qdPt;
	//	qdPt.h = inX;
	//	qdPt.v = inY;
	//	QDGlobalToLocalPoint (GetWindowPort ((WindowRef) mAreaProperties.fMacWindow), &qdPt);
	//	SPoint	qWhere (qdPt.h, qdPt.v);
	HIViewRef	contentView = 0;
	HIViewFindByID (HIViewGetRoot ((WindowRef) mAreaProperties.fMacWindow), kHIViewWindowContentID, &contentView);
	HIPoint	hPt = CGPointMake (inX, inY);
	HIPointConvert (&hPt, kHICoordSpace72DPIGlobal, NULL, kHICoordSpaceView, contentView);
	return SPoint (hPt.x, hPt.y);
#else
	// 64 bit: 4D passes the NSWindow. Global (Carbon) coordinates have their origin at the top
	// left of the main screen, Cocoa screen coordinates at its bottom left; the result is
	// relative to the top left of the window's content view, like the HIView code above.
	id	window = (id) mAreaProperties.fMacWindow;
	if (window == nil || ![window isKindOfClass: [NSWindow class]] || [[NSScreen screens] count] == 0)
		return SPoint (inX, inY);
	const CGFloat	mainHeight = NSMaxY ([[[NSScreen screens] objectAtIndex: 0] frame]);
	NSPoint			p = [(NSWindow*) window convertPointFromScreen: NSMakePoint (inX, mainHeight - inY)];
	NSView			*content = [(NSWindow*) window contentView];
	if (content != nil)
	{
		p = [content convertPoint: p fromView: nil];
		if (![content isFlipped])
			p.y = NSHeight ([content bounds]) - p.y;
	}
	return SPoint (p.x, p.y);
#endif
#else
	POINT	pt;
	pt.x = inX;
	pt.y = inY;
	::ScreenToClient ((HWND) mAreaProperties.fWinHWND, &pt);
	return SPoint ((int) pt.x, (int) pt.y);
#endif
}

#if	WINVER
#if	kUSE_FAKE_AREA
// Our child window ("fake area") passes mouse and key messages on to 4D. The old 32 bit
// build linked 4D's ASI_ functions from ASINTPPC.lib, which has no 64 bit version; they
// are looked up in the 4D executable at run time instead. When 4D does not export them,
// the messages go to 4D's window (the parent), mouse coordinates converted.
namespace
{
	typedef	LONG	(__stdcall *ASIMessageProc) (HWND, UINT, WPARAM, LPARAM);
	typedef	void	(__stdcall *ASIKeyProc) (HWND, UINT, WPARAM, LPARAM);

	template <class Proc>
	Proc	HostProc (const char *inName)
	{
		return reinterpret_cast <Proc> (::GetProcAddress (::GetModuleHandleW (NULL), inName));
	}

	LRESULT	ForwardToParent (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool inClientCoordinates)
	{
		HWND	parent = ::GetParent (hWnd);
		if (parent == NULL)
			return 0;
		if (inClientCoordinates)
		{
			POINT	pt = { (LONG) (short) LOWORD (lParam), (LONG) (short) HIWORD (lParam) };
			::MapWindowPoints (hWnd, parent, &pt, 1);
			lParam = MAKELPARAM (pt.x, pt.y);
		}
		return ::SendMessageW (parent, message, wParam, lParam);
	}

	LONG	ASI_EventMouse (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static const ASIMessageProc	proc = HostProc<ASIMessageProc> ("ASI_EventMouse");
		if (proc)
			return proc (hWnd, message, wParam, lParam);
		return (LONG) ForwardToParent (hWnd, message, wParam, lParam, message != WM_SETCURSOR);	// WM_SETCURSOR: no coordinates
	}

	LONG	ASI_NCMessage (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static const ASIMessageProc	proc = HostProc<ASIMessageProc> ("ASI_NCMessage");
		if (proc)
			return proc (hWnd, message, wParam, lParam);
		return (LONG) ForwardToParent (hWnd, message, wParam, lParam, false);	// screen coordinates
	}

	void	ASI_EventKey (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static const ASIKeyProc	proc = HostProc<ASIKeyProc> ("ASI_EventKey");
		if (proc)
			proc (hWnd, message, wParam, lParam);
		else
			ForwardToParent (hWnd, message, wParam, lParam, false);
	}
}
#endif

LONG APIENTRY
DMArea::AreaWndProc (HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

#if	kUSE_FAKE_AREA	// from MyWinExt.c
	switch(iMessage)
	{
/*	we never get this message...
		case WM_MOUSEWHEEL:
		{
			long APIENTRY fakeAreaWheelEvtHndlr (ListDataHandle area, HWND hWnd, WPARAM wParam, LPARAM lParam);
			AreaPrivateData* pdata = (AreaPrivateData*)::GetWindowLongPtrW (hWnd, GWLP_USERDATA);
			if (pdata)
			{
				ListDataHandle 	listHandle = (ListDataHandle) pdata->aref;
				lResult = fakeAreaWheelEvtHndlr (listHandle, hWnd, wParam, lParam);
			}
			else
				lResult = FALSE;
			break;
		}
*/

		case WM_SETCURSOR:
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDBLCLK: // ++
		case WM_RBUTTONDBLCLK: // ++
		case WM_MBUTTONDOWN: // ++
		case WM_MBUTTONUP: // ++
		case WM_MBUTTONDBLCLK: // ++
		case WM_XBUTTONDOWN: // ++
		case WM_XBUTTONUP: // ++
		case WM_XBUTTONDBLCLK: // ++
			ASI_EventMouse(hWnd, iMessage, wParam, lParam);
			lResult = ::DefWindowProc(hWnd, iMessage, wParam, lParam);
			lResult = 0;
			break;

		case WM_NCHITTEST:
		{
			// handle mNoHitTest "hole" for editing in 4D
			DMArea	*a = reinterpret_cast <DMArea*> (::GetWindowLongPtrW (hWnd, GWLP_USERDATA));
			if (a != NULL && not a->mNoHitTest.IsEmpty())
			{
				POINT	pt;
				HWND	win = ::GetParent (hWnd);	// hWnd is our fake area

				pt.x = (LONG) (short) LOWORD (lParam);
				pt.y = (LONG) (short) HIWORD (lParam);
				::ScreenToClient (win, &pt);
				if (SPoint (float (pt.x), float (pt.y)).IsContained (a->mNoHitTest))
				{
//?!?				ASI_NCMessage (hWnd, iMessage, wParam, lParam);
					return HTTRANSPARENT;
				}
			}

			// FALL THROUGH
		}

		case WM_NCLBUTTONDOWN:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCLBUTTONUP:
		case WM_NCMOUSEMOVE: // ++
		case WM_NCDESTROY: // ++
		case WM_NCACTIVATE:
		case WM_NCCALCSIZE:
		case WM_NCPAINT:
		case WM_NCRBUTTONDOWN:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCRBUTTONUP: // ++
		case WM_NCMBUTTONDOWN: // ++
		case WM_NCMBUTTONUP: // ++
		case WM_NCMBUTTONDBLCLK: // ++
			ASI_NCMessage(hWnd, iMessage, wParam, lParam);
			lResult = ::DefWindowProc (hWnd, iMessage, wParam, lParam);
			break;

		case WM_NCCREATE: // moved to call asi
			return TRUE;
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			ASI_EventKey (hWnd, iMessage, wParam, lParam );
			lResult = ::DefWindowProc (hWnd, iMessage, wParam, lParam);
			break;

		case WM_PAINT:
		{
#if	1
			DMArea	*a = reinterpret_cast <DMArea*> (::GetWindowLongPtrW (hWnd, GWLP_USERDATA));
			if (a != NULL)
				a->HandleUpdate (true);
			lResult = ::DefWindowProc (hWnd, iMessage, wParam, lParam);
#else
			if (ASI_EventUpdate (hWnd))
				lResult = 0;
			else
				lResult = ::DefWindowProc (hWnd, iMessage, wParam, lParam);
#endif
			break;
		}

	/*	case WM_ENTERIDLE:
			ASI_EnterIdle(hWnd, iMessage, wParam, lParam );
			lResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
			break;*/

	/*	case WM_ACTIVATE:
		case WM_CHILDACTIVATE:
		case WM_MDIACTIVATE:
		case WM_MOUSEACTIVATE:
			ASI_EventActivate(hWnd, iMessage, wParam, lParam);
			lResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
			break;*/

	/*	case WM_COMMAND:
			ASI_EventCommand(hWnd, iMessage, wParam, lParam);
			lResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
				{
				AreaPrivateData* pdata = (AreaPrivateData*)::GetWindowLongPtrW (hWnd, GWLP_USERDATA);

				if (pdata && pdata->proc)
					pdata->proc(pdata->aref, (void*)lParam);
				}
			break;*/

	/*	case WM_MENUSELECT:
			if (!ASI_MenuSelect( hWnd, iMessage, wParam, lParam))
				lResult = 0;
			else
				lResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
			break;*/

	/*	case WM_ERASEBKGND:
		case WM_SIZE:
		case WM_MOVE:
		case WM_HSCROLL:
		case WM_VSCROLL:
			ASI_WindChange(hWnd, iMessage, wParam, lParam);
			lResult = DefWindowProc(hWnd, iMessage, wParam, lParam); // never!!
			break;*/

		case WM_HSCROLL:
		case WM_VSCROLL:
		{
			HWND	hitControl = (HWND)lParam;
			UIScrollBar	*sb = reinterpret_cast <UIScrollBar*> (::GetWindowLongPtrW (hitControl, GWLP_USERDATA));
			if (sb != NULL)
			{
				sb->ScrollProc (LOWORD (wParam));
				lResult = 0;
				break;
			}
			// FALL THROUGH
		}

		default:
			lResult = ::DefWindowProc (hWnd, iMessage, wParam, lParam);
			break;
	}

#else

	WndProcMap::key_type	key (hWnd);
	WndProcMap::const_iterator	it = sWndProcMap.find (key);
	if (it == sWndProcMap.end())
		lResult = ::DefWindowProc (hWnd, iMessage, wParam, lParam);
	else
	{
		switch (iMessage)
		{
			case WM_HSCROLL:
			case WM_VSCROLL:
			{
				HWND	hitControl = (HWND)lParam;
				UIScrollBar	*sb = reinterpret_cast <UIScrollBar*> (::GetWindowLongPtrW (hitControl, GWLP_USERDATA));
				if (sb != NULL)
				{
					sb->ScrollProc (LOWORD (wParam));
					lResult = 0;
					break;
				}
				// FALL THROUGH
			}

			default:
			    return ::CallWindowProc ((WNDPROC) it->second.oldProc, hWnd, iMessage, wParam, lParam);
		}
	}
#endif

	return lResult;
}

#if	!kUSE_FAKE_AREA

DMArea::WndProcMap	DMArea::sWndProcMap;

void
DMArea::RegisterProc (void)
{
	WndProcMap::key_type	key ((HWND) mAreaProperties.fWinHWND);
	WndProcMap::iterator	it = sWndProcMap.find (key);
	if (it == sWndProcMap.end())
	{
		wndProcMap	v;
		v.oldProc = (WNDPROC) ::SetWindowLongPtrW (key, GWLP_WNDPROC, (LONG_PTR) AreaWndProc);
		v.refCount = 1;
		WndProcMap::value_type	value (key, v);
		sWndProcMap.insert (value);
	}
	else
	{
		it->second.refCount++;
	}
}

void
DMArea::UnRegisterProc (void)
{
	WndProcMap::key_type	key ((HWND) mAreaProperties.fWinHWND);
	WndProcMap::iterator	it = sWndProcMap.find (key);
	if (it == sWndProcMap.end())
	{
		// ?!?
	}
	else
	{
		wndProcMap	&v = it->second;
		v.refCount--;
		if (v.refCount == 0)
		{
			sWndProcMap.erase (it);
		}
	}
}

#endif

#endif
