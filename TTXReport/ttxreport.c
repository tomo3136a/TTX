/*
 * Tera Term Report Extension
 * (C) 2021 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt_res.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <commctrl.h>
#include <tchar.h>

#include "ttxcommon.h"
#include "ttxcmn_util.h"
#include "ttxcmn_ui.h"
#include "stringlist.h"
#include "resource.h"

#define ORDER 6060

#define INISECTION "TTXReport"

#define ID_MENUITEM 56060
#define ID_MENUITEM1 (ID_MENUITEM + 1)
#define ID_MENUITEM2 (ID_MENUITEM + 2)
#define ID_MENUITEM3 (ID_MENUITEM + 3)
//ID_MENUITEM: view report
//ID_MENUITEM1: clear report
//ID_MENUITEM2: open report editor
//ID_MENUITEM3: setup dialog

static HANDLE hInst; /* Instance handle of TTX*.DLL */
static HWND report_dialog = NULL;

#define REPORT_RULE_NUM 20

typedef enum
{
	INFO_CMD_CLEAR = 0x01, //next clear flag on
	INFO_CMD_NODSP = 0x02, //Not display
	INFO_CMD_COUNT = 0x04, //counting
	INFO_CMD_SAVE = 0x08,  //saving
} INFO_CMD;

typedef struct
{
	char seq;
	char nxt;
	char sub;
	INFO_CMD cmd;
	PCHAR ptn;
} TInfoCmd;
typedef TInfoCmd *PInfoCmd;

typedef struct
{
	PTTSet ts;
	PComVar cv;
	BOOL skip;

	//menu
	HMENU SetupMenu;
	HMENU ControlMenu;

	//callback
	Trecv origPrecv;
	TReadFile origPReadFile;

	PReadIniFile origReadIniFile;
	PWriteIniFile origWriteIniFile;
	PParseParam origParseParam;

	//report view
	BOOL ChangeReport;
	BOOL ReportAutoStart;
	POINT report_win_size;
	int report_win_pos;

	//report font
	TCHAR report_font[LF_FACESIZE];
	POINT report_font_size;
	int report_font_charset;

	//message area
	TCHAR report_title[TitleBuffSize];
	TCHAR report_note_path[MAXPATHLEN];

	//report clear
	CHAR report_clear[TitleBuffSize];
	BOOL ClearReq;

	//ピックアップ表示
	TInfoCmd report_rule[REPORT_RULE_NUM];
	int report_rule_num;
	TCHAR info_seq;
	TCHAR info_test_path[MAXPATHLEN];
	PStringList info_test;

	//テスト表示
	TCHAR test_file[4][MAXPATHLEN];
	PStringList test[4];

	//マッチカウンタ
	int info_cnt;
	int info_cnt_ok;
	int info_cnt_ng;

	//結果出力
	TCHAR result_file[MAXPATHLEN];

} TInstVar;

static TInstVar *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	int i;

	pvar->ts = ts;
	pvar->cv = cv;
	pvar->skip = FALSE;

	//reset hook callback
	pvar->origPrecv = NULL;
	pvar->origPReadFile = NULL;

	//initialize report
	pvar->ChangeReport = FALSE;
	pvar->ReportAutoStart = FALSE;
	pvar->report_win_size.x = 0;
	pvar->report_win_size.y = 0;
	pvar->report_win_pos = 0;

	pvar->report_font[0] = 0;
	pvar->report_font_size.x = 0;
	pvar->report_font_size.y = 0;
	pvar->report_font_charset = 0;

	pvar->report_title[0] = 0;
	pvar->report_note_path[0] = 0;

	for (i = 0; i < REPORT_RULE_NUM; i++)
	{
		memset(&(pvar->report_rule[i]), 0, sizeof(pvar->report_rule[0]));
		pvar->report_rule[i].ptn = NULL;
	}
	pvar->report_rule_num = 0;
	pvar->info_seq = 0;
	pvar->ClearReq = TRUE;
	pvar->report_clear[0] = 0;

	pvar->info_test_path[0] = 0;
	pvar->info_test = NULL;
	for (i = 0; i < 4; i++)
	{
		pvar->test_file[i][0] = 0;
		pvar->test[i] = NULL;
	}

	pvar->info_cnt = 0;
	pvar->info_cnt_ok = 0;
	pvar->info_cnt_ng = 0;

	pvar->result_file[0] = 0;
}

///////////////////////////////////////////////////////////////

//static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks) {
//  printf("TTXGetUIHooks %d\n", ORDER);
//	return;
//}

///////////////////////////////////////////////////////////////

static void PASCAL TTXReadIniFile(TT_LPCTSTR fn, PTTSet ts)
{
	TCHAR name[20];
	LPTSTR buf;
	size_t buf_sz;
	LPTSTR ctx;
	LPTSTR p;
	int seq, nxt, cmd, sub;
	size_t sz;
	int i;

	if (!pvar->skip)
		(pvar->origReadIniFile)(fn, ts);

	buf = NULL;

	//auto start
	pvar->ReportAutoStart = GetIniOnOff(_T(INISECTION), _T("ReportAutoStart"), FALSE, fn);

	//report window size
	buf_sz = GetIniString(_T(INISECTION), _T("ReportSize"), _T(""), &buf, 128, 64, fn);
	if (buf_sz)
	{
		p = _tcstok_s(buf, _T(", "), &ctx);
		if (p)
			pvar->report_win_size.x = _tstoi(p);
		p = _tcstok_s(NULL, _T(", "), &ctx);
		if (p)
			pvar->report_win_size.y = _tstoi(p);
	}
	//report window font
	buf_sz = GetIniString(_T(INISECTION), _T("ReportFont"), _T(""), &buf, 128, 64, fn);
	if (buf_sz)
	{
		p = _tcstok_s(buf, _T(","), &ctx);
		if (p)
			_tcscpy_s(pvar->report_font, LF_FACESIZE, p);
		p = _tcstok_s(NULL, _T(", "), &ctx);
		if (p)
			pvar->report_font_size.y = _tstoi(p);
		p = _tcstok_s(NULL, _T(", "), &ctx);
		if (p)
			pvar->report_font_charset = _tstoi(p);
	}
	//report window title
	GetPrivateProfileString(_T(INISECTION), _T("ReportTitle"), _T(""),
							pvar->report_title, sizeof(pvar->report_title)/sizeof(TCHAR), fn);
	GetPrivateProfileString(_T(INISECTION), _T("ReportNote"), _T(""),
							pvar->report_note_path, sizeof(pvar->report_note_path)/sizeof(TCHAR), fn);
	//test pattern
	// GetPrivateProfileString(_T(INISECTION), _T("ReportClear"), _T(""),
	// 						pvar->report_clear, sizeof(pvar->report_clear)/sizeof(TCHAR), fn);
	buf_sz = GetIniString(_T(INISECTION), _T("ReportClear"), _T(""), &buf, 128, 64, fn);
	if (buf_sz)
	{
		LPSTR p = toMB(buf);
		strcpy_s(pvar->report_clear, buf_sz, p);
		TTXFree(&p);
	}

	GetPrivateProfileString(_T(INISECTION), _T("InfoTest"), _T(""),
							pvar->info_test_path, sizeof(pvar->info_test_path)/sizeof(TCHAR), fn);
	//test rule
	pvar->report_rule_num = 0;
	for (i = 0; i < REPORT_RULE_NUM; i++)
	{
		_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("ReportRule%d"), i + 1);
		GetIniString(_T(INISECTION), name, _T(""), &buf, 128, 64, fn);
		if (!buf[0])
			continue;
		seq = nxt = sub = cmd = 0;
		p = _tcstok_s(buf, _T(", "), &ctx);
		if (p)
			seq = _tstoi(p);
		p = _tcstok_s(NULL, _T(", "), &ctx);
		if (p)
			nxt = _tstoi(p);
		p = _tcstok_s(NULL, _T(", "), &ctx);
		if (p)
			sub = _tstoi(p);
		p = _tcstok_s(NULL, _T(", "), &ctx);
		if (p)
			cmd = _tstoi(p);
		if (ctx)
		{
			sz = _tcscnlen(ctx, buf_sz);
			if (sz > 0)
			{
				pvar->report_rule[i].seq = seq;
				pvar->report_rule[i].nxt = nxt;
				pvar->report_rule[i].sub = sub;
				pvar->report_rule[i].cmd = cmd;
				if (pvar->report_rule[i].ptn != NULL)
				{
					free(pvar->report_rule[i].ptn);
					pvar->report_rule[i].ptn = NULL;
				}
				sz++;
				pvar->report_rule[i].ptn = toMB(ctx);
				pvar->report_rule_num = i + 1;
			}
		}
	}
	//pattern file
	for (i = 0; i < 4; i++)
	{
		_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("InfoTest%d"), i + 1);
		GetPrivateProfileString(_T(INISECTION), name, _T(""),
								pvar->test_file[i], sizeof(pvar->test_file[i])/sizeof(TCHAR), fn);
	}
	//結果ファイル
	GetPrivateProfileString(_T(INISECTION), _T("InfoResult"), _T(""),
							pvar->result_file, sizeof(pvar->result_file)/sizeof(TCHAR), fn);

	if (buf)
		free(buf);
}

static void PASCAL TTXWriteIniFile(TT_LPCTSTR fn, PTTSet ts)
{
	TCHAR name[20];
	LPTSTR buf;
	int buf_sz;
	int i;
	LPTSTR p;

	(pvar->origWriteIniFile)(fn, ts);

	buf_sz = MAX_PATH;
	buf = malloc(buf_sz*sizeof(TCHAR));

	WriteIniOnOff(_T(INISECTION), _T("ReportAutoStart"), pvar->ReportAutoStart, TRUE, fn);

	if (pvar->report_win_size.x || pvar->report_win_size.y)
	{
		_sntprintf_s(buf, buf_sz, _TRUNCATE, _T("%d,%d"),
					pvar->report_win_size.x,
					pvar->report_win_size.y);
		WritePrivateProfileString(_T(INISECTION), _T("ReportSize"), buf, fn);
	}
	if (pvar->report_font[0])
	{
		_sntprintf_s(buf, buf_sz, _TRUNCATE, _T("%s,%d,%d"),
					pvar->report_font,
					pvar->report_font_size.y,
					pvar->report_font_charset);
		WritePrivateProfileString(_T(INISECTION), _T("ReportFont"), buf, fn);
	}
	if (pvar->report_title[0])
	{
		WritePrivateProfileString(_T(INISECTION), _T("ReportTitle"), pvar->report_title, fn);
	}
	if (pvar->report_note_path[0])
	{
		WritePrivateProfileString(_T(INISECTION), _T("ReportNote"), pvar->report_note_path, fn);
	}
	if (pvar->report_clear[0])
	{
		LPTSTR p;
		p = toTC(pvar->report_clear);
		WritePrivateProfileString(_T(INISECTION), _T("ReportClear"), p, fn);
		TTXFree(&p);
	}
	if (pvar->info_test_path[0])
	{
		WritePrivateProfileString(_T(INISECTION), _T("InfoTest"), pvar->info_test_path, fn);
	}
	for (i = 0; i < REPORT_RULE_NUM; i++)
	{
		if (pvar->report_rule[i].ptn && pvar->report_rule[i].ptn[0])
		{
			_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("ReportRule%d"), i + 1);
			p = toTC(pvar->report_rule[i].ptn);
			_sntprintf_s(buf, buf_sz, _TRUNCATE, _T("%d,%d,%d,%d,%s"),
						pvar->report_rule[i].seq,
						pvar->report_rule[i].nxt,
						pvar->report_rule[i].sub,
						pvar->report_rule[i].cmd,
						p);
			TTXFree(&p);
			WritePrivateProfileString(_T(INISECTION), name, buf, fn);
		}
	}
	for (i = 0; i < 4; i++)
	{
		if (pvar->test_file[i][0])
		{
			_sntprintf_s(name, sizeof(name)/sizeof(name[0]), _TRUNCATE, _T("InfoTest%d"), i + 1);
			WritePrivateProfileString(_T(INISECTION), name, pvar->test_file[i], fn);
		}
	}
	if (pvar->result_file[0])
	{
		WritePrivateProfileString(_T(INISECTION), _T("InfoResult"), pvar->result_file, fn);
	}

	if (buf)
		free(buf);
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
//メッセージ領域

void SetInfoMsg(HWND hWnd, LPCTSTR szText, DWORD dwSize)
{
	WINDOWINFO wi;
	RECT rcMsg;
	RECT rcLog;
	LONG x0, y0, x, y, w, h, dy;

	GetWindowInfo(hWnd, &wi);
	x0 = wi.rcClient.left;
	y0 = wi.rcClient.top;

	HWND hTitle = GetDlgItem(hWnd, IDC_MSG);
	GetClientRect(hTitle, &rcMsg);
	h = rcMsg.bottom;

	HFONT hFont, hFontOld;
	hFont = (HFONT)SendMessage(hTitle, WM_GETFONT, 0, 0);
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hTitle, &ps);
	hFontOld = SelectObject(hDC, hFont);
	dy = DrawText(hDC, szText, dwSize, &rcMsg,
				   DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_EXPANDTABS | DT_CALCRECT);
	SelectObject(hDC, hFontOld);
	EndPaint(hTitle, &ps);

	GetWindowRect(hTitle, &rcMsg);
	x = rcMsg.left - x0;
	y = rcMsg.top - y0;
	w = rcMsg.right - rcMsg.left;
	h = rcMsg.bottom - rcMsg.top - h + dy;
	MoveWindow(hTitle, x, y, w, h, TRUE);

	SetDlgItemText(hWnd, IDC_MSG, szText);
	dy = h - rcMsg.bottom + rcMsg.top;

	HWND hLog = GetDlgItem(report_dialog, IDC_LOG);
	GetWindowRect(hLog, &rcLog);
	x = rcLog.left - x0;
	y = rcLog.top - y0 + dy;
	w = rcLog.right - rcLog.left;
	h = rcLog.bottom - rcLog.top - dy;
	MoveWindow(hLog, x, y, w, h, TRUE);
}

VOID UpdateReportMsg(HWND hWnd)
{
	TCHAR buf[1024];
	size_t path_sz;
	LPTSTR path;
	DWORD dwSize;
	HANDLE hFile;
	LPTSTR p;
	size_t sz;

	memset(buf, 0, 1024*sizeof(TCHAR));
	if (pvar->report_title[0])
	{
		_tcsncpy_s(buf, 1024, pvar->report_title, _TRUNCATE);
		_tcsncat_s(buf, 1024, _T("\r\n"), _TRUNCATE);
	}

	p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
	path_sz = _tcsnlen(pvar->report_note_path, _TRUNCATE) + _tcsnlen(p, _TRUNCATE) + 2;
	path = (LPTSTR)malloc(path_sz*sizeof(TCHAR));
	GetAbsolutePath(path, path_sz, pvar->report_note_path, p);
	TTXFree(&p);
	hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	free(path);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		sz = _tcsnlen(buf, _TRUNCATE);
		if (ReadFile(hFile, buf + sz, 1024 - (DWORD)sz, &dwSize, NULL))
			buf[sz + dwSize - 1] = _T('\0');
		CloseHandle(hFile);
	}

	if (pvar->ChangeReport)
		SetInfoMsg(report_dialog, buf, dwSize);
}

///////////////////////////////////////////////////////////////
//テストパターン

VOID LoadInfoTest()
{
	int i;

	LoadStringList(&(pvar->info_test), pvar->info_test_path);
	for (i = 0; i < 4; i++)
	{
		LoadStringList(&(pvar->test[i]), pvar->test_file[0]);
	}
}

void EditInfoTest()
{
	TCHAR command[MAX_PATH];
	LPTSTR file;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	file = pvar->info_test_path;
	if (!file || !file[0])
		return;

	memset(&si, 0, sizeof(si));
	GetStartupInfo(&si);
	memset(&pi, 0, sizeof(pi));

	_sntprintf_s(command, sizeof(command)/sizeof(command[0]), 
		_TRUNCATE, _T("%s %s"), _T("notepad.exe"), file);

	if (CreateProcess(NULL, command, NULL, NULL, FALSE, 0,
					  NULL, NULL, &si, &pi) == 0)
	{
		TCHAR buf[80];
		_sntprintf_s(buf, sizeof(buf)/sizeof(buf[0]), 
			_TRUNCATE, _T("Can't open test file. (%d)"), 
			GetLastError());
		MessageBox(0, buf, _T("ERROR"), MB_OK | MB_ICONWARNING);
	}
}

///////////////////////////////////////////////////////////////
//ログ領域

void ClearReportLog()
{
	SetDlgItemText(report_dialog, IDC_LOG, _T(""));
	pvar->info_seq = 0;
	pvar->info_cnt = 0;
	pvar->info_cnt_ok = 0;
	pvar->info_cnt_ng = 0;
	pvar->ClearReq = FALSE;
}

void SetInfoLog(LPTSTR szText, DWORD dwSize)
{
	HWND hWnd = GetDlgItem(report_dialog, IDC_LOG);
	if (pvar->ClearReq)
	{
		SetDlgItemText(report_dialog, IDC_LOG, szText);
		pvar->ClearReq = FALSE;
	}
	else
	{
		int end = GetWindowTextLength(hWnd);
		if (end > 65536)
		{
			SetDlgItemText(report_dialog, IDC_LOG, szText);
			return;
		}
		SendMessage(hWnd, EM_SETSEL, end, end);
		SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)szText);
	}
}

void SaveInfoLog(LPTSTR szFile)
{
	TCHAR buf[1024];
	time_t time_local;
	struct tm tm_local;
	HANDLE hFile;
	DWORD dwSize;
	size_t sz;

	if (!pvar->ChangeReport)
		return;

	hFile = CreateFile(szFile, GENERIC_WRITE, 0, NULL,
					   OPEN_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	SetFilePointer(hFile, 0, NULL, FILE_END);

	GetDlgItemText(report_dialog, IDC_LOG, buf, 1024);
	sz = _tcsnlen(buf, _TRUNCATE);
	if (sz)
		WriteFile(hFile, buf, (DWORD)sz, &dwSize, NULL);

	GetDlgItemText(report_dialog, IDC_STATUS, buf, 1024);
	sz = _tcsnlen(buf, _TRUNCATE);
	if (sz)
		WriteFile(hFile, buf, (DWORD)sz, &dwSize, NULL);

	time(&time_local);
	localtime_s(&tm_local, &time_local);
	_tcsftime(buf, sizeof(buf)/sizeof(buf[0]), 
		_T("\r\n%Y-%m-%d %H:%M:%S\r\n\r\n"), &tm_local);
	WriteFile(hFile, buf, (DWORD)_tcsnlen(buf, _TRUNCATE), 
		&dwSize, NULL);

	CloseHandle(hFile);
}

///////////////////////////////////////////////////////////////
//ステータス領域

void SetInfoStatus()
{
	TCHAR tmp[256];
	if (pvar->ChangeReport)
	{
		HWND hWnd = GetDlgItem(report_dialog, IDC_STATUS);
		_sntprintf_s(tmp, sizeof(tmp)/sizeof(tmp[0]), _TRUNCATE, 
			_T("SEQ:%d  CNT:%d  OK:%d  NG:%d"),	
			pvar->info_seq, pvar->info_cnt, pvar->info_cnt_ok, pvar->info_cnt_ng);
		SetDlgItemText(report_dialog, IDC_STATUS, tmp);
	}
}

///////////////////////////////////////////////////////////////
//Report ダイアログ

#define WM_SEND_UPDATE1 (WM_USER + 1)
#define WM_SEND_UPDATE2 (WM_USER + 2)

static LRESULT CALLBACK report_dlg_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static POINT ptMsg, ptLog, ptBtn, ptSts;
	static HFONT hFont;
	static UINT ucnt = 0;

	switch (msg)
	{
	case WM_INITDIALOG:
		SetDlgFont(hWnd, IDC_MSG, &hFont, pvar->report_font_size.y, pvar->report_font);
		SetDlgFont(hWnd, IDC_LOG, &hFont, pvar->report_font_size.y, pvar->report_font);
		GetPointRB(hWnd, IDC_MSG, &ptMsg);
		GetPointRB(hWnd, IDC_LOG, &ptLog);
		GetPointRB(hWnd, IDOK, &ptBtn);
		GetPointRB(hWnd, IDC_STATUS, &ptSts);
		pvar->ChangeReport = TRUE;
		LoadInfoTest();
		SetWindowSize(hWnd, &(pvar->report_win_size));
		SetFocus(GetDlgItem(hWnd, IDC_LOG));
		return TRUE;

	case WM_SIZE:
		MovePointRB(hWnd, IDC_MSG, &ptMsg, RB_RIGHT);
		MovePointRB(hWnd, IDC_LOG, &ptLog, RB_RIGHT | RB_BOTTOM);
		MovePointRB(hWnd, IDOK, &ptBtn, RB_TOP | RB_BOTTOM);
		MovePointRB(hWnd, IDC_STATUS, &ptSts, RB_RIGHT | RB_TOP | RB_BOTTOM);
		GetWindowSize(hWnd, &(pvar->report_win_size));
		UpdateReportMsg(hWnd);
		return TRUE;

	case WM_ACTIVATE:
		if (LOWORD(wp))
			SetFocus(GetParent(hWnd));
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wp))
		{
		case IDOK:
			pvar->ChangeReport = FALSE;
			EndDialog(hWnd, 0);
			return TRUE;

		case IDCANCEL:
			pvar->ChangeReport = FALSE;
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		pvar->ChangeReport = FALSE;
		EndDialog(hWnd, 0);
		return TRUE;

	case WM_DESTROY:
		pvar->ChangeReport = FALSE;
		DeleteObject(hFont);
		return TRUE;
	}
	return FALSE;
}

static BOOL OpenReport(HWND hWin)
{
	if (!pvar->ChangeReport)
	{
		if (report_dialog)
		{
			DestroyWindow(report_dialog);
			report_dialog = 0;
		}
		report_dialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_REPORT_DIALOG),
									 hWin, (DLGPROC)report_dlg_proc);
		SetHomePosition(report_dialog, hWin, pvar->report_win_pos);
		UpdateReportMsg(report_dialog);
		SetFocus(GetParent(report_dialog));
	}
	return TRUE;
}
static BOOL CloseReport()
{
	if (pvar->ChangeReport)
	{
		PostMessage(report_dialog, WM_CLOSE, 0, MAKELPARAM(0, 0));
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////

//受信処理
void ttx_recv(char *rstr, int rcnt)
{
	static char buff[InBuffSize];
	static unsigned int blen;
	int i, j;
	char ch;
	LPTSTR p;

	for (i = 0; i < rcnt; i++)
	{
		ch = rstr[i];
		if (ch < 0 || ch > 0x20)
		{
			buff[blen++] = ch;
			if (blen >= InBuffSize)
				blen = 0;
			continue;
		}
		if (ch == 0x20)
		{
			if (blen && buff[blen - 1] != 0x20)
			{
				buff[blen++] = 0x20;
				if (blen >= InBuffSize)
					blen = 0;
			}
			continue;
		}
		if (blen == 0)
			continue;
		if (buff[blen - 1] == 0x20)
			blen--;
		buff[blen] = 0;
		if (blen == 0)
			continue;

		//クリアテスト
		if (pvar->report_clear[0] && strstr(buff, pvar->report_clear))
		{
			if (pvar->ChangeReport)
			{
				ClearReportLog();
				SetInfoStatus();
			}
			blen = 0;
			continue;
		}

		//パターンテスト
		if (info_test_match_head(pvar->info_test, buff))
		{
			if (pvar->ChangeReport)
			{
				buff[blen++] = '\r';
				buff[blen++] = '\n';
				buff[blen] = '\0';
				p = toTC(buff);
				SetInfoLog(p, blen);
				TTXFree(&p);
			}
			blen = 0;
			continue;
		}

		BOOL flg = FALSE;
		int seq = pvar->info_seq;
		int num = pvar->report_rule_num;
		for (j = 0; j < num; j++)
		{
			PInfoCmd item = &(pvar->report_rule[j]);
			if (item->seq != 0 && (item->seq & seq) == 0)
				continue;
			if (!item->ptn || !item->ptn[0] || !strstr(buff, item->ptn))
				continue;

			if (item->nxt && seq != item->nxt)
			{
				seq = item->nxt;
				if (seq < 0 || seq >= REPORT_RULE_NUM)
					seq = 0;
				pvar->info_seq = seq;
			}

			if (item->sub)
			{
				BOOL flg = FALSE;
				for (i = 0; i < 4; i++)
				{
					if (!flg && item->sub & (0x01 << i))
						flg = info_test_match(pvar->test[i], buff);
				}
				if (flg)
					pvar->info_cnt_ok++;
				else
					pvar->info_cnt_ng++;
			}

			int cmd = item->cmd;
			if (cmd & INFO_CMD_COUNT) //count
			{
				pvar->info_cnt++;
			}
			if (pvar->ChangeReport)
			{
				SetInfoStatus();
				if (cmd & INFO_CMD_SAVE && pvar->result_file[0])
					SaveInfoLog(pvar->result_file);

				if (!(cmd & INFO_CMD_NODSP)) //disable print
				{
					buff[blen++] = '\r';
					buff[blen++] = '\n';
					buff[blen] = '\0';
					p = toTC(buff);
					SetInfoLog(p, blen);
					TTXFree(&p);
				}

				if (cmd & INFO_CMD_CLEAR) //clear
					pvar->ClearReq = TRUE;
			}
			break;
		}

		blen = 0;
	}
}

int PASCAL TTXrecv(SOCKET s, char *buff, int len, int flags)
{
	int rlen;

	if ((rlen = pvar->origPrecv(s, buff, len, flags)) > 0)
	{
		ttx_recv(buff, rlen);
	}
	return rlen;
}

BOOL PASCAL TTXReadFile(HANDLE fh, LPVOID buff, DWORD len, LPDWORD rbytes, LPOVERLAPPED rol)
{
	BOOL result;

	if ((result = pvar->origPReadFile(fh, buff, len, rbytes, rol)) != FALSE)
	{
		ttx_recv(buff, *rbytes);
	}
	return result;
}

static void PASCAL TTXOpenTCP(TTXSockHooks *hooks)
{
	pvar->origPrecv = *hooks->Precv;
	*hooks->Precv = TTXrecv;
	if (pvar->ReportAutoStart && !pvar->ChangeReport)
	{
		OpenReport(GetActiveWindow());
	}
}

static void PASCAL TTXCloseTCP(TTXSockHooks *hooks)
{
	if (pvar->origPrecv)
	{
		*hooks->Precv = pvar->origPrecv;
	}
}

static void PASCAL TTXOpenFile(TTXFileHooks *hooks)
{
	pvar->origPReadFile = *hooks->PReadFile;
	*hooks->PReadFile = TTXReadFile;
	if (pvar->ReportAutoStart && !pvar->ChangeReport)
	{
		OpenReport(GetActiveWindow());
	}
}

static void PASCAL TTXCloseFile(TTXFileHooks *hooks)
{
	if (pvar->origPReadFile)
	{
		*hooks->PReadFile = pvar->origPReadFile;
	}
}

///////////////////////////////////////////////////////////////

//static void PASCAL TTXSetWinSize(int rows, int cols) {
//  printf("TTXSetWinSize %d\n", ORDER);
//}

///////////////////////////////////////////////////////////////

//
// ReportSettingProc設定ダイアログのコールバック関数。
//
static LRESULT CALLBACK ReportSettingProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPTSTR p;

	switch (msg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(dlg, IDC_MSG_NAME, pvar->report_title);
		SetDlgItemText(dlg, IDC_MSG_PATH, pvar->report_note_path);
		SetDlgItemInt(dlg, IDC_WND_W, pvar->report_win_size.x, FALSE);
		SetDlgItemInt(dlg, IDC_WND_H, pvar->report_win_size.y, FALSE);
		SetDlgItemInt(dlg, IDC_WND_POS, pvar->report_win_pos, FALSE);
		SendMessage(GetDlgItem(dlg, IDC_START), BM_SETCHECK,
					pvar->ReportAutoStart ? BST_CHECKED : BST_UNCHECKED, 0);
		SetDlgItemTextA(dlg, IDC_CLEAR, pvar->report_clear);
		SetDlgItemText(dlg, IDC_PATH7, pvar->info_test_path);
		SetDlgItemText(dlg, IDC_PATH1, pvar->test_file[0]);
		SetDlgItemText(dlg, IDC_PATH2, pvar->test_file[1]);
		SetDlgItemText(dlg, IDC_PATH3, pvar->test_file[2]);
		SetDlgItemText(dlg, IDC_PATH4, pvar->test_file[3]);
		SetDlgItemText(dlg, IDC_PATH6, pvar->result_file);
		MoveParentCenter(dlg);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_MSG_BTN:
			p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
			TTXDup(&p, MAX_PATH, NULL);
			RemoveFileName(p);
			OpenFileDlg(dlg, IDC_MSG_PATH, _T("説明ファイル"), _T(""), p, 1);
			TTXFree(&p);
			return TRUE;
		case IDC_BUTTON7:
			p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
			TTXDup(&p, MAX_PATH, NULL);
			RemoveFileName(p);
			OpenFileDlg(dlg, IDC_PATH7, _T("パターンファイル"), _T(""), p, 1);
			TTXFree(&p);
			return TRUE;
		case IDC_BUTTON1:
			p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
			TTXDup(&p, MAX_PATH, NULL);
			RemoveFileName(p);
			OpenFileDlg(dlg, IDC_PATH1, _T("パターンファイル1"), _T(""), p, 1);
			TTXFree(&p);
			return TRUE;
		case IDC_BUTTON2:
			p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
			TTXDup(&p, MAX_PATH, NULL);
			RemoveFileName(p);
			OpenFileDlg(dlg, IDC_PATH2, _T("パターンファイル2"), _T(""), p, 1);
			TTXFree(&p);
			return TRUE;
		case IDC_BUTTON3:
			p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
			TTXDup(&p, MAX_PATH, NULL);
			RemoveFileName(p);
			OpenFileDlg(dlg, IDC_PATH3, _T("パターンファイル3"), _T(""), p, 1);
			TTXFree(&p);
			return TRUE;
		case IDC_BUTTON4:
			p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
			TTXDup(&p, MAX_PATH, NULL);
			RemoveFileName(p);
			OpenFileDlg(dlg, IDC_PATH4, _T("パターンファイル4"), _T(""), p, 1);
			TTXFree(&p);
			return TRUE;
		case IDC_BUTTON6:
			p = TTXGetPath(pvar->ts, ID_SETUPFNAME);
			TTXDup(&p, MAX_PATH, NULL);
			RemoveFileName(p);
			OpenFileDlg(dlg, IDC_PATH6, _T("結果出力ファイル"), _T(""), p, 1);
			TTXFree(&p);
			return TRUE;

		case IDOK:
			GetDlgItemText(dlg, IDC_MSG_NAME, pvar->report_title, sizeof(pvar->report_title));
			GetDlgItemText(dlg, IDC_MSG_PATH, pvar->report_note_path, sizeof(pvar->report_note_path));
			pvar->report_win_size.x = GetDlgItemInt(dlg, IDC_WND_W, NULL, FALSE);
			pvar->report_win_size.y = GetDlgItemInt(dlg, IDC_WND_H, NULL, FALSE);
			pvar->report_win_pos = GetDlgItemInt(dlg, IDC_WND_POS, NULL, FALSE);
			GetDlgItemText(dlg, IDC_PATH1, pvar->test_file[0], sizeof(pvar->test_file[0]));
			GetDlgItemText(dlg, IDC_PATH2, pvar->test_file[1], sizeof(pvar->test_file[1]));
			GetDlgItemText(dlg, IDC_PATH3, pvar->test_file[2], sizeof(pvar->test_file[2]));
			GetDlgItemText(dlg, IDC_PATH4, pvar->test_file[3], sizeof(pvar->test_file[3]));
			GetDlgItemTextA(dlg, IDC_CLEAR, pvar->report_clear, sizeof(pvar->report_clear));
			pvar->ReportAutoStart = IsDlgButtonChecked(dlg, IDC_START) == BST_CHECKED;
			GetDlgItemText(dlg, IDC_PATH6, pvar->result_file, sizeof(pvar->result_file));
			GetDlgItemText(dlg, IDC_PATH7, pvar->info_test_path, sizeof(pvar->info_test_path));

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

// メニュー /////////////////////////////////////////////////////////////

static void PASCAL TTXModifyMenu(HMENU menu)
{
	UINT flag;
	UINT lang;
	LPTSTR s;

	flag = MF_ENABLED;
	lang = UILang(pvar->ts->UILanguageFile);

	pvar->SetupMenu = GetSubMenu(menu, ID_SETUP);
	AppendMenu(pvar->SetupMenu, MF_SEPARATOR, 0, NULL);
	s = (lang == 2) ? _T("受信レポート(&Q)...") : _T("Report setup...");
	AppendMenu(pvar->SetupMenu, flag, TTXMenuID(ID_MENUITEM3), s);

	pvar->ControlMenu = GetSubMenu(menu, ID_CONTROL);
	AppendMenu(pvar->ControlMenu, MF_SEPARATOR, 0, NULL);
	s = (lang == 2) ? _T("受信レポート(&V)...") : _T("&View Report");
	AppendMenu(pvar->ControlMenu, flag, TTXMenuID(ID_MENUITEM), s);
	s = (lang == 2) ? _T("受信レポートクリア(&L)...") : _T("C&lear Report");
	AppendMenu(pvar->ControlMenu, flag, TTXMenuID(ID_MENUITEM1), s);
	s = (lang == 2) ? _T("テストパターン編集...") : _T("Edit &Test Pattern");
	AppendMenu(pvar->ControlMenu, flag, TTXMenuID(ID_MENUITEM2), s);
}

static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
	UINT uflag;
	if (menu == pvar->SetupMenu || menu == pvar->ControlMenu)
	{
		uflag = MF_BYCOMMAND | (pvar->ChangeReport ? MF_CHECKED : 0);
		CheckMenuItem(menu, TTXMenuID(ID_MENUITEM), uflag);
		uflag = MF_BYCOMMAND | (pvar->ChangeReport ? MF_ENABLED : MF_GRAYED);
		EnableMenuItem(menu, TTXMenuID(ID_MENUITEM1), uflag);
	}
}

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (TTXMenuOrgID(cmd))
	{
	case ID_MENUITEM: //view report
		if (pvar->ChangeReport)
		{
			CloseReport();
		}
		else
		{
			OpenReport(hWin);
		}
		return 1;

	case ID_MENUITEM1: //clear report
		if (pvar->ChangeReport)
		{
			ClearReportLog();
			UpdateReportMsg(hWin);
			SetHomePosition(report_dialog, hWin, pvar->report_win_pos);
		}
		return 1;

	case ID_MENUITEM2: //open report editor
		EditInfoTest();
		return 1;

	case ID_MENUITEM3: //setup dialog
		switch (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_SETUP_REPORT),
							   hWin, ReportSettingProc, (LPARAM)NULL))
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

//static void PASCAL TTXEnd(void) {
//  printf("TTXEnd %d\n", ORDER);
//}

//static void PASCAL TTXSetCommandLine(PCHAR cmd, int cmdlen, PGetHNRec rec) {
//  printf("TTXSetCommandLine %d\n", ORDER);
//}

///////////////////////////////////////////////////////////////

static TTXExports Exports = {
	/* This must contain the size of the structure. See below for its usage. */
	sizeof(TTXExports),

	ORDER,

	/* Now we just list the functions that we've implemented. */
	TTXInit,
	NULL, //TTXGetUIHooks,
	TTXGetSetupHooks,
	TTXOpenTCP,
	TTXCloseTCP,
	NULL, //TTXSetWinSize,
	TTXModifyMenu,
	TTXModifyPopupMenu,
	TTXProcessCommand,
	NULL, //TTXEnd,
	NULL, //TTXSetCommandLine,
	TTXOpenFile,
	TTXCloseFile,
};

BOOL __declspec(dllexport) PASCAL TTXBind(WORD Version, TTXExports *exports)
{
	int size = sizeof(Exports) - sizeof(exports->size);
	/* do version checking if necessary */
	/* if (Version!=TTVERSION) return FALSE; */

	if (TTXIgnore(ORDER, _T(INISECTION), Version))
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
