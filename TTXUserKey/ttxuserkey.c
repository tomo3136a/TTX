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

#include "compat_w95.h"
#include "ttxcommon.h"
#include "resource.h"

#define ORDER 6020

#define INISECTION "TTXUserKey"

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

const char *userkeytype_en[] = {"String", "Convert", "Macro file", "Menu ID"};
const char *userkeytype_ja[] = {"文字列", "変換文字列", "マクロ", "メニュー"};
const char **userkeytype = userkeytype_en;
#define USERKEYTYPE_CNT (sizeof(userkeytype_en) / sizeof(userkeytype_en[0]))

void InitKeyListDlg(HWND hWnd, char *fn)
{
	HWND hCombo;
	char name[32];
	strset_t ctx;
	char *p;
	int i;
	UINT lang;

	lang = UILang(pvar->ts->UILanguageFile);
	userkeytype = (lang == 2) ? userkeytype_ja : userkeytype_en;

	hCombo = GetDlgItem(hWnd, IDC_COMBO_KEY);
	GetIniStrSet("key code", &(pvar->codeset), 64, 64, fn);
	if (pvar->codeset)
	{
		p = StrSetTok(pvar->codeset, &ctx);
		while (p)
		{
			strncpy_s(name, sizeof(name), p, (int)(strchr(p, '=') - p));
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
	GetIniStrSet("menu", &(pvar->menuset), 64, 64, fn);
	if (pvar->menuset)
	{
		p = StrSetTok(pvar->menuset, &ctx);
		while (p)
		{
			strncpy_s(name, sizeof(name), p, (int)(strchr(p, '=') - p));
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)name);
			p = StrSetTok(NULL, &ctx);
		}
	}
}

void LoadKeyList(HWND hWnd, UINT uid, LPSTR fn)
{
	LVCOLUMN lvcol;
	LVITEM item;
	HWND hList;
	char name[32];
	char *buf;
	int buf_sz;
	int i, j;
	char *ctx;
	char *p, *p2;
	UINT lang;

	lang = UILang(pvar->ts->UILanguageFile);

	hList = GetDlgItem(hWnd, uid);
	lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcol.fmt = LVCFMT_LEFT;
	lvcol.cx = 40;
	lvcol.pszText = (lang == 2) ? "キー" : "key";
	lvcol.iSubItem = 0;
	ListView_InsertColumn(hList, 0, &lvcol);
	lvcol.cx = 80;
	lvcol.pszText = (lang == 2) ? "タイプ" : "type";
	lvcol.iSubItem = 1;
	ListView_InsertColumn(hList, 1, &lvcol);
	lvcol.cx = 300;
	lvcol.pszText = (lang == 2) ? "値" : "value";
	lvcol.iSubItem = 2;
	ListView_InsertColumn(hList, 2, &lvcol);

	buf_sz = 1024;
	buf = malloc(buf_sz);
	if (!buf)
		return;

	for (i = 0; i < USERKEY_MAX; i++)
	{
		_snprintf_s(name, sizeof(name), _TRUNCATE, "User%d", i + 1);
		GetPrivateProfileString("User keys", name, "", buf, buf_sz, fn);
		if (buf[0])
		{
			p = strtok_s(buf, ",", &ctx);
			strcpy_s(name, sizeof(name), p);
			p = StrSetFindVal(pvar->codeset, name);
			if (*p)
			{
				strcpy_s(name, sizeof(name), p);
				p = strchr(name, '=');
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
			p = strtok_s(NULL, ",", &ctx);
			j = atoi(p);
			item.pszText = (char *)userkeytype[j];
			item.iSubItem = 1;
			ListView_SetItem(hList, &item);
			p = strtok_s(NULL, ",", &ctx);
			if (j == 3)
			{
				j = atoi(p);
				_snprintf_s(name, sizeof(name), _TRUNCATE, "%d", j);
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

void SetKeyMapOff(UINT code, LPSTR fn)
{
	strset_t apps, keys;
	strset_t app_ctx, keys_ctx;
	char *app, *key;

	apps = keys = NULL;
	GetIniSects(&apps, 64, 64, fn);
	app = StrSetTok(apps, &app_ctx);
	while (app)
	{
		if (_strcmpi(app, "User Keys") != 0)
		{
			GetIniKeys(app, &keys, 64, 64, fn);
			key = StrSetTok(keys, &keys_ctx);
			while (key)
			{
				if (GetIniNum(app, key, 0, fn) == code)
				{
					WritePrivateProfileString(app, key, "off", fn);
				}
				key = StrSetTok(NULL, &keys_ctx);
			}
		}
		app = StrSetTok(NULL, &app_ctx);
	}
	free(keys);
	free(apps);
}

void SaveKeyList(HWND hWnd, UINT uid, LPSTR fn)
{
	LVITEM item;
	HWND hList;
	char name[32];
	char *buf;
	int buf_sz;
	int code;
	int i, j, cnt;
	char *p;

	buf_sz = 1024;
	buf = malloc(buf_sz);
	if (!buf)
		return;

	hList = GetDlgItem(hWnd, uid);
	cnt = ListView_GetItemCount(hList);
	item.mask = LVIF_TEXT;
	item.pszText = name;
	item.cchTextMax = sizeof(name);
	for (i = 0; i < cnt; i++)
	{
		item.iItem = i;
		item.iSubItem = 0;
		ListView_GetItem(hList, &item);
		p = StrSetFindKey(pvar->codeset, name);
		p = strskip(p, '=');
		if (!(*p))
		{
			p = name;
		}
		code = atoi(p);
		_snprintf_s(buf, buf_sz, _TRUNCATE, "%s,", p);

		item.iSubItem = 1;
		ListView_GetItem(hList, &item);
		for (j = 0; j < USERKEYTYPE_CNT; j++)
		{
			if (strncmp(userkeytype[j], name, sizeof(name)) == 0)
			{
				break;
			}
		}
		_snprintf_s(name, sizeof(name), _TRUNCATE, "%d,", j);
		strcat_s(buf, buf_sz, name);

		item.iSubItem = 2;
		ListView_GetItem(hList, &item);
		if (j == 3)
		{
			j = atoi(name);
			_snprintf_s(name, sizeof(name), _TRUNCATE, "%d", j);
		}
		strcat_s(buf, buf_sz, name);
		SetKeyMapOff(code, fn);
		_snprintf_s(name, sizeof(name), _TRUNCATE, "User%d", i + 1);
		WritePrivateProfileString("User keys", name, buf, fn);
	}
	free(buf);

	for (i = cnt; i < USERKEY_MAX; i++)
	{
		_snprintf_s(name, sizeof(name), _TRUNCATE, "User%d", i + 1);
		WritePrivateProfileString("User keys", name, NULL, fn);
	}
}

///////////////////////////////////////////////////////////////

void AddKeyListItem(HWND hWnd, char *k, int type, char *s)
{
	LVITEM item;
	char name[32];
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
		if (_strcmpi(name, k) == 0)
			break;
	}
	if (i >= cnt)
	{
		item.iItem = i;
		_snprintf_s(name, sizeof(name), _TRUNCATE, k);
		ListView_InsertItem(hWnd, &item);
	}
	item.pszText = (char *)userkeytype[type];
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
	char *buf;
	int buf_sz;
	int i, cnt;

	buf_sz = 1024;
	buf = malloc(buf_sz);
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
	char *buf;
	int buf_sz;
	int i, cnt;

	buf_sz = 1024;
	buf = malloc(buf_sz);
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
	char name[32];
	char *buf;
	int buf_sz;
	int i;

	buf_sz = 1024;
	buf = malloc(buf_sz);
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
		i = atoi(buf);
		_snprintf_s(name, sizeof(name), _TRUNCATE, "%d", i);
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
	char name[32];
	char *buf;
	int buf_sz;
	int i;
	LPSTR p;

	switch (msg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(dlg, IDC_PATH, pvar->ts->KeyCnfFN);
		InitKeyListDlg(dlg, ".\\userkey.txt");
		LoadKeyList(dlg, IDC_KEYLIST, pvar->ts->KeyCnfFN);
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
				buf = malloc(buf_sz);
				if (!buf)
					return FALSE;
				i = SendMessage(GetDlgItem(dlg, IDC_COMBO_CMD), CB_GETCURSEL, 0, 0);
				if (i < 0)
				{
					GetDlgItemText(dlg, IDC_COMBO_CMD, name, sizeof(name));
					strcpy_s(buf, buf_sz, StrSetFindKey(pvar->menuset, name));
					if (!buf[0])
					{
						strcpy_s(buf, buf_sz, name);
					}
				}
				else
				{
					strcpy_s(buf, buf_sz, StrSetAt(pvar->menuset, i));
				}
				SetDlgItemText(dlg, IDC_EDIT_STR, buf);
				free(buf);
				break;
			}
			return TRUE;

		case IDC_BUTTON_SEL:
			buf_sz = MAX_PATH;
			buf = malloc(buf_sz);
			if (!buf)
				return FALSE;
			GetDlgItemText(dlg, IDC_EDIT_STR, buf, buf_sz);
			p = "マクロファイル(*.ttl)\0*.ttl\0\0";
			OpenFileDlg(dlg, IDC_EDIT_STR, "マクロファイルを選択してください", p, buf, NULL, 1);
			free(buf);
			return TRUE;

		case IDC_BUTTON_ADD:
			buf_sz = MAX_PATH;
			buf = malloc(buf_sz);
			if (!buf)
				return FALSE;
			GetDlgItemText(dlg, IDC_COMBO_KEY, name, sizeof(name));
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
			SaveKeyList(dlg, IDC_KEYLIST, pvar->ts->KeyCnfFN);
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

	lang = UILang(pvar->ts->UILanguageFile);

	pvar->SetupMenu = GetSubMenu(menu, ID_SETUP);
	s = (lang == 2) ? "ユーザーキー(&U)..." : "&UserKey setup...";
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
