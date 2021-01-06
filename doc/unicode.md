# TTX I/F �ł̓��� Unicode ������

(Tera Term 4.105 2020/12/29 ����)

## Tera Term Extension interface �\���̒���

### �o�C�g��������܂ލ\����

* PTTSet ts
* TKeyMap (UserKeyStr �� Unicode ���͑Ó��H)

### �o�C�g������A����уo�C�g������|�C���^���܂ލ\����

* TComVar

### �o�C�g������|�C���^���܂ލ\����

* TGetHNRec

### �o�C�g������|�C���^���܂ރR�[���o�b�N

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

### _UNICODE �Ή��̕�����|�C���^���܂ރR�[���o�b�N

* TCreateFile * PCreateFile;

### TTXBind

�v���O�C�����őΉ�����ɂ� TTXBindProc �̃o�[�W�����Ŕ��ʁH

```c
typedef BOOL (PASCAL * TTXBindProc)(WORD Version, TTXExports * exports);
```

## �g�p�\����

(2020/12/29 ����)

tttset �\���̓ǂݍ��݂̂�
> HomeDir, LogFN, MacroFN, HostName, Title, PortType, TCPPort, ComPort, UILanguageFile, AutoComPortReconnect

tttset �\���̓ǂݏ���
> SetupFName, KeyCnfFN

TComVar �\���̓ǂݍ��݂̂�

> HWin

TKeyMap, TGetHNRec
> (���g�p)

## ��̓f�[�^

``` c
TTXExports
    TTXInit
        PTTSet ts
            ������܂�
        PComVar cv
            ������܂�
    TTXGetUIHooks
        TTXUIHooks * UIHooks
            PSetupTerminal * SetupTerminal;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    ������܂�
            PSetupWin * SetupWin;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    ������܂�
            PSetupKeyboard * SetupKeyboard;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    ������܂�
            PSetupSerialPort * SetupSerialPort;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    ������܂�
            PSetupTCPIP * SetupTCPIP;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    ������܂�
            PGetHostName * GetHostName;
                (BOOL)
                HWND WndParent
                PGetHNRec GetHNRec
                    PCHAR SetupFN;
                        ������
                    WORD PortType;
                    PCHAR HostName; 
                        ������
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
                    ������
            PAboutDialog * AboutDialog;
                (BOOL)
                HWND WndParent
            PChooseFontDlg * ChooseFontDlg;
                (BOOL)
                HWND WndParent
                LPLOGFONT LogFont
                PTTSet ts
                    ������܂�
            PSetupGeneral * SetupGeneral;
                (BOOL)
                HWND WndParent
                PTTSet ts
                    ������܂�
            PWindowWindow * WindowWindow;
                (BOOL)
                HWND WndParent
                PBOOL Close
    TTXGetSetupHooks
        TTXSetupHooks * setupHooks
            PReadIniFile * ReadIniFile;
                (void)
                PCHAR FName
                    ������
                PTTSet ts
                    ������܂�
            PWriteIniFile * WriteIniFile;
                (void)
                PCHAR FName
                    ������
                PTTSet ts
                    ������܂�
            PReadKeyboardCnf * ReadKeyboardCnf;
                (void)
                PCHAR FName
                    ������
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
                    ������
                PCHAR IniDest
                    ������
            PAddHostToList * AddHostToList;
                (void)
                PCHAR FName
                    ������
                PCHAR Host
                    ������
            PParseParam * ParseParam;
                (void)
                PCHAR Param
                    ������
                PTTSet ts
                    ������܂�
                PCHAR DDETopic
                    ������
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
                    ������
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
            ������
        int cmdlen
        PGetHNRec rec
            PCHAR SetupFN;
                ������
            WORD PortType;
            PCHAR HostName; 
                ������
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
                    UNICODE �Ή�
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
