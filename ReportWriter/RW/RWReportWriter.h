#ifndef	_RWReportWriter_h_
# define	_RWReportWriter_h_

# include	"RWReportData.h"
# include	"RWCalculator.h"
# include	"RWPageComposer.h"

// forward declarations
class	RWDataSource;


class	RWReportWriter	:	public	RWCalcDataProvider
{
public:
								RWReportWriter (RWDataSource &inDataSource, RWReportData &inData, RWPageComposer &inComposer);
	virtual						~RWReportWriter (void);

				void		*		Report (size_t *outSize);
				void				PositionObjects (RWObjList *inObjects, const SRect &inRect, bool inFit, bool inIsOverflow, bool inDoBinding = true);

inline			RWDataSource&	GetDataSource (void) const;
inline	const	RWReportData&	GetReportData (void) const;
inline	const	RWPageComposer&	GetPageComposer (void) const;

inline			void				GetPageBounds (SRect &outRect) const;
inline			int				GetCurrentPage (void) const;
inline			int				GetNumberOfPages (void) const;
inline			int				GetCurrentSubPage (void) const;
inline			int				GetNumberOfSubPages (void) const;
inline			int				GetCurrentHorPage (void) const;
inline			int				GetNumberOfHorPages (void) const;

inline			int				GetCurrentFrame (void) const;
inline			int				GetNumberOfFrames (void) const;
inline			bool				IsLastPage (bool inCollision) const;
inline			int				GetLastError(void) const;
	
	virtual		bool			GetCalculatedValue (const RWString inName, RWValue &outVar) const;

				int				GetReportVariable (const RWString inName) const;
				void			CreateVariable (const RWString inName, ECalcType inCalc = ECalcType_None);
				bool			GetVariable (const RWString inName, RWValue &outVar, ECalcType inCalc = ECalcType_None) const;
				void			SetVariable (const RWString inName, RWValue *inVar);
//				const RWString		GetVariable (long inDataID) const;
				RWString		FormatVariable (RWValue &inVar, const RWString inFormat) const;
				void			PositionGroup (RWGroup* inGroup, const SRect &inRect, bool inFit, bool inIsOverflow);

private:
				void			MoveObjects (RWObjList *inObjects, const SRect &inRect, bool inIsOverflow);
				void			DrawReport (void);
#if	MACVER
	static		void			DrawReportCB (void *inData);
#endif
				void			DrawStaticReport (void);
		RWObject::EDrawState	DrawStaticPage (RWPageSection *inBody);
				void			PrepareDynamicReport (void);
				bool			FindNextSection (void);
				bool			PeekNextSection (void);
				bool			GetNextSection (void);
				void			DrawDynamicReport (void);
		RWObject::EDrawState	DrawDynamicPage (void);

			// defensive programming - not implemented
								RWReportWriter (void);
								RWReportWriter (const RWReportWriter &inOriginal);
				RWReportWriter&	operator = (const RWReportWriter &inOriginal);

private:
	RWDataSource 		&mSource;
	RWReportData 		&mData;
	RWPageComposer		&mComposer;
	RWString			mVarNames [RW_VarNamesRWCount];
	SRect				mPageBounds;
	time_t				mPrintTime;
	bool				mCountingPages;
	unsigned long		mCurPage;
	unsigned long		mNumPages;
	unsigned long		mCurSubPage;
	unsigned long		mCurHorPage;
	unsigned long		mMaxHorPage;
	unsigned long		mCurrentPage;
	unsigned long		mTotalPages;
	bool				mLastPageCollision;
	unsigned long	*	mNumHorPages;
	unsigned long	*	mNumSubPages;
	// unsigned long		mBreakPage;

	// for dynamic (iterated) reports
	RWPageSection	*				mPageSection;
	RWSectionList::const_iterator	mPageIterator;
	RWSection		*               mCurrentBody;
	bool							mFetchRecord;
//	RWBreakSectionList				mBreakHeaders;
	RWBreakSection	**              mBreakHeaders;
//	RWBreakSectionList				mBreakFooters;
	RWCalculator					mCalculator;
	long							mBreakLevels;
	long							mBreakLevel;
	bool							mIsOverflow;
	int                             mLastError;
};


inline			RWDataSource&	RWReportWriter::GetDataSource (void) const		{ return mSource; }
inline	const	RWReportData&	RWReportWriter::GetReportData (void) const		{ return mData; }
inline	const	RWPageComposer&	RWReportWriter::GetPageComposer (void) const	{ return mComposer; }

inline			void				RWReportWriter::GetPageBounds (SRect &outRect) const	{ outRect = mPageBounds; }
inline			int				RWReportWriter::GetCurrentPage (void) const		{ return mCurPage; }
inline			int				RWReportWriter::GetNumberOfPages (void) const	{ return mNumPages; }
inline			int				RWReportWriter::GetCurrentSubPage (void) const	{ return mCurSubPage; }
inline			int				RWReportWriter::GetCurrentHorPage (void) const	{ return mCurHorPage; }
inline			int				RWReportWriter::GetNumberOfSubPages (void) const
{
	if (mNumSubPages)
		return mNumSubPages [mCurPage];
	return 1;
}
inline			int				RWReportWriter::GetNumberOfHorPages (void) const
{
	if (mNumHorPages)
		return mNumHorPages [mCurPage];
	return mMaxHorPage;
}
inline			int				RWReportWriter::GetCurrentFrame (void) const	{ return mCurrentPage; }
inline			int				RWReportWriter::GetNumberOfFrames (void) const	{ return mTotalPages; }
inline			bool				RWReportWriter::IsLastPage (bool inCollision) const
{
	return mCurPage == mNumPages || (mLastPageCollision && inCollision && mCurPage == mNumPages - 1);
}
inline			int				RWReportWriter::GetLastError (void) const		{ return mLastError; }

#endif
