/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 3/20/09 4:24p $
 * $brcm_Workfile: bdbg_os_priv.c $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/ucos/bdbg_os_priv.c $
 * 
 * Hydra_Software_Devel/6   3/20/09 4:24p cnovak
 * PR51415: Fix release build compile warnings. Detab file.
 * 
 * Hydra_Software_Devel/5   2/18/09 11:22a cnovak
 * PR51415: In BDBG_P_Lock/Unlock, call bkni's CHECK_CRITICAL to know
 * whether or not to mutex lock.
 * 
 * Hydra_Software_Devel/4   1/26/09 5:20p cnovak
 * PR51415: rework bdbg_os_priv interface to not suggest BKNI_EventHandle
 * usage in BDBG_P_Lock, which must lock in both task and isr contexts
 * 
 * Hydra_Software_Devel/3   1/21/09 2:50p cnovak
 * PR24626: Now that assert checks have been added to bkni for context
 * validity, BDBG_P_Lock/Unlock can not use the BKNI interface for its
 * mutex.
 * 
 * Hydra_Software_Devel/2   6/19/08 4:05p cnovak
 * PR43697: Move to Hydra branch.
 * 
 * Platform_Software_Devel/3   7/31/06 5:21p cnovak
 * PR12841: Add support for task-mode uC/OS driver.
 * 
 * Platform_Software_Devel/2   5/17/06 6:11p cnovak
 * PR21065: Add BDBG_P_Lock/Unlock
 * 
 * Platform_Software_Devel/1   3/8/06 1:07p cnovak
 * PR 12841: From HDI97401_Integration
 * 
 ***************************************************************************/

/* uCOS include files */
#include "type.h"
#include "ucos.h"
#include "ucosdrv.h"
 
#include "bstd.h"
#include "bkni.h"
#include "bdbg_os_priv.h"

/****************************************************************************
    Global functions
****************************************************************************/
/* tap into bkni's function to check for a BKNI critical section. It's ugly,
   but there is no BKNI standard interface to do this.
*/
extern inline bool CHECK_CRITICAL(void);

/****************************************************************************
    Static data
****************************************************************************/
/* platform spefic type for timestamps */
static ULONG initTicks;
/* OS-specific mutex */
static OS_EVENT * g_pBdbgMutex = NULL;

/****************************************************************************
****************************************************************************/
void
BDBG_P_InitializeTimeStamp(void)
{
    initTicks = OSTimeGet();
}

/****************************************************************************
****************************************************************************/
void
BDBG_P_GetTimeStamp(char *timeStamp, int size_t)
{
    ULONG currentTicks;
    int hours, minutes, seconds;
    int milliseconds;
    int rc;
    
    currentTicks = OSTimeGet();

    /* 1 tick every 10 ms */
    milliseconds = (currentTicks - initTicks)*10;
    
    /* Calculate the time   */
    hours = (milliseconds)/3600000;
    milliseconds = milliseconds % 3600000;
    minutes = (((milliseconds)/60000))%60000;
    milliseconds = milliseconds % 60000;
    seconds = (milliseconds/1000)%1000;
    milliseconds = milliseconds % 1000;
        
    /* print the formatted time including the milliseconds  */
    rc = BKNI_Snprintf(timeStamp, size_t, "%02u:%02u:%02u.%03u", hours, minutes, seconds, milliseconds);
    return;
}

/****************************************************************************
****************************************************************************/
BERR_Code 
BDBG_P_OsInit(void)
{
    BERR_Code rc;
    
    /* This lock needs to work between both ISR and Task contexts. We're using
       a mutex because we only have a tiny amount of code running in ISR 
       context -- the L1 handler -- and we don't expect it to need the BDBG 
       lock.
       
       The lock is only used when a debug module is registered for the first
       time.
       
       We need to go directly to the OS for our mutex because we have code
       in BKNI which checks to make sure no one is trying to call AcquireMutex
       from within a Magnum ISR context. 
    */
    if (g_pBdbgMutex != NULL) {
        rc = BERR_TRACE(BERR_UNKNOWN);
        BKNI_Fail();
    }

    g_pBdbgMutex = OSSemCreate(1);
    if (g_pBdbgMutex == NULL) {
        rc = BERR_TRACE(BERR_OS_ERROR);
        BKNI_Fail();
    }

    return BERR_SUCCESS;
}

/****************************************************************************
****************************************************************************/
void 
BDBG_P_OsUninit(void)
{
    /* No way to release kernel object in uCOS */
    BKNI_Printf("%s: uCOS Event Leak - no method to destroy semaphore\n", __FUNCTION__);
}

/****************************************************************************
****************************************************************************/
void 
BDBG_P_Lock(void)
{
    UBYTE ucosError;

    if (g_pBdbgMutex == NULL) {
        ucosError = BERR_TRACE(BERR_NOT_INITIALIZED);
        return;
    }
    
    if (CHECK_CRITICAL()) {
        return;
    }
    
    /* Wait forever on this mutex */
    OSSemPend(g_pBdbgMutex, 0, &ucosError); 

    if (ucosError != OS_NO_ERR) {
        ucosError = BERR_TRACE(ucosError);
    }
}

/****************************************************************************
****************************************************************************/
void 
BDBG_P_Unlock(void)
{
    BERR_Code rc;
    uint32_t mutexCount;

    if (g_pBdbgMutex == NULL) {
        rc = BERR_TRACE(BERR_NOT_INITIALIZED);
        return;
    }

    if (CHECK_CRITICAL()) {
        return;
    }
    
    OS_ENTER_CRITICAL();
    mutexCount = g_pBdbgMutex->OSEventCnt;
    OS_EXIT_CRITICAL();
    if (mutexCount > 0) {
        /* Mutex already posted. Don't do it again */
        rc = BERR_TRACE(mutexCount);
    } else {
        OSSemPost(g_pBdbgMutex);
    }
}

/* End of file */

