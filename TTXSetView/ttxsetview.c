/*
 * Tera Term Set view Extension
 * (C) 2022 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt_res.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <time.h>
//#include <commctrl.h>
#include <tchar.h>

#include "ttxcommon.h"
#include "ttxversion.h"
//#include "stringlist.h"
#include "resource.h"

#define ORDER 6068

#define INISECTION "TTXSetView"

#define ID_MENUITEM 56068

static HANDLE hInst; /* Instance handle of TTX*.DLL */
static HWND view_dialog = NULL;

typedef struct
{
	PTTSet ts;
	PComVar cv;
	// BOOL skip;

	//menu
	HMENU HelpMenu;

	//callback
	PReadIniFile origReadIniFile;
	PWriteIniFile origWriteIniFile;
	PParseParam origParseParam;

	//report view
	BOOL ChangeView;
	POINT view_win_size;
	int view_win_pos;

} TInstVar;

static TInstVar *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	pvar->ts = ts;
	pvar->cv = cv;
	// pvar->skip = FALSE;

	pvar->ChangeView = FALSE;
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

///////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////

// static void PASCAL TTXSetWinSize(int rows, int cols)
// {
// 	printf("TTXSetWinSize %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

// static void PASCAL TTXReadIniFile(TT_LPCTSTR fn, PTTSet ts)
// {
// }

// static void PASCAL TTXWriteIniFile(TT_LPCTSTR fn, PTTSet ts)
// {
// }

// static void PASCAL TTXParseParam(TT_LPTSTR Param, PTTSet ts, PCHAR DDETopic)
// {
// 	size_t buf_sz;
// 	LPTSTR buf;
// 	LPTSTR next;

// 	(pvar->origParseParam)(Param, ts, DDETopic);

// 	buf_sz = _tcsnlen(Param, _TRUNCATE);
// 	buf = (LPTSTR)malloc(buf_sz*sizeof(TCHAR));
// 	next = Param;
// 	while (next = TTXGetParam(buf, buf_sz, next))
// 	{
// 		if (_tcsnicmp(buf, _T("/F="), 3) == 0)
// 		{
// 			pvar->skip = TRUE;
// 			TTXReadIniFile(&buf[3], ts);
// 			pvar->skip = FALSE;
// 			break;
// 		}
// 	}
// 	free(buf);
// }

// static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks)
// {
// 	pvar->origReadIniFile = *hooks->ReadIniFile;
// 	*hooks->ReadIniFile = TTXReadIniFile;

// 	pvar->origWriteIniFile = *hooks->WriteIniFile;
// 	*hooks->WriteIniFile = TTXWriteIniFile;

// 	pvar->origParseParam = *hooks->ParseParam;
// 	*hooks->ParseParam = TTXParseParam;
// }

///////////////////////////////////////////////////////////////
//表示領域

//LPCTSTR type_name[] = {_T("eterm_lookfeel_t"),_T("cygterm_t"),_T("tttset"),_T("TGetHNRec"),_T("TComVar")};
LPCTSTR type_name[] = {_T("tttset"),_T("TComVar")};

void InitView(HWND hWnd, LPCTSTR fn)
{
	HWND hCombo;
	//LPTSTR p;
	int i;
	UINT lang;
	//HFILE hfile;

	lang = UILang(pvar->ts->UILanguageFile);
	//userkeytype = (lang == 2) ? userkeytype_ja : userkeytype_en;

	hCombo = GetDlgItem(hWnd, IDC_COMBO_TYPE);
	for (i = 0; i < sizeof(type_name)/sizeof(type_name[0]); i++)
	{
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)type_name[i]);
	}

	hCombo = GetDlgItem(hWnd, IDC_COMBO_VER);

	// hfile = CreateFile("release.lst", );
	// ReadFile()
	// GetIniStrSet(_T("menu"), &(pvar->menuset), 64, 64, fn);
	// if (pvar->menuset)
	// {
	// 	p = StrSetTok(pvar->menuset, &ctx);
	// 	while (p)
	// 	{
	// 		i = (int)(_tcschr(p, _T('=')) - p);
	// 		_tcsncpy_s(name, sizeof(name)/sizeof(name[0]), p, i);
	// 		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)name);
	// 		p = StrSetTok(NULL, &ctx);
	// 	}
	// }
}

// void SetInfoMsg(HWND hWnd, LPCTSTR szText, DWORD dwSize)
// {
// 	WINDOWINFO wi;
// 	RECT rcMsg;
// 	RECT rcLog;
// 	LONG x0, y0, x, y, w, h, dy;

// 	GetWindowInfo(hWnd, &wi);
// 	x0 = wi.rcClient.left;
// 	y0 = wi.rcClient.top;

// 	HWND hTitle = GetDlgItem(hWnd, IDC_MSG);
// 	GetClientRect(hTitle, &rcMsg);
// 	h = rcMsg.bottom;

// 	HFONT hFont, hFontOld;
// 	hFont = (HFONT)SendMessage(hTitle, WM_GETFONT, 0, 0);
// 	PAINTSTRUCT ps;
// 	HDC hDC = BeginPaint(hTitle, &ps);
// 	hFontOld = SelectObject(hDC, hFont);
// 	dy = DrawText(hDC, szText, dwSize, &rcMsg,
// 				   DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_EXPANDTABS | DT_CALCRECT);
// 	SelectObject(hDC, hFontOld);
// 	EndPaint(hTitle, &ps);

// 	GetWindowRect(hTitle, &rcMsg);
// 	x = rcMsg.left - x0;
// 	y = rcMsg.top - y0;
// 	w = rcMsg.right - rcMsg.left;
// 	h = rcMsg.bottom - rcMsg.top - h + dy;
// 	MoveWindow(hTitle, x, y, w, h, TRUE);

// 	SetDlgItemText(hWnd, IDC_MSG, szText);
// 	dy = h - rcMsg.bottom + rcMsg.top;

// 	HWND hLog = GetDlgItem(view_dialog, IDC_LOG);
// 	GetWindowRect(hLog, &rcLog);
// 	x = rcLog.left - x0;
// 	y = rcLog.top - y0 + dy;
// 	w = rcLog.right - rcLog.left;
// 	h = rcLog.bottom - rcLog.top - dy;
// 	MoveWindow(hLog, x, y, w, h, TRUE);
// }

// VOID UpdateReportMsg(HWND hWnd)
// {
// 	TCHAR buf[1024];
// 	size_t path_sz;
// 	LPTSTR path;
// 	DWORD dwSize;
// 	HANDLE hFile;
// 	LPTSTR p;
// 	size_t sz;

// 	memset(buf, 0, 1024*sizeof(TCHAR));
// 	if (pvar->report_title[0])
// 	{
// 		_tcsncpy_s(buf, 1024, pvar->report_title, _TRUNCATE);
// 		_tcsncat_s(buf, 1024, _T("\r\n"), _TRUNCATE);
// 	}

// 	p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
// 	path_sz = _tcsnlen(pvar->report_note_path, _TRUNCATE) + _tcsnlen(p, _TRUNCATE) + 2;
// 	path = (LPTSTR)malloc(path_sz*sizeof(TCHAR));
// 	GetAbsolutePath(path, path_sz, pvar->report_note_path, p);
// 	TTXFree(&p);
// 	hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
// 	free(path);
// 	if (hFile != INVALID_HANDLE_VALUE)
// 	{
// 		sz = _tcsnlen(buf, _TRUNCATE);
// 		if (ReadFile(hFile, buf + sz, 1024 - sz, &dwSize, NULL))
// 			buf[sz + dwSize - 1] = _T('\0');
// 		CloseHandle(hFile);
// 	}

// 	if (pvar->ChangeView)
// 		SetInfoMsg(view_dialog, buf, dwSize);
// }


///////////////////////////////////////////////////////////////
//View ダイアログ

static LRESULT CALLBACK view_dlg_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static POINT ptListView, ptBtnOk, ptBtnUpdate;
	static UINT ucnt = 0;
	TCHAR path[MAX_PATH];

	switch (msg)
	{
	case WM_INITDIALOG:
		GetPointRB(hWnd, IDC_LISTVIEW, &ptListView);
		GetPointRB(hWnd, IDOK, &ptBtnUpdate);
		GetPointRB(hWnd, IDOK, &ptBtnOk);
		pvar->ChangeView = TRUE;

		memset(path, 0, sizeof(path));
		GetModuleFileName(NULL, path, sizeof(path)/sizeof(path[0]) - 1);
    	RemoveFileName(path);
		CombinePath(path, sizeof(path)/sizeof(path[0]), _T("data"));
		InitView(hWnd, path);
		//LoadInfoTest();
		////SetWindowSize(hWnd, &(pvar->view_win_size));
		SetFocus(GetDlgItem(hWnd, IDC_LISTVIEW));
		return TRUE;

	case WM_SIZE:
		MovePointRB(hWnd, IDC_LISTVIEW, &ptListView, RB_RIGHT | RB_BOTTOM);
		MovePointRB(hWnd, IDOK, &ptBtnUpdate, RB_LEFT | RB_RIGHT);
		MovePointRB(hWnd, IDOK, &ptBtnOk, RB_LEFT | RB_RIGHT | RB_TOP | RB_BOTTOM);
		GetWindowSize(hWnd, &(pvar->view_win_size));
		//UpdateReportMsg(hWnd);
		return TRUE;

	case WM_ACTIVATE:
		if (LOWORD(wp))
			SetFocus(GetParent(hWnd));
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wp))
		{
		case IDC_UPDATE:
			return TRUE;
		case IDOK:
			pvar->ChangeView = FALSE;
			EndDialog(hWnd, 0);
			return TRUE;

		case IDCANCEL:
			pvar->ChangeView = FALSE;
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		pvar->ChangeView = FALSE;
		EndDialog(hWnd, 0);
		return TRUE;

	case WM_DESTROY:
		pvar->ChangeView = FALSE;
		//DeleteObject(hFont);
		return TRUE;
	}
	return FALSE;
}

static BOOL OpenView(HWND hWin)
{
	if (!pvar->ChangeView)
	{
		if (view_dialog)
		{
			DestroyWindow(view_dialog);
			view_dialog = 0;
		}
		view_dialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_VIEW_DIALOG),
									 hWin, (DLGPROC)view_dlg_proc);
		SetHomePosition(view_dialog, hWin, pvar->view_win_pos);
		//UpdateReportMsg(view_dialog);
		SetFocus(GetParent(view_dialog));
	}
	return TRUE;
}
static BOOL CloseView()
{
	if (pvar->ChangeView)
	{
		PostMessage(view_dialog, WM_CLOSE, 0, MAKELPARAM(0, 0));
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////

//static void PASCAL TTXSetWinSize(int rows, int cols) {
//  printf("TTXSetWinSize %d\n", ORDER);
//}

///////////////////////////////////////////////////////////////

static void PASCAL TTXModifyMenu(HMENU menu)
{
	UINT lang;
	LPTSTR s;
	int idx;

	lang = UILang(pvar->ts->UILanguageFile);

	idx = GetMenuItemCount(menu) - 1;
	pvar->HelpMenu = GetSubMenu(menu, idx);

	s = (lang == 2) ? _T("Set 一覧(&L)") : _T("TTX Set View");
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
	case ID_MENUITEM: //view report
		if (pvar->ChangeView)
		{
			CloseView();
		}
		else
		{
			OpenView(hWin);
		}
		return 1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////

//static void PASCAL TTXEnd(void) {
//  printf("TTXEnd %d\n", ORDER);
//}

//static void PASCAL TTXSetCommandLine(PCHAR cmd, int cmdlen, PGetHNRec rec) {
//  printf("TTXSetCommandLine %d\n", ORDER);
//}

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
	NULL, //TTXEnd,
	NULL, //TTXSetCommandLine,
	NULL, //TTXOpenFile,
	NULL, //TTXCloseFile,
};

BOOL __declspec(dllexport) PASCAL TTXBind(WORD Version, TTXExports *exports)
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
