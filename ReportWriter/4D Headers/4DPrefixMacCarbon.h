# define	_4D_Package_					1
# define	TARGET_CARBON					1
# define	TARGET_API_MAC_OS8				0
# define	TARGET_API_MAC_CARBON			1
# define	OPAQUE_TOOLBOX_STRUCTS			1
# define	OPAQUE_UPP_TYPES				1
# define	ACCESSOR_CALLS_ARE_FUNCTIONS	1
# define	CALL_NOT_IN_CARBON				0

# define	PP_Target_Classic				!TARGET_CARBON
# define	PP_Target_Carbon				TARGET_CARBON

# define	PP_Uses_PowerPlant_Namespace	1
# define	PP_Obsolete_ThrowExceptionCode	1
# define	PP_Suppress_Notes_20			1
# define	PP	PP_PowerPlant

# define	MSL_USE_PRECOMPILED_HEADERS	0
# define	USE_PRECOMPILED_MAC_HEADERS	0

#define	MACVER 1
#define	WINVER 0

#define __MACH__  1
#define __MWERKS__ 1
# include	<ansi_prefix.mach.h>
# include	<CarbonCore/ConditionalMacros.h>
# include	<CarbonCore/MacTypes.h>
# include	<CarbonCore/MacErrors.h>
# include	<CarbonCore/MacMemory.h>

#ifndef	_USE_MAC_API_
# if	_4D_Package_ || !WINVER
#  define	_USE_MAC_API_	0
# else
#  define	_USE_MAC_API_	0
# endif
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
