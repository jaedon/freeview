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
 * $brcm_Workfile: hdsd_single_interface.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/hdsd_single_interface.c $
 * 
 * 4   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>

/* This example shows how to drive both SD and HD outputs with the
same video and graphics content.
Key points are:
1) two cloned windows allow dual decode to go to four decode windows
2) dual graphics with indentical content
3) independent control of scaling and position. the programmer must make them look
the same.
4) This example only runs on 7038 and family.
*/
int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner0 = btuner_open(bconfig->resources.qam.objects[0]);
	btuner_t tuner1 = btuner_open(bconfig->resources.qam.objects[1]);
	bdecode_t decode0 = bdecode_open(B_ID(0));
	bdecode_t decode1 = bdecode_open(B_ID(1));
	bdisplay_t display0 = bdisplay_open(B_ID(0));
	bdisplay_t display1 = bdisplay_open(B_ID(1));

	/* two graphics are totally independent. user makes sure content is the same */
	bgraphics_t graphics0 = bgraphics_open(B_ID(0), display0);
	bgraphics_t graphics1 = bgraphics_open(B_ID(1), display1);

	/* here's the trick. use clone to create the decode windows on the 2nd display */
	bdecode_window_t window0 = bdecode_window_open(B_ID(0), display0);
	bdecode_window_t window1 = bdecode_window_open(B_ID(1), display0);
	/* unused: bdecode_window_t window2 = bdecode_window_clone(window0, B_ID(2), display1); */
	bdecode_window_t window3 = bdecode_window_clone(window1, B_ID(3), display1);
	btuner_qam_params qam;
	bstream_t stream0, stream1;
	bstream_mpeg mpeg;
	bband_t band;
	bdecode_window_settings dw_settings;
	bdisplay_settings display_settings;
	bsurface_t fb0, fb1;
	bsettop_rect rect = {100,100,100,100};
	bsurface_create_settings surface_create_settings;

	if (rc) goto error;

	/* This is a main/pip application, so throw away the audio on 2nd decode */
	bdecode_detach_audio(decode1);

	/* Switch display to HD and SD */
	bdisplay_get(display0, &display_settings);
	display_settings.format = bvideo_format_1080i;
	display_settings.svideo = NULL;
	display_settings.composite = NULL;
	display_settings.rf = NULL;
	bdisplay_set(display0, &display_settings);
	bdisplay_get(display1, &display_settings);
	display_settings.format = bvideo_format_ntsc;
	display_settings.svideo = boutput_svideo_open(B_ID(0));
	display_settings.composite = boutput_composite_open(B_ID(0));
	display_settings.rf = boutput_rf_open(B_ID(0));
	bdisplay_set(display1, &display_settings);

	/* Create two framebuffers and a single surface to blit into */
	bsurface_create_settings_init(&surface_create_settings, graphics0);
	surface_create_settings.width = 1920;
	surface_create_settings.width = 1080;
	fb0 = bgraphics_create_framebuffer(graphics0, &surface_create_settings, false);
	surface_create_settings.width = 720;
	surface_create_settings.width = 480;
	fb1 = bgraphics_create_framebuffer(graphics1, &surface_create_settings, false);

	/* draw the smaller one first, then scale up */
	rect.x = rect.y = 50;
	rect.width = rect.height = 150;
	bsurface_fill(fb1, &rect, 0xFF777777);

	rect.x = rect.x*1920/720;
	rect.width = rect.width*1920/720;
	rect.y = rect.y*1080/480;
	rect.height = rect.height*1080/480;
	bsurface_fill(fb0, &rect, 0xFF777777);

	/* reposition pip's */
	bdecode_window_get(window1, &dw_settings);
	dw_settings.position.x = 1920/2;
	dw_settings.position.y = 0;
	dw_settings.position.width = 1920/2;
	dw_settings.position.height = 1080/2;
	bdecode_window_set(window1, &dw_settings);
	bdecode_window_get(window3, &dw_settings);
	dw_settings.position.x = 720/2;
	dw_settings.position.y = 0;
	dw_settings.position.width = 720/2;
	dw_settings.position.height = 480/2;
	bdecode_window_set(window3, &dw_settings);

	/* tune and start the first decode */
	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x11;
	mpeg.audio[0].pid = 0x14;
	mpeg.audio[0].format = baudio_format_mpeg;
	mpeg.pcr_pid = 0x11;
	btuner_qam_params_init(&qam, tuner0);
	band = btuner_tune_qam(tuner0, 765000000, &qam);
	if (!band) goto error;
	stream0 = bstream_open(band, &mpeg);
	if (!stream0) goto error;
	if (bdecode_start(decode0, stream0, window0))
		goto error;

	/* tune and start the second decode */
	mpeg.video[0].pid = 0x31;
	mpeg.audio[0].pid = 0x34;
	mpeg.pcr_pid = 0x31;
	band = btuner_tune_qam(tuner1, 765000000, &qam);
	if (!band) goto error;
	stream1 = bstream_open(band, &mpeg);
	if (!stream1) goto error;
	if (bdecode_start(decode1, stream1, window1))
		goto error;

	getchar(); /* press enter to stop both decodes */

	return 0;
error:
	return 1;
}
