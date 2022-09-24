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

#ifdef __cplusplus
}
#endif

#endif /* _TTXCMN_UTIL_H */
