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
#include <commctrl.h>
#include <tchar.h>

#include "ttxcommon.h"
#include "resource.h"

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

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	pvar->ts = ts;
	pvar->cv = cv;

	pvar->SetupFName = TTXGetPath(ts, ID_SETUPFNAME);
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

static void TrimVersion(LPTSTR ver)
{
	LPTSTR p, q;

	p = ver + _tcslen(ver);
	while (ver < p)
	{
		if (*p == _T('.'))
		{
			q = p + 1;
			while (*q)
			{
				if (*q != _T('0'))
					return;
				q++;
			}
			*p = 0;
		}
		p--;
	}
}

static BOOL GetModuleVersion(LPTSTR buf, int sz, LPTSTR fn)
{
	//#pragma comment(lib, "version.lib")
	DWORD dwSize;
	DWORD dwHandle;
	LPVOID lpBuf;
	UINT uLen;
	VS_FIXEDFILEINFO *pFileInfo;

	dwSize = GetFileVersionInfoSize(fn, &dwHandle);
	if (dwSize == 0)
	{
		return FALSE;
	}

	lpBuf = malloc((dwSize + 2)*sizeof(TCHAR));
	memset(lpBuf, 0, (dwSize + 2)*sizeof(TCHAR));
	if (!GetFileVersionInfo(fn, dwHandle, dwSize, lpBuf))
	{
		free(lpBuf);
		return FALSE;
	}

	if (!VerQueryValue(lpBuf, _T("\\"), (LPVOID *)&pFileInfo, &uLen))
	{
		free(lpBuf);
		return FALSE;
	}
	_sntprintf_s(buf, sz, _TRUNCATE, _T("%d.%d.%d.%d"),
				HIWORD(pFileInfo->dwFileVersionMS), 
				LOWORD(pFileInfo->dwFileVersionMS),
				HIWORD(pFileInfo->dwFileVersionLS), 
				LOWORD(pFileInfo->dwFileVersionLS));

	TrimVersion(buf);

	free(lpBuf);
	return TRUE;
}

static BOOL GetModuleDescription(LPTSTR buf, int sz, LPCTSTR fn)
{
	//#pragma comment(lib, "version.lib")
	DWORD dwSize;
	DWORD dwHandle;
	LPVOID lpBuf;
	LPTSTR lpKey;
	LPDWORD lpdwTrans;
	UINT uLen;
	LPTSTR s;
	LPTSTR p;

	dwSize = GetFileVersionInfoSize(fn, &dwHandle);
	if (dwSize == 0)
	{
		return FALSE;
	}

	lpBuf = (LPTSTR)malloc((dwSize + 2) * sizeof(TCHAR));
	if (!lpBuf)
	{
		return FALSE;
	}
	memset(lpBuf, 0, (dwSize + 2) * sizeof(TCHAR));
	if (!GetFileVersionInfo(fn, dwHandle, dwSize, lpBuf))
	{
		free(lpBuf);
		return FALSE;
	}

	lpKey = _T("\\VarFileInfo\\Translation");
	if (!VerQueryValue(lpBuf, lpKey, (LPVOID *)&lpdwTrans, &uLen))
	{
		free(lpBuf);
		return FALSE;
	}

	lpKey = malloc(sizeof(TCHAR)*128);
	if (!lpKey)
	{
		return FALSE;
	}
	s = _T("\\StringFileInfo\\%04x%04x\\FileDescription");
	_sntprintf_s(lpKey, 128, 128, s, LOWORD(*lpdwTrans), HIWORD(*lpdwTrans));
	if (!VerQueryValue(lpBuf, lpKey, (LPVOID *)&p, &uLen))
	{
		free(lpKey);
		free(lpBuf);
		return FALSE;
	}
	_tcscpy_s(buf, sz, (0 < uLen) ? p : _T(""));

	free(lpKey);
	free(lpBuf);
	return TRUE;
}

void LoadListView(HWND dlg, UINT uid, LPTSTR fn)
{
	HWND hWnd;
	LVCOLUMN lvcol;
	LVITEM item;
	WIN32_FIND_DATA win32fd;
	HANDLE hFind;
	TCHAR name[64];
	// TCHAR ent[70];
	TCHAR buf[512];
	UINT lang;
	int i;
	TCHAR path[MAX_PATH];
	LPTSTR p;

	lang = UILang(pvar->ts->UILanguageFile);

	hWnd = GetDlgItem(dlg, uid);
	memset(&lvcol, 0, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt = LVCFMT_LEFT;
	lvcol.cx = 150;
	lvcol.pszText = (lang == 2) ? _T("名前") : _T("name");
	lvcol.iSubItem = 0;
	ListView_InsertColumn(hWnd, 0, &lvcol);
	lvcol.cx = 70;
	lvcol.pszText = (lang == 2) ? _T("バージョン") : _T("version");
	lvcol.iSubItem = 1;
	ListView_InsertColumn(hWnd, 1, &lvcol);
	lvcol.cx = 250;
	lvcol.pszText = (lang == 2) ? _T("説明") : _T("information");
	lvcol.iSubItem = 2;
	ListView_InsertColumn(hWnd, 2, &lvcol);
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.cx = 40;
	lvcol.pszText = (lang == 2) ? _T("設定") : _T("On/Off");
	lvcol.iSubItem = 3;
	ListView_InsertColumn(hWnd, 3, &lvcol);
	lvcol.fmt = LVCFMT_RIGHT;
	lvcol.cx = 50;
	lvcol.pszText = _T("#");
	lvcol.iSubItem = 4;
	ListView_InsertColumn(hWnd, 4, &lvcol);

	memset(path, 0, sizeof(path));
	GetModuleFileName(NULL, path, sizeof(path)/sizeof(path[0]) - 1);
    RemoveFileName(path);
	CombinePath(path, sizeof(path)/sizeof(path[0]), _T("TTX*.DLL"));

	hFind = FindFirstFile(path, &win32fd);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		i = 0;
		do
		{
			if (FILE_ATTRIBUTE_DIRECTORY & win32fd.dwFileAttributes)
				continue;
			_tcscpy_s(name, sizeof(name)/sizeof(name[0]), win32fd.cFileName);
			RemoveFileExt(name);
			memset(&item, 0, sizeof(LVITEM));
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.pszText = name;
			item.iSubItem = 0;
			ListView_InsertItem(hWnd, &item);
			//_sntprintf_s(ent, sizeof(ent), _TRUNCATE, _T(".\\%s"), win32fd.cFileName);
		    RemoveFileName(path);
			CombinePath(path, sizeof(path)/sizeof(path[0]), win32fd.cFileName);
			if (GetModuleVersion(buf, sizeof(buf)/sizeof(buf[0]), path))
			{
				item.pszText = buf;
				item.iSubItem = 1;
				ListView_SetItem(hWnd, &item);
			}
			if (GetModuleDescription(buf, sizeof(buf)/sizeof(buf[0]), path))
			{
				item.pszText = buf;
				item.iSubItem = 2;
				ListView_SetItem(hWnd, &item);
			}
			p = (_tcsnicmp(name, _T(INISECTION), sizeof(INISECTION)) == 0) ? _T("-") : _T("on");
			GetPrivateProfileString(_T(INISECTION), name, p, buf, sizeof(buf)/sizeof(TCHAR), fn);
			p = _tcschr(buf, _T(','));
			if (NULL != p)
				*p = 0;
			item.pszText = buf;
			item.iSubItem = 3;
			ListView_SetItem(hWnd, &item);
			if (NULL != p)
			{
				item.pszText = p + 1;
				item.iSubItem = 4;
				ListView_SetItem(hWnd, &item);
			}
			i++;
		} while (FindNextFile(hFind, &win32fd));
	}
	FindClose(hFind);
}

void SaveListView(HWND dlg, UINT uid, LPCTSTR fn)
{
	HWND hWnd;
	TCHAR buf1[16];
	TCHAR buf2[64];
	int i, cnt;

	hWnd = GetDlgItem(dlg, uid);
	cnt = ListView_GetItemCount(hWnd);
	for (i = 0; i < cnt; i++)
	{
		ListView_GetItemText(hWnd, i, 3, buf1, sizeof(buf1)/sizeof(buf1[0]));
		ListView_GetItemText(hWnd, i, 4, buf2, sizeof(buf2)/sizeof(buf2[0]));
		if (buf2[0] && (buf2[0] != _T('0')))
		{
			_tcscat_s(buf1, sizeof(buf1), _T(","));
			_tcscat_s(buf1, sizeof(buf1), buf2);
		}
		ListView_GetItemText(hWnd, i, 0, buf2, sizeof(buf2)/sizeof(buf2[0]));
		WritePrivateProfileString(_T(INISECTION), buf2, buf1, fn);
	}
}

void UpdateListView(HWND dlg, UINT uid, int idx)
{
	HWND hwnd;
	LVITEM item;
	TCHAR buf[64];
	int i;

	hwnd = GetDlgItem(dlg, uid);

	memset(&item, 0, sizeof(LVITEM));
	item.mask = LVIF_TEXT;
	item.iItem = idx;
	item.iSubItem = 3;
	item.cchTextMax = 3;

	for (i = 0; i < 5; i++)
	{
		ListView_GetItemText(hwnd, idx, i, buf, sizeof(buf) / sizeof(buf[0]));
		item.iSubItem = i;
		item.pszText = buf;
		if ((3 == i) && (_T('o') == buf[0]))
			item.pszText = (0 == _tcsnicmp(buf, _T("on"), 2)) ? _T("off") : _T("on");
		ListView_SetItem(hwnd, &item);
	}
}

//
// SettingProc setting dialog callback
//
static LRESULT CALLBACK SettingProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static POINT ptListView, ptBtn;
	static BOOL bUpdate = FALSE;
	LV_HITTESTINFO lvinfo;

	switch (msg)
	{
	case WM_INITDIALOG:
		GetPointRB(dlg, IDC_LISTVIEW, &ptListView);
		GetPointRB(dlg, IDOK, &ptBtn);
		bUpdate = FALSE;
		LoadListView(dlg, IDC_LISTVIEW, pvar->SetupFName);
		MoveParentCenter(dlg);
		return TRUE;

	case WM_SIZE:
		MovePointRB(dlg, IDC_LISTVIEW, &ptListView, RB_RIGHT | RB_BOTTOM);
		MovePointRB(dlg, IDOK, &ptBtn, RB_LEFT | RB_RIGHT | RB_TOP | RB_BOTTOM);
		return TRUE;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->idFrom)
		{
		case IDC_LISTVIEW:
			switch (((LPNMLISTVIEW)lParam)->hdr.code)
			{
			case NM_DBLCLK:
				GetCursorPos((LPPOINT)&lvinfo.pt);
				ScreenToClient(((LPNMLISTVIEW)lParam)->hdr.hwndFrom, &lvinfo.pt);
				ListView_HitTest(((LPNMLISTVIEW)lParam)->hdr.hwndFrom, &lvinfo);
				if (lvinfo.flags & LVHT_ONITEM)
				{
					UpdateListView(dlg, IDC_LISTVIEW, lvinfo.iItem);
					bUpdate = TRUE;
				}
				break;
			}
			break;
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (bUpdate)
			{
				SaveListView(dlg, IDC_LISTVIEW, pvar->SetupFName);
				EndDialog(dlg, IDOK);
				return TRUE;
			}
		case IDCANCEL:
			EndDialog(dlg, IDCANCEL);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

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
		switch (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PLUGIN),
							   hWin, SettingProc, (LPARAM)NULL))
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

// static void PASCAL TTXEnd(void)
// {
// 	 printf("TTXEnd %d\n", ORDER);
// }

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
