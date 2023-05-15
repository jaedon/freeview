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
 * $brcm_Workfile: hdsdsimul.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/hdsdsimul.c $
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

int opengraphics(bdisplay_t display)
{
	bgraphics_t graphics = bgraphics_open(B_ID(0),display);
	bsurface_create_settings create_settings;
	bsurface_settings surface_settings;
	bgraphics_settings graphics_settings;
	bsurface_t surface1;
	bsettop_rect r = {0,0,300,200};

	bsurface_create_settings_init(&create_settings, graphics);
	create_settings.width = 300;
	create_settings.height = 200;
	create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
	surface1 = bsurface_create(graphics, &create_settings);
	bsurface_get(surface1, &surface_settings);
	surface_settings.position.x = 50;
	surface_settings.position.y = 50;
	surface_settings.visible = true;
	surface_settings.zorder = 0;
	bsurface_set(surface1, &surface_settings);
	bsurface_fill(surface1, &r, 0xFFFFFFFF); /* white */

	return 0;
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(B_ID(0));
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display0 = bdisplay_open(B_ID(0));
	bdisplay_t display1 = bdisplay_open(B_ID(1));
	bdecode_window_t window0 = bdecode_window_open(B_ID(0), display0);
	bdecode_window_t window1 = bdecode_window_clone(window0, B_ID(2), display1);
	btuner_qam_params qam;
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	bdisplay_settings disp_cfg;

	bdisplay_get(display0, &disp_cfg);
	disp_cfg.format = bvideo_format_1080i;
	disp_cfg.composite = NULL;
	disp_cfg.svideo = NULL;
	disp_cfg.rf = NULL;
	bdisplay_set(display0, &disp_cfg);

	bdisplay_get(display1, &disp_cfg);
	disp_cfg.format = bvideo_format_ntsc;
	disp_cfg.composite = boutput_composite_open(B_ID(0));
	disp_cfg.svideo = boutput_svideo_open(B_ID(0));
	disp_cfg.rf = boutput_rf_open(B_ID(0));
	bdisplay_set(display1, &disp_cfg);

	opengraphics(display0);

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

	if (bdecode_start(decode, stream, window0))
		goto error;

	printf("Press any key to continue");getchar();

	bsettop_uninit();
	return 0;
error:
	return 1;
}

