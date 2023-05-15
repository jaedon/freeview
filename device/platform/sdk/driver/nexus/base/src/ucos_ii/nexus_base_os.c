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
* $brcm_Revision: 14 $
* $brcm_Date: 9/30/12 10:38a $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/base/src/ucos_ii/nexus_base_os.c $
* 
* 14   9/30/12 10:38a agin
* SWNOOS-575:  Include functions needed only by 97408.  97408 is based on
*  an older reference code.
* 
* 13   9/19/12 1:04p agin
* SWNOOS-573:  Leveled to linuxuser\nexus_base_os.c (main\39).
* 
* 12   9/13/12 5:31p agin
* SWNOOS-570:  97408 no-os cache work-around.
* 
* 11   9/1/12 10:52a agin
* SWNOOS-567:  Clean up code.
* 
* 10   8/28/12 12:32p agin
* SWNOOS-559:  Fixed NEXUS_FlushCache, changed task prorities.
* 
* 9   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 8   8/29/11 12:19p agin
* SWNOOS-481:  Add additional task priorities for UCOS Diags only.
* 
* 7   7/11/11 3:34p agin
* SWNOOS-466:  Don't need to zero out pSettings->driverModuleInit and
*  pSettings->driverModuleUninit.
* 
* 6   7/10/11 4:13p agin
* SWNOOS-466:  Initialize driverModuleInit and driverModuleUninit.
* 
* 5   5/21/11 11:40a agin
* SWNOOS-461:  Add NEXUS_P_Base_StrCmp.
* 
* 4   3/31/11 7:08a agin
* SWNOOS-450:  Support GHS for 97125 again.
* 
* 3   11/11/10 5:10p vsilyaev
* SW7405-4995: Use BKNI_Memcmp to preinitialize structure used in
*  GetDefaultSettings
* 
* 2   9/15/10 3:41p jtna
* SW7420-1004: added BERR_StoreError and NEXUS_GetLastError_Os
* 
* 1   7/20/10 6:26p jfisher
* SW7572-30:  Add ucos_ii support.
* 
* 1   7/20/10 2:08p jfisher
* SW7572-30:  Add ucos_ii support.
* 
* 8   7/13/09 1:22p cnovak
* PR56705: Adjust the ucos task priorities which we assign to the Nexus
*  module priorities.
* 
* 7   3/4/09 10:46a agadre
* PR51817:Update ucos task priorities
* 
* 6   8/6/08 4:02p cnovak
* PR45469: Default thread priorities setup for uCOS.
* 
* 5   8/5/08 1:50p cnovak
* PR43697: Setup default uCOS task priorities for each Nexus scheduler
*  thread.
* 
* 4   7/31/08 12:17p cnovak
* PR43697: Minor tweaking.
* 
* 3   7/22/08 11:59a cnovak
* PR43697: Enable call to MIPS cache flush. Fix bug in environment
*  get/set routines.
* 
* 2   6/19/08 6:13p cnovak
* PR43697: Fix Time_Add implementation.
* 
* 1   6/17/08 2:36p cnovak
* PR43697: Add uCOS to Nexus.
* 
***************************************************************************/
#include "nexus_base.h"
#include "nexus_base_priv.h"
#include "ucos_ii.h"
//#include "cache_lib.h"

BDBG_MODULE(nexus_base_os);


/****************************************************************************
    Defines
****************************************************************************/
/* We're porting the 256-task version of uCOS. If we move to a new version,
   it will likely require a different port, or a bit of refactoring at least.
*/
#define UCOS_MAX_TASKS          (256)
#define UCOS_TASK_STACKSIZE_C   (2048 * sizeof(unsigned int))
#define UCOS_OS_TICK_RATE_MS    (1000/OS_TICKS_PER_SEC)    /* How many ms in an OS tick? <<< JPF -the value is really ms per tick */
#define MAX_ENV_ENTRIES         (64)

/****************************************************************************
    Map the NEXUS scheduler priorities into a default set of uCOS tasks
    Per NDS, the 256-task version of uCOS has the following assignments:
    
    NDS-UK           0-7, 56-71, 88-95, 128-175, 192-215, 224-231, 248-255
    NDS-TI           24-31, 40-47, 96-111, 216-223, 240-247
    DRIVERS (HDI+CA) 8-23, 32-39, 48-55, 72-87, 112-127, 176-191, 232-239
       
    We use the highest driver task (8) for the ISR task. We also need to 
    pick one for main. Since NEXUS uses many fewer tasks then the old HDI4
    driver implementation, we don't need to worry about running out - I hope.
    
    These task priorities are relative to Nexus, not the system. DO NOT put 
    the idle task at the lowest uCOS task or else modules like frontend, which
    uses idle, will not function correctly.
****************************************************************************/
const NEXUS_ModulePriority ucosPriorityMap[NEXUS_ModulePriority_eMax] = {
#ifdef UCOS_DIAGS
    203,    /* NEXUS_ModulePriority_eIdle */
    202,    /* NEXUS_ModulePriority_eLow */
    201,    /* NEXUS_ModulePriority_eDefault */
    200,    /* NEXUS_ModulePriority_eHigh */
    206,    /* NEXUS_ModulePriority_eIdleActiveStandby */
    205,    /* NEXUS_ModulePriority_eLowActiveStandby */
    204     /* NEXUS_ModulePriority_eHighActiveStandby */
#else
    72,     /* NEXUS_ModulePriority_eIdle */
    48,     /* NEXUS_ModulePriority_eLow */
    32,     /* NEXUS_ModulePriority_eDefault */
    12,     /* NEXUS_ModulePriority_eHigh */
#endif
};

/****************************************************************************
    Structures
****************************************************************************/

typedef unsigned int b_task_t;

typedef struct NEXUS_OsEnv {
    unsigned count;
    struct {
        const char *key;
        const char *value;
    } env[MAX_ENV_ENTRIES];
} NEXUS_OsEnv;

/****************************************************************************
    Static variables
****************************************************************************/
/* This module is a singleton. We can only have 1 */
static bool bModuleInitialized = false;
//static NEXUS_Thread * aTaskObjectList[UCOS_MAX_TASKS];
static NEXUS_OsEnv NEXUS_P_OsEnv = {
    0,
    {{ NULL, NULL }}
};

typedef void (* UCOS_TaskFunction )(void *pv_param);

void 
NEXUS_Time_Get(NEXUS_Time *time)
{
    long osTicks;

    BDBG_ASSERT(time);
    
    osTicks = OSTimeGet();
    time->osTicks = osTicks;

    return;
}

long 
NEXUS_Time_Diff(const NEXUS_Time *future, const NEXUS_Time *past)
{
    long tickDiff;
    NEXUS_Time tempTime;
    
    BDBG_ASSERT(future);
    BDBG_ASSERT(past);

    /* Check for wrap condition */
    if (past->osTicks > future->osTicks) {
        /* Get max tick value */
        tempTime.osTicks = -1;
        tickDiff = (tempTime.osTicks - past->osTicks) + future->osTicks;
    } else {
        tickDiff = future->osTicks - past->osTicks;
    }
    
    return (tickDiff * UCOS_OS_TICK_RATE_MS);
}

void 
NEXUS_Time_Add(NEXUS_Time *time, long delta_ms)
{
    BDBG_ASSERT(time);
    
    time->osTicks += (delta_ms / UCOS_OS_TICK_RATE_MS);
    
    return;
}

void 
NEXUS_Thread_GetDefaultSettings(NEXUS_ThreadSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->priority = ucosPriorityMap[NEXUS_ModulePriority_eDefault];
    pSettings->stackSize = UCOS_TASK_STACKSIZE_C;
    return;
}

#if (BCHP_CHIP==7408) /* This is needed until 7408 uses nexus_base.c /main/47 or later */
BERR_Code BERR_StoreError_tagged(const char *file, unsigned lineno, const char *error, BERR_Code rc)
{
    /* not yet implemented */
    if (rc) {
        BKNI_Printf("!!!Error %s(%#x) at %s:%d\n", error, rc, file, lineno); /* mimic original BERR_TRACE behavior */
    }
    return rc;
}

void NEXUS_GetLastError_Os(BERR_Code *code)
{
    /* not yet implemented */
    *code = 0;
    return;
}
#endif

BDBG_OBJECT_ID(NEXUS_Thread);

typedef struct NEXUS_Thread {
    BDBG_OBJECT(NEXUS_Thread)
    int task; /* task id of the new thread */
    char name[16];
    void (*pThreadFunc)(void *);
    void *pContext;
    b_task_t handle;
    NEXUS_ThreadSettings settings;
    NEXUS_P_ThreadInfo info;
    unsigned * stack;
} NEXUS_Thread;

static NEXUS_Thread * aTaskObjectList[UCOS_MAX_TASKS];

static int 
NEXUS_P_ThreadEntry(void *data)
{
    unsigned taskId;
    NEXUS_ThreadHandle thread = (NEXUS_ThreadHandle)data;

    BDBG_MSG(("NEXUS_P_ThreadEntry"));
    
    /* Get the current task ID (priority) and use it to find our task object */
    taskId = thread->task;
    if (taskId >= UCOS_MAX_TASKS) {
        BKNI_Printf("Invalid task [%d]!!\n", taskId);
        BKNI_Fail();
        return -1;
    }

    thread = aTaskObjectList[taskId];

    if (!thread) {
        BKNI_Printf("Invalid task called [%d]!!\n", taskId);
        BKNI_Fail();
        return -1;
    }

    BDBG_MSG(("NEXUS_P_ThreadEntry: task handle %p", thread));

    if (((unsigned long)thread->pThreadFunc & 0xe0000000) != 0x80000000) {
        BKNI_Printf("trying to execute task at 0x%.8lx\n", (unsigned long)thread->pThreadFunc);
        BKNI_Fail();
    }

    /* Call the original task */
    (*thread->pThreadFunc)(thread->pContext);

    BKNI_Printf("Task terminated!!\n");

    return 0;
}

NEXUS_ThreadHandle 
NEXUS_Thread_Create(const char *pThreadName, void (*pThreadFunc)(void *), void *pContext, const NEXUS_ThreadSettings *pSettings)
{
    BERR_Code mrc;
    NEXUS_ThreadHandle hThread;
    int priority, stackSize;
    unsigned *p_task_stack;

    BDBG_ASSERT(pThreadName);
    BDBG_ASSERT(pThreadFunc);

    BDBG_MSG(("NEXUS_Thread_Create"));

    /* Use locals so we can change them witout changing the settings they provided */
    priority = -1;
    stackSize = -1;
    
    /* If we get a NULL pointer, use our default settings */
    if (pSettings) {
        priority = pSettings->priority;
        stackSize = pSettings->stackSize;
    }

    /* If they used -1 then use these defaults */
    if (priority < 0) priority = ucosPriorityMap[NEXUS_ModulePriority_eDefault];
    if (stackSize < 0) stackSize = UCOS_TASK_STACKSIZE_C;

    BDBG_MSG(("Thread Pri: %d Stack: %d", priority, stackSize));

    if (priority >= UCOS_MAX_TASKS) {
        BKNI_Printf("BKNI_AddTask: priority 0x%.8lx invalid\n", priority) ;
        return NULL;
    }

    /* We allocate the task stack for uCOS. After we've successfully allocated
       the memory, keep a pointer to the "stack"
    */
    hThread = BKNI_Malloc(sizeof(*hThread) + stackSize);
    if (!hThread) {
        mrc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    BKNI_Memset(hThread, 0, sizeof(*hThread) + stackSize);

    p_task_stack = (unsigned *)((char *)hThread + sizeof(*hThread));
    
    BDBG_MSG(("NEXUS_Thread_Create: task=%p, stack=%p", hThread, p_task_stack));

#ifdef __MON_STACK_USAGE__
    BKNI_Memset((char *)p_task_stack, 0x55, stackSize);
    task_stack_tbl[priority] = ( char * )p_task_stack;
#endif

    BDBG_OBJECT_INIT(hThread, NEXUS_Thread);
    hThread->task = priority;
    hThread->stack = p_task_stack;
    strcpy(hThread->name, (const char *) pThreadName);
    hThread->pThreadFunc = pThreadFunc;
    hThread->pContext = pContext;
    hThread->settings.priority = priority;
    hThread->settings.stackSize = stackSize;
    NEXUS_P_ThreadInfo_Init(&(hThread->info));
    hThread->info.pThreadName = hThread->name;

    aTaskObjectList[priority] = hThread;

    hThread->task = priority;
    hThread->handle = priority;
	if (OSTaskCreateExt(NEXUS_P_ThreadEntry,hThread,(OS_STK*)&p_task_stack[stackSize/sizeof(unsigned int) - 1],
						priority,priority,(OS_STK*)p_task_stack,stackSize/sizeof(unsigned int),
						NULL,OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR) != OS_ERR_NONE)
    {
        aTaskObjectList[priority] = NULL;
        BKNI_Free(hThread);
        BKNI_Printf("Error creating uC/OS task [%d]\n", priority);
        return NULL;
    }

    return hThread;
}

void 
NEXUS_Thread_Destroy(NEXUS_ThreadHandle thread)
{
    BDBG_MSG(("NEXUS_Thread_Destroy"));

    BDBG_OBJECT_ASSERT(thread, NEXUS_Thread);

    if (thread->task) {
        if (thread->task) {
            BDBG_WRN(("NEXUS_Thread_Destroy: %#lx killing (%s:%u)", (unsigned long)thread, thread->name, thread->task));

#if OS_TASK_DEL_EN > 0
        	OSTaskDel(thread->handle);
#endif
        }
    }

    BDBG_OBJECT_DESTROY(thread, NEXUS_Thread);
    BKNI_Free(thread);
    
    return;
}

static void
NEXUS_P_Base_Os_MarkThread_locked(const char *name)
{
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
    BERR_Code rc = BERR_SUCCESS;
    
    if (bModuleInitialized == true) {
        rc = BERR_TRACE(BERR_LEAKED_RESOURCE);
        return rc;
    }

    /* Initialize our task list */
    BKNI_Memset(aTaskObjectList, 0x00, sizeof(aTaskObjectList));

    bModuleInitialized = true;

    return rc;
}

void 
NEXUS_P_Base_Os_Uninit(void)
{
    bModuleInitialized = false;
}

const char *
NEXUS_P_Base_Os_GetTaskNameFromStack(const unsigned long *stack)
{
    return NULL;
}


NEXUS_P_ThreadInfo *
NEXUS_P_ThreadInfo_Get(void)
{
    /* Not sure what this is supposed to do */
    return NULL;
}

const char *
NEXUS_GetEnv(const char *name)
{
    unsigned i;
    
    for (i=0;i<NEXUS_P_OsEnv.count;i++) {
        if (! NEXUS_P_Base_StrCmp(NEXUS_P_OsEnv.env[i].key, name)) {
            BDBG_MSG(("GetEnv: %s=%s", NEXUS_P_OsEnv.env[i].key, NEXUS_P_OsEnv.env[i].value));
            return NEXUS_P_OsEnv.env[i].value;
        }
    }
    BDBG_MSG(("GetEnv: %s", name));
    
    return NULL;
}

void 
NEXUS_SetEnv(const char *name, const char *value)
{
    unsigned i;

    BDBG_MSG(("SetEnv: %s=%s", name, value));

    /* if already there, replace old with new */
    for (i=0;i<NEXUS_P_OsEnv.count;i++) {
        if (! NEXUS_P_Base_StrCmp(NEXUS_P_OsEnv.env[i].key, name)) {
            NEXUS_P_OsEnv.env[i].value = value;
            return;
        }
    }
    /* not already there -- add it (save new key.value pair) */
    if (i<sizeof(NEXUS_P_OsEnv.env)/sizeof(*NEXUS_P_OsEnv.env)) {
        NEXUS_P_OsEnv.env[i].key = name;
        NEXUS_P_OsEnv.env[i].value = value;
        NEXUS_P_OsEnv.count = i+1;
    }
    return;
}

void 
NEXUS_FlushCache(const void *pvAddr, size_t ulNumBytes)
{
    #ifdef CACHE_WORKAROUND
        flush_dcache((unsigned long)pvAddr, (unsigned long)((unsigned char*)pvAddr + ulNumBytes));
    #else
        clear_d_cache((uint32_t)pvAddr, (uint32_t)ulNumBytes);
    #endif    
}

void
NEXUS_Base_GetDefaultSettings(NEXUS_Base_Settings *pSettings)
{
    unsigned i;
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    for(i=0;i<sizeof(pSettings->threadSettings)/sizeof(pSettings->threadSettings[0]);i++) {
        NEXUS_Thread_GetDefaultSettings(&pSettings->threadSettings[i]);
        /* Map the scheduler's priority to an real uCOS priority */
        pSettings->threadSettings[i].priority = ucosPriorityMap[i];
    }
    return;
}

int 
NEXUS_atoi(const char *str)
{
    int i=0;

    while ((*str >= '0') && (*str <='9'))
        i = i*10 + *(str++) - '0';

    return i;
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

#ifdef GHS
int
strcasecmp(const char *s1, const char *s2)
{
    int i;
    for (i=0;s1[i] && s2[i];i++) {
        char c1 = tolower((int)s1[i]);
        char c2 = tolower((int)s2[i]);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
    }
    /* if one string is longer than the other then they don't compare */
    if (s1[i]) return 1; /*s1 longer than s2*/
    if (s2[i]) return -1;/*s2 longer than s1*/
    return 0;
}
#endif


