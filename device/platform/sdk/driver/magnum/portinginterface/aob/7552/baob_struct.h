/***************************************************************************
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
 * $brcm_Workfile: baob_struct.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/22/12 4:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/aob/7552/baob_struct.h $
 * 
 * Hydra_Software_Devel/1   6/22/12 4:44p farshidf
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure
 * 
 * 20   2/9/12 12:44p farshidf
 * SW3128-1: merge to main
 * 
 * Fw_Integration_Devel/5   2/9/12 12:19p farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/6   2/8/12 2:15p dorothyl
 * SW3128-1 : OOB BERT
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/5   2/1/12 5:50p farshidf
 * SW3128-1: add 7552 fields
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/4   2/1/12 5:49p farshidf
 * SW3128-1: merge to integ
 * 
 * 18   1/18/12 11:18a farshidf
 * SW3128-1: change the status elemnt to 32 bits for 7552 compatability
 * 
 * 17   1/12/12 2:28p farshidf
 * SW3128-1: merge back the structure
 * 
 * 16   1/9/12 12:22p farshidf
 * SW3128-102: merge to main
 * 
 * Fw_Integration_Devel/3   1/9/12 11:55a farshidf
 * SW3128-102: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/2   1/6/12 1:24p dorothyl
 * SW3128-1 : add nyq filter option
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/1   12/13/11 12:27p mpovich
 * SW3128-69: Add lock/unlock IRQs and status for Out of Band.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/SW3128-69/1   12/13/11 10:31a mpovich
 * SW3128-69: Add Acquire After Tune flag.
 * 
 * 9   11/9/11 4:39p farshidf
 * SW7552-139: adapt the code for magnum support
 * 
 * 8   4/5/11 3:28p farshidf
 * SW3461-1: merge  main
 * 
 * AOB_3128_1/8   4/5/11 10:54a dorothyl
 * SW3128-1: OOB UPDATE
 * 
 * AOB_3128_1/7   3/31/11 11:44a farshidf
 * SW3128-1: fix compile issue
 * 
 * AOB_3128_1/6   3/31/11 11:32a dorothyl
 * SW3128-1: oob status fix
 * 
 * AOB_3128_1/5   3/29/11 10:39a dorothyl
 * SW3128-1: oob bersrc
 * 
 * AOB_3128_1/4   3/23/11 1:05p mpovich
 * SW3128-1: Rebase AOB from main branch
 * 
 * 4   3/22/11 5:05p mpovich
 * SW3128-1: Add latest AOB driver changes.
 * 
 * AOB_3128_1/3   3/18/11 4:38p cbrooks
 * sw3128-1:Added Status for OOB
 * 
 * AOB_3128_1/2   3/18/11 12:43p cbrooks
 * sw3128-1:new code
 * 
 * AOB_3128_1/1   3/17/11 6:16p cbrooks
 * sw3128-1:Added OOB code
 * 
 * 3   3/8/11 3:27p farshidf
 * SW3461-1: fix compile issues
 * 
 * 2   3/8/11 2:46p cbrooks
 * sw3128-1:new OOB code
 * 
 * 1   2/23/11 11:58a cbrooks
 * sw3128-1:new code
 * 
 * 2   12/16/10 6:33p farshidf
 * SW3461-1: update
 * 
 * 1   12/13/10 4:41p farshidf
 * SW3128-1: add aob
 *
 ***************************************************************************/
#ifndef BAOB_STRUCT_H__
#define BAOB_STRUCT_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LEAP_BASED_CODE
#define F_HS		   15000000
#endif

/****************************************************************************************
* Enum used by  BAOB_AcqParams_t
****************************************************************************************/


typedef enum BAOB_Acquire_Params_BPS_s
{
  BAOB_Acquire_Params_BPS_eDVS_178, 
  BAOB_Acquire_Params_BPS_eDVS_167_GradeA,
  BAOB_Acquire_Params_BPS_eDVS_167_GradeB,
  BAOB_Acquire_Params_BPS_eBERT_TEST_MODE
}BAOB_Acquire_Params_BPS_t;

typedef enum BAOB_Acquire_Params_FEC_s
{
  BAOB_Acquire_Params_BPS_eDVS178, 
  BAOB_Acquire_Params_BPS_eDVS167
}BAOB_Acquire_Params_FEC_t;

typedef enum BAOB_Acquire_Params_PLBW_s
{
	BAOB_Acquire_Params_PLBW_eLow, 
	BAOB_Acquire_Params_PLBW_eMed,
	BAOB_Acquire_Params_PLBW_eHigh
}BAOB_Acquire_Params_PLBW_t;

typedef enum BAOB_Acquire_Params_BERT_Source_s
{
	BAOB_Acquire_Params_BERT_Source_eI, 
	BAOB_Acquire_Params_BERT_Source_eQ,
	BAOB_Acquire_Params_BERT_Source_eIQ,
	BAOB_Acquire_Params_BERT_Source_eFEC
}BAOB_Acquire_Params_BERT_Source_t;

typedef enum BAOB_Acquire_Params_OffOn_s
{
  BAOB_Acquire_Params_eDisable, 
  BAOB_Acquire_Params_eEnable
}BAOB_Acquire_Params_OffOn_t;

typedef enum BAOB_Acquire_Params_Nyq_s
{	
	BAOB_Acquire_Params_Nyq_ehalf_raised,
	BAOB_Acquire_Params_Nyq_ehalf_rtraised,
	BAOB_Acquire_Params_Nyq_ethird_rtraised
}BAOB_Acquire_Params_Nyq_t;


#ifndef LEAP_BASED_CODE
typedef enum BAOB_AcquireStartMode
{
  BAOB_AcquireStartMode_Acquire = 1, 
  BAOB_AcquireStartMode_ResetStatus = 2,
  BAOB_AcquireStartMode_UpdateStatus = 4
} BAOB_AcquireStartMode_t;
#endif

typedef struct BAOB_Acquire_Params_s
{
#ifndef LEAP_BASED_CODE
	volatile BAOB_AcquireStartMode_t			AcquireStartMode;
#endif
	BAOB_Acquire_Params_OffOn_t					Auto;
	BAOB_Acquire_Params_OffOn_t					AI;
	BAOB_Acquire_Params_OffOn_t					IS;
	BAOB_Acquire_Params_OffOn_t     			BYP;
	BAOB_Acquire_Params_BPS_t					BPS;
	BAOB_Acquire_Params_FEC_t					AA;
	BAOB_Acquire_Params_PLBW_t					PLBW;
	BAOB_Acquire_Params_BERT_Source_t			BERSRC;
	BAOB_Acquire_Params_OffOn_t					Acquire_After_Tune;
	BAOB_Acquire_Params_Nyq_t					Nyq;
}BAOB_Acquire_Params_t;


typedef enum BAOB_Local_Params_BERT_Source_s
{
	BAOB_Local_Params_BERT_Source_eI, 
	BAOB_Local_Params_BERT_Source_eQ,
	BAOB_Local_Params_BERT_Source_eIQ,
	BAOB_Local_Params_BERT_Source_eFEC
}BAOB_Local_Params_BERT_Source_t;

typedef enum BAOB_Local_Params_CTRL3_s
{
   BAOB_Local_Params_CTRL3_DEFAULT	
}BAOB_Local_Params_CTRL3_t;

typedef struct BAOB_Local_Params_s
{
	BAOB_Local_Params_BERT_Source_t		BERSRC;
	uint32_t							CTRL3;
#ifndef LEAP_BASED_CODE
	uint32_t							LockUpdate;
	uint32_t							LockStatus;
	uint32_t							BBSAcqAdd;
	uint32_t							BBSStaAdd;
#endif
}BAOB_Local_Params_t;

/***************************************************************************
* BAOB_AcqParams_t This is the main structure by the AOB acquisition functions
****************************************************************************/
typedef struct BAOB_AcqParams_s
{
	BAOB_Acquire_Params_t		BAOB_Acquire_Params;
	BAOB_Local_Params_t			BAOB_Local_Params;      /*needs initialization in BAOB_P_Init()*/
}BAOB_AcqParams_t;

/****************************************************************************************
* Enum used by  BAOB_Status_t
***************************************************************************************/

typedef enum BAOB_Status_NotInvertedInveted_s
{
  BAOB_Status_eNotInverted, 
  BAOB_Status_eInverted
}BAOB_Status_NotInvertedInveted_t;

typedef enum BAOB_Status_UnlockLock_s
{
  BAOB_Status_eUnlock, 
  BAOB_Status_eLock
}BAOB_Status_UnlockLock_t;

typedef enum BAOB_Status_BERT_Source_s
{
	BAOB_Status_BERT_Source_eI, 
	BAOB_Status_BERT_Source_eQ,
	BAOB_Status_BERT_Source_eIQ,
	BAOB_Status_BERT_Source_eFEC
}BAOB_Status_BERT_Source_t;

typedef enum BAOB_Status_PLBW_s
{
	BAOB_Status_PLBW_eLow, 
	BAOB_Status_PLBW_eMed,
	BAOB_Status_PLBW_eHigh
}BAOB_Status_PLBW_t;

typedef enum BAOB_Status_FEC_s
{
  BAOB_Status_BPS_eDVS178, 
  BAOB_Status_BPS_eDVS167
}BAOB_Status_FEC_t;

typedef enum BAOB_Status_BPS_s
{
	BAOB_Status_BPS_eDVS_178, 
	BAOB_Status_BPS_eDVS_167_GradeA,
	BAOB_Status_BPS_eDVS_167_GradeB,
	BAOB_Status_BPS_eBERT_TEST_MODE
}BAOB_Status_BPS_t;

typedef enum BAOB_Status_OffOn_s
{
	BAOB_Status_eDisable, 
	BAOB_Status_eEnable
}BAOB_Status_OffOn_t;

typedef struct BAOB_Status_s
{
	BAOB_Status_OffOn_t							Auto;
	BAOB_Status_OffOn_t							AI;
	BAOB_Status_OffOn_t     					BYP;
	BAOB_Status_BPS_t							BPS;
	BAOB_Status_FEC_t							AA;
	BAOB_Status_PLBW_t							PLBW;
	BAOB_Status_BERT_Source_t					BERSRC;
	BAOB_Status_UnlockLock_t					RLK;
	BAOB_Status_UnlockLock_t					FLK;
	BAOB_Status_NotInvertedInveted_t			SI;
	BAOB_Status_UnlockLock_t					BERSYN;
	uint32_t									Reack_Count;
	uint32_t									SNR;
	uint32_t									Sample_Rate;
	uint32_t									Symbol_Rate;
	int32_t									    Frequency_Error;
	int32_t					  				    Symbol_Error;
	uint32_t									BERT;
	uint32_t									FEC_Corr_RS_Blocks;
	uint32_t									FEC_UCorr_RS_Blocks;
	uint32_t									ATM_Total_Count;
	uint32_t									ATM_Loss_Count;
	int32_t	   									EstChannelPower_dbm;
	int32_t										FrontEndGain_db;
	int32_t								   		DAGC_Gain_db;
	int32_t								   		EQ_Gain_db;
}BAOB_Status_t;

/* Channel power on/off status */
typedef enum BAOB_PowerStatus
{
    BAOB_ePower_Off = 0,
    BAOB_ePower_On  = 1,
    BAOB_ePower_Unknown = 0xFF
} BAOB_PowerStatus;







#ifdef __cplusplus
}
#endif

#endif



