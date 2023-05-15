/***************************************************************************
 *     Copyright (c) 2006-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7440.h $
 * $brcm_Revision: Hydra_Software_Devel/25 $
 * $brcm_Date: 9/18/08 2:02p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7440.h $
 * 
 * Hydra_Software_Devel/25   9/18/08 2:02p davidp
 * PR46650: Fix MemCfgMode enum field names.
 * 
 * Hydra_Software_Devel/24   9/8/08 6:43p davidp
 * PR46650: Add BXVD_P_GET_MEMORY_CONFIG macro to sanity check heaps
 * passed to BXVD_Open.
 * 
 * Hydra_Software_Devel/23   6/12/08 4:47p nilesh
 * PR43585: Updated multi-decode XVD to compile with 7440
 * 
 * Hydra_Software_Devel/22   11/6/07 5:54p davidp
 * PR27168: Remove use of BXVD_P_ChipEnable_RevE1 routine and associated
 * data structure, use RevE0
 * 
 * Hydra_Software_Devel/21   11/6/07 2:20p davidp
 * PR27168: Remove use of SETUP_HEAP macro and associated routines per
 * code review
 * 
 * Hydra_Software_Devel/20   9/28/07 12:23p nilesh
 * PR27168: Consolidate constants to bxvd_platform.h and rename according
 * to XVD conventions
 * 
 * Hydra_Software_Devel/19   8/22/07 3:49p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/18   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/1   7/3/07 12:09p nilesh
 * PR29915: Added FGT support
 * 
 * Hydra_Software_Devel/17   5/14/07 11:30a nilesh
 * PR30249: Merged UOD 2.x changes to mainline
 * 
 * Hydra_Software_Devel/PR30249/2   5/10/07 2:09p nilesh
 * PR30249: Added support for new stripe_multiple parameter in AVD Init
 * 
 * Hydra_Software_Devel/PR30249/1   5/10/07 1:57p nilesh
 * PR30249: Updated memory configuration table to account for larger
 * stripe height (64) on 7440
 * 
 * Hydra_Software_Devel/16   4/24/07 1:44p nilesh
 * PR22766: Added support for platform specific FGT buffer allocation
 * 
 * Hydra_Software_Devel/15   3/2/07 8:43a pblanco
 * PR26188: Removed SD_PARAM define from all Rev. E0 core platforms.
 * 
 * Hydra_Software_Devel/14   10/17/06 2:28p nilesh
 * PR23113: Enable DVD-specific API by defining BXVD_ENABLE_DVD_API for
 * 7440
 * 
 * Hydra_Software_Devel/13   10/16/06 1:52p nilesh
 * PR23113: Added support for AVD 1 Core interrupts
 * 
 * Hydra_Software_Devel/12   10/10/06 12:23p nilesh
 * PR23113: Disable DVD-specific APIs until FW/DM supports it in the main
 * branch
 * 
 * Hydra_Software_Devel/11   10/9/06 6:28p nilesh
 * PR23113: Added DVD-specific API calls: SetClipTime() and
 * [Set/Get]PtsErrorReport[Upper/Lower]Threshold()
 * 
 * Hydra_Software_Devel/10   9/8/06 2:35p nilesh
 * PR23113: Initial support for dual decode
 * 
 * Hydra_Software_Devel/9   9/8/06 10:38a nilesh
 * PR23113: Update to use core rev e1
 * 
 * Hydra_Software_Devel/8   9/7/06 4:32p davidp
 * PR22867: Create and use platform specfic FW memory configuration table.
 * 
 * Hydra_Software_Devel/7   9/5/06 3:05p davidp
 * PR22867: Merge secure heap (Cabac) support
 * 
 * Hydra_Software_Devel/xvd_cabac_heap/1   8/31/06 9:50a nilesh
 * PR22867: Separate FW memory so Cabac bin memory can me moved to secure
 * heap.
 * 
 * Hydra_Software_Devel/5   8/21/06 3:22p nilesh
 * PR22673: Fix for possible macro namespace conflicts
 * 
 * Hydra_Software_Devel/4   8/21/06 1:53p nilesh
 * PR22673: Restructure FW Load mechanism to use standard core firmware
 * struct definition
 * 
 * Hydra_Software_Devel/3   8/18/06 2:48p nilesh
 * PR23113: Updated to use 7440 include files (single decode)
 * 
 * Hydra_Software_Devel/2   8/17/06 12:03p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/PR22673/1   8/16/06 3:07p nilesh
 * PR23113: 7440 Platform Support
 * 
 * Hydra_Software_Devel/1   8/9/06 11:10a nilesh
 * PR23113: Add framework to support 7440 XVD
 *
 ***************************************************************************/

#ifndef _BXVD_PLATFORM_7440_H_
#define _BXVD_PLATFORM_7440_H_

#include "bchp_7440.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_xpt_pcroffset0.h"
#include "bchp_xpt_pcroffset1.h"

/* AVD 0 */
#include "bchp_avd_intr2_0.h"
#include "bchp_int_id_avd_intr2_0.h"
#include "bchp_decode_cpuregs2_0.h"
#include "bchp_decode_cpuregs_0.h"
#include "bchp_decode_ind_sdram_regs2_0.h"
#include "bchp_decode_cpuaux2_0.h"

/* AVD 1 */
#include "bchp_avd_intr2_1.h"
#include "bchp_int_id_avd_intr2_1.h"
#include "bchp_decode_cpuregs2_1.h"
#include "bchp_decode_cpuregs_1.h"
#include "bchp_decode_ind_sdram_regs2_1.h"
#include "bchp_decode_cpuaux2_1.h"

#define BXVD_MAX_INSTANCE_COUNT     2

#define TEST_PORT_CONTROL_VALUE_ARC_0 0x00000011
#define TEST_PORT_CONTROL_VALUE_ARC_1 0x00000011

#define BXVD_P_GET_MEMORY_CONFIG(hXvd, eMemCfgMode)  \
      eMemCfgMode = BXVD_P_MemCfgMode_eUMA;

/*********************
 * Setup Interrupts
 *********************/
#define BXVD_P_USE_SETUP_INTERRUPTS_REVE0 1 
#define BXVD_P_SETUP_INTERRUPTS BXVD_P_SetupInterrupts_RevE0 
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
#define BXVD_P_USE_INIT_REG_PTRS_7440A0 1
#define BXVD_P_INIT_REG_PTRS BXVD_P_InitRegPtrs_7440A0
BXVD_P_INIT_REG_PTRS_PROTOTYPE(7440A0);

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

/* Enable DVD-specific code */
#define BXVD_ENABLE_DVD_API 1
#define BXVD_P_AVD_INIT_STRIPE_MULTIPLE 2 /* odd-128 (from Gaurav A) */
#endif
