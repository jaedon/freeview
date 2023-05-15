/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_power_management.c $
* $brcm_Revision: 36 $
* $brcm_Date: 11/2/10 12:38p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/7400/src/nexus_power_management.c $
* 
* 36   11/2/10 12:38p jhaberf
* SW35125-1: Added 35125 to checks for BCHP_CHIP
* 
* 35   9/15/10 5:13p erickson
* SW7425-9: fix 7425 and all future chips that don't have any code in
*  nexus_power_management.c
*
* 34   3/26/10 3:08p erickson
* SW7325-697: added NEXUS_PowerManagementCore_eImageInput. requires for
*  7325/7335 MFD dynamic power management.
*
* 33   9/10/09 4:34p jhaberf
* SW35230-1: Creating 35230 DTV chip build environment
*
* 32   8/19/09 4:04p mward
* PR55545: Support for 7125.
*
* 31   8/11/09 2:19a jrubio
* PR55882: ad 7340 support
*
* 30   7/16/09 1:06p erickson
* PR55968: added skeleton 7420 code, but commented out for now
*
* 29   7/10/09 6:14p jhaberf
* PR53796: Modifying nexus files to build for 35130.
*
* 28   6/26/09 2:30p erickson
* PR55968: fix 7405
*
* 27   6/26/09 10:37a jrubio
* PR55968: 7325 has different Registers for PWR
*
* 26   6/25/09 2:56p erickson
* PR45902: add NEXUS_PowerManagementCore_eVideoDecoder
*
* 25   6/24/09 12:49p erickson
* PR55968: separate set-top and DTV power management code
*
***************************************************************************/
#include "nexus_core_module.h"
#include "nexus_power_management.h"
#include "priv/nexus_core.h"

#if (BCHP_CHIP==7325) || (BCHP_CHIP==7340)
#include "bchp_clkgen.h"
#elif (BCHP_CHIP==35125) || (BCHP_CHIP==35230) || (BCHP_CHIP==35330)
#include "bchp_clkgen_regs.h"
#elif (BCHP_CHIP==7335)
#include "bchp_clk.h"
#else
/* no BCHP_CLK used, therefore no #include needed.
if your chip needs BCHP_CLK registers, please add to the preceding #if lists
with positive "==" tests, not negative "!=" tests. */
#endif

BDBG_MODULE(nexus_power_management);

static int g_powerState[NEXUS_PowerManagementCore_eMax];
static bool g_powerStatus[NEXUS_PowerManagementCore_eMax];
static bool g_force;

void NEXUS_PowerManagement_Init(void)
{
    BKNI_Memset(g_powerState, 0, sizeof(g_powerState));
    BKNI_Memset(g_powerStatus, 0, sizeof(g_powerStatus)); /* force the change */

    /* set init power state */
    g_force = true;
    NEXUS_PowerManagement_SetCoreState(0, false);
    g_force = false;
}

void NEXUS_PowerManagement_Uninit(void)
{
    BKNI_Memset(g_powerState, 0, sizeof(g_powerState));
    g_force = true;
    NEXUS_PowerManagement_SetCoreState(0, false);
    g_force = false;
}

#if NEXUS_POWER_MANAGEMENT
static void NEXUS_P_SetXptPower(BREG_Handle reg, bool powerOn)
{
#if 1
    /* PR 53709 - disabling XPT power management until we solve long term stress test stability issues. */
    BSTD_UNUSED(reg);
    BSTD_UNUSED(powerOn);
#else
    BREG_AtomicUpdate32( reg, BCHP_CLKGEN_PWRDN_CTRL_0,
        BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_XPT_MASK,
        powerOn ? 0 : BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_XPT_MASK );
#endif
}

#if BCHP_CHIP == 7335
static void NEXUS_P_SetGfxPower(BREG_Handle reg, bool powerOn)
{
    BREG_AtomicUpdate32( reg, BCHP_CLK_PM_CTRL,
        BCHP_CLK_PM_CTRL_DIS_GFX_108M_CLK_MASK,
        powerOn ? 0 : BCHP_CLK_PM_CTRL_DIS_GFX_108M_CLK_MASK );

    BREG_AtomicUpdate32( reg, BCHP_CLK_PM_CTRL_2,
        BCHP_CLK_PM_CTRL_2_DIS_GFX_216M_CLK_MASK,
        powerOn ? 0 : BCHP_CLK_PM_CTRL_2_DIS_GFX_216M_CLK_MASK );
}
#elif BCHP_CHIP == 7325
static void NEXUS_P_SetGfxPower(BREG_Handle reg, bool powerOn)
{
    BREG_AtomicUpdate32( reg, BCHP_CLKGEN_PWRDN_CTRL_1,
       BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_GFX_MASK,
       powerOn ? 0 : BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_GFX_MASK);

    BREG_AtomicUpdate32( reg, BCHP_CLKGEN_PWRDN_CTRL_0,
        BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_GFX_MASK,
        powerOn ? 0 : BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_GFX_MASK );
}

#endif

#endif

void NEXUS_PowerManagement_SetCoreState( NEXUS_PowerManagementCore core, bool poweredUp )
{
    bool desiredStatus;

    if (core >= NEXUS_PowerManagementCore_eMax) {
        BDBG_ERR(("Invalid power management core: %d", core));
        return;
    }

    if (poweredUp) {
        g_powerState[core]++;
    }
    else {
        if (g_powerState[core]) {
            g_powerState[core]--;
        }
    }

#if NEXUS_POWER_MANAGEMENT
    desiredStatus = g_powerState[NEXUS_PowerManagementCore_eTransport] ||
            g_powerState[NEXUS_PowerManagementCore_eHsm];
    if (g_force || desiredStatus != g_powerStatus[NEXUS_PowerManagementCore_eTransport]) {
        BDBG_MSG(("powering %s XPT clocks (%d %d)", desiredStatus?"up":"down",
            g_powerState[NEXUS_PowerManagementCore_eTransport],
            g_powerState[NEXUS_PowerManagementCore_eHsm]));
        NEXUS_P_SetXptPower(g_pCoreHandles->reg, desiredStatus);
        g_powerStatus[NEXUS_PowerManagementCore_eTransport] = desiredStatus;
    }

#if BCHP_CHIP == 7325 || BCHP_CHIP == 7335
    desiredStatus =
        g_powerState[NEXUS_PowerManagementCore_eGraphics2D] ||
        g_powerState[NEXUS_PowerManagementCore_eGraphics3D] ||
        g_powerState[NEXUS_PowerManagementCore_eDma] ||
        g_powerState[NEXUS_PowerManagementCore_eImageInput] || /* MFD is wired to GFX clock */
        g_powerState[NEXUS_PowerManagementCore_eVideoDecoder]; /* MFD is wired to GFX clock */
    if (g_force || desiredStatus != g_powerStatus[NEXUS_PowerManagementCore_eGraphics2D]) {
        BDBG_MSG(("powering %s GFX clocks", desiredStatus?"up":"down"));
        NEXUS_P_SetGfxPower(g_pCoreHandles->reg, desiredStatus);
        g_powerStatus[NEXUS_PowerManagementCore_eGraphics2D] = desiredStatus;
    }
    /* 7405, 7400 cannot power down GFX because it's tied to MEMC power */
#elif BCHP_CHIP == 7420
    /* 7420 has separate power down control for GFX & DMA */
#if 0
    desiredStatus =
        g_powerState[NEXUS_PowerManagementCore_eGraphics2D] ||
        g_powerState[NEXUS_PowerManagementCore_eGraphics3D];
    if (g_force || desiredStatus != g_powerStatus[NEXUS_PowerManagementCore_eGraphics2D]) {
        BDBG_MSG(("powering %s GFX clocks", desiredStatus?"up":"down"));
        NEXUS_P_SetGfxPower(g_pCoreHandles->reg, desiredStatus);
        g_powerStatus[NEXUS_PowerManagementCore_eGraphics2D] = desiredStatus;
    }

    desiredStatus = g_powerState[NEXUS_PowerManagementCore_eDma];
    if (g_force || desiredStatus != g_powerStatus[NEXUS_PowerManagementCore_eDma]) {
        BDBG_MSG(("powering %s DMA clocks", desiredStatus?"up":"down"));
        NEXUS_P_SetDmaPower(g_pCoreHandles->reg, desiredStatus);
        g_powerStatus[NEXUS_PowerManagementCore_eDma] = desiredStatus;
    }
#endif
#endif

#else
    BSTD_UNUSED(desiredStatus);
#endif
}

void NEXUS_PowerManagement_GetCoreState( NEXUS_PowerManagementCore core, bool *pPoweredUp )
{
    if (core >= NEXUS_PowerManagementCore_eMax) {
        BDBG_ERR(("Invalid power management core: %d", core));
        return;
    }

    *pPoweredUp = g_powerState[core];
}
