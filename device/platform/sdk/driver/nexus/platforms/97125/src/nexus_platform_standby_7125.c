
/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_platform_standby_7125.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 4/12/11 5:49p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97125/src/nexus_platform_standby_7125.c $
 * 
 * 11   4/12/11 5:49p mward
 * SW7125-905:  Functions for Power Management 2.0.
 * 
 * 10   11/24/10 2:47p gskerl
 * SW7125-727: Merged to main branch
 * 
 * SW7125-727/2   11/24/10 2:34p gskerl
 * SW7125-727: Removed reset code from before and after suspend
 * 
 * 9   10/12/10 2:20p mward
 * SW7125-237: Removed unused variables.
 * 
 * 8   10/12/10 11:31a gskerl
 * SW7125-237: Removed BSP suspend handshake since it is now done by the
 *  kernel (since 2.6.31-2.4)
 * 
 * 7   9/7/10 6:24p gskerl
 * SW7125-237: Allow app to pass passive standby wakeup devices through
 *  NEXUS_PlatformStandbySettings struct
 * 
 * 6   9/2/10 2:58p gskerl
 * SW7125-237: Added missing argument to error message. Also, for BSP
 *  handshake failure, just print error message and continue normally
 * 
 * 5   8/24/10 6:02p gskerl
 * SW7125-237: Eliminated default wakeup after 10 seconds. Removed debug
 *  statement
 * 
 * 4   8/24/10 2:35p gskerl
 * SW7125-237: Added passive standby support
 * 
 * 3   5/28/10 6:15p gskerl
 * SW7125-403: Started adding support active standby for 7125
 * 
 * 3   5/28/10 6:14p gskerl
 * SW7125-403: Started adding support active standby for 7125
 * 
 * 2   3/8/10 4:26p erickson
 * SW7405-3986: rename macro to NEXUS_POWER_STANDBY
 * 
 * 1   1/29/10 3:45p mward
 * SW7125-138: Adding the platform-specific g_passiveModuleList.
 * 
 * 1   1/28/10 4:36p erickson
 * SW7420-532: refactor nexus platform standby for linux 2.6.31
 *
 *****************************************************************************/

#include "nexus_platform_standby.h"
#include "nexus_platform_priv.h"

#include "bchp_pm_l2.h"
#include "bchp_wktmr.h"

BDBG_MODULE(nexus_platform_standby_7125);


#if NEXUS_POWER_MANAGEMENT
void NEXUS_Platform_P_ResetWakeupDevices(const NEXUS_PlatformStandbySettings *pSettings)
{
    BSTD_UNUSED(pSettings);

    BREG_Write32(g_pCoreHandles->reg, BCHP_PM_L2_CPU_MASK_SET, 0xFFFFFFFF);
    BREG_Write32(g_pCoreHandles->reg, BCHP_PM_L2_CPU_CLEAR, 0xFFFFFFFF);
}

void NEXUS_Platform_P_SetWakeupDevices(const NEXUS_PlatformStandbySettings *pSettings)
{     
    unsigned val;
    
    val = BREG_Read32(g_pCoreHandles->reg, BCHP_PM_L2_CPU_MASK_CLEAR);
    /* Set IR for wakeup */
    if(pSettings->wakeupSettings.ir) {	   
	val |= BCHP_PM_L2_CPU_MASK_CLEAR_IRR_INTR_MASK;
    }	
    
    /* Set keypad for wakeup */
    if(pSettings->wakeupSettings.keypad) {
	val |= BCHP_PM_L2_CPU_MASK_CLEAR_KPD_INTR_MASK;
    }	
	
    /* Set gpio for wakeup */
    if(pSettings->wakeupSettings.gpio) {
	val |= BCHP_PM_L2_CPU_MASK_CLEAR_GPIO_MASK;
    }
	
    /* Set cec for wakeup */
    if(pSettings->wakeupSettings.cec) {
	val |= BCHP_PM_L2_CPU_MASK_CLEAR_CEC_INTR_MASK;
    }
    
    /* Set timeout for wakeup */
    if(pSettings->wakeupSettings.timeout) {	
	unsigned counter = BREG_Read32(g_pCoreHandles->reg, BCHP_WKTMR_COUNTER);	
	BREG_Write32(g_pCoreHandles->reg, BCHP_WKTMR_EVENT, 1);
	BREG_Write32(g_pCoreHandles->reg, BCHP_WKTMR_ALARM, (counter + pSettings->wakeupSettings.timeout));
	val |= BCHP_PM_L2_CPU_MASK_CLEAR_TIMER_INTR_MASK;	
    }    	
    BREG_Write32(g_pCoreHandles->reg, BCHP_PM_L2_CPU_MASK_CLEAR, val);
}

void NEXUS_Platform_P_GetStandbyStatus(NEXUS_PlatformStandbyStatus *pStatus)
{
    unsigned long   l2CpuIntStatus = 0;

    l2CpuIntStatus = BREG_Read32(g_pCoreHandles->reg, BCHP_PM_L2_CPU_STATUS);

    if (l2CpuIntStatus & BCHP_PM_L2_CPU_STATUS_IRR_INTR_MASK) {
	pStatus->wakeupStatus.ir = true;
    }
    else if (l2CpuIntStatus & BCHP_PM_L2_CPU_MASK_CLEAR_KPD_INTR_MASK) {
	pStatus->wakeupStatus.keypad = true;
    }
    else if (l2CpuIntStatus & BCHP_PM_L2_CPU_MASK_CLEAR_GPIO_MASK) {
	pStatus->wakeupStatus.gpio = true;
    }
    else if (l2CpuIntStatus & BCHP_PM_L2_CPU_MASK_CLEAR_CEC_INTR_MASK) {
	pStatus->wakeupStatus.cec = true;
    }
    else if (l2CpuIntStatus & BCHP_PM_L2_CPU_STATUS_TIMER_INTR_MASK) {
	pStatus->wakeupStatus.timeout = true;
    }
}
#endif
