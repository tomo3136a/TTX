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

    /* �����񃊃X�g */
    //todo: TTXReport �ł����g��Ȃ��̂Ō�ňړ�����
    typedef struct _TStringList
    {
        struct _TStringList *nxt;
        size_t len;
        char str[0];
    } TStringList, *PStringList, **PPStringList;

    ///�����񃊃X�g�N���A
    void ClearStringList(PPStringList p);
    ///�����񃊃X�g�ɕ������ǉ�
    void AddStringList(PPStringList p, LPSTR s);
    ///�t�@�C�����當���񃊃X�g��ǂݍ���(5)
    BOOL LoadStringList(PPStringList p, LPCTSTR path);
    ///�t�@�C�����當���񃊃X�g��ǂݍ���(4)
    BOOL info_test_match(PStringList p, PCHAR buff);
    ///�t�@�C�����當���񃊃X�g��ǂݍ���(1)
    BOOL info_test_match_head(PStringList p, PCHAR buff);

#ifdef __cplusplus
}
#endif

#endif /* _STRINGLIST_H */
