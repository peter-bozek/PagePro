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

SRHeader::SRHeader (const CText inText, int inColSpan, int inRowSpan, long inStyleID, SRTable* father)
	:	SRText (father->GetReportData(), father->GetOrder ()),
		mWidth (0),
		mHeight (0),
		mColSpan (inColSpan),
		mRowSpan (inRowSpan),
		// mStyleID (inStyleID),
		mStartCol (0)
{
	mObjectKind = eObject_TblHdr;
    mText.Copy (inText);
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
SRHeader::Parse (SRReportData *inReport, XMLElement *inNode, long inStyleID)
{
	mStyleID = inStyleID;

	PSObject::LoadXML (inNode);

#if	0
	XMLAttribute	*attrib;
	for (attrib = inNode->FirstAttribute(); attrib; attrib = attrib->Next())
	{
		const CXMLText	name = attrib->Name();
		const CXMLText	value = attrib->Value();

		if (STR_EQUALS (name, "style"))
		{
			mStyleID = 0;
			if (sscanf (value, "%li", &mStyleID) != 1)
				mStyleID = inStyleID;
		}
//		else if (STR_EQUALS (name, "encoding"))
//		{
//			if (sscanf (value, "%li", &mEncoding) != 1)
//				mEncoding = DEFAULT_ENCODING;
//		}
		else if (STR_EQUALS (name, "width"))
		{
			mWidth = 0;
			sscanf (value, "%g", &mWidth);
		}
		else if (STR_EQUALS (name, "height"))
		{
			mHeight = 0;
			sscanf (value, "%g", &mHeight);
		}
		else if (STR_EQUALS (name, "colspan"))
		{
			mColSpan = 1;
			sscanf (value, "%i", &mColSpan);
			if (mColSpan < 1)
				mColSpan = 1;
		}
		else if (STR_EQUALS (name, "rowspan"))
		{
			mRowSpan = 1;
			sscanf (value, "%i", &mRowSpan);
			if (mRowSpan < 1)
				mRowSpan = 1;
		}
	}


//	mStyle = inReport->GetStyle (styleID);
#endif
	mText = RWTools::ParseIntoText (inNode);

	return;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

void
SRHeader::WriteSelf (FILE *fd, const char* inObjectType)
{
	fprintf (fd, "<%s", inObjectType);
	if (mStyleID != 0)
		fprintf (fd, " style=\"%ld\"", mStyleID);
	if (mWidth != 0)
		fprintf (fd, " width=\"%g\"", mWidth);
	if (mHeight)
		fprintf (fd, " height=\"%g\"", mHeight);
	if (mColSpan > 1)
		fprintf (fd, " colspan=\"%d\"", mColSpan);
	if (mRowSpan > 1)
		fprintf (fd, " rowspan=\"%d\"", mRowSpan);
	if (mIsAttributed)
		fprintf (fd, " attr=\"1\"");

	if (not mParsedText.IsEmpty())
	{
		fprintf (fd, ">");
		RWTools::WriteText (fd, mParsedText);
		fprintf (fd, "</%s>\r\n", inObjectType);
	}
	else
		fprintf (fd, " />\r\n");

	return;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

XMLElement*
SRHeader::WriteSelf ( XMLElement *inParent, const char *inObjectType)
{
    XMLElement	* elem = inParent->GetDocument()->NewElement(inObjectType);
	if (mStyleID != 0)
		elem->SetAttribute ("style", (int)mStyleID);
	if (mWidth != 0)
		elem->SetAttribute ("width", mWidth);
	if (mHeight)
		elem->SetAttribute ("height", mHeight);
	if (mColSpan > 1)
		elem->SetAttribute ("colspan", mColSpan);
	if (mRowSpan > 1)
		elem->SetAttribute ("rowspan", mRowSpan);
	if (mIsAttributed)
		elem->SetAttribute ("attr", 1);

    XMLElement	*node = inParent->InsertEndChild (elem)->ToElement();

	if (not mParsedText.IsEmpty())
		RWTools::WriteText (node, mParsedText);

	return node;
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
	mParsedText.Copy (mText);
}


// ---------------------------------------------------------------------------
// ParseText														  [public]
// ---------------------------------------------------------------------------

void
SRHeader::ParseText (SRTable *inParent)
{
	if (not mText.IsEmpty())	//mbs 12022010	don't crash ;-)
	{
		long	textLen = mText.StrLength();
		long	curPos = 0, delta = 0, endPos;
		
		CText	result (mText);
		if (mIsDynamic)
		{
			CText varName;
			CText format;
			while (curPos < textLen && RWTools::ParseTextForVar (false, mText, textLen, curPos, endPos, varName, format))
			{
				const CText	varname = varName;
				bool		encode = mIsAttributed;
				if (!varname.empty() && varname[0] == '+')
				{
					varname.erase(0, 1);
					encode = false;
				}
				RWTextValue	varText;
				if (!varname.empty())
					varText = inParent->GetVariableText (varname);
				varName.clear();
				format.clear();
                
				size_t	varLen;
				if (varText)
					varLen = varText.size();
				else
					varLen = 0;
				result.clear (curPos - delta, endPos - curPos);
				if (varLen > 0)
				{
					if (encode)
					{
						CXMLText	encoded;
						CText	    us (varText, varLen);
						TiXmlBase::PutString ((const char*) us.GetUTF8(), &encoded);
						us.AssignUTF8 ((const UTF8Char*) encoded.c_str(), encoded.length());
						result.Insert (curPos - delta, us);
						varLen = us.StrLength();
					}
					else
						result.Insert (curPos - delta, varText, varLen);
				}
				delta += endPos - curPos - varLen;
				curPos = endPos;
			}
		}
		mParsedText.Attach (result.Release());
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
const CText
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
SRColumn::Parse (SRReportData *inReport, XMLElement *inNode, long inStyleID)
{
	mStyleID = inStyleID;

	PSObject::LoadXML (inNode);

#if	0
	XMLAttribute	*attrib;
	long			lVal;
	for (attrib = inNode->FirstAttribute(); attrib; attrib = attrib->Next())
	{
		const CXMLText	name = attrib->Name();
		const CXMLText	value = attrib->Value();

		if (STR_EQUALS (name, "id"))
		{
			mId = 0;
			sscanf (value, "%li", &mId);
		}
		else if (STR_EQUALS (name, "style"))
		{
			mStyleID = 0;
			if (sscanf (value, "%li", &mStyleID) != 1)
				mStyleID = inStyleID;
		}
//		else if (STR_EQUALS (name, "encoding"))
//		{
//			if (sscanf (value, "%li", &mEncoding) != 1)
//				mEncoding = DEFAULT_ENCODING;
//		}
		else if (STR_EQUALS (name, "width"))
		{
			mWidth = 0;
			sscanf (value, "%g", &mWidth);
		}
		else if (STR_EQUALS (name, "grid"))
		{
			lVal = 1;
			sscanf (value, "%li", &lVal);
			mGrid = (lVal != 0);
		}
		else if (STR_EQUALS (name, "source"))
		{
			if (STR_EQUALS (value, "%ROWNUM%"))		// •••
				mPrintRowNum = true;
			else
			{
				mPrintRowNum = false;
				mSource.FromXML (value);
				if (mSource)
					if (*mSource == '[')
						mVar = inReport->GetReportWriter()->CreateField (mSource, ECalcType_None);
					else
						mVar = inReport->GetReportWriter()->CreateVariable (mSource, SR4DVariable::SR4DVariable_ArrayAuto, ECalcType_None);
			}
		}
		else if (STR_EQUALS (name, "format"))
		{
			mFormat.FromXML (value);
		}
		else if (STR_EQUALS (name, "title"))
		{
			mTitle.FromXML (value);
		}
		else if (STR_EQUALS (name, "duplicates"))
		{
			lVal = 1;
			sscanf (value, "%li", &lVal);
			mPrintRepeatingValues = (lVal != 0);
		}
		else if (STR_EQUALS (name, "level"))
		{
			mLevel = 0;
			sscanf (value, "%li", &mLevel);
			if (mLevel < 0 || mLevel > 10)
				mLevel = 0;
		}
	}
#endif

	if (not mSource.IsEmpty() && TEXT_EQUALS (mSource, "%ROWNUM%"))
		mPrintRowNum = true;
	else
	{
		mPrintRowNum = false;
        if (not mSource.IsEmpty()) {
			if (*mSource == '[')
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

void
SRColumn::WriteSelf (FILE *fd, const char* inObjectType)
{
	fprintf (fd, "<%s id=\"%ld\"", inObjectType, mId);
	if (mStyleID != 0)
		fprintf (fd, " style=\"%ld\"", mStyleID);
	if (mWidth != 0)
		fprintf (fd, " width=\"%g\"", mWidth);
	if (not mGrid)
		fprintf (fd, " grid=\"0\"");
	if (not mFormat.IsEmpty())
	{
		CXMLText		name = mFormat.ToXML();
		TIXML_STRING	tsname (name);
		mFormat.FreeXML (name);
		TIXML_STRING	encoded;
		TiXmlBase::PutString (tsname, &encoded);
		fprintf (fd, " format=\"%s\"", encoded.c_str());
	}
	if (mPrintRowNum)
		fprintf (fd, " rownum=\"1\"");
	if (mPrintRepeatingValues != 1)
		fprintf (fd, " duplicates=\"0\"");
	if (mIsAttributed)
		fprintf (fd, " attr=\"1\"");
	fprintf (fd, " />\r\n");

	return;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

XMLElement*
SRColumn::WriteSelf (XMLElement *inParent, const char *inObjectType)
{
		elem (inObjectType);
	elem.SetAttribute ("id", mId);
	if (mStyleID != 0)
		elem.SetAttribute ("style", mStyleID);
	if (mWidth != 0)
		elem.SetAttribute ("width", mWidth);
	if (not mGrid)
		elem.SetAttribute ("grid", 0L);
	if (not mFormat.IsEmpty())
	{
		CXMLText	xml = mFormat.ToXML();
		elem.SetAttribute ("format", xml);
		mFormat.FreeXML (xml);
	}
	if (mPrintRowNum)
		elem.SetAttribute ("rownum", 1L);
	if (mPrintRepeatingValues != 1)
		elem.SetAttribute ("duplicates", 0L);
	if (mIsAttributed)
		elem.SetAttribute ("attr", 1);

	XMLNode	*node = inParent->InsertEndChild (elem);

	return node->ToElement();
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
				mPrintRowNum = TEXT_EQUALS (mSource, "%ROWNUM%");
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
SRTable::Create (SRReportData *inReport, XMLElement *inNode, long inOrder)
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
SRTable::ParseHeading (XMLElement *inNode)
{
	mNumTopHeadings = 0;
	mFixedColumns = true;

    XMLElement	*elem;
	XMLNode		*node, *next;

	for (node = inNode->FirstChildElement ("tr"); node; node = next)
	{
		next = node->NextSibling();
		elem = node->ToElement();
		if (elem == NULL || !STR_EQUALS (elem->Value(), "tr"))
			continue;
		mNumTopHeadings++;
	}

	if (mNumTopHeadings > 0)
	{
		mHeaders = new SRHdrList [mNumTopHeadings];
		int	line = 0;
		int	numRows = 0, numCols = -1;
		for (node = inNode->FirstChildElement ("tr"); node && line < mNumTopHeadings; node = next, line++)
		{
			next = node->NextSibling();
			elem = node->ToElement();
			if (elem == NULL || !STR_EQUALS (elem->Value(), "tr"))
				continue;

			long			styleID = mStyleID;
			const char		*style = elem->Attribute ("style");
			if (style)
			{
				styleID = 0;
				if (sscanf (style, "%li", &styleID) != 1)
					styleID = mStyleID;
			}
            XMLElement	*tdElem = elem->FirstChildElement ("td");
			XMLNode		*tdNode, *tdNext;
			int				thisLineNumCols = 0;
			for (tdNode = tdElem; tdNode; tdNode = tdNext)
			{
				tdNext = tdNode->NextSibling();
				tdElem = tdNode->ToElement();
				if (tdElem == NULL || !STR_EQUALS (tdElem->Value(), "td"))
					continue;

				SRHeader	*hdr = new SRHeader (this);
				hdr->Parse (mReportData, tdElem, styleID);
				mHeaders [line].push_back (hdr);
				thisLineNumCols += hdr->GetColSpan();
				if (hdr->GetRowSpan() + line > numRows)
					numRows = hdr->GetRowSpan() + line;	//mbs 28102009	+ line
			}
			if (numCols == -1)
				numCols = thisLineNumCols;
			else
				assert (numCols >= thisLineNumCols);
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
SRTable::LoadXML (XMLElement *inNode, const PSObjProps* pes)
{
	SRObject::LoadXML (inNode);

//	mBindH = false;
	mBindV = false;
//	mFixedH = false;
	mFixedV = false;

	XMLAttribute	*attrib;
#if	0
//	mStyle = inReport->GetStyle (mStyleID);

	for (attrib = inNode->FirstAttribute(); attrib; attrib = attrib->Next())
	{
		const CXMLText	name = attrib->Name();
		const CXMLText	value = attrib->Value();

		if (STR_EQUALS (name, "style"))
		{
			mStyleID = 0;
			sscanf (value, "%li", &mStyleID);
		}
		else if (STR_EQUALS (name, "frame"))
		{
			sscanf (value, "%i", &mFrame);
			if (mFrame < 0 || mFrame > 2)
				mFrame = 1;
		}
		else if (STR_EQUALS (name, "frameOffset"))
		{
			mFrameOffset = 2;
			sscanf (value, "%g", &mFrameOffset);
			if (mFrameOffset < 0)
				mFrameOffset = 0;
			else if (mFrameOffset > 64)
				mFrameOffset = 64;
		}
		else if (STR_EQUALS (name, "frameThickness"))
		{
			mFrameThickness = 0;
			sscanf (value, "%g", &mFrameThickness);
			if (mFrameThickness < 0 || mFrameThickness > 10)
				mFrameThickness = 1;
		}
		else if (STR_EQUALS (name, "rowHeight"))
		{
			mRowHeight = 0;
			sscanf (value, "%g", &mRowHeight);
		}
		else if (STR_EQUALS (name, "cols"))
		{
			mNumColumns = 0;
			sscanf (value, "%i", &mNumColumns);
		}
	}

    XMLElement	*elem = inNode->FirstChildElement ("Script");
//	if (elem != NULL)
//		GetDataSource().ParseScript (mScript, elem);
#endif

    XMLElement	*elem = inNode->FirstChildElement ("Head");

	long		lVal;
	const char	*cVal;
	if (elem != NULL)
	{
		//mbs 08122009	don't parse if not drawn - support for DMTable
		lVal = 1;
		cVal = elem->Attribute ("draw");
		if (cVal)
		{
			if (sscanf (cVal, "%li", &lVal) != 1)
				lVal = 1;
		}
		if (lVal == 1)
			ParseHeading (elem);	// fixed table heading
		elem = inNode;
	}
//	else
	{
		elem = inNode->FirstChildElement ("Columns");
		if (elem != NULL)
		{
			//mbs 08122009	don't parse if not drawn - support for DMTable
			lVal = 1;
			cVal = elem->Attribute ("draw");
			if (cVal)
			{
				if (sscanf (cVal, "%li", &lVal) != 1)
					lVal = 1;
			}
			if (lVal != 1)
				elem = NULL;
		}
		if (elem != NULL)
		{
			for (attrib = elem->FirstAttribute(); attrib; attrib = attrib->Next())
			{
				const CXMLText	name = attrib->Name();
				const CXMLText	value = attrib->Value();

				if (STR_EQUALS (name, "rowHeight"))
				{
					mRowHeight = 0;
					sscanf (value, "%g", &mRowHeight);
				}
			}
		}
	}

	if (/* mDataID && */ elem)
	{
		XMLNode	*node, *next;

		inNode = elem;
		for (node = elem->FirstChildElement(); node; node = next)
		{
			next = node->NextSibling();
			elem = node->ToElement();
			if (elem == NULL || !STR_EQUALS (elem->Value(), "Col"))
			{
				continue;
			}

			SRColumn	*column = new SRColumn (this);
			column->Parse (mReportData, elem, mStyleID);
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
		if (column->GetTitle())
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
	if (mScript != NULL)
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

void
SRTable::Write (FILE *fd, bool inIsInBody, bool inUseCalculator)
{
	std::sort<SRColList::iterator, SRColumnCompareID> (mColumns.begin(), mColumns.end(), SRColumnCompareID());
	WriteSelf (fd, "Table");
	fprintf (fd, "</Table>\r\n");

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

XMLElement*
SRTable::Write (XMLElement *inParent, bool inIsInBody, bool inUseCalculator)
{
	std::sort<SRColList::iterator, SRColumnCompareID> (mColumns.begin(), mColumns.end(), SRColumnCompareID());
    XMLElement	*me = WriteSelf (inParent, "Table");

	return me;
}


// ---------------------------------------------------------------------------
// WriteSelf													   [protected]
// ---------------------------------------------------------------------------

void
SRTable::WriteSelf (FILE *fd, const char* inObjectType)
{
	SRObject::WriteSelf (fd, inObjectType);

	if (mDataID != 0)
		fprintf (fd, " dataID=\"%ld\"", mDataID);
	if (mStyleID != 0)
		fprintf (fd, " style=\"%ld\"", mStyleID);
	if (mFrame != 1)
		fprintf (fd, " frame=\"%d\"", mFrame);
	if (mFrameOffset != 2)
		fprintf (fd, " frameOffset=\"%g\"", mFrameOffset);
	if (mFrameThickness != 1)
		fprintf (fd, " frameThickness=\"%g\"", mFrameThickness);
	if (mHGridThickness != 0.5)
		fprintf (fd, " hGridThickness=\"%g\"", mHGridThickness);
	if (mFrameColor != cBlackColor)
		fprintf (fd, " frameColor=\"%s\"", (const char*) mFrameColor);

	if (mRowHeight != 0)
		fprintf (fd, " rowHeight=\"%g\"", mRowHeight);
	fprintf (fd, " cols=\"%d\">\r\n", mNumColumns);

	if (mNumTopHeadings > 0)
	{
		fprintf (fd, "<Head>\r\n");
		for (int line = 0; line < mNumTopHeadings; line++)
		{
			fprintf (fd, "<tr>\r\n");
			SRHdrList::const_iterator	hdr = mHeaders [line].begin();
			while (hdr != mHeaders [line].end())
			{
				(*hdr)->WriteSelf (fd, "td");
				hdr++;
			}
			fprintf (fd, "</tr>\r\n");
		}
		fprintf (fd, "</Head>\r\n");
	}

	if (mColumns.size() > 0)
	{
		fprintf (fd, "<Columns>\r\n");
		SRColList::const_iterator	col = mColumns.begin();
		while (col != mColumns.end())
		{
			(*col)->WriteSelf (fd, "Col");
			col++;
		}
		fprintf (fd, "</Columns>\r\n");
	}

	return;
}


XMLElement*
SRTable::WriteSelf (XMLElement *inParent, const char *inObjectType)
{
    XMLElement	*me = SRObject::WriteSelf (inParent, inObjectType);

	if (mDataID != 0)
		me->SetAttribute ("dataID", mDataID);
	if (mStyleID != 0)
		me->SetAttribute ("style", mStyleID);
	if (mFrame != 1)
		me->SetAttribute ("frame", mFrame);
	if (mFrameOffset != 2)
		me->SetAttribute ("frameOffset", mFrameOffset);
	if (mFrameThickness != 1)
		me->SetAttribute ("frameThickness", mFrameThickness);
	if (mHGridThickness != 0.5)
		me->SetAttribute ("hGridThickness", mHGridThickness);
	if (mFrameColor != cBlackColor)
		me->SetAttribute ("frameColor", (const char*) mFrameColor);

	if (mRowHeight != 0)
		me->SetAttribute ("rowHeight", mRowHeight);
	me->SetAttribute ("cols", mNumColumns);

	if (mNumTopHeadings > 0)
	{
        XMLElement	head ("Head");
        XMLElement	*headers = me->InsertEndChild (head)->ToElement();
		for (int line = 0; line < mNumTopHeadings; line++)
		{
            XMLElement	tr ("tr");
            XMLElement	*headerline = headers->InsertEndChild (tr)->ToElement();
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
        XMLElement	cols ("Columns");
        XMLElement	*columns = me->InsertEndChild (cols)->ToElement();
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
		case PSObjPropFrameColor:		outValue.SetXMLText ((const char*) mFrameColor); break;
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
