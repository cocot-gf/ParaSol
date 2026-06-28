/* 汎用モジュール
 * crcChecker.h
 *
 *  Created on: 2026/06/05
 *      Author: cocot
 *
 *  関数を呼び出す前にCRC生成器と必要に応じてDMAC0を起動しておくこと
 */

#ifndef SRC_INCLUDE_CRCCHECKER_H_
#define SRC_INCLUDE_CRCCHECKER_H_

#include <stdint.h>
#define CRCSIZE (4)

uint32_t addCrc(void*, uint32_t, uint32_t);
uint32_t checkCrc(void*, uint32_t);
uint32_t calcCrc(void*, uint32_t);

#endif /* SRC_INCLUDE_CRCCHECKER_H_ */
