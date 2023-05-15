/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_frontend_7125_qam.c $
* $brcm_Revision: 21 $
* $brcm_Date: 11/17/11 12:08p $
*
* API Description:
*   API name: Frontend 7125 QAM
*    APIs to open, close, and setup initial settings for QAM demodulators on a
*    BCM7125 Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/7125/src/nexus_frontend_7125_qam.c $
* 
* 21   11/17/11 12:08p mward
* SW3461-79: Add No Signal to GetLockStatus()
* 
* 20   7/11/11 5:42p mward
* SW7125-1022:  Parameter check protection.
* 
* 19   7/11/11 3:49p mward
* SW7125-968:  Tuner powers on shared components, so power on tuner
*  before ADS.
* 
* 18   7/6/11 6:31p mward
* SW7125-1016:  Return NEXUS_Error values.
* 
* 17   7/6/11 6:19p mward
* SW7125-891:  Add Viterbi status values as done for 31xx.
* 
* 16   7/6/11 6:15p mward
* SW7125-1017:  Use accumulated counters.
* 
* 15   6/2/11 2:30p mward
* SW7125-513:  Improper use of BDBG_CASSERT().
* 
* 14   5/31/11 4:29p mward
* SW7125-513:  Fix change detection in
*  NEXUS_Frontend_7125_SetConfigSettings().
* 
* 13   5/27/11 4:09p mward
* SW7125-926:  Use NEXUS_Amplifier for LNA, rather than direct control in
*  tnr/7125.
* 
* SW7125-926/4   5/27/11 1:24p mward
* SW7125-513:  Correct scaling of status.dsChannelPower.
* 
* SW7125-926/3   5/24/11 12:21p mward
* SW7125-513:  Add omitted lnaAgcLevel to status.
* 
* SW7125-926/2   4/29/11 4:38p mward
* SW7125-926:  Encode gain/boost into configSettings.lnaGain.
* 
* SW7125-926/1   4/28/11 5:29p mward
* SW7125-926:  Add update (LNA) gain callback, LNA gain configuration
*  setting for 7125 tuner/DS.
* 
* 12   4/26/11 6:10p mward
* SW7125-905:  BADS_DisablePowerSaver() before tune / acquire.
* 
* 11   12/2/10 5:39p mward
* SW7125-728:  Add Annex C QAM64 and QAM256 support same as Annex A
*  except Nyquist alpha = 12% instead of 15%.
* 
* 10   10/22/10 5:09p mward
* SW7125-497:  Use special symbolRate values to turn on and off status
*  timer for control handoff with DOCSIS.
* 
* 9   10/18/10 10:35a mward
* SW7125-513:  Declare unused to prevent compiler warning.
* 
* 8   10/12/10 6:43p mward
* SW7125-513:  Add an untune (power down) function.
* 
* 7   8/24/10 3:46p mward
* SW7125-513: SW7405-4808:  BADS_ResetStatus() now takes a
*  BADS_ChannelHandle instead of a BADS_Handle.
* 
* 6   8/19/10 11:52a mward
* SW7125-513:  Uninstall callbacks when closing.
* 
* 5   7/30/10 10:49a mward
* SW7125-513:  Fixed lost event and re-init failure.
* 
* 4   7/27/10 1:53p mward
* SW7125-513: Merge to main branch:  Simplify names.
* 
* 3   7/23/10 6:57p mward
* SW7125-513:  Fixes for dual-tuner dual-channel.
* 
* 2   7/22/10 3:15p mward
* SW7125-513: Fixes for dual-channel.
* 
* 1   7/12/10 3:29p mward
* SW7125-513:  Add host control of 7125 internal DS and tuner.  Copy and
*  adapt 7550 implementation.
* 
***************************************************************************/

#include "nexus_frontend_module.h"
#include "nexus_frontend_7125_priv.h"
#include "bads.h"
#include "bads_priv.h"
#include "bads_7125_priv.h"

BDBG_MODULE(nexus_frontend_7125_qam);

void NEXUS_Frontend_GetDefault7125Settings(
    NEXUS_7125FrontendSettings *pSettings   /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

#include "bads.h"
#include "bads_7125.h"

BDBG_OBJECT_ID(NEXUS_7125Qam);

#define NEXUS_MAX_7125_ADSCHN 2

#define NEXUS_FRONTEND_P_7125_ADS_INTERRUPT 0

typedef struct NEXUS_7125Qam
{
    BDBG_OBJECT(NEXUS_7125Qam)
    BKNI_EventHandle isrEvent;
    NEXUS_EventCallbackHandle isrEventCallback;
    BADS_Handle adsHandle;
    unsigned    numfrontends;
    NEXUS_FrontendHandle frontendHandle[NEXUS_MAX_7125_ADSCHN];
    NEXUS_IsrCallbackHandle lockAppCallback[NEXUS_MAX_7125_ADSCHN];
    NEXUS_IsrCallbackHandle updateGainAppCallback[NEXUS_MAX_7125_ADSCHN];
    bool lockStatus[NEXUS_MAX_7125_ADSCHN];
    NEXUS_Time lastStatusTime;
    BADS_ChannelHandle  ads_chn[NEXUS_MAX_7125_ADSCHN];
    NEXUS_FrontendQamSettings   last_ads[NEXUS_MAX_7125_ADSCHN];
	NEXUS_TunerHandle tuner[NEXUS_MAX_7125_ADSCHN];
    NEXUS_7125TuneSettings retuneSettings[NEXUS_MAX_7125_ADSCHN];
	unsigned lnaAgcValue;		/* LNA status from NEXUS_AmplifierStatus */
	bool lnaGainBoostEnabled;
	bool lnaSuperBoostEnabled;
	bool lnaTiltEnabled;
	NEXUS_CallbackDesc updateGainCallbackDesc; 	/* Callback will be called when the gain from the lna needs to be updated. */
} NEXUS_7125Qam;

typedef struct NEXUS_7125Channel
{
    unsigned chn_num;       /* channel number */
    NEXUS_7125Qam *pDevice; /* 7125 device*/
} NEXUS_7125Channel;

static NEXUS_7125Qam *pDevice;

static void NEXUS_Frontend_P_7125_TnrAdsInterrupt_isr(void *pParam1, int param2, const BTNR_7125_AdsInterruptData *pData);

/***************************************************************************
Summary:
    Lock callback handler for a 7125  Inband device
 ***************************************************************************/
static void NEXUS_Frontend_P_7125_callback_isr(void *pParam)
{
    NEXUS_IsrCallbackHandle callback = (NEXUS_IsrCallbackHandle)pParam;

     if ( callback )
    {
        NEXUS_IsrCallback_Fire_isr(callback);
    }
}

#if NEXUS_FRONTEND_P_7125_ADS_INTERRUPT
/***************************************************************************
Summary:
    ISR for a 7125 device
 ***************************************************************************/
static void NEXUS_Frontend_P_7125_L1_isr(void *pParam1, int pParam2)
{
    BSTD_UNUSED(pParam2);

    BADS_7125_HandleInterrupt_isr(pParam1);
}
#endif

/***************************************************************************
Summary:
    ISR Event Handler for a 7125 device
 ***************************************************************************/
static void NEXUS_Frontend_P_7125_IsrEvent(void *pParam)
{
    BADS_7125_Handle hImplDev;
    BADS_ChannelHandle hChnDev;
    BADS_LockStatus lockstatus;
    NEXUS_7125Qam *pDevice = pParam;
    BADS_Handle hDev;
	unsigned i;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_7125Qam);
	hDev = pDevice->adsHandle;
	hImplDev = (BADS_7125_Handle) hDev->pImpl;

	for (i=0;i<pDevice->numfrontends;i++)
	{
		hChnDev = (BADS_ChannelHandle)(hImplDev->hAdsChn[i]);
		if ((pDevice->tuner[i]) && (pDevice->retuneSettings[i].frequency))
		{
			BDBG_MSG(("Calling BADS_ProcessInterruptEvent"));
			BADS_7125_GetLockStatus(hChnDev, &lockstatus);
			if (lockstatus != BADS_LockStatus_eLocked)
				NEXUS_Tuner_Tune7125(pDevice->tuner[i], &(pDevice->retuneSettings[i]));
			BADS_ProcessInterruptEvent(pDevice->adsHandle);
		}
	}
}

/***************************************************************************
 * Module callback functions for tuning
 ***************************************************************************/
static void NEXUS_Frontend_P_7125_CloseQam(NEXUS_FrontendHandle handle);
static NEXUS_Error NEXUS_Frontend_P_7125_TuneQam(void *handle, const NEXUS_FrontendQamSettings *pSettings);
static NEXUS_Error NEXUS_Frontend_P_7125_GetQamStatus(void *handle, NEXUS_FrontendQamStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_7125_GetSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length);
static void NEXUS_Frontend_P_7125_UntuneQam(void *handle);
static void NEXUS_Frontend_P_7125_ResetQamStatus(void *handle);


NEXUS_FrontendHandle NEXUS_Frontend_Open7125(  /* attr{destructor=NEXUS_Frontend_Close} */
    const NEXUS_7125FrontendSettings *pSettings
    )
{
    BERR_Code errCode;
    BADS_Settings adsSettings;
    BADS_ChannelSettings chnSettings;
    unsigned int i, num_ch, chn_num;
    NEXUS_FrontendHandle frontendHandle;
    NEXUS_7125Channel *channelHandle;
    NEXUS_IsrCallbackHandle callback;
	NEXUS_IsrCallbackHandle updateGainCallback  = NULL;

    BDBG_ASSERT(NULL != pSettings);
    chn_num = pSettings->channelNumber;

    if ( chn_num >= NEXUS_MAX_7125_ADSCHN)
    {
        BDBG_ERR((" channel number exceeds supported channels."));
        goto err_malloc;
    }

    if(pDevice == NULL ) {
        pDevice = BKNI_Malloc(sizeof(NEXUS_7125Qam));
        if ( NULL == pDevice )
        {
            errCode = BERR_TRACE(BERR_OS_ERROR);
            goto err_malloc;
        }

		errCode = BADS_7125_GetDefaultSettings(&adsSettings, NULL);
		if ( errCode != BERR_SUCCESS ) goto err_ads;
	
		BKNI_Memset(pDevice, 0, sizeof(NEXUS_7125Qam));
		BDBG_OBJECT_SET(pDevice, NEXUS_7125Qam);
	
		adsSettings.hTmr = g_pCoreHandles->tmr;
		adsSettings.hHeap = g_pCoreHandles->heap[0]; /* select default heap. extend NEXUS_7125FrontendSettings if other heap index needed. */
	
		/* Open ADS device */
		errCode = BADS_Open(&pDevice->adsHandle,
							g_pCoreHandles->chp,
							(void*)g_pCoreHandles->reg,
							g_pCoreHandles->bint,
							&adsSettings);
		if ( errCode != BERR_SUCCESS )
		{
			errCode = BERR_TRACE(errCode);
			goto err_ads;
		}
	
		BDBG_WRN(("Initializating 7125 QAM core..."));
		errCode = BADS_Init(pDevice->adsHandle);
		if ( errCode != BERR_SUCCESS )
		{
			errCode = BERR_TRACE(errCode);
			goto err_init;
		}
		BDBG_WRN(("Initializating 7125 QAM core... Done"));
	
		errCode = BADS_GetTotalChannels(pDevice->adsHandle, &num_ch);
		if (errCode!=BERR_SUCCESS) goto err_init;
	
		if(num_ch > NEXUS_MAX_7125_ADSCHN){
			BDBG_ERR(("7125 only supports %d downstream channels", NEXUS_MAX_7125_ADSCHN));
			goto err_init;
		}
	
		/* Configure ADS channels */
		for (i=0;i<num_ch;i++) {
			errCode = BADS_GetChannelDefaultSettings( pDevice->adsHandle, i, &chnSettings);
			if (errCode!=BERR_SUCCESS) goto err_init;
			errCode = BADS_OpenChannel( pDevice->adsHandle, &pDevice->ads_chn[i], i, &chnSettings);
			if (errCode!=BERR_SUCCESS) goto err_init;
		}
#if NEXUS_FRONTEND_P_7125_ADS_INTERRUPT
		errCode = NEXUS_Core_ConnectInterrupt(pSettings->isrNumber,
											  NEXUS_Frontend_P_7125_L1_isr,
											  (void *)pDevice,(int)0);
#endif
		/* Get events and register callbacks */
		errCode = BADS_GetInterruptEventHandle(pDevice->adsHandle, &pDevice->isrEvent);
		if ( errCode )
		{
			errCode = BERR_TRACE(errCode);
			goto err_app_callback;
		}
		pDevice->isrEventCallback = NEXUS_RegisterEvent(pDevice->isrEvent, NEXUS_Frontend_P_7125_IsrEvent, pDevice);
		if ( NULL == pDevice->isrEventCallback )
		{
			errCode = BERR_TRACE(BERR_OS_ERROR);
			goto err_app_callback;
		}
	}
	else
	{
		BDBG_MSG(("found device"));
	}

	/* check if fronthandle is already opened*/
	if ( pDevice->frontendHandle[chn_num] != NULL )
	{
		return pDevice->frontendHandle[chn_num];
	}

	channelHandle = (NEXUS_7125Channel*)BKNI_Malloc(sizeof(NEXUS_7125Channel));
	if ( NULL == channelHandle )
	{
		errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto err_init;
	}

    /* Create a Nexus frontend handle */
    frontendHandle = NEXUS_Frontend_P_Create(channelHandle);
    if ( NULL == frontendHandle  )
    {
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_init;
    }

    /* Establish device capabilities */
    frontendHandle->capabilities.qam = true;
    BKNI_Memset(frontendHandle->capabilities.qamModes, true, sizeof(frontendHandle->capabilities.qamModes));

    /* Bind required callbacks */
    frontendHandle->close = NEXUS_Frontend_P_7125_CloseQam;
    frontendHandle->tuneQam = NEXUS_Frontend_P_7125_TuneQam;
    frontendHandle->getQamStatus = NEXUS_Frontend_P_7125_GetQamStatus;
    frontendHandle->getSoftDecisions = NEXUS_Frontend_P_7125_GetSoftDecisions;
    frontendHandle->untune = NEXUS_Frontend_P_7125_UntuneQam;
    frontendHandle->resetStatus = NEXUS_Frontend_P_7125_ResetQamStatus;

    callback = NEXUS_IsrCallback_Create(frontendHandle, NULL);
    if ( NULL == callback )    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        goto err_callback_create;
    }

	updateGainCallback = NEXUS_IsrCallback_Create(frontendHandle, NULL);
	if ( NULL == updateGainCallback )    {
		errCode = BERR_TRACE(BERR_OS_ERROR);
		goto err_callback_create;
	}

    errCode = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eLockChange, (BADS_CallbackFunc)NEXUS_Frontend_P_7125_callback_isr, (void*)callback);
    if (errCode!=BERR_SUCCESS) goto err_app_callback;

	errCode = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eUpdateGain, (BADS_CallbackFunc)NEXUS_Frontend_P_7125_callback_isr, (void*)updateGainCallback);
	if (errCode!=BERR_SUCCESS) goto err_app_callback;

    pDevice->lockAppCallback[chn_num] = callback;
	pDevice->updateGainAppCallback[chn_num] = updateGainCallback ;

    pDevice->frontendHandle[chn_num] = frontendHandle;
	pDevice->tuner[chn_num] = pSettings->devices.tuner;

    if ( pDevice->tuner[chn_num] )
    {
        errCode = NEXUS_Tuner_P_Install7125AdsInterrupt(pDevice->tuner[chn_num], NEXUS_Frontend_P_7125_TnrAdsInterrupt_isr, frontendHandle, (int)pDevice);
        if ( errCode )
        {
            goto err_ads_tnr_isr;
        }
    }

    /* save channel number in channelHandle*/
    channelHandle->chn_num = chn_num;
    channelHandle->pDevice = pDevice;
    pDevice->numfrontends++;

    return frontendHandle;

err_ads_tnr_isr:
    NEXUS_IsrCallback_Destroy(callback);
err_callback_create:
    NEXUS_Frontend_P_Destroy(frontendHandle);
    NEXUS_UnregisterEvent(pDevice->isrEventCallback);
err_app_callback:
err_init:
    BADS_Close(pDevice->adsHandle);
err_ads:
    BKNI_Free(pDevice);
	pDevice = NULL;
err_malloc:
    return NULL;
}


/***************************************************************************
 * Module callback functions for tuning
 ***************************************************************************/
static void NEXUS_Frontend_P_7125_CloseQam(
    NEXUS_FrontendHandle handle
    )
{
    NEXUS_7125Channel *pChannel;
    unsigned int i;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    pChannel =(NEXUS_7125Channel*) handle->pDeviceHandle;
    BDBG_ASSERT (pDevice == (NEXUS_7125Qam *)pChannel->pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_7125Qam);

    if ( NULL != pDevice->tuner[pChannel->chn_num] )
    {
        NEXUS_Tuner_P_Remove7125AdsInterrupt(pDevice->tuner[pChannel->chn_num]);
    }

	BADS_InstallCallback(pDevice->ads_chn[pChannel->chn_num], BADS_Callback_eLockChange, (BADS_CallbackFunc)NULL, (void*)NULL);

    if ( NULL != pDevice->lockAppCallback[pChannel->chn_num])
	{
		NEXUS_IsrCallback_Destroy(pDevice->lockAppCallback[pChannel->chn_num]);
		pDevice->lockAppCallback[pChannel->chn_num] = NULL;
	}

	BADS_InstallCallback(pDevice->ads_chn[pChannel->chn_num], BADS_Callback_eUpdateGain, (BADS_CallbackFunc)NULL, (void*)NULL);

	if ( NULL != pDevice->updateGainAppCallback[pChannel->chn_num])
	{
		NEXUS_IsrCallback_Destroy(pDevice->updateGainAppCallback[pChannel->chn_num]);
		pDevice->updateGainAppCallback[pChannel->chn_num] = NULL;
	}

    NEXUS_Frontend_P_Destroy(handle);

    pDevice->frontendHandle[pChannel->chn_num] = NULL;
    BKNI_Free(pChannel);
    pDevice->numfrontends--;

    /* see if there are still open channels*/
    if (pDevice->numfrontends)
	{
		BDBG_MSG(("%s() return with numfrontends = %u",__FUNCTION__, pDevice->numfrontends));
		return;
	}

    for ( i = 0; i < NEXUS_MAX_7125_ADSCHN && NULL != pDevice->ads_chn[i]; i++) {
        BADS_CloseChannel(pDevice->ads_chn[i]);
        (pDevice->ads_chn[i]) = NULL;
    }

	BDBG_MSG(("%s() closed BADS channels, unregistering isrEventCallback",__FUNCTION__));
	NEXUS_UnregisterEvent(pDevice->isrEventCallback);

    if (pDevice->adsHandle) BADS_Close(pDevice->adsHandle);
    pDevice->adsHandle = NULL;

    BDBG_OBJECT_DESTROY(pDevice, NEXUS_7125Qam);
    BKNI_Free(pDevice);
	pDevice = NULL;
}

static NEXUS_Error NEXUS_Frontend_P_7125_TuneQam(void *handle, const NEXUS_FrontendQamSettings *pSettings)
{
    BERR_Code  rc;
    NEXUS_Error errCode;
    NEXUS_7125Channel *channelHandle = (NEXUS_7125Channel *)handle;
    NEXUS_7125Qam *pDevice = channelHandle->pDevice;
    BADS_InbandParam params;
    BADS_ModulationType qam_mode;
    unsigned chn_num = channelHandle->chn_num;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_7125Qam);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(chn_num < NEXUS_MAX_7125_ADSCHN);

    if ( pSettings->annex == NEXUS_FrontendQamAnnex_eA )
    {
        switch ( pSettings->mode )
        {
        case NEXUS_FrontendQamMode_e16:
            qam_mode = BADS_ModulationType_eAnnexAQam16;
            break;
        case NEXUS_FrontendQamMode_e32:
            qam_mode = BADS_ModulationType_eAnnexAQam32;
            break;
        case NEXUS_FrontendQamMode_e64:
            qam_mode = BADS_ModulationType_eAnnexAQam64;
            break;
        case NEXUS_FrontendQamMode_e128:
            qam_mode = BADS_ModulationType_eAnnexAQam128;
            break;
        case NEXUS_FrontendQamMode_e256:
            qam_mode = BADS_ModulationType_eAnnexAQam256;
            break;
        default:
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
    }
    else if ( pSettings->annex == NEXUS_FrontendQamAnnex_eB )
    {
        switch ( pSettings->mode )
        {
        case NEXUS_FrontendQamMode_e64:
            qam_mode = BADS_ModulationType_eAnnexBQam64;
            break;
        case NEXUS_FrontendQamMode_e256:
            qam_mode = BADS_ModulationType_eAnnexBQam256;
            break;
        default:
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
    }
    else if ( pSettings->annex == NEXUS_FrontendQamAnnex_eC )
    {
        switch ( pSettings->mode )
        {
        case NEXUS_FrontendQamMode_e64:
            qam_mode = BADS_ModulationType_eAnnexCQam64;
            break;
        case NEXUS_FrontendQamMode_e256:
            qam_mode = BADS_ModulationType_eAnnexCQam256;
            break;
        default:
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
    }
    else
    {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    params.modType = qam_mode;
    /*params.symbolRate = pSettings->symbolRate;*/
    params.enableDpm = false;
    params.invertSpectrum = false;
    params.spectrum = BADS_SpectrumMode_eAuto;

    NEXUS_IsrCallback_Set(pDevice->lockAppCallback[chn_num], &(pSettings->lockCallback));

    params.symbolRate = pSettings->symbolRate;

    if ((pSettings->symbolRate != 1) && (pSettings->symbolRate != 2)) /* internal codes for "stop timer" and "start timer" */
    {
        if (pDevice->tuner[chn_num])
        {
            pDevice->retuneSettings[chn_num].frequency = pSettings->frequency;
    		switch ( pSettings->bandwidth )
    		{
    			case NEXUS_FrontendQamBandwidth_e8Mhz:
    				pDevice->retuneSettings[chn_num].bandwidth = 8;
    				break;
    			case NEXUS_FrontendQamBandwidth_e7Mhz:
    				pDevice->retuneSettings[chn_num].bandwidth = 7;
    				break;
    			case NEXUS_FrontendQamBandwidth_e6Mhz:
    				pDevice->retuneSettings[chn_num].bandwidth = 6;
    				break;
    			case NEXUS_FrontendQamBandwidth_e5Mhz:
    				pDevice->retuneSettings[chn_num].bandwidth = 5;
    				break;
    			default:
    				return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    		}
            errCode = NEXUS_Tuner_Tune7125(pDevice->tuner[chn_num], &(pDevice->retuneSettings[chn_num]));
    
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }
#ifdef NEXUS_POWER_MANAGEMENT
    rc = BADS_DisablePowerSaver(pDevice->ads_chn[chn_num]);
    if (rc!=BERR_SUCCESS){BERR_TRACE(rc);}
#endif
    rc = BADS_Acquire(pDevice->ads_chn[chn_num], &params );
    if (rc!=BERR_SUCCESS){ return BERR_TRACE(rc);}

    pDevice->last_ads[chn_num] = *pSettings;

    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_7125_GetQamStatus(void *handle, NEXUS_FrontendQamStatus *pStatus)
{
    BERR_Code  rc;
    struct BADS_Status st;
    NEXUS_7125Channel *channelHandle = handle;
    NEXUS_7125Qam *pDevice = (NEXUS_7125Qam *)channelHandle->pDevice;

    BSTD_UNUSED(pStatus);

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_7125Qam);
    BDBG_ASSERT(channelHandle->chn_num < NEXUS_MAX_7125_ADSCHN);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    rc = BADS_GetStatus(pDevice->ads_chn[channelHandle->chn_num], &st);
    if (rc!=BERR_SUCCESS){ return BERR_TRACE(rc); }

    if ( pDevice->tuner[channelHandle->chn_num] )
    {
        NEXUS_7125TunerStatus tunerStatus;
        NEXUS_Tuner_Get7125Status(pDevice->tuner[channelHandle->chn_num], &tunerStatus);
        pStatus->ifAgcLevel = tunerStatus.ifAgcLevel;
        pStatus->rfAgcLevel = tunerStatus.rfAgcLevel;
        pStatus->dsChannelPower = (tunerStatus.dpmLvel * 10)/256; /* 256ths dB to 10ths dB */
        pStatus->lnaAgcLevel = tunerStatus.lnaAgcLevel;
    }

    pStatus->fecLock = st.isFecLock;
    pStatus->receiverLock = st.isQamLock;
    pStatus->snrEstimate = (st.snrEstimate * 100)/256;
    pStatus->fecCorrected = st.accCorrectedCount;
    pStatus->fecUncorrected = st.accUncorrectedCount;
    pStatus->carrierPhaseOffset = st.carrierPhaseOffset;
    pStatus->carrierFreqOffset = st.carrierFreqOffset;
    pStatus->symbolRate = st.rxSymbolRate;
    pStatus->symbolRateError = st.symbolRateError;
    pStatus->berEstimate = st.berRawCount;
    pStatus->mainTap = st.mainTap;
    pStatus->equalizerGain = st.equalizerGain;
    pStatus->postRsBer = 0;/* Not supported */
    pStatus->postRsBerElapsedTime = 0;/* Not supported */
    pStatus->settings = pDevice->last_ads[channelHandle->chn_num];
    pStatus->spectrumInverted = st.isSpectrumInverted;
    pStatus->fecClean = st.cleanCount;
    pStatus->intAgcLevel = st.agcAGFLevel;

    pStatus->viterbiUncorrectedBits = st.correctedBits + (uint32_t)((uint64_t)pStatus->fecUncorrected * 11224)/1000;

    if(pStatus->settings.annex == NEXUS_FrontendQamAnnex_eA){
        pStatus->viterbiTotalBits = (uint32_t)(((uint64_t)pStatus->fecCorrected + (uint64_t)pStatus->fecUncorrected + (uint64_t)pStatus->fecClean) * 204 * 8);  
    }
    else if(pStatus->settings.annex == NEXUS_FrontendQamAnnex_eB){
        pStatus->viterbiTotalBits = (uint32_t)(((uint64_t)pStatus->fecCorrected + (uint64_t)pStatus->fecUncorrected + (uint64_t)pStatus->fecClean) * 127 * 7);  
    }
    if (pStatus->viterbiTotalBits) {
        pStatus->viterbiErrorRate = (uint32_t)((uint64_t)pStatus->viterbiUncorrectedBits * 2097152 * 1024 / (unsigned)pStatus->viterbiTotalBits);
    }
    
    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_7125_GetSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length)
{
    #define TOTAL_ADS_SOFTDECISIONS 30

    int j;
    size_t i;
    NEXUS_Error errCode;
    int16_t return_length;
    NEXUS_7125Channel *channelHandle = handle;
    NEXUS_7125Qam *pDevice = (NEXUS_7125Qam *)channelHandle->pDevice;
    unsigned chn_num = channelHandle->chn_num;
    int16_t d_i[TOTAL_ADS_SOFTDECISIONS], d_q[TOTAL_ADS_SOFTDECISIONS];

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_7125Qam);

    for( i=0; i<length; i += TOTAL_ADS_SOFTDECISIONS )
    {
        errCode = BADS_GetSoftDecision(pDevice->ads_chn[chn_num], (int16_t)TOTAL_ADS_SOFTDECISIONS, d_i, d_q, &return_length);
        if ( errCode )
        {
            return (NEXUS_Error)BERR_TRACE(errCode);
        }
        for ( j=0; j<TOTAL_ADS_SOFTDECISIONS && i+j<length; j++ )
        {
            pDecisions[i+j].i = d_i[j] * 32;
            pDecisions[i+j].q = d_q[j] * 32;
        }
    }
    return NEXUS_SUCCESS;
}

void NEXUS_Frontend_GetDefault7125ConfigSettings(
    NEXUS_7125ConfigSettings *pConfigSettings   /* [out] */
    )
{
    BDBG_ASSERT(NULL != pConfigSettings);
    BKNI_Memset(pConfigSettings, 0, sizeof(*pConfigSettings));
    pConfigSettings->agcValue = 0; /*force gain read*/
}

NEXUS_Error NEXUS_Frontend_7125_GetConfigSettings(
    NEXUS_FrontendHandle handle,                 /* [In]  */
    NEXUS_7125ConfigSettings *pConfigSettings    /* [out]  */
    )
{
    NEXUS_7125Channel *channelHandle = (NEXUS_7125Channel *)handle->pDeviceHandle;
    NEXUS_7125Qam *pDevice = (NEXUS_7125Qam *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_7125Qam);

    pConfigSettings->updateGainCallback = pDevice->updateGainCallbackDesc;
    pConfigSettings->agcValue = pDevice->lnaAgcValue;
    pConfigSettings->gainBoostEnabled = pDevice->lnaGainBoostEnabled;
    pConfigSettings->superBoostEnabled = pDevice->lnaSuperBoostEnabled;
    pConfigSettings->tiltEnabled = pDevice->lnaTiltEnabled;

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Frontend_7125_SetConfigSettings(
    NEXUS_FrontendHandle handle,                 	/* [In] */
    const NEXUS_7125ConfigSettings *pConfigSettings	/* [In] */
    )
{
    NEXUS_Error errCode;
    NEXUS_7125Channel *channelHandle = (NEXUS_7125Channel *)handle->pDeviceHandle;
    NEXUS_7125Qam *pDevice = (NEXUS_7125Qam *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_7125Qam);

    if(channelHandle->chn_num < NEXUS_MAX_7125_ADSCHN) {
        NEXUS_IsrCallback_Set(pDevice->updateGainAppCallback[channelHandle->chn_num], &(pConfigSettings->updateGainCallback));
        pDevice->updateGainCallbackDesc = pConfigSettings->updateGainCallback;

		if ( ( (pDevice->lnaAgcValue != pConfigSettings->agcValue) ||
			   (pDevice->lnaGainBoostEnabled != pConfigSettings->gainBoostEnabled) ||
			   (pDevice->lnaSuperBoostEnabled != pConfigSettings->superBoostEnabled) ||
			   (pDevice->lnaTiltEnabled != pConfigSettings->tiltEnabled))
			 &&(pDevice->tuner[channelHandle->chn_num]))
		{
			errCode = NEXUS_Tuner_SetTunerConfigSettings7125(pDevice->tuner[channelHandle->chn_num], pConfigSettings);
			if (errCode!=NEXUS_SUCCESS) { return BERR_TRACE(errCode);}
			pDevice->lnaAgcValue = pConfigSettings->agcValue;
			pDevice->lnaGainBoostEnabled = pConfigSettings->gainBoostEnabled;
			pDevice->lnaSuperBoostEnabled = pConfigSettings->superBoostEnabled;
			pDevice->lnaTiltEnabled = pConfigSettings->tiltEnabled;
		}
		return NEXUS_SUCCESS;
	}
	else
	{
		return NEXUS_INVALID_PARAMETER;
	}
}

static void NEXUS_Frontend_P_7125_ResetQamStatus(void *handle)
{
    BERR_Code  rc;
    NEXUS_7125Channel *channelHandle = handle;
    NEXUS_7125Qam *pDevice = (NEXUS_7125Qam *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_7125Qam);
    rc = BADS_ResetStatus(pDevice->ads_chn[channelHandle->chn_num]);
    if (rc) rc = BERR_TRACE(rc);
}

static void NEXUS_Frontend_P_7125_TnrAdsInterrupt_isr(void *pParam1, int param2, const BTNR_7125_AdsInterruptData *pData)
{
    NEXUS_FrontendHandle handle = (NEXUS_FrontendHandle)pParam1;
    NEXUS_7125Qam *pDevice = (NEXUS_7125Qam *)param2;
    NEXUS_7125Channel *channelHandle = (NEXUS_7125Channel*) handle->pDeviceHandle;
    unsigned chn_num = channelHandle->chn_num;

    BDBG_ASSERT(sizeof(BTNR_7125_AdsInterruptData) == sizeof(BADS_7125_TnrInterruptData)); /* Make sure structure sizes match */

    BADS_7125_ProcessTnrInterrupt_isr(pDevice->ads_chn[chn_num], (BADS_7125_TnrInterruptData *)pData);
}

static void NEXUS_Frontend_P_7125_UntuneQam(void *handle)
{
    BERR_Code  rc;
	NEXUS_7125Channel *channelHandle = handle;
	NEXUS_7125Qam *pDevice = (NEXUS_7125Qam *)channelHandle->pDevice;
    unsigned chn_num = channelHandle->chn_num;

	BDBG_MSG(("%s()",__FUNCTION__));

	BDBG_OBJECT_ASSERT(pDevice, NEXUS_7125Qam);

#ifdef NEXUS_POWER_MANAGEMENT
    rc = NEXUS_Tuner_Untune7125(pDevice->tuner[chn_num]);
    if (rc) {rc = BERR_TRACE(rc);}
    BDBG_MSG(("%s() call BADS_EnablePowerSaver()",__FUNCTION__));
    rc = BADS_EnablePowerSaver(pDevice->ads_chn[channelHandle->chn_num]);
    if (rc) {rc = BERR_TRACE(rc);}
#else
    BSTD_UNUSED(pDevice);
    BSTD_UNUSED(channelHandle);
    BSTD_UNUSED(rc);
    BSTD_UNUSED(chn_num);
#endif
}
