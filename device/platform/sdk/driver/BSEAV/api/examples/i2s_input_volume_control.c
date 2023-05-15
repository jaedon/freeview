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
 * $brcm_Workfile: i2s_input_volume_control.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/i2s_input_volume_control.c $
 * 
 * 2   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <assert.h>

/**
This example shows how to "tune" a 656 input. This requires external hardware which 
is board/customer specific. So this serves as sample code only.
The result is an analog stream which can be decoded or encoded. 
See comments within the example for specifics.
**/

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(B_ID(7)); /* This number depends on the platform,
		but will always be different than the linein or RF-based tuner.
		This number is valid for 7038 based platforms.
		On some platforms, the 656 input hardware might be shared with other analog
		paths, and in this case use of the different tunes is mutually exclusive.
		On a 7038, when using tuner0 for analog, tuner7 is not available. */
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bstream_t stream;
	btuner_analog_params analog_params;
	baudio_volume vol;
	int i=0;
	
	if (rc) goto error;

	assert(tuner);
	
	btuner_analog_params_init(&analog_params, tuner);
	analog_params.video_format = bvideo_format_ntsc; /* you can also select PAL */

	/* Instead of adding another tune function, we reuse the tune_rf function
	and ignore the frequency parameter. */	
	stream = btuner_tune_rf(tuner, 0, &analog_params);
	if (!stream) goto error;

	/* You are now decoding from the 656 input. */
	if (bdecode_start(decode, stream, window))
		goto error;

	vol.muted = false;
	
	printf("\nThe min volume setting is 0 and the max volume setting is 100\n");
	printf("\nTo increase the volume in steps of 10 press enter\n\n");
	getchar();
	
	for(i=0; i< 11; i++){
		vol.right = vol.left = i * 10;
		bdecode_set_audio_volume(decode, &vol);
		printf("\nThe current volume setting is = %d\n",vol.right);
		getchar();
	}

	getchar(); /* press enter to stop decode */
	
	return 0;
error:
	return 1;
}
