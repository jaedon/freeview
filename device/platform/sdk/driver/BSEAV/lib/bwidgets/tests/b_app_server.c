/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: b_app_server.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 11/3/10 12:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/tests/b_app_server.c $
 * 
 * 3   11/3/10 12:40p erickson
 * SW7420-1148: update speed
 * 
 * 2   10/27/10 11:17a erickson
 * SW7420-1148: update
 * 
 * 1   10/26/10 11:02a erickson
 * SW7420-1148: create client/server gfx app
 * 
 ****************************************************************/
#include "b_app_server.h"
#include "b_app_client.h"
#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "bwin.h"
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/un.h>
#include <unistd.h>

BDBG_MODULE(b_app_server);

int b_init_server(NEXUS_VideoFormat format, struct b_server_status *status)
{
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Error rc;

    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = format;
    status->display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
    
    if(platformConfig.outputs.component[0]){
        NEXUS_Display_AddOutput(status->display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    }
    if (videoFormatInfo.height <= 576) {
        NEXUS_Display_AddOutput(status->display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    }
    
    status->graphics.width = videoFormatInfo.width;
    status->graphics.height = videoFormatInfo.height;
    
    NEXUS_Display_GetGraphicsSettings(status->display, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.clip.width = status->graphics.width;
    graphicsSettings.clip.height = status->graphics.height;
    rc = NEXUS_Display_SetGraphicsSettings(status->display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    
    return 0;
}

void b_uninit_server(const struct b_server_status *status)
{
    NEXUS_Display_Close(status->display);
}

/**************************************************/

struct {
    int socket_fd;
    bool done;
    pthread_t listen_thread;
    /* TODO: need mutex for this list */
    BLST_S_HEAD(server_client_list, b_client) clients;
} g_server;

static void *listen_for_clients(void *context)
{
    struct sockaddr_un sock_addr;
    int rc;
    struct b_server_status *status = context;

    g_server.socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (g_server.socket_fd == -1) {
        rc = BERR_TRACE(errno);
        return 0;
    }
    
    unlink(B_UNIX_SOCKET_NAME);
 
    BKNI_Memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, B_UNIX_SOCKET_NAME);
    rc = bind(g_server.socket_fd, (struct sockaddr *)&sock_addr, strlen(sock_addr.sun_path)+sizeof(sock_addr.sun_family));
    if (rc == -1) {
        rc = BERR_TRACE(errno);
        goto done;
    }
 
    rc = listen(g_server.socket_fd, 10);
    if (rc == -1) {
        rc = BERR_TRACE(errno);
        goto done;
    }
 
    printf("server listening...\n");
    while (1) {
#define B_MAX_CLIENTS 16
        struct pollfd fds[B_MAX_CLIENTS+1];
        unsigned num = 0;
        b_client_t client;
        
        /* add listener */        
        fds[num].fd = g_server.socket_fd;
        fds[num].events = POLLIN;
        num++;
        
        /* add clients. note that client->pollnum cannot be zero if included in the poll. */
        for (client = BLST_S_FIRST(&g_server.clients); num<B_MAX_CLIENTS && client; client = BLST_S_NEXT(client, link)) {
            if (client->fd != -1) {
                client->pollnum = num;
                fds[num].fd = client->fd;
                fds[num].events = POLLIN;
                num++;
            }
        }
        
        /* wake up every second to see if app is exiting */
        while (!g_server.done) {
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
        for (client = BLST_S_FIRST(&g_server.clients); client; client = BLST_S_NEXT(client, link)) {
            if (client->pollnum) {
                if (fds[client->pollnum].revents & (POLLERR|POLLHUP)) {
                    close(client->fd);
                    client->fd = -1;
                    if (status->client_changed) {
                        (status->client_changed)(status->context, client);
                    }
                }
                else if (fds[client->pollnum].revents) {
                    b_message msg;
                    rc = b_recv_message(client->fd, &msg);
                    if (!rc) {
                        BDBG_MSG(("client %pd: type %d, data %d", (void*)client, msg.type, msg.datasize));
                        switch (msg.type) {
                        case b_message_type_report_offscreen:    
                            if (client->surface[0]) {
                                NEXUS_Surface_Destroy(client->surface[0]);
                                client->surface[0] = NULL;
                            }
                            if (client->surface[1]) {
                                NEXUS_Surface_Destroy(client->surface[1]);
                                client->surface[1] = NULL;
                            }
                            b_create_surfaces((struct b_message_report_offscreen_data *)msg.data, &client->surface[0], &client->surface[1]);
                            client->current_surface = -1;
                            printf("created surfaces %p, %p\n", (void*)client->surface[0], (void*)client->surface[1]);
                            break;
                        case b_message_type_offscreen_ready: 
                            client->current_surface = ((struct b_message_index *)msg.data)->index;
                            if (status->client_changed) {
                                (status->client_changed)(status->context, client);
                            }
                            break;    
                        default:
                            break;    
                        }
                    }
                }
            }
        }
        /* check listener */
        if (fds[0].revents) {
            int fd;
            
            fd = accept(g_server.socket_fd, NULL, NULL);
            if (fd < 0) {
                if (!g_server.done) {
                    rc = BERR_TRACE(errno);
                }
                goto done;
            }
            
            /* TODO: add authetication, set client->type before inserting into the list */
            
            client = BKNI_Malloc(sizeof(*client));
            BKNI_Memset(client, 0, sizeof(*client));
            printf("client %p connected\n", (void*)client);
            client->fd = fd;
            BLST_S_INSERT_HEAD(&g_server.clients, client, link);
        }
    }
 
done:
    close(g_server.socket_fd);
    /* TODO: close all */
    return 0;
}

int b_start_server(struct b_server_status *status)
{
    int rc;
    g_server.done = false;
    rc = pthread_create(&g_server.listen_thread, NULL, listen_for_clients, status);
    return rc;
}

void b_stop_server(void)
{
    g_server.done = true;
    close(g_server.socket_fd);
    pthread_join(g_server.listen_thread, NULL);
}

b_client_t b_get_client(unsigned type)
{
    b_client_t client;
    for (client = BLST_S_FIRST(&g_server.clients); client; client = BLST_S_NEXT(client, link)) {
        if (client->type == type && client->fd != -1) {
            return client;
        }
    }
    return NULL;
}

void b_release_client(b_client_t client, bool disconnect)
{
    /* only delete if disconnected */
    if (client->fd == -1 || disconnect) {
        BLST_S_REMOVE(&g_server.clients, client, b_client, link);
        if (client->fd) {
            close(client->fd);
        }
        if (client->surface[0]) {
            NEXUS_Surface_Destroy(client->surface[0]);
        }
        if (client->surface[1]) {
            NEXUS_Surface_Destroy(client->surface[1]);
        }
        BKNI_Free(client);
    }
    /* else, we could keep track of which clients have been gotten by the server */
}

void b_complete_client(b_client_t client)
{
    if (client->fd != -1 && client->current_surface != -1) {
        b_message msg;
        b_message_init(&msg);
        msg.type = b_message_type_offscreen_done;
        msg.datasize = sizeof(struct b_message_index);
        
        /* after blit to server's fb completes, we can notify the client that we're done w/ its offscreen surface */
        ((struct b_message_index *)msg.data)->index = client->current_surface;
        b_send_message(client->fd, &msg);
        client->current_surface = -1;
    }
}
