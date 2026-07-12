/* ParaSol
 * FFT_Cmd.c
 *
 *  Created on: 2026/06/07
 *      Author: cocot
 */

#include <solistAi.h>
#include <wdt.h>

#include "cmdCommon.h"
#include "global.h"

static void initFft(void);
static void execFft(void);
//------------------------------------------------------------------------------
void FFT_Cmd(uint8_t cmd) {
  uint8_t cmdGrp = cmd & 0xf0;

  switch(cmd - cmdGrp) {
  case 0:
    initFft();
    break;

  case 1:
    execFft();
    break;

  default:
    setResultCode(UNSUPT_CMD, cmd);
    break;
  }
}
//------------------------------------------------------------------------------
static void initFft(void) {
  if(checkParamBytes(sizeof(uint16_t) + sizeof(uint8_t)))  return;

  uint16_t size   = ((uint16_t)rxBuf->payload[1] << 8) + rxBuf->payload[0];
  uint8_t  window = rxBuf->payload[2];

  // パラメータチェック
  // size
  const uint16_t size_chk[] = {2,4,8,16,32,64,128,256,512,1024,2048};
  int i = 0;
  while(i < (sizeof(size_chk) / sizeof(uint16_t)) ) {
    if(size == size_chk[i]) {
      break;
    }
    i++;
  }
  if(i == (sizeof(size_chk) / sizeof(uint16_t)) ) {
    setResultCode(PARAM_ERR, 1);
    return;
  }

  // window
  if(window != 0 && window != 1) {
    setResultCode(PARAM_ERR, 2);
    return;
  }

  // コマンド実行
  fft_Init(size, window);
  fftSize = size;
}
//------------------------------------------------------------------------------
static void execFft(void) {
  if(fftSize == 0) {
    setResult(UNINITED);
    return;
  }

  // データ数チェック
  if(rxBuf->payloadLength < (fftSize << 1)) {
    setResult(FEW_DATA);
    return;
  }

  if(rxBuf->payloadLength > (fftSize << 1)) {
    setResult(MANY_DATA);
    return;
  }

  // コマンド実行
  fft_Start((fft_t*)&rxBuf->payload, fftSize);
  while(fft_IsBusy()) {
    wdt_clear();
  }

  fft_GetResult((fft_t*)txBuf->payload, fftSize + 2);
  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = fftSize + 2;
}
//------------------------------------------------------------------------------
