# include	"RWDataProvider.h"

# if	_4D_Package_
extern	"C"		void Yield4D (void);
# endif


RWDataProvider::RWDataProvider (void)
	:	mObjectID (0)
{
	return;
}


RWDataProvider::~RWDataProvider (void)
{
	{
		RWObjectDataMap::iterator	oit;

		for (oit = mObjectData.begin(); oit != mObjectData.end(); oit++)
		{
			RWObjectDataMap::value_type	&value (*oit);
			value.second.Free();
		}
	}

	{
		RWTableMap::iterator	tit;

		for (tit = mTables.begin(); tit != mTables.end(); tit++)
		{
			RWTableMap::value_type	&tvalue (*tit);
			tableData	*td = tvalue.second;
			delete td;
		}
	}

	return;
}


RWDataID
RWDataProvider::AddObject (const RWValue &inValue)
{
	RWDataID	id = ++mObjectID;

	RWObjectDataMap::value_type	data (id, inValue);
	mObjectData.insert (mObjectData.end(), data);

	return id;
}


RWDataID
RWDataProvider::AddTableObject (long inNumColumns, bool inTranspose)
{
	RWDataID	id = ++mObjectID;
	tableData	*td = new tableData (id, inNumColumns, inTranspose);
	mTables.insert (mTables.end(), RWTableMap::value_type (id, td));

	return id;
}


void
RWDataProvider::PutTableCellData (RWDataID inDataID, long inRow, long inColumn, const RWValue &inValue)
{
	tableData	*td = GetTableObject (inDataID);
	if (td != NULL)
		td->PutCellData (inRow, inColumn, inValue);

	return;
}


bool
RWDataProvider::IsEmpty (void)
const
{
	return mObjectData.size() == 0 && mTables.size() == 0;
}


bool
RWDataProvider::GetObject (RWDataID inDataID, RWValue &outValue)
const
{
	bool	found = false;
	RWObjectDataMap::const_iterator	it = mObjectData.find (inDataID);

	if (it != mObjectData.end())
	{
		found = true;
		outValue.Attach (it->second);
	}

	return found;
}


RWDataProvider::tableData*
RWDataProvider::GetTableObject (RWDataID inDataID)
const
{
	tableData	*td = NULL;
	RWTableMap::const_iterator	it = mTables.find (inDataID);

	if (it != mTables.end())
		td = it->second;

	return td;
}


long
RWDataProvider::GetTableRowCount (RWDataID inDataID)
const
{
	long				rows = 0;
	const	tableData	*td = GetTableObject (inDataID);

	if (td != NULL)
		rows = td->GetRowCount();

	return rows;
}


long
RWDataProvider::GetTableColumnCount (RWDataID inDataID)
const
{
	long			cols = 0;
	const tableData	*td = GetTableObject (inDataID);

	if (td != NULL)
		cols = td->GetColumnCount();

	return cols;
}


bool
RWDataProvider::GetTableCellData (RWDataID inDataID, long inRow, long inColumn, RWValue &outValue)
const
{
	bool		found = false;
	tableData	*td = GetTableObject (inDataID);

	if (td != NULL)
		found = td->GetCellData (inRow, inColumn, outValue);

	return found;
}

//mbs 06102006
bool
RWDataProvider::GetTableCellData (RWDataID inDataID, long inRow, long inColumn, RWValue &outValue, long &outStartRow, long &outNumRows)
const
{
	bool		found = false;
	tableData	*td = GetTableObject (inDataID);

	if (td != NULL)
		found = td->GetCellData (inRow, inColumn, outValue, outStartRow, outNumRows);

	return found;
}




RWDataProvider::tableData::tableData (RWDataID id, long inNumRows, long inNumColumns, bool inTranspose)
	:	fDataID (id),
		fNumRows (inNumRows),
		fNumCols (inNumColumns),
		fTranspose (inTranspose),
		fTableData (0)
{
	if (fNumCols < 0)		// "cols" missing in the XML
		fNumCols = 0;
	fTableData = new RWObjectDataMap [fNumCols];
	return;
}


RWDataProvider::tableData::tableData (RWDataID id, long inNumColumns, bool inTranspose)
	:	fDataID (id),
		fNumRows (0),
		fNumCols (inNumColumns),
		fTranspose (inTranspose),
		fTableData (0)
{
	fTableData = new RWObjectDataMap [inNumColumns];
	return;
}


RWDataProvider::tableData::~tableData (void)
{
	for (long i = 0; i < fNumCols; i++)
	{
		RWObjectDataMap	*col = &fTableData [i];
		RWObjectDataMap::iterator	it;
		for (it = col->begin(); it != col->end(); it++)
		{
			RWObjectDataMap::value_type	&value (*it);
			value.second.Free();
		}
	}

	delete [] fTableData;

	return;
}


long
RWDataProvider::tableData::GetRowCount (void)
const
{
	long	rows = fTranspose ? fNumCols : fNumRows;

	return rows;
}


long
RWDataProvider::tableData::GetColumnCount (void)
const
{
	long	cols = fTranspose ? fNumRows : fNumCols;

	return cols;
}


RWDataProvider::RWObjectDataMap*
RWDataProvider::tableData::GetColumn (long inColumn)
const
{
	RWDataProvider::RWObjectDataMap*	cd = NULL;

	if (inColumn > 0 && inColumn <= fNumCols)
		cd = & fTableData [inColumn - 1];

assert (cd != NULL);

	return cd;
}


void
RWDataProvider::tableData::PutCellData (long inRow, long inColumn, const RWValue &inValue)
{
	long			row = fTranspose ? inColumn : inRow;
	long			col = fTranspose ? inRow : inColumn;
	RWObjectDataMap	*cd = GetColumn (col);
	if (cd == NULL)
		return;

	if (fNumRows < row)
		fNumRows = row;

	bool	insert = true;
	if (row > 1 && !cd->empty())	// eliminate duplicate (repeating) values; "--end()" of an empty map was undefined
	{
/*	1) insertion is done sequentially
    2) map is always sorted
	==> we will just grab the last item...

		RWObjectDataMap::const_iterator	it = cd->lower_bound (row - 1);
		if (it == cd->end())
			it--;
		if (it != cd->end())
		{
			const RWValue	&var = it->second;
			if (var == inValue)
				insert = false;
		}
*/
		const RWValue	&var = (--(cd->end()))->second;
		if (var == inValue)
			insert = false;
	}

	if (insert)
	{
		RWObjectDataMap::value_type	data (row, inValue);
		cd->insert (cd->end(), data);
//WriteValue (stdout, RWDataID (row), inValue);
//fflush(stdout);
	}

	return;
}


bool
RWDataProvider::tableData::GetCellData (long inRow, long inColumn, RWValue &outValue)
const
{
	bool	found = false;
	long	row = fTranspose ? inColumn : inRow;
	long	col = fTranspose ? inRow : inColumn;

	if (row > 0 && row <= fNumRows && col > 0 && col <= fNumCols)
	{
		const RWObjectDataMap			*cd = GetColumn (col);
		if (cd->size() > 0)	//mbs 12022010	don't crash ;-)
		{
			RWObjectDataMap::const_iterator	it = cd->lower_bound (row);
			if (it == cd->end())
				it--;
			else if (it != cd->begin() && it->first > (RWDataID) row)
				it--;
//			RWObjectDataMap::const_iterator	next = it.next;

			if (it != cd->end())
			{
				found = true;
				outValue.Attach (it->second);
				// span is from row it.first to row next.first - 1
				// or to the end if next == cd->end()
			}
		}
	}

	return found;
}


//mbs 06102006
bool
RWDataProvider::tableData::GetCellData (long inRow, long inColumn, RWValue &outValue, long &outStartRow, long &outNumRows)
const
{
	bool	found = false;
	long	row = fTranspose ? inColumn : inRow;
	long	col = fTranspose ? inRow : inColumn;

	if (row > 0 && row <= fNumRows && col > 0 && col <= fNumCols)
	{
		const RWObjectDataMap	*cd = GetColumn (col);
		if (cd->size() > 0)	//mbs 12022010	don't crash ;-)
		{
			RWObjectDataMap::const_iterator	it = cd->lower_bound (row);
			if (it == cd->end())
				it--;
			else if (it != cd->begin() && it->first > (RWDataID) row)
				it--;

			if (it != cd->end())
			{
				found = true;
				outValue.Attach (it->second);
				outStartRow = it->first;
				it++;
				if (it == cd->end())
					outNumRows = fNumRows - outStartRow + 1;
				else
					outNumRows = it->first - outStartRow;
			}
		}
		else
		{
			outStartRow = 0;
			outNumRows = 0;
		}
	}

	return found;
}



static	const char16_t	*kValueTag = u"v";		// "Value"
static	const char16_t	*kIDTag = u"id";		// "id"
static	const char16_t	*kKindTag = u"k";		// "kind"
static	const char16_t	*kColumnTag = u"c";		// "Column"


// ---------------------------------------------------------------------------
// ReadDateParts													   [local]
// ---------------------------------------------------------------------------
// "a<sep>b<sep>c" as three integers

static	bool
ReadDateParts (RWStringView inText, char16_t inSeparator, long outParts[3])
{
	std::vector<RWString>	parts = RWStr::Split (RWStr::Trim (inText), inSeparator);
	if (parts.size() != 3)
		return false;
	for (int i = 0; i < 3; i++)
	{
		std::optional<long long>	value = RWStr::ToInteger (parts[i]);
		if (!value)
			return false;
		outParts[i] = (long) *value;
	}
	return true;
}


// ---------------------------------------------------------------------------
// ParseValue													   [protected]
// ---------------------------------------------------------------------------
// inNode is a "v" (Value) element

void
RWDataProvider::ParseValue (RWXmlNode inNode, RWDataID &outID, RWValue &outVar)
{
	outVar.Free();

	if (inNode.HasAttr (kIDTag))
		outID = (RWDataID) inNode.AttrInt (kIDTag, 0);

	long	kindValue = (long) inNode.AttrInt (kKindTag, RWValue::eValue_Undefined);
	if (kindValue == 4)						// eValue_XMLText (UTF-8 text) in older data
		kindValue = RWValue::eValue_Text;
	RWValue::EValue_Kind	kind = RWValue::EValue_Kind (kindValue);

	switch (kind)
	{
//		default:
		case RWValue::eValue_Undefined:
			break;

		case RWValue::eValue_PictRefScreen:	// invalid case!
		case RWValue::eValue_PictRefPrint:	// invalid case!
			// ••• TODO •••	what to do?!?
			break;

		case RWValue::eValue_BLOB:
		case RWValue::eValue_PicturePICT:
		case RWValue::eValue_PicturePDF:
		case RWValue::eValue_PictureJPG:
		case RWValue::eValue_PicturePNG:
		case RWValue::eValue_PictureTIFF:
		case RWValue::eValue_PictureEMF:
		{
			//	has Attribute ("format", "PICT");
			//	has Attribute ("encoding", "base64");
			SBlob	data;
			data.Init();
			RWTools::ReadData (inNode, data);
			outVar.SetPicture (kind, data, true);
			break;
		}

		case RWValue::eValue_Text:
			outVar.SetText (RWTools::ParseIntoText (inNode));
			break;

		case RWValue::eValue_Boolean:
		case RWValue::eValue_Integer:
		case RWValue::eValue_Real:
		case RWValue::eValue_DateTime:
		case RWValue::eValue_Date:
		case RWValue::eValue_Time:
		{
			RWString	text = RWTools::ParseIntoText (inNode);
			long		lVal = 0;
			double		dVal = 0;
			long		parts[3];

			switch (kind)
			{
				case RWValue::eValue_Boolean:
					lVal = RWStr::ToInteger (text).value_or (0) != 0;
					break;

				case RWValue::eValue_Integer:
				case RWValue::eValue_DateTime:
					lVal = (long) RWStr::ToInteger (text).value_or (0);
					break;

				case RWValue::eValue_Real:
					dVal = RWStr::ToDouble (text).value_or (0);
					break;

				case RWValue::eValue_Date:
					// day: 0 - 31 ==> 5 bits
					// month: 0 - 12 ==> 4 bits
					// day | (month << 5) | (year << 9)
					if (ReadDateParts (text, u'-', parts))
						lVal = (parts[0] << 9) | (parts[1] << 5) | parts[2];
					break;

				case RWValue::eValue_Time:
					if (ReadDateParts (text, u':', parts))
						lVal = (parts[0] * 3600L) + (parts[1] * 60L) + parts[2];
					break;

				default:	// to shut up compiler
					break;
			}

			if (kind == RWValue::eValue_Real)
				outVar.SetReal (dVal);
			else
				outVar.SetInteger (lVal, kind);
			break;
		}

		default:
			break;
	}

# if	_4D_Package_
	Yield4D();
# endif

	return;
}


// ---------------------------------------------------------------------------
// WriteValue													   [protected]
// ---------------------------------------------------------------------------
// appends a "v" (Value) element

void
RWDataProvider::WriteValue (RWXmlNode inParent, RWDataID inID, const RWValue &inVar)
{
	RWValue::EValue_Kind	kind = inVar.GetKind();
	RWString				text;

	RWXmlNode	me = inParent.Append (kValueTag);
	me.SetAttrInt (kIDTag, (long long) inID);
	me.SetAttrInt (kKindTag, kind);

	switch (kind)
	{
		case RWValue::eValue_Undefined:
			break;

		case RWValue::eValue_Boolean:
		case RWValue::eValue_Integer:
			text = RWStr::Format ("%ld", inVar.GetInteger());
			break;

		case RWValue::eValue_Real:
			text = RWStr::Format ("%.15lg", inVar.GetReal());
			break;

		case RWValue::eValue_Text:
			text = inVar.GetText();
			break;

		case RWValue::eValue_DateTime:
			text = RWStr::Format ("%lu", (unsigned long) inVar.GetInteger());
			break;

		case RWValue::eValue_Date:
			// day: 0 - 31 ==> 5 bits
			// month: 0 - 12 ==> 4 bits
			// day | (month << 5) | (year << 9)
			text = RWStr::Format ("%04ld-%02ld-%02ld", inVar.GetInteger() >> 9, (inVar.GetInteger() >> 5) & 0xF, inVar.GetInteger() & 0x1F);
			break;

		case RWValue::eValue_Time:
			text = RWStr::Format ("%02ld:%02ld:%02ld", inVar.GetInteger() / 3600, inVar.GetInteger() / 60 % 60, inVar.GetInteger() % 60);
			break;

		case RWValue::eValue_PictRefScreen:	// invalid case!
		case RWValue::eValue_PictRefPrint:	// invalid case!
			// ••• TODO •••	what to do?!?
			text = RWStr::Format ("%ld", (long) (intptr_t) inVar.GetPictureRef());
			break;

		case RWValue::eValue_BLOB:
		case RWValue::eValue_PicturePICT:
		case RWValue::eValue_PicturePDF:
		case RWValue::eValue_PictureJPG:
		case RWValue::eValue_PicturePNG:
		case RWValue::eValue_PictureTIFF:
		case RWValue::eValue_PictureEMF:
			if (inVar.GetBlobSize() != 0)	// should be always true...
			{
				me.SetAttr (u"format", RWStr::FromASCII (RWValue::GetPictFormats() [kind - RWValue::eValue_BLOB]));
				me.SetAttr (u"encoding", u"base64");
				RWTools::WriteData (me, inVar.GetBlob());
			}
			break;
	}

	if (!text.empty())
		RWTools::WriteText (me, text);

# if	_4D_Package_
	Yield4D();
# endif

	return;
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------
// inParent is the "ReportData" element of a report

void
RWDataProvider::Parse (RWXmlNode inParent)
{
	if (!IsEmpty() || !inParent)
		return;

	for (RWXmlNode elem : inParent.Child (u"Objects").Children (kValueTag))
	{
		RWDataID	id = 0;
		RWValue		value;

		ParseValue (elem, id, value);
		// AddObject(), but uses provided id, not mObjectID
		mObjectID++;
		mObjectData.insert (mObjectData.end(), RWObjectDataMap::value_type (id, value));
	}

	for (RWXmlNode table : inParent.Child (u"Tables").Children (u"Table"))
	{
		RWDataID	id = (RWDataID) table.AttrInt (kIDTag, 0);
		long		rows = (long) table.AttrInt (u"rows", -1);	// for TableData construction - repeating values are not preserved...
		long		cols = (long) table.AttrInt (u"cols", -1);	// for TableData construction
		bool		transpose = table.AttrInt (u"transpose", 0) != 0;

		// AddTableObject(), but uses provided id, not mObjectID
		tableData	*td = new tableData (id, rows, cols, transpose);
		mTables.insert (mTables.end(), RWTableMap::value_type (id, td));

		long	column = 0;
		for (RWXmlNode columnElem : table.Children (kColumnTag))
		{
			column++;
			for (RWXmlNode row : columnElem.Children (kValueTag))
			{
				RWDataID	rowID = 0;
				RWValue		value;

				ParseValue (row, rowID, value);
				// PutTableCellData(), but uses provided td
				td->PutCellData (rowID, column, value);
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

void
RWDataProvider::Write (RWXmlNode inParent)
const
{
	if (mObjectData.size() != 0)
	{
		RWXmlNode	objects = inParent.Append (u"Objects");
		objects.SetAttrInt (u"size", (long long) mObjectData.size());

		for (const auto &object : mObjectData)
			WriteValue (objects, object.first, object.second);
	}

	if (mTables.size() != 0)
	{
		RWXmlNode	tables = inParent.Append (u"Tables");
		tables.SetAttrInt (u"size", (long long) mTables.size());

		for (const auto &entry : mTables)
		{
			const tableData	*td = entry.second;
			RWXmlNode		table = tables.Append (u"Table");
			table.SetAttrInt (kIDTag, (long long) td->fDataID);
			if (td->fTranspose)
				table.SetAttr (u"transpose", u"1");
			table.SetAttrInt (u"rows", td->fNumRows);
			table.SetAttrInt (u"cols", td->fNumCols);

			const RWObjectDataMap	*c = td->fTableData;
			for (long coln = 1; coln <= td->fNumCols; coln++, c++)
			{
				RWXmlNode	column = table.Append (kColumnTag);
				column.SetAttrInt (kIDTag, coln);
				column.SetAttrInt (u"size", (long long) c->size());

				for (const auto &cell : *c)
					WriteValue (column, cell.first, cell.second);
			}
		}
	}

	return;
}
