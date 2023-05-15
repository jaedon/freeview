/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: graphics_bitmap.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 6:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/graphics_bitmap.c $
 * 
 * 1   2/7/05 6:59p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   10/5/04 6:19p vsilyaev
 * PR 12925: Fixed HD modes.
 * 
***************************************************************************/

#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define SOURCE_WIDTH	720
#define SOURCE_HEIGHT	480

static uint16_t 
read_16(FILE *fin) 
{
	int b1, b2;

	b1 = fgetc(fin);
	b2 = fgetc(fin);

	return  (uint16_t) ((b1&0xFF) | ((b2&0xFF)<<8));
}

static uint32_t 
read_32(FILE *fin) 
{
	int b1, b2, b3, b4;

	b1 = fgetc(fin);
	b2 = fgetc(fin);
	b3 = fgetc(fin);
	b4 = fgetc(fin);

	return  (uint32_t) ( (b1&0xFF) | ((b2&0xFF)<<8) | ((b3&0xFF)<<16) | ((b4&0xFF)<<24) );
}

static uint32_t 
read_24(FILE *fin) 
{
	int b1, b2, b3;

	b1 = fgetc(fin);
	b2 = fgetc(fin);
	b3 = fgetc(fin);

	return  (uint32_t) ( (b1&0xFF) | ((b2&0xFF)<<8) | ((b3&0xFF)<<16) );
}


static FILE *
openbitmap(const char *name)
{
	FILE *bitmap;

	uint16_t identifier;
	uint32_t filesize;
	uint32_t offset;
	uint32_t header;
	uint32_t width, height;
    uint16_t planes, pixels;
	uint32_t format, size;

	bitmap = fopen(name, "r");
	if (!bitmap) {
		perror(name);
		return NULL;
	}
	identifier = read_16(bitmap);
	filesize = read_32(bitmap);
	read_32(bitmap); /* skipped */
	offset = read_32(bitmap);
	printf("identifier %#x filesize %d offset %d\n", identifier, filesize, offset);

	header = read_32(bitmap);
	width = read_32(bitmap);
	height = read_32(bitmap);
	planes = read_16(bitmap);
	pixels = read_16(bitmap);
	printf("height %d width %d planes %d pixels %d\n", height, width, planes, pixels);

	format = read_32(bitmap);
	size = read_32(bitmap);
	printf("format %d size %d\n", format, size);

	if ( ! (width == 720 && height == 480 && pixels == 24 && format == 0)) {
		printf("%s:unsupported file format\n", name);
		return NULL;
	}
	fseek(bitmap, offset, SEEK_SET);

	return bitmap;
}

int 
main(int argc, const char *argv[]) {
	FILE *bitmap;
	bvideo_format format = bvideo_format_ntsc;

	if (argc<3) {
		printf("Usage: %s <bitmap> <displayformat>\n", argv[0]);
		return 1;
	}
	bitmap = openbitmap(argv[1]);
	if (!bitmap) {
		return 1;
	}
	if (!strcmp("480i", argv[2])) {
		format = bvideo_format_ntsc;
	} else if (!strcmp("480p", argv[2])) {
		format = bvideo_format_480p;
	} else if (!strcmp("720p", argv[2])) {
		format = bvideo_format_720p;
	} else if (!strcmp("1080i", argv[2])) {
		format = bvideo_format_1080i;
	} else {
		printf("unknown video format %s\n",  argv[2]);
		return 1;
	}
	{
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bgraphics_settings gfx_settings;
	bsurface_create_settings cfg;
	bsurface_t surface, framebuffer;
	bsurface_memory mem;
	bsurface_settings settings;
	bdisplay_settings disp_cfg;
	int y, x;
	bvideo_format_settings format_info;

	if (rc) goto error;


	bdisplay_get(display, &disp_cfg);
	disp_cfg.format = format;
	if( disp_cfg.format != bvideo_format_ntsc)
	{
		disp_cfg.composite = NULL;
		disp_cfg.svideo = NULL;
		disp_cfg.rf = NULL;
	}
	bdisplay_set(display, &disp_cfg);

	bvideo_get_format_settings(format, &format_info);

	bsurface_create_settings_init(&cfg, graphics);

	cfg.width = format_info.width;
	cfg.height = format_info.height;

	framebuffer = bgraphics_create_framebuffer(graphics, &cfg, true);


	bgraphics_get( graphics, &gfx_settings );
	gfx_settings.destination_width = format_info.width;
	gfx_settings.destination_height = format_info.height;
	gfx_settings.source_width = SOURCE_WIDTH;
	gfx_settings.source_height = SOURCE_HEIGHT;
	bgraphics_set( graphics, &gfx_settings );

	bsurface_get( framebuffer, &settings );
	bsurface_fill( framebuffer, &settings.position, 0);

	cfg.height = SOURCE_HEIGHT;
	cfg.width = SOURCE_WIDTH;
	cfg.pixel_format = bgraphics_pixel_format_a8_r8_g8_b8;

	surface = bsurface_create(graphics, &cfg);

	bsurface_get( surface, &settings );
	bsurface_fill( surface, &settings.position, 0);

	settings.visible = true;
	bsurface_set( surface, &settings );

	bsurface_get_memory(surface, &mem);

	for(y=479;y>=0;y--) {
		uint32_t *buf = (uint32_t*)((unsigned)mem.buffer+y*mem.pitch);
		for(x=0;x<720;x++) {
			uint32_t data = read_24(bitmap);
			data |= (0xFF)<<24;
			buf[x] = data;
		}
	}
	bgraphics_sync(graphics);

	printf("any key to exit\n");
	getchar();
	}

	return 0;
error:
	return 1;
}
