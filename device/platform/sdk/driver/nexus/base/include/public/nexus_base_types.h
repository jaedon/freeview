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
* $brcm_Workfile: nexus_base_types.h $
* $brcm_Revision: 14 $
* $brcm_Date: 8/1/12 4:00p $
*
* API Description:
*   API name: Base
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/public/nexus_base_types.h $
* 
* 14   8/1/12 4:00p erickson
* SW7425-2734: allow server to limit resources for untrusted clients
* 
* 13   11/29/11 11:32a erickson
* SW7420-2129: move definition of NEXUS_HeapHandle
* 
* 12   8/2/11 2:56p vsilyaev
* SW7125-1014 : Add additional scheduler threads for active standby
*  modules
* 
* SW7125-1014/1   8/2/11 10:09a gmohile
* SW7125-1014 : Add additional scheduler threads for active standby
*  modules
* 
* 11   1/27/11 4:49p mphillip
* SW7425-63: Update procfs interface to allow larger outputs and to work
*  on newer kernels
* 
* 10   10/4/10 10:27a erickson
* SW7550-587: add NEXUS_TristateEnable
* 
* 9   10/1/10 9:39a erickson
* SW7420-1009: add NEXUS_ANY_ID and NEXUS_NOT_AVAILABLE
*
* 8   9/23/10 2:12p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 7   9/14/10 4:58p erickson
* SWDEPRECATED-3408: remove unnecessary #include bchp.h. if an app
*  requires it, it should #include itself.
*
* 6   9/14/10 10:27a erickson
* SWDEPRECATED-2425: improve comments
*
* 5   10/17/08 11:15a jgarrett
* PR 47994: Adding BCHP for 3548 B0 support
*
* 4   7/14/08 4:07p vsilyaev
* PR 44727: Fixed NEXUS_CALLBACKDESC_INIT
*
* 3   7/1/08 5:30p vsilyaev
* PR 44452: Fixed handling of timeout in NEXUS_P_Scheduler_Step
*
* 2   1/28/08 9:41a erickson
* PR35457: comment update
*
* 1   1/18/08 2:12p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/6   10/29/07 12:22p ptimariu
* PR 34419: removed ';', c++ comp issue
*
* Nexus_Devel/5   10/4/07 12:54p erickson
* PR34419: added standard NEXUS_Error values. added comments.
*
* Nexus_Devel/4   10/3/07 2:51p vsilyaev
* PR 34419: Renamed cntx to context
*
* Nexus_Devel/3   9/17/07 6:46p vsilyaev
* PR 34419: Added second parameter to callback
*
* Nexus_Devel/2   9/13/07 5:20p vsilyaev
* PR 34419: Use plugins for OS
*
* Nexus_Devel/1   9/12/07 1:55p vsilyaev
* PR 34419: Base audio and video types
*
*
***************************************************************************/
#ifndef NEXUS_BASE_TYPES_H
#define NEXUS_BASE_TYPES_H

#include "bstd_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
nexus_base_types.h provides data types and macros for Base.
It does not provide Base API's.
**/

/**
Summary:
NEXUS_Error is the common return type for Nexus Interfaces.
Any non-zero NEXUS_Error value is a failure. Zero is success.

Description:
An Interface may return one of the standard codes defined below.
It may build a custom code using NEXUS_MAKE_ERR_CODE.
It may propogate an internal non-zero error code whose value is undefined.
**/
typedef unsigned NEXUS_Error;

/**
Summary:
Standard Nexus error codes.
**/
#define NEXUS_SUCCESS              0  /* success (always zero) */
#define NEXUS_NOT_INITIALIZED      1  /* parameter not initialized */
#define NEXUS_INVALID_PARAMETER    2  /* parameter is invalid */
#define NEXUS_OUT_OF_SYSTEM_MEMORY 3  /* out of KNI module memory (aka OS memory) */
#define NEXUS_OUT_OF_DEVICE_MEMORY 4  /* out of MEM module memory (ala heap memory) */
#define NEXUS_TIMEOUT              5  /* reached timeout limit */
#define NEXUS_OS_ERROR             6  /* generic OS error */
#define NEXUS_LEAKED_RESOURCE      7  /* resource being freed has attached resources that haven't been freed */
#define NEXUS_NOT_SUPPORTED        8  /* requested feature is not supported */
#define NEXUS_UNKNOWN              9  /* unknown */
#define NEXUS_NOT_AVAILABLE        10 /* no resource available */

/* These are private macros used by the NEXUS ERR macros below. */
#define NEXUS_P_ERR_ID_MASK   UINT32_C(0xFFFF0000)   /* {private} */
#define NEXUS_P_ERR_ID_SHIFT  16                    /* {private} */
#define NEXUS_P_ERR_NUM_MASK  UINT32_C(0x0000FFFF)  /* {private} */

/**
Summary:
Interfaces can use NEXUS_MAKE_ERR_CODE to build custom error codes.
There is no central repository for ID's in the code.
You can grep the nexus header files for NEXUS_MAKE_ERR_CODE to find existing values.
Please talk with the Nexus architecture team before making use of a new ID.
**/
#define NEXUS_MAKE_ERR_CODE(id, num) \
    (((((NEXUS_Error)(id)) << NEXUS_P_ERR_ID_SHIFT) & NEXUS_P_ERR_ID_MASK) | \
     (((NEXUS_Error)(num)) & NEXUS_P_ERR_NUM_MASK))

#define NEXUS_GET_ERR_NUM(code) \
    (((NEXUS_Error)(code)) & NEXUS_P_ERR_NUM_MASK)

#define NEXUS_GET_ERR_ID(code) \
    ((((NEXUS_Error)(code)) & NEXUS_P_ERR_ID_MASK) >> NEXUS_P_ERR_ID_SHIFT)


/*
Summary:
Every Nexus module is identified by one handle.

Description:
This handle is access by the Platform or application.
It is not used in the module's Interfaces.
*/
typedef struct NEXUS_Module *NEXUS_ModuleHandle;

/**
Summary:
Function prototype used by NEXUS_CallbackDesc.
**/
typedef void (*NEXUS_Callback)(void *context, int param);

/*
Summary:
Standard definition of a callback in Nexus.
*/
typedef struct NEXUS_CallbackDesc {
    NEXUS_Callback callback; /* Function pointer */
    void *context;           /* First parameter to callback function. */
    int param;               /* Second parameter to callback function. */
} NEXUS_CallbackDesc;

#define NEXUS_CALLBACKDESC_INITIALIZER() {NULL, NULL, 0}
#define NEXUS_CALLBACKDESC_INIT(cb) do {(cb)->callback = NULL;(cb)->context=NULL;(cb)->param=0;}while(0)

/**
Summary:
Indicates the priority in which events, timers and callbacks from this module will be serviced relative to other modules.

Description:
Inside base, separate threads are created for each priority level.
Call events, timers and callbacks for a given priority will be serviced in FIFO order.
Specifying the correct priority is critical to avoid having a fast interface waiting on a slow interface.

Module priorities do not necessarily map to thread priorities. See Nexus_Architecture for a discussion of thread priorities in Nexus Base.

See Also:
NEXUS_ModuleSettings
**/
typedef enum NEXUS_ModulePriority
{
    NEXUS_ModulePriority_eIdle,    /* Very low priority. This should be used by modules with blocking functions or any work that has minimal performance requirements. */
    NEXUS_ModulePriority_eLow,     /* Low priority. This should be used by slow-executing modules. */
    NEXUS_ModulePriority_eDefault, /* Medium priority. This is used by most modules. */
    NEXUS_ModulePriority_eHigh,    /* The highest priority. This should only be used by fast-executing modules (no slow functions) which require it. */
    NEXUS_ModulePriority_eIdleActiveStandby,    /* Very low priority. This should be used by modules with blocking functions or any work that has minimal performance requirements, but remain to be active during active standby */
    NEXUS_ModulePriority_eLowActiveStandby,     /* Low priority. This should be used by slow-executing modules, but remain to be active during active standby */
    NEXUS_ModulePriority_eHighActiveStandby,    /* The highest priority. This should only be used by fast-executing modules (no slow functions) which require it, and remain active during active standby */
    NEXUS_ModulePriority_eMax
} NEXUS_ModulePriority;

/**
Summary:
Settings used to create a Nexus module

Description:

See Also:
NEXUS_Module_GetDefaultSettings
NEXUS_Module_Create
**/
typedef struct NEXUS_ModuleSettings
{
    NEXUS_ModulePriority priority;  /* Default=NEXUS_ModulePriority_Default */    
    void (*dbgPrint)(void);         /* optional pointer debug callback, this callback should not directly or indirectly call into the nexus Base module */
    const char *dbgModules;         /* optional comma-delimited list of dbg_modules that would produce the debug output */
    int dbgPrintSize;               /* optional size for the print buffer if it will exceed a default page */
} NEXUS_ModuleSettings;

/**
Summary:
General purpose ID used for dynamic allocation of resources.

Description:
For instance, NEXUS_Playpump_Open(NEXUS_ANY_ID, NULL) will open any available playpump index.
To learn what index was opened, use the interface's GetStatus function.
**/
#define NEXUS_ANY_ID 0xFEFEFEFE

/**
Summary:
Standard definition of a tristate in Nexus.
**/
typedef enum NEXUS_TristateEnable {
    NEXUS_TristateEnable_eDisable, /* backward compat with boolean false */
    NEXUS_TristateEnable_eEnable,  /* backward compat with boolean true */
    NEXUS_TristateEnable_eNotSet,  /* this is the tristate */
    NEXUS_TristateEnable_eMax
} NEXUS_TristateEnable;

/***************************************************************************
Summary:
Heap handle

Description:
This is used to manage custom memory configurations.
A NEXUS_HeapHandle is created by specifying custom heap options in NEXUS_PlatformSettings.
***************************************************************************/
typedef struct NEXUS_HeapImpl *NEXUS_HeapHandle;

/***************************************************************************
Summary:
NEXUS_ClientResources allows server to limit resources available to untrusted clients

Resources are set by the server in two ways:
1. using NEXUS_ClientSettings.configuration when calling NEXUS_Platform_RegisterClient
2. using NEXUS_PlatformStartServerSettings.unauthenticatedConfiguration when calling NEXUS_Platform_StartServer

Resources are enforced in each module using nexus_client_resources.h macros

Resources can be read by client using NEXUS_Platform_GetClientConfiguration
***************************************************************************/
#define NEXUS_MAX_IDS 16
typedef struct NEXUS_ClientResourceIdList
{
    unsigned id[NEXUS_MAX_IDS]; /* packed array of 'total' elements. */
    unsigned total; /* count of elements in id[] */
} NEXUS_ClientResourceIdList;
typedef struct NEXUS_ClientResourceCount
{
    unsigned total; /* count of resources */
} NEXUS_ClientResourceCount;

typedef struct NEXUS_ClientResources
{
    NEXUS_ClientResourceIdList simpleAudioDecoder;
    NEXUS_ClientResourceIdList simpleVideoDecoder;
    NEXUS_ClientResourceIdList surfaceClient;
    
    NEXUS_ClientResourceCount dma;
    NEXUS_ClientResourceCount graphics2d;
    NEXUS_ClientResourceCount graphics3d;
    NEXUS_ClientResourceCount pictureDecoder;
    NEXUS_ClientResourceCount playpump;
    NEXUS_ClientResourceCount recpump;
    NEXUS_ClientResourceCount simpleAudioPlayback;
    NEXUS_ClientResourceCount stcChannel;
    NEXUS_ClientResourceCount surface;
} NEXUS_ClientResources;

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_BASE_TYPES_H */

