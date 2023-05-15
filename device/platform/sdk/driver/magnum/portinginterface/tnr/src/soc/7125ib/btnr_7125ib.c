/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_7125ib.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 9/7/12 11:10a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/7125ib/btnr_7125ib.c $
 * 
 * Hydra_Software_Devel/10   9/7/12 11:10a mward
 * SW7425-3765: restructure TNR directory
 * 
 * Hydra_Software_Devel/9   6/6/11 1:55p mward
 * SW7125-967:  BTNR_7125_P_Get_DPM() is not working right.  Exclude it
 * from BTNR_7125_GetTunerStatus().
 * 
 * Hydra_Software_Devel/8   5/27/11 5:07p mward
 * SW7125-926:  Allow use of NEXUS_Amplifier for LNA, rather than direct
 * control in tnr/7125. (merge to main)
 * 
 * Hydra_Software_Devel/SW7125-926/3   5/24/11 12:26p mward
 * SW7125-513:  Set frequency in device settings.  Add rev letter, rev
 * number, and type.
 * 
 * Hydra_Software_Devel/SW7125-926/2   4/29/11 4:36p mward
 * SW7125-926:  Don't set the BBS_Cable_LNA_Address if not using in-driver
 * LNA control.
 * 
 * Hydra_Software_Devel/SW7125-926/1   4/28/11 5:34p mward
 * SW7125-926:  Add a BTNR_SetAgcRegValFunc().  Check for LNA I2C, don't
 * use local LNA control if not provided.
 * 
 * Hydra_Software_Devel/7   4/19/11 3:03p mward
 * SWNOOS-452:  Do some first-time initialization on tuner open, rather
 * than first tune.  This will reduce the chance that initialization of
 * one tuner will interfere with ongoing operation of another (unless the
 * second is only opened after the first is already in use).
 * 
 * Hydra_Software_Devel/6   8/13/10 4:07p mward
 * SW7125-513:  Merge.
 * 
 * Hydra_Software_Devel/SW7125-513/2   8/13/10 4:03p mward
 * SW7125-513:  Options to set AGC thresholds at open, and to skip
 * initialization.  Do initialization common to both tuners only when
 * initializing the first one.
 * 
 * Hydra_Software_Devel/SW7125-513/1   8/3/10 7:05p mward
 * SW7125-513: Reconcile with DOCSIS release STB_3.0.1alpha1_07302010.
 * 
 * Hydra_Software_Devel/5   7/23/10 6:55p mward
 * SW7125-513:  Fixes for dual-tuner.
 * 
 * Hydra_Software_Devel/4   7/22/10 2:46p mward
 * SW7125-513: Disable timer for now, don't have enough timer resources.
 * Will need to share the ads device's timer if one is needed.
 * 
 * Hydra_Software_Devel/3   7/16/10 2:14p mward
 * SW7125-513: Add back last 7550 history to show origin version.
 * 
 * Hydra_Software_Devel/2   7/13/10 7:15p mward
 * SW7125-513: Update macros for multi-channel.
 * 
 * Hydra_Software_Devel/1   7/12/10 3:20p mward
 * SW7125-513:  Add host control of 7125 internal DS and tuner.  Copy and
 * adapt 7550 implementation.
 * 
 * Hydra_Software_Devel/39   6/22/10 6:13p vishk
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "bmem.h"
#include "btnr_priv.h"
#include "btnr_7125.h"
#include "btnr_7125ib_priv.h"
#include "bchp_ds_qdsafe_0_0.h"
#include "btnr_7125_tune.h"
#include "bchp_sun_top_ctrl.h"

BDBG_MODULE(btnr_7125);
#define	DEV_MAGIC_ID			((BERR_TNR_ID<<16) | 0xFACE)

#define BTNR_7125_P_USE_TIMER 0

/******************************************************************************
  BTNR_7125_P_TimerFunc()
 ******************************************************************************/
BERR_Code BTNR_7125_P_TimerFunc(void *myParam1, int myParam2)
{
  BTNR_Handle hDev = (BTNR_Handle)myParam1;
  BTNR_P_7125_Handle *p7125 = (BTNR_P_7125_Handle *)(hDev->hDevImpl);
  BSTD_UNUSED(myParam2);
  if (p7125->hInterruptEvent)
  {
	  BKNI_SetEvent(p7125->hInterruptEvent); 
  }
  return BERR_SUCCESS;
}



/******************************************************************************
  BTNR_7125_P_GetInterruptEventHandle()
 ******************************************************************************/
BERR_Code BTNR_7125_GetInterruptEventHandle(BTNR_Handle h, BKNI_EventHandle* hEvent)
{
    *hEvent = ((BTNR_P_7125_Handle *)(h->hDevImpl))->hInterruptEvent;
    return BERR_SUCCESS;
}

/******************************************************************************
  BTNR_7125_P_ProcessInterruptEvent()
 ******************************************************************************/
BERR_Code BTNR_7125_ProcessInterruptEvent(BTNR_Handle hDev)
{
  BTNR_P_7125_Handle *p7125 = (BTNR_P_7125_Handle *)(hDev->hDevImpl);
  BTNR_7125_TuneSettings pInitSettings; 

  if (p7125->pTnrModeData->BBSNexusConnectMode & TNR_NexusStatusMode_EnableStatusForNexus)
  {
	BTNR_7125_P_Get_Status_TNR_Core0(p7125);
	p7125->pTnrModeData->BBSNexusConnectMode &= (~TNR_NexusStatusMode_EnableStatusForNexus);
  }
 
  if (p7125->pTnrModeData->BBSConnectMode & TNR_TuneStartMode_Manual)
  {
	p7125->pTnrModeData->BBSConnectMode &= (~TNR_TuneStartMode_Manual);
	pInitSettings.Bandwidth = p7125->pTnrModeData->TunerBw;
	pInitSettings.Freq = p7125->pTnrModeData->TunerFreq;
	BTNR_7125_Tune(hDev, &pInitSettings);
  }
  else if (p7125->pTnrModeData->BBSConnectMode & TNR_TuneStartMode_DPMStatus)
  {
	p7125->pTnrModeData->BBSConnectMode &= (~TNR_TuneStartMode_DPMStatus);
	BTNR_7125_P_Get_DPM(p7125);
  }
  else if (p7125->pTnrModeData->BBSConnectMode & TNR_TuneStartMode_ResetStatus)
  {
	p7125->pTnrModeData->BBSConnectMode &= (~TNR_TuneStartMode_ResetStatus);
	BTNR_7125_P_Reset_Status_TNR_Core0(p7125);
  }

  return BERR_SUCCESS;
}

/*******************************************************************************/
BERR_Code BTNR_7125_Open(
    BTNR_Handle *phDev,                 /* [output] Returns handle */
	BREG_Handle hRegister,				/* Register handle */
    BTNR_7125_Settings *pSettings		/* [Input] settings structure */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_7125_Handle h7125Dev;
    BTNR_P_7125_Settings *pTnrImplData;
    BTNR_Handle hDev;
	uint32_t BufSrc;

    BDBG_ENTER(BTNR_7125_Open);

    hDev = NULL;
	if (pSettings->unitNumber > 1)
	{
		BDBG_ERR(("BTNR_7125_Open: bad unitNumber %u, only 0 and 1 allowed.\n",pSettings->unitNumber));
		retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto done;
	}
    /* Alloc memory from the system heap */
    h7125Dev = (BTNR_7125_Handle) BKNI_Malloc( sizeof( BTNR_P_7125_Handle ) );
    if( h7125Dev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_7125_Open: BKNI_malloc() failed\n"));
        goto done;
    }
    BKNI_Memset( h7125Dev, 0x00, sizeof( BTNR_P_7125_Handle ) );

    hDev = (BTNR_Handle) BKNI_Malloc( sizeof( BTNR_P_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_7125_Open: BKNI_malloc() failed\n"));
        BKNI_Free( h7125Dev );
        goto done;
    }
    BKNI_Memset( hDev, 0x00, sizeof( BTNR_P_Handle ) );

	h7125Dev->pTnrModeData = (BTNR_P_7125_AcquireSettings_t *)BMEM_AllocAligned(pSettings->hHeap, sizeof(BTNR_P_7125_AcquireSettings_t), 0, 0 );
	BKNI_Memset( h7125Dev->pTnrModeData, 0x00, sizeof( BTNR_P_7125_AcquireSettings_t ) );
	h7125Dev->pTnrStatus = (BTNR_7125_P_Status_t *)BMEM_AllocAligned(pSettings->hHeap, sizeof(BTNR_7125_P_Status_t), 0, 0 );
	BKNI_Memset( h7125Dev->pTnrStatus, 0x00, sizeof( BTNR_7125_P_Status_t ) );
	BMEM_ConvertAddressToOffset(pSettings->hHeap, h7125Dev->pTnrStatus, &BufSrc );
	h7125Dev->pTnrModeData->StartSturctureAddress = BufSrc;
	h7125Dev->hI2cLNA = pSettings->hI2cLNA;
    h7125Dev->magicId = DEV_MAGIC_ID;
	h7125Dev->hHeap = pSettings->hHeap;
    pTnrImplData = &h7125Dev->settings;
    pTnrImplData->rfFreq = 0;
    pTnrImplData->tunerMode = BTNR_TunerMode_eLast;
	pTnrImplData->unitNumber = pSettings->unitNumber;
	pTnrImplData->iRevLetter = (BCHP_VER>>16)&0xFF;
	pTnrImplData->iRevNumber = (BCHP_VER)&0xFF;
	pTnrImplData->iType = 0x7125;
	h7125Dev->hRegister = hRegister;

    hDev->hDevImpl = (void *) h7125Dev;
    hDev->magicId = DEV_MAGIC_ID;
    hDev->pSetRfFreq = (BTNR_SetRfFreqFunc) BTNR_7125_SetRfFreq;
    hDev->pGetRfFreq = (BTNR_GetRfFreqFunc) BTNR_7125_GetRfFreq;
    hDev->pGetAgcRegVal = (BTNR_GetAgcRegValFunc) BTNR_P_7125_GetAgcRegVal;
    hDev->pSetAgcRegVal = (BTNR_SetAgcRegValFunc) BTNR_P_7125_SetAgcRegVal;
    hDev->pGetInfo = (BTNR_GetInfoFunc) BTNR_7125_GetInfo;
    hDev->pClose = (BTNR_CloseFunc) BTNR_7125_Close;
    hDev->pGetPowerSaver = (BTNR_GetPowerSaverFunc) BTNR_7125_GetPowerSaver;
    hDev->pSetPowerSaver = (BTNR_SetPowerSaverFunc) BTNR_7125_SetPowerSaver;

	BKNI_CreateEvent(&(h7125Dev->hInterruptEvent));

	#if BTNR_7125_P_USE_TIMER
	{
		/* Create timer for status lock check */
		BTMR_Settings sTimerSettings;

		BTMR_GetDefaultTimerSettings(&sTimerSettings);
		sTimerSettings.type = BTMR_Type_ePeriodic;
		sTimerSettings.cb_isr = (BTMR_CallbackFunc)BTNR_7125_P_TimerFunc;
		sTimerSettings.pParm1 = (void*)hDev;
		sTimerSettings.parm2 = 0;
		sTimerSettings.exclusive = false;
	
		retCode = BTMR_CreateTimer (pSettings->hTmr, &h7125Dev->hTimer, &sTimerSettings);
	}
    if ( retCode != BERR_SUCCESS )
    {
        BDBG_ERR(("%s(): Create Timer Failed",__FUNCTION__));
        retCode = BERR_TRACE(retCode);
        goto done;
    }
	#else
	h7125Dev->hTimer = NULL;
	#endif

	if ((h7125Dev->pTnrModeData->BBS_Cable_LNA_Address == 0) && (h7125Dev->hI2cLNA))
	{
		h7125Dev->pTnrModeData->BBS_Cable_LNA_Address = CABLE_LNA_BASE_ADDRESS;
		BTNR_7125_P_Probe_LNA(h7125Dev); 
	}
	h7125Dev->pTnrModeData->Min_AGC_Threshold = pSettings->VgaCal.Min_AGC_Threshold;
	h7125Dev->pTnrModeData->Max_AGC_Threshold = pSettings->VgaCal.Max_AGC_Threshold;
	h7125Dev->pTnrModeData->DoneFirstTimeFlag = pSettings->skipInit ? BTNR_ON : BTNR_OFF;
done:
    *phDev = hDev;
    BDBG_LEAVE(BTNR_7125_Open);
    return( retCode );
}

BERR_Code BTNR_7125_GetDefaultSettings(
    BTNR_7125_Settings *pDefSettings  /* [out] Returns default setting */
    )
{
    BDBG_ASSERT(NULL != pDefSettings);

    BKNI_Memset(pDefSettings, 0, sizeof(*pDefSettings));
	pDefSettings->VgaCal.Min_AGC_Threshold = 	DS_MIN_AGC_THRESHOLD;
	pDefSettings->VgaCal.Max_AGC_Threshold = 	DS_MAX_AGC_THRESHOLD;

    return BERR_SUCCESS;
}

/********************************************
 Tuner default settings 
 *******************************************/


static const BTNR_7125_TuneSettings defTuneParams = 
{
    BTNR_7125_Bandwidth_e6MHz,		/* Bandwidth */
    147000000                       /* Tuner Frequency */
};

/***************************************************************************
Summary:
	BTNR_7125_GetDefaultTuneSettings
****************************************************************************/
BERR_Code BTNR_7125_GetDefaultTuneSettings(BTNR_7125_TuneSettings *pSettings)
{
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = defTuneParams; 
	return BERR_SUCCESS;
}


/***************************************************************************
Summary:
	BTNR_7125_Tune
****************************************************************************/
BERR_Code BTNR_7125_Tune(
    BTNR_Handle hTnr,
    const BTNR_7125_TuneSettings *pSettings
    )
{

	BTNR_P_7125_Handle *h7125Dev=((BTNR_P_7125_Handle *)(hTnr->hDevImpl));
	BTNR_7125_TuneSettings pInitSettings; 

	BDBG_MSG(("BTNR_7125_Tune(%u,%u,%u)\n",h7125Dev->settings.unitNumber,pSettings->Freq,pSettings->Bandwidth));


	
	if (h7125Dev->pTnrModeData->NexusParamsDisable  & TNR_NexusParamsMode_eDisable)
	{
		pInitSettings.Bandwidth = h7125Dev->pTnrModeData->TunerBw;
		pInitSettings.Freq = h7125Dev->pTnrModeData->TunerFreq;
	}
	else
	{
		pInitSettings.Bandwidth = pSettings->Bandwidth;
		pInitSettings.Freq = pSettings->Freq;
	}
	if (h7125Dev->pTnrModeData->TunerFreq==0)
	{	/*MCW initTune was done on change of settings/mode, may now be unnecessary. */
		h7125Dev->pTnrModeData->TunerBw = pInitSettings.Bandwidth;
		h7125Dev->pTnrModeData->TunerFreq = pInitSettings.Freq;
		h7125Dev->settings.rfFreq = pInitSettings.Freq;
		h7125Dev->pTnrModeData->TunerAcquisitionType = BTNR_7125_TunerAcquisitionType_eInitTune;
		BTNR_7125_P_Tune_TNR_Core0(h7125Dev);
	}
	else if ((pInitSettings.Freq != h7125Dev->pTnrModeData->TunerFreq) || 
		(pInitSettings.Freq != h7125Dev->pTnrModeData->TunerFreq))
	{
		h7125Dev->pTnrModeData->TunerBw = pInitSettings.Bandwidth;
		h7125Dev->pTnrModeData->TunerFreq = pInitSettings.Freq;
		h7125Dev->settings.rfFreq = pInitSettings.Freq;
		h7125Dev->pTnrModeData->TunerAcquisitionType = BTNR_7125_TunerAcquisitionType_eTune;
		BTNR_7125_P_Tune_TNR_Core0(h7125Dev);
	}
	else
	{
		h7125Dev->pTnrModeData->TunerAcquisitionType = BTNR_7125_TunerAcquisitionType_eMiniTune;
		BTNR_7125_P_Tune_TNR_Core0(h7125Dev);
	}

	if (h7125Dev->hTimer && (h7125Dev->pTnrModeData->TunerAcquisitionType != BTNR_7125_TunerAcquisitionType_eMiniTune))
	{
		BTMR_StopTimer(h7125Dev->hTimer);
		BTMR_StartTimer(h7125Dev->hTimer, 500000);   /* the timer is in Micro second */
		BDBG_MSG(("BTNR_7125_Tune\n"));
	}

	return BERR_SUCCESS;
}


/***************************************************************************
Summary:
	BTNR_7125_SetRFByPassMode
****************************************************************************/
BERR_Code BTNR_7125_SetRFByPassMode(
    BTNR_Handle hTnr,
    bool mode
    )
{

	BTNR_P_7125_Handle *h7125Dev=((BTNR_P_7125_Handle *)(hTnr->hDevImpl));
	h7125Dev->pTnrModeData->RFbypassMode = mode;
	BTNR_7125_P_Set_Rf_ByPass_mode(h7125Dev, mode);
	return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	BTNR_7125_GetRFByPassMode
****************************************************************************/
BERR_Code BTNR_7125_GetRFByPassMode(
    BTNR_Handle hTnr,
    bool *mode
    )
{

	BTNR_P_7125_Handle *h7125Dev=((BTNR_P_7125_Handle *)(hTnr->hDevImpl));
	*mode = h7125Dev->pTnrModeData->RFbypassMode;
	return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	BTNR_7125_GetTunerStatus
****************************************************************************/
BERR_Code BTNR_7125_GetTunerStatus(
    BTNR_Handle hTnr,
    BTNR_7125_TunerStatus *pStatus
    )
{

	BTNR_P_7125_Handle *h7125Dev=((BTNR_P_7125_Handle *)(hTnr->hDevImpl));
	BTNR_7125_P_Get_Status_TNR_Core0(h7125Dev);
	pStatus->rfAgcVal = (int32_t)(h7125Dev->pTnrStatus->RF_AGC_LEVEL);
	pStatus->ifAgcVal = (int32_t)(h7125Dev->pTnrStatus->IF_AGC_LEVEL);
	pStatus->lnaAgcVal = (int32_t)(h7125Dev->pTnrStatus->LNA_AGC_LEVEL);
	#if 0 /* Not working well */
	BTNR_7125_P_Get_DPM(h7125Dev);
	#endif
	pStatus->dpmLvel = (int32_t)(h7125Dev->pTnrStatus->DPM_Gain_256dbmV);

	return BERR_SUCCESS;
}

/***************************************************************************
BTNR_InstallDeviceInterruptCallback: Used to enable and install an application
                                     callback for Device relevant Interrupt.
****************************************************************************/
BERR_Code BTNR_7125_InstallAdsInterruptCallback(BTNR_Handle						hDev,
												BTNR_7125_AdsInterruptCallback  fCallBack_isr,
												void                            *pParam1,
												int					            param2
												)
{
   BTNR_P_7125_Handle *p7125 = (BTNR_P_7125_Handle *)(hDev->hDevImpl);
   BTNR_P_CallbackInfo *callback;

   BDBG_ENTER(BTNR_InstallDeviceInterruptCallback);
   callback =&(p7125->InterruptCallbackInfo);

   BKNI_EnterCriticalSection();
   callback->func = (BTNR_7125_AdsInterruptCallback)fCallBack_isr;
   callback->pParm1 = pParam1;
   callback->Parm2 = param2;
   BKNI_LeaveCriticalSection();
   

   BDBG_LEAVE(BTNR_InstallDeviceInterruptCallback);

   return BERR_TRACE(BERR_SUCCESS);
}


/******************************************************************************
BTNR_7125_UnInstallInterruptCallback()
******************************************************************************/ 
BERR_Code BTNR_7125_RemoveAdsInterruptCallback(
    BTNR_Handle handle  /* [in] BTNR handle */
)
{
    BTNR_P_CallbackInfo *callback;
    BTNR_P_7125_Handle *p7125 = (BTNR_P_7125_Handle *)(handle->hDevImpl);

    BDBG_ASSERT(handle);
    
    callback = &(p7125->InterruptCallbackInfo);

    callback->func = NULL;
    callback->pParm1 = NULL;
    callback->Parm2 = (int)NULL;

    return BERR_TRACE(BERR_SUCCESS);
} 


