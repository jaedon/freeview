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
 * $brcm_Workfile: audio_video.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/audio_video.c $
 * 
 * 3   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/* 
 * this tests is used to verify that audio/video could be started independently 
 * and that the audio decoder could decode program not-related to video 
 */
#include "bsettop.h"
#include "bstd.h"
#include <stdio.h>

static void 
wait_key(void)
{
	getchar();
#if 0
	getchar();
#endif
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	btuner_qam_params qam;
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	bvideo_decode_t video;
	baudio_decode_t audio;
	bdecode_t decode_es = bdecode_open(B_ID(2));
	bstream_t stream_es;
	bstream_mpeg mpeg_es;
	bdecode_status status;

	if (rc) goto error;
	BDBG_ASSERT(decode);
	BDBG_ASSERT(decode_es);

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;

	bstream_mpeg_init(&mpeg_es);
	mpeg_es.audio[0].pid = 0x34; /* different pid, than first program */
	mpeg_es.audio[0].format = baudio_format_ac3;

	band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	BDBG_ASSERT(stream);
	stream_es = bstream_open(band, &mpeg_es);
	BDBG_ASSERT(stream_es);

	rc = bdecode_get_status(decode, &status);
	BDBG_ASSERT(rc==b_ok);

	rc = bdecode_start(decode, stream, window);
	BDBG_ASSERT(rc==b_ok);

	printf("prass any key to remove video ");wait_key();
	video = bdecode_detach_video(decode);
	BDBG_ASSERT(video);
	rc = bdecode_get_status(decode, &status);
	BDBG_ASSERT(rc==b_ok);
	printf("prass any key to restore video");wait_key();
	rc = bdecode_attach_video(decode, video);
	BDBG_ASSERT(rc==b_ok);
	rc = bdecode_get_status(decode, &status);
	BDBG_ASSERT(rc==b_ok);

	printf("prass any key to remove audio");wait_key();
	audio = bdecode_detach_audio(decode);
	BDBG_ASSERT(audio);
	rc = bdecode_get_status(decode, &status);
	BDBG_ASSERT(rc==b_ok);

	printf("prass any key to start another audio");wait_key();
	rc = bdecode_attach_audio(decode_es, audio);
	BDBG_ASSERT(rc==b_ok);
	rc = bdecode_get_status(decode_es, &status);
	BDBG_ASSERT(rc==b_ok);
	rc = bdecode_start(decode_es, stream_es, window);
	BDBG_ASSERT(rc==b_ok);
	rc = bdecode_get_status(decode_es, &status);
	BDBG_ASSERT(rc==b_ok);

	printf("prass any key to restore audio");wait_key();
	audio = bdecode_detach_audio(decode_es);
	BDBG_ASSERT(audio);
	rc = bdecode_get_status(decode_es, &status);
	BDBG_ASSERT(rc==b_ok);
	rc = bdecode_attach_audio(decode, audio);
	BDBG_ASSERT(rc==b_ok);
	rc = bdecode_get_status(decode, &status);
	BDBG_ASSERT(rc==b_ok);
	printf("prass any key to stop decode");wait_key();
	bdecode_stop(decode);
	rc = bdecode_get_status(decode, &status);
	BDBG_ASSERT(rc==b_ok);
	bdecode_stop(decode_es);
	rc = bdecode_get_status(decode_es, &status);
	BDBG_ASSERT(rc==b_ok);
	printf("prass any key to start decode (without audio or video)");wait_key();
	audio = bdecode_detach_audio(decode);
	BDBG_ASSERT(audio);
	video = bdecode_detach_video(decode);
	BDBG_ASSERT(video);
	rc = bdecode_start(decode, stream, window);
	BDBG_ASSERT(rc==b_ok);
	rc = bdecode_get_status(decode, &status);
	BDBG_ASSERT(rc==b_ok);
	printf("prass any key to start video");wait_key();
	rc = bdecode_attach_video(decode, video);
	BDBG_ASSERT(rc==b_ok);
	rc = bdecode_get_status(decode, &status);
	BDBG_ASSERT(rc==b_ok);
	printf("prass any key to start audio");wait_key();
	rc = bdecode_attach_audio(decode, audio);
	BDBG_ASSERT(rc==b_ok);
	rc = bdecode_get_status(decode, &status);
	BDBG_ASSERT(rc==b_ok);
	printf("prass any key to stop decode");wait_key();
	bdecode_stop(decode);
	rc = bdecode_get_status(decode, &status);
	BDBG_ASSERT(rc==b_ok);


	bstream_close(stream);
	bdecode_close(decode);
	bdecode_window_close(window);
	bdisplay_close(display);
	bsettop_uninit();

	return 0;
error:
	return 1;
}

