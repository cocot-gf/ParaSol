# ParaSol
Converts the Solist-AI™ MCU into an SPI peripheral device.  
Solist-AI™ MCUをSPIペリフェラル・デバイス化するファームウェア

# 1 概要
ParaSol はローム株式会社のマイコンML63Q2500 シリーズに搭載されているSolist-AI™の機能を、
ペリフェラル・デバイスとして利用可能にするファームウェアです。
SPI 通信に対応し、ホストCPU のメーカーや型式に依存せず、データを与えるだけでSolist-AI™の
機能を利用できます。

オプション機能のCRC による誤り検出を使用することで、通信データの誤り・破損を検出し、
信頼性の高いデータ転送を実現できます。待機時の低速クロック動作や省電力スタンバイ機能を利用し、
高い省電力性を実現できます。

ParaSol は株式会社データ・テクノのブレークアウトボード "AIBBY" 向けに開発されています。
別添のマニュアルには、同社の評価ボードDT-EBML63Q2557やローム株式会社のリファレンスボード
RB-D63Q2557TB64をはじめ、48ピンデバイスへ移植するために必要なI/O ポートの割り当て変更方法も
紹介しています。

# 2 特徴
* Solist-AI™ API の全機能に対応
* SPI スレーブ動作 モード0 に対応、最大クロック速度12MHz
* 外部回路を必要としないデュアルポート通信機能
* READY・電源状態通知信号
* CRC32 による誤り検出機能
* 送受信合計16K バイトの大容量通信バッファ
* 待機時は低速クロックで動作、省電力スタンバイ機能を搭載

# 3 動作ブロック図
シングルポート構成
<img width="796" height="375" alt="1" src="https://github.com/user-attachments/assets/70b3839f-94c8-4f12-8c48-a2272a16c8e5" />
  
デュアルポート構成
<img width="796" height="376" alt="2" src="https://github.com/user-attachments/assets/530eb834-6243-4fa1-8ead-435ae3012b8a" />
