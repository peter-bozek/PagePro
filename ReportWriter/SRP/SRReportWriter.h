#ifndef	_SRReportWriter_h_
# define	_SRReportWriter_h_

# include	"SRReportData.h"
# include	"SRDataSource.h"
# include	"RWCalculator.h"

// forward declarations
class	SR4DData;


class	SRReportWriter
	:	public	RWCalcDataProvider
{
public:
								SRReportWriter (SRDataSource &inDataSource, SRReportData &inData, long inFlags);
								~SRReportWriter (void);

				void			ReportToFile (FILE *fd);
				XMLDocument*	ReportToXML (FILE *fd);

inline			SRDataSource&	GetDataSource (void) const;
inline	const	SRReportData&	GetReportData (void) const;
inline			long			GetFlags (void) const;
inline			bool			IsExport (void) const;

	virtual		bool			GetCalculatedValue (const CText inName, RWValue &outVar) const;
				bool			IsReportVariable (const CText inName) const;
				int				IsSRReportVariable (const CText inName) const;
				bool			IsRWReportVariable (const CText inName) const;
				bool			IsCalculatedVariable (const CText inName) const;
				SR4DData	*	CreateVariable (const CText inName, long inIndex, ECalcType inCalc);
				bool			GetVariable (const CText inName, long inIndex, RWValue &outVar, ECalcType inCalc = ECalcType_CurrentValue) const;
				void			GetVariable (const CText inName, RWValue &outVar) const;
//				const CText		GetVariable (long inDataID) const;
				RWTextValue		FormatVariable (const RWValue &inVar, const CText inFormat) const;
				SR4DData	*	CreateField (const CText inName, ECalcType inCalc);
				bool			GetField (const CText inName, RWValue &outVar, ECalcType inCalc = ECalcType_CurrentValue) const;
				SR4DData*		CreateBreak (const CText inName, SRBreakSection::EBreakOn inBreak);
const RWList<RWCalculatedValue*>&	GetCalculatedVariables (void) const	{ return mCalculator.GetVariables(); }

protected:
				int				IsReportVariable (const CText inName, int inFrom, int inTo) const;
				void			Report (void);
				void			InitBreakTable (void);
				int				CheckBreak (void);

				void			FillReport (void);
				void			FillHeadersFooters (const char *inWhich);
				void			ProcessBreak (int inLevel, bool isFooter, bool emitIfEmpty);
				void			FillOneSection (SRSection *inSection, bool inFetch, bool inUseOld);

			// defensive programming - not implemented
								SRReportWriter (void);
								SRReportWriter (const SRReportWriter &inOriginal);
				SRReportWriter&	operator = (const SRReportWriter &inOriginal);

private:
	SRDataSource 				&mSource;
	SRReportData 				&mData;
	long						mFlags;	//mbs 05112010	bit 0: used by plugin, bit 1: export ID and Name
	RWCalculator				mCalculator;
	int							mBreakLevels;
	long						mCurrentIteration;
	bool						mEmitCalculator;

	RWTextValue					mVarNames [RW_VarNamesSRCount];
	FILE*						mOutFile;
	XMLDocument*				mOutXML;
    XMLElement*				mOutXMLRoot;
};


inline			SRDataSource&	SRReportWriter::GetDataSource (void) const		{ return mSource; }
inline	const	SRReportData&	SRReportWriter::GetReportData (void) const		{ return mData; }

inline			long			SRReportWriter::GetFlags (void) const			{ return mFlags; }
inline			bool			SRReportWriter::IsExport (void) const			{ return mFlags != 0; }	//mbs 05112010 pB 2011
#endif
