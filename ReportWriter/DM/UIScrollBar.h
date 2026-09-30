/*
 *  UIScrollBar.h
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 25.11.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */
#ifndef	_UIScrollBar_h
#define	_UIScrollBar_h
# include	"RWBaseTypes.h"

class UIScrollBar;

class UIScrollClient
{
public:
	virtual					~UIScrollClient (void);
	virtual	void			HandleScroll (UIScrollBar *which, SInt32 amount) = 0;
};


class UIScrollBar
{
public:
#if	WINVER
					UIScrollBar (UIScrollClient *inClient, HWND inWindow, const SRect& inRect);
#else
					UIScrollBar (UIScrollClient *inClient, WindowRef inWindow, const SRect& inRect);
#endif
					~UIScrollBar (void);

	SInt32			GetValue (void) const;
	SInt32			GetMinimum (void) const;
	SInt32			GetMaximum (void) const;
	SInt32			GetFew (void) const;
	SInt32			GetPage (void) const;

	void			SetValue (SInt32 val);
	void			SetValues (SInt32 val, SInt32 min, SInt32 max, SInt32 few, SInt32 thumb);
	void			SetEnabled (bool enabled);
	void			SetVisible (bool visible);
	void			Update (const SRect *inRect);
	OSStatus		Track (QDPoint where);
#if WINVER
	void			Update (const SRect *inRect, HDC inDC);
	void			ParentPosChanged (void);
	void			ScrollProc (long param);
#else
protected:
	static	DEFINE_API (void)	ActionProc (ControlRef theControl, ControlPartCode partCode);
#endif

protected:
	void			SetValuesInternal (SInt32 val, SInt32 min, SInt32 max, SInt32 thumb);

protected:
	UIScrollClient *_client;
	SRect 			_rect;
	SInt32			_val;
	SInt32			_min;
	SInt32			_max;
	SInt32			_page;
	SInt32			_few;
	SInt32			_thumb;

#if WINVER
	HWND			_window;
	HWND			_control;
	SInt32			_maxDisplacement;
	QDPoint			_parentPos;
	FARPROC			_oldProc;
#else
	WindowRef 		_window;
	ControlHandle	_control;
//	Boolean			_supportsLiveFeedback;
	bool			_composite;			// are we in a composited window?

static	ControlActionUPP	sActionProc;
#endif
	
};

inline			UIScrollClient::~UIScrollClient (void)	{}
inline	SInt32	UIScrollBar::GetValue (void) const		{ return this? this->_val: 0; }
inline	SInt32	UIScrollBar::GetMinimum (void) const	{ return this? this->_min: 0; }
inline	SInt32	UIScrollBar::GetMaximum (void) const	{ return this? this->_max: 0; }
inline	SInt32	UIScrollBar::GetFew (void) const		{ return this? this->_few: 0; }
inline	SInt32	UIScrollBar::GetPage (void) const		{ return this? this->_page: 0; }

#endif
