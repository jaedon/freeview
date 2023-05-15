/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_core_avd_reve1.c $
 * $brcm_Revision: Hydra_Software_Devel/29 $
 * $brcm_Date: 6/18/12 4:27p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_core_avd_reve1.c $
 * 
 * Hydra_Software_Devel/29   6/18/12 4:27p davidp
 * SW7435-164: Remove un-needed include file.
 * 
 * Hydra_Software_Devel/28   11/30/09 4:38p btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_*
 * at compile.
 * 
 * Hydra_Software_Devel/27   1/21/09 2:01p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 * 
 * Hydra_Software_Devel/26   9/12/07 3:44p nilesh
 * PR34832: Fixed issue with mailbox interrupt handler being called during
 * AVD boot process
 * 
 * Hydra_Software_Devel/25   9/6/07 5:35p nilesh
 * PR29915: Added BERR_TRACE wrapper around all return codes
 * 
 * Hydra_Software_Devel/24   8/31/07 3:33p nilesh
 * PR29915: Fix error default error code on ChipEnable for core e1
 * 
 * Hydra_Software_Devel/23   8/31/07 3:24p nilesh
 * PR29915: Fix boot result code propogation
 * 
 * Hydra_Software_Devel/22   8/21/07 3:05p davidp
 * PR34052: Move register address pointers to platform_info.stReg
 * structure.
 * 
 * Hydra_Software_Devel/21   5/4/07 12:55p rayshap
 * PR30268:remove UART programing from XVD
 * 
 * Hydra_Software_Devel/20   3/26/07 10:30a pblanco
 * PR27168: Added bchp_common.h for BCHP_PHYSICAL_OFFSET definition.
 * 
 * Hydra_Software_Devel/19   3/23/07 1:27p pblanco
 * PR27168: Began conversion of hard coded value to appropriate #defined
 * constants.
 * 
 * Hydra_Software_Devel/18   3/19/07 10:22a pblanco
 * PR27168: Added defined constants for init timeout, baud rate and UART
 * frequency. Also cleaned up some code formatting.
 * 
 * Hydra_Software_Devel/17   3/2/07 12:15p pblanco
 * PR27283: Removed conditionalized UART code.
 * 
 * Hydra_Software_Devel/16   2/26/07 5:44p nilesh
 * PR26188: Fix 7400B0 memory setup issues
 * 
 * Hydra_Software_Devel/15   2/26/07 3:45p davidp
 * PR26188: Disable UART setup in BXVD_P_ChpEnable_RevE1
 * 
 * Hydra_Software_Devel/14   2/6/07 11:07a nilesh
 * PR27575: Fix 7440 Watchdog and make watchdog more robust on all
 * platforms.
 * 
 * Hydra_Software_Devel/PR27575/1   2/2/07 2:11p nilesh
 * PR27575: Fix 7440 Watchdog
 * 
 * Hydra_Software_Devel/13   2/2/07 1:25p gaurava
 * PR 27283: Firmware with new UART initialization checked in for 7440.
 * 
 * Hydra_Software_Devel/11   1/29/07 12:43p pblanco
 * PR27283: Backed out most recent change... FW isn't ready yet.
 * 
 * Hydra_Software_Devel/10   1/29/07 11:34a pblanco
 * PR27283: Checked in core code with new UART initialization enabled.
 * 
 * Hydra_Software_Devel/9   1/23/07 1:43p pblanco
 * PR27283: New UART initialization support. Turned off for now via
 * NEW_UART_SETUP until new FW is delivered.
 * 
 * Hydra_Software_Devel/8   12/14/06 2:22p nilesh
 * PR25443: Updated to support simplified FW API
 * 
 * Hydra_Software_Devel/7   11/17/06 4:52p nilesh
 * PR25868: Merged authenticated image support to mainline
 * 
 * Hydra_Software_Devel/PR25868/2   11/16/06 3:44p nilesh
 * PR25868: Added authenticated image support, Refactored BIMG FW Load
 * code
 * 
 * Hydra_Software_Devel/PR25868/1   11/14/06 3:54p nilesh
 * PR25868: Added pre-relocated image support
 * 
 * Hydra_Software_Devel/6   10/17/06 10:26a nilesh
 * PR23113: Moved AVD UART MUX setup to PI
 * 
 * Hydra_Software_Devel/5   9/13/06 1:42p nilesh
 * PR23113: Disable UART/PIN Mux configuration by default
 * 
 * Hydra_Software_Devel/4   9/12/06 11:59a nilesh
 * PR23113: Fixed AVD UART initialization sequence to prevent garbage
 * characters on the AVD incoming UART
 * 
 * Hydra_Software_Devel/2   9/8/06 2:35p nilesh
 * PR23113: Initial support for dual decode
 *
 ***************************************************************************/
#include "bxvd.h"
#include "bxvd_priv.h"
#include "bxvd_intr.h"
#include "bxvd_reg.h"
#include "bxvd_userdata.h"
#include "bxvd_platform.h"
#include "bchp_common.h"

/* Local constants */
#define BXVD_REVE1_OL_INIT_TIMEOUT 1000         /* One second init timeout */
#define BXVD_REVE1_CORE_BAUD_RATE 115200        /* OL & IL UART baud rate */
#define BXVD_REVE1_CORE_UART_FREQ (200*1000000) /* UART clock frequency */

BDBG_MODULE(BXVD_CORE_AVD_REVE1);

#if BXVD_P_USE_CORE_CHIP_ENABLE_REVE1

BERR_Code BXVD_P_ChipEnable_RevE1(BXVD_Handle hXvd)
{
   uint32_t tries;
   volatile uint32_t uiVal;
   BERR_Code rc = BERR_SUCCESS;


   BDBG_ASSERT(hXvd);
   BDBG_ENTER(BXVD_P_ChipEnable_RevE1);

   /* Enable the AVD UART pin-mux and start the decoder*/
   BXVD_Reg_Write32(hXvd, 
		    hXvd->stPlatformInfo.stReg.uiSun_TestPortControl,
		    hXvd->stPlatformInfo.stReg.uiSun_TestPortControlAVDValue);

   /* Write to VectorTB, CpuDbg registers and AuxRegs 
    * in THIS ORDER to begin 7401 operation 
    */

   /* program the relocation base address for outer-loop */
   BXVD_Reg_Write32(hXvd, 
		    hXvd->stPlatformInfo.stReg.uiDecode_OuterInstructionBase,
		    hXvd->uiOuterLoopInstructionBase); 

   /* program the outer loop image end of code address */
   BXVD_Reg_Write32(hXvd, 
		    hXvd->stPlatformInfo.stReg.uiDecode_OuterEndOfCode, 
		    hXvd->uiOuterLoopEOC);

   /* program the relocation base address for inner-loop */
   BXVD_Reg_Write32(hXvd, 
		    hXvd->stPlatformInfo.stReg.uiDecode_InnerInstructionBase,
		    hXvd->uiInnerLoopInstructionBase); 

   /* program the inner loop image end of code address */
   BXVD_Reg_Write32(hXvd, 
		    hXvd->stPlatformInfo.stReg.uiDecode_InnerEndOfCode, 
		    hXvd->uiInnerLoopEOC);

   /* program global IO base register - Outer */
   BXVD_Reg_Write32(hXvd,
		    hXvd->stPlatformInfo.stReg.uiDecode_OuterGlobalIOBase,
		    BCHP_PHYSICAL_OFFSET);

   /* program global IO base register - Inner */
   BXVD_Reg_Write32(hXvd,
		    hXvd->stPlatformInfo.stReg.uiDecode_InnerGlobalIOBase,
		    BCHP_PHYSICAL_OFFSET);		    

#if 0
/* START - 7440 UART Debug Code */
   {
      /* Setup UART MUX to tie:
         - UART2 to AVD0 Outer Loop 
         - UART3 to AVD0 Inner Loop
         - UART0 to AVD1 Outer Loop
         - UART4 to AVD1 Inner Loop
      */
      BXVD_Reg_Write32(hXvd, 
                       BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,
                       ((BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_2_cpu_sel_AVD0_OL << BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_2_cpu_sel_SHIFT) & BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_2_cpu_sel_MASK) | 
                       ((BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_3_cpu_sel_AVD0_IL << BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_3_cpu_sel_SHIFT) & BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_3_cpu_sel_MASK) |
                       ((BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_0_cpu_sel_AVD1_OL << BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_0_cpu_sel_SHIFT) & BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_0_cpu_sel_MASK) |
                       ((BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_4_cpu_sel_AVD1_IL << BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_4_cpu_sel_SHIFT) & BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_4_cpu_sel_MASK));      
   }
   /* END - 7440 Debug Code */
#endif

#if 0
   /* ((BCHP_CHIP == 7400) && (BCHP_VER == BCHP_VER_B0))                       *
    * HACK: Enable XVD UARTs on 7400. We should move this to the platform 7400 */
   {
      uint32_t reg;
      if (hXvd->uDecoderInstance == 0)
      {         
         reg = BXVD_Reg_Read32(hXvd, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);
         reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel) | BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_2_cpu_sel));
         reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, AVD0_OL);
         reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_2_cpu_sel, AVD0_IL);
         BXVD_Reg_Write32(hXvd, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL, reg);
      } else {
         reg = BXVD_Reg_Read32(hXvd, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);
         reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel) | BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_2_cpu_sel));
         reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, AVD1_OL);
         reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_2_cpu_sel, AVD1_IL);
         BXVD_Reg_Write32(hXvd, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL, reg);
      }
   }
#endif

   /* PR34832: We disable the mailbox interrupt callback here because
    * during AVD boot, the CPU needs to write the CPU2Host mailbox
    * register to initialize the AVD UART baud rate.  This causes an
    * unwanted interrupt to occur, so we turn off the mailbox
    * interrupt during the boot process */
   rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_MBX_ISR);
   if (rc != BERR_SUCCESS) 
   {
      BXVD_DBG_ERR(hXvd, ("Error disabling mailbox ISR callback"));
      return BERR_TRACE(rc);
   }
   
   /* Program UART frequency and baud rate prior to boot */
   BXVD_Reg_Write32(hXvd,
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox, 
                    BXVD_REVE1_CORE_BAUD_RATE);
		
   BXVD_Reg_Write32(hXvd,
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterHost2CPUMailbox,
                    BXVD_REVE1_CORE_UART_FREQ);

   /* Boot the core */
   if (hXvd->stSettings.pAVDBootCallback) {
      /* Call external boot callback */
      rc = (*hXvd->stSettings.pAVDBootCallback)(hXvd->stSettings.pAVDBootCallbackData,
                                                hXvd->eAVDBootMode);
	   
      if (rc != BERR_SUCCESS) {
         BXVD_DBG_ERR(hXvd, ("Error #%d booting the AVD externally", rc));
         return BERR_TRACE(rc);
      }
   } else {
      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUDebug,
                       1);
	   
      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUAux,
                       0 ); 
      BXVD_Reg_Write32(hXvd,
                       hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUDebug,
                       0);
   }

   /* Poll for MBX clear to verify outer loop initialization */
   for ( tries = 0; tries < BXVD_REVE1_OL_INIT_TIMEOUT; tries++)
   {
      BKNI_Sleep(1);

      uiVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox);

      if (uiVal ==0) 
         break;
   }			

   /* If we've waited 1 second, the default value of BXVD_OL_INIT_TIMEOUT, the
    * core boot timed out. Return an error 
    */
   if (tries >= BXVD_REVE1_OL_INIT_TIMEOUT)
   {
      BXVD_DBG_ERR(hXvd, ("AVD core boot timeout. Error was 0x%x (%d)", uiVal, uiVal));
      rc = BERR_TRACE(BERR_TIMEOUT);
   }

   /* PR34832: We clear and re-enable the mailbox interrupt after the
    * boot has finished */
   rc = BINT_ClearCallback(hXvd->stDecoderContext.pCbAVC_MBX_ISR);
   if (rc != BERR_SUCCESS) 
   {
      BXVD_DBG_ERR(hXvd, ("Error clearing pending mailbox ISR callbacks"));
      return BERR_TRACE(rc);
   }

   rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_MBX_ISR);
   if (rc != BERR_SUCCESS) 
   {
      BXVD_DBG_ERR(hXvd, ("Error enabling mailbox ISR callback"));
      return BERR_TRACE(rc);
   }

   BDBG_LEAVE(BXVD_P_ChipEnable_RevE1);

   return BERR_TRACE(rc);
}

#endif /* BXVD_P_USE_CORE_CHIP_ENABLE_REVE1 */
