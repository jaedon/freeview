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
 * $brcm_Workfile: clone.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/clone.c $
 * 
 * 4   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>

/* We will open one decode, but two displays and two windows. 
The second window will be a clone of the first. The decode is started
on the first window, but the video will appear in both windows. */
int main()
{
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(bconfig->resources.qam.objects[0]);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display0 = bdisplay_open(B_ID(0));
	bdisplay_t display1 = bdisplay_open(B_ID(1));
	bdecode_window_t window0, window1;
	btuner_qam_params qam;
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	bdisplay_settings settings;
	if (rc) goto error;

	/* create the main decode window */
	window0 = bdecode_window_open(B_ID(0), display0);
	if (!window0) goto error;

	/* create a cloned window on the 2nd display. The ID of the cloned
	window is absolute and depends on whether the platform is single or dual decode. */
#if BCHP_CHIP == 7038 || BCHP_CHIP == 7400
/* for PIP systems, there are two windows on display0, so cloned windows on display1 start at 2. */
#define CLONE_WINDOW_ID 2
#else
/* for single deocde systems, there is only one window on display0, so cloned windows on display1 start at 1. */
#define CLONE_WINDOW_ID 1
#endif

	/* when we clone, we specify the new display */
	window1 = bdecode_window_clone(window0, B_ID(CLONE_WINDOW_ID), display1);
	if (!window1) goto error;

	/* This will be HD/SD simul when we clone */
	bdisplay_get(display0, &settings);
	settings.format = bvideo_format_1080i;
	settings.composite = NULL;
	settings.svideo = NULL;
	settings.rf = NULL;
	settings.aspect_ratio = bdisplay_content_mode_box;
	bdisplay_set(display0, &settings);

	bdisplay_get(display1, &settings);
	settings.format = bvideo_format_ntsc;
	settings.composite = boutput_composite_open(B_ID(0));
	settings.svideo = boutput_svideo_open(B_ID(0));
	settings.rf = boutput_rf_open(B_ID(0));
	settings.content_mode = bdisplay_content_mode_full;
	bdisplay_set(display1, &settings);

	btuner_qam_params_init(&qam, tuner);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x21;

	band = btuner_tune_qam(tuner, 765000000, &qam);
	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	/* Decode is only started on the master window, but will appear on both
	displays because of the cloned window. */
	if (bdecode_start(decode, stream, window0))
		goto error;

	printf("Press enter to change SD aspect ratio\n");getchar();

	bdisplay_get(display1, &settings);
	settings.aspect_ratio = bdisplay_aspect_ratio_16x9;
	settings.content_mode = bdisplay_content_mode_box;
	bdisplay_set(display1, &settings);
	
	printf("Press enter to close SD\n");getchar();

	/* Closing the cloned window will stop HD/SD simul mode. */
	bdecode_window_close(window1);

	printf("Press enter to stop\n");getchar();

	return 0;
error:
	return 1;
}
