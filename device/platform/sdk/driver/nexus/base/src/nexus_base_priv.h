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
* $brcm_Workfile: nexus_base_priv.h $
* $brcm_Revision: 19 $
* $brcm_Date: 9/28/12 11:19a $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/base/src/nexus_base_priv.h $
* 
* 19   9/28/12 11:19a erickson
* SW7435-362: stop scheduler threads before uninitializing modules. this
*  ensures no lock is held when it is destroyed.
* 
* 18   9/27/12 1:12p erickson
* SW7435-362: enforce module lock is held when creating/destroying
*  callbacks, timers and events
* 
* 17   7/26/12 4:43p vsilyaev
* SW7435-280: Moved NEXUS_Time_Add to the private API
* 
* 16   7/25/12 1:43p vsilyaev
* SW7435-267: Collect statistics per module (module lock) and per
*  scheduler
* 
* 15   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 14   7/9/12 1:58p erickson
* SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
* 
* 13   11/29/11 11:37a erickson
* SW7420-2129: store current default heap per module
* 
* 12   7/21/11 2:32p vsilyaev
* SW7125-1014 : Add module enable/disable
* 
* SW7125-1014/1   7/20/11 4:38p gmohile
* SW7125-1014 : Add module enable/disable
* 
* 11   9/23/10 2:12p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 10   4/8/09 12:30p erickson
* PR53862: reintroduce NEXUS_P_SchedulerGetInfo
*
* 9   1/20/09 3:46p vsilyaev
* PR 49285, PR 50582: Improved debugability of scheduller and fixed
*  problem in the scheduller, where it goes to WaitForGroup to extended
*  amount of time
*
* 8   6/18/08 1:12p erickson
* PR43758: fix warnings, misspellings
*
* 7   6/9/08 7:11p vsilyaev
* PR 43184: Changed phase of detection for outstanding callbacks
*
* 6   6/5/08 2:19p vsilyaev
* PR 42318: Rearranged scheduller to share the same context in user and
*  kernel mode, this allows to serve callback from kernel to user with
*  single transition kernel<->user
*
* 5   5/28/08 5:49p vsilyaev
* PR 42318: Reworked scheduller to allow use of externally created thread
*
* 4   4/8/08 10:40a vsilyaev
* PR 41083: Added bprofile support
*
* 3   2/28/08 9:39p vsilyaev
* PR 40103: Added NEXUS_TaskCallback functions
*
* 2   1/31/08 11:49a vsilyaev
* PR 38682: Added support for runtime options
*
* 1   1/18/08 2:13p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/7   1/11/08 3:49p vsilyaev
* PR 38682: Fixed typos and removed unused code.
*
* Nexus_Devel/6   10/23/07 4:16p vsilyaev
* PR 36199: Added memory mapper sub-module
*
* Nexus_Devel/5   10/10/07 4:19p vsilyaev
* PR 35824: Added NEXUS_Time type
*
* Nexus_Devel/4   9/13/07 5:20p vsilyaev
* PR 34419: Use plugins for OS
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
#ifndef NEXUS_BASE_PRIV_H__
#define NEXUS_BASE_PRIV_H__

#include "nexus_base_os.h"
#include "nexus_base_os_priv.h"
#include "blifo.h"
#include "bkni_event_group.h"

extern NEXUS_ModuleHandle NEXUS_Base;
#define NEXUS_MODULE_SELF NEXUS_Base

#ifdef __cplusplus
extern "C"
{
#endif


BERR_Code NEXUS_P_Base_Os_Init(void);
void NEXUS_P_Base_Os_Uninit(void);

void NEXUS_P_Base_Scheduler_Init(void);
void NEXUS_P_Base_Scheduler_Uninit(void);


typedef struct NEXUS_P_Scheduler NEXUS_P_Scheduler;

BDBG_OBJECT_ID_DECLARE(NEXUS_Module);

struct b_objdb;
struct NEXUS_Module {
    BDBG_OBJECT(NEXUS_Module)
    BKNI_MutexHandle lock;
    NEXUS_P_Scheduler *scheduler;
    NEXUS_ModuleSettings settings;
    const char *pModuleName;
    BLST_S_ENTRY(NEXUS_Module) link;
    struct b_objdb *objdb; /* pointer to the object data base used by this module */
    bool enabled;
    struct {
        NEXUS_HeapHandle heap, fullyMappedHeap; /* default heap for current client in this module */
        bool untrusted; /* current client in this module is untrusted */
    } currentClient;
};

NEXUS_P_Scheduler *NEXUS_P_Scheduler_Create(NEXUS_ModulePriority priority, const char *name, const NEXUS_ThreadSettings *pSettings);
NEXUS_P_Scheduler *NEXUS_P_Scheduler_Init(NEXUS_ModulePriority priority, const char *name, const NEXUS_ThreadSettings *pSettings); /* initialiase scheduller for use with external thread */

void NEXUS_P_Scheduler_Stop(NEXUS_P_Scheduler *scheduler);
void NEXUS_P_Scheduler_Destroy(NEXUS_P_Scheduler *scheduler);
void NEXUS_P_MapInit(void);

typedef struct NEXUS_P_SchedulerInfo {
    BKNI_MutexHandle callback_lock; /* callback that is acquired when callback active */
} NEXUS_P_SchedulerInfo;
void NEXUS_P_SchedulerGetInfo(NEXUS_P_Scheduler *scheduler, NEXUS_P_SchedulerInfo *info);

NEXUS_Error NEXUS_P_Scheduler_Step(NEXUS_P_Scheduler *scheduler, unsigned timeout, NEXUS_P_Base_Scheduler_Status *status, bool (*complete)(void *context), void *context);
const char *NEXUS_P_PrepareFileName(const char *pFileName);
NEXUS_TimerHandle NEXUS_Module_P_ScheduleTimer(NEXUS_ModuleHandle module, unsigned delayMs, void (*pCallback)(void *),  void *pContext, const char *pFileName, unsigned lineNumber);

/**
Summary:
Adds delta in milliseconds to the timestamp.

Description:
    Delta could be only positive. Trying to add negative delta would result in no-op. And there is no provision for stable addition.
    For example:
    NEXUS_Time_Add(1); NEXUS_Time_Add(1); NEXUS_Time_Add(1); NEXUS_Time_Add(1);
    And
    NEXUS_Time_Add(4);
    Could produce different results.
**/
void NEXUS_Time_Add(
    NEXUS_Time *time,
    long delta_ms /* milliseconds */
    );


#ifdef __cplusplus
}
#endif

#endif /* NEXUS_BASE_PRIV_H__ */
