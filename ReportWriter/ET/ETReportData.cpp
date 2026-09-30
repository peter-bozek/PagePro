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

ETReportData::ETReportData (XMLDocument *inXML)
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

const XMLElement*
ETReportData::GetReport (void)
const
{
	return mXML->RootElement();
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
	XMLNode		*report = mXML->RootElement();	// should be same as mXML->FirstChild ("Report");
	XMLNode		*node = NULL, *next;
    XMLElement	*elem;
	const char *	value;
	
	if (report)
	{
		elem = report->ToElement();
		if (elem && (value = elem->Attribute ("Version")) != NULL && TEXT_EQUALS (value, "1.0"))
		{
			node = report->FirstChildElement();
			if ((value = elem->Attribute ("Dynamic")) != NULL && atol (value) != 0)
				mIsDynamic = true;
			if ((value = elem->Attribute ("name")) != NULL)
				mName.FromXML (value);
			if ((value = elem->Attribute ("id")) != NULL)
				mID.FromXML (value);
		}
	}
	
	for ( ; node; node = next )
	{
		next = node->NextSibling();
		elem = node->ToElement();
		if (elem == NULL)
		{
			//			report->RemoveChild (node);
			continue;
		}
		
		value = elem->Value();
		if (strcasecmp (value, "StyleSet") == 0)
		{
			ParseStyleSet (elem);
		}
		else if (strcasecmp (value, "Header") == 0 || strcasecmp (value, "Page") == 0 || strcasecmp (value, "Footer") == 0)
			ParseSection (elem);
		else if (strcasecmp (value, "Watermark") == 0)
			ParseSection (elem);
		else if (strcasecmp (value, "BreakHeader") == 0 || strcasecmp (value, "BreakFooter") == 0)
		{
			ParseSection (elem);
		}
		else
		{
			//			report->RemoveChild (node);
			continue;
		}
	}
	
	return;
}


// ---------------------------------------------------------------------------
// ParseStyleSet													 [private]
// ---------------------------------------------------------------------------

void
ETReportData::ParseStyleSet (XMLElement *inStyleSet)
{
	XMLNode	*node;
	RWStyle		*style;
	
	for ( node = inStyleSet->FirstChildElement(); node; node = node->NextSibling() )
	{
        XMLElement	*elem = node->ToElement();
		if (elem == NULL)
			continue;
		if (!strcasecmp (elem->Value(), "Style") == 0)
			continue;
		
		style = new RWStyle (&mStyles, elem);
		mStyles.insert (pair<long,RWStyle*> (style->GetID (), style));
	}
	
	//mbs 23122009	create default only if not present
	if (mStyles.FindStyle (0) == NULL || mStyles.FindStyle (0)->GetID() != 0)
	{
		style = new RWStyle (&mStyles, NULL);
		mStyles.insert (pair<long,RWStyle*> (0, style));	// add default style
	}
	
	return;
}


// ---------------------------------------------------------------------------
// ParseSection														 [private]
// ---------------------------------------------------------------------------

void
ETReportData::ParseSection (XMLElement *inSection)
{
	if (mIsDynamic)
	{
		if (strcasecmp (inSection->Value(), "Page") == 0)
		{
			ETPageSection	*pageSection = new ETPageSection;
			pageSection->Parse (this, inSection);
			mBody.push_back (pageSection);
			
			XMLNode	*node, *next;
			
			for ( node = inSection->FirstChildElement(); node; node = next )
			{
				next = node->NextSibling();
                XMLElement	*elem = node->ToElement();
				if (elem == NULL)
				{
					continue;
				}
				
				const CXMLText	value = elem->Value();
				
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
		if (strcasecmp (inSection->Value(), "Page") == 0)
		{
			ETPageSection	*pageSection = new ETPageSection;
			pageSection->Parse (this, inSection);
			mBody.push_back (pageSection);
			ParseObjects (pageSection->GetObjects(), inSection);
		}
		else if (strcasecmp (inSection->Value(), "Header") == 0 || strcasecmp (inSection->Value(), "Footer") == 0)
		{
			ETHeaderFooterSection	*headerFooter = new ETHeaderFooterSection (inSection->Value());
			headerFooter->Parse (this, inSection);
			mPageSections.push_back (headerFooter);
			ParseObjects (headerFooter->GetObjects(), inSection);
		}
		else if (strcasecmp (inSection->Value(), "Watermark") == 0)
		{
			if (mWatermark == NULL)
			{
				mWatermark = new ETWatermarkSection (inSection->Value());
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
ETReportData::ParseObjects (ETObjList *inParent, XMLElement *inObject)
{
	XMLNode	*node, *next;
	ETObject	*obj;
	int			seqID = 0;
	
	for ( node = inObject->FirstChildElement(); node; node = next )
	{
		next = node->NextSibling();
        XMLElement	*elem = node->ToElement();
		if (elem == NULL)
		{
			//			inObject->RemoveChild (node);
			continue;
		}
		
		obj = NULL;
		const CXMLText	value = elem->Value();
		
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
		{
			obj = ETText::Create (this, elem, ++seqID);
		}
		else if (STR_EQUALS (value, "Table"))
		{
//mbs 05112010	TODO: implement...
//			obj = ETTable::Create (this, elem, ++seqID);
		}
		
		else if (STR_EQUALS (value, "Var") || STR_EQUALS (value, "Variable"))
		{
			obj = ETVariable::Create (this, elem, ++seqID);
		}
		if (obj != NULL)
		{
			inParent->push_back (obj);
		}
	}
	
	// order by top/left coordinates
	std::sort<ETObjList::iterator, ETObjectComparePosition> (inParent->begin(), inParent->end(), ETObjectComparePosition());
	
# if	_4D_Package_
	Yield4D();
# endif
	
	return;
}
