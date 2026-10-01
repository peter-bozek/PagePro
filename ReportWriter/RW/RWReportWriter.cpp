# include	"RWReportWriter.h"
# include	"RWReportData.h"
# include	"RWDataSource.h"
//# include	"RWTextFilePageComposer.h"
# include	"RWReportVariables.h"
# include	<cmath>
# include	<algorithm>

# if	_4D_Package_
extern	"C"		void Yield4D (void);
# endif

# include	"SRLicense.h"

#if	MACVER
# include	"RWMacPageComposer.h"
struct	DrawReportArgs
{
	OSStatus		outResult;
	RWReportWriter	*inThis;
};
#endif


// ---------------------------------------------------------------------------
// RWReportWriter							Constructor				  [public]
// ---------------------------------------------------------------------------

RWReportWriter::RWReportWriter (RWDataSource &inDataSource, RWReportData &inData, RWPageComposer &inComposer)
	:	mSource (inDataSource),
		mData (inData),
		mComposer (inComposer),
		mPageBounds (0, 0, 0, 0),
		mNumHorPages (0),
		mNumSubPages (0),
		mPageSection (0),
		mCurrentBody (0),
		mFetchRecord (true),
		mBreakHeaders (0),
		mBreakLevels (-1),
		mBreakLevel (0),
		mLastError (0)	//mbs 08102010
{
	mData.SetReportWriter (this);
	RWInitReportVariable (mVarNames);
	return;
}


// ---------------------------------------------------------------------------
// ~RWReportWriter							Destructor				  [public]
// ---------------------------------------------------------------------------

RWReportWriter::~RWReportWriter (void)
{
	if (mNumSubPages != NULL)
		delete [] mNumSubPages;
	if (mNumHorPages != NULL)
		delete [] mNumHorPages;
	if (mBreakHeaders != NULL)
		delete [] mBreakHeaders;

	return;
}


// ---------------------------------------------------------------------------
// GetReportVariable												  [public]
// ---------------------------------------------------------------------------

int
RWReportWriter::GetReportVariable (const RWString inName)
const
{
	int	index;
	RWString	name (inName);
	for (index = 0; index < RW_VarNamesRWCount; index++)
		if (name == mVarNames [index])
			return index;

	return -1;
}


// ---------------------------------------------------------------------------
// CreateVariable													  [public]
// ---------------------------------------------------------------------------

void
RWReportWriter::CreateVariable (const RWString inName, ECalcType inCalc)
{
	if (GetReportVariable (inName) == RW_VarNotFound)
	{
		// create break value computation
		if (inCalc != ECalcType_None)
			mCalculator.Add (inName);

		// create the variable
		mSource.CreateVariable (inName);
	}

	return;
}


// ---------------------------------------------------------------------------
// GetCalculatedValue												  [public]
// ---------------------------------------------------------------------------

bool
RWReportWriter::GetCalculatedValue (const RWString inName, RWValue &outVar)
const
{
	return GetVariable (inName, outVar, ECalcType_CurrentValue);
}


// ---------------------------------------------------------------------------
// GetVariable														  [public]
// ---------------------------------------------------------------------------
// Get report variable

bool
RWReportWriter::GetVariable (const RWString inName, RWValue &outVar, ECalcType inCalc)
const
{
	bool	found = true;
	int		index = GetReportVariable (inName);

	outVar.Free();

	switch (index)
	{
		case RW_VarNotFound:
			// get break value
			if (inCalc > ECalcType_None)
			{
				double	d;
				found = mCalculator.GetValue (inName, RWCalculatedValue::RWCalculatedType (inCalc), d);
				if (found)
					outVar.SetReal (d);
			}
			else
				found = mSource.GetVariable (inName, outVar);
			break;

		case RW_VarPage:
//		case RW_VarSRPage:
			outVar.SetInteger (GetCurrentPage());
			break;

		case RW_VarPages:
			outVar.SetInteger (GetNumberOfPages());
			break;

		case RW_VarSubPage:
			outVar.SetInteger (GetCurrentSubPage());
			break;

		case RW_VarSubPages:
			outVar.SetInteger (GetNumberOfSubPages());
			break;

		case RW_VarHorPage:
			outVar.SetInteger (GetCurrentHorPage());
			break;
			
		case RW_VarHorPages:
			outVar.SetInteger (GetNumberOfHorPages());
			break;
		case RW_VarFrame:
			outVar.SetInteger (GetCurrentFrame());
			break;

		case RW_VarFrames:
			outVar.SetInteger (GetNumberOfFrames());
			break;

		case RW_VarDateTime:
			outVar.SetInteger ((long) mPrintTime, RWValue::eValue_DateTime);
			break;

		case RW_VarDate:
		case RW_VarTime:
		{
			struct	tm	*tm = localtime (&mPrintTime);
			if (index == RW_VarDate)
				outVar.SetInteger (tm->tm_mday + ((tm->tm_mon + 1) << 5) + ((tm->tm_year + 1900L) << 9), RWValue::eValue_Date);
			else
				outVar.SetInteger (tm->tm_sec + tm->tm_min * 60L + tm->tm_hour * 3600L, RWValue::eValue_Time);
			break;
		}

		case RW_VarName:
			outVar.SetText (mData.GetName());
			break;
	}

	return found;
}


// ---------------------------------------------------------------------------
// SetVariable														  [public]
// ---------------------------------------------------------------------------

void
RWReportWriter::SetVariable (const RWString inName, RWValue *inVar)
{
	if (GetReportVariable (inName) == RW_VarNotFound)
	{
		mSource.SetVariable (inName, inVar);
	}

	return;
}


// ---------------------------------------------------------------------------
// FormatVariable													  [public]
// ---------------------------------------------------------------------------
// Format variable depending on DataSource

RWString
RWReportWriter::FormatVariable (RWValue &inVar, const RWString inFormat)
const
{
	return mSource.FormatVariable (inVar, inFormat);
}


// ---------------------------------------------------------------------------
// Report															  [public]
// ---------------------------------------------------------------------------
// Generate the report

void*
RWReportWriter::Report (size_t *outSize)
{
	mPrintTime = time (NULL);
	mNumPages = mTotalPages = mMaxHorPage = 0;
	mLastPageCollision = false;

	if (mNumSubPages)	{
		delete [] mNumSubPages;
		mNumSubPages = NULL;
	}
	if (mNumHorPages)	{
		delete [] mNumHorPages;
		mNumHorPages = NULL;
	}
	
	// give the DataSource a chance to get data...
	mSource.ParseReport (mData.GetReport());

# if	_4D_Package_
	Yield4D();
# endif

	// create Sections/Objects from XML
	mData.ParseReport();

# if	_4D_Package_
	Yield4D();
# endif

	// give the composer a chance to get default page size and orientation
	mComposer.ParseReport (mData.GetReport());
	mComposer.SetJobName (mData.GetName());

# if	_4D_Package_
	Yield4D();
# endif

	if (mData.IsDynamic())
		PrepareDynamicReport();

	// try without last page headers/footers
	mCountingPages = true;
	DrawReport();

	// try again with last page headers/footers
	mNumPages = mCurPage;
	mNumSubPages = new unsigned long [mCurPage + 2];	// one for page zero, one to be safe
	mNumHorPages = new unsigned long [mCurPage + 2];	// one for page zero, one to be safe
	mTotalPages = 0;
	DrawReport();
	if (mCurPage != mNumPages)	// we have a collision on last page
//	if (mCurrentPage != mTotalPages)	//mbs 04052005	or should it be this way?
	{
		mLastPageCollision = true;
		mNumPages = mCurPage;
	}

	// and finally - do it!
	mCountingPages = false;

#if	TARGET_DEBUG
	printf ("\nRWReportWriter: mNumPages = %ld, mTotalPages = %ld, mLastPageCollision = %d\n", mNumPages, mTotalPages, (int) mLastPageCollision);
#endif

	mPrintTime = time (NULL);
#if	MACVER
//	if (static_cast <RWMacPageComposer*> (&mComposer)->IsQD())
//	{
//		DrawReportArgs	data = { noErr, this };
//		RW_RunInMainThread (DrawReportCB, &data);
//		if (data.outResult)
//			throw long (data.outResult);
//	}
//	else
#endif
    DrawReport();

	size_t	size;
	void	*result = mComposer.FinishReport (size);
	if (outSize)
		*outSize = size;

	return result;
}


// ---------------------------------------------------------------------------
// DrawReportCB												[static] [private]
// ---------------------------------------------------------------------------
// Draw the report

#if	MACVER
void
RWReportWriter::DrawReportCB (void *inData)
{
	DrawReportArgs	&data = *reinterpret_cast <DrawReportArgs*> (inData);
	data.outResult = noErr;
	try
	{
		if (data.inThis->mData.IsDynamic())
			data.inThis->DrawDynamicReport();
		else
			data.inThis->DrawStaticReport();
	}
	catch (long e)
	{
		data.outResult = e;
	}
	catch (...)
	{
		data.outResult = -1;
	}
}
#endif


// ---------------------------------------------------------------------------
// DrawReport														 [private]
// ---------------------------------------------------------------------------
// Draw the report

void
RWReportWriter::DrawReport (void)
{
	if (mData.IsDynamic())
		DrawDynamicReport();
	else
		DrawStaticReport();
}


// ---------------------------------------------------------------------------
// DrawStaticReport													 [private]
// ---------------------------------------------------------------------------
// Draw the report

void
RWReportWriter::DrawStaticReport (void)
{
	RWSectionList			*sections;
	RWSectionList::iterator	sit;
	RWObject::EDrawState	state;

	mCurPage = mCurSubPage = mCurrentPage = mCurHorPage = 1;
//	mSource.Reset();

	for ( ; ; )
	{
		RWPageSection	*page = NULL;
		// find the page willing to print
		sections = mData.GetBodySections();
		for (sit = sections->begin(); sit != sections->end(); sit++)
		{
			RWPageSection	*thisPage = static_cast <RWPageSection*> (*sit);
			// Reset Page section on start of the report
			if (mCurPage == 1 && mCurHorPage == 1)
				thisPage->Reset (true);
			if (thisPage->WillingToPrint (this, mLastPageCollision))	//mbs 04052005	use mLastPageCollision, not false
			{
				mComposer.GetPageBounds (thisPage->GetPageOrientation(), thisPage->GetPageSize(), mPageBounds);
				// Position all objects on start of the report
				if (mCurPage == 1 && mCurHorPage == 1)
					thisPage->PositionObjects (this, mComposer, false);
				if (page == NULL)
					page = thisPage;
			}
		}

		if (page == NULL)
		{
			mCurPage--;
			break;
		}

		state = DrawStaticPage (page);
		mCurrentPage++;
		if (mCountingPages)
			mTotalPages++;

		switch (state)
		{
			case RWObject::eDrawState_Done:	// try to find next page
				if (mCountingPages && mNumHorPages != NULL)
					mNumHorPages [mCurPage] = mCurHorPage;
				if (mCountingPages && mNumSubPages != NULL) {
					mNumSubPages [mCurPage] = mCurSubPage;
					if (mCurSubPage > 1) {
						int page = mCurSubPage;
						while (--page) {
							mNumSubPages [mCurPage - page] = mCurSubPage;
						}
					}
				}
				mCurHorPage = 1;
				mCurPage++;
				mCurSubPage++;
				break;

			case RWObject::eDrawState_Horizontal:
			case RWObject::eDrawState_Both:
				if (++mCurHorPage > mMaxHorPage)
					mMaxHorPage = mCurHorPage;
				if (mCountingPages && mNumHorPages != NULL)
					mNumHorPages [mCurPage] = mCurHorPage;
				if (mCurHorPage > 99)
				{
					printf ("RWReportWriter::DrawStaticReport: mCurSubPage > 99\n");
					throw -12L;
				}
				continue;

			case RWObject::eDrawState_Vertical:
				if (mCountingPages && mNumSubPages != NULL) {
					mNumSubPages [mCurPage] = mCurSubPage;
					if (mCurSubPage > 1) {
						int page = mCurSubPage;
						while (--page) {
							mNumSubPages [mCurPage - page] = mCurSubPage;
						}
					}
				}
				mCurHorPage = 1;
				mCurSubPage++;
				mCurPage++;
				if (mCurPage > 9999)
				{
					printf ("RWReportWriter::DrawStaticReport: mCurPage > 9999\n");
					throw -13L;
				}
				continue;
		}
		break;
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawStaticPage													 [private]
// ---------------------------------------------------------------------------
// Draw one page

RWObject::EDrawState
RWReportWriter::DrawStaticPage (RWPageSection *inBody)
{
	SRect					origRect, pageRect;
	RWSectionList			*sections;
	RWSection				*sec;
	RWSectionList::iterator	sit;
	float					collisionSpace = 0;
	int						state = RWObject::eDrawState_Done;
	RWWatermarkSection		*watermark = NULL;

	mComposer.GetPageBounds (inBody->GetPageOrientation(), inBody->GetPageSize(), origRect);
	mPageBounds = origRect;
	if (not mCountingPages)
	{
		mComposer.OpenNewPage (origRect, mCurrentPage, mTotalPages);
        mComposer.InitPagePosition();

		if (mCurrentPage > mComposer.GetLastPage())
			return RWObject::EDrawState (state);
		watermark = mData.GetWatermarkSection();

		if (watermark && not watermark->IsOnTop())
		{
			watermark->Reset (true);
			if (watermark->WillingToPrint (this, false))
			{
				pageRect = mPageBounds;
				watermark->PositionObjects (this, mComposer, false);
				if (watermark->GetBounds (this, mComposer, pageRect, true, false))
					watermark->Draw (this, mComposer, pageRect, false);
			}
		}
	}

	
	sections = mData.GetPageSections();
	// process headers & footers, draw headers
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = *sit;
		sec->Reset (true);
		if (sec->GetKind() == RWSection::eSectionKind_Header)
		{
			if (sec->WillingToPrint (this, mLastPageCollision))
			{
				pageRect = origRect;
				sec->PositionObjects (this, mComposer, false);
				if (sec->GetBounds (this, mComposer, pageRect, true, false))
				{
//					if (not pageRect.IsEmpty())
					{
						if (not mLastPageCollision || sec->WillingToPrint (this, false))
						{
							if (origRect.top < pageRect.bottom)
								origRect.top = pageRect.bottom;
							state |= sec->Draw (this, mComposer, pageRect, false);
						}
						else if (origRect.top < pageRect.bottom)
							collisionSpace += pageRect.bottom - origRect.top;
					}
				}
			}
		}
		else if (sec->GetKind() == RWSection::eSectionKind_Footer)
		{
			if (sec->WillingToPrint (this, mLastPageCollision))
			{
				pageRect = origRect;
				sec->PositionObjects (this, mComposer, false);
				if (sec->GetBounds (this, mComposer, pageRect, true, false))
				{
//					if (not pageRect.IsEmpty())
					{
						if (not mLastPageCollision || sec->WillingToPrint (this, false))
						{
							if (sec->IsFromBottom())
							{
								if (origRect.bottom > pageRect.top)
									origRect.bottom = pageRect.top;
							}
							else
							{
								if (origRect.top < pageRect.bottom)
									origRect.top = pageRect.bottom;
							}
//							state |= sec->Draw (this, mComposer, pageRect);
						}
						else if (origRect.bottom > pageRect.top)
							collisionSpace += origRect.bottom - pageRect.top;
					}
				}
			}
		}
	}

	/*
	 origRect contain page without headers and footers
	 mPageBounds contain page 
	 headers are printed
	 */
	
	if ((state & RWObject::eDrawState_Vertical) != 0)
	{
		/* can happen only if headers did not fit into page */
		printf ("RWReportWriter::DrawStaticPage: some Header/Footer does not fit vertically\n");
		throw -10;
	}

	pageRect.top = origRect.bottom;
	pageRect.left = mPageBounds.left;
	pageRect.bottom = mPageBounds.bottom;
	pageRect.right = mPageBounds.right;

# if	_4D_Package_
	Yield4D();
# endif

	// process body
	if (inBody != NULL)
	{
		float	start = origRect.top;
		origRect.bottom -= collisionSpace;
		if (mCurPage > 1 && mCurSubPage == 1)
		{
			inBody->Reset (false); // Reset objects but don't reset text fields
			MoveObjects (inBody->GetObjects(), origRect, false);
		}

		if (inBody->GetBounds (this, mComposer, origRect, true, false))
		{
//			if (not origRect.IsEmpty())
				state |= inBody->Draw (this, mComposer, origRect, false);
			if ((state & RWObject::eDrawState_Vertical) != 0 && start == origRect.bottom)
			{
				printf ("RWReportWriter::DrawStaticPage: Body does not fit vertically\n");
				throw -11L;	// nothing printed
			}
		}
		else
		{
			printf ("RWReportWriter::DrawStaticPage: Body does not fit on page even if it must\n");
			throw -3L;
		}
	}

# if	_4D_Package_
	Yield4D();
# endif

	// draw footers
	origRect.bottom = pageRect.bottom;
	origRect.left = mPageBounds.left;
	origRect.right = mPageBounds.right;
	mCalculator.SetLevel (0); // pB valid for footers only, PeekSection sets it for all other sections
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = *sit;
		if (sec->GetKind() == RWSection::eSectionKind_Footer)
		{
			if (sec->WillingToPrint (this, mLastPageCollision))
			{
				pageRect = origRect;
				sec->PositionObjects (this, mComposer, false);
				if (sec->GetBounds (this, mComposer, pageRect, true, false))
				{
//					if (not pageRect.IsEmpty())
					{
						if (not mLastPageCollision || sec->WillingToPrint (this, false))
						{
							if (sec->IsFromBottom())
							{
								if (origRect.bottom > pageRect.top)
									origRect.bottom = pageRect.top;
							}
							else
							{
								if (origRect.top < pageRect.bottom)
									origRect.top = pageRect.bottom;
							}
							state |= sec->Draw (this, mComposer, pageRect, false);
						}
					}
				}
			}
		}
	}

	if (not mCountingPages)
	{
		if (watermark && watermark->IsOnTop())
		{
			watermark->Reset (true);
			if (watermark->WillingToPrint (this, false))
			{
				pageRect = mPageBounds;
				watermark->PositionObjects (this, mComposer, false);
				if (watermark->GetBounds (this, mComposer, pageRect, true, false))
					watermark->Draw (this, mComposer, pageRect, false);
			}
		}

		RW_CheckLicense (&mComposer, &mPageBounds, true);

		mComposer.ClosePage();
	}

# if	_4D_Package_
	Yield4D();
# endif

	return RWObject::EDrawState (state);
}


// ---------------------------------------------------------------------------
// PrepareDynamicReport												 [private]
// ---------------------------------------------------------------------------

void
RWReportWriter::PrepareDynamicReport (void)
{
	RWSectionList			*sections;
	RWSectionList::iterator	sit;
	RWSection				*sec;
	int						breakLevel;

	mCurrentBody = NULL;
//	mBreakHeaders.clear();
	if (mBreakHeaders != NULL)
	{
		delete [] mBreakHeaders;
		mBreakHeaders = NULL;
	}
//	mBreakFooters.clear();
	mBreakLevels = -1;
	sections = mData.GetBodySections();
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = *sit;
		if (sec->GetKind() == RWSection::eSectionKind_Page)	// should be the first (& only first) section
			mPageSection = static_cast <RWPageSection*> (sec);
		else if (sec->GetKind() == RWSection::eSectionKind_Body)
		{
			if (mCurrentBody == NULL)
				mCurrentBody = sec;
		}
		else	// break header or footer
		{
			RWBreakSection	*bs = static_cast <RWBreakSection*> (sec);
			breakLevel = bs->GetLevel();
			if (breakLevel > mBreakLevels)
				mBreakLevels = breakLevel;
		}
	}

	//mbs 20092010	create break for totals
	if (mBreakLevels == -1 && mCalculator.GetVariables().size() > 0)
		mBreakLevels = 0;

	mBreakLevel = mBreakLevels;
	mCalculator.InitLevels (mBreakLevels + 1);
//	mBreakHeaders.resize (mBreakLevels + 1);
	if (mBreakLevels >= 0)
		mBreakHeaders = new RWBreakSection* [mBreakLevels + 1];
//	mBreakFooters.resize (mBreakLevels + 1);
	return;
}


bool
RWReportWriter::FindNextSection (void)
{
	bool		peekNext = mFetchRecord;
	RWSection	*sec = mCurrentBody;
	int			breakLevel;

	while (peekNext)
	{
		if (mPageIterator == mData.GetBodySections()->end())
			break;
		sec = *mPageIterator;
		if (	mPageSection != sec		//mbs 06022006	ignore [empty] Page section
			&&	sec->WillingToPrint (this, mLastPageCollision)	//mbs 04052005	use mLastPageCollision, not false
		)
			peekNext = false;
		else
		{
			if (sec->GetKind() == RWSection::eSectionKind_BreakFooter)
			{
				RWBreakSection	*bs = static_cast <RWBreakSection*> (sec);
				breakLevel = bs->GetLevel();
				if (mBreakLevels >= 0)
					mCalculator.ShuntTotals (breakLevel);
			}
			mPageIterator++;	// ignore invisible/empty section
		}
	}

	if (peekNext)
		mCurrentBody = NULL;
	else
	{
		if (mFetchRecord)
			mCurrentBody = sec;
	}

	return peekNext;
}


bool
RWReportWriter::PeekNextSection (void)
{
	if (FindNextSection())
		return true;

	if (	mCurrentBody->GetKind() == RWSection::eSectionKind_BreakHeader
		||	mCurrentBody->GetKind() == RWSection::eSectionKind_BreakFooter
	)
	{
		RWBreakSection	*bs = static_cast <RWBreakSection*> (mCurrentBody);
		mBreakLevel = bs->GetLevel();
		if (mBreakLevels >= 0)
			mCalculator.SetLevel (mBreakLevel);
	}
	else
	{
//		mBreakLevel = mBreakLevels;
		if (mBreakLevels >= 0)
			mCalculator.SetLevel (mBreakLevels);
	}

	return false;
}


bool
RWReportWriter::GetNextSection (void)
{
	if (PeekNextSection())
		return true;

	if (mFetchRecord)
	{
		mPageIterator++;
		mFetchRecord = false;
		if (mCurHorPage == 1)
			mCurrentBody->PositionObjects (this, mComposer, false);
	}

	return false;
}


// ---------------------------------------------------------------------------
// DrawDynamicReport												 [private]
// ---------------------------------------------------------------------------
// Draw the report

void
RWReportWriter::DrawDynamicReport (void)
{
	RWSectionList			*sections;
	RWSectionList::iterator	sit;
	RWSection				*sec;
	RWObject::EDrawState	state;

	mCurPage = mCurSubPage = mCurrentPage = mCurHorPage = 1;
//	mSource.Reset ();

	mBreakLevel = mBreakLevels;
//	mBreakHeaders.clear();
//	mBreakHeaders.resize (mBreakLevel + 1);
	if (mBreakLevels >= 0)
	{
		memset (mBreakHeaders, 0, (mBreakLevels + 1) * sizeof (RWBreakSection*));
		mCalculator.ShuntTotals (0);	//mbs 20092010	if no break headers/footers
	}
	mCurrentBody = NULL;
	sections = mData.GetBodySections();
	for (mPageIterator = sections->begin(); mPageIterator != sections->end(); mPageIterator++)
	{
		sec = *mPageIterator;
		sec->Reset (true);
#if 0
		if (mCurrentBody == NULL)
		{
			if (sec->GetKind() == RWSection::eSectionKind_Body)
				mCurrentBody = sec;
			else if (sec->GetKind() == RWSection::eSectionKind_BreakHeader)
			{
				int				breakLevel;
				RWBreakSection	*bs = static_cast <RWBreakSection*> (sec);
				breakLevel = bs->GetLevel();
				mBreakHeaders [breakLevel] = bs;
			}
		}
#endif
	}

	mPageIterator = sections->begin();
	mFetchRecord = true;
	mIsOverflow = false;

	for ( ; ; )
	{
		if (FindNextSection())
		{
			mCurPage--;
			mCurSubPage--;
			if (mCurrentPage == 1) {
				throw 1;
			}
			break;
		}

		state = DrawDynamicPage();
		mCurrentPage++;
		if (mCountingPages)
			mTotalPages++;

		switch (state)
		{
			case RWObject::eDrawState_Done:
				if (mCountingPages && mNumSubPages != NULL) {
					mNumSubPages [mCurPage] = mCurSubPage;
					if (mCurSubPage > 1) {
						int page = mCurSubPage;
						while (--page) {
							mNumSubPages [mCurPage - page] = mCurSubPage;
						}
					}
				}
				if (mCountingPages && mNumHorPages != NULL)
					mNumHorPages [mCurPage] = mCurHorPage;
				break;

			case RWObject::eDrawState_Horizontal:
			case RWObject::eDrawState_Both:
				if (++mCurHorPage > mMaxHorPage)
					mMaxHorPage = mCurHorPage;
				if (mCountingPages && mNumHorPages != NULL)
					mNumHorPages [mCurPage] = mCurHorPage;
				if (mCurHorPage > 99)
				{
					printf ("RWReportWriter::DrawDynamicReport: mCurSubPage > 99\n");
					throw -12L;
				}
				continue;

			case RWObject::eDrawState_Vertical:
				if (mCountingPages && mNumHorPages != NULL)
					mNumHorPages [mCurPage] = mCurHorPage;
				if (mCountingPages && mNumSubPages != NULL) {
					mNumSubPages [mCurPage] = mCurSubPage;
					if (mCurSubPage > 1) {
						int page = mCurSubPage;
						while (--page) {
							mNumSubPages [mCurPage - page] = mCurSubPage;
						}
					}
				}
				mCurHorPage = 1;
				mCurSubPage++;
				mCurPage++;
				if (mCurPage > 9999)
				{
					printf ("RWReportWriter::DrawDynamicReport: mCurPage > 9999\n");
					throw -13L;
				}
				continue;
		}
		break;
	}

	return;
}


// ---------------------------------------------------------------------------
// DrawDynamicPage													 [private]
// ---------------------------------------------------------------------------
// Draw one page

RWObject::EDrawState
RWReportWriter::DrawDynamicPage (void)
{
	SRect							origRect, pageRect;
	RWSectionList					*sections;
	RWSectionList::const_iterator	sit;
	RWSection						*sec;
	float							collisionSpace = 0;
	int								state = RWObject::eDrawState_Done;
	int								breakLevel, blMax;
	RWWatermarkSection				*watermark = NULL;
	bool							bodyLevel = false;
	int								bodyPrinted; // to check if any body was printed already
	
	mComposer.GetPageBounds (mPageSection->GetPageOrientation(), mPageSection->GetPageSize(), origRect);
 	mPageBounds = origRect;
    
	if (not mCountingPages)
	{
		if (mCurrentPage > mComposer.GetLastPage())
			return RWObject::EDrawState (state);
		mComposer.OpenNewPage (origRect, mCurrentPage, mTotalPages);
        mComposer.InitPagePosition();

		watermark = mData.GetWatermarkSection();

		if (watermark && not watermark->IsOnTop())
		{
			watermark->Reset (true);
			if (watermark->WillingToPrint (this, false))
			{
				pageRect = mPageBounds;
				watermark->PositionObjects (this, mComposer, false);
				if (watermark->GetBounds (this, mComposer, pageRect, true, false))
					watermark->Draw (this, mComposer, pageRect, false);
			}
		}
	}

	sections = mData.GetPageSections();
	// process headers & footers, draw headers
	mCalculator.SetLevel (0); // pB valid for headers only, PeekSection sets it for all other sections
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = *sit;
		sec->Reset (true);
		if (sec->GetKind() == RWSection::eSectionKind_Header)
		{
			if (sec->WillingToPrint (this, mLastPageCollision))
			{
				pageRect = origRect;
				sec->PositionObjects (this, mComposer, false);
				if (sec->GetBounds (this, mComposer, pageRect, true, false))
				{
//					if (not pageRect.IsEmpty())
					{
						if (not mLastPageCollision || sec->WillingToPrint (this, false))
						{
							if (origRect.top < pageRect.bottom)
								origRect.top = pageRect.bottom;
								state |=  sec->Draw (this, mComposer, pageRect, false); // pB this should set 
						}
						else if (origRect.top < pageRect.bottom)
							collisionSpace += pageRect.bottom - origRect.top;
					}
				}
			}
		}
		else if (sec->GetKind() == RWSection::eSectionKind_Footer)
		{
			if (sec->WillingToPrint (this, mLastPageCollision))
			{
				pageRect = origRect;
				sec->PositionObjects (this, mComposer, false);
				if (sec->GetBounds (this, mComposer, pageRect, true, false))
				{
//					if (not pageRect.IsEmpty())
					{
						if (not mLastPageCollision || sec->WillingToPrint (this, false))
						{
							if (sec->IsFromBottom())
							{
								if (origRect.bottom > pageRect.top)
									origRect.bottom = pageRect.top;
							}
							else
							{
								if (origRect.top < pageRect.bottom)
									origRect.top = pageRect.bottom;
							}
//							state |= sec->Draw (this, mComposer, pageRect);
						}
						else if (origRect.bottom > pageRect.top)
							collisionSpace += origRect.bottom - pageRect.top;
					}
				}
			}
		}
		else if  (sec->GetKind() == RWSection::eSectionKind_FillFooter)
		{
			origRect.bottom -= sec->GetMinSpace();
		}
	}

	/*
	 origRect contain page without headers and footers
	 mPageBounds contain page 
	 headers are printed
	 */
	
	if ((state & RWObject::eDrawState_Vertical) != 0)
	{
		/* can happen only if headers did not fit into page */
		printf ("RWReportWriter::DrawDynamicPage: some Header/Footer does not fit vertically!!!\n");
		throw -10L;
	}

	//pageRect is really a footer rect
	pageRect.top = origRect.bottom;
	pageRect.left = mPageBounds.left;
	pageRect.bottom = mPageBounds.bottom;
	pageRect.right = mPageBounds.right;

	PeekNextSection();

	// draw break headers with property "print on all pages"
	RWBreakSection	*bs;
	blMax = mBreakLevel;
	//	if (mCurrentBody->GetKind() != RWSection::eSectionKind_Body)  //pB returned back
	if (mCurrentBody->GetKind() == RWSection::eSectionKind_BreakHeader)
		blMax--;
	for (breakLevel = 0; breakLevel <= blMax; breakLevel++)
	{
		bs = mBreakHeaders [breakLevel];
		if (bs == NULL)		// no break header for this level
			continue;
		if (bs->WillingToPrint (this, mLastPageCollision))	// first time or reprint
		{
			bs->Reset (false); // pB 2011-1-25 do not reset printed status - we want to print only overflow items
			if (mBreakLevels >= 0)
				mCalculator.SetLevel (bs->GetLevel());
			SRect	r (origRect);
			bs->PositionObjects (this, mComposer, true);
			if (bs->GetBounds (this, mComposer, r, true, true))
			{
//				if (not r.IsEmpty())
				{
					if (not mLastPageCollision || bs->WillingToPrint (this, false))
					{
						bodyLevel = (breakLevel == mBreakLevels); // next section must be body
						if (origRect.top < r.bottom)
							origRect.top = r.bottom;
						int	thisState = bs->Draw (this, mComposer, r, true);
						state |= thisState;
						if (thisState == RWObject::eDrawState_Vertical && r.Height() == 0)	//mbs 03012010	r.IsEmpty() is false for non-zero width...
						{
							printf ("RWReportWriter::DrawDynamicPage: Some break header does not fit!!!\n");
							throw -11L;	// nothing printed
						}
					}
					else if (origRect.top < r.bottom)
						collisionSpace += r.bottom - origRect.top;
				}
			}
		}
	}

/*	no collision space - logic was changed and only last body is moved to next page
	if (collisionSpace > (origRect.Height() / 2))
	{
		collisionSpace = origRect.Height() - collisionSpace;
	}
	origRect.bottom -= collisionSpace;
*/
	bool	isNewPage = true;
	float	start = origRect.top;

# if	_4D_Package_
	Yield4D();
# endif

	// draw body sections
	bodyPrinted = 0;
	while (mCurrentBody != NULL && (state == RWObject::eDrawState_Done || state == RWObject::eDrawState_Horizontal))
	{
		if (mCurrentBody->GetPageThrow() == RWSection::ePageThrow_Before && not isNewPage)
		{
			state |= RWObject::eDrawState_Vertical;
			break;
		}

		if (mCurrentBody->GetMinSpace() > origRect.Height() && not isNewPage)
		{
			state |= RWObject::eDrawState_Vertical;
			break;
		}

		bool	isOverflow = mIsOverflow && not mFetchRecord;

		if (GetNextSection())
			break;

		if (mCurrentBody->GetKind() == RWSection::eSectionKind_BreakHeader)
		{
			bs = static_cast <RWBreakSection*> (mCurrentBody);
//			breakLevel = bs->GetLevel();
			mBreakHeaders [mBreakLevel] = bs;
			if (bs->IsPrintAlways ()) {
				mCurSubPage = 1;
			}
			for (breakLevel = 0; breakLevel <= mBreakLevels; breakLevel++)	//mbs 05012010	needs to reset mIsBreak
			{
				bs = mBreakHeaders [breakLevel];
				if (bs == NULL)		// no break header for this level
					continue;
				bs->ProcessBreak (mBreakLevel);
			}
		}

		if (isOverflow && mCurHorPage == 1)
		{
			mCurrentBody->Reset (false);
			MoveObjects (mCurrentBody->GetObjects(), origRect, isOverflow);
		}

		SRect	r (origRect);
		if (mCurrentBody->GetBounds (this, mComposer, r, true, isOverflow))
		{
			if (mBreakLevels >= 0 && mCurrentBody->GetKind() == RWSection::eSectionKind_Body)
			{
				if (not isOverflow && (r.Height() > 0)) // pB 2012 increment counters only if we are going to print anything
				{ 
					mCurrentBody->FetchCalcValues (this);
					mCalculator.Increment (this);
				}
				for (breakLevel = 0; breakLevel <= mBreakLevels; breakLevel++)	//mbs 05012010	needs to reset mIsBreak
				{
					bs = mBreakHeaders [breakLevel];
					if (bs == NULL)		// no break header for this level
						continue;
					bs->ProcessBreak (mBreakLevels + 1);
				}
			}
			
			// body level used later to print fill footer
			bodyLevel = ( (mCurrentBody->GetKind() == RWSection::eSectionKind_Body)
						 ||  ( (mCurrentBody->GetKind() == RWSection::eSectionKind_BreakHeader) && (breakLevel == mBreakLevels + 1)) );
			
			/* we want to move last body to next page if we are in collision 
			 but only if there was some body printed on this page already 
			 pB 2012 - we move last break as well ]*/
			if ( /*bodyLevel &&*/ mLastPageCollision && !isOverflow && 
					(mPageIterator == mData.GetBodySections()->end())  
					// && (bodyPrinted != 0)  // pB has side effect - can lead to empty last page
				)
			{
				state |= RWObject::eDrawState_Vertical;	// move to next page
			}
			else
			{
				
				int	thisState = mCurrentBody->Draw (this, mComposer, r, isOverflow);
				state |= thisState;
				if (thisState == RWObject::eDrawState_Vertical && r.Height() == 0 && isOverflow)	//mbs 03012010	r.IsEmpty() is false for non-zero width...
				{
					printf ("RWReportWriter::DrawDynamicPage: Some Body does not fit!!!\n");
					throw -11L;	// nothing printed ???? pB
				}
				if (mCurrentBody->GetKind() == RWSection::eSectionKind_Body)
					bodyPrinted++;
				// move the "frame" for the next section
				if (mCurrentBody->IsFromBottom())
				{
					if (origRect.bottom > r.top)
						origRect.bottom = r.top;
				}
				else
				{
					if (origRect.top < r.bottom)
						origRect.top = r.bottom;
				}
#if	0
				if (start == origRect.bottom)
				{
					printf ("RWReportWriter::DrawDynamicPage: Some Body does not fit horizontally?\n");
					throw -11L;	// nothing printed
				}
# endif
			}
			mIsOverflow = true;
		}
		else if (isNewPage)
		{
			printf ("RWReportWriter::DrawDynamicPage: Body does not fit on a new page!?!\n");
			throw -14L;	// nothing printed
		}
		else
		{
			state |= RWObject::eDrawState_Vertical;	// move to next page (e.g. keep together)
			mIsOverflow = false;					// ==> it is not really an overflow
			if (start == origRect.bottom)
			{
				printf ("RWReportWriter::DrawDynamicPage: Body does not fit horizontally?\n");
				throw -11L;	// nothing printed
			}
			break;
		}

		if (mCurrentBody->IsPrinted())
		{
			mFetchRecord = true;

			if (mCurrentBody->GetKind() == RWSection::eSectionKind_BreakFooter)
			{
				bs = static_cast <RWBreakSection*> (mCurrentBody);
//				breakLevel = bs->GetLevel();
				if (mBreakLevels >= 0)
				{
					mCalculator.ShuntTotals (mBreakLevel);
					for (breakLevel = 0; breakLevel <= mBreakLevels; breakLevel++)	//mbs 05012010	needs to reset mIsBreak
					{
						bs = mBreakHeaders [breakLevel];
						if (bs == NULL)		// no break header for this level
							continue;
						bs->ProcessBreak (mBreakLevel);
					}
				}
			}

			if (mCurrentBody->GetPageThrow() == RWSection::ePageThrow_After)
			{
				if (not PeekNextSection())	// no new [empty] page at the end of a report
					state |= RWObject::eDrawState_Vertical;
				break;
			}
			isNewPage = false;
			if (PeekNextSection())
				break;
		}
	}

# if	_4D_Package_
	Yield4D();
# endif

	//mbs 25122009	fillFooter support
	mCalculator.SetLevel (0); // pB valid for footers only, PeekSection sets it for all other sections
	if (not mCountingPages // && origRect.Height() > 0)  // pB 2011 changed logic of fill footers
		 && bodyLevel )
	{
		// draw fill footers
		for (sit = sections->begin(); sit != sections->end(); sit++)
		{
			sec = *sit;
			if (sec->GetKind() == RWSection::eSectionKind_FillFooter)
			{
				if (sec->WillingToPrint (this, mLastPageCollision))
				{
					origRect.bottom += sec->GetMinSpace();
					sec->Reset (true);
					sec->ExpandFillFooter (origRect);
					sec->PositionObjects (this, mComposer, false);
					if (sec->GetBounds (this, mComposer, origRect, true, false))
					{
						if (not mLastPageCollision || sec->WillingToPrint (this, false))
							sec->Draw (this, mComposer, origRect, false);	// ignore possible problems with vertical expansion...
						break;	// only first fill footer
					}
				}
			}
		}
	}

	// draw footers
	origRect.bottom = pageRect.bottom;
	origRect.left = mPageBounds.left;
	origRect.right = mPageBounds.right;
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = *sit;
		if (sec->GetKind() == RWSection::eSectionKind_Footer)
		{
			if (sec->WillingToPrint (this, mLastPageCollision))
			{
				pageRect = origRect;
				sec->Reset (true);
				sec->PositionObjects (this, mComposer, false);
				if (sec->GetBounds (this, mComposer, pageRect, true, false))
				{
//					if (not pageRect.IsEmpty())
					{
						if (not mLastPageCollision || sec->WillingToPrint (this, false))
						{
							if (sec->IsFromBottom())
							{
								if (origRect.bottom > pageRect.top)
									origRect.bottom = pageRect.top;
							}
							else
							{
								if (origRect.top < pageRect.bottom)
									origRect.top = pageRect.bottom;
							}
							int	thisState = sec->Draw (this, mComposer, pageRect, false);
							state |= thisState;
							if (thisState != RWObject::eDrawState_Done && pageRect.Height() == 0)	//mbs 03012010	r.IsEmpty() is false for non-zero width...
							{
								printf ("RWReportWriter::DrawDynamicPage: Some Footer does not fit!!!\n");
								throw -11L;	// nothing printed
							}
						}
					}
				}
			}
		}
	}

	if (not mCountingPages)
	{
		if (watermark && watermark->IsOnTop())
		{
			watermark->Reset (true);
			if (watermark->WillingToPrint (this, false))
			{
				pageRect = mPageBounds;
				watermark->PositionObjects (this, mComposer, false);
				if (watermark->GetBounds (this, mComposer, pageRect, true, false))
					watermark->Draw (this, mComposer, pageRect, false);
			}
		}

		RW_CheckLicense (&mComposer, &mPageBounds, true);

		mComposer.ClosePage();
	}

# if	_4D_Package_
	Yield4D();
# endif

	return RWObject::EDrawState (state);
}


const	float	cMinFloatValue = -INFINITY;

// ---------------------------------------------------------------------------
// PositionObjects													  [public]
// ---------------------------------------------------------------------------
// Position objects within a section/group

//mbs 04082010	added inDoBinding - don't resize objects in a group when group is resized due to contained objects growing!
void
RWReportWriter::PositionObjects (RWObjList *inObjects, const SRect &inRect, bool inFit, bool inIsOverflow, bool inDoBinding)
{
	RWObjList::const_iterator	it;
	float						thisLineTop = cMinFloatValue;
	float						totalVOffset = 0;				// incremented as we work through the section
	float						maxVOffset = 0;					// largest stretch for all objects with the same top
	float						bottomVal = cMinFloatValue;		// support shrinking & expanding on same line

	float						maxHOffset = 0;
	
	std::sort<RWObjList::iterator, RWObjectComparePosition> (inObjects->begin(), inObjects->end(), RWObjectComparePosition());

	for (it = inObjects->begin(); it != inObjects->end(); it++)
	{
		RWObject	*obj = *it;
		SRect		r (inRect);
		SRect		pos (obj->GetPosition());

		if (not obj->WillingToPrint (inIsOverflow))
			continue;

		if (obj->GetKind() == RWObject::eObject_Group)
		{
			RWGroup	*group = static_cast<RWGroup*> (obj);
			
			PositionGroup(group, r, inFit, inIsOverflow);
			/*if (group->GetGrow())
			{
//				r += group->GetVirtualPosition();	// adjust the frame for children
				SPoint	grTL = group->GetVirtualPosition().TopLeft();
				r.SetRect (	inRect.top + grTL.v,
						   inRect.left + grTL.h,
						   inRect.bottom,
						   inRect.right
						   );
				PositionObjects (group->GetObjects(), r, inFit, inIsOverflow, false);
			}
			else
			{
				if (not obj->GetBounds (mComposer, r, inFit, inIsOverflow, NULL))
				{
					printf ("RWReportWriter::PositionObjects: Objects do not fit into fixed-sized group\n");
					// throw -7L;
				}
				PositionObjects (group->GetObjects(), r, true, inIsOverflow, false);
			
			r = inRect;*/
		}

		if (not obj->GetBounds (mComposer, r, inFit, inIsOverflow, NULL))
		{
			printf ("RWReportWriter::PositionObjects: Object does not fit into section/group\n");
			mLastError = -6; // throw -6L;
		}

		if (pos.top != thisLineTop)
		{
			thisLineTop = pos.top;
			totalVOffset += maxVOffset;
			maxVOffset = 0;
			bottomVal = cMinFloatValue;
			maxHOffset = 0;
		}

		float	vertExpansion = obj->GetVerticalExpansion();
		float	bottomPos = pos.bottom + vertExpansion;
		if (bottomPos > bottomVal)
		{
			bottomVal = bottomPos;
			maxVOffset = vertExpansion;
		}

		if (totalVOffset != 0 && obj->GetMove())
			obj->Move (0, totalVOffset);
		
	}

	//mbs 04082010	last "line" too!
	totalVOffset += maxVOffset;

	//mbs 05092005	resize all bound objects
	if (inDoBinding && totalVOffset != 0)
	{
		for (it = inObjects->begin(); it != inObjects->end(); it++)
		{
			RWObject	*obj = *it;
			if (obj->GetBinding())  // pB ????
				obj->AdjustBounds (0, totalVOffset);
		}
	}

	std::sort<RWObjList::iterator, RWObjectCompareOrder> (inObjects->begin(), inObjects->end(), RWObjectCompareOrder());

	return;
}

// ---------------------------------------------------------------------------
// PositionGroup														 [private]
// ---------------------------------------------------------------------------
// Set new positions of objects within a group

void
RWReportWriter::PositionGroup (RWGroup* inGroup, const SRect &inRect, bool inFit, bool inIsOverflow)
{
	SRect r (inRect);
	if (inGroup->GetGrow())
	{
		//				r += group->GetVirtualPosition();	// adjust the frame for children
		SPoint	grTL = inGroup->GetVirtualPosition().TopLeft();
		r.SetRect (	inRect.top + grTL.v,
				   inRect.left + grTL.h,
				   inRect.bottom,
				   inRect.right
				   );
		PositionObjects (inGroup->GetObjects(), r, inFit, inIsOverflow, false);
	}
	else
	{
		if (not inGroup->GetBounds (mComposer, r, inFit, inIsOverflow, NULL))
		{
			printf ("RWReportWriter::PositionObjects: Objects do not fit into fixed-sized group\n");
			// throw -7L;
		}
		PositionObjects (inGroup->GetObjects(), r, true, inIsOverflow, false);
	}
}

// ---------------------------------------------------------------------------
// MoveObjects														 [private]
// ---------------------------------------------------------------------------
// After drawing a page, move all remaining objects up

void
RWReportWriter::MoveObjects (RWObjList *inObjects, const SRect &inRect, bool inIsOverflow)
{
	RWObjList::const_iterator	it;
	float						thisLineTop = cMinFloatValue;
	float						totalVOffset = 0;				// incremented as we work through the section
	float						maxVOffset = 0;					// largest stretch for all objects with the same top
	float						bottomVal = cMinFloatValue;		// support shrinking & expanding on same line
	bool						firstObject = true;

	std::sort<RWObjList::iterator, RWObjectComparePosition> (inObjects->begin(), inObjects->end(), RWObjectComparePosition());

	for (it = inObjects->begin(); it != inObjects->end(); it++)
	{
		RWObject	*obj = *it;

		if (not obj->WillingToPrint (inIsOverflow))
			continue;

		SRect		r (inRect);
		SRect		pos (obj->GetPosition());

		if (inIsOverflow && (obj->GetKind() == RWObject::eObject_Group)) // group sprinted in overflow need to be recalculated
			PositionGroup(static_cast<RWGroup*> (obj), r, false, inIsOverflow);

				
		if (not obj->GetBounds (mComposer, r, false, inIsOverflow, NULL))
		{
			printf ("RWReportWriter::MoveObjects: Object does not fit into section/group?!?\n");
			mLastError = -6; // throw -6L;
		}

		if (firstObject)
		{
			firstObject = false;
			totalVOffset = inRect.top - r.top;
		}

		if (pos.top != thisLineTop)
		{
			thisLineTop = pos.top;
			totalVOffset += maxVOffset;
			maxVOffset = 0;
			bottomVal = cMinFloatValue;
		}

		float	vertExpansion = obj->GetVerticalExpansion();
		float	bottomPos = pos.bottom + vertExpansion;
		if (bottomPos > bottomVal)
		{
			bottomVal = bottomPos;
			maxVOffset = vertExpansion;
		}

		if (totalVOffset != 0 && obj->GetMove())
			obj->Move (0, totalVOffset);
	}

	totalVOffset += maxVOffset;

	//mbs 05092005	resize all bound objects
	if (totalVOffset != 0)
	{
		for (it = inObjects->begin(); it != inObjects->end(); it++)
		{
			RWObject	*obj = *it;
			if (obj->GetBinding())  // pB ????
				obj->AdjustBounds (0, totalVOffset);
		}
	}
	
	std::sort<RWObjList::iterator, RWObjectCompareOrder> (inObjects->begin(), inObjects->end(), RWObjectCompareOrder());

	return;
}
