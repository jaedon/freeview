/**************************************************************************
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
 * $brcm_Workfile: baob_api.c $
 * $brcm_Revision: 26 $
 * $brcm_Date: 10/11/12 12:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/aob/baob_api.c $
 * 
 * 26   10/11/12 12:00p farshidf
 * SW3128-233: AOB Total time increase at each acquire and enable the
 *  INV_CLK to get cabelcard signal
 * 
 * 25   10/2/12 7:00p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/11   10/2/12 6:33p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/AP_V5_0_AOB_DEV/2   10/2/12 5:29p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/AP_V5_0_AOB_DEV/SW3461-258/1   9/27/12 10:50a atanugul
 * SW3461-258: Support for obtaining revision number of F/W core driver
 *  modules
 * 
 * Fw_Integration_Devel/AP_V5_0_AOB_DEV/1   8/29/12 6:11p farshidf
 * SW7584-44: merge to integ/main
 * 
 * Fw_Integration_Devel/AP_V5_0_AOB_DEV/SW7584-41/1   8/29/12 5:36p farshidf
 * SW7552-336: re-structure TNR folder to point to AP tnr code
 * 
 * Fw_Integration_Devel/9   8/21/12 5:12p mpovich
 * SW3472-6: Merge to 5_0 Dev branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_SYS_DEV/SW3472-6/1   7/12/12 8:12p mpovich
 * SW3472-6: Use channelized versions of common, Sys_Evt set functions.
 * 
 * Fw_Integration_Devel/8   3/29/12 6:34p farshidf
 * SW3128-139: remove warning
 * 
 * Fw_Integration_Devel/AP_V4_0_AOB_DEV/2   3/29/12 6:20p farshidf
 * SW3128-139: remove warning
 * 
 * Fw_Integration_Devel/AP_V4_0_AOB_DEV/1   3/9/12 2:17p dorothyl
 * SW3128-1: ADD ACQ TIMER
 * 
 * Fw_Integration_Devel/6   2/9/12 12:26p farshidf
 * SW3128-1: merge ti integ
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/5   1/30/12 5:33p farshidf
 * SW3128-118: Add Aob callback to tuner for tuner status pulling
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/4   1/30/12 5:30p farshidf
 * SW3128-118 : Add Aob callback to tuner for tuner status pulling
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/3   12/13/11 3:54p mpovich
 * SW3128-1: Rebase from integ. branch.
 * 
 * Fw_Integration_Devel/4   12/13/11 3:26p mpovich
 * SW3128-1: Rebase to integ. branch.
 * 
 * 19   12/13/11 3:17p mpovich
 * SW3128-1: Merge to main branch (conditionally use of LEAP specific
 *  define).
 * 
 * 18   12/13/11 1:21p mpovich
 * SW3128-1: Merge to the main branch.
 * 
 * Fw_Integration_Devel/3   12/13/11 12:16p mpovich
 * SW3128-69: Add lock/unlock IRQs and status for Out of Band.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/SW3128-69/3   12/13/11 11:00a mpovich
 * SW3128-69: Ensure OOB acquire parameters have memory allocated for them
 *  when the OOB is opened.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/SW3128-69/2   12/13/11 9:52a mpovich
 * SW3128-69: Implement Lock/Unlock status/IRQs for Out of Band.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/SW3128-69/1   12/12/11 9:40p mpovich
 * SW3128-69: Implement Lock/Unlock status/IRQs for Out of Band.
 * 
 * 17   11/22/11 4:34p farshidf
 * SW3461-99: merge to main
 * 
 * Fw_Integration_Devel/2   11/22/11 3:22p farshidf
 * SW3461-99: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/2   11/9/11 4:41p farshidf
 * SW7552-139: merge to dev
 * 
 * 16   11/9/11 4:38p farshidf
 * SW7552-139: adapt the code for magnum support
 * 
 * 15   11/3/11 12:07p farshidf
 * SW3128-60: merge to main
 * 
 * Fw_Integration_Devel/1   11/3/11 11:45a farshidf
 * SW3128-60: merge to Integ
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/1   10/28/11 2:27p farshidf
 * SW3461-75: move to new IRQ handler
 * 
 * 14   4/18/11 5:32p farshidf
 * SW3128-1: update to make it work with host chip
 * 
 * 13   4/12/11 11:47a farshidf
 * SW3128-1: fix warning
 * 
 * 12   4/8/11 5:32p farshidf
 * SW3128-1: add the power protection
 * 
 * 11   4/7/11 11:24a farshidf
 * SW3128-1: add lock status
 * 
 * 10   4/4/11 9:58a farshidf
 * SW3128-1: merge main
 * 
 * AOB_3128_1/3   4/1/11 5:08p farshidf
 * SW3128-1: fix the auto acquire
 * 
 * AOB_3128_1/2   3/23/11 1:04p mpovich
 * SW3128-1: Rebase AOB from main branch
 * 
 * 8   3/22/11 5:04p mpovich
 * SW3128-1: Add latest AOB driver changes.
 * 
 * AOB_3128_1/1   3/21/11 6:25p farshidf
 * SW3461-1: update naming
 * 
 * 7   3/21/11 4:51p mpovich
 * SW3128-1: Add extra interfaces for OOB HAB command processing.
 * 
 * 6   3/8/11 3:27p farshidf
 * SW3461-1: fix compile issues
 * 
 * 5   3/8/11 3:10p farshidf
 * SW3128-1: add the OOb chnages
 * 
 * 4   3/8/11 2:46p cbrooks
 * sw3128-1:new OOB code
 * 
 * 3   2/16/11 11:19a farshidf
 * SW3128-1: add th WFE timer and flag
 * 
 * 2   12/16/10 6:37p farshidf
 * SW3128-1: update
 * 
 * 1   12/13/10 4:41p farshidf
 * SW3128-1: add aob
 * 
 * 
 ***************************************************************************/
#include "bchp_common.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "btmr.h"
#include "baob_api.h"
#include "baob_acquire.h"
#include "baob_status.h"
#include "baob_utils.h"

#include "bchp_leap_ctrl.h"
#include "bchp_oob.h"
#include "baob_priv.h"
#include "baob_irq.h"
#include "baob_ver.h"
#ifdef LEAP_BASED_CODE 
#include "bversion.h"
#endif

#include "sys_evt.h"


#if ! defined(LEAP_BASED_CODE) || (BLEAP_AOB_ENABLED)

/* Timer interval for checking lock status */
#define BAOB_P_TIMER_IRQ_INTERVAL_USEC (25000)

static void BAOB_P_TimerFunc(void *myParam1, int myParam2);
static void BAOB_3x7x_P_MutexTake ( BAOB_3x7x_Handle hDev );
static void BAOB_3x7x_P_MutexGive ( BAOB_3x7x_Handle hDev );
static void BAOB_3x7x_P_UpdateLockStatusToSystem(BAOB_3x7x_Handle hDev, bool bForceUpdate);
static void BAOB_3x7x_P_Unlock(BAOB_3x7x_Handle hDev, bool bForceUnlockToSystem);

/*******************************************************************************
*
*   HAB to API function call
*
*******************************************************************************/

/*******************************************************************************
*   BAOB_Open
*******************************************************************************/

BERR_Code BAOB_3x7x_Open(
    BAOB_3x7x_Handle *pAob,                          /* [out] Returns handle */
    BTMR_Handle  htmr,							/* timer handle */
	BKNI_EventHandle hIntEvent
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BAOB_3x7x_Handle hDev;
	BTMR_Settings sTimerSettings;
    
    /* Alloc memory from the system heap */
    hDev = (BAOB_3x7x_Handle) BKNI_Malloc( sizeof(BAOB_P_3x7x_Handle ) );
    if( hDev == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BAOB_3x7x_Open: BKNI_malloc() failed"));
        goto done;
    }
    BKNI_Memset( hDev, 0x00, sizeof(BAOB_P_3x7x_Handle ) );
    hDev->magicId = DEV_MAGIC_ID;
	hDev->hIntEvent = hIntEvent;
	hDev->hRegister = BCHP_PHYSICAL_OFFSET;

	hDev->pStatus = (BAOB_Status_t *)BKNI_Malloc(sizeof(BAOB_Status_t));
    hDev->pAcqParam = (BAOB_AcqParams_t *)BKNI_Malloc(sizeof(BAOB_AcqParams_t));
    if( hDev->pStatus == NULL || hDev->pAcqParam == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BAOB_3x7x_Open: BKNI_malloc() failed"));
        goto done;
    }
	BKNI_Memset( hDev->pStatus, 0x00, sizeof(BAOB_Status_t) );
    BKNI_Memset( hDev->pAcqParam, 0x00, sizeof(BAOB_AcqParams_t) );

    /* Create mutex for sychronization locks */
    if ( BERR_SUCCESS != (retCode = BKNI_CreateMutex(&hDev->hMutex)) )
    {
        BDBG_ERR(("BAOB_3x7x_Open: Create Mutex Failed"));
        goto done;
    }

	/* Create timer for status lock check */
	sTimerSettings.type = BTMR_Type_eCountDown;
	sTimerSettings.cb_isr = (BTMR_CallbackFunc)BAOB_P_TimerFunc;
	sTimerSettings.pParm1 = (void*)hDev;
	sTimerSettings.parm2 = 0;
	retCode = BTMR_CreateTimer (htmr, &hDev->hTimer, &sTimerSettings);
	if ( retCode != BERR_SUCCESS )
	{
	   BDBG_ERR(("BAOB_3x7x_Open: Create Timer Failed"));
	   goto done;
    }	

	sTimerSettings.type = BTMR_Type_eStopWatch;
	sTimerSettings.cb_isr = 0;
	sTimerSettings.pParm1 = 0;
	sTimerSettings.parm2 = 0;
	retCode = BTMR_CreateTimer (htmr, &hDev->hStatusTimer, &sTimerSettings);
	if ( retCode != BERR_SUCCESS )
	{
        BDBG_ERR(("BAOB_3x7x_Open: Create Timer hStatus Failed"));
	   goto done;
    }
    /* TODO: If timer triggers an interrupt, then a race condition possibly exists at init.  Fix this later */
	BTMR_StartTimer(hDev->hStatusTimer, 9942053);   /* the timer is in Micro second to make the timer 9.94s*/

    *pAob = hDev;

done:
    if( retCode != BERR_SUCCESS )
    {
        if ( hDev != NULL ) 
        {
            BAOB_3x7x_Close(hDev);
        }
        *pAob = NULL;
    }
    return( retCode );
}


/*******************************************************************************
*  BAOB_3x7x_Close
*******************************************************************************/

BERR_Code BAOB_3x7x_Close(
   BAOB_3x7x_Handle hDev                    /* [in] Device handle */
    )
{
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	
    BAOB_3x7x_P_MutexTake(hDev);

    BAOB_3x7x_P_Unlock(hDev,false);

	if( NULL != hDev->hTimer )
    {
        BTMR_DestroyTimer(hDev->hTimer);
    }
    if( NULL != hDev->hIntEvent )
    {
        BKNI_DestroyEvent(hDev->hIntEvent);
    }
    if ( NULL != hDev->pStatus ) 
    {
        BKNI_Free(hDev->pStatus);
    }
    if ( NULL != hDev->pAcqParam ) 
    {
        BKNI_Free(hDev->pAcqParam);
    }
    if ( NULL != hDev->hMutex ) 
    {
        BKNI_DestroyMutex(hDev->hMutex);
    }
    hDev->magicId = 0x00;       /* clear it to catch improper use */
    BKNI_Free( (void *) hDev );

    return BERR_SUCCESS;
}

/***************************************************************************
 * BAOB_3x7x_P_MutexTake()
 ***************************************************************************/
static void BAOB_3x7x_P_MutexTake ( BAOB_3x7x_Handle hDev )
{
    /* Take the driver's critical section (setting Acq Settings, Acquiring, ...) */
    if ( hDev->hMutex != NULL )
    { 
        BKNI_AcquireMutex(hDev->hMutex);
    }
}

/***************************************************************************
 * BAOB_3x7x_P_MutexGive()
 ***************************************************************************/
static void BAOB_3x7x_P_MutexGive ( BAOB_3x7x_Handle hDev )
{
    /* Release the driver's critical section (setting Acq Settings, Acquiring, ...) */
    if ( hDev->hMutex != NULL )
    {
        BKNI_ReleaseMutex(hDev->hMutex);
    }
}

/*******************************************************************************
*  BAOB_3x7x_SetAcqSettings
*******************************************************************************/

BERR_Code BAOB_3x7x_SetAcqSettings(
    BAOB_3x7x_Handle hDev			 /* [in] Device channel handle */
    )
{
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    return BERR_SUCCESS;
}

/*******************************************************************************
*  BAOB_3x7x_SetAcquireParams
*******************************************************************************/
BERR_Code BAOB_3x7x_SetAcquireParams(
    BAOB_3x7x_Handle hDev,			 /* [in] Device channel handle */
    const BAOB_Acquire_Params_t *pClientBuff
    )
{
    BERR_Code eResult = BERR_SUCCESS;
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if ( NULL != pClientBuff ) 
    {
        BAOB_3x7x_P_MutexTake(hDev);

        hDev->pAcqParam->BAOB_Acquire_Params = *pClientBuff;

        BAOB_3x7x_P_MutexGive(hDev);
    }
    else
    {
        eResult = BERR_INVALID_PARAMETER;
    }
    
    return ( eResult );
}

/*******************************************************************************
*  BAOB_3x7x_GetAcquireParams
*******************************************************************************/
BERR_Code BAOB_3x7x_GetAcquireParams(
    BAOB_3x7x_Handle hDev,			 /* [in] Device channel handle */
    BAOB_Acquire_Params_t *pClientBuff
    )
{
    BERR_Code eResult = BERR_SUCCESS;
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if ( NULL != pClientBuff ) 
    {
        *pClientBuff = hDev->pAcqParam->BAOB_Acquire_Params;
    }
    else
    {
        eResult = BERR_INVALID_PARAMETER;
    }

    return ( eResult );
}

/*******************************************************************************
*   BAOB_3x7x_PreAcquire (Setup before starting BAOB_Acquire)
*******************************************************************************/
BERR_Code BAOB_3x7x_PreAcquire(
    BAOB_3x7x_Handle hDev            /* [in] Device channel handle */
    )
{
    BERR_Code eResult = BERR_SUCCESS;
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_MSG(("BAOB_3x7x_PreAcquire "));

    if ( hDev->PowerStatus != BAOB_ePower_On )
    {
        BDBG_ERR(("!!!!!BAOB_3x7x_Acquire: not powered on"));
        eResult = BERR_NOT_INITIALIZED;
    }
    else
    {
        /* Inhibit auto-acquire momentarily (will very soon start the acquire) */
        hDev->autoAcquireMasked = true;
        /* Mask Lock/Unlock IRQs for the channel - before HAB done issued for the Acquire command */
        /* eResult = Sys_EvtSet(SysEvent_eOOB_PreAcquire, SysEvent_AP_eEvtSet, (SysEventData_union_t)(uint32_t)0); */
        eResult = Sys_EvtReportPreAcquire(SysEvt_CompType_eOOB, 0);
    }
    return ( eResult );
}

/*******************************************************************************
*  BAOB_3x7x_Acquire
*******************************************************************************/
BERR_Code BAOB_3x7x_Acquire(
   BAOB_3x7x_Handle hDev            /* [in] Device  handle */
    )
{
    BERR_Code eResult = BERR_SUCCESS;
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BAOB_3x7x_P_MutexTake(hDev);

	if ( hDev->PowerStatus != BAOB_ePower_On )
	{
        BDBG_ERR(("!!!!!BAOB_3x7x_Acquire: not powered on"));
        eResult = BERR_NOT_INITIALIZED;
	}
    else
    {
		hDev->pStatus->Reack_Count = 0;
        /* Stop the period lock-check timer interrupt during acquisition */
        BTMR_StopTimer(hDev->hTimer);   /* the timer is in Micro second */;

        /* Indicate Acquire start (ensures unlock issued to Host in the event of a re-acquire w/o a Tune) */
        /* Sys_EvtSet(SysEvent_eOOB_AcquireStart, SysEvent_AP_eEvtSet, (SysEventData_union_t)(uint32_t)0); */
        Sys_EvtReportAcquireStart(SysEvt_CompType_eOOB, 0);

        eResult = BAOB_P_Acquire(hDev);

        /* Indicate Acquire finish (Reallows lock/unlock IRQ and status updates to Host). */
        /* Sys_EvtSet(SysEvent_eOOB_AcquireComplete, SysEvent_AP_eEvtSet, (SysEventData_union_t)(uint32_t)0); */
        Sys_EvtReportAcquireComplete(SysEvt_CompType_eOOB, 0);

        /* Force the lock status to update after the acquire completes */
        BAOB_P_GetLock_Status(hDev);
        BAOB_3x7x_P_UpdateLockStatusToSystem(hDev, true);

        /* Auto acquire is no longer masked */
        hDev->autoAcquireMasked  = false; 

        BTMR_StartTimer(hDev->hTimer, BAOB_P_TIMER_IRQ_INTERVAL_USEC);   /* the timer is in Micro second */;
    }

    BAOB_3x7x_P_MutexGive(hDev);

    return ( eResult );
}

/*******************************************************************************
*  BAOB_3x7x_Status (Update status struct)
*******************************************************************************/

BERR_Code BAOB_3x7x_Status(
   BAOB_3x7x_Handle hDev          /* [in] Device  handle */
    )
{
    BERR_Code eResult = BERR_SUCCESS;
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if (hDev->PowerStatus != BAOB_ePower_On)
	{
        BDBG_ERR(("!!!!!BAOB_3x7x_Status: not powered on"));
        eResult = BERR_NOT_INITIALIZED;
	}
    else
    {
        eResult = BAOB_P_Get_Status(hDev);
    }

    return ( eResult );
}

/*******************************************************************************
*  BAOB_3x7x_GetStatus (Read status from status struct)
*******************************************************************************/
BERR_Code BAOB_3x7x_GetStatus(
   BAOB_3x7x_Handle hDev,          /* [in] Device  handle */
   BAOB_Status_t *pStatus
    )
{
    BERR_Code eResult = BERR_SUCCESS;
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( NULL != (hDev->pStatus) );

	if (hDev->PowerStatus != BAOB_ePower_On)
	{
		BDBG_ERR(("!!!!!BAOB_GetStatus: not powered on"));
        eResult = BERR_NOT_INITIALIZED;
	}
    else if ( NULL == pStatus )
    {
        BDBG_ERR(("!!!!!BAOB_GetStatus: null pointer"));
        eResult = BERR_INVALID_PARAMETER;
    }
    else
    {
        BKNI_Memcpy(pStatus, hDev->pStatus, sizeof(BAOB_Status_t));
    }

    return ( eResult );
}

/*******************************************************************************
*  BAOB_3x7x_GetLockStatus
*******************************************************************************/
BERR_Code BAOB_3x7x_GetLockStatus(
   BAOB_3x7x_Handle hDev,					/* [in] Device  handle */
    bool *pIsLocked                        /* [out] Returns lock status, 0=not lock, 1=locked */
    )
{
    BERR_Code eResult = BERR_SUCCESS;
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if (hDev->PowerStatus != BAOB_ePower_On)
	{
        BDBG_ERR(("!!!!!BAOB_GetStatus: not powered on"));
        eResult = BERR_NOT_INITIALIZED;
	}
    else if ( NULL == pIsLocked ) 
    {
        BDBG_ERR(("!!!!!BAOB_GetStatus: null ptr"));
        eResult = BERR_INVALID_PARAMETER;
    }
    else
    {
        eResult = BAOB_P_Get_LockStatus(hDev);
        *pIsLocked = hDev->pStatus->FLK;
    }

    return ( eResult );
}

/*******************************************************************************
*  BAOB_P_GetSoftDecisionBuf
*******************************************************************************/
BERR_Code BAOB_P_GetSoftDecisionBuf(
    BAOB_3x7x_Handle hDev,					/* [in] Device channel handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *iVal,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    )
{
    if ( NULL != nbrGotten ) 
    {
        *nbrGotten = 0;
    }
    return BERR_SUCCESS;
}

/*******************************************************************************
*  BAOB_3x7x_GetSoftDecision
*******************************************************************************/
BERR_Code BAOB_3x7x_GetSoftDecision(BAOB_3x7x_Handle hDev,           /* [in]  Device channel handle */
                               void *pClientDataBuf )      /* [out] Client data buffer */
{
#if TRUE
  #warning BAOB_P_GetSoftDecision(hDev) still in development !
          return BERR_SUCCESS;
#else
	int16_t *pI;						/* [out] 30 I-values */
	int16_t *pQ;						/* [out] 30 Q-values */
    int16_t *nbrGotten ;                 /* [out] Number of values gotten/read */

	if (hDev->PowerStatus != BAOB_ePower_On)
	{
		BDBG_ERR(("!!!!!BAOB_GetSoftDecision: The channel is not power on yet "));
        return BERR_NOT_INITIALIZED;
	}
    else
    {
        pI = (int16_t *)BKNI_Malloc(sizeof(int16_t)*32);
        pQ = (int16_t *)BKNI_Malloc(sizeof(int16_t)*32);
        BAOB_GetSoftDecisionBuf (hDev, 30, pI, pQ, nbrGotten);
        /* copy it to hab buffer */
        /* once copy to hab */
        BKNI_Free(pI);
        BKNI_Free(pQ);
        return BERR_SUCCESS;
    }
#endif
}

/*******************************************************************************
*  BAOB_3x7x_PowerUp
*******************************************************************************/
BERR_Code BAOB_3x7x_PowerUp ( 
   BAOB_3x7x_Handle hDev            /* [in] Device  handle */
    )
{
    BERR_Code eResult = BERR_SUCCESS;
	BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BAOB_3x7x_P_MutexTake(hDev);

    if ( hDev->PowerStatus != BAOB_ePower_On )
    {
        if ( BERR_SUCCESS == (eResult = BAOB_P_PowerUp(hDev)) )
        {
            hDev->PowerStatus = BAOB_ePower_On;
        }
    }

    BAOB_3x7x_P_MutexGive(hDev);

    return ( eResult );
}

/*******************************************************************************
*  BAOB_3x7x_PowerDown
*******************************************************************************/

BERR_Code BAOB_3x7x_PowerDown(
   BAOB_3x7x_Handle hDev            /* [in] Device  handle */
    )
{
    BERR_Code eResult = BERR_SUCCESS;
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BAOB_3x7x_P_MutexTake(hDev);

    if ( hDev->PowerStatus != BAOB_ePower_Off )
    {
        /* Unlock, but only generate an unlock event to the system if previously locked */
        BAOB_3x7x_P_Unlock(hDev,false);
        eResult = BAOB_P_PowerDn(hDev);
        hDev->PowerStatus = BAOB_ePower_Off;
    }

    BAOB_3x7x_P_MutexGive(hDev);

    return ( eResult );
}

/*******************************************************************************
*  BAOB_3x7x_GetPowerStatus
*******************************************************************************/
BERR_Code BAOB_3x7x_GetPowerStatus(
   BAOB_3x7x_Handle hDev,                /* [in] Device  handle */
   BAOB_PowerStatus *pPowerStatus   /* [out] Power status */
    )
{
    BERR_Code eResult = BERR_SUCCESS;
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    if ( NULL == pPowerStatus ) 
    {
        eResult = BERR_INVALID_PARAMETER;
    }
	else 
	{
		*pPowerStatus = hDev->PowerStatus;
	}

    return ( eResult );
}

/*******************************************************************************
*  BAOB_3x7x_ResetStatus
*******************************************************************************/
BERR_Code BAOB_3x7x_ResetStatus(
   BAOB_3x7x_Handle hDev            /* [in] Device  handle */
    )
{
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	return (BAOB_P_Reset_Status(hDev));
}

/******************************************************************************
* BAOB_3x7x_P_TimerFunc()
******************************************************************************/
static void BAOB_P_TimerFunc(void *myParam1, int myParam2)
{

	BAOB_3x7x_Handle hDev = (BAOB_3x7x_Handle) myParam1;
	BDBG_ASSERT( hDev );
	BKNI_SetAnyEvent(hDev->hIntEvent, BKNI_AOB_FLAG);
}

/******************************************************************************
*BAOB_3x7x_ProcessInterruptEvent()
******************************************************************************/

void BAOB_3x7x_ProcessInterruptEvent (BAOB_3x7x_Handle hDev)
{	
    /* Invoked P_Acquire() flag */
    bool bAcquired = false;

    BAOB_3x7x_P_MutexTake(hDev);

    if (hDev->PowerStatus != BAOB_ePower_On) 
    {
        BDBG_ERR(("!!!!!BAOB_3x7x_ProcessInterruptEvent: not powered on"));
    }
    else
    {
        BTMR_StopTimer(hDev->hTimer);

        BAOB_P_GetLock_Status(hDev);
        BAOB_3x7x_P_UpdateLockStatusToSystem(hDev, false);

        if ( (hDev->pAcqParam->BAOB_Acquire_Params.Auto == BAOB_Acquire_Params_eEnable) && (! hDev->autoAcquireMasked) )
        {
            if ( hDev->pAcqParam->BAOB_Acquire_Params.BYP == BAOB_Acquire_Params_eEnable )
            {
                if ( hDev->pStatus->RLK == BAOB_Status_eUnlock )
                {
                    BAOB_P_Acquire(hDev);
                    bAcquired = true;
                }
            }
            else
            {
                if ( hDev->pStatus->FLK == BAOB_Status_eUnlock )
                {
                    BAOB_P_Acquire(hDev);
                    bAcquired = true;
                }
            }
        }

        if ( bAcquired ) 
        {
            BAOB_P_GetLock_Status(hDev);
            BAOB_3x7x_P_UpdateLockStatusToSystem(hDev, false);
        }

        BTMR_StartTimer(hDev->hTimer, BAOB_P_TIMER_IRQ_INTERVAL_USEC);
    }

    BAOB_3x7x_P_MutexGive(hDev);
}

/*******************************************************************************
*  BAOB_3x7x_P_UpdateLockStatusToSystem
*******************************************************************************/
static void BAOB_3x7x_P_UpdateLockStatusToSystem(
   BAOB_3x7x_Handle hDev,            /* [in] Device  handle */
   bool bForceUpdate                 /* [in] force lock/unlock event to system flag */
    )
{
    bool bUpdateLockStatusToSystem = false;
    bool bLocked = false;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    if ( hDev->PowerStatus == BAOB_ePower_On )
	{
        if ( hDev->pStatus->FLK != hDev->FLK_Prev || bForceUpdate )
        {
            hDev->FLK_Prev = hDev->pStatus->FLK;
            bUpdateLockStatusToSystem = true;
        }
    }
    else
    {
        if ( hDev->pStatus->FLK == BAOB_Status_eLock || bForceUpdate )
        {
            hDev->pStatus->FLK = BAOB_Status_eUnlock;
            bUpdateLockStatusToSystem = true;
        }
    }
    bLocked = (hDev->pStatus->FLK == BAOB_Status_eLock);

    if ( bUpdateLockStatusToSystem )
    {
        /* Sys_EvtSet((bLocked ? SysEvent_eOOB_LockAcquired : SysEvent_eOOB_LockLost), SysEvent_AP_eEvtSet, (SysEventData_union_t)(uint32_t)0); */
        Sys_EvtReportLockChg(SysEvt_CompType_eOOB, 0, bLocked ? SysEvt_LockStatus_eLocked : SysEvt_LockStatus_eLockLost);
    }
}

/*******************************************************************************
*  BAOB_3x7x_P_Unlock - note: should be called within the AOB sync. mutex lock
*******************************************************************************/
static void BAOB_3x7x_P_Unlock(
   BAOB_3x7x_Handle hDev,            /* [in] Device  handle */
   bool bForceUnlockToSystem
    )
{
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    /* Stop auto acquire (momentarily until acquired) */
    BTMR_StopTimer(hDev->hTimer);
    /* Stop auto-acquire from running until acquire is commanded */
    hDev->autoAcquireMasked  = true; 
    /* Force unlock and generate system unlock event */
    hDev->pStatus->FLK = BAOB_Status_eUnlock;
    BAOB_3x7x_P_UpdateLockStatusToSystem(hDev,bForceUnlockToSystem);
}

/*******************************************************************************
*  BAOB_3x7x_Unlock
*******************************************************************************/
BERR_Code BAOB_3x7x_Unlock(
   BAOB_3x7x_Handle hDev            /* [in] Device  handle */
    )
{
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    
    BAOB_3x7x_P_MutexTake(hDev);

    /* Unlock and force unlock event to be generated to the system */
    BAOB_3x7x_P_Unlock(hDev,true);

    BAOB_3x7x_P_MutexGive(hDev);

    return ( BERR_SUCCESS );
}


/***************************************************************************
BAOB_3x7x_InstallAobCallback
****************************************************************************/
BERR_Code BAOB_3x7x_InstallAobCallback(BAOB_3x7x_Handle				hDev,
									  BAOB_3x7x_AobCallback			fCallBack,
									  void                          *pParam1
												)
{
   hDev->pCallback[BAOB_Callback_eTuner] = (BAOB_3x7x_AobCallback)fCallBack;
   hDev->pCallbackParam[BAOB_Callback_eTuner] = pParam1;
   return BERR_SUCCESS;
}

/******************************************************************************
BAOB_3x7x_RemoveAobCallback()
******************************************************************************/ 
BERR_Code BAOB_3x7x_RemoveAobCallback(
    BAOB_3x7x_Handle			hDev  
)
{
    BDBG_ASSERT(hDev);
    
    hDev->pCallback[BAOB_Callback_eTuner] = NULL;
    hDev->pCallbackParam[BAOB_Callback_eTuner] = NULL;

    return BERR_SUCCESS;
} 

/******************************************************************************
BAOB_3x7x_GetComponentVersion()
******************************************************************************/
BERR_Code BAOB_3x7x_GetComponentVersion(
    BAOB_3x7x_Handle hDev,
    void *pVersion
)
{
    BSTD_UNUSED(hDev);
#ifdef LEAP_BASED_CODE    
    BComponent_VerInfo_t *pMesgData = (BComponent_VerInfo_t *)pVersion;

    pMesgData->uMajor = BAOB_COMPONENT_VERSION_MAJOR;
    pMesgData->uMinor = BAOB_COMPONENT_VERSION_MINOR;
    pMesgData->uType = BAOB_COMPONENT_VERSION_BUILD_TYPE;
    pMesgData->uBuildId = BAOB_COMPONENT_VERSION_BUILD_ID;
#else
    /* 75xx */
    BSTD_UNUSED(pVersion);
#endif

    return  BERR_SUCCESS;

}

#endif /* (BLEAP_AOB_ENABLED) */

