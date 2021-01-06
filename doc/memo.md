# 開発メモ

## TTX とは

TTX は、 Tera Term eXtension の略称。
構造を保持しているため、古いソースコードでビルドしたプラグインファイルはそのまま使えるらしい。時々、呼び出し規約が変わっており、古すぎてもだめらしい。

Tera Term 4.70～4.105 くらいまでは個別のバグはあるものの、動作させることができる。それでも最新版でビルドすれば動作が望める。

## C11H17N3O8.txt とは

TTX を制御するための設定ファイル。
***C11H17N3O8*** は、ふぐ毒で知られる テトロドトキシン (tetrodotoxin, TTX) の化学式。神経毒で


## コードの整形

可能な限りきれいな状態を保つ。

ソースファイルの名称は、基本的に小文字に統一する。

エディタに Visual Studio Code を使用してるので、とりあえず、 Shift ＋ Alt+F で整形する。

## コードの共通化

共通コードは、common/ttxcommon.h および common/ttxcommon.c に纏める。ただし、Ｔera Ｔerm ソースコードに依存せず、単体でコンパイル可能なソースコードとする。各プラグインソースコードにおいては、Include により使用可能にする。

```c
#include "compat_w95.h"
#include "ttxcommon.h"              //<== 追加
#include "resource.h"
```

## UI 画面の国際化

国際化は難しいので当面はシンプルにコード内実装とする。
UILangageFile の選択が日本語(=2)とその他(=1)とし、その他の場合は英語表示とする。

```c
    UINT lang;
    LPSTR s;

    lang = UILang(pvar->ts->UILanguageFile);

    s = (lang == 2) ? "日本語(&J)..." : "&Japanese...";
    AppendMenu(pvar->SetupMenu, flag, ID_MENUITEM, s);
```

## プラグイン ON/OFF 対応

プラグインを有効・無効を制御したいプラグインには、TTXBind 関数内に ***TTXIgnore()*** 関数を追加する。

```c
BOOL __declspec(dllexport) PASCAL TTXBind(WORD Version, TTXExports *exports)
{
    int size = sizeof(Exports) - sizeof(exports->size);
    /* do version checking if necessary */
    /* if (Version!=TTVERSION) return FALSE; */

    //{when TTXPlugin support}              //<== 追加
    if (TTXIgnore(ORDER, INISECTION, 0))    //<== 追加
        return TRUE;                        //<== 追加

```

## メニューアイテム番号の重複対応

メニューアイテムの番号は、56000 以降に割り当てる。
各プラグイン同士で重複しないように気を付ける。重なってしまったときに対応できるように、メニューアイテム番号オフセットに対応させる。

***pvar->menuoffset*** の設定を追加する。また、メニューアイテムの番号(ID_MENUITEM や ID_MENUITEM1 等)の箇所に " ***+ pvar->menuoffset*** " を追加する。

```c
typedef struct
{
    PTTSet ts;
    PComVar cv;
    BOOL skip;
    int menuoffset;         // <==追加
} TInstVar;
...

static void PASCAL TTXModifyMenu(HMENU menu)
{
    HMENU submenu;
    UINT menu_id;

    pvar->menuoffset = MenuOffset(INISECTION, ID_MENUITEM, 0);  // <==追加

    submenu = GetSubMenu(menu, 0);
    menu_id = ID_MENUITEM + pvar->menuoffset;                   // <==オフセットを加算
    AppendMenu(submenu, MF_ENABLED, menu_id, "item");
    ...
}
...

static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
    //同様にオフセットを加算
}
...

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
    UINT menu_id;

    menu_id  = cmd + pvar->menuoffset;          // <==オフセットを加算
    switch (menu_id)
    {
    case ID_MENUITEM:
        //menu pocedure...
        break;
    case ID_MENUITEM1:
        //menu pocedure...
        break;
    ...
    }
    return 0;
}
```

## メモリの動的割り当て

基本的には、関数内でメモリの割り当て、開放を行う。
***buf*** および ***buf_sz*** は、下記以外で値を変更しない。( ***buf*** の箇所は任意の名前)

最初にメモリ領域を定義する。

```c
char *buf;
int buf_sz;

buf = NULL;
buf_sz = 4100;      // <== 例： 4096バイト + 4バイト
```

メモリ領域を確保する。

```c
if (buf)
{
    free(buf);
    buf = NULL;
}
buf = malloc(buf_sz);
if (!buf)
{
    //メモリを確保できなかった場合、既に確保したメモリを解放
    return -1;
}
```

メモリ領域の使用が終わったらメモリを開放する。

```c
if (buf)
{
    free(buf);
    buf = NULL;
}
```

## 文字列の動的割り当て

基本的には、メモリの割り当てと同じ。基本的には、関数内でメモリの割り当て、開放を行う。
***buf*** および ***buf_sz*** は、下記以外で値を変更しない。( ***buf*** の箇所は任意の名前)

最初にメモリ領域を定義する。

```c
PCHAR buf;
int buf_sz;
PCHAR p;

buf = NULL;
buf_sz = 0;
```

文字列を複製する。

```c
if (buf)
{
    free(buf);
    buf = NULL;
}
buf_sz = strlen(src) + 2;
buf = malloc(buf_sz * sizeof(CHAR));
if (!buf)
{
    //メモリを確保できなかった場合、既に確保したメモリを解放
    return -1;
}
memcpy(buf, buf_sz, src);
```

文字列を追加する。

```c
buf_sz += strlen(src);
p = malloc(buf_sz);
if (!p)
{
    if (buf)
        fee(buf);
    //メモリを確保できなかった場合、既に確保したメモリを解放
    return -1;
}
strcpy_s(p, buf_sz, buf);
strcat_s(p, buf_sz, src);
free(buf);
buf = p;
```

文字列の使用が終わったらメモリを開放する。

```c
if (buf)
{
    free(buf);
    buf = NULL;
}
```

## ビルド環境

複数のビルド環境を管理するのは出機能もないので、 cmake だけにする。
ビルドの使う環境の Visual Studio の最新版を使用する。

ただし、インストーラは cmake から呼び出せないので Visual Studio を開いてビルドする。

## プラグインバージョン

全体の CMakeLists.txt の project に VERSION を指定してプロダクトバージョンを設定する。
個別のプラグインにバージョンを付けたい場合は、プラグインごとの CMakeLists.txt の project に VERSION を指定してファイルバージョンを設定する。プラグインごとの CMakeLists.txt の project に VERSION を指定しなかった場合は、プロダクトバージョンが適用される。

プロダクトバージョンの例(全体で1つ)：

``` Cmake
cmake_minimum_required(VERSION 3.14)

project(TTX VERSION 1.0.4)        # <== プロダクトバージョンは 1.0.4

add_subdirectory(TTXPlugin)
set_target_properties(TTXPlugin PROPERTIES FOLDER TTX)
```

ファイルバージョンの例(プラグインごと指定可能)：

``` Cmake
cmake_minimum_required(VERSION 3.14)

project(TTXPlugin)              # <== VERSION 指定しない場合は、
                                #     プロダクトバージョンを使用

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(USE_UNICODE_API)
  add_definitions(-DUNICODE -D_UNICODE)
endif()
```

プラグインリスト表示には、リソースファイルの内容を表示するため、ダイアログが不要でもリーソースファイルを作る用にする。

## ダイアログの中央配置

ダイアログを親ウインドウの中央に配置するには、ダイアログの Procedure 関数の初期化処理 WM_CREATE において、ダイアログの初期化後、 **MoveParent()** 関数を実行する。

```c
static LRESULT CALLBACK SettingProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        //ダイアログ内の初期化
        MoveParentCenter(dlg);      // <== ここを追加する。
        return TRUE;
    }
    return FALSE;
}
```

## ダイアログボックス内のコンポーネントのリサイズ

ダイアログのリサイズでダイアログ内のコンポーネントを連動してリサイズするには、Procedure 関数の初期化処理 WM_CREATE でコンポーネントの位置を **GetPointRB()** 関数で保存し、リサイズ処理 WM_SIZE で **MovePointRB()** 関数を使い位置を補正する。

- GetPointRB() コンポーネントの右下がダイアログの右下からの位置を取得
- MovePoinrRB() コンポーネントの位置をダイアログの右下空の位置に移動

```c
static LRESULT CALLBACK dlg_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static POINT ptMsg, ptLog, ptBtn, ptSts;    // <== 対象の位置保存用

    switch (msg)
    {
    case WM_INITDIALOG:
        GetPointRB(hWnd, IDC_MSG, &ptMsg);      // <== 位置を保存
        GetPointRB(hWnd, IDC_LOG, &ptLog);      // <== 位置を保存
        GetPointRB(hWnd, IDOK, &ptBtn);         // <== 位置を保存
        GetPointRB(hWnd, IDC_STATUS, &ptSts);   // <== 位置を保存
        //その他の設定
        return TRUE;

    case WM_SIZE:
        MovePointRB(hWnd, IDC_MSG, &ptMsg,
                    RB_RIGHT);                      // <== 位置を補正
        MovePointRB(hWnd, IDC_LOG, &ptLog,
                    RB_RIGHT | RB_BOTTOM);          // <== 位置を補正
        MovePointRB(hWnd, IDOK, &ptBtn,
                    RB_TOP | RB_BOTTOM);            // <== 位置を補正
        MovePointRB(hWnd, IDC_STATUS, &ptSts,
                    RB_RIGHT | RB_TOP | RB_BOTTOM); // <== 位置を補正
        //その他の設定
        return TRUE;

```

## 著作権

著作権は、ソースコードやドキュメントの最初あたりに

```
(C) 2020,2021 tomo3136a
```

と書いておけばよい。**「(C)」** と **「公開した年」** と **「著作者」** があれば他は不要らしい。
あと、著作権者の連絡先とかもどこかに明記しておく。

## ライセンス

数多あるオープンソース用ライセンスから、制限が極力なさそうな MIT ライセンスを選ぶ。
MIT ライセンスだと明示して、ライセンスファイルをわかる場所に置いておく。
