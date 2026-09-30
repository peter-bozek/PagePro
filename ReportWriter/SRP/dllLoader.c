#include <Windows.h>

BOOL __stdcall DllMain (HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved);
BOOL	LoadLibs (void);
void __stdcall	FourDPackex( long selector, void* params, void** data, void* result );
typedef void __stdcall	(*FourDPackexCB) ( long selector, void* params, void** data, void* result );

HINSTANCE		gMyInstance = 0;
HMODULE			gDll = 0;
FourDPackexCB	gDllMain = 0;

# define	kUseLocal	1

#if	!kUseLocal
static	wchar_t	*path = L"C:\\Test\\RW\\Debug\\RW.dll";
#endif


BOOL __stdcall DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
#pragma unused (lpvReserved)

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			gMyInstance = hInst;
			return 1;
			
		case DLL_PROCESS_DETACH:
			return 0;

	  	default:
			return 1;
   	}
}


BOOL	LoadLibs (void)
{
	if (gDllMain != 0)
		return TRUE;

#if kUseLocal
	{
		char	fullPath[_MAX_PATH];
		char	drive [4];
		char	path [_MAX_PATH];

		fullPath[0] = 0;
		GetModuleFileNameA ((HMODULE) gMyInstance, fullPath, sizeof (fullPath));
		splitpath (fullPath, drive, path, NULL, NULL);
		makepath (fullPath, drive, path, "RW", ".DLL");

		gDll = LoadLibraryA( fullPath );
	}
#else
		gDll = LoadLibraryW (path);
#endif

	if (gDll == 0)
	{
		DWORD	err = GetLastError();
		wchar_t	buf [512];
		FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, buf, sizeof (buf), NULL);
		MessageBoxW (NULL, buf, L"dllLoader", MB_OK | MB_ICONSTOP | MB_APPLMODAL);
		return FALSE;
	}

	gDllMain = (FourDPackexCB) GetProcAddress ( gDll, "_FourDPackex@16" );
	if (gDllMain == 0)
	{
		FreeLibrary (gDll);
		gDll = 0;
		return FALSE;
	}

	return TRUE;
}


void __stdcall	FourDPackex( long selector, void* params, void** data, void* result )
{
	if (gDllMain == 0 && selector != -2 && selector != -220)
	{
		if (! LoadLibs())
			return;
	}

	if (gDllMain != 0)
	{
		gDllMain (selector, params, data, result);
	}

	if (gDll != 0 && (selector == -2 || selector == -220))
	{
		FreeLibrary (gDll);
		gDll = 0;
		gDllMain = 0;
	}
}
