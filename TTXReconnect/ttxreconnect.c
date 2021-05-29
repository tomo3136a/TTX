/*
 * Tera Term Reconnect Extension
 * (C) 2020 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compat_w95.h"
#include "ttxcommon.h"

#define _TTX_VERSION_SUPPORT

#ifdef _TTX_VERSION_SUPPORT
#include "ttxversion.h"
#else
#define TTXInitVersion()
#define TS(a, b) (a->b)
#endif

#define ORDER 6030

#define INISECTION "TTXReconnect"

#define ID_MENUITEM 56030
#define ID_MENUITEM1 (ID_MENUITEM + 1)

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;
	BOOL skip;

	//menu
	HMENU FileMenu;

	//original callback
	PReadIniFile origReadIniFile;
	PParseParam origParseParam;

	//status
	BOOL ConnectMsg;
	BOOL Reconnect;

	//setting
	UINT ReconnectWait;

} TInstVar;

static TInstVar FAR *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	pvar->ts = ts;
	pvar->cv = cv;
	pvar->skip = FALSE;

	pvar->ConnectMsg = FALSE;
	pvar->Reconnect = FALSE;
	pvar->ReconnectWait = 0;
	TTXInitVersion();
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
// {
// 	 printf("TTXGetUIHooks %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

static void PASCAL TTXReadIniFile(PCHAR fn, PTTSet ts)
{
	char buf[16];

	if (!pvar->skip)
		(pvar->origReadIniFile)(fn, ts);

	GetPrivateProfileString(INISECTION, "ReconnectWait", "", buf, sizeof(buf), fn);
	pvar->ReconnectWait = atoi(buf);
}

static void PASCAL TTXParseParam(PCHAR Param, PTTSet ts, PCHAR DDETopic)
{
	char buf[MAX_PATH + 20];
	PCHAR next;

	(pvar->origParseParam)(Param, ts, DDETopic);

	next = Param;
	while (next = TTXGetParam(buf, sizeof(buf), next))
	{
		if (_strnicmp(buf, "/F=", 3) == 0)
		{
			pvar->skip = TRUE;
			TTXReadIniFile(&buf[3], ts);
			pvar->skip = FALSE;
			break;
		}
	}
}

static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks)
{
	pvar->origReadIniFile = *hooks->ReadIniFile;
	*hooks->ReadIniFile = TTXReadIniFile;
	pvar->origParseParam = *hooks->ParseParam;
	*hooks->ParseParam = TTXParseParam;
}

///////////////////////////////////////////////////////////////

void DisplayReconnect()
{
	CHAR buf[256];
	UINT lang;
	LPSTR s;

	lang = UILang(pvar->ts->UILanguageFile);

	buf[0] = 0;
	switch (pvar->ts->PortType)
	{
	case IdTCPIP:
		_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%s:%d", pvar->ts->HostName, pvar->ts->TCPPort);
		break;

	case IdSerial:
		_snprintf_s(buf, sizeof(buf), _TRUNCATE, "COM%d", pvar->ts->ComPort);
		break;

	case IdFile:
		_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%s", pvar->ts->HostName);
		break;

	case IdNamedPipe:
		_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%s", pvar->ts->HostName);
	}
	s = (lang == 2) ? "ÄÚ‘±" : "reconnect";
	MessageBox(pvar->cv->HWin, buf, s, MB_OK);
}

static void PASCAL TTXOpenTCP(TTXSockHooks *hooks)
{
	if (pvar->Reconnect)
	{
		pvar->Reconnect = FALSE;
		if (pvar->ConnectMsg)
		{
			DisplayReconnect();
		}
		else
		{
			Sleep(pvar->ReconnectWait);
		}
	}
}

static void PASCAL TTXCloseTCP(TTXSockHooks *hooks)
{
	if (pvar->Reconnect)
	{
		PostMessage(pvar->cv->HWin, WM_USER_COMMSTART, 0, 0);
	}
}

static void PASCAL TTXOpenFile(TTXFileHooks *hooks)
{
	if (pvar->Reconnect)
	{
		pvar->Reconnect = FALSE;
		if (pvar->ConnectMsg)
		{
			DisplayReconnect();
		}
		else
		{
			Sleep(pvar->ReconnectWait);
		}
	}
	else if (TS(pvar->ts, AutoComPortReconnect))
	{
		Sleep(pvar->ReconnectWait);
	}
}

static void PASCAL TTXCloseFile(TTXFileHooks *hooks)
{
	if (pvar->Reconnect)
	{
		PostMessage(pvar->cv->HWin, WM_USER_COMMSTART, 0, 0);
	}
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXSetWinSize(int rows, int cols)
// {
// 	 printf("TTXSetWinSize %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

static void PASCAL TTXModifyMenu(HMENU menu)
{
	UINT lang;
	LPSTR s;

	lang = UILang(pvar->ts->UILanguageFile);

	pvar->FileMenu = GetSubMenu(menu, ID_FILE);

	s = (lang == 2) ? "ÄÚ‘±(&F)..." : "reconnect...";
	InsertMenu(pvar->FileMenu, 3, MF_BYPOSITION, TTXMenuID(ID_MENUITEM), s);
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
	case ID_MENUITEM1:
		pvar->ConnectMsg = (TTXMenuOrgID(cmd) == ID_MENUITEM1) ? TRUE : FALSE;
		if (pvar->cv->Open)
		{
			pvar->Reconnect = TRUE;
			SendMessage(pvar->cv->HWin, WM_USER_COMMNOTIFY, 0, FD_CLOSE);
		}
		else
		{
			pvar->Reconnect = FALSE;
			PostMessage(pvar->cv->HWin, WM_USER_COMMSTART, 0, 0);
		}
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

static TTXExports Exports = {
	/* This must contain the size of the structure. See below for its usage. */
	sizeof(TTXExports),

	ORDER,

	/* Now we just list the functions that we've implemented. */
	TTXInit,
	NULL, //TTXGetUIHooks,
	TTXGetSetupHooks,
	TTXOpenTCP,
	TTXCloseTCP,
	NULL, //TTXSetWinSize,
	TTXModifyMenu,
	NULL, //TTXModifyPopupMenu,
	TTXProcessCommand,
	NULL, //TTXEnd,
	NULL, //TTXSetCommandLine,
	TTXOpenFile,
	TTXCloseFile,
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
