/* ParaSol
 * global.h
 *
 *  Created on: 2026/06/06
 *      Author: cocot
 */

#ifndef SRC_INCLUDE_GLOBAL_H_
#define SRC_INCLUDE_GLOBAL_H_

#include <stdbool.h>
#include "typedef.h"

// バージョン番号
#define MAJOR (1)
#define MINOR (0)

// GPIO定義
#define LED       (0x04U)
#define nREADY    (0x40U)
#define LEDPORT   (PORT7->P7DO)
#define READYPORT (PORT7->P7DO)

// リセット条件の回数
#define RESETASSERT  (4)

extern const bool DUALPORT;

// 送受信バッファ
extern TxFrame* const txBuf;
extern RxFrame* const rxBuf;

// 送受信管理変数
extern bool occurEXI;
extern int  xferBytes;
extern int  activePort;
extern int  resetCountActive;
extern int  resetCountInactive;

// AI管理変数
#define MODELSMAX (3)
#define OSUADMAX  (1)
extern uint8_t  modelCount;
extern uint8_t  lastExecInstance;
extern uint16_t fftSize;

// その他
extern uint8_t idleClock;

#endif /* SRC_INCLUDE_GLOBAL_H_ */
