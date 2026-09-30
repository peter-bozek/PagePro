/*
 *  RW4DText.h
 *  ReportWriter
 *
 *  Created by Peter Bozek on 22/06/2010.
 *  Copyright 2010 INFORCE Bratislava sro. All rights reserved.
 *
 */

#include <deque>
#include "RWStyle.h"

class	RWSpan
{
public: 
	enum add_mode {
		mode_add,
		mode_xor
	};
	
	// 4D v12 styled text markup
	static const	RWStringView endSpan;
	static const	RWStringView startSpan;
	static const	RWStringView boldText;
	static const	RWStringView italicText;
	static const	RWStringView underlineText;
	static const	RWStringView fontSize;
	static const	RWStringView fontName;
	static const	RWStringView fontColor;
	static const	RWStringView breakTag;


	int				mOffset;
	int				mLength;
	int				mStyle;
	float			mSize;
	CText			mFont;
	unsigned long	mColor;
	bool			mHasColor;
	bool			mIsSubscript;
	bool			mIsSuperScript;
	
	RWSpan (int inOffset, int inLength ) : 
		mOffset (inOffset), mLength (inLength), mStyle (0), mSize (0), mColor (0), mHasColor (false) {};
	RWSpan (int inOffset, int inLength, int inStyle, float inSize, CText inFont, unsigned long inColor, bool inHasColor ) : 
			mOffset (inOffset), mLength (inLength), mStyle (inStyle), mSize (inSize), mFont (inFont), mColor (inColor), mHasColor (inHasColor) {};
	RWSpan (const RWSpan& inSpan) :
			mOffset (inSpan.mOffset), mLength (inSpan.mLength), mStyle (inSpan.mStyle), mSize (inSpan.mSize), mFont (inSpan.mFont), mColor (inSpan.mColor), mHasColor (inSpan.mHasColor) {};
	
	void		Join (const RWSpan& inSpan, int mode);
	void		Remove (const RWSpan& inSpan);

	bool		IsOverlapping (const RWSpan& inSpan) const;
	CText		toXML(const CText& inString) const;
	bool		operator == (const RWSpan& inSpan) const;
	bool		IsEmpty (void) const;
};
					 	 
typedef		RWArray <RWSpan*>	 RWSpanList;

class RW4DStyledText 
{

	public:

	CText		mText;
	CText		mPlainText;
	RWSpanList	mSpanList;
	
	RW4DStyledText (const CText& inString);
	void		Initialize (void);
	CText		toXMLString(void) const;
	void		AddSpan (const RWSpan& inSpan, int mode);
	void		RemoveSpan (const RWSpan& inSpan);
	void		Scale (float inScale, int inMode);
	void		Clean (void);

	static void UnitTest (void);
	
};
