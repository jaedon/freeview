/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: b_app_client.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 11/3/10 12:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/tests/b_app_client.h $
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
 * 2   10/22/10 3:21p erickson
 * SWDEPRECATED-626: revive bwidgets on nexus
 * 
 ****************************************************************/
#ifndef B_APP_CLIENT_H__
#define B_APP_CLIENT_H__

#include "bwin.h"
#include "nexus_types.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************
Offscreen UI control
*****************************/

struct b_offscreen_status 
{
    NEXUS_Rect update_rect;
    NEXUS_SurfaceHandle surface; /* drawing surface */
    NEXUS_Graphics2DHandle blitter;
    BKNI_EventHandle checkpoint_event;
    
    NEXUS_SurfaceHandle back_surface[2]; /* double buffered back buffer*/
    unsigned current_back_surface; /* index to back_surface[] */
    
    bwin_engine_t win_engine;
    bwin_font_t default_font;
    bwin_framebuffer_t win_framebuffer;
    bwin_t win_framebuffer_window;
    
    void (*sync)(void *context);
    void *context;
    
    /* linked if the server */
    struct b_server_status *server;
};
    
int b_init_offscreen(unsigned width, unsigned height, unsigned backwidth, unsigned backheight, struct b_offscreen_status *status);
void b_uninit_offscreen(const struct b_offscreen_status *status);
void checkpoint(const struct b_offscreen_status *status);

/****************************
Client/server messaging
*****************************/

/**
General message api
**/
typedef struct b_message {
    unsigned type;
    unsigned datasize;
#define B_MAX_MSG 1000
    uint8_t data[B_MAX_MSG];
} b_message;

void b_message_init(b_message *msg);
int b_send_message(int fd, const b_message *msg);
int b_recv_message(int fd, b_message *msg);

/**
Specific messages
**/
enum b_message_type {
    /* from client to server */
    b_message_type_report_offscreen, /* payload: b_message_report_offscreen_data, 1 or 2 surfaces */
    b_message_type_offscreen_ready,  /* payload: b_message_index, index of the surface that is ready */
    
    /* from server to client */
    b_message_type_offscreen_done,   /* the surface given at b_message_offscreen_ready has been blitted
                                        payload: b_message_index, index of the surface which completed */
    b_message_type_activate,         /* client is visible on the server. */
    b_message_type_deactivate,       /* client is not visible on the server. */
    b_message_type_user_input,       /* payload: b_message_user_input_data */
    b_message_type_focus,            /* gain focus */
    b_message_type_blur,             /* lose focus, but still visible */
    
    b_message_max
};

struct b_message_report_offscreen_data {
    unsigned offset[2]; /* offset[1]!=0 means double buffered. all other params must be the same for both surfaces */
    unsigned width, height, pitch;
    NEXUS_PixelFormat pixelFormat;
};

/* multi-purpose message */
struct b_message_index {
    unsigned index;
};

void b_get_surface_data(const struct b_offscreen_status *status, b_message *msg);
NEXUS_Error b_create_surfaces(const struct b_message_report_offscreen_data *data, NEXUS_SurfaceHandle *pSurface0, NEXUS_SurfaceHandle *pSurface1);

struct b_message_user_input_data {
    unsigned code;
};

/* returns fd */
int b_connect_to_server(void);
void b_disconnect_from_server(int fd);

#ifdef __cplusplus
}
#endif

#endif
