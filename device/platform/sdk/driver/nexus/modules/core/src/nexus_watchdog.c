/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_watchdog.c $
* $brcm_Revision: 4 $
* $brcm_Date: 12/19/11 11:28a $
*
* API Description:
*   API name: Watchdog
*    Specific APIs related to HW watchdog
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/7400/src/nexus_watchdog.c $
* 
* 4   12/19/11 11:28a erickson
* SW7125-1181: merge, use bchp_common.h to detect AON chips
* 
* SW7125-1181/2   12/16/11 4:18p mward
* SW7125-1811:  Default to the AON reset status method to avoid errors
*  for future chips.
* 
* SW7125-1181/1   12/16/11 3:45p mward
* SW7125-1181:  BCHP_TIMER_WDCRS register can't be used to reliably check
*  for watchdog resets on secured parts. Use RESET registers in AON if
*  available, SUN_TOP otherwise.
* 
* 3   9/3/10 11:12a erickson
* SWDEPRECATED-3768: add NEXUS_WATCHDOG_MIN_TIMEOUT because of possible
*  conflict with GISB timeouts
*
* 2   5/6/10 5:27p erickson
* SW7400-2743: check BCHP_SUN_TOP_CTRL_RESET_CTRL_watchdog_reset_enable
*
* 1   4/20/09 10:23a jtna
* PR54011: added nexus_watchdog.c
*
***************************************************************************/

#include "nexus_watchdog.h"
#include "nexus_core_module.h"
#include "bchp_common.h"
#include "bchp_timer.h" /* TODO: we are assuming that this is available on all platforms and the register
                           names are identical. this appears to be the case at least for now */
#include "bchp_sun_top_ctrl.h"

BDBG_MODULE(nexus_watchdog);

#define NEXUS_WATCHDOG_TIMER_FREQ 27000000 /* 27 MHz */

static bool g_watchdog_started = false;
static unsigned g_watchdog_timeout = 0;


NEXUS_Error NEXUS_Watchdog_SetTimeout(unsigned timeout)
{
    if (g_watchdog_started) { /* the HW already protects against this, but it doesn't hurt to stop it in SW */
        BDBG_ERR(("NEXUS_Watchdog_SetTimeout: Timeout cannot be set while timer is counting"));
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if (timeout<NEXUS_WATCHDOG_MIN_TIMEOUT || timeout>NEXUS_WATCHDOG_MAX_TIMEOUT) {
        BDBG_ERR(("NEXUS_Watchdog_SetTimeout: Invalid timeout value specified"));
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    BDBG_ASSERT(g_NexusCore.publicHandles.reg);
    /* write timeout value to register */
    BREG_Write32(g_NexusCore.publicHandles.reg, BCHP_TIMER_WDTIMEOUT, timeout*NEXUS_WATCHDOG_TIMER_FREQ);

    g_watchdog_timeout = timeout;
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Watchdog_StartTimer(void)
{
    if (g_watchdog_timeout==0) {
        BDBG_ERR(("NEXUS_Watchdog_StartTimer: Timeout value was not previously set"));
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

#if defined(BCHP_SUN_TOP_CTRL_RESET_CTRL_watchdog_reset_enable_SHIFT)
    if (!(BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_RESET_CTRL) & BCHP_SUN_TOP_CTRL_RESET_CTRL_watchdog_reset_enable_MASK)) {
        BDBG_ERR(("NEXUS_Watchdog_StartTimer requires that BCHP_SUN_TOP_CTRL_RESET_CTRL_watchdog_reset_enable be 1. Set this in nexus_platform_pinmux.c or in your application."));
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
#endif

    /* magic start sequence */
    BREG_Write32(g_NexusCore.publicHandles.reg, BCHP_TIMER_WDCMD, 0xff00);
    BREG_Write32(g_NexusCore.publicHandles.reg, BCHP_TIMER_WDCMD, 0x00ff);

    g_watchdog_started = true;

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Watchdog_StopTimer(void)
{
    if (!g_watchdog_started) {
        BDBG_WRN(("NEXUS_Watchdog_StopTimer: Watchdog timer was not previously started"));
        /* continue */
    }

    /* magic stop sequence */
    BREG_Write32(g_NexusCore.publicHandles.reg, BCHP_TIMER_WDCMD, 0xee00);
    BREG_Write32(g_NexusCore.publicHandles.reg, BCHP_TIMER_WDCMD, 0x00ee);

    g_watchdog_started = false;

    return NEXUS_SUCCESS;
}

#if defined(BCHP_AON_CTRL_REG_START)
#include "bchp_aon_ctrl.h"
/* SUN_TOP_CTRL_RESET_HISTORY is NOT USED in chips that have a sys_aon always-on power island module.  
   They use the reset history feature in sys_aon. */
void NEXUS_Watchdog_GetLastResetStatus(bool *pStatus)
{
    uint32_t ulVal;
    BDBG_ASSERT(g_NexusCore.publicHandles.reg);
    ulVal = BREG_Read32(g_NexusCore.publicHandles.reg, BCHP_AON_CTRL_RESET_HISTORY);
    ulVal = BCHP_GET_FIELD_DATA(ulVal, AON_CTRL_RESET_HISTORY,  watchdog_timer_reset);
	*pStatus = (ulVal!=0);
	ulVal = BREG_Read32(g_NexusCore.publicHandles.reg, BCHP_AON_CTRL_RESET_CTRL);
	BCHP_SET_FIELD_DATA( ulVal, AON_CTRL_RESET_CTRL, clear_reset_history, 1 );
    BREG_Write32(g_NexusCore.publicHandles.reg, BCHP_AON_CTRL_RESET_CTRL,ulVal); /* clear status */

    return;
}
#else
void NEXUS_Watchdog_GetLastResetStatus(bool *pStatus)
{
    uint32_t ulVal;
    BDBG_ASSERT(g_NexusCore.publicHandles.reg);
    ulVal = BREG_Read32(g_NexusCore.publicHandles.reg, BCHP_SUN_TOP_CTRL_RESET_HISTORY);
    ulVal = BCHP_GET_FIELD_DATA(ulVal, SUN_TOP_CTRL_RESET_HISTORY,  watchdog_timer_reset);
	*pStatus = (ulVal!=0);
	ulVal = BREG_Read32(g_NexusCore.publicHandles.reg, BCHP_SUN_TOP_CTRL_RESET_CTRL);
	BCHP_SET_FIELD_DATA( ulVal, SUN_TOP_CTRL_RESET_CTRL, clear_reset_history, 1 );
    BREG_Write32(g_NexusCore.publicHandles.reg, BCHP_SUN_TOP_CTRL_RESET_CTRL,ulVal); /* clear status */

    return;
}
#endif
