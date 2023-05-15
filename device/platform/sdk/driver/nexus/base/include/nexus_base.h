/***************************************************************************
*     (c)2007-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_base.h $
* $brcm_Revision: 42 $
* $brcm_Date: 9/28/12 11:19a $
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/nexus_base.h $
* 
* 42   9/28/12 11:19a erickson
* SW7435-362: stop scheduler threads before uninitializing modules. this
*  ensures no lock is held when it is destroyed.
* 
* 41   9/4/12 1:11p vsilyaev
* SW7420-1004: Removed NEXUS_GetLastError
* 
* 40   8/3/12 3:50p vsilyaev
* SW7420-2330: Added control to module and callback debug features
* 
* 39   7/26/12 4:43p vsilyaev
* SW7435-280: Moved NEXUS_Time_Add to the private API
* 
* 38   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 37   7/9/12 1:58p erickson
* SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
* 
* 36   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 35   12/15/11 1:23p vsilyaev
* SW7420-2085: Assert on object validity before accessing reference
*  counter
* 
* 34   11/29/11 11:37a erickson
* SW7420-2129: store current default heap per module
* 
* 33   11/4/11 1:54p vsilyaev
* SW7231-439: Improved debuggability
* 
* 32   10/7/11 1:49p vsilyaev
* SW7420-2085: Added support for object reference counting
* 
* 31   9/14/11 2:10p erickson
* SW7405-5478: add NEXUS_Module_GetName for debug
* 
* 30   5/3/11 5:07p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
* 29   4/20/11 7:56p jtna
* SW7425-365: added back previous definition of NEXUS_OFFSETOF as comment
*  for reference
* 
* 28   4/20/11 7:43p jtna
* SW7425-365: change NEXUS_OFFSETOF macro to use offsetof macro, to
*  silence build warnings
* 
* 27   4/19/11 12:23p vsilyaev
* SW7405-5221: Route nexus BERR_TRACE through common route
* 
* 26   4/4/11 6:14p vsilyaev
* SW7405-5221: Added option to forward debug output to FIFO
* 
* 25   3/28/11 3:45p vsilyaev
* SW7335-1214: Added NEXUS_CallbackHandler framework
* 
* 24   1/19/11 11:11a erickson
* SW7420-1123: add NEXUS_Module_GetSettings
* 
* 23   12/16/10 4:04p vsilyaev
* SW7425-40: Added macros to verify memory layour of structure members
* 
* 22   11/15/10 3:11p vsilyaev
* SW7405-4996: Added NEXUS_StrCmp
* 
* 21   11/3/10 4:16p erickson
* SW7420-1223: refactor for application use of pointer/offset conversion
*  functions
* 
* 20   9/23/10 2:12p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 19   9/15/10 3:38p jtna
* SW7420-1004: added BERR_StoreError and NEXUS_GetLastError_Os
*
* 18   7/14/10 11:59a erickson
* SW7405-4621: split out nexus_base_os.h
*
* 17   7/14/10 10:27a erickson
* SW7405-4621: narrow #include to minimum
*
* 16   8/10/09 11:55a erickson
* PR57475: added comment that NEXUS_SetEnv(name, NULL) is supported
*
* 15   6/18/09 11:49a vsilyaev
* PR 54926: Added debug options to the module settings
*
* 14   4/8/09 4:31p vsilyaev
* PR 54016: Added support for precompiled header
*
* 13   1/26/09 11:06a erickson
* PR35457: clarify comment
*
* 12   10/3/08 11:18a vsilyaev
* PR 45361: Fixed BTRC and BPROFILE support with -std=c89
*
* 11   9/22/08 3:26p vsilyaev
* PR 42318: Removed bkni_multi.h
*
* 10   9/22/08 3:24p vsilyaev
* PR 42318: Use linux specific files for linux specific prototypes
*
* 9   6/10/08 3:29p vsilyaev
* PR 43184: Added support for Start/Stop callbacks in linux kernel/user
*  proxy mode
*
* 8   6/9/08 7:11p vsilyaev
* PR 43184: Changed phase of detection for outstanding callbacks
*
* 7   6/5/08 2:19p vsilyaev
* PR 42318: Rearranged scheduller to share the same context in user and
*  kernel mode, this allows to serve callback from kernel to user with
*  single transition kernel<->user
*
* 6   4/8/08 10:40a vsilyaev
* PR 41083: Added bprofile support
*
* 5   3/17/08 12:46p erickson
* PR40103: remove NEXUS_QueueCallback
*
* 4   2/28/08 10:41p vsilyaev
* PR 40103: Added interfaceHandle and settings for the
*  NEXUS_IsrCallbackCreate
*
* 3   2/28/08 9:39p vsilyaev
* PR 40103: Added NEXUS_TaskCallback functions
*
* 2   2/25/08 2:58p erickson
* PR39786: correct comment
*
* 1   1/18/08 2:12p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/22   11/28/07 2:11p erickson
* PR35457: doc update
*
***************************************************************************/
#ifndef NEXUS_BASE_H
#define NEXUS_BASE_H

#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "nexus_base_types.h"  /* base types which are also available to applications via nexus_core symlink */
#include "nexus_base_os.h"     /* functions that must be implemented in OS abstraction */
#include "nexus_base_mmap.h"   /* functions for nexus_base_mmap.c */
#include "nexus_base_driver.h" /* types that must be used by platform/driver code */
#include "nexus_base_object.h" /* types and functions to manage the nexus objects */

/*=***********************
Nexus Base is an internal component of Nexus. It is not a module. It is not callable by applications.

Nexus Base provides the following features to modules:

1. Module locking
Each module creates a handle which is uses for locking.
See NEXUS_LockModule.

2. Callback support
Invoke callbacks which are given with NEXUS_CallbackDesc.
NEXUS_TaskCallback and NEXUS_IsrCallback set up handles which can be used to fire a callback from task or isr context.

3. Events
Receive a synchronized task callback when a BKNI_EventHandle is set.
See NEXUS_RegisterEvent.

4. Timers
Receive a synchronized task callback after a specified time interval.
See NEXUS_ScheduleTimer.

5. Synchronized callback handler
Receive a syncrhroniized task callback after other nexus component issued asynchronous callback

See nexus_base_os.h for threads and misc functions callable inside and outside nexus.
*************************/


#ifdef __cplusplus
#error Internal header file cannot be built by C++ compiler
#endif

/**
Summary:
Initialize the Nexus Base Core.

Description:
This is called from Platform initialization code.
It should be called after magnum initialization and prior to  NEXUS_Base_Init

See Also:
NEXUS_Base_Init
NEXUS_Base_Core_Uninit
**/
NEXUS_Error NEXUS_Base_Core_Init(void);

/**
Summary:
Uninit the Nexus Base Core.

Description:
This is called from Platform uninitialization code.
It should be called after NEXUS_Base_Uninit and prior to magnum uninitialization

See Also:
NEXUS_Base_Core_Init
NEXUS_Base_Uninit
**/
void NEXUS_Base_Core_Uninit(void);

/**
Summary:
Initialize the Nexus Base.

Description:
This is called from Platform initialization code.

See Also:
NEXUS_Base_Uninit
**/
NEXUS_Error NEXUS_Base_Init(
    const NEXUS_Base_Settings *pSettings
    );

/**
Summary:
Stop all scheduler threads

Description:
This should be called before destroying modules.
It ensures that no locks are held while they are destroyed.
It means that no module uninit can wait for a timer/callback/event.
**/
void NEXUS_Base_Stop(void);

/**
Summary:
Uninit the Nexus Base.

Description:
This is called from Platform uninitialization code.

See Also:
NEXUS_Base_Init
**/
void NEXUS_Base_Uninit(void);

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_Module_Create
**/
void NEXUS_Module_GetDefaultSettings(
    NEXUS_ModuleSettings *pSettings    /* [out] Default Settings */
    );

/**
Summary:
Create a new Nexus module.

Description:
This is called by each module's Init function.
See Nexus_Architecture for an in-depth discussion of Nexus Modules.
The NEXUS_ModuleHandle is required to use Base services.

Returns:
A handle for the module

See Also:
NEXUS_Module_Destroy
NEXUS_VideoDecoderModule_Init - one usage example
**/
NEXUS_ModuleHandle NEXUS_Module_Create(
    const char *pModuleName,                 /* Module Name -- Should be unique across all modules */
    const NEXUS_ModuleSettings *pSettings    /* Module Settings */
    );


/**
Summary:
Destroy a Nexus module.

Description:

See Also:
NEXUS_Module_Create
NEXUS_VideoDecoderModule_Uninit - one usage example
**/
void NEXUS_Module_Destroy(
    NEXUS_ModuleHandle module
    );

/**
Summary:
Get module name
**/
const char *NEXUS_Module_GetName(NEXUS_ModuleHandle module);

/**
Summary:
Get settings passed into NEXUS_Module_Create
**/
void NEXUS_Module_GetSettings(
    NEXUS_ModuleHandle module,
    NEXUS_ModuleSettings *pSettings    /* [out] */
    );

/**
Summary:
Macro to lock the current module.

Description:
This is called by the synchronization thunk. It should never be called from inside module code.
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
NEXUS_TryLockModule
NEXUS_UnlockModule
**/
#define NEXUS_LockModule() NEXUS_Module_Lock(NEXUS_MODULE_SELF)

/**
Summary:
Macro to try to lock the current module.

Description:
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
NEXUS_LockModule
NEXUS_UnlockModule
**/
#define NEXUS_TryLockModule() NEXUS_Module_TryLock(NEXUS_MODULE_SELF)

/**
Summary:
Unlock the current module.

Description:
This is called by the synchronization thunk. It should never be called from inside module code.
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
**/
#define NEXUS_UnlockModule() NEXUS_Module_Unlock(NEXUS_MODULE_SELF)

/**
Summary:
Assert that the current module is locked.

Description:
This is useful inside private API functions to ensure that the caller has properly
locked the module before calling.

It has little value inside public API functions because the synchronization thunk
already guarantees that the module is locked.

NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
NEXUS_LockModule
**/
#define NEXUS_ASSERT_MODULE() BDBG_ASSERT(NEXUS_Module_Assert(NEXUS_MODULE_SELF))

/**
Summary:
Actual function called by NEXUS_ASSERT_MODULE macro.

Description:

See Also:
NEXUS_ASSERT_MODULE
**/
bool NEXUS_Module_Assert(
    NEXUS_ModuleHandle module
    );

/**
Summary:
Actual function called by NEXUS_LockModule macro.

Description:

See Also:
NEXUS_LockModule
**/
void NEXUS_Module_Lock_Tagged(
    NEXUS_ModuleHandle module,  /* Module Handle */
    const char *pFileName,
    unsigned lineNumber
    );

/**
Summary:
Actual function called by NEXUS_TryLockModule macro.

Description:

See Also:
NEXUS_TryLockModule
**/
bool NEXUS_Module_TryLock_Tagged(
    NEXUS_ModuleHandle module,   /* Module Handle */
    const char *pFileName,
    unsigned lineNumber
    );

/**
Summary:
Actual function called by NEXUS_UnlockModule macro.

Description:

See Also:
NEXUS_UnlockModule
**/
void NEXUS_Module_Unlock_Tagged(
    NEXUS_ModuleHandle module,   /* Module Handle */
    const char *pFileName,
    unsigned lineNumber
    );

/**
Summary:
Declare that this module will make downcalls into the specified module.

Description:
This is a recommended call to help enforce no circular calls between modules.
See Nexus_Architecture.doc for an in-depth discussion of down calls.
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
**/
#if NEXUS_P_DEBUG_LOCK 
#define NEXUS_UseModule(module) NEXUS_Module_Use(NEXUS_MODULE_SELF, (module), __FILE__, __LINE__)
#else 
#define NEXUS_UseModule(module) (void)0
#endif

/**
Summary:
Actual function called by NEXUS_UseModule macro.

Description:

See Also:
NEXUS_UseModule
**/
void NEXUS_Module_Use(
    NEXUS_ModuleHandle upModule,  /* source module handle */
    NEXUS_ModuleHandle downModule,  /* source module handle */
    const char *pFileName,
    unsigned lineNumber
    );

/**
Get/set current heap for current client in the module.

There can only be one client active in a module at a time. The thunk stores the client's current module and trusted/untrusted
state. Nexus modules can retrieve the current heap. Modules should never assume they can use g_pCoreHandles->heap[0] as a default. 
**/
void NEXUS_Module_SetCurrentClient(NEXUS_ModuleHandle module, NEXUS_HeapHandle heap, NEXUS_HeapHandle fullyMappedHeap, bool untrusted);

typedef enum NEXUS_DefaultHeapType 
{
    NEXUS_DefaultHeapType_eFull, /* never null and must have driver mapping. equivalent to server's heap[0]. */
    NEXUS_DefaultHeapType_eAny, /* never null, but may not have driver mapping. for instance, graphics heap. */
    NEXUS_DefaultHeapType_eBounds, /* never null if client is untrusted client. may be null for trusted client. */
    NEXUS_DefaultHeapType_eMax
} NEXUS_DefaultHeapType;
    
NEXUS_HeapHandle NEXUS_Module_GetDefaultHeap(NEXUS_ModuleHandle module, NEXUS_DefaultHeapType defaultHeapType);

#define NEXUS_P_DefaultHeap(HEAP, HEAP_TYPE) ((HEAP)?(HEAP):NEXUS_Module_GetDefaultHeap(NEXUS_MODULE_SELF, (HEAP_TYPE)))

/**
Summary:
Settings for Nexus Callback.

Description:

See Also:
NEXUS_TaskCallback_Create
**/
typedef struct NEXUS_CallbackSettings  {
    NEXUS_ModulePriority priority;
} NEXUS_CallbackSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_TaskCallback_Create
**/
void NEXUS_Callback_GetDefaultSettings(
    NEXUS_CallbackSettings *pSettings /* [out] */
);

/**
Summary:
IsrCallback handle obtained from NEXUS_IsrCallback_Create.

Description:
IsrCallback functions are used to help with firing task level callback from the interrupt context.
To fire task callback from the interrupt context, calls NEXUS_IsrCallback_Create function to
preallocation single NEXUS_IsrCallbackHandle for each NEXUS_CallbackDesc, then user uses
NEXUS_IsrCallback_Fire function in order to fire callback.

See Also:
NEXUS_IsrCallback_Create
**/
typedef struct NEXUS_IsrCallback *NEXUS_IsrCallbackHandle;

/**
Summary:
Create an IsrCallback object so that callbacks can be invoked from an ISR function.

Description:
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
NEXUS_IsrCallback_Destroy
**/
#if NEXUS_P_DEBUG_CALLBACKS 
#define NEXUS_IsrCallback_Create(handler, settings) NEXUS_Module_IsrCallback_Create(NEXUS_MODULE_SELF,(handler), (settings), __FILE__,__LINE__)
#else
#define NEXUS_IsrCallback_Create(handler, settings) NEXUS_Module_IsrCallback_Create(NEXUS_MODULE_SELF,(handler), (settings), NULL, 0)
#endif

/**
Summary:
Destroy the IsrCallback object.

Description:
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
NEXUS_IsrCallback_Create
**/
#define NEXUS_IsrCallback_Destroy(callback) NEXUS_Module_IsrCallback_Destroy(NEXUS_MODULE_SELF,(callback))

/**
Summary:
Set the callback function for this IsrCallback instance.

Description:

See Also:
NEXUS_IsrCallback_Create
**/
#define NEXUS_IsrCallback_Set(callback, pDesc) NEXUS_Module_IsrCallback_Set((callback), (pDesc))

/**
Summary:
Fire the current callback from an ISR function.

Description:
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
NEXUS_IsrCallback_Set
**/
#define NEXUS_IsrCallback_Fire_isr(callback) NEXUS_Module_IsrCallback_Fire_isr(NEXUS_MODULE_SELF, (callback))

/**
Summary:
Actual function called by NEXUS_IsrCallback_Create.

Description:

See Also:
NEXUS_IsrCallback_Create
**/
NEXUS_IsrCallbackHandle NEXUS_Module_IsrCallback_Create(
    NEXUS_ModuleHandle module,
	void *interfaceHandle,
	const NEXUS_CallbackSettings *pSettings,
    const char *pFileName,
    unsigned lineNumber
    );

/**
Summary:
Actual function called by NEXUS_IsrCallback_Destroy.

Description:

See Also:
NEXUS_IsrCallback_Destroy
**/
void NEXUS_Module_IsrCallback_Destroy(
        NEXUS_ModuleHandle module,
        NEXUS_IsrCallbackHandle callback
        );

/**
Summary:
Actual function called by NEXUS_IsrCallback_Set.

Description:

See Also:
NEXUS_IsrCallback_Set
**/
void NEXUS_Module_IsrCallback_Set(
        NEXUS_IsrCallbackHandle callback,
        const NEXUS_CallbackDesc *pDesc
        );

/**
Summary:
Actual function called by NEXUS_IsrCallback_Fire_isr.

Description:

See Also:
NEXUS_IsrCallback_Fire_isr
**/
void NEXUS_Module_IsrCallback_Fire_isr(
    NEXUS_ModuleHandle module,
    NEXUS_IsrCallbackHandle callback /* preallocated IsrCallback */
    );


/**
Summary:
IsrCallback handle obtained from NEXUS_IsrCallback_Create.

Description:
IsrCallback functions are used to fire task level callback from the task context.
To fire task callback from the tasj context, call NEXUS_TaskCallback_Create function to
preallocation single NEXUS_TaskCallbackHandle for each NEXUS_CallbackDesc, then user uses
NEXUS_TaskCallback_Fire function in order to fire callback.

See Also:
NEXUS_IsrCallback_Create
**/
typedef struct NEXUS_TaskCallback *NEXUS_TaskCallbackHandle;

/**
Summary:
Create an TaskCallback object so that callbacks can be invoked from the task context

Description:
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
NEXUS_TaskCallback_Destroy
**/
#if NEXUS_P_DEBUG_CALLBACKS 
#define NEXUS_TaskCallback_Create(handler, settings) NEXUS_Module_TaskCallback_Create(NEXUS_MODULE_SELF,(handler),(settings),__FILE__,__LINE__)
#else
#define NEXUS_TaskCallback_Create(handler, settings) NEXUS_Module_TaskCallback_Create(NEXUS_MODULE_SELF,(handler),(settings),NULL,0)
#endif

/**
Summary:
Destroy the TaskCallback object.

Description:
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
NEXUS_TaskCallback_Create
**/
#define NEXUS_TaskCallback_Destroy(callback) NEXUS_Module_TaskCallback_Destroy(NEXUS_MODULE_SELF,(callback))

/**
Summary:
Set the callback function for this TaskCallback instance.

Description:

See Also:
NEXUS_TaskCallback_Create
**/
#define NEXUS_TaskCallback_Set(callback, pDesc) NEXUS_Module_TaskCallback_Set((callback), (pDesc))

/**
Summary:
Fire the current callback.

Description:
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
NEXUS_TaskCallback_Set
**/
#define NEXUS_TaskCallback_Fire(callback) NEXUS_Module_TaskCallback_Fire(NEXUS_MODULE_SELF, (callback))

/**
Summary:
Actual function called by NEXUS_TaskCallback_Create.

Description:

See Also:
NEXUS_TaskCallback_Create
**/
NEXUS_TaskCallbackHandle
NEXUS_Module_TaskCallback_Create(
        NEXUS_ModuleHandle module,
        void *interfaceHandle,
        const NEXUS_CallbackSettings *pSettings,
        const char *pFileName,
        unsigned lineNumber
        );

/**
Summary:
Actual function called by NEXUS_TaskCallback_Set.

Description:

See Also:
NEXUS_TaskCallback_Set
**/
void NEXUS_Module_TaskCallback_Set(
        NEXUS_TaskCallbackHandle handle,
        const NEXUS_CallbackDesc *pDesc
        );

/**
Summary:
Actual function called by NEXUS_TaskCallback_Destroy.

Description:

See Also:
NEXUS_TaskCallback_Destroy
**/
void NEXUS_Module_TaskCallback_Destroy(
        NEXUS_ModuleHandle module,
        NEXUS_TaskCallbackHandle callback
        );

/**
Summary:
Actual function called by NEXUS_TaskCallback_Fire.

Description:

See Also:
NEXUS_TaskCallback_Fire
**/
void NEXUS_Module_TaskCallback_Fire(
        NEXUS_ModuleHandle module,
        NEXUS_TaskCallbackHandle callback
        );


/**
Summary:
Request that a module's function is called whenever an event is set.

Description:
Base will lock the module before calling the callback.
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.
The event is a BKNI_EventHandle. Events are set with BKNI_SetEvent.
If set an event is set multiple times in rapid succession, you may only receive one callback.

See Also:
NEXUS_UnregisterEvent
NEXUS_Module_RegisterEvent
**/
#if NEXUS_P_DEBUG_CALLBACKS 
#define NEXUS_RegisterEvent(event, callback, context) \
    NEXUS_Module_RegisterEvent(NEXUS_MODULE_SELF, event, callback, context, __FILE__, __LINE__)
#else
#define NEXUS_RegisterEvent(event, callback, context) \
    NEXUS_Module_RegisterEvent(NEXUS_MODULE_SELF, event, callback, context, NULL, 0)
#endif

/**
Summary:
Unregister the callback set up by NEXUS_RegisterEvent

Description:
NEXUS_MODULE_SELF must be defined to be a reference to the NEXUS_ModuleHandle for the current module.

See Also:
NEXUS_RegisterEvent
NEXUS_Module_UnregisterEvent
**/
#if NEXUS_P_DEBUG_CALLBACKS 
#define NEXUS_UnregisterEvent(event) \
    NEXUS_Module_UnregisterEvent(NEXUS_MODULE_SELF, event, __FILE__, __LINE__)
#else
#define NEXUS_UnregisterEvent(event) NEXUS_Module_UnregisterEvent(NEXUS_MODULE_SELF, event, NULL, 0)
#endif


/**
Summary:
Initialize NEXUS_CallbackDesc structure

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_CallbackDesc_Init(
    NEXUS_CallbackDesc *desc /* [out] */
    );

/**
Summary:
Handle obtained from NEXUS_RegisterEvent

Description:

See Also:
NEXUS_RegisterEvent
**/
typedef struct NEXUS_EventCallback *NEXUS_EventCallbackHandle;

/**
Summary:
Actual function called by NEXUS_RegisterEvent macro.

Description:

See Also:
NEXUS_RegisterEvent
**/
NEXUS_EventCallbackHandle NEXUS_Module_RegisterEvent(
    NEXUS_ModuleHandle module,     /* Module Handle */
    BKNI_EventHandle event,        /* Event To Be Registered. See BKNI_CreateEvent. */
    void (*pCallback)(void *),     /* Callback Function */
    void *pContext,                /* Context provided to callback */
    const char *pFileName,
    unsigned lineNumber
    );

/**
Summary:
Actual function called by NEXUS_UnregisterEvent macro.

Description:

See Also:
NEXUS_UnregisterEvent
**/
void NEXUS_Module_UnregisterEvent(
    NEXUS_ModuleHandle module,   /* Module Handle */
    NEXUS_EventCallbackHandle event,      /* Event callback To Be Unregistered */
    const char *pFileName,
    unsigned lineNumber
    );

/**
Summary:
Handle for a timer.

Description:

See Also:
NEXUS_ScheduleTimer
**/
typedef struct NEXUS_Timer *NEXUS_TimerHandle;

/**
Summary:
Schedule a callback after an elapsed time.

Description:
Base will lock the module before calling the callback function.
Timers are one-shot and are self-cancelling. If you want a recurring timer, you should call NEXUS_ScheduleTimer from inside your time callback.

See Also:
NEXUS_CancelTimer
**/
#if NEXUS_P_DEBUG_CALLBACKS 
#define NEXUS_ScheduleTimer(delayMs, callback, context) \
    NEXUS_Module_ScheduleTimer(NEXUS_MODULE_SELF, delayMs, callback, context, __FILE__, __LINE__)
#else
#define NEXUS_ScheduleTimer(delayMs, callback, context) \
    NEXUS_Module_ScheduleTimer(NEXUS_MODULE_SELF, delayMs, callback, context, NULL, 0)
#endif

/**
Summary:
Cancel a timer.

Description:

See Also:
NEXUS_ScheduleTimer
**/
#if NEXUS_P_DEBUG_CALLBACKS 
#define NEXUS_CancelTimer(timer) \
    NEXUS_Module_CancelTimer(NEXUS_MODULE_SELF, timer, __FILE__, __LINE__)
#else
#define NEXUS_CancelTimer(timer) NEXUS_Module_CancelTimer(NEXUS_MODULE_SELF, timer, NULL, 0)
#endif

/**
Summary:
Actual function called by NEXUS_ScheduleTimer macro.

Description:

See Also:
NEXUS_ScheduleTimer
**/
NEXUS_TimerHandle NEXUS_Module_ScheduleTimer(
    NEXUS_ModuleHandle module,     /* Module Handle */
    unsigned delayMs,              /* Delay in Milliseconds */
    void (*pCallback)(void *),     /* Callback function */
    void *pContext,                 /* Context provided to callback */
    const char *pFileName,
    unsigned lineNumber
    );

/**
Summary:
Actual function called by NEXUS_CancelTimer macro.

Description:

See Also:
NEXUS_CancelTimer
**/
void NEXUS_Module_CancelTimer(
    NEXUS_ModuleHandle module,    /* Module Handle */
    NEXUS_TimerHandle timer,      /* Timer to be cancelled */
    const char *pFileName,
    unsigned lineNumber
    );

/**
Summary:
ISR variant of NEXUS_FlushCache

Description:

See Also:
NEXUS_FlushCache
**/
#define NEXUS_FlushCache_isr NEXUS_FlushCache

/**
Summary:
Stop all callbacks associated with this interface handle.
Wait for any running callbacks to finish, and ensure that no other callbacks for this handle are fired.
**/
void NEXUS_Base_P_StopCallbacks(
        void *interfaceHandle
        );

/**
Summary:
Re-enable all callbacks for this interface handle.
All interfaces default to being already started.
If you call StopCallbacks followed by StartCallbacks, no pending callbacks will be lost. <<maybe>>
**/
void NEXUS_Base_P_StartCallbacks(
        void *interfaceHandle
        );

#include "nexus_base_os_types.h"

/**
Summary:
Returns the current timestamp
**/
void NEXUS_Time_Get(
    NEXUS_Time *time
    );

/**
Summary:
Returns difference in milliseconds between two timestamps obtained with b_time_get.

Description:
It's recomended that code would use this function in a way that it could return
only positive values (where applicable, it helps in handling wraparound conditions)
**/
long NEXUS_Time_Diff(
    const NEXUS_Time *future,
    const NEXUS_Time *past
    );


/**
Summary:
Give address mapping data for a block of memory to allow address/offset conversion using functions in nexus_base_mmap.h.

Description:
Multiple calls to NEXUS_P_AddMap can be made to specify a set of memory blocks.

See Also: NEXUS_AddrToOffset NEXUS_OffsetToCachedAddr NEXUS_OffsetToUncachedAddr
**/
NEXUS_Error NEXUS_P_AddMap(
    uint32_t offset,    /* physical offset in memory */
    void *cached,       /* cached address */
    void *uncached,     /* uncached address */
    size_t length       /* size in bytes */
    );

/**
Summary:
In both arguments are not NULL, then works the same as standard strcmp.
Otherwise returns 0 if both str1 and str2 are NULL and returns not 0 if only one argument is NULL.
**/
int NEXUS_StrCmp(
        const char *str1, 
        const char *str2
        );

/**
Summary:
Returns size of the structure or union member
**/
#define NEXUS_SIZEOF(type, field) sizeof(((type *)0)->field)

/**
Summary:
Returns offset of the structure member
**/
#define NEXUS_OFFSETOF(type, field) offsetof(type, field)
/* #define NEXUS_OFFSETOF(type, field) ((uint8_t *)(&((type *)0)->field) - (uint8_t *)0) */ /* this definition generates compiler warnings on 2.6.37 kernel toolchain */

/**
Summary:
Validates that members of two different structures have indentical representation 
**/
#define NEXUS_ASSERT_FIELD(type1, field1, type2, field2) BDBG_CASSERT(NEXUS_OFFSETOF(type1, field1)==NEXUS_OFFSETOF(type2,field2) && NEXUS_SIZEOF(type1,field1)==NEXUS_SIZEOF(type2,field2))

/**
Summary:
Validates that size of two structure is identical
**/
#define NEXUS_ASSERT_STRUCTURE(type1, type2) BDBG_CASSERT(sizeof(type1)==sizeof(type2))

/**
Summary:
    Synchronized callback handler.

Description:
    NEXUS_CallbackHandler provides way for the nexus module to set a handler for the asynchronous callback that its execution serialized with the rest of module API's.
    For specific needs NEXUS_CallbackHandler allowes to set a guard function that could be called without serialization and could guard execution of main handler. 

See Also:
NEXUS_CallbackHandler_Init
NEXUS_CallbackHandler_Shutdown
NEXUS_CallbackHandler_PrepareCallback
NEXUS_CallbackHandler_SetGuard
 **/
typedef struct NEXUS_CallbackHandler NEXUS_CallbackHandler;

/**
Summary:
Initializes callback handler

Description:
Initializes callback handler with callbadk and context passed to the callback function.
Base will lock the module before calling the callback function.

NEXUS_CallbackHandler_Init should be called once and only once, prior to any other refference to the callback handler.

See Also:
NEXUS_CallbackHandler_Shutdown
NEXUS_CallbackHandler_PrepareCallback
**/
#if NEXUS_P_DEBUG_CALLBACKS 
#define NEXUS_CallbackHandler_Init(h, callback, context) NEXUS_Base_P_CallbackHandler_Init(&(h), NEXUS_MODULE_SELF, callback, context, __FILE__, __LINE__)
#else
#define NEXUS_CallbackHandler_Init(h, callback, context) NEXUS_Base_P_CallbackHandler_Init(&(h), NEXUS_MODULE_SELF, callback, context, NULL, 0)
#endif

/**
Summary:
Releases resources assiciated with the callback handler

Description:

See Also:
NEXUS_CallbackHandler_Init
**/
#define NEXUS_CallbackHandler_Shutdown(h) NEXUS_Base_P_CallbackHandler_Shutdown(&(h))

/**
Summary:
Links NEXUS_CallbackDesc with NEXUS_CallbackHandler

Description:
Initializes NEXUS_CallbackDesc so asynchronous callback would get routed through a base layer, where it serialiazed with the module context and delivered to the user provided callback handler.

See Also:
NEXUS_CallbackHandler_Init
NEXUS_CallbackHandler_Stop
**/
#define NEXUS_CallbackHandler_PrepareCallback(h, _callback) do { BDBG_OBJECT_ASSERT(&(h), NEXUS_CallbackHandler);(_callback).context=&(h);(_callback).callback=NEXUS_Base_P_CallbackHandler_Dispatch;} while(0)



/**
Summary:
Stops deffered callbacks

Description:

See Also:
NEXUS_CallbackHandler_PrepareCallback
**/
#define NEXUS_CallbackHandler_Stop(h) NEXUS_Base_P_CallbackHandler_Stop(&(h))

#define NEXUS_CALLBACK_DEFLECTED    NEXUS_MAKE_ERR_CODE(0x200, 0)

/**
Summary:
Sets guard function

Description:
Guear function could be executed without serializing with the module context, and if returns error code other then NEXUS_SUCCESS
(for example  NEXUS_CALLBACK_DEFLECTED), callback would not get executed. It's important that guard function should not call any blocking
functions.

See Also:
NEXUS_CallbackHandler_Init
**/
#define NEXUS_CallbackHandler_SetGuard(h, guard)    do { BDBG_OBJECT_ASSERT(&(h), NEXUS_CallbackHandler);(h).pCallbackGuard = guard;} while(0)



/* internal function to handle all callback dispatched inside nexus */
void NEXUS_Base_P_CallbackHandler_Init(NEXUS_CallbackHandler *handler, NEXUS_ModuleHandle module, void (*pCallback)(void *), void *pContext,const char *pFileName,unsigned lineNumber);
void NEXUS_Base_P_CallbackHandler_Dispatch(void *context, int arg);
void NEXUS_Base_P_CallbackHandler_Stop(NEXUS_CallbackHandler *handler);
void NEXUS_Base_P_CallbackHandler_Shutdown(NEXUS_CallbackHandler *handler);

BDBG_OBJECT_ID_DECLARE(NEXUS_CallbackHandler); 

struct NEXUS_CallbackHandler {
    const char *pFileName;
    unsigned lineNumber;
    BDBG_OBJECT(NEXUS_CallbackHandler)
    NEXUS_ModuleHandle module; /* module associated with the callback handler */
    NEXUS_Error (*pCallbackGuard)(void *); /* optional callback that is  executed prior to acquiring module lock, it called unsycnrhonized */
    void (*pCallback)(void *); /* Callback Function */
    void *pContext;            /* Context provided to callback */
    NEXUS_TimerHandle timer;
};

#if (NEXUS_BASE_OS_linuxkernel && defined(NEXUS_MODE_driver)) || NEXUS_BASE_MODE_PROXY
/**
nexus/base exposes an external scheduler interface used in linux kernel mode and in the proxy.
See nexus/platform/PLATFORM/src/linuxkernel/nexus_driver_callbacks.c for linux kernel mode usage.
See nexus/platform/PLATFORM/src/linuxuser.proxy/nexus_platform_os.c for linux proxy usage.
**/
#define NEXUS_BASE_EXTERNAL_SCHEDULER 1
#endif

NEXUS_Error NEXUS_P_Base_ExternalScheduler_Step(
    NEXUS_ModulePriority priority,
    unsigned timeout,
    NEXUS_P_Base_Scheduler_Status *status, /* [out] */
    bool (*complete)(void *context),
    void *context
    );

typedef struct NEXUS_Base_Scheduler_Config  {
    const char *name;
    const NEXUS_ThreadSettings *pSettings;
    BKNI_MutexHandle callback_lock; /* callback that is acquired when callback active */
} NEXUS_Base_Scheduler_Config;

void NEXUS_P_Base_GetSchedulerConfig(
    NEXUS_ModulePriority priority,
    NEXUS_Base_Scheduler_Config *config /* [out] */
    );


#endif /* !defined NEXUS_BASE_H */

