/*
 *  DMReport.h
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 17.10.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o. All rights reserved.
 *
 */

// DM - Design Mode version of SRObjects

#ifndef	_DMReport_h_
#define	_DMReport_h_

# include	"DMObject.h"
# include	"SRDataSource.h"
# include	"ExtendedExecute.h"

class	PSStyleListD
	:	public	RWArray<PSObject*>,
		public	RWStyleContainer
{
public:
					RWStyle*	FindStyle (long inID) const;
					long		GetNewID () const;
};

// DM Style
class	DMStyle
:	public	DMBase,
public	RWStyle
{
public:
	//	virtual						~DMStyle (void);
	static		DMStyle	*		Create (RWStyleContainer *inContainer, DMBase *inParent, XMLElement *inNode);
	DMStyle	*		Clone (RWStyleContainer *inContainer, DMBase *inParent, long inNewID);
	
	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
	virtual		XMLElement*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);
	
protected:
	DMStyle (RWStyleContainer *inContainer, DMBase *inParent);
	DMStyle (RWStyleContainer *inContainer, DMBase *inParent, long inBaseID);
	//	virtual		DMBase		*	Clone (DMBase *inParent);
	
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);
	
private:
	// defensive programming - not implemented
	DMStyle (const DMStyle &inOriginal);
	DMStyle	&		operator = (const DMStyle &inOriginal);
	
protected:
	static const UserProps	sUserProperties[];
};

// DM Section
class	DMSection
	:	public	DMBase
{
public:
	enum	ESection_Kind
	{
		eSectionKind_Scrap	= 0,
		eSectionKind_Page,
		eSectionKind_Header,
		eSectionKind_BreakHeader,
		eSectionKind_Body,
		eSectionKind_BreakFooter,
//		eSectionKind_FillFooter,	property of a footer
		eSectionKind_Footer,
		eSectionKind_Watermark
	};

	enum	EPageThrow
	{
		ePageThrow_None = 0,
		ePageThrow_Before,
		ePageThrow_After
	};

	virtual						~DMSection (void);

	virtual		int				CompareOrder (const DMBase *other) const;
	inline		ESection_Kind	GetSectionKind (void) const;

	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		PSObjList	*	GetObjects (OSType id);
	virtual		XMLElement*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			AdjustDrawingPosition (RWPageComposer *inComposer, const SPoint inParent);
	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		void			ParseData (RWPageComposer *inComposer);
	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);
	virtual		void			HandleTrackSelect (SRect &inWhere, UInt32 inFlags);

protected:
								DMSection (DMBase *inParent, ESection_Kind inKind, const CXMLText inType);
								DMSection (DMBase *inParent, const DMSection &inOriginal);
//	virtual		DMBase		*	Clone (DMBase *inParent);

	virtual		bool			GetObjects (OSType id, PSObjListD* &outList);
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);
	virtual		void			LoadXMLObjects (const PSObjProps* pes, XMLElement *inNode);
//	virtual		bool			WriteXMLObjects (const PSObjProps* pes, XMLElement *inNode);

private:
	// defensive programming - not implemented
								DMSection (const DMSection &inOriginal);
				DMSection	&	operator = (const DMSection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	ESection_Kind			mSectionKind;
	CXMLText				mType;
	RWTextValue				mName;
	PSObjListD				mObjects;
	float					mHeight;
	float					mMinSpace;
	bool					mDraw;
	bool					mKeepTogether;
	bool					mFromBottom;
	bool					mFixedHeight;
//	union
//	{
//		EPageThrow			mPageThrow;
		int					mPageThrowI;
//	};
	ExtendedExecute			mScript;
	SRect					mLabelRect;
};

inline		DMSection::ESection_Kind	DMSection::GetSectionKind (void) const	{ return mSectionKind; }


class	DMHeaderFooterSection
	:	public	DMSection
{
public:
//	virtual							~DMHeaderFooterSection (void);
	static	DMHeaderFooterSection*	Create (DMBase *inParent, XMLElement *inNode, ESection_Kind inKind, const CXMLText inType);

	virtual		long				GetUserProperties (const UserProps* &outProps) const;
	virtual		const PSObjProps *	GetProperties (void) const;
	virtual		bool				GetProperty (OSType id, RWValue &outValue);
	virtual		bool				SetProperty (OSType id, RWValue &inValue);
	bool							GetFill (void) const;
protected:
									DMHeaderFooterSection (DMBase *inParent, ESection_Kind inKind, const CXMLText inType);
									DMHeaderFooterSection (DMBase *inParent, const DMSection &inOriginal);

	virtual		void				LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
			// defensive programming - not implemented
									DMHeaderFooterSection (const DMHeaderFooterSection &inOriginal);
			DMHeaderFooterSection&	operator = (const DMHeaderFooterSection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	float					mFixed;
	bool					mFirstPage;
	int						mEvenPage;
	int						mOddPage;
	bool					mLastPage;
	bool					mFillPage;
};

inline		bool					DMHeaderFooterSection::GetFill (void) const	{ return mFillPage; }

// Break Header, Break Footer
class	DMBreakSection
	:	public	DMSection
{
public:
	enum	EBreakOn
	{
		eBreakOn_None = 0,
		eBreakOn_Field,
		eBreakOn_Variable,
		eBreakOn_Array
	};
//	virtual							~DMBreakSection (void);
	static		DMBreakSection	*	Create (DMBase *inParent, XMLElement *inNode, ESection_Kind inKind, const CXMLText inType);

	virtual		long				GetUserProperties (const UserProps* &outProps) const;
	virtual		const PSObjProps *	GetProperties (void) const;
	virtual		bool				GetProperty (OSType id, RWValue &outValue);
	virtual		bool				SetProperty (OSType id, RWValue &inValue);

	inline		int					GetBreakLevel (void) const;
	inline		void				SetBreakLevel (int inLevel);

protected:
									DMBreakSection (DMBase *inParent, ESection_Kind inKind, const CXMLText inType);
									DMBreakSection (DMBase *inParent, const DMBreakSection &inOriginal);

	virtual		void				LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
			// defensive programming - not implemented
									DMBreakSection (const DMBreakSection &inOriginal);
				DMBreakSection	&	operator = (const DMBreakSection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	int						mLevel;
	bool					mPrintAlways;
	RWTextValue				mBreakOn;
//	union
//	{
//		EBreakOn			mBreakType;
		int					mBreakTypeI;
//	};
	RWTextValue				mAlias;
};

inline		int					DMBreakSection::GetBreakLevel (void) const	{ return mLevel; }
inline		void				DMBreakSection::SetBreakLevel (int inLevel)	{ mLevel = inLevel; }


// Scrap
class	DMScrapSection
	:	public	DMSection
{
public:
//	virtual							~DMScrapSection (void);
	static		DMScrapSection	*	Create (DMBase *inParent, XMLElement *inNode, const CXMLText inType);

	virtual		long				GetUserProperties (const UserProps* &outProps) const;
	virtual		const PSObjProps *	GetProperties (void) const;

protected:
									DMScrapSection (DMBase *inParent, const CXMLText inType);
									DMScrapSection (DMBase *inParent, const DMScrapSection &inOriginal);

private:
			// defensive programming - not implemented
									DMScrapSection (const DMScrapSection &inOriginal);
				DMScrapSection	&	operator = (const DMScrapSection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
};


// Page
class	DMPageSection
	:	public	DMSection
{
public:
//	virtual							~DMPageSection (void);
	static		DMPageSection	*	Create (DMBase *inParent, XMLElement *inNode, const CXMLText inType);

	virtual		long				GetUserProperties (const UserProps* &outProps) const;
	virtual		const PSObjProps *	GetProperties (void) const;
	virtual		bool				GetProperty (OSType id, RWValue &outValue);
	virtual		bool				SetProperty (OSType id, RWValue &inValue);

protected:
									DMPageSection (DMBase *inParent, const CXMLText inType);
									DMPageSection (DMBase *inParent, const DMPageSection &inOriginal);

private:
			// defensive programming - not implemented
									DMPageSection (const DMPageSection &inOriginal);
				DMPageSection	&	operator = (const DMPageSection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
//	RWTextValue				mPageOrientation;
//	RWTextValue				mPageSize;
};


// Body
class	DMBodySection
	:	public	DMSection
{
public:
//	virtual							~DMBodySection (void);
	static		DMBodySection	*	Create (DMBase *inParent, XMLElement *inNode, const CXMLText inType);

	virtual		long				GetUserProperties (const UserProps* &outProps) const;
	virtual		const PSObjProps *	GetProperties (void) const;
//	virtual		bool				GetProperty (OSType id, RWValue &outValue);
//	virtual		bool				SetProperty (OSType id, RWValue &inValue);

protected:
									DMBodySection (DMBase *inParent, const CXMLText inType);
									DMBodySection (DMBase *inParent, const DMBodySection &inOriginal);

private:
			// defensive programming - not implemented
									DMBodySection (const DMBodySection &inOriginal);
				DMBodySection	&	operator = (const DMBodySection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
};


// Watermark
class	DMWatermarkSection
	:	public	DMHeaderFooterSection
{
public:
//	virtual							~DMBodySection (void);
	static		DMWatermarkSection*	Create (DMBase *inParent, XMLElement *inNode, const CXMLText inType);

	virtual		long				GetUserProperties (const UserProps* &outProps) const;
	virtual		const PSObjProps *	GetProperties (void) const;
	virtual		bool				GetProperty (OSType id, RWValue &outValue);
	virtual		bool				SetProperty (OSType id, RWValue &inValue);
				bool				IsOnTop (void) const;
	DMBase::EHitTest	HitTest (SPoint &inWhere, DMBase* &outObjectHit);

protected:
									DMWatermarkSection (DMBase *inParent, const CXMLText inType);
									DMWatermarkSection (DMBase *inParent, const DMBodySection &inOriginal);

	virtual		void				LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
			// defensive programming - not implemented
									DMWatermarkSection (const DMBodySection &inOriginal);
				DMWatermarkSection&	operator = (const DMBodySection &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	bool				mOnTop;
};



// DM DataSource
class	DMDataSource
	:	public	DMBase
{
public:
	virtual						~DMDataSource (void);

	virtual	const PSObjProps *	GetProperties (void) const = 0;
//	virtual		bool			GetProperty (OSType id, RWValue &outValue);
//	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		XMLElement*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

				SRDataSource*	GetRealDataSource (void);
				void			ClearDataSource (void);

protected:
								DMDataSource (DMBase *inParent);
								DMDataSource (DMBase *inParent, const DMDataSource &inOriginal);
//	virtual		DMBase		*	Clone (DMBase *inParent);

//	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
	// defensive programming - not implemented
								DMDataSource (const DMDataSource &inOriginal);
				DMDataSource &	operator = (const DMDataSource &inOriginal);

protected:
	SRDataSource	*	mRealDataSource;
};


// DM 4DDataSource
class	DM4DDataSource
	:	public	DMDataSource
{
public:
	enum	ERelate
	{
		eRelate_None,
		eRelate_Automatic,
		eRelate_Manual
	};
	enum	EDataSource
	{
		eDataSource_Undefined,
		eDataSource_Table,
		eDataSource_Fixed,
		eDataSource_Variable,
		eDataSource_Array
	};

	virtual						~DM4DDataSource (void);
	static	DM4DDataSource	*	Create (DMBase *inParent, XMLElement *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		XMLElement*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

protected:
								DM4DDataSource (DMBase *inParent);
								DM4DDataSource (DMBase *inParent, const DM4DDataSource &inOriginal);
//	virtual		DMBase		*	Clone (DMBase *inParent);

	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
	// defensive programming - not implemented
								DM4DDataSource (const DM4DDataSource &inOriginal);
				DM4DDataSource&	operator = (const DM4DDataSource &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	ExtendedExecute		mStartScript;
	ExtendedExecute		mBodyScript;
	ExtendedExecute		mEndScript;
//	union
//	{
//		EDataSource		mSource;	// table, array size, variable, fixed
		int				mSourceI;
//	};
	RWTextValue			mName;		// [4], arrayName, variableName
	long				mNumIterations;
	int					mMainTable;
//	union
//	{
//		ERelate			mRelateOne;
		int				mRelateOneI;
//	};
//	union
//	{
//		ERelate			mRelateMany;
		int				mRelateManyI;
//	};
	RWTextValue			mCallBackName;
	bool				mSRPCompatibility;
};


// DM Guide
class	DMGuide
	:	public	DMBase
{
public:
	virtual						~DMGuide (void);
	static		DMGuide		*	Create (DMBase *inParent, XMLElement *inNode, bool inVertical);

				bool			IsVertical (void) const;
	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
	virtual		XMLElement  *	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode, double inWidth, SRGBColor inColor);
	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);

protected:
								DMGuide (DMBase *inParent, bool inVertical);
								DMGuide (DMBase *inParent, const DMGuide &inOriginal);
//	virtual		DMBase		*	Clone (DMBase *inParent);

private:
	// defensive programming - not implemented
								DMGuide (const DMGuide &inOriginal);
				DMGuide	&		operator = (const DMGuide &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	bool				mVertical;
	float				mPos;
};

inline		bool				DMGuide::IsVertical (void) const		{ return mVertical; }


// DM Report
class	DMReport
	:	public	DMBase
{
friend class	DMBase;

public:
								DMReport (XMLDocument *inXML);
	virtual						~DMReport (void);

	virtual		void			SetReport (XMLDocument *inXML);
				void			GetReport (XMLDocument &outXML);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
	virtual		PSObjList	*	GetObjects (OSType id);
	virtual		XMLElement  *	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

				void			ParseObjects (const PSObjProps* pes, XMLElement *inNode, DMBase *inParent, PSObjListD &objList);

				DMBase		*	GetObject (long inObject) const;
				DMBase		*	GetObjectByID (CText &inName) const;
	static		DMReport	*	GetReportObject (long inObject);
	static		DMReport	*	GetReportOfObject (long inObject);
	static		DMBase		*	FindObject (long inObject);
	static		DMBase		*	FindObjectByID (CText &inName);
				DMBase		*	CreateObject (OSType inKind, long inParent, XMLElement *inNode);
				DMBase		*	CreateObject (OSType inKind, DMBase *inParent, XMLElement *inNode);
				DMBase		*	CreateObject (XMLElement *inNode, long inParent);
				bool			RemoveObject (DMBase *inObject);
				bool			ChangeObjectParent (DMBase *inObject, long inParent);
				void			AdjustSections (DMBreakSection *inSection, int inLevel);
	//			DMStyle		*	CloneStyle (DMBase *inObject, DMStyle *inStyle);
	inline		PSStyleListD *	GetStyleContainer (void);
	
	virtual		void			AdjustDrawingPosition (RWPageComposer *inComposer, const SPoint inParent);
	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		void			ParseData (RWPageComposer *inComposer);
	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);
	virtual		void			HandleTrackSelect (SRect &inWhere, UInt32 inFlags);
				DMBase		*	GetParentAt (SPoint &inWhere);
				void			DrawFrame (const DMBase *inObject, EDrawDM inMode, long inStyleID, const CText inText, const SRect &inRect, bool inAttributed, RWStyle* inStyle = NULL);
				void			DrawSelection (const DMBase *inObject, const SRect &inRect);

				void			SetPageComposer (RWPageComposer *inComposer);
	inline		RWPageComposer*	GetPageComposer (void) const;
				void			SetPageMetrics (float outPageWidth, float outPageHeight, const SRect outMargins);
				DMStyle		*	GetStyle (long inStyleID) const;
	virtual		void			Modified (void);
				void			DeselectAll (void);
	inline		bool			IsSimple (void) const;
				SRDataSource*	GetDataSource (void);
                bool            IsValidObject(DMBase * inObject);
	static		PSObject*		GetMappedObject (long inObject);

protected:
//	virtual		DMBase		*	Clone (DMBase *inParent);

	virtual		bool			GetObjects (OSType id, PSObjListD* &outList);
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);
	virtual		void			LoadXMLObjects (const PSObjProps* pes, XMLElement *inNode);
	virtual		bool			WriteXMLObjects (const PSObjProps* pes, XMLElement *inNode);

				void			AddObject (DMBase *inObject);
				void			DeleteObject (DMBase *inObject);
				void			AddSelectedObject (DMBase *inObject);
				void			RemoveSelectedObject (DMBase *inObject);
	virtual		void			ScaleChanged (void);
	virtual		void			CalculatePosition (void);
    inline      double          GetScale (void) const;

private:
	// defensive programming - not implemented
								DMReport (const DMReport &inOriginal);
				DMReport	&	operator = (const DMReport &inOriginal);

protected:
	static		PSObjListD	sReports;
	static const PSObjProps	sProperties[];
	static const PSObjProps	sPropertiesEditor[];
	static const PSObjProps	sPropertiesGuides[];
	static const PSObjProps	sPropertiesStyles[];
	static const UserProps	sUserProperties[];

	long					mSeqIDs [eObject_Kind_Count];
	RWTextValue				mName;
	bool					mSimple;
	float					mPageWidth;
	float					mPageHeight;
	bool					mUsePhysical;
	SRect					mPageMargins;
//	RWValue					mPageSetup;
	RWValue					mPageFormat;
	RWValue					mPrintSettings;
	RWValue					mDevMode;
	RWValue					mDeviceNames;
	RWValue					mPageSetupDialog;
	RWValue					mPrintDialog;
// v1.4
//	bool					mLabelReport;
//	int						mLabelH;
//	int						mLabelV;
//    
//	float					mLabelMarginTop;
//	float					mLabelMarginLeft;
//	float					mLabelMarginBottom;
//	float					mLabelMarginRight;
	
    bool                    mReportRotation;
    bool                    mReportMirror;
    
	DMDataSource		*	mDataSource;
	PSObjListD				mGuides;
	PSStyleListD			mStyles;
	PSObjListD				mSections;
	PSObjList				mObjects;			// list of all objects owned by some child
	PSObjList				mSelectedObjects;	// list of all selected objects
	int						mMaxBreakHeaderLevel;
	int						mMaxBreakFooterLevel;

	// Editor properties
	bool					mShowMargins;
	bool					mShowRuler;
	int						mRulerUnits;
	float					mGridSize;
	bool					mShowGrid;
	bool					mSnapToGrid;
	bool					mShowGuides;
	bool					mLockGuides;
	bool					mSnapToGuide;
//	bool					mShowSections;
	bool					mLockSections;
	bool					mShowObjBorders;
	double					mScale;
	double					mGridRadius;
	SRGBColor				mGridColor;
	SRGBColor				mGuideColor;
	double					mGuideWidth;
	
	PSStyleListD			mEditorStyles;

	bool					mDirty;
	RWPageComposer		*	mComposer;
};



inline	const PSObject::PSObjProps *	DMStyle::GetProperties (void) const								{ return RWStyle::sProperties; }
//inline		bool						DMStyle::GetProperty (OSType id, RWValue &outValue)				{ return RWStyle::GetProperty (id, outValue); }
//inline		bool						DMStyle::SetProperty (OSType id, RWValue &inValue)				{ return RWStyle::SetProperty (id, inValue); }
inline		XMLElement  *				DMStyle::WriteXML (XMLNode *inParent, const PSObjProps* pes)	{ return RWStyle::WriteXML (inParent, pes); }
inline		void						DMStyle::LoadXML (XMLElement *inNode, const PSObjProps* pes)	{ return RWStyle::LoadXML (inNode, pes); }

inline	const PSObject::PSObjProps *	DMSection::GetProperties (void) const				{ return sProperties; }
inline	const PSObject::PSObjProps *	DMHeaderFooterSection::GetProperties (void) const	{ return sProperties; }
inline	const PSObject::PSObjProps *	DMBreakSection::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMScrapSection::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMPageSection::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMBodySection::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMWatermarkSection::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	DM4DDataSource::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMGuide::GetProperties (void) const					{ return sProperties; }
inline	const PSObject::PSObjProps *	DMReport::GetProperties (void) const				{ return sProperties; }

inline		bool						DMWatermarkSection::IsOnTop (void) const			{ return mOnTop; }

inline		RWPageComposer*				DMReport::GetPageComposer (void) const					{ return mComposer; }
inline		void						DMReport::SetPageComposer (RWPageComposer *inComposer)	{ mComposer = inComposer; }
inline		void						DMReport::Modified (void)								{ mDirty = true; }
inline		void						DMReport::ScaleChanged (void)							{}
inline		bool						DMReport::IsSimple (void) const							{ return mSimple; }
inline		PSStyleListD *				DMReport::GetStyleContainer (void)					{return &mStyles;}
inline      double                      DMReport::GetScale (void) const                     {return mScale;}
#endif
