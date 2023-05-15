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
* $brcm_Revision: 25 $
* $brcm_Date: 9/4/12 4:03p $
*
* API Description:
*   API name: Platform
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/linuxuser.client/nexus_platform_os.c $
* 
* 25   9/4/12 4:03p erickson
* SW7420-1123: refactor common, local code for linuxuser.client and
*  linuxuser.proxy
* 
* 24   8/31/12 2:54p erickson
* SW7425-3418: rework user mode callbacks: filter redundant callbacks on
*  server. sync NEXUS_StopCallbacks across all callback threads.
* 
* 23   8/20/12 12:16p erickson
* SW7425-3260: add stub NEXUS_Platform_GetDefaultSettings, don't call KNI
*  if nexus not up
* 
* 22   7/9/12 11:30a erickson
* SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
* 
* 21   6/6/12 1:36p erickson
* SW7435-27: use /dev/mem for core1_server uncached mmap because
*  /dev/brcm0 doesn't honor O_SYNC
*
* 20   5/9/12 10:55a erickson
* SW7435-27: rework NEXUS_WEBCPU env variable to support core0
*  client/server, core1_server, and eventually core1_client
*
* 19   5/8/12 5:33p erickson
* SW7435-27: add webcpu support
*
* 18   10/24/11 11:23a erickson
* SW7420-1690: add bcmdriver non-root mmap support
*
* 17   9/13/11 1:23p erickson
* SW7420-1148: change client modes to unprotected/protected/untrusted
*
* 16   9/8/11 8:49a erickson
* SW7420-1123: clean shutdown of clients, fix call to
*  NEXUS_Base_Core_Uninit, improve client BDBG_ERR's to direct user to
*  look at server console
*
* 15   8/3/11 10:40a erickson
* SW7420-1992: avoid client thunk errors by hardcoding fake address.
*  could be extended in the future.
*
* 14   7/26/11 12:03p erickson
* SW7420-1992: add stub NEXUS_Platform_Init to allow multi-mode apps
*
* 13   5/25/11 2:29p erickson
* SW7420-1123: fix remove_heap
*
* 12   4/11/11 11:47a erickson
* SW7405-522: add NEXUS_Base_Core_Init()
*
* 11   3/25/11 12:10p erickson
* SW7420-1123: server must pass heap status to avoid initial error from
*  NEXUS_Heap_GetStatus
*
* 10   3/23/11 4:35p erickson
* SW7420-1671: fix array bounds
*
* 9   3/11/11 2:57p erickson
* SW7400-3006: implement client-side support for
*  NEXUS_Platform_CreateHeap, sync up with
*  linuxuser.proxy/nexus_platform_os.c on minor points
*
* 8   3/7/11 12:17p erickson
* SW7420-1123: fix mmaps[] size
*
* 7   2/16/11 2:17p erickson
* SW7420-1123: implement nelem/nelem_out for IPC thunk
*
* 6   2/15/11 4:00p erickson
* SW7420-1491: NEXUS_Platform_P_Magnum_Init is callable from
*  nexus_platform_version_local.c
*
* 5   1/21/11 3:45p erickson
* SW7420-1123: clean shutdown of client
*
* 4   1/21/11 3:09p erickson
* SW7420-1123: pass NEXUS_ClientConfiguration from server to client as
*  part of initial connection, use it for client mmap
*
* 3   1/19/11 2:03p erickson
* SW7420-1123: socket-based usermode IPC
*
* 2   12/7/10 5:47p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 1   9/29/10 9:34a erickson
* SW7420-1123: add linux usermode IPC
*
***************************************************************************/

#include "nexus_types.h"
#include "nexus_base.h"
#include "client/nexus_client_prologue.h"
#include "nexus_platform_client_impl.h"
#include "nexus_platform.h"
#include "nexus_platform_standby.h"
#include "../linuxuser.proxy/nexus_platform_priv_proxy.h"
#include "blst_list.h"
#if NEXUS_WEBCPU_core1_server
NEXUS_Error NEXUS_Platform_P_InitWebCpuServer(void);
void NEXUS_Platform_P_UninitWebCpuServer(void);
#endif

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

BDBG_MODULE(nexus_platform_os);

/* define prototypes for init/uninit functions */
#define NEXUS_PLATFORM_P_DRIVER_MODULE(module) \
    extern BERR_Code nexus_client_##module##_init(NEXUS_P_ClientModuleHandle module); \
    extern void nexus_client_##module##_uninit(void);
#include "nexus_ipc_modules.h"
#undef NEXUS_PLATFORM_P_DRIVER_MODULE

static NEXUS_Error NEXUS_Platform_P_StartIpcClient(const NEXUS_ClientAuthenticationSettings *pSettings);
static void NEXUS_Platform_P_StopIpcClient(void);
static NEXUS_Error NEXUS_Platform_P_InitOS(void);
static void NEXUS_Platform_P_UninitOS(void);

static struct {
    const char *name;
    NEXUS_P_ClientModuleHandle module;
    NEXUS_Error (*init)(NEXUS_P_ClientModuleHandle module);
    void (*uninit)(void);
} g_nexus_client_handlers[] = {
#define NEXUS_PLATFORM_P_DRIVER_MODULE(module) {#module, NULL, nexus_client_##module##_init, nexus_client_##module##_uninit},
#include "nexus_ipc_modules.h"
#undef NEXUS_PLATFORM_P_DRIVER_MODULE
};
#define NEXUS_PLATFORM_P_NUM_DRIVERS  (sizeof(g_nexus_client_handlers)/sizeof(*g_nexus_client_handlers))

static struct NEXUS_Platform_P_State
{
    NEXUS_ModuleHandle module;
    int mem_fd;
    int uncached_mem_fd;
    struct {
        NEXUS_HeapHandle heap;
        void *addr;
        void *uncached_addr;
        unsigned long length;
        bool dynamic;
    } mmaps[NEXUS_MAX_HEAPS];
    bool init;
    struct nexus_client_init_data init_data;
} NEXUS_Platform_P_State;

#define NEXUS_MODULE_SELF NEXUS_Platform_P_State.module

static NEXUS_Error nexus_p_add_heap(unsigned i, NEXUS_HeapHandle heap, unsigned base, unsigned length, NEXUS_MemoryType memoryType, bool dynamic, void *user_address)
{
    void *addr;
    void *uncached_addr = NULL;

    if (user_address) {
        addr = user_address;
    }
    else {
        /* clients don't deal with eDriver or fake addresses, just eApplication mapping */
        if (memoryType & NEXUS_MEMORY_TYPE_APPLICATION_CACHED) {
            addr = mmap(0, length, PROT_READ|PROT_WRITE, MAP_SHARED, NEXUS_Platform_P_State.mem_fd, base);
            BDBG_MSG(("cached mmap offset=%x=>%x size=%d fd=%d errno=%d", base, addr, length, NEXUS_Platform_P_State.mem_fd, (addr==MAP_FAILED) ? errno : 0));
            if (addr==MAP_FAILED) return BERR_TRACE(BERR_OS_ERROR);
        }
        else {
            /* use a fake address, but don't bother allocating it for unique addresses. this avoids errors in the thunk.
            because the client's fake addresses aren't unique, the client cannot use them to specific memory for server use.
            if allocation of fake addresses is required, use linuxuser/nexus_map.h. */
            addr = (void*)0x80000000;
        }
    }

#if NEXUS_WEBCPU_core1_server
    if (memoryType & NEXUS_MEMORY_TYPE_DRIVER_UNCACHED) {
        uncached_addr = mmap(0, length, PROT_READ|PROT_WRITE, MAP_SHARED, NEXUS_Platform_P_State.uncached_mem_fd, base);
        BDBG_MSG(("uncached mmap offset=%x=>%x size=%d fd=%d errno=%d", base, uncached_addr, length, NEXUS_Platform_P_State.uncached_mem_fd, (uncached_addr==MAP_FAILED) ? errno : 0));
        if (uncached_addr==MAP_FAILED) return BERR_TRACE(BERR_OS_ERROR);
    }
#endif

    NEXUS_P_AddMap(base, addr, uncached_addr, length);

    NEXUS_Platform_P_State.mmaps[i].heap = heap;
    NEXUS_Platform_P_State.mmaps[i].addr = addr;
    NEXUS_Platform_P_State.mmaps[i].uncached_addr = uncached_addr;
    NEXUS_Platform_P_State.mmaps[i].length = length;
    NEXUS_Platform_P_State.mmaps[i].dynamic = dynamic;
    return 0;
}

static void nexus_p_remove_heap(unsigned i)
{
    if (NEXUS_Platform_P_State.mmaps[i].heap) {
        if (NEXUS_Platform_P_State.mmaps[i].addr) {
            munmap(NEXUS_Platform_P_State.mmaps[i].addr, NEXUS_Platform_P_State.mmaps[i].length);
        }
        if (NEXUS_Platform_P_State.mmaps[i].uncached_addr) {
            munmap(NEXUS_Platform_P_State.mmaps[i].uncached_addr, NEXUS_Platform_P_State.mmaps[i].length);
        }
        NEXUS_Platform_P_State.mmaps[i].heap = NULL;
    }
}

static NEXUS_Error
NEXUS_Platform_P_InitOS(void)
{
    NEXUS_Error rc;
    int mem_fd;
    unsigned i;

    /* do memory map through bcmdriver. the server must grant access to heaps. */
    mem_fd = open("/dev/brcm0", O_RDWR);
    if (mem_fd == -1) {
        /* if that fails (because we have an old version of bcmdriver, we can try /dev/mem. this will only work for root user.  */
        mem_fd = open("/dev/mem", O_RDWR);
        if ( mem_fd < 0 ) { rc = BERR_TRACE(BERR_OS_ERROR); goto err_mem_open; }
    }
    NEXUS_Platform_P_State.mem_fd = mem_fd;

#if NEXUS_WEBCPU_core1_server
    /* webcpu client needs uncached memory for GRC */
#if 0
    /* bcm_driver.c mmap does not support O_SYNC. I believe adding something like this will fix it: if ((file->f_flags & O_SYNC)) vma->vm_flags |= VM_IO;
    but, for now, assume core1_server runs as root and can use /dev/mem. */
    mem_fd = open("/dev/brcm0", O_SYNC|O_RDWR);
#else
    mem_fd = -1;
#endif
    if (mem_fd == -1) {
        mem_fd = open("/dev/mem", O_SYNC|O_RDWR);
        if ( mem_fd < 0 ) { rc = BERR_TRACE(BERR_OS_ERROR); goto err_mem_open; }
    }
    NEXUS_Platform_P_State.uncached_mem_fd = mem_fd;
#else
    NEXUS_Platform_P_State.uncached_mem_fd = -1;
#endif

    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_HeapHandle heap = NEXUS_Platform_P_State.init_data.config.heap[i];
        if (heap) {
            BDBG_ASSERT(NEXUS_Platform_P_State.init_data.config.heap[i] == NEXUS_Platform_P_State.init_data.heap[i].heap);
#if NEXUS_WEBCPU_core1_server
            if (i == 0) {
                NEXUS_Platform_P_State.init_data.heap[i].memoryType = NEXUS_MemoryType_eFull;
            }
#endif
            rc = nexus_p_add_heap(i,
                NEXUS_Platform_P_State.init_data.heap[i].heap,
                NEXUS_Platform_P_State.init_data.heap[i].offset,
                NEXUS_Platform_P_State.init_data.heap[i].size,
                NEXUS_Platform_P_State.init_data.heap[i].memoryType,
                false,
                NULL);
            if (rc) {rc = BERR_TRACE(rc); goto err_mmap;}
        }
    }

    return NEXUS_SUCCESS;

err_mmap:
    NEXUS_Platform_P_UninitOS();
err_mem_open:
    return rc;
}

static void
NEXUS_Platform_P_UninitOS(void)
{
    unsigned i;
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        nexus_p_remove_heap(i);

    }
    close(state->mem_fd);
    if (state->uncached_mem_fd != -1) {
        close(state->uncached_mem_fd);
    }
    BKNI_Memset(state->mmaps, 0, sizeof(state->mmaps));
    return;
}



void
NEXUS_Platform_Uninit(void)
{
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;

    NEXUS_Platform_P_ModulesUninit();
#if NEXUS_CONFIG_IMAGE
    NEXUS_Platform_P_UninitImage();
#endif

#if NEXUS_WEBCPU_core1_server
    NEXUS_Platform_P_UninitWebCpuServer();
#endif

    NEXUS_Platform_P_StopIpcClient();

    BDBG_MSG(("<OS"));
    NEXUS_Platform_P_UninitOS();

    BDBG_MSG(("<MODULE"));
    NEXUS_Module_Destroy(state->module);
    state->module = NULL;

    BDBG_MSG(("<BASE"));
    NEXUS_Base_Uninit();

    BDBG_MSG(("<MAGNUM"));
    NEXUS_Platform_P_Magnum_Uninit();

    state->init = false;
}

NEXUS_P_ClientHandle g_client;

static NEXUS_Error NEXUS_Platform_P_StartIpcClient(const NEXUS_ClientAuthenticationSettings *pSettings)
{
    unsigned i;
    NEXUS_Error  rc;

    g_client = NEXUS_P_Client_Init(pSettings, &NEXUS_Platform_P_State.init_data);
    if (!g_client) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    for (i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS;i++) {
        unsigned data_size = 4096; /* hardcoded max unix domain socket size */
        BDBG_MSG(("ipc init %d: %s (%d bytes)", i, g_nexus_client_handlers[i].name, data_size));

        g_nexus_client_handlers[i].module = NEXUS_P_Client_InitModule(g_client, i, data_size);
        if (!g_nexus_client_handlers[i].module) {
            rc = BERR_TRACE(NEXUS_UNKNOWN);
            goto error;
        }
        (*g_nexus_client_handlers[i].init)(g_nexus_client_handlers[i].module);
    }

    BDBG_WRN(("IPC client started"));
    return 0;

error:
    NEXUS_Platform_P_StopIpcClient();
    BDBG_ASSERT(rc); /* failure path */
    return rc;
}

static void NEXUS_Platform_P_StopIpcClient()
{
    unsigned i;

    /* tell server immediately that we are going down */
    if (g_client) {
        NEXUS_P_Client_Disconnect(g_client);
    }

    /* then start tearing down */
    for (i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS;i++) {
        if (g_nexus_client_handlers[i].module) {
            NEXUS_P_Client_UninitModule(g_nexus_client_handlers[i].module);
            (*g_nexus_client_handlers[i].uninit)();
            g_nexus_client_handlers[i].module = NULL;
        }
    }
    if (g_client) {
        NEXUS_P_Client_Uninit(g_client);
        g_client = NULL;
    }
}

void NEXUS_Platform_GetDefaultSettings(NEXUS_PlatformSettings *pSettings)
{
    memset(pSettings, 0, sizeof(*pSettings));
    fprintf(stderr, "### libnexus_client.so does not implement NEXUS_Platform_GetDefaultSettings\n");
}

NEXUS_Error NEXUS_Platform_Init_tagged( const NEXUS_PlatformSettings *pSettings, unsigned platformCheck, unsigned versionCheck, unsigned structSizeCheck )
{
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(platformCheck);
    BSTD_UNUSED(versionCheck);
    BSTD_UNUSED(structSizeCheck);
    fprintf(stderr, "### libnexus_client.so does not implement NEXUS_Platform_Init\n");
    return -1;
}

NEXUS_Error NEXUS_Platform_AuthenticatedJoin( const NEXUS_ClientAuthenticationSettings *pSettings )
{
    BERR_Code rc;
    struct NEXUS_Platform_P_State *state = &NEXUS_Platform_P_State;

    rc = NEXUS_Platform_P_Magnum_Init();
    if( rc != BERR_SUCCESS)  { rc = BERR_TRACE(rc); goto err_magnum; }

    /* Now, proceed to boot the board. -- Always initialize base first */
    rc = NEXUS_Base_Init(NULL);
    if ( rc !=BERR_SUCCESS ) { rc = BERR_TRACE(rc); goto err_base; }

    BDBG_MSG((">MODULE"));
    state->module = NEXUS_Module_Create("proxy", NULL);
    if ( !state->module ) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_proxy; }

    /* start ipc client modules */
    rc = NEXUS_Platform_P_StartIpcClient(pSettings);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_ipcclient;}

    /* InitOS happens after the IPC connections are made. This allows us to learn the server-provided
    heap configuration and map according to it. */
    BDBG_MSG((">OS"));
    rc = NEXUS_Platform_P_InitOS();
    if(rc!=BERR_SUCCESS) { goto err_os; }

#if NEXUS_WEBCPU_core1_server
    /* must bring up local modules after ipc is up so we can make calls to the server */
    rc = NEXUS_Platform_P_InitWebCpuServer();
    if(rc!=BERR_SUCCESS) { goto err_os; }
#endif

    rc = NEXUS_Platform_P_ModulesInit(NULL);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_modules;}

    NEXUS_Platform_P_State.init = true;

    return NEXUS_SUCCESS;

err_modules:
    NEXUS_Platform_P_UninitOS();
err_os:
    NEXUS_Platform_P_StopIpcClient();
err_ipcclient:
    NEXUS_Module_Destroy(state->module);
err_proxy:
    NEXUS_Base_Uninit();
err_base:
    NEXUS_Platform_P_Magnum_Uninit();
err_magnum:
    return NEXUS_NOT_SUPPORTED;
}

void NEXUS_Platform_GetClientConfiguration( NEXUS_ClientConfiguration *pSettings )
{
    *pSettings = NEXUS_Platform_P_State.init_data.config;
}

void NEXUS_Platform_P_StopCallbacks(void *interfaceHandle)
{
    /* stop callbacks that are local */
    NEXUS_Base_P_StopCallbacks(interfaceHandle);
    
    /* stop callbacks in server and in flight */
    NEXUS_P_Client_StopCallbacks(g_client, interfaceHandle);
}

void NEXUS_Platform_P_StartCallbacks(void *interfaceHandle)
{
    /* start callbacks in server */
    NEXUS_P_Client_StartCallbacks(g_client, interfaceHandle);
    
    /* start callbacks that are local */
    NEXUS_Base_P_StartCallbacks(interfaceHandle);
}

/* including the c code requires less refactoring of internal apis */
#include "../linuxuser.proxy/nexus_platform_local.c"
