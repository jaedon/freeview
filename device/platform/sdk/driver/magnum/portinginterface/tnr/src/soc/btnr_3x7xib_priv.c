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
* $brcm_Workfile: btnr_3x7xib_priv.c $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 9/21/12 3:51p $
*
* [File Description:]
*
* Revision History:
*
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/btnr_3x7xib_priv.c $
 * 
 * Hydra_Software_Devel/6   9/21/12 3:51p farshidf
 * SW7552-336: re-structure TNR folder to point to AP tnr code
 * 
 * Hydra_Software_Devel/5   9/10/12 12:09p farshidf
 * SW7425-3765: clean up after Removing symbolic links and flatten out TNR
 * directory structure
 * 
 * Hydra_Software_Devel/4   9/10/12 12:03p farshidf
 * SW7425-3765: clean up after Removing symbolic links and flatten out TNR
 * directory structure
 * 
 * Hydra_Software_Devel/3   9/7/12 3:34p farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/2   9/7/12 2:51p farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/1   9/7/12 10:26a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/18   8/29/12 2:40p farshidf
 * SW7552-336: re-structure TNR folder to point to AP tnr code
 * 
 * Hydra_Software_Devel/17   7/13/12 5:17p farshidf
 * SW7552-302: stop the rror meassge
 * 
 * Hydra_Software_Devel/16   7/5/12 4:34p farshidf
 * SW7552-295: fix BBS/BRUTUS issues for UFE reset when channel changed
 * 
 * Hydra_Software_Devel/15   7/3/12 4:23p farshidf
 * SW3461-120: rewrite the UFE spare register at each tune since the
 * register get reset everytime the UFE block is reset
 * 
 * Hydra_Software_Devel/14   6/13/12 11:00a farshidf
 * SW7552-288: update 7552 with the latest Tuner code from 3462 (it will
 * be labeled REFSW_TNR_7552_06132012)
 * 
 * Hydra_Software_Devel/13   5/23/12 5:14p farshidf
 * SW3461-1: update the tuner AGC monitoring
 * 
 * Hydra_Software_Devel/12   5/21/12 7:01p farshidf
 * SW3461-213: add the External LNA settings from Dave
 * 
 * Hydra_Software_Devel/11   5/21/12 6:25p farshidf
 * SW3461-213: Make the Tuner Branch 5.0 to compile for 7552
 * 
 * Hydra_Software_Devel/10   3/5/12 4:59p farshidf
 * SW7552-223: TNR power up/down
 * 
 * Hydra_Software_Devel/9   2/13/12 2:37p farshidf
 * SW3461-140: update the tuner status field for next tune comparason
 * 
 * Hydra_Software_Devel/8   11/10/11 4:21p farshidf
 * SW7552-144: fix the loopthr.daisy,dpm
 * 
 * Hydra_Software_Devel/7   11/9/11 6:17p farshidf
 * SW7552-144: update the enum
 * 
 * Hydra_Software_Devel/6   10/24/11 3:36p farshidf
 * SW7552-134: write to UFE reg after power up
 * 
 * Hydra_Software_Devel/5   8/30/11 5:57p farshidf
 * SW7552-112: add the suuport for BBS to switch to different power mode
 * 
 * Hydra_Software_Devel/4   8/26/11 5:04p farshidf
 * SW7552-102: fix the tuner issue to lock the first time
 * 
 * Hydra_Software_Devel/3   7/28/11 3:54p farshidf
 * SW3461-1: reset the LAN usage register
 * 
 * Hydra_Software_Devel/2   6/12/11 1:18p farshidf
 * SW7552-36: clean up
 * 
 * Hydra_Software_Devel/1   6/12/11 12:30p farshidf
 * SW7552-36: code clean up
* 
***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "bdbg.h"
#include "btnr_priv.h"
#include "btmr.h"
#include "btnr_struct.h"
#include "btnr_3x7xib_priv.h"
#include "btnr_init.h"
#include "btnr_tune.h"
#include "bchp_ufe_misc2.h"
#include "bchp_ufe.h"
#include "bchp_ufe_afe.h"
#include "bchp_sdadc.h"
#include "bchp_gio_aon.h"
BDBG_MODULE(btnr_3x7xib_priv);

#define DEV_MAGIC_ID                    ((BERR_TNR_ID<<16) | 0xFACE)


/******************************************************************************
* BTNR_3x7x_Close
******************************************************************************/
BERR_Code BTNR_3x7x_Close(
    BTNR_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_3x7x_Handle btnr_3x7x_handle;
	BTNR_P_3x7x_Handle *p3x7x;

    BDBG_ENTER(BTNR_3x7x_Close);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    /* verify the handle is good before using it */
    btnr_3x7x_handle = (BTNR_3x7x_Handle) hDev->hDevImpl;
	p3x7x = (BTNR_P_3x7x_Handle *)(hDev->hDevImpl);

    hDev->magicId = 0x00;       /* clear it to catch inproper use */
	BTMR_DestroyTimer(p3x7x->hTmr);
	BKNI_DestroyEvent(p3x7x->hIntEvent);
	BMEM_Free( p3x7x->hHeap, p3x7x->pTunerParams );
    BMEM_Free( p3x7x->hHeap, p3x7x->pTunerStatus );
    BKNI_Free( (void *) hDev->hDevImpl );
    BKNI_Free( (void *) hDev );

    BDBG_LEAVE(BTNR_3x7x_Close);
    return( retCode );
}
/***************************************************************************
Summary:
	BTNR_3x7x_SetTuneSettings
****************************************************************************/
BERR_Code BTNR_3x7x_SetSettings(
		  BTNR_3x7x_Handle hDev,    /* [in] Device handle */
		  BTNR_Settings *settings     /* [in] TNR settings. */
		)		
{    
	BERR_Code retCode = BERR_SUCCESS;
	BTNR_TunerApplicationMode_t TunerApplication=0;
	BDBG_ENTER(BTNR_3x7x_SetSettings);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

   if (hDev->pTunerStatus->PowerStatus != BTNR_ePower_On)
   {
		BDBG_ERR(("BTNR_3x7x_SetRfFreq: power is still off  "));
		return BERR_NOT_INITIALIZED;
   }

	

	switch (settings->tnrApplication)
	{
		case BTNR_TunerApplication_eCable:
			TunerApplication = BTNR_TunerApplicationMode_eCable;
			break;
		case BTNR_TunerApplication_eTerrestrial:
			TunerApplication = BTNR_TunerApplicationMode_eTerrestrial;
			break;
		case BTNR_TunerApplication_eLast:
			TunerApplication = BTNR_TunerApplicationMode_eTerrestrial;
			break;
	}
			
	hDev->pTunerParams->BTNR_Acquire_Params.Application = TunerApplication;	
	switch (settings->bandwidth)
	{
		case 8000000:
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth = BTNR_LPF_Bandwidth_e8MHz;
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth = 0;
			break;
		case 7000000:
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth = BTNR_LPF_Bandwidth_e7MHz;
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth = 0;
			break;
		case 6000000:
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth = BTNR_LPF_Bandwidth_e6MHz;
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth = 0;
			break;
		case 5000000:
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth = BTNR_LPF_Bandwidth_e5MHz;
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth = 0;
			break;
		case 1700000:
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth = BTNR_LPF_Bandwidth_e1_7MHz;
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth = 0;
			break;   
		default:
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth = BTNR_LPF_Bandwidth_eVariable;
			hDev->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth = settings->bandwidth;
			break;
	}

	switch (settings->std)
	{
		case BTNR_Standard_eDvbt:
			hDev->pTunerParams->BTNR_Acquire_Params.Standard= BTNR_Standard_eDVBT;
			break;
		case BTNR_Standard_eIsdbt:
			hDev->pTunerParams->BTNR_Acquire_Params.Standard= BTNR_Standard_eISDBT;
			break;
		case BTNR_Standard_eQam:
			hDev->pTunerParams->BTNR_Acquire_Params.Standard= BTNR_Standard_eQAM;
			break;
		case BTNR_Standard_eDvbt2:
			hDev->pTunerParams->BTNR_Acquire_Params.Standard= BTNR_Standard_eT2;
			break; 
		default:
			hDev->pTunerParams->BTNR_Acquire_Params.Standard= BTNR_Standard_eQAM;
			break;
	}
	hDev->pTunerParams->BTNR_RF_Input_Mode = settings->rfInputMode;

	if (TunerApplication != hDev->pTunerParams->BTNR_Local_Params.TunerApplication) 
		hDev->pTunerParams->BTNR_TuneType.TuneType = BTNR_TuneType_eInitTune;
	else
		hDev->pTunerParams->BTNR_TuneType.TuneType = BTNR_TuneType_eTune;

	if ((BTNR_ePower_On == hDev->pTunerStatus->PowerStatus) && (settings->rfInputMode != (BTNR_RfInputMode)hDev->pTunerParams->BTNR_Local_Params.RfInputMode))
	{
		BTNR_P_TunerSetInputMode(hDev);
		BDBG_MSG(("Switch RF Input Mode  RFInput = %d  App = %d", settings->rfInputMode, hDev->pTunerParams->BTNR_Acquire_Params.Application));
	}

	hDev->pTunerParams->BTNR_Local_Params.TunerApplication = TunerApplication;
	hDev->pTunerParams->BTNR_Local_Params.RfInputMode = settings->rfInputMode;
	hDev->pTunerParams->BTNR_BBS_Params.BTNR_RF_Input_Mode = settings->rfInputMode;


    BDBG_LEAVE(BTNR_3x7x_SetSettings);
    return retCode;
}

/***************************************************************************
Summary:
	BTNR_3x7x_GetSettings
****************************************************************************/
BERR_Code BTNR_3x7x_GetSettings(
    BTNR_3x7x_Handle hDev,    /* [in] Device handle */
    BTNR_Settings *settings     /* [out] TNR settings. */
    )
{   
    BERR_Code retCode = BERR_SUCCESS;
	BTNR_TunerApplication TunerApplication=0;

    BDBG_ENTER(BTNR_3x7x_GetSettings);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	switch (hDev->pTunerParams->BTNR_Acquire_Params.Standard)
	{
		case BTNR_Standard_eDVBT:
			settings->std = BTNR_Standard_eDvbt;
			break;
		case BTNR_Standard_eISDBT:
			settings->std = BTNR_Standard_eIsdbt;
			break;
		case BTNR_Standard_eQAM:
			settings->std = BTNR_Standard_eQam;
			break;
		case BTNR_Standard_eT2:
			settings->std = BTNR_Standard_eDvbt2;
			break; 
		default:
			settings->std = BTNR_Standard_eQam;
			break;
	}
	switch (hDev->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth)
	{
		case 1:
			settings->bandwidth = 8000000;
			break;
		case 2:
			settings->bandwidth = 7000000;
			break;
		case 3:
			settings->bandwidth = 6000000;
			break;
		case 4:
			settings->bandwidth = 5000000;
			break;
		case 5:
			settings->bandwidth = 1700000;
			break;
		case 6:
			settings->bandwidth = hDev->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth;
			break;
	}
	switch (hDev->pTunerParams->BTNR_Acquire_Params.Application)
	{
		case BTNR_TunerApplicationMode_eCable:
			TunerApplication = BTNR_TunerApplication_eCable;
			break;
		case BTNR_TunerApplicationMode_eTerrestrial:
			TunerApplication = BTNR_TunerApplication_eTerrestrial;
			break;
	}

    settings->tnrApplication = TunerApplication;
    settings->rfInputMode = hDev->pTunerParams->BTNR_RF_Input_Mode;

    BDBG_LEAVE(BTNR_3x7x_GetSettings);
    return retCode;
}

/***************************************************************************
Summary:
	BTNR_3x7x_SetRfFreq
****************************************************************************/
BERR_Code BTNR_3x7x_SetRfFreq(
    BTNR_3x7x_Handle hDev,            /* [in] Device handle */
    uint32_t rfFreq,                    /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode            /* [in] Requested tuner mode */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BSTD_UNUSED(tunerMode);
	BDBG_ENTER(BTNR_3x7x_SetRfFreq);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );


   if (hDev->pTunerStatus->PowerStatus != BTNR_ePower_On)
   {
		BDBG_ERR(("BTNR_3x7x_SetRfFreq: power is still off  "));
		return BERR_NOT_INITIALIZED;
   }
   BDBG_MSG(("BTNR_3x7x_Tune: TUNE TIME START"));
   hDev->pTunerParams->BTNR_Internal_Params.LNA_Enable = BTNR_Internal_Params_eEnable;

   hDev->pTunerParams->BTNR_Acquire_Params.RF_Freq = rfFreq;
 
   if (hDev->pTunerParams->BTNR_TuneType.TuneType == BTNR_TuneType_eInitTune)
	{
       /* Important: Init Tuner before TNR */
		BDBG_MSG(("BTNR_3x7x_Tune: BTNR_TuneType_eInitTune %0x %0x %0x %0x %0x", hDev->pTunerParams->BTNR_Acquire_Params.Application, hDev->pTunerParams->BTNR_Acquire_Params.Standard,  hDev->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth,  hDev->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth,  hDev->pTunerParams->BTNR_Acquire_Params.RF_Freq ));
        BTNR_P_TunerInit(hDev);
		BTNR_P_TunerTune(hDev);
		BTNR_P_TnrInit(hDev);
	}
	else if (hDev->pTunerParams->BTNR_TuneType.TuneType == BTNR_TuneType_eTune)
	{
		BDBG_MSG(("BTNR_3x7x_Tune: BTNR_TuneType_eTune %0x %0x %0x %0x %0x", hDev->pTunerParams->BTNR_Acquire_Params.Application, hDev->pTunerParams->BTNR_Acquire_Params.Standard,  hDev->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth,  hDev->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth,  hDev->pTunerParams->BTNR_Acquire_Params.RF_Freq ));
 		BTNR_P_TunerTune(hDev);
		BTNR_P_TnrInit(hDev);
		
	}
	else if (hDev->pTunerParams->BTNR_TuneType.TuneType == BTNR_TuneType_eMiniTune)
	{
		BDBG_MSG(("BTNR_3x7x_Tune: BTNR_TuneType_eMiniTune"));
		BTNR_P_TnrMiniInit(hDev);    /* just reset the data path */
		BTNR_P_TunerTune(hDev);
	}
	 BREG_Write32(hDev->hRegister, BCHP_UFE_SPARE, hDev->pTunerParams->BTNR_Local_Params.TunerBBSaddress);

	/* make sure the status fileds are up to date since Dave is using staus field to know if new Tune will be required SW3461-140*/
	BTMR_StopTimer(hDev->hTmr);
	BTNR_P_TunerAGCMonitor(hDev);
	BDBG_MSG(("BTNR_3x7x_Tune: TUNE TIME COMPLETE"));

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
    BDBG_LEAVE(BTNR_3x7x_SetRfFreq);
    return retCode;
}

/***************************************************************************
Summary:
	BTNR_3x7x_GetRfFreq
****************************************************************************/
BERR_Code BTNR_3x7x_GetRfFreq(
    BTNR_3x7x_Handle hDev,            /* [in] Device handle */
    uint32_t *rfFreq,                   /* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode           /* [output] Returns tuner mode */
    )
{
    BERR_Code retCode = BERR_SUCCESS;


    BDBG_ENTER(BTNR_3x7x_GetRfFreq);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );


    *rfFreq =  hDev->pTunerParams->BTNR_Acquire_Params.RF_Freq;
    *tunerMode = BTNR_TunerMode_eDigital;

    BDBG_LEAVE(BTNR_3x7x_GetRfFreq);
    return retCode;
}
/***************************************************************************
Summary:
	BTNR_3x7x_GetInfo
****************************************************************************/

BERR_Code BTNR_3x7x_GetInfo(
    BTNR_3x7x_Handle hDev,            /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo             /* [out] Tuner information */
    )
{   
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_3x7x_GetInfo);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    tnrInfo->tunerMaker = 1;
    tnrInfo->tunerId = 0x7552;
    tnrInfo->tunerMajorVer = 0;
    tnrInfo->tunerMinorVer = 0;

    BDBG_LEAVE(BTNR_3x7x_GetInfo);
    return retCode;
}


/***************************************************************************
 * BTNR_3x7x_PowerUp()
 ***************************************************************************/
BERR_Code BTNR_3x7x_PowerUp (BTNR_3x7x_Handle hTnr)
{
    uint32_t temp;     
	BERR_Code eResult = BERR_SUCCESS;
	
	if (hTnr->pTunerStatus->PowerStatus == BTNR_ePower_On)
		return eResult;
 
    BREG_WriteField(hTnr->hRegister, UFE_MISC2_CLK_RESET, CLK_SDADC_RESET, 0x0);  /* This must be AFTER UFE reset*/   
  	hTnr->pTunerParams->BTNR_Internal_Params.LNA_Enable = BTNR_Internal_Params_eEnable;

    hTnr->pTunerParams->BTNR_Internal_Params.LNA_Enable = BTNR_Internal_Params_eEnable;
    BTNR_P_Tuner_PowerUpPLL(hTnr);
    hTnr->pTunerStatus->PowerStatus = BTNR_ePower_On;
  
	temp = BREG_Read32(hTnr->hRegister, BCHP_GIO_AON_DATA_LO);
    temp = temp | 0x00000010;
    BREG_Write32(hTnr->hRegister, BCHP_GIO_AON_DATA_LO, temp);     
    hTnr->pTunerStatus->External_FGLNA_Mode = 1;

	BREG_Write32(hTnr->hRegister, BCHP_UFE_LFSR_SEED , 0);
	return ( eResult );
}

/***************************************************************************
 * BTNR_3x7x_PowerDown()
 ***************************************************************************/
BERR_Code BTNR_3x7x_PowerDown(BTNR_3x7x_Handle hTnr)
{
	uint32_t BBS_add = hTnr->pTunerParams->BTNR_Local_Params.TunerBBSaddress;
	
	BDBG_MSG(("BTNR_3x7x_PowerDown"));
	
    BREG_Write32(hTnr->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01, 0x0);
    BREG_Write32(hTnr->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_01, 0x0);
    BREG_Write32(hTnr->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02, 0x0);
    BREG_Write32(hTnr->hRegister, BCHP_UFE_AFE_TNR0_RESET_01, 0xFFFFFFFF);
    BREG_Write32(hTnr->hRegister, BCHP_UFE_AFE_TNR0_RESETB_01, 0x0);
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AFE_TNR_RESET_01, 0xFFFFFFFF);
    BREG_Write32(hTnr->hRegister, BCHP_UFE_AFE_TNR_RESETB_01, 0x0);
    BREG_Write32(hTnr->hRegister, BCHP_UFE_MISC2_CLK_RESET, 0x7);
    BREG_Write32(hTnr->hRegister, BCHP_SDADC_CTRL_PWRUP, 0x0);

	BKNI_Memset( hTnr->pTunerParams, 0x00, sizeof( BTNR_3x7x_TuneParams_t ) );
	hTnr->pTunerParams->BTNR_Local_Params.TunerBBSaddress = BBS_add;
    BTMR_StopTimer(hTnr->hTmr);
	hTnr->pTunerStatus->PowerStatus = BTNR_ePower_Off;
	return BERR_SUCCESS;
}


/***************************************************************************
Summary:
	BTNR_3x7x_SetPowerSaver
****************************************************************************/
BERR_Code BTNR_3x7x_SetPowerSaver(
    BTNR_3x7x_Handle hDev,                    /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings /* [in] Power saver settings. */
    )
{   
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_3x7x_SetPowerSaver);
    
    if(!pwrSettings->enable)
		BTNR_3x7x_PowerUp(hDev);
    else 
		BTNR_3x7x_PowerDown(hDev);
     
    BDBG_LEAVE(BTNR_3x7x_SetPowerSaver);
    return retCode;
}
/***************************************************************************
 * BTNR_3x7x_GetPowerSaver()
 ***************************************************************************/
BERR_Code BTNR_3x7x_GetPowerSaver(
    BTNR_3x7x_Handle hDev,                    /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings        /* [in] Power saver settings. */
    )
{   
    BERR_Code retCode = BERR_SUCCESS;
	BDBG_ENTER(BTNR_3x7x_GetPowerSaver);

    pwrSettings->enable = hDev->pTunerStatus->PowerStatus;

    BDBG_LEAVE(BTNR_3x7x_GetPowerSaver);
    return retCode;
}



