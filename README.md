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

ParaSol は株式会社データ・テクノの[ブレークアウトボード "AIBBY"](https://www.datatecno.co.jp/prod_info/aibby/) 向けに開発されています。
別添のマニュアルには、同社の[評価ボードDT-EBML63Q2557](https://www.datatecno.co.jp/prod_info/solistai_board/)や
ローム株式会社の[リファレンスボードRB-D63Q2557TB64](https://ros.rohm.co.jp/product/rbd63q2557tb64/01tRC00000BcvYPYAZ)をはじめ、
48ピンデバイスへ移植するために必要なI/O ポートの割り当て変更方法も紹介しています。

# 2 特徴
* Solist-AI™ API の全機能に対応
* SPI スレーブ動作 モード0 に対応、最大クロック速度12MHz
* 外部回路を必要としないデュアルポート通信機能
* READY・電源状態通知信号
* CRC32 による誤り検出機能
* 送受信合計16K バイトの大容量通信バッファ
* 待機時は低速クロックで動作、省電力スタンバイ機能を搭載

# 3 動作ブロック図
## シングルポート構成
シングルポート構成は、SPIマスターになるデバイスとParaSolが1対1で通信する想定をしています。READYとLED信号をGPIOなどで読むと、
ParaSolが処理中などでSPI通信ができない場合の状態を判定可能です。
  
<img width="796" height="375" alt="1" src="https://github.com/user-attachments/assets/70b3839f-94c8-4f12-8c48-a2272a16c8e5" />
  
## デュアルポート構成
デュアルポート構成は、SPIマスターになるデバイスが2台あって別々のバスに分かれている場合を想定し、プライマリとセカンダリを切り替えながら通信します。
プライマリはメインCPUから、セカンダリはPCなどをつないでAIの重みデータを書き込んだりするのが当初の想定です。
  
<img width="796" height="376" alt="2" src="https://github.com/user-attachments/assets/530eb834-6243-4fa1-8ead-435ae3012b8a" />

# 4 プログラム利用方法
使い方はParaSol_ProductManual.pdfを読んでください。  
ParaSol_srcツリーにはソースファイル一式が含まれています。  
プロジェクトのインポートに必要なドットファイルがアップロードできないので、zipでもソースをアップロードしてあります。

ソースのコンパイルに必要なツールはロームのサイトからダウンロードできますが、ロームの評価ボードを購入しないとダウンロード権が得られません。

なので、binaryフォルダにコンパイル済みのファイルを置いてあります。
hexとelfの中身は実質同じなので、Armマイコンに対応したライティングソフトでCPUに書き込めばすぐに動きます。
  
[![Youtube](https://github.com/user-attachments/assets/021a77d3-e82f-4512-aa27-0bdfd01b35eb)](https://youtu.be/9uAWkHuoVFw)

# 5 応用例
ML63Q2537 48ピンデバイスにデュアルポート構成のプログラムを書き込み、M5Stack用BUSモジュールに内蔵させた応用例です。
  
<img width="600" height="599" alt="g" src="https://github.com/user-attachments/assets/68597ea7-1185-4626-af2f-329df97a164a" />
<img width="600" height="586" alt="b" src="https://github.com/user-attachments/assets/fcd459e4-e02b-4d8e-b177-f3aa03a58ff1" />
<img width="600" height="588" alt="c" src="https://github.com/user-attachments/assets/c865f02c-99cd-4107-84be-65bba7541e5b" />
