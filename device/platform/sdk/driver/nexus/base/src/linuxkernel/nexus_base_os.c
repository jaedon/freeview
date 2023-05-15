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
* $brcm_Revision: 24 $
* $brcm_Date: 9/24/12 11:15a $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/base/src/linuxkernel/nexus_base_os.c $
* 
* 24   9/24/12 11:15a erickson
* SW7435-369: NEXUS_SetEnv must copy name and value
* 
* 23   8/3/12 1:38p vsilyaev
* SW7420-2328: Fixed spelling of the function name
* 
* 22   7/31/12 1:35p vsilyaev
* SW7420-2328: Fixed spelling of the function name
* 
* 21   7/31/12 12:08p erickson
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 20   7/26/12 4:48p vsilyaev
* SW7435-280: Use Linux jiffies for the timestamp
* 
* 19   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 18   4/6/12 10:19a erickson
* SW7420-1148: provide nexus_base_os.h stubs
* 
* 17   8/3/11 5:33p vsilyaev
* SW7420-1465, SW7405-5221: Route debug output to the special FIFO
*  instead of syslog buffer
* 
* 16   5/3/11 5:08p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
* 15   4/20/11 9:10a erickson
* SW7425-365: add linux 2.6.37 support
* 
* 14   2/7/11 11:16a vsilyaev
* SW7420-1441: Short-cicruit cache flush for fake mapped addresses
* 
* 13   11/11/10 5:11p vsilyaev
* SW7405-4995: Use BKNI_Memcmp to preinitialize structure used in
*  GetDefaultSettings
* 
* 12   9/23/10 2:12p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 11   9/16/10 1:12p jtna
* SW7420-1004: move BERR_StoreError and NEXUS_GetLastError to outside of
*  LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
*
* 10   9/15/10 3:39p jtna
* SW7420-1004: added BERR_StoreError and NEXUS_GetLastError_Os
*
* 9   10/12/09 1:44p erickson
* SW7420-228: fix NEXUS_Thread implementation for linux 2.6.31
*
* 8   9/11/09 12:25p erickson
* SW7420-228: linux 2.6.30 support
*
* 7   8/10/09 11:55a erickson
* PR57475: support NEXUS_SetEnv(name, NULL)
*
* 6   1/20/09 3:46p vsilyaev
* PR 49285, PR 50582: Improved debugability of scheduller and fixed
*  problem in the scheduller, where it goes to WaitForGroup to extended
*  amount of time
*
* 5   8/6/08 2:25p erickson
* PR45469: move NEXUS_Base_GetDefaultSettings to OS specific code
*
* 4   7/14/08 12:23p erickson
* PR44765: protect NEXUS_FlushCache from zero length
*
* 3   7/9/08 2:39p erickson
* PR44643: switch from gettimeofday to jiffies, allows for settimeofday
*  changes to calendar time
*
* 2   1/31/08 11:49a vsilyaev
* PR 38682: Added support for runtime options
*
* 1   1/18/08 2:14p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/3   1/16/08 3:36p vsilyaev
* PR 38682: Removed commented code
*
* Nexus_Devel/2   1/15/08 11:12a vsilyaev
* PR 38682: Improved linuxkernel build
*
***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "nexus_base_os.h"
#include "nexus_base_os_types.h"
#include "nexus_base_os_priv.h"
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/pci.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

BDBG_MODULE(nexus_base_os);

void
NEXUS_Time_Get(NEXUS_Time *time)
{
    *time = jiffies;
    return;
}

long
NEXUS_Time_Diff(const NEXUS_Time *future, const NEXUS_Time *past)
{
    long jiffies_diff = (long)(*future) - (long)(*past); /* this would take care  by its own of wrap in the jiffies */
    long msec_diff; /* howwever jiffies_to_msecs takes unsigned and return unsigned, to if difference negative, convert it */
    if(jiffies_diff>=0) {
        msec_diff = jiffies_to_msecs(jiffies_diff);
    } else {
        msec_diff = -jiffies_to_msecs(-jiffies_diff);
    }
    return msec_diff;
}

void
NEXUS_Time_Add(NEXUS_Time *time, long delta_ms)
{
    if(delta_ms > 0) {
        long delta_jiffies = msecs_to_jiffies(delta_ms);
        *time += delta_jiffies; 
    }
    return;
}

void
NEXUS_Thread_GetDefaultSettings(NEXUS_ThreadSettings *pSettings)
{
    memset(pSettings, 0, sizeof(*pSettings));
    pSettings->priority = 1;
    pSettings->stackSize = 8*1024;
}

BDBG_OBJECT_ID(NEXUS_Thread);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)

/* use kthread() */
#include <linux/kthread.h>

struct NEXUS_Thread {
    BDBG_OBJECT(NEXUS_Thread)
    char name[16];
    void (*pThreadFunc)(void *);
    void *pContext;
    NEXUS_ThreadSettings settings;
    NEXUS_P_ThreadInfo info;
    struct task_struct *task;
};

static int NEXUS_P_ThreadStart(void *data)
{
    NEXUS_ThreadHandle thread = data;
    
    NEXUS_Base_P_Thread_AssociateInfo(thread, current, &thread->info); 
    thread->pThreadFunc(thread->pContext);

    /* do not return until stopped */
    while (1) {
        set_current_state(TASK_INTERRUPTIBLE); /* go half asleep before checking condition */
        if (kthread_should_stop()) break;
        schedule();
    }

    return 0;
}

NEXUS_ThreadHandle
NEXUS_Thread_Create(const char *pThreadName, void (*pThreadFunc)(void *), void *pContext, const NEXUS_ThreadSettings *pSettings)
{
    NEXUS_ThreadHandle  thread;
    BERR_Code mrc;
    NEXUS_ThreadSettings defaultSettings;

    BDBG_ASSERT(pThreadName);
    BDBG_ASSERT(pThreadFunc);

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
    NEXUS_P_ThreadInfo_Init(&thread->info);
    thread->info.pThreadName = thread->name;

    /* in linux kernel stack size is fixed to 2 4K pages */
#define LINUX_KERNEL_STACK_SIZE (8*1024)
    if (thread->settings.stackSize < LINUX_KERNEL_STACK_SIZE) {
        BDBG_WRN(("NEXUS_Thread_Create: %s stack size %u forced to %u",  thread->name, thread->settings.stackSize, LINUX_KERNEL_STACK_SIZE));
    }
    thread->settings.stackSize = LINUX_KERNEL_STACK_SIZE;

    thread->task = kthread_run(NEXUS_P_ThreadStart, thread, thread->name);
    if (!thread->task) {
        BKNI_Free(thread);
        mrc = BERR_TRACE(BERR_UNKNOWN);
        return NULL;
    }

    return thread;

err_alloc:
    return NULL;
}

void
NEXUS_Thread_Destroy(NEXUS_ThreadHandle thread)
{
    BDBG_OBJECT_ASSERT(thread, NEXUS_Thread);
    kthread_stop(thread->task);
    NEXUS_Base_P_Thread_DisassociateInfo(thread, &thread->info); 
    BDBG_OBJECT_DESTROY(thread, NEXUS_Thread);
    BKNI_Free(thread);
    return;
}

#else /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30) */

/* use kernel_thread() */

struct NEXUS_Thread {
    struct task_struct *task;
    BDBG_OBJECT(NEXUS_Thread)
    char name[16];
    void (*pThreadFunc)(void *);
    void *pContext;
    struct work_struct tq;
    NEXUS_ThreadSettings settings;
    NEXUS_P_ThreadInfo info;
};

/* This private function is called from the new kernel thread */
static int
NEXUS_P_ThreadEntry(void *data)
{
    NEXUS_ThreadHandle task = data;

    BDBG_ASSERT(task);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    daemonize(task->name);
    allow_signal(SIGTERM);
    allow_signal(SIGINT);
    allow_signal(SIGKILL);
#else
    daemonize();
    strncpy(current->comm, task->name, sizeof(current->comm));
    current->comm[sizeof(current->comm)-1] = '\0';
    /* Must lock this before mucking with signals */
    spin_lock_irq(&current->sigmask_lock);
    siginitsetinv(&current->blocked, sigmask(SIGKILL)|sigmask(SIGINT)|sigmask(SIGTERM));
    spin_unlock_irq(&current->sigmask_lock);
#endif

    task->task = current;
    /* ignore the return value */
    (task->pThreadFunc)(task->pContext);
    task->task = NULL;
    return 0;
}

static void __attribute__((no_instrument_function))
NEXUS_P_ThreadStart(void *data)
{
    NEXUS_ThreadHandle task = data;
    NEXUS_Base_P_Thread_AssociateInfo(task, current, &task->info); 
    kernel_thread((int (*)(void *))NEXUS_P_ThreadEntry, (void *)task, 0);
}

NEXUS_ThreadHandle
NEXUS_Thread_Create(const char *pThreadName, void (*pThreadFunc)(void *), void *pContext, const NEXUS_ThreadSettings *pSettings)
{
    NEXUS_ThreadHandle  thread;
    BERR_Code mrc;
    NEXUS_ThreadSettings defaultSettings;

    BDBG_ASSERT(pThreadName);
    BDBG_ASSERT(pThreadFunc);

    if (!pSettings) {
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
    NEXUS_P_ThreadInfo_Init(&thread->info);
    thread->info.pThreadName = thread->name;

    /* in linux kernel stack size is fixed to 2 4K pages */
#define LINUX_KERNEL_STACK_SIZE (8*1024)
    if (thread->settings.stackSize < LINUX_KERNEL_STACK_SIZE) {
        BDBG_WRN(("NEXUS_Thread_Create: %s stack size %u forced to %u",  thread->name, thread->settings.stackSize, LINUX_KERNEL_STACK_SIZE));
    }
    thread->settings.stackSize = LINUX_KERNEL_STACK_SIZE;

    /* start launcher with task queue */
    INIT_WORK(&thread->tq, NEXUS_P_ThreadStart, thread);
    schedule_work(&thread->tq);

    return thread;

err_alloc:
    return NULL;
}

void
NEXUS_Thread_Destroy(NEXUS_ThreadHandle thread)
{
    BDBG_OBJECT_ASSERT(thread, NEXUS_Thread);
    if (thread->task) {
        BKNI_Sleep(10);
        if (thread->task) {
            BDBG_WRN(("NEXUS_Thread_Destroy: %#lx killing (%s:%u)", (unsigned long)thread, thread->name, thread->task->pid));
            kill_proc(thread->task->pid, SIGTERM, 1);
            /* Wait for task to die. */
            BKNI_Sleep(500);
        }
    }
    
    NEXUS_Base_P_Thread_DisassociateInfo(thread, &thread->info); 

    /* now we are sure the thread is in zombie state. We
    notify keventd to clean the process up. */
    kill_proc(2, SIGCHLD, 1);
    BDBG_OBJECT_DESTROY(thread, NEXUS_Thread);
    BKNI_Free(thread);
    return;
}
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30) */

void
NEXUS_FlushCache(const void *pvAddr, size_t ulNumBytes)
{
    if (ulNumBytes) {
        if((unsigned long)pvAddr >= 0x80000000) {
            dma_cache_wback_inv((unsigned long)pvAddr, ulNumBytes);
        } else {
            BDBG_MSG(("flushing fake address %#lx", (unsigned long)pvAddr));
        }
    }
    return;
}


NEXUS_P_ThreadInfo *
NEXUS_P_ThreadInfo_Get(void)
{
    return NEXUS_Base_P_Thread_GetInfo(current);
}

static struct {
    unsigned count; /* provides short-circuit to full array search */
    struct {
#define NEXUS_KEY_SIZE 32
#define NEXUS_VALUE_SIZE 64
        char key[NEXUS_KEY_SIZE];
        char value[NEXUS_VALUE_SIZE];
    } env[64];
} NEXUS_P_OsEnv = {
    0
   /* *** */
};

const char *
NEXUS_GetEnv(const char *name)
{
    unsigned i;
    for(i=0;i<NEXUS_P_OsEnv.count;i++) {
        if (NEXUS_P_OsEnv.env[i].key[0] && NEXUS_P_Base_StrCmp(NEXUS_P_OsEnv.env[i].key, name)==0) {
            return NEXUS_P_OsEnv.env[i].value;
        }
    }
    return NULL;
}

void
NEXUS_SetEnv(const char *name, const char *value)
{
    unsigned i;
    unsigned freeslot = NEXUS_P_OsEnv.count;

    for(i=0;i<NEXUS_P_OsEnv.count;i++) {
        if (!NEXUS_P_OsEnv.env[i].key[0] && freeslot == NEXUS_P_OsEnv.count) {
            freeslot = i;
        }

        if (NEXUS_P_OsEnv.env[i].key[0] && NEXUS_P_Base_StrCmp(NEXUS_P_OsEnv.env[i].key, name)==0) {
            if (!value) {
                /* if we're unsetting, free the slot but don't reduce the count */
                NEXUS_P_OsEnv.env[i].key[0] = 0;
                NEXUS_P_OsEnv.env[i].value[0] = 0;
            }
            else {
                b_strncpy(NEXUS_P_OsEnv.env[i].value, value, NEXUS_VALUE_SIZE);
            }
            return;
        }
    }
    if (!value) return;

    /* save new key.value pair */
    if (freeslot<sizeof(NEXUS_P_OsEnv.env)/sizeof(*NEXUS_P_OsEnv.env)) {
        b_strncpy(NEXUS_P_OsEnv.env[freeslot].key, name, NEXUS_KEY_SIZE);
        b_strncpy(NEXUS_P_OsEnv.env[freeslot].value, value, NEXUS_VALUE_SIZE);
        if (freeslot == NEXUS_P_OsEnv.count) {
            NEXUS_P_OsEnv.count++;
        }
    }
    else {
        BDBG_WRN(("Unable to store NEXUS_SetEnv(%s,%s)", name, value));
    }
    return;
}

BERR_Code
NEXUS_P_Base_Os_Init(void)
{
    return BERR_SUCCESS;
}

void
NEXUS_P_Base_Os_Uninit(void)
{
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
    return (int)simple_strtol(str, NULL, 0);
}

int
NEXUS_P_Base_StrCmp(const char *str1, const char *str2)
{
    int ch1, ch2, diff;

    for(;;) {
        ch1 = *str1++;
        ch2 = *str2++;
        diff = ch1 - ch2;
        if (diff) {
            return diff;
        } else if (ch1=='\0') {
            return 0;
        }
    }
}

int b_strlen(const char *s) 
{
    unsigned i = 0;
    while (*s++) i++;
    return i;
}

char *b_strncpy(char *dest, const char *src, int n)
{
    char *org = dest;
    while (n--) {
        *dest++ = *src;
        if (!*src++) break;
    }
    return org;
}

void NEXUS_P_Base_Os_MarkThread(const char *name)
{
    BSTD_UNUSED(name);
}

const char *NEXUS_P_Base_Os_GetTaskNameFromStack(const unsigned long *stack)
{
    BSTD_UNUSED(stack);
    return NULL;
}
