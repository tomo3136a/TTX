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
#include <tchar.h>

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

#define TIMER_INTERVAL 200
#define BORDER_NUM 4

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;
	BOOL skip;

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

	DWORD border[BORDER_NUM];

} TInstVar;

static TInstVar FAR *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	int i;

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

	for (i = 0; i < BORDER_NUM; i ++)
		pvar->border[i] = 0;
}

///////////////////////////////////////////////////////////////

static void DrawTextToMenuBarRight(HWND hwnd, LPCTSTR text, int decoration)
{
	MENUBARINFO mbi;
	RECT rect;
	HDC hDC;
	// NONCLIENTMETRICS ncm;
	// HFONT hFont, oldFont;

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
		// ncm.cbSize = sizeof(NONCLIENTMETRICS);
		// SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
		// hFont = CreateFontIndirect(&ncm.lfMenuFont);
		// oldFont = (HFONT)SelectObject(hDC, hFont);
		GetWindowRect(hwnd, &rect);
		rect.right = mbi.rcBar.right - rect.left - 30;
		rect.left = mbi.rcBar.right - rect.left - 80;
		rect.bottom = mbi.rcBar.bottom - rect.top;
		rect.top = mbi.rcBar.top - rect.top;
		DrawText(hDC, _T("               "), 15, &rect, DT_RIGHT);
		if (text)
			DrawText(hDC, text, _tcslen(text), &rect, DT_RIGHT);
		// SelectObject(hDC, oldFont);
		// DeleteObject(hFont);
		ReleaseDC(hwnd, hDC);
	}
}

static void CALLBACK DurationTimerProc(HWND hwnd, UINT msg, UINT_PTR ev, DWORD now)
{
	size_t buf_sz;
	TCHAR buf[16];
	time_t duration;
	time_t border;
	int h, m, s;
	int decoration;
	int i;

	buf_sz = sizeof(buf)/sizeof(buf[0]);
	duration = time(NULL) + pvar->duration - pvar->base;

	border = 0;
	decoration = 0;
	for (i = 0; i < BORDER_NUM; i++)
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
		_tcsftime(buf, buf_sz, _T("%r"), &t);
	}
	else
	{
		s = duration % 60;
		m = ((duration - s) / 60) % 60;
		h = (((duration - s) / 60 - m) / 60) % 1000;
		if (h)
		{
			_sntprintf_s(buf, buf_sz, _TRUNCATE, _T("%3d:%02d:%02d "), h, m, s);
		}
		else if (m)
		{
			_sntprintf_s(buf, buf_sz, _TRUNCATE, _T("%4s%2d:%02d "), _T(""), m, s);
		}
		else if (s)
		{
			_sntprintf_s(buf, buf_sz, _TRUNCATE, _T("%7s%2d "), _T(""), s);
		}
		else
		{
			_sntprintf_s(buf, buf_sz, _TRUNCATE, _T("%9s "), _T(""));
		}
	}

	DrawTextToMenuBarRight(hwnd, buf, decoration);
	return;
}

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

static void PASCAL TTXReadIniFile(TT_LPCTSTR fn, PTTSet ts)
{
	int i;
	TCHAR name[16];

	if (!pvar->skip)
		(pvar->origReadIniFile)(fn, ts);

	pvar->enableOnOff = GetIniOnOff(_T(INISECTION), _T("EnableOnOff"), FALSE, fn);
	pvar->nowTimeMode = GetIniOnOff(_T(INISECTION), _T("NowTimeMode"), FALSE, fn);
	pvar->resetStart = GetIniOnOff(_T(INISECTION), _T("ResetStart"), FALSE, fn);
	pvar->connectStart = GetIniOnOff(_T(INISECTION), _T("ConnectStart"), FALSE, fn);
	pvar->disconnectStop = GetIniOnOff(_T(INISECTION), _T("DisconnectStop"), FALSE, fn);

	for (i = 0; i < BORDER_NUM; i ++)
	{
		_sntprintf_s(name, 16, _TRUNCATE, _T("Border%d"), (i + 1));
		pvar->border[i] = GetIniNum(_T(INISECTION), name, 0, fn);
	}
}

static void PASCAL TTXWriteIniFile(TT_LPCTSTR fn, PTTSet ts)
{
	int i;
	TCHAR name[16];

	(pvar->origWriteIniFile)(fn, ts);

	WriteIniOnOff(_T(INISECTION), _T("EnableOnOff"), pvar->enableOnOff, FALSE, fn);
	WriteIniOnOff(_T(INISECTION), _T("NowTimeMode"), pvar->nowTimeMode, FALSE, fn);
	WriteIniOnOff(_T(INISECTION), _T("ResetStart"), pvar->resetStart, FALSE, fn);
	WriteIniOnOff(_T(INISECTION), _T("ConnectStart"), pvar->connectStart, FALSE, fn);
	WriteIniOnOff(_T(INISECTION), _T("DisconnectStop"), pvar->disconnectStop, FALSE, fn);

	for (i = 0; i < BORDER_NUM; i ++)
	{
		_sntprintf_s(name, 16, _TRUNCATE, _T("Border%d"), (i + 1));
		WriteIniNum(_T(INISECTION), name, pvar->border[i], FALSE, fn);
	}
}

static void PASCAL TTXParseParam(TT_LPTSTR Param, PTTSet ts, PCHAR DDETopic)
{
	size_t buf_sz;
	LPTSTR buf;
	LPTSTR next;

	(pvar->origParseParam)(Param, ts, DDETopic);

	buf_sz = _tcsnlen(Param, _TRUNCATE);
	buf = (LPTSTR)malloc(buf_sz*sizeof(TCHAR));
	next = Param;
	while (next = TTXGetParam(buf, buf_sz, next))
	{
		if (_tcsnicmp(buf, _T("/F="), 3) == 0)
		{
			pvar->skip = TRUE;
			TTXReadIniFile(&buf[3], ts);
			pvar->skip = FALSE;
			break;
		}
	}
	free(buf);
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
	LPTSTR s;

	lang = UILang(pvar->ts->UILanguageFile);

	RemoveMenu(hmenu, uid, MF_BYCOMMAND);
	if (enable)
	{
		s = (lang == 2) ? _T("タイマ(&T)") : _T("&Timer");
		AppendMenu(hmenu, MF_BYPOSITION, TTXMenuID(ID_MENUITEM), s);
	}
}

static void PASCAL TTXModifyMenu(HMENU menu)
{
	MENUITEMINFO mii;
	UINT flag;
	UINT lang;
	LPTSTR s;

	lang = UILang(pvar->ts->UILanguageFile);

	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	pvar->DurationMenu = CreatePopupMenu();
	mii.hSubMenu = pvar->DurationMenu;
	mii.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
	mii.wID = TTXMenuID(ID_MENUITEM9);
	s = (lang == 2) ? _T("経過時間(&D)") : _T("&Duration");
	mii.dwTypeData = s;
	InsertMenuItem(GetSubMenu(menu, ID_CONTROL), ID_CONTROL_MACRO, FALSE, &mii);

	flag = MF_BYCOMMAND | MF_STRING | MF_ENABLED;
	s = (lang == 2) ? _T("タイマ接続開始(&C)") : _T("&Connent start timer");
	AppendMenu(pvar->DurationMenu, flag, TTXMenuID(ID_MENUITEM1), s);
	s = (lang == 2) ? _T("タイマ切断停止(&D)") : _T("&Disconnent stop timer");
	AppendMenu(pvar->DurationMenu, flag, TTXMenuID(ID_MENUITEM2), s);
	s = (lang == 2) ? _T("タイマリセット開始(&R)") : _T("&Reset start timer");
	AppendMenu(pvar->DurationMenu, flag, TTXMenuID(ID_MENUITEM3), s);
	s = (lang == 2) ? _T("時刻表示(&M)") : _T("Now time &mode");
	AppendMenu(pvar->DurationMenu, flag, TTXMenuID(ID_MENUITEM4), s);
	s = (lang == 2) ? _T("開始/停止メニュー(&O)") : _T("&On/Off Menu");
	AppendMenu(pvar->DurationMenu, flag, TTXMenuID(ID_MENUITEM5), s);
	AppendMenu(pvar->DurationMenu, MF_SEPARATOR, 0, NULL);
	s = (lang == 2) ? _T("タイマクリア(&C)") : _T("&Clear timer");
	AppendMenu(pvar->DurationMenu, flag, TTXMenuID(ID_MENUITEM6), s);

	UpdateMenuBar(menu, TTXMenuID(ID_MENUITEM), pvar->enableOnOff);
}

static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
	UINT flag;

	if (menu == pvar->DurationMenu)
	{
		flag = (pvar->connectStart) ? MF_CHECKED : 0;
		CheckMenuItem(menu, TTXMenuID(ID_MENUITEM1), MF_BYCOMMAND | MF_ENABLED | flag);
		flag = (pvar->disconnectStop) ? MF_CHECKED : 0;
		CheckMenuItem(menu, TTXMenuID(ID_MENUITEM2), MF_BYCOMMAND | MF_ENABLED | flag);
		flag = (pvar->resetStart) ? MF_CHECKED : 0;
		CheckMenuItem(menu, TTXMenuID(ID_MENUITEM3), MF_BYCOMMAND | MF_ENABLED | flag);
		flag = (pvar->nowTimeMode) ? MF_CHECKED : 0;
		CheckMenuItem(menu, TTXMenuID(ID_MENUITEM4), MF_BYCOMMAND | MF_ENABLED | flag);
		flag = (pvar->enableOnOff) ? MF_CHECKED : 0;
		CheckMenuItem(menu, TTXMenuID(ID_MENUITEM5), MF_BYCOMMAND | MF_ENABLED | flag);
	}
}

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (TTXMenuOrgID(cmd))
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
		UpdateMenuBar(GetMenu(hWin), TTXMenuID(ID_MENUITEM), pvar->enableOnOff);
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

	if (TTXIgnore(ORDER, _T(INISECTION), 0))
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
