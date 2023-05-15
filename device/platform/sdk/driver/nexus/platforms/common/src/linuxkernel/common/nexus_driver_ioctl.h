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
* $brcm_Workfile: nexus_driver_ioctl.h $
* $brcm_Revision: 23 $
* $brcm_Date: 7/9/12 11:30a $
*
* API Description:
*   API name: Platform (private)
*    This file enumerates linuxkernel driver supported on the given platform
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/src/linuxkernel/nexus_driver_ioctl.h $
* 
* 23   7/9/12 11:30a erickson
* SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
* 
* 22   11/8/11 4:13p erickson
* SW7405-5543: refactor callback dequeue to cancel callbacks in flight
*  and to only synchronize with schedulers where callback is current
* 
* 21   8/22/11 2:16p erickson
* SW7420-1148: print error if trusted client terminates abnormally.
*  server can now specify defaultUntrustedHeap.
* 
* 20   8/9/11 12:50p vsilyaev
* SW7420-1465: Improved handshake protocol
* 
* 19   8/4/11 8:02p vsilyaev
* SW7420-1465: Added protocol to ensure that all buffered debug output
*  gets written out
* 
* 18   8/3/11 5:33p vsilyaev
* SW7420-1465, SW7405-5221: Route debug output to the special FIFO
*  instead of syslog buffer
* 
* 17   5/3/11 5:08p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
* 16   4/1/11 9:59a erickson
* SW7420-1671: refactor object database for multiprocess stress
*
* 15   12/22/10 2:28p erickson
* SW7422-2: add support for multiple physical address regions within a
*  single MEMC
*
* 14   12/7/10 5:47p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 13   10/28/10 12:36p erickson
* SW7420-1148: cleanup object database in the proxy layer before calling
*  NEXUS_Platform_Uninit
*
* 12   9/23/10 2:24p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 11   6/10/10 4:10p mphillip
* SW7405-4091: Move nexus_driver 3D hooks to separate file for NFE
*
* 10   8/25/09 12:24p erickson
* SW7420-228: rework platform memory mapping to allow for multiple MEMC's
*  on 7420
*
* PR55501/1   8/14/09 7:37p bandrews
* PR55501: kernel mode support for 7420 with memc1 mappable from user
*
* 9   7/8/09 10:42a jtna
* PR55968: 3d graphics power management
*
* 8   11/3/08 5:42p mphillip
* PR47441: 3D proxy ioctls
*
* 7   9/4/08 10:22a katrep
* PR45527: fixed compilattion issue if NEXUS_CONFIG_IMAGE is not defined
*
* 6   8/21/08 3:17p katrep
* PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
*
* 5   7/31/08 5:19p vsilyaev
* PR 45137: Initial support for multi application mode
*
* 4   6/10/08 3:29p vsilyaev
* PR 43184: Added support for Start/Stop callbacks in linux kernel/user
*  proxy mode
*
* 3   6/5/08 2:19p vsilyaev
* PR 42318: Rearranged scheduller to share the same context in user and
*  kernel mode, this allows to serve callback from kernel to user with
*  single transition kernel<->user
*
* 2   1/30/08 11:49a vsilyaev
* PR 38682: Added routing of callbacks into the user space
*
* 1   1/29/08 5:59p vsilyaev
* PR 38682: ioctl's for internal user/kernel communication
*
*
***************************************************************************/

#ifndef __NEXUS_DRIVER_IOCTL_H_
#define __NEXUS_DRIVER_IOCTL_H_

/* This file cannot #include linux header files. It defines a Broadcom-only API to the driver. */
#include "nexus_memory.h"
#include "nexus_base_ioctl.h"
#ifdef NEXUS_CONFIG_IMAGE
#include "nexus_img_ioctl.h"
#endif
#include "priv/nexus_core_driver.h"

#define PROXY_NEXUS_CALLBACK_PACKET 4

typedef struct nexus_driver_callback_desc {
    NEXUS_CallbackDesc desc;
    void *interfaceHandle; /* interface handle that is used in NEXUS_Start/StopCallbacks */
} nexus_driver_callback_desc;

/* PROXY_NEXUS_Scheduler is used to dequeue, not run */
typedef struct PROXY_NEXUS_Scheduler {
    struct {
        unsigned priority;
    } in;
    struct {
        unsigned count;
        nexus_driver_callback_desc callbacks[PROXY_NEXUS_CALLBACK_PACKET];
    } out;
} PROXY_NEXUS_Scheduler;
#define IOCTL_PROXY_NEXUS_Scheduler NEXUS_IOCTL(101, 1, PROXY_NEXUS_Scheduler)

typedef struct PROXY_NEXUS_GetMemory {
    NEXUS_MemoryDescriptor region[NEXUS_MAX_HEAPS];
} PROXY_NEXUS_GetMemory;
#define IOCTL_PROXY_NEXUS_GetMemory NEXUS_IOCTL(101, 2, PROXY_NEXUS_GetMemory)

typedef struct PROXY_NEXUS_SchedulerLock {
    unsigned priority;
    bool lock;
} PROXY_NEXUS_SchedulerLock;
#define IOCTL_PROXY_NEXUS_SchedulerLock NEXUS_IOCTL(101, 3, PROXY_NEXUS_SchedulerLock)

typedef struct PROXY_NEXUS_AuthenticatedJoin {
    NEXUS_Certificate certificate;
} PROXY_NEXUS_AuthenticatedJoin;
#define IOCTL_PROXY_NEXUS_AuthenticatedJoin NEXUS_IOCTL(101, 4, PROXY_NEXUS_AuthenticatedJoin)

#ifdef NEXUS_CONFIG_IMAGE
#define IOCTL_PROXY_NEXUS_Image NEXUS_IOCTL(101, 5, BIMG_Ioctl)
#endif

#ifdef NEXUS_HAS_3D
#include "nexus_driver_graphics3d.h"
#define IOCTL_PROXY_NEXUS_GetSurfaceOffset NEXUS_IOCTL(101, 6, PROXY_NEXUS_GetSurfaceOffset)
#define IOCTL_PROXY_NEXUS_Hook3dInterrupt NEXUS_IOCTL(101, 7, PROXY_NEXUS_Hook3dInterrupt)
#define IOCTL_PROXY_NEXUS_WaitFor3dInterrupt NEXUS_IOCTL(101, 8, PROXY_NEXUS_WaitFor3dInterrupt)
#define IOCTL_PROXY_NEXUS_Unhook3dInterrupt NEXUS_IOCTL(101, 9, PROXY_NEXUS_Unhook3dInterrupt)
#define IOCTL_PROXY_NEXUS_3dPowerManagement NEXUS_IOCTL(101, 10, PROXY_NEXUS_3dPowerManagement)
#endif /* NEXUS_HAS_3D */

typedef struct PROXY_NEXUS_Log_Instance {
    unsigned instance;
} PROXY_NEXUS_Log_Instance;

typedef struct PROXY_NEXUS_Log_Activate {
    unsigned debug_log_size;
} PROXY_NEXUS_Log_Activate;

typedef struct PROXY_NEXUS_Log_Dequeue {
    PROXY_NEXUS_Log_Instance instance;
    unsigned timeout;
    void *buffer;
    size_t buffer_size;
} PROXY_NEXUS_Log_Dequeue;


#define IOCTL_PROXY_NEXUS_Log_Activate  NEXUS_IOCTL(101, 11, PROXY_NEXUS_Log_Activate)
#define IOCTL_PROXY_NEXUS_Log_Create    NEXUS_IOCTL(101, 12, PROXY_NEXUS_Log_Instance)
#define IOCTL_PROXY_NEXUS_Log_Dequeue   NEXUS_IOCTL(101, 13, PROXY_NEXUS_Log_Dequeue)
#define IOCTL_PROXY_NEXUS_Log_Destroy   NEXUS_IOCTL(101, 14, PROXY_NEXUS_Log_Instance)
#define IOCTL_PROXY_NEXUS_Log_Deactivate    NEXUS_IOCTL(101, 15, int)
#define IOCTL_PROXY_NEXUS_Log_Test      NEXUS_IOCTL(101, 16, int)

/* this PROXY uninit is used to signal clean shutdown. it is different the actual IOCTL_PLATFORM_NEXUS_Platform_Uninit */
#define IOCTL_PROXY_NEXUS_Platform_Uninit   NEXUS_IOCTL(101, 17, int)

/* PROXY_NEXUS_RunScheduler is used to run, not dequeue */
typedef struct PROXY_NEXUS_RunScheduler {
    struct {
        unsigned timeout;
        unsigned priority;
    } in;
    struct {
        bool has_callbacks; /* if true, call IOCTL_PROXY_NEXUS_Scheduler to get them */
    } out;
} PROXY_NEXUS_RunScheduler;
#define IOCTL_PROXY_NEXUS_RunScheduler NEXUS_IOCTL(101, 18, PROXY_NEXUS_RunScheduler)

#define IOCTL_PROXY_NEXUS_StopCallbacks       NEXUS_IOCTL(101, 19, int)
#define IOCTL_PROXY_NEXUS_StartCallbacks      NEXUS_IOCTL(101, 20, int)

#endif /* __NEXUS_DRIVER_IOCTL_H_ */

