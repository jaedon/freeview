/***************************************************************************
 *     Copyright (c) 2009-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_revk0.h $
 * $brcm_Revision: Hydra_Software_Devel/39 $
 * $brcm_Date: 8/15/12 5:17p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_platform_revk0.h $
 * 
 * Hydra_Software_Devel/39   8/15/12 5:17p davidp
 * SW7429-246: Set the UART clock speed for 7429 to 426 MHz.
 * 
 * Hydra_Software_Devel/38   7/27/12 5:48p davidp
 * SW7360-4: Define platform as rev L core.
 * 
 * Hydra_Software_Devel/37   7/9/12 3:43p davidp
 * SW7429-203: Disable BXVD_P_SVD_GISB_ERR_WORKAROUND on 7429. Rev L does
 * not have the hardware issue.
 * 
 * Hydra_Software_Devel/36   6/25/12 4:24p davidp
 * SW7563-3: Add support for 7563 platform.
 * 
 * Hydra_Software_Devel/35   4/24/12 4:10p davidp
 * SW7584-20: Add support for 7584 platform.
 * 
 * Hydra_Software_Devel/34   4/5/12 5:22p davidp
 * SW7344-306: Printout AVD HW revision in BXVD_Open.
 * 
 * Hydra_Software_Devel/33   4/4/12 5:37p davidp
 * SW7435-8: Rev M decoder does not use BL picture or MV buffers.
 * 
 * Hydra_Software_Devel/32   3/13/12 3:12p davidp
 * SW7435-8: Set 7435 core freq.
 * 
 * Hydra_Software_Devel/31   2/29/12 12:44a davidp
 * SW7435-8: Define MAX_INSTANCE_COUNT to 2 for 7435.
 * 
 * Hydra_Software_Devel/30   2/3/12 3:17p davidp
 * SW7425-2024: Define BXVD_P OTP register contants to build on platforms
 * without the decoder OTP register.
 * 
 * Hydra_Software_Devel/29   2/2/12 3:06p davidp
 * SW7425-2024: Define platform specific supported decode protocols mask.
 * 
 * Hydra_Software_Devel/28   1/6/12 5:25p davidp
 * SW7346-630: Add BXVD_P_VerifyWatchdogFired_RevK0 to read ARC600 PC
 * addresses properly.
 * 
 * Hydra_Software_Devel/27   12/5/11 2:55p davidp
 * SW7435-8: Add support for 7435 platform.
 * 
 * Hydra_Software_Devel/26   11/10/11 2:37p davidp
 * SW7425-1599: Add bSVCProgressiveOnly flag to BXVD_ChannelSettings.
 * 
 * Hydra_Software_Devel/25   10/10/11 5:33p davidp
 * SW7429-17: Add support for 7429 platform.
 * 
 * Hydra_Software_Devel/24   9/22/11 1:13p davidp
 * SW7425-1116: Remove SVD GISB error workaround for rev B0.
 * 
 * Hydra_Software_Devel/23   9/6/11 5:58p davidp
 * SWDTV-7795: Set UART clock speed to 426mhz for 35233.
 * 
 * Hydra_Software_Devel/22   7/28/11 5:54p davidp
 * SW7231-127: Add support for the rev K cores that do not have
 * BCHP_PWR_RESOURCE_AVD0_PWR resource,
 * 
 * Hydra_Software_Devel/21   7/28/11 3:35p davidp
 * SW7552-75: Add BCHP_GetFeature call to determins AVD core clock freq on
 * platforms where freq varies.
 * 
 * Hydra_Software_Devel/20   7/20/11 3:27p davidp
 * SW7231-127: Add BXVD_Standby and BXVD_Resume support.
 * 
 * Hydra_Software_Devel/19   6/27/11 4:18p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/5   6/17/11 2:29p davidp
 * SW7425-542:: Reduce FW image size to 1.5mb, also FW added MVC error
 * handling.
 * 
 * Hydra_Software_Devel/SW7425-542/4   6/16/11 5:09p davidp
 * SW7425-615: Merge mainline GISB error workaround code to branch.
 * 
 * Hydra_Software_Devel/SW7425-542/3   6/7/11 5:44p davidp
 * SW7425-542: Add macros and support routines to handle old and new HIM
 * FW API.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/1/11 6:11p davidp
 * SW7425-542: Add Host Interface Memory Read and Write macros.
 * 
 * Hydra_Software_Devel/SW7425-542/1   5/31/11 3:59p davidp
 * SW7425-542: Add support for Host Interface Memory (ARC Scratch memory)
 * FW API changes.
 * 
 * Hydra_Software_Devel/18   6/16/11 4:30p davidp
 * SW7425-615: Add and use BXVD_P_GISB_ERR_WORKAROUND symbolic constant.
 * 
 * Hydra_Software_Devel/17   4/4/11 4:39p davidp
 * SW7425-284: Define XPT Rave Context info symbolic contants.
 * 
 * Hydra_Software_Devel/16   3/30/11 3:45p davidp
 * SWBLURAY-25322: Change stripe width and stripe multiple constants.
 * 
 * Hydra_Software_Devel/15   2/1/11 11:31a davidp
 * SW7344-5: 7344/46 ARC clock is running at 426 mhz
 * 
 * Hydra_Software_Devel/14   1/31/11 3:50p davidp
 * SW7422-22: Include BLD stripe reg definitions.
 * 
 * Hydra_Software_Devel/13   1/24/11 5:34p davidp
 * SWBLURAY-23545: Add support for 7640 platform.
 * 
 * Hydra_Software_Devel/12   12/15/10 5:25p davidp
 * SW7422-149: Use constant definitions for FW IL and BL load offsets.
 * 
 * Hydra_Software_Devel/11   11/22/10 11:02p davidp
 * SW7344-5: Fix BINT_IDs for SVD only platforms.
 * 
 * Hydra_Software_Devel/10   11/19/10 6:45p davidp
 * SW7344-5: Add support for 7344 and 7346 platforms.
 * 
 * Hydra_Software_Devel/9   11/9/10 7:17p davidp
 * SW7422-22: Change core clk freq to 400 MHz.
 * 
 * Hydra_Software_Devel/8   11/8/10 4:26p davidp
 * SW7422-22: Fix Base Layer loading.
 * 
 * Hydra_Software_Devel/7   10/27/10 5:34p davidp
 * SW7422-22: Add VICH IL and BL interrupt support, add external boot
 * callback support.
 * 
 * Hydra_Software_Devel/6   10/21/10 2:58p davidp
 * SW7231-8: Add support for 7231 and 7135 platforms.
 * 
 * Hydra_Software_Devel/5   10/7/10 3:32p davidp
 * SW7422-22: Add new reset procedure with BLD support.
 * 
 * Hydra_Software_Devel/4   9/29/10 2:48p davidp
 * SW7422-22: Add BLD specific registers to register table.
 * 
 * Hydra_Software_Devel/3   9/23/10 3:39p davidp
 * SW7422-22: Add initial support for 7422.
 * 
 * Hydra_Software_Devel/2   9/22/10 5:48p davidp
 * SW7358-11: Fix AVD reset and add Rev K specific chip enable routine.
 * 
 * Hydra_Software_Devel/1   9/10/10 5:46p davidp
 * SW7358-11: Add support for AVD Rev K core.
 * 
 *
 ***************************************************************************/

#ifndef _BXVD_PLATFORM_REVK0_H_
#define _BXVD_PLATFORM_REVK0_H_

#if ((BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435))
#define BXVD_P_SVD_AND_AVD_PRESENT 1
#define BXVD_P_SVD_PRESENT 1

#if ((BCHP_VER == BCHP_VER_A0) | (BCHP_VER == BCHP_VER_A1))
/* SW7425-628: work around for the bus error caused by register reads. */
#define BXVD_P_SVD_GISB_ERR_WORKAROUND 1
#endif

#elif ((BCHP_CHIP == 7231) || (BCHP_CHIP == 7135) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7429))
#define BXVD_P_SVD_ONLY_PRESENT 1
#define BXVD_P_SVD_PRESENT 1

#if (((BCHP_VER == BCHP_VER_A0) || (BCHP_VER == BCHP_VER_A1)) && (BCHP_CHIP != 7429))
/* SW7425-628: work around for the bus error caused by register reads. */
#define BXVD_P_SVD_GISB_ERR_WORKAROUND 1
#endif

#elif (BCHP_CHIP == 7640)
#define BXVD_P_DVD_CHIP 1
#define BXVD_P_AVD_INIT_STRIPE_WIDTH 1
#define BXVD_P_AVD_INIT_STRIPE_MULTIPLE 0
#endif

/* SVD Rev L and later support Interlaced SVC */
#if ((((BCHP_CHIP == 7231) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7425)) && \
      ((BCHP_VER != BCHP_VER_A0) && (BCHP_VER != BCHP_VER_A1))) || BCHP_CHIP == 7429)
#define BXVD_P_SVC_INTERLACED_SUPPORT 1
#endif

/* SVD Rev M */
#if (BCHP_CHIP == 7435)
#define BXVD_P_SVC_INTERLACED_SUPPORT 1
#define BXVD_P_VP6_SUPPORT 1
#define BXVD_P_ILS_BUFFERS_INTERNAL 1
#endif

/* AVD Rev M */
#if (BCHP_CHIP == 7584)
#define BXVD_P_VP6_SUPPORT 1
#endif

/* Some platforms need to determine AVD core clock speed at runtime. */
#if (BCHP_CHIP == 7552)
#define BXVD_P_DYNAMIC_AVD_CORE_FREQ 1
#endif

#if BXVD_P_SVD_AND_AVD_PRESENT
#include "bchp_svd_intr2_0.h"
#include "bchp_int_id_svd_intr2_0.h"
#include "bchp_avd_intr2_1.h"
#include "bchp_int_id_avd_intr2_1.h"
#include "bchp_reg_cabac2bins2_1.h"
#include "bchp_decode_ip_shim_1.h"
#include "bchp_decode_sint_1.h"
#include "bchp_decode_sint_oloop_1.h"
#include "bchp_decode_main_1.h"
#include "bchp_avd_cache_1.h"
#include "bchp_decode_rvc_1.h"
#include "bchp_decode_ind_sdram_regs_1.h"
#include "bchp_decode_ind_sdram_regs2_1.h"
#include "bchp_decode_cpuregs2_1.h"
#include "bchp_decode_cpuregs_1.h"
#include "bchp_decode_cpuaux2_1.h"
#include "bchp_decode_cpuaux_1.h"
#include "bchp_decode_sd_1.h"
#include "bchp_bld_decode_cpuregs_0.h"
#include "bchp_bld_decode_ind_sdram_regs_0.h"
#include "bchp_bld_decode_cpuaux_0.h"
#include "bchp_bld_decode_sd_0.h"
#include "bchp_bld_decode_ip_shim_0.h"
#include "bchp_bld_decode_main_0.h"
#include "bchp_ils_regs_0.h"
#include "bchp_bld_decode_sint_0.h"

#elif BXVD_P_SVD_ONLY_PRESENT 
#include "bchp_svd_intr2_0.h"
#include "bchp_int_id_svd_intr2_0.h"
#include "bchp_bld_decode_cpuregs_0.h"
#include "bchp_bld_decode_ind_sdram_regs_0.h"
#include "bchp_bld_decode_cpuaux_0.h"
#include "bchp_bld_decode_sd_0.h"
#include "bchp_bld_decode_ip_shim_0.h"
#include "bchp_bld_decode_main_0.h"
#include "bchp_ils_regs_0.h"
#include "bchp_bld_decode_sint_0.h"

#else
#include "bchp_avd_intr2_0.h"
#include "bchp_int_id_avd_intr2_0.h"
#endif

#include "bchp_reg_cabac2bins2_0.h"
#include "bchp_decode_rvc_0.h"
#include "bchp_avd_cache_0.h"

#include "bchp_bvnf_intr2_3.h"
#include "bchp_decode_cpuregs2_0.h"
#include "bchp_decode_cpuregs_0.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_xpt_pcroffset.h"
#include "bchp_xpt_rave.h"

#include "bchp_decode_ind_sdram_regs_0.h"
#include "bchp_decode_ind_sdram_regs2_0.h"
#include "bchp_decode_cpuaux2_0.h"
#include "bchp_decode_cpuaux_0.h"
#include "bchp_decode_main_0.h"
#include "bchp_decode_sint_0.h"
#include "bchp_decode_sint_oloop_0.h"
#include "bchp_decode_ip_shim_0.h"

#include "bafl.h"

#define BXVD_P_CHIP_PRODUCT_REVISION  BCHP_SUN_TOP_CTRL_PRODUCT_ID

#define  BXVD_P_AVD_ARC600 1
#define  BXVD_P_FW_HIM_API 1

#include "bxvd_vdec_info.h"

#include "bxvd_priv.h" 

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

#if ((BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435))
#define BXVD_MAX_INSTANCE_COUNT     2
#else
#define BXVD_MAX_INSTANCE_COUNT     1
#endif

#if (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 35233) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7429) || (BCHP_CHIP == 7435)
/* UART clock frequency */
#define BXVD_P_AVD_CORE_UART_FREQ (426*1000000) 

#else /* All other chips */
/* UART clock frequency */
#define BXVD_P_AVD_CORE_UART_FREQ (400*1000000) 
#endif

/* Rev K core use 1.50 MB */

#define BXVD_P_FW_IMAGE_SIZE              0x00180000
#define BXVD_P_FW_INNER_IMAGE_OFFSET      0x00090000
#define BXVD_P_FW_BASELAYER_IMAGE_OFFSET  0x0012d000

/* Define supported decode protocols mask and OTP control register constants. */ 
#if (((BCHP_CHIP == 7231) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7640)) && \
     ((BCHP_VER != BCHP_VER_A0) && (BCHP_VER != BCHP_VER_A1)) || BCHP_CHIP == 7360 || BCHP_CHIP == 7429)
/* SVD/AVD Rev L */
#define BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS  BXVD_P_REVL_DECODE_PROTOCOLS_MASK
#define BXVD_P_CORE_REVISION 'L'

#define BXVD_P_OTP_CTL_REG  BCHP_DECODE_IP_SHIM_0_OTP_CTL_REG
#define BXVD_P_OTP_CTL_REG_disable_RV9_MASK BCHP_DECODE_IP_SHIM_0_OTP_CTL_REG_disable_RV9_MASK

#elif ((BCHP_CHIP == 7435) || (BCHP_CHIP == 7563) || (BCHP_CHIP == 7584))
/* SVD/AVD Rev M */
#define BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS  BXVD_P_REVM_DECODE_PROTOCOLS_MASK
#define BXVD_P_CORE_REVISION 'M'

#define BXVD_P_OTP_CTL_REG  BCHP_DECODE_IP_SHIM_0_OTP_CTL_REG
#define BXVD_P_OTP_CTL_REG_disable_RV9_MASK BCHP_DECODE_IP_SHIM_0_OTP_CTL_REG_disable_RV9_MASK

#else
/* SVD/AVD Rev K */
#define BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS  BXVD_P_REVK_DECODE_PROTOCOLS_MASK
#define BXVD_P_CORE_REVISION 'K'

#define BXVD_P_OTP_CTL_REG  0
#define BXVD_P_OTP_CTL_REG_disable_RV9_MASK 0
#endif 

#if (BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS & (BXVD_P_CREATE_PROTOCOLS_MASK(BAVC_VideoCompressionStd_eRV9)))
/* HW is RV9 capable, but it could still be disabled by OTP */
#define BXVD_P_RV9_CAPABLE 1

#else
/* HW can not decode RV9 protocol streams */
#define BXVD_P_RV9_CAPABLE 0
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
#define BXVD_P_USE_FW_LOAD_CORE_REVK0 1
#define BXVD_P_FW_LOAD BXVD_P_FWLoad_RevK0
BXVD_P_FW_LOAD_PROTOTYPE(RevK0);

/* Chip enable */
#define BXVD_P_USE_CORE_CHIP_ENABLE_REVK0 1
#define BXVD_P_CHIP_ENABLE BXVD_P_ChipEnable_RevK0
BXVD_P_CHIP_ENABLE_PROTOTYPE(RevK0);

/* Chip Reset */
#define BXVD_P_USE_CORE_RESET_CHIP_REVK0 1
#define BXVD_P_RESET_CHIP BXVD_P_ChipReset_RevK0
BXVD_P_CHIP_RESET_PROTOTYPE(RevK0);

/* Init Register Pointers/Masks */
#define BXVD_P_USE_INIT_REG_PTRS_REVK0 1
#define BXVD_P_INIT_REG_PTRS BXVD_P_InitRegPtrs_RevK0
BXVD_P_INIT_REG_PTRS_PROTOTYPE(RevK0);

/* Setup FW Memory */
#define BXVD_P_USE_SETUP_FW_MEMORY_REVE0 1
#define BXVD_P_SETUP_FW_MEMORY BXVD_P_SetupFWMemory_RevE0
BXVD_P_SETUP_FW_MEMORY_PROTOTYPE(RevE0);

/* Teardown FW Memory */
#define BXVD_P_USE_TEAR_DOWN_FW_MEMORY_REVE0 1
#define BXVD_P_TEAR_DOWN_FW_MEMORY BXVD_P_TearDownFWMemory_RevE0
BXVD_P_TEAR_DOWN_FW_MEMORY_PROTOTYPE(RevE0);

/* Verify Watchdog Fired */
#define BXVD_P_USE_VERIFY_WATCHDOG_FIRED_REVK0 1
#define BXVD_P_VERIFY_WATCHDOG_FIRED BXVD_P_VerifyWatchdogFired_RevK0
BXVD_P_VERIFY_WATCHDOG_FIRED_PROTOTYPE(RevK0);

/* Convert userdata offset to address */
#define BXVD_P_USE_CONVERT_UD_OFF2ADDR_REVE0 1
#define BXVD_P_CONVERT_UD_OFF2ADDR BXVD_P_ConvertUDOff2Addr_RevE0
BXVD_P_CONVERT_UD_OFF2ADDR_PROTOTYPE(RevE0);

/* Rave Context Register info needed by FW */
#define BXVD_P_RAVE_CONTEXT_SIZE        (BCHP_XPT_RAVE_CX1_AV_CDB_WRITE_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR)
#define BXVD_P_RAVE_CX_HOLD_CLR_STATUS  BCHP_XPT_RAVE_CX_HOLD_CLR_STATUS
#define BXVD_P_RAVE_PACKET_COUNT        BCHP_XPT_RAVE_PACKET_COUNT 

/* Use Host Interface Memory routines */
#define BXVD_P_USE_READ_DISPLAY_INFO_HIM_API 1

#define BXVD_P_IS_DISPLAY_INFO_EQUAL BXVD_P_IsDisplayInfoEqual_HIM_API

void BXVD_P_ReadDisplayInfo_HIM_API(BXVD_Handle hXvd, 
                                    uint32_t uiDisplayInfoOffset,
                                    BXVD_P_DisplayInfo *pstDisplayInfo);

bool BXVD_P_IsDisplayInfoEqual_HIM_API(BXVD_P_DisplayInfo stDisplayInfo, 
                                       BXVD_P_DisplayInfo stDisplayInfo1);

#if BXVD_P_POWER_MANAGEMENT

/* For systems that do not have BCHP PWR AVD0_PWR resource */
#if (BCHP_CHIP == 35233)

#define BXVD_P_USE_SET_POWER_STATE_REVE0_PM2 1
#define BXVD_P_SET_POWER_STATE BXVD_P_SetPowerState_RevE0_PM2

void BXVD_P_SetPowerState_RevE0_PM2(BXVD_Handle hXvd, 
                                    BXVD_P_PowerState PowerState);
#else

#define BXVD_P_USE_SET_POWER_STATE_REVK0 1
#define BXVD_P_SET_POWER_STATE BXVD_P_SetPowerState_RevK0

void BXVD_P_SetPowerState_RevK0(BXVD_Handle hXvd, 
                                BXVD_P_PowerState PowerState);
#endif
#endif /* BXVD_P_POWER_MANAGEMENT */

#endif /* _BXVD_PLATFORM_REVK0_H_ */


