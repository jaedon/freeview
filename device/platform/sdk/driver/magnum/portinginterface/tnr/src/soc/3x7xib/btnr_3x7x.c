/*************************************************************************
*     (c)2005-2011 Broadcom Corporation
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
* $brcm_Workfile: btnr_3x7x.c $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 9/24/12 3:34p $
*
* [File Description:]
*
* Revision History:
*
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/3x7xib/btnr_3x7x.c $
 * 
 * Hydra_Software_Devel/4   9/24/12 3:34p farshidf
 * SW7425-3987: Replace uncache memory access with cache memory access
 * 
 * Hydra_Software_Devel/3   9/24/12 3:07p farshidf
 * SW7425-3987: Replace uncache memory access with cache memory access
 * 
 * Hydra_Software_Devel/2   9/24/12 3:04p farshidf
 * SW7425-3987: Replace uncache memory access with cache memory access
 * 
 * Hydra_Software_Devel/1   9/7/12 10:23a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
* 
***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "bdbg.h"
#include "bmem.h"
#include "btnr_priv.h"
#include "btmr.h"
#include "btnr_3x7x.h"
#include "btnr_3x7xib_priv.h"
#include "bchp_ufe.h"
#include "bchp_ufe_afe.h"
#include "bchp_sdadc.h"
#include "bchp_ufe_misc2.h"
#include "btnr_tune.h"
#include "btnr_struct.h"
#include "btnr_callback.h"


BDBG_MODULE(btnr_3x7x);

#define DEV_MAGIC_ID                    ((BERR_TNR_ID<<16) | 0xFACE)

/******************************************************************************
* BTNR_3x7x_Open() 
******************************************************************************/
BERR_Code BTNR_3x7x_Open(
    BTNR_Handle *phDev,                 /* [output] Returns handle */
    BTNR_3x7x_Settings *pSettings, /* [Input] settings structure */ 
	BREG_Handle hRegister
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_3x7x_Handle h3x7xDev;
	BTMR_Settings sTimerSettings;
    BTNR_Handle hDev;
	uint32_t BufSrc;
	void *cached_ptr, *tmpAddress;
    BDBG_ENTER(BTNR_3x7x_Open);
  
	   BDBG_MSG(("BTNR_3x7x_Open"));
    
    hDev = NULL;
    /* Alloc memory from the system heap */
    h3x7xDev = (BTNR_3x7x_Handle) BKNI_Malloc( sizeof( BTNR_P_3x7x_Handle ) );
    if( h3x7xDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_3x7x_Open: BKNI_malloc() failed\n"));
        goto done;
    }
    BKNI_Memset( h3x7xDev, 0x00, sizeof( BTNR_P_3x7x_Handle ) );

    hDev = (BTNR_Handle) BKNI_Malloc( sizeof( BTNR_P_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BTNR_3x7x_Open: BKNI_malloc() failed\n"));
        BKNI_Free( h3x7xDev );
        goto done;
    }
    BKNI_Memset( hDev, 0x00, sizeof( BTNR_P_Handle ) );


    tmpAddress = (BTNR_3x7x_TuneParams_t *)BMEM_AllocAligned(pSettings->hHeap, sizeof(BTNR_3x7x_TuneParams_t), 0, 0 );
	if (tmpAddress == NULL )
	{
			BDBG_ERR(("BTNR_Open: BKNI_malloc() failed"));
			goto done;
	}
	BMEM_ConvertAddressToCached(pSettings->hHeap, tmpAddress, &cached_ptr);
	BKNI_Memset(cached_ptr, 0x00, sizeof( BTNR_3x7x_TuneParams_t ) );
	h3x7xDev->pTunerParams = cached_ptr;
	BMEM_FlushCache(pSettings->hHeap, h3x7xDev->pTunerParams, sizeof( BTNR_3x7x_TuneParams_t ) );
	BMEM_ConvertAddressToOffset(pSettings->hHeap, tmpAddress, &BufSrc );
	h3x7xDev->pTunerParams->BTNR_Local_Params.TunerBBSaddress = BufSrc;
	h3x7xDev->pTunerParams->BTNR_Local_Params.TunerApplication = 0xf;
	h3x7xDev->pTunerParams->BTNR_Local_Params.RfInputMode = 0xf;

	tmpAddress = (BTNR_3x7x_TuneStatus_t *)BMEM_AllocAligned(pSettings->hHeap, sizeof(BTNR_3x7x_TuneStatus_t), 0, 0 ); 
	if (tmpAddress == NULL )
	{
			BDBG_ERR(("BTNR_Open: BKNI_malloc() failed"));
			goto done;
	}
	BMEM_ConvertAddressToCached(pSettings->hHeap, tmpAddress, &cached_ptr);
	BKNI_Memset(cached_ptr, 0x00, sizeof( BTNR_3x7x_TuneStatus_t ) );
	h3x7xDev->pTunerStatus = cached_ptr;
	BMEM_FlushCache(pSettings->hHeap, h3x7xDev->pTunerStatus, sizeof( BTNR_3x7x_TuneStatus_t ) );
	BMEM_ConvertAddressToOffset(pSettings->hHeap, tmpAddress, &BufSrc );
	h3x7xDev->pTunerParams->BTNR_BBS_Params.StartSturctureAddress = BufSrc;

	h3x7xDev->magicId = DEV_MAGIC_ID;
	h3x7xDev->hHeap = pSettings->hHeap;
	h3x7xDev->hRegister = hRegister;

#if BCHP_TNR_CORE_V_1_1
	h3x7xDev->pTunerParams->BTNR_Local_Params.RevId  = BCHP_VER_B0;
#endif

	hDev->hDevImpl = (void *) h3x7xDev;
	hDev->magicId = DEV_MAGIC_ID;
	hDev->pSetRfFreq = (BTNR_SetRfFreqFunc) BTNR_3x7x_SetRfFreq;
	hDev->pGetRfFreq = (BTNR_GetRfFreqFunc) BTNR_3x7x_GetRfFreq;
	hDev->pGetAgcRegVal = (BTNR_GetAgcRegValFunc) NULL;
	hDev->pSetAgcRegVal = (BTNR_SetAgcRegValFunc) NULL;
	hDev->pGetInfo = (BTNR_GetInfoFunc) BTNR_3x7x_GetInfo;
	hDev->pClose = (BTNR_CloseFunc) BTNR_3x7x_Close;
	hDev->pGetPowerSaver = (BTNR_GetPowerSaverFunc) BTNR_3x7x_GetPowerSaver;
	hDev->pSetPowerSaver = (BTNR_SetPowerSaverFunc) BTNR_3x7x_SetPowerSaver;
	hDev->pGetSettings = (BTNR_GetSettingsFunc) BTNR_3x7x_GetSettings;
    hDev->pSetSettings = (BTNR_SetSettingsFunc) BTNR_3x7x_SetSettings;   

	BKNI_CreateEvent(&(h3x7xDev->hIntEvent));

	    /* Create timer for status lock check */
    BTMR_GetDefaultTimerSettings(&sTimerSettings);
    sTimerSettings.type = BTMR_Type_ePeriodic;
    sTimerSettings.cb_isr = (BTMR_CallbackFunc)BTNR_3x7x_P_TimerFunc;
    sTimerSettings.pParm1 = (void*)hDev;
    sTimerSettings.parm2 = 0;
    sTimerSettings.exclusive = false;

    retCode = BTMR_CreateTimer (pSettings->hTmr, &h3x7xDev->hTmr, &sTimerSettings);
    if ( retCode != BERR_SUCCESS )
    {
        BDBG_ERR(("BTHD_Open: Create Timer Failed"));
        retCode = BERR_TRACE(retCode);
        goto done;
    }
#if 0
                BREG_Write32(h3x7xDev->hRegister, BCHP_SUN_TOP_CTRL_SW_INIT_1_SET,  0x00000080); 
                BREG_Write32(h3x7xDev->hRegister, BCHP_SUN_TOP_CTRL_SW_INIT_1_CLEAR,  0x00000080); 
                
                BREG_WriteField(h3x7xDev->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_BIAS, 1);
                BREG_WriteField(h3x7xDev->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_SDADC_REG1p0, 0x1);
                BREG_WriteField(h3x7xDev->hRegister, UFE_AFE_TNR0_PWRUP_02, PHY_PLL_master_PWRUP, 0x1);
                BREG_WriteField(h3x7xDev->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch, 0x20);
                BREG_WriteField(h3x7xDev->hRegister, UFE_AFE_TNR0_RESET_01, PHYPLL_dreset, 0x0);
    BREG_Write32(hRegister, BCHP_UFE_MISC2_CLK_RESET, 0x0);
    BREG_Write32(hRegister, BCHP_SDADC_CTRL_PWRUP, 0x3);
    BREG_Write32(hRegister, BCHP_SDADC_CTRL_RESET, 0x0);
                

                /* power down the UFE reg */
                BREG_WriteField(h3x7xDev->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_SDADC_REG1p0, 0x0);
                BREG_WriteField(h3x7xDev->hRegister, UFE_AFE_TNR0_PWRUP_02, PHY_PLL_master_PWRUP, 0x0);
                BREG_WriteField(h3x7xDev->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch, 0x0);
                BREG_WriteField(h3x7xDev->hRegister, UFE_AFE_TNR0_RESET_01, PHYPLL_dreset, 0x01);
    BREG_Write32(hRegister, BCHP_UFE_MISC2_CLK_RESET, 0x7);
    BREG_Write32(hRegister, BCHP_SDADC_CTRL_PWRUP, 0x0);
    BREG_Write32(hRegister, BCHP_SDADC_CTRL_RESET, 0x3);
                BREG_WriteField(h3x7xDev->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_BIAS, 0x0);
#endif

	h3x7xDev->pTunerStatus->PowerStatus = BTNR_ePower_Off;
done:
    *phDev = hDev;
    BDBG_LEAVE(BTNR_3x7x_Open);
    return( retCode );
}


/******************************************************************************
* BTNR_3x7x_GetDefaultSettings
******************************************************************************/
BERR_Code BTNR_3x7x_GetDefaultSettings(
    BTNR_3x7x_Settings *pDefSettings  /* [out] Returns default setting */
    )
{
    BDBG_ASSERT(NULL != pDefSettings);
    return BERR_SUCCESS;
}


/******************************************************************************
  BTNR_3x7x_P_TimerFunc()
 ******************************************************************************/
BERR_Code BTNR_3x7x_P_TimerFunc(void *myParam1, int myParam2)
{
    BTNR_Handle hDev = (BTNR_Handle)myParam1;
	BTNR_P_3x7x_Handle *p3x7x = (BTNR_P_3x7x_Handle *)(hDev->hDevImpl);
	BSTD_UNUSED(myParam2);
	BTNR_P_TunerAGCMonitor(p3x7x);
	BKNI_SetEvent(p3x7x->hIntEvent); 
	return BERR_SUCCESS;
}

/******************************************************************************
  BTNR_3x7x_GetInterruptEventHandle()
 ******************************************************************************/
BERR_Code BTNR_3x7x_GetInterruptEventHandle(BTNR_Handle h, BKNI_EventHandle* hEvent)
{
    *hEvent = ((BTNR_P_3x7x_Handle *)(h->hDevImpl))->hIntEvent;
    return BERR_SUCCESS;
}

/******************************************************************************
  BTNR_3x7x_ProcessInterruptEvent()
 ******************************************************************************/
BERR_Code BTNR_3x7x_ProcessInterruptEvent(BTNR_Handle hTnrDev)
{
  BTNR_Settings pInitSettings;
  uint32_t RF_Freq;
  BTNR_P_3x7x_Handle *hDev = (BTNR_P_3x7x_Handle *)(hTnrDev->hDevImpl);

   if (hDev->pTunerStatus->PowerStatus != BTNR_ePower_On)
   {
		BDBG_MSG(("BTNR_3x7x_ProcessInterruptEvent: power is still off  "));
		return BERR_NOT_INITIALIZED;
   }
  BTMR_StopTimer(hDev->hTmr);
  /* BTNR_P_TunerAGCMonitor(hDev); *//* to make sure tuner AGC are up to date since they rely on AGC for twiking */
  BMEM_FlushCache(hDev->hHeap, hDev->pTunerParams, sizeof( BTNR_3x7x_TuneParams_t ) );

  BDBG_MSG(("BTNR_3x7x_P_TimerFunc %0x  ", hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode ));
  if (hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode & BTNR_BBSConnectMode_EnableStatus)
  {
	  BDBG_MSG(("ProcessInterruptEvent: BTNR_BBSConnectMode_EnableStatus"));

	hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode &= (~BTNR_BBSConnectMode_EnableStatus);
  }
 
  if (hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode & BTNR_BBSConnectMode_Tune)
  {
	hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode &= (~BTNR_BBSConnectMode_Tune);
	BDBG_MSG(("1. ProcessInterruptEvent: BTNR_BBSConnectMode_Tune %d %d %d ",hDev->pTunerParams->BTNR_BBS_Params.LPF_Bandwidth, hDev->pTunerParams->BTNR_BBS_Params.Application, hDev->pTunerParams->BTNR_BBS_Params.BTNR_RF_Input_Mode));
    pInitSettings.rfInputMode = hDev->pTunerParams->BTNR_BBS_Params.BTNR_RF_Input_Mode;

	switch (hDev->pTunerParams->BTNR_BBS_Params.Standard)
	{
		case BTNR_Standard_eDVBT:
			pInitSettings.std = BTNR_Standard_eDvbt;
			break;
		case BTNR_Standard_eISDBT:
			pInitSettings.std = BTNR_Standard_eIsdbt;
			break;
		case BTNR_Standard_eQAM:
			pInitSettings.std = BTNR_Standard_eQam;
			break;
		case BTNR_Standard_eT2:
			pInitSettings.std = BTNR_Standard_eDvbt2;
			break;
	}

	switch (hDev->pTunerParams->BTNR_BBS_Params.LPF_Bandwidth)
	{
		case 1:
			pInitSettings.bandwidth = 8000000;
			break;
		case 2:
			pInitSettings.bandwidth = 7000000;
			break;
		case 3:
			pInitSettings.bandwidth = 6000000;
			break;
		case 4:
			pInitSettings.bandwidth = 5000000;
			break;
		case 5:
			pInitSettings.bandwidth = 1700000;
			break;
		case 6:
			pInitSettings.bandwidth = hDev->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth;
			break;
	}
	switch (hDev->pTunerParams->BTNR_BBS_Params.Application)
	{
		case BTNR_TunerApplicationMode_eCable:
			pInitSettings.tnrApplication = BTNR_TunerApplication_eCable;
			break;
		case BTNR_TunerApplicationMode_eTerrestrial:
			pInitSettings.tnrApplication = BTNR_TunerApplication_eTerrestrial;
			break;
	}
	BTNR_3x7x_SetSettings(hDev, &pInitSettings);
	RF_Freq = hDev->pTunerParams->BTNR_Acquire_Params.RF_Freq ;
		BDBG_MSG(("ProcessInterruptEvent: BTNR_BBSConnectMode_Tune %d %d ", pInitSettings.bandwidth ,pInitSettings.tnrApplication));
	BTNR_3x7x_SetRfFreq(hDev,RF_Freq, BTNR_TunerMode_eDigital);
  }

  if (hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode & BTNR_BBSConnectMode_RfMode)
  {
	  hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode &= (~BTNR_BBSConnectMode_RfMode);
  }
  if (hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode & BTNR_BBSConnectMode_EnableLoop)
  {
	  hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode &= (~BTNR_BBSConnectMode_EnableLoop);
	  BDBG_MSG(("ProcessInterruptEvent: BTNR_BBSConnectMode_EnableLoop = %d", hDev->pTunerParams->BTNR_BBS_Params.BTNR_LoopThru_Params.LoopThru_Enable ));	
	  hDev->pTunerParams->BTNR_LoopThru_Params.LoopThru_Source = hDev->pTunerParams->BTNR_BBS_Params.BTNR_LoopThru_Params.LoopThru_Source; 
	  BTNR_3x7x_Set_RF_LoopThrough(hTnrDev, hDev->pTunerParams->BTNR_BBS_Params.BTNR_LoopThru_Params.LoopThru_Enable);
  }
  if (hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode & BTNR_BBSConnectMode_EnableDaisy)
  {
	 hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode &= (~BTNR_BBSConnectMode_EnableDaisy);
	 BDBG_MSG(("ProcessInterruptEvent: BTNR_BBSConnectMode_EnableDaisy = %d", hDev->pTunerParams->BTNR_BBS_Params.BTNR_Daisy_Params.Daisy_Enable ));
	 hDev->pTunerParams->BTNR_Daisy_Params.Daisy_Source = hDev->pTunerParams->BTNR_BBS_Params.BTNR_Daisy_Params.Daisy_Source; 
	 hDev->pTunerParams->BTNR_Daisy_Params.Daisy_Enable = hDev->pTunerParams->BTNR_BBS_Params.BTNR_Daisy_Params.Daisy_Enable; 
	 BTNR_P_Daisy_Control(hDev);
  }

  if (hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode & BTNR_BBSConnectMode_EnableDPM)
  {
	  hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode &= (~BTNR_BBSConnectMode_EnableDPM);	
	  BDBG_MSG(("ProcessInterruptEvent: BTNR_BBSConnectMode_EnableDPM = %d", hDev->pTunerParams->BTNR_BBS_Params.BTNR_DPM_Params.DPM_Enable));	
	  hDev->pTunerParams->BTNR_DPM_Params.DPM_Enable = hDev->pTunerParams->BTNR_BBS_Params.BTNR_DPM_Params.DPM_Enable; 
	  hDev->pTunerParams->BTNR_DPM_Params.DPM_Target = hDev->pTunerParams->BTNR_BBS_Params.BTNR_DPM_Params.DPM_Target; 
	  hDev->pTunerParams->BTNR_DPM_Params.DPM_Freq = hDev->pTunerParams->BTNR_BBS_Params.BTNR_DPM_Params.DPM_Freq;
	  BTNR_P_DPM_Control(hDev);
  }

   if (hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode & BTNR_BBSConnectMode_ResetStatus)
  {
	 BDBG_MSG(("ProcessInterruptEvent: BTNR_BBSConnectMode_ResetStatus"));	
	 hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode &= (~BTNR_BBSConnectMode_ResetStatus);
	 BMEM_FlushCache(hDev->hHeap, hDev->pTunerStatus, sizeof( BTNR_3x7x_TuneStatus_t ) );
	 BTNR_P_TunerStatusReset(hDev);
  }

  switch (hDev->pTunerParams->BTNR_Acquire_Params.Application)
  {
	case BTNR_TunerApplicationMode_eTerrestrial:		
		BTMR_StartTimer(hDev->hTmr, 500000);   /* the timer is in Micro second */
	break;
	case BTNR_TunerApplicationMode_eCable:
		BTMR_StartTimer(hDev->hTmr, 100000);   /* the timer is in Micro second */
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid hDev->pTunerParams->BTNR_Acquire_Params.Application, value received is %d",hDev->pTunerParams->BTNR_Acquire_Params.Application));
  }	
  /*BDBG_MSG(("End BTNR_3x7x_P_TimerFunc %0x s= %0x", hDev->pTunerParams->BTNR_BBS_Params.BBSConnectMode, hDev->pTunerStatus->Lock ));*/

  return BERR_SUCCESS;

}

/******************************************************************************
  BTNR_3x7x_Set_RF_Offset()
 ******************************************************************************/
BERR_Code BTNR_3x7x_Set_RF_Offset(BTNR_Handle hTnrDev, int32_t RF_Offset, uint32_t Symbol_Rate)
{
	BTNR_P_3x7x_Handle *hDev = (BTNR_P_3x7x_Handle *)(hTnrDev->hDevImpl);
   if (hDev->pTunerStatus->PowerStatus != BTNR_ePower_On)
   {
		BDBG_ERR(("BTNR_3x7x_Set_RF_Offset: power is still off  "));
		return BERR_NOT_INITIALIZED;
   }
	BTNR_P_Set_RF_Offset(hDev, RF_Offset, Symbol_Rate);
	return BERR_SUCCESS;
}

/******************************************************************************
  BTNR_3x7x_Get_RF_Status()
 ******************************************************************************/
BERR_Code BTNR_3x7x_Get_RF_Status(BTNR_Handle hTnrDev, BTNR_3x7x_RfStatus_t *RfCallbackStatus)
{
	BTNR_P_3x7x_Handle *hTnr = (BTNR_P_3x7x_Handle *)(hTnrDev->hDevImpl);
    if (hTnr->pTunerStatus->PowerStatus != BTNR_ePower_On)
    {
		BDBG_ERR(("BTNR_3x7x_ProcessInterruptEvent: power is still off  "));
		return BERR_NOT_INITIALIZED;
    }
	BTNR_P_Get_RF_Status(hTnr);
	RfCallbackStatus->RF_Freq = hTnr->pTunerStatus->Tuner_RF_Freq;
    RfCallbackStatus->Total_Mix_After_ADC = hTnr->pTunerParams->BTNR_Local_Params.Total_Mix_After_ADC;
    RfCallbackStatus->PreADC_Gain_x256db  = hTnr->pTunerStatus->Tuner_PreADC_Gain_x256db;
    RfCallbackStatus->PostADC_Gain_x256db = hTnr->pTunerParams->BTNR_Local_Params.PostADC_Gain_x256db;
    RfCallbackStatus->External_Gain_x256db = hTnr->pTunerStatus->External_Gain_x256db;  

	RfCallbackStatus->RF_Offset = hTnr->pTunerParams->BTNR_Local_Params.RF_Offset;
    RfCallbackStatus->Symbol_Rate = hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate ;  

	return BERR_SUCCESS;
}

/******************************************************************************
  BTNR_3x7x_Set_RF_LoopThrough()
 ******************************************************************************/
BERR_Code BTNR_3x7x_Set_RF_LoopThrough(BTNR_Handle hTnrDev, bool EnableRfLoopThrough)
{
	BTNR_P_3x7x_Handle *hTnr = (BTNR_P_3x7x_Handle *)(hTnrDev->hDevImpl);
	
	if (EnableRfLoopThrough)
		hTnr->pTunerParams->BTNR_LoopThru_Params.LoopThru_Enable = BTNR_LoopThru_Params_eEnable;
	else
		hTnr->pTunerParams->BTNR_LoopThru_Params.LoopThru_Enable = BTNR_LoopThru_Params_eDisable;

	BTNR_P_LoopThru_Control(hTnr);
	return BERR_SUCCESS;
}


/******************************************************************************
  BTNR_3x7x_Get_RF_LoopThrough()
 ******************************************************************************/
BERR_Code BTNR_3x7x_Get_RF_LoopThrough(BTNR_Handle hTnrDev, bool EnableRfLoopThrough)
{
	BTNR_P_3x7x_Handle *hTnr = (BTNR_P_3x7x_Handle *)(hTnrDev->hDevImpl);
	
	if (hTnr->pTunerParams->BTNR_LoopThru_Params.LoopThru_Enable == BTNR_LoopThru_Params_eEnable)
		EnableRfLoopThrough = true;
	else
		EnableRfLoopThrough = false;

	return BERR_SUCCESS;
}



