/* ParaSol
 * setup.c
 *
 *  Created on: 2026/06/06
 *      Author: cocot
 */

#include <wdt.h>
#include <clock.h>
#include <ssiof0.h>
#include <ssiof1.h>
#include <irq.h>
#include <dmac.h>
#include "lsicnt.h"

#include "global.h"

void clk32kPLL48M(void);
//------------------------------------------------------------------------------
static void setupWdt(void) {
  wdt_init(WDT_2S);
  wdt_clear();
}
//------------------------------------------------------------------------------
static void setupClock(void) {
  clk_setLsclkCircuit(CLK_LOSCMD_XT32K | CLK_LMOD_LOWPOWER);
  clk_setHsclk(CLK_SYSC_OSCLK | CLK_OUTC_OSCLK_DIV8, CLK_XSPEN_DIS, CLK_HXSPEN_DIS);  // SYSC = OSCLK/1

  clk32kPLL48M();

  uint32_t initPeri;
  if(DUALPORT) {
    initPeri = PERI_SIOF0 | PERI_DMAC | PERI_AI;
  }
  else {
    initPeri = PERI_SIOF1 | PERI_DMAC | PERI_AI;
  }
  set_bit(LSICNT->CLKCON,   initPeri);  // ペリフェラルクロック供給・リセット解除
  clear_bit(LSICNT->RSTCON, initPeri);
}
//------------------------------------------------------------------------------
static void setupGlobal(void) {
  txBuf->status.useCrc = false;
  txBuf->status.result = SYSRESET;
  txBuf->payloadLength = 0x00;

  activePort = 0;
  resetCountActive = 0;
  resetCountInactive = 0;

  modelCount = 0;
  lastExecInstance = 0;
  fftSize = 0;

  idleClock = 0;
}
//------------------------------------------------------------------------------
static void setupGpio(void) {
  if(DUALPORT) {
    PORT4->P4MOD0 = 0x15151215U;  // プライマリ P43=SS0# in, P42=SDI0 in, P41=SDO0 out, P40=SCK0 in

    PORT3->P3MOD1 = 0x05050000U;  // セカンダリ P35=SS0# in, P34=SDI0 in
    PORT3->P3MOD0 = 0x00050000U;  //        P33=SDO0 out, P32=SCK0 in
  }
  else {
    PORT6->P6MOD0 = 0x15151215U;  // P63=SS1# in, P62=SDI1 in, P61=SDO1 out, P60=SCK1 in
  }

  PORT7->P7DO   = nREADY | LED;  // READY=H, LED=点灯
  PORT7->P7MOD1 = 0x00020000U;  // P76=BUSY out
  PORT7->P7MOD0 = 0x00020000U;  // P72=LED out
}
//------------------------------------------------------------------------------
static void setupSsiof(void) {  // スレーブ Mode0 MSBファースト DMA使用
  if(DUALPORT) {
    ssiof0_init(SSIOF_MOZ_HIZ | SSIOF_SOZ_HIZ | SSIOF_SSZ_HIZ
              | SSIOF_CPOL_LOW | SSIOF_CPHA_1SM_2SH | SSIOF_DIR_MSB
              | SSIOF_MDF_DIS | SSIOF_LG_8BIT | SSIOF_MST_SLAVE
              ,(SSIOF_INT_RD_THRESH_1 | SSIOF_INT_WR_THRESH_0));
  }
  else {
    ssiof1_init(SSIOF_MOZ_HIZ | SSIOF_SOZ_HIZ | SSIOF_SSZ_HIZ
              | SSIOF_CPOL_LOW | SSIOF_CPHA_1SM_2SH | SSIOF_DIR_MSB
              | SSIOF_MDF_DIS | SSIOF_LG_8BIT | SSIOF_MST_SLAVE
              ,(SSIOF_INT_RD_THRESH_1 | SSIOF_INT_WR_THRESH_0));
  }
}
//------------------------------------------------------------------------------
static void setupExi(void) {  // P63=SS1#をEXI3として併用 立ち上がりエッジ検出
  irq_exi_dis();
  irq_ext3_dis();
  if(DUALPORT) {
    irq_ext5_dis();
    irq_ext3_init(EXIn_EDGE_RISING, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, EXIn_PORT_SEL_P43 );
    irq_ext5_clearIRQ();
  }
  else {
    irq_ext3_init(EXIn_EDGE_RISING, EXIn_SAMPLING_DIS, EXIn_FILTER_DIS, EXIn_PORT_SEL_P63 );
  }
  irq_ext3_clearIRQ();
  irq_exi_ena();
}
//------------------------------------------------------------------------------
static void setupDmac(void) {
  dmac_setChFix();
}
//------------------------------------------------------------------------------
void setup(void) {
  __disable_irq();

  setupWdt();
  setupClock();
  setupGlobal();
  setupGpio();
  setupSsiof();
  setupExi();
  setupDmac();

  __enable_irq();
}
//------------------------------------------------------------------------------
void NMI_Handler(void)
{ // WDTでweakのDefault_Handlerへ飛ばないようにする空のハンドラ
}
//------------------------------------------------------------------------------
void clk32kPLL48M(void) {
  clk_setLsclk(CLK_XTM_CRYSTAL);  // XT32K有効化
  while(clk_checkLsOscStable() == 0) {
      wdt_clear();
  }

  clk_enaPLLHsclk();  // PLL有効化
  while(clk_checkPllStable() == 0) {
    wdt_clear();
  }

  clk_setSysclk(CLK_SYSCLK_HSCLK);  // 48.00512MHz切り替え
}
//------------------------------------------------------------------------------
