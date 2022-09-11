/*
 * TTX common utility library
 * (C) 2022 tomo3136a
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

#include "ttxcmn_util.h"

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

#if 0
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
#endif
