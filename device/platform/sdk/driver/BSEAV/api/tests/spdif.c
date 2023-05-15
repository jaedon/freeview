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
 * $brcm_Workfile: spdif.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/spdif.c $
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

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner0 = btuner_open(B_ID(0));
	bdecode_t decode0 = bdecode_open(B_ID(0));
	bdisplay_t display0 = bdisplay_open(B_ID(0));
	bdecode_window_t window0 = bdecode_window_open(B_ID(0), display0);
	btuner_t tuner1 = btuner_open(B_ID(1));
	bdecode_t decode1 = bdecode_open(B_ID(1));
	bdisplay_t display1 = bdisplay_open(B_ID(1));
	bdecode_window_t window1 = bdecode_window_open(B_ID(1), display1);
	btuner_qam_params qam;
	bstream_t stream0, stream1;
	bstream_mpeg mpeg;
	bband_t band0, band1;
	bdisplay_settings disp_cfg0, disp_cfg1;
	boutput_spdif_settings spdif_settings;

	if (rc) goto error;

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
	band0 = bstreamer_attach( B_ID(2), bstream_mpeg_type_ts );
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
	band1 = bstreamer_attach( B_ID(2), bstream_mpeg_type_ts );
#else
	printf("Decoding from a QAM demod...\n");
	band1 = btuner_tune_qam(tuner1, 765000000, &qam);
#endif

	if (!band1) goto error;

	stream1 = bstream_open(band1, &mpeg);
	if (!stream0) goto error;

	if (bdecode_start(decode1, stream1, window1))
		goto error;

	printf("PCM SPDIF connected to the first decode, Press any key to change SPDIF output to AC3"); getchar(); 

	bdecode_stop(decode0);	

	bdisplay_get(display0, &disp_cfg0);
	boutput_spdif_get(disp_cfg0.spdif, &spdif_settings);
	spdif_settings.pcm = false;
	boutput_spdif_set(disp_cfg0.spdif, &spdif_settings);
	bdisplay_set(display0, &disp_cfg0);
	if (bdecode_start(decode0, stream0, window0))
		goto error;

	printf("Press any key to change SPDIF output to the second output"); getchar();

	bdecode_stop(decode0);
	bdecode_stop(decode1);

	bdisplay_get(display0, &disp_cfg0);
	bdisplay_get(display1, &disp_cfg1);
	disp_cfg1.spdif = disp_cfg0.spdif;
	disp_cfg0.spdif = NULL;
	bdisplay_set(display0, &disp_cfg0);
	bdisplay_set(display1, &disp_cfg1);

	if (bdecode_start(decode0, stream0, window0))
		goto error;
	if (bdecode_start(decode1, stream1, window1))
		goto error;

	printf("Press any key to change SPDIF type to PCM"); getchar();
	bdisplay_get(display1, &disp_cfg1);
	boutput_spdif_get(disp_cfg1.spdif, &spdif_settings);
	spdif_settings.pcm = true;
	boutput_spdif_set(disp_cfg1.spdif, &spdif_settings);
	bdisplay_set(display1, &disp_cfg1);
	printf("Press any key to continue"); getchar();

	bsettop_uninit();
	return 0;
error:
	return 1;
}
