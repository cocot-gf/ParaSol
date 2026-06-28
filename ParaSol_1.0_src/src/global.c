/* ParaSol
 * global.c
 *
 *  Created on: 2026/06/06
 *      Author: cocot
 */

#include <can.h>
#include <stdbool.h>

#include "typedef.h"

#ifdef DUAL       // コンパイラのプリプロセッサオプションでDUALを宣言して切り替える
const bool DUALPORT = true;  // デュアルポートで使うか否か
#else
const bool DUALPORT = false;
#endif

// 送受信バッファ
static TxFrame bssBuf;
TxFrame* const txBuf = &bssBuf;
RxFrame* const rxBuf = (RxFrame*)CAN_MESSAGE_RAM_ADDR;

// 送受信管理変数
bool  occurEXI;
int   xferBytes;
int   activePort;
int   resetCountActive;
int   resetCountInactive;

// AI管理変数
uint8_t   modelCount;
uint8_t   lastExecInstance; // 直前に実行したStartTrainやStartPredictのインスタンス番号+1を保持する。
uint16_t  fftSize;

// その他
uint8_t idleClock;
