#ifndef	_SRReportWriter_h_
# define	_SRReportWriter_h_

# include	"SRReportData.h"
# include	"SRDataSource.h"
# include	"RWCalculator.h"

// forward declarations
class	SR4DData;


# include	<memory>

class	SRReportWriter
	:	public	RWCalcDataProvider
{
public:
								SRReportWriter (SRDataSource &inDataSource, SRReportData &inData, long inFlags);
								~SRReportWriter (void);

	// processes the report; the result is an RWXML (".rwxml") document
				std::unique_ptr<RWXmlDocument>	ReportToXML (void);

inline			SRDataSource&	GetDataSource (void) const;
inline	const	SRReportData&	GetReportData (void) const;
inline			long			GetFlags (void) const;
inline			bool			IsExport (void) const;

	virtual		bool			GetCalculatedValue (const RWString inName, RWValue &outVar) const;
				bool			IsReportVariable (const RWString inName) const;
				int				IsSRReportVariable (const RWString inName) const;
				bool			IsRWReportVariable (const RWString inName) const;
				bool			IsCalculatedVariable (const RWString inName) const;
				SR4DData	*	CreateVariable (const RWString inName, long inIndex, ECalcType inCalc);
				bool			GetVariable (const RWString inName, long inIndex, RWValue &outVar, ECalcType inCalc = ECalcType_CurrentValue) const;
				void			GetVariable (const RWString inName, RWValue &outVar) const;
//				const RWString		GetVariable (long inDataID) const;
				RWString		FormatVariable (const RWValue &inVar, const RWString inFormat) const;
				SR4DData	*	CreateField (const RWString inName, ECalcType inCalc);
				bool			GetField (const RWString inName, RWValue &outVar, ECalcType inCalc = ECalcType_CurrentValue) const;
				SR4DData*		CreateBreak (const RWString inName, SRBreakSection::EBreakOn inBreak);
const RWList<RWCalculatedValue*>&	GetCalculatedVariables (void) const	{ return mCalculator.GetVariables(); }

protected:
				int				IsReportVariable (const RWString inName, int inFrom, int inTo) const;
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

	RWString					mVarNames [RW_VarNamesSRCount];
	RWXmlDocument*				mOutXML;		// set while ReportToXML runs
	RWXmlNode					mOutXMLRoot;
};


inline			SRDataSource&	SRReportWriter::GetDataSource (void) const		{ return mSource; }
inline	const	SRReportData&	SRReportWriter::GetReportData (void) const		{ return mData; }

inline			long			SRReportWriter::GetFlags (void) const			{ return mFlags; }
inline			bool			SRReportWriter::IsExport (void) const			{ return mFlags != 0; }	//mbs 05112010 pB 2011
#endif
