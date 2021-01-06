# TTX
Tera Term Extension Library

## 概要

Tera Term のプラグイン集です。

| 機能 | プラグイン | 説明 |
| ---- | ---------- | ---- |
| 受信レポート表示 | [TTXReport](TTXReport) | 受信した文字列を解析し、マッチした行をレポートウィンドウに表示します |
| 再接続 | [TTXReconnect](TTXReconnect) | 再接続メニューを追加します。再接続までの待ち時間を指定できます |
| 接続経過時間を表示 | [TTXDuration](TTXDuration) | 接続開始からの時間を表示します |
| TTX プラグイン一覧表示 | [TTXPlugin](TTXPlugin) | TTX プラグインの一覧を表示します。可能な場合は、プラグインを有効/無効を設定します |
| 実行環境切り替え | [TTXEnv](TTXEnv) | 実行環境をメニューから切り替え可能にします |
| 環境パッケージ作成 | [TTXPackage](TTXPackage) | 実行環境をパッケージ化し、移植バッチファイルを作成します |
| ショートカット作成 | [TTXShortcut](TTXShortcut) | 現在の設定の Tera Term を起動するショートカットを作成します |
| ユーザーキー設定 | [TTXUserKey](TTXUserKey) | ファンクションキーなどをユーザーキーに割り当て、キー押下にて文字列送信・マクロ実行・メニュー選択を行えるように設定します |
| TTX プラグインのミスマッチ推定 | [TTXEstimateMismatch](TTXEstimateMismatch) | Tera Term のバージョンによって TTX プラグインの tttset 構造体がミスマッチしていないか推定する情報を表示します |

## 著作権等

本プログラムはフリーソフトウェアです。MITライセンスのもとに配布されています。

本プログラムは商用利用問わず無償で利用できますが、作者は本プログラムの使用にあたり生じる障害や問題に対して一切の責任を負いません。

  URL: https://github.com/tomo3136a/TTX

## 使用上の注意

次の点は注意して使用願います。

* 日本語等の2バイト文字はパス名として正常に認識しないかもしれません。
* 長いパス名など正常に認識しないかもしれません。

## 対応バージョン

  Tera Term v4.96, v4.98, v4.105

## ビルド準備

ビルドするには次の環境を用意します。

* **cmake**
* **Visual Studio 2019**
* **Tera Term ソースコード**

インストーラも作成する場合は、Visual Studio 拡張機能 **「Microsoft Visual Studio Installer Projects」** もインストールします。

```txt
cmake version 3.14.0
```

```txt
-- Building for: Visual Studio 16 2019
-- Selecting Windows SDK version 10.0.17763.0 to target Windows 10.0.19041.
-- The C compiler identification is MSVC 19.27.29112.0
-- The CXX compiler identification is MSVC 19.27.29112.0
```

「**Microsoft Visual Studio Installer Projects**」

```txt
Works with
  Visual Studio 2017, 2019
More Info
  Version 0.9.9
  Released on 2017/2/10 5:47:29
  Last updated
  2020/9/16 2:04:23
```

## ビルド

次の順でビルドします。

1. 本ソースコード(TTX)を入手します。
2. Tera Term ソースコードフォルダに、本ソースコードを配置します。
3. 本ソースコードのフォルダにある **build.bat** を実行します。 
   コマンドラインで行う場合は、**build.bat** があるディレクトリで以下を実行します。

    ```bat
    mkdir build
    cd build
    cmake -A Win32 ..
    cmake --build . --config Release
    ```

4. **build/Release/** にビルドしたプラグインが生成されます。また、**build/data/** にその他のファイルが生成されます。
5. インストーラを作成する場合は、本ソースコードの **Setup/Setup.sln** を Visual Studio で開き、インストーラをビルドしてください。ビルドしたインストーラ(TTXSetup-x.x.x.msi) は、**Setup/Release/** フォルダに生成されます。

## インストール

### インストーラによるインストール

Tera Term が標準のインストール先にインストール済みの場合、インストーラ(TTXSetup-x.x.x.msi) を使用してインストールできます。

1. インストーラを実行します。
2. ライセンスに同意します。
3. インストールが必要なコンポーネントを選択します。
    不要なプラグインはチェックを外してください。
4. インストール先を選択します。
5. インストールを開始します。

### コマンドラインからのインストール

ビルド後、本ソースコードのフォルダにある **install.bat** を実行します。

**instal.bat** は、**build/Relese/** フォルダと **build/data/** フォルダのファイルを Tera Term のフォルダにコピーします。

