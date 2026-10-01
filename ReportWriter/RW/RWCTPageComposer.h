/*
 *  RWCTPageComposer.h
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 17.01.2010.
 *  Copyright 2010 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */

// requires MacOS 10.5

#ifndef	_RWCTPageComposer_h_
# define	_RWCTPageComposer_h_

# include	"RWMacCGPageComposer.h"


# include	<map>
# define	RWStyleToCFDictionaryMap	std::map <RWStyle, CFMutableDictionaryRef, RWStyleLess>

# include "RWStyle.h"
struct RWStyleLess
{
	bool operator()(const RWStyle& x, const RWStyle& y) const { return x.less (y); }
};

class	RWCTPageComposer
	:	public	RWMacCGPageComposer
{
friend class	RWCTPrintText;

public:
								RWCTPageComposer (unsigned long inFlags, RWString &inDst, RWString &inPrinter);	//mbs 25072011	printer
	virtual						~RWCTPageComposer (void);

	virtual		void			StyleChanged (RWStyle *inStyle);
	virtual		RWNativePageComposer*	CreateComposerForPrinting (void) const;

	virtual		void			DrawTextBox (const RWString inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText);
	virtual		double			MeasureText (const RWString inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText);
	virtual		double			MeasureWord (const RWString inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading);
	virtual		void			DrawWord (const RWString inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle);
	virtual		bool			IsUnicodeFont (RWStyle *inStyle);

	static		CTFontRef		CreateFont (const RWString inName, long inNameLength, float inSize, int style);

protected:
				CFDictionaryRef	MapStyle (RWStyle *inStyle);

//private:
// default is ok
//								RWCTPageComposer (const RWCTPageComposer &inOriginal);
//			RWCTPageComposer&	operator = (const RWCTPageComposer &inOriginal);

protected:
	RWStyleToCFDictionaryMap	mStyleMap;
};

#endif
