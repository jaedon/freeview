/***************************************************************************
 *     Copyright (c) 2004-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: test_draw.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 11/15/06 11:00a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/utils/test_draw.c $
 * 
 * 5   11/15/06 11:00a erickson
 * PR22882: warning cleanup
 * 
 * 4   5/31/06 4:18p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/2   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 3   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/1   3/9/06 11:15a tokushig
 * add sync calls so draws show up on screen after they are requested
 * 
 * 2   6/7/05 2:35p erickson
 * PR15789: fixed compilation error
 * 
 * 1   2/7/05 9:10p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   1/14/05 4:55p erickson
 * PR13829: added test of bwin copy/fill
 * 
 * Irvine_BSEAVSW_Devel/9   3/24/04 9:15a erickson
 * PR9758: added test for reparenting and child clipping
 * 
 * Irvine_BSEAVSW_Devel/8   3/23/04 5:34p erickson
 * PR9753: extended test app to test zorder
 * 
 * Irvine_BSEAVSW_Devel/7   2/19/04 3:32p bandrews
 * PR9723: Implemented ARGB1555 support and tested ARGB8888 support
 * 
 * Irvine_BSEAVSW_Devel/6   2/11/04 4:18p erickson
 * PR9691: updated to work with recent changes
 * 
 * Irvine_BSEAVSW_Devel/5   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/4   1/15/04 10:14a erickson
 * PR9107: continued initial development, ready for mlibs port
 *
 * Irvine_BSEAVSW_Devel/3   1/14/04 11:36p erickson
 * PR9107: freetype support added to iolaus and still doing initial
 * development
 *
 * Irvine_BSEAVSW_Devel/2   1/13/04 2:01p erickson
 * PR9107: iolaus drawing, clipping, event queue working
 *
 * Irvine_BSEAVSW_Devel/1   1/13/04 12:32a erickson
 * PR9107: initial impl of iolaus
 *
 ****************************************************************/
#include "bwin.h"
#include "setup.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

extern bsurface_t surface;

void do_sync(bwin_framebuffer_t fb)
{
    BSTD_UNUSED(fb);
    settopapi_sync();
}
    
void do_copy_rect(bwin_framebuffer_t destfb, const bwin_rect *destrect,
        bwin_framebuffer_t srcfb, const bwin_rect *srcrect)
{
    BSTD_UNUSED(destfb);
    BSTD_UNUSED(srcfb);
    bsurface_copy(
        surface, (const bsettop_rect *)destrect, 
        surface, (const bsettop_rect *)srcrect);
}

int main()
{
    bwin_engine_t win_engine;
    bwin_framebuffer_t fb;
    bwin_t win;
    bwin_framebuffer_settings fb_settings;
    bwin_engine_settings win_engine_settings;
    unsigned width = 400;
    unsigned height = 400;
    bwin_rect fullrect = {0,0,0,0};
    bsettopapi_graphics settings;
    bsettopapi_graphics_status status;
    
    fullrect.width = width;
    fullrect.height = height;
    
    settings.display_format = bvideo_format_ntsc;
    settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
    settings.double_buffered = true;
    settings.use_offscreen_buffer = true;
    settopapi_init(&settings, &status);

    /* bring up bwin with a single framebuffer */
    bwin_engine_settings_init(&win_engine_settings);
    win_engine = bwin_open_engine(&win_engine_settings);
    assert(win_engine);

    printf("framebuffer %p, %dx%d, pitch %d\n", status.buffer, status.width, status.height, status.pitch);
    bwin_framebuffer_settings_init(&fb_settings);
    fb_settings.width = status.width;
    fb_settings.height = status.height;
    fb_settings.pitch = status.pitch;
    fb_settings.buffer = status.buffer;
    fb_settings.pixel_format = bwin_pixel_format_r5_g6_b5;
    fb_settings.drawops.sync = do_sync;
    fb_settings.drawops.copy_rect = do_copy_rect;
    fb = bwin_open_framebuffer(win_engine, &fb_settings);
    assert(fb);

    /* get the toplevel window */
    bwin_get_framebuffer_settings(fb, &fb_settings);

    win = fb_settings.window;
    bwin_fill_rect(win, &fullrect, 0xFF00FF00, NULL);
    do_sync(fb);
    getchar();

    /* fill a box with blue gradient */
    {
    bwin_rect cliprect = {50,50,2,50};
    int i;
    int color = 0xFF000000;
    for (i=0;i<25;i++) {
        color += 10;
        bwin_fill_rect(win, &fullrect, color, &cliprect);
        cliprect.x += 2;
    }
    do_sync(fb);
    getchar();
	}
    
    /* copy the box */
    {
    bwin_rect srcrect = {50,50,50,50};
    bwin_rect destrect = {50,125,50,50};
    bwin_copy_rect(win, &destrect, win, &srcrect, NULL);
    do_sync(fb);
    getchar();
    }
    
    /* copy and scale */
    {
    bwin_rect srcrect = {50,50,50,50};
    bwin_rect destrect = {50,200,100,100};
    bwin_copy_rect(win, &destrect, win, &srcrect, NULL);
    do_sync(fb);
    getchar();
    }
    
    /* copy and scale with clip */
    {
    bwin_rect srcrect = {50,50,50,50};
    bwin_rect destrect = {150,200,100,100};
    bwin_rect cliprect = {200,200,50,10};
    bwin_copy_rect(win, &destrect, win, &srcrect, &cliprect);
    do_sync(fb);
    getchar();
    }
    
    /* cleanup */
    bwin_close_framebuffer(fb);
    bwin_close_engine(win_engine);

    settopapi_uninit();

    return 0;
}
