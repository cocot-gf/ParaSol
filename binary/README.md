# このフォルダにあるもの
このフォルダにはParaSolのコンパイル済みバイナリイメージが置かれています。  
ファイル形式はELFとHEXの2種類に分かれていますが、データの内容は同じです。  
書き込みソフトに合わせてどちらかを利用してください。

# 各ファイルの説明
ファイル名には ParaSol_ に続いてバージョン番号とファイルの簡単な概要が付いています。

| ファイル名 | 概要 | PWRSTAT | READY |
| --- | --- | --- | --- |
| Single | シングルポート構成でコンパイルしたもの。 | P72 | P76 |
| Dual | デュアルポート構成でコンパイルしたもの。 | P72 | P76 |
| Single_LedP56_RdyP76 | シングルポート構成、DT-EBML63Q2557のFT2232H経由でPCと通信するための変更。<br> P76の追加配線が必要。 | P56 | P76 |
| Dual_LedP44_RdyP45 | デュアルポート構成、48ピンパッケージデバイス用の変更。 | P44 | P45 |
| Dual_ParaSolPCB | デュアルポート構成、ParaSolPCB用の変更。 | P80 | P81 |


# ソースの変更箇所
マニュアルの「10章 移植の手引き」のサンプルとして、ソースの変更箇所を示しておきます。

## 標準のソースファイル
global.h
```c
// GPIO定義
#define LED       (0x04U)          // P72
#define READY     (0x40U)          // P76
#define LEDPORT   (PORT7->P7DO)    // P72
#define READYPORT (PORT7->P7DO)    // P76
```

setup.c
```c
static void setupGpio(void) {
  if(DUALPORT) {
    PORT4->P4MOD0 = 0x15151215U;  // プライマリ P43=SS0# in, P42=SDI0 in, P41=SDO0 out, P40=SCK0 in

    PORT3->P3MOD1 = 0x05050000U;  // セカンダリ P35=SS0# in, P34=SDI0 in
    PORT3->P3MOD0 = 0x00050000U;  //           P33=SDO0 out, P32=SCK0 in
  }
  else {
    PORT6->P6MOD0 = 0x15151215U;  // P63=SS1# in, P62=SDI1 in, P61=SDO1 out, P60=SCK1 in
  }

  clear_bit(READYPORT, READY);    // READY=L 起動処理中はLow
  set_bit(LEDPORT,   LED);        // LED=点灯
  PORT7->P7MOD1 = 0x00020000U;    // P76=READY out
  PORT7->P7MOD0 = 0x00020000U;    // P72=LED out
}
```


## Single_LedP56_RdyP76 - DT-EBML63Q2557のFT2232H経由でPCと通信するための変更
global.h
```c
// GPIO定義
#define LED         (0x40U)        // 変更 P56
#define READY       (0x40U)
#define LEDPORT     (PORT5->P5DO)  // 変更 P56
#define READYPORT   (PORT7->P7DO)
```

setup.c
```c
static void setupGpio(void) {
  // 中略

  clear_bit(READYPORT, READY);
  set_bit(LEDPORT,   LED);
  PORT7->P7MOD1 = 0x00020000U;
  PORT5->P5MOD1 = 0x00020000U;    // 変更 P56
}
```


## Dual_LedP44_RdyP45 - 48ピンパッケージデバイス用の変更
global.h
```c
// GPIO定義
#define LED       (0x10U)        // 変更 P44
#define READY     (0x20U)        // 変更 P45
#define LEDPORT   (PORT4->P4DO)  // 変更 P44
#define READYPORT (PORT4->P4DO)  // 変更 P45
```

setup.c
```c
static void setupGpio(void) {
  // 中略

  clear_bit(READYPORT, READY);
  set_bit(LEDPORT,   LED);
  PORT4->P4MOD1 = 0x00000202U;    // 変更 P45 P44
}
```


## Dual_ParaSolPCB - ParaSolPCBの出荷時インストール用の変更
ParaSolPCBはプルアップ抵抗を搭載しているので、ポートモードの設定時にマイコンの内蔵プルアップ(bit2)をOFFにしています。

global.h
```c
// GPIO定義
#define LED       (0x01U)        // 変更 P80
#define READY     (0x02U)        // 変更 P81
#define LEDPORT   (PORT8->P8DO)  // 変更 P80
#define READYPORT (PORT8->P8DO)  // 変更 P81
```

setup.c
```c
static void setupGpio(void) {
  if(DUALPORT) {
    PORT4->P4MOD0 = 0x11111211U;  // 変更 内蔵プルアップなし

    PORT3->P3MOD1 = 0x01010000U;  // 変更 内蔵プルアップなし
    PORT3->P3MOD0 = 0x00010000U;  // 変更 内蔵プルアップなし
  }
  else {
    PORT6->P6MOD0 = 0x15151215U;
  }

  clear_bit(READYPORT, READY);
  set_bit(LEDPORT,   LED);
  PORT8->P8MOD0 = 0x00000202U;    // 変更 P81 P80
}
```

00_sysCmd.c - changeAccessPort() の抜粋
```c
  if(port == 0) {
    PORT3->P3MOD1 = 0x01010000U;  // 変更 ここでも内蔵プルアップビットをOFFの状態で切り替える。
    PORT3->P3MOD0 = 0x00010000U;  // 変更
    PORT4->P4MOD0 = 0x11111211U;  // 変更

    irq_ext3_init(EXIn_EDGE_RISING, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, EXIn_PORT_SEL_P43);
  }
  else {
    PORT4->P4MOD0 = 0x01010001U;  // 変更
    PORT3->P3MOD1 = 0x00001111U;  // 変更
    PORT3->P3MOD0 = 0x12110000U;  // 変更

    irq_ext5_init(EXIn_EDGE_RISING, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, EXIn_PORT_SEL_P35);
  }
```
