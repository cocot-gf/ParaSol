/* ParaSol
 * cmdTable.c
 *
 *  Created on: 2026/06/07
 *      Author: cocot
 *
 */

#include <solistAi.h>
#include "cmdCommon.h"
#include "global.h"

void sysCmd(uint8_t);
void ACC_Cmd(uint8_t);
void FFT_Cmd(uint8_t);
void Conv_Cmd(uint8_t);
void ODL_Cmd(uint8_t);
void OSUAD_Cmd(uint8_t);
void Weight_Cmd(uint8_t);

//------------------------------------------------------------------------------
void cmdTable(void) {
  setResult(SUCCESS);  // デフォルトで戻り値なしの成功を入れておく

  switch(rxBuf->command) {
  case 0x00:  // NOP 予約
  case 0xff:  // NOP 予約
    break;

  case 0x01 ... 0x0f:
    sysCmd(rxBuf->command);
    break;

  case 0x10 ... 0x1f:
    ACC_Cmd(rxBuf->command);
    break;

  case 0x20 ... 0x2f:
    FFT_Cmd(rxBuf->command);
    break;

  case 0x30 ... 0x3f:
    Conv_Cmd(rxBuf->command);
    break;

  case 0x40 ... 0x4f:
    ODL_Cmd(rxBuf->command);
    break;

  case 0x50 ... 0x5f:
    OSUAD_Cmd(rxBuf->command);
    break;

  case 0x60 ... 0x6f:
    Weight_Cmd(rxBuf->command);
    break;

  default:
    setResultCode(UNSUPT_CMD, rxBuf->command);
    break;
  }
}
//------------------------------------------------------------------------------
