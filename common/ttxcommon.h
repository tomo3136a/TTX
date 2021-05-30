/*
 * TTX common library
 * (C) 2021 tomo3136a
 */

#include <windows.h>

/* auto resize for dialog compornent by window resize */
#define RB_LEFT 0x0001
#define RB_TOP 0x0002
#define RB_RIGHT 0x0004
#define RB_BOTTOM 0x0008

#define TTXID "C11H17N3O8"

#ifdef __cplusplus
extern "C"
{
#endif

    /* ttx support */
    ///TTX load test
    BOOL TTXIgnore(int order, PCHAR name, WORD version);

    ///token command line parameter
    PCHAR TTXGetParam(PCHAR buf, int sz, PCHAR param);

    ///get UI language ID(1=English, 2=Japanese)
    UINT UILang(PCHAR lang);

    ///get Menu ID offset
    UINT TTXMenuID(UINT uid);
    UINT TTXMenuOrgID(UINT uid);

    /* string */
    ///�����񒆂ɕ��������������̃|�C���^��Ԃ�
    char *strskip(char *p, char c);

    /* string set */
    ///�A���������`
    typedef char *strset_t;

    ///�A�������񂩂珇���؂�o��
    char *StrSetTok(strset_t p, strset_t *ctx);

    ///�A��������̃T�C�Y���擾����
    int StrSetSize(strset_t p, int *cnt);

    ///�A�������񂩂�L�[���[�h�̘A��������쐬
    int StrSetKeys(strset_t dst, strset_t src);

    ///�A�������񂩂�L�[���[�h�̃C���f�b�N�X�擾
    int StrSetFindIndex(strset_t p, char *k);

    ///�A�������񂩂�L�[���[�h�Ō�����������擾
    char *StrSetFindKey(strset_t p, char *k);

    ///�A�������񂩂�l�Ō�����������擾
    char *StrSetFindVal(strset_t p, char *v);

    ///�A�������񂩂�n�Ԗڂ̕�������擾����
    char *StrSetAt(strset_t p, int n);

    /* �����񃊃X�g */
    //todo: TTXReport �ł����g��Ȃ��̂Ō�ňړ�����
    typedef struct _TStringList
    {
        struct _TStringList *nxt;
        int len;
        char str[0];
    } TStringList, *PStringList, **PPStringList;

    ///�����񃊃X�g�N���A
    void ClearStringList(PPStringList p);
    ///�����񃊃X�g�ɕ������ǉ�
    void AddStringList(PPStringList p, PCHAR s);
    ///�t�@�C�����當���񃊃X�g��ǂݍ���(5)
    BOOL LoadStringList(PPStringList p, PCHAR path);
    ///�t�@�C�����當���񃊃X�g��ǂݍ���(4)
    BOOL info_test_match(PStringList p, PCHAR buff);
    ///�t�@�C�����當���񃊃X�g��ǂݍ���(1)
    BOOL info_test_match_head(PStringList p, PCHAR buff);

    /* path */
    // fileapi.h �͎g��Ȃ��悤�Ȃ̂ő�ւ������A�݊����͂Ȃ�

    /* find �^ */
    ///find file name address
    PCHAR FindFileName(PCHAR path);

    ///find file extension address
    PCHAR FindFileExt(PCHAR path);

    ///find path component path address
    PCHAR FindPathNextComponent(PCHAR path);

    /* build �^(src to dst) */
    ///get parent path
    PCHAR GetParentPath(PCHAR dst, int sz, PCHAR src);

    ///get path item name
    PCHAR GetPathName(PCHAR dst, int sz, PCHAR src);

    ///get linearized path
    PCHAR GetLinearizedPath(PCHAR dst, int sz, PCHAR src);

    ///get absolute path
    PCHAR GetAbsolutePath(PCHAR dst, int sz, PCHAR src, PCHAR base);

    ///get related path
    PCHAR GetRelatedPath(PCHAR dst, int sz, PCHAR src, PCHAR base, int lv);

    /* replase �^ */
    ///remove last slash from path
    PCHAR RemovePathSlash(PCHAR path);

    ///remove last slash from path
    PCHAR RemoveFileName(PCHAR path);

    ///remove last slash from path
    PCHAR RemoveFileExt(PCHAR path);

    ///combine path
    PCHAR CombinePath(PCHAR path, int sz, PCHAR fn);

    /* test �^ */
    ///test exist file
    BOOL FileExists(PCHAR path);

    /* setting file */
    ///�Z�N�V�������̘A��������擾(�J����free(outp))
    DWORD GetIniSects(strset_t *outp, DWORD sz, DWORD nsz, char *fn);

    ///�Z�N�V�������̃L�[���[�h��/�l�̘A��������擾(�J����free(outp))
    DWORD GetIniStrSet(char *sect, strset_t *outp, DWORD sz, DWORD nsz, char *fn);

    ///�Z�N�V�������̃L�[���[�h���̘A��������擾(�J����free(outp))
    DWORD GetIniKeys(char *sect, strset_t *outp, DWORD sz, DWORD nsz, char *fn);

    ///ON/OFF �ݒ��ݒ�t�@�C������擾
    BOOL GetIniOnOff(char *sect, char *name, BOOL bDefault, char *fn);

    ///���l�ݒ��ݒ�t�@�C������擾
    UINT GetIniNum(char *sect, char *name, int nDefault, char *fn);

    ///�������ݒ�t�@�C������擾(�J����free(outp))
    DWORD GetIniString(char *sect, char *name, char *sDefault,
                       PCHAR *outp, DWORD sz, DWORD nsz, char *fn);

    ///OM/OFF �ݒ��ݒ�t�@�C���ɏ�������
    BOOL WriteIniOnOff(char *sect, char *name, int bFlag, BOOL bEnable, char *fn);

    ///���l�ݒ��ݒ�t�@�C���ɏ�������
    BOOL WriteIniNum(char *sect, char *name, int val, BOOL bEnable, char *fn);

    /* window control */
    ///get right-bottom point from window item
    VOID GetPointRB(HWND hWnd, UINT uItem, POINT *pt);

    ///move right-bottom point within window item
    VOID MovePointRB(HWND hWnd, UINT uItem, POINT *ptRB, UINT uFlag);

    ///get window size to point structure
    VOID GetWindowSize(HWND hWnd, POINT *pt);

    ///set window size from point structure
    VOID SetWindowSize(HWND hWnd, POINT *pt);

    ///move window relative direction
    VOID SetHomePosition(HWND hWnd, HWND hWndBase, UINT uPos);

    ///adjust window position to center of parent window
    VOID MoveParentCenter(HWND hWnd);

    ///create dialog font and set to phFont (require to delete item after)
    VOID SetDlgFont(HWND hWnd, UINT uItem, HFONT *phFont, LONG uH, PCHAR szFont);

    /* dialog */
    ///open to file select dialog
    BOOL OpenFileDlg(HWND hWnd, UINT editCtl, PCHAR szTitle,
                     PCHAR szFilter, PCHAR szPath, PCHAR fn, int n);

    ///open to folder select dialog
    BOOL OpenFolderDlg(HWND hWnd, UINT editCtl, PCHAR szTitle, PCHAR szPath);

#ifdef __cplusplus
}
#endif
