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
 * $brcm_Workfile: decode_audio_category_code.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/decode_audio_category_code.c $
 * 
 * 5   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[]) 
{
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	bool on_the_fly = true;
	uint8_t category_code;
	bool copyright;
	int i;
	
	if (rc) goto error;

	if (argc != 3) goto error;

	bstream_mpeg_init(&mpeg);
	
	mpeg.pcr_pid = (int)strtol(argv[1], NULL, 0);
	mpeg.video[0].pid = (int)strtol(argv[1], NULL, 0);
	mpeg.audio[0].pid = (int)strtol(argv[2], NULL, 0);
	mpeg.audio[0].format = baudio_format_ac3;
	printf("pcr: %#x; video: %#x; audio: %#x\n", mpeg.pcr_pid, mpeg.video[0].pid, mpeg.audio[0].pid);

	band = bstreamer_attach( B_ID(0), bstream_mpeg_type_ts );
	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	printf("starting with default category code (should be general). press enter to change to U.S. digital broadcast receiver.\n");
	getchar();

	{	
		bdisplay_settings display_settings;
		boutput_spdif_settings spdif_settings;
		baudio_decode_t audio = NULL;

		category_code = 0x1;
		copyright = 0x0;
		
		if (!on_the_fly) audio = bdecode_detach_audio(decode);

		for (i=1; i< 8; i++){
			bdisplay_get(display, &display_settings);
			boutput_spdif_get(display_settings.spdif, &spdif_settings);
			spdif_settings.category_code = category_code*i;
			spdif_settings.copyright = i & 0x1;
			boutput_spdif_set(display_settings.spdif, &spdif_settings);
			bdisplay_set(display, &display_settings);
			printf("should be U.S. digital broadcast receiver (%02x) now. press enter to quit.\n", category_code);
			getchar();
		}
		
		if (!on_the_fly) bdecode_attach_audio(decode, audio);
				
		printf("should be U.S. digital broadcast receiver (%02x) now. press enter to quit.\n", category_code);
		getchar();
	}
	
	return 0;
error:
	return 1;
}

