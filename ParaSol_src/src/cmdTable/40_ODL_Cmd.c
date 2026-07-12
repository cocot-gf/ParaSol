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
static void initOdl(void);
static void resetOdlTrain(void);
static void resetReservoir(void);
static void startOdlTrainPredict(bool);
static void transfer_X(void);
static void startOdlTrainPredict_T(bool);
static void getResult(void);
static void getOdlLoss(bool);
//------------------------------------------------------------------------------
void ODL_Cmd(uint8_t cmd) {
  uint8_t cmdGrp = cmd & 0xf0;

  switch(cmd - cmdGrp) {
  case 0:
    setModels();
    break;

  case 1:
    initOdl();
    break;

  case 2:
    resetOdlTrain();
    break;

  case 3:
    resetReservoir();
    break;

  case 4:
    startOdlTrainPredict(false);
    break;

  case 5:
    startOdlTrainPredict(true);
    break;

  case 6:
    transfer_X();
    break;

  case 7:
    startOdlTrainPredict_T(false);
    break;

  case 8:
    startOdlTrainPredict_T(true);
    break;

  case 9:
    getResult();
    break;

  case 10:
    getOdlLoss(false);
    break;

  case 11:
    getOdlLoss(true);
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
    lastExecInstance = 0; // 学習・推論未実施
  }
}
//------------------------------------------------------------------------------
static void initOdl(void) {
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
    if(rxBuf->payload[0] <= (lastExecInstance - 1)) {
      lastExecInstance = 0;  // 学習・推論後のインスタンスと同じか若いものを初期化したら未実行状態に戻す
    }
  }
}
//------------------------------------------------------------------------------
static void resetOdlTrain(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  ODL_Reset(rxBuf->payload[0]);
  if(rxBuf->payload[0] == (lastExecInstance - 1)) {
    lastExecInstance = 0;  // 学習・推論後に初期化したら未実行状態に戻す
  }
}
//------------------------------------------------------------------------------
static void resetReservoir(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  ODL_ResetReservoir(rxBuf->payload[0]);
}
//------------------------------------------------------------------------------
static void startOdlTrainPredict(bool isPredict) {
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

  ML_ACC_UnprotectMemory();
  aiTransferDMA(&rxBuf->payload[1], (void*)ODL_GetInputAddress(), param->inputSize << 1);
  ML_ACC_ProtectMemory();

  if(isPredict) {
    ODL_StartPredict(instance, NULL, (bfloat16*)&rxBuf->payload[1 + (param->inputSize << 1)]);
  }
  else {
    ODL_StartTrain(instance, NULL, (bfloat16*)&rxBuf->payload[1 + (param->inputSize << 1)]);
  }

  lastExecInstance = instance + 1;
}
//------------------------------------------------------------------------------
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

  ML_ACC_UnprotectMemory();
  aiTransferDMA(&rxBuf->payload[1], (void*)ODL_GetInputAddress(), param->inputSize << 1);
  ML_ACC_ProtectMemory();
}
//------------------------------------------------------------------------------
static void startOdlTrainPredict_T(bool isPredict) {
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
  }
  else {
    ODL_StartTrain(instance, NULL, (bfloat16*)&rxBuf->payload[1]);
  }

  lastExecInstance = instance + 1;
}
//------------------------------------------------------------------------------
static void getResult(void) {
  // 直前に学習・推論していない
  if(lastExecInstance == 0) {
    setResult(UNINITED);
    return;
  }

  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(lastExecInstance + 1);
  uint16_t ySize = param->outputSize << 1;

  ML_ACC_UnprotectMemory();
  aiTransferDMA((void*)ODL_GetOutputAddress(), txBuf->payload, ySize);
  ML_ACC_ProtectMemory();
  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = ySize;
}
//------------------------------------------------------------------------------
static void getOdlLoss(bool returnByFloat) {
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
