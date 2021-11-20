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
#include <tchar.h>

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

	//menu
	HMENU FileMenu;

	//status
	TCHAR UIMsg[256];

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

BOOL MakeShortcut(const PTCHAR name, const PTCHAR params, WORD wKey, const PTCHAR lnk)
{
	PTCHAR tt_path;
	int tt_path_sz;
	PTCHAR app_path;
	PTCHAR app_args;
	IShellLink *pSL;
	IPersistFile *pPF;
	HRESULT res;
	int sz;
#ifndef _UNICODE
	wchar_t *ws;
#endif

	tt_path_sz = MAX_PATH;
	tt_path = malloc(tt_path_sz*sizeof(TCHAR));
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
	app_path = _tcsdup(tt_path);
	res = pSL->lpVtbl->SetPath(pSL, app_path);
	if (res != S_OK)
	{
		pSL->lpVtbl->Release(pSL);
		free(app_path);
		free(tt_path);
		return FALSE;
	}

	app_args = _tcsdup(params);
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
		sz = _tcslen(lnk) + 1;
#ifdef _UNICODE
		res = pPF->lpVtbl->Save(pPF, lnk, TRUE);
#else
		ws = (wchar_t *)malloc(sizeof(wchar_t) * (sz + 1));
		res = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lnk, sz, ws, sz + 1);
		res = pPF->lpVtbl->Save(pPF, ws, TRUE);
		free(ws);
#endif /* _UNICODE */
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
	PTCHAR path;
	int path_sz;
	PTCHAR buf;
	int buf_sz;
	LPTSTR p;
	TCHAR name[64];
	UINT wKey;
	PCHAR s;
	PTCHAR ws;

	switch (msg)
	{
	case WM_INITDIALOG:
		SetDlgItemTextA(dlg, IDC_NAME, pvar->ts->Title);
		CheckDlgButton(dlg, IDC_CHECK1, BST_CHECKED);
		SetDlgItemTextA(dlg, IDC_PATH1, pvar->ts->SetupFName);
		//CheckDlgButton(dlg, IDC_CHECK2, BST_CHECKED);
		SetDlgItemTextA(dlg, IDC_PATH2, pvar->ts->KeyCnfFN);
		//CheckDlgButton(dlg, IDC_CHECK3, BST_CHECKED);
		SetDlgItemTextA(dlg, IDC_PATH3, pvar->ts->LogFN);
		//CheckDlgButton(dlg, IDC_CHECK4, BST_CHECKED);
		SetDlgItemTextA(dlg, IDC_PATH4, pvar->ts->MacroFN);
		path_sz = MAX_PATH;
		path = (PTCHAR)malloc(path_sz*sizeof(TCHAR));
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
			SetDlgItemTextA(dlg, IDC_PATH1, s);
			return TRUE;

		case IDC_CHECK2:
			s = (IsDlgButtonChecked(dlg, IDC_CHECK2) == BST_CHECKED) ? pvar->ts->KeyCnfFN : "";
			SetDlgItemTextA(dlg, IDC_PATH2, s);
			return TRUE;

		case IDC_CHECK3:
			s = (IsDlgButtonChecked(dlg, IDC_CHECK3) == BST_CHECKED) ? pvar->ts->LogFN : "";
			SetDlgItemTextA(dlg, IDC_PATH3, s);
			return TRUE;

		case IDC_CHECK4:
			s = (IsDlgButtonChecked(dlg, IDC_CHECK4) == BST_CHECKED) ? pvar->ts->MacroFN : "";
			SetDlgItemTextA(dlg, IDC_PATH4, s);
			return TRUE;

		case IDC_BUTTON2:
			path_sz = MAX_PATH;
			path = (PTCHAR)malloc(path_sz*sizeof(TCHAR));
			if (path)
			{
				GetDlgItemText(dlg, IDC_PATH2, path, path_sz);
				ws = _T("キーマップファイルを選択してください");
				OpenFileDlg(dlg, IDC_PATH2, ws, _T("KeyMap(*.CNF)\0*.CNF\0"), path, NULL, 0);
				free(path);
			}
			return TRUE;

		case IDC_BUTTON3:
			path_sz = MAX_PATH;
			path = (PTCHAR)malloc(path_sz*sizeof(TCHAR));
			if (path)
			{
				GetDlgItemText(dlg, IDC_PATH3, path, path_sz);
				ws = _T("ログファイルを選択してください");
				OpenFileDlg(dlg, IDC_PATH3, ws, _T("Log(*.LOG)\0*.LOG\0"), path, NULL, 0);
				free(path);
			}
			return TRUE;

		case IDC_BUTTON4:
			path_sz = MAX_PATH;
			path = (PTCHAR)malloc(path_sz*sizeof(TCHAR));
			if (path)
			{
				GetDlgItemText(dlg, IDC_PATH4, path, path_sz);
				ws = _T("マクロファイルを選択してください");
				OpenFileDlg(dlg, IDC_PATH4, ws, _T("Macro(*.TTTL)\0*.TTL\0"), path, NULL, 0);
				free(path);
			}
			return TRUE;

		case IDC_BUTTON5:
			path_sz = MAX_PATH;
			path = (PTCHAR)malloc(path_sz*sizeof(TCHAR));
			if (path)
			{
				GetDlgItemText(dlg, IDC_PATH5, path, path_sz);
				ws = _T("出力先を選択してください");
				OpenFolderDlg(dlg, IDC_PATH5, ws, path);
				free(path);
			}
			return TRUE;

		case IDOK:
			path_sz = MAX_PATH;
			path = (PTCHAR)malloc(path_sz*sizeof(TCHAR));
			if (!path)
				return FALSE;

			buf_sz = MAX_PATH;
			buf = (PTCHAR)malloc(buf_sz*sizeof(TCHAR));
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
				_tcscat_s(buf, buf_sz, _T(" "));
			}
			/* command line options(/F=setup file) */
			if (IsDlgButtonChecked(dlg, IDC_CHECK1) == BST_CHECKED)
			{
				p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
				if (p[0])
				{
					_tcscat_s(buf, buf_sz, _T("/F=\""));
					_tcscat_s(buf, buf_sz, p);
					_tcscat_s(buf, buf_sz, _T("\" "));
				}
				TTXFree(p);
			}
			/* command line options(/K=keymap file) */
			if (IsDlgButtonChecked(dlg, IDC_CHECK2) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_PATH2, path, path_sz);
				if (path[0])
				{
					_tcscat_s(buf, buf_sz, _T("/K=\""));
					_tcscat_s(buf, buf_sz, path);
					_tcscat_s(buf, buf_sz, _T("\" "));
				}
			}
			/* command line options(/L=log file) */
			if (IsDlgButtonChecked(dlg, IDC_CHECK3) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_PATH3, path, path_sz);
				if (path[0])
				{
					_tcscat_s(buf, buf_sz, _T("/L=\""));
					_tcscat_s(buf, buf_sz, path);
					_tcscat_s(buf, buf_sz, _T("\" "));
				}
			}
			/* command line options(/M=macro file) */
			if (IsDlgButtonChecked(dlg, IDC_CHECK4) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_PATH4, path, path_sz);
				if (path[0])
				{
					_tcscat_s(buf, buf_sz, _T("/M=\""));
					_tcscat_s(buf, buf_sz, path);
					_tcscat_s(buf, buf_sz, _T("\" "));
				}
			}
			/* output folder */
			GetDlgItemText(dlg, IDC_PATH5, path, path_sz);
			/* output shortcut file name */
			GetDlgItemText(dlg, IDC_NAME, name, sizeof(name));
			if (!name[0])
			{
				ws = _T("名前がありません。");
				MessageBox(dlg, ws, _T("Tera Term"), MB_OK | MB_ICONEXCLAMATION);
				free(buf);
				free(path);
				return TRUE;
			}
			/* shortcut key */
			wKey = 0;

			CombinePath(path, path_sz, name);
			_tcscat_s(path, path_sz, _T(".lnk"));
			if ((IsDlgButtonChecked(dlg, IDC_CHECK5) != BST_CHECKED) && FileExists(path))
			{
				ws = _T("既にファイルが存在しています。");
				MessageBox(dlg, ws, _T("Tera Term"), MB_OK | MB_ICONEXCLAMATION);
				free(buf);
				free(path);
				return TRUE;
			}

			if (MakeShortcut(name, buf, wKey, path))
			{
				ws = _T("ショートカット %s を作成しました。\n\n作成先： %s\n");
				_sntprintf_s(pvar->UIMsg, sizeof(pvar->UIMsg)/sizeof(pvar->UIMsg[0]), 
					_TRUNCATE, ws, name, path);
			}
			else
			{
				ws = _T("ショートカット %s を作成出来ませんでした。");
				_sntprintf_s(pvar->UIMsg, sizeof(pvar->UIMsg)/sizeof(pvar->UIMsg[0]), 
					_TRUNCATE, ws, name);
				MessageBox(dlg, pvar->UIMsg, _T("Tera Term"), MB_OK | MB_ICONEXCLAMATION);
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
	LPTSTR s;

	lang = UILang(pvar->ts->UILanguageFile);

	pvar->FileMenu = GetSubMenu(menu, ID_FILE);

	s = (lang == 2) ? _T("ショートカット作成(&M)...") : _T("&Make shortcut...");
	InsertMenu(pvar->FileMenu, ID_FILE_PRINT2, MF_BYCOMMAND, TTXMenuID(ID_MENUITEM), s);
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXModifyPopupMenu(HMENU menu)
// {
//   printf("TTXModifyPopupMenu %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (TTXMenuOrgID(cmd))
	{
	case ID_MENUITEM:
		switch (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_SHORTCUT),
							   hWin, ShortcutProc, (LPARAM)NULL))
		{
		case IDOK:
			MessageBox(hWin, pvar->UIMsg, _T("Tera Term"), MB_OK | MB_ICONINFORMATION);
			pvar->UIMsg[0] = 0;
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
		TTX_DLL_PROCESS_ATTACH();
		hInst = hInstance;
		pvar = &InstVar;
		break;
	case DLL_PROCESS_DETACH:
		/* do process cleanup */
		break;
	}
	return TRUE;
}
