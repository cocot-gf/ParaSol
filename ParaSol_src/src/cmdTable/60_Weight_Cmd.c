/* ParaSol
 * Weight_Cmd.c
 *
 *  Created on: 2026/06/08
 *      Author: cocot
 */

#include <solistAi.h>

#include "cmdCommon.h"
#include "global.h"

static void getParameters(void);
static void getModels(void);
static void getMemoryUsage(void);
static void getWeight(bool);
static void setWeight(bool);
static void getReservoir(void);
static void setReservoir(void);
//------------------------------------------------------------------------------
void Weight_Cmd(uint8_t cmd) {
  uint8_t cmdGrp = cmd & 0xf0;

  switch(cmd - cmdGrp) {
  case 0:
    getModels();
    break;

  case 1:
    getMemoryUsage();
    break;

  case 2:
    getParameters();
    break;

  case 3:
    getWeight(true);
    break;

  case 4:
    getWeight(false);
    break;

  case 5:
    getReservoir();
    break;

  case 6:
    setWeight(true);
    break;

  case 7:
    setWeight(false);
    break;

  case 8:
    setReservoir();
    break;

  default:
    setResultCode(UNSUPT_CMD, cmd);
    break;
  }
}
//------------------------------------------------------------------------------
static void getModels(void) {
  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = sizeof(modelCount);
  txBuf->payload[0] = modelCount;
}
//------------------------------------------------------------------------------
static void getMemoryUsage(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(rxBuf->payload[0]);

  uint32_t beta = (param->hiddenSize * param->outputSize) << 1;
  uint32_t p    = (param->hiddenSize * param->hiddenSize) << 1;
  const uint32_t OTHER = p + 16*1024;

  MemUsage* usage = (MemUsage*)txBuf->payload;
  usage->inputSize  = param->inputSize;
  usage->hiddenSize = param->hiddenSize;
  usage->outputSize = param->outputSize;
  usage->aiRam      = p + beta + OTHER;
  usage->betaSize   = beta;
  usage->pSize      = p;
  usage->resSize    = param->hiddenSize << 1;

  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = sizeof(MemUsage);
}
//------------------------------------------------------------------------------
static void getParameters(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(rxBuf->payload[0]);
  Packed_ODL_Parameters* packed = (Packed_ODL_Parameters*)txBuf->payload;

  packed->inputSize          = param->inputSize;
  packed->hiddenSize         = param->hiddenSize;
  packed->outputSize         = param->outputSize;
  packed->forgettingFactor   = param->forgettingFactor;
  packed->activationFunction = param->activationFunction;
  packed->lossFunction       = param->lossFunction;
  packed->seed               = param->seed;
  packed->scaleAlpha         = param->scaleAlpha;
  packed->scaleGamma         = param->scaleGamma;
  packed->leakRate           = param->leakRate;
  packed->l2Param            = param->l2Param;

  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = sizeof(Packed_ODL_Parameters);
}
//------------------------------------------------------------------------------
static void getWeight(bool isBeta) {
  if(checkParamBytes(sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint16_t)))  return;
  if(checkInstanceRange())  return;

  uint32_t offset = *(uint32_t*)&rxBuf->payload[sizeof(uint8_t)];
  uint16_t size   = *(uint16_t*)&rxBuf->payload[sizeof(uint8_t) + sizeof(uint32_t)];

  const uint16_t SIZEMAX = PAYLOADMAX - sizeof(uint8_t) - (txBuf->status.useCrc ? CRCSIZE : 0);
  if(size > SIZEMAX) {  // ペイロードに入るサイズ以上に取得しようとするとサイズを自動で調整する。
    size = SIZEMAX;
  }

  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(rxBuf->payload[0]);
  uint32_t beta = (param->hiddenSize * param->outputSize) << 1;
  uint32_t p    = (param->hiddenSize * param->hiddenSize) << 1;
  uint32_t weightSize = isBeta ? beta : p;

  // 実際に送信するサイズの計算
  if(offset >= weightSize || size == 0) {
    size = 0;
  }
  else if((offset + size) > weightSize) {
    size = weightSize - offset;
  }

  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = size;

  if(size == 0) {
    return;
  }
  else if(isBeta) {
    ODL_GetWeightBeta(txBuf->payload, rxBuf->payload[0], offset, size);
  }
  else {
    ODL_GetWeightP(txBuf->payload, rxBuf->payload[0], offset, size);
  }
}
//------------------------------------------------------------------------------
static void setWeight(bool isBeta) {
  if(checkParamBytes(sizeof(uint8_t) + sizeof(uint32_t)))  return;
  if(checkInstanceRange())  return;

  uint32_t offset = *(uint32_t*)&rxBuf->payload[sizeof(uint8_t)];
  uint16_t size   = rxBuf->payloadLength - sizeof(uint8_t) - sizeof(uint32_t);

  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(rxBuf->payload[0]);
  uint32_t beta = (param->hiddenSize * param->outputSize) << 1;
  uint32_t p    = (param->hiddenSize * param->hiddenSize) << 1;

  uint32_t weightSize = isBeta ? beta : p;

  if(size == 0) {  // 書き込むデータが無い
    return;
  }
  else if((offset + size) > weightSize) {  // 重みサイズより大きい数を書こうとしたとき
    setResult(MANY_DATA);
    return;
  }
  else if(isBeta) {
    ODL_SetWeightBeta(&rxBuf->payload[sizeof(uint8_t) + sizeof(uint32_t)], rxBuf->payload[0], offset, size);
  }
  else {
    ODL_SetWeightP(&rxBuf->payload[sizeof(uint8_t) + sizeof(uint32_t)], rxBuf->payload[0], offset, size);
  }
}
//------------------------------------------------------------------------------
static void getReservoir(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(rxBuf->payload[0]);
  ODL_GetReservoir(rxBuf->payload[0], (bfloat16*)txBuf->payload);

  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = param->hiddenSize << 1;
}
//------------------------------------------------------------------------------
static void setReservoir(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;
  if(checkInstanceRange())  return;

  ODL_Parameters* param = (ODL_Parameters*)ODL_GetParameters(rxBuf->payload[0]);
  uint16_t reqSize = (param->hiddenSize << 1) + 1; // +1はinstanceの指定バイト

  // データ数チェック
  if(rxBuf->payloadLength < reqSize) {
    setResult(FEW_DATA);
    return;
  }

  if(rxBuf->payloadLength > reqSize) {
    setResult(MANY_DATA);
    return;
  }

  ODL_SetReservoir(rxBuf->payload[0], (bfloat16*)&rxBuf->payload[1]);
}
//------------------------------------------------------------------------------
