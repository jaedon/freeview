/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: showimage.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 1/9/07 11:00a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/utils/showimage.c $
 * 
 * 7   1/9/07 11:00a erickson
 * PR24374: adding DBG
 * 
 * 6   11/15/06 10:59a erickson
 * PR22822: added -loops and -quit options. general cleanup.
 * 
 * 5   10/25/05 12:22p erickson
 * PR17070: fixed fb_rect, allow params in any order
 * 
 * 4   10/6/05 9:29a erickson
 * PR17070: fixed bsurface_copy call by limiting the destination rectangle
 * to the max size of the framebuffer.
 * 
 * 3   9/29/05 4:23p erickson
 * PR17070: if scaling, prefer using a hardware scaler (by using
 * bsurface_copy)
 * 
 * 2   2/23/05 2:31p erickson
 * PR14180: added vxworks support
 * 
 * 1   2/7/05 9:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/5   2/4/05 2:23p erickson
 * PR13829: tweaked setup.h
 * 
 * Irvine_BSEAVSW_Devel/4   1/12/05 12:21p erickson
 * PR13639: have to fill background with opaque black to see images
 * 
 * Irvine_BSEAVSW_Devel/3   1/5/05 3:04p erickson
 * PR13639: small update to test app
 * 
 * Irvine_BSEAVSW_Devel/2   12/13/04 11:33a erickson
 * PR13495: added ARGB1555 support
 * 
 * Irvine_BSEAVSW_Devel/1   12/8/04 2:51p erickson
 * PR12610: created showimage utils
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

extern int which_scale_alg;

BDBG_MODULE(showimage);

void printUsage()
{
	printf(
	"Usage: showimage OPTIONS FILENAME [X Y WIDTH HEIGHT]\n"
	"  OPTIONS are:\n"
	"    -fmt DISPLAYFORMAT    Values are 480i,480p,720p,1080i\n"
	"    -pixelfmt PIXELFORMAT Values are rgb565,argb1555,argb8888\n"
	"    -loops X              For performance testing\n"
	"    -quit                 Exit when done. For performance testing.\n"
	);
}

#define MAX_DISPFORMAT 4
struct {
	const char *name;
	bvideo_format format;
} g_dispformat[MAX_DISPFORMAT] = {
	{"480i",bvideo_format_ntsc},
	{"480p",bvideo_format_480p},
	{"720p",bvideo_format_720p},
	{"1080i",bvideo_format_1080i}
};

#define MAX_PIXELFORMAT 3
struct {
	const char *name;
	bgraphics_pixel_format format;
	bwin_pixel_format bwin_format;
} g_pixelformat[MAX_PIXELFORMAT] = {
	{"rgb565",  bgraphics_pixel_format_r5_g6_b5,    bwin_pixel_format_r5_g6_b5},
	{"argb1555",bgraphics_pixel_format_a1_r5_g5_b5, bwin_pixel_format_a1_r5_g5_b5},
	{"argb8888",bgraphics_pixel_format_a8_r8_g8_b8, bwin_pixel_format_a8_r8_g8_b8}
};

int main(int argc, char **argv)
{
	int i;
	unsigned loops = 1;
	bool quit = false;
	
	bwin_engine_t win_engine;
	bwin_framebuffer_t fb = NULL;
	bwin_framebuffer_settings fb_settings;
	bwin_engine_settings win_engine_settings;
	const char *filename;
	bwin_rect destrect = {0,0,0,0};
	bwin_rect imagerect = {0,0,0,0};
	int mode = bwin_image_render_mode_stretch;
	bsettopapi_graphics settings;
	bsettopapi_graphics_status graphics_status;
	bsettop_rect fb_rect = {0,0,0,0};
	bwin_image_t image;
	bwin_image_settings image_settings;
	int curarg = 1;
	bwin_pixel_format bwin_pformat;
	bsurface_t offscreen_surface = NULL;
	
	/* Set defaults */
	settings.display_format = bvideo_format_ntsc;
	settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
	settings.double_buffered = false;
	settings.use_offscreen_buffer = false;
	bwin_pformat = bwin_pixel_format_r5_g6_b5;

	while (curarg < argc) {
		if (!strcmp(argv[curarg],"-fmt") && curarg+1 < argc) {
			curarg++;
			for (i=0;i<MAX_DISPFORMAT;i++) {
				if (!strcmp(argv[curarg],g_dispformat[i].name)) {
					settings.display_format = g_dispformat[i].format;
					break;
				}
			}
		}
		else if (!strcmp(argv[curarg],"-pixelfmt") && curarg+1 < argc) {
			curarg++;
			for (i=0;i<MAX_PIXELFORMAT;i++) {
				if (!strcmp(argv[curarg],g_pixelformat[i].name)) {
					settings.pixel_format = g_pixelformat[i].format;
					break;
				}
			}
		}
		else if (!strcmp(argv[curarg],"-loops") && curarg+1 < argc) {
			curarg++;
			loops = atoi(argv[curarg]);
		}
		else if (!strcmp(argv[curarg],"-quit")) {
			quit = true;
		}
		else 
			break;
		curarg++;
	}
	
	if (curarg >= argc) {
		printUsage();
		return -1;
	}
	
	filename = argv[curarg++];

	/* bring up settop api */
	settopapi_init(&settings, &graphics_status);
	
	/* bring up bwin with a single framebuffer */
	bwin_engine_settings_init(&win_engine_settings);
	win_engine = bwin_open_engine(&win_engine_settings);
	assert(win_engine);

	image = bwin_image_load(win_engine, filename);
	if (!image) {
		printf("Unable to load image %s\n", filename);
		goto done;
	}

	bwin_image_get_settings(image, &image_settings);
	imagerect.width = image_settings.width;
	imagerect.height = image_settings.height;
	
	/* default to rendering unscaled at 0,0 */
	destrect = imagerect;
	
	/* when blitting to the framebuffer, we cannot exceed the size */
	fb_rect.width = graphics_status.width;
	fb_rect.height = graphics_status.height;
	
	printf("framebuffer %p, %dx%d, pitch %d\n", 
		graphics_status.buffer, graphics_status.width, graphics_status.height, graphics_status.pitch);
	
	/* create an off-screen surface to allow hardware scale to the framebuffer */
	if (image_settings.width != (unsigned)graphics_status.width || image_settings.height != (unsigned)graphics_status.height) {
		bsurface_create_settings cs;
		bsurface_memory mem;

		printf("using hardware scaler with offscreen surface %dx%d\n", imagerect.width, imagerect.height);		
		bsurface_create_settings_init(&cs, graphics_status.graphics);
		cs.width = imagerect.width;
		cs.height = imagerect.height;
		offscreen_surface = bsurface_create(graphics_status.graphics, &cs);
		bsurface_get_memory(offscreen_surface, &mem);
		
		graphics_status.width = imagerect.width;
		graphics_status.height = imagerect.height;
		graphics_status.buffer = mem.buffer;
		graphics_status.pitch = mem.pitch;
	}
	
	bwin_framebuffer_settings_init(&fb_settings);
	fb_settings.width = graphics_status.width;
	fb_settings.height = graphics_status.height;
	fb_settings.pitch = graphics_status.pitch;
	fb_settings.buffer = graphics_status.buffer;
	fb_settings.pixel_format = bwin_pformat;
	fb = bwin_open_framebuffer(win_engine, &fb_settings);
	assert(fb);

	/* get the toplevel window */
	bwin_get_framebuffer_settings(fb, &fb_settings);

	/* after reading default width/height, allow overrides */
	if (curarg < argc) destrect.x = atoi(argv[curarg++]);
	if (curarg < argc) destrect.y = atoi(argv[curarg++]);
	if (curarg < argc) destrect.width = atoi(argv[curarg++]);
	if (curarg < argc) destrect.height = atoi(argv[curarg++]);
	if (curarg < argc) mode = atoi(argv[curarg++]);

	/* this provides a timestamp on the console for timing */
	BDBG_WRN(("begin bwin processing..."));
	
	/* Fill with opaque black, because blitting graphics will not change
	the alpha portion of the dest pixel. */
	{
	bwin_rect fillrect = {0,0,0,0};
	fillrect.width = graphics_status.width;
	fillrect.height = graphics_status.height;
	bwin_fill_rect(fb_settings.window, &fillrect, 0xFF000000, NULL);
	}
	
	if (offscreen_surface) {
		/* make sure dest rect doesn't exceed fb_rect */
		if ((destrect.x + destrect.width > fb_rect.width) ||
			(destrect.y + destrect.height > fb_rect.height))
		{
			destrect.width = fb_rect.width - destrect.x;
			destrect.height = fb_rect.height - destrect.y;
		}
		
		/* bwin will render unscaled */
		while (loops--) {
			bwin_image_render(fb_settings.window, image,
				mode, &imagerect, NULL, NULL);
		}
		printf("scaled copy %d %d %d %d\n", destrect.width, destrect.height, fb_rect.width, fb_rect.height);
		/* settop api will use hardware (if present) to upscale */
		bsurface_copy(graphics_status.framebuffer, (bsettop_rect*)&destrect, offscreen_surface, (bsettop_rect*)&imagerect);
	}
	else {
		/* let bwin scale */
		while (loops--) {
			bwin_image_render(fb_settings.window, image,
				mode, &destrect, NULL, NULL);
		}
	}
		
	BDBG_WRN(("calling sync..."));
	settopapi_sync();
	
	if (!quit) {
		printf("press Enter to quit\n");
#ifdef __vxworks
		{
		char str[10];
		fgets(str, 10, stdin);
		}
#else
		getchar();
#endif
	}
	
done:
	if (image)
		bwin_image_close(image);
	if (offscreen_surface) {
		bsurface_destroy(offscreen_surface);
	}
	bwin_close_framebuffer(fb);
	bwin_close_engine(win_engine);
	settopapi_uninit();

	return 0;
}

#ifdef __vxworks
int go(char *arg1, char *arg2, char *arg3)
{
	char *argv[4] = {"showimage", arg1, arg2, arg3};
	return main(4, argv);
}
#endif
