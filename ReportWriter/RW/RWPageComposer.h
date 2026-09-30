#ifndef	_RWPageComposer_h_
# define	_RWPageComposer_h_

# include	"RWBaseTypes.h"

// forward declarations
class	RWStyle;
class	RWPageComposer;
class	RWNativePrintText;

#if	WINVER
class	    RWWinPageComposer;
typedef		RWWinPageComposer	RWNativePageComposer;
#else
# if	1
class	    RWCTPageComposer;
typedef		RWCTPageComposer	RWNativePageComposer;
# else
class	    RWMacPageComposer;
typedef		RWMacPageComposer	RWNativePageComposer;
# endif
#endif

typedef	    RWMap<long, RWPageComposer*>		PSesList;


// Composer specific object for picture rendering
struct	RWPictData
{
public:
	virtual						~RWPictData (void);

	inline		float			GetWidth (void) const;
	inline		float			GetHeight (void) const;

protected:
	inline						RWPictData (void);

private:
			// defensive programming - not implemented
								RWPictData (const RWPictData &inOriginal);
				RWPictData&		operator = (const RWPictData &inOriginal);

public:
	float					fWidth;
	float					fHeight;
};


// Composer specific object for text rendering
class	RWPrintText
{
public:
	virtual						~RWPrintText (void);

//	virtual		void			Init (RWPageComposer &inComposer, const CText inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit) = 0;
	virtual		void			Reset (void) = 0;

	inline		CText   		GetText (void) const;
	inline		float			GetWidth (void) const;
	inline		float			GetHeight (void) const;
	inline		float			GetLineHeight (void) const;
	inline		float			GetPrintedHeight (void) const;
	inline		int				GetLineCount (void) const;			// unused
	inline		int				GetPrintedLineCount (void) const;	// unused
	inline		bool			IsPrinted (void) const;

protected:
								RWPrintText (RWStyle *inStyle);
								RWPrintText (const CText inText, RWStyle *inStyle, bool inAttributed);

private:
			// defensive programming - not implemented
								RWPrintText (const RWPrintText &inOriginal);
				RWPrintText&	operator = (const RWPrintText &inOriginal);

protected:
	RWStyle				*mStyle;
	RWTextValue			mText;
	float				mWidth;
	float				mLineHeight;
	float				mHeight;
	float				mPrintedHeight;
	int					mNumLines;
	int					mPrintedLines;
};


typedef	struct	RWClipInfo*		RWClipInfoRef;		// for Clipping - nothing / GraphicsContainer
typedef	struct	RWContextInfo*	RWContextInfoRef;	// for CG state - nothing / GraphicsContainer

class	StClipToRect
{
public:
	inline	StClipToRect (RWPageComposer *inComposer, const SRect &inRect);
	inline	StClipToRect (RWPageComposer *inComposer, const SRect &inRect, const SRect &inExcludeRect);
	inline	~StClipToRect (void);
protected:
	RWPageComposer *mComposer;
	RWClipInfoRef	mClip;
};


class	RWPageComposer
{
public:
	enum	EMacPageComposerFlags
	{
		eDestinationPrinter	= 0,
		eDestinationFile	= 1,
		eDestinationFax		= 2,
		eDestinationPreview	= 3,
		eDestinationPDF		= 4,		//mbs 11082010
		eDestinationScreen	= 8,
		eDestinationMask	= 0x0F,
		eValidatePageSetup	= 0x00000010,
		eUseDefPageSetup	= 0x00000020,
		eUse4DPageSetup		= 0x00000040,
		eValidateJobSetup	= 0x00000100,
		eUseDefJobSetup		= 0x00000200,
		eUse4DJobSetup		= 0x00000400,
		eAskPageSetup		= 0x00001000,
		eAskJobSetup		= 0x00002000,
		eNoProgress			= 0x00004000,
		eNoDefPrinter		= 0x00008000,	//mbs 29062011	don't use default printer on Windows
		ePDFDontEmbedFonts	= 0x00010000,	//mbs 18072011
		eUserFlagsMask		= 0x0001F770,
		eRanJobSetup		= 0x00020000,
		eResetMargins		= 0x00040000,
		eDefault			= eDestinationPrinter | eAskPageSetup | eAskJobSetup
	};

	virtual						~RWPageComposer (void);

				long			GetDestination (void) const			{ return (mFlags & eDestinationMask);	}
				bool			AskPageSetup (void) const			{ return (mFlags & eAskPageSetup) != 0;	}
				bool			AskJobSetup (void) const			{ return (mFlags & eAskJobSetup) != 0;	}
				bool			ShowProgress (void) const			{ return (mFlags & eNoProgress) == 0;	}
		unsigned long			GetFlags (void) const				{ return mFlags;	}
				void			SetFlags (unsigned long inFlags)	{ mFlags = inFlags; }
				void			IncreaseBatchLevel (void);
				void			DecreaseBatchLevel (void);
	
    virtual		void			ParseReport (const XMLElement *inReport);
	virtual		void		*	FinishReport (size_t &outSize) = 0;

inline			float			GetReportPageWidth (void) const;
inline			float			GetReportPageHeight (void) const;
inline			bool			GetReportMargins (SRect &outMargins) const;
	virtual		void			GetPageBounds (const CText inOrientation, const CText inSize, SRect &outRect);
	virtual		bool			GetPageMetrics (float &outPageWidth, float &outPageHeight, SRect &outMargins);
	virtual		bool			GetPageMetrics (SRect &outPageRect, SRect &outPaperRect, SRect &outMargins);
	virtual		bool			GetPaperMetrics (float &outPaperWidth, float &outPaperHeight, SRect &outMargins);

				long			PrintSettings (float &outPaperWidth, float &outPaperHeight, SRect &outMargins);
	virtual		void			OpenNewPage (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages) = 0;
	virtual		void			ClosePage (void) = 0;
				unsigned long	GetFirstPage (void) const	{ return mFirstPage; }
				unsigned long	GetLastPage (void) const	{ return mLastPage; }

	inline		SRect		GetPageRect (void) const	{ return mPageRect; }
	virtual		void			SetPageRect (const SRect &inRect);
	virtual		SRect		GetTruePageRect (void) const	{ return mPageRect; }
				
				void			DrawLine (const SRect &inRect, float inThickness, SRGBColor inLineColor, UInt8 inFlags, float inLineLen = 0, float inSpaceLen = 0);
				void			DrawRect (const SRect &inParent, const SRect &inRect, float inThickness, SRGBColor inFrameColor,
										  bool inFill, SRGBColor inFillColor, long inRows, long inCols, UInt8 inFlags);
	virtual		void			DrawLine (float top, float left, float bottom, float right, float inThickness, SRGBColor inLineColor, float inLineLen = 0, float inSpaceLen = 0) = 0;
	virtual		void			DrawRect (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
										  bool inFill, SRGBColor inFillColor, float inLineLen = 0, float inSpaceLen = 0) = 0;
	virtual		void			DrawOval (const SRect &inRect, float inThickness, bool inFrame, SRGBColor inFrameColor,
											bool inFill, SRGBColor inFillColor, float inLineLen = 0, float inSpaceLen = 0) = 0;

	virtual		void			GetPictBounds (SRect &ioRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, bool inGrow) = 0;
	virtual		void			DrawPict (SRect &inRect, const RWPicture &inPicture, EPictFormat inSizing, RWPictData **cd, double inRotation, float alfa) = 0;
//	virtual		void			FreePict (RWPictData **cd) = 0;

	virtual		void			DrawTextBox (const CText inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText);
	virtual		double			MeasureText (const CText inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText);
	virtual		double			GetNativeRotation (double inRotation)	{ return inRotation; }	//mbs 29062011

	virtual		bool			GetDPI (float &x, float &y, void* inWindow);

								// screen drawing
	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect);
	virtual		RWClipInfoRef	ClipToRect (const SRect &inRect, const SRect &inExcludeRect);
	virtual		void			RestoreClip (RWClipInfoRef &ioClipInfo);
	virtual		void			StyleChanged (RWStyle *inStyle);
	virtual		void	*		GetContext (void) const;
	virtual		void			SetContext (void *inContext);
	virtual	RWNativePageComposer*	CreateComposerForPrinting (void) const { return NULL; }

	static	RWNativePageComposer*	CreateScreenComposer (void);
	static	RWPageComposer*         CreatePrinterComposer (unsigned long inFlags, CText &inDst, CText &inPrinter);	//mbs 25072011	printer
//				void				SetJobName (const UTF8Char *inName);	//mbs 08102010	unused
				void				SetJobName (const CText inName);

	static      RWPageComposer*     GetSessionObject (long inSession);
	static		long				OpenSession (RWPageComposer* &outSession, unsigned long inFlags, CText &inTemplate, CText &inDst, CText &inJobName, CText &inPrinter );	//mbs 25072011	printer
	static		void				CloseSession (RWPageComposer *inSession);

	virtual		void				SaveContext (RWContextInfoRef &outContext)		{}
	virtual		void				RestoreContext (RWContextInfoRef &ioContext)	{}
	virtual	const RWPrintContextRef	GetPrintContext (void)							{ return 0; }
	virtual		void				ApplyTransform (CGAffineTransform &inMatrix)	{}
    virtual		void                InitPagePosition (void);

	virtual		double              MeasureWord (const CText inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading) = 0;
	virtual		void				DrawWord (const CText inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle) = 0;
    virtual     long                GetInternalID () {return mInternalID;}
protected:
	virtual		OSStatus			OpenSession (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation) = 0;
	virtual		void				CloseSession (bool inRelease) = 0;

                                    RWPageComposer (unsigned long inFlags, CText &inDst, CText &inPrinter);	//mbs 25072011	printer
                                    RWPageComposer (unsigned long inFlags);

//private:
								RWPageComposer (const RWPageComposer &inOriginal);
		RWPageComposer&			operator = (const RWPageComposer &inOriginal);

protected:
	static	PSesList		sSessions;
	static	bool			sUseTF;

	unsigned long			mFlags;
//	const char		*		mDestination;	//mbs 11082010
	CText					mDestination;
	CText					mPrinterName;	//mbs 25072011
	RWTextValue				mJobName;
	RWTextValue				mPageSize;
	RWTextValue				mPageOrientation;
	float					mPageWidth;
	float					mPageHeight;
	SRect					mReportPageMargins;
	SRect					mPaperRect;
	SRect					mPageRect;
	bool					mUseReportMargins;
	bool					mUsePhysical;
	UInt32			        mFirstPage;
	UInt32			        mLastPage;
	long					mBatchLevel;
	bool					mPageIsOpen;

    // v1.4
    bool                    mReportRotation;
    bool                    mReportMirror;
    
    // 1.5
    long                    mInternalID;
    static long				mSessionCounter;

};


inline						RWPictData::RWPictData (void)	: 		fWidth (0), fHeight (0)	{}
inline	float					RWPictData::GetWidth (void) const								{ return fWidth; }
inline	float					RWPictData::GetHeight (void) const								{ return fHeight; }

inline	CText			        RWPrintText::GetText (void) const								{ return mText; }
inline	float					RWPrintText::GetWidth (void) const								{ return mWidth; }
inline	float					RWPrintText::GetHeight (void) const								{ return mHeight; }
inline	float					RWPrintText::GetLineHeight (void) const							{ return mLineHeight; }
inline	float					RWPrintText::GetPrintedHeight (void) const						{ return mPrintedHeight; }
inline	int					    RWPrintText::GetLineCount (void) const							{ return mNumLines; }
inline	int					    RWPrintText::GetPrintedLineCount (void) const					{ return mPrintedLines; }
inline	bool					RWPrintText::IsPrinted (void) const								{ return mNumLines <= mPrintedLines; }

inline	float					RWPageComposer::GetReportPageWidth (void) const					{ return mPageWidth; }
inline	float					RWPageComposer::GetReportPageHeight (void) const				{ return mPageHeight; }
inline	bool					RWPageComposer::GetReportMargins (SRect &outMargins) const		{ outMargins = mReportPageMargins; return mUseReportMargins; }
inline	void					RWPageComposer::SetPageRect (const SRect &inRect)				{ mPageRect = inRect; }

inline	RWClipInfoRef			RWPageComposer::ClipToRect (const SRect &inRect)				{ return 0; }
inline	RWClipInfoRef			RWPageComposer::ClipToRect (const SRect &inRect, const SRect &inExcludeRect)				{ return 0; }
inline	void					RWPageComposer::RestoreClip (RWClipInfoRef &ioClipInfo)			{}
inline	void					RWPageComposer::StyleChanged (RWStyle *inStyle)					{}
inline	void	*				RWPageComposer::GetContext (void) const							{ return 0; }
inline	void					RWPageComposer::SetContext (void *inContext)					{}

inline	StClipToRect::StClipToRect (RWPageComposer *inComposer, const SRect &inRect) : mComposer (inComposer)	{ mClip = mComposer->ClipToRect (inRect); }
inline	StClipToRect::StClipToRect (RWPageComposer *inComposer, const SRect &inRect, const SRect &inExcludeRect) : mComposer (inComposer)	{ mClip = mComposer->ClipToRect (inRect, inExcludeRect); }
inline	StClipToRect::~StClipToRect (void)		{ mComposer->RestoreClip (mClip); }

#endif
