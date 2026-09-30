/*
 *  RWTextFormatter.h
 *  ReportWriter
 *
 *  Created by Peter Bozek on 31/05/2010.
 *  Copyright 2010-2024 INFORCE Bratislava sro. All rights reserved.
 *
 */

#include "RWStyle.h"
#include "RWPageComposer.h"

class TFWord
{

public:
	inline			TFWord( int start, int length, RWStyle* style);

	void			GetWordMetrix (RWPageComposer &inComposer, const CText inText);
	void			AdjustStyle (int inFlags, double inSize, int inSizeSign, int inStyle, SRGBColor &inColor, CText &inFont); 
	
	// Getters and Setters
	
public:
	int			mStartChar;
	int			mLength;
	int			mPrintableLength; // pB 2011 length without unprintable characters
	double		mWidth;
	double		mHeight;
	double		mBaseline;
	double		mStartInLine;
	bool		mKeepWithNext;
	bool		mFixedPosition;
	bool		mIsSpace;
	bool		mIsTab;
	bool		mNewLine;
	RWStyle		mStyle;
};  


TFWord::TFWord (int start, int length, RWStyle* style) 
	:	mStartChar (start),
		mLength (length),
		mPrintableLength (length),
		mKeepWithNext (false),
		mFixedPosition (false),
		mIsSpace (false),
		mIsTab (false),
		mNewLine (false),
		mStyle (NULL, RWXmlNode())
{
	mStyle.CloneFrom (style);
}


typedef		RWArray <TFWord*>	TFWordVector;

class TFLine
{
	
public:
	TFLine (int start, int length) : mStartWord(start), mLengthInWords (length) {};

	// Getters and Setters

public:
	int			mStartWord;
	int			mLengthInWords;
	int			mFirstDynamicWord;
	int			mSpaces;
	double		mLineHeight;
	double		mLineWidth;
	double		mBaselinePosition;
	double		mMinLineHeight;
};  

typedef		RWArray <TFLine*>	TFLineVector;


class RWTFPrintText : public  RWPrintText
{
public :
	
								RWTFPrintText (RWStyle *inStyle);
	virtual						~RWTFPrintText (void);
	virtual		void			Reset (void);
	
				void			Init (RWPageComposer &inComposer, const CText inText, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit);
//				void			GetBounds (RWPageComposer &inComposer, SRect &ioRect, bool inFit);
				int				Draw (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inDoDraw);
	
protected:
				void			Free();
				void			BuildLines (RWPageComposer &inComposer, bool inWrap);
				void			DrawLine (RWPageComposer &inComposer, SRect &ioRect, int inLine);
				void			ApplyAttributes (const CText inText, long *attributes, long start, long end);
	
private:
	// defensive programming - not implemented
								RWTFPrintText (const RWTFPrintText &inOriginal);
				RWTFPrintText&	operator = (const RWTFPrintText &inOriginal);
	
	
protected :
	static CText	breakAfterNoWrap;
	static CText	breakAfter;
	static CText	breakBefore;

	RWPageComposer	*mComposer;
	TFWordVector	mWords;
	TFLineVector	mLines;
		
	long			mTextLength;
};
