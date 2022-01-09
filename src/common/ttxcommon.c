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

//�R�}���h���C���p�����[�^���
// param ����͂��A�T�C�Y sz �� buf �ɕ���
//�u �v�̓p�����[�^��؂蕶��
//�u;�v�ȍ~�̓R�����g����
//�u"�v����u"�v�܂ł́A������Ƃ��Ĉ���
//�߂�l�́A���������ꍇ NULL �ȊO�ɂȂ�
// buf �� NULL �֎~
// MBS(ShiftJIS)�͑Ή����Ȃ�(�u@[\]^_`{|}~�v�͋�؂蕶���Ɏg�p����݌v���Ȃ�)

LPTSTR TTXGetParam(LPTSTR buf, size_t sz, LPTSTR param)
{
	size_t i = 0;
	BOOL quoted = FALSE;

	if (param == NULL) {
		return NULL;
	}

	buf[0] = _T('\0');

	//�X�y�[�X�X�L�b�v�E�I������
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

	//������̍Ō�͕K��NUL����
	buf[i] = _T('\0');
	return param;
}

// UI����w��
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
LPTSTR CombinePath(LPTSTR path, int sz, LPCTSTR fn)
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

///////////////////////////////////////////////////////////////////////////

//�Z�N�V�������̘A��������擾(�J����free(outp))
//�߂�l�͕�������i�[�����o�b�t�@�̃T�C�Y�A�擾�o���Ȃ��Ƃ��� 0
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

//�Z�N�V�������̘A��������擾(�J����free(outp))
//�߂�l�͕�������i�[�����o�b�t�@�̃T�C�Y�A�擾�o���Ȃ��Ƃ��� 0
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
//�Z�N�V�������̃L�[���[�h���̘A��������擾(�J����free(outp))
//�߂�l�͕�������i�[�����o�b�t�@�̃T�C�Y�A�擾�o���Ȃ��Ƃ��� 0
DWORD GetIniKeys(LPCTSTR sect, strset_t *outp, DWORD sz, DWORD nsz, LPCTSTR fn)
{
	return GetIniString(sect, NULL, _T(""), outp, sz, nsz, fn);
}

// ON/OFF �ݒ��ݒ�t�@�C������擾
// INI�t�@�C�� fn �� sect �Z�N�V������ name �L�[�̒l�� on �Ȃ� TRUE ��Ԃ�
//���̑��� FALSE ��Ԃ�
//�l��������΁A�f�t�H���g�l bDefault ��Ԃ�
BOOL GetIniOnOff(LPCTSTR sect, LPCTSTR name, BOOL bDefault, LPCTSTR fn)
{
	TCHAR buf[32];

	GetPrivateProfileString(sect, name, _T(""), buf, sizeof(buf) / sizeof(buf[0]), fn);
	return buf[0] ? ((_tcsnicmp(_T("on"), buf, 2) == 0) ? TRUE : FALSE) : bDefault;
}

//���l��ݒ�t�@�C������擾
// INI�t�@�C�� fn �� sect �Z�N�V������ name �L�[�̒l�𐔒l�Ƃ��Ď擾
//�l��������΁A�f�t�H���g�l nDefault ��Ԃ�
UINT GetIniNum(LPCTSTR sect, LPCTSTR name, INT nDefault, LPCTSTR fn)
{
	return GetPrivateProfileInt(sect, name, nDefault, fn);
}

//�������ݒ�t�@�C������擾
// INI�t�@�C�� fn �� sect �Z�N�V������ name �L�[�̒l�𕶎���Ƃ��� outp �Ɏ擾
//�l��������΁A�f�t�H���g�l sDefault �� outp �ɕԂ�
//�߂�l�͕�������i�[�����o�b�t�@�̃T�C�Y�A�擾�o���Ȃ��Ƃ��� 0
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

// OM/OFF �ݒ��ݒ�t�@�C���ɏ�������
// INI�t�@�C�� fn �� sect �Z�N�V������ name �L�[�̒l bFlag ����������
// bEnable=TRUE �Ȃ�΁A�������ޒl�́AbFlag �� FALSE �ȊO�Ȃ� "on", FALSE�Ȃ� "off"
// bEnable=FALSE �Ȃ�� name �L�[���폜
BOOL WriteIniOnOff(LPCTSTR sect, LPCTSTR name, int bFlag, BOOL bEnable, LPCTSTR fn)
{
	LPTSTR p = bEnable ? ((bFlag != FALSE) ? _T("on") : _T("off")) : NULL;
	return WritePrivateProfileString(sect, name, p, fn);
}

//���l��ݒ�t�@�C���ɏ�������
// INI�t�@�C�� fn �� sect �Z�N�V������ name �L�[�ɐ��l val ����������
// bEnable=FALSE �Ȃ�� name �L�[���폜
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

// move right-bottom point within window item
//�E�C���h�E�̃T�C�Y�ύX���A�E�C���h�E����UI�R���|�[�l���g�̈ʒu�E�T�C�Y�𒲐�
// uFlag==RB_LEFT �̏ꍇ�A�E�C���h�E�̉E����UI�R���|�[�l���g�̍����̋�����ۂ�
// uFlag==RB_TOP �̏ꍇ�A�E�C���h�E�̉�����UI�R���|�[�l���g�̏㑤�̋�����ۂ�
// uFlag==RB_RIGHT �̏ꍇ�A�E�C���h�E�̉E����UI�R���|�[�l���g�̉E���̋�����ۂ�
// uFlag==RB_BOTTOM �̏ꍇ�A�E�C���h�E�̉�����UI�R���|�[�l���g�̉����̋�����ۂ�
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

// set window size from point structure
//�E�C���h�E�̃T�C�Y��point�^�Őݒ�
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

// create dialog font and set to phFont (require to delete item after)
//�_�C�A���O�Ƀt�H���g��ݒ�
//�d�l���I������� DeleteObject ���邱��
VOID SetDlgFont(HWND hWnd, UINT uItem, HFONT *phFont, LONG uH, LPTSTR szFont)
{
	HWND hItem = GetDlgItem(hWnd, uItem);
	*phFont = CreateFont(uH, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						 DEFAULT_QUALITY, FF_DONTCARE, (szFont[0] ? szFont : _T("�l�r �S�V�b�N")));
	SendMessage(hItem, WM_SETFONT, (WPARAM)(*phFont), MAKELPARAM(TRUE, 0));
}

// open to file select dialog
//�t�@�C���I���_�C�A���O��\��������
//�p�X�́AhWnd �E�C���h�E�� editCtrl UI �R���|�[�l���g�ɓǂݏ�������
// hWnd �E�C���h�E�������� editCtl �� -1 �̏ꍇ�́A UI �R���|�[�l���g�͎g�p�����A szPath ��ǂݍ���
// szPath �� NULL �ȊO�̏ꍇ�� szPath �ɂ���������(�T�C�Y�� MAX_PATH �ȏ�ł��邱��)
//���������ꍇ TRUE ��Ԃ�
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
//�t�H���_�I���_�C�A���O��\��������
//�p�X�́AhWnd �E�C���h�E�� editCtrl UI �R���|�[�l���g�l�ɓǂݏ�������
// TODO: ��������O�ɂȂ邱�Ƃ������悤�ɂ���
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
