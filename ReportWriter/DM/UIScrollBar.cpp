/*
 *  UIScrollBar.cpp
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 25.11.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */

//# include	"UIScrollBar.h"
# include	"DMArea.h"			// needed for kUSE_FAKE_AREA

#if	WINVER

# include	<commctrl.h>

extern	HINSTANCE	gMyInstance;

// inWindow has to be a fake area with ScrollProc handling
UIScrollBar::UIScrollBar (UIScrollClient *inClient, HWND inWindow, const SRect& inRect)
	:	_client (inClient),
		_rect (inRect),
		_val (0),
		_min (0),
		_max (0),
		_page (0),
		_few (0),
		_thumb (0),
		_window (inWindow),
		_control (0),
		_maxDisplacement (0)
{
		_parentPos.h = _parentPos.v = 0;
		const SInt16 w = (SInt16) (inRect.Width());
		const SInt16 h = (SInt16) (inRect.Height());
		const Boolean horiz = (h < w) ? true:false;

#if	kUSE_FAKE_AREA
		ParentPosChanged();	// if using fake area
#endif
		_control = ::CreateWindowW (WC_SCROLLBARW, L"",
			(DWORD) (WS_CHILD | (horiz ? SBS_HORZ:SBS_VERT) | WS_VISIBLE),
			inRect.left + _parentPos.h, inRect.top + _parentPos.v, w, h,
			inWindow, NULL, gMyInstance, NULL);
		if (_control != 0)
			::SetWindowLongPtrW (_control, GWLP_USERDATA, (LONG_PTR) this);
}


UIScrollBar::~UIScrollBar (void)
{
	if (_control)
	{
		::ShowWindow (_control, SW_HIDE);
		::DestroyWindow (_control);
	}
}


void
UIScrollBar::SetValue (SInt32 val)
{
	if (val != _val)
		SetValues (val, _min, _max - _maxDisplacement, _few, _thumb);	//mbs 30042010	added missing _maxDisplacement
}


void
UIScrollBar::SetValuesInternal (SInt32 val, SInt32 min, SInt32 max, SInt32 thumb)
{
	if (val == _val && min == _min && max == (_max - _maxDisplacement) && thumb == _thumb)
		return;

	_val = val;
	_min = min;
	_max = max;
	_thumb = thumb;

	if (_val < _min)
		_val = _min;
	else if (_val > _max)
		_val = _max;

	if (max == min)
		_maxDisplacement = _thumb = 0;
	else
	{
		_maxDisplacement = _thumb - 1;
		_max += _maxDisplacement;
	}

	SCROLLINFO	sinfo;
	sinfo.cbSize = sizeof (sinfo);
	sinfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL;
	if (_min >= _max - _maxDisplacement)	// disable
	{
		sinfo.nMin = sinfo.nMax = sinfo.nPos = 0;
		sinfo.nPage = 0;
	}
	else
	{
		sinfo.nMin = _min;
		sinfo.nMax = _max,
		sinfo.nPos = _val;
		sinfo.nPage = _thumb;
	}
	::SetScrollInfo (_control, SB_CTL, &sinfo, TRUE);
}


void
UIScrollBar::SetEnabled (bool enabled)
{
	::EnableWindow (_control, enabled);
}


void
UIScrollBar::SetVisible (bool visible)
{
	::ShowWindow (_control, visible? SW_SHOWDEFAULT: SW_HIDE);
	if (visible && _max == _min)
	{
		SCROLLINFO	sinfo;

		sinfo.cbSize = sizeof (sinfo);
		sinfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL | SIF_TRACKPOS;
		sinfo.nMin = sinfo.nMax = sinfo.nPos = 0;
		sinfo.nPage = 0;
		::SetScrollInfo (_control, SB_CTL, &sinfo, TRUE);
	}
}


void
UIScrollBar::Update (const SRect* inRect)
{
	if (inRect != 0 && _rect != *inRect)
	{
		_rect = *inRect;
		::SetWindowPos (_control, NULL, _rect.left + _parentPos.h, _rect.top + _parentPos.v, _rect.Width(), _rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER /* | SWP_NOREDRAW */);
		_maxDisplacement = -1;	//mbs 30062011	force recalc (multipage forms)
	}

	if (! ::IsWindowVisible (_control))
		SetVisible (true);
	else
	{
#if	0
		//mbs 23032007	kill "animation" of scrollbar under Vista
		SCROLLINFO	sinfo;

		sinfo.cbSize = sizeof (sinfo);
		sinfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
		if (::GetScrollInfo (_control, SB_CTL, &sinfo))
		{
			sinfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL | SIF_TRACKPOS;
			::SetScrollInfo (_control, SB_CTL, &sinfo, TRUE);
		}
#else
		::UpdateWindow (_control);	//mbs 10052010
#endif
	}
}


void
UIScrollBar::Update (const SRect* inRect, HDC inDC)
{
	if (inRect != 0 && _rect != *inRect)
	{
		_rect = *inRect;
		::SetWindowPos (_control, NULL, _rect.left + _parentPos.h, _rect.top + _parentPos.v, _rect.Width(), _rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
	}

	if (! ::IsWindowVisible (_control))
		SetVisible (true);
	else
	{
		//mbs 23032007	kill "animation" of scrollbar under Vista
		SCROLLINFO	sinfo;

		sinfo.cbSize = sizeof (sinfo);
		sinfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
		if (::GetScrollInfo (_control, SB_CTL, &sinfo))
		{
			sinfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL | SIF_TRACKPOS;
			::SetScrollInfo (_control, SB_CTL, &sinfo, TRUE);
		}
	}
	SendMessage (_control, WM_PAINT, (WPARAM) inDC, 0);
}


OSStatus
UIScrollBar::Track (QDPoint where)
{
	return noErr;
}


void UIScrollBar::ParentPosChanged (void)
{
//	if (!_control)
//		return;

	POINT pt = { 0, 0 };
//	HWND	parent = ::GetParent (_control);
	::MapWindowPoints (_window, ::GetParent (_window), &pt, 1);
	_parentPos.h = -pt.x;
	_parentPos.v = -pt.y;
}


void
UIScrollBar::ScrollProc (long param)
{
	SInt32		amount = 0, newVal;

	if (this == 0)
		return;

	switch (param)
	{
		case SB_TOP:
			amount = _val;
			break;

		case SB_BOTTOM:
			amount = _val - _max;
			break;

		case SB_LINEUP:
			amount = _few;
			break;

		case SB_LINEDOWN:
			amount = - _few;
			break;

		case SB_PAGEUP:
			amount = _page;
			break;

		case SB_PAGEDOWN:
			amount = - _page;
			break;

		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
		{
			amount = _val;
			SCROLLINFO	sinfo;

			sinfo.cbSize = sizeof (sinfo);
			sinfo.fMask = SIF_ALL;
			sinfo.nTrackPos = 0;
			if (::GetScrollInfo (_control, SB_CTL, &sinfo))
				amount -= sinfo.nTrackPos;
			break;
		}

		default:
			return;
	}

	newVal = _val - amount;
	if (newVal < _min)
		newVal = _min;
	else if (newVal > _max - _maxDisplacement)
		newVal = _max - _maxDisplacement;
	amount = _val - newVal;
	if (amount == 0)
		return;
	SetValue (newVal);
	_client->HandleScroll (this, amount);
}

#else



// 64 bit macOS: the Carbon scroll bar control does not exist; values are kept, nothing is drawn
UIScrollBar::UIScrollBar (UIScrollClient *inClient, void *inWindow, const SRect& inRect)
	:	_client (inClient),
		_rect (inRect),
		_val (0),
		_min (0),
		_max (0),
		_page (0),
		_few (0),
		_thumb (0),
		_window (inWindow),
		_control (0)
//		_supportsLiveFeedback (false)
{

}


UIScrollBar::~UIScrollBar (void)
{
	if (_control)
	{
    }
}


void
UIScrollBar::SetValue (SInt32 val)
{
	if (val != _val)
		SetValues (val, _min, _max, _few, _thumb);
}


void
UIScrollBar::SetValuesInternal (SInt32 val, SInt32 min, SInt32 max, SInt32 thumb)
{
	if (val == _val && min == _min && max == _max && thumb == _thumb)
		return;

	_val = val;
	_min = min;
	_max = max;
	_thumb = thumb;

	if (_val < _min)
		_val = _min;
	else if (_val > _max)
		_val = _max;

	if (_min >= _max)	// disable
	{
	}
}


void
UIScrollBar::SetEnabled (bool enabled)
{
}


void
UIScrollBar::SetVisible (bool visible)
{
}


void
UIScrollBar::Update (const SRect* inRect)
{
}




OSStatus
UIScrollBar::Track (QDPoint where)
{
	return noErr;
}

#endif


void
UIScrollBar::SetValues (SInt32 val, SInt32 min, SInt32 max, SInt32 few, SInt32 thumb)
{
	//mbs 30062011	set _page first!
	if (few >= 0)
		_few = few;
	else
		_few = 1;
	if (thumb - few >= 0)
		_page = thumb - few;
	else
		_page = 1;

	SetValuesInternal (val, min, max, thumb);
}
