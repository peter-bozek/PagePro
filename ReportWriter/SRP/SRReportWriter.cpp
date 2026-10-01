# include	"SRReportWriter.h"
# include	"SRReportData.h"
# include	"SRDataSource.h"
# include	"RWReportVariables.h"
# include	"SRDataFormatter.h"
//# include	"4DPluginAPI.h"
extern	"C"		void Yield4D (void);


// ---------------------------------------------------------------------------
// SRReportWriter							Constructor				  [public]
// ---------------------------------------------------------------------------

SRReportWriter::SRReportWriter (SRDataSource &inDataSource, SRReportData &inData, long inFlags)
	:	mSource (inDataSource),
		mData (inData),
		mFlags (inFlags & ~1),	//mbs 05112010
		mBreakLevels (-1),
		mCurrentIteration (0),
		mOutXML (NULL)
{
	mSource.SetReportWriter (this);
	mData.SetReportWriter (this);
	SRInitReportVariable (mVarNames);
	return;
}


// ---------------------------------------------------------------------------
// ~SRReportWriter							Destructor				  [public]
// ---------------------------------------------------------------------------

SRReportWriter::~SRReportWriter (void)
{
	return;
}


// ---------------------------------------------------------------------------
// GetCalculatedValue												  [public]
// ---------------------------------------------------------------------------

bool
SRReportWriter::GetCalculatedValue (const RWString inName, RWValue &outVar)
const
{
	bool	found = false;

	if (IsReportVariable (inName))
	{
		outVar.SetInteger (1L);
		found = true;
	}
	else
	{
		found = GetField (inName, outVar, ECalcType_CurrentValue);
		if (not found)
			found = GetVariable (inName, mCurrentIteration, outVar, ECalcType_CurrentValue);
	}

	return found;
}


// ---------------------------------------------------------------------------
// IsReportVariable													  [public]
// ---------------------------------------------------------------------------

bool
SRReportWriter::IsReportVariable (const RWString inName)
const
{
	return IsReportVariable (inName, 0, RW_VarNamesSRCount) >= 0;
}


// ---------------------------------------------------------------------------
// IsSRReportVariable												  [public]
// ---------------------------------------------------------------------------

int
SRReportWriter::IsSRReportVariable (const RWString inName)
const
{
	return IsReportVariable (inName, RW_VarNamesRWCount, RW_VarNamesSRCount);
}


// ---------------------------------------------------------------------------
// IsRWReportVariable												  [public]
// ---------------------------------------------------------------------------

bool
SRReportWriter::IsRWReportVariable (const RWString inName)
const
{
	return IsReportVariable (inName, 0, RW_VarNamesRWCount) >= 0;
}


// ---------------------------------------------------------------------------
// IsReportVariable												   [protected]
// ---------------------------------------------------------------------------

int
SRReportWriter::IsReportVariable (const RWString inName, int inFrom, int inTo)
const
{
	int			index;
    RWString   	name (inName);
	for (index = inFrom; index < inTo; index++)
	{
		if (name == RWString (mVarNames [index]))
			return index;
	}
	
	return RW_VarNotFound;
}


// ---------------------------------------------------------------------------
// IsCalculatedVariable												  [public]
// ---------------------------------------------------------------------------

bool
SRReportWriter::IsCalculatedVariable (const RWString inName)
const
{
	bool	found = true;
	if (not IsReportVariable (inName))
		found = (mCalculator.Get (inName) != NULL);

	return found;
}


// ---------------------------------------------------------------------------
// CreateVariable													  [public]
// ---------------------------------------------------------------------------

SR4DData*
SRReportWriter::CreateVariable (const RWString inName, long inIndex, ECalcType inCalc)
{
	SR4DData	*var = NULL;

	if (not IsReportVariable (inName))
	{
		// create break value computation
		if (inCalc > ECalcType_None)
			mCalculator.Add (inName);

		// create the variable
		var = mSource.CreateVariable (inName, inIndex);
	}

	return var;
}


// ---------------------------------------------------------------------------
// GetVariable														  [public]
// ---------------------------------------------------------------------------
// Get report variable

bool
SRReportWriter::GetVariable (const RWString inName, long inIndex, RWValue &outVar, ECalcType inCalc)
const
{
	bool	found = true;
	int		index;

	outVar.Free();
	if (IsRWReportVariable (inName))
	{
		outVar.SetInteger (1L);
		return found;
	}
	else if ((index = IsSRReportVariable (inName)) >= 0)
	{
		found = mSource.GetStdVariable (index, outVar);
		return found;
	}

	// get break value
	else if (inCalc > ECalcType_None)
	{
		double	d;
		found = mCalculator.GetValue (inName, RWCalculatedValue::RWCalculatedType (inCalc), d);
		if (found)
			outVar.SetReal (d);
	}
	else
		found = mSource.GetVariable (inName, inIndex, outVar, inCalc == ECalcType_OldValue);

	return found;
}


// ---------------------------------------------------------------------------
// GetVariable														  [public]
// ---------------------------------------------------------------------------
//mbs 05082010

void
SRReportWriter::GetVariable (const RWString inName, RWValue &outVar)
const
{
	int		index;

	outVar.Free();
	if (IsRWReportVariable (inName))
	{
		outVar.SetInteger (1L);
	}
	else if ((index = IsSRReportVariable (inName)) >= 0)
	{
		mSource.GetStdVariable (index, outVar);
	}
	else
		mSource.GetVariable (inName, outVar);

	return;
}


// ---------------------------------------------------------------------------
// CreateField														  [public]
// ---------------------------------------------------------------------------

SR4DData*
SRReportWriter::CreateField (const RWString inName, ECalcType inCalc)
{
	SR4DData	*fld = NULL;

	// create break value computation
	if (inCalc > ECalcType_None)
		mCalculator.Add (inName);

	// create the field
	fld = mSource.CreateField (inName);

	return fld;
}


// ---------------------------------------------------------------------------
// GetField															  [public]
// ---------------------------------------------------------------------------
// Get report field

bool
SRReportWriter::GetField (const RWString inName, RWValue &outVar, ECalcType inCalc)
const
{
	bool	found = true;

	outVar.Free();

	// get break value
	if (inCalc > ECalcType_None)
	{
		double	d;
		found = mCalculator.GetValue (inName, RWCalculatedValue::RWCalculatedType (inCalc), d);
		if (found)
			outVar.SetReal (d);
	}
	else
		found = mSource.GetField (inName, outVar, inCalc == ECalcType_OldValue);

	return found;
}


// ---------------------------------------------------------------------------
// CreateBreak														  [public]
// ---------------------------------------------------------------------------

SR4DData*
SRReportWriter::CreateBreak (const RWString inName, SRBreakSection::EBreakOn inBreak)
{
	SR4DData	*brk = NULL;

	switch (inBreak)
	{
		case SRBreakSection::eBreakOn_Field:
			brk = mSource.CreateField (inName);
			break;

		case SRBreakSection::eBreakOn_Variable:
			brk = mSource.CreateVariable (inName, SR4DVariable::SR4DVariable_Variable);
			break;

		case SRBreakSection::eBreakOn_Array:
			brk = mSource.CreateVariable (inName, SR4DVariable::SR4DVariable_ArrayAuto);
			break;

		case SRBreakSection::eBreakOn_None:
			break;
	}

//	if (brk != NULL && mBreaks.find (inName) == mBreaks.end())
//		mBreaks.insert (inName, brk);

	return brk;
}


// ---------------------------------------------------------------------------
// FormatVariable													  [public]
// ---------------------------------------------------------------------------
// Format variable depending on DataSource

RWString
SRReportWriter::FormatVariable (const RWValue &inVar, const RWString inFormat)
const
{
	return SRDataFormatter::FormatVariable (inVar, inFormat);
}


// ---------------------------------------------------------------------------
// Report														   [protected]
// ---------------------------------------------------------------------------
// Generate the report

void
SRReportWriter::Report (void)
{
	// create Sections/Objects from XML
	mData.ParseReport();

Yield4D();

	// give the DataSource a chance to get data...
	mSource.ParseReport (mData.GetReport());

	mSource.Reset();	// compute iterations, ...
	mCurrentIteration = 0;
	InitBreakTable();

	if (mOutXML != NULL)
	{
		mOutXMLRoot = mData.Write (*mOutXML);
		mSource.Write (mOutXMLRoot);
	}

Yield4D();

	FillReport();

	mSource.Close();	// run end script, ...

	if (mOutXML != NULL)
		mSource.WriteReportData (mOutXMLRoot);

	return;
}


// ---------------------------------------------------------------------------
// ReportToXML														  [public]
// ---------------------------------------------------------------------------
// Generate the report as an RWXML document ("Report" root element).
// The old version returned NULL: ReportToFile deleted the document it built.

std::unique_ptr<RWXmlDocument>
SRReportWriter::ReportToXML (void)
{
	std::unique_ptr<RWXmlDocument>	doc (new RWXmlDocument);

	mOutXML = doc.get();
	try
	{
		Report();
	}
	catch (...)
	{
		mOutXML = NULL;
		mOutXMLRoot = RWXmlNode();
		throw;
	}
	mOutXML = NULL;
	mOutXMLRoot = RWXmlNode();

	return doc;
}


// ---------------------------------------------------------------------------
// InitBreakTable													 [private]
// ---------------------------------------------------------------------------

void
SRReportWriter::InitBreakTable (void)
{
	SRSectionList					*sections;
	SRSectionList::const_iterator	sit;
	SRBreakSection					*sec;
	int								breakLevel;

	mBreakLevels = -1;
	sections = mData.GetBreakHeaders();
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = static_cast <SRBreakSection*> (*sit);
		breakLevel = sec->GetLevel();
		if (breakLevel > mBreakLevels)
			mBreakLevels = breakLevel;
	}

	sections = mData.GetBreakFooters();
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = static_cast <SRBreakSection*> (*sit);
		breakLevel = sec->GetLevel();
		if (breakLevel > mBreakLevels)
			mBreakLevels = breakLevel;
	}

	//mbs 20092010	create break for totals
	if (mBreakLevels == -1 && mCalculator.GetVariables().size() > 0)
		mBreakLevels = 0;

	mCalculator.InitLevels (mBreakLevels + 1);

	//mbs 29112005	create empty break footers if required - needed for totals shunting
	if (	mBreakLevels >= 0
		&&	mData.GetBodySection() != NULL
		&&	mData.GetBodySection()->NeedsProcessing()
	)
	{
		std::vector<char>	breaks (size_t (mBreakLevels + 1), 0);

		sections = mData.GetBreakHeaders();
		for (sit = sections->begin(); sit != sections->end(); sit++)
		{
			sec = static_cast <SRBreakSection*> (*sit);
			breakLevel = sec->GetLevel();
			breaks [breakLevel] = 1;
		}

		sections = mData.GetBreakFooters();
		for (sit = sections->begin(); sit != sections->end(); sit++)
		{
			sec = static_cast <SRBreakSection*> (*sit);
			breakLevel = sec->GetLevel();
			breaks [breakLevel] |= 2;
		}

		for (breakLevel = 1; breakLevel <= mBreakLevels; breakLevel++)
		{
			if (breaks [breakLevel] == 1)	// only header is present?
			{
				sec = new SRBreakSection (&mData, u"BreakFooter", breakLevel);
				sections->push_back (sec);
			}
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// FillReport														 [private]
// ---------------------------------------------------------------------------
// Fill the report with data

void
SRReportWriter::FillReport (void)
{
	bool	processWatermark = true;
	bool	processHeader = true;
	bool	processTotal = false;
	int		thisBreakLevel = 0, breakLevel;

	SRPageSection	*body = mData.GetBodySection();

	if (body && not mData.IsSimple())
		mEmitCalculator = body->NeedsProcessing();
	else
		mEmitCalculator = false;

	if (mOutXML != NULL)
		mOutXMLRoot = body->WritePage (mOutXMLRoot, mData.IsSimple(), true);

	long	numIterations = mSource.GetNumberOfIterations();

	for ( ; ; )
	{
		if (mSource.FetchNextRecord())
		{
			mCurrentIteration++;
		}
		else
			processTotal = true;

		if (processWatermark)
		{
			if (mData.GetWatermarkSection())
				FillOneSection (mData.GetWatermarkSection(), true, false);
			processWatermark = false;
		}

		if (processHeader)
		{
			FillHeadersFooters ("Header");
			processHeader = false;
		}

		if (numIterations > 0 && (mCurrentIteration == 1 || processTotal || (thisBreakLevel = CheckBreak()) <= mBreakLevels))
		{
			// break headers
			if (processTotal || (mCurrentIteration > 1)) // pB 20100811
			{
				if (processTotal)
					thisBreakLevel = 1;
				for (breakLevel = mBreakLevels; breakLevel >= thisBreakLevel; breakLevel--)
				{
					ProcessBreak (breakLevel, true, not processTotal);
				}
			}
			else
				// all break headers on first iteration
				thisBreakLevel = 0;

			if (not processTotal)
				// break headers
				for (breakLevel = thisBreakLevel; breakLevel <= mBreakLevels; breakLevel++)
				{
					ProcessBreak (breakLevel, false, false);
				}

			if (mBreakLevels >= 0)
				mCalculator.SetLevel (mBreakLevels);
		}

Yield4D();

		if (processTotal)
		{
//			Update4DCalcVariables (thisBreakLevel);
			ProcessBreak (0, true, false);
			FillHeadersFooters ("Footer");
			break;
		}
		else
		{
			if (mBreakLevels >= 0)
//				mCalculator.Increment (static_cast <RWCalcDataProvider*> (&mSource));
				mCalculator.Increment (this);
//			Update4DCalcVariables (-1);
			if (body)
			{
				body->FetchValues (false); // pB 2011-8-25 moved 
				FillOneSection (body, false, false);
			}
		}
	}

	if (mOutXML != NULL)
		mOutXMLRoot = body->WritePage (mOutXMLRoot, mData.IsSimple(), false);
}


// ---------------------------------------------------------------------------
// FillHeadersFooters												 [private]
// ---------------------------------------------------------------------------
// process headers or footers

void
SRReportWriter::FillHeadersFooters (const char *inWhich)
{
	SRSectionList					*sections = mData.GetPageSections();
	SRSectionList::const_iterator	sit;

	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		SRSection	*sec = *sit;
		if (RWStr::EqualsNoCase (sec->GetType(), inWhich))
			FillOneSection (sec, true, false);
	}

	return;
}


// ---------------------------------------------------------------------------
// ProcessBreak														 [private]
// ---------------------------------------------------------------------------
// process break headers or footers

void
SRReportWriter::ProcessBreak (int inLevel, bool isFooter, bool emitIfEmpty)
{
	if (mBreakLevels >= 0)
		mCalculator.SetLevel (inLevel);

	SRSectionList					*sections = isFooter ? mData.GetBreakFooters() : mData.GetBreakHeaders();
	SRSectionList::const_iterator	sit;

	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		SRBreakSection	*sec = static_cast <SRBreakSection*> (*sit);
		if (sec->GetLevel() == inLevel)
			if (emitIfEmpty || not sec->IsEmpty())
				FillOneSection (sec, true, isFooter);
	}

	if (mBreakLevels >= 0)
		mCalculator.ShuntTotals (inLevel);

	return;
}


// ---------------------------------------------------------------------------
// CheckBreak														 [private]
// ---------------------------------------------------------------------------
// check for a break

int
SRReportWriter::CheckBreak (void)
{
	SRSectionList					*sections;
	SRSectionList::const_iterator	sit;
	SRBreakSection					*sec;
	int								thisBreakLevel = mBreakLevels + 1, breakLevel;

	sections = mData.GetBreakHeaders();
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = static_cast <SRBreakSection*> (*sit);
		breakLevel = sec->GetLevel();
		if (breakLevel < thisBreakLevel && sec->IsBreak (mCurrentIteration))
			thisBreakLevel = breakLevel;
	}

	sections = mData.GetBreakFooters();
	for (sit = sections->begin(); sit != sections->end(); sit++)
	{
		sec = static_cast <SRBreakSection*> (*sit);
		breakLevel = sec->GetLevel();
		if (breakLevel < thisBreakLevel && sec->IsBreak (mCurrentIteration))
			thisBreakLevel = breakLevel;
	}

	return thisBreakLevel;
}


// ---------------------------------------------------------------------------
// FillOneSection													 [private]
// ---------------------------------------------------------------------------
// Fill the report with data for one section

void
SRReportWriter::FillOneSection (SRSection *inSection, bool inFetch, bool inUseOld)
{
	if (inFetch)
		inSection->FetchValues (inUseOld);
	inSection->FetchCalcValues();
	if (	(inUseOld && mEmitCalculator)	//mbs 29112005	always emit break footers
		||	inSection->NeedsProcessing()
	)
	{
		if (mOutXML)
			inSection->Write (mOutXMLRoot, mEmitCalculator);
	}
}
