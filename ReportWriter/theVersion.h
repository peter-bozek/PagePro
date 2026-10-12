// Version
#ifndef	TARGET_DEBUG
#define	TARGET_DEBUG	0
#endif


#define kVersionValue1			2	// major
#define kVersionValue2			0	// minor
#define kVersionValue3			0	// fix
#define kVersionValue4			1	// build

#if	TARGET_DEBUG
#define kIsDevRelease			1
#define	TARGET_STR				"(Debug)"
#define kVersionStage			3	// dev (0), alpha (1), beta (2), release (3)
//#undef	kVersionValue4
//#define kVersionValue4		1	// build
#define kVersion				1.7.1
#define kVersionString			"1.7.1"
#else
#define kIsDevRelease			0
#define kVersionStage			2	// dev (0), alpha (1), beta (2), release (3)
#define kVersion				2.0.0
#define kVersionString			"2.0.0"
#endif

#ifndef	RC_INVOKED
#define kProductCopyright		©2009-2026 INFORCE Bratislava spol. s r. o.
#endif
#define kProductCopyrightString	"©2009-2026 INFORCE Bratislava spol. s r. o."
#define kProductVersion			2.0.0
#define kProductVersionString	"2.0.0"
#define kProductName			Report Writer for 4D
#define kProductNameString		"Report Writer for 4D"
#define	kProductWebString		"http://www.pagepro4d.eu"
#define kDLLName				"ReportWriter.4DX"

// mac only
#define kVersionLang			0	// english-us
#define kVersion1				Part of PagePro
#define kVersion1String			"Part of PagePro"
#define kVersionStr1Line2		kProductCopyrightString
#define kVersionStr2			kProductWebString

// MachO only
#define	kBundleName				kProductName
#define	kBundleExecutable		kProductName
#define	kBundleIdentifier		sk.inforce.RW.plugin4D

#define	kBundleNameString		kProductNameString
#define	kBundleIdentifierString	"sk.inforce.RW.plugin4D"
