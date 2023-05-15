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
 * $brcm_Workfile: encode.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/encode.c $
 * 
 * 4   9/6/12 6:03p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>

#define PLAY_COMPOSITE              1

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(bconfig->resources.linein.objects[0]);
#if PLAY_COMPOSITE
	btuner_linein_t linein = btuner_linein_open(B_ID(8));
#endif
	bencode_t encode = bencode_open(B_ID(0));
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	btuner_analog_params analog;
	bencode_settings encode_settings;
	btuner_analog_params analog_settings ;
	bstream_t analog_stream, digital_stream;
	if (rc) goto error;

	bencode_settings_init(&encode_settings, bencode_quality_best, encode);
	encode_settings.video_format = bvideo_codec_h264 ;

#if PLAY_COMPOSITE
	btuner_analog_params_init(&analog_settings, tuner) ;
	analog_stream = btuner_tune_linein(tuner, linein, &analog_settings) ;
#else
	btuner_analog_params_init(&analog, tuner);

	analog_stream = btuner_tune_rf(tuner, 63250000, &analog);
	if (!analog_stream) goto error;
#endif

	digital_stream = bencode_start(encode, analog_stream, &encode_settings);
	if (!digital_stream) goto error;

	if (bdecode_start(decode, digital_stream, window)) goto error;

	getchar(); /* press enter to stop encode and digital decode */

	bencode_stop(encode, digital_stream);
	bdecode_stop(decode);

#if !PLAY_COMPOSITE
	if (bdecode_start(decode, analog_stream, window)) goto error;

	getchar(); /* press enter to stop analog decode */
#endif

	return 0;
error:
	return 1;
}
