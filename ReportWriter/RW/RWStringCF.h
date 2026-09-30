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
}

#endif
