/* 汎用モジュール
 * lsicnt.h
 *
 *  Created on: 2026/06/04
 *      Author: cocot
 */

#ifndef SRC_INCLUDE_LSICNT_H_
#define SRC_INCLUDE_LSICNT_H_

// CLKCON + RSTCON
#define PERI_TM0        ( 0x00000001U )
#define PERI_TM1        ( 0x00000002U )
#define PERI_TM2        ( 0x00000004U )
#define PERI_TM3        ( 0x00000008U )
#define PERI_TM4        ( 0x00000010U )
#define PERI_TM5        ( 0x00000020U )
#define PERI_FTM0       ( 0x00000100U )
#define PERI_FTM1       ( 0x00000200U )
#define PERI_NTMS       ( 0x00001000U )
#define PERI_SAD0       ( 0x00002000U )
#define PERI_SAD1       ( 0x00004000U )
#define PERI_TM1K       ( 0x00008000U )
#define PERI_SIOF0      ( 0x00010000U )
#define PERI_SIOF1      ( 0x00020000U )
#define PERI_UAF0       ( 0x00040000U )
#define PERI_UAF1       ( 0x00080000U )
#define PERI_UAF2       ( 0x00100000U )
#define PERI_I2CF0      ( 0x00200000U )
#define PERI_UAF3       ( 0x00400000U )
#define PERI_CAN        ( 0x00800000U )
#define PERI_RTC        ( 0x04000000U )
#define PERI_VLS        ( 0x08000000U )
#define PERI_CMP        ( 0x10000000U )
#define PERI_DMAC       ( 0x20000000U )
#define PERI_AI         ( 0x40000000U )
#define PERI_CRC        ( 0x80000000U )


// MCUINTEN + MCUISTAT
#define MCUINT_FLCINT      ( 0x00000001U )
#define MCUINT_RAM_PAR     ( 0x00000010U )
#define MCUINT_AI_PAR      ( 0x00000020U )
#define MCUINT_CAN_PAR     ( 0x00000040U )
#define MCUINT_CAN_BUS_ERR ( 0x00000100U )


#endif /* SRC_INCLUDE_LSICNT_H_ */
