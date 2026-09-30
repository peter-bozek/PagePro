# include	"RWReportData.h"
# include	"RWReportWriter.h"
# include	<algorithm>

# if	_4D_Package_
extern	"C"		void Yield4D (void);
# endif


// ---------------------------------------------------------------------------
// RWReportData								Constructor				  [public]
// ---------------------------------------------------------------------------

RWReportData::RWReportData (XMLDocument *inXML)
	:	mReportWriter (0),
		mXML (inXML),
//		mName (0),
//		mCurrentDataID (0),
		mWatermark (0),
		mIsDynamic (false)
{
}


// ---------------------------------------------------------------------------
// ~RWReportData							Destructor				  [public]
// ---------------------------------------------------------------------------

RWReportData::~RWReportData (void)
{
	mName.Free();
	if (mWatermark)
		delete mWatermark;
}


// ---------------------------------------------------------------------------
// GetReport														  [public]
// ---------------------------------------------------------------------------

const XMLElement*
RWReportData::GetReport (void)
const
{
//	return mXML->FirstChild ("Report")->ToElement();
	return mXML->RootElement();
}


// ---------------------------------------------------------------------------
// GetName															  [public]
// ---------------------------------------------------------------------------

const CText
RWReportData::GetName (void)
const
{
	return mName;
}


// ---------------------------------------------------------------------------
// ParseReport														  [public]
// ---------------------------------------------------------------------------

void
RWReportData::ParseReport (void)
{
    XMLNode		*report = mXML->RootElement();	// should be same as mXML->FirstChild ("Report");
    XMLNode		*node = NULL, *next;
    XMLElement	*elem;
	CXMLText	value;

	if (report)
	{
		elem = report->ToElement();
        if (elem && (value = elem->Attribute ("Version")).length() > 0 &&  (value.compare( "1.0") == 0))
		{
			node = report->FirstChildElement();
			if ((value = elem->Attribute ("Dynamic")).length() > 0 && std::stoi (value) != 0)
				mIsDynamic = true;
			if ((value = elem->Attribute ("name")).length() > 0)
				mName.FromXML (value);
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
		if (value.compare( "StyleSet") == 0)
		{
			ParseStyleSet (elem);
//			report->RemoveChild (node);
		}
		else if (value.compare( "Header") == 0 || value.compare("Page") == 0  || value.compare("Footer") == 0 )
			ParseSection (elem);
		else if (STR_EQUALS (value, "Watermark"))
			ParseSection (elem);
		else if (value.compare("BreakHeader") == 0  || value.compare("BreakFooter") == 0 )
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
RWReportData::ParseStyleSet (XMLElement *inStyleSet)
{
    XMLNode	*node;
	RWStyle		*style;

	for ( node = inStyleSet->FirstChildElement(); node; node = node->NextSibling() )
	{
        XMLElement	*elem = node->ToElement();
		if (elem == NULL)
			continue;
        if ( strcasecmp(elem->Value(), "Style") != 0)
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
RWReportData::ParseSection (XMLElement *inSection)
{
	if (mIsDynamic)
	{
		if (strcasecmp (inSection->Value(), "Page") == 0)
		{
			RWPageSection	*pageSection = new RWPageSection;
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

				if (strcasecmp (value.c_str(), "Body") == 0)
				{
					RWSection	*body = new RWSection (RWSection::eSectionKind_Body);
					body->Parse (this, elem);
					mBody.push_back (body);
					ParseObjects (body->GetKeepTogether(), body->GetObjects(), elem);
				}
				else if (strncasecmp (value.c_str(), "Break", strlen("Break")) == 0)
				{
					RWBreakSection	*breakLevel = new RWBreakSection (value);
					breakLevel->Parse (this, elem);
					mBody.push_back (breakLevel);
					ParseObjects (breakLevel->GetKeepTogether(), breakLevel->GetObjects(), elem);
				}
				else if (strcasecmp (value.c_str(), "Header") == 0 || strcasecmp (value.c_str(), "Footer") == 0)
				{
					RWHeaderFooterSection	*headerFooter = new RWHeaderFooterSection (value);
					headerFooter->Parse (this, elem);
					mPageSections.push_back (headerFooter);
					ParseObjects (headerFooter->GetKeepTogether(), headerFooter->GetObjects(), elem);
				}
				else if (strcasecmp (value.c_str(), "Watermark") == 0)
				{
					if (mWatermark == NULL)
					{
						mWatermark = new RWWatermarkSection (value);
						mWatermark->Parse (this, elem);
						ParseObjects (mWatermark->GetKeepTogether(), mWatermark->GetObjects(), elem);
					}
				}
			}
		}
	}
	else
	{
		if (STR_EQUALS (inSection->Value(), "Page"))
		{
			RWPageSection	*pageSection = new RWPageSection;
			pageSection->Parse (this, inSection);
			mBody.push_back (pageSection);
			ParseObjects (pageSection->GetKeepTogether(), pageSection->GetObjects(), inSection);
		}
/*
		else if (STR_EQUALS (inSection->Value(), "BreakHeader"))
		{
			RWBreakSection	*breakSection = new RWBreakSection (inSection->Value());
			breakSection->Parse (this, inSection);
			mBreakHeaders.push_back (breakSection);
			ParseObjects (breakSection->GetKeepTogether(), breakSection->GetObjects(), inSection);
		}
		else if (STR_EQUALS (inSection->Value(), "BreakFooter"))
		{
			RWBreakSection	*breakSection = new RWBreakSection (inSection->Value());
			breakSection->Parse (this, inSection);
			mBreakFooters.push_back (breakSection);
			ParseObjects (breakSection->GetKeepTogether(), breakSection->GetObjects(), inSection);
		}
*/
		else if (STR_EQUALS (inSection->Value(), "Header") || STR_EQUALS (inSection->Value(), "Footer"))
		{
			RWHeaderFooterSection	*headerFooter = new RWHeaderFooterSection (inSection->Value());
			headerFooter->Parse (this, inSection);
			mPageSections.push_back (headerFooter);
			ParseObjects (headerFooter->GetKeepTogether(), headerFooter->GetObjects(), inSection);
		}
		else if (STR_EQUALS (inSection->Value(), "Watermark"))
		{
			if (mWatermark == NULL)
			{
				mWatermark = new RWWatermarkSection (inSection->Value());
				mWatermark->Parse (this, inSection);
				ParseObjects (mWatermark->GetKeepTogether(), mWatermark->GetObjects(), inSection);
			}
		}
	}

//	XMLElement	*elem = inSection->FirstChildElement();
//	if (elem && STR_EQUALS (elem->Value(), "Objects"))
//	{
//		ParseObjects (section->GetKeepTogether(), section->GetObjects(), elem);
//	}
//	else if (elem)
//		inSection->RemoveChild (elem);

	return;
}


// ---------------------------------------------------------------------------
// ParseObjects														 [private]
// ---------------------------------------------------------------------------

void
RWReportData::ParseObjects (bool inKeepTogether, RWObjList *inParent, XMLElement *inObject)
{
	XMLNode	*node, *next;
	RWObject	*obj;
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
			RWGroup	*group = RWGroup::Create (this, elem, ++seqID);
			if (group != NULL)
			{
				obj = group;
				ParseObjects (inKeepTogether, group->GetObjects(), elem);	// pB changed: Group must always fit on a single page...
			}
		}
#if OLD_RW_FORMAT
		else if (STR_EQUALS (value, "Data"))
		{
#if 1
			obj = RWTable::Create (this, elem, ++seqID /* , ++mCurrentDataID */);
#else
			value = mReportWriter->GetVariable (++mCurrentDataID);
			if (value == NULL)	// it is a table
				obj = RWTable::Create (this, elem, ++seqID, mCurrentDataID);
			else
			{
				RWVariable	*var = RWVariable::Create (this, elem, ++seqID);
				obj = var;
				if (var != NULL)
					var->SetVariableText (value);
			}
#endif
		}
#endif
		else if (STR_EQUALS (value, "Text"))
		{
			obj = RWText::Create (this, elem, ++seqID);
		}
		else if (STR_EQUALS (value, "Table"))
		{
			obj = RWTable::Create (this, elem, ++seqID);
		}
		else if (STR_EQUALS (value, "Line"))
		{
			obj = RWLine::Create (this, elem, ++seqID);
		}
		else if (STR_EQUALS (value, "Rect") || STR_EQUALS (value, "Rectangle"))
		{
			obj = RWRect::Create (this, elem, ++seqID);
		}
		else if (STR_EQUALS (value, "Oval"))
		{
			obj = RWOval::Create (this, elem, ++seqID);
		}
		else if (STR_EQUALS (value, "Var") || STR_EQUALS (value, "Variable"))
		{
			obj = RWVariable::Create (this, elem, ++seqID);
		}
		else if (STR_EQUALS (value, "Pict") || STR_EQUALS (value, "Picture"))
		{
			obj = RWPict::Create (this, elem, ++seqID);
		}
		else
		{
#if OLD_RW_FORMAT
			if (!STR_EQUALS (value, "Object"))
			{
//				inObject->RemoveChild (node);
				continue;
			}

			const CXMLText	type = elem->Attribute ("Type");
			if (type == NULL)
			{
//				inObject->RemoveChild (node);
				continue;
			}

			if (STR_EQUALS (type, "Line"))
				obj = RWLine::Create (this, elem, ++seqID);
			else if (STR_EQUALS (type, "Rect") || STR_EQUALS (type, "Rectangle"))
				obj = RWRect::Create (this, elem, ++seqID);
			else if (STR_EQUALS (type, "Oval"))
				obj = RWOval::Create (this, elem, ++seqID);
			else if (STR_EQUALS (type, "Pict") || STR_EQUALS (type, "Picture"))
				obj = RWPict::Create (this, elem, ++seqID);
			else if (STR_EQUALS (type, "Text"))
				obj = RWText::Create (this, elem, ++seqID);
			else if (STR_EQUALS (type, "Var") || STR_EQUALS (type, "Variable"))
				obj = RWVariable::Create (this, elem, ++seqID);
			else if (STR_EQUALS (type, "Table"))
				obj = RWTable::Create (this, elem, ++seqID /* , 0 */ );
//			else
#endif
//			{
//				inObject->RemoveChild (node);
//			}
		}
		if (obj != NULL)
		{
			inParent->push_back (obj);
			if (inKeepTogether)
				obj->SetKeepTogether();
		}
	}

	// order by top/left coordinates
//	inParent->sort (RWObjectCompareTopLeft());
	std::sort<RWObjList::iterator, RWObjectComparePosition> (inParent->begin(), inParent->end(), RWObjectComparePosition());

# if	_4D_Package_
	Yield4D();
# endif

	return;
}
