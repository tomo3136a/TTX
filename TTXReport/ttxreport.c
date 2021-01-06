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

#include "compat_w95.h"
#include "ttxcommon.h"
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
	int menuoffset;

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
	char report_font[LF_FACESIZE];
	POINT report_font_size;
	int report_font_charset;

	//message area
	char report_title[TitleBuffSize];
	char report_note_path[MAXPATHLEN];

	//report clear
	char report_clear[TitleBuffSize];
	BOOL ClearReq;

	//ピックアップ表示
	TInfoCmd report_rule[REPORT_RULE_NUM];
	int report_rule_num;
	char info_seq;
	char info_test_path[MAXPATHLEN];
	PStringList info_test;

	//テスト表示
	char test_file[4][MAXPATHLEN];
	PStringList test[4];

	//マッチカウンタ
	int info_cnt;
	int info_cnt_ok;
	int info_cnt_ng;

	//結果出力
	char result_file[MAXPATHLEN];

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

static void PASCAL TTXReadIniFile(PCHAR fn, PTTSet ts)
{
	char name[20];
	char *buf;
	int buf_sz;
	PCHAR ctx;
	PCHAR p;
	int seq, nxt, cmd, sub, sz;
	int i;

	if (!pvar->skip)
		(pvar->origReadIniFile)(fn, ts);

	buf = NULL;

	//auto start
	pvar->ReportAutoStart = GetIniOnOff(INISECTION, "ReportAutoStart", FALSE, fn);

	//report window size
	GetIniString(INISECTION, "ReportSize", "", &buf, 128, 64, fn);
	if (buf[0])
	{
		p = strtok_s(buf, ", ", &ctx);
		if (p)
			pvar->report_win_size.x = atoi(p);
		p = strtok_s(NULL, ", ", &ctx);
		if (p)
			pvar->report_win_size.y = atoi(p);
	}
	//report window font
	GetIniString(INISECTION, "ReportFont", "", &buf, 128, 64, fn);
	if (buf[0])
	{
		p = strtok_s(buf, ",", &ctx);
		if (p)
			strcpy_s(pvar->report_font, LF_FACESIZE, p);
		p = strtok_s(NULL, ", ", &ctx);
		if (p)
			pvar->report_font_size.y = atoi(p);
		p = strtok_s(NULL, ", ", &ctx);
		if (p)
			pvar->report_font_charset = atoi(p);
	}
	//report window title
	GetPrivateProfileString(INISECTION, "ReportTitle", "",
							pvar->report_title, sizeof(pvar->report_title), fn);
	GetPrivateProfileString(INISECTION, "ReportNote", "",
							pvar->report_note_path, sizeof(pvar->report_note_path), fn);
	//test pattern
	GetPrivateProfileString(INISECTION, "ReportClear", "",
							pvar->report_clear, sizeof(pvar->report_clear), fn);
	GetPrivateProfileString(INISECTION, "InfoTest", "",
							pvar->info_test_path, sizeof(pvar->info_test_path), fn);
	//test rule
	pvar->report_rule_num = 0;
	for (i = 0; i < REPORT_RULE_NUM; i++)
	{
		_snprintf_s(name, sizeof(name), _TRUNCATE, "ReportRule%d", i + 1);
		buf_sz = GetIniString(INISECTION, name, "", &buf, 128, 64, fn);
		if (!buf[0])
			continue;
		seq = nxt = sub = cmd = 0;
		p = strtok_s(buf, ", ", &ctx);
		if (p)
			seq = atoi(p);
		p = strtok_s(NULL, ", ", &ctx);
		if (p)
			nxt = atoi(p);
		p = strtok_s(NULL, ", ", &ctx);
		if (p)
			sub = atoi(p);
		p = strtok_s(NULL, ", ", &ctx);
		if (p)
			cmd = atoi(p);
		if (ctx)
		{
			sz = strnlen_s(ctx, buf_sz);
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
				pvar->report_rule[i].ptn = (PCHAR)malloc(sz);
				memset(pvar->report_rule[i].ptn, 0, sz);
				strcpy_s(pvar->report_rule[i].ptn, sz, ctx);
				pvar->report_rule_num = i + 1;
			}
		}
	}
	//pattern file
	for (i = 0; i < 4; i++)
	{
		_snprintf_s(name, sizeof(name), _TRUNCATE, "InfoTest%d", i + 1);
		GetPrivateProfileString(INISECTION, name, "",
								pvar->test_file[i], sizeof(pvar->test_file[i]), fn);
	}
	//結果ファイル
	GetPrivateProfileString(INISECTION, "InfoResult", "",
							pvar->result_file, sizeof(pvar->result_file), fn);

	if (buf)
		free(buf);
}

static void PASCAL TTXWriteIniFile(PCHAR fn, PTTSet ts)
{
	char name[20];
	char *buf;
	int buf_sz;
	int i;

	(pvar->origWriteIniFile)(fn, ts);

	buf_sz = MAX_PATH;
	buf = malloc(buf_sz);

	WriteIniOnOff(INISECTION, "ReportAutoStart", pvar->ReportAutoStart, TRUE, fn);

	if (pvar->report_win_size.x || pvar->report_win_size.y)
	{
		_snprintf_s(buf, buf_sz, _TRUNCATE, "%d,%d",
					pvar->report_win_size.x,
					pvar->report_win_size.y);
		WritePrivateProfileString(INISECTION, "ReportSize", buf, fn);
	}
	if (pvar->report_font[0])
	{
		_snprintf_s(buf, buf_sz, _TRUNCATE, "%s,%d,%d",
					pvar->report_font,
					pvar->report_font_size.y,
					pvar->report_font_charset);
		WritePrivateProfileString(INISECTION, "ReportFont", buf, fn);
	}
	if (pvar->report_title[0])
	{
		WritePrivateProfileString(INISECTION, "ReportTitle", pvar->report_title, fn);
	}
	if (pvar->report_note_path[0])
	{
		WritePrivateProfileString(INISECTION, "ReportNote", pvar->report_note_path, fn);
	}
	if (pvar->report_clear[0])
	{
		WritePrivateProfileString(INISECTION, "ReportClear", pvar->report_clear, fn);
	}
	if (pvar->info_test_path[0])
	{
		WritePrivateProfileString(INISECTION, "InfoTest", pvar->info_test_path, fn);
	}
	for (i = 0; i < REPORT_RULE_NUM; i++)
	{
		if (pvar->report_rule[i].ptn && pvar->report_rule[i].ptn[0])
		{
			_snprintf_s(name, sizeof(name), _TRUNCATE, "ReportRule%d", i + 1);
			_snprintf_s(buf, buf_sz, _TRUNCATE, "%d,%d,%d,%d,%s",
						pvar->report_rule[i].seq,
						pvar->report_rule[i].nxt,
						pvar->report_rule[i].sub,
						pvar->report_rule[i].cmd,
						pvar->report_rule[i].ptn);
			WritePrivateProfileString(INISECTION, name, buf, fn);
		}
	}
	for (i = 0; i < 4; i++)
	{
		if (pvar->test_file[i][0])
		{
			_snprintf_s(name, sizeof(name), _TRUNCATE, "InfoTest%d", i + 1);
			WritePrivateProfileString(INISECTION, name, pvar->test_file[i], fn);
		}
	}
	if (pvar->result_file[0])
	{
		WritePrivateProfileString(INISECTION, "InfoResult", pvar->result_file, fn);
	}

	if (buf)
		free(buf);
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
//メッセージ領域

void SetInfoMsg(HWND hWnd, char *szText, DWORD dwSize)
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
	dy = DrawTextA(hDC, szText, dwSize, &rcMsg,
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
	char tmp[1024];
	char path[MAX_PATH];
	DWORD dwSize;
	HANDLE hFile;
	int sz;

	memset(tmp, 0, sizeof(tmp) / sizeof(tmp[0]));
	if (pvar->report_title[0])
	{
		strncpy_s(tmp, 1024, pvar->report_title, _TRUNCATE);
		strncat_s(tmp, 1024, "\r\n", _TRUNCATE);
	}
	GetAbsolutePath(path, sizeof(path), pvar->report_note_path, pvar->ts->SetupFName);
	hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		sz = strnlen_s(tmp, _TRUNCATE);
		if (ReadFile(hFile, tmp + sz, 1024 - sz, &dwSize, NULL))
			tmp[sz + dwSize - 1] = '\0';
		CloseHandle(hFile);
	}

	if (pvar->ChangeReport)
		SetInfoMsg(report_dialog, tmp, dwSize);
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
	char command[MAX_PATH];
	char *file;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	file = pvar->info_test_path;
	if (!file || !file[0])
		return;

	memset(&si, 0, sizeof(si));
	GetStartupInfo(&si);
	memset(&pi, 0, sizeof(pi));

	_snprintf_s(command, sizeof(command), _TRUNCATE, "%s %s", "notepad.exe", file);

	if (CreateProcess(NULL, command, NULL, NULL, FALSE, 0,
					  NULL, NULL, &si, &pi) == 0)
	{
		char buf[80];
		_snprintf_s(buf, sizeof(buf), _TRUNCATE, "Can't open test file. (%d)", GetLastError());
		MessageBox(0, buf, "ERROR", MB_OK | MB_ICONWARNING);
	}
}

///////////////////////////////////////////////////////////////
//ログ領域

void ClearReportLog()
{
	SetDlgItemText(report_dialog, IDC_LOG, "");
	pvar->info_seq = 0;
	pvar->info_cnt = 0;
	pvar->info_cnt_ok = 0;
	pvar->info_cnt_ng = 0;
	pvar->ClearReq = FALSE;
}

void SetInfoLog(char *szText, DWORD dwSize)
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

void SaveInfoLog(LPSTR szFile)
{
	char buf[1024];
	time_t time_local;
	struct tm tm_local;
	HANDLE hFile;
	DWORD dwSize;
	int sz;

	if (!pvar->ChangeReport)
		return;

	hFile = CreateFile(szFile, GENERIC_WRITE, 0, NULL,
					   OPEN_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	SetFilePointer(hFile, 0, NULL, FILE_END);

	GetDlgItemText(report_dialog, IDC_LOG, buf, 1024);
	sz = strnlen_s(buf, _TRUNCATE);
	if (sz)
		WriteFile(hFile, buf, sz, &dwSize, NULL);

	GetDlgItemText(report_dialog, IDC_STATUS, buf, 1024);
	sz = strnlen_s(buf, _TRUNCATE);
	if (sz)
		WriteFile(hFile, buf, sz, &dwSize, NULL);

	time(&time_local);
	localtime_s(&tm_local, &time_local);
	strftime(buf, sizeof(buf), "\r\n%Y-%m-%d %H:%M:%S\r\n\r\n", &tm_local);
	WriteFile(hFile, buf, strnlen_s(buf, _TRUNCATE), &dwSize, NULL);

	CloseHandle(hFile);
}

///////////////////////////////////////////////////////////////
//ステータス領域

void SetInfoStatus()
{
	char tmp[256];
	if (pvar->ChangeReport)
	{
		HWND hWnd = GetDlgItem(report_dialog, IDC_STATUS);
		_snprintf_s(tmp, sizeof(tmp), _TRUNCATE, "SEQ:%d  CNT:%d  OK:%d  NG:%d",
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
				SetInfoLog(buff, blen);
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
					SetInfoLog(buff, blen);
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
	char *buf;
	int buf_sz;

	buf_sz = MAX_PATH;

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
		SetDlgItemText(dlg, IDC_CLEAR, pvar->report_clear);
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
			buf = malloc(buf_sz);
			GetDlgItemText(dlg, IDC_MSG_PATH, buf, buf_sz);
			OpenFileDlg(dlg, IDC_MSG_PATH, "説明ファイル", "", buf, pvar->ts->SetupFName, 1);
			free(buf);
			return TRUE;
		case IDC_BUTTON7:
			buf = malloc(buf_sz);
			GetDlgItemText(dlg, IDC_PATH7, buf, buf_sz);
			OpenFileDlg(dlg, IDC_PATH7, "パターンファイル", "", buf, pvar->ts->SetupFName, 1);
			free(buf);
			return TRUE;
		case IDC_BUTTON1:
			buf = malloc(buf_sz);
			GetDlgItemText(dlg, IDC_PATH1, buf, buf_sz);
			OpenFileDlg(dlg, IDC_PATH1, "パターンファイル1", "", buf, pvar->ts->SetupFName, 1);
			free(buf);
			return TRUE;
		case IDC_BUTTON2:
			buf = malloc(buf_sz);
			GetDlgItemText(dlg, IDC_PATH2, buf, buf_sz);
			OpenFileDlg(dlg, IDC_PATH2, "パターンファイル2", "", buf, pvar->ts->SetupFName, 1);
			free(buf);
			return TRUE;
		case IDC_BUTTON3:
			buf = malloc(buf_sz);
			GetDlgItemText(dlg, IDC_PATH3, buf, buf_sz);
			OpenFileDlg(dlg, IDC_PATH3, "パターンファイル3", "", buf, pvar->ts->SetupFName, 1);
			free(buf);
			return TRUE;
		case IDC_BUTTON4:
			buf = malloc(buf_sz);
			GetDlgItemText(dlg, IDC_PATH4, buf, buf_sz);
			OpenFileDlg(dlg, IDC_PATH4, "パターンファイル4", "", buf, pvar->ts->SetupFName, 1);
			free(buf);
			return TRUE;
		case IDC_BUTTON6:
			buf = malloc(buf_sz);
			GetDlgItemText(dlg, IDC_PATH6, buf, buf_sz);
			OpenFileDlg(dlg, IDC_PATH6, "結果出力ファイル", "", buf, pvar->ts->SetupFName, 1);
			free(buf);
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
			GetDlgItemText(dlg, IDC_CLEAR, pvar->report_clear, sizeof(pvar->report_clear));
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
	LPSTR s;

	flag = MF_ENABLED;
	lang = UILang(pvar->ts->UILanguageFile);
	pvar->menuoffset = MenuOffset(INISECTION, ID_MENUITEM, 0);

	pvar->SetupMenu = GetSubMenu(menu, ID_SETUP);
	AppendMenu(pvar->SetupMenu, MF_SEPARATOR, 0, NULL);
	s = (lang == 2) ? "受信レポート(&Q)..." : "Report setup...";
	AppendMenu(pvar->SetupMenu, flag, ID_MENUITEM3 + pvar->menuoffset, s);

	pvar->ControlMenu = GetSubMenu(menu, ID_CONTROL);
	AppendMenu(pvar->ControlMenu, MF_SEPARATOR, 0, NULL);
	s = (lang == 2) ? "受信レポート(&V)..." : "&View Report";
	AppendMenu(pvar->ControlMenu, flag, ID_MENUITEM + pvar->menuoffset, s);
	s = (lang == 2) ? "受信レポートクリア(&L)..." : "C&lear Report";
	AppendMenu(pvar->ControlMenu, flag, ID_MENUITEM1 + pvar->menuoffset, s);
	s = (lang == 2) ? "テストパターン編集..." : "Edit &Test Pattern";
	AppendMenu(pvar->ControlMenu, flag, ID_MENUITEM2 + pvar->menuoffset, s);
}

static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
	UINT uflag;
	if (menu == pvar->SetupMenu || menu == pvar->ControlMenu)
	{
		uflag = MF_BYCOMMAND | (pvar->ChangeReport ? MF_CHECKED : 0);
		CheckMenuItem(menu, ID_MENUITEM + pvar->menuoffset, uflag);
		uflag = MF_BYCOMMAND | (pvar->ChangeReport ? MF_ENABLED : MF_GRAYED);
		EnableMenuItem(menu, ID_MENUITEM1 + pvar->menuoffset, uflag);
	}
}

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (cmd + pvar->menuoffset)
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
			MessageBox(hWin, "Error", "Can't display dialog box.",
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
