 #ifndef	_RWMacPageComposer_h_
# define	_RWMacPageComposer_h_

// Macintosh specific drawing/printing
// Printing using PM
// Text drawing using ATSUI

# include	"RWPageComposer.h"
# include	<ApplicationServices/ApplicationServices.h>	// PrintCore (PM...), ATSUI, CoreText

# include	<map>
# define	RWStyleToATSUStyleMap	std::map <RWStyle*, ATSUStyle>

class	RWMacPageComposer
	:	public	RWPageComposer
{
friend class	RWMacPrintText;
friend class	RWPageComposer;

public:
	virtual						~RWMacPageComposer (void);

	virtual		bool			IsQD (void) const = 0;

			PMDestinationType	GetDestinationType (void) const;
			CFURLRef			GetDestinationURL (void) const;

    virtual		void			ParseReport (RWXmlNode inReport) override;
	virtual		void*			FinishReport (size_t &outSize);

	virtual		void			GetPageBounds (const CText inOrientation, const CText inSize, SRect &outRect);
	virtual		bool			GetPageMetrics (float &outPageWidth, float &outPageHeight, SRect &outMargins);
	virtual		bool			GetPageMetrics (SRect &outPageRect, SRect &outPaperRect, SRect &outMargins);
	virtual		void			OpenNewPage (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages);
	virtual		void			ClosePage (void);
	virtual		SRect			GetTruePageRect (void) const;

	virtual		void			DrawTextBox (const CText inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText);
	virtual		double			MeasureText (const CText inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText);

	virtual	const RWPrintContextRef	GetPrintContext (void) = 0;
	virtual		CGContextRef *	GetGContext (void) = 0;
	virtual		void			ReleaseGContext (void) = 0;
	virtual		double			MeasureWord (const CText inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading);
	virtual		void			DrawWord (const CText inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle);

	virtual		bool			GetDPI (float &x, float &y, void* inWindow);

//	virtual		void			StyleChanged (RWStyle *inStyle);

inline	const	PMPageFormat	GetPageFormat (void) const;
				void			SetPageFormat (const PMPageFormat inPageFormat);
				void			SetPageFormat (const SBlob &inPageFormat);
inline	const	PMPrintSettings	GetPrintSettings (void) const;
				void			SetPrintSettings (const PMPrintSettings inPrintSettings);
				void			SetPrintSettings (const SBlob &inPrintSettings);

protected:
								RWMacPageComposer (unsigned long inFlags, CText &inDst, CText &printer);	//mbs 25072011	printer

	virtual		OSStatus		OpenSessionSelf (void) = 0;
	virtual		void			OpenNewPageSelf (void) = 0;
	virtual		void			ClosePageSelf (bool inFinally) = 0;
	virtual		CGContextRef	DrawTextBoxBegin (const RWStyle *inStyle, const SRect &inRect) = 0;
	virtual		void			DrawTextBoxEnd (CGContextRef inRef) = 0;

	virtual		OSStatus		OpenSession (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation);
	virtual		void			CloseSession (bool inRelease);
				ATSUStyle		MapStyle (RWStyle *inStyle);
//	static		void			MeasureText (const UniChar *inText, UniCharCount inSize, ATSUStyle inStyle, SRect &ioRect, bool inWrap, bool inAttributed, RWPrintText **ioPrintText);

	struct	OpenSessionArgs
	{
		OSStatus			outResult;
		RWMacPageComposer	*inThis;
		bool				inDoPageSetup;
		bool				inDoJobSetup;
		unsigned long		inCurPage;
		unsigned long		inNumPages;
		bool				inOrientation;
	};
	struct	OpenNewPageArgs
	{
		OSStatus			outResult;
		RWMacPageComposer	*inThis;
		const SRect			&inRect;
		unsigned long		inCurPage;
		unsigned long		inNumPages;
	};
	static		void			OpenSessionCB (void *inData);
				OSStatus		OpenSessionSafe (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation);
				void			CloseDocument (void);
	static		void			CloseDocumentCB (void *inData);
	static		void			OpenNewPageCB (void *inData);
				void			OpenNewPageSafe (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages);
	static		void			ClosePageCB (void *inData);
				void			ClosePageSafe (void);

//private:
								RWMacPageComposer (const RWMacPageComposer &inOriginal);
			RWMacPageComposer&	operator = (const RWMacPageComposer &inOriginal);

protected:
	bool					mDocIsOpen;
	SRect                   mTruePageRect;
	float					mOffsetX;
	float					mOffsetY;
	PMPrintSession			mPrintSession;
	PMPageFormat			mPageFormat;
	PMPrintSettings         mPrintSettings;
	RWStyleToATSUStyleMap	mStyleMap;
};

inline	const	PMPageFormat	RWMacPageComposer::GetPageFormat (void) const		{ return mPageFormat; }
inline	const	PMPrintSettings	RWMacPageComposer::GetPrintSettings (void) const	{ return mPrintSettings; }

#endif
