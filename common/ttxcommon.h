/*
 * TTX common library
 * (C) 2021 tomo3136a
 */

#ifdef _COMPAT95
#include "compat_w95.h"
#define TTX_DLL_PROCESS_ATTACH() DoCover_IsDebuggerPresent()
#else
#define TTX_DLL_PROCESS_ATTACH()
#endif /* _COMPAT95 */

#ifdef _UNICODE
#define TT_TCHAR wchar_t
#define TT_LPTSTR wchar_t *
#define TT_LPTCSTR const wchar_t *
#else
#define TT_TCHAR CHAR
#define TT_LPTSTR LPSTR
#define TT_LPTCSTR LPCSTR
#endif /* _UNICODE */

//#define TT_TCHAR TCHAR
//#define TT_LPTSTR LPTSTR
//#define TT_LPTCSTR LPTCSTR

#include <windows.h>

/* auto resize for dialog compornent by window resize */
#define RB_LEFT 0x0001
#define RB_TOP 0x0002
#define RB_RIGHT 0x0004
#define RB_BOTTOM 0x0008

#define TTXID "C11H17N3O8"

#ifdef __cplusplus
extern "C"
{
#endif

    /* ttx support */
    ///TTX load test
    BOOL TTXIgnore(int order, PTCHAR name, WORD version);

    ///token command line parameter
    PTCHAR TTXGetParam(PTCHAR buf, size_t sz, PTCHAR param);

    ///get UI language ID(1=English, 2=Japanese)
    UINT UILang(PCHAR lang);

    ///get Menu ID offset
    int TTXMenuID(UINT uid);
    int TTXMenuOrgID(UINT uid);

    ///get environment string
    enum {
        ID_HOMEDIR              = 1,
        ID_SETUPFNAME           = 2,
        ID_KEYCNFNM             = 3,
        ID_LOGFN                = 4,
        ID_MACROFN              = 5,
        ID_UILANGUAGEFILE       = 6,
        ID_UILANGUAGEFILE_INI   = 7,
        ID_EXEDIR               = 8,
        ID_LOGDIR               = 9,
        ID_STRMAX,
    };
    //size_t GetEnvPath(PTTSet ts, UINT uid, TCHAR *buf, DWORD dwsz);
    LPTSTR TTXGetPath(PTTSet ts, UINT uid);

    /* string */
    LPSTR WC2MB(UINT cp, LPWSTR pwzSrc);
    inline LPSTR WC2ACP(LPWSTR pwzSrc){ return WC2MB(CP_ACP, pwzSrc); }
    inline LPSTR WC2UTF8(LPWSTR pwzSrc){ return WC2MB(CP_UTF8, pwzSrc); }

    LPWSTR MB2WC(UINT cp, LPSTR pszSrc);
    inline LPWSTR ACP2WC(LPSTR pszSrc){ return MB2WC(CP_ACP, pszSrc); }
    inline LPWSTR UTF82WC(LPSTR pszSrc){ return MB2WC(CP_UTF8, pszSrc); }

#ifdef _UNICODE
    inline LPSTR toMB(LPTSTR pszSrc){ return WC2MB(CP_ACP, pszSrc); }
    inline LPTSTR toTC(LPSTR pszSrc){ return MB2WC(CP_ACP, pszSrc); }
#else
    inline LPSTR toMB(LPCTSTR pszSrc){ return _tcsdup(pszSrc); }
    inline LPTSTR toTC(LPCSTR pszSrc){ return _tcsdup(pszSrc); }
#endif /* _UNICODE */

    BOOL TTXFree(LPVOID pBuf);

    ///�����񒆂ɕ��������������̃|�C���^��Ԃ�
    PTCHAR strskip(PTCHAR p, TCHAR c);

    /* string set */
    ///�A���������`
    typedef PTCHAR strset_t;

    ///�A�������񂩂珇���؂�o��
    PTCHAR StrSetTok(strset_t p, strset_t *ctx);

    ///�A��������̃T�C�Y���擾����
    int StrSetSize(strset_t p, int *cnt);

    ///�A�������񂩂�L�[���[�h�̘A��������쐬
    int StrSetKeys(strset_t dst, strset_t src);

    ///�A�������񂩂�L�[���[�h�̃C���f�b�N�X�擾
    int StrSetFindIndex(strset_t p, PTCHAR k);

    ///�A�������񂩂�L�[���[�h�Ō�����������擾
    PTCHAR StrSetFindKey(strset_t p, PTCHAR k);

    ///�A�������񂩂�l�Ō�����������擾
    PTCHAR StrSetFindVal(strset_t p, PTCHAR v);

    ///�A�������񂩂�n�Ԗڂ̕�������擾����
    PTCHAR StrSetAt(strset_t p, int n);

    /* path */
    // fileapi.h �͎g��Ȃ��悤�Ȃ̂ő�ւ������A�݊����͂Ȃ�

    /* find �^ */
    ///find file name address
    PTCHAR FindFileName(PTCHAR path);

    ///find file extension address
    PTCHAR FindFileExt(PTCHAR path);

    ///find path component path address
    PTCHAR FindPathNextComponent(PTCHAR path);

    /* build �^(src to dst) */
    ///get parent path
    PTCHAR GetParentPath(PTCHAR dst, int sz, PTCHAR src);

    ///get path item name
    PTCHAR GetPathName(PTCHAR dst, int sz, PTCHAR src);

    ///get linearized path
    PTCHAR GetLinearizedPath(PTCHAR dst, int sz, PTCHAR src);

    ///get absolute path
    PTCHAR GetAbsolutePath(PTCHAR dst, int sz, PTCHAR src, PTCHAR base);

    ///get related path
    PTCHAR GetRelatedPath(PTCHAR dst, int sz, PTCHAR src, PTCHAR base, int lv);

    /* replase �^ */
    ///remove last slash from path
    PTCHAR RemovePathSlash(PTCHAR path);

    ///remove last slash from path
    PTCHAR RemoveFileName(PTCHAR path);

    ///remove last slash from path
    PTCHAR RemoveFileExt(PTCHAR path);

    ///combine path
    PTCHAR CombinePath(PTCHAR path, int sz, PTCHAR fn);

    /* test �^ */
    ///test exist file
    BOOL FileExists(PTCHAR path);

    /* setting file */
    ///�Z�N�V�������̘A��������擾(�J����free(outp))
    DWORD GetIniSects(strset_t *outp, DWORD sz, DWORD nsz, LPCTSTR fn);

    ///�Z�N�V�������̃L�[���[�h��/�l�̘A��������擾(�J����free(outp))
    DWORD GetIniStrSet(LPCTSTR sect, strset_t *outp, DWORD sz, DWORD nsz, LPCTSTR fn);

    ///�Z�N�V�������̃L�[���[�h���̘A��������擾(�J����free(outp))
    DWORD GetIniKeys(LPCTSTR sect, strset_t *outp, DWORD sz, DWORD nsz, LPCTSTR fn);

    ///ON/OFF �ݒ��ݒ�t�@�C������擾
    BOOL GetIniOnOff(LPCTSTR sect, LPCTSTR name, BOOL bDefault, LPCTSTR fn);

    ///���l�ݒ��ݒ�t�@�C������擾
    UINT GetIniNum(LPCTSTR sect, LPCTSTR name, int nDefault, LPCTSTR fn);

    ///�������ݒ�t�@�C������擾(�J����free(outp))
    DWORD GetIniString(LPCTSTR sect, LPCTSTR name, LPCTSTR sDefault,
                       PTCHAR *outp, DWORD sz, DWORD nsz, LPCTSTR fn);
    LPSTR GetIniStringA(LPCTSTR sect, LPCTSTR name, LPCTSTR sDefault, LPCTSTR fn);

    ///OM/OFF �ݒ��ݒ�t�@�C���ɏ�������
    BOOL WriteIniOnOff(LPCTSTR sect, LPCTSTR name, int bFlag, BOOL bEnable, LPCTSTR fn);

    ///���l�ݒ��ݒ�t�@�C���ɏ�������
    BOOL WriteIniNum(LPCTSTR sect, LPCTSTR name, int val, BOOL bEnable, LPCTSTR fn);

    /* window control */
    ///get right-bottom point from window item
    VOID GetPointRB(HWND hWnd, UINT uItem, POINT *pt);

    ///move right-bottom point within window item
    VOID MovePointRB(HWND hWnd, UINT uItem, POINT *ptRB, UINT uFlag);

    ///get window size to point structure
    VOID GetWindowSize(HWND hWnd, POINT *pt);

    ///set window size from point structure
    VOID SetWindowSize(HWND hWnd, POINT *pt);

    ///move window relative direction
    VOID SetHomePosition(HWND hWnd, HWND hWndBase, UINT uPos);

    ///adjust window position to center of parent window
    VOID MoveParentCenter(HWND hWnd);

    ///create dialog font and set to phFont (require to delete item after)
    VOID SetDlgFont(HWND hWnd, UINT uItem, HFONT *phFont, LONG uH, PTCHAR szFont);

    /* dialog */
    ///open to file select dialog
    BOOL OpenFileDlg(HWND hWnd, UINT editCtl, PTCHAR szTitle,
                     PTCHAR szFilter, PTCHAR szPath, PTCHAR fn, int n);

    ///open to folder select dialog
    BOOL OpenFolderDlg(HWND hWnd, UINT editCtl, PTCHAR szTitle, PTCHAR szPath);

#ifdef __cplusplus
}
#endif
