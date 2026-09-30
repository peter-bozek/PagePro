#ifndef	_SRReportData_h_
# define	_SRReportData_h_

# include	"RWStyle.h"
# include	"SRObject.h"
# include	"SRSection.h"


// forward declarations
class	SRReportWriter;


// class for storing the pre-parsed report
class	SRReportData
	:	public	PSObject
{
friend class	SRReportWriter;
	
public:
									SRReportData (RWXmlDocument *inXML);
	virtual							~SRReportData (void);

inline			SRReportWriter	*	GetReportWriter (void) const;
		RWXmlNode				GetReport (void) const;
				const CText			GetName (void) const;
//inline			SRSectionList	*	GetSections (void);
inline			SRSectionList	*	GetPageSections (void);
inline			SRSectionList	*	GetBreakHeaders (void);
inline			SRSectionList	*	GetBreakFooters (void);
inline			SRPageSection	*	GetBodySection (void);
inline			SRWatermarkSection*	GetWatermarkSection (void);
inline			RWStyle			*	GetStyle (long inID) const;
//				void				ParseScript (char * &outScript, XMLElement *inNode);
inline			bool				IsSimple (void) const;
	
//inline			SBlob				GetPageSetup (void) const;
//inline			SBlob				GetPageFormat (void) const;
//inline			SBlob				GetPrintSettings (void) const;

	virtual		const PSObjProps *	GetProperties (void) const;
	virtual		bool				GetProperty (OSType id, RWValue &outValue);
	virtual		bool				SetProperty (OSType id, RWValue &inValue);
	inline		RWStyleList *		GetStyles (void);
	inline		long				GetLastStyle (void) const;	//mbs 20052011
protected:
inline			void				SetReportWriter (SRReportWriter *inReportWriter);
				void				ParseReport (void);
                RWXmlNode				Write (RWXmlDocument &outXML) const;

private:
				void				ParseStyleSet (RWXmlNode inStyleSet);
				void				ParseSection (RWXmlNode inSection);
				void				ParseObjects (SRObjListD *inParent, RWXmlNode inObject);
//				void				ParseDataSource (XMLElement *inNode);

			// defensive programming - not implemented
									SRReportData (void);
									SRReportData (const SRReportData &inOriginal);
				SRReportData	&	operator = (const SRReportData &inOriginal);

private:
static	const PSObjProps	sProperties[];
		SRReportWriter	*	mReportWriter;
		RWXmlDocument	*		mXML;
		RWTextValue			mName;
		RWTextValue			mID;

		RWStyleList			mStyles;
		long				mLastStyle;	//mbs 20052011
		SRSectionList		mPageSections;		// Headers & Footers
		SRSectionList		mBreakHeaders;
		SRSectionList		mBreakFooters;
		SRWatermarkSection*	mWatermark;
		SRPageSection	*	mBody;
		bool				mSimple;
		float				mPageWidth;
		float				mPageHeight;
		bool				mUsePhysical;
	
// v1.4
//		bool					mLabelReport;
//		int						mLabelH;
//		int						mLabelV;

        bool                mReportRotation;
        bool                mReportMirror;
    
		SRect				mPageMargins;
//		RWValue				mPageSetup;
		RWValue				mPageFormat;
		RWValue				mPrintSettings;
		RWValue				mDevMode;
		RWValue				mDeviceNames;
		RWValue				mPageSetupDialog;
		RWValue				mPrintDialog;
	
};



inline	void					SRReportData::SetReportWriter (SRReportWriter *inReportWriter)	{ mReportWriter = inReportWriter; }
inline	SRReportWriter	*		SRReportData::GetReportWriter (void) const						{ return mReportWriter; }
//inline	SRSectionList	*		SRReportData::GetSections (void)							{ return &mSections; }
inline	RWStyle			*		SRReportData::GetStyle (long inStyleID)	const					{ return mStyles.FindStyle (inStyleID); }
inline	bool					SRReportData::IsSimple (void)	const							{ return mSimple; }

inline	SRSectionList	*		SRReportData::GetPageSections (void)							{ return &mPageSections; }
inline	SRSectionList	*		SRReportData::GetBreakHeaders (void)							{ return &mBreakHeaders; }
inline	SRSectionList	*		SRReportData::GetBreakFooters (void)							{ return &mBreakFooters; }
inline	SRPageSection	*		SRReportData::GetBodySection (void)								{ return mBody; }
inline	SRWatermarkSection	*	SRReportData::GetWatermarkSection (void)						{ return mWatermark; }
inline	RWStyleList *			SRReportData::GetStyles (void)									{ return &mStyles; }
inline	long					SRReportData::GetLastStyle (void) const							{ return mLastStyle; }	//mbs 20052011

//inline	SBlob					SRReportData::GetPageSetup (void) const									{ return mPageSetup; }
//inline	SBlob					SRReportData::GetPageFormat (void) const								{ return mPageFormat; }
//inline	SBlob					SRReportData::GetPrintSettings (void) const								{ return mPrintSettings; }

inline	const PSObject::PSObjProps *	SRReportData::GetProperties (void) const						{ return sProperties; }

#endif

