/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab_3128.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 10/18/12 5:17p $
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/src/leapctfe/3128/bhab_3128.c $
 * 
 * Hydra_Software_Devel/11   10/18/12 5:17p atanugul
 * SW3461-252: Add initializers for BHAB_GetInternalGain(),
 * BHAB_GetExternalGain() and BHAB_SetExternalGain()
 * 
 * Hydra_Software_Devel/10   9/28/12 1:38p atanugul
 * SW3128-228: Add support for BHAB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/9   11/9/11 2:58p atanugul
 * SW3128-68: Add BHAB_ReadSlave() and BHAB_ReadSlave() API to 3128
 * 
 * Hydra_Software_Devel/8   11/8/11 5:06p atanugul
 * SW3128-64: Add BHAB_312x_GetConfigSettings BHAB_312x_SetConfigSettings
 * support for 312x
 * 
 * Hydra_Software_Devel/7   8/30/11 12:09p atanugul
 * SW3128-47: Add Support for BCM3123
 * 
 * Hydra_Software_Devel/6   5/27/11 4:42p atanugul
 * SW3461-3: Initializer for BHAB_3461_GetConfigSettings and
 * BHAB_3461_SetConfigSettings
 * 
 * Hydra_Software_Devel/5   5/6/11 3:49p atanugul
 * SW3128-2: Added support to configure TM_MTSIF_CTRL - Transport Control
 * Register for BCM3128
 * 
 * Hydra_Software_Devel/4   4/29/11 10:16a atanugul
 * SW3128-8: Add SPI support for 3128
 * 
 * Hydra_Software_Devel/3   3/4/11 3:12p vishk
 * SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
 * board) plug on 7422 board
 * 
 * Hydra_Software_Devel/2   2/2/11 12:46p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/1   2/1/11 10:44a atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 ***************************************************************************/

#include "bhab.h"
#include "bhab_priv.h"
#include "bhab_3128.h"
#include "bhab_312x_priv.h"

BDBG_MODULE(bhab_3128);


static const BHAB_Settings defDevSettings =
{	
    0x66, /* chipAddr */
    NULL, /* interruptEnableFunc */
    NULL, /* interruptEnableFuncParam */
    /* API function table */
    {
        BHAB_312x_Open,
        BHAB_312x_Close,
        BHAB_312x_InitAp, 
        BHAB_312x_GetApStatus,
        BHAB_312x_GetApVersion,
        BHAB_312x_GetVersionInfo,        
        BHAB_312x_ReadRegister,
        BHAB_312x_WriteRegister,
        BHAB_312x_ReadMemory,
        BHAB_312x_WriteMemory,
        BHAB_312x_ReadMbox,
        BHAB_312x_WriteMbox,
        BHAB_312x_HandleInterrupt_isr,
        BHAB_312x_ProcessInterruptEvent,
        BHAB_312x_EnableLockInterrupt, 
        BHAB_312x_InstallInterruptCallback, 
        BHAB_312x_UnInstallInterruptCallback,
        BHAB_312x_SendHabCommand,
        BHAB_312x_GetInterruptEventHandle, 
        BHAB_312x_GetWatchDogTimer, 
        BHAB_312x_SetWatchDogTimer,
        BHAB_312x_GetNmiConfig,
        BHAB_312x_SetNmiConfig,
        BHAB_312x_GetConfigSettings,
        BHAB_312x_SetConfigSettings,
        BHAB_312x_ReadSlave,
        BHAB_312x_WriteSlave,
        NULL, /* BHAB_GetInternalGain */
        NULL, /* BHAB_GetExternalGain */
        NULL  /* BHAB_SetExternalGain */
    },
    0x60, /* slaveChipAddr */
    false, /* isSpi */
    false /* isMtsif */
};


/******************************************************************************
 BHAB_3128_GetDefaultSettings()
******************************************************************************/
BERR_Code BHAB_3128_GetDefaultSettings(
	BHAB_Settings *pDefSettings /* [out] default settings */
)
{
	*pDefSettings = defDevSettings;
	return BERR_SUCCESS;
}
