/* 汎用モジュール
 * crcChecker.c
 *
 *  Created on: 2026/06/05
 *      Author: cocot
 *
 *  関数を呼び出す前にDMAC0を起動しておくこと
 */

#include <crc.h>
#include <dmac.h>
#include <dmac0.h>
#include <wdt.h>
#include "lsicnt.h"

#define CRCSIZE (4)

uint32_t addCrc(void*, uint32_t, uint32_t);
uint32_t checkCrc(void*, uint32_t);
uint32_t calcCrc(void*, uint32_t);
//------------------------------------------------------------------------------
/**
 * addCrc
 *
 * addrからdataSizeバイトのCRC32_MPEG2を計算し、dataSizeの後に4バイトのCRCを追加する。
 * dataSize + CRCSIZE > bufferSizeの条件では書き込みエリアが足りないためエラーとする。
 *
 * @param   addr        計算開始アドレス
 * @param   dataSize    計算するバイト数
 * @param   bufferSize  バッファサイズ
 * @return  CRCを追加した合計のdataSize
 */
uint32_t addCrc(void* addr, uint32_t dataSize, uint32_t bufferSize) {
  if(dataSize + CRCSIZE > bufferSize) { // CRCを書き込む場所が足りない
    return 0;
  }

  uint32_t crc = calcCrc(addr, dataSize);

  *((uint8_t*)addr + dataSize)     = (crc & 0xff000000) >> 24;
  *((uint8_t*)addr + dataSize + 1) = (crc & 0x00ff0000) >> 16;
  *((uint8_t*)addr + dataSize + 2) = (crc & 0x0000ff00) >> 8;
  *((uint8_t*)addr + dataSize + 3) = (crc & 0x000000ff);

  return dataSize + CRCSIZE;
}
//------------------------------------------------------------------------------
/**
 * checkCrc
 *
 * addrからCRCを含むsizeバイトのCRCを計算し結果を返す。
 * データが正常なら戻り値は0(false)となる。
 *
 * @param   addr        計算開始アドレス
 * @param   size        計算するバイト数
 * @return  CRCの計算値
 */
uint32_t checkCrc(void* addr, uint32_t size) {
  if(size < CRCSIZE) { // データサイズチェックエラーはCRC不一致として返す
    return 1;
  }

  return calcCrc(addr, size);
}
//------------------------------------------------------------------------------
/**
 * calcCrc
 *
 * addrからsizeバイトのCRCを計算し結果を返す。
 * USE_DMAのdefineがあればDMAC0を使って高速に転送する。
 *
 * @param   addr        計算開始アドレス
 * @param   size        計算するバイト数
 * @return  CRCの計算値
 */
uint32_t calcCrc(void* addr, uint32_t size) {
  set_bit(LSICNT->CLKCON,   PERI_CRC);
  clear_bit(LSICNT->RSTCON, PERI_CRC);

  crc_Init(CRC_DIRECTION_MSB_FIRST | CRC_BITWIDTH_8BIT | CRC_MODE_CRC32, 0xFFFFFFFFU);

  dmac0_init(DMAC_TMOD_ARQ_AUTO | DMAC_TMOD_SDP_INC | DMAC_TMOD_DDP_FIX | DMAC_TMOD_TSIZ_BYTE
           | DMAC_TMOD_BRQ_BURST | DMAC_TMOD_IMK_IMASK, DMAC_REQ_NOTSEL, (void*)0 );
  dmac0_Transfer(addr, (void*)&CRC->CRCDATA, size);
  dmac0_enable();
  while(dmac0_checkEnd()) {
    wdt_clear();
  }
  dmac0_disable();
  uint32_t crc = crc_getCalcResult();

  set_bit(LSICNT->RSTCON,   PERI_CRC);
  clear_bit(LSICNT->CLKCON, PERI_CRC);

  return crc;
}
//------------------------------------------------------------------------------
