/* ParaSol
 * sysCmd.c
 *
 *  Created on: 2026/06/07
 *      Author: cocot
 */

#include <string.h>
#include <clock.h>
#include <solistAi.h>
#include <lp_manage.h>
#include <wdt.h>
#include <irq.h>

#include "setup.h"
#include "crcChecker.h"
#include "cmdCommon.h"
#include "global.h"

static void setIdleClock(void);
static void getVersion(void);
static void calcPayloadCrc(void);
static void enterStop(void);
static void changeAccessPort(uint8_t);
//------------------------------------------------------------------------------
void sysCmd(uint8_t cmd) {
  uint8_t cmdGrp = cmd & 0xf0;

  switch(cmd - cmdGrp) {
  case 1:  // CRCチェックOFF、ONから戻すときは01 0000 B6BCD187
    txBuf->status.useCrc = false;
    break;

  case 2:  // CRCチェックON
    txBuf->status.useCrc = true;
    break;

  case 3:  // アイドルクロック設定
    setIdleClock();
    break;

  case 4:  // 省電力スタンバイ
    enterStop();
    break;

  case 5:  // バージョン情報
    getVersion();
    break;

  case 6:  // ペイロードのCRCを計算
    calcPayloadCrc();
    break;

  case 7:  // デバッグ用、コマンド以外をリードバック、CRCは再計算
    txBuf->status.result = SUCC_VALUE;
    memcpy(&txBuf->payloadLength, &rxBuf->payloadLength, BUFFERSIZE - 1);
    break;

  case 8:  // DUALPORT
    changeAccessPort(cmd);
    break;

  default:
    setResultCode(UNSUPT_CMD, cmd);
    break;
  }
}
//------------------------------------------------------------------------------
static void setIdleClock(void) {
  if(checkParamBytes(sizeof(uint8_t)))  return;

  if(rxBuf->payload[0] > CLK_SYSC_OSCLK_DIV32) {
    setResultCode(PARAM_ERR, 1);
    return;
  }

  idleClock = rxBuf->payload[0];
}
//------------------------------------------------------------------------------
static void enterStop(void) {
  wdt_clear();
  clear_bit(LEDPORT, LED);
  set_bit(READYPORT, nREADY);
  lp_setStopMode();  // デバッガを接続しているとマイコンの仕様でSTOPに入らない

  // Wakeup
  set_bit(LEDPORT, LED);
  clear_bit(READYPORT, nREADY);
  clear_bit(SCB->SCR, 4); // SLEEPDEEPビットをオフにしないとHALTに入るときにPLLが止まる
  clk32kPLL48M();
}
//------------------------------------------------------------------------------
static void getVersion(void) {
  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = 4;
  txBuf->payload[0] = MINOR;
  txBuf->payload[1] = MAJOR;
  *(uint16_t*)&txBuf->payload[2] = SolistAi_GetVersion();
}
//------------------------------------------------------------------------------
static void calcPayloadCrc(void) {
  uint32_t crc = calcCrc(&rxBuf->payload, rxBuf->payloadLength);

  txBuf->status.result = SUCC_VALUE;
  txBuf->payloadLength = 4;
  txBuf->payload[0] = (crc & 0xff000000U) >> 24; // ここはビッグエンディアン
  txBuf->payload[1] = (crc & 0x00ff0000U) >> 16;
  txBuf->payload[2] = (crc & 0x0000ff00U) >> 8;
  txBuf->payload[3] = (crc & 0x000000ffU);
}
//------------------------------------------------------------------------------
// アクティブポート変更コマンドは両カウンタをクリアするので、SS#リセットの挙動が他のコマンドと変わる
// ほかのコマンドでは非アクティブカウンタはクリアされないので、累計4回で必ずリセットがかかる
static void changeAccessPort(uint8_t cmd) {
  if(!DUALPORT) {
    setResultCode(UNSUPT_CMD, cmd);
    return;
  }

  if(checkParamBytes(sizeof(uint8_t)))  return;

  uint8_t channel = rxBuf->payload[0];

  // パラメータチェック
  if(channel >= 2) {
    setResultCode(PARAM_ERR, 1);
    return;
  }

  activePort = channel;

  irq_exi_dis();
  irq_ext5_dis();   // セカンダリーポートのSSリセットだけ禁止する
  resetCountActive   = 0;
  resetCountInactive = 0;

  // ポートは同時にSSIOFへ繋がらないように解除してから設定する
  if(channel == 0) {  // プライマリへ変更
    PORT3->P3MOD1 = 0x05050000U;  // セカンダリ Hi-Z出力 プルアップ入力
    PORT3->P3MOD0 = 0x00050000U;
    PORT4->P4MOD0 = 0x15151215U;  // プライマリ P43=SS0# in, P42=SDI0 in, P41=SDO0 out, P40=SCK0 in
  }
  else {              // セカンダリへ変更
    PORT4->P4MOD0 = 0x05050005U;  // プライマリ Hi-Z出力 プルアップ入力
    PORT3->P3MOD1 = 0x00001515U;  // セカンダリ P35=SS0# in, P34=SDI0 in
    PORT3->P3MOD0 = 0x12150000U;  //        P33=SDO0 out, P32=SCK0 in

    irq_ext5_init(EXIn_EDGE_RISING, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, EXIn_PORT_SEL_P35 );
  }

  irq_ext3_clearIRQ();
  irq_ext5_clearIRQ();
  irq_exi_ena();
}
//------------------------------------------------------------------------------
