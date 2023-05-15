/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Revision: 4 $
* $brcm_Date: 2/2/12 11:21a $
*
* Description:
*   API name: Event
*    Library routines for event management
*
* Revision History:
*
* $brcm_Log: /nexus/lib/os/src/ucos_ii/b_os_thread.c $
* 
* 4   2/2/12 11:21a agin
* SWNOOS-515:  Support SDE6.
* 
* 3   9/18/11 4:44p agin
* SWNOOS-485:  Relevel to main\9 linuxuser.
* 
* 2   5/18/11 4:41p agin
* SWNOOS-458:  Fix compiler warnings.
* 
* 1   2/17/11 6:44a agin
* SWNOOS-441:  Support UCOS Diags.
* 
***************************************************************************/
#include "b_os_lib.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <string.h>
#include <stdlib.h>
#include "blst_queue.h"

/*#include <taskLib.h>*/

#define DEFAULT_STACK_SIZE (20*1024)

BDBG_MODULE(b_os_thread);

static BLST_Q_HEAD(threadList, B_Thread) g_threadList = BLST_Q_INITIALIZER(B_Thread);

static uint32_t g_threadKey;
static bool g_initialized;
/*static BLST_Q_HEAD(threadList, B_Thread) g_threadList = BLST_Q_INITIALIZER(B_Thread);*/
static B_MutexHandle g_threadListLock;

/*static void *B_Thread_P_Launcher(void *);*/

static void B_Thread_P_Destroy(B_ThreadHandle thread);
static B_ThreadHandle B_Thread_P_FindById(int threadId);


/***************************************************************************
Summary:
Thread Handle
***************************************************************************/
typedef struct B_Thread
{
    BLST_Q_ENTRY(B_Thread) node;
    int threadId;
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
	#if 0
    int rc;
    B_Error errCode;
    B_Thread *pThread;
    B_ThreadSettings defaultSettings;
    static const char *pDefaultName = "Unknown";
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

    /* This lock will also prevent the thread from starting (even if it has higher priority) until we're done adding it to the list. */
    B_Mutex_Lock(g_threadListLock);

    /* These were supposed to be set before calling this, but ... */
    if (!(priority = pSettings->priority))   priority = 100;
    if (!(stackSize = pSettings->stackSize)) stackSize = DEFAULT_STACK_SIZE;

    /* ROY -- enforce this until we get things running! */
    if (stackSize <= 8192) stackSize = DEFAULT_STACK_SIZE;

    pThread->threadId = taskSpawn((char*)pName, priority, 0 /*options?*/, stackSize, (FUNCPTR)B_Thread_P_Launcher, (int)pThread, 0,0,0,0,0,0,0,0,0);
    if ( !pThread->threadId )
    {
        B_Mutex_Unlock(g_threadListLock);
        free(pThread->pName);
        B_Os_Free(pThread);
        errCode = BERR_TRACE(B_ERROR_OS_ERROR);
        return NULL;
    }

    /* Add to thread list, main loop will begin after we release the lock */
    BLST_Q_INSERT_TAIL(&g_threadList, pThread, node);
    B_Mutex_Unlock(g_threadListLock);
	#else
    B_Thread *pThread;
	#endif

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
    B_Mutex_Lock(g_threadListLock);
    B_Thread_P_Destroy(thread);
    B_Mutex_Unlock(g_threadListLock);
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
#if 0
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
#endif
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

    /* Serialize with the thread list */
    B_Mutex_Lock(g_threadListLock);
    B_Mutex_Unlock(g_threadListLock);

    /* Enter main loop */
    pThread->threadFunction(pThread->pThreadParam);
    return NULL;
}

B_Error B_Thread_P_Init(void)
{
    B_MutexSettings settings;

    B_Mutex_GetDefaultSettings(&settings);
    g_threadListLock = B_Mutex_Create(&settings);
    if (!g_threadListLock) return BERR_TRACE(B_ERROR_OS_ERROR);

    g_initialized = true;

    return B_ERROR_SUCCESS;
}

void B_Thread_P_Uninit(void)
{
    BDBG_ASSERT(g_initialized);

    B_Mutex_Lock(g_threadListLock);
    {
        B_Thread *pThread, *pNextThread = NULL;
        for (pThread = BLST_Q_FIRST(&g_threadList); NULL != pThread; pThread = pNextThread)
        {
            pNextThread = BLST_Q_NEXT(pThread, node);
            B_Thread_P_Destroy(pThread);
        }
    }    
    B_Mutex_Unlock(g_threadListLock);

    B_Mutex_Destroy(g_threadListLock);
    g_initialized = false;
}

/* The thread list lock should be acquired prior to calling this routine */
static void B_Thread_P_Destroy(
    B_ThreadHandle thread
    )
{
#if 0
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
#endif
}   

/* The thread list lock should NOT be acquired prior to calling this routine */
static B_ThreadHandle B_Thread_P_FindById(int threadId)
{
    B_Thread *pThread = NULL;
    B_Mutex_Lock(g_threadListLock);
    for (pThread = BLST_Q_FIRST(&g_threadList); NULL != pThread; pThread = BLST_Q_NEXT(pThread, node))
        if ( pThread->threadId == threadId ) break;
    B_Mutex_Unlock(g_threadListLock);
    return pThread;
}

