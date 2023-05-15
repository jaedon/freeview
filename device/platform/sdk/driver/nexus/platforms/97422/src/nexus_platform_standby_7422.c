/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_platform_standby_7422.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 8/21/12 11:37a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97422/src/nexus_platform_standby_7422.c $
 * 
 * 2   8/21/12 11:37a erickson
 * SW7435-235: remove unused code
 * 
 * 1   10/7/10 4:07p nickh
 * SW7422-10: Add initial version
 *
 *****************************************************************************/
#include "nexus_platform_standby.h"
#include "nexus_platform_priv.h"

#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_kbd1.h"
#include "bchp_memc_arb_0.h"
#include "bchp_memc_arb_1.h"
#include "bchp_memc_ddr_0.h"
#include "bchp_memc_ddr_1.h"
#if 0
#include "bchp_memc_ddr23_aphy_ac_0.h"
#include "bchp_memc_ddr23_aphy_wl0_0.h"
#include "bchp_memc_ddr23_aphy_wl1_0.h"
#include "bchp_memc_ddr23_aphy_ac_1.h"
#include "bchp_memc_ddr23_aphy_wl0_1.h"
#include "bchp_memc_ddr23_aphy_wl1_1.h"
#include "bchp_pm_l2.h"
#endif
#include "bchp_wktmr.h"
#include "bchp_timer.h"
#include "bchp_bsp_glb_control.h"
#include "bchp_bsp_cmdbuf.h"

BDBG_MODULE(nexus_platform_standby_7425);

#if NEXUS_POWER_STANDBY
#define BDEV_WR(addr, data) BREG_Write32(g_standbyState.reg, addr, data)
#define BDEV_RD(addr)       BREG_Read32(g_standbyState.reg, addr)

#define BDEV_RD_F(reg, field) \
    ((BDEV_RD(BCHP_##reg) & BCHP_##reg##_##field##_MASK) >> \
     BCHP_##reg##_##field##_SHIFT)
#define BDEV_WR_F_RB(reg, field, val) do { \
    BDEV_WR(BCHP_##reg, \
    (BDEV_RD(BCHP_##reg) & ~BCHP_##reg##_##field##_MASK) | \
    (((val) << BCHP_##reg##_##field##_SHIFT) & \
     BCHP_##reg##_##field##_MASK)); \
    BDEV_RD(BCHP_##reg); \
    } while(0)

void NEXUS_Platform_P_PrePwrPassiveStandby()
{
    /* reset everything before doing PWR down */
    BREG_Write32(g_standbyState.reg, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET, 0x5ff00);
    BREG_Write32(g_standbyState.reg, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET, 0x0);
}

NEXUS_Error NEXUS_Platform_P_PreStandby()
{
    unsigned i;
    unsigned delay = 20; /* TODO */
    uint32_t tmp;

    /* TODO: move some of this into PWR */
#if 0
    BDEV_WR_F_RB(SUN_TOP_CTRL_OTP_OPTION_TEST_0, otp_option_pwr_ctrl_disable, 0); 
#endif

    /* disable AVD, RPT clocks */
#if 0
    BDEV_WR_F_RB(VCXO_CTL_MISC_RAP_AVD_PLL_CTRL, RESET, 1);
    BDEV_WR_F_RB(VCXO_CTL_MISC_RAP_AVD_PLL_CTRL, POWERDOWN, 1);
#endif

    /* TODO: linux will handle MEMC0, possibly MEMC1 */

    /* disable MEMC1 */
#if 0
    BDEV_WR_F_RB(CLK_MEMC_1_PM_CTRL, DIS_216M_CLK, 0);
    BDEV_WR_F_RB(CLK_MEMC_1_PM_CTRL, DIS_108M_CLK, 0);
    BDEV_WR_F_RB(CLK_DDR23_APHY_1_PM_CTRL, DIS_216M_CLK, 0);
    BDEV_WR_F_RB(CLK_DDR23_APHY_1_PM_CTRL, DIS_108M_CLK, 0);

    BDEV_WR_F_RB(MEMC_ARB_1_CLIENT_INFO_127, RR_EN, 0);
    BDEV_WR_F_RB(MEMC_DDR_1_SSPD_CMD, SSPD, 1);
    BDEV_WR_F_RB(MEMC_DDR_1_WARM_BOOT, WARM_BOOT, 1);
#endif

#if 0
    BDEV_WR_F_RB(MEMC_DDR23_APHY_AC_1_DDR_PAD_CNTRL, DEVCLK_OFF_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_AC_1_DDR_PAD_CNTRL, HIZ_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_AC_1_DDR_PAD_CNTRL, IDDQ_MODE_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_AC_1_POWERDOWN, PLLCLKS_OFF_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_WL0_1_DDR_PAD_CNTRL, IDDQ_MODE_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_WL0_1_WORDSLICE_CNTRL_1, PWRDN_DLL_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_WL1_1_DDR_PAD_CNTRL, IDDQ_MODE_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_WL1_1_WORDSLICE_CNTRL_1, PWRDN_DLL_ON_SELFREF, 1);
#endif

#if 0
    BDEV_WR_F_RB(MEMC_DDR23_APHY_AC_1_PLL_CTRL0_REG, PDN_BGR, 1);
    BDEV_WR_F_RB(CLK_DDR23_APHY_1_PM_CTRL, DIS_216M_CLK, 1);
    BDEV_WR_F_RB(CLK_DDR23_APHY_1_PM_CTRL, DIS_108M_CLK, 1);
    BDEV_WR_F_RB(CLK_MEMC_1_PM_CTRL, DIS_216M_CLK, 1);
    BDEV_WR_F_RB(CLK_MEMC_1_PM_CTRL, DIS_108M_CLK, 1);
#endif

    /* MEMC_APHY_0 init */

#if 0
    BDEV_WR_F_RB(MEMC_DDR23_APHY_AC_0_DDR_PAD_CNTRL, DEVCLK_OFF_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_AC_0_DDR_PAD_CNTRL, IDDQ_MODE_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_AC_0_POWERDOWN, PLLCLKS_OFF_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_WL0_0_DDR_PAD_CNTRL, IDDQ_MODE_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_WL0_0_WORDSLICE_CNTRL_1, PWRDN_DLL_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_WL1_0_DDR_PAD_CNTRL, IDDQ_MODE_ON_SELFREF, 1);
    BDEV_WR_F_RB(MEMC_DDR23_APHY_WL1_0_WORDSLICE_CNTRL_1, PWRDN_DLL_ON_SELFREF, 1);
#endif

    /* BSP */
    i = 0;
    while (BDEV_RD_F(BSP_GLB_CONTROL_GLB_IRDY, CMD_IDRY2) != 1) {
        if (i++ == 10) {
            BDBG_WRN(("BSP CMD_IDRY2 timeout"));
            break;
        }
        BKNI_Sleep(10);
    }
    BREG_Write32(g_standbyState.reg, BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + 0x180, 0x00000010);
    BREG_Write32(g_standbyState.reg, BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + 0x184, 0x00000098);
    BREG_Write32(g_standbyState.reg, BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + 0x188, 0xabcdef00);
    BREG_Write32(g_standbyState.reg, BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + 0x18c, 0xb055aa4f);
    BREG_Write32(g_standbyState.reg, BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + 0x190, 0x789a0004);
    BREG_Write32(g_standbyState.reg, BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + 0x194, 0x00000000);

    BDEV_WR_F_RB(BSP_GLB_CONTROL_GLB_ILOAD2, CMD_ILOAD2, 1);

    i = 0;
    while (BDEV_RD_F(BSP_GLB_CONTROL_GLB_OLOAD2, CMD_OLOAD2) != 1) {
        if (i++ == 10) {
            BDBG_WRN(("BSP CMD_OLOAD2 timeout"));
            break;
        }
        BKNI_Sleep(10);
    }

    BREG_Write32(g_standbyState.reg, BCHP_BSP_GLB_CONTROL_GLB_HOST_INTR_STATUS, 0);
    BREG_Write32(g_standbyState.reg, BCHP_BSP_GLB_CONTROL_GLB_OLOAD2, 0);
    tmp = BREG_Read32(g_standbyState.reg, BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE + 0x494);
    if (tmp != 0 && tmp != 1) {
        BDBG_ERR(("BSP command failed: %08lx. This probably means that PM is disabled via OTP."));
        return BERR_TRACE(-1);
    }

    /* set up IR */
    BREG_Write32(g_standbyState.reg, BCHP_KBD1_STATUS, 0);
    BREG_Write32(g_standbyState.reg, BCHP_KBD1_DIVISOR, 0x1f);
    BREG_Write32(g_standbyState.reg, BCHP_KBD1_FILTER1, 0);
    BREG_Write32(g_standbyState.reg, BCHP_KBD1_DATA1, 0);
    BREG_Write32(g_standbyState.reg, BCHP_KBD1_DATA0, 0);
    BREG_Write32(g_standbyState.reg, BCHP_KBD1_CMD, 0x67);
    BREG_Write32(g_standbyState.reg, BCHP_KBD1_KBD_MASK0, 0);
    BREG_Write32(g_standbyState.reg, BCHP_KBD1_KBD_MASK1, 0);
    BREG_Write32(g_standbyState.reg, BCHP_KBD1_KBD_PAT0, 0);
    BREG_Write32(g_standbyState.reg, BCHP_KBD1_KBD_PAT1, 0);

    BREG_Write32(g_standbyState.reg, BCHP_WKTMR_EVENT, 1);
    BREG_Write32(g_standbyState.reg, BCHP_WKTMR_ALARM, BREG_Read32(g_standbyState.reg, BCHP_WKTMR_COUNTER) + delay);
    if ((int)delay == -1) {
        BDBG_WRN(("Will loop until SUN_TOP_CTRL_UNCLEARED_SCRATCH != 0"));
        BREG_Write32(g_standbyState.reg, BCHP_SUN_TOP_CTRL_UNCLEARED_SCRATCH, 0);
    }
    else {
        BREG_Write32(g_standbyState.reg, BCHP_SUN_TOP_CTRL_UNCLEARED_SCRATCH, 1);
    }
#if 0
    BREG_Write32(g_standbyState.reg, BCHP_PM_L2_CPU_MASK_SET, 0xffffffff);
    BREG_Write32(g_standbyState.reg, BCHP_PM_L2_CPU_CLEAR, 0xffffffff);
    BDEV_WR_F_RB(PM_L2_CPU_MASK_CLEAR, TIMER_INTR, 1);
    BDEV_WR_F_RB(PM_L2_CPU_MASK_CLEAR, IRR_INTR, 1);
#endif
    return 0;
}

NEXUS_Error NEXUS_Platform_P_PostStandby()
{
    BREG_Write32(g_standbyState.reg, BCHP_TIMER_WDCMD, 0xee00);
    BREG_Write32(g_standbyState.reg, BCHP_TIMER_WDCMD, 0x00ee);

    /* TODO: put avd into/out of reset. temporarily required for PWR pi.
    BREG_Write32(g_standbyState.reg, BCHP_SUN_TOP_CTRL_SW_RESET, 0x6000000);
     */

    /* DME: this big reset is probably not needed if we do the big reset before PWR PI goes into passive standby. */
    /* reset as much as possible */
    BREG_Write32(g_standbyState.reg, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET, 0x5ff00);
    BREG_Write32(g_standbyState.reg, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET, 0x0);

    /* enable AVD, RPT clocks */
#if 0
    BDEV_WR_F_RB(VCXO_CTL_MISC_RAP_AVD_PLL_CTRL, POWERDOWN, 0);
    BDEV_WR_F_RB(VCXO_CTL_MISC_RAP_AVD_PLL_CTRL, RESET, 0);
#endif

    return 0;
}

#endif
