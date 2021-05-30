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
    ///文字列中に文字を検索し次のポインタを返す
    char *strskip(char *p, char c);

    /* string set */
    ///連結文字列定義
    typedef char *strset_t;

    ///連結文字列から順次切り出す
    char *StrSetTok(strset_t p, strset_t *ctx);

    ///連結文字列のサイズを取得する
    int StrSetSize(strset_t p, int *cnt);

    ///連結文字列からキーワードの連結文字列作成
    int StrSetKeys(strset_t dst, strset_t src);

    ///連結文字列からキーワードのインデックス取得
    int StrSetFindIndex(strset_t p, char *k);

    ///連結文字列からキーワードで検索し文字列取得
    char *StrSetFindKey(strset_t p, char *k);

    ///連結文字列から値で検索し文字列取得
    char *StrSetFindVal(strset_t p, char *v);

    ///連結文字列からn番目の文字列を取得する
    char *StrSetAt(strset_t p, int n);

    /* 文字列リスト */
    //todo: TTXReport でしか使わないので後で移動する
    typedef struct _TStringList
    {
        struct _TStringList *nxt;
        int len;
        char str[0];
    } TStringList, *PStringList, **PPStringList;

    ///文字列リストクリア
    void ClearStringList(PPStringList p);
    ///文字列リストに文字列を追加
    void AddStringList(PPStringList p, PCHAR s);
    ///ファイルから文字列リストを読み込む(5)
    BOOL LoadStringList(PPStringList p, PCHAR path);
    ///ファイルから文字列リストを読み込む(4)
    BOOL info_test_match(PStringList p, PCHAR buff);
    ///ファイルから文字列リストを読み込む(1)
    BOOL info_test_match_head(PStringList p, PCHAR buff);

    /* path */
    // fileapi.h は使わないようなので代替え実装、互換性はない

    /* find 型 */
    ///find file name address
    PCHAR FindFileName(PCHAR path);

    ///find file extension address
    PCHAR FindFileExt(PCHAR path);

    ///find path component path address
    PCHAR FindPathNextComponent(PCHAR path);

    /* build 型(src to dst) */
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

    /* replase 型 */
    ///remove last slash from path
    PCHAR RemovePathSlash(PCHAR path);

    ///remove last slash from path
    PCHAR RemoveFileName(PCHAR path);

    ///remove last slash from path
    PCHAR RemoveFileExt(PCHAR path);

    ///combine path
    PCHAR CombinePath(PCHAR path, int sz, PCHAR fn);

    /* test 型 */
    ///test exist file
    BOOL FileExists(PCHAR path);

    /* setting file */
    ///セクション名の連結文字列取得(開放はfree(outp))
    DWORD GetIniSects(strset_t *outp, DWORD sz, DWORD nsz, char *fn);

    ///セクション内のキーワード名/値の連結文字列取得(開放はfree(outp))
    DWORD GetIniStrSet(char *sect, strset_t *outp, DWORD sz, DWORD nsz, char *fn);

    ///セクション内のキーワード名の連結文字列取得(開放はfree(outp))
    DWORD GetIniKeys(char *sect, strset_t *outp, DWORD sz, DWORD nsz, char *fn);

    ///ON/OFF 設定を設定ファイルから取得
    BOOL GetIniOnOff(char *sect, char *name, BOOL bDefault, char *fn);

    ///数値設定を設定ファイルから取得
    UINT GetIniNum(char *sect, char *name, int nDefault, char *fn);

    ///文字列を設定ファイルから取得(開放はfree(outp))
    DWORD GetIniString(char *sect, char *name, char *sDefault,
                       PCHAR *outp, DWORD sz, DWORD nsz, char *fn);

    ///OM/OFF 設定を設定ファイルに書き込む
    BOOL WriteIniOnOff(char *sect, char *name, int bFlag, BOOL bEnable, char *fn);

    ///数値設定を設定ファイルに書き込む
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
