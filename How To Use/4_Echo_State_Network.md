# Echo State Network(ESN)を使った時系列データの学習
書きかけです。

## ESN向けの初期化手順

| パラメータ | 設定値 |
|-|-|
|inputSize|キーになるデータセットのサイズに合わせて任意。|
|hiddenSize|必要RAMサイズと計算時間が許す範囲で165以下の大きめの値。<br>少なすぎると再現性が減り、大きすぎると必要RAMと計算時間が飛躍的に増大する。|
|outputSize|結果として得たいデータセットのサイズ。|
|forgettingFactor|1|
|activationFunction|任意、3(tanh)から試すのがオススメ。|
|lossFunction|任意|
|seed|任意|
|scaleAlpha|0(Auto)か、とりあえず LN(10^15) / inputSize|
|scaleGamma|0以外の任意、とりあえず <img width="108" height="37" alt="aa" src="https://github.com/user-attachments/assets/f6caaf4b-4656-4ecd-828e-2f639aa901b1" /> からスタート。|
|leakRate|任意、0.5からスタート。|
|L2Param|0または任意、0.5～10あたりを入れると再現性・学習速度・発散耐性が上がり、hiddenSizeを大幅に縮小できる。|
