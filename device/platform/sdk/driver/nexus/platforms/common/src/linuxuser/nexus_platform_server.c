/***************************************************************************
 *     (c)2004-2013 Broadcom Corporation
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
 * $brcm_Workfile: nexus_platform_server.c $
 * $brcm_Revision: SW7435-660/1 $
 * $brcm_Date: 2/21/13 4:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: Y:/stb/branch/scho_ursr124_SW7435_660/nexus/platforms/common/src/linuxuser/nexus_platform_server.c $
 * 
 * SW7435-660/1   2/21/13 4:45p scho
 * SW7435-660: use NEXUS_Thread_Create to limit stack size.
 * 
 * 69   11/5/12 2:31p erickson
 * SWSECURITY-202: make NEXUS_Platform_GetClientConfiguration work the
 *  same in user and kernel modes
 * 
 * 68   10/16/12 5:34p mward
 * SW7425-1792: don't use client on NEXUS_Platform_ReleaseObject
 * 
 * 67   10/12/12 11:57a erickson
 * SW7425-1792: don't use client on NEXUS_Platform_ReleaseObject
 * 
 * 66   10/10/12 12:57p vsilyaev
 * SW7425-1792: Fixed stub functions
 * 
 * 65   10/8/12 2:30p vsilyaev
 * SW7425-1792: Allow application to control life-cycle of objects
 * 
 * 64   9/17/12 3:04p erickson
 * SW7435-344: add NEXUS_OBJECT_REGISTER/UNREGISTER to syncthunk for
 *  usermode server
 * 
 * 63   9/14/12 2:53p erickson
 * SW7425-3418: don't try to recover callback state on EPIPE, convert
 *  LockModule to TryLockModule for middle of channel thread
 * 
 * 62   9/13/12 11:48a erickson
 * SW7425-3418: fix race condition between callback reader and writer. add
 *  protection to avoid incorrect wrap.
 * 
 * 61   9/5/12 9:58a erickson
 * SW7435-339: disconnect client on bad read from main socket. needed for
 *  internet-domain socket connections with NEXUS_WEBCPU mode.
 * 
 * 60   9/5/12 9:26a erickson
 * SW7435-339: no credentials for NEXUS_WEBCPU
 * 
 * 59   8/31/12 3:28p erickson
 * SW7425-3418: rename nexus_socket functions to avoid linker collision
 *  with os lib
 * 
 * 58   8/31/12 2:54p erickson
 * SW7425-3418: rework user mode callbacks: filter redundant callbacks on
 *  server. sync NEXUS_StopCallbacks across all callback threads.
 * 
 * 57   8/27/12 2:41p erickson
 * SW7425-3821: /shared requires 0777 access
 * 
 * 56   8/17/12 12:22p erickson
 * SW7435-303: replace server->mutex with platform module lock, which
 *  reduces unlock/lock holes
 * 
 * 55   8/16/12 4:35p erickson
 * SW7435-303: add NEXUS_Platform_GetClientStatus
 * 
 * 54   8/10/12 3:29p erickson
 * SW7435-303: fix mmap unregister and crash found during stress test
 * 
 * 53   8/7/12 10:35a erickson
 * SW7420-2332: fix race condition in b_objdb when cleaning up clients
 * 
 * 52   8/2/12 8:40a erickson
 * SW7435-290: add NEXUS_Profile_MarkThread calls for server threads
 * 
 * 51   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 *
 * 50   7/30/12 10:14a erickson
 * SW7420-2328: do not use BERR_TRACE when nexus not initialized
 *
 * 49   7/30/12 8:53a erickson
 * SW7435-239: Make the Nexus IPC UNIX domain socket location accessible
 *  to applications running on LXC
 *
 * 48   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 *
 * 47   7/9/12 1:58p erickson
 * SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
 *
 * 46   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 *
 * 45   5/11/12 2:45p erickson
 * SW7231-748: unlock required before platform calls can unregister
 *  clients from objdb
 *
 * 44   5/9/12 3:35p erickson
 * SW7231-748: fix #includes
 *
 * 43   5/9/12 3:18p erickson
 * SW7231-748: change #includes
 *
 * 42   5/8/12 5:33p erickson
 * SW7435-27: add webcpu support
 *
 * 41   2/15/12 10:13a erickson
 * SW7231-624: don't authenticate client against dynamically created
 *  client, changed several ERR's to WRN's
 *
 * 40   1/4/12 11:49a erickson
 * SW7420-1690: use chmod instead of umask to avoid race conditions in
 *  multithreaded system
 *
 * 39   1/3/12 3:52p erickson
 * SW7425-1364: fix comment
 *
 * 38   12/5/11 11:23a erickson
 * SW7420-2129: don't set default heap in nexus_driver_module_heap_verify.
 *  allow each module to set default using internal functions.
 *
 * 37   11/29/11 11:37a erickson
 * SW7420-2129: store default heap in nexus_driver_client_state
 *
 * 36   11/8/11 6:06p jgarrett
 * SW7422-400: Adding audio debug log support in kernel mode
 *
 * 35   11/3/11 6:53p jgarrett
 * SW7422-400: Enabling audio debug logs on 40nm APE chips
 *
 * 34   11/3/11 4:29p erickson
 * SW7420-1123: set server->done earlier. solves race condition on
 *  shutdown under stress.
 *
 * 33   11/1/11 3:25p erickson
 * SW7420-1690: set permissions to allow non-root users
 *
 * 32   10/24/11 11:23a erickson
 * SW7420-1690: add bcmdriver non-root mmap support
 *
 * 31   9/20/11 10:11a erickson
 * SW7420-1123: store entire client configuration
 *
 * 30   9/16/11 3:40p erickson
 * SW7420-2064: clean up unprotected callbacks when client exits, allow
 *  re-registration when client connects
 *
 * 29   9/13/11 1:23p erickson
 * SW7420-1148: change client modes to unprotected/protected/untrusted
 *
 * 28   8/29/11 11:44a erickson
 * SW7231-359: remove unnecessary lock/unlock in init/uninit server
 *
 * 27   8/29/11 10:53a erickson
 * SW7231-354: resolve coverity warning
 *
 * 26   8/25/11 2:02p erickson
 * SW7420-1148: only use defaultUntrustedHeap if untrusted
 *
 * 25   8/25/11 10:54a erickson
 * SW7420-1148: default server to using untrusted clients
 *
 * 24   8/22/11 2:16p erickson
 * SW7420-1148: print error if trusted client terminates abnormally.
 *  server can now specify defaultUntrustedHeap.
 *
 * 23   8/8/11 3:39p erickson
 * SW7420-1123: don't close connections on bad function call
 *
 * 22   8/5/11 5:14p erickson
 * SW7420-1123: it is normal that a module is compiled in but not
 *  initialized. don't fail in NEXUS_Platform_P_InitServer.
 *
 * 21   6/16/11 12:34p erickson
 * SW7425-459: prevent duplicate StartServer
 *
 * 20   4/11/11 12:06p erickson
 * SW7420-1671: speed up disconnect
 *
 * 19   4/8/11 12:34p erickson
 * SW7420-1671: NEXUS_Platform_P_InitServer is always called now
 *
 * 18   4/1/11 11:33a erickson
 * SW7420-1671: fix no NEXUS_SERVER_SUPPORT
 *
 * 17   4/1/11 9:59a erickson
 * SW7420-1671: refactor object database for multiprocess stress
 *
 * 16   3/25/11 12:10p erickson
 * SW7420-1123: server must pass heap status to avoid initial error from
 *  NEXUS_Heap_GetStatus
 *
 * 15   3/25/11 11:40a erickson
 * SW7420-1123: alloc ipc buffer per channel, not per channel connection
 *
 * 14   3/25/11 11:39a erickson
 * SW7420-1671: default trustUnauthenticatedClients = true
 *
 * 13   3/25/11 9:29a erickson
 * SW7420-1671: add
 *  NEXUS_PlatformStartServerSettings.trustUnauthenticatedClients
 *
 * 12   3/24/11 4:15p erickson
 * SW7420-1671: fix locking in NEXUS_Platform_StopServer
 *
 * 11   3/23/11 12:04p erickson
 * SW7420-1671: simplify nexus_driver_module_object_uninit params
 *
 * 10   3/23/11 12:00p erickson
 * SW7420-1671: fix DBG on failure, downgrade normal WRN's to MSG's
 *
 * 9   2/28/11 3:26p erickson
 * SW7420-1123: fix warning
 *
 * 8   2/28/11 3:11p erickson
 * SW7420-1123: refactor server init/uninit for simpler, cleaner app
 *  init/uninit
 *
 * 7   2/17/11 3:29p erickson
 * SW7420-1123: allow NULL
 *
 * 6   2/15/11 2:28p erickson
 * SW7420-1123: refactor IPC thunk to separate in/out params
 *
 * 5   2/10/11 5:09p erickson
 * SW7420-1123: refactor server/channel synchronization
 *
 * 4   1/24/11 3:57p erickson
 * SW7420-1123: improve shutdown
 *
 * 3   1/21/11 3:09p erickson
 * SW7420-1123: pass NEXUS_ClientConfiguration from server to client as
 *  part of initial connection, use it for client mmap
 *
 * 2   1/19/11 2:03p erickson
 * SW7420-1123: socket-based usermode IPC
 *
 * 1   12/7/10 5:47p erickson
 * SW7420-1148: refactor kernel mode proxy for process isolation
 *
 * 1   9/29/10 9:34a erickson
 * SW7420-1123: add linux usermode IPC
 *
 ************************************************************/
#include "nexus_platform_priv.h"
#include "nexus_platform_audio_log.h"

#if NEXUS_SERVER_SUPPORT
#include "nexus_driver.h"
#include "server/nexus_ipc_api.h"
#include "server/nexus_server_prologue.h"
#include "bcm_driver.h"
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#if NEXUS_WEBCPU
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <sys/un.h>
#endif
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h> /* see SIGPIPE below */
#endif

BDBG_MODULE(nexus_platform_server);

#if NEXUS_SERVER_SUPPORT

#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */

/**
Must extern all module-specific functions. We cannot build a master header file of all modules.
**/
#define NEXUS_PLATFORM_P_DRIVER_MODULE(X) \
    extern int nexus_server_##X##_open(struct nexus_driver_module_header **pHeader); \
    extern int nexus_server_##X##_close(void); \
    extern int nexus_server_##X##_process(void *driver_state, void *in_data, unsigned in_data_size, void *out_data, unsigned out_mem_size, unsigned *p_out_data_size);
#include "nexus_ipc_modules.h"
#undef NEXUS_PLATFORM_P_DRIVER_MODULE

/**
index of g_nexus_server_handlers[] is the module id.
**/
static const struct {
    const char *name;
    int (*open)(struct nexus_driver_module_header **pHeader);
    int (*close)(void);
    int (*process)(void *driver_state, void *in_data, unsigned in_data_size, void *out_data, unsigned out_mem_size, unsigned *p_out_data_size);
} g_nexus_server_handlers[] = {
#define NEXUS_PLATFORM_P_DRIVER_MODULE(X) \
    {#X, nexus_server_##X##_open, nexus_server_##X##_close, nexus_server_##X##_process},
#include "nexus_ipc_modules.h"
#undef NEXUS_PLATFORM_P_DRIVER_MODULE
};
#define NEXUS_PLATFORM_P_NUM_DRIVERS (sizeof(g_nexus_server_handlers)/sizeof(g_nexus_server_handlers[0]))

static struct nexus_driver_state {
    int16_t open_count;
    bool active;
    bool master_active;
    bool allow_unauthenticated_clients;

    struct nexus_driver_client_state *server; /* nexus_driver_open sets server_id = client_id of the server by calling nexus_driver_objects_init().
                        the server_id is required for non-verifiable functions like those that return structs with handles allocated inside the kernel.
                        it is also an optimization to skip verification for the trusted server. */

    BLST_S_HEAD(nexus_driver_client_list, nexus_driver_client_state) clients; /* allowed, open */
    BLST_S_HEAD(nexus_driver_allowed_client_list, nexus_driver_client_state) allowed_clients; /* allowed, but not open */
    BLST_S_HEAD(nexus_driver_cleanup_client_list, nexus_driver_client_state) cleanup_clients; /* allowed, closed, need to be cleaned up */
} nexus_driver_state;

BDBG_OBJECT_ID(nexus_driver_client_state);

BDBG_OBJECT_ID(b_server_callback_cxn);
struct b_server_callback_cxn {
    BDBG_OBJECT(b_server_callback_cxn)
    BLST_S_ENTRY(b_server_callback_cxn) link;
    struct b_server_callback *callback;
    struct NEXUS_Client *client;
    int fd;
    struct {
        unsigned wptr, rptr;
        unsigned id[NEXUS_CALLBACK_QUEUE_SIZE];
    } queue;
    BKNI_MutexHandle lock;
};

BDBG_OBJECT_ID(b_server_callback);
struct b_server_callback {
    BDBG_OBJECT(b_server_callback)
    int listen_fd;
    BLST_S_HEAD(callback_connections, b_server_callback_cxn) clients;
};

struct b_client_module_cxn;

/* each client connection to an ServerChannel */
BDBG_OBJECT_ID(b_client_module_cxn);
struct b_client_module_cxn
{
    BDBG_OBJECT(b_client_module_cxn)
    BLST_S_ENTRY(b_client_module_cxn) client_link;
    BLST_S_ENTRY(b_client_module_cxn) channel_link;
    struct NEXUS_ServerChannel *channel;
    NEXUS_ClientHandle client;
    int fd;
    int pollnum;
    struct nexus_driver_module_driver_state client_module_state; /* generic struct for nexus_driver_callbacks.c/nexus_driver_objects.c */
    bool deleted; /* deleted from outside the channel thread. */
};

/**
Threading model

there is one server thread and N channel threads.
the server has one mutex and each channel has a mutex.
if you must acquire both mutexes, you must acquire server, then channel - never channel, than server.
the channel thread processes the IPC functions. when making those IPC calls, it does not hold either the server or channel lock.
this is essential to preventing deadlock in this scenario:
1) server calls NEXUS_Platform_RegisterClient. this acquires: platform module lock, then server lock, then channel lock
2) channel proceses NEXUS_Platform IPC calls. this acquires: channel lock, copies any data locally for call, then releases the channel lock, then acquires platform module lock.

to make this work, deletion of client connections and clients is deferred. the server never deletes them directly while the channel thread is going.
instead, then channel thread cleans up connections when it is not processing an IPC calls (see nexus_cleanup_channel_connections_lock).
then, the server thread cleans up clients when all of the client connections have been cleaned up (see nexus_cleanup_clients).
**/

BDBG_OBJECT_ID(NEXUS_ServerChannel);
struct NEXUS_ServerChannel
{
    BDBG_OBJECT(NEXUS_ServerChannel)
    BLST_S_ENTRY(NEXUS_ServerChannel) link;
    unsigned moduleId;
    const char *moduleName;
    unsigned dataSize;
    void *in_data; /* size is dataSize */
    void *out_data; /* size is dataSize */

    struct nexus_driver_module_header *header; /* points to local storage in each module's thunk */
    struct NEXUS_Server *server;
    int listen_fd;
    NEXUS_ThreadHandle thread;
    BKNI_MutexHandle mutex;
    bool done;
    NEXUS_ModulePriority priority;

    /* list of client connections per module. protected by channel->mutex. */
    BLST_S_HEAD(channel_connection_list, b_client_module_cxn) clients;
};

BDBG_OBJECT_ID(NEXUS_Client);
struct NEXUS_Client
{
    BDBG_OBJECT(NEXUS_Client)
    BLST_S_ENTRY(NEXUS_Client) link;
    NEXUS_ClientSettings settings;
    struct NEXUS_Server *server;
    unsigned pid;
    int fd;
    int pollnum;
    struct nexus_driver_client_state client_state; /* generic struct for nexus_driver_callbacks.c/nexus_driver_objects.c */
    bool dynamicallyCreated;
    bool deleted;
    unsigned numJoins;

    struct b_server_callback_cxn callback_cxn[NEXUS_ModulePriority_eMax];

    /* list of client connections for this client */
    BLST_S_HEAD(connection_list, b_client_module_cxn) connections;
};

BDBG_OBJECT_ID(NEXUS_Server);
struct NEXUS_Server
{
    BDBG_OBJECT(NEXUS_Server)
    NEXUS_PlatformStartServerSettings settings;

    struct nexus_driver_client_state server_client_state; /* needed when server needs to interact w/ object database as a client */
    int listen_fd;
    NEXUS_ThreadHandle thread;
    bool done;
    struct b_server_callback callback[NEXUS_ModulePriority_eMax];

    BLST_S_HEAD(channel_list, NEXUS_ServerChannel) channels;
    BLST_S_HEAD(client_list, NEXUS_Client) clients;
};

static struct NEXUS_Server *g_server = NULL;

static void nexus_server_thread(void *context);
static void nexus_server_channel_thread(void *context);
static void nexus_unregister_client_lock(NEXUS_ClientHandle client);
static void nexus_autoclose_client_objects_lock(NEXUS_ClientHandle client);
static void nexus_cleanup_clients_lock(struct NEXUS_Server *server, bool *p_pending);
static void nexus_cleanup_channel_connections_lock(struct NEXUS_ServerChannel *channel);
static void nexus_platform_p_set_mmap_access(NEXUS_ClientHandle client, bool grant);

NEXUS_Error NEXUS_Platform_P_InitServer(void)
{
    struct NEXUS_Server *server;
    unsigned i;
    NEXUS_Error rc;

    /* this is required to keep magnum from terminating when a sockets are closed on abnormal termination.
    TODO: should the app be required to call this? */
    signal(SIGPIPE, SIG_IGN);

    server = BKNI_Malloc(sizeof(*server));
    if (!server) {
        return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(server, 0, sizeof(*server));
    BDBG_OBJECT_SET(server, NEXUS_Server);
    g_server = server;

    BDBG_OBJECT_SET(&server->server_client_state, nexus_driver_client_state);
    b_objdb_set_default_client(&server->server_client_state.client);

    nexus_driver_state.server = &server->server_client_state;

    /* init callback "callbacks", one per NEXUS_ModulePriority, shared for all modules & clients */
    for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
        BDBG_OBJECT_SET(&server->callback[i], b_server_callback);
        server->callback[i].listen_fd = -1;
    }

    /* TODO: consolidate some modules on a single thread */
    for (i=0;i<NEXUS_PLATFORM_P_NUM_DRIVERS;i++) {
        struct NEXUS_ServerChannel *channel;

        channel = BKNI_Malloc(sizeof(*channel));
        if (!channel) {
            rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            goto error;
        }
        BKNI_Memset(channel, 0, sizeof(*channel));
        BDBG_OBJECT_SET(channel, NEXUS_ServerChannel);

        rc = (g_nexus_server_handlers[i].open)(&channel->header);
        if (rc) {
            /* if the open fails, the module was likely compiled in, but not initialized. this is normal.
            we should only fail if someone tries to call one of the functions for that module. */
            BKNI_Free(channel);
            continue;
        }

        BDBG_ASSERT(channel->header);
        channel->moduleId = i;
        channel->moduleName = g_nexus_server_handlers[i].name;
        channel->dataSize = 4096; /* max size for single transaction with unix domain sockets */
        channel->in_data = BKNI_Malloc(channel->dataSize);
        if (!channel->in_data) {
            rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            BKNI_Free(channel);
            goto error;
        }
        channel->out_data = BKNI_Malloc(channel->dataSize);
        if (!channel->out_data) {
            rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            BKNI_Free(channel->in_data);
            BKNI_Free(channel);
            goto error;
        }

        BKNI_CreateMutex(&channel->mutex);
        channel->server = server;

        BLST_S_INSERT_HEAD(&server->channels, channel, link);
    }

    rc = NEXUS_Platform_P_InitAudioLog();
    if ( rc )
    {
        rc = BERR_TRACE(rc);
        goto error;
    }

    /* app must call NEXUS_Platform_StartServer to start threads */

    return 0;

error:
    NEXUS_Platform_P_UninitServer();
    BDBG_ASSERT(rc); /* failure path */
    return rc;
}

void NEXUS_Platform_P_UninitServer(void)
{
    struct NEXUS_ServerChannel *channel;
    struct NEXUS_Server *server = g_server;
    unsigned i;

    BDBG_OBJECT_ASSERT(server, NEXUS_Server);

    NEXUS_Platform_P_UninitAudioLog();

    if (server->thread) {
        /* implicit Stop */
        NEXUS_Platform_StopServer();
    }

    nexus_autoclose_client_objects_lock(NULL);

    /* close all channels */
    /* coverity[use_after_free] */
    while ((channel = BLST_S_FIRST(&server->channels))) {
        BDBG_OBJECT_ASSERT(channel, NEXUS_ServerChannel);
        BDBG_ASSERT(!channel->thread);
        BDBG_ASSERT(channel->server == server);
        BLST_S_REMOVE(&server->channels, channel, NEXUS_ServerChannel, link);
        BKNI_DestroyMutex(channel->mutex);
        BKNI_Free(channel->in_data);
        BKNI_Free(channel->out_data);
        nexus_driver_callback_uninit(channel->header);

        (g_nexus_server_handlers[channel->moduleId].close)();

        BDBG_OBJECT_DESTROY(channel, NEXUS_ServerChannel);
        BKNI_Free(channel);
    }

    for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
        BDBG_ASSERT(server->callback[i].listen_fd == -1);
        BDBG_OBJECT_DESTROY(&server->callback[i], b_server_callback);
    }

    b_objdb_set_default_client(NULL);
    BDBG_OBJECT_DESTROY(server, NEXUS_Server);
    BKNI_Free(server);

    g_server = NULL;
}

NEXUS_Error NEXUS_Platform_StartServer(const NEXUS_PlatformStartServerSettings *pSettings)
{
    struct NEXUS_ServerChannel *channel;
    struct NEXUS_Server *server = g_server;
    int rc;
    NEXUS_PlatformStartServerSettings defaultSettings;

    if (!server) {
        /* InitServer was not called */
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    if (server->thread) {
        /* server already started */
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    BDBG_OBJECT_ASSERT(server, NEXUS_Server);

    if (!pSettings) {
        NEXUS_Platform_GetDefaultStartServerSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }
    if (pSettings->allowUnprotectedClientsToCrash) {
        BDBG_WRN(("setting allowUnprotectedClientsToCrash = true leads to undefined and unsupported behavior"));
    }
    server->settings = *pSettings;

    BDBG_WRN(("NEXUS_Platform_StartServer %d modules", NEXUS_PLATFORM_P_NUM_DRIVERS));

    server->thread = NEXUS_Thread_Create("ipcserver", nexus_server_thread, server, NULL);
    if (!server->thread) {rc = BERR_TRACE(NEXUS_UNKNOWN); goto error;}

    /* TODO: consolidate some modules on a single thread */
    for (channel = BLST_S_FIRST(&server->channels); channel; channel = BLST_S_NEXT(channel, link)) {
        BDBG_OBJECT_ASSERT(channel, NEXUS_ServerChannel);
        channel->thread = NEXUS_Thread_Create("ipcchannel", nexus_server_channel_thread, channel, NULL);
        if (!channel->thread) {rc = BERR_TRACE(NEXUS_UNKNOWN); goto error;}
    }

    BDBG_WRN(("server listening"));
    return 0;

error:
    NEXUS_Platform_StopServer();
    BDBG_ASSERT(rc); /* failure path */
    return rc;
}

void NEXUS_Platform_StopServer(void)
{
    struct NEXUS_ServerChannel *channel;
    struct NEXUS_Server *server = g_server;
    NEXUS_ClientHandle client;
    bool pending;

    BDBG_OBJECT_ASSERT(server, NEXUS_Server);

    BDBG_WRN(("NEXUS_Platform_StopServer"));
    server->done = true;

    /* unregister clients. they will be cleaned up when the channel threads exit.
    we unlock the platform module because b_objdb acquires the platform lock when cleaning up that module.
    in theory, all other calls into server code should be done at this point so it's safe. rework may be in order. */
    for (client = BLST_S_FIRST(&server->clients); client; client = BLST_S_NEXT(client, link)) {
        nexus_unregister_client_lock(client);
    }

    /* close all channels first. this stops all communication in-process. */
    for (channel = BLST_S_FIRST(&server->channels); channel; channel = BLST_S_NEXT(channel, link)) {
        BDBG_OBJECT_ASSERT(channel, NEXUS_ServerChannel);
        if (channel->thread) {
            channel->done = true;
            shutdown(channel->listen_fd, SHUT_RDWR); /* cause listening thread to wake up */
            NEXUS_UnlockModule();
            NEXUS_Thread_Destroy(channel->thread);
            NEXUS_LockModule();
            channel->thread = NULL;
            BDBG_ASSERT(channel->listen_fd == -1);
        }
    }

    /* terminate main server thread. this stops all new communication and the last thread. */
    if (server->thread) {
        shutdown(server->listen_fd, SHUT_RDWR); /* cause listening thread to wake up */
        NEXUS_UnlockModule();
        NEXUS_Thread_Destroy(server->thread);
        NEXUS_LockModule();
        server->thread = NULL;
        BDBG_ASSERT(server->listen_fd == -1);
    }
    nexus_cleanup_clients_lock(server, &pending);
    BDBG_ASSERT(!pending);
    
    /* NEXUS_Platform_P_UninitServer does cleanup */
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

static void nexus_cleanup_clients_lock(struct NEXUS_Server *server, bool *p_pending)
{
    NEXUS_ClientHandle client;

    BDBG_OBJECT_ASSERT(server, NEXUS_Server);
    *p_pending = false;

    /* if there are no more connections, then we can remove the client */
    for (client = BLST_S_FIRST(&server->clients); client; ) {
        NEXUS_ClientHandle next;

        BDBG_OBJECT_ASSERT(client, NEXUS_Client);
        next = BLST_S_NEXT(client, link);

        if (client->deleted && !BLST_S_FIRST(&client->connections)) {
            /* we can delete this client */
            unsigned i;

            BLST_S_REMOVE(&server->clients, client, NEXUS_Client, link);
            for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
                struct b_server_callback_cxn *callback_cxn = &client->callback_cxn[i];
                BDBG_OBJECT_ASSERT(callback_cxn, b_server_callback_cxn);
                if (callback_cxn->fd != -1) {
                    close(callback_cxn->fd);
                }
                BLST_S_REMOVE(&callback_cxn->callback->clients, callback_cxn, b_server_callback_cxn, link);
                BKNI_DestroyMutex(callback_cxn->lock);
            }

            BDBG_OBJECT_DESTROY(client, NEXUS_Client);
            BKNI_Free(client);
        }
        else if (client->deleted) {
            *p_pending = true;
        }

        client = next;
    }
}

static void nexus_autoclose_client_lock(struct NEXUS_Client *client)
{
    unsigned i;

    BDBG_OBJECT_ASSERT(client, NEXUS_Client);

    if (client->fd != -1) {
        close(client->fd);
        client->fd = -1;
    }

    /* disconnect callbacks */
    for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
        struct b_server_callback_cxn *callback_cxn = &client->callback_cxn[i];
        BDBG_OBJECT_ASSERT(callback_cxn, b_server_callback_cxn);
        if (callback_cxn->fd != -1) {
            BDBG_MSG(("disconnect client(%p) from callback(%d)", client, i));
            close(callback_cxn->fd);
            callback_cxn->fd = -1;
        }
    }
    if (client->dynamicallyCreated) {
        nexus_unregister_client_lock(client);
    }
    else {
        nexus_autoclose_client_objects_lock(client);
    }
}

typedef enum nexus_cxn_type
{
    nexus_cxn_type_main,
    nexus_cxn_type_module,
    nexus_cxn_type_scheduler
} nexus_cxn_type;

static int nexus_server_bind(nexus_cxn_type cxn_type, unsigned index)
{
#if NEXUS_WEBCPU
    int listen_fd;
    struct sockaddr_in sock_addr;
    int rc;
    int optval;

    /* set up server's main listener socket */
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        return BERR_TRACE(-1);
    }

    /* allow immediate reuse of sockets in TIME_WAIT state, at risk of receiving stale data */
    optval = 1;
    rc = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    if (rc) return BERR_TRACE(rc);

    BKNI_Memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    switch (cxn_type) {
    case nexus_cxn_type_main:
        sock_addr.sin_port = htons(5000);
        break;
    case nexus_cxn_type_module:
        sock_addr.sin_port = htons(5001+index);
        break;
    case nexus_cxn_type_scheduler:
        sock_addr.sin_port = htons(6001+index);
        break;
    }

    BDBG_MSG(("bind port %d for type %d, index %d", (int)ntohs(sock_addr.sin_port), cxn_type, index));
    rc = bind(listen_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
    if (rc == -1) {
        BDBG_WRN(("unable to bind port %d for type %d, index %d: errno %d", (int)ntohs(sock_addr.sin_port), cxn_type, index, errno));
        close(listen_fd);
        return BERR_TRACE(-1);
    }
#else
    int listen_fd;
    struct sockaddr_un sock_addr;
    int rc;

    /* set up server's main listener socket */
    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        return BERR_TRACE(-1);
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#else
    mkdir(B_UNIX_DIR, 0777);
    chmod(B_UNIX_DIR, 0777);
#endif

    BKNI_Memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    switch (cxn_type) {
    case nexus_cxn_type_main:
        strcpy(sock_addr.sun_path, B_UNIX_MAIN_SOCKET_STR);
        break;
    case nexus_cxn_type_module:
        sprintf(sock_addr.sun_path, B_UNIX_SOCKET_FORMAT_STR, index);
        break;
    case nexus_cxn_type_scheduler:
        sprintf(sock_addr.sun_path, B_UNIX_SCHEDULER_SOCKET_FMT_STR, index);
        break;
    }
    unlink(sock_addr.sun_path);

    rc = bind(listen_fd, (struct sockaddr *)&sock_addr, strlen(sock_addr.sun_path)+sizeof(sock_addr.sun_family));
    if (rc == -1) {
        close(listen_fd);
        return BERR_TRACE(-1);
    }

    /* allow non-root access */
    chmod(sock_addr.sun_path, 0666);
#endif

    rc = listen(listen_fd, 10);
    if (rc == -1) {
        close(listen_fd);
        return BERR_TRACE(-1);
    }

    return listen_fd;
}

static void nexus_server_thread(void *context)
{
    struct NEXUS_Server *server = context;
    int rc;
    unsigned i;

    NEXUS_Profile_MarkThread("nexus_server_thread");
    
    server->listen_fd = nexus_server_bind(nexus_cxn_type_main, 0);
    if (server->listen_fd == -1) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto done;
    }

    /* set up callback listener sockets */
    for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
        struct b_server_callback *callback = &server->callback[i];

        BDBG_OBJECT_ASSERT(callback, b_server_callback);

        callback->listen_fd = nexus_server_bind(nexus_cxn_type_scheduler, i);
        if (callback->listen_fd == -1) {
            rc = BERR_TRACE(NEXUS_UNKNOWN);
            return;
        }
    }

#define B_MAX_CLIENTS 16 /* required for poll() */

    BDBG_MSG(("server listening..."));
    while (!server->done) {
        struct pollfd fds[1+NEXUS_ModulePriority_eMax+B_MAX_CLIENTS];
        unsigned num = 0;
        struct NEXUS_Client *client;

        /* add main server listener */
        fds[num].fd = server->listen_fd;
        fds[num].events = POLLIN;
        num++;

        /* add callback listeners */
        for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
            fds[num].fd = server->callback[i].listen_fd;
            fds[num].events = POLLIN;
            num++;
        }

        /* wake up every second to see if app is exiting or client list has changed */
        while (!server->done) {
            bool pending;
            unsigned timeout = 1000;
            NEXUS_LockModule();
            nexus_cleanup_clients_lock(server, &pending);
            NEXUS_UnlockModule();
            if (pending) {
                timeout = 1; /* very short timeout if channel threads are cleaning up */
            }

            /* add clients to detect disconnects. note that client->pollnum cannot be zero if included in the poll. */
            NEXUS_LockModule();
            for (client = BLST_S_FIRST(&server->clients); num<1+NEXUS_ModulePriority_eMax+B_MAX_CLIENTS && client; client = BLST_S_NEXT(client, link)) {
                BDBG_OBJECT_ASSERT(client, NEXUS_Client);
                if (client->fd != -1) {
                    client->pollnum = num;
                    fds[num].fd = client->fd;
                    fds[num].events = POLLIN;
                    num++;
                }
            }
            NEXUS_UnlockModule();

            rc = poll(fds, num, timeout);
            if (rc < 0) {
                rc = BERR_TRACE(errno);
                goto done;
            }
            else if (rc > 0) {
                break;
            }
        }

        /* check if a client has disconnected */
        NEXUS_LockModule();
        for (client = BLST_S_FIRST(&server->clients); client; ) {
            struct NEXUS_Client *next;
            BDBG_OBJECT_ASSERT(client, NEXUS_Client);
            next = BLST_S_NEXT(client, link);
            if (client->fd != -1) {
                if (fds[client->pollnum].revents & (POLLERR|POLLHUP)) {
                    if (client->settings.configuration.mode == NEXUS_ClientMode_eUnprotected) {
                        BDBG_ERR(("abnormal termination of an unprotected client %p is not supported", client));
                        if (!server->settings.allowUnprotectedClientsToCrash) {
                            BKNI_Fail();
                        }
                    }
                    else {
                        BDBG_MSG(("abnormal close of unprotected client %p", client));
                    }
                    nexus_autoclose_client_lock(client);
                }
                else if (fds[client->pollnum].revents) {
                    enum nexus_main_socket_message_type msg;
                    /* getting a message from the main client socket. */
                    rc = b_nexus_read(client->fd, &msg, sizeof(msg));
                    if (rc == sizeof(msg)) {
                        switch (msg) {
                        case nexus_main_socket_message_type_disconnect:
                            BDBG_MSG(("clean close of client %p", client));
                            nexus_autoclose_client_lock(client);
                            break;
                        case nexus_main_socket_message_type_stop_callbacks:
                            {
                            void *interface;
                            rc = b_nexus_read(client->fd, &interface, sizeof(interface));
                            if (rc == sizeof(interface)) {
                                static unsigned stopCallbacksCount = 0; /* global id */
                                NEXUS_CallbackDesc callback;
                                callback.param = ++stopCallbacksCount; /* grab the next id. no sync needed. */
                                
                                /* stop production of callbacks for this interface */
                                NEXUS_StopCallbacks(interface);
                                
                                /* send "stop callbacks" id through every callback connection so client can sync */
                                BDBG_MSG_TRACE(("stop callbacks: %p, %d", client, callback.param));
                                for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
                                    nexus_server_send_callback(&client->callback_cxn[i], NULL, 0, &callback);
                                }
                                b_nexus_write(client->fd, &callback.param, sizeof(callback.param));
                            }
                            }
                            break;
                        case nexus_main_socket_message_type_start_callbacks:
                            {
                            void *interface;
                            rc = b_nexus_read(client->fd, &interface, sizeof(interface));
                            if (rc == sizeof(interface)) {
                                /* start production of callbacks for this interface */
                                NEXUS_StartCallbacks(interface);
                            }
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    else if (rc == -1) {
                        nexus_autoclose_client_lock(client);
                    }
                }
            }
            client = next;
        }
        NEXUS_UnlockModule();

        /* check listener for a new client or client callback connection */
        for (i=0;i<1+NEXUS_ModulePriority_eMax;i++) {
            if (fds[i].revents) {
                int fd;
                struct ucred credentials;
                socklen_t ucred_length = sizeof(struct ucred);

                fd = accept(i == 0 ? server->listen_fd : server->callback[i-1].listen_fd, NULL, NULL);
                if (fd < 0) {
                    if (!server->done) {
                        rc = BERR_TRACE(errno);
                    }
                    continue;
                }

                rc = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &credentials, &ucred_length);
                if (rc) {
                    BERR_TRACE(rc);
                    close(fd);
                }
                else {
                    /* credentials.pid is the process ID for this client */

                    if (i == 0) {
                        NEXUS_ClientAuthenticationSettings auth;
                        struct nexus_client_init_data init_data;

                        /* some manual ipc that matches nexus_platform_client.c code */
                        rc = b_nexus_read(fd, &auth, sizeof(auth));
                        if (rc != sizeof(auth)) {
                            BDBG_WRN(("unable to read authentication settings: %d", rc));
                            close(fd);
                            continue;
                        }

                        /* search already registered clients */
                        NEXUS_LockModule();
                        for (client = BLST_S_FIRST(&server->clients); client; client = BLST_S_NEXT(client, link)) {
                            if (client->fd == -1 && !client->dynamicallyCreated) {
                                if (!BKNI_Memcmp(&client->settings.authentication.certificate, &auth.certificate, sizeof(client->settings.authentication.certificate))) {
                                    /* found it */
                                    BDBG_MSG(("authenticated client %p for pid %d", client, credentials.pid));
                                    break;
                                }
                            }
                        }
                        NEXUS_UnlockModule();

                        /* if not found, and if unauthenticated clients are allowed, create a new one */
                        if (!client && server->settings.allowUnauthenticatedClients) {
                            NEXUS_ClientSettings clientSettings;
                            BDBG_MSG(("creating dynamic client for pid %d", credentials.pid));
                            NEXUS_Platform_GetDefaultClientSettings(&clientSettings);
                            clientSettings.configuration = server->settings.unauthenticatedConfiguration;
                            client = NEXUS_Platform_RegisterClient(&clientSettings);
                            if (client) {
                                client->dynamicallyCreated = true;
                            }
                        }

                        /* unable to authenticate or allow this client */
                        if (!client) {
                            BDBG_WRN(("rejected unauthenticated client %d connection", credentials.pid));
                            close(fd);
                            continue;
                        }

                        BKNI_Memset(&init_data, 0, sizeof(init_data));
                        init_data.config = client->settings.configuration;
                        for (i=0;i<NEXUS_MAX_HEAPS;i++) {
                            if (init_data.config.heap[i]) {
                                NEXUS_MemoryStatus status;
                                init_data.heap[i].heap = init_data.config.heap[i];
                                NEXUS_Heap_GetStatus(init_data.heap[i].heap, &status);
                                init_data.heap[i].offset  = status.offset;
                                init_data.heap[i].size = status.size;
                                init_data.heap[i].memoryType = status.memoryType;
                            }
                        }
                        rc = b_nexus_write(fd, &init_data, sizeof(init_data));
                        if (rc != sizeof(init_data)) {
                            BDBG_WRN(("unable to confirm client: %d", rc));
                            close(fd);
                            if (client->dynamicallyCreated) {
                                /* equivalent of NEXUS_Platform_UnregisterClient, without platform lock */
                                NEXUS_LockModule();
                                nexus_unregister_client_lock(client);
                                NEXUS_UnlockModule();
                            }
                            continue;
                        }

                        /* this client is ready */
                        client->pid = credentials.pid;
                        client->fd = fd;
                        client->numJoins++;

                        nexus_platform_p_set_mmap_access(client, true);
                        BDBG_MSG(("client %p (pid %d) ready", client, credentials.pid));
                    }
                    else {
                        struct NEXUS_Client *client;
                        unsigned priority = i-1;

                        /* new callback connection */
                        /* find the client */
                        NEXUS_LockModule();
                        for (client = BLST_S_FIRST(&server->clients); client; client = BLST_S_NEXT(client, link)) {
                            if (client->pid == (unsigned)credentials.pid) {
                                if (client->callback_cxn[priority].fd != -1) {
                                    BDBG_WRN(("client(%p) callback(%d) already connected", client, priority));
                                    client = NULL;
                                }
                                break;
                            }
                        }
                        NEXUS_UnlockModule();
                        if (client) {
                            int flags;
                            /* client has connected */
                            BDBG_MSG(("connect client(%p) to callback(%d)", client, priority));
                            client->callback_cxn[priority].fd = fd;
                            
                            /* must be non-blocking. this allows us to read back # of callbacks consumed without blocking, and
                            allows us to fire callbacks without client backing up server. */
                            flags = fcntl(fd, F_GETFL, 0);
                            fcntl(fd, F_SETFL, flags | O_NONBLOCK);
                        }
                        else {
                            close(fd);
                        }
                    }
                }
            }
        }
    }

done:
    close(server->listen_fd);
    server->listen_fd = -1;

    for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
        struct b_server_callback *callback = &server->callback[i];
        BDBG_OBJECT_ASSERT(callback, b_server_callback);
        close(callback->listen_fd);
        callback->listen_fd = -1;
    }

    return;
}

static void nexus_cleanup_channel_connections_lock(struct NEXUS_ServerChannel *channel)
{
    struct b_client_module_cxn *cxn;

    BDBG_OBJECT_ASSERT(channel, NEXUS_ServerChannel);
    BKNI_AcquireMutex(channel->mutex);
    for (cxn = BLST_S_FIRST(&channel->clients); cxn;) {
        struct b_client_module_cxn *next;

        BDBG_OBJECT_ASSERT(cxn, b_client_module_cxn);
        next = BLST_S_NEXT(cxn, channel_link);
        if (cxn->deleted) {
            /* remove from channel and from client */
            BLST_S_REMOVE(&channel->clients, cxn, b_client_module_cxn, channel_link);
            BDBG_OBJECT_ASSERT(cxn->client, NEXUS_Client);
            BLST_S_REMOVE(&cxn->client->connections, cxn, b_client_module_cxn, client_link);

            BDBG_OBJECT_DESTROY(cxn, b_client_module_cxn);
            BKNI_Free(cxn);
        }
        cxn = next;
    }
    BKNI_ReleaseMutex(channel->mutex);
}

/* this thread never acquires the server mutex */
static void nexus_server_channel_thread(void *context)
{
    struct NEXUS_ServerChannel *channel = context;
    int rc;

    NEXUS_Profile_MarkThread("nexus_server_channel_thread");
    
    channel->listen_fd = nexus_server_bind(nexus_cxn_type_module, channel->moduleId);
    if (channel->listen_fd == -1) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        return;
    }

    BDBG_MSG(("%s(%d) server channel listening...", channel->moduleName, channel->moduleId));
    while (!channel->done) {
        struct pollfd fds[B_MAX_CLIENTS+1];
        unsigned num = 0;
        struct b_client_module_cxn *cxn;

        /* add listener */
        fds[num].fd = channel->listen_fd;
        fds[num].events = POLLIN;
        num++;

        /* wake up every second to see if app is exiting */
        while (!channel->done) {
            if (NEXUS_TryLockModule()) {
                nexus_cleanup_channel_connections_lock(channel);
                NEXUS_UnlockModule();
            }

            /* add clients. note that client->pollnum cannot be zero if included in the poll. */
            BKNI_AcquireMutex(channel->mutex);
            for (cxn = BLST_S_FIRST(&channel->clients); num<B_MAX_CLIENTS && cxn; cxn = BLST_S_NEXT(cxn, channel_link)) {
                BDBG_OBJECT_ASSERT(cxn, b_client_module_cxn);
                if (cxn->fd != -1) {
                    cxn->pollnum = num;
                    fds[num].fd = cxn->fd;
                    fds[num].events = POLLIN;
                    num++;
                }
            }
            BKNI_ReleaseMutex(channel->mutex);

            rc = poll(fds, num, 1000);
            if (rc < 0) {
                rc = BERR_TRACE(errno);
                goto done;
            }
            else if (rc > 0) {
                break;
            }
        }

        /* check clients */
        BKNI_AcquireMutex(channel->mutex);
        for (cxn = BLST_S_FIRST(&channel->clients); cxn; cxn = BLST_S_NEXT(cxn, channel_link)) {
            BDBG_OBJECT_ASSERT(cxn, b_client_module_cxn);
            if (!cxn->deleted && cxn->fd != -1) {
                if (fds[cxn->pollnum].revents & (POLLERR|POLLHUP)) {
                    BDBG_OBJECT_ASSERT(cxn->channel, NEXUS_ServerChannel);
                    BDBG_OBJECT_ASSERT(cxn->client, NEXUS_Client);
                    BDBG_MSG(("disconnect client(%p) from module(%d)", cxn->client, cxn->channel->moduleId));
                    close(cxn->fd);
                    cxn->fd = -1;
                }
                else if (fds[cxn->pollnum].revents) {
                    /* we're going to make an IPC call. we must unlock, but this should be safe
                    because the cxn and client are guaranteed to not be destroyed here. */

                    BKNI_ReleaseMutex(channel->mutex);

                    /* process client message */
                    rc = b_nexus_read(cxn->fd, channel->in_data, channel->dataSize);
                    if (rc < 0) {
                        /* not necessarily an error. client may have closed, which is normal. */
                        BDBG_MSG(("read failed: %d %d", rc, errno));
                        close(cxn->fd);
                        cxn->fd = -1;
                    }
                    else if (rc) {
                        unsigned out_size;

                        /*BDBG_MSG_TRACE(("read %d", rc));*/
                        rc = (*g_nexus_server_handlers[channel->moduleId].process)(&cxn->client_module_state, channel->in_data, rc, channel->out_data, channel->dataSize, &out_size);
                        if (rc) {
                            BDBG_ERR(("client(%p,%p,%d) module(%d) call failed: %d", cxn->client, cxn, cxn->fd, cxn->channel->moduleId, rc));
                            /* write a 1 byte response, which will cause a failure on the client side, but keep connection open */
                            out_size = 1;
                        }

                        rc = b_nexus_write(cxn->fd, channel->out_data, out_size);
                        if (rc < 0) {
                            /* if we can't write a response, we need to close connection */
                            BDBG_MSG(("write failed: %d %d", rc, errno));
                            close(cxn->fd);
                            cxn->fd = -1;
                        }
                        /*BDBG_MSG_TRACE(("wrote %d out of %d", rc, channel->dataSize));*/
                    }

                    BKNI_AcquireMutex(channel->mutex);
                }
            }
        }
        BKNI_ReleaseMutex(channel->mutex);

        /* check listener for client module connection */
        if (fds[0].revents) {
            int fd;
            struct ucred credentials;
            socklen_t ucred_length = sizeof(struct ucred);

            fd = accept(channel->listen_fd, NULL, NULL);
            if (fd < 0) {
                if (!channel->done) {
                    rc = BERR_TRACE(errno);
                }
                continue;
            }

            /* find the client */
            rc = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &credentials, &ucred_length);
            if (rc) {
                BERR_TRACE(rc);
                close(fd);
            }
            else {
                BKNI_AcquireMutex(channel->mutex);
                for (cxn = BLST_S_FIRST(&channel->clients); cxn; cxn = BLST_S_NEXT(cxn, channel_link)) {
                    BDBG_OBJECT_ASSERT(cxn, b_client_module_cxn);
                    BDBG_OBJECT_ASSERT(cxn->client, NEXUS_Client);
                    if (!cxn->deleted && cxn->client->pid == (unsigned)credentials.pid) {
                        if (cxn->fd != -1) {
                            BDBG_ERR(("client(%p) module(%d) already connected", cxn->client, cxn->channel->moduleId));
                            cxn = NULL;
                        }
                        break;
                    }
                }
                if (cxn) {
                    /* client has connected */
                    BDBG_MSG(("connect client(%p) to module(%d)", cxn->client, cxn->channel->moduleId));
                    cxn->fd = fd;
                }
                else {
                    /* not necessarily an error */
                    BDBG_MSG(("no module connection for client pid %d", credentials.pid));
                    close(fd);
                }
                BKNI_ReleaseMutex(channel->mutex);
            }
        }
    }

done:
    close(channel->listen_fd);
    channel->listen_fd = -1;

    /* when the channel thread exists, all connections must be deleted */
    NEXUS_LockModule();
    nexus_cleanup_channel_connections_lock(channel);
    NEXUS_UnlockModule();
    return;
}

NEXUS_ClientHandle NEXUS_Platform_RegisterClient(const NEXUS_ClientSettings *pSettings)
{
    NEXUS_ClientHandle client;
    struct NEXUS_Server *server = g_server;
    struct NEXUS_ServerChannel *channel;
    unsigned i;

    BDBG_OBJECT_ASSERT(server, NEXUS_Server);
    BDBG_ASSERT(pSettings);

    client = BKNI_Malloc(sizeof(*client));
    if (!client) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto err_malloc;
    }
    BKNI_Memset(client, 0, sizeof(*client));
    BDBG_OBJECT_SET(client, NEXUS_Client);
    client->pid = 0;
    client->fd = -1;
    client->server = server;
    client->settings = *pSettings;
    BDBG_OBJECT_SET(&client->client_state, nexus_driver_client_state);
    client->client_state.client.resources.allowed = client->settings.configuration.resources;
    switch (client->settings.configuration.mode) {
    case NEXUS_ClientMode_eUnprotected:
        client->client_state.client.verify = b_objdb_verify_none; /* TODO: support _parital for unprotected clients, but _none for the server */
        client->client_state.client.limited_api = false;
        break;
    case NEXUS_ClientMode_eProtected:
        client->client_state.client.verify = b_objdb_verify_partial;
        client->client_state.client.limited_api = false;
        break;
    case NEXUS_ClientMode_eUntrusted:
        client->client_state.client.verify = b_objdb_verify_full;
        client->client_state.client.limited_api = true;
        break;
    default:
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_invalid_mode;
    }

    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_HeapHandle heap = client->settings.configuration.heap[i];
        if (!heap) continue;

        if (!client->client_state.defaultHeap) {
            client->client_state.defaultHeap = heap;
        }
        if (!client->client_state.defaultFullyMappedHeap) {
            NEXUS_MemoryStatus status;
            int rc;
            rc = NEXUS_Heap_GetStatus(heap, &status);
            if (!rc) {
                if ((status.memoryType & NEXUS_MemoryType_eFull) == NEXUS_MemoryType_eFull) {
                    client->client_state.defaultFullyMappedHeap = heap;
                    break; /* we have both heaps */
                }
            }
        }
    }
    if (!client->client_state.defaultHeap) {
        if (client->settings.configuration.mode == NEXUS_ClientMode_eUnprotected) {
            /* if unprotected and no heaps specified, give all heaps */
            NEXUS_PlatformConfiguration config;
            NEXUS_Platform_GetConfiguration(&config);
            BDBG_CASSERT(sizeof(client->client_state.config.heap) == sizeof(config.heap));
            BKNI_Memcpy(client->client_state.config.heap, config.heap, sizeof(client->client_state.config.heap));

            /* default to heap[0] which must exist and be eFull */
            client->client_state.defaultHeap = client->client_state.defaultFullyMappedHeap = config.heap[0];
        }
        else {
            BDBG_ERR(("client %p has no default heap", client));
            goto err_no_default_heap;
        }
    }

    BKNI_Memcpy(&client->client_state.config, &client->settings.configuration, sizeof(client->client_state.config));

    BLST_S_INSERT_HEAD(&server->clients, client, link);

    /* pre-create all callback connections */
    for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
        struct b_server_callback_cxn *callback_cxn = &client->callback_cxn[i];
        BDBG_OBJECT_SET(callback_cxn, b_server_callback_cxn);
        callback_cxn->fd = -1;
        callback_cxn->client = client;
        callback_cxn->callback = &server->callback[i];
        BLST_S_INSERT_HEAD(&callback_cxn->callback->clients, callback_cxn, link);
        BDBG_OBJECT_ASSERT(callback_cxn->callback, b_server_callback);
        BKNI_CreateMutex(&callback_cxn->lock);
    }

    /* pre-create all module connections */
    for (channel = BLST_S_FIRST(&server->channels); channel; channel = BLST_S_NEXT(channel, link)) {
        struct b_client_module_cxn *cxn;
        NEXUS_ModuleSettings moduleSettings;

        BDBG_OBJECT_ASSERT(channel, NEXUS_ServerChannel);
        cxn = BKNI_Malloc(sizeof(*cxn));
        if (!cxn) {
            BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            goto err_malloc_cxn;
        }
        BKNI_Memset(cxn, 0, sizeof(*cxn));
        BDBG_OBJECT_SET(cxn, b_client_module_cxn);
        cxn->fd = -1; /* will listen for this */
        cxn->client = client;
        cxn->channel = channel;
        cxn->client_module_state.client = &client->client_state; /* this is the object database's CLIENT_ID */
        NEXUS_Module_GetSettings(channel->header->module, &moduleSettings);
        cxn->client_module_state.slave_scheduler = (void*)&client->callback_cxn[moduleSettings.priority]; /* slave_scheduler is just a
            pointer to the correct callback connection so the callback can get routed to the right client with the right priority socket */

        BLST_S_INSERT_HEAD(&client->connections, cxn, client_link);
        BKNI_AcquireMutex(channel->mutex);
        BLST_S_INSERT_HEAD(&channel->clients, cxn, channel_link);
        BKNI_ReleaseMutex(channel->mutex);
    }

    BDBG_MSG(("registered client(%p)", client));

    return client;

err_malloc_cxn:
    nexus_unregister_client_lock(client);
    NEXUS_UnlockModule();
err_malloc:
    return NULL;

err_no_default_heap:
err_invalid_mode:
    BDBG_OBJECT_DESTROY(client, NEXUS_Client);
    BKNI_Free(client);
    return NULL;
}

/* NEXUS_Platform_UnregisterClient can be called for explicitly and dynamically registered clients */
void NEXUS_Platform_UnregisterClient( NEXUS_ClientHandle client )
{
    BDBG_OBJECT_ASSERT(client, NEXUS_Client);
    /* we unlock the platform module because b_objdb acquires the platform lock when cleaning up that module.
    because nothing else is done in this function, it is safe. */
    nexus_unregister_client_lock(client);
}

static void nexus_autoclose_client_objects_lock(NEXUS_ClientHandle client)
{
    /* auto-close all objects associated with this client */
    struct NEXUS_Server *server = g_server;
    struct NEXUS_ServerChannel *channel;
    struct b_objdb_client *objdb_client;

    if (client) {
        BDBG_OBJECT_ASSERT(client, NEXUS_Client);
        BDBG_MSG(("autoclose client(%p) objects", client));
         objdb_client = &client->client_state.client;
    }
    else {
        BDBG_MSG(("autoclose server objects"));
        objdb_client = &server->server_client_state.client;
    }

    /* clean up anything left open */
    for (channel = BLST_S_FIRST(&server->channels); channel; channel = BLST_S_NEXT(channel, link)) {
        struct nexus_driver_module_header *header;
        header = channel->header;
        if (!header || header->module == NEXUS_MODULE_SELF) {
            continue;
        }
        b_objdb_uninit_client_objects(&header->objdb, objdb_client);

        /* deactivate all entries for this client, even if object not in database */
        b_objdb_uninit_client_callbacks(&header->objdb, objdb_client);
    }
    
    if (client && client->pid) {
        nexus_platform_p_set_mmap_access(client, false);
    }
}

/* we cannot delete clients or connections in this function. we can only
mark them for deletion. */
static void nexus_unregister_client_lock(NEXUS_ClientHandle client)
{
    struct b_client_module_cxn *cxn;

    BDBG_OBJECT_ASSERT(client, NEXUS_Client);

    BDBG_MSG(("nexus_unregister_client_lock: %p %d", client, client->fd));
    if (client->fd != -1) {
        close(client->fd);
        client->fd = -1;
    }
    nexus_autoclose_client_objects_lock(client);
    client->deleted = true; /* nexus_cleanup_clients will delete once every connection is deleted */

    for (cxn = BLST_S_FIRST(&client->connections); cxn; cxn = BLST_S_NEXT(cxn, client_link)) {
        BDBG_OBJECT_ASSERT(cxn, b_client_module_cxn);
        cxn->deleted = true; /* nexus_cleanup_channel_connections_lock will delete connections */
        if (cxn->fd != -1) {
            close(cxn->fd);
            cxn->fd = -1;
        }
    }
}

extern int g_NEXUS_driverFd; /* from nexus_platform_os.c */

static void nexus_platform_p_set_mmap_access(NEXUS_ClientHandle client, bool grant)
{
    t_bcm_linux_mmap_access access;
    int rc;

    BKNI_Memset(&access, 0, sizeof(access));
#if !NEXUS_WEBCPU
/* for webcpu, we have no unix domain socket credentials, so we must allow everything */
    BDBG_ASSERT(client->pid);
#endif
    access.pid = client->pid;

    if (grant) {
        unsigned i,j;
        for (i=0,j=0;i<NEXUS_MAX_HEAPS && j<BCM_MAX_HEAPS;i++) {
            NEXUS_MemoryStatus status;
            if (client->client_state.config.heap[i]) {
                rc = NEXUS_Heap_GetStatus(client->client_state.config.heap[i], &status);
                if (rc) {rc = BERR_TRACE(rc); continue;}
                access.heap[j].offset = status.offset;
                access.heap[j].size = status.size;
                j++;
            }
        }
    }
    /* if !grant, memset(0) is sufficient to revoke all access. */

    rc = ioctl(g_NEXUS_driverFd, BRCM_IOCTL_SET_MMAP_ACCESS, &access);
    if (rc) rc = BERR_TRACE(rc); /* fall through. client mmap may fail, but server keeps going. */

    return;
}

/**
callbackId is a unique number per callback type and client. it can be used to filter redundant callbacks.
**/
void nexus_server_send_callback(void *slave, void *interface, unsigned callbackId, const NEXUS_CallbackDesc *pCallback)
{
    struct nexus_callback_data data;
    struct b_server_callback_cxn *callback_cxn = slave;
    int rc;
    unsigned i;

    BDBG_OBJECT_ASSERT(callback_cxn, b_server_callback_cxn);
    BDBG_OBJECT_ASSERT(callback_cxn->client, NEXUS_Client);

    /* if connection has closed, do not send */
    if (callback_cxn->fd == -1) {
        return;
    }

    /* read how many callbacks were last consumed by client. 
    this avoids firing redundant callbacks and filling the socket buffer (and blocking). */
    while (1) {
        uint8_t num;
        /* non-blocking read */
        rc = read(callback_cxn->fd, &num, sizeof(num));
        if (rc != sizeof(num)) {
            break;
        }
        BKNI_AcquireMutex(callback_cxn->lock);
        /* consume num from our queue */
        BDBG_MSG_TRACE(("client %p: consumed %d callback(s)", callback_cxn->client, (unsigned)num));
        
#define DEPTH(rptr,wptr,size) ((wptr)>=(rptr)?(wptr)-(rptr):(size)-(rptr)+(wptr))

        /* don't wrap if some bug occurs. */
        if (num > DEPTH(callback_cxn->queue.rptr,callback_cxn->queue.wptr, NEXUS_CALLBACK_QUEUE_SIZE)) {
            BDBG_ERR(("callback read race condition: %d %d %d", num, callback_cxn->queue.rptr, callback_cxn->queue.wptr));
            num = DEPTH(callback_cxn->queue.rptr,callback_cxn->queue.wptr, NEXUS_CALLBACK_QUEUE_SIZE);
        }
        
        callback_cxn->queue.rptr += num;
        if (callback_cxn->queue.rptr >= NEXUS_CALLBACK_QUEUE_SIZE) {
            callback_cxn->queue.rptr -= NEXUS_CALLBACK_QUEUE_SIZE;
        }
        BKNI_ReleaseMutex(callback_cxn->lock);
    }
    
    BKNI_AcquireMutex(callback_cxn->lock);
    if (interface) {
        /* determine if this callback is already queued. if so, don't queue again. if not, write. 
        the OS holds the callback queue, which keeps us from having to create threads on the server. */
        for (i=callback_cxn->queue.rptr;i!=callback_cxn->queue.wptr;) {
            if (callback_cxn->queue.id[i] == callbackId) {
                /* this callback is already queued. don't refire. */
                BDBG_MSG_TRACE(("callback_cxn %p discarded callback %d", callback_cxn, callbackId));
                BKNI_ReleaseMutex(callback_cxn->lock);
                return;
            }
            if (++i == NEXUS_CALLBACK_QUEUE_SIZE) {
                i = 0;
            }
        }
    }
    
    BDBG_MSG_TRACE(("client %p: send callback %p(%p,%d)", callback_cxn->client, pCallback->callback, pCallback->context, pCallback->param));
    data.interface = interface;
    data.callback = *pCallback;

    /* must add to the queue before releasing the mutex */    
    callback_cxn->queue.id[callback_cxn->queue.wptr] = callbackId;
    if (++callback_cxn->queue.wptr == NEXUS_CALLBACK_QUEUE_SIZE) {
        callback_cxn->queue.wptr = 0;
    }
    if (callback_cxn->queue.wptr == callback_cxn->queue.rptr) {
        /* increase NEXUS_CALLBACK_QUEUE_SIZE because we have more callbacks per priority */
        BDBG_ERR(("callback overflow for client %p, callback cxn %p (%d %d)", callback_cxn->client, callback_cxn, callback_cxn->queue.wptr, callback_cxn->queue.rptr));
    }
    BKNI_ReleaseMutex(callback_cxn->lock);
    
    /* TODO: consider a non-blocking write here so client cannot block server. would require retry logic. */
    rc = b_nexus_write(callback_cxn->fd, &data, sizeof(data));
    if (rc != sizeof(data)) {
        if (errno == EPIPE) {
            /* the connection is gone, so just be done */
            return;
        }
        /* this is an error because now we have to remove from the list, and there's no reliable way to do this */
        BDBG_ERR(("nexus_server_send_callback write failed: %d", rc, errno));
        BKNI_AcquireMutex(callback_cxn->lock);
        if (DEPTH(callback_cxn->queue.rptr,callback_cxn->queue.wptr, NEXUS_CALLBACK_QUEUE_SIZE)) {
            if (callback_cxn->queue.wptr) {
                callback_cxn->queue.wptr--;
            }
            else {
                callback_cxn->queue.wptr = NEXUS_CALLBACK_QUEUE_SIZE-1;
            }
        }
        BKNI_ReleaseMutex(callback_cxn->lock);
    }
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

NEXUS_Error NEXUS_Platform_GetClientStatus( NEXUS_ClientHandle client, NEXUS_ClientStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(client, NEXUS_Client);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->connected = (client->fd != -1);
    pStatus->numJoins = client->numJoins;
    return 0;
}

NEXUS_Error NEXUS_Platform_AcquireObject(NEXUS_ClientHandle client, const NEXUS_InterfaceName *type, void *object)
{
    return NEXUS_Platform_P_AcquireObject(&client->client_state.client, type, object);
}

void NEXUS_Platform_ReleaseObject(const NEXUS_InterfaceName *type, void *object)
{
    NEXUS_Platform_P_ReleaseObject(type, object);
    return;
}

#else /* NEXUS_SERVER_SUPPORT */

/* stub the public API */
NEXUS_Error NEXUS_Platform_P_InitServer(void)
{
#if NEXUS_AUDIO_DSP_DEBUG
    NEXUS_Error rc = NEXUS_Platform_P_InitAudioLog();
    if ( rc ) { return BERR_TRACE(rc); }
#endif
    return 0;
}
void NEXUS_Platform_P_UninitServer(void)
{
    NEXUS_Platform_P_UninitAudioLog();
}
NEXUS_Error NEXUS_Platform_StartServer(const NEXUS_PlatformStartServerSettings *pSettings)
{
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}
void NEXUS_Platform_StopServer(void)
{
}
NEXUS_ClientHandle NEXUS_Platform_RegisterClient( const NEXUS_ClientSettings *pSettings )
{
    BSTD_UNUSED(pSettings);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return NULL;
}
void NEXUS_Platform_UnregisterClient( NEXUS_ClientHandle client )
{
    BSTD_UNUSED(client);
}
NEXUS_Error NEXUS_Platform_GetClientStatus( NEXUS_ClientHandle client, NEXUS_ClientStatus *pStatus )
{
    BSTD_UNUSED(client);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Platform_AcquireObject(NEXUS_ClientHandle client, const NEXUS_InterfaceName *type, void *object)
{
    BSTD_UNUSED(client);
    BSTD_UNUSED(type);
    BSTD_UNUSED(object);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

void NEXUS_Platform_ReleaseObject(const NEXUS_InterfaceName *type, void *object)
{
    BSTD_UNUSED(type);
    BSTD_UNUSED(object);
    (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return;
}

#endif /* NEXUS_SERVER_SUPPORT */

/**
The following client functions only have meaning in the client, not in the server.
They exist here because the server is the superset of the client.
**/
NEXUS_Error NEXUS_Platform_AuthenticatedJoin(const NEXUS_ClientAuthenticationSettings *pSettings)
{
    BSTD_UNUSED(pSettings);
    BDBG_ERR(("You are calling client api's with a server-only Nexus library. Have you set NEXUS_MODE=client?"));
    return NEXUS_NOT_SUPPORTED;
}

void NEXUS_Platform_GetDefaultClientAuthenticationSettings( NEXUS_ClientAuthenticationSettings *pSettings )
{
    BSTD_UNUSED(pSettings);
}

void NEXUS_Platform_GetClientConfiguration( NEXUS_ClientConfiguration *pSettings )
{
    NEXUS_PlatformConfiguration config;
    
    /* syncthunk skips nexus_platform_client.h, so we manually lock */
    NEXUS_LockModule();
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    
    /* kernel mode server works like client, so we mimic that in user mode */
    NEXUS_Platform_GetConfiguration(&config);
    BKNI_Memcpy(pSettings->heap, config.heap, sizeof(pSettings->heap));
    NEXUS_UnlockModule();
}
