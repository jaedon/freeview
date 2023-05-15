/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab_3472.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 12/5/12 10:39a $
 *
 * Module Description:
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/src/leapctfe/3472/bhab_3472.c $
 * 
 * Hydra_Software_Devel/5   12/5/12 10:39a atanugul
 * SW3462-52: Add Support Tuner Gain API
 * 
 * Hydra_Software_Devel/4   12/4/12 4:46p atanugul
 * SW3472-34: Add Support for GetVersionInfo() API
 * 
 * Hydra_Software_Devel/3   10/18/12 5:18p atanugul
 * SW3461-252: Add initializers for BHAB_GetInternalGain(),
 * BHAB_GetExternalGain() and BHAB_SetExternalGain()
 * 
 * Hydra_Software_Devel/2   9/28/12 1:39p atanugul
 * SW3128-228: Add support for BHAB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/1   7/9/12 5:13p atanugul
 * SW3472-8: Add PI Suport for 3472
 * 
 * 
 ***************************************************************************/
#include "bhab.h"
#include "bhab_priv.h"
#include "bhab_34xx_priv.h"

BDBG_MODULE(bhab_3472);


static const BHAB_Settings defDevSettings =
{	
    0x6c, /* chipAddr */
    NULL, /* interruptEnableFunc */
    NULL, /* interruptEnableFuncParam */
    /* API function table */
    {
        BHAB_34xx_Open,
        BHAB_34xx_Close,
        BHAB_34xx_InitAp, 
        BHAB_34xx_GetApStatus,
        BHAB_34xx_GetApVersion,
        BHAB_34xx_GetVersionInfo,       
        BHAB_34xx_ReadRegister,
        BHAB_34xx_WriteRegister,
        BHAB_34xx_ReadMemory,
        BHAB_34xx_WriteMemory,
        BHAB_34xx_ReadMbox,
        BHAB_34xx_WriteMbox,
        BHAB_34xx_HandleInterrupt_isr,
        BHAB_34xx_ProcessInterruptEvent,
        BHAB_34xx_EnableLockInterrupt, 
        BHAB_34xx_InstallInterruptCallback, 
        BHAB_34xx_UnInstallInterruptCallback,
        BHAB_34xx_SendHabCommand,
        BHAB_34xx_GetInterruptEventHandle, 
        BHAB_34xx_GetWatchDogTimer, 
        BHAB_34xx_SetWatchDogTimer,
        BHAB_34xx_GetNmiConfig,
        BHAB_34xx_SetNmiConfig,
        BHAB_34xx_GetConfigSettings,
        BHAB_34xx_SetConfigSettings,
        NULL, /* BHAB_ReadSlave */
        NULL, /* BHAB_WriteSlave */
        BHAB_34xx_GetInternalGain,        
        BHAB_34xx_GetExternalGain,        
        BHAB_34xx_SetExternalGain        
    },
    0x60, /* slaveChipAddr */
    false, /* isSpi */
    false /* isMtsif */      
};


/******************************************************************************
 BHAB_3472_GetDefaultSettings()
******************************************************************************/
BERR_Code BHAB_3472_GetDefaultSettings(
	BHAB_Settings *pDefSettings /* [out] default settings */
)
{
	*pDefSettings = defDevSettings;
	return BERR_SUCCESS;
}
