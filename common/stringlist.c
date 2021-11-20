/*
 * string list library
 * (C) 2021 tomo3136a
 */

// #include "teraterm.h"
// #include "tttypes.h"
// #include "ttplugin.h"
// #include <windows.h>
// #include <tchar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <shlobj.h>
// #include <time.h>

#include "stringlist.h"

///////////////////////////////////////////////////////////////////////////

//文字列リスト
//文字列を方方向リンクリストで管理する
//文字列は文字列リストにmallocで複製して使用する
//文字列の最大文字数は 1023 とする(理由はないが一応制限を入れておく)

//文字列リストを空にする
// p は NULL 禁止
void ClearStringList(PPStringList p)
{
	PStringList q;
	while (q = *p) {
		(*p) = q->nxt;
		free(q);
	}
}

//文字列リストの最後に文字列を追加
// p, s は NULL 禁止
void AddStringList(PPStringList p, PCHAR s)
{
	while (*p) p = &((*p)->nxt);
	int sz = strnlen_s(s, 1024 - 1 - sizeof(TStringList));
	*p = (PStringList)malloc(sizeof(TStringList) + sz + 1);
	(*p)->len = sz;
	(*p)->nxt = NULL;
	strcpy_s((*p)->str, 1024, s);
}

//文字列リストをファイルから読み込む
// 1つの文字列はファイルの1行に相当する
// p は NULL 禁止
//ファイルが無い場合は FALSE を返す
//文字列の先頭・末尾のスペースはトリムする
//連続するスペースは1文字に集約する
//制御文字は破棄する
//空文字列は破棄する
BOOL LoadStringList(PPStringList p, LPCTSTR path)
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
	while (ReadFile(hFile, tmp, sizeof(tmp) / sizeof(tmp[0]), &dwSize, NULL)) {
		if (dwSize == 0)
			break;
		for (i = 0; i < dwSize; i++) {
			ch = tmp[i];
			if (ch < 0 || ch > 0x20) {
				buf[blen++] = ch;
				if (blen >= STRINGLISTBUFSZ)
					blen = 0;
				continue;
			}
			if (ch == 0x20) {
				if (blen && buf[blen - 1] != 0x20) {
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
	while (p) {
		if (lstrcmpiA(p->str, buf) == 0)
			return TRUE;
		p = p->nxt;
	}
	return FALSE;
}

BOOL info_test_match_head(PStringList p, PCHAR buf)
{
	while (p) {
		if (_strnicmp(p->str, buf, p->len) == 0)
			return TRUE;
		p = p->nxt;
	}
	return FALSE;
}
