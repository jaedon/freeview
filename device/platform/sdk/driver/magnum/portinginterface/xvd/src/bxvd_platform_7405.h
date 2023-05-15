/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7405.h $
 * $brcm_Revision: Hydra_Software_Devel/35 $
 * $brcm_Date: 4/5/12 5:21p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7405.h $
 * 
 * Hydra_Software_Devel/35   4/5/12 5:21p davidp
 * SW7344-306: Printout AVD HW revision in BXVD_Open.
 * 
 * Hydra_Software_Devel/34   2/2/12 3:04p davidp
 * SW7425-2024: Define platform specific supported decode protocols mask.
 * 
 * Hydra_Software_Devel/33   7/26/11 4:27p davidp
 * SW7231-127: Move PM2 7420 flavor to core reve0 support files, this
 * method is also used on other platforms.
 * 
 * Hydra_Software_Devel/32   7/25/11 3:08p davidp
 * SW7231-127: Move old style PM support from 7405 specific to Rev E0
 * specific support file.
 * 
 * Hydra_Software_Devel/31   7/21/11 6:47a davidp
 * SW7231-127: Fix compiler warnings.
 * 
 * Hydra_Software_Devel/30   7/20/11 3:27p davidp
 * SW7231-127: Add BXVD_Standby and BXVD_Resume support.
 * 
 * Hydra_Software_Devel/29   5/14/10 6:29p davidp
 * SW7125-426: Add dynamic power management support for 7125, disabled by
 * default.
 * 
 * Hydra_Software_Devel/28   11/16/09 6:33p davidp
 * SW7468-14: Add support for 7468 and 7408 platforms.
 * 
 * Hydra_Software_Devel/27   9/16/09 5:05p davidp
 * SW3548-2458: Stop IL ARC and reset AVD core hw blocks in reset routine.
 * 
 * Hydra_Software_Devel/26   7/23/09 1:44p davidp
 * PR55546: Add 7125 support to xvd.
 * 
 * Hydra_Software_Devel/25   7/17/09 4:28p davidp
 * PR55227: Add support for 7340 and 7342 support.
 * 
 * Hydra_Software_Devel/24   6/2/09 5:15p davidp
 * PR55198: Remove unused define constants.
 * 
 * Hydra_Software_Devel/23   6/1/09 6:38p davidp
 * PR55198: Add AVD power management register pointers and masks to
 * register table.
 * 
 * Hydra_Software_Devel/22   1/30/09 1:16p davidp
 * PR51623: Add support for 7336 chip, Rev I core clone.
 * 
 * Hydra_Software_Devel/21   12/23/08 1:19p davidp
 * PR50640: Add support for 7405 C0.
 * 
 * Hydra_Software_Devel/20   10/31/08 3:31p davidp
 * PR46543: Implement BXVD_POWER_MANAGEMENT build switch.
 * 
 * Hydra_Software_Devel/19   10/8/08 2:30p davidp
 * PR46543: Disable power management functionality.
 * 
 * Hydra_Software_Devel/18   10/3/08 5:29p davidp
 * PR46543: Add power management support for 3548/3556.
 * 
 * Hydra_Software_Devel/17   9/18/08 2:02p davidp
 * PR46650: Fix MemCfgMode enum field names.
 * 
 * Hydra_Software_Devel/16   9/8/08 6:45p davidp
 * PR46650: Add BXVD_P_GET_MEMORY_CONFIG macro to sanity check heaps
 * passed to BXVD_Open.
 * 
 * Hydra_Software_Devel/15   5/22/08 4:55p davidp
 * PR41411: Add SUB_GISB_ARB REQ_MASK register and AVD mask bit fields to
 * platform register structure.
 * 
 * Hydra_Software_Devel/14   1/25/08 7:02p davidp
 * PR38884: Add decode stripe width register reference pointers/mask/shift
 * items to register table to support 7405 B0 differences.
 * 
 * Hydra_Software_Devel/13   11/20/07 2:28p davidp
 * PR27168: Move setup of TEST_PORT_CTRL register to BSP module.
 * 
 * Hydra_Software_Devel/12   11/13/07 5:38p davidp
 * PR36901: Add support for 97335, use BXVD_CHIP families, 97335 is same
 * as 97405.
 * 
 * Hydra_Software_Devel/11   11/6/07 2:20p davidp
 * PR27168: Remove use of SETUP_HEAP macro and associated routines per
 * code review
 * 
 * Hydra_Software_Devel/10   9/28/07 12:23p nilesh
 * PR27168: Consolidate constants to bxvd_platform.h and rename according
 * to XVD conventions
 * 
 * Hydra_Software_Devel/9   8/22/07 3:48p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/8   8/13/07 4:09p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/4   7/13/07 5:15p nilesh
 * PR29915: Updated memory tables based on v2.9 of Mosaic API doc
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/3   7/10/07 7:40p nilesh
 * PR29915: Cleaned up 7405 platform code
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/2   7/10/07 6:46p nilesh
 * PR29915: Cleaned up 7405 platform code
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/1   7/10/07 6:18p nilesh
 * PR29915: Added 7405 support to mosaic branch
 * 
 * Hydra_Software_Devel/7   7/10/07 2:57p nilesh
 * PR28215: Added support for non-UMA mode
 * 
 * Hydra_Software_Devel/6   7/6/07 7:42a pblanco
 * PR28215: Added chip specific reset routine.
 * 
 * Hydra_Software_Devel/5   6/28/07 3:57p pblanco
 * PR28215: Bringup changes - sanity checkin.
 * 
 * Hydra_Software_Devel/4   6/22/07 11:26a pblanco
 * PR28215: Added platform specfic chip init code.
 * 
 * Hydra_Software_Devel/3   6/22/07 11:08a pblanco
 * PR28215: Use proper value for platform specific test port control.
 * 
 * Hydra_Software_Devel/2   5/15/07 1:49p pblanco
 * PR28215: Moved includes that broke the build from .c file.
 * 
 * Hydra_Software_Devel/1   4/19/07 9:59a pblanco
 * PR27645: Adding to source control.
 * 
 ***************************************************************************/

#ifndef _BXVD_PLATFORM_7405_H_
#define _BXVD_PLATFORM_7405_H_

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
#include "bchp_decode_main_0.h"
#include "bchp_decode_sint_0.h"
#include "bchp_decode_sint_oloop_0.h"
#include "bchp_decode_cpuaux2_0.h"
#include "bchp_decode_cpuaux_0.h"
#include "bchp_decode_sd_0.h"
#include "bchp_decode_ip_shim_0.h"

#include "bxvd_priv.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif


/* Macro to determine memory configuration, UMA, NON-UMA or UNKNOWN */

/* 7405 A0/A1, 7335 */
#if (((BCHP_CHIP == 7405) && ((BCHP_VER == BCHP_VER_A0) || (BCHP_VER == BCHP_VER_A1))) || \
       (BCHP_CHIP == 7335))
#define BXVD_P_GET_MEMORY_CONFIG(hXvd, eMemCfgMode)  \
      eMemCfgMode = BXVD_P_MemCfgMode_eUNKNOWN;

/* 7325 */
#elif (BCHP_CHIP == 7325)                                                     
#define BXVD_P_GET_MEMORY_CONFIG(hXvd, eMemCfgMode)  \
      eMemCfgMode = BXVD_P_MemCfgMode_eUMA;                                   

/* 7405 B0+,3548, 3556 */
#elif ((BCHP_CHIP == 7405) && (BCHP_VER >= BCHP_VER_B0)) || \
      (BCHP_CHIP == 3548) || \
      (BCHP_CHIP == 3556) || \
      (BCHP_CHIP == 7336) || \
      (BCHP_CHIP == 7340) || \
      (BCHP_CHIP == 7342) || \
      (BCHP_CHIP == 7125) || \
      (BCHP_CHIP == 7408) || \
      (BCHP_CHIP == 7468)

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
#endif

/* Define clock register and mask values */
#if (BCHP_CHIP == 7405) 
#define BXVD_P_AVD0_CTRL                       BCHP_VCXO_CTL_MISC_AVD_CTRL
#define BXVD_P_AVD0_CTRL_PWRDN_MASK            BCHP_VCXO_CTL_MISC_AVD_CTRL_POWERDOWN_MASK
#define BXVD_P_AVD0_CORE_CLK_CTRL              BCHP_CLK_PM_CTRL_1
#define BXVD_P_AVD0_CORE_CLK_CTRL_PWRDN_MASK   BCHP_CLK_PM_CTRL_1_DIS_AVD0_PROG_CLK_MASK
#define BXVD_P_AVD0_SCB_CLK_CTRL               BCHP_CLK_PM_CTRL_2
#define BXVD_P_AVD0_SCB_CLK_CTRL_PWRDWN_MASK   BCHP_CLK_PM_CTRL_2_DIS_AVD_216M_CLK_MASK
#define BXVD_P_AVD0_GISB_CLK_CTRL              BCHP_CLK_PM_CTRL
#define BXVD_P_AVD0_GISB_CLK_CTRL_PWRDWN_MASK  BCHP_CLK_PM_CTRL_DIS_AVD0_108M_CLK_MASK

#elif (BCHP_CHIP == 7325)
#define BXVD_P_AVD0_CTRL                          BCHP_CLKGEN_AVD_CTRL
#define BXVD_P_AVD0_CTRL_PWRDN_MASK               BCHP_CLKGEN_AVD_CTRL_POWERDOWN_MASK
#define BXVD_P_AVD0_CORE_CLK_CTRL                 BCHP_CLKGEN_PWRDN_CTRL_3
#define BXVD_P_AVD0_CORE_CLK_CTRL_PWRDN_MASK      BCHP_CLKGEN_PWRDN_CTRL_3_PWRDN_CLOCK_AVD0_PROG_MASK
#define BXVD_P_AVD0_SCB_CLK_CTRL                  BCHP_CLKGEN_PWRDN_CTRL_0    
#define BXVD_P_AVD0_SCB_CLK_CTRL_PWRDWN_MASK      BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_AVD_MASK
#define BXVD_P_AVD0_GISB_CLK_CTRL                 BCHP_CLKGEN_PWRDN_CTRL_1
#define BXVD_P_AVD0_GISB_CLK_CTRL_PWRDWN_MASK     BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_AVD_MASK   

#elif (BCHP_CHIP == 7335)
#define BXVD_P_AVD0_CTRL                          BCHP_VCXO_CTL_MISC_AVD_CTRL
#define BXVD_P_AVD0_CTRL_PWRDN_MASK               BCHP_VCXO_CTL_MISC_AVD_CTRL_POWERDOWN_MASK
#define BXVD_P_AVD0_CORE_CLK_CTRL                 BCHP_CLK_PM_CTRL_1
#define BXVD_P_AVD0_CORE_CLK_CTRL_PWRDN_MASK      BCHP_CLK_PM_CTRL_1_DIS_AVD0_PROG_CLK_MASK
#define BXVD_P_AVD0_SCB_CLK_CTRL                  BCHP_CLK_PM_CTRL_2
#define BXVD_P_AVD0_SCB_CLK_CTRL_PWRDWN_MASK      BCHP_CLK_PM_CTRL_2_DIS_AVD_216M_CLK_MASK
#define BXVD_P_AVD0_GISB_CLK_CTRL                 BCHP_CLK_PM_CTRL
#define BXVD_P_AVD0_GISB_CLK_CTRL_PWRDWN_MASK     BCHP_CLK_PM_CTRL_DIS_AVD0_108M_CLK_MASK

#elif (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556) 
#define BXVD_P_AVD0_CTRL                        BCHP_VCXO_CTL_MISC_AVD_CTRL
#define BXVD_P_AVD0_CTRL_PWRDN_MASK             BCHP_VCXO_CTL_MISC_AVD_CTRL_POWERDOWN_MASK
#define BXVD_P_AVD0_CORE_CLK_CTRL               BCHP_CLKGEN_PWRDN_CTRL_3
#define BXVD_P_AVD0_CORE_CLK_CTRL_PWRDN_MASK    BCHP_CLKGEN_PWRDN_CTRL_3_PWRDN_CLOCK_250_CG_AVD_MASK
#define BXVD_P_AVD0_SCB_CLK_CTRL                BCHP_CLKGEN_PWRDN_CTRL_0
#define BXVD_P_AVD0_SCB_CLK_CTRL_PWRDWN_MASK    BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_AVD_MASK
#define BXVD_P_AVD0_GISB_CLK_CTRL               BCHP_CLKGEN_PWRDN_CTRL_1
#define BXVD_P_AVD0_GISB_CLK_CTRL_PWRDWN_MASK   BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_AVD_MASK

#elif (BCHP_CHIP == 7125) && (BCHP_VER < BCHP_VER_C0)
#define BXVD_P_AVD0_CTRL                        BCHP_VCXO_CTL_MISC_RAP_AVD_PLL_CHL_6
#define BXVD_P_AVD0_CTRL_PWRDN_MASK             BCHP_VCXO_CTL_MISC_RAP_AVD_PLL_CHL_6_DIS_CH_MASK
#define BXVD_P_AVD0_CORE_CLK_CTRL               BCHP_CLKGEN_PWRDN_CTRL_0
#define BXVD_P_AVD0_CORE_CLK_CTRL_PWRDN_MASK    BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_250_AVD_MASK
#define BXVD_P_AVD0_SCB_CLK_CTRL                BCHP_CLKGEN_PWRDN_CTRL_1
#define BXVD_P_AVD0_SCB_CLK_CTRL_PWRDWN_MASK    BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_216_AVD0_MASK
#define BXVD_P_AVD0_GISB_CLK_CTRL               BCHP_CLKGEN_PWRDN_CTRL_2
#define BXVD_P_AVD0_GISB_CLK_CTRL_PWRDWN_MASK   BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_AVD0_MASK

#elif (BCHP_CHIP == 7125) && (BCHP_VER >= BCHP_VER_C0)
#define BXVD_P_AVD0_CTRL                        BCHP_VCXO_CTL_MISC_RAP_AVD_PLL_CHL_6
#define BXVD_P_AVD0_CTRL_PWRDN_MASK             BCHP_VCXO_CTL_MISC_RAP_AVD_PLL_CHL_6_DIS_CH_MASK
#define BXVD_P_AVD0_CORE_CLK_CTRL               BCHP_CLKGEN_AVD_CLK_PM_CTRL
#define BXVD_P_AVD0_CORE_CLK_CTRL_PWRDN_MASK    BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_250_MASK
#define BXVD_P_AVD0_SCB_CLK_CTRL                BCHP_CLKGEN_AVD_CLK_PM_CTRL
#define BXVD_P_AVD0_SCB_CLK_CTRL_PWRDWN_MASK    BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_216_MASK
#define BXVD_P_AVD0_GISB_CLK_CTRL               BCHP_CLKGEN_AVD_CLK_PM_CTRL
#define BXVD_P_AVD0_GISB_CLK_CTRL_PWRDWN_MASK   BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_108_MASK


#elif (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) || (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) 
#define BXVD_P_AVD0_CTRL                        0x00
#define BXVD_P_AVD0_CTRL_PWRDN_MASK             0x00
#define BXVD_P_AVD0_CORE_CLK_CTRL               0x00
#define BXVD_P_AVD0_CORE_CLK_CTRL_PWRDN_MASK    0x00
#define BXVD_P_AVD0_SCB_CLK_CTRL                0x00
#define BXVD_P_AVD0_SCB_CLK_CTRL_PWRDWN_MASK    0x00
#define BXVD_P_AVD0_GISB_CLK_CTRL               0x00
#define BXVD_P_AVD0_GISB_CLK_CTRL_PWRDWN_MASK   0x00
#endif

#define BXVD_MAX_INSTANCE_COUNT     1

/* UART clock frequency */
#define BXVD_P_AVD_CORE_UART_FREQ (250*1000000) 

/* Define supported decode protocols mask */ 
#if ((BCHP_CHIP == 7335) || (BCHP_CHIP == 7325))
/* AVD Rev H */
#define BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS  BXVD_P_REVH_DECODE_PROTOCOLS_MASK
#define BXVD_P_CORE_REVISION 'H'

#else
/* AVD Rev I */
#define BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS  BXVD_P_REVI_DECODE_PROTOCOLS_MASK
#define BXVD_P_CORE_REVISION 'I'
#endif

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
#define BXVD_P_USE_CORE_RESET_CHIP_7405 1
#define BXVD_P_RESET_CHIP BXVD_P_ChipReset_7405
BXVD_P_CHIP_RESET_PROTOTYPE(7405);

/* Init Register Pointers/Masks */
#define BXVD_P_USE_INIT_REG_PTRS_7405 1
#define BXVD_P_INIT_REG_PTRS BXVD_P_InitRegPtrs_7405
BXVD_P_INIT_REG_PTRS_PROTOTYPE(7405);

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

#if BXVD_P_POWER_MANAGEMENT

#ifdef BCHP_PWR_RESOURCE_AVD0

/* Use BCHP_PWR PM2 method */
#define BXVD_P_USE_SET_POWER_STATE_REVE0_PM2 1
#define BXVD_P_SET_POWER_STATE BXVD_P_SetPowerState_RevE0_PM2

void BXVD_P_SetPowerState_RevE0_PM2(BXVD_Handle hXvd,
                                    BXVD_P_PowerState PowerStateNew);

#else /* Use direct CLK register access PM method, PM 1 */

#define BXVD_P_USE_SET_POWER_STATE_REVE0 1
#define BXVD_P_SET_POWER_STATE BXVD_P_SetPowerState_RevE0

void BXVD_P_SetPowerState_RevE0(BXVD_Handle hXvd,
                               BXVD_P_PowerState PowerStateNew);
#endif 
#endif

#endif /* _BXVD_PLATFORM_7405_H_ */
