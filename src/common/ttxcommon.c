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
BOOL TTXIgnore(int order, LPCTSTR name, WORD version)
{
	// TODO: test order, test version
	TCHAR buf[32];
	LPTSTR p;

	GetPrivateProfileString(_T(TTX_SECTION), name, _T(""), 
		buf, sizeof(buf) / sizeof(buf[0]), _T(INI_FILE));
	if (_tcsnicmp(_T("off"), buf, 3) == 0)
		return TRUE;
	p = _tcschr(buf, _T(','));
	_menu_offset = (NULL == p) ? 0 : _ttoi(1 + p);
	return FALSE;
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
	size_t buf_sz;
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

LPTSTR TTXGetPath(PTTSet ts, UINT uid)
{
	LPTSTR s = NULL;
#ifdef TT4
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
			s = NULL;
			break;
        case ID_LOGDIR:
			s = NULL;
			break;
        case ID_FILEDIR:
			s = ts->FileDir;
			break;
		default:
			s = NULL;
			break;
	}
#else
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
		default:
			s = NULL;
			break;
	}
#endif /* TT4 */
	return (s) ? _tcsdup(s) : NULL;
}

BOOL TTXSetPath(PTTSet ts, UINT uid, LPTSTR s)
{
#ifdef TT4
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
		default:
			return FALSE;
	}
#else
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
		default:
			return FALSE;
	}
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

BOOL TTXFree(LPVOID *pBuf)
{
	free(*pBuf);
	pBuf = NULL;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////

//文字列 P 中に文字 c を検索し次のポインタを返す
// p が NULL または文字列最終の場合はそのまま返す
LPTSTR strskip(LPTSTR p, TCHAR c)
{
	if (p) {
		while (*p) {
			if (*p == c) {
				return ++p;
			}
			p++;
		}
	}
	return p;
}

///////////////////////////////////////////////////////////////////////////

//連結文字列
//文字列(NULL 終端文字列)を詰めて連結し最後だけ NULL が2回連続になる文字列を連結文字列とする
//空文字列は含めることはできない

//連結文字列から文字列の順次切り出し
// ctx は現在の走査位置保存用変数のポインタ
// p は連結文字列の開始位置(走査開始時))、NULL の場合は ctx の値を使う(2つ目以降の操作時)
//すべて切り出したら NULL を返す
LPTSTR StrSetTok(strset_t p, strset_t *ctx)
{
	LPTSTR ret;

	p = p ? p : (ctx ? *ctx : 0);
	if (!p || !*p)
		return NULL;
	ret = p;
	while (*p++)
		;
	if (ctx)
		*ctx = p;
	return ret;
}

//連結文字列のサイズと文字列個数を取得
//文字列の個数が不要の時は cnt=NULL とする
// p は NULL は禁止
int StrSetSize(strset_t p, int *cnt)
{
	int _cnt = 0;
	int sz = 1;
	while (*p) {
		while (*p++) sz++;
		sz++;
		_cnt++;
	}
	if (cnt)
		*cnt = _cnt;
	return sz;
}

//連結文字列からキーワードの連結文字列作成
//「=」をセパレート文字として前半をキーワードとする
// dst, src は NULL 禁止
// dst のサイズは少なくとも src のサイズ以上であること
int StrSetKeys(strset_t dst, strset_t src)
{
	const TCHAR sep = _T('=');
	BOOL enb;
	int cnt;

	cnt = 1;
	while (*src) {
		enb = TRUE;
		while (*src) {
			if (enb) {
				if (*src == sep) {
					enb = FALSE;
					*dst = 0;
				}
				else {
					*dst = *src;
				}
				dst++;
				cnt++;
			}
			src++;
		}
		*dst = 0;
		dst++;
		cnt++;
		src++;
	}
	*dst = 0;
	return cnt;
}

//連結文字列からキーワードのインデックス取得
// p, k は NULL 禁止
//キーワードが見つからない場合は文字列個数を返す
int StrSetFindIndex(strset_t p, LPTSTR k)
{
	const TCHAR sep = _T('=');
	int cnt = 0;
	while (*p) {
		LPTSTR p2 = p;
		LPTSTR k2 = k;
		while (*p2 && *k2 && (*p2 != sep) && (*p2 == *k2)) {
			p2++;
			k2++;
		}
		if ((*p2 == sep) && (*k2 == 0)) {
			return cnt;
		}
		cnt++;
		while (*p++)
			;
	}
	return cnt;
}

//連結文字列からキーワードで検索し文字列取得
// p, k は NULL 禁止
//キーワードが見つからない場合は空文字列を返す
LPTSTR StrSetFindKey(strset_t p, LPTSTR k)
{
	const TCHAR sep = _T('=');
	while (*p) {
		LPTSTR p2 = p;
		LPTSTR k2 = k;
		while (*p2 && *k2 && (*p2 == *k2)) {
			p2++;
			k2++;
		}
		if ((*p2 == sep) && (*k2 == 0)) {
			return p;
		}
		while (*p++)
			;
	}
	return p;
}

//連結文字列から値で検索し文字列取得
//値は「=」文字の次以降の文字列
// p, v は NULL 禁止
//値が見つからない場合は空文字列を返す
LPTSTR StrSetFindVal(strset_t p, LPTSTR v)
{
	const TCHAR sep = _T('=');
	while (*p) {
		LPTSTR p2 = strskip(p, sep);
		LPTSTR v2 = v;
		if (*p2) {
			while (*p2 && *v2 && (*p2 == *v2)) {
				p2++;
				v2++;
			}
			if ((*p2 == 0) && (*v2 == 0)) {
				return p;
			}
		}
		while (*p++)
			;
	}
	return p;
}

//連結文字列からn番目の文字列を取得
// p は NULL 禁止
// n が範囲外なら空文字列を返す
LPTSTR StrSetAt(strset_t p, int n)
{
	int cnt = 0;
	while (*p && (cnt < n)) {
		cnt++;
		while (*p++)
			;
	}
	return p;
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
	int n;

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
	while (base[i] && buf[i] && (_totupper(base[i]) == _totupper(buf[i]))) {
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
		if (GetEnvironmentVariable(name, buf, src_sz + 1) == 0)
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
LPTSTR CombinePath(LPTSTR path, int sz, LPCTSTR fn)
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

///////////////////////////////////////////////////////////////////////////

// get right-bottom point from window item
//ウインドウ内のUIコンポーネントの右下のポイントを pt に取得
VOID GetPointRB(HWND hWnd, UINT uItem, POINT *pt)
{
	HWND hItem;
	RECT rect;
	hItem = GetDlgItem(hWnd, uItem);
	GetWindowRect(hItem, &rect);
	pt->x = rect.right;
	pt->y = rect.bottom;
	ScreenToClient(hWnd, pt);
	GetClientRect(hWnd, &rect);
	pt->x = rect.right - pt->x;
	pt->y = rect.bottom - pt->y;
}

// move right-bottom point within window item
//ウインドウのサイズ変更時、ウインドウ内のUIコンポーネントの位置・サイズを調整
// uFlag==RB_LEFT の場合、ウインドウの右側とUIコンポーネントの左側の距離を保つ
// uFlag==RB_TOP の場合、ウインドウの下側とUIコンポーネントの上側の距離を保つ
// uFlag==RB_RIGHT の場合、ウインドウの右側とUIコンポーネントの右側の距離を保つ
// uFlag==RB_BOTTOM の場合、ウインドウの下側とUIコンポーネントの下側の距離を保つ
VOID MovePointRB(HWND hWnd, UINT uItem, POINT *ptRB, UINT uFlag)
{
	HWND hItem = GetDlgItem(hWnd, uItem);
	RECT rect;
	POINT pt;
	LONG x, y, dx, dy, xx, yy;
	GetWindowRect(hItem, &rect);
	pt.x = rect.left;
	pt.y = rect.top;
	dx = rect.right - rect.left;
	dy = rect.bottom - rect.top;
	ScreenToClient(hWnd, &pt);
	GetClientRect(hWnd, &rect);
	x = pt.x;
	y = pt.y;
	xx = rect.right - dx - pt.x - ptRB->x;
	yy = rect.bottom - dy - pt.y - ptRB->y;
	if (uFlag & RB_LEFT) {
		x += xx;
		dx -= xx;
	}
	if (uFlag & RB_TOP) {
		y += yy;
		dy -= yy;
	}
	if (uFlag & RB_RIGHT) {
		dx += xx;
	}
	if (uFlag & RB_BOTTOM) {
		dy += yy;
	}
	MoveWindow(hItem, x, y, dx, dy, 1);
}

// get window size to point structure
//ウインドウのサイズをpoint型で取得
VOID GetWindowSize(HWND hWnd, POINT *pt)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	if (pt->x)
		pt->x = rect.right - rect.left;
	if (pt->y)
		pt->y = rect.bottom - rect.top;
}

// set window size from point structure
//ウインドウのサイズをpoint型で設定
VOID SetWindowSize(HWND hWnd, POINT *pt)
{
	if (pt->x || pt->y) {
		RECT rect;
		LONG w, h;
		GetWindowRect(hWnd, &rect);
		w = (pt->x) ? pt->x : (rect.right - rect.left);
		h = (pt->y) ? pt->y : (rect.bottom - rect.top);
		MoveWindow(hWnd, rect.left, rect.top, w, h, TRUE);
		// SendMessage(hWnd, WM_SIZING, 0, MAKELPARAM(TRUE, 0));
	}
}

// move window relative direction
//基準となるウインドウ hWndBase にたいして隣接する位置のウインドウ hWnd を移動
//ウインドウの位置は、 uPos 方向で指定
// uPos==0 の場合、右側
// uPos==1 の場合、下側
// uPos==2 の場合、左側
// uPos==3 の場合、上側
VOID SetHomePosition(HWND hWnd, HWND hWndBase, UINT uPos)
{
	RECT rect;
	LONG x, y, w, h;

	GetWindowRect(hWnd, &rect);
	w = rect.right - rect.left;
	h = rect.bottom - rect.top;

	GetWindowRect(hWndBase, &rect);
	x = rect.left;
	y = rect.top;
	switch (uPos) {
		case 0:
			x = rect.right;
			y = rect.top;
			break;
		case 1:
			x = rect.left - w;
			y = rect.top;
			break;
		case 2:
			x = rect.left;
			y = rect.top - h;
			break;
		case 3:
			x = rect.left;
			y = rect.bottom;
			break;
		default:
			x = rect.left;
			y = rect.top;
			break;
	}
	MoveWindow(hWnd, x, y, w, h, TRUE);
}

#if 0
///test to exist cursor position into window item
BOOL TestCursorPos(HWND hWnd, UINT uItem)
{
	HWND hItem;
	RECT rect;
	POINT pt;
	hItem = GetDlgItem(hWnd, uItem);
	GetWindowRect(hItem, &rect);
	GetCursorPos(&pt);
	return (pt.x < rect.left || pt.x > rect.right || pt.y < rect.top || pt.y > rect.bottom);
}
#endif

// adjust window position to center of parent window
//ウインドウを親ウインドウの中心に移動
VOID MoveParentCenter(HWND hWnd)
{
	RECT prc, rc;
	GetWindowRect(GetParent(hWnd), &prc);
	GetClientRect(hWnd, &rc);
	int x = (prc.left + prc.right - rc.right) / 2;
	int y = (prc.top + prc.bottom - rc.bottom) / 2;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

// create dialog font and set to phFont (require to delete item after)
//ダイアログにフォントを設定
//仕様が終わったら DeleteObject すること
VOID SetDlgFont(HWND hWnd, UINT uItem, HFONT *phFont, LONG uH, LPTSTR szFont)
{
	HWND hItem = GetDlgItem(hWnd, uItem);
	*phFont = CreateFont(uH, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						 DEFAULT_QUALITY, FF_DONTCARE, (szFont[0] ? szFont : _T("ＭＳ ゴシック")));
	SendMessage(hItem, WM_SETFONT, (WPARAM)(*phFont), MAKELPARAM(TRUE, 0));
}

// open to file select dialog
//ファイル選択ダイアログを表示させる
//パスは、hWnd ウインドウの editCtrl UI コンポーネントに読み書きする
// hWnd ウインドウが無いか editCtl が -1 の場合は、 UI コンポーネントは使用せず、 szPath を読み込む
// szPath が NULL 以外の場合は szPath にも書き込む(サイズは MAX_PATH 以上であること)
//成功した場合 TRUE を返す
BOOL OpenFileDlg(HWND hWnd, UINT editCtl, LPTSTR szTitle, LPTSTR szFilter, LPTSTR szPath, LPTSTR fn, int n)
{
	TCHAR szFile[MAX_PATH];
	OPENFILENAME ofn;

	szFile[0] = 0;

	if (hWnd && editCtl != 0xffffffff) {
		GetDlgItemText(hWnd, editCtl, szFile, sizeof(szFile) / sizeof(szFile[0]));
	}
	else {
		_tcscpy_s(szFile, sizeof(szFile) / sizeof(szFile[0]), szPath);
	}

	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
	ofn.lpstrTitle = szTitle;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NODEREFERENCELINKS;

	if (GetOpenFileName(&ofn)) {
		if (fn) {
			GetRelatedPath(szFile, sizeof(szFile) / sizeof(szFile[0]), ofn.lpstrFile, fn, n);
		}
		else {
			_tcscpy_s(szFile, sizeof(szFile) / sizeof(szFile[0]), ofn.lpstrFile);
		}
		if (hWnd && editCtl != 0xffffffff) {
			SetDlgItemText(hWnd, editCtl, szFile);
		}
		if (szPath) {
			_tcscpy_s(szPath, MAX_PATH, szFile);
		}
		return TRUE;
	}
	return FALSE;
}

// open to folder select dialog
//フォルダ選択ダイアログを表示させる
//パスは、hWnd ウインドウの editCtrl UI コンポーネント値に読み書きする
// TODO: メモリ例外になることが無いようにする
static int CALLBACK setDefaultFolder(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED) {
		SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
		SendMessage(hwnd, BFFM_SETEXPANDED, (WPARAM)TRUE, lpData);
	}
	return 0;
}

BOOL OpenFolderDlg(HWND hWnd, UINT editCtl, LPTSTR szTitle, LPTSTR szPath)
{
	BROWSEINFO bi;
	LPITEMIDLIST pidlRoot;
	LPITEMIDLIST pidlBrowse;
	TCHAR buf[MAX_PATH];
	BOOL ret = FALSE;

	if (!SUCCEEDED(SHGetSpecialFolderLocation(hWnd, CSIDL_DESKTOP, &pidlRoot))) {
		return FALSE;
	}

	if (hWnd && editCtl != 0xffffffff) {
		GetDlgItemText(hWnd, editCtl, buf, sizeof(buf) / sizeof(buf[0]));
	}
	else {
		_tcscpy_s(buf, sizeof(buf) / sizeof(buf[0]), szPath);
	}

	bi.hwndOwner = hWnd;
	bi.pidlRoot = pidlRoot;
	bi.pszDisplayName = buf;
	bi.lpszTitle = szTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_VALIDATE | BIF_NEWDIALOGSTYLE;
	bi.lpfn = setDefaultFolder;
	bi.lParam = (LPARAM)buf;
	pidlBrowse = SHBrowseForFolder(&bi);
	if (pidlBrowse != NULL) {
		if (SHGetPathFromIDList(pidlBrowse, szPath)) {
			if (hWnd && editCtl != 0xffffffff) {
				SetDlgItemText(hWnd, editCtl, szPath);
			}
		}
		CoTaskMemFree(pidlBrowse);
	}
	CoTaskMemFree(pidlRoot);

	return ret;
}
