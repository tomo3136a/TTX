# Tera Term ユーザーキー設定プラグイン

## 概要

ファンクションキーなどをユーザーキーに割り当て、キー押下にて文字列送信・マクロ実行・メニュー選択を行えるように設定します。
メニューから Tera Term を起動するショートカットを作成します。

## 対応バージョン

以下のバージョンで動作を確認

  Tera Term v4.96, v4.98, v4.104, v4.105

## 使い方

1. プラグインをビルドします。
2. Tera Term 実行環境 (ttermpro.exeがあるフォルダ) にビルドした TTXUserkey.dll をコピーします。
3. ショートカット作成を行うときは、メニュー[設定-ユーザーキー] を選択します。


## ビルド

cmake でビルドする場合

```bat
mkdir build
cd build
cmake -A Win32 ..
cmake --build . --config Release
```
