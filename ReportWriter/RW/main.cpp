

#if	WIN32
# include	<Windows.h>
#endif

# include	"RWDemoDataSource.h"
# include	"RWReportWriter.h"
# include	"RWReportData.h"


void	RW_RunInMainThread (RW_CBFunction inFunction, void *inData)
{
	inFunction (inData);
}


#define	LOOP_DIR		0

#define	HAS_PDF			!__MACH__
#define	HAS_QD			0
#define	HAS_CG			1
#define	HAS_CT			1
#define	HAS_WIN			WINVER

#if	HAS_PDF
# include	"RWPoDoFoPageComposer.h"
#endif

#if	TARGET_OS_MAC
# include	"RWMacCGPageComposer.h"
# include	"RWCTPageComposer.h"
inline		RWPageComposer*	NewPageComposer (unsigned long inFlags, const char *inDst)
{
	if ((inFlags & 0x200) == 0 && (void*) CTFrameGetLineOrigins != 0)
		return new RWCTPageComposer (inFlags, inDst);
	return new RWMacCGPageComposer (inFlags, inDst);
}

#else
# include	"RWWinPageComposer.h"
inline		RWPageComposer*	NewPageComposer (unsigned long inFlags, const char *inDst)
{
	return new RWWinPageComposer (inFlags, inDst, L"");
}
#endif

# include	<stdio.h>

#if	!defined (_STLP_STD) && defined (__DEBUGMEM_H) && ! __MACH__
# include	<ansi_files.h>		// __close_all
#endif

# include	<unistd.h>
# include	<dirent.h>

#if	TARGET_OS_MAC
short		gMyResFile = -1;
#endif

// forward declarations

# if	HAS_PDF
void	ReportToPDFFile (RWDataSource& inDataSource, TiXmlDocument *inXML, const char *inFileName);
# endif
void	ReportToPrinter (RWDataSource& inDataSource, TiXmlDocument *inXML, unsigned long inFlags, const char *inFileName);






# if	HAS_PDF
void	ReportToPDFFile (RWDataSource& inDataSource, TiXmlDocument *inXML, const char *inFileName)
{
	try
	{
		RWPoDoFoPageComposer		composer (RWPageComposer::eDestinationPreview, inFileName, "");	// must be destroyed AFTER RWReportData (picture objects)
		RWReportData				reportData (inXML);
		RWReportWriter				reporter (inDataSource, reportData, composer);
		reporter.Report (NULL);
	}
	catch (long l)
	{
		printf ("\n••• Exception in ReportToPDFFile: %ld\n", l);
		fflush (stdout);
	}
	catch (...)
	{
		printf ("\n••• Unknown exception in ReportToPDFFile!\n");
		fflush (stdout);
	}

	return;
}

#endif


#if	HAS_QD
void	ReportToQDPrinter (RWDataSource& inDataSource, TiXmlDocument *inXML, unsigned long inFlags, const char *inFileName)
{
	try
	{
        UString printer("");
        UString fileName (inFileName);
		RWMacQDPageComposer		composer (inFlags, fileName, printer);
		RWReportData			reportData (inXML);
		RWReportWriter			reporter (inDataSource, reportData, composer);
		reporter.Report (NULL);
	}
	catch (long l)
	{
		printf ("\n••• Exception in ReportToQDPrinter: %ld\n", l);
		fflush (stdout);
	}
	catch (...)
	{
		printf ("\n••• Unknown exception in ReportToQDPrinter!\n");
		fflush (stdout);
	}

	return;
}
#endif


void	ReportToPrinter (RWDataSource& inDataSource, TiXmlDocument *inXML, unsigned long inFlags, const char *inFileName)
{
	try
	{
		auto_ptr <RWPageComposer>	composer (NewPageComposer (inFlags, inFileName));
		{
			RWReportData			reportData (inXML);
			RWReportWriter			reporter (inDataSource, reportData, *composer);
			reporter.Report (NULL);
		}
	}
	catch (long l)
	{
		printf ("\n••• Exception in ReportToPrinter: %ld\n", l);
		fflush (stdout);
	}
	catch (...)
	{
		printf ("\n••• Unknown exception in ReportToPrinter!\n");
		fflush (stdout);
	}
	return;
}


static	void	WriteBlobToDisk (void *inBlob, size_t inSize, const char *inFileName)
{
	if (inBlob)
	{
		if (inSize > 0)
		{
			FILE	*f = fopen (inFileName, "wb");
			if (f)
			{
				fwrite (inBlob, 1, inSize, f);
				fclose (f);
			}
		}
		free (inBlob);
	}

	return;
}


static	void	ProcessReportXml (void)
{
	printf ("\nProcessing 'report.xml'\n");
	fflush (stdout);
	TiXmlDocument	xml ("report.xml");
	bool loadOkay = xml.LoadFile();

	if (not loadOkay)
	{
		printf ("Could not load RW test file 'report.xml'. Error='%s'.\n", xml.ErrorDesc());
		fflush (stdout);
		return;
	}

	RWDemoDataSource			demoDataSource;
	RWDemoDataSourceProvider	dynamicDataSource;
	RWDataSource	*dataSource = &demoDataSource;

	{
		TiXmlNode		*node = xml.RootElement();	// should be same as xml.FirstChild ("Report");
		TiXmlElement	*elem;
		ConstCXMLText	value;

		if (node)
		{
			elem = node->ToElement();
			if (elem && (value = elem->Attribute ("Dynamic")) != NULL && atol (value) != 0)
				dataSource = &dynamicDataSource;
		}
	}

# if	HAS_TEXT
//	size_t		size = 0;
//	void	*	blob = NULL;
	printf ("\nReportToTextFile\n");
	fflush (stdout);
	ReportToTextFile (*dataSource, &xml, "output.txt");
	printf ("Processed 'report.xml' into 'output.txt'\n");
	fflush (stdout);
/*
	blob = ReportToTextBlob (*dataSource, &xml, size);
	if (blob)
		WriteBlobToDisk (blob, size, "blob.txt");
	blob = NULL;
*/
# endif

# if	HAS_PDF
	printf ("\nReportToPDFFile\n");
	ReportToPDFFile (*dataSource, &xml, "output.pdf");
	printf ("Processed 'report.xml' into 'output.pdf'\n");
	fflush (stdout);
/*
	blob = ReportToPDFBlob (*dataSource, &xml, size);
	if (blob)
		WriteBlobToDisk (blob, size, "blob.pdf");
	blob = NULL;
*/
#endif

	printf ("\nReportToPrinter\n");
	fflush (stdout);
#if	TARGET_OS_MAC && __MACH__
# if	HAS_QD
	ReportToQDPrinter (*dataSource, &xml,
		RWPageComposer::eDestinationFile | RWPageComposer::eAskPageSetup | RWPageComposer::eAskJobSetup,
		"output.QD.pdf");
	printf ("\nProcessed 'report.xml' into 'output.QD.pdf'\n");
	fflush (stdout);
# endif
	if ((void*) CTFrameGetLineOrigins == 0)
	{
		ReportToPrinter (*dataSource, &xml,
						 RWPageComposer::eDestinationFile | RWPageComposer::eAskPageSetup | RWPageComposer::eAskJobSetup,
						 "output.CG.pdf");
		printf ("\nProcessed 'report.xml' into 'output.CG.pdf'\n");
		fflush (stdout);
	}
	else
	{
		ReportToPrinter (*dataSource, &xml,
						 0x200 | RWPageComposer::eDestinationFile | RWPageComposer::eAskPageSetup | RWPageComposer::eAskJobSetup,
						 "output.CG.pdf");
		printf ("\nProcessed 'report.xml' into 'output.CG.pdf'\n");
		fflush (stdout);
		ReportToPrinter (*dataSource, &xml,
						 RWPageComposer::eDestinationFile | RWPageComposer::eAskPageSetup | RWPageComposer::eAskJobSetup,
						 "output.CT.pdf");
		printf ("\nProcessed 'report.xml' into 'output.CT.pdf'\n");
		fflush (stdout);
	}
#else
	ReportToPrinter (*dataSource, &xml,
		RWPageComposer::eDestinationFile | RWPageComposer::eAskPageSetup | RWPageComposer::eAskJobSetup,
		"output.printer.pdf");
	printf ("\nProcessed 'report.xml' into 'output.printer.pdf'\n");
	fflush (stdout);
#endif

	return;
}


static	void	ProcessOneReport (char *inFileName, int inFlags)
{
	printf ("\nProcessing '%s'\n", inFileName);
	fflush (stdout);

	TiXmlDocument	xml (inFileName);
	bool loadOkay = xml.LoadFile();

	if (not loadOkay)
	{
		printf ("Could not load RW file '%s'. Error='%s'.\n", inFileName, xml.ErrorDesc());
		fflush (stdout);
		return;
	}

	RWDemoDataSource			demoDataSource;
	RWDemoDataSourceProvider	dynamicDataSource;
	RWDataSource	*dataSource = &demoDataSource;

	{
		TiXmlNode		*node = xml.RootElement();	// should be same as xml.FirstChild ("Report");
		TiXmlElement	*elem;
		ConstCXMLText	value;

		if (node)
		{
			elem = node->ToElement();
			if (elem && (value = elem->Attribute ("Dynamic")) != NULL && atol (value) != 0)
				dataSource = &dynamicDataSource;
		}
	}
	char	pdfName [1024];
	strcpy (pdfName, inFileName);
#if	TARGET_OS_MAC && __MACH__
# if	HAS_QD
	strcpy (pdfName + strlen (pdfName) - 7, ".QD.pdf");
	ReportToQDPrinter (*dataSource, &xml, RWPageComposer::eDestinationFile | inFlags, pdfName);
	printf ("Processed '%s' into '%s'\n", inFileName, pdfName);
	fflush (stdout);
# endif
	if ((void*) CTFrameGetLineOrigins == 0)
	{
		strcpy (pdfName + strlen (pdfName) - 7, ".CG.pdf");
		ReportToPrinter (*dataSource, &xml, RWPageComposer::eDestinationFile | inFlags, pdfName);
		printf ("Processed '%s' into '%s'\n", inFileName, pdfName);
		fflush (stdout);
	}
	else
	{
		strcpy (pdfName + strlen (pdfName) - 7, ".CG.pdf");
		ReportToPrinter (*dataSource, &xml, 0x200 | RWPageComposer::eDestinationFile | inFlags, pdfName);
		printf ("Processed '%s' into '%s'\n", inFileName, pdfName);
		fflush (stdout);
		strcpy (pdfName + strlen (pdfName) - 7, ".CT.pdf");
		ReportToPrinter (*dataSource, &xml, RWPageComposer::eDestinationFile | inFlags, pdfName);
		printf ("Processed '%s' into '%s'\n", inFileName, pdfName);
		fflush (stdout);
	}

#else
	strcpy (pdfName + strlen (pdfName) - 4, ".pdf");
	ReportToPrinter (*dataSource, &xml, RWPageComposer::eDestinationFile | inFlags, pdfName);
	printf ("Processed '%s' into '%s'\n", inFileName, pdfName);
	fflush (stdout);
#endif

	return;
}


extern	"C"
int
main (int argc, const char * argv[])
{
	int	result = 0;

#if	TARGET_OS_MAC
	gMyResFile = CurResFile();

#if	1
#if	__MACH__ && !_MSL_USING_MW_C_HEADERS
	CHDIR1
	CHDIR2
	CHDIR3
#else
	CHDIR1h
	CHDIR2h
	CHDIR3h

	{
		FSRef		theRef;
		CFURLRef	theFrameworkURL;
		CFURLRef	theBundleURL;
		CFBundleRef theBundle;

		/* Find the folder containing all the frameworks */
		result = FSFindFolder (kOnAppropriateDisk, kFrameworksFolderType, false, &theRef);

		if (result == noErr)
		{
			/* Turn the framework folder FSRef into a CFURL */
			theFrameworkURL = CFURLCreateFromFSRef (kCFAllocatorSystemDefault, &theRef);

			if (theFrameworkURL != NULL)
			{
				/* Create a CFURL pointing to the desired framework */
				theBundleURL = CFURLCreateCopyAppendingPathComponent (kCFAllocatorSystemDefault,
						theFrameworkURL, CFSTR("System.framework"), false);

				CFRelease (theFrameworkURL);

				if (theBundleURL != NULL)
				{
					/* Turn the CFURL into a bundle reference */
					theBundle = CFBundleCreate (kCFAllocatorSystemDefault, theBundleURL);

					CFRelease (theBundleURL);

					if (theBundle != NULL)
					{
						if (CFBundleLoadExecutable (theBundle))
						{
							typedef int (*Chdir)(const char *);
							Chdir	chdirP = (Chdir) CFBundleGetFunctionPointerForName (theBundle, CFSTR("chdir"));
							if (chdirP != NULL)
							{
								# define	chdir(x)	chdirP(x)
								CHDIR1
								CHDIR2
								CHDIR3
								#undef	chdir
							}
							CFBundleUnloadExecutable (theBundle);
						}
						CFRelease (theBundle);
					}
				}
			}
		}
		result = 0;
	}
#endif
#endif
#endif

#ifdef	__DEBUGMEM_H

	// DebugMemTurnOptionsOn (dgbmemOptDontFreeBlocks);
	DebugMemForgetLeaks();
	DebugMemReadLeaks ("rw_app.log");
#endif

	RWTools_TestBase64();

#if	LOOP_DIR
	int	flags = RWPageComposer::eNoProgress;	// RWPageComposer::eAskPageSetup | RWPageComposer::eAskJobSetup;
#endif
	TiXmlBase::SetEncodeAscii (false);		// we use UTF8, so no &xC8 for multibyte chars...
	TiXmlBase::SetCondenseWhiteSpace (false);

	try
	{
		ProcessReportXml();
	}
	catch (long l)
	{
		printf ("\n••• Exception in ProcessReportXml: %ld\n", l);
		fflush (stdout);
		result = 1;
	}
	catch (...)
	{
		printf ("\n••• Uncaught exception in ProcessReportXml!\n");
		fflush (stdout);
		result = 1;
	}

#if	LOOP_DIR
	DIR	*d = opendir (".");
	if (d)
	{
		for ( ; ; )
		{
			struct dirent	*e = readdir (d);
			if (e == NULL)
				break;
/*
			if (strcmp (e->d_name, ".") == 0)
				continue;
			if (strcmp (e->d_name, "..") == 0)
				continue;
*/
			size_t	l = strlen (e->d_name);
			if (l > 7 && strcmp (e->d_name + l - 7, ".rw.xml") == 0)
			{
				try
				{
					ProcessOneReport (e->d_name, flags);
				}
				catch (long l)
				{
					printf ("\n••• Exception in main: %ld\n", l);
					fflush (stdout);
					result = 1;
				}
				catch (...)
				{
					printf ("\n••• Unknown exception in main!\n");
					fflush (stdout);
					result = 1;
				}
			}
		}
		closedir (d);
	}
#endif

#ifdef	__DEBUGMEM_H
# if	!(defined (_STLP_STD))
	# if !__MACH__ || _MSL_USING_MW_C_HEADERS
		__close_all();
	# endif
# endif

#if TARGET_OS_MAC
	UTEC::Free();
#endif
	size_t	leaks = DebugMemReportLeaks ("rw_app.log");
	if (leaks != 0)
	{
		printf ("\n••• Leaked memory: %ld bytes\n", leaks);
		fflush (stdout);
	}
#endif

	printf ("\nDONE.\n");
	fflush (stdout);
	return result;
}
