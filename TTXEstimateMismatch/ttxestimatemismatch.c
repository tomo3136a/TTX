/*
 * Tera Term Estimate of tttset structure mismatch Extension
 * (C) 2020 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt-version.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

#include "ttxcommon.h"

#if ((TT_VERSION_MAJOR * 10000 + TT_VERSION_MINOR) < 40105)
#error "not support build version"
#endif

#define ORDER 6000

#define INISECTION "TTXEstimateMismatch"

#define ID_MENUITEM 56000

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;

	//menu
	HMENU HelpMenu;

	//original callback
	PReadIniFile origReadIniFile;

	//status
	WORD tt_version;

} TInstVar;

static TInstVar FAR *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	WORD TTRunningVersion();

	pvar->ts = ts;
	pvar->cv = cv;

	pvar->tt_version = TTRunningVersion();
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXOpenTCP(TTXSockHooks *hooks)
// {
// 	 printf("TTXOpenTCP %d\n", ORDER);
// }

// static void PASCAL TTXCloseTCP(TTXSockHooks *hooks)
// {
// 	 printf("TTXCloseTCP %d\n", ORDER);
// }

// static void PASCAL TTXOpenFile(TTXFileHooks *hooks) {
//   printf("TTXOpenFile %d\n", ORDER);
// }

// static void PASCAL TTXCloseFile(TTXFileHooks *hooks) {
//   printf("TTXCloseFile %d\n", ORDER);
// }

// static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
// {
// 	 printf("TTXGetUIHooks %d\n", ORDER);
// }

// static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks)
// {
// 	printf("TTXGetSetupHooks %d\n", ORDER);
// }

// static void PASCAL TTXSetWinSize(int rows, int cols) {
//	printf("TTXSetWinSize %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

void DisplayTTTSet(HWND hWnd, PTTSet ts, WORD tt_version);

static void PASCAL TTXModifyMenu(HMENU menu)
{
	LPTSTR s;
	int idx;
	
	idx = GetMenuItemCount(menu) - 1;
	pvar->HelpMenu = GetSubMenu(menu, idx);
	s = _T("Estimate of structure mismatch in TTSet...");
	InsertMenu(pvar->HelpMenu, 2, MF_BYPOSITION, TTXMenuID(ID_MENUITEM), s);
}

// static void PASCAL TTXModifyPopupMenu(HMENU menu)
// {
// 	 printf("TTXModifyPopupMenu %d\n", ORDER);
// }

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (TTXMenuOrgID(cmd))
	{
	case ID_MENUITEM:
		DisplayTTTSet(hWin, pvar->ts, pvar->tt_version);
		return 1;
	}

	return 0;
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXEnd(void)
// {
// 	 printf("TTXEnd %d\n", ORDER);
// }

// static void PASCAL TTXSetCommandLine(PCHAR cmd, int cmdlen, PGetHNRec rec)
// {
// 	 printf("TTXSetCommandLine %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

// same function with ttset::RunningVersion
static WORD TTRunningVersion()
{
//#pragma comment(lib, "version.lib")
	TCHAR szPath[MAX_PATH];
	DWORD dwSize;
	DWORD dwHandle;
	LPVOID lpBuf;
	UINT uLen;
	VS_FIXEDFILEINFO *pFileInfo;
	int major, minor;

	GetModuleFileName(NULL, szPath, sizeof(szPath)/sizeof(szPath[0]) - 1);

	dwSize = GetFileVersionInfoSize(szPath, &dwHandle);
	if (dwSize == 0)
	{
		return 0;
	}

	lpBuf = malloc(dwSize*sizeof(TCHAR));
	if (!GetFileVersionInfo(szPath, dwHandle, dwSize, lpBuf))
	{
		free(lpBuf);
		return 0;
	}

	if (!VerQueryValue(lpBuf, _T("\\"), (LPVOID *)&pFileInfo, &uLen))
	{
		free(lpBuf);
		return 0;
	}

	major = HIWORD(pFileInfo->dwFileVersionMS);
	minor = LOWORD(pFileInfo->dwFileVersionMS);

	free(lpBuf);

	return major * 10000 + minor;
}

static TTXExports Exports = {
	/* This must contain the size of the structure. See below for its usage. */
	sizeof(TTXExports),

	ORDER,

	/* Now we just list the functions that we've implemented. */
	TTXInit,
	NULL, //TTXGetUIHooks,
	NULL, //TTXGetSetupHooks,
	NULL, //TTXOpenTCP,
	NULL, //TTXCloseTCP,
	NULL, //TTXSetWinSize,
	TTXModifyMenu,
	NULL, //TTXModifyPopupMenu,
	TTXProcessCommand,
	NULL, //TTXEnd,
	NULL, //TTXSetCommandLine,
	NULL, //TTXOpenFile,
	NULL, //TTXCloseFile,
};

BOOL __declspec(dllexport) PASCAL FAR TTXBind(WORD Version, TTXExports *exports)
{
	int size = sizeof(Exports) - sizeof(exports->size);
	/* do version checking if necessary */
	/* if (Version!=TTVERSION) return FALSE; */

	if (TTXIgnore(ORDER, _T(INISECTION), Version))
		return TRUE;

	if (size > exports->size)
	{
		size = exports->size;
	}
	memcpy((char *)exports + sizeof(exports->size),
		   (char *)&Exports + sizeof(exports->size),
		   size);
	return TRUE;
}

BOOL WINAPI DllMain(HANDLE hInstance,
					ULONG ul_reason_for_call,
					LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_THREAD_ATTACH:
		/* do thread initialization */
		break;
	case DLL_THREAD_DETACH:
		/* do thread cleanup */
		break;
	case DLL_PROCESS_ATTACH:
		/* do process initialization */
		hInst = hInstance;
		pvar = &InstVar;
		break;
	case DLL_PROCESS_DETACH:
		/* do process cleanup */
		break;
	}
	return TRUE;
}
