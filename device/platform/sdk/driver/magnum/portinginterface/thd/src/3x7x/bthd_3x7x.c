/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthd_3x7x.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 11/15/12 2:07p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/thd/src/3x7x/bthd_3x7x.c $
 * 
 * Hydra_Software_Devel/2   11/15/12 2:07p farshidf
 * SW7552-205: fix compile issue
 * 
 * Hydra_Software_Devel/1   9/7/12 9:33a farshidf
 * SW7425-3819: Remove symbolic links and flatten out THD directory
 * structure
 * 
 * Hydra_Software_Devel/6   8/6/12 4:06p farshidf
 * SW7552-247: update to new API
 * 
 * Hydra_Software_Devel/5   5/24/12 1:55p farshidf
 * SW3461-1: fix the TS mode
 * 
 * Hydra_Software_Devel/4   3/2/12 2:10p farshidf
 * SW7552-223: implement 7552 THD Frontend PM
 * 
 * Hydra_Software_Devel/3   2/9/12 2:13p farshidf
 * SW7552-205: port to new 4.0  code
 * 
 * Hydra_Software_Devel/2   2/2/12 6:34p atanugul
 * SW3461-141: Add initializers for BTHD_SetAcquireParams
 * BTHD_GetAcquireParams and BTHD_PowerUp
 * 
 * Hydra_Software_Devel/1   10/10/11 2:13p farshidf
 * SW7552-134: update to 2.0 of 3461 code
 * 
 * Hydra_Software_Devel/7   8/16/11 6:11p farshidf
 * SW7552-105: update the frontend with 3461_V1_0_RC0 label
 * 
 * Hydra_Software_Devel/6   8/16/11 6:04p farshidf
 * SW7552-105: update the frontend with 3461_V1_0_RC0 label
 * 
 * Hydra_Software_Devel/5   6/10/11 2:44p farshidf
 * SWDTV-7504: add the new isdbt flag
 * 
 * Hydra_Software_Devel/4   6/2/11 10:16a atanugul
 * SW3461-5: fix compilation error
 * 
 * Hydra_Software_Devel/3   6/1/11 5:42p atanugul
 * SW3461-5: Add initializer for BTHD_RequestThdAsyncStatus,
 * BTHD_GetThdAsyncStatus, BTHD_InstallCallback
 * 
 * Hydra_Software_Devel/2   4/27/11 12:13p farshidf
 * SWDTV-6190: add the BBS call from Nexus
 * 
 * Hydra_Software_Devel/1   4/21/11 6:39p farshidf
 * SWDTV-6191: add the THD code
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bthd.h"
#include "bthd.h"
#include "bthd_3x7x.h"
#include "bthd_api.h"


BDBG_MODULE(bthd_3x7x);


static const BTHD_InbandParams defInbandParams = 
{
  BTHD_InbandMode_eDvbt,         /* Inband mode */
  BTHD_Bandwidth_8Mhz,            /* Bandwidth */
  666000000,                      /* Tuner Frequency */
  36000000,                       /* IF Frequency */
  BTHD_CCI_Auto,                  /* Co-channel filter options */
  BTHD_PullInRange_eWide,         /* Pull-in range */
  true,                           /* Mode/Guard auto acquire */
  BTHD_TransmissionMode_e2K,      /* Manual Transmission mode (FFT Size) */
  BTHD_GuardInterval_e1_4,        /* Manual guard interval selection */
  false,						  /* acquire after tune  does not apply to 7552*/
  BTHD_Decode_Hp,                 /* Decode mode */
  false,                          /* TPS Acquire */
  BTHD_Modulation_e64Qam,         /* Modulation type */
  BTHD_CodeRate_e1_2,             /* Manual coderate LP */
  BTHD_CodeRate_e1_2,             /* Manual coderate HP */
  BTHD_Hierarchy_0,               /* Hierarchy selection */
  true,							  /* ISDB-T TMCC acquire */
  false,						  /* ISDB-T partial reception */
  BTHD_Modulation_e16Qam,		  /* ISDB-T Layer A modulation type */
  BTHD_CodeRate_e1_2,			  /* ISDB-T Layer A code rate */
  BTHD_IsdbtTimeInterleaving_0X,  /* ISDB-T Layer A time interleaving */
  13,                             /*ISDB-T Layer A number of segments*/
  BTHD_Modulation_e16Qam,         /* ISDB-T Layer B modulation type */
  BTHD_CodeRate_e1_2,             /* ISDB-T Layer B code rate */
  BTHD_IsdbtTimeInterleaving_0X,  /* ISDB-T Layer B time interleaving */
  0,                              /*ISDB-T Layer A number of segments*/
  BTHD_Modulation_e16Qam,         /* ISDB-T Layer B modulation type */
  BTHD_CodeRate_e1_2,             /* ISDB-T Layer C code rate */
  BTHD_IsdbtTimeInterleaving_0X,  /* ISDB-T Layer C time interleaving*/
  0,                               /* ISDB-T Layer C number of segments*/
  BTHD_ThdAcquisitionMode_eAuto    /* Auto Acquire */
};




/***************************************************************************
 * BTHD_3x7x_GetDefaultInbandParams
 ***************************************************************************/
BERR_Code BTHD_3x7x_GetDefaultInbandParams(BTHD_InbandParams *pDefSettings)
{
  *pDefSettings = defInbandParams; 
  return BERR_SUCCESS;
}

/***************************************************************************
 * BTHD_7550_GetDefaultSettings()
 ***************************************************************************/
BERR_Code BTHD_3x7x_GetDefaultSettings(BTHD_Settings *pDefSettings)
{
  BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTHD_3x7x_GetDefaultSettings);
  
	BKNI_Memset(pDefSettings, 0, sizeof(*pDefSettings));
	
    pDefSettings->api.Open										= BTHD_3x7x_Open;
	pDefSettings->api.Close										= BTHD_3x7x_Close;
	pDefSettings->api.Init										= BTHD_3x7x_InitializeParams;
	pDefSettings->api.TuneAcquire								= BTHD_3x7x_Acquire;
	pDefSettings->api.GetThdStatus								= BTHD_3x7x_GetStatus;
	pDefSettings->api.ResetInbandStatus							= BTHD_3x7x_ResetStatus;
	pDefSettings->api.GetInterruptEventHandle					= BTHD_3x7x_GetInterruptEventHandle;
	pDefSettings->api.ProcessInterruptEvent						= BTHD_3x7x_ProcessInterruptEvent;
	pDefSettings->api.GetLockStateChangeEventHandle				= BTHD_3x7x_GetLockStateChangeEvent;
	pDefSettings->api.GetSoftDecisionBuf						= BTHD_3x7x_GetConstellation;
	pDefSettings->api.GetDefaultInbandParams					= BTHD_3x7x_GetDefaultInbandParams;
	pDefSettings->api.PowerUp									= BTHD_3x7x_PowerUp;
	pDefSettings->api.PowerDown									= BTHD_3x7x_PowerDown,	
    pDefSettings->api.GetEWSEventHandle							= BTHD_3x7x_GetEWSEventHandle,
    pDefSettings->api.GetThdLockStatus							= BTHD_3x7x_GetLockStatus,
	pDefSettings->api.GetBBSInterruptEventHandle				= BTHD_3x7x_GetBBSInterruptEventHandle,
	pDefSettings->api.ProcessBBSInterruptEvent					= BTHD_3x7x_ProcessBBSInterruptEvent,
	pDefSettings->api.InstallCallback							= BTHD_3x7x_InstallCallback,

    BDBG_LEAVE(BTHD_3x7x_GetDefaultSettings);
    return( retCode );
  return BERR_SUCCESS;
}


