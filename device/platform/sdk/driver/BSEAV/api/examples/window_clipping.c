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
 * $brcm_Workfile: window_clipping.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/window_clipping.c $
 * 
 * 4   9/6/12 6:03p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(bconfig->resources.qam.objects[0]);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bdecode_window_settings ws;
	bvideo_format_settings fs;
	bdisplay_settings ds;
	btuner_qam_params qam;
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	if (rc) goto error;

	btuner_qam_params_init(&qam, tuner);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x31;

	printf("Decoding from a QAM demod...\n");
	band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);

	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	getchar(); /* press enter to stop decode */

	/* remove any A/R correction, which makes cliprect calcs harder. */
	bdisplay_get(display, &ds);
	ds.content_mode = bdisplay_content_mode_full;
	bdisplay_set(display, &ds);
	
	/* get the display width and height */
	bdisplay_get_video_format_settings(&ds, &fs);
	
	bdecode_window_get(window, &ws);
		
	/* zoom: cut off 75 from all sides, but keep size the same. */
	bdecode_window_get(window, &ws);
	ws.cliprect.x = 75;
	ws.cliprect.y = 75;
	ws.cliprect.width = fs.width-(75*2);
	ws.cliprect.height = fs.height-(75*2);
	bdecode_window_set(window, &ws);
	getchar();
	
	/* clip: set window size to be same as clip size. this is just clip, not zoom. */
	bdecode_window_get(window, &ws);
	ws.position.x = 75;
	ws.position.y = 75;
	ws.position.width = fs.width-(ws.position.x*2);
	ws.position.height = fs.height-(ws.position.y*2);
	/* now recalc clip relative to window. */
	ws.cliprect.x = (ws.position.x * ws.position.width / fs.width);
	ws.cliprect.y = (ws.position.y * ws.position.height / fs.height);
	ws.cliprect.width = ws.position.width - ws.cliprect.x * 2;
	ws.cliprect.height = ws.position.height - ws.cliprect.y * 2;
	bdecode_window_set(window, &ws);
	getchar();

	/* move the window around the screen. doesn't affect clip. */
	bdecode_window_get(window, &ws);
	ws.position.x += 25;
	bdecode_window_set(window, &ws);
	getchar();
	
	return 0;
error:
	return 1;
}
