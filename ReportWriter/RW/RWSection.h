#ifndef	_RWSection_h_
# define	_RWSection_h_

# include	"RWObject.h"


// forward declarations
class	RWReportWriter;


class	RWSection
{
public:
	enum	ESection_Kind
	{
		eSectionKind_Page	= 0,
		eSectionKind_Header,
		eSectionKind_BreakHeader,
		eSectionKind_Body,
		eSectionKind_BreakFooter,
		eSectionKind_FillFooter,
		eSectionKind_Footer,
		eSectionKind_Watermark
	};
	enum	EPageThrow
	{
		ePageThrow_None = 0,
		ePageThrow_Before,
		ePageThrow_After
	};

									RWSection (RWStringView inKind);
									RWSection (ESection_Kind inKind);
	virtual							~RWSection (void);

	virtual		void				Reset (bool inAll);
    virtual		void				Parse (RWReportData *inReport, RWXmlNode inNode);
	virtual		bool				WillingToPrint (RWReportWriter *inWriter, bool inCollision) const;

	virtual		void				PositionObjects (RWReportWriter *inWriter, RWPageComposer &inComposer, bool inIsOverflow);
	virtual		bool				GetBounds (RWReportWriter *inWriter, RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow);
				RWObjList		*	GetObjects (void);
				ESection_Kind		GetKind (void) const;
//				const RWString			GetName (void) const;
				EPageThrow			GetPageThrow (void) const;
				float				GetMinSpace (void) const;
				bool				IsPrinted (void) const;
				bool				GetKeepTogether (void) const;
				bool				IsFromBottom (void) const;
				void				FetchCalcValues (RWReportWriter *inWriter);
virtual	RWObject::EDrawState		Draw (RWReportWriter *inWriter, RWPageComposer &inComposer, SRect &ioRect, bool inIsOverflow);
				void				ExpandFillFooter (const SRect &inRect);

private:
			// defensive programming - not implemented
									RWSection (const RWSection &inOriginal);
				RWSection		&	operator = (const RWSection &inOriginal);

protected:
	ESection_Kind		mKind;
#if	TARGET_DEBUG
	long				mIteration;
#endif
//	RWString			mName;
	RWObjList			mObjects;
	float				mHeight;
	float				mMinSpace;
	bool				mDraw;
	bool				mKeepTogether;
	bool				mFromBottom;
	EPageThrow			mPageThrow;
	float				mBottomSpace;
	bool				mPrinted;
	bool				mFixedHeight;	
};


class	RWHeaderFooterSection
	:	public	RWSection
{
public:
									RWHeaderFooterSection (RWStringView inKind);
	virtual							~RWHeaderFooterSection (void);

    virtual		void				Parse (RWReportData *inReport, RWXmlNode inNode) override;
	virtual		bool				WillingToPrint (RWReportWriter *inWriter, bool inCollision) const;

	virtual		void				PositionObjects (RWReportWriter *inWriter, RWPageComposer &inComposer, bool inIsOverflow);
	virtual		bool				GetBounds (RWReportWriter *inWriter, RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow);
//	virtual		float				GetMove (void) const;

private:
			// defensive programming - not implemented
									RWHeaderFooterSection (const RWHeaderFooterSection &inOriginal);
			RWHeaderFooterSection&	operator = (const RWHeaderFooterSection &inOriginal);

protected:
	float				mFixed;
	bool				mFirstPage;
	int					mEvenPage;
	int					mOddPage;
	bool				mLastPage;
};


// Break Header, Break Footer
class	RWBreakSection
	:	public	RWSection
{
public:
									RWBreakSection (RWStringView inKind);
	virtual							~RWBreakSection (void);

	virtual		void				Reset (bool inAll);
    virtual		void				Parse (RWReportData *inReport, RWXmlNode inNode) override;
	virtual		bool				WillingToPrint (RWReportWriter *inWriter, bool inCollision) const;
	virtual		bool				IsPrintAlways () const;

				int					GetLevel (void) const;
//				const RWString			GetBreakOn (void) const;
				void				ProcessBreak (long inBreakLevel);

private:
			// defensive programming - not implemented
									RWBreakSection (const RWBreakSection &inOriginal);
				RWBreakSection	&	operator = (const RWBreakSection &inOriginal);

protected:
	int					mLevel;
	bool				mPrintAlways;
//	RWString			mBreakOn;
	bool				mIsBreak;
};


// Page
class	RWPageSection
	:	public	RWSection
{
public:
									RWPageSection (void);
	virtual							~RWPageSection (void);

    virtual		void				Parse (RWReportData *inReport, RWXmlNode inNode) override;
				const RWString			GetPageOrientation (void) const;
				const RWString			GetPageSize (void) const;

private:
			// defensive programming - not implemented
									RWPageSection (const RWPageSection &inOriginal);
				RWPageSection	&	operator = (const RWPageSection &inOriginal);

protected:
	RWString				mPageOrientation;
	RWString				mPageSize;
};


// Watermark
class	RWWatermarkSection
	:	public	RWHeaderFooterSection
{
public:
									RWWatermarkSection (RWStringView inKind);
	virtual							~RWWatermarkSection (void);

    virtual		void				Parse (RWReportData *inReport, RWXmlNode inNode) override;
				bool				IsOnTop (void) const;

private:
			// defensive programming - not implemented
									RWWatermarkSection (const RWWatermarkSection &inOriginal);
			RWWatermarkSection	&	operator = (const RWWatermarkSection &inOriginal);

protected:
	bool				mOnTop;
};


typedef	RWList<RWBreakSection*>	RWBreakSectionList;	// no destructor for objects deletion

// container for RWSection pointers, destructor deletes the RWSections first
typedef	RWArray<RWSection*>		RWSectionList;

#endif
