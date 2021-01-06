# TTXPlugin

## 概要

TTXPlugin は、Tera Term の拡張プラグイン (TTX) をリストアップ表示します。
また、可能な場合、拡張プラグインを無効にします。

※拡張プラグインを無効化は、 **「TTXPlugin に対応した TTX」**  のみ可能

TTX は、通常インストーラで使用有無を使用者が選択してインストールします。
TTX は細かい制御を提供できるため有用ですが、常に必要になるというわけではありません。
メニューにも常に表示されます。

そこで、使わないときは TTX の機能を取り除きたいと思うのですが、再インストールするか、プログラムフォルダから目的の DLL ファイルを削除する以外に方法はなく、開発者以外の人には敷居が高いものとなっています。

本拡張は、 **「TTXPlugin に対応した TTX」** に関しては、メニューから ON/OFF が行えるようにします。

## 対応バージョン

以下のバージョンで動作を確認

  Tera Term v4.98, v4.105

## 使い方

1. プラグインをビルドします。
2. Tera Term 実行環境 (ttermpro.exeがあるフォルダ) にビルドした TTXPlugin.dll をコピーします。
3. Tera Term を実行し、 メニュー[ヘルプ-TTX] を選択します。
4. TXX がリストアップされます。
5. 有効にしたい項目は「on」に、無効にしたい項目は「off」にします。
変更は項目をダブルクリックします。
「---」 は変更できない項目です。
6. 「OK」ボタンをクリックします。設定しない場合は「CANCEL」をクリックします。
7. Tera Term を再起動します。(再起動しないと設定は反映されません)

## 設定ファイル

TERATERM.INI 設定ファイルは使用しません。ttermpro.exe と同じディレクトリににある「 ttx.txt 」を使用します。

[Load] セクションに TTX ファイルの名前と設定として 「On」 「Off」 「---」 を登録します。
「Off」 を設定すると TTX を無効にします。
省略も含めてその他の場合は 「On」 として認識し TTX 有効になります。
「---」 を設定すると設定画面から TTX を切り替え設定はできなくなります。

## ビルド

cmake を使用してビルドします。

```bat
mkdir build
cd build
cmake -A Win32 ..
cmake --build . --config Release
```

## 開発メモ

本機能の制限として **「TTXPlugin に対応した TTX」** のみが制御対象となります。

TTX を **「TTXPlugin に対応した TTX」** にするには、各 TTX ソースコードに TTXIgnore() 関数追加と TTXBind() 関数に呼び出し文を追加します。

TTXIgnore() 関数追加：

``` C
BOOL TTXIgnore(int order, PCHAR name, DWORD version)
{
    char buf[8];
    GetPrivateProfileString("Load", INISECTION, "", buf, sizeof(buf), ".\\ttx.txt");
    return (_strnicmp("off", buf, 3) == 0);
}
```

TTXBind() 関数に呼び出し文を追加：

``` C
BOOL __declspec(dllexport) PASCAL FAR TTXBind(WORD Version, TTXExports *exports)
{
    int size = sizeof(Exports) - sizeof(exports->size);
    /* do version checking if necessary */
    /* if (Version!=TTVERSION) return FALSE; */

    if (TTXIgnore(ORDER, INISECTION, 0))     //<= 追加
        return TRUE;     　　　　　　　　　　　//<= 追加

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

Tera Term のプログラム内部を修正し TTX の DLL 呼び出し前に判別すれば、 ttx.txt ファイルをすべての TTX を対象にすることができます。
