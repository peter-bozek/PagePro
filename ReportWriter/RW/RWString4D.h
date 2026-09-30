/*
 *  RWString4D.h
 *  ReportWriter
 *
 *  RWString <-> 4D plugin API. PA_Unichar and char16_t are both 16 bit
 *  UTF-16 code units, so these are copies / pointer casts, no conversion.
 */

#ifndef	_RWString4D_h_
# define	_RWString4D_h_

# include	"4DPluginAPI.h"
# include	"RWString.h"

static_assert (sizeof (PA_Unichar) == sizeof (char16_t), "PA_Unichar must be a 16 bit code unit");

namespace	RWStr
{
	inline	RWString			FromPA (const PA_Unichar *inText, size_t inLength)
	{
		return inText ? RWString (reinterpret_cast <const char16_t*> (inText), inLength) : RWString();
	}

	inline	RWString			FromPA (const PA_Unichar *inText)
	{
		return inText ? RWString (reinterpret_cast <const char16_t*> (inText)) : RWString();
	}

	inline	RWString			FromPA (PA_Unistring *inText)
	{
		return inText ? FromPA (PA_GetUnistring (inText), size_t (PA_GetUnistringLength (inText))) : RWString();
	}

	// the pointer is valid while inText lives and is not modified
	inline	PA_Unichar*			ToPA (const RWString &inText)
	{
		return const_cast <PA_Unichar*> (reinterpret_cast <const PA_Unichar*> (inText.c_str()));
	}

	// caller owns the result (PA_DisposeUnistring), unless it is handed to 4D
	inline	PA_Unistring		CreatePA (const RWString &inText)
	{
		return PA_CreateUnistring (ToPA (inText));
	}

	inline	void				SetPA (PA_Unistring *ioTarget, const RWString &inText)
	{
		PA_SetUnistring (ioTarget, ToPA (inText));
	}
}

#endif
