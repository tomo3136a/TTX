/*
 * Tera Term Shortcut Extension
 * (C) 2021 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt_res.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <shlobj.h>

#include "compat_w95.h"
#include "ttxcommon.h"
#include "resource.h"

#define ORDER 6010

#define INISECTION "TTXShortcut"

#define ID_MENUITEM 56010

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;
	int menuoffset;

	//menu
	HMENU FileMenu;

	//status
	char UIMsg[256];

} TInstVar;

static TInstVar FAR *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	pvar->ts = ts;
	pvar->cv = cv;

	pvar->UIMsg[0] = 0;
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
// {
// 	 printf("TTXGetUIHooks %d\n", ORDER);
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
// 	 printf("TTXOpenFile %d\n", ORDER);
// }

// static void PASCAL TTXCloseFile(TTXFileHooks *hooks)
// {
// 	 printf("TTXCloseFile %d\n", ORDER);
// }

// static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks)
// {
//   printf("TTXGetSetupHooks %d\n", ORDER);
// }

// static void PASCAL TTXSetWinSize(int rows, int cols)
// {
// 	 printf("TTXSetWinSize %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

/* Type-checking macro to provide arguments for CoCreateInstance() etc.
 * The pointer arithmetic is a compile-time pointer type check that 'obj'
 * really is a 'type **', but is intended to have no effect at runtime. */
#define COMPTR(type, obj) &IID_##type,                                         \
						  (void **)(void *)((obj) +                            \
											(sizeof((obj) - (type **)(obj))) - \
											(sizeof((obj) - (type **)(obj))))

BOOL MakeShortcut(const char *name, const char *params, WORD wKey, const char *lnk)
{
	char *tt_path;
	int tt_path_sz;
	char *app_path, *app_args;
	IShellLink *pSL;
	IPersistFile *pPF;
	HRESULT res;
	int sz;
	wchar_t *ws;

	tt_path_sz = MAX_PATH;
	tt_path = malloc(tt_path_sz);
	if (!tt_path)
		return FALSE;

	/* Get Application path */
	memset(tt_path, 0, tt_path_sz);
	GetModuleFileName(NULL, tt_path, tt_path_sz - 1);

	/* Create the new item. */
	if (!SUCCEEDED(CoCreateInstance(&CLSID_ShellLink, NULL,
									CLSCTX_INPROC_SERVER,
									COMPTR(IShellLink, &pSL))))
	{
		free(tt_path);
		return FALSE;
	}

	/* Set path, parameters and icon. */
	app_path = _strdup(tt_path);
	res = pSL->lpVtbl->SetPath(pSL, app_path);
	if (res != S_OK)
	{
		pSL->lpVtbl->Release(pSL);
		free(app_path);
		free(tt_path);
		return FALSE;
	}

	app_args = _strdup(params);
	res = pSL->lpVtbl->SetArguments(pSL, app_args);
	free(app_args);
	if (res != S_OK)
	{
		pSL->lpVtbl->Release(pSL);
		free(app_path);
		free(tt_path);
		return FALSE;
	}

	res = pSL->lpVtbl->SetIconLocation(pSL, app_path, 0);
	if (res != S_OK)
	{
		pSL->lpVtbl->Release(pSL);
		free(app_path);
		free(tt_path);
		return FALSE;
	}

	if (wKey & 0x0ff)
	{
		res = pSL->lpVtbl->SetHotkey(pSL, wKey);
		if (res != S_OK)
		{
			pSL->lpVtbl->Release(pSL);
			free(app_path);
			free(tt_path);
			return FALSE;
		}
	}

	if (SUCCEEDED(pSL->lpVtbl->QueryInterface(pSL, COMPTR(IPersistFile, &pPF))))
	{
		sz = strlen(lnk) + 1;
		ws = (wchar_t *)malloc(sizeof(wchar_t) * (sz + 1));
		res = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lnk, sz, ws, sz + 1);
		res = pPF->lpVtbl->Save(pPF, ws, TRUE);
		free(ws);
		pPF->lpVtbl->Release(pPF);
		if (res != S_OK)
		{
			pSL->lpVtbl->Release(pSL);
			free(app_path);
			free(tt_path);
			return FALSE;
		}
	}

	pSL->lpVtbl->Release(pSL);
	free(app_path);
	free(tt_path);

	return TRUE;
}

///////////////////////////////////////////////////////////////
//
// ShortcutProc設定ダイアログのコールバック関数。
//
static LRESULT CALLBACK ShortcutProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPITEMIDLIST pidl;
	char *path;
	int path_sz;
	char *buf;
	int buf_sz;
	char name[64];
	UINT wKey;
	char *s;

	switch (msg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(dlg, IDC_NAME, pvar->ts->Title);
		CheckDlgButton(dlg, IDC_CHECK1, BST_CHECKED);
		SetDlgItemText(dlg, IDC_PATH1, pvar->ts->SetupFName);
		//CheckDlgButton(dlg, IDC_CHECK2, BST_CHECKED);
		SetDlgItemText(dlg, IDC_PATH2, pvar->ts->KeyCnfFN);
		//CheckDlgButton(dlg, IDC_CHECK3, BST_CHECKED);
		SetDlgItemText(dlg, IDC_PATH3, pvar->ts->LogFN);
		//CheckDlgButton(dlg, IDC_CHECK4, BST_CHECKED);
		SetDlgItemText(dlg, IDC_PATH4, pvar->ts->MacroFN);
		path_sz = MAX_PATH;
		path = malloc(path_sz);
		if (path)
		{
			if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl) == NOERROR)
			{
				SHGetPathFromIDList(pidl, path);
				CoTaskMemFree(pidl);
				SetDlgItemText(dlg, IDC_PATH5, path);
			}
			free(path);
		}
		MoveParentCenter(dlg);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK1:
			s = (IsDlgButtonChecked(dlg, IDC_CHECK1) == BST_CHECKED) ? pvar->ts->SetupFName : "";
			SetDlgItemText(dlg, IDC_PATH1, s);
			return TRUE;

		case IDC_CHECK2:
			s = (IsDlgButtonChecked(dlg, IDC_CHECK2) == BST_CHECKED) ? pvar->ts->KeyCnfFN : "";
			SetDlgItemText(dlg, IDC_PATH2, s);
			return TRUE;

		case IDC_CHECK3:
			s = (IsDlgButtonChecked(dlg, IDC_CHECK3) == BST_CHECKED) ? pvar->ts->LogFN : "";
			SetDlgItemText(dlg, IDC_PATH3, s);
			return TRUE;

		case IDC_CHECK4:
			s = (IsDlgButtonChecked(dlg, IDC_CHECK4) == BST_CHECKED) ? pvar->ts->MacroFN : "";
			SetDlgItemText(dlg, IDC_PATH4, s);
			return TRUE;

		case IDC_BUTTON2:
			path_sz = MAX_PATH;
			path = malloc(path_sz);
			if (path)
			{
				GetDlgItemText(dlg, IDC_PATH2, path, path_sz);
				s = "キーマップファイルを選択してください";
				OpenFileDlg(dlg, IDC_PATH2, s, "KeyMap(*.CNF)\0*.CNF\0", path, NULL, 0);
				free(path);
			}
			return TRUE;

		case IDC_BUTTON3:
			path_sz = MAX_PATH;
			path = malloc(path_sz);
			if (path)
			{
				GetDlgItemText(dlg, IDC_PATH3, path, path_sz);
				s = "ログファイルを選択してください";
				OpenFileDlg(dlg, IDC_PATH3, s, "Log(*.LOG)\0*.LOG\0", path, NULL, 0);
				free(path);
			}
			return TRUE;

		case IDC_BUTTON4:
			path_sz = MAX_PATH;
			path = malloc(path_sz);
			if (path)
			{
				GetDlgItemText(dlg, IDC_PATH4, path, path_sz);
				s = "マクロファイルを選択してください";
				OpenFileDlg(dlg, IDC_PATH4, s, "Macro(*.TTTL)\0*.TTL\0", path, NULL, 0);
				free(path);
			}
			return TRUE;

		case IDC_BUTTON5:
			path_sz = MAX_PATH;
			path = malloc(path_sz);
			if (path)
			{
				GetDlgItemText(dlg, IDC_PATH5, path, path_sz);
				s = "出力先を選択してください";
				OpenFolderDlg(dlg, IDC_PATH5, s, path);
				free(path);
			}
			return TRUE;

		case IDOK:
			path_sz = MAX_PATH;
			path = malloc(path_sz);
			if (!path)
				return FALSE;

			buf_sz = MAX_PATH;
			buf = malloc(buf_sz);
			if (!buf)
			{
				free(path);
				return FALSE;
			}

			buf[0] = 0;
			/* command line options */
			GetDlgItemText(dlg, IDC_OPTS, buf, buf_sz);
			if (buf[0])
			{
				strcat_s(buf, buf_sz, " ");
			}
			/* command line options(/F=setup file) */
			if (IsDlgButtonChecked(dlg, IDC_CHECK1) == BST_CHECKED)
			{
				if (pvar->ts->SetupFName[0])
				{
					strcat_s(buf, buf_sz, "/F=\"");
					strcat_s(buf, buf_sz, pvar->ts->SetupFName);
					strcat_s(buf, buf_sz, "\" ");
				}
			}
			/* command line options(/K=keymap file) */
			if (IsDlgButtonChecked(dlg, IDC_CHECK2) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_PATH2, path, path_sz);
				if (path[0])
				{
					strcat_s(buf, buf_sz, "/K=\"");
					strcat_s(buf, buf_sz, path);
					strcat_s(buf, buf_sz, "\" ");
				}
			}
			/* command line options(/L=log file) */
			if (IsDlgButtonChecked(dlg, IDC_CHECK3) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_PATH3, path, path_sz);
				if (path[0])
				{
					strcat_s(buf, buf_sz, "/L=\"");
					strcat_s(buf, buf_sz, path);
					strcat_s(buf, buf_sz, "\" ");
				}
			}
			/* command line options(/M=macro file) */
			if (IsDlgButtonChecked(dlg, IDC_CHECK4) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_PATH4, path, path_sz);
				if (path[0])
				{
					strcat_s(buf, buf_sz, "/M=\"");
					strcat_s(buf, buf_sz, path);
					strcat_s(buf, buf_sz, "\" ");
				}
			}
			/* output folder */
			GetDlgItemText(dlg, IDC_PATH5, path, path_sz);
			/* output shortcut file name */
			GetDlgItemText(dlg, IDC_NAME, name, sizeof(name));
			if (!name[0])
			{
				s = "名前がありません。";
				MessageBox(dlg, s, "Tera Term", MB_OK | MB_ICONEXCLAMATION);
				free(buf);
				free(path);
				return TRUE;
			}
			/* shortcut key */
			wKey = 0;

			CombinePath(path, path_sz, name);
			strcat_s(path, path_sz, ".lnk");
			if ((IsDlgButtonChecked(dlg, IDC_CHECK5) != BST_CHECKED) && FileExists(path))
			{
				s = "既にファイルが存在しています。";
				MessageBox(dlg, s, "Tera Term", MB_OK | MB_ICONEXCLAMATION);
				free(buf);
				free(path);
				return TRUE;
			}

			if (MakeShortcut(name, buf, wKey, path))
			{
				s = "ショートカット %s を作成しました。\n\n作成先： %s\n";
				_snprintf_s(pvar->UIMsg, sizeof(pvar->UIMsg), _TRUNCATE, s, name, path);
			}
			else
			{
				s = "ショートカット %s を作成出来ませんでした。";
				_snprintf_s(pvar->UIMsg, sizeof(pvar->UIMsg), _TRUNCATE, s, name);
				MessageBox(dlg, pvar->UIMsg, "Tera Term", MB_OK | MB_ICONEXCLAMATION);
				free(buf);
				free(path);
				return TRUE;
			}
			free(buf);
			free(path);

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
	pvar->menuoffset = MenuOffset(INISECTION, ID_MENUITEM, 0);

	pvar->FileMenu = GetSubMenu(menu, ID_FILE);

	s = (lang == 2) ? "ショートカット作成(&M)..." : "&Make shortcut...";
	InsertMenu(pvar->FileMenu, ID_FILE_PRINT2, MF_BYCOMMAND, ID_MENUITEM + pvar->menuoffset, s);
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXModifyPopupMenu(HMENU menu)
// {
//   printf("TTXModifyPopupMenu %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (cmd + pvar->menuoffset)
	{
	case ID_MENUITEM:
		switch (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_SHORTCUT),
							   hWin, ShortcutProc, (LPARAM)NULL))
		{
		case IDOK:
			MessageBox(hWin, pvar->UIMsg, "Tera Term", MB_OK | MB_ICONINFORMATION);
			pvar->UIMsg[0] = 0;
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
