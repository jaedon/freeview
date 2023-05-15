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
 * $brcm_Workfile: nexus_platform_standby_7425.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 4/2/12 3:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97425/src/nexus_platform_standby_7425.c $
 * 
 * 6   4/2/12 3:44p jtna
 * SW7425-1183: fix BDBG_MODULE string. fix indentation
 * 
 * 5   12/21/11 12:23p gmohile
 * SW7425-1183: Fix wake up status
 * 
 * 4   12/2/11 11:20a gmohile
 * SW7425-1183: Add transport wake up
 * 
 * 3   8/9/11 4:56p gmohile
 * SW7425-533: Fix wake up status
 * 
 * 2   6/9/11 7:04p nickh
 * SW7425-533: Add wakeup device support
 * 
 * 1   9/14/10 7:06p hongtaoz
 * SW7425-9: add initial version to compile for 7425;
 *
 *****************************************************************************/

#include "nexus_platform_standby.h"
#include "nexus_platform_priv.h"

#include "bchp_aon_pm_l2.h"
#include "bchp_wktmr.h"

BDBG_MODULE(nexus_platform_standby_7425);

#if NEXUS_POWER_MANAGEMENT
void NEXUS_Platform_P_ResetWakeupDevices(const NEXUS_PlatformStandbySettings *pSettings)
{
    BSTD_UNUSED(pSettings);

    BREG_Write32(g_pCoreHandles->reg, BCHP_AON_PM_L2_CPU_MASK_SET, 0xFFFFFFFF);
    BREG_Write32(g_pCoreHandles->reg, BCHP_AON_PM_L2_CPU_CLEAR, 0xFFFFFFFF);
}

void NEXUS_Platform_P_SetWakeupDevices(const NEXUS_PlatformStandbySettings *pSettings)
{     
    unsigned val;
    
    val = BREG_Read32(g_pCoreHandles->reg, BCHP_AON_PM_L2_CPU_MASK_CLEAR);

    /* Set IR for wakeup */
    if(pSettings->wakeupSettings.ir) {
        val |= BCHP_AON_PM_L2_CPU_MASK_CLEAR_IRR_INTR_MASK;
    }

    /* Set keypad for wakeup */
    if(pSettings->wakeupSettings.keypad) {
        val |= BCHP_AON_PM_L2_CPU_MASK_CLEAR_KPD_INTR_MASK;
    }

    /* Set gpio for wakeup */
    if(pSettings->wakeupSettings.gpio) {
        val |= BCHP_AON_PM_L2_CPU_MASK_CLEAR_GPIO_MASK;
    }

    /* Set cec for wakeup */
    if(pSettings->wakeupSettings.cec) {
        val |= BCHP_AON_PM_L2_CPU_MASK_CLEAR_CEC_INTR_MASK;
    }
    
    /* Set xpt for wakeup */
    if(pSettings->wakeupSettings.transport) {
        val |= BCHP_AON_PM_L2_CPU_MASK_CLEAR_XPT_PMU_MASK;
    }

    /* Set timeout for wakeup */
    if(pSettings->wakeupSettings.timeout) {
        unsigned counter = BREG_Read32(g_pCoreHandles->reg, BCHP_WKTMR_COUNTER);
        BREG_Write32(g_pCoreHandles->reg, BCHP_WKTMR_EVENT, 1);
        BREG_Write32(g_pCoreHandles->reg, BCHP_WKTMR_ALARM, (counter + pSettings->wakeupSettings.timeout));
        val |= BCHP_AON_PM_L2_CPU_MASK_CLEAR_TIMER_INTR_MASK;
    }
    BREG_Write32(g_pCoreHandles->reg, BCHP_AON_PM_L2_CPU_MASK_CLEAR, val);
}

void NEXUS_Platform_P_GetStandbyStatus(NEXUS_PlatformStandbyStatus *pStatus)
{
    unsigned long l2CpuIntStatus = 0;

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    l2CpuIntStatus = BREG_Read32(g_pCoreHandles->reg, BCHP_AON_PM_L2_CPU_STATUS);
    
    if (l2CpuIntStatus & BCHP_AON_PM_L2_CPU_STATUS_IRR_INTR_MASK) {
        pStatus->wakeupStatus.ir = true;
    }
    if (l2CpuIntStatus & BCHP_AON_PM_L2_CPU_STATUS_KPD_INTR_MASK) {
        pStatus->wakeupStatus.keypad = true;
    }
    if (l2CpuIntStatus & BCHP_AON_PM_L2_CPU_STATUS_GPIO_MASK) {
        pStatus->wakeupStatus.gpio = true;
    }
    if (l2CpuIntStatus & BCHP_AON_PM_L2_CPU_STATUS_CEC_INTR_MASK) {
        pStatus->wakeupStatus.cec = true;
    }
    if (l2CpuIntStatus & BCHP_AON_PM_L2_CPU_STATUS_XPT_PMU_MASK) {
        pStatus->wakeupStatus.transport = true;
    }
    if (l2CpuIntStatus & BCHP_AON_PM_L2_CPU_STATUS_TIMER_INTR_MASK) {
        pStatus->wakeupStatus.timeout = true;
    }
}
#endif
