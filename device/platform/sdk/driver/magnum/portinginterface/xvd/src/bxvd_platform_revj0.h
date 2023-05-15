/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_revj0.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 4/5/12 5:22p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_revj0.h $
 * 
 * Hydra_Software_Devel/7   4/5/12 5:22p davidp
 * SW7344-306: Printout AVD HW revision in BXVD_Open.
 * 
 * Hydra_Software_Devel/6   2/2/12 3:06p davidp
 * SW7425-2024: Define platform specific supported decode protocols mask.
 * 
 * Hydra_Software_Devel/5   10/12/09 4:32p davidp
 * SW7630-31: Change BXVD_P_AVD_INIT_STRIPE_WIDTH value to 1.
 * 
 * Hydra_Software_Devel/4   10/5/09 5:16p davidp
 * SW7550-53: Stop outer and inner loop cores, reset specific AVD blocks
 * before performing a software reset.
 * 
 * Hydra_Software_Devel/3   9/21/09 5:23p davidp
 * SW7630-31: Change DVD memory config macro to eUma.
 * 
 * Hydra_Software_Devel/2   8/31/09 5:29p davidp
 * SW7630-31: Add support for 7630 platform.
 * 
 * Hydra_Software_Devel/1   6/4/09 4:06p davidp
 * PR54107: Add support for 35130 platform, create Rev J platform family.
 * 
 ***************************************************************************/

#ifndef _BXVD_PLATFORM_REVJ0_H_
#define _BXVD_PLATFORM_REVJ0_H_

#include "bchp_avd_intr2_0.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_int_id_avd_intr2_0.h"
#include "bchp_decode_cpuregs2_0.h"
#include "bchp_decode_cpuregs_0.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_xpt_pcroffset.h"

#include "bchp_decode_ind_sdram_regs_0.h"
#include "bchp_decode_ind_sdram_regs2_0.h"
#include "bchp_decode_cpuaux2_0.h"
#include "bchp_decode_cpuaux_0.h"
#include "bchp_decode_main_0.h"
#include "bchp_decode_sint_0.h"
#include "bchp_decode_sint_oloop_0.h"
#include "bchp_decode_ip_shim_0.h"

#if (BCHP_CHIP == 7630)
#define BXVD_P_DVD_CHIP 1

#define BXVD_P_AVD_INIT_STRIPE_WIDTH 1
#define BXVD_P_AVD_INIT_STRIPE_MULTIPLE 0
#endif

#if (!BXVD_P_DVD_CHIP)
#include "bchp_decode_sd_0.h"

#define BXVD_P_GET_MEMORY_CONFIG(hXvd, eMemCfgMode)                            \
{                                                                              \
   uint32_t uiPfriSource;                                                      \
   uiPfriSource = (hXvd->uiDecode_PFRIDataRegVal  &                            \
                   hXvd->stPlatformInfo.stReg.uiDecode_PFRIDataSourceMask) >>  \
                   hXvd->stPlatformInfo.stReg.uiDecode_PFRIDataSourceShift;    \
   if (uiPfriSource == 0)                                                      \
   {                                                                           \
      eMemCfgMode = BXVD_P_MemCfgMode_eUMA;                                    \
   }                                                                           \
   else                                                                        \
   {                                                                           \
      eMemCfgMode = BXVD_P_MemCfgMode_eNONUMA;                                 \
   }                                                                           \
}
#else
#define BXVD_P_GET_MEMORY_CONFIG(hXvd, eMemCfgMode)                            \
{                                                                              \
      eMemCfgMode = BXVD_P_MemCfgMode_eUMA;                                    \
}
#endif

#define BXVD_MAX_INSTANCE_COUNT     1

/* UART clock frequency */
#define BXVD_P_AVD_CORE_UART_FREQ (250*1000000) 

/* Define supported decode protocols mask */ 
/* AVD Rev J */
#define BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS  BXVD_P_REVJ_DECODE_PROTOCOLS_MASK
#define BXVD_P_CORE_REVISION 'J'

/*********************
 * Setup Interrupts
 *********************/
/* The BXVD_P_USE_SETUP_INTERRUPTS_7401B0 is needed to conditionally compile
 * the function we need, since all platform source files are included
 * as part of the build so that we can share code as needed between
 * platforms/families */
#define BXVD_P_USE_SETUP_INTERRUPTS_REVE0 1 

/* The BXVD_P_SetupInterrupts is needed to have XVD use the proper
 * platform-specific implementation. The main XVD code calls
 * BXVD_P_SetupInterrupts, which doesn't really exist */
#define BXVD_P_SETUP_INTERRUPTS BXVD_P_SetupInterrupts_RevE0 

/* This macro creates the proper function prototype for the
 * platform-specific implementation to prevent "reference to undefined
 * function" errors */
BXVD_P_SETUP_INTERRUPTS_PROTOTYPE(RevE0); 

/* Setup Watchdog */
#define BXVD_P_USE_SETUP_WATCHDOG_REVE0 1
#define BXVD_P_SETUP_WATCHDOG BXVD_P_SetupWatchdog_RevE0
BXVD_P_SETUP_WATCHDOG_PROTOTYPE(RevE0); 

/* Firmware loading */
#define BXVD_P_USE_FW_LOAD_CORE_REVE0 1
#define BXVD_P_FW_LOAD BXVD_P_FWLoad_RevE0
BXVD_P_FW_LOAD_PROTOTYPE(RevE0);

/* Chip enable */
#define BXVD_P_USE_CORE_CHIP_ENABLE_REVE0 1
#define BXVD_P_CHIP_ENABLE BXVD_P_ChipEnable_RevE0
BXVD_P_CHIP_ENABLE_PROTOTYPE(RevE0);

/* Chip Reset */
#define BXVD_P_USE_CORE_RESET_CHIP_REVJ0 1
#define BXVD_P_RESET_CHIP BXVD_P_ChipReset_RevJ0
BXVD_P_CHIP_RESET_PROTOTYPE(RevJ0);

/* Init Register Pointers/Masks */
#define BXVD_P_USE_INIT_REG_PTRS_REVJ0 1
#define BXVD_P_INIT_REG_PTRS BXVD_P_InitRegPtrs_RevJ0
BXVD_P_INIT_REG_PTRS_PROTOTYPE(RevJ0);

/* Setup FW Memory */
#define BXVD_P_USE_SETUP_FW_MEMORY_REVE0 1
#define BXVD_P_SETUP_FW_MEMORY BXVD_P_SetupFWMemory_RevE0
BXVD_P_SETUP_FW_MEMORY_PROTOTYPE(RevE0);

/* Teardown FW Memory */
#define BXVD_P_USE_TEAR_DOWN_FW_MEMORY_REVE0 1
#define BXVD_P_TEAR_DOWN_FW_MEMORY BXVD_P_TearDownFWMemory_RevE0
BXVD_P_TEAR_DOWN_FW_MEMORY_PROTOTYPE(RevE0);

/* Verify Watchdog Fired */
#define BXVD_P_USE_VERIFY_WATCHDOG_FIRED_REVE0 1
#define BXVD_P_VERIFY_WATCHDOG_FIRED BXVD_P_VerifyWatchdogFired_RevE0
BXVD_P_VERIFY_WATCHDOG_FIRED_PROTOTYPE(RevE0);

/* Convert userdata offset to address */
#define BXVD_P_USE_CONVERT_UD_OFF2ADDR_REVE0 1
#define BXVD_P_CONVERT_UD_OFF2ADDR BXVD_P_ConvertUDOff2Addr_RevE0
BXVD_P_CONVERT_UD_OFF2ADDR_PROTOTYPE(RevE0);

#endif /* _BXVD_PLATFORM_REVJ0_H_ */


