#ifndef	_RWDataSource_h_
# define	_RWDataSource_h_

# include	"RWBaseTypes.h"

typedef	const struct	SOpaqueCategoryItem_*	SOpaqueCategoryItem;


class	RWDataSource
{
public:
	enum	EHeadings	{	eTopHeadings, eLeftHeadings	};
						RWDataSource (void);
virtual					~RWDataSource (void);

virtual		void		ParseReport (RWXmlNode inReport);

#if 0
virtual		bool		IsDynamic (void) const;
virtual		long		GetNumberOfIterations (void) const;
virtual		long		GetCurrentIteration (void) const;

virtual	void			Reset (void);	// reset variables
virtual	void			Push (void);	// save variables, current record number
virtual	void			Pop (void);		// restore variables, current record number
//virtual	long			FetchNextRecord (void);	// fetch next "record" - can be break (>0), record (0) or eof (-1)
virtual	bool			FetchNextRecord (void);	// fetch next "record" - false == EOF
#endif

virtual		void		CreateVariable (const RWString inName, RWValue *inVar = NULL);
virtual		void		SetVariable (const RWString inName, RWValue *inVar);
virtual		bool		GetVariable (const RWString inName, RWValue &outVar) const;
virtual		bool		GetStdVariable (int inVar, RWValue &outVar) const;

virtual		bool		GetData (RWDataID inDataID, RWValue &outVar) const;
virtual		int			GetTableRowCount (RWDataID inDataID) const;
virtual		int			GetTableColumnCount (RWDataID inDataID) const;
//virtual	const RWString	GetTableCellData (RWDataID inDataID, int inRow, int inColumn) const;
virtual		bool		GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue) const;
virtual		bool		GetTableCellData (RWDataID inDataID, int inRow, int inColumn, RWValue &outValue, int &outStartRow, int &outNumRows) const;	//mbs 06102006

virtual		int			GetTableHeadings (RWDataID inDataID, EHeadings inWhich, const SOpaqueCategoryItem *&outTable) const;
virtual	RWString		GetTableHeadingData (RWDataID inDataID, SOpaqueCategoryItem inHeading, int inItem, int &outSpan, int &outLevel) const;
virtual	RWString		GetTableTitle (RWDataID inDataID) const;

virtual	RWString		FormatVariable (const RWValue &inVar, const RWString inFormat) const = 0;

private:
			// defensive programming - not implemented
						RWDataSource (const RWDataSource &inOriginal);
		RWDataSource&	operator = (const RWDataSource &inOriginal);

protected:
	RWVarMap			mVariables;
//	RWVarMap			mTempVariables;
//	bool				mUseTemp;
	time_t				mPrintTime;
	long				mIteration;
};

#endif
