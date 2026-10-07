# このフォルダには...
このフォルダにはParaSolのコンパイル済みバイナリイメージが置かれています。  
ファイル形式はELFとHEXの2種類に分かれていますが、データの内容は同じです。書き込みソフトに合わせてどちらかを利用してください。

# 各ファイルの説明
ファイル名には ParaSol_ に続いてバージョン番号とファイルの簡単な概要が付いています。

| イメージ名 | 概要 |
| --- | --- |
| Single | シングルポート構成でコンパイルしたもの。プリプロセッサオプションの変更のみ。 |
| Dual | デュアルポート構成でコンパイルしたもの。プリプロセッサオプションの変更のみ。 |
| Dual_LedP44_RdyP45 | デュアルポート構成で、PWRSTAT(LED)をP44、READYをP45に変更したもの。 |
| Dual_ParaSolPCB | デュアルポート構成で、PWRSTATをP80、READYをP81に変更したもの。ParaSolPCB用。 |


# ソースの変更箇所
マニュアルの「10章 移植の手引き」のサンプルとして、ソースの変更箇所を示しておきます。

## Dual_LedP44_RdyP45 - 48ピンパッケージデバイス用
global.h
```c
// GPIO定義
#define LED       (0x10U)
#define READY     (0x20U)
#define LEDPORT   (PORT4->P4DO)
#define READYPORT (PORT4->P4DO)
```

setup.c
```c
static void setupGpio(void) {
  if(DUALPORT) {
    PORT4->P4MOD0 = 0x15151215U;  // プライマリ P43=SS0# in, P42=SDI0 in, P41=SDO0 out, P40=SCK0 in

    PORT3->P3MOD1 = 0x05050000U;  // セカンダリ P35=SS0# in, P34=SDI0 in
    PORT3->P3MOD0 = 0x00050000U;  //        P33=SDO0 out, P32=SCK0 in
  }
  else {
    PORT6->P6MOD0 = 0x15151215U;  // P63=SS1# in, P62=SDI1 in, P61=SDO1 out, P60=SCK1 in
  }

  clear_bit(READYPORT, READY);    // READY=L 起動処理中はLow
  set_bit(LEDPORT,   LED);        // LED=点灯
  PORT4->P4MOD1 = 0x00000202U;    // P45=READY out P44=LED out
}
```


## Dual_ParaSolPCB - ParaSolPCBの出荷時インストール用
global.h
```c
// GPIO定義
#define LED       (0x01U)
#define READY     (0x02U)
#define LEDPORT   (PORT8->P8DO)
#define READYPORT (PORT8->P8DO)
```

setup.c
```c
static void setupGpio(void) {
  if(DUALPORT) {                  // プルアップを実装しているのでSPIの入力ポートはプルアップ(bit2)をOFFにしている。
    PORT4->P4MOD0 = 0x11111211U;  // プライマリ P43=SS0# in, P42=SDI0 in, P41=SDO0 out, P40=SCK0 in

    PORT3->P3MOD1 = 0x01010000U;  // セカンダリ P35=SS0# in, P34=SDI0 in
    PORT3->P3MOD0 = 0x00010000U;  //        P33=SDO0 out, P32=SCK0 in
  }
  else {
    PORT6->P6MOD0 = 0x15151215U;  // P63=SS1# in, P62=SDI1 in, P61=SDO1 out, P60=SCK1 in
  }

  clear_bit(READYPORT, READY);    // READY=L 起動処理中はLow
  set_bit(LEDPORT,   LED);        // LED=点灯
  PORT8->P8MOD0 = 0x00000202U;    // P81=READY out P80=LED out
}
```

00_sysCmd.c - changeAccessPort() の抜粋
```c
  if(port == 0) {  // プライマリへ変更
    PORT3->P3MOD1 = 0x01010000U;  // セカンダリ Hi-Z出力  ここもプルアップ(bit2)をOFFにした入力に設定する
    PORT3->P3MOD0 = 0x00010000U;
    PORT4->P4MOD0 = 0x11111211U;  // プライマリ P43=SS0# in, P42=SDI0 in, P41=SDO0 out, P40=SCK0 in

    irq_ext3_init(EXIn_EDGE_RISING, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, EXIn_PORT_SEL_P43);
  }
  else {              // セカンダリへ変更
    PORT4->P4MOD0 = 0x01010001U;  // プライマリ Hi-Z出力
    PORT3->P3MOD1 = 0x00001111U;  // セカンダリ P35=SS0# in, P34=SDI0 in
    PORT3->P3MOD0 = 0x12110000U;  //        P33=SDO0 out, P32=SCK0 in

    irq_ext5_init(EXIn_EDGE_RISING, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, EXIn_PORT_SEL_P35);
  }
```
