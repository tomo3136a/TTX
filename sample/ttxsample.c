/*
 * Tera Term {xxx} Extension
 * (C) {year} {author}
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt_res.h"
//#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
//{system_includes here}

#include "ttxcommon.h"
#include "resource.h"

//{order_number}
#define ORDER 6000

#define INISECTION "TTX{xxx}"

//{menu nunber}
#define ID_MENUITEM 56000+0
#define ID_MENUITEM1 (ID_MENUITEM + 1)
#define ID_MENUITEM2 (ID_MENUITEM + 2)
//{menuid...}

//{other_define...}

static HANDLE hInst; /* Instance handle of TTX*.DLL */
//{global_variable}

//{enum, structure...}

typedef struct
{
	PTTSet ts;
	PComVar cv;
	BOOL skip;		//{if use ParseParam hook}

	//menu
	HMENU SetupMenu; //{when use Setup menu}
	//{other menu handle...}

	//callback
	Trecv origPrecv;		 //{when use recv Hook}
	TReadFile origPReadFile; //{when use ReadFile Hook}

	PReadIniFile origReadIniFile;	//{when use ReadIniFile Hook}
	PWriteIniFile origWriteIniFile; //{when use WriteIniFile Hook}
	PParseParam origParseParam;		//{when use command-line option and start-up ReadIniFile Hook}
									//{other Hook callback...}

	//setup
	//{setup file member...}

	//status
	//{status member...}

} TInstVar;

static TInstVar *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	pvar->ts = ts;
	pvar->cv = cv;
	pvar->skip = FALSE; //{when use ParseParam hook}

	//clear hook callback
	pvar->origPrecv = NULL;		//{when use recv Hook}
	pvar->origPReadFile = NULL; //{when use ReadFile Hook}
								//{other clear hook...}

	//initialize setup and status member
	//{initialize setup member...}
	//{initialize status member...}
}

///////////////////////////////////////////////////////////////

//{when use UI hook}
//static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks) {
//  printf("TTXGetUIHooks %d\n", ORDER);
//	return;
//}

///////////////////////////////////////////////////////////////

//{when use ReadIniFile hook}
static void PASCAL TTXReadIniFile(TT_LPCTSTR FName, PTTSet ts)
{
	TCHAR s[20]; //{use fixed size string}

	LPTSTR buf;	//{use flexible string}
	int buf_sz; //{use flexible string}

	LPTSTR ctx; //{use string parse ","}
	LPTSTR p;

	int seq, nxt, cmd, sub, sz;
	int i;

	if (!pvar->skip) //{when use ParseParam hook}
		(pvar->origReadIniFile)(FName, ts);

	buf = NULL; //{use flexible string, pointer initialize}

	//on/off setting
	//pvar->{name} = GetIniOnOff(_T(INISECTION), _T("{name}"), FALSE, FName);

	//string setting(fixed size)
	//GetPrivateProfileString(_T(INISECTION), _T("{name}"), {initialze string},
	//	pvar->{name}, sizeof(pvar->{name})/sizeof(pvar->{name}[0]), FName);

	//string setting(flexible size)
	//GetIniString(_T(INISECTION), _T("{name}"), _T(""), &buf, 128, 64, FName);

	//{use string parse with ","}
	if (buf && buf[0])
	{
		p = _tcstok_s(buf, _T(", "), &ctx); //{first token}
		//{fixed string}
		//if (p)
		//	_tcscpy_s(pvar->{name}, sizeof(pvar->{name})/sizeof(pvar->{name}[0]), p);

		p = _tcstok_s(NULL, _T(", "), &ctx); //{next tolen}
											//{number}
											//if (p)
											//	pvar->{name} = atoi(p);

		//{...}
	}

	if (buf)	   //{use flexible string}
		free(buf); //{use flexible string}
}

//{when use WriteIniFile hook}
static void PASCAL TTXWriteIniFile(TT_LPTSTR FName, PTTSet ts)
{
	TCHAR name[20]; //{valiable member name}

	LPTSTR buf;	//{use string}
	int buf_sz; //{use string}

	(pvar->origWriteIniFile)(FName, ts);

	buf_sz = MAX_PATH;
	buf = (LPTSTR)malloc(buf_sz*sizeof(TCHAR));

	//{write on/off to setup-file}
	//WriteIniOnOff(_T(INISECTION), _T("{name}"), pvar->{name}, TRUE, FName);

	//{write string to setup-file}
	//WritePrivateProfileString(_T(INISECTION), _T("{name}"), buf, FName);

	//{...}

	if (buf)
		free(buf);
}

//{when use command-line option and start-up ReadIniFile Hook}
static void PASCAL TTXParseParam(TT_LPTSTR Param, PTTSet ts, PCHAR DDETopic)
{
	LPTSTR buf;
	int buf_sz;
	LPTSTR next;

	(pvar->origParseParam)(Param, ts, DDETopic);

	buf_sz = 4100;
	buf = (LPTSTR)malloc(buf_sz*sizeof(TCHAR));
	if (buf)
	{
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
			//{other command-line option parse...}

		}
		free(buf);
	}
}

//{when use setup function Hook}
static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks)
{
	//{when ReadIniFile Hook}
	//pvar->origReadIniFile = *hooks->ReadIniFile;
	//*hooks->ReadIniFile = TTXReadIniFile;

	//{when WriteIniFile Hook}
	//pvar->origWriteIniFile = *hooks->WriteIniFile;
	//*hooks->WriteIniFile = TTXWriteIniFile;

	//{when use command-line option and start-up ReadIniFile Hook}
	//pvar->origParseParam = *hooks->ParseParam;
	//*hooks->ParseParam = TTXParseParam;
}

///////////////////////////////////////////////////////////////

//{when use recv Hook or ReadFile Hook}
void ttx_recv(char *rstr, int rcnt)
{
	static char buf[InBuffSize];
	static unsigned int blen = 0;
	int i;
	char ch;

	for (i = 0; i < rcnt; i++)
	{
		ch = rstr[i];
		if (ch != 0)
		{
			buf[blen++] = ch;
			if (blen >= InBuffSize)
				blen = 0;
			continue;
		}
		buf[blen] = 0;
		//{parse receive buffer data}

		blen = 0;
	}
}

//{when use recv Hook}
int PASCAL TTXrecv(SOCKET s, char *buff, int len, int flags)
{
	int rlen;

	if ((rlen = pvar->origPrecv(s, buff, len, flags)) > 0)
	{
		ttx_recv(buff, rlen);
	}
	return rlen;
}

//{when use ReadFile Hook}
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

	//{open event procedure...}
}

static void PASCAL TTXCloseTCP(TTXSockHooks *hooks)
{
	if (pvar->origPrecv)
	{
		*hooks->Precv = pvar->origPrecv;
	}

	//{close event procedure...}
}

static void PASCAL TTXOpenFile(TTXFileHooks *hooks)
{
	pvar->origPReadFile = *hooks->PReadFile;
	*hooks->PReadFile = TTXReadFile;

	//{open event procedure...}
}

static void PASCAL TTXCloseFile(TTXFileHooks *hooks)
{
	if (pvar->origPReadFile)
	{
		*hooks->PReadFile = pvar->origPReadFile;
	}

	//{close event procedure...}
}

///////////////////////////////////////////////////////////////

//static void PASCAL TTXSetWinSize(int rows, int cols) {
//  printf("TTXSetWinSize %d\n", ORDER);
//}

///////////////////////////////////////////////////////////////

//
// SettingProc設定ダイアログのコールバック関数。
//
static LRESULT CALLBACK SettingProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPTSTR buf;
	int buf_sz;
	LPTSTR path;

	buf_sz = 4100;

	switch (msg)
	{
	case WM_INITDIALOG:
		//{initialize dialog}
		MoveParentCenter(dlg);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		//{use file selector}
		case IDC_BUTTON1:
			path = TTXGetPath(pvar->ts, ID_SETUPFNAME);
			buf = (LPTSTR)malloc(buf_sz*siezof(buf[0]));
			GetDlgItemText(dlg, IDC_PATH1, buf, buf_sz);
			OpenFileDlg(dlg, IDC_PATH1, _T("ファイル"), 
				_T("File(*.txt)\0*.txt\0"), buf, path, 0);
			free(buf);
			TTXFree(&path);
			return TRUE;

		//{use folder selector}
		case IDC_BUTTON2:
			buf = (LPTSTR)malloc(buf_sz*sizeof(buf[0]));
			OpenFolderDlg(dlg, IDC_PATH2, _T("フォルダ"), buf);
			free(buf);
			return TRUE;

		case IDOK:
			//{commit setting}
			EndDialog(dlg, IDOK);
			return TRUE;

		case IDCANCEL:
			//{cancel setting}
			EndDialog(dlg, IDCANCEL);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////

//{when use menu}
static void PASCAL TTXModifyMenu(HMENU menu)
{
	UINT flag;
	UINT lang;
	LPTSTR s;

	flag = MF_ENABLED;
	lang = UILang(pvar->ts->UILanguageFile);

	pvar->SetupMenu = GetSubMenu(menu, ID_SETUP);
	AppendMenu(pvar->SetupMenu, MF_SEPARATOR, 0, NULL);
	s = (lang == 2) ? _T("設定(&S)...") : _T("&setup...");
	AppendMenu(pvar->SetupMenu, flag, TTXMenuID(ID_MENUITEM), s);

	//{other menu insert/ppend...}

}

//{when use dinamical menu}
static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
	UINT uflag;

	if (menu == pvar->SetupMenu)
	{
		//{use check mark at menu}
		//uflag = MF_BYCOMMAND | (pvar->{name} ? MF_CHECKED : 0);
		//CheckMenuItem(menu, ID_MENUITEM + pvar->{name}, uflag);

		//{use enable/disable menu}
		//uflag = MF_BYCOMMAND | (pvar->{name} ? MF_ENABLED : MF_GRAYED);
		//EnableMenuItem(menu, ID_MENUITEM1 + pvar->{name}, uflag);
	}
}

//{when use to call menu process}
static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	switch (TTXMenuOrgID(cmd))
	{
	case ID_MENUITEM:
 		//setup dialog
 		switch (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_SETUP_xxx),
							   hWin, SettingProc, (LPARAM)NULL))
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

	case ID_MENUITEM1:
		//{menu process...}
		return 1;

	//{other menu entries...}
	}
	return 0;
}

///////////////////////////////////////////////////////////////

//{when use close tera term process}
//static void PASCAL TTXEnd(void) {
//  printf("TTXEnd %d\n", ORDER);
//}

//{not use?}
//static void PASCAL TTXSetCommandLine(TTX_LPTSTR cmd, int cmdlen, PGetHNRec rec) {
//  printf("TTXSetCommandLine %d\n", ORDER);
//}

///////////////////////////////////////////////////////////////

static TTXExports Exports = {
	/* This must contain the size of the structure. See below for its usage. */
	sizeof(TTXExports),

	ORDER,

	/* Now we just list the functions that we've implemented. */
	NULL, //TTXInit,
	NULL, //TTXGetUIHooks,
	NULL, //TTXGetSetupHooks,
	NULL, //TTXOpenTCP,
	NULL, //TTXCloseTCP,
	NULL, //TTXSetWinSize,
	NULL, //TTXModifyMenu,
	NULL, //TTXModifyPopupMenu,
	NULL, //TTXProcessCommand,
	NULL, //TTXEnd,
	NULL, //TTXSetCommandLine,
	NULL, //TTXOpenFile,
	NULL, //TTXCloseFile,
};

BOOL __declspec(dllexport) PASCAL TTXBind(WORD Version, TTXExports *exports)
{
	int size = sizeof(Exports) - sizeof(exports->size);
	/* do version checking if necessary */
	/* if (Version!=TTVERSION) return FALSE; */

	//{when TTXPlugin support}
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
