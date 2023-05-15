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
 * $brcm_Workfile: blended_fill.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/blended_fill.c $
 * 
 * 2   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bsurface_create_settings create_settings;
	bsurface_settings surface_settings;
	bgraphics_settings gs;
	bsurface_t fb;
	bsettop_rect r = {0,0,300,200};
	bsettop_rect upper_left, upper_right;
	bdecode_t decode = bdecode_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;

	if (rc) goto error;

	/* get a decode going so we can see alpha in the fb */
	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x11;
	mpeg.pcr_pid = 0x11;
	band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
	stream = bstream_open(band, &mpeg);
	if (!stream) 
		goto error;
	if (bdecode_start(decode, stream, window))
		goto error;
	
	bsurface_create_settings_init(&create_settings, graphics);
	create_settings.pixel_format = bgraphics_pixel_format_a8_r8_g8_b8;
	fb = bgraphics_create_framebuffer(graphics, &create_settings, false);

	bgraphics_get(graphics, &gs);
	gs.chromakey_enabled = false;
	bgraphics_set(graphics, &gs);
	
	bsurface_get(fb, &surface_settings);
	r = surface_settings.position;
	upper_left = r;
	upper_left.width /= 2;
	upper_left.height /= 2;
	upper_right = upper_left;
	upper_right.x += upper_right.width;

	/**
	all test start with solid white fill.
	all subsequent operations should be sub-fills or blended fills.
	for testing purposes, the white background should be thought of as a customer's UI or graphic. it cannot be recreated.
		any fill on top cannot change the color channel of the white background.
	**/
	
	bsurface_fill(fb, &r, 0xFFFFFFFF); /* white */
	bsurface_sync(fb);
	printf("white background\n");
	getchar();
	
	bsurface_fill(fb, &r, 0xFFFFFFFF); /* white */
	bsurface_fill(fb, &upper_right, 0xFF000000); 
	bsurface_fill(fb, &upper_left, 0xFF808080); 
	bsurface_sync(fb);
	printf("solid gray/black boxes (fill)\n");
	getchar();
	
	bsurface_fill(fb, &r, 0xFFFFFFFF); /* white */
	bsurface_fill(fb, &upper_right, 0xFF000000); 
	bsurface_blended_fill(fb, &upper_left, 0x80000000, /* 50% in the blue channel will produce yellow */
		BSURFACE_FILL_BLEND_WITH_PIXEL_ALPHA|BSURFACE_FILL_DONT_CHANGE_ALPHA); 
	bsurface_sync(fb);
	/* the alpha value is used to blend colors only, but not set the alpha channel */
	printf("solid gray/black boxes (blended fill)\n");
	getchar();
	
	bsurface_fill(fb, &r, 0xFFFFFFFF); /* white */
	bsurface_fill(fb, &upper_right, 0xFF000000); 
	bsurface_fill(fb, &upper_left, 0x80808080); /* copy color and alpha, no blend */
	bsurface_sync(fb);
	printf("50%% alpha gray box (fill)\n");
	getchar();
	
	bsurface_fill(fb, &r, 0xFFFFFFFF); /* white */
	bsurface_fill(fb, &upper_right, 0xFF000000); 
	bsurface_blended_fill(fb, &upper_left, 0x80000000,
		BSURFACE_FILL_BLEND_WITH_PIXEL_ALPHA|BSURFACE_FILL_SET_ALPHA_WITH_PIXEL_ALPHA);
	bsurface_sync(fb);
	/* blend colors using alpha, copy alpha */
	printf("50%% alpha gray box (blended fill)\n");
	getchar();

	bsurface_fill(fb, &r, 0xFFFFFFFF); /* white */
	bsurface_fill(fb, &upper_right, 0x80808080); 
	bsurface_blended_fill(fb, &upper_left, 0x80808080,
		BSURFACE_FILL_COPY_COLOR);
	bsurface_sync(fb);
	printf("50%% alpha gray box (fill + blended fill)\n");
	getchar();

	bsurface_fill(fb, &r, 0x80FFFFFF); /* white */
	bsurface_sync(fb);
	printf("50%% alpha white background (fill)\n");
	getchar();
	
	bsurface_fill(fb, &r, 0xFFFFFFFF); /* white */
	bsurface_blended_fill(fb, &r, 0x80000000, /* only alpha matters */
		BSURFACE_FILL_BLEND_DONT_CHANGE_COLOR|BSURFACE_FILL_SET_ALPHA_WITH_PIXEL_ALPHA);
	bsurface_sync(fb);
	printf("50%% alpha white background (blended fill)\n");
	getchar();
	
	/* shutdown */
	bdecode_stop(decode);
	bdecode_close(decode);
	bgraphics_close(graphics);
	bdecode_window_close(window);
	bdisplay_close(display);	
	bsettop_uninit();
	return 0;
error:
	return 1;
}
