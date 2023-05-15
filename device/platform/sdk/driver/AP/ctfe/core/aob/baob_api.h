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
 * $brcm_Workfile: baob_api.h $
 * $brcm_Revision: 18 $
 * $brcm_Date: 10/2/12 7:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/aob/baob_api.h $
 * 
 * 18   10/2/12 7:00p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/8   10/2/12 6:33p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/AP_V5_0_AOB_DEV/1   10/2/12 5:29p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/SW3461-258/1   9/27/12 10:50a atanugul
 * SW3461-258: Support for obtaining revision number of F/W core driver
 *  modules
 * 
 * Fw_Integration_Devel/7   5/15/12 6:43p farshidf
 * SW3128-163: merge to integ
 * 
 * 16   5/15/12 4:30p farshidf
 * SW3128-163: Create a hab command for WB ADC or OOb ADC for OOB demod
 * 
 * 15   4/27/12 7:03p farshidf
 * SW3128-1: merge to main
 * 
 * Fw_Integration_Devel/6   4/27/12 6:36p farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V4_0_AOB_DEV/2   4/27/12 10:49a farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/5   4/27/12 10:48a farshidf
 * SW3128-1: merge to integ
 * 
 * 14   4/27/12 10:42a farshidf
 * SW3128-1: reset OOB fifo callback
 * 
 * 13   3/30/12 11:18a farshidf
 * SW3128-139: Fw version 4.1
 * 
 * Fw_Integration_Devel/4   3/29/12 4:38p farshidf
 * SW3128-139: merge to Integ
 * 
 * Fw_Integration_Devel/AP_V4_0_AOB_DEV/1   3/9/12 2:17p dorothyl
 * SW3128-1 : add timer
 * 
 * Fw_Integration_Devel/3   2/9/12 12:19p farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/2   1/30/12 5:30p farshidf
 * SW3128-118 : Add Aob callback to tuner for tuner status pulling
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/1   12/13/11 12:30p mpovich
 * SW3128-69: Add lock/unlock IRQs and status for Out of Band.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/SW3128-69/1   12/12/11 9:37p mpovich
 * SW3128-69: Implement Lock/Unlock status/IRQs for Out of Band.
 * 
 * 9   4/12/11 11:47a farshidf
 * SW3128-1: fix warning
 * 
 * 8   3/25/11 6:31p mpovich
 * SW3128-1: Merge latest from AOB branch to the main branch.
 * 
 * AOB_3128_1/2   3/23/11 1:05p mpovich
 * SW3128-1: Rebase AOB from main branch
 * 
 * 7   3/22/11 5:04p mpovich
 * SW3128-1: Add latest AOB driver changes.
 * 
 * AOB_3128_1/1   3/21/11 6:25p farshidf
 * SW3461-1: update naming
 * 
 * 6   3/21/11 4:51p mpovich
 * SW3128-1: Add extra interfaces for OOB HAB command processing.
 * 
 * 5   3/8/11 3:27p farshidf
 * SW3461-1: fix compile issues
 * 
 * 4   3/8/11 3:10p farshidf
 * SW3128-1: add the OOb chnages
 * 
 * 3   3/8/11 2:46p cbrooks
 * sw3128-1:new OOB code
 * 
 * 2   12/16/10 6:33p farshidf
 * SW3461-1: update
 * 
 * 1   12/13/10 4:41p farshidf
 * SW3128-1: add aob
 *
 ***************************************************************************/
#ifndef BAOB_API_H__
#define BAOB_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "baob_struct.h"

typedef struct BAOB_P_3x7x_Handle   *BAOB_3x7x_Handle;

/***************************************************************************
* HAB Function call
****************************************************************************/
BERR_Code BAOB_3x7x_Open(BAOB_3x7x_Handle *pAob, BTMR_Handle  htmr, BKNI_EventHandle hIntEvent);
BERR_Code BAOB_3x7x_Close(BAOB_3x7x_Handle hDev);
BERR_Code BAOB_3x7x_SetAcqSettings(BAOB_3x7x_Handle hDev);
BERR_Code BAOB_3x7x_SetAcquireParams(BAOB_3x7x_Handle hDev, const BAOB_Acquire_Params_t *pClientBuff);
BERR_Code BAOB_3x7x_GetAcquireParams(BAOB_3x7x_Handle hDev, BAOB_Acquire_Params_t *pClientBuff);
BERR_Code BAOB_3x7x_PreAcquire(BAOB_3x7x_Handle hDev);
BERR_Code BAOB_3x7x_Acquire(BAOB_3x7x_Handle hDev);
BERR_Code BAOB_3x7x_Status(BAOB_3x7x_Handle hDev);
BERR_Code BAOB_3x7x_GetStatus(BAOB_3x7x_Handle hDev, BAOB_Status_t *pStatus);
BERR_Code BAOB_3x7x_GetLockStatus(BAOB_3x7x_Handle hDev, bool *isLock );
BERR_Code BAOB_3x7x_PowerUp(BAOB_3x7x_Handle hDev);
BERR_Code BAOB_3x7x_PowerDown(BAOB_3x7x_Handle hDev);
BERR_Code BAOB_3x7x_GetPowerStatus(BAOB_3x7x_Handle hDev, BAOB_PowerStatus *pPowerStatus);
BERR_Code BAOB_3x7x_ResetStatus(BAOB_3x7x_Handle hDev);
void BAOB_3x7x_ProcessInterruptEvent (BAOB_3x7x_Handle hDev);
BERR_Code BAOB_3x7x_Unlock(BAOB_3x7x_Handle hDev);
BERR_Code BAOB_3x7x_GetComponentVersion(BAOB_3x7x_Handle hDev,  void *pVersion);


/* void BAOB_P_TimerFunc(void *myParam1, int myParam2); */

BERR_Code BAOB_3x7x_GetSoftDecision(BAOB_3x7x_Handle hDev,           /* [in]  Device channel handle */
                                    void *pClientDataBuf );     /* [out] Client data buffer */
BERR_Code BAOB_P_GetSoftDecisionBuf(
    BAOB_3x7x_Handle hDev,					/* [in] Device channel handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *iVal,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    );




/***************************************************************/
/***************************************************************/

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/


/***************************************************************************
Summary:
    Data for the AOB  callback
****************************************************************************/
/*This is a single NON-channelized structure called on demand then sent through the HAB*/
/*Each sub-structure is completely filled by an appropriate function call*/

typedef enum BAOB_CallbackMode
{
	/* Most of these are currently not supported, are here for future use */
    BAOB_CallbackMode_eSetMode = 0,
	BAOB_CallbackMode_eRequestMode = 1,
	BAOB_CallbackMode_eEnablePower = 2,
	BAOB_CallbackMode_eDisablePower = 3, 
	BAOB_CallbackMode_eResetFifo = 4,
    BAOB_CallbackMode_eLast
} BAOB_CallbackMode;

typedef struct BAOB_P_AobCallbackData_s
{
	void*			            hTunerChn;
	uint16_t		          Mode;										/*Callback is to send data on eSetMode, Callback is to get data on eRequestMode*/
	uint32_t              Symbol_Rate;            /*Upper Symbol_Rate currently being scanned on eSetMode: return last value sent on eRequestMode*/
	int32_t		            Freq_Offset;					  /*Front end offset to use on eSetMode: return last value sent on eRequestMode*/
	uint32_t		          RF_Freq;						    /*RF frequency of the tuner on eRequestMode: set to 0 if unknown*/
	int32_t		            Total_Mix_After_ADC;    /*Sum of mixer frequencies after ADC on eRequestMode*/
	int16_t		            PreADC_Gain_x256db ;    /*Gain in db*256 before ADC on eRequestMode: set to 0x8000 if unknown*/
	int16_t		            PostADC_Gain_x256db;    /*Gain in db*256 after ADC on eRequestMode: set to 0x8000 if unknown*/
	int16_t		            External_Gain_x256db;   /*Gain in db*256 external to chip (like external LNA) on eRequestMode: set to 0x8000 if unknown*/
	bool					RfOOBInputMode;
}BAOB_P_AobCallbackData_t;



/***************************************************************************
Summary:
    Callback from AOB to AOB
****************************************************************************/
typedef void (*BAOB_3x7x_AobCallback)(void *pParam);


/***************************************************************************
Summary:
    Enumeration for Callback types

Description:
    This enumeration defines Callback types.

See Also:

****************************************************************************/
typedef enum BAOB_Callback
{    
	BAOB_Callback_eTuner,				/* Callback to tuner regarding a new setting */ 
    BAOB_Callback_eLast                 /* More may be required */
} BAOB_Callback;

/***************************************************************************
Summary:
    Callback info
****************************************************************************/

BERR_Code BAOB_3x7x_InstallAobCallback(BAOB_3x7x_Handle hDev, BAOB_3x7x_AobCallback fCallBack, void *pParam1);
BERR_Code BAOB_3x7x_RemoveAobCallback(BAOB_3x7x_Handle hDev);



/*******************************************************************************
*
*    Module Handles
*
*******************************************************************************/

typedef struct BAOB_P_3x7x_Handle
{
    uint32_t			magicId;                   /* Used to check if structure is corrupt */
    BREG_Handle			hRegister;
	unsigned int		mxChnNo;
	BTMR_TimerHandle	hTimer;
	BAOB_Status_t		*pStatus;
	BAOB_AcqParams_t	*pAcqParam;
	BKNI_EventHandle	hIntEvent;
	BAOB_PowerStatus PowerStatus;
    bool                autoAcquireMasked;
    BAOB_Status_UnlockLock_t FLK_Prev;
    BKNI_MutexHandle    hMutex;
	BAOB_3x7x_AobCallback		pCallback[BAOB_Callback_eLast];
	void 						*pCallbackParam[BAOB_Callback_eLast];
	BTMR_TimerHandle	hStatusTimer;

} BAOB_P_3x7x_Handle;
#ifdef __cplusplus
}
#endif

#endif



