/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: test2.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/15/05 5:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/utils/test2.c $
 * 
 * 1   2/15/05 5:23p erickson
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
#include "bsettop.h"

bwin_font_t font;
extern bsurface_t surface;

bgraphics_pixel g_palette_colors[] = {
	0xFF000000, 
	0xFFFF0000, 
	0xFF00FF00, 
	0xFF0000FF
};

bgraphics_palette g_palette = {g_palette_colors, 4};

void do_sync(bwin_framebuffer_t fb)
{
	BSTD_UNUSED(fb);
	settopapi_sync();
}

void prompt(const char *msg)
{
	printf("%s: Press ENTER to end this test.\n", msg);
	settopapi_sync();
	getchar();
}

int main()
{
	bwin_engine_t win_engine;
	bwin_framebuffer_t fb;
	bwin_framebuffer_settings fb_settings;
	bwin_t w0;
	bwin_engine_settings win_engine_settings;
	unsigned width, height;
	bsettopapi_graphics settopapi_graphics;
	bsettopapi_graphics_status settopapi_status;

	/* bring up settop api and get the basic information we need for graphics */
	settopapi_graphics.display_format = bvideo_format_ntsc;
	settopapi_graphics.pixel_format = bgraphics_pixel_format_palette8;
	settopapi_graphics.double_buffered = false;
	settopapi_graphics.use_offscreen_buffer = false;
	settopapi_init(&settopapi_graphics, &settopapi_status);
	width = settopapi_status.width;
	height = settopapi_status.height;
	
	bsurface_load_clut(surface, g_palette);

	/* bring up bwin with a single framebuffer */
	bwin_engine_settings_init(&win_engine_settings);
	win_engine = bwin_open_engine(&win_engine_settings);
	assert(win_engine);

	printf("framebuffer %p, %dx%d, pitch %d\n", 
		settopapi_status.buffer, width, height, settopapi_status.pitch);
	bwin_framebuffer_settings_init(&fb_settings);
	fb_settings.width = width;
	fb_settings.height = height;
	fb_settings.pitch = settopapi_status.pitch;
	fb_settings.buffer = settopapi_status.buffer;
	fb_settings.pixel_format = bwin_pixel_format_palette8;
	fb = bwin_open_framebuffer(win_engine, &fb_settings);
	assert(fb);

	// get the toplevel window
	bwin_get_framebuffer_settings(fb, &fb_settings);

	//////////////////////////////////////////
	w0 = fb_settings.window;
	bwin_rect r0 = {0,0,width,height};
	bwin_fill_rect(w0, &r0, 0, NULL);

	bwin_rect r1 = {100,100,100,100};
	bwin_fill_rect(w0, &r1, 1, NULL);
	r1.x += 30;
	r1.y += 30;
	bwin_fill_rect(w0, &r1, 2, NULL);
	r1.x += 30;
	r1.y += 30;
	bwin_fill_rect(w0, &r1, 3, NULL);
	prompt("draw red,green,blue boxes");

	//font = bwin_open_font(win_engine, "fonts/arial_18_aa.bwin_font", -1, true);
	font = bwin_open_font(win_engine, "fonts/arial_italic_35_aa.bwin_font", -1, true);
	if (font) {
		bwin_draw_text(w0, font, 200, 100, "Hello World", -1, 3, NULL);
		prompt("draw font");
	}
	
	// cleanup
	bwin_close_framebuffer(fb);
	bwin_close_engine(win_engine);
	settopapi_uninit();
	return 0;
}
