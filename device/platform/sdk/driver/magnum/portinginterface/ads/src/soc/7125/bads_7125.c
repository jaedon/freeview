/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_7125.c $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 9/28/12 2:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/soc/7125/bads_7125.c $
 * 
 * Hydra_Software_Devel/16   9/28/12 2:18p atanugul
 * SW3128-228: Add support for  BADS_GetVersionInfo()
 * 
 * Hydra_Software_Devel/15   2/8/12 5:59p atanugul
 * SW3128-119: Add initializers for for SetAcquireParams, GetAcquireParams
 * and PowerUp
 * 
 * Hydra_Software_Devel/14   12/8/11 3:31p atanugul
 * SW3128-89: Add Initializer for ADS_RequestSpectrumAnalyzerData  and
 * BADS_GetSpectrumAnalyzerData
 * 
 * Hydra_Software_Devel/13   9/15/11 3:30p atanugul
 * SW3128-50: Initializer fot BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/12   9/6/11 3:29p mward
 * SW7425-1129: Identify the stub function (placeholder)
 * 
 * Hydra_Software_Devel/11   9/6/11 1:52p nickh
 * SW7425-1129: Add stub function
 * 
 * Hydra_Software_Devel/10   6/7/11 4:28p mward
 * SW7125-926:  Add interrupt from tuner to request LNA gain.
 * 
 * Hydra_Software_Devel/SW7125-926/1   4/28/11 5:32p mward
 * SW7125-926:  Add interrupt from tuner to request LNA gain.
 * 
 * Hydra_Software_Devel/9   5/16/11 2:01p farshidf
 * SWDTV-6857: add the Scan calls
 * 
 * Hydra_Software_Devel/8   11/29/10 12:18p mward
 * SW7405-5021:  NULL-initialize pointers to functions which allow 7405
 * host to control LNA via 31xx tuner/demod chips.
 * 
 * Hydra_Software_Devel/7   10/27/10 3:31p mward
 * SW7420-1020:  Identify the unused,  NULL function pointers.
 * 
 * Hydra_Software_Devel/6   10/27/10 3:18p mward
 * SW7420-1020:  Init AsyncStatus function pointers to NULL, not supported
 * yet.
 * 
 * Hydra_Software_Devel/5   10/6/10 1:53p mward
 * SW7405-4915: Add nexus api to control power saver mode in downstream
 * demod and tuner, out of banddemod and tuner and upstream. Also add
 * nexus api to control daisy chaining of the frontends.
 * 
 * Hydra_Software_Devel/4   8/30/10 3:38p farshidf
 * SW7550-543: update the function table
 * 
 * Hydra_Software_Devel/3   7/22/10 2:42p mward
 * SW7125-513: Correct function description.
 * 
 * Hydra_Software_Devel/2   7/16/10 2:10p mward
 * SW7125-513: Add back last 7550 history to show origin version.
 * 
 * Hydra_Software_Devel/1   7/12/10 2:59p mward
 * SW7125-513:  Add host control of 7125 internal DS and tuner.  Copy and
 * adapt 7550 implementation.
 * 
 * Hydra_Software_Devel/3   5/18/10 11:50a farshidf
 * SW7550-440: merge to main branch
 *  
 ***************************************************************************/
#include "bads.h"
#include "bads_priv.h"
#include "bads_7125.h"
#include "bads_7125_priv.h"

BDBG_MODULE(bads_7125);

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
        BADS_7125_Open,
        BADS_7125_Close,
        BADS_7125_Init,
        BADS_7125_GetVersion,
        NULL, /* BADS_GetVersionInfo */        
        NULL, /* GetBondingCapability */
        BADS_7125_GetTotalChannels,
        BADS_7125_OpenChannel,
        BADS_7125_CloseChannel,
        BADS_7125_GetDevice,
        BADS_7125_GetChannelDefaultSettings,
        BADS_7125_GetStatus,
        BADS_7125_GetLockStatus,
        BADS_7125_GetSoftDecision,
        BADS_7125_InstallCallback,
        NULL, /* BADS_GetDefaultAcquireParams */
        NULL, /* BADS_SetAcquireParams */
        NULL, /* BADS_GetAcquireParams */        
        BADS_7125_Acquire,
        BADS_7125_EnablePowerSaver,
        BADS_7125_DisablePowerSaver,
        BADS_7125_ProcessNotification,
        BADS_7125_SetDaisyChain,
        BADS_7125_GetDaisyChain,
        BADS_7125_ResetStatus,
        BADS_7125_GetInterruptEventHandle,
        BADS_7125_ProcessInterruptEvent,
        NULL, /* Untune */
        NULL, /* RequestAsyncStatus */
        NULL,  /* GetAsyncStatus */
        NULL, /* GetScanStatus */        
        NULL, /* ReadSlave */
        NULL,  /* WriteSlave */
        NULL, /*SetScanParam*/
        NULL, /* GetScanParam*/
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
    BADS_7125_Open()

****************************************************************************/
BERR_Code BADS_7125_GetDefaultSettings(
    BADS_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BADS_7125_GetDefaultSettings);
    BSTD_UNUSED(hChip);

    *pDefSettings = defDevSettings;

    BDBG_LEAVE(BADS_7125_GetDefaultSettings);
    return( retCode );
}

/***************************************************************************
Summary:
    This function is called from the tuner PI.

Description:
    This function will allow the tuner to manage the power

Returns:
    TODO:

See Also:
    BADS_7125_ProcessTnrInterrupt_isr()

****************************************************************************/
BERR_Code BADS_7125_ProcessTnrInterrupt_isr(
	BADS_ChannelHandle hChannel,
	const BADS_7125_TnrInterruptData *pInterruptData
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;

	switch (pInterruptData->action) {
		case BADS_7125_ResetDpm:
			BADS_7125_P_ResetDPM(hChannel);
			break;
		case BADS_7125_SetDpm:
			BADS_7125_P_SetDPM(hChannel, pInterruptData->iOutdivMs);
			break;
		case BADS_7125_RequestLnaGain:
			if ( hImplDev->pCallback[chn][BADS_Callback_eUpdateGain] != NULL )
			{
					BDBG_MSG(("%s(): Calling BADS_Callback_eUpdateGain callback"));
					(hImplDev->pCallback[chn][BADS_Callback_eUpdateGain])(hImplDev->pCallbackParam[chn][BADS_Callback_eUpdateGain] );
			}
			break;
		default:
			retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
			break;
	}
    return( retCode );
}
