/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_7550.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/22/12 12:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/soc/7550/bads_7550.c $
 * 
 * Hydra_Software_Devel/1   6/22/12 12:46p farshidf
 * SW7425-2983: add 7550 ADS code
 * 
 * Hydra_Software_Devel/11   2/8/12 5:59p atanugul
 * SW3128-119: Add initializers for for SetAcquireParams, GetAcquireParams
 * and PowerUp
 * 
 * Hydra_Software_Devel/10   12/8/11 3:31p atanugul
 * SW3128-89: Add Initializer for ADS_RequestSpectrumAnalyzerData  and
 * BADS_GetSpectrumAnalyzerData
 * 
 * Hydra_Software_Devel/9   9/15/11 3:29p atanugul
 * SW3128-50: Initializer fot BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/8   9/6/11 1:53p nickh
 * SW7425-1129: Add stub function
 * 
 * Hydra_Software_Devel/7   5/16/11 2:01p farshidf
 * SWDTV-6857: add the Scan calls
 * 
 * Hydra_Software_Devel/6   2/11/11 3:56p farshidf
 * SW7550-628: update teh function table
 * 
 * Hydra_Software_Devel/5   10/25/10 4:18p farshidf
 * SW7550-606: update the function table
 * 
 * Hydra_Software_Devel/4   8/30/10 3:42p farshidf
 * SW7550-543: merge to mian branch
 * 
 * Hydra_Software_Devel/SW7550-543/2   8/30/10 3:26p farshidf
 * SW7550-543: change the stopTimer to untune
 * 
 * Hydra_Software_Devel/SW7550-543/1   8/27/10 4:58p pinglong
 * SW7550-543: support channel switch between QAM and OFDM on bcm97590
 * 
 * Hydra_Software_Devel/3   5/18/10 11:50a farshidf
 * SW7550-440: merge to main branch
 * 
 * Hydra_Software_Devel/SW7550-440/1   5/18/10 11:47a farshidf
 * SW7550-440: add the dpm callback code
 * 
 * Hydra_Software_Devel/2   2/11/10 11:02a farshidf
 * SW7550-262: add the re-acquire feature for 7550 QAM code
 * 
 * Hydra_Software_Devel/1   11/18/09 6:03p farshidf
 * SW7550-38: 7550 inital ADS files
 *  
 ***************************************************************************/
#include "bads.h"
#include "bads_priv.h"
#include "bads_7550.h"
#include "bads_7550_api.h"

BDBG_MODULE(bads_7550);

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
BERR_Code BADS_7550_GetDefaultSettings(
    BADS_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_7550_GetDefaultSettings);
    BSTD_UNUSED(hChip);
    BKNI_Memset(pDefSettings, 0, sizeof(*pDefSettings));

    pDefSettings->funcPtr.Open						= BADS_Soc_Open;
	pDefSettings->funcPtr.Close						= BADS_Soc_Close;
	pDefSettings->funcPtr.Init						= BADS_Soc_Init;
	pDefSettings->funcPtr.GetVersion				= BADS_Soc_GetVersion;
	pDefSettings->funcPtr.GetTotalChannels			= BADS_Soc_GetTotalChannels;
	pDefSettings->funcPtr.OpenChannel				= BADS_Soc_OpenChannel;
	pDefSettings->funcPtr.CloseChannel				= BADS_Soc_CloseChannel;
	pDefSettings->funcPtr.GetDevice					= BADS_Soc_GetDevice;
	pDefSettings->funcPtr.GetChannelDefaultSettings	= BADS_Soc_GetChannelDefaultSettings;
	pDefSettings->funcPtr.GetStatus					= BADS_Soc_GetStatus;
	pDefSettings->funcPtr.GetLockStatus				= BADS_Soc_GetLockStatus;
	pDefSettings->funcPtr.GetSoftDecision			= BADS_Soc_GetSoftDecision;
	pDefSettings->funcPtr.InstallCallback			= BADS_Soc_InstallCallback;
	pDefSettings->funcPtr.Acquire					= BADS_Soc_Acquire;
	pDefSettings->funcPtr.EnablePowerSaver			= BADS_Soc_EnablePowerSaver;
	pDefSettings->funcPtr.ProcessNotification		= BADS_Soc_ProcessNotification;
	pDefSettings->funcPtr.SetDaisyChain				= BADS_Soc_SetDaisyChain;
	pDefSettings->funcPtr.GetDaisyChain				= BADS_Soc_GetDaisyChain;
	pDefSettings->funcPtr.ResetStatus				= BADS_Soc_ResetStatus;
	pDefSettings->funcPtr.GetInterruptEventHandle	= BADS_Soc_GetInterruptEventHandle;
	pDefSettings->funcPtr.ProcessInterruptEvent		= BADS_Soc_ProcessInterruptEvent;
	pDefSettings->funcPtr.Untune					= BADS_Soc_Untune;
	pDefSettings->isOpenDrain						= false;
    pDefSettings->transportConfig					= BADS_TransportData_eGpioSerial; 
 
    BDBG_LEAVE(BADS_7550_GetDefaultSettings);
    return( retCode );
}


/***************************************************************************
Summary:
    This function is called from the tuner PI.

Description:
    This funstion will allow the tuner to manage the power

Returns:
    TODO:

See Also:
    BADS_7550_ProcessTnrInterrupt_isr()

****************************************************************************/
BERR_Code BADS_7550_ProcessTnrInterrupt_isr(
	BADS_ChannelHandle hChannel,
	const BADS_7550_TnrInterruptData *pInterruptData
    )
{
	BERR_Code retCode = BERR_SUCCESS;

	if (pInterruptData->bResetDpm) 
	{
		BADS_Soc_P_ResetDPM(hChannel);
	}
	else
	{
		BADS_Soc_P_SetDPM(hChannel, pInterruptData->iOutdivMs);
	}
    return( retCode );
}


