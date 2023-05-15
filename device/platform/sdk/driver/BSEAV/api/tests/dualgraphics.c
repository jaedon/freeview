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
 * $brcm_Workfile: dualgraphics.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/dualgraphics.c $
 * 
 * 2   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>

#define USE_BLITTER

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display0 = bdisplay_open(B_ID(0));
	bgraphics_t graphics0 = bgraphics_open(B_ID(0),display0);
	bdisplay_t display1 = bdisplay_open(B_ID(1));
	bgraphics_t graphics1 = bgraphics_open(B_ID(1),display1);
	bsurface_create_settings create_settings;
	bsurface_settings surface_settings;
	bsurface_t surface0, surface1;
	bdisplay_settings display_settings0, display_settings1;
	bsurface_memory mem0, mem1;
	unsigned i,j;

	if (rc) goto error;

	/* set up dual output */
	bdisplay_get(display0, &display_settings0);
	bdisplay_get(display1, &display_settings1);
	display_settings1.svideo = display_settings0.svideo;
	display_settings1.composite = display_settings0.composite;
	display_settings1.rf = display_settings0.rf;
	display_settings0.svideo = NULL;
	display_settings0.composite = NULL;
	display_settings0.rf = NULL;
	bdisplay_set(display0, &display_settings0);
	bdisplay_set(display1, &display_settings1);

#ifdef USE_BLITTER
	bsurface_create_settings_init(&create_settings, graphics0);
	create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
	surface0 = bsurface_create(graphics0, &create_settings);
	bsurface_get(surface0, &surface_settings);
	surface_settings.visible = true;
	surface_settings.position.x = 0;
	surface_settings.position.y = 0;
	bsurface_set(surface0, &surface_settings);

	bsurface_create_settings_init(&create_settings, graphics1);
	create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
	surface1 = bsurface_create(graphics1, &create_settings);
	bsurface_get(surface1, &surface_settings);
	surface_settings.visible = true;
	bsurface_set(surface1, &surface_settings);
#else
	bsurface_create_settings_init(&create_settings, graphics0);
	surface0 = bsurface_get_display(graphics0, &create_settings);

	bsurface_create_settings_init(&create_settings, graphics1);
	surface1 = bsurface_get_display(graphics1, &create_settings);
#endif

	bsurface_get_memory(surface0, &mem0);
	bsurface_get_memory(surface1, &mem1);
	for(j=0;;j++) {
		for(i=0;i<1000;i++) {
			unsigned x, y;
			unsigned short *buf;

			x = rand() % create_settings.width;
			y = rand() % create_settings.height;
			buf = (unsigned short *)((unsigned)mem0.buffer+y*mem0.pitch);
			buf[x] = rand() | 0xF800; /* red color */

			x = rand() % create_settings.width;
			y = rand() % create_settings.height;
			buf = (unsigned short *)((unsigned)mem1.buffer+y*mem1.pitch);
			buf[x] = rand() | 0x001F; /* blue color */
		}
		printf(".");fflush(stdout);

		bgraphics_sync(graphics0);
		bgraphics_sync(graphics1);
	}

	getchar(); /* press enter to stop decode */
	return 0;
error:
	return 1;
}
