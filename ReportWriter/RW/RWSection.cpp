# include	"RWSection.h"
# include	"RWReportWriter.h"


// ---------------------------------------------------------------------------
// RWSection								Constructor				  [public]
// ---------------------------------------------------------------------------

RWSection::RWSection (RWStringView inKind)
	:	mKind (eSectionKind_Body),
		mHeight (0),
		mMinSpace (0),
		mDraw (true),
		mKeepTogether (false),
		mFromBottom (false),
		mPageThrow (ePageThrow_None),
		mBottomSpace (0),
		mPrinted (false),
		mFixedHeight (false)
{
	if (STR_EQUALS (inKind, "Header"))
		mKind = eSectionKind_Header;
	else if (STR_EQUALS (inKind, "BreakHeader"))
		mKind = eSectionKind_BreakHeader;
//	else if (STR_EQUALS (inKind, "Body"))
//		mKind = eSectionKind_Body;
	else if (STR_EQUALS (inKind, "BreakFooter"))
		mKind = eSectionKind_BreakFooter;
//	else if (STR_EQUALS (inKind, "FillFooter"))
//		mKind = eSectionKind_FillFooter;
	else if (STR_EQUALS (inKind, "Footer"))
		mKind = eSectionKind_Footer;
//	else if (STR_EQUALS (inKind, "Page"))
//		mKind = eSectionKind_Page;
	else if (STR_EQUALS (inKind, "Watermark"))
		mKind = eSectionKind_Watermark;
}

RWSection::RWSection (ESection_Kind inKind)
	:	mKind (inKind),
		mHeight (0),
		mMinSpace (0),
		mDraw (true),
		mKeepTogether (false),
		mFromBottom (false),
		mPageThrow (ePageThrow_None),
		mBottomSpace (0),
		mPrinted (false),
		mFixedHeight (false)
{
}


// ---------------------------------------------------------------------------
// ~RWSection								Destructor				  [public]
// ---------------------------------------------------------------------------

RWSection::~RWSection (void)
{
//	mName.Free();

	return;
}


// ---------------------------------------------------------------------------
// GetKind															  [public]
// ---------------------------------------------------------------------------

RWSection::ESection_Kind
RWSection::GetKind (void)
const
{
	return mKind;
}


// ---------------------------------------------------------------------------
// GetObjects														  [public]
// ---------------------------------------------------------------------------

RWObjList *
RWSection::GetObjects (void)
{
	return &mObjects;
}


#if 0
// ---------------------------------------------------------------------------
// GetName															  [public]
// ---------------------------------------------------------------------------

const CText
RWSection::GetName (void)
const
{
	return mName;
}
#endif


// ---------------------------------------------------------------------------
// GetPageThrow														  [public]
// ---------------------------------------------------------------------------

RWSection::EPageThrow
RWSection::GetPageThrow (void)
const
{
	return mPageThrow;
}


// ---------------------------------------------------------------------------
// GetMinSpace														  [public]
// ---------------------------------------------------------------------------

float
RWSection::GetMinSpace (void)
const
{
	return mMinSpace;
}


// ---------------------------------------------------------------------------
// IsPrinted														  [public]
// ---------------------------------------------------------------------------

bool
RWSection::IsPrinted (void)
const
{
	return mPrinted;
}


// ---------------------------------------------------------------------------
// GetKeepTogether													  [public]
// ---------------------------------------------------------------------------

bool
RWSection::GetKeepTogether (void)
const
{
	return mKeepTogether;
}


// ---------------------------------------------------------------------------
// IsFromBottom														  [public]
// ---------------------------------------------------------------------------

bool
RWSection::IsFromBottom (void)
const
{
	return mFromBottom;
}


// ---------------------------------------------------------------------------
// FetchCalcValues													  [public]
// ---------------------------------------------------------------------------

void
RWSection::FetchCalcValues (RWReportWriter *inWriter)
{
	RWObjList::const_iterator	it;

	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		RWObject	*obj = *it;
		obj->FetchCalcValue (inWriter);
	}

	return;
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
RWSection::Reset (bool inAll)
{
	RWObjList::const_iterator	it;

	mBottomSpace = 0;
	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		RWObject	*obj = *it;
		obj->Reset (inAll);
		float	bottom = obj->GetPosition().bottom;
		if (bottom > mBottomSpace)
			mBottomSpace = bottom;
	}
 	if (mHeight > mBottomSpace) // pB objects can be bigger then section, then we do not have bottomspace
 		mBottomSpace = mHeight - mBottomSpace;
 	else
		mBottomSpace = 0;

	if (inAll)
		mPrinted = false;

	return;
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
RWSection::Parse (RWReportData * /*inReport*/, RWXmlNode inNode)
{
	for (const auto &[name, value] : inNode.Attributes())
	{
		if (STR_EQUALS (name, "height"))
		{
			mHeight = 0;
			RWStr::ReadNumber (value, mHeight);
		}
		else if (STR_EQUALS (name, "minSpace"))
		{
			mMinSpace = 0;
			RWStr::ReadNumber (value, mMinSpace);
		}
		else if (STR_EQUALS (name, "draw"))
		{
			mDraw = false;
			RWStr::ReadNumber (value, mDraw);
		}
		else if (STR_EQUALS (name, "keepTogether"))
		{
			mKeepTogether = false;
			RWStr::ReadNumber (value, mKeepTogether);
		}
		else if (STR_EQUALS (name, "bindToBottom"))
		{
			mFromBottom = false;
			RWStr::ReadNumber (value, mFromBottom);
		}
		else if (STR_EQUALS (name, "fixedHeight"))
		{
			mFixedHeight = false;
			RWStr::ReadNumber (value, mFixedHeight);
		}
		else if (STR_EQUALS (name, "pageThrow"))
		{
			long	lVal;
			if (RWStr::ReadNumber (value, lVal))
			{
				if (lVal >= ePageThrow_None && lVal <= ePageThrow_After)
					mPageThrow = EPageThrow (lVal);
			}
			else if (STR_EQUALS (value, "before"))
				mPageThrow = ePageThrow_Before;
			else if (STR_EQUALS (value, "after"))
				mPageThrow = ePageThrow_After;
			else
				mPageThrow = ePageThrow_None;
		}
#if	TARGET_DEBUG
		else if (STR_EQUALS (name, "iteration"))
			RWStr::ReadNumber (value, mIteration);
#endif
	}

	// header must not be bound to bottom...
	if (mKind <= eSectionKind_Body)
		mFromBottom = false;

	return;
}


// ---------------------------------------------------------------------------
// WillingToPrint													  [public]
// ---------------------------------------------------------------------------

bool
RWSection::WillingToPrint (RWReportWriter *inWriter, bool inCollision)
const
{
	bool	printIt = mDraw && not mPrinted;

	return printIt;
}


// ---------------------------------------------------------------------------
// PositionObjects													  [public]
// ---------------------------------------------------------------------------
// position all objects in a section

void
RWSection::PositionObjects (RWReportWriter *inWriter, RWPageComposer &/*inComposer*/, bool inIsOverflow)
{
	SRect	frame;

	inWriter->GetPageBounds (frame);
	inWriter->PositionObjects (&mObjects, frame, false, inIsOverflow);

	return;
}


// ---------------------------------------------------------------------------
// GetBounds														  [public]
// ---------------------------------------------------------------------------

bool
RWSection::GetBounds (RWReportWriter *inWriter, RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow)
{
	bool	fit = true;
	bool elementFit = true;

	if (mKeepTogether && not inIsOverflow) // pB ????
		fit = false; // in this case we must fit explicitly, but the coondition is not used?

	if (ioRect.Height() == 0) // pB
		return false;

	if (mFixedHeight)
	{
		if (mHeight > ioRect.Height()) {
			return false;
		}
		else
		{
			if (mFromBottom)
				ioRect.top = ioRect.bottom - mHeight;
			else
				ioRect.bottom = ioRect.top + mHeight;
			return true;
		}
	}

	// pB here follows handling of dynamic height

	//	if (WillingToPrint (inWriter))
	{
		double	height = ioRect.top;
		double	width = ioRect.left;

		RWObjList::const_iterator	it;

		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			RWObject	*obj = *it;
			SRect		r (ioRect);
			r.bottom -= mBottomSpace; // will be added to height after measuring all objects
			if (obj->WillingToPrint (inIsOverflow))
			{
				if (not obj->GetBounds (inComposer, r, fit, inIsOverflow, NULL)) {
					elementFit = false;
					continue;
				}
				if (r.right > width)
					width = r.right;
				if (r.bottom > height)
					height = r.bottom;
			}
		}
		height -= ioRect.top;
		width -= ioRect.left;

		// if (height)  this was causing problems with empty section - we want them to consume space

		// here are checks if we fit into space
		if (mKeepTogether && inFit && height > ioRect.Height())
			fit = false; // fixed height does not fit in
		else {
			fit = true;

			// requested height moved here - the space at the bottom does not have to fit
			height += mBottomSpace;

			// pB 1.2.7 - moved here so ioRect is modified after checks
			if (mFromBottom)
				ioRect.top = ioRect.bottom - height;
			else
				ioRect.bottom = ioRect.top + height;
			//mbs 04082010	don't shrink page width -> centering of objects...
			if (ioRect.right < ioRect.left + width)
				ioRect.right = ioRect.left + width;

			if (!inIsOverflow && height == 0 && !elementFit) { // pB 2012 on this (first ) page nothing fits ??
				fit = false; // first body on page and nothing fits
			}
		}
	}

	return fit;
}


// ---------------------------------------------------------------------------
// Draw																  [public]
// ---------------------------------------------------------------------------

RWObject::EDrawState
RWSection::Draw (RWReportWriter * /*inWriter*/, RWPageComposer &inComposer, SRect &ioRect, bool inIsOverflow)
{
	int							state = RWObject::eDrawState_Done;
	RWObjList::const_iterator	it;

	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		RWObject	*obj = *it;
		SRect		r (ioRect);
		if (obj->WillingToPrint (inIsOverflow))
		{
			int outState = 0;
			if (obj->GetBounds (inComposer, r, true, inIsOverflow, NULL))
				outState = obj->Draw (inComposer, r, inIsOverflow);
			else
				outState = RWObject::eDrawState_Vertical;
			if (not mFixedHeight) //pB if fixed height, what is printed is printed
				state |= outState;
		}
	}

	if (state == RWObject::eDrawState_Done) {
		mPrinted = true;
	}

	return RWObject::EDrawState (state);
}


// ---------------------------------------------------------------------------
// ExpandFillFooter													  [public]
// ---------------------------------------------------------------------------
// expand all objects in a FillFooter section

void
RWSection::ExpandFillFooter (const SRect &inRect)	//mbs 06012010
{
	if (mKind == eSectionKind_FillFooter)	//mbs 07032010	== instead of = !!!
	{
		mHeight = inRect.Height();
		RWObjList::const_iterator	it;
		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			RWObject	*obj = *it;
			if (obj->GetBinding())	//mbs 13012010	implicit binding?
			{
				SRect	pos = obj->GetPosition();
				if (mHeight > pos.top)
					obj->AdjustBounds (0, mHeight - pos.bottom);
				else
					obj->AdjustBounds (0, - pos.Height());
			}
		}
	}
	return;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// RWHeaderFooterSection					Constructor				  [public]
// ---------------------------------------------------------------------------

RWHeaderFooterSection::RWHeaderFooterSection (RWStringView inKind)
	:	RWSection (inKind),
		mFixed (-1),
		mFirstPage (true),
		mEvenPage (1),
		mOddPage (1),
		mLastPage (true)
{
	if (mKind == eSectionKind_Footer)
		mFromBottom = true;
}


// ---------------------------------------------------------------------------
// ~RWHeaderFooterSection					Destructor				  [public]
// ---------------------------------------------------------------------------

RWHeaderFooterSection::~RWHeaderFooterSection (void)
{
}


// ---------------------------------------------------------------------------
// GetMove															  [public]
// ---------------------------------------------------------------------------

#if	0
float
RWHeaderFooterSection::GetMove (void)
const
{
	return mFixed;
}
#endif


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
RWHeaderFooterSection::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWSection::Parse (inReport, inNode);

	long	fillPage = 0;

	for (const auto &[name, value] : inNode.Attributes())
	{
		if (STR_EQUALS (name, "fixed"))
		{
			mFixed = 0;
			RWStr::ReadNumber (value, mFixed);
		}
		else if (STR_EQUALS (name, "firstPage"))
		{
			mFirstPage = false;
			RWStr::ReadNumber (value, mFirstPage);
		}
		else if (STR_EQUALS (name, "evenPage"))
		{
			RWStr::ReadNumber (value, mEvenPage);
			if (mEvenPage < 0 || mEvenPage > 2)
				mEvenPage = 1;
		}
		else if (STR_EQUALS (name, "oddPage"))
		{
			RWStr::ReadNumber (value, mOddPage);
			if (mOddPage < 0 || mOddPage > 2)
				mOddPage = 1;
		}
		else if (STR_EQUALS (name, "lastPage"))
		{
			mLastPage = false;
			RWStr::ReadNumber (value, mLastPage);
		}
		else if (STR_EQUALS (name, "fillPage"))
		{
			fillPage = 0;
			RWStr::ReadNumber (value, fillPage);
		}
	}

	if (mKind == eSectionKind_Footer && fillPage)
	{
		mKind = eSectionKind_FillFooter;
		// fill footer must not be fixed size/position...
		mFixed = -1;
		// mHeight = 0;
		if (mMinSpace < mHeight)
			mMinSpace = mHeight;
		mPageThrow = ePageThrow_None;
		mFromBottom = false;
	}

	return;
}


// ---------------------------------------------------------------------------
// WillingToPrint													  [public]
// ---------------------------------------------------------------------------

bool
RWHeaderFooterSection::WillingToPrint (RWReportWriter *inWriter, bool inCollision)
const
{
	bool	printIt = mDraw && not mPrinted;

	if (printIt)
	{
		int		curPage = inWriter->GetCurrentPage();
		bool	lastPage = inWriter->IsLastPage (inCollision);

		if ( (curPage == 1) && lastPage) // pB 2011-01-17 special handling of one-page reports
		{
			if (mFirstPage && mLastPage)  // both
				printIt = true;
			else if (mFirstPage && (mOddPage == 1))  // first, even and last
				printIt = true;
			else if (mFirstPage || mLastPage)  // at least one
				printIt = ((mEvenPage == 0) && (mOddPage == 0));
			else
				printIt = false;
		}
		else
		{
			if (curPage == 1) {
				printIt = mFirstPage;
			}

			if (lastPage) {
				printIt =  (mLastPage || ((curPage & 1) == 0 && (mEvenPage == 1)) || ((curPage & 1) == 1 && (mOddPage == 1)));
			}

			if ((curPage != 1) && !lastPage)
			{
				printIt = (((curPage & 1) == 0 && (mEvenPage != 0)) || ((curPage & 1) == 1 && (mOddPage != 0)));
			}
		}
		/* printIt =	( mFirstPage && curPage == 1 )
			||	( mLastPage && lastPage)
			||	( mEvenPage && (curPage & 1) == 0 && (mEvenPage == 1 || (not lastPage) ) )
			||	( mOddPage  && (curPage & 1) == 1 && (mOddPage == 1 || (curPage != 1 && not lastPage) ) )
			; */

	}

	return printIt;
}


// ---------------------------------------------------------------------------
// PositionObjects													  [public]
// ---------------------------------------------------------------------------
// position all objects in a section, not called for Page section

void
RWHeaderFooterSection::PositionObjects (RWReportWriter *inWriter, RWPageComposer &/*inComposer*/, bool inIsOverflow)
{
	SRect	frame;

	inWriter->GetPageBounds (frame);
	if (mFixedHeight) // pB mHeight > 0)	// fixed height
	{
		if (frame.top + mHeight > frame.bottom)		// does not fit
		{
			printf ("RWHeaderFooterSection::PositionObjects: section does not fit\n");
			throw -5L;
		}
		frame.bottom = frame.top + mHeight;
	}

	inWriter->PositionObjects (&mObjects, frame, mFixedHeight, inIsOverflow);

	return;
}


// ---------------------------------------------------------------------------
// GetBounds														  [public]
// ---------------------------------------------------------------------------

bool
RWHeaderFooterSection::GetBounds (RWReportWriter *inWriter, RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow)
{
	bool	fit = true;

//	if (WillingToPrint (inWriter))
	{
		float	height = 0;
		float	width = 0;

		if (mFixed >= 0)	// fixed position
		{
			if (mKind == eSectionKind_Header)
				ioRect.top = mFixed;					// header from top
			else
			{
				SRect	page;
				inWriter->GetPageBounds (page);
				ioRect.bottom = page.bottom - mFixed;	// footer from bottom
			}
		}

		height = mHeight;	// pB 2010-9
		if (mFixedHeight)	// fixed height
		{
//			if (mKind == eSectionKind_Header)
			if (mFromBottom)
				ioRect.top = ioRect.bottom - mHeight;	// footer from bottom
			else
				ioRect.bottom = ioRect.top + mHeight;	// header from top
			return true;
		}

		height = ioRect.top; // pB changed from +=
		width = ioRect.left;

		RWObjList::const_iterator	it;

		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			RWObject	*obj = *it;
			SRect		r (ioRect);
			r.bottom -= mBottomSpace; // will be added to height after measuring all objects
			if (obj->WillingToPrint (inIsOverflow))
			{
				if (not obj->GetBounds (inComposer, r, inFit, inIsOverflow, NULL))
					return false;
				if (r.bottom > height)
					height = r.bottom;
				if (r.right > width)
					width = r.right;
			}
		}
		height -= ioRect.top;
		width -= ioRect.left;

		if (height)
			height += mBottomSpace;

		if (mKind != eSectionKind_FillFooter)
		{
			if (mFromBottom)
				ioRect.top = ioRect.bottom - height;
			else
				ioRect.bottom = ioRect.top + height;
		}
		//mbs 04082010	don't shrink page width -> centering of objects...
		if (ioRect.right < ioRect.left + width)
			ioRect.right = ioRect.left + width;
	}

	return fit;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// RWBreakSection							Constructor				  [public]
// ---------------------------------------------------------------------------
RWBreakSection::RWBreakSection (RWStringView inKind)
	:	RWSection (inKind),
		mLevel (0),
//		mBreakOn (0),
		mPrintAlways (false),
		mIsBreak (true)
{
}


// ---------------------------------------------------------------------------
// ~RWBreakSection							Destructor				  [public]
// ---------------------------------------------------------------------------

RWBreakSection::~RWBreakSection (void)
{
//	mBreakOn.Free();

	return;
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
RWBreakSection::Reset (bool inAll)
{
	RWSection::Reset (inAll);

	if (inAll)
		mIsBreak = true;

	return;
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
RWBreakSection::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWSection::Parse (inReport, inNode);

	for (const auto &[name, value] : inNode.Attributes())
	{
		if (STR_EQUALS (name, "level"))
		{
			mLevel = 0;
			RWStr::ReadNumber (value, mLevel);
		}
		else if (STR_EQUALS (name, "always"))
		{
			mPrintAlways = false;
			RWStr::ReadNumber (value, mPrintAlways);
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// WillingToPrint													  [public]
// ---------------------------------------------------------------------------

bool
RWBreakSection::WillingToPrint (RWReportWriter *inWriter, bool inCollision)
const
{
	bool	printIt = mDraw && (mPrintAlways || mIsBreak);

	return printIt;
}


// ---------------------------------------------------------------------------
// IsPrintAlways													  [public]
// ---------------------------------------------------------------------------

bool
RWBreakSection::IsPrintAlways ()
const
{
	return mDraw && mPrintAlways;
}

// ---------------------------------------------------------------------------
// GetLevel															  [public]
// ---------------------------------------------------------------------------

int
RWBreakSection::GetLevel (void)
const
{
	return mLevel;
}


// ---------------------------------------------------------------------------
// GetBreakOn														  [public]
// ---------------------------------------------------------------------------
/*
const CText
RWBreakSection::GetBreakOn (void)
const
{
	return mBreakOn;
}
*/


// ---------------------------------------------------------------------------
// ProcessBreak														  [public]
// ---------------------------------------------------------------------------

void
RWBreakSection::ProcessBreak (long inBreakLevel)
{
	mIsBreak = inBreakLevel <= mLevel;
	return;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// RWPageSection							Constructor				  [public]
// ---------------------------------------------------------------------------
RWPageSection::RWPageSection (void)
	:	RWSection (eSectionKind_Page)
{
}


// ---------------------------------------------------------------------------
// ~RWPageSection							Destructor				  [public]
// ---------------------------------------------------------------------------

RWPageSection::~RWPageSection (void)
{
//	mPageOrientation.Free();
//	mPageSize.Free();

	return;
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
RWPageSection::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWSection::Parse (inReport, inNode);

	for (const auto &[name, value] : inNode.Attributes())
	{
		if (STR_EQUALS (name, "Orientation"))
			mPageOrientation = value;
		else if (STR_EQUALS (name, "Size"))
			mPageSize = value;
	}

	return;
}


// ---------------------------------------------------------------------------
// GetPageOrientation												  [public]
// ---------------------------------------------------------------------------

const CText
RWPageSection::GetPageOrientation (void)
const
{
	return mPageOrientation;
}


// ---------------------------------------------------------------------------
// GetPageSize														  [public]
// ---------------------------------------------------------------------------

const CText
RWPageSection::GetPageSize (void)
const
{
	return mPageSize;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// RWWatermarkSection						Constructor				  [public]
// ---------------------------------------------------------------------------

RWWatermarkSection::RWWatermarkSection (RWStringView inKind)
	:	RWHeaderFooterSection (inKind),
		mOnTop (false)
{
}


// ---------------------------------------------------------------------------
// ~RWWatermarkSection						Destructor				  [public]
// ---------------------------------------------------------------------------

RWWatermarkSection::~RWWatermarkSection (void)
{
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
RWWatermarkSection::Parse (RWReportData *inReport, RWXmlNode inNode)
{
	RWHeaderFooterSection::Parse (inReport, inNode);

	if (inNode.HasAttr (u"onTop"))
		mOnTop = inNode.AttrInt (u"onTop", 0) != 0;

	mFixed = -1;
	mHeight = 0;
	mMinSpace = 0;
	mPageThrow = ePageThrow_None;
	mKeepTogether = false;
	mFromBottom = false;

	return;
}


// ---------------------------------------------------------------------------
// IsOnTop															  [public]
// ---------------------------------------------------------------------------

bool
RWWatermarkSection::IsOnTop (void)
const
{
	return mOnTop;
}
