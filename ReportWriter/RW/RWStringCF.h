/*
 *  RWStringCF.h
 *  ReportWriter
 *
 *  RWString <-> CoreFoundation (Mac only). CFString stores UTF-16 like
 *  RWString, so these are copies, no conversion.
 */

#ifndef	_RWStringCF_h_
# define	_RWStringCF_h_

# include	"RWString.h"
# include	<CoreFoundation/CoreFoundation.h>

static_assert (sizeof (UniChar) == sizeof (char16_t), "UniChar must be a 16 bit code unit");

namespace	RWStr
{
	// caller releases the result
	inline	CFStringRef		CreateCFString (RWStringView inText)
	{
		return CFStringCreateWithCharacters (kCFAllocatorDefault, reinterpret_cast <const UniChar*> (inText.data()), CFIndex (inText.size()));
	}

	inline	RWString		FromCFString (CFStringRef inText)
	{
		if (inText == NULL)
			return RWString();
		RWString	result (size_t (CFStringGetLength (inText)), u'\0');
		CFStringGetCharacters (inText, CFRangeMake (0, CFIndex (result.size())), reinterpret_cast <UniChar*> (&result[0]));
		return result;
	}

	// POSIX path for a path from 4D: 4D on the Mac uses HFS paths ("Disk:Folder:file"),
	// converted like the original UString::GetFSName did. A path starting with '/' is
	// taken as POSIX already.
	inline	RWString		POSIXPathFromHFS (RWStringView inPath)
	{
		if (inPath.empty() || inPath[0] == u'/')
			return RWString (inPath);

		RWString	result;
		CFStringRef	path = CreateCFString (inPath);
		if (path)
		{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
			CFURLRef	url = CFURLCreateWithFileSystemPath (kCFAllocatorDefault, path, kCFURLHFSPathStyle, false);
#pragma clang diagnostic pop
			CFRelease (path);
			if (url)
			{
				UInt8	cpath [4096];
				if (CFURLGetFileSystemRepresentation (url, false, cpath, sizeof (cpath)))
					result = FromUTF8 (reinterpret_cast <const char*> (cpath));
				CFRelease (url);
			}
		}
		return result;
	}
}

#endif
