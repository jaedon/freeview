/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_3461.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 9/28/12 2:17p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/leap/3461/bads_3461.c $
 * 
 * Hydra_Software_Devel/12   9/28/12 2:17p atanugul
 * SW3128-228: Add support for  BADS_GetVersionInfo()
 * 
 * Hydra_Software_Devel/11   6/21/12 4:24p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure; cleanup
 * 
 * Hydra_Software_Devel/10   6/21/12 3:21p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure; cleanup
 * 
 * Hydra_Software_Devel/9   6/18/12 6:11p atanugul
 * SW7425-2983: Update bads.inc based on the new ADS directory structure
 * 
 * Hydra_Software_Devel/8   2/8/12 5:59p atanugul
 * SW3128-119: Add initializers for for SetAcquireParams, GetAcquireParams
 * and PowerUp
 * 
 * Hydra_Software_Devel/7   12/8/11 3:31p atanugul
 * SW3128-89: Add Initializer for ADS_RequestSpectrumAnalyzerData  and
 * BADS_GetSpectrumAnalyzerData
 * 
 * Hydra_Software_Devel/6   10/5/11 4:40p atanugul
 * SW3461-3: Add BADS_Leap_GetScanStatus(); update
 * BADS_Leap_GetAsyncStatus() and BADS_Leap_GetScanParam() based on 3128
 * ADS PI
 * 
 * Hydra_Software_Devel/5   9/15/11 3:30p atanugul
 * SW3128-50: Initializer fot BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/4   9/6/11 1:51p nickh
 * SW7425-1129: Add stub function
 * 
 * Hydra_Software_Devel/3   6/23/11 3:19p atanugul
 * SW3461-3: Add ADS Scan support for 3461; code cleanup
 * 
 * Hydra_Software_Devel/2   5/26/11 6:05p atanugul
 * SW3461-3: Missing initializer for Get/Set ScanParam
 * 
 * Hydra_Software_Devel/1   3/16/11 3:37p atanugul
 * SW3461-3: Add support for BCM3461
 * 
 *
 ***************************************************************************/
#include "bhab.h"
#include "bads.h"
#include "bads_priv.h"
#include "bads_3461.h"
#include "../bads_leap_priv.h"

BDBG_MODULE(bads_3461);

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
        NULL, /* BADS_SetAcquireParams */
        NULL, /* BADS_GetAcquireParams */       
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
        NULL, /* BADS_RequestSpectrumAnalyzerData */
        NULL /* BADS_GetSpectrumAnalyzerData  */        
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
BERR_Code BADS_3461_GetDefaultSettings(
    BADS_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_3461_GetDefaultSettings);
    BSTD_UNUSED(hChip);

    *pDefSettings = defDevSettings;

    BDBG_LEAVE(BADS_3461_GetDefaultSettings);
    return( retCode );
}
