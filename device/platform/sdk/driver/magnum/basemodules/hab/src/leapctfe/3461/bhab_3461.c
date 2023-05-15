/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab_3461.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 10/18/12 5:15p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/src/leapctfe/3461/bhab_3461.c $
 * 
 * Hydra_Software_Devel/7   10/18/12 5:15p atanugul
 * SW3461-269: Merge to main
 * 
 * Hydra_Software_Devel/SW3461-252/1   9/28/12 5:49p atanugul
 * SW3461-269: Add support for BHAB_GetVersionInfo(); Add tnrApplication
 * and rfInputMode to BHAB_ConfigSettings; Add BHAB_GetInternalGain(),
 * BHAB_GetExternalGain() and BHAB_SetExternalGain() API
 * $brcm_Log: /magnum/basemodules/hab/src/leapctfe/3461/bhab_3461.c $
 * 
 * Hydra_Software_Devel/7   10/18/12 5:15p atanugul
 * SW3461-269: Merge to main
 * 
 * Hydra_Software_Devel/6   9/28/12 1:39p atanugul
 * SW3128-228: Add support for BHAB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/5   11/9/11 3:20p atanugul
 * SW3128-68: Add Initializer for BHAB_ReadSlave() and BHAB_ReadSlave()
 * 
 * Hydra_Software_Devel/4   5/27/11 4:28p atanugul
 * SW3461-3: Added BHAB_GetConfigSettings() and BHAB_SetConfigSettings()
 * 
 * Hydra_Software_Devel/3   5/11/11 4:07p atanugul
 * SW3461-3: Fix compile error
 * 
 * Hydra_Software_Devel/2   5/6/11 3:55p atanugul
 * SW3128-2: Missing initializers for isSpi and isMtsif.
 * 
 * Hydra_Software_Devel/1   3/16/11 3:32p atanugul
 * SW3461-3: Add support for BCM3461
 * 
 *
 ***************************************************************************/

#include "bhab.h"
#include "bhab_priv.h"
#include "bhab_3461_priv.h"

BDBG_MODULE(bhab_3461);


static const BHAB_Settings defDevSettings =
{	
    0x6c, /* chipAddr */
    NULL, /* interruptEnableFunc */
    NULL, /* interruptEnableFuncParam */
    /* API function table */
    {
        BHAB_3461_Open,
        BHAB_3461_Close,
        BHAB_3461_InitAp, 
        BHAB_3461_GetApStatus,
        BHAB_3461_GetApVersion,
        BHAB_3461_GetVersionInfo,
        BHAB_3461_ReadRegister,
        BHAB_3461_WriteRegister,
        BHAB_3461_ReadMemory,
        BHAB_3461_WriteMemory,
        BHAB_3461_ReadMbox,
        BHAB_3461_WriteMbox,
        BHAB_3461_HandleInterrupt_isr,
        BHAB_3461_ProcessInterruptEvent,
        BHAB_3461_EnableLockInterrupt, 
        BHAB_3461_InstallInterruptCallback, 
        BHAB_3461_UnInstallInterruptCallback,
        BHAB_3461_SendHabCommand,
        BHAB_3461_GetInterruptEventHandle, 
        BHAB_3461_GetWatchDogTimer, 
        BHAB_3461_SetWatchDogTimer,
        BHAB_3461_GetNmiConfig,
        BHAB_3461_SetNmiConfig,
        BHAB_3461_GetConfigSettings,
        BHAB_3461_SetConfigSettings,
        NULL, /* BHAB_ReadSlave */
        NULL, /* BHAB_WriteSlave */
        BHAB_3461_GetInternalGain,        
        BHAB_3461_GetExternalGain,        
        BHAB_3461_SetExternalGain        
    },
    0x60, /* slaveChipAddr */
    false, /* isSpi */
    false /* isMtsif */      
};


/******************************************************************************
 BHAB_3461_GetDefaultSettings()
******************************************************************************/
BERR_Code BHAB_3461_GetDefaultSettings(
	BHAB_Settings *pDefSettings /* [out] default settings */
)
{
	*pDefSettings = defDevSettings;
	return BERR_SUCCESS;
}
