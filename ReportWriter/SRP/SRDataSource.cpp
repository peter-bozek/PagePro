# include	"SRDataSource.h"
# include	"SRDataFormatter.h"
# include	"SRReportData.h"
#if	WINVER
# include	"RWWinPageComposer.h"
# define	COMPOSER	RWWinPageComposer
#else
# include	"RWMacCGPageComposer.h"
# define	COMPOSER	RWMacCGPageComposer
#endif
# include	"4DPluginAPI.h"
// using namespace	FourDAPIEx;
# define	USE_CUSTOM_CALLBACK	1

#if	USE_CUSTOM_CALLBACK
# include	"PrivateTypes.h"
# include	"EntryPoints.h"
#endif

//# include	<stdio.h>	// snprintf
# include	<memory>	// auto_ptr
# include	"PSObjProps.h"


const PSObject::PSObjProps	SRDataSource::sProperties[] = {
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }		},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }					},
{ PSObjPropType,			false,	PSProps_Attribute,	PSProps_List,		"type",				{ s4DKind, -1 }					},
{ PSObjPropSource,			true,	PSProps_Attribute,	PSProps_List,		"source",			{ sSource, 0 }					},
{ PSObjPropIterations,		true,	PSProps_Attribute,	PSProps_Integer,	"iterations",		{ NULL, -1, 0, INT_MAX }		},
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_Integer,	"tableID",			{ NULL, 0, 1, INT_MAX }			},	// main table!
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},
{ PSObjPropRelateOne,		true,	PSProps_Attribute,	PSProps_List,		"relateOne",		{ sRelate, eRelate_Manual }		},
{ PSObjPropRelateMany,		true,	PSProps_Attribute,	PSProps_List,		"relateMany",		{ sRelate, eRelate_Manual }			},
{ PSObjPropCallback,		true,	PSProps_Attribute,	PSProps_String,		"callback",			{ NULL }						},
{ PSObjPropStartScript,		true,	PSProps_OneChild,	PSProps_String,		"StartScript",		{ NULL }						},
{ PSObjPropBodyScript,		true,	PSProps_OneChild,	PSProps_String,		"BodyScript",		{ NULL }						},
{ PSObjPropEndScript,		true,	PSProps_OneChild,	PSProps_String,		"EndScript",		{ NULL }						},
{ PSObjPropSRPCompatibility,true,	PSProps_Attribute,	PSProps_Boolean,	"SRPCompatibility",	{ NULL, 0, 0, 1 }				},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};


// ---------------------------------------------------------------------------
// SRDataSource								Constructor				  [public]
// ---------------------------------------------------------------------------

SRDataSource::SRDataSource (void)
	:	PSObject (eObject_DataSource),
		mSource (eDataSource_Fixed),
		mNumIterations (1),	//mbs 09012010	default to 1
		mMainTable (0),
		mRelateOne (eRelate_Manual),
		mRelateMany (eRelate_Manual),
		mSRPCompatibility (false),
		mCallBackID (0),
		mCurIteration (0),
		mReportWriter (NULL)
{
}


// ---------------------------------------------------------------------------
// ~SRDataSource							Destructor				  [public]
// ---------------------------------------------------------------------------

SRDataSource::~SRDataSource (void)
{
	{
		SRVarMap::const_iterator	vit;

		for (vit = mVariables.begin(); vit != mVariables.end(); vit++)
		{
			SRVarMap::value_type	value (*vit);
			delete value.second;
		}
		mVariables.clear();
	}

	{
		SRFldMap::const_iterator	fit;

		for (fit = mFields.begin(); fit != mFields.end(); fit++)
		{
			SRFldMap::value_type	value (*fit);
			delete value.second;
		}
		mFields.clear();
	}

	return;
}


// ---------------------------------------------------------------------------
// SetCallBackID												   [protected]
// ---------------------------------------------------------------------------

void
SRDataSource::SetCallBackID (void)
{
	mCallBackID = 0;
	if (not mCallBackName.IsEmpty())
	{
        CText name = mCallBackName;
		mCallBackID = PA_GetMethodID ((PA_Unichar *)name.c_str());
		if (mCallBackID == -1)
			mCallBackID = 0;
	}
}


// ---------------------------------------------------------------------------
// ParseDataSource													 [private]
// ---------------------------------------------------------------------------

void
SRDataSource::ParseDataSource ( XMLElement *inNode)
{
	const CXMLText	value = inNode->Attribute (FindPropertyByID (PSObjPropType, GetProperties())->name);
	if (!value.empty())
		assert (STR_EQUALS (value, "4D"));

	LoadXML (inNode);

	//mbs 05082010
	if (mSource != eDataSource_Fixed)
		mNumIterations = -1;

	return;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRDataSource::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropType:			outValue.SetXMLText (s4DKind [0]); break;
		case PSObjPropSource:		outValue.SetXMLText (sSource [mSource]); break;
		case PSObjPropIterations:	outValue.SetInteger (mNumIterations); break;
		case PSObjPropID:			outValue.SetInteger (mMainTable); break;
		case PSObjPropName:			outValue.SetText (mName); break;
		case PSObjPropRelateOne:	outValue.SetXMLText (sRelate [mRelateOne]); break;
		case PSObjPropRelateMany:	outValue.SetXMLText (sRelate [mRelateMany]); break;
		case PSObjPropCallback:		outValue.SetText (mCallBackName); break;
		case PSObjPropStartScript:	outValue.SetText (mStartScript); break;
		case PSObjPropBodyScript:	outValue.SetText (mBodyScript); break;
		case PSObjPropEndScript:	outValue.SetText (mEndScript); break;
		case PSObjPropSRPCompatibility:	outValue.SetBoolean (mSRPCompatibility); break;

		default:					return PSObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRDataSource::SetProperty (OSType id, RWValue &inValue)
{
	long	lVal;
	switch (id)
	{
		case PSObjPropType:			break;
		case PSObjPropSource:
			if ((lVal = SetListProperty (inValue, sSource)) >= 0)
			{
				mSource = EDataSource (lVal);
				if (mSource != eDataSource_Fixed)	//mbs 10012010	default to 1 --> need to clear it
					mNumIterations = -1;
				return true;
			}
			break;
		case PSObjPropIterations:	return SetIntegerProperty (inValue, mNumIterations, 1); break;
		case PSObjPropID:			return SetIntegerProperty (inValue, mMainTable, 1); break;
		case PSObjPropName:			return SetStringProperty (inValue, mName); break;
		case PSObjPropRelateOne:
			if ((lVal = SetListProperty (inValue, sRelate)) >= 0)
			{
				mRelateOne = ERelate (lVal);
				return true;
			}
			break;
		case PSObjPropRelateMany:
			if ((lVal = SetListProperty (inValue, sRelate)) >= 0)
			{
				mRelateMany = ERelate (lVal);
				return true;
			}
			break;
		case PSObjPropCallback:
			if (SetStringProperty (inValue, mCallBackName))
			{
				SetCallBackID();
				return true;
			}
			break;
		case PSObjPropStartScript:	return SetStringProperty (inValue, mStartScript); break;
		case PSObjPropBodyScript:	return SetStringProperty (inValue, mBodyScript); break;
		case PSObjPropEndScript:	return SetStringProperty (inValue, mEndScript); break;
		case PSObjPropSRPCompatibility:	return SetBooleanProperty (inValue, mSRPCompatibility);

		default:					return PSObject::SetProperty (id, inValue);
	}

	return false;
}


// ---------------------------------------------------------------------------
// ParseReport													   [protected]
// ---------------------------------------------------------------------------

void
SRDataSource::ParseReport (RWXmlNode inReport)
{
	if (inReport != NULL)
	{
        const XMLElement	*node = inReport->FirstChildElement ("DataSource");
		if (node != NULL)
		{
            const XMLElement	*elem = node->ToElement(); // return self, so we can keep the code

			if (elem != NULL)
				ParseDataSource (const_cast<XMLElement *>(elem));
		}
	}

	return;
}


#if 0
// ---------------------------------------------------------------------------
// GetTableHeadings													  [public]
// ---------------------------------------------------------------------------
// Return array of headings in a table

int
SRDataSource::GetTableHeadings (RWDataID inDataID, EHeadings inWhich, const SOpaqueCategoryItem *&outTable)
const
{
	outTable = NULL;
	return 0;
}


// ---------------------------------------------------------------------------
// GetTableHeadingData												  [public]
// ---------------------------------------------------------------------------
// Return text of heading in a table

const CText
SRDataSource::GetTableHeadingData (RWDataID inDataID, SOpaqueCategoryItem inHeading, int inItem, int &outSpan, int &outLevel)
const
{
	return NULL;
}


// ---------------------------------------------------------------------------
// GetTableTitle													  [public]
// ---------------------------------------------------------------------------
// Return title (topleft cell in a table with both top & left headings)

const CText
SRDataSource::GetTableTitle (RWDataID inDataID)
const
{
	return NULL;
}
#endif


// ---------------------------------------------------------------------------
// FormatVariable													  [public]
// ---------------------------------------------------------------------------
// Convert specified variable to a text representation

RWTextValue
SRDataSource::FormatVariable (const RWValue &inVar, const CText inFormat)
const
{
	return SRDataFormatter::FormatVariable (inVar, inFormat);
}



// ---------------------------------------------------------------------------
// GetNumberOfIterations											  [public]
// ---------------------------------------------------------------------------

long
SRDataSource::GetNumberOfIterations (void)
const
{
	return mNumIterations;
}



// ---------------------------------------------------------------------------
// GetCurrentIteration												  [public]
// ---------------------------------------------------------------------------

long
SRDataSource::GetCurrentIteration (void)
const
{
	return mCurIteration;
}



// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
SRDataSource::Reset (void)
{
	mCurIteration = 0;
	SetStdVariables();
	SetStdRecordNumber();

	RunScript (mStartScript, NULL, true);

	if (mNumIterations < 0)	// true if source != eDataSource_Fixed (fixed number of iterations)
	{
		mNumIterations = 0;
		if (mSource == eDataSource_Undefined)
			;
		else if (mSource == eDataSource_Table)
		{
			if (mMainTable > 0)
			{
				PA_UseAutomaticRelations (0, 0);
				mNumIterations = PA_RecordsInSelection (mMainTable);
			}
		}
		else if (!mName.IsEmpty() /* && (mSource == eDataSource_Variable || mSource == eDataSource_Array) */)
		{
			SR4DVariable	var (mName, SR4DVariable::SR4DVariable_Variable);
			var.Fetch (SR4DVariable::SR4DVariable_Variable, false);	//mbs 05082010	-2 instead of -1 - no need to fetch array element
			if (mSource == eDataSource_Array)	//mbs 05082010	fetch the array size!!!
				mNumIterations = var.GetSize();
			else
			{
				RWValue	val;
				var.GetValue (val);
				switch (val.GetKind())
				{
					case RWValue::eValue_Boolean:
					case RWValue::eValue_Integer:
						mNumIterations = val.GetInteger();
						break;

					case RWValue::eValue_Real:
						mNumIterations = long (val.GetReal());
						break;

					default:
						mNumIterations = 0;
						break;
				}
			}
		}
	}

	if (mNumIterations < 0)
		mNumIterations = 0;

	return;
}


// ---------------------------------------------------------------------------
// FetchNextRecord													  [public]
// ---------------------------------------------------------------------------

bool
SRDataSource::FetchNextRecord (void)
{
	//mbs 22062006	need to shunt the values after last record, too...
	if (mCurIteration > 0 && mCurIteration <= mNumIterations)
	{
//		long		iteration = (mCurIteration < mNumIterations ? mCurIteration : mNumIterations - 1);
		SR4DData	*dat;
		SRVarMap::const_iterator	vit;

		for (vit = mVariables.begin(); vit != mVariables.end(); vit++)
		{
			SRVarMap::value_type	vval (*vit);
			dat = vval.second;
			dat->Fetch (mCurIteration, false);
			dat->Shunt();
		}

		SRFldMap::const_iterator	fit;

		for (fit = mFields.begin(); fit != mFields.end(); fit++)
		{
			SRFldMap::value_type	fval (*fit);
			dat = fval.second;
			dat->Fetch (mCurIteration, false);
			dat->Shunt();
		}
	}

	bool	haveRecord = mCurIteration < mNumIterations;
	if (haveRecord)
	{
		mCurIteration++;
		if (mSource == eDataSource_Table)
		{
			PA_UseAutomaticRelations (mRelateOne == eRelate_Automatic, mRelateMany == eRelate_Automatic);
			PA_GotoSelectedRecord (mMainTable, mCurIteration);
			if (mRelateOne == eRelate_Manual)
				PA_RelateOne (mMainTable);
			if (mRelateMany == eRelate_Manual)
				PA_RelateMany (mMainTable);
		}
		SetStdRecordNumber();
		RunScript (mBodyScript, NULL, true);
	}
	/* else
		RunScript (mEndScript, NULL, false); */
	// pB moved outside of loop
	// end script here would be executed before footers 

	return haveRecord;
}

// ---------------------------------------------------------------------------
// Close														  [public]
// called after report is processed
// ---------------------------------------------------------------------------
//pB 20110907

void
SRDataSource::Close ()
{
	RunScript (mEndScript, NULL, false);
}

// ---------------------------------------------------------------------------
// CreateVariable													  [public]
// ---------------------------------------------------------------------------

SR4DData*
SRDataSource::CreateVariable (const CText inName, long inIndex)
{
	SR4DVariable				*var = NULL;
	SRVarNameKey				key ( inName, inIndex);
	SRVarMap::const_iterator	it = mVariables.find (key);
	if (it == mVariables.end())
	{
		var = new SR4DVariable (inName, inIndex);
		SRVarMap::value_type	value (key, var);
		mVariables.insert (value);
	}
	else
		var = it->second;

	return var;
}


// ---------------------------------------------------------------------------
// GetVariable														  [public]
// ---------------------------------------------------------------------------

bool
SRDataSource::GetVariable (const CText inName, long inIndex, RWValue &outVar, bool inUseOld)
{
	//mbs 14012010	we have to always create a variable - otherwise <%var%> will not function for non-report variables
#if	0
	bool						found = true;
	SRVarNameKey				key (SConstText (inName), inIndex);
	SRVarMap::const_iterator	it = mVariables.find (key);

	if (it == mVariables.end())
		found = false;

	if (found)
	{
		SRVarMap::value_type	value (*it);
		if (inUseOld)
			value.second->GetOldValue (outVar);
		else
		{
			value.second->Fetch (mCurIteration, false);
			value.second->GetValue (outVar);
		}
	}

	return found;
#else
	SR4DData	*var = CreateVariable (inName, inIndex);
	if (inUseOld)
		var->GetOldValue (outVar);
	else
	{
		var->Fetch (mCurIteration, false);
		var->GetValue (outVar);
	}
	return true;
#endif
}


// ---------------------------------------------------------------------------
// GetVariable														  [public]
// ---------------------------------------------------------------------------
//mbs 05082010

void
SRDataSource::GetVariable (const CText inName, RWValue &outVar)
{
	SR4DVariable	var (inName, SR4DVariable::SR4DVariable_Variable);
	var.Fetch (mCurIteration, false);
	var.DetachValue (outVar);
}


// ---------------------------------------------------------------------------
// CreateField														  [public]
// ---------------------------------------------------------------------------

SR4DData*
SRDataSource::CreateField (const CText inName)
{
	SR4DField					*fld = NULL;
//	SRFldMap::key_type			key (SConstText (inName));
	SRFldMap::const_iterator	it = mFields.find (inName);
	if (it == mFields.end())
	{
		long	table = 0, field = 0;
		int		scanned;
        scanned = sscanf (RWTextValue::UTF_16_to_UTF8(inName).c_str(), "[%ld]%ld", &table, &field);
		if (scanned == 0)
		{
			short	st = 0, sf = 0;
			PA_GetTableAndFieldNumbers (const_cast <PA_Unichar*> (inName.c_str()), &st, &sf);
			if (PA_GetLastError() != 0)
			{
				st = sf = 0;
				PA_UseVirtualStructure();
				PA_GetTableAndFieldNumbers (const_cast <PA_Unichar*> (inName.c_str()), &st, &sf);
				PA_UseRealStructure();
				PA_GetTrueFieldNumber( st, sf,  &st, &sf);
				
			}
			table = st;
			field = sf;
		}
		fld = new SR4DField (table, field);
		SRFldMap::value_type	value ( inName, fld);
		mFields.insert (value);
	}
	else
		fld = it->second;

	return fld;
}


// ---------------------------------------------------------------------------
// GetField															  [public]
// ---------------------------------------------------------------------------

bool
SRDataSource::GetField (const CText inName, RWValue &outVar, bool inUseOld)
{
#if 0
	bool						found = true;
	SRFldMap::const_iterator	it = mFields.find (SConstText (inName));

	if (it == mFields.end())
		found = false;

	if (found)
	{
		SRFldMap::value_type	value (*it);
		if (inUseOld)
			value.second->GetOldValue (outVar);
		else
		{
			value.second->Fetch (mCurIteration, false);
			value.second->GetValue (outVar);
		}
	}

	return found;
#else
	
	SR4DData	*var = CreateField (inName);
	if (inUseOld)
		var->GetOldValue (outVar);
	else
	{
		var->Fetch (mCurIteration, false);
		var->GetValue (outVar);
	}
	return true;
	
#endif
}


// ---------------------------------------------------------------------------
// Invalidate														  [public]
// ---------------------------------------------------------------------------

void
SRDataSource::Invalidate (void)
{
	{
		SRVarMap::const_iterator	vit;

		for (vit = mVariables.begin(); vit != mVariables.end(); vit++)
		{
			SRVarMap::value_type	value (*vit);
			value.second->Invalidate();
		}
	}

	{
		SRFldMap::const_iterator	fit;

		for (fit = mFields.begin(); fit != mFields.end(); fit++)
		{
			SRFldMap::value_type	value (*fit);
			value.second->Invalidate();
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// RunScript														  [public]
// ---------------------------------------------------------------------------

void
SRDataSource::RunScript (ExtendedExecute &inScript, PSObject *inObject, bool inAlwaysInvalidate)
{
	if (not inScript.IsEmpty())
	{
		SetStdObjectID (inObject);
		if (mCallBackID != 0)
		{
# if USE_CUSTOM_CALLBACK
			PA_Handle	values = PA_NewHandle (3 * sizeof (PA_Variable));	// $0, $1, $2
			if (values != NULL)
			{
				PA_Variable*	vars = (PA_Variable*) PA_LockHandle (values);
				vars [0].fType = eVK_Undefined;			// no retVal
				PA_Unistring	us = PA_CreateUnistring (inScript.Get());
				PA_SetStringVariable (&vars [1], &us);
				PA_SetLongintVariable (&vars [2], (long) inObject);
				PA_UnlockHandle (values);
				PA_ExecuteMethodByID_2 (mCallBackID, values, 1);
//				result = PA_GetLastError();
				vars = (PA_Variable*) PA_LockHandle (values);
				PA_ClearVariable (&vars [0]);
				PA_ClearVariable (&vars [1]);
				PA_ClearVariable (&vars [2]);
				PA_UnlockHandle (values);
				PA_DisposeHandle (values);
			}
# else
			PA_Unistring	us = PA_CreateUnistring (const_cast <CText> (inScript));
			PA_Variable		args[2];
			PA_SetStringVariable (&args [0], &us);
			PA_SetLongintVariable (&args [1], inObject);
//			PA_ExecuteMethod (&us);
			PA_ExecuteMethodByID (mCallBackID, &args, 2);
			PA_ClearVariable (&args [0]);
			PA_ClearVariable (&args [1]);
# endif
		}
		else
		{
			inScript.Execute();
		}
			
	}

	if (inAlwaysInvalidate || (inScript && *inScript))
		Invalidate();

	return;
}


// ---------------------------------------------------------------------------
// SetStdVariables												   [protected]
// ---------------------------------------------------------------------------
// RWReport
//		SRDate, SRTime, SRPage, SRArea

static	const	UTF16Char	sSRDate [] = {	'S', 'R', 'D', 'a', 't', 'e', 0	};
static	const	UTF16Char	sSRTime [] = {	'S', 'R', 'T', 'i', 'm', 'e', 0	};
static	const	UTF16Char	sSRPage [] = {	'S', 'R', 'P', 'a', 'g', 'e', 0	};
static	const	UTF16Char	sSRRecord [] = {'S', 'R', 'R', 'e', 'c', 'o', 'r', 'd', 0	};
static	const	UTF16Char	sSRArea [] = {	'S', 'R', 'A', 'r', 'e', 'a', 0	};
static	const	UTF16Char	sSRObject [] = {'S', 'R', 'O', 'b', 'j', 'e', 'c', 't', 'I', 'D', 0	};

//static	const	UTF16Char	sRWDate [] = {	'R', 'W', 'D', 'a', 't', 'e', 0	};
//static	const	UTF16Char	sRWTime [] = {	'R', 'W', 'T', 'i', 'm', 'e', 0	};
static	const	UTF16Char	sRWRecord [] = {	'R', 'W', 'R', 'e', 'c', 'o', 'r', 'd', 0	};
static	const	UTF16Char	sRWReport [] = {	'R', 'W', 'R', 'e', 'p', 'o', 'r', 't', 0	};
static	const	UTF16Char	sRWObject [] = {	'R', 'W', 'O', 'b', 'j', 'e', 'c', 't', 0	};

void
SRDataSource::SetStdVariables (void)
{
	PA_Variable	v;

	mPrintTime = time (0);

	if (mSRPCompatibility)
	{
		struct	tm	*lt = localtime (&mPrintTime);

		PA_SetDateVariable (&v, lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900);
		PA_SetVariable (const_cast <UTF16Char*> (sSRDate), v, true);
//		PA_SetVariable (const_cast <UTF16Char*> (sRWDate), v, true);

		PA_SetTimeVariable (&v, lt->tm_hour * 3600L + lt->tm_min * 60L + lt->tm_sec);
		PA_SetVariable (const_cast <UTF16Char*> (sSRTime), v, true);
//		PA_SetVariable (const_cast <UTF16Char*> (sRWTime), v, true);

		PA_SetLongintVariable (&v, 1);
		PA_SetVariable (const_cast <UTF16Char*> (sSRPage), v, true);

		PA_SetLongintVariable (&v, (long) mReportWriter);
		PA_SetVariable (const_cast <UTF16Char*> (sSRArea), v, true);
	}

	PA_SetLongintVariable (&v, (long) mReportWriter);
	PA_SetVariable (const_cast <UTF16Char*> (sRWReport), v, true);
}


// ---------------------------------------------------------------------------
// SetStdRecordNumber											   [protected]
// ---------------------------------------------------------------------------
// RWRecord
//		SRRecord

void
SRDataSource::SetStdRecordNumber (void)
{
	PA_Variable	v;
	mIteration = mCurIteration;

	PA_SetLongintVariable (&v, mCurIteration);
	PA_SetVariable (const_cast <UTF16Char*> (sRWRecord), v, true);
	if (mSRPCompatibility)
		PA_SetVariable (const_cast <UTF16Char*> (sSRRecord), v, true);
}


// ---------------------------------------------------------------------------
// SetStdObjectID												   [protected]
// ---------------------------------------------------------------------------
// RWObject
//		SRObjectID

void
SRDataSource::SetStdObjectID (PSObject *inObject)
{
	PA_Variable	v;

	PA_SetLongintVariable (&v, (long) inObject);
	PA_SetVariable (const_cast <UTF16Char*> (sRWObject), v, true);
	if (mSRPCompatibility)
		PA_SetVariable (const_cast <UTF16Char*> (sSRObject), v, true);
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SRDataSource::Write (FILE *fd)
const
{
	fprintf (fd, "<DataSource kind=\"4D\" iterations=\"%lu\" />\r\n", mNumIterations);

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
SRDataSource::Write ( XMLElement *inParent)
const
{
    XMLElement	elem ("DataSource");
	elem.SetAttribute ("kind", "4D");
	elem.SetAttribute ("iterations", mNumIterations);
	inParent->InsertEndChild (elem);

	return;
}


// ---------------------------------------------------------------------------
// WriteReportData													  [public]
// ---------------------------------------------------------------------------

void
SRDataSource::WriteReportData (FILE *fd)
const
{
	fprintf (fd, "<ReportData");
	if (mData.IsEmpty())
		fprintf (fd, " />\r\n");
	else
	{
		fprintf (fd, ">\r\n");
		mData.Write (fd);
		fprintf (fd, "</ReportData>\r\n");
	}

	return;
}


// ---------------------------------------------------------------------------
// WriteReportData													  [public]
// ---------------------------------------------------------------------------

void
SRDataSource::WriteReportData ( XMLElement *inParent)
const
{
    XMLElement	elem ("ReportData");
	mData.Write (inParent->InsertEndChild (elem)->ToElement());

	return;
}


// ---------------------------------------------------------------------------
// EmitPicture														  [public]
// ---------------------------------------------------------------------------

RWDataID
SRDataSource::EmitPicture (const RWValue &inVar)
{
//	RWValue	value;
//	COMPOSER::GetPictureFromRef (inVar, value);
//	value.SetBlob (inVar.GetBlobData(), inVar.GetBlobSize(), false);
//	return mData.AddObject (value);
	return mData.AddObject (inVar);
}



// ---------------------------------------------------------------------------
// EmitRepeating													  [public]
// ---------------------------------------------------------------------------

RWDataID
SRDataSource::EmitRepeating (SR4DData *inVar, bool inIsHorizontal)
{
	long		size = inVar->GetSize();
	RWDataID	dataID = mData.AddTableObject (1, inIsHorizontal);
	if (size)
	{
		RWDataProvider::tableData*	data = mData.GetTableObject (dataID);
		RWValue						value;
		for (long i = 1; i <= size; i++)
		{
			inVar->Invalidate();
			inVar->Fetch (i, true);
			inVar->GetValue (value);
			data->PutCellData (i, 1, value);
		}
	}

	return dataID;
}
