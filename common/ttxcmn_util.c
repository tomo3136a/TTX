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

//������ P ���ɕ��� c �����������̃|�C���^��Ԃ�
// p �� NULL �܂��͕�����ŏI�̏ꍇ�͂��̂܂ܕԂ�
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
//�A��������
//������(NULL �I�[������)���l�߂ĘA�����Ōゾ�� NULL ��2��A���ɂȂ镶�����A��������Ƃ���
//�󕶎���͊܂߂邱�Ƃ͂ł��Ȃ�

//�A�������񂩂當����̏����؂�o��
// ctx �͌��݂̑����ʒu�ۑ��p�ϐ��̃|�C���^
// p �͘A��������̊J�n�ʒu(�����J�n��))�ANULL �̏ꍇ�� ctx �̒l���g��(2�ڈȍ~�̑��쎞)
//���ׂĐ؂�o������ NULL ��Ԃ�
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

//�A��������̃T�C�Y�ƕ���������擾
//������̌����s�v�̎��� cnt=NULL �Ƃ���
// p �� NULL �͋֎~
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

//�A�������񂩂�L�[���[�h�̘A��������쐬
//�u=�v���Z�p���[�g�����Ƃ��đO�����L�[���[�h�Ƃ���
// dst, src �� NULL �֎~
// dst �̃T�C�Y�͏��Ȃ��Ƃ� src �̃T�C�Y�ȏ�ł��邱��
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

//�A�������񂩂�L�[���[�h�̃C���f�b�N�X�擾
// p, k �� NULL �֎~
//�L�[���[�h��������Ȃ��ꍇ�͕��������Ԃ�
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

//�A�������񂩂�L�[���[�h�Ō�����������擾
// p, k �� NULL �֎~
//�L�[���[�h��������Ȃ��ꍇ�͋󕶎����Ԃ�
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

//�A�������񂩂�l�Ō�����������擾
//�l�́u=�v�����̎��ȍ~�̕�����
// p, v �� NULL �֎~
//�l��������Ȃ��ꍇ�͋󕶎����Ԃ�
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

//�A�������񂩂�n�Ԗڂ̕�������擾
// p �� NULL �֎~
// n ���͈͊O�Ȃ�󕶎����Ԃ�
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
// fileapi.h �͎g��Ȃ��悤�Ȃ̂ő�ւ������A�݊����͂Ȃ�
// �h���C�u���⃋�[�g�p�X�Ȃǂ͐������F���o���Ȃ�����
// ���L�t�H���_�Ȃǂ͐������F���o���Ȃ�����
// MAX_PATH �ȏ�͑ΏۊO
// MBS(ShiftJIS)�͑Ή����Ȃ�(�u@[\]^_`{|}~�v�̕����R�[�h��ASCII�����Ƃ��Ĉ���)

// find address of file name
// src �̃p�X����t�@�C�����̈ʒu���擾
LPTSTR FindFileName(LPCTSTR path)
{
	LPTSTR p;
	p = _tcsrchr(path, _T('\\'));
	p = p ? (p + 1) : path;
	return p;
}

// find address of file extension
// path ����t�@�C���g���q�̈ʒu���擾
LPTSTR FindFileExt(LPCTSTR path)
{
	LPTSTR p, q;
	p = _tcsrchr(path, _T('\\'));
	p = p ? (p + 1) : path;
	q = _tcsrchr(p, _T('.'));
	return (p == q) ? NULL : q;
}

// find path next component name
// path �̎��̃p�X�Z�O�����g�̈ʒu���擾
// path �̍Ōオ�p�X��؂蕶���̏ꍇ�́A����͖������̂Ƃ��Ĉ���
// path �Ƀp�X��؂蕶���������ꍇ�͑S�̂�ΏۂƂ���
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
// src �̐e�f�B���N�g�����擾���A�T�C�Y dst_sz �� dst �ɕ���
// src �̍Ōオ�p�X��؂蕶���̏ꍇ�́A����͖������̂Ƃ��Ĉ���
// src �Ƀp�X��؂蕶���������ꍇ�͑S�̂�ΏۂƂ���
// dst=NULL �͋֎~
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
// src �̃t�@�C�������擾���A�T�C�Y dst_sz �� dst �ɕ���
// src �̍Ōオ�p�X��؂蕶���̏ꍇ�́A����͖������̂Ƃ��Ĉ���
// src �Ƀp�X��؂蕶���������ꍇ�͑S�̂�ΏۂƂ���
// dst=NULL �͋֎~
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
// src �̃p�X����u.�v��u..�v����菜���Ē��񉻂����p�X�𕡐�
// dst=NULL �͋֎~
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
// src �̃p�X�����΃p�X���擾
// src �����΃p�X�̏ꍇ�́Abase �t�@�C������Ƃ���
// dst=NULL �͋֎~
//�u/�v�����͑Ή����Ă��Ȃ�
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
// src �̃p�X���瑊�΃p�X���擾
// src ����΃p�X�̏ꍇ�́Abase �t�@�C������Ƃ���
//�������u..�v�͍ő� lv �܂łƂ��A����ȏ�̏ꍇ�͐�΃p�X�̂܂܂Ƃ���
// dst=NULL �͋֎~
//�u/�v�����͑Ή����Ă��Ȃ�
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
// src �̃p�X����g���q���������t�@�C���̖��O���擾
// dst=NULL �͋֎~
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
// src �̃p�X����g���q���擾
// dst=NULL �͋֎~
//�u.�v�͊܂܂Ȃ�
//�����ꍇ�� NULL ��Ԃ�
//�p�X��؂蕶���u\�v�u/�v�͔F�����Ȃ�
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
// path �̍Ō�̕�������؂蕶���Ȃ�Ō�̕������폜
//�֐��͔j��֐�
//�Ō�̃p�X��؂蕶���͍폜
//�p�X��؂蕶���u/�v�́u\�v�ɕϊ�
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
// path �Ɏq�v�f fn ��A��
// path=NULL �͋֎~
LPTSTR CombinePath(LPTSTR path, size_t sz, LPCTSTR fn)
{
	RemovePathSlash(path);
	_tcscat_s(path, sz, _T("\\"));
	_tcscat_s(path, sz, fn);
	return path;
}

// test exist file
//�t�@�C�������݂��邩�m�F
//���݂��Ă���� TRUE ��Ԃ�
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
