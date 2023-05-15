/***************************************************************************
 *     Copyright (c) 2004-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: test.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 11/15/06 11:00a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/utils/test.c $
 * 
 * 3   11/15/06 11:00a erickson
 * PR22882: warning cleanup
 * 
 * 2   2/15/05 5:23p erickson
 * PR13387: added palette8 support
 * 
 * 1   2/7/05 9:10p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
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

bwin_font_t font;

bwin_t createwindow(bwin_t parent, const char *name,
	int x, int y, bwin_rect *rect, int z, int color)
{
	bwin_settings window_settings;
	bwin_settings_init(&window_settings);
	window_settings.parent = parent;
	window_settings.name = name;
	window_settings.x = x;
	window_settings.y = y;
	window_settings.rect = *rect;
	window_settings.data = (void*)color;
	window_settings.visible = true;
	window_settings.zorder = z;
	window_settings.transparent = false;
	return bwin_open(&window_settings);
}

void do_sync(bwin_framebuffer_t fb)
{
	BSTD_UNUSED(fb);
	settopapi_sync();
}

void process_msgs(bwin_engine_t win_engine)
{
	/* process the event queue. in this case, we only have the initial paint messages.
	after that, we're done */
	while (1) {
		bwin_event ev;
		if (!bwin_get_event(win_engine, &ev, 1)) {
			bwin_settings settings;

			switch (ev.type) {
			case bwin_event_type_paint:
				bwin_get(ev.window, &settings);
				printf("fill_rect %d,%d,%u,%u, color=0x%08x\n",
					settings.x+ev.paint.rect.x,
					settings.y+ev.paint.rect.y,
					ev.paint.rect.width,
					ev.paint.rect.height,
					(unsigned)settings.data);
				bwin_fill_rect(ev.window, &settings.rect, (uint32_t)settings.data, &ev.paint.rect);
				if (font)
					bwin_draw_text(ev.window, font, 4,15, "Hello", -1, 0xFFFFFFFF, &ev.paint.rect);
				break;
			default:
				break;
			}
		}
		else
			break;
	}
}

void do_msgs(const char *msg, bwin_engine_t win_engine)
{
	process_msgs(win_engine);
	printf("%s: Press ENTER to end this test.\n", msg);
	getchar();
}

int main()
{
	bwin_engine_t win_engine;
	bwin_framebuffer_t fb;
	bwin_framebuffer_settings fb_settings;
	bwin_t w0,w1,w2,w3;
	bwin_settings s;
	bwin_engine_settings win_engine_settings;
	unsigned width = 400;
	unsigned height = 400;
	bsettopapi_graphics settopapi_graphics;
	bsettopapi_graphics_status settopapi_status;
	bwin_rect r1 = {0,0,100,100};
	bwin_rect r2 = {0,0,200,100};
	bwin_rect r3 = {0,0,30,30};

	/* bring up settop api and get the basic information we need for graphics */
	settopapi_graphics.display_format = bvideo_format_ntsc;
	settopapi_graphics.pixel_format = bgraphics_pixel_format_r5_g6_b5;
	settopapi_graphics.double_buffered = false;
	settopapi_graphics.use_offscreen_buffer = false;
	settopapi_init(&settopapi_graphics, &settopapi_status);

	/* bring up bwin with a single framebuffer */
	bwin_engine_settings_init(&win_engine_settings);
	win_engine = bwin_open_engine(&win_engine_settings);
	assert(win_engine);

#if 0
	font = bwin_open_font(win_engine, "fonts/arial_18_aa.bwin_font", -1, true);
#endif
	font = bwin_open_font(win_engine, "fonts/arial_italic_35_aa.bwin_font", -1, true);

	/* don't assume that the font was created. operate anyway. */
	if (!font)
		printf("Unable to create font. Continuing with test.\n");

	printf("framebuffer %p, %dx%d, pitch %d\n", 
		settopapi_status.buffer, width, height, settopapi_status.pitch);
	bwin_framebuffer_settings_init(&fb_settings);
	fb_settings.width = width;
	fb_settings.height = height;
	fb_settings.pitch = settopapi_status.pitch;
	fb_settings.buffer = settopapi_status.buffer;
	fb_settings.pixel_format = bwin_pixel_format_r5_g6_b5;
	fb_settings.drawops.sync = do_sync;
	fb = bwin_open_framebuffer(win_engine, &fb_settings);
	assert(fb);

	/* get the toplevel window */
	bwin_get_framebuffer_settings(fb, &fb_settings);

	/*************************************/
	w0 = fb_settings.window;
	bwin_get(w0, &s);
	s.data = (void*)0xFF000000;
	bwin_set(w0, &s);
	bwin_repaint(w0, NULL);
	do_msgs("full screen basic test", win_engine);

	/*************************************/
	w1 = createwindow(fb_settings.window, "1", 100,100,&r1, 1, 0xFFFF0000);
	w2 = createwindow(fb_settings.window, "2", 320,120,&r2, 1, 0xFF00FF00);
	w3 = createwindow(w2, "3", 10,10,&r3, 0, 0xFF0000FF);
	do_msgs("clipping test", win_engine);
	bwin_close(w3);
	bwin_close(w2);
	bwin_close(w1);

	/*************************************/
	w1 = createwindow(fb_settings.window, "1", 120,120, &r1, 1, 0xFFFF0000);
	w2 = createwindow(fb_settings.window, "2", 100,100, &r2, 2, 0xFF00FF00);
	w3 = createwindow(w2, "3", 10,10, &r3, 0, 0xFF0000FF);
	do_msgs("zorder test(1)", win_engine);

	bwin_get(w1, &s);
	s.zorder = 2;
	bwin_set(w1, &s);
	bwin_get(w2, &s);
	s.zorder = 1;
	bwin_set(w2, &s);
	do_msgs("zorder test(2)", win_engine);

	/*************************************/
	bwin_get(w1, &s);
	s.parent = w2;
	s.x = 40;
	s.y = 40;
	bwin_set(w1, &s);
	do_msgs("reparent test1", win_engine);
	/* when repainting the child explicitly, it should still be clipped by
	its parent. */
	bwin_repaint(w1, NULL);
	do_msgs("reparent test2", win_engine);

	bwin_close(w3);
	bwin_close(w1);
	bwin_close(w2);

	/* cleanup */
	bwin_close_framebuffer(fb);
	bwin_close_engine(win_engine);

	settopapi_uninit();

	return 0;
}
