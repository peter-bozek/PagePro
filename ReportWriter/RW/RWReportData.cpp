# include	"RWReportData.h"
# include	"RWReportWriter.h"
# include	<algorithm>

# if	_4D_Package_
extern	"C"		void Yield4D (void);
# endif


// ---------------------------------------------------------------------------
// RWReportData								Constructor				  [public]
// ---------------------------------------------------------------------------

RWReportData::RWReportData (RWXmlDocument *inXML)
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
	mName.clear();
	if (mWatermark)
		delete mWatermark;
}


// ---------------------------------------------------------------------------
// GetReport														  [public]
// ---------------------------------------------------------------------------

RWXmlNode
RWReportData::GetReport (void)
const
{
	return mXML->Root();
}


// ---------------------------------------------------------------------------
// GetName															  [public]
// ---------------------------------------------------------------------------

const RWString
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
	RWXmlNode	report = mXML->Root();	// should be "Report"

	if (!report || report.Attr (u"Version") != u"1.0")
		return;

	if (report.AttrInt (u"Dynamic", 0) != 0)
		mIsDynamic = true;
	if (report.HasAttr (u"name"))
		mName = report.Attr (u"name");

	for (RWXmlNode elem : report.Children())
	{
		if (elem.NameIs ("StyleSet"))
			ParseStyleSet (elem);
		else if (elem.NameIs ("Header") || elem.NameIs ("Page") || elem.NameIs ("Footer"))
			ParseSection (elem);
		else if (RWStr::EqualsNoCase (elem.Name(), "Watermark"))
			ParseSection (elem);
		else if (elem.NameIs ("BreakHeader") || elem.NameIs ("BreakFooter"))
			ParseSection (elem);
	}

	return;
}


// ---------------------------------------------------------------------------
// ParseStyleSet													 [private]
// ---------------------------------------------------------------------------

void
RWReportData::ParseStyleSet (RWXmlNode inStyleSet)
{
	RWStyle		*style;

	for (RWXmlNode elem : inStyleSet.Children())
	{
		if (!RWStr::EqualsNoCase (elem.Name(), "Style"))
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
RWReportData::ParseSection (RWXmlNode inSection)
{
	const RWString	sectionName = inSection.Name();

	if (mIsDynamic)
	{
		if (RWStr::EqualsNoCase (sectionName, "Page"))
		{
			RWPageSection	*pageSection = new RWPageSection;
			pageSection->Parse (this, inSection);
			mBody.push_back (pageSection);

			for (RWXmlNode elem : inSection.Children())
			{
				const RWString	value = elem.Name();

				if (RWStr::EqualsNoCase (value, "Body"))
				{
					RWSection	*body = new RWSection (RWSection::eSectionKind_Body);
					body->Parse (this, elem);
					mBody.push_back (body);
					ParseObjects (body->GetKeepTogether(), body->GetObjects(), elem);
				}
				else if (RWStr::StartsWithNoCase (value, "Break"))
				{
					RWBreakSection	*breakLevel = new RWBreakSection (value);
					breakLevel->Parse (this, elem);
					mBody.push_back (breakLevel);
					ParseObjects (breakLevel->GetKeepTogether(), breakLevel->GetObjects(), elem);
				}
				else if (RWStr::EqualsNoCase (value, "Header") || RWStr::EqualsNoCase (value, "Footer"))
				{
					RWHeaderFooterSection	*headerFooter = new RWHeaderFooterSection (value);
					headerFooter->Parse (this, elem);
					mPageSections.push_back (headerFooter);
					ParseObjects (headerFooter->GetKeepTogether(), headerFooter->GetObjects(), elem);
				}
				else if (RWStr::EqualsNoCase (value, "Watermark"))
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
		if (RWStr::EqualsNoCase (sectionName, "Page"))
		{
			RWPageSection	*pageSection = new RWPageSection;
			pageSection->Parse (this, inSection);
			mBody.push_back (pageSection);
			ParseObjects (pageSection->GetKeepTogether(), pageSection->GetObjects(), inSection);
		}
		else if (RWStr::EqualsNoCase (sectionName, "Header") || RWStr::EqualsNoCase (sectionName, "Footer"))
		{
			RWHeaderFooterSection	*headerFooter = new RWHeaderFooterSection (sectionName);
			headerFooter->Parse (this, inSection);
			mPageSections.push_back (headerFooter);
			ParseObjects (headerFooter->GetKeepTogether(), headerFooter->GetObjects(), inSection);
		}
		else if (RWStr::EqualsNoCase (sectionName, "Watermark"))
		{
			if (mWatermark == NULL)
			{
				mWatermark = new RWWatermarkSection (sectionName);
				mWatermark->Parse (this, inSection);
				ParseObjects (mWatermark->GetKeepTogether(), mWatermark->GetObjects(), inSection);
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// ParseObjects														 [private]
// ---------------------------------------------------------------------------

void
RWReportData::ParseObjects (bool inKeepTogether, RWObjList *inParent, RWXmlNode inObject)
{
	RWObject	*obj;
	int			seqID = 0;

	for (RWXmlNode elem : inObject.Children())
	{
		obj = NULL;
		const RWString	value = elem.Name();

		if (RWStr::EqualsNoCase (value, "Group"))
		{
			RWGroup	*group = RWGroup::Create (this, elem, ++seqID);
			if (group != NULL)
			{
				obj = group;
				ParseObjects (inKeepTogether, group->GetObjects(), elem);	// pB changed: Group must always fit on a single page...
			}
		}
#if OLD_RW_FORMAT
		else if (RWStr::EqualsNoCase (value, "Data"))
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
		else if (RWStr::EqualsNoCase (value, "Text"))
		{
			obj = RWText::Create (this, elem, ++seqID);
		}
		else if (RWStr::EqualsNoCase (value, "Table"))
		{
			obj = RWTable::Create (this, elem, ++seqID);
		}
		else if (RWStr::EqualsNoCase (value, "Line"))
		{
			obj = RWLine::Create (this, elem, ++seqID);
		}
		else if (RWStr::EqualsNoCase (value, "Rect") || RWStr::EqualsNoCase (value, "Rectangle"))
		{
			obj = RWRect::Create (this, elem, ++seqID);
		}
		else if (RWStr::EqualsNoCase (value, "Oval"))
		{
			obj = RWOval::Create (this, elem, ++seqID);
		}
		else if (RWStr::EqualsNoCase (value, "Var") || RWStr::EqualsNoCase (value, "Variable"))
		{
			obj = RWVariable::Create (this, elem, ++seqID);
		}
		else if (RWStr::EqualsNoCase (value, "Pict") || RWStr::EqualsNoCase (value, "Picture"))
		{
			obj = RWPict::Create (this, elem, ++seqID);
		}
		else
		{
#if OLD_RW_FORMAT
			if (!RWStr::EqualsNoCase (value, "Object"))
			{
//				inObject->RemoveChild (node);
				continue;
			}

			const RWString	type = elem->Attribute ("Type");
			if (type == NULL)
			{
//				inObject->RemoveChild (node);
				continue;
			}

			if (RWStr::EqualsNoCase (type, "Line"))
				obj = RWLine::Create (this, elem, ++seqID);
			else if (RWStr::EqualsNoCase (type, "Rect") || RWStr::EqualsNoCase (type, "Rectangle"))
				obj = RWRect::Create (this, elem, ++seqID);
			else if (RWStr::EqualsNoCase (type, "Oval"))
				obj = RWOval::Create (this, elem, ++seqID);
			else if (RWStr::EqualsNoCase (type, "Pict") || RWStr::EqualsNoCase (type, "Picture"))
				obj = RWPict::Create (this, elem, ++seqID);
			else if (RWStr::EqualsNoCase (type, "Text"))
				obj = RWText::Create (this, elem, ++seqID);
			else if (RWStr::EqualsNoCase (type, "Var") || RWStr::EqualsNoCase (type, "Variable"))
				obj = RWVariable::Create (this, elem, ++seqID);
			else if (RWStr::EqualsNoCase (type, "Table"))
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
