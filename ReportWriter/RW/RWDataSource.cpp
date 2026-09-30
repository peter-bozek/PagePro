# include	"RWDataSource.h"


// ---------------------------------------------------------------------------
// RWDataSource								Default Constructor		  [public]
// ---------------------------------------------------------------------------

RWDataSource::RWDataSource (void)
//	:	mUseTemp (false)
	:	mPrintTime (0),
		mIteration (0)
{
	mPrintTime = time (0);
	return;
}


// ---------------------------------------------------------------------------
// ~RWDataSource							Destructor				  [public]
// ---------------------------------------------------------------------------

RWDataSource::~RWDataSource (void)
{
	return;
}


// ---------------------------------------------------------------------------
// ParseReport														  [public]
// ---------------------------------------------------------------------------

void
RWDataSource::ParseReport (RWXmlNode inReport)
{
	return;
}


#if 0

// ---------------------------------------------------------------------------
// IsDynamic														  [public]
// ---------------------------------------------------------------------------

bool
RWDataSource::IsDynamic (void)
const
{
	return false;
}


// ---------------------------------------------------------------------------
// GetNumberOfIterations											  [public]
// ---------------------------------------------------------------------------

long
RWDataSource::GetNumberOfIterations (void)
const
{
	return 0;
}


// ---------------------------------------------------------------------------
// GetCurrentIteration												  [public]
// ---------------------------------------------------------------------------

long
RWDataSource::GetCurrentIteration (void)
const
{
	return 0;
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
RWDataSource::Reset (void)
{
	mVariables.clear();
	mTempVariables.clear();
	mUseTemp = false;

	return;
}


// ---------------------------------------------------------------------------
// Push																  [public]
// ---------------------------------------------------------------------------

void
RWDataSource::Push (void)
{
	mUseTemp = true;

	return;
}


// ---------------------------------------------------------------------------
// Pop																  [public]
// ---------------------------------------------------------------------------

void
RWDataSource::Pop (void)
{
	mTempVariables.clear();
	mUseTemp = false;

	return;
}


// ---------------------------------------------------------------------------
// FetchNextRecord													  [public]
// ---------------------------------------------------------------------------

bool
RWDataSource::FetchNextRecord (void)
{
	return false;
}
#endif


// ---------------------------------------------------------------------------
// CreateVariable													  [public]
// ---------------------------------------------------------------------------

void
RWDataSource::CreateVariable (const CText inName, RWValue *inVar)
{
	SetVariable (inName, inVar);

	return;
}


// ---------------------------------------------------------------------------
// SetVariable														  [public]
// ---------------------------------------------------------------------------

void
RWDataSource::SetVariable (const CText inName, RWValue *inVar)
{
	RWVarMap::key_type		key (inName);
	RWVarMap::value_type	value (key, inVar);
	RWVarMap::iterator		it;

/*
	if (mUseTemp)
	{
		it = mTempVariables.find (key);
		if (it == mTempVariables.end())
			mTempVariables.insert (value);
		else
			it->second = value.second;
	}
	else
*/
	{
		it = mVariables.find (key);
		if (it == mVariables.end())
			mVariables.insert (value);
		else
			it->second = value.second;
	}

	return;
}


// ---------------------------------------------------------------------------
// GetStdVariable													  [public]
// ---------------------------------------------------------------------------

bool
RWDataSource::GetStdVariable (int inVar, RWValue &outVar)
const
{
	if (inVar == RW_VarRWDate || inVar == RW_VarRWTime)
	{
		struct	tm	*tm = localtime (&mPrintTime);
		if (inVar == RW_VarRWDate)
			outVar.SetInteger (tm->tm_mday + ((tm->tm_mon + 1) << 5) + ((tm->tm_year + 1900L) << 9), RWValue::eValue_Date);
		else
			outVar.SetInteger (tm->tm_sec + tm->tm_min * 60L + tm->tm_hour * 3600L, RWValue::eValue_Time);
	}
	else if (inVar == RW_VarRWRecord)
		outVar.SetInteger (mIteration);
	else
		return false;
	return true;
}
	

// ---------------------------------------------------------------------------
// GetVariable														  [public]
// ---------------------------------------------------------------------------

bool
RWDataSource::GetVariable (const CText inName, RWValue &outVar)
const
{
	bool	found = false;
	RWVarMap::key_type			key (inName);
	RWVarMap::const_iterator	it;

/*
	if (mUseTemp)
	{
		it = mTempVariables.find (key);
		if (it == mTempVariables.end())
			it = mVariables.find (key);
	}
	else
*/
		it = mVariables.find (key);

	if (it == mVariables.end())
		found = false;
	else
	{
		found = true;
		RWVarMap::value_type	value (*it);
		if (value.second)
			outVar.Attach (*value.second);
	}
	return found;
}


// ---------------------------------------------------------------------------
// GetData															  [public]
// ---------------------------------------------------------------------------

bool
RWDataSource::GetData (RWDataID inDataID, RWValue &outVar)
const
{
	return false;
}



// ---------------------------------------------------------------------------
// GetTableRowCount													  [public]
// ---------------------------------------------------------------------------
// Return number of data rows in a table

int
RWDataSource::GetTableRowCount (RWDataID inDataID)
const
{
	return 0;
}


// ---------------------------------------------------------------------------
// GetTableColumnCount												  [public]
// ---------------------------------------------------------------------------
// Return number of data columns in a table

int
RWDataSource::GetTableColumnCount (RWDataID inDataID)
const
{
	return 0;
}


/*
// ---------------------------------------------------------------------------
// GetTableCellData													  [public]
// ---------------------------------------------------------------------------
// Return text for specified cell in a table

const CText
RWDataSource::GetTableCellData (RWDataID inDataID, int inRow, int inColumn)
const
{
	return NULL;
}
*/


// ---------------------------------------------------------------------------
// GetTableCellData													  [public]
// ---------------------------------------------------------------------------
// Return text for specified cell in a table

bool
RWDataSource::GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue)
const
{
	return false;
}

//mbs 06102006
bool
RWDataSource::GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue, int &outStartRow, int &outNumRows)
const
{
	return false;
}


// ---------------------------------------------------------------------------
// GetTableHeadings													  [public]
// ---------------------------------------------------------------------------
// Return array of headings in a table

int
RWDataSource::GetTableHeadings (RWDataID inDataID, EHeadings inWhich, const SOpaqueCategoryItem *&outTable)
const
{
	outTable = NULL;
	return 0;
}


// ---------------------------------------------------------------------------
// GetTableHeadingData												  [public]
// ---------------------------------------------------------------------------
// Return text of heading in a table

RWTextValue
RWDataSource::GetTableHeadingData (RWDataID inDataID, SOpaqueCategoryItem inHeading, int inItem, int &outSpan, int &outLevel)
const
{
	RWTextValue	v;
	return v;
}


// ---------------------------------------------------------------------------
// GetTableTitle													  [public]
// ---------------------------------------------------------------------------
// Return title (topleft cell in a table with both top & left headings)

RWTextValue
RWDataSource::GetTableTitle (RWDataID inDataID)
const
{
	RWTextValue	v;
	return v;
}
