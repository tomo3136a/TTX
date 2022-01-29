/*
 * Tera Term Environment Extension
 * (C) 2021 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#ifndef TT4
#include "tttypes_key.h"
#endif /* not TT4 */
#include "ttplugin.h"
#include "tt_res.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

#include "ttxcommon.h"
#include "resource.h"

#define ORDER 6040

#define INISECTION "TTXEnv"
#define TTSECTION "Tera Term"

#define MENUITEM_NUM 7
#define ID_MENUITEM 56040
#define ID_MENUITEM0 (ID_MENUITEM + MENUITEM_NUM)
#define ID_MENUITEM1 (ID_MENUITEM0 + 1)
#define ID_MENUITEM2 (ID_MENUITEM0 + 2)

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct
{
	PTTSet ts;
	PComVar cv;
	BOOL skip;
	HWND hwnd;

	//menu
	HMENU SetupMenu;
	HMENU EnvMenu;

	// original callback
	PGetHostName origGetHostName;
	PReadIniFile origReadIniFile;
	PWriteIniFile origWriteIniFile;
	PParseParam origParseParam;

	//status
	BOOL UseEnv;
	BOOL EnableEnv;
	BOOL OverEnv;
	BOOL UseKeyCnf;
	LPTSTR SetupFile;
	LPTSTR SetupDir;
	LPTSTR ttpath;

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
	pvar->hwnd = cv->HWin;

	pvar->UseKeyCnf = FALSE;
	pvar->UseEnv = FALSE;
	pvar->EnableEnv = FALSE;
	pvar->OverEnv = FALSE;
	pvar->SetupFile = NULL;
	pvar->SetupDir = NULL;
	pvar->ttpath = TTXGetModuleFileName(NULL);
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXOpenTCP(TTXSockHooks *hooks)
// {
// 	 printf("TTXOpenTCP %d\n", ORDER);
// }

// static void PASCAL TTXCloseTCP(TTXSockHooks *hooks)
// {
// 	 printf("TTXCloseTCP %d\n", ORDER);
// }

// static void PASCAL TTXOpenFile(TTXFileHooks *hooks) {
//   printf("TTXOpenFile %d\n", ORDER);
// }

// static void PASCAL TTXCloseFile(TTXFileHooks *hooks) {
//   printf("TTXCloseFile %d\n", ORDER);
// }

static BOOL PASCAL TTXGetHostName(HWND WndParent, PGetHNRec GetHNRec)
{
	if (pvar->hwnd == 0)
		pvar->hwnd = WndParent;
	return (pvar->origGetHostName)(WndParent, GetHNRec);
}

static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
{
	pvar->origGetHostName = *hooks->GetHostName;
	*hooks->GetHostName = TTXGetHostName;
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXReadIniFile(TT_LPCTSTR fn, PTTSet ts)
{
	TCHAR buf[32];
	size_t path_sz;
	LPTSTR path;
	LPTSTR p;

	if (!pvar->skip)
		(pvar->origReadIniFile)(fn, ts);

	GetPrivateProfileString(_T(INISECTION), _T("KeyCnf"), _T(""), 
		buf, sizeof(buf)/sizeof(buf[0]), fn);
	pvar->UseKeyCnf = (buf[0]) ? TRUE : FALSE;
	if (pvar->UseKeyCnf)
	{
		p  = buf;
		path_sz = _tcsnlen(p, _TRUNCATE) + _tcsnlen(fn, _TRUNCATE) + 2;
		path = (LPTSTR)malloc(path_sz*sizeof(TCHAR));
		GetAbsolutePath(path, path_sz, p, fn);
		TTXSetPath(ts, ID_KEYCNFNM, path);
		free(path);
		if (pvar->hwnd == 0)
			pvar->hwnd = pvar->cv->HWin;
		PostMessage(pvar->hwnd, WM_USER_ACCELCOMMAND, IdCmdLoadKeyMap, 0);
	}

	pvar->EnableEnv = GetIniOnOff(_T(INISECTION), _T("EnableEnv"), FALSE, fn);
	if (!pvar->OverEnv)
	{
		pvar->UseEnv = pvar->EnableEnv;
		path_sz = _tcsnlen(fn, _TRUNCATE) + 1;
		path = (LPTSTR)malloc(path_sz*sizeof(TCHAR));
		_tcscpy_s(path, path_sz, fn);
		if (pvar->SetupFile)
		{
			free(pvar->SetupFile);
			pvar->SetupFile = NULL;
		}
		pvar->SetupFile = path;
	
		p = NULL;
		GetIniString(_T(INISECTION), _T("SetupDir"), _T(""), &p, 128, 64, fn);
		path_sz = _tcsnlen(p, _TRUNCATE) + _tcsnlen(fn, _TRUNCATE) + 2;
		path = (LPTSTR)malloc(path_sz*sizeof(TCHAR));
		GetAbsolutePath(path, path_sz, p, fn);
		TTXFree(&p);
		if (pvar->SetupDir)
		{
			free(pvar->SetupDir);
			pvar->SetupDir = NULL;
		}
		pvar->SetupDir = path;
	}
	pvar->OverEnv = FALSE;
}

static void PASCAL TTXWriteIniFile(TT_LPCTSTR fn, PTTSet ts)
{
	size_t path_sz;
	LPTSTR path;
	LPTSTR p1;
	LPTSTR p2;

	(pvar->origWriteIniFile)(fn, ts);

	path = NULL;
	if (pvar->UseKeyCnf)
	{
		p1 = TTXGetPath(pvar->ts, ID_KEYCNFNM);
		p2 = TTXGetPath(pvar->ts, ID_SETUPFNAME);
		path_sz = _tcsnlen(p1, _TRUNCATE) + _tcsnlen(p2, _TRUNCATE) + 2;
		path = (LPTSTR)malloc(path_sz*sizeof(TCHAR));
		GetRelatedPath(path, path_sz, p1, p2, 0);
		TTXFree(&p1);
		TTXFree(&p2);
	}
	WritePrivateProfileString(_T(INISECTION), _T("KeyCnf"), path, fn);
	free(path);

	path = NULL;
	if (pvar->EnableEnv)
	{
		p1 = pvar->SetupDir;
		p2 = TTXGetPath(pvar->ts, ID_SETUPFNAME);
		path_sz = _tcsnlen(p1, _TRUNCATE) + _tcsnlen(p2, _TRUNCATE) + 2;
		path = (LPTSTR)malloc(path_sz*sizeof(TCHAR));
		GetRelatedPath(path, path_sz, p1, p2, 0);
		TTXFree(&p2);
	}
	WritePrivateProfileString(_T(INISECTION), _T("SetupDir"), path, fn);
	free(path);

	WriteIniOnOff(_T(INISECTION), _T("EnableEnv"), pvar->EnableEnv, pvar->EnableEnv, fn);
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

///////////////////////////////////////////////////////////////
//
// EnvSettingProc設定ダイアログのコールバック関数。
//
// static LRESULT CALLBACK EnvSettingProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
// {
// 	BOOL flg;
// 	char buf[MAX_PATH];
// 	PCHAR p;

// 	switch (msg)
// 	{
// 	case WM_INITDIALOG:
// 		SendMessage(GetDlgItem(dlg, IDC_CHECK1), BM_SETCHECK,
// 					pvar->EnableEnv ? BST_CHECKED : BST_UNCHECKED, 0);
// 		SetDlgItemText(dlg, IDC_PATH1, pvar->SetupDir);
// 		SetDlgItemText(dlg, IDC_PATH2, pvar->ts->KeyCnfFN);
// 		EnableWindow(GetDlgItem(dlg, IDC_PATH1), pvar->EnableEnv);
// 		EnableWindow(GetDlgItem(dlg, IDC_BUTTON1), pvar->EnableEnv);
// 		MoveParentCenter(dlg);
// 		return TRUE;

// 	case WM_COMMAND:
// 		switch (LOWORD(wParam))
// 		{
// 		case IDC_CHECK1:
// 			flg = (IsDlgButtonChecked(dlg, IDC_CHECK1) == BST_CHECKED) ? TRUE : FALSE;
// 			EnableWindow(GetDlgItem(dlg, IDC_PATH1), flg);
// 			EnableWindow(GetDlgItem(dlg, IDC_BUTTON1), flg);
// 			p = (pvar->SetupDir[0]) ? pvar->SetupDir : pvar->ts->SetupFName;
// 			SetDlgItemText(dlg, IDC_PATH1, p);
// 			return TRUE;

// 		case IDC_BUTTON1:
// 			GetDlgItemText(dlg, IDC_PATH1, buf, sizeof(buf));
// 			OpenFolderDlg(dlg, IDC_PATH1, "環境フォルダを選択してください", buf, 0);
// 			return TRUE;

// 		case IDC_BUTTON2:
// 			GetDlgItemText(dlg, IDC_PATH2, buf, sizeof(buf));
// 			p = "キーボード設定ファイル(*.cnf)\0*.cnf\0\0";
// 			OpenFileDlg(dlg, IDC_PATH2, "キーマップファイル", p, buf, 0);
// 			return TRUE;

// 		case IDOK:
// 			GetDlgItemText(dlg, IDC_PATH2, pvar->ts->KeyCnfFN, sizeof(pvar->ts->KeyCnfFN));
// 			flg = (IsDlgButtonChecked(dlg, IDC_CHECK1) == BST_CHECKED) ? TRUE : FALSE;
// 			pvar->EnableEnv = flg;
// 			pvar->UseEnv = flg ? flg : pvar->UseEnv;
// 			if (flg)
// 			{
// 				GetDlgItemText(dlg, IDC_PATH1, pvar->SetupDir, sizeof(pvar->SetupDir));
// 			}
// 			EndDialog(dlg, IDOK);
// 			return TRUE;

// 		case IDCANCEL:
// 			EndDialog(dlg, IDCANCEL);
// 			return TRUE;
// 		}
// 		break;
// 	}

// 	return FALSE;
// }

///////////////////////////////////////////////////////////////

// src_baseを基準としたsrcのパスをdst_baseのフォルダにコピーする。
// ファイルコピーしたら TRUE を返す
BOOL CopyPathEnv(LPTSTR dst, int sz, LPTSTR src, BOOL force, LPCTSTR src_base, LPCTSTR dst_base)
{
	TCHAR path[MAX_PATH];
	TCHAR tmp[MAX_PATH];
	LPTSTR name;

	GetAbsolutePath(path, sizeof(path)/sizeof(path[0]), src, src_base);
	name = FindFileName(path);
	GetAbsolutePath(dst, sz, name, dst_base);
	if (_tcsicmp(path, dst) != 0)
	{
		if (FileExists(dst) && !force)
		{
			LPCTSTR s = _T("ファイルが存在しています。上書きしますか。\r%s");
			_sntprintf_s(tmp, sizeof(tmp)/sizeof(tmp[0]), _TRUNCATE, s, name);
			if (MessageBox(0, tmp, _T("WARNING"), MB_YESNO | MB_ICONWARNING) != IDYES)
			{
				_tcscpy_s(dst, sz, path);
				return FALSE;
			}
		}
		CopyFile(path, dst, FALSE);
		return TRUE;
	}
	return FALSE;
}

//keyconfファイルに定義したユーザーキーがマクロの場合、
//fnがあるパスに複製しkeyconfファイルの内容も置き換える
void CopyUserKeyMacro(LPTSTR keycnf, LPCTSTR fn)
{
	TCHAR name[64];
	TCHAR buf[MAX_PATH];
	TCHAR dst[MAX_PATH];
	LPTSTR p;
	int i;

	for (i = 1; i < IdUser1; i++)
	{
		_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("User%d"), i);
		GetPrivateProfileString(_T("User keys"), name, _T(""), buf, sizeof(buf)/sizeof(buf[0]), keycnf);
		if (!buf[0])
			break;
		p = _tcschr(buf, _T(','));
		if (!p || _tstoi(p + 1) != 2)
			continue;
		p = _tcschr(p + 1, _T(','));
		if (!p)
			continue;
		if (CopyPathEnv(dst, sizeof(dst)/sizeof(dst[0]), ++p, FALSE, pvar->ttpath, fn))
		{
			GetRelatedPath(p, buf + sizeof(buf)/sizeof(buf[0]) - p, dst, pvar->ttpath, 0);
			WritePrivateProfileString(_T("User keys"), name, buf, keycnf);
		}
	}
}

//
// EnvCopyProc設定ダイアログのコールバック関数。
//
static LRESULT CALLBACK EnvCopyProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[MAX_PATH];
	TCHAR path[MAX_PATH];
	TCHAR fn[MAX_PATH];
	LPTSTR p;
	LPTSTR s;

	switch (msg)
	{
	case WM_INITDIALOG:
		GetAbsolutePath(buf, sizeof(buf)/sizeof(buf[0]), pvar->SetupDir, pvar->SetupFile);
		SetDlgItemText(dlg, IDC_PATH1, buf);
		p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
		GetPathName(buf, sizeof(buf)/sizeof(buf[0]), p);
		TTXFree(&p);
		SetDlgItemText(dlg, IDC_PATH2, buf);
		p = TTXGetPath(pvar->ts, ID_KEYCNFNM);
		GetPathName(buf, sizeof(buf)/sizeof(buf[0]), p);
		TTXFree(&p);
		SetDlgItemText(dlg, IDC_PATH3, buf);
		CheckDlgButton(dlg, IDC_CHECK2, BST_CHECKED);
		CheckDlgButton(dlg, IDC_CHECK3, BST_CHECKED);
		CheckDlgButton(dlg, IDC_CHECK5, BST_CHECKED);
		MoveParentCenter(dlg);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case MAKEWPARAM(IDC_NAME, EN_CHANGE):
			if (IsDlgButtonChecked(dlg, IDC_CHECK1) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_NAME, buf, sizeof(buf)/sizeof(buf[0]));
				if (buf[0])
				{
					_tcscat_s(buf, sizeof(buf)/sizeof(buf[0]), _T(".INI"));
					SetDlgItemText(dlg, IDC_PATH2, buf);
					RemoveFileExt(buf);
					_tcscat_s(buf, sizeof(buf)/sizeof(buf[0]), _T(".CNF"));
					SetDlgItemText(dlg, IDC_PATH3, buf);
				}
				else
				{
					p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
					GetPathName(buf, sizeof(buf)/sizeof(buf[0]), p);
					TTXFree(&p);
					SetDlgItemText(dlg, IDC_PATH2, buf);
					p = TTXGetPath(pvar->ts, ID_KEYCNFNM);
					GetPathName(buf, sizeof(buf)/sizeof(buf[0]), p);
					TTXFree(&p);
					SetDlgItemText(dlg, IDC_PATH3, buf);
				}
			}
			return TRUE;
		}
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			//GetDlgItemText(dlg, IDC_PATH1, buf, sizeof(buf)/sizeof(buf[0]));
			s = _T("複製先フォルダを選択してください");
			OpenFolderDlg(dlg, IDC_PATH1, s, NULL, 0);
			return TRUE;

		case IDC_CHECK1:
			if (IsDlgButtonChecked(dlg, IDC_CHECK1) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_NAME, buf, sizeof(buf)/sizeof(buf[0]));
				if (buf[0])
				{
					_tcscat_s(buf, sizeof(buf)/sizeof(buf[0]), _T(".INI"));
					SetDlgItemText(dlg, IDC_PATH2, buf);
					RemoveFileExt(buf);
					_tcscat_s(buf, sizeof(buf)/sizeof(buf[0]), _T(".CNF"));
					SetDlgItemText(dlg, IDC_PATH3, buf);
				}
				else
				{
					p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
					GetPathName(buf, sizeof(buf)/sizeof(buf[0]), p);
					TTXFree(&p);
					SetDlgItemText(dlg, IDC_PATH2, buf);
					p = TTXGetPath(pvar->ts, ID_KEYCNFNM);
					GetPathName(buf, sizeof(buf)/sizeof(buf[0]), p);
					TTXFree(&p);
					SetDlgItemText(dlg, IDC_PATH3, buf);
				}
				EnableWindow(GetDlgItem(dlg, IDC_PATH2), FALSE);
				EnableWindow(GetDlgItem(dlg, IDC_PATH3), FALSE);
			}
			else
			{
				EnableWindow(GetDlgItem(dlg, IDC_PATH2), TRUE);
				EnableWindow(GetDlgItem(dlg, IDC_PATH3), TRUE);
			}
			return TRUE;

		case IDOK:
			GetDlgItemText(dlg, IDC_PATH1, buf, sizeof(buf)/sizeof(buf[0]));
			GetAbsolutePath(fn, sizeof(fn)/sizeof(fn[0]), buf, pvar->SetupFile);
			if (!FileExists(fn))
			{
				if (CreateDirectory(fn, NULL) == 0)
				{
					s = _T("設定ファイルを作成できません。");
					MessageBox(dlg, s, _T("WARNING"), MB_OK | MB_ICONWARNING);
					return FALSE;
				}
			}
			GetDlgItemText(dlg, IDC_PATH2, buf, sizeof(buf)/sizeof(buf[0]));
			if (!buf[0])
			{
				s = _T("設定ファイルのパスを設定してください。");
				MessageBox(dlg, s, _T("WARNING"), MB_OK | MB_ICONWARNING);
				return FALSE;
			}
			CombinePath(fn, sizeof(fn)/sizeof(fn[0]), buf);
			if (FileExists(fn))
			{
				s = _T("設定ファイルが存在しています。上書きしますか。");
				if (MessageBox(dlg, s, _T("WARNING"), MB_YESNO | MB_ICONWARNING) != IDYES)
				{
					return FALSE;
				}
			}
			p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
			CopyFile(p, fn, FALSE);
			TTXFree(&p);
			WritePrivateProfileString(_T(INISECTION), _T("EnableEnv"), NULL, fn);
			WritePrivateProfileString(_T(INISECTION), _T("SetupDir"), NULL, fn);
			WritePrivateProfileString(_T(INISECTION), _T("KeyCnf"), NULL, fn);

			// set title
			if (IsDlgButtonChecked(dlg, IDC_CHECK2) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_NAME, buf, sizeof(buf)/sizeof(buf[0]));
				if (buf[0])
				{
					WritePrivateProfileString(_T(TTSECTION), _T("Title"), buf, fn);
				}
			}

			if (IsDlgButtonChecked(dlg, IDC_CHECK5) == BST_CHECKED)
			{
				pvar->OverEnv = TRUE;
			}

			// set environment master
			if (IsDlgButtonChecked(dlg, IDC_CHECK4) == BST_CHECKED)
			{
				WriteIniOnOff(_T(INISECTION), _T("EnableEnv"), TRUE, TRUE, fn);
				if (pvar->OverEnv)
				{
					pvar->EnableEnv = TRUE;
					pvar->UseEnv = TRUE;
					if (pvar->SetupFile)
					{
						free(pvar->SetupFile);
						pvar->SetupFile = NULL;
					}
					pvar->SetupFile = _tcsdup(fn);
				}
			}

			// set startup macro
			p = TTXGetPath(pvar->ts, ID_MACROFN);
			if (p[0] &&
				(IsDlgButtonChecked(dlg, IDC_CHECK3) == BST_CHECKED) &&
				CopyPathEnv(buf, sizeof(buf)/sizeof(buf[0]), p, FALSE, pvar->ttpath, fn))
			{
				GetRelatedPath(path, sizeof(path)/sizeof(path[0]), buf, pvar->ttpath, 0);
				WritePrivateProfileString(_T(TTSECTION), _T("StartupMacro"), path, fn);
			}
			TTXFree(&p);

			// copy keymap file
			GetDlgItemText(dlg, IDC_PATH3, buf, sizeof(buf));
			if (buf[0])
			{
				GetAbsolutePath(path, sizeof(path), buf, fn);
				LPTSTR s = _T("キーマップファイルが存在しています。上書きしますか。");
				if (!FileExists(path) ||
					(MessageBox(dlg, s, _T("WARNING"), MB_YESNO | MB_ICONWARNING) == IDYES))
				{
					p = TTXGetPath(pvar->ts, ID_KEYCNFNM);
					CopyFile(p, path, FALSE);
					TTXFree(&p);
				}
				GetRelatedPath(buf, sizeof(buf)/sizeof(buf[0]), path, fn, 2);
				WritePrivateProfileString(_T(INISECTION), _T("KeyCnf"), buf, fn);
				if (IsDlgButtonChecked(dlg, IDC_CHECK3) == BST_CHECKED)
				{
					CopyUserKeyMacro(path, fn);
				}
			}
			if (pvar->OverEnv)
			{
				TTXSetPath(pvar->ts, ID_SETUPFNAME, fn);
			}
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
	MENUITEMINFO mi;
	UINT flag;
	UINT lang;
	LPTSTR s;

	flag = MF_ENABLED;
	lang = UILang(pvar->ts->UILanguageFile);

	pvar->SetupMenu = GetSubMenu(menu, ID_SETUP);
	s = (lang == 2) ? _T("環境の保存(&V)...") : _T("En&vironment copy...");
	AppendMenu(pvar->SetupMenu, flag, TTXMenuID(ID_MENUITEM2), s);
	// s = (lang == 2) ? "環境の設定(&M)..." : "Environ&ment setup...";
	// AppendMenu(pvar->SetupMenu, flag, TTXMenuID(ID_MENUITEM1), s);

	if (pvar->EnvMenu != NULL)
	{
		DestroyMenu(pvar->EnvMenu);
		pvar->EnvMenu = NULL;
	}
	if (pvar->UseEnv)
	{
		pvar->EnvMenu = CreateMenu();
		memset(&mi, 0, sizeof(MENUITEMINFO));
		mi.cbSize = sizeof(MENUITEMINFO);
		mi.fMask = MIIM_TYPE | MIIM_SUBMENU;
		mi.fType = MFT_STRING;
		mi.hSubMenu = pvar->EnvMenu;
		mi.dwTypeData = (lang == 2) ? _T("環境(&A)") : _T("Environment");
		InsertMenuItem(menu, ID_HELPMENU, TRUE, &mi);
	}
}

static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
	HANDLE hFind;
	HANDLE hFind2;
	WIN32_FIND_DATA win32fd;
	WIN32_FIND_DATA win32fd2;
	TCHAR path[MAX_PATH];
	TCHAR name[32];
	UINT uflg;
	UINT uid;
	LPTSTR p;

	if (menu == pvar->EnvMenu)
	{
		while (DeleteMenu(menu, 0, MF_BYPOSITION) != 0)
		{
		}

		uid = TTXMenuID(ID_MENUITEM);
		p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
		uflg = MF_ENABLED | ((_tcsnicmp(p, pvar->SetupFile, MAX_PATH) == 0) ? MF_CHECKED : 0);
		TTXFree(&p);
		GetRelatedPath(path, sizeof(path)/sizeof(path[0]), pvar->SetupFile, pvar->SetupFile, 0);
		RemoveFileExt(path);
		_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("&0 + %s"), path);
		AppendMenu(menu, uflg, uid++, name);

		GetAbsolutePath(path, sizeof(path)/sizeof(path[0]), pvar->SetupDir, pvar->SetupFile);
		CombinePath(path, sizeof(path)/sizeof(path[0]), _T("*.INI"));
		hFind = FindFirstFile(path, &win32fd);
		if (hFind == INVALID_HANDLE_VALUE)
			return;
		do
		{
			if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			if (win32fd.cFileName[0] == _T('_') || win32fd.cFileName[0] == _T('.'))
				continue;
			GetAbsolutePath(path, sizeof(path)/sizeof(path[0]), pvar->SetupDir, pvar->SetupFile);
			CombinePath(path, sizeof(path)/sizeof(path[0]), win32fd.cFileName);
			if (_tcsnicmp(path, pvar->SetupFile, MAX_PATH) == 0)
				continue;
			if (uid == (TTXMenuID(ID_MENUITEM + MENUITEM_NUM)))
			{
				AppendMenu(menu, MF_ENABLED, uid, _T("more..."));
				break;
			}
			p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
			uflg = MF_ENABLED | ((_tcsnicmp(p, path, MAX_PATH) == 0) ? MF_CHECKED : 0);
			TTXFree(&p);
			GetRelatedPath(path, sizeof(path)/sizeof(path[0]), path, pvar->SetupFile, 0);
			RemoveFileExt(path);
			_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("&%d + %s"), TTXMenuOrgID(uid - ID_MENUITEM), path);
			AppendMenu(menu, uflg, uid++, name);
		} while (FindNextFile(hFind, &win32fd));
		FindClose(hFind);

		GetAbsolutePath(path, sizeof(path)/sizeof(path[0]), pvar->SetupDir, pvar->SetupFile);
		CombinePath(path, sizeof(path)/sizeof(path[0]), _T("*"));
		hFind2 = FindFirstFile(path, &win32fd2);
		if (hFind2 == INVALID_HANDLE_VALUE)
			return;
		do
		{
			if (!(win32fd2.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				continue;
			if (win32fd2.cFileName[0] == _T('_') || win32fd2.cFileName[0] == _T('.'))
				continue;
			GetAbsolutePath(path, sizeof(path)/sizeof(path[0]), pvar->SetupDir, pvar->SetupFile);
			CombinePath(path, sizeof(path)/sizeof(path[0]), win32fd2.cFileName);
			CombinePath(path, sizeof(path)/sizeof(path[0]), _T("*.INI"));
			hFind = FindFirstFile(path, &win32fd);
			if (hFind == INVALID_HANDLE_VALUE)
				return;
			do
			{
				if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					continue;
				if (win32fd2.cFileName[0] == _T('_') || win32fd2.cFileName[0] == _T('.'))
					continue;
				if (uid == TTXMenuID(ID_MENUITEM + MENUITEM_NUM))
				{
					AppendMenu(menu, MF_ENABLED, uid, _T("more..."));
					break;
				}
				GetAbsolutePath(path, sizeof(path)/sizeof(path[0]), pvar->SetupDir, pvar->SetupFile);
				CombinePath(path, sizeof(path)/sizeof(path[0]), win32fd2.cFileName);
				CombinePath(path, sizeof(path)/sizeof(path[0]), win32fd.cFileName);
				p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
				uflg = MF_ENABLED | ((_tcsnicmp(p, path, MAX_PATH) == 0) ? MF_CHECKED : 0);
				TTXFree(&p);
				GetRelatedPath(path, sizeof(path)/sizeof(path[0]), path, pvar->SetupFile, 0);
				RemoveFileExt(path);
				while (p = _tcschr(path, _T('\\')))
					*p = _T('/');
				_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("&%d + %s"), TTXMenuOrgID(uid - ID_MENUITEM), path);
				AppendMenu(menu, uflg, uid++, name);
			} while (FindNextFile(hFind, &win32fd));
			FindClose(hFind);
		} while (FindNextFile(hFind2, &win32fd2));
		FindClose(hFind2);
	}
}

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	MENUITEMINFO mii;
	LPTSTR path;
	size_t path_sz;
	TCHAR name[32];
	LPTSTR p, p2;

	pvar->hwnd = hWin;

	switch (TTXMenuOrgID(cmd))
	{
		// case ID_MENUITEM1:
		// 	switch (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_SETUP_ENV),
		// 						   hWin, EnvSettingProc, (LPARAM)NULL))
		// 	{
		// 	case IDOK:
		// 		break;
		// 	case IDCANCEL:
		// 		break;
		// 	case -1:
		// 		MessageBox(hWin, "Error", "Can't display dialog box.",
		// 				   MB_OK | MB_ICONEXCLAMATION);
		// 		break;
		// 	}
		// 	return 1;

	case ID_MENUITEM2:
		switch (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_COPY_ENV),
							   hWin, EnvCopyProc, (LPARAM)NULL))
		{
		case IDOK:
			if (pvar->OverEnv)
			{
				SendMessage(hWin, WM_USER_ACCELCOMMAND, IdCmdRestoreSetup, 0);
			}
			break;
		case IDCANCEL:
			break;
		case -1:
			MessageBox(hWin, _T("Error"), _T("Can't display dialog box."),
					   MB_OK | MB_ICONEXCLAMATION);
			break;
		}
		return 1;

	case ID_MENUITEM:
		TTXSetPath(pvar->ts, ID_SETUPFNAME, pvar->SetupFile);
		pvar->OverEnv = TRUE;
		SendMessage(hWin, WM_USER_ACCELCOMMAND, IdCmdRestoreSetup, 0);
		return 1;
	}

	if (cmd > ID_MENUITEM && cmd < ID_MENUITEM + MENUITEM_NUM)
	{
		memset(&mii, 0, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_TYPE;
		mii.dwTypeData = name;
		mii.cch = sizeof(name)/sizeof(name[0]) - 1;
		GetMenuItemInfo(pvar->EnvMenu, TTXMenuOrgID(cmd), FALSE, &mii);
		p = _tcschr(name, _T('+'));
		if (p)
		{
			path_sz = MAX_PATH;
			TTXDup(&path, path_sz, NULL);
			GetAbsolutePath(path, path_sz, pvar->SetupDir, pvar->SetupFile);
			CombinePath(path, path_sz, (p + 2));
			_tcscat_s(path, path_sz, _T(".INI"));
			RemovePathSlash(path);
			TTXSetPath(pvar->ts, ID_SETUPFNAME, path);
			TTXFree(&path);
			pvar->OverEnv = TRUE;
			SendMessage(hWin, WM_USER_ACCELCOMMAND, IdCmdRestoreSetup, 0);
		}
		return 1;
	}

	if (cmd == ID_MENUITEM + MENUITEM_NUM)
	{
		path_sz = MAX_PATH;
		path = TTXGetPath(pvar->ts, ID_SETUPFNAME);
		TTXDup(&path, path_sz, NULL);
		p = _T("設定ファイル");
		p2 = _T("設定ファイル(*.ini)\0*.ini\0\0");
		if (OpenFileDlg(0, 0, p, p2, path, 0))
		{
			TTXSetPath(pvar->ts, ID_SETUPFNAME, path);
			TTXFree(&path);
			pvar->OverEnv = TRUE;
			SendMessage(hWin, WM_USER_ACCELCOMMAND, IdCmdRestoreSetup, 0);
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
	TTXGetUIHooks,
	TTXGetSetupHooks,
	NULL, //TTXOpenTCP,
	NULL, //TTXCloseTCP,
	NULL, //TTXSetWinSize,
	TTXModifyMenu,
	TTXModifyPopupMenu,
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
