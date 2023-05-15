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
 * $brcm_Workfile: graphics_test2.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/graphics_test2.c $
 * 
 * 5   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct
{
	char name[10];
	bgraphics_pixel_format format;
} pixel_format_test;

const int TEST_COUNT = 13;

pixel_format_test tests[] = 
{
	{ "Exit", 0 },
	{ "RGB565", bgraphics_pixel_format_r5_g6_b5 },
	{ "ARGB1555", bgraphics_pixel_format_a1_r5_g5_b5 },
	{ "RGBA5551", bgraphics_pixel_format_r5_g5_b5_a1 },
	{ "ARGB4444", bgraphics_pixel_format_a4_r4_g4_b4 },
	{ "RGBA4444", bgraphics_pixel_format_r4_g4_b4_a4 },
	{ "A8PALETTE8", bgraphics_pixel_format_a8_palette8 },
	{ "ARGB8888", bgraphics_pixel_format_a8_r8_g8_b8 },
	{ "PALETTE8", bgraphics_pixel_format_palette8 },
	{ "VTUNNEL", bgraphics_pixel_format_video_tunnel },
	{ "YCbYCr8888", bgraphics_pixel_format_y08_cb8_y18_cr8 },
	{ "PALETTE2", bgraphics_pixel_format_palette2 },
	{ "PALETTE4", bgraphics_pixel_format_palette4 },
};

bsurface_t create_green_surface(bgraphics_t graphics,bgraphics_pixel_format format);
bresult run_test(bgraphics_t graphics, pixel_format_test test);

int main(int argc, char* argv[]) 
{
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bool sequenced = false;
	int i;

	if (rc) goto error;

	if (argc > 1)
	{
		if (strcmp(argv[1], "-s") == 0)
		{
			sequenced = true;
		}
	}

	printf("This program tests which pixel formats are supported in"
		" the set-top api\n");

	if (sequenced)
	{
		for (i = 1; i < TEST_COUNT; i++)
		{
			run_test(graphics, tests[i]);
		}
	}
	else
	{
		while (1)
		{
			printf("Menu:\n");
			for (i = 0; i < TEST_COUNT; i++)
			{
				printf("%d) %s\n", i, tests[i].name);
			}
			printf("> ");

			scanf("%d", &i);
			getchar();

/*			if (isdigit(i))
			{
				i = i - 48;
			}*/

			if (i == 0)
			{
				break;
			}
			else if (i > 0 && i < TEST_COUNT)
			{
				run_test(graphics, tests[i]);
			}
			else
			{
				printf("Invalid selection.  Please try again.\n\n");
			}
		}
	}

error:

	bgraphics_close(graphics);
	exit(rc);
}

bresult run_test(bgraphics_t graphics, pixel_format_test test)
{
	bsurface_t surface;
	bsurface_memory mem;
	bsurface_t fb;
    
	surface = create_green_surface(graphics, test.format);
	if (!surface) return berr_out_of_memory;

	bgraphics_sync(graphics);
	bsurface_get_memory(surface, &mem);
	printf("pixel[0,0] = %08lx\n", *(unsigned long *)mem.buffer);
	fb = bsurface_get_display(graphics, NULL);
	if (fb) {
		bsurface_get_memory(fb, &mem);
		printf("fb[0,0] = %08lx\n", *(unsigned long *)mem.buffer);
	}

	bsurface_destroy(surface);
	return b_ok;
}

bsurface_t create_green_surface(bgraphics_t graphics, bgraphics_pixel_format format)
{
	bresult rc;
	bsurface_t surface;
	bsurface_create_settings create_settings;
	bsurface_settings surface_settings;
	bsettop_rect r = {0,0,720,480};
	bgraphics_palette palette;

	bsurface_create_settings_init(&create_settings, graphics);
	create_settings.width = 720;
	create_settings.height = 480;
	create_settings.pixel_format = format;

	surface = bsurface_create(graphics, &create_settings);
	bsurface_get(surface, &surface_settings);

	surface_settings.position.x = 0;
	surface_settings.position.y = 0;
	surface_settings.visible = true;
	surface_settings.zorder = 1;

	rc = bsurface_set(surface, &surface_settings);

	switch( format )
	{
		case bgraphics_pixel_format_palette2:
		case bgraphics_pixel_format_palette4:
		case bgraphics_pixel_format_palette8:
		case bgraphics_pixel_format_a8_palette8:
			{
				const bgraphics_pixel clut2_table[] = {
					0x000000ff,
					0xff0000ff,  // blue
					0xffff0000,  // red
					0xff00ff00   // green
					};

				palette.palette = clut2_table;
				palette.length = (sizeof (clut2_table) / sizeof(uint32_t));
				printf(" Loading Clut Table %d\n", palette.length);
				rc = bsurface_load_clut(surface, palette);
			}
			break;
		default:
			break;
	}

	if (rc) goto error;

	switch( format )
	{
		case bgraphics_pixel_format_palette2:
			rc = bsurface_fill(surface, &r, 0x02); /* red */
			break;
		case bgraphics_pixel_format_palette4:
			rc = bsurface_fill(surface, &r, 0x03);	 /* green */
			break;
		case bgraphics_pixel_format_palette8:
			rc = bsurface_fill(surface, &r, 0x01);  /* blue */
			break;
		case bgraphics_pixel_format_a8_palette8:
			rc = bsurface_fill(surface, &r, 0xFF01);  /* blue */
			break;
		default:
			rc = bsurface_fill(surface, &r, 0xFF00FF00); /* green */
			break;
	}

	if (rc) goto error;

	return surface;

error:

	printf("Error occurred during test: %d\n", rc);
	if (!surface)
	{
		bsurface_destroy(surface);
	}
	return NULL;
}

