#ifndef	_RWDataProvider_h_
# define	_RWDataProvider_h_

# include	"RWBaseTypes.h"

/*
- repeating values are squeezed
- repeating objects have just one column of data (even horizontally repeated)
- for simplicity, we will use RWValue as members...
*/


class	RWDataProvider
{
public:
							RWDataProvider (void);
							~RWDataProvider (void);

// data provider (creation/preparation)
		RWDataID			AddObject (const RWValue &inValue);
		RWDataID			AddTableObject (long inNumColumns, bool inTranspose);
		void				PutTableCellData (RWDataID inDataID, long inRow, long inColumn, const RWValue &inValue);
		void				FinalizeTableObject (RWDataID inDataID);

// data consumer
		bool				IsEmpty (void) const;
		bool				GetObject (RWDataID inDataID, RWValue &outValue) const;
		long				GetTableRowCount (RWDataID inDataID) const;
		long				GetTableColumnCount (RWDataID inDataID) const;
		bool				GetTableCellData (RWDataID inDataID, long inRow, long inColumn, RWValue &outValue) const;
		bool				GetTableCellData (RWDataID inDataID, long inRow, long inColumn, RWValue &outValue, long &outStartRow, long &outNumRows) const;	//mbs 06102006

// persistency
		void				Parse (RWXmlNode inParent);
		void				Write (RWXmlNode inParent) const;

typedef	RWMap <RWDataID, RWValue>		RWObjectDataMap;
	class	tableData
	{
	friend class	RWDataProvider;
	public:
		long				GetRowCount (void) const;
		long				GetColumnCount (void) const;
		RWObjectDataMap*	GetColumn (long inColumn) const;
		void				PutCellData (long inRow, long inColumn, const RWValue &inValue);
		bool				GetCellData (long inRow, long inColumn, RWValue &outValue) const;
		bool				GetCellData (long inRow, long inColumn, RWValue &outValue, long &outStartRow, long &outNumRows) const;	//mbs 06102006

	protected:
		RWDataID			fDataID;
		long				fNumRows;
		long				fNumCols;
		bool				fTranspose;
		RWObjectDataMap		*fTableData;	// array of columns

							tableData (RWDataID id, long inNumRows, long inNumColumns, bool inTranspose);
							tableData (RWDataID id, long inNumColumns, bool inTranspose);
							~tableData (void);

	private:
			// defensive programming - not implemented
							tableData (const tableData &inOriginal);
		tableData	&		operator = (const tableData &inOriginal);
	};
typedef	RWMap<RWDataID, tableData*>		RWTableMap;

		tableData	*		GetTableObject (RWDataID inDataID) const;

protected:
	static	void				ParseValue (RWXmlNode inNode, RWDataID &outID, RWValue &outVar);
	static	void				WriteValue (RWXmlNode inParent, RWDataID inID, const RWValue &inVar);

private:
			// defensive programming - not implemented
							RWDataProvider (const RWDataProvider &inOriginal);
		RWDataProvider	&	operator = (const RWDataProvider &inOriginal);

protected:
	RWObjectDataMap			mObjectData;
	RWTableMap				mTables;
	RWDataID				mObjectID;
};

#endif
