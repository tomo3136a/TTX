/*
 * Tera Term Environment Package Extension
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

#define ORDER 6015

#define INISECTION "TTXPackage"

#define ID_MENUITEM 56015

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

// static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
// {
// 	 printf("TTXGetUIHooks %d\n", ORDER);
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

//convert ini file to template ini file
static BOOL conv_ini_file(PCHAR path, PCHAR fn)
{
	strset_t apps, keys;
	strset_t app_ctx, key_ctx;
	PCHAR app, key, val, s;
	PCHAR val_ctx;
	PCHAR base;
	int base_sz;
	PCHAR buf;
	int buf_sz;
	BOOL flg;

	base_sz = strlen(fn) + 1;
	base = malloc(base_sz);
	GetParentPath(base, base_sz, fn);
	base_sz = strlen(base);

	buf_sz = MAX_PATH;
	buf = malloc(buf_sz);

	apps = keys = val = NULL;

	if (GetIniSects(&apps, 1024, 1024, path))
	{
		app = StrSetTok(apps, &app_ctx);
		while (app)
		{
			if (GetIniKeys(app, &keys, 1024, 1024, path))
			{
				key = StrSetTok(keys, &key_ctx);
				while (key)
				{
					if (GetIniString(app, key, "", &val, 1024, 1024, path))
					{
						buf[0] = 0;
						s = strtok_s(val, ",", &val_ctx);
						flg = FALSE;
						while (s)
						{
							if (buf[0])
								strcat_s(buf, buf_sz, ",");
							if (_strnicmp(s, base, base_sz) == 0)
							{
								strcat_s(buf, buf_sz, "{BASE}");
								strcat_s(buf, buf_sz, s + base_sz);
								flg = TRUE;
							}
							else
							{
								strcat_s(buf, buf_sz, s);
							}
							s = strtok_s(NULL, ",", &val_ctx);
						}
						if (flg)
							WritePrivateProfileString(app, key, buf, path);
					}
					key = StrSetTok(NULL, &key_ctx);
				}
			}
			app = StrSetTok(NULL, &app_ctx);
		}
	}

	if (val)
		free(val);
	if (keys)
		free(keys);
	if (apps)
		free(apps);
	free(buf);
	free(base);
	return TRUE;
}

static BOOL ConvertFile(PCHAR path, PCHAR fn, BOOL bFailIfExists)
{
	HANDLE hReadFile;
	DWORD dwReadSize;
	HANDLE hWriteFile;
	DWORD dwWriteSize;
	PCHAR dst;
	int dst_sz;
	PCHAR buf;
	int buf_sz;

	hReadFile = CreateFile(path, GENERIC_READ, 0, NULL,
						   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hReadFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(0, "ファイルが開けません", NULL, MB_OK);
		return FALSE;
	}

	dst_sz = strlen(path) + 5;
	dst = malloc(dst_sz);
	strcpy_s(dst, dst_sz, path);
	strcat_s(dst, dst_sz, ".in");

	hWriteFile = CreateFile(dst, GENERIC_WRITE, 0, NULL,
							CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hWriteFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(0, "書き込みファイルが開けません", NULL, MB_OK);
		CloseHandle(hReadFile);
		free(dst);
		return FALSE;
	}

	buf_sz = 1024;
	buf = malloc(buf_sz);
	while (ReadFile(hReadFile, buf, buf_sz, &dwReadSize, NULL))
	{
		if (!dwReadSize)
			break;
		WriteFile(hWriteFile, buf, dwReadSize, &dwWriteSize, NULL);
	}
	FlushFileBuffers(hWriteFile);
	CloseHandle(hWriteFile);
	CloseHandle(hReadFile);

	free(buf);
	free(dst);
	return TRUE;
}

//ディレクトリ削除
static BOOL DeleteDirectory(PCHAR src)
{
	char *path;
	int path_sz, src_sz;
	WIN32_FIND_DATA win32fd;
	HANDLE hFind;

	src_sz = strlen(src) + 5;
	path_sz = src_sz;
	path = malloc(path_sz);
	strcpy_s(path, path_sz, src);
	strcat_s(path, path_sz, "\\*.*");

	hFind = FindFirstFile(path, &win32fd);
	free(path);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return FALSE;
	}
	do
	{
		if (FILE_ATTRIBUTE_DIRECTORY & win32fd.dwFileAttributes)
		{
			if (0 == win32fd.cFileName[0])
				continue;
			if ('.' == win32fd.cFileName[0])
			{
				if (0 == win32fd.cFileName[1])
					continue;
				if ('.' == win32fd.cFileName[1])
				{
					if (0 == win32fd.cFileName[2])
						continue;
				}
			}
			path_sz = src_sz + strlen(win32fd.cFileName) + 2;
			path = malloc(path_sz);
			strcpy_s(path, path_sz, src);
			strcat_s(path, path_sz, "\\");
			strcat_s(path, path_sz, win32fd.cFileName);
			if (!DeleteDirectory(path))
			{
				FindClose(hFind);
				free(path);
				return FALSE;
			}
			free(path);
			continue;
		}
		path_sz = src_sz + strlen(win32fd.cFileName) + 2;
		path = malloc(path_sz);
		strcpy_s(path, path_sz, src);
		strcat_s(path, path_sz, "\\");
		strcat_s(path, path_sz, win32fd.cFileName);
		if (!DeleteFile(path))
		{
			FindClose(hFind);
			free(path);
			return FALSE;
		}
		free(path);
	} while (FindNextFile(hFind, &win32fd));
	FindClose(hFind);

	RemoveDirectory(src);
	return TRUE;
}

//_setup.cmd を作成
//path: 作成先のフォルダ
//shortcut: ディスクトップショートカット作成の有効化
//ini: ショートカット作成時の ININ ファイル相対パス
BOOL make_setup_cmd(PCHAR path, BOOL shortcut, PCHAR ini)
{
	char *buf;
	int buf_sz;
	HANDLE hFile;
	DWORD dwWriteSize;
	PCHAR s;

	buf_sz = strlen(path) + 16;
	buf = malloc(buf_sz);
	strcpy_s(buf, buf_sz, path);
	CombinePath(buf, buf_sz, "_setup.cmd");

	hFile = CreateFile(buf, GENERIC_WRITE, 0, NULL,
					   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	free(buf);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(0, "ファイルが開けません", NULL, MB_OK);
		return FALSE;
	}

	s = "@echo off\r\n"
		"pushd %~dp0\r\n"
		"setlocal ENABLEDELAYEDEXPANSION\r\n"
		"set base=%~dp0\r\n"
		"for /f \"delims=\" %%a in (\"%base:~0,-1%\") do set nm=%%~na\r\n\r\n";
	WriteFile(hFile, s, strlen(s), &dwWriteSize, NULL);

	s = "echo 文字列置換中...\r\n"
		"set PS=powershell -Command\r\n"
		"%PS% \"gci *.in -Recurse |%%{gc $_|"
		"%%{$_ -replace '{BASE}','!base!'}|sc ($_.FullName -replace '\\.in$','')}\"\r\n"
		"%PS% \"gci *.in -Recurse |remove-item\"\r\n"
		"\r\n";
	WriteFile(hFile, s, strlen(s), &dwWriteSize, NULL);

	if (shortcut)
	{
		s = "set lnk=%HOMEDRIVE%%HOMEPATH%\\Desktop\\%nm%.lnk\r\n"
			"if @@==@%1@ goto :sc\r\n\r\n"
			"set app=ttermpro.exe\r\n"
			"set src=%ProgramFiles(x86)%\\teraterm\\\r\n"
			"if not exist \"!src!%app%\" set src=%ProgramFiles%\\teraterm\\\r\n"
			"if not exist \"!src!%app%\" (\r\n"
			"   echo Teraterm が見つかりませんでした。\r\n"
			"   exit\r\n"
			")\r\n"
			"\r\n";
		WriteFile(hFile, s, strlen(s), &dwWriteSize, NULL);

		s = "echo ショートカット作成中...\r\n"
			"if exist %lnk% del /f %lnk%\r\n";
		WriteFile(hFile, s, strlen(s), &dwWriteSize, NULL);
		if (ini && ini[0])
		{
			s = "set ini=%base%";
			WriteFile(hFile, s, strlen(s), &dwWriteSize, NULL);
			WriteFile(hFile, ini, strlen(ini), &dwWriteSize, NULL);
			s = "\r\n"
				"set opt=/F=\"\"\"%ini%\"\"\"\r\n";
		}
		else
		{
			s = "set opt=\r\n";
		}
		WriteFile(hFile, s, strlen(s), &dwWriteSize, NULL);
		s = //"set key=Ctrl+F1\r\n"
			"set key=\r\n"
			"%PS% \"$w=New-Object -ComObject Wscript.Shell;$s=$w.CreateShortCut('%lnk%');"
			"$s.TargetPath='!src!%app%';$s.Hotkey='!key!';$s.Arguments='!opt!';$s.save()\"\r\n"
			"\r\n";
		WriteFile(hFile, s, strlen(s), &dwWriteSize, NULL);

		s = ":sc\r\n"
			"echo ショートカット実行\r\n"
			"start \"%nm%\" %lnk%\r\n"
			"\r\n";
		WriteFile(hFile, s, strlen(s), &dwWriteSize, NULL);
	}

	s = ":end\r\n"
		"endlocal\r\n"
		"popd\r\n"
		"goto :eof\r\n";
	WriteFile(hFile, s, strlen(s), &dwWriteSize, NULL);

	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	return TRUE;
}

//cabinet ファイルを作成、base64エンコード、バッチファイルを作成
BOOL make_cabinet(char *name, char *listfile, char *outpath)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char *path;
	int path_sz;
	char *buf;
	int buf_sz;
	PCHAR s;
	HANDLE hWriteFile;
	DWORD dwWriteSize;
	HANDLE hReadFile;
	DWORD dwReadSize;

	path_sz = MAX_PATH;
	path = malloc(path_sz);

	buf_sz = MAX_PATH;
	buf = malloc(buf_sz);

	//make cabinet file
	s = "makecab /D CabinetName1=\"%s.cab\" /D DiskDirectoryTemplate=\"%s\" /F \"%s\"";
	GetParentPath(path, path_sz, listfile);
	_snprintf_s(buf, buf_sz, buf_sz, s, name, path, listfile);
	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	if (CreateProcess(NULL, buf, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
					  NULL, path, &si, &pi) == 0)
	{
		free(buf);
		free(path);
		return FALSE;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	//encode to b64
	s = "certutil -encode \"%s.cab\" \"%s.b64\"";
	_snprintf_s(buf, buf_sz, buf_sz, s, name, name);
	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	if (CreateProcess(NULL, buf, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
					  NULL, path, &si, &pi) == 0)
	{
		free(buf);
		free(path);
		return FALSE;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	//make batch file
	strcpy_s(path, path_sz, outpath);
	CombinePath(path, path_sz, name);
	strcat_s(path, path_sz, ".bat");
	hWriteFile = CreateFile(path, GENERIC_WRITE, 0, NULL,
							CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hWriteFile == INVALID_HANDLE_VALUE)
	{
		free(buf);
		free(path);
		return FALSE;
	}
	s = "@echo off\r\n"
		"pushd %~dp0\r\n"
		"set nm=%~n0\r\n"
		"if exist %nm% goto :setup\r\n"
		"certutil -decode \"%~dpnx0\" \"%TEMP%\\%nm%.cab\"\r\n"
		"mkdir %nm%\r\n"
		"expand \"%TEMP%\\%nm%.cab\" -F:* %nm%\r\n"
		"del \"%TEMP%\\%nm%.cab\"\r\n"
		"set sc=1\r\n"
		":setup\r\n"
		"if exist %nm%\\_setup.cmd call %nm%\\_setup.cmd %sc%\r\n"
		"popd\r\n"
		"exit /b\r\n\r\n";
	WriteFile(hWriteFile, s, strlen(s), &dwWriteSize, NULL);

	GetParentPath(path, path_sz, listfile);
	CombinePath(path, path_sz, name);
	strcat_s(path, path_sz, ".b64");

	hReadFile = CreateFile(path, GENERIC_READ, 0, NULL,
						   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hReadFile == INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(hWriteFile);
		CloseHandle(hWriteFile);
		free(buf);
		free(path);
		return FALSE;
	}
	while (ReadFile(hReadFile, buf, buf_sz, &dwReadSize, NULL))
	{
		if (!dwReadSize)
			break;
		WriteFile(hWriteFile, buf, dwReadSize, &dwWriteSize, NULL);
	}
	CloseHandle(hReadFile);
	FlushFileBuffers(hWriteFile);
	CloseHandle(hWriteFile);

	free(buf);
	free(path);
	return TRUE;
}

//ファイルにファイルリストを書き出す
BOOL write_filelist(HANDLE hFile, PCHAR src, int n)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd;
	char *path;
	int path_sz;
	DWORD dwWriteSize;
	PCHAR s;
	PCHAR p;

	path_sz = MAX_PATH;
	path = malloc(path_sz);

	//file list
	_snprintf_s(path, path_sz, _TRUNCATE, "%s\\*.*", src);
	hFind = FindFirstFile(path, &win32fd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		free(path);
		return FALSE;
	}
	do
	{
		if (win32fd.cFileName[0] == '.' ||
			win32fd.cFileName[0] == '_' ||
			win32fd.cFileName[0] == '~' ||
			win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		p = FindFileExt(win32fd.cFileName);
		if (_stricmp(p, ".EXE") == 0 ||
			_stricmp(p, ".DLL") == 0 ||
			_stricmp(p, ".IN") == 0)
			continue;

		strcpy_s(path, path_sz, src);
		CombinePath(path, path_sz, win32fd.cFileName);

		if (_stricmp(p, ".INI") == 0 ||
			_stricmp(p, ".CNF") == 0)
		{
			PCHAR org;
			int org_sz;

			org_sz = strlen(path) + 4;
			org = malloc(org_sz);
			strcpy_s(org, org_sz, path);
			strcat_s(path, path_sz, ".in");
			CopyFile(org, path, FALSE);

			conv_ini_file(path, pvar->ts->SetupFName);
		}

		WriteFile(hFile, path, lstrlen(path), &dwWriteSize, NULL);
		WriteFile(hFile, "\r\n", 2, &dwWriteSize, NULL);
	} while (FindNextFile(hFind, &win32fd));
	FindClose(hFind);

	//directory list
	_snprintf_s(path, path_sz, _TRUNCATE, "%s\\*.*", src);
	hFind = FindFirstFile(path, &win32fd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		free(path);
		return FALSE;
	}
	do
	{
		if (win32fd.cFileName[0] == '.' ||
			win32fd.cFileName[0] == '_' ||
			win32fd.cFileName[0] == '~' ||
			!(win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;

		strcpy_s(path, path_sz, src);
		CombinePath(path, path_sz, win32fd.cFileName);

		p = (lstrlen(path) > n) ? (path + n) : "";
		s = ".Set DestinationDir=\"";
		WriteFile(hFile, s, lstrlen(s), &dwWriteSize, NULL);
		WriteFile(hFile, p, lstrlen(p), &dwWriteSize, NULL);
		s = "\"\r\n";
		WriteFile(hFile, s, lstrlen(s), &dwWriteSize, NULL);
		if (!write_filelist(hFile, path, n))
		{
			FindClose(hFind);
			free(path);
			return FALSE;
		}
	} while (FindNextFile(hFind, &win32fd));
	FindClose(hFind);
	free(path);
	return TRUE;
}

//パッケージ作成
//name: パッケージ名
//src: パッケージ化対象のフォルダ
//dst: パッケージの出力策フォルダ
//bSetup: _setup.cmd の追加要求
BOOL make_package(PCHAR name, PCHAR src, PCHAR dst, BOOL bSetup)
{
	HANDLE hFile;
	char *tmp;
	int tmp_sz;
	char *lst;
	int lst_sz;
	char *path;
	int path_sz;
	DWORD dwWriteSize;
	PCHAR s;

	//create tempolary folder
	tmp_sz = MAX_PATH;
	tmp = malloc(tmp_sz);
	GetTempPath(tmp_sz, tmp);
	strcat_s(tmp, tmp_sz, "tt_");
	strcat_s(tmp, tmp_sz, name);
	CreateDirectory(tmp, NULL);

	//build file list
	lst_sz = MAX_PATH;
	lst = malloc(lst_sz);
	strcpy_s(lst, lst_sz, tmp);
	CombinePath(lst, lst_sz, "files.txt");
	hFile = CreateFile(lst, GENERIC_WRITE, 0, NULL,
					   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		s = "ファイルリストが作成できません";
		MessageBox(0, s, NULL, MB_OK);
		free(lst);
		free(tmp);
		return FALSE;
	}
	if (bSetup)
	{
		path_sz = MAX_PATH;
		path = malloc(path_sz);
		GetRelatedPath(path, path_sz, pvar->ts->SetupFName, pvar->ts->SetupFName, 0);
		if (make_setup_cmd(tmp, bSetup, path))
		{
			strcpy_s(path, path_sz, tmp);
			CombinePath(path, path_sz, "_setup.cmd");
			strcat_s(path, path_sz, "\r\n");
			WriteFile(hFile, path, strlen(path), &dwWriteSize, NULL);
		}
		free(path);
	}
	write_filelist(hFile, src, strlen(src) + 1);
	CloseHandle(hFile);

	//make batch file
	if (!make_cabinet(name, lst, dst))
	{
		MessageBox(0, dst, name, MB_OK);
	}

	//destroy templary folder
	DeleteDirectory(tmp);

	free(lst);
	free(tmp);
	return TRUE;
}

///////////////////////////////////////////////////////////////
//
// PackageProc設定ダイアログのコールバック関数。
//
static LRESULT CALLBACK PackageProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPITEMIDLIST pidl;
	char name[64];
	char *path;
	int path_sz;
	char *buf;
	int buf_sz;
	char *s;
	BOOL flg;

	switch (msg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(dlg, IDC_NAME, pvar->ts->Title);
		path_sz = MAX_PATH;
		path = malloc(path_sz);
		GetParentPath(path, path_sz, pvar->ts->SetupFName);
		SetDlgItemText(dlg, IDC_PATH1, path);
		if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl) == NOERROR)
		{
			SHGetPathFromIDList(pidl, path);
			CoTaskMemFree(pidl);
			SetDlgItemText(dlg, IDC_PATH5, path);
		}
		free(path);
		MoveParentCenter(dlg);
		return TRUE;

	case WM_COMMAND:
		path_sz = MAX_PATH;
		path = malloc(path_sz);
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			GetDlgItemText(dlg, IDC_PATH1, path, MAX_PATH);
			s = "対象フォルダを選択してください";
			OpenFolderDlg(dlg, IDC_PATH1, s, path);
			free(path);
			return TRUE;

		case IDC_BUTTON5:
			GetDlgItemText(dlg, IDC_PATH5, path, MAX_PATH);
			s = "出力先を選択してください";
			OpenFolderDlg(dlg, IDC_PATH5, s, path);
			free(path);
			return TRUE;

		case IDOK:
			buf_sz = MAX_PATH;
			buf = malloc(buf_sz);
			GetDlgItemText(dlg, IDC_NAME, name, sizeof(name));
			GetDlgItemText(dlg, IDC_PATH1, path, path_sz);
			GetDlgItemText(dlg, IDC_PATH5, buf, buf_sz);
			flg = (IsDlgButtonChecked(dlg, IDC_CHECK2) == BST_CHECKED);
			flg = make_package(name, path, buf, flg);
			free(buf);
			GetTempPath(path_sz, path);
			strcat_s(path, path_sz, "tt_");
			strcat_s(path, path_sz, name);
			if (flg)
			{
				s = "パッケージ %s を作成しました。\n\n作成先： %s\n";
				_snprintf_s(pvar->UIMsg, sizeof(pvar->UIMsg), _TRUNCATE, s, name, path);
				free(path);
			}
			else
			{
				s = "パッケージ %s を作成出来ませんでした。\n\n作成先： %s\n";
				_snprintf_s(pvar->UIMsg, sizeof(pvar->UIMsg), _TRUNCATE, s, name, path);
				MessageBox(dlg, pvar->UIMsg, "Tera Term", MB_OK | MB_ICONEXCLAMATION);
				free(path);
				return TRUE;
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
	UINT lang;
	LPSTR s;

	lang = UILang(pvar->ts->UILanguageFile);
	pvar->menuoffset = MenuOffset(INISECTION, ID_MENUITEM, 0);

	pvar->FileMenu = GetSubMenu(menu, ID_FILE);

	s = (lang == 2) ? "パッケージ出力(&P)" : "Export &package";
	InsertMenu(pvar->FileMenu, ID_FILE_PRINT2, MF_BYCOMMAND, ID_MENUITEM + pvar->menuoffset, s);
}

///////////////////////////////////////////////////////////////

// static void PASCAL TTXModifyPopupMenu(HMENU menu)
// {
// 	 printf("TTXModifyPopupMenu %d\n", ORDER);
// }

///////////////////////////////////////////////////////////////

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (cmd + pvar->menuoffset)
	{
	case ID_MENUITEM:
		switch (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PACKAGE),
							   hWin, PackageProc, (LPARAM)NULL))
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
