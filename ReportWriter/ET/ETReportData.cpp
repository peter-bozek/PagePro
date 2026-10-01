/*
 *  ETReportData.cpp
 *  ReportWriter
 *
 *  Created by Peter Bozek on 12/11/2010.
 *  Copyright 2010 INFORCE Bratislava. All rights reserved.
 *
 */

#include "ETReportData.h"

# include	<algorithm>

# if	_4D_Package_
extern	"C"		void Yield4D (void);
# endif


// ---------------------------------------------------------------------------
// ETReportData								Constructor				  [public]
// ---------------------------------------------------------------------------

ETReportData::ETReportData (RWXmlDocument *inXML)
:	mReportWriter (0),
mXML (inXML),
mWatermark (0),
mIsDynamic (false)
{
}


// ---------------------------------------------------------------------------
// ~ETReportData							Destructor				  [public]
// ---------------------------------------------------------------------------

ETReportData::~ETReportData (void)
{
	mName.Free();
	if (mWatermark)
		delete mWatermark;
}


// ---------------------------------------------------------------------------
// GetReport														  [public]
// ---------------------------------------------------------------------------

RWXmlNode
ETReportData::GetReport (void)
const
{
	return mXML->Root();
}


// ---------------------------------------------------------------------------
// GetName															  [public]
// ---------------------------------------------------------------------------

const CText
ETReportData::GetName (void)
const
{
	return mName;
}


// ---------------------------------------------------------------------------
// ParseReport														  [public]
// ---------------------------------------------------------------------------

void
ETReportData::ParseReport (void)
{
	RWXmlNode	report = mXML->Root();	// "Report"

	if (!report || !RWStr::Equals (report.Attr (u"Version"), "1.0"))
		return;

	if (report.AttrInt (u"Dynamic", 0) != 0)
		mIsDynamic = true;
	if (report.HasAttr (u"name"))
		mName = report.Attr (u"name");
	if (report.HasAttr (u"id"))
		mID = report.Attr (u"id");

	for (RWXmlNode elem : report.Children())
	{
		const RWString	value = elem.Name();
		if (STR_EQUALS (value, "StyleSet"))
			ParseStyleSet (elem);
		else if (STR_EQUALS (value, "Header") || STR_EQUALS (value, "Page") || STR_EQUALS (value, "Footer"))
			ParseSection (elem);
		else if (STR_EQUALS (value, "Watermark"))
			ParseSection (elem);
		else if (STR_EQUALS (value, "BreakHeader") || STR_EQUALS (value, "BreakFooter"))
			ParseSection (elem);
	}

	return;
}


// ---------------------------------------------------------------------------
// ParseStyleSet													 [private]
// ---------------------------------------------------------------------------

void
ETReportData::ParseStyleSet (RWXmlNode inStyleSet)
{
	RWStyle		*style;

	for (RWXmlNode elem : inStyleSet.Children())
	{
		if (!STR_EQUALS (elem.Name(), "Style"))
			continue;

		style = new RWStyle (&mStyles, elem);
		mStyles.insert (pair<long,RWStyle*> (style->GetID (), style));
	}

	//mbs 23122009	create default only if not present
	if (mStyles.FindStyle (0) == NULL || mStyles.FindStyle (0)->GetID() != 0)
	{
		style = new RWStyle (&mStyles, RWXmlNode());
		mStyles.insert (pair<long,RWStyle*> (0, style));	// add default style
	}

	return;
}


// ---------------------------------------------------------------------------
// ParseSection														 [private]
// ---------------------------------------------------------------------------

void
ETReportData::ParseSection (RWXmlNode inSection)
{
	const RWString	sectionName = inSection.Name();

	if (mIsDynamic)
	{
		if (STR_EQUALS (sectionName, "Page"))
		{
			ETPageSection	*pageSection = new ETPageSection;
			pageSection->Parse (this, inSection);
			mBody.push_back (pageSection);

			for (RWXmlNode elem : inSection.Children())
			{
				const RWString	value = elem.Name();

				if (STR_EQUALS (value, "Body"))
				{
					ETSection	*body = new ETSection (ETSection::eSectionKind_Body);
					body->Parse (this, elem);
					mBody.push_back (body);
					ParseObjects (body->GetObjects(), elem);
				}
				else if (STR_STARTS_WITH (value, "Break"))
				{
					ETBreakSection	*breakLevel = new ETBreakSection (value);
					breakLevel->Parse (this, elem);
					mBody.push_back (breakLevel);
					ParseObjects (breakLevel->GetObjects(), elem);
				}
				else if (STR_EQUALS (value, "Header") || STR_EQUALS (value, "Footer"))
				{
					ETHeaderFooterSection	*headerFooter = new ETHeaderFooterSection (value);
					headerFooter->Parse (this, elem);
					mPageSections.push_back (headerFooter);
					ParseObjects (headerFooter->GetObjects(), elem);
				}
				else if (STR_EQUALS (value, "Watermark"))
				{
					if (mWatermark == NULL)
					{
						mWatermark = new ETWatermarkSection (value);
						mWatermark->Parse (this, elem);
						ParseObjects (mWatermark->GetObjects(), elem);
					}
				}
			}
		}
	}
	else
	{
		if (STR_EQUALS (sectionName, "Page"))
		{
			ETPageSection	*pageSection = new ETPageSection;
			pageSection->Parse (this, inSection);
			mBody.push_back (pageSection);
			ParseObjects (pageSection->GetObjects(), inSection);
		}
		else if (STR_EQUALS (sectionName, "Header") || STR_EQUALS (sectionName, "Footer"))
		{
			ETHeaderFooterSection	*headerFooter = new ETHeaderFooterSection (sectionName);
			headerFooter->Parse (this, inSection);
			mPageSections.push_back (headerFooter);
			ParseObjects (headerFooter->GetObjects(), inSection);
		}
		else if (STR_EQUALS (sectionName, "Watermark"))
		{
			if (mWatermark == NULL)
			{
				mWatermark = new ETWatermarkSection (sectionName);
				mWatermark->Parse (this, inSection);
				ParseObjects (mWatermark->GetObjects(), inSection);
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// ParseObjects														 [private]
// ---------------------------------------------------------------------------

void
ETReportData::ParseObjects (ETObjList *inParent, RWXmlNode inObject)
{
	ETObject	*obj;
	int			seqID = 0;

	for (RWXmlNode elem : inObject.Children())
	{
		obj = NULL;
		const RWString	value = elem.Name();

		if (STR_EQUALS (value, "Group"))
		{
			ETGroup	*group = ETGroup::Create (this, elem, ++seqID);
			if (group != NULL)
			{
				obj = group;
				ParseObjects (group->GetObjects(), elem);	// pB changed: Group must always fit on a single page...
			}
		}
		else if (STR_EQUALS (value, "Text"))
			obj = ETText::Create (this, elem, ++seqID);
		else if (STR_EQUALS (value, "Table"))
		{
//mbs 05112010	TODO: implement...
//			obj = ETTable::Create (this, elem, ++seqID);
		}
		else if (STR_EQUALS (value, "Var") || STR_EQUALS (value, "Variable"))
			obj = ETVariable::Create (this, elem, ++seqID);

		if (obj != NULL)
			inParent->push_back (obj);
	}

	// order by top/left coordinates
	std::sort<ETObjList::iterator, ETObjectComparePosition> (inParent->begin(), inParent->end(), ETObjectComparePosition());
	
# if	_4D_Package_
	Yield4D();
# endif
	
	return;
}
