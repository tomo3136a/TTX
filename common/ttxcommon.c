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

//�R�}���h���C���p�����[�^���
//param ����͂��A�T�C�Y sz �� buf �ɕ���
//�u �v�̓p�����[�^��؂蕶��
//�u;�v�ȍ~�̓R�����g����
//�u"�v����u"�v�܂ł́A������Ƃ��Ĉ���
//�߂�l�́A���������ꍇ NULL �ȊO�ɂȂ�
//buf �� NULL �֎~
//MBS(ShiftJIS)�͑Ή����Ȃ�(�u@[\]^_`{|}~�v�͋�؂蕶���Ɏg�p����݌v���Ȃ�)

PCHAR TTXGetParam(PCHAR buf, int sz, PCHAR param)
{
	int i = 0;
	BOOL quoted = FALSE;

	if (param == NULL)
	{
		return NULL;
	}

	buf[0] = '\0';

	//�X�y�[�X�X�L�b�v�E�I������
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

	//������̍Ō�͕K��NUL����
	buf[i] = '\0';
	return param;
}

//UI����w��
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

//������ P ���ɕ��� c �����������̃|�C���^��Ԃ�
//p �� NULL �܂��͕�����ŏI�̏ꍇ�͂��̂܂ܕԂ�
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

//�A��������
//������(NULL �I�[������)���l�߂ĘA�����Ōゾ�� NULL ��2��A���ɂȂ镶�����A��������Ƃ���
//�󕶎���͊܂߂邱�Ƃ͂ł��Ȃ�

//�A�������񂩂當����̏����؂�o��
//ctx �͌��݂̑����ʒu�ۑ��p�ϐ��̃|�C���^
//p �͘A��������̊J�n�ʒu(�����J�n��))�ANULL �̏ꍇ�� ctx �̒l���g��(2�ڈȍ~�̑��쎞)
//���ׂĐ؂�o������ NULL ��Ԃ�
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

//�A��������̃T�C�Y�ƕ���������擾
//������̌����s�v�̎��� cnt=NULL �Ƃ���
//p �� NULL �͋֎~
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

//�A�������񂩂�L�[���[�h�̘A��������쐬
//�u=�v���Z�p���[�g�����Ƃ��đO�����L�[���[�h�Ƃ���
//dst, src �� NULL �֎~
//dst �̃T�C�Y�͏��Ȃ��Ƃ� src �̃T�C�Y�ȏ�ł��邱��
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

//�A�������񂩂�L�[���[�h�̃C���f�b�N�X�擾
//p, k �� NULL �֎~
//�L�[���[�h��������Ȃ��ꍇ�͕��������Ԃ�
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

//�A�������񂩂�L�[���[�h�Ō�����������擾
//p, k �� NULL �֎~
//�L�[���[�h��������Ȃ��ꍇ�͋󕶎����Ԃ�
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

//�A�������񂩂�l�Ō�����������擾
//�l�́u=�v�����̎��ȍ~�̕�����
//p, v �� NULL �֎~
//�l��������Ȃ��ꍇ�͋󕶎����Ԃ�
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

//�A�������񂩂�n�Ԗڂ̕�������擾
//p �� NULL �֎~
//n ���͈͊O�Ȃ�󕶎����Ԃ�
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

//�����񃊃X�g
//�����������������N���X�g�ŊǗ�����
//������͕����񃊃X�g��malloc�ŕ������Ďg�p����
//������̍ő啶������ 1023 �Ƃ���(���R�͂Ȃ����ꉞ���������Ă���)

//�����񃊃X�g����ɂ���
//p �� NULL �֎~
void ClearStringList(PPStringList p)
{
	PStringList q;
	while (q = *p)
	{
		(*p) = q->nxt;
		free(q);
	}
}

//�����񃊃X�g�̍Ō�ɕ������ǉ�
//p, s �� NULL �֎~
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

//�����񃊃X�g���t�@�C������ǂݍ���
//1�̕�����̓t�@�C����1�s�ɑ�������
//p �� NULL �֎~
//�t�@�C���������ꍇ�� FALSE ��Ԃ�
//������̐擪�E�����̃X�y�[�X�̓g��������
//�A������X�y�[�X��1�����ɏW�񂷂�
//���䕶���͔j������
//�󕶎���͔j������
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
// fileapi.h �͎g��Ȃ��悤�Ȃ̂ő�ւ������A�݊����͂Ȃ�
// �h���C�u���⃋�[�g�p�X�Ȃǂ͐������F���o���Ȃ�����
// ���L�t�H���_�Ȃǂ͐������F���o���Ȃ�����
// MAX_PATH �ȏ�͑ΏۊO
// MBS(ShiftJIS)�͑Ή����Ȃ�(�u@[\]^_`{|}~�v�̕����R�[�h��ASCII�����Ƃ��Ĉ���)

//find address of file name
//src �̃p�X����t�@�C�����̈ʒu���擾
PCHAR FindFileName(PCHAR path)
{
	PCHAR p;
	p = strrchr(path, '\\');
	p = p ? (p + 1) : path;
	return p;
}

//find address of file extension
//path ����t�@�C���g���q�̈ʒu���擾
PCHAR FindFileExt(PCHAR path)
{
	PCHAR p, q;
	p = strrchr(path, '\\');
	p = p ? (p + 1) : path;
	q = strrchr(p, '.');
	return (p == q) ? NULL : q;
}

//find path next component name
//path �̎��̃p�X�Z�O�����g�̈ʒu���擾
//path �̍Ōオ�p�X��؂蕶���̏ꍇ�́A����͖������̂Ƃ��Ĉ���
//path �Ƀp�X��؂蕶���������ꍇ�͑S�̂�ΏۂƂ���
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
//src �̐e�f�B���N�g�����擾���A�T�C�Y sz �� dst �ɕ���
//src �̍Ōオ�p�X��؂蕶���̏ꍇ�́A����͖������̂Ƃ��Ĉ���
//src �Ƀp�X��؂蕶���������ꍇ�͑S�̂�ΏۂƂ���
//dst=NULL �͋֎~
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
//src �̃t�@�C�������擾���A�T�C�Y sz �� dst �ɕ���
//src �̍Ōオ�p�X��؂蕶���̏ꍇ�́A����͖������̂Ƃ��Ĉ���
//src �Ƀp�X��؂蕶���������ꍇ�͑S�̂�ΏۂƂ���
//dst=NULL �͋֎~
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
//src �̃p�X����u.�v��u..�v����菜���Ē��񉻂����p�X�𕡐�
//dst=NULL �͋֎~
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
//src �̃p�X�����΃p�X���擾
//src �����΃p�X�̏ꍇ�́Abase �t�@�C������Ƃ���
//dst=NULL �͋֎~
//�u/�v�����͑Ή����Ă��Ȃ�
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
//src �̃p�X���瑊�΃p�X���擾
//src ����΃p�X�̏ꍇ�́Abase �t�@�C������Ƃ���
//�������u..�v�͍ő� lv �܂łƂ��A����ȏ�̏ꍇ�͐�΃p�X�̂܂܂Ƃ���
//dst=NULL �͋֎~
//�u/�v�����͑Ή����Ă��Ȃ�
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
//src �̃p�X����g���q���������t�@�C���̖��O���擾
//dst=NULL �͋֎~
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
//src �̃p�X����g���q���擾
//dst=NULL �͋֎~
//�u.�v�͊܂܂Ȃ�
//�����ꍇ�� NULL ��Ԃ�
//�p�X��؂蕶���u\�v�u/�v�͔F�����Ȃ�
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
//path �̍Ō�̕�������؂蕶���Ȃ�Ō�̕������폜
//�֐��͔j��֐�
//�Ō�̃p�X��؂蕶���͍폜
//�p�X��؂蕶���u/�v�́u\�v�ɕϊ�
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
//path �Ɏq�v�f fn ��A��
//path=NULL �͋֎~
PCHAR CombinePath(PCHAR path, int sz, PCHAR fn)
{
	RemovePathSlash(path);
	strcat_s(path, sz, "\\");
	strcat_s(path, sz, fn);
	return path;
}

//test exist file
//�t�@�C�������݂��邩�m�F
//���݂��Ă���� TRUE ��Ԃ�
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

//�Z�N�V�������̘A��������擾(�J����free(outp))
//�߂�l�͕�������i�[�����o�b�t�@�̃T�C�Y�A�擾�o���Ȃ��Ƃ��� 0
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

//�Z�N�V�������̘A��������擾(�J����free(outp))
//�߂�l�͕�������i�[�����o�b�t�@�̃T�C�Y�A�擾�o���Ȃ��Ƃ��� 0
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
//�Z�N�V�������̃L�[���[�h���̘A��������擾(�J����free(outp))
//�߂�l�͕�������i�[�����o�b�t�@�̃T�C�Y�A�擾�o���Ȃ��Ƃ��� 0
DWORD GetIniKeys(char *sect, strset_t *outp, DWORD sz, DWORD nsz, char *fn)
{
	return GetIniString(sect, NULL, "", outp, sz, nsz, fn);
}

//ON/OFF �ݒ��ݒ�t�@�C������擾
//INI�t�@�C�� fn �� sect �Z�N�V������ name �L�[�̒l�� on �Ȃ� TRUE ��Ԃ�
//���̑��� FALSE ��Ԃ�
//�l��������΁A�f�t�H���g�l bDefault ��Ԃ�
BOOL GetIniOnOff(char *sect, char *name, BOOL bDefault, char *fn)
{
	char buf[32];

	GetPrivateProfileString(sect, name, "", buf, sizeof(buf), fn);
	return buf[0] ? ((_strnicmp("on", buf, 2) == 0) ? TRUE : FALSE) : bDefault;
}

//���l��ݒ�t�@�C������擾
//INI�t�@�C�� fn �� sect �Z�N�V������ name �L�[�̒l�𐔒l�Ƃ��Ď擾
//�l��������΁A�f�t�H���g�l nDefault ��Ԃ�
UINT GetIniNum(char *sect, char *name, INT nDefault, char *fn)
{
	return GetPrivateProfileInt(sect, name, nDefault, fn);
}

//�������ݒ�t�@�C������擾
//INI�t�@�C�� fn �� sect �Z�N�V������ name �L�[�̒l�𕶎���Ƃ��� outp �Ɏ擾
//�l��������΁A�f�t�H���g�l sDefault �� outp �ɕԂ�
//�߂�l�͕�������i�[�����o�b�t�@�̃T�C�Y�A�擾�o���Ȃ��Ƃ��� 0
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

//OM/OFF �ݒ��ݒ�t�@�C���ɏ�������
//INI�t�@�C�� fn �� sect �Z�N�V������ name �L�[�̒l bFlag ����������
//bEnable=TRUE �Ȃ�΁A�������ޒl�́AbFlag �� FALSE �ȊO�Ȃ� "on", FALSE�Ȃ� "off"
//bEnable=FALSE �Ȃ�� name �L�[���폜
BOOL WriteIniOnOff(char *sect, char *name, int bFlag, BOOL bEnable, char *fn)
{
	char *p = bEnable ? ((bFlag != FALSE) ? "on" : "off") : NULL;
	return WritePrivateProfileString(sect, name, p, fn);
}

//���l��ݒ�t�@�C���ɏ�������
//INI�t�@�C�� fn �� sect �Z�N�V������ name �L�[�ɐ��l val ����������
//bEnable=FALSE �Ȃ�� name �L�[���폜
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
//�E�C���h�E����UI�R���|�[�l���g�̉E���̃|�C���g�� pt �Ɏ擾
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
//�E�C���h�E�̃T�C�Y�ύX���A�E�C���h�E����UI�R���|�[�l���g�̈ʒu�E�T�C�Y�𒲐�
//uFlag==RB_LEFT �̏ꍇ�A�E�C���h�E�̉E����UI�R���|�[�l���g�̍����̋�����ۂ�
//uFlag==RB_TOP �̏ꍇ�A�E�C���h�E�̉�����UI�R���|�[�l���g�̏㑤�̋�����ۂ�
//uFlag==RB_RIGHT �̏ꍇ�A�E�C���h�E�̉E����UI�R���|�[�l���g�̉E���̋�����ۂ�
//uFlag==RB_BOTTOM �̏ꍇ�A�E�C���h�E�̉�����UI�R���|�[�l���g�̉����̋�����ۂ�
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
//�E�C���h�E�̃T�C�Y��point�^�Ŏ擾
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
//�E�C���h�E�̃T�C�Y��point�^�Őݒ�
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
//��ƂȂ�E�C���h�E hWndBase �ɂ������ėאڂ���ʒu�̃E�C���h�E hWnd ���ړ�
//�E�C���h�E�̈ʒu�́A uPos �����Ŏw��
// uPos==0 �̏ꍇ�A�E��
// uPos==1 �̏ꍇ�A����
// uPos==2 �̏ꍇ�A����
// uPos==3 �̏ꍇ�A�㑤
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
//�E�C���h�E��e�E�C���h�E�̒��S�Ɉړ�
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
//�_�C�A���O�Ƀt�H���g��ݒ�
//�d�l���I������� DeleteObject ���邱��
VOID SetDlgFont(HWND hWnd, UINT uItem, HFONT *phFont, LONG uH, PCHAR szFont)
{
	HWND hItem = GetDlgItem(hWnd, uItem);
	*phFont = CreateFont(
		uH, 0, 0, 0,
		0, 0, 0, 0,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, FF_DONTCARE, (szFont[0] ? szFont : "�l�r �S�V�b�N"));
	SendMessage(hItem, WM_SETFONT, (WPARAM)(*phFont), MAKELPARAM(TRUE, 0));
}

//open to file select dialog
//�t�@�C���I���_�C�A���O��\��������
//�p�X�́AhWnd �E�C���h�E�� editCtrl UI �R���|�[�l���g�ɓǂݏ�������
//hWnd �E�C���h�E�������� editCtl �� -1 �̏ꍇ�́A UI �R���|�[�l���g�͎g�p�����A szPath ��ǂݍ���
//szPath �� NULL �ȊO�̏ꍇ�� szPath �ɂ���������(�T�C�Y�� MAX_PATH �ȏ�ł��邱��)
//���������ꍇ TRUE ��Ԃ�
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
//�t�H���_�I���_�C�A���O��\��������
//�p�X�́AhWnd �E�C���h�E�� editCtrl UI �R���|�[�l���g�l�ɓǂݏ�������
//TODO: ��������O�ɂȂ邱�Ƃ������悤�ɂ���
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
