/* ParaSol
 * ODL_Cmd.c
 *
 *  Created on: 2026/06/08
 *      Author: cocot
 */

#include <solistAi.h>

#include "cmdCommon.h"
#include "global.h"

static void setModels(void);
static void odlInit(void);
static void odlReset(void);
static void odlResetReservoir(void);
static void odlStartTrainPredict_XT(bool);
static void odlStartTrainPredict_X(bool);
static void transfer_X(void);
static void odlStartTrainPredict_T(bool);
static void odlStartTrainPredict(bool);
static void odlGetResult(void);
static void odlGetLoss(bool);
//------------------------------------------------------------------------------
void ODL_Cmd(uint8_t cmd) {
  uint8_t cmdGrp = cmd & 0xf0;

  switch(cmd - cmdGrp) {
  case 0:  // モデル数設定
    setModels();
    break;

  case 1:  // インスタンス初期化
    odlInit();
    break;

  case 2:  // 学習リセット
    odlReset();
    break;

  case 3:  // リザバーリセット
    odlResetReservoir();
    break;

  case 4:  // XTを使って学習
    odlStartTrainPredict_XT(false);
    break;

  case 5:  // XTを使って推論
    odlStartTrainPredict_XT(true);
    break;

  case 6:  // Xを使って学習
    odlStartTrainPredict_X(false);
    break;

  case 7:  // Xを使って推論
    odlStartTrainPredict_X(true);
    break;

  case 8:  // Xを中間Xバッファへ転送
    transfer_X();
    break;

  case 9:  // 中間XバッファとTを使って学習
    odlStartTrainPredict_T(false);
    break;

  case 10:  // 中間XバッファとTを使って推論
    odlStartTrainPredict_T(true);
    break;

  case 11:  // 中間Xバッファを使って学習
    odlStartTrainPredict(false);
    break;

  case 12:  // 中間Xバッファを使って推論
    odlStartTrainPredict(true);
    break;

  case 13:  // 最後に推論したインスタンスのYを取得
    odlGetResult();
    break;

  case 14:  // lossをbf16で取得
    odlGetLoss(false);
    break;

  case 15:  // lossをfloatで取得
    odlGetLoss(true);
    break;

  default:
    setResultCode(UNSUPT_CMD, cmd);
    break;
  }
}
//------------------------------------------------------------------------------
static void setModels(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;

  uint8_t models = rxBuf->payload[0];

  // パラメータチェック
  if(models == 0 || models > MODELSMAX) {
    setResultCode(PARAM_ERR, 1);
    return;
  }

  bool result = ODL_SetModelCount(models);
  if(! result) {
    setResult(FAILED);
  }
  else {
    modelCount = models;  // 初期化できたらモデル数管理値を変更
    lastPredictInstance = 0; // 推論未実施
  }
}
//------------------------------------------------------------------------------
static void odlInit(void) {
  if(checkParamBytes(sizeof(uint8_t) + sizeof(Packed_ODL_Parameters)))  return;
  if(checkInstanceRange())  return;

  Packed_ODL_Parameters* packed = (Packed_ODL_Parameters*)&rxBuf->payload[1];
  ODL_Parameters param;

  param.inputSize          = packed->inputSize;
  param.hiddenSize         = packed->hiddenSize;
  param.outputSize         = packed->outputSize;
  param.forgettingFactor   = packed->forgettingFactor;
  param.activationFunction = packed->activationFunction;
  param.lossFunction       = packed->lossFunction;
  param.seed               = packed->seed;
  param.scaleAlpha         = packed->scaleAlpha;
  param.scaleGamma         = packed->scaleGamma;
  param.leakRate           = packed->leakRate;
  param.l2Param            = packed->l2Param;

  // 構造体のパラメータチェックはAPIに任せる
  bool result = ODL_Initialize(rxBuf->payload[0], &param);
  if(! result) {
    setResult(FAILED);
  }
  else {
    if(rxBuf->payload[0] <= (lastPredictInstance - 1)) {
      lastPredictInstance = 0;  // 推論後のインスタンスと同じか若いものを初期化したら推論未実施に戻す
    }
  }
}
//------------------------------------------------------------------------------
static void odlReset(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  ODL_Reset(rxBuf->payload[0]);
}
//------------------------------------------------------------------------------
static void odlResetReservoir(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  ODL_ResetReservoir(rxBuf->payload[0]);
}
//------------------------------------------------------------------------------
static void odlStartTrainPredict_XT(bool isPredict) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  uint8_t instance = rxBuf->payload[0];
  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(instance);
  uint16_t reqSize = ((param->inputSize + param->outputSize) << 1) + 1; // +1はinstanceの指定バイト

  // データ数チェック
  if(rxBuf->payloadLength < reqSize) {
    setResult(FEW_DATA);
    return;
  }

  if(rxBuf->payloadLength > reqSize) {
    setResult(MANY_DATA);
    return;
  }

  aiTransferDMA(&rxBuf->payload[1], (void*)ODL_GetInputAddress(), param->inputSize << 1);

  if(isPredict) {
    ODL_StartPredict(instance, NULL, (bfloat16*)&rxBuf->payload[1 + (param->inputSize << 1)]);
    lastPredictInstance = instance + 1;
  }
  else {
    ODL_StartTrain(instance, NULL, (bfloat16*)&rxBuf->payload[1 + (param->inputSize << 1)]);
  }

  while(ODL_IsBusy()) {
  }
}
//------------------------------------------------------------------------------
// 引数Xだけで学習・推論する
static void odlStartTrainPredict_X(bool isPredict) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  uint8_t instance = rxBuf->payload[0];
  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(instance);
  uint16_t reqSize = (param->inputSize << 1) + 1; // +1はinstanceの指定バイト

  // データ数チェック
  if(rxBuf->payloadLength < reqSize) {
    setResult(FEW_DATA);
    return;
  }

  if(rxBuf->payloadLength > reqSize) {
    setResult(MANY_DATA);
    return;
  }

  aiTransferDMA(&rxBuf->payload[1], (void*)ODL_GetInputAddress(), param->inputSize << 1);

  if(isPredict) {
    ODL_StartPredict(instance, NULL, NULL);
    lastPredictInstance = instance + 1;
  }
  else {
    ODL_StartTrain(instance, NULL, NULL);
  }

  while(ODL_IsBusy()) {
  }
}
//------------------------------------------------------------------------------
// 中間Xバッファへデータ転送する
static void transfer_X(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  uint8_t instance = rxBuf->payload[0];
  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(instance);
  uint16_t reqSize = (param->inputSize << 1) + 1; // +1はinstanceの指定バイト

  // データ数チェック
  if(rxBuf->payloadLength < reqSize) {
    setResult(FEW_DATA);
    return;
  }

  if(rxBuf->payloadLength > reqSize) {
    setResult(MANY_DATA);
    return;
  }

  aiTransferDMA(&rxBuf->payload[1], (void*)ODL_GetInputAddress(), param->inputSize << 1);
}
//------------------------------------------------------------------------------
// 中間Xバッファと引数Tを使用して学習・推論する
static void odlStartTrainPredict_T(bool isPredict) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  uint8_t instance = rxBuf->payload[0];
  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(instance);
  uint16_t reqSize = (param->outputSize << 1) + 1; // +1はinstanceの指定バイト

  // データ数チェック
  if(rxBuf->payloadLength < reqSize) {
    setResult(FEW_DATA);
    return;
  }

  if(rxBuf->payloadLength > reqSize) {
    setResult(MANY_DATA);
    return;
  }

  if(isPredict) {
    ODL_StartPredict(instance, NULL, (bfloat16*)&rxBuf->payload[1]);
    lastPredictInstance = instance + 1;
  }
  else {
    ODL_StartTrain(instance, NULL, (bfloat16*)&rxBuf->payload[1]);
  }

  while(ODL_IsBusy()) {
  }
}
//------------------------------------------------------------------------------
// 中間Xバッファを使用して学習・推論する
static void odlStartTrainPredict(bool isPredict) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  uint8_t instance = rxBuf->payload[0];

  if(isPredict) {
    ODL_StartPredict(instance, NULL, NULL);
    lastPredictInstance = instance + 1;
  }
  else {
    ODL_StartTrain(instance, NULL, NULL);
  }

  while(ODL_IsBusy()) {
  }
}
//------------------------------------------------------------------------------
// 最後に実行したYを取得
// DMAを使ったYの取り出しは、呼び出し元が取り出しサイズを決定しなければならず、バッファは全インスタンスで共有されているため、事前に推論していないと取り出せない。
// 初期化関数を実行した後はYサイズが変更されている可能性があるため、未推論状態に戻る。
// https://esh.rohm.co.jp/s/question/0D5RC00001eCM8m0AG/ai%E5%87%BA%E5%8A%9B%E7%B5%90%E6%9E%9C%E3%81%AE%E5%8F%96%E5%BE%97%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6?language=ja
static void odlGetResult(void) {
  if(lastPredictInstance == 0) {
    setResult(UNINITED);
    return;
  }

  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(lastPredictInstance - 1);
  uint16_t ySize = param->outputSize << 1;

  aiTransferDMA((void*)ODL_GetOutputAddress(), txBuf->payload, ySize);
  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = ySize;
}
//------------------------------------------------------------------------------
// 指定したinstanceのlossを取得
static void odlGetLoss(bool returnByFloat) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  txBuf->status.result = SUCC_VALUE;

  if(returnByFloat) {
    txBuf->payloadLength = sizeof(float);
    *(float*)&txBuf->payload = ODL_GetLossAsFloat(rxBuf->payload[0]);
  }
  else {
    txBuf->payloadLength = sizeof(bfloat16);
    *(bfloat16*)&txBuf->payload = ODL_GetLoss(rxBuf->payload[0]);
  }
}
//------------------------------------------------------------------------------
