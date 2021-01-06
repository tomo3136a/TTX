# TTX I/F での内部 Unicode 化検討

(Tera Term 4.105 2020/12/29 調べ)

## Tera Term Extension interface 構造体調査

### バイト文字列を含む構造体

* PTTSet ts
* TKeyMap (UserKeyStr の Unicode 化は妥当？)

### バイト文字列、およびバイト文字列ポインタを含む構造体

* TComVar

### バイト文字列ポインタを含む構造体

* TGetHNRec

### バイト文字列ポインタを含むコールバック

* PChangeDirectory * ChangeDirectory;
* PReadIniFile * ReadIniFile;
* PWriteIniFile * WriteIniFile;
* PReadKeyboardCnf * ReadKeyboardCnf;
* PCopyHostList * CopyHostList;
* PAddHostToList * AddHostToList;
* PParseParam * ParseParam;
* Tinet_addr * Pinet_addr;
* TWSAAsyncGetHostByName * PWSAAsyncGetHostByName;
* TWSAAsyncGetAddrInfo * PWSAAsyncGetAddrInfo;
* TTXSetCommandLine

### _UNICODE 対応の文字列ポインタを含むコールバック

* TCreateFile * PCreateFile;

### TTXBind

プラグイン側で対応するには TTXBindProc のバージョンで判別？

```c
typedef BOOL (PASCAL * TTXBindProc)(WORD Version, TTXExports * exports);
```

## 使用構造体

(2020/12/29 調べ)

tttset 構造体読み込みのみ
> HomeDir, LogFN, MacroFN, HostName, Title, PortType, TCPPort, ComPort, UILanguageFile, AutoComPortReconnect

tttset 構造体読み書き
> SetupFName, KeyCnfFN

TComVar 構造体読み込みのみ

> HWin

TKeyMap, TGetHNRec
> (未使用)

## 解析データ

``` c
TTXExports
    TTXInit
        PTTSet ts
            文字列含む
        PComVar cv
            文字列含む
    TTXGetUIHooks
        TTXUIHooks * UIHooks
            PSetupTerminal * SetupTerminal;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    文字列含む
            PSetupWin * SetupWin;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    文字列含む
            PSetupKeyboard * SetupKeyboard;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    文字列含む
            PSetupSerialPort * SetupSerialPort;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    文字列含む
            PSetupTCPIP * SetupTCPIP;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    文字列含む
            PGetHostName * GetHostName;
                (BOOL)
                HWND WndParent
                PGetHNRec GetHNRec
                    PCHAR SetupFN;
                        文字列
                    WORD PortType;
                    PCHAR HostName; 
                        文字列
                    WORD Telnet;
                    WORD TelPort;
                   WORD TCPPort;
                    WORD ProtocolFamily;
                    WORD ComPort;
                    WORD MaxComPort;
            PChangeDirectory * ChangeDirectory;
                (BOOL)
                HWND WndParent
                PCHAR CurDir
                    文字列
            PAboutDialog * AboutDialog;
                (BOOL)
                HWND WndParent
            PChooseFontDlg * ChooseFontDlg;
                (BOOL)
                HWND WndParent
                LPLOGFONT LogFont
                PTTSet ts
                    文字列含む
            PSetupGeneral * SetupGeneral;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    文字列含む
            PWindowWindow * WindowWindow;
                (BOOL)
                HWND WndParent
                PBOOL Close
    TTXGetSetupHooks
        TTXSetupHooks * setupHooks
            PReadIniFile * ReadIniFile;
                (void)
                PCHAR FName
                    文字列
                PTTSet ts
                    文字列含む
            PWriteIniFile * WriteIniFile;
                (void)
                PCHAR FName
                    文字列
                PTTSet ts
                    文字列含む
            PReadKeyboardCnf * ReadKeyboardCnf;
                (void)
                PCHAR FName
                    文字列
                PKeyMap KeyMap
                    WORD Map[IdKeyMax];
                    int UserKeyPtr[NumOfUserKey], 
                        UserKeyLen[NumOfUserKey];
                    BYTE UserKeyStr[KeyStrMax+1];
                        T.B.D.
                    BYTE UserKeyType[NumOfUserKey];
                BOOL ShowWarning
            PCopyHostList * CopyHostList;
                (void)
                PCHAR IniSrc
                    文字列
                PCHAR IniDest
                    文字列
            PAddHostToList * AddHostToList;
                (void)
                PCHAR FName
                    文字列
                PCHAR Host
                    文字列
            PParseParam * ParseParam;
                (void)
                PCHAR Param
                    文字列
                PTTSet ts
                    文字列含む
                PCHAR DDETopic
                    文字列
    TTXOpenTCP
        TTXSockHooks * hooks
            Tclosesocket * Pclosesocket;
                (int)
                SOCKET s
            Tconnect * Pconnect;
                (int)
                SOCKET s
                const struct sockaddr *name
                int namelen
            Thtonl * Phtonl;
                (u_long)
                u_long hostlong
            Thtons * Phtons;
                (u_long)
                u_short hostshort
            Tinet_addr * Pinet_addr;
                (unsigned long)
                const char * cp
                    文字列
            Tioctlsocket * Pioctlsocket;
                (int)
                SOCKET s
                long cmd
                u_long *argp
            Trecv * Precv;
                (int)
                SOCKET s
                char * buf
                int len
                int flags
            Tselect * Pselect;
                (int)
                int nfds
                fd_set *readfds
                fd_set *writefds
                fd_set *exceptfds
                const struct timeval *timeout
            Tsend * Psend;
                (int)
                SOCKET s
                const char * buf
                int len
                int flags
            Tsetsockopt * Psetsockopt;
                (int)
                SOCKET s
                int level
                int optname
                const char * optval
                int optlen
            Tsocket * Psocket;
                (SOCKET)
                int af
                int type
                int protocol
            TWSAAsyncSelect * PWSAAsyncSelect;
                (int)
                SOCKET s
                HWND hWnd
                u_int wMsg
                long lEvent
            TWSAAsyncGetHostByName * PWSAAsyncGetHostByName;
                (HANDLE)
                HWND hWnd
                u_int wMsg
                const char * name
                char * buf
                int buflen
            TWSACancelAsyncRequest * PWSACancelAsyncRequest;
                (int)
                HANDLE hAsyncTaskHandle
            TWSAGetLastError * PWSAGetLastError;
                (int)
              Tgetaddrinfo * Pgetaddrinfo;
            Tfreeaddrinfo * Pfreeaddrinfo;
                (void)
                struct addrinfo *ai
            TWSAAsyncGetAddrInfo * PWSAAsyncGetAddrInfo;
                (HANDLE)
                HWND hWnd
                unsigned int wMsg
                const char * hostname
                const char * portname
                struct addrinfo * hints
                struct addrinfo * * res
    TTXCloseTCP
        TTXSockHooks * hooks
    TTXSetWinSize
        int rows
        int cols
    TTXModifyMenu
        HMENU menu
    TTXModifyPopupMenu
        HMENU menu
    TTXProcessCommand
        HWND hWin
        WORD cmd
    TTXEnd
    TTXSetCommandLine
        PCHAR cmd
            文字列
        int cmdlen
        PGetHNRec rec
            PCHAR SetupFN;
                文字列
            WORD PortType;
            PCHAR HostName; 
                文字列
            WORD Telnet;
            WORD TelPort;
            WORD TCPPort;
            WORD ProtocolFamily;
            WORD ComPort;
            WORD MaxComPort;
    TTXOpenFile
        TTXFileHooks * hooks
            TCreateFile * PCreateFile;
                (HANDLE)
                LPCTSTR FName
                    UNICODE 対応
                DWORD AcMode
                DWORD ShMode
                LPSECURITY_ATTRIBUTES SecAttr
                DWORD CreateDisposition
                DWORD FileAttr
                HANDLE Template
            TCloseFile * PCloseFile;
                (BOOL)
                HANDLE FHandle
            TReadFile * PReadFile;
                (BOOL)
                HANDLE FHandle
                LPVOID Buff
                DWORD ReadSize
                LPDWORD ReadBytes
                LPOVERLAPPED ReadOverLap
            TWriteFile * PWriteFile;
                (BOOL)
                HANDLE FHandle
                LPCVOID Buff
                DWORD WriteSize
                LPDWORD WriteBytes
                LPOVERLAPPED WriteOverLap
    TTXCloseFile
        TTXFileHooks * hooks
```
