/* ParaSol
 * spiTransfer.c
 *
 *  Created on: 2026/06/07
 *      Author: cocot
 */

#include <ssiof0.h>
#include <ssiof1.h>
#include <dmac.h>
#include <dmac0.h>
#include <dmac1.h>
#include <irq.h>
#include <clock.h>

#include "crcChecker.h"
#include "typedef.h"
#include "global.h"

//------------------------------------------------------------------------------
void beginTransfer(void) {
  occurEXI = false;
  // 受信準備
  rxBuf->command = 0;
  rxBuf->payloadLength = 0;

  // 送信準備
  uint32_t txSize = HEADERSIZE + txBuf->payloadLength;

  // 必要であればCRCを追加
  if(txBuf->status.useCrc) {
    // CRCを追加するためのバッファサイズが足りない場合は txSize = 0 となる
    txSize = addCrc(txBuf, txSize, BUFFERSIZE);
  }

  if(DUALPORT) {
    ssiof0_clearFifo();

    // 送信バッファ DMAでCRCまで送信し、それ以降はSSIOFの自動0xFF送信に任せる
    dmac0_init(DMAC_TMOD_ARQ_EXT | DMAC_TMOD_SDP_INC | DMAC_TMOD_DDP_FIX | DMAC_TMOD_TSIZ_BYTE
             | DMAC_TMOD_BRQ_CYCLE | DMAC_TMOD_IMK_IMASK, DMAC_REQ_SSIOF0_TX , (void*)0 );
    dmac0_Transfer(txBuf, (void*)&SSIOF0->SF0DWR, txSize);

    // 受信バッファ DMAでバッファサイズ限界まで受信させる
    dmac1_init(DMAC_TMOD_ARQ_EXT | DMAC_TMOD_SDP_FIX | DMAC_TMOD_DDP_INC | DMAC_TMOD_TSIZ_BYTE
             | DMAC_TMOD_BRQ_CYCLE | DMAC_TMOD_IMK_IMASK, DMAC_REQ_SSIOF0_RX , (void*)0 );
    dmac1_Transfer((void*)&SSIOF0->SF0DRR, rxBuf, BUFFERSIZE);
  }
  else {
    ssiof1_clearFifo();

    dmac0_init(DMAC_TMOD_ARQ_EXT | DMAC_TMOD_SDP_INC | DMAC_TMOD_DDP_FIX | DMAC_TMOD_TSIZ_BYTE
             | DMAC_TMOD_BRQ_CYCLE | DMAC_TMOD_IMK_IMASK, DMAC_REQ_SSIOF1_TX , (void*)0 );
    dmac0_Transfer(txBuf, (void*)&SSIOF1->SF1DWR, txSize);

    dmac1_init(DMAC_TMOD_ARQ_EXT | DMAC_TMOD_SDP_FIX | DMAC_TMOD_DDP_INC | DMAC_TMOD_TSIZ_BYTE
             | DMAC_TMOD_BRQ_CYCLE | DMAC_TMOD_IMK_IMASK, DMAC_REQ_SSIOF1_RX , (void*)0 );
    dmac1_Transfer((void*)&SSIOF1->SF1DRR, rxBuf, BUFFERSIZE);
  }

  dmac0_enable();
  dmac1_enable();

  // トランスミッタ稼働
  if(DUALPORT) {
    set_bit(SSIOF0->SF0CTRL, 1);
  }
  else {
    set_bit(SSIOF1->SF1CTRL, 1);
  }

  write_reg32(CLKCNT->FCON01, (CLKCNT->FCON01 & 0xFFFFFFF8) | idleClock);
  set_bit(READYPORT, nREADY);
}
//------------------------------------------------------------------------------
void EXI_IRQHandler(void)
{
  int intr = irq_ext3_checkIRQ();  // primary = bit0
  intr += (DUALPORT & irq_ext5_checkIRQ()) << 1;  // secondary = bit1

  if(intr) {
    clear_bit(READYPORT, nREADY);
    write_reg32(CLKCNT->FCON01, (CLKCNT->FCON01 & 0xFFFFFFF8) | CLK_SYSC_OSCLK);

    if((activePort + 1) & intr) { // アクティブポートのSS#変化
      dmac0_disable();      // DMAとSPIを止める
      dmac1_disable();
      if(DUALPORT) {
        ssiof0_stop();
      }
      else {
        ssiof1_stop();
      }

      xferBytes = BUFFERSIZE - DMAC1->DMACSIZ1;  // 実際の転送サイズを算出

      if(xferBytes != 0) {
        resetCountActive = 0;   // データ転送があった場合アクティブカウンタをリセット
      }
      else {
        resetCountActive++;   // データ転送がなかった場合カウントアップ
      }
    }  // active port
    else if(DUALPORT && (intr & 1)){  // 非アクティブポートのSS変化
      resetCountInactive++;   // DUALPORTかつプライマリポートの時に非アクティブカウンタをカウントアップ
    }  // inactive port

    if(resetCountActive == RESETASSERT || resetCountInactive == RESETASSERT) {
      clear_bit(LEDPORT, LED);
      NVIC_SystemReset();
    }

    if(intr & 1) {
      irq_ext3_clearIRQ();
    }
    if(intr & 2) {
      irq_ext5_clearIRQ();
    }
    occurEXI = true;
  } // EXI
}
//------------------------------------------------------------------------------
