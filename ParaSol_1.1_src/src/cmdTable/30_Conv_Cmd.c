/* ParaSol
 * Conv_Cmd.c
 *
 *  Created on: 2026/06/08
 *      Author: cocot
 */

#include <solistAi.h>

#include "cmdCommon.h"
#include "global.h"

static void genRandNums(void);
static bool paramCheck_Bf_Int(uint16_t, uint8_t);
static void convInt16_to_Bf16(void);
static void convBf16_to_Int16(void);
static void convBf16_to_Float(void);
//------------------------------------------------------------------------------
void Conv_Cmd(uint8_t cmd) {
  uint8_t cmdGrp = cmd & 0xf0;

  switch(cmd - cmdGrp) {
  case 0:
    genRandNums();
    break;

  case 1:
    convInt16_to_Bf16();
    break;

  case 2:
    convBf16_to_Int16();
    break;

  case 3:
    convBf16_to_Float();
    break;

  default:
    setResultCode(UNSUPT_CMD, cmd);
    break;
  }
}
//------------------------------------------------------------------------------
static void genRandNums(void) {
  if(checkParamBytes(sizeof(uint16_t) + sizeof(uint16_t)))  return;

  // パラメータチェック
  uint16_t n    = ((uint16_t)rxBuf->payload[1] << 8) + rxBuf->payload[0];
  uint16_t seed = ((uint16_t)rxBuf->payload[3] << 8) + rxBuf->payload[2];

  if(n > (txBuf->status.useCrc ? 2045 : 2046)) {
    setResultCode(PARAM_ERR, 1);
    return;
  }

  if(seed == 0 || seed > 32767) {
    setResultCode(PARAM_ERR, 2);
    return;
  }

  ODL_GenerateRandomNumber((bfloat16*)txBuf->payload, n, seed);
  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = n << 1;
}
//------------------------------------------------------------------------------
static bool paramCheck_Bf_Int(uint16_t size, uint8_t qFormat) {  // チェックNGでエラーをセットしてtrueで戻る
  if(checkParamBytes(sizeof(uint8_t)))  return true;

  // パラメータチェック
  if(qFormat > 15) {
    setResultCode(PARAM_ERR, 1);
    return true;
  }

  //データ数が奇数の場合
  if((size & 1) == 1) {
    setResult(FEW_DATA);
    return true;
  }

  return false;
}
//------------------------------------------------------------------------------
static void convInt16_to_Bf16(void) {
  uint16_t size = rxBuf->payloadLength - 1;
  uint8_t qFormat = rxBuf->payload[0];

  if(paramCheck_Bf_Int(size, qFormat))  return;

  // この関数はハードフォルトが起きない
  ODL_ToBfloat16((bfloat16*)txBuf->payload, (int16_t*)&rxBuf->payload[1], qFormat, size >> 1);
  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = size;
}
//------------------------------------------------------------------------------
static void convBf16_to_Int16(void) {
  uint16_t size = rxBuf->payloadLength - 1;
  uint8_t qFormat = rxBuf->payload[0];

  if(paramCheck_Bf_Int(size, qFormat))  return;

  // 入出力の変数アドレスが2バイトアライメントに置かれてないとハードフォルトが起きるため、ダミーバイト0x77を挿入する。
  ODL_Bfloat16ToFixed((int16_t*)&txBuf->payload[1], (bfloat16*)&rxBuf->payload[1], qFormat, size >> 1);
  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = 1 + size;
  txBuf->payload[0] = 0x77;
}
//------------------------------------------------------------------------------
static void convBf16_to_Float(void) {
  //データ数が奇数の場合
  uint16_t size = rxBuf->payloadLength - 1;
  if((size & 1) == 1) {
    setResult(FEW_DATA);
    return;
  }

  // 入出力の変数アドレスが2バイトアライメントに置かれてないとハードフォルトが起きるため、ダミーバイト0x77を挿入する。
  ODL_Bfloat16ToFloat((float*)&txBuf->payload[1], (bfloat16*)&rxBuf->payload[1], size >> 1);
  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = 1 + (size << 1);
  txBuf->payload[0] = 0x77;
}
//------------------------------------------------------------------------------
