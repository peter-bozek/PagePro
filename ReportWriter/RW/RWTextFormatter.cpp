/*
 *  RWTextFormatter.cpp
 *  ReportWriter
 *
 *  Created by Peter Bozek on 31/05/2010.
 *  Copyright 2010 INFORCE Bratislava sro. All rights reserved.
 *
 */

# include	"RWTextFormatter.h"
# include	"RWPageComposer.h"
# include	"PSObjProps.h"

//wchar_t on Mac is UTF32 -> CText does NOT support conversion from/to UTF32!!!

//CText	RWTFPrintText::breakAfter (L" \t.,:;)}\n\r");
//CText	RWTFPrintText::breakBefore (L" \t({");
CText	RWTFPrintText::breakAfterNoWrap (u"\n\r");
CText	RWTFPrintText::breakAfter (u" \t.,;:)>}\n\r");
CText	RWTFPrintText::breakBefore (u" \t(<{");


void
TFWord::AdjustStyle (int inFlags, double inSize, int inSizeSign, int inStyle, SRGBColor &inColor, CText &inFont)
{
	RWValue	v;

	switch (inFlags)
	{
//		case 'b':
		case 'B':
			v.SetBoolean (true);
			mStyle.SetProperty (PSObjPropStyleB, v);
			break;
			
//		case 'i':
		case 'I':
			v.SetBoolean (true);
			mStyle.SetProperty (PSObjPropStyleI, v);
			break;
			
//		case 'u':
		case 'U':
			v.SetBoolean (true);
			mStyle.SetProperty (PSObjPropStyleU, v);
			break;
			
//		case 'c':
		case 'C':	// color
			v.SetInteger (inColor, RWValue::eValue_Integer);	// this limits the color to AARRGGBB...
			mStyle.SetProperty (PSObjPropTextColor, v);
			break;

//		case 'f':
		case 'F':	// font face
			v.SetText (inFont.c_str());
			mStyle.SetProperty (PSObjPropFontName, v);
			break;

//		case 's':
		case 'S':	// font size
		{
			double	fSize = mStyle.GetSize();
			if (inSizeSign == '+')
				inSize = fSize + (fSize / 4) * inSize;
			else if (inSizeSign == '-')
			{
				inSize = fSize - (fSize / 4) * inSize;
				if (inSize < 5)
					inSize = 5;
			}
			if (inSize > 4 && inSize != fSize)
			{
				v.SetReal (inSize);
				mStyle.SetProperty (PSObjPropSize, v);
			}
			break;
		}
			
		default:	// SPAN STYLE attributes
			if (inFlags & 256)
			{
				// inFlags & 1	font
				if (inFlags & 1)
				{
					v.SetText (inFont.c_str());
					mStyle.SetProperty (PSObjPropFontName, v);
				}
				// inFlags & 2	size
				if (inFlags & 2)
				{
					double	fSize = mStyle.GetSize();
					if (inSizeSign == '+')
						inSize = fSize + (fSize / 4) * inSize;
					else if (inSizeSign == '-')
					{
						inSize = fSize - (fSize / 4) * inSize;
						if (inSize < 5)
							inSize = 5;
					}
					if (inSize > 4 && inSize != fSize)
					{
						v.SetReal (inSize);
						mStyle.SetProperty (PSObjPropSize, v);
					}
				}
				
				// inFlags & 4	bold
				if (inFlags & 4)
				{
					v.SetBoolean (inStyle & RWStyle::st_bold);
					mStyle.SetProperty (PSObjPropStyleB, v);
				}

				// inFlags & 8	italic
				if (inFlags & 8)
				{
					v.SetBoolean (inStyle & RWStyle::st_italic);
					mStyle.SetProperty (PSObjPropStyleI, v);
				}

				// inFlags & 16	underline
				if (inFlags & 16)
				{
					v.SetBoolean (inStyle & RWStyle::st_underline);
					mStyle.SetProperty (PSObjPropStyleU, v);
				}

				// inFlags & 32	color
				if (inFlags & 32)
				{
					v.SetInteger (inColor, RWValue::eValue_Integer);	// this limits the color to AARRGGBB...
					mStyle.SetProperty (PSObjPropTextColor, v);
				}
				break;
			}
			// fall through
			
		case 0:	// unknown/unsupported
			break;
	}
}


void
TFWord::GetWordMetrix (RWPageComposer &inComposer, const CText inText)
{
	double	ascent, descent, leading;
	if (mPrintableLength > 0)
		mWidth = inComposer.MeasureWord (inText.substr (mStartChar, mPrintableLength), mPrintableLength, &mStyle, ascent, descent, leading) * mStyle.GetHorizontalScale();
	else
	{
		double width;
		width = inComposer.MeasureWord (inText.substr (0, mLength), mLength, &mStyle, ascent, descent, leading);
		mWidth = 0;
	}
	mBaseline = ascent;
	mHeight = ascent + descent;
}


RWTFPrintText::RWTFPrintText (RWStyle *inStyle)
	:	RWPrintText (inStyle),
		mComposer (0),
		mTextLength (0)
{
}


RWTFPrintText::~RWTFPrintText (void)
{
	{
		TFWordVector::iterator	iter;
		for (iter = mWords.begin(); iter != mWords.end(); iter++)
		{
			TFWord	*obj = *iter;
			if (obj != NULL)
				mComposer->StyleChanged (&obj->mStyle);
		}
	}
	Free();
}


void
RWTFPrintText::Free (void)
{
	mTextLength = 0;
	mLines.clear();
	mWords.clear();
	return;
}


void
RWTFPrintText::Reset (void)
{
	mPrintedHeight = 0;
	mPrintedLines = 0;
}


void			
RWTFPrintText::Init (RWPageComposer &inComposer, const CText inText, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit)
{
	mComposer = &inComposer;
	/*
	 Procedure split passed text to words, producing words as a text runs. 
	 
	 Words are separated either by change of style or ends with one of breakAfter characters or before breakBefore characters.
	 Space and tabs are one - character words.
	 
	 Procedure fills in TFWordVector structure with TFWords with all properties except mStartInLine:
	 
	 If words are separated only by style change, first word gets mKeepWithNext flag set.
	 If word follows TFTab or line separator, it has mFixedPosition flag set.
	 
	 At first approximation, tab width is 3M and line break width is 0. 
	 */
	
	Free();
	
	std::vector<long>	aattributes;
	long	*attributes = NULL;
	if (inAttributed)
	{
		mText.Attach (RWTools::SplitAttributedString (inText, &aattributes));
		attributes = aattributes.data();
	}
	else
		mText = inText;
	
	mTextLength = 0;
	mWidth = ioRect.Width();
	mLineHeight = 0;
	mHeight = 0;
	mPrintedHeight = 0;
	mNumLines = -1;
	const CText	cText = mText;

	if (not mText.IsEmpty())
	{
		mTextLength = mText.size();
		
		/*
		 First split text to words
		 		 */
		int				firstChar = 0;
		size_t			found = 0;
		const char16_t	*as, *begin;
		begin = cText.c_str();

        if (inWrap)
		{
			for (as = begin; *as != 0; as++)
			{
				found = breakBefore.find (*as, 0);
				if (found != CText::npos)
				{
					if (firstChar < (as - begin))
					{
						TFWord	*word = new TFWord (firstChar, as - begin - firstChar, mStyle);
						mWords.push_back (word);
						firstChar = as - begin;
					}
				}
				else if (attributes)
				{
					for (int j = 3; j < attributes[0]; j += 2) 
					{
						if ((attributes[j] == (as - begin)) && (firstChar < (as - begin)))
						{
							TFWord	*word = new TFWord (firstChar, as - begin - firstChar, mStyle);
							word->mKeepWithNext = true;
							mWords.push_back (word);
							firstChar = as - begin;
						}
					}	
				}
				
				found = breakAfter.find (*as, 0);
				if (found != CText::npos)
				{
					if (firstChar < (as - begin + 1))
					{
						TFWord	*word = new TFWord (firstChar, as - begin - firstChar + 1, mStyle);
						if (*as == ' ')
							word->mIsSpace = true;
						else if (*as == '\t') 
							word->mIsTab = true;
						else if ((*as == '\r') && (*(as + 1) == '\n')) // CR LF - should not happen in 4D texts (but can be copied into?)
						{
							word->mNewLine = true;
							word->mPrintableLength -= 1;
							as++;
						}
						else if ((*as == '\r') || (*as == '\n')) 
						{
							word->mNewLine = true;
							word->mPrintableLength -= 1;  // 
						}
						mWords.push_back (word);
						firstChar = as - begin + 1;
					}
				}
			}
		} else
		{
			for (as = begin; *as != 0; as++)
			{
				if (attributes)
				{
					for (int j = 3; j < attributes[0]; j += 2) 
					{
						if ((attributes[j] == (as - begin)) && (firstChar < (as - begin)))
						{
							TFWord	*word = new TFWord (firstChar, as - begin - firstChar, mStyle);
							word->mKeepWithNext = true;
							mWords.push_back (word);
							firstChar = as - begin;
						}
					}	
				}
				
				found = breakAfterNoWrap.find (*as, 0);
				if (found != CText::npos)
				{
					if (firstChar < (as - begin + 1))
					{
						TFWord	*word = new TFWord (firstChar, as - begin - firstChar + 1, mStyle);
						if (*as == ' ')
							word->mIsSpace = true;
						else if (*as == '\t') 
							word->mIsTab = true;
						else if ((*as == '\r') && (*(as + 1) == '\n')) // CR LF - should not happen in 4D texts (but can be copied into?)
						{
							word->mNewLine = true;
							word->mPrintableLength -= 1;
							as++;
						}
						else if ((*as == '\r') || (*as == '\n')) 
						{
							word->mNewLine = true;
							word->mPrintableLength -= 1;  // 
						}
						mWords.push_back (word);
						firstChar = as - begin + 1;
					}
				}
			}
		}
		
		if (firstChar < (as - begin))
		{
			TFWord	*word = new TFWord (firstChar, as - begin - firstChar, mStyle);
			mWords.push_back (word);
		}

		/* 
		 Apply styles to words
		 Each style apply to whole word
		 */
		if (attributes)
		{
			ApplyAttributes (inText, attributes, 2, attributes[0]);
		}
		
		/* 
		 Create lines
		 */
		
		SPoint	origTL (ioRect.TopLeft());
		
		//mbs 11062010	support null width for left/center/right aligned objects
		bool	measureWidth = (ioRect.Width() == 0);
# define	kMeasureWidth		4000

		if (measureWidth)
			ioRect.right = ioRect.left + kMeasureWidth;
		if (mStyle->GetRotation() != 0)
		{
			if (measureWidth)
				ioRect.bottom = ioRect.top + kMeasureWidth;
			RWTools::MakeMatrixFromUserRect (ioRect, inComposer.GetNativeRotation (mStyle->GetRotation()), 0, 0, 0);	//mbs 29062011
		}
		mWidth = ioRect.Width();	// BuildLines needs this!!!

		BuildLines (inComposer, inWrap);
		
		/*
		 Set up line metric
		 */
		
		double	maxWidth = 0;
		double	lastHeight = 0;
		TFLineVector::iterator	lineIterator;
		for (lineIterator = mLines.begin(); lineIterator != mLines.end(); lineIterator++)
		{
			TFLine	*lineObj = *lineIterator;
//			int		word = lineObj->mStartWord;
			int		spaceCount = 0, firstDynamic = lineObj->mStartWord;
			double	width = 0, height = 0, baseline = 0;
			for (int word = lineObj->mStartWord; word < (lineObj->mStartWord + lineObj->mLengthInWords); word++)
			{
				width = width + mWords[word]->mWidth;	
				if ( mWords[word]->mBaseline > baseline)
					baseline = mWords[word]->mBaseline;
				if ( mWords[word]->mHeight > height)
					height = mWords[word]->mHeight;

				if (mWords[word]->mIsSpace)
					spaceCount++;
				else if (mWords[word]->mIsTab)
				{
					spaceCount = 0;
					firstDynamic = word + 1;
				}
				else if (mWords[word]->mNewLine)
				{
					spaceCount = 0;
					firstDynamic = word + 1;
				}
			}
			
			lineObj->mMinLineHeight = height;	

			height *= mStyle->GetLineSpacing();
			// baseline *= mStyle->GetLineSpacing(); no baseline move of first line

			lineObj->mFirstDynamicWord = firstDynamic;		
			lineObj->mSpaces = spaceCount;		
			lineObj->mLineWidth = width;	
			lineObj->mLineHeight = height;	
			lineObj->mBaselinePosition = lastHeight + baseline;
			lastHeight += height;
			if (width > maxWidth)
			{
				maxWidth = width;
			}
			if (mLineHeight == 0)
				mLineHeight = height; // init mLineHeight to height of first line
		}
		
		/* 
		 Set up word position on lines
		 */
		int style = mStyle->GetJustification();

		if (measureWidth)	//mbs 09072010	mWidth is incorrect - use max measured
			mWidth = maxWidth;

		for (lineIterator = mLines.begin(); lineIterator != mLines.end(); lineIterator++ )
		{
			TFLine	*lineObj = *lineIterator;
			double	width = lineObj->mLineWidth;
			double	pos = 0, spaceAdj = 0;
			
			switch (style) {
				case RWStyle::st_default:
				case RWStyle::st_left:
					break;
				case RWStyle::st_justify:
					if(lineIterator == (mLines.end() - 1))
						break;
				case RWStyle::st_fulljustify:
					if(lineObj->mSpaces > 0)
						spaceAdj = (mWidth - width) / lineObj->mSpaces;
					break;
				case RWStyle::st_right:
					pos = mWidth - width;
					break;
				case RWStyle::st_center:
					pos = (mWidth - width) / 2;
					break;
					
				default:
					break;
			}
			
			for (int word = lineObj->mStartWord; word < (lineObj->mStartWord + lineObj->mLengthInWords); word++)
			{
				mWords[word]->mStartInLine = pos;
				if ( (mWords[word]->mIsSpace) && (word >= lineObj->mFirstDynamicWord) )
					mWords[word]->mWidth += spaceAdj;
				pos += mWords[word]->mWidth;
			}
		}
		/*
		 Return values
		*/
		// mWidth = maxWidth;
		mHeight = lastHeight;
		mNumLines = mLines.size();
		
		ioRect.right = ioRect.left + maxWidth;
		ioRect.bottom = ioRect.top + lastHeight;

		if (mStyle->GetRotation() != 0)
		{
			RWTools::MakeUserRectFromText (ioRect, inComposer.GetNativeRotation (mStyle->GetRotation()));	//mbs 29062011
			ioRect.bottom = origTL.v + ioRect.Height();
			ioRect.right = origTL.h + ioRect.Width();
			ioRect.top = origTL.v;
			ioRect.left = origTL.h;
		}
	}
	else
	{
		mWidth = 0;
		mNumLines = 0;
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
	}
}


void
RWTFPrintText::ApplyAttributes (const CText inText, long *attributes, long start, long end)
{
	long		i, j, level = 1;
	for (i = start, j = start; i < end; i = j + 2)	//mbs 13022011	added initialization for j - crash on invalid attributed string "i>i</i>"
	{
		const char16_t	*as = inText.c_str() + attributes[i];
		if (*as == '/')
			continue;
		for (j = i + 2; j < end; j += 2)
		{
			const char16_t	*ae = inText.c_str() + attributes[j];
			if (*ae != '/')
			{
				if (*ae == *as)
					level++;
				continue;
			}
			if (ae[1] == *as)
			{
				if (--level == 0)
					break;
			}
		}
		
		long		startChar = attributes[i+1];
		long		endChar = attributes[1] + 1;	// past the end
		if (j < attributes[0])
			endChar = attributes[j+1];

		double		size = 0;
		int			sizeSign = 0;
		int			style = 0;
		SRGBColor	color;
		CText		fontName;
		int	flags = RWTools::ParseAttributedStringAttribute (as, size, sizeSign, style, color, fontName);
		TFWordVector::iterator	iter;
		for (iter = mWords.begin(); iter != mWords.end(); iter++)
		{
			TFWord	*obj = *iter;
			if (obj != NULL)
			{
				//mbs 10062010	not ALL words...
				if (obj->mStartChar < startChar)
					continue;
				if (obj->mStartChar >= endChar)
					break;
				obj->AdjustStyle (flags, size, sizeSign, style, color, fontName);
			}
		}
		if (j > i + 4)
			ApplyAttributes (inText, attributes, i+2, j-2);
		level = 1;
	}
}


void			
RWTFPrintText::BuildLines (RWPageComposer &inComposer, bool inWrap)
{	
	/*
	 Procedure builds the lines with words based on following rules:
	 - Procedure creates TFLine object
	 - TFSpace(s) at the beginning of line is skipped 
	 - Words are positioned at the line untill the last word does not exceed line width
	 - Last word pointer is moved to previous word, if it has mKeepWithNext flag set to previous word etc.
	 - If no words remained on the line, word(s) is assiged to line
	 - If a word has mFixedPosition flag set, index of next word is stored in mFirstDynamicWord and mSpaces is cleared
	 - if TFSpace is inserted into line the mSpaces is incremented.
	 
	 */
	
	const CText	cText = mText;
	int			firstWord = 0, currentWord = 0;
	double		width = 0;
	
	TFWordVector::iterator	iter;
	for (iter = mWords.begin(); iter != mWords.end(); iter++)
	{
		TFWord	*obj = *iter;
		if (obj != NULL)
			obj->GetWordMetrix (inComposer, cText);
	}
	
	int		fullWordStart  = -1;
	bool	addNext	= false;
	bool	skipSpaces = false;
	for (iter = mWords.begin(); iter != mWords.end(); iter++, currentWord++)
	{
		TFWord	*obj = *iter;
		if (obj != NULL)
		{
			if ((firstWord == currentWord) && obj->mIsSpace && skipSpaces)
			{
				firstWord = firstWord + 1;
				fullWordStart = -1;
				continue;
			}
			
			if(obj->mKeepWithNext) {
				addNext = true;
				if (fullWordStart == -1) {
					fullWordStart = currentWord;
				}
			} else {
				if (addNext) {
					addNext = false;	
				} else {
					fullWordStart = -1;
				}
			}
			
			if (obj->mIsTab)
			{ // tab has internal width 0, text will be aligned to next multiply of .5')
				obj->mWidth = (floor((width + obj->mStyle.GetSize()) / 36) + 1) * 36 - width;
			}
			
			if (obj->mWidth + width > mWidth ) //does not fit in
			{
				if ((firstWord == fullWordStart) && (obj->mKeepWithNext)) //need to keep adding words 
				{
					width += obj->mWidth;
					continue;
				}
				
				if (!inWrap ) {
					if (obj->mNewLine)
					{
						TFLine	*line = new TFLine (firstWord, currentWord - firstWord + 1); //includes current word
						mLines.push_back (line);
						firstWord = currentWord + 1;
						width = 0;
						skipSpaces  = false;
					} else 
					{
						width += obj->mWidth;
						continue;
					}

					continue;
				}
				
				if (firstWord == currentWord)
				{
					TFLine	*line = new TFLine (firstWord, 1);
					mLines.push_back (line);
					firstWord = currentWord + 1;
					width = 0;
					skipSpaces = true;
					continue;
				}
				
				
				{
					int last = currentWord - 1; // this is last word of previous sequence
					
					/* first jump at the start of the word */
					if (firstWord < currentWord)
					{
						TFWord	*objBack = mWords[currentWord - 1];
						if (objBack->mKeepWithNext) 
						{
							if (fullWordStart > firstWord)
							{
								iter -= currentWord - fullWordStart + 1;
								currentWord = fullWordStart - 1; // next iteration starts here
								last = fullWordStart - 1;
							} else {
								// add current word  
								last = currentWord;
							}
							
						}
						else 
						{
							iter--;
							currentWord--;
						}
						
					}
					
					/* then remove trailing spaces */
					
					for ( ; last >= 0 ;  last--)	{
						TFWord	*objBack = mWords[last];
						if(not (objBack->mIsSpace))
							break;
					}
					
					
					TFLine	*line = new TFLine (firstWord, last - firstWord + 1);
					mLines.push_back (line);
					width = 0;
					firstWord = currentWord + 1;
					skipSpaces = true;
				}
			}
			else if (obj->mNewLine)
			{
				TFLine	*line = new TFLine (firstWord, currentWord - firstWord + 1); //includes current word
				mLines.push_back (line);
				firstWord = currentWord + 1;
				width = 0;
				skipSpaces = false;
			}
			else
			{
				width += obj->mWidth;
			}
			
		}
	}
	
	if (firstWord < (int) mWords.size())
	{
		// pB 2010-12 trailing spaces cannot be removed

		TFLine	*line = new TFLine (firstWord, mWords.size() - firstWord);
		mLines.push_back (line);
	}
}

int
RWTFPrintText::Draw (RWPageComposer &inComposer, SRect &ioRect, bool inFit, bool inDoDraw)
{
	SPoint	origTL (ioRect.TopLeft());
	double eps = 0.001;  // rounding error

	if (inDoDraw) 
	{
		StClipToRect	clip (&inComposer, ioRect);
		if (mStyle->GetRotation() != 0) 
		{
			CGAffineTransform	t = RWTools::MakeMatrixFromUserRect (inComposer.GetPrintContext(), ioRect, inComposer.GetNativeRotation (mStyle->GetRotation()), mWidth, mHeight);	//mbs 29062011
			inComposer.ApplyTransform (t);
		}

		/*
		 set up position where we are going to print 
		*/
		
		if (( ioRect.Height() >= mHeight) && (mPrintedLines == 0))
		{
			if (mStyle->GetVerticalJustification() == RWStyle::st_top)
			{
				ioRect.bottom -= ioRect.Height() - mHeight;
			}
			else if (mStyle->GetVerticalJustification() == RWStyle::st_bottom)
			{
				ioRect.top += ioRect.Height() - mHeight;
			}
			else if (mStyle->GetVerticalJustification() == RWStyle::st_center)
			{
				double	delta = (ioRect.Height() - mHeight) / 2;
				ioRect.top += delta;
				ioRect.bottom -= delta;
			}
		}
		else if (mPrintedLines > 0)
		{
			double height = 0;
			for (int line = mPrintedLines ; line != (int) mLines.size(); line++)
			{
				height += mLines[line]->mLineHeight;
				if (height > (ioRect.Height() + eps))
					break;
			}
			if (height < ioRect.Height())
			{
				ioRect.bottom = ioRect.top + height;
			}
		}

		double totalPrinted = 0;
		for (int line = mPrintedLines ; line != (int) mLines.size(); line++)
		{
			if ((totalPrinted + mLines[line]->mMinLineHeight) > (ioRect.Height() + eps))
			{
				if (inComposer.GetDestination () == RWPageComposer::eDestinationScreen) // not inFit) pB 2010-12 never print partial line but display it on screen
				{
					DrawLine (inComposer, ioRect, line);
					mPrintedLines = line + 1;	//mbs 11062010	+1
					totalPrinted += mLines[line]->mLineHeight;
				}
				break;
			}
			DrawLine (inComposer, ioRect, line);
			mPrintedLines = line + 1;	//mbs 11062010	+1
			totalPrinted += mLines[line]->mLineHeight;
		}	
		mPrintedHeight += totalPrinted;
	}
	else
	{		// only measure text we need for printing
		if (mStyle->GetRotation() != 0)
			RWTools::MakeMatrixFromUserRect (inComposer.GetPrintContext(), ioRect, inComposer.GetNativeRotation (mStyle->GetRotation()), mWidth, mHeight);	//mbs 29062011
		ioRect.right = ioRect.left + mWidth;
		if (mPrintedLines == 0)
		{
			ioRect.bottom = ioRect.top + mHeight;
		}
		else
		{
			double height = 0;
			for (int line = mPrintedLines; line != (int) mLines.size(); line++)
			{
				height += mLines[line]->mLineHeight;
			}
			ioRect.bottom = ioRect.top + height;
		}

		//mbs 11062010	set minimum height for one line to be printed
		if (mPrintedLines < mNumLines)	// at least one line
			mLineHeight = mLines[mPrintedLines]->mLineHeight;
		else
			mLineHeight = 0;
	}

	if (mStyle->GetRotation() != 0)
	{
		RWTools::MakeUserRectFromText (ioRect, inComposer.GetNativeRotation (mStyle->GetRotation()));	//mbs 29062011
		ioRect.bottom = origTL.v + ioRect.Height();
		ioRect.right = origTL.h + ioRect.Width();
		ioRect.top = origTL.v;
		ioRect.left = origTL.h;
	}

	return mPrintedLines;
	
}


void
RWTFPrintText::DrawLine (RWPageComposer &inComposer, SRect &ioRect, int inLine)
{
	const CText	cText = mText;
	int			wordCount = mLines[inLine]->mLengthInWords;
	double		baseline = mLines[inLine]->mBaselinePosition + ioRect.top - mPrintedHeight;
	int			word = mLines[inLine]->mStartWord;
	TFWord *	w = mWords[word];
	TFWord *	nextWord;
	int			len = w->mPrintableLength;
	
	word++;
	while  (word < (mLines[inLine]->mStartWord + wordCount))
	{
		// here we shoud remove unprintable characters, currently only CR from the end of the line
		//join words with same style on line
		nextWord = mWords[word];

		if ( (w->mStyle == nextWord->mStyle)
			&& !(nextWord->mIsTab) 
			&& !((nextWord->mStyle.GetJustification() == RWStyle::st_justify)
						|| (nextWord->mStyle.GetJustification() == RWStyle::st_fulljustify)))
		{
			len += nextWord->mPrintableLength;
		} else {
			inComposer.DrawWord (cText.substr (w->mStartChar, len), len, ioRect.left + w->mStartInLine, baseline, &w->mStyle);
			w = nextWord;
			len = w->mPrintableLength;	
		}
		word++;
	}
	inComposer.DrawWord (cText.substr (w->mStartChar, len), len, ioRect.left + w->mStartInLine , baseline, &w->mStyle);
	
	
}
