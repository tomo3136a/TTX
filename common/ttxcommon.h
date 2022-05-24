/*
 * TTX common library
 * (C) 2021 tomo3136a
 */

#ifndef _TTXCOMMOM_H
#define _TTXCOMMOM_H

#ifdef TT4
#define TT_TCHAR CHAR
#define TT_LPTSTR LPSTR
#define TT_LPCTSTR LPSTR
#else
#define TT_TCHAR wchar_t
#define TT_LPTSTR wchar_t *
#define TT_LPCTSTR const wchar_t *
#endif /* TT4 */

#include <windows.h>

/* auto resize for dialog compornent by window resize */
#define RB_LEFT 0x0001
#define RB_TOP 0x0002
#define RB_RIGHT 0x0004
#define RB_BOTTOM 0x0008

#define TTXID "C11H17N3O8"

#ifdef __cplusplus
extern "C" {
#endif

/* ttx support */
/// TTX load test
BOOL TTXIgnore(int order, LPCTSTR name, WORD version);

// token command line parameter
LPTSTR TTXGetParam(LPTSTR buf, size_t sz, LPTSTR param);

// get UI language ID(1=English, 2=Japanese)
UINT UILang(LPSTR lang);

/// get Menu ID offset
UINT TTXMenuID(UINT uid);
UINT TTXMenuOrgID(UINT uid);

LPTSTR TTXGetModuleFileName(HMODULE hModule);

// path string
enum {
	ID_HOMEDIR = 1,
	ID_SETUPFNAME = 2,
	ID_KEYCNFNM = 3,
	ID_LOGFN = 4,
	ID_MACROFN = 5,
	ID_UILANGUAGEFILE = 6,
	ID_UILANGUAGEFILE_INI = 7,
	ID_LOGDEFAULTPATH = 8,
	ID_EXEDIR = 9, /* support v5 */
	ID_LOGDIR = 10, /* support v5 */
	ID_FILEDIR = 11,
	ID_STRMAX,
};
LPTSTR TTXGetPath(PTTSet ts, UINT uid);
BOOL TTXSetPath(PTTSet ts, UINT uid, LPTSTR s);

/* string */
LPSTR WC2MB(UINT cp, LPWSTR pwzSrc);
inline LPSTR WC2ACP(LPWSTR pwzSrc)
{
	return WC2MB(CP_ACP, pwzSrc);
}
inline LPSTR WC2UTF8(LPWSTR pwzSrc)
{
	return WC2MB(CP_UTF8, pwzSrc);
}

LPWSTR MB2WC(UINT cp, LPSTR pszSrc);
inline LPWSTR ACP2WC(LPSTR pszSrc)
{
	return MB2WC(CP_ACP, pszSrc);
}
inline LPWSTR UTF82WC(LPSTR pszSrc)
{
	return MB2WC(CP_UTF8, pszSrc);
}

#ifdef TT4
inline LPSTR toMB(LPTSTR pszSrc)
{
	return _tcsdup(pszSrc);
}
inline LPTSTR toTC(LPSTR pszSrc)
{
	return _tcsdup(pszSrc);
}
#else
inline LPSTR toMB(LPTSTR pszSrc)
{
	return WC2MB(CP_ACP, pszSrc);
}
inline LPTSTR toTC(LPSTR pszSrc)
{
	return MB2WC(CP_ACP, pszSrc);
}
#endif /* TT4 */

BOOL TTXDup(LPTSTR *pszBuf, size_t sz, LPTSTR szSrc);
BOOL TTXFree(LPVOID *pBuf);

///�����񒆂ɕ��������������̃|�C���^��Ԃ�
LPTSTR strskip(LPTSTR p, TCHAR c);

/* string set */
///�A���������`
typedef LPTSTR strset_t;

///�A�������񂩂珇���؂�o��
LPTSTR StrSetTok(strset_t p, strset_t *ctx);

///�A��������̃T�C�Y���擾����
int StrSetSize(strset_t p, int *cnt);

///�A�������񂩂�L�[���[�h�̘A��������쐬
int StrSetKeys(strset_t dst, strset_t src);

///�A�������񂩂�L�[���[�h�̃C���f�b�N�X�擾
int StrSetFindIndex(strset_t p, LPTSTR k);

///�A�������񂩂�L�[���[�h�Ō�����������擾
LPTSTR StrSetFindKey(strset_t p, LPTSTR k);

///�A�������񂩂�l�Ō�����������擾
LPTSTR StrSetFindVal(strset_t p, LPTSTR v);

///�A�������񂩂�n�Ԗڂ̕�������擾����
LPTSTR StrSetAt(strset_t p, int n);

/* path */
// fileapi.h �͎g��Ȃ��悤�Ȃ̂ő�ւ������A�݊����͂Ȃ�

/* find �^ */
/// find file name address
LPTSTR FindFileName(LPCTSTR path);

/// find file extension address
LPTSTR FindFileExt(LPCTSTR path);

/// find path component path address
LPTSTR FindPathNextComponent(LPCTSTR path);

/* build �^(src to dst) */
/// get parent path
LPTSTR GetParentPath(LPTSTR dst, size_t dst_sz, LPCTSTR src);

/// get path item name
LPTSTR GetPathName(LPTSTR dst, size_t dst_sz, LPCTSTR src);

/// get linearized path
LPTSTR GetLinearizedPath(LPTSTR dst, size_t dst_sz, LPCTSTR src);

/// get absolute path
LPTSTR GetAbsolutePath(LPTSTR dst, size_t dst_sz, LPCTSTR src, LPCTSTR base);

/// get related path
LPTSTR GetRelatedPath(LPTSTR dst, size_t dst_sz, LPCTSTR src, LPCTSTR base, int lv);

/// get contract path
LPTSTR GetContractPath(LPTSTR dst, size_t dst_sz, LPTSTR src);

/* replase �^ */
/// remove last slash from path
LPTSTR RemovePathSlash(LPTSTR path);

/// remove last slash from path
LPTSTR RemoveFileName(LPTSTR path);

/// remove last slash from path
LPTSTR RemoveFileExt(LPTSTR path);

/// combine path
LPTSTR CombinePath(LPTSTR path, size_t sz, LPCTSTR fn);

/* test �^ */
/// test exist file
BOOL FileExists(LPCTSTR path);

/* setting file */
///�Z�N�V�������̘A��������擾(�J����free(outp))
DWORD GetIniSects(strset_t *outp, DWORD sz, DWORD nsz, LPCTSTR fn);

///�Z�N�V�������̃L�[���[�h��/�l�̘A��������擾(�J����free(outp))
DWORD GetIniStrSet(LPCTSTR sect, strset_t *outp, DWORD sz, DWORD nsz, LPCTSTR fn);

///�Z�N�V�������̃L�[���[�h���̘A��������擾(�J����free(outp))
DWORD GetIniKeys(LPCTSTR sect, strset_t *outp, DWORD sz, DWORD nsz, LPCTSTR fn);

/// ON/OFF �ݒ��ݒ�t�@�C������擾
BOOL GetIniOnOff(LPCTSTR sect, LPCTSTR name, BOOL bDefault, LPCTSTR fn);

///���l�ݒ��ݒ�t�@�C������擾
UINT GetIniNum(LPCTSTR sect, LPCTSTR name, int nDefault, LPCTSTR fn);

///�������ݒ�t�@�C������擾(�J����free(outp))
DWORD GetIniString(LPCTSTR sect, LPCTSTR name, LPCTSTR sDefault, LPTSTR *outp, DWORD sz, DWORD nsz, LPCTSTR fn);
LPSTR GetIniStringA(LPCTSTR sect, LPCTSTR name, LPCTSTR sDefault, LPCTSTR fn);

/// OM/OFF �ݒ��ݒ�t�@�C���ɏ�������
BOOL WriteIniOnOff(LPCTSTR sect, LPCTSTR name, int bFlag, BOOL bEnable, LPCTSTR fn);

///���l�ݒ��ݒ�t�@�C���ɏ�������
BOOL WriteIniNum(LPCTSTR sect, LPCTSTR name, int val, BOOL bEnable, LPCTSTR fn);

/* window control */
/// get right-bottom point from window item
VOID GetPointRB(HWND hWnd, UINT uItem, POINT *pt);

/// move right-bottom point within window item
VOID MovePointRB(HWND hWnd, UINT uItem, POINT *ptRB, UINT uFlag);

/// get window size to point structure
VOID GetWindowSize(HWND hWnd, POINT *pt);

/// set window size from point structure
VOID SetWindowSize(HWND hWnd, POINT *pt);

/// move window relative direction
VOID SetHomePosition(HWND hWnd, HWND hWndBase, UINT uPos);

/// adjust window position to center of parent window
VOID MoveParentCenter(HWND hWnd);

/// create dialog font and set to phFont (require to delete item after)
VOID SetDlgFont(HWND hWnd, UINT uItem, HFONT *phFont, LONG uH, LPTSTR szFont);

/* dialog */
enum {
	PTF_CONTRACT = 16,
	PTF_SETTPATH = 32,
	PTF_GETPATH = 64
};
/// open to file select dialog
BOOL OpenFileDlg(HWND hWnd, UINT editCtl, LPTSTR szTitle, LPTSTR szFilter, LPTSTR szPath, UINT uFlag);

/// open to folder select dialog
BOOL OpenFolderDlg(HWND hWnd, UINT editCtl, LPTSTR szTitle, LPTSTR szPath, UINT uFlag);

#ifdef __cplusplus
}
#endif

#endif /* _TTXCOMMOM_H */
