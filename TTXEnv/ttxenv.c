/*
 * Tera Term Environment Extension
 * (C) 2021 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt_res.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compat_w95.h"
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
	int menuoffset;

	//menu
	HMENU SetupMenu;
	HMENU EnvMenu;

	//original callback
	PReadIniFile origReadIniFile;
	PWriteIniFile origWriteIniFile;
	PParseParam origParseParam;

	//status
	BOOL UseEnv;
	BOOL EnableEnv;
	BOOL OverEnv;
	BOOL UseKeyCnf;
	char SetupFile[MAX_PATH];
	char SetupDir[MAX_PATH];
	char ttpath[MAX_PATH];

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

	pvar->UseKeyCnf = FALSE;
	pvar->UseEnv = FALSE;
	pvar->EnableEnv = FALSE;
	pvar->OverEnv = FALSE;
	pvar->SetupFile[0] = 0;
	pvar->SetupDir[0] = 0;
	memset(pvar->ttpath, sizeof(pvar->ttpath), 0);
	GetModuleFileName(NULL, pvar->ttpath, sizeof(pvar->ttpath) - 1);
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

// static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
// {
// 	 printf("TTXGetUIHooks %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

static void PASCAL TTXReadIniFile(PCHAR fn, PTTSet ts)
{
	char buf[MAX_PATH];

	if (!pvar->skip)
		(pvar->origReadIniFile)(fn, ts);

	GetPrivateProfileString(INISECTION, "KeyCnf", "", buf, sizeof(buf), fn);
	pvar->UseKeyCnf = (buf[0]) ? TRUE : FALSE;
	if (pvar->UseKeyCnf)
	{
		GetAbsolutePath(pvar->ts->KeyCnfFN, sizeof(pvar->ts->KeyCnfFN), buf, fn);
		PostMessage(pvar->cv->HWin, WM_USER_ACCELCOMMAND, IdCmdLoadKeyMap, 0);
	}

	pvar->EnableEnv = GetIniOnOff(INISECTION, "EnableEnv", FALSE, fn);
	if (!pvar->OverEnv)
	{
		pvar->UseEnv = pvar->EnableEnv;
		strcpy_s(pvar->SetupFile, sizeof(pvar->SetupFile), fn);
		GetPrivateProfileString(INISECTION, "SetupDir", "", buf, sizeof(buf), fn);
		GetAbsolutePath(pvar->SetupDir, sizeof(pvar->SetupDir), buf, fn);
	}
	pvar->OverEnv = FALSE;
}

static void PASCAL TTXWriteIniFile(PCHAR fn, PTTSet ts)
{
	char buf[MAX_PATH];
	PCHAR p;

	(pvar->origWriteIniFile)(fn, ts);

	p = NULL;
	if (pvar->UseKeyCnf)
	{
		GetRelatedPath(buf, sizeof(buf), pvar->ts->KeyCnfFN, pvar->ts->SetupFName, 0);
		p = buf;
	}
	WritePrivateProfileString(INISECTION, "KeyCnf", p, fn);

	p = NULL;
	if (pvar->EnableEnv)
	{
		GetRelatedPath(buf, sizeof(buf), pvar->SetupDir, pvar->ts->SetupFName, 0);
		p = buf;
	}
	WriteIniOnOff(INISECTION, "EnableEnv", pvar->EnableEnv, pvar->EnableEnv, fn);
	WritePrivateProfileString(INISECTION, "SetupDir", p, fn);
}

static void PASCAL TTXParseParam(PCHAR Param, PTTSet ts, PCHAR DDETopic)
{
	char buf[MAX_PATH + 20];
	PCHAR next;

	(pvar->origParseParam)(Param, ts, DDETopic);

	next = Param;
	while (next = TTXGetParam(buf, sizeof(buf), next))
	{
		if (_strnicmp(buf, "/F=", 3) == 0)
		{
			pvar->skip = TRUE;
			TTXReadIniFile(&buf[3], ts);
			pvar->skip = FALSE;
			break;
		}
	}
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
// 			OpenFolderDlg(dlg, IDC_PATH1, "環境フォルダを選択してください", buf);
// 			return TRUE;

// 		case IDC_BUTTON2:
// 			GetDlgItemText(dlg, IDC_PATH2, buf, sizeof(buf));
// 			p = "キーボード設定ファイル(*.cnf)\0*.cnf\0\0";
// 			OpenFileDlg(dlg, IDC_PATH2, "キーマップファイル", p, buf);
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
BOOL CopyPathEnv(PCHAR dst, int sz, PCHAR src, BOOL force, PCHAR src_base, PCHAR dst_base)
{
	char path[MAX_PATH];
	char tmp[MAX_PATH];
	char *name;

	GetAbsolutePath(path, sizeof(path), src, src_base);
	name = FindFileName(path);
	GetAbsolutePath(dst, sz, name, dst_base);
	if (_stricmp(path, dst) != 0)
	{
		if (FileExists(dst) && !force)
		{
			const char *s = "ファイルが存在しています。上書きしますか。\r%s";
			_snprintf_s(tmp, sizeof(tmp), _TRUNCATE, s, name);
			if (MessageBox(0, tmp, "WARNING", MB_YESNO | MB_ICONWARNING) != IDYES)
			{
				strcpy_s(dst, sz, path);
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
void CopyUserKeyMacro(PCHAR keycnf, PCHAR fn)
{
	char name[64];
	char buf[MAX_PATH];
	char dst[MAX_PATH];
	char *p;
	int i;

	for (i = 1; i < IdUser1; i++)
	{
		_snprintf_s(name, sizeof(name), _TRUNCATE, "User%d", i);
		GetPrivateProfileString("User keys", name, "", buf, sizeof(buf), keycnf);
		if (!buf[0])
			break;
		p = strchr(buf, ',');
		if (!p || atoi(p + 1) != 2)
			continue;
		p = strchr(p + 1, ',');
		if (!p)
			continue;
		if (CopyPathEnv(dst, sizeof(dst), ++p, FALSE, pvar->ttpath, fn))
		{
			GetRelatedPath(p, buf + sizeof(buf) - p, dst, pvar->ttpath, 0);
			WritePrivateProfileString("User keys", name, buf, keycnf);
		}
	}
}

//
// EnvCopyProc設定ダイアログのコールバック関数。
//
static LRESULT CALLBACK EnvCopyProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char buf[MAX_PATH];
	char path[MAX_PATH];
	char fn[MAX_PATH];
	PCHAR s;

	switch (msg)
	{
	case WM_INITDIALOG:
		GetAbsolutePath(buf, sizeof(buf), pvar->SetupDir, pvar->SetupFile);
		SetDlgItemText(dlg, IDC_PATH1, buf);
		GetPathName(buf, sizeof(buf), pvar->ts->SetupFName);
		SetDlgItemText(dlg, IDC_PATH2, buf);
		GetPathName(buf, sizeof(buf), pvar->ts->KeyCnfFN);
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
				GetDlgItemText(dlg, IDC_NAME, buf, sizeof(buf));
				if (buf[0])
				{
					strcat_s(buf, sizeof(buf), ".INI");
					SetDlgItemText(dlg, IDC_PATH2, buf);
					RemoveFileExt(buf);
					strcat_s(buf, sizeof(buf), ".CNF");
					SetDlgItemText(dlg, IDC_PATH3, buf);
				}
				else
				{
					GetPathName(buf, sizeof(buf), pvar->ts->SetupFName);
					SetDlgItemText(dlg, IDC_PATH2, buf);
					GetPathName(buf, sizeof(buf), pvar->ts->KeyCnfFN);
					SetDlgItemText(dlg, IDC_PATH3, buf);
				}
			}
			return TRUE;
		}
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			GetDlgItemText(dlg, IDC_PATH1, buf, sizeof(buf));
			s = "複製先フォルダを選択してください";
			OpenFolderDlg(dlg, IDC_PATH1, s, buf);
			return TRUE;

		case IDC_CHECK1:
			if (IsDlgButtonChecked(dlg, IDC_CHECK1) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_NAME, buf, sizeof(buf));
				if (buf[0])
				{
					strcat_s(buf, sizeof(buf), ".INI");
					SetDlgItemText(dlg, IDC_PATH2, buf);
					RemoveFileExt(buf);
					strcat_s(buf, sizeof(buf), ".CNF");
					SetDlgItemText(dlg, IDC_PATH3, buf);
				}
				else
				{
					GetPathName(buf, sizeof(buf), pvar->ts->SetupFName);
					SetDlgItemText(dlg, IDC_PATH2, buf);
					GetPathName(buf, sizeof(buf), pvar->ts->KeyCnfFN);
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
			GetDlgItemText(dlg, IDC_PATH1, buf, sizeof(buf));
			GetAbsolutePath(fn, sizeof(fn), buf, pvar->SetupFile);
			if (!FileExists(fn))
			{
				if (CreateDirectory(fn, NULL) == 0)
				{
					s = "設定ファイルを作成できません。";
					MessageBox(dlg, s, "WARNING", MB_OK | MB_ICONWARNING);
					return FALSE;
				}
			}
			GetDlgItemText(dlg, IDC_PATH2, buf, sizeof(buf));
			if (!buf[0])
			{
				s = "設定ファイルのパスを設定してください。";
				MessageBox(dlg, s, "WARNING", MB_OK | MB_ICONWARNING);
				return FALSE;
			}
			CombinePath(fn, sizeof(fn), buf);
			if (FileExists(fn))
			{
				s = "設定ファイルが存在しています。上書きしますか。";
				if (MessageBox(dlg, s, "WARNING", MB_YESNO | MB_ICONWARNING) != IDYES)
				{
					return FALSE;
				}
			}
			CopyFile(pvar->ts->SetupFName, fn, FALSE);
			WritePrivateProfileString(INISECTION, "EnableEnv", NULL, fn);
			WritePrivateProfileString(INISECTION, "SetupDir", NULL, fn);
			WritePrivateProfileString(INISECTION, "KeyCnf", NULL, fn);

			// set title
			if (IsDlgButtonChecked(dlg, IDC_CHECK2) == BST_CHECKED)
			{
				GetDlgItemText(dlg, IDC_NAME, buf, sizeof(buf));
				if (buf[0])
				{
					WritePrivateProfileString(TTSECTION, "Title", buf, fn);
				}
			}

			if (IsDlgButtonChecked(dlg, IDC_CHECK5) == BST_CHECKED)
			{
				pvar->OverEnv = TRUE;
			}

			// set environment master
			if (IsDlgButtonChecked(dlg, IDC_CHECK4) == BST_CHECKED)
			{
				WriteIniOnOff(INISECTION, "EnableEnv", TRUE, TRUE, fn);
				if (pvar->OverEnv)
				{
					pvar->EnableEnv = TRUE;
					pvar->UseEnv = TRUE;
					strcpy_s(pvar->SetupFile, sizeof(pvar->SetupFile), fn);
				}
			}

			// set startup macro
			if (pvar->ts->MacroFN[0] &&
				(IsDlgButtonChecked(dlg, IDC_CHECK3) == BST_CHECKED) &&
				CopyPathEnv(buf, sizeof(buf), pvar->ts->MacroFN, FALSE, pvar->ttpath, fn))
			{
				GetRelatedPath(path, sizeof(path), buf, pvar->ttpath, 0);
				WritePrivateProfileString(TTSECTION, "StartupMacro", path, fn);
			}

			// copy keymap file
			GetDlgItemText(dlg, IDC_PATH3, buf, sizeof(buf));
			if (buf[0])
			{
				GetAbsolutePath(path, sizeof(path), buf, fn);
				char *s = "キーマップファイルが存在しています。上書きしますか。";
				if (!FileExists(path) ||
					(MessageBox(dlg, s, "WARNING", MB_YESNO | MB_ICONWARNING) == IDYES))
				{
					CopyFile(pvar->ts->KeyCnfFN, path, FALSE);
				}
				GetRelatedPath(buf, sizeof(buf), path, fn, 2);
				WritePrivateProfileString(INISECTION, "KeyCnf", buf, fn);
				if (IsDlgButtonChecked(dlg, IDC_CHECK3) == BST_CHECKED)
				{
					CopyUserKeyMacro(path, fn);
				}
			}
			if (pvar->OverEnv)
			{
				strcpy_s(pvar->ts->SetupFName, sizeof(pvar->ts->SetupFName), fn);
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
	PCHAR s;

	flag = MF_ENABLED;
	lang = UILang(pvar->ts->UILanguageFile);
	pvar->menuoffset = MenuOffset(INISECTION, ID_MENUITEM, 0);

	pvar->SetupMenu = GetSubMenu(menu, ID_SETUP);
	s = (lang == 2) ? "環境の保存(&V)..." : "En&vironment copy...";
	AppendMenu(pvar->SetupMenu, flag, ID_MENUITEM2 + pvar->menuoffset, s);
	// s = (lang == 2) ? "環境の設定(&M)..." : "Environ&ment setup...";
	// AppendMenu(pvar->SetupMenu, flag, ID_MENUITEM1 + pvar->menuoffset, s);

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
		mi.dwTypeData = (lang == 2) ? "環境(&A)" : "Environment";
		InsertMenuItem(menu, ID_HELPMENU, TRUE, &mi);
	}
}

static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
	HANDLE hFind;
	HANDLE hFind2;
	WIN32_FIND_DATA win32fd;
	WIN32_FIND_DATA win32fd2;
	char path[MAX_PATH];
	char name[32];
	UINT uflg;
	UINT uid;
	PCHAR p;

	if (menu == pvar->EnvMenu)
	{
		while (DeleteMenu(menu, 0, MF_BYPOSITION) != 0)
		{
		}

		uid = ID_MENUITEM + pvar->menuoffset;
		uflg = MF_ENABLED | ((_strnicmp(pvar->ts->SetupFName, pvar->SetupFile, MAX_PATH) == 0) ? MF_CHECKED : 0);
		GetRelatedPath(path, sizeof(path), pvar->SetupFile, pvar->SetupFile, 0);
		RemoveFileExt(path);
		_snprintf_s(name, sizeof(name), _TRUNCATE, "&0 + %s", path);
		AppendMenu(menu, uflg, uid++, name);

		GetAbsolutePath(path, sizeof(path), pvar->SetupDir, pvar->SetupFile);
		CombinePath(path, sizeof(path), "*.INI");
		hFind = FindFirstFile(path, &win32fd);
		if (hFind == INVALID_HANDLE_VALUE)
			return;
		do
		{
			if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			if (win32fd.cFileName[0] == '_' || win32fd.cFileName[0] == '.')
				continue;
			GetAbsolutePath(path, sizeof(path), pvar->SetupDir, pvar->SetupFile);
			CombinePath(path, sizeof(path), win32fd.cFileName);
			if (_strnicmp(path, pvar->SetupFile, MAX_PATH) == 0)
				continue;
			if (uid == (ID_MENUITEM + MENUITEM_NUM + pvar->menuoffset))
			{
				AppendMenu(menu, MF_ENABLED, uid, "more...");
				break;
			}
			uflg = MF_ENABLED | ((_strnicmp(pvar->ts->SetupFName, path, MAX_PATH) == 0) ? MF_CHECKED : 0);
			GetRelatedPath(path, sizeof(path), path, pvar->SetupFile, 0);
			RemoveFileExt(path);
			_snprintf_s(name, sizeof(name), _TRUNCATE, "&%d + %s", (uid - ID_MENUITEM - pvar->menuoffset), path);
			AppendMenu(menu, uflg, uid++, name);
		} while (FindNextFile(hFind, &win32fd));
		FindClose(hFind);

		GetAbsolutePath(path, sizeof(path), pvar->SetupDir, pvar->SetupFile);
		CombinePath(path, sizeof(path), "*");
		hFind2 = FindFirstFile(path, &win32fd2);
		if (hFind2 == INVALID_HANDLE_VALUE)
			return;
		do
		{
			if (!(win32fd2.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				continue;
			if (win32fd2.cFileName[0] == '_' || win32fd2.cFileName[0] == '.')
				continue;
			GetAbsolutePath(path, sizeof(path), pvar->SetupDir, pvar->SetupFile);
			CombinePath(path, sizeof(path), win32fd2.cFileName);
			CombinePath(path, sizeof(path), "*.INI");
			hFind = FindFirstFile(path, &win32fd);
			if (hFind == INVALID_HANDLE_VALUE)
				return;
			do
			{
				if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					continue;
				if (win32fd2.cFileName[0] == '_' || win32fd2.cFileName[0] == '.')
					continue;
				if (uid == (ID_MENUITEM + MENUITEM_NUM + pvar->menuoffset))
				{
					AppendMenu(menu, MF_ENABLED, uid, "more...");
					break;
				}
				GetAbsolutePath(path, sizeof(path), pvar->SetupDir, pvar->SetupFile);
				CombinePath(path, sizeof(path), win32fd2.cFileName);
				CombinePath(path, sizeof(path), win32fd.cFileName);
				uflg = MF_ENABLED | ((_strnicmp(pvar->ts->SetupFName, path, MAX_PATH) == 0) ? MF_CHECKED : 0);
				GetRelatedPath(path, sizeof(path), path, pvar->SetupFile, 0);
				RemoveFileExt(path);
				while (p = strchr(path, '\\'))
					*p = '/';
				_snprintf_s(name, sizeof(name), _TRUNCATE, "&%d + %s", (uid - ID_MENUITEM - pvar->menuoffset), path);
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
	CHAR path[MAX_PATH];
	CHAR name[32];
	PCHAR p;

	switch (cmd + pvar->menuoffset)
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
			MessageBox(hWin, "Error", "Can't display dialog box.",
					   MB_OK | MB_ICONEXCLAMATION);
			break;
		}
		return 1;

	case ID_MENUITEM:
		strcpy_s(pvar->ts->SetupFName, sizeof(pvar->ts->SetupFName), pvar->SetupFile);
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
		mii.cch = sizeof(name) - 1;
		GetMenuItemInfo(pvar->EnvMenu, cmd + pvar->menuoffset, FALSE, &mii);
		p = strchr(name, '+');
		if (p)
		{
			GetAbsolutePath(path, sizeof(path), pvar->SetupDir, pvar->SetupFile);
			CombinePath(path, sizeof(path), (p + 2));
			strcat_s(path, sizeof(path), ".INI");
			RemovePathSlash(path);
			strcpy_s(pvar->ts->SetupFName, sizeof(pvar->ts->SetupFName), path);
			pvar->OverEnv = TRUE;
			SendMessage(hWin, WM_USER_ACCELCOMMAND, IdCmdRestoreSetup, 0);
		}
		return 1;
	}

	if (cmd == ID_MENUITEM + MENUITEM_NUM)
	{
		strcpy_s(path, sizeof(path), pvar->ts->SetupFName);
		p = "設定ファイル(*.ini)\0*.ini\0\0";
		if (OpenFileDlg(0, 0, "設定ファイル", p, path, NULL, 0))
		{
			strcpy_s(pvar->ts->SetupFName, sizeof(pvar->ts->SetupFName), path);
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
