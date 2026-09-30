#ifndef	_RWDataSourceProvider_h_
# define	_RWDataSourceProvider_h_

# include	"RWDataSource.h"
# include	"RWDataProvider.h"


class	RWDataSourceProvider	:	public	RWDataSource
{
public:
						RWDataSourceProvider (void);
virtual					~RWDataSourceProvider (void);

    virtual		void		ParseReport (const XMLElement *inReport);

		RWDataProvider&	GetDataProvider (void);

virtual		bool		GetData (RWDataID inDataID, RWValue &outVar) const;
virtual		int			GetTableRowCount (RWDataID inDataID) const;
virtual		int			GetTableColumnCount (RWDataID inDataID) const;
//virtual	const CText	GetTableCellData (RWDataID inDataID, int inRow, int inColumn) const;
virtual		bool		GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue) const;
virtual		bool		GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue, int &outStartRow, int &outNumRows) const;	//mbs 06102006

private:
			// defensive programming - not implemented
						RWDataSourceProvider (const RWDataSourceProvider &inOriginal);
RWDataSourceProvider&	operator = (const RWDataSourceProvider &inOriginal);

protected:
		RWDataProvider	mData;
};

#endif
