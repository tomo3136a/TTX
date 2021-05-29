/*
 * Tera Term Plugin Extension
 * (C) 2021 tomo3136a
 * TTX(Tera Term Extension) �̈ꗗ��\������B
 * �܂��A�@�\�̗L���E������؂�ւ����s���B
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

#define ID_MENUITEM 56001

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;

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
	char *p;

	lang = UILang(pvar->ts->UILanguageFile);

	hWnd = GetDlgItem(dlg, uid);
	memset(&lvcol, 0, sizeof(LVCOLUMN));
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt = LVCFMT_LEFT;
	lvcol.cx = 130;
	lvcol.pszText = (lang == 2) ? "���O" : "name";
	lvcol.iSubItem = 0;
	ListView_InsertColumn(hWnd, 0, &lvcol);
	lvcol.cx = 70;
	lvcol.pszText = (lang == 2) ? "�o�[�W����" : "version";
	lvcol.iSubItem = 1;
	ListView_InsertColumn(hWnd, 1, &lvcol);
	lvcol.cx = 250;
	lvcol.pszText = (lang == 2) ? "����" : "information";
	lvcol.iSubItem = 2;
	ListView_InsertColumn(hWnd, 2, &lvcol);
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.cx = 40;
	lvcol.pszText = (lang == 2) ? "�ݒ�" : "On/Off";
	lvcol.iSubItem = 3;
	ListView_InsertColumn(hWnd, 3, &lvcol);
	lvcol.fmt = LVCFMT_RIGHT;
	lvcol.cx = 50;
	lvcol.pszText = "#";
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
			memset(&item, 0, sizeof(LVITEM));
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
			if (GetModuleDescription(buf, sizeof(buf), ent))
			{
				item.pszText = buf;
				item.iSubItem = 2;
				ListView_SetItem(hWnd, &item);
			}
			GetPrivateProfileString(INISECTION, name, "on", buf, sizeof(buf), fn);
			p = strchr(buf, ',');
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

void SaveListView(HWND dlg, UINT uid, PCHAR fn)
{
	HWND hWnd;
	char buf1[16];
	char buf2[64];
	int i, cnt;

	hWnd = GetDlgItem(dlg, uid);
	cnt = ListView_GetItemCount(hWnd);
	for (i = 0; i < cnt; i++)
	{
		ListView_GetItemText(hWnd, i, 3, buf1, sizeof(buf1));
		ListView_GetItemText(hWnd, i, 4, buf2, sizeof(buf2));
		if (buf2[0] && (buf2[0] != '0'))
		{
			strcat_s(buf1, sizeof(buf1), ",");
			strcat_s(buf1, sizeof(buf1), buf2);
		}
		ListView_GetItemText(hWnd, i, 0, buf2, sizeof(buf2));
		WritePrivateProfileString(INISECTION, buf2, buf1, fn);
	}
}

void UpdateListView(HWND dlg, UINT uid, int idx)
{
	HWND hwnd;
	LVITEM item;
	char buf[64];
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
		if ((3 == i) && ('o' == buf[0]))
			item.pszText = (0 == _strnicmp(buf, "on", 2)) ? "off" : "on";
		ListView_SetItem(hwnd, &item);
	}
}

//
// SettingProc setting dialog callback
//
static LRESULT CALLBACK SettingProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL bUpdate = FALSE;
	LV_HITTESTINFO lvinfo;

	switch (msg)
	{
	case WM_INITDIALOG:
		bUpdate = FALSE;
		LoadListView(dlg, IDC_LISTVIEW, pvar->ts->SetupFName);
		MoveParentCenter(dlg);
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
				SaveListView(dlg, IDC_LISTVIEW, pvar->ts->SetupFName);
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
	LPSTR s;
	int idx;

	lang = UILang(pvar->ts->UILanguageFile);

	idx = GetMenuItemCount(menu) - 1;
	pvar->HelpMenu = GetSubMenu(menu, idx);

	s = (lang == 2) ? "TTX �@�\�ꗗ(&L)" : "TTX &list";
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
