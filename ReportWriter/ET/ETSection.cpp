/*
 *  ETSection.cpp
 *  ReportWriter
 *
 *  Created by Peter Bozek on 12/11/2010.
 *  Copyright 2010 INFORCE Bratislava. All rights reserved.
 *
 */

# include	"ETSection.h"
# include	"ETReport.h"


// ---------------------------------------------------------------------------
// ETSection								Constructor				  [public]
// ---------------------------------------------------------------------------

ETSection::ETSection (RWStringView inKind)
:	mKind (eSectionKind_Body), mType (u"Body"),
mDraw (true)
{
    if (STR_EQUALS (inKind, "Header")) {
		mKind = eSectionKind_Header;
        mType = u"Header";
    }
    else if (STR_EQUALS (inKind, "BreakHeader")) {
		mKind = eSectionKind_BreakHeader;
        mType = u"BreakHeader";
    }
//    else if (STR_EQUALS (inKind, "Body")) 
//			mKind = eSectionKind_Body;
    else if (STR_EQUALS (inKind, "BreakFooter")) {
		mKind = eSectionKind_BreakFooter;
        mType = u"BreakFooter";
    }
	//	else if (STR_EQUALS (inKind, "FillFooter"))
	//		mKind = eSectionKind_FillFooter;
    else if (STR_EQUALS (inKind, "Footer")) {
		mKind = eSectionKind_Footer;
        mType = u"Footer";
    }
	//	else if (STR_EQUALS (inKind, "Page"))
	//		mKind = eSectionKind_Page;
    else if (STR_EQUALS (inKind, "Watermark")) {
		mKind = eSectionKind_Watermark;
        mType = u"Watermark";
    }
}

ETSection::ETSection (ESection_Kind inKind)
:	mKind (inKind),
mDraw (true)
{
}


// ---------------------------------------------------------------------------
// ~ETSection								Destructor				  [public]
// ---------------------------------------------------------------------------

ETSection::~ETSection (void)
{
	return;
}


// ---------------------------------------------------------------------------
// GetKind															  [public]
// ---------------------------------------------------------------------------

ETSection::ESection_Kind
ETSection::GetKind (void)
const
{
	return mKind;
}


// ---------------------------------------------------------------------------
// GetObjects														  [public]
// ---------------------------------------------------------------------------

ETObjList *
ETSection::GetObjects (void)
{
	return &mObjects;
}

// ---------------------------------------------------------------------------
// FetchCalcValues													  [public]
// ---------------------------------------------------------------------------

void
ETSection::FetchCalcValues (ETReport *inWriter)
{
	ETObjList::const_iterator	it;
	
	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		ETObject	*obj = *it;
		obj->FetchCalcValue (inWriter);
	}
	
	return;
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
ETSection::Parse (ETReportData * /*inReport*/, RWXmlNode inNode)
{
	long			lVal;
	
	for (const auto &[name, value] : inNode.Attributes())
	{
		
		if (STR_EQUALS (name, "draw"))
		{
			lVal = 0;
			RWStr::ReadNumber (value, lVal);
			mDraw = (lVal != 0);
		}
		//mbs 15112010
		else if (STR_EQUALS (name, "name"))
			mName.FromXML (value);
		else if (STR_EQUALS (name, "id"))
			mID.FromXML (value);
	}
	
	return;
}


// ---------------------------------------------------------------------------
// WillingToPrint													  [public]
// ---------------------------------------------------------------------------

bool
ETSection::WillingToPrint (ETReport *inWriter)
const
{	
	return mDraw;
}


// ---------------------------------------------------------------------------
// PositionObjects													  [public]
// ---------------------------------------------------------------------------
// position all objects in a section

void
ETSection::PositionObjects (ETReport *inWriter)
{
	
	inWriter->PositionObjects (&mObjects);
	
	return;
}


// ---------------------------------------------------------------------------
// Export																  [public]
// ---------------------------------------------------------------------------

void
ETSection::Export (ETReport * inWriter)
{
	inWriter->WriteText ("SectionStart", this);

	ETObjList::const_iterator	it;
	
	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		ETObject	*obj = *it;
		if (obj->WillingToPrint ())
		{
			if (it != mObjects.begin())
				inWriter->WriteText ("Delimiter", this);
			obj->Export ();
		}
	}
	inWriter->WriteText ("SectionEnd", this);
	
	return;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// ETHeaderFooterSection					Constructor				  [public]
// ---------------------------------------------------------------------------

ETHeaderFooterSection::ETHeaderFooterSection (RWStringView inKind)
:	ETSection (inKind)
{
}


// ---------------------------------------------------------------------------
// ~ETHeaderFooterSection					Destructor				  [public]
// ---------------------------------------------------------------------------

ETHeaderFooterSection::~ETHeaderFooterSection (void)
{
}

// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
ETHeaderFooterSection::Parse (ETReportData *inReport, RWXmlNode inNode)
{
	ETSection::Parse (inReport, inNode);
	
	return;
}

#pragma	mark	-

// ---------------------------------------------------------------------------
// ETBreakSection							Constructor				  [public]
// ---------------------------------------------------------------------------
ETBreakSection::ETBreakSection (RWStringView inKind)
:	ETSection (inKind),
mLevel (0),
mIsBreak (true)
{
}


// ---------------------------------------------------------------------------
// ~ETBreakSection							Destructor				  [public]
// ---------------------------------------------------------------------------

ETBreakSection::~ETBreakSection (void)
{
	return;
}



// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
ETBreakSection::Parse (ETReportData *inReport, RWXmlNode inNode)
{
	ETSection::Parse (inReport, inNode);
	
	
	for (const auto &[name, value] : inNode.Attributes())
	{
		
		if (STR_EQUALS (name, "level"))
		{
			mLevel = 0;
			RWStr::ReadNumber (value, mLevel);
		}
	}
	
	return;
}


// ---------------------------------------------------------------------------
// WillingToPrint													  [public]
// ---------------------------------------------------------------------------

bool
ETBreakSection::WillingToPrint (ETReport *inWriter)
const
{
	bool	printIt = mDraw || mIsBreak;
	
	return printIt;
}


// ---------------------------------------------------------------------------
// GetLevel															  [public]
// ---------------------------------------------------------------------------

int
ETBreakSection::GetLevel (void)
const
{
	return mLevel;
}



// ---------------------------------------------------------------------------
// ProcessBreak														  [public]
// ---------------------------------------------------------------------------

void
ETBreakSection::ProcessBreak (long inBreakLevel)
{
	mIsBreak = inBreakLevel <= mLevel;
	return;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// ETPageSection							Constructor				  [public]
// ---------------------------------------------------------------------------
ETPageSection::ETPageSection (void)
:	ETSection (eSectionKind_Page)
{
}


// ---------------------------------------------------------------------------
// ~ETPageSection							Destructor				  [public]
// ---------------------------------------------------------------------------

ETPageSection::~ETPageSection (void)
{
	
	return;
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
ETPageSection::Parse (ETReportData *inReport, RWXmlNode inNode)
{
	ETSection::Parse (inReport, inNode);
		
	return;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// ETWatermarkSection						Constructor				  [public]
// ---------------------------------------------------------------------------

ETWatermarkSection::ETWatermarkSection (RWStringView inKind)
:	ETHeaderFooterSection (inKind),
mOnTop (false)
{
}


// ---------------------------------------------------------------------------
// ~ETWatermarkSection						Destructor				  [public]
// ---------------------------------------------------------------------------

ETWatermarkSection::~ETWatermarkSection (void)
{
}


// ---------------------------------------------------------------------------
// Parse															  [public]
// ---------------------------------------------------------------------------

void
ETWatermarkSection::Parse (ETReportData *inReport, RWXmlNode inNode)
{
	ETHeaderFooterSection::Parse (inReport, inNode);
	
	const CXMLText	value = inNode.Attr (u"onTop");
	
	if (!value.empty())
	{
		long			lVal = 0;
		
		RWStr::ReadNumber (value, lVal);
		mOnTop = (lVal != 0);
	}
		
	return;
}


// ---------------------------------------------------------------------------
// IsOnTop															  [public]
// ---------------------------------------------------------------------------

bool
ETWatermarkSection::IsOnTop (void)
const
{
	return mOnTop;
}
