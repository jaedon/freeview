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
 * $brcm_Workfile: audio_vol.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/audio_vol.c $
 * 
 * 4   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	bresult rc;
	bdecode_t decode;
	bdisplay_t display;
	bdecode_window_t window;
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	bdisplay_settings display_settings;
	int curarg = 1;
	int use_streamer = 0;
	int spdif_compressed = 0;
	
	while (curarg < argc) {
		if (!strcmp(argv[curarg], "-spdif=compressed")) {
			spdif_compressed = 1;
		}
		else if (!strcmp(argv[curarg], "-streamer")) {
			use_streamer = 1;
		}
		else {
			printf("Invalid argument: %s\n", argv[curarg]);
			exit(1);
		}
		curarg++;
	}

	rc = bsettop_init(BSETTOP_VERSION);
	if (rc) goto error;
	decode = bdecode_open(B_ID(0));
	display = bdisplay_open(B_ID(0));
	window = bdecode_window_open(B_ID(0), display);

	if (spdif_compressed) {
		bdisplay_get(display, &display_settings);
		if (display_settings.spdif) {
			boutput_spdif_settings spdif_settings;
			boutput_spdif_get(display_settings.spdif, &spdif_settings);
			spdif_settings.pcm = false;
			boutput_spdif_set(display_settings.spdif, &spdif_settings);
		}
		bdisplay_set(display, &display_settings);
	}

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x11;
	mpeg.audio[0].pid = 0x14;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x11;

	if (use_streamer) {
		printf("Decoding from a streamer...\n");
		band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts );
	}
	else {
		btuner_t tuner = btuner_open(B_ID(0));
		btuner_qam_params qam;
		btuner_qam_params_init(&qam, tuner);
		qam.symbol_rate = 5056900;
		qam.mode = 64;
		
		printf("Decoding from a QAM demod...\n");
		band = btuner_tune_qam(tuner, 765000000, &qam);
	}
	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	bdisplay_get(display, &display_settings);
	while (1) {
		baudio_volume vol;
		char buf[80];
		int cmd;

		printf(
		"\nMenu:\n-----------------------\n"
		"1. get all\n"
		"2. set all\n"
		"3. set decoder\n"
		"4. set DAC\n"
		"5. set SPDIF\n"
		"6. set RF\n"
		);
		cmd = atoi(gets(buf));

		if (cmd > 1) {
			printf("Enter vol (-100 to 100) or 'm' for mute:\n");
			gets(buf);
			if (buf[0] == 'm') {
				vol.muted = true;
			}
			else {
				vol.right = vol.left = atoi(buf);
				vol.muted = false;
			}
		}

		switch (cmd) {
		case 2:
			bdecode_set_audio_volume(decode, &vol);
			bdisplay_set_dac_audio_volume(display, &vol);
			if (display_settings.spdif)
				boutput_spdif_set_audio_volume(display_settings.spdif, &vol);
			if (display_settings.rf)
				boutput_rf_set_audio_volume(display_settings.rf, &vol);
			break;
		case 3:
			bdecode_set_audio_volume(decode, &vol);
			break;
		case 4:
			bdisplay_set_dac_audio_volume(display, &vol);
			break;
		case 5:
			if (display_settings.spdif)
				boutput_spdif_set_audio_volume(display_settings.spdif, &vol);
			break;
		case 6:
			if (display_settings.rf)
				boutput_rf_set_audio_volume(display_settings.rf, &vol);
			break;
		}

		printf("\nCurrent state:\n----------------------\n");
		bdecode_get_audio_volume(decode, &vol);
		printf("decoder %d, %d\n", vol.left, vol.right);
		bdisplay_get_dac_audio_volume(display, &vol);
		printf("DAC %d, %d\n", vol.left, vol.right);
		if (display_settings.spdif) {
			boutput_spdif_get_audio_volume(display_settings.spdif, &vol);
			printf("SPDIF %d, %d\n", vol.left, vol.right);
		}
		if (display_settings.rf) {
			boutput_rf_get_audio_volume(display_settings.rf, &vol);
			printf("RF %d, %d\n", vol.left, vol.right);
		}
	}

	return 0;
error:
	return 1;
}
