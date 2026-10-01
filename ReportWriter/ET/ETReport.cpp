/*
 *  ETReport.cpp
 *  ReportWriter
 *
 *  Created by Peter Bozek on 12/11/2010.
 *  Copyright 2010 INFORCE Bratislava. All rights reserved.
 *
 */


# include	"ETReport.h"
# include	"ETReportData.h"
# include	"RWDataSource.h"
# include	"RWReportVariables.h"
# include   "RWBaseTypes.h"
# include	<cmath>
# include	<algorithm>

# if	_4D_Package_
extern	"C"		void Yield4D (void);
# endif

# include	"SRLicense.h"



// ---------------------------------------------------------------------------
// ETReport							Constructor				  [public]
// ---------------------------------------------------------------------------

ETReport::ETReport (RWDataSource &inDataSource, ETReportData &inData, long inFlags)
:	mSource (inDataSource),
mData (inData),
mOutputOptions (e_OutputOptions (inFlags)),
mPrintTime (0),
mStage (e_OutputStage (0)),
mJsonSectionOpen (false),
mPageSection (0),
mCurrentBody (0),
mFetchRecord (true),
mBreakHeaders (0),
mBreakLevels (-1),
mBreakLevel (0),
mIsOverflow (false),
mLastError (0)	
{
	mData.SetReportWriter (this);
	RWInitReportVariable (mVarNames);
	return;
}


// ---------------------------------------------------------------------------
// ~RWReportWriter							Destructor				  [public]
// ---------------------------------------------------------------------------

ETReport::~ETReport (void)
{
	if (mBreakHeaders != NULL)
		delete [] mBreakHeaders;
	
	return;
}


// ---------------------------------------------------------------------------
// GetReportVariable												  [public]
// ---------------------------------------------------------------------------

int
ETReport::GetReportVariable (const RWString inName)
const
{
	int	index;
	RWString	name (inName);
	for (index = 0; index < RW_VarNamesRWCount; index++)
		if (name.compare(mVarNames [index]) == 0)
			return index;
	
	return -1;
}


// ---------------------------------------------------------------------------
// CreateVariable													  [public]
// ---------------------------------------------------------------------------

void
ETReport::CreateVariable (const RWString inName, ECalcType inCalc)
{
	if (GetReportVariable (inName) == RW_VarNotFound)
	{
		// create break value computation
		if (inCalc != ECalcType_None)
			mCalculator.Add (inName);
		
		// create the variable
		mSource.CreateVariable (inName);
	}
	
	return;
}


// ---------------------------------------------------------------------------
// GetCalculatedValue												  [public]
// ---------------------------------------------------------------------------

bool
ETReport::GetCalculatedValue (const RWString inName, RWValue &outVar)
const
{
	return GetVariable (inName, outVar, ECalcType_CurrentValue);
}


// ---------------------------------------------------------------------------
// GetVariable														  [public]
// ---------------------------------------------------------------------------
// Get report variable

bool
ETReport::GetVariable (const RWString inName, RWValue &outVar, ECalcType inCalc)
const
{
	bool	found = true;
	int		index = GetReportVariable (inName);
	
	outVar.Free();
	
	switch (index)
	{
		case RW_VarNotFound:
			// get break value
			if (inCalc > ECalcType_None)
			{
				double	d;
				found = mCalculator.GetValue (inName, RWCalculatedValue::RWCalculatedType (inCalc), d);
				if (found)
					outVar.SetReal (d);
			}
			else
				found = mSource.GetVariable (inName, outVar);
			break;
			
		case RW_VarPage:
			//		case RW_VarSRPage:
			outVar.SetInteger (1);
			break;
			
		case RW_VarPages:
			outVar.SetInteger (1);
			break;
			
		case RW_VarSubPage:
			outVar.SetInteger (1);
			break;
			
		case RW_VarSubPages:
			outVar.SetInteger (1);
			break;
			
		case RW_VarFrame:
			outVar.SetInteger (1);
			break;
			
		case RW_VarFrames:
			outVar.SetInteger (1);
			break;
			
		case RW_VarDateTime:
			outVar.SetInteger ((long) mPrintTime, RWValue::eValue_DateTime);
			break;
			
		case RW_VarDate:
		case RW_VarTime:
		{
			struct	tm	*tm = localtime (&mPrintTime);
			if (index == RW_VarDate)
				outVar.SetInteger (tm->tm_mday + ((tm->tm_mon + 1) << 5) + ((tm->tm_year + 1900L) << 9), RWValue::eValue_Date);
			else
				outVar.SetInteger (tm->tm_sec + tm->tm_min * 60L + tm->tm_hour * 3600L, RWValue::eValue_Time);
			break;
		}
			
		case RW_VarName:
			outVar.SetText (mData.GetName());
			break;
	}
	
	return found;
}


// ---------------------------------------------------------------------------
// SetVariable														  [public]
// ---------------------------------------------------------------------------

void
ETReport::SetVariable (const RWString inName, RWValue *inVar)
{
	if (GetReportVariable (inName) == RW_VarNotFound)
	{
		mSource.SetVariable (inName, inVar);
	}
	
	return;
}


// ---------------------------------------------------------------------------
// FormatVariable													  [public]
// ---------------------------------------------------------------------------
// Format variable depending on DataSource

RWString
ETReport::FormatVariable (RWValue &inVar, const RWString inFormat)
const
{
	return mSource.FormatVariable (inVar, inFormat);
}



// ---------------------------------------------------------------------------
// ReportToFile														  [public]
// ---------------------------------------------------------------------------
// Draw the report

bool
ETReport::ReportToFile (const RWString &inPath)
{
	mTextOut.clear();
	mXmlOut.Clear();
	mXmlCurrent = RWXmlNode();
	mJsonOut.SetObject();
	mJsonSectionOpen = false;

	// give the DataSource a chance to get data...
	mSource.ParseReport (mData.GetReport());
# if	_4D_Package_
	Yield4D();
# endif
	// create Sections/Objects from XML
	mData.ParseReport();
# if	_4D_Package_
	Yield4D();
# endif
	mName = mData.GetName();		// was never set, so exports had no report name

	mPrintTime = time (NULL);
	WriteText ("ReportStart", NULL);
	if (mData.IsDynamic())
	{
		PrepareDynamicReport();
		DrawDynamicReport();
	}
	else
		DrawStaticReport();
	WriteText ("ReportEnd", NULL);

	// write the whole export at once, as UTF-8
	switch (GetFormat())
	{
		case eFormat_Text:
		case eFormat_HTML:	return RWStr::WriteFile (inPath, RWStr::ToUTF8 (mTextOut));
		case eFormat_XML:	return mXmlOut.SaveFile (inPath);
		case eFormat_JSON:	return RWStr::WriteFile (inPath, RWJson::ToUTF8 (mJsonOut, true));
		default:			return RWStr::WriteFile (inPath, std::string_view());	// no format: empty file, as before
	}
}


// ---------------------------------------------------------------------------
// DrawStaticReport													 [private]
// ---------------------------------------------------------------------------
// Draw the report

void
ETReport::DrawStaticReport (void)
{
	ETSectionList			*sections;
	ETSectionList::iterator	sit;
	
	
		// ony one page when exporting report
	{
		ETPageSection	*page = NULL;
		// find the page willing to print
		sections = mData.GetBodySections();
		for (sit = sections->begin(); sit != sections->end(); sit++)
		{
			ETPageSection	*thisPage = static_cast <ETPageSection*> (*sit);
			// Reset Page section on start of the report
			if (thisPage->WillingToPrint (this))	//mbs 04052005	use mLastPageCollision, not false
			{
				if (page == NULL)
					page = thisPage;
			}
		}
		
		
		DrawStaticPage (page);

	}
	
	return;
}


// ---------------------------------------------------------------------------
// DrawStaticPage													 [private]
// ---------------------------------------------------------------------------
// Draw one page

void
ETReport::DrawStaticPage (ETPageSection *inBody)
{
	SRect					origRect, pageRect;
	ETSectionList			*sections;
	ETSection				*sec;
	ETSectionList::iterator	sit;
	ETWatermarkSection		*watermark = NULL;
	
	watermark = mData.GetWatermarkSection();
	
	if (watermark && not watermark->IsOnTop())
	{
		if (watermark->WillingToPrint (this))
		{
			watermark->Export (this);
		}
	}
	
	
	
	sections = mData.GetPageSections();
	// process headers & footers, draw headers
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = *sit;
		if (sec->GetKind() == ETSection::eSectionKind_Header)
		{
			if (sec->WillingToPrint (this))
			{
				sec->Export (this);
			}
		}
	}
	
	
# if	_4D_Package_
	Yield4D();
# endif
	
	// process body
	if (inBody != NULL)
	{
		inBody->Export (this);
	}
	
# if	_4D_Package_
	Yield4D();
# endif
	
	// draw footers
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = *sit;
		if (sec->GetKind() == ETSection::eSectionKind_Footer)
		{
			if (sec->WillingToPrint (this))
			{
				sec->Export (this);
			}
		}
	}
	

	{
		if (watermark && watermark->IsOnTop())
		{
			if (watermark->WillingToPrint (this))
			{
					watermark->Export (this);
			}
		}
		
	}
	
# if	_4D_Package_
	Yield4D();
# endif
	
	return;
}


// ---------------------------------------------------------------------------
// PrepareDynamicReport												 [private]
// ---------------------------------------------------------------------------

void
ETReport::PrepareDynamicReport (void)
{
	ETSectionList			*sections;
	ETSectionList::iterator	sit;
	ETSection				*sec;
	int						breakLevel;
	
	mCurrentBody = NULL;
	//	mBreakHeaders.clear();
	if (mBreakHeaders != NULL)
	{
		delete [] mBreakHeaders;
		mBreakHeaders = NULL;
	}
	//	mBreakFooters.clear();
	mBreakLevels = -1;
	sections = mData.GetBodySections();
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = *sit;
		if (sec->GetKind() == ETSection::eSectionKind_Page)	// should be the first (& only first) section
			mPageSection = static_cast <ETPageSection*> (sec);
		else if (sec->GetKind() == ETSection::eSectionKind_Body)
		{
			if (mCurrentBody == NULL)
				mCurrentBody = sec;
		}
		else	// break header or footer
		{
			ETBreakSection	*bs = static_cast <ETBreakSection*> (sec);
			breakLevel = bs->GetLevel();
			if (breakLevel > mBreakLevels)
				mBreakLevels = breakLevel;
		}
	}
	
	if (mBreakLevels == -1 && mCalculator.GetVariables().size() > 0)
		mBreakLevels = 0;
	
	mBreakLevel = mBreakLevels;
	mCalculator.InitLevels (mBreakLevels + 1);
	//	mBreakHeaders.resize (mBreakLevels + 1);
	if (mBreakLevels >= 0)
		mBreakHeaders = new ETBreakSection* [mBreakLevels + 1];
	//	mBreakFooters.resize (mBreakLevels + 1);
	return;
}


bool
ETReport::FindNextSection (void)
{
	bool		peekNext = mFetchRecord;
	ETSection	*sec = mCurrentBody;
	int			breakLevel;
	
	while (peekNext)
	{
		if (mPageIterator == mData.GetBodySections()->end())
			break;
		sec = *mPageIterator;
		if (	mPageSection != sec		//mbs 06022006	ignore [empty] Page section
			&&	sec->WillingToPrint (this)	//mbs 04052005	use mLastPageCollision, not false
			)
			peekNext = false;
		else
		{
			if (sec->GetKind() == ETSection::eSectionKind_BreakFooter)
			{
				ETBreakSection	*bs = static_cast <ETBreakSection*> (sec);
				breakLevel = bs->GetLevel();
				if (mBreakLevels >= 0)
					mCalculator.ShuntTotals (breakLevel);
			}
			mPageIterator++;	// ignore invisible/empty section
		}
	}
	
	if (peekNext)
		mCurrentBody = NULL;
	else
	{
		if (mFetchRecord)
			mCurrentBody = sec;
	}
	
	return peekNext;
}


bool
ETReport::PeekNextSection (void)
{
	if (FindNextSection())
		return true;
	
	if (	mCurrentBody->GetKind() == ETSection::eSectionKind_BreakHeader
		||	mCurrentBody->GetKind() == ETSection::eSectionKind_BreakFooter
		)
	{
		ETBreakSection	*bs = static_cast <ETBreakSection*> (mCurrentBody);
		mBreakLevel = bs->GetLevel();
		if (mBreakLevels >= 0)
			mCalculator.SetLevel (mBreakLevel);
	}
	else
	{
		//		mBreakLevel = mBreakLevels;
		if (mBreakLevels >= 0)
			mCalculator.SetLevel (mBreakLevels);
	}
	
	return false;
}


bool
ETReport::GetNextSection (void)
{
	if (PeekNextSection())
		return true;
	
	if (mFetchRecord)
	{
		mPageIterator++;
		mFetchRecord = false;
	}
	
	return false;
}


// ---------------------------------------------------------------------------
// DrawDynamicReport												 [private]
// ---------------------------------------------------------------------------
// Draw the report

void
ETReport::DrawDynamicReport (void)
{
	ETSectionList			*sections;
	ETSectionList::iterator	sit;
	ETSection				*sec;
	
	
	mBreakLevel = mBreakLevels;

	if (mBreakLevels >= 0)
	{
		memset (mBreakHeaders, 0, (mBreakLevels + 1) * sizeof (ETBreakSection*));
		mCalculator.ShuntTotals (0);	//mbs 20092010	if no break headers/footers
	}
	mCurrentBody = NULL;
	sections = mData.GetBodySections();
	for (mPageIterator = sections->begin(); mPageIterator != sections->end(); mPageIterator++)
	{
		sec = *mPageIterator;
	}
	
	mPageIterator = sections->begin();
	mFetchRecord = true;
	mIsOverflow = false;
	
	DrawDynamicPage();
	
	
	return;
}


// ---------------------------------------------------------------------------
// DrawDynamicPage													 [private]
// ---------------------------------------------------------------------------
// Draw one page

void
ETReport::DrawDynamicPage (void)
{
	SRect							origRect, pageRect;
	ETSectionList					*sections;
	ETSectionList::const_iterator	sit;
	ETSection						*sec;
	int								breakLevel;
	ETWatermarkSection				*watermark = NULL;
	
	watermark = mData.GetWatermarkSection();
	
	if (watermark && not watermark->IsOnTop())
	{
		watermark->Export (this);
	}
	
	sections = mData.GetPageSections();
	// process headers & footers, draw headers
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = *sit;
		if (sec->GetKind() == ETSection::eSectionKind_Header)
		{
			if (sec->WillingToPrint (this))
			{
				if (mOutputOptions & eo_headers)
					sec->Export (this);
			}
		}
	}
	
	ETBreakSection	*bs;

	PeekNextSection();
	
	bool	isNewPage = true;
	
# if	_4D_Package_
	Yield4D();
# endif
	
	// draw body sections
	while (mCurrentBody != NULL)
	{
		
		if (GetNextSection())
			break;
		
		if (mCurrentBody->GetKind() == ETSection::eSectionKind_BreakHeader)
		{
			bs = static_cast <ETBreakSection*> (mCurrentBody);
			mBreakHeaders [mBreakLevel] = bs;
			for (breakLevel = 0; breakLevel <= mBreakLevels; breakLevel++)	//mbs 05012010	needs to reset mIsBreak
			{
				bs = mBreakHeaders [breakLevel];
				if (bs == NULL)		// no break header for this level
					continue;
				bs->ProcessBreak (mBreakLevel);
			}
		}
		
		{
			if (mBreakLevels >= 0 && mCurrentBody->GetKind() == ETSection::eSectionKind_Body)
			{
				{
					mCurrentBody->FetchCalcValues (this);
					mCalculator.Increment (this);
				}
				for (breakLevel = 0; breakLevel <= mBreakLevels; breakLevel++)	//mbs 05012010	needs to reset mIsBreak
				{
					bs = mBreakHeaders [breakLevel];
					if (bs == NULL)		// no break header for this level
						continue;
					bs->ProcessBreak (mBreakLevels + 1);
				}
			}
			
			if ( (
                  ((mCurrentBody->GetKind() == ETSection::eSectionKind_BreakHeader) ||
                   (mCurrentBody->GetKind() == ETSection::eSectionKind_BreakFooter))
				  &&  (mOutputOptions & eo_totals)
                  )
				|| (mCurrentBody->GetKind() == ETSection::eSectionKind_Body))
			{
					mCurrentBody->Export (this);
			}
		}
		
		{
			mFetchRecord = true;
			
			if (mCurrentBody->GetKind() == ETSection::eSectionKind_BreakFooter)
			{
				bs = static_cast <ETBreakSection*> (mCurrentBody);
				//				breakLevel = bs->GetLevel();
				if (mBreakLevels >= 0)
				{
					mCalculator.ShuntTotals (mBreakLevel);
					for (breakLevel = 0; breakLevel <= mBreakLevels; breakLevel++)	//mbs 05012010	needs to reset mIsBreak
					{
						bs = mBreakHeaders [breakLevel];
						if (bs == NULL)		// no break header for this level
							continue;
						bs->ProcessBreak (mBreakLevel);
					}
				}
			}
			
			isNewPage = false;
			if (PeekNextSection())
				break;
		}
	}
	
# if	_4D_Package_
	Yield4D();
# endif
	
	{
		// draw fill footers
		for (sit = sections->begin(); sit != sections->end(); sit++)
		{
			sec = *sit;
			if (sec->GetKind() == ETSection::eSectionKind_FillFooter)
			{
				if (sec->WillingToPrint (this))
				{
					if (mOutputOptions & eo_totals)
						sec->Export (this);
				}
			}
		}
	}
	
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = *sit;
		if (sec->GetKind() == ETSection::eSectionKind_Footer)
		{
			if (sec->WillingToPrint (this))
			{
				if (mOutputOptions & eo_headers)
					sec->Export (this);
			}
		}
	}
	
	{
		if (watermark && watermark->IsOnTop())
		{
			if (watermark->WillingToPrint (this))
			{
				watermark->Export (this);
			}
		}
				
	}
	
# if	_4D_Package_
	Yield4D();
# endif
	
	return;
}


const	float	cMinFloatValue = -INFINITY;

// ---------------------------------------------------------------------------
// PositionObjects													  [public]
// ---------------------------------------------------------------------------
// Position objects within a section/group

void
ETReport::PositionObjects (ETObjList *inObjects)
{
	ETObjList::const_iterator	it;
	if (mOutputOptions & eo_sortbyitem)
		std::sort<ETObjList::iterator, ETObjectCompareOrder> (inObjects->begin(), inObjects->end(), ETObjectCompareOrder());
	else 
		std::sort<ETObjList::iterator, ETObjectComparePosition> (inObjects->begin(), inObjects->end(), ETObjectComparePosition());
	
	return;
}

// ---------------------------------------------------------------------------
// PositionGroup														 [private]
// ---------------------------------------------------------------------------
// Set new positions of objects within a group

void
ETReport::PositionGroup (ETGroup* inGroup)
{
	PositionObjects (inGroup->GetObjects());
}


// ---------------------------------------------------------------------------
// GetFormat														 [private]
// ---------------------------------------------------------------------------
// one format per export; the order is the precedence of the old code (JSON added last)

ETReport::EFormat
ETReport::GetFormat (void)
const
{
	if (mOutputOptions & eo_text)
		return eFormat_Text;
	if (mOutputOptions & eo_html)
		return eFormat_HTML;
	if (mOutputOptions & eo_xml)
		return eFormat_XML;
	if (mOutputOptions & eo_json)
		return eFormat_JSON;
	return eFormat_None;
}


// ---------------------------------------------------------------------------
// JSON output														 [private]
// ---------------------------------------------------------------------------
// { "version": "1.0", "name": ..., "sections": [ { "type", "id", "items": [ ... ] } ] }

void
ETReport::JsonCloseSection (void)
{
	if (mJsonSectionOpen)
	{
		mJsonOut[u"sections"].PushBack (mJsonSection, mJsonOut.GetAllocator());
		mJsonSectionOpen = false;
	}
}

void
ETReport::JsonAddItem (const char *inType, ETObject *inObject, const RWString &inText, bool inAttributed)
{
	RWJsonAllocator	&alloc = mJsonOut.GetAllocator();

	if (!mJsonSectionOpen)		// items outside of a section
	{
		mJsonSection.SetObject();
		mJsonSection.AddMember (u"items", RWJsonValue (rapidjson::kArrayType), alloc);
		mJsonSectionOpen = true;
	}

	RWJsonValue	item (rapidjson::kObjectType);
	item.AddMember (u"type", RWJson::String (RWStr::FromASCII (inType), alloc), alloc);
	if (not inObject->mName.empty())
		item.AddMember (u"name", RWJson::String (inObject->mName, alloc), alloc);
	if (not inObject->mID.empty())
		item.AddMember (u"id", RWJson::String (inObject->mID, alloc), alloc);
	RWString	itemClass = static_cast <ETText*> (inObject)->GetClass();
	if (!itemClass.empty())
		item.AddMember (u"class", RWJson::String (itemClass, alloc), alloc);
	if (inAttributed)
	{
		item.AddMember (u"value", RWJson::String (RWTools::SplitAttributedString (inText, NULL), alloc), alloc);
		item.AddMember (u"styled", RWJson::String (inText, alloc), alloc);
	}
	else
		item.AddMember (u"value", RWJson::String (inText, alloc), alloc);

	mJsonSection[u"items"].PushBack (item, alloc);
}


// ---------------------------------------------------------------------------
// WriteText														 [private]
// ---------------------------------------------------------------------------
// report structure events: ReportStart, ReportEnd, SectionStart, SectionEnd, Delimiter

void
ETReport::WriteText (const char *inType, void * inObject)
{
	const EFormat	format = GetFormat();

	if (strcasecmp (inType, "ReportStart") == 0)
	{
		if (format == eFormat_HTML)
		{
			mTextOut.append (u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n");
			mTextOut.append (u"<html>\r\n");
			mTextOut.append (u"<head>\r\n");
			mTextOut.append (u"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\r\n");
			mTextOut.append (u"<title>");
			mTextOut.append (RWStr::EscapeXML (mName));
			mTextOut.append (u"</title>\r\n");
			mTextOut.append (u"</head>\r\n");
			mTextOut.append (u"<body>\r\n");
		}
		else if (format == eFormat_XML)
		{
			mXmlOut.AddDeclaration (u"utf-8", true);
			mXmlCurrent = mXmlOut.Node().Append (u"Report");
			mXmlCurrent.SetAttr (u"Version", u"1.0");
			if (not mName.empty())
				mXmlCurrent.SetAttr (u"Name", mName);
		}
		else if (format == eFormat_JSON)
		{
			RWJsonAllocator	&alloc = mJsonOut.GetAllocator();
			mJsonOut.AddMember (u"version", RWJson::String (u"1.0", alloc), alloc);
			if (not mName.empty())
				mJsonOut.AddMember (u"name", RWJson::String (mName, alloc), alloc);
			mJsonOut.AddMember (u"sections", RWJsonValue (rapidjson::kArrayType), alloc);
		}
		mStage = es_reportstart;
	}
	else if (strcasecmp (inType, "ReportEnd") == 0)
	{
		if (format == eFormat_HTML)
		{
			mTextOut.append (u"</body>\r\n");
			mTextOut.append (u"</html>\r\n");
		}
		else if (format == eFormat_JSON)
			JsonCloseSection();
	}
	else if (strcasecmp (inType, "SectionStart") == 0)
	{
		ETSection	*section = static_cast <ETSection*> (inObject);
		if (format == eFormat_HTML)
			mTextOut.append (u"<DIV>\r\n");
		else if (format == eFormat_XML)
		{
			mXmlCurrent = mXmlCurrent.Append (u"Section");
			if (not section->GetType().empty())
				mXmlCurrent.SetAttr (u"type", section->GetType());
			if (not section->GetID().empty())
				mXmlCurrent.SetAttr (u"id", section->GetID());
		}
		else if (format == eFormat_JSON)
		{
			RWJsonAllocator	&alloc = mJsonOut.GetAllocator();
			JsonCloseSection();
			mJsonSection.SetObject();
			if (not section->GetType().empty())
				mJsonSection.AddMember (u"type", RWJson::String (section->GetType(), alloc), alloc);
			if (not section->GetID().empty())
				mJsonSection.AddMember (u"id", RWJson::String (section->GetID(), alloc), alloc);
			mJsonSection.AddMember (u"items", RWJsonValue (rapidjson::kArrayType), alloc);
			mJsonSectionOpen = true;
		}
	}
	else if (strcasecmp (inType, "SectionEnd") == 0)
	{
		if (format == eFormat_Text)
			mTextOut.append (u"\r\n");
		else if (format == eFormat_HTML)
			mTextOut.append (u"</DIV>\r\n");
		else if (format == eFormat_XML)
		{
			if (mXmlCurrent.NameIs ("Section"))
				mXmlCurrent = mXmlCurrent.Parent();
		}
		else if (format == eFormat_JSON)
			JsonCloseSection();
	}
	else if (strcasecmp (inType, "Delimiter") == 0)
	{
		if (format == eFormat_Text)
			mTextOut.append (u"\t");
	}
#if	TARGET_DEBUG
	else
		printf ("ETReport::WriteText: unhandled case \"%s\"!\n", inType);
#endif
}


// ---------------------------------------------------------------------------
// WriteText														 [private]
// ---------------------------------------------------------------------------
// report items: "Text" (static text, only with eo_static) and "Variable"

void
ETReport::WriteText (const char *inType, ETObject * inObject, RWString &inText)
{
	const EFormat	format = GetFormat();
	ETText			*toText = static_cast<ETText*> (inObject);
	const bool		isAttributed = toText->IsAttributed ();
	const bool		isText = strcasecmp (inType, "Text") == 0;
	const bool		isVariable = strcasecmp (inType, "Variable") == 0;

	if (!(isText && (mOutputOptions & eo_static)) && !isVariable)
	{
#if	TARGET_DEBUG
		if (!isText)
			printf ("ETReport::WriteText2: unhandled case \"%s\"!\n", inType);
#endif
		return;
	}

	switch (format)
	{
		case eFormat_Text:
			mTextOut.append (isAttributed ? RWTools::SplitAttributedString (inText, NULL) : RWString (inText));
			break;

		case eFormat_HTML:
		{
			const char16_t	*tag = isText ? u"DIV" : u"span";
			mTextOut.append (u"<").append (tag);
			if (not inObject->mName.empty())
				mTextOut.append (u" name=\"").append (RWStr::EscapeXML (inObject->mName)).append (u"\"");
			if (not inObject->mID.empty())
				mTextOut.append (u" id=\"").append (RWStr::EscapeXML (inObject->mID)).append (u"\"");
			mTextOut.append (u">");
			mTextOut.append (RWStr::EscapeXML (inText));
			mTextOut.append (u"</").append (tag).append (u">\r\n");
			break;
		}

		case eFormat_XML:
		{
			RWXmlNode	elem = mXmlCurrent.Append (isText ? u"text" : u"variable");	// element names as the old writer produced them
			if (not inObject->mName.empty())
				elem.SetAttr (u"name", inObject->mName);
			if (not inObject->mID.empty())
				elem.SetAttr (u"id", inObject->mID);
			elem.SetAttr (u"class", toText->GetClass());
			elem.SetAttr (u"type", isText ? u"text" : u"variable");
			if (not inText.empty())
				elem.AppendText (inText);
			break;
		}

		case eFormat_JSON:
			JsonAddItem (isText ? "text" : "variable", inObject, inText, isAttributed);
			break;

		default:
			break;
	}
}
