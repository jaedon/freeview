/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab_4538.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 10/18/12 5:18p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/src/leapsatfe/4538/bhab_4538.c $
 * 
 * Hydra_Software_Devel/4   10/18/12 5:18p atanugul
 * SW3461-252: Add initializers for BHAB_GetInternalGain(),
 * BHAB_GetExternalGain() and BHAB_SetExternalGain()
 * 
 * Hydra_Software_Devel/3   10/15/12 2:11p enavarro
 * SWSATFE-220: implemented BHAB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/2   9/28/12 1:39p atanugul
 * SW3128-228: Add support for BHAB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/1   7/11/12 4:02p enavarro
 * SWSATFE-174: initial check-in
 *
 ***************************************************************************/
#include "bhab.h"
#include "bhab_priv.h"
#include "bhab_4538.h"
#include "bhab_4538_priv.h"

BDBG_MODULE(bhab_4538);


static const BHAB_Settings defDevSettings =
{   
    0x68, /* chipAddr */
    NULL, /* interruptEnableFunc */
    NULL, /* interruptEnableFuncParam */
    /* API function table */
    {
        BHAB_4538_P_Open,
        BHAB_4538_P_Close,
        BHAB_4538_P_InitAp, 
        BHAB_4538_P_GetApStatus,
        BHAB_4538_P_GetApVersion,
        BHAB_4538_P_GetVersionInfo,      
        BHAB_4538_P_ReadRegister,
        BHAB_4538_P_WriteRegister,
        BHAB_4538_P_ReadMemory,
        BHAB_4538_P_WriteMemory,
        NULL, /* BHAB_4538_P_ReadMbox */
        NULL, /* BHAB_4538_P_WriteMbox */
        BHAB_4538_P_HandleInterrupt_isr,
        BHAB_4538_P_ProcessInterruptEvent,
        NULL, /* BHAB_4538_P_EnableLockInterrupt */
        BHAB_4538_P_InstallInterruptCallback, 
        BHAB_4538_P_UnInstallInterruptCallback,
        BHAB_4538_P_SendHabCommand,
        BHAB_4538_P_GetInterruptEventHandle, 
        NULL, /* BHAB_4538_P_GetWatchDogTimer */
        NULL, /* BHAB_4538_P_SetWatchDogTimer */
        NULL, /* BHAB_4538_P_GetNmiConfig */
        NULL, /* BHAB_4538_P_SetNmiConfig */
        NULL, /* BHAB_4538_P_GetConfigSettings */
        NULL, /* BHAB_4538_P_SetConfigSettings */
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
 BHAB_4538_GetDefaultSettings()
******************************************************************************/
BERR_Code BHAB_4538_GetDefaultSettings(
   BHAB_Settings *pDefSettings /* [out] default settings */
)
{
   *pDefSettings = defDevSettings;
   return BERR_SUCCESS;
}
