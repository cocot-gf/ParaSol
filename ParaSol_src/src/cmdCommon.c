/* ParaSol
 * cmdCommon.c
 *
 *  Created on: 2026/06/18
 *      Author: cocot
 */

#include <wdt.h>
#include <dmac.h>
#include <dmac0.h>

#include "global.h"

//------------------------------------------------------------------------------
void setResult(ResultCode result) {
  txBuf->status.result = result;
  txBuf->payloadLength = 0;
}
//------------------------------------------------------------------------------
void setResultCode(ResultCode result, uint8_t code) {
  txBuf->status.result = result;
  txBuf->payloadLength = 1;
  txBuf->payload[0] = code;
}
//------------------------------------------------------------------------------
float bf16ToFloat(bfloat16 bf16) {
  union
  {
      uint32_t u32;
      float    f;
  } conv;

  conv.u32 = ((uint32_t)bf16) << 16;
  return conv.f;
}
//------------------------------------------------------------------------------
bool checkParamBytes(int size) { // チェックNGならエラーをセットしてtrueで戻る
  if(rxBuf->payloadLength < size) {
    setResult(FEW_PARAM);
    return true;
  }
  return false;
}
//------------------------------------------------------------------------------
bool checkInstanceRange(void) {
  // setModelsしていない
  if(modelCount == 0) {
    setResult(UNINITED);
    return true;
  }

  // インスタンス範囲チェック
  if(rxBuf->payload[0] >= modelCount) {
    setResultCode(PARAM_ERR, 1);
    return true;
  }
  return false;
}
//------------------------------------------------------------------------------
void aiTransferDMA(void* src, void* dst, uint32_t size) {
  dmac0_init(DMAC_TMOD_ARQ_AUTO | DMAC_TMOD_SDP_INC | DMAC_TMOD_DDP_INC | DMAC_TMOD_TSIZ_BYTE
           | DMAC_TMOD_BRQ_BURST | DMAC_TMOD_IMK_IMASK, DMAC_REQ_NOTSEL, (void*)0 );
  dmac0_Transfer(src, dst, size);
  dmac0_enable();
  while(dmac0_checkEnd()) {
    wdt_clear();
  }
  dmac0_disable();
}
//------------------------------------------------------------------------------
