/*************************************************************************
*     (c)2005-2012 Broadcom Corporation
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
* $brcm_Workfile: btnr_callback.c $
* $brcm_Revision: 13 $
* $brcm_Date: 9/7/12 12:04p $
*
* [File Description:]
*
* Revision History:
*
* $brcm_Log: /AP/ctfe/core/tnr/btnr_callback.c $
* 
* 13   9/7/12 12:04p farshidf
* SW7425-3765: Remove symbolic links and flatten out TNR directory
*  structure
* 
* Fw_Integration_Devel/11   9/7/12 12:03p farshidf
* SW7425-3765: Remove symbolic links and flatten out TNR directory
*  structure
* 
* Fw_Integration_Devel/AP_V5_0_TNR_DEV/3   9/7/12 12:01p farshidf
* SW7425-3765: Remove symbolic links and flatten out TNR directory
*  structure
* 
* Fw_Integration_Devel/AP_V5_0_TNR_DEV/2   8/15/12 6:47p farshidf
* SW7563-14: remove external LNA calls
* 
* Fw_Integration_Devel/AP_V5_0_TNR_DEV/1   5/7/12 6:30p farshidf
* SW3472-2: Make the tuner multi channel
* 
* Fw_Integration_Devel/8   3/23/12 3:08p farshidf
* SW3128-125: FW version 4.6
* 
* Fw_Integration_Devel/AP_V4_0_TNR_DEV/2   3/23/12 2:49p farshidf
* SW3128-125: FW version 4.6
* 
* SW3461-173/1   3/16/12 3:03p farshidf
* SW3461-1: add power protection for tuner callback from demod
* 
* 8   2/15/12 5:38p farshidf
* SW3461-1: merge to integ
* 
* Fw_Integration_Devel/7   2/15/12 5:37p farshidf
* SW3461-1: merge to integ
* 
* Fw_Integration_Devel/AP_V4_0_TNR_DEV/1   2/9/12 11:48a cbrooks
* sw3461-1:fixed baud rate droppouts
* 
* 7   8/12/11 3:14p farshidf
* SW3461-1: merge to main
* 
* Fw_Integration_Devel/5   8/12/11 2:17p farshidf
* SW3461-1: remove printf
* 
* Fw_Integration_Devel/4   8/2/11 6:19p farshidf
* SW3461-1: merge to integ
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/9   8/2/11 6:11p farshidf
* SW3461-1: update the tuner structure
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/8   7/27/11 1:33p farshidf
* SW3461-1: magnum compatible
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/7   7/27/11 1:19p farshidf
* SW3461-1: magnum compatible
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/6   7/27/11 1:17p farshidf
* SW3461-1: make it magnum compatible
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/5   7/14/11 5:36p cbrooks
* sw3461-1:removed print statements
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/4   7/14/11 5:20p cbrooks
* sw3461-1:callback support
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/3   7/7/11 5:22p farshidf
* SW3461-17: add the tuner callback function
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/2   7/7/11 4:47p cbrooks
* SW3461-1:working on callbacak functions and status
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/1   7/1/11 3:19p cbrooks
* sw3461-1:added ADS callback support
* 
* 4   7/20/11 11:36a farshidf
* SW7552-60: mereg the latest tuner code
* 
* Fw_Integration_Devel/1   6/29/11 12:39p farshidf
* SW3461-13: merge to integration branch
* 
* Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/2   6/22/11 6:47p cbrooks
* sw3461-1:added callback support
* 
* Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/1   6/21/11 5:15p cbrooks
* sw3461-1:Changed Callback Function
* 
* 3   6/9/11 6:39p mpovich
* SW3461-1: Merge Ver 0.4 Integ. onto main branch.
* 
* SW_System_4_Integ_Test/1   6/6/11 2:10p mpovich
* SW3461-1: Merge disparate branches for test purposes.
* 
* Tnr_Fw_Devel_4/1   5/25/11 10:03a farshidf
* SW3461-6: add the timer Interrupt
* 
* 2   5/20/11 6:42a mpovich
* SW3461-1: rename UFE (BUFE) module to TNR (BTNR).
* 
* TNR_3461_1/1   5/19/11 5:11p mpovich
* SW3461-1: Change BUFE module prefix to BTNR
* 
* 1   5/17/11 5:13p farshidf
* SW3461-1: add the callbacks
* 
* 1   5/17/11 1:40p farshidf
* SW3128-1: add the new callbacks
* 
***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "btmr.h"
#ifdef LEAP_BASED_CODE
#include "btnr_api.h"
#else
#include "btnr.h"
#include "bdbg.h"
#include "btnr_priv.h"
#include "btnr_3x7xib_priv.h"
BDBG_MODULE(btnr_callback);
#endif
#include "btnr_tune.h"
#include "btnr_init.h"

/********************************************************************************************/
/*Function to program the RF offset for the ADS channel scan                                */
/********************************************************************************************/
BERR_Code BTNR_P_Set_RF_Offset(BTNR_3x7x_ChnHandle hTnr, int32_t RF_Offset, uint32_t Symbol_Rate)	
{
	BERR_Code retCode = BERR_SUCCESS;

	if (hTnr->pTunerStatus->PowerStatus != BTNR_ePower_On)
  {
		BDBG_ERR(("BTNR_P_Set_RF_Offset: power is still off  "));
		return BERR_NOT_INITIALIZED;
  }

	/*These two parameters come from the BADS_Acquire() to program the front end for 1-7 MBaud and freq offsets*/
	/*RF_Offset is an int32_t*/
	/*Symbol_Rate is an uint32_t*/
	hTnr->pTunerParams->BTNR_Local_Params.RF_Offset   = RF_Offset;    
	  
	BTNR_P_Program_Back_DDFS(hTnr);
	if (Symbol_Rate != 1)
	{
	hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate = Symbol_Rate;
	BTNR_P_Program_VID_CIC_HB_SAW(hTnr);
	}
	return retCode;
}

/********************************************************************************************/
/*Function to get the RF status for the ADS channel scan                                    */
/********************************************************************************************/
BERR_Code BTNR_P_Get_RF_Status(BTNR_3x7x_ChnHandle hTnr)	
{	
	BERR_Code retCode = BERR_SUCCESS;

   if (hTnr->pTunerStatus->PowerStatus != BTNR_ePower_On)
   {
		BDBG_ERR(("BTNR_P_Get_RF_Status: power is still off  "));
		return BERR_NOT_INITIALIZED;
   }
	/*get LNA status registers*/
#ifdef LEAP_BASED_CODE
#if EXT_LNA_ENABLE
	BLNA_P_Read_Registers(hTnr->hTnr->pLna);
#endif
#endif

	/*These three parameters come from the BTNR_P_TunerStatus()*/
	/*hTnr->pTunerStatus->Tuner_RF_Freq is an uint32_t*/
	/*hTnr->pTunerStatus->Tuner_PreADC_Gain_x256db is an int16_t*/
	/*hTnr->pTunerStatus->External_Gain_x256db is an int16_t*/
	BTNR_P_TunerStatus(hTnr);
	
	/*These four parameters come from the BTNR_P_InitStatus()*/
	/*hTnr->pTunerParams->BTNR_Local_Params.RF_Offset is an int32_t */
	/*hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate is an uint32_t*/
	/*hTnr->pTunerParams->BTNR_Local_Params.Total_Mix_After_ADC is an int32_t*/
	/*hTnr->pTunerParams->BTNR_Local_Params.PostADC_Gain_x256db is an int16_t*/
	BTNR_P_InitStatus(hTnr);

	return retCode;
}

/********************************************************************************************/
/*Function to set the Tuner power mode                                  */
/********************************************************************************************/
BERR_Code BTNR_P_LNA_AGC_Power(BTNR_3x7x_ChnHandle hTnr, uint16_t Mode)
{
   
   if (hTnr->pTunerStatus->PowerStatus != BTNR_ePower_On)
   {
		BDBG_ERR(("BTNR_P_LNA_AGC_Power: power is still off  "));
		return BERR_NOT_INITIALIZED;
   }
	
   BSTD_UNUSED(Mode);
	/*BDBG_MSG(("BTNR_P_LNA_AGC_Power and the mode is %d", Mode)); */
	
	return BERR_SUCCESS;


}
