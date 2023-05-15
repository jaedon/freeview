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
 * $brcm_Workfile: graphics_fill.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/graphics_fill.c $
 * 
 * 2   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <unistd.h>

bdisplay_t display;
bgraphics_t graphics;

void prompt() {
#if 1
	getchar();
#else
	usleep(100 * 1000);
#endif
}

bgraphics_pixel_format pxl[] = {
#if 1
	bgraphics_pixel_format_r5_g6_b5,
	bgraphics_pixel_format_a1_r5_g5_b5,
	bgraphics_pixel_format_r5_g5_b5_a1,
	bgraphics_pixel_format_a4_r4_g4_b4,
	bgraphics_pixel_format_r4_g4_b4_a4,
#endif
	bgraphics_pixel_format_a8_r8_g8_b8,
	bgraphics_pixel_format_y08_cb8_y18_cr8,

//	bgraphics_pixel_format_a0,
//	bgraphics_pixel_format_palette8,
//	bgraphics_pixel_format_a8_palette8,
//	bgraphics_pixel_format_palette2,
//	bgraphics_pixel_format_palette4
};

void filltest(bsurface_t fb, bgraphics_pixel_format pixel_format)
{
	bsurface_t surface1;
	bsurface_create_settings create_settings;
	unsigned i;

	/* draw a box into the fb */
#if 0
	unsigned long color = 0xFF000000;
#define GRAD_HEIGHT 20
	for (i=0;i<10;i++) {
		bsettop_rect r = {0, i*GRAD_HEIGHT, 200, GRAD_HEIGHT};
		bsurface_fill(fb, &r, color);
		color += 0x1010;
	}
#else
	unsigned long color;
	color = 0xcdab4521; // customer color
//	color = 0xFFFFFFFF; // ARGB8888 white
//	color = 0xFFFF8080; // AYCrCb8888 white

	bsettop_rect r = {0, 0, 200, 200};
	bsurface_fill(fb, &r, color);
/*
RGB->YCrCb: 0xFFFFFFFF => EA80EA80 (should be FF80FF80)
*/
#endif

	bgraphics_sync(graphics);

	bsurface_memory mem;
	bsurface_get_memory(fb, &mem);
	printf("fb memory: %08lX\n", *(unsigned long*)mem.buffer);
	prompt();

	for (i=0;i<sizeof(pxl)/sizeof(bgraphics_pixel_format);i++) {
		printf("fill %d, copy %d => %d: copy\n", pxl[i], pxl[i], pixel_format);
		bsurface_create_settings_init(&create_settings, graphics);
		create_settings.width = r.width;
		create_settings.height = r.height;
		create_settings.pixel_format = pxl[i];
		surface1 = bsurface_create(graphics, &create_settings);
		bsurface_fill(surface1, &r, color);

		bsettop_rect dest = {250+i*20,100+i*20,r.width, r.height};
		bsurface_copy(fb, &dest, surface1, &r);
		bgraphics_sync(graphics);

		bsurface_get_memory(surface1, &mem);
		printf("surface1 memory: %08lX\n", *(unsigned long*)mem.buffer);
		
		prompt();
		bsurface_destroy(surface1);
	}
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	display = bdisplay_open(B_ID(0));

	unsigned i;
	for (i=0;i<sizeof(pxl)/sizeof(bgraphics_pixel_format);i++) {
		bsurface_create_settings create_settings;
		bsurface_t fb;
		bsurface_settings surface_settings;

		graphics = bgraphics_open(B_ID(0),display);
		bsurface_create_settings_init(&create_settings, graphics);
		create_settings.pixel_format = pxl[i];
		fb = bgraphics_create_framebuffer(graphics, &create_settings, false);

		bsurface_get(fb, &surface_settings);
		bsettop_rect r1 = {0,0,surface_settings.position.width,surface_settings.position.height};
		bsurface_fill(fb, &r1, 0xFF000000);
		printf("framebuffer %d: fill\n", pxl[i]);

		filltest(fb, pxl[i]);

		bgraphics_close(graphics);
	}


	return rc;
}
