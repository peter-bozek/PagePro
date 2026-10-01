# include	"RWObject.h"
# include	"RWReportData.h"
# include	"RWDataSource.h"
# include	"RWReportWriter.h"


// ---------------------------------------------------------------------------
// RWObject									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

RWObject::RWObject (int inOrder)
	:	mReportData (0),
		mSeqID (inOrder),
		mPosition (0, 0, 0, 0),
//		mFixedH (false),
		mFixedV (false),
//		mBindH (false),
		mBindV (false),
		mAlignment (eAlign_None),
		mDraw (eDraw_Yes),

		mVirtPosition (0, 0, 0, 0),
		mPrinted (false)
//		mStrech (false),
//		mVertExpansion (0)
{
}

// ---------------------------------------------------------------------------
// ~RWObject								Destructor			   [protected]
// ---------------------------------------------------------------------------

RWObject::~RWObject (void)
{
}


// ---------------------------------------------------------------------------
// GetReportData													  [public]
// ---------------------------------------------------------------------------

const RWReportData*
RWObject::GetReportData (void)
const
{
	return mReportData;
}


// ---------------------------------------------------------------------------
// GetReportWriter													  [public]
// ---------------------------------------------------------------------------

RWReportWriter*
RWObject::GetReportWriter (void)
const
{
	return mReportData->GetReportWriter();
}


// ---------------------------------------------------------------------------
// GetDataSource													  [public]
// ---------------------------------------------------------------------------

RWDataSource&
RWObject::GetDataSource (void)
const
{
	return mReportData->GetReportWriter()->GetDataSource();
}


// ---------------------------------------------------------------------------
// ComparePosition													  [public]
// ---------------------------------------------------------------------------

int
RWObject::ComparePosition (const RWObject *other)
const
{
	if (mPosition.top < other->mPosition.top)
		return -1;
	else if (mPosition.top > other->mPosition.top)
		return 1;
	else if (mPosition.left < other->mPosition.left)
		return -1;
	else if (mPosition.left > other->mPosition.left)
		return 1;
	return 0;
}


// ---------------------------------------------------------------------------
// CompareOrder														  [public]
// ---------------------------------------------------------------------------

int
RWObject::CompareOrder (const RWObject *other)
const
{
	if (mSeqID < other->mSeqID)
		return -1;
	else if (mSeqID > other->mSeqID)
		return 1;

	return 0;	// should not happen - seqID should be unique within a section...
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWObject::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	mReportData = inReport;
//	mNode = inNode;

	long			lVal;

	for (const auto &[name, value] : inNode.Attributes())
	{

		if (RWStr::EqualsNoCase (name, "r"))
		{
			// "left;top;right;bottom" as SRObject::WriteSelf writes it (the sscanf that
			// replaced this read "top,left,bottom,right" and so only got one number)
			mPosition = value;
		}
#if 1	// OLD_RW_FORMAT
		else if (RWStr::EqualsNoCase (name, "t"))
		{
			mPosition.top = 0;
			RWStr::ReadNumber (value, mPosition.top);
		}
		else if (RWStr::EqualsNoCase (name, "l"))
		{
			mPosition.left = 0;
			RWStr::ReadNumber (value, mPosition.left);
		}
		else if (RWStr::EqualsNoCase (name, "b"))
		{
			mPosition.bottom = 0;
			RWStr::ReadNumber (value, mPosition.bottom);
		}
		else if (RWStr::EqualsNoCase (name, "ri"))
		{
			mPosition.right = 0;
			RWStr::ReadNumber (value, mPosition.right);
		}
		else if (RWStr::EqualsNoCase (name, "h") || RWStr::EqualsNoCase (name, "height"))
		{
			mPosition.bottom = 0;
			RWStr::ReadNumber (value, mPosition.bottom);
			mPosition.bottom += mPosition.top; // pB  causes problems with table - height contain row height - need to be renamed
		}
		else if (RWStr::EqualsNoCase (name, "w") || RWStr::EqualsNoCase (name, "width"))
		{
			mPosition.right = 0;
			RWStr::ReadNumber (value, mPosition.right);
			mPosition.right += mPosition.left;
		}
#endif
/*
		else if (RWStr::EqualsNoCase (name, "fixH"))
		{
			lVal = 0;
			RWStr::ReadNumber (value, lVal);
			mFixedH = (lVal != 0);
		}
*/
		else if (RWStr::EqualsNoCase (name, "fixV"))
		{
			lVal = 0;
			RWStr::ReadNumber (value, lVal);
			mFixedV = (lVal != 0);
		}
/*
		else if (RWStr::EqualsNoCase (name, "bindH"))
		{
			lVal = 0;
			RWStr::ReadNumber (value, lVal);
			mBindH = (lVal != 0);
		}
*/
		else if (RWStr::EqualsNoCase (name, "bindV"))
		{
			lVal = 0;
			RWStr::ReadNumber (value, lVal);
			mBindV = (lVal != 0);
		}
		else if (RWStr::EqualsNoCase (name, "align"))
		{
			if (RWStr::ReadNumber (value, lVal))
			{
				if (lVal >= eAlign_None && lVal <= eAlign_Right)
					mAlignment = EAlignment (lVal);
			}
			else if (RWStr::EqualsNoCase (value, "left"))
				mAlignment = eAlign_Left;
			else if (RWStr::EqualsNoCase (value, "center"))
				mAlignment = eAlign_Center;
			else if (RWStr::EqualsNoCase (value, "right"))
				mAlignment = eAlign_Right;
		}
		else if (RWStr::EqualsNoCase (name, "draw"))
		{
			if (RWStr::ReadNumber (value, lVal))
			{
				if (lVal >= eDraw_No && lVal <= eDraw_Always)
					mDraw = EDraw (lVal);
			}
			else if (RWStr::EqualsNoCase (value, "no"))
				mDraw = eDraw_No;
			else if (RWStr::EqualsNoCase (value, "yes"))
				mDraw = eDraw_Yes;
			else if (RWStr::EqualsNoCase (value, "on overflow"))
				mDraw = eDraw_OnOverflow;
			else if (RWStr::EqualsNoCase (value, "always"))
				mDraw = eDraw_Always;
		}
	}

	if (mAlignment != eAlign_None)
	{
//		mPosition.right -= mPosition.left;
//		mPosition.left = 0;
	}
	mVirtPosition = mPosition;

	return;
}


// ---------------------------------------------------------------------------
// GetMove															  [public]
// ---------------------------------------------------------------------------

bool
RWObject::GetMove (void)
const
{
//	return not (mFixedV || mFixedH);
	return not mFixedV;
}


// ---------------------------------------------------------------------------
// GetGrow															  [public]
// ---------------------------------------------------------------------------

bool
RWObject::GetGrow (void)
const
{
	return false;
}


// ---------------------------------------------------------------------------
// GetBinding														  [public]
// ---------------------------------------------------------------------------

bool
RWObject::GetBinding (void)
const
{
//	return mBindV || mBindH;
	return mBindV;
}


// ---------------------------------------------------------------------------
// GetPosition														  [public]
// ---------------------------------------------------------------------------

SRect
RWObject::GetPosition (void)
const
{
	return mPosition;
}


// ---------------------------------------------------------------------------
// GetVirtualPosition												  [public]
// ---------------------------------------------------------------------------

SRect
RWObject::GetVirtualPosition (void)
const
{
	return mVirtPosition;
}

// ---------------------------------------------------------------------------
// RemoveRow													  [public]
// ---------------------------------------------------------------------------

bool
RWObject::RemoveRow (bool* outRemoveRow)
{
	return *outRemoveRow;
}

// ---------------------------------------------------------------------------
// WillingToPrint													  [public]
// ---------------------------------------------------------------------------

bool
RWObject::WillingToPrint (bool inIsOverflow)
const
{
	bool	draw = not mPrinted;

	// if (draw)
	{
		switch (mDraw)
		{
			case eDraw_No:
				draw = false;
				break;
			case eDraw_Yes:
				draw = draw; // not inIsOverflow;  // pB changed back
				break;
			case eDraw_OnOverflow:
				draw = inIsOverflow;
				break;
			case eDraw_Always:
				draw = true;
				break;
		}
	}

	return draw;
}


// ---------------------------------------------------------------------------
// GetVerticalExpansion												  [public]
// ---------------------------------------------------------------------------

float
RWObject::GetVerticalExpansion (void)
const
{
	float	height = 0;

	if (mVirtPosition.top < 0 && mPosition.top >= 0)	//mbs 13012010	check original top, too
		height = mVirtPosition.bottom - (mPosition.bottom - mPosition.top);
	else
		if (GetGrow ())
			height = (mVirtPosition.bottom - mVirtPosition.top) - (mPosition.bottom - mPosition.top);

	return height;
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
RWObject::Reset (bool inAll)
{
	if (inAll || (mPrinted && mDraw >= eDraw_OnOverflow))	//mbs 26072010
	{
		mVirtPosition = mPosition;
		mPrinted = false;
	}
	else
	{
		mVirtPosition.bottom -= mVirtPosition.top - mPosition.top;
		mVirtPosition.top = mPosition.top;
	}

	return;
}


// ---------------------------------------------------------------------------
// GetBounds														  [public]
// ---------------------------------------------------------------------------

bool
RWObject::GetBounds (RWPageComposer &/*inComposer*/, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow)
{
	if (outRemoveRow)
		*outRemoveRow = false;
	SRect	r (	ioRect.top + mVirtPosition.top,
				ioRect.left + mVirtPosition.left,
				ioRect.top + mVirtPosition.bottom,
				ioRect.left + mVirtPosition.right
			);

	mBounds = r;
	if (mAlignment != eAlign_None)
		AlignOnPage (ioRect, mBounds);

	if (	(r.right < ioRect.left) // pB objects to the right and left are printed
		||	(r.left > ioRect.right)
		||	(r.bottom < ioRect.top) )
	{
		ioRect &= r;  //it is out of printing area, collapsing its rect may mean it is unprintable
		mPrinted = true;
		return true;
	}



/*	TODO: FIXME!!!	*/
	if (inFit)   /* && GetReportData()->IsDynamic() */
	{
		 // ioRect &= r;
		/* must fit whole or not at all  */
		ioRect.top = (ioRect.top > r.top) ? ioRect.top : r.top; // v 1.2.3 do not move top
		ioRect.bottom = (ioRect.bottom < r.bottom) ? ioRect.bottom : r.bottom;
		ioRect.left = r.left;
		ioRect.right = r.right;

	}
	else
		ioRect = r;
/**/

	if (r.Height() == 0)	//mbs 03012010	r.IsEmpty() is false for non-zero width...
		return true;

	return ioRect.Height() > 0;	//mbs 03012010	IsEmpty() is false for non-zero width...	not IsEmpty();
}

// ---------------------------------------------------------------------------
// Draw																  [public]
// ---------------------------------------------------------------------------

bool
RWObject::CanDraw (SRect &inRect, bool &outIsPrinted)
{
	if /*((mBounds.left > inRect.right) || (mBounds.right < inRect.left) ||*/ (mBounds.bottom < inRect.top) {  // pB 2010-12 v21 does not work with alignment of group
		outIsPrinted = true;
		return false;  // do not proceed with printing
	}

	if (mBounds.top > inRect.bottom) {
		outIsPrinted = false;
		return false;
	}

	return true;
}


// ---------------------------------------------------------------------------
// AdjustBounds														  [public]
// ---------------------------------------------------------------------------

void
RWObject::AdjustBounds (float hDelta, float vDelta)
{
	mVirtPosition.right += hDelta;
	mVirtPosition.bottom += vDelta;

	return;
}


// ---------------------------------------------------------------------------
// Move																  [public]
// ---------------------------------------------------------------------------

void
RWObject::Move (float hDelta, float vDelta)
{
	mVirtPosition.top += vDelta;
	mVirtPosition.left += hDelta;
	mVirtPosition.bottom += vDelta;
	mVirtPosition.right += hDelta;

	return;
}


// ---------------------------------------------------------------------------
// FetchCalcValue													  [public]
// ---------------------------------------------------------------------------

void
RWObject::FetchCalcValue (RWReportWriter *inWriter)
{
	return;
}


// ---------------------------------------------------------------------------
// GetVariableText												   [protected]
// ---------------------------------------------------------------------------

RWString
RWObject::GetVariableText (const RWString inVariableName, const RWString inFormat)
const
{
	RWValue		var;
	RWString	result;

	if (GetReportWriter()->GetVariable (inVariableName, var))
		result = GetReportWriter()->FormatVariable (var, inFormat);
//	else
//		result.Copy ("Unknown Variable");

	return result;
}


// ---------------------------------------------------------------------------
// AlingOnPage													   [protected]
// ---------------------------------------------------------------------------

//mbs 04082010
void
RWObject::AlignOnPage (const SRect &origRect, SRect &ioRect)
const
{
	double	width = ioRect.Width();
	SRect	page = GetReportWriter()->GetPageComposer().GetTruePageRect();

	// adjust to container - section or group
	if (origRect.left > page.left)
		page.left = origRect.left;
	if (origRect.right < page.right)
		page.right = origRect.right;

	// align
	if (mAlignment == eAlign_Left)
	{
		ioRect.left = page.left;
		ioRect.right = ioRect.left + width;
	}
	else if (mAlignment == eAlign_Right)
	{
		ioRect.right = page.right - 1;
		ioRect.left = ioRect.right - width;
	}
	else	// if (mAlignment == eAlign_Center)
	{
		ioRect.left = (page.Width() - width) / 2;
		ioRect.right = ioRect.left + width;
	}

}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

RWGroup*
RWGroup::Create (RWReportData *inReport, RWXmlNode inNode, int inOrder)
{
	RWGroup	*group = new RWGroup (inOrder);
	group->Parse (inReport, inNode);

	return group;
}


// ---------------------------------------------------------------------------
// RWGroup									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

RWGroup::RWGroup (int inOrder)
	:	RWObject (inOrder),
//		mExpandH (false),
		mExpandV (false),
		mEmptyByChild (false)
{
}


// ---------------------------------------------------------------------------
// ~RWGroup									Destructor			   [protected]
// ---------------------------------------------------------------------------

RWGroup::~RWGroup (void)
{
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
RWGroup::Reset (bool inAll)
{
// 	if (inAll || (mPrinted && mDraw >= eDraw_OnOverflow))	//mbs 26072010
	{
		mEmptyByChild = false;
		RWObjList::const_iterator	it;
		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			RWObject	*obj = *it;
			obj->Reset (inAll);
		}
	}
	RWObject::Reset (inAll);

	return;
}


#if	0
// ---------------------------------------------------------------------------
// SetKeepTogether													  [public]
// ---------------------------------------------------------------------------

void
RWGroup::SetKeepTogether (void)
{
//	RWObject::SetKeepTogether();

	RWObjList::iterator	it;
	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		RWObject	*obj = *it;
		obj->SetKeepTogether();
	}

	return;
}
#endif


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWGroup::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWObject::Parse (inReport, inNode);

	long			    lVal;

	for (const auto &[name, value] : inNode.Attributes())
	{

/*
		if (RWStr::EqualsNoCase (name, "expandH"))
		{
			lVal = 0;
			RWStr::ReadNumber (value, lVal);
			mExpandH = (lVal != 0);
		}
		else
*/
		if (RWStr::EqualsNoCase (name, "expandV"))
		{
			lVal = 0;
			RWStr::ReadNumber (value, lVal);
			mExpandV = (lVal != 0);
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// GetGrow															  [public]
// ---------------------------------------------------------------------------

bool
RWGroup::GetGrow (void)
const
{
//	return mExpandV || mExpandH;
	return mExpandV;
}


// ---------------------------------------------------------------------------
// GetBounds														  [public]
// ---------------------------------------------------------------------------

bool
RWGroup::GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow)
{
	bool	fit = true;
	bool	dontDraw = false, removeMe = false;

	if (not mEmptyByChild)	//mbs 25092009	remove "row" support
	{
		SRect	origRect (ioRect);
		if (GetGrow())	// shrink/expand as needed
		{
			RWObjList::const_iterator	it;
			RWObject					*obj;
			float						bBottom = 0, rBottom = 0;
			float						bRight = 0, rRight = 0;
// ••• TODO •••	CHECK the following change!!!
//			SRect						r (ioRect);
//			r += mVirtPosition;	// adjust the frame for children
			SRect	r (	ioRect.top + mVirtPosition.top,
					   ioRect.left + mVirtPosition.left,
					   ioRect.top  + mVirtPosition.bottom,
					   ioRect.left + mVirtPosition.right
					   );

			for (it = mObjects.begin(); it != mObjects.end(); it++)
			{
				obj = *it;
				if (obj->RemoveRow (&removeMe)) { // only text can return true
					dontDraw = true;
					break;
				}
				if (obj->WillingToPrint (inIsOverflow))
				{
					SRect	o (obj->GetPosition());
					if (o.bottom > rBottom)
						rBottom = o.bottom;
					if (o.right > rRight)
						rRight = o.right;

					SRect	b (r);
					if (not obj->GetBounds (inComposer, b, inFit, inIsOverflow, &removeMe))
					{
						fit = false;
						break;
					}
					if (removeMe)
					{
						dontDraw = true;
						break;
					}
					else
					{
						if (b.bottom > bBottom)
							bBottom = b.bottom;
						if (b.right > bRight)
							bRight = b.right;
					}
				}
			}

			if (not dontDraw && fit)
			{
				float	vDelta;
				if (rBottom > mPosition.Height() )
					vDelta = bBottom - r.top - mVirtPosition.Height() ;
				else
					vDelta = bBottom - (rBottom + r.top) - (mVirtPosition.Height() - mPosition.Height()); //pB
				float	hDelta = bRight - (rRight + r.left) - (mVirtPosition.Width() - mPosition.Width());;	// the only horizontally expandable object is RWTable
				r.bottom = r.top + mVirtPosition.Height() + vDelta;
				r.right = r.left + mVirtPosition.Width() + hDelta;
				if (inFit)
				{
					if (r.bottom > ioRect.bottom)	// whole group must fit on a single page
					{
						if (true) // mDraw >= eDraw_OnOverflow)
							// pB 2011 group can be split to several pages
						{
							r.bottom = ioRect.bottom;
							ioRect = r;
							AdjustBounds (hDelta, vDelta);	// adjust contained objects wanting to resize
						} else
						{
							fit = false;
							ioRect.bottom = ioRect.top;
						}
					}
					else
					{
						ioRect = r;
						AdjustBounds (hDelta, vDelta);	// adjust contained objects wanting to resize
					}
				}
				else
				{
					/* if (r.bottom > ioRect.bottom)
						r.bottom = ioRect.bottom;  pB 2010-10 if in measure mode do not restrict to available space */
					ioRect = r;
					AdjustBounds (hDelta, vDelta);		// adjust contained objects wanting to resize
				}
			}
		}
		else if (inFit)
		{
			// pB 2013 v 1.3.1 - constant size group should be possible to remove too (which trumps constant size, though)
			RWObjList::const_iterator		it;
			RWObject					*obj;
			
			for (it = mObjects.begin(); it != mObjects.end(); it++)
			{
				obj = *it;
				if (obj->RemoveRow (&removeMe)) {
					dontDraw = true;
					break;
				}
			}
			
			SRect	r (	ioRect.top + mVirtPosition.top,
						ioRect.left + mVirtPosition.left,
						ioRect.top + mVirtPosition.bottom,
						ioRect.left + mVirtPosition.right
					);
			fit = ioRect.Contains (r);
			if (fit)
				ioRect = r;
		}
		else
			fit = RWObject::GetBounds (inComposer, ioRect, inFit, inIsOverflow, &dontDraw);
		//mbs 04082010
		if (fit && mAlignment != eAlign_None) {
			AlignOnPage (origRect, ioRect);
		}
	}
	else
		dontDraw = true;

	if (dontDraw)	//mbs 25092009	remove "row" support
	{
		mEmptyByChild = true;
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
		
		if (GetGrow()) {
				// pB virtual position is used to calculate if size changed - and it changed to 0,0
			mVirtPosition.bottom = mVirtPosition.top;
			mVirtPosition.right = mVirtPosition.left;
		}
		
		fit = true;
//		mPrinted = true;
	}
	
	// pB 2013-8-22 v 1.3.1 do not propagate removal to top group(s)
	/*
	if (outRemoveRow)
		*outRemoveRow = dontDraw;
	*/
	// this makes outRemoveRow unused var, let's keep it for compatibility and/or future use
	
	return fit;
}


// ---------------------------------------------------------------------------
// AdjustBounds														  [public]
// ---------------------------------------------------------------------------

void
RWGroup::AdjustBounds (float hDelta, float vDelta)
{
	RWObject::AdjustBounds (hDelta, vDelta);

	RWObjList::const_iterator	it;
	RWObject					*obj;

	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		obj = *it;
 		if (obj->GetBinding())
			obj->AdjustBounds (hDelta, vDelta);
	}

	return;
}


// ---------------------------------------------------------------------------
// FetchCalcValue													  [public]
// ---------------------------------------------------------------------------

void
RWGroup::FetchCalcValue (RWReportWriter *inWriter)
{
	RWObjList::const_iterator	it;
	RWObject					*obj;

	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		obj = *it;
		obj->FetchCalcValue (inWriter);
	}

	return;
}


// ---------------------------------------------------------------------------
// Draw																  [public]
// ---------------------------------------------------------------------------

RWObject::EDrawState
RWGroup::Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow)
{
	int	state = eDrawState_Done;

	if (not mEmptyByChild)	//mbs 25092009	remove "row" support
	{
		RWObjList::const_iterator	it;
		RWObject					*obj;

		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			obj = *it;
			if (obj->WillingToPrint (inIsOverflow))
			{
				SRect	r (inRect);
// no! before we Draw, GetBounds is called ==> inRect is already adjusted!
//				r += mVirtPosition;	// adjust the frame for children
				if (obj->GetBounds (inComposer, r, true, inIsOverflow, NULL))
					state |= obj->Draw (inComposer, r, inIsOverflow);
				else
				{
					state |= eDrawState_Vertical;
					break;
				}
			}
		}

		// group MUST fit onto a single page... // pB not valid anymore, group may be split, however for 
		//  groups in break headers and headers, the text is reset and printing properrty decides where is printed - first time or  always
		
		/* it need to be solved diferently, this has too many side effects
		 the goal is to control printing in repeating break header, but we cannot distinguish that case here
		  // pB TODO
		if ((state != eDrawState_Done) && (mDraw < eDraw_OnOverflow)) //if the group cannot print in overflow, it is printed as one item
			state = eDrawState_Done;
		 */
	}

	if (state == eDrawState_Done)
		mPrinted = true;

	return EDrawState (state);
}


RWObjList *
RWGroup::GetObjects (void)
{
	return &mObjects;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

RWLine*
RWLine::Create (RWReportData *inReport, RWXmlNode inNode, int inOrder)
{
	RWLine	*line = new RWLine (inOrder);
	line->Parse (inReport, inNode);

	return line;
}


// ---------------------------------------------------------------------------
// RWLine									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

RWLine::RWLine (int inOrder)
	:	RWObject (inOrder),
		mThickness (1),
		mLineColor (cBlackColor),
		mFlags (RWLine_Horizontal)
{
}

// ---------------------------------------------------------------------------
// ~RWLine									Destructor			   [protected]
// ---------------------------------------------------------------------------

RWLine::~RWLine (void)
{
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWLine::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWObject::Parse (inReport, inNode);

//	if (node)
//		elem = node.Child (u"LineProps");

    RWXmlNode elem = inNode.Child (u"LineProps");
	if (!elem)
		elem = inNode;

	if (elem)
	{
		for (const auto &[name, value] : elem.Attributes())
		{

			if (RWStr::EqualsNoCase (name, "thickness"))
			{
				mThickness = 1;
				RWStr::ReadNumber (value, mThickness);
				if (mThickness < 0 || mThickness > 10)
					mThickness = 1;
			}
			else if (RWStr::EqualsNoCase (name, "lineColor"))
			{
				mLineColor = value.c_str();
			}
			else if (RWStr::EqualsNoCase (name, "flags"))
			{
				mFlags = (unsigned short) ((long) RWStr::ToInteger (value).value_or (0) & 0x07);
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// GetBounds														  [public]
// ---------------------------------------------------------------------------

//mbs 04082010
bool
RWLine::GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow)
{
	SRect	origRect (ioRect);
			// pB 2013 v1.3.1 fix for case when bounding box is thinner then line width
	if (mFlags != RWLine_Vertical) {
			if ((mVirtPosition.bottom - mVirtPosition.top) < mThickness)
				mVirtPosition.bottom = mVirtPosition.top + mThickness;
	}
	bool	fit = RWObject::GetBounds (inComposer, ioRect, inFit, inIsOverflow, outRemoveRow);
	if (fit && mAlignment != eAlign_None)
		AlignOnPage (origRect, ioRect);
	return fit;
}


// ---------------------------------------------------------------------------
// AdjustBounds														  [public]
// ---------------------------------------------------------------------------

void
RWLine::AdjustBounds (float hDelta, float vDelta)
{
	if (mPosition.bottom - mPosition.top > mPosition.right - mPosition.left)		// vertical line
		mVirtPosition.bottom += vDelta;
	else
		mVirtPosition.right += hDelta;

	return;
}


// ---------------------------------------------------------------------------
// Draw																  [public]
// ---------------------------------------------------------------------------

RWObject::EDrawState
RWLine::Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow)
{
	if (CanDraw (inRect, mPrinted)) {		// pB

		/* there is overlapping between printing area and rect */

		inComposer.DrawLine (inRect, mThickness, mLineColor, mFlags); // pB changd inRect to mBounds
		mPrinted = true;
	}
	return eDrawState_Done;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

RWOval*
RWOval::Create (RWReportData *inReport, RWXmlNode inNode, int inOrder)
{
	RWOval	*rect = new RWOval (inOrder);
	rect->Parse (inReport, inNode);

	return rect;
}


// ---------------------------------------------------------------------------
// RWOval									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

RWOval::RWOval (int inOrder)
	:	RWObject (inOrder),
		mThickness (1),
		mFrameColor (cBlackColor),
		mFill (false),
		mFillColor (cBlackColor)
{
}

// ---------------------------------------------------------------------------
// ~RWOval									Destructor			   [protected]
// ---------------------------------------------------------------------------

RWOval::~RWOval (void)
{
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWOval::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWObject::Parse (inReport, inNode);

//	if (node)
//		node = node->FirstChild ("RectProps");

	RWXmlNode	elem = inNode.Child (GetKind() == eObject_Oval ? u"OvalProps" : u"RectProps");
	if (!elem)
		elem = inNode;

	if (elem)
	{
		for (const auto &[name, value] : elem.Attributes())
		{

			if (RWStr::EqualsNoCase (name, "thickness"))
			{
				mThickness = 1;
				RWStr::ReadNumber (value, mThickness);
				if (mThickness < 0 || mThickness > 10)
					mThickness = 1;
			}
			else if (RWStr::EqualsNoCase (name, "frameColor") || RWStr::EqualsNoCase (name, "lineColor"))
			{
				mFrameColor = value.c_str();
			}
			else if (RWStr::EqualsNoCase (name, "fillColor"))
			{
				mFill = true;
				mFillColor = value.c_str();
			}
		}
	}

	return;
}

// ---------------------------------------------------------------------------
// GetBounds														  [public]
// ---------------------------------------------------------------------------

//pB
bool
RWOval::GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow)
{
		SRect	origRect (ioRect);
	bool	fit = RWObject::GetBounds (inComposer, ioRect, inFit, inIsOverflow, outRemoveRow);
	if (fit && mAlignment != eAlign_None)
		AlignOnPage (origRect, ioRect);
	return fit;
}

// ---------------------------------------------------------------------------
// Draw																  [public]
// ---------------------------------------------------------------------------

RWObject::EDrawState
RWOval::Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow)
{
//mbs 09072010	try clipping
	if (CanDraw (inRect, mPrinted)) {		// pB

		/* there is overlapping between printing area and rect */

		if (inRect.Contains (mBounds))
			inComposer.DrawOval (inRect, mThickness, true, mFrameColor, mFill, mFillColor);
		else
		{
			StClipToRect	clip (&inComposer, inRect);
			inComposer.DrawOval (mBounds, mThickness, true, mFrameColor, mFill, mFillColor);
		}

		mPrinted = true;
	}
	return eDrawState_Done;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

RWRect*
RWRect::Create (RWReportData *inReport, RWXmlNode inNode, int inOrder)
{
	RWRect	*rect = new RWRect (inOrder);
	rect->Parse (inReport, inNode);

	return rect;
}


// ---------------------------------------------------------------------------
// RWRect									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

RWRect::RWRect (int inOrder)
	:	RWOval (inOrder),
		mRows (1),
		mCols (1),
		mFlags (RWRect_Full)
{
}

// ---------------------------------------------------------------------------
// ~RWRect									Destructor			   [protected]
// ---------------------------------------------------------------------------

RWRect::~RWRect (void)
{
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWRect::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWOval::Parse (inReport, inNode);

//	if (node)
//		node = node->FirstChild ("RectProps");

    RWXmlNode elem = inNode.Child (u"RectProps");
	if (!elem)
		elem = inNode;

	if (elem)
	{
		for (const auto &[name, value] : elem.Attributes())
		{

			if (RWStr::EqualsNoCase (name, "rows"))
			{
				mRows = (long) RWStr::ToInteger (value).value_or (0);
				if (mRows < 2)
					mRows = 1;
			}
			else if (RWStr::EqualsNoCase (name, "cols"))
			{
				mCols = (long) RWStr::ToInteger (value).value_or (0);
				if (mCols < 2)
					mCols = 1;
			}
			else if (RWStr::EqualsNoCase (name, "flags"))
			{
				mFlags = (unsigned short) ((long) RWStr::ToInteger (value).value_or (0) & RWRect_Full);
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// Draw																  [public]
// ---------------------------------------------------------------------------

RWObject::EDrawState
RWRect::Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow)
{
	// ••• TODO •••	no support for "split" rectangle - must fit on page
//mbs 09072010	try clipping

	if (CanDraw (inRect, mPrinted)) {		// pB

		/* there is overlapping between printing area and rect */

		if (inRect.Contains (mBounds))
			inComposer.DrawRect (inRect, inRect, mThickness, mFrameColor, mFill, mFillColor, mRows, mCols, mFlags);
		else
		{
			 StClipToRect	clip (&inComposer, inRect);
			inComposer.DrawRect (mBounds, mBounds, mThickness, mFrameColor, mFill, mFillColor, mRows, mCols, mFlags); // pB 2012-5
		}

		mPrinted = true;
	}
	return eDrawState_Done;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

RWPict*
RWPict::Create (RWReportData *inReport, RWXmlNode inNode, int inOrder)
{
	RWPict	*pict = new RWPict (inOrder);
	pict->Parse (inReport, inNode);

	return pict;
}


// ---------------------------------------------------------------------------
// RWPict									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

RWPict::RWPict (int inOrder)
	:	RWObject (inOrder),
//		mExpandH (false),
		mExpandV (false),
		mFormat (ePictFormat_Normal),
		mFrame (false),
		mFrameOffset (2),
		mFrameThickness (1),
		mFrameColor (cBlackColor),
		mDataID (0),
		mComposerPictureData (0),
		mObjectRotation(0),
        mFillColor(cEmptyColor)
//		mComposerPictureCreator (0)
{
//	mPicture.Init();
}


// ---------------------------------------------------------------------------
// ~RWPict									Destructor			   [protected]
// ---------------------------------------------------------------------------

RWPict::~RWPict (void)
{
//	if (mComposerPictureCreator && mComposerPictureData)
//		mComposerPictureCreator->FreePict (&mComposerPictureData);

	if (mComposerPictureData)
		delete mComposerPictureData;

	if (mDataID == 0)		// free the picture only if it does not belong to a data source
		mPicture.Free();

	return;
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWPict::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWObject::Parse (inReport, inNode);

//	if (node)
//		node = node->FirstChild ("PictProps");

    RWXmlNode elem = inNode.Child (u"PictProps");
	if (!elem)
		elem = inNode;

	if (elem)
	{
		for (const auto &[name, value] : elem.Attributes())
		{
			long			lVal;

/*
			if (RWStr::EqualsNoCase (name, "expandH"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mExpandH = (lVal != 0);
			}
			else
*/
			if (RWStr::EqualsNoCase (name, "expandV"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mExpandV = (lVal != 0);
			}
			else if (RWStr::EqualsNoCase (name, "dataID"))
			{
				mDataID = (long) RWStr::ToInteger (value).value_or (0);
			}
			else if (RWStr::EqualsNoCase (name, "format"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				if (lVal < ePictFormat_First || lVal >= ePictFormat_Last)
					mFormat = ePictFormat_Normal;
				else
					mFormat = EPictFormat (lVal);
			}
			else if (RWStr::EqualsNoCase (name, "frame"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mFrame = (lVal != 0);
			}
			else if (RWStr::EqualsNoCase (name, "frameOffset"))
			{
				mFrameOffset = 2;
				RWStr::ReadNumber (value, mFrameOffset);
				if (mFrameOffset < 0 || mFrameOffset > 20)
					mFrameOffset = 2;
			}
			else if (RWStr::EqualsNoCase (name, "frameThickness"))
			{
				mFrameThickness = 1;
				RWStr::ReadNumber (value, mFrameThickness);
				if (mFrameThickness < 0 || mFrameThickness > 10)
					mFrameThickness = 1;
			}
			else if (RWStr::EqualsNoCase (name, "frameColor"))
			{
				mFrameColor = value.c_str();
			}
            else if (RWStr::EqualsNoCase (name, "rotation"))
            {
                mObjectRotation = 0;
                RWStr::ReadNumber (value, mObjectRotation);
            }
            else if (RWStr::EqualsNoCase (name, "fillColor"))
            {
                mFillColor = value.c_str();
            }
		}
	}

	if (mDataID == 0)
	{
		elem = elem.Child (u"ImageData");
		if (!elem)
			elem = inNode;

		SBlob	pictData;
		RWTools::ReadData (inNode, pictData);
		if (pictData)
			mPicture.SetBlob (pictData, true);
	}

	return;
}


// ---------------------------------------------------------------------------
// GetGrow															  [public]
// ---------------------------------------------------------------------------

bool
RWPict::GetGrow (void)
const
{
//	return mExpandV || mExpandH;
	return mExpandV;
}


// ---------------------------------------------------------------------------
// GetBounds														  [public]
// ---------------------------------------------------------------------------

bool
RWPict::GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow)
{
	SRect	origRect (ioRect);
	if (outRemoveRow)
		*outRemoveRow = false;
//	if (mComposerPictureCreator != &inComposer)
	{
//		if (mComposerPictureCreator && mComposerPictureData)
//			mComposerPictureCreator->FreePict (&mComposerPictureData);
//		mComposerPictureCreator = &inComposer;
		if (mDataID != 0 && mPicture.GetKind() < RWValue::eValue_PictRefScreen)
			GetDataSource().GetData (mDataID, mPicture);	// that value should be owned by RWDataProvider and thus not freed by our "value"
	}

	bool	fit = true;
	if (mComposerPictureData == NULL || GetGrow())
	{
// ••• TODO •••	CHECK the following change!!!
// this is not good - it will shrink ioRect and GetPictBounds will not grow...
// but calling GetPictBounds with ePictFormat_Normal will do what we really want!
//		SRect	r (ioRect);
//		r += mVirtPosition;	// adjust position
		SRect	r (	ioRect.top + mVirtPosition.top,
				   ioRect.left + mVirtPosition.left,
				   ioRect.top + mVirtPosition.bottom,
				   ioRect.left + mVirtPosition.right
				   );

		if (mFrame)
			r *= mFrameOffset;
		if (r.Width() > mPosition.Width())	// can't expand horizontally
			r.right = r.left + mPosition.Width();
		inComposer.GetPictBounds (r, mPicture, ePictFormat_Normal, &mComposerPictureData, true);	// full size
		inComposer.GetPictBounds (r, mPicture, mFormat, &mComposerPictureData, true);	// adjusted size (limited width)
		if (mDataID == 0)		// free the picture if it does not belong to a data source
			mPicture.Free();	// page composer has already created a composer specific representation of our picture
		if (r.IsEmpty() && GetGrow())
			mPrinted = true;
		else
			if (mFrame)
				r *= -mFrameOffset;
		if (GetGrow())
		{
			if (inFit)
				fit = ioRect.Contains (r);
		}
		else
			fit = RWObject::GetBounds (inComposer, ioRect, inFit, inIsOverflow, outRemoveRow);
		if (fit)
			ioRect = r;
	}
	else
		fit = RWObject::GetBounds (inComposer, ioRect, inFit, inIsOverflow, outRemoveRow);
	//mbs 04082010
	if (fit && mAlignment != eAlign_None)
		AlignOnPage (origRect, ioRect);
	return fit;
}


// ---------------------------------------------------------------------------
// Draw																  [public]
// ---------------------------------------------------------------------------

RWObject::EDrawState
RWPict::Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow)
{
	if (CanDraw (inRect, mPrinted)) {		// pB

		/* there is overlapping between printing area and rect */
			if (mComposerPictureData && mComposerPictureData->GetHeight() > 0)
		{
			SRect	r (mBounds);  // pB 2012 changed from r (inRect);
			if (mFrame)
			{
				inComposer.DrawRect (r, mFrameThickness, true, mFrameColor, false, mFrameColor);
				r *= mFrameOffset;	//mbs 20072011	inset, not -mFrameOffset
			}
			inComposer.DrawPict (r, mPicture, mFormat, &mComposerPictureData, mObjectRotation, ((float)mFillColor.alpha) / 0xFFFF);
		}

		mPrinted = true;

	}
	return eDrawState_Done;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

RWText*
RWText::Create (RWReportData *inReport, RWXmlNode inNode, int inOrder)
{
	RWText	*text = new RWText (inOrder);
	text->Parse (inReport, inNode);

	return text;
}


// ---------------------------------------------------------------------------
// RWText									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

RWText::RWText (int inOrder)
	:	RWObject (inOrder),
//		mStyleID (0),
//		mExpandH (false),
		mExpandV (false),
		mIsDynamic (false),
		mIsAttributed (false),
		mKeepTogether (false),
		mDrawIfEmpty (eEmpty_Draw),
		mFrame (false),
		mFrameOffset (2),
		mFrameThickness (1),
		mFrameColor (cBlackColor),
//		mVarName (0),
//		mVarValue (0),
		mStyle (0),
//		mText (0),
		mPrintText (0)
{
}

// ---------------------------------------------------------------------------
// ~RWText									Destructor			   [protected]
// ---------------------------------------------------------------------------

RWText::~RWText (void)
{
//	mVarName.Free();
//	mText.Free();
	if (mPrintText)
		delete mPrintText;

	return;
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWText::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWObject::Parse (inReport, inNode);
	long	mStyleID = 0;
//	if (node)
//		node = node->FirstChild (GetKind() == eObject_Text ? "TextProps" : "VariableProps");

	RWXmlNode	elem = inNode.Child (GetKind() == eObject_Text ? u"TextProps" : u"VariableProps");
	if (!elem)
		elem = inNode;

	if (elem)
	{
		long			lVal;

		for (const auto &[name, value] : elem.Attributes())
		{

			if (RWStr::EqualsNoCase (name, "style"))
			{
				mStyleID = 0;
				RWStr::ReadNumber (value, mStyleID);
			}
/*
			else if (RWStr::EqualsNoCase (name, "expandH"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mExpandH = (lVal != 0);
			}
*/
			else if (RWStr::EqualsNoCase (name, "expandV"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mExpandV = (lVal != 0);
			}
			else if (RWStr::EqualsNoCase (name, "dynamic"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mIsDynamic = (lVal != 0);
			}
			else if (RWStr::EqualsNoCase (name, "attributed"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mIsAttributed = (lVal != 0);
			}
			else if (RWStr::EqualsNoCase (name, "keepTogether"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mKeepTogether = (lVal != 0);
			}
			/*
			else if (RWStr::EqualsNoCase (name, "drawIfEmpty"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				if (lVal >= eEmpty_Draw && lVal <= eEmpty_RemoveRow)
					mDrawIfEmpty = EEmpty (lVal);
			}
*/
			else if (RWStr::EqualsNoCase (name, "empty"))
			{
				if (RWStr::ReadNumber (value, lVal))
				{
					if (lVal >= eEmpty_Draw && lVal <= eEmpty_RemoveRow)
						mDrawIfEmpty = EEmpty (lVal);
				}
				else if (RWStr::EqualsNoCase (value, "draw"))
					mDrawIfEmpty = eEmpty_Draw;
				else if (RWStr::EqualsNoCase (value, "remove"))
					mDrawIfEmpty = eEmpty_Remove;
				else if (RWStr::EqualsNoCase (value, "remove row"))
					mDrawIfEmpty = eEmpty_RemoveRow;
			}
			else if (RWStr::EqualsNoCase (name, "frame"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mFrame = (lVal != 0);
			}
			else if (RWStr::EqualsNoCase (name, "frameOffset"))
			{
				mFrameOffset = 2;
				RWStr::ReadNumber (value, mFrameOffset);
				if (mFrameOffset < 0 || mFrameOffset > 20)
					mFrameOffset = 2;
			}
			else if (RWStr::EqualsNoCase (name, "frameThickness"))
			{
				mFrameThickness = 1;
				RWStr::ReadNumber (value, mFrameThickness);
				if (mFrameThickness < 0 || mFrameThickness > 10)
					mFrameThickness = 1;
			}
			else if (RWStr::EqualsNoCase (name, "frameColor"))
			{
				mFrameColor = value.c_str();
			}
			else if (RWStr::EqualsNoCase (name, "var"))
			{
				mVarName = value;
			}
			else if (RWStr::EqualsNoCase (name, "val"))
			{
				mVarValue.SetReal (RWStr::ToDouble (value).value_or (0));
			}
		}

		mText = RWTools::ParseIntoText (elem);
	}

	//mbs 07052010	support attributed text
	if (	mIsDynamic
		&&	(	mText.empty()
			 || (mIsAttributed && !RWStr::Contains (mText, u"&lt;%"))
			 || (not mIsAttributed && !RWStr::Contains (mText, u"<%"))
			 )
		)
		mIsDynamic = false;

	if (not mFrame)
		mFrameOffset = 0;

	mStyle = inReport->GetStyle (mStyleID);

/*
	if (mIsDynamic)
	{
		size_t	textLen = strlen (mText);
		RWString	startPos = mText;
		while ((startPos = strstr (startPos, "<%")) != NULL && size_t (startPos - mText) < textLen)
		{
			RWString	endPos = strstr (startPos + 2, "%>");
			if (endPos != NULL)
			{
				size_t	nameLen = endPos - startPos - 2;
				if (nameLen > 0)
				{
					char16_t	var [36];
					RWString	fmtPos;
					for (fmtPos = startPos + 2; fmtPos < endPos && *fmtPos != ';'; fmtPos++)
						;
					if (fmtPos < endPos)
						nameLen = fmtPos - startPos - 2;
					if (nameLen >= sizeof (var))
						nameLen = sizeof (var) - 1;
					memcpy (var, startPos + 2, nameLen);
					var [nameLen] = '\0';
					GetReportWriter()->CreateVariable (var);
				}
				startPos = endPos + 2;
			}
			else
				break;
		}
	}
*/
	if (!mVarName.empty())
		GetReportWriter()->CreateVariable (mVarName);

//	if (not mStyle->ShouldWrap())
//		mKeepTogether = true;
	if (mStyle->GetRotation() != 0)	//mbs 14012010
		mKeepTogether = true;
	return;
}


// ---------------------------------------------------------------------------
// GetGrow															  [public]
// ---------------------------------------------------------------------------

bool
RWText::GetGrow (void)
const
{
//	return mExpandV || mExpandH;
	return mExpandV;
}

// ---------------------------------------------------------------------------
// GetGrowH															  [public]
// ---------------------------------------------------------------------------

bool
RWText::GetGrowH (void)
const
{
	//	return mExpandV || mExpandH;
	return mExpandH;
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
RWText::Reset (bool inAll)
{
	if (inAll || (mPrinted && mDraw >= eDraw_OnOverflow))	//mbs 26072010
	{
		if (mPrintText)
		{
//			delete mPrintText;
//			mPrintText = NULL;
			mPrintText->Reset();
		}
	}

	RWObject::Reset (inAll);

	return;
}


// ---------------------------------------------------------------------------
// SetKeepTogether													  [public]
// ---------------------------------------------------------------------------

void
RWText::SetKeepTogether (void)
{
	mKeepTogether = true;
	return;
}

// ---------------------------------------------------------------------------
// RemoveRow													  [public]
// ---------------------------------------------------------------------------

bool
RWText::RemoveRow (bool* outRemoveRow)
{
	RWString	text;
	bool		ret = false;
	if (mIsDynamic)
		text = ParseText();
	else
		text = std::exchange (mText, RWString());
	
	if (text.empty() && (mDrawIfEmpty == eEmpty_RemoveRow)) {
		*outRemoveRow = true;
		ret = true;
	}
	
	if (mIsDynamic)
		text.clear();
	else
		mText = std::exchange (text, RWString());	// no need to copy...		
	
	return ret;
}


// ---------------------------------------------------------------------------
// GetBounds														  [public]
// ---------------------------------------------------------------------------

bool
RWText::GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow)
{
//	SRect	r (ioRect);
//	RWObject::GetBounds (inComposer, r, not mKeepTogether, inIsOverflow, outRemoveRow);
	SRect	origRect (ioRect);

	RWString	text;
	if (mIsDynamic)
		text = ParseText();
	else
		text = std::exchange (mText, RWString());

	bool	fit = true;
	bool	proceed = true;
	if (outRemoveRow)
		*outRemoveRow = false; // ?? first true stops processing, so why this?

	//mbs 10012010	added - otherwise the object would have to be growable
	if (text.empty() && mDrawIfEmpty != eEmpty_Draw)
	{
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
		mVirtPosition.right = mVirtPosition.left;
		mVirtPosition.bottom = mVirtPosition.top;
		proceed = false;
		if ((outRemoveRow) && (mDrawIfEmpty == eEmpty_RemoveRow))
			*outRemoveRow = true;
//		mPrinted = true;
	}

	if (proceed)
	{
		if (inFit && mFrame && (mDrawIfEmpty == eEmpty_Draw || not text.empty()) && ioRect.Height() < 2*mFrameOffset)	// will fail if mPosition.Height() is less than 2*mFrameOffset
			fit = proceed = false;
	}

	// pB 2012-6-04

	if (proceed)
	{
		SRect	r (mPosition);
		if (mFrame)
		{
			r *= mFrameOffset;
			if (mPosition.Width() == 0)
				r.left = r.right = mFrameOffset;
		}

		bool	measured = false;
		if (mPrintText && mIsDynamic)	//mbs 01012010 && is better than or!
		{
//			mPrintText->Init (inComposer, text, mStyle, r, mStyle->ShouldWrap(), mIsAttributed, false);
//			measured = true;
			delete mPrintText;
			mPrintText = NULL;
		}
//		else

		// for overflow or draw always start with first line
		if (mPrintText && (mDraw >= eDraw_OnOverflow)) {
			mPrintText->Reset ();
		}

		if (mPrintText == NULL)
		{
			inComposer.MeasureText (text, mStyle, r, mStyle->ShouldWrap(), mIsAttributed, false, &mPrintText);
			measured = true;
		}

		if (GetGrow())
		{
		if (mFrame && (mDrawIfEmpty == eEmpty_Draw || not r.IsEmpty()))
				r *= -mFrameOffset;
			if (mPrintText != NULL)
			{
				if (mStyle->GetRotation() != 0)	//mbs 14012010	GetHeight/GetWidth/... are nonsense for rotated text...
				{
					if (measured)
					{
						mVirtPosition.bottom = mVirtPosition.top + r.Height();
						mVirtPosition.right = mVirtPosition.left + r.Width();
					}
				}
				else
				{
					mVirtPosition.bottom = mVirtPosition.top + 2 * mFrameOffset + mPrintText->GetHeight() - mPrintText->GetPrintedHeight();
					mVirtPosition.right = mVirtPosition.left + 2 * mFrameOffset + mPrintText->GetWidth();
				}
			}
			else
			{
				mVirtPosition.bottom = mVirtPosition.top + r.Height();
				mVirtPosition.right = mVirtPosition.left + r.Width();
			}
		}
		else
		{
			if (mDrawIfEmpty == eEmpty_Draw || not r.IsEmpty())
			{
				if (mPosition.Width() > 0)
					mVirtPosition.right = mVirtPosition.left + mPosition.Width();
				else if (mPrintText != NULL)
				{
					if (mStyle->GetRotation() != 0)	//mbs 14012010	GetWidth is nonsense for rotated text...
					{
						if (measured)
							mVirtPosition.right = mVirtPosition.left + 2 * mFrameOffset + r.Width();
					}
					else
						mVirtPosition.right = mVirtPosition.left + 2 * mFrameOffset + mPrintText->GetWidth();
				}
				if (mPrintText != NULL)
				{
					double offset = 0; // pB added 2012-4-9
					if (mFrame) {
						offset = 2*mFrameOffset;
					}
					double minHeight = mPrintText->GetLineHeight() + offset > mPosition.Height() ? mPrintText->GetLineHeight() + offset : mPosition.Height();
					mVirtPosition.bottom = mVirtPosition.top + minHeight;
				}
				else
					mVirtPosition.bottom = mVirtPosition.top + mPosition.Height();
			}
		}

		//mbs 02092005	prevent infinite loop...
		if (inFit && mVirtPosition.right > ioRect.right)
			mVirtPosition.right = ioRect.right - 1;

		fit = RWObject::GetBounds (inComposer, ioRect, inFit /*|| (mKeepTogether && not inIsOverflow)*/, inIsOverflow, outRemoveRow);

		if (inFit && fit && mPrintText != NULL && (not mKeepTogether || inIsOverflow))
		{
			if (!mPrintText->IsPrinted()) {
				r = ioRect;
				if (mFrame)
					r *= mFrameOffset;
				inComposer.MeasureText (text, mStyle, r, mStyle->ShouldWrap(), mIsAttributed, true, &mPrintText);
				if (r.Height()  == 0) // could not fit a line
					fit = false;
			}
		}
	}

	if (proceed && fit && mAlignment != eAlign_None)
		AlignOnPage (origRect, ioRect);

	if (mIsDynamic)
		text.clear();
	else
		mText = std::exchange (text, RWString());	// no need to copy...

	return fit;
}


// ---------------------------------------------------------------------------
// FetchCalcValue													  [public]
// ---------------------------------------------------------------------------

void
RWText::FetchCalcValue (RWReportWriter *inWriter)
{
	if (!mVarName.empty())
		inWriter->SetVariable (mVarName, &mVarValue);
	return;
}


// ---------------------------------------------------------------------------
// Draw																  [public]
// ---------------------------------------------------------------------------

RWObject::EDrawState
RWText::Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow)
{
//	RWString	text;
//	if (mIsDynamic)
//		text = ParseText();
//	else
//		text.Attach (mText.Detach());

//	if (not text.IsEmpty() || mDrawIfEmpty == eEmpty_Draw)
	if (mPrintText || mDrawIfEmpty == eEmpty_Draw)
	{
		if (CanDraw (inRect, mPrinted)) {		// pB

			SRect	r (inRect);

			if (mFrame)
			{
				inComposer.DrawRect (inRect, mFrameThickness, true, mFrameColor, false, mFrameColor);
				r *= mFrameOffset;
			}
			inComposer.DrawTextBox (mPrintText? mPrintText->GetText(): RWString(), mStyle, r, mStyle->ShouldWrap(), mIsAttributed, GetGrow(), &mPrintText);
			if (mPrintText)
			{
				if (GetGrow())	// ignore multiline text not fitting into the text box... pB 2010-12 in case of one line not fully printed line
					mPrinted = mPrintText->IsPrinted();
				else
					mPrinted = mPrintText->GetPrintedLineCount () > 0; // pB 2011-01-25 was: true;
			}
			else
				mPrinted = true;
		}
	}
	else
		mPrinted = true;

//	if (mIsDynamic)
//		text.Free();
//	else
//		mText.Attach (text.Detach());	// no need to copy...

	return mPrinted ? eDrawState_Done : eDrawState_Vertical;
}

// ---------------------------------------------------------------------------
// AdjustBounds														  [public]
// ---------------------------------------------------------------------------

void
RWText::AdjustBounds (float hDelta, float vDelta)
{
	mVirtPosition.top += vDelta;
	mVirtPosition.left += hDelta;
	mVirtPosition.bottom += vDelta;
	mVirtPosition.right += hDelta;

	return;
}

// ---------------------------------------------------------------------------
// ParseText													   [protected]
// ---------------------------------------------------------------------------
// <% report_variable [ ; format ] %>

RWString
RWText::ParseText (void)
{
	RWString	text;

	//mbs 30042010	support attributed text
	if (not mText.empty())
	{
		long	textLen = (long) mText.size();
		long	curPos = 0, delta = 0, endPos;

		RWString	result (mText);		// was "(mText, textLen)": the substring *from* textLen, i.e. empty
		RWString   varName;
		RWString   format;
        
		while (curPos < textLen && RWTools::ParseTextForVar (mIsAttributed, mText, textLen, curPos, endPos, varName, format))
		{
			RWStringView	varname = varName;
			bool			encode = mIsAttributed;

			if (!varname.empty() && varname[0] == u'+')
			{
				varname.remove_prefix (1);
				encode = false;
			}
			RWString	varText;
			if (!varname.empty())
				varText = GetVariableText (RWString (varname), format);
			
            
			size_t	varLen;
			if (!varText.empty())
				varLen = varText.size();
			else
				varLen = 0;
			result.erase (curPos - delta, endPos - curPos);
			if (varLen > 0)
			{
				if (encode)
				{
					RWString	us = RWTools::EscapeAttributedString (varText);

//					us.AssignUTF8 ((const UTF8Char*) encoded.c_str(), encoded.length());
					result.insert (curPos - delta, us);
					varLen = us.length();
				}
				else
					result.insert (curPos - delta, varText);
			}
			delta += endPos - curPos - varLen;
			curPos = endPos;
		}
		text = result;
	}

	return text;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

RWVariable*
RWVariable::Create (RWReportData *inReport, RWXmlNode inNode, int inOrder)
{
	RWVariable	*var = new RWVariable (inOrder);
	var->Parse (inReport, inNode);

	return var;
}


// ---------------------------------------------------------------------------
// RWVariable								Default Constructor	   [protected]
// ---------------------------------------------------------------------------

RWVariable::RWVariable (int inOrder)
	:	RWText (inOrder),
//		mSource (0),
//		mFormat (0),
//		mCalcShow (false),
		mCalcType (ECalcType_None)
{
}

// ---------------------------------------------------------------------------
// ~RWVariable								Destructor			   [protected]
// ---------------------------------------------------------------------------

RWVariable::~RWVariable (void)
{
//	mSource.Free();
//	mFormat.Free();

	return;
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWVariable::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWText::Parse (inReport, inNode);

    RWXmlNode elem = inNode.Child (u"VariableProps");
	if (!elem)
		elem = inNode;
//	if (elem)
//	{
//		mSource.FromXML (elem.Attr (u"source"));
//		mFormat.FromXML (elem.Attr (u"format"));
//		elem = elem.Child (u"Calc");
//	}
	if (elem)
	{
		long			lVal;

		for (const auto &[name, value] : elem.Attributes())
		{

			if (RWStr::EqualsNoCase (name, "source") || RWStr::EqualsNoCase (name, "src"))
			{
				mSource.assign (value);
			}
			else if (RWStr::EqualsNoCase (name, "format"))
			{
				mFormat.assign (value.c_str());
			}
			else if (RWStr::EqualsNoCase (name, "calc"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				if (lVal >= ECalcType_None && lVal < ECalcType_Last)
					mCalcType = ECalcType (lVal);
			}
		}
	}

//	GetVariableText();

	if (!mSource.empty())
		GetReportWriter()->CreateVariable (mSource, mCalcType);

	return;
}


// ---------------------------------------------------------------------------
// GetBounds														  [public]
// ---------------------------------------------------------------------------

bool
RWVariable::GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow)
{
	GetVariableData();

	return RWText::GetBounds (inComposer, ioRect, inFit, inIsOverflow, outRemoveRow);
}


// ---------------------------------------------------------------------------
// Draw																  [public]
// ---------------------------------------------------------------------------

RWObject::EDrawState
RWVariable::Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow)
{
//	GetVariableData();	// no! before we Draw, GetBounds is called

	return RWText::Draw (inComposer, inRect, inIsOverflow);
}


// ---------------------------------------------------------------------------
// GetVariableData												   [protected]
// ---------------------------------------------------------------------------

void
RWVariable::GetVariableData (void)
{
	if (!mSource.empty())
	{
		RWString	v;
		RWValue		var;

		if (GetReportWriter()->GetVariable (mSource, var, mCalcType))
			v = GetReportWriter()->FormatVariable (var, mFormat);
		if (mPrintText &&
			/* mText != v  is not implemented*/
			mText != v && ( mText.empty() || v.empty()) //  ?? || memcmp (mText.c_str(), v.data_.c_str(), (mText.size() + 1) * CChar_Size) != 0)
			)
		{
			delete mPrintText;
			mPrintText = NULL;
		}
		mText = std::exchange (v, RWString());
	}

	return;
}


// ---------------------------------------------------------------------------
// SetVariableText												   [protected]
// ---------------------------------------------------------------------------

void
RWVariable::SetVariableText (const RWString inConstValue)
{
	mSource.clear();
	mText = inConstValue;

	return;
}
