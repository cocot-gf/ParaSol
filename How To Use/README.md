# ParaSolのAI学習・推論機能の使い方

ParaSolの説明書には「各コマンドの利用方法や使用する順番は、ロームのサンプルプログラムを参照すること。」と記載しておりましたが、サンプルプログラムは登録者しか入手できないためParaSolを作ったという経緯があり、説明の辻褄が合いません。

そのため、本ドキュメントでは、実機で動作を確認した具体的な事例をもとに、実践的な使い方を解説します。
解説にはParaSolのコマンドを使っています。各コマンドがSolist-AIのどのAPI関数に対応しているかは、
ParaSolのソースにあるcmdTableをご確認いただくと良いでしょう。

Solist-AIの内部アルゴリズムは公開されていませんが、ベースになっているのは[慶應義塾大学 松谷研究室](https://www.arc.ics.keio.ac.jp/research)のアルゴリズムだということがロームの資料に記載されています。興味がある方は検索してみてください。


※著者はAIのスペシャリストではありません。間違っている部分があるかもしれません。

# 目次
1. [AIエンジンの初期化手順と初期化パラメータの解説](https://github.com/cocot-gf/ParaSol/blob/Ver.1.5-RELEASE/How%20To%20Use/1_Initialize.md)
2. Solist-AIの学習・推論でできること
3. 教師なし学習：未知の異常検知、分類
4. 教師あり学習：分類、既知の異常検知、予測
5. 時系列データの学習：予測

上記は書く予定の項目で、リンクが張られていないものは概ね11月中を目途に完成させる予定です。

# AI一般についての参考サイト
[2026年版機械学習ガイド - IBM](https://www.ibm.com/jp-ja/think/machine-learning)  

### 学習方法
[ニューラル・ネットワークとは](https://www.ibm.com/jp-ja/think/topics/neural-networks)  
[教師なし学習とは](https://www.ibm.com/jp-ja/think/topics/unsupervised-learning)  
[半教師あり学習とは](https://www.ibm.com/jp-ja/think/topics/semi-supervised-learning)  
[自己教師あり学習とは](https://www.ibm.com/jp-ja/think/topics/self-supervised-learning)  
[教師あり学習とは](https://www.ibm.com/jp-ja/think/topics/supervised-learning)  
[オートエンコーダーとは](https://www.ibm.com/jp-ja/think/topics/autoencoder)  
[線形回帰とは](https://www.ibm.com/jp-ja/think/topics/linear-regression)  
[時系列モデルとは](https://www.ibm.com/jp-ja/think/topics/time-series-model)  
[リカレント・ニューラル・ネットワーク（RNN）とは](https://www.ibm.com/jp-ja/think/topics/recurrent-neural-networks)

### 評価・最適化
[損失関数とは](https://www.ibm.com/jp-ja/think/topics/loss-function)  
[正則化とは](https://www.ibm.com/jp-ja/think/topics/regularization)  
[Lasso回帰とは](https://www.ibm.com/jp-ja/think/topics/lasso-regression)  
[リッジ回帰とは](https://www.ibm.com/jp-ja/think/topics/ridge-regression)  

[過剰適合とは](https://www.ibm.com/jp-ja/think/topics/overfitting)  
[過小適合とは](https://www.ibm.com/jp-ja/think/topics/underfitting)  
[過剰適合と過小適合：最適なバランスを見つける](https://www.ibm.com/jp-ja/think/topics/overfitting-vs-underfitting)  
[モデル・チューニングとは](https://www.ibm.com/jp-ja/think/topics/model-tuning)  
[ハイパーパラメーターのチューニングとは](https://www.ibm.com/jp-ja/think/topics/hyperparameter-tuning)  
