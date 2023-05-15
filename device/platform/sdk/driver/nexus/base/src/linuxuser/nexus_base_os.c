/***************************************************************************
*     (c)2008-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_base_os.c $
* $brcm_Revision: 40 $
* $brcm_Date: 9/24/12 9:06a $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/base/src/linuxuser/nexus_base_os.c $
* 
* 40   9/24/12 9:06a erickson
* SWANDROID-221: add #include for android
* 
* 39   9/19/12 10:36a erickson
* SW7445-45: revise 7445 support
* 
* 38   9/18/12 5:28p erickson
* SW7445-45: add 7445 support
* 
* 37   8/13/12 11:54a vsilyaev
* SW7420-2328: Properly initialize threadId for all threads
* 
* 36   8/9/12 12:48p vsilyaev
* SW7420-2328: Better handle thread 'marking' for already know threads
* 
* 35   8/6/12 12:21p vsilyaev
* SW7420-2328: Properly use pthread TLS to mimic non-pthread TLS
*  implementation
* 
* 34   8/3/12 2:51p vsilyaev
* SW7420-2328: Use pthread functions to implement TLS (thread local
*  storage)
* 
* 33   7/31/12 1:35p vsilyaev
* SW7420-2328: Fixed spelling of the function name
* 
* 32   7/26/12 4:47p vsilyaev
* SW7420-1148: Verify that NEXUS_Time_Add is not used to 'substract' time
* 
* 31   7/24/12 6:42p vsilyaev
* SW7435-267: Added OS specific function to get time difference in
*  microseconds
* 
* 30   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 29   6/22/12 6:46a jgarrett
* SW7425-3281: Adding standard strlen/strncpy functions
* 
* 28   5/15/12 2:04p jtna
* SW7231-577: remove coverity comments from NEXUS_GetEnv. the right fix
*  is elsewhere
* 
* 27   2/10/12 11:30a vsilyaev
* SW7425-2063: Add additional header file for Android compiles.
* 
* SW7425-2063/2   2/8/12 3:12p ttrammel
* SW7425-2063: Revise Android fix to just include a header file.
* 
* SW7425-2063/1   12/28/11 12:25p alexpan
* SW7425-2063: Fix Android ICS build error
* 
* 26   1/17/12 12:14p jtna
* SW7231-577: Coverity Defect ID:36199 TAINTED_STRING
* 
* 25   1/13/12 10:45a jtna
* SW7231-577: Coverity Defect ID:36199 TAINTED_STRING
* 
* 24   1/3/12 10:57a jtna
* SW7231-577: Coverity Defect ID:36199 TAINTED_STRING
* 
* 23   11/15/11 9:04a erickson
* SW7425-1764: honor PTHREAD_STACK_MIN in NEXUS_Thread_GetDefaultSettings
*  for linuxuser
* 
* 22   9/13/11 6:06p jhaberf
* SWDTV-6551: increased minimum stack size for 35233 DTV chip
* 
* 21   8/3/11 5:33p vsilyaev
* SW7420-1465, SW7405-5221: Route debug output to the special FIFO
*  instead of syslog buffer
* 
* 20   7/22/11 7:13p vsilyaev
* SW7420-2008: Validate result from pthread_getspecific and don't free
*  thread memory of pthread_join have failed
* 
* 19   5/13/11 4:12p erickson
* SWDTV-6386: call prctl to set thread name
* 
* 18   5/10/11 5:07p erickson
* SW7420-1123: improve NEXUS_Thread_Create error handling
* 
* 17   4/19/11 12:23p vsilyaev
* SW7405-5221: Route nexus BERR_TRACE through common route
* 
* 16   3/30/11 11:59a erickson
* SW7420-1706: modify NEXUS_FlushCache for android
* 
* 15   12/3/10 11:28a jtna
* SW7420-1004: delete threadKeyError on Uninit
* 
* 14   11/11/10 5:11p vsilyaev
* SW7405-4995: Use BKNI_Memcmp to preinitialize structure used in
*  GetDefaultSettings
* 
* 13   9/23/10 2:12p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 12   9/15/10 3:40p jtna
* SW7420-1004: added BERR_StoreError and NEXUS_GetLastError_Os
*
* 11   10/22/09 4:28p erickson
* SW7405-3245: add comments, increase stack_top guess
*
* 10   8/10/09 11:55a erickson
* PR57475: support NEXUS_SetEnv(name, NULL)
*
* 9   10/13/08 11:31a erickson
* PR47572: if cacheflush fails, print addr and size - the likely cause
*
* 8   10/3/08 11:18a vsilyaev
* PR 45361: Fixed BTRC and BPROFILE support with -std=c89
*
* 7   8/6/08 2:25p erickson
* PR45469: move NEXUS_Base_GetDefaultSettings to OS specific code
*
* 6   7/9/08 12:11p erickson
* PR44643: switch to clock_gettime so we can tolerate settimeofday
*
* 5   5/6/08 4:56p jgarrett
* PR 42024: Fixing bogus warning on shutdown
*
* 4   4/14/08 5:39p vsilyaev
* PR 41083: Fixed thread stack lookup
*
* 3   4/8/08 10:40a vsilyaev
* PR 41083: Added bprofile support
*
* 2   1/30/08 11:49a vsilyaev
* PR 38682: Added routing of callbacks into the user space
*
* 1   1/18/08 2:14p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/3   1/11/08 3:49p vsilyaev
* PR 38682: Fixed typos and removed unused code.
*
* Nexus_Devel/2   9/24/07 10:33a vsilyaev
* PR 34419: Added tagged versions of Nexus_Module_Lock and NEXUS_atoi
* function
*
* Nexus_Devel/4   9/6/07 1:04p vsilyaev
* PR 34419: Added core module
*
* Nexus_Devel/3   9/5/07 2:47p vsilyaev
* PR 34419: Added environment
*
* Nexus_Devel/2   8/30/07 7:07p vsilyaev
* PR 34419: More complete implementation
*
* Nexus_Devel/1   8/28/07 6:39p vsilyaev
* PR 34419: Initial implementation
*
***************************************************************************/
#include "nexus_base.h"
#include "nexus_base_priv.h"
#include <string.h>
#ifdef B_REFSW_ANDROID
#include <asm/page.h>
#endif
#include <pthread.h>
#include <stdlib.h>
#include <sys/prctl.h>

BDBG_MODULE(nexus_base_os);

#define NEXUS_P_USE_PTHREAD_TLS   1

void
NEXUS_Time_Get(NEXUS_Time *time)
{
    int rc;

    /* We no longer use gettimeofday. clock_gettime(CLOCK_MONOTONIC) is resilient to calendar time changes, which
    applications may need to perform. */
    rc = clock_gettime(CLOCK_MONOTONIC, time);
    if (rc!=0) {
        BDBG_ERR(("clock_gettime returned %d, ignored", rc));
    }
    return;
}

long
NEXUS_Time_Diff(const NEXUS_Time *future, const NEXUS_Time *past)
{
    return 1000*(future->tv_sec - past->tv_sec) + (future->tv_nsec - past->tv_nsec)/1000000;
}

long NEXUS_Time_DiffMicroseconds(const NEXUS_Time *future, const NEXUS_Time *past)
{
    return 1000000*(future->tv_sec - past->tv_sec) + (future->tv_nsec - past->tv_nsec)/1000;
}

void
NEXUS_Time_Add(NEXUS_Time *time, long delta_ms)
{
    if(delta_ms>0) {
        time->tv_nsec += (delta_ms%1000) * 1000000;
        time->tv_sec += delta_ms/1000;
        if (time->tv_nsec > 1000000000) {
            time->tv_nsec -= 1000000000;
            time->tv_sec++;
        }
    }
    return;
}

void
NEXUS_Thread_GetDefaultSettings(NEXUS_ThreadSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->priority = 1;
    pSettings->stackSize = 64*1024;
#ifdef PTHREAD_STACK_MIN
    if (pSettings->stackSize < PTHREAD_STACK_MIN) {
        pSettings->stackSize = PTHREAD_STACK_MIN;
    }
#endif
}

BDBG_OBJECT_ID(NEXUS_Thread);

struct NEXUS_Thread {
    NEXUS_P_ThreadInfo info; /* must be the first member */
    char name[16];
    BDBG_OBJECT(NEXUS_Thread)
    pthread_t thread;
    void (*pThreadFunc)(void *);
    void *pContext;
    NEXUS_ThreadSettings settings;
    const uint8_t *stack_top;
    size_t stack_len;
    BLST_S_ENTRY(NEXUS_Thread) link;
};

static struct {
#if NEXUS_P_USE_PTHREAD_TLS
    pthread_key_t threadKey;
#endif
    BLST_S_HEAD(NEXUS_ThreadHead, NEXUS_Thread) threads;
} NEXUS_P_Os_State;



static void *
NEXUS_P_ThreadEntry(void *t)
{
    uint8_t buf[1];
    NEXUS_ThreadHandle thread = t;
    void *threadId = (void *)pthread_self();
    
    thread->stack_top = buf;
    BDBG_MSG(("thread '%s' stack %#lx", thread->name, (unsigned long)thread->stack_top));
    /* Push the name into the Linux process so it will be visible from procps tools */
    prctl(PR_SET_NAME, thread->name, 0, 0, 0);
    
#if NEXUS_P_USE_PTHREAD_TLS
    thread->info.threadId = threadId;
    pthread_setspecific(NEXUS_P_Os_State.threadKey, &thread->info);
#else
    NEXUS_Base_P_Thread_AssociateInfo(thread, threadId, &thread->info); 
#endif
    thread->pThreadFunc(thread->pContext);
    return NULL;
}

NEXUS_ThreadHandle
NEXUS_Thread_Create(const char *pThreadName, void (*pThreadFunc)(void *), void *pContext, const NEXUS_ThreadSettings *pSettings)
{
    NEXUS_ThreadHandle  thread;
    BERR_Code mrc;
    int rc;
    pthread_attr_t attr;
    NEXUS_ThreadSettings defaultSettings;

    if (!pThreadName || !pThreadFunc) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }

    if(!pSettings) {
        NEXUS_Thread_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    thread = BKNI_Malloc(sizeof(*thread));
    if(!thread) {
        mrc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(thread, NEXUS_Thread);
    strncpy(thread->name, pThreadName,sizeof(thread->name)-1);
    thread->name[sizeof(thread->name)-1]='\0';
    thread->pThreadFunc = pThreadFunc;
    thread->pContext = pContext;
    thread->settings = *pSettings;
    thread->stack_top = NULL;
    thread->stack_len = pSettings->stackSize;
    NEXUS_P_ThreadInfo_Init(&thread->info);
    thread->info.nexusThread = thread;
    thread->info.pThreadName = thread->name;

    rc = pthread_attr_init(&attr);
    if (rc!=0) {
        rc = BERR_TRACE(BERR_OS_ERROR);
        goto err_attr;
    }
    rc = pthread_attr_setstacksize(&attr, pSettings->stackSize);
    if (rc!=0) {
        mrc = BERR_TRACE(BERR_OS_ERROR);
        goto err_stack;
    }
    if (pthread_create(&thread->thread, &attr, NEXUS_P_ThreadEntry, thread)) {
        mrc = BERR_TRACE(BERR_OS_ERROR);
        goto err_thread;
    }
    pthread_attr_destroy(&attr);
    
    BLST_S_INSERT_HEAD(&NEXUS_P_Os_State.threads, thread, link);
    return thread;

err_thread:
err_stack:
    pthread_attr_destroy(&attr);
err_attr:
    BKNI_Free(thread);
err_alloc:
    return NULL;
}

void
NEXUS_Thread_Destroy(NEXUS_ThreadHandle thread)
{
    int rc;
    BDBG_OBJECT_ASSERT(thread, NEXUS_Thread);
    rc = pthread_join(thread->thread, NULL);
    if(rc!=0) {
        BDBG_ERR(("pthread_join: failed %d for thread %s(%#lx)", rc, thread->info.pThreadName, (unsigned long)thread));
    }
    NEXUS_LockModule();
    BLST_S_REMOVE(&NEXUS_P_Os_State.threads, thread, NEXUS_Thread, link);
    NEXUS_UnlockModule();
    if(rc==0) {
#if !NEXUS_P_USE_PTHREAD_TLS
        NEXUS_Base_P_Thread_DisassociateInfo(thread, &thread->info); 
#endif
        BDBG_OBJECT_DESTROY(thread, NEXUS_Thread);
        BKNI_Free(thread);
    }
    return;
}

static void
NEXUS_P_Base_Os_MarkThread_locked(const char *name)
{
    NEXUS_ThreadHandle thread;
    NEXUS_P_ThreadInfo *threadInfo;
    uint8_t buf[1];
    void *threadId = (void *)pthread_self();

    BDBG_ASSERT(name);
#if NEXUS_P_USE_PTHREAD_TLS
    threadInfo = pthread_getspecific(NEXUS_P_Os_State.threadKey);
#else
    threadInfo = NEXUS_Base_P_Thread_GetInfo(threadId);
#endif
    if(threadInfo!=NULL) {
        thread = threadInfo->nexusThread;
        if(thread) {
            BDBG_WRN(("NEXUS_P_Base_MarkThread: duplicate make '%s'<>'%s'", name, thread->name));
            return;
        }
    }
    thread = BKNI_Malloc(sizeof(*thread));
    if(!thread) {
        NEXUS_Error rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        BSTD_UNUSED(rc);
        return;
    }
    /* initialiaze main pseudo thread */
    BDBG_OBJECT_INIT(thread, NEXUS_Thread);
    strncpy(thread->name, name, sizeof(thread->name)-1);
    NEXUS_P_ThreadInfo_Init(&thread->info);
    thread->info.pThreadName = thread->name;
    thread->info.nexusThread = thread;
    thread->info.threadId = threadId;
    thread->pThreadFunc = NULL;
    thread->pContext = NULL;
    NEXUS_Thread_GetDefaultSettings(&thread->settings);
    thread->stack_top = buf + 1024*8; /* This function is being called from a stack which already has some
        unknown amount of data on it. We need to guess top of the stack. */
    thread->stack_len = 64*1024;
    BDBG_MSG(("external thread '%s' stack %#lx", thread->name, (unsigned long)thread->stack_top));
    BLST_S_INSERT_HEAD(&NEXUS_P_Os_State.threads, thread, link);
#if NEXUS_P_USE_PTHREAD_TLS
    pthread_setspecific(NEXUS_P_Os_State.threadKey, &thread->info);
#else
    NEXUS_Base_P_Thread_AssociateInfo(thread, threadId, &thread->info); /* associates thread info with the thread */
#endif
    return;
}

/**
NEXUS_P_Base_Os_MarkThread allows nexus to store bookkeeping information
about external threads. This function does not need to be called for threads
nexus creates using NEXUS_Thread_Create.
threads.
**/
void
NEXUS_P_Base_Os_MarkThread(const char *name)
{
    /* NEXUS_LockModule can't be uses since NEXUS_P_Base_Os_MarkThread_locked would create key, and Unlock would see then Unlock without Lock */
    BDBG_OBJECT_ASSERT(NEXUS_MODULE_SELF, NEXUS_Module);
    BKNI_AcquireMutex(NEXUS_MODULE_SELF->lock);
    NEXUS_P_Base_Os_MarkThread_locked(name);
    BKNI_ReleaseMutex(NEXUS_MODULE_SELF->lock);
    return;
}

BERR_Code
NEXUS_P_Base_Os_Init(void)
{
    BLST_S_INIT(&NEXUS_P_Os_State.threads);
#if NEXUS_P_USE_PTHREAD_TLS
    {
        int rc = pthread_key_create(&NEXUS_P_Os_State.threadKey, NULL);
        if(rc!=0) {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }
#endif
    return BERR_SUCCESS;
}

void
NEXUS_P_Base_Os_Uninit(void)
{
    NEXUS_ThreadHandle thread;

    while(NULL!=(thread=BLST_S_FIRST(&NEXUS_P_Os_State.threads))) {
        BLST_S_REMOVE_HEAD(&NEXUS_P_Os_State.threads, link); /* don't free or destroy thread */
        if(thread->pThreadFunc==NULL) { /* dummy placeholder */
#if !NEXUS_P_USE_PTHREAD_TLS
            NEXUS_Base_P_Thread_DisassociateInfo(thread, &thread->info); 
#endif
            BDBG_OBJECT_DESTROY(thread, NEXUS_Thread);
            BKNI_Free(thread); /* could delete it */
        }
        else
        {
            BDBG_WRN(("NEXUS_P_Base_Os_Uninit: %#lx leaked thread '%s'", (unsigned long)thread, thread->name));
        }
    }
#if NEXUS_P_USE_PTHREAD_TLS
    pthread_key_delete(NEXUS_P_Os_State.threadKey);
#endif
    return;
}

const char *
NEXUS_P_Base_Os_GetTaskNameFromStack(const unsigned long *stack)
{
    NEXUS_ThreadHandle  thread;
    /* don't acquire critical section here, no one shall create tasks while running profile report */
    for(thread = BLST_S_FIRST(&NEXUS_P_Os_State.threads); thread ; thread = BLST_S_NEXT(thread, link)) {
        if ((uint32_t)stack < (uint32_t)thread->stack_top && (uint32_t)stack >= (uint32_t)thread->stack_top - thread->stack_len) {
            /* bingo */
            BDBG_MSG(("'%s' thread for stack %#lx (%#lx...%#lx)", thread->name, (unsigned long)stack, (unsigned long)thread->stack_top - thread->stack_len, (unsigned long)thread->stack_top));
            return thread->name;
        }
    }
    BDBG_WRN(("unknown thread for stack %#lx", (unsigned long)stack));
    return NULL;
}


NEXUS_P_ThreadInfo *
NEXUS_P_ThreadInfo_Get(void)
{
    NEXUS_P_ThreadInfo *threadInfo;
#if NEXUS_P_USE_PTHREAD_TLS
    threadInfo = pthread_getspecific(NEXUS_P_Os_State.threadKey);
    if(threadInfo==NULL) {
        void *threadId = (void *)pthread_self();
        threadInfo = NEXUS_Base_P_AllocateThreadInfo(threadId);  
        pthread_setspecific(NEXUS_P_Os_State.threadKey, threadInfo);
        NEXUS_Base_P_TickThreadInfo(threadInfo); 
    }
#else /* NEXUS_P_USE_PTHREAD_TLS */
    threadInfo = NEXUS_Base_P_Thread_GetInfo((void *)pthread_self());
#endif /* NEXUS_P_USE_PTHREAD_TLS */
    return threadInfo;
}

const char *
NEXUS_GetEnv(const char *name)
{
    return getenv(name);
}

void
NEXUS_SetEnv(const char *name, const char *value)
{
    if (value) {
        setenv(name, value, 1);
    }
    else {
        unsetenv(name);
    }
    return;
}

#if BCHP_CHIP == 7445
#warning must implement cacheflush
#define cacheflush(pvAddr, ulNumBytes, DCACHE)  (0)
#elif defined B_REFSW_ANDROID
#include <sys/linux-syscalls.h>
#include <sys/cachectl.h>
#define cacheflush(pvAddr, ulNumBytes, DCACHE) syscall(__NR_cacheflush, (long int) pvAddr, ulNumBytes, DCACHE)
#else
#include <sys/cachectl.h>
#endif

void
NEXUS_FlushCache(const void *pvAddr, size_t ulNumBytes)
{
    int rc;

    rc = cacheflush((void *)pvAddr, ulNumBytes, DCACHE);
    if (rc<0) {
        BDBG_ERR(("cacheflush has returned error %d, addr %p, size %u, ignored", rc, pvAddr, ulNumBytes));
    }
    return;
}

void
NEXUS_Base_GetDefaultSettings(NEXUS_Base_Settings *pSettings)
{
    unsigned i;
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    for(i=0;i<sizeof(pSettings->threadSettings)/sizeof(pSettings->threadSettings[0]);i++) {
        NEXUS_Thread_GetDefaultSettings(&pSettings->threadSettings[i]);
    }
    return;
}

int
NEXUS_atoi(const char *str)
{
    return atoi(str);
}

int
NEXUS_P_Base_StrCmp(const char *str1, const char *str2)
{
    return strcmp(str1, str2);
}

int b_strlen(const char *s)
{
    return strlen(s);
}

char *b_strncpy(char *dest, const char *src, int n)
{
    return strncpy(dest,src,n);
}

