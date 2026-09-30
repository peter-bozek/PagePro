#ifndef	_SRObject_h_
# define	_SRObject_h_

# include	"RWBaseTypes.h"
# include	"PSObject.h"
# include	"ExtendedExecute.h"
# include	<functional>

// forward declarations
class	SRDataSource;
class	SRReportData;
class	SRReportWriter;
class	RWStyle;
class	SRObject;
class	SR4DData;
class	SRObject;

//typedef	RWList<SRObject*>		SRObjList;			// list
typedef	RWArray<SRObject*>		SRObjListD;			// list with destruction of objects
typedef	RWArray<SRObjListD*>	SRObjTableD;		// table with destruction of objects


// SR Object
class	SRObject
	:	public	PSObject
{
public:
			SRReportData	*	GetReportData (void) const;
			SRReportWriter	*	GetReportWriter (void) const;
			SRDataSource	&	GetDataSource (void) const;

				int				ComparePosition (const SRObject *other) const;
				SRect			GetPosition (void) const;									// position relative to parent

				int				CompareOrder (const SRObject *other) const;
				long			GetOrder (void) const;										// print order

	virtual		void			Reset (void);
	virtual		void			FetchValue (bool inUseOld);
	virtual		void			FetchCalcValue (void);
	virtual		void			Write (FILE *fd, bool inIsInBody, bool inUseCalculator) = 0;
    virtual		XMLElement*	Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator) = 0;
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
	virtual						~SRObject (void);

				RWTextValue		GetVariableText (const CText inVariableName, const CText inFormat) const;

	virtual	const SRObject	*	FindCalculatedObject (const CText inName) const;
	static		SRObject	*	CreateCalculatedObject (SRReportData *inReport, long inOrder, const CText inName);

protected:
								SRObject (SRReportData *inReport, long inOrder, EObject_Kind inKind);
    virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL) override;
	virtual		void			WriteSelf (FILE *fd, const char *inObjectType);
    virtual		XMLElement*	WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
								SRObject (const SRObject &inOriginal);
				SRObject	&	operator = (const SRObject &inOriginal);

protected:
	static const PSObjProps	sProperties[];
	SRReportData	*		mReportData;
	RWTextValue				mName;	//mbs 15112010
	RWTextValue				mID;	//mbs 15112010
	long					mSeqID;
	SRect					mPosition;
//	bool					mFixedH;		// fixed position ==> no Move
	bool					mFixedV;
//	bool					mBindH;			// stretch as needed ==> Grow
	bool					mBindV;
	EAlignment  			mAlignment;
	EDraw		  			mDraw;
};


/*
template <class T>
struct SRlessPosition
	: std::binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const { return x->ComparePosition (y) < 0; }
};
typedef	SRlessPosition<SRObject*>		SRObjectComparePosition;
*/


template <class T>
struct SRlessOrder
	: std::binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const { return x->CompareOrder (y) < 0; }
};
typedef	SRlessOrder<SRObject*>		SRObjectCompareOrder;



// SR Group object - no processing
class	SRGroup
	:	public	SRObject
{
public:
    static		SRGroup		*	Create (SRReportData *inReport, XMLElement *inNode, long inOrder);

	virtual		void			Reset (void);
	virtual		void			FetchValue (bool inUseOld);
	virtual		void			FetchCalcValue (void);
	virtual		void			Write (FILE *fd, bool inIsInBody, bool inUseCalculator);
    virtual		XMLElement*	Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator);
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

//	virtual		PSObjList	*	GetObjects (OSType id) const;
				SRObjListD	*	GetObjects (void);
	virtual	const SRObject	*	FindCalculatedObject (const CText inName) const;

protected:
								SRGroup (SRReportData *inReport, long inOrder);
	virtual						~SRGroup (void);
//	virtual		bool			GetObjects (OSType id, const PSObjListD* &outList) const;
//	virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL) override;
	virtual		void			WriteSelf (FILE *fd, const char *inObjectType);
    virtual		XMLElement*	WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
								SRGroup (const SRGroup &inOriginal);
				SRGroup		&	operator = (const SRGroup &inOriginal);

protected:
	static const PSObjProps	sProperties[];
//	bool					mExpandH;		// variable width ==> shrink/expand as needed
	bool					mExpandV;
	SRObjListD				mObjects;
};


// SR Line object - no processing
class	SRLine
	:	public	SRObject
{
public:
    static		SRLine		*	Create (SRReportData *inReport, XMLElement *inNode, long inOrder);

	virtual		void			Write (FILE *fd, bool inIsInBody, bool inUseCalculator);
    virtual		XMLElement*	Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator);
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

protected:
								SRLine (SRReportData *inReport, long inOrder);
	virtual						~SRLine (void);
//	virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL) override;
	virtual		void			WriteSelf (FILE *fd, const char *inObjectType);
    virtual		XMLElement*	WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
								SRLine (const SRLine &inOriginal);
				SRLine		&	operator = (const SRLine &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
	float				mThickness;
	SRGBColor			mLineColor;
	UInt8				mFlags;	// RWLine_Flags
};


// SR Oval object - no processing
class	SROval
	:	public	SRLine
{
public:
    static		SROval		*	Create (SRReportData *inReport, XMLElement *inNode, long inOrder);

	virtual		void			Write (FILE *fd, bool inIsInBody, bool inUseCalculator);
    virtual		XMLElement*	Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator);
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

protected:
								SROval (SRReportData *inReport, long inOrder);
								SROval (void);
	virtual						~SROval (void);
//	virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL) override;
	virtual		void			WriteSelf (FILE *fd, const char *inObjectType);
    virtual		XMLElement*	WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
								SROval (const SROval &inOriginal);
				SROval		&	operator = (const SROval &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
	bool				mFill;
	SRGBColor			mFillColor;
};


// SR Rect object - no processing
class	SRRect
	:	public	SROval
{
public:
    static		SRRect		*	Create (SRReportData *inReport, XMLElement *inNode, long inOrder);

	virtual		void			Write (FILE *fd, bool inIsInBody, bool inUseCalculator);
    virtual		XMLElement*	Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator);
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

protected:
								SRRect (SRReportData *inReport, long inOrder);
	virtual						~SRRect (void);
//	virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL) override;
	virtual		void			WriteSelf (FILE *fd, const char *inObjectType);
    virtual		XMLElement*	WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
								SRRect (const SRRect &inOriginal);
				SRRect		&	operator = (const SRRect &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
	long				mRows;
	long				mCols;
	UInt8				mFlags;	// RWRect_Flags
};


// SR Picture object - static picture - no processing
class	SRPict
	:	public	SROval
{
public:
    static		SRPict		*	Create (SRReportData *inReport, XMLElement *inNode, long inOrder);

	virtual		void			Write (FILE *fd, bool inIsInBody, bool inUseCalculator);
    virtual		XMLElement*	Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator);
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

protected:
								SRPict (SRReportData *inReport, long inOrder);
	virtual						~SRPict (void);
    virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL) override;
	virtual		void			WriteSelf (FILE *fd, const char *inObjectType);
    virtual		XMLElement*	    WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
								SRPict (const SRPict &inOriginal);
				SRPict		&	operator = (const SRPict &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
//	bool				mExpandH;		// variable width ==> shrink/expand as needed
	bool				mExpandV;
	EPictFormat			mFormat;
	bool				mFrame;
	float				mFrameOffset;
	RWPicture			mPicture;
	RWDataID			mDataID;
    float				mObjectRotation;
    int                 mMirror;
    float				mSkew;
    SRGBColor			mFillColor;

};


// SR Text object - "static" text - need to process <%var%> tags for 4D variables
// static text may contain dynamic reference "<%var [ ;fmt [; fmt [; fmt]]] %>"
class	SRText
	:	public	SROval
{
public:
    static		SRText		*	Create (SRReportData *inReport, XMLElement *inNode, long inOrder);

	virtual		void			Write (FILE *fd, bool inIsInBody, bool inUseCalculator);
    virtual		XMLElement*	Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator);
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

protected:
								SRText (SRReportData *inReport, long inOrder);
//								SRText (void);
	virtual						~SRText (void);
    virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL) override;
				RWTextValue		ParseText (bool& outStillDynamic) const;
				RWTextValue		LocalizeText (void) const;
	virtual		void			WriteSelf (FILE *fd, const char *inObjectType);
    virtual		XMLElement*	WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
								SRText (const SRText &inOriginal);
				SRText		&	operator = (const SRText &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
	long				mStyleID;	
//	bool				mExpandH;		// variable width ==> shrink/expand as needed
	bool				mExpandV;
	bool				mIsDynamic;
	bool				mIsAttributed;
	bool				mKeepTogether;
//	EEmpty				mDrawIfEmpty;
	int					mDrawIfEmpty;
	bool				mFrame;
	float				mFrameOffset;
	RWTextValue			mText;
};


// SR Variable object - result can be RWPict, RWText, RWVariable or RWTable (repeating objects)
class	SRVariable
	:	public	SRText
{
friend class	SRReportData;
friend class	SRObject;

public:
    static		SRVariable	*	Create (SRReportData *inReport, XMLElement *inNode, long inOrder);

	virtual		void			Reset (void);
	virtual		void			FetchValue (bool inUseOld);
	virtual		void			FetchCalcValue (void);
	virtual		void			Write (FILE *fd, bool inIsInBody, bool inUseCalculator);
    virtual		XMLElement*	Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator);
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
	
	virtual	const SRObject	*	FindCalculatedObject (const CText inName) const;

protected:
								SRVariable (SRReportData *inReport, long inOrder);
	virtual						~SRVariable (void);
    virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL) override;
//	virtual		void			WriteSelf (FILE *fd, const char *inObjectType);
//	virtual		*	WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
								SRVariable (const SRVariable &inOriginal);
				SRVariable	&	operator = (const SRVariable &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
	RWTextValue			mSource;
	RWTextValue			mFormat;
//	int					mType;		// var, array {iteration}, array {index}
	long				mIndex;		// array {index}
//	bool				mCalcShow;
	ECalcType			mCalcType;
//	RWTextValue			mRecordCalcInto;
	ERepeat				mRepeat;
	float				mRepeatOffset;	// offset for repeat
	ExtendedExecute		mScript;
	SR4DData		*	mVar;
	RWValue				mValue;
	RWDataID			mDataID;
};


// SR Field object - similar to SRVariable
class	SRField
	:	public	SRVariable
{
friend class	SRReportData;

public:
    static		SRField		*	Create (SRReportData *inReport, XMLElement *inNode, long inOrder);

//	virtual		void			Reset (void);
	virtual		void			FetchValue (bool inUseOld);
	virtual		void			FetchCalcValue (void);
//	virtual		void			Write (FILE *fd, bool inIsInBody, bool inUseCalculator);
//	virtual		*	Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator);
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

protected:
								SRField (SRReportData *inReport, long inOrder);
	virtual						~SRField (void);
//	virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL) override;
//	virtual		void			WriteSelf (FILE *fd, const char *inObjectType);
//	virtual		*	WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
								SRField (const SRField &inOriginal);
				SRField		&	operator = (const SRField &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
};



class	SRTable;
// top headings
class	SRHeader
	:	public	SRText 
{
friend	class	SRTable;

public:
inline			int				GetColSpan (void) const;
inline			int				GetStartCol (void) const;
inline			int				GetRowSpan (void) const;

	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
				void			ResetText (void);
				void			ParseText (SRTable *inParent);

								~SRHeader (void);
protected:
								SRHeader (SRTable* father);
								SRHeader (const CText inText, int inColSpan, int inRowSpan, long inStyleID, SRTable* father);
    void			Parse (SRReportData *inReport, XMLElement *inNode, long inStyleID);
inline			void			AdjustStartCol (int inCol);
				void			WriteSelf (FILE *fd, const char *inObjectType);
    XMLElement*	WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
								SRHeader (const SRHeader &inOriginal);
				SRHeader	&	operator = (const SRHeader &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
	float				mWidth;
	float				mHeight;
	RWTextValue			mText;
	int					mColSpan;
	int					mRowSpan;
	// long				mStyleID;		// pB is in text
	int					mStartCol;
	// bool				mIsDynamic;		// pB is in text
	// bool				mIsAttributed;	// pB is in text
	RWTextValue			mParsedText;
};


// column
class	SRColumn
	:	public	SRText
{
friend	class	SRTable;

public:
inline			long			GetID (void) const;
inline			bool			IsRowNum (void) const;
inline			SR4DData	*	GetData (void) const;
inline			long			GetLevel (void) const;
inline		    ExtendedExecute	&	GetQuery (void);
inline			const CText		GetTitle (void) const;

	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

								~SRColumn (void);
protected:
								SRColumn (SRTable * father);
								SRColumn (long inWidth, long inStyleID, SRTable * father);
								SRColumn (const SRColumn &inOriginal);
    void			Parse (SRReportData *inReport, XMLElement *inNode, long inStyleID);
inline			void			SetStyle (long inStyleID);
inline			void			SetID (long inID);
				void			WriteSelf (FILE *fd, const char *inObjectType);
    XMLElement*	WriteSelf (XMLElement *inParent, const char *inObjectType);

private:
			// defensive programming - not implemented
				SRColumn&		operator = (const SRColumn &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
	long				mId;
	float				mWidth;
	bool				mGrid;
	//long				mStyleID;		// pB is in text
	RWTextValue			mFormat;
	RWTextValue			mSource;
	bool				mPrintRowNum;
	bool				mPrintRepeatingValues;
	// bool				mIsAttributed;		// pB is in text
	long				mLevel;			// level of table relations - order of columns evaluation
	ExtendedExecute		mQuery;			// query to get this column's value
	SR4DData		*	mVar;
	RWTextValue			mTitle;
};

typedef	RWArray<SRHeader*>	SRHdrList;
typedef	RWArray<SRColumn*>	SRColList;

class	SRTable
	:	public	SRObject
{
public:
    static		SRTable		*	Create (SRReportData *inReport, XMLElement *inNode, long inOrder);

	virtual		void			Reset (void);
	virtual		void			FetchValue (bool inUseOld);
//	virtual		void			FetchCalcValue (void);
	virtual		void			Write (FILE *fd, bool inIsInBody, bool inUseCalculator);
    virtual		XMLElement*	Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator);
	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

protected:
								SRTable (SRReportData *inReport, long inOrder);
	virtual						~SRTable (void);
    virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL) override;
	virtual		void			WriteSelf (FILE *fd, const char *inObjectType);
    virtual		XMLElement*	WriteSelf (XMLElement *inParent, const char *inObjectType);

    void			ParseHeading (XMLElement *inNode);
				SRColList	*	GetColumns (void);
				void			AdjustHeaders (void);
				void			AdjustColumns (void);
				void			CreateHeadersFromColumns (void);
				SRHeader	*	GetHeader (int inRow, int inCol) const;
				SRColumn	*	GetColumn (int inCol) const;
				long			EmitValues (SRColList::iterator inStartCol, long inRowNum);
	
private:
			// defensive programming - not implemented
								SRTable (const SRTable &inOriginal);
				SRTable		&	operator = (const SRTable &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
	SRHdrList		*	mHeaders;
	SRColList			mColumns;
	long				mStyleID;
	int					mFrame;
	float				mFrameOffset;
	float				mFrameThickness;
	SRGBColor			mFrameColor;
	float				mHGridThickness;
	float				mRowHeight;
	int					mNumColumns;
	int					mNumTopHeadings;
	int					mNumLeftHeadings;
	bool				mFixedColumns;
	ExtendedExecute		mScript;
	RWDataID			mDataID;
};

inline	const PSObject::PSObjProps *	SRObject::GetProperties (void) const	{ return sProperties; }
inline	const PSObject::PSObjProps *	SRGroup::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	SRLine::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	SROval::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	SRRect::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	SRPict::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	SRText::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	SRVariable::GetProperties (void) const	{ return sProperties; }
inline	const PSObject::PSObjProps *	SRField::GetProperties (void) const		{ return sProperties; }
inline	const PSObject::PSObjProps *	SRTable::GetProperties (void) const		{ return sProperties; }

#endif
