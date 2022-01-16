# Tera Term 5 対応

date:  2021/11/23

自作プラグインを Tera Term 5 対応にするための検討メモをここに記す。

## 概要

Tera Term v.4.106 リリース(r9298, 2021/6/5)後、本格的に Tera Term 5 の実装が始まった。

Tera Term 5 のプラグインに対する主要な機能変更は以下となる。

* メジャーバージョンアップ
* 内部コードの Unicode 対応
* 設定ファイルの保存先の変更
* パス文字列の MAX_PATH(=260) 文字数の撤廃
* Windows 95 のサポート終了
* 実行環境変数(TTset, ComVar)の構造変更

## 「メジャーバージョンアップ」について

セキュリティに問題や使用機能に大きなバグでもない限り、すぐに最新版に切り替える人は多くないと思われる。\
自分も含め、COM ポートのようなレガシデバイスを主として使用する人は、最新版にする必要性を感じない。\
このため、プラグインとしてはしばらくは両メジャーバージョンに対応したい。

プラグインのソースコードは、は、可能な限り同じとし、v4 用、Tera Term 5 用に別々にビルドすることで対応する。\
v4 用のビルドは、コンパイラのオプション **-DTT4** を指定する。

## 「内部コードの Unicode 対応」について

v4 以前は、内部コードは基本的にバイト文字で動作していた。\
Tera Term 5 では、一部を除いてバイト文字は廃止し、ワイド文字(Unicode)に変更。

Unicode 化しない機能：

* 通信データ\
通信データはバイト列として扱うため変更対象外。

* 実行環境変数(TTset, ComVar)\
※ 将来的にはワイド文字になると思われる。

* 設定ファイル(TERATERM.INI, KEYBOARD.CNF, cygterm.cfg)\
※ GetPrivateProfileStringW 関数で自動的にワイド文字に変換して取得する。\
※ GetPrivateProfileStringA 関数で明示的にバイト文字で取得する。

1. tchar.h インクルード
2. コンパイルオプション **-DUNICODE**, **-D_UNICODE** を指定
3. 文字列用の変数の型を修正
	| v4 以前 | Tera Term 5 |
	|-|-|
	|char | TCHAR |
	|char * | LPTSTR |
	|const char * | LPCTSTR |
	|||
4. 対象の文字列は、 **_T( )** で括る
5. 文字列操作関数は、 tchar.h の関数に置き換え
	* strxxx → _tcsxxx (※xxxはいろいろな関数名)
	* atoi → _tstoi
	* strlen → _tsnlen
	* snprintf_s → sntprintf_s
6. セキュア対策していない文字列関数はこの際セキュア対策
7. 引数の文字列サイズを型に対応\
	sizeof(buf) → sizeof(buf)/sizeof(buf[0])\
	sizeof(buf) → sizeof(buf)/sizeof(TCHAR)\
	p = malloc(len) → p = (LPTSTR)malloc(len*sizeof(TCHAR))
8. プラグイン呼び出し関数の引数の型は、マクロを使用\
プラグイン呼び出し関数の引数は const の扱いが異なるため、\
単純置き換えはワーニングになる。このため、両対応のマクロを使用する。
	| v4 以前 | Tera Term 5 | 両対応プラグイン |
	|-|-|-|
	|char * | wchar_t * | TTX_LPTSTR |
	|char * | const wchar_t * | TTX_LPCTSTR |
	|||

## 「設定ファイルの保存先の変更」について

v4 では、VirtualStore 機能により、プログラムファイルフォルダの TeraTerm フォルダの下に設定ファイルがあるように見せていた。\
Tera Term 5 では AppData/Roaming/teraterm5 フォルダの下に移動した。\
その他の設定ファイルは、インストール時から変化しないのでこの対象ではない。\

プラグインとしては、大きく対応は必要ない。Roamingフォルダは階層が深く、\
テキストボックスで表示するととても長い。
何かしらの対策が欲しい。(T.B.D.)

各種パスの取得・設定は、ヘルパ関数を使用する。(ttxcommon.h参照)

| 項目 | 関数 | 説明 |
|-|-|-|
|パス取得 | TTXGetPath(ts, id) | 設定変数 ts から id で指定したパスを取得する。<br/>取得したパスは TTXFree() で破棄する|
|パス設定 | TTXSetPath(ts, id, s) | 設定変数 ts から id で指定したパスに s を設定する。 |
|||

    //path string
    enum {
        ID_HOMEDIR              = 1,
        ID_SETUPFNAME           = 2,
        ID_KEYCNFNM             = 3,
        ID_LOGFN                = 4,
        ID_MACROFN              = 5,
        ID_UILANGUAGEFILE       = 6,
        ID_UILANGUAGEFILE_INI   = 7,
        ID_EXEDIR               = 8,
        ID_LOGDIR               = 9,
        ID_STRMAX,
    };
    LPTSTR TTXGetPath(PTTSet ts, UINT uid);
    BOOL TTXSetPath(PTTSet ts, UINT uid, LPTSTR s);

    BOOL TTXFree(LPVOID *pBuf);

TTXFree() 関数は、文字列を格納したポインタ変数のアドレスを引数に渡す。\
ポインタの指すメモリを解放し、ポインタ変数は NULL に設定する。

## 「パス文字列の MAX_PATH(=260) 文字数の撤廃」について

v4 以前は、スタック上に MAX_PATH バイトの配列を用意して使用していた。
Tera Term 5 では動的に必要分だけ用意し、使用が終わったら破棄する。

動的な文字列領域の取得は、基本的に **malloc()** 関数を使用する。\
文字列のタイプを変換を伴うことが多いため、次のヘルパ関数を使用する。(ttxcommon.h参照)

### LPTSTR文字列の関数

| 項目 | 関数 | 説明 |
|-|-|-|
|LPTSTR 文字列複製| _tcsdup(pszSrc) | LPTSTR 文字列を複製する。<br/>取得した文字列は free() で破棄する|
|バイト文字列取得| toMB(pszSrc) | LPTSTR 文字列からバイト文字列を取得する。<br/>取得した文字列は TTXFree() で破棄する|
|ワイド文字列取得| toWC(pszSrc) | LPTSTR 文字列からワイド文字列を取得する。<br/>取得した文字列は TTXFree() で破棄する|
|||

### ワイド文字列の関数

| 項目 | 関数 | 説明 |
|-|-|-|
|ワイド文字列からバイト文字列取得| WC2MB(cp, pwzSrc) | ワイド文字列から指定したコードページ cp の<br/>バイト文字列を取得する。<br/>取得した文字列は TTXFree() で破棄する|
|ワイド文字列からバイト文字列取得<br/>(バイト文字列はOSの標準)| WC2ACP(pwzSrc) | ワイド文字列からバイト文字列を取得する。<br/>取得した文字列は TTXFree() で破棄する|
|ワイド文字列からバイト文字列取得<br/>(バイト文字列はUTF-8)| WC2UTF8(pwzSrc) | ワイド文字列からバイト文字列を取得する。<br/>取得した文字列は TTXFree() で破棄する|
|||

### バイト文字列の関数

| 項目 | 関数 | 説明 |
|-|-|-|
|バイト文字列からワイド文字列取得| MB2WC(cp, pwzSrc) | 指定したコードページ cp のバイト文字列から<br/>ワイド文字列を取得する。<br/>取得した文字列は TTXFree() で破棄する|
|バイト文字列からワイド文字列取得<br/>(バイト文字列はOSの標準)| ACP2WC(cp, pwzSrc) | バイト文字列からワイド文字列を取得する。<br/>取得した文字列は TTXFree() で破棄する|
|バイト文字列からワイド文字列取得<br/>(バイト文字列はUTF-8)| UTF82WC(cp, pwzSrc) | バイト文字列からワイド文字列を取得する。<br/>取得した文字列は TTXFree() で破棄する|
|||

## 「Windows 95 のサポート終了」について

Windows 95 用のビルドをサポートしない。
v4 では、**DllMain()** 関数に、**DoCover_IsDebuggerPresent()** を入れるため、
compat_w95.h をインクルードしていたが、Windows95 は確認出来なかった。\
サポート外となったため、削除。v4 でも無くても Windows10 は動作している。

## 「実行環境変数(TTset, ComVar)の構造変更」について

プラグインから見れば、どのバージョンの Tera Term でも同じプラグインを使えるのが望ましい。\
しかしながら、Tetra Term 本体からすれば、機能を追加・更新により実行環境変数の構造は変わってきてしまう。\
できれば、同じメジャーバージョンでの実行環境変数(TTset, ComVar)は、末尾に追加していくだけにしてほしい。

v4 から Tera Term 5 になるにあたって、将来追加する可能性がある箇所に予約領域を設けてくれた。\
v4 と Tera Term 5 で構造は異なるが、Tera Term 5 内では構造が保たれると考えられる。
