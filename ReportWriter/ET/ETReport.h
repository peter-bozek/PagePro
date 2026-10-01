/*
 *  ETReport.h
 *  ReportWriter
 *
 *  Created by Peter Bozek on 12/11/2010.
 *  Copyright 2010INFORCE Bratislava. All rights reserved.
 *
 */

#ifndef	_ETReport_h_
# define	_ETReport_h_

# include	"ETReportData.h"
# include	"RWCalculator.h"
# include	"RWBaseTypes.h"
# include	"RWJson.h"

// forward declarations
class	RWDataSource;
class	ETObject;

enum e_OutputOptions {
	eo_xml			=	0x0200,
	eo_html			=	0x0400,
	eo_text			=	0x0800,
	eo_text_cvs		=	0x0010,
	eo_body			=	0x1000,
	eo_totals		=	0x2000,
	eo_headers		=	0x4000,
	eo_static		=	0x0020,
	eo_sortbyitem	=	0x0040,
	eo_json			=	0x8000,		// JSON export (used when none of text, HTML, XML is set)
	
	eo_last
};

enum e_OutputStage {
	es_reportstart		= 1,
	es_sectionstart		= 2,
	es_groupstart		= 4,
	es_groupend			= 8,
	es_sectionend		= 16,
	es_reportend		= 32,
	
	es_lastD
};

class	ETReport	:	public	RWCalcDataProvider
{
public:
					ETReport (RWDataSource &inDataSource, ETReportData &inData, long inFlags);
	virtual			~ETReport (void);
		
	inline			RWDataSource&	GetDataSource (void) const;
	inline	const	ETReportData&	GetReportData (void) const;
	
	inline			int				GetLastError(void) const;
	
	virtual		bool			GetCalculatedValue (const CText inName, RWValue &outVar) const;
	
	int				GetReportVariable (const CText inName) const;
	void			CreateVariable (const CText inName, ECalcType inCalc = ECalcType_None);
	bool			GetVariable (const CText inName, RWValue &outVar, ECalcType inCalc = ECalcType_None) const;
	void			SetVariable (const CText inName, RWValue *inVar);
	void			PositionObjects (ETObjList *inObjects);
	void			PositionGroup (ETGroup* inGroup);

	RWTextValue		FormatVariable (RWValue &inVar, const CText inFormat) const;
	
	// builds the export (XML, HTML, text, CSV, JSON) and writes it as UTF-8; false if the file could not be written
	bool			ReportToFile (const RWString &inPath);
	e_OutputOptions	GetOutputOptions (void);
	void			WriteText (const char *inType, void * object);
	void			WriteText (const char *inType, ETObject * object, RWTextValue &inText);

private:
	enum EFormat	{ eFormat_None, eFormat_Text, eFormat_HTML, eFormat_XML, eFormat_JSON };
	EFormat			GetFormat (void) const;
	void			JsonAddItem (const char *inType, ETObject *inObject, const RWString &inText, bool inAttributed);
	void			JsonCloseSection (void);

public:

private:
	void			DrawStaticReport (void);
	void			DrawStaticPage (ETPageSection *inBody);
	void			PrepareDynamicReport (void);
	bool			FindNextSection (void);
	bool			PeekNextSection (void);
	bool			GetNextSection (void);
	void			DrawDynamicReport (void);
	void			DrawDynamicPage (void);
		
	// writing methods
	
	// defensive programming - not implemented
					ETReport (void);
					ETReport (const ETReport &inOriginal);
					ETReport&	operator = (const ETReport &inOriginal);
	
private:
	RWDataSource 		&mSource;
	ETReportData 		&mData;
	e_OutputOptions		mOutputOptions;
	RWTextValue			mVarNames [RW_VarNamesRWCount];
	time_t				mPrintTime;
	
	e_OutputStage		mStage;

	// output, built in memory by WriteText and written once by ReportToFile
	RWString			mTextOut;			// text / HTML
	RWXmlDocument		mXmlOut;			// XML
	RWXmlNode			mXmlCurrent;		// element receiving XML output
	RWJsonDocument		mJsonOut;			// JSON
	RWJsonValue			mJsonSection;		// section being filled
	bool				mJsonSectionOpen;
	
	RWTextValue			mName;
	
	// for dynamic (iterated) reports
	ETPageSection	*	mPageSection;
	ETSectionList::const_iterator		mPageIterator;
	ETSection		*	mCurrentBody;
	bool				mFetchRecord;
	ETBreakSection	**	mBreakHeaders;
	RWCalculator		mCalculator;
	long				mBreakLevels;
	long				mBreakLevel;
	bool				mIsOverflow;
	int					mLastError;
};


inline			RWDataSource&	ETReport::GetDataSource (void) const		{ return mSource; }
inline	const	ETReportData&	ETReport::GetReportData (void) const		{ return mData; }

#endif
