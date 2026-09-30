/*
 *  RW4DText.cpp
 *  ReportWriter
 *
 *  Created by Peter Bozek on 22/06/2010.
 *  Copyright 2010 INFORCE sro. All rights reserved.
 *
 */

#include "RW4DText.h"
# include	<stdio.h>

const	char* RWSpan::endSpan = "</SPAN>";
const	char* RWSpan::startSpan = "<SPAN STYLE=";
const	char* RWSpan::boldText = "font-weight:bold";
const	char* RWSpan::italicText = "font-style:italic";
const	char* RWSpan::underlineText = "text-decoration:underline";
const	char* RWSpan::fontSize = "font-size:";
const	char* RWSpan::fontName = "font-family:";
const	char* RWSpan::fontColor = "color:";
const	UTF16Char RWSpan::breakTag[] = {'<', 'B', 'R', '/', '>'};

bool		
RWSpan::IsOverlapping (const RWSpan& inSpan)
const
{
	return ( ((inSpan.mOffset >= mOffset) && (inSpan.mOffset < (mOffset + mLength)))
			|| (((inSpan.mOffset + inSpan.mLength) > mOffset) && ((inSpan.mOffset + inSpan.mLength) <= (mOffset + mLength)))
			|| ((inSpan.mOffset <= mOffset) && ((inSpan.mOffset+ inSpan.mLength) >= (mOffset + mLength))) );
}

bool		
RWSpan::IsEmpty (void)
const
{
	return ( (mStyle == 0) && (mSize == 0) && (mFont.StrLength() == 0) && (mHasColor == false) );
}

void
RWSpan::Join (const RWSpan& inSpan, int mode)
{
	if (mode == mode_add)
	{
		mStyle |= inSpan.mStyle;
		if (mSize == 0)
			mSize = inSpan.mSize;
		if (mFont.StrLength() == 0) 
			mFont = inSpan.mFont;
		if ((mHasColor == false) && (inSpan.mHasColor == true))
		{
			mColor = inSpan.mColor;
			mHasColor = true;
		}
	}
	else
	{
		mStyle ^= inSpan.mStyle;
		if (inSpan.mSize > 0)
			mSize = inSpan.mSize;
		if (inSpan.mFont.StrLength() > 0) 
			mFont = inSpan.mFont;
		if (inSpan.mHasColor) {
			mColor = inSpan.mColor;
			mHasColor = true;
		}
	}
}

void
RWSpan::Remove (const RWSpan& inSpan)
{
	mStyle = mStyle & ~inSpan.mStyle;
	if (inSpan.mSize == mSize)
		mSize = 0;
	if (mFont == inSpan.mFont) 
		mFont.Delete(0);

	if ((inSpan.mHasColor == true) && (mHasColor == true) && (inSpan.mColor == mColor)) 
		mHasColor = false;
}

bool
RWSpan::operator == (const RWSpan& inSpan)
const
{
	return ( (mStyle == inSpan.mStyle)
			&& (mSize == inSpan.mSize)
			&& (mFont == inSpan.mFont) 
			&& (mColor == inSpan.mColor) 
			&& (mHasColor == inSpan.mHasColor) 
			
			);
}

CText
RWSpan::toXML (const CText& inString)
const
{

	CText spanString = inString.Substring (mOffset, mLength);

	int where, last = 0;
	UTF16Char cr = 0x0d;
	while ((where = spanString.Find (cr, last, CText::eCF_StrictlyEqual)) > -1) 
	{
		last = where;
		spanString.Delete(last, 1);
		spanString.Insert(last, RWSpan::breakTag, 5);
	}
	
	if (IsEmpty())
		return spanString;
		
	CText	styledString (mLength + 128);
		
	styledString.AppendAscii (startSpan);
	styledString.AppendAscii ("\"");
	
	bool	needSemicolon = false;
	
	if (mFont.StrLength() > 0) {
		styledString.AppendAscii(fontName);
		styledString.AppendAscii("'");
		styledString.Append(mFont);
		styledString.AppendAscii("'");
		needSemicolon = true;
	}


	char buf[32];
	if (mSize > 0) {
		if (needSemicolon) 
			styledString.AppendAscii(";");
		styledString.AppendAscii(fontSize);
		sprintf(buf, "%.2fpt", mSize) ;
		styledString.AppendAscii(buf);
//		styledString.AppendAscii("pt");
		needSemicolon = true;
	}
	
	if (mStyle & RWStyle::st_bold) {
		if (needSemicolon) 
			styledString.AppendAscii(";");
		styledString.AppendAscii(boldText);
		needSemicolon = true;
	}
		
	if (mStyle & RWStyle::st_italic) {
		if (needSemicolon) 
			styledString.AppendAscii(";");
		styledString.AppendAscii(italicText);
		needSemicolon = true;
	}
	
	if (mStyle & RWStyle::st_underline) {
		if (needSemicolon) 
			styledString.AppendAscii(";");
		styledString.AppendAscii(underlineText);
		needSemicolon = true;
	}
	
	if (mHasColor) {
		if (needSemicolon) 
			styledString.AppendAscii(";");
		styledString.AppendAscii(fontColor);
		unsigned int color = mColor & 0x00ffffff;
		sprintf (buf, "#%X", color);
//		styledString.AppendAscii("#");
		styledString.AppendAscii(buf);
		needSemicolon = true;
	}
	
	styledString.AppendAscii("\">");
	styledString.Append (spanString);
	styledString.AppendAscii(endSpan);

	return styledString;
}

RW4DStyledText::RW4DStyledText (const CText& inString)
	:	mText (inString)
{
	Initialize();
}

void
RW4DStyledText::Initialize (void)
{
	/* styled text is in mText
	 procedure moves plain text to mPlainText
	 and build list of non-overlapping styles 
	 in mSpanList 
	 */
	
	CText		attributes;
	RWSpan*		span;
	long		plainPosition = 0;
	long		last = 0;
	long		where = 0;
	
	if (mText.StrLength() == 0)
	{
		span = new RWSpan (0, 0);
		mSpanList.push_back (span);
		return;
	}
	
	UTF16Char cr = 0x0d;
	while ((where = mText.Find (RWSpan::breakTag, 5, last, CText::eCF_StrictlyEqual)) > -1) 
	{
		last = where;
		mText.Delete(last, 5);
		mText.Insert(last, &cr, 1);
	}
	
	last = 0;
	while (last < mText.StrLength()) {

		where = mText.Find (RWSpan::startSpan, last, CText::eCF_StrictlyEqual);

		if (where == CText::_NotFound_) {
			span = new RWSpan (plainPosition, mText.StrLength() - last);
			mSpanList.push_back(span);
			mPlainText.Append (mText.Substring(last, mText.StrLength() - last));
			plainPosition += (mText.StrLength() - last);
			return;
		}
		else
		{ 
			if (where > last)
			{
				span = new RWSpan (plainPosition, where - last);
				mSpanList.push_back(span);
				mPlainText.Append (mText.Substring(last, where - last));
				plainPosition += (where - last);
			}
		}
		
		
		long end = mText.Find ('>', where, CText::eCF_StrictlyEqual);
		if (end == CText::_NotFound_) {
			end = mText.StrLength();
			return;
		}
		
		long endTag = mText.Find (RWSpan::endSpan, where, CText::eCF_StrictlyEqual);
		if (endTag == CText::_NotFound_) {
			endTag = mText.StrLength();
		}
		
		span = new RWSpan (plainPosition, endTag - end - 1);
		mPlainText.Append (mText.Substring (end + 1, endTag - end - 1));
		plainPosition += (endTag - end - 1);

		attributes = mText.Substring (where, end - where);
		
		if (attributes.Find(RWSpan::boldText, 0, CText::eCF_StrictlyEqual) != CText::_NotFound_) 
			span->mStyle |= RWStyle::st_bold;
		if (attributes.Find(RWSpan::italicText, 0, CText::eCF_StrictlyEqual) != CText::_NotFound_) 
			span->mStyle |= RWStyle::st_italic;
		if (attributes.Find(RWSpan::underlineText, 0, CText::eCF_StrictlyEqual) != CText::_NotFound_) 
			span->mStyle |= RWStyle::st_underline;
		
		int attPosition = attributes.Find (RWSpan::fontName, 0, CText::eCF_StrictlyEqual);
		if (attPosition != CText::_NotFound_) {
			int endName = attributes.Find ("'", attPosition + strlen (RWSpan::fontName) + 1, CText::eCF_StrictlyEqual);
			if (endName != CText::_NotFound_) {
				span->mFont = attributes.Substring (attPosition + strlen (RWSpan::fontName) + 1, endName - attPosition - strlen (RWSpan::fontName) - 1);
			}
		}

		attPosition = attributes.Find(RWSpan::fontSize, 0, CText::eCF_StrictlyEqual);
		if (attPosition != CText::_NotFound_) {
			// span->mSize = mText.ToNumber(attPosition + strlen (RWSpan::fontSize), 6);
			char buf[32];
			attributes.Substring(attPosition + strlen (RWSpan::fontSize), 6).ToAscii(buf, 6);
			float size;
			if (sscanf(buf, "%f", &size))
				span->mSize = size;
		}

		attPosition = attributes.Find(RWSpan::fontColor, 0, CText::eCF_StrictlyEqual);
		if (attPosition != CText::_NotFound_) {
			span->mHasColor = true;
			char buf [32];
			attributes.Substring(attPosition + strlen(RWSpan::fontColor) + 1, 9).ToAscii(buf, 9);
			unsigned int color;
			if (sscanf(buf, "%X", &color))
				span->mColor = (color | 0xFF000000);
		}
		
		mSpanList.push_back(span);
		last = endTag + strlen (RWSpan::endSpan);
	}
	return;
}

CText
RW4DStyledText::toXMLString (void)
const
{
	CText output (mText.StrLength() + 128);
	RWSpanList::const_iterator iter;
	for (iter = mSpanList.begin(); iter != mSpanList.end(); iter++)
	{
		const RWSpan *span = *iter;
		output.Append (span->toXML (mPlainText));
	}
	return output;
}

void
RW4DStyledText::AddSpan (const RWSpan& inSpan, int mode)
{
	unsigned int	index; 
	for (index = 0; index < mSpanList.size(); index++)
	{
		RWSpan *span = mSpanList[index];
		if (not span->IsOverlapping (inSpan))
			continue;
		
		if ((inSpan.mOffset <= span->mOffset) && ((inSpan.mOffset+ inSpan.mLength) >= (span->mOffset + span->mLength)))
		{
			span->Join (inSpan, mode);
			continue;
		}
		
		if (inSpan.mOffset > span->mOffset)
		{
			RWSpan* newSpan = new RWSpan (*span);
			newSpan->mLength = inSpan.mOffset - span->mOffset;
			span->mLength -= (inSpan.mOffset - span->mOffset);
			span->mOffset = inSpan.mOffset;
			RWSpanList::iterator iter = mSpanList.begin() + index;
			mSpanList.insert (iter, newSpan);
			continue;
		}
		
		if ((inSpan.mOffset + inSpan.mLength) < (span->mOffset + span->mLength))
		{
			RWSpan* newSpan = new RWSpan (*span);

			int diff = ((span->mOffset + span->mLength) - (inSpan.mOffset + inSpan.mLength));
			span->mLength -= diff;
			span->Join (inSpan, mode);

			newSpan->mOffset = span->mOffset + span->mLength;
			newSpan->mLength = diff;
			if (index < (mSpanList.size() - 1))
			{
				RWSpanList::iterator iter = mSpanList.begin() + index + 1;
				mSpanList.insert (iter, newSpan);
			}
			else 
				mSpanList.push_back (newSpan);

			return;
		}
		
	}
}

void
RW4DStyledText::RemoveSpan (const RWSpan& inSpan)
{
	unsigned int	index; 
	for (index = 0; index < mSpanList.size(); index++)
	{
		RWSpan *span = mSpanList[index];
		if (not span->IsOverlapping (inSpan))
			continue;
		
		if ((inSpan.mOffset <= span->mOffset) && ((inSpan.mOffset+ inSpan.mLength) >= (span->mOffset + span->mLength)))
		{
			span->Remove (inSpan);
			continue;
		}
		
		if (inSpan.mOffset > span->mOffset)
		{
			RWSpan* newSpan = new RWSpan (*span);
			newSpan->mLength = inSpan.mOffset - span->mOffset;
			span->mOffset = inSpan.mOffset;
			span->mLength -= (inSpan.mOffset - span->mOffset);
			RWSpanList::iterator iter = mSpanList.begin() + index;
			mSpanList.insert (iter, newSpan);
			continue;
		}
		
		if ((inSpan.mOffset + inSpan.mLength) < (span->mOffset + span->mLength))
		{
			RWSpan* newSpan = new RWSpan (*span);

			int diff = ((span->mOffset + span->mLength) - (inSpan.mOffset + inSpan.mLength));
			span->mLength -= diff;
			span->Remove (inSpan);
			
			newSpan->mOffset = span->mOffset + span->mLength;
			newSpan->mLength = diff;
			if (index < (mSpanList.size() - 1))
			{
				RWSpanList::iterator iter = mSpanList.begin() + index + 1;
				mSpanList.insert (iter, newSpan);
				
			} else 
				mSpanList.push_back (newSpan);
			
			return;
		}
		
	}
}

void 
RW4DStyledText::Scale (float inScale, int inMode)
{
	if (inScale > 0.01)
	{
		RWSpanList::iterator iter;
		float	newSize;
		for (iter = mSpanList.begin(); iter != mSpanList.end(); iter++)
		{
			RWSpan *span = *iter;
			newSize = ((float) floor (span->mSize * inScale * pow( 10, inMode) + 0.5)) / ((float) pow (10, inMode));
			span->mSize = newSize;
		}
	}
}

void 
RW4DStyledText::Clean (void)
{
	for (int index = mSpanList.size() - 2; index >= 0; index--)
	{
		RWSpan *span = mSpanList[index];
		RWSpan *spanNext = mSpanList[index + 1];
		if (*span == *spanNext)
		{
			span->mLength += spanNext->mLength;
			RWSpanList::iterator iter = mSpanList.begin() + index + 1;
			mSpanList.erase (iter);

		}
	}
}
