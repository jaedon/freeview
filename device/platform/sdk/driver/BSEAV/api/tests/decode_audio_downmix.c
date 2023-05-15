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
 * $brcm_Workfile: decode_audio_downmix.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/decode_audio_downmix.c $
 * 
 * 5   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>

struct {
	baudio_downmix downmix;
	const char *downmix_str;
} g_downmix[6] = {
	{baudio_downmix_stereo, "stereo"}, 
	{baudio_downmix_multichannel, "multichannel"}, 
	{baudio_downmix_none, "none"}, 
	{baudio_downmix_left, "left"}, 
	{baudio_downmix_right, "right"}, 
	{baudio_downmix_monomix, "monomix"}
};


int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	bool on_the_fly = false;
	int i;
	
	if (rc) goto error;

	
	bstream_mpeg_init(&mpeg);
	/* tested with acmod3_2_31_34.ts where left and right are the same note, but different octaves. */
	/*mpeg.video[0].pid = 0x31;*/
	mpeg.audio[0].pid = 0x1101;
	mpeg.audio[0].format = baudio_format_mpeg;
	mpeg.pcr_pid = 0x1001;

	band = bstreamer_attach( B_ID(0), bstream_mpeg_type_ts );

	if (!band) goto error;
	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;
	if (bdecode_start(decode, stream, window))
		goto error;

	printf("starting in default mode (should be stereo). press enter to change mode.\n");
	getchar();
	
	for (i=0;i<6;i++) {
		bdecode_settings settings;
		baudio_decode_t audio = NULL;

		if (!on_the_fly) audio = bdecode_detach_audio(decode);
		
		bdecode_get(decode, &settings);
		settings.audio.downmix = g_downmix[i].downmix;
		bdecode_set(decode, &settings);

		if (!on_the_fly) bdecode_attach_audio(decode, audio);
				
		printf("now in %s mode. press enter to change mode.\n", g_downmix[i].downmix_str);
		getchar();
	}
	
	return 0;
error:
	return 1;
}
