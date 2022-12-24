/*
 * Tera Term Plugin Extension
 * (C) 2021 tomo3136a
 * TTX(Tera Term Extension) の一覧を表示する。
 * また、機能の有効・無効を切り替えを行う。
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

#include "ttxcommon.h"

#include "pluginlistdlg.h"

#define ORDER 6001

#define INISECTION "TTXPlugin"

#define ID_MENUITEM 56001

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;

	//menu
	HMENU HelpMenu;

	PTSTR SetupFName;

} TInstVar;

static TInstVar FAR *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static VOID SaveIniSects(LPCTSTR fn)
{
	LPCTSTR on_plugins[] = { _T(INISECTION), _T("ttxssh"), _T("TTXProxy") };
	LPCTSTR sect = on_plugins[0];
	TCHAR buf[16];
	UINT i;
	if (GetPrivateProfileSection(sect, buf, _countof(buf), fn) == 0)
	{
		for (i = 0; i < _countof(on_plugins); i ++)
			WritePrivateProfileString(sect, on_plugins[i], _T("-"), fn);
	}
}

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	//DWORD flag;

	pvar->ts = ts;
	pvar->cv = cv;

	pvar->SetupFName = TTXGetPath(ts, ID_SETUPFNAME);
	//flag = GetPrivateProfileInt(_T("TTX"), _T("flag"), 0, pvar->SetupFName);
	//if (~flag & 0x0008)
	//	SaveIniSects(pvar->SetupFName);
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
// {
// 	 printf("TTXGetUIHooks %d\n", ORDER);
// }

// static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks)
// {
// 	printf("TTXGetSetupHooks %d\n", ORDER);
// }

// static void PASCAL TTXOpenTCP(TTXSockHooks *hooks)
// {
// 	 printf("TTXOpenTCP %d\n", ORDER);
// }

// static void PASCAL TTXCloseTCP(TTXSockHooks *hooks)
// {
// 	 printf("TTXCloseTCP %d\n", ORDER);
// }

// static void PASCAL TTXOpenFile(TTXFileHooks *hooks)
// {
//   printf("TTXOpenFile %d\n", ORDER);
// }

// static void PASCAL TTXCloseFile(TTXFileHooks *hooks)
// {
//   printf("TTXCloseFile %d\n", ORDER);
// }

// static void PASCAL TTXSetWinSize(int rows, int cols)
// {
// 	printf("TTXSetWinSize %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

static void PASCAL TTXModifyMenu(HMENU menu)
{
	UINT lang;
	LPTSTR s;
	int idx;

	lang = UILang(pvar->ts->UILanguageFile);

	idx = GetMenuItemCount(menu) - 1;
	pvar->HelpMenu = GetSubMenu(menu, idx);

	s = (lang == 2) ? _T("TTX プラグイン一覧(&L)") : _T("TTX plugin &list");
	InsertMenu(pvar->HelpMenu, 2, MF_BYPOSITION, TTXMenuID(ID_MENUITEM), s);
}

// static void PASCAL TTXModifyPopupMenu(HMENU menu)
// {
// 	printf("TTXModifyPopupMenu %d\n", ORDER);
// }

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (TTXMenuOrgID(cmd))
	{
	case ID_MENUITEM:
		switch (PluginListDialog(hInst, hWin, pvar->ts))
		{
		case IDOK:
			MessageBox(hWin, _T("Please exit Tera Term."), _T("Tera Term"),
					   MB_OK | MB_ICONINFORMATION);
			break;
		case IDCANCEL:
			break;
		case -1:
			MessageBox(hWin, _T("Error"), _T("Can't display dialog box."),
					   MB_OK | MB_ICONEXCLAMATION);
			break;
		}
		return 1;
	}

	return 0;
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXEnd(void)
{
	//  printf("TTXEnd %d\n", ORDER);
	TTXFree(&pvar->SetupFName);
}

// static void PASCAL TTXSetCommandLine(TT_LPTSTR cmd, int cmdlen, PGetHNRec rec)
// {
// 	 printf("TTXSetCommandLine %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

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
	TTXEnd,
	NULL, //TTXSetCommandLine,
	NULL, //TTXOpenFile,
	NULL, //TTXCloseFile,
};

BOOL __declspec(dllexport) PASCAL FAR TTXBind(WORD Version, TTXExports *exports)
{
	int size = sizeof(Exports) - sizeof(exports->size);
	/* do version checking if necessary */
	/* if (Version!=TTVERSION) return FALSE; */

	//TTXIgnore(ORDER, _T(INISECTION), Version);
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
