# TTX Estimate of TTSet structure mismatch

## 説明

旧版の Tera Term と TTSet 構造体が崩れているか推測するプラグインです。

TTSet 構造体の中のバイト配列を文字列としてメッセージボックスに表示します。

通常、バイト配列は文字列として扱うことが多いため、構造体が崩れて構造体メンバーの
位置がずれたら文字列が読めないか、先頭が欠けたりします。

文字列が読めなかったり、先頭が欠けたりしたら TTSet 構造体が崩れている可能性が
高いと推測できます。

## 使い方

1. 最新の Tera Teerm のソースコードでプラグインをビルドします。
2. 対象とする旧版の Tera Term 実行環境(ttermpro.exeがあるフォルダ)にビルドした TTXEstimateMismatch.DLL をコピーします。
3. 旧版の Tera Term を実行し、メニュー[ヘルプ-Estimate of TTSet structure mismatch] を選択します。
4. 表示される項目 ( 名前=値 ) のうち、値の文字列を読めなかったり、先頭が欠けたりしているか確認します。

※ 文字列が無い項目もあります。
