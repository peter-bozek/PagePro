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
ETObject::Parse (ETReportData *inReport, XMLElement *inNode)
{
	mReportData = inReport;
	
	XMLAttribute const	*attrib;
	long		    lVal;
	
	for ( attrib = inNode->FirstAttribute(); attrib; attrib = attrib->Next() )
	{
		const CXMLText	name = attrib->Name();
		const CXMLText	value = attrib->Value();
		
		if (STR_EQUALS (name, "r"))
		{
			mPosition = value.c_str();
		}
		else if (STR_EQUALS (name, "t"))
		{
			mPosition.top = 0;
			sscanf (value.c_str(), "%lg", &mPosition.top);
		}
		else if (STR_EQUALS (name, "l"))
		{
			mPosition.left = 0;
			sscanf (value.c_str(), "%lg", &mPosition.left);
		}
		else if (STR_EQUALS (name, "b"))
		{
			mPosition.bottom = 0;
			sscanf (value.c_str(), "%lg", &mPosition.bottom);
		}
		else if (STR_EQUALS (name, "ri"))
		{
			mPosition.right = 0;
			sscanf (value.c_str(), "%lg", &mPosition.right);
		}
		else if (STR_EQUALS (name, "h") || STR_EQUALS (name, "height"))
		{
			mPosition.bottom = 0;
			sscanf (value.c_str(), "%lg", &mPosition.bottom);
			mPosition.bottom += mPosition.top;
		}
		else if (STR_EQUALS (name, "w") || STR_EQUALS (name, "width"))
		{
			mPosition.right = 0;
			sscanf (value.c_str(), "%lg", &mPosition.right);
			mPosition.right += mPosition.left;
		}
		else if (STR_EQUALS (name, "draw"))
		{
			if (sscanf (value.c_str(), "%li", &lVal) == 1)
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
ETGroup::Create (ETReportData *inReport, XMLElement *inNode, int inOrder)
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
ETGroup::Parse (ETReportData *inReport, XMLElement *inNode)
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
ETText::Create (ETReportData *inReport, XMLElement *inNode, int inOrder)
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
ETText::Parse (ETReportData *inReport, XMLElement *inNode)
{
	ETObject::Parse (inReport, inNode);
	long	mStyleID = 0;
	
    XMLElement	*elem = inNode->FirstChildElement (GetKind() == eObject_Text ? "TextProps" : "VariableProps");
	if (elem == NULL)
		elem = inNode;
	
	if (elem)
	{
		XMLAttribute const	*attrib;
		long			    lVal;
		
		for ( attrib = elem->FirstAttribute(); attrib; attrib = attrib->Next() )
		{
			const CXMLText	name = attrib->Name();
			const CXMLText	value = attrib->Value();
			
			if (STR_EQUALS (name, "style"))
			{
				mStyleID = 0;
				sscanf (value.c_str(), "%li", &mStyleID);
			}
			else if (STR_EQUALS (name, "dynamic"))
			{
				lVal = 0;
				sscanf (value.c_str(), "%li", &lVal);
				mIsDynamic = (lVal != 0);
			}
			else if (STR_EQUALS (name, "attributed"))
			{
				lVal = 0;
				sscanf (value.c_str(), "%li", &lVal);
				mIsAttributed = (lVal != 0);
			}
			else if (STR_EQUALS (name, "empty"))
			{
				if (sscanf (value.c_str(), "%li", &lVal) == 1)
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
				mVarName.Copy (value.c_str());
			}
			else if (STR_EQUALS (name, "val"))
			{
				mVarValue.SetReal (atof (value.c_str()));
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
		
		CText	result (mText);
		CText   varName;
		CText   format;
		while (curPos < textLen && RWTools::ParseTextForVar (mIsAttributed, mText, textLen, curPos, endPos, varName, format))
		{
			const UniChar *	varname = varName.c_str();
			bool		    encode = mIsAttributed;
			if (varname && *varname == '+')
			{
				varname++;
				encode = false;
			}
			RWTextValue	varText;
			if (varname && *varname)
				varText = GetVariableText (varname, format);
			varName.erase();
			format.erase();
			size_t	varLen;
			if (!varText.IsEmpty())
				varLen = varText.StrLength();
			else
				varLen = 0;
			result.erase (curPos - delta, endPos - curPos);
			if (varLen > 0)
			{
				if (encode)
				{
                    StrPair     	encoded;
					CText	        us (varText, varLen);
                    encoded.SetStr(RWTextValue::UTF_16_to_UTF8(us).c_str(), StrPair::NEEDS_ENTITY_PROCESSING);
                    us = RWTextValue::UTF_8_to_UTF16(encoded.GetStr());
					result.insert (curPos - delta, us);
					varLen = us.length();
				}
				else
					result.insert (curPos - delta, varText, varLen);
			}
			delta += endPos - curPos - varLen;
			curPos = endPos;
		}
		text.Attach (result);
	}
	
	return text;
}


#pragma	mark	-

// ---------------------------------------------------------------------------
// Create													 [static] [public]
// ---------------------------------------------------------------------------

ETVariable*
ETVariable::Create (ETReportData *inReport, XMLElement *inNode, int inOrder)
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
ETVariable::Parse (ETReportData *inReport, XMLElement *inNode)
{
	ETText::Parse (inReport, inNode);
	
    XMLElement	*elem = inNode->FirstChildElement ("VariableProps");
	if (elem == NULL)
		elem = inNode;
	//	if (elem)
	//	{
	//		mSource.FromXML (elem->Attribute ("source"));
	//		mFormat.FromXML (elem->Attribute ("format"));
	//		elem = elem->FirstChildElement ("Calc");
	//	}
	if (elem)
	{
		const XMLAttribute	*attrib;
		long			lVal;
		
		for ( attrib = elem->FirstAttribute(); attrib; attrib = attrib->Next() )
		{
			const CXMLText	name = attrib->Name();
			const CXMLText	value = attrib->Value();
			
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
				sscanf (value.c_str(), "%li", &lVal);
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
