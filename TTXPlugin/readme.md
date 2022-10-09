# TTXPlugin

## 概要

TTXPlugin は、Tera Term の拡張プラグイン (TTX) をリストアップ表示します。
また、可能な場合、拡張プラグインを無効にします。

※拡張プラグインを無効化は、現時点では **「TTXPlugin に対応した TTX」**  のみ可能 \
　TeraTerm のソースコードにパッチをあてることにより、すべての拡張プラグインに対応させることができます。

TTX は、通常インストーラで使用有無を使用者が選択してインストールします。
TTX は細かい制御を提供できるため有用ですが、常に必要になるというわけではありません。
メニューにも常に表示されます。

そこで、使わないときは TTX の機能を取り除きたいと思うのですが、再インストールするか、プログラムフォルダから目的の DLL ファイルを削除する以外に方法はなく、開発者以外の人には敷居が高いものとなっています。

本拡張プラグインにより、メニューから拡張プラグインを ON/OFF が行えるようにします。

## 対応バージョン

以下のバージョンで動作を確認

  Tera Term v4.98, v4.105, v5.0alpha

## 使い方

1. プラグインをビルドします。
2. Tera Term 実行環境 (ttermpro.exeがあるフォルダ) にビルドした TTXPlugin.dll をコピーします。
3. Tera Term を実行し、 メニュー[ヘルプ-TTX] を選択します。
4. TXX がリストアップされます。
5. 有効にしたい項目は「on」に、無効にしたい項目は「off」にします。
変更は項目をダブルクリックします。
「-」 は変更できない項目です。
6. 「OK」ボタンをクリックします。設定しない場合は「CANCEL」をクリックします。
7. Tera Term を再起動します。(再起動しないと設定は反映されません)

## 設定ファイル

TTX の設定は、 TERATERM.INI に設定します。

[TTXPlugin] セクションに TTX ファイルの名前と設定として 「on」 「off」 「-」 を登録します。  
「on」 を設定すると TTX を有効にします。  
「off」 を設定すると TTX を無効にします。  
「-」 設定すると TTX を有効にします。また、設定画面から TTX を無効に設定はできなくなります。  
値を省略した場合や、登録のない TTX は 「off」 として認識し TTX を無効にします。

また、拡張プラグインのDLLファイルが対応しいている必要がありますが、
2つ目のパラメータにメニューID のオフセットを指定できます。

次の例では、
「TTXPlugin」 「ttxssh」 「TTXProxy」 の TTX は無効にできない。  
「TTXShortcut」 「TTXUserKey」 の TTX は有効にします。  
「TTXDuration」 や登録のない「TTXTest_Exports」のTTX は無効にします。  

TERATERM.INI:

``` INI
[TTXPlugin]
TTXPlugin=-
ttxssh=-
TTXProxy=-
TTXShortcut=on
TTXUserKey=on
TTXDuration=off
```

## ビルド

cmake を使用してビルドします。

```bat
mkdir build
cd build
cmake -A Win32 ..
cmake --build . --config Release
```

### Tera Term パッチ

Tera Term にパッチを当ててビルドすることにより、すべての拡張プラグインの on/off を行うことが可能にします。

パッチ：

``` C
--- teraterm/teraterm/ttplug.c.org	Mon May 24 00:35:59 2021
+++ teraterm/teraterm/ttplug.c	Mon May 24 06:43:54 2021
@@ -68,6 +68,18 @@
   char buf[1024];
   DWORD err;
   char uimsg[MAX_UIMSG];
+  char *p1;
+  char *p2;
+
+  p1 = strrchr(fileName, '\\');
+  if (NULL != p1) {
+    p2 = strchr(++p1, '.');
+    if (NULL != p2) {
+      strncpy_s(buf, 1024, p1, (p2 - p1)/sizeof(char));
+      GetPrivateProfileString("TTXPlugin", buf, "off", buf, sizeof(buf), ts.SetupFName);
+      if (_strnicmp("off", buf, 3) == 0) return;
+    }
+  }
 
   if (NumExtensions>=MAXNUMEXTENSIONS) return;
   LibHandle[NumExtensions] = LoadLibrary(fileName);
```

## 開発メモ

Tera Term にパッチを当てない場合、 **「TTXPlugin に対応した TTX」** のみ制御対象とdなります。

TTX を **「TTXPlugin に対応した TTX」** にするには、各 TTX ソースコードに TTXIgnore() 関数追加と TTXBind() 関数に呼び出し文を追加します。

TTXIgnore() 関数追加：

``` C
BOOL TTXIgnore(int order, PCHAR name, DWORD version)
```

TTXBind() 関数に呼び出し文を追加：

``` C
BOOL __declspec(dllexport) PASCAL FAR TTXBind(WORD Version, TTXExports *exports)
{
    int size = sizeof(Exports) - sizeof(exports->size);
    /* do version checking if necessary */
    /* if (Version!=TTVERSION) return FALSE; */

    if (TTXIgnore(ORDER, INISECTION, Version))    //<= 追加
        return TRUE;     　　　　　　　　　　　     //<= 追加

    if (size > exports->size)
    {
        size = exports->size;
    }
    memcpy((char *)exports + sizeof(exports->size),
           (char *)&Exports + sizeof(exports->size),
           size);
    return TRUE;
}

```

本機能は、 TTX をロードしないわけではなく、呼び出しエントリを登録しないだけですので、根本的な解決方法ではありませんが、普段の使用では十分と思います。
