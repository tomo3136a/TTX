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

#include "compat_w95.h"
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
	int menuoffset;

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

#define BUF_SZ 65536
#define LINE(buf, nm, s)        \
	strcat_s(buf, BUF_SZ, nm);  \
	strcat_s(buf, BUF_SZ, "="); \
	strcat_s(buf, BUF_SZ, s);   \
	strcat_s(buf, BUF_SZ, "\r")

void DisplayTTTSet(HWND hWnd)
{
	CHAR buf[128];
	CHAR *msg;
	PTTSet ts;

	msg = malloc(BUF_SZ);
	ts = pvar->ts;
	msg[0] = 0;
	LINE(msg, "HomeDir", ts->HomeDir);
	LINE(msg, "SetupFName", ts->SetupFName);
	LINE(msg, "KeyCnfFN", ts->KeyCnfFN);
	LINE(msg, "LogFN", ts->LogFN);
	LINE(msg, "MacroFN", ts->MacroFN);
	LINE(msg, "HostName", ts->HostName);
	LINE(msg, "VTFont", ts->VTFont);
	LINE(msg, "PrnFont", ts->PrnFont);
	LINE(msg, "PrnDev", ts->PrnDev);
	LINE(msg, "FileDir", ts->FileDir);
	LINE(msg, "FileSendFilter", ts->FileSendFilter);
	LINE(msg, "DelimList", ts->DelimList);
	LINE(msg, "TEKFont", ts->TEKFont);
	LINE(msg, "Answerback", ts->Answerback);
	LINE(msg, "Title", ts->Title);
	LINE(msg, "TermType", ts->TermType);
	LINE(msg, "MouseCursorName", ts->MouseCursorName);
	LINE(msg, "CygwinDirectory", ts->CygwinDirectory);
	LINE(msg, "Locale", ts->Locale);
	LINE(msg, "ViewlogEditor", ts->ViewlogEditor);
	LINE(msg, "LogDefaultName", ts->LogDefaultName);
	LINE(msg, "LogDefaultPath", ts->LogDefaultPath);
	LINE(msg, "EtermLookfeel.BGSPIPath", ts->EtermLookfeel.BGSPIPath);
	LINE(msg, "EtermLookfeel.BGThemeFile", ts->EtermLookfeel.BGThemeFile);
	LINE(msg, "UILanguageFile", ts->UILanguageFile);
	LINE(msg, "UIMsg", ts->UIMsg);
	LINE(msg, "UILanguageFile_ini", ts->UILanguageFile_ini);
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "Build:%d.%d, Running:%d",
				TT_VERSION_MAJOR, TT_VERSION_MINOR, pvar->tt_version);
	MessageBox(hWnd, msg, buf, MB_OK);

	msg[0] = 0;
	LINE(msg, "XModemRcvCommand", ts->XModemRcvCommand);
	LINE(msg, "ZModemRcvCommand", ts->ZModemRcvCommand);
	LINE(msg, "YModemRcvCommand", ts->YModemRcvCommand);
	LINE(msg, "ConfirmChangePasteStringFile", ts->ConfirmChangePasteStringFile);
	strcat_s(msg, BUF_SZ, "---- v4.63 ----\r");
	strcat_s(msg, BUF_SZ, "---- v4.67 ----\r");
	LINE(msg, "TerminalUID", ts->TerminalUID);
	LINE(msg, "ClickableUrlBrowser", ts->ClickableUrlBrowser);
	LINE(msg, "ClickableUrlBrowserArg", ts->ClickableUrlBrowserArg);
	LINE(msg, "ScpSendDir", ts->ScpSendDir);
	LINE(msg, "BGImageFilePath", ts->BGImageFilePath);
	strcat_s(msg, BUF_SZ, "---- v4.80 ----\r");
	LINE(msg, "CygtermSettings.term", ts->CygtermSettings.term);
	LINE(msg, "CygtermSettings.term_type", ts->CygtermSettings.term_type);
	LINE(msg, "CygtermSettings.port_start", ts->CygtermSettings.port_start);
	LINE(msg, "CygtermSettings.port_range", ts->CygtermSettings.port_range);
	LINE(msg, "CygtermSettings.shell", ts->CygtermSettings.shell);
	LINE(msg, "CygtermSettings.env1", ts->CygtermSettings.env1);
	LINE(msg, "CygtermSettings.env2", ts->CygtermSettings.env2);
	LINE(msg, "LogTimestampFormat", ts->LogTimestampFormat);
	strcat_s(msg, BUF_SZ, "---- v4.96 ----\r");
	strcat_s(msg, BUF_SZ, "---- v4.98 ----\r");
	strcat_s(msg, BUF_SZ, "---- v4.100 ----\r");
	strcat_s(msg, BUF_SZ, "---- v4.102 ----\r");
	strcat_s(msg, BUF_SZ, "---- v4.104 ----\r");
	LINE(msg, "DialogFontName", ts->DialogFontName);
	strcat_s(msg, BUF_SZ, "\r");
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "ConfigVersion=%d\r", ts->ConfigVersion);
	strcat_s(msg, BUF_SZ, buf);
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "RunningVersion=%d\r", ts->RunningVersion);
	strcat_s(msg, BUF_SZ, buf);
	strcat_s(msg, BUF_SZ, "---- v4.105 ----\r");
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "Build:%d.%d, Running:%d",
				TT_VERSION_MAJOR, TT_VERSION_MINOR, pvar->tt_version);
	MessageBox(hWnd, msg, buf, MB_OK);
	free(msg);
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXModifyMenu(HMENU menu)
{
	LPSTR s;
	int idx;
	
	pvar->menuoffset = MenuOffset(INISECTION, ID_MENUITEM, 0);

	idx = GetMenuItemCount(menu) - 1;
	pvar->HelpMenu = GetSubMenu(menu, idx);
	s = "Estimate of structure mismatch in TTSet...";
	InsertMenu(pvar->HelpMenu, 2, MF_BYPOSITION, ID_MENUITEM + pvar->menuoffset, s);
}

// static void PASCAL TTXModifyPopupMenu(HMENU menu)
// {
// 	 printf("TTXModifyPopupMenu %d\n", ORDER);
// }

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (cmd)
	{
	case ID_MENUITEM:
		DisplayTTTSet(hWin);
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
	char szPath[MAX_PATH];
	DWORD dwSize;
	DWORD dwHandle;
	LPVOID lpBuf;
	UINT uLen;
	VS_FIXEDFILEINFO *pFileInfo;
	int major, minor;

	GetModuleFileName(NULL, szPath, sizeof(szPath) - 1);

	dwSize = GetFileVersionInfoSize(szPath, &dwHandle);
	if (dwSize == 0)
	{
		return 0;
	}

	lpBuf = malloc(dwSize);
	if (!GetFileVersionInfo(szPath, dwHandle, dwSize, lpBuf))
	{
		free(lpBuf);
		return 0;
	}

	if (!VerQueryValue(lpBuf, "\\", (LPVOID *)&pFileInfo, &uLen))
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

	if (TTXIgnore(ORDER, INISECTION, 0))
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
		DoCover_IsDebuggerPresent();
		hInst = hInstance;
		pvar = &InstVar;
		break;
	case DLL_PROCESS_DETACH:
		/* do process cleanup */
		break;
	}
	return TRUE;
}
