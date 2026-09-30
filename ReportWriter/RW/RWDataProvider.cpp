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
	fTableData = new RWObjectDataMap [inNumColumns];
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

	if (fNumRows < row)
		fNumRows = row;

	bool	insert = true;
	if (row > 1)	// eliminate duplicate (repeating) values
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



static	const char	*sValueTag = "v";	// "Value"
static	const char	*sIDTag = "id";		// "id"
static	const char	*sKindTag = "k";	// "kind"
static	const char	*sColumnTag = "c";	// "Column"


// inNode should point to "Value" element
void
RWDataProvider::ParseValue (XMLElement const *inNode, RWDataID &outID, RWValue &outVar)
{
	outVar.Free();

	RWValue::EValue_Kind	    kind = RWValue::eValue_Undefined;
	const XMLAttribute			*attrib;

	for ( attrib = inNode->FirstAttribute(); attrib; attrib = attrib->Next() )
	{
		const CXMLText	name(attrib->Name());
		const CXMLText	value = attrib->Value();

		if (name.compare( sIDTag) == 0)
		{
			outID = 0;
			sscanf (value.c_str(), "%lu", &outID);
		}
		else if (name.compare( sKindTag) == 0)
		{
			kind = RWValue::EValue_Kind (std::stol(value));
		}
	}

	switch (kind)
	{
//		default:
		case RWValue::eValue_Undefined:
			break;

		case RWValue::eValue_PictRefScreen:	// invalid case!
		case RWValue::eValue_PictRefPrint:	// invalid case!
			// еее TODO еее	what to do?!?
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

		case RWValue::eValue_Boolean:
		case RWValue::eValue_Integer:
		case RWValue::eValue_Real:
		case RWValue::eValue_XMLText:
		case RWValue::eValue_Text:
		case RWValue::eValue_DateTime:
		case RWValue::eValue_Date:
		case RWValue::eValue_Time:
		{
			RWTextValue	text = RWTools::ParseIntoText (inNode);
			if (kind == RWValue::eValue_Text)
			{
				outVar.SetText (text.Detach(), true);
			}
			else
			{
				long		lVal = 0;
				double		dVal = 0;
				CXMLText	u8text = text.ToXML();
				text.Free();
				if (!u8text.empty())
				{
					if (kind == RWValue::eValue_XMLText)
					{
						outVar.SetXMLText (u8text, true);
						break;	// to not free u8text
					}

					switch (kind)
					{
						case RWValue::eValue_Boolean:
						case RWValue::eValue_Integer:
//						case RWValue::eValue_PictureRef:	// invalid case!
							sscanf (u8text.c_str(), "%ld", &lVal);
							if (kind == RWValue::eValue_Boolean)
								lVal = (lVal != 0);
							break;

						case RWValue::eValue_Real:
							sscanf (u8text.c_str(), "%lg", &dVal);
							break;

						case RWValue::eValue_DateTime:
							sscanf (u8text.c_str(), "%lu", &lVal);
							break;

						case RWValue::eValue_Date:
						{
							// day: 0 - 31 ==> 5 bits
							// month: 0 - 12 ==> 4 bits
							// day | (month << 5) | (year << 9)
							int	d, m, y;
							if (sscanf (u8text.c_str(), "%d-%d-%d", &y, &m, &d) == 3)
								lVal = (y << 9) | (m << 5) | d;
							break;
						}

						case RWValue::eValue_Time:
						{
							int	h, m, s;
							if (sscanf (u8text.c_str(), "%d:%d:%d", &h, &m, &s) == 3)
								lVal = (h * 3600L) + (m * 60L) + s;
							break;
						}

						default:	// to shut up compiler
							break;
					}
					RWTextValue::FreeXML (u8text);
				}

				if (kind == RWValue::eValue_Real)
					outVar.SetReal (dVal);
				else
					outVar.SetInteger (lVal, kind);
			}
			break;
		}
	}

# if	_4D_Package_
	Yield4D();
# endif

	return;
}


void
RWDataProvider::WriteValue (FILE *fd, RWDataID inID, const RWValue &inVar)
{
	char		buf [64];
	char		*result = NULL;
	CText   	text;
	long		size = 0;
	RWValue::EValue_Kind	kind = inVar.GetKind();
	fprintf (fd, "<%s %s=\"%lu\" %s=\"%d\"", sValueTag, sIDTag, inID, sKindTag, kind);

	switch (kind)
	{
		case RWValue::eValue_Undefined:
			break;

		case RWValue::eValue_Boolean:
		case RWValue::eValue_Integer:
			snprintf (buf, sizeof (buf), "%ld", inVar.GetInteger());
			result = buf;
			break;

		case RWValue::eValue_Real:
			snprintf (buf, sizeof (buf), "%.15lg", inVar.GetReal());
			result = buf;
			break;

		case RWValue::eValue_XMLText:
			result = const_cast <char*> (inVar.GetXMLText().c_str());
			break;
			
		case RWValue::eValue_Text:
			text = inVar.GetText();
			break;

		case RWValue::eValue_DateTime:
			snprintf (buf, sizeof (buf), "%lu", inVar.GetInteger());
			result = buf;
			break;

		case RWValue::eValue_Date:
			// day: 0 - 31 ==> 5 bits
			// month: 0 - 12 ==> 4 bits
			// day | (month << 5) | (year << 9)
			snprintf (buf, sizeof (buf), "%04ld-%02ld-%02ld", inVar.GetInteger() >> 9, (inVar.GetInteger() >> 5) & 0xF, inVar.GetInteger() & 0x1F);
			result = buf;
			break;

		case RWValue::eValue_Time:
			snprintf (buf, sizeof (buf), "%02ld:%02ld:%02ld", inVar.GetInteger() / 3600, inVar.GetInteger() / 60 % 60, inVar.GetInteger() % 60);
			result = buf;
			break;

		case RWValue::eValue_PictRefScreen:	// invalid case!
		case RWValue::eValue_PictRefPrint:	// invalid case!
			// еее TODO еее	what to do?!?
			snprintf (buf, sizeof (buf), "%ld", inVar.GetInteger());
			result = buf;
			break;
			
		case RWValue::eValue_BLOB:
		case RWValue::eValue_PicturePICT:
		case RWValue::eValue_PicturePDF:
		case RWValue::eValue_PictureJPG:
		case RWValue::eValue_PicturePNG:
		case RWValue::eValue_PictureTIFF:
		case RWValue::eValue_PictureEMF:
			result = reinterpret_cast <char*> (inVar.GetBlobData());
			size = inVar.GetBlobSize();
			break;

//		default:
//			break;
	}

	if (result != NULL || !text.empty())
	{
		if (kind >= RWValue::eValue_BLOB)
		{
			if (size == 0)	// should not occur...
				fprintf (fd, " />");
			else
			{
				fprintf (fd, " format=\"%s\" encoding=\"base64\">\r\n", RWValue::GetPictFormats() [kind - RWValue::eValue_BLOB]);
				RWTools::WriteData (fd, inVar.GetBlob());
				fprintf (fd, "</%s>\r\n", sValueTag);
			}
		}
		else	// text
		{
			fprintf (fd, ">");
			if (!text.empty())
				RWTools::WriteText (fd, text);
			else
				RWTools::WriteText (fd, result);
			fprintf (fd, "</%s>\r\n", sValueTag);
		}
	}
	else
		fprintf (fd, " />\r\n");

# if	_4D_Package_
	Yield4D();
# endif

	return;
}


void
RWDataProvider::WriteValue (XMLElement *inParent, RWDataID inID, const RWValue &inVar)
{
	char			buf [64];
	char			*result = NULL;
    CText		    text;
	long			size = 0;
    
	RWValue::EValue_Kind	kind = inVar.GetKind();
    
    XMLElement *	   me = inParent->InsertNewChildElement (sValueTag);
	me->SetAttribute (sIDTag, (unsigned int) inID);
	me->SetAttribute (sKindTag, kind);
    inParent = me; // inParent->InsertEndChild (me)->ToElement();

	switch (kind)
	{
		case RWValue::eValue_Undefined:
			break;

		case RWValue::eValue_Boolean:
		case RWValue::eValue_Integer:
			snprintf (buf, sizeof (buf), "%ld", inVar.GetInteger());
			result = buf;
			break;

		case RWValue::eValue_Real:
			snprintf (buf, sizeof (buf), "%.15lg", inVar.GetReal());
			result = buf;
			break;

		case RWValue::eValue_XMLText:
			result = const_cast <char*> (inVar.GetXMLText().c_str());
			break;
			
		case RWValue::eValue_Text:
			text = (CText) inVar.GetText();
			break;

		case RWValue::eValue_DateTime:
			snprintf (buf, sizeof (buf), "%lu", inVar.GetInteger());
			result = buf;
			break;

		case RWValue::eValue_Date:
			// day: 0 - 31 ==> 5 bits
			// month: 0 - 12 ==> 4 bits
			// day | (month << 5) | (year << 9)
			snprintf (buf, sizeof (buf), "%04ld-%02ld-%02ld", inVar.GetInteger() >> 9, (inVar.GetInteger() >> 5) & 0xF, inVar.GetInteger() & 0x1F);
			result = buf;
			break;

		case RWValue::eValue_Time:
			snprintf (buf, sizeof (buf), "%02ld:%02ld:%02ld", inVar.GetInteger() / 3600, inVar.GetInteger() / 60 % 60, inVar.GetInteger() % 60);
			result = buf;
			break;

		case RWValue::eValue_PictRefScreen:	// invalid case!
		case RWValue::eValue_PictRefPrint:	// invalid case!
			// еее TODO еее	what to do?!?
			snprintf (buf, sizeof (buf), "%ld", inVar.GetInteger());
			result = buf;
			break;

		case RWValue::eValue_BLOB:
		case RWValue::eValue_PicturePICT:
		case RWValue::eValue_PicturePDF:
		case RWValue::eValue_PictureJPG:
		case RWValue::eValue_PicturePNG:
		case RWValue::eValue_PictureTIFF:
		case RWValue::eValue_PictureEMF:
			result = reinterpret_cast <char*> (inVar.GetBlobData());
			size = inVar.GetBlobSize();
			break;

//		default:
//			break;
	}

	if (result != NULL || text.empty())
	{
		if (kind >= RWValue::eValue_BLOB)
		{
			if (size != 0)	// should be always true...
			{
				inParent->SetAttribute ("format", RWValue::GetPictFormats() [kind - RWValue::eValue_BLOB]);
				inParent->SetAttribute ("encoding", "base64");
				RWTools::WriteData (inParent, inVar.GetBlob());
			}
		}
		else	// text
		{
			if (text.empty())
				RWTools::WriteText (inParent, text);
			else
				RWTools::WriteText (inParent, result);
		}
	}

# if	_4D_Package_
	Yield4D();
# endif

	return;
}


// inNode should point to "ReportData" element of a report

void
RWDataProvider::Parse (XMLElement const *inParent)
{
	if (IsEmpty() && inParent != NULL)
	{
        XMLElement	const	*firstChild = inParent->FirstChildElement ("Objects");
        XMLNode             *node;
        XMLElement	 const  *elem;
		RWDataID		    id;
		RWValue			    value;

		if (firstChild != NULL)
            elem = firstChild->FirstChildElement (sValueTag);
		if (elem != NULL)
		{
			elem = elem->ToElement();
			if (elem != NULL)
			{
				for ( ; node != NULL; node = node->NextSibling())
				{
					elem = node->ToElement();
					if (elem == NULL)
						continue;
					if (strcasecmp (elem->Value(), sValueTag) == 0)
					{
						id = 0;
						value.Free();

						ParseValue (elem, id, value);
						// AddObject(), but uses provided id, not mObjectID
						mObjectID++;
						RWObjectDataMap::value_type	data (id, value);
						mObjectData.insert (mObjectData.end(), data);
					}
				}
			}
		}


        firstChild = inParent->FirstChildElement ("Tables");
		if (firstChild != NULL)
            elem = firstChild->FirstChildElement ("Table");
		if (elem != NULL)
		{
			elem = elem->ToElement();
			if (elem != NULL)
			{
				for ( ; node != NULL; node = node->NextSibling())
				{
					elem = node->ToElement();
					if (elem == NULL)
						continue;
					if (strcasecmp (elem->Value(), "Table") != 0)
						continue;

					const XMLAttribute	*attrib;
					id = 0;
					long	rows = -1;	// for TableData construction - repeating values are not preserved...
					long	cols = -1;	// for TableData construction
					bool	transpose = false;

					for ( attrib = elem->FirstAttribute(); attrib; attrib = attrib->Next() )
					{
						const char *	name = attrib->Name();
						const char *	avalue = attrib->Value();

						if (strcasecmp (name, sIDTag) == 0)
						{
							id = 0;
							sscanf (avalue, "%lu", &id);
						}
						else if (strcasecmp (name, "rows") == 0)
						{
							rows = atol (avalue);
						}
						else if (strcasecmp (name, "cols") == 0)
						{
							cols = atol (avalue);
						}
						else if (strcasecmp (name, "transpose") == 0)
						{
							transpose = (atol (avalue) != 0);
						}
					}

					// AddTableObject(), but uses provided id, not mObjectID
					tableData	*td = new tableData (id, rows, cols, transpose);
					mTables.insert (mTables.end(), RWTableMap::value_type (id, td));

					XMLNode	const	*column = elem->FirstChildElement (sColumnTag);
					cols = 0;

					for ( ; column != NULL; column = column->NextSibling())
					{
						elem = column->ToElement();
						if (elem == NULL)
							continue;

						XMLNode const	*row = column->FirstChildElement (sValueTag);
						cols++;

						for ( ; row != NULL; row = row->NextSibling())
						{
							elem = row->ToElement();
							if (elem == NULL)
								continue;
							if (strcasecmp (elem->Value(), sValueTag))
							{
								id = 0;
								value.Free();

								ParseValue (elem, id, value);
								// PutTableCellData(), but uses provided td
								td->PutCellData (id, cols, value);
							}
						}	// Value
					}	// Column
				}	// Table
			}
		}
	}

	return;
}


void
RWDataProvider::Write (FILE *fd)
const
{
	RWObjectDataMap::const_iterator	oit;

	if (mObjectData.size() != 0)
	{
		fprintf (fd, "<Objects size=\"%lu\">\r\n", mObjectData.size());

		for (oit = mObjectData.begin(); oit != mObjectData.end(); oit++)
		{
			WriteValue (fd, oit->first, oit->second);
		}

		fprintf (fd, "</Objects>\r\n");
	}

	if (mTables.size() != 0)
	{
		fprintf (fd, "<Tables size=\"%lu\">\r\n", mTables.size());
		RWTableMap::const_iterator	tit;

		for (tit = mTables.begin(); tit != mTables.end(); tit++)
		{
			const RWTableMap::value_type	&tvalue (*tit);
			tableData	*td = tvalue.second;
			fprintf (fd, "<Table %s=\"%lu\"%s rows=\"%ld\" cols=\"%ld\">\r\n",
				sIDTag, td->fDataID, td->fTranspose ? " transpose=\"1\"" : "", td->fNumRows, td->fNumCols);
			RWObjectDataMap	*c = td->fTableData;
			for (long coln = 1; coln <= td->fNumCols; coln++, c++)
			{
				fprintf (fd, "<%s %s=\"%lu\" size=\"%lu\"", sColumnTag, sIDTag, coln, c->size());
				if (c->size() > 0)
				{
					fprintf (fd, ">\r\n");
					for (oit = c->begin(); oit != c->end(); oit++)
					{
						WriteValue (fd, oit->first, oit->second);
					}
					fprintf (fd, "</%s>\r\n", sColumnTag);
				}
				else
					fprintf (fd, " />\r\n");
			}
			fprintf (fd, "</Table>\r\n");
		}
		fprintf (fd, "</Tables>\r\n");
	}

	return;
}


void
RWDataProvider::Write (XMLElement *inParent)
const
{
	RWObjectDataMap::const_iterator	oit;

	if (mObjectData.size() != 0)
	{
        XMLElement *	objects = inParent->InsertNewChildElement ("Objects");
        objects->SetAttribute ("size", (unsigned int) mObjectData.size());

		for (oit = mObjectData.begin(); oit != mObjectData.end(); oit++)
		{
			WriteValue (objects, oit->first, oit->second);
		}
	}

	if (mTables.size() != 0)
	{
        XMLElement *	tables = inParent->InsertNewChildElement ("Tables");
        tables->SetAttribute ("size", (unsigned int) mTables.size());
		RWTableMap::const_iterator	tit;

		for (tit = mTables.begin(); tit != mTables.end(); tit++)
		{
			const RWTableMap::value_type	&tvalue (*tit);
			tableData				*td = tvalue.second;
            XMLElement * 			table = tables->InsertNewChildElement("Table");
			table->SetAttribute (sIDTag, (unsigned int) td->fDataID);
			if (td->fTranspose)
				table->SetAttribute ("transpose", "1");
			table->SetAttribute ("rows",  (unsigned int) td->fNumRows);
			table->SetAttribute ("cols",  (unsigned int) td->fNumCols);

            RWObjectDataMap			*c = td->fTableData;
			for (unsigned int coln = 1; coln <= td->fNumCols; coln++, c++)
			{
                XMLElement *    column = table->InsertNewChildElement (sColumnTag);
				column->SetAttribute (sIDTag, coln);
				column->SetAttribute ("size", (unsigned int) c->size());

				for (oit = c->begin(); oit != c->end(); oit++)
				{
					WriteValue (column, oit->first, oit->second);
				}
			}
		}
	}

	return;
}
