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
 * $brcm_Workfile: nexus_generic_driver.c $
 * $brcm_Revision: SW7435-433/1 $
 * $brcm_Date: 12/10/12 2:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/common/src/linuxkernel/nexus_generic_driver.c $
 * 
 * SW7435-433/1   12/10/12 2:52p mward
 * SW7435-519: prevent stale callbacks from crashing driver. release
 *  scheduler lock when a client terminates.
 * 
 * 45   8/20/12 11:17a erickson
 * SW7420-2332: back out /main/42 change
 * 
 * 44   8/17/12 4:14p erickson
 * SW7435-303: skip objdb cleanup for platform module. it results in a
 *  deadlock if any client is still active.
 * 
 * 43   8/16/12 4:35p erickson
 * SW7435-303: add NEXUS_Platform_GetClientStatus
 * 
 * 42   8/8/12 11:13a erickson
 * SW7420-2332: avoid deadlock by only allowing server to trigger
 *  cleanup_queue
 * 
 * 41   8/7/12 10:35a erickson
 * SW7420-2332: fix race condition in b_objdb when cleaning up clients
 * 
 * 40   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 39   7/31/12 11:39a erickson
 * SW7358-173: move linux-specific code out of generic driver
 * 
 * 38   7/24/12 4:05p vsilyaev
 * SW7420-2328: Properly set default client
 * 
 * 37   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 36   7/13/12 11:57a erickson
 * SW7552-303: use OS-reported max_dcache_line_size instead of nexus macro
 * 
 * 35   7/9/12 1:58p erickson
 * SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
 * 
 * 34   7/9/12 11:30a erickson
 * SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
 * 
 * 33   6/22/12 4:45p erickson
 * SW7420-2078: open/close objdb in
 *  NEXUS_Platform_P_InitServer/NEXUS_Platform_P_UninitServer
 * 
 * 32   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 31   5/9/12 3:18p erickson
 * SW7231-748: add ioctl to unlocked list
 * 
 * 30   4/20/12 2:10p erickson
 * SW7231-748: allow init/uninit platform functions to be call unlocked,
 *  all others are locked
 * 
 * 29   12/5/11 11:23a erickson
 * SW7420-2129: don't set default heap in nexus_driver_module_heap_verify.
 *  allow each module to set default using internal functions.
 * 
 * 28   11/29/11 11:37a erickson
 * SW7420-2129: store default heap in nexus_driver_client_state
 * 
 * 27   11/11/11 10:23a erickson
 * SW7420-2116: don't hold driver lock while waiting for IMG thread
 * 
 * 26   11/8/11 4:13p erickson
 * SW7405-5543: refactor callback dequeue to cancel callbacks in flight
 *  and to only synchronize with schedulers where callback is current
 * 
 * 25   11/1/11 1:52p erickson
 * SW7420-2116: extend nexus_driver_state locking to close race
 *  conditions, add client->list to know state of client, refactor
 *  uninit_pending/uninit_in_progress booleans
 * 
 * 24   10/25/11 3:24p erickson
 * SW7420-2107: fix debuglog activate/deactivate call pairs
 * 
 * 23   10/25/11 2:35p gmohile
 * SW7358-173 : Change task flag to non-freezable during ioctl
 * 
 * 22   9/26/11 2:03p erickson
 * SW7420-1148: fix prev checkin
 * 
 * 21   9/22/11 4:42p erickson
 * SW7420-1148: autocleanup must be by module, then by client. multiple
 *  clients with interdependencies may have shutdown simultaneously.
 * 
 * 20   9/20/11 10:13a erickson
 * SW7420-1148: fix NEXUS_Platform_GetClientConfiguration, store entire
 *  client config in driver's client state
 * 
 * 19   9/16/11 3:40p erickson
 * SW7420-2064: clean up unprotected callbacks when client exits, allow
 *  re-registration when client connects
 * 
 * 18   9/13/11 1:24p erickson
 * SW7420-1148: change client modes to unprotected/protected/untrusted
 * 
 * 17   9/2/11 10:03a erickson
 * SW7420-1148: allow nexus_register/unregister to objdb from nexus
 *  module. enables more complex resource management.
 * 
 * 16   8/30/11 11:15a erickson
 * SW7420-1148: fix trusted=false even if certificate used. improve MSG-
 *  level debug for general system analysis.
 * 
 * 15   8/25/11 2:02p erickson
 * SW7420-1148: only use defaultUntrustedHeap if untrusted
 * 
 * 14   8/25/11 10:54a erickson
 * SW7420-1148: default server to using untrusted clients
 * 
 * 13   8/22/11 2:16p erickson
 * SW7420-1148: print error if trusted client terminates abnormally.
 *  server can now specify defaultUntrustedHeap.
 *
 * 12   8/10/11 4:32p erickson
 * SW7420-1148: add trusted boolean to nexus_generic_driver_open to
 *  prevent non-root from initializing nexus
 *
 * 11   8/9/11 2:09p erickson
 * SW7125-1069: protect against possible NULL dereference
 *
 * 10   8/9/11 12:49p vsilyaev
 * SW7420-1465: Improved handshake protocol
 *
 * 9   8/4/11 8:27p vsilyaev
 * SW7420-1465: Tighten check for Log related ioctl's
 *
 * 8   8/4/11 8:02p vsilyaev
 * SW7420-1465: Added protocol to ensure that all buffered debug output
 *  gets written out
 *
 * 7   8/3/11 5:33p vsilyaev
 * SW7420-1465, SW7405-5221: Route debug output to the special FIFO
 *  instead of syslog buffer
 *
 * 6   7/28/11 12:41p erickson
 * SW7420-1992: remove bad BDBG_WRN
 *
 * 5   7/26/11 12:05p erickson
 * SW7420-1992: NEXUS_Platform_StopServer must terminate all clients
 *  (except the server itself)
 *
 * 4   6/1/11 10:08a erickson
 * SW7358-46: add started event so that we can ensure proxy's IMG thread
 *  is in ioctl before NEXUS_Platform_Init is called
 *
 * 3   5/31/11 4:35p erickson
 * SW7420-1907: are WRN's when driver cannot be opened
 *
 * 2   5/17/11 10:04a erickson
 * SW7420-1819: simplify create/destroy client api, perform proper checks
 *  to pass stress test.
 *
 * 1   5/3/11 5:10p erickson
 * SW7420-1819: repartition so NFE is a standard feature
 *
 ************************************************************/
#include "nexus_base_driver.h"
#include "nexus_base_mmap.h"
#include "nexus_base_os.h"
#include "nexus_platform_priv.h"
#include "bkni.h"
#include "nexus_generic_driver_impl.h"
#include "nexus_driver_procfs.h"
#include "nexus_driver_ioctl.h"
#include "nexus_platform_ioctl.h" /* allows driver to watch for certain platform ioctls */

/* do not #include linux header files here. this file is generic and must compile with no OS dependency.
if a new OS feature is needed, an API must be added. */

BDBG_MODULE(nexus_generic_driver);

#define BDBG_MSG_TRACE(x) /*BDBG_MSG(x)*/
#define LOCK() BKNI_AcquireMutex(nexus_driver_state.lock)
#define UNLOCK() BKNI_ReleaseMutex(nexus_driver_state.lock)

#ifdef NEXUS_HAS_3D
#include "nexus_driver_graphics3d.h"
#endif /* NEXUS_HAS_3D */


/* declare prototypes for all ioctl functions */
#define NEXUS_PLATFORM_P_DRIVER_MODULE(module) \
    int nexus_driver_##module##_ioctl(void *context, unsigned int cmd, unsigned long arg, unsigned long type, bool unlocked); \
    int nexus_driver_##module##_open(unsigned); \
    void nexus_driver_##module##_close(void);

#include "nexus_driver_modules.h"
#undef NEXUS_PLATFORM_P_DRIVER_MODULE

static int nexus_driver_proxy_ioctl(void *context, unsigned int cmd, unsigned long arg, unsigned long type, bool unlocked);

/* define list of ioctl handlers */
static const struct {
   int (*ioctl)(void *context, unsigned int cmd, unsigned long arg, unsigned long type, bool unlocked);
} g_nexus_driver_ioctl_handlers[] = {
#define NEXUS_PLATFORM_P_DRIVER_MODULE(module) { nexus_driver_##module##_ioctl } ,
#include "nexus_driver_modules.h"
#undef NEXUS_PLATFORM_P_DRIVER_MODULE
{nexus_driver_proxy_ioctl} /* special entry for callback driver */
};

/* NEXUS_PLATFORM_P_NUM_DRIVERS == the number of modules + 1 for the proxy driver */
#define NEXUS_PLATFORM_P_NUM_DRIVERS  (sizeof(g_nexus_driver_ioctl_handlers)/sizeof(*g_nexus_driver_ioctl_handlers))


/* define list of open/close handlers, we keep open/close handlers separate to not polute ioctl table (ioctl table accessed more ofter and likely to fit in L1 cache) */
static struct {
    int (*open)(unsigned);
    void (*close)(void);
    struct nexus_driver_module_header *header; /* pointer into the module meta-data */
} g_nexus_driver_state_handlers[] = {
#define NEXUS_PLATFORM_P_DRIVER_MODULE(module) {nexus_driver_##module##_open, nexus_driver_##module##_close, NULL},
#include "nexus_driver_modules.h"
#undef NEXUS_PLATFORM_P_DRIVER_MODULE
    {NULL, NULL, NULL} /* no special open/close for proxy */
};

struct nexus_driver_state nexus_driver_state;
static int      nexus_driver_server_init_lock(unsigned pid);
static int      nexus_driver_server_preinit(void);
static int      nexus_driver_server_postinit_lock(void);
static void     nexus_driver_server_preuninit_lock(void);
static void     nexus_driver_server_uninit_lock(void);
static void     nexus_driver_process_cleanup_queue_lock(void);
static struct   nexus_driver_client_state *nexus_driver_open_client_lock(unsigned pid);
static void     nexus_driver_close_client_lock(struct nexus_driver_client_state *client);
static struct   nexus_driver_client_state *nexus_driver_connect_client_lock(unsigned pid, const NEXUS_Certificate *pCertificate);
static void     nexus_driver_disable_client_lock(struct nexus_driver_client_state *client);
static struct nexus_driver_client_state *nexus_driver_create_client_lock(const NEXUS_Certificate *pCertificate, const NEXUS_ClientConfiguration *pConfig);
static void     nexus_driver_disable_clients_lock(bool including_server);

void
nexus_driver_free(void *addr)
{
    if(addr) {
        BKNI_Free(addr);
    }
    return;
}

/* route all address conversions through dedicated functions */
void
nexus_driver_send_addr(void **paddr)
{
    /* inplace convert address from virtual to physical */
    void *addr = *paddr;
    if(addr) {
        *paddr = (void *)NEXUS_AddrToOffset(addr);
    }
    return;
}


void
nexus_driver_recv_addr_cached(void **paddr)
{
    void *addr = *paddr;
    /* inplace convert address from physical to virtual */
    if(addr) {
        *paddr = NEXUS_OffsetToCachedAddr((uint32_t)addr);
    }
    return;
}

/* nexus_driver_module_init is called from each module's nexus_driver_module_open handler */
int
nexus_driver_module_init(unsigned index, struct nexus_driver_module_header *header, NEXUS_ModuleHandle module,
        const struct b_objdb_class *class_table, const char *name)
{
    header->entry_count = 0;
    header->callback_map = NULL; /* lazily allocate callback map */
    header->name = name;
    /* module may be NULL if module is compiled in but not initialized by NEXUS_Platform_Init */
    header->module = module;
    b_objdb_init(&header->objdb, class_table, module);
    header->objdb.name = name;
    header->objdb.module = module;
    header->objdb.cancel_callbacks_locked = nexus_driver_deactivate_callbacks;
    header->objdb.cancel_callbacks_context = header;
    g_nexus_driver_state_handlers[index].header = header;
    BDBG_MSG(("nexus_driver_module_init: registering %d: %#lx (%s:%#lx) as %u", index, (unsigned long)header, name, (unsigned long)module));
    return 0;
}

/* nexus_driver_module_uninit is called from each module's nexus_driver_module_close handler */
void
nexus_driver_module_uninit(struct nexus_driver_module_header *header)
{
    b_objdb_uninit(&header->objdb);
    nexus_driver_callback_uninit(header);
}

int
nexus_generic_driver_open(unsigned module, void **pstate, unsigned pid, bool trusted)
{
    struct nexus_driver_module_driver_state *driver_state = NULL;
    int rc = 0;

    BDBG_MSG_TRACE((">>nexus_driver_open: %u", module));
    if(module >= NEXUS_PLATFORM_P_NUM_DRIVERS) {
        rc = BERR_TRACE(NEXUS_NOT_AVAILABLE);
        goto done;
    }

    /* alloc per-instance data */
    driver_state = BKNI_Malloc(sizeof(*driver_state));
    if (driver_state==NULL) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto done;
    }
    driver_state->pid = pid;

    LOCK();
    /* server opens with 0 first. client with NEXUS_PLATFORM_P_NUM_DRIVERS-1 first.
    only allow the server to init the driver. */
    if (module != 0 && (!nexus_driver_state.server || nexus_driver_state.uninit_in_progress)) {
        rc = NEXUS_NOT_AVAILABLE; /* don't use BERR_TRACE. this is not an error in the driver. */
        goto lockeddone;
    }

    if (nexus_driver_state.uninit_pending) {
        nexus_driver_server_uninit_lock();
    }

    /* init driver on first open */
    if (!nexus_driver_state.uninit_in_progress && !nexus_driver_state.server) {
        if (!trusted) {
            rc = BERR_TRACE(NEXUS_NOT_AVAILABLE);
            goto lockeddone;
        }
        rc = nexus_driver_server_init_lock(pid);
        if (rc) {
            rc = BERR_TRACE(NEXUS_NOT_AVAILABLE);
            goto lockeddone;
        }
        /* server's client has been created, so nexus_driver_open_client will work. */
    }

    driver_state->client = nexus_driver_open_client_lock(pid);
    if (!driver_state->client) {
        if (nexus_driver_state.uninit_in_progress) {
            /* client was not created because we are shutting down. uninit_in_progress must be checked after nexus_driver_open_client
            because the client could be pending inside on the LOCK(). */
            rc = NEXUS_NOT_AVAILABLE; /* don't use BERR_TRACE. this is not an error in the driver. */
            goto lockeddone;
        }
        /* it's normal for nexus_driver_open_client to return NULL until Join ioctl. however, no other driver
        will be opened until Join, so we can populate driver_state->client then. */
    }

    nexus_driver_state.open_count++;
    
    if (module< NEXUS_PLATFORM_P_NUM_DRIVERS-1) {
        driver_state->slave_scheduler = nexus_driver_slave_scheduler_acquire(pid);
    }
    else {
        /* proxy */
        driver_state->slave_scheduler = NULL;
    }

    *pstate = driver_state;

lockeddone:
    UNLOCK();
done:
    if (rc) {
        if (driver_state) {
            BKNI_Free(driver_state);
        }
    }
    BDBG_MSG_TRACE(("<<nexus_driver_open: %u -> %d", module, rc));
    return rc;
}

void nexus_driver_disable_clients(bool including_server)
{
    LOCK();
    nexus_driver_disable_clients_lock(including_server);
    UNLOCK();
}

static void nexus_driver_disable_clients_lock(bool including_server)
{
    struct nexus_driver_client_state *client, *next;
    for (client = BLST_S_FIRST(&nexus_driver_state.clients); client;) {
        next = BLST_S_NEXT(client, link);
        if (including_server || client != nexus_driver_state.server) {
            nexus_driver_disable_client_lock(client);
        }
        client = next;
    }
}

void
nexus_generic_driver_close(unsigned module, void *state, bool abnormal_termination)
{
    struct nexus_driver_module_driver_state *driver_state = state;

    BDBG_MSG_TRACE((">>nexus_driver_close:%u %s", module, abnormal_termination?"SIGNAL":""));

    BDBG_ASSERT(driver_state);
    if (driver_state->slave_scheduler) {
        struct nexus_driver_module_header *header = g_nexus_driver_state_handlers[module].header;
        nexus_driver_slave_scheduler_release(header, driver_state->slave_scheduler);
    }

    LOCK();

    /* may be NULL if client closes before Join */
    if (driver_state->client) {
        nexus_driver_close_client_lock(driver_state->client);
    }

    /* test if the server is closing without a full shutdown */
    if ((driver_state->client == nexus_driver_state.server) && !nexus_driver_state.uninit_in_progress && driver_state->client && !driver_state->client->refcnt) {
        BDBG_WRN(("forcing driver shutdown"));
        /* the server is going down without having shutdown all clients. signal is likely pending.
        the best we can do is to kill all clients, which will then cause the full shutdown. */
        nexus_driver_disable_clients_lock(true);
    }
    BKNI_Free(driver_state);

    /* if this is the last driver closed, we can do the full shutdown */
    BDBG_ASSERT(nexus_driver_state.open_count);
    if (--nexus_driver_state.open_count == 0) {
        /* we can only uninit if there's no signal pending. if not, we must defer until open or rmmod. */
        if (!abnormal_termination) {
            nexus_driver_server_uninit_lock();
        }
        else {
            nexus_driver_state.uninit_pending = true;
        }
    }
    UNLOCK();

    BDBG_MSG_TRACE(("<<nexus_driver_close: %u done", module));
}

int nexus_generic_driver_validate_mmap(unsigned module, void *context_, unsigned offset, unsigned size)
{
    struct nexus_driver_module_driver_state *context = context_;
    struct nexus_driver_client_state *client = context->client;

    BSTD_UNUSED(module); /* unused. added for api consistency. */
    BDBG_MSG(("nexus_generic_driver_validate_mmap %#x, %d\n", offset, size));

    BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
    BDBG_ASSERT(client->refcnt);

    if (!client->pid) {
        /* client has been disabled by the server */
        return -1; /* don't use BERR_TRACE. this is not an error in the driver. */
    }

    if (client->client.verify != b_objdb_verify_none) {
        /* if untrusted, it must be within one of the client's heaps */
        unsigned i;
        for (i=0;i<NEXUS_MAX_HEAPS;i++) {
            if (client->config.heap[i]) {
                NEXUS_MemoryStatus status;
                NEXUS_Heap_GetStatus(client->config.heap[i], &status);
                BDBG_MSG(("heap: %#x %d", status.offset, status.size));
                if (offset >= status.offset && offset + size <= status.offset + status.size) {
                    /* it's good */
                    break;
                }
            }
        }
        if (i == NEXUS_MAX_HEAPS) {
            /* not within a heap, so we fail */
            BDBG_ERR(("nexus_driver_mmap: no matching client heap found"));
            return -1;
        }
    }
    /* it's good */
    return 0;
}

int
nexus_generic_driver_ioctl(unsigned module, void *context_, unsigned int cmd, unsigned long arg)
{
    int rc;
    struct nexus_driver_module_driver_state *context = context_;
    struct nexus_driver_client_state *client = context->client;
    bool unlocked = false;

    BDBG_MSG_TRACE((">>nexus_driver_ioctl %u %#x %#x", module, cmd, arg));
    
    if (module == NEXUS_PLATFORM_P_NUM_DRIVERS-1 && cmd == IOCTL_PROXY_NEXUS_AuthenticatedJoin) {
        /* one exception: client == NULL when a client first Joins because it has not been authenticated yet. */
    }
    else {
        if (!client) {
            /* most likely a second process tried to call Init instead of Join. */
            BDBG_WRN(("second process did not Join"));
            rc = NEXUS_NOT_AVAILABLE; /* don't use BERR_TRACE. this is not an error in the driver. */
            goto done;
        }
        BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
        BDBG_ASSERT(client->refcnt);
        if(!client->pid) {
            if(module==(NEXUS_PLATFORM_P_NUM_DRIVERS-1) && (cmd==IOCTL_PROXY_NEXUS_Log_Dequeue || cmd==IOCTL_PROXY_NEXUS_Log_Deactivate || cmd==IOCTL_PROXY_NEXUS_Log_Destroy)) {
                /* one more exception allow Log_Dequeue ioctl's in */
            } else {
                BDBG_WRN(("client disabled by server"));
                rc = NEXUS_NOT_AVAILABLE; /* don't use BERR_TRACE. this is not an error in the driver. */
                goto done;
            }
        }
    }

    /* special server ioctls.
    unlocked = true if platform module must be called with no lock (e.g. because the lock will be created or destroyed in the call) */
    if (module == 0 && client->client.verify == b_objdb_verify_none) {
        switch (cmd) {
        case IOCTL_PLATFORM_NEXUS_Platform_Uninit:
            unlocked = true;
            break;
        case IOCTL_PLATFORM_NEXUS_Platform_Init_tagged:
            nexus_driver_server_preinit();
            unlocked = true;
            break;
        case IOCTL_PLATFORM_NEXUS_Platform_GetDefaultSettings:
        case IOCTL_PLATFORM_NEXUS_Platform_InitStandby:
        case IOCTL_PLATFORM_NEXUS_Platform_UninitStandby:    
        case IOCTL_PLATFORM_NEXUS_INIT:    
            unlocked = true;
            break;
        }
    }

    LOCK();
    if (nexus_driver_state.cleanup_pending) {
        /* cleanup of the desired client may be pending. we can clean up now. if nothing is pending, this is harmless. */
        nexus_driver_process_cleanup_queue_lock();
    }
    UNLOCK();

    rc = g_nexus_driver_ioctl_handlers[module].ioctl(context, cmd, arg, NEXUS_IOCTL_TYPE(cmd), unlocked);
    if (rc) {return BERR_TRACE(rc);}
    
done:
    BDBG_MSG_TRACE(("<<nexus_driver_ioctl %u %#x %#x -> %d", module, cmd, arg, rc));

    return rc;
}

int nexus_generic_driver_init(const struct nexus_generic_driver_init_settings *settings)
{
    int rc;
    unsigned i;

    rc = BKNI_Init();
    if (rc) return rc;
    rc = BDBG_Init();
    if (rc) return rc;

    rc = NEXUS_Base_Core_Init();
    if (rc) return rc;

    rc = BKNI_CreateMutex(&nexus_driver_state.lock);
    if (rc) return rc;

#ifdef NEXUS_CONFIG_IMAGE
    rc = nexus_img_interfaces_init();
    if (rc) return rc;
#endif

    nexus_driver_state.settings = *settings;

    /* set default nexus_driver_state state */
    /* unless server calls NEXUS_Platform_StartServer setting this to false, we default to allowing. */
    NEXUS_Platform_GetDefaultStartServerSettings(&nexus_driver_state.serverSettings);
    for(i=0;i<NEXUS_DRIVER_MAX_LOG_READERS;i++) {
        nexus_driver_state.debugLog.readers[i].fifo = NULL;
    }
    rc = nexus_driver_proc_init();
    if (rc) return rc;

    return 0;
}

void nexus_generic_driver_uninit(void)
{
    if (nexus_driver_state.uninit_pending) {
        /* if we didn't have a clean close, we need to clean on rmmod */
        LOCK();
        nexus_driver_server_uninit_lock();
        UNLOCK();
    }

    nexus_driver_proc_done();
#ifdef NEXUS_CONFIG_IMAGE
    nexus_img_interfaces_shutdown();
#endif

    BKNI_DestroyMutex(nexus_driver_state.lock);
    NEXUS_Base_Core_Uninit();
    {
        struct nexus_driver_state *state = &nexus_driver_state;
        unsigned i;

        for(i=0;i<NEXUS_DRIVER_MAX_LOG_READERS;i++) {
            if(state->debugLog.readers[i].fifo != NULL) {
                BDBG_FifoReader_Destroy(nexus_driver_state.debugLog.readers[i].fifo);
                state->debugLog.readers[i].fifo = NULL;
            }
        }

        if(state->debugLog.logWriter!=NULL) {
            BDBG_Log_SetFifo(NULL);
            BDBG_Fifo_Destroy(state->debugLog.logWriter);
            state->debugLog.logWriter = NULL;
        }
    }
    BDBG_WRN(("uninitialized"));
    BDBG_Uninit();
    BKNI_Uninit();
}


static int
nexus_driver_proxy_ioctl(void *context, unsigned int cmd, unsigned long arg, unsigned long type, bool unlocked)
{
    int rc;
    struct nexus_driver_module_driver_state *state = context;
    unsigned i;

    BSTD_UNUSED(unlocked);
    
    switch(cmd) {
    case IOCTL_PROXY_NEXUS_AuthenticatedJoin:
        {{
            PROXY_NEXUS_AuthenticatedJoin data;

            /* NEXUS_Platform_Init must already be called */
            if(!nexus_driver_state.active) {
                rc=(int)BERR_TRACE(NEXUS_NOT_AVAILABLE);
                goto err_ioctl;
            }

            if (arg) {
                rc = copy_from_user_small(&data, (void *)arg, sizeof(data));
                if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            }

            LOCK();
            /* authenticate and register pid with client */
            if (!nexus_driver_connect_client_lock(state->pid, arg ? &data.certificate : NULL)) {
                rc = BERR_TRACE(NEXUS_UNKNOWN);
                BDBG_ASSERT(!state->client);
            }
            else {
                /* now we can open the client and attach to the driver_state */
                state->client = nexus_driver_open_client_lock(state->pid);
            }
            UNLOCK();
            if (!state->client) {
                rc = BERR_TRACE(NEXUS_UNKNOWN);
                goto err_ioctl;
            }

            /* create callback scheduler */
            state->slave_scheduler = nexus_driver_slave_scheduler_create(state->client->pid);
            if (!state->slave_scheduler) {
                rc =(int)BERR_TRACE(NEXUS_UNKNOWN);
                if (state->client) {
                    LOCK();
                    nexus_driver_close_client_lock(state->client);
                    UNLOCK();
                    state->client = NULL;
                }
                goto err_ioctl;
            }
            state->client->joined = true;
            state->client->numJoins++;
            break;
        }}
    case IOCTL_PROXY_NEXUS_Platform_Uninit:
        {{
            /* clean uninit of client */
            BDBG_OBJECT_ASSERT(state->client, nexus_driver_client_state);
            state->client->joined = false;
            break;
        }}
    case IOCTL_PROXY_NEXUS_RunScheduler:
        {{
            PROXY_NEXUS_RunScheduler data;
            rc = copy_from_user_small(&data.in, (void *)arg, sizeof(data.in));
            /* coverity[dead_error_condition] - it's important to do this check in case it's not a dead condition in the future. */
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            rc = nexus_driver_run_scheduler(data.in.priority, data.in.timeout, &data.out.has_callbacks, state->slave_scheduler);
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            rc = copy_to_user_small((void *)((uint8_t *)arg+sizeof(data.in)), &data.out, sizeof(data.out));
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            break;
        }}
    case IOCTL_PROXY_NEXUS_Scheduler:
        {{
            PROXY_NEXUS_Scheduler data;
            rc = copy_from_user_small(&data.in, (void *)arg, sizeof(data.in));
            /* coverity[dead_error_condition] - it's important to do this check in case it's not a dead condition in the future. */
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            data.out.count = nexus_driver_scheduler_dequeue(data.in.priority, data.out.callbacks, PROXY_NEXUS_CALLBACK_PACKET, state->slave_scheduler);
            rc = copy_to_user_small((void *)((uint8_t *)arg+sizeof(data.in)), &data.out, sizeof(data.out));
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            break;
        }}
    case IOCTL_PROXY_NEXUS_GetMemory:
        {{
            unsigned int i = 0;
            PROXY_NEXUS_GetMemory data;
            unsigned total = 0;

            BDBG_OBJECT_ASSERT(state->client, nexus_driver_client_state);

            BKNI_Memset(&data, 0, sizeof(data));
            for (i=0;i<NEXUS_MAX_HEAPS;i++) {
                if (state->client->config.heap[i]) {
                    NEXUS_MemoryStatus status;
                    NEXUS_Heap_GetStatus(state->client->config.heap[i], &status);
                    data.region[total].base = status.offset;
                    data.region[total].length = status.size;
                    data.region[total].memoryType = NEXUS_MemoryType_eApplication;
                    total++;
                }
            }
            rc = copy_to_user_small((void *)arg, &data, sizeof(data));
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            break;
        }}
    case IOCTL_PROXY_NEXUS_SchedulerLock:
        {{
            PROXY_NEXUS_SchedulerLock data;
            rc = copy_from_user_small(&data, (void *)arg, sizeof(data));
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            nexus_driver_scheduler_lock(state->client, data.priority, data.lock);
            break;
        }}
#ifdef NEXUS_CONFIG_IMAGE
    case IOCTL_PROXY_NEXUS_Image:
        {{
            nexus_img_ioctl(cmd, arg);
            break;
        }}
#endif
#ifdef NEXUS_HAS_3D
    case IOCTL_PROXY_NEXUS_GetSurfaceOffset:
        {{
            PROXY_NEXUS_GetSurfaceOffset data;
            rc = copy_from_user_small(&data, (void *)arg, sizeof(data));
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            nexus_driver_3d_get_surface_offset(&data);
            rc = copy_to_user_small((void *)arg, &data, sizeof(data));
            if(rc!=0) {rc = BERR_TRACE(rc);goto err_fault;}
            break;
        }}
    case IOCTL_PROXY_NEXUS_Hook3dInterrupt:
        {{
            BERR_Code err;
            PROXY_NEXUS_Hook3dInterrupt data;
            rc = copy_from_user_small(&data, (void *)arg, sizeof(data));
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            err = nexus_driver_3d_hook_interrupt(&data);
            if( err != BERR_SUCCESS ) {
                rc = BERR_TRACE(rc);
                goto err_ioctl;
            }
            break;
        }}
    case IOCTL_PROXY_NEXUS_WaitFor3dInterrupt:
        {{
            PROXY_NEXUS_WaitFor3dInterrupt data;
            rc = copy_from_user_small(&data, (void *)arg, sizeof(data));
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            nexus_driver_3d_wait_for_interrupt(&data);
            rc = copy_to_user_small((void *)arg, &data, sizeof(data));
            if(rc!=0) {rc = BERR_TRACE(rc);goto err_fault;}
            break;
        }}
    case IOCTL_PROXY_NEXUS_Unhook3dInterrupt:
        {{
            nexus_driver_3d_unhook_interrupt();
            break;
        }}
    case IOCTL_PROXY_NEXUS_3dPowerManagement:
    {{
        PROXY_NEXUS_3dPowerManagement data;
        rc = copy_from_user_small(&data, (void *)arg, sizeof(data));
        if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
        nexus_driver_3d_power_management(&data);
        break;
    }}
#endif /* NEXUS_HAS_3D */
    case IOCTL_PROXY_NEXUS_Log_Activate:
    {{
        BDBG_Fifo_CreateSettings logSettings;
        PROXY_NEXUS_Log_Activate data;
        BERR_Code mrc;
        struct nexus_driver_state *state = &nexus_driver_state;

        if (!state->debugLog.logWriter) {
            rc = copy_from_user_small(&data, (void *)arg, sizeof(data));
            if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
            BDBG_Fifo_GetDefaultCreateSettings(&logSettings);
    
            logSettings.nelements = data.debug_log_size;
            BDBG_Log_GetElementSize(&logSettings.elementSize);
            mrc = BDBG_Fifo_Create(&state->debugLog.logWriter, &logSettings);
            if(mrc!=BERR_SUCCESS) {rc=BERR_TRACE(mrc);goto err_ioctl;}
        }
        BDBG_Log_SetFifo(state->debugLog.logWriter);
        break;
    }}
    case IOCTL_PROXY_NEXUS_Log_Deactivate:
    {{
        BDBG_Log_SetFifo(NULL);
        break;
    }}
    case IOCTL_PROXY_NEXUS_Log_Test:
    {{
        int result;
        result = nexus_driver_state.debugLog.logWriter!=NULL;
        rc = copy_to_user_small((void *)arg, &result, sizeof(result));
        if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
        break;
    }}
    case IOCTL_PROXY_NEXUS_Log_Create:
    {{
        if(nexus_driver_state.debugLog.logWriter==NULL) {rc=BERR_TRACE(NEXUS_NOT_AVAILABLE);goto err_ioctl;}
        for(i=0;i<NEXUS_DRIVER_MAX_LOG_READERS;i++) {
            PROXY_NEXUS_Log_Instance  data;
            if(nexus_driver_state.debugLog.readers[i].fifo == NULL) {
                BERR_Code mrc;
                mrc = BDBG_FifoReader_Create(&nexus_driver_state.debugLog.readers[i].fifo, nexus_driver_state.debugLog.logWriter);
                if(mrc!=BERR_SUCCESS) {rc=BERR_TRACE(mrc);goto err_ioctl;}
                data.instance  = i;
                rc = copy_to_user_small((void *)arg, &data, sizeof(data));
                if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
                goto done;
            }
        }
        rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto err_ioctl;
    }}
    case IOCTL_PROXY_NEXUS_Log_Dequeue:
    {{
        PROXY_NEXUS_Log_Dequeue data;
        struct nexus_driver_state *state = &nexus_driver_state;
        BERR_Code mrc;
        char dbgStr[257];
        size_t dbgStrLen;


        rc = copy_from_user_small(&data, (void *)arg, sizeof(data));
        if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
        if(data.instance.instance>=NEXUS_DRIVER_MAX_LOG_READERS) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto err_ioctl; }
        if(state->debugLog.readers[data.instance.instance].fifo == NULL) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto err_ioctl; }
        dbgStrLen = sizeof(dbgStr);
        if(data.buffer_size>dbgStrLen) {
            dbgStrLen = data.buffer_size;
        }
        mrc = BDBG_Log_Dequeue(state->debugLog.readers[data.instance.instance].fifo, &data.timeout, dbgStr, dbgStrLen, &data.buffer_size);
        if(mrc!=BERR_SUCCESS) {rc=BERR_TRACE(mrc);break;}
        rc=copy_to_user_small(data.buffer, dbgStr, data.buffer_size);
        if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
        rc=copy_to_user_small(arg, &data, sizeof(data));
        if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
        break;
    }}
    case IOCTL_PROXY_NEXUS_Log_Destroy:
    {{
        PROXY_NEXUS_Log_Instance data;
        struct nexus_driver_state *state = &nexus_driver_state;

        rc = copy_from_user_small(&data, (void *)arg, sizeof(data));
        if(rc!=0) {rc=BERR_TRACE(rc);goto err_fault;}
        if(data.instance>=NEXUS_DRIVER_MAX_LOG_READERS) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto err_ioctl; }
        if(state->debugLog.readers[data.instance].fifo == NULL) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto err_ioctl; }
        BDBG_FifoReader_Destroy(state->debugLog.readers[data.instance].fifo);
        state->debugLog.readers[data.instance].fifo = NULL;
        break;
    }}
    case IOCTL_PROXY_NEXUS_StopCallbacks:
        NEXUS_Platform_P_StopCallbacks(arg);
        break;
    case IOCTL_PROXY_NEXUS_StartCallbacks:
        NEXUS_Platform_P_StartCallbacks(arg);
        break;
    default:
        rc = NEXUS_UNKNOWN;
        goto err_ioctl;
    }
done:
    return 0;

err_ioctl:
err_fault:
    return rc;
}

static void
nexus_driver_module_print_cb_null(void *context)
{
    void (*callback)() = (void(*)())context;
    callback();
}

void
nexus_driver_module_init_enum_cb(void *cntx, NEXUS_ModuleHandle handle, const char *name, const NEXUS_ModuleSettings *settings)
{
    if (settings->dbgModules && settings->dbgPrint)
        nexus_driver_proc_register_status(name, handle, settings->dbgModules, nexus_driver_module_print_cb_null, (void *)settings->dbgPrint, settings->dbgPrintSize);
}

void
nexus_driver_module_uninit_enum_cb(void *cntx, NEXUS_ModuleHandle handle, const char *name, const NEXUS_ModuleSettings *settings)
{
    nexus_driver_proc_unregister_status(name);
}

/**
client management

a client is added to one of three lists. it cannot be in two lists at the same time.

the three lists are:
    nexus_driver_state.allowed_clients - client is explicitly registered by server, but not opened
        client->refcnt == 0 && client->pid == 0
        only in this list if not open and cleanup has been performed.
    nexus_driver_state.clients - open clients
        client->refcnt > 0 && client->pid != 0
        this client is in use.
    nexus_driver_state.cleanup_clients - closed clients that need object cleanup
        client->pid == 0
        if client->refcnt > 0, then we must wait for drivers to close
        if client->refcnt == 0, this will be finalized, either by deleting or returning to allowed_clients list

some notes:
it is assumed that NEXUS_Platform_Init/Uninit will be called non-reentrantly.
NEXUS_Platform_Init or NEXUS_Platform_Uninit calls may fail. use nexus_driver_state.active to know if nexus is up or not.
server cannot call NEXUS_Platform_Uninit during close with signal_pending(current).
clients are all closed and prevented from opening while server uninit is in progress.
however, once server uninit is complete, clients may started opening before server init is done.
**/

BDBG_OBJECT_ID(nexus_driver_client_state);

static struct nexus_driver_client_state *
nexus_driver_open_client_lock(unsigned pid)
{
    struct nexus_driver_client_state *client;

    BDBG_MSG_TRACE(("nexus_driver_open_client(%d)", pid));

    if (nexus_driver_state.uninit_in_progress) {
        client = NULL;
        goto done;
    }

    /* verify the pid is not currently in the cleanup list */
    for (client = BLST_S_FIRST(&nexus_driver_state.cleanup_clients); client; client = BLST_S_NEXT(client, link)) {
        if (client->pid == pid) {
            BDBG_ERR(("client %d cannot be started again until cleaned up", pid));
            client = NULL;
            goto done;
        }
    }

    /* check if already open */
    for (client = BLST_S_FIRST(&nexus_driver_state.clients); client; client = BLST_S_NEXT(client, link)) {
        if (client->pid == pid) {
            BDBG_MSG_TRACE(("  found %#x in open clients", client));
            break;
        }
    }

    /* if not open, check if allowed */
    if (!client) {
        for (client = BLST_S_FIRST(&nexus_driver_state.allowed_clients); client; client = BLST_S_NEXT(client, link)) {
            if (client->pid == pid) {
                /* move from allowed to open */
                BDBG_ASSERT(client->list == nexus_driver_allowed_clients_list);
                BLST_S_REMOVE(&nexus_driver_state.allowed_clients, client, nexus_driver_client_state, link);
                BLST_S_INSERT_HEAD(&nexus_driver_state.clients, client, link);
                client->list = nexus_driver_clients_list;
                BDBG_MSG_TRACE(("  found %#x in allowed clients", client));
                break;
            }
        }
    }

    if (!client) {
        BDBG_MSG(("cannot find client for %d", pid));
    }
    else {
        BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
        client->refcnt++;
        BDBG_ASSERT(client->pid && client->pid == pid);
        /* if client->pid is set, then client must be in nexus_driver_state.clients */
    }

done:
    return client;
}

/*
this is called by the client when its refcnt is decremented to zero or when the server wants to force a client to be disabled.
after this, the client is unusable and any driver connected to it will fail.
add to cleanup_clients for finalization.
*/
static void
nexus_driver_disable_client_lock(struct nexus_driver_client_state *client)
{
    BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
    if (client->joined && client->client.verify == b_objdb_verify_none) {
        BDBG_ERR(("abnormal termination of an unprotected client %p is not supported", client));
        if (!nexus_driver_state.serverSettings.allowUnprotectedClientsToCrash) {
            BKNI_Fail();
        }
    }
    client->joined = false;
    if (client->pid) {
        if (client->pid != nexus_driver_state.server->pid && client->refcnt) {
            NEXUS_Platform_P_TerminateProcess(client->pid);
        }

        BDBG_ASSERT(client->list == nexus_driver_clients_list);
        BLST_S_REMOVE(&nexus_driver_state.clients, client, nexus_driver_client_state, link);
        client->pid = 0;
        BDBG_MSG(("disconnect client: %p", client));
        BLST_S_INSERT_HEAD(&nexus_driver_state.cleanup_clients, client, link);
        client->list = nexus_driver_cleanup_clients_list;
        nexus_driver_state.cleanup_pending = true;
    }
    
    nexus_driver_unlock_schedulers(client);
}

/* must be paired one-for-one with nexus_driver_open_client */
static void
nexus_driver_close_client_lock(struct nexus_driver_client_state *client)
{
    BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
    BDBG_MSG_TRACE(("nexus_driver_close_client: %p %d", client, client->refcnt));
    BDBG_ASSERT(client->refcnt);
    if (--client->refcnt == 0) {
        nexus_driver_disable_client_lock(client);
    }
}

struct nexus_driver_client_state *
nexus_driver_client_id(struct nexus_driver_module_driver_state *state)
{
    BDBG_OBJECT_ASSERT(state->client, nexus_driver_client_state);
    return state->client;
}

struct nexus_driver_client_state *
nexus_driver_create_client(const NEXUS_Certificate *pCertificate, const NEXUS_ClientConfiguration *pConfig)
{
    struct nexus_driver_client_state *client;
    LOCK();
    client = nexus_driver_create_client_lock(pCertificate, pConfig);
    UNLOCK();
    return client;
}

/**
nexus_driver_create_client is called in three places:
1) from NEXUS_Platform_RegisterClient for authenticated clients
2) from nexus_driver_connect_client from unauthenticated clients (pCertificate == NULL)
3) from nexus_driver_server_init for the server's "client" (pCertificate == NULL)
**/
static struct nexus_driver_client_state *
nexus_driver_create_client_lock(const NEXUS_Certificate *pCertificate, const NEXUS_ClientConfiguration *pConfig)
{
    struct nexus_driver_client_state *client;
    unsigned i;

    client = BKNI_Malloc(sizeof(*client));
    if (!client) return NULL;
    BKNI_Memset(client, 0, sizeof(*client));
    BDBG_OBJECT_SET(client, nexus_driver_client_state);
    
    client->client.resources.allowed = pConfig->resources;

    client->config = *pConfig;
    if (pCertificate) {
        /* this path is only for authenticated clients */
        client->certificate = *pCertificate;
    }
    switch (pConfig->mode) {
    case NEXUS_ClientMode_eUnprotected:
        client->client.verify = b_objdb_verify_none; /* TODO: support _parital for unprotected clients, but _none for the server */
        client->client.limited_api = false;
        break;
    case NEXUS_ClientMode_eProtected:
        client->client.verify = b_objdb_verify_partial; 
        client->client.limited_api = false;
        break;
    case NEXUS_ClientMode_eUntrusted:
        client->client.verify = b_objdb_verify_full; 
        client->client.limited_api = true;
        break;
    default:    
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_invalid_mode;
    }

    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_HeapHandle heap = client->config.heap[i];
        if (!heap) continue;
        
        if (!client->defaultHeap) {
            client->defaultHeap = heap;
        }
        if (!client->defaultFullyMappedHeap) {
            NEXUS_MemoryStatus status;
            int rc;
            rc = NEXUS_Heap_GetStatus(heap, &status);
            if (!rc) {
                if ((status.memoryType & NEXUS_MemoryType_eFull) == NEXUS_MemoryType_eFull) {
                    client->defaultFullyMappedHeap = heap;
                    break; /* we have both heaps */
                }
            }
        }
    }
    if (!client->defaultHeap) {
        if (pConfig->mode == NEXUS_ClientMode_eUnprotected) {
            /* for the server's client, NEXUS_Platform_Init has not been called yet, so no defaults heaps are available. this is
            handled in postinit. */    
            if (nexus_driver_state.server) {
                /* if unprotected and no heaps specified, give all heaps */
                BDBG_CASSERT(sizeof(client->config.heap) == sizeof(g_pCoreHandles->nexusHeap));
                BKNI_Memcpy(client->config.heap, g_pCoreHandles->nexusHeap, sizeof(client->config.heap));
                
                /* default to heap[0] which must exist and be eFull */
                client->defaultHeap = client->defaultFullyMappedHeap = g_pCoreHandles->nexusHeap[0];
            }
        }
        else {
            BDBG_ERR(("client %p has no default heap", client));
            goto err_no_default_heap;
        }
    }
    
    /* if dynamic is true, it can be deleted after it is closed */
    client->dynamic = (pCertificate == NULL);

    BLST_S_INSERT_HEAD(&nexus_driver_state.allowed_clients, client, link);
    client->list = nexus_driver_allowed_clients_list;

    BDBG_MSG(("create client: %p, cert length %d, %s", client, client->certificate.length, 
        pConfig->mode==NEXUS_ClientMode_eUnprotected?"unprotected":
        pConfig->mode==NEXUS_ClientMode_eProtected?"protected":"untrutsed"));
    BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
    return client;
    
err_no_default_heap:
err_invalid_mode:
    BDBG_OBJECT_DESTROY(client, nexus_driver_client_state);
    BKNI_Free(client);
    return NULL;
}

static void
nexus_driver_destroy_client_lock(struct nexus_driver_client_state *client)
{
    BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
    BDBG_ASSERT(!client->refcnt);
    if (client->list == nexus_driver_cleanup_clients_list) {
        return;
    }
    BDBG_MSG(("destroy client: %p", client));
    BDBG_ASSERT(client->list == nexus_driver_allowed_clients_list);
    BLST_S_REMOVE(&nexus_driver_state.allowed_clients, client, nexus_driver_client_state, link);
    BDBG_OBJECT_DESTROY(client, nexus_driver_client_state);
    BKNI_Free(client);
}

void
nexus_driver_destroy_client(struct nexus_driver_client_state *client)
{
    LOCK();
    BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
    if (client->refcnt) {
        /* cannot destroy now because client is still open, but server has unregistered, so we can destroy on final close */
        nexus_driver_disable_client_lock(client);
        client->dynamic = true;
    }
    else {
        nexus_driver_destroy_client_lock(client);
    }
    UNLOCK();
}

/*
process the cleanup_clients list.
this means searching the object database for handles.
if the refcnt hasn't fallen to zero, we have to leave it in the list.
*/
static void
nexus_driver_process_cleanup_queue_lock(void)
{
    struct nexus_driver_client_state *client;
    unsigned i;

    /* first, clean up object database. to preserve calling order, we must process per module, then per client */
    for(i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS-1;i++) {
        struct nexus_driver_module_header *header;
        header = g_nexus_driver_state_handlers[i].header;
        /* no objects to clean up in platform module. this would deadlock if run with an active client, so we avoid it. 
        another option is ensure platform lock is acquired, then pass NEXUS_MODULE_SELF to b_objdb so it can avoid a re-acquire */
        if (!header || !header->module || header->module == NEXUS_MODULE_SELF) {
            continue;
        }
        
        for (client = BLST_S_FIRST(&nexus_driver_state.cleanup_clients); client; client = BLST_S_NEXT(client, link)) {
            BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
            /* all clients in this list must be closed */
            BDBG_ASSERT(client->pid == 0);
            BDBG_MSG(("process cleanup queue: client %#x, module %s", client, header->name));
            b_objdb_uninit_client_objects(&header->objdb, &client->client);
        }
    }
    
    /* make a second pass to clean up clients */
    for (client = BLST_S_FIRST(&nexus_driver_state.cleanup_clients); client; ) {
        struct nexus_driver_client_state *next;
        next = BLST_S_NEXT(client, link);
            
        /* need to go per client, per header */
        for(i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS-1;i++) {
            struct nexus_driver_module_header *header;
            header = g_nexus_driver_state_handlers[i].header;
            if (!header || !header->module || header->module == NEXUS_MODULE_SELF) {
                continue;
            }
            
            /* deactivate all callbacks for this client, even if object not in database */
            b_objdb_uninit_client_callbacks(&header->objdb, &client->client);
        }

        if (!client->refcnt) {
            /* finalize */
            BDBG_ASSERT(client->list == nexus_driver_cleanup_clients_list);
            BLST_S_REMOVE(&nexus_driver_state.cleanup_clients, client, nexus_driver_client_state, link);
            BLST_S_INSERT_HEAD(&nexus_driver_state.allowed_clients, client, link);
            client->list = nexus_driver_allowed_clients_list;
            if (client->dynamic) {
                nexus_driver_destroy_client_lock(client);
            }
        }
        client = next;
    }
    nexus_driver_state.cleanup_pending = (BLST_S_FIRST(&nexus_driver_state.cleanup_clients) != NULL);
}

/* find client which is allowed, not open, and has no pid assigned */
static struct nexus_driver_client_state *
nexus_driver_connect_client_lock(unsigned pid, const NEXUS_Certificate *pCertificate)
{
    struct nexus_driver_client_state *client = NULL;

    BDBG_MSG(("nexus_driver_connect_client: pid=%d cert length %d", pid, pCertificate?pCertificate->length:0));
    if (pCertificate && pCertificate->length) {
        /* if you provide a certificate, it must match */
        for (client = BLST_S_FIRST(&nexus_driver_state.allowed_clients); client; client = BLST_S_NEXT(client, link)) {
            BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
            if (client->pid) {
                /* can only connect one process to a client */
                continue;
            }
            if (client->certificate.length == pCertificate->length &&
                !BKNI_Memcmp(&client->certificate, pCertificate, pCertificate->length))
            {
                client->pid = pid;
                break;
            }
        }
        if (!client) {
            BDBG_ERR(("unable to find certificate match"));
        }
    }
    else {
        /* cannot authenticate. does the server allow unauthenticated clients? */
        if (nexus_driver_state.serverSettings.allowUnauthenticatedClients) {
            client = nexus_driver_create_client_lock(NULL, &nexus_driver_state.serverSettings.unauthenticatedConfiguration);
            if (client) {
                client->pid = pid;
            }
            else {
                BDBG_ERR(("unable to create unauthenticated client"));
            }
        }
        else {
            BDBG_ERR(("connection of unauthenticated client will fail because server has not allowed it"));
        }
    }

    if (client) {
        BDBG_OBJECT_ASSERT(client, nexus_driver_client_state);
        BDBG_MSG(("connect client: %p pid=%d", client, client->pid));
    }
    else {
        BDBG_ERR(("nexus_driver_connect_client: rejecting client: pid=%d, cert length %d", pid, pCertificate?pCertificate->length:0));
    }

    return client;
}

int nexus_driver_module_heap_verify( struct nexus_driver_client_state *client, NEXUS_HeapHandle heap, NEXUS_HeapHandle *pHeap )
{
    unsigned i;

    /* if unprotected, no transform should be done */
    if (client->client.verify == b_objdb_verify_none) {
        *pHeap = heap;
        return 0;
    }

    /* protected/untrusted client */
    if (!heap) {
        /* module code will select the appropriate default heap using NEXUS_P_DefaultHeap or NEXUS_P_DefaultBoundsHeap */
        return 0;
    }

    /* verify client's heap */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (heap == client->config.heap[i]) {
            *pHeap = heap;
            return 0;
        }
    }

    BDBG_ERR(("client %p tried to use invalid heap %p, but was rejected", client, heap));
    return -1;
}

/*
nexus_driver_server_init is called when the first open is performed.
it cannot be done on insmod because the server's process (pid) is required
nexus_driver_server_init allows clients to be created on subsequent opens
nexus_driver_server_init is called before NEXUS_Platform_Init. the module handles are not open yet. therefore a nexus_driver_server_postinit is required.
*/
static int nexus_driver_server_init_lock(unsigned pid)
{
    if (!nexus_driver_state.server) {
        NEXUS_ClientConfiguration config;
        BKNI_Memset(&config, 0, sizeof(config));
        config.mode = NEXUS_ClientMode_eUnprotected;
        struct nexus_driver_client_state *client = nexus_driver_create_client_lock(NULL, &config);
        if (!client) return BERR_TRACE(-1);
        client->pid = pid;
        nexus_driver_state.server = client;
        b_objdb_set_default_client(&client->client);
    }
    return 0;
}

static int nexus_driver_server_preinit(void)
{
    int rc;
#ifdef NEXUS_CONFIG_IMAGE
    /* need proxy's IMG thread to connect before calling NEXUS_Platform_Init.
    this must be called while LOCK() is not held. */
    rc = nexus_img_wait_until_ready();
    if (rc) return BERR_TRACE(rc);
#endif
    return rc;
}

/*
called after NEXUS_Platform_Init completes
this connects with all proxy thunks
*/
static int nexus_driver_server_postinit_lock(void)
{
    int rc = 0;
    unsigned i;
    struct nexus_driver_client_state *client = nexus_driver_state.server;
    
    /* connect with the thunk headers now the module handles are avaiable */
    for (i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS-1;i++) {
        rc = g_nexus_driver_state_handlers[i].open(i);
        if (rc) { rc = BERR_TRACE(rc); break; }
    }

    /* give the server's client all heaps */    
    BDBG_CASSERT(sizeof(client->config.heap) == sizeof(g_pCoreHandles->nexusHeap));
    BKNI_Memcpy(client->config.heap, g_pCoreHandles->nexusHeap, sizeof(client->config.heap));
    
    return rc;
}

/*
called before NEXUS_Platform_Uninit is called.
*/
static void nexus_driver_server_preuninit_lock(void)
{
    unsigned i;
    struct nexus_driver_client_state *client;

    if (nexus_driver_state.uninit_in_progress) {
        /* this check is needed because the NEXUS_Platform_Uninit ioctl may fail, which would cause
        nexus_driver_status.active to remain true. */
        return;
    }
    nexus_driver_state.uninit_in_progress = true;

    /* close all open clients */
    nexus_driver_disable_clients_lock(true);

    /* now we wait for every client's driver interface to close */
    while (1) {
        unsigned refcnt = 0;
        for (client = BLST_S_FIRST(&nexus_driver_state.clients); client; client = BLST_S_NEXT(client, link)) {
            if (client != nexus_driver_state.server) {
                refcnt += client->refcnt;
            }
        }
        for (client = BLST_S_FIRST(&nexus_driver_state.cleanup_clients); client; client = BLST_S_NEXT(client, link)) {
            if (client != nexus_driver_state.server) {
                refcnt += client->refcnt;
            }
        }
        if (!refcnt) break;
        UNLOCK();
        BKNI_Sleep(1);
        LOCK();
    }

    /* final client cleanup before nexus is uninitialized */
    nexus_driver_process_cleanup_queue_lock();
    
    for(i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS-1;i++) {
        g_nexus_driver_state_handlers[i].close();
        g_nexus_driver_state_handlers[i].header = NULL;
    }
}


/*
full shutdown
bring driver back to insmod'd state (as if there were no opens).
call NEXUS_Platform_Uninit if needed.
*/
static void nexus_driver_server_uninit_lock(void)
{
    struct nexus_driver_client_state *client;

    BDBG_MSG_TRACE((">nexus_driver_server_uninit %d", nexus_driver_state.active));
    BDBG_ASSERT(!nexus_driver_state.open_count);
    nexus_driver_state.uninit_pending = false;

    if (nexus_driver_state.active) {
        UNLOCK();
        NEXUS_Platform_Uninit();
        LOCK();
        BDBG_ASSERT(!nexus_driver_state.active);
        BDBG_ASSERT(nexus_driver_state.uninit_in_progress);
    }
    else {
        nexus_driver_state.uninit_in_progress = true;
    }
    BDBG_ASSERT(nexus_driver_state.server);

    /* one last cleanup. because all drivers are closed, this must delete all clients.
    the clients and cleanup_clients list must be empty afterwards. */
    BDBG_ASSERT(!BLST_S_FIRST(&nexus_driver_state.clients));
    nexus_driver_process_cleanup_queue_lock();
    BDBG_ASSERT(!BLST_S_FIRST(&nexus_driver_state.clients));
    BDBG_ASSERT(!BLST_S_FIRST(&nexus_driver_state.cleanup_clients));

    /* the only client left are the non-dynamic ones. we can destroy those too. */
    while ((client = BLST_S_FIRST(&nexus_driver_state.allowed_clients))) {
        nexus_driver_destroy_client_lock(client);
    }
    b_objdb_set_default_client(NULL);
    nexus_driver_state.server = NULL;
    nexus_driver_state.uninit_in_progress = false;

    BDBG_MSG_TRACE(("<nexus_driver_server_uninit"));
}

void nexus_generic_driver_read_register(uint32_t addr, uint32_t *p_value)
{
    NEXUS_Platform_ReadRegister(addr, p_value);
}

void nexus_generic_driver_write_register(uint32_t addr, uint32_t value)
{
    NEXUS_Platform_WriteRegister(addr, value);
}


void nexus_driver_get_client_configuration(void *client_id, NEXUS_ClientConfiguration *pSettings)
{
    struct nexus_driver_client_state *client;
    LOCK();
    for (client = BLST_S_FIRST(&nexus_driver_state.clients); client; client = BLST_S_NEXT(client, link)) {
        if (client_id == &client->client) {
            *pSettings = client->config;
            goto done;
        }
    }
    BDBG_ERR(("invalid client %p", client_id));
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
done:
    UNLOCK();
}

int nexus_driver_server_postinit(void)
{
    int rc;

    LOCK();    
    rc = nexus_driver_server_postinit_lock();
    UNLOCK();
    if (rc) return BERR_TRACE(rc);
    
    return 0;
}

void nexus_driver_server_preuninit(void)
{
    LOCK();
    nexus_driver_server_preuninit_lock();
    UNLOCK();
}
