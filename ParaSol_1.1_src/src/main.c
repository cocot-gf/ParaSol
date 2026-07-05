/* ParaSol
 * main.c
 *
 *  Created on: 2026/06/06
 *      Author: cocot
 *
 *  コンパイラのプリプロセッサオプションでDUALを宣言するとデュアルポート構成でコンパイルされる。
 */

#include <wdt.h>
#include <lp_manage.h>
#include <clock.h>

#include "setup.h"
#include "spiTransfer.h"
#include "crcChecker.h"
#include "cmdCommon.h"
#include "cmdTable.h"
#include "global.h"

static bool frameCheck(void);
//------------------------------------------------------------------------------
int main(void) {
  setup();

  while(1) {
    beginTransfer();

    while(! occurEXI) { // 通信終了割り込みを待機
      wdt_clear();
      lp_setHaltMode();   // デバッガを接続しているとマイコンの仕様で十分な省電力状態にならない
    }

    setResult(FAILED);
    if(! frameCheck()) {
      cmdTable();
    }
  }
}
//------------------------------------------------------------------------------
static bool frameCheck(void) { // チェックOKならfalse
  // フレームオーバー
  if(xferBytes == BUFFERSIZE) {
    setResult(FRAME_OVR);
    return true;
  }

  // payloadLengthの妥当性
  if(rxBuf->payloadLength > (BUFFERSIZE - 1 - HEADERSIZE - (txBuf->status.useCrc ? CRCSIZE : 0) ) ) {
    setResult(WRONG_PLEN);
    return true;
  }

  // payloadLengthより実際の転送バイト数が少ない
  if(rxBuf->payloadLength > (xferBytes - HEADERSIZE - (txBuf->status.useCrc ? CRCSIZE : 0) ) ) {
    setResult(SHORT_PL);
    return true;
  }

  // 受信CRC
  if(txBuf->status.useCrc && checkCrc(rxBuf, xferBytes)) {
    setResult(WRONG_CRC);
    return true;
  }

  return false;
}
//------------------------------------------------------------------------------
