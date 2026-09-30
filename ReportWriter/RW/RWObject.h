#ifndef	_RWObject_h_
# define	_RWObject_h_

# include	"RWPageComposer.h"
# include	<functional>

// forward declarations
class	RWDataSource;
class	RWReportData;
class	RWReportWriter;
class	RWStyle;
class	RWObject;
//class	RWBodySection;
//class	RWBreakSection;


// container for RWObject pointers, destructor deletes the RWObjects first
typedef	RWArray<RWObject*>	RWObjList;


// virtual object
class	RWObject
{
public:
	enum	EObject_Kind	{ eObject_Group, eObject_Line, eObject_Rect, eObject_Oval, eObject_Pict, eObject_Text, eObject_Var, eObject_Table };
	enum	EAlignment		{ eAlign_None = 0, eAlign_Left, eAlign_Center, eAlign_Right };
	enum	EDraw			{ eDraw_No = 0, eDraw_Yes = 1, eDraw_OnOverflow = 2, eDraw_Always = 3 };
	enum	EEmpty			{ eEmpty_Draw = 0, eEmpty_Remove, eEmpty_RemoveRow };
	enum	EDrawState		{ eDrawState_Done = 0, eDrawState_Horizontal = 1, eDrawState_Vertical = 2, eDrawState_Both = 3 };

	const	RWReportData	*	GetReportData (void) const;
			RWReportWriter	*	GetReportWriter (void) const;
			RWDataSource	&	GetDataSource (void) const;

			int					ComparePosition (const RWObject *other) const;
			int					CompareOrder (const RWObject *other) const;

				bool			GetMove (void) const;										// fixed position within the parent (section or group) ?
	virtual		bool			GetGrow (void) const;										// variable height dependant on content ?
				bool			GetBinding (void) const;									// variable height dependant on the parent (section or group) ?
				long			GetOrder (void) const;										// print order
				SRect			GetPosition (void) const;									// position relative to parent
				SRect			GetVirtualPosition (void) const;							// "printing" position relative to parent
				bool			WillingToPrint (bool inIsOverflow) const;
				float			GetVerticalExpansion (void) const;
	virtual		void			Reset (bool inAll);
	virtual		void			SetKeepTogether (void);
	virtual		EObject_Kind	GetKind (void) const = 0;
	virtual		bool			GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow);		// space needed to draw, relative to parent
	virtual		void			AdjustBounds (float hDelta, float vDelta);					// bound object: parent was resized
	virtual		void			Move (float hDelta, float vDelta);
	virtual		void			FetchCalcValue (RWReportWriter *inWriter);
	virtual		bool			RemoveRow (bool* outRemoveRow);
	virtual		EDrawState		Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow) = 0;
				 bool			CanDraw (SRect &inRect, bool &outIsPrinted);	
	virtual						~RWObject (void);

protected:
								RWObject (int inOrder);
    virtual		void			Parse (RWReportData *inReport, XMLElement *inNode);
				RWTextValue		GetVariableText (const CText inVariableName, const CText inFormat) const;
				void			AlignOnPage (const SRect &origRect, SRect &ioRect) const;	//mbs 04082010	left/center/right

private:
			// defensive programming - not implemented
								RWObject (const RWObject &inOriginal);
				RWObject	&	operator = (const RWObject &inOriginal);

protected:
	RWReportData	*	mReportData;
	long				mSeqID;
	SRect				mPosition;
//	bool				mFixedH;		// fixed position ==> GetMove
	bool				mFixedV;
//	bool				mBindH;			// stretch as needed ==> GetBinding
	bool				mBindV;
	EAlignment  		mAlignment;
	EDraw		  		mDraw;

	SRect				mVirtPosition;
	bool				mPrinted;
	SRect				mBounds;
};


struct RWlessPosition
{
	bool operator()(const T& x, const T& y) const { return x->ComparePosition (y) < 0; }
};


struct RWlessOrder
{
	bool operator()(const T& x, const T& y) const { return x->CompareOrder (y) < 0; }
};


// group object
class	RWGroup
	:	public	RWObject
{
public:
    static		RWGroup		*	Create (RWReportData *inReport, XMLElement *inNode, int inOrder);

	virtual		bool			GetGrow (void) const;										// variable height dependant on content ?
	virtual		void			Reset (bool inAll);
//	virtual		void			SetKeepTogether (void);
	virtual		EObject_Kind	GetKind (void) const;
	virtual		bool			GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow);
	virtual		void			AdjustBounds (float hDelta, float vDelta);
	virtual		void			FetchCalcValue (RWReportWriter *inWriter);
	virtual		EDrawState		Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow);

				RWObjList	*	GetObjects (void);

protected:
								RWGroup (int inOrder);
	virtual						~RWGroup (void);
    virtual		void			Parse (RWReportData *inReport, XMLElement *inNode);

private:
			// defensive programming - not implemented
								RWGroup (const RWGroup &inOriginal);
				RWGroup		&	operator = (const RWGroup &inOriginal);

protected:
//	bool				mExpandH;		// variable width ==> shrink/expand as needed
	bool				mExpandV;
	RWObjList			mObjects;
	bool				mEmptyByChild;
};


// line object
class	RWLine
	:	public	RWObject
{
public:
    static		RWLine		*	Create (RWReportData *inReport, XMLElement *inNode, int inOrder);

	virtual		EObject_Kind	GetKind (void) const;
	virtual		bool			GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow);
	virtual		void			AdjustBounds (float hDelta, float vDelta);
	virtual		EDrawState		Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow);

protected:
								RWLine (int inOrder);
	virtual						~RWLine (void);
    virtual		void			Parse (RWReportData *inReport, XMLElement *inNode);

private:
			// defensive programming - not implemented
								RWLine (const RWLine &inOriginal);
				RWLine		&	operator = (const RWLine &inOriginal);

protected:
	float				mThickness;
	SRGBColor			mLineColor;
	unsigned short		mFlags;	// RWLine_Flags
};


// oval
class	RWOval
	:	public	RWObject
{
public:
    static		RWOval		*	Create (RWReportData *inReport, XMLElement *inNode, int inOrder);

	virtual		EObject_Kind	GetKind (void) const;
	virtual		bool			GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow);
//	virtual		void			AdjustBounds (float hDelta, float vDelta);
	virtual		EDrawState		Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow);

protected:
								RWOval (int inOrder);
	virtual						~RWOval (void);
    virtual		void			Parse (RWReportData *inReport, XMLElement *inNode);

private:
			// defensive programming - not implemented
								RWOval (const RWOval &inOriginal);
				RWOval		&	operator = (const RWOval &inOriginal);

protected:
	float				mThickness;
	SRGBColor			mFrameColor;
	bool				mFill;
	SRGBColor			mFillColor;
};


// rectangle object
class	RWRect
	:	public	RWOval
{
public:
    static		RWRect		*	Create (RWReportData *inReport, XMLElement *inNode, int inOrder);

	virtual		EObject_Kind	GetKind (void) const;
//	virtual		bool			GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow);
//	virtual		void			AdjustBounds (float hDelta, float vDelta);
	virtual		EDrawState		Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow);

protected:
								RWRect (int inOrder);
	virtual						~RWRect (void);
    virtual		void			Parse (RWReportData *inReport, XMLElement *inNode);

private:
			// defensive programming - not implemented
								RWRect (const RWRect &inOriginal);
				RWRect		&	operator = (const RWRect &inOriginal);

protected:
	long				mRows;
	long				mCols;
	unsigned short		mFlags;	// RWRect_Flags
};


// static picture
class	RWPict
	:	public	RWObject
{
public:
    static		RWPict		*	Create (RWReportData *inReport, XMLElement *inNode, int inOrder);

	virtual		EObject_Kind	GetKind (void) const;
	virtual		bool			GetGrow (void) const;										// variable height dependant on content ?
	virtual		bool			GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow);
//	virtual		void			AdjustBounds (float hDelta, float vDelta);
	virtual		EDrawState		Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow);

protected:
								RWPict (int inOrder);
	virtual						~RWPict (void);
    virtual		void			Parse (RWReportData *inReport, XMLElement *inNode);

private:
			// defensive programming - not implemented
								RWPict (const RWPict &inOriginal);
				RWPict		&	operator = (const RWPict &inOriginal);

protected:
//	bool				mExpandH;		// variable width ==> shrink/expand as needed
	bool				mExpandV;
	EPictFormat			mFormat;
	bool				mFrame;
	float				mFrameOffset;
	float				mFrameThickness;
	SRGBColor			mFrameColor;
	RWPicture			mPicture;
	RWDataID			mDataID;
	RWPictData		*	mComposerPictureData;
	// ?? float				mRotation;
	float				mObjectRotation;
	int					mMirror;
	float				mSkew;
    SRGBColor			mFillColor;

//	RWPageComposer	*	mComposerPictureCreator;
};


// static text - may contain dynamic reference "<%var [ ;fmt [; fmt [; fmt]]] %>"
class	RWText
	:	public	RWObject
{
public:
    static		RWText		*	Create (RWReportData *inReport, XMLElement *inNode, int inOrder);

	virtual		EObject_Kind	GetKind (void) const;
	virtual		bool			GetGrow (void) const;										// variable height dependant on content ?
	virtual		bool			GetGrowH (void) const;										// variable height dependant on content ?
	virtual		void			Reset (bool inAll);
	virtual		void			SetKeepTogether (void);
	virtual		bool			GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow);
	virtual		void			AdjustBounds (float hDelta, float vDelta);
	virtual		void			FetchCalcValue (RWReportWriter *inWriter);
	virtual		bool			RemoveRow (bool* outRemoveRow);
	virtual		EDrawState		Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow);

protected:
								RWText (int inOrder);
	virtual						~RWText (void);
    virtual		void			Parse (RWReportData *inReport, XMLElement *inNode);
				RWTextValue		ParseText (void);

private:
			// defensive programming - not implemented
								RWText (const RWText &inOriginal);
				RWText		&	operator = (const RWText &inOriginal);

protected:
//	long				mStyleID;
	bool				mExpandH;		// variable width ==> shrink/expand as needed
	bool				mExpandV;
	bool				mIsDynamic;
	bool				mIsAttributed;
	bool				mKeepTogether;
	EEmpty				mDrawIfEmpty;
	bool				mFrame;
	float				mFrameOffset;
	float				mFrameThickness;
	SRGBColor			mFrameColor;
	RWTextValue			mVarName;		// for calculated values
//	double				mVarValue;		// for calculated values
	RWValue				mVarValue;		// for calculated values - RWVarMap needs permanent object, not a stack object
	RWStyle			*	mStyle;
	RWTextValue			mText;
//	int					mLinesPrinted;
	RWPrintText		*	mPrintText;
};


// variable - dynamic text
class	RWVariable
	:	public	RWText
{
friend class	RWReportData;

public:
    static		RWVariable	*	Create (RWReportData *inReport, XMLElement *inNode, int inOrder);

	virtual		EObject_Kind	GetKind (void) const;
	virtual		bool			GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow);
//	virtual		void			AdjustBounds (float hDelta, float vDelta);
	virtual		EDrawState		Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow);

protected:
								RWVariable (int inOrder);
	virtual						~RWVariable (void);
    virtual		void			Parse (RWReportData *inReport, XMLElement *inNode);

				void			GetVariableData (void);
				void			SetVariableText (const CText inConstValue);

private:
			// defensive programming - not implemented
								RWVariable (const RWVariable &inOriginal);
				RWVariable	&	operator = (const RWVariable &inOriginal);

protected:
	RWTextValue			mSource;
	RWTextValue			mFormat;
//	bool				mCalcShow;
	ECalcType			mCalcType;
};



// top headings
class	RWHeader
{
friend	class	RWTable;	// constructor, Parse, SetData, SetWidth

public:
inline			RWStyle		*	GetStyle (void) const;
inline			float			GetWidth (void) const;
inline			float			GetHeight (void) const;
inline			const CText	       	GetText (void) const;
inline			int				GetColSpan (void) const;
inline			int				GetStartCol (void) const;
inline			int				GetRowSpan (void) const;
inline			bool			IsAttributed (void) const;

								~RWHeader (void);
protected:
								RWHeader (void);
								RWHeader (const CText inText, int inColSpan, int inRowSpan, RWStyle *inStyle);
                void			Parse (RWReportData *inReport, XMLElement *inNode, long inStyleID);
inline			void			SetWidth (float inWidth);
inline			void			SetHeight (float inHeight);
inline			void			AdjustStartCol (int inCol);

private:
			// defensive programming - not implemented
								RWHeader (const RWHeader &inOriginal);
				RWHeader	&	operator = (const RWHeader &inOriginal);

protected:
	float				mWidth;
	float				mHeight;
	RWTextValue			mText;
	int					mColSpan;
	int					mRowSpan;
	RWStyle			*	mStyle;
	int					mStartCol;
	bool				mIsAttributed;
};


// column
class	RWColumn
{
friend	class	RWTable;	// Parse, SetStyle

public:
inline			int				GetID (void) const;
inline			RWStyle		*	GetStyle (void) const;
inline			float			GetWidth (void) const;
inline			void			SetWidth (float inWidth);
inline			bool			GetGrid (void) const;
inline			const CXMLText		GetFormat (void) const;
inline			bool			IsRowNum (void) const;
inline			bool			PrintRepeatingValues (void) const;
inline			bool			IsAttributed (void) const;

								~RWColumn (void);
protected:
								RWColumn (void);
								RWColumn (float inWidth, RWStyle *inStyle);
								RWColumn (const RWColumn &inOriginal);
    void			Parse (RWReportData *inReport, XMLElement *inNode, long inStyleID);
inline			void			SetStyle (RWStyle *inStyle);

private:
			// defensive programming - not implemented
				RWColumn&		operator = (const RWColumn &inOriginal);

protected:
	int					mId;
	float				mWidth;
	bool				mGrid;
	RWStyle			*	mStyle;
	RWTextValue			mFormat;
	bool				mPrintRowNum;
	bool				mPrintRepeatingValues;
	bool				mIsAttributed;
};

typedef	RWArray<RWHeader*>	RWHdrList;
typedef	RWArray<RWColumn*>	RWColList;


class	RWTable
	:	public	RWObject
{
public:
    static		RWTable		*	Create (RWReportData *inReport, XMLElement *inNode, int inOrder);

	virtual		void			Reset (bool inAll);
	virtual		EObject_Kind	GetKind (void) const;
	virtual		bool			GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow);
//	virtual		void			AdjustBounds (float hDelta, float vDelta);
	virtual		EDrawState		Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow);
				RWColList	*	GetColumns (void);
	virtual		bool			GetGrow(void) const;
protected:
								RWTable (int inOrder);
	virtual						~RWTable (void);
    virtual		void			Parse (RWReportData *inReport, XMLElement *inNode);
    void			ParseHeading (XMLElement *inNode);
				void			AdjustHeaders (void);
				void			AdjustColumns (void);
				void			CalculateAll (RWPageComposer &inComposer);
				RWHeader	*	GetHeader (int inRow, int inCol) const;
				RWColumn	*	GetColumn (int inCol) const;
				float			GetColsWidth (int inFrom, int inTo) const;
				RWTextValue		GetTableHeadingData (RWDataSource &inSrc, const void* inHeading, int inLine, int inCol, int &outSpan, int &outLevel) const;

private:
			// defensive programming - not implemented
								RWTable (const RWTable &inOriginal);
				RWTable		&	operator = (const RWTable &inOriginal);

protected:
	RWHdrList		*	mHeaders;
	RWColList			mColumns;
	RWStyle			*	mStyle;
	long				mStyleID;
	RWDataID			mDataID;
	int					mFrame;
	float				mFrameOffset;
	float				mFrameThickness;
	SRGBColor			mFrameColor;
	float				mHGridThickness;
	float				mRowHeight;
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

inline	RWObject::EObject_Kind	RWGroup::GetKind (void) const		{ return eObject_Group; }
inline	RWObject::EObject_Kind	RWLine::GetKind (void) const		{ return eObject_Line; }
inline	RWObject::EObject_Kind	RWRect::GetKind (void) const		{ return eObject_Rect; }
inline	RWObject::EObject_Kind	RWOval::GetKind (void) const		{ return eObject_Oval; }
inline	RWObject::EObject_Kind	RWPict::GetKind (void) const		{ return eObject_Pict; }
inline	RWObject::EObject_Kind	RWText::GetKind (void) const		{ return eObject_Text; }
inline	RWObject::EObject_Kind	RWVariable::GetKind (void) const	{ return eObject_Var; }
inline	RWObject::EObject_Kind	RWTable::GetKind (void) const		{ return eObject_Table; }
inline	bool					RWTable::GetGrow (void) const		{return true;}
inline	long					RWObject::GetOrder (void) const		{ return mSeqID; }
inline	void					RWObject::SetKeepTogether (void)	{}

#endif
