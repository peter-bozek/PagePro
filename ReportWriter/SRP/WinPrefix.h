#pragma once
//# include	<Win32Headers.h>

#ifndef	_4D_Package_
# define	_4D_Package_					1
#endif

# define	USE_MAC_TYPES					0
# define	_USE_MAC_API_					0
# define	WIN32_LEAN_AND_MEAN
//# define	WIN32							0x601
# define	WINVER							0x601
# define	_WIN32_WINNT					0x601
# define	STRICT							1
# define	Compile4DLL						1

# define	MACVER							0
# define	TARGET_CARBON					0
# define	TARGET_API_MAC_OS8				0
# define	TARGET_API_MAC_CARBON			0
# define	OPAQUE_TOOLBOX_STRUCTS			0
# define	OPAQUE_UPP_TYPES				0
# define	ACCESSOR_CALLS_ARE_FUNCTIONS	0
# define	OLDROUTINENAMES					1
# define	CALL_NOT_IN_CARBON				1

// # define 	UNICODE 						1
#if	__MWERKS__
# include	<ansi_prefix.win32.h>
#endif

#if	USE_MAC_TYPES
# include	<ConditionalMacros.h>

#undef EXTERN_API
#undef EXTERN_API_C
#undef EXTERN_API_STDCALL
#undef EXTERN_API_C_STDCALL

#undef DEFINE_API
#undef DEFINE_API_C
#undef DEFINE_API_STDCALL
#undef DEFINE_API_C_STDCALL

#undef CALLBACK_API
#undef CALLBACK_API_C
#undef CALLBACK_API_STDCALL
#undef CALLBACK_API_C_STDCALL

#undef TARGET_RT_LITTLE_ENDIAN
#undef TARGET_RT_BIG_ENDIAN
#endif

#define EXTERN_API(_type)                       /*__declspec(dllimport)*/ _type __stdcall
#define EXTERN_API_C(_type)                     /*__declspec(dllimport)*/ _type __stdcall
#define EXTERN_API_STDCALL(_type)               /*__declspec(dllimport)*/ _type __stdcall
#define EXTERN_API_C_STDCALL(_type)             /*__declspec(dllimport)*/ _type __stdcall

#define DEFINE_API(_type)                       /*__declspec(dllexport)*/ _type __stdcall
#define DEFINE_API_C(_type)                     /*__declspec(dllexport)*/ _type __stdcall
#define DEFINE_API_STDCALL(_type)               /*__declspec(dllexport)*/ _type __stdcall
#define DEFINE_API_C_STDCALL(_type)             /*__declspec(dllexport)*/ _type __stdcall

#define CALLBACK_API(_type, _name)              _type (__stdcall * _name)
#define CALLBACK_API_C(_type, _name)            _type (__stdcall * _name)
#define CALLBACK_API_STDCALL(_type, _name)      _type (__stdcall * _name)
#define CALLBACK_API_C_STDCALL(_type, _name)    _type (__stdcall * _name)

#define TARGET_RT_LITTLE_ENDIAN				0
#define TARGET_RT_BIG_ENDIAN				1

#if	USE_MAC_TYPES
# include	<MacTypes.h>
# include	<MacErrors.h>
# include	<MacMemory.h>
#undef	Length

#ifndef PtoCstr
	#define PtoCstr		p2cstr
#endif

#ifndef CtoPstr
	#define CtoPstr		c2pstr
#endif

#ifndef PtoCString
	#define PtoCString	p2cstr
#endif

#ifndef CtoPString
	#define CtoPString	c2pstr
#endif

#ifndef topLeft
	#define topLeft(r)	(((Point *) &(r))[0])
#endif

#ifndef botRight
	#define botRight(r)	(((Point *) &(r))[1])
#endif

#ifndef TRUE
	#define TRUE		true
#endif

#ifndef FALSE
	#define FALSE		false
#endif

#define MacSetCursor		SetCurso
#define MacShowCursor		ShowCurso
#define MacGetCursor		GetCurso
#else
typedef unsigned char                   UInt8;
typedef signed char                     SInt8;
typedef unsigned short                  UInt16;
typedef signed short                    SInt16;
typedef unsigned long                   UInt32;
typedef signed long                     SInt32;
typedef signed long long                SInt64;
typedef unsigned long long              UInt64;
typedef SInt16                          OSErr;
typedef SInt32                          OSStatus;
typedef unsigned long                   FourCharCode;
typedef FourCharCode                    OSType;
typedef FourCharCode                    ResType;
typedef SInt16		                    ResID;
typedef unsigned char                   Boolean;
typedef UInt32                          UTF32Char;
typedef UInt16                          UniChar;
typedef UInt16                          UTF16Char;
typedef UInt8                           UTF8Char;
typedef unsigned char *                 StringPtr;
typedef StringPtr *                     StringHandle;
typedef const unsigned char *           ConstStringPtr;
typedef const unsigned char *           ConstStr255Param;
enum	{
  noErr							= 0,
  unimpErr                      = -4,
  paramErr						= -50,
  userCanceledErr               = -128,
  memFullErr                    = -108,
  nilHandleErr                  = -109
};
inline unsigned char StrLength(ConstStr255Param string) { return (*string); }
#endif

# include	<cmath>
#if	!__MACH__ && !__MWERKS__
# define	isnan(x)	_isnan(x)
# define	isinf(x)	(!_finite(x))
# define	INFINITY	_HUGE
# define	not	!
# define	snprintf	_snprintf
#pragma	warning(disable: 4068 4390 4065 4996 4305 4244)
#endif

# include	<Windows.h>

# include	"XConfig.h"
