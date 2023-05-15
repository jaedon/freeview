/***************************************************************************
 *     Copyright (c) 2009-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_revn0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/8/12 4:58p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_platform_revn0.h $
 * 
 * Hydra_Software_Devel/1   10/8/12 4:58p davidp
 * SW7445-16: Add support for 7445 platform.
 * 
 *
 ***************************************************************************/

#ifndef _BXVD_PLATFORM_REVN0_H_
#define _BXVD_PLATFORM_REVN0_H_

#if (BCHP_CHIP == 7445)
#define BXVD_P_SHVD_AND_HVD_PRESENT 1
#define BXVD_P_HVD_PRESENT 1
#define BXVD_P_SVD_PRESENT 1
#define BXVD_P_SVC_INTERLACED_SUPPORT 1
#define BXVD_P_VP6_SUPPORT 1
#define BXVD_P_ILS_BUFFERS_INTERNAL 1
#endif


#if BXVD_P_SHVD_AND_HVD_PRESENT
#include "bchp_shvd_intr2_0.h"
#include "bchp_shvd_intr2_1.h"
#include "bchp_int_id_shvd_intr2_0.h"
#include "bchp_int_id_shvd_intr2_1.h"
#include "bchp_int_id_shvd_intr2_2.h"
#include "bchp_reg_cabac2bins2_0.h"
#include "bchp_reg_cabac2bins2_1.h"
#include "bchp_hevd_ol_ctl_0.h"
#include "bchp_hevd_ol_ctl_1.h"
#include "bchp_hevd_ol_ctl_2.h"

#include "bchp_decode_sint_0.h"
#include "bchp_decode_sint_1.h"
#include "bchp_hevd_ol_sint_0.h"
#include "bchp_hevd_ol_sint_1.h"
#include "bchp_hevd_ol_sint_2.h"
/* #include "bchp_decode_sint_oloop_1.h"*/
#include "bchp_decode_main_0.h"
#include "bchp_decode_main_1.h"
#include "bchp_hevd_pcache_0.h"
#include "bchp_hevd_pcache_1.h"
#include "bchp_hevd_pcache_2.h"
#include "bchp_decode_rvc_1.h"
#include "bchp_hevd_ol_cpu_regs_0.h"
#include "bchp_hevd_il_cpu_regs_0.h"
/*#include "bchp_hevd_ol_cpu_debug_0.h" Wrong... 
  #include "bchp_hevd_il_cpu_debug_0.h"

  #include "bchp_decode_ind_sdram_regs_1.h" Old...
  #include "bchp_decode_ind_sdram_regs2_1.h"
*/

#include "bchp_hevd_ol_cpu_regs_0.h"
#include "bchp_hevd_il_cpu_regs_0.h"
/*#include "bchp_decode_cpuregs2_1.h"
  #include "bchp_decode_cpuregs_1.h"
*/

#include "bchp_hevd_ol_cpu_debug_0.h"
#include "bchp_hevd_il_cpu_debug_0.h"
/*#include "bchp_decode_cpuaux2_1.h"
  #include "bchp_decode_cpuaux_1.h"
*/

#include "bchp_hevd_pfri_0.h"
#include "bchp_hevd_pfri_1.h"
#include "bchp_hevd_pfri_2.h"
/*#include "bchp_decode_sd_1.h" */

#include "bchp_dcd_pipe_ctl_0.h"
#include "bchp_decode_rvc_0.h"


#include "bchp_bld_bl_cpu_regs_0.h"
/*#include "bchp_bld_decode_cpuregs_0.h" */
/*#include "bchp_bld_decode_ind_sdram_regs_0.h" */

#include "bchp_bld_bl_cpu_debug_0.h"
/*#include "bchp_bld_decode_cpuaux_0.h" */

#include "bchp_bld_decode_ip_shim_0.h"
#include "bchp_bld_decode_sint_0.h"
#include "bchp_bld_decode_main_0.h"
#include "bchp_ils_regs_0.h"

#endif

#if 0
#include "bchp_bld_decode_sd_0.h"
#include "bchp_bld_decode_ip_shim_0.h"
#include "bchp_bld_decode_main_0.h"
#include "bchp_ils_regs_0.h"
#include "bchp_bld_decode_sint_0.h"
#endif

/* Common to all flavors */
#include "bchp_sun_top_ctrl.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_xpt_pcroffset.h"
#include "bchp_xpt_rave.h"


#if 0
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
#endif

#include "bafl.h"

#define BXVD_P_CHIP_PRODUCT_REVISION  BCHP_SUN_TOP_CTRL_PRODUCT_ID

#define  BXVD_P_AVD_ARC600 1
#define  BXVD_P_FW_HIM_API 1

#include "bxvd_vdec_info.h"

#include "bxvd_priv.h" 


#define BXVD_P_GET_MEMORY_CONFIG(hXvd, eMemCfgMode)   \
{                                                     \
   eMemCfgMode = BXVD_P_MemCfgMode_eUNKNOWN;          \
}

#if (BCHP_CHIP == 7445)
#define BXVD_MAX_INSTANCE_COUNT     2
#else
#define BXVD_MAX_INSTANCE_COUNT     1
#endif

/* UART clock frequency */
#define BXVD_P_AVD_CORE_UART_FREQ (426*1000000) 

/* Rev K core use 1.50 MB */

#define BXVD_P_FW_IMAGE_SIZE              0x00180000
#define BXVD_P_FW_INNER_IMAGE_OFFSET      0x00090000
#define BXVD_P_FW_BASELAYER_IMAGE_OFFSET  0x0012d000

#if (BCHP_CHIP == 7445)
/* SVD/AVD Rev L */
#define BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS  BXVD_P_REVN_DECODE_PROTOCOLS_MASK
#define BXVD_P_CORE_REVISION 'N'

#define BXVD_P_OTP_CTL_REG  BCHP_DECODE_IP_SHIM_0_OTP_CTL_REG
#define BXVD_P_OTP_CTL_REG_disable_RV9_MASK BCHP_DECODE_IP_SHIM_0_OTP_CTL_REG_disable_RV9_MASK

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
#define BXVD_P_USE_FW_LOAD_CORE_REVN0 1
#define BXVD_P_FW_LOAD BXVD_P_FWLoad_RevN0
BXVD_P_FW_LOAD_PROTOTYPE(RevN0);

/* Chip enable */
#define BXVD_P_USE_CORE_CHIP_ENABLE_REVN0 1
#define BXVD_P_CHIP_ENABLE BXVD_P_ChipEnable_RevN0
BXVD_P_CHIP_ENABLE_PROTOTYPE(RevN0);

/* Chip Reset */
#define BXVD_P_USE_CORE_RESET_CHIP_REVN0 1
#define BXVD_P_RESET_CHIP BXVD_P_ChipReset_RevN0
BXVD_P_CHIP_RESET_PROTOTYPE(RevN0);

/* Init Register Pointers/Masks */
#define BXVD_P_USE_INIT_REG_PTRS_REVN0 1
#define BXVD_P_INIT_REG_PTRS BXVD_P_InitRegPtrs_RevN0
BXVD_P_INIT_REG_PTRS_PROTOTYPE(RevN0);

/* Setup FW Memory */
#define BXVD_P_USE_SETUP_FW_MEMORY_REVE0 1
#define BXVD_P_SETUP_FW_MEMORY BXVD_P_SetupFWMemory_RevE0
BXVD_P_SETUP_FW_MEMORY_PROTOTYPE(RevE0);

/* Teardown FW Memory */
#define BXVD_P_USE_TEAR_DOWN_FW_MEMORY_REVE0 1
#define BXVD_P_TEAR_DOWN_FW_MEMORY BXVD_P_TearDownFWMemory_RevE0
BXVD_P_TEAR_DOWN_FW_MEMORY_PROTOTYPE(RevE0);

/* Verify Watchdog Fired */
#define BXVD_P_USE_VERIFY_WATCHDOG_FIRED_REVN0 1
#define BXVD_P_VERIFY_WATCHDOG_FIRED BXVD_P_VerifyWatchdogFired_RevN0
BXVD_P_VERIFY_WATCHDOG_FIRED_PROTOTYPE(RevN0);

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

#define BXVD_P_USE_SET_POWER_STATE_REVN0 1
#define BXVD_P_SET_POWER_STATE BXVD_P_SetPowerState_RevN0

void BXVD_P_SetPowerState_RevN0(BXVD_Handle hXvd, 
                                BXVD_P_PowerState PowerState);
#endif
#endif /* BXVD_P_POWER_MANAGEMENT */

#endif /* _BXVD_PLATFORM_REVN0_H_ */


