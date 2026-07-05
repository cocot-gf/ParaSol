/* ParaSol
 * cmdCommon.h
 *
 *  Created on: 2026/06/18
 *      Author: cocot
 */

#ifndef SRC_INCLUDE_CMDCOMMON_H_
#define SRC_INCLUDE_CMDCOMMON_H_

#include "typedef.h"

void setResult(ResultCode);
void setResultCode(ResultCode, uint8_t);
float bf16ToFloat(bfloat16);
bool checkParamBytes(int);
bool checkInstanceRange(void);
void aiTransferDMA(void*, void*, uint16_t);

#endif /* SRC_INCLUDE_CMDCOMMON_H_ */
