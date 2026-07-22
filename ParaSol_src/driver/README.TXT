ParaSol - Driverフォルダ

このフォルダは「ML63Q2500シリーズ リファレンスソフトウェア v1.2」と
「加速度センサを使った異常検知のAIライブラリ rev.3」に含まれる
ソースコードを格納する場所です。

これらのソースコードは、使用許諾によって再配布が禁止されていますので、
ゼロバイトの同名のファイルを仮置きしています。

お手数ですが、ロームのサイトから「ML63Q2500シリーズ リファレンスソフトウェア」
「加速度センサを使った異常検知のAIライブラリ」をダウンロードし、
同名のファイルを上書きコピーしてください。

ダウンロードURL
  https://www.rohm.co.jp/products/micon/solist-ai/ml63q2500-group

必要なファイル一覧

※ fft.h  mlacc.h  mlacc_odl.h  solistAi.h  SolistAi_Library.a は
　「加速度センサを使った～」のアーカイブに含まれています。

[inc] 19個
can.h            dmac_common.h    mlacc_odl.h      wdt.h
clock.h          fft.h            rdwr_reg.h
crc.h            irq.h            solistAi.h
dmac.h           lp_manage.h      ssiof0.h
dmac0.h          mcu.h            ssiof1.h
dmac1.h          mlacc.h          ssiof_common.h

[lib] 1個
SolistAi_Library.a

[src] 8個
crc.c         irq.c         ssiof1.c
dmac0.c       lp_manage.c   wdt.c
dmac1.c       ssiof0.c
