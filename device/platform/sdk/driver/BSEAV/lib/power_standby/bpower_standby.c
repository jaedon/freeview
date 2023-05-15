 /***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpower_standby.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 11/9/09 4:13p $
 *
 * Module Description:
 *    Main power standby module
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/power_standby/bpower_standby.c $
 * 
 * 7   11/9/09 4:13p erickson
 * SWLINUX-1418: remove redundant BKNI_Init/Uninit calls
 *
 * 6   6/16/09 8:00p maivu
 * PR 55564: NEXUS_Platform_Uninit now calls BKNI_Uninit. So we really
 * need to call BKNI_Init before using it. Note, PWR also use BKNI_xyz,
 * which is called from us here
 *
 * 5   4/30/09 1:16p maivu
 * PR 54724: Do not init/uninit BKNI. Leave it to Nexus or app to handle
 * this. Nexus does not uninit bkni.
 *
 * 4   2/11/09 10:40a maivu
 * PR 51963: Added support for generic GPIO wakeup
 *
 * 3   2/2/09 3:49p maivu
 * PR 50308: Don't execute BDBG_Init/Uninit, let apps call it
 *
 * 2   11/24/08 8:35p maivu
 * PR 49381, PR  49382: Fix coverity issue
 *
 * 1   10/29/08 4:00p maivu
 * PR 48036: power standby library, based off power_standby app in
 * rockford. Initial support for 3548/3556. Tested and confirmed working
 * on 93549B0
 *
 ******************************************************************/
#include "bpower_standby.h"
#include "bpower_priv.h"
#include "bpwr.h"
#include "berr.h"


BDBG_MODULE(bpower_standby);


void bpower_standby_get_default_settings( bpower_standby_settings *pSettings )
{
    pSettings->bPassive = true;
    pSettings->hours = 24;
    pSettings->minutes = 0;
    pSettings->seconds = 0;

}

bpower_standby_t bpower_standby_create(const bpower_standby_settings *pSettings )
{
    bpower_standby_t power_standby;

/* 3548/3556 only supports passive standby */
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
    if (!pSettings->bPassive) {
        return NULL;
    }
#endif

    power_standby = BKNI_Malloc(sizeof(*power_standby));
    if (power_standby == 0) {
        return NULL;
    }

    BKNI_Memset(power_standby, 0, sizeof(*power_standby));

    /* save settings */
    power_standby->settings = *pSettings;

    /* initialize standby, call PWR open */
    bpower_standby_p_init(power_standby);

    return power_standby;

}

void bpower_standby_destroy( bpower_standby_t power_standby )
{

    BPWR_Close(power_standby->hPwr);

    /* uninitialize modules open in bpower_standby_p_init */
    BREG_Close(power_standby->hReg);

    BKNI_Free(power_standby);
}

int bpower_standby_shutdown(bpower_standby_t power_standby)
{
    int eStatus;


    /* Call pmlib to power down Ethernet, USB, SATA */
    if (bpower_standby_p_os_powerdown(power_standby) != BPOWER_STANDBY_SUCCESS)
    {
        return BPOWER_STANDBY_FAILURE;
    }

    /* Now power down AV modules */
    if (power_standby->settings.bPassive) {
        BPWR_SetModes(power_standby->hPwr, power_standby->passiveModuleList,
                      power_standby->passiveModuleCnt, BPWR_Mode_ePowerDown);
    }
    else {
        BPWR_SetModes(power_standby->hPwr, power_standby->activeModuleList,
                      power_standby->activeModuleCnt, BPWR_Mode_ePowerDown);
    }

    if (BPWR_ApplyChanges(power_standby->hPwr) != BERR_SUCCESS) {
        return BPOWER_STANDBY_FAILURE;
    }

    eStatus = bpower_p_brcm_standby(power_standby);


    return eStatus;
}

int bpower_standby_resume(bpower_standby_t power_standby, bpower_standby_wakeup_settings *wakeup)
{
    BSTD_Module *reverse_modules= NULL;
    BSTD_Module *modules_to_reverse= NULL;
    uint32_t module_count = 0;
    uint32_t ix;


    if (power_standby->settings.bPassive)   {
        module_count =power_standby->passiveModuleCnt;
        reverse_modules = BKNI_Malloc(module_count * sizeof(BSTD_Module));
        modules_to_reverse = power_standby->passiveModuleList;
    } else {
        module_count = power_standby->activeModuleCnt;
        reverse_modules = BKNI_Malloc(module_count * sizeof(BSTD_Module));
        modules_to_reverse = power_standby->activeModuleList;
    }

    for (ix=0; ix < module_count; ix++) {
        reverse_modules[ix] = modules_to_reverse[module_count - ix - 1];
    }

    BPWR_SetModes(power_standby->hPwr, reverse_modules, module_count, BPWR_Mode_ePowerUp);

    if (BPWR_ApplyChanges(power_standby->hPwr) != BERR_SUCCESS) {
        BKNI_Free(reverse_modules);
        return BPOWER_STANDBY_FAILURE;
    }

    if (bpower_standby_p_os_powerup(power_standby) != BPOWER_STANDBY_SUCCESS) {
        BKNI_Free(reverse_modules);
        return BPOWER_STANDBY_FAILURE;
    }

    *wakeup = power_standby->wakeup;

    BKNI_Free(reverse_modules);
    return BPOWER_STANDBY_SUCCESS;

}

