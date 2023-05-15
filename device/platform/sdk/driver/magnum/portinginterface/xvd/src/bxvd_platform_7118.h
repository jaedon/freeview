/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7118.h $
 * $brcm_Revision: Hydra_Software_Devel/31 $
 * $brcm_Date: 2/2/12 3:04p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7118.h $
 * 
 * Hydra_Software_Devel/31   2/2/12 3:04p davidp
 * SW7425-2024: Define platform specific supported decode protocols mask.
 * 
 * Hydra_Software_Devel/30   9/18/08 2:04p davidp
 * PR46650: Fix MemCfgMode enum field names.
 * 
 * Hydra_Software_Devel/29   9/8/08 6:43p davidp
 * PR46650: Add BXVD_P_GET_MEMORY_CONFIG macro to sanity check heaps
 * passed to BXVD_Open.
 * 
 * Hydra_Software_Devel/28   5/22/08 4:56p davidp
 * PR41411: Add SUB_GISB_ARB REQ_MASK register and AVD mask bit fields to
 * platform register structure.
 * 
 * Hydra_Software_Devel/27   11/15/07 4:23p davidp
 * PR27168: Use core_chip_enable_reve0 routine instead of 7118 specific
 * routine. Routines are not identical.
 * 
 * Hydra_Software_Devel/26   11/6/07 2:21p davidp
 * PR27168: Remove use of SETUP_HEAP macro and associated routines per
 * code review
 * 
 * Hydra_Software_Devel/25   9/28/07 12:23p nilesh
 * PR27168: Consolidate constants to bxvd_platform.h and rename according
 * to XVD conventions
 * 
 * Hydra_Software_Devel/24   8/22/07 3:48p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/23   3/1/07 3:35p pblanco
 * PR26188: Removed SD_PARAM define from all Rev. E0 core platforms.
 * 
 * Hydra_Software_Devel/22   2/27/07 3:17p pblanco
 * PR22867: Added platform specific chip initialization routine.
 * 
 * Hydra_Software_Devel/21   9/7/06 4:30p davidp
 * PR22867: Create and use platform specfic FW memory configuration table.
 * 
 * Hydra_Software_Devel/20   8/21/06 3:21p nilesh
 * PR22673: Fix for possible macro namespace conflicts
 * 
 * Hydra_Software_Devel/19   8/21/06 1:53p nilesh
 * PR22673: Restructure FW Load mechanism to use standard core firmware
 * struct definition
 * 
 * Hydra_Software_Devel/18   8/17/06 12:03p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/PR22673/1   8/16/06 2:01p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/17   8/14/06 5:31p davidp
 * PR22673: Move setup interrupts and setup watchdog routines to
 * bxvd_core_avd_reve0.c
 * 
 * Hydra_Software_Devel/16   8/7/06 11:47a nilesh
 * PR22673: Renamed incorrect rev G to rev E0
 * 
 * Hydra_Software_Devel/15   8/1/06 5:58p davidp
 * PR22673: Add verify watchdog fired platform routine.
 * 
 * Hydra_Software_Devel/14   7/31/06 7:51p davidp
 * PR22673: Add picture buffer address macros for chip specific use.
 * 
 * Hydra_Software_Devel/13   7/31/06 9:37a davidp
 * PR22673: Remove chromaPhys argument from BXVD_P_GET_PIC_BUF_PHYS_ADDR
 * macro.
 * 
 * Hydra_Software_Devel/12   7/27/06 8:14p davidp
 * PR22673: Move chip specific send FW command code to platform files.
 * 
 * Hydra_Software_Devel/11   7/26/06 4:51p davidp
 * PR22673: Cleanup more platform specific code, add hHWMemHeap element in
 * Xvd handle.
 * 
 * Hydra_Software_Devel/10   7/25/06 8:43a pblanco
 * PR22673: Defined platform specific FW image extern definitions.
 * 
 * Hydra_Software_Devel/9   7/24/06 10:59a davidp
 * PR22673: Added SetupFWBuffer platform routines.
 * 
 * Hydra_Software_Devel/8   7/21/06 9:49a pblanco
 * PR22673: Added userdata offset to address conversion code.
 * 
 * Hydra_Software_Devel/7   7/20/06 6:20p davidp
 * PR22673: Add init register pointers and mask platform routines.
 * 
 * Hydra_Software_Devel/6   7/19/06 2:25p pblanco
 * PR22673: Moved conditional defines from bxvd_priv.h to the appropriate
 * platformspecific header files.
 * 
 * Hydra_Software_Devel/5   7/19/06 12:00p pblanco
 * PR22673: Give core functions a standardized form of their names without
 * the Core prefix. This allows us to use the same names for core and
 * platform functions.
 * 
 * Hydra_Software_Devel/4   7/19/06 11:24a pblanco
 * PR22673: Added the remainder of bxvd_priv.c conditionalized code to
 * platform and core modules.
 * 
 * Hydra_Software_Devel/3   7/19/06 8:03a pblanco
 * PR22673: Fixed bad definition for core load function.
 * 
 * Hydra_Software_Devel/2   7/18/06 3:31p pblanco
 * PR22673: Moved FW load to platform appropriate modules for 7400, 7401
 * and 7118.
 * 
 * Hydra_Software_Devel/1   7/18/06 12:10a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 * 
 * Hydra_Software_Devel/PR22673/1   7/17/06 3:07p nilesh
 * PR22673: Restructure on 97118
 *
 ***************************************************************************/

#ifndef _BXVD_PLATFORM_7118_H_
#define _BXVD_PLATFORM_7118_H_

#include "bchp_7118.h"
#include "bchp_avd0_intr2.h"
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

/* Setup Interrupts */
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
#define BXVD_P_USE_INIT_REG_PTRS_7118A0 1
#define BXVD_P_INIT_REG_PTRS BXVD_P_InitRegPtrs_7118A0
BXVD_P_INIT_REG_PTRS_PROTOTYPE(7118A0);

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
