/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: stb_gallery_client.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 11/3/10 12:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/tests/stb_gallery_client.c $
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
#include "bwidgets.h"
#include "b_app_client.h"
#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/time.h>

BDBG_MODULE(stb_gallery_client);

static int cnt = 0;
static char time_buffer[64];
static char counter_buffer[64];
bwidget_t background, time_label, counter_label;
struct b_offscreen_status offscreen_status;
int g_client_fd;

void idle_callback(bwidget_engine_t engine);
static void sync_fb(void *context);

int main(void)
{
    bbutton_settings button_settings;
    blabel_settings label_settings;
    bwidget_engine_settings widget_engine_settings;
    bwidget_engine_t widget_engine;
    NEXUS_Error rc;

    NEXUS_Platform_Join();
    
    rc = b_init_offscreen(640, 480, 640, 480, &offscreen_status);
    BDBG_ASSERT(!rc);
    offscreen_status.sync = sync_fb;
    offscreen_status.context = &offscreen_status;

    widget_engine_settings.idle = idle_callback; /* we'll process user input at idle time */
    widget_engine = bwidget_engine_open(offscreen_status.win_engine, &widget_engine_settings);

    bbutton_get_default_settings(&button_settings);
    button_settings.label.widget.window.x = 0;
    button_settings.label.widget.window.y = 0;
    button_settings.label.widget.window.rect.width = 720;
    button_settings.label.widget.window.rect.height = 480;
    button_settings.label.widget.window.parent = offscreen_status.win_framebuffer_window;
    button_settings.label.font = offscreen_status.default_font;
    background = bbutton_create(widget_engine, &button_settings);
    
    blabel_get_default_settings(&label_settings);
    label_settings.widget.window.x = 150;
    label_settings.widget.window.y = 80;
    label_settings.widget.window.rect.width = 180;
    label_settings.widget.window.rect.height = 40;
    label_settings.widget.window.parent = bwidget_win(background);
    label_settings.text = counter_buffer;
    label_settings.background_color = 0xFa006020;
    label_settings.bevel = 1;
    counter_buffer[0] = 0;
    label_settings.font = offscreen_status.default_font;
    counter_label = blabel_create(widget_engine, &label_settings);
    
    label_settings.widget.window.x += 200;
    label_settings.text = time_buffer;
    time_buffer[0] = 0;
    label_settings.font = offscreen_status.default_font;
    time_label = blabel_create(widget_engine, &label_settings);

    /* display a picture. because it's outside of the fast update region, it doesn't affect performance */
    blabel_get_default_settings(&label_settings);
    label_settings.widget.window.x = 150;
    label_settings.widget.window.y = 140;
    label_settings.widget.window.rect.width = 380;
    label_settings.widget.window.rect.height = 300;
    label_settings.widget.window.parent = bwidget_win(background);
    label_settings.image = bwin_image_load(offscreen_status.win_engine, "/mnt/nfs/erickson/media/server_images/pic2.jpg");
    blabel_create(widget_engine, &label_settings);

    g_client_fd = b_connect_to_server();
    BDBG_ASSERT(g_client_fd >= 0);
    
    {
        b_message msg;
        b_get_surface_data(&offscreen_status, &msg);
        rc = b_send_message(g_client_fd, &msg);
        BDBG_ASSERT(!rc);
    }

    /* add ability for this app to interrupt the bwin event sleep */
    bwin_add_io_handle(offscreen_status.win_engine, (bwin_io_handle)2);
    bwidget_run(widget_engine);
    
    b_disconnect_from_server(g_client_fd);

    b_uninit_offscreen(&offscreen_status);
    NEXUS_Platform_Uninit();

    return 0;
}

void idle_callback(bwidget_engine_t engine)
{
    struct timeval tv;
    static unsigned start_sec = 0;
    
    BSTD_UNUSED(engine);
    
    gettimeofday(&tv, NULL);
    if (!start_sec) {
        start_sec = tv.tv_sec;
    }
    tv.tv_sec -= start_sec;
    snprintf(time_buffer, 64, "Time: %ld:%02ld", tv.tv_sec/60, tv.tv_sec%60);
    bwin_repaint(bwidget_win(time_label), NULL);
    snprintf(counter_buffer, 64, "Count: %d", ++cnt);
    bwin_repaint(bwidget_win(counter_label), NULL);
    
    /* wake up bwin right away again. the client is going to go as fast as possible.
    we could slow it down so that every new frame is visible (not faster than the display refresh rate), but that's
    not the point of this app. */
    bwin_trigger_io(offscreen_status.win_engine, (bwin_io_handle)2);
}

static void sync_fb(void *context)
{
    b_message msg;
    struct b_offscreen_status *status = context;
        
    /* tell server that client's offscreen surface is ready */
    b_message_init(&msg);
    msg.type = b_message_type_offscreen_ready;
    ((struct b_message_index *)msg.data)->index = status->current_back_surface;
    msg.datasize = sizeof(struct b_message_index);
    b_send_message(g_client_fd, &msg);
    
    /* we are double-buffered, so we could offset our wait by one. but for this app it doesn't matter because
    the client is going AFAP. it might matter if the server waited to respond until the display vsync. */
    while (1) {
        int rc = b_recv_message(g_client_fd, &msg);
        if (rc) break; /* failure */
        
        if (msg.type == b_message_type_offscreen_done) {
            /* got msg */
            break;
        }
    }
}
