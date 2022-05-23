# TTTypes テストプラグイン

## 概要

プラグインで使用する TTTset, TComVar 構造体のバージョン依存率を下げ、メンバアクセスのスケーラビリティを向上する。

## 目標

- 対象は TTTset, TComVar 構造体およびメンバ変数の eterm_lookfeel_t, cygterm_t 構造体
- ビルドは最新版 Teraterm のソースコードで行う
- 実行バージョンに合わせて構造体のメンバにアクセス
- スケーラビリティ 80% 実装を目標
- TTXBind() 内で指定した TTTset, TComVar 構造体のメンバが実装される前のバージョンでは、 TTX をロードしない

## 使用方法

- ttxversion.h をインクルードする。(ttxversion.h 内でインクルードする ttxversion_db.h も用意する)
- ビルド対象に ttxversion.c を追加
- ts->xxx の代わりに、TS(ts,xxx) を使用
- cv->xxx の代わりに、CV(cv,xxx) を使用
- TTXBind() 内で、バージョンを指定して TTXInitVersion() 呼び出す (Teraterm の実行バージョンを登録)
- TXInitVersion() の引数を 0 にした時、実行バージョンをリソースから取得。
- プラグインで使用する TTTset 構造体のメンバーが使用可能か TEST_TS(xxx) でチェック。
- プラグインで使用する TComVar 構造体のメンバーが使用可能か TEST_CV(xxx) でチェック。
- TTXBind() 内で、 TEST_TS(xxx)/TEST_CV(xxx) でチェックし、不可ならプラグインを取り込まない。
- メンバが複数ある場合は、チェックに TEST_TS2(xxx,xxx), TEST_TS3(xxx,xxx,xxx), ... を使用する。
- メンバが複数ある場合は、チェックに TEST_CV2(xxx,xxx), TEST_CV3(xxx,xxx,xxx), ... を使用する。
- TTXBind() 内で、 TEST_CV(xxx)/TEST_CV(xxx) をチェックし、メンバーが使用不可の場合、別の処理を行える。
