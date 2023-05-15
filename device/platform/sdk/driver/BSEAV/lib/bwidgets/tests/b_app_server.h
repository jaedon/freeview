/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: b_app_server.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 11/3/10 12:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/tests/b_app_server.h $
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
#ifndef B_APP_SERVER_H__
#define B_APP_SERVER_H__

#include "bwin.h"
#include "nexus_types.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct b_client *b_client_t;

/**
server controls the framebuffer 
**/
struct b_server_status
{
    void (*client_changed)(void *context, b_client_t client);
    void *context;
    
    NEXUS_DisplayHandle display;
    struct {
        unsigned width, height;
    } graphics;
};

int b_init_server(
    NEXUS_VideoFormat format, 
    struct b_server_status *status
    );
void b_uninit_server(
    const struct b_server_status *status
    );

/**
server manages IPC connections to clients

This app-based IPC is different from nexus-based IPC.
It can be customized to app needs.
**/
int b_start_server(struct b_server_status *status);
void b_stop_server(void);

#define B_UNIX_SOCKET_NAME "/tmp/stb"

struct b_client {
    unsigned type; /* the authenticated type */
    NEXUS_SurfaceHandle surface[2]; /* server surface handles mapped to client memory */
    int current_surface; /* -1 means no current surface */
    
    /* private */
    int fd; /* socket */
    BLST_S_ENTRY(b_client) link;
    unsigned pollnum;
};

b_client_t b_get_client(unsigned type);
void b_release_client(b_client_t client, bool disconnect);
void b_complete_client(b_client_t client);

#ifdef __cplusplus
}
#endif

#endif
