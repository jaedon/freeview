/******************************************************************************
 *    (c)2008-2013 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *****************************************************************************/
#include "platform_ipc_session.h"
#include "platform_ipc_client.h"
#include "ipc_stubs_dfb_server.h"
#include "blst_list.h"
#include <stdio.h>
#include <sys/poll.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#define DFB_MAX_CLIENTS 65

BDBG_MODULE(platform_dfb_ipc);

struct b_dfb_client;

struct brc_client {
    bool created;
    struct b_dfb_client *parent;
};
BDBG_OBJECT_ID(b_dfb_client);


static pthread_t ipc_thread_id;

typedef struct dfb_platform_server {
    BLST_D_HEAD(b_dfb_client_list, b_dfb_client) clients;
    BKNI_MutexHandle lock;
    struct {
        bool exit;
        bipc_t server;
    } ipc;
} *b_dfb_server_t;

typedef struct b_dfb_client {
    BDBG_OBJECT(b_dfb_client)
    BLST_D_ENTRY(b_dfb_client) link;
    struct {
        int fd;
        bipc_server_client_t client;
        DFB_PlatformNexusHandle dfbClient;
    } ipc;
    brc_client_client_name name;
    struct brc_client client;
    b_dfb_server_t server;

} *b_dfb_client_t;


static const bipc_server_descriptor * const ipc_interfaces [] = {
    &bipc_brc_client_server_descriptor
};

static struct dfb_platform_server g_server;

static b_dfb_client_t b_dfb_client_create(b_dfb_server_t server)
{
    b_dfb_client_t client;

    client = BKNI_Malloc(sizeof(*client));
    if(client==NULL) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    BKNI_Memset(client, 0, sizeof(*client));
    BDBG_OBJECT_SET(client, b_dfb_client);
    BLST_D_INSERT_HEAD(&server->clients, client, link);
    client->server = server;

    return client;

err_alloc:
    return NULL;
}

static void b_dfb_client_destroy(b_dfb_client_t client)
{
     BDBG_OBJECT_ASSERT(client, b_dfb_client);
    /*    b_refsw_client_shutdown(client); */
    BLST_D_REMOVE(&client->server->clients, client, link);
    BDBG_OBJECT_DESTROY(client, b_dfb_client);
    BKNI_Free(client);

    return;
}

static void b_dfb_ipc_client_destroy(b_dfb_server_t server, b_dfb_client_t client)
{
    bipc_server_client_destroy(server->ipc.server, client->ipc.client);
    close(client->ipc.fd);
    b_dfb_client_destroy(client);

    return;
}


static void *ipc_thread(void *context)
{
    b_dfb_server_t server = context;
    int listen_fd;
    int rc;
    struct sockaddr_un sock_addr;
    b_dfb_client_t client;

    BKNI_AcquireMutex(server->lock);
    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(listen_fd < 0) { perror("");rc=BERR_TRACE(errno); goto done; }

    BKNI_Memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    BDBG_ASSERT(sizeof(sock_addr.sun_path)>=sizeof(dfb_platform_ipc_endpoint));
    unlink(dfb_platform_ipc_endpoint);
    BKNI_Memcpy(sock_addr.sun_path, dfb_platform_ipc_endpoint, sizeof(dfb_platform_ipc_endpoint));

    fcntl(listen_fd, F_SETFL, O_NONBLOCK);

    rc = bind(listen_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr.sun_family)+sizeof(dfb_platform_ipc_endpoint));
    if(rc!=0) { perror("");rc=BERR_TRACE(errno); goto done; }
    /* allow non-root access */
    chmod(dfb_platform_ipc_endpoint, 0666);
    rc = listen(listen_fd, 10);
    if(rc!=0) { perror("");rc=BERR_TRACE(errno); goto done; }
    while(!server->ipc.exit) {
        struct pollfd fds[DFB_MAX_CLIENTS];
        b_dfb_client_t clients[DFB_MAX_CLIENTS];
        unsigned i,nfds,events;
        const unsigned timeout = 1000;

        for(client=BLST_D_FIRST(&server->clients),i=0;i<DFB_MAX_CLIENTS;i++,client=BLST_D_NEXT(client, link))  {
            fds[i].revents = 0;
            clients[i] = client;
            fds[i].events = POLLIN;
            if(client) {
                fds[i].fd = client->ipc.fd;
            } else {
                /* if reached B_MAX_CLIENTS, stop listening for new clients */
                fds[i].fd = listen_fd;
                i++;
                break;
            }
        }
        nfds = i;
        BKNI_ReleaseMutex(server->lock);
        BDBG_MSG(("poll %u", nfds));
        rc = poll(fds, nfds, timeout);
        BDBG_MSG(("poll %u->%d", nfds, rc));
        BKNI_AcquireMutex(server->lock);
        if(rc<0) { perror("");rc=BERR_TRACE(errno); goto done; }
        events = (unsigned)rc;
        for(i=0;i<nfds && events ;i++) {
            if(fds[i].revents & POLLIN) {
                events --;
                client = clients[i];
                if(client) {
                    rc = bipc_server_client_process(server->ipc.server, client->ipc.client);
                    if(rc!=0) {
                        BDBG_LOG(("closing client:%#lx with fd:%d ", (unsigned long)client, client->ipc.fd));
                        b_dfb_ipc_client_destroy(server, client);
                    }
                } else {
                    bipc_server_client_create_settings settings;

                    BDBG_ASSERT(listen_fd==fds[i].fd);
                    rc = accept(listen_fd, NULL, NULL);
                    if(rc<0) { perror("");(void)BERR_TRACE(errno); goto done;}
                    BDBG_LOG(("new client with fd:%d connected", rc));

                    client = b_dfb_client_create(server);
                    if(!client) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto done;}
                    bipc_server_get_default_client_create_settings(&settings);
                    client->ipc.fd = rc;
                    settings.recv_fd = rc;
                    settings.send_fd = rc;
                    settings.ipc_context = client;
                    fcntl(rc, F_SETFL, O_NONBLOCK);
                    client->ipc.client = bipc_server_client_create(server->ipc.server, &settings);
                    if(!client->ipc.client) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto done;}
                }
            }
        }
    }

    done:
    while(NULL!=(client=BLST_D_FIRST(&server->clients))) {
        b_dfb_ipc_client_destroy(server, client);
    }
    close(listen_fd);

    BKNI_ReleaseMutex(server->lock);
    return NULL;
}

brc_client_t  brc_client_create(bipc_t ipc,  const brc_client_client_configuration *config)
{
    b_dfb_client_t client = (void *)ipc;
    b_dfb_server_t server;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_OBJECT_ASSERT(client, b_dfb_client);
    if(client->client.created) {
        BDBG_WRN(("brc_client_create:%#lx (%d) client already created", (unsigned long)client, client->ipc.fd));
        return NULL;
    }

    client->name = config->name;
    client->name.string[sizeof(client->name.string)-1]='\0';
    BDBG_LOG(("client name %s ", client->name.string));

    server = client->server;
    client->client.created = true;
    client->client.parent = client;

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return &client->client;
}

DFB_PlatformResult   brc_client_get_settings(brc_client_t client, DFB_PlatformShared *shm)
{
    DFB_PlatformResult rc = DFB_PLATFORM_OK;

    BSTD_UNUSED(client);

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(shm);
    rc = DFB_Platform_P_GetShared( shm );
    if(rc)
    {
        BDBG_ERR(("%s: Failed to get shared settings.\n", __FUNCTION__));
        return rc;
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return rc;
}

void  brc_client_destroy(brc_client_t _client)
{
    b_dfb_client_t client = _client->parent;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_OBJECT_ASSERT(client, b_dfb_client);
/*    b_refsw_client_shutdown(client); */
    client->client.created = false;
    client->client.parent = NULL;

    return;

}
DFB_PlatformResult DFB_Platform_P_StartIpcServer(void)
{
    DFB_PlatformResult ret = DFB_PLATFORM_OK;
    b_dfb_server_t server = &g_server;
    bipc_server_create_settings ipc_settings;
    int rc;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    rc = BKNI_Init();
    if (rc)
    {
        BDBG_ERR(("Error during BKNI_Init()"));
        ret = DFB_PLATFORM_FAILURE;
        goto error_;
    }

    memset(server, 0, sizeof(&server));

    BKNI_CreateMutex(&server->lock);
    BLST_D_INIT(&server->clients);

    bipc_server_get_default_create_settings(&ipc_settings);
    ipc_settings.interfaces = ipc_interfaces;
    ipc_settings.interface_count = sizeof(ipc_interfaces)/sizeof(*ipc_interfaces);
    server->ipc.exit = false;
    server->ipc.server = bipc_server_create(&ipc_settings);
    if(!server->ipc.server)
    {
        BDBG_ERR(("Error during bipc_server_create()"));
        BKNI_Uninit();
        ret = DFB_PLATFORM_FAILURE;
    }
    BDBG_ASSERT(server->ipc.server);
    signal(SIGPIPE, SIG_IGN);

    if(pthread_create(&ipc_thread_id, NULL, ipc_thread, server))
    {
        BDBG_ERR(("Error during pthread_create()"));
        BKNI_Uninit();
        ret = DFB_PLATFORM_FAILURE;
    }
    BDBG_MSG_TRACE((">>%s ret: %d ", __FUNCTION__, ret));

error_:

    return ret;
}

void DFB_Platform_P_StopIpcServer(void)
{
    b_dfb_server_t server = &g_server;

    server->ipc.exit = true;
    pthread_join(ipc_thread_id, NULL);
    bipc_server_destroy(server->ipc.server);
    BKNI_DestroyMutex(server->lock);
    BKNI_Uninit();

    return;
}
