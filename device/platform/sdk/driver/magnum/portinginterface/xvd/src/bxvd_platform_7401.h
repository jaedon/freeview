/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7401.h $
 * $brcm_Revision: Hydra_Software_Devel/28 $
 * $brcm_Date: 4/5/12 5:21p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7401.h $
 * 
 * Hydra_Software_Devel/28   4/5/12 5:21p davidp
 * SW7344-306: Printout AVD HW revision in BXVD_Open.
 * 
 * Hydra_Software_Devel/27   2/2/12 3:04p davidp
 * SW7425-2024: Define platform specific supported decode protocols mask.
 * 
 * Hydra_Software_Devel/26   9/18/08 2:02p davidp
 * PR46650: Fix MemCfgMode enum field names.
 * 
 * Hydra_Software_Devel/25   9/8/08 6:43p davidp
 * PR46650: Add BXVD_P_GET_MEMORY_CONFIG macro to sanity check heaps
 * passed to BXVD_Open.
 * 
 * Hydra_Software_Devel/24   5/22/08 4:52p davidp
 * PR41411: Add verification that the AVD can access GISB.
 * 
 * Hydra_Software_Devel/23   11/6/07 2:21p davidp
 * PR27168: Remove use of SETUP_HEAP macro and associated routines per
 * code review
 * 
 * Hydra_Software_Devel/22   9/28/07 12:23p nilesh
 * PR27168: Consolidate constants to bxvd_platform.h and rename according
 * to XVD conventions
 * 
 * Hydra_Software_Devel/21   8/22/07 3:48p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/20   3/1/07 3:59p pblanco
 * PR26188: Removed SD_PARAM from all E0 based platforms.
 * 
 * Hydra_Software_Devel/19   9/7/06 4:30p davidp
 * PR22867: Create and use platform specfic FW memory configuration table.
 * 
 * Hydra_Software_Devel/18   9/5/06 3:05p davidp
 * PR22867: Merge secure heap (Cabac) support
 * 
 * Hydra_Software_Devel/xvd_cabac_heap/1   8/30/06 5:11p davidp
 * PR22867: Separate FW memory so Cabac bin memory can me moved to secure
 * heap.
 * 
 * Hydra_Software_Devel/17   8/21/06 3:21p nilesh
 * PR22673: Fix for possible macro namespace conflicts
 * 
 * Hydra_Software_Devel/16   8/21/06 1:53p nilesh
 * PR22673: Restructure FW Load mechanism to use standard core firmware
 * struct definition
 * 
 * Hydra_Software_Devel/15   8/17/06 12:03p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/PR22673/1   8/16/06 2:02p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/14   8/14/06 5:30p davidp
 * PR22673: Move setup interrupts and setup watchdog routines to
 * bxvd_core_avd_reve0.c
 * 
 * Hydra_Software_Devel/13   8/7/06 11:47a nilesh
 * PR22673: Renamed incorrect rev G to rev E0
 * 
 * Hydra_Software_Devel/12   8/1/06 5:58p davidp
 * PR22673: Add verify watchdog fired platform routine.
 * 
 * Hydra_Software_Devel/11   7/31/06 7:50p davidp
 * PR22673: Add picture buffer address macros for chip specific use.
 * 
 * Hydra_Software_Devel/10   7/26/06 4:50p davidp
 * PR22673: Cleanup more platform specific code, add hHWMemHeap element in
 * Xvd handle.
 * 
 * Hydra_Software_Devel/9   7/25/06 8:42a pblanco
 * PR22673: Defined platform specific FW image extern definitions.
 * 
 * Hydra_Software_Devel/8   7/24/06 10:59a davidp
 * PR22673: Added SetupFWBuffer platform routines.
 * 
 * Hydra_Software_Devel/7   7/21/06 9:49a pblanco
 * PR22673: Added userdata offset to address conversion code.
 * 
 * Hydra_Software_Devel/6   7/20/06 6:20p davidp
 * PR22673: Add init register pointers and mask platform routines.
 * 
 * Hydra_Software_Devel/5   7/19/06 2:25p pblanco
 * PR22673: Moved conditional defines from bxvd_priv.h to the appropriate
 * platform specific header files.
 * 
 * Hydra_Software_Devel/4   7/19/06 11:59a pblanco
 * PR22673: Give core functions a standardized form of their names without
 * the Core prefix. This allows us to use the same names for core and
 * platform functions.
 * 
 * Hydra_Software_Devel/3   7/19/06 11:23a pblanco
 * PR22673: Added the remainder of bxvd_priv.c conditionalized code to
 * platform and core modules.
 * 
 * Hydra_Software_Devel/2   7/18/06 3:30p pblanco
 * PR22673: Moved FW load to platform appropriate modules for 7400, 7401
 * and 7118.
 * 
 * Hydra_Software_Devel/1   7/18/06 12:11a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 * 
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:20p nilesh
 * PR22673: Code Restructure
 *
 ***************************************************************************/

#ifndef _BXVD_PLATFORM_7401_H_
#define _BXVD_PLATFORM_7401_H_

#include "bchp_7401.h"
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
/* AVD Rev E */
#define BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS  BXVD_P_REVE_DECODE_PROTOCOLS_MASK
#define BXVD_P_CORE_REVISION 'E'

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
#define BXVD_P_USE_CORE_RESET_CHIP_REVE0 1
#define BXVD_P_RESET_CHIP BXVD_P_ChipReset_RevE0
BXVD_P_CHIP_RESET_PROTOTYPE(RevE0);

/* Init Register Pointers/Masks */
#define BXVD_P_USE_INIT_REG_PTRS_7401B0 1
#define BXVD_P_INIT_REG_PTRS BXVD_P_InitRegPtrs_7401B0
BXVD_P_INIT_REG_PTRS_PROTOTYPE(7401B0);

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
