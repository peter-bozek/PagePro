/*
 *  ETSection.h
 *  ReportWriter
 *
 *  Created by Peter Bozek on 12/11/2010.
 *  Copyright 2010 INFORCE Bratislava. All rights reserved.
 *
 */

#ifndef	_ETSection_h_
# define	_ETSection_h_

# include	"ETObject.h"


// forward declarations
class	ETReport;

class	ETSection
{
public:
	enum	ESection_Kind
	{
		eSectionKind_Page	= 0,
		eSectionKind_Header,
		eSectionKind_BreakHeader,
		eSectionKind_Body,
		eSectionKind_BreakFooter,
		eSectionKind_FillFooter,
		eSectionKind_Footer,
		eSectionKind_Watermark
	};
	enum	EPageThrow
	{
		ePageThrow_None = 0,
		ePageThrow_Before,
		ePageThrow_After
	};
	
								ETSection (RWStringView inKind);
								ETSection (ESection_Kind inKind);
	virtual						~ETSection (void);
	
	virtual		void			Parse (ETReportData *inReport, RWXmlNode inNode);
	virtual		bool			WillingToPrint (ETReport *inWriter) const;
	virtual		void			PositionObjects (ETReport *inWriter);
	
	ETObjList		*	GetObjects (void);
	ESection_Kind		GetKind (void) const;
	void				FetchCalcValues (ETReport *inWriter);
	void				Export (ETReport *inWriter);
    inline	RWString		GetType();
    inline	RWString		GetName();
    inline	RWString		GetID();

private:
	// defensive programming - not implemented
						ETSection (const ETSection &inOriginal);
						ETSection		&	operator = (const ETSection &inOriginal);
	
protected:
	ESection_Kind		mKind;
	ETObjList			mObjects;
	RWString			mName;
	RWString			mID;
    RWString         mType;
	bool				mDraw;
};
inline	RWString		ETSection::GetType()		{return mType;}
inline	RWString		ETSection::GetName()		{return mName;}
inline	RWString		ETSection::GetID()          {return mID;}


class	ETHeaderFooterSection
:	public	ETSection
{
public:
									ETHeaderFooterSection (RWStringView inKind);
	virtual							~ETHeaderFooterSection (void);
	
	virtual		void				Parse (ETReportData *inReport, RWXmlNode inNode) override;
//	virtual		bool				WillingToPrint (ETReport *inWriter) const;
	
//	virtual		void				PositionObjects (ETReport *inWriter);
	
private:
	// defensive programming - not implemented
	ETHeaderFooterSection (const ETHeaderFooterSection &inOriginal);
	ETHeaderFooterSection&	operator = (const ETHeaderFooterSection &inOriginal);
	
protected:
};


// Break Header, Break Footer
class	ETBreakSection
:	public	ETSection
{
public:
									ETBreakSection(RWStringView inKind);
	virtual							~ETBreakSection (void);
	
	virtual		void				Parse (ETReportData *inReport, RWXmlNode inNode) override;
	virtual		bool				WillingToPrint (ETReport *inWriter) const;
	
	int								GetLevel (void) const;
	void							ProcessBreak (long inBreakLevel);
	
private:
	// defensive programming - not implemented
									ETBreakSection (const ETBreakSection &inOriginal);
									ETBreakSection	&	operator = (const ETBreakSection &inOriginal);
	
protected:
	int					mLevel;
	bool				mIsBreak;
};


// Page
class	ETPageSection
:	public	ETSection
{
public:
									ETPageSection(void);
	virtual							~ETPageSection (void);
	
	virtual		void				Parse (ETReportData *inReport, RWXmlNode inNode) override;
	
private:
	// defensive programming - not implemented
									ETPageSection (const ETPageSection &inOriginal);
									ETPageSection	&	operator = (const ETPageSection &inOriginal);
	
protected:
};


// Watermark
class	ETWatermarkSection
:	public	ETHeaderFooterSection
{
public:
									ETWatermarkSection(RWStringView inKind);
	virtual							~ETWatermarkSection (void);
	
	virtual		void				Parse (ETReportData *inReport, RWXmlNode inNode) override;
	bool							IsOnTop (void) const;
	
private:
	// defensive programming - not implemented
									ETWatermarkSection (const ETWatermarkSection &inOriginal);
									ETWatermarkSection	&	operator = (const ETWatermarkSection &inOriginal);
	
protected:
	bool				mOnTop;
};


typedef	RWList<ETBreakSection*>	ETBreakSectionList;	// no destructor for objects deletion
typedef	RWArray<ETSection*>		ETSectionList;

#endif
