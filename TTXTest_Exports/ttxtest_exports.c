/*
 * Tera Term Test Exports
 * (C) 2021 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "ttcommon.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

#include "ttxcommon.h"
#include "ttxversion.h"

#define ORDER 6020

#define INISECTION "TTXTest_Exports"

#define ID_MENUITEM 5800

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct {
	PTTSet ts;
	PComVar cv;

	PSetupTerminal SetupTerminal;
	PSetupWin SetupWin;
	PSetupKeyboard SetupKeyboard;
	PSetupSerialPort SetupSerialPort;
	PSetupTCPIP SetupTCPIP;
	PGetHostName GetHostName;
	PChangeDirectory ChangeDirectory;
	PAboutDialog AboutDialog;
	PChooseFontDlg ChooseFontDlg;
	PSetupGeneral SetupGeneral;
	PWindowWindow WindowWindow;
	PTTDLGSetUILanguageFile TTDLGSetUILanguageFile;

	PReadIniFile ReadIniFile;
	PWriteIniFile WriteIniFile;
	PReadKeyboardCnf ReadKeyboardCnf;
	PCopyHostList CopyHostList;
	PAddHostToList AddHostToList;
	PParseParam ParseParam;

	Tclosesocket Pclosesocket;
	Tconnect Pconnect;
	Thtonl Phtonl;
	Thtons Phtons;
	Tinet_addr Pinet_addr;
	Tioctlsocket Pioctlsocket;
	Trecv Precv;
	Tselect Pselect;
	Tsend Psend;
	Tsetsockopt Psetsockopt;
	Tsocket Psocket;
	TWSAAsyncSelect PWSAAsyncSelect;
	TWSAAsyncGetHostByName PWSAAsyncGetHostByName;
	TWSACancelAsyncRequest PWSACancelAsyncRequest;
	TWSAGetLastError PWSAGetLastError;
	Tfreeaddrinfo Pfreeaddrinfo;
	TWSAAsyncGetAddrInfo PWSAAsyncGetAddrInfo;

	TCreateFile PCreateFile;
	TCloseFile PCloseFile;
	TReadFile PReadFile;
	TWriteFile PWriteFile;

} TInstVar;

static TInstVar FAR *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

#define DBG_VIEW(fn,fmt,...) {\
	TCHAR title[256];\
	TCHAR buf[256];\
	DWORD dw = (DWORD)(pvar->cv->HWin);\
	_sntprintf_s(title, 256, 256, fn _T("() %d"), tt_version);\
	_sntprintf_s(buf, 256, 256, _T("cv->HWin=%d\n\n") fmt, dw, __VA_ARGS__);\
	MessageBox(0, buf, title, MB_OK | MB_ICONINFORMATION);\
}

#ifdef TT4
#define __FUNCTIONT__ __FUNCTION__
#else /* TT4 */
#define __FUNCTIONT__ __FUNCTIONW__
#endif /* TT4 */

// static void WinListView()
// {
// 	TCHAR msg[1024];
// 	TCHAR buf[256];
// 	DWORD dw1;
// 	DWORD dw2;

// 	dw1 = (DWORD)(pvar->cv->HWin);
// 	int n = GetRegisteredWindowCount();
// 	_sntprintf_s(msg, 1024, 1024, _T("WindowList: cv->HWin=%d\n"), dw1);

// 	for (int i = 0; i < n; i ++)
// 	{
// 		HWND hwnd = GetNthWin(i);
// 		dw2 = (DWORD)(hwnd);
// 		_sntprintf_s(buf, 256, 256, _T("    [%d]=%d\n"), i, dw2);
// 		_tcscat_s(msg, 1024, buf);

// 	}
// 	MessageBox(0, msg, _T("test plugin"), MB_OK | MB_ICONINFORMATION);
// }

// void WINAPI NotifyMessage(PComVar cv, const char *msg, const char *title, DWORD flag)
// {
// 	wchar_t *titleW = ToWcharA(title);
// 	wchar_t *msgW = ToWcharA(msg);
// 	NotifyMessageW(cv, msgW, titleW, flag);
// 	free(titleW);
// 	free(msgW);
// }


///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	pvar->ts = ts;
	pvar->cv = cv;
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T(""));
}

///////////////////////////////////////////////////////////////

static BOOL PASCAL TTXSetupTerminal(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\nts=%p (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupTerminal(WndParent, ts);
}

static BOOL PASCAL TTXSetupWin(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\nts=%p (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupWin(WndParent, ts);
}

static BOOL PASCAL TTXSetupKeyboard(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\nts=%p (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupKeyboard(WndParent, ts);
}

static BOOL PASCAL TTXSetupSerialPort(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\nts=%p (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupSerialPort(WndParent, ts);
}

static BOOL PASCAL TTXSetupTCPIP(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\nts=%p (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupTCPIP(WndParent, ts);
}

static BOOL PASCAL TTXGetHostName(HWND WndParent, PGetHNRec GetHNRec)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\nGetHNRec=%p"), WndParent, GetHNRec);
	return pvar->GetHostName(WndParent, GetHNRec);
}

static BOOL PASCAL TTXChangeDirectory(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\nts=%p (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->ChangeDirectory(WndParent, ts);
}

static BOOL PASCAL TTXAboutDialog(HWND WndParent)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p"), WndParent);
	return pvar->AboutDialog(WndParent);
}

static BOOL PASCAL TTXChooseFontDlg(HWND WndParent, LPLOGFONT LogFont, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\nLogFont=%p\nts=%p (pvar->ts=%p)"), WndParent, LogFont, ts, pvar->ts);
	return pvar->ChooseFontDlg(WndParent, LogFont, ts);
}

static BOOL PASCAL TTXSetupGeneral(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\nts=%p (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupGeneral(WndParent, ts);
}

static BOOL PASCAL TTXWindowWindow(HWND WndParent, PBOOL Close)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\nClose=%p"), WndParent, Close);
	return pvar->WindowWindow(WndParent, Close);
}

static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
{
	//DBG_VIEW(__FUNCTIONT__, _T("%s"), _T(""));
	pvar->SetupTerminal = *hooks->SetupTerminal;
	pvar->SetupWin = *hooks->SetupWin;
	pvar->SetupKeyboard = *hooks->SetupKeyboard;
	pvar->SetupSerialPort = *hooks->SetupSerialPort;
	pvar->SetupTCPIP = *hooks->SetupTCPIP;
	pvar->GetHostName = *hooks->GetHostName;
	pvar->ChangeDirectory = *hooks->ChangeDirectory;
	pvar->AboutDialog = *hooks->AboutDialog;
	pvar->ChooseFontDlg = *hooks->ChooseFontDlg;
	pvar->SetupGeneral = *hooks->SetupGeneral;
	pvar->WindowWindow = *hooks->WindowWindow;

	*hooks->SetupTerminal = TTXSetupTerminal;
	*hooks->SetupWin = TTXSetupWin;
	*hooks->SetupKeyboard = TTXSetupKeyboard;
	*hooks->SetupSerialPort = TTXSetupSerialPort;
	*hooks->SetupTCPIP = TTXSetupTCPIP;
	*hooks->GetHostName = TTXGetHostName;
	*hooks->ChangeDirectory = TTXChangeDirectory;
	*hooks->AboutDialog = TTXAboutDialog;
	*hooks->ChooseFontDlg = TTXChooseFontDlg;
	*hooks->SetupGeneral = TTXSetupGeneral;
	*hooks->WindowWindow = TTXWindowWindow;
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXReadIniFile(TT_LPCTSTR FName, PTTSet ts)
{
	BEGIN_TTX_STR(FName);
	DBG_VIEW(__FUNCTIONT__, _T("FName=%s\nts=%p (pvar->ts=%p)"), FNameW, ts, pvar->ts);
	END_TTX_STR(FName);
	pvar->ReadIniFile(FName, ts);
}

static void PASCAL TTXWriteIniFile(TT_LPCTSTR FName, PTTSet ts)
{
	BEGIN_TTX_STR(FName);
	DBG_VIEW(__FUNCTIONT__, _T("FName=%s\nts=%p (pvar->ts=%p)"), FNameW, ts, pvar->ts);
	END_TTX_STR(FName);
	pvar->WriteIniFile(FName, ts);
}

static void PASCAL TTXReadKeyboardCnf(TT_LPCTSTR FName, PKeyMap KeyMap, BOOL ShowWarning)
{
	BEGIN_TTX_STR(FName);
	DBG_VIEW(__FUNCTIONT__, _T("FName=%s\nKepMap=%p\nShowWarning=%d"), FNameW, KeyMap, ShowWarning);
	END_TTX_STR(FName);
	pvar->ReadKeyboardCnf(FName, KeyMap, ShowWarning);
}

static void PASCAL TTXCopyHostList(TT_LPCTSTR IniSrc, TT_LPCTSTR IniDest)
{
	BEGIN_TTX_STR2(IniSrc, IniDest);
	DBG_VIEW(__FUNCTIONT__, _T("src=%s dst=%s"), IniSrcW, IniDestW);
	END_TTX_STR2(IniSrc, IniDest);
	pvar->CopyHostList(IniSrc, IniDest);
}

static void PASCAL TTXAddHostToList(TT_LPCTSTR FName, TT_LPCTSTR Host)
{
	BEGIN_TTX_STR2(FName, Host);
	DBG_VIEW(__FUNCTIONT__, _T("FName=%s\nHost=%s"), FNameW, HostW);
	END_TTX_STR2(FName, Host);
	pvar->AddHostToList(FName, Host);
}

static void PASCAL TTXParseParam(TT_LPTSTR Param, PTTSet ts, PCHAR DDETopic)
{
	BEGIN_TTX_STR(Param);
	DBG_VIEW(__FUNCTIONT__, _T("param=%s\nts=%p (pvar->ts=%p)"), ParamW, ts, pvar->ts);
	END_TTX_STR(Param);
	pvar->ParseParam(Param, ts, DDETopic);
}

static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks)
{
	pvar->AddHostToList = *hooks->AddHostToList;
	pvar->CopyHostList = *hooks->CopyHostList;
	pvar->ParseParam = *hooks->ParseParam;
	pvar->ReadIniFile = *hooks->ReadIniFile;
	pvar->ReadKeyboardCnf = *hooks->ReadKeyboardCnf;
	pvar->WriteIniFile = *hooks->WriteIniFile;

	*hooks->AddHostToList = TTXAddHostToList;
	*hooks->CopyHostList = TTXCopyHostList;
	*hooks->ParseParam = TTXParseParam;
	*hooks->ReadIniFile = TTXReadIniFile;
	*hooks->ReadKeyboardCnf = TTXReadKeyboardCnf;
	*hooks->WriteIniFile = TTXWriteIniFile;
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXOpenTCP(TTXSockHooks *hooks)
{
	// printf("TTXOpenTCP %d\n", ORDER);
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T("hooks"));
	
	// Tclosesocket Pclosesocket;
	// Tconnect Pconnect;
	// Thtonl Phtonl;
	// Thtons Phtons;
	// Tinet_addr Pinet_addr;
	// Tioctlsocket Pioctlsocket;
	// Trecv Precv;
	// Tselect Pselect;
	// Tsend Psend;
	// Tsetsockopt Psetsockopt;
	// Tsocket Psocket;
	// TWSAAsyncSelect PWSAAsyncSelect;
	// TWSAAsyncGetHostByName PWSAAsyncGetHostByName;
	// TWSACancelAsyncRequest PWSACancelAsyncRequest;
	// TWSAGetLastError PWSAGetLastError;
	// Tfreeaddrinfo Pfreeaddrinfo;
	// TWSAAsyncGetAddrInfo PWSAAsyncGetAddrInfo;
}

static void PASCAL TTXCloseTCP(TTXSockHooks *hooks)
{
	// printf("TTXCloseTCP %d\n", ORDER);
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T("hooks"));
}

static BOOL PASCAL TTXTReadFile(HANDLE FHandle, LPVOID Buff, DWORD ReadSize, LPDWORD ReadBytes, LPOVERLAPPED ReadOverLap)
{
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T("hooks"));
   return FALSE;
}

static BOOL PASCAL TTXTWriteFile(HANDLE FHandle, LPCVOID Buff, DWORD WriteSize, LPDWORD WriteBytes, LPOVERLAPPED WriteOverLap)
{
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T("hooks"));
    return FALSE;
}

static HANDLE PASCAL TTXTCreateFile(LPCTSTR FName, DWORD AcMode, DWORD ShMode, LPSECURITY_ATTRIBUTES SecAttr, DWORD CreateDisposition, DWORD FileAttr, HANDLE Template)
{
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T("hooks"));
 	return 0;
}

static BOOL PASCAL TTXTCloseFile(HANDLE FHandle)
{
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T("hooks"));
 	return FALSE;
}

static void PASCAL TTXOpenFile(TTXFileHooks *hooks)
{
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T("hooks"));
	if (HAS_TTXHOOK(TTXFileHooks)) {
		pvar->PCreateFile = *hooks->PCreateFile;
		pvar->PCloseFile = *hooks->PCloseFile;
		pvar->PReadFile = *hooks->PReadFile;
		pvar->PWriteFile = *hooks->PWriteFile;

		*hooks->PCreateFile = TTXTCreateFile;
		*hooks->PCloseFile = TTXTCloseFile;
		*hooks->PReadFile = TTXTReadFile;
		*hooks->PWriteFile = TTXTWriteFile;
	}
}

static void PASCAL TTXCloseFile(TTXFileHooks *hooks)
{
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T("hooks"));
	if (HAS_TTXHOOK(TTXFileHooks)) {
		*hooks->PCreateFile = pvar->PCreateFile;
		*hooks->PCloseFile = pvar->PCloseFile;
		*hooks->PReadFile = pvar->PReadFile;
		*hooks->PWriteFile = pvar->PWriteFile;
	}
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXSetWinSize(int rows, int cols)
{
	DBG_VIEW(__FUNCTIONT__, _T("r=%d, c=%d"), rows, cols);
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXModifyMenu(HMENU menu)
{
	//DBG_VIEW(__FUNCTIONT__, _T("menu=%p"), menu);
	HMENU hmenu = GetSubMenu(menu, ID_HELPMENU);
	AppendMenu(hmenu, MF_ENABLED, ID_MENUITEM, _T("&test"));
}

static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
	//DBG_VIEW(__FUNCTIONT__, _T("menu=%s"), menu);
}

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p, cmd=%d"), hWin, cmd);
	return 0;
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXEnd(void)
{
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T(""));
}

static void PASCAL TTXSetCommandLine(TT_LPTSTR cmd, int cmdlen, PGetHNRec rec)
{
	BEGIN_TTX_STR(cmd);
	DBG_VIEW(__FUNCTIONT__, _T("cmd=%s, len=%d\nGetHNRec=%p"), cmdW, cmdlen, rec);
	END_TTX_STR(cmd);
}


///////////////////////////////////////////////////////////////

static TTXExports Exports = {
	/* This must contain the size of the structure. See below for its usage. */
	sizeof(TTXExports),

	ORDER,

	/* Now we just list the functions that we've implemented. */
	TTXInit,
	TTXGetUIHooks,
	TTXGetSetupHooks,
	TTXOpenTCP,
	TTXCloseTCP,
	TTXSetWinSize,
	TTXModifyMenu,
	TTXModifyPopupMenu,
	TTXProcessCommand,
	TTXEnd,
	TTXSetCommandLine,
	TTXOpenFile,
	TTXCloseFile,
};

BOOL __declspec(dllexport) PASCAL FAR TTXBind(WORD Version, TTXExports *exports)
{
	int size = sizeof(Exports) - sizeof(exports->size);
	/* do version checking if necessary */
	/* if (Version!=TTVERSION) return FALSE; */

	if (TTXIgnore(ORDER, _T(INISECTION), 0))
		return TRUE;
	
	TTXInitVersion(0);

	if (size > exports->size) {
		size = exports->size;
	}
	memcpy((char *)exports + sizeof(exports->size), (char *)&Exports + sizeof(exports->size), size);
	return TRUE;
}

BOOL WINAPI DllMain(HANDLE hInstance, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
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
