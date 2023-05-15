/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7403.h $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 4/5/12 5:21p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7403.h $
 * 
 * Hydra_Software_Devel/11   4/5/12 5:21p davidp
 * SW7344-306: Printout AVD HW revision in BXVD_Open.
 * 
 * Hydra_Software_Devel/10   2/10/12 3:25p davidp
 * SW7425-2024: Set decode protocols mask to include Divx.
 * 
 * Hydra_Software_Devel/9   2/2/12 3:04p davidp
 * SW7425-2024: Define platform specific supported decode protocols mask.
 * 
 * Hydra_Software_Devel/8   9/18/08 2:02p davidp
 * PR46650: Fix MemCfgMode enum field names.
 * 
 * Hydra_Software_Devel/7   9/9/08 11:31a davidp
 * PR46650: Add BXVD_P_GET_MEMORY_CONFIG macro to sanity check heaps
 * passed to BXVD_Open.
 * 
 * Hydra_Software_Devel/6   5/22/08 4:54p davidp
 * PR41411: Add SUB_GISB_ARB REQ_MASK register and AVD mask bit fields to
 * platform register structure.
 * 
 * Hydra_Software_Devel/5   11/6/07 2:21p davidp
 * PR27168: Remove use of SETUP_HEAP macro and associated routines per
 * code review
 * 
 * Hydra_Software_Devel/4   9/28/07 12:23p nilesh
 * PR27168: Consolidate constants to bxvd_platform.h and rename according
 * to XVD conventions
 * 
 * Hydra_Software_Devel/3   8/22/07 3:48p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/2   3/2/07 8:42a pblanco
 * PR26188: Removed SD_PARAM define from all Rev. E0 core platforms.
 * 
 * Hydra_Software_Devel/1   11/17/06 4:26p davidp
 * PR25671: Initial checkin for 7403 platform support.
 * 
 ***************************************************************************/

#ifndef _BXVD_PLATFORM_7403_H_
#define _BXVD_PLATFORM_7403_H_

#include "bchp_7403.h"
#include "bchp_avd0_intr2.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_int_id_avd0_intr2.h"
#include "bchp_decode_cpuregs2.h"
#include "bchp_decode_cpuregs.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_xpt_pcroffset0.h"

#include "bchp_decode_ind_sdram_regs2.h"
#include "bchp_decode_cpuaux2.h"
#include "bchp_decode_cpuaux.h"

#define BXVD_MAX_INSTANCE_COUNT     1

/* The 7400, 7401 and 7118 all use this test port control value */
#define TEST_PORT_CONTROL_VALUE_ARC_0 0x00000018

#define BXVD_P_GET_MEMORY_CONFIG(hXvd, eMemCfgMode)  \
      eMemCfgMode = BXVD_P_MemCfgMode_eUMA;

/* Define supported decode protocols mask */ 
/* AVD Rev H */
#define BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS  BXVD_P_REVH_DECODE_PROTOCOLS_MASK
#define BXVD_P_CORE_REVISION 'H'

/*********************
 * Setup Interrupts
 *********************/
/* The BXVD_P_USE_SETUP_INTERRUPTS_REVE0 is needed to conditionally compile
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
#define BXVD_P_USE_CORE_RESET_CHIP_REVE0 1
#define BXVD_P_RESET_CHIP BXVD_P_ChipReset_RevE0
BXVD_P_CHIP_RESET_PROTOTYPE(RevE0);

/* Init Register Pointers/Masks */
#define BXVD_P_USE_INIT_REG_PTRS_7403 1
#define BXVD_P_INIT_REG_PTRS BXVD_P_InitRegPtrs_7403
BXVD_P_INIT_REG_PTRS_PROTOTYPE(7403);

/* Setup FW Memory */
#define BXVD_P_USE_SETUP_FW_MEMORY_REVE0 1
#define BXVD_P_SETUP_FW_MEMORY BXVD_P_SetupFWMemory_RevE0
BXVD_P_SETUP_FW_MEMORY_PROTOTYPE(RevE0);

/* Setup FW Memory */
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

#endif
