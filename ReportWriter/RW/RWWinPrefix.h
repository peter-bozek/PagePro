/*
 *  RWWinPrefix.h
 *  ReportWriter
 *
 *  Forced include of the Windows build (ReportWriter.vcxproj, /FI). Replaces
 *  SRP/WinPrefix.h, which needed the XF toolbox (XConfig.h) and old compiler
 *  workarounds. Defines the platform switches and the Mac type names the
 *  shared code uses (OSType, SInt32, noErr, ...).
 */

#pragma once

#ifndef	_4D_Package_
# define	_4D_Package_			1
#endif

# define	MACVER					0
# define	USE_MAC_TYPES			0
# define	TARGET_API_MAC_CARBON	0

#ifndef	NOMINMAX
# define	NOMINMAX							// std::min / std::max
#endif
#ifndef	_WIN32_WINNT
# define	_WIN32_WINNT			0x0A00		// Windows 10 (4D v20 and later)
#endif
#ifndef	WINVER
# define	WINVER					_WIN32_WINNT
#endif
#ifndef	STRICT
# define	STRICT					1
#endif
#ifndef	_CRT_SECURE_NO_WARNINGS
# define	_CRT_SECURE_NO_WARNINGS
#endif

# include	<ciso646>			// "not", "and", "or" as used in the shared code
# include	<cmath>
# include	<cstdint>
# include	<Windows.h>

// Mac type names used by the shared code
typedef unsigned char			UInt8;
typedef signed char				SInt8;
typedef unsigned short			UInt16;
typedef signed short			SInt16;
typedef unsigned long			UInt32;
typedef signed long				SInt32;
typedef signed long long		SInt64;
typedef unsigned long long		UInt64;
typedef SInt16					OSErr;
typedef SInt32					OSStatus;
typedef unsigned long			FourCharCode;
typedef FourCharCode			OSType;
typedef FourCharCode			ResType;
typedef unsigned char			Boolean;
typedef UInt32					UTF32Char;
typedef UInt16					UniChar;
typedef UInt16					UTF16Char;
typedef UInt8					UTF8Char;
typedef unsigned char *			StringPtr;
typedef const unsigned char *	ConstStr255Param;

enum
{
	noErr				= 0,
	unimpErr			= -4,
	paramErr			= -50,
	memFullErr			= -108,
	nilHandleErr		= -109,
	userCanceledErr		= -128
};

#ifdef	_MSC_VER
# pragma	warning (disable: 4068)		// unknown pragma (#pragma mark)
#endif
