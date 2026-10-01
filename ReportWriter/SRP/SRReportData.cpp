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

SRReportData::SRReportData (RWXmlDocument *inXML)
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

RWXmlNode
SRReportData::GetReport (void)
const
{
	return mXML->Root();		// "SRReport"
}


// ---------------------------------------------------------------------------
// GetName															  [public]
// ---------------------------------------------------------------------------

const RWString
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
	RWXmlNode	report = mXML->Root();		// "SRReport"
	if (!report)
		return;

	PSObject::LoadXML (report);
	//mbs 20052011	add default style -> needed for SRText's RWStyle parsing
	RWStyle	*style = new RWStyle (&mStyles, RWXmlNode());
	mStyles.insert (pair<long,RWStyle*> (style->GetID (), style));

	for (RWXmlNode elem : report.Children())
	{
		const RWString	value = elem.Name();
		if (RWStr::EqualsNoCase (value, "StyleSet"))
			ParseStyleSet (elem);
		else if (RWStr::EqualsNoCase (value, "Watermark"))
			ParseSection (elem);
		else if (RWStr::EqualsNoCase (value, "Header") || RWStr::EqualsNoCase (value, "Body") || RWStr::EqualsNoCase (value, "Page") || RWStr::EqualsNoCase (value, "Footer"))
			ParseSection (elem);
		else if (RWStr::EqualsNoCase (value, "BreakHeader") || RWStr::EqualsNoCase (value, "BreakFooter"))
			ParseSection (elem);
//		else if (RWStr::EqualsNoCase (value, "DataSource"))	--> parsed by SRDataSource
//			ParseDataSource (elem);
		else if (not mReportWriter->IsExport())	//mbs 05112010
		{
/*
			else if (RWStr::EqualsNoCase (value, "PageSetup"))		// Classic
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mPageSetup.SetBlob (data, true);
			}
*/
			if (RWStr::EqualsNoCase (value, "PageFormat"))		// Carbon
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mPageFormat.SetBlob (data, true);
			}
			else if (RWStr::EqualsNoCase (value, "PrintSettings"))	// Carbon
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mPrintSettings.SetBlob (data, true);
			}
			else if (RWStr::EqualsNoCase (value, "DevMode"))			// Win32
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mDevMode.SetBlob (data, true);
			}
			else if (RWStr::EqualsNoCase (value, "DeviceNames"))		// Win32
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mDeviceNames.SetBlob (data, true);
			}
			else if (RWStr::EqualsNoCase (value, "PageSetupDlg"))	// Win32
			{
				SBlob	data;
				data.Init();
				RWTools::ReadData (elem, data);
				mPageSetupDialog.SetBlob (data, true);
			}
			else if (RWStr::EqualsNoCase (value, "PrintDlg"))		// Win32
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
SRReportData::ParseStyleSet (RWXmlNode inStyleSet)
{
	for (RWXmlNode elem : inStyleSet.Children())
	{
		if (!RWStr::EqualsNoCase (elem.Name(), "Style"))
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
SRReportData::ParseSection (RWXmlNode inSection)
{
	const RWString	value = inSection.Name();
	if (RWStr::EqualsNoCase (value, "Watermark"))
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
	else if (RWStr::EqualsNoCase (value, "Body") || RWStr::EqualsNoCase (value, "Page"))
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
	else if (RWStr::EqualsNoCase (value, "BreakHeader"))
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
	else if (RWStr::EqualsNoCase (value, "BreakFooter"))
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
	else if (RWStr::EqualsNoCase (value, "Header") || RWStr::EqualsNoCase (value, "Footer"))
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
SRReportData::ParseObjects (SRObjListD *inParent, RWXmlNode inObject)
{
	int			seqID = 0;

	for (RWXmlNode elem : inObject.Children())
	{
		SRObject		*obj = NULL;
		const RWString	value = elem.Name();

		if (RWStr::EqualsNoCase (value, "Group"))
		{
			SRGroup	*group = SRGroup::Create (this, elem, ++seqID);
			if (group != NULL)
			{
				obj = group;
				ParseObjects (group->GetObjects(), elem);
			}
		}
		else if (RWStr::StartsWithNoCase (value, "Pict"))
			obj = SRPict::Create (this, elem, ++seqID);	//mbs 05112010	TODO: what to do with export?!?
		else if (RWStr::EqualsNoCase (value, "Text"))
		{
			if (not mReportWriter->IsExport() || (mReportWriter->GetFlags() & eo_static) != 0)	//mbs 05112010	eo_static
				obj = SRText::Create (this, elem, ++seqID);
		}
		else if (RWStr::StartsWithNoCase (value, "Var"))
			obj = SRVariable::Create (this, elem, ++seqID);
		else if (RWStr::EqualsNoCase (value, "Field"))
			obj = SRField::Create (this, elem, ++seqID);
		else if (RWStr::EqualsNoCase (value, "Table"))
			obj = SRTable::Create (this, elem, ++seqID);
		else if (not mReportWriter->IsExport())	//mbs 05112010
		{
			if (RWStr::EqualsNoCase (value, "Line"))
				obj = SRLine::Create (this, elem, ++seqID);
			else if (RWStr::EqualsNoCase (value, "Rect"))
				obj = SRRect::Create (this, elem, ++seqID);
			else if (RWStr::EqualsNoCase (value, "Oval"))
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

RWXmlNode
SRReportData::Write (RWXmlDocument &outXML)
const
{
	RWXmlNode	report = outXML.Node().Append (u"Report");
	report.SetAttr (u"Version", u"1.0");
	if (not mName.empty())
		report.SetAttr (u"name", mName);
	if (not mID.empty())
		report.SetAttr (u"id", mID);
	report.SetAttribute (u"pageWidth", mPageWidth);
	report.SetAttribute (u"pageHeight", mPageHeight);
	report.SetAttr (u"pageMargins", mPageMargins.ToString());
	if (mUsePhysical)
		report.SetAttribute (u"usePhysical", 1);
	if (not mSimple)
		report.SetAttribute (u"Dynamic", 1);
	if (mReportRotation)
		report.SetAttribute (u"rotation", 1);
	if (mReportMirror)
		report.SetAttribute (u"mirror", 1);

	RWXmlNode	styles = report.Append (u"StyleSet");
	for (const auto &entry : mStyles)
	{
		const	RWStyle	*style = entry.second;
		RWXmlNode		elem = styles.Append (u"Style");
		elem.SetAttribute (u"id", style->GetID());
		elem.SetAttr (u"font", style->GetFName());

		RWString	name;
		if (style->GetBaseID() == -1)
			name = style->GetName();
		else if (RWStyle *baseStyle = GetStyle (style->GetBaseID()))
			name = baseStyle->GetName();
		if (!name.empty())
			elem.SetAttr (u"name", name);

		elem.SetAttribute (u"size", style->GetSize());
		if (style->ShouldWrap())
			elem.SetAttribute (u"wrap", 1);
		int	v = style->GetStyle();
		if (v & RWStyle::st_bold)
			elem.SetAttribute (u"bold", 1);
		if (v & RWStyle::st_italic)
			elem.SetAttribute (u"italic", 1);
		if (v & RWStyle::st_underline)
			elem.SetAttribute (u"underline", 1);
		if (v & RWStyle::st_strikethrough)
			elem.SetAttribute (u"strikethrough", 1);
		v = style->GetJustification();
		if (v != RWStyle::st_default)
			elem.SetAttribute (u"align", v);
		v = style->GetVerticalJustification();
		if (v != RWStyle::st_default)
			elem.SetAttribute (u"valign", v);

		SRGBColor	rgb = style->GetTextColor();
		if (rgb != RWStyle::cDefTextColor)
			elem.SetAttr (u"textColor", rgb.ToString());
		rgb = style->GetBackColor();
		if (rgb != RWStyle::cDefBackColor)
			elem.SetAttr (u"backColor", rgb.ToString());
		rgb = style->GetFrameColor();
		if (rgb != RWStyle::cDefFrameColor)
			elem.SetAttr (u"frameColor", rgb.ToString());
		float	f = style->GetRotation();
		if (f != 0)
			elem.SetAttribute (u"rotation", f);
		f = style->GetBaseLineShift();
		if (f != 0)
			elem.SetAttribute (u"baseLineShift", f);
		f = style->GetHorizontalScale();
		if (f != 1)
			elem.SetAttribute (u"hScale", f);
		f = style->GetLineSpacing();
		if (f != 1.2f)
			elem.SetAttribute (u"lineSpacing", f);
	}

	if (not mReportWriter->IsExport())	//mbs 05112010
	{
		// print setup blobs of the platform printing APIs
		auto	writeBlob = [&report] (const RWValue &inValue, RWStringView inName, RWStringView inKind)
		{
			if (inValue.GetBlob())
			{
				RWXmlNode	elem = report.Append (inName);
				elem.SetAttr (u"kind", inKind);
				elem.SetAttr (u"encoding", u"base64");
				RWTools::WriteData (elem, inValue.GetBlob());
			}
		};
		writeBlob (mPageFormat, u"PageFormat", u"Carbon");
		writeBlob (mPrintSettings, u"PrintSettings", u"Carbon");
		writeBlob (mDevMode, u"DevMode", u"Win32");
		writeBlob (mDeviceNames, u"DeviceNames", u"Win32");
		writeBlob (mPageSetupDialog, u"PageSetupDlg", u"Win32");
		writeBlob (mPrintDialog, u"PrintDlg", u"Win32");
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
		case PSObjPropMargins:			outValue.SetText (mPageMargins.ToString()); break;

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
							if (simple != (RWStr::EqualsNoCase (obj->GetType(), "Page")))
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

