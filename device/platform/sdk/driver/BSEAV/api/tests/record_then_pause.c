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
 * $brcm_Workfile: record_then_pause.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/record_then_pause.c $
 * 
 * 3   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include "bcmindexer.h"
#include <stdio.h>
#include <sys/stat.h>

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(B_ID(0));
	brecord_t record = brecord_open(B_ID(0));
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bplayback_t playback = bplayback_open(B_ID(0));
	btuner_qam_params qam;
	bstream_t livestream, playstream;
	bplayback_params playback_params;
	bstream_mpeg mpeg;
	bband_t band;
	bplayback_file_t playfile;
	brecord_file_t recfile = brecord_file_open("stream.mpg", "stream.nav");
	brecord_params params;
	if (rc) goto error;

	btuner_qam_params_init(&qam, tuner);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x11;
	mpeg.audio[0].pid = 0x14;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x11;

	brecord_params_init(&params, record);
	params.index_format = bindex_format_bcm;

	band = btuner_tune_qam(tuner, 765000000, &qam);
	if (!band) goto error;

	livestream = bstream_open(band, &mpeg);
	if (!livestream) goto error;

	if (brecord_start(record, livestream, recfile, &params))
		goto error;

	if (bdecode_start(decode, livestream, window))
		goto error;
		
	/* At this point, we're recording in the background and doing live decode. */

	getchar(); /* press enter to stop decode */

	/* Before we stop, get the current PTS and make sure it's valid. */
	bdecode_status decode_status;
	bdecode_get_status(decode, &decode_status);
	printf("Current PTS: %08lX\n", decode_status.video_pts);

	/* Now stop decode and start playback at the location we want to be at.
	To make this test have a clean transition, we have to set
	BMVD_SetChannelChangeMode(chn, BMVD_ChannelChangeMode_eLastFramePreviousChannel).
	This can be added in bsettop_base_mvd.c into bsettop_p_mvd_init.
	If you don't set this, at best you'll get a 1/2 second of black and then a still frame
	and at worst, you get all black until you start the trick mode. It's intermittent
	because the play-then-pause is a race condition. The first frame may or may not
	get out. */

	bdecode_stop(decode);

	playfile = bplayback_file_open("stream.mpg", "stream.nav");
	if (!playfile) goto error;

	bplayback_params_init(&playback_params, playback);
	playback_params.index_format = bindex_format_bcm;

	playstream = bplayback_start_timeshifting(playback, &mpeg, playfile, &playback_params,
		record);
	if (!playstream) goto error;

	/* We have no bplayback function to go to a PTS. So this is a hack. */
	struct stat st;
	stat("stream.nav", &st);
	bplayback_goto_index(playback, st.st_size / sizeof(BNAV_Entry), SEEK_END);

	if (bdecode_start(decode, playstream, window))
		goto error;

	bplayback_pause(playback);
	getchar(); /* press enter to stop decode */

	/* Now start a trick mode from that location. Should be from the right location. */
	bplayback_trickmode_params playback_trickmode_params;
	playback_trickmode_params.mode = bplayback_player_mode_brcm;
	playback_trickmode_params.mode_modifier = -1;
	playback_trickmode_params.slow_motion_rate = 1;
	bplayback_trickmode(playback, &playback_trickmode_params);
	getchar(); /* press enter to stop decode */

	bdecode_stop(decode);
	bplayback_stop(playback);
	brecord_stop(record);
	bsettop_uninit();

	return 0;
error:
	return 1;
}
