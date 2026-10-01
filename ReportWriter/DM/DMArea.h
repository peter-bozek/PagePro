/*
 *  DMArea.h
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 01.11.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o. All rights reserved.
 *
 */

#ifndef	_DMArea_h_
#define	_DMArea_h_

#include <string>

# include	"DMReport.h"
# include	"4DPluginAPI.h"
// using namespace	FourDAPIEx;

#if	MACVER
#  include	"RWCTPageComposer.h"		// CoreGraphics + CoreText
#else
# include	"RWWinPageComposer.h"
# define	kUSE_FAKE_AREA	1
# define	kUSE_OFFSCREEN	1
# define	kUSE_CLIPMODE	1
#endif

# include	"UIScrollBar.h"
# include	"DMUndo.h"

// DM Area
class	DMArea
	:	public	DMReport,
		public	UIScrollClient
{
public:
	enum	ETool
	{
		eTool_Select,
		eTool_CreateGroup,
		eTool_CreateLine,
		eTool_CreateRect,
		eTool_CreateOval,
		eTool_CreatePict,
		eTool_CreateText,
		eTool_CreateVar,
		eTool_CreateField,
		eTool_CreateTable,
		eTool_last
	};
								DMArea (void);
	virtual						~DMArea (void);

	virtual		void			HandleScroll (UIScrollBar *which, SInt32 amount);

	static		void			HandleEvent (PA_PluginParameters params);

	virtual		void			SetReport (RWXmlDocument *inXML) override;

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
				SPoint			MapToArea (const SPoint inPt) const;
				SRect			MapToArea (const SRect inRect) const;
				SPoint			MapFromArea (const SPoint inPt) const;
				SRect			MapFromArea (const SRect inRect) const;
				SRect			MapFromAreaRotated (const DMBase *inObject, const SRect inRect) const;
				SPoint			GlobalToLocal (short inX, short inY) const;
                Boolean         WaitMouseMoved( SPoint where, double when);

	inline		bool			IsRecordingUndo (void) const;
	inline		bool			IsPerformingUndoRedo (void) const;
	inline		SInt32			GetUndoRedoState (SInt32& canUndo, SInt32& canRedo);
				SInt32			SaveUndo (SInt32 inOperation, bool inSelection = false);
	inline		SInt32			AddUndoCreate (DMBase *inObject);
	inline		SInt32			AddUndoDelete (DMBase *inObject);
	inline		SInt32			AddUndoChangeParent (DMBase *inObject, DMBase *inOldParent);
	inline		SInt32			AddUndoProperty (DMBase *inObject, OSType inID, const RWValue &inOldValue, const RWValue &inNewValue);
	inline		SInt32			AddUndoSnapshot (DMBase *inObject, bool inOldState);
	inline		SInt32			DoEventUndo (void);
	inline		SInt32			DoEventRedo (void);
    inline      void            CheckCurrentObject (DMBase *inObject);
	inline		void			GetEditorRect( SRect & rect) const;
	inline		void			CallScript( void) ;
    static      UInt32          GetEventModifiers (void);

#if	0
				SInt32			DoEventCut (void);
				SInt32			DoEventCopy (void);
				SInt32			DoEventPaste (void);
				SInt32			DoEventClear (void);
				SInt32			DoEventSelectAll (void);
#endif

protected:
	static		void			DrawDesign (PA_PluginParameters params);
	void						HandleEvent (void);
	void						HandleUpdate (bool inWindow);
	void						AdjustCursor (int inHit);
	void						HandleMouse (void);
#if !__LP64__
    bool                        TrackObject (DMBase *inObj, QDPoint wPt, int inHit);
	bool						TrackSelect (QDPoint wPt);
	bool						TrackNewObject (DMBase *inObj, QDPoint wPt);
    void						TrackNewGuide (QDPoint wPt, bool inVertical);
    inline      void            TrackMouse4Obj (DMBase *inObj, QDPoint wPt) ;
#else
    void                        TrackObjectStart ();
    void                        TrackObjectBody (SPoint inWhere, double when);
    void                        TrackObjectEnd (int err);
    void                        TrackNewObjectStart ();
    void                        TrackNewObjectBody (SPoint inWhere, double when);
    void                        TrackNewObjectEnd (int err);

#endif
    
	virtual		void			ScaleChanged (void);
	virtual		void			CalculatePosition (void);
	void						CalcRectangles (void);
	void						DrawRulers (void);
static	float					RoundUI (float f);
	void						SnapPoint (DMBase *inObj, SPoint &ioPoint, bool h, bool v) const;
	void						SnapRect (DMBase *inObj, SRect &ioRect, int inHit) const;
	void						MakeProportional (SRect &iRect, SPoint &ioPoint);
	
	void						OnBeginDrag (void);
	void						OnAllowDrop (void);
	void						OnDrag (void);
	void						OnDrop (void);
	
#if	WINVER
	static	LONG APIENTRY		AreaWndProc (HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

#if	kUSE_FAKE_AREA
	static		bool			sClassRegistered;
#else
				void			RegisterProc (void);
				void			UnRegisterProc (void);
	struct	wndProcMap
	{
		WNDPROC	oldProc;
		long	refCount;
	};
	typedef	RWMap<HWND,wndProcMap>	WndProcMap;
	static	WndProcMap			sWndProcMap;
#endif
#endif

protected:
	static const UserProps	sUserProperties[];
	static float			sRoundUI;
	static float			sSnapUI;
	static float			sScrollUI;

	RWNativePageComposer	*mScreen;
#if	MACVER
	SRect					mPortRect;			// window port rect - for CG mapping
	bool					mComposite;			// are we in a composited window?
#else
#if	kUSE_FAKE_AREA
	HWND					mArea;
#endif
#endif
	SRect					mNoHitTest;			// if 4D is showing edit field, ignore this area...
    RWString				mAreaName;
	PA_PluginProperties		mAreaProperties;
	SRect					mAreaFullRect;		// 4D's area rect
	SRect					mToolbarRect;
	SRect					mEditorRect;		// editor's usable area
	SRect					mRulerRectH;
	SRect					mRulerRectV;
	SRect					mScrollRectH;
	SRect					mScrollRectV;

	UIScrollBar			*	mScrollH;
	UIScrollBar			*	mScrollV;
	SPoint					mScrollPos;
	SPoint					mScrollPosScaled;
	EDrawDM					mDrawMode;
	bool					mAreaSelected;
	bool					mDirtyIdle;

	long					mEventLevel;
	PA_PluginParameters		mCurParams;
	SRect					mNewAreaRect;
	PA_AreaEvent			mCurEvent;
	PA_AreaEvent			mMenuEvent;
	int						mLastEventHit;	// EHitTest
	DMBase					*mLastObjectHit;
	DMBase					*mLastObjectCreated;
	double					mLastEventTime;
    SPoint                   mLastEventPos;
    SPoint                   mLastAreaPos;
	UInt32					mLastEventModifiers;
	long					mLastEventKey;
	PA_Unichar				mLastEventChar [8];
	int						mCurCharPos;
	bool					mDoubleClick;
//	union	{
//		ETool				mTool;
		int					mToolI;
//	};
	SRect					mTrackSelect;
	CGAffineTransform		mScaleDraw;		// mScale - MapFromArea
	CGAffineTransform		mScaleClick;	// 1/mScale - MapToArea
	bool					mRulerAbsolute;
	bool					mCallScriptInIdle;
	PA_AreaEvent            mInterfaceEvent;
    bool                    mRequestUpdate;
	SPoint					mLastDragPos;
	long					mLastDragTick;
	DMBase					*mLastDragObject;
	bool					mIsDrag;
	
	DMUndo					mUndoBuffer;
    int                     mTrackEvent;
    int                     mTrackHit;
};


inline		
void			
DMArea::GetEditorRect( SRect & rect) const
{
	rect = mEditorRect;
}

inline		
void			
DMArea::CallScript( void) 
{
	mCallScriptInIdle = true;
}

inline
bool
DMArea::IsRecordingUndo (void)
const
{
	return mUndoBuffer.IsRecording();
}


inline
bool
DMArea::IsPerformingUndoRedo (void)
const
{
	return mUndoBuffer.IsPerformingUndoRedo();
}

inline
SInt32
DMArea::GetUndoRedoState (SInt32& canUndo, SInt32& canRedo)
{
	return mUndoBuffer.GetUndoRedoState (canUndo, canRedo);
}

inline
SInt32
DMArea::AddUndoCreate (DMBase *inObject)
{
	return mUndoBuffer.AddCreate (this, inObject);
}

inline
SInt32
DMArea::AddUndoDelete (DMBase *inObject)
{
	return mUndoBuffer.AddDelete (this, inObject);
}

inline
void
DMArea::CheckCurrentObject (DMBase *inObject)
{
    if (mLastObjectHit == inObject)
        mLastObjectHit = NULL;
}

inline
SInt32
DMArea::AddUndoChangeParent (DMBase *inObject, DMBase *inOldParent)
{
	return mUndoBuffer.AddParentChange (this, inObject, inOldParent);
}

inline
SInt32
DMArea::AddUndoProperty (DMBase *inObject, OSType inID, const RWValue &inOldValue, const RWValue &inNewValue)
{
	return mUndoBuffer.AddProperty (this, inObject, inID, inOldValue, inNewValue);
}

inline
SInt32
DMArea::AddUndoSnapshot (DMBase *inObject, bool inOldState)
{
	return mUndoBuffer.AddSnapshot (this, inObject, inOldState);
}

inline
SInt32
DMArea::DoEventUndo (void)
{
	return mUndoBuffer.Undo (this);
}

inline
SInt32
DMArea::DoEventRedo (void)
{
	return mUndoBuffer.Redo (this);
}

#endif
