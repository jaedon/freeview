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
* $brcm_Workfile: nexus_base.c $
* $brcm_Revision: 61 $
* $brcm_Date: 9/28/12 11:19a $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/base/src/nexus_base.c $
* 
* 61   9/28/12 11:19a erickson
* SW7435-362: stop scheduler threads before uninitializing modules. this
*  ensures no lock is held when it is destroyed.
* 
* 60   9/27/12 1:12p erickson
* SW7435-362: enforce module lock is held when creating/destroying
*  callbacks, timers and events
* 
* 59   9/4/12 1:11p vsilyaev
* SW7420-1004: Removed NEXUS_GetLastError
* 
* 58   8/20/12 12:03p erickson
* SW7425-3260: use BKNI_AcquireMutex_tagged to tunnel NEXUS_Module_Lock
*  file/line through for debug
* 
* 57   8/13/12 12:41p vsilyaev
* SW7420-2328: Clear cache when association of get replaced
* 
* 56   8/13/12 11:54a vsilyaev
* SW7420-2328: Properly initialize threadId for all threads
* 
* 55   8/10/12 5:06p vsilyaev
* SW7420-2328: For exernal allocation of  ThreadInfo try to find a match
*  in list of already allocated entries
* 
* 54   8/9/12 3:37p vsilyaev
* SW7425-3725: Silence release build warning
* 
* 53   8/9/12 3:17p erickson
* SW7425-3725: silence release build warning, but don't short-circuit
*  BDBG_OBJECT_ASSERT which gives valuable crash info
* 
* 52   8/3/12 3:50p vsilyaev
* SW7420-2330: Added control to module and callback debug features
* 
* 51   8/3/12 2:53p vsilyaev
* SW7420-2328: Use one-entry cache to speed-up retrieval of thread
*  specific data
* 
* 50   8/1/12 12:39p vsilyaev
* SW7420-2328: Verify that nexus is initialized
* 
* 49   7/31/12 1:35p vsilyaev
* SW7420-2328: Fixed spelling of the function name
* 
* 48   7/25/12 1:43p vsilyaev
* SW7435-267: Collect statistics per module (module lock) and per
*  scheduler
* 
* 47   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 46   7/20/12 3:25p vsilyaev
* SW7435-267: Added code to provide/print callback timing statistics
* 
* 45   7/9/12 1:58p erickson
* SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
* 
* 44   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 43   5/17/12 4:45p erickson
* SW7231-748: merge
* 
* SW7231-748/1   5/17/12 4:04p gmohile
* SW7231-748 : Bypass assert for disabled modules
* 
* 42   11/29/11 11:37a erickson
* SW7420-2129: store current default heap per module
* 
* 41   11/11/11 2:59p vsilyaev
* SW7125-1156, SW7420-2085: Fixed debug output
* 
* 40   11/4/11 1:54p vsilyaev
* SW7231-439: Improved debuggability
* 
* 39   11/4/11 1:01p vsilyaev
* SW7231-439, SW7420-2085: Added debug code for the reference counting
* 
* 38   10/7/11 1:49p vsilyaev
* SW7420-2085: Added support for object reference counting
* 
* 37   9/14/11 2:10p erickson
* SW7405-5478: add NEXUS_Module_GetName for debug
* 
* 36   8/5/11 5:13p erickson
* SW7420-1123: attempt to give more helpful error if module handle is
*  NULL. likely just uninitialized.
* 
* 35   8/2/11 2:58p vsilyaev
* SW7125-1014 : Add additional scheduler threads for active standby
*  modules
* 
* SW7125-1014/3   8/2/11 10:09a gmohile
* SW7125-1014 : Add additional scheduler threads for active standby
*  modules
* 
* 34   7/21/11 2:32p vsilyaev
* SW7125-1014 : Add module enable/disable
* 
* SW7125-1014/2   7/20/11 4:38p gmohile
* SW7125-1014 : Add module enable/disable
* 
* SW7125-1014/1   7/12/11 3:05p gmohile
* SW7125-1014 : Add module enable function
* 
* 33   5/31/11 3:12p erickson
* SW7420-1907: unlock module in NEXUS_Module_Create if param check fails
* 
* 32   5/13/11 4:13p erickson
* SWDTV-6386: add "nx_" namespace prefix for internal nexus threads
* 
* 31   4/5/11 12:16p vsilyaev
* SW7405-5221: Fixed linuxkernel build
* 
* 30   4/4/11 6:14p vsilyaev
* SW7405-5221: Added option to forward debug output to FIFO
* 
* 29   3/30/11 6:56p vsilyaev
* SW7335-1214: Serialize access to the field that holds a timer handle
* 
* 28   3/16/11 10:11a erickson
* SW7420-1642: add NEXUS_Base_Settings.driverModuleInit and
*  .driverModuleUninit
*
* 27   2/9/11 10:43a erickson
* SW7425-63: refactor default NEXUS_ModuleSettings to fix uninitialized
*  memory
*
* 26   1/28/11 11:48a mphillip
* SW7425-63: Fix initializer warning
*
* 25   1/19/11 11:11a erickson
* SW7420-1123: add NEXUS_Module_GetSettings
*
* 24   11/15/10 3:11p vsilyaev
* SW7405-4996: Added NEXUS_StrCmp
*
* 23   11/15/10 3:05p vsilyaev
* SWNOOS-430: Replaced NO_OS with NO_OS_DIAGS
*
* 22   9/23/10 2:12p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 21   11/24/09 12:25p erickson
* SW7405-3439: rename nexus/base external scheduler interface functions
*
* 20   6/23/09 5:27p vsilyaev
* PR 54926: Added NEXUS_Module_EnumerateAll
*
* 19   6/19/09 3:20p vsilyaev
* PR 54926: Added debug options to the module settings
*
* 18   4/19/09 7:20p agin
* PR54339: Add nexus no-os scheduler support.
*
* 17   4/8/09 12:33p erickson
* PR53862: use NEXUS_P_SchedulerGetInfo to get scheduler's callback_lock
*
* 16   2/6/09 10:22a erickson
* PR51878: fix warning
*
* 15   1/28/09 6:27p vsilyaev
* PR 51392: Moved magnum initialization from Nexus base to the platform.
*  Initialize only magnum layer inside of
*  NEXUS_Platform_GetDefaultSettings
*
* 14   1/20/09 3:46p vsilyaev
* PR 49285, PR 50582: Improved debugability of scheduller and fixed
*  problem in the scheduller, where it goes to WaitForGroup to extended
*  amount of time
*
* 13   11/7/08 11:49a erickson
* PR35457: update comments
*
* 12   11/5/08 12:15p vsilyaev
* PR 48694: Separated handling of linuxkernel and linuxkernel with
*  driver/ioctls/proxy
*
* 11   8/19/08 12:49p erickson
* PR45859: print module priority using BDBG_MSG
*
* 10   8/6/08 2:24p erickson
* PR45469: move NEXUS_Base_GetDefaultSettings to OS specific code
*
* 9   6/9/08 7:11p vsilyaev
* PR 43184: Changed phase of detection for outstanding callbacks
*
* 8   6/5/08 2:19p vsilyaev
* PR 42318: Rearranged scheduller to share the same context in user and
*  kernel mode, this allows to serve callback from kernel to user with
*  single transition kernel<->user
*
* 7   4/2/08 11:31a erickson
* PR40198: fix DEBUG=n warning
*
* 6   3/27/08 5:29p rjlewis
* PR40352: zero malloced memory.
*
* 5   3/20/08 6:15p rjlewis
* PR40352: type mismatch compile error under VxWorks.
*
* 4   2/28/08 9:39p vsilyaev
* PR 40103: Added NEXUS_TaskCallback functions
*
* 3   2/22/08 10:52a erickson
* PR39870: convert assert to runtime check for coverity
*
* 2   1/31/08 11:49a vsilyaev
* PR 38682: Added support for runtime options
*
* 1   1/18/08 2:13p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/19   1/16/08 3:37p vsilyaev
* PR 38682: Don't execute KNI/DBG initialization in the linuxkernel
*
* Nexus_Devel/18   11/14/07 5:22p vsilyaev
* PR 35824: Fixed NEXUS_Module_Assert behaviour on unknown threads
*
* Nexus_Devel/17   11/9/07 5:59p vsilyaev
* PR 34419: Added check for the recursive used of locks
*
* Nexus_Devel/16   10/23/07 4:16p vsilyaev
* PR 36199: Added memory mapper sub-module
*
* Nexus_Devel/15   10/12/07 5:42p vsilyaev
* PR 35824: Fixed NEXUS_TryLockModules
*
* Nexus_Devel/14   10/10/07 4:19p vsilyaev
* PR 35824: Added NEXUS_Time type
*
* Nexus_Devel/13   10/3/07 2:51p vsilyaev
* PR 34419: Renamed objectx to context
*
* Nexus_Devel/12   9/28/07 6:47p vsilyaev
* PR 34419: Added functions to fire callback from the interrupt context
*
* Nexus_Devel/11   9/28/07 1:03p vsilyaev
* PR 34419: Added more debug output on exit and fixed typo in the debug
* print
*
* Nexus_Devel/10   9/24/07 10:24p vsilyaev
* PR 34419: Fixed memory leak
*
* Nexus_Devel/9   9/24/07 6:15p jgarrett
* PR 35002: Fixing string funcs
*
* Nexus_Devel/8   9/24/07 10:33a vsilyaev
* PR 34419: Added tagged versions of Nexus_Module_Lock and NEXUS_atoi
* function
*
* Nexus_Devel/7   9/21/07 6:26p jgarrett
* PR 35002: Fixing Base_Init(NULL)
*
* Nexus_Devel/6   9/10/07 4:32p vsilyaev
* PR 34419: Added NEXUS_CallbackDesc
*
* Nexus_Devel/5   9/7/07 3:07p vsilyaev
* PR 34419: Create schedullers on demand
*
* Nexus_Devel/4   9/4/07 2:43p vsilyaev
* PR 34419: Use GetDefaultSettings
*
* Nexus_Devel/3   8/31/07 6:02p vsilyaev
* PR 34419: Added configuration structure for NEXUS_Base
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
#include "nexus_base_os_types.h"
#include "nexus_base_statistics.h"
#include "bkni_multi.h"

#define BDBG_MSG_TRACE(x) BDBG_MSG(x)

BDBG_MODULE(nexus_base);

#define NEXUS_BASE_P_MAX_USER_THREADS    256

#define NEXUS_P_THREAD_ID_COMPARE(node, key) (((uint8_t *)(key) > (uint8_t *)(node)->threadId) ? 1 : ( ((uint8_t *)(key) < (uint8_t *)(node)->threadId) ? -1 : 0))
BLST_AA_TREE_HEAD(NEXUS_P_ThreadInfoTree, NEXUS_P_ThreadInfo);
BLST_AA_TREE_GENERATE_REMOVE(NEXUS_P_ThreadInfoTree, NEXUS_P_ThreadInfo, node)
BLST_AA_TREE_GENERATE_INSERT(NEXUS_P_ThreadInfoTree, void *, NEXUS_P_ThreadInfo, node, NEXUS_P_THREAD_ID_COMPARE)
BLST_AA_TREE_GENERATE_FIND(NEXUS_P_ThreadInfoTree, void *, NEXUS_P_ThreadInfo, node, NEXUS_P_THREAD_ID_COMPARE)

static struct {
    NEXUS_P_Scheduler *schedulers[NEXUS_ModulePriority_eMax];
    BLST_S_HEAD(NEXUS_P_ModuleList, NEXUS_Module) modules;
    BKNI_MutexHandle callbackHandlerLock; /* lock that serializes access to the field that holds timer handle */
    NEXUS_Base_Settings settings;
    /* for user threads (ones that aren't created with the NEXUS_Thread_Create, we maintain fixed number of 'threads' specific storages, they aren't dynamically allocated since there is no 'good' way of knowing that thread was killed and thread specific storage could be recycled */
    bool coreInit;
    struct {
        BKNI_MutexHandle lock;
        struct NEXUS_P_ThreadInfoTree tree;
        unsigned currentTick; /* this is incremented on each call to get threadInfo, and it's used to keep contant of time */
        NEXUS_P_ThreadInfo info[NEXUS_BASE_P_MAX_USER_THREADS]; 
        unsigned lastUsed[NEXUS_BASE_P_MAX_USER_THREADS]; /* on each call it's get copied from the  currentTick, and it's used at the recycle time to get node that should be recycled (e.g. node that was not accessed for long */
        struct {
            void *threadId;
            NEXUS_P_ThreadInfo *threadInfo;
        } cache;
    } userThreadInfo;
} NEXUS_P_Base_State;

static const char NEXUS_P_Base_Name[] = "NEXUS_Base";


void
NEXUS_Module_GetDefaultSettings(NEXUS_ModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->priority = NEXUS_ModulePriority_eDefault;
}

BDBG_OBJECT_ID(NEXUS_Module);

NEXUS_ModuleHandle NEXUS_Base=NULL;

#if BDBG_DEBUG_BUILD
static const char *
NEXUS_P_Base_StrChr(const char *str, char c)
{
    for(;;str++) {
        char ch = *str;
        if( ch==c) {
            return str;
        } else if (ch=='\0') {
            return NULL;
        }
    }
}

static char *
NEXUS_P_Base_Strncpy(char *dst, const char *src, size_t len)
{
    char *buf;

    for(buf=dst;len>0;buf++,src++) {
        char ch = *src;
        *buf=ch;
        len--;
        if(ch=='\0') {
            break;
        }
    }
    for(;len>0;len--,buf++) {
        *buf='\0';
    }
    return dst;
}

static void
NEXUS_P_Base_SetModuleDebugLevel(const char *modulelist, BDBG_Level level)
{
    if(!modulelist) {
        return;
    }
    for(;;) {
        char buf[64];
        const char *end = NEXUS_P_Base_StrChr(modulelist, ',');
        size_t name_len;

        if (!end) {
            if(*modulelist) {
                BDBG_SetModuleLevel(modulelist, level);
            }
            break;
        }
        name_len = end-modulelist;
        if(name_len>0 && name_len<sizeof(buf)) {
            NEXUS_P_Base_Strncpy(buf, modulelist, name_len);
            buf[name_len] = '\0';
            BDBG_SetModuleLevel(buf, level);
        }
        modulelist = end+1;
    }
    return;
}
#endif /* BDBG_DEBUG_BUILD */

static const char *NEXUS_P_Scheduler_names[NEXUS_ModulePriority_eMax] = {
    "nx_sched_idle",
    "nx_sched_low",
    "nx_sched",
    "nx_sched_high",
    "nx_sched_idle_active",
    "nx_sched_low_active",
    "nx_sched_high_active"
};

void
NEXUS_CallbackDesc_Init(NEXUS_CallbackDesc *desc)
{
    BDBG_ASSERT(desc);
    desc->callback = NULL;
    desc->context = NULL;
    desc->param = 0;
    return;
}

NEXUS_ModuleHandle
NEXUS_Module_Create(const char *pModuleName, const NEXUS_ModuleSettings *pSettings)
{
    NEXUS_ModuleHandle module;
    NEXUS_ModuleHandle prev,cur;
    BERR_Code rc;
    NEXUS_ModuleSettings defaultSettings;

    BDBG_ASSERT(pModuleName);

    if(pModuleName!=NEXUS_P_Base_Name) {
        NEXUS_LockModule();
    }

    if(pSettings==NULL) {
        NEXUS_Module_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    if ((unsigned)pSettings->priority >= sizeof(NEXUS_P_Base_State.schedulers)/sizeof(NEXUS_P_Base_State.schedulers[0])) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_paramcheck;
    }

    module = BKNI_Malloc(sizeof(*module));
    if(!module) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    BKNI_Memset(module, 0, sizeof(*module));
    BDBG_OBJECT_SET(module, NEXUS_Module);
    module->settings = *pSettings;
    module->pModuleName = pModuleName;
    rc = BKNI_CreateMutex(&module->lock);
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_lock;
    }
    if(pModuleName!=NEXUS_P_Base_Name) {
        if(NEXUS_P_Base_State.schedulers[pSettings->priority]==NULL) { /* create scheduler on demand */
           NEXUS_P_Base_State.schedulers[pSettings->priority] = NEXUS_P_Scheduler_Create(pSettings->priority, NEXUS_P_Scheduler_names[pSettings->priority], &NEXUS_P_Base_State.settings.threadSettings[pSettings->priority]);
           if(NEXUS_P_Base_State.schedulers[pSettings->priority]==NULL) {
               rc = BERR_TRACE(BERR_OS_ERROR);
               goto err_scheduler;
           }
        }
        module->scheduler = NEXUS_P_Base_State.schedulers[pSettings->priority];
    } else {
        module->scheduler = NULL;
    }
    /* insert into the sorted list */
    for(prev=NULL, cur=BLST_S_FIRST(&NEXUS_P_Base_State.modules); cur!=NULL; cur=BLST_S_NEXT(cur, link)) {
        int cmp;
        cmp = NEXUS_P_Base_StrCmp(pModuleName, cur->pModuleName);
        if(cmp<0) {
            break; /* Bingo */
        } else if(cmp==0) {
            BDBG_ERR(("NEXUS_Module_Create: duplicated module name"));
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err_name;
        }
        prev = cur;
    }
    if(prev) {
        BLST_S_INSERT_AFTER(&NEXUS_P_Base_State.modules, prev, module, link);
    } else {
        BLST_S_INSERT_HEAD(&NEXUS_P_Base_State.modules, module, link);
    }

    if (NEXUS_P_Base_State.settings.driverModuleInit) {
        (NEXUS_P_Base_State.settings.driverModuleInit)(NEXUS_P_Base_State.settings.procContext, module, pModuleName, pSettings);
    }

    module->enabled = true;

    if(pModuleName!=NEXUS_P_Base_Name) {
        NEXUS_UnlockModule();
    }

    BDBG_MSG(("Creating module %s, priority %d", pModuleName, pSettings->priority));
    return module;
err_name:
err_scheduler:
    BKNI_DestroyMutex(module->lock);
err_lock:
    BKNI_Free(module);
err_alloc:
err_paramcheck:
    if(pModuleName!=NEXUS_P_Base_Name) {
        NEXUS_UnlockModule();
    }
    return NULL;
}


void
NEXUS_Module_Destroy(NEXUS_ModuleHandle module)
{
    BDBG_OBJECT_ASSERT(module, NEXUS_Module);
    NEXUS_LockModule();

    if (NEXUS_P_Base_State.settings.driverModuleUninit) {
        (NEXUS_P_Base_State.settings.driverModuleUninit)(NEXUS_P_Base_State.settings.procContext, module, module->pModuleName, &module->settings);
    }

    BDBG_MSG(("Destroying module %s", module->pModuleName));

    module->enabled = false;
    BLST_S_REMOVE(&NEXUS_P_Base_State.modules, module, NEXUS_Module, link);
    NEXUS_UnlockModule();
    BKNI_DestroyMutex(module->lock);
    BDBG_OBJECT_DESTROY(module, NEXUS_Module);
    BKNI_Free(module);
    return;
}

const char *NEXUS_Module_GetName(NEXUS_ModuleHandle module)
{
    BDBG_OBJECT_ASSERT(module, NEXUS_Module);
    return module->pModuleName;
}

void NEXUS_Module_GetSettings( NEXUS_ModuleHandle module, NEXUS_ModuleSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(module, NEXUS_Module);
    NEXUS_LockModule();
    *pSettings = module->settings;
    NEXUS_UnlockModule();
}

/* See nexus/base/src/$(OS)/nexus_base_os.c for NEXUS_Base_GetDefaultSettings */

#if NEXUS_BASE_EXTERNAL_SCHEDULER
/**
Allow an external scheduler to drive the scheduler state machine. This allows for thread consolidation
in complex systems like linux kernel mode. */
static NEXUS_Error
NEXUS_P_Base_ExternalSchedulerInit(void)
{
    unsigned i;
    for(i=0;i<sizeof(NEXUS_P_Base_State.schedulers)/sizeof(NEXUS_P_Base_State.schedulers[0]);i++) {
        NEXUS_P_Base_State.schedulers[i] = NEXUS_P_Scheduler_Init(i, NEXUS_P_Scheduler_names[i], &NEXUS_P_Base_State.settings.threadSettings[i]);
        if(!NEXUS_P_Base_State.schedulers[i]) {
            return BERR_TRACE(NEXUS_OS_ERROR);
        }
    }
    return NEXUS_SUCCESS;
}

/* Drive the scheduler from an external context. */
NEXUS_Error
NEXUS_P_Base_ExternalScheduler_Step(NEXUS_ModulePriority priority, unsigned timeout, NEXUS_P_Base_Scheduler_Status *status, bool (*complete)(void *context), void *context)
{
    NEXUS_P_Scheduler *scheduler;

    scheduler = NEXUS_P_Base_State.schedulers[priority];
    return NEXUS_P_Scheduler_Step(scheduler, timeout, status, complete, context);
}

/* All the external context to get the scheduler's mutex. */
void
NEXUS_P_Base_GetSchedulerConfig(NEXUS_ModulePriority priority, NEXUS_Base_Scheduler_Config *config)
{
    BKNI_Memset(config, 0, sizeof(*config));
    if (priority < NEXUS_ModulePriority_eMax) {
        config->name = NEXUS_P_Scheduler_names[priority];
        config->pSettings = &NEXUS_P_Base_State.settings.threadSettings[priority];

        if (NEXUS_P_Base_State.schedulers[priority]) {
            NEXUS_P_SchedulerInfo info;
            NEXUS_P_SchedulerGetInfo(NEXUS_P_Base_State.schedulers[priority], &info);
            config->callback_lock = info.callback_lock;
        }
    }
    return;
}
#endif

NEXUS_Error
NEXUS_Base_Core_Init(void)
{
    NEXUS_Error rc;

    BDBG_ASSERT(NEXUS_Base==NULL);
    rc = NEXUS_P_Base_Os_Init();
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_os;
    }

    BKNI_Memset(&NEXUS_P_Base_State.userThreadInfo, 0, sizeof(NEXUS_P_Base_State.userThreadInfo));
    BLST_AA_TREE_INIT(NEXUS_P_ThreadInfoTree, &NEXUS_P_Base_State.userThreadInfo.tree);
    rc = BKNI_CreateMutex(&NEXUS_P_Base_State.userThreadInfo.lock);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_thread_info; }

#if BDBG_DEBUG_BUILD
    NEXUS_P_Base_SetModuleDebugLevel(NEXUS_GetEnv("wrn_modules"), BDBG_eWrn);
    NEXUS_P_Base_SetModuleDebugLevel(NEXUS_GetEnv("msg_modules"), BDBG_eMsg);
    NEXUS_P_Base_SetModuleDebugLevel(NEXUS_GetEnv("trace_modules"), BDBG_eTrace);
#endif
    NEXUS_P_Base_State.coreInit = true;
 
    return NEXUS_SUCCESS;

err_thread_info:
    NEXUS_P_Base_Os_Uninit();
err_os:
    return BERR_TRACE(rc);
}

void
NEXUS_Base_Core_Uninit(void)
{
    BDBG_ASSERT(NEXUS_Base==NULL);
    BDBG_MSG_TRACE(("NEXUS_Base_Core_Uninit: NEXUS_P_Base_Os_Uninit"));
    NEXUS_P_Base_Os_Uninit();
    BKNI_DestroyMutex(NEXUS_P_Base_State.userThreadInfo.lock);
    NEXUS_P_Base_State.coreInit = false;
    return;
}


NEXUS_Error
NEXUS_Base_Init(const NEXUS_Base_Settings *pSettings)
{
    BERR_Code rc;
    unsigned i;
    NEXUS_Base_Settings settings;


    if(!pSettings) {
        NEXUS_Base_GetDefaultSettings(&settings);
        pSettings = &settings;
    }

    if(!NEXUS_P_Base_State.coreInit) {
        BDBG_ERR(("NEXUS_Base_Core_Init should be called prior to NEXUS_Base_Init"));
        NEXUS_Base_Core_Init();
    }


    BLST_S_INIT(&NEXUS_P_Base_State.modules);
    NEXUS_P_Base_State.settings = *pSettings;
    NEXUS_P_MapInit();
    NEXUS_P_Base_Stats_Init();

    BDBG_ASSERT(NEXUS_Base==NULL);
    rc = BKNI_CreateMutex(&NEXUS_P_Base_State.callbackHandlerLock);
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_calback_handler_lock;
    }

    for(i=0;i<sizeof(NEXUS_P_Base_State.schedulers)/sizeof(NEXUS_P_Base_State.schedulers[0]);i++) {
        NEXUS_P_Base_State.schedulers[i] = NULL;
    }
    NEXUS_Base = NEXUS_Module_Create(NEXUS_P_Base_Name, NULL);
    if(!NEXUS_Base) {
        rc = BERR_TRACE(BERR_OS_ERROR);
        goto err_module;
    }
    NEXUS_P_Base_Os_MarkThread("main"); 
    NEXUS_LockModule();
#if NEXUS_BASE_EXTERNAL_SCHEDULER
    rc = NEXUS_P_Base_ExternalSchedulerInit(); /* initialize schedulers, but don't start threads */
    if(rc!=BERR_SUCCESS) {
        NEXUS_UnlockModule();
        NEXUS_Module_Destroy(NEXUS_Base);
        rc = BERR_TRACE(rc);
        goto err_module;
    }
#endif
    NEXUS_UnlockModule();
    NEXUS_P_Base_Scheduler_Init();

    return BERR_SUCCESS;

err_module:
    BKNI_DestroyMutex(NEXUS_P_Base_State.callbackHandlerLock);
err_calback_handler_lock:
    return rc;
}

#define NEXUS_P_FILENAME(str) ((str)?(str):"")

void NEXUS_Base_Stop(void)
{
    unsigned i;
    BDBG_ASSERT(NEXUS_Base);
    BDBG_MSG_TRACE(("NEXUS_Base_Stop:>"));
    NEXUS_LockModule();
    BDBG_MSG_TRACE(("NEXUS_Base_Stop:Locked"));
    for(i=0;i<sizeof(NEXUS_P_Base_State.schedulers)/sizeof(NEXUS_P_Base_State.schedulers[0]);i++) {
        if(NEXUS_P_Base_State.schedulers[i]) {
            BDBG_MSG_TRACE(("NEXUS_Base_Uninit:NEXUS_P_Scheduler_Stop"));
            NEXUS_P_Scheduler_Stop(NEXUS_P_Base_State.schedulers[i]);
        }
    }
    NEXUS_UnlockModule();
}

void
NEXUS_Base_Uninit(void)
{
    unsigned i;
    BDBG_ASSERT(NEXUS_Base);
    BDBG_MSG_TRACE(("NEXUS_Base_Uninit:>"));
    NEXUS_LockModule();
    BDBG_MSG_TRACE(("NEXUS_Base_Uninit:Locked"));
    for(i=0;i<sizeof(NEXUS_P_Base_State.schedulers)/sizeof(NEXUS_P_Base_State.schedulers[0]);i++) {
        if(NEXUS_P_Base_State.schedulers[i]) {
            BDBG_MSG_TRACE(("NEXUS_Base_Uninit:NEXUS_P_Scheduler_Destroy"));
            NEXUS_P_Scheduler_Destroy(NEXUS_P_Base_State.schedulers[i]);
        }
    }
    BDBG_MSG_TRACE(("NEXUS_Base_Uninit: NEXUS_P_Base_Scheduler_Uninit"));
    NEXUS_P_Base_Scheduler_Uninit();
    BDBG_MSG_TRACE(("NEXUS_Base_Uninit: NEXUS_UnlockModule"));
    NEXUS_UnlockModule();
    BDBG_MSG_TRACE(("NEXUS_Base_Uninit: NEXUS_Module_Destroy"));
    NEXUS_Module_Destroy(NEXUS_Base);
    BDBG_MSG_TRACE(("NEXUS_Base_Uninit: BKNI_DestroyMutex"));
    BKNI_DestroyMutex(NEXUS_P_Base_State.callbackHandlerLock);
    NEXUS_P_Base_Stats_Uninit();
    NEXUS_Base = NULL;
    return;
}

bool
NEXUS_Module_Assert(NEXUS_ModuleHandle module)
{
#if NEXUS_P_DEBUG_MODULE_LOCKS
    NEXUS_P_ThreadInfo *info;
    NEXUS_P_LockEntry *entry;
    
    if (!module->enabled) {
        return true;
    } 
    info = NEXUS_P_ThreadInfo_Get();
    if(info) {
        if (BLIFO_READ_PEEK(&info->stack)>0) {
            /* check if lock is held anywhere in the stack */
            entry = BLIFO_READ(&info->stack);
            while (1) {
                if (entry->module == module) return true;
                if (entry == info->locks) break;
                entry--;
            }
        }
        return false;
    }
#endif
    return true;
    BSTD_UNUSED(module);
}

#if NEXUS_P_DEBUG_MODULE_LOCKS
static void
NEXUS_Module_P_CheckLock(const char *function, NEXUS_P_ThreadInfo *info, NEXUS_ModuleHandle module, const char *pFileName, unsigned lineNumber)
{
    BSTD_UNUSED(pFileName);
    BSTD_UNUSED(lineNumber);
    BSTD_UNUSED(function);
    if(info && BLIFO_READ_PEEK(&info->stack)>0) {
        const NEXUS_P_LockEntry *entry;
        entry = BLIFO_READ(&info->stack);
        if(entry->module == module) {
            BDBG_ERR(("%s[%s]: trying apply recursive lock:%s at %s:%u old at %s:%u", function, info->pThreadName, module->pModuleName, NEXUS_P_FILENAME(pFileName), lineNumber, NEXUS_P_FILENAME(entry->pFileName), entry->lineNumber));
            BDBG_ASSERT(0);
        }
    }
    return;
}
#else
#define NEXUS_Module_P_CheckLock(function, info, module, pFileName, lineNumber)
#endif

#if BKNI_TRACK_MALLOCS
/* use _tagged versions of acquire to tunnel file/line info through */
#else
/* if they don't exist for this OS, reverse-define them */
#define BKNI_AcquireMutex_tagged(MUTEX, FILE, LINE) BKNI_AcquireMutex(MUTEX)
#define BKNI_TryAcquireMutex_tagged(MUTEX, FILE, LINE) BKNI_TryAcquireMutex(MUTEX)
#endif

void
NEXUS_Module_Lock_Tagged(NEXUS_ModuleHandle module, const char *pFileName, unsigned lineNumber)
{
    NEXUS_P_ThreadInfo *info;
    BERR_Code rc;
    NEXUS_P_MODULE_STATS_STATE();

    NEXUS_P_MODULE_STATS_START();
    BSTD_UNUSED(pFileName);
    BSTD_UNUSED(lineNumber);
    
    if (!module) {
        BDBG_ERR(("Locking a NULL module handle. It is possible that the module was not initialized and the application is calling its API."));
        BSTD_UNUSED(module);
    }
    BDBG_OBJECT_ASSERT(module, NEXUS_Module);
#if NEXUS_P_DEBUG_MODULE_LOCKS
    info = NEXUS_P_ThreadInfo_Get();
#endif
    NEXUS_Module_P_CheckLock("NEXUS_Module_Lock_Tagged", info, module, pFileName, lineNumber);
    rc = BKNI_AcquireMutex_tagged(module->lock, pFileName, lineNumber);
    BDBG_ASSERT(rc==BERR_SUCCESS);
#if NEXUS_P_DEBUG_MODULE_LOCKS
    if(info) {
        if(BLIFO_WRITE_PEEK(&info->stack)>0) {
            NEXUS_P_LockEntry *entry = BLIFO_WRITE(&info->stack);
            entry->module = module;
            entry->pFileName = pFileName;
            entry->lineNumber = lineNumber;
            BLIFO_WRITE_COMMIT(&info->stack, 1);
        } else {
            BDBG_WRN(("NEXUS_Module_Lock[%s]: overflow of lock LIFO %s:%u", info->pThreadName, NEXUS_P_FILENAME(pFileName), lineNumber));
        }
    }
#endif
    NEXUS_P_MODULE_STATS_STOP();
    return;
    BSTD_UNUSED(info);
}


bool
NEXUS_Module_TryLock_Tagged(NEXUS_ModuleHandle module, const char *pFileName, unsigned lineNumber )
{
    NEXUS_P_ThreadInfo *info;

    BERR_Code rc;
    BSTD_UNUSED(pFileName);
    BSTD_UNUSED(lineNumber);
    BDBG_OBJECT_ASSERT(module, NEXUS_Module);
#if NEXUS_P_DEBUG_MODULE_LOCKS
    info = NEXUS_P_ThreadInfo_Get();
#endif
    NEXUS_Module_P_CheckLock("NEXUS_Module_TryLock_Tagged", info, module, pFileName, lineNumber);
    rc = BKNI_TryAcquireMutex_tagged(module->lock, pFileName, lineNumber);
#if NEXUS_P_DEBUG_MODULE_LOCKS
    if(rc==BERR_SUCCESS && info) {
        if(BLIFO_WRITE_PEEK(&info->stack)>0) {
            NEXUS_P_LockEntry *entry = BLIFO_WRITE(&info->stack);
            entry->module = module;
            entry->pFileName = pFileName;
            entry->lineNumber = lineNumber;
            BLIFO_WRITE_COMMIT(&info->stack, 1);
        } else {
            BDBG_WRN(("NEXUS_Module_TryLock[%s]: overflow of lock LIFO %s:%u", info->pThreadName, NEXUS_P_FILENAME(pFileName), lineNumber));
        }
    }
#endif
    return rc==BERR_SUCCESS;
    BSTD_UNUSED(info);
}

void
NEXUS_Module_Unlock_Tagged(NEXUS_ModuleHandle module, const char *pFileName, unsigned lineNumber)
{
    NEXUS_P_ThreadInfo *info;

    BSTD_UNUSED(pFileName);
    BSTD_UNUSED(lineNumber);
    BDBG_OBJECT_ASSERT(module, NEXUS_Module);
#if NEXUS_P_DEBUG_MODULE_LOCKS
    info = NEXUS_P_ThreadInfo_Get();
    if(info) {
        if(BLIFO_READ_PEEK(&info->stack)>0) {
            NEXUS_P_LockEntry *entry = BLIFO_READ(&info->stack);
            if(entry->module!=module) {
                BDBG_ERR(("NEXUS_Module_Unlock[%s]: not paired unlock operation %s(%s:%u), last lock:%s(%s:%u) ", info->pThreadName,  module->pModuleName, NEXUS_P_FILENAME(pFileName), lineNumber, entry->module->pModuleName, NEXUS_P_FILENAME(entry->pFileName), entry->lineNumber));
                BDBG_ASSERT(0);
            }
            BLIFO_READ_COMMIT(&info->stack, 1);
        } else {
            BDBG_ERR(("NEXUS_Module_Unlock[%s]: underflow of lock LIFO (%s:%u)", info->pThreadName, NEXUS_P_FILENAME(pFileName), lineNumber));
            BDBG_ASSERT(0);
        }
    }
#endif
    BKNI_ReleaseMutex(module->lock);
    return;
    BSTD_UNUSED(info);
}

void
NEXUS_Module_Enable_Tagged(NEXUS_ModuleHandle module, const char *pFileName, unsigned lineNumber)
{    
    BSTD_UNUSED(pFileName);
    BSTD_UNUSED(lineNumber);
    BDBG_OBJECT_ASSERT(module, NEXUS_Module);
    BDBG_ASSERT(module->enabled == false);
    
    module->enabled = true;
    BKNI_ReleaseMutex(module->lock);
    BDBG_MSG(("Enabling %s Module", module->pModuleName));    

    return;
}

void
NEXUS_Module_Disable_Tagged(NEXUS_ModuleHandle module, const char *pFileName, unsigned lineNumber)
{   
    BERR_Code rc;
    BSTD_UNUSED(pFileName);
    BSTD_UNUSED(lineNumber);
    BDBG_OBJECT_ASSERT(module, NEXUS_Module);        
    BDBG_ASSERT(module->enabled == true);
    
    BDBG_MSG(("Disabling %s Module", module->pModuleName));
    rc = BKNI_AcquireMutex_tagged(module->lock, pFileName, lineNumber);
    BDBG_ASSERT(rc==BERR_SUCCESS);      

    module->enabled = false;

    return;
}

void
NEXUS_Module_Use(NEXUS_ModuleHandle upModule, NEXUS_ModuleHandle downModule, const char *pFileName, unsigned lineNumber)
{
    BSTD_UNUSED(pFileName);
    BSTD_UNUSED(lineNumber);
    BSTD_UNUSED(upModule);
    BSTD_UNUSED(downModule);
    return;
}

void NEXUS_Module_SetCurrentClient(NEXUS_ModuleHandle module, NEXUS_HeapHandle heap, NEXUS_HeapHandle fullyMappedHeap, bool untrusted)
{
    module->currentClient.heap = heap;
    module->currentClient.fullyMappedHeap = fullyMappedHeap;
    module->currentClient.untrusted = untrusted;
}

NEXUS_HeapHandle NEXUS_Module_GetDefaultHeap(NEXUS_ModuleHandle module, NEXUS_DefaultHeapType defaultHeapType)
{
    switch (defaultHeapType) {
    case NEXUS_DefaultHeapType_eFull:
        return module->currentClient.fullyMappedHeap;
    case NEXUS_DefaultHeapType_eBounds:
        if (module->currentClient.untrusted) {
            return module->currentClient.heap;
        }
        else {
            return NULL;
        }
    default: /* eAny */
        return module->currentClient.heap;
    }
}

void
NEXUS_P_ThreadInfo_Init(NEXUS_P_ThreadInfo *info)
{
#if NEXUS_P_DEBUG_MODULE_LOCKS
    BLIFO_INIT(&info->stack, info->locks, NEXUS_P_BASE_MAX_LOCKS);
#endif
    info->nexusThread = NULL;
    info->threadId = NULL;
    info->pThreadName = "";
    info->client = NULL;
    return ;
}

void
NEXUS_Module_GetPriority(NEXUS_ModuleHandle module, NEXUS_ModulePriority *pPriority)
{
    BDBG_OBJECT_ASSERT(module, NEXUS_Module);
    BDBG_ASSERT(pPriority);
    *pPriority = module->settings.priority;
    return ;
}


bool NEXUS_Module_ActiveStandyCompatible(NEXUS_ModuleHandle module)
{
    BDBG_OBJECT_ASSERT(module, NEXUS_Module);
    
    if(module->settings.priority == NEXUS_ModulePriority_eIdleActiveStandby ||
       module->settings.priority == NEXUS_ModulePriority_eLowActiveStandby ||
       module->settings.priority == NEXUS_ModulePriority_eHighActiveStandby) {
    return true;
    } else {
    return false;
    }
}


#ifdef NO_OS_DIAGS
void
NEXUS_Base_NO_OS_Scheduler_Dispatch(void)
{
    int idx;
    for (idx=0; idx<NEXUS_ModulePriority_eMax; idx++)
    {
        if (NEXUS_P_Base_State.schedulers[idx])
            NEXUS_P_NO_OS_Scheduler_Thread(NEXUS_P_Base_State.schedulers[idx]);
    }
}
#endif /* NO_OS_DIAGS */

void
NEXUS_Module_EnumerateAll(void (*callback)(void *context, NEXUS_ModuleHandle module, const char *pModuleName, const NEXUS_ModuleSettings *pSettings), void *context)
{
    NEXUS_ModuleHandle module;
    NEXUS_LockModule();
    for(module=BLST_S_FIRST(&NEXUS_P_Base_State.modules); module!=NULL; module=BLST_S_NEXT(module, link)) {
        callback(context,module,module->pModuleName,&module->settings);
    }
    NEXUS_UnlockModule();
    return ;
}

int NEXUS_StrCmp(const char *str1, const char *str2)
{
    if(str1==NULL) {
        if(str2==NULL) {
            return 0;
        } else {
            return 1;
        }
    } else if(str2==NULL) {
        return -1;
    } else {
        return NEXUS_P_Base_StrCmp(str1, str2);
    }
}

BDBG_OBJECT_ID(NEXUS_CallbackHandler);

BDBG_FILE_MODULE(nexus_callbackhandler);
#define BDBG_MSG_CALLBACK(x)    /* BDBG_MODULE_MSG(nexus_callbackhandler, x) */

void NEXUS_Base_P_CallbackHandler_Init(NEXUS_CallbackHandler *handler, NEXUS_ModuleHandle module, void (*pCallback)(void *), void *pContext,const char *pFileName,unsigned lineNumber)
{
    BDBG_ASSERT(handler);
    BDBG_OBJECT_INIT(handler, NEXUS_CallbackHandler);
    BDBG_MSG_CALLBACK(("Init:%#x", (unsigned)handler));
    handler->module = module;
    handler->pCallback = pCallback;
    handler->pContext = pContext;
    handler->pFileName = pFileName;
    handler->lineNumber = lineNumber;
    handler->pCallbackGuard = NULL;
    handler->timer = NULL;
    return;
}

static void NEXUS_Base_P_CallbackHandler_TimerDispatch(void *context)
{
    NEXUS_CallbackHandler *handler=context;
    BDBG_OBJECT_ASSERT(handler, NEXUS_CallbackHandler);
    /* module lock already acquired by the scheduller */
    BKNI_AcquireMutex(NEXUS_P_Base_State.callbackHandlerLock);
    handler->timer=NULL;
    BKNI_ReleaseMutex(NEXUS_P_Base_State.callbackHandlerLock);
    if(handler->pCallbackGuard) {
        NEXUS_Error rc = handler->pCallbackGuard(handler->pContext);
        if(rc!=NEXUS_SUCCESS) {
            BDBG_MSG_CALLBACK(("Timer Deflected:%#x", (unsigned)handler));
            return;
        }
    }
    BDBG_MSG_CALLBACK(("Timer:%#x", (unsigned)handler));
    handler->pCallback(handler->pContext);
    return;
}

static void NEXUS_Base_P_CallbackHandler_TimerCancel(NEXUS_CallbackHandler *handler)
{
    BDBG_MSG_CALLBACK(("TimerCanceled:%#x %#x", (unsigned)handler, (unsigned)handler->timer));
    BKNI_AcquireMutex(NEXUS_P_Base_State.callbackHandlerLock);
    if(handler->timer) {
        NEXUS_Module_CancelTimer(handler->module, handler->timer, handler->pFileName, handler->lineNumber);
        handler->timer = NULL;
    }
    BKNI_ReleaseMutex(NEXUS_P_Base_State.callbackHandlerLock);
    return;
}

void NEXUS_Base_P_CallbackHandler_Dispatch(void *context, int arg)
{
    NEXUS_CallbackHandler *handler=context;
    BSTD_UNUSED(arg);
    BDBG_OBJECT_ASSERT(handler, NEXUS_CallbackHandler);
    if(handler->pCallbackGuard) {
        NEXUS_Error rc = handler->pCallbackGuard(handler->pContext);
        if(rc!=NEXUS_SUCCESS) {
            BDBG_MSG_CALLBACK(("Deflected:%#x", (unsigned)handler));
            return;
        }
    }

    if(NEXUS_Module_TryLock_Tagged(handler->module, handler->pFileName, handler->lineNumber)) {
        /* don't check timer, if it was set it would just cause 1:1 mapping between schedulled callback and callbacks executed */
        BDBG_MSG_CALLBACK(("Locked:%#x", (unsigned)handler));
        handler->pCallback(handler->pContext);
        NEXUS_Module_Unlock_Tagged(handler->module, handler->pFileName, handler->lineNumber);
    } else {
        BKNI_AcquireMutex(NEXUS_P_Base_State.callbackHandlerLock);
        if(handler->timer==NULL) {
            BDBG_MSG_CALLBACK(("Queued:%#x", (unsigned)handler));
            handler->timer = NEXUS_Module_P_ScheduleTimer(handler->module, 0, NEXUS_Base_P_CallbackHandler_TimerDispatch, handler, handler->pFileName, handler->lineNumber);
        } else {
            BDBG_MSG_CALLBACK(("Skipped:%#x %#x", (unsigned)handler, (unsigned)handler->timer));
            /* however if timer is already set, then multiple callbacks get collapsed, this is fine, since callbacks don't have guarantees to get called for each  'Fire' function */
        }
        BKNI_ReleaseMutex(NEXUS_P_Base_State.callbackHandlerLock);
    }
    return;
}

void NEXUS_Base_P_CallbackHandler_Stop(NEXUS_CallbackHandler *handler)
{
    BDBG_OBJECT_ASSERT(handler, NEXUS_CallbackHandler);
    BDBG_MSG_CALLBACK(("Stop:%#x", (unsigned)handler));
    /* cancel any pending timer */
    NEXUS_Base_P_CallbackHandler_TimerCancel(handler);
    return;
}


void NEXUS_Base_P_CallbackHandler_Shutdown(NEXUS_CallbackHandler *handler)
{
    BDBG_OBJECT_ASSERT(handler, NEXUS_CallbackHandler);
    BDBG_MSG_CALLBACK(("Shutdown:%#x", (unsigned)handler));
    NEXUS_Base_P_CallbackHandler_TimerCancel(handler);
#if 0
    BDBG_OBJECT_UNSET(handler, NEXUS_CallbackHandler);
#else
    BDBG_OBJECT_DESTROY(handler, NEXUS_CallbackHandler);
#endif
    return;
}


static void NEXUS_Base_P_FreeThreadInfo_locked(NEXUS_P_ThreadInfo *threadInfo)
{
    if(threadInfo >= NEXUS_P_Base_State.userThreadInfo.info && threadInfo < NEXUS_P_Base_State.userThreadInfo.info+NEXUS_BASE_P_MAX_USER_THREADS) {
        unsigned index = threadInfo - NEXUS_P_Base_State.userThreadInfo.info;
        NEXUS_P_Base_State.userThreadInfo.lastUsed[index] = 0;
    }
    BLST_AA_TREE_REMOVE(NEXUS_P_ThreadInfoTree, &NEXUS_P_Base_State.userThreadInfo.tree, threadInfo);
    threadInfo->nexusThread = NULL;
    threadInfo->threadId = NULL;
    return;
}

static void NEXUS_Base_P_TickThreadInfo_locked(NEXUS_P_ThreadInfo *threadInfo)
{
    unsigned index;
    unsigned tick;   
    BDBG_ASSERT(threadInfo >= NEXUS_P_Base_State.userThreadInfo.info && threadInfo < NEXUS_P_Base_State.userThreadInfo.info+NEXUS_BASE_P_MAX_USER_THREADS);
    index = threadInfo - NEXUS_P_Base_State.userThreadInfo.info;
    tick = NEXUS_P_Base_State.userThreadInfo.currentTick+1; 
    tick = tick == 0 ? 1 : tick; /* skip 0 */
    NEXUS_P_Base_State.userThreadInfo.currentTick = tick;
    NEXUS_P_Base_State.userThreadInfo.lastUsed[index] = tick;
    return;
}

static NEXUS_P_ThreadInfo *NEXUS_Base_P_AllocateThreadInfo_locked(void *threadId)
{
    unsigned i;
    unsigned min = 0;
    NEXUS_P_ThreadInfo *threadInfo = NULL;
    NEXUS_P_ThreadInfo *insertedThreadInfo = NULL;

    BDBG_ASSERT(threadId!=NULL);

    for(i=min+1;i<NEXUS_BASE_P_MAX_USER_THREADS;i++) {
        if(NEXUS_P_Base_State.userThreadInfo.lastUsed[i]==0) { /* BINGO found unused entry */
            min = i;
            break;
        } else if(NEXUS_P_Base_State.userThreadInfo.lastUsed[i] < NEXUS_P_Base_State.userThreadInfo.lastUsed[min]) { /* search minimal entry */
            min = i;
        }
    }
    threadInfo = NEXUS_P_Base_State.userThreadInfo.info + min;
    if(NEXUS_P_Base_State.userThreadInfo.lastUsed[min]) { /* if found used entry, it should be freed first */
        NEXUS_Base_P_FreeThreadInfo_locked(threadInfo);
    }
    NEXUS_P_ThreadInfo_Init(threadInfo);
    threadInfo->threadId = threadId;
    insertedThreadInfo = BLST_AA_TREE_INSERT(NEXUS_P_ThreadInfoTree, &NEXUS_P_Base_State.userThreadInfo.tree, threadId, threadInfo);
    BDBG_ASSERT(insertedThreadInfo == threadInfo); /* this could fail if there was already element with thes ame threadId */
    BSTD_UNUSED(insertedThreadInfo);
    NEXUS_Base_P_TickThreadInfo_locked(threadInfo);
    return threadInfo;
}

NEXUS_P_ThreadInfo *NEXUS_Base_P_AllocateThreadInfo(void *threadId)
{
    NEXUS_P_ThreadInfo *threadInfo;
    BKNI_AcquireMutex(NEXUS_P_Base_State.userThreadInfo.lock);
    threadInfo = BLST_AA_TREE_FIND(NEXUS_P_ThreadInfoTree, &NEXUS_P_Base_State.userThreadInfo.tree, threadId); /* try to find it in already allocated pool */
    if(threadInfo) { 
        NEXUS_P_ThreadInfo_Init(threadInfo);
        threadInfo->threadId = threadId;
    } else {
        threadInfo = NEXUS_Base_P_AllocateThreadInfo_locked(threadId); /* new threadInfo could be allocated only for unique threadId */
    }
    BKNI_ReleaseMutex(NEXUS_P_Base_State.userThreadInfo.lock);
    return threadInfo;
}


void NEXUS_Base_P_Thread_AssociateInfo(NEXUS_ThreadHandle thread, void *threadId, NEXUS_P_ThreadInfo *threadInfo)
{
    NEXUS_P_ThreadInfo *insertedThreadInfo;
    BKNI_AcquireMutex(NEXUS_P_Base_State.userThreadInfo.lock);
    threadInfo->nexusThread = thread;
    threadInfo->threadId = threadId;
    insertedThreadInfo = BLST_AA_TREE_INSERT(NEXUS_P_ThreadInfoTree, &NEXUS_P_Base_State.userThreadInfo.tree, threadId, threadInfo);
    if(insertedThreadInfo != threadInfo) { /* insertedThreadInfo points to thread with the same key _that_ is already in the tree */
        if(NEXUS_P_Base_State.userThreadInfo.cache.threadInfo == insertedThreadInfo) {
            NEXUS_P_Base_State.userThreadInfo.cache.threadId = NULL; /* clear stale cache entry */
            NEXUS_P_Base_State.userThreadInfo.cache.threadInfo = NULL;
        }
        NEXUS_Base_P_FreeThreadInfo_locked(insertedThreadInfo);
        insertedThreadInfo = BLST_AA_TREE_INSERT(NEXUS_P_ThreadInfoTree, &NEXUS_P_Base_State.userThreadInfo.tree, threadId, threadInfo);
        BDBG_ASSERT(insertedThreadInfo == threadInfo); /* now it should be really inserted */
        BSTD_UNUSED(insertedThreadInfo);
    }
    BKNI_ReleaseMutex(NEXUS_P_Base_State.userThreadInfo.lock);
    return;
}

void NEXUS_Base_P_Thread_DisassociateInfo(NEXUS_ThreadHandle thread, NEXUS_P_ThreadInfo *threadInfo)
{
    BSTD_UNUSED(thread);
    BKNI_AcquireMutex(NEXUS_P_Base_State.userThreadInfo.lock);
    BDBG_ASSERT(threadInfo->nexusThread == thread);
    if(NEXUS_P_Base_State.userThreadInfo.cache.threadId == threadInfo->threadId) {
        NEXUS_P_Base_State.userThreadInfo.cache.threadId = NULL; /* clear stale cache entry */
        NEXUS_P_Base_State.userThreadInfo.cache.threadInfo = NULL;
    }
    NEXUS_Base_P_FreeThreadInfo_locked(threadInfo);

    BKNI_ReleaseMutex(NEXUS_P_Base_State.userThreadInfo.lock);
    return;
}

NEXUS_P_ThreadInfo *NEXUS_Base_P_Thread_GetInfo(void *threadId)
{
    NEXUS_P_ThreadInfo *threadInfo;

    if(!NEXUS_P_Base_State.coreInit) {
        return NULL; /* there are case when NEXUS_Base_P_Thread_GetInfo called before/after nexus was initialized/uninitialized */
    }
    BKNI_AcquireMutex(NEXUS_P_Base_State.userThreadInfo.lock);
    if(NEXUS_P_Base_State.userThreadInfo.cache.threadId == threadId) { /* test cache first */
        threadInfo = NEXUS_P_Base_State.userThreadInfo.cache.threadInfo;
    } else {
        threadInfo = BLST_AA_TREE_FIND(NEXUS_P_ThreadInfoTree, &NEXUS_P_Base_State.userThreadInfo.tree, threadId);
        if(threadInfo == NULL) { /* allocate a new */
            threadInfo = NEXUS_Base_P_AllocateThreadInfo_locked(threadId);
        }
        NEXUS_P_Base_State.userThreadInfo.cache.threadId = threadId; /* update cache */
        NEXUS_P_Base_State.userThreadInfo.cache.threadInfo = threadInfo;
        if(threadInfo >= NEXUS_P_Base_State.userThreadInfo.info && threadInfo < NEXUS_P_Base_State.userThreadInfo.info+NEXUS_BASE_P_MAX_USER_THREADS) {
            /* internally allocated */
            NEXUS_Base_P_TickThreadInfo_locked(threadInfo);
        }
    }
    BKNI_ReleaseMutex(NEXUS_P_Base_State.userThreadInfo.lock);
    return threadInfo;
}

void NEXUS_Base_P_TickThreadInfo(NEXUS_P_ThreadInfo *threadInfo)
{
    if(threadInfo >= NEXUS_P_Base_State.userThreadInfo.info && threadInfo < NEXUS_P_Base_State.userThreadInfo.info+NEXUS_BASE_P_MAX_USER_THREADS) {
        /* delay serialization  point */
        BKNI_AcquireMutex(NEXUS_P_Base_State.userThreadInfo.lock);
        NEXUS_Base_P_TickThreadInfo_locked(threadInfo);
        BKNI_ReleaseMutex(NEXUS_P_Base_State.userThreadInfo.lock);
    }
    return;
}

