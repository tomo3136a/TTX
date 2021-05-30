# 拡張機能追加メモ

(Tera Term 4.105 2020/12/29 調べ)

[Tera Term reserved]

```ini
ORDER=4800, 5000, 5800, 4000, 5800, 9999, 5990, 4800, 4000, 4000, 5900, 5800, 2501, 4000, 6001, 4000,
MENU=54009, 54209, 54200, 37000, 55000, 39393, 55500, 55501, 55502, 55503, 55101-55154, 55199, 6000, 55302, 55303, 55200, 55210, 
```

ORDERの番号は、重複した番号を割り当てても問題ないが、わかりやすいように 6000 番台で統一する。

MENUの番号は、以下で定義される。

* 最大値 65535 まで設定可能。
* 番号は重なってはいけない。
* 0-54999 は、システムで予約。
* 55000-55999 は、他の TTX で割り当て済み。

このため、残りの 56000-65535 をフリー領域とし、56000-56999 をこの TTX を割り当てる。

[TTXEstimateMatch]

```ini
 ORDER=6000
 MENU=56000,"Estimate of structure mismatch in TTSet..."
```

[TTXPlugin]

```ini
 ORDER=6001
 MENU=56001,"TTX &list","TTX 機能一覧(&L)"
 FILE=".\\ttx.txt"
```

[TTXShortcut]

```ini
ORDER=6010
MENU=56010,"&Make shortcut...","ショートカット作成(&M)..."
```

[TTXPackage]

```ini
ORDER=6015
MENU=56015,"&Export package...","パッケージ出力(&E)..."
```

[TTXUserKey]

```ini
ORDER=6020
MENU=56020,"&UserKey setup...","ユーザーキー(&U)..."
```

[TTXReconnect]

```ini
ORDER=6030
MENU=56030,"reconnect...","再接続(&F)..."
MENU_1=56031,"reconnect...","再接続(&F)..."
```

[TTXEnv]

```ini
ORDER=6040
MENU=56040,"master environment"
MENU_1=56041,"environment 1"
MENU_2=56042,"environment 2"
MENU_3=56043,"environment 3"
MENU_4=56044,"environment 4"
MENU_5=56045,"environment 5"
MENU_6=56046,"environment 6"
MENU_7=56047,"more environment..."
MENU_8=56048,"En&vironment setup...","環境設定(&V)..."
MENU_9=56049,"Environment cop&y...","環境保存(&Y)..."
INT_1="EnableEnv",bool,rw
INI_2="SetupDir",path,rw
INI_3="KeyCnf",path,rw
```

[TTXReport]

```ini
ORDER=6060
MENU=56060,"&View Report","受信レポート(&V)..."
MENU_1=56061,"C&lear Report","受信レポートクリア(&L)..."
MENU_2=56062,"Edit &Test Pattern","テストパターン編集..."
MENU_3=56063,"Report setup...","受信レポート(&Q)..."
INI_1="ReportAutoStart",bool,rw,"自動起動"
INI_2="ReportSize",point,rw,"ウィンドウサイズ"
INI_3="ReportFont",font,rw,"フォント"
INI_4="ReportTitle",string,rw,"タイトル"
INI_5="ReportNote",path,rw,"ノート"
INI_6="ReportClear",string,rw,"パターン"
INI_7="InfoTest",path,rw,"パターン"
INI_8="ReportRule1",string,rw,"評価ルール"
INI_9="ReportRule2",string,rw,"評価ルール"
INI_10="ReportRule3",string,rw,"評価ルール"
INI_11="ReportRule4",string,rw,"評価ルール"
INI_12="ReportRule5",string,rw,"評価ルール"
INI_13="ReportRule6",string,rw,"評価ルール"
INI_14="ReportRule7",string,rw,"評価ルール"
INI_15="ReportRule8",string,rw,"評価ルール"
INI_16="ReportRule9",string,rw,"評価ルール"
INI_17="ReportRule10",string,rw,"評価ルール"
INI_18="ReportRule11",string,rw,"評価ルール"
INI_19="ReportRule12",string,rw,"評価ルール"
INI_20="ReportRule13",string,rw,"評価ルール"
INI_21="ReportRule14",string,rw,"評価ルール"
INI_22="ReportRule15",string,rw,"評価ルール"
INI_23="ReportRule16",string,rw,"評価ルール"
INI_24="ReportRule17",string,rw,"評価ルール"
INI_25="ReportRule18",string,rw,"評価ルール"
INI_26="ReportRule19",string,rw,"評価ルール"
INI_27="ReportRule20",string,rw,"評価ルール"
INI_28="InfoTest1",string,rw,"テストファイルのパス"
INI_29="InfoTest2",string,rw,"テストファイルのパス"
INI_30="InfoTest3",string,rw,"テストファイルのパス"
INI_31="InfoTest4",string,rw,"テストファイルのパス"
INI_32="InfoResult",path,rw,"結果のパス"
```

[TTXDuration]

```ini
ORDER=6070
MENU=56070,"&Timer","タイマ(&T)"
MENU_1=56071,"Connent start timer","タイマ接続開始"
MENU_2=56072,"Disconnent stop timer","タイマ切断停止"
MENU_3=56073,"Reset start timer","タイマリセット開始"
MENU_4=56074,"Now time &mode","時刻表示"
MENU_5=56075,"On/Off Menu","開始/停止メニュー"
MENU_6=56076,"Clear timer","タイマクリア"
MENU_9=56079,"&Duration","経過時間"
IdDurationTimer=3002
```
