# include	"SRSection.h"
# include	"SRReportWriter.h"
# include	"SRDataSource.h"
# include	"PSObjProps.h"

# define	ALL_ObjProps	\
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }		},	\
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }					},	\
{ PSObjPropType,			false,	PSProps_Attribute,	PSProps_String,		"type",				{ NULL }						},	\
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},	\
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_String,		"id",				{ NULL }						},	\
{ PSObjPropHeight,			true,	PSProps_Attribute,	PSProps_Real,		"height",			{ NULL, 0, 0, INT_MAX }			},	\
{ PSObjPropMinSpace,		true,	PSProps_Attribute,	PSProps_Real,		"minSpace",			{ NULL, 0, 0, 512 }				},	\
{ PSObjPropDraw,			true,	PSProps_Attribute,	PSProps_Boolean,	"draw", 			{ NULL, 1, 0, 1 }				},	\
{ PSObjPropKeepTogether,	true,	PSProps_Attribute,	PSProps_Boolean,	"keepTogether", 	{ NULL, 0, 0, 1 }				},	\
{ PSObjPropPageThrow,		true,	PSProps_Attribute,	PSProps_List,		"pageThrow",		{ sPageThrow, 0 }				},	\
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},	\
{ PSObjPropObjects,			true,	PSProps_Childs,		PSProps_Objects,	"Objects",			{ NULL }						},	\
{ PSObjPropExpandV, 		true,	PSProps_Attribute,	PSProps_Boolean,	"fixedHeight",		{ NULL, 0, 0, 1 }				},


const PSObject::PSObjProps	SRSection::sProperties[] = {
ALL_ObjProps
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SRHeaderFooterSection::sProperties[] = {
ALL_ObjProps
{ PSObjPropBind,			true,	PSProps_Attribute,	PSProps_Boolean,	"bindToBottom",		{ NULL, 0, 0, 1 }				},
{ PSObjPropFixed,			true,	PSProps_Attribute,	PSProps_Real,		"fixed",			{ NULL, -1, 0, 512 }			},
{ PSObjPropFirstPage,		true,	PSProps_Attribute,	PSProps_Boolean,	"firstPage",		{ NULL, 1, 0, 1 }				},
{ PSObjPropEvenPage,		true,	PSProps_Attribute,	PSProps_Integer,	"evenPage",			{ NULL, 1, 0, 2 }				},
{ PSObjPropOddPage,			true,	PSProps_Attribute,	PSProps_Integer,	"oddPage",			{ NULL, 1, 0, 2 }				},
{ PSObjPropLastPage,		true,	PSProps_Attribute,	PSProps_Boolean,	"lastPage",			{ NULL, 1, 0, 1 }				},
{ PSObjPropFill,			true,	PSProps_Attribute,	PSProps_Boolean,	"fillPage",			{ NULL, 0, 0, 1 }				},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SRBreakSection::sProperties[] = {
ALL_ObjProps
{ PSObjPropBind,			true,	PSProps_Attribute,	PSProps_Boolean,	"bindToBottom",		{ NULL, 0, 0, 1 }				},
{ PSObjPropBreakOnField,	true,	PSProps_Attribute,	PSProps_String,		"breakOnField",		{ NULL }						},
{ PSObjPropBreakOnVariable,	true,	PSProps_Attribute,	PSProps_String,		"breakOnVariable",	{ NULL }						},
{ PSObjPropBreakOnArray,	true,	PSProps_Attribute,	PSProps_String,		"breakOnArray",		{ NULL }						},
{ PSObjPropBreakLevel,		true,	PSProps_Attribute,	PSProps_Integer,	"level",			{ NULL, 0, 0, 64 }				},
{ PSObjPropPrintAlways,		true,	PSProps_Attribute,	PSProps_Boolean,	"always",			{ NULL, 0, 0, 1 }				},
{ PSObjPropBreakOn,			true,	PSProps_Attribute,	PSProps_String,		"breakOn",			{ NULL }, true						},
{ PSObjPropBreakType,		true,	PSProps_Attribute,	PSProps_List,		"breakType",		{ sBreakType, eBreakOn_None }, true	},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

const PSObject::PSObjProps	SRWatermarkSection::sProperties[] = {
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }		},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }					},
{ PSObjPropType,			false,	PSProps_Attribute,	PSProps_String,		"type",				{ NULL }						},
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_String,		"id",				{ NULL }						},
{ PSObjPropDraw,			true,	PSProps_Attribute,	PSProps_Boolean,	"draw", 			{ NULL, 1, 0, 1 }				},
{ PSObjPropScript,			true,	PSProps_OneChild,	PSProps_String,		"Script",			{ NULL }						},
{ PSObjPropObjects,			true,	PSProps_Childs,		PSProps_Objects,	"Objects",			{ NULL }						},
{ PSObjPropFirstPage,		true,	PSProps_Attribute,	PSProps_Boolean,	"firstPage",		{ NULL, 1, 0, 1 }				},
{ PSObjPropEvenPage,		true,	PSProps_Attribute,	PSProps_Integer,	"evenPage",			{ NULL, 1, 0, 2 }				},
{ PSObjPropOddPage,			true,	PSProps_Attribute,	PSProps_Integer,	"oddPage",			{ NULL, 1, 0, 2 }				},
{ PSObjPropLastPage,		true,	PSProps_Attribute,	PSProps_Boolean,	"lastPage",			{ NULL, 1, 0, 1 }				},
{ PSObjPropOnTop,			true,	PSProps_Attribute,	PSProps_Boolean,	"onTop",			{ NULL, 0, 0, 1 }				},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};

#pragma	mark	-

// ---------------------------------------------------------------------------
// SRSection								Constructor			   [protected]
// ---------------------------------------------------------------------------

SRSection::SRSection (RWStringView inType)
	:	PSObject (eObject_Section),
		mHeight (0),
		mMinSpace (0),
		mDraw (true),
		mKeepTogether (false),
		mFromBottom (false),
		mPageThrow (ePageThrow_None),
		mFixedHeight (false)
{
	mType.assign (inType);
	if (RWStr::Equals (mType, "Footer"))
		mFromBottom = true;

	return;
}


// ---------------------------------------------------------------------------
// SRSection								Destructor				  [public]
// ---------------------------------------------------------------------------

SRSection::~SRSection (void)
{
	mType.clear();

	return;
}


// ---------------------------------------------------------------------------
// GetType															  [public]
// ---------------------------------------------------------------------------

const RWString
SRSection::GetType (void)
const
{
	return mType;
}


// ---------------------------------------------------------------------------
// GetName															  [public]
// ---------------------------------------------------------------------------

const RWString
SRSection::GetName (void)
const
{
	return mName;
}


// ---------------------------------------------------------------------------
// GetObjects														  [public]
// ---------------------------------------------------------------------------

SRObjListD *
SRSection::GetObjects (void)
{
	return &mObjects;
}


// ---------------------------------------------------------------------------
// GetPageThrow														  [public]
// ---------------------------------------------------------------------------

SRSection::EPageThrow
SRSection::GetPageThrow (void)
const
{
	return mPageThrow;
}


// ---------------------------------------------------------------------------
// IsEmpty															  [public]
// ---------------------------------------------------------------------------

bool
SRSection::IsEmpty (void)
const
{
	bool	isEmpty = true;
	if (	(not mScript.IsEmpty())
		||	(mFixedHeight)	//mbs 01012010	especially footer must not be deleted...
		||	(mObjects.size() != 0)
		||	(mDraw && mPageThrow != ePageThrow_None)
	)
		isEmpty = false;

	return isEmpty;
}


// ---------------------------------------------------------------------------
// NeedsProcessing													  [public]
// ---------------------------------------------------------------------------

bool
SRSection::NeedsProcessing (void)
const
{
	bool	needsProcessing = false;
	if (mDraw && (mFixedHeight || mObjects.size() != 0))	//mbs 02012010	mHeight != 0 - especially footer must not be deleted...
		needsProcessing = true;

	return needsProcessing;
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
SRSection::Parse (SRReportData *inReport,  RWXmlNode inNode)
{
	mReportData = inReport;
	LoadXML (inNode);

	// header must not be bound to bottom...
	if (! RWStr::EqualsNoCase (mType, "Footer") && ! RWStr::EqualsNoCase (mType, "BreakFooter"))
		mFromBottom = false;

	return;
}


// ---------------------------------------------------------------------------
// WriteSection														  [public]
// ---------------------------------------------------------------------------

#if 0
#endif

// ---------------------------------------------------------------------------
// WriteSection														  [public]
// ---------------------------------------------------------------------------

RWXmlNode
SRSection::WriteSection (RWXmlNode inParent, RWStringView inSectionName)
const
{
	RWXmlNode	elem = inParent.Append (inSectionName);

#if	TARGET_DEBUG
	elem.SetAttribute (u"iteration", GetDataSource().GetCurrentIteration());
#endif
	if (not mName.empty())
		elem.SetAttr (u"name", mName);
	if (not mID.empty())
		elem.SetAttr (u"id", mID);
	if (mHeight != 0)
		elem.SetAttribute (u"height", mHeight);
	if (mMinSpace != 0)
		elem.SetAttribute (u"minSpace", mMinSpace);
	if (not mDraw)
		elem.SetAttribute (u"draw", 0);
	if (mKeepTogether)
		elem.SetAttribute (u"keepTogether", 1);
	if (mFixedHeight)
		elem.SetAttribute (u"fixedHeight", 1);

	if (RWStr::EqualsNoCase (mType, "Footer"))
	{
		if (not mFromBottom)
			elem.SetAttribute (u"bindToBottom", 0);
	}
	else if (RWStr::EqualsNoCase (mType, "BreakFooter"))
	{
		if (mFromBottom)
			elem.SetAttribute (u"bindToBottom", 1);
	}
	if (mPageThrow != ePageThrow_None)
		elem.SetAttr (u"pageThrow", mPageThrow == ePageThrow_Before ? u"before" : u"after");

	return elem;
}

// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

#if 0
#endif

// ---------------------------------------------------------------------------
// FetchValues														  [public]
// ---------------------------------------------------------------------------

void
SRSection::FetchValues (bool inUseOld)
{
	if (not mScript.IsEmpty())
		GetDataSource().RunScript (mScript, this);

	if (mObjects.size() > 0)
	{
		SRObjListD::const_iterator	it;

		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			SRObject	*obj = *it;
			obj->Reset();
			obj->FetchValue (inUseOld);
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// FetchCalcValues													  [public]
// ---------------------------------------------------------------------------

void
SRSection::FetchCalcValues (void)
{
	if (mObjects.size() > 0)
	{
		SRObjListD::const_iterator	it;

		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			SRObject	*obj = *it;
			obj->FetchCalcValue();
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// GetDataSource													  [public]
// ---------------------------------------------------------------------------

SRDataSource&
SRSection::GetDataSource (void)
const
{
	return mReportData->GetReportWriter()->GetDataSource();
}



// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRSection::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropType:			outValue.SetText (mType); break;
		case PSObjPropName:			outValue.SetText (mName); break;
		case PSObjPropID:			outValue.SetText (mID); break;	//mbs 15112010
		case PSObjPropHeight:		outValue.SetReal (mHeight); break;
		case PSObjPropMinSpace:		outValue.SetReal (mMinSpace); break;
		case PSObjPropDraw:			outValue.SetBoolean (mDraw); break;
		case PSObjPropKeepTogether:	outValue.SetBoolean (mKeepTogether); break;
		case PSObjPropExpandV:		outValue.SetBoolean (mFixedHeight); break;
		case PSObjPropBind:
			if (! RWStr::EqualsNoCase (mType, "Footer") && ! RWStr::EqualsNoCase (mType, "BreakFooter"))
				return false;
			outValue.SetBoolean (mFromBottom);
			break;
		case PSObjPropPageThrow:	outValue.SetText (RWStr::FromASCII (sPageThrow [mPageThrow]));break;						
		case PSObjPropScript:		outValue.SetText (mScript); break;
		case PSObjPropObjects:		outValue.SetInteger (mObjects.size()); break;

		default:					return PSObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRSection::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropName:			return SetStringProperty (inValue, mName);
		case PSObjPropID:			return mReportData->GetReportWriter()->IsExport()? SetStringProperty (inValue, mID): false;	//mbs 15112010
		case PSObjPropHeight:		return SetRealProperty (inValue, mHeight, 0, 4096);
		case PSObjPropMinSpace:		return SetRealProperty (inValue, mMinSpace, 0, 4096);
		case PSObjPropDraw:			return SetBooleanProperty (inValue, mDraw);
		case PSObjPropKeepTogether:	return SetBooleanProperty (inValue, mKeepTogether);
		case PSObjPropExpandV:		return SetBooleanProperty (inValue, mFixedHeight);
		case PSObjPropBind:
			if (! RWStr::EqualsNoCase (mType, "Footer") && ! RWStr::EqualsNoCase (mType, "BreakFooter"))
				break;
			return SetBooleanProperty (inValue, mFromBottom);
		case PSObjPropPageThrow:
		{
			long	lVal;
			if ((lVal = SetListProperty (inValue, sPageThrow)) >= 0)
			{
				mPageThrow = EPageThrow (lVal);
				return true;
			}
			break;
		}

		case PSObjPropScript:		return SetStringProperty (inValue, mScript);

		default:					return PSObject::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// SRHeaderFooterSection					Constructor				  [public]
// ---------------------------------------------------------------------------

SRHeaderFooterSection::SRHeaderFooterSection (RWStringView inType)
	:	SRSection (inType),
		mFixed (-1),
		mFirstPage (true),
		mEvenPage (1),
		mOddPage (1),
		mLastPage (true),
		mFillPage (false)
{
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
SRHeaderFooterSection::Parse (SRReportData *inReport, RWXmlNode inNode)
{
	SRSection::Parse (inReport, inNode);

	if (! RWStr::EqualsNoCase (mType, "Footer"))
		mFillPage = false;

	return;
}

// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

#if 0
#endif

// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SRHeaderFooterSection::Write (RWXmlNode inParent, bool inUseCalculator)
const
{
    RWXmlNode me = WriteSection (inParent, mType);

	if (mFixed != -1)
		me.SetAttribute (u"fixed", mFixed);
	me.SetAttribute (u"firstPage", mFirstPage);
	me.SetAttribute (u"evenPage", mEvenPage);
	me.SetAttribute (u"oddPage", mOddPage);
	me.SetAttribute (u"lastPage", mLastPage);
	if (mFillPage)
		me.SetAttribute (u"fillPage", 1);

	if (mObjects.size() > 0)
	{
		SRObjListD::const_iterator	it;

		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			SRObject	*obj = *it;
			obj->Write (me, false, inUseCalculator);
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRHeaderFooterSection::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropFixed:		outValue.SetReal (mFixed); break;
		case PSObjPropFirstPage:	outValue.SetBoolean (mFirstPage); break;
		case PSObjPropEvenPage:		outValue.SetInteger (mEvenPage); break;
		case PSObjPropOddPage:		outValue.SetInteger (mOddPage); break;
		case PSObjPropLastPage:		outValue.SetBoolean (mLastPage); break;
		case PSObjPropFill:
			if (! RWStr::EqualsNoCase (mType, "Footer"))
				return false;
			outValue.SetBoolean (mFillPage);
			break;
			
		default:					return SRSection::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRHeaderFooterSection::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropFixed:		return SetRealProperty (inValue, mFixed, -1, 4096);
		case PSObjPropFirstPage:	return SetBooleanProperty (inValue, mFirstPage);
		case PSObjPropEvenPage:		return SetIntegerProperty (inValue, mEvenPage, 0, 2);
		case PSObjPropOddPage:		return SetIntegerProperty (inValue, mOddPage, 0, 2);
		case PSObjPropLastPage:		return SetBooleanProperty (inValue, mLastPage);
		case PSObjPropFill:
			if (! RWStr::EqualsNoCase (mType, "Footer"))
				break;
			return SetBooleanProperty (inValue, mFillPage);
			
		default:					return SRSection::SetProperty (id, inValue);
	}

	return false;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// SRBreakSection							Constructor				  [public]
// ---------------------------------------------------------------------------
SRBreakSection::SRBreakSection (RWStringView inType)
	:	SRSection (inType),
		mLevel (0),
		mPrintAlways (false),
		mBreakType (eBreakOn_None),
		mBreakObject (0)
{
}


// ---------------------------------------------------------------------------
// SRBreakSection							Constructor				  [public]
// ---------------------------------------------------------------------------
SRBreakSection::SRBreakSection (SRReportData *inReport, RWStringView inType, int inLevel)
	:	SRSection (inType),
		mLevel (inLevel),
		mPrintAlways (false),
		mBreakType (eBreakOn_None),
		mBreakObject (0)
{
	mReportData = inReport;
	mDraw = false;
}


// ---------------------------------------------------------------------------
// SRBreakSection							Destructor				  [public]
// ---------------------------------------------------------------------------

SRBreakSection::~SRBreakSection (void)
{
	return;
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
SRBreakSection::Parse (SRReportData *inReport, RWXmlNode inNode)
{
	SRSection::Parse (inReport, inNode);

	if (mBreakOn.empty())
		mBreakType = eBreakOn_None;

/*	empty sections are deleted -> SRData expects a const which gets destroyed...
	==> postpone the break creation --> moved into separate CreateBreak() function
	if (mBreakType != EBreakOn_None)
		mBreakObject = inReport->GetReportWriter()->CreateBreak (mBreakOn, mBreakType);
*/

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

#if 0
#endif

// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SRBreakSection::Write (RWXmlNode inParent, bool inUseCalculator)
const
{
    RWXmlNode me = WriteSection (inParent, mType);

	me.SetAttribute (u"level", mLevel);
	if (not IsEmpty())
	{
		if (mPrintAlways)
			me.SetAttribute (u"always", 1);
		if (mBreakType != eBreakOn_None && not mBreakOn.empty())
		{
//			static const char * breakOn[] = { "noBreak", "breakOnField", "breakOnVariable", "breakOnArray" };
//			me->SetAttribute (breakOn [mBreakType], mBreakOn);
			me.SetAttr (u"breakOn", mBreakOn);
		}

		if (mObjects.size() > 0)
		{
			SRObjListD::const_iterator	it;

			for (it = mObjects.begin(); it != mObjects.end(); it++)
			{
				SRObject	*obj = *it;
				obj->Write (me, false, inUseCalculator);
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRBreakSection::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropBreakOnField:
			if (mBreakType != eBreakOn_Field)
				return false;
			outValue.SetText (mBreakOn);
			break;

		case PSObjPropBreakOnVariable:
			if (mBreakType != eBreakOn_Variable)
				return false;
			outValue.SetText (mBreakOn);
			break;

		case PSObjPropBreakOnArray:
			if (mBreakType != eBreakOn_Array)
				return false;
			outValue.SetText (mBreakOn);
			break;

		case PSObjPropBreakLevel:		outValue.SetInteger (mLevel); break;
		case PSObjPropPrintAlways:
			if (! RWStr::EqualsNoCase (mType, "BreakHeader"))
				return false;
			outValue.SetBoolean (mPrintAlways);
			break;

		case PSObjPropBreakOn:			outValue.SetText (mBreakOn); break;
		case PSObjPropBreakType:		outValue.SetText (RWStr::FromASCII (sBreakType [mBreakType]));break;						
			
		default:						return SRSection::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRBreakSection::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropBreakOnField:
			if (SetStringProperty (inValue, mBreakOn))
			{
				mBreakType = eBreakOn_Field;
				return true;
			}
			break;
		case PSObjPropBreakOnVariable:
			if (SetStringProperty (inValue, mBreakOn))
			{
				mBreakType = eBreakOn_Variable;
				return true;
			}
			break;
		case PSObjPropBreakOnArray:
			if (SetStringProperty (inValue, mBreakOn))
			{
				mBreakType = eBreakOn_Array;
				return true;
			}
			break;

		case PSObjPropBreakOn:			return SetStringProperty (inValue, mBreakOn);
		case PSObjPropBreakType:
		{
			long	lVal;
			if ((lVal = SetListProperty (inValue, sBreakType)) >= 0)
			{
				mBreakType = EBreakOn (lVal);
				return true;
			}
			break;
		}

		case PSObjPropBreakLevel:		return SetIntegerProperty (inValue, mLevel, 0);
		case PSObjPropPrintAlways:		return 	(RWStr::EqualsNoCase (mType, "BreakHeader"))? SetBooleanProperty (inValue, mPrintAlways): false;

		default:						return SRSection::SetProperty (id, inValue);
	}

	return false;
}


// ---------------------------------------------------------------------------
// CreateBreak														  [public]
// ---------------------------------------------------------------------------

void
SRBreakSection::CreateBreak (SRReportData *inReport)
{
	if (mBreakType != eBreakOn_None)
		mBreakObject = inReport->GetReportWriter()->CreateBreak (mBreakOn, mBreakType);

	return;
}


// ---------------------------------------------------------------------------
// GetLevel															  [public]
// ---------------------------------------------------------------------------

int
SRBreakSection::GetLevel (void)
const
{
	return mLevel;
}


// ---------------------------------------------------------------------------
// IsBreak															  [public]
// ---------------------------------------------------------------------------

bool
SRBreakSection::IsBreak (long inIteration)
const
{
//	return mBreakObject != NULL && mBreakObject->IsChanged();
	bool	isBreak = false;

	if (mBreakObject != NULL)
	{
		mBreakObject->Fetch (inIteration, false);
		isBreak = mBreakObject->IsChanged();
	}

	return isBreak;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// SRPageSection							Constructor				  [public]
// ---------------------------------------------------------------------------
SRPageSection::SRPageSection (RWStringView inType)
	:	SRSection (inType)
{
}

// ---------------------------------------------------------------------------
// SRPageSection							Constructor				  [public]
// ---------------------------------------------------------------------------
SRPageSection::SRPageSection (bool inEmpty)
	:	SRSection (u"Body")
{
	mDraw = false;
}


// ---------------------------------------------------------------------------
// SRPageSection							Destructor				  [public]
// ---------------------------------------------------------------------------

SRPageSection::~SRPageSection (void)
{
	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SRPageSection::Write (RWXmlNode inParent, bool inUseCalculator)
const
{
// ••• TODO •••	emit values used in calculations (if inUseCalculator & variable is not in the body)

    RWXmlNode me = WriteSection (inParent, mType);

	if (mObjects.size() > 0)
	{
		SRObjListD::const_iterator	it;

		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			SRObject	*obj = *it;
			obj->Write (me, true, inUseCalculator);
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// WritePage														  [public]
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// WritePage														  [public]
// ---------------------------------------------------------------------------

RWXmlNode
SRPageSection::WritePage (RWXmlNode inParent, bool inSimple, bool inStart)
const
{
	// simple reports write straight into the report; otherwise each page opens
	// a <Page> element (inStart) and closing it returns to its parent
	if (inSimple)
		return inParent;
	if (inStart)
		return inParent.Append (u"Page");
	return inParent.Parent();
}


// ---------------------------------------------------------------------------
// GetPageOrientation												  [public]
// ---------------------------------------------------------------------------

const RWString
SRPageSection::GetPageOrientation (void)
const
{
//	return mPageOrientation;
	return RWString();
}


// ---------------------------------------------------------------------------
// GetPageSize														  [public]
// ---------------------------------------------------------------------------

const RWString
SRPageSection::GetPageSize (void)
const
{
//	return mPageSize;
	return RWString();
}


// ---------------------------------------------------------------------------
// CreateCalculatedObject											  [public]
// ---------------------------------------------------------------------------

void
SRPageSection::CreateCalculatedObjects (const RWList<RWCalculatedValue*>& inCalc)
{
	RWList<RWCalculatedValue*>::const_iterator	cit;
	long	order = 100000;
	
	for (cit = inCalc.begin(); cit != inCalc.end(); cit++)
	{
		const RWString					name = (*cit)->GetName();
		const SRObject				*obj = NULL;
		SRObjListD::const_iterator	it;
		
		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			obj = (*it)->FindCalculatedObject (name);
			if (obj != NULL)
				break;
		}
		if (obj == NULL)
		{
			SRObject	*no = SRObject::CreateCalculatedObject (mReportData, ++order, name);
			mObjects.push_back (no);
		}
	}
	return;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// SRWatermarkSection						Constructor				  [public]
// ---------------------------------------------------------------------------

SRWatermarkSection::SRWatermarkSection (RWStringView inType)
	:	SRHeaderFooterSection (inType),
		mOnTop (false)
{
}


// ---------------------------------------------------------------------------
// SRWatermarkSection						Destructor				  [public]
// ---------------------------------------------------------------------------

SRWatermarkSection::~SRWatermarkSection (void)
{
	return;
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
SRWatermarkSection::Parse (SRReportData *inReport, RWXmlNode inNode)
{
	SRHeaderFooterSection::Parse (inReport, inNode);

	mHeight = 0;
	mMinSpace = 0;
	mKeepTogether = false;
	mFromBottom = false;
	mPageThrow = ePageThrow_None;
	mFillPage = false;
	mFixed = -1;

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SRWatermarkSection::Write (RWXmlNode inParent, bool inUseCalculator)
const
{
    RWXmlNode me = WriteSection (inParent, mType);

	if (mOnTop)
		me.SetAttribute (u"onTop", 1);

	if (mObjects.size() > 0)
	{
		SRObjListD::const_iterator	it;

		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			SRObject	*obj = *it;
			obj->Write (me, false, inUseCalculator);
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRWatermarkSection::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropHeight:
		case PSObjPropMinSpace:
		case PSObjPropKeepTogether:
		case PSObjPropBind:
		case PSObjPropPageThrow:
		case PSObjPropFixed:
		case PSObjPropExpandV:
		case PSObjPropFill:			return false;

		case PSObjPropOnTop:		outValue.SetBoolean (mOnTop); break;
			
		default:					return SRHeaderFooterSection::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRWatermarkSection::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropHeight:
		case PSObjPropMinSpace:
		case PSObjPropKeepTogether:
		case PSObjPropBind:
		case PSObjPropPageThrow:
		case PSObjPropExpandV:
		case PSObjPropFixed:
		case PSObjPropFill:			break;
		case PSObjPropOnTop:		return SetBooleanProperty (inValue, mOnTop);
			
		default:					return SRHeaderFooterSection::SetProperty (id, inValue);
	}

	return false;
}
