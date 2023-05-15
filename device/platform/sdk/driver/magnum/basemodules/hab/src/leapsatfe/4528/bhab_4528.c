/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab_4528.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/18/12 5:17p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/src/leapsatfe/4528/bhab_4528.c $
 * 
 * Hydra_Software_Devel/5   10/18/12 5:17p atanugul
 * SW3461-252: Add initializers for BHAB_GetInternalGain(),
 * BHAB_GetExternalGain() and BHAB_SetExternalGain()
 * 
 * Hydra_Software_Devel/4   9/28/12 1:39p atanugul
 * SW3128-228: Add support for BHAB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/3   11/9/11 3:20p atanugul
 * SW3128-68: Add Initializer for BHAB_ReadSlave() and BHAB_ReadSlave()
 * 
 * Hydra_Software_Devel/2   11/3/11 10:28a enavarro
 * SWSATFE-86: changed GET_VERSION HAB command
 * 
 * Hydra_Software_Devel/1   8/11/11 4:05p enavarro
 * SW7425-883: initial version
 *
 ***************************************************************************/
#include "bhab.h"
#include "bhab_priv.h"
#include "bhab_4528.h"
#include "bhab_4528_priv.h"

BDBG_MODULE(bhab_4528);


static const BHAB_Settings defDevSettings =
{   
    0x68, /* chipAddr */
    NULL, /* interruptEnableFunc */
    NULL, /* interruptEnableFuncParam */
    /* API function table */
    {
        BHAB_4528_P_Open,
        BHAB_4528_P_Close,
        BHAB_4528_P_InitAp, 
        BHAB_4528_P_GetApStatus,
        BHAB_4528_P_GetApVersion,
        NULL, /* BHAB_GetVersionInfo */        
        BHAB_4528_P_ReadRegister,
        BHAB_4528_P_WriteRegister,
        BHAB_4528_P_ReadMemory,
        BHAB_4528_P_WriteMemory,
        NULL, /* BHAB_4528_P_ReadMbox */
        NULL, /* BHAB_4528_P_WriteMbox */
        BHAB_4528_P_HandleInterrupt_isr,
        BHAB_4528_P_ProcessInterruptEvent,
        NULL, /* BHAB_4528_P_EnableLockInterrupt */
        BHAB_4528_P_InstallInterruptCallback, 
        BHAB_4528_P_UnInstallInterruptCallback,
        BHAB_4528_P_SendHabCommand,
        BHAB_4528_P_GetInterruptEventHandle, 
        NULL, /* BHAB_4528_P_GetWatchDogTimer */
        NULL, /* BHAB_4528_P_SetWatchDogTimer */
        NULL, /* BHAB_4528_P_GetNmiConfig */
        NULL, /* BHAB_4528_P_SetNmiConfig */
        NULL, /* BHAB_4528_P_GetConfigSettings */
        NULL, /* BHAB_4528_P_SetConfigSettings */
        NULL, /* BHAB_ReadSlave */
        NULL, /* BHAB_WriteSlave */
        NULL, /* BHAB_GetInternalGain */
        NULL, /* BHAB_GetExternalGain */
        NULL  /* BHAB_SetExternalGain */        
    },
    0,     /* slaveChipAddr */
    false, /* isSpi */
    true   /* isMtsif */
};


/******************************************************************************
 BHAB_4528_GetDefaultSettings()
******************************************************************************/
BERR_Code BHAB_4528_GetDefaultSettings(
   BHAB_Settings *pDefSettings /* [out] default settings */
)
{
   *pDefSettings = defDevSettings;
   return BERR_SUCCESS;
}
