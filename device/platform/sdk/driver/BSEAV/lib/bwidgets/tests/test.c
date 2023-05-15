/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: test.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 11/3/10 12:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/tests/test.c $
 * 
 * 5   11/3/10 12:40p erickson
 * SW7420-1148: update speed
 * 
 * 4   10/26/10 11:02a erickson
 * SW7420-1148: create client/server gfx app
 * 
 * 3   10/25/10 10:06a erickson
 * SWDEPRECATED-626: refactor, add image support
 * 
 * 2   10/22/10 3:21p erickson
 * SWDEPRECATED-626: revive bwidgets on nexus
 * 
 * 1   2/7/05 9:01p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   2/3/05 1:03p erickson
 * PR12110: simplified api and adapted to build system changes
 * 
 * Irvine_BSEAVSW_Devel/2   7/29/04 3:34p erickson
 * PR12110: added user input support, extended demo
 *
 * Irvine_BSEAVSW_Devel/1   7/29/04 2:07p erickson
 * PR12110: first pass, hello world working
 *
 ****************************************************************/
#include "bwidgets.h"
#include "b_app_server.h"
#include "b_app_client.h"
#include "nexus_platform.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

bwidget_t button1, button2, button3;

void user_input(void *context);
void idle_callback(bwidget_engine_t engine);

int main(void)
{
    bbutton_settings button_settings;
    bwidget_engine_settings widget_engine_settings;
    bwidget_engine_t widget_engine;
    NEXUS_Error rc;
    struct b_server_status server_status;
    struct b_offscreen_status offscreen_status;

    NEXUS_Platform_Init(NULL);
    
    rc = b_init_server(NEXUS_VideoFormat_eNtsc, &server_status);
    BDBG_ASSERT(!rc);

    rc = b_init_offscreen(720, 480, server_status.graphics.width, server_status.graphics.height, &offscreen_status);
    BDBG_ASSERT(!rc);
    offscreen_status.server = &server_status;

    printf("open widget_engine\n");
    widget_engine_settings.idle = idle_callback; /* we'll process user input at idle time */
    widget_engine = bwidget_engine_open(offscreen_status.win_engine, &widget_engine_settings);

    printf("create button1 in bwidgets\n");
    bbutton_get_default_settings(&button_settings);
    button_settings.label.widget.window.x = 50;
    button_settings.label.widget.window.y = 50;
    button_settings.label.widget.window.rect.width = 200;
    button_settings.label.widget.window.rect.height = 70;
    button_settings.label.widget.window.parent = offscreen_status.win_framebuffer_window;
    button_settings.label.text = "Hello World";
    button_settings.label.font = offscreen_status.default_font;
    button1 = bbutton_create(widget_engine, &button_settings);

    printf("create button2 in bwidgets\n");
    bbutton_get_default_settings(&button_settings);
    button_settings.label.widget.window.x = 300;
    button_settings.label.widget.window.y = 50;
    button_settings.label.widget.window.rect.width = 200;
    button_settings.label.widget.window.rect.height = 70;
    button_settings.label.widget.window.parent = offscreen_status.win_framebuffer_window;
    button_settings.label.text = "Button 2";
    button_settings.label.font = offscreen_status.default_font;
    button2 = bbutton_create(widget_engine, &button_settings);

    printf("create button3 in bwidgets\n");
    bbutton_get_default_settings(&button_settings);
    button_settings.label.widget.window.x = 50;
    button_settings.label.widget.window.y = 130;
    button_settings.label.widget.window.rect.width = 200;
    button_settings.label.widget.window.rect.height = 70;
    button_settings.label.widget.window.parent = offscreen_status.win_framebuffer_window;
    button_settings.label.image = bwin_image_load(offscreen_status.win_engine, "pictures/sunset.jpg");
    button_settings.label.text = "Button 2";
    button_settings.label.text_color = 0xFF000000;
    button_settings.label.font = offscreen_status.default_font;
    button3 = bbutton_create(widget_engine, &button_settings);

    printf("set focus\n");
    bwidget_set_focus(button3);

    bwin_add_io_handle(offscreen_status.win_engine, (bwin_io_handle)1);

    /* This is the main event loop for the application. It processes bwin
    events and redraws the widgets. You're done. */
    printf("run the gui\n");
    bwidget_run(widget_engine);

    b_uninit_offscreen(&offscreen_status);
    b_uninit_server(&server_status);
    NEXUS_Platform_Uninit();

    return 0;
}

/* settop api callback */
void user_input(void *context)
{
    bwin_engine_t win_engine = (bwin_engine_t)context;
    /* interrupt the bwin event loop. this will interrupt the sleep and
    cause the ui to be more responsive. */
    bwin_trigger_io(win_engine, (bwin_io_handle)1);
}

void idle_callback(bwidget_engine_t engine)
{
    BSTD_UNUSED(engine);
}
