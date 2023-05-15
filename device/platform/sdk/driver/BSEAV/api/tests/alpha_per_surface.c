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
 * $brcm_Workfile: alpha_per_surface.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/alpha_per_surface.c $
 * 
 * 2   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>

/* You can run in 2 modes.
1) explicit blit calls with no bgraphics_sync.
2) implicit blit calls using bgraphics_sync.
*/
//#define MANUAL_BLIT

static uint32_t color[10] = {
0xff16231b,
0xff16e9e8,
0xff90cde7,
0xffef438d,
0xff0e0f76,
0xff52255a,
0xff9cf92e,
0xffed7263,
0xffdcc233,
0xffefd79f
};

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bsurface_create_settings create_settings;
	bsurface_settings fb_settings;
	bsurface_t framebuffer;
	bsettop_rect rect = {0,0,200,200};
	int i, j;

	if (rc) goto error;

	/* Get the framebuffer (or create on-screen surface) */
	bsurface_create_settings_init(&create_settings, graphics);
	create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
	framebuffer = bgraphics_create_framebuffer(graphics, &create_settings, false);
	bsurface_get(framebuffer, &fb_settings);

	/* create several off-screen surfaces with various alpha values */
	for (j=0;j<5;j++) {
		bsurface_t surface[10];
		for (i=0;i<10;i++) {
			bsurface_settings settings;

			/* Create off-screen surface */
			create_settings.width = rect.width;
			create_settings.height = rect.height;
			create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
			surface[i] = bsurface_create(graphics, &create_settings);

			bsurface_get(surface[i], &settings);
			settings.visible = true;
			settings.position.x = 20 + 20*i;
			settings.position.y = 20 + 20*i;
			settings.zorder = i;

			switch (j) {
			case 0: settings.alpha = 0xFF; break;
			case 1: settings.alpha = 0xFF / (i+1); break;
			case 2: settings.alpha = 0xFF; break;
			case 3: settings.alpha = 0xa0; break;
			case 4: settings.alpha = 0xFF; break;
			}
			//if (i == 9) settings.alpha = 0xFF;
			bsurface_set(surface[i], &settings);

			//uint32_t color = rand() % 0xFFFFFFFF | 0xFF000000;
			printf("fill color=0x%06x, alpha=0x%x\n", color[i], settings.alpha);
			bsurface_fill(surface[i], &rect, color[i]);
			//bgraphics_sync(graphics);
		}
		bgraphics_sync(graphics);
		getchar();

		for (i=0;i<10;i++)
			bsurface_destroy(surface[i]);
	}

	return 0;
error:
	return 1;
}

