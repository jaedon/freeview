/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_3x7x.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 2/8/12 5:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/35233/bads_3x7x.c $
 * 
 * Hydra_Software_Devel/8   2/8/12 5:59p atanugul
 * SW3128-119: Add initializers for for SetAcquireParams, GetAcquireParams
 * and PowerUp
 * 
 * Hydra_Software_Devel/7   12/8/11 3:31p atanugul
 * SW3128-89: Add Initializer for ADS_RequestSpectrumAnalyzerData  and
 * BADS_GetSpectrumAnalyzerData
 * 
 * Hydra_Software_Devel/6   9/15/11 3:30p atanugul
 * SW3128-50: Initializer fot BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/5   9/6/11 1:54p nickh
 * SW7425-1129: Add stub function
 * 
 * Hydra_Software_Devel/4   5/31/11 3:55p farshidf
 * SW7552-36: add disable power saver
 * 
 * Hydra_Software_Devel/3   5/6/11 2:24p farshidf
 * SWDTV-6857: merge main
 * 
 * Hydra_Software_Devel/SWDTV-6857/1   5/6/11 2:03p farshidf
 * SWDTV-6587: add scan structure and calls
 * 
 * Hydra_Software_Devel/2   4/27/11 1:11a farshidf
 * SWDTV-6190: compile fix
 * 
 * Hydra_Software_Devel/1   4/20/11 12:38p farshidf
 * SWDTV-6190: add intial DS files
 *  
 ***************************************************************************/
#include "bads.h"
#include "bads_priv.h"
#include "bads_3x7x.h"
#include "bads_api.h"


BDBG_MODULE(bads_3x7x);

/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BADS_Settings defDevSettings =
{
    0,       
    NULL,                   /* BHAB handle, must be provided by application*/
    {
        BADS_3x7x_Open,
        BADS_3x7x_Close,
        BADS_3x7x_Init,
        BADS_3x7x_GetVersion,
        NULL,	
        BADS_3x7x_GetTotalChannels,
        BADS_3x7x_OpenChannel,
        BADS_3x7x_CloseChannel,
        BADS_3x7x_GetDevice,
        BADS_3x7x_GetChannelDefaultSettings,
        BADS_3x7x_GetStatus,
        BADS_3x7x_GetLockStatus,
        BADS_3x7x_GetSoftDecision,
        BADS_3x7x_InstallCallback,
        NULL, /* BADS_GetDefaultAcquireParams */
        NULL, /* BADS_SetAcquireParams */
        NULL, /* BADS_GetAcquireParams */        
        BADS_3x7x_Acquire,
        BADS_3x7x_EnablePowerSaver,
        BADS_3x7x_DisablePowerSaver,
        BADS_3x7x_ProcessNotification,
        BADS_3x7x_SetDaisyChain,
        BADS_3x7x_GetDaisyChain,
        BADS_3x7x_ResetStatus,
        BADS_3x7x_GetInterruptEventHandle,
        BADS_3x7x_ProcessInterruptEvent,
        BADS_3x7x_Untune,
        NULL,
        NULL,
        NULL, /* GetScanStatus */        
        NULL,
        NULL,
        BADS_3x7x_SetScanParam,
        BADS_3x7x_GetScanParam,
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
    BADS_7550_Open()

****************************************************************************/
BERR_Code BADS_3x7x_GetDefaultSettings(
    BADS_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_7550_GetDefaultSettings);
    BSTD_UNUSED(hChip);

    *pDefSettings = defDevSettings;

    BDBG_LEAVE(BADS_7550_GetDefaultSettings);
    return( retCode );
}

