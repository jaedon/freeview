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
* $brcm_Revision: 12 $
* $brcm_Date: 7/23/12 11:35a $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/base/src/ucos/nexus_base_os.c $
* 
* 12   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 11   11/10/11 10:15p ksk
* SW7358-92: Added the changes required for HDI port & Update ucos task
*  priorities for AVS.
* 
* 7358_HDI_PORT/2   11/1/11 1:27p ksk
* SW7358-92:Update ucos task priorities for AVS.
* 
* 7358_HDI_PORT/1   8/17/11 5:51p bselva
* SW7358-92: Added the changes required for HDI port
* 
* 10   11/11/10 5:10p vsilyaev
* SW7405-4995: Use BKNI_Memcmp to preinitialize structure used in
*  GetDefaultSettings
* 
* 9   9/15/10 3:40p jtna
* SW7420-1004: added BERR_StoreError and NEXUS_GetLastError_Os
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

#include <stdlib.h>
#include <string.h>

#include "nexus_base.h"
#include "nexus_base_priv.h"

#include "type.h"     /* HDI4 data types -- used by uCOS */
#include "ucos.h"     /* uCOS interface */
#include "ucos_cfg.h" /* uCOS configuration */

BDBG_MODULE(nexus_base_os);

/****************************************************************************
	Global Externs. THESE WILL BE CLEANED UP!
****************************************************************************/
extern void mips_clean_dcache(unsigned addr, long bytes);

/****************************************************************************
	Defines
****************************************************************************/
/* We're porting the 256-task version of uCOS. If we move to a new version,
   it will likely require a different port, or a bit of refactoring at least.
*/
#define UCOS_MAX_TASKS          (OS_MAX_TASKS)
#define	UCOS_TASK_STACKSIZE_C   (2048 * sizeof(ULONG))
#define UCOS_OS_TICK_RATE_MS    (10)    /* How many ms in an OS tick? */
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
	72,     /* NEXUS_ModulePriority_eIdle */
	48,     /* NEXUS_ModulePriority_eLow */
	32,     /* NEXUS_ModulePriority_eDefault */
	12,     /* NEXUS_ModulePriority_eHigh */
	73,		/*NEXUS_ModulePriority_eIdleActiveStandby*/
	49,		/*NEXUS_ModulePriority_eLowActiveStandby*/
	13		/*NEXUS_ModulePriority_eHighActiveStandby*/		
};

/****************************************************************************
	Structures
****************************************************************************/
BDBG_OBJECT_ID(NEXUS_Thread);

typedef struct NEXUS_Thread {
    BDBG_OBJECT(NEXUS_Thread)
    int task; /* task id of the new thread */
    char name[16];
    void (*pThreadFunc)(void *);
    void *pContext;
    unsigned * stack;
    NEXUS_ThreadSettings settings;
    NEXUS_P_ThreadInfo info;
} NEXUS_Thread;

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
static NEXUS_Thread * aTaskObjectList[UCOS_MAX_TASKS];
static NEXUS_OsEnv NEXUS_P_OsEnv = {
    0,
    {{ NULL, NULL }}
};

typedef void (* UCOS_TaskFunction )(void *pv_param);

/****************************************************************************
****************************************************************************/
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

/****************************************************************************
****************************************************************************/
void 
NEXUS_P_Base_Os_Uninit(void)
{
    bModuleInitialized = false;
}

/****************************************************************************
    Return the current timestamp. NEXUS_Time is an abstract data type as
    far as NEXUS is concerned. 
****************************************************************************/
void 
NEXUS_Time_Get(NEXUS_Time *time)
{
    long osTicks;

    BDBG_ASSERT(time);
    
    osTicks = OSTimeGet();
    time->osTicks = osTicks;

    return;
}

/****************************************************************************
    Return time difference in ms
****************************************************************************/
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

/****************************************************************************
****************************************************************************/
void 
NEXUS_Time_Add(NEXUS_Time *time, long delta_ms)
{
    BDBG_ASSERT(time);
    
    time->osTicks += (delta_ms / UCOS_OS_TICK_RATE_MS);
    
    return;
}

/****************************************************************************
****************************************************************************/
void 
NEXUS_FlushCache(const void *pvAddr, size_t ulNumBytes)
{
    mips_clean_dcache((unsigned long)pvAddr, ulNumBytes);
}

/****************************************************************************
****************************************************************************/
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

/****************************************************************************
****************************************************************************/
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

/****************************************************************************
****************************************************************************/
int 
NEXUS_atoi(const char *str)
{
    return atoi(str);
}

/****************************************************************************
****************************************************************************/
void 
NEXUS_Thread_GetDefaultSettings(NEXUS_ThreadSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->priority = ucosPriorityMap[NEXUS_ModulePriority_eDefault];
    pSettings->stackSize = UCOS_TASK_STACKSIZE_C;
    return;
}

/****************************************************************************
    This private function is called from the new kernel thread. From here
    we call the real task function.
****************************************************************************/
static int 
NEXUS_P_ThreadEntry(void *data)
{
	unsigned taskId;
    NEXUS_ThreadHandle thread;

    BDBG_MSG(("NEXUS_P_ThreadEntry"));
    
	/* Get the current task ID (priority) and use it to find our task object */
	taskId = OSGetTaskID();
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

/****************************************************************************
****************************************************************************/
NEXUS_P_ThreadInfo *
NEXUS_P_ThreadInfo_Get(void)
{
	/* Not sure what this is supposed to do */
    return NULL;
}

/****************************************************************************
****************************************************************************/
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

/****************************************************************************
****************************************************************************/
NEXUS_ThreadHandle 
NEXUS_Thread_Create(const char *pThreadName, void (*pThreadFunc)(void *), void *pContext, const NEXUS_ThreadSettings *pSettings)
{
    BERR_Code mrc;
    NEXUS_ThreadHandle thread;
    int priority, stackSize;
	int	ucos_status;
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
    thread = BKNI_Malloc(sizeof(*thread) + stackSize);
    if (!thread) {
        mrc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    BKNI_Memset(thread, 0, sizeof(*thread) + stackSize);

	p_task_stack = (unsigned *)((char *)thread + sizeof(*thread));
    
    BDBG_MSG(("NEXUS_Thread_Create: task=%p, stack=%p", thread, p_task_stack));

#ifdef __MON_STACK_USAGE__
	BKNI_Memset((char *)p_task_stack, 0x55, stackSize);
	task_stack_tbl[priority] = ( char * )p_task_stack;
#endif

    BDBG_OBJECT_INIT(thread, NEXUS_Thread);
    thread->task = priority;
    thread->stack = p_task_stack;
    strncpy(thread->name, pThreadName,sizeof(thread->name)-1);
    thread->name[sizeof(thread->name)-1]='\0';
    thread->pThreadFunc = pThreadFunc;
    thread->pContext = pContext;
    thread->settings.priority = priority;
    thread->settings.stackSize = stackSize;
    NEXUS_P_ThreadInfo_Init(&thread->info);
    thread->info.pThreadName = thread->name;

	aTaskObjectList[priority] = thread;

	/* NDS wants us to call OSAddTask, not OSTaskCreate. OSAddTask doesn't
	   let us pass in a user-defined parameter, so we'll need to store the
	   parameter and pass it in from our shim
	*/
	ucos_status = OSAddTask((UCOS_TaskFunction)&NEXUS_P_ThreadEntry,
			                priority,
							(ULONG *)p_task_stack,
							stackSize);

	if (OS_PRIO_EXIST == ucos_status) {
        aTaskObjectList[priority] = NULL;
		BKNI_Free(thread);
        BKNI_Printf("Error creating uC/OS task [%d]\n", priority);
        return NULL;
	}

    return thread;
}

/****************************************************************************
    Wait for the task to exit. If the task function has already returned, it 
    returns the saved return code. This functions releases all resources 
    associated with the NEXUS_Thread, and that object can't be used any more. 

    We do not force the task to exit -- this is the user's responsibility. 
    However NEXUS_Thread_Destroy might interrupt any blocked kernel interface 
    functions.
****************************************************************************/
void 
NEXUS_Thread_Destroy(NEXUS_ThreadHandle thread)
{
    BDBG_MSG(("NEXUS_Thread_Destroy"));

    BDBG_OBJECT_ASSERT(thread, NEXUS_Thread);

    if (thread->task) {
        if (thread->task) {
            int ucos_status;
            BDBG_WRN(("NEXUS_Thread_Destroy: %#lx killing (%s:%u)", (unsigned long)thread, thread->name, thread->task));

            ucos_status = OSTaskDel(thread->task);

            if (OS_NO_ERR != ucos_status) {
                /* Now what? */
                BDBG_ERR(("Failed to delete task %d", thread->task));
            }
        }
    }

    BDBG_OBJECT_DESTROY(thread, NEXUS_Thread);
    BKNI_Free(thread);
    
    return;
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

