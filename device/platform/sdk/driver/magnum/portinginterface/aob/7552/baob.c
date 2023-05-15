/**************************************************************************
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
 * $brcm_Workfile: baob.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 9/24/12 3:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/aob/7552/baob.c $
 * 
 * Hydra_Software_Devel/21   9/24/12 3:09p farshidf
 * SW7425-3633: Replace uncache memory access with cache memory access
 * 
 * Hydra_Software_Devel/20   9/24/12 2:47p farshidf
 * SW7425-3633: Replace uncache memory access with cache memory access
 * 
 * Hydra_Software_Devel/19   9/24/12 2:30p farshidf
 * SW7425-3633: Replace uncache memory access with cache memory access
 * 
 * Hydra_Software_Devel/18   3/12/12 10:35a farshidf
 * SW3128-1: add callback to tuner through Nexus
 * 
 * Hydra_Software_Devel/17   3/1/12 12:01p farshidf
 * SW7574-105: 7572 Constellation function
 * 
 * Hydra_Software_Devel/16   3/1/12 11:57a farshidf
 * SW7574-105: 7572 Constellation function
 * 
 * Hydra_Software_Devel/15   2/29/12 12:05p farshidf
 * SW7574-105: 7572 Constellation function
 * 
 * Hydra_Software_Devel/14   2/1/12 5:57p farshidf
 * SW3128-1: fix the OOb reset issue
 * 
 * Hydra_Software_Devel/13   1/18/12 4:37p farshidf
 * SW7574-69: fix the BBS interface
 * 
 * Hydra_Software_Devel/12   1/16/12 4:17p farshidf
 * SW7574-69: fix the get lock status call
 * 
 * Hydra_Software_Devel/11   1/12/12 6:59p farshidf
 * SW7574-69: fix the get lock status call
 * 
 * Hydra_Software_Devel/10   1/12/12 5:43p farshidf
 * SW7574-69: fix the get lock status call
 * 
 * Hydra_Software_Devel/9   1/2/12 2:05p farshidf
 * SW7552-170: update lock status update
 * 
 * Hydra_Software_Devel/8   12/16/11 5:49p farshidf
 * SW7552-170: remove the LSFR_ENABLE bit settings
 * 
 * Hydra_Software_Devel/7   12/15/11 11:45a farshidf
 * SW7552-170: add the BBS support
 * 
 * Hydra_Software_Devel/6   12/14/11 4:07p farshidf
 * SW7552-170: fix the OOB callback
 * 
 * Hydra_Software_Devel/5   12/14/11 3:58p farshidf
 * SW7552-170: fix the AOB interface
 * 
 * Hydra_Software_Devel/4   12/14/11 10:40a farshidf
 * SW7552-170: add the reset
 * 
 * Hydra_Software_Devel/3   12/12/11 2:38p farshidf
 * SW7552-170: add get default settings
 * 
 * Hydra_Software_Devel/2   12/12/11 12:01p farshidf
 * SW7552-170: fix the AOB pi
 * 
 * Hydra_Software_Devel/1   11/9/11 4:49p farshidf
 * SW7552-139: add aob support
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bads.h"
#include "btmr.h"
#include "bmth.h"
#include "bkni.h"
#include "baob.h"
#include "baob_struct.h"
#include "baob_priv.h"
#include "baob_acquire.h"
#include "baob_status.h"
#include "baob_utils.h"
#include "bchp_oob.h"
#include "bchp_ds_topm.h"
#include "bchp_ds_tops.h"
BDBG_MODULE(baob);

/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BAOB_Settings defDevSettings =
{
    0,                   
    NULL,                       /* Hab handle, must be provided by application*/
    BAOB_SETTINGS_XTALFREQ, 
    false,   
    true,                       /* Default is open drain mode. */
    true,                        /* Default is serial data. */ 
	NULL,
	NULL,
	BAOB_NyquistFilter_eRaisedCosine_50
};
/*******************************************************************************
*   BAOB_3x7x_Open
*******************************************************************************/

BERR_Code BAOB_Open(
	BAOB_Handle *pAob,                  /* [out] Returns handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_Handle hRegister,              /* [in] Register handle */
    BINT_Handle hInterrupt,             /* [in] Interrupt handle */
    const BAOB_Settings *pDefSettings   /* [in] Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BAOB_Handle hDev;
	BTMR_Settings sTimerSettings;
	BAOB_3x7x_Handle hImplDev = NULL;
	uint32_t BufSrc;
	void *cached_ptr, *tmpAddress;
    BDBG_ENTER(BAOB_Open);
    BDBG_MSG(("BAOB_Open"));


    hDev = (BAOB_Handle) BKNI_Malloc( sizeof(BAOB_P_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BAOB_Open: BKNI_malloc() failed"));
        goto done;
    }
    BKNI_Memset( hDev, 0x00, sizeof(BAOB_P_Handle ) );
	BKNI_Memcpy(&hDev->settings, pDefSettings, sizeof(*pDefSettings));

    /* Alloc memory from the system heap */
    hImplDev = (BAOB_3x7x_Handle) BKNI_Malloc( sizeof(BAOB_P_3x7x_Handle ) );
    if( hImplDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BAOB_3x7x_Open: BKNI_malloc() failed"));
        goto done;
    }
    
    BKNI_Memset( hImplDev, 0x00, sizeof(BAOB_P_3x7x_Handle ) );
    hImplDev->magicId = DEV_MAGIC_ID;
	hImplDev->hChip = hChip;
    hImplDev->hRegister = hRegister;
    hImplDev->hInterrupt = hInterrupt;
	hDev->pImpl = hImplDev;

	tmpAddress = (BAOB_AcqParams_t *)BMEM_AllocAligned(pDefSettings->hHeap, sizeof(BAOB_AcqParams_t), 0, 0 );
	if (tmpAddress == NULL )
	{
			BDBG_ERR(("BAOB_Open: BMEM_malloc() failed"));
			goto done;
	}
	BMEM_ConvertAddressToCached(hDev->settings.hHeap, tmpAddress, &cached_ptr);
	BKNI_Memset(cached_ptr, 0x00, sizeof( BAOB_AcqParams_t ) );
	hImplDev->pAcqParam = cached_ptr;
	BMEM_FlushCache(hDev->settings.hHeap, hImplDev->pAcqParam, sizeof( BAOB_AcqParams_t ) );
	BMEM_ConvertAddressToOffset(hDev->settings.hHeap, tmpAddress, &BufSrc );
	hImplDev->pAcqParam->BAOB_Local_Params.BBSAcqAdd = BufSrc;

	tmpAddress = (BAOB_Status_t *)BMEM_AllocAligned(pDefSettings->hHeap, sizeof(BAOB_Status_t), 0, 0 );
	if (tmpAddress == NULL )
	{
			BDBG_ERR(("BAOB_Open: BMEM_malloc() failed"));
			goto done;
	}

	BMEM_ConvertAddressToCached(hDev->settings.hHeap, tmpAddress, &cached_ptr);
	BKNI_Memset(cached_ptr, 0x00, sizeof( BAOB_Status_t ) );
	hImplDev->pStatus = cached_ptr;
	BMEM_FlushCache(hDev->settings.hHeap, hImplDev->pStatus, sizeof( BAOB_Status_t ) );
	BMEM_ConvertAddressToOffset(hDev->settings.hHeap, tmpAddress, &BufSrc );
	hImplDev->pAcqParam->BAOB_Local_Params.BBSStaAdd = BufSrc;

	if (hDev->settings.enableFEC)
		hImplDev->pAcqParam->BAOB_Acquire_Params.BYP = BAOB_Acquire_Params_eEnable;
	else
		hImplDev->pAcqParam->BAOB_Acquire_Params.BYP = BAOB_Acquire_Params_eDisable;

    BKNI_CreateEvent(&(hImplDev->hIntEvent));
	/* Create timer for status lock check */

	sTimerSettings.type = BTMR_Type_ePeriodic;
	sTimerSettings.cb_isr = (BTMR_CallbackFunc)BAOB_P_TimerFunc;
	sTimerSettings.pParm1 = (void*)hDev;
	sTimerSettings.parm2 = 0;
	retCode = BTMR_CreateTimer (hDev->settings.hTmr, &hImplDev->hTimer, &sTimerSettings);
	if ( retCode != BERR_SUCCESS )
	{
	   BDBG_ERR(("BAOB_Open: Create Timer Failed"));
	   goto done;
    }	

    *pAob = hDev;

done:
    if( retCode != BERR_SUCCESS )
    {
        if( hImplDev != NULL )
        {
            BKNI_Free( hImplDev );
        } 
		if( hDev != NULL )
        {
            BKNI_Free( hDev );
        }
        *pAob = NULL;
    }
	BDBG_LEAVE(BAOB_3x7x_Open);
    return( retCode );
}


/*******************************************************************************
*  BAOB_3x7x_Close
*******************************************************************************/

BERR_Code BAOB_Close(
   BAOB_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;
	BDBG_ENTER(BAOB_Close);
	BDBG_ASSERT( hDev );
    BDBG_ASSERT( hImplDev->magicId == DEV_MAGIC_ID );
	
	BTMR_DestroyTimer(hImplDev->hTimer);
	BKNI_DestroyEvent(hImplDev->hIntEvent);
    hImplDev->magicId = 0x00;       /* clear it to catch improper use */
	BMEM_Free(hDev->settings.hHeap ,(void *)(hImplDev->pStatus) );
	BMEM_Free(hDev->settings.hHeap,(void *)(hImplDev->pAcqParam) );
    BKNI_Free( (void *) hImplDev );
	BKNI_Free( (void *) hDev );
	    
	BDBG_LEAVE(BAOB_Close);
    return retCode;
}


/*******************************************************************************
*  BAOB_3x7x_Acquire
*******************************************************************************/
BERR_Code BAOB_Acquire(
    BAOB_Handle hDev,			 /* [in] Device channel handle */
    BAOB_AcquireParam *obParams           /* [in] Device  handle */
    )
{
 
	BERR_Code retCode = BERR_SUCCESS;

	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;
    BDBG_ENTER(BAOB_Acquire);
	BDBG_ASSERT( hDev );
    BDBG_ASSERT( hImplDev->magicId == DEV_MAGIC_ID );
    BDBG_MSG(("BAOB_Acquire"));

	if (hImplDev->PowerStatus != BAOB_ePower_On)
	{
		BDBG_ERR(("!!!!!BAOB_Acquire: The channel is not power on yet "));
        return BERR_NOT_INITIALIZED;
	}
	hImplDev->pAcqParam->BAOB_Acquire_Params.Nyq = hDev->settings.nyquist;
   
	if (obParams->autoAcquire)
		hImplDev->pAcqParam->BAOB_Acquire_Params.Auto =BAOB_Acquire_Params_eEnable ;
	else
		hImplDev->pAcqParam->BAOB_Acquire_Params.Auto = BAOB_Acquire_Params_eDisable;

	if (obParams->spectrum == BAOB_SpectrumMode_eAuto)
		hImplDev->pAcqParam->BAOB_Acquire_Params.AI = BAOB_Acquire_Params_eEnable;
	else
	{
		hImplDev->pAcqParam->BAOB_Acquire_Params.AI = BAOB_Acquire_Params_eDisable;
		if(obParams->spectrum == BAOB_SpectrumMode_eNoInverted)
			hImplDev->pAcqParam->BAOB_Acquire_Params.IS = BAOB_Acquire_Params_eDisable;
		else
			hImplDev->pAcqParam->BAOB_Acquire_Params.IS = BAOB_Acquire_Params_eEnable;
	}

	switch(obParams->berSrc)
	{
		case BAOB_BerInputSrc_eRcvIChOutput:
			hImplDev->pAcqParam->BAOB_Acquire_Params.BERSRC = BAOB_Acquire_Params_BERT_Source_eI; 
			break;
		case BAOB_BerInputSrc_eRcvQChOutput:
			hImplDev->pAcqParam->BAOB_Acquire_Params.BERSRC = BAOB_Acquire_Params_BERT_Source_eQ; 
			break;
		case BAOB_BerInputSrc_eRcvIQChOutputIntrlv:
			hImplDev->pAcqParam->BAOB_Acquire_Params.BERSRC = BAOB_Acquire_Params_BERT_Source_eIQ; 
			break;
		case BAOB_BerInputSrc_eFecOutput:
			hImplDev->pAcqParam->BAOB_Acquire_Params.BERSRC = BAOB_Acquire_Params_BERT_Source_eFEC; 
			break;
		default:
			BDBG_ERR(("Unsupported berSrc"));
	}
	switch(obParams->modType)
	{
		case BAOB_ModulationType_eDvs167Qpsk:
			hImplDev->pAcqParam->BAOB_Acquire_Params.AA = BAOB_Acquire_Params_BPS_eDVS167; 
			break;
		case BAOB_ModulationType_eDvs178Qpsk:
			hImplDev->pAcqParam->BAOB_Acquire_Params.AA = BAOB_Acquire_Params_BPS_eDVS178; 
			break;
		default:
			BDBG_ERR(("Unsupported modulation type"));

	}

	switch (obParams->symbolRate)
	{
		case 1024000:
			hImplDev->pAcqParam->BAOB_Acquire_Params.BPS = BAOB_Acquire_Params_BPS_eDVS_178;
			break;
		case 772000: 
			hImplDev->pAcqParam->BAOB_Acquire_Params.BPS = BAOB_Acquire_Params_BPS_eDVS_167_GradeA;
			break;
		case 1544000: 
			hImplDev->pAcqParam->BAOB_Acquire_Params.BPS = BAOB_Acquire_Params_BPS_eDVS_167_GradeB;
			break;
		default:
			BDBG_ERR(("Unsupported symbolRate %d", obParams->symbolRate));
	}
	hImplDev->pAcqParam->BAOB_Acquire_Params.PLBW = BAOB_Acquire_Params_PLBW_eMed;
	hImplDev->pAcqParam->BAOB_Local_Params.LockUpdate = false;
	hImplDev->pAcqParam->BAOB_Local_Params.LockStatus = 0xFF;
	BAOB_P_Acquire(hImplDev);	

	BTMR_StartTimer(hImplDev->hTimer, 25000);   /* the timer is in Micro second */;
	BDBG_LEAVE(BAOB_Acquire);
	return retCode;
}


/*******************************************************************************
*  BAOB_3x7x_GetStatus (Read status from status struct)
*******************************************************************************/
BERR_Code BAOB_GetStatus(
   BAOB_Handle hDev,          /* [in] Device  handle */
   BAOB_Status *pStatus
    )
{

	BERR_Code retCode = BERR_SUCCESS;
	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;
	BDBG_ENTER(BAOB_3x7x_Status);
	BDBG_ASSERT( hDev );
    BDBG_ASSERT( hImplDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( NULL != (hImplDev->pStatus) );

	if (hImplDev->PowerStatus != BAOB_ePower_On)
	{
		BDBG_ERR(("!!!!!BAOB_GetStatus: The channel is not power on yet "));
        return BERR_NOT_INITIALIZED;
	}
    else if ( NULL == pStatus )
    {
        return BERR_INVALID_PARAMETER;
    }
    else
    {
		BAOB_P_Get_Status(hImplDev);  
		switch(hImplDev->pStatus->AA)
		{
		case BAOB_Acquire_Params_BPS_eDVS167:
			pStatus->modType = BAOB_ModulationType_eDvs167Qpsk;
			break;
		case BAOB_Acquire_Params_BPS_eDVS178:
			pStatus->modType = BAOB_ModulationType_eDvs178Qpsk;
			break;
		}
		pStatus->isFecLock = hImplDev->pStatus->FLK;
		pStatus->symbolRate = hImplDev->pStatus->Symbol_Rate;
		pStatus->sysXtalFreq = 54000000;
		pStatus->snrEstimate = hImplDev->pStatus->SNR;
 	    pStatus->carrierFreqOffset = hImplDev->pStatus->Frequency_Error;
		pStatus->carrierPhaseOffset = 0; /* not supported */
		pStatus->uncorrectedCount = hImplDev->pStatus->FEC_UCorr_RS_Blocks;
		pStatus->correctedCount = hImplDev->pStatus->FEC_Corr_RS_Blocks;
		pStatus->berErrorCount = hImplDev->pStatus->BERT;
		pStatus->isQamLock = hImplDev->pStatus->RLK;

        return BERR_SUCCESS;
    }
	BDBG_LEAVE(BAOB_Status);
	return retCode;

}

/*******************************************************************************
*  BAOB_3x7x_GetLockStatus
*******************************************************************************/
BERR_Code BAOB_GetLockStatus(
   BAOB_Handle hDev,					/* [in] Device  handle */
    bool *isLock                        /* [out] Returns lock status, 0=not lock, 1=locked */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;
    BDBG_ENTER(BAOB_GetLockStatus);
	BDBG_ASSERT( hDev );
    BDBG_ASSERT( hImplDev->magicId == DEV_MAGIC_ID );
	if (hImplDev->PowerStatus != BAOB_ePower_On)
	{
		BDBG_ERR(("!!!!!BAOB_GetLockStatus: The channel is not power on yet "));
        return BERR_NOT_INITIALIZED;
	}
    else
    {
		BAOB_P_GetLock_Status(hImplDev);
        *isLock = (hImplDev->pStatus->RLK && hImplDev->pStatus->FLK);
    }
    BDBG_LEAVE(BAOB_GetLockStatus);
    return( retCode );
}

/*******************************************************************************
*  BAOB_P_GetSoftDecisionBuf
*******************************************************************************/
BERR_Code BAOB_GetSoftDecisionBuf(
    BAOB_Handle hDev,					/* [in] Device channel handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *iVal,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	uint8_t i;
	int32_t ReadReg;
	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;
	BDBG_ENTER(BAOB_GetSoftDecisionBuf);
    BDBG_ASSERT( hDev );


    for (i = 0; i < nbrToGet ; i++)
    {
        ReadReg = BREG_Read32(hImplDev->hRegister, BCHP_OOB_LDSFT);
        iVal[i] = ((ReadReg & 0xff000000) >> 16);    /* still use 16 bit for I and Q value   */           
        qVal[i] = ((ReadReg & 0x00ff0000) >> 8);

    }
    *nbrGotten = nbrToGet;  

    BDBG_LEAVE(BAOB_GetSoftDecisionBuf);
    return( retCode );
}

/*******************************************************************************
*  BAOB_3x7x_DisablePowerSaver
*******************************************************************************/
BERR_Code BAOB_DisablePowerSaver ( 
   BAOB_Handle hDev            /* [in] Device  handle */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;
	BDBG_ENTER(BAOB_DisablePowerSaver);
	BDBG_ASSERT( hDev );
    BDBG_ASSERT( hImplDev->magicId == DEV_MAGIC_ID );
    if (hImplDev->PowerStatus == BAOB_ePower_On)
        return BERR_SUCCESS;
	hImplDev->PowerStatus = BAOB_ePower_On;

	BREG_WriteField(hImplDev->hRegister, DS_TOPM_PD, OOB_60_DISABLE, 0);		/* power up the DS core */	
	BREG_WriteField(hImplDev->hRegister, DS_TOPM_RST_MASK, OB_RSTEN, 1);
	BREG_WriteField(hImplDev->hRegister, DS_TOPM_GBL, RESET, 1);
	BREG_WriteField(hImplDev->hRegister, DS_TOPM_GBL, RESET, 0);
	BKNI_Delay(20);

	/* reset the FE Fifo */
	BREG_WriteField(hImplDev->hRegister, DS_TOPS_CNTL, FIFO_RESET_OB, 1);
	BREG_WriteField(hImplDev->hRegister, DS_TOPS_CNTL, FIFO_RESET_OB, 0);

	BREG_Write32(hImplDev->hRegister, BCHP_DS_TOPS_SW_SPARE2, hImplDev->pAcqParam->BAOB_Local_Params.BBSAcqAdd);
	BREG_Write32(hImplDev->hRegister, BCHP_DS_TOPS_SW_SPARE1, hImplDev->pAcqParam->BAOB_Local_Params.BBSStaAdd);

    BAOB_P_PowerUp(hImplDev);
	
    BDBG_LEAVE(BAOB_DisablePowerSaver);
    return( retCode );
}

/*******************************************************************************
*  BAOB_3x7x_EnablePowerSaver
*******************************************************************************/

BERR_Code BAOB_EnablePowerSaver(
   BAOB_Handle hDev            /* [in] Device  handle */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;
	BDBG_ENTER(BAOB_EnablePowerSaver);
	BDBG_ASSERT( hDev );
    BDBG_ASSERT( hImplDev->magicId == DEV_MAGIC_ID );
    if (hImplDev->PowerStatus == BAOB_ePower_Off)
        return BERR_SUCCESS;
	hImplDev->PowerStatus = BAOB_ePower_Off;
	BAOB_P_PowerDn(hImplDev);

	BDBG_LEAVE(BAOB_EnablePowerSaver);
    return( retCode );
}


/*******************************************************************************
*  BAOB_3x7x_ResetStatus
*******************************************************************************/
BERR_Code BAOB_ResetStatus(
   BAOB_Handle hDev            /* [in] Device  handle */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;
	BDBG_ENTER(BAOB_ResetStatus);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hImplDev->magicId == DEV_MAGIC_ID );
	BAOB_P_Reset_Status(hImplDev);
	BDBG_LEAVE(BAOB_ResetStatus);
    return( retCode );
}

/******************************************************************************
* BAOB_3x7x_P_TimerFunc()
******************************************************************************/
void BAOB_P_TimerFunc(void *myParam1, int myParam2)
{
	BAOB_Handle hDev = (BAOB_Handle)myParam1;
	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;
	BDBG_ASSERT( hImplDev );
	BSTD_UNUSED(myParam2);

	if (hImplDev->PowerStatus != BAOB_ePower_On) 
	{
		BDBG_ERR(("BAOB_P_TimerFunc: power is still off  "));
		return;
	}
	BAOB_P_GetLock_Status(hImplDev);

	if( hImplDev->pCallback[BAOB_Callback_eLockChange] != NULL )
	{
         if (hImplDev->pStatus->FLK)
		 {
			    if (hImplDev->pAcqParam->BAOB_Local_Params.LockUpdate)
				{
					hImplDev->pAcqParam->BAOB_Local_Params.LockUpdate = false;
					(hImplDev->pCallback[BAOB_Callback_eLockChange])(hImplDev->pCallbackParam[BAOB_Callback_eLockChange] );
				}

		 }
		 else
		 {
			    if (hImplDev->pAcqParam->BAOB_Local_Params.LockStatus != hImplDev->pStatus->FLK )
                {	 
					hImplDev->pAcqParam->BAOB_Local_Params.LockUpdate = true;
					(hImplDev->pCallback[BAOB_Callback_eLockChange])(hImplDev->pCallbackParam[BAOB_Callback_eLockChange] );
				}
		 }
	}
	hImplDev->pAcqParam->BAOB_Local_Params.LockStatus = hImplDev->pStatus->FLK;
	BMEM_FlushCache(hDev->settings.hHeap, hImplDev->pAcqParam, sizeof( BAOB_AcqParams_t ) );
	if (hImplDev->pAcqParam->BAOB_Acquire_Params.AcquireStartMode & BAOB_AcquireStartMode_ResetStatus)
	{
		hImplDev->pAcqParam->BAOB_Acquire_Params.AcquireStartMode &= (~BAOB_AcquireStartMode_ResetStatus);
		BMEM_FlushCache(hDev->settings.hHeap, hImplDev->pStatus, sizeof( BAOB_Status_t ) );
		BAOB_P_Reset_Status(hImplDev);
	}
	if (hImplDev->pAcqParam->BAOB_Acquire_Params.AcquireStartMode & BAOB_AcquireStartMode_UpdateStatus)
	{
		hImplDev->pAcqParam->BAOB_Acquire_Params.AcquireStartMode &= (~BAOB_AcquireStartMode_UpdateStatus);
		BMEM_FlushCache(hDev->settings.hHeap, hImplDev->pStatus, sizeof( BAOB_Status_t ) );
		BAOB_P_Get_Status(hImplDev);
	}
	if (hImplDev->pAcqParam->BAOB_Acquire_Params.AcquireStartMode & BAOB_AcquireStartMode_Acquire)
	{
		hImplDev->pAcqParam->BAOB_Acquire_Params.AcquireStartMode &= (~BAOB_AcquireStartMode_Acquire);
		BKNI_SetEvent(hImplDev->hIntEvent);
	}
	else
	{
		if (hImplDev->pAcqParam->BAOB_Acquire_Params.Auto == BAOB_Acquire_Params_eEnable)
		{
		 if (hImplDev->pAcqParam->BAOB_Acquire_Params.BYP == BAOB_Acquire_Params_eEnable)
		 {
		  if (hImplDev->pStatus->RLK == BAOB_Status_eUnlock)
			BKNI_SetEvent(hImplDev->hIntEvent);
		 }
		 else
		 {
		  if (hImplDev->pStatus->FLK == BAOB_Status_eUnlock)
			BKNI_SetEvent(hImplDev->hIntEvent);
		 }
		}
	}
}

/******************************************************************************
*BAOB_3x7x_ProcessInterruptEvent()
******************************************************************************/
BERR_Code BAOB_InstallCallback(
    BAOB_Handle hDev,                   /* [in] Device handle */
    BAOB_Callback callbackType,         /* [in] Type of callback */
    BAOB_CallbackFunc pCallback,        /* [in] Function Ptr to callback */
    void *pParam                        /* [in] Generic parameter send on callback */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;
    BDBG_ENTER(BAOB_InstallCallback);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hImplDev->magicId == DEV_MAGIC_ID );

    switch( callbackType )
    {
        case BAOB_Callback_eLockChange:
            hImplDev->pCallback[callbackType] = pCallback;
            hImplDev->pCallbackParam[callbackType] = pParam;
            break;
		case BAOB_Callback_eTuner:
            hImplDev->pCallback[callbackType] = pCallback;
            hImplDev->pCallbackParam[callbackType] = pParam;
            break;
        
        default:
            retCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }

    BDBG_LEAVE(BAOB_InstallCallback);
    return( retCode );
}

/******************************************************************************
*BAOB_3x7x_ProcessInterruptEvent()
******************************************************************************/

void BAOB_ProcessInterruptEvent (BAOB_Handle hDev)
{	
	BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) hDev->pImpl;

	BTMR_StopTimer(hImplDev->hTimer);  /* the timer is in Micro second */
	BKNI_ResetEvent(hImplDev->hIntEvent);
	BAOB_P_Acquire(hImplDev);	
	BTMR_StartTimer(hImplDev->hTimer, 25000);   /* the timer is in Micro second */;
}

/******************************************************************************
*BAOB_GetDefaultSettings()
******************************************************************************/
BERR_Code BAOB_GetDefaultSettings(
    BAOB_Settings *pDefSettings,        /* [out] Returns default setting */
    BCHP_Handle hChip                   /* [in] Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BSTD_UNUSED(hChip);
    BDBG_ENTER(BAOB_GetDefaultSettings);

    *pDefSettings = defDevSettings;

    BDBG_LEAVE(BAOB_GetDefaultSettings);
    return( retCode );
}



/******************************************************************************
  BAOB_GetInterruptEventHandle()
 ******************************************************************************/
BERR_Code BAOB_GetInterruptEventHandle(BAOB_Handle h, BKNI_EventHandle* hEvent)
{
    BAOB_3x7x_Handle hImplDev = (BAOB_3x7x_Handle) h->pImpl;
	*hEvent = hImplDev->hIntEvent;
    return BERR_SUCCESS;
}

