# include	"RWObject.h"
# include	"RWReportData.h"
# include	"RWReportWriter.h"
# include	"RWDataSource.h"
# include	<assert.h>
# include	<algorithm>
# include   <stdio.h>

// ¥¥¥ TODO ¥¥¥	all data rows have the same height, but Pictures are computed differently...


inline	bool operator <  (const RWColumn &x, const RWColumn &y);
inline	bool operator == (const RWColumn &x, const RWColumn &y);

inline	bool operator < (const RWColumn &x, const RWColumn &y)
{
	return x.GetID() < y.GetID();
}

inline	bool operator == (const RWColumn &x, const RWColumn &y)
{
	return x.GetID() == y.GetID();
}

template <class T>
struct RWless
	: std::binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const {return (*x < *y);}
};

typedef	RWless<RWColumn*>		RWColumnCompareID;
//typedef	RWequal_to<RWColumn*>	RWColumnFindID;


typedef	RWArray<RWHeader*>	RWHdrList;
typedef	RWArray<RWColumn*>	RWColList;


#pragma	mark	-

// ---------------------------------------------------------------------------
// RWHeader									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

RWHeader::RWHeader (void)
	:	mWidth (0),
		mHeight (0),
//		mText (0),
		mColSpan (1),
		mRowSpan (1),
		mStyle (0),
		mStartCol (0),
		mIsAttributed (false)
{
}


// ---------------------------------------------------------------------------
// RWHeader									Constructor			   [protected]
// ---------------------------------------------------------------------------

RWHeader::RWHeader (const CText inText, int inColSpan, int inRowSpan, RWStyle *inStyle)
	:	mWidth (0),
		mHeight (0),
//		mText (0),
		mColSpan (inColSpan),
		mRowSpan (inRowSpan),
		mStyle (inStyle),
		mStartCol (0),
		mIsAttributed (false)
{
	mText.Copy (inText);

	return;
}


// ---------------------------------------------------------------------------
// ~RWHeader								Destructor			   [protected]
// ---------------------------------------------------------------------------

RWHeader::~RWHeader (void)
{
//	mText.Free();

	return;
}


// ---------------------------------------------------------------------------
// GetStyle															  [public]
// ---------------------------------------------------------------------------

inline
RWStyle *
RWHeader::GetStyle (void)
const
{
	return mStyle;
}


// ---------------------------------------------------------------------------
// GetWidth															  [public]
// ---------------------------------------------------------------------------

inline
float
RWHeader::GetWidth (void)
const
{
	return mWidth;
}


// ---------------------------------------------------------------------------
// GetHeight														  [public]
// ---------------------------------------------------------------------------

inline
float
RWHeader::GetHeight (void)
const
{
	return mHeight;
}


// ---------------------------------------------------------------------------
// SetWidth														   [protected]
// ---------------------------------------------------------------------------

inline
void
RWHeader::SetWidth (float inWidth)
{
	mWidth = inWidth;

	return;
}


// ---------------------------------------------------------------------------
// SetHeight													   [protected]
// ---------------------------------------------------------------------------

inline
void
RWHeader::SetHeight (float inHeight)
{
	mHeight = inHeight;

	return;
}


// ---------------------------------------------------------------------------
// GetText															  [public]
// ---------------------------------------------------------------------------

inline
const CText
RWHeader::GetText (void)
const
{
	return mText;
}


// ---------------------------------------------------------------------------
// GetColSpan														  [public]
// ---------------------------------------------------------------------------

inline
int
RWHeader::GetColSpan (void)
const
{
	return mColSpan;
}


// ---------------------------------------------------------------------------
// GetStartCol														  [public]
// ---------------------------------------------------------------------------

inline
int
RWHeader::GetStartCol (void)
const
{
	return mStartCol;
}


// ---------------------------------------------------------------------------
// AdjustStartCol												   [protected]
// ---------------------------------------------------------------------------

inline
void
RWHeader::AdjustStartCol (int inCol)
{
	mStartCol += inCol;

	return;
}


// ---------------------------------------------------------------------------
// GetRowSpan														  [public]
// ---------------------------------------------------------------------------

inline
int
RWHeader::GetRowSpan (void)
const
{
	return mRowSpan;
}


// ---------------------------------------------------------------------------
// IsAttributed														  [public]
// ---------------------------------------------------------------------------

inline
bool
RWHeader::IsAttributed (void)
const
{
	return mIsAttributed;
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWHeader::Parse (RWReportData *inReport, XMLElement *inNode, long inStyleID)
{
	long			styleID = inStyleID;
    const XMLAttribute	*attrib;

	for (attrib = inNode->FirstAttribute(); attrib; attrib = attrib->Next())
	{
		const CXMLText	name = attrib->Name();
        const CXMLText	value = attrib->Value();

		if (name.compare( "style") == 0)
		{
			styleID = 0;
			if (sscanf (value.c_str(), "%li", &styleID) != 1)
				styleID = inStyleID;
		}
		else if (name.compare( "width") == 0)
		{
			mWidth = 0;
			sscanf (value.c_str(), "%g", &mWidth);
		}
		else if (name.compare( "height") == 0)
		{
			mHeight = 0;
			sscanf (value.c_str(), "%g", &mHeight);
		}
		else if (name.compare( "colspan") == 0)
		{
			mColSpan = 1;
			sscanf (value.c_str(), "%i", &mColSpan);
			if (mColSpan < 1)
				mColSpan = 1;
		}
		else if (name.compare("rowspan") == 0)
		{
			mRowSpan = 1;
			sscanf (value.c_str(), "%i", &mRowSpan);
			if (mRowSpan < 1)
				mRowSpan = 1;
		}
		else if (name.compare( "attr") == 0)
		{
			long	lVal = 1;
			sscanf (value.c_str(), "%li", &lVal);
			mIsAttributed = (lVal != 0);
		}
	}

	mText = RWTools::ParseIntoText (inNode);

	mStyle = inReport->GetStyle (styleID);

	return;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// RWColumn									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

RWColumn::RWColumn (void)
	:	mId (0),
		mWidth (0),
		mGrid (true),
		mStyle (0),
//		mFormat (0),
		mPrintRowNum (false),
		mPrintRepeatingValues (true),
		mIsAttributed (false)
{
}


// ---------------------------------------------------------------------------
// RWColumn									Constructor			   [protected]
// ---------------------------------------------------------------------------

RWColumn::RWColumn (float inWidth, RWStyle *inStyle)
	:	mId (0),
		mWidth (inWidth),
		mGrid (true),
		mStyle (inStyle),
//		mFormat (0),
		mPrintRowNum (false),
		mPrintRepeatingValues (true),
		mIsAttributed (false)
{
}


// ---------------------------------------------------------------------------
// RWColumn									Constructor			   [protected]
// ---------------------------------------------------------------------------

RWColumn::RWColumn (const RWColumn &inOriginal)
	:	mId (inOriginal.mId),
		mWidth (inOriginal.mWidth),
		mGrid (inOriginal.mGrid),
		mStyle (inOriginal.mStyle),
		mFormat ((const CXMLText) inOriginal.mFormat),	// make a copy
		mPrintRowNum (inOriginal.mPrintRowNum),
		mPrintRepeatingValues (inOriginal.mPrintRepeatingValues),
		mIsAttributed (inOriginal.mIsAttributed)
{
}


// ---------------------------------------------------------------------------
// ~RWColumn								Destructor			   [protected]
// ---------------------------------------------------------------------------

RWColumn::~RWColumn (void)
{
//	mFormat.Free();

	return;
}


// ---------------------------------------------------------------------------
// GetID															  [public]
// ---------------------------------------------------------------------------

inline
int
RWColumn::GetID (void)
const
{
	return mId;
}


// ---------------------------------------------------------------------------
// GetStyle															  [public]
// ---------------------------------------------------------------------------

inline
RWStyle *
RWColumn::GetStyle (void)
const
{
	return mStyle;
}


// ---------------------------------------------------------------------------
// GetWidth															  [public]
// ---------------------------------------------------------------------------

inline
float
RWColumn::GetWidth (void)
const
{
	return mWidth;
}


// ---------------------------------------------------------------------------
// SetWidth														   [protected]
// ---------------------------------------------------------------------------

inline
void
RWColumn::SetWidth (float inWidth)
{
	mWidth = inWidth;
}


// ---------------------------------------------------------------------------
// GetGrid															  [public]
// ---------------------------------------------------------------------------

inline
bool
RWColumn::GetGrid (void)
const
{
	return mGrid;
}


// ---------------------------------------------------------------------------
// GetFormat														  [public]
// ---------------------------------------------------------------------------

inline
const CXMLText
RWColumn::GetFormat (void)
const
{
	return mFormat;
}


// ---------------------------------------------------------------------------
// IsRowNum															  [public]
// ---------------------------------------------------------------------------

inline
bool
RWColumn::IsRowNum (void)
const
{
	return mPrintRowNum;
}


// ---------------------------------------------------------------------------
// PrintRepeatingValues												  [public]
// ---------------------------------------------------------------------------

inline
bool
RWColumn::PrintRepeatingValues (void)
const
{
	return mPrintRepeatingValues;
}


// ---------------------------------------------------------------------------
// IsAttributed														  [public]
// ---------------------------------------------------------------------------

inline
bool
RWColumn::IsAttributed (void)
const
{
	return mIsAttributed;
}


// ---------------------------------------------------------------------------
// SetStyle														   [protected]
// ---------------------------------------------------------------------------

inline
void
RWColumn::SetStyle (RWStyle *inStyle)
{
	mStyle = inStyle;

	return;
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWColumn::Parse (RWReportData *inReport, XMLElement *inNode, long inStyleID)
{
	long			styleID = inStyleID;
    XMLAttribute const	*attrib;
	long			lVal;

	for (attrib = inNode->FirstAttribute(); attrib; attrib = attrib->Next())
	{
        const CXMLText	name = attrib->Name();
        const CXMLText	value = attrib->Value();

		if (STR_EQUALS (name, "id"))
		{
			mId = 0;
			sscanf (value, "%i", &mId);
		}
		else if (STR_EQUALS (name, "style"))
		{
			styleID = 0;
			if (sscanf (value, "%li", &styleID) != 1)
				styleID = inStyleID;
		}
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
		else if (STR_EQUALS (name, "format"))
		{
			mFormat.FromXML (value);
		}
		else if (STR_EQUALS (name, "rownum"))
		{
			lVal = 1;
			sscanf (value, "%li", &lVal);
			mPrintRowNum = (lVal != 0);
		}
		else if (STR_EQUALS (name, "duplicates"))
		{
			lVal = 1;
			sscanf (value, "%li", &lVal);
			mPrintRepeatingValues = (lVal != 0);
		}
		else if (STR_EQUALS (name, "attr"))
		{
			lVal = 1;
			sscanf (value, "%li", &lVal);
			mIsAttributed = (lVal != 0);
		}
	}

	mStyle = inReport->GetStyle (styleID);

	return;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

RWTable*
RWTable::Create (RWReportData *inReport, XMLElement *inNode, int inOrder)
{
	RWTable	*table = new RWTable (inOrder);
	table->Parse (inReport, inNode);

	return table;
}


// ---------------------------------------------------------------------------
// RWTable									Constructor			   [protected]
// ---------------------------------------------------------------------------

RWTable::RWTable (int inOrder)
	:	RWObject (inOrder),
		mHeaders (0),
		mStyle (0),
		mStyleID (0),
		mDataID (0),
		mFrame (1),
		mFrameOffset (2),
		mFrameThickness (1),
		mFrameColor (cBlackColor),
		mHGridThickness (0.5),
		mRowHeight (0),
		mNumColumns (0),
		mNumRows (0),
		mNumTopHeadings (0),
		mNumLeftHeadings (0),
		mFixedColumns (false),
		mTopHeadingsHeight (0),
		mTopRowHeights (0),
		mLeftHeadingsWidth (0),
		mLinesPrinted (0),
		mLastPrintedColumn (0),
		mColWidthsCalculated (false),
		mLeftHeadingsCurItem (0),
		mLeftHeadingsStartPos (0)
{
}


// ---------------------------------------------------------------------------
// ~RWTable									Destructor			   [protected]
// ---------------------------------------------------------------------------

RWTable::~RWTable (void)
{
	if (mHeaders)
		delete [] mHeaders;
	if (mTopRowHeights)
		delete [] mTopRowHeights;
	if (mLeftHeadingsCurItem)
		delete [] mLeftHeadingsCurItem;
	if (mLeftHeadingsStartPos)
		delete [] mLeftHeadingsStartPos;

	return;
}


// ---------------------------------------------------------------------------
// Reset															  [public]
// ---------------------------------------------------------------------------

void
RWTable::Reset (bool inAll)
{
	RWObject::Reset (inAll);

	if (inAll)
	{
		mLinesPrinted = 0;
		mLastPrintedColumn = 0;
		if (mLeftHeadingsCurItem)
		{
			for (int i = 0; i < mNumLeftHeadings; i++)
			{
				mLeftHeadingsCurItem [i] = 0;
				mLeftHeadingsStartPos [i] = 0;
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// ParseHeading													   [protected]
// ---------------------------------------------------------------------------

void
RWTable::ParseHeading (XMLElement *inNode)
{
	mNumTopHeadings = 0;
	mFixedColumns = true;

    XMLElement	*elem;
	XMLNode		*node, *next;

	for (node = inNode->FirstChildElement ("tr"); node; node = next)
	{
		next = node->NextSibling();
		elem = node->ToElement();
		if (elem == NULL ||  (elem->Value().compare( "tr") != 0)
			continue;
		mNumTopHeadings++;
	}

	if (mNumTopHeadings > 0)
	{
		mHeaders = new RWHdrList [mNumTopHeadings];
		int	line = 0;
		int	numRows = 0, numCols = -1;
		for (node = inNode->FirstChildElement ("tr"); node && line < mNumTopHeadings; node = next, line++)
		{
			next = node->NextSibling();
			elem = node->ToElement();
			if (elem == NULL ||  (elem->Value().compare( "tr") != 0)
				continue;

			long			styleID = mStyleID;
            const CXMLText	style = elem->Attribute ("style");
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

				RWHeader	*hdr = new RWHeader;
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
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
RWTable::Parse (RWReportData *inReport, XMLElement *inNode)
{
	RWObject::Parse (inReport, inNode);

//	mFixedH = false;
	mFixedV = false;
//	mBindH = false;
	mBindV = false;

	XMLAttribute	*attrib;
	for (attrib = inNode->FirstAttribute(); attrib; attrib = attrib->Next())
	{
		const CXMLText	name = attrib->Name();
		const CXMLText	value = attrib->Value();

		if (STR_EQUALS (name, "dataID"))
		{
			mDataID = 0;
			sscanf (value, "%li", &mDataID);
		}
		else if (STR_EQUALS (name, "style"))
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
		else if (STR_EQUALS (name, "frameColor") || STR_EQUALS (name, "lineColor"))
		{
			mFrameColor = value;
		}
		else if (STR_EQUALS (name, "hGridThickness"))
		{
			mHGridThickness = 0;
			sscanf (value, "%g", &mHGridThickness);
			if (mHGridThickness < 0 || mHGridThickness > 10)
				mHGridThickness = 0.5;
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

    XMLElement	*elem;
/*
	if (mDataID == 0)
		elem = inNode;
	else
		elem = inNode->FirstChildElement ("Table");

	if (elem != NULL)
	{
		ParseHeading (elem);	// fixed table heading
		elem = inNode;
	}
	else
*/
	elem = inNode->FirstChildElement ("Head");

	if (elem != NULL)
	{
		ParseHeading (elem);	// fixed table heading
		elem = inNode;
	}

	{
		elem = inNode->FirstChildElement ("Columns");
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

	if (mDataID && elem)
	{
		XMLNode	*node, *next;

		inNode = elem;
		for (node = elem->FirstChildElement(); node; node = next)
		{
			next = node->NextSibling();
			elem = node->ToElement();
			if (elem == NULL || !STR_EQUALS (elem->Value(), "Col"))
			{
//				inNode->RemoveChild (node);
				continue;
			}

			RWColumn	*column = new RWColumn;
			column->Parse (inReport, elem, mStyleID);
			mColumns.push_back (column);
		}
	}

	mStyle = inReport->GetStyle (mStyleID);


	// query the data server
	const RWDataSource&	src = GetDataSource();
	int					line, col, hdr;
	if (mDataID != 0)
	{
		const SOpaqueCategoryItem	*headings;
		mNumLeftHeadings = src.GetTableHeadings (mDataID, RWDataSource::eLeftHeadings, headings);
		mNumRows = src.GetTableRowCount (mDataID);
//		if (mFixedColumns)
			// left headings are counted in mNumColumns!!!
			mNumColumns = src.GetTableColumnCount (mDataID) + mNumLeftHeadings;
		if (mNumLeftHeadings > 0)
		{
			mLeftHeadingsCurItem = new int [mNumLeftHeadings];
			mLeftHeadingsStartPos = new int [mNumLeftHeadings];
			for (line = 0; line < mNumLeftHeadings; line++)
			{
				mLeftHeadingsCurItem [line] = 0;
				mLeftHeadingsStartPos [line] = 0;
			}

		}
		if (not mFixedColumns)	// convert top headings into fixed table heading
		{
			mNumTopHeadings = src.GetTableHeadings (mDataID, RWDataSource::eTopHeadings, headings);
			if (mNumTopHeadings > 0)
			{
				mHeaders = new RWHdrList [mNumTopHeadings];
				RWHeader	*header;
				RWTextValue	text;
				if (mNumLeftHeadings > 0)
				{
					text = src.GetTableTitle (mDataID);
					header = new RWHeader (text, mNumLeftHeadings, mNumTopHeadings, mStyle);
					mHeaders [0].push_back (header);
				}

				for (line = 0; line < mNumTopHeadings; line++)
				{
					for (col = mNumLeftHeadings, hdr = 0; col < mNumColumns; hdr++)
					{
						int			span, level;
						text = src.GetTableHeadingData (mDataID, headings[line], hdr, span, level);
//						RWStyle	*	st = GetReportData()->GetStyle (mStyle->GetID() + level);
						header = new RWHeader (text, span, 0, mStyle);
						mHeaders [line].push_back (header);
						col += header->GetColSpan();
					}
				}
			}
		}
	}

	AdjustHeaders();
	AdjustColumns();

	return;
}


// ---------------------------------------------------------------------------
// AdjustHeaders												   [protected]
// ---------------------------------------------------------------------------

void
RWTable::AdjustHeaders (void)
{
	// adjust startColumn for headers
	if (mNumTopHeadings > 0)
	{
		int	line, col, hdr, last;
		for (line = 0; line < mNumTopHeadings; line++)
		{
			last = mHeaders [line].size();
			for (col = 0, hdr = 0; col < mNumColumns && hdr < last; hdr++)
			{
				RWHeader	*header = mHeaders [line] [hdr];
				int			subline, subcol, subhdr, colend, span;
				header->AdjustStartCol (col);
//				if (col == 0)
					col = header->GetStartCol();
				span = header->GetColSpan();
				colend = col + span;
				for (subline = line + header->GetRowSpan() - 1; subline > line; subline--)
				{
					int	sublast = mHeaders [subline].size();
					for (subcol = 0, subhdr = 0; subcol < colend && subhdr < sublast; subhdr++)
					{
						header = mHeaders [subline] [subhdr];
						if (col > header->GetStartCol())
							break;
						header->AdjustStartCol (span);	//mbs 02082010	real span instead of 1
						subcol += header->GetColSpan();
					}
				}
				col = colend;
			}
		}

#if	0 && TARGET_DEBUG
		printf ("\nRW: AdjustHeaders: RWTABLE %ld, %d topHeadings, %d rows, %d cols\n", mDataID, mNumTopHeadings, mNumRows, mNumColumns);
		for (line = 0; line < mNumTopHeadings; line++)
		{
			int	last = mHeaders [line].size();
			printf ("Line %d: %d", line + 1, last);
			for (hdr = 0; hdr < last; hdr++)
			{
				RWHeader	*header = mHeaders [line] [hdr];
				col = header->GetStartCol();
				printf ("\t%d,%d", col + 1, header->GetColSpan());
				col += header->GetColSpan();
			}
			printf ("\n");
		}
		printf ("\n");
	fflush (stdout);
#endif

	}

	return;
}


// ---------------------------------------------------------------------------
// AdjustColumns												   [protected]
// ---------------------------------------------------------------------------

void
RWTable::AdjustColumns (void)
{
	RWColumn	*column, *def = NULL;

	// create real column order
	if (not mFixedColumns && mColumns.size() > 0)
	{
		RWColList		tmpCols (mColumns);
		std::sort<RWColList::iterator, RWColumnCompareID> (tmpCols.begin(), tmpCols.end(), RWColumnCompareID());
		mColumns.erase (mColumns.begin(), mColumns.end());	// mColumns.clear(); but w/o RWColumn deletion
		mColumns.reserve (mNumColumns);
		for (int cur = 1; cur <= mNumColumns; cur++)
		{
			RWColumn							*curcol, *right = NULL;
			RWList<RWColumn*>::const_iterator	it;
			int									col = cur - mNumLeftHeadings;
			column = NULL;

			if (col > 0 || def == NULL)
				for (it = tmpCols.begin(); it != tmpCols.end(); it++)
				{
					curcol = *it;
					int	id = curcol->GetID();
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
			column = new RWColumn (*column);
			if (col <= 0)
				column->SetStyle (mStyle);
			mColumns.push_back (column);
		}
	}

	// create missing columns
	while (mColumns.size() < (unsigned int) mNumColumns)
	{
		column = new RWColumn (0, mStyle);
		mColumns.push_back (column);
	}

	return;
}


// ---------------------------------------------------------------------------
// GetHeader													   [protected]
// ---------------------------------------------------------------------------

RWHeader *
RWTable::GetHeader (int inRow, int inCol)
const
{
assert (inRow >= 0);
assert (inRow < mNumTopHeadings);
assert (inCol >= 0);
assert (inCol < mNumColumns);

	int			line, col, hdr;
	RWHeader	*header = NULL;
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

RWColumn *
RWTable::GetColumn (int inCol)
const
{
assert (inCol >= 0);
assert (inCol < mNumColumns);

	return mColumns [inCol];
}


// ---------------------------------------------------------------------------
// GetColsWidth													   [protected]
// ---------------------------------------------------------------------------

float
RWTable::GetColsWidth (int inFrom, int inTo)
const
{
assert (inFrom >= 0);
assert (inTo >= inFrom);
assert (inTo < mNumColumns);

	int		col;
	float	colsWidth = 0;
	for (col = inFrom; col <= inTo; col++)
	{
		RWColumn	*column = GetColumn (col);
		colsWidth += column->GetWidth();
	}

	return colsWidth;
}


// ---------------------------------------------------------------------------
// CalculateAll													   [protected]
// ---------------------------------------------------------------------------

void
RWTable::CalculateAll (RWPageComposer &inComposer)
{
	mTopHeadingsHeight = 0;

	RWHeader	*header;
	RWColumn	*column;
	float		width;
	float		height;
	int			line, col, hdr, last, span;
	float		lineWidth, lineHeight;
	bool		fixed;

	float	*colWidths = new float [mNumColumns];
/*	//mbs 06082010	commented out
	if (mPosition.Width() > mNumColumns * 10)
	{
		fixed = true;
		width = mPosition.Width() / mNumColumns;
		for (col = 0; col < mNumColumns; col++)
			colWidths [col] = width;
	}
	else
*/
	{
		memset (colWidths, 0, mNumColumns * sizeof (float));
		fixed = false;
	}

	// calculate top headers
	for (line = 0; line < mNumTopHeadings; line++)
	{
		lineWidth = 0;
		last = mHeaders [line].size();
		for (hdr = 0; hdr < last; hdr++)
		{
			header = mHeaders [line] [hdr];
			col = header->GetStartCol();
			width = header->GetWidth();
			height = header->GetHeight();
			if (width == 0 || height == 0)
			{
				SRect	r (0, 0, 200, 0);
				inComposer.MeasureText (header->GetText(), header->GetStyle(), r, false, header->IsAttributed(), true, NULL);
				if (width == 0)
				{
					width = r.Width();
					if (mFrame)		// space for frame
						width += (mFrameThickness + mFrameOffset) * 2;
				}
				if (height == 0)
				{
					height = r.Height();
					if (mFrame)		// space for frame
						height += (mFrameThickness + mFrameOffset) * 2;
				}
			}

			header->SetWidth (width);
			lineWidth += width;
			header->SetHeight (height);
//			if (not fixed)
			{
				span = header->GetColSpan();
				width /= span;
				for (span--; span >= 0; span--)
				{
					if (colWidths [col + span] < width)
						colWidths [col + span] = width;
				}
			}
			col += header->GetColSpan();
		}
	}

	// adjust height of top headers
	mTopRowHeights = new float [mNumTopHeadings];
	for (line = mNumTopHeadings - 1; line >= 0; line--)
	{
		lineHeight = 0;
		last = mHeaders [line].size();
		for (hdr = 0; hdr < last; hdr++)
		{
			header = mHeaders [line] [hdr];
			col = header->GetStartCol();
			height = header->GetHeight();
			span = header->GetRowSpan();
			for (span--; span > 0; span--)
				height -= mTopRowHeights [line + span];
			if (height > lineHeight)
				lineHeight = height;
			col += header->GetColSpan();
		}

		mTopRowHeights [line] = lineHeight;
		mTopHeadingsHeight += lineHeight;

		for (hdr = 0; hdr < last; hdr++)
		{
			header = mHeaders [line] [hdr];
			col = header->GetStartCol();
			height = lineHeight;
			span = header->GetRowSpan();
			for (span--; span > 0; span--)
				height += mTopRowHeights [line + span];
			header->SetHeight (height);
			col += header->GetColSpan();
		}
	}

	// calculate columns
	for (col = 0; col < mNumColumns; col++)
	{
		column = GetColumn (col);
		width = column->GetWidth();
		if (width == 0)
		{
			if (mNumRows > 0)
			{
				RWDataSource	&src = GetDataSource();
				RWStyle			*st = column->GetStyle();
				RWValue			var;
				RWTextValue		text;

				lineWidth = 0;
				if (col <= mNumLeftHeadings - 1)	// leftHeadings
				{
					const SOpaqueCategoryItem	*headings;
					src.GetTableHeadings (mDataID, RWDataSource::eLeftHeadings, headings);
					const SOpaqueCategoryItem	headingsCol = headings[col];
					for (line = 0, hdr = 0; line < mNumRows; hdr++)
					{
						int	span, level;
						text = src.GetTableHeadingData (mDataID, headingsCol, hdr, span, level);
						if (not text.IsEmpty())
						{
							SRect	r (0, 0, 800, 0);
							st = GetReportData()->GetStyle (st->GetID() + level);
							width = inComposer.MeasureText (text, st, r, false, column->IsAttributed(), true, NULL);
							if (width > lineWidth)
								lineWidth = width;
						}
						text.Free();
						hdr += span;
					}
				}
				else if (column->IsRowNum())	// %ROWNUM% column
				{
					var.SetInteger (mNumRows);
					text = src.FormatVariable (var, column->GetFormat());
					if (not text.IsEmpty())
					{
						SRect	r (0, 0, 800, 0);
						width = inComposer.MeasureText (text, st, r, false, column->IsAttributed(), true, NULL);
						if (width > lineWidth)
							lineWidth = width;
					}
					text.Free();
				}
				else	// data column
				{
					if (mFixedColumns)
						hdr = col - mNumLeftHeadings;
					else
						hdr = col;
					const CText	format = column->GetFormat();
					for (line = 0; line < mNumRows; line++)
					{
						if (src.GetTableCellData (mDataID, line + 1, hdr + 1, var))
						{
							if (var.GetKind() >= RWValue::eValue_PictRefScreen)
							{
								SRect	r (0, 0, 800, 400);
								RWPictData	*cd = NULL;
								inComposer.GetPictBounds (r, var, ePictFormat_ScaledProp, &cd, true);
								if (r.Width() > lineWidth)
									lineWidth = r.Width();
//								inComposer.FreePict (&cd);
								if (cd)
									delete cd;
							}
							else
							{
								text = src.FormatVariable (var, format);
								if (not text.IsEmpty())
								{
									SRect	r (0, 0, 800, 0);
									width = inComposer.MeasureText (text, st, r, false, column->IsAttributed(), true, NULL);
									if (width > lineWidth)
										lineWidth = width;
								}
								text.Free();
							}
						}
					}
				}
				if (mFrame)		// space for frame
					lineWidth += (mFrameThickness + mFrameOffset) * 2;
				width = lineWidth;
				if (width < colWidths [col])
					width = colWidths [col];	// expand column
				else
					colWidths [col] = width;	// expand header
			}
			else
				width = colWidths [col];		// no data - use header
			column->SetWidth (width);
		}
		else if (width > colWidths [col])		// column with specified width, header is taller - expand header
			colWidths [col] = width;
		else if (width < colWidths [col])
			column->SetWidth (colWidths [col]);	// header is wider - expand column

		if (col <= mNumLeftHeadings - 1)
			mLeftHeadingsWidth += width;
	}

	// adjust headers
	for (line = 0; line < mNumTopHeadings; line++)
	{
		last = mHeaders [line].size();
		for (hdr = 0; hdr < last; hdr++)
		{
			header = mHeaders [line] [hdr];
			col = header->GetStartCol();
			span = header->GetColSpan();
			width = 0;
			for (span--; span >= 0; span--)
				width += colWidths [col + span];
			header->SetWidth (width);
		}
	}

	// calculate row height - every column might use different style
	if (mRowHeight <= 0)	//mbs 09082010	don't adjust height if set by user
	{
		#if	CChar_Size == 1
			#define	rowTextMeasurement	((const CText) "ROW Ãšg")
		#else
			CText	us (reinterpret_cast <const UTF8Char*> ("ROW Ãšg"));
			#define	rowTextMeasurement	(us.GetU16Str())
		#endif

		for (col = -1; col < mNumColumns; col++)
		{
			RWStyle	*style;
			SRect	r (0, 0, 200, 0);
			if (col >= 0)
				style = GetColumn (col)->GetStyle();
			else
				style = mStyle;
			width = inComposer.MeasureText (rowTextMeasurement, style, r, false, false, true, NULL);
			height = r.Height();
			if (mFrame)		// space for frame
				height += (mFrameThickness + mFrameOffset) * 2;
			if (mRowHeight < height)
				mRowHeight = height;
		}
	}

#if	0 && TARGET_DEBUG
	printf ("\nRW: CalculateAll: RWTABLE %ld, %d topHeadings, %d rows, %d cols\n", mDataID, mNumTopHeadings, mNumRows, mNumColumns);
	for (line = 0; line < mNumTopHeadings; line++)
	{
		int	last = mHeaders [line].size();
		printf ("Line %d: %d", line + 1, last);
		width = 0;
		for (hdr = 0; hdr < last; hdr++)
		{
			header = mHeaders [line] [hdr];
			col = header->GetStartCol();
			width += header->GetWidth();
			printf ("\t%d,%d,%.2f", col + 1, header->GetColSpan(), header->GetWidth());
		}
		printf ("\twidth=%.2f\n", width);
	}
	printf ("Column widths: ");
	width = 0;
	for (col = 0; col < mNumColumns; col++)
	{
		column = GetColumn (col);
		width += column->GetWidth();
		printf (col > 0 ? ", %.2f" : "%.2f", column->GetWidth());
	}
	printf ("\twidth=%.2f\n", width);
	printf ("colWidths    : ");
	width = 0;
	for (col = 0; col < mNumColumns; col++)
	{
		width += colWidths [col];
		printf (col > 0 ? ", %.2f" : "%.2f", colWidths [col]);
	}
	printf ("\twidth=%.2f\n\n", width);
	fflush (stdout);
#endif

	delete [] colWidths;

	mColWidthsCalculated = true;

	return;
}


// ---------------------------------------------------------------------------
// GetBounds														  [public]
// ---------------------------------------------------------------------------

bool
RWTable::GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inIsOverflow, bool *outRemoveRow)
{
	if (outRemoveRow)
		*outRemoveRow = false;
	if (not mColWidthsCalculated)
		CalculateAll (inComposer);

	if (GetReportWriter()->GetCurrentHorPage() == 1)
		mLastPrintedColumn = mNumLeftHeadings;		// fixed left headings must fit!

	float	colsWidth, rowsHeight;
	if (mLastPrintedColumn == mNumColumns)
		colsWidth = 0;
	else
		colsWidth = GetColsWidth (mLastPrintedColumn, mNumColumns - 1);
	rowsHeight = (mNumRows - mLinesPrinted) * mRowHeight;	// + mFrameThickness + mFrameOffset;
	mVirtPosition.bottom = mVirtPosition.top + mTopHeadingsHeight + rowsHeight;	// + ((mFrameThickness + mFrameOffset) * 2);
	mVirtPosition.right = mVirtPosition.left + mLeftHeadingsWidth + colsWidth;	// + ((mFrameThickness + mFrameOffset) * 2);
	if (mFrame)		// space for frame
	{
		rowsHeight += mFrameThickness + mFrameOffset;
		mVirtPosition.bottom += (mFrameThickness + mFrameOffset) * 3 + mFrameThickness;
		mVirtPosition.right += (mFrameThickness + mFrameOffset) * 2 + mFrameThickness;
	}

	bool	fit = RWObject::GetBounds (inComposer, ioRect, inFit, inIsOverflow, NULL);
	int		line, col, last, hdr;
	if (inFit && fit)
	{
		if (mTopHeadingsHeight > ioRect.Height() && mDataID != 0)	// static table might be split over page boundaries
			fit = false;
		else
		{
			colsWidth = mLeftHeadingsWidth;	// + mFrameThickness + mFrameOffset;
			if (mFrame)		// space for frame
				colsWidth += mFrameThickness + mFrameOffset;
			for (col = mLastPrintedColumn; col < mNumColumns; col++)
			{
				colsWidth += GetColsWidth (col, col);
				if (colsWidth > ioRect.Width())
					break;
			}
			mColsToPrint = col - mLastPrintedColumn;
			if (mColsToPrint == 0 && mDataID != 0 && mNumColumns == 1 && mLastPrintedColumn == 0)
				mColsToPrint = 1;	// the single-column table does not fit on paper...

			if (mColsToPrint > 0)
			{
				for (line = 0; line < mNumTopHeadings; line++)
				{
					last = mHeaders [line].size();
					for (hdr = 0; hdr < last; hdr++)
					{
						RWHeader	*header = mHeaders [line] [hdr];
						col = header->GetStartCol();
						if (col < mLastPrintedColumn)
							continue;
						if (col >= mLastPrintedColumn + mColsToPrint)
							break;
						if (col + header->GetColSpan() > mLastPrintedColumn + mColsToPrint)
						{
							if (col - mLastPrintedColumn > 0)
								mColsToPrint = col - mLastPrintedColumn;
						}
					}
				}
				assert (mColsToPrint > 0);
			}

			if (mDataID == 0)
			{
				// mRowsToPrint is number of table [header] rows
				rowsHeight = ioRect.Height();	// - ((mFrameThickness + mFrameOffset) * 2);
				if (mFrame)		// space for frame
					rowsHeight -= (mFrameThickness + mFrameOffset) * 2;
				for (mRowsToPrint = 0; mRowsToPrint < mNumTopHeadings && rowsHeight >= mTopRowHeights [mRowsToPrint]; mRowsToPrint++)
					rowsHeight -= mTopRowHeights [mRowsToPrint];
				if ((mRowsToPrint < mNumTopHeadings && mNumTopHeadings < 10) || (mRowsToPrint < 2 && mNumTopHeadings > 1))
					fit = false;
				else if (mRowsToPrint == mNumTopHeadings - 1)	// at least two rows at start-of-page
					mRowsToPrint--;
			}
			else
			{
				// mRowsToPrint is number of data rows
				if (mFrame)		// space for frame
//					mRowsToPrint = (ioRect.Height() - mTopHeadingsHeight - mFrameThickness - (mFrameOffset * 2)) / mRowHeight;
					mRowsToPrint = (ioRect.Height() - mTopHeadingsHeight - mFrameThickness * 2 - ((mFrameThickness + mFrameOffset) * 2)) / mRowHeight;	//mbs 04082010
				else
					mRowsToPrint = (ioRect.Height() - mTopHeadingsHeight + 0.00001) / mRowHeight; // to avoid rounding differences
				if (mRowsToPrint < 2 && mNumRows - mLinesPrinted > 1)	// at least two rows at end-of-page
					fit = false;
				else if (mRowsToPrint - (mNumRows - mLinesPrinted) == -1 && mNumRows - mLinesPrinted > 1)	// at least two rows at start-of-page
					mRowsToPrint--;

				//+++ no support for "grouped" data lines to be kept together...
				if (mRowsToPrint > 0 && mNumLeftHeadings > 0)	// left headings
				{
					RWDataSource&				src = GetDataSource();
					const SOpaqueCategoryItem	*headings;
					src.GetTableHeadings (mDataID, RWDataSource::eLeftHeadings, headings);
					for (col = 0; col < mNumLeftHeadings; col++)
					{
						for (line = 0; line < mRowsToPrint; )
						{
							int	span, level;
							RWTextValue	text = GetTableHeadingData (src, headings[col], line + mLinesPrinted, col, span, level);
							if (mLinesPrinted > mLeftHeadingsStartPos [col])
								span -= mLinesPrinted - mLeftHeadingsStartPos [col];
//							span -= (mLinesPrinted + line - mLeftHeadingsStartPos [col]);
							if (span <= mRowsToPrint)
								if (line + span > mRowsToPrint)
								{
									if (span < 4)
										mRowsToPrint = line;
									else if (	mRowsToPrint - line < 2			// at least two rows at end-of-page
											||	mRowsToPrint - line == span - 1	// at least two rows at start-of-page
									)
										mRowsToPrint--;
								}
							line += span;
						}
					}
					if (mRowsToPrint < 1)
						fit = false;
				}
			}
		}
	}
	
	if (fit && mAlignment != eAlign_None)
	{
		float	width = ioRect.Width();
#if	0	//mbs 28052010
		SRect	page;
		GetReportWriter()->GetPageBounds (page);
		SRect	margins;
		if (GetReportWriter()->GetPageComposer().GetReportMargins (margins))	//mbs 23122009
		{
//			page.top += margins.top;
			page.left += margins.left;
//			page.bottom -= margins.bottom;
			page.right -= margins.right;
		}
#else
		SRect	page = GetReportWriter()->GetPageComposer().GetTruePageRect();
#endif
		if (ioRect.Width() < page.Width())
		{
			if (mAlignment == eAlign_Left)
			{
				ioRect.left = page.left;
				ioRect.right = ioRect.left + width;
			}
			else if (mAlignment == eAlign_Right)
			{
				ioRect.right = page.right - 1;
				ioRect.left = ioRect.right - width;
			}
			else	// if (mAlignment == eAlign_Center)
			{
				ioRect.left = (page.Width() - width) / 2;
				ioRect.right = ioRect.left + width;
			}
		}
		else if (mAlignment == eAlign_Left)
		{
			ioRect.left = page.left;
			ioRect.right = ioRect.left + width;
		}
	}
	return fit;
}


// ---------------------------------------------------------------------------
// Draw																  [public]
// ---------------------------------------------------------------------------

RWObject::EDrawState
RWTable::Draw (RWPageComposer &inComposer, SRect &inRect, bool inIsOverflow)
{
	if (mColsToPrint > 0)
	{
		RWStyle		*style = mStyle;
		RWColumn	*column;
		SRect		r (inRect);
		int			line, col, hdr, last, linesToPrint;

		if (mDataID == 0)
		{
			r.bottom = r.top;	// + ((mFrameThickness + mFrameOffset) * 2);
			if (mFrame)		// space for frame
				r.bottom += (mFrameThickness + mFrameOffset) * 2;
			for (line = 0; line < mRowsToPrint + mLinesPrinted; line++)
				if (line >= mLinesPrinted)
					r.bottom += mTopRowHeights [line];
		}
		else
		{
			r.bottom = r.top + mTopHeadingsHeight + mRowsToPrint * mRowHeight;	// + ((mFrameThickness + mFrameOffset) * 2);
			if (mFrame)		// space for frame
			{
				r.bottom += (mFrameThickness + mFrameOffset) * 2;
				if (mTopHeadingsHeight > 0)
					r.bottom += mFrameThickness;
			}
		}

		r.right = r.left + mLeftHeadingsWidth;	// + ((mFrameThickness + mFrameOffset) * 2);
		if (mFrame)		// space for frame
			r.right += (mFrameThickness + mFrameOffset) * 2;
		r.right += GetColsWidth (mLastPrintedColumn, mLastPrintedColumn + mColsToPrint - 1);

		// draw the table frame
		SRect	rect (r);
		if (mFrame > 1)
		{
			r.right += mFrameThickness;
			r.bottom += mFrameThickness;
			inComposer.DrawRect (r, mFrameThickness, true, mStyle->GetFrameColor(), false, cBlackColor);
			r.right -= mFrameThickness;
			r.bottom -= mFrameThickness;
		}
		if (mFrame)		// space for frame
			r *= mFrameThickness + mFrameOffset;

		// draw top headers
		if (mDataID == 0)
			linesToPrint = mRowsToPrint;
		else
			linesToPrint = mNumTopHeadings;

		for (line = 0; line < linesToPrint; line++)
		{
			RWHeader	*header;
			last = mHeaders [line].size();
			r.left = rect.left;	// + mFrameThickness + mFrameOffset;
			if (mFrame)		// space for frame
				r.left += mFrameThickness + mFrameOffset;
			if (mNumLeftHeadings > 0)
				col = 0;
			else
				col = mLastPrintedColumn;
			for (hdr = 0; hdr < last && col < mLastPrintedColumn + mColsToPrint; hdr++)
			{
				header = mHeaders [line] [hdr];
				if (	(mNumLeftHeadings == 0 || header->GetStartCol() >= mNumLeftHeadings)
					&&	(header->GetStartCol() + header->GetColSpan() <= mLastPrintedColumn))
					continue;

				style = header->GetStyle();
				if (col != header->GetStartCol())
				{
					if (col < mLastPrintedColumn)
						col = mLastPrintedColumn;
					if (col < header->GetStartCol())
						r.left += GetColsWidth (col, header->GetStartCol() - 1);
					col = header->GetStartCol();
				}
				r.right = r.left + header->GetWidth();
				if (hdr < last && col < mLastPrintedColumn && col >= mNumLeftHeadings)
					r.right -= GetColsWidth (col, mLastPrintedColumn - 1);

				if (mFrame)		// space for frame
				{
					if (r.right > rect.right - mFrameThickness - mFrameOffset)
						r.right = rect.right - mFrameThickness - mFrameOffset;
				}
				else
				{
					if (r.right > rect.right)
						r.right = rect.right;
				}
				r.bottom = r.top + header->GetHeight();

				if (mFrame)
				{
					r.bottom += mFrameThickness;
					r.right += mFrameThickness;
					inComposer.DrawRect (r, mFrameThickness, true, style->GetFrameColor(), false, cBlackColor);
					r *= mFrameThickness + mFrameOffset;
					r.right -= mFrameThickness;
				}

				const CText	ctext = header->GetText();
				if (ctext && *ctext)
					inComposer.DrawTextBox (ctext, style, r, style->ShouldWrap(), header->IsAttributed(), true, NULL);

				r.left = r.right;
				if (mFrame)
				{
					r.top -= mFrameThickness + mFrameOffset;
					r.left += mFrameThickness + mFrameOffset;
				}
				col += header->GetColSpan();
			}
			r.top += mTopRowHeights [line];
		}


		if (mDataID != 0 && mRowsToPrint > 0)
		{
			// draw vertical grid
			if (mFrame)
			{
				r.left = rect.left + mFrameThickness + mFrameOffset;
				r.top = rect.top + mTopHeadingsHeight + mFrameThickness * 2 + mFrameOffset;
				if (mNumTopHeadings > 0)
					r.top += mFrameThickness;
				r.bottom = rect.bottom - mFrameThickness - mFrameOffset;
				r.right = r.left;
				inComposer.DrawLine (r, mFrameThickness, mFrameColor, RWLine_Vertical);	// mStyle->GetFrameColor()
				for (col = 0; col < mNumLeftHeadings; col++)
				{
					column = GetColumn (col);
					style = column->GetStyle();
					r.left += column->GetWidth();
					r.right = r.left;
					if (column->GetGrid())
						inComposer.DrawLine (r, mFrameThickness, style->GetFrameColor(), RWLine_Vertical);
				}
				for (col = mLastPrintedColumn; col < mLastPrintedColumn + mColsToPrint; col++)
				{
					column = GetColumn (col);
					style = column->GetStyle();
					r.left += column->GetWidth();
					r.right = r.left;
					if (col == mLastPrintedColumn + mColsToPrint - 1)
						inComposer.DrawLine (r, mFrameThickness, mFrameColor, RWLine_Vertical);
					else if (column->GetGrid())
						inComposer.DrawLine (r, mFrameThickness, style->GetFrameColor(), RWLine_Vertical);
				}
			}

			// draw the cells
			RWDataSource&				src = GetDataSource();
			const SOpaqueCategoryItem	*headings;
			if (mNumLeftHeadings > 0)
				src.GetTableHeadings (mDataID, RWDataSource::eLeftHeadings, headings);

			r.top = rect.top + mTopHeadingsHeight;	// + mFrameThickness + mFrameOffset;
			if (mFrame)		// space for frame
			{
				r.top += mFrameThickness + mFrameOffset;
				if (mNumTopHeadings > 0)
					r.top += mFrameThickness;
			}

			for (line = 0; line < mRowsToPrint; line++)
			{
				r.left = rect.left;	// + mFrameThickness + mFrameOffset;
				if (mFrame)		// space for frame
					r.left += mFrameThickness + mFrameOffset;

				// left headings
				for (col = 0; col < mNumLeftHeadings; col++)
				{
					int	span, level;
					RWTextValue	text = GetTableHeadingData (src, headings[col], line + mLinesPrinted, col, span, level);
					column = GetColumn (col);
					r.right = r.left + column->GetWidth();
					if (	span == 1
						||	line == 0	// mLeftHeadingsStartPos [col] < mLinesPrinted
						||	mLeftHeadingsStartPos [col] == line + mLinesPrinted
						||	mLeftHeadingsStartPos [col] + span - 1 == line + mLinesPrinted
					)
					{
						style = column->GetStyle();
						style = GetReportData()->GetStyle (style->GetID() + level);
						last = span - (mLinesPrinted + line - mLeftHeadingsStartPos [col]);
						// draw horizontal grid	//mbs 27012010
						if (line == 0 && mFrame)
						{
							r.bottom = r.top;
							inComposer.DrawLine (r, mFrameThickness, mFrameColor, RWLine_Horizontal);	// style->GetFrameColor()
							r.top += mFrameThickness;
						}
						
						r.bottom = r.top + last * mRowHeight;
						if (mFrame)		// space for frame
						{
							if (r.bottom > rect.bottom - mFrameThickness - mFrameOffset)
							{
								r.bottom = rect.bottom - mFrameThickness - mFrameOffset;
								last = 1;
							}
							r *= mFrameThickness + mFrameOffset;
						}
						else
						{
							if (r.bottom > rect.bottom)
							{
								r.bottom = rect.bottom;
								last = 1;
							}
						}
						if (line == 0 || span == 1 || mLinesPrinted + line != mLeftHeadingsStartPos [col] + span - 1)
						{
							if (not text.IsEmpty())
								inComposer.DrawTextBox (text, style, r, style->ShouldWrap(), column->IsAttributed(), true, NULL);
						}

						if (mFrame && (line == 0 || last == 1))
						{
							r *= - mFrameThickness - mFrameOffset;
							SRect	bottom (r);
							bottom.top = bottom.bottom;
							inComposer.DrawLine (bottom, mFrameThickness, style->GetFrameColor(), RWLine_Horizontal);
							r.left = r.right;
						}
						else if (mFrame)		// space for frame
						{
							r.top -= mFrameThickness + mFrameOffset;
							r.left = r.right + mFrameThickness + mFrameOffset;
						}
						if (line == 0 && mFrame)	// draw horizontal grid	//mbs 27012010
							r.top -= mFrameThickness;
					}
					else
						r.left = r.right;
				}

				// draw horizontal grid	//mbs 17122009
				if (line == 0 && mFrame)
				{
					r.bottom = r.top;
					r.left = rect.left + mLeftHeadingsWidth + mFrameThickness + mFrameOffset;
					r.right = rect.right - mFrameOffset;
					inComposer.DrawLine (r, mFrameThickness, mFrameColor, RWLine_Horizontal);	// style->GetFrameColor()
					r.top += mFrameThickness;
				}

				// data columns
				for (col = mLastPrintedColumn; col < mLastPrintedColumn + mColsToPrint; col++)
				{
					column = GetColumn (col);
					style = column->GetStyle();
					const CText	format = column->GetFormat();
					r.right = r.left + column->GetWidth();
					r.bottom = r.top + mRowHeight;
					if (mFrame)		// space for frame
						r *= mFrameThickness + mFrameOffset;
					RWTextValue	text;

					if (column->IsRowNum())	// %ROWNUM% column
					{
						RWValue	row (long (1 + line + mLinesPrinted));
						text = src.FormatVariable (row, format);
						if (not text.IsEmpty())
							inComposer.DrawTextBox (text, style, r, style->ShouldWrap(), column->IsAttributed(), true, NULL);
					}
					else
					{
						RWValue	var;
						int	srow, nrows;
						if (src.GetTableCellData (mDataID, line + mLinesPrinted + 1, col - mNumLeftHeadings + 1, var, srow, nrows))
						{
                            if (column->PrintRepeatingValues() || line == 0 || srow == line + mLinesPrinted + 1) {
								if (var.GetKind() >= RWValue::eValue_PictRefScreen)
								{
									RWPictData	*cd = NULL;
									inComposer.GetPictBounds (r, var, ePictFormat_ScaledProp, &cd, false);
									inComposer.DrawPict (r, var, ePictFormat_ScaledProp, &cd, 0, 0);
//									inComposer.FreePict (&cd);
									if (cd)
										delete cd;
								}
								else
								{
									text = src.FormatVariable (var, format);
									if (not text.IsEmpty())
										inComposer.DrawTextBox (text, style, r, style->ShouldWrap(), column->IsAttributed(), true, NULL);
								}
                            }
						}
					}
					text.Free();
					if (mFrame)		// space for frame
						r.top -= mFrameThickness + mFrameOffset;
					r.left = r.right;	// + mFrameThickness + mFrameOffset;
					if (mFrame)		// space for frame
						r.left += mFrameThickness + mFrameOffset;
				}

				r.top += mRowHeight;

				// draw horizontal grid
				if (mFrame)
				{
					if (line == mRowsToPrint - 1)
					{
						r.top -= mFrameThickness;
						r.bottom = r.top;
						r.left = rect.left + mLeftHeadingsWidth + mFrameThickness + mFrameOffset;
						r.right = rect.right - mFrameOffset;
						inComposer.DrawLine (r, mFrameThickness, mFrameColor, RWLine_Horizontal);	// style->GetFrameColor()
					}
					else if (mHGridThickness > 0)
					{
						r.top -= (mFrameThickness - mHGridThickness) / 2;
						r.bottom = r.top;
						r.left = rect.left + mLeftHeadingsWidth + mFrameThickness * 2 + mFrameOffset;
						r.right = rect.right - mFrameThickness - mFrameOffset;
						inComposer.DrawLine (r, mHGridThickness, mFrameColor, RWLine_Horizontal);	// style->GetFrameColor()
						r.top += (mFrameThickness - mHGridThickness) / 2;
					}
				}
			}
		}
	}

	int	state = eDrawState_Done;
	mLastPrintedColumn += mColsToPrint;
	if (mLastPrintedColumn < mNumColumns)
		state = eDrawState_Horizontal;
	else
		mLinesPrinted += mRowsToPrint;

	if (mDataID == 0)
	{
		if (mLinesPrinted < mNumTopHeadings)
			state |= eDrawState_Vertical;
	}
	else if (mLinesPrinted < mNumRows)
		state |= eDrawState_Vertical;

	if (state == eDrawState_Done && mLastPrintedColumn == mNumColumns)
		mPrinted = true;

	return EDrawState (state);
}


// ---------------------------------------------------------------------------
// GetTableHeadingData											   [protected]
// ---------------------------------------------------------------------------

RWTextValue
RWTable::GetTableHeadingData (RWDataSource &inSrc, const void* inHeading, int inLine, int inCol, int &outSpan, int &outLevel)
const
{
assert (inCol >= 0 && inCol < mNumLeftHeadings);
assert (inLine >= 0 && inLine < mNumRows);

	RWTextValue	text;
	int			item = mLeftHeadingsCurItem [inCol];
	int			line = inLine - mLeftHeadingsStartPos [inCol];
	int			span = 1, level;

	const		SOpaqueCategoryItem	hdr = reinterpret_cast <const SOpaqueCategoryItem> (inHeading);

	if (line < 0)
	{
		line = inLine;
		item = 0;
	}
	for ( ; line >= 0; item++)
	{
		text = inSrc.GetTableHeadingData (mDataID, hdr, item, span, level);
		line -= span;
	}

	--item;
	if (mLeftHeadingsCurItem [inCol] != item)
	{
		mLeftHeadingsCurItem [inCol] = item;
		mLeftHeadingsStartPos [inCol] = inLine - line - span;
	}
	outSpan = span;	// - (mLinesPrinted + inLine - mLeftHeadingsStartPos [inCol]);
	outLevel = level;

	return text;
}
