# ttset パス名対応

2020/1/9 tomo3136

## 概要

`TeraTerm v5` 以降は、内部パス文字列のワイドキャラクタ化と同時に、パス名の文字数は `MAX_PATH` 制限が無くなり、ワイドキャラクタの可変長文字列になる。

### 可変長化

* `TeraTerm v4` 以前:  
  ttset 構造体の固定長のパス名(マルチバイト文字列)を使用。(パス名は MAX_PATH の文字数制限)

* `TeraTerm v5` 以降:  
  マルチバイト文字列でアクセスする場合は、 TeraTerm v4 以前と同じ構造体メンバーのパス名を使用。(パス名は MAX_PATH の文字数制限)  
  ワイドキャラクタ文字列でアクセスする場合は、ttset 構造体に新規に追加されたメンバーを使用。

## 対応

プラグインは、ソースコードレベルで `TeraTerm v4`(内部マルチバイト文字列) 、`TeraTerm v5`(内部ワイドキャラクタ文字列) 両方に対応したいため、以下とする。

* `v4` 用の場合は、ビルドオプション `-DTT4` を付けてビルドする。(cmake で指定)  
  未指定しない場合は、 `v5` 用をビルドする。(今後メインになるはずなので)
* `ttset` 構造体のパス文字列へのアクセスは、次の関数を使用する。(`ttxcommon.h` 参照)
  * `TTXGetPath()`:  
    `ttset` 構造体から `uid` に対応したパス文字列を取得する。  
    * `v4` では、内部マルチバイト文字列のコピーを取得する。
    * `v5` では、内部ワイドキャラクタ文字列のコピーを取得する。

    戻り値の文字列が無い場合は、 NULL を返す。  
    戻り値の文字列は、不要になったら `TTXFree()` でメモリを解放する。

    ```c ttxcommon.h
      LPTSTR TTXGetPath(PTTSet ts, UINT uid);
    ```

  * `TTXSetPath()`:  
    `uid` に対応したパス文字列を`ttset` 構造体に設定する。  
    設定できた場合は、 TRUE を返す。出来ない場合は、 FALSE を返す。

    ```c ttxcommon.h
      BOOL TTXSetPath(PTTSet ts, UINT uid, LPTSTR path);
    ```

* パス文字列の `uid`:  
  (`ttxcommon.h` 参照)

  ```c ttxcommon.h
    //path string
    enum {
        ID_HOMEDIR              = 1,
        ID_SETUPFNAME           = 2,
        ID_KEYCNFNM             = 3,
        ID_LOGFN                = 4,
        ID_MACROFN              = 5,
        ID_UILANGUAGEFILE       = 6,
        ID_UILANGUAGEFILE_INI   = 7,
        ID_EXEDIR               = 8,   /* support v5 */
        ID_LOGDIR               = 9,   /* support v5 */
        ID_FILEDIR              = 10,
        ID_STRMAX,
    };
  ```
