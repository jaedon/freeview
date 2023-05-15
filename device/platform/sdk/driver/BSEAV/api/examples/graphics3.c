/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: graphics3.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 7:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/graphics3.c $
 * 
 * 1   2/7/05 7:00p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   9/22/04 5:51p marcusk
 * PR12755: Updated to support graphics clipping for zoom and pan support
 *
***************************************************************************/

#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>

#if 1
#define DISPLAY_FORMAT	bvideo_format_1080i 
#else
#define DISPLAY_FORMAT	bvideo_format_ntsc
#endif

#if 1
#define SD_GRAPHICS 1	/* Always use 720x480 size graphics and scale up to HD */
#else
#define SD_GRAPHICS 0
#endif

#define DEST_WIDTH		(DISPLAY_FORMAT==bvideo_format_1080i?1920:720)
#define DEST_HEIGHT		(DISPLAY_FORMAT==bvideo_format_1080i?1080:480)

#define SOURCE_WIDTH	(SD_GRAPHICS?720:DEST_WIDTH)
#define SOURCE_HEIGHT	(SD_GRAPHICS?480:DEST_HEIGHT)

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bgraphics_settings gfx_settings;
	bsurface_create_settings cfg;
	bsurface_t surface, framebuffer;
	bsurface_memory mem;
	bsurface_settings settings;
	bdisplay_settings disp_cfg;

	int i, j;

	if (rc) goto error;

	bdisplay_get(display, &disp_cfg);
	disp_cfg.format = DISPLAY_FORMAT;
	if( disp_cfg.format == bvideo_format_1080i )
	{
		disp_cfg.composite = NULL;
		disp_cfg.svideo = NULL;
		disp_cfg.rf = NULL;
	}
	bdisplay_set(display, &disp_cfg);

	bsurface_create_settings_init(&cfg, graphics);

	cfg.width = DEST_WIDTH;
	cfg.height = DEST_HEIGHT;

	framebuffer = bgraphics_create_framebuffer(graphics, &cfg, true);

	bgraphics_get( graphics, &gfx_settings );
	gfx_settings.destination_width = DEST_WIDTH;
	gfx_settings.destination_height = DEST_HEIGHT;
	gfx_settings.source_width = SOURCE_WIDTH;
	gfx_settings.source_height = SOURCE_HEIGHT;
	bgraphics_set( graphics, &gfx_settings );

	cfg.height = SOURCE_HEIGHT;
	cfg.width = SOURCE_WIDTH;

	surface = bsurface_create(graphics, &cfg);

	bsurface_get( surface, &settings );
	bsurface_fill( surface, &settings.position, 0 );

	settings.visible = true;
	bsurface_set( surface, &settings );

	bsurface_get_memory(surface, &mem);
	for(i=0;i<1000;i++) {
		unsigned x = rand() % cfg.width;
		unsigned y = rand() % cfg.height;
		unsigned short *buf = (unsigned short *)((unsigned)mem.buffer+y*mem.pitch);
		buf[x] = rand();
		if (i%100==0) 
		{
			printf(".");
			fflush(stdout);
		}

		/* because we're drawing to the framebuffer and not double-buffered,
		this only serves to flush cached memory. In general, you should always
		use it. */
		bgraphics_sync(graphics);
	}

	bsurface_get( surface, &settings );

	for(j=0;j<10;j++) {
		/* zoom */
		for(i=0;i<200;i++) {
			settings.cliprect.x++;
			settings.cliprect.y++;
			settings.cliprect.width-=2;
			settings.cliprect.height-=2;
			bsurface_set( surface, &settings );
			bgraphics_sync(graphics);
		}

		for(i=0;i<200;i++) {
			settings.cliprect.x--;
			settings.cliprect.y--;
			settings.cliprect.width+=2;
			settings.cliprect.height+=2;
			bsurface_set( surface, &settings );
			bgraphics_sync(graphics);
		}
	}

	return 0;
error:
	return 1;
}
