/*
 * TTX common library
 * (C) 2021 tomo3136a
 */

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <shlobj.h>
#include <time.h>
#include <tchar.h>

#include "ttxcommon.h"

#define TTX_SECTION "TTXPlugin"
#define INI_FILE ".\\TERATERM.INI"

///////////////////////////////////////////////////////////////////////////

static int _menu_offset = 0;
WORD tt_version = 0;
extern BOOL TTXInitVersion(WORD version);

BOOL TTXIgnore(int order, LPCTSTR name, WORD version)
{
	// TODO: test order, test version
	TCHAR buf[32];
	LPTSTR p;

	GetPrivateProfileString(_T(TTX_SECTION), name, _T("off"), 
		buf, sizeof(buf) / sizeof(buf[0]), _T(INI_FILE));
	if (buf[0] == 0)
		return TRUE;
	if (_tcsnicmp(_T("off"), buf, 3) == 0)
		return TRUE;
	p = _tcschr(buf, _T(','));
	_menu_offset = (NULL == p) ? 0 : _ttoi(1 + p);
	return !TTXInitVersion(version);
}

// get offset based Menu ID
UINT TTXMenuID(UINT uid)
{
	return (UINT)(uid + _menu_offset);
}
// get original based Menu ID
UINT TTXMenuOrgID(UINT uid)
{
	return (UINT)(uid - _menu_offset);
}

//コマンドラインパラメータ解析
// param を解析し、サイズ sz の buf に複製
//「 」はパラメータ区切り文字
//「;」以降はコメント扱い
//「"」から「"」までは、文字列として扱う
//戻り値は、成功した場合 NULL 以外になる
// buf は NULL 禁止
// MBS(ShiftJIS)は対応しない(「@[\]^_`{|}~」は区切り文字に使用する設計しない)

LPTSTR TTXGetParam(LPTSTR buf, size_t sz, LPTSTR param)
{
	size_t i = 0;
	BOOL quoted = FALSE;

	if (param == NULL) {
		return NULL;
	}

	buf[0] = _T('\0');

	//スペーススキップ・終了判定
	while (*param == _T(' ')) {
		param++;
	}
	if (*param == _T('\0') || *param == _T(';')) {
		return NULL;
	}

	while (*param != _T('\0') && (quoted || *param != _T(';')) && (quoted || *param != _T(' '))) {
		if (*param == _T('"')) {
			if (*(param + 1) != _T('"')) {
				quoted = !quoted;
				param++;
				continue;
			}
		}
		if (i + 1 < sz) {
			buf[i++] = *param;
		}
		param++;
	}

	//文字列の最後は必ずNUL文字
	buf[i] = _T('\0');
	return param;
}

// UI言語指定
UINT UILang(LPSTR lang)
{
	lang += 4;
	return strstr(lang, "Eng")	 ? 1
		   : strstr(lang, "Jap") ? 2
		   : strstr(lang, "Rus") ? 3
		   : strstr(lang, "Kor") ? 4
		   : strstr(lang, "UTF") ? 5
								 : 1;
}

LPTSTR TTXGetModuleFileName(HMODULE hModule)
{
	DWORD buf_sz;
	LPTSTR buf;

	buf_sz = MAX_PATH + 1;
	buf = (LPTSTR)malloc(buf_sz*sizeof(TCHAR));
	while (buf && (GetModuleFileName(hModule, buf, buf_sz) >= buf_sz))
	{
		free(buf);
		buf_sz += buf_sz;
		buf = (LPTSTR)malloc(buf_sz*sizeof(TCHAR));
	}
	return buf;
}

static LPSTR TTXGetPath_v4(PTTSet ts, UINT uid)
{
	LPSTR s = NULL;
	//not use wide character
	switch(uid)
	{
		case ID_HOMEDIR:
			s = ts->HomeDir;
			break;
        case ID_SETUPFNAME:
			s = ts->SetupFName;
			break;
        case ID_KEYCNFNM:
			s = ts->KeyCnfFN;
			break;
        case ID_LOGFN:
			s = ts->LogFN;
			break;
        case ID_MACROFN:
			s = ts->MacroFN;
			break;
        case ID_UILANGUAGEFILE:
			s = ts->UILanguageFile;
			break;
        case ID_UILANGUAGEFILE_INI:
			s = ts->UILanguageFile_ini;
			break;
        case ID_EXEDIR:
			s = NULL; //RemoveFileName(TTXGetModuleFileName(0));
			break;
        case ID_LOGDIR:
			s = ts->HomeDir;
			break;
        case ID_FILEDIR:
			s = ts->FileDir;
			break;
        case ID_LOGDEFAULTPATH:
			s = NULL;
			break;
		default:
			s = NULL;
			break;
	}
	return s;
}

#ifndef TT4
static LPTSTR TTXGetPath_v5(PTTSet ts, UINT uid)
{
	LPTSTR s = NULL;
	switch(uid)
	{
		case ID_HOMEDIR:
			s = ts->HomeDirW;
			break;
        case ID_SETUPFNAME:
			s = ts->SetupFNameW;
			break;
        case ID_KEYCNFNM:
			s = ts->KeyCnfFNW;
			break;
        case ID_LOGFN:
			s = ts->LogFNW;
			break;
        case ID_MACROFN:
			s = ts->MacroFNW;
			break;
        case ID_UILANGUAGEFILE:
			s = ts->UILanguageFileW;
			break;
        case ID_UILANGUAGEFILE_INI:
			s = ts->UILanguageFileW_ini;
			break;
        case ID_EXEDIR:
			s = ts->ExeDirW;
			break;
        case ID_LOGDIR:
			s = ts->LogDirW;
			break;
        case ID_FILEDIR:
			s = ts->FileDirW;
			break;
        case ID_LOGDEFAULTPATH:
			s = ts->LogDefaultPathW;
			break;
		default:
			s = NULL;
			break;
	}
	return s;
}
#endif /* TT4 */

LPTSTR TTXGetPath(PTTSet ts, UINT uid)
{
	LPTSTR s = NULL;
#ifdef TT4
	s = TTXGetPath_v4(ts, uid);
#else
	if (IS_TT4())
	{
		LPSTR p = TTXGetPath_v4(ts, uid);
		return (p) ? (p[0]) ? toTC(p) : NULL : NULL;
	}
	s = TTXGetPath_v5(ts, uid);
#endif /* TT4 */
	return (s) ? (s[0]) ? _tcsdup(s) : NULL : NULL;
}

static BOOL TTXSetPath_v4(PTTSet ts, UINT uid, LPSTR s)
{
	//not use wide character
	switch(uid)
	{
		case ID_HOMEDIR:
			strncpy_s(ts->HomeDir, sizeof(ts->HomeDir), s, _TRUNCATE);
			break;
        case ID_SETUPFNAME:
			strncpy_s(ts->SetupFName, sizeof(ts->SetupFName), s, _TRUNCATE);
			break;
        case ID_KEYCNFNM:
			strncpy_s(ts->KeyCnfFN, sizeof(ts->KeyCnfFN), s, _TRUNCATE);
			break;
        case ID_LOGFN:
			strncpy_s(ts->LogFN, sizeof(ts->LogFN), s, _TRUNCATE);
			break;
        case ID_MACROFN:
			strncpy_s(ts->MacroFN, sizeof(ts->MacroFN), s, _TRUNCATE);
			break;
        case ID_UILANGUAGEFILE:
			strncpy_s(ts->UILanguageFile, sizeof(ts->UILanguageFile), s, _TRUNCATE);
			break;
        case ID_UILANGUAGEFILE_INI:
			strncpy_s(ts->UILanguageFile_ini, sizeof(ts->UILanguageFile_ini), s, _TRUNCATE);
			break;
        case ID_EXEDIR:
			return FALSE;
        case ID_LOGDIR:
			return FALSE;
        case ID_FILEDIR:
			strncpy_s(ts->FileDir, sizeof(ts->FileDir), s, _TRUNCATE);
			break;
        case ID_LOGDEFAULTPATH:
			return FALSE;
		default:
			return FALSE;
	}
	return TRUE;
}

#ifndef TT4
static BOOL TTXSetPath_v5(PTTSet ts, UINT uid, LPTSTR s)
{
	LPSTR p;
	switch(uid)
	{
		case ID_HOMEDIR:
			free(ts->HomeDirW);
			ts->HomeDirW = _wcsdup(s);
			p = toMB(s);
			strncpy_s(ts->HomeDir, sizeof(ts->HomeDir), p, _TRUNCATE);
			free(p);
			break;
        case ID_SETUPFNAME:
			free(ts->SetupFNameW);
			ts->SetupFNameW = _wcsdup(s);
			p = toMB(s);
			strncpy_s(ts->SetupFName, sizeof(ts->SetupFName), p, _TRUNCATE);
			free(p);
			break;
        case ID_KEYCNFNM:
			free(ts->KeyCnfFNW);
			ts->KeyCnfFNW = _wcsdup(s);
			p = toMB(s);
			strncpy_s(ts->KeyCnfFN, sizeof(ts->KeyCnfFN), p, _TRUNCATE);
			free(p);
			break;
        case ID_LOGFN:
			free(ts->LogFNW);
			ts->LogFNW = _wcsdup(s);
			p = toMB(s);
			strncpy_s(ts->LogFN, sizeof(ts->LogFN), p, _TRUNCATE);
			free(p);
			break;
        case ID_MACROFN:
			free(ts->MacroFNW);
			ts->MacroFNW = _wcsdup(s);
			p = toMB(s);
			strncpy_s(ts->MacroFN, sizeof(ts->MacroFN), p, _TRUNCATE);
			free(p);
			break;
        case ID_UILANGUAGEFILE:
			free(ts->UILanguageFileW);
			ts->UILanguageFileW = _wcsdup(s);
			p = toMB(s);
			strncpy_s(ts->UILanguageFile, sizeof(ts->UILanguageFile), p, _TRUNCATE);
			free(p);
			break;
        case ID_UILANGUAGEFILE_INI:
			free(ts->UILanguageFileW_ini);
			ts->UILanguageFileW_ini = _wcsdup(s);
			p = toMB(s);
			strncpy_s(ts->UILanguageFile_ini, sizeof(ts->UILanguageFile_ini), p, _TRUNCATE);
			free(p);
			break;
        case ID_EXEDIR:
			free(ts->ExeDirW);
			ts->ExeDirW = _wcsdup(s);
			break;
        case ID_LOGDIR:
			free(ts->LogDirW);
			ts->LogDirW = _wcsdup(s);
			break;
        case ID_FILEDIR:
			free(ts->FileDirW);
			ts->FileDirW = _wcsdup(s);
			p = toMB(s);
			strncpy_s(ts->FileDir, sizeof(ts->FileDir), p, _TRUNCATE);
			free(p);
			break;
        case ID_LOGDEFAULTPATH:
			free(ts->LogDefaultPathW);
			ts->LogDefaultPathW = _wcsdup(s);
			p = toMB(s);
			strncpy_s(ts->reserve_LogDefaultPath, sizeof(ts->reserve_LogDefaultPath), p, _TRUNCATE);
			free(p);
			break;
		default:
			return FALSE;
	}
	return TRUE;
}
#endif /* TT4 */

BOOL TTXSetPath(PTTSet ts, UINT uid, LPTSTR s)
{
#ifdef TT4
	return TTXSetPath_v4(ts, uid, s);
#else
	if (IS_TT4())
	{
		LPSTR p = toMB(s);
		BOOL b = TTXSetPath_v4(ts, uid, p);
		TTXFree(&p);
		return b;
	}
	return TTXSetPath_v5(ts, uid, s);
#endif /* TT4 */
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////

LPSTR WC2MB(UINT cp, LPWSTR pwzSrc)
{
	int n = WideCharToMultiByte(cp, 0, pwzSrc, -1, NULL, 0, NULL, NULL);
	n = (pwzSrc != NULL) ? n : 0;
	if( n > 0 )
	{
		PSTR pszBuf = (PSTR)malloc(sizeof(CHAR) * n);
		if (pszBuf)
			WideCharToMultiByte(cp, 0, pwzSrc, -1, pszBuf, n, NULL, NULL);
		return pszBuf;
	}
	return NULL;
}

LPWSTR MB2WC(UINT cp, LPSTR pszSrc)
{
	int n = MultiByteToWideChar(cp, 0, pszSrc, -1, NULL, 0);
	n = (pszSrc != NULL) ? n : 0;
	if( n > 0 )
	{
		PWSTR pwzBuf = (PWSTR)malloc(sizeof(WCHAR) * n);
		if (pwzBuf)
			MultiByteToWideChar(cp, 0, pszSrc, -1, pwzBuf, n);
		return pwzBuf;
	}
	return NULL;
}

LPVOID TTXAlloc(size_t sz)
{
	return malloc(sz);
}

BOOL TTXFree(LPVOID *pBuf)
{
	free(*pBuf);
	pBuf = NULL;
	return TRUE;
}

BOOL TTXDup(LPTSTR *pBuf, size_t sz, LPTSTR szSrc)
{
	LPTSTR szOrg;
	LPTSTR szDst;
	LPTSTR s;

	szOrg = *pBuf;
	szDst = (LPTSTR)TTXAlloc((sz + 1) * sizeof(TCHAR));
	s = (szSrc) ? szSrc : (szOrg) ? szOrg : _T("");
	_tcscpy_s(szDst, sz + 1, s);
	TTXFree(pBuf);
	*pBuf = szDst;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
// path
// fileapi.h は使わないようなので代替え実装、互換性はない
// ドライブ名やルートパスなどは正しく認識出来ないかも
// 共有フォルダなどは正しく認識出来ないかも
// MAX_PATH 以上は対象外
// MBS(ShiftJIS)は対応しない(「@[\]^_`{|}~」の文字コードはASCII文字として扱う)

// find address of file name
// src のパスからファイル名の位置を取得
LPTSTR FindFileName(LPCTSTR path)
{
	LPTSTR p;
	p = _tcsrchr(path, _T('\\'));
	p = p ? (p + 1) : path;
	return p;
}

// find address of file extension
// path からファイル拡張子の位置を取得
LPTSTR FindFileExt(LPCTSTR path)
{
	LPTSTR p, q;
	p = _tcsrchr(path, _T('\\'));
	p = p ? (p + 1) : path;
	q = _tcsrchr(p, _T('.'));
	return (p == q) ? NULL : q;
}

// find path next component name
// path の次のパスセグメントの位置を取得
// path の最後がパス区切り文字の場合は、それは無いものとして扱う
// path にパス区切り文字が無い場合は全体を対象とする
LPTSTR FindPathNextComponent(LPCTSTR path)
{
	LPTSTR sp, ep, p;

	sp = (LPTSTR)path;
	ep = sp + _tcslen(sp);
	p = ep;
	if (p > sp) {
		p--;
		ep = (*p == _T('\\') || *p == _T('/')) ? (ep - 1) : ep;
		p = ep;
		while (p > sp) {
			p--;
			if (*p == _T('\\') || *p == _T('/')) {
				*p = 0;
				p++;
				break;
			}
		}
	}
	return p;
}

// get parent path
// src の親ディレクトリを取得し、サイズ dst_sz の dst に複製
// src の最後がパス区切り文字の場合は、それは無いものとして扱う
// src にパス区切り文字が無い場合は全体を対象とする
// dst=NULL は禁止
LPTSTR GetParentPath(LPTSTR dst, size_t dst_sz, LPCTSTR src)
{
	LPTSTR sp, ep, p;

	sp = (LPTSTR)src;
	ep = sp + _tcslen(sp);
	p = ep;
	if (p > sp) {
		p--;
		ep = (*p == '\\' || *p == '/') ? (ep - 1) : ep;
		p = ep;
		while (p > sp) {
			p--;
			if (*p == '\\' || *p == '/' || *p == ':') {
				break;
			}
		}
	}
	_tcsncpy_s(dst, dst_sz, sp, p - sp);
	return dst;
}

// get path segment name
// src のファイル名を取得し、サイズ dst_sz の dst に複製
// src の最後がパス区切り文字の場合は、それは無いものとして扱う
// src にパス区切り文字が無い場合は全体を対象とする
// dst=NULL は禁止
LPTSTR GetPathName(LPTSTR dst, size_t dst_sz, LPCTSTR src)
{
	LPTSTR sp, ep, p;

	sp = (LPTSTR)src;
	ep = sp + _tcslen(sp);
	p = ep;
	if (p > sp) {
		p--;
		ep = (*p == '\\' || *p == '/') ? (ep - 1) : ep;
		p = ep;
		while (p > sp) {
			p--;
			if (*p == '\\' || *p == '/' || *p == ':') {
				p++;
				break;
			}
		}
	}
	_tcsncpy_s(dst, dst_sz, p, ep - p);
	return dst;
}

// get linearized path
// src のパスから「.」や「..」を取り除いて直列化したパスを複製
// dst=NULL は禁止
LPTSTR GetLinearizedPath(LPTSTR dst, size_t dst_sz, LPCTSTR src)
{
	LPTSTR sp, ep, p, p2;
	INT_PTR n;

	if ((dst == NULL) || (src == NULL))
		return NULL;
	sp = (LPTSTR)src;
	ep = sp + _tcslen(sp);
	_tcscpy_s(dst, dst_sz, _T(""));
	if (sp[0] == _T('\\') && sp[1] == _T('\\')) {
		sp += 2;
		while (*sp) {
			if (*sp == _T('\\')) {
				sp++;
				break;
			}
			sp++;
		}
		_tcsncpy_s(dst, dst_sz, src, sp - src);
	}
	p = p2 = sp;
	while (p <= ep) {
		if (*p == _T('\\') || *p == _T('/') || p == ep) {
			n = p - sp;
			if (n > 0 && (p[-1] == _T('\\') || p[-1] == _T('/'))) {
				// Nothing
			}
			else if (n > 1 && p[-1] == _T('.') && (p[-2] == _T('\\') || p[-2] == _T('/'))) {
				// Nothing
			}
			else if (n > 2 && p[-1] == _T('.') && p[-2] == _T('.') && (p[-3] == _T('\\') || p[-3] == _T('/'))) {
				p2 = _tcsrchr(dst, _T('\\'));
				if (!p2) {
					p++;
					p2 = dst;
				}
				*p2 = 0;
			}
			else {
				_tcsncat_s(dst, dst_sz, sp, p - sp);
				n = _tcslen(dst) - 1;
				if (n >= 0 && dst[n] == _T('/'))
					dst[n] = _T('\\');
			}
			sp = p;
		}
		p++;
	}
	return dst;
}

// get absolute path
// src のパスから絶対パスを取得
// src が相対パスの場合は、base ファイルを基準とする
// dst=NULL は禁止
//「/」文字は対応していない
LPTSTR GetAbsolutePath(LPTSTR dst, size_t dst_sz, LPCTSTR src, LPCTSTR base)
{
	TCHAR buf[MAX_PATH];
	LPTSTR p;

	_tcscpy_s(buf, sizeof(buf) / sizeof(buf[0]), _T(""));
	p = _tcschr(src, _T('\\'));
	if (p != src) {
		if (p == NULL || p[-1] != _T(':')) {
			GetParentPath(buf, sizeof(buf) / sizeof(buf[0]), base);
			_tcscat_s(buf, sizeof(buf) / sizeof(buf[0]), _T("\\"));
		}
	}
	_tcscat_s(buf, sizeof(buf) / sizeof(buf[0]), src);
	return GetLinearizedPath(dst, dst_sz, buf);
}

// get related path
// src のパスから相対パスを取得
// src が絶対パスの場合は、base ファイルを基準とする
//ただし「..」は最大 lv 個までとし、それ以上の場合は絶対パスのままとする
// dst=NULL は禁止
//「/」文字は対応していない
LPTSTR GetRelatedPath(LPTSTR dst, size_t dst_sz, LPCTSTR src, LPCTSTR base, int lv)
{
	TCHAR buf[MAX_PATH];
	int i, j, k;
	LPTSTR p;

	i = j = 0;
	GetLinearizedPath(buf, sizeof(buf) / sizeof(buf[0]), src);
	while (buf[i]) {
		if (!base[i]) {
			j = i + 1;
			break;
		}
		if (_totupper(base[i]) != _totupper(buf[i]))
			break;
		if (base[i++] == _T('\\'))
			j = i;
	}
	p = buf + j;
	if (base[i] == _T('\\') && buf[i] == 0) {
		j = i + 1;
		p = buf + i;
	}

	_tcscpy_s(dst, dst_sz, _T(""));
	if (j) {
		i = j;
		k = 0;
		while (base[i]) {
			if (k > lv) {
				p = buf;
				_tcscpy_s(dst, dst_sz, _T(""));
				break;
			}
			while (base[i] && base[i] != _T('\\')) i++;
			if (!base[i])
				break;
			i++;
			if (dst[0]) {
				_tcscat_s(dst, dst_sz, _T("\\"));
			}
			_tcscat_s(dst, dst_sz, _T(".."));
			k++;
		}
	}
	if (dst[0] && p[0])
		_tcscat_s(dst, dst_sz, _T("\\"));
	_tcscat_s(dst, dst_sz, p);
	return dst;
}

LPTSTR GetContractPath(LPTSTR dst, size_t dst_sz, LPTSTR src)
{
	size_t src_sz;
	LPTSTR buf;
	size_t buf_sz;
	int i;
	LPCTSTR name;
	LPCTSTR env_name[] = {
		_T("LOCALAPPDATA"), _T("APPDATA"), 
		_T("OneDrive"), _T("OneDriveConsumer"), 
		_T("USERPROFILE"), _T("ALLUSERPROFILE"), 
		_T("ProgramFiles(x86)"), _T("ProgramFiles"), 
		_T("windir") 
	};

	if ((src == NULL) || (src[0] == 0))
		return NULL;

	src_sz = _tcsclen(src);
	buf = (PTSTR)malloc((src_sz + 1) * sizeof(TCHAR));

	for (i = 0; i < sizeof(env_name)/sizeof(env_name[0]); i ++)
	{
		name = env_name[i];
		if (GetEnvironmentVariable(name, buf, (DWORD)src_sz + 1) == 0)
			continue;
		buf_sz = _tcsclen(buf);
		if (buf[0] == 0)
			continue;
		if ((buf_sz == 0) || (_tcsnicmp(src, buf, buf_sz) != 0))
			continue;
		_tcscpy_s(buf, src_sz + 1, &src[buf_sz]);
		_sntprintf_s(dst, dst_sz, dst_sz, _T("%%%s%%%s"), name, buf);
		free(buf);
		return dst;
	}
	free(buf);
	return src;
}

// get file name
// src のパスから拡張子を除いたファイルの名前を取得
// dst=NULL は禁止
LPTSTR GetFileName(LPTSTR dst, int dst_sz, LPCTSTR src)
{
	LPTSTR p;
	p = FindFileName(src);
	if (p) {
		_tcscpy_s(dst, dst_sz, p);
		p = _tcsrchr(dst, '.');
		if (p) {
			*p = 0;
		}
		return dst;
	}
	return (LPTSTR)src;
}

// get file extension
// src のパスから拡張子を取得
// dst=NULL は禁止
//「.」は含まない
//無い場合は NULL を返す
//パス区切り文字「\」「/」は認識しない
LPTSTR GetFileExt(LPTSTR dst, int dst_sz, LPTSTR src)
{
	LPTSTR p;
	p = FindFileExt(src);
	if (p) {
		_tcscpy_s(dst, dst_sz, ++p);
		return dst;
	}
	return NULL;
}

// remove last slash from path
// path の最後の文字が区切り文字なら最後の文字を削除
//関数は破壊関数
//最後のパス区切り文字は削除
//パス区切り文字「/」は「\」に変換
LPTSTR RemovePathSlash(LPTSTR path)
{
	LPTSTR p;

	p = path;
	while (p = _tcschr(p, _T('/'))) {
		*p = _T('\\');
	}
	p = path + _tcslen(path) - 1;
	if (p >= path && (*p == _T('\\'))) {
		*p = 0;
	}
	return path;
}

/// remove last slash from path
LPTSTR RemoveFileName(LPTSTR path)
{
	LPTSTR p;

	p = FindFileName(path);
	p = (p == path) ? path : (p - 1);
	*p = 0;
	return path;
}

/// remove last slash from path
LPTSTR RemoveFileExt(LPTSTR path)
{
	LPTSTR p;

	p = FindFileExt(path);
	if (p && *p) {
		*p = 0;
	}
	return path;
}

// combine path
// path に子要素 fn を連結
// path=NULL は禁止
LPTSTR CombinePath(LPTSTR path, size_t sz, LPCTSTR fn)
{
	RemovePathSlash(path);
	_tcscat_s(path, sz, _T("\\"));
	_tcscat_s(path, sz, fn);
	return path;
}

// test exist file
//ファイルが存在するか確認
//存在していれは TRUE を返す
BOOL FileExists(LPCTSTR path)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	hFind = FindFirstFile(path, &fd);
	if (hFind == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	FindClose(hFind);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////

//セクション名の連結文字列取得(開放はfree(outp))
//戻り値は文字列を格納したバッファのサイズ、取得出来ないときは 0
DWORD GetIniSects(strset_t *outp, DWORD sz, DWORD nsz, LPCTSTR fn)
{
	LPTSTR buf;

	buf = (LPTSTR)malloc(sz * sizeof(TCHAR));
	if (!buf)
		return 0;

	while (sz < GetPrivateProfileSectionNames(buf, sz, fn) + 3) {
		free(buf);
		sz += nsz ? nsz : sz;
		buf = (LPTSTR)malloc(sz * sizeof(TCHAR));
		if (!buf)
			return 0;
	}
	if (outp) {
		if (*outp) {
			free(*outp);
			*outp = NULL;
		}
		*outp = (strset_t)buf;
	}
	else {
		free(buf);
	}

	return sz;
}

//セクション内の連結文字列取得(開放はfree(outp))
//戻り値は文字列を格納したバッファのサイズ、取得出来ないときは 0
DWORD GetIniStrSet(LPCTSTR sect, strset_t *outp, DWORD sz, DWORD nsz, LPCTSTR fn)
{
	LPTSTR buf;

	buf = (LPTSTR)malloc(sz * sizeof(TCHAR));
	if (!buf)
		return 0;
	while (sz < GetPrivateProfileSection(sect, buf, sz, fn) + 3) {
		free(buf);
		sz += nsz ? nsz : sz;
		buf = (LPTSTR)malloc(sz * sizeof(TCHAR));
		if (!buf)
			return 0;
	}
	if (outp) {
		if (*outp) {
			free(*outp);
			*outp = NULL;
		}
		*outp = (strset_t)buf;
	}
	else {
		free(buf);
	}

	return sz;
}
//セクション内のキーワード名の連結文字列取得(開放はfree(outp))
//戻り値は文字列を格納したバッファのサイズ、取得出来ないときは 0
DWORD GetIniKeys(LPCTSTR sect, strset_t *outp, DWORD sz, DWORD nsz, LPCTSTR fn)
{
	return GetIniString(sect, NULL, _T(""), outp, sz, nsz, fn);
}

// ON/OFF 設定を設定ファイルから取得
// INIファイル fn の sect セクションに name キーの値が on なら TRUE を返す
//その他は FALSE を返す
//値が無ければ、デフォルト値 bDefault を返す
BOOL GetIniOnOff(LPCTSTR sect, LPCTSTR name, BOOL bDefault, LPCTSTR fn)
{
	TCHAR buf[32];

	GetPrivateProfileString(sect, name, _T(""), buf, sizeof(buf) / sizeof(buf[0]), fn);
	return buf[0] ? ((_tcsnicmp(_T("on"), buf, 2) == 0) ? TRUE : FALSE) : bDefault;
}

//数値を設定ファイルから取得
// INIファイル fn の sect セクションに name キーの値を数値として取得
//値が無ければ、デフォルト値 nDefault を返す
UINT GetIniNum(LPCTSTR sect, LPCTSTR name, INT nDefault, LPCTSTR fn)
{
	return GetPrivateProfileInt(sect, name, nDefault, fn);
}

//文字列を設定ファイルから取得
// INIファイル fn の sect セクションに name キーの値を文字列として outp に取得
//値が無ければ、デフォルト値 sDefault を outp に返す
//戻り値は文字列を格納したバッファのサイズ、取得出来ないときは 0
DWORD GetIniString(LPCTSTR sect, LPCTSTR name, LPCTSTR sDefault, LPTSTR *outp, DWORD sz, DWORD nsz, LPCTSTR fn)
{
	LPTSTR buf;

	buf = (LPTSTR)malloc(sz * sizeof(TCHAR));
	if (!buf)
		return 0;

	while (sz < GetPrivateProfileString(sect, name, sDefault, buf, sz, fn) + 3) {
		free(buf);
		sz += nsz ? nsz : sz;
		buf = (LPTSTR)malloc(sz * sizeof(TCHAR));
		if (!buf)
			return 0;
	}
	if (outp) {
		if (*outp) {
			free(*outp);
			*outp = NULL;
		}
		*outp = (strset_t)buf;
	}
	else {
		free(buf);
	}

	return sz;
}

LPSTR GetIniStringA(LPCTSTR sect, LPCTSTR name, LPCTSTR sDefault, LPCTSTR fn)
{
	LPTSTR buf;
	PCHAR p;

	buf = NULL;
	GetIniString(sect, name, sDefault, &buf, 256, 256, fn);
	p = toMB(buf);
	free(buf);
	return p;
}

// OM/OFF 設定を設定ファイルに書き込む
// INIファイル fn の sect セクションに name キーの値 bFlag を書き込む
// bEnable=TRUE ならば、書き込む値は、bFlag が FALSE 以外なら "on", FALSEなら "off"
// bEnable=FALSE ならば name キーを削除
BOOL WriteIniOnOff(LPCTSTR sect, LPCTSTR name, int bFlag, BOOL bEnable, LPCTSTR fn)
{
	LPTSTR p = bEnable ? ((bFlag != FALSE) ? _T("on") : _T("off")) : NULL;
	return WritePrivateProfileString(sect, name, p, fn);
}

//数値を設定ファイルに書き込む
// INIファイル fn の sect セクションに name キーに数値 val を書き込む
// bEnable=FALSE ならば name キーを削除
BOOL WriteIniNum(LPCTSTR sect, LPCTSTR name, int val, BOOL bEnable, LPCTSTR fn)
{
	TCHAR buf[16];
	LPTSTR p = NULL;
	if (bEnable || val) {
		_sntprintf_s(buf, sizeof(buf), _TRUNCATE, _T("%d"), val);
		p = buf;
	}
	return WritePrivateProfileString(sect, name, p, fn);
}
