/* ParaSol
 * typedef.h
 *
 *  Created on: 2026/06/06
 *      Author: cocot
 */

#ifndef SRC_INCLUDE_TYPEDEF_H_
#define SRC_INCLUDE_TYPEDEF_H_

#include <solistAi.h> // bfloat16
#include <stddef.h>   // NULL
#include <stdint.h>   // uintN_t

#define BUFFERSIZE (8192)
#define HEADERSIZE (sizeof(((TxFrame*)NULL)->status) + sizeof(((TxFrame*)NULL)->payloadLength)) // 3
#define PAYLOADMAX (8189) // ペイロードの最大長 BUFFERSIZE - HEADERSIZE、 CRCを追加するときは4バイト減る
#ifndef CRCSIZE
  #define CRCSIZE  (4)
#endif

typedef enum {
  SUCCESS,    //  0  正常終了、戻り値なし
  SUCC_VALUE, //  1  正常終了、戻り値あり
              // フレーム検証エラー
  FRAME_OVR,  //  2  送受信バッファがBUFFERSIZEに達した
  WRONG_PLEN, //  3  PayloadLengthがPAYLOADMAXまたはPAYLOADMAX-CRCSIZEを超えている
  SHORT_PL,   //  4  ペイロードのデータ数がPayloadLengthより少ない
  WRONG_CRC,  //  5  CRCエラー
              // コマンド解析エラー
  UNSUPT_CMD, //  6  コマンド未サポート  戻り値：(1バイト)コマンド番号
  FEW_PARAM,  //  7  パラメータのバイト数が足りない
  PARAM_ERR,  //  8  パラメータ検証エラー 戻り値：(1バイト)問題のあるパラメータ番号
  FEW_DATA,   //  9  コマンドに渡すデータ数が足りない
  MANY_DATA,  //  10  コマンドに渡すデータ数が多い
              // 実行エラー
  UNINITED,   //  11 初期化されていない
  FAILED,     //  12 実行エラー
  SYSRESET = 0x77, // 0x77 リセット成功
} ResultCode;


// ステータスバイトのビットフィールド
typedef struct __attribute__((__packed__)) {
  ResultCode  result : 7;
  uint8_t     useCrc : 1; // CRC使用モード
} Status;


// 送信フレーム構造
typedef struct __attribute__((__packed__)) {
  Status    status;
  uint16_t  payloadLength;   // ペイロードの有効長
  uint8_t   payload[PAYLOADMAX];
} TxFrame;


// 受信フレーム構造
typedef struct __attribute__((__packed__)) {
  uint8_t   command;
  uint16_t  payloadLength;   // ペイロードの有効長
  uint8_t   payload[PAYLOADMAX];
} RxFrame;


// パディングなしのODL_Parameters
typedef struct __attribute__((__packed__)) {
  uint16_t  inputSize;
  uint16_t  hiddenSize;
  uint16_t  outputSize;
  bfloat16  forgettingFactor;
  uint8_t   activationFunction;
  uint8_t   lossFunction;
  uint16_t  seed;
  bfloat16  scaleAlpha;
  bfloat16  scaleGamma;
  bfloat16  leakRate;
  bfloat16  l2Param;
} Packed_ODL_Parameters;


// MemUsage
typedef struct __attribute__((__packed__)) {
  uint16_t  inputSize;
  uint16_t  hiddenSize;
  uint16_t  outputSize;
  uint32_t  aiRam;
  uint32_t  betaSize;
  uint32_t  pSize;
  uint16_t  resSize;
} MemUsage;

#endif /* SRC_INCLUDE_TYPEDEF_H_ */
