/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_3128.c $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 9/28/12 2:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/leap/3128/bads_3128.c $
 * 
 * Hydra_Software_Devel/14   9/28/12 2:17p atanugul
 * SW3128-228: Add support for  BADS_GetVersionInfo()
 * 
 * Hydra_Software_Devel/13   6/21/12 4:24p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure; cleanup
 * 
 * Hydra_Software_Devel/12   6/21/12 3:20p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure; cleanup
 * 
 * Hydra_Software_Devel/11   6/18/12 5:15p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS directory
 * structure
 * 
 * Hydra_Software_Devel/10   2/8/12 5:14p atanugul
 * SW3128-119: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams
 * 
 * Hydra_Software_Devel/9   12/8/11 3:13p atanugul
 * SW3128-89: Add PI support for Spectrum Analyzer Data retrieval
 * 
 * Hydra_Software_Devel/8   9/15/11 3:20p atanugul
 * SW3128-50: Add BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/7   8/31/11 5:59p nickh
 * SW7425-1129: Add stub function
 * 
 * Hydra_Software_Devel/6   8/30/11 11:18a atanugul
 * SW3128-47: Add Support for BCM3123
 * 
 * Hydra_Software_Devel/5   5/16/11 2:51p atanugul
 * SW3128-1: Add scan support to 3128
 * 
 * Hydra_Software_Devel/4   3/4/11 3:16p vishk
 * SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
 * board) plug on 7422 board
 * 
 * Hydra_Software_Devel/3   3/2/11 6:08p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/2   2/2/11 12:46p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/1   2/1/11 7:45a atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 *  
 ***************************************************************************/
#include "bhab.h"
#include "bads.h"
#include "bads_priv.h"
#include "bads_3128.h"
#include "../bads_leap_priv.h"

BDBG_MODULE(bads_3128);

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
        BADS_Leap_GetVersionInfo,
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
BERR_Code BADS_3128_GetDefaultSettings(
    BADS_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_3128_GetDefaultSettings);
    BSTD_UNUSED(hChip);

    *pDefSettings = defDevSettings;

    BDBG_LEAVE(BADS_3128_GetDefaultSettings);
    return( retCode );
}
