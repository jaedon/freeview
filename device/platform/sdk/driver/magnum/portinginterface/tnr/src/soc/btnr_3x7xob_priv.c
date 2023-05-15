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
* $brcm_Workfile: btnr_3x7xob_priv.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 9/7/12 10:26a $
*
* [File Description:]
*
* Revision History:
*
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/btnr_3x7xob_priv.c $
 * 
 * Hydra_Software_Devel/1   9/7/12 10:26a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/4   3/12/12 1:07p farshidf
 * SW3461-1: add the AOB to tuner callback
 * 
 * Hydra_Software_Devel/3   12/14/11 2:48p farshidf
 * SW7552-170: remove the tuner power hard code values
 * 
 * Hydra_Software_Devel/2   12/13/11 5:49p farshidf
 * SW7552-170: temp code to get it working
 * 
 * Hydra_Software_Devel/1   12/8/11 11:28p farshidf
 * SW7552-170: first version of OOB tuner for B0 verification
*
***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "bdbg.h"
#include "btnr_priv.h"
#include "btmr.h"
#include "btnr_ob_struct.h"
#include "btnr_3x7xob_priv.h"
#include "btnr_ob_init.h"
#include "btnr_ob_tune.h"
#include "bchp_ufe_misc2.h"
#include "bchp_ufe.h"
#include "bchp_ufe_oob.h"
BDBG_MODULE(btnr_3x7xob_priv);

#define DEV_MAGIC_ID                    ((BERR_TNR_ID<<16) | 0xFACE)


/******************************************************************************
* BTNR_Ob_3x7x_Close
******************************************************************************/
BERR_Code BTNR_Ob_3x7x_Close(
    BTNR_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
	BTNR_Ob_P_3x7x_Handle *p3x7x;

    BDBG_ENTER(BTNR_Ob_3x7x_Close);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    /* verify the handle is good before using it */
	p3x7x = (BTNR_Ob_P_3x7x_Handle *)(hDev->hDevImpl);

    hDev->magicId = 0x00;       /* clear it to catch inproper use */
	p3x7x->pTunerStatus->PowerStatus = BTNR_Ob_ePower_Off;
	BTMR_DestroyTimer(p3x7x->hTimer);
	BKNI_DestroyEvent(p3x7x->hInterruptEvent);
	BMEM_Free( p3x7x->hHeap, p3x7x->pTunerParams );
    BMEM_Free( p3x7x->hHeap, p3x7x->pTunerStatus );
    BKNI_Free( (void *) hDev->hDevImpl );
    BKNI_Free( (void *) hDev );

    BDBG_LEAVE(BTNR_Ob_3x7x_Close);
    return( retCode );
}
/***************************************************************************
Summary:
	BTNR_Ob_3x7x_SetTuneSettings
****************************************************************************/
BERR_Code BTNR_Ob_3x7x_SetSettings(
		  BTNR_Ob_3x7x_Handle hDev,    /* [in] Device handle */
		  BTNR_Settings *settings     /* [in] TNR settings. */
		)		
{    
	BERR_Code retCode = BERR_SUCCESS;
	BDBG_ENTER(BTNR_Ob_3x7x_SetSettings);
    BDBG_ASSERT( hDev );
	BSTD_UNUSED(settings);
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	if (BTNR_Ob_ePower_On == hDev->pTunerStatus->PowerStatus) 
	{
		BTNR_Ob_3x7x_PowerUp(hDev);
	}	
    BDBG_LEAVE(BTNR_Ob_3x7x_SetSettings);
    return retCode;
}

/***************************************************************************
Summary:
	BTNR_Ob_3x7x_GetSettings
****************************************************************************/
BERR_Code BTNR_Ob_3x7x_GetSettings(
    BTNR_Ob_3x7x_Handle hDev,    /* [in] Device handle */
    BTNR_Settings *settings     /* [out] TNR settings. */
    )
{   
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_Ob_3x7x_GetSettings);
    BDBG_ASSERT( hDev );
	BSTD_UNUSED(settings);
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BDBG_LEAVE(BTNR_Ob_3x7x_GetSettings);
    return retCode;
}

/***************************************************************************
Summary:
	BTNR_Ob_3x7x_SetRfFreq
****************************************************************************/
BERR_Code BTNR_Ob_3x7x_SetRfFreq(
    BTNR_Ob_3x7x_Handle hDev,            /* [in] Device handle */
    uint32_t rfFreq,                    /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode            /* [in] Requested tuner mode */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BSTD_UNUSED(tunerMode);
	BDBG_ENTER(BTNR_Ob_3x7x_SetRfFreq);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if (hDev->pTunerStatus->PowerStatus != BTNR_Ob_ePower_On)
   {
		BDBG_ERR(("BTNR_Ob_3x7x_SetRfFreq: power is still off  "));
		return BERR_NOT_INITIALIZED;
   }

   BREG_Write32(hDev->hRegister, BCHP_UFE_OOB_SPARE, hDev->pTunerParams->BTNR_Ob_BBS_Params.BBSAddress);
   /* Important: Init Tuner before TNR */
   BTNR_Ob_P_TunerInit(hDev);
   hDev->pTunerParams->BTNR_Ob_BBS_Params.rfFreq = rfFreq;
   BTNR_Ob_P_TnrInit(hDev, rfFreq);
 
	BTMR_StopTimer(hDev->hTimer);
	BTMR_StartTimer(hDev->hTimer, 500000);
    BDBG_LEAVE(BTNR_Ob_3x7x_SetRfFreq);
    return retCode;
}

/***************************************************************************
Summary:
	BTNR_Ob_3x7x_GetRfFreq
****************************************************************************/
BERR_Code BTNR_Ob_3x7x_GetRfFreq(
    BTNR_Ob_3x7x_Handle hDev,            /* [in] Device handle */
    uint32_t *rfFreq,                   /* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode           /* [output] Returns tuner mode */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

	BDBG_MSG(("BTNR_Ob_3x7x_GetRfFreq"));
    BDBG_ENTER(BTNR_Ob_3x7x_GetRfFreq);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );


    *rfFreq =  hDev->pTunerParams->BTNR_Ob_BBS_Params.rfFreq;
    *tunerMode = BTNR_TunerMode_eDigital;

    BDBG_LEAVE(BTNR_Ob_3x7x_GetRfFreq);
    return retCode;
}
/***************************************************************************
Summary:
	BTNR_Ob_3x7x_GetInfo
****************************************************************************/

BERR_Code BTNR_Ob_3x7x_GetInfo(
    BTNR_Ob_3x7x_Handle hDev,            /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo             /* [out] Tuner information */
    )
{   
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_Ob_3x7x_GetInfo);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	BDBG_MSG(("BTNR_Ob_3x7x_GetInfo"));
    tnrInfo->tunerMaker = 1;
    tnrInfo->tunerId = 0x7552;
    tnrInfo->tunerMajorVer = 0;
    tnrInfo->tunerMinorVer = 0;

    BDBG_LEAVE(BTNR_Ob_3x7x_GetInfo);
    return retCode;
}


/***************************************************************************
 * BTNR_Ob_3x7x_PowerUp()
 ***************************************************************************/
BERR_Code BTNR_Ob_3x7x_PowerUp (BTNR_Ob_3x7x_Handle hTnr)
{
    BERR_Code eResult = BERR_SUCCESS;
	BDBG_MSG(("BTNR_Ob_3x7x_PowerUp"));
    BREG_Write32(hTnr->hRegister, BCHP_UFE_MISC2_CLK_RESET, 0x0);  /* This must be AFTER UFE reset*/   
    
    BTNR_Ob_P_Tuner_Power_Control(hTnr);
    hTnr->pTunerStatus->PowerStatus = BTNR_Ob_ePower_On;
	
	BREG_Write32(hTnr->hRegister, BCHP_UFE_LFSR_SEED , 0);
	return ( eResult );
}

/***************************************************************************
 * BTNR_Ob_3x7x_PowerDown()
 ***************************************************************************/
BERR_Code BTNR_Ob_3x7x_PowerDown(BTNR_Ob_3x7x_Handle hTnr)
{

	/*BREG_Write32(hTnr->hRegister, BCHP_UFE_MISC2_CLK_RESET,  0x7); */
	BKNI_Memset( hTnr->pTunerParams, 0x00, sizeof( BTNR_Ob_3x7x_TuneParams_t ) );
	hTnr->pTunerStatus->PowerStatus = BTNR_Ob_ePower_Off;
	return BERR_SUCCESS;
}


/***************************************************************************
Summary:
	BTNR_Ob_3x7x_SetPowerSaver
****************************************************************************/
BERR_Code BTNR_Ob_3x7x_SetPowerSaver(
    BTNR_Ob_3x7x_Handle hDev,                    /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings /* [in] Power saver settings. */
    )
{   
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_Ob_3x7x_SetPowerSaver);
    
    if(!pwrSettings->enable)
		BTNR_Ob_3x7x_PowerUp(hDev);
    else 
		BTNR_Ob_3x7x_PowerDown(hDev);
     
    BDBG_LEAVE(BTNR_Ob_3x7x_SetPowerSaver);
    return retCode;
}
/***************************************************************************
 * BTNR_Ob_3x7x_GetPowerSaver()
 ***************************************************************************/
BERR_Code BTNR_Ob_3x7x_GetPowerSaver(
    BTNR_Ob_3x7x_Handle hDev,                    /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings        /* [in] Power saver settings. */
    )
{   
    BERR_Code retCode = BERR_SUCCESS;
	BDBG_ENTER(BTNR_Ob_3x7x_GetPowerSaver);

    pwrSettings->enable = hDev->pTunerStatus->PowerStatus;

    BDBG_LEAVE(BTNR_Ob_3x7x_GetPowerSaver);
    return retCode;
}



