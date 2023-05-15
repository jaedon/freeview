/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: b_os_thread.c $
* $brcm_Revision: 10 $
* $brcm_Date: 12/2/11 3:38p $
*
* Description:
*   API name: Event
*    Library routines for event management
*
* Revision History:
*
* $brcm_Log: /nexus/lib/os/src/linuxuser/b_os_thread.c $
* 
* 10   12/2/11 3:38p jgarrett
* SW7425-1876: Naming threads
* 
* 9   2/18/11 5:25p zhangjq
* SW7420-1419:Clean up Android Media Player code
* 
* 8   7/6/09 1:14p jgarrett
* PR 55572: Adding option for thread termination
* 
* 7   5/6/08 5:10p jgarrett
* PR 42024: Adding thread name to leak message for debugging
* 
* 6   4/10/08 9:19a erickson
* PR41590: fix warning
*
* 5   4/3/08 4:50p jgarrett
* PR 40306: Fixing find logic
*
* 4   4/1/08 1:47p jgarrett
* PR 40306: Fixing warning
*
* 3   4/1/08 11:40a jgarrett
* PR 40306: Asserting init has been called
*
* 2   3/10/08 8:54p jgarrett
* PR 40306: fixing default stack size
*
* 1   3/10/08 1:57p jgarrett
* PR 40306: Adding oslib
*
***************************************************************************/
#include "b_os_lib.h"
#include "bkni.h"
#include "bkni_multi.h"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "blst_queue.h"
#include <sys/prctl.h>

#define DEFAULT_STACK_SIZE (64*1024)

BDBG_MODULE(b_os_thread);

static BLST_Q_HEAD(threadList, B_Thread) g_threadList = BLST_Q_INITIALIZER(B_Thread);
static pthread_mutex_t g_threadListLock = PTHREAD_MUTEX_INITIALIZER;

static void *B_Thread_P_Launcher(void *);
static pthread_key_t g_threadKey;
static bool g_initialized;

static void B_Thread_P_Destroy(B_ThreadHandle thread);
static B_ThreadHandle B_Thread_P_FindById(pthread_t threadId);

/***************************************************************************
Summary:
Thread Handle
***************************************************************************/
typedef struct B_Thread
{
    BLST_Q_ENTRY(B_Thread) node;
    pthread_t threadId;
    char *pName;
    B_ThreadFunc threadFunction;
    void *pThreadParam;
    bool terminate;
} B_Thread;

/***************************************************************************
Summary:
Delay a thread for the specified time

Description:
This will busy-wait a thread for the specified time without yielding.
***************************************************************************/
void B_Thread_Delay(
    int microseconds
    )
{
    BKNI_Delay(microseconds);
}

/***************************************************************************
Summary:
Suspend a thread for the specified time

Description:
This will yield the CPU for the specified time.  May round up to the nearest
OS scheduler tick value.
***************************************************************************/
void B_Thread_Sleep(
    int milliseconds
    )
{
    BKNI_Sleep(milliseconds);
}

/***************************************************************************
Summary:
Get Default Thread Settings
***************************************************************************/
void B_Thread_GetDefaultSettings(
    B_ThreadSettings *pSettings     /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->priority = 100;
    pSettings->terminate = true;
}

/***************************************************************************
Summary:
Create a thread using the specified settings
***************************************************************************/
B_ThreadHandle B_Thread_Create(
    const char *pName,                  /* Thread Name - Optional */
    B_ThreadFunc threadFunction,        /* Thread Main Routine */
    void *pThreadParam,                 /* Parameter provided to threadFunction */
    const B_ThreadSettings *pSettings   /* Pass NULL for defaults */
    )
{
    int rc;
    B_Error errCode;
    B_Thread *pThread;
    B_ThreadSettings defaultSettings;
    static const char *pDefaultName = "B_OS Unknown";
    pthread_attr_t threadAttr;

    BDBG_ASSERT(NULL != threadFunction);

    if ( NULL == pName )
    {
        pName = pDefaultName;
    }

    if ( NULL == pSettings )
    {
        B_Thread_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    if ( !g_initialized )
    {
        BDBG_ERR(("Error, B_Os_Init() has not been called"));
        BDBG_ASSERT(g_initialized);
        return NULL;
    }

    pThread = B_Os_Malloc(sizeof(B_Thread));
    if ( NULL == pThread )
    {
        errCode = BERR_TRACE(B_ERROR_OUT_OF_MEMORY);
        return NULL;
    }

    pThread->threadFunction = threadFunction;
    pThread->pThreadParam = pThreadParam;
    pThread->pName = strdup(pName);
    pThread->terminate = pSettings->terminate;
    if ( NULL == pThread->pName )
    {
        B_Os_Free(pThread);
        errCode = BERR_TRACE(B_ERROR_OUT_OF_MEMORY);
        return NULL;
    }

    pthread_attr_init(&threadAttr);
    rc = pthread_attr_setstacksize(&threadAttr,
                                   (pSettings->stackSize==0)?DEFAULT_STACK_SIZE:pSettings->stackSize);
    if ( rc )
    {
        free(pThread->pName);
        B_Os_Free(pThread);
        errCode = BERR_TRACE(B_ERROR_OS_ERROR);
        return NULL;
    }

    /* Serialize with the thread list */
    (void)pthread_mutex_lock(&g_threadListLock);
    rc = pthread_create(&pThread->threadId, &threadAttr, B_Thread_P_Launcher, pThread);
    if ( rc )
    {
        pthread_mutex_unlock(&g_threadListLock);
        free(pThread->pName);
        B_Os_Free(pThread);
        errCode = BERR_TRACE(B_ERROR_OS_ERROR);
        return NULL;
    }

    /* Add to thread list, main loop will begin after we release the lock */
    BLST_Q_INSERT_TAIL(&g_threadList, pThread, node);
    pthread_mutex_unlock(&g_threadListLock);

    return pThread;
}

/***************************************************************************
Summary:
Destroy a thread.

Description:
This will signal the thread to exit and will wait for the thread to
terminate before returning.
***************************************************************************/
void B_Thread_Destroy(
    B_ThreadHandle thread
    )
{
    BDBG_ASSERT(NULL != thread);

    /* Serialize with thread list */
    (void)pthread_mutex_lock(&g_threadListLock);
    {
        B_Thread_P_Destroy(thread);
    }
    pthread_mutex_unlock(&g_threadListLock);
}

/***************************************************************************
Summary:
Get a handle to the currently executing thread

Description:
This will return NULL if the thread was not created with B_Thread_Create
***************************************************************************/
B_ThreadHandle B_Thread_GetSelf(void)
{
    return (B_ThreadHandle)pthread_getspecific(g_threadKey);
}

/***************************************************************************
Summary:
Get a handle to the currently executing thread

Description:
This will return a thread ID for any thread, not just those created by
B_Thread_Create.  This is useful for bprofile.
***************************************************************************/
B_Error B_Thread_GetId(
    B_ThreadHandle thread,  /* Pass NULL for the current thread */
    int *pId
    )
{
    pthread_t threadId;

    BDBG_ASSERT(NULL != pId);

    if ( NULL == thread )
    {
        threadId = pthread_self();
    }
    else
    {
        threadId = thread->threadId;
    }

    *pId = (int)threadId;

    return B_ERROR_SUCCESS;
}

/***************************************************************************
Summary:
Get the name of a specified thread for debugging.

Description:
This will return the name of a thread specified, and will return
an error for those threads not created with B_ThreadCreate.
***************************************************************************/
B_Error B_Thread_GetName(
    B_ThreadHandle thread,  /* Pass NULL for current thread */
    char *pName,            /* Pointer to array for name */
    int maxNameLen          /* Size of name array */
    )
{
    BDBG_ASSERT(NULL != pName);
    BDBG_ASSERT(maxNameLen > 0);

    if ( NULL == thread )
    {
        thread = B_Thread_GetSelf();
    }
    if ( NULL == thread )
    {
        pName[0] = '\0';
        return BERR_TRACE(B_ERROR_INVALID_PARAMETER);
    }

    strncpy(pName, thread->pName, maxNameLen);

    return B_ERROR_SUCCESS;
}

/***************************************************************************
Summary:
Get the name of a specified thread for debugging.

Description:
This will return the name of a thread specified, and will return
an error for those threads not created with B_ThreadCreate.  This is useful
for bprofile.
***************************************************************************/
B_Error B_Thread_GetNameFromId(
    int id,         /* ID returned by B_Thread_GetId */
    char *pName,    /* Pointer to array for name */
    int maxNameLen  /* Size of name array */
    )
{
    B_ThreadHandle thread;

    BDBG_ASSERT(NULL != pName);
    BDBG_ASSERT(maxNameLen > 0);

    thread = B_Thread_P_FindById(id);

    return B_Thread_GetName(thread, pName, maxNameLen);
}

static void *B_Thread_P_Launcher(void *pParam)
{
    B_Thread *pThread = pParam;

    /* Set thread name */    
    prctl(PR_SET_NAME, pThread->pName, 0, 0, 0);

    /* Register with thread local storage */
    pthread_setspecific(g_threadKey, pThread);

    /* Serialize with the thread list */
    (void)pthread_mutex_lock(&g_threadListLock);
    pthread_mutex_unlock(&g_threadListLock);

#ifndef ANDROID
    if ( pThread->terminate )
    {
        /* Permit asynchronous cancel requests */
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    }
#endif


    /* Enter main loop */
    pThread->threadFunction(pThread->pThreadParam);
    return NULL;
}

B_Error B_Thread_P_Init(void)
{
    int rc;

    rc = pthread_key_create(&g_threadKey, NULL);
    if ( rc )
    {
        return BERR_TRACE(B_ERROR_OS_ERROR);
    }

    g_initialized = true;

    return B_ERROR_SUCCESS;
}

void B_Thread_P_Uninit(void)
{
    BDBG_ASSERT(g_initialized);

    (void)pthread_mutex_lock(&g_threadListLock);
    {
        B_Thread *pThread, *pNextThread = NULL;

        for ( pThread = BLST_Q_FIRST(&g_threadList);
              NULL != pThread;
              pThread = pNextThread )
        {
            pNextThread = BLST_Q_NEXT(pThread, node);
            BDBG_WRN(("Thread %p (%s) not destroyed.  Cleaning up.", pThread, pThread->pName?pThread->pName:"unnamed"));
            B_Thread_P_Destroy(pThread);
        }
    }
    pthread_mutex_unlock(&g_threadListLock);

    pthread_key_delete(g_threadKey);
    g_initialized = false;
}

/* The thread list lock should be acquired prior to calling this routine */
static void B_Thread_P_Destroy(
    B_ThreadHandle thread
    )
{
    BDBG_ASSERT(NULL != thread);

#ifndef ANDROID
    if ( thread->terminate )
    {
        (void)pthread_cancel(thread->threadId);
    }
#endif    
    (void)pthread_join(thread->threadId, NULL);
    BLST_Q_REMOVE(&g_threadList, thread, node);
    free(thread->pName);
    B_Os_Free(thread);
}

/* The thread list lock should NOT be acquired prior to calling this routine */
static B_ThreadHandle B_Thread_P_FindById(pthread_t threadId)
{
    B_Thread *pThread = NULL;

    (void)pthread_mutex_lock(&g_threadListLock);
    {
        for ( pThread = BLST_Q_FIRST(&g_threadList);
              NULL != pThread;
              pThread = BLST_Q_NEXT(pThread, node) )
        {
            if ( pThread->threadId == threadId )
                break;
        }
    }
    pthread_mutex_unlock(&g_threadListLock);

    return pThread;
}

