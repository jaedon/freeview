/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_3109.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/28/12 2:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/8051/3109/bads_3109.c $
 * 
 * Hydra_Software_Devel/2   9/28/12 2:16p atanugul
 * SW3128-228: Add support for  BADS_GetVersionInfo()
 * 
 * Hydra_Software_Devel/1   6/21/12 3:34p vishk
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure
 * 
 * Hydra_Software_Devel/18   2/8/12 5:58p atanugul
 * SW3128-119: Add initializers for for SetAcquireParams, GetAcquireParams
 * and PowerUp
 * 
 * Hydra_Software_Devel/17   12/8/11 3:30p atanugul
 * SW3128-89: Add Initializer for ADS_RequestSpectrumAnalyzerData  and
 * BADS_GetSpectrumAnalyzerData
 * 
 * Hydra_Software_Devel/16   9/15/11 3:29p atanugul
 * SW3128-50: Initializer fot BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/15   8/31/11 7:23p nickh
 * SW7425-1129: Add stub function
 * 
 * Hydra_Software_Devel/11   5/16/11 2:01p farshidf
 * SWDTV-6857: add the Scan calls
 * 
 * Hydra_Software_Devel/10   11/24/10 7:58p agin
 * SW7405-5021:  Allow 7405 host to control LNA via 31xx tuner/demod
 * chips.
 * 
 * Hydra_Software_Devel/9   10/21/10 6:49p vishk
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/8   10/5/10 7:25p VISHK
 * SW7405-4915: Add nexus api to control power saver mode in downstream
 * demod and tuner, out of banddemod and tuner and upstream. Also add
 * nexus api to control daisy chaining of the frontends.
 * 
 * Hydra_Software_Devel/7   8/30/10 3:31p farshidf
 * SW7550-543: update the function table
 * 
 * Hydra_Software_Devel/6   2/11/10 11:09a farshidf
 * SW7550-262: make the API function compatible with 7550
 * 
 * Hydra_Software_Devel/5   11/24/09 4:48p farshidf
 * SW7550-38: set to NULL the new param for 7550
 * 
 * Hydra_Software_Devel/4   11/5/09 1:53p VISHK
 * SW7405-3210: BADS_31xx_GetStatus not returning values for
 * correctedCount, uncorrectedCount and berRawCount
 * 
 * Hydra_Software_Devel/3   8/25/09 2:49p vishk
 * SW7405-2839: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/2   3/10/09 3:50p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 *  
 ***************************************************************************/
#include "bhab.h"
#include "bads.h"
#include "bads_priv.h"
#include "bads_3109.h"
#include "../bads_31xx_priv.h"

BDBG_MODULE(bads_3109);

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
        BADS_31xx_Open,
        BADS_31xx_Close,
        BADS_31xx_Init,
        BADS_31xx_GetVersion,
        NULL, /* BADS_GetVersionInfo */
        NULL,
        BADS_31xx_GetTotalChannels,
        BADS_31xx_OpenChannel,
        BADS_31xx_CloseChannel,
        BADS_31xx_GetDevice,
        BADS_31xx_GetChannelDefaultSettings,
        BADS_31xx_GetStatus,
        BADS_31xx_GetLockStatus,
        BADS_31xx_GetSoftDecision,
        BADS_31xx_InstallCallback,
        NULL, /* BADS_GetDefaultAcquireParams */
        NULL, /* BADS_SetAcquireParams */
        NULL, /* BADS_GetAcquireParams */    
        BADS_31xx_Acquire,
        BADS_31xx_EnablePowerSaver,
        BADS_31xx_DisablePowerSaver,
        BADS_31xx_ProcessNotification,
        BADS_31xx_SetDaisyChain,
        BADS_31xx_GetDaisyChain,
        BADS_31xx_ResetStatus,
        NULL,
        NULL,
        NULL,
        BADS_31xx_RequestAsyncStatus,
        BADS_31xx_GetAsyncStatus,
        NULL, /* GetScanStatus */        
        BADS_31xx_ReadSlave,
        BADS_31xx_WriteSlave,
        NULL, /*SetScanParam*/
        NULL, /* GetScanParam*/
        NULL, /* BADS_RequestSpectrumAnalyzerData */
        NULL /* BADS_GetSpectrumAnalyzerData  */        
    },
    false,
    BADS_TransportData_eSerial,
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
    BADS_3109_Open()

****************************************************************************/
BERR_Code BADS_3109_GetDefaultSettings(
    BADS_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_3109_GetDefaultSettings);
    BSTD_UNUSED(hChip);

    *pDefSettings = defDevSettings;

    BDBG_LEAVE(BADS_3109_GetDefaultSettings);
    return( retCode );
}
