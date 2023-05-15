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
 * $brcm_Workfile: palette_blit.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/palette_blit.c $
 * 
 * 4   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <bstd.h>
#include <bkni.h>

bdisplay_t display;
bgraphics_t graphics;

void prompt() {
#if 1
	printf("Press ENTER:\n");
	getchar();
#else
	usleep(100 * 1000);
#endif
}

bgraphics_pixel_format pxl[] = {
	bgraphics_pixel_format_a8_palette8,
	bgraphics_pixel_format_palette8,
	bgraphics_pixel_format_palette2,
	bgraphics_pixel_format_palette4,
	bgraphics_pixel_format_r5_g6_b5,
	bgraphics_pixel_format_a1_r5_g5_b5,
	bgraphics_pixel_format_r5_g5_b5_a1,
	bgraphics_pixel_format_a4_r4_g4_b4,
	bgraphics_pixel_format_r4_g4_b4_a4,
	bgraphics_pixel_format_a8_r8_g8_b8,
	bgraphics_pixel_format_y08_cb8_y18_cr8
};

bgraphics_pixel g_palette[] = {
	0xFF0000FF,
	0xFF00FF00,
	0xFFFF0000
	};
	
const char *colorstr[] = {"blue", "green", "red"};

void cpu_bytefill(bsurface_t surface, unsigned char val)
{
	bsurface_memory mem;
	bsurface_settings settings;
	bsurface_get_memory(surface, &mem);
	bsurface_get(surface, &settings);
	memset(mem.buffer, val, mem.pitch * settings.position.height);
}

void cpu_shortfill(bsurface_t surface, unsigned short val)
{
	bsurface_memory mem;
	bsurface_settings settings;
	unsigned short *ptr;
	unsigned i,j;

	bsurface_get_memory(surface, &mem);
	bsurface_get(surface, &settings);
	ptr = (unsigned short *)mem.buffer;
	for (j=0;j<settings.position.height;j++) {
		for (i=0;i<settings.position.width;i++)
			ptr[i] = val;
		ptr += mem.pitch / 2;
	}
}

int main(void) {
	int i,j;
	bsurface_t surface;
	bsurface_create_settings create_settings;
	bsurface_t fb;
	bsurface_settings surface_settings;
	bgraphics_palette palette = {g_palette, 3};
	bresult rc = bsettop_init(BSETTOP_VERSION);
	int fbmode = 1;

	display = bdisplay_open(B_ID(0));

	graphics = bgraphics_open(B_ID(0),display);
	bsurface_create_settings_init(&create_settings, graphics);
	
	switch (fbmode) {
	case 0: 
		create_settings.pixel_format = bgraphics_pixel_format_y08_cb8_y18_cr8;
		break;
	case 1: 
		create_settings.pixel_format = bgraphics_pixel_format_a8_r8_g8_b8;
		break;
	case 2:
		create_settings.pixel_format = bgraphics_pixel_format_palette8;
		break;
	}
	fb = bgraphics_create_framebuffer(graphics, &create_settings, false);
	
	if (fbmode == 2) {
		bsurface_load_clut(fb, palette);
	}

	for (j=0;j<11;j++) {
		bsettop_rect r1 = {0,0,surface_settings.position.width,surface_settings.position.height};

		create_settings.pixel_format = pxl[j];
		surface = bsurface_create(graphics, &create_settings);

		if (j <=3) {
			bsurface_load_clut(surface, palette);
		}

		bsurface_get(surface, &surface_settings);
		surface_settings.visible = true;
		bsurface_set(surface, &surface_settings);

		/* Loop through the palette of blue, green , red */
		for (i=0;i<3;i++) {
			switch (j) {
			case 0:
				bsurface_fill(surface, &r1, 0xFF00 | i); break;
			case 1:
			case 2:
			case 3:
				bsurface_fill(surface, &r1, i); break;
			default:
				/* ARGB8888 */
				bsurface_fill(surface, &r1,
					0xFF000000 | ((0xFF) << (i*8))
					); break;
			}
			rc = bgraphics_sync(graphics);
			if (!rc) {
				bsurface_memory mem;
				bsurface_get_memory(fb, &mem);
				printf("fb mem  %08lx\n", *(unsigned long*)mem.buffer);
				bsurface_get_memory(surface, &mem);
				printf("surf mem %08lx\n", *(unsigned long*)mem.buffer);
				printf("color should be %s\n", colorstr[i]);
			}
			prompt();
		}
		bsurface_destroy(surface);
	}

	/* Alpha surface */
	create_settings.pixel_format = bgraphics_pixel_format_a8;
	surface = bsurface_create(graphics, &create_settings);
	for (j=0;j<3;j++) {
		bsurface_get(surface, &surface_settings);
		surface_settings.color = 0xFF000000 | (0xFF << (j*8));

		printf("%08x\n", surface_settings.color);
		printf("color should be %s\n", colorstr[j]);

		surface_settings.visible = true;
		bsurface_set(surface, &surface_settings);

		for (i=0;i<=0xFF;i+=04) {
			bsettop_rect r1 = {0,0,surface_settings.position.width,surface_settings.position.height};
			bsurface_fill(surface, &r1, i);
			bgraphics_sync(graphics);
			//printf("%d: %x\n", j, i);
			BKNI_Sleep(1);
		}
		prompt();
	}
	bsurface_destroy(surface);

	/* We're done */
	bgraphics_close(graphics);
	bdisplay_close(display);
	bsettop_uninit();

	return rc;
}
