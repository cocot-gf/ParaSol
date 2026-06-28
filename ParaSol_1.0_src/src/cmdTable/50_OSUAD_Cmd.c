/* ParaSol
 * OSUAD_Cmd.c
 *
 *  Created on: 2026/06/08
 *      Author: cocot
 */

#include <solistAi.h>

#include "cmdCommon.h"
#include "global.h"

static void initOsuad(void);
static void resetOsuadTrain(void);
static void startOsuadTrainPredict(bool);
static void getOsuadLoss(void);
//------------------------------------------------------------------------------
void OSUAD_Cmd(uint8_t cmd) {
  uint8_t cmdGrp = cmd & 0xf0;

  switch(cmd - cmdGrp) {
  case 0:
    initOsuad();
    break;

  case 1:
    resetOsuadTrain();
    break;

  case 2:
    startOsuadTrainPredict(false);
    break;

  case 3:
    startOsuadTrainPredict(true);
    break;

  case 4:
    getOsuadLoss();
    break;

  default:
    setResultCode(UNSUPT_CMD, cmd);
    break;
  }
}
//------------------------------------------------------------------------------
static void initOsuad(void) {
  if(checkParamBytes(sizeof(uint8_t) + sizeof(Packed_ODL_Parameters)))  return;

  // setModelsしていない
  if(modelCount == 0) {
    setResult(UNINITED);
    return;
  }

  // インスタンス範囲チェック
  if(rxBuf->payload[0] >= (modelCount + 1)) {
    setResultCode(PARAM_ERR, 1);
    return;
  }

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
  bool result = OSUAD_Initialize(&param, rxBuf->payload[0]);
  if(! result) {
    setResult(FAILED);
  }
  else {
    lastExecInstance = 0; // 学習・推論未実施
  }
}
//------------------------------------------------------------------------------
static void resetOsuadTrain(void) {
  // setModelsしていない
  if(modelCount == 0) {
    setResult(UNINITED);
  }

  OSUAD_Reset();
  lastExecInstance = 0; // 学習・推論未実施
}
//------------------------------------------------------------------------------
static void startOsuadTrainPredict(bool isPredict) {
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

  if(isPredict) {
    ODL_StartPredict(instance, NULL, NULL);
  }
  else {
    ODL_StartTrain(instance, NULL, NULL);
  }

  lastExecInstance = instance + 1;
}
//------------------------------------------------------------------------------
static void getOsuadLoss(void) {
  // setModelsしていない
  if(modelCount == 0) {
    setResult(UNINITED);
  }

  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = sizeof(bfloat16);
  *(bfloat16*)&txBuf->payload = OSUAD_GetLoss();
}
//------------------------------------------------------------------------------
