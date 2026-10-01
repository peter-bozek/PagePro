/*
 *  ETObject.cpp
 *  ReportWriter
 *
 *  Created by Peter Bozek on 12/11/2010.
 *  Copyright 2010 INFORCE Bratislava. All rights reserved.
 *
 */

#include "ETObject.h"

# include	"ETObject.h"
# include	"ETReportData.h"
# include	"RWDataSource.h"
# include	"ETReport.h"


// ---------------------------------------------------------------------------
// ETObject									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

ETObject::ETObject (int inOrder)
:	mReportData (0),
mSeqID (inOrder),
mPosition (0, 0, 0, 0),
mDraw (eDraw_Yes)
{
}

// ---------------------------------------------------------------------------
// ~ETObject								Destructor			   [protected]
// ---------------------------------------------------------------------------

ETObject::~ETObject (void)
{
}


// ---------------------------------------------------------------------------
// GetReportData													  [public]
// ---------------------------------------------------------------------------

const ETReportData*
ETObject::GetReportData (void)
const
{
	return mReportData;
}


// ---------------------------------------------------------------------------
// GetReportWriter													  [public]
// ---------------------------------------------------------------------------

ETReport*
ETObject::GetReportWriter (void)
const
{
	return mReportData->GetReportWriter();
}


// ---------------------------------------------------------------------------
// GetDataSource													  [public]
// ---------------------------------------------------------------------------

RWDataSource&
ETObject::GetDataSource (void)
const
{
	return mReportData->GetReportWriter()->GetDataSource();
}


// ---------------------------------------------------------------------------
// ComparePosition													  [public]
// ---------------------------------------------------------------------------

int
ETObject::ComparePosition (const ETObject *other)
const
{
	if (mPosition.top < other->mPosition.top)
		return -1;
	else if (mPosition.top > other->mPosition.top)
		return 1;
	else if (mPosition.left < other->mPosition.left)
		return -1;
	else if (mPosition.left > other->mPosition.left)
		return 1;
	return 0;
}


// ---------------------------------------------------------------------------
// CompareOrder														  [public]
// ---------------------------------------------------------------------------

int
ETObject::CompareOrder (const ETObject *other)
const
{
	if (mSeqID < other->mSeqID)
		return -1;
	else if (mSeqID > other->mSeqID)
		return 1;
	
	return 0;	// should not happen - seqID should be unique within a section...
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
ETObject::Parse (ETReportData *inReport, RWXmlNode inNode)
{
	mReportData = inReport;
	
	long		    lVal;
	
	for (const auto &[name, value] : inNode.Attributes())
	{
		
		if (STR_EQUALS (name, "r"))
		{
			mPosition = value.c_str();
		}
		else if (STR_EQUALS (name, "t"))
		{
			mPosition.top = 0;
			RWStr::ReadNumber (value, mPosition.top);
		}
		else if (STR_EQUALS (name, "l"))
		{
			mPosition.left = 0;
			RWStr::ReadNumber (value, mPosition.left);
		}
		else if (STR_EQUALS (name, "b"))
		{
			mPosition.bottom = 0;
			RWStr::ReadNumber (value, mPosition.bottom);
		}
		else if (STR_EQUALS (name, "ri"))
		{
			mPosition.right = 0;
			RWStr::ReadNumber (value, mPosition.right);
		}
		else if (STR_EQUALS (name, "h") || STR_EQUALS (name, "height"))
		{
			mPosition.bottom = 0;
			RWStr::ReadNumber (value, mPosition.bottom);
			mPosition.bottom += mPosition.top;
		}
		else if (STR_EQUALS (name, "w") || STR_EQUALS (name, "width"))
		{
			mPosition.right = 0;
			RWStr::ReadNumber (value, mPosition.right);
			mPosition.right += mPosition.left;
		}
		else if (STR_EQUALS (name, "draw"))
		{
			if (RWStr::ReadNumber (value, lVal))
			{
				if (lVal >= eDraw_No && lVal <= eDraw_Always)
					mDraw = EDraw (lVal);
			}
			else if (STR_EQUALS (value, "no"))
				mDraw = eDraw_No;
			else if (STR_EQUALS (value, "yes"))
				mDraw = eDraw_Yes;
			else if (STR_EQUALS (value, "on overflow"))
				mDraw = eDraw_OnOverflow;
			else if (STR_EQUALS (value, "always"))
				mDraw = eDraw_Always;
		}
		//mbs 15112010
		else if (STR_EQUALS (name, "name"))
			mName.FromXML (value.c_str());
		else if (STR_EQUALS (name, "id"))
			mID.FromXML (value.c_str());
	}
		
	return;
}


// ---------------------------------------------------------------------------
// GetPosition														  [public]
// ---------------------------------------------------------------------------

SRect
ETObject::GetPosition (void)
const
{
	return mPosition;
}


// ---------------------------------------------------------------------------
// WillingToPrint													  [public]
// ---------------------------------------------------------------------------

bool
ETObject::WillingToPrint ()
const
{
	bool	draw = true;
	
	if (mDraw == eDraw_No)
			draw = false;	
	
	return draw;
}

// ---------------------------------------------------------------------------
// FetchCalcValue													  [public]
// ---------------------------------------------------------------------------

void
ETObject::FetchCalcValue (ETReport *inWriter)
{
	return;
}


// ---------------------------------------------------------------------------
// GetVariableText												   [protected]
// ---------------------------------------------------------------------------

RWTextValue
ETObject::GetVariableText (const CText inVariableName, const CText inFormat)
const
{
	RWValue		var;
	RWTextValue	result;
	
	if (GetReportWriter()->GetVariable (inVariableName, var))
		result = GetReportWriter()->FormatVariable (var, inFormat);
	
	return result;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

ETGroup*
ETGroup::Create (ETReportData *inReport, RWXmlNode inNode, int inOrder)
{
	ETGroup	*group = new ETGroup (inOrder);
	group->Parse (inReport, inNode);
	
	return group;
}


// ---------------------------------------------------------------------------
// ETGroup									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

ETGroup::ETGroup (int inOrder)
:	ETObject (inOrder),
mEmptyByChild (false)
{
}


// ---------------------------------------------------------------------------
// ~ETGroup									Destructor			   [protected]
// ---------------------------------------------------------------------------

ETGroup::~ETGroup (void)
{
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
ETGroup::Parse (ETReportData *inReport, RWXmlNode inNode)
{
	ETObject::Parse (inReport, inNode);
		
	return;
}


// ---------------------------------------------------------------------------
// FetchCalcValue													  [public]
// ---------------------------------------------------------------------------

void
ETGroup::FetchCalcValue (ETReport *inWriter)
{
	ETObjList::const_iterator	it;
	ETObject					*obj;
	
	for (it = mObjects.begin(); it != mObjects.end(); it++)
	{
		obj = *it;
		obj->FetchCalcValue (inWriter);
	}
	
	return;
}


// ---------------------------------------------------------------------------
// Export																  [public]
// ---------------------------------------------------------------------------

void
ETGroup::Export ()
{
	
	GetReportWriter()->WriteText ("GroupStart", this);

	if (not mEmptyByChild)	
	{
		ETObjList::const_iterator	it;
		ETObject					*obj;
		
		for (it = mObjects.begin(); it != mObjects.end(); it++)
		{
			obj = *it;
			if (obj->WillingToPrint ())
			{
				if (it != mObjects.begin())
					GetReportWriter()->WriteText ("Delimiter", this);
				obj->Export ();
			}
		}
	}
	
	GetReportWriter()->WriteText ("GroupEnd", this);
		
	return;
}


ETObjList *
ETGroup::GetObjects (void)
{
	return &mObjects;
}


#pragma	mark	-

#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

ETText*
ETText::Create (ETReportData *inReport, RWXmlNode inNode, int inOrder)
{
	ETText	*text = new ETText (inOrder);
	text->Parse (inReport, inNode);
	
	return text;
}


// ---------------------------------------------------------------------------
// ETText									Default Constructor	   [protected]
// ---------------------------------------------------------------------------

ETText::ETText (int inOrder)
:	ETObject (inOrder),
mIsDynamic (false),
mIsAttributed (false),
mDrawIfEmpty (eEmpty_Draw),
mStyle (0)
{
}

// ---------------------------------------------------------------------------
// ~ETText									Destructor			   [protected]
// ---------------------------------------------------------------------------

ETText::~ETText (void)
{	
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
ETText::Parse (ETReportData *inReport, RWXmlNode inNode)
{
	ETObject::Parse (inReport, inNode);
	long	mStyleID = 0;
	
	RWXmlNode	elem = inNode.Child (GetKind() == eObject_Text ? u"TextProps" : u"VariableProps");
	if (!elem)
		elem = inNode;
	
	if (elem)
	{
		long			    lVal;
		
		for (const auto &[name, value] : elem.Attributes())
		{
			
			if (STR_EQUALS (name, "style"))
			{
				mStyleID = 0;
				RWStr::ReadNumber (value, mStyleID);
			}
			else if (STR_EQUALS (name, "dynamic"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mIsDynamic = (lVal != 0);
			}
			else if (STR_EQUALS (name, "attributed"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				mIsAttributed = (lVal != 0);
			}
			else if (STR_EQUALS (name, "empty"))
			{
				if (RWStr::ReadNumber (value, lVal))
				{
					if (lVal >= eEmpty_Draw && lVal <= eEmpty_RemoveRow)
						mDrawIfEmpty = EEmpty (lVal);
				}
				else if (STR_EQUALS (value, "draw"))
					mDrawIfEmpty = eEmpty_Draw;
				else if (STR_EQUALS (value, "remove"))
					mDrawIfEmpty = eEmpty_Remove;
				else if (STR_EQUALS (value, "remove row"))
					mDrawIfEmpty = eEmpty_RemoveRow;
			}
			else if (STR_EQUALS (name, "var"))
			{
				mVarName = value;
			}
			else if (STR_EQUALS (name, "val"))
			{
				mVarValue.SetReal (RWStr::ToDouble (value).value_or (0));
			}
		}
		
		mText = RWTools::ParseIntoText (elem);
	}
	
	//mbs 07052010	support attributed text
	if (	mIsDynamic
		&&	(	mText.IsEmpty()
			 || (mIsAttributed &&  (TEXT_STR (mText, "&lt;%") == STR_NOTFOUND))
			 || (not mIsAttributed && TEXT_STR (mText, "<%") == STR_NOTFOUND)
			 )
		)
		mIsDynamic = false;
	
	
	mStyle = inReport->GetStyle (mStyleID);
	
	if (!mVarName.IsEmpty())
		GetReportWriter()->CreateVariable (mVarName);
	
	return;
}



// ---------------------------------------------------------------------------
// FetchCalcValue													  [public]
// ---------------------------------------------------------------------------

void
ETText::FetchCalcValue (ETReport *inWriter)
{
    if (!mVarName.IsEmpty())
		inWriter->SetVariable (mVarName, &mVarValue);
	return;
}


// ---------------------------------------------------------------------------
// Export															  [public]
// ---------------------------------------------------------------------------

void
ETText::Export (void)
{

	mText = ParseText();
	GetReportWriter()->WriteText ("text", this, mText);
	return;
}

// ---------------------------------------------------------------------------
// GetClass												   [protected]
// ---------------------------------------------------------------------------

RWTextValue
ETText::GetClass()
{
    if (mStyle->GetBaseID() == -1) {
        return mStyle->GetName();
    } else {
        return (GetReportData()->GetStyle(mStyle->GetBaseID()))->GetName();
    }
}



// ---------------------------------------------------------------------------
// ParseText													   [protected]
// ---------------------------------------------------------------------------
// <% report_variable [ ; format ] %>

RWTextValue
ETText::ParseText (void)
{
	RWTextValue	text;
	
	//mbs 30042010	support attributed text
	if (not mText.IsEmpty())
	{
		long	textLen = mText.StrLength();
		long	curPos = 0, delta = 0, endPos;
		
		RWString	result (mText);
		RWString	varName;
		RWString	format;
		while (curPos < textLen && RWTools::ParseTextForVar (mIsAttributed, mText, textLen, curPos, endPos, varName, format))
		{
			RWStringView	varname = varName;
			bool			encode = mIsAttributed;
			if (!varname.empty() && varname[0] == u'+')
			{
				varname.remove_prefix (1);
				encode = false;
			}
			RWString	varText;
			if (!varname.empty())
				varText = GetVariableText (RWString (varname), format);
			varName.clear();
			format.clear();

			result.erase (curPos - delta, endPos - curPos);
			if (encode)		// was tinyxml2 StrPair entity processing, which decodes instead of encoding
				varText = RWTools::EscapeAttributedString (varText);
			result.insert (curPos - delta, varText);
			delta += endPos - curPos - (long) varText.size();
			curPos = endPos;
		}
		text = result;
	}
	
	return text;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

ETVariable*
ETVariable::Create (ETReportData *inReport, RWXmlNode inNode, int inOrder)
{
	ETVariable	*var = new ETVariable (inOrder);
	var->Parse (inReport, inNode);
	
	return var;
}


// ---------------------------------------------------------------------------
// RWVariable								Default Constructor	   [protected]
// ---------------------------------------------------------------------------

ETVariable::ETVariable (int inOrder)
:	ETText (inOrder),
mCalcType (ECalcType_None)
{
}

// ---------------------------------------------------------------------------
// ~ETVariable								Destructor			   [protected]
// ---------------------------------------------------------------------------

ETVariable::~ETVariable (void)
{
	
	return;
}


// ---------------------------------------------------------------------------
// Parse														   [protected]
// ---------------------------------------------------------------------------

void
ETVariable::Parse (ETReportData *inReport, RWXmlNode inNode)
{
	ETText::Parse (inReport, inNode);
	
	RWXmlNode	elem = inNode.Child (u"VariableProps");
	if (!elem)
		elem = inNode;
	//	if (elem)
	//	{
	//		mSource.FromXML (elem.Attr (u"source"));
	//		mFormat.FromXML (elem.Attr (u"format"));
	//		elem = elem.Child (u"Calc");
	//	}
	if (elem)
	{
		long			lVal;
		
		for (const auto &[name, value] : elem.Attributes())
		{
			
			if (STR_EQUALS (name, "source") || STR_EQUALS (name, "src"))
			{
				mSource.FromXML (value);
			}
			else if (STR_EQUALS (name, "format"))
			{
				mFormat.FromXML (value);
			}
			else if (STR_EQUALS (name, "calc"))
			{
				lVal = 0;
				RWStr::ReadNumber (value, lVal);
				if (lVal >= ECalcType_None && lVal < ECalcType_Last)
					mCalcType = ECalcType (lVal);
			}
		}
	}
	
	//	GetVariableText();
	
	if (!mSource.IsEmpty())
		GetReportWriter()->CreateVariable (mSource, mCalcType);
	
	return;
}



// ---------------------------------------------------------------------------
// Export																  [public]
// ---------------------------------------------------------------------------

void
ETVariable::Export ()
{
	GetVariableData();
	GetReportWriter()->WriteText ("variable", this, mText);
	
	return;
}


// ---------------------------------------------------------------------------
// GetVariableData												   [protected]
// ---------------------------------------------------------------------------

void
ETVariable::GetVariableData (void)
{
	if (!mSource.IsEmpty())
	{
		RWTextValue	v;
		RWValue		var;
		
		if (GetReportWriter()->GetVariable (mSource, var, mCalcType))
			v = GetReportWriter()->FormatVariable (var, mFormat);
		mText.Attach (v.Detach());
	}
	
	return;
}


// ---------------------------------------------------------------------------
// SetVariableText												   [protected]
// ---------------------------------------------------------------------------

void
ETVariable::SetVariableText (const CText inConstValue)
{
	mSource.Free();
	mText.Copy (inConstValue);
	
	return;
}
