# ParaSolのAI学習・推論機能の使い方

ロームが公開しているAI処理のサンプルアプリケーションは、利用シーンが不明確で活用イメージが湧きません。
本ドキュメントでは、実機で動作を確認した具体的な事例をもとに、実践的な使い方を解説します。

解説にはParaSolのコマンドを使っています。各コマンドがSolist-AIのどのAPI関数に対応しているかは、
ParaSolのソースにあるcmdTableをご確認いただくと良いでしょう。

※著者はAIのスペシャリストではありません。間違っている部分があるかもしれません。

# 目次
以下の項目は書きかけで、概ね11月中を目途に書き足していく予定です。

1. AIエンジンの初期化と実行パラメータの解説
2. 教師なし学習、異常検知、オートエンコーダー
3. 教師あり学習、分類、予測
4. 時系列データの学習、予測

# 参考サイト
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
