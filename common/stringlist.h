/*
 * string list library
 * (C) 2021 tomo3136a
 */

#include <windows.h>

#ifndef _STRINGLIST_H
#define _STRINGLIST_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* 文字列リスト */
    //todo: TTXReport でしか使わないので後で移動する
    typedef struct _TStringList
    {
        struct _TStringList *nxt;
        size_t len;
        char str[0];
    } TStringList, *PStringList, **PPStringList;

    ///文字列リストクリア
    void ClearStringList(PPStringList p);
    ///文字列リストに文字列を追加
    void AddStringList(PPStringList p, LPSTR s);
    ///ファイルから文字列リストを読み込む(5)
    BOOL LoadStringList(PPStringList p, LPCTSTR path);
    ///ファイルから文字列リストを読み込む(4)
    BOOL info_test_match(PStringList p, PCHAR buff);
    ///ファイルから文字列リストを読み込む(1)
    BOOL info_test_match_head(PStringList p, PCHAR buff);

#ifdef __cplusplus
}
#endif

#endif /* _STRINGLIST_H */
