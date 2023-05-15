/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_7550ib.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 9/11/12 11:55a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/7550ib/btnr_7550ib.c $
 * 
 * Hydra_Software_Devel/3   9/11/12 11:55a farshidf
 * SW7425-3819: Remove symbolic links and flatten out THD directory
 * structure
 * 
 * Hydra_Software_Devel/2   9/11/12 11:51a farshidf
 * SW7425-3819: Remove symbolic links and flatten out THD directory
 * structure
 * 
 * Hydra_Software_Devel/1   9/7/12 10:11a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/44   12/6/10 10:18a farshidf
 * SW7550-628: Ping merge
 * 
 * Hydra_Software_Devel/SW7550-628/1   12/3/10 4:49p pinglong
 * SW7550-628: AGC sometimes is out of range after tuning to an ISDB-T
 * channel
 * 
 * Hydra_Software_Devel/43   10/28/10 6:49p farshidf
 * SW7550-607: new BBS interface
 * 
 * Hydra_Software_Devel/42   10/15/10 11:23a farshidf
 * SW7550-592: merge
 * 
 * Hydra_Software_Devel/SW7550-592/2   10/14/10 1:59p farshidf
 * SW7550-592: change the name
 * 
 * Hydra_Software_Devel/SW7550-592/1   10/12/10 6:09p farshidf
 * SW7550-592: create a seperate call for DPM
 * 
 * Hydra_Software_Devel/40   9/29/10 10:41a farshidf
 * SW7550-587: merge to main
 * 
 * Hydra_Software_Devel/SW7550-587/1   9/29/10 6:14p pinglong
 * SW7550-587: support setting LNA Boost/Tilt on and off
 * 
 * Hydra_Software_Devel/39   6/22/10 6:13p vishk
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 * Hydra_Software_Devel/38   5/19/10 5:59p farshidf
 * SW7550-400: add the ofdm signal strength and dpm power level
 * 
 * Hydra_Software_Devel/37   5/18/10 12:00p farshidf
 * SW7550-440: merge to main branch
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/SW7550-440/1   5/18/10 11:47a farshidf
 * SW7550-440: add the dpm callback
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/5   5/14/10 4:40p farshidf
 * SW7550-440: add the installisrcallback
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/4   5/10/10 2:19p farshidf
 * SW7550-38: update the default settings
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/3   5/3/10 5:29p farshidf
 * SW7550-38: fix the BBS status page while mini tune is getting called
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/2   5/3/10 1:49p farshidf
 * SW7550-38: do not stop the tuner in mini tune mode
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/1   4/29/10 6:50p farshidf
 * SW7550-38: add the new cable mode for tuner
 * 
 * Hydra_Software_Devel/36   4/27/10 4:59p farshidf
 * SW7550-38: store the bandwidth and Freq
 * 
 * Hydra_Software_Devel/35   4/27/10 2:36p farshidf
 * SW7550-38: typo fix
 * 
 * Hydra_Software_Devel/34   4/27/10 2:00p farshidf
 * SW7550-38: add the mini tune call
 * 
 * Hydra_Software_Devel/33   4/15/10 2:54p farshidf
 * SW7550-236: add the AGC value
 * 
 * Hydra_Software_Devel/32   4/15/10 2:06p farshidf
 * SW7550-236: add the AGC setting for QAM
 * 
 * Hydra_Software_Devel/31   3/12/10 5:41p farshidf
 * SW7550-38: set the timer exclusive to false after fixing the tmr code
 * 
 * Hydra_Software_Devel/30   3/8/10 1:31a farshidf
 * SW7550-38: use exclusive timer
 * 
 * Hydra_Software_Devel/28   3/4/10 5:33p farshidf
 * SW7550-38: increase the timer to 500ms
 * 
 * Hydra_Software_Devel/27   3/4/10 5:23p farshidf
 * SW7550-38: add the dynamic LNA address
 * 
 * Hydra_Software_Devel/26   3/3/10 12:19p farshidf
 * SW7550-38: merge from branch 9
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/3   2/27/10 1:31a thayashi
 * Runs tuner init only when mode is changed
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/2   2/25/10 12:02p farshidf
 * SW7550-38: add the get RFbypass
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/1   2/24/10 5:07p farshidf
 * SW7550-38: RF bypass update
 * 
 * Hydra_Software_Devel/24   1/11/10 7:03p farshidf
 * SW7550-38: compile fix
 * 
 * Hydra_Software_Devel/23   1/11/10 6:56p farshidf
 * SW7550-38: add the rfagc calc
 * 
 * Hydra_Software_Devel/22   1/11/10 4:29p farshidf
 * SW7550-38: compile fix
 * 
 * Hydra_Software_Devel/21   1/11/10 4:26p farshidf
 * SW7550-38: upodate the Tuner get status
 * 
 * Hydra_Software_Devel/20   1/8/10 5:20p farshidf
 * SW7550-38: compile fix
 * 
 * Hydra_Software_Devel/19   1/8/10 5:13p farshidf
 * SW7550-38: add the BTNR_7550_GetTunerStatus to ge the tuner Rfagcvalue
 * 
 * Hydra_Software_Devel/18   1/6/10 10:44a farshidf
 * SW7550-38: change the function name from BTNR_7550_RFByPassMode to
 * BTNR_7550_SetRFByPassMode
 * 
 * Hydra_Software_Devel/17   1/5/10 12:47p farshidf
 * SW7550-38: add the RF bypass and daisy chain
 * 
 * Hydra_Software_Devel/16   12/28/09 2:53p farshidf
 * w7550-38: compile fix
 * 
 * Hydra_Software_Devel/15   12/28/09 2:45p farshidf
 * SW7550-38: be able to tune with BBS with Nexus
 * 
 * Hydra_Software_Devel/14   12/22/09 2:07p farshidf
 * SW7550-38: add hack code to enable the Tuner status in Nexus
 * 
 * Hydra_Software_Devel/13   11/30/09 12:56p farshidf
 * SW7550-38: add the hHeap to be able to free it at close
 * 
 * Hydra_Software_Devel/12   11/20/09 6:45p farshidf
 * SW7550-38: update the interface param
 * 
 * Hydra_Software_Devel/11   11/19/09 5:11p farshidf
 * SW7550-38: check-in latest changes
 * 
 * Hydra_Software_Devel/10   11/17/09 8:15p farshidf
 * SW7550-38: update the return
 * 
 * Hydra_Software_Devel/9   11/17/09 8:02p farshidf
 * SW7550-38: fix code
 * 
 * Hydra_Software_Devel/8   11/17/09 7:59p farshidf
 * SW7550-38: update the code to the latest nexus call
 * 
 * Hydra_Software_Devel/7   11/13/09 11:28a farshidf
 * SW7550-38: update the BBS interface
 * 
 * Hydra_Software_Devel/6   11/12/09 10:26p farshidf
 * SW7550-38: Use THD Reserve for BBS
 * 
 * Hydra_Software_Devel/5   11/12/09 10:04p farshidf
 * SW7550-38: BBS code changes required
 * 
 * Hydra_Software_Devel/4   11/12/09 8:40p farshidf
 * SW7550-38:add  BBS control code
 * 
 * Hydra_Software_Devel/3   11/12/09 6:57p farshidf
 * SW7550-40: update the tuner files
 * 
 * Hydra_Software_Devel/2   11/10/09 7:11p farshidf
 * 7550-38: add the tuner code
 * 
 * Hydra_Software_Devel/1   11/5/09 10:24a farshidf
 * SW7550-38: add the 7550 tuner code
 * 
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "bmem.h"
#include "btnr_priv.h"
#include "btnr_7550ib.h"
#include "btnr_75xxib_priv.h"
#include "bchp_ds_qdsafe_0.h"
#include "bchp_thd_core.h"
#include "btnr_7550ib_tune.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_gfd_1.h"

BDBG_MODULE(btnr_7550ib);
#define	DEV_MAGIC_ID			((BERR_TNR_ID<<16) | 0xFACE)


/******************************************************************************
  BTNR_7550_P_TimerFunc()
 ******************************************************************************/
BERR_Code BTNR_7550_P_TimerFunc(void *myParam1, int myParam2)
{
  BTNR_Handle hDev = (BTNR_Handle)myParam1;
  BTNR_P_75xx_Handle *p75xx = (BTNR_P_75xx_Handle *)(hDev->hDevImpl);
  BSTD_UNUSED(myParam2);
  BKNI_SetEvent(p75xx->hInterruptEvent); 
  return BERR_SUCCESS;
}



/******************************************************************************
  BTNR_7550_P_GetInterruptEventHandle()
 ******************************************************************************/
BERR_Code BTNR_7550_GetInterruptEventHandle(BTNR_Handle h, BKNI_EventHandle* hEvent)
{
    *hEvent = ((BTNR_P_75xx_Handle *)(h->hDevImpl))->hInterruptEvent;
    return BERR_SUCCESS;
}

/******************************************************************************
  BTNR_7550_P_ProcessInterruptEvent()
 ******************************************************************************/
BERR_Code BTNR_7550_ProcessInterruptEvent(BTNR_Handle hDev)
{
  BTNR_P_75xx_Handle *p75xx = (BTNR_P_75xx_Handle *)(hDev->hDevImpl);
  BTNR_7550_TuneSettings pInitSettings; 

  if (p75xx->pTnrModeData->BBSNexusConnectMode & TNR_NexusStatusMode_EnableStatusForNexus)
  {
	BTNR_7550_P_Get_Status_TNR_Core0(p75xx);
	p75xx->pTnrModeData->BBSNexusConnectMode &= (~TNR_NexusStatusMode_EnableStatusForNexus);
  }
 
  if (p75xx->pTnrModeData->BBSConnectMode & TNR_TuneStartMode_Manual)
  {
	p75xx->pTnrModeData->BBSConnectMode &= (~TNR_TuneStartMode_Manual);
	pInitSettings.Bandwidth = p75xx->pTnrModeData->TunerBw;
	pInitSettings.AcquisitionMode = p75xx->pTnrModeData->TunerAcquisitionMode;
	pInitSettings.Freq = p75xx->pTnrModeData->TunerFreq;
	pInitSettings.TunerSetup = p75xx->pTnrModeData->TunerFreq;
	BTNR_7550_Tune(hDev, &pInitSettings);
  }
  else if (p75xx->pTnrModeData->BBSConnectMode & TNR_TuneStartMode_DPMStatus)
  {
	p75xx->pTnrModeData->BBSConnectMode &= (~TNR_TuneStartMode_DPMStatus);
	BTNR_7550_P_Get_DPM(p75xx);
  }
  else if (p75xx->pTnrModeData->BBSConnectMode & TNR_TuneStartMode_ResetStatus)
  {
	p75xx->pTnrModeData->BBSConnectMode &= (~TNR_TuneStartMode_ResetStatus);
	BTNR_7550_P_Reset_Status_TNR_Core0(p75xx);
  }

  return BERR_SUCCESS;
}

/*******************************************************************************/
BERR_Code BTNR_7550_Open(
    BTNR_Handle *phDev,                 /* [output] Returns handle */
	BREG_Handle hRegister,				/* Register handle */
    BTNR_7550_Settings *pSettings		/* [Input] settings structure */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_75xx_Handle h75xxDev;
    BTNR_P_75xx_Settings *pTnrImplData;
	BTMR_Settings sTimerSettings;
    BTNR_Handle hDev;
	uint32_t BufSrc;
	uint32_t reg;
	static bool first_tune = false;

    BDBG_ENTER(BTNR_7550_Open);
    BSTD_UNUSED( pSettings );
    
    hDev = NULL;
    /* Alloc memory from the system heap */
    h75xxDev = (BTNR_75xx_Handle) BKNI_Malloc( sizeof( BTNR_P_75xx_Handle ) );
    if( h75xxDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_7550_Open: BKNI_malloc() failed\n"));
        goto done;
    }
    BKNI_Memset( h75xxDev, 0x00, sizeof( BTNR_P_75xx_Handle ) );

    hDev = (BTNR_Handle) BKNI_Malloc( sizeof( BTNR_P_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_7550_Open: BKNI_malloc() failed\n"));
        BKNI_Free( h75xxDev );
        goto done;
    }
    BKNI_Memset( hDev, 0x00, sizeof( BTNR_P_Handle ) );

	h75xxDev->pTnrModeData = (BTNR_P_75xx_AcquireSettings_t *)BMEM_AllocAligned(pSettings->hHeap, sizeof(BTNR_P_75xx_AcquireSettings_t), 0, 0 );
	BKNI_Memset( h75xxDev->pTnrModeData, 0x00, sizeof( BTNR_P_75xx_AcquireSettings_t ) );
	BMEM_ConvertAddressToOffset(pSettings->hHeap, h75xxDev->pTnrModeData, &BufSrc );
	BREG_Write32(hRegister, BCHP_GFD_1_SCRATCH0, BufSrc);
	h75xxDev->pTnrStatus = (BTNR_75xx_P_Status_t *)BMEM_AllocAligned(pSettings->hHeap, sizeof(BTNR_75xx_P_Status_t), 0, 0 );
	BKNI_Memset( h75xxDev->pTnrStatus, 0x00, sizeof( BTNR_75xx_P_Status_t ) );
	BMEM_ConvertAddressToOffset(pSettings->hHeap, h75xxDev->pTnrStatus, &BufSrc );
	h75xxDev->pTnrModeData->StartSturctureAddress = BufSrc;
	h75xxDev->hI2cTHD = pSettings->hI2cTHD;
	h75xxDev->hI2cADS = pSettings->hI2cADS;
    h75xxDev->magicId = DEV_MAGIC_ID;
	h75xxDev->hHeap = pSettings->hHeap;
    pTnrImplData = &h75xxDev->settings;
    pTnrImplData->rfFreq = 0;
    pTnrImplData->tunerMode = BTNR_TunerMode_eLast;
	h75xxDev->hRegister = hRegister;

    hDev->hDevImpl = (void *) h75xxDev;
    hDev->magicId = DEV_MAGIC_ID;
    hDev->pSetRfFreq = (BTNR_SetRfFreqFunc) BTNR_75xx_SetRfFreq;
    hDev->pGetRfFreq = (BTNR_GetRfFreqFunc) BTNR_75xx_GetRfFreq;
    hDev->pGetAgcRegVal = (BTNR_GetAgcRegValFunc) BTNR_P_75xx_GetAgcRegVal;
    hDev->pSetAgcRegVal = (BTNR_SetAgcRegValFunc) NULL;
    hDev->pGetInfo = (BTNR_GetInfoFunc) BTNR_75xx_GetInfo;
    hDev->pClose = (BTNR_CloseFunc) BTNR_75xx_Close;
    hDev->pGetPowerSaver = (BTNR_GetPowerSaverFunc) BTNR_75xx_GetPowerSaver;
    hDev->pSetPowerSaver = (BTNR_SetPowerSaverFunc) BTNR_75xx_SetPowerSaver;

	BKNI_CreateEvent(&(h75xxDev->hInterruptEvent));

	    /* Create timer for status lock check */
    BTMR_GetDefaultTimerSettings(&sTimerSettings);
    sTimerSettings.type = BTMR_Type_ePeriodic;
    sTimerSettings.cb_isr = (BTMR_CallbackFunc)BTNR_7550_P_TimerFunc;
    sTimerSettings.pParm1 = (void*)hDev;
    sTimerSettings.parm2 = 0;
    sTimerSettings.exclusive = false;

    retCode = BTMR_CreateTimer (pSettings->hTmr, &h75xxDev->hTimer, &sTimerSettings);
    if ( retCode != BERR_SUCCESS )
    {
        BDBG_ERR(("BTHD_Open: Create Timer Failed"));
        retCode = BERR_TRACE(retCode);
        goto done;
    }	

	if (first_tune == false)
	{
		reg = BREG_Read32(h75xxDev->hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14);
		reg &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_02) |
				BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_03));
		reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_02, 1) | 
				BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_03, 1);
		BREG_Write32 (h75xxDev->hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, reg);
#ifdef REF_7550_BOARD
		h75xxDev->pTnrModeData->BBS_Cable_LNA_Address = CABLE_LNA_BASE_ADDRESS;
		BTNR_7550_P_Probe_Second_LNA(h75xxDev); 
#else
		BTNR_7550_P_Probe_First_LNA(h75xxDev);
#endif
		first_tune = true;
	}
	
done:
    *phDev = hDev;
    BDBG_LEAVE(BTNR_7550_Open);
    return( retCode );
}

BERR_Code BTNR_7550_GetDefaultSettings(
    BTNR_7550_Settings *pDefSettings  /* [out] Returns default setting */
    )
{
    BDBG_ASSERT(NULL != pDefSettings);

    BKNI_Memset(pDefSettings, 0, sizeof(*pDefSettings));

    return BERR_SUCCESS;
}

/********************************************
 Tuner default settings 
 *******************************************/


static const BTNR_7550_TuneSettings defTuneParams = 
{
    BTNR_7550_Bandwidth_e8MHz,		/* Bandwidth */
	BTNR_7550_Standard_eDVBT,       /* Inband mode */
    666000000,                      /* Tuner Frequency */
	BTNR_7550_Setup_eTerrestrial,			/* Tuner Mode */
};

/***************************************************************************
Summary:
	BTNR_7550_GetDefaultTuneSettings
****************************************************************************/
BERR_Code BTNR_7550_GetDefaultTuneSettings(BTNR_7550_TuneSettings *pSettings)
{
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = defTuneParams; 
	return BERR_SUCCESS;
}


/***************************************************************************
Summary:
	BTNR_7550_Tune
****************************************************************************/
BERR_Code BTNR_7550_Tune(
    BTNR_Handle hTnr,
    const BTNR_7550_TuneSettings *pSettings
    )
{

	BTNR_P_75xx_Handle *h75Dev=((BTNR_P_75xx_Handle *)(hTnr->hDevImpl));
	BTNR_7550_TuneSettings pInitSettings; 
	static BTNR_7550_Standard TunerAcquisitionMode = 0;
	static BTNR_7550_Bandwidth TunerBw = 0;
	static uint32_t TunerFreq = 0;
	static uint32_t TunerSetup = 0;

	BDBG_MSG(("BTNR_7550_Tune\n"));


	
	if (h75Dev->pTnrModeData->NexusParamsDisable  & TNR_NexusParamsMode_eDisable)
	{
		pInitSettings.Bandwidth = h75Dev->pTnrModeData->TunerBw;
		pInitSettings.AcquisitionMode = h75Dev->pTnrModeData->TunerAcquisitionMode;
		pInitSettings.Freq = h75Dev->pTnrModeData->TunerFreq;
		pInitSettings.TunerSetup = h75Dev->pTnrModeData->TunerSetup;
	}
	else
	{
		pInitSettings.Bandwidth = pSettings->Bandwidth;
		pInitSettings.AcquisitionMode = pSettings->AcquisitionMode;
		pInitSettings.Freq = pSettings->Freq;
		pInitSettings.TunerSetup =pSettings->TunerSetup;
	}
	if ((pInitSettings.AcquisitionMode != TunerAcquisitionMode) || (pInitSettings.TunerSetup != TunerSetup))
	{
		h75Dev->pTnrModeData->TunerBw = pInitSettings.Bandwidth;
		h75Dev->pTnrModeData->TunerAcquisitionMode = pInitSettings.AcquisitionMode;
		h75Dev->pTnrModeData->TunerFreq = pInitSettings.Freq;
		h75Dev->pTnrModeData->TunerSetup = pInitSettings.TunerSetup;
		h75Dev->pTnrModeData->TunerAcquisitionType = BTNR_7550_TunerAcquisitionType_eInitTune;
		BTNR_7550_P_Tune_TNR_Core0(h75Dev);
	}
	else if ((pInitSettings.Bandwidth != TunerBw) || (pInitSettings.Freq != TunerFreq))
	{
		h75Dev->pTnrModeData->TunerBw = pInitSettings.Bandwidth;
		h75Dev->pTnrModeData->TunerFreq = pInitSettings.Freq;
		h75Dev->pTnrModeData->TunerAcquisitionType = BTNR_7550_TunerAcquisitionType_eTune;
		BTNR_7550_P_Tune_TNR_Core0(h75Dev);
	}
	else
	{
		h75Dev->pTnrModeData->TunerAcquisitionType = BTNR_7550_TunerAcquisitionType_eMiniTune;
		BTNR_7550_P_Tune_TNR_Core0(h75Dev);
	}
	TunerAcquisitionMode = pInitSettings.AcquisitionMode;
	TunerFreq = pInitSettings.Freq;
	TunerBw = pInitSettings.Bandwidth;
	TunerSetup = pInitSettings.TunerSetup;

	if (h75Dev->pTnrModeData->TunerAcquisitionType != BTNR_7550_TunerAcquisitionType_eMiniTune)
	{
		BTMR_StopTimer(h75Dev->hTimer);
		BTMR_StartTimer(h75Dev->hTimer, 500000);   /* the timer is in Micro second */
		BDBG_MSG(("BTNR_7550_Tune\n"));
	}

	return BERR_SUCCESS;
}


/***************************************************************************
Summary:
	BTNR_7550_SetRFByPassMode
****************************************************************************/
BERR_Code BTNR_7550_SetRFByPassMode(
    BTNR_Handle hTnr,
    bool mode
    )
{

	BTNR_P_75xx_Handle *h75Dev=((BTNR_P_75xx_Handle *)(hTnr->hDevImpl));
	h75Dev->pTnrModeData->RFbypassMode = mode;
	BTNR_7550_P_Set_Rf_ByPass_mode(h75Dev, mode);
	return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	BTNR_7550_GetRFByPassMode
****************************************************************************/
BERR_Code BTNR_7550_GetRFByPassMode(
    BTNR_Handle hTnr,
    bool *mode
    )
{

	BTNR_P_75xx_Handle *h75Dev=((BTNR_P_75xx_Handle *)(hTnr->hDevImpl));
	*mode = h75Dev->pTnrModeData->RFbypassMode;
	return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	BTNR_7550_GetTunerStatus
****************************************************************************/
BERR_Code BTNR_7550_GetTunerStatus(
    BTNR_Handle hTnr,
    BTNR_7550_TunerStatus *pStatus
    )
{

	BTNR_P_75xx_Handle *h75Dev=((BTNR_P_75xx_Handle *)(hTnr->hDevImpl));
	BTNR_7550_P_Get_Status_TNR_Core0(h75Dev);
	pStatus->rfAgcVal = (int32_t)(h75Dev->pTnrStatus->RF_AGC_LEVEL);
	pStatus->ifAgcVal = (int32_t)(h75Dev->pTnrStatus->IF_AGC_LEVEL);
	pStatus->lnaAgcVal = (int32_t)(h75Dev->pTnrStatus->LNA_AGC_LEVEL);
	pStatus->agcStatus = (int32_t)(h75Dev->pTnrStatus->Tuner_AGC_Status);
	if ((h75Dev->pTnrModeData->TunerAcquisitionMode == BTNR_7550_Standard_eDVBT) || (h75Dev->pTnrModeData->TunerAcquisitionMode == BTNR_7550_Standard_eISDBT))
		pStatus->signalStrength = (int32_t)(h75Dev->pTnrStatus->InputPower_256db);
	if ((h75Dev->pTnrModeData->TunerAcquisitionMode == BTNR_7550_Standard_eQAM) && (h75Dev->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable))
	{	
		pStatus->dpmLvel = (int32_t)(h75Dev->pTnrStatus->DPM_Gain_256dbmV);
	}
	else
		pStatus->dpmLvel = 0;

	return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	BTNR_7550_GetDPM
****************************************************************************/
BERR_Code BTNR_7550_EnableDPM(
    BTNR_Handle hTnr
    )
{
	BTNR_P_75xx_Handle *h75Dev=((BTNR_P_75xx_Handle *)(hTnr->hDevImpl));
	BTNR_7550_P_Get_DPM(h75Dev);
	return BERR_SUCCESS;
}

/***************************************************************************
BTNR_InstallDeviceInterruptCallback: Used to enable and install an application
                                     callback for Device relevant Interrupt.
****************************************************************************/
BERR_Code BTNR_7550_InstallAdsInterruptCallback(BTNR_Handle						hDev,
												BTNR_7550_AdsInterruptCallback  fCallBack_isr,
												void                            *pParam1,
												int					            param2
												)
{
   BTNR_P_75xx_Handle *p75xx = (BTNR_P_75xx_Handle *)(hDev->hDevImpl);
   BTNR_P_CallbackInfo *callback;

   BDBG_ENTER(BTNR_InstallDeviceInterruptCallback);
   callback =&(p75xx->InterruptCallbackInfo);

   BKNI_EnterCriticalSection();
   callback->func = (BTNR_7550_AdsInterruptCallback)fCallBack_isr;
   callback->pParm1 = pParam1;
   callback->Parm2 = param2;
   BKNI_LeaveCriticalSection();
   

   BDBG_LEAVE(BTNR_InstallDeviceInterruptCallback);

   return BERR_TRACE(BERR_SUCCESS);
}


/******************************************************************************
BTNR_75xx_UnInstallInterruptCallback()
******************************************************************************/ 
BERR_Code BTNR_7550_RemoveAdsInterruptCallback(
    BTNR_Handle handle  /* [in] BTNR handle */
)
{
    BTNR_P_CallbackInfo *callback;
    BTNR_P_75xx_Handle *p75xx = (BTNR_P_75xx_Handle *)(handle->hDevImpl);

    BDBG_ASSERT(handle);
    
    callback = &(p75xx->InterruptCallbackInfo);

    callback->func = NULL;
    callback->pParm1 = NULL;
    callback->Parm2 = (int)NULL;

    return BERR_TRACE(BERR_SUCCESS);
} 

/***************************************************************************
Summary:
	BTNR_7550_SetLNABoost
****************************************************************************/
BERR_Code BTNR_7550_SetLNABoost(
    BTNR_Handle hTnr,
    bool on
    )
{
    BTNR_P_75xx_Handle *h75Dev=((BTNR_P_75xx_Handle *)(hTnr->hDevImpl));

    if (h75Dev->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable)
        h75Dev->pTnrModeData->BBS_DS_LNA_Boost_On = (uint32_t)(on == true) ? BTNR_ON : BTNR_OFF;
    else if (h75Dev->pTnrModeData->TunerSetup == BTNR_7550_Setup_eTerrestrial)
        h75Dev->pTnrModeData->BBS_THD_LNA_Boost_On = (uint32_t)(on == true) ? BTNR_ON : BTNR_OFF;
    else
        BDBG_ERR(("Invalid TunerSetup setting!"));

    BTNR_7550_P_Set_LNA_Boost(h75Dev);
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	BTNR_7550_SetLNATilt
****************************************************************************/
BERR_Code BTNR_7550_SetLNATilt(
    BTNR_Handle hTnr,
    bool on
    )
{
    BTNR_P_75xx_Handle *h75Dev=((BTNR_P_75xx_Handle *)(hTnr->hDevImpl));

    if (h75Dev->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable)
        h75Dev->pTnrModeData->BBS_DS_LNA_Tilt_On = (uint32_t)(on == true) ? BTNR_ON : BTNR_OFF;
    else if (h75Dev->pTnrModeData->TunerSetup == BTNR_7550_Setup_eTerrestrial)
        h75Dev->pTnrModeData->BBS_THD_LNA_Tilt_On = (uint32_t)(on == true) ? BTNR_ON : BTNR_OFF;
    else
        BDBG_ERR(("Invalid TunerSetup setting!"));

    BTNR_7550_P_Set_LNA_Tilt(h75Dev);
    return BERR_SUCCESS;
}

