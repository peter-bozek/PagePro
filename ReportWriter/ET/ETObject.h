/*
 *  ETObject.h
 *  ReportWriter
 *
 *  Created by Peter Bozek on 12/11/2010.
 *  Copyright 2010 INFORCE Bratislava. All rights reserved.
 *
 */

#ifndef	_ETObject_h_
# define	_ETObject_h_

# include	"RWBaseTypes.h"
# include	"RWObject.h"	// for RWlessPosition & RWlessOrder
# include	<functional>

// forward declarations
class	RWDataSource;
class	ETReportData;
class	ETReport;
class	RWStyle;
class	ETObject;

// container for RWObject pointers, destructor deletes the RWObjects first
typedef	RWArray<ETObject*>	ETObjList;


// virtual object
class	ETObject
{
friend class	ETReport;	// direct access to mName & mID

public:
	enum	EObject_Kind	{ eObject_Group, eObject_Line, eObject_Rect, eObject_Oval, eObject_Pict, eObject_Text, eObject_Var, eObject_Table };
	enum	EAlignment		{ eAlign_None = 0, eAlign_Left, eAlign_Center, eAlign_Right };
	enum	EDraw			{ eDraw_No = 0, eDraw_Yes = 1, eDraw_OnOverflow = 2, eDraw_Always = 3 };
	enum	EEmpty			{ eEmpty_Draw = 0, eEmpty_Remove, eEmpty_RemoveRow };
	enum	EDrawState		{ eDrawState_Done = 0, eDrawState_Horizontal = 1, eDrawState_Vertical = 2, eDrawState_Both = 3 };
	
	const	ETReportData *		GetReportData (void) const;
			ETReport *			GetReportWriter (void) const;
			RWDataSource &		GetDataSource (void) const;
	
	int                         ComparePosition (const ETObject *other) const;
	int                         CompareOrder (const ETObject *other) const;
	
	long                        GetOrder (void) const;										// print order
	SRect                       GetPosition (void) const;									// position relative to parent
	bool                        WillingToPrint () const;
    inline      RWTextValue     GetName();
    inline      RWTextValue     GetID();
	
	virtual		EObject_Kind	GetKind (void) const = 0;
	virtual		void			FetchCalcValue (ETReport *inWriter);
	virtual		void			Export () = 0;
	virtual						~ETObject (void);
protected:
								ETObject (int inOrder);
	virtual		void			Parse (ETReportData *inReport, XMLElement *inNode);
	RWTextValue                 GetVariableText (const CText inVariableName, const CText inFormat) const;
	
private:
	// defensive programming - not implemented
	ETObject (const ETObject &inOriginal);
	ETObject	&	operator = (const ETObject &inOriginal);
	
protected:
	RWTextValue			mName;
	RWTextValue			mID;
	ETReportData	*	mReportData;
	long				mSeqID;
	SRect				mPosition;
	EDraw		  		mDraw;
};

inline	RWTextValue		ETObject::GetName()		{return mName;}
inline	RWTextValue		ETObject::GetID()		{return mID;}

/*
template <class T>
struct RWlessPosition
: std::binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const { return x->ComparePosition (y) < 0; }
};
*/
typedef	RWlessPosition<ETObject*>		ETObjectComparePosition;


/*
template <class T>
struct RWlessOrder
: std::binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const { return x->CompareOrder (y) < 0; }
};
*/
typedef	RWlessOrder<ETObject*>		ETObjectCompareOrder;


// group object
class	ETGroup
:	public	ETObject
{
public:
	static		ETGroup		*	Create (ETReportData *inReport, XMLElement *inNode, int inOrder);
	
	virtual		EObject_Kind	GetKind (void) const;
	virtual		void			FetchCalcValue (ETReport *inWriter);
				void			Export ();
	
	ETObjList	*	GetObjects (void);
	
protected:
								ETGroup (int inOrder);
	virtual						~ETGroup (void);
	virtual		void			Parse (ETReportData *inReport, XMLElement *inNode);
	
private:
	// defensive programming - not implemented
	ETGroup (const ETGroup &inOriginal);
	ETGroup		&	operator = (const ETGroup &inOriginal);
	
protected:
	ETObjList			mObjects;
	bool				mEmptyByChild;
};



// static text - may contain dynamic reference "<%var [ ;fmt [; fmt [; fmt]]] %>"
class	ETText
:	public	ETObject
{
public:
	static		ETText		*	Create (ETReportData *inReport, XMLElement *inNode, int inOrder);
	
	virtual		EObject_Kind	GetKind (void) const;
	virtual		void			FetchCalcValue (ETReport *inWriter);
	virtual		void			Export (void);
				bool			IsAttributed (void) const;
    RWTextValue                 GetClass();

protected:
	ETText (int inOrder);
	virtual						~ETText (void);
	virtual		void			Parse (ETReportData *inReport, XMLElement *inNode);
	RWTextValue					ParseText (void);
	
private:
	// defensive programming - not implemented
	ETText (const ETText &inOriginal);
	ETText		&	operator = (const ETText &inOriginal);
	
protected:
	bool				mIsDynamic;
	bool				mIsAttributed;
	EEmpty				mDrawIfEmpty;
	RWTextValue			mVarName;		// for calculated values
	RWValue				mVarValue;		// for calculated values - RWVarMap needs permanent object, not a stack object
	RWStyle			*	mStyle;
	RWTextValue			mText;
};

inline bool				ETText::IsAttributed (void) const		{return mIsAttributed; }



// variable - dynamic text
class	ETVariable
:	public	ETText
{
	friend class	ETReportData;
	
public:
	static		ETVariable	*	Create (ETReportData *inReport, XMLElement *inNode, int inOrder);
	
	virtual		EObject_Kind	GetKind (void) const;
	virtual		void			Export ();
	
protected:
								ETVariable (int inOrder);
	virtual						~ETVariable (void);
	virtual		void			Parse (ETReportData *inReport, XMLElement *inNode);
	
	void			GetVariableData (void);
	void			SetVariableText (const CText inConstValue);
	
private:
	// defensive programming - not implemented
	ETVariable (const ETVariable &inOriginal);
	ETVariable	&	operator = (const ETVariable &inOriginal);
	
protected:
	RWTextValue			mSource;
	RWTextValue			mFormat;
	ECalcType			mCalcType;
};



// top headings
class	ETHeader
{
	friend	class	ETTable;	// constructor, Parse, SetData, SetWidth
	
public:
	inline			RWStyle		*	GetStyle (void) const;
	inline			const CText		GetText (void) const;
	inline			int				GetColSpan (void) const;
	inline			int				GetStartCol (void) const;
	inline			int				GetRowSpan (void) const;
	inline			bool			IsAttributed (void) const;
	
									~ETHeader (void);
protected:
									ETHeader (void);
									ETHeader (const CText inText, int inColSpan, int inRowSpan, RWStyle *inStyle);
	void			Parse (ETReportData *inReport, XMLElement *inNode, long inStyleID);
	inline			void			AdjustStartCol (int inCol);
	
private:
	// defensive programming - not implemented
	ETHeader (const ETHeader &inOriginal);
	ETHeader	&	operator = (const ETHeader &inOriginal);
	
protected:
	RWTextValue			mText;
	int					mColSpan;
	int					mRowSpan;
	RWStyle			*	mStyle;
	int					mStartCol;
	bool				mIsAttributed;
};


// column
class	ETColumn
{
	friend	class	ETTable;	// Parse, SetStyle
	
public:
	inline			int				GetID (void) const;
	inline			RWStyle		*	GetStyle (void) const;
	inline			bool			GetGrid (void) const;
	inline			const CText		GetFormat (void) const;
	inline			bool			IsRowNum (void) const;
	inline			bool			PrintRepeatingValues (void) const;
	inline			bool			IsAttributed (void) const;
	
									~ETColumn (void);
protected:
									ETColumn (void);
									ETColumn (float inWidth, RWStyle *inStyle);
									ETColumn (const ETColumn &inOriginal);
	void							Parse (ETReportData *inReport, XMLElement *inNode, long inStyleID);
	inline			void			SetStyle (RWStyle *inStyle);
	
private:
	// defensive programming - not implemented
	ETColumn&		operator = (const ETColumn &inOriginal);
	
protected:
	int					mId;
	RWStyle			*	mStyle;
	RWTextValue			mFormat;
	bool				mPrintRowNum;
	bool				mPrintRepeatingValues;
	bool				mIsAttributed;
};

typedef	RWArray<ETHeader*>	ETHdrList;
typedef	RWArray<ETColumn*>	ETColList;


class	ETTable
:	public	ETObject
{
public:
	static		ETTable		*	Create (ETReportData *inReport, XMLElement *inNode, int inOrder);
	
	virtual		void			Reset (bool inAll);
	virtual		EObject_Kind	GetKind (void) const;
	virtual		void			Export (void);
				ETColList	*	GetColumns (void);
	
protected:
								ETTable (int inOrder);
	virtual						~ETTable (void);
	virtual		void			Parse (ETReportData *inReport, XMLElement *inNode);
	void			ParseHeading (XMLElement *inNode);
	void			AdjustHeaders (void);
	void			AdjustColumns (void);
	ETHeader	*	GetHeader (int inRow, int inCol) const;
	ETColumn	*	GetColumn (int inCol) const;
	float			GetColsWidth (int inFrom, int inTo) const;
	RWTextValue		GetTableHeadingData (RWDataSource &inSrc, const void* inHeading, int inLine, int inCol, int &outSpan, int &outLevel) const;
	
private:
	// defensive programming - not implemented
			ETTable (const ETTable &inOriginal);
			ETTable		&	operator = (const ETTable &inOriginal);
	
protected:
	ETHdrList		*	mHeaders;
	ETColList			mColumns;
	RWStyle			*	mStyle;
	long				mStyleID;
	RWDataID			mDataID;
	int					mNumColumns;
	int					mNumRows;
	int					mNumTopHeadings;
	int					mNumLeftHeadings;
	bool				mFixedColumns;
	float				mTopHeadingsHeight;
	float				*mTopRowHeights;
	float				mLeftHeadingsWidth;
	int					mLinesPrinted;
	int					mLastPrintedColumn;
	int					mColsToPrint;
	int					mRowsToPrint;
	bool				mColWidthsCalculated;
	int					*mLeftHeadingsCurItem;
	int					*mLeftHeadingsStartPos;
};


inline	ETObject::EObject_Kind	ETGroup::GetKind (void) const		{ return eObject_Group; }
inline	ETObject::EObject_Kind	ETText::GetKind (void) const		{ return eObject_Text; }
inline	ETObject::EObject_Kind	ETVariable::GetKind (void) const	{ return eObject_Var; }
inline	ETObject::EObject_Kind	ETTable::GetKind (void) const		{ return eObject_Table; }
inline	long					ETObject::GetOrder (void) const		{ return mSeqID; }

#endif
