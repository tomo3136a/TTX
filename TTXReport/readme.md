# ■■■■ ＴＴＸＲｅｐｏｒｔ ■■■■

## 概要

このツールは、Teraterm のプラグインです。
受信した行に設定ファイル(TERATERM.INI)に設定した文字列を含む場合、
専用ダイアログにその行を表示します。

## 使い方

TTXReport.dll を Teraterm.exe のあるフォルダにコピーして使用します。

メニュー[ Control-InformationBox ]  
  Report ウインドウを表示/非表示を切り替えます。

メニュー[ Control-Information Clear ]  
  Report ウインドウの表示を初期状態にします。
  
メニュー[ Control-Information Auto Start ]  
  起動時に Report ウインドウを表示すか設定します。

## 設定ファイル

TTXReport のセクションは、[TTXReport] を使用します。

> ReportAutoStart=on/off
> ReportSize=<ウィンドウサイズX>,<ウィンドウサイズY>
> ReportFont=<フォント名>,<フォント高さ>,<キャラクタセット>
> ReportTitle=<タイトル領域表示テキスト>
> ReportNote=<タイトル領域表示テキストのファイルのパス>
> ReportClear=<状態をクリアするマッチ文字列>
> ReportRule[1-20]=<シーケンス番号>,<次のシーケンス番号>,<コマンド>,<タイプ>,<検索文字列>

## 将来ほしい機能

* ルール定義設定画面
