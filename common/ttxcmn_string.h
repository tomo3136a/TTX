/*
 * TTX common utility library
 * (C) 2022 tomo3136a
 */

#ifndef _TTXCMN_UTIL_H
#define _TTXCMN_UTIL_H

#include <windows.h>
#include <tchar.h>

#ifdef __cplusplus
extern "C" {
#endif

///文字列中に文字を検索し次のポインタを返す
LPTSTR strskip(LPTSTR p, TCHAR c);

/* string set */
///連結文字列定義
typedef LPTSTR strset_t;

///連結文字列から順次切り出す
LPTSTR StrSetTok(strset_t p, strset_t *ctx);

///連結文字列のサイズを取得する
int StrSetSize(strset_t p, int *cnt);

///連結文字列からキーワードの連結文字列作成
int StrSetKeys(strset_t dst, strset_t src);

///連結文字列からキーワードのインデックス取得
int StrSetFindIndex(strset_t p, LPTSTR k);

///連結文字列からキーワードで検索し文字列取得
LPTSTR StrSetFindKey(strset_t p, LPTSTR k);

///連結文字列から値で検索し文字列取得
LPTSTR StrSetFindVal(strset_t p, LPTSTR v);

///連結文字列からn番目の文字列を取得する
LPTSTR StrSetAt(strset_t p, int n);

#ifdef __cplusplus
}
#endif

#endif /* _TTXCMN_UTIL_H */
