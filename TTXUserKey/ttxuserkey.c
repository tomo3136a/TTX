/*
 * Tera Term UserKey Edit Extension
 * (C) 2021 tomo3136a
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

#define ORDER 6020

#define INISECTION "TTXUserKey"
#define DBFILE "userkey.db"

#define ID_MENUITEM 56020

//99(IdKeyMax-IdUser1)まで割り振れるけど現実的な最大値にしておく
#define USERKEY_MAX 20

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;

	//menu
	HMENU SetupMenu;

	//status
	strset_t codeset;
	strset_t menuset;

} TInstVar;

static TInstVar FAR *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	pvar->ts = ts;
	pvar->cv = cv;

	pvar->codeset = NULL;
	pvar->menuset = NULL;
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
// {
// 	 printf("TTXGetUIHooks %d\n", ORDER);
// }

// static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks)
// {
// 	 printf("TTXGetSetupHooks %d\n", ORDER);
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
//   printf("TTXSetWinSize %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

const PTCHAR userkeytype_en[] = {_T("String"), _T("Convert"), _T("Macro file"), _T("Menu ID")};
const PTCHAR userkeytype_ja[] = {_T("文字列"), _T("変換文字列"), _T("マクロ"), _T("メニュー")};
const PTCHAR *userkeytype = userkeytype_en;
#define USERKEYTYPE_CNT (sizeof(userkeytype_en) / sizeof(userkeytype_en[0]))

void InitKeyListDlg(HWND hWnd, PTCHAR fn)
{
	HWND hCombo;
	TCHAR name[32];
	strset_t ctx;
	PTCHAR p;
	int i;
	UINT lang;

	lang = UILang(pvar->ts->UILanguageFile);
	userkeytype = (lang == 2) ? userkeytype_ja : userkeytype_en;

	hCombo = GetDlgItem(hWnd, IDC_COMBO_KEY);
	GetIniStrSet(_T("key code"), &(pvar->codeset), 64, 64, fn);
	if (pvar->codeset)
	{
		p = StrSetTok(pvar->codeset, &ctx);
		while (p)
		{
			i = (int)(_tcschr(p, _T('=')) - p);
			_tcsncpy_s(name, sizeof(name)/sizeof(name[0]), p, i);
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)name);
			p = StrSetTok(NULL, &ctx);
		}
	}

	hCombo = GetDlgItem(hWnd, IDC_COMBO_TYPE);
	for (i = 0; i < USERKEYTYPE_CNT; i++)
	{
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)userkeytype[i]);
	}

	hCombo = GetDlgItem(hWnd, IDC_COMBO_CMD);
	GetIniStrSet(_T("menu"), &(pvar->menuset), 64, 64, fn);
	if (pvar->menuset)
	{
		p = StrSetTok(pvar->menuset, &ctx);
		while (p)
		{
			i = (int)(_tcschr(p, _T('=')) - p);
			_tcsncpy_s(name, sizeof(name)/sizeof(name[0]), p, i);
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)name);
			p = StrSetTok(NULL, &ctx);
		}
	}
}

void LoadKeyList(HWND hWnd, UINT uid, LPTSTR fn)
{
	LVCOLUMN lvcol;
	LVITEM item;
	HWND hList;
	TCHAR name[32];
	PTCHAR buf;
	int buf_sz;
	int i, j;
	PTCHAR ctx;
	PTCHAR p;
	PTCHAR p2;
	UINT lang;

	lang = UILang(pvar->ts->UILanguageFile);

	hList = GetDlgItem(hWnd, uid);
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt = LVCFMT_LEFT;
	lvcol.cx = 40;
	lvcol.pszText = (lang == 2) ? _T("キー") : _T("key");
	lvcol.iSubItem = 0;
	ListView_InsertColumn(hList, 0, &lvcol);
	lvcol.cx = 80;
	lvcol.pszText = (lang == 2) ? _T("タイプ") : _T("type");
	lvcol.iSubItem = 1;
	ListView_InsertColumn(hList, 1, &lvcol);
	lvcol.cx = 300;
	lvcol.pszText = (lang == 2) ? _T("値") : _T("value");
	lvcol.iSubItem = 2;
	ListView_InsertColumn(hList, 2, &lvcol);

	buf_sz = 1024;
	buf = (PTCHAR)malloc(buf_sz*sizeof(TCHAR));
	if (!buf)
		return;

	for (i = 0; i < USERKEY_MAX; i++)
	{
		_sntprintf_s(name, sizeof(name)/sizeof(name[0]), 
			_TRUNCATE, _T("User%d"), i + 1);
		GetPrivateProfileString(_T("User keys"), name, _T(""), 
			buf, buf_sz, fn);
		if (buf[0])
		{
			p = _tcstok_s(buf, _T(","), &ctx);
			_tcscpy_s(name, sizeof(name)/sizeof(name[0]), p);
			p = StrSetFindVal(pvar->codeset, name);
			if (*p)
			{
				_tcscpy_s(name, sizeof(name)/sizeof(name[0]), p);
				p = _tcschr(name, _T('='));
				if (p)
				{
					*p = 0;
				}
			}
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.pszText = name;
			item.iSubItem = 0;
			ListView_InsertItem(hList, &item);
			p = _tcstok_s(NULL, _T(","), &ctx);
			j = _tstoi(p);
			item.pszText = (PTCHAR)userkeytype[j];
			item.iSubItem = 1;
			ListView_SetItem(hList, &item);
			p = _tcstok_s(NULL, _T(","), &ctx);
			if (j == 3)
			{
				j = _tstoi(p);
				_sntprintf_s(name, sizeof(name)/sizeof(name[0]), 
					_TRUNCATE, _T("%d"), j);
				p2 = StrSetFindKey(pvar->menuset, name);
				if (p2 && *p2)
				{
					p = p2;
				}
			}
			item.pszText = p;
			item.iSubItem = 2;
			ListView_SetItem(hList, &item);
		}
	}
	free(buf);
}

void SetKeyMapOff(UINT code, LPTSTR fn)
{
	strset_t apps, keys;
	strset_t app_ctx, keys_ctx;
	PTCHAR app;
	PTCHAR key;

	apps = keys = NULL;
	GetIniSects(&apps, 64, 64, fn);
	app = StrSetTok(apps, &app_ctx);
	while (app)
	{
		if (_tcsicmp(app, _T("User Keys")) != 0)
		{
			GetIniKeys(app, &keys, 64, 64, fn);
			key = StrSetTok(keys, &keys_ctx);
			while (key)
			{
				if (GetIniNum(app, key, 0, fn) == code)
				{
					WritePrivateProfileString(app, key, _T("off"), fn);
				}
				key = StrSetTok(NULL, &keys_ctx);
			}
		}
		app = StrSetTok(NULL, &app_ctx);
	}
	free(keys);
	free(apps);
}

void SaveKeyList(HWND hWnd, UINT uid, LPTSTR fn)
{
	LVITEM item;
	HWND hList;
	TCHAR name[32];
	PTCHAR buf;
	int buf_sz;
	int code;
	int i, j, cnt;
	PTCHAR p;

	buf_sz = 1024;
	buf = (PTCHAR)malloc(buf_sz*sizeof(TCHAR));
	if (!buf)
		return;

	hList = GetDlgItem(hWnd, uid);
	cnt = ListView_GetItemCount(hList);
	item.mask = LVIF_TEXT;
	item.pszText = name;
	item.cchTextMax = sizeof(name)/sizeof(name[0]);
	for (i = 0; i < cnt; i++)
	{
		item.iItem = i;
		item.iSubItem = 0;
		ListView_GetItem(hList, &item);
		p = StrSetFindKey(pvar->codeset, name);
		p = strskip(p, _T('='));
		if (!(*p))
		{
			p = name;
		}
		code = _tstoi(p);
		_sntprintf_s(buf, buf_sz, _TRUNCATE, _T("%s,"), p);

		item.iSubItem = 1;
		ListView_GetItem(hList, &item);
		for (j = 0; j < USERKEYTYPE_CNT; j++)
		{
			if (_tcsncmp(userkeytype[j], name, sizeof(name)/sizeof(name[0])) == 0)
			{
				break;
			}
		}
		_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("%d,"), j);
		_tcscat_s(buf, buf_sz, name);

		item.iSubItem = 2;
		ListView_GetItem(hList, &item);
		if (j == 3)
		{
			j = _tstoi(name);
			_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("%d"), j);
		}
		_tcscat_s(buf, buf_sz, name);
		SetKeyMapOff(code, fn);
		_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("User%d"), i + 1);
		WritePrivateProfileString(_T("User keys"), name, buf, fn);
	}
	free(buf);

	for (i = cnt; i < USERKEY_MAX; i++)
	{
		_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("User%d"), i + 1);
		WritePrivateProfileString(_T("User keys"), name, NULL, fn);
	}
}

///////////////////////////////////////////////////////////////

void AddKeyListItem(HWND hWnd, PTCHAR k, int type, PTCHAR s)
{
	LVITEM item;
	TCHAR name[32];
	int i, cnt;

	if (!(k[0]) || type < 0 || type >= 4)
		return;

	cnt = ListView_GetItemCount(hWnd);
	item.mask = LVIF_TEXT;
	item.pszText = name;
	item.cchTextMax = sizeof(name);
	item.iSubItem = 0;
	for (i = 0; i < cnt; i++)
	{
		item.iItem = i;
		ListView_GetItem(hWnd, &item);
		if (_tcsicmp(name, k) == 0)
			break;
	}
	if (i >= cnt)
	{
		item.iItem = i;
		_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, k);
		ListView_InsertItem(hWnd, &item);
	}
	item.pszText = (PTCHAR)userkeytype[type];
	item.iSubItem = 1;
	ListView_SetItem(hWnd, &item);
	item.pszText = s;
	item.iSubItem = 2;
	ListView_SetItem(hWnd, &item);
	ListView_SetItemState(hWnd, i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	ListView_EnsureVisible(hWnd, i, TRUE);
}

void DeleteKeyListItem(HWND hWnd)
{
	int i, j, cnt;

	cnt = ListView_GetItemCount(hWnd);
	for (i = 0, j = 0; i < cnt; i++)
	{
		if (ListView_GetItemState(hWnd, i, LVIS_SELECTED))
		{
			j = i;
			ListView_DeleteItem(hWnd, i);
			i--;
			cnt--;
		}
	}
	ListView_SetItemState(hWnd, j, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	ListView_EnsureVisible(hWnd, j, TRUE);
}

void MoveKeyListItemPrior(HWND hWnd)
{
	LVITEM item;
	PTCHAR buf;
	int buf_sz;
	int i, cnt;

	buf_sz = 1024;
	buf = (PTCHAR)malloc(buf_sz*sizeof(TCHAR));
	if (!buf)
		return;

	cnt = ListView_GetItemCount(hWnd);
	item.mask = LVIF_TEXT;
	item.pszText = buf;
	item.cchTextMax = buf_sz;
	for (i = 1; i < cnt; i++)
	{
		if (ListView_GetItemState(hWnd, i, LVIS_SELECTED))
		{
			item.iSubItem = 0;
			item.iItem = i - 1;
			ListView_GetItem(hWnd, &item);
			item.iItem = i + 1;
			ListView_InsertItem(hWnd, &item);
			item.iSubItem = 1;
			item.iItem = i - 1;
			ListView_GetItem(hWnd, &item);
			item.iItem = i + 1;
			ListView_SetItem(hWnd, &item);
			item.iSubItem = 2;
			item.iItem = i - 1;
			ListView_GetItem(hWnd, &item);
			item.iItem = i + 1;
			ListView_SetItem(hWnd, &item);
			ListView_DeleteItem(hWnd, i - 1);
		}
	}
	free(buf);
}

void MoveKeyListItemNext(HWND hWnd)
{
	LVITEM item;
	PTCHAR buf;
	int buf_sz;
	int i, cnt;

	buf_sz = 1024;
	buf = (PTCHAR)malloc(buf_sz*sizeof(TCHAR));
	if (!buf)
		return;

	cnt = ListView_GetItemCount(hWnd);
	item.mask = LVIF_TEXT;
	item.pszText = buf;
	item.cchTextMax = buf_sz;
	for (i = cnt - 2; i >= 0; i--)
	{
		if (ListView_GetItemState(hWnd, i, LVIS_SELECTED))
		{
			item.iSubItem = 0;
			item.iItem = i + 1;
			ListView_GetItem(hWnd, &item);
			item.iItem = i;
			ListView_InsertItem(hWnd, &item);
			item.iSubItem = 1;
			item.iItem = i + 2;
			ListView_GetItem(hWnd, &item);
			item.iItem = i;
			ListView_SetItem(hWnd, &item);
			item.iSubItem = 2;
			item.iItem = i + 2;
			ListView_GetItem(hWnd, &item);
			item.iItem = i;
			ListView_SetItem(hWnd, &item);
			ListView_DeleteItem(hWnd, i + 2);
		}
	}
	free(buf);
}

void UpdateKeyListItem(HWND dlg, int idx)
{
	HWND hWnd;
	HWND hListView;
	LVITEM item;
	TCHAR name[32];
	PTCHAR buf;
	int buf_sz;
	int i;

	buf_sz = 1024;
	buf = (PTCHAR)malloc(buf_sz*sizeof(TCHAR));
	if (!buf)
		return;

	hListView = GetDlgItem(dlg, IDC_KEYLIST);
	item.mask = TVIF_HANDLE | TVIF_TEXT;
	item.iItem = idx;
	item.iSubItem = 0;
	item.pszText = buf;
	item.cchTextMax = buf_sz;
	ListView_GetItem(hListView, &item);

	hWnd = GetDlgItem(dlg, IDC_COMBO_KEY);
	i = SendMessage(hWnd, CB_FINDSTRINGEXACT, -1, (LPARAM)buf);
	if (i < 0)
	{
		SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)buf);
	}
	else
	{
		SendMessage(hWnd, CB_SETCURSEL, i, 0);
	}
	item.iSubItem = 1;
	ListView_GetItem(hListView, &item);

	hWnd = GetDlgItem(dlg, IDC_COMBO_TYPE);
	i = SendMessage(hWnd, CB_FINDSTRINGEXACT, -1, (LPARAM)buf);
	EnableWindow(GetDlgItem(dlg, IDC_COMBO_CMD), (i == 3));
	EnableWindow(GetDlgItem(dlg, IDC_EDIT_STR), (i != 3));
	EnableWindow(GetDlgItem(dlg, IDC_BUTTON_SEL), (i == 2));
	SendMessage(hWnd, CB_SETCURSEL, i, 0);
	item.iSubItem = 2;
	ListView_GetItem(hListView, &item);
	SetDlgItemText(dlg, IDC_EDIT_STR, buf);

	name[0] = 0;
	if (i == 3)
	{
		i = _tstoi(buf);
		_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("%d"), i);
	}
	SetDlgItemText(dlg, IDC_COMBO_CMD, name);
	free(buf);
}

///////////////////////////////////////////////////////////////

//
// KeyListProc設定ダイアログのコールバック関数。
//

static WNDPROC KeyListProcOld;

static LRESULT CALLBACK KeyListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPMSG pMsg = NULL;

	if (uMsg == WM_GETDLGCODE && lParam != 0)
	{
		pMsg = (LPMSG)lParam;
		if (pMsg->message == WM_KEYDOWN)
		{
			switch (pMsg->wParam)
			{
			case VK_UP:
				if (GetKeyState(VK_SHIFT) < 0)
				{
					MoveKeyListItemPrior(hWnd);
					return FALSE;
				}
				break;
			case VK_DOWN:
				if (GetKeyState(VK_SHIFT) < 0)
				{
					MoveKeyListItemNext(hWnd);
					return FALSE;
				}
				break;
			case VK_DELETE:
				DeleteKeyListItem(hWnd);
				break;
			default:
				break;
			}
		}
	}
	return CallWindowProc(KeyListProcOld, hWnd, uMsg, wParam, lParam);
}

//
// UserKeySettingProc設定ダイアログのコールバック関数。
//
static LRESULT CALLBACK UserKeySettingProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LV_HITTESTINFO lvinfo;
	TCHAR path[MAX_PATH];
	TCHAR name[32];
	PTCHAR buf;
	int buf_sz;
	int i;
	LPTSTR p;

	switch (msg)
	{
	case WM_INITDIALOG:
		memset(path, 0, sizeof(path));
		GetModuleFileName(NULL, path, sizeof(path)/sizeof(path[0]) - 1);
    	RemoveFileName(path);
		CombinePath(path, sizeof(path)/sizeof(path[0]), _T(DBFILE));
		InitKeyListDlg(dlg, path);

		p = TTXGetPath(pvar->ts, ID_KEYCNFNM);
		SetDlgItemText(dlg, IDC_PATH, p);
		LoadKeyList(dlg, IDC_KEYLIST, p);
		TTXFree(p);
		KeyListProcOld = (WNDPROC)SetWindowLong(
			GetDlgItem(dlg, IDC_KEYLIST), GWLP_WNDPROC, (LONG)KeyListProc);
		MoveParentCenter(dlg);
		return TRUE;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->idFrom)
		{
		case IDC_KEYLIST:
			switch (((LPNMLISTVIEW)lParam)->hdr.code)
			{
			case LVN_ITEMCHANGED:
				UpdateKeyListItem(dlg, ((LPNMLISTVIEW)lParam)->iItem);
				break;

			case NM_DBLCLK:
				GetCursorPos((LPPOINT)&lvinfo.pt);
				ScreenToClient(((LPNMLISTVIEW)lParam)->hdr.hwndFrom, &lvinfo.pt);
				ListView_HitTest(((LPNMLISTVIEW)lParam)->hdr.hwndFrom, &lvinfo);
				if ((lvinfo.flags & LVHT_ONITEM) != 0)
				{
					UpdateKeyListItem(dlg, lvinfo.iItem);
				}
				break;
			}
			break;
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_COMBO_TYPE:
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				i = SendMessage(GetDlgItem(dlg, IDC_COMBO_TYPE), CB_GETCURSEL, 0, 0);
				EnableWindow(GetDlgItem(dlg, IDC_COMBO_CMD), (i == 3));
				EnableWindow(GetDlgItem(dlg, IDC_EDIT_STR), (i != 3));
				EnableWindow(GetDlgItem(dlg, IDC_BUTTON_SEL), (i == 2));
			}
			return TRUE;

		case IDC_COMBO_CMD:
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE:
			case CBN_EDITCHANGE:
				buf_sz = MAX_PATH;
				buf = (PTCHAR)malloc(buf_sz*sizeof(TCHAR));
				if (!buf)
					return FALSE;
				i = SendMessage(GetDlgItem(dlg, IDC_COMBO_CMD), CB_GETCURSEL, 0, 0);
				if (i < 0)
				{
					GetDlgItemText(dlg, IDC_COMBO_CMD, name, sizeof(name));
					_tcscpy_s(buf, buf_sz, StrSetFindKey(pvar->menuset, name));
					if (!buf[0])
					{
						_tcscpy_s(buf, buf_sz, name);
					}
				}
				else
				{
					_tcscpy_s(buf, buf_sz, StrSetAt(pvar->menuset, i));
				}
				SetDlgItemText(dlg, IDC_EDIT_STR, buf);
				free(buf);
				break;
			}
			return TRUE;

		case IDC_BUTTON_SEL:
			buf_sz = MAX_PATH;
			buf = (PTCHAR)malloc(buf_sz*sizeof(TCHAR));
			if (!buf)
				return FALSE;
			GetDlgItemText(dlg, IDC_EDIT_STR, buf, buf_sz);
			p = _T("マクロファイル(*.ttl)\0*.ttl\0\0");
			OpenFileDlg(dlg, IDC_EDIT_STR, _T("マクロファイルを選択してください"), p, buf, NULL, 1);
			free(buf);
			return TRUE;

		case IDC_BUTTON_ADD:
			buf_sz = MAX_PATH;
			buf = (PTCHAR)malloc(buf_sz*sizeof(TCHAR));
			if (!buf)
				return FALSE;
			GetDlgItemText(dlg, IDC_COMBO_KEY, name, sizeof(name)/sizeof(name[0]));
			GetDlgItemText(dlg, IDC_EDIT_STR, buf, buf_sz);
			i = SendMessage(GetDlgItem(dlg, IDC_COMBO_TYPE), CB_GETCURSEL, 0, 0);
			AddKeyListItem(GetDlgItem(dlg, IDC_KEYLIST), name, i, buf);
			SetFocus(GetDlgItem(dlg, IDC_KEYLIST));
			free(buf);
			return TRUE;

		case IDC_BUTTON_DEL:
			DeleteKeyListItem(GetDlgItem(dlg, IDC_KEYLIST));
			SetFocus(GetDlgItem(dlg, IDC_KEYLIST));
			return TRUE;

		case IDOK:
			p = TTXGetPath(pvar->ts, ID_KEYCNFNM);
			SetDlgItemText(dlg, IDC_PATH, p);
			SaveKeyList(dlg, IDC_KEYLIST, p);
			TTXFree(p);
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
	LPTSTR s;

	lang = UILang(pvar->ts->UILanguageFile);

	pvar->SetupMenu = GetSubMenu(menu, ID_SETUP);
	s = (lang == 2) ? _T("ユーザーキー(&U)...") : _T("&UserKey setup...");
	AppendMenu(pvar->SetupMenu, MF_BYCOMMAND, TTXMenuID(ID_MENUITEM), s);
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
		switch (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_USERKEY),
							   hWin, UserKeySettingProc, (LPARAM)NULL))
		{
		case IDOK:
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
