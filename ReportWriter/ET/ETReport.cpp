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
fd (0),
mStage (e_OutputStage (0)),
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
ETReport::GetReportVariable (const CText inName)
const
{
	int	index;
	CText	name (inName);
	for (index = 0; index < RW_VarNamesRWCount; index++)
		if (name.compare(mVarNames [index]) == 0)
			return index;
	
	return -1;
}


// ---------------------------------------------------------------------------
// CreateVariable													  [public]
// ---------------------------------------------------------------------------

void
ETReport::CreateVariable (const CText inName, ECalcType inCalc)
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
ETReport::GetCalculatedValue (const CText inName, RWValue &outVar)
const
{
	return GetVariable (inName, outVar, ECalcType_CurrentValue);
}


// ---------------------------------------------------------------------------
// GetVariable														  [public]
// ---------------------------------------------------------------------------
// Get report variable

bool
ETReport::GetVariable (const CText inName, RWValue &outVar, ECalcType inCalc)
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
ETReport::SetVariable (const CText inName, RWValue *inVar)
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

RWTextValue
ETReport::FormatVariable (RWValue &inVar, const CText inFormat)
const
{
	return mSource.FormatVariable (inVar, inFormat);
}



// ---------------------------------------------------------------------------
// ReportToFile														  [public]
// ---------------------------------------------------------------------------
// Draw the report

void
ETReport::ReportToFile (FILE *inFile)
{
	fd = inFile;

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
// WriteText														 [private]
// ---------------------------------------------------------------------------
// writes to output

void			
ETReport::WriteText (const char *inType, void * inObject)
{
	if (strcasecmp (inType, "ReportStart") == 0)
	{
		{
			if (mOutputOptions & eo_text) 
			{
				// nothing to output 
			} else if (mOutputOptions & eo_html) 
			{
				fprintf (fd, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n");
				fprintf (fd, "<html>\r\n");
				fprintf (fd, "<head>\r\n");
				fprintf (fd, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\r\n");
				fprintf (fd, "<title>");
				//	const char *s = GetReport()->Attribute ("Name");
				if (not mName.IsEmpty())
				{
					CXMLText	name = mName.ToXML();
					fprintf (fd, "%s", name.c_str());
					mName.FreeXML (name);
				}			
				fprintf (fd, "</title>\r\n");
				fprintf (fd, "</head>\r\n");
				fprintf (fd, "<body>\r\n");
			} 
			else if (mOutputOptions & eo_xml) 
			{
				fprintf (fd, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\r\n");
				fprintf (fd, "<Report Version=\"1.0\"");
				//	const char *s = GetReport()->Attribute ("Name");
                if (not mName.IsEmpty())
                {
                    CXMLText	name = mName.ToXMLEscaped();
                    fprintf (fd, " Name=\"%s\"", name.c_str());
                    mName.FreeXML (name);
                }
				fprintf (fd, ">\r\n");
			}
			mStage = es_reportstart;
		}
	}
	else if (strcasecmp (inType, "ReportEnd") == 0)
	{
		{
			if (mOutputOptions & eo_text) 
			{
				// nothing to output 
			} 
			else if (mOutputOptions & eo_html) 
			{
				fprintf (fd, "</body>\r\n");
				fprintf (fd, "</html>\r\n");
			} 
			else if (mOutputOptions & eo_xml) 
			{
				fprintf (fd, "</Report>\r\n");
			}
		}
	}
	else if (strcasecmp (inType, "SectionStart") == 0)
	{
		{
			if (mOutputOptions & eo_text) 
			{
				// nothing to output 
			} 
			else if (mOutputOptions & eo_html) 
			{
				fprintf (fd, "<DIV>\r\n");
			} 
			else if (mOutputOptions & eo_xml) 
			{
                ETSection* section = (ETSection*) (inObject) ;
				fprintf (fd, "<Section");
                if (not section->GetType().IsEmpty())
                {
                    fprintf (fd, " type=\"");
                    RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8(section->GetType()) );
                    fprintf (fd, "\"");
                }
//                if (not section->GetName().IsEmpty())
//                {
//                    fprintf (fd, " name=\"");
//                    RWTools::WriteText (fd, section->GetName());
//                    fprintf (fd, "\"");
//                }
                if (not section->GetID().IsEmpty())
                {
                    fprintf (fd, " id=\"");
                    RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8(section->GetID()));
                    fprintf (fd, "\"");
                }
               fprintf (fd, ">\r\n");

			}
		}		
	}
	else if (strcasecmp(inType, "SectionEnd") == 0)
	{
		{
			if (mOutputOptions & eo_text) 
			{
				fprintf (fd, "\r\n");
			} 
			else if (mOutputOptions & eo_html) 
			{
				fprintf (fd, "</DIV>\r\n");
			} 
			else if (mOutputOptions & eo_xml) 
			{
				fprintf (fd, "</Section>\r\n");
			}
		}		
	}
	else if (strcasecmp (inType, "Delimiter") == 0)
	{
		{
			if (mOutputOptions & eo_text) 
			{
				fprintf (fd, "\t");
			} 
		}		
	}	
#if	TARGET_DEBUG
	else
		printf ("ETReport::WriteText: unhandled case \"%s\"!\n", inType);
#endif
}

void			
ETReport::WriteText (const char *inType, ETObject * inObject, RWTextValue &inText)
{
	CXMLText	text;
	ETText *	toText = static_cast<ETText*> (inObject);
	bool		isAttributed = toText->IsAttributed ();
	if (strcasecmp (inType, "Text") == 0)
	{
		{
			if (mOutputOptions & eo_static)
			{
				if (mOutputOptions & eo_text) 
				{
					RWTextValue		plainText;
					if (isAttributed)
						plainText.Attach(RWTools::SplitAttributedString(inText, NULL));	
					else 
						plainText = inText;
					
					text = plainText.ToXML();	// UTF8

					fprintf (fd, "%s", text.c_str());
					plainText.FreeXML (text);
					plainText.Free();

				} 
				else if (mOutputOptions & eo_html)
				{
					fprintf (fd, "<DIV");
					if (not inObject->mName.IsEmpty())
					{
						fprintf (fd, " name=\"");
						RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8 (inObject->mName));
						fprintf (fd, "\"");
					}
					if (not inObject->mID.IsEmpty())
					{
						fprintf (fd, " id=\"");
						RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8 (inObject->mID));
						fprintf (fd, "\"");
					}
					fprintf (fd, ">");				
					text = inText.ToXMLEscaped();	// UTF8
					fprintf (fd, "%s", text.c_str());
					inText.FreeXML (text);
					fprintf (fd, "</DIV>\r\n");
				}
				else if (mOutputOptions & eo_xml)
				{
					fprintf (fd, "<%s", inType);
					if (not inObject->mName.IsEmpty())
					{
						fprintf (fd, " name=\"");
						RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8 ( inObject->mName));
						fprintf (fd, "\"");
					}
					if (not inObject->mID.IsEmpty())
					{
						fprintf (fd, " id=\"");
						RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8 (inObject->mID));
						fprintf (fd, "\"");
					}
                    fprintf (fd, " class=\"");
                    RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8 (toText->GetClass()));
                    fprintf (fd, "\"");
                    fprintf (fd, " type=\"text\"");
					fprintf (fd, ">");
					text = inText.ToXMLEscaped();	// UTF8
					fprintf (fd, "%s", text.c_str());
					inText.FreeXML (text);
					fprintf (fd, "</%s>\r\n", inType);
				}
			}
		}
	}
	else if (strcasecmp (inType, "Variable") == 0)
	{
		{
			{
				if (mOutputOptions & eo_text)
				{
					RWTextValue		plainText;
                    if (isAttributed)
                        plainText.Attach(RWTools::SplitAttributedString(inText, NULL));
                    else
                        plainText = inText;
					text = plainText.ToXML();	// UTF8
                    if(text.length() > 0)
                         fprintf (fd, "%s", text.c_str());
					plainText.FreeXML (text);
					plainText.Free();
				} 
				else if (mOutputOptions & eo_html) 
				{
					fprintf (fd, "<span");
					if (not inObject->mName.IsEmpty())
					{
						fprintf (fd, " name=\"");
						RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8 (inObject->mName));
						fprintf (fd, "\"");
					}
					if (not inObject->mID.IsEmpty())
					{
						fprintf (fd, " id=\"");
						RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8 (inObject->mID));
						fprintf (fd, "\"");
					}
					fprintf (fd, ">");					
					text = inText.ToXMLEscaped();	// UTF8
                    if(text.length() > 0)
                        fprintf (fd, "%s", text.c_str());
					inText.FreeXML (text);
					fprintf (fd, "</span>\r\n");
				} 
				else if (mOutputOptions & eo_xml) 
				{
					fprintf (fd, "<variable");
					if (not inObject->mName.IsEmpty())
					{
						fprintf (fd, " name=\"");
						RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8 (inObject->mName));
						fprintf (fd, "\"");
					}
					if (not inObject->mID.IsEmpty())
					{
						fprintf (fd, " id=\"");
						RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8 (inObject->mID));
						fprintf (fd, "\"");
					}
                    fprintf (fd, " class=\"");
                    RWTools::WriteText (fd, RWTextValue::UTF_16_to_UTF8 (toText->GetClass()));
                    fprintf (fd, "\"");
                    fprintf (fd, " type=\"variable\"");
					fprintf (fd, ">");
					text = inText.ToXMLEscaped();	// UTF8
                    if(text.length() > 0)
                        fprintf (fd, "%s", text.c_str());
					inText.FreeXML (text);
					fprintf (fd, "</variable>\r\n");
				}
			}
		}		
	}
#if	TARGET_DEBUG
	else
		printf ("ETReport::WriteText2: unhandled case \"%s\"!\n", inType);
#endif
}
