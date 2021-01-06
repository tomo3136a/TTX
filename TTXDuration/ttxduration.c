/*
 * Tera Term Duration Extension
 * (C) 2021 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt_res.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <shlobj.h>

#include "compat_w95.h"
#include "ttxcommon.h"

#define ORDER 6070

#define INISECTION "TTXDuration"

#define ID_MENUITEM 56070
#define ID_MENUITEM1 (ID_MENUITEM + 1)
#define ID_MENUITEM2 (ID_MENUITEM + 2)
#define ID_MENUITEM3 (ID_MENUITEM + 3)
#define ID_MENUITEM4 (ID_MENUITEM + 4)
#define ID_MENUITEM5 (ID_MENUITEM + 5)
#define ID_MENUITEM6 (ID_MENUITEM + 6)
#define ID_MENUITEM7 (ID_MENUITEM + 7)
#define ID_MENUITEM8 (ID_MENUITEM + 8)
#define ID_MENUITEM9 (ID_MENUITEM + 9)

#define IdDurationTimer 3002

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;
	BOOL skip;
	int menuoffset;

	//menu
	HMENU DurationMenu;

	//original callback
	Tconnect origPconnect;
	TCreateFile origPCreateFile;
	PReadIniFile origReadIniFile;
	PWriteIniFile origWriteIniFile;
	PParseParam origParseParam;

	//status
	BOOL enable;
	time_t base;
	time_t duration;

	//setting
	BOOL enableOnOff;
	BOOL nowTimeMode;
	BOOL resetStart;
	BOOL connectStart;
	BOOL disconnectStop;

	DWORD border[4];

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

	pvar->enable = FALSE;
	pvar->base = 0;
	pvar->duration = 0;

	pvar->enableOnOff = FALSE;
	pvar->nowTimeMode = FALSE;
	pvar->resetStart = FALSE;
	pvar->connectStart = FALSE;
	pvar->disconnectStop = FALSE;

	pvar->border[0] = 0;
	pvar->border[1] = 0;
	pvar->border[2] = 0;
	pvar->border[3] = 0;
}

///////////////////////////////////////////////////////////////

static void DrawTextToMenuBarRight(HWND hwnd, PCHAR text, int decoration)
{
	MENUBARINFO mbi;
	RECT rect;
	HDC hDC;

	mbi.cbSize = sizeof(MENUBARINFO);
	if (GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi))
	{
		hDC = GetWindowDC(hwnd);
		switch (decoration)
		{
		case 1:
			SetBkColor(hDC, RGB(255, 0, 0));
			SetTextColor(hDC, RGB(255, 255, 255));
			break;
		case 2:
			SetBkColor(hDC, RGB(255, 255, 0));
			break;
		case 3:
			SetBkColor(hDC, RGB(0, 255, 0));
			break;
		}
		GetWindowRect(hwnd, &rect);
		rect.right = mbi.rcBar.right - rect.left - 30;
		rect.left = rect.right - 50;
		rect.top = mbi.rcBar.top - rect.top + 1;
		rect.bottom = rect.top + 8;
		DrawText(hDC, "               ", 15, &rect, DT_NOCLIP | DT_RIGHT);
		if (text)
			DrawText(hDC, text, strlen(text), &rect, DT_NOCLIP | DT_RIGHT);
		ReleaseDC(hwnd, hDC);
	}
}

static void CALLBACK DurationTimerProc(HWND hwnd, UINT msg, UINT_PTR ev, DWORD now)
{
	time_t duration;
	time_t border;
	CHAR buf[16];
	int h, m, s;
	int decoration;
	int i;

	duration = time(NULL) + pvar->duration - pvar->base;

	border = 0;
	decoration = 0;
	for (i = 0; i < sizeof(pvar->border) / sizeof(pvar->border[0]); i++)
	{
		if ((border >= pvar->border[i]) || (pvar->border[i] > duration))
			continue;
		border = pvar->border[i];
		decoration = i + 1;
	}

	if (pvar->nowTimeMode)
	{
		struct tm t;
		duration -= pvar->duration - pvar->base;
		localtime_s(&t, &duration);
		strftime(buf, sizeof(buf), "%r", &t);
	}
	else
	{
		s = duration % 60;
		m = ((duration - s) / 60) % 60;
		h = (((duration - s) / 60 - m) / 60) % 1000;
		if (h)
		{
			_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%3d:%02d:%02d ", h, m, s);
		}
		else if (m)
		{
			_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%4s%2d:%02d ", "", m, s);
		}
		else if (s)
		{
			_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%7s%2d ", "", s);
		}
		else
		{
			_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%9s ", "");
		}
	}

	DrawTextToMenuBarRight(hwnd, buf, decoration);

	//	SendMessage(hwnd, WM_IDLE, 0, 0);
	return;
}

#define TIMER_INTERVAL 200

static VOID TimerControl(HWND hWnd)
{
	if (pvar->enable)
	{
		time(&pvar->base);
		SetTimer(hWnd, IdDurationTimer,
				 TIMER_INTERVAL, DurationTimerProc);
	}
	else
	{
		pvar->duration += time(NULL) - pvar->base;
		KillTimer(hWnd, IdDurationTimer);
	}
}

static void ConnectAction()
{
	if (pvar->connectStart)
	{
		if (pvar->resetStart)
		{
			pvar->duration = 0;
			time(&pvar->base);
		}
		if (!pvar->enable)
		{
			pvar->enable = TRUE;
			TimerControl(pvar->cv->HWin);
		}
	}
}

static void DisconnectAction()
{
	if (pvar->disconnectStop)
	{
		if (pvar->enable)
		{
			pvar->enable = FALSE;
			TimerControl(pvar->cv->HWin);
		}
	}
}

///////////////////////////////////////////////////////////////

static int PASCAL TTXconnect(
	SOCKET s, const struct sockaddr *name, int namelen)
{
	int ret;

	ret = (pvar->origPconnect)(s, name, namelen);
	if (ret != INVALID_SOCKET)
		ConnectAction();
	return ret;
}

static void PASCAL TTXOpenTCP(TTXSockHooks *hooks)
{
	pvar->origPconnect = *hooks->Pconnect;
	*hooks->Pconnect = TTXconnect;
}

static void PASCAL TTXCloseTCP(TTXSockHooks *hooks)
{
	*hooks->Pconnect = pvar->origPconnect;
	DisconnectAction();
}

static HANDLE PASCAL TTXCreateFile(
	LPCTSTR FName, DWORD AcMode, DWORD ShMode, LPSECURITY_ATTRIBUTES SecAttr,
	DWORD CreateDisposition, DWORD FileAttr, HANDLE Template)
{
	HANDLE ret;

	ret = (pvar->origPCreateFile)(
		FName, AcMode, ShMode, SecAttr, CreateDisposition,
		FileAttr, Template);
	if (ret != INVALID_HANDLE_VALUE)
		ConnectAction();
	return ret;
}

static void PASCAL TTXOpenFile(TTXFileHooks *hooks)
{
	pvar->origPCreateFile = *hooks->PCreateFile;
	*hooks->PCreateFile = TTXCreateFile;
}

static void PASCAL TTXCloseFile(TTXFileHooks *hooks)
{
	*hooks->PCreateFile = pvar->origPCreateFile;
	DisconnectAction();
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
// {
// 	 printf("TTXGetUIHooks %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

static void PASCAL TTXReadIniFile(PCHAR fn, PTTSet ts)
{
	if (!pvar->skip)
		(pvar->origReadIniFile)(fn, ts);

	pvar->enableOnOff = GetIniOnOff(INISECTION, "EnableOnOff", FALSE, fn);
	pvar->nowTimeMode = GetIniOnOff(INISECTION, "NowTimeMode", FALSE, fn);
	pvar->resetStart = GetIniOnOff(INISECTION, "ResetStart", FALSE, fn);
	pvar->connectStart = GetIniOnOff(INISECTION, "ConnectStart", FALSE, fn);
	pvar->disconnectStop = GetIniOnOff(INISECTION, "DisconnectStop", FALSE, fn);

	pvar->border[0] = GetIniNum(INISECTION, "Border1", 30, fn);
	pvar->border[1] = GetIniNum(INISECTION, "Border2", 20, fn);
	pvar->border[2] = GetIniNum(INISECTION, "Border3", 10, fn);
	pvar->border[3] = GetIniNum(INISECTION, "Border4", 0, fn);
}

static void PASCAL TTXWriteIniFile(PCHAR fn, PTTSet ts)
{
	(pvar->origWriteIniFile)(fn, ts);

	WriteIniOnOff(INISECTION, "EnableOnOff", pvar->enableOnOff, FALSE, fn);
	WriteIniOnOff(INISECTION, "NowTimeMode", pvar->nowTimeMode, FALSE, fn);
	WriteIniOnOff(INISECTION, "ResetStart", pvar->resetStart, FALSE, fn);
	WriteIniOnOff(INISECTION, "ConnectStart", pvar->connectStart, FALSE, fn);
	WriteIniOnOff(INISECTION, "DisconnectStop", pvar->disconnectStop, FALSE, fn);

	WriteIniNum(INISECTION, "Border1", pvar->border[0], FALSE, fn);
	WriteIniNum(INISECTION, "Border2", pvar->border[1], FALSE, fn);
	WriteIniNum(INISECTION, "Border3", pvar->border[2], FALSE, fn);
	WriteIniNum(INISECTION, "Border4", pvar->border[3], FALSE, fn);
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

	pvar->origWriteIniFile = *hooks->WriteIniFile;
	*hooks->WriteIniFile = TTXWriteIniFile;

	pvar->origParseParam = *hooks->ParseParam;
	*hooks->ParseParam = TTXParseParam;
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXSetWinSize(int rows, int cols)
// {
// 	 printf("TTXSetWinSize %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

static void UpdateMenuBar(HMENU hmenu, UINT uid, BOOL enable)
{
	UINT lang;
	LPSTR s;

	lang = UILang(pvar->ts->UILanguageFile);
	pvar->menuoffset = MenuOffset(INISECTION, ID_MENUITEM, 0);

	RemoveMenu(hmenu, uid, MF_BYCOMMAND);
	if (pvar->enableOnOff)
	{
		s = (lang == 2) ? "タイマ(&T)" : "&Timer";
		AppendMenu(hmenu, MF_BYPOSITION, ID_MENUITEM + pvar->menuoffset, s);
	}
}

static void PASCAL TTXModifyMenu(HMENU menu)
{
	MENUITEMINFO mii;
	UINT flag;
	UINT lang;
	LPSTR s;

	lang = UILang(pvar->ts->UILanguageFile);

	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	pvar->DurationMenu = CreatePopupMenu();
	mii.hSubMenu = pvar->DurationMenu;
	mii.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
	mii.wID = ID_MENUITEM9 + pvar->menuoffset;
	s = (lang == 2) ? "経過時間(&D)" : "&Duration";
	mii.dwTypeData = s;
	InsertMenuItem(GetSubMenu(menu, ID_CONTROL), ID_CONTROL_MACRO, FALSE, &mii);

	flag = MF_BYCOMMAND | MF_STRING | MF_ENABLED;
	s = (lang == 2) ? "タイマ接続開始(&C)" : "&Connent start timer";
	AppendMenu(pvar->DurationMenu, flag, ID_MENUITEM1 + pvar->menuoffset, s);
	s = (lang == 2) ? "タイマ切断停止(&D)" : "&Disconnent stop timer";
	AppendMenu(pvar->DurationMenu, flag, ID_MENUITEM2 + pvar->menuoffset, s);
	s = (lang == 2) ? "タイマリセット開始(&R)" : "&Reset start timer";
	AppendMenu(pvar->DurationMenu, flag, ID_MENUITEM3 + pvar->menuoffset, s);
	s = (lang == 2) ? "時刻表示(&M)" : "Now time &mode";
	AppendMenu(pvar->DurationMenu, flag, ID_MENUITEM4 + pvar->menuoffset, s);
	s = (lang == 2) ? "開始/停止メニュー(&O)" : "&On/Off Menu";
	AppendMenu(pvar->DurationMenu, flag, ID_MENUITEM5 + pvar->menuoffset, s);
	AppendMenu(pvar->DurationMenu, MF_SEPARATOR, 0, NULL);
	s = (lang == 2) ? "タイマクリア(&C)" : "&Clear timer";
	AppendMenu(pvar->DurationMenu, flag, ID_MENUITEM6 + pvar->menuoffset, s);

	UpdateMenuBar(menu, ID_MENUITEM + pvar->menuoffset, pvar->enableOnOff);
}

static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
	UINT flag;

	if (menu == pvar->DurationMenu)
	{
		flag = (pvar->connectStart) ? MF_CHECKED : 0;
		CheckMenuItem(menu, ID_MENUITEM1 + pvar->menuoffset, MF_BYCOMMAND | MF_ENABLED | flag);
		flag = (pvar->disconnectStop) ? MF_CHECKED : 0;
		CheckMenuItem(menu, ID_MENUITEM2 + pvar->menuoffset, MF_BYCOMMAND | MF_ENABLED | flag);
		flag = (pvar->resetStart) ? MF_CHECKED : 0;
		CheckMenuItem(menu, ID_MENUITEM3 + pvar->menuoffset, MF_BYCOMMAND | MF_ENABLED | flag);
		flag = (pvar->nowTimeMode) ? MF_CHECKED : 0;
		CheckMenuItem(menu, ID_MENUITEM4 + pvar->menuoffset, MF_BYCOMMAND | MF_ENABLED | flag);
		flag = (pvar->enableOnOff) ? MF_CHECKED : 0;
		CheckMenuItem(menu, ID_MENUITEM5 + pvar->menuoffset, MF_BYCOMMAND | MF_ENABLED | flag);
	}
}

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (cmd + pvar->menuoffset)
	{
	case ID_MENUITEM:
		pvar->enable = !pvar->enable;
		if (pvar->resetStart)
		{
			pvar->duration = 0;
			time(&pvar->base);
		}
		TimerControl(hWin);
		return 1;

	case ID_MENUITEM1:
		pvar->connectStart = !pvar->connectStart;
		return 1;

	case ID_MENUITEM2:
		pvar->disconnectStop = !pvar->disconnectStop;
		return 1;

	case ID_MENUITEM3:
		pvar->resetStart = !pvar->resetStart;
		return 1;

	case ID_MENUITEM4:
		pvar->nowTimeMode = !pvar->nowTimeMode;
		return 1;

	case ID_MENUITEM5:
		pvar->enableOnOff = !pvar->enableOnOff;
		UpdateMenuBar(GetMenu(hWin), ID_MENUITEM + pvar->menuoffset, pvar->enableOnOff);
		DrawMenuBar(hWin);
		return 1;

	case ID_MENUITEM6:
		pvar->enable = FALSE;
		TimerControl(hWin);
		pvar->nowTimeMode = FALSE;
		pvar->resetStart = FALSE;
		pvar->connectStart = FALSE;
		pvar->disconnectStop = FALSE;
		pvar->base = 0;
		pvar->duration = 0;
		DrawTextToMenuBarRight(hWin, NULL, 0);
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
	TTXModifyPopupMenu,
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
