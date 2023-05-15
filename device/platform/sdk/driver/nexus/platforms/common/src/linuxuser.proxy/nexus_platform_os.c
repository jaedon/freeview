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
* $brcm_Workfile: nexus_platform_os.c $
* $brcm_Revision: 86 $
* $brcm_Date: 10/29/12 3:33p $
*
* API Description:
*   API name: Platform
*    Specific version of APIs to communicate with the driver over the proxy layer
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/linuxuser.proxy/nexus_platform_os.c $
* 
* 86   10/29/12 3:33p jgarrett
* SW7425-4157: Adding audio_logs_enabled=y
* 
* 85   9/4/12 4:03p erickson
* SW7420-1123: refactor common, local code for linuxuser.client and
*  linuxuser.proxy
* 
* 84   8/9/12 4:39p erickson
* SW7425-3736: check rc from BKNI_CreateMutex
* 
* 83   7/27/12 2:14p jgarrett
* SW7425-3281: Merge to main branch
* 
* SW7425-3281/3   7/18/12 9:40a jgarrett
* SW7425-3281: Merging platform changes for 7231 release
* 
* 82   7/9/12 11:29a erickson
* SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
* 
* 81   6/20/12 2:36p davidp
* SW7435-164: Remove un-needed private xvd header file include.
* 
* SW7425-3281/2   7/10/12 2:20p piyushg
* SW7425-3281: Kernel mode wmdrmpd module INIT
* 
* SW7425-3281/1   6/20/12 6:07p piyushg
* SW7425-3281: Add "wmdrmpd", "wmdrmpd_core" and "wmdrmpd_io" modules to
*  nexus platform.
* 
* 80   3/26/12 3:57p gmohile
* SW7425-1473 : FIx kernel mode build for raaga encoder
* 
* 79   1/4/12 11:49a erickson
* SW7420-1690: use chmod instead of umask to avoid race conditions in
*  multithreaded system
* 
* 78   11/8/11 6:06p jgarrett
* SW7422-400: Adding audio debug log support in kernel mode
* 
* 77   11/8/11 4:13p erickson
* SW7405-5543: refactor callback dequeue to cancel callbacks in flight
*  and to only synchronize with schedulers where callback is current
*
* 76   10/27/11 4:08p erickson
* SW7420-1148: allow non-root access to /dev/nexus* nodes
*
* 75   10/25/11 2:23p erickson
* SW7420-2107: clean up fully after Uninit, unify more server/client
*  init/uninit logic
*
* 74   9/13/11 1:23p erickson
* SW7420-1148: change client modes to unprotected/protected/untrusted
*
* 73   9/6/11 10:05a erickson
* SWNOOS-482: add NEXUS_FileModuleSettings.schedulerSettings[] per-thread
*  array
*
* 72   8/25/11 10:54a erickson
* SW7420-1148: default server to using untrusted clients
*
* 71   8/22/11 2:16p erickson
* SW7420-1148: print error if trusted client terminates abnormally.
*  server can now specify defaultUntrustedHeap.
*
* 70   8/10/11 4:09p erickson
* SW7420-1989: allow Join to succeed even after
*  NEXUS_Platform_GetDefaultSettings. it will be messy, but it should not
*  fail.
*
* 69   8/9/11 12:50p vsilyaev
* SW7420-1465: Use smaller buffer size for the kernel
*
* 68   8/8/11 5:01p erickson
* SW7420-1465: fix NEXUS_Platform_Join
*
* 67   8/4/11 8:02p vsilyaev
* SW7420-1465: Added protocol to ensure that all buffered debug output
*  gets written out
*
* 66   8/3/11 5:33p vsilyaev
* SW7420-1465, SW7405-5221: Route debug output to the special FIFO
*  instead of syslog buffer
*
* 65   8/2/11 1:48p erickson
* SW7125-136: add #ifndef NEXUS_SECURITY_VIDEO_VERIFICATION_LEGACY_65NM
*  for XVD
*
* 64   7/1/11 11:20a erickson
* SW7346-187: consolidate code for MIPS kernel mode test. make
*  nexus_map.c generic.
*
* 63   5/31/11 4:33p erickson
* SW7420-1907: if NEXUS_Platform_GetDefaultSettings fails, memset out
*  param to zero to avoid app access to uninitialized memory
*
* 62   5/17/11 4:31p katrep
* SW7425-394:pull correct file module default settings
*
* 61   4/20/11 7:48p vsilyaev
* SW7425-394: Allow application to set number of I/O worker threads that
*  are serving I/O requests
*
* 60   4/15/11 6:05p vsilyaev
* SW7425-232: Initialize FileMux module in the proxy side
*
* 59   4/14/11 3:08p erickson
* SW7420-1671: catch bad init or uninit
*
* 58   4/8/11 11:02a erickson
* SW7420-1671: init fds to -1, only close if != -1
*
* 57   4/7/11 5:26p erickson
* SW7420-1671: improve error recovery. remove sleep.
*
* 56   4/5/11 9:56a erickson
* SW7405-5221: Added option to forward debug output to FIFO
*
* 55   4/1/11 9:59a erickson
* SW7420-1671: refactor object database for multiprocess stress
*
* 54   3/24/11 11:48a jgarrett
* SW7422-146: Fixing kernel mode builds
*
* 53   3/22/11 1:31p erickson
* SW7405-4011: fix callbackLock support for multiprocess
*
* 52   3/18/11 12:35p erickson
* SW7405-4011: synchronize with all callback schedulers inside
*  NEXUS_P_Proxy_StopCallbacks to avoid race conditions
*
* 51   3/17/11 5:32p erickson
* SW7400-3006: add NEXUS_PlatformCreateHeapSettings.userAddress support
*  to linuxuser.proxy
*
* 50   3/14/11 1:51p vsilyaev
* SW7425-167: Improved debugabily of the shutdown sequence
*
* 49   3/11/11 2:20p erickson
* SW7400-3006: add client-side support for NEXUS_Platform_CreateHeap
*
* 48   3/2/11 10:46a erickson
* SW7422-310: dynamic fake address mapping scheme using nexus_map.c
*
* 47   2/11/11 8:39p vsilyaev
* SW7425-81: Added image interface plumbing to the video encoder module
*
* 46   12/22/10 2:28p erickson
* SW7422-2: add support for multiple physical address regions within a
*  single MEMC
*
* 45   12/9/10 5:12p erickson
* SW7420-1148: client uses /dev/platform to mmap so that root access is
*  not required
*
* 44   12/7/10 5:47p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 43   10/28/10 6:56p mward
* SW7420-1123: Correct NEXUS_Platform_P_Magnum_Init() should be if ...
*  false.
*
* 42   10/28/10 12:36p erickson
* SW7420-1148: cleanup object database in the proxy layer before calling
*  NEXUS_Platform_Uninit
*
* 41   10/12/10 11:27a erickson
* SW7420-1148: fix NEXUS_Platform_Uninit for clients
*
* 40   10/11/10 10:43a mward
* SW7420-1123: NEXUS_Platform_P_Magnum_Init() can't be static.  Check for
*  already initialized, don't init again.
*
* 39   9/23/10 2:24p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 38   8/21/10 4:45p ttrammel
* SW7420-974: Fix errant error code in mmap message when call suceeds.
*
* 37   3/19/10 10:12a erickson
* SW7405-3832: create standard method for specific
*  NEXUS_PLATFORM_VERSION. use tagged NEXUS_Platform_Init to enforce
*  binary compatibility. consolidate NEXUS_Platform_GetReleaseVersion
*  using standard macros.
*
* 36   2/11/10 2:44p jgarrett
* SW7408-71: Adding IMG support for 7408
*
* 36   2/11/10 2:42p jgarrett
* SW7408-71: Adding IMG support for APE
*
* 35   2/4/10 4:47p jrubio
* SW7420-532: add InitStandby and UninitStanbdy functions for KernelMode
*
* 34   12/15/09 2:12p vsilyaev
* SW7405-2875: Added back support for nexus schedullers (timeouts and
*  such) in the slave proccess
*
* 33   11/24/09 12:25p erickson
* SW7405-3439: rename nexus/base external scheduler interface functions
*
* 32   11/13/09 5:00p erickson
* SW7325-250: add nexus_platform_standby support
*
* 31   8/25/09 1:35p erickson
* SW7420-228: not every MEMC must be mmapped
*
* 30   8/25/09 12:24p erickson
* SW7420-228: rework platform memory mapping to allow for multiple MEMC's
*  on 7420
*
* PR55501/1   8/14/09 7:37p bandrews
* PR55501: kernel mode support for 7420 with memc1 mappable from user
*
* 28   3/3/09 1:57p katrep
* PR52516: fixed kernelmode build error with latest XVD
*
* 27   2/25/09 10:59a erickson
* PR52454: add missing munmap
*
* 26   2/6/09 10:23a erickson
* PR51872: fix warning
*
* 25   1/28/09 6:27p vsilyaev
* PR 51392: Moved magnum initialization from Nexus base to the platform.
*  Initialize only magnum layer inside of
*  NEXUS_Platform_GetDefaultSettings
*
* 24   11/5/08 5:33p katrep
* PR47655: Fixed nexus lockup in certain scenarios
*
* 23   11/4/08 3:57p mphillip
* PR47441: Expose the proxy ioctl for the 3d proxy
*
* 22   9/12/08 3:01p erickson
* PR45527: fix warning
*
* 21   8/21/08 5:12p vsilyaev
* PR 45788: Fixed condition in ASSERT
*
* 20   8/21/08 3:17p katrep
* PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
*
* 19   8/7/08 4:51p vsilyaev
* PR 45137: Added slave mode support to the SettopApi shim
*
* 18   8/1/08 5:43p vsilyaev
* PR 45137: Improved multi application support
*
* 17   7/31/08 5:19p vsilyaev
* PR 45137: Initial support for multi application mode
*
* 16   7/23/08 1:54p erickson
* PR44598: NEXUS_Platform_GetDefaultSettings must do a partial bringup of
*  the driver so that it can be proxied.
*
* 15   7/1/08 3:45p vsilyaev
* PR 44452: Don't maintain local copy of
*  NEXUS_Platform_GetDefaultSettings
*
* 14   6/10/08 3:29p vsilyaev
* PR 43184: Added support for Start/Stop callbacks in linux kernel/user
*  proxy mode
*
* 13   6/9/08 7:11p vsilyaev
* PR 43184: Changed phase of detection for outstanding callbacks
*
* 12   6/5/08 2:19p vsilyaev
* PR 42318: Rearranged scheduller to share the same context in user and
*  kernel mode, this allows to serve callback from kernel to user with
*  single transition kernel<->user
*
* 11   2/8/08 6:46p vsilyaev
* PR 38682: Added support for 97405 platform
*
* 10   2/8/08 10:07a vsilyaev
* PR 38682: Removed not needed debug statements
*
* 9   2/7/08 11:07p vsilyaev
* PR 38682: Added linux kernel/proxy support to the 93563 platform
*
* 8   2/7/08 11:17a vsilyaev
* PR 38682: Added syslog/message routing
*
* 7   1/30/08 4:28p vsilyaev
* PR 38682: Improved handling of callbacks traversing user/kernel
*  boundary
*
* 6   1/30/08 11:49a vsilyaev
* PR 38682: Added routing of callbacks into the user space
*
* 5   1/29/08 5:54p vsilyaev
* PR 38682: Support of callbacks in the linux kernel/user proxy
*  environment
*
* 4   1/28/08 12:55p vsilyaev
* PR 38682: Don't create device nodes if they exist
*
* 3   1/25/08 2:35p vsilyaev
* PR 38682: Added handling of 'attr' to the function arguments
*
* 2   1/24/08 4:25p vsilyaev
* PR 38682: Linuxuser version of proxied platform
*
* 1   1/24/08 12:33p vsilyaev
* PR 38682: Proxy version of nexus_platform
*
***************************************************************************/

#include "nexus_types.h"
#include "nexus_base.h"
#include "nexus_base_os_types.h"
#include "nexus_platform.h"
#include "nexus_platform_standby.h"
#include "nexus_platform_priv_proxy.h"
#if NEXUS_HAS_FILE
#include "nexus_file_init.h"
#endif

#include "nexus_platform_audio_log.h"
#include "blst_list.h"

#include <string.h>
/* stdio is needed to print a failure message when KNI is not started */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/mman.h>
#include <linux/kdev_t.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "nexus_driver_ioctl.h"
#include "nexus_map.h"
#include "nexus_platform_debug_log.h"

#if NEXUS_CONFIG_IMAGE
#include "priv/nexus_core_img_id.h"
#include "bxvd_image.h"
#if BCHP_CHIP == 7408
#include "bape_img.h"
#elif defined BDSP_RAAGA_SUPPORT
#include "bdsp_raaga_img.h"
#else
#include "brap_img.h"
#endif
#include "nexus_platform_image.h"
#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
#include "bvce_image.h"
#endif
#endif /* NEXUS_CONFIG_IMAGE */

BDBG_MODULE(nexus_platform_os);

/* define prototypes for init/uninit functions */
#define NEXUS_PLATFORM_P_DRIVER_MODULE(module) \
    BERR_Code nexus_proxy_##module##_init(int fd); \
    void nexus_proxy_##module##_uninit(void);
#include "nexus_driver_modules.h"
#undef NEXUS_PLATFORM_P_DRIVER_MODULE

static NEXUS_Error NEXUS_Platform_P_InitProxy(int fd);
static void NEXUS_Platform_P_UninitProxy(void);
static void NEXUS_Platform_P_SchedulerThread(void *scheduler_);
static void NEXUS_Platform_P_SchedulerSlaveThread(void *scheduler_);
static void NEXUS_Platform_P_UninitOS(void);

static const struct {
    const char *name;
    NEXUS_Error (*init)(int fd);
    void (*uninit)(void);
} g_nexus_proxy_handlers[] = {
#define NEXUS_PLATFORM_P_DRIVER_MODULE(module) {#module, nexus_proxy_##module##_init, nexus_proxy_##module##_uninit},
#include "nexus_driver_modules.h"
#undef NEXUS_PLATFORM_P_DRIVER_MODULE
    {"proxy", NEXUS_Platform_P_InitProxy, NEXUS_Platform_P_UninitProxy} /* special entry for callback interface */
};
#define NEXUS_PLATFORM_P_NUM_DRIVERS  (sizeof(g_nexus_proxy_handlers)/sizeof(*g_nexus_proxy_handlers))

struct NEXUS_Platform_P_ProxyScheduler {
    NEXUS_ThreadHandle thread;
    NEXUS_ModulePriority priority;
    BKNI_MutexHandle callbackLock; /* held while callback is running */

    BKNI_MutexHandle dataLock; /* held when accessing data.out or current_callback */
    PROXY_NEXUS_Scheduler data;
    void *current_callback;
};

typedef struct nexus_proxy_stopped_callback_entry {
    BLST_D_ENTRY(nexus_proxy_stopped_callback_entry) list;
    void *interfaceHandle;
} nexus_proxy_stopped_callback_entry;

static struct NEXUS_Platform_P_State
{
    NEXUS_ModuleHandle module;
    int mem_fd;
    struct {
        NEXUS_HeapHandle heap;
        void *addr;
        unsigned long length;
        bool dynamic;
    } mmaps[NEXUS_MAX_HEAPS];
    int proxy_fd;
    bool stop;
    bool init; /* NEXUS_P_Init called */
    bool platform_init; /* NEXUS_Platform_Init called */
    bool slave;
    struct NEXUS_Platform_P_ProxyScheduler schedulers[NEXUS_ModulePriority_eMax];
    int fds[NEXUS_PLATFORM_P_NUM_DRIVERS];
    BLST_D_HEAD(nexus_proxy_stopped_callback_list, nexus_proxy_stopped_callback_entry) stopped_callbacks;
    NEXUS_ClientConfiguration clientConfig;
    NEXUS_Platform_P_DebugLog debugLog;
} NEXUS_Platform_P_State;

int proxy_ioctl_fd = -1;

#define NEXUS_MODULE_SELF NEXUS_Platform_P_State.module

void NEXUS_Platform_GetDefaultSettings_proxy(NEXUS_PlatformSettings *pSettings);
NEXUS_Error NEXUS_Platform_Init_tagged_proxy(const NEXUS_PlatformSettings *pSettings, unsigned platformCheck, unsigned versionCheck, unsigned structSizeCheck);
void NEXUS_Platform_Uninit_proxy(void);
NEXUS_Error NEXUS_Platform_InitStandby_proxy(const NEXUS_PlatformStandbySettings *pSettings);
void NEXUS_Platform_UninitStandby_proxy(void);

static void *NEXUS_Platform_P_MapMemory( unsigned long offset, unsigned long length, bool cached )
{
    if (!cached) return NULL; /* only cached mmap allows in proxy */
    return mmap(0, length, PROT_READ|PROT_WRITE, MAP_SHARED, NEXUS_Platform_P_State.mem_fd, offset);
}

static void NEXUS_Platform_P_UnmapMemory( void *addr, unsigned long length )
{
    munmap(addr, length);
}

static NEXUS_Error nexus_p_add_heap(unsigned i, NEXUS_HeapHandle heap, unsigned base, unsigned length, NEXUS_MemoryType memoryType, bool dynamic, void *user_address)
{
    void *addr;

    if (user_address) {
        addr = user_address;
    }
    else {
        addr = nexus_p_map_memory(base, length, (memoryType & NEXUS_MemoryType_eApplication)?NEXUS_MemoryMapType_eCached:NEXUS_MemoryMapType_eFake);
        BDBG_MSG(("mmap offset=%x=>%x size=%d fd=%d errno=%d", base, addr, length, NEXUS_Platform_P_State.mem_fd, (addr==MAP_FAILED) ? errno : 0));
        if (addr==MAP_FAILED) return BERR_TRACE(BERR_OS_ERROR);
    }

    NEXUS_P_AddMap(base, addr, addr, length);

    NEXUS_Platform_P_State.mmaps[i].heap = heap;
    NEXUS_Platform_P_State.mmaps[i].addr = addr;
    NEXUS_Platform_P_State.mmaps[i].length = length;
    NEXUS_Platform_P_State.mmaps[i].dynamic = dynamic;
    return 0;
}

static void nexus_p_remove_heap(unsigned i)
{
    if (NEXUS_Platform_P_State.mmaps[i].heap) {
        nexus_p_unmap_memory(NEXUS_Platform_P_State.mmaps[i].addr, NEXUS_Platform_P_State.mmaps[i].length);
        NEXUS_Platform_P_State.mmaps[i].heap = NULL;
    }
}

static NEXUS_Error
NEXUS_Platform_P_InitOS(void)
{
    NEXUS_Error rc;
    PROXY_NEXUS_GetMemory mem;
    int mem_fd;
    unsigned i;
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;
    struct nexus_map_settings map_settings;

    if(!state->slave) {
        for(i=0;i<NEXUS_ModulePriority_eMax;i++) {
            NEXUS_Base_Scheduler_Config config;

            BDBG_MSG((">CALLBACK[%u]", i));
            rc = BKNI_CreateMutex(&state->schedulers[i].callbackLock);
            if (rc) {rc = BERR_TRACE(rc); goto err_create_mutex;}
            rc = BKNI_CreateMutex(&state->schedulers[i].dataLock);
            if (rc) {rc = BERR_TRACE(rc); goto err_create_mutex;}

            NEXUS_P_Base_GetSchedulerConfig(i, &config);
            state->schedulers[i].priority = i;
            state->schedulers[i].thread = NEXUS_Thread_Create(config.name, NEXUS_Platform_P_SchedulerThread, &state->schedulers[i], config.pSettings);
            if(!state->schedulers[i].thread) { rc = BERR_TRACE(NEXUS_OS_ERROR); goto err_threads; }
        }
    }
    else {
        /* only create one */
        state->schedulers[0].priority = 0;
        rc = BKNI_CreateMutex(&state->schedulers[0].callbackLock);
        if (rc) {rc = BERR_TRACE(rc); goto err_create_mutex;}
        rc = BKNI_CreateMutex(&state->schedulers[0].dataLock);
        if (rc) {rc = BERR_TRACE(rc); goto err_create_mutex;}
        state->schedulers[0].thread = NEXUS_Thread_Create("slave_callback", NEXUS_Platform_P_SchedulerSlaveThread, &state->schedulers[0], NULL);
        if (!state->schedulers[0].thread) { rc = BERR_TRACE(NEXUS_OS_ERROR); goto err_threads; }

        BDBG_ASSERT(!state->schedulers[1].callbackLock); /* be sure no state leaked over from previous Init */
    }

    NEXUS_Platform_GetClientConfiguration(&state->clientConfig);

    BDBG_MSG((">IOCTL_PROXY_NEXUS_GetMemory"));
    rc = ioctl(state->proxy_fd, IOCTL_PROXY_NEXUS_GetMemory, &mem);
    if (rc) {rc=BERR_TRACE(rc);goto err_get_memory;}

    mem_fd = open("/dev/nexus_platform", O_RDWR);
    if ( mem_fd < 0 ) { rc = BERR_TRACE(BERR_OS_ERROR); goto err_mem_open; }
    fcntl(mem_fd, FD_CLOEXEC, 1);
    state->mem_fd = mem_fd;

    nexus_p_get_default_map_settings(&map_settings);
    /* see nexus_platform_core.c for fake address range logic */
    map_settings.offset = 0x80000000;
    map_settings.size = 0x80000000 - 4096;
    map_settings.mmap = NEXUS_Platform_P_MapMemory;
    map_settings.munmap = NEXUS_Platform_P_UnmapMemory;
    rc = nexus_p_init_map(&map_settings);
    if (rc) {rc = BERR_TRACE(rc); goto err_init_map;}

    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        BDBG_MSG(("mem.region[%d]: %#x %#x %d", i, mem.region[i].base, mem.region[i].length, mem.region[i].memoryType));
        if (mem.region[i].length) {
            rc = nexus_p_add_heap(i, (NEXUS_HeapHandle)-1, mem.region[i].base, mem.region[i].length, mem.region[i].memoryType, false, NULL);
            if (rc) { rc = BERR_TRACE(BERR_OS_ERROR); goto err_mmap; }
        }
    }

    BLST_D_INIT(&state->stopped_callbacks);

    return NEXUS_SUCCESS;

err_mmap:
    NEXUS_Platform_P_UninitOS();
err_init_map:
    close(mem_fd);
err_mem_open:
err_get_memory:
err_threads:
err_create_mutex:
    state->stop = true;
    for(i=0;i<NEXUS_ModulePriority_eMax;i++) {
        if (state->schedulers[i].thread) {
            NEXUS_Thread_Destroy(state->schedulers[i].thread);
        }
        if (state->schedulers[i].callbackLock) {
            BKNI_DestroyMutex(state->schedulers[i].callbackLock);
            BKNI_DestroyMutex(state->schedulers[i].dataLock);
        }
    }
    return rc;
}

static void
NEXUS_Platform_P_UninitOS(void)
{
    unsigned i;
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;
    nexus_proxy_stopped_callback_entry *entry;

    while(NULL!=(entry=BLST_D_FIRST(&state->stopped_callbacks))) {
        BLST_D_REMOVE_HEAD(&state->stopped_callbacks, list);
        BKNI_Free(entry);
    }

    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        nexus_p_remove_heap(i);
    }
    close(state->mem_fd);
    nexus_p_uninit_map();

    state->stop = true;
    for(i=0;i<NEXUS_ModulePriority_eMax;i++) {
        if (state->schedulers[i].thread) {
            NEXUS_Thread_Destroy(state->schedulers[i].thread);
        }
        if (state->schedulers[i].callbackLock) {
            BKNI_DestroyMutex(state->schedulers[i].callbackLock);
            BKNI_DestroyMutex(state->schedulers[i].dataLock);
        }
    }
}

static NEXUS_Error
NEXUS_Platform_P_InitOneDriver(unsigned driver)
{
    NEXUS_Error result;
    char devName[64];
    int rc;
    struct stat st;
    bool create_node = true;

    if(driver >= NEXUS_PLATFORM_P_NUM_DRIVERS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BKNI_Snprintf(devName, sizeof(devName), "/dev/nexus/nexus_%s", g_nexus_proxy_handlers[driver].name);
#else
    BKNI_Snprintf(devName, sizeof(devName), "/dev/nexus_%s", g_nexus_proxy_handlers[driver].name);
#endif
    rc = stat(devName, &st);
    if(rc==0) {
        if(S_ISCHR(st.st_mode) && MAJOR(st.st_rdev) == NEXUS_PLATFORM_P_DRIVER_MAJOR && MINOR(st.st_rdev) == driver) {
            create_node = false;
        } else {
            BDBG_MSG(("Removing stale device node %s (%#x major=%u(%u) minor=%u(%u))", devName, (unsigned)st.st_mode, (unsigned)MAJOR(st.st_rdev), NEXUS_PLATFORM_P_DRIVER_MAJOR, (unsigned)MINOR(st.st_rdev), driver));
            /* coverity[toctou] - it's ok to use the filename after the stat() check */
            rc = unlink(devName);
            if(rc!=0) {
                BDBG_ERR(("unlink:'%s' failed", devName));
                return BERR_TRACE(NEXUS_OS_ERROR);
            }
        }
    }
    if(create_node) {
        BDBG_MSG(("Making device node %s (major=%u minor=%u)", devName,  NEXUS_PLATFORM_P_DRIVER_MAJOR, driver));
        /* coverity[toctou] - it's ok to use the filename after the stat() check */
        rc = mknod(devName, 0666 | S_IFCHR, makedev(NEXUS_PLATFORM_P_DRIVER_MAJOR, driver));
        if(rc!=0) {
            BDBG_ERR(("mknode:'%s' failed", devName));
            return BERR_TRACE(NEXUS_OS_ERROR);
        }
        /* allow non-root access */
        chmod(devName, 0666);
    }
    /* coverity[toctou] - it's ok to use the filename after the stat() check */
    rc = open(devName, O_RDWR);
    if(rc<0) {
        /* if client starts before server, it will fail here */
        BDBG_ERR(("can't access device '%s'", devName));
        return BERR_TRACE(NEXUS_OS_ERROR);
    }
    fcntl(rc, FD_CLOEXEC, 1);
    result = g_nexus_proxy_handlers[driver].init(rc);
    if(result!=NEXUS_SUCCESS) {
        close(rc); /* close opened device */
        return BERR_TRACE(result);
    }
    NEXUS_Platform_P_State.fds[driver] = rc;
    return NEXUS_SUCCESS;
}

/* startingIndex allows the platform driver to be brought up first, followed by all the other drivers */
static NEXUS_Error
NEXUS_Platform_P_InitDriver(void)
{
    unsigned i;

    for(i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS;i++) {
        if (NEXUS_Platform_P_State.fds[i] == -1) {
            NEXUS_Error result = NEXUS_Platform_P_InitOneDriver(i);
            if(result!=NEXUS_SUCCESS) {
                return BERR_TRACE(result);
            }
        }
    }
    return NEXUS_SUCCESS;
}

static void
NEXUS_Platform_P_UninitDriver(void)
{
    unsigned i;
    for(i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS;i++) {
        if (NEXUS_Platform_P_State.fds[i] != -1) {
            g_nexus_proxy_handlers[i].uninit();
            close(NEXUS_Platform_P_State.fds[i]);
            NEXUS_Platform_P_State.fds[i]=-1;
        }
    }
    return;
}

/* bring up the proxy base and the driver.
NEXUS_P_Init is only called by the server, not the client. */
static NEXUS_Error NEXUS_P_Init(void)
{
    NEXUS_Error errCode;
    NEXUS_ModuleHandle proxy;
    unsigned i;
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;
    char device_fd[8];

    BDBG_ASSERT(state->init == false);
    BKNI_Memset(state, 0, sizeof(*state));
    for(i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS;i++) {
        state->fds[i] = -1; /* unopened == -1, not 0 */
    }

    errCode = NEXUS_Platform_P_Magnum_Init();
    if( errCode != BERR_SUCCESS)  { errCode = BERR_TRACE(errCode); goto err_magnum; }

    /* Now, proceed to boot the board. -- Always initialize base first */
    errCode = NEXUS_Base_Init(NULL);
    if ( errCode!=BERR_SUCCESS ) { errCode = BERR_TRACE(errCode); goto err_base; }


    BDBG_MSG((">MODULE"));
    proxy = NEXUS_Module_Create("proxy", NULL);
    if ( !proxy) { errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_proxy; }
    state->module = proxy;

    BDBG_MSG((">PLATFORM DRIVER"));
    errCode = NEXUS_Platform_P_InitOneDriver(0);
    if ( errCode!=BERR_SUCCESS ) { errCode = BERR_TRACE(errCode); goto err_driver; }

    /* register the proxy driver first*/
    BDBG_MSG((">PROXY DRIVER"));
    errCode = NEXUS_Platform_P_InitOneDriver( NEXUS_PLATFORM_P_NUM_DRIVERS-1);
    if ( errCode!=BERR_SUCCESS ) { errCode = BERR_TRACE(errCode); goto err_driver; }


    BDBG_MSG((">DEBUG_LOG"));
    BDBG_ASSERT(state->proxy_fd>0);
    BKNI_Snprintf(device_fd, sizeof(device_fd), "%d", state->proxy_fd);
    fcntl(state->proxy_fd, FD_CLOEXEC, 0);
    NEXUS_Platform_P_DebugLog_Init(&state->debugLog, device_fd);
    fcntl(state->proxy_fd, FD_CLOEXEC, 1);
    if(state->debugLog.logWriter) {
        int urc;
        const char *debug_log_size;

        PROXY_NEXUS_Log_Activate activate;
        activate.debug_log_size = state->debugLog.logSize;
        if(activate.debug_log_size>256) {
            activate.debug_log_size /= 4;
        }
        debug_log_size = NEXUS_GetEnv("kernel_debug_log_size");
        if(debug_log_size) {
            activate.debug_log_size = 2*NEXUS_atoi(debug_log_size);
        }
        urc = ioctl(state->proxy_fd, IOCTL_PROXY_NEXUS_Log_Activate, &activate);
    }

    state->init = true;
    return 0;

err_driver:
    NEXUS_Module_Destroy(proxy);
err_proxy:
    NEXUS_Base_Uninit();
err_base:
    NEXUS_Platform_P_Magnum_Uninit();
err_magnum:
    BDBG_ASSERT(errCode); /* if we've taken this path, it's only because of failure */
    return errCode;
}

/* NEXUS_P_Uninit is called by the server and the client. */
static void NEXUS_P_Uninit(void)
{
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;

    if (!state->slave) {
        int urc;
        BDBG_MSG(("<DEBUG_LOG"));
        urc = ioctl(state->proxy_fd, IOCTL_PROXY_NEXUS_Log_Deactivate, NULL);
        NEXUS_Platform_P_DebugLog_Uninit(&state->debugLog);
    }

    BDBG_MSG(("<DRIVER"));
    NEXUS_Platform_P_UninitDriver();

    BDBG_MSG(("<MODULE"));
    NEXUS_Module_Destroy(state->module);
    state->module = NULL;

    BDBG_MSG(("<BASE"));
    NEXUS_Base_Uninit();

    BDBG_MSG(("<MAGNUM"));
    NEXUS_Platform_P_Magnum_Uninit();

    memset(state, 0, sizeof(*state)); /* can't use BKNI_Memset here */
    return;
}

/* local copy of NEXUS_Platform_GetDefaultSettings, it initializes base and opens driver.
a partial bringup of the kernel driver is necessary to allow NEXUS_Platform_GetDefaultSettings to be
proxied so it can get defaults from Magnum. */
void
NEXUS_Platform_GetDefaultSettings(NEXUS_PlatformSettings *pSettings)
{
    if (!NEXUS_Platform_P_State.init) {
        NEXUS_Error errCode = NEXUS_P_Init();
        if (errCode) {
            /* If NEXUS_P_Init fails here, it will likely fail again in NEXUS_Platform_Init and return an error */
            errCode = BERR_TRACE(errCode);
            /* app can't know that this function failed, so at least prevent access to uninitialized memory */
            memset(pSettings, 0, sizeof(*pSettings));
            return;
        }
    }

    /* call the proxy version */
    NEXUS_Platform_GetDefaultSettings_proxy(pSettings);
#if NEXUS_HAS_FILE
    {
        NEXUS_FileModuleSettings fileModuleSettings;
        NEXUS_FileModule_GetDefaultSettings(&fileModuleSettings);
        pSettings->fileModuleSettings.workerThreads = fileModuleSettings.workerThreads;
        BDBG_CASSERT(sizeof(pSettings->fileModuleSettings.schedulerSettings) == sizeof(fileModuleSettings.schedulerSettings));
        BKNI_Memcpy(pSettings->fileModuleSettings.schedulerSettings, fileModuleSettings.schedulerSettings, sizeof(pSettings->fileModuleSettings.schedulerSettings));
    }
#endif
    #if NEXUS_NUM_AUDIO_DSP
    if ( NEXUS_GetEnv("audio_logs_enabled") )
    {
        pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eUartMessage].enabled = true;
        pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eDramMessage].enabled = true;
        pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eCoreDump].enabled = true;
    }
    else
    {
        if ( NEXUS_GetEnv("audio_uart_file") )
        {
            pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eUartMessage].enabled = true;
        }
        if ( NEXUS_GetEnv("audio_debug_file") )
        {
            pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eDramMessage].enabled = true;
        }
        if ( NEXUS_GetEnv("audio_core_file") )
        {
            pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eCoreDump].enabled = true;
        }
    }
    #endif
}

#if NEXUS_CONFIG_IMAGE
static NEXUS_ThreadHandle gImageThread=NULL;
static void NEXUS_Platform_P_ImageThread(void *context)
{
    BSTD_UNUSED(context);
    BDBG_MSG(("Image Thread"));
    BDBG_ASSERT(NEXUS_Platform_P_State.proxy_fd>0);
    Nexus_Platform_P_Image_Handler(NEXUS_Platform_P_State.proxy_fd, IOCTL_PROXY_NEXUS_Image);
    return;
}

static NEXUS_Error NEXUS_Platform_P_InitImage(void)
{
    NEXUS_ThreadSettings threadSettings;
    NEXUS_Error rc;
    BDBG_MSG(("Image init"));

    rc = Nexus_Platform_P_Image_Init();
    if(rc != NEXUS_SUCCESS)
    {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
#ifndef NEXUS_SECURITY_VIDEO_VERIFICATION_LEGACY_65NM
    /* BXVD_IMAGE_Interface is neither required nor defined when region verification is enabled */
    rc = Nexus_Platform_P_Image_Interfaces_Register(&BXVD_IMAGE_Interface, BXVD_IMAGE_Context, NEXUS_CORE_IMG_ID_XVD);
    if(rc != NEXUS_SUCCESS)
    {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
#endif
    #if BCHP_CHIP == 7408
    rc = Nexus_Platform_P_Image_Interfaces_Register(&BAPE_IMG_Interface, BAPE_IMG_Context, NEXUS_CORE_IMG_ID_RAP);
    if(rc != NEXUS_SUCCESS)
    {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    #elif defined BDSP_RAAGA_SUPPORT
    rc = Nexus_Platform_P_Image_Interfaces_Register(&BDSP_IMG_Interface, BDSP_IMG_Context, NEXUS_CORE_IMG_ID_RAP);
    if(rc != NEXUS_SUCCESS)
    {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    #else
    rc = Nexus_Platform_P_Image_Interfaces_Register(&BRAP_IMG_Interface, BRAP_IMG_Context, NEXUS_CORE_IMG_ID_RAP);
    if(rc != NEXUS_SUCCESS)
    {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    #endif
#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
    rc = Nexus_Platform_P_Image_Interfaces_Register(&BVCE_IMAGE_Interface, BVCE_IMAGE_Context, NEXUS_CORE_IMG_ID_VCE);
    if(rc != NEXUS_SUCCESS)
    {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
#endif

    NEXUS_Thread_GetDefaultSettings(&threadSettings);
    gImageThread = NEXUS_Thread_Create("Nexus_Image",NEXUS_Platform_P_ImageThread,NULL,&threadSettings);
    if(gImageThread == NULL)
    {
        BDBG_ERR(("Failed to create image thread"));
        return BERR_TRACE(NEXUS_OS_ERROR);
    }
    BDBG_MSG(("Created Image Thread"));
    return NEXUS_SUCCESS;

}
static void NEXUS_Platform_P_UninitImage(void)
{
    if (gImageThread) {
        BDBG_MSG(("Image Uninit"));
        Nexus_Platform_P_Image_Stop(NEXUS_Platform_P_State.proxy_fd, IOCTL_PROXY_NEXUS_Image);
        NEXUS_Thread_Destroy(gImageThread);
        Nexus_Platform_P_Image_Shutdown();
        gImageThread=NULL;
    }
}
#endif


/* local copy of NEXUS_Platform_Init, it initializes base, opens driver (if not already done by NEXUS_Platform_GetDefaultSettings, then calls driver NEXUS_Platform_Init, then initializes local modules */
NEXUS_Error
NEXUS_Platform_Init_tagged(const NEXUS_PlatformSettings *pSettings, unsigned platformCheck, unsigned versionCheck, unsigned structSizeCheck)
{
    NEXUS_Error errCode;

    if (NEXUS_Platform_P_State.platform_init) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    if (!NEXUS_Platform_P_State.init) {
        errCode = NEXUS_P_Init();
        if (errCode) return BERR_TRACE(errCode);
    }

    if (platformCheck != NEXUS_PLATFORM) {
        /* This code also ensures that NEXUS_PLATFORM is numeric, not alpha-numeric */
        BDBG_ERR(("NEXUS_Platform_Init failed with platform mismatch (nexus=%d, caller=%d). The application must be recompiled for this platform.",
            NEXUS_PLATFORM, platformCheck));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (versionCheck != NEXUS_P_GET_VERSION(NEXUS_PLATFORM)) {
        BDBG_ERR(("NEXUS_Platform_Init failed with version mismatch (nexus=%d.%d, caller=%d.%d). Please recompile application and/or nexus.",
            NEXUS_P_GET_VERSION(NEXUS_PLATFORM) / NEXUS_PLATFORM_VERSION_UNITS,
            NEXUS_P_GET_VERSION(NEXUS_PLATFORM) % NEXUS_PLATFORM_VERSION_UNITS,
            versionCheck / NEXUS_PLATFORM_VERSION_UNITS,
            versionCheck % NEXUS_PLATFORM_VERSION_UNITS));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (structSizeCheck != NEXUS_P_GET_STRUCT_SIZES()) {
        BDBG_ERR(("NEXUS_Platform failed with struct size mismatch (nexus=%d, caller=%d). Please recompile application and/or nexus.",
            NEXUS_P_GET_STRUCT_SIZES(), structSizeCheck));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

#if NEXUS_CONFIG_IMAGE
    BDBG_MSG(("Image"));
    errCode = NEXUS_Platform_P_InitImage();
    if ( errCode!=BERR_SUCCESS ) { errCode = BERR_TRACE(errCode); goto err_platform_init; }
#endif

    BDBG_MSG((">PLATFORM"));
    errCode = NEXUS_Platform_Init_tagged_proxy(pSettings, platformCheck, versionCheck, structSizeCheck);
    if ( errCode!=BERR_SUCCESS ) { errCode = BERR_TRACE(errCode); goto err_image_init; }

    BDBG_MSG((">ALL OTHER DRIVERS"));
    errCode = NEXUS_Platform_P_InitDriver( );
    if ( errCode!=BERR_SUCCESS ) { errCode = BERR_TRACE(errCode); goto err_driver; }

    BDBG_MSG((">OS"));
    errCode = NEXUS_Platform_P_InitOS();
    if ( errCode!=BERR_SUCCESS ) { errCode = BERR_TRACE(errCode); goto err_os; }

    errCode = NEXUS_Platform_P_ModulesInit(pSettings);
    if(errCode!=NEXUS_SUCCESS) {errCode=BERR_TRACE(errCode);goto err;}

    errCode = NEXUS_Platform_P_InitAudioLog();
    if(errCode!=NEXUS_SUCCESS) {NEXUS_Platform_Uninit(); return BERR_TRACE(errCode);}

    /* Success */
    NEXUS_Platform_P_State.platform_init = true;
    BDBG_MSG((">DONE"));
    return NEXUS_SUCCESS;

err:
    NEXUS_Platform_P_State.module = NULL;
    NEXUS_Platform_P_UninitOS();
err_os:
    NEXUS_Platform_P_UninitDriver();
err_driver:
    NEXUS_Platform_Uninit_proxy();
err_image_init:
#if NEXUS_CONFIG_IMAGE
    NEXUS_Platform_P_UninitImage();
err_platform_init:
#endif
    NEXUS_P_Uninit();
    return errCode;
}

void
NEXUS_Platform_Uninit(void)
{
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;

    if (!state->platform_init) {
        return;
    }
    state->platform_init = false;

    NEXUS_Platform_P_UninitAudioLog();
    NEXUS_Platform_P_ModulesUninit();
#if NEXUS_CONFIG_IMAGE
    NEXUS_Platform_P_UninitImage();
#endif

    if(!state->slave) {
        BDBG_MSG(("<OS"));
        NEXUS_Platform_P_UninitOS();

        BDBG_MSG(("<PLATFORM_UNINIT"));
        NEXUS_Platform_Uninit_proxy();

        BDBG_MSG(("<NEXUS UNINIT"));
        NEXUS_P_Uninit();
    } else {
        BDBG_MSG(("<OS"));
        NEXUS_Platform_P_UninitOS();

        /* tell driver that shutdown is clean */
        (void)ioctl(state->proxy_fd, IOCTL_PROXY_NEXUS_Platform_Uninit, NULL);

        BDBG_MSG(("<DRIVER"));
        NEXUS_Platform_P_UninitDriver();

        BDBG_MSG(("<NEXUS UNINIT"));
        NEXUS_P_Uninit();
    }
    return ;
}

/* local copy */
NEXUS_Error NEXUS_Platform_InitStandby( const NEXUS_PlatformStandbySettings *pSettings )
{
    NEXUS_Error errCode;

    if (!NEXUS_Platform_P_State.init) {
        errCode = NEXUS_P_Init();
        if (errCode) return BERR_TRACE(errCode);
    }

    BDBG_MSG((">PLATFORM"));
    errCode = NEXUS_Platform_InitStandby_proxy(pSettings);
    if ( errCode!=BERR_SUCCESS ) { errCode = BERR_TRACE(errCode); goto err; }

    return 0;

err:
    NEXUS_P_Uninit();
    return errCode;
}

void NEXUS_Platform_UninitStandby(void)
{
    BDBG_MSG(("<PLATFORM"));
    NEXUS_Platform_UninitStandby_proxy();
    NEXUS_P_Uninit();
}

static
NEXUS_Error NEXUS_Platform_P_InitProxy(int fd)
{
    NEXUS_Platform_P_State.proxy_fd = fd;
    proxy_ioctl_fd = fd;
    return NEXUS_SUCCESS;
}

static
void NEXUS_Platform_P_UninitProxy(void)
{
    NEXUS_Platform_P_State.proxy_fd = -1;
    proxy_ioctl_fd = -1;
    return;
}

/* find a stopped interface handle */
static nexus_proxy_stopped_callback_entry *
NEXUS_P_Proxy_FindStoppedCallbacks( void *interfaceHandle )
{
    nexus_proxy_stopped_callback_entry *entry;
    for(entry=BLST_D_FIRST(&NEXUS_Platform_P_State.stopped_callbacks);entry;entry=BLST_D_NEXT(entry,list)) {
        if(entry->interfaceHandle == interfaceHandle) {
            return entry;
        }
    }
    return NULL;
}

static int
NEXUS_Platform_P_RunCallbacks(int proxy_fd, PROXY_NEXUS_RunScheduler *runScheduler, PROXY_NEXUS_SchedulerLock *schedulerLock, struct NEXUS_Platform_P_ProxyScheduler *scheduler)
{
    unsigned i;
    int rc;
    PROXY_NEXUS_Scheduler *data = &scheduler->data;

    /* first, call blocking function to drive kernel state machine */
    rc = ioctl(proxy_fd, IOCTL_PROXY_NEXUS_RunScheduler, runScheduler);
    if(rc!=0) { rc = BERR_TRACE(BERR_OS_ERROR);goto done; }
    if (!runScheduler->out.has_callbacks) {
        BDBG_MSG(("NEXUS_Platform_P_RunCallbacks(%p): no callbacks", scheduler));
        goto done;
    }

    BKNI_AcquireMutex(scheduler->dataLock);
    BDBG_ASSERT(data->out.count == 0); /* nothing should have been left over */
    data->in.priority = runScheduler->in.priority;
    rc = ioctl(proxy_fd, IOCTL_PROXY_NEXUS_Scheduler, data);
    if(rc!=0) {
        rc = BERR_TRACE(BERR_OS_ERROR);goto unlock_data;
    }
    BDBG_ASSERT(data->out.count<=PROXY_NEXUS_CALLBACK_PACKET);
    for(i=0;i<data->out.count;i++) {
        BDBG_MSG(("NEXUS_Platform_P_RunCallbacks: callback:%#lx context:%#lx param:%d", (unsigned long)data->out.callbacks[i].desc.callback, (unsigned long)data->out.callbacks[i].desc.context, data->out.callbacks[i].desc.param));
        if(data->out.callbacks[i].desc.callback) {
            nexus_proxy_stopped_callback_entry *entry;

            schedulerLock->lock = true;
            rc = ioctl(proxy_fd, IOCTL_PROXY_NEXUS_SchedulerLock, schedulerLock);
            if(rc!=0) { rc = BERR_TRACE(BERR_OS_ERROR); break; }

            if (data->out.callbacks[i].interfaceHandle) {
                /* remember the current callback so we only sync with schedulers where necessary */
                scheduler->current_callback = data->out.callbacks[i].interfaceHandle;

                /* must acquire callbackLock before releasing dataLock.
                dataLock cannot be held during callback. */
                BKNI_AcquireMutex(scheduler->callbackLock);
                BKNI_ReleaseMutex(scheduler->dataLock);

                NEXUS_LockModule();
                /* check NEXUS_P_Proxy_FindStoppedCallbacks. this is needed for non-Close StopCallbacks
                where the stop may be persistent. */
                entry = NEXUS_P_Proxy_FindStoppedCallbacks(data->out.callbacks[i].interfaceHandle);
                NEXUS_UnlockModule();
                if (!entry) {
                    data->out.callbacks[i].desc.callback(data->out.callbacks[i].desc.context, data->out.callbacks[i].desc.param);
                }

                BKNI_AcquireMutex(scheduler->dataLock);
                BKNI_ReleaseMutex(scheduler->callbackLock);
                scheduler->current_callback = NULL;
            }
            schedulerLock->lock = false;
            rc = ioctl(proxy_fd, IOCTL_PROXY_NEXUS_SchedulerLock, schedulerLock);
            if(rc!=0) { rc = BERR_TRACE(BERR_OS_ERROR); break; }
        }
    }
unlock_data:
    data->out.count = 0; /* all have been consumed */
    BKNI_ReleaseMutex(scheduler->dataLock);
done:
    return rc;
}

static void
NEXUS_Platform_P_SchedulerThread(void *scheduler_)
{
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;
    struct NEXUS_Platform_P_ProxyScheduler *scheduler = scheduler_;
    PROXY_NEXUS_SchedulerLock schedulerLock;
    int proxy_fd;
    NEXUS_ModulePriority priority;

    BDBG_MSG(("NEXUS_Platform_P_SchedulerThread>>:%#lx[%u]", (unsigned long)scheduler, scheduler->priority));
    priority = scheduler->priority;
    schedulerLock.priority = priority;
    proxy_fd = state->proxy_fd;
    while(!state->stop) {
        NEXUS_P_Base_Scheduler_Status status;
        PROXY_NEXUS_RunScheduler data;

        NEXUS_P_Base_ExternalScheduler_Step(priority, 0, &status, NULL, NULL);
         /* since user thread can't wakeup on kernel events, we do polling in user scheduler */
        if(status.exit) {
            break;
        }
        data.in.priority = priority;
        data.in.timeout = status.idle?1000:100; /* since kernel thread wouldn't makeup on user events/timers we limit sleep time to 100ms or 1000ms */
        BDBG_MSG(("NEXUS_Platform_P_SchedulerThread:%#lx[%u] timeout:%u(%u)", (unsigned long)scheduler, (unsigned)scheduler->priority, (unsigned)data.in.timeout, (unsigned)status.timeout));
        if(data.in.timeout>status.timeout) {
           data.in.timeout=status.timeout;
        }
        if(NEXUS_Platform_P_RunCallbacks(proxy_fd, &data, &schedulerLock, scheduler)!=0) {
            break;
        }
    }
    BDBG_MSG(("NEXUS_Platform_P_SchedulerThread<<:%#lx[%u]", (unsigned long)scheduler, scheduler->priority));
    return;
}

static void
NEXUS_Platform_P_SchedulerSlaveThread(void *scheduler_)
{
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;
    PROXY_NEXUS_SchedulerLock schedulerLock;
    int proxy_fd;
    struct NEXUS_Platform_P_ProxyScheduler *scheduler = scheduler_;

    BDBG_MSG(("NEXUS_Platform_P_SchedulerSlaveThread>>"));
    schedulerLock.priority = NEXUS_ModulePriority_eDefault;
    proxy_fd = state->proxy_fd;
    while(!state->stop) {
        PROXY_NEXUS_RunScheduler data;
        unsigned i;

        for(i=0;i<NEXUS_ModulePriority_eMax;i++) { /* in a single shared thread just step over all schedulers */
            NEXUS_P_Base_Scheduler_Status status;

            NEXUS_P_Base_ExternalScheduler_Step((NEXUS_ModulePriority)i, 0, &status, NULL, NULL);
        }

        data.in.priority = NEXUS_ModulePriority_eDefault;
        data.in.timeout = 100;
        BDBG_MSG(("NEXUS_Platform_P_SchedulerSlaveThread: timeout:%u(%u)", (unsigned)data.in.timeout, (unsigned)data.in.timeout));
        if(NEXUS_Platform_P_RunCallbacks(proxy_fd, &data, &schedulerLock, scheduler)!=0) {
            break;
        }
    }
    BDBG_MSG(("NEXUS_Platform_P_SchedulerSlaveThread<<"));
    return ;
}

void NEXUS_Platform_P_StopCallbacks( void *interfaceHandle )
{
    nexus_proxy_stopped_callback_entry *entry;
    unsigned i;
    int rc;

    if (!interfaceHandle) return;
    
    /* first, stop callbacks in the driver */
    rc = ioctl(NEXUS_Platform_P_State.proxy_fd, IOCTL_PROXY_NEXUS_StopCallbacks, interfaceHandle);
    if (rc!=0) { BERR_TRACE(BERR_OS_ERROR);}
    
    /* second, stop callbacks that are local */
    NEXUS_Base_P_StopCallbacks(interfaceHandle);
    
    /* third, add to persistent list of stopped callbacks */
    NEXUS_LockModule();
    entry = NEXUS_P_Proxy_FindStoppedCallbacks(interfaceHandle);
    if(entry==NULL) {
        entry = BKNI_Malloc(sizeof(*entry));
        if(entry) {
            entry->interfaceHandle = interfaceHandle;
            BLST_D_INSERT_HEAD(&NEXUS_Platform_P_State.stopped_callbacks, entry, list);
        } else {
            NEXUS_Error rc =BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            BSTD_UNUSED(rc);
        }
    }
    NEXUS_UnlockModule();

    /* here all relevant callbacks are marked as 'stopped'. now, we must synchronize with every scheduler
    to ensure this function returns with no stopped callback still running on any scheduler. */
    for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
        struct NEXUS_Platform_P_ProxyScheduler *scheduler = &NEXUS_Platform_P_State.schedulers[i];
        if (scheduler->callbackLock) {
            unsigned j;
            bool wait = false;

            /* delete in-flight callbacks that have been stopped */
            BKNI_AcquireMutex(scheduler->dataLock);
            for (j=0;j<PROXY_NEXUS_CALLBACK_PACKET && j<scheduler->data.out.count;j++) {
                if (scheduler->data.out.callbacks[j].interfaceHandle == interfaceHandle) {
                    scheduler->data.out.callbacks[j].interfaceHandle = NULL;
                }
            }
            wait = (scheduler->current_callback == interfaceHandle);
            BKNI_ReleaseMutex(scheduler->dataLock);

            /* only wait for stopped callback is current on the scheduler */
            if (wait) {
                /* a simple acquire/release pair is enough to ensure that any active callback (which may include a stopped callback) completes.
                if you find that your application has deadlocked here, please debug your application and don't relax this code.
                for instance, you may be trying to close an interface inside a callback, or your close function might be protected with the same
                mutex which is being acquired inside a callback. both of these cases will result in a deadlock here. */
                BKNI_AcquireMutex(scheduler->callbackLock);
                BKNI_ReleaseMutex(scheduler->callbackLock);
            }
        }
    }

    return;
}

void NEXUS_Platform_P_StartCallbacks( void *interfaceHandle )
{
    nexus_proxy_stopped_callback_entry *entry;
    int rc;

    if (!interfaceHandle) return;
    
    rc = ioctl(NEXUS_Platform_P_State.proxy_fd, IOCTL_PROXY_NEXUS_StartCallbacks, interfaceHandle);
    if (rc!=0) { BERR_TRACE(BERR_OS_ERROR);}

    NEXUS_Base_P_StartCallbacks(interfaceHandle);

    NEXUS_LockModule();
    entry = NEXUS_P_Proxy_FindStoppedCallbacks(interfaceHandle);
    if(entry!=NULL) {
        BLST_D_REMOVE(&NEXUS_Platform_P_State.stopped_callbacks, entry, list);
        BKNI_Free(entry);
    }
    NEXUS_UnlockModule();
    return;
}

NEXUS_Error
NEXUS_Platform_AuthenticatedJoin(const NEXUS_ClientAuthenticationSettings *pSettings)
{
    BERR_Code rc;
    int urc;
    unsigned i;
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;

    if (state->platform_init) {
        /* if you init (don't uninit) then join, you will fail here.
        likewise, if you init, fork (but don't exec), then join, you will fail here. */
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    if (state->init) {
        BDBG_WRN(("clients should not call NEXUS_Platform_GetDefaultSettings"));
        NEXUS_P_Uninit();
        /* fall through. console will contain errors, but it will work. */
    }

    for(i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS;i++) {
        state->fds[i] = -1; /* unopened == -1, not 0 */
    }
    state->slave = true;

    rc = NEXUS_Platform_P_Magnum_Init();
    if( rc != BERR_SUCCESS)  { rc = BERR_TRACE(rc); goto err_magnum; }

    /* Now, proceed to boot the board. -- Always initialize base first */
    rc = NEXUS_Base_Init(NULL);
    if ( rc !=BERR_SUCCESS ) { rc = BERR_TRACE(rc); goto err_base; }

    BDBG_MSG((">MODULE"));
    state->module = NEXUS_Module_Create("proxy", NULL);
    if ( !state->module ) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_proxy; }

    BDBG_MSG((">PROXY DRIVER"));
    rc = NEXUS_Platform_P_InitOneDriver( NEXUS_PLATFORM_P_NUM_DRIVERS-1);
    if(rc!=BERR_SUCCESS) {
        goto err_driver;
    }

    BDBG_ASSERT(state->proxy_fd>0);
    urc = ioctl(state->proxy_fd, IOCTL_PROXY_NEXUS_AuthenticatedJoin, pSettings);
    if(urc!=0) { rc = BERR_TRACE(BERR_OS_ERROR);goto err_join; }

    BDBG_MSG((">ALL OTHER DRIVERS"));
    rc = NEXUS_Platform_P_InitDriver();
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_driver; }

    BDBG_MSG((">OS"));
    rc = NEXUS_Platform_P_InitOS();
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_os; }

    rc = NEXUS_Platform_P_ModulesInit(NULL);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_modules;}

    state->platform_init = true;

    return NEXUS_SUCCESS;

err_modules:
    NEXUS_Platform_P_UninitOS();
err_driver:
err_join:
    NEXUS_Platform_P_UninitDriver();
err_os:
    NEXUS_Module_Destroy(state->module);
err_proxy:
    NEXUS_Base_Uninit();
err_base:
    NEXUS_Platform_P_Magnum_Uninit();
err_magnum:
    return NEXUS_NOT_SUPPORTED;
}

/* including the c code requires less refactoring of internal apis */
#include "nexus_platform_local.c"
