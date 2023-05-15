/***************************************************************************
*     (c)2008-2013 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_client.c $
* $brcm_Revision: SW7435-660-1/1 $
* $brcm_Date: 2/22/13 2:26p $
*
* API Description:
*   API name: Platform
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/linuxuser.client/nexus_platform_client.c $
* 
* SW7435-660-1/1   2/22/13 2:26p dongho
* SW7435-660-1: use NEXUS_Thread_Create to limit stack size
* 
* 15   9/5/12 4:45p erickson
* SW7435-339: default core1's address to core0 server to standard address
* 
* 14   8/31/12 3:27p erickson
* SW7425-3418: rename nexus_socket functions to avoid linker collision
*  with os lib
* 
* 13   8/31/12 2:54p erickson
* SW7425-3418: rework user mode callbacks: filter redundant callbacks on
*  server. sync NEXUS_StopCallbacks across all callback threads.
* 
* 12   7/17/12 11:12a erickson
* SW7435-235: refactor directories to remove symbolic links
* 
* 11   5/8/12 5:33p erickson
* SW7435-27: add webcpu support
* 
* 10   9/8/11 8:49a erickson
* SW7420-1123: clean shutdown of clients, fix call to
*  NEXUS_Base_Core_Uninit, improve client BDBG_ERR's to direct user to
*  look at server console
* 
* 9   8/22/11 2:16p erickson
* SW7420-1148: print error if trusted client terminates abnormally.
*  server can now specify defaultUntrustedHeap.
* 
* 8   5/18/11 11:00a erickson
* SW7420-1671: call shutdown() on socket before closing
* 
* 7   3/25/11 12:10p erickson
* SW7420-1123: server must pass heap status to avoid initial error from
*  NEXUS_Heap_GetStatus
*
* 6   2/16/11 2:17p erickson
* SW7420-1123: implement nelem/nelem_out for IPC thunk
*
* 5   2/15/11 2:28p erickson
* SW7420-1123: refactor IPC thunk to separate in/out params
*
* 4   1/21/11 3:45p erickson
* SW7420-1123: clean shutdown of client
*
* 3   1/21/11 3:09p erickson
* SW7420-1123: pass NEXUS_ClientConfiguration from server to client as
*  part of initial connection, use it for client mmap
*
* 2   1/19/11 2:03p erickson
* SW7420-1123: socket-based usermode IPC
*
* 1   9/29/10 9:34a erickson
* SW7420-1123: add linux usermode IPC
*
***************************************************************************/

#include "nexus_types.h"
#include "nexus_base.h"
#include "nexus_platform.h"
#include "nexus_platform_thunks.h"
#include "client/nexus_client_prologue.h"
#include "nexus_platform_client_impl.h"
#include "blst_slist.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#if NEXUS_WEBCPU
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <sys/un.h>
#endif
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */

BDBG_MODULE(nexus_platform_client);

BDBG_OBJECT_ID(NEXUS_P_Client);
BDBG_OBJECT_ID(NEXUS_P_ClientModule);

BDBG_OBJECT_ID(NEXUS_P_ClientCallback);
struct NEXUS_P_ClientCallback
{
    BDBG_OBJECT(NEXUS_P_ClientCallback)
    int fd;
    NEXUS_ThreadHandle thread;
    NEXUS_P_ClientHandle client;
    BKNI_MutexHandle callbackLock; /* held while callback is running */
    unsigned lastStopCallbacksCount;
};

BDBG_OBJECT_ID_DECLARE(NEXUS_P_Client);
struct NEXUS_P_Client
{
    BDBG_OBJECT(NEXUS_P_Client)
    BKNI_MutexHandle lock;
    int fd; /* main socket for connection to server */
    struct NEXUS_P_ClientCallback callback[NEXUS_ModulePriority_eMax];
};

BDBG_OBJECT_ID_DECLARE(NEXUS_P_ClientModule);
struct NEXUS_P_ClientModule
{
    BDBG_OBJECT(NEXUS_P_ClientModule)
    NEXUS_P_ClientHandle client;
    unsigned module_id;
    int fd; /* socket file descriptor */
    BKNI_MutexHandle lock; /* allows multithreaded calls on client */
    void *data;
    unsigned data_size; /* size of memory pointed to be data. not the amount of data within that memory. */
};

/* receive and fire callbacks from the server callbacks */
static void NEXUS_P_ClientCallbackThread(void *context)
{
    struct NEXUS_P_ClientCallback *callback = context;
    while (1) {
        struct nexus_callback_data data[NEXUS_CALLBACK_QUEUE_SIZE];
        int rc;
        unsigned i;
        uint8_t num;

        /* listen for callbacks */
        rc = b_nexus_read(callback->fd, data, sizeof(data));
        if (rc < 0 || rc%sizeof(data[0])!=0) {
            if (callback->client->fd == -1) break;
            BDBG_ERR(("schedule thread terminating: %d %d. check server console for error messages.", rc, errno));
            BERR_TRACE(rc);
            break;
        }
        num = rc/sizeof(data[0]);
        if (!num) continue;
        
        rc = b_nexus_write(callback->fd, &num, sizeof(num));
        if (rc != sizeof(num)) {
            BERR_TRACE(rc);
            break;
        }
        
        /* TEST: add BKNI_Sleep(rand()%50) here to verify StartCallbacks waits until pipeline empty */
        for (i=0;i<num;i++) {
            if (!data[i].interface) {
                BDBG_MSG_TRACE(("received stop callbacks: %d", data[i].callback.param));
                callback->lastStopCallbacksCount = data[i].callback.param;
            }
            else if (data[i].callback.callback) {
                BKNI_AcquireMutex(callback->callbackLock);
                /* TEST: add BKNI_Sleep(rand()%50) here to test proper thread synchronization */
                BDBG_MSG_TRACE(("%p calling: %p(%p,%d)", callback, data[i].callback.callback, data[i].callback.context, data[i].callback.param));
                (data[i].callback.callback)(data[i].callback.context, data[i].callback.param);
                BKNI_ReleaseMutex(callback->callbackLock);
            }
        }
    }
    return;
}

typedef enum nexus_cxn_type
{
    nexus_cxn_type_main,
    nexus_cxn_type_module,
    nexus_cxn_type_scheduler
} nexus_cxn_type;

static int nexus_client_connect(nexus_cxn_type cxn_type, unsigned index)
{
#if NEXUS_WEBCPU
    struct sockaddr_in sock_addr;
    int fd;
    int rc;
    const char *servername = NULL;
    
    /* set up server's main listener socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        return BERR_TRACE(-1);
    }

    BKNI_Memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    
    servername = NEXUS_GetEnv("servername");
    if (!servername) {
        /* hardcoded IP for core0/core1 bridge */
        servername = "192.168.250.1";
    }
    inet_aton(servername, &sock_addr.sin_addr);
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
    
    rc = connect(fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
    if (rc == -1) {
        close(fd);
        return BERR_TRACE(-1);
    }
    return fd;
#else
    int fd;
    struct sockaddr_un sock_addr;
    int rc;
    
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        return BERR_TRACE(-1);
    }

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
    rc = connect(fd, (struct sockaddr *)&sock_addr, strlen(sock_addr.sun_path)+sizeof(sock_addr.sun_family));
    if (rc == -1) {
        BDBG_ERR(("unable to connect to main ipc server. check server console for error messages."));
        close(fd);
        return BERR_TRACE(-1);
    }
    return fd;
#endif
}

NEXUS_P_ClientHandle NEXUS_P_Client_Init(const NEXUS_ClientAuthenticationSettings *pSettings, struct nexus_client_init_data *p_client_init_data)
{
    int rc;
    NEXUS_P_ClientHandle client;
    unsigned i;
    NEXUS_ClientAuthenticationSettings defaultSettings;

    client = BKNI_Malloc(sizeof(*client));
    if (!client) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    BKNI_Memset(client, 0, sizeof(*client));
    BDBG_OBJECT_SET(client, NEXUS_P_Client);
    BKNI_CreateMutex(&client->lock);

    for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
        struct NEXUS_P_ClientCallback *callback = &client->callback[i];
        BDBG_OBJECT_SET(callback, NEXUS_P_ClientCallback);
        callback->fd = -1; /* set proper default in case init goes wrong */
        BKNI_CreateMutex(&callback->callbackLock);
    }

    client->fd = nexus_client_connect(nexus_cxn_type_main, 0);
    if (client->fd == -1) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto error;
    }

    /* send authentication and see if we're accepted */
    if (!pSettings) {
        NEXUS_Platform_GetDefaultClientAuthenticationSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* this is some internal, manual IPC */
    rc = b_nexus_write(client->fd, pSettings, sizeof(*pSettings));
    if (rc != sizeof(*pSettings)) {
        BDBG_ERR(("unable to send authentication: %d. check server console for error messages.", rc));
        goto error;
    }
    rc = b_nexus_read(client->fd, p_client_init_data, sizeof(*p_client_init_data));
    if (rc != sizeof(*p_client_init_data)) {
        BDBG_ERR(("unable to receive authentication response: %d. check server console for error messages.", rc));
        goto error;
    }

    for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
        struct NEXUS_P_ClientCallback *callback = &client->callback[i];
        callback->client = client;
        
        callback->fd = nexus_client_connect(nexus_cxn_type_scheduler, i);
        if (callback->fd == -1) {
            rc = BERR_TRACE(NEXUS_UNKNOWN);
            goto error;
        }

        callback->thread = NEXUS_Thread_Create("nxcallback", NEXUS_P_ClientCallbackThread, callback, NULL);
		if (!callback->thread) {rc = BERR_TRACE(rc); goto error;}
    }

    return client;

error:
    NEXUS_P_Client_Uninit(client);
    return NULL;
}

void NEXUS_P_Client_Disconnect(NEXUS_P_ClientHandle client)
{
    BDBG_OBJECT_ASSERT(client, NEXUS_P_Client);
    if (client->fd != -1) {
        int fd = client->fd;
        enum nexus_main_socket_message_type msg;
        
        /* when we close the main socket, the server will close the callback sockets immediately,
        so we have to set client->fd = -1 before the close so that the callback thread knows it's a clean shutdown. */
        client->fd = -1;
        
        msg = nexus_main_socket_message_type_disconnect;
        b_nexus_write(fd, &msg, sizeof(msg));
        
        b_nexus_socket_close(fd);
    }
}

void NEXUS_P_Client_Uninit(NEXUS_P_ClientHandle client)
{
    unsigned i;

    BDBG_OBJECT_ASSERT(client, NEXUS_P_Client);

    for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
        struct NEXUS_P_ClientCallback *callback = &client->callback[i];
        if (callback->fd != -1) {
            b_nexus_socket_close(callback->fd);
        }
        if (callback->thread) {
            NEXUS_Thread_Destroy(callback->thread);
        }
        BKNI_DestroyMutex(callback->callbackLock);
        BDBG_OBJECT_UNSET(callback, NEXUS_P_ClientCallback);
    }
    if (client->lock) {
        BKNI_DestroyMutex(client->lock);
    }
    BDBG_OBJECT_DESTROY(client, NEXUS_P_Client);
    BKNI_Free(client);
}

NEXUS_P_ClientModuleHandle NEXUS_P_Client_InitModule(NEXUS_P_ClientHandle client, unsigned module_id, unsigned data_size)
{
    int rc;
    NEXUS_P_ClientModuleHandle module;

    module = BKNI_Malloc(sizeof(*module));
    if (!module) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    BKNI_Memset(module, 0, sizeof(*module));
    BDBG_OBJECT_SET(module, NEXUS_P_ClientModule);
    module->module_id = module_id;
    module->fd = -1;
    module->client = client;

    rc = BKNI_CreateMutex(&module->lock);
    if (rc) {
        rc = BERR_TRACE(rc);
        goto error;
    }

    module->fd = nexus_client_connect(nexus_cxn_type_module, module->module_id);
    if (module->fd == -1) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto error;
    }
    
    module->data_size = data_size;
    if (module->data_size) {
        module->data = BKNI_Malloc(module->data_size);
        if (!module->data) {
            rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            goto error;
        }
    }

    BDBG_MSG(("connect client module %d", module->module_id));
    return module;

error:
    NEXUS_P_Client_UninitModule(module);
    return NULL;
}

void NEXUS_P_Client_UninitModule(NEXUS_P_ClientModuleHandle module)
{
    BDBG_OBJECT_ASSERT(module, NEXUS_P_ClientModule);
    BDBG_MSG(("disconnect client module %d", module->module_id));
    if (module->fd != -1) {
        b_nexus_socket_close(module->fd);
    }
    if (module->data) {
        BKNI_Free(module->data);
    }
    if (module->lock) {
        BKNI_DestroyMutex(module->lock);
    }
    BDBG_OBJECT_DESTROY(module, NEXUS_P_ClientModule);
    BKNI_Free(module);
}

NEXUS_Error
NEXUS_P_Client_LockModule(NEXUS_P_ClientModuleHandle module, void **data, unsigned *size)
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(module, NEXUS_P_ClientModule);
    rc = BKNI_AcquireMutex(module->lock);
    if (rc) return BERR_TRACE(rc);
    *data = module->data;
    *size = module->data_size;
    return 0;
}

void
NEXUS_P_Client_UnlockModule(NEXUS_P_ClientModuleHandle module)
{
    BDBG_OBJECT_ASSERT(module, NEXUS_P_ClientModule);
    BKNI_ReleaseMutex(module->lock);
}

NEXUS_Error
NEXUS_P_Client_CallServer(NEXUS_P_ClientModuleHandle module, const void *in_params, unsigned in_param_size, void *out_params, unsigned out_param_mem, unsigned *p_out_param_size)
{
    int rc;

    BDBG_OBJECT_ASSERT(module, NEXUS_P_ClientModule);

    if (in_param_size > module->data_size) {
        return BERR_TRACE(-1);
    }

    /* BDBG_MSG_TRACE(("write %d", in_param_size)); */
    rc = b_nexus_write(module->fd, in_params, in_param_size);
    if (rc != (int)in_param_size) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    rc = b_nexus_read(module->fd, out_params, out_param_mem);
    if (rc < 0) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    /* BDBG_MSG_TRACE(("read %d", rc)); */
    *p_out_param_size = rc;

    return 0;

}

void NEXUS_P_Client_StopCallbacks(NEXUS_P_ClientHandle client, void *interfaceHandle)
{
    unsigned i;
    unsigned stopCallbacksCount;
    int rc;
    enum nexus_main_socket_message_type msg;
    
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BKNI_AcquireMutex(client->lock);
#endif
    /* stop callbacks on server and learn stopCallbacksCount to wait for */
    msg = nexus_main_socket_message_type_stop_callbacks;
    rc = b_nexus_write(client->fd, &msg, sizeof(msg));
    if (rc != sizeof(msg)) {
        BERR_TRACE(NEXUS_UNKNOWN);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
        BKNI_ReleaseMutex(client->lock);
#endif
        goto error;
    }
    rc = b_nexus_write(client->fd, &interfaceHandle, sizeof(interfaceHandle));
    if (rc != sizeof(interfaceHandle)) {
        BERR_TRACE(NEXUS_UNKNOWN);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
        BKNI_ReleaseMutex(client->lock);
#endif
        goto error;
    }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BKNI_ReleaseMutex(client->lock);
#endif
    
    rc = b_nexus_read(client->fd, &stopCallbacksCount, sizeof(stopCallbacksCount));
    if (rc != sizeof(stopCallbacksCount)) {
        BERR_TRACE(NEXUS_UNKNOWN);
        goto error;
    }
    
    /* here all relevant callbacks are marked as 'stopped'. now, we must synchronize with every callback thread
    to ensure this function returns with no stopped callback in flight. */
    while (1) {
        bool synced = true;
        for (i=0;i<NEXUS_ModulePriority_eMax;i++) {
            struct NEXUS_P_ClientCallback *callback = &client->callback[i];
            BDBG_OBJECT_ASSERT(callback, NEXUS_P_ClientCallback);
            BKNI_AcquireMutex(callback->callbackLock);
            if (callback->lastStopCallbacksCount < stopCallbacksCount) {
                synced = false;
            }
            BKNI_ReleaseMutex(callback->callbackLock);
        }
        if (synced) break;
    }
    
    return;
    
error:
    BDBG_ERR(("unable to stop callbacks on %p", interfaceHandle));
    return;
}

void NEXUS_P_Client_StartCallbacks(NEXUS_P_ClientHandle client, void *interfaceHandle)
{
    int rc;
    enum nexus_main_socket_message_type msg;
    
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BKNI_AcquireMutex(client->lock);
#endif
    /* start callbacks on server */
    msg = nexus_main_socket_message_type_start_callbacks;
    rc = b_nexus_write(client->fd, &msg, sizeof(msg));
    if (rc != sizeof(msg)) {
        BERR_TRACE(NEXUS_UNKNOWN);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
        BKNI_ReleaseMutex(client->lock);
#endif
        goto error;
    }
    rc = b_nexus_write(client->fd, &interfaceHandle, sizeof(interfaceHandle));
    if (rc != sizeof(interfaceHandle)) {
        BERR_TRACE(NEXUS_UNKNOWN);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
        BKNI_ReleaseMutex(client->lock);
#endif
        goto error;
    }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BKNI_ReleaseMutex(client->lock);
#endif
    return;
    
error:
    BDBG_ERR(("unable to start callbacks on %p", interfaceHandle));
    return;
}
