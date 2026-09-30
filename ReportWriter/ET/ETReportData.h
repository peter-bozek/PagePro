/*
 *  ETReportData.h
 *  ReportWriter
 *
 *  Created by Peter Bozek on 12/11/2010.
 *  Copyright 2010 INFORCE Bratislava. All rights reserved.
 *
 */

#ifndef	_ETReportData_h_
# define	_ETReportData_h_

# include	"RWStyle.h"
# include	"ETObject.h"
# include	"ETSection.h"

// forward declarations
class	ETReport;


// class for storing the pre-parsed report
class	ETReportData
{
	friend class	ETReport;
public:
					ETReportData (RWXmlDocument *inXML);
					~ETReportData (void);
	
	inline			bool				IsDynamic (void) const;
	inline			ETReport		*	GetReportWriter (void) const;
					RWXmlNode			GetReport (void) const;
	const           CText				GetName (void) const;
	inline			ETWatermarkSection*	GetWatermarkSection (void);
	inline			ETSectionList	*	GetPageSections (void);
	inline			ETSectionList	*	GetBodySections (void);
	inline			RWStyle			*	GetStyle (long inID) const;
	
protected:
	inline			void				SetReportWriter (ETReport *inReportWriter);	
					void				ParseReport (void);
	
private:
	void				ParseStyleSet (XMLElement *inStyleSet);
	void				ParseSection (XMLElement *inSection);
	void				ParseObjects (ETObjList *inParent, XMLElement *inObject);
	
	// defensive programming - not implemented
						ETReportData (void);
						ETReportData (const ETReportData &inOriginal);
						ETReportData	&	operator = (const ETReportData &inOriginal);
	
private:
	ETReport	*		mReportWriter;
	RWXmlDocument	*	mXML;
	RWTextValue			mName;
	RWTextValue			mID;
	
	RWStyleList			mStyles;
	ETWatermarkSection*	mWatermark;			// watermark
	ETSectionList		mPageSections;		// Headers & Footers
	ETSectionList		mBody;				// page(s) for static, breaks & bodies for dynamic
	bool				mIsDynamic;
	//		int					mCurrentDataID;
};



inline	bool					ETReportData::IsDynamic (void) const									{ return mIsDynamic; }
inline	void					ETReportData::SetReportWriter (ETReport *inReportWriter)			{ mReportWriter = inReportWriter; }
inline	ETReport	*			ETReportData::GetReportWriter (void) const								{ return mReportWriter; }
inline	RWStyle			*		ETReportData::GetStyle (long inStyleID)	const							{ return mStyles.FindStyle (inStyleID); }

inline	ETWatermarkSection	*	ETReportData::GetWatermarkSection (void)								{ return mWatermark; }
inline	ETSectionList	*		ETReportData::GetPageSections (void)									{ return &mPageSections; }
inline	ETSectionList	*		ETReportData::GetBodySections (void)									{ return &mBody; }

#endif
