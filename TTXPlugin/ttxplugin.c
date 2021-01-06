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

#include "compat_w95.h"
#include "ttxcommon.h"
#include "resource.h"

#define ORDER 6001

#define INISECTION "TTXPlugin"
#define LOADSECTION "Load"
#define MENUSECTION "Menu"

#define ID_MENUITEM 56001

#define CONTROLFILE ".\\ttx.txt"

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;
	int menuoffset;

	//menu
	HMENU HelpMenu;

} TInstVar;

static TInstVar FAR *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	pvar->ts = ts;
	pvar->cv = cv;
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

static void TrimVersion(char *ver)
{
	LPSTR p, q;

	p = ver + strlen(ver);
	while (ver < p)
	{
		if (*p == '.')
		{
			q = p + 1;
			while (*q)
			{
				if (*q != '0')
					return;
				q++;
			}
			*p = 0;
		}
		p--;
	}
}

static BOOL GetModuleVersion(char *buf, int sz, char *fn)
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

	lpBuf = malloc(dwSize + 2);
	memset(lpBuf, 0, dwSize + 2);
	if (!GetFileVersionInfo(fn, dwHandle, dwSize, lpBuf))
	{
		free(lpBuf);
		return FALSE;
	}

	if (!VerQueryValue(lpBuf, "\\", (LPVOID *)&pFileInfo, &uLen))
	{
		free(lpBuf);
		return FALSE;
	}
	_snprintf_s(buf, sz, _TRUNCATE, "%d.%d.%d.%d",
				HIWORD(pFileInfo->dwFileVersionMS), LOWORD(pFileInfo->dwFileVersionMS),
				HIWORD(pFileInfo->dwFileVersionLS), LOWORD(pFileInfo->dwFileVersionLS));

	TrimVersion(buf);

	free(lpBuf);
	return TRUE;
}

static BOOL GetModuleDescription(char *buf, int sz, char *fn)
{
	//#pragma comment(lib, "version.lib")
	DWORD dwSize;
	DWORD dwHandle;
	LPVOID lpBuf;
	UINT uLen;
	LPSTR s;
	LPSTR p;

	dwSize = GetFileVersionInfoSize(fn, &dwHandle);
	if (dwSize == 0)
	{
		return FALSE;
	}

	lpBuf = malloc(dwSize + 2);
	memset(lpBuf, 0, dwSize + 2);
	if (!GetFileVersionInfo(fn, dwHandle, dwSize, lpBuf))
	{
		free(lpBuf);
		return FALSE;
	}

	s = "\\StringFileInfo\\040904b0\\FileDescription";
	if (!VerQueryValue(lpBuf, s, (LPVOID *)&p, &uLen))
	{
		free(lpBuf);
		return FALSE;
	}
	strcpy_s(buf, sz, (0 < uLen) ? p : "");

	free(lpBuf);
	return TRUE;
}

void LoadListView(HWND dlg, UINT uid, PCHAR fn)
{
	HWND hWnd;
	LVCOLUMN lvcol;
	LVITEM item;
	WIN32_FIND_DATA win32fd;
	HANDLE hFind;
	char name[64];
	char ent[70];
	char buf[256];
	UINT lang;
	int i;

	lang = UILang(pvar->ts->UILanguageFile);

	hWnd = GetDlgItem(dlg, uid);
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt = LVCFMT_LEFT;
	lvcol.cx = 130;
	lvcol.pszText = (lang == 2) ? "名前" : "name";
	lvcol.iSubItem = 0;
	ListView_InsertColumn(hWnd, 0, &lvcol);
	lvcol.cx = 70;
	lvcol.pszText = (lang == 2) ? "バージョン" : "version";
	lvcol.iSubItem = 1;
	ListView_InsertColumn(hWnd, 1, &lvcol);
	lvcol.cx = 40;
	lvcol.pszText = (lang == 2) ? "設定" : "On/Off";
	lvcol.iSubItem = 2;
	ListView_InsertColumn(hWnd, 2, &lvcol);
	lvcol.cx = 50;
	lvcol.pszText = (lang == 2) ? "メニュー" : "Menu";
	lvcol.iSubItem = 3;
	ListView_InsertColumn(hWnd, 3, &lvcol);
	lvcol.cx = 250;
	lvcol.pszText = (lang == 2) ? "説明" : "information";
	lvcol.iSubItem = 4;
	ListView_InsertColumn(hWnd, 4, &lvcol);

	hFind = FindFirstFile(".\\TTX*.DLL", &win32fd);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		i = 0;
		do
		{
			if (FILE_ATTRIBUTE_DIRECTORY & win32fd.dwFileAttributes)
				continue;
			strcpy_s(name, sizeof(name), win32fd.cFileName);
			RemoveFileExt(name);
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.pszText = name;
			item.iSubItem = 0;
			ListView_InsertItem(hWnd, &item);
			_snprintf_s(ent, sizeof(ent), _TRUNCATE, ".\\%s", win32fd.cFileName);
			if (GetModuleVersion(buf, sizeof(buf), ent))
			{
				item.pszText = buf;
				item.iSubItem = 1;
				ListView_SetItem(hWnd, &item);
			}
			ListView_SetItem(hWnd, &item);
			GetPrivateProfileString(LOADSECTION, name, "on", buf, sizeof(buf), fn);
			item.pszText = buf;
			item.iSubItem = 2;
			ListView_SetItem(hWnd, &item);
			GetPrivateProfileString(MENUSECTION, name, "", buf, sizeof(buf), fn);
			item.pszText = buf;
			item.iSubItem = 3;
			ListView_SetItem(hWnd, &item);
			if (GetModuleDescription(buf, sizeof(buf), ent))
			{
				item.pszText = buf;
				item.iSubItem = 4;
				ListView_SetItem(hWnd, &item);
			}
			i++;
		} while (FindNextFile(hFind, &win32fd));
	}
	FindClose(hFind);
}

void SaveListView(HWND dlg, UINT uid, PCHAR fn)
{
	HWND hWnd;
	char name[64];
	char buf[16];
	int i, cnt;

	hWnd = GetDlgItem(dlg, uid);
	cnt = ListView_GetItemCount(hWnd);
	for (i = 0; i < cnt; i++)
	{
		ListView_GetItemText(hWnd, i, 0, name, sizeof(name));
		ListView_GetItemText(hWnd, i, 2, buf, sizeof(buf));
		WritePrivateProfileString(LOADSECTION, name, buf, fn);
		ListView_GetItemText(hWnd, i, 3, buf, sizeof(buf));
		WritePrivateProfileString(MENUSECTION, name, (buf[0] ? buf : NULL), fn);
	}
}

BOOL GetListViewIgnore(HWND dlg, UINT uid, int idx)
{
	char buf[16];

	ListView_GetItemText(GetDlgItem(dlg, uid), idx, 2, buf, sizeof(buf));
	return (strchr(buf, '-') != NULL);
}

BOOL GetListViewOnOff(HWND dlg, UINT uid, int idx)
{
	char buf[16];

	ListView_GetItemText(GetDlgItem(dlg, uid), idx, 2, buf, sizeof(buf));
	return (_strnicmp(buf, "on", 2) == 0);
}

void UpdateListView(HWND dlg, UINT uid, int idx, BOOL b)
{
	HWND hWnd;
	LVITEM item;

	hWnd = GetDlgItem(dlg, uid);
	item.mask = LVIF_TEXT;
	item.iSubItem = 0;
	item.iItem = idx;
	item.pszText = b ? "on" : "off";
	item.cchTextMax = 3;
	item.iSubItem = 2;
	ListView_SetItem(hWnd, &item);
	ListView_SetItemState(hWnd, idx, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	ListView_EnsureVisible(hWnd, idx, TRUE);
}

void DrawListViewItem(LPDRAWITEMSTRUCT lpDraw)
{
	HDC hdc;
	HWND hwnd;
	UINT uid;
	RECT rcItem, rcSubItem;
	HBRUSH hbr, hbr2;
	char buf[256];
	UINT flg;
	int i;

	hdc = lpDraw->hDC;
	SaveDC(hdc);

	hwnd = lpDraw->hwndItem;
	uid = lpDraw->itemID;

	ListView_GetItemRect(hwnd, uid, &rcItem, LVIR_LABEL);
	ListView_GetItemText(hwnd, uid, 0, buf, sizeof(buf) / sizeof(buf[0]));
	if (lpDraw->itemState & ODS_SELECTED)
		hbr = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
	else
		hbr = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(hdc, &rcItem, hbr);
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, buf, -1, &rcItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	hbr2 = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	for (i = 1; i < 5; i++)
	{
		ListView_GetSubItemRect(hwnd, uid, i, LVIR_LABEL, &rcSubItem);
		ListView_GetItemText(hwnd, uid, i, buf, sizeof(buf) / sizeof(buf[0]));
		if (i == 2 && _strnicmp(buf, "off", 3) == 0)
		{
			FillRect(hdc, &rcSubItem, hbr2);
		}

		flg = DT_VCENTER | DT_SINGLELINE;
		flg |= (i == 1 || i == 2 || i == 3) ? DT_CENTER : DT_LEFT;
		DrawText(hdc, buf, -1, &rcSubItem, flg);
	}

	rcItem.right = rcSubItem.right;
	if (lpDraw->itemState & ODS_FOCUS)
		DrawFocusRect(hdc, &rcItem);

	DeleteObject(hbr2);
	DeleteObject(hbr);

	RestoreDC(hdc, -1);
}

//
// SettingProc setting dialog callback
//
static LRESULT CALLBACK SettingProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LV_HITTESTINFO lvinfo;
	int i;
	BOOL b;

	switch (msg)
	{
	case WM_INITDIALOG:
		LoadListView(dlg, IDC_LISTVIEW, CONTROLFILE);
		MoveParentCenter(dlg);
		return TRUE;

	case WM_DRAWITEM:
		DrawListViewItem((LPDRAWITEMSTRUCT)lParam);
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
				if ((lvinfo.flags & LVHT_ONITEM) != 0)
				{
					i = lvinfo.iItem;
					if (GetListViewIgnore(dlg, IDC_LISTVIEW, i))
						break;
					b = GetListViewOnOff(dlg, IDC_LISTVIEW, i);
					UpdateListView(dlg, IDC_LISTVIEW, i, !b);
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
			SaveListView(dlg, IDC_LISTVIEW, CONTROLFILE);
			EndDialog(dlg, IDOK);
			return TRUE;

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
	LPSTR s;
	int idx;

	lang = UILang(pvar->ts->UILanguageFile);
	pvar->menuoffset = MenuOffset(INISECTION, ID_MENUITEM, 0);

	idx = GetMenuItemCount(menu) - 1;
	pvar->HelpMenu = GetSubMenu(menu, idx);

	s = (lang == 2) ? "TTX 機能一覧(&L)" : "TTX &list";
	InsertMenu(pvar->HelpMenu, 2, MF_BYPOSITION, ID_MENUITEM + pvar->menuoffset, s);
}

// static void PASCAL TTXModifyPopupMenu(HMENU menu)
// {
// 	printf("TTXModifyPopupMenu %d\n", ORDER);
// }

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (cmd + pvar->menuoffset)
	{
	case ID_MENUITEM:
		switch (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PLUGIN),
							   hWin, SettingProc, (LPARAM)NULL))
		{
		case IDOK:
			MessageBox(hWin, "Please exit Tera Term.", "Tera Term",
					   MB_OK | MB_ICONINFORMATION);
			break;
		case IDCANCEL:
			break;
		case -1:
			MessageBox(hWin, "Error", "Can't display dialog box.",
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
