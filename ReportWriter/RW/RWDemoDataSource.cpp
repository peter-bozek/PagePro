# include	"RWDemoDataSource.h"

# include	<stdio.h>	// snprintf


namespace	RWDemoDataFormatter
{

RWTextValue	FormatVariable (const RWValue &inVar, const CText inFormat)
{
	char		buf [64];
	const char	*result = NULL;
	RWTextValue	re;

	switch (inVar.GetKind())
	{
		case RWValue::eValue_Undefined:
			result = ""; // pB 2010-12 "###Undefined Value###";
			break;

		case RWValue::eValue_Boolean:
		{
			CText	u (inFormat);
			if (u.length() == 0)
                u = CText((unsigned short *) u"True;False");
			long	pos = u.find (';', 0);
			if (pos == string::npos)
				pos = u.length();
			if (inVar.GetInteger())
				u.erase (pos, string::npos);
			else
				u.erase (0, pos + 1);
			re.Attach (u.c_str());
			break;
		}

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
			re = inVar.GetText();
			break;

		case RWValue::eValue_DateTime:
		{
			time_t	tim = (time_t) inVar.GetInteger();
			struct	tm *lt = localtime (&tim);
			strftime (buf, sizeof (buf), "%Y-%m-%d %T %Z", lt);
			result = buf;
			break;
		}

		case RWValue::eValue_Date:
		{	// day: 0 - 31 ==> 5 bits
			// month: 0 - 12 ==> 4 bits
			// day | (month << 5) | (year << 9)
			snprintf (buf, sizeof (buf), "%04ld-%02ld-%02ld", inVar.GetInteger() >> 9, (inVar.GetInteger() >> 5) & 0xF, inVar.GetInteger() & 0x1F);
			result = buf;
			break;
		}

		case RWValue::eValue_Time:
		{
			snprintf (buf, sizeof (buf), "%02ld.%02ld.%02ld", inVar.GetInteger() / 3600, inVar.GetInteger() / 60 % 60, inVar.GetInteger() % 60);
			result = buf;
			break;
		}

		case RWValue::eValue_BLOB:
			result = "###BLOB Value###";
			break;

		case RWValue::eValue_PictRefScreen:
		case RWValue::eValue_PictRefPrint:
		case RWValue::eValue_PicturePICT:
		case RWValue::eValue_PicturePDF:
		case RWValue::eValue_PictureJPG:
		case RWValue::eValue_PicturePNG:
		case RWValue::eValue_PictureTIFF:
		case RWValue::eValue_PictureEMF:
			result = "###Picture Value###";
			break;

		default:
			result = "###Unknown Data Type###";
			break;
	}

	if (result)
		re = result;

	return re;
}


}	// namespace	RWDemoDataFormatter



/// minimal table item stub = { flags, number|text }
typedef	struct	STable_item_stub
{
    unsigned long			flags;          ///< flags
    enum {                 // possible item value types
        TYPE_NUMBER         = 0x00000000,   ///< 64 bits, (double), (long long)
        TYPE_TEXT           = 0x00000001    ///< (const char *), (char *)
    };
    union {
        double              number;         ///< number value
        const char *        text;           ///< text value (must be const due union ?)
    };
}	STable_item_stub;

/// generic category item (hierarchical list) = { flags, id, span, level, text, key }
typedef	struct	SCategory_item
{
    unsigned long			level;          ///< level
    const char *            text;           ///< text
    unsigned long			span;           ///< span over lower-level items
    unsigned long			flags;          ///< flags
    enum {  // item flags, attributes
      ATTR_MASK           = 0x0000ffff,     ///< attributes mask
      ATTR_EXPANDED       = 0x00000020,     ///< item is expanded [-]
      ATTR_EXISTS         = 0x00001000,     ///< item exists
      ATTR_LOWEST         = 0x00002000      ///< item is lowest (has no childs)
    };
}	SCategory_item;


// for CP 1250	: ⁄
// for UTF8		: Ú
// for UTF16	: ⁄
// just FYI: it is an U_accute = 0xDA ;-)
static	SCategory_item	topHdr[] =
{
	{	0, "hdr 1 Úg", 1, 0	},
	{	0, "hdr 2 Úg", 1, 0	},
	{	0, "hdr 3 Úg", 1, 0	},
	{	0, "hdr 4 Úg", 1, 0	},
	{	0, "hdr 5 Úg", 1, 0	},
	{	0, "hdr 6 Úg", 1, 0	},
	{	0, "hdr 7 Úg", 1, 0	},
	{	0, "hdr 8 Úg", 1, 0	},
	{	0, "hdr 9 Úg", 1, 0	}
//	{	0, "hdr 10 Úg", 1, 0	},
//	{	0, "hdr 11 Úg", 1, 0	},
//	{	0, "hdr 12 Úg", 1, 0	},
//	{	0, "hdr 13 Úg", 1, 0	},
//	{	0, "hdr 14 Úg", 1, 0	}
};

static	SCategory_item	left1Hdr[] =
{
	{	0, "row 1 Úg\nLevel=0",		1, 0	},
	{	1, "row 2 Úg\nLevel=1",		2, 0	},
	{	2, "row 3 Úg\nLevel=2",		3, 0	},
	{	3, "row 4 Úg\nLevel=3",		4, 0	},
	{	4, "row 5 Úg\r\nLevel=4",	5, 0	},
	{	5, "row 6 Úg\rLevel=5",		6, 0	},
	{	0, "row 7 Úg Level=0",		7, 0	},
	{	1, "row 8 Úg Level=1",		8, 0	},
	{	2, "row 9 Úg Level=2",		9, 0	},
	{	3, "row 10 Úg Level=3",	10, 0	},
	{	4, "row 11 Úg Level=4",	11, 0	},
	{	5, "row 12 Úg Level=5",	12, 0	},
	{	0, "row 13 Úg Level=0",	13, 0	},
	{	0, "row 14 Úg Level=0",	5, 0	},
	{	0, "row 15 Úg Level=0",	5, 0	},
	{	0, "row 16 Úg Level=0",	5, 0	},
	{	0, "row 17 Úg Level=0",	5, 0	},
	{	0, "row 18 Úg Level=0",	5, 0	},
	{	0, "row 19 Úg Level=0",	5, 0	},
	{	0, "row 20 Úg Level=0",	5, 0	},
	{	0, "row 21 Úg Level=0",	5, 0	},
	{	0, "row 22 Úg Level=0",	5, 0	},
	{	0, "row 23 Úg Level=0",	5, 0	},
	{	0, "row 24 Úg Level=0",	1, 0	},
	{	0, "row 25 Úg Level=0",	1, 0	}
};

static	SCategory_item	left2Hdr[] =
{
	{	0, "1 Úg", 1, 0	},
	{	0, "2.1 Úg", 1, 0	},
	{	0, "2.2 Úg", 1, 0	},
	{	0, "3 Úg", 3, 0	},
	{	0, "4 Úg", 4, 0	},
	{	0, "5 Úg", 5, 0	},
	{	0, "6 Úg", 6, 0	},
	{	0, "7 Úg", 7, 0	},
	{	0, "8 Úg", 8, 0	},
	{	0, "9 Úg",	9, 0	},
	{	0, "10.1 Úg", 3, 0	},
	{	0, "10.2 Úg", 4, 0	},
	{	0, "10.3 Úg", 3, 0	},
	{	0, "11 Úg", 11, 0	},
	{	0, "12 Úg", 12, 0	},
	{	0, "13 Úg", 13, 0	},
	{	0, "14 Úg", 5, 0	},
	{	0, "15 Úg", 5, 0	},
	{	0, "16 Úg", 5, 0	},
	{	0, "17 Úg", 5, 0	},
	{	0, "18.1 Úg", 3, 0	},
	{	0, "18.2 Úg", 2, 0	},
	{	0, "19 Úg", 5, 0	},
	{	0, "20 Úg", 5, 0	},
	{	0, "21 Úg", 5, 0	},
	{	0, "22 Úg", 5, 0	},
	{	0, "23 Úg", 5, 0	},
	{	0, "24 Úg", 1, 0	},
	{	0, "25 Úg", 1, 0	}
};


static	SCategory_item*	topHdrLine[] = { topHdr };
static	SCategory_item*	leftHdrLine[] = { left1Hdr, left2Hdr };


static	inline	bool	IsTable (RWDataID inDataID)	{ return inDataID == 1 || inDataID == 2; }


// ---------------------------------------------------------------------------
// RWDemoDataSource							Default Constructor		  [public]
// ---------------------------------------------------------------------------

RWDemoDataSource::RWDemoDataSource (void)
{
	return;
}


// ---------------------------------------------------------------------------
// GetTableRowCount													  [public]
// ---------------------------------------------------------------------------
// Return number of data rows in a table

int
RWDemoDataSource::GetTableRowCount (RWDataID inDataID)
const
{
	if (inDataID == 1)
	{
		return 4;
	}
	else if (inDataID == 2)
	{
		const	SCategory_item	*i;
		int	count;
		for ( count = 0, i = left1Hdr; i < left1Hdr + sizeof (left1Hdr) / sizeof (left1Hdr [0]); i++ )
			count += i->span;
		return count;
	}

	return 0;
}


// ---------------------------------------------------------------------------
// GetTableColumnCount												  [public]
// ---------------------------------------------------------------------------
// Return number of data columns in a table

int
RWDemoDataSource::GetTableColumnCount (RWDataID inDataID)
const
{
	if (inDataID == 1)
	{
		return 20;
	}
	else if (inDataID == 2)
	{
		const	SCategory_item	*i;
		int	count;
		for ( count = 0, i = topHdr; i < topHdr + sizeof (topHdr) / sizeof (topHdr [0]); i++ )
			count += i->span;
		return count;
	}

	return 0;
}


// ---------------------------------------------------------------------------
// GetTableCellData													  [public]
// ---------------------------------------------------------------------------
// Return text for specified cell in a table

//const char	*
//RWDemoDataSource::GetTableCellData (RWDataID inDataID, int inRow, int inColumn)
bool
RWDemoDataSource::GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue)
const
{
	outValue.Free();

	if (IsTable (inDataID))
	{
		static	char	buf [64];
		snprintf (buf, sizeof (buf), "Úg Cell <%d, %d>", inRow, inColumn);
		#if	CChar_Size == 1
			outValue.SetText ((const CText) buf);
		#else
			CText	us (RWTextValue::UTF_8_to_UTF16(buf));
			outValue.SetText (us.c_str(), true);
		#endif
		return true;
	}

	return false;
}

//mbs 06102006
bool
RWDemoDataSource::GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue, int &outStartRow, int &outNumRows)
const
{
	outValue.Free();

	if (IsTable (inDataID))
	{
		static	char	buf [64];
		snprintf (buf, sizeof (buf), "Úg Cell <%d, %d>", inRow, inColumn);
		#if	CChar_Size == 1
			outValue.SetText ((const CText) buf);
		#else
			CText	us ( RWTextValue::UTF_8_to_UTF16(buf));
			outValue.SetText (us.c_str(), true);
		#endif

		outStartRow = inRow;
		outNumRows = 1;
		return true;
	}

	return false;
}


// ---------------------------------------------------------------------------
// GetTableHeadings													  [public]
// ---------------------------------------------------------------------------
// Return array of headings in a table

int
RWDemoDataSource::GetTableHeadings (RWDataID inDataID, EHeadings inWhich, const SOpaqueCategoryItem *&outTable)
const
{
	if (inDataID == 2)
	{
		if (inWhich == eTopHeadings)
		{
			outTable = reinterpret_cast <const SOpaqueCategoryItem*> (topHdrLine);
			return 1;
		}
		else
		{
			outTable = reinterpret_cast <const SOpaqueCategoryItem*> (leftHdrLine);
			return 2;
		}

	}
	else
	{
		outTable = NULL;
		return 0;
	}
}


// ---------------------------------------------------------------------------
// GetTableHeadingData												  [public]
// ---------------------------------------------------------------------------
// Return text of heading in a table

RWTextValue
RWDemoDataSource::GetTableHeadingData (RWDataID inDataID, SOpaqueCategoryItem inHeading, int inItem, int &outSpan, int &outLevel)
const
{
	RWTextValue	text;

	if (inDataID == 2)
	{
		const	SCategory_item	*i = reinterpret_cast <const SCategory_item*> (inHeading);
		if (i == topHdr)
		{
			if (inItem >= 0 && inItem < (int) (sizeof (topHdr) / sizeof (topHdr [0])))
			{
				outSpan = topHdr [inItem].span;
				outLevel = topHdr [inItem].level;
				text = reinterpret_cast <const UTF8Char*> (topHdr [inItem].text);
			}
		}
		else if (i == left1Hdr)
		{
			if (inItem >= 0 && inItem < (int) (sizeof (left1Hdr) / sizeof (left1Hdr [0])))
			{
				outSpan = left1Hdr [inItem].span;
				outLevel = left1Hdr [inItem].level;
				text = reinterpret_cast <const UTF8Char*> (left1Hdr [inItem].text);
			}
		}
		else if (i == left2Hdr)
		{
			if (inItem >= 0 && inItem < (int) (sizeof (left2Hdr) / sizeof (left2Hdr [0])))
			{
				outSpan = left2Hdr [inItem].span;
				outLevel = left2Hdr [inItem].level;
				text = reinterpret_cast <const UTF8Char*> (left2Hdr [inItem].text);
			}
		}
	}

	return text;
}


// ---------------------------------------------------------------------------
// GetTableTitle													  [public]
// ---------------------------------------------------------------------------
// Return title (topleft cell in a table with both top & left headings)

RWTextValue
RWDemoDataSource::GetTableTitle (RWDataID inDataID)
const
{
	RWTextValue	text;

	if (inDataID == 2)
	{
		text = "Demo Table";
	}

	return text;
}


// ---------------------------------------------------------------------------
// FormatVariable													  [public]
// ---------------------------------------------------------------------------
// Convert specified variable to a text representation

RWTextValue
RWDemoDataSource::FormatVariable (const RWValue &inVar, const CText inFormat)
const
{
	return RWDemoDataFormatter::FormatVariable (inVar, inFormat);
}



// ---------------------------------------------------------------------------
// RWDemoDataSourceProvider					Constructor				  [public]
// ---------------------------------------------------------------------------

RWDemoDataSourceProvider::RWDemoDataSourceProvider (void)
{
	return;
}


// ---------------------------------------------------------------------------
// ~RWDemoDataSourceProvider				Destructor				  [public]
// ---------------------------------------------------------------------------

RWDemoDataSourceProvider::~RWDemoDataSourceProvider (void)
{
	return;
}


// ---------------------------------------------------------------------------
// FormatVariable													  [public]
// ---------------------------------------------------------------------------
// Convert specified variable to a text representation

RWTextValue
RWDemoDataSourceProvider::FormatVariable (const RWValue &inVar, const CText inFormat)
const
{
	return RWDemoDataFormatter::FormatVariable (inVar, inFormat);
}
