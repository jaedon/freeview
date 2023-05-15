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
 * $brcm_Workfile: format_change.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/format_change.c $
 * 
 * 2   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>

#ifndef USE_STREAMER
#define USE_STREAMER 0
#endif

#define PIXEL888(r,g,b) ((uint32_t)(((r&0xFF)<<(16)) | ((g&0xFF)<<8) | (b&0xFF)))
#define PIXEL(r,g,b) PIXEL888(r,g,b)
//#define PIXEL(r,g,b) ((uint32_t)(((r&0xFF)<<(16)) | 0xFF000000 | ((g&0xFF)<<8) | (b&0xFF)))
//
static void 
draw_graphics(bgraphics_t graphics, bsurface_t framebuffer, unsigned width, unsigned height)
{
	bsettop_rect rect;
	rect.x=rect.y=0;
	rect.width=width/2;
	rect.height=height/2;
	bsurface_fill(framebuffer, &rect, PIXEL(0xFF, 0, 0));
	rect.x=rect.width;
	bsurface_fill(framebuffer, &rect, PIXEL(0, 0, 0xFF));
	rect.x=0;
	rect.y=height/2;
	bsurface_fill(framebuffer, &rect, PIXEL(0xFF, 0xFF, 0xFF));
	rect.x=rect.width;
	rect.height=height/4;
	bsurface_fill(framebuffer, &rect, PIXEL(0, 0xFF, 0));

	bgraphics_sync(graphics);
	return;
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(B_ID(0));
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	btuner_qam_params qam;
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bsurface_create_settings cfg;
	bsurface_settings scfg;
	bsurface_t framebuffer;
	bdisplay_settings disp_cfg;
	bgraphics_settings gfx_cfg;


	if (rc) goto error;

	btuner_qam_params_init(&qam, tuner);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x21;

#if USE_STREAMER
	printf("Decoding from a streamer...\n");
	band = bstreamer_attach( B_ID(2), bstream_mpeg_type_ts );
#else
	printf("Decoding from a QAM demod...\n");
	band = btuner_tune_qam(tuner, 765000000, &qam);
#endif

	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	bsurface_create_settings_init(&cfg, graphics);

	cfg.width = 1920;
	cfg.height = 1080;

	framebuffer = bsurface_get_display(graphics, &cfg);

	bgraphics_get(graphics, &gfx_cfg);
	gfx_cfg.enabled = true;
	bgraphics_set(graphics, &gfx_cfg);
	draw_graphics(graphics, framebuffer, 720, 480);

	printf("Press any key to change format to 480i");getchar();
	bdisplay_get(display, &disp_cfg);
	disp_cfg.format = bvideo_format_ntsc;
	bdisplay_set(display, &disp_cfg);

	printf("Press any key to reenable graphics");getchar();
	bgraphics_get(graphics, &gfx_cfg);
	gfx_cfg.enabled = true;
	bgraphics_set(graphics, &gfx_cfg);
	draw_graphics(graphics, framebuffer, 720, 480);
	for(;;) {
	printf("Press any key to change format to 1080i");getchar();
	disp_cfg.format = bvideo_format_1080i;
	disp_cfg.composite = NULL;
	disp_cfg.svideo = NULL;
	disp_cfg.rf = NULL;
	bdisplay_set(display, &disp_cfg);

	printf("Press any key to reenable graphics");getchar();
	bgraphics_get(graphics, &gfx_cfg);
	gfx_cfg.enabled = true;
	bgraphics_set(graphics, &gfx_cfg);
	draw_graphics(graphics, framebuffer, 1920, 1080);
	
	printf("Press any key to change format to 720p");getchar();
	disp_cfg.format = bvideo_format_720p;
	disp_cfg.composite = NULL;
	disp_cfg.svideo = NULL;
	disp_cfg.rf = NULL;
	bdisplay_set(display, &disp_cfg);

	printf("Press any key to reenable graphics");getchar();
	bgraphics_get(graphics, &gfx_cfg);
	gfx_cfg.enabled = true;
	bgraphics_set(graphics, &gfx_cfg);
	draw_graphics(graphics, framebuffer, 1280, 720);

	printf("Press any key to change format to 480p");getchar();
	disp_cfg.format = bvideo_format_480p;
	disp_cfg.composite = NULL;
	disp_cfg.svideo = NULL;
	disp_cfg.rf = NULL;
	bdisplay_set(display, &disp_cfg);

	printf("Press any key to reenable graphics");getchar();
	bgraphics_get(graphics, &gfx_cfg);
	gfx_cfg.enabled = true;
	bgraphics_set(graphics, &gfx_cfg);
	draw_graphics(graphics, framebuffer, 720, 480);
		
	printf("Press any key to change format to 480i");getchar();
	disp_cfg.format = bvideo_format_ntsc;
	disp_cfg.composite = boutput_composite_open(B_ID(0));
	disp_cfg.svideo = boutput_svideo_open(B_ID(0));
	disp_cfg.rf = boutput_rf_open(B_ID(0));
	bdisplay_set(display, &disp_cfg);

	printf("Press any key to reenable graphics");getchar();
	bgraphics_get(graphics, &gfx_cfg);
	gfx_cfg.enabled = true;
	bgraphics_set(graphics, &gfx_cfg);
	draw_graphics(graphics, framebuffer, 720, 480);
	}

	printf("Press any key to continue");getchar();

	bsettop_uninit();
	return 0;
error:
	return 1;
}
