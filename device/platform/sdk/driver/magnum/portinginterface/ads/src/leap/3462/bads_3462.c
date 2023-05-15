/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_3462.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 9/28/12 2:17p $
 *
 * Module Description:
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/leap/3462/bads_3462.c $
 * 
 * Hydra_Software_Devel/5   9/28/12 2:17p atanugul
 * SW3128-228: Add support for  BADS_GetVersionInfo()
 * 
 * Hydra_Software_Devel/4   6/21/12 4:25p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure; cleanup
 * 
 * Hydra_Software_Devel/3   6/21/12 3:21p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure; cleanup
 * 
 * Hydra_Software_Devel/2   6/18/12 6:11p atanugul
 * SW7425-2983: Update bads.inc based on the new ADS directory structure
 * 
 * Hydra_Software_Devel/1   4/10/12 4:06p atanugul
 * SW3462-4: Add ADS PI support for 3462
 * 
 ***************************************************************************/
#include "bhab.h"
#include "bads.h"
#include "bads_priv.h"
#include "bads_3462.h"
#include "../bads_leap_priv.h"

BDBG_MODULE(bads_3462);

/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BADS_Settings defDevSettings =
{
    BHAB_DevId_eADS0,       
    NULL,                   /* BHAB handle, must be provided by application*/
    {
        BADS_Leap_Open,
        BADS_Leap_Close,
        BADS_Leap_Init,
        BADS_Leap_GetVersion,
        NULL, /* BADS_GetVersionInfo */        
        NULL,
        BADS_Leap_GetTotalChannels,
        BADS_Leap_OpenChannel,
        BADS_Leap_CloseChannel,
        BADS_Leap_GetDevice,
        BADS_Leap_GetChannelDefaultSettings,
        BADS_Leap_GetStatus,
        BADS_Leap_GetLockStatus,
        BADS_Leap_GetSoftDecision,
        BADS_Leap_InstallCallback,
        BADS_Leap_GetDefaultAcquireParams,
        BADS_Leap_SetAcquireParams,
        BADS_Leap_GetAcquireParams,
        BADS_Leap_Acquire,
        BADS_Leap_EnablePowerSaver,
        BADS_Leap_DisablePowerSaver,
        BADS_Leap_ProcessNotification,
        BADS_Leap_SetDaisyChain,
        BADS_Leap_GetDaisyChain,
        BADS_Leap_ResetStatus,
        NULL,
        NULL,
        NULL,
        BADS_Leap_RequestAsyncStatus,
        BADS_Leap_GetAsyncStatus,
        BADS_Leap_GetScanStatus,        
        BADS_Leap_ReadSlave,
        BADS_Leap_WriteSlave,
        BADS_Leap_SetScanParam,
        BADS_Leap_GetScanParam,        
        BADS_Leap_RequestSpectrumAnalyzerData,        
        BADS_Leap_GetSpectrumAnalyzerData        
    },
    false,
    BADS_TransportData_eGpioSerial,
    NULL,
    NULL
};

/***************************************************************************
Summary:
    This function returns the default settings for Qam In-Band Downstream module.

Description:
    This function is responsible for returns the default setting for 
    BADS module. The returning default setting should be when
    opening the device.

Returns:
    TODO:

See Also:
    BADS_Leap_Open()

****************************************************************************/
BERR_Code BADS_3462_GetDefaultSettings(
    BADS_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_3462_GetDefaultSettings);
    BSTD_UNUSED(hChip);

    *pDefSettings = defDevSettings;

    BDBG_LEAVE(BADS_3462_GetDefaultSettings);
    return( retCode );
}
