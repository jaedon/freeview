/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_7552.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/21/12 4:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/soc/7552/bads_7552.c $
 * 
 * Hydra_Software_Devel/2   9/21/12 4:09p farshidf
 * SW7552-360: Add BADS_3x7x_SetAcquireParams for 7552
 * 
 * Hydra_Software_Devel/1   6/22/12 12:46p farshidf
 * SW7425-2983: add 7552 ADS code
 * 
 * Hydra_Software_Devel/5   2/8/12 5:59p atanugul
 * SW3128-119: Add initializers for for SetAcquireParams, GetAcquireParams
 * and PowerUp
 * 
 * Hydra_Software_Devel/4   12/22/11 4:50p farshidf
 * SW7552-170: add scanstatus calls
 * 
 * Hydra_Software_Devel/3   12/8/11 3:31p atanugul
 * SW3128-89: Add Initializer for ADS_RequestSpectrumAnalyzerData  and
 * BADS_GetSpectrumAnalyzerData
 * 
 * Hydra_Software_Devel/2   10/10/11 3:30p farshidf
 * SW7552-134: fix compiling
 * 
 * Hydra_Software_Devel/1   10/10/11 2:05p farshidf
 * SW7552-134: sync up to V2_0 of 3461
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
#include "bads_7552.h"
#include "bads_3x7x_priv.h"


BDBG_MODULE(bads_7552);

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
    BADS_7552_Open()

****************************************************************************/
BERR_Code BADS_7552_GetDefaultSettings(
    BADS_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_7552_GetDefaultSettings);
    BSTD_UNUSED(hChip);

	BKNI_Memset(pDefSettings, 0, sizeof(*pDefSettings));

    pDefSettings->funcPtr.Open						= BADS_3x7x_Open;
	pDefSettings->funcPtr.Close						= BADS_3x7x_Close;
	pDefSettings->funcPtr.Init						= BADS_3x7x_Init;
	pDefSettings->funcPtr.GetVersion				= BADS_3x7x_GetVersion;
	pDefSettings->funcPtr.GetTotalChannels			= BADS_3x7x_GetTotalChannels;
	pDefSettings->funcPtr.OpenChannel				= BADS_3x7x_OpenChannel;
	pDefSettings->funcPtr.CloseChannel				= BADS_3x7x_CloseChannel;
	pDefSettings->funcPtr.GetDevice					= BADS_3x7x_GetDevice;
	pDefSettings->funcPtr.GetChannelDefaultSettings	= BADS_3x7x_GetChannelDefaultSettings;
	pDefSettings->funcPtr.GetStatus					= BADS_3x7x_GetStatus;
	pDefSettings->funcPtr.GetLockStatus				= BADS_3x7x_GetLockStatus;
	pDefSettings->funcPtr.GetSoftDecision			= BADS_3x7x_GetSoftDecision;
	pDefSettings->funcPtr.InstallCallback			= BADS_3x7x_InstallCallback;
	pDefSettings->funcPtr.Acquire					= BADS_3x7x_Acquire;
	pDefSettings->funcPtr.EnablePowerSaver			= BADS_3x7x_EnablePowerSaver;
	pDefSettings->funcPtr.DisablePowerSaver			= BADS_3x7x_DisablePowerSaver;
	pDefSettings->funcPtr.ProcessNotification		= BADS_3x7x_ProcessNotification;
	pDefSettings->funcPtr.SetDaisyChain				= BADS_3x7x_SetDaisyChain;
	pDefSettings->funcPtr.GetDaisyChain				= BADS_3x7x_GetDaisyChain;
	pDefSettings->funcPtr.ResetStatus				= BADS_3x7x_ResetStatus;
	pDefSettings->funcPtr.GetInterruptEventHandle	= BADS_3x7x_GetInterruptEventHandle;
	pDefSettings->funcPtr.ProcessInterruptEvent		= BADS_3x7x_ProcessInterruptEvent;
	pDefSettings->funcPtr.Untune					= BADS_3x7x_Untune;
	pDefSettings->funcPtr.GetScanStatus				= BADS_3x7x_GetScanStatus,	
    pDefSettings->funcPtr.SetScanParam				= BADS_3x7x_SetScanParam,
    pDefSettings->funcPtr.GetScanParam				= BADS_3x7x_GetScanParam,
	pDefSettings->funcPtr.SetAcquireParams			= BADS_3x7x_SetAcquireParams,
	pDefSettings->isOpenDrain						= false;
    pDefSettings->transportConfig					= BADS_TransportData_eGpioSerial; 


    BDBG_LEAVE(BADS_7552_GetDefaultSettings);
    return( retCode );
}

