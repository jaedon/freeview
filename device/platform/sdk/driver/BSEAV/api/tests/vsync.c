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
 * $brcm_Workfile: vsync.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/vsync.c $
 * 
 * 2   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <stdlib.h>


static void 
resize(bdecode_window_t window)
{
	bdecode_window_settings win_cfg;

	printf("press enter to resize a window"); getchar();
	if (bdecode_window_get(window, &win_cfg)) { exit(0); }
	win_cfg.position.width /= 2;
	win_cfg.position.height /= 2;
	if (bdecode_window_set(window, &win_cfg)) { exit(0); }
	printf("press enter to restore size of a window"); getchar();
	win_cfg.position.width *= 2;
	win_cfg.position.height *= 2;
	if (bdecode_window_set(window, &win_cfg)) { exit(0); }
	return;
}

int main(int argc, const char *argv[]) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bplaypump_t playpump = bplaypump_open(B_ID(0), NULL);
	bplayback_t playback = bplayback_open();
	bstream_t stream;
	bstream_mpeg mpeg;
	bplayback_file_t file;
	bplayback_params playback_params;
	bdisplay_settings disp_cfg;


	if (rc) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x22;
	mpeg.audio[0].format = baudio_format_mpeg;
	if (argc==5) {
		mpeg.video[0].pid = strtol(argv[2], NULL, 0);
		mpeg.audio[0].pid =  strtol(argv[3], NULL, 0);
		mpeg.audio[0].format = strtol(argv[4], NULL, 0);
		printf("using user supplied file %s: video %#x audio %#x:%#x\n", argv[1], mpeg.video[0].pid, mpeg.audio[0].pid, mpeg.audio[0].format);
		file=bplayback_file_open(argv[1], NULL);
	} else {
		file=bplayback_file_open("vsync.mpg", NULL);
	}
	if (!file) {
		printf("can't open file\n");
		goto error;
	}
	mpeg.pcr_pid = mpeg.video[0].pid;

	bplayback_params_init(&playback_params, playback);
	stream = bplayback_start(playback, playpump, &mpeg, file, &playback_params);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	for(;;) {
		resize(window);
		bdisplay_get(display, &disp_cfg);
		printf("Press any key to change format to 480p");getchar();
		disp_cfg.format = bvideo_format_480p;
		disp_cfg.composite = NULL;
		disp_cfg.svideo = NULL;
		disp_cfg.rf = NULL;
		bdisplay_set(display, &disp_cfg);
		resize(window);
		printf("Press any key to change format to 720p");getchar();
		disp_cfg.format = bvideo_format_720p;
		disp_cfg.composite = NULL;
		disp_cfg.svideo = NULL;
		disp_cfg.rf = NULL;
		bdisplay_set(display, &disp_cfg);
		resize(window);
		printf("Press any key to change format to 1080i");getchar();
		disp_cfg.format = bvideo_format_1080i;
		disp_cfg.composite = NULL;
		disp_cfg.svideo = NULL;
		disp_cfg.rf = NULL;
		bdisplay_set(display, &disp_cfg);
		resize(window);
		printf("Press any key to change format to 480i");getchar();
		bdisplay_get(display, &disp_cfg);
		disp_cfg.format = bvideo_format_ntsc;
		bdisplay_set(display, &disp_cfg);
	}

	bsettop_uninit();
	return 0;
error:
	return 1;
}
