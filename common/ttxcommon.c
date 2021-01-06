/*
 * TTX common library
 * (C) 2021 tomo3136a
 */

#include "teraterm.h"
#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <shlobj.h>
#include <time.h>

#include "ttxcommon.h"

///////////////////////////////////////////////////////////////////////////

BOOL TTXIgnore(int order, PCHAR name, WORD version)
{
	//TODO: test order, test version
	char buf[16];
	GetPrivateProfileString("Load", name, "", buf, sizeof(buf), ".\\ttx.txt");
	return (_strnicmp("off", buf, 3) == 0);
}

//get Menu ID offset
int MenuOffset(PCHAR name, UINT id, UINT cnt)
{
	//TODO: cnt
	return GetPrivateProfileInt("MenuID", name, id, ".\\ttx.txt") - id;
}

//コマンドラインパラメータ解析
//param を解析し、サイズ sz の buf に複製
//「 」はパラメータ区切り文字
//「;」以降はコメント扱い
//「"」から「"」までは、文字列として扱う
//戻り値は、成功した場合 NULL 以外になる
//buf は NULL 禁止
//MBS(ShiftJIS)は対応しない(「@[\]^_`{|}~」は区切り文字に使用する設計しない)

PCHAR TTXGetParam(PCHAR buf, int sz, PCHAR param)
{
	int i = 0;
	BOOL quoted = FALSE;

	if (param == NULL)
	{
		return NULL;
	}

	buf[0] = '\0';

	//スペーススキップ・終了判定
	while (*param == ' ')
	{
		param++;
	}
	if (*param == '\0' || *param == ';')
	{
		return NULL;
	}

	while (*param != '\0' &&
		   (quoted || *param != ';') &&
		   (quoted || *param != ' '))
	{
		if (*param == '"')
		{
			if (*(param + 1) != '"')
			{
				quoted = !quoted;
				param++;
				continue;
			}
		}
		if (i < sz - 1)
		{
			buf[i++] = *param;
		}
		param++;
	}

	//文字列の最後は必ずNUL文字
	buf[i] = '\0';
	return param;
}

//UI言語指定
UINT UILang(PCHAR lang)
{
	lang += 4;
	return strstr(lang, "Eng")
			   ? 1
			   : strstr(lang, "Jap")
					 ? 2
					 : strstr(lang, "Rus")
						   ? 3
						   : strstr(lang, "Kor")
								 ? 4
								 : strstr(lang, "UTF")
									   ? 5
									   : 1;
}

///////////////////////////////////////////////////////////////////////////

//文字列 P 中に文字 c を検索し次のポインタを返す
//p が NULL または文字列最終の場合はそのまま返す
char *strskip(char *p, char c)
{
	if (p)
	{
		while (*p)
		{
			if (*p == c)
			{
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
//ctx は現在の走査位置保存用変数のポインタ
//p は連結文字列の開始位置(走査開始時))、NULL の場合は ctx の値を使う(2つ目以降の操作時)
//すべて切り出したら NULL を返す
char *StrSetTok(strset_t p, strset_t *ctx)
{
	char *ret;

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
//p は NULL は禁止
int StrSetSize(strset_t p, int *cnt)
{
	int _cnt = 0;
	int sz = 1;
	while (*p)
	{
		while (*p++)
			sz++;
		sz++;
		_cnt++;
	}
	if (cnt)
		*cnt = _cnt;
	return sz;
}

//連結文字列からキーワードの連結文字列作成
//「=」をセパレート文字として前半をキーワードとする
//dst, src は NULL 禁止
//dst のサイズは少なくとも src のサイズ以上であること
int StrSetKeys(strset_t dst, strset_t src)
{
	const char sep = '=';
	BOOL enb;
	int cnt;

	cnt = 1;
	while (*src)
	{
		enb = TRUE;
		while (*src)
		{
			if (enb)
			{
				if (*src == sep)
				{
					enb = FALSE;
					*dst = 0;
				}
				else
				{
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
//p, k は NULL 禁止
//キーワードが見つからない場合は文字列個数を返す
int StrSetFindIndex(strset_t p, char *k)
{
	const char sep = '=';
	int cnt = 0;
	while (*p)
	{
		char *p2 = p;
		char *k2 = k;
		while (*p2 && *k2 && (*p2 != sep) && (*p2 == *k2))
		{
			p2++;
			k2++;
		}
		if ((*p2 == sep) && (*k2 == 0))
		{
			return cnt;
		}
		cnt++;
		while (*p++)
			;
	}
	return cnt;
}

//連結文字列からキーワードで検索し文字列取得
//p, k は NULL 禁止
//キーワードが見つからない場合は空文字列を返す
char *StrSetFindKey(strset_t p, char *k)
{
	const char sep = '=';
	while (*p)
	{
		char *p2 = p;
		char *k2 = k;
		while (*p2 && *k2 && (*p2 == *k2))
		{
			p2++;
			k2++;
		}
		if ((*p2 == sep) && (*k2 == 0))
		{
			return p;
		}
		while (*p++)
			;
	}
	return p;
}

//連結文字列から値で検索し文字列取得
//値は「=」文字の次以降の文字列
//p, v は NULL 禁止
//値が見つからない場合は空文字列を返す
char *StrSetFindVal(strset_t p, char *v)
{
	const char sep = '=';
	while (*p)
	{
		char *p2 = strskip(p, sep);
		char *v2 = v;
		if (*p2)
		{
			while (*p2 && *v2 && (*p2 == *v2))
			{
				p2++;
				v2++;
			}
			if ((*p2 == 0) && (*v2 == 0))
			{
				return p;
			}
		}
		while (*p++)
			;
	}
	return p;
}

//連結文字列からn番目の文字列を取得
//p は NULL 禁止
//n が範囲外なら空文字列を返す
char *StrSetAt(strset_t p, int n)
{
	int cnt = 0;
	while (*p && (cnt < n))
	{
		cnt++;
		while (*p++)
			;
	}
	return p;
}

///////////////////////////////////////////////////////////////////////////

//文字列リスト
//文字列を方方向リンクリストで管理する
//文字列は文字列リストにmallocで複製して使用する
//文字列の最大文字数は 1023 とする(理由はないが一応制限を入れておく)

//文字列リストを空にする
//p は NULL 禁止
void ClearStringList(PPStringList p)
{
	PStringList q;
	while (q = *p)
	{
		(*p) = q->nxt;
		free(q);
	}
}

//文字列リストの最後に文字列を追加
//p, s は NULL 禁止
void AddStringList(PPStringList p, PCHAR s)
{
	while (*p)
		p = &((*p)->nxt);
	int sz = strnlen_s(s, 1024 - 1 - sizeof(TStringList));
	*p = (PStringList)malloc(sizeof(TStringList) + sz + 1);
	(*p)->len = sz;
	(*p)->nxt = NULL;
	strcpy_s((*p)->str, 1024, s);
}

//文字列リストをファイルから読み込む
//1つの文字列はファイルの1行に相当する
//p は NULL 禁止
//ファイルが無い場合は FALSE を返す
//文字列の先頭・末尾のスペースはトリムする
//連続するスペースは1文字に集約する
//制御文字は破棄する
//空文字列は破棄する
BOOL LoadStringList(PPStringList p, PCHAR path)
{
#define STRINGLISTBUFSZ 1024
	char tmp[STRINGLISTBUFSZ];
	char buf[STRINGLISTBUFSZ];
	DWORD dwSize;
	HANDLE hFile;
	int blen;
	DWORD i;
	char ch;

	ClearStringList(p);
	if (!path || !path[0])
		return FALSE;

	hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	blen = 0;
	while (ReadFile(hFile, tmp, sizeof(tmp) / sizeof(tmp[0]), &dwSize, NULL))
	{
		if (dwSize == 0)
			break;
		for (i = 0; i < dwSize; i++)
		{
			ch = tmp[i];
			if (ch < 0 || ch > 0x20)
			{
				buf[blen++] = ch;
				if (blen >= STRINGLISTBUFSZ)
					blen = 0;
				continue;
			}
			if (ch == 0x20)
			{
				if (blen && buf[blen - 1] != 0x20)
				{
					buf[blen++] = 0x20;
					if (blen >= STRINGLISTBUFSZ)
						blen = 0;
				}
				continue;
			}
			if (blen == 0)
				continue;
			if (buf[blen - 1] == 0x20)
				blen--;
			buf[blen] = 0;
			if (blen == 0)
				continue;
			AddStringList(p, buf);
			blen = 0;
		}
	}
	CloseHandle(hFile);
	return TRUE;
}

BOOL info_test_match(PStringList p, PCHAR buf)
{
	while (p)
	{
		if (lstrcmpi(p->str, buf) == 0)
			return TRUE;
		p = p->nxt;
	}
	return FALSE;
}

BOOL info_test_match_head(PStringList p, PCHAR buf)
{
	while (p)
	{
		if (_strnicmp(p->str, buf, p->len) == 0)
			return TRUE;
		p = p->nxt;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////
// path
// fileapi.h は使わないようなので代替え実装、互換性はない
// ドライブ名やルートパスなどは正しく認識出来ないかも
// 共有フォルダなどは正しく認識出来ないかも
// MAX_PATH 以上は対象外
// MBS(ShiftJIS)は対応しない(「@[\]^_`{|}~」の文字コードはASCII文字として扱う)

//find address of file name
//src のパスからファイル名の位置を取得
PCHAR FindFileName(PCHAR path)
{
	PCHAR p;
	p = strrchr(path, '\\');
	p = p ? (p + 1) : path;
	return p;
}

//find address of file extension
//path からファイル拡張子の位置を取得
PCHAR FindFileExt(PCHAR path)
{
	PCHAR p, q;
	p = strrchr(path, '\\');
	p = p ? (p + 1) : path;
	q = strrchr(p, '.');
	return (p == q) ? NULL : q;
}

//find path next component name
//path の次のパスセグメントの位置を取得
//path の最後がパス区切り文字の場合は、それは無いものとして扱う
//path にパス区切り文字が無い場合は全体を対象とする
PCHAR FindPathNextComponent(PCHAR path)
{
	PCHAR sp, ep, p;

	sp = path;
	ep = sp + strlen(sp);
	p = ep;
	if (p > sp)
	{
		p--;
		ep = (*p == '\\' || *p == '/') ? (ep - 1) : ep;
		p = ep;
		while (p > sp)
		{
			p--;
			if (*p == '\\' || *p == '/')
			{
				*p = 0;
				p++;
				break;
			}
		}
	}
	return p;
}

//get parent path
//src の親ディレクトリを取得し、サイズ sz の dst に複製
//src の最後がパス区切り文字の場合は、それは無いものとして扱う
//src にパス区切り文字が無い場合は全体を対象とする
//dst=NULL は禁止
PCHAR GetParentPath(PCHAR dst, int sz, PCHAR src)
{
	PCHAR sp, ep, p;

	sp = src;
	ep = sp + strlen(sp);
	p = ep;
	if (p > sp)
	{
		p--;
		ep = (*p == '\\' || *p == '/') ? (ep - 1) : ep;
		p = ep;
		while (p > sp)
		{
			p--;
			if (*p == '\\' || *p == '/' || *p == ':')
			{
				break;
			}
		}
	}
	strncpy_s(dst, sz, sp, p - sp);
	return dst;
}

//get path segment name
//src のファイル名を取得し、サイズ sz の dst に複製
//src の最後がパス区切り文字の場合は、それは無いものとして扱う
//src にパス区切り文字が無い場合は全体を対象とする
//dst=NULL は禁止
PCHAR GetPathName(PCHAR dst, int sz, PCHAR src)
{
	PCHAR sp, ep, p;

	sp = src;
	ep = sp + strlen(sp);
	p = ep;
	if (p > sp)
	{
		p--;
		ep = (*p == '\\' || *p == '/') ? (ep - 1) : ep;
		p = ep;
		while (p > sp)
		{
			p--;
			if (*p == '\\' || *p == '/' || *p == ':')
			{
				p++;
				break;
			}
		}
	}
	strncpy_s(dst, sz, p, ep - p);
	return dst;
}

//get linearized path
//src のパスから「.」や「..」を取り除いて直列化したパスを複製
//dst=NULL は禁止
PCHAR GetLinearizedPath(PCHAR dst, int sz, PCHAR src)
{
	PCHAR sp, ep, p, p2;
	int n;

	sp = src;
	ep = sp + strlen(sp);
	strcpy_s(dst, sz, "");
	if (sp[0] == '\\' && sp[1] == '\\')
	{
		sp += 2;
		while (*sp)
		{
			if (*sp == '\\')
			{
				sp++;
				break;
			}
			sp++;
		}
		strncpy_s(dst, sz, src, sp - src);
	}
	p = p2 = sp;
	while (p <= ep)
	{
		if (*p == '\\' || *p == '/' || p == ep)
		{
			n = p - sp;
			//n = p - src;
			if (n > 0 && (p[-1] == '\\' || p[-1] == '/'))
			{
				//Nothing
			}
			else if (n > 1 && p[-1] == '.' && (p[-2] == '\\' || p[-2] == '/'))
			{
				//Nothing
			}
			else if (n > 2 && p[-1] == '.' && p[-2] == '.' && (p[-3] == '\\' || p[-3] == '/'))
			{
				p2 = strrchr(dst, '\\');
				if (!p2)
				{
					p++;
					p2 = dst;
				}
				*p2 = 0;
			}
			else
			{
				strncat_s(dst, sz, sp, p - sp);
				n = strlen(dst) - 1;
				if (n >= 0 && dst[n] == '/')
					dst[n] = '\\';
			}
			sp = p;
		}
		p++;
	}
	return dst;
}

//get absolute path
//src のパスから絶対パスを取得
//src が相対パスの場合は、base ファイルを基準とする
//dst=NULL は禁止
//「/」文字は対応していない
PCHAR GetAbsolutePath(PCHAR dst, int sz, PCHAR src, PCHAR base)
{
	char buf[MAX_PATH];
	char *p;

	strcpy_s(buf, sizeof(buf), "");
	p = strchr(src, '\\');
	if (p != src)
	{
		if (p == NULL || p[-1] != ':')
		{
			GetParentPath(buf, sizeof(buf), base);
			strcat_s(buf, sizeof(buf), "\\");
		}
	}
	strcat_s(buf, sizeof(buf), src);
	return GetLinearizedPath(dst, sz, buf);
}

//get related path
//src のパスから相対パスを取得
//src が絶対パスの場合は、base ファイルを基準とする
//ただし「..」は最大 lv 個までとし、それ以上の場合は絶対パスのままとする
//dst=NULL は禁止
//「/」文字は対応していない
PCHAR GetRelatedPath(PCHAR dst, int sz, PCHAR src, PCHAR base, int lv)
{
	char buf[MAX_PATH];
	int i, j, k;
	char *p;

	i = j = 0;
	GetLinearizedPath(buf, sizeof(buf), src);
	while (base[i] && buf[i] && (toupper(base[i]) == toupper(buf[i])))
	{
		if (base[i++] == '\\')
			j = i;
	}
	p = buf + j;
	if (base[i] == '\\' && buf[i] == 0)
	{
		j = i + 1;
		p = buf + i;
	}

	strcpy_s(dst, sz, "");
	if (j)
	{
		i = j;
		k = 0;
		while (base[i])
		{
			if (k > lv)
			{
				p = buf;
				strcpy_s(dst, sz, "");
				break;
			}
			while (base[i] && base[i] != '\\')
				i++;
			if (!base[i])
				break;
			i++;
			if (dst[0])
			{
				strcat_s(dst, sz, "\\");
			}
			strcat_s(dst, sz, "..");
			k++;
		}
	}
	if (dst[0] && p[0])
		strcat_s(dst, sz, "\\");
	strcat_s(dst, sz, p);
	return dst;
}

//get file name
//src のパスから拡張子を除いたファイルの名前を取得
//dst=NULL は禁止
PCHAR GetFileName(PCHAR dst, int sz, PCHAR src)
{
	PCHAR p;
	p = FindFileName(src);
	if (p)
	{
		strcpy_s(dst, sz, p);
		p = strrchr(dst, '.');
		if (p)
		{
			*p = 0;
		}
		return dst;
	}
	return src;
}

//get file extension
//src のパスから拡張子を取得
//dst=NULL は禁止
//「.」は含まない
//無い場合は NULL を返す
//パス区切り文字「\」「/」は認識しない
PCHAR GetFileExt(PCHAR dst, int sz, PCHAR src)
{
	PCHAR p;
	p = FindFileExt(src);
	if (p)
	{
		strcpy_s(dst, sz, ++p);
		return dst;
	}
	return NULL;
}

//remove last slash from path
//path の最後の文字が区切り文字なら最後の文字を削除
//関数は破壊関数
//最後のパス区切り文字は削除
//パス区切り文字「/」は「\」に変換
PCHAR RemovePathSlash(PCHAR path)
{
	PCHAR p;

	p = path;
	while (p = strchr(p, '/'))
	{
		*p = '\\';
	}
	p = path + strlen(path) - 1;
	if (p >= path && (*p == '\\'))
	{
		*p = 0;
	}
	return path;
}

///remove last slash from path
PCHAR RemoveFileName(PCHAR path)
{
	PCHAR p;

	p = FindFileName(path);
	p = (p == path) ? path : (p - 1);
	*p = 0;
	return path;
}

///remove last slash from path
PCHAR RemoveFileExt(PCHAR path)
{
	PCHAR p;

	p = FindFileExt(path);
	if (p && *p)
	{
		*p = 0;
	}
	return path;
}

//combine path
//path に子要素 fn を連結
//path=NULL は禁止
PCHAR CombinePath(PCHAR path, int sz, PCHAR fn)
{
	RemovePathSlash(path);
	strcat_s(path, sz, "\\");
	strcat_s(path, sz, fn);
	return path;
}

//test exist file
//ファイルが存在するか確認
//存在していれは TRUE を返す
BOOL FileExists(PCHAR path)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	hFind = FindFirstFile(path, &fd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	FindClose(hFind);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////

//セクション名の連結文字列取得(開放はfree(outp))
//戻り値は文字列を格納したバッファのサイズ、取得出来ないときは 0
DWORD GetIniSects(strset_t *outp, DWORD sz, DWORD nsz, char *fn)
{
	PCHAR buf;

	buf = (PTCHAR)malloc(sz);
	if (!buf)
		return 0;

	while (sz < GetPrivateProfileSectionNames(buf, sz, fn) + 3)
	{
		free(buf);
		sz += nsz ? nsz : sz;
		buf = (PCHAR)malloc(sz);
		if (!buf)
			return 0;
	}
	if (outp)
	{
		if (*outp)
		{
			free(*outp);
			*outp = NULL;
		}
		*outp = buf;
	}
	else
	{
		free(buf);
	}

	return sz;
}

//セクション内の連結文字列取得(開放はfree(outp))
//戻り値は文字列を格納したバッファのサイズ、取得出来ないときは 0
DWORD GetIniStrSet(char *sect, strset_t *outp, DWORD sz, DWORD nsz, char *fn)
{
	PCHAR buf;

	buf = (PTCHAR)malloc(sz);
	if (!buf)
		return 0;

	while (sz < GetPrivateProfileSection(sect, buf, sz, fn) + 3)
	{
		free(buf);
		sz += nsz ? nsz : sz;
		buf = (PCHAR)malloc(sz);
		if (!buf)
			return 0;
	}
	if (outp)
	{
		if (*outp)
		{
			free(*outp);
			*outp = NULL;
		}
		*outp = buf;
	}
	else
	{
		free(buf);
	}

	return sz;
}
//セクション内のキーワード名の連結文字列取得(開放はfree(outp))
//戻り値は文字列を格納したバッファのサイズ、取得出来ないときは 0
DWORD GetIniKeys(char *sect, strset_t *outp, DWORD sz, DWORD nsz, char *fn)
{
	return GetIniString(sect, NULL, "", outp, sz, nsz, fn);
}

//ON/OFF 設定を設定ファイルから取得
//INIファイル fn の sect セクションに name キーの値が on なら TRUE を返す
//その他は FALSE を返す
//値が無ければ、デフォルト値 bDefault を返す
BOOL GetIniOnOff(char *sect, char *name, BOOL bDefault, char *fn)
{
	char buf[32];

	GetPrivateProfileString(sect, name, "", buf, sizeof(buf), fn);
	return buf[0] ? ((_strnicmp("on", buf, 2) == 0) ? TRUE : FALSE) : bDefault;
}

//数値を設定ファイルから取得
//INIファイル fn の sect セクションに name キーの値を数値として取得
//値が無ければ、デフォルト値 nDefault を返す
UINT GetIniNum(char *sect, char *name, INT nDefault, char *fn)
{
	return GetPrivateProfileInt(sect, name, nDefault, fn);
}

//文字列を設定ファイルから取得
//INIファイル fn の sect セクションに name キーの値を文字列として outp に取得
//値が無ければ、デフォルト値 sDefault を outp に返す
//戻り値は文字列を格納したバッファのサイズ、取得出来ないときは 0
DWORD GetIniString(char *sect, char *name, char *sDefault, PCHAR *outp, DWORD sz, DWORD nsz, char *fn)
{
	PCHAR buf;

	buf = (PTCHAR)malloc(sz);
	if (!buf)
		return 0;

	while (sz < GetPrivateProfileString(sect, name, sDefault, buf, sz, fn) + 3)
	{
		free(buf);
		sz += nsz ? nsz : sz;
		buf = (PCHAR)malloc(sz);
		if (!buf)
			return 0;
	}
	if (outp)
	{
		if (*outp)
		{
			free(*outp);
			*outp = NULL;
		}
		*outp = buf;
	}
	else
	{
		free(buf);
	}

	return sz;
}

//OM/OFF 設定を設定ファイルに書き込む
//INIファイル fn の sect セクションに name キーの値 bFlag を書き込む
//bEnable=TRUE ならば、書き込む値は、bFlag が FALSE 以外なら "on", FALSEなら "off"
//bEnable=FALSE ならば name キーを削除
BOOL WriteIniOnOff(char *sect, char *name, int bFlag, BOOL bEnable, char *fn)
{
	char *p = bEnable ? ((bFlag != FALSE) ? "on" : "off") : NULL;
	return WritePrivateProfileString(sect, name, p, fn);
}

//数値を設定ファイルに書き込む
//INIファイル fn の sect セクションに name キーに数値 val を書き込む
//bEnable=FALSE ならば name キーを削除
BOOL WriteIniNum(char *sect, char *name, int val, BOOL bEnable, char *fn)
{
	char buf[16];
	char *p = NULL;
	if (bEnable || val)
	{
		_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%d", val);
		p = buf;
	}
	return WritePrivateProfileString(sect, name, p, fn);
}

///////////////////////////////////////////////////////////////////////////

//get right-bottom point from window item
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

//move right-bottom point within window item
//ウインドウのサイズ変更時、ウインドウ内のUIコンポーネントの位置・サイズを調整
//uFlag==RB_LEFT の場合、ウインドウの右側とUIコンポーネントの左側の距離を保つ
//uFlag==RB_TOP の場合、ウインドウの下側とUIコンポーネントの上側の距離を保つ
//uFlag==RB_RIGHT の場合、ウインドウの右側とUIコンポーネントの右側の距離を保つ
//uFlag==RB_BOTTOM の場合、ウインドウの下側とUIコンポーネントの下側の距離を保つ
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
	if (uFlag & RB_LEFT)
	{
		x += xx;
		dx -= xx;
	}
	if (uFlag & RB_TOP)
	{
		y += yy;
		dy -= yy;
	}
	if (uFlag & RB_RIGHT)
	{
		dx += xx;
	}
	if (uFlag & RB_BOTTOM)
	{
		dy += yy;
	}
	MoveWindow(hItem, x, y, dx, dy, 1);
}

//get window size to point structure
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

//set window size from point structure
//ウインドウのサイズをpoint型で設定
VOID SetWindowSize(HWND hWnd, POINT *pt)
{
	if (pt->x || pt->y)
	{
		RECT rect;
		LONG w, h;
		GetWindowRect(hWnd, &rect);
		w = (pt->x) ? pt->x : (rect.right - rect.left);
		h = (pt->y) ? pt->y : (rect.bottom - rect.top);
		MoveWindow(hWnd, rect.left, rect.top, w, h, TRUE);
		//SendMessage(hWnd, WM_SIZING, 0, MAKELPARAM(TRUE, 0));
	}
}

//move window relative direction
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
	switch (uPos)
	{
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

//adjust window position to center of parent window
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

//create dialog font and set to phFont (require to delete item after)
//ダイアログにフォントを設定
//仕様が終わったら DeleteObject すること
VOID SetDlgFont(HWND hWnd, UINT uItem, HFONT *phFont, LONG uH, PCHAR szFont)
{
	HWND hItem = GetDlgItem(hWnd, uItem);
	*phFont = CreateFont(
		uH, 0, 0, 0,
		0, 0, 0, 0,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, FF_DONTCARE, (szFont[0] ? szFont : "ＭＳ ゴシック"));
	SendMessage(hItem, WM_SETFONT, (WPARAM)(*phFont), MAKELPARAM(TRUE, 0));
}

//open to file select dialog
//ファイル選択ダイアログを表示させる
//パスは、hWnd ウインドウの editCtrl UI コンポーネントに読み書きする
//hWnd ウインドウが無いか editCtl が -1 の場合は、 UI コンポーネントは使用せず、 szPath を読み込む
//szPath が NULL 以外の場合は szPath にも書き込む(サイズは MAX_PATH 以上であること)
//成功した場合 TRUE を返す
BOOL OpenFileDlg(HWND hWnd, UINT editCtl, PCHAR szTitle, PCHAR szFilter, PCHAR szPath, PCHAR fn, int n)
{
	CHAR szFile[MAX_PATH];
	OPENFILENAME ofn;

	szFile[0] = 0;

	if (hWnd && editCtl != 0xffffffff)
	{
		GetDlgItemText(hWnd, editCtl, szFile, sizeof(szFile));
	}
	else
	{
		strcpy_s(szFile, sizeof(szFile), szPath);
	}

	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrTitle = szTitle;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NODEREFERENCELINKS;

	if (GetOpenFileName(&ofn))
	{
		if (fn)
		{
			GetRelatedPath(szFile, sizeof(szFile), ofn.lpstrFile, fn, n);
		}
		else
		{
			strcpy_s(szFile, sizeof(szFile), ofn.lpstrFile);
		}
		if (hWnd && editCtl != 0xffffffff)
		{
			SetDlgItemText(hWnd, editCtl, szFile);
		}
		if (szPath)
		{
			strcpy_s(szPath, MAX_PATH, szFile);
		}
		return TRUE;
	}
	return FALSE;
}

//open to folder select dialog
//フォルダ選択ダイアログを表示させる
//パスは、hWnd ウインドウの editCtrl UI コンポーネント値に読み書きする
//TODO: メモリ例外になることが無いようにする
static int CALLBACK setDefaultFolder(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
		SendMessage(hwnd, BFFM_SETEXPANDED, (WPARAM)TRUE, lpData);
	}
	return 0;
}

BOOL OpenFolderDlg(HWND hWnd, UINT editCtl, PCHAR szTitle, PCHAR szPath)
{
	BROWSEINFOA bi;
	LPITEMIDLIST pidlRoot;
	LPITEMIDLIST pidlBrowse;
	char buf[MAX_PATH];
	BOOL ret = FALSE;

	if (!SUCCEEDED(SHGetSpecialFolderLocation(hWnd, CSIDL_DESKTOP, &pidlRoot)))
	{
		return FALSE;
	}

	if (hWnd && editCtl != 0xffffffff)
	{
		GetDlgItemText(hWnd, editCtl, buf, sizeof(buf));
	}
	else
	{
		strcpy_s(buf, sizeof(buf), szPath);
	}

	bi.hwndOwner = hWnd;
	bi.pidlRoot = pidlRoot;
	bi.pszDisplayName = buf;
	bi.lpszTitle = szTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_VALIDATE | BIF_NEWDIALOGSTYLE;
	bi.lpfn = setDefaultFolder;
	bi.lParam = (LPARAM)buf;
	pidlBrowse = SHBrowseForFolder(&bi);
	if (pidlBrowse != NULL)
	{
		if (SHGetPathFromIDList(pidlBrowse, szPath))
		{
			if (hWnd && editCtl != 0xffffffff)
			{
				SetDlgItemText(hWnd, editCtl, szPath);
			}
		}
		CoTaskMemFree(pidlBrowse);
	}
	CoTaskMemFree(pidlRoot);

	return ret;
}
