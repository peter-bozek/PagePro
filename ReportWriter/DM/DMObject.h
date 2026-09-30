/*
 *  DMObject.h
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 29.09.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o. All rights reserved.
 *
 */

// DM - Design Mode version of SRObjects

#ifndef	_DMObject_h_
#define	_DMObject_h_

# include	"PSObject.h"
# include	<functional>
# include	"RWPageComposer.h"
# include	"RWStyle.h"
# include	"ExtendedExecute.h"

class	DMReport;


// DM Base
class	DMBase
	:	public	PSObject
{
friend class	DMReport;
friend class	DMArea;

public:
			struct	UserProps
			{
				OSType	id;
				bool	writable;
			};

			virtual				~DMBase (void);

	enum	EDrawDM			{ eDraw_Normal = 0, eDraw_Alias, eDraw_Format, eDraw_Name, eDraw_ID, eDraw_Order, eDraw_Size, eDraw_Resource, eDraw_Last };
	enum	EHitTest		{ eHit_None = 0, eHit_Object, eHit_TLH, eHit_TCH, eHit_TRH, eHit_RCH, eHit_BRH, eHit_BCH, eHit_BLH, eHit_LCH, eHit_ResizeH, eHit_ResizeV };
	enum	EOrder			{ eOrder_Reset = 0, eOrder_Set, eOrder_Up, eOrder_Down, eOrder_Deleted, eOrder_Sequentially };
	enum	ELockState		{ eLockState_Unlocked = 0, eLockState_Locked, eLockState_Fully };
    enum    EOutsetSize     { eOut_Handle = 0, eOut_Proximity};
    
//	static		DMBase		*	CreateFrom (DMBase *inParent, const DMBase &inOriginal);								// for cross-report Duplicate fuctionality
//	static		DMBase		*	CreateFrom (DMBase *inParent, Ti *inNode);									// from XML
//	static		DMBase		*	Create (DMBase *inParent, EObject_Kind inKind, const SRect &inWhere);					// from editor

	inline		DMBase		*	GetParent (void) const;
				DMReport	*	GetReport (void) const;

	inline		long			GetOrder (void) const;										// print order/position in list (header/column)
	inline		void			SetOrder (long inSeqID);
	virtual		int				CompareOrder (const DMBase *other) const;

	inline		bool			GetSelected (void) const;
				void			SetSelected (bool inSelect);

	inline		SRect			GetPosition (void) const;									// position relative to parent
	inline		void			SetPosition (SRect &inRect);
	inline		SRect			GetDrawPosition (void) const;								// position in editor
//				int				ComparePosition (const DMObject *other) const;
	inline		bool			IsVisible (void) const;
	inline		void			SetVisible (bool inVisible);
	inline		bool			IsLocked (void) const;
	inline		bool			IsSelectable (void) const;
	inline		void			SetLocked (DMBase::ELockState inLocked);

	virtual		long			GetUserProperties (const UserProps* &outProps) const = 0;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
	virtual		PSObjList	*	GetObjects (OSType id);
//	virtual		Ti*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			AdjustDrawingPosition (RWPageComposer *inComposer, const SPoint inParent);
	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		void			ParseData (RWPageComposer *inComposer);
	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);
	virtual		void			HandleTrackSelect (SRect &inWhere, UInt32 inFlags);

protected:
								DMBase (DMBase *inParent, EObject_Kind inKind);
				void			Init (void);
//	virtual		DMBase		*	Clone (DMBase *inParent);

	inline		void			SetParent (DMBase *inParent);
				void			AdjustPosition (OSType id, SRect &rect, float fVal);
	virtual		void			AdjustOrder (EOrder inOrder, long inSeq);

	virtual		bool			GetObjects (OSType id, PSObjListD* &outList);
    virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);
    virtual     double          GetProxySize(EOutsetSize flag);


private:
	// defensive programming - not implemented
								DMBase (const DMBase &inOriginal);
					DMBase	&	operator = (const DMBase &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	DMBase				*	mParent;
	RWTextValue				mID;
	long					mSeqID;		// for editor
	SRect					mPosition;
	bool					mVisible;	// for editor
	int						mLocked;	// for editor	0 = unlocked, 1 = locked, 2 = locked & not selectable
	bool					mSelected;	// for editor
	SRect					mDrawRect;	// for editor
};


// DM Object
class	DMObject
	:	public	DMBase
{
public:
	virtual						~DMObject (void);

	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		Ti*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);

protected:
								DMObject (DMBase *inParent, EObject_Kind inKind);
								DMObject (DMBase *inParent, const DMObject &inOriginal);
//	virtual		DMBase		*	Clone (DMBase *inParent);
				void			Init (void);

	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
	// defensive programming - not implemented
								DMObject (const DMObject &inOriginal);
				DMObject	&	operator = (const DMObject &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	RWTextValue				mName;
//	bool					mFixedH;		// fixed position ==> no Move
	bool					mFixedV;
//	bool					mBindH;			// stretch as needed ==> Grow
	bool					mBindV;
//	union
//	{
		EAlignment			mAlignment;
		// int					mAlignmentI;		// pB 2012
//	};
//	union
//	{
		EDraw				mDraw;
//		int					mDrawI;
//	};
//	bool					mExpandH;		// variable width ==> shrink/expand as needed
	bool					mExpandV;
};


// DM Group object
class	DMGroup
	:	public	DMObject
{
public:
    static		DMGroup		*	Create (DMBase *inParent, XMLElement *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		PSObjList	*	GetObjects (OSType id);
//	virtual		Ti*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			AdjustDrawingPosition (RWPageComposer *inComposer, const SPoint inParent);
	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);
	virtual		void			HandleTrackSelect (SRect &inWhere, UInt32 inFlags);


protected:
								DMGroup (DMBase *inParent);
								DMGroup (DMBase *inParent, const DMGroup &inOriginal);
	virtual						~DMGroup (void);
				void			Init (void);
	virtual		bool			GetObjects (OSType id, PSObjListD* &outList);
    virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);
    virtual		void			LoadXMLObjects (const PSObjProps* pes, XMLElement *inNode);

private:
	// defensive programming - not implemented
								DMGroup (const DMGroup &inOriginal);
				DMGroup		&	operator = (const DMGroup &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	PSObjListD				mObjects;
};


// DM Line object
class	DMLine
	:	public	DMObject
{
public:
    static		DMLine		*	Create (DMBase *inParent, XMLElement *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		Ti*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);
				UInt8			GetFlags (void) const;

protected:
								DMLine (DMBase *inParent);
								DMLine (DMBase *inParent, const DMLine &inOriginal);
	virtual						~DMLine (void);
				void			Init (void);
    virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
	// defensive programming - not implemented
								DMLine (const DMLine &inOriginal);
				DMLine		&	operator = (const DMLine &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	float					mThickness;
	SRGBColor				mLineColor;
	UInt8					mFlags;	// RWLine_Flags
};


// DM Oval object
class	DMOval
	:	public	DMLine
{
public:
    static		DMOval		*	Create (DMBase *inParent, XMLElement *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		Ti*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
//	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);

protected:
								DMOval (DMBase *inParent);
								DMOval (DMBase *inParent, const DMOval &inOriginal);
	virtual						~DMOval (void);
				void			Init (void);
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
	// defensive programming - not implemented
								DMOval (const DMOval &inOriginal);
				DMOval		&	operator = (const DMOval &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	bool					mFill;
	SRGBColor				mFillColor;
};


// DM Rect object
class	DMRect
	:	public	DMOval
{
public:
	static		DMRect		*	Create (DMBase *inParent, XMLElement *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		Ti*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
//	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);

protected:
								DMRect (DMBase *inParent);
								DMRect (DMBase *inParent, const DMRect &inOriginal);
	virtual						~DMRect (void);
				void			Init (void);
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
	// defensive programming - not implemented
								DMRect (const DMRect &inOriginal);
				DMRect		&	operator = (const DMRect &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	long					mRows;
	long					mCols;
//	UInt8					mFlags;	// RWRect_Flags
};


// DM Pict object
class	DMPict
	:	public	DMOval
{
public:
	static		DMPict		*	Create (DMBase *inParent, XMLElement *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		Ti*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
//	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);
    inline      float           GetRotation (void) const;

protected:
								DMPict (DMBase *inParent);
								DMPict (DMBase *inParent, const DMPict &inOriginal);
	virtual						~DMPict (void);
				void			Init (void);
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);
	virtual		void			LoadXMLObjects (const PSObjProps* pes, XMLElement *inNode);
	virtual		bool			WriteXMLObjects (const PSObjProps* pes, XMLElement *inNode);

private:
	// defensive programming - not implemented
								DMPict (const DMPict &inOriginal);
				DMPict		&	operator = (const DMPict &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
//	RWTextValue				mSource;		// URI?
//	union
//	{
//		EPictFormat			mFormat;		// static (pasted) picture
		int					mFormatI;
//	};
	bool					mFrame;
	float					mFrameOffset;
	RWPicture				mPicture;
	RWPicture				mPicture4D;
	RWPictData			*	mComposerPictureData;
	float					mObjectRotation;
};

inline float
DMPict::GetRotation() const {return mObjectRotation;}

// DM Text object
class	DMText
	:	public	DMOval
{
public:
	static		DMText		*	Create (DMBase *inParent, XMLElement *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		Ti*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		void			ParseData (RWPageComposer *inComposer);
//	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);

	inline		long			GetStyleID (void) const;

protected:
								DMText (DMBase *inParent);
								DMText (DMBase *inParent, const DMText &inOriginal);
	virtual						~DMText (void);
				void			Init (void);
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
	// defensive programming - not implemented
								DMText (const DMText &inOriginal);
				DMText		&	operator = (const DMText &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	long					mStyleID;
	bool					mIsDynamic;
	bool					mIsAttributed;
	bool					mKeepTogether;
//	union
//	{
//		EEmpty				mDrawIfEmpty;
		int					mDrawIfEmptyI;
//		bool					mDrawIfEmpty;
//	}
	bool						mFrame;
	float						mFrameOffset;
	RWTextValue				mText;
	RWTextValue				mParsedText;	//mbs 31052010	for "preview" in editor
	
	RWStyle					mStyle;
};


// DM Variable object - Field
class	DMVariable
	:	public	DMText
{
public:
	enum	SR4DVariable_ArrayElement
	{
		SR4DVariable_Expression = -3,	//mbs 17062010
		SR4DVariable_Variable = -2,
		SR4DVariable_ArrayAuto = -1
	};

	static		DMVariable	*	Create (DMBase *inParent, XMLElement *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		Ti*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		void			ParseData (RWPageComposer *inComposer);

	inline		bool			HasScript (void) const;
protected:
								DMVariable (DMBase *inParent);
								DMVariable (DMBase *inParent, const DMVariable &inOriginal);
	virtual						~DMVariable (void);
				void			Init (void);
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

private:
	// defensive programming - not implemented
								DMVariable (const DMVariable &inOriginal);
			DMVariable		&	operator = (const DMVariable &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	RWTextValue				mAlias;
	RWTextValue				mFormat;
//	int						mType;		// var, array {iteration}, array {index}
	long					mIndex;		// array {index}
//	bool					mCalcShow;
//	union
//	{
//		ECalcType			mCalcType;
		int					mCalcTypeI;
//	};
//	RWTextValue				mRecordCalcInto;
//	union
//	{
//		ERepeat				mRepeat;
		int					mRepeatI;
//	};
	float					mRepeatOffset;	// offset for repeat
	ExtendedExecute			mScript;
	RWPicture				mPicture4D;				//mbs 29072011
	RWPictData			*	mComposerPictureData;	//mbs 29072011
	float					mObjectRotation;		// pB v1.4
};

inline bool
DMVariable::HasScript (void) const	{return !mScript.IsEmpty();}

// DM Field object
class	DMField
	:	public	DMVariable
{
public:
	static		DMField		*	Create (DMBase *inParent, XMLElement *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

//	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		void			ParseData (RWPageComposer *inComposer);

protected:
								DMField (DMBase *inParent);
								DMField (DMBase *inParent, const DMField &inOriginal);
	virtual						~DMField (void);

private:
	// defensive programming - not implemented
								DMField (const DMField &inOriginal);
			DMField			&	operator = (const DMField &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
};


// forward declarations
class	DMTable;


// DM Table Header object
class	DMHeader
	:	public	DMText
{
friend	class	DMTable;	// constructor, LoadXML, SetData, SetWidth

public:
//	static		DMHeader	*	Create (DMBase *inParent, Ti *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
	virtual		XmlElement*	    WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);

	inline		long			GetStyleID (void) const;
	inline		float			GetWidth (void) const;
	inline		float			GetHeight (void) const;
	inline		const CText		GetText (void) const;
	inline		int				GetColSpan (void) const;
	inline		int				GetRowSpan (void) const;
	inline		bool			IsAttributed (void) const;

protected:
								DMHeader (DMBase *inParent);
								DMHeader (DMBase *inParent, const DMHeader &inOriginal);
	virtual						~DMHeader (void);
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

	inline		void			SetDMWidth (float inWidth);
	inline		void			SetDMHeight (float inHeight);
	inline		float			GetDMWidth (void) const;
	inline		float			GetDMHeight (void) const;
	inline		void			SetColSpan (int inColSpan);
	inline		void			SetRowSpan (int inRowSpan);

private:
	// defensive programming - not implemented
								DMHeader (const DMHeader &inOriginal);
			DMHeader		&	operator = (const DMHeader &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
//	RWTextValue				mText;
	float					mWidth;
	float					mHeight;
	int						mColSpan;
	int						mRowSpan;
//	long					mStyleID;
};


// DM Table Column object
class	DMColumn
	:	public	DMText
{
friend	class	DMTable;	// constructor, LoadXML, SetData, SetWidth

public:
//	static		DMColumn	*	Create (DMBase *inParent, Ti *inNode);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
	virtual		XMLElement*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);

	inline		long			GetStyleID (void) const;
	inline		float			GetWidth (void) const;
//	inline		void			SetWidth (float inWidth);
	inline		bool			GetGrid (void) const;
//	inline		const CText		GetFormat (void) const;
	inline		const CText		GetSource (void) const;
//	inline		bool			IsRowNum (void) const;
	inline		bool			IsAttributed (void) const;

protected:
								DMColumn (DMBase *inParent);
								DMColumn (DMBase *inParent, const DMColumn &inOriginal);
	virtual						~DMColumn (void);
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);

	inline		void			SetDMWidth (float inWidth);
	inline		void			SetDMHeight (float inHeight);
	inline		float			GetDMWidth (void) const;
	inline		float			GetDMHeight (void) const;

private:
	// defensive programming - not implemented
								DMColumn (const DMColumn &inOriginal);
			DMColumn		&	operator = (const DMColumn &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const UserProps	sUserProperties[];
	float					mWidth;
	bool					mGrid;
	bool					mPrintRowNum;
	bool					mPrintRepeatingValues;
	RWTextValue				mSource;
	RWTextValue				mAlias;
	RWTextValue				mFormat;
	long					mLevel;		// level of table relations - order of columns evaluation
	ExtendedExecute			mScript;	// query to get this column's value
//	RWTextValue				mTitle;		// implicit header
};


// DM Table object
class	DMTable
	:	public	DMOval
{
public:
	static		DMTable		*	Create (DMBase *inParent, XMLElement *inNode);
	virtual						~DMTable (void);

	virtual		long			GetUserProperties (const UserProps* &outProps) const;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
//	virtual		PSObjList	*	GetObjects (OSType id);
	virtual		XMLElement*	WriteXML (XMLNode *inParent, const PSObjProps* pes = NULL);

	virtual		void			AdjustDrawingPosition (RWPageComposer *inComposer, const SPoint inParent);
	virtual		void			Draw (RWPageComposer *inComposer, const SRect &inParent, EDrawDM inMode);
	virtual		EHitTest		HitTest (SPoint &inWhere, DMBase* &outObjectHit);
//	virtual		void			HandleTrackSelect (SRect &inWhere, UInt32 inFlags);
	inline		void			Recalculate (void);
	inline		long			GetStyleID (void) const;
	inline		float			GetDMFrameAdjustment (void) const;

protected:
								DMTable (DMBase *inParent);
								DMTable (DMBase *inParent, const DMTable &inOriginal);
				void			Init (void);
	virtual		void			LoadXML (XMLElement *inNode, const PSObjProps* pes = NULL);
	virtual		bool			GetObjects (OSType id, PSObjListD* &outList);
	virtual		void			LoadXMLObjects (const PSObjProps* pes, XMLElement *inNode);
	virtual		bool			WriteXMLObjects (const PSObjProps* pes, XMLElement *inNode);
	void						AdjustHeaders (void);
	void						PrepareForHeader (void);
	void						FixUpGrid (void);
	bool						ResizeGrid (int inNumHeaders, int inNumColumns);
	void						CalculateAll (RWPageComposer *inComposer);
	PSObjListD				*	GetHeaderRow (int inRow) const;
	DMHeader				*	GetHeader (PSObjListD *inRow, int inCol) const;
	DMHeader				*	GetHeader (int inRow, int inCol) const;
	DMColumn				*	GetColumn (int inCol) const;
	float						GetColsWidth (int inFrom, int inTo) const;

private:
	// defensive programming - not implemented
								DMTable (const DMTable &inOriginal);
				DMTable		&	operator = (const DMTable &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	static const PSObjProps	sPropertiesHead[];
	static const PSObjProps	sPropertiesHeader[];
	static const PSObjProps	sPropertiesColumns[];
	static const UserProps	sUserProperties[];
	long					mStyleID;
	int						mFrame;
	float					mFrameOffset;
	double					mHGridThickness;
	PSObjTableD				mHeaders;
	PSObjListD				mColumns;
	float					mRowHeight;
	bool					mDrawHeaders;
	bool					mDrawColumns;
	ExtendedExecute			mScript;
	int						mNumTopHeadings;
	int						mNumColumns;
	int						mCurHdrRow;
	float					mTopHeadingsHeight;
	RWList<float>			mTopRowHeights;
	RWList<float>			mColWidths;
	float					mTopHeadingsWidth;
	float					mRowHeightDM;
	bool					mColWidthsCalculated;
};


template <class T>
struct DMlessOrder
	: std::binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const { return static_cast <DMBase*> (x)->CompareOrder (static_cast <DMBase*> (y)) < 0; }
};
typedef	DMlessOrder<PSObject*>		DMObjectCompareOrder;



inline	DMBase *						DMBase::GetParent (void) const				{ return mParent; }
inline	void							DMBase::SetParent (DMBase *inParent)		{ mParent = inParent; }
inline	long							DMBase::GetOrder (void) const				{ return mSeqID; }
inline	void							DMBase::SetOrder (long inSeqID)				{ mSeqID = inSeqID; }
inline	bool							DMBase::GetSelected (void) const			{ return mSelected; }
inline	SRect							DMBase::GetPosition (void) const			{ return mPosition; }
inline	void							DMBase::SetPosition (SRect &inRect)			{ mPosition = inRect; }
inline	SRect							DMBase::GetDrawPosition (void) const		{ return mDrawRect; }
inline	bool							DMBase::IsVisible (void) const				{ return mVisible; }
inline	void							DMBase::SetVisible (bool inVisible)			{ mVisible = inVisible; }
inline	bool							DMBase::IsLocked (void) const				{ return mLocked != eLockState_Unlocked; }
inline	bool							DMBase::IsSelectable (void) const			{ return mVisible && mLocked != eLockState_Fully; }
inline	void							DMBase::SetLocked (DMBase::ELockState inLocked)		{ mLocked = int (inLocked); }
inline	UInt8							DMLine::GetFlags (void) const				{ return mFlags; }

inline	const PSObject::PSObjProps *	DMBase::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMObject::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	DMGroup::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMLine::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMOval::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMRect::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMPict::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMText::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMVariable::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	DMField::GetProperties (void) const			{ return sProperties; }
inline	const PSObject::PSObjProps *	DMHeader::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	DMColumn::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	DMTable::GetProperties (void) const			{ return sProperties; }

inline	long							DMText::GetStyleID (void) const				{ return mStyleID; }
//inline									DMHeader::~DMHeader (void)					{}
inline	long							DMHeader::GetStyleID (void) const			{ return mStyleID; }
inline	float							DMHeader::GetWidth (void) const				{ return mWidth; }
inline	float							DMHeader::GetHeight (void) const			{ return mHeight; }
inline	const CText						DMHeader::GetText (void) const				{ return mText; }
inline	int								DMHeader::GetColSpan (void) const			{ return mColSpan; }
inline	int								DMHeader::GetRowSpan (void) const			{ return mRowSpan; }
inline	bool							DMHeader::IsAttributed (void) const			{ return mIsAttributed; }
//inline	void							DMHeader::SetWidth (float inWidth)			{ mWidth = inWidth; }
//inline	void							DMHeader::SetHeight (float inHeight)		{ mHeight = inHeight; }
inline	float							DMHeader::GetDMWidth (void) const			{ return mPosition.Width(); }
inline	float							DMHeader::GetDMHeight (void) const			{ return mPosition.Height(); }
inline	void							DMHeader::SetDMWidth (float inWidth)		{ mPosition.right = mPosition.left + inWidth; }
inline	void							DMHeader::SetDMHeight (float inHeight)		{ mPosition.bottom = mPosition.top + inHeight; }
inline	void							DMHeader::SetColSpan (int inColSpan)		{ mColSpan = inColSpan; }
inline	void							DMHeader::SetRowSpan (int inRowSpan)		{ mRowSpan = inRowSpan; }
//inline									DMColumn::~DMColumn (void)					{}
inline	long							DMColumn::GetStyleID (void) const			{ return mStyleID; }
inline	float							DMColumn::GetWidth (void) const				{ return mWidth; }
//inline	void							DMColumn::SetWidth (float inWidth)			{ mWidth = inWidth; }
inline	bool							DMColumn::GetGrid (void) const				{ return mGrid; }
inline	const CText						DMColumn::GetSource (void) const			{ return mSource; }
inline	bool							DMColumn::IsAttributed (void) const			{ return mIsAttributed; }
inline	void							DMColumn::SetDMWidth (float inWidth)		{ mPosition.right = mPosition.left + inWidth; }
inline	void							DMColumn::SetDMHeight (float inHeight)		{ mPosition.bottom = mPosition.top + inHeight; }
inline	float							DMColumn::GetDMWidth (void) const			{ return mPosition.Width(); }
inline	float							DMColumn::GetDMHeight (void) const			{ return mPosition.Height(); }
inline	void							DMTable::Recalculate (void)					{ mColWidthsCalculated = false; }
inline	long							DMTable::GetStyleID (void) const			{ return mStyleID; }
inline	float							DMTable::GetDMFrameAdjustment (void) const	{ return mFrame? mThickness + 2 * mFrameOffset: 0.0; }

#endif
