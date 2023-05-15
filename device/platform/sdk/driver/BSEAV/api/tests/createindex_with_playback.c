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
 * $brcm_Workfile: createindex_with_playback.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/createindex_with_playback.c $
 * 
 * 2   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <sys/time.h>

/**
This app creates an index using HW playback, SCD and record.
It should operate at maximum XPT HW bitrate (100 Mbps for 740x.)

TODO: make into a general util with cmdline params.
**/

typedef struct timeval b_time_t;

bool finished;

static bplayback_loopmode 
bplay_pause(void *context)
{
	finished= true;
	return bplayback_loopmode_pause; 
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bplaypump_t playpump = bplaypump_open(B_ID(0), NULL);
	bplayback_t playback = bplayback_open();
	bplayback_file_t file;
	brecpump_t recpump = brecpump_open(B_ID(0), NULL);
	brecord_t record = brecord_open();
	brecord_file_t recfile;
	brecord_params recparams;
	bstream_mpeg mpeg;
	bstream_t stream;
	bplayback_params playback_params;
	bplayback_status status;
	double mpegfilesize = 0;
	b_time_t	start_time;
	b_time_t	stop_time;
	float speed;


	b_time_get(&start_time);
	printf("Start time (absolute) is %ld seconds\n", start_time.tv_sec);

	file = bplayback_file_open("videos/macroblock.ts",NULL);
	if (!file) goto error;

	recfile = brecord_file_open(NULL,"videos/stream.nav");

	bstream_mpeg_init(&mpeg);
#if 0
	mpeg.video[0].pid = 0x14;
	mpeg.video[0].format = 0x1b;
#else

	mpeg.video[0].pid = 0x1a;
	mpeg.video[0].format = 0x1b;
	mpeg.audio[0].pid = 0x1b;
	mpeg.audio[0].format = 0x3;
	mpeg.pcr_pid = 0x1a;

#endif

	bplayback_params_init(&playback_params, playback);
	playback_params.end_of_stream = bplay_pause;
	playback_params.index_format = bindex_format_none;

	stream = bplayback_start(playback, playpump, &mpeg, file, &playback_params);
	if (!stream) goto error;

	brecord_params_init(&recparams, record);
	recparams.index_format = bindex_format_bcm;

	if (brecord_start(record, recpump, stream, recfile, &recparams))
		goto error;

	while(!finished)
		BKNI_Sleep(1000);

	bplayback_get_status(playback, &status);
	mpegfilesize = status.mpeg_file_size /(1024.0*1024.0);
	
	brecord_stop(record);
	brecord_file_close(recfile);
	brecord_close(record);

	bplayback_stop(playback);
	bplayback_file_close(file);
	bplayback_close(playback);
	
	b_time_get(&stop_time);
	speed = mpegfilesize*8/(stop_time.tv_sec - start_time.tv_sec);
	printf("Stop time (absolute) is %ld seconds\n", stop_time.tv_sec);
	printf("Mpeg file size %f MB, Time spent %ld s, Speed %f Mbps\n",mpegfilesize, stop_time.tv_sec - start_time.tv_sec, speed);

	return 0;
error:
	return 1;
}



