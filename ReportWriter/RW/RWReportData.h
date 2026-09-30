#ifndef	_RWReportData_h_
# define	_RWReportData_h_

# include	"RWStyle.h"
# include	"RWObject.h"
# include	"RWSection.h"


// forward declarations
class	RWReportWriter;


// class for storing the pre-parsed report
class	RWReportData
{
friend class	RWReportWriter;
public:
    RWReportData (XMLDocument *inXML);
									~RWReportData (void);

inline			bool					IsDynamic (void) const;
inline			RWReportWriter	*       GetReportWriter (void) const;
        const	XMLElement	*		    GetReport (void) const;
        const   CText                   GetName (void) const;
inline			RWWatermarkSection*     GetWatermarkSection (void);
inline			RWSectionList	*		GetPageSections (void);
inline			RWSectionList	*		GetBodySections (void);
inline			RWStyle			*       GetStyle (long inID) const;

protected:
inline			void				SetReportWriter (RWReportWriter *inReportWriter);
				void				ParseReport (void);

private:
    void				ParseStyleSet (XMLElement *inStyleSet);
    void				ParseSection (XMLElement *inSection);
    void				ParseObjects (bool inKeepTogether, RWObjList *inParent, XMLElement *inObject);

			// defensive programming - not implemented
									RWReportData (void);
									RWReportData (const RWReportData &inOriginal);
				RWReportData	&	operator = (const RWReportData &inOriginal);

private:
		RWReportWriter	*       mReportWriter;
    XMLDocument 	*       mXML;
		RWTextValue             mName;

		RWStyleList             mStyles;
		RWWatermarkSection*     mWatermark;			// watermark
		RWSectionList			mPageSections;		// Headers & Footers
		RWSectionList			mBody;				// page(s) for static, breaks & bodies for dynamic
		bool					mIsDynamic;
//		int                     mCurrentDataID;
};



inline	bool						RWReportData::IsDynamic (void) const									{ return mIsDynamic; }
inline	void						RWReportData::SetReportWriter (RWReportWriter *inReportWriter)			{ mReportWriter = inReportWriter; }
inline	RWReportWriter	*           RWReportData::GetReportWriter (void) const								{ return mReportWriter; }
inline	RWStyle			*           RWReportData::GetStyle (long inStyleID)	const							{ return mStyles.FindStyle (inStyleID); }

inline	RWWatermarkSection	*       RWReportData::GetWatermarkSection (void)								{ return mWatermark; }
inline	RWSectionList	*			RWReportData::GetPageSections (void)									{ return &mPageSections; }
inline	RWSectionList	*			RWReportData::GetBodySections (void)									{ return &mBody; }

#endif
