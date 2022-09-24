/*
 * TTX common library
 * (C) 2021 tomo3136a
 */

#ifndef _TTXCOMMOM_H
#define _TTXCOMMOM_H

#include <windows.h>

#ifdef TT4
#define TT_TCHAR CHAR
#define TT_LPTSTR LPSTR
#define TT_LPCTSTR LPSTR
#else
#define TT_TCHAR wchar_t
#define TT_LPTSTR wchar_t *
#define TT_LPCTSTR const wchar_t *
#endif /* TT4 */

#define IS_TT4() (ttx_api_version<500)

#define BEGIN_TTX_STR(m) LPTSTR m##W = (IS_TT4()) ? toTC((PCHAR)m) : m;
#define END_TTX_STR(m) if (IS_TT4()) { TTXFree(&m##W); }
#define BEGIN_TTX_STR2(m1,m2) BEGIN_TTX_STR(m1) BEGIN_TTX_STR(m2)
#define END_TTX_STR2(m1,m2) END_TTX_STR(m1) END_TTX_STR(m2)

#ifdef __cplusplus
extern "C" {
#endif

/* ttx support */
/// TTX API version
///   0: unknown
///   23: Teraterm v2.3 or v4.x API
///   500: Teraterm5 API
extern WORD ttx_api_version;

/// TTX load test
BOOL TTXIgnore(int order, LPCTSTR name, WORD version);

// token command line parameter
LPTSTR TTXGetParam(LPTSTR buf, size_t sz, LPTSTR param);

/// get UI language ID(1=English, 2=Japanese)
/// TODO: numbering
UINT UILang(LPSTR lang);

/// get Menu ID offset
UINT TTXMenuID(UINT uid);
UINT TTXMenuOrgID(UINT uid);

/// memory allocate
LPVOID TTXAlloc(size_t sz);
BOOL TTXFree(LPVOID *pBuf);
BOOL TTXDup(LPTSTR *pszBuf, size_t sz, LPTSTR szSrc);

// path string
enum {
	ID_HOMEDIR = 1,
	ID_SETUPFNAME = 2,
	ID_KEYCNFNM = 3,
	ID_LOGFN = 4,
	ID_MACROFN = 5,
	ID_UILANGUAGEFILE = 6,
	ID_UILANGUAGEFILE_INI = 7,
	ID_EXEDIR = 8, /* support v5 */
	ID_LOGDIR = 9, /* support v5 */
	ID_FILEDIR = 10,
	ID_LOGDEFAULTPATH = 11,
	ID_STRMAX,
};
LPTSTR TTXGetPath(PTTSet ts, UINT uid);
BOOL TTXSetPath(PTTSet ts, UINT uid, LPTSTR s);

/* string convert */
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

///�A���������`
typedef LPTSTR strset_t;

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

#ifdef __cplusplus
}
#endif

#endif /* _TTXCOMMOM_H */
