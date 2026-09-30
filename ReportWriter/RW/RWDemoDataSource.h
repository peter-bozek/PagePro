#ifndef	_RWDemoDataSource_h_
# define	_RWDemoDataSource_h_

# include	"RWBaseTypes.h"
# include	"RWDataSource.h"
# include	"RWDataSourceProvider.h"

namespace	RWDemoDataFormatter
{
	RWTextValue		FormatVariable (const RWValue &inVar, const CText inFormat);
}


class	RWDemoDataSource
	:	public	RWDataSource
{
public:
						RWDemoDataSource (void);
//						~RWDemoDataSource (void);

virtual			int		GetTableRowCount (RWDataID inDataID) const;
virtual			int		GetTableColumnCount (RWDataID inDataID) const;
//virtual	const char	*	GetTableCellData (RWDataID inDataID, int inRow, int inColumn) const;
virtual			bool	GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue) const;
virtual		bool		GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue, int &outStartRow, int &outNumRows) const;	//mbs 06102006

virtual			int		GetTableHeadings (RWDataID inDataID, EHeadings inWhich, const SOpaqueCategoryItem *&outTable) const;
virtual	RWTextValue		GetTableHeadingData (RWDataID inDataID, SOpaqueCategoryItem inHeading, int inItem, int &outSpan, int &outLevel) const;
virtual	RWTextValue		GetTableTitle (RWDataID inDataID) const;

virtual	RWTextValue		FormatVariable (const RWValue &inVar, const CText inFormat) const;
};



class	RWDemoDataSourceProvider	: public	RWDataSourceProvider
{
public:
						RWDemoDataSourceProvider (void);
						~RWDemoDataSourceProvider (void);

virtual	RWTextValue		FormatVariable (const RWValue &inVar, const CText inFormat) const;

private:
			// defensive programming - not implemented
						RWDemoDataSourceProvider (const RWDemoDataSourceProvider &inOriginal);
RWDemoDataSourceProvider&	operator = (const RWDemoDataSourceProvider &inOriginal);
};

#endif
