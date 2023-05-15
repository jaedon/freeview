/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7400.h $
 * $brcm_Revision: Hydra_Software_Devel/45 $
 * $brcm_Date: 4/5/12 5:21p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7400.h $
 * 
 * Hydra_Software_Devel/45   4/5/12 5:21p davidp
 * SW7344-306: Printout AVD HW revision in BXVD_Open.
 * 
 * Hydra_Software_Devel/44   2/10/12 3:22p davidp
 * SW7400-3067: Set decode protocols mask to include Dixv.
 * 
 * Hydra_Software_Devel/43   2/2/12 3:04p davidp
 * SW7425-2024: Define platform specific supported decode protocols mask.
 * 
 * Hydra_Software_Devel/42   7/25/11 3:28p davidp
 * SW7231-127: Fix compiler warning.
 * 
 * Hydra_Software_Devel/41   7/25/11 3:08p davidp
 * SW7231-127: Move old style PM support from 7405 specific to Rev E0
 * specific support file.
 * 
 * Hydra_Software_Devel/40   9/18/08 2:02p davidp
 * PR46650: Fix MemCfgMode enum field names.
 * 
 * Hydra_Software_Devel/39   9/8/08 6:43p davidp
 * PR46650: Add BXVD_P_GET_MEMORY_CONFIG macro to sanity check heaps
 * passed to BXVD_Open.
 * 
 * Hydra_Software_Devel/38   11/20/07 2:28p davidp
 * PR27168: Move setup of TEST_PORT_CTRL register to BSP module.
 * 
 * Hydra_Software_Devel/37   11/6/07 5:53p davidp
 * PR27168: Remove use of BXVD_P_ChipEnable_RevE1 routine and associated
 * data structure, use RevE0
 * 
 * Hydra_Software_Devel/36   11/6/07 2:20p davidp
 * PR27168: Remove use of SETUP_HEAP macro and associated routines per
 * code review
 * 
 * Hydra_Software_Devel/35   9/28/07 12:23p nilesh
 * PR27168: Consolidate constants to bxvd_platform.h and rename according
 * to XVD conventions
 * 
 * Hydra_Software_Devel/34   8/22/07 3:48p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/33   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/3   8/10/07 1:07p davidp
 * PR29915: Remove unneeded validate macro, use correct parameter for
 * validate_pdefsettings macro.
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/2   7/2/07 1:53p nilesh
 * PR29915: Consolidated platform memory allocation code
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/1   6/15/07 11:36a nilesh
 * PR29915: Updated memory tables according to latest AVD Core Mosaic API
 * document
 * 
 * Hydra_Software_Devel/32   3/1/07 3:11p pblanco
 * PR26188: Removed definition of SD_PARAM register.
 * 
 * Hydra_Software_Devel/31   2/26/07 5:44p nilesh
 * PR26188: Fix 7400B0 memory setup issues
 * 
 * Hydra_Software_Devel/30   2/26/07 3:41p davidp
 * PR26188: Use correct heaps for picture buffers and FW code/context
 * memory
 * 
 * Hydra_Software_Devel/29   2/13/07 1:49p pblanco
 * PR26188: Fixed setup heap and chip reset routine names.
 * 
 * Hydra_Software_Devel/28   2/6/07 9:14a pblanco
 * PR26188: 7400B0 code changes.
 * 
 * Hydra_Software_Devel/27   1/30/07 10:39a pblanco
 * PR26188: Another luma/chroma buffer address name change.
 * 
 * Hydra_Software_Devel/26   1/30/07 10:32a pblanco
 * PR26188: Change luma and chroma buffer names to reflect PPB changes.
 * 
 * Hydra_Software_Devel/25   11/28/06 12:38p pblanco
 * PR26188: Final changes. Both A0 and B0 build properly now.
 * 
 * Hydra_Software_Devel/24   11/27/06 4:01p pblanco
 * PR26188: Sanity checkin. Added more 7400B0 specific includes and fixed
 * references to new RDB constants.
 * 
 * Hydra_Software_Devel/23   11/27/06 2:57p pblanco
 * PR26188: Conditionally include intr2 header for 7400B0.
 * 
 * Hydra_Software_Devel/22   10/30/06 12:22p davidp
 * PR25098: Increase 7400 watchdog timeout value to approx 750ms
 * 
 * Hydra_Software_Devel/21   9/8/06 10:41a nilesh
 * PR22673: Moved TEST_PORT_CONTROL define to platform header file
 * 
 * Hydra_Software_Devel/20   9/7/06 4:30p davidp
 * PR22867: Create and use platform specfic FW memory configuration table.
 * 
 * Hydra_Software_Devel/19   8/21/06 3:21p nilesh
 * PR22673: Fix for possible macro namespace conflicts
 * 
 * Hydra_Software_Devel/18   8/1/06 6:05p davidp
 * PR22673: Add XPT include file.
 * 
 * Hydra_Software_Devel/17   8/1/06 5:58p davidp
 * PR22673: Add verify watchdog fired platform routine.
 * 
 * Hydra_Software_Devel/16   8/1/06 12:26p davidp
 * PR22673: Init picture buffer heap pointer in xvd_to_vdc structure in
 * SetupFWMemory().
 * 
 * Hydra_Software_Devel/15   7/31/06 7:51p davidp
 * PR22673: Add picture buffer address macros for chip specific use.
 * 
 * Hydra_Software_Devel/14   7/31/06 9:15a pblanco
 * PR22673: Fix bad checkin comment. Meant 7400 not 7499...
 * 
 * Hydra_Software_Devel/13   7/31/06 9:13a pblanco
 * PR22673: Moved 7499 specific BAVC definitions here.
 * 
 * Hydra_Software_Devel/12   7/27/06 8:14p davidp
 * PR22673: Move chip specific send FW command code to platform files.
 * 
 * Hydra_Software_Devel/11   7/26/06 4:51p davidp
 * PR22673: Cleanup more platform specific code, add hHWMemHeap element in
 * Xvd handle.
 * 
 * Hydra_Software_Devel/10   7/25/06 8:42a pblanco
 * PR22673: Defined platform specific FW image extern definitions.
 * 
 * Hydra_Software_Devel/9   7/24/06 10:58a davidp
 * PR22673: Added SetupFWBuffer platform routines.
 * 
 * Hydra_Software_Devel/8   7/21/06 9:49a pblanco
 * PR22673: Added userdata offset to address conversion code.
 * 
 * Hydra_Software_Devel/7   7/20/06 6:33p davidp
 * PR22673: Add init register pointers and mask platform routines.
 * 
 * Hydra_Software_Devel/6   7/20/06 2:23p nilesh
 * PR22673: Created BXVD_P_CONTEXT_PLATFORM macro to allow platform-
 * specific fields in BXVD_P_Context struct
 * 
 * Hydra_Software_Devel/5   7/19/06 2:25p pblanco
 * PR22673: Moved conditional defines from bxvd_priv.h to the appropriate
 * platformspecific header files.
 * 
 * Hydra_Software_Devel/4   7/19/06 11:49a pblanco
 * PR22673: Fix code for 7400.
 * 
 * Hydra_Software_Devel/3   7/19/06 11:23a pblanco
 * PR22673: Added the remainder of bxvd_priv.c conditionalized code to
 * platform and core modules.
 * 
 * Hydra_Software_Devel/2   7/18/06 3:31p pblanco
 * PR22673: Moved FW load to platform appropriate modules for 7400, 7401
 * and 7118.
 * 
 * Hydra_Software_Devel/1   7/18/06 12:10a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 * 
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:54p nilesh
 * PR22673: Restructure on 97400
 *
 ***************************************************************************/

#ifndef _BXVD_PLATFORM_7400_H_
#define _BXVD_PLATFORM_7400_H_
 
#include "bchp_7400.h"
#include "bchp_avd_intr2_0.h"
#include "bchp_avd_intr2_1.h"
#include "bchp_decode_cpuregs2_0.h"
#include "bchp_decode_cpuregs2_1.h"
#include "bchp_decode_cpuregs_0.h"
#include "bchp_decode_cpuregs_1.h"
#include "bchp_xpt_pcroffset.h"
#include "bchp_int_id_avd_intr2_0.h"
#include "bchp_int_id_avd_intr2_1.h"
#include "bchp_decode_ind_sdram_regs_0.h"
#include "bchp_decode_ind_sdram_regs_1.h"
#include "bchp_decode_cpuaux_0.h"
#include "bchp_decode_cpuaux_1.h"
#include "bchp_decode_cpuaux2_0.h"
#include "bchp_decode_cpuaux2_1.h"
#include "bchp_decode_ind_sdram_regs2_0.h"
#include "bchp_decode_ind_sdram_regs2_1.h"
#include "bchp_decode_ip_shim_0.h"
#include "bchp_decode_ip_shim_1.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_sun_l2.h"
#include "bchp_sun_top_ctrl.h"

#include "bxvd_priv.h"

#define BXVD_MAX_INSTANCE_COUNT     2

#define BXVD_P_VALIDATE_PDEFSETTINGS(pDefSettings)               \
{                                                                \
   if (pDefSettings->hFrmMemory == NULL)                         \
   {                                                             \
      BDBG_ERR(("Framework Heap device Settings not supplied")); \
      return BERR_TRACE(BERR_INVALID_PARAMETER);                 \
   }                                                             \
}

#define BXVD_P_GET_MEMORY_CONFIG(hXvd, eMemCfgMode)  \
      eMemCfgMode = BXVD_P_MemCfgMode_eNONUMA;

/* Define supported decode protocols mask */ 
/* AVD Rev H */
#define BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS  BXVD_P_REVH_DECODE_PROTOCOLS_MASK
#define BXVD_P_CORE_REVISION 'H'

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
#define BXVD_P_USE_INIT_REG_PTRS_7400B0 1
#define BXVD_P_INIT_REG_PTRS BXVD_P_InitRegPtrs_7400B0
BXVD_P_INIT_REG_PTRS_PROTOTYPE(7400B0);

/* Setup FW Memory  */
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

#if BXVD_P_POWER_MANAGEMENT

#define BXVD_P_USE_SET_POWER_STATE_REVE0 1
#define BXVD_P_SET_POWER_STATE BXVD_P_SetPowerState_RevE0

void BXVD_P_SetPowerState_RevE0(BXVD_Handle hXvd,
                               BXVD_P_PowerState PowerStateNew);
#endif

#endif
