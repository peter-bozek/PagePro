/*	variance computation

Therefore a simple algorithm to calculate variance can be described by the following pseudocode:
long n = 0;
double sum = 0;
double sum_sqr = 0;
double variance;

foreach x in data:
  n += 1;
  sum += x;
  sum_sqr += x*x;
end for

variance = (sum_sqr - sum*sum/n)/(n-1);


Another algorithm which avoids large numbers in sum_sqr while summing up
double avg = 0;
double var = 0;
long n = data.length; // number of elements
// the array is 1-indexed

for i = 1 to n
 avg = (avg*(i-1) + data[i]) / i;
 var = (var * (i - 1) + (data[i] - avg)*(data[i] - avg)) / i;
end for

return var; // resulting variance


A similar algorithm, slightly more efficient, which is also numerically stable. It also computes the mean. This algorithm is due to Knuth (The Art of Computer Programming: Semi-Numerical Algorithms), who cites Welford, in Technometrics v. 4, 1962, pp 419-420.
double mean = 0;
double S = 0;
long N;

for N = 1 to data.length
    double delta = data[N] - mean;
    mean = mean + delta / N;
    S = S + delta * ( data[N] - mean ); // yes this is the new mean
end for

return S / ( N - 1 ) // the variance

*/



# include	"RWCalculator.h"
# include	<math.h>

typedef	RWList<RWCalculatedValue*>	RWCVList;


RWCalculatedValue::SCalculatedValue::SCalculatedValue (void)
{
	Init();
}



void
RWCalculatedValue::SCalculatedValue::Init (void)
{
	fCount = 0;
	fSum = fVar = 0;
	fMin = INFINITY;
	fMax = -INFINITY;
}


RWCalculatedValue::RWCalculatedValue (const RWString inName)
	:	mLevels (0),
		mValues (0),
		mName (inName)
{
//	mValues = new SCalculatedValue [inLevels];
	return;
}


RWCalculatedValue::~RWCalculatedValue (void)
{
	if (mValues)
		delete [] mValues;

	return;
}


void
RWCalculatedValue::InitLevels (int inLevels)
{
	mLevels = inLevels;
	if (mValues != NULL)
		delete [] mValues;
	mValues = new SCalculatedValue [inLevels];
	mLast.Init();
	return;
}


const RWString
RWCalculatedValue::GetName (void)
const
{
	return mName;
}


void
RWCalculatedValue::Increment (double newValue)
{
	Push();
	for (int i = 0; i < mLevels; i++)
	{
		mValues [i].fCount++;
		if (isnan (newValue) || isinf (newValue))
			;
		else
		{
			mValues [i].fSum += newValue;
			if (mValues [i].fCount > 1)
			{
				if (mValues [i].fMin > newValue)
					mValues [i].fMin = newValue;
				if (mValues [i].fMax < newValue)
					mValues [i].fMax = newValue;

				double	avg = mValues [i].fSum / mValues [i].fCount;
				mValues [i].fVar =
						(mValues [i].fCount - 1) * mValues [i].fVar / mValues [i].fCount
					+	(avg - newValue) * (avg - newValue) / (mValues [i].fCount - 1);
			}
			else
			{
				mValues [i].fMin = newValue;
				mValues [i].fMax = newValue;
			}
		}
	}
	return;
}


double
RWCalculatedValue::GetValue (RWCalculatedType inWhich, int inLevel)
const
{
//if (inLevel < 0 || inLevel >= mLevels)	DebugStr ("\pInvalid Level");
assert (inLevel >= 0 && inLevel < mLevels);

	double				rv;
	const SCalculatedValue*		v = &mValues[inLevel];

	/*		currently, it is not possible to distinguis for headers and footers which value to take - old or current
			if any thing from body fits, it is current, which may not be correct pB 2012
	if (inLevel == 0) {
		v = (&mLast);
	} else {
		v = &mValues[inLevel];
	}
	 */
	
	switch (inWhich)
	{
		case RWCalculatedType_Count:	rv = v->fCount; break;
		case RWCalculatedType_Sum:		rv = v->fSum; break;
		case RWCalculatedType_Min:		rv = (v->fMin == INFINITY ? 0 : v->fMin); break;
		case RWCalculatedType_Max:		rv = (v->fMax == -INFINITY ? 0 : v->fMax); break;
		case RWCalculatedType_Var:		rv = v->fVar; break;
		case RWCalculatedType_Avg:		rv = (v->fCount > 0 ? v->fSum / v->fCount : 0); break;
		case RWCalculatedType_Dev:		rv = sqrt (v->fVar); break;
		default:						rv = 0; break;
	}

	return rv;
}

double
RWCalculatedValue::GetOldValue (RWCalculatedType inWhich)
const
{
	double				rv;
	
	switch (inWhich)
	{
		case RWCalculatedType_Count:	rv = mLast.fCount; break;
		case RWCalculatedType_Sum:		rv = mLast.fSum; break;
		case RWCalculatedType_Min:		rv = (mLast.fMin == INFINITY ? 0 : mLast.fMin); break;
		case RWCalculatedType_Max:		rv = (mLast.fMax == -INFINITY ? 0 : mLast.fMax); break;
		case RWCalculatedType_Var:		rv = mLast.fVar; break;
		case RWCalculatedType_Avg:		rv = (mLast.fCount > 0 ? mLast.fSum / mLast.fCount : 0); break;
		case RWCalculatedType_Dev:		rv = sqrt (mLast.fVar); break;
		default:						rv = 0; break;
	}
	
	return rv;
}


void
RWCalculatedValue::Shunt (int inLevel)
{
//if (inLevel < 0 || inLevel >= mLevels)	DebugStr ("\pInvalid Level");
assert (inLevel >= 0 && inLevel < mLevels);
	
	Push();
	mValues [inLevel].Init();
}

void
RWCalculatedValue::Push (void)
{
	//if (inLevel < 0 || inLevel >= mLevels)	DebugStr ("\pInvalid Level");
	if (mLevels >= 0) {
		mLast = mValues[0];	
	} else {
		mLast.Init();
	}
}



RWCalculator::RWCalculator (void)
	:	mLevels (0),
		mLevel (0)
{
}


RWCalculator::~RWCalculator (void)
{
	{
		RWCVList::const_iterator	it;

		for (it = mVariables.begin(); it != mVariables.end(); it++)
		{
			RWCalculatedValue	*var = *it;
			delete var;
		}
	}
}


void
RWCalculator::InitLevels (int inLevels)
{
//if (inLevels < 0)	DebugStr ("\pInvalid Level");
assert (inLevels >= 0);

	mLevels = inLevels;
	mLevel = inLevels - 1;

	{
		RWCVList::const_iterator	it;

		for (it = mVariables.begin(); it != mVariables.end(); it++)
		{
			RWCalculatedValue	*var = *it;
			var->InitLevels (inLevels);
		}
	}
}


RWCalculatedValue*
RWCalculator::Get (const RWString inName)
const
{
	RWCVList::const_iterator	it;
	RWCalculatedValue			*var = NULL;
	for (it = mVariables.begin(); it != mVariables.end(); it++)
	{
		var = *it;
		if ( var->GetName() == inName)
			break;
	}
	if (it == mVariables.end())
		var = NULL;

	return var;
}


void
RWCalculator::Add (const RWString inName)
{
	RWCalculatedValue	*var = Get (inName);

	if (var == NULL)
	{
		var = new RWCalculatedValue (inName);
		mVariables.push_back (var);
	}

	return;
}


void
RWCalculator::Increment (const RWCalcDataProvider *inData)
{
	RWCVList::const_iterator	it;
	RWCalculatedValue			*var;
	RWValue						value;
	for (it = mVariables.begin(); it != mVariables.end(); it++)
	{
		var = *it;
		if (inData->GetCalculatedValue (var->GetName(), value))
		{
			switch (value.GetKind())
			{
				case RWValue::eValue_Boolean:
				case RWValue::eValue_Integer:
				case RWValue::eValue_DateTime:
				case RWValue::eValue_Date:
				case RWValue::eValue_Time:
					var->Increment ((double) value.GetInteger());
					break;

				case RWValue::eValue_Real:
					var->Increment (value.GetReal());
					break;

				case RWValue::eValue_Text:
					var->Increment (double (not value.IsEmpty()));
					break;

				case RWValue::eValue_PictRefScreen:
				case RWValue::eValue_PictRefPrint:
					var->Increment (double (value.GetInteger() != 0));
					break;

				case RWValue::eValue_BLOB:
				case RWValue::eValue_PicturePICT:
				case RWValue::eValue_PicturePDF:
				case RWValue::eValue_PictureJPG:
				case RWValue::eValue_PicturePNG:
				case RWValue::eValue_PictureTIFF:
				case RWValue::eValue_PictureEMF:
					var->Increment (double (value.GetBlobSize() != 0));
					break;

				default:
					break;
			}
		}
	}

	return;
}


bool
RWCalculator::GetValue (const RWString inName, RWCalculatedValue::RWCalculatedType inWhich, double &outValue)
const
{
	bool					found = false;
	const RWCalculatedValue	*var = Get (inName);

	if (var != NULL)
	{
		found = true;
		outValue = var->GetValue (inWhich, mLevel);
	}

	return found;
}


int
RWCalculator::GetLevel (void)
const
{
	return mLevel;
}


void
RWCalculator::SetLevel (int inLevel)
{
//if (inLevel < 0 || inLevel >= mLevels)	DebugStr ("\pInvalid Level");
assert (inLevel >= 0 && (inLevel == 0 || inLevel < mLevels));

	mLevel = inLevel;
	return;
}


void
RWCalculator::ShuntTotals (int inLevel)
{
//if (inLevel < 0 || inLevel >= mLevels)	DebugStr ("\pInvalid Level");
assert (inLevel >= 0 && inLevel < mLevels);

	RWCVList::const_iterator	it;
	RWCalculatedValue			*var;
	for (it = mVariables.begin(); it != mVariables.end(); it++)
	{
		var = *it;
		var->Shunt (inLevel);
	}

	return;
}
