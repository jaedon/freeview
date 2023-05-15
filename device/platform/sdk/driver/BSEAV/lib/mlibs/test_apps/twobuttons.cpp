/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: twobuttons.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/7/12 10:06a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/test_apps/twobuttons.cpp $
 * 
 * 3   9/7/12 10:06a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <bsettop.h>
#include <bwin.h>
#include <mapplication.h>
#include <mbutton.h>
#include <assert.h>

bgraphics_t graphics;

void do_sync(bwin_framebuffer_t fb)
{
	bgraphics_sync(graphics);
}

int main() {
	bwin_engine_t win_engine;
	bwin_engine_settings win_engine_settings;
	bwin_framebuffer_t fb;
	bwin_framebuffer_settings fb_settings;

	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bsurface_create_settings surface_create_settings;
	bsurface_settings surface_settings;
	bsurface_t framebuffer, surface;
	bsurface_memory mem;

	graphics = bgraphics_open(B_ID(0),display);

#if 0 /* does not work!*/
	/* bring up settop api graphics */
	assert(!rc);
	assert(display);
	assert(graphics);
	bsurface_create_settings_init(&surface_create_settings, graphics);
	surface_create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
	surface_create_settings.width = 720;
	surface_create_settings.height = 480;
	surface = bsurface_create(graphics, &surface_create_settings);
	assert(surface);
	bsurface_get(surface, &surface_settings);
	surface_settings.visible = true;
	bsurface_set(surface, &surface_settings);
	bsurface_get_memory(surface, &mem);

#else /* try creating a framebuffer explicitly */
    bsurface_create_settings_init(&surface_create_settings, graphics);
    surface_create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
    surface_create_settings.width = 720;
    surface_create_settings.height = 480;

    framebuffer = bgraphics_create_framebuffer(graphics, &surface_create_settings, 0); /* specifying 1 to back_buffer_count does not work */
    BDBG_ASSERT(framebuffer);

#if 0 /* does not work! */
    /* offscreen surface */
    surface = bsurface_create(graphics, &surface_create_settings);
    BDBG_ASSERT(surface);
    bsurface_get(surface, &surface_settings);
    surface_settings.visible = true;
    bsurface_set(surface, &surface_settings);
#else
    /* draw directly to framebuffer */
    surface = framebuffer;
#endif

    bsurface_get_memory(surface, &mem);
#endif
	/* bring up bwin with a single framebuffer */
	bwin_engine_settings_init(&win_engine_settings);
	win_engine = bwin_open_engine(&win_engine_settings);
	assert(win_engine);

	bwin_framebuffer_settings_init(&fb_settings);
	fb_settings.width = surface_create_settings.width;
	fb_settings.height = surface_create_settings.height;
	fb_settings.pitch = mem.pitch;
	fb_settings.buffer = mem.buffer;
	fb_settings.pixel_format = bwin_pixel_format_r5_g6_b5;
	fb_settings.drawops.sync = do_sync;
	fb = bwin_open_framebuffer(win_engine, &fb_settings);
	assert(fb);

#if 0 /* draw a rectangle */
    {
        bwin_t w0;
        bwin_rect r1 = {200,200,100,100};
        
        bwin_get_framebuffer_settings(fb, &fb_settings);
        w0 = fb_settings.window;
        bwin_fill_rect(w0, &r1, 0xff0000ff, NULL); /* blue */
        bgraphics_sync(graphics);
        getchar();
    }
#endif

	/* init mlibs */
	MApplication::Settings settings;
	MApplication::getDefaultSettings(&settings);
	settings.win_engine = win_engine;

	MFrameBuffer::Settings mfb_settings;
	MFrameBuffer::getDefaultSettings(&mfb_settings);
	mfb_settings.framebuffer = fb;

	/* here's the gui */
	MApplication app(&settings);
	MFrameBuffer mfb(&app, &mfb_settings);
	MWidget w(&mfb, MRect(0,0,720,480));
	w.setTextColor(0xFFFFFFFF);
	w.setBackgroundColor(0xFF000000);
	MPushButton b1(&w, MRect(40,40,200,40), "button1");
	MPushButton b2(&w, MRect(40,90,200,40), "button2");
	w.show();
	w.focus();
	app.setMainWidget(&w);
	return app.run();
}

