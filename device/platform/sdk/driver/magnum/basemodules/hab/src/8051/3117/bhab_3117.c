/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab_3117.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/18/12 5:18p $
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/src/8051/3117/bhab_3117.c $
 * 
 * Hydra_Software_Devel/3   10/18/12 5:18p atanugul
 * SW3461-252: Add initializers for BHAB_GetInternalGain(),
 * BHAB_GetExternalGain() and BHAB_SetExternalGain()
 * 
 * Hydra_Software_Devel/2   9/28/12 1:38p atanugul
 * SW3128-228: Add support for BHAB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/10   11/9/11 3:19p atanugul
 * SW3128-68: Add Initializer for BHAB_ReadSlave() and BHAB_ReadSlave()
 * 
 * Hydra_Software_Devel/9   5/27/11 4:41p atanugul
 * SW3461-3: Initializer for BHAB_3461_GetConfigSettings and
 * BHAB_3461_SetConfigSettings
 * 
 * Hydra_Software_Devel/8   5/6/11 3:49p atanugul
 * SW3128-2: Added support to configure TM_MTSIF_CTRL - Transport Control
 * Register for BCM3128
 * 
 * Hydra_Software_Devel/7   5/2/11 6:34p mward
 * SW3128-8: missing initializer for isSpi.
 * 
 * Hydra_Software_Devel/6   12/2/10 7:31p VISHK
 * SW7405-5037: To make the i2c address programmable for the slave
 * device(LNA) of 31xx frontend chip.
 * 
 * Hydra_Software_Devel/5   10/5/09 10:46a vishk
 * SW7405-2851: 31xx: Add support to configure the Watchdog feature of
 * 31xx.
 * 
 * Hydra_Software_Devel/4   3/10/09 1:45p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/3   1/29/09 7:14p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Removed GetLockStateChangeEventHandle.
 * 
 * Hydra_Software_Devel/2   1/16/09 7:20p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/1   10/20/08 2:51p gmohile
 * PR 47386 : Checkin initial version
 * 
 ***************************************************************************/

#include "bhab.h"
#include "bhab_priv.h"
#include "bhab_3117.h"
#include "bhab_31xx_priv.h"

BDBG_MODULE(bhab_3117);


static const BHAB_Settings defDevSettings =
  {	
    0x66, /* chipAddr */
    NULL, /* interruptEnableFunc */
    NULL, /* interruptEnableFuncParam */
    /* API function table */
    {
        BHAB_31xx_Open,
        BHAB_31xx_Close,
        BHAB_31xx_InitAp, 
        BHAB_31xx_GetApStatus,
        BHAB_31xx_GetApVersion,
        NULL, /* BHAB_GetVersionInfo */        
        BHAB_31xx_ReadRegister,
        BHAB_31xx_WriteRegister,
        BHAB_31xx_ReadMemory,
        BHAB_31xx_WriteMemory,
        BHAB_31xx_ReadMbox,
        BHAB_31xx_WriteMbox,
        BHAB_31xx_HandleInterrupt_isr,
        BHAB_31xx_ProcessInterruptEvent,
        BHAB_31xx_EnableLockInterrupt, 
        BHAB_31xx_InstallInterruptCallback, 
        BHAB_31xx_UnInstallInterruptCallback,
        BHAB_31xx_SendHabCommand,
        BHAB_31xx_GetInterruptEventHandle, 
        BHAB_31xx_GetWatchDogTimer, 
        BHAB_31xx_SetWatchDogTimer,
        BHAB_31xx_GetNmiConfig,
        BHAB_31xx_SetNmiConfig,
        NULL, /* BHAB_GetConfigSettings */
        NULL, /* BHAB_SetConfigSettings */
        NULL, /* BHAB_ReadSlave */
        NULL, /* BHAB_WriteSlave */
        NULL, /* BHAB_GetInternalGain */
        NULL, /* BHAB_GetExternalGain */
        NULL  /* BHAB_SetExternalGain */
    },
    0x60, /* slaveChipAddr */
    false, /* isSpi */
    false /* isMtsif */
};


/******************************************************************************
 BHAB_3117_GetDefaultSettings()
******************************************************************************/
BERR_Code BHAB_3117_GetDefaultSettings(
	BHAB_Settings *pDefSettings /* [out] default settings */
)
{
	*pDefSettings = defDevSettings;
	return BERR_SUCCESS;
}
