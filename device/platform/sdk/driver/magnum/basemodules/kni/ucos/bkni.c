/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni.c $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 3/10/10 3:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kni/ucos/bkni.c $
 * 
 * Hydra_Software_Devel/14   3/10/10 3:07p mward
 * SW7400-2712:  Add annotation to indicate to Coverity that BKNI_Fail()
 * is a "kill path".
 * 
 * Hydra_Software_Devel/13   5/14/09 10:55a cnovak
 * PR43697: Create a free pool for mutexes and events that are destroyed.
 * uCOS does not provide a "destroy" method.
 * 
 * Hydra_Software_Devel/12   3/20/09 4:28p cnovak
 * PR12841: Fix release build compile warnings. Detab file.
 * 
 * Hydra_Software_Devel/11   2/18/09 11:28a cnovak
 * PR51415: tabs to spaces for consistency. No code changes.
 * 
 * Hydra_Software_Devel/10   2/18/09 11:23a cnovak
 * PR51415: Make CHECK_CRITICAL an inline function so it can be called by
 * BDBG_P_Lock/Unlock.
 * 
 * Hydra_Software_Devel/9   1/27/09 5:53p cnovak
 * PR43697: Fix deadlock case in BKNI_SetEvent caused by 1) the fact that
 * SetEvent can be called by both task and ISR (CriticalSection) contexts
 * and 2) the use of a mutex to lock the group event list. Check
 * CriticalSection flag and create _isr version of SetEvent.
 * NOTE: This will not work if SetEvent is called from a reall interrupt
 * context on an event that is part of a group.
 * 
 * Hydra_Software_Devel/8   1/26/09 5:23p cnovak
 * PR24626: Initialize g_csOwner to a non-zero value.
 * 
 * Hydra_Software_Devel/7   1/21/09 2:55p cnovak
 * PR24626: Now that assert checks have been added to test context
 * validity, BKNI functions can not use the kni API for mutex handling.
 * Modified to go straight to the OS interface.
 * Restore original implementation of WaitForGroup.
 * 
 * Hydra_Software_Devel/6   1/19/09 1:58p cnovak
 * PR32280: Add BKNI_AssertIsrContext functionality, merged from other OS
 * ports.
 * 
 * Hydra_Software_Devel/5   1/16/09 1:13p cnovak
 * PR43697: In WaitForGroup, call WaitForEvent before checking for
 * signaled events. This is required in order to clear the event status
 * in cases where the event was signaled before driver called
 * WaitForGroup.
 * 
 * Hydra_Software_Devel/4   10/29/08 3:31p cnovak
 * PR43697: Re-enable the OS schedule lock/unlock in BKNI CriticalSection.
 * See note in EnterCriticalSection as to why this is necessary.
 * 
 * Hydra_Software_Devel/3   8/25/08 3:08p cnovak
 * PR43697: Add BKNI_Vsnprintf.
 * 
 * Hydra_Software_Devel/2   6/19/08 5:40p cnovak
 * PR43697: Remove unused code in BKNI_Delay.
 * 
 * Hydra_Software_Devel/1   6/19/08 4:30p cnovak
 * PR43697: Clean up uCOS port and move back to Hyrdra branch.
 * 
 * Platform_Software_Devel/5   1/8/08 10:59a agadre
 * PR12841: Fix BKNI_Delay
 * 
 * Platform_Software_Devel/4   5/25/07 7:53a cnovak
 * PR12841: CSP64223: When using Task-Mode drivers, lock the OS task
 * scheduler during BKNI CriticalSection to prevent any chance of
 * prioirty inversion.
 * 
 * Platform_Software_Devel/3   7/31/06 5:57p cnovak
 * PR12841: Print build message when compiling Task Mode driver.
 * 
 * Platform_Software_Devel/2   7/31/06 5:23p cnovak
 * PR12841: Add support for task-mode uC/OS driver. Remove excess BDBG
 * messages.
 * 
 * Platform_Software_Devel/1   7/25/06 6:17p cnovak
 * PR12841: KNI compliant functions put in bkni.c
 * 
 ***************************************************************************/

/****************************************************************************
****************************************************************************/
/*
    NOTE: 
    This is (or should be) a compliant implementation of the Magnum
    KNI interface. All non-compliant functions are in bkni_ucos.c. 
    
    The non-compliant functions in bkni_ucos.c still carry the BKNI_
    name, but that will be changed.

    DO NOT IMPLEMENT NON-COMPLIANT KNI FUNCTIONS. At least not in here 
*/
/****************************************************************************
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* uCos/HDI include files */
#include "type.h"
#include "ucos.h"
#include "ucosdrv.h"
#include "ucospriv.h"

/* Magnum include files */
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bkni_event_group.h"
#include "blst_list.h"
#include "blst_slist.h"

/* uCOS doesn't give us the same functionality as Linux & VxWorks. Instead 
   of implementing platform-specific code in here, use a helper shim. 
   Example: BKNI_Delay, as implemented, needs to know the CPU clock and makes
   direct access to the MIPS CP0 registers. Move that implementation to the 
   shim.
*/
#include "b_ucos.h"

BDBG_MODULE(kni);

/* needed to support tagged interface */
#undef BKNI_Delay
#undef BKNI_Sleep
#undef BKNI_CreateEvent
#undef BKNI_DestroyEvent
#undef BKNI_WaitForEvent
#undef BKNI_SetEvent
#undef BKNI_EnterCriticalSection
#undef BKNI_LeaveCriticalSection
#undef BKNI_CreateMutex
#undef BKNI_DestroyMutex
#undef BKNI_TryAcquireMutex
#undef BKNI_AcquireMutex
#undef BKNI_ReleaseMutex

/****************************************************************************
    Defines
****************************************************************************/
#define MSEC2TICKS_NOINFINITE(msec)        ((msec) == 0 ? 1 : (((msec)+9)/10))
#define MSEC2TICKS(msec)                   ((msec) == BKNI_INFINITE ? 0 : MSEC2TICKS_NOINFINITE(msec))

/****************************************************************************
    Data Structures
****************************************************************************/
typedef struct BKNI_GroupObj {
    BLST_D_HEAD(group, BKNI_EventObj) members;
    OS_EVENT * pLock;
    BKNI_EventHandle hEvent;
} BKNI_GroupObj;

typedef struct BKNI_EventObj {
    BLST_S_ENTRY(BKNI_EventObj) link;
    BLST_D_ENTRY(BKNI_EventObj) list;
    OS_EVENT * pOsEvent;
    BKNI_GroupObj * group;
} BKNI_EventObj;

typedef struct BKNI_MutexObj {
    BLST_S_ENTRY(BKNI_MutexObj) link;
    OS_EVENT * pOsEvent;
} BKNI_MutexObj;


/****************************************************************************
    These macros were originally designed to be used in a debug build, 
    but it appears that we need them all the time because we can't 
    tell when BKNI_SetEvent or BDBG_P_Lock has been called from a
    CriticalSection. 
****************************************************************************/
#if 1 /* BDBG_DEBUG_BUILD */

static uint32_t g_BKNI_CsOwner = (uint32_t)-1;

#define SET_CRITICAL() do { g_BKNI_CsOwner = OSGetTaskID(); } while (0)
#define CLEAR_CRITICAL() do { g_BKNI_CsOwner = (uint32_t)-1; } while (0)
/* BDBG_P_Lock/Unlock needs this information also. Make this a public function
   instead of a macro
*/
//#define CHECK_CRITICAL() ( g_BKNI_CsOwner == OSGetTaskID() )
inline bool CHECK_CRITICAL(void) { return (g_BKNI_CsOwner == OSGetTaskID()); }

#define ASSERT_CRITICAL() do \
{\
    if ( !CHECK_CRITICAL() )\
    {\
        BKNI_Printf("Error, must be in critical section to call %s [%d]\n", __FUNCTION__, g_BKNI_CsOwner);\
        BKNI_Fail();\
    }\
} while (0)

#define ASSERT_NOT_CRITICAL() do \
{\
    if ( CHECK_CRITICAL() )\
    {\
        BKNI_Printf("Error, must not be in critical section to call %s [%d]\n", __FUNCTION__, g_BKNI_CsOwner);\
        BKNI_Fail();\
    }\
} while (0)

#else

#define ASSERT_CRITICAL() (void)0
#define ASSERT_NOT_CRITICAL() (void)0
#define SET_CRITICAL() (void)0
#define CLEAR_CRITICAL() (void)0
#define CHECK_CRITICAL() 0

#endif

/****************************************************************************
    Module globals
****************************************************************************/
static BKNI_MutexHandle ghCriticalSectionMutex = NULL; 

/* uCOS does not provide any methods for destroying allocated OS_EVENTs. This means
   we have a resource leak any time a BKNI client uses one of the destroy methods:
   
   BKNI_DestroyMutex
   BKNI_DestroyEvent

   Create a free-pool list for these objects. Anytime a mutex or event is released, add it 
   to a free pool, and always check the free pool before allocating any new events from
   uCOS.
*/

/* Create a new list head for our MUTEX free pool */
static BLST_S_HEAD(BKNI_P_MutexFreePool, BKNI_MutexObj) gMutexFreePool = BLST_S_INITIALIZER(gMutexFreePool); 
/* Create a new list head for our EVENT free pool */
static BLST_S_HEAD(BKNI_P_EventFreePool, BKNI_EventObj) gEventFreePool = BLST_S_INITIALIZER(gEventFreePool); 


/****************************************************************************
    Static function prototypes
****************************************************************************/
static void BKNI_P_PostSemaphore(OS_EVENT * pEvent);
static BERR_Code BKNI_P_GetTime(unsigned long *pulMsec);

/****************************************************************************
****************************************************************************/
BERR_Code 
BKNI_Init(void)
{
    BERR_Code rc;
    
    /* Create our CriticalSection Mutex */
    rc = BKNI_CreateMutex(&ghCriticalSectionMutex);
    if (rc != BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
    }

    return BERR_SUCCESS;
}

/****************************************************************************
****************************************************************************/
void 
BKNI_Uninit(void)
{
    if (ghCriticalSectionMutex) {
        BKNI_DestroyMutex(ghCriticalSectionMutex);
    }
}

/****************************************************************************
    Don't use BDBG_ messages in BKNI_CreateMutex -- we get called during
    KNI & DBG init.
****************************************************************************/
BERR_Code 
BKNI_CreateMutex(BKNI_MutexHandle *mutex)
{
    BKNI_MutexObj * pMutexObj;
    bool bAllocNewOsEvent = false;

    ASSERT_NOT_CRITICAL();

    *mutex = NULL;

    /* First, check our free pool */
    OS_ENTER_CRITICAL();
    pMutexObj = BLST_S_FIRST(&gMutexFreePool);
    if (pMutexObj == NULL) {
        OS_EXIT_CRITICAL();
        bAllocNewOsEvent = true;
        pMutexObj = (BKNI_MutexObj *)BKNI_Malloc(sizeof(BKNI_MutexObj));
    } else {
        BLST_S_REMOVE_HEAD(&gMutexFreePool, link);
        OS_EXIT_CRITICAL();
    }

    if (pMutexObj == NULL) {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    if (bAllocNewOsEvent == true) {
        pMutexObj->pOsEvent = OSSemCreate(1);
        if (pMutexObj->pOsEvent == NULL) {
            BKNI_Free(pMutexObj);
            return BERR_TRACE(BERR_OS_ERROR); 
        }
    } else {
        /* Initialize the Sem count for this mutex. We don't have to do this
           in a critical section because the sem isn't being used right now 
        */
        pMutexObj->pOsEvent->OSEventCnt = 1;
    }

    *mutex = (BKNI_MutexHandle)pMutexObj;

    return BERR_SUCCESS;
}

/****************************************************************************
****************************************************************************/
void
BKNI_DestroyMutex(BKNI_MutexHandle mutex)
{
    ASSERT_NOT_CRITICAL();

    /* uCos doesn't have a way to destroy the semaphore. Add it to our free pool */
    if (mutex) {
        OS_ENTER_CRITICAL();
        BLST_S_INSERT_HEAD(&gMutexFreePool, (BKNI_MutexObj *)mutex, link);
        OS_EXIT_CRITICAL();
        mutex = NULL;
    }

    return;
}

/****************************************************************************
****************************************************************************/
BERR_Code 
BKNI_TryAcquireMutex(BKNI_MutexHandle mutex)
{
    BERR_Code errCode = BERR_SUCCESS;

    ASSERT_NOT_CRITICAL();

    if (!mutex || !(mutex->pOsEvent)) {
        return BERR_INVALID_PARAMETER;
    }

    /* See if we can acquire this mutex. If so, grab it and go. 
       If not, return a timeout error. Don't block.
    */
    OS_ENTER_CRITICAL();
    if (mutex->pOsEvent->OSEventCnt) {
        /* We know it's available -- grab it */
        mutex->pOsEvent->OSEventCnt--;
        errCode = BERR_SUCCESS;
    } else {
        errCode = BERR_TIMEOUT;
    }
    OS_EXIT_CRITICAL();

    return errCode;
}

/****************************************************************************
****************************************************************************/
BERR_Code 
BKNI_AcquireMutex(BKNI_MutexHandle mutex)
{
    UBYTE       ucosErr;
    BERR_Code   errCode = BERR_SUCCESS;

    ASSERT_NOT_CRITICAL();

    if (!mutex || !(mutex->pOsEvent)) {
        return BERR_INVALID_PARAMETER;
    }

    /* Wait forever on this mutex */
    OSSemPend(mutex->pOsEvent, 0, &ucosErr); 

    switch( ucosErr ) {
        case OS_NO_ERR: 
            break;
        case OS_TIMEOUT:
            errCode = BERR_TIMEOUT;
            break;
        default:
            errCode = BERR_OS_ERROR;
            break;
    }
    
    return errCode;
}

/****************************************************************************
****************************************************************************/
void
BKNI_ReleaseMutex(BKNI_MutexHandle mutex)
{
    ASSERT_NOT_CRITICAL();

    if (mutex && mutex->pOsEvent) {
        BKNI_P_PostSemaphore(mutex->pOsEvent);
    }
}

/****************************************************************************
****************************************************************************/
/* coverity[+kill]  */
void 
BKNI_Fail(void)
{
    volatile int i;
    
    BKNI_Printf("BKNI_Fail causing intentional system fault. Inspect prior error messages to determine failure cause.\n");

    i = *(int *)0; /* try to fail, take 1 */
    i = 0;
    i = 1/i;  /* try to fail, take 2 */
}


/****************************************************************************
****************************************************************************/
int 
BKNI_Printf(const char *fmt, ...)
{
    va_list arglist;
    int rc;

    va_start( arglist, fmt );
    rc = vprintf(fmt, arglist);
    va_end(arglist);

    return rc;
}


/****************************************************************************
****************************************************************************/
int 
BKNI_Snprintf(char *str, size_t len, const char *fmt, ...)
{
    BERR_Code err;
    va_list arglist;
    int rc;

    va_start( arglist, fmt );
    rc = vsprintf(str, fmt, arglist);
    va_end(arglist);
    if ((size_t)rc > len) {
        /* Buffer overflow */
        err = BERR_TRACE(BERR_UNKNOWN);
    }
    return rc;
}

/****************************************************************************
****************************************************************************/
int 
BKNI_Vprintf(const char *fmt, va_list ap)
{
    return vprintf(fmt, ap);
}

/****************************************************************************
****************************************************************************/
int 
BKNI_Vsnprintf(char *s, size_t n, const char *fmt, va_list ap)
{
    return vsnprintf(s, n, fmt, ap);
}

/****************************************************************************
****************************************************************************/
void
BKNI_Delay(unsigned int microsec)
{
    b_ucos_delay(microsec);
}


/****************************************************************************
****************************************************************************/
BERR_Code 
BKNI_Sleep(unsigned int millisec)
{
    uint32_t ticks = MSEC2TICKS(millisec);
    uint32_t oldT, res;

    ASSERT_NOT_CRITICAL();

    if (OSRunning) {
        OSTimeDly(ticks);
    } else {
        oldT = OSTimeGet();
        do {
            res = OSTimeGet() - oldT;
            if (res > 0x7ffff) { /* clock swap */
                res += 0xffffffffL;
            }       
        } 
        while(res<ticks);
    }
    return BERR_SUCCESS;
}

/****************************************************************************
****************************************************************************/
BERR_Code 
BKNI_CreateEvent(BKNI_EventHandle * event)
{
    BKNI_EventObj * pEventObj;
    bool bAllocNewOsEvent = false;

    ASSERT_NOT_CRITICAL();

    *event = NULL;

    /* First, check our free pool */
    OS_ENTER_CRITICAL();
    pEventObj = BLST_S_FIRST(&gEventFreePool);
    if (pEventObj == NULL) {
        OS_EXIT_CRITICAL();
        bAllocNewOsEvent = true;
        pEventObj = (BKNI_EventObj *)BKNI_Malloc(sizeof(BKNI_EventObj));
    } else {
        BLST_S_REMOVE_HEAD(&gEventFreePool, link);
        OS_EXIT_CRITICAL();
    }

    if (pEventObj == NULL) {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    if (bAllocNewOsEvent == true) {
        pEventObj->pOsEvent = OSSemCreate(0);
        if (pEventObj->pOsEvent == NULL) {
            BKNI_Free(pEventObj);
            return BERR_TRACE(BERR_OS_ERROR);
        }
    } else {
        /* Initialize the Sem count for this event. We don't have to do this
           in a critical section because the sem isn't being used right now 
        */
        pEventObj->pOsEvent->OSEventCnt = 0;
    }

    pEventObj->group = NULL;

    *event = pEventObj;
    
    return BERR_SUCCESS;
}

/****************************************************************************
****************************************************************************/
void
BKNI_DestroyEvent(BKNI_EventHandle event)
{
    ASSERT_NOT_CRITICAL();

    if (event) {
        OS_ENTER_CRITICAL();
        BLST_S_INSERT_HEAD(&gEventFreePool, (BKNI_EventObj *)event, link);
        OS_EXIT_CRITICAL();
    }
    
    return;
}

/****************************************************************************
****************************************************************************/
BERR_Code 
BKNI_WaitForEvent(BKNI_EventHandle event, int timeoutMsec)
{
    BERR_Code   errCode = BERR_SUCCESS;
    UBYTE       ucosErr;
    
    int ticks;

    if (!event || !(event->pOsEvent)) {
        return BERR_INVALID_PARAMETER;
    }
    
    if (timeoutMsec != 0) {
        ASSERT_NOT_CRITICAL();
    }

    /* According to PI documentation, a timeout of 0 tells us to 
       check for an event signal and return immediately. We're not supposed
       to let it get scheduled because an ISR is allowed to call this
       ONLY if the timeout is 0.
    */
    if (timeoutMsec == 0) {
        OS_ENTER_CRITICAL();
        if (event->pOsEvent->OSEventCnt) {
            event->pOsEvent->OSEventCnt--;
            errCode = BERR_SUCCESS;
        } else {
            errCode = BERR_TIMEOUT;
        }
        OS_EXIT_CRITICAL();
        return errCode;
    }

    if (timeoutMsec == BKNI_INFINITE) {
        ticks = 0;
    } else {
        if (timeoutMsec < 30) {
            /* NOTE: The Linux kernel kni interface a 30ms minimum timeout -- why? */
            timeoutMsec = 30;
        }
        /* This macro forces us to round up to the next 10ms */
        ticks = MSEC2TICKS(timeoutMsec);
    }

    OSSemPend(event->pOsEvent, ticks, &ucosErr); 

    switch( ucosErr ) {
        case OS_NO_ERR: 
            break;
        case OS_TIMEOUT:
            errCode = BERR_TIMEOUT;
            break;
        default:
            errCode = BERR_OS_ERROR;
            break;
    }

    return errCode;
}

/****************************************************************************
    By current definition, BKNI_SetEvent can be called from both task and 
    interrupt context (interrupt means ISR or CriticalSection). Unfortunately,
    BKNI_SetEvent_isr is permanently mapped to BKNI_SetEvent.
    
    Since this is the case, we need to add logic to know whether or not we are 
    inside a critical section so we can process accordingly. 
    
    NOTE: I do not expect BKNI_SetEvent to be called from real interrupt 
    context (as opposed to our high-priority ISR task) because I have no way
    of knowing we're actually running in interrupt context. I only know we're
    in a CriticalSection, which we enter before we call Magnum ISRs. 
****************************************************************************/
static void
BKNI_P_SetEvent_isr(BKNI_EventHandle event)
{
    OS_STATUS ucosStatus;

    if (event && event->pOsEvent) {
        BKNI_GroupObj *group = event->group;

        /* We're in ISR context (either a real ISR or a CriticalSection), so 
           the OS scheduler is already being held. Don't attempt to lock it again.
        */
        /* prevent the event count saturating */
        ucosStatus = OSSemPost(event->pOsEvent);
        if (event->pOsEvent->OSEventCnt > 1) {
            event->pOsEvent->OSEventCnt = 1;
        }

        if (ucosStatus != OS_NO_ERR) {
            ucosStatus = BERR_TRACE(ucosStatus);
        }
        /* If this event is part of a group, signal the group event */
        if (group) {
            BKNI_P_SetEvent_isr(group->hEvent);
        }
    }
}

void
BKNI_SetEvent(BKNI_EventHandle event)
{
    UBYTE ucosError;
    OS_STATUS ucosStatus;

    /* */
    if (CHECK_CRITICAL()) {
        BKNI_P_SetEvent_isr(event);
        return;
    }
    
    if (event && event->pOsEvent) {
        BKNI_GroupObj *group = event->group;
        if (group) {
            OSSemPend(group->pLock, 0, &ucosError); 
            if (ucosError != OS_NO_ERR) {
                ucosError = BERR_TRACE(ucosError);
            }
        }
        /* Hold the the scheduler while we prevent the event count saturating */
        OSSchedLock();
        ucosStatus = OSSemPost(event->pOsEvent);
        if (event->pOsEvent->OSEventCnt > 1) {
            event->pOsEvent->OSEventCnt = 1;
        }
        OSSchedUnlock();
        /* event may now be stale.... */
        if (ucosStatus != OS_NO_ERR) {
            ucosStatus = BERR_TRACE(ucosStatus);
        }
        if (group) {
            BKNI_SetEvent(group->hEvent);
            BKNI_P_PostSemaphore(group->pLock);
        }
    }
}

/****************************************************************************
****************************************************************************/
void
BKNI_ResetEvent(BKNI_EventHandle event)
{
    BDBG_ASSERT((((int) event) % 4) == 0);
    BDBG_ASSERT((((int) event->pOsEvent) % 4) == 0);
    if (event && event->pOsEvent) {
        event->pOsEvent->OSEventCnt = 0;
    }
}

/****************************************************************************
****************************************************************************/
void 
BKNI_EnterCriticalSection(void)
{
    BERR_Code rc;

    ASSERT_NOT_CRITICAL();

    /* Stop the task scheduler - This is to prevent priority inversion issues.
       An example of a potential issue is:
       
       1. a low priority task enters a BKNI critical section
       2. an I2C interrupt occurs
       3. the interrupt task runs, then blocks on a BKNI critical section
       4. the low priority task starts running again
       5. a medium priority task preempts the low priority task
       
       Result: interrupts are blocked for an indeterminate time, and I2C 
       interrupt is lost.
       
       Locking the scheduler prevents a higher priority task from running
       while we're in a BKNI critical section, allowing us to get out as
       quickly as possible.
    */
    if (OSRunning) {
        OSSchedLock();
    }   

    rc = BKNI_AcquireMutex(ghCriticalSectionMutex);
    if (rc != BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        BKNI_Fail();
    }

    SET_CRITICAL();
}

/****************************************************************************
****************************************************************************/
void
BKNI_LeaveCriticalSection(void)
{
    ASSERT_CRITICAL();
    CLEAR_CRITICAL();

    BKNI_ReleaseMutex(ghCriticalSectionMutex);

    /* Re-enable the task schedular */
    if (OSRunning) {
        OSSchedUnlock();
    }
    return; 
}

/****************************************************************************
    Event Group Functions
****************************************************************************/
BERR_Code 
BKNI_CreateEventGroup(BKNI_EventGroupHandle *pGroup)
{
    BKNI_EventGroupHandle group;
    BERR_Code result = BERR_SUCCESS;

    group = (BKNI_EventGroupHandle)BKNI_Malloc(sizeof(*group));
    if (!group) {
        result = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_no_memory;
    }
    BLST_D_INIT(&group->members);
    
    group->pLock = OSSemCreate(1);
    if (group->pLock == NULL) {
        result = BERR_TRACE(BERR_OS_ERROR);
        goto err_mutex;
    }
    result = BKNI_CreateEvent(&(group->hEvent));
    if (result != BERR_SUCCESS) {
        result = BERR_TRACE(result);
        goto err_event;
    }

    *pGroup = group;

    return BERR_SUCCESS;

err_event:
    /* uCOS does not have a method to destroy a semaphore */
    BDBG_ERR(("%s: uCOS Event Leak - no method to destroy semaphore", __FUNCTION__));
err_mutex:
    BKNI_Free((void *)group);
err_no_memory:
    return result;
}

/****************************************************************************
****************************************************************************/
void
BKNI_DestroyEventGroup(BKNI_EventGroupHandle group)
{
    UBYTE ucosError;
    BKNI_EventHandle event;

    OSSemPend(group->pLock, 0, &ucosError); 
    if (ucosError != OS_NO_ERR) {
        ucosError = BERR_TRACE(ucosError);
        BDBG_ASSERT(false);
    }
    while( NULL != (event=BLST_D_FIRST(&group->members)) ) {
        event->group = NULL;
        BLST_D_REMOVE_HEAD(&group->members, list);
    }
    BKNI_P_PostSemaphore(group->pLock);
    BDBG_ERR(("%s: uCOS Event Leak - no method to destroy semaphore", __FUNCTION__));
    BKNI_DestroyEvent(group->hEvent);
    BKNI_Free((void *)group);

    return;
}

/****************************************************************************
****************************************************************************/
BERR_Code
BKNI_AddEventGroup(BKNI_EventGroupHandle group, BKNI_EventHandle event)
{
    UBYTE ucosError;
    BERR_Code result = BERR_SUCCESS;

    /* IMPORTANT: group lock shall be acquired before event lock */
    OSSemPend(group->pLock, 0, &ucosError); 
    if (ucosError != OS_NO_ERR) {
        ucosError = BERR_TRACE(ucosError);
        BDBG_ASSERT(false);
    }
    if (event->group != NULL) {
        BDBG_ERR(("Event %#x already connected to group %#x", (unsigned)event, (unsigned)group));
        result = BERR_TRACE(BERR_OS_ERROR);
    } else {
        bool bIsSet;
        BLST_D_INSERT_HEAD(&group->members, event, list);
        event->group = group;
        OS_ENTER_CRITICAL();
        bIsSet = event->pOsEvent->OSEventCnt > 0;
        OS_EXIT_CRITICAL();
        if (bIsSet) { /* kinda racey */
            BKNI_SetEvent(group->hEvent);
        }
    }
    BKNI_P_PostSemaphore(group->pLock);

    return result;
}

/****************************************************************************
****************************************************************************/
BERR_Code
BKNI_RemoveEventGroup(BKNI_EventGroupHandle group, BKNI_EventHandle event)
{
    UBYTE ucosError;
    BERR_Code result = BERR_SUCCESS;

    OSSemPend(group->pLock, 0, &ucosError); 
    if (ucosError != OS_NO_ERR) {
        ucosError = BERR_TRACE(ucosError);
        BDBG_ASSERT(false);
    }
    if (event->group != group) {
        BDBG_ERR(("Event %#x doesn't belong to group %#x", event, group));
        result = BERR_TRACE(BERR_OS_ERROR);
    } else {
        BLST_D_REMOVE(&group->members, event, list);
        event->group = NULL;
    }
    BKNI_P_PostSemaphore(group->pLock);

    return result;
}

/****************************************************************************
****************************************************************************/
static unsigned
group_get_events(BKNI_EventGroupHandle group, BKNI_EventHandle *events, unsigned max_events)
{
    UBYTE ucosError;
    BKNI_EventHandle ev;
    unsigned event;

    OSSemPend(group->pLock, 0, &ucosError); 
    if (ucosError != OS_NO_ERR) {
        ucosError = BERR_TRACE(ucosError);
        BDBG_ASSERT(false);
    }
    for(event=0, ev=BLST_D_FIRST(&group->members); ev && event<max_events ; ev=BLST_D_NEXT(ev, list)) {
        bool bIsSet;
        OS_ENTER_CRITICAL();
        if ((bIsSet = (ev->pOsEvent->OSEventCnt > 0)) == true) {
            ev->pOsEvent->OSEventCnt = 0;
        }
        OS_EXIT_CRITICAL();
        if (bIsSet) {
            events[event] = ev;
            event++;
        }
    }
    BKNI_P_PostSemaphore(group->pLock);

    return event;
}

/****************************************************************************
****************************************************************************/
BERR_Code
BKNI_WaitForGroup(BKNI_EventGroupHandle group, int timeoutMsec, BKNI_EventHandle *events, unsigned max_events, unsigned *nevents)
{
    BERR_Code result;
    unsigned long target, now;
    bool forever = false;

    ASSERT_NOT_CRITICAL();

    if (max_events < 1) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if (timeoutMsec == BKNI_INFINITE) {
        forever = true;
    } else if (timeoutMsec == 0) {
        BKNI_WaitForEvent(group->hEvent, 0); /* just clear event */
        *nevents = group_get_events(group, events, max_events);
        return *nevents ? BERR_SUCCESS : BERR_TIMEOUT;
    } else {
        BKNI_P_GetTime(&target);
        target += timeoutMsec;
    }

    for(;;) {
        /* Make sure we clear the group's event status by calling WaitForEvent.
           Other OS ports check their event list immediately, but they don't 
           use an Event for the group itself.
        */
        result = BKNI_WaitForEvent(group->hEvent, forever ? BKNI_INFINITE : timeoutMsec);
        if (result == BERR_SUCCESS) {
            *nevents = group_get_events(group, events, max_events);
            if (*nevents) {
                return BERR_SUCCESS;
            }
        } else if (result == BERR_TIMEOUT) {
            *nevents = 0;
            return BERR_TIMEOUT;
        } else {
            return BERR_TRACE(BERR_OS_ERROR);
        }
        /* false wakeup */
        if (!forever) {
            result = BKNI_P_GetTime(&now);
            if (result != BERR_SUCCESS) {
                return BERR_TRACE(BERR_OS_ERROR);
            }
            if ((int)(now - target) >= 0) {
                *nevents = 0;
                return BERR_TIMEOUT;
            }
            timeoutMsec = target - now;
        }
    }
}

/* needed to support tagged interface */
#undef BKNI_Malloc
#undef BKNI_Free

void *
BKNI_Memset(void *b, int c, size_t len)
{
    return memset(b, c, len);
}

void *
BKNI_Memcpy(void *dst, const void *src, size_t len)
{
    return memcpy(dst, src, len);
}

int 
BKNI_Memcmp(const void *b1, const void *b2, size_t len)
{
    return memcmp(b1, b2, len);
}

void *
BKNI_Memchr(const void *b, int c, size_t len)
{
    return memchr(b, c, len);
}

void *
BKNI_Memmove(void *dst, const void *src, size_t len)
{
    return memmove(dst, src, len);
}

void *
BKNI_Malloc(size_t size)
{
    return malloc(size);
}

void 
BKNI_Free(void *ptr)
{
    free(ptr);
}

/****************************************************************************
****************************************************************************/
void 
BKNI_AssertIsrContext(const char *filename, unsigned lineno)
{
    if ( !CHECK_CRITICAL() ) {
        BDBG_P_AssertFailed("Not in critical section", filename, lineno);
    }
}

/****************************************************************************
****************************************************************************/
static BERR_Code 
BKNI_P_GetTime(unsigned long *pulMsec)
{
    unsigned long  ticks;
    ticks = (unsigned long)OSTimeGet();
    *pulMsec  = (unsigned long)(ticks*10);
    return BERR_SUCCESS;
}

/****************************************************************************
    Provide an internal method for releasing a "mutex" (really a uCOS
    semaphore). Internal functions can't use the external BKNI interface
    now that we've added additional checks for validity.
    
    Since we're using a semaphore, make sure the count never gets higher 
    than 1. If it does, it means we released the mutex more times than it 
    was acquired
****************************************************************************/
static void
BKNI_P_PostSemaphore(OS_EVENT * pEvent)
{
    uint32_t mutexCount;

    /* Since we're using a semaphore, make sure the count never
       gets higher than 1. If it does, it means we released the
       mutex more times than it was acquired
    */
    OS_ENTER_CRITICAL();
    mutexCount = pEvent->OSEventCnt;
    OS_EXIT_CRITICAL();
    if (mutexCount > 0) {
        /* Mutex already posted. Don't do it again */
        mutexCount = BERR_TRACE(mutexCount);
    } else {
        OSSemPost(pEvent);
    }
}

