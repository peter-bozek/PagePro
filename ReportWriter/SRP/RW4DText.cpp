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

const	RWStringView RWSpan::endSpan = u"</SPAN>";
const	RWStringView RWSpan::startSpan = u"<SPAN STYLE=";
const	RWStringView RWSpan::boldText = u"font-weight:bold";
const	RWStringView RWSpan::italicText = u"font-style:italic";
const	RWStringView RWSpan::underlineText = u"text-decoration:underline";
const	RWStringView RWSpan::fontSize = u"font-size:";
const	RWStringView RWSpan::fontName = u"font-family:";
const	RWStringView RWSpan::fontColor = u"color:";
const	RWStringView RWSpan::breakTag = u"<BR/>";

namespace
{
	const	char16_t	kCR = 0x0d;

	// position of inPart in inText at or after inStart, -1 if missing
	long	Find (const RWString &inText, RWStringView inPart, size_t inStart)
	{
		size_t	pos = inStart <= inText.size() ? inText.find (inPart, inStart) : RWString::npos;
		return pos == RWString::npos ? -1 : long (pos);
	}

	// substring that tolerates positions past the end
	RWString	Substring (const RWString &inText, long inStart, long inLength)
	{
		if (inStart < 0 || size_t (inStart) >= inText.size() || inLength <= 0)
			return RWString();
		return inText.substr (size_t (inStart), size_t (inLength));
	}
}

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
	return ( (mStyle == 0) && (mSize == 0) && mFont.empty() && (mHasColor == false) );
}

void
RWSpan::Join (const RWSpan& inSpan, int mode)
{
	if (mode == mode_add)
	{
		mStyle |= inSpan.mStyle;
		if (mSize == 0)
			mSize = inSpan.mSize;
		if (mFont.empty()) 
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
		if (!inSpan.mFont.empty())
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
		mFont.clear();

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

RWString
RWSpan::toXML (const RWString& inString)
const
{

	RWString	spanString = Substring (inString, mOffset, mLength);

	// line breaks as <BR/>
	for (long where = Find (spanString, RWStringView (&kCR, 1), 0); where >= 0; where = Find (spanString, RWStringView (&kCR, 1), size_t (where)))
		spanString.replace (size_t (where), 1, breakTag);

	if (IsEmpty())
		return spanString;

	RWString	styledString;
	styledString.reserve (mLength + 128);

	styledString.append (startSpan);
	styledString.append (u"\"");

	bool	needSemicolon = false;

	if (!mFont.empty()) {
		styledString.append (fontName);
		styledString.append (u"'");
		styledString.append (mFont);
		styledString.append (u"'");
		needSemicolon = true;
	}

	if (mSize > 0) {
		if (needSemicolon)
			styledString.append (u";");
		styledString.append (fontSize);
		styledString.append (RWStr::Format ("%.2fpt", mSize));
		needSemicolon = true;
	}

	if (mStyle & RWStyle::st_bold) {
		if (needSemicolon)
			styledString.append (u";");
		styledString.append (boldText);
		needSemicolon = true;
	}

	if (mStyle & RWStyle::st_italic) {
		if (needSemicolon)
			styledString.append (u";");
		styledString.append (italicText);
		needSemicolon = true;
	}

	if (mStyle & RWStyle::st_underline) {
		if (needSemicolon)
			styledString.append (u";");
		styledString.append (underlineText);
		needSemicolon = true;
	}

	if (mHasColor) {
		if (needSemicolon)
			styledString.append (u";");
		styledString.append (fontColor);
		styledString.append (RWStr::Format ("#%X", (unsigned int) (mColor & 0x00ffffff)));
		needSemicolon = true;
	}

	styledString.append (u"\">");
	styledString.append (spanString);
	styledString.append (endSpan);

	return styledString;
}

RW4DStyledText::RW4DStyledText (const RWString& inString)
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
	
	RWString	attributes;
	RWSpan*		span;
	long		plainPosition = 0;
	long		last = 0;
	long		where = 0;

	if (mText.empty())
	{
		span = new RWSpan (0, 0);
		mSpanList.push_back (span);
		return;
	}

	// <BR/> as CR
	for (where = Find (mText, RWSpan::breakTag, 0); where >= 0; where = Find (mText, RWSpan::breakTag, size_t (where)))
		mText.replace (size_t (where), RWSpan::breakTag.size(), 1, kCR);

	last = 0;
	while (last < long (mText.size())) {

		where = Find (mText, RWSpan::startSpan, size_t (last));

		if (where < 0) {
			span = new RWSpan (plainPosition, long (mText.size()) - last);
			mSpanList.push_back(span);
			mPlainText.append (mText, size_t (last), RWString::npos);
			plainPosition += (long (mText.size()) - last);
			return;
		}
		else if (where > last)
		{
			span = new RWSpan (plainPosition, where - last);
			mSpanList.push_back(span);
			mPlainText.append (Substring (mText, last, where - last));
			plainPosition += (where - last);
		}

		long end = Find (mText, u">", size_t (where));
		if (end < 0)
			return;

		long endTag = Find (mText, RWSpan::endSpan, size_t (where));
		if (endTag < 0)
			endTag = long (mText.size());

		span = new RWSpan (plainPosition, endTag - end - 1);
		mPlainText.append (Substring (mText, end + 1, endTag - end - 1));
		plainPosition += (endTag - end - 1);

		attributes = Substring (mText, where, end - where);

		if (Find (attributes, RWSpan::boldText, 0) >= 0)
			span->mStyle |= RWStyle::st_bold;
		if (Find (attributes, RWSpan::italicText, 0) >= 0)
			span->mStyle |= RWStyle::st_italic;
		if (Find (attributes, RWSpan::underlineText, 0) >= 0)
			span->mStyle |= RWStyle::st_underline;

		long attPosition = Find (attributes, RWSpan::fontName, 0);
		if (attPosition >= 0) {
			long	nameStart = attPosition + long (RWSpan::fontName.size()) + 1;	// past the quote
			long	endName = Find (attributes, u"'", size_t (nameStart));
			if (endName >= 0)
				span->mFont = Substring (attributes, nameStart, endName - nameStart);
		}

		attPosition = Find (attributes, RWSpan::fontSize, 0);
		if (attPosition >= 0) {
			float	size;
			if (RWStr::ReadNumber (Substring (attributes, attPosition + long (RWSpan::fontSize.size()), 6), size))
				span->mSize = size;
		}

		attPosition = Find (attributes, RWSpan::fontColor, 0);
		if (attPosition >= 0) {
			span->mHasColor = true;
			RWString	hex = u"0x" + Substring (attributes, attPosition + long (RWSpan::fontColor.size()) + 1, 8);	// past the '#'
			std::optional<long long>	color = RWStr::ToInteger (hex);
			if (color)
				span->mColor = ((unsigned long) *color | 0xFF000000);
		}

		mSpanList.push_back(span);
		last = endTag + long (RWSpan::endSpan.size());
	}
	return;
}

RWString
RW4DStyledText::toXMLString (void)
const
{
	RWString	output;
	output.reserve (mText.size() + 128);
	RWSpanList::const_iterator iter;
	for (iter = mSpanList.begin(); iter != mSpanList.end(); iter++)
	{
		const RWSpan *span = *iter;
		output.append (span->toXML (mPlainText));
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
			span->mLength -= (inSpan.mOffset - span->mOffset);	// before moving the offset (was after: length never shrank)
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
