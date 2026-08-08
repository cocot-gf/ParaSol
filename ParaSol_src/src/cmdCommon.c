/* ParaSol
 * cmdCommon.c
 *
 *  Created on: 2026/06/18
 *      Author: cocot
 */

#include <dmac.h>
#include <dmac0.h>
#include <wdt.h>

#include "global.h"
//------------------------------------------------------------------------------
// ペイロードが空の結果コードを設定する
void setResult(ResultCode result) {
  txBuf->status.result = result;
  txBuf->payloadLength = 0;
}
//------------------------------------------------------------------------------
// 結果コードとペイロードサイズを設定する
void setResultCode(ResultCode result, uint8_t code) {
  txBuf->status.result = result;
  txBuf->payloadLength = 1;
  txBuf->payload[0] = code;
}
//------------------------------------------------------------------------------
// bf16からfloatへの変換
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
// パラメータバイトがすべて届いているかのチェック
// チェックNGなら結果コードをセットしてtrueで戻る
bool checkParamBytes(int size) {
  if(rxBuf->payloadLength < size) {
    setResult(FEW_PARAM);
    return true;
  }
  return false;
}
//------------------------------------------------------------------------------
// instanceパラメータがmodelCount以内に入っているかのチェック
// チェックNGなら結果コードをセットしてtrueで戻る
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
// 入力X,出力Y転送用DMAコマンド
void aiTransferDMA(void* src, void* dst, uint32_t size) {
  ML_ACC_UnprotectMemory();

  dmac0_init(DMAC_TMOD_ARQ_AUTO | DMAC_TMOD_SDP_INC | DMAC_TMOD_DDP_INC | DMAC_TMOD_TSIZ_BYTE
           | DMAC_TMOD_BRQ_BURST | DMAC_TMOD_IMK_IMASK, DMAC_REQ_NOTSEL, (void*)0 );
  dmac0_Transfer(src, dst, size);
  dmac0_enable();
  while(dmac0_checkEnd()) {
    wdt_clear();
  }
  dmac0_disable();

  ML_ACC_ProtectMemory();
}
//------------------------------------------------------------------------------
