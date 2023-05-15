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
 * $brcm_Workfile: video_transition.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/video_transition.c $
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

#define PIXEL(r,g,b) ((uint32_t)(((r&0xFF)<<(16)) | ((g&0xFF)<<8) | (b&0xFF)))

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
	bdecode_window_settings win_cfg;
	btuner_analog_params analog;
	btuner_linein_t linein;
	unsigned i;
	bgraphics_t graphics;
	bsurface_t framebuffer;
	bsettop_rect rect;
	bsurface_create_settings cfg;

	if (rc) goto error;

	btuner_qam_params_init(&qam, tuner);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x21;

	for(i=0;i<3;i++) {
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

	printf("wait for decode to come up and press enter to stop decode"); getchar(); 
	bdecode_stop(decode);
	printf("press enter to start decode again"); getchar();
	if (bdecode_start(decode, stream, window))
		goto error;
	printf("wait for decode to come up and press enter to hide a window"); getchar(); 
	if (bdecode_window_get(window, &win_cfg)) 
		goto error;
	win_cfg.visible=false;
	if (bdecode_window_set(window, &win_cfg)) 
		goto error;
	printf("press enter to show window again"); getchar();
	win_cfg.visible=true;
	if (bdecode_window_set(window, &win_cfg)) 
		goto error;
	printf("press enter to resize a window"); getchar();
	win_cfg.position.width /= 2;
	win_cfg.position.height /= 2;
	if (bdecode_window_set(window, &win_cfg)) 
		goto error;
	printf("press enter to restore size of a window"); getchar();
	win_cfg.position.width *= 2;
	win_cfg.position.height *= 2;
	if (bdecode_window_set(window, &win_cfg)) 
		goto error;

	printf("press enter to stop decode and destroy a window"); getchar();
	bdecode_stop(decode);
	bdecode_window_close(window);
	printf("press enter to create window and start decode"); getchar();
	window = bdecode_window_open(B_ID(0), display);
	if (!window) goto error;
	if (bdecode_start(decode, stream, window))
		goto error;
	printf("press enter to stop decode and destroy a window"); getchar();
	bdecode_stop(decode);
	bstream_close(stream);
	printf("press enter to create window and start analog decode"); getchar();
	btuner_analog_params_init(&analog, tuner);
	linein = btuner_linein_open(B_ID(0));
	stream = btuner_tune_linein(tuner, linein, &analog);
	if (bdecode_start(decode, stream, window))
		goto error;

	printf("press enter to start RF analog video");getchar();
	bdecode_stop(decode);
	bstream_close(stream);
	stream = btuner_tune_rf(tuner, 411250000, &analog);
	if (bdecode_start(decode, stream, window))
		goto error;
	printf("press enter to retune RF analog video");getchar();
	bdecode_stop(decode);
	bstream_close(stream);
	stream = btuner_tune_rf(tuner, 411250000, &analog);
	if (bdecode_start(decode, stream, window))
		goto error;

	printf("press enter to resize a window"); getchar();
	win_cfg.position.width /= 2;
	win_cfg.position.height /= 2;
	if (bdecode_window_set(window, &win_cfg)) 
		goto error;

	printf("press enter to close a analog window, create a digital one and start digital decode"); getchar();
	bdecode_stop(decode);
	bstream_close(stream);

	btuner_qam_params_init(&qam, tuner);
	qam.symbol_rate = 5056900;
	qam.mode = 64;
#if USE_STREAMER
	printf("Decoding from a streamer...\n");
	band = bstreamer_attach( B_ID(2), bstream_mpeg_type_ts );
#else
	printf("Decoding from a QAM demod...\n");
	band = btuner_tune_qam(tuner, 765000000, &qam);
#endif
	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;


	if (bdecode_start(decode, stream, window))
		goto error;

	switch(i) {
	case 0:
		printf("press enter to enable graphics"); getchar();
		bdecode_stop(decode);
		bstream_close(stream);
		graphics = bgraphics_open(B_ID(0),display);
		bsurface_create_settings_init(&cfg, graphics);
		framebuffer = bsurface_get_display(graphics, &cfg);
		rect.x=rect.y=0;
		rect.width=cfg.width/2;
		rect.height=cfg.height/2;
		bsurface_fill(framebuffer, &rect, PIXEL(0, 0, 0xFF));
		bgraphics_sync(graphics);
		printf("press enter to start decode"); getchar();
		break;
	case 1:
		printf("press enter to disable graphics"); getchar();
		bdecode_stop(decode);
		bstream_close(stream);
		bgraphics_close(graphics);
		printf("press enter to start decode"); getchar();
		break;
	case 2:
		printf("Done, press enter to exit"); getchar();
		bdecode_stop(decode);
		bstream_close(stream);
		break;
	}
	}


	return 0;
error:
	return 1;
}
