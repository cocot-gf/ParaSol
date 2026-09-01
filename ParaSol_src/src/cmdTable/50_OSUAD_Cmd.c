/* ParaSol
 * OSUAD_Cmd.c
 *
 *  Created on: 2026/06/08
 *      Author: cocot
 */

#include <solistAi.h>

#include "cmdCommon.h"
#include "global.h"

static void osuadInit(void);
static void osuadReset(void);
static void osuadGetLoss(void);
//------------------------------------------------------------------------------
void OSUAD_Cmd(uint8_t cmd) {
  uint8_t cmdGrp = cmd & 0xf0;

  switch(cmd - cmdGrp) {
  case 0:  // 全インスタンスを初期化
    osuadInit();
    break;

  case 1:  // 全学習をリセット
    osuadReset();
    break;

  case 2:  // 全インスタンスの中で一番小さいlossを取得
    osuadGetLoss();
    break;

  default:
    setResultCode(UNSUPT_CMD, cmd);
    break;
  }
}
//------------------------------------------------------------------------------
static void osuadInit(void) {
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
    lastPredictInstance = 0; // 推論未実施
  }
}
//------------------------------------------------------------------------------
static void osuadReset(void) {
  // setModelsしていない
  if(modelCount == 0) {
    setResult(UNINITED);
  }

  OSUAD_Reset();
}
//------------------------------------------------------------------------------
static void osuadGetLoss(void) {
  // setModelsしていない
  if(modelCount == 0) {
    setResult(UNINITED);
  }

  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = sizeof(bfloat16);
  *(bfloat16*)&txBuf->payload = OSUAD_GetLoss();
}
//------------------------------------------------------------------------------
