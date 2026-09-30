# include	"SRReportData.h"
# include	"SRReportWriter.h"
# include	"ETReport.h"
# include	<algorithm>
# include	"PSObjProps.h"

// A4, half inch margins
# define	DEF_PAGE_WIDTH	(595)	// - 72)
# define	DEF_PAGE_HEIGHT	(842)	// - 72)


const PSObject::PSObjProps	SRReportData::sProperties[] = {
{ PSObjPropOID,				false,	PSProps_None,		PSProps_Integer,	"oid",				{ NULL, 0, 1, LONG_MAX }		},
{ PSObjPropKind,			false,	PSProps_None,		PSProps_List,		"kind",				{ sKind, -1 }					},
{ PSObjPropVersion,			false,	PSProps_Attribute,	PSProps_Real,		"Version",			{ NULL, 0, 1, LONG_MAX }		},
{ PSObjPropName,			true,	PSProps_Attribute,	PSProps_String,		"name",				{ NULL }						},
{ PSObjPropID,				true,	PSProps_Attribute,	PSProps_String,		"id",				{ NULL }						},
{ PSObjPropDynamic,			true,	PSProps_Attribute,	PSProps_Boolean,	"simple",			{ NULL, 0, 0, 1 } 				},
{ PSObjPropWidth,			true,	PSProps_Attribute,	PSProps_Real,		"pageWidth",		{ NULL, -1, 100, 4096 }			},
{ PSObjPropHeight,			true,	PSProps_Attribute,	PSProps_Real,		"pageHeight",		{ NULL, -1, 100, 4096 }			},
{ PSObjPropPaper,			true,	PSProps_Attribute,	PSProps_Boolean,	"usePhysical",		{ NULL, 0, 0, 1 } 				},
{ PSObjPropMargins,			true,	PSProps_Attribute,	PSProps_Rect,		"pageMargins",		{ NULL }						},

// v 1.4 properties
//{ PSObjPropLabel,			true,	PSProps_Attribute,	PSProps_Boolean,	"label",			{ NULL, 0, 0, 1 }				},
//{ PSObjPropLabelH,			true,	PSProps_Attribute,	PSProps_Integer,	"LabelH",			{ NULL, 0, 0, 100 }				},
//{ PSObjPropLabelV,			true,	PSProps_Attribute,	PSProps_Integer,	"labelV",			{ NULL, 0, 0, 100 }				},
//
//{ PSObjPropLabelMTop,		true,	PSProps_Attribute,	PSProps_Real,		"labelMarginTop",	{ NULL, 0, -100, 1024 }			},
//{ PSObjPropLabelMLeft,		true,	PSProps_Attribute,	PSProps_Real,		"labelMarginLeft",	{ NULL, 0, -100, 1024 }			},
//{ PSObjPropLabelMBottom,	true,	PSProps_Attribute,	PSProps_Real,		"labelMarginBottom", { NULL, 0, -100, 1024 }		},
//{ PSObjPropLabelMRight,		true,	PSProps_Attribute,	PSProps_Real,		"labelMarginRight",	{ NULL, 0, -100, 1024 }			},
   
{ PSObjPropObjectRotation,	true,	PSProps_Attribute,	PSProps_Boolean,	"rotation",			{ NULL, 0, 0, 1 }               },
{ PSObjPropMirror,			true,	PSProps_Attribute,	PSProps_Boolean,	"mirror",           { NULL, 0, 0, 1 }               },

{ PSObjPropStyle,			false,	PSProps_None,		PSProps_Objects,	"StyleSet",			{ NULL }						},
{ PSObjPropBodySection,		false,	PSProps_None,		PSProps_Objects,	"Page",				{ NULL }						},
{ PSObjPropBodySection,		false,	PSProps_None,		PSProps_Objects,	"Body",				{ NULL }						},
{ PSObjPropPageSections,	false,	PSProps_None,		PSProps_Objects,	"Headers/Footers",	{ NULL }						},
{ PSObjPropBreakHeaders,	false,	PSProps_None,		PSProps_Objects,	"BreakHeader",		{ NULL }						},
{ PSObjPropBreakFooters,	false,	PSProps_None,		PSProps_Objects,	"BreakFooter",		{ NULL }						},
{ PSObjPropWatermarkSection,false,	PSProps_None,		PSProps_Objects,	"Watermark",		{ NULL }						},

	
	
	
//{ PSObjPropPageSetup,		true,	PSProps_None,		PSProps_BLOB,		"PageSetup",		{ NULL }						},
//{ PSObjPropPageFormat,		true,	PSProps_None,		PSProps_BLOB,		"PageFormat",		{ NULL }						},
//{ PSObjPropPrintSettings,	true,	PSProps_None,		PSProps_BLOB,		"PrintSettings",	{ NULL }						},
//{ PSObjPropDevMode,			true,	PSProps_OneChild,	PSProps_BLOB,		"DevMode",			{ NULL }						},
//{ PSObjPropDeviceNames,		true,	PSProps_OneChild,	PSProps_BLOB,		"DeviceNames",		{ NULL }						},
//{ PSObjPropPageSetupDlg,	true,	PSProps_OneChild,	PSProps_BLOB,		"PageSetupDlg",		{ NULL }						},
//{ PSObjPropPrintDlg,		true,	PSProps_OneChild,	PSProps_BLOB,		"PrintDlg",			{ NULL }						},
{ 0, 						false,	PSProps_None,		PSProps_Boolean,	NULL,				{ NULL }						}
};


// ---------------------------------------------------------------------------
// SRReportData								Constructor				  [public]
// ---------------------------------------------------------------------------

SRReportData::SRReportData (XMLDocument *inXML)
	:	PSObject (eObject_Document),
		mReportWriter (0),
		mXML (inXML),
		mLastStyle (0),	//mbs 20052011
		mWatermark (0),
		mBody (0),
		mSimple (false),
		mPageWidth (DEF_PAGE_WIDTH),
		mPageHeight (DEF_PAGE_HEIGHT),
		mUsePhysical (false),
        mReportRotation(false),
        mReportMirror(false),
        mPageMargins (36, 36, 36, 36)
{
}


// ---------------------------------------------------------------------------
// ~SRReportData							Destructor				  [public]
// ---------------------------------------------------------------------------

SRReportData::~SRReportData (void)
{
	if (mWatermark)
		delete mWatermark;
	if (mBody)
		delete mBody;
	return;
}


// ---------------------------------------------------------------------------
// GetReport														  [public]
// ---------------------------------------------------------------------------

const XMLElement*
SRReportData::GetReport (void)
const
{
//	return mXML->FirstChild ("SRReport")->ToElement();
	return mXML->RootElement();
}


// ---------------------------------------------------------------------------
// GetName															  [public]
// ---------------------------------------------------------------------------

const CText
SRReportData::GetName (void)
const
{
	return mName;
}


// ---------------------------------------------------------------------------
// ParseReport														  [public]
// ---------------------------------------------------------------------------

void
SRReportData::ParseReport (void)
{
//	XMLNode		*report = mXML->FirstChild ("SRReport");	// should be same as mXML->RootElement();
	XMLNode		*report = mXML->RootElement();
	XMLNode		*node = NULL, *next;
    XMLElement	*elem;
	const CXMLText	value;

	if (report)
	{
		elem = report->ToElement();
		if (elem /* && (value = elem->Attribute ("Version")) != NULL && STR_EQUALS (value, "1.0") */)
		{
			PSObject::LoadXML (elem);
			node = report->FirstChildElement();
			//mbs 20052011	add default style -> needed for SRText's RWStyle parsing
			RWStyle	*style = new RWStyle (&mStyles, NULL);
			mStyles.insert (pair<long,RWStyle*> (style->GetID (), style));
		}
	}

	for ( ; node; node = next )
	{
		next = node->NextSibling();
		elem = node->ToElement();
		if (elem == NULL)
			continue;

		value = elem->Value();
		if (STR_EQUALS (value, "StyleSet"))
			ParseStyleSet (elem);
		else if (STR_EQUALS (value, "Watermark"))
			ParseSection (elem);
		else if (STR_EQUALS (value, "Header") || STR_EQUALS (value, "Body") || STR_EQUALS (value, "Page") || STR_EQUALS (value, "Footer"))
			ParseSection (elem);
		else if (STR_EQUALS (value, "BreakHeader") || STR_EQUALS (value, "BreakFooter"))
			ParseSection (elem);
//		else if (STR_EQUALS (value, "DataSource"))	--> parsed by SRDataSource
//			ParseDataSource (elem);
//		else if (STR_EQUALS (value, "Editor"))
//			ParseEditorSettings (elem);
//		else if (STR_EQUALS (value, "Guides"))
//			ParseGuides (elem);
		else if (not mReportWriter->IsExport())	//mbs 05112010
		{
/*
			else if (STR_EQUALS (value, "PageSetup"))		// Classic
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mPageSetup.SetBlob (data, true);
			}
*/
			if (STR_EQUALS (value, "PageFormat"))		// Carbon
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mPageFormat.SetBlob (data, true);
			}
			else if (STR_EQUALS (value, "PrintSettings"))	// Carbon
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mPrintSettings.SetBlob (data, true);
			}
			else if (STR_EQUALS (value, "DevMode"))			// Win32
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mDevMode.SetBlob (data, true);
			}
			else if (STR_EQUALS (value, "DeviceNames"))		// Win32
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mDeviceNames.SetBlob (data, true);
			}
			else if (STR_EQUALS (value, "PageSetupDlg"))	// Win32
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mPageSetupDialog.SetBlob (data, true);
			}
			else if (STR_EQUALS (value, "PrintDlg"))		// Win32
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mPrintDialog.SetBlob (data, true);
			}
		}
	}


	//mbs 17092010	create missing calculated variables in Body section
	const RWList<RWCalculatedValue*>&		calc = mReportWriter->GetCalculatedVariables();
	if (calc.size() > 0)
	{
		if (mBody == NULL)
			mBody = new SRPageSection (true);
		mBody->CreateCalculatedObjects (calc);
	}
	if (mBody && mBody->IsEmpty())
	{
		delete mBody;
		mBody = NULL;
	}
	
	return;
}


// ---------------------------------------------------------------------------
// ParseStyleSet													 [private]
// ---------------------------------------------------------------------------

void
SRReportData::ParseStyleSet (XMLElement *inStyleSet)
{
	XMLNode	*node;
//	RWStyle		*style = new RWStyle (NULL);

//	mStyles.push_back (style);	// add default style

	for ( node = inStyleSet->FirstChildElement(); node; node = node->NextSibling() )
	{
        XMLElement	*elem = node->ToElement();
		if (elem == NULL)
			continue;
		if (!STR_EQUALS (elem->Value(), "Style"))
			continue;

		RWStyle	*style = new RWStyle (&mStyles, elem);
		mStyles.insert (pair<long,RWStyle*> (style->GetID (), style));
	}

	mLastStyle = mStyles.GetNewID() - 1;	//mbs 20052011
	return;
}


// ---------------------------------------------------------------------------
// ParseSection														 [private]
// ---------------------------------------------------------------------------

void
SRReportData::ParseSection (XMLElement *inSection)
{
	const CXMLText	value = inSection->Value();
	if (STR_EQUALS (value, "Watermark"))
	{
		if (mWatermark == NULL)
		{
			mWatermark = new SRWatermarkSection (value);
			mWatermark->Parse (this, inSection);
			ParseObjects (mWatermark->GetObjects(), inSection);
			if (mWatermark->IsEmpty())
			{
				delete mWatermark;
				mWatermark = NULL;
			}
		}
	}
	else if (STR_EQUALS (value, "Body") || STR_EQUALS (value, "Page"))
	{
		if (mBody == NULL)
		{
			mBody = new SRPageSection (value);
			mBody->Parse (this, inSection);
			ParseObjects (mBody->GetObjects(), inSection);
/* moved into ParseReport
			if (mBody->IsEmpty())
			{
				delete mBody;
				mBody = NULL;
			}
*/
		}
	}
	else if (STR_EQUALS (value, "BreakHeader"))
	{
		SRBreakSection	*breakHeader = new SRBreakSection (value);
		breakHeader->Parse (this, inSection);
		ParseObjects (breakHeader->GetObjects(), inSection);
		if (breakHeader->IsEmpty())
			delete breakHeader;
		else
		{
			breakHeader->CreateBreak (this);
			mBreakHeaders.push_back (breakHeader);
		}
	}
	else if (STR_EQUALS (value, "BreakFooter"))
	{
		SRBreakSection	*breakFooter = new SRBreakSection (value);
		breakFooter->Parse (this, inSection);
		ParseObjects (breakFooter->GetObjects(), inSection);
		if (breakFooter->IsEmpty())
			delete breakFooter;
		else
		{
			breakFooter->CreateBreak (this);
			mBreakFooters.push_back (breakFooter);
		}
	}
	else if (STR_EQUALS (value, "Header") || STR_EQUALS (value, "Footer"))
	{
		SRHeaderFooterSection	*headerFooter = new SRHeaderFooterSection (value);
		headerFooter->Parse (this, inSection);
		ParseObjects (headerFooter->GetObjects(), inSection);
		if (headerFooter->IsEmpty())
			delete headerFooter;
		else
			mPageSections.push_back (headerFooter);
	}

	return;
}


// ---------------------------------------------------------------------------
// ParseObjects														 [private]
// ---------------------------------------------------------------------------

void
SRReportData::ParseObjects (SRObjListD *inParent, XMLElement *inObject)
{
	XMLNode	*node, *next;
	int			seqID = 0;

	for ( node = inObject->FirstChildElement(); node; node = next )
	{
		next = node->NextSibling();
        XMLElement	*elem = node->ToElement();
		if (elem == NULL)
			continue;

		SRObject		*obj = NULL;
		const CXMLText	value = elem->Value();

		if (STR_EQUALS (value, "Group"))
		{
			SRGroup	*group = SRGroup::Create (this, elem, ++seqID);
			if (group != NULL)
			{
				obj = group;
				ParseObjects (group->GetObjects(), elem);
			}
		}
		else if (STR_STARTS_WITH (value, "Pict"))
			obj = SRPict::Create (this, elem, ++seqID);	//mbs 05112010	TODO: what to do with export?!?
		else if (STR_EQUALS (value, "Text"))
		{
			if (not mReportWriter->IsExport() || (mReportWriter->GetFlags() & eo_static) != 0)	//mbs 05112010	eo_static
				obj = SRText::Create (this, elem, ++seqID);
		}
		else if (STR_STARTS_WITH (value, "Var"))
			obj = SRVariable::Create (this, elem, ++seqID);
		else if (STR_EQUALS (value, "Field"))
			obj = SRField::Create (this, elem, ++seqID);
		else if (STR_EQUALS (value, "Table"))
			obj = SRTable::Create (this, elem, ++seqID);
		else if (not mReportWriter->IsExport())	//mbs 05112010
		{
			if (STR_EQUALS (value, "Line"))
				obj = SRLine::Create (this, elem, ++seqID);
			else if (STR_EQUALS (value, "Rect"))
				obj = SRRect::Create (this, elem, ++seqID);
			else if (STR_EQUALS (value, "Oval"))
				obj = SROval::Create (this, elem, ++seqID);
		}
		
		if (obj != NULL)
			inParent->push_back (obj);
	}

	// order by top/left coordinates
//	inParent->sort (SRObjectCompareTopLeft());
//	std::sort<SRObjListD::iterator, SRObjectComparePosition> (inParent->begin(), inParent->end(), SRObjectComparePosition());
	std::sort<SRObjListD::iterator, SRObjectCompareOrder> (inParent->begin(), inParent->end(), SRObjectCompareOrder());

	return;
}


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

// Removed void SRReportData::Write(FILE *fd) const as per instructions


// ---------------------------------------------------------------------------
// WriteReportEnd													  [public]
// ---------------------------------------------------------------------------

// Removed void SRReportData::WriteReportEnd(FILE *fd) const as per instructions


// ---------------------------------------------------------------------------
// Write															  [public]
// ---------------------------------------------------------------------------

XMLElement*
SRReportData::Write (XMLDocument *outXML)
const
{
/*
	{
		TiXmlDeclaration	decl ("1.0", "utf-8", "yes");
		outXML->InsertEndChild (decl);
	}
*/
// Removed TiXmlDeclaration insertion block; using tinyxml2 style

    XMLElement*	report = NULL;
	{
        XMLElement* root = outXML->NewElement("Report");
		root->SetAttribute ("Version", "1.0");
//		const CXMLText	s = GetReport()->Attribute ("Name");
		if (not mName.IsEmpty())
		{
//			root.SetAttribute ("Name", (const char*) mName);
			CXMLText	name = mName.ToXML();
			root->SetAttribute ("name", name.c_str());
			mName.FreeXML (name);
		}
		if (not mID.IsEmpty())
		{
			CXMLText	name = mID.ToXML();
			root->SetAttribute ("id", name.c_str());
			mID.FreeXML (name);
		}
		root->SetAttribute ("pageWidth", mPageWidth);
		root->SetAttribute ("pageHeight", mPageHeight);
		root->SetAttribute ("pageMargins", (const char*) mPageMargins);
		if (mUsePhysical)
			root->SetAttribute ("usePhysical", 1);
		if (not mSimple)
			root->SetAttribute ("Dynamic", 1);
        if (mReportRotation)
            root->SetAttribute ("rotation", 1);
        if (mReportMirror)
            root->SetAttribute ("mirror", 1);
		report = outXML->InsertEndChild(root)->ToElement();
	}

    XMLElement*	styles = NULL;
	{
        XMLElement* stylesElem = outXML->NewElement("StyleSet");
		styles = report->InsertEndChild(stylesElem)->ToElement();
	}

	{
		RWStyleList::const_iterator	it;

		for (it = mStyles.begin(); it != mStyles.end(); it++)
		{
			const	RWStyle	*style = (*it).second;
            XMLElement* elem = outXML->NewElement("Style");
			elem->SetAttribute ("id", style->GetID());
			RWTextValue	fname (style->GetFName());
			CXMLText	name = fname.ToXML();
			elem->SetAttribute ("font", name.c_str());
			fname.FreeXML (name);
            
            if (style->GetBaseID() == -1) {
                fname = style->GetName();
            } else {
                RWStyle * baseStyle = GetStyle (style->GetBaseID());
                fname = baseStyle->GetName();
            }
            if(!fname.IsEmpty()) {
                name = fname.ToXML();
                elem->SetAttribute ("name", name.c_str());
                fname.FreeXML (name);
            }

			elem->SetAttribute ("size", style->GetSize());
			if (style->ShouldWrap())
				elem->SetAttribute ("wrap", 1);
//			if (style->IsFramed())
//				elem->SetAttribute ("frame", 1);
			int	v = style->GetStyle();
			if (v & RWStyle::st_bold)
				elem->SetAttribute ("bold", 1);
			if (v & RWStyle::st_italic)
				elem->SetAttribute ("italic", 1);
			if (v & RWStyle::st_underline)
				elem->SetAttribute ("underline", 1);
			if (v & RWStyle::st_strikethrough)
				elem->SetAttribute ("strikethrough", 1);
			v = style->GetJustification();
			if (v != RWStyle::st_default)
//				elem.SetAttribute ("align", sJustification [v]);
				elem->SetAttribute ("align", v);
			v = style->GetVerticalJustification();
			if (v != RWStyle::st_default)
//				elem.SetAttribute ("valign", sVAlignment [v]);
				elem->SetAttribute ("valign", v);
//			float	f = style->GetHorizontalOffset();
//			if (f != 0)
//				elem.SetAttribute ("hOffset", f);
//			f = style->GetVerticalOffset();
//			if (f != 0)
//				elem.SetAttribute ("vOffset", f);

			SRGBColor	rgb = style->GetTextColor();
			if (rgb != RWStyle::cDefTextColor)
				elem->SetAttribute ("textColor", (const char*) rgb);
			rgb = style->GetBackColor();
			if (rgb != RWStyle::cDefBackColor)
				elem->SetAttribute ("backColor", (const char*) rgb);
			rgb = style->GetFrameColor();
			if (rgb != RWStyle::cDefFrameColor)
				elem->SetAttribute ("frameColor", (const char*) rgb);
//			if (style->GetPSName() != NULL)
//				elem.SetAttribute ("fontPS", style->GetPSName());
			float	f = style->GetRotation();
			if (f != 0)
				elem->SetAttribute ("rotation", f);
			f = style->GetBaseLineShift();
			if (f != 0)
				elem->SetAttribute ("baseLineShift", f);
			f = style->GetHorizontalScale();
			if (f != 1)
				elem->SetAttribute ("hScale", f);
			f = style->GetLineSpacing();
			if (f != 1.2)
				elem->SetAttribute ("lineSpacing", f);
			styles->InsertEndChild(elem);
		}
	}


	if (not mReportWriter->IsExport())	//mbs 05112010
	{
/*
		if (mPageSetup.GetBlob())
		{
            XMLElement* pageSetup = outXML->NewElement("PageSetup");
			styles = report->InsertEndChild(pageSetup)->ToElement();
			styles->SetAttribute ("kind", "Classic");
			styles->SetAttribute ("encoding", "base64");
			RWTools::WriteData (styles, mPageSetup.GetBlob());
		}
*/
		if (mPageFormat.GetBlob())
		{
            XMLElement* pageFormat = outXML->NewElement("PageFormat");
			styles = report->InsertEndChild(pageFormat)->ToElement();
			styles->SetAttribute ("kind", "Carbon");
			styles->SetAttribute ("encoding", "base64");
			RWTools::WriteData (styles, mPageFormat.GetBlob());
		}
		if (mPrintSettings.GetBlob())
		{
            XMLElement* printSettings = outXML->NewElement("PrintSettings");
			styles = report->InsertEndChild(printSettings)->ToElement();
			styles->SetAttribute ("kind", "Carbon");
			styles->SetAttribute ("encoding", "base64");
			RWTools::WriteData (styles, mPrintSettings.GetBlob());
		}
		if (mDevMode.GetBlob())
		{
            XMLElement* devMode = outXML->NewElement("DevMode");
			styles = report->InsertEndChild(devMode)->ToElement();
			styles->SetAttribute ("kind", "Win32");
			styles->SetAttribute ("encoding", "base64");
			RWTools::WriteData (styles, mDevMode.GetBlob());
		}
		if (mDeviceNames.GetBlob())
		{
            XMLElement* deviceNames = outXML->NewElement("DeviceNames");
			styles = report->InsertEndChild(deviceNames)->ToElement();
			styles->SetAttribute ("kind", "Win32");
			styles->SetAttribute ("encoding", "base64");
			RWTools::WriteData (styles, mDeviceNames.GetBlob());
		}
		if (mPageSetupDialog.GetBlob())
		{
            XMLElement* pageSetupDlg = outXML->NewElement("PageSetupDlg");
			styles = report->InsertEndChild(pageSetupDlg)->ToElement();
			styles->SetAttribute ("kind", "Win32");
			styles->SetAttribute ("encoding", "base64");
			RWTools::WriteData (styles, mPageSetupDialog.GetBlob());
		}
		if (mPrintDialog.GetBlob())
		{
            XMLElement* printDlg = outXML->NewElement("PrintDlg");
			styles = report->InsertEndChild(printDlg)->ToElement();
			styles->SetAttribute ("kind", "Win32");
			styles->SetAttribute ("encoding", "base64");
			RWTools::WriteData (styles, mPrintDialog.GetBlob());
		}
	}

	return report;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRReportData::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropVersion:			outValue.SetReal (1); break;
		case PSObjPropName:				outValue.SetText (mName); break;
		case PSObjPropID:				outValue.SetText (mID); break;	//mbs 15112010
		case PSObjPropDynamic:			outValue.SetBoolean (mSimple); break;
		case PSObjPropWidth:			outValue.SetReal (mPageWidth); break;
		case PSObjPropHeight:			outValue.SetReal (mPageHeight); break;
		case PSObjPropPaper:			outValue.SetBoolean (mUsePhysical); break;
		case PSObjPropMargins:			outValue.SetXMLText ((const char*) mPageMargins); break;

        case PSObjPropObjectRotation:	outValue.SetBoolean (mReportRotation); break;
        case PSObjPropMirror:			outValue.SetBoolean (mReportMirror); break;

		case PSObjPropStyle:			outValue.SetInteger (mStyles.size()); break;
		case PSObjPropPageSections:		outValue.SetInteger (mPageSections.size()); break;
		case PSObjPropBreakHeaders:		outValue.SetInteger (mBreakHeaders.size()); break;
		case PSObjPropBreakFooters:		outValue.SetInteger (mBreakFooters.size()); break;

//		case PSObjPropPageSetup:		outValue.Attach (mPageSetup); break;
		case PSObjPropPageFormat:		outValue.Attach (mPageFormat); break;
		case PSObjPropPrintSettings:	outValue.Attach (mPrintSettings); break;
		case PSObjPropDevMode:			outValue.Attach (mDevMode); break;
		case PSObjPropDeviceNames:		outValue.Attach (mDeviceNames); break;
		case PSObjPropPageSetupDlg:		outValue.Attach (mPageSetupDialog); break;
		case PSObjPropPrintDlg:			outValue.Attach (mPrintDialog); break;

		default:						return PSObject::GetProperty (id, outValue);
	}

	return true;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
SRReportData::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropVersion:			break;
		case PSObjPropName:				return SetStringProperty (inValue, mName);
		case PSObjPropID:				return mReportWriter->IsExport()? SetStringProperty (inValue, mID): false;	//mbs 15112010
		case PSObjPropDynamic:			return SetBooleanProperty (inValue, mSimple);
/*
		{
			bool	simple = mSimple;
			if (SetBooleanProperty (inValue, simple))
			{
				if (simple != mSimple)
				{
					PSObjList::iterator	iter;
					for (iter = mSections.begin(); iter != mSections.end(); iter++)
					{
						DMBase	*obj = static_cast <DMBase*> (*iter);
						if (obj != NULL)
						{
							if (simple != (STR_EQUALS (obj->GetType(), "Page")))
								return false;
					}
					mSimple = simple;
				}
				return true;
			}
			break;
		}
*/

		case PSObjPropWidth:			return SetRealProperty (inValue, mPageWidth, 32);
		case PSObjPropHeight:			return SetRealProperty (inValue, mPageHeight, 32);
		case PSObjPropPaper:			return SetBooleanProperty (inValue, mUsePhysical);
		case PSObjPropMargins:			return SetRectProperty (inValue, mPageMargins);

        case PSObjPropObjectRotation:	return SetBooleanProperty (inValue, mReportRotation);
        case PSObjPropMirror:			return SetBooleanProperty (inValue, mReportMirror);

		case PSObjPropStyle:			break;
		case PSObjPropPageSections:		break;
		case PSObjPropBreakHeaders:		break;
		case PSObjPropBreakFooters:		break;
		case PSObjPropWatermarkSection:	break;

/*
		case PSObjPropPageSetup:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mPageSetup.Clone (inValue);
				return true;
			}
			break;
*/
		case PSObjPropPageFormat:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mPageFormat.Clone (inValue);
				return true;
			}
			break;
		case PSObjPropPrintSettings:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mPrintSettings.Clone (inValue);
				return true;
			}
			break;
		case PSObjPropDevMode:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mDevMode.Clone (inValue);
				return true;
			}
			break;
		case PSObjPropDeviceNames:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mDeviceNames.Clone (inValue);
				return true;
			}
			break;
		case PSObjPropPageSetupDlg:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mPageSetupDialog.Clone (inValue);
				return true;
			}
			break;
		case PSObjPropPrintDlg:
			if (inValue.GetKind() == RWValue::eValue_BLOB)
			{
				mPrintDialog.Clone (inValue);
				return true;
			}
			break;

		default:					return PSObject::SetProperty (id, inValue);
	}

	return false;
}

