# ParaSol PCB
[ParaSol ファームウェア](https://github.com/cocot-gf/ParaSol/tree/a9ea3deae3c0ca9f91adb6652b4928051e767940)用 超小型ML63Q2537 マイコンボード  
<img width="640" height="480" alt="DSC02185" src="https://github.com/user-attachments/assets/0e1c7422-30e9-44f6-b7ec-8ddf61a84a9b" />
<img width="506" height="316" alt="DSC02185" src="https://github.com/user-attachments/assets/85b86bd1-0dae-479d-a0f6-890b7bd1fec3" />


# 1 概要
ParaSol PCBは[ParaSolのファームウェア](https://github.com/cocot-gf/ParaSol/tree/a9ea3deae3c0ca9f91adb6652b4928051e767940)を実行するために設計された、超小型のML63Q2537マイコンボードです。
18mm角の基板を採用し、M5Stack BUSモジュールのような実装面積が限られた小型筐体にも手軽に組み込みが可能です。
基板周囲にはデュアルポート構成のParaSolで使用する通信ポートを引き出し、システムクロック用の水晶振動子をはじめ、
動作に必要なプルアップ・プルダウン抵抗も内蔵しており、システム構築のために細々した周辺部品を用意する必要がありません。  

ParaSol PCB のマイコンには出荷時点の最新版ParaSol ファームウェアが書き込まれており、購入後すぐに使用できます。  

# 2 特徴
* 18mm角の基板、600mil幅、14ピンDIPスタイルのピン配置
* 48MHz PLL発振用の水晶振動子を搭載
* デュアルポート構成の最新版ParaSolファームウェアを書込み済み
* SWDピンあり、汎用小型マイコンボードとしても利用可能
* ワイドレンジの電源電圧2.5～5V

# 3 利用方法
基板のピン配置などの仕様は[ParaSolPCB_ProductManual.pdf](https://github.com/cocot-gf/ParaSol/blob/86225ecf893b6cf2e4ed778cf5b3287f270a0bf2/ParaSolPCB/ParaSolPCB_1.0_ProductManual.pdf) を読んでください。  
Solist-AIの機能を含めたParaSolのソフト的な使い方は、親フォルダにある[ParaSol_ProductManual.pdf](https://github.com/cocot-gf/ParaSol/blob/b48c929ddfc375fc4ac8e3bb0690078d2fe2760e/ParaSol_1.5_ProductManual_AMDT3.pdf) を読んでください。  
[binaryツリー](https://github.com/cocot-gf/ParaSol/tree/213b270182fe4c7482088d16837ffaf44c1fdd3f/binary)には、この基板にインストールされている最新版のコンパイル済みバイナリデータも掲載しています。
  
# 4 ケース内部に貼ってあるシールについて
<img width="400" height="325" alt="DSC02199" src="https://github.com/user-attachments/assets/c5610b88-8012-4110-866a-c50c5df5387c" />

このシールに書かれている数字は、動作確認とシリアル番号の管理のためにGet Versionコマンドで取得したUID(ユニークID)です。
ファームウェアを書き込み後、全数通電検査を行っています。  
UIDは半導体メーカーが書き込んでいる情報なので、重複することはありません。

# 5 技術的な問い合わせ
技術的な問い合わせはメールで受け付けています。

gadget.factory.mail@gmail.com <または> gadget_factory@bf.wakwak.com
