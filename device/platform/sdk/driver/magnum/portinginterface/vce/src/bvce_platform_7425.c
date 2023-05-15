/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_platform_7425.c $
 * $brcm_Revision: Hydra_Software_Devel/38 $
 * $brcm_Date: 10/8/12 4:26p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_platform_7425.c $
 * 
 * Hydra_Software_Devel/38   10/8/12 4:26p nilesh
 * SW7425-3959: Removed code to set VIP_BVB and VIP_ERR registers.  VCE FW
 * will set the registers, instead.
 * 
 * Hydra_Software_Devel/37   9/5/12 11:33a nilesh
 * SW7425-3816: Added reference to JIRA in comment
 * 
 * Hydra_Software_Devel/36   9/5/12 11:24a nilesh
 * SW7425-3816: Fixed compiler warning
 * 
 * Hydra_Software_Devel/35   9/5/12 11:13a nilesh
 * SW7425-3816: Added MAU PFRI sw init to MAU reset logic
 * 
 * Hydra_Software_Devel/34   8/29/12 2:00p nilesh
 * SW7425-3360: Support new BVN2VICE mailbox definition
 * 
 * Hydra_Software_Devel/SW7425-3360/3   8/20/12 5:10p nilesh
 * SW7425-3360: Fixed compilation error
 * 
 * Hydra_Software_Devel/SW7425-3360/2   8/20/12 4:43p nilesh
 * SW7425-3360: Updated BVN2VICE MBOX definition
 * 
 * Hydra_Software_Devel/SW7425-3360/1   7/5/12 5:51p nilesh
 * SW7425-3360: Initial VCE FW FNRT support. Various bug fixes.
 * 
 * Hydra_Software_Devel/33   5/1/12 1:01p nilesh
 * SW7425-1221: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1221/1   4/30/12 1:26p nilesh
 * SW7425-2643: Updated BVN2VCE MBOX for AFD/BAR support
 * 
 * Hydra_Software_Devel/32   4/13/12 9:22a nilesh
 * SW7425-2765: Clear BVB/ERR status on VIP and VIP1
 * 
 * Hydra_Software_Devel/31   4/12/12 1:35p nilesh
 * SW7425-2765: Enabled BVB/ERR status on VIP and VIP1
 * 
 * Hydra_Software_Devel/30   2/14/12 10:05a nilesh
 * SW7425-2220: Cleaned up state when StopEncode(eImmediate) is used
 * 
 * Hydra_Software_Devel/29   2/1/12 2:15p nilesh
 * SW7425-2239: Updated for new channel change flag in BVN to VCE Mailbox
 * 
 * Hydra_Software_Devel/28   1/18/12 3:43p nilesh
 * SW7425-2084: Added support for new BVN2VICE RDB definition
 * 
 * Hydra_Software_Devel/27   1/10/12 1:25p nilesh
 * SW7435-1: Added support for multiple instances
 * 
 * Hydra_Software_Devel/26   12/5/11 3:27p nilesh
 * SW7425-1847: Reset CPU Speed Register (VICE2_MISC_SCRATCH_0) to zero
 * before boot
 * 
 * Hydra_Software_Devel/25   12/5/11 2:13p nilesh
 * SW7425-960: Updated VICE2 MBOX RDB version
 * 
 * Hydra_Software_Devel/24   9/23/11 9:33a nilesh
 * SW7425-891: Fixed FORCE_SHUTOFF to no longer do a read/modify/write of
 * MAU_FW_CONTROL register which was causing PIC_START and DONE_WR_REG to
 * be set and trigger another MAU execution
 * 
 * Hydra_Software_Devel/23   9/20/11 3:30p nilesh
 * SW7425-891: Check MAU status before triggering FORCE_SHUTOFF
 * 
 * Hydra_Software_Devel/22   9/15/11 4:25p nilesh
 * SW7425-891: Fix a1 compile
 * 
 * Hydra_Software_Devel/21   9/15/11 4:17p nilesh
 * SW7425-891: Implemented clean shutdown of MAU before SW INIT
 * 
 * Hydra_Software_Devel/20   9/14/11 2:55p nilesh
 * SW7425-890: Added support for 2 separate cabac data ready interrupts
 * for 7425 B0
 * 
 * Hydra_Software_Devel/19   9/8/11 1:58p nilesh
 * SW7425-891: Add B0 and Dual Encode Support
 * 
 * Hydra_Software_Devel/SW7425-891/1   9/7/11 3:52p nilesh
 * SW7425-891: Fixed B0 compilation warning
 * 
 * Hydra_Software_Devel/18   8/15/11 9:18p nilesh
 * SW7425-1006: Fixed compilation asssert with latest BCHP
 * 
 * Hydra_Software_Devel/17   8/11/11 11:29a nilesh
 * SW7425-1006: VCE FW API v2.0
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/4   8/10/11 4:49p nilesh
 * SW7425-448: Rebase against mainline
 * 
 * Hydra_Software_Devel/16   8/10/11 4:22p nilesh
 * SW7425-448: Added Dynamic Power Management
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/3   8/5/11 10:43a nilesh
 * SW7425-1006: Merge from mainline
 * 
 * Hydra_Software_Devel/15   8/5/11 10:29a nilesh
 * SW7425-1006: Updated CASSERT for newer bchp_common.h VICE2 defines
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/2   8/2/11 9:32p nilesh
 * SW7425-448: Refactored power management logic.
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/1   7/22/11 11:15p nilesh
 * SW7425-1: Additional CDB debug logs
 * 
 * Hydra_Software_Devel/14   4/5/11 4:37p nilesh
 * SW7425-172,SW7425-173: Added device (watchdog) and channel (event)
 * callback suppport
 * 
 * Hydra_Software_Devel/13   3/31/11 1:21p nilesh
 * SW7425-1: Phase 2.0 Test Release
 * 
 * Hydra_Software_Devel/VCE_Phase_1_RC/1   3/16/11 12:23p nilesh
 * SW7425-171: Added BVN Metadata Change Event
 * 
 * Hydra_Software_Devel/12   3/15/11 5:04p nilesh
 * SW7425-170: Fixed A1 compile error
 * 
 * Hydra_Software_Devel/11   2/8/11 4:48p nilesh
 * SW7425-1: Fixed reset logic bug causing bus error on close
 * 
 * Hydra_Software_Devel/10   1/26/11 5:41p nilesh
 * SW7425-1: Fix MB Arc debug log
 * 
 * Hydra_Software_Devel/9   1/25/11 10:53a nilesh
 * SW7425-1: By default, enable JTAG access to Pic ARC if in non-polling
 * mode
 * 
 * Hydra_Software_Devel/8   12/7/10 10:45a nilesh
 * SW7425-1: Moved VICE2 SUN_TOP init to bchp_7425.c
 * 
 * Hydra_Software_Devel/7   12/6/10 11:52a nilesh
 * SW7425-1: Moved mailbox register config to platform config
 * 
 * Hydra_Software_Devel/6   12/1/10 3:42p nilesh
 * SW7425-1: Updated delay mode API.  Added logic that verifies BVN
 * metadata address at compile-time and run-time.
 * 
 * Hydra_Software_Devel/5   11/18/10 12:18p nilesh
 * SW7425-1: Removed some ifdefs to more more modular on other platforms
 * 
 * Hydra_Software_Devel/4   11/18/10 12:09p nilesh
 * SW7425-1: Fix watchdog
 * 
 * Hydra_Software_Devel/3   11/17/10 3:56p nilesh
 * SW7425-1: Reordered reset logic to prevent bus error
 * 
 * Hydra_Software_Devel/2   11/17/10 1:23p nilesh
 * SW7425-1: Moved MIPs debug register definitions to platform specific
 * file
 * 
 * Hydra_Software_Devel/1   11/17/10 12:41p nilesh
 * SW7425-1: Moved platform specific code
 *
 ***************************************************************************/

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bdbg.h"           /* debug interface */
#include "bkni.h"           /* kernel interface */

#include "bvce.h"
#include "bvce_priv.h"
#include "bvce_debug.h"

#include "bchp_common.h"
#include "bchp_vice2_misc.h"
#include "bchp_vice2_arcss_ess_ctrl_0.h"
#include "bchp_vice2_arcss_ess_ctrl_1.h"
#include "bchp_vice2_arcss_misc.h"
#include "bchp_vice2_arcss_ess_dccm_0.h"
#include "bchp_vice2_arcss_ess_dccm_1.h"
#include "bchp_vice2_l2.h"
#include "bchp_vice2_cabac_0.h"
#include "bchp_vice2_vip_0.h"
#include "bchp_vice2_vip1_0.h"
#include "bchp_int_id_vice2_l2.h"
#include "bchp_sun_top_ctrl.h"
#include "bavc_vce_mbox.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

/* For Debug */
#include "bchp_vice2_cme_0.h"

BDBG_MODULE(BVCE_PLATFORM_7425);

#define BVCE_P_REGISTER_START (BCHP_PHYSICAL_OFFSET + 0x700000)

static const BVCE_Platform_P_RegisterSetting s_astViceResetRegisterSettings[] =
{
 { "Arc[0] Boot",               BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_PROC_START,                                    0x00000000,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_PROC_START_ENABLE_MASK },
 { "Vice SUNTOP Reset (Set)",   BCHP_SUN_TOP_CTRL_SW_INIT_1_SET,                                                0xFFFFFFFF,             BCHP_SUN_TOP_CTRL_SW_INIT_1_SET_vice2_sw_init_MASK },
 { "Vice SUNTOP Reset (Clear)", BCHP_SUN_TOP_CTRL_SW_INIT_1_CLEAR,                                              0xFFFFFFFF,             BCHP_SUN_TOP_CTRL_SW_INIT_1_SET_vice2_sw_init_MASK },
 { "Vice CPU Speed (MISC_SCRATCH_0)", BCHP_VICE2_MISC_SCRATCH_0,                                                0x00000000,             0xFFFFFFF },
 { "Reg Addr Offset",           BCHP_VICE2_ARCSS_MISC_INIT_SYS_REG_ADDR_OFFSET,                                 BVCE_P_REGISTER_START,  BCHP_VICE2_ARCSS_MISC_INIT_SYS_REG_ADDR_OFFSET_OFFSET_MASK },
 { "Arc Config",                BCHP_VICE2_ARCSS_MISC_INIT_SYS_ARC_CONFIG,                                      0x00000000,             BCHP_VICE2_ARCSS_MISC_INIT_SYS_ARC_CONFIG_SEL_MASK },
#if BVCE_P_POLL_FW_MBX
 { "Arc[0] Host I/F",           BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_HOST_IF,                                   0xFFFFFFFF,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_HOST_IF_SEL_MASK },
#else
 { "Arc[0] Host I/F",           BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_HOST_IF,                                   0x00000000,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_HOST_IF_SEL_MASK },
#endif
 { "Cache Miss",                BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_BVCI2SCB_BRIDGE_CTRL,                      0x00000000,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_BVCI2SCB_BRIDGE_CTRL_CACHE_MISS_EN_MASK },
 { "Read Swap",                 BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_BVCI2SCB_BRIDGE_CTRL,                      0x00000000,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_BVCI2SCB_BRIDGE_CTRL_DATA_RD_SWAP_MASK },
 { "Write Swap",                BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_BVCI2SCB_BRIDGE_CTRL,                      0x00000000,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_BVCI2SCB_BRIDGE_CTRL_DATA_WR_SWAP_MASK },
 { "Host2Vice",                 BCHP_VICE2_ARCSS_ESS_DCCM_0_DATAi_ARRAY_BASE + HOST2VICE_MBOX_OFFSET,           VICE2_BOOT_STATUS_INIT, BCHP_VICE2_ARCSS_ESS_DCCM_0_DATAi_DATA_MASK },
 { "Vice2Host",                 BCHP_VICE2_ARCSS_ESS_DCCM_0_DATAi_ARRAY_BASE + VICE2HOST_MBOX_OFFSET,           0x00000000,             BCHP_VICE2_ARCSS_ESS_DCCM_0_DATAi_DATA_MASK },
 { "L2 Interrupt Mask Set",     BCHP_VICE2_L2_CPU_MASK_SET,                                                     0xFFFFFFFF,             0xFFFFFFFF },
 { "L2 Interrupt Clear",        BCHP_VICE2_L2_CPU_CLEAR,                                                        0xFFFFFFFF,             0xFFFFFFFF },
 { "Arc[0] Boot",               BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_PROC_START,                                    0x00000000,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_PROC_START_ENABLE_MASK },
};

#ifdef BCHP_VICE2_L2_CPU_CLEAR_VICE2_05_INTR_MASK
static const BVCE_Platform_P_RegisterSetting s_astViceInterruptEnableRegisterSettings[] =
{
#if !BVCE_P_POLL_FW_MBX
 { "Vice2Host Interrupt Clear",      BCHP_VICE2_L2_CPU_CLEAR,                                                        0xFFFFFFFF,             BCHP_VICE2_L2_CPU_CLEAR_VICE2_28_INTR_MASK },
 { "Vice2Host Interrupt Mask",       BCHP_VICE2_L2_CPU_MASK_CLEAR,                                                   0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_28_INTR_MASK },
#endif
#if BVCE_P_ENABLE_WATCHDOG
 { "Arc[0] Watchdog Interrupt Clear", BCHP_VICE2_L2_CPU_CLEAR,                                                        0xFFFFFFFF,             BCHP_VICE2_L2_CPU_CLEAR_VICE2_00_INTR_MASK },
 { "Arc[0] Watchdog Interrupt Mask",  BCHP_VICE2_L2_CPU_MASK_CLEAR,                                                   0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_00_INTR_MASK },
#endif
 { "Error Interrupt Clear",          BCHP_VICE2_L2_CPU_CLEAR,                                                        0xFFFFFFFF,             BCHP_VICE2_L2_CPU_CLEAR_VICE2_27_INTR_MASK },
 { "Error Interrupt Mask",           BCHP_VICE2_L2_CPU_MASK_CLEAR,                                                   0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_27_INTR_MASK },
#if !BVCE_P_POLL_FW_DATAREADY
 { "Data Ready Interrupt Clear",               BCHP_VICE2_L2_CPU_CLEAR,                                                        0xFFFFFFFF,             BCHP_VICE2_L2_CPU_CLEAR_VICE2_02_INTR_MASK },
 { "Data Ready Interrupt Mask",                BCHP_VICE2_L2_CPU_MASK_CLEAR,                                                   0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_02_INTR_MASK },
#endif
};

static const BVCE_Platform_P_RegisterSetting s_astViceInterruptDisableRegisterSettings[] =
{
 { "Vice2Host Interrupt Mask",       BCHP_VICE2_L2_CPU_MASK_SET,                                                     0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_28_INTR_MASK },
 { "Arc[0] Watchdog Interrupt Mask", BCHP_VICE2_L2_CPU_MASK_SET,                                                     0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_00_INTR_MASK },
 { "Error Interrupt Mask",           BCHP_VICE2_L2_CPU_MASK_SET,                                                     0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_27_INTR_MASK },
 { "Data Ready Mask",                BCHP_VICE2_L2_CPU_MASK_SET,                                                     0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_02_INTR_MASK },
};
#else
static const BVCE_Platform_P_RegisterSetting s_astViceInterruptEnableRegisterSettings[] =
{
#if !BVCE_P_POLL_FW_MBX
 { "Vice2Host Interrupt Clear",      BCHP_VICE2_L2_CPU_CLEAR,                                                        0xFFFFFFFF,             BCHP_VICE2_L2_CPU_CLEAR_VICE2_MBOX_INTR_MASK },
 { "Vice2Host Interrupt Mask",       BCHP_VICE2_L2_CPU_MASK_CLEAR,                                                   0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_MBOX_INTR_MASK },
#endif
#if BVCE_P_ENABLE_WATCHDOG
 { "Arc[0] Watchdog Interrupt Clear", BCHP_VICE2_L2_CPU_CLEAR,                                                        0xFFFFFFFF,             BCHP_VICE2_L2_CPU_CLEAR_VICE2_WDOG_0_INTR_MASK },
 { "Arc[0] Watchdog Interrupt Mask",  BCHP_VICE2_L2_CPU_MASK_CLEAR,                                                   0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_WDOG_0_INTR_MASK },
#endif
 { "Error Interrupt Clear",          BCHP_VICE2_L2_CPU_CLEAR,                                                        0xFFFFFFFF,             BCHP_VICE2_L2_CPU_CLEAR_VICE2_ERROR_INTR_MASK },
 { "Error Interrupt Mask",           BCHP_VICE2_L2_CPU_MASK_CLEAR,                                                   0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_ERROR_INTR_MASK },
#if !BVCE_P_POLL_FW_DATAREADY
 { "Data Ready Interrupt Clear",               BCHP_VICE2_L2_CPU_CLEAR,                                                        0xFFFFFFFF,             BCHP_VICE2_L2_CPU_CLEAR_VICE2_CABAC_RDY_0_INTR_MASK },
 { "Data Ready Interrupt Mask",                BCHP_VICE2_L2_CPU_MASK_CLEAR,                                                   0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_CABAC_RDY_0_INTR_MASK },
#endif
};

static const BVCE_Platform_P_RegisterSetting s_astViceInterruptDisableRegisterSettings[] =
{
 { "Vice2Host Interrupt Mask",       BCHP_VICE2_L2_CPU_MASK_SET,                                                     0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_MBOX_INTR_MASK },
 { "Arc[0] Watchdog Interrupt Mask", BCHP_VICE2_L2_CPU_MASK_SET,                                                     0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_WDOG_0_INTR_MASK },
 { "Error Interrupt Mask",           BCHP_VICE2_L2_CPU_MASK_SET,                                                     0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_ERROR_INTR_MASK },
 { "Data Ready Mask",                BCHP_VICE2_L2_CPU_MASK_SET,                                                     0xFFFFFFFF,             BCHP_VICE2_L2_CPU_MASK_CLEAR_VICE2_CABAC_RDY_0_INTR_MASK },
};
#endif

static const BVCE_Platform_P_RegisterSetting s_astViceBootRegisterSettings[] =
{
 { "Arc[0] Boot",               BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_PROC_START,                                    0XFFFFFFFF,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_PROC_START_ENABLE_MASK },
};

static const BVCE_Platform_P_RegisterSetting s_astViceWatchdogEnableRegisterSettings[] =
{
 { "Arc[0] Watchdog Counter",   BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_WATCHDOG_CTRL,                             0x00001500,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_WATCHDOG_CTRL_PERIOD_MASK },
 { "Arc[0] Watchdog Enable",    BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_WATCHDOG_CTRL,                             0x00010000,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_WATCHDOG_CTRL_ENABLE_MASK },
 { "Arc[1] Watchdog Counter",   BCHP_VICE2_ARCSS_ESS_CTRL_1_INIT_SYS_WATCHDOG_CTRL,                             0x00001500,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_WATCHDOG_CTRL_PERIOD_MASK },
 { "Arc[1] Watchdog Enable",    BCHP_VICE2_ARCSS_ESS_CTRL_1_INIT_SYS_WATCHDOG_CTRL,                             0x00010000,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_WATCHDOG_CTRL_ENABLE_MASK },
};

static const BVCE_Platform_P_RegisterSetting s_astViceWatchdogDisableRegisterSettings[] =
{
 { "Arc[0] Watchdog Enable",    BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_WATCHDOG_CTRL,                             0x00000000,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_WATCHDOG_CTRL_ENABLE_MASK },
 { "Arc[1] Watchdog Enable",    BCHP_VICE2_ARCSS_ESS_CTRL_1_INIT_SYS_WATCHDOG_CTRL,                             0x00000000,             BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_WATCHDOG_CTRL_ENABLE_MASK },
};

BERR_Code
BVCE_Platform_P_GetConfig(
         unsigned uiInstance,
         BVCE_Platform_P_Config *pstPlatformConfig
         )
{
   BDBG_ASSERT( pstPlatformConfig );
   BDBG_ASSERT( uiInstance < BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES );

   BSTD_UNUSED( uiInstance );

   BKNI_Memset(
            pstPlatformConfig,
            0,
            sizeof( BVCE_Platform_P_Config )
            );

   BDBG_CASSERT( 2 == BAVC_VICE_BVN2VICE_MAJORREVISION_ID );
#ifdef BAVC_VICE_MBOX_OFFSET_BVN2VICE_DATA_0_START
   BDBG_CASSERT( 1 == BAVC_VICE_MBOX_REVISION_ID );
   BDBG_CASSERT( HOST2VICE_MBOX_OFFSET == BAVC_VICE_MBOX_OFFSET_HOST2VICE );
   BDBG_CASSERT( VICE2HOST_MBOX_OFFSET == BAVC_VICE_MBOX_OFFSET_VICE2HOST );
   BDBG_CASSERT( BVN2VICE_MBOX_OFFSET == BAVC_VICE_MBOX_OFFSET_BVN2VICE );
   BDBG_CASSERT( BVN2VICE_MBOX_PAYLOAD_OFFSET == BAVC_VICE_MBOX_OFFSET_BVN2VICE_DATA_0_START );
#else
   BDBG_CASSERT( HOST2VICE_MBOX_OFFSET == BCHP_VICE2_REGSET_MISC_MBOX_INTERFACE_HOST2VICE_OFFSET );
   BDBG_CASSERT( VICE2HOST_MBOX_OFFSET == BCHP_VICE2_REGSET_MISC_MBOX_INTERFACE_VICE2HOST_OFFSET );
   BDBG_CASSERT( BVN2VICE_MBOX_OFFSET == BCHP_VICE2_REGSET_MISC_MBOX_INTERFACE_BVN2VICE_OFFSET );
   BDBG_CASSERT( BVN2VICE_MBOX_PAYLOAD_OFFSET == BCHP_VICE2_REGSET_MISC_MBOX_INTERFACE_DWORD_00_BVB_PIC_SIZE_OFFSET );
#endif

   pstPlatformConfig->stViceReset.astRegisterSettings = s_astViceResetRegisterSettings;
   pstPlatformConfig->stViceReset.uiRegisterCount = sizeof( s_astViceResetRegisterSettings ) / sizeof ( BVCE_Platform_P_RegisterSetting );

   pstPlatformConfig->stViceInterruptEnable.astRegisterSettings = s_astViceInterruptEnableRegisterSettings;
   pstPlatformConfig->stViceInterruptEnable.uiRegisterCount = sizeof( s_astViceInterruptEnableRegisterSettings ) / sizeof ( BVCE_Platform_P_RegisterSetting );

   pstPlatformConfig->stViceInterruptDisable.astRegisterSettings = s_astViceInterruptDisableRegisterSettings;
   pstPlatformConfig->stViceInterruptDisable.uiRegisterCount = sizeof( s_astViceInterruptDisableRegisterSettings ) / sizeof ( BVCE_Platform_P_RegisterSetting );

   pstPlatformConfig->stViceBoot.astRegisterSettings = s_astViceBootRegisterSettings;
   pstPlatformConfig->stViceBoot.uiRegisterCount = sizeof( s_astViceBootRegisterSettings ) / sizeof ( BVCE_Platform_P_RegisterSetting );

   pstPlatformConfig->stViceWatchdogEnable.astRegisterSettings = s_astViceWatchdogEnableRegisterSettings;
   pstPlatformConfig->stViceWatchdogEnable.uiRegisterCount = sizeof( s_astViceWatchdogEnableRegisterSettings ) / sizeof ( BVCE_Platform_P_RegisterSetting );

   pstPlatformConfig->stViceWatchdogDisable.astRegisterSettings = s_astViceWatchdogDisableRegisterSettings;
   pstPlatformConfig->stViceWatchdogDisable.uiRegisterCount = sizeof( s_astViceWatchdogDisableRegisterSettings ) / sizeof ( BVCE_Platform_P_RegisterSetting );

   BDBG_ASSERT( 2 == BVCE_PLATFORM_P_NUM_ARC_CORES );
   pstPlatformConfig->stCore[0].uiInstructionStartPhysicalAddress = BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_ARC_INSTR_ADDR_OFFSET;
   pstPlatformConfig->stCore[0].uiDataSpaceStartRelativeOffset = BCHP_VICE2_ARCSS_ESS_CTRL_0_INIT_SYS_DATA_SPACE_START;
   pstPlatformConfig->stCore[0].uiDCCMBase = BCHP_VICE2_ARCSS_ESS_DCCM_0_DATAi_ARRAY_BASE;
   pstPlatformConfig->stCore[1].uiInstructionStartPhysicalAddress = BCHP_VICE2_ARCSS_ESS_CTRL_1_INIT_SYS_ARC_INSTR_ADDR_OFFSET;
   pstPlatformConfig->stCore[1].uiDataSpaceStartRelativeOffset = BCHP_VICE2_ARCSS_ESS_CTRL_1_INIT_SYS_DATA_SPACE_START;
   pstPlatformConfig->stCore[1].uiDCCMBase = BCHP_VICE2_ARCSS_ESS_DCCM_1_DATAi_ARRAY_BASE;

   BDBG_ASSERT( 2 == BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS );
   pstPlatformConfig->stOutput[0].stCDB.uiReadPointer = BCHP_VICE2_CABAC_0_CDB_CTX0_READ_PTR;
   pstPlatformConfig->stOutput[0].stCDB.uiBasePointer = BCHP_VICE2_CABAC_0_CDB_CTX0_BASE_PTR;
   pstPlatformConfig->stOutput[0].stCDB.uiValidPointer = BCHP_VICE2_CABAC_0_CDB_CTX0_VALID_PTR;
   pstPlatformConfig->stOutput[0].stCDB.uiWritePointer = BCHP_VICE2_CABAC_0_CDB_CTX0_WRITE_PTR;
   pstPlatformConfig->stOutput[0].stCDB.uiEndPointer = BCHP_VICE2_CABAC_0_CDB_CTX0_END_PTR;

   pstPlatformConfig->stOutput[0].stITB.uiReadPointer = BCHP_VICE2_CABAC_0_ITB_CTX0_READ_PTR;
   pstPlatformConfig->stOutput[0].stITB.uiBasePointer = BCHP_VICE2_CABAC_0_ITB_CTX0_BASE_PTR;
   pstPlatformConfig->stOutput[0].stITB.uiValidPointer = BCHP_VICE2_CABAC_0_ITB_CTX0_VALID_PTR;
   pstPlatformConfig->stOutput[0].stITB.uiWritePointer = BCHP_VICE2_CABAC_0_ITB_CTX0_WRITE_PTR;
   pstPlatformConfig->stOutput[0].stITB.uiEndPointer = BCHP_VICE2_CABAC_0_ITB_CTX0_END_PTR;

   pstPlatformConfig->stOutput[1].stCDB.uiReadPointer = BCHP_VICE2_CABAC_0_CDB_CTX1_READ_PTR;
   pstPlatformConfig->stOutput[1].stCDB.uiBasePointer = BCHP_VICE2_CABAC_0_CDB_CTX1_BASE_PTR;
   pstPlatformConfig->stOutput[1].stCDB.uiValidPointer = BCHP_VICE2_CABAC_0_CDB_CTX1_VALID_PTR;
   pstPlatformConfig->stOutput[1].stCDB.uiWritePointer = BCHP_VICE2_CABAC_0_CDB_CTX1_WRITE_PTR;
   pstPlatformConfig->stOutput[1].stCDB.uiEndPointer = BCHP_VICE2_CABAC_0_CDB_CTX1_END_PTR;

   pstPlatformConfig->stOutput[1].stITB.uiReadPointer = BCHP_VICE2_CABAC_0_ITB_CTX1_READ_PTR;
   pstPlatformConfig->stOutput[1].stITB.uiBasePointer = BCHP_VICE2_CABAC_0_ITB_CTX1_BASE_PTR;
   pstPlatformConfig->stOutput[1].stITB.uiValidPointer = BCHP_VICE2_CABAC_0_ITB_CTX1_VALID_PTR;
   pstPlatformConfig->stOutput[1].stITB.uiWritePointer = BCHP_VICE2_CABAC_0_ITB_CTX1_WRITE_PTR;
   pstPlatformConfig->stOutput[1].stITB.uiEndPointer = BCHP_VICE2_CABAC_0_ITB_CTX1_END_PTR;

   BDBG_CASSERT( 28 == HOST_CMD_MAILBOX_INTERRUPT_LEVEL );
   BDBG_CASSERT( 27 == HOST_EVENTS_INTERRUPT_LEVEL );
   pstPlatformConfig->stInterrupt.uiInterruptStatusRegister = BCHP_VICE2_L2_CPU_STATUS;
   pstPlatformConfig->stInterrupt.uiInterruptClearRegister = BCHP_VICE2_L2_CPU_CLEAR;

#ifdef BCHP_VICE2_L2_CPU_CLEAR_VICE2_05_INTR_MASK
   pstPlatformConfig->stInterrupt.stMask.uiWatchdog[0] = BCHP_VICE2_L2_CPU_STATUS_VICE2_00_INTR_MASK;
   pstPlatformConfig->stInterrupt.idWatchdog[0] = BCHP_INT_ID_VICE2_00_INTR;

   pstPlatformConfig->stInterrupt.stMask.uiWatchdog[1] = BCHP_VICE2_L2_CPU_STATUS_VICE2_01_INTR_MASK;
   pstPlatformConfig->stInterrupt.idWatchdog[1] = BCHP_INT_ID_VICE2_01_INTR;

   pstPlatformConfig->stInterrupt.stMask.uiDataReady = BCHP_VICE2_L2_CPU_STATUS_VICE2_02_INTR_MASK;
   pstPlatformConfig->stInterrupt.idDataReady = BCHP_INT_ID_VICE2_02_INTR;

   pstPlatformConfig->stInterrupt.stMask.uiMailbox = BCHP_VICE2_L2_CPU_STATUS_VICE2_28_INTR_MASK;
   pstPlatformConfig->stInterrupt.idMailbox = BCHP_INT_ID_VICE2_28_INTR;

   pstPlatformConfig->stInterrupt.stMask.uiError = BCHP_VICE2_L2_CPU_STATUS_VICE2_27_INTR_MASK;
   pstPlatformConfig->stInterrupt.idEvent = BCHP_INT_ID_VICE2_27_INTR;
#else
   pstPlatformConfig->stInterrupt.stMask.uiWatchdog[0] = BCHP_VICE2_L2_CPU_STATUS_VICE2_WDOG_0_INTR_MASK;
   pstPlatformConfig->stInterrupt.idWatchdog[0] = BCHP_INT_ID_VICE2_WDOG_0_INTR;

   pstPlatformConfig->stInterrupt.stMask.uiWatchdog[1] = BCHP_VICE2_L2_CPU_STATUS_VICE2_WDOG_1_INTR_MASK;
   pstPlatformConfig->stInterrupt.idWatchdog[1] = BCHP_INT_ID_VICE2_WDOG_1_INTR;

   pstPlatformConfig->stInterrupt.stMask.uiDataReady[0] = BCHP_VICE2_L2_CPU_STATUS_VICE2_CABAC_RDY_0_INTR_MASK;
   pstPlatformConfig->stInterrupt.idDataReady[0] = BCHP_INT_ID_VICE2_CABAC_RDY_0_INTR;

#ifdef BCHP_VICE2_L2_CPU_STATUS_VICE2_CABAC_RDY_1_INTR_MASK
   pstPlatformConfig->stInterrupt.stMask.uiDataReady[1] = BCHP_VICE2_L2_CPU_STATUS_VICE2_CABAC_RDY_1_INTR_MASK;
   pstPlatformConfig->stInterrupt.idDataReady[1] = BCHP_INT_ID_VICE2_CABAC_RDY_1_INTR;
#endif

   pstPlatformConfig->stInterrupt.stMask.uiMailbox = BCHP_VICE2_L2_CPU_STATUS_VICE2_MBOX_INTR_MASK;
   pstPlatformConfig->stInterrupt.idMailbox = BCHP_INT_ID_VICE2_MBOX_INTR;

   pstPlatformConfig->stInterrupt.stMask.uiError = BCHP_VICE2_L2_CPU_STATUS_VICE2_ERROR_INTR_MASK;
   pstPlatformConfig->stInterrupt.idEvent = BCHP_INT_ID_VICE2_ERROR_INTR;
#endif

   pstPlatformConfig->stDebug.uiCMEPictureIndex = BCHP_VICE2_CME_0_PICTURE_INDEX;
   pstPlatformConfig->stDebug.uiPicArcPC = 0x712018;
   pstPlatformConfig->stDebug.uiPicArcStatus32 = 0x712028;
   pstPlatformConfig->stDebug.uiCDBDepth[0] = BCHP_VICE2_CABAC_0_CDB_CTX0_DEPTH;
   pstPlatformConfig->stDebug.uiCDBDepth[1] = BCHP_VICE2_CABAC_0_CDB_CTX1_DEPTH;
#if (BCHP_VER >= BCHP_VER_B0)
   pstPlatformConfig->stDebug.uiSTC[0] = BCHP_VICE2_ARCSS_MISC_STC0_LOWER;
   pstPlatformConfig->stDebug.uiSTC[1] = BCHP_VICE2_ARCSS_MISC_STC1_LOWER;
#else
   pstPlatformConfig->stDebug.uiSTC[0] = BCHP_VICE2_ARCSS_MISC_STC;
#endif

   pstPlatformConfig->stMailbox.uiHost2ViceMailboxAddress = BCHP_VICE2_ARCSS_ESS_DCCM_0_DATAi_ARRAY_BASE + HOST2VICE_MBOX_OFFSET;
   pstPlatformConfig->stMailbox.uiVice2HostMailboxAddress = BCHP_VICE2_ARCSS_ESS_DCCM_0_DATAi_ARRAY_BASE + VICE2HOST_MBOX_OFFSET;
   pstPlatformConfig->stMailbox.uiBvn2ViceMailboxAddress = BCHP_VICE2_ARCSS_ESS_DCCM_0_DATAi_ARRAY_BASE + BVN2VICE_MBOX_OFFSET;

#ifdef BCHP_PWR_SUPPORT
#if BCHP_PWR_RESOURCE_VICE0_PWR
   pstPlatformConfig->stPower.astResource[BVCE_Power_Type_ePower].id = BCHP_PWR_RESOURCE_VICE0_PWR;
#endif
#if BCHP_PWR_RESOURCE_VICE0_CLK
   pstPlatformConfig->stPower.astResource[BVCE_Power_Type_eClock].id = BCHP_PWR_RESOURCE_VICE0_CLK;
#endif
#endif

   return BERR_TRACE( BERR_SUCCESS );
}

/* The following reset code is for 7425 B0 */
#include "bchp_vice2_mau_0.h"
BERR_Code
BVCE_Platform_P_PreReset(
   unsigned uiInstance,
   BREG_Handle hReg
   )
{
   uint32_t uiReg;
   BDBG_ASSERT( uiInstance < BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES );

   BSTD_UNUSED( uiInstance );

   BDBG_ENTER( BVCE_Platform_P_PreReset );

#if (BCHP_CHIP == 7425) && (BCHP_VER >= BCHP_VER_B0)
   /* VICE Shutdown Sequence from Brad Delanghe 05/06/2011:
    *
    * Proposal for 7425B0:
    *  1. SCB transaction will be discarded immediately
    *    Note:
    *       a. This does not comply with the SCB bus spec
    *       b. This could potentially corrupt data for downstream blocks
    *       c. This will not work for PFRI transactions, due to the multi-cycle nature of the standard
    *  2. PFRI transactions need to be shut-down using a multi cycle procedure.
    *       a. Write 1 to MAU register FORCE_SHUTOFF to stop any future patch requests from ViCE internal clients
    *       b. Poll MAU core status register to confirm it is IDLE
    *       c. Issue sw_init
    *  3. ViCE design team is making efforts to verify that the sw_init can be issued at any time (while idle or while processing), but there is no guarantee that all possible cases can be verified.  Please use the random sw_init with caution.
    *
    */

   /* Check MAU Status */
   uiReg = BREG_Read32( hReg, BCHP_VICE2_MAU_0_STATUS );
   if ( BCHP_FIELD_ENUM( VICE2_MAU_0_STATUS, MAU_STATUS, BUSY) == BCHP_GET_FIELD_DATA( uiReg, VICE2_MAU_0_STATUS, MAU_STATUS ) )
   {
      BDBG_ERR(("MAU is BUSY before FORCE_SHUTOFF"));
   }

   /* Set Force Shutoff */
   uiReg = 0;
   BCHP_SET_FIELD_ENUM(uiReg, VICE2_MAU_0_FW_CONTROL, FORCE_SHUTOFF, ENABLE );
   BREG_Write32(hReg, BCHP_VICE2_MAU_0_FW_CONTROL, uiReg );

   /* SW7425-3816: SW INIT MAU_PFRI to clear the outstanding PFRI state */
   BKNI_Sleep(10);
   uiReg = 0;
   BCHP_SET_FIELD_DATA(uiReg, VICE2_MISC_SW_INIT_SET, MAU_PFRI, 1 );
   BREG_Write32(hReg, BCHP_VICE2_MISC_SW_INIT_SET, uiReg );
   BKNI_Sleep(1);
   BREG_Write32(hReg, BCHP_VICE2_MISC_SW_INIT_CLR, uiReg );

   /* Check status until idle */
   uiReg = BREG_Read32( hReg, BCHP_VICE2_MAU_0_STATUS );
   while ( BCHP_FIELD_ENUM( VICE2_MAU_0_STATUS, MAU_STATUS, BUSY) == BCHP_GET_FIELD_DATA( uiReg, VICE2_MAU_0_STATUS, MAU_STATUS ) )
   {
      BDBG_WRN(("MAU is BUSY..."));
      BKNI_Sleep(1);
      uiReg = BREG_Read32( hReg, BCHP_VICE2_MAU_0_STATUS );
   }
#else
   BSTD_UNUSED( hReg );
   BSTD_UNUSED( uiReg );
#endif

   BDBG_LEAVE( BVCE_Platform_P_PreReset );

   return BERR_TRACE( BERR_SUCCESS );
}

/* Enable UART for Vice Arc 0/1 */
BERR_Code
BVCE_Platform_P_EnableUART(
   BREG_Handle hReg
   )
{
   uint32_t uiReg;

   uiReg = BREG_Read32(
            hReg,
            BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL
            );

   BCHP_SET_FIELD_ENUM(uiReg, SUN_TOP_CTRL_UART_ROUTER_SEL, port_6_cpu_sel, VICE2_ARC0);
   BCHP_SET_FIELD_ENUM(uiReg, SUN_TOP_CTRL_UART_ROUTER_SEL, port_5_cpu_sel, VICE2_ARC1);
   BCHP_SET_FIELD_ENUM(uiReg, SUN_TOP_CTRL_UART_ROUTER_SEL, port_4_cpu_sel, VICE2_ARC0);
   BCHP_SET_FIELD_ENUM(uiReg, SUN_TOP_CTRL_UART_ROUTER_SEL, port_3_cpu_sel, VICE2_ARC1);

   BREG_Write32(
            hReg,
            BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,
            uiReg
            );

   uiReg = BREG_Read32(
      hReg,
      BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10
      );

   BCHP_SET_FIELD_ENUM( uiReg, SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_041, TP_IN_21);
   BCHP_SET_FIELD_ENUM( uiReg, SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_042, TP_OUT_22);
   BCHP_SET_FIELD_ENUM( uiReg, SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_037, TP_IN_17);
   BCHP_SET_FIELD_ENUM( uiReg, SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_038, TP_OUT_18);

   BREG_Write32(
            hReg,
            BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,
            uiReg
            );

   return BERR_SUCCESS;
}
