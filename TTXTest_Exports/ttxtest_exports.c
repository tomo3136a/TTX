/*
 * Tera Term Test Exports
 * (C) 2021 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

#include "ttxcommon.h"
#include "ttxversion.h"
#include "ttxdebug.h"

#define ORDER 6020

#define INISECTION "TTXTest_Exports"

#define ID_MENUITEM 5801

#define TEST_FLAG_GETHNREC 0x0100
#define TEST_FLAG_COMM     0x0200

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct {
	PTTSet ts;
	PComVar cv;

	DWORD flag;

	/* ttdialog */
	PSetupTerminal SetupTerminal;
	PSetupWin SetupWin;
	PSetupKeyboard SetupKeyboard;
	PSetupSerialPort SetupSerialPort;
	PSetupTCPIP SetupTCPIP;
	PGetHostName GetHostName;
	PChangeDirectory ChangeDirectory;
	PAboutDialog AboutDialog;
	PChooseFontDlg ChooseFontDlg;
	PSetupGeneral SetupGeneral;
	PWindowWindow WindowWindow;
	PTTDLGSetUILanguageFile TTDLGSetUILanguageFile;

	/* ttsetup */
	PReadIniFile ReadIniFile;
	PWriteIniFile WriteIniFile;
	PReadKeyboardCnf ReadKeyboardCnf;
	PCopyHostList CopyHostList;
	PAddHostToList AddHostToList;
	PParseParam ParseParam;

	/* ttwsk */
	Tclosesocket Pclosesocket;
	Tconnect Pconnect;
	Thtonl Phtonl;
	Thtons Phtons;
	Tinet_addr Pinet_addr;
	Tioctlsocket Pioctlsocket;
	Trecv Precv;
	Tselect Pselect;
	Tsend Psend;
	Tsetsockopt Psetsockopt;
	Tsocket Psocket;
	TWSAAsyncSelect PWSAAsyncSelect;
	TWSAAsyncGetHostByName PWSAAsyncGetHostByName;
	TWSACancelAsyncRequest PWSACancelAsyncRequest;
	TWSAGetLastError PWSAGetLastError;
	Tfreeaddrinfo Pfreeaddrinfo;
	TWSAAsyncGetAddrInfo PWSAAsyncGetAddrInfo;

	/* ttfileio */
	TCreateFile PCreateFile;
	TCloseFile PCloseFile;
	TReadFile PReadFile;
	TWriteFile PWriteFile;

} TInstVar;

static TInstVar FAR *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

///////////////////////////////////////////////////////////////

static void PASCAL TTXInit(PTTSet ts, PComVar cv)
{
	pvar->ts = ts;
	pvar->cv = cv;

	pvar->flag = 0; //TEST_FLAG_GETHNREC | TEST_FLAG_COMM;

	pvar->PCreateFile = NULL;
	pvar->PCloseFile = NULL;
	pvar->PReadFile = NULL;
	pvar->PWriteFile = NULL;

	InitInfoView();
	DBG_VIEW(__FUNCTIONT__, _T("ts=%p cv=%p"), ts, cv);
}

///////////////////////////////////////////////////////////////

static BOOL PASCAL TTXSetupTerminal(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p ts=%p\r\n  (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupTerminal(WndParent, ts);
}

static BOOL PASCAL TTXSetupWin(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p ts=%p\r\n  (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupWin(WndParent, ts);
}

static BOOL PASCAL TTXSetupKeyboard(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p ts=%p\r\n  (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupKeyboard(WndParent, ts);
}

static BOOL PASCAL TTXSetupSerialPort(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p ts=%p\r\n  (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupSerialPort(WndParent, ts);
}

static BOOL PASCAL TTXSetupTCPIP(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p ts=%p\r\n  (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupTCPIP(WndParent, ts);
}

VOID dbg_gethnrec(PGetHNRec GetHNRec)
{
	if (pvar->flag & TEST_FLAG_GETHNREC)
	{
		if (GetHNRec)
		{
#ifndef TT4
			DBG_VIEW(_T("  GetHNRec"), _T("setupfn=%hs\r\n")
				_T("    setupfnw=%s\r\n")
				_T("    MaxComPort=%d ComPort=%d HostName=%s\r\n")
				_T("    ProtocolFamily=%d PortType=%d TcpPort=%d\r\n")
				_T("    Telnet=%d TelPort=%d"),
				GetHNRec->SetupFN, GetHNRec->SetupFNW, 
				GetHNRec->MaxComPort, GetHNRec->ComPort, GetHNRec->HostName, 
				GetHNRec->ProtocolFamily, GetHNRec->PortType, GetHNRec->TCPPort, 
				GetHNRec->Telnet, GetHNRec->TelPort);
#endif /* TT4 */
		}
		else
		{
			DBG_VIEW(_T("  GetHNRec"), _T("(NULL)"));
		}
	}
}
static BOOL PASCAL TTXGetHostName(HWND WndParent, PGetHNRec GetHNRec)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p GetHNRec=%p"), WndParent, GetHNRec);
	//TODO: convert format of GetHNRec

	//GetHNRec->HostName[0] = 0;
	dbg_gethnrec(GetHNRec);
	BOOL res = pvar->GetHostName(WndParent, GetHNRec);
	if (res)
		dbg_gethnrec(GetHNRec);
	return res;
}

static BOOL _TTXChangeDirectory(HWND WndParent, PTSTR CurDir)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p\r\n  CurDir=%s"), WndParent, CurDir);
	return TRUE;
}
#ifdef TT4
static BOOL PASCAL TTXChangeDirectory(HWND WndParent, PCHAR CurDir)
{
	_TTXChangeDirectory(WndParent, CurDir);
	return pvar->ChangeDirectory(WndParent, CurDir);
}
#else  /* TT4 */
static BOOL PASCAL TTXChangeDirectory(HWND WndParent, PTTSet ts)
{
	_TTXChangeDirectory(WndParent, ts->FileDirW);
	return pvar->ChangeDirectory(WndParent, ts);
}
#endif /* TT4 */

static BOOL PASCAL TTXAboutDialog(HWND WndParent)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p"), WndParent);
	return pvar->AboutDialog(WndParent);
}

static BOOL PASCAL TTXChooseFontDlg(HWND WndParent, LPLOGFONT LogFont, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p LogFont=%p ts=%p\r\n  (pvar->ts=%p)"), WndParent, LogFont, ts, pvar->ts);
	return pvar->ChooseFontDlg(WndParent, LogFont, ts);
}

static BOOL PASCAL TTXSetupGeneral(HWND WndParent, PTTSet ts)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p ts=%p\r\n  (pvar->ts=%p)"), WndParent, ts, pvar->ts);
	return pvar->SetupGeneral(WndParent, ts);
}

static BOOL PASCAL TTXWindowWindow(HWND WndParent, PBOOL Close)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p Close=%p"), WndParent, Close);
	return pvar->WindowWindow(WndParent, Close);
}

static void PASCAL TTXGetUIHooks(TTXUIHooks *hooks)
{
	HWND hWnd = GetActiveWindow();
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p hInst=%p"), hWnd, hInst);
	pvar->SetupTerminal = *hooks->SetupTerminal;
	pvar->SetupWin = *hooks->SetupWin;
	pvar->SetupKeyboard = *hooks->SetupKeyboard;
	pvar->SetupSerialPort = *hooks->SetupSerialPort;
	pvar->SetupTCPIP = *hooks->SetupTCPIP;
	pvar->GetHostName = *hooks->GetHostName;
	pvar->ChangeDirectory = *hooks->ChangeDirectory;
	pvar->AboutDialog = *hooks->AboutDialog;
	pvar->ChooseFontDlg = *hooks->ChooseFontDlg;
	pvar->SetupGeneral = *hooks->SetupGeneral;
	pvar->WindowWindow = *hooks->WindowWindow;

	*hooks->SetupTerminal = TTXSetupTerminal;
	*hooks->SetupWin = TTXSetupWin;
	*hooks->SetupKeyboard = TTXSetupKeyboard;
	*hooks->SetupSerialPort = TTXSetupSerialPort;
	*hooks->SetupTCPIP = TTXSetupTCPIP;
	*hooks->GetHostName = TTXGetHostName;
	*hooks->ChangeDirectory = TTXChangeDirectory;
	*hooks->AboutDialog = TTXAboutDialog;
	*hooks->ChooseFontDlg = TTXChooseFontDlg;
	*hooks->SetupGeneral = TTXSetupGeneral;
	*hooks->WindowWindow = TTXWindowWindow;
	{
		TCHAR title[64];
		_sntprintf_s(title, 64, 64, _T("build=%d.%d running=%d"), 
			TT_VERSION_MAJOR, TT_VERSION_MINOR, ttx_api_version);
		OpenInfoView(hInst, hWnd, title);
	}
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXReadIniFile(TT_LPCTSTR FName, PTTSet ts)
{
	BEGIN_TTX_STR(FName);
	DBG_VIEW(__FUNCTIONT__, _T("FName=%s\r\n  ts=%p (pvar->ts=%p)"), FNameW, ts, pvar->ts);
	END_TTX_STR(FName);
	pvar->ReadIniFile(FName, ts);
}

static void PASCAL TTXWriteIniFile(TT_LPCTSTR FName, PTTSet ts)
{
	BEGIN_TTX_STR(FName);
	DBG_VIEW(__FUNCTIONT__, _T("FName=%s\r\n  ts=%p (pvar->ts=%p)"), FNameW, ts, pvar->ts);
	END_TTX_STR(FName);
	pvar->WriteIniFile(FName, ts);
}

static void PASCAL TTXReadKeyboardCnf(TT_LPCTSTR FName, PKeyMap KeyMap, BOOL ShowWarning)
{
	BEGIN_TTX_STR(FName);
	DBG_VIEW(__FUNCTIONT__, _T("FName=%s\r\n  KeyMap=%p ShowWarning=%d"), FNameW, KeyMap, ShowWarning);
	END_TTX_STR(FName);
	pvar->ReadKeyboardCnf(FName, KeyMap, ShowWarning);
}

static void PASCAL TTXCopyHostList(TT_LPCTSTR IniSrc, TT_LPCTSTR IniDest)
{
	BEGIN_TTX_STR2(IniSrc, IniDest);
	DBG_VIEW(__FUNCTIONT__, _T("src=%s\r\n  dst=%s"), IniSrcW, IniDestW);
	END_TTX_STR2(IniSrc, IniDest);
	pvar->CopyHostList(IniSrc, IniDest);
}

static void PASCAL TTXAddHostToList(TT_LPCTSTR FName, TT_LPCTSTR Host)
{
	BEGIN_TTX_STR2(FName, Host);
	DBG_VIEW(__FUNCTIONT__, _T("FName=%s\r\n  Host=%s"), FNameW, HostW);
	END_TTX_STR2(FName, Host);
	pvar->AddHostToList(FName, Host);
}

static void PASCAL TTXParseParam(TT_LPTSTR Param, PTTSet ts, PCHAR DDETopic)
{
	BEGIN_TTX_STR(Param);
	DBG_VIEW(__FUNCTIONT__, _T("param=%s\r\n  ts=%p (pvar->ts=%p)"), ParamW, ts, pvar->ts);
	END_TTX_STR(Param);
	pvar->ParseParam(Param, ts, DDETopic);
}

static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks)
{
	//DBG_VIEW(__FUNCTIONT__, _T("hooks=%p"), hooks);
	pvar->AddHostToList = *hooks->AddHostToList;
	pvar->CopyHostList = *hooks->CopyHostList;
	pvar->ParseParam = *hooks->ParseParam;
	pvar->ReadIniFile = *hooks->ReadIniFile;
	pvar->ReadKeyboardCnf = *hooks->ReadKeyboardCnf;
	pvar->WriteIniFile = *hooks->WriteIniFile;

	*hooks->AddHostToList = TTXAddHostToList;
	*hooks->CopyHostList = TTXCopyHostList;
	*hooks->ParseParam = TTXParseParam;
	*hooks->ReadIniFile = TTXReadIniFile;
	*hooks->ReadKeyboardCnf = TTXReadKeyboardCnf;
	*hooks->WriteIniFile = TTXWriteIniFile;
}

///////////////////////////////////////////////////////////////

static int PASCAL TTXclosesocket(SOCKET s)
{
	return pvar->Pclosesocket(s);
}
static int PASCAL TTXconnect(SOCKET s, const struct sockaddr *name, int namelen)
{
	return pvar->Pconnect(s, name, namelen);
}
static u_long PASCAL TTXhtonl(u_long hostlong)
{
	return pvar->Phtonl(hostlong);
}
static u_short PASCAL TTXhtons(u_short hostshort)
{
	return pvar->Phtons(hostshort);
}
static unsigned long PASCAL TTXinet_addr(const char *cp)
{
	return pvar->Pinet_addr(cp);
}
static int PASCAL TTXioctlsocket(SOCKET s, long cmd, u_long *argp)
{
	return pvar->Pioctlsocket(s, cmd, argp);
}
static int PASCAL TTXrecv(SOCKET s, char *buf, int len, int flags)
{
	return pvar->Precv(s, buf, len, flags);
}
static int PASCAL TTXselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
							const struct timeval *timeout)
{
	return pvar->Pselect(nfds, readfds, writefds, exceptfds, timeout);
}
static int PASCAL TTXsend(SOCKET s, const char *buf, int len, int flags)
{
	return pvar->Psend(s, buf, len, flags);
}
static int PASCAL TTXsetsockopt(SOCKET s, int level, int optname, const char *optval, int optlen)
{
	return pvar->Psetsockopt(s, level, optname, optval, optlen);
}
static SOCKET PASCAL TTXsocket(int af, int type, int protocol)
{
	return pvar->Psocket(af, type, protocol);
}
static int PASCAL TTXWSAAsyncSelect(SOCKET s, HWND hWnd, u_int wMsg, long lEvent)
{
	return pvar->PWSAAsyncSelect(s, hWnd, wMsg, lEvent);
}
static HANDLE PASCAL TTXWSAAsyncGetHostByName(HWND hWnd, u_int wMsg, const char *name, char *buf, int buflen)
{
	return pvar->PWSAAsyncGetHostByName(hWnd, wMsg, name, buf, buflen);
}
static int PASCAL TTXWSACancelAsyncRequest(HANDLE hAsyncTaskHandle)
{
	return pvar->PWSACancelAsyncRequest(hAsyncTaskHandle);
}
static int PASCAL TTXWSAGetLastError(void)
{
	return pvar->PWSAGetLastError();
}
static HANDLE PASCAL TTXWSAAsyncGetAddrInfo(HWND hWnd, unsigned int wMsg, const char *hostname, const char *portname,
											struct addrinfo *hints, struct addrinfo **res)
{
	return pvar->PWSAAsyncGetAddrInfo(hWnd, wMsg, hostname, portname, hints, res);
}
static void PASCAL TTXfreeaddrinfo(struct addrinfo *ai)
{
	pvar->Pfreeaddrinfo(ai);
	return;
}

static void PASCAL TTXOpenTCP(TTXSockHooks *hooks)
{
	DBG_VIEW(__FUNCTIONT__, _T("hooks=%p"), hooks);
	pvar->Pclosesocket = *hooks->Pclosesocket;
	pvar->Pconnect = *hooks->Pconnect;
	pvar->Phtonl = *hooks->Phtonl;
	pvar->Phtons = *hooks->Phtons;
	pvar->Pinet_addr = *hooks->Pinet_addr;
	pvar->Pioctlsocket = *hooks->Pioctlsocket;
	pvar->Precv = *hooks->Precv;
	pvar->Pselect = *hooks->Pselect;
	pvar->Psend = *hooks->Psend;
	pvar->Psetsockopt = *hooks->Psetsockopt;
	pvar->Psocket = *hooks->Psocket;
	pvar->PWSAAsyncSelect = *hooks->PWSAAsyncSelect;
	pvar->PWSAAsyncGetHostByName = *hooks->PWSAAsyncGetHostByName;
	pvar->PWSACancelAsyncRequest = *hooks->PWSACancelAsyncRequest;
	pvar->PWSAGetLastError = *hooks->PWSAGetLastError;
	if (HAS_TTXHOOK(freeaddrinfo)) {
		pvar->Pfreeaddrinfo = *hooks->Pfreeaddrinfo;
	}
	if (HAS_TTXHOOK(WSAAsyncGetAddrInfo)) {
		pvar->PWSAAsyncGetAddrInfo = *hooks->PWSAAsyncGetAddrInfo;
	}

	*hooks->Pclosesocket = TTXclosesocket;
	*hooks->Pconnect = TTXconnect;
	*hooks->Phtonl = TTXhtonl;
	*hooks->Phtons = TTXhtons;
	*hooks->Pinet_addr = TTXinet_addr;
	*hooks->Pioctlsocket = TTXioctlsocket;
	*hooks->Precv = TTXrecv;
	*hooks->Pselect = TTXselect;
	*hooks->Psend = TTXsend;
	*hooks->Psetsockopt = TTXsetsockopt;
	*hooks->Psocket = TTXsocket;
	*hooks->PWSAAsyncSelect = TTXWSAAsyncSelect;
	*hooks->PWSAAsyncGetHostByName = TTXWSAAsyncGetHostByName;
	*hooks->PWSACancelAsyncRequest = TTXWSACancelAsyncRequest;
	*hooks->PWSAGetLastError = TTXWSAGetLastError;
	if (HAS_TTXHOOK(freeaddrinfo)) {
		*hooks->Pfreeaddrinfo = TTXfreeaddrinfo;
	}
	if (HAS_TTXHOOK(WSAAsyncGetAddrInfo)) {
		*hooks->PWSAAsyncGetAddrInfo = TTXWSAAsyncGetAddrInfo;
	}
}

static void PASCAL TTXCloseTCP(TTXSockHooks *hooks)
{
	DBG_VIEW(__FUNCTIONT__, _T("hooks=%p"), hooks);
	*hooks->Pclosesocket = pvar->Pclosesocket;
	*hooks->Pconnect = pvar->Pconnect;
	*hooks->Phtonl = pvar->Phtonl;
	*hooks->Phtons = pvar->Phtons;
	*hooks->Pinet_addr = pvar->Pinet_addr;
	*hooks->Pioctlsocket = pvar->Pioctlsocket;
	*hooks->Precv = pvar->Precv;
	*hooks->Pselect = pvar->Pselect;
	*hooks->Psend = pvar->Psend;
	*hooks->Psetsockopt = pvar->Psetsockopt;
	*hooks->Psocket = pvar->Psocket;
	*hooks->PWSAAsyncSelect = pvar->PWSAAsyncSelect;
	*hooks->PWSAAsyncGetHostByName = pvar->PWSAAsyncGetHostByName;
	*hooks->PWSACancelAsyncRequest = pvar->PWSACancelAsyncRequest;
	*hooks->PWSAGetLastError = pvar->PWSAGetLastError;
	if (HAS_TTXHOOK(freeaddrinfo)) {
		*hooks->Pfreeaddrinfo = pvar->Pfreeaddrinfo;
	}
	if (HAS_TTXHOOK(WSAAsyncGetAddrInfo)) {
		*hooks->PWSAAsyncGetAddrInfo = pvar->PWSAAsyncGetAddrInfo;
	}
}

VOID comm_setting(LPDCB pDCB){
	const char *parities[] = {"none", "odd", "even", "mark", "space"};
	const char *stop_bits[] = {"1", "1.5", "2"};
	DBG_VIEW(__FUNCTIONT__, 
		_T("BaudRate=%d BytesSize=%d StopBits=%hs Parity=%hs"), 
		pDCB->BaudRate, pDCB->ByteSize, stop_bits[pDCB->StopBits], parities[pDCB->Parity]);
}
VOID comm_rcvmode(LPDCB pDCB)
{
	DBG_VIEW(__FUNCTIONT__, 
		_T("fParity=%d fErrorChar=%d ErrorChar=%d fNull=%d EofChar=%d EvtChar=%d"),
		pDCB->fParity, pDCB->fErrorChar, pDCB->ErrorChar, 
		pDCB->fNull, pDCB->EofChar, pDCB->EvtChar);
}
VOID comm_cts_rts(LPDCB pDCB)
{
	const char *rts[] = {"DISABLE", "ENABLE", "HANDSHAKE", "TOGGLE"};
	DBG_VIEW(__FUNCTIONT__, 
		_T("fOutxCtsFlow=%d fRtsControl=%hs"),
		pDCB->fOutxCtsFlow, rts[pDCB->fRtsControl]);
}
VOID comm_dsr_dtr(LPDCB pDCB)
{
	const char *dtr[] = {"DISABLE", "ENABLE", "HANDSHAKE"};
	DBG_VIEW(__FUNCTIONT__, 
		_T("fOutxDsrFlow=%d fDsrSensitivity=%d fDtrControl=%hs"),
		pDCB->fOutxDsrFlow, pDCB->fDsrSensitivity, dtr[pDCB->fDtrControl]);
}
VOID comm_xonxoff(LPDCB pDCB)
{
	DBG_VIEW(__FUNCTIONT__, 
		_T("fOutX=%d fInX=%d XoffChar=%d, XonChar=%d"),
		pDCB->fOutX, pDCB->fInX, pDCB->XoffChar, pDCB->XonChar);
}
VOID comm_flowbuf(LPDCB pDCB)
{
	DBG_VIEW(__FUNCTIONT__, 
		_T("fTXContinueOnXoff=%d XoffLim=%d XonLim=%d"),
		pDCB->fTXContinueOnXoff, pDCB->XoffLim, pDCB->XonLim);
}
VOID comm_state(HANDLE FHandle)
{
	DCB dcb;
    //COMMTIMEOUTS timeouts;

    if (GetCommState(FHandle, &dcb)) {
		comm_setting(&dcb);		/* setting */
		comm_rcvmode(&dcb);		/* recive mode */
		comm_cts_rts(&dcb);		/* CTS/RTS */
		comm_dsr_dtr(&dcb);		/* DSR/DTR */
		comm_xonxoff(&dcb);		/* XON/XOFF */
		comm_flowbuf(&dcb);		/* flow buffer */
	}
}

VOID comm_modem(HANDLE FHandle)
{
	DWORD sts;
	GetCommModemStatus(FHandle, &sts);
	DBG_VIEW(__FUNCTIONT__, 
		_T("CTS=%d DSR=%d RING=%d RLSD=%d"),
		((sts&MS_CTS_ON)?1:0), 
		((sts&MS_DSR_ON)?1:0), 
		((sts&MS_RING_ON)?1:0), 
		((sts&MS_RLSD_ON)?1:0));
}

static BOOL PASCAL TTXTReadFile(HANDLE FHandle, LPVOID Buff, DWORD ReadSize, LPDWORD ReadBytes,
								LPOVERLAPPED ReadOverLap)
{
	//DBG_VIEW(__FUNCTIONT__, _T("FHandle=%p"), FHandle);
	if (pvar->flag & TEST_FLAG_COMM)
	{
		comm_state(FHandle);
		comm_modem(FHandle);
	}
	return pvar->PReadFile(FHandle, Buff, ReadSize, ReadBytes, ReadOverLap);
}

static BOOL PASCAL TTXTWriteFile(HANDLE FHandle, LPCVOID Buff, DWORD WriteSize, LPDWORD WriteBytes,
								 LPOVERLAPPED WriteOverLap)
{
	//DBG_VIEW(__FUNCTIONT__, _T("FHandle=%p"), FHandle);
	if (pvar->flag & TEST_FLAG_COMM)
	{
		comm_state(FHandle);
		comm_modem(FHandle);
	}
	return pvar->PWriteFile(FHandle, Buff, WriteSize, WriteBytes, WriteOverLap);
}

static HANDLE PASCAL TTXTCreateFile(LPCSTR FName, DWORD AcMode, DWORD ShMode, LPSECURITY_ATTRIBUTES SecAttr,
									DWORD CreateDisposition, DWORD FileAttr, HANDLE Template)
{
	DBG_VIEW(__FUNCTIONT__, _T("FName=%hs\r\n")
		_T("  AcMode=%08x ShMode=%08x SecAttr=%p CreateDisposition=%d\r\n")
		_T("  FileAttr=%08x Template=%p\r\n")
		_T("  (pvar->PCreateFile=%p [CreateFileA=%p CreateFileW=%p])"),
		FName,
		AcMode, ShMode, SecAttr, CreateDisposition, FileAttr, Template,
		pvar->PCreateFile, CreateFileA , CreateFileW);
	HANDLE FHandle = pvar->PCreateFile(FName, AcMode, ShMode, SecAttr, CreateDisposition, FileAttr, Template);
	comm_state(FHandle);
	comm_modem(FHandle);
	return FHandle;
}

static BOOL PASCAL TTXTCloseFile(HANDLE FHandle)
{
	DBG_VIEW(__FUNCTIONT__, _T("FHandle=%p\r\n  pvar->PCloseFile=%p CloseHandle=%p"),
		FHandle, pvar->PCloseFile, CloseHandle);
	comm_state(FHandle);
	comm_modem(FHandle);
	return pvar->PCloseFile(FHandle);
}

static void PASCAL TTXOpenFile(TTXFileHooks *hooks)
{
	if (HAS_TTXHOOK(TTXFileHooks)) {
		DBG_VIEW(__FUNCTIONT__, _T("hooks=%p\r\n")
			_T("  pvar->PCreateFile=%p hooks->PCreateFile=%p\r\n")
			_T("  pvar->PCloseFile=%p hooks->PCloseFile=%p\r\n")
			_T("  pvar->PReadFile=%p hooks->PReadFile=%p\r\n")
			_T("  pvar->PWriteFile=%p hooks->PWriteFile=%p"),
			hooks,
			pvar->PCreateFile, *hooks->PCreateFile,
			pvar->PCloseFile, *hooks->PCloseFile,
			pvar->PReadFile, *hooks->PReadFile,
			pvar->PWriteFile, *hooks->PWriteFile);

		pvar->PCreateFile = *hooks->PCreateFile;
		pvar->PCloseFile = *hooks->PCloseFile;
		pvar->PReadFile = *hooks->PReadFile;
		pvar->PWriteFile = *hooks->PWriteFile;

		*hooks->PCreateFile = TTXTCreateFile;
		*hooks->PCloseFile = TTXTCloseFile;
		*hooks->PReadFile = TTXTReadFile;
		*hooks->PWriteFile = TTXTWriteFile;
	}
	else {
		DBG_VIEW(__FUNCTIONT__, _T("hooks=%p"),	hooks);
	}
}

static void PASCAL TTXCloseFile(TTXFileHooks *hooks)
{
	DBG_VIEW(__FUNCTIONT__, _T("hooks=%p"), hooks);
	if (HAS_TTXHOOK(TTXFileHooks)) {
		if (pvar->PCreateFile) {
			*hooks->PCreateFile = pvar->PCreateFile;
			pvar->PCreateFile = NULL;
		}
		if (pvar->PCreateFile) {
			*hooks->PCloseFile = pvar->PCloseFile;
			pvar->PCloseFile = NULL;
		}
		if (pvar->PReadFile) {
			*hooks->PReadFile = pvar->PReadFile;
			pvar->PReadFile = NULL;
		}
		if (pvar->PWriteFile) {
			*hooks->PWriteFile = pvar->PWriteFile;
			pvar->PWriteFile = NULL;
		}
	}
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXSetWinSize(int rows, int cols)
{
	DBG_VIEW(__FUNCTIONT__, _T("r=%d, c=%d"), rows, cols);
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXModifyMenu(HMENU menu)
{
	int idx = GetMenuItemCount(menu) - 1;
	HMENU hmenu = GetSubMenu(menu, idx);
	InsertMenu(hmenu, 2, MF_BYPOSITION, ID_MENUITEM, _T("&Exports"));
	DBG_VIEW(__FUNCTIONT__, _T("menu=%p, hmenu=%p"), menu, hmenu);
}

static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
	DBG_VIEW(__FUNCTIONT__, _T("menu=%p"), menu);
}

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
	DBG_VIEW(__FUNCTIONT__, _T("hwnd=%p, cmd=%d"), hWin, cmd);
	if (cmd == ID_MENUITEM){
		if (IsOpenInfoView()){
			CloseInfoView();
		}
		else {
			OpenInfoView(hInst, hWin, NULL);
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////

static void PASCAL TTXEnd(void)
{
	DBG_VIEW(__FUNCTIONT__, _T("%s"), _T(""));
}

static void PASCAL TTXSetCommandLine(TT_LPTSTR cmd, int cmdlen, PGetHNRec rec)
{
	BEGIN_TTX_STR(cmd);
	DBG_VIEW(__FUNCTIONT__, _T("cmd=%s\r\n  len=%d GetHNRec=%p"), cmdW, cmdlen, rec);
	// TODO: convert format GetHNRec
	if (pvar->flag & TEST_FLAG_GETHNREC)
		dbg_gethnrec(rec);

	//_tcscpy_s(cmd, cmdlen, cmdW);
	END_TTX_STR(cmd);
}

///////////////////////////////////////////////////////////////

static TTXExports Exports = {
	/* This must contain the size of the structure. See below for its usage. */
	sizeof(TTXExports),

	ORDER,

	/* Now we just list the functions that we've implemented. */
	TTXInit,
	TTXGetUIHooks,
	TTXGetSetupHooks,
	TTXOpenTCP,
	TTXCloseTCP,
	TTXSetWinSize,
	TTXModifyMenu,
	TTXModifyPopupMenu,
	TTXProcessCommand,
	TTXEnd,
	TTXSetCommandLine,
	TTXOpenFile,
	TTXCloseFile,
};

BOOL __declspec(dllexport) PASCAL FAR TTXBind(WORD Version, TTXExports *exports)
{
	int size = sizeof(Exports) - sizeof(exports->size);
	/* do version checking if necessary */
	/* if (Version!=TTVERSION) return FALSE; */

	if (TTXIgnore(ORDER, _T(INISECTION), Version))
		return TRUE;

	//TTXInitVersion(Version);
#if 0
	{
		TCHAR buf[256];
		_sntprintf_s(buf, 256, 256,
					 _T("Build version=%d.%d\n")
					 _T("TTVERSION=%d\n")
					 _T("TTXBind().Version=%d\n")
					 _T("TTXCommon.ttx_api_version=%d\n"),
					 TT_VERSION_MAJOR, TT_VERSION_MINOR, TTVERSION, Version, ttx_api_version);
		MessageBox(0, buf, _T(INISECTION) _T(" ") __FUNCTIONT__ _T("()"), MB_OK | MB_ICONINFORMATION);
	}
#endif

	if (size > exports->size) {
		size = exports->size;
	}
	memcpy((char *)exports + sizeof(exports->size), (char *)&Exports + sizeof(exports->size), size);
	return TRUE;
}

BOOL WINAPI DllMain(HANDLE hInstance, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
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
