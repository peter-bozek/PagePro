# include	"RWMacPageComposer.h"
# include	"RWStringCF.h"
# include	"RWStyle.h"
# include	<memory>

#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

// #include "DrawUnicodeString.h"
// #include "TextUtilities.h"

#if	_4D_Package_
# include "4DPluginAPI.h"
// using namespace	FourDAPI;
#endif

# define	STRING_ENCODING	kCFStringEncodingUTF8		// kTextEncodingUnicodeDefault + kUnicodeUTF8Format


#define	ROUND_UP(x)	(x)


#ifndef	verify_noerr
   #define verify_noerr(errorCode)                                            \
      do                                                                      \
      {                                                                       \
          if ( 0 != (errorCode) )                                             \
          {                                                                   \
          }                                                                   \
      } while ( 0 )
#endif





// ---------------------------------------------------------------------------
// RWMacPageComposer						Constructor				  [public]
// ---------------------------------------------------------------------------

RWMacPageComposer::RWMacPageComposer (unsigned long inFlags, RWString &inDst, RWString &inPrinter)	//mbs 25072011	printer
	:	RWPageComposer (inFlags, inDst, inPrinter),	//mbs 25072011	printer
		mDocIsOpen (false),
		mTruePageRect (0, 0, 0, 0),
		mOffsetX (0),
		mOffsetY (0),
		mPrintSession (0),
		mPageFormat (0),
		mPrintSettings (0)
{
	if ((mFlags & eDestinationMask) == eDestinationPDF)
		mFlags = (mFlags & ~eDestinationMask) | eDestinationFile;
	return;
}


// ---------------------------------------------------------------------------
// RWMacPageComposer						Constructor			   [protected]
// ---------------------------------------------------------------------------

RWMacPageComposer::RWMacPageComposer (const RWMacPageComposer &inOriginal)
	:	RWPageComposer (inOriginal),
		mDocIsOpen (false),
		mTruePageRect (0, 0, 0, 0),
		mOffsetX (0),
		mOffsetY (0),
		mPrintSession (0),
		mPageFormat (0),
		mPrintSettings (0)
{
	*this = inOriginal;
}


// ---------------------------------------------------------------------------
// operator =													   [protected]
// ---------------------------------------------------------------------------

RWMacPageComposer&
RWMacPageComposer::operator = (const RWMacPageComposer &inOriginal)
{
	CloseSession (true);
	StyleChanged (NULL);
	RWPageComposer::operator = (inOriginal);

//	mPageIsOpen = false;
//	mDocIsOpen = false;
	mTruePageRect = inOriginal.mTruePageRect;
	mOffsetX = inOriginal.mOffsetX;
	mOffsetY = inOriginal.mOffsetY;
	if (inOriginal.mPageFormat != NULL)
	{
		PMCreatePageFormat (&mPageFormat);
		if (mPageFormat != NULL)
			PMCopyPageFormat (inOriginal.mPageFormat, mPageFormat);
	}
	if (inOriginal.mPrintSettings != NULL)
	{
		PMCreatePrintSettings (&mPrintSettings);
		if (mPrintSettings != NULL)
			PMCopyPrintSettings (inOriginal.mPrintSettings, mPrintSettings);
	}
	return *this;
}



// ---------------------------------------------------------------------------
// ~RWMacPageComposer						Destructor				  [public]
// ---------------------------------------------------------------------------

RWMacPageComposer::~RWMacPageComposer (void)
{
	CloseSession (true);
	StyleChanged (NULL);

	return;
}



// ---------------------------------------------------------------------------
// GetDestinationType												  [public]
// ---------------------------------------------------------------------------

PMDestinationType
RWMacPageComposer::GetDestinationType (void)
const
{
	PMDestinationType	dest = kPMDestinationPrinter;

	switch (GetDestination())
	{
		case eDestinationPrinter:	dest = kPMDestinationPrinter; break;
		case eDestinationFile:		dest = kPMDestinationFile; break;
		case eDestinationFax:		dest = kPMDestinationFax; break;
		case eDestinationPreview:	dest = kPMDestinationPreview; break;
	}
	return dest;
}



// ---------------------------------------------------------------------------
// GetDestinationURL												  [public]
// ---------------------------------------------------------------------------

CFURLRef
RWMacPageComposer::GetDestinationURL (void)
const
{
	CFURLRef	dstURL = 0;

	if (!mDestination.empty() && GetDestination() != eDestinationPrinter)
	{
		#if	0 && __MACH__ && !_MSL_USING_MW_C_HEADERS
			dstURL = CFURLCreateWithFileSystemPath (NULL,
						CFStringCreateWithCString (NULL, mDestination.GetFSName(), kCFStringEncodingUTF8),
						kCFURLPOSIXPathStyle,
						false);
		#else
			#if	0	//mbs 25072011	leak?!?
				dstURL = CFURLCreateWithFileSystemPath (NULL,
							RWStr::CreateCFString (mDestination),
							kCFURLHFSPathStyle,
							false);
			#else
				CFStringRef dest = RWStr::CreateCFString (mDestination);
				if (dest)
				{
					dstURL = CFURLCreateWithFileSystemPath (kCFAllocatorDefault, dest, kCFURLHFSPathStyle, false);
					CFRelease (dest);
				}
			#endif
		#endif
	}

	return dstURL;
}




// ---------------------------------------------------------------------------
// StyleChanged														  [public]
// ---------------------------------------------------------------------------

//void
//RWMacPageComposer::StyleChanged (RWStyle *inStyle)
//{
//	if (inStyle == NULL)
//	{
//		RWStyleToATSUStyleMap::const_iterator	it;
//
//		for (it = mStyleMap.begin(); it != mStyleMap.end(); it++)
//		{
//			ATSUStyle	style = (*it).second;
//			ATSUDisposeStyle (style);
//		}
//		mStyleMap.clear();
//	}
//	else
//	{
//		RWStyleToATSUStyleMap::key_type v (inStyle);
//		RWStyleToATSUStyleMap::iterator	it = mStyleMap.find (v);
//		if (it != mStyleMap.end())
//		{
//			ATSUStyle	style = (*it).second;
//			ATSUDisposeStyle (style);
//			mStyleMap.erase (it);
//		}
//	}
//}


// ---------------------------------------------------------------------------
// MapStyle														   [protected]
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
// OpenSession													   [protected]
// ---------------------------------------------------------------------------

OSStatus
RWMacPageComposer::OpenSession (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation)
{
	if (mBatchLevel == 0)
	{
# if	_4D_Package_
		if (mFlags & eUse4DPageSetup)
		{
			SetPageFormat (reinterpret_cast <PMPageFormat> (PA_GetCarbonPageFormat()));
			mFlags &= ~eUse4DPageSetup;
		}
		if (/* mPrintSettings == NULL && */ (mFlags & eUse4DJobSetup))
		{
			SetPrintSettings (reinterpret_cast <PMPrintSettings> (PA_GetCarbonPrintSettings()));
			mFlags &= ~eUse4DJobSetup;
		}
# endif
		OpenSessionArgs	data = { noErr, this, inDoPageSetup, inDoJobSetup, inCurPage, inNumPages, inOrientation };
		RW_RunInMainThread (OpenSessionCB, &data);
		return data.outResult;
	}
/*
	else
	{
		mPrintSession = mBatch->mPrintSession;
		PMRetain (mPrintSession);
		mDocIsOpen = true;
	}
*/
	return noErr;
}


// ---------------------------------------------------------------------------
// OpenSessionCB										  [static] [protected]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::OpenSessionCB (void *inData)
{	  
	OpenSessionArgs	&data = *reinterpret_cast <OpenSessionArgs*> (inData);
	try
	{
		data.outResult = data.inThis->OpenSessionSafe (data.inDoPageSetup, data.inDoJobSetup, data.inCurPage, data.inNumPages, data.inOrientation);
	}
	catch (long e)
	{
		data.outResult = e;
	}
	catch (...)
	{
		data.outResult = -1;
	}
}


// ---------------------------------------------------------------------------
// OpenSessionSafe												   [protected]
// ---------------------------------------------------------------------------

OSStatus
RWMacPageComposer::OpenSessionSafe (bool inDoPageSetup, bool inDoJobSetup, unsigned long inCurPage, unsigned long inNumPages, bool inOrientation)
{
	OSStatus	status = noErr;
	Boolean		bOK;

	//inDoPageSetup = inDoJobSetup = false;

	if (mPrintSession == NULL)
		status = PMCreateSession (&mPrintSession);

	if (status == noErr &&!mPrinterName.empty())	//mbs 25072011	printer
	{
		CFStringRef printerName = RWStr::CreateCFString (mPrinterName);
        CFArrayRef  printers = NULL;
        OSStatus err = PMServerCreatePrinterList( kPMServerLocal, &printers );
        if( err == noErr )
        {
            CFIndex i, count = CFArrayGetCount( printers );
            for(i = 0; i < count; ++i)
            {
                PMPrinter printer = (PMPrinter)CFArrayGetValueAtIndex( printers, i );
                CFStringRef name = PMPrinterGetName( printer );
                if (CFStringCompare(name, printerName, 0) == 0)
                {
                    status = PMSessionSetCurrentPMPrinter(mPrintSession, printer);
                
                }
            }
            if (printers)
                CFRelease (printers);
        }

		if (printerName)
			CFRelease (printerName);
	}

    NSPrintInfo *_printInfo = [[NSPrintInfo alloc] init];

	if (status == noErr)
	{

		if (mPageFormat == NULL)
		{

            status = PMCreatePageFormat (&mPageFormat);
			status = PMSessionDefaultPageFormat (mPrintSession, mPageFormat);
				
            if (inOrientation) {
				status = PMSetOrientation(mPageFormat, kPMLandscape, kPMUnlocked);
				status = PMSessionValidatePageFormat (mPrintSession, mPageFormat, &bOK);

				/*PMRect page;
				status = PMGetAdjustedPageRect (mPageFormat, &page);*/
			}
		}
		else
			status = PMSessionValidatePageFormat (mPrintSession, mPageFormat, &bOK);
        
        PMPageFormat pageFormat = (PMPageFormat) [_printInfo PMPageFormat];
        status = PMCopyPageFormat(mPageFormat, pageFormat);
        [_printInfo updateFromPMPageFormat];

	}

	if (status == noErr)
	{
		if (mPrintSettings == NULL)
		{
			status = PMCreatePrintSettings (&mPrintSettings);
			status = PMSessionDefaultPrintSettings (mPrintSession, mPrintSettings);
		}
		else
			status = PMSessionValidatePrintSettings (mPrintSession, mPrintSettings, &bOK);

        PMPrintSettings printSettings = (PMPrintSettings) [_printInfo PMPrintSettings];
        status = PMCopyPrintSettings(mPrintSettings, printSettings);
        [_printInfo updateFromPMPrintSettings];

		if (status == noErr && mPrintSettings != NULL && not mJobName.empty())
		{
			CFStringRef	n = RWStr::CreateCFString (mJobName);
			if (n)
			{
				PMPrintSettingsSetJobName (mPrintSettings, n);
				CFRelease (n);
			}
		}
	}
    
    
#if __LP64__
    if (status == noErr && inDoPageSetup && AskPageSetup()) {
        NSPageLayout *pageLayout = [NSPageLayout pageLayout];
        NSInteger buttonPressed = [pageLayout runModalWithPrintInfo:_printInfo];
        
        if (buttonPressed == NSOKButton) {
            _printInfo = [pageLayout printInfo];
            PMPageFormat pageFormat = (PMPageFormat) [_printInfo PMPageFormat];
            status = PMCopyPageFormat(pageFormat, mPageFormat);
            
        } else {
            status = kPMCancel;        // user clicked Cancel button
        }
        // [pageLayout release];
    }
    
    if (status == noErr && inDoJobSetup && AskJobSetup())
    {
        NSPrintPanel *printPanel = [NSPrintPanel printPanel];
        NSPrintPanelOptions options = [printPanel options] | NSPrintPanelShowsPaperSize;
        [printPanel setOptions:options];
        NSInteger buttonPressed = [printPanel runModalWithPrintInfo:_printInfo];

        if (buttonPressed == NSOKButton) {
            _printInfo = [printPanel printInfo];
            PMPrintSettings printSettings = (PMPrintSettings) [_printInfo PMPrintSettings];
            status = PMCopyPrintSettings(printSettings, mPrintSettings);

        } else {
            status = kPMCancel;        // user clicked Cancel button
        }
       //  [printPanel release];
       //  [_printInfo release];
        [NSApp setWindowsNeedUpdate:true];
    }
#else
	if (status == noErr && inDoPageSetup && AskPageSetup())
	{
		status = PMSessionPageSetupDialog (
										   mPrintSession,
										   mPageFormat,
										   &bOK );
		if (status == noErr && !bOK)
			status = kPMCancel;		// user clicked Cancel button
	}
	if (status == noErr && inDoJobSetup && AskJobSetup())
	{
        status = PMSetPageRange (mPrintSettings, 1, inNumPages);
		status = PMSessionPrintDialog (
									   mPrintSession,
									   mPrintSettings,
									   mPageFormat,
									   &bOK );
		if (status == noErr && !bOK)
			status = kPMCancel;		// user clicked Cancel button
	}
#endif

	if (status == noErr)
	{
		status = PMGetFirstPage (mPrintSettings, &mFirstPage);
		if (status == noErr)
			status = PMGetLastPage (mPrintSettings, &mLastPage);
		if (status == noErr)
			if (inNumPages < mLastPage)
				mLastPage = inNumPages;
	}

	//mbs 08072010
	if (status == noErr)
	{
		//mbs 29072010
		long	flags = mFlags;

		if (inDoPageSetup)
			mFlags &= ~eAskPageSetup;
		if (inDoJobSetup && AskJobSetup())
		{
			mFlags |= eRanJobSetup;
			mFlags &= ~eAskJobSetup;
		}

		status = OpenSessionSelf();

		//mbs 29072010
		if (status != noErr)
			mFlags = flags;
	}
	return status;
}


// ---------------------------------------------------------------------------
// CloseDocument												   [protected]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::CloseDocument (void)
{
	if (mDocIsOpen)
		if (mBatchLevel == 0)
			RW_RunInMainThread (CloseDocumentCB, this);
//		else
//			mDocIsOpen = false;
}


// ---------------------------------------------------------------------------
// CloseDocumentCB										  [static] [protected]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::CloseDocumentCB (void *inData)
{
	RWMacPageComposer	&data = *reinterpret_cast <RWMacPageComposer*> (inData);
#if i386
	if (data.ShowProgress())
		PMSessionEndDocument (data.mPrintSession);
	else
#endif
		PMSessionEndDocumentNoDialog (data.mPrintSession);
	data.mDocIsOpen = false;
}

// ---------------------------------------------------------------------------
// CloseSession													   [protected]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::CloseSession (bool inRelease)
{
	if (mPrintSession != NULL)
	{
		ClosePage();
		StyleChanged (NULL);	//mbs 08102010	clear cache

		if (mBatchLevel != 0)	//mbs 08102010	don't close session!!!
			inRelease = false;
		else
		{
			CloseDocument();
			PMRelease (mPrintSession);
			mPrintSession = NULL;
		}
	}
	if (inRelease)
	{
		if (mPageFormat != NULL)
		{
			PMRelease (mPageFormat);
			mPageFormat = NULL;
		}

		if (mPrintSettings != NULL)
		{
			PMRelease (mPrintSettings);
			mPrintSettings = NULL;
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// FinishReport														  [public]
// ---------------------------------------------------------------------------
// Close the page, close the session

void*
RWMacPageComposer::FinishReport (size_t &outSize)
{
	CloseSession (true);

	outSize = 0;

	return NULL;
}


// ---------------------------------------------------------------------------
// SetPageFormat													  [public]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::SetPageFormat (const PMPageFormat inPageFormat)
{
	mTruePageRect.SetRect (0, 0, 0, 0);
//	if (PMUnflattenPageFormat != (void*) kUnresolvedCFragSymbolAddress)
	{
		if (inPageFormat != NULL)
		{
			if (mPageFormat == NULL)
				PMCreatePageFormat (&mPageFormat);
			if (mPageFormat != NULL)
				PMCopyPageFormat (inPageFormat, mPageFormat);
		}
		else if (mPageFormat != NULL)
		{
			PMRelease (mPageFormat);
			mPageFormat = NULL;
		}
	}
}

void
RWMacPageComposer::SetPageFormat (const SBlob &inPageFormat)
{
	mTruePageRect.SetRect (0, 0, 0, 0);
//	if (PMUnflattenPageFormat != (void*) kUnresolvedCFragSymbolAddress)
	{
		if (inPageFormat)
		{
//			Handle		hdl = NULL;
//			OSStatus	status = PtrToHand (inPageFormat.fData, &hdl, inPageFormat.fSize);
            CFDataRef   data = CFDataCreate( kCFAllocatorDefault, (const UInt8 *)inPageFormat.fData, inPageFormat.fSize);
			if (data)
			{
				if (mPageFormat != NULL)
				{
					PMRelease (mPageFormat);
					mPageFormat = NULL;
				}
				(void) PMPageFormatCreateWithDataRepresentation (data, &mPageFormat);
                CFRelease(data);
			}
//			if (hdl)
//				DisposeHandle (hdl);
		}
		else
		{
			if (mPageFormat != NULL)
			{
				PMRelease (mPageFormat);
				mPageFormat = NULL;
			}
		}
	}
}


// ---------------------------------------------------------------------------
// SetPrintSettings													  [public]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::SetPrintSettings (const PMPrintSettings inPrintSettings)
{
//	if (PMUnflattenPageFormat != (void*) kUnresolvedCFragSymbolAddress)
	{
		if (inPrintSettings != NULL)
		{
			if (mPrintSettings == NULL)
				PMCreatePrintSettings (&mPrintSettings);
			if (mPrintSettings != NULL)
				PMCopyPrintSettings (inPrintSettings, mPrintSettings);
		}
		else if (mPrintSettings != NULL)
		{
			PMRelease (mPrintSettings);
			mPrintSettings = NULL;
		}
	}
}

void
RWMacPageComposer::SetPrintSettings (const SBlob &inPrintSettings)
{
//	if (PMUnflattenPrintSettings != (void*) kUnresolvedCFragSymbolAddress)
	{
		if (inPrintSettings)
		{
//			Handle		hdl = NULL;
//			OSStatus	status = PtrToHand (inPrintSettings.fData, &hdl, inPrintSettings.fSize);
            CFDataRef   data = CFDataCreate( kCFAllocatorDefault, (const UInt8 *)inPrintSettings.fData, inPrintSettings.fSize);
			if (data)
			{
				if (mPrintSettings != NULL)
				{
					PMRelease (mPrintSettings);
					mPrintSettings = NULL;
				}
//				status = PMUnflattenPrintSettings (hdl, &mPrintSettings);
                (void) PMPrintSettingsCreateWithDataRepresentation (data, &mPrintSettings);
                CFRelease(data);

			}
//			if (hdl)
//				DisposeHandle (hdl);
		}
		else
		{
			if (mPrintSettings != NULL)
			{
				PMRelease (mPrintSettings);
				mPrintSettings = NULL;
			}
		}
	}
}


// ---------------------------------------------------------------------------
// ParseReport														  [public]
// ---------------------------------------------------------------------------
// Get default page size & page orientation

void
RWMacPageComposer::ParseReport (RWXmlNode inReport)
{
/*
	if (mBatch)
	{
		*this = *mBatch;
	}
	else
*/
	if (mBatchLevel == 0)
	{
		RWPageComposer::ParseReport (inReport);

		SBlob		blob;
		blob.Init();
		try
		{
			RWXmlNode	elem;
			if ((mFlags & (eUseDefPageSetup | eUse4DPageSetup)) == 0)
			{
				elem = inReport.Child (u"PageFormat");
				if (elem) // && PMUnflattenPageFormat != (void*) kUnresolvedCFragSymbolAddress)
				{
					RWTools::ReadData (elem, blob);
					SetPageFormat (blob);
					blob.Free();
				}
			}

			if ((mFlags & (eUseDefJobSetup | eUse4DJobSetup)) == 0)
			{
				elem = inReport.Child (u"PrintSettings");
				if (elem) // && PMUnflattenPrintSettings != (void*) kUnresolvedCFragSymbolAddress)
				{
					RWTools::ReadData (elem, blob);
					SetPrintSettings (blob);
					blob.Free();
				}
			}
		}
		catch (...)
		{
		}
		if (blob)
			blob.Free();
	}

	return;
}


// ---------------------------------------------------------------------------
// GetPageBounds													  [public]
// ---------------------------------------------------------------------------
// Get default page size, page orientation and encoding

void
RWMacPageComposer::GetPageBounds (const RWString inOrientation, const RWString inSize, SRect &outRect)
{
	bool		changed = false;
	OSStatus	status;

	//mbs 24122009	always set the rectangles/margins!
	if (mPageRect.Width() == 0 || mTruePageRect.Width() == 0)
	{
        if (GetPageMetrics (mTruePageRect, mPaperRect, mReportPageMargins)) {
			if (mUsePhysical) {
				mPageRect = mPaperRect + SPoint (mReportPageMargins.left, mReportPageMargins.top);
			} else {
				mPageRect = mTruePageRect;
			}
        }
	}

	if (mPageRect.Width() == 0 || mTruePageRect.Width() == 0 || AskPageSetup())
	{
		status = OpenSession (true, false, 1, 1, RWStr::Equals (mPageOrientation, "Landscape"));
		CloseSession (false);
		if (status != noErr)
		{
			printf ("RWMacPageComposer::GetPageBounds: OpenSession: status != noErr || mGC == nil\n");
			throw status;
		}
		changed = true;
	}

	//mbs 24122009	always set the rectangles/margins!
	if (changed)
	{
        if (GetPageMetrics (mTruePageRect, mPaperRect, mReportPageMargins)) {
			if (mUsePhysical) {
				mPageRect = mPaperRect + SPoint (mReportPageMargins.left, mReportPageMargins.top);
			} else {
				mPageRect = mTruePageRect;
			}
        }
	}

	if (mPageRect.Width() == 0)
		RWPageComposer::GetPageBounds (inOrientation, inSize, outRect);
	else
		outRect = mPageRect;

	return;
}


SRect
RWMacPageComposer::GetTruePageRect (void) const
{
	if (mUsePhysical)
		return mTruePageRect + SPoint (mTruePageRect.left - mPaperRect.left, mTruePageRect.top - mPaperRect.top);
	return mTruePageRect;
}

// ---------------------------------------------------------------------------
// GetPageMetrics													  [public]
// ---------------------------------------------------------------------------
// Get page size and margins

bool
RWMacPageComposer::GetPageMetrics (float &outPageWidth, float &outPageHeight, SRect &outMargins)
{
	SRect pageRect, paperRect;
	if (GetPageMetrics (pageRect, paperRect, outMargins))
	{
		outPageWidth = pageRect.Width();
		outPageHeight = pageRect.Height();
		return true;
	}
	return false;
}


// ---------------------------------------------------------------------------
// GetPageMetrics													  [public]
// ---------------------------------------------------------------------------
// Get page size and margins

bool
RWMacPageComposer::GetPageMetrics (SRect &outPageRect, SRect &outPaperRect, SRect &outMargins)
{
	if (mPageFormat != NULL)
	{
		PMRect		page, paper;
		OSStatus	status;
//		status = PMGetPhysicalPaperSize (mPageFormat, &paper);
//		status = PMGetPhysicalPageSize (mPageFormat, &page);
//		status = PMGetUnadjustedPaperRect (mPageFormat, &paper);
//		status = PMGetUnadjustedPageRect (mPageFormat, &page);
		status = PMGetAdjustedPageRect (mPageFormat, &page);
		if (status == noErr)
			status = PMGetAdjustedPaperRect (mPageFormat, &paper);
		if (status == noErr)
		{
			outPageRect.SetRect ((float) page.top, page.left, page.bottom, page.right);
			outPaperRect.SetRect ((float) paper.top, paper.left, paper.bottom, paper.right);
			//mbs 12082010	keep report margins
			SRect margins (float (page.top - paper.top), page.left - paper.left, paper.bottom - page.bottom, paper.right - page.right);
			if (0 == (mFlags & eResetMargins))
			{
				if (margins.top < mReportPageMargins.top)
					margins.top = mReportPageMargins.top;
				if (margins.left < mReportPageMargins.left)
					margins.left = mReportPageMargins.left;
				if (margins.bottom < mReportPageMargins.bottom)
					margins.bottom = mReportPageMargins.bottom;
				if (margins.right < mReportPageMargins.right)
					margins.right = mReportPageMargins.right;
			}
			outMargins = margins;
			return true;
		}
	}
	return false;
}




// ---------------------------------------------------------------------------
// OpenNewPage														  [public]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::OpenNewPage (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages)
{
	OpenNewPageArgs	data = { noErr, this, inRect, inCurPage, inNumPages };
	RW_RunInMainThread (OpenNewPageCB, &data);
	if (data.outResult)
		throw long (data.outResult);
}


// ---------------------------------------------------------------------------
// OpenNewPageCB										  [static] [protected]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::OpenNewPageCB (void *inData)
{
	OpenNewPageArgs	&data = *reinterpret_cast <OpenNewPageArgs*> (inData);
	data.outResult = noErr;
	try
	{
		data.inThis->OpenNewPageSafe (data.inRect, data.inCurPage, data.inNumPages);
	}
	catch (long e)
	{
		data.outResult = e;
	}
	catch (...)
	{
		data.outResult = -1;
	}
}


// ---------------------------------------------------------------------------
// OpenNewPageSafe												   [protected]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::OpenNewPageSafe (const SRect &inRect, unsigned long inCurPage, unsigned long inNumPages)
{
	OSStatus	status = noErr;
	if (not mDocIsOpen)
	{
		status = OpenSession (false, true, inCurPage, inNumPages, RWStr::Equals (mPageOrientation, "Landscape"));
		if (status != noErr)
		{
			printf ("RWMacPageComposer::OpenNewPage: OpenSession: status != noErr || mGC == nil\n");
			throw status;
		}
		if (mBatchLevel == 0)
		{
			status = PMSetFirstPage (mPrintSettings, mFirstPage, false);
			status = PMSetLastPage (mPrintSettings, mLastPage, false);
		}
		//mbs 08102010	always open document!!!
#if i386
			if (ShowProgress())
				status = PMSessionBeginCGDocument (
										mPrintSession,
										mPrintSettings,
										mPageFormat );
			else
#endif
				status = PMSessionBeginCGDocumentNoDialog (
												 mPrintSession,
												 mPrintSettings,
												 mPageFormat );
		mDocIsOpen = true;
	}
	else
		ClosePage();

	if (inCurPage < mFirstPage)
		return;
	if (inCurPage > mLastPage)
		return;

/*
	PMRect	rect = { inRect.top, inRect.left, inRect.bottom, inRect.right }, orig;
	status = PMGetPhysicalPaperSize (mPageFormat, &orig);
	status = PMSetPhysicalPaperSize (mPageFormat, &rect);
	status = PMSessionValidatePageFormat (mPrintSession, mPageFormat, &bOK);
	status = PMGetPhysicalPaperSize (mPageFormat, &orig);

	status = PMGetUnadjustedPaperRect (mPageFormat, &orig);
	status = PMSetUnadjustedPaperRect (mPageFormat, &rect);
	status = PMSessionValidatePageFormat (mPrintSession, mPageFormat, &bOK);
	status = PMGetUnadjustedPaperRect (mPageFormat, &orig);

 status = PMSetOrientation (mPageFormat, RWStr::EqualsNoCase (mPageOrientation, "Landscape") ? kPMLandscape : kPMPortrait, false);
*/

#if i386

	if (ShowProgress())
		status = PMSessionBeginPage (
									mPrintSession,
									mPageFormat,
									NULL );
	else
#endif
		status = PMSessionBeginPageNoDialog (
									 mPrintSession,
									 mPageFormat,
									 NULL );
	if (status != noErr)
	{
		printf ("RWMacPageComposer::OpenNewPage: PMSessionBeginPage: status != noErr || mGC == nil\n");
		throw status;
	}

	mPageIsOpen = true;
//	mPageRect = inRect;
/*
	PMRect	pageRect;
	status = PMGetAdjustedPageRect (mPageFormat, &pageRect);
	mPageRect.top = ROUND_UP (pageRect.top);
	mPageRect.left = ROUND_UP (pageRect.left);
	mPageRect.bottom = ROUND_UP (pageRect.bottom);
	mPageRect.right = ROUND_UP (pageRect.right);
*/

	OpenNewPageSelf();

#if	0 && TARGET_DEBUG
	DrawRect (mPageRect, 1, true, cRedColor, false, cRedColor, 1.8, 1.7);
#endif

	return;
}




// ---------------------------------------------------------------------------
// ClosePage														  [public]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::ClosePage (void)
{
	if (mPageIsOpen)
		RW_RunInMainThread (ClosePageCB, this);
}


// ---------------------------------------------------------------------------
// ClosePageCB											  [static] [protected]
// ---------------------------------------------------------------------------

void
RWMacPageComposer::ClosePageCB (void *inData)
{
	RWMacPageComposer	&data = *reinterpret_cast <RWMacPageComposer*> (inData);
	if (data.ShowProgress())
		data.ClosePageSelf (false);
#if i386
	if (data.ShowProgress())
		PMSessionEndPage (data.mPrintSession);
	else
#endif
		PMSessionEndPageNoDialog (data.mPrintSession);
	data.mPageIsOpen = false;
	data.ClosePageSelf (true);
}

void
RWMacPageComposer::DrawTextBox (const RWString inText, RWStyle *inStyle, const SRect &inRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText)
{
if (true)	// (sUseTF) - we don't have MeasureWord()/DrawWord()
	RWPageComposer::DrawTextBox (inText, inStyle, inRect, inWrap, inAttributed, inFit, ioPrintText);
else
{
#if 0
	SRect	r (inRect);
	if (ioPrintText != NULL && *ioPrintText != NULL)
		inText = (*ioPrintText)->GetText();

	if (mPageIsOpen)
	{
		RWMacPrintTextContext	ctx;
		ctx.fCGContext = DrawTextBoxBegin (inStyle, inRect);
		ctx.fPageBottom = mPageRect.bottom;

		try
		{
			if (inText != NULL && *inText)
			{
				if (ioPrintText == NULL)	// RWTable support
				{
					RWMacPrintText	txt (inText, inStyle, inAttributed);
					txt.Init (*this, r, inWrap, inFit);
					r = inRect;
					txt.Draw (*this, r, inFit, true, &ctx);
				}
				else
				{
					if (*ioPrintText == NULL)
					{
						*ioPrintText = new RWMacPrintText (inText, inStyle, inAttributed);
						static_cast <RWMacPrintText*> (*ioPrintText)->Init (*this, r, inWrap, inFit);
						r = inRect;
					}
					static_cast <RWMacPrintText*> (*ioPrintText)->Draw (*this, r, inFit, true, &ctx);
				}
			}
			DrawTextBoxEnd (ctx.fCGContext);
		}
		catch (...)
		{
			DrawTextBoxEnd (ctx.fCGContext);
			throw;
		}
	}
	else if (ioPrintText != NULL && *ioPrintText != NULL)	// nothing to do if we don't draw and ioPrintText is empty
	{
		if (inText != NULL && *inText)
			static_cast <RWMacPrintText*> (*ioPrintText)->Draw (*this, r, inFit, true, NULL);
	}
#endif
}
}

double
RWMacPageComposer::MeasureText (const RWString inText, RWStyle *inStyle, SRect &ioRect, bool inWrap, bool inAttributed, bool inFit, RWPrintText **ioPrintText)
{
if (true)	// (sUseTF) - we don't have MeasureWord()/DrawWord()
	return RWPageComposer::MeasureText (inText, inStyle, ioRect, inWrap, inAttributed, inFit, ioPrintText);
else
{
#if 0
	if (inText != NULL && *inText)
	{
		if (ioPrintText == NULL)	// RWTable support
		{
			RWMacPrintText	txt (inText, inStyle, inAttributed);
			txt.Init (*this, ioRect, inWrap, inFit);
//			txt.Draw (*this, ioRect, inFit, false, NULL);
		}
		else
		{
			if (*ioPrintText == NULL)
			{
				*ioPrintText = new RWMacPrintText (inText, inStyle, inAttributed);
				static_cast <RWMacPrintText*> (*ioPrintText)->Init (*this, ioRect, inWrap, inFit);
			}
			else
				static_cast <RWMacPrintText*> (*ioPrintText)->Draw (*this, ioRect, inFit, false, NULL);
		}
	}
	else
	{
		ioRect.bottom = ioRect.top;
		ioRect.right = ioRect.left;
	}

	return ioRect.Width();
#endif
}
}


#pragma	mark -

// Determines if the ATSUBatchBreakLines() is to be used
#define USE_BATCHBREAKLINES __MACH__

// Determines if a pointer to the entire text buffer or just
// the local portion of the text buffer is to be used when
// setting up a layout. Setting thie to 0 on systems prior
// to 10.4 can be problematic, and is not recommended.
#define USE_LOCAL_POINTERS 1

// Controls the method used to determine line height
#define USE_GETGLYPHBOUNDS 0



/*
bool
RWMacPrintText::GetBounds (RWMacPageComposer &inComposer, SRect &ioRect, bool inFit)
{
	DrawLines (inComposer, ioRect, inFit, false, NULL);
	return mFullRect.bottom <= ioRect.bottom;
}
*/

#if 0
int
RWMacPrintText::Draw (RWMacPageComposer &inComposer, SRect &ioRect, bool inFit, bool inDoDraw, void *inContext)
{
	if (mNumLines > mPrintedLines)
		return DrawLines (inComposer, ioRect, inFit, inDoDraw, reinterpret_cast <RWMacPrintTextContext*> (inContext));
	return 0;
}


int
RWMacPrintText::DrawLines (RWMacPageComposer &inComposer, SRect &ioRect, bool inFit, bool inDoDraw, RWMacPrintTextContext *inContext)
{
	// inDoDraw is true if we draw (otherwise we measure)
	// if we draw, inFit means "only full height lines", otherwise it means "get height limited to ioRect"
	ATSUTextLayout				layout;
    UniCharArrayOffset			layoutStart, currentStart, currentEnd;
	UniCharCount				layoutLength;
    ATSUAttributeTag			tags[1];
    ByteCount					sizes[1];
    ATSUAttributeValuePtr		values[1];
	Fixed						lineWidth, ascent, descent;
	float						y;
	float						endY;
    ItemCount					numSoftBreaks;
    UniCharArrayOffset			*theSoftBreaks;
	CFIndex						i;
	ItemCount					j;
	bool						draw = inDoDraw;
	bool						done = false;
	bool						advance = true;
	SPoint						origTL (ioRect.TopLeft());
	bool						measureWidth = (ioRect.Width() == 0);
	float						width;
# define	kMeasureWidth		4000

	if (measureWidth && not inDoDraw)
		mWidth = 0;
	if (measureWidth)
		ioRect.right = ioRect.left + kMeasureWidth;

	if (inDoDraw && inContext && inContext->fCGContext)
	{
		CGContextSaveGState (inContext->fCGContext);
		CGRect	rect = CGRectMake (ioRect.left, inContext->fPageBottom - ioRect.bottom, ioRect.Width(), ioRect.Height());
//		if (inFit)
			CGContextClipToRect (inContext->fCGContext, rect);
#if	0 && TARGET_DEBUG
		inComposer.DrawRect (ioRect, 0.25, true, cBlackColor, false, cWhiteColor, 0.5, 3);
#endif
	}

	if (mStyle->GetRotation() != 0)
	{
		if (measureWidth)
			ioRect.bottom = ioRect.top + kMeasureWidth;
		CGAffineTransform	t = RWTools::MakeMatrixFromUserRect (reinterpret_cast <const RWPrintContextRef> (inContext), ioRect, mStyle->GetRotation(), mWidth, mHeight);
		if (inDoDraw && inContext && inContext->fCGContext)
		{
#if	0 && TARGET_DEBUG
			const SRGBColor	color	( 65535, 0, 65535, 65535 );
			inComposer.DrawRect (ioRect, 0.25, true, color, false, cWhiteColor, 1, 2);
#endif
			CGContextConcatCTM (inContext->fCGContext, t);
#if	0 && TARGET_DEBUG
			inComposer.DrawRect (ioRect, 0.25, true, cBlueColor, false, cWhiteColor, 1, 2);
#endif
		}
	}

	// vertical alignment
	if (inDoDraw && inContext && ioRect.Height() >= mHeight)
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
			float	delta = (ioRect.Height() - mHeight) / 2;
			ioRect.top += delta;
			ioRect.bottom -= delta;
		}
#if	0 && TARGET_DEBUG
		inComposer.DrawRect (ioRect, 0.25, true, cBlackColor, false, cWhiteColor, .75, 3);
#endif
	}

//	EraseRect ();

	// Prepare the coordinates for drawing. In our example, "x" and "y" are the coordinates in QD space.
//	lineWidth = X2Fix (measureWidth ? kMeasureWidth : ioRect.Width());
	lineWidth = X2Fix (ioRect.Width());
//	x = ioRect.left;
	y = ioRect.top;
	endY = y;
	if (not inDoDraw && not inFit)	//mbs 03012010
		ioRect.bottom = ioRect.top + 2e10;

	// Loop over all the layouts, break them into lines, then draw them
	int	line = 0;
	CFIndex	numLayouts = 0;
	if (mLayouts)
		numLayouts = CFArrayGetCount (mLayouts);
	for (i = 0; i < numLayouts; i++)
	{
		layout = (ATSUTextLayout)CFArrayGetValueAtIndex (mLayouts, i);

		// In this example, we are breaking text into lines.
		// Therefore, we need to make sure the layout knows the width of the line.
		tags[0] = kATSULineWidthTag;
		sizes[0] = sizeof(Fixed);
		values[0] = &lineWidth;
		verify_noerr( ATSUSetLayoutControls (layout, 1, tags, sizes, values) );

		// Make sure the layout knows the proper CGContext to use for drawing
		if (inContext && inContext->fCGContext)
		{
			tags[0] = kATSUCGContextTag;
			sizes[0] = sizeof(CGContextRef);
			values[0] = &inContext->fCGContext;
			verify_noerr( ATSUSetLayoutControls (layout, 1, tags, sizes, values) );
		}

		// Find out about this layout's text buffer
	    verify_noerr( ATSUGetTextLocation (layout, NULL, NULL, &layoutStart, &layoutLength, NULL) );

		// Break the text into lines
		//
		// There are two methods for doing this. ATSUBreakLine() will do line breaks one at a time,
		// while ATSUBatchBreakLines() will break an entire paragraph at once. ATSUBatchBreakLines()
		// offers up to 50% greater performance than ATSUBreakLine(), but is only available on 10.2
		// and later.
		//
        #if USE_BATCHBREAKLINES
            verify_noerr( ATSUBatchBreakLines (layout, layoutStart, layoutLength, lineWidth, &numSoftBreaks) );
        #else
            currentStart = layoutStart;
            currentEnd = layoutStart + layoutLength;
            do {
                verify_noerr( ATSUBreakLine (layout, currentStart, lineWidth, true, &currentEnd) );
                currentStart = currentEnd;
            } while ( currentEnd < layoutStart + layoutLength );
        #endif

		// Obtain a list of all the line break positions
		verify_noerr( ATSUGetSoftLineBreaks (layout, layoutStart, layoutLength, 0, NULL, &numSoftBreaks) );
		theSoftBreaks = (UniCharArrayOffset *) malloc (numSoftBreaks * sizeof(UniCharArrayOffset));
		verify_noerr( ATSUGetSoftLineBreaks (layout, layoutStart, layoutLength, numSoftBreaks, theSoftBreaks, &numSoftBreaks) );

		// Loop over all the lines and draw them
		currentStart = layoutStart;
		for (j=0; j <= numSoftBreaks; j++)
		{
			currentEnd = ((numSoftBreaks > 0 ) && (numSoftBreaks > j)) ? theSoftBreaks[j] : layoutStart + layoutLength;

			// This is the height of a line, the ascent and descent.
			//
			// The ascent is the amount of text that extends above the baseline.
			// The descent is the amount of text that extends below the baseline.
			// (The y-coordinate that is passed to ATSUDrawText is where the baseline will be drawn.)
			//
			// Many fonts also include "leading", which is extra space specified by the font designer
			// to be applied below the baseline when spacing apart lines. Leading is usually included
			// when fetching the descent, unless the kATSLineIgnoreFontLeading layout control is set,
			// or when using ATSUGetAttribute to fetch the descent directly from a style (in that case,
			// use kATSULeadingTag to fetch the leading separately).
			//
			// There are two methods for getting these values: using ATSUGetLineControl() and ATSUGetGlyphBounds()
			// The ATSUGetLineControl method is preferred, but only works on 10.2 and later systems. The
			// ATSUGetGlyphBounds method works on all systems, including Classic and CarbonLib on Mac OS 8 and 9.
			//
			#if USE_GETGLYPHBOUNDS
				ATSTrapezoid theBounds;
				ItemCount numBounds;

				// Note that when calling ATSUGetGlyphBounds on an entire line at once, there is always only one trapezoid returned.
				ATSUGetGlyphBounds (layout, 0, 0, currentStart, currentEnd - currentStart, kATSUseFractionalOrigins, 1, &theBounds, &numBounds);
				ascent = - theBounds.upperLeft.y;
				descent = theBounds.lowerLeft.y;
			#else
				ATSUGetLineControl (layout, currentStart, kATSULineAscentTag, sizeof(ATSUTextMeasurement), &ascent, NULL);
				ATSUGetLineControl (layout, currentStart, kATSULineDescentTag, sizeof(ATSUTextMeasurement), &descent, NULL);
			#endif

			if (measureWidth && j == 0 && not inDoDraw)
			{
/*
				Rect				uBounds = { 0, 0, 0, 0 }, uImage;
				ATSUStyle			ustyle;
				UniCharArrayOffset	runStart;
				UniCharCount		runLength;
				ATSUGetRunStyle (layout, 0, &ustyle, &runStart, &runLength);
				MeasureUnicodeString (mUniText + currentStart, currentEnd - currentStart, ustyle, &uBounds, &uImage);
				ioRect.right = ROUND_UP (ioRect.left + uBounds.right - uBounds.left);
				ioRect.bottom = ROUND_UP (ioRect.top + uBounds.bottom - uBounds.top);
*/
				OSStatus			err;
				{
#if	__MACH__
#if	1
					ATSUTextMeasurement	oTextBefore, oTextAfter, oAscent, oDescent;
					err = ATSUGetUnjustifiedBounds (layout, currentStart, currentEnd - currentStart,
													&oTextBefore, &oTextAfter, &oAscent, &oDescent );
//					SetRect(textbounds, - FixRound(oTextBefore), - FixRound(oAscent), FixRound(oTextAfter), FixRound(oDescent + oLeading)+1);
//					ioRect.right = ROUND_UP (ioRect.left + Fix2X (oTextBefore + oTextAfter));
//					ioRect.bottom = ROUND_UP (ioRect.top + Fix2X (oAscent + oDescent) + 1);
					width = Fix2X (oTextBefore + oTextAfter);
#else
					Rect	rect;
					err = ATSUMeasureTextImage (layout, currentStart, currentEnd - currentStart, 0, 0, &rect);
//					ioRect.right = ROUND_UP (ioRect.left + rect.right - rect.left);
//					ioRect.bottom = ROUND_UP (ioRect.top + rect.bottom - rect.top + 1);
					width = rect.right - rect.left;
#endif
#else
					ATSUTextMeasurement	oTextBefore, oTextAfter, oAscent, oDescent;
					err = ATSUMeasureText ( layout, currentStart, currentEnd - currentStart,
										  &oTextBefore, &oTextAfter, &oAscent, &oDescent);
//					SetRect(textbounds, - FixRound(oTextBefore), - FixRound(oAscent), FixRound(oTextAfter), FixRound(oDescent + oLeading)+1);
//					ioRect.right = ROUND_UP (ioRect.left + Fix2X (oTextBefore + oTextAfter));
//					ioRect.bottom = ROUND_UP (ioRect.top + Fix2X (oAscent + oDescent) + 1);
					width = Fix2X (oTextBefore + oTextAfter);
#endif
				}
				if (line == 0)
				{
					mWidth = width;
					mLineHeight = Fix2X (ascent) + Fix2X (descent);
				}
				else if (width > mWidth)
					mWidth = width;
			}
			else if (line == 0)
			{
				mWidth = ioRect.Width();
				mLineHeight = Fix2X (ascent) + Fix2X (descent);
			}

			if (mPrintedLines <= line)
			{
				if (y + Fix2X (ascent) + Fix2X (descent) > ioRect.bottom) // if we go past the end of the frame, then:
				{
					if (inFit || y >= ioRect.bottom)	// allow partial row to be printed
					{
						advance = false;
						if (mNumLines == -1)			// we don't know the full height, yet - continue
							draw = false;
						else							// stop
						{
							done = true;
							break;
						}
					}
				}

				// Make room for the area above the baseline.
				y += Fix2X (ascent);

				// Draw the text
				if (draw && inContext && inContext->fCGContext)
				{
					float	cgY = inContext->fPageBottom - y; // Subtract the y coordinate from the height of the page to get the coordinate in CG-aware space.
					verify_noerr( ATSUDrawText(layout, currentStart, currentEnd - currentStart, X2Fix (ioRect.left), X2Fix (cgY)) );
				}

				// Make room for the area below the baseline
				y += Fix2X(descent);

				// ••• TODO •••	incorrect leading
				y++;
				if (advance)
					endY = y;
			}

			// Prepare for next line
			currentStart = currentEnd;
			line++;
			if (not mWrap)
				break;
		}
		free (theSoftBreaks);
		if ( done )
			break; // if we go past the end of the window, stop
	}

    // Tear down the CGContext
//	if (inDoDraw && inContext && inContext->fCGContext)
//		CGContextFlush(inContext->fCGContext);

	if (mNumLines == -1)
	{
		mHeight = y - ioRect.top;
		mNumLines = line;
		ioRect.bottom = y;
	}

	if (inDoDraw)
	{
		if (inFit)
			ioRect.bottom = endY;
	}
	else
	{
		if (measureWidth)
			ioRect.right = ioRect.left + mWidth;
		if (inFit)
			ioRect.bottom = endY;
		else
			ioRect.bottom = ioRect.top + mHeight - mPrintedHeight;
	}

	if (inDoDraw /* && inContext && inContext->fCGContext */)
	{
		mPrintedLines = line;
		mPrintedHeight += endY - ioRect.top;
	}

	if (inDoDraw && inContext && inContext->fCGContext)
	{
#if	0 && TARGET_DEBUG
		inComposer.DrawRect (ioRect, 0.5, true, cGreenColor, false, cWhiteColor, 0.5, 1);
#endif
		CGContextRestoreGState (inContext->fCGContext);
	}

	if (/* inDoDraw && inContext && inContext->fCGContext && */ mStyle->GetRotation() != 0)
	{
		RWTools::MakeUserRectFromText (ioRect, mStyle->GetRotation());
		ioRect.bottom = origTL.v + ioRect.Height();
		ioRect.right = origTL.h + ioRect.Width();
		ioRect.top = origTL.v;
		ioRect.left = origTL.h;
	}
#if	0 && TARGET_DEBUG
	if (inDoDraw && inContext && inContext->fCGContext)
	{
		inComposer.DrawRect (ioRect, 0.5, true, cRedColor, false, cWhiteColor, 0.5, 0.5);
	}
#endif

	return mPrintedLines;
}

bool
RWMacPrintText::FindParagraph (UniCharArrayOffset paragraphStart, UniCharArrayOffset *paragraphEnd)
{
    UniChar         CR   = 0x000D;  // ASCII carrige return  '\r'
    UniChar         LF   = 0x000A;  // ASCII newline         '\n'
    UniChar         LSEP = 0x2028;  // Unicode line separator
    UniChar         PSEP = 0x2029;  // Unicode paragraph separator
    UniCharCount    currentPosition;
    UniChar         currentChar;
    Boolean         endOfText = false;

    // Check to see if we even have any text
    //
    if (mUniText == NULL)
    {
        *paragraphEnd = 0;
        return true;
    }

    // Loop over the text and check for hard line breaks
    //
    // There are five possbile sequences that constitute a hard line break:
    // (all five are considered valid)
    //
    //      LF      (Unix style)
    //      CR      (Mac style)
    //      CRLF    (Windows/DOS style)
    //      PSEP    (New Unicode style, used by BBEdit, WorldText, and TextEdit)
    //      LSEP    (Other new Unicode style)
    //
    for (currentPosition=paragraphStart; (currentPosition < mTextLength); currentPosition++)
    {
        currentChar = mUniText[currentPosition];

        if ( (currentChar == PSEP) || (currentChar ==  LSEP) || (currentChar ==  LF) )
        {
            break;
        }

        if ( currentChar == CR )
        {
            if ( currentPosition < (mTextLength - 1) )
            {
                if ( mUniText[currentPosition + 1] == LF )
                {     // Treat DOS/Windows style CRLF line breaks as a single entity
                    currentPosition++;
                }
            }
            break;
        }

    }

    // Special case -- if we reached the end of the text but didn't find a
    // separator, currentPostion will be one too many, so decrement it.
    //
    if (currentPosition == mTextLength)
    {
        currentPosition--;
    }

    // See if we went all the way to the end of the text
    //
    if (currentPosition == (mTextLength - 1))
    {
        endOfText = true;
    }

    // This is an ATSUI-style offset (i.e., "between" the array elements),
    // so we have to return the offset that is AFTER the paragraph separator.
    // (Note that if we encounter a CRLF style break, we treat that a single entity; see above)
    //
    *paragraphEnd = currentPosition + 1;
    return endOfText;
}
#endif

#if 0
static	OSStatus GetNamedFontID(const char* fontName, ATSUFontID *theFontID)
{
	return ATSUFindFontFromName(
								const_cast <char*> (fontName),
								strlen(fontName),
								kFontFullName,
								kFontMacintoshPlatform,
								kFontNoScript,
								kFontNoLanguageCode, theFontID);
}

static int FIDRECCMP(const void *a, const void *b) {
	FontNameIDRec *fida, *fidb;
	fida = (FontNameIDRec *) a;
	fidb = (FontNameIDRec *) b;
	return strcmp(fida->name, fidb->name);
}

//static
OSStatus GetInstalledFontList(
									  FontNameIDVector *fontList,	/* place to return pointer to list */
									  long *listLength) {			/* place to return length of list */
	OSStatus err;
	ItemCount i, numFonts;
	ATSUFontID *fontIDList = NULL;
	FontNameIDVector fiv;
	ItemCount fivlen;
	/* set up */
	fivlen = 0;
	fiv = NULL;
	/* iterate over installed fonts */
	err = ATSUFontCount( &numFonts );
	if ( err == noErr ) {
		fontIDList = (ATSUFontID*) malloc( numFonts * sizeof(ATSUFontID) );
		if (fontIDList == NULL) {
			err = memFullErr;
		} else {
			fiv = (FontNameIDVector) malloc( numFonts * sizeof(FontNameIDRec) );
			if (fiv == NULL) {
				err = memFullErr;
			} else {
				err = ATSUGetFontIDs( fontIDList, numFonts, NULL );
				if (err == noErr) {
					for ( i = 0; i < numFonts; i++ ) {
						ByteCount namelen;
						ItemCount oNameIndex;
						char namestring[512];
						/* best effort here.  If the name lookup or the name tests
						 fail, then we simply don't include the item in the list */
						err = ATSUFindFontName( fontIDList[ i ], kFontFullName,
											   kFontMacintoshPlatform, kFontRomanScript, kFontEnglishLanguage,
											   sizeof(namestring), namestring, &namelen, &oNameIndex );
						if (err == noErr) {
							namestring[namelen] = 0;
							if ( namelen > 0 && namestring[0] != '.' && namestring[0] != '%' && namestring[0] != '#' ) {
								char* p;
								p = (char*) malloc(strlen(namestring) + 1);
								if (p == NULL) {
									err = memFullErr;
									break;
								} else {
									strcpy(p, namestring);
									fiv[fivlen].theFontID = fontIDList[ i ];
									fiv[fivlen++].name = p;
								}
							}
						}
					}

				}
			}
			free(fontIDList);
		}
	}
	/* sort the resulting list */
	if (err == noErr) {
		if (fivlen > 0) {
			qsort(fiv, fivlen, sizeof(FontNameIDRec), FIDRECCMP);
		}
		*fontList = fiv;
		*listLength = fivlen;
	} else {
		if (fiv != NULL) {
			for (i=0; i<fivlen; i++)
				free(fiv[i].name);
			free(fiv);
		}
	}
	return err;
}

/* MakeSimpleATSUIStyle creates a simple ATSUI style record that
 can be used in calls to the RenderCFString routine. */
static	OSStatus MakeSimpleATSUIStyle(ATSUFontID theFontID, short fontSize, short qdStyle, ATSURGBAlphaColor *fontColor, ATSUStyle *theStyle) {
	ATSUFontID atsuFont;
	OSStatus err;
	/* Three parrallel arrays for setting up attributes. */
#define kTagMax 32
	ItemCount tagCount;
	ATSUAttributeTag theTags[kTagMax];
	ByteCount theSizes[kTagMax];
	ATSUAttributeValuePtr theValues[kTagMax];
	Fixed atsuSize;
	short atsuOrientation;
	ATSURGBAlphaColor defaultColor = { 0, 0, 0, 1 };
	Boolean trueVar = true, falseVar = false;
	ATSUStyle localStyle;
	/* initial tag count */
	tagCount = 0;
	/* the font */
	atsuFont = theFontID;
	theTags[tagCount] = kATSUFontTag;
	theSizes[tagCount] = sizeof(ATSUFontID);
	theValues[tagCount++] = &atsuFont;
	/* the size */
	atsuSize = FixRatio(fontSize, 1);
	theTags[tagCount] = kATSUSizeTag;
	theSizes[tagCount] = sizeof(Fixed);
	theValues[tagCount++] = &atsuSize;
	/* the orientation */
	atsuOrientation = kATSUStronglyHorizontal;
	theTags[tagCount] = kATSUVerticalCharacterTag;
	theSizes[tagCount] = sizeof(UInt16);
	theValues[tagCount++] = &atsuOrientation;
	/* font color */
	theTags[tagCount] = kATSURGBAlphaColorTag;	// kATSUColorTag;
	theSizes[tagCount] = sizeof(ATSURGBAlphaColor);
	theValues[tagCount++] = (fontColor ? fontColor: &defaultColor);
	/* bold */
	theTags[tagCount] = kATSUQDBoldfaceTag;
	theSizes[tagCount] = sizeof(Boolean);
	theValues[tagCount++] = ((qdStyle & bold) != 0 ? &trueVar : &falseVar);
	/* italic */
	theTags[tagCount] = kATSUQDItalicTag;
	theSizes[tagCount] = sizeof(Boolean);
	theValues[tagCount++] = ((qdStyle & italic) != 0 ? &trueVar : &falseVar);
	/* underline */
	theTags[tagCount] = kATSUQDUnderlineTag;
	theSizes[tagCount] = sizeof(Boolean);
	theValues[tagCount++] = ((qdStyle & underline) != 0 ? &trueVar : &falseVar);
	/* condensed */
	theTags[tagCount] = kATSUQDCondensedTag;
	theSizes[tagCount] = sizeof(Boolean);
	theValues[tagCount++] = ((qdStyle & condense) != 0 ? &trueVar : &falseVar);
	/* extended */
	theTags[tagCount] = kATSUQDExtendedTag;
	theSizes[tagCount] = sizeof(Boolean);
	theValues[tagCount++] = ((qdStyle & extend) != 0 ? &trueVar : &falseVar);
	err = ATSUCreateStyle(&localStyle);
	if (err == noErr) {
		err = ATSUSetAttributes( localStyle, tagCount, theTags, theSizes, theValues );
		if (err == noErr) {
			*theStyle = localStyle;
		} else {
			ATSUDisposeStyle(localStyle);
		}
	}
	/* done, return */
	return err;
}
#endif




static	CGDisplayErr GetDisplayDPI(
    CFDictionaryRef displayModeDict,
    CGDirectDisplayID displayID,
    float &horizontalDPI, float &verticalDPI );


bool	RWMacPageComposer::GetDPI (float &x, float &y, void* inWindow)
{
	return CGDisplayNoErr == GetDisplayDPI( CGDisplayCurrentMode(kCGDirectMainDisplay), kCGDirectMainDisplay, x, y );
}


# include	<IOKit/graphics/IOGraphicsLib.h>

//    Handy utility function for retrieving an int from a CFDictionaryRef
static int GetIntFromDictionaryForKey( CFDictionaryRef desc, CFStringRef key )
{
    CFNumberRef value;
    int num = 0;
    if ( (value = reinterpret_cast <CFNumberRef> (CFDictionaryGetValue(desc, key))) == NULL
            || CFGetTypeID(value) != CFNumberGetTypeID())
        return 0;
    CFNumberGetValue(value, kCFNumberIntType, &num);
    return num;
}

static CGDisplayErr GetDisplayDPI(
    CFDictionaryRef displayModeDict,
    CGDirectDisplayID displayID,
    float &horizontalDPI, float &verticalDPI )
{
    CGDisplayErr err = kCGErrorFailure;
    io_connect_t displayPort;
    CFDictionaryRef displayDict;

    //    Grab a connection to IOKit for the requested display
    displayPort = CGDisplayIOServicePort( displayID );
    if ( displayPort != MACH_PORT_NULL )
    {
        //    Find out what IOKit knows about this display
        displayDict = IODisplayCreateInfoDictionary(displayPort, 0);
        if ( displayDict != NULL )
        {
            const double mmPerInch = 25.4;
            double horizontalSizeInInches =
                (double)GetIntFromDictionaryForKey(displayDict,
                        CFSTR(kDisplayHorizontalImageSize)) / mmPerInch;
            double verticalSizeInInches =
                (double)GetIntFromDictionaryForKey(displayDict,
                        CFSTR(kDisplayVerticalImageSize)) / mmPerInch;

            //    Make sure to release the dictionary we got from IOKit
            CFRelease(displayDict);

            // Now we can calculate the actual DPI
            // with information from the displayModeDict
            horizontalDPI =
                (double)GetIntFromDictionaryForKey( displayModeDict, kCGDisplayWidth )
                    / horizontalSizeInInches;
            verticalDPI = (double)GetIntFromDictionaryForKey( displayModeDict,
                    kCGDisplayHeight ) / verticalSizeInInches;
            err = CGDisplayNoErr;
        }
    }
    return err;
}



double
RWMacPageComposer::MeasureWord (const RWString inText, int inTextLength, RWStyle *inStyle, double &outAscent, double &outDescent, double &outLeading)
{
	return 0;
}

void
RWMacPageComposer::DrawWord (const RWString inText, int inTextLength, float inX, float inBaseLine, RWStyle *inStyle)
{
	if (mPageIsOpen)
	{
/*
		CGContextRef	cg = GetGContext();
		Font		*font = MapStyle (inStyle);
		FontFamily	ff;
		font->GetFamily (&ff);
		REAL		fontSize = font->GetSize();
		INT			fontStyle = font->GetStyle();
		REAL		emHeight = ff.GetEmHeight (fontStyle);
		REAL		ascent = fontSize / emHeight * ff.GetCellAscent (fontStyle);
		Color		color (inStyle->GetTextColor());
		SolidBrush	solidBrush (color);
		RectF		r (inX, inBaseLine - ascent, 0, 0);
		mGraphics->DrawString ((const wchar_t*) inText, inTextLength, font, r, NULL, &solidBrush);
		ReleaseGContext();
 */
	}
}
