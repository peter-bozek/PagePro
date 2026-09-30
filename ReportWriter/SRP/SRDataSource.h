#ifndef	_SRDataSource_h_
# define	_SRDataSource_h_

# include	"RWBaseTypes.h"
# include	"RWDataSourceProvider.h"
# include	"SR4DData.h"
# include	"PSObject.h"
# include	"ExtendedExecute.h"

typedef	std::pair <CText, long>			SRVarNameKey;
typedef	RWMap<SRVarNameKey, SR4DVariable*>		SRVarMap;
typedef	RWMap<CText, SR4DField*>			SRFldMap;


// forward declarations
class	SRReportWriter;


class	SRDataSource
	: public	PSObject,
	  public	RWDataSourceProvider
{
public:
	enum	ERelate
	{
		eRelate_None,
		eRelate_Automatic,
		eRelate_Manual
	};
	enum	EDataSource
	{
		eDataSource_Undefined,
		eDataSource_Table,
		eDataSource_Fixed,
		eDataSource_Variable,
		eDataSource_Array
	};

						SRDataSource (void);
						~SRDataSource (void);

virtual		void		ParseReport (RWXmlNode inReport) override;
inline		void		SetReportWriter (SRReportWriter *inReportWriter);
			long		GetNumberOfIterations (void) const;
			long		GetCurrentIteration (void) const;

//virtual		void		CreateVariable (SConstText inName, RWValue *inVar = NULL);
//virtual		void		SetVariable (SConstText inName, RWValue *inVar);
//virtual		bool		GetVariable (SConstText inName, RWValue &outVar) const;

			SR4DData*	CreateVariable (const CText inName, long inIndex);
			bool		GetVariable (const CText inName, long inIndex, RWValue &outVar, bool inUseOld);
			void		GetVariable (const CText inName, RWValue &outVar);
			SR4DData*	CreateField (const CText inName);
			bool		GetField (const CText inName, RWValue &outVar, bool inUseOld);
			RWDataID	EmitPicture (const RWValue &inVar);
			RWDataID	EmitRepeating (SR4DData *inVar, bool inIsHorizontal);

//virtual			int		GetTableHeadings (RWDataID inDataID, EHeadings inWhich, const SOpaqueCategoryItem *&outTable) const;
//virtual	const CText		GetTableHeadingData (RWDataID inDataID, SOpaqueCategoryItem inHeading, int inItem, int &outSpan, int &outLevel) const;
//virtual	const CText		GetTableTitle (RWDataID inDataID) const;

virtual	RWTextValue		FormatVariable (const RWValue &inVar, const CText inFormat) const;

		void			Reset (void);	// reset variables, seek before first record
		bool			FetchNextRecord (void);	// fetch next "record" - false == EOF

		void			RunScript (ExtendedExecute &inScript, PSObject *inObject = NULL, bool inAlwaysInvalidate = false);
		void			Close ();
		void			Invalidate (void);
		void			Write (FILE *fd) const;
		void			Write (XMLElement *inParent) const;
		void			WriteReportData (FILE *fd) const;
		void			WriteReportData (XMLElement *inParent) const;

	virtual	const PSObjProps *	GetProperties (void) const;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);

protected:
		void			SetCallBackID (void);
		void			SetStdVariables (void);					// RWReport / SRDate, SRTime, SRPage, SRArea
		void			SetStdRecordNumber (void);				// RWRecord / SRRecord
		void			SetStdObjectID (PSObject *inObject);	// RWObject / SRObjectID

		void			ParseDataSource (XMLElement *inNode);

private:
			// defensive programming - not implemented
						SRDataSource (const SRDataSource &inOriginal);
		SRDataSource&	operator = (const SRDataSource &inOriginal);

protected:
	static	const PSObjProps	sProperties[];
		ExtendedExecute	mStartScript;
		ExtendedExecute	mBodyScript;
		ExtendedExecute	mEndScript;
		EDataSource		mSource;	// table, array size, variable, fixed
		RWTextValue		mName;		// [4], arrayName, variableName
		long			mNumIterations;
		int				mMainTable;
		short			mRelateOne;
		short			mRelateMany;
		RWTextValue		mCallBackName;
		bool			mSRPCompatibility;
		long			mCallBackID;
		long			mCurIteration;
		SRReportWriter *mReportWriter;
		SRVarMap		mVariables;
		SRFldMap		mFields;
};

inline	const PSObject::PSObjProps *	SRDataSource::GetProperties (void) const			{ return sProperties; }
inline	void							SRDataSource::SetReportWriter (SRReportWriter *inReportWriter)	{ mReportWriter = inReportWriter; }

#endif
