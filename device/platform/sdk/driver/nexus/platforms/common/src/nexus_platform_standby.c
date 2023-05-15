/***************************************************************************
*     (c)2004-2014 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_standby.c $
* $brcm_Revision: PROD_HUMAX_URSR/1 $
* $brcm_Date: 7/4/14 6:49p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/nexus_platform_standby.c $
*
* PROD_HUMAX_URSR/1   7/4/14 6:49p scho
* SW7425-4944: Preserve wakeup status during S3 cold boot
* 
* 55   10/25/12 2:09p gmohile
* SW7435-453 : Add core standby
*
* 54   10/24/12 3:05p gmohile
* SW7435-444 : Add BMRC Standby/Resume
*
* 53   9/27/12 4:26p gmohile
* SW7425-3519,SW7425-3520,SW7425-3077 : ALways resume all modulesfor
*  proper transition
*
* 52   7/26/12 4:01p gmohile
* SW7425-3519,SW7425-3520,SW7425-3077: Add frontend standby
*
* 51   5/30/12 6:35p agin
* SWNOOS-540:  Fixed compiler error with SDE.
*
* 50   5/9/12 3:35p erickson
* SW7231-748: fix warning
*
* 49   4/20/12 3:34p gmohile
* SW7231-783 : Enable proper module locking for standby
*
* 48   4/20/12 2:09p erickson
* SW7358-249: back out workaround
*
* 46   3/15/12 3:58p jtna
* SW7425-1708: fix indentation. add better comments.
*
* 45   3/5/12 3:35p rjlewis
* SW7425-2536: Add calls to BCHP_Standby/Resume (needed for AVS
*  save/restore).
*
* 44   3/5/12 2:44p gmohile
* SW7231-503 : Recover from failing to go to standby
*
* 43   3/2/12 12:39p gmohile
* SW7231-689 : 7231 AON register fix no required for B2
*
* 42   2/24/12 11:54a gmohile
* SW7231-689 : Save/restore IRQ0_AON reg for 7231 S3 warm boot
*
* 41   2/13/12 11:34a gmohile
* SW7231-655 : Coverity Defect ID:36799 MISSING_BREAK
*
* 40   2/10/12 5:17p gmohile
* SW7425-1708 : Fix transition from S3 to S1/S2
*
* 39   1/27/12 5:01p gmohile
* SW7425-1708 : initialize frontend after resume from S3
*
* 38   1/11/12 5:44p jtna
* SW7425-1708: enable BCHP_PWR_Standby/Resume
*
* 37   1/11/12 5:23p gmohile
* SW7425-1708 : re-init pinmux after S3 resume
*
* 36   12/13/11 5:18p jtna
* SW7425-1708: comment out BCHP_PWR_Standby/Resume, pending more testing
*
* 35   12/13/11 11:53a mward
* SW7425-1708: bchp_pwr.h only available if NEXUS_POWER_MANAGEMENT.
*
* 34   12/12/11 2:12p jtna
* SW7425-1708: add BCHP_PWR_Standby/Resume() for clockgen power down
*
* 33   11/17/11 4:39p jtna
* SW7425-1777: fix indentation. the if-else spaghetti was hard to read.
*
* 32   10/31/11 10:19a katrep
* SW7231-345:compiler warning
*
* 31   8/29/11 10:52a erickson
* SW7231-353: check return code of NEXUS_Platform_Init
*
* 30   8/26/11 5:00p dlwin
* SW7346-471: Fix for coverity: 35269 issue.
*
* 29   8/24/11 3:48p gmohile
* SW7231-345 : Add S3 standby support
*
* 28   7/29/11 10:49a mward
* SW7125-1014: On 7125 platforms, the app needs to power down the
*  frontends, (earlier) do not untune them in
*  NEXUS_Platform_SetStandbySettings().
*
* 27   7/21/11 4:14p gmohile
* SW7125-1014 : Rework power management
*
* 26   6/30/11 4:14p mward
* SW7125-968: i2c is needed by frontend.  Lock and unlock i2c module
*  accordingly.
*
* 25   6/29/11 5:50p mward
* SW7125-1011: Removing workaround delay.
*
* 24   6/29/11 2:39p mward
* SW7125-1011:  Delay to allow for untune to complete on slower frontends
*  before locking module.
*
* 23   5/11/11 5:29p mward
* SW7125-905:  Lock / unlock must be in exact reverse order.
*
* 22   5/11/11 3:25p mward
* SW7125-905:  Lock file and frontend modules only for passive standby.
*
* 21   3/31/11 3:58p gmohile
* SW7408-260 : Open/close VDC handles during resume/standby
*
* 20   3/1/11 5:39p gmohile
* SW7408-210 : Add standby support for XPT, SCD, TMN
*
* 19   2/25/11 2:52p gmohile
* SW7408-210 : Deprecate use of NEXUS_POWER_STANDBY
*
* 18   2/24/11 12:02p gmohile
* SW7408-210 : Add BVN standby
*
* 17   2/23/11 10:35a gmohile
* SW7408-210 : Compile time check for modules
*
* 16   2/22/11 4:16p gmohile
* SW7408-210 : Fix kernelmode standby
*
* 15   2/22/11 2:13p xhuang
* SW7408-210: fix compile error for no UHF platform
*
* 14   2/17/11 11:02p hongtaoz
* SW7408-210: fixed kernel mode compile error;
*
* 13   2/17/11 4:24p gmohile
* SW7408-210 : Merge PM 2.0 support to mainline
*
* SW7408-210/3   2/15/11 4:26p gmohile
* SW7408-210 : Add standby support
*
* SW7408-210/2   2/11/11 5:04p gmohile
* SW7408-210 : Fix active and pasive standby
*
* SW7408-210/1   2/8/11 10:54a gmohile
* SW7408-210 : Add 7420 standby support
*
* 12   9/17/10 5:56p jtna
* SW7420-972: make use of BCHP_PWR-flavor of power management
*
* 11   3/8/10 4:25p erickson
* SW7405-3986: rename macro to NEXUS_POWER_STANDBY
*
* 10   1/28/10 4:36p erickson
* SW7420-532: refactor nexus platform standby for linux 2.6.31
*
* SW7420-70/1   1/18/10 4:57p erickson
* SW7420-70: refactor nexus_platform_standby api
*
* 9   1/6/10 9:39a erickson
* SW7420-70: temp 7420 reset to make use of PWR PI more stable
*
* 8   12/22/09 2:34p erickson
* SW7420-70: add more ID's for 7420 standby
*
* 7   12/14/09 2:34p erickson
* SW7420-332: rename BSTD_TMN_ID
*
* 6   12/14/09 12:38p erickson
* SW7325-250: add NEXUS_Platform_P_UnmapMemory for register memory
*
* 5   12/11/09 5:51p erickson
* SW7325-250: fix 3548 ids. add NEXUS_Platform_P_Magnum_Init.
*
* 4   11/25/09 6:12p nickh
* SW7325-250: Add 7420 support
*
* 3   11/18/09 2:20p jrubio
* SW7325-250: clean up Compile Warnings
*
* 2   11/16/09 5:30p jrubio
* SW7325-250: make sure NEXUS_POWER_MANAGEMENT=n works
*
* 1   11/13/09 5:00p erickson
* SW7325-250: add nexus_platform_standby support
*
***************************************************************************/

#include "nexus_base.h"
#include "nexus_platform_standby.h"
#include "nexus_platform_priv.h"
#include "bchp_common.h"
#include "breg_mem.h"
#if NEXUS_POWER_MANAGEMENT
#include "bchp_pwr.h"
#endif

#if NEXUS_HAS_FRONTEND
#include "nexus_frontend_init.h"
#include "priv/nexus_frontend_standby_priv.h"
#endif



BDBG_MODULE(nexus_platform_standby);


#if NEXUS_POWER_MANAGEMENT
NEXUS_PlatformStandbyState g_standbyState = {{NEXUS_PlatformStandbyMode_eOn,
                          {false,false,false,false,false,false,false,0}, false},
                         false};
#endif

void NEXUS_Platform_GetStandbySettings( NEXUS_PlatformStandbySettings *pSettings )
{
#if NEXUS_POWER_MANAGEMENT
   *pSettings = g_standbyState.settings;
#else
   BKNI_Memset(pSettings, 0, sizeof(*pSettings));
#endif
}

NEXUS_Error NEXUS_Platform_P_Standby(const NEXUS_PlatformStandbySettings *pSettings)
{
    NEXUS_Error errCode = NEXUS_SUCCESS;
    NEXUS_Platform_P_ModuleInfo *module_info;
    NEXUS_StandbySettings standby_settings;

    BDBG_CASSERT(sizeof(standby_settings) == sizeof(*pSettings));
    BKNI_Memcpy(&standby_settings, pSettings, sizeof(*pSettings));

    for (module_info = BLST_Q_FIRST(&g_NEXUS_platformHandles.handles); module_info; module_info = BLST_Q_NEXT(module_info, link)) {
        bool power_down=true, lock=true;

        if (pSettings->mode == NEXUS_PlatformStandbyMode_eActive && module_info->lock_mode == NEXUS_PlatformStandbyLockMode_ePassiveOnly) {
            power_down = lock = false;
        }
        if (module_info->lock_mode == NEXUS_PlatformStandbyLockMode_eNone) {
            lock = false;
        }

        /* We could transition from active to passive or vice versa */
        /* So modules could transition from standby -> resume and locked -> unlocked and vice versa */
        if (lock && !module_info->locked) {
            BDBG_MSG(("Disable module %s", NEXUS_Module_GetName(module_info->module)));
            NEXUS_Module_Disable(module_info->module);
            module_info->locked = true;
        }

        if (module_info->standby) {
            if (power_down && !module_info->powerdown) {
                BDBG_MSG(("Standby module %s", NEXUS_Module_GetName(module_info->module)));
                errCode = module_info->standby(power_down, &standby_settings);
                if (errCode) { errCode = BERR_TRACE(errCode);goto err; }
                    module_info->powerdown = true;
	    }
	    else if (!power_down && module_info->powerdown) {
                    BDBG_MSG(("Resume module %s", NEXUS_Module_GetName(module_info->module)));
                    errCode = module_info->standby(power_down, &standby_settings);
                    if (errCode) { errCode = BERR_TRACE(errCode);goto err; }
                    module_info->powerdown = false;
	    }
	}
        if (!lock && module_info->locked) {
            BDBG_MSG(("Enable module %s", NEXUS_Module_GetName(module_info->module)));
            NEXUS_Module_Enable(module_info->module);
            module_info->locked = false;
        }
    }

err:
    return errCode;
}

NEXUS_Error NEXUS_Platform_P_Resume(const NEXUS_PlatformStandbySettings *pSettings)
{
    NEXUS_Error errCode = NEXUS_SUCCESS;
    NEXUS_Platform_P_ModuleInfo *module_info;
    NEXUS_StandbySettings standby_settings;

    BDBG_CASSERT(sizeof(standby_settings) == sizeof(*pSettings));
    BKNI_Memcpy(&standby_settings, pSettings, sizeof(*pSettings));

    for (module_info = BLST_Q_LAST(&g_NEXUS_platformHandles.handles); module_info; module_info = BLST_Q_PREV(module_info, link)) {
        if (module_info->standby && module_info->powerdown) {
            BDBG_MSG(("Resume module %s", NEXUS_Module_GetName(module_info->module)));
            errCode = module_info->standby(false, &standby_settings);
            if (errCode) { errCode=BERR_TRACE(errCode);goto err; }
            module_info->powerdown = false;
        }
        if (module_info->locked) {
            BDBG_MSG(("Enable module %s", NEXUS_Module_GetName(module_info->module)));
            NEXUS_Module_Enable(module_info->module);
            module_info->locked = false;
        }
    }

err:
    return errCode;
}


NEXUS_Error NEXUS_Platform_P_SetStandbySettings( const NEXUS_PlatformStandbySettings *pSettings, bool resetWakeupStatus )
{
#if NEXUS_POWER_MANAGEMENT
    NEXUS_Error errCode = NEXUS_SUCCESS;

    BDBG_ASSERT(pSettings);

    if (g_NEXUS_platformModule == NULL ) {
        return NEXUS_NOT_INITIALIZED;
    }

    if(resetWakeupStatus) { 	
    NEXUS_Platform_P_ResetWakeupDevices(pSettings);
    }

    /* If state is unchanged; let app reconfigure wakeup up devices and exit */
    if (pSettings->mode == g_standbyState.settings.mode) {
        goto set_wakeup;
    }

    BDBG_MSG(("Entering Standby mode %d", pSettings->mode));

    BDBG_CASSERT(NEXUS_PlatformStandbyMode_eOn == (NEXUS_PlatformStandbyMode)NEXUS_StandbyMode_eOn);
    BDBG_CASSERT(NEXUS_PlatformStandbyMode_eActive == (NEXUS_PlatformStandbyMode)NEXUS_StandbyMode_eActive);
    BDBG_CASSERT(NEXUS_PlatformStandbyMode_ePassive == (NEXUS_PlatformStandbyMode)NEXUS_StandbyMode_ePassive);
    BDBG_CASSERT(NEXUS_PlatformStandbyMode_eDeepSleep == (NEXUS_PlatformStandbyMode)NEXUS_StandbyMode_eDeepSleep);

    if (g_standbyState.settings.mode == NEXUS_PlatformStandbyMode_eDeepSleep && pSettings->mode!= NEXUS_PlatformStandbyMode_eDeepSleep) {
        NEXUS_Platform_P_InitBoard();
        NEXUS_Platform_P_InitPinmux();
        NEXUS_Platform_P_InitVcxo();
    }

    /* First resume/unlock all modules so that S3 specific standby settings can be applied and frontend can be uninit */
    errCode = NEXUS_Platform_P_Resume(pSettings);
    if (errCode) { errCode = BERR_TRACE(errCode);goto err; }

    switch(pSettings->mode) {
    case NEXUS_PlatformStandbyMode_eDeepSleep:
    case NEXUS_PlatformStandbyMode_eActive:
    case NEXUS_PlatformStandbyMode_ePassive:
        errCode = NEXUS_Platform_P_Standby(pSettings);
        if (errCode) { errCode = BERR_TRACE(errCode);goto err; }
	break;

    case NEXUS_PlatformStandbyMode_eOn:
        break;

    case NEXUS_PlatformStandbyMode_eMax:
        BDBG_WRN(("Unknown Power State"));
        errCode = NEXUS_UNKNOWN ;
        break;
    }

set_wakeup:
    if (pSettings->mode == NEXUS_PlatformStandbyMode_ePassive || pSettings->mode == NEXUS_PlatformStandbyMode_eDeepSleep) {
        NEXUS_Platform_P_SetWakeupDevices(pSettings);
    }

    g_standbyState.settings = *pSettings;

err:
    /* If we fail while transtioning from On -> Standby; try to recover back to On mode
       Standby -> On mode failure is fatal and we cannot recover at this point. */
    if (errCode != NEXUS_SUCCESS) {
        if (g_standbyState.settings.mode == NEXUS_PlatformStandbyMode_eOn) {
#if defined(HUMAX_PLATFORM_BASE)
            /* errCode value shall not be overwritten here to return exact error status */
            NEXUS_Platform_P_Resume(&g_standbyState.settings);
#else
            errCode = NEXUS_Platform_P_Resume(&g_standbyState.settings);
#endif
        } else {
            BDBG_ERR(("*******************************************************************"));
            BDBG_ERR(("FATAL ERROR WHILE RESUMING! THE SYSTEM IS IN AN UNRECOVERABLE STATE"));
            BDBG_ERR(("*******************************************************************"));
        }
    }

    return errCode;

#else
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(resetWakeupStatus);
    return NEXUS_SUCCESS;
#endif
}

NEXUS_Error NEXUS_Platform_SetStandbySettings( const NEXUS_PlatformStandbySettings *pSettings )
{
    return NEXUS_Platform_P_SetStandbySettings(pSettings, true);
}

NEXUS_Error NEXUS_Platform_GetStandbyStatus(NEXUS_PlatformStandbyStatus *pStatus)
{
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

#if NEXUS_POWER_MANAGEMENT
    NEXUS_Platform_P_GetStandbyStatus(pStatus);
#endif

    return NEXUS_SUCCESS;
}


/* These apis have been deprecated but are maintained only for backward compatibility */

void NEXUS_Platform_GetDefaultStandbySettings( NEXUS_PlatformStandbySettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_Error NEXUS_Platform_InitStandby( const NEXUS_PlatformStandbySettings *pSettings )
{
#if NEXUS_POWER_MANAGEMENT
    NEXUS_PlatformSettings *pPlatformSettings;
    NEXUS_Error rc;

    /* Coverity: 35269 */
    pPlatformSettings = BKNI_Malloc(sizeof(NEXUS_PlatformSettings));
    if (!pPlatformSettings) {
        return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
    }
    NEXUS_Platform_GetDefaultSettings(pPlatformSettings);
    pPlatformSettings->standbySettings = *pSettings;
    rc = NEXUS_Platform_Init(pPlatformSettings);
    BKNI_Free(pPlatformSettings);
    if (rc) return BERR_TRACE(rc);
#else
    BSTD_UNUSED(pSettings);
#endif
    return 0;
}

void NEXUS_Platform_UninitStandby(void)
{
#if NEXUS_POWER_MANAGEMENT
    NEXUS_Platform_Uninit();
#endif
}

NEXUS_Error NEXUS_Platform_PreStandby(void)
{
    return 0;
}

NEXUS_Error NEXUS_Platform_PostStandby(void)
{
    return 0;
}

NEXUS_Error NEXUS_Platform_P_StandbyFrontend(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT && NEXUS_HAS_FRONTEND
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i;

    /* First init the frontend if required and then bring it out of standby */
    if (g_standbyState.settings.mode == NEXUS_PlatformStandbyMode_eDeepSleep && pSettings->mode != NEXUS_StandbyMode_eDeepSleep) {
        if (pSettings->openFrontend) {
	    NEXUS_Module_Enable(g_NEXUS_platformHandles.frontend);
            NEXUS_Platform_InitFrontend();
	    NEXUS_Module_Disable(g_NEXUS_platformHandles.frontend);
        } else {
	    return rc;
	}
    }

    /* This may need to go to platform specific frontend code */
    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if (pConfig->frontend[i]) {
             rc = NEXUS_Frontend_Standby_priv(pConfig->frontend[i], enabled, pSettings);
	     if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
	}
    }

    /* Power down frontend and then uninit it for S3 mode */
    if(pSettings->mode == NEXUS_StandbyMode_eDeepSleep) {
	NEXUS_Module_Enable(g_NEXUS_platformHandles.frontend);
	NEXUS_Platform_UninitFrontend();
	NEXUS_Module_Disable(g_NEXUS_platformHandles.frontend);
    }

error:
    return rc;
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
    return NEXUS_SUCCESS;
#endif
}
