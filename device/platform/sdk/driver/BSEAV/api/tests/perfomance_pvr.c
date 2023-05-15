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
 * $brcm_Workfile: perfomance_pvr.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/perfomance_pvr.c $
 * 
 * 2   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>


/*
* This test is used to test perfomance of the PVR record and playback operations
*/
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
	brecpump_t recpump0 = brecpump_open(B_ID(0), NULL);
	brecpump_t recpump1 = brecpump_open(B_ID(1), NULL);
	brecpump_t recpump2 = brecpump_open(B_ID(2), NULL);
	brecord_t record0 = brecord_open();
	brecord_t record1 = brecord_open();
	brecord_t record2 = brecord_open();
	bplaypump_t playpump0 = bplaypump_open(B_ID(0), NULL);
	bplaypump_t playpump1 = bplaypump_open(B_ID(1), NULL);
	bplayback_t playback0 = bplayback_open();
	bplayback_t playback1 = bplayback_open();
	btuner_qam_params qam;
	bstream_t stream0, stream1;
    bstream_t streamp0, streamp1;
	bstream_mpeg mpeg0, mpeg1, mpeg2;
	bband_t band0, band1;
	brecord_file_t fileR0=brecord_file_open("media/stream0.mpg", NULL);
	brecord_file_t fileR1=brecord_file_open("media/stream1.mpg", NULL);
	brecord_file_t fileR2=brecord_file_open("media/stream2.mpg", NULL);
	bplayback_file_t fileP0=bplayback_file_open("media/stream0.mpg", NULL);
	bplayback_file_t fileP1=bplayback_file_open("media/stream1.mpg", NULL);
	brecord_params rec_cfg;
	bplayback_params play_cfg;
	btuner_t tuner2;
	bstream_t stream2;
	bband_t band2;
	btuner_sds_params sds;
	
	if (rc) goto error;

	btuner_qam_params_init(&qam, tuner0);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	bstream_mpeg_init(&mpeg0);
	mpeg0.video[0].pid = 0;
	mpeg0.audio[0].pid = 0x1FFF;

	band0 = btuner_tune_qam(tuner0, 765000000, &qam);
	if (!band0) goto error;

	stream0 = bstream_open(band0, &mpeg0);
	if (!stream0) goto error;

	brecord_params_init(&rec_cfg, record0);
	if (brecord_start(record0, recpump0, stream0, fileR0, &rec_cfg))
		goto error;

	bstream_mpeg_init(&mpeg1);
	mpeg1.video[0].pid = 0;
	mpeg1.audio[0].pid = 0x1FFF;

	band1 = btuner_tune_qam(tuner1, 765000000, &qam);
	if (!band1) goto error;

	stream1 = bstream_open(band1, &mpeg1);
	if (!stream1) goto error;

	if (brecord_start(record1, recpump1, stream1, fileR1, &rec_cfg))
		goto error;

	printf("Dual record has started, press enter to start dual playback..."); getchar(); 

	bplayback_params_init(&play_cfg, playback0);

	bstream_mpeg_init(&mpeg0);
	mpeg0.video[0].pid = 0x21;
	mpeg0.audio[0].pid = 0x24;
	mpeg0.audio[0].format = baudio_format_ac3;
	mpeg0.pcr_pid = 0x21;
	streamp0 = bplayback_start(playback0, playpump0, &mpeg0, fileP0, &play_cfg);
	if (!streamp0) goto error;

	if (bdecode_start(decode0, streamp0, window0))
		goto error;


	bstream_mpeg_init(&mpeg1);
	mpeg1.video[0].pid = 0x11;
	mpeg1.audio[0].pid = 0x14;
	mpeg1.audio[0].format = baudio_format_ac3;
	mpeg1.pcr_pid = 0x11;
	streamp1 = bplayback_start(playback1, playpump1, &mpeg1, fileP1, &play_cfg);
	if (!streamp1) goto error;

	if (bdecode_start(decode1, streamp1, window1))
		goto error;

	printf("Press any key to start 3-rd record(SDS) ..."); getchar(); 


	tuner2 = btuner_open(B_ID(2)); /* first SDS tuner on for97038 board */
	if (!tuner2) 
		goto error;


	btuner_sds_params_init(&sds, tuner2);
	sds.symbol_rate = 20000000;
	sds.mode = btuner_sds_mode_qpsk_dvb;
	bstream_mpeg_init(&mpeg2);
	mpeg2.video[0].pid = 0;
	mpeg2.audio[0].pid = 0x1FFF;

	band2 = btuner_tune_sds(tuner2, 1200000000, &sds);
	if (!band2) goto error;


	stream2 = bstream_open(band2, &mpeg2);
	if (!stream2) goto error;

	if (brecord_start(record2, recpump2, stream2, fileR2, &rec_cfg))
		goto error;

	printf("Press any key to continue..."); getchar(); 

	bsettop_uninit();
	return 0;
error:
	return 1;
}
