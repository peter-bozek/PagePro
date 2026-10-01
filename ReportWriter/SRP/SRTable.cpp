# include	"SRObject.h"
# include	"SRReportData.h"
# include	"SRReportWriter.h"
# include	"SRDataSource.h"
# include	<assert.h>
# include	<algorithm>
# include	"PSObjProps.h"



inline	const PSObject::PSObjProps *	SRHeader::GetProperties (void) const	{ return sProperties; }
inline	const PSObject::PSObjProps *	SRColumn::GetProperties (void) const	{ return sProperties; }


static	const	char	sPrintRowNum[] = "%ROWNUM%";

const PSObject::PSObjProps	SRHeader::sProperties[] = {
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }		},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }					},
{ PSObjPropWidth,			true,	PSProps_Attribute,	PSProps_Real,		"width",			{ NULL, 0, 0, 512 }				},
{ PSObjPropHeight,			true,	PSProps_Attribute,	PSProps_Real,		"height",			{ NULL, 0, 0, 512 }				},
{ PSObjPropColSpan,			true,	PSProps_Attribute,	PSProps_Integer,	"colspan",			{ NULL, 1, 0, 100 }				},
{ PSObjPropRowSpan,			true,	PSProps_Attribute,	PSProps_Integer,	"rowspan",			{ NULL, 1, 0, 100 }				},
{ PSObjPropData,			true,	PSProps_Value,		PSProps_String,		"text",				{ NULL }						},
{ PSObjPropDynamic,			true,	PSProps_Attribute,	PSProps_Boolean,	"dynamic",			{ NULL, 0, 0, 1 }				},
{ PSObjPropAttributed,		true,	PSProps_Attribute,	PSProps_Boolean,	"attributed",		{ NULL, 0, 0, 1 }				},


{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }, true	},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }, true	},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }, true					},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }, true		},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }, true			},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }, true			},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropHorAlign,		true,	PSProps_Attribute,	PSProps_List,		"halign",			{ sJustification, 0 }, true		},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, 0 }	, true		},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }, true					},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }, true					},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }, true	},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }, true	},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }, true		},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }, true		},
	
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SRColumn::sProperties[] = {
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }		},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }					},
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_Integer,	"id",				{ NULL, 0, 1, LONG_MAX }		},
{ PSObjPropWidth,			true,	PSProps_Attribute,	PSProps_Real,		"width",			{ NULL, 0, 0, 512 }				},
{ PSObjPropGrid,			true,	PSProps_Attribute,	PSProps_Boolean,	"grid",				{ NULL, 1, 0, 1 }				},
{ PSObjPropSource,			true,	PSProps_Attribute,	PSProps_XMLString,	"source",			{ NULL }						},
{ PSObjPropFormat,			true,	PSProps_Attribute,	PSProps_String,		"format",			{ NULL }						},
{ PSObjPropData,			true,	PSProps_Value,		PSProps_String,		"text",				{ NULL }						},
{ PSObjPropRowNum,			true,	PSProps_Attribute,	PSProps_Boolean,	"rownum",			{ NULL, 0, 0, 1 }				},
{ PSObjPropDuplicates,		true,	PSProps_Attribute,	PSProps_Boolean,	"duplicates",		{ NULL, 0, 0, 1 }				},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},
{ PSObjPropLevel,			true,	PSProps_Attribute,	PSProps_Integer,	"level",			{ NULL, 0, 0, 10 }				},
{ PSObjPropAttributed,		true,	PSProps_Attribute,	PSProps_Boolean,	"attributed",		{ NULL, 0, 0, 1 }				},

{ PSObjPropStyle,			true,	PSProps_Attribute,	PSProps_Integer,	"style",			{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropBaseID,			true,	PSProps_Attribute,	PSProps_Integer,	"baseId",			{ NULL, -1, 0, LONG_MAX }, true	},
{ PSObjPropFlags,			true,	PSProps_Attribute,	PSProps_Integer,	"features",			{ NULL, 0, 0, ULONG_MAX }, true	},
{ PSObjPropFontName,		true,	PSProps_Attribute,	PSProps_String,		"font",				{ NULL }, true					},
{ PSObjPropSize,			true,	PSProps_Attribute,	PSProps_Real,		"size",				{ NULL, 0, 4, 128 }, true		},
{ PSObjPropStyleF,			true,	PSProps_Attribute,	PSProps_Integer,	"qdStyle",			{ NULL, 0, 0, 7 }, true			},
{ PSObjPropStyleB,			true,	PSProps_Attribute,	PSProps_Boolean,	"bold",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleI,			true,	PSProps_Attribute,	PSProps_Boolean,	"italic",			{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleU,			true,	PSProps_Attribute,	PSProps_Boolean,	"underline",		{ NULL, 0, 0, 1 }, true			},
{ PSObjPropStyleS,			true,	PSProps_Attribute,	PSProps_Boolean,	"strikethrough",	{ NULL, 0, 0, 1 }, true			},
{ PSObjPropWrap,			true,	PSProps_Attribute,	PSProps_Boolean,	"wrap",				{ NULL, 0, 0, 1 }, true			},
{ PSObjPropHorAlign,		true,	PSProps_Attribute,	PSProps_List,		"halign",			{ sJustification, 0 }, true		},
{ PSObjPropVertAlign,		true,	PSProps_Attribute,	PSProps_List,		"valign",			{ sVAlignment, 0 }	, true		},
{ PSObjPropTextColor,		true,	PSProps_Attribute,	PSProps_Color,		"textColor",		{ NULL }, true					},
{ PSObjPropBackColor,		true,	PSProps_Attribute,	PSProps_Color,		"backColor",		{ NULL }, true					},
{ PSObjPropRotation,		true,	PSProps_Attribute,	PSProps_Real,		"rotation",			{ NULL, 0, -360, 360 }, true	},
{ PSObjPropBaseLineShift,	true,	PSProps_Attribute,	PSProps_Real,		"baseLineShift",	{ NULL, 0, -100, 256 }, true	},
{ PSObjPropHorizontalScale,	true,	PSProps_Attribute,	PSProps_Real,		"hScale",			{ NULL, 1, 0.1, 100 }, true		},
{ PSObjPropLineSpacing,		true,	PSProps_Attribute,	PSProps_Real,		"lineSpacing",		{ NULL, 1.2, 0.5, 10 }, true		},

{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};


/*
inline	bool operator <  (const SRColumn &x, const SRColumn &y);
inline	bool operator == (const SRColumn &x, const SRColumn &y);

inline	bool operator < (const SRColumn &x, const SRColumn &y)
{
	return x.GetID() < y.GetID();
}

inline	bool operator == (const SRColumn &x, const SRColumn &y)
{
	return x.GetID() == y.GetID();
}
*/

template <class T>
struct SRlessID
{
	bool operator()(const T& x, const T& y) const {return (x->GetID() < y->GetID());}
};

typedef	SRlessID<SRColumn*>		SRColumnCompareID;

template <class T>
struct SRlessLevel
{
	bool operator()(const T& x, const T& y) const {return (x->GetLevel() < y->GetLevel());}
};

typedef	SRlessLevel<SRColumn*>		SRColumnCompareLevel;


#pragma	mark	-

// ---------------------------------------------------------------------------
// SRHeader									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SRHeader::SRHeader (SRTable* father)
	:	SRText (father->GetReportData(), father->GetOrder ()),
		mWidth (0),
		mHeight (0),
		mColSpan (1),
		mRowSpan (1),
		//mStyleID (0),
		mStartCol (0)
		//mIsDynamic (false),
		//mIsAttributed (false)
{
	mObjectKind = eObject_TblHdr;
}


// ---------------------------------------------------------------------------
// SRHeader									Constructor			   [protected]
// ---------------------------------------------------------------------------

SRHeader::SRHeader (const RWString inText, int inColSpan, int inRowSpan, long inStyleID, SRTable* father)
	:	SRText (father->GetReportData(), father->GetOrder ()),
		mWidth (0),
		mHeight (0),
		mColSpan (inColSpan),
		mRowSpan (inRowSpan),
		// mStyleID (inStyleID),
		mStartCol (0)
{
	mObjectKind = eObject_TblHdr;
    mText = inText;
	mStyleID = inStyleID;

	return;
}


// ---------------------------------------------------------------------------
// ~SRHeader								Destructor			   [protected]
// ---------------------------------------------------------------------------

SRHeader::~SRHeader (void)
{
//	mText.Free();

	return;
}


// ---------------------------------------------------------------------------
// GetColSpan														  [public]
// ---------------------------------------------------------------------------

inline
int
SRHeader::GetColSpan (void)
const
{
	return mColSpan;
}


// ---------------------------------------------------------------------------
// GetStartCol														  [public]
// ---------------------------------------------------------------------------

inline
int
SRHeader::GetStartCol (void)
const
{
	return mStartCol;
}


// ---------------------------------------------------------------------------
// AdjustStartCol												   [protected]
// ---------------------------------------------------------------------------

inline
void
SRHeader::AdjustStartCol (int inCol)
{
	mStartCol += inCol;

	return;
}


// ---------------------------------------------------------------------------
// GetRowSpan														  [public]
// ---------------------------------------------------------------------------

inline
int
SRHeader::GetRowSpan (void)
const
{
	return mRowSpan;
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
SRHeader::Parse (SRReportData *inReport, RWXmlNode inNode, long inStyleID)
{
	mStyleID = inStyleID;

	PSObject::LoadXML (inNode);

	mText = RWTools::ParseIntoText (inNode);

	return;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

RWXmlNode
SRHeader::WriteSelf (RWXmlNode inParent, const char *inObjectType)
{
	RWXmlNode	elem = inParent.Append (RWStr::FromASCII (inObjectType));
	if (mStyleID != 0)
		elem.SetAttribute (u"style", (int)mStyleID);
	if (mWidth != 0)
		elem.SetAttribute (u"width", mWidth);
	if (mHeight)
		elem.SetAttribute (u"height", mHeight);
	if (mColSpan > 1)
		elem.SetAttribute (u"colspan", mColSpan);
	if (mRowSpan > 1)
		elem.SetAttribute (u"rowspan", mRowSpan);
	if (mIsAttributed)
		elem.SetAttribute (u"attr", 1);

	if (not mParsedText.empty())
		RWTools::WriteText (elem, mParsedText);

	return elem;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRHeader::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
//		case PSObjPropStyle:		outValue.SetInteger (mStyleID); break;
		case PSObjPropWidth:		outValue.SetReal (mWidth); break;
		case PSObjPropHeight:		outValue.SetReal (mHeight); break;
		case PSObjPropColSpan:		outValue.SetInteger (mColSpan); break;
		case PSObjPropRowSpan:		outValue.SetInteger (mRowSpan); break;
//		case PSObjPropData:			outValue.SetText (mText); break;
//		case PSObjPropDynamic:		outValue.SetBoolean (mIsDynamic); break;
//		case PSObjPropAttributed:	outValue.SetBoolean (mIsAttributed); break;

		default:					return SRText::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRHeader::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
//		case PSObjPropStyle:		return SetIntegerProperty (inValue, mStyleID, 0);
		case PSObjPropWidth:		return SetRealProperty (inValue, mWidth, 0, 4096);
		case PSObjPropHeight:		return SetRealProperty (inValue, mHeight, 0, 1024);
		case PSObjPropColSpan:		return SetIntegerProperty (inValue, mColSpan, 0, 100);
		case PSObjPropRowSpan:		return SetIntegerProperty (inValue, mRowSpan, 0, 100);
//		case PSObjPropData:			return SetStringProperty (inValue, mText);
//		case PSObjPropDynamic:		return SetBooleanProperty (inValue, mIsDynamic);
//		case PSObjPropAttributed:	return SetBooleanProperty (inValue, mIsAttributed);

			
		default:					return SRText::SetProperty (id, inValue);
	}

	return false;
}


// ---------------------------------------------------------------------------
// ResetText														  [public]
// ---------------------------------------------------------------------------

void
SRHeader::ResetText (void)
{
	mParsedText = mText;
}


// ---------------------------------------------------------------------------
// ParseText														  [public]
// ---------------------------------------------------------------------------

void
SRHeader::ParseText (SRTable *inParent)
{
	if (not mText.empty())	//mbs 12022010	don't crash ;-)
	{
		long	textLen = (long) mText.size();
		long	curPos = 0, delta = 0, endPos;
		
		RWString	result (mText);
		if (mIsDynamic)
		{
			RWString	varName;
			RWString	format;
			while (curPos < textLen && RWTools::ParseTextForVar (false, mText, textLen, curPos, endPos, varName, format))
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
					varText = inParent->GetVariableText (RWString (varname), format);
				varName.clear();
				format.clear();

				result.erase (curPos - delta, endPos - curPos);
				if (encode)
					varText = RWTools::EscapeAttributedString (varText);
				result.insert (curPos - delta, varText);
				delta += endPos - curPos - (long) varText.size();
				curPos = endPos;
			}
		}
		mParsedText = result;
	}
	return;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// SRColumn									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

SRColumn::SRColumn (SRTable* father)
	:	SRText (father->GetReportData(), father->GetOrder ()),
		mId (0),
		mWidth (0),
		mGrid (true),
//		mStyleID (0),
		mPrintRowNum (false),
		mPrintRepeatingValues (true),
//		mIsAttributed (false),
		mLevel (0),
		mVar (0)
{
	mObjectKind = eObject_TblCol;
}


// ---------------------------------------------------------------------------
// SRColumn									Constructor			   [protected]
// ---------------------------------------------------------------------------

SRColumn::SRColumn (long inWidth, long inStyleID, SRTable* father)
	:	SRText (father->GetReportData(), father->GetOrder ()),
		mId (0),
		mWidth (inWidth),
		mGrid (true),
//		mStyleID (inStyleID),
		mPrintRowNum (false),
		mPrintRepeatingValues (true),
//		mIsAttributed (false),
		mLevel (0),
		mVar (0)
{
	mObjectKind = eObject_TblCol;
	mStyleID = inStyleID;
}


// ---------------------------------------------------------------------------
// SRColumn									Constructor			   [protected]
// ---------------------------------------------------------------------------

SRColumn::SRColumn (const SRColumn &inOriginal)
	:	SRText(inOriginal.GetReportData(), inOriginal.GetOrder()),
		mId (inOriginal.mId),
		mWidth (inOriginal.mWidth),
		mGrid (inOriginal.mGrid),
//		mStyleID (inOriginal.mStyleID),
//		mEncoding (inOriginal.mEncoding),
		mFormat (inOriginal.mFormat),
		mSource (inOriginal.mSource),
		mPrintRowNum (inOriginal.mPrintRowNum),
		mPrintRepeatingValues (inOriginal.mPrintRepeatingValues),
//		mIsAttributed (inOriginal.mIsAttributed),
		mLevel (inOriginal.mLevel),
		mQuery (inOriginal.mQuery),
		mVar (inOriginal.mVar),
		mTitle (inOriginal.mTitle)
{
	mObjectKind = eObject_TblCol;
	mStyleID = inOriginal.mStyleID;
	mIsAttributed = inOriginal.mIsAttributed;
}

// ---------------------------------------------------------------------------
// ~SRColumn								Destructor			   [protected]
// ---------------------------------------------------------------------------

SRColumn::~SRColumn (void)
{
	return;
}


// ---------------------------------------------------------------------------
// GetID															  [public]
// ---------------------------------------------------------------------------

inline
long
SRColumn::GetID (void)
const
{
	return mId;
}


// ---------------------------------------------------------------------------
// IsRowNum															  [public]
// ---------------------------------------------------------------------------

inline
bool
SRColumn::IsRowNum (void)
const
{
	return mPrintRowNum;
}


// ---------------------------------------------------------------------------
// GetData															  [public]
// ---------------------------------------------------------------------------

inline
SR4DData*
SRColumn::GetData (void)
const
{
	return mVar;
}


// ---------------------------------------------------------------------------
// GetLevel															  [public]
// ---------------------------------------------------------------------------

inline
long
SRColumn::GetLevel (void)
const
{
	return mLevel;
}


// ---------------------------------------------------------------------------
// GetQuery															  [public]
// ---------------------------------------------------------------------------

inline
ExtendedExecute&
SRColumn::GetQuery (void)
{
	return mQuery;
}


// ---------------------------------------------------------------------------
// GetTitle															  [public]
// ---------------------------------------------------------------------------

inline
const RWString
SRColumn::GetTitle (void)
const
{
	return mTitle;
}


// ---------------------------------------------------------------------------
// SetStyle														   [protected]
// ---------------------------------------------------------------------------

inline
void
SRColumn::SetStyle (long inStyleID)
{
	mStyleID = inStyleID;

	return;
}


// ---------------------------------------------------------------------------
// SetID														   [protected]
// ---------------------------------------------------------------------------

inline
void
SRColumn::SetID (long inID)
{
	mId = inID;

	return;
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
SRColumn::Parse (SRReportData *inReport, RWXmlNode inNode, long inStyleID)
{
	mStyleID = inStyleID;

	PSObject::LoadXML (inNode);


	if (not mSource.empty() && RWStr::Equals (mSource, "%ROWNUM%"))
		mPrintRowNum = true;
	else
	{
		mPrintRowNum = false;
        if (not mSource.empty()) {
			if (mSource[0] == u'[')
				mVar = inReport->GetReportWriter()->CreateField (mSource, ECalcType_None);
			else
				mVar = inReport->GetReportWriter()->CreateVariable (mSource, SR4DVariable::SR4DVariable_ArrayAuto, ECalcType_None);
        }
	}
	return;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

RWXmlNode
SRColumn::WriteSelf (RWXmlNode inParent, const char *inObjectType)
{
	RWXmlNode	elem = inParent.Append (RWStr::FromASCII (inObjectType));
	elem.SetAttribute (u"id", mId);
	if (mStyleID != 0)
		elem.SetAttribute (u"style", mStyleID);
	if (mWidth != 0)
		elem.SetAttribute (u"width", mWidth);
	if (not mGrid)
		elem.SetAttribute (u"grid", 0);
	if (not mFormat.empty())
		elem.SetAttr (u"format", mFormat);
	if (mPrintRowNum)
		elem.SetAttribute (u"rownum", 1);
	if (mPrintRepeatingValues != 1)
		elem.SetAttribute (u"duplicates", 0);
	if (mIsAttributed)
		elem.SetAttribute (u"attr", 1);

	return elem;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRColumn::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropID:			outValue.SetInteger (mId); break;
//		case PSObjPropStyle:		outValue.SetInteger (mStyleID); break;
		case PSObjPropWidth:		outValue.SetReal (mWidth); break;
		case PSObjPropGrid:			outValue.SetBoolean (mGrid); break;
		case PSObjPropSource:		outValue.SetText (mSource); break;
		case PSObjPropFormat:		outValue.SetText (mFormat); break;
		case PSObjPropData:			outValue.SetText (mTitle); break;
		case PSObjPropRowNum:		outValue.SetBoolean (mPrintRowNum); break;
		case PSObjPropDuplicates:	outValue.SetBoolean (mPrintRepeatingValues); break;
		case PSObjPropScript:		outValue.SetText (mQuery); break;
		case PSObjPropLevel:		outValue.SetInteger (mLevel); break;
//		case PSObjPropAttributed:	outValue.SetBoolean (mIsAttributed); break;

		default:					return SRText::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRColumn::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropID:			return SetIntegerProperty (inValue, mId, 0);
//		case PSObjPropStyle:		return SetIntegerProperty (inValue, mStyleID, 0);
		case PSObjPropWidth:		return SetRealProperty (inValue, mWidth, 0, 512);
		case PSObjPropGrid:			return SetBooleanProperty (inValue, mGrid);
		case PSObjPropSource:
			if (SetStringProperty (inValue, mSource))
			{
				mPrintRowNum = RWStr::Equals (mSource, "%ROWNUM%");
				return true;
			}
			break;
		case PSObjPropFormat:		return SetStringProperty (inValue, mFormat);
		case PSObjPropData:			return SetStringProperty (inValue, mTitle);
		case PSObjPropRowNum:		return SetBooleanProperty (inValue, mPrintRowNum);
		case PSObjPropDuplicates:	return SetBooleanProperty (inValue, mPrintRepeatingValues);
		case PSObjPropScript:		return SetStringProperty (inValue, mQuery);
		case PSObjPropLevel:		return SetIntegerProperty (inValue, mLevel, 0, 10);
//		case PSObjPropAttributed:	return SetBooleanProperty (inValue, mIsAttributed);

		default:					return SRText::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

SRTable*
SRTable::Create (SRReportData *inReport, RWXmlNode inNode, long inOrder)
{
	SRTable	*table = new SRTable (inReport, inOrder);
	table->LoadXML (inNode);

	return table;
}


// ---------------------------------------------------------------------------
// SRTable									Constructor			   [protected]
// ---------------------------------------------------------------------------

SRTable::SRTable (SRReportData *inReport, long inOrder)
	:	SRObject (inReport, inOrder, eObject_Table),
		mHeaders (0),
		mStyleID (0),
		mFrame (1),
		mFrameOffset (2),
		mFrameThickness (1),
		mFrameColor (cBlackColor),
		mHGridThickness (0.5),
		mRowHeight (0),
		mNumColumns (0),
		mNumTopHeadings (0),
		mNumLeftHeadings (0),
		mFixedColumns (false),
		mDataID (0)
{
}


// ---------------------------------------------------------------------------
// ~SRTable									Destructor			   [protected]
// ---------------------------------------------------------------------------

SRTable::~SRTable (void)
{
	if (mHeaders)
		delete [] mHeaders;

	return;
}


// ---------------------------------------------------------------------------
// ParseHeading													   [protected]
// ---------------------------------------------------------------------------

void
SRTable::ParseHeading (RWXmlNode inNode)
{
	mNumTopHeadings = 0;
	mFixedColumns = true;

	for (RWXmlNode row : inNode.Children())
		if (RWStr::EqualsNoCase (row.Name(), "tr"))
			mNumTopHeadings++;

	if (mNumTopHeadings > 0)
	{
		mHeaders = new SRHdrList [mNumTopHeadings];
		int	line = 0;
		int	numRows = 0, numCols = -1;
		for (RWXmlNode row : inNode.Children())
		{
			if (!RWStr::EqualsNoCase (row.Name(), "tr"))
				continue;
			if (line >= mNumTopHeadings)
				break;

			long	styleID = mStyleID;
			if (row.HasAttr (u"style"))
			{
				styleID = 0;
				if (!RWStr::ReadNumber (row.Attr (u"style"), styleID))
					styleID = mStyleID;
			}

			int		thisLineNumCols = 0;
			for (RWXmlNode cell : row.Children())
			{
				if (!RWStr::EqualsNoCase (cell.Name(), "td"))
					continue;

				SRHeader	*hdr = new SRHeader (this);
				hdr->Parse (mReportData, cell, styleID);
				mHeaders [line].push_back (hdr);
				thisLineNumCols += hdr->GetColSpan();
				if (hdr->GetRowSpan() + line > numRows)
					numRows = hdr->GetRowSpan() + line;	//mbs 28102009	+ line
			}
			if (numCols == -1)
				numCols = thisLineNumCols;
			else
				assert (numCols >= thisLineNumCols);
			line++;
		}
		if (numCols > 0 && mNumColumns > 0)
			assert (numCols == mNumColumns);
		mNumColumns = numCols;
	}

	return;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
SRTable::LoadXML (RWXmlNode inNode, const PSObjProps* pes)
{
	SRObject::LoadXML (inNode);

//	mBindH = false;
	mBindV = false;
//	mFixedH = false;
	mFixedV = false;

	// "draw" other than 1 means the part is not drawn (DMTable support) - mbs 08122009
	auto	drawn = [] (RWXmlNode inElem) -> bool
	{
		long	lVal = 1;
		if (inElem.HasAttr (u"draw") && !RWStr::ReadNumber (inElem.Attr (u"draw"), lVal))
			lVal = 1;
		return lVal == 1;
	};

	RWXmlNode	elem = inNode.Child (u"Head");
	if (elem && drawn (elem))
		ParseHeading (elem);	// fixed table heading

	elem = inNode.Child (u"Columns");
	if (elem && !drawn (elem))
		elem = RWXmlNode();
	if (elem && elem.HasAttr (u"rowHeight"))
	{
		mRowHeight = 0;
		RWStr::ReadNumber (elem.Attr (u"rowHeight"), mRowHeight);
	}

	if (/* mDataID && */ elem)
	{
		for (RWXmlNode col : elem.Children())
		{
			if (!RWStr::EqualsNoCase (col.Name(), "Col"))
				continue;

			SRColumn	*column = new SRColumn (this);
			column->Parse (mReportData, col, mStyleID);
			mColumns.push_back (column);
			if (column->GetID() == 0)
				column->SetID (mColumns.size());
		}
		if (mColumns.size() > 0 && mNumColumns > 0)
			assert (mColumns.size() == (size_t) mNumColumns);
		if (mNumColumns == 0)	//mbs 17122009
			mNumColumns = mColumns.size();
	}

	if (mNumTopHeadings > 0)
		AdjustHeaders();
//	else
		AdjustColumns();
	if (mNumTopHeadings == 0 && mNumColumns > 0)
		CreateHeadersFromColumns();

	return;
}


// ---------------------------------------------------------------------------
// AdjustHeaders												   [protected]
// ---------------------------------------------------------------------------

void
SRTable::AdjustHeaders (void)
{
	// adjust startColumn for headers
	int	line, col, hdr, last;
	for (line = 0; line < mNumTopHeadings; line++)
	{
		last = mHeaders [line].size();
		for (col = 0, hdr = 0; col < mNumColumns && hdr < last; hdr++)
		{
			SRHeader	*header = mHeaders [line] [hdr];
			int	subline, subcol, subhdr, colend;
			header->AdjustStartCol (col);
//				if (col == 0)
				col = header->GetStartCol();
			colend = col + header->GetColSpan();
			for (subline = line + header->GetRowSpan() - 1; subline > line; subline--)
			{
				int	sublast = mHeaders [subline].size();
				for (subcol = 0, subhdr = 0; subcol < colend && subhdr < sublast; subhdr++)
				{
					header = mHeaders [subline] [subhdr];
					if (col > header->GetStartCol())
						break;
					header->AdjustStartCol (1);
					subcol += header->GetColSpan();
				}
			}
			col = colend;
		}
	}

#if	0 && TARGET_DEBUG
	printf ("\nRW: AdjustHeaders: SRTABLE %ld, %d topHeadings, %d cols\n", mDataID, mNumTopHeadings, mNumColumns);
	for (line = 0; line < mNumTopHeadings; line++)
	{
		int	last = mHeaders [line].size();
		printf ("Line %d: %d", line + 1, last);
		for (hdr = 0; hdr < last; hdr++)
		{
			SRHeader	*header = mHeaders [line] [hdr];
			col = header->GetStartCol();
			printf ("\t%d,%d", col + 1, header->GetColSpan());
			col += header->GetColSpan();
		}
		printf ("\n");
	}
	printf ("\n");
#endif

	return;
}


// ---------------------------------------------------------------------------
// AdjustColumns												   [protected]
// ---------------------------------------------------------------------------

void
SRTable::AdjustColumns (void)
{
	SRColumn	*column, *def = NULL;

	// create real column order
	if (not mFixedColumns && mColumns.size() > 0)
	{
		SRColList	tmpCols (mColumns);
		std::sort<SRColList::iterator, SRColumnCompareID> (tmpCols.begin(), tmpCols.end(), SRColumnCompareID());
		mColumns.erase (mColumns.begin(), mColumns.end());	// mColumns.clear(); but w/o RWColumn deletion
		mColumns.reserve (mNumColumns);
		for (int cur = 1; cur <= mNumColumns; cur++)
		{
			SRColumn					*curcol, *right = NULL;
			SRColList::const_iterator	it;
			long						col = cur - mNumLeftHeadings;
			column = NULL;

			if (col > 0 || def == NULL)
				for (it = tmpCols.begin(); it != tmpCols.end(); it++)
				{
					curcol = *it;
					long	id = curcol->GetID();
					if (id == col)
					{
						column = curcol;
						break;
					}
					else if (id < 0 && id == (cur - mNumColumns - 1))
						right = curcol;
					else if (id == 0)
						def = curcol;
				}

			if (col <= 0)
				column = def;
			else if (column == NULL)
			{
				if (right == NULL)
					column = def;
				else
					column = right;
			}
			if (column == NULL)
				column = tmpCols [0];
			column = new SRColumn (*column);
			if (col <= 0)
				column->SetStyle (mStyleID);
			mColumns.push_back (column);
		}
	}

	// create missing [empty] columns
	if (mColumns.size() > 0)
	{
		while (mColumns.size() < (unsigned int) mNumColumns)
		{
			column = new SRColumn (0, mStyleID, this);
			mColumns.push_back (column);
			column->SetID (mColumns.size());
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// CreateHeadersFromColumns										   [protected]
// ---------------------------------------------------------------------------

void
SRTable::CreateHeadersFromColumns (void)
{
	SRColList::const_iterator	it;
	SRColumn					*column;
	bool						hasHeader = false;

	for (it = mColumns.begin(); it != mColumns.end(); it++)
	{
		column = *it;
		if (!column->GetTitle().empty())
		{
			hasHeader = true;
			break;
		}
	}

	if (hasHeader)
	{
		mNumTopHeadings = 1;
		mHeaders = new SRHdrList [mNumTopHeadings];

		std::sort<SRColList::iterator, SRColumnCompareID> (mColumns.begin(), mColumns.end(), SRColumnCompareID());
		for (it = mColumns.begin(); it != mColumns.end(); it++)
		{
			column = *it;

			SRHeader	*hdr = new SRHeader (column->GetTitle(), 1, 1, mStyleID, this);
			mHeaders [0].push_back (hdr);
		}
		AdjustHeaders();
	}

	return;
}


// ---------------------------------------------------------------------------
// GetHeader													   [protected]
// ---------------------------------------------------------------------------

SRHeader *
SRTable::GetHeader (int inRow, int inCol)
const
{
assert (inRow >= 0);
assert (inRow < mNumTopHeadings);
assert (inCol >= 0);
assert (inCol < mNumColumns);

	int			line, col, hdr;
	SRHeader	*header = NULL;
	for (line = inRow; line >= 0; line--)
	{
		int	last = mHeaders [line].size();
		for (col = 0, hdr = 0; col <= inCol && hdr < last; hdr++)
		{
			header = mHeaders [line] [hdr];
			if (header->GetStartCol() > inCol)
				break;
			col += header->GetColSpan();
		}
		if (header->GetStartCol() <= inCol && header->GetStartCol() + header->GetColSpan() >= inCol)
			break;
	}

	return header;
}


// ---------------------------------------------------------------------------
// GetColumn													   [protected]
// ---------------------------------------------------------------------------

SRColumn *
SRTable::GetColumn (int inCol)
const
{
assert (inCol >= 0);
assert (inCol < mNumColumns);

	return mColumns [inCol];
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
SRTable::Reset (void)
{
	mDataID = 0;

	if (mNumTopHeadings > 0)	//mbs 14012010	dynamic text in headers
	{
		for (int line = 0; line < mNumTopHeadings; line++)
		{
			SRHdrList::const_iterator	hdr = mHeaders [line].begin();
			while (hdr != mHeaders [line].end())
			{
				(*hdr)->ResetText();
				hdr++;
			}
		}
	}
	return;
}


// ---------------------------------------------------------------------------
// FetchValue														  [public]
// ---------------------------------------------------------------------------

void
SRTable::FetchValue (bool inUseOld)
{
	if (!mScript.Get().empty())
		GetDataSource().RunScript (mScript, this);

	long	emitted = 0;
	if (mColumns.size() > 0)
	{
		std::sort<SRColList::iterator, SRColumnCompareLevel> (mColumns.begin(), mColumns.end(), SRColumnCompareLevel());
		mDataID = GetDataSource().GetDataProvider().AddTableObject (mNumColumns, false);
		emitted = EmitValues (mColumns.begin(), 1);
	}

	if (emitted == 0 && mNumTopHeadings > 0)	//mbs 14012010	dynamic text in headers
	{
		for (int line = 0; line < mNumTopHeadings; line++)
		{
			SRHdrList::const_iterator	hdr = mHeaders [line].begin();
			while (hdr != mHeaders [line].end())
			{
				(*hdr)->ParseText (this);
				hdr++;
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// EmitValues														  [public]
// ---------------------------------------------------------------------------

long
SRTable::EmitValues (SRColList::iterator inStartCol, long inRowNum)
{
	bool		seen = false;
	long		numRowsThisLevel = 0;
	long		numRowsEmitted = 0;
	long		row;
	SRColumn	*col;
	SR4DData	*data;
	SRColList::iterator	cit;

	long		level = 0;
	for (cit = inStartCol; cit != mColumns.end(); cit++)
	{
		col = *cit;
		if (col->IsRowNum())
			continue;

        if (col->GetLevel() > level) {
			if (seen)
				break;
			else
				level = col->GetLevel();
        }
        
		seen = true;
		ExtendedExecute	&script = col->GetQuery();
		if (not script.IsEmpty())
			GetDataSource().RunScript (script, col);

		data = col->GetData();
		if (data)
		{
			if (level > 0)
				data->Invalidate();
			data->Fetch (1, true);
			row = data->GetSize();
		}
		else
			row = 0;
		if (row > numRowsThisLevel)
			numRowsThisLevel = row;
	}

	if (seen && (numRowsThisLevel > 0 || inStartCol != mColumns.begin()))
	{
		RWDataProvider::tableData*	table = GetDataSource().GetDataProvider().GetTableObject (mDataID);
		RWValue						value;

		if (numRowsThisLevel == 0)
			numRowsThisLevel = 1; // fill in with empty row pB
		
		for (row = 1; row <= numRowsThisLevel; row++)
		{
			for (cit = inStartCol; cit != mColumns.end(); cit++)
			{
				col = *cit;
				if (col->IsRowNum())
					continue;

				if (col->GetLevel() > level)
					break;

				data = col->GetData();
				if (data)
				{
//					if (level > 0 && row > 1)
					{
						data->Invalidate();
						data->Fetch (row, true);	//mbs 06082010	true instead of level > 0
					}
					data->GetValue (value);
					table->PutCellData (inRowNum + numRowsEmitted, col->GetID(), value);
				}
			}

			if (cit != mColumns.end())
				numRowsEmitted += EmitValues (cit, inRowNum + numRowsEmitted);
			else
			{
				numRowsEmitted++;
				if (inRowNum == 1 && numRowsEmitted == 1 && mNumTopHeadings > 0)	//mbs 14012010	dynamic text in headers
				{
					for (int line = 0; line < mNumTopHeadings; line++)
					{
						SRHdrList::const_iterator	hdr = mHeaders [line].begin();
						while (hdr != mHeaders [line].end())
						{
							(*hdr)->ParseText (this);
							hdr++;
						}
					}
				}
			}
		}
	}

	return numRowsEmitted;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

RWXmlNode
SRTable::Write (RWXmlNode inParent, bool inIsInBody, bool inUseCalculator)
{
	std::sort<SRColList::iterator, SRColumnCompareID> (mColumns.begin(), mColumns.end(), SRColumnCompareID());
    RWXmlNode me = WriteSelf (inParent, "Table");

	return me;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

RWXmlNode
SRTable::WriteSelf (RWXmlNode inParent, const char *inObjectType)
{
    RWXmlNode me = SRObject::WriteSelf (inParent, inObjectType);

	if (mDataID != 0)
		me.SetAttribute (u"dataID", mDataID);
	if (mStyleID != 0)
		me.SetAttribute (u"style", mStyleID);
	if (mFrame != 1)
		me.SetAttribute (u"frame", mFrame);
	if (mFrameOffset != 2)
		me.SetAttribute (u"frameOffset", mFrameOffset);
	if (mFrameThickness != 1)
		me.SetAttribute (u"frameThickness", mFrameThickness);
	if (mHGridThickness != 0.5)
		me.SetAttribute (u"hGridThickness", mHGridThickness);
	if (mFrameColor != cBlackColor)
		me.SetAttribute (u"frameColor", mFrameColor.ToString());

	if (mRowHeight != 0)
		me.SetAttribute (u"rowHeight", mRowHeight);
	me.SetAttribute (u"cols", mNumColumns);

	if (mNumTopHeadings > 0)
	{
		RWXmlNode	headers = me.Append (u"Head");
		for (int line = 0; line < mNumTopHeadings; line++)
		{
			RWXmlNode	headerline = headers.Append (u"tr");
			SRHdrList::const_iterator	hdr = mHeaders [line].begin();
			while (hdr != mHeaders [line].end())
			{
				(*hdr)->WriteSelf (headerline, "td");
				hdr++;
			}
		}
	}

	if (mColumns.size() > 0)
	{
		RWXmlNode	columns = me.Append (u"Columns");
		SRColList::const_iterator	col = mColumns.begin();
		while (col != mColumns.end())
		{
			(*col)->WriteSelf (columns, "Col");
			col++;
		}
	}

	return me;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRTable::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropStyle:			outValue.SetInteger (mStyleID); break;
		case PSObjPropFrame:			outValue.SetInteger (mFrame); break;
		case PSObjPropFrameOffset:		outValue.SetReal (mFrameOffset); break;
		case PSObjPropFrameThickness:	outValue.SetReal (mFrameThickness); break;
		case PSObjPropFrameColor:		outValue.SetText (mFrameColor.ToString()); break;
		case PSObjPropHGridThickness:	outValue.SetReal (mHGridThickness); break;
		case PSObjPropHeight:			outValue.SetReal (mRowHeight); break;
		case PSObjPropNumCols:			outValue.SetInteger (mNumColumns); break;
		case PSObjPropNumHeadings:		outValue.SetInteger (mNumTopHeadings); break;
		case PSObjPropScript:			outValue.SetText (mScript); break;

		case PSObjPropHeader:
		case PSObjPropColumn:			return false;

		default:						return SRObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRTable::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropStyle:			return SetIntegerProperty (inValue, mStyleID, 0);
		case PSObjPropFrame:			return SetIntegerProperty (inValue, mFrame, 0, 2);
		case PSObjPropFrameOffset:		return SetRealProperty (inValue, mFrameOffset, 0, 256);
		case PSObjPropFrameThickness:	return SetRealProperty (inValue, mFrameThickness, 0, 64);
		case PSObjPropFrameColor:		return SetColorProperty (inValue, mFrameColor);
		case PSObjPropHGridThickness:	return SetRealProperty (inValue, mHGridThickness, 0, 64);
		case PSObjPropHeight:			return SetRealProperty (inValue, mRowHeight, 0, 1024);
		case PSObjPropPosHeight:		
		case PSObjPropNumCols:
		case PSObjPropNumHeadings:
		case PSObjPropHeader:
		case PSObjPropColumn:			return false;
		case PSObjPropScript:			return SetStringProperty (inValue, mScript);

		default:						return SRObject::SetProperty (id, inValue);
	}

	return false;
}
