# include	"RWDataSourceProvider.h"


// ---------------------------------------------------------------------------
// RWDataSourceProvider						Default Constructor		  [public]
// ---------------------------------------------------------------------------

RWDataSourceProvider::RWDataSourceProvider (void)
{
	return;
}


// ---------------------------------------------------------------------------
// ~RWDataSourceProvider					Destructor				  [public]
// ---------------------------------------------------------------------------

RWDataSourceProvider::~RWDataSourceProvider (void)
{
	return;
}


// ---------------------------------------------------------------------------
// ParseReport														  [public]
// ---------------------------------------------------------------------------

void
RWDataSourceProvider::ParseReport (const XMLElement *inReport)
{
	if (inReport != NULL)
	{
		XMLElement const	*node = inReport->FirstChildElement ("ReportData");
		if (node != NULL)
		{
//            XMLElement	*elem = node->ToElement();
//
//			if (elem != NULL)
				mData.Parse (node);
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// GetData															  [public]
// ---------------------------------------------------------------------------

bool
RWDataSourceProvider::GetData (RWDataID inDataID, RWValue &outVar)
const
{
	return mData.GetObject (inDataID, outVar);
}



// ---------------------------------------------------------------------------
// GetTableRowCount													  [public]
// ---------------------------------------------------------------------------
// Return number of data rows in a table

int
RWDataSourceProvider::GetTableRowCount (RWDataID inDataID)
const
{
	return mData.GetTableRowCount (inDataID);
}


// ---------------------------------------------------------------------------
// GetTableColumnCount												  [public]
// ---------------------------------------------------------------------------
// Return number of data columns in a table

int
RWDataSourceProvider::GetTableColumnCount (RWDataID inDataID)
const
{
	return  mData.GetTableColumnCount (inDataID);
}


/*
// ---------------------------------------------------------------------------
// GetTableCellData													  [public]
// ---------------------------------------------------------------------------
// Return text for specified cell in a table

const CText
RWDataSourceProvider::GetTableCellData (RWDataID inDataID, int inRow, int inColumn)
const
{
	return NULL;
}
*/


// ---------------------------------------------------------------------------
// GetTableCellData													  [public]
// ---------------------------------------------------------------------------
// Return value for specified cell in a table

bool
RWDataSourceProvider::GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue)
const
{
	return mData.GetTableCellData (inDataID, inRow, inColumn, outValue);
}

//mbs 06102006
bool
RWDataSourceProvider::GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue, int &outStartRow, int &outNumRows)
const
{
	long	startRow, numRows;
	bool	found = mData.GetTableCellData (inDataID, inRow, inColumn, outValue, startRow, numRows);
	outStartRow = startRow;
	outNumRows = numRows;
	return found;
}


// ---------------------------------------------------------------------------
// GetDataProvider													  [public]
// ---------------------------------------------------------------------------

RWDataProvider&
RWDataSourceProvider::GetDataProvider (void)
{
	return mData;
}
