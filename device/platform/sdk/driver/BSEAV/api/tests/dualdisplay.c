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
 * $brcm_Workfile: dualdisplay.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/dualdisplay.c $
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

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner0 = btuner_open(B_ID(0));
	bdecode_t decode0 = bdecode_open(B_ID(0));
	bdisplay_t display0 = bdisplay_open(B_ID(0));
	bdecode_window_t window0 = bdecode_window_open(B_ID(0), display0);
	bgraphics_t graphics0 = bgraphics_open(B_ID(0),display0);
	btuner_t tuner1 = btuner_open(B_ID(1));
	bdecode_t decode1 = bdecode_open(B_ID(1));
	bdisplay_t display1 = bdisplay_open(B_ID(1));
	bdecode_window_t window1 = bdecode_window_open(B_ID(1), display1);
	bgraphics_t graphics1 = bgraphics_open(B_ID(1),display1);
	btuner_qam_params qam;
	bstream_t stream0, stream1;
	bstream_mpeg mpeg;
	bband_t band0, band1;
	bsurface_create_settings cfg;
	bsurface_t framebuffer0;
	bsurface_t framebuffer1;
	bsettop_rect rect;
	bdecode_window_settings win_cfg;
	bdisplay_settings display_settings0, display_settings1;

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

	btuner_qam_params_init(&qam, tuner0);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x21;

#if USE_STREAMER
	printf("Decoding from a streamer...\n");
	band0 = bstreamer_attach( B_ID(0), bstream_mpeg_type_ts );
#else
	printf("Decoding from a QAM demod...\n");
	band0 = btuner_tune_qam(tuner0, 765000000, &qam);
#endif

	if (!band0) goto error;

	stream0 = bstream_open(band0, &mpeg);
	if (!stream0) goto error;

	if (bdecode_start(decode0, stream0, window0))
		goto error;

	printf("First decode started, press any key to start second decode...");
	getchar(); 


	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x11;
	mpeg.audio[0].pid = 0x14;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x11;

#if USE_STREAMER
	printf("Decoding from a streamer...\n");
	band1 = band0;
#else
	printf("Decoding from a QAM demod...\n");
	band1 = btuner_tune_qam(tuner1, 765000000, &qam);
#endif

	if (!band1) goto error;

	stream1 = bstream_open(band1, &mpeg);
	if (!stream1) goto error;

	if (bdecode_start(decode1, stream1, window1))
		goto error;

	printf("Dual decode started, press any key to resize first window");getchar();
	if (bdecode_window_get(window0, &win_cfg)) goto error;

	win_cfg.position.width /= 2;
	win_cfg.position.height /= 2;

	if (bdecode_window_set(window0, &win_cfg)) goto error;
	
	printf("press any key to resize second window");getchar();
	if (bdecode_window_get(window1, &win_cfg)) goto error;

	win_cfg.position.width /= 2;
	win_cfg.position.height /= 2;

	if (bdecode_window_set(window1, &win_cfg)) goto error;

	printf("press any key to return window size of the second window");getchar();
	if (bdecode_window_get(window1, &win_cfg)) goto error;

	win_cfg.position.width *= 2;
	win_cfg.position.height *= 2;

	if (bdecode_window_set(window1, &win_cfg)) goto error;

	printf("press any key to return window size of the first window");getchar();
	if (bdecode_window_get(window0, &win_cfg)) goto error;

	win_cfg.position.width *= 2;
	win_cfg.position.height *= 2;

	if (bdecode_window_set(window0, &win_cfg)) goto error;

	printf("Dual decode is going, press any key activate graphics...");
	getchar(); 

	bsurface_create_settings_init(&cfg, graphics0);

	framebuffer0 = bsurface_get_display(graphics0, &cfg);

	rect.x=rect.y=0;
	rect.width=cfg.width/2;
	rect.height=cfg.height/2;
	bsurface_fill(framebuffer0, &rect, PIXEL(0xFF, 0, 0));
	rect.x=rect.width;
	bsurface_fill(framebuffer0, &rect, PIXEL(0, 0, 0xFF));
	rect.x=0;
	rect.y=cfg.height/2;
	bsurface_fill(framebuffer0, &rect, PIXEL(0xFF, 0xFF, 0xFF));
	rect.x=rect.width;
	rect.height=cfg.height/4;
	bsurface_fill(framebuffer0, &rect, PIXEL(0, 0xFF, 0));

	bgraphics_sync(graphics0);

	bsurface_create_settings_init(&cfg, graphics1);

	framebuffer1 = bsurface_get_display(graphics1, &cfg);

	rect.x=rect.y=0;
	rect.width=cfg.width/2;
	rect.height=cfg.height/2;
	bsurface_fill(framebuffer1, &rect, PIXEL(0, 0, 0xFF));
	rect.x=rect.width;
	bsurface_fill(framebuffer1, &rect, PIXEL(0xFF, 0, 0));
	rect.x=0;
	rect.y=cfg.height/2;
	bsurface_fill(framebuffer1, &rect, PIXEL(0xFF, 0xFF, 0xFF));
	rect.x=rect.width;
	rect.height=cfg.height/4;
	bsurface_fill(framebuffer1, &rect, PIXEL(0xFF, 0, 0xFF));

	bgraphics_sync(graphics1);


	printf("Press any key to stop decode 0..."); getchar(); 
	bdecode_stop(decode0);
	printf("Press any key to stop decode 1..."); getchar(); 
	bdecode_stop(decode1);

	printf("Press any key to start decodes..."); getchar(); 
	if (bdecode_start(decode0, stream0, window0)) goto error;
	if (bdecode_start(decode1, stream1, window1)) goto error;

	printf("Press any key to close streams and swap outputs...");getchar(); 

	bstream_close(stream0);
	bstream_close(stream1);

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x21;

#if USE_STREAMER
	band0 = band1;
#else
	printf("Decoding from a QAM demod...\n");
	band0 = btuner_tune_qam(tuner0, 765000000, &qam);
#endif
	if (!band0) goto error;

	stream0 = bstream_open(band0, &mpeg);
	if (!stream0) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x11;
	mpeg.audio[0].pid = 0x14;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x11;

	
#if USE_STREAMER
	printf("Decoding from a streamer...\n");
	band1 = bstreamer_attach( B_ID(2), bstream_mpeg_type_ts );
#else
	printf("Decoding from a QAM demod...\n");
	band1 = btuner_tune_qam(tuner1, 765000000, &qam);
#endif
	if (!band1) goto error;

	stream1 = bstream_open(band1, &mpeg);
	if (!stream1) goto error;

	if (bdecode_start(decode0, stream1, window0)) goto error;
	if (bdecode_start(decode1, stream0, window1)) goto error;


	printf("Press any key to continue..."); getchar(); 

	bsettop_uninit();
	return 0;
error:
	return 1;
}
