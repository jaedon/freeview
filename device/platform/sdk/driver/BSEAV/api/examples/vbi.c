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
 * $brcm_Workfile: vbi.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/vbi.c $
 * 
 * 4   9/6/12 6:03p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

void print_some_vbi_cc(bstream_t stream, int total, BKNI_EventHandle event);

void vbi_callback(void *context)
{
	BKNI_SetEvent((BKNI_EventHandle)context);
}

int main() {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(bconfig->resources.qam.objects[0]);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	btuner_qam_params qam;
	btuner_analog_params analog;
	bband_t band;
	bstream_t stream;
	bstream_mpeg mpeg;
	bstream_vbi_settings vbisettings;
	bdecode_window_settings windowsettings;
	BKNI_EventHandle event;
	if (rc) goto error;

	BKNI_CreateEvent(&event);

	/* by resizing we eliminate the possibility of passthrough. This way we know that
	we're really decoding and reencoding if we see anything out of the VEC. */
	bdecode_window_get(window, &windowsettings);
	windowsettings.position.x = 100;
	windowsettings.position.y = 100;
	windowsettings.position.width = 320;
	windowsettings.position.height = 240;
	windowsettings.cliprect.width = 0;
	bdecode_window_set(window, &windowsettings);

	btuner_analog_params_init(&analog, tuner);
	btuner_qam_params_init(&qam, tuner);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	/***************************
	* analog close captioning
	**/
	stream = btuner_tune_rf(tuner, 411250000, &analog);
	if (stream) {
		if (bdecode_start(decode, stream, window))
			goto error;

		/* You can start vbi for analog before starting decode. */
		bstream_vbi_get_settings(stream, &vbisettings);
		vbisettings.cc.display = display;
		vbisettings.cc_data_ready_callback = vbi_callback;
		vbisettings.callback_context = event;
		bstream_vbi_set_settings(stream, &vbisettings);

		print_some_vbi_cc(stream, 200, event);

		bdecode_stop(decode);
	}

	/***************************
	* digital close captioning
	**/
	band = btuner_tune_qam(tuner, 765000000, &qam);
	if (!band) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x21;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	/* You should start vbi for digital streams after bdecode_start. */
	bstream_vbi_get_settings(stream, &vbisettings);
	vbisettings.cc.display = display;
	bstream_vbi_set_settings(stream, &vbisettings);

	print_some_vbi_cc(stream, 200, event);

	return 0;
error:
	return 1;
}

void print_some_vbi_cc(bstream_t stream, int total, BKNI_EventHandle event)
{
	while (total > 0) {
		int i;
		size_t n;
		bstream_vbi_cc cc;
		if (bstream_vbi_cc_read(stream, &cc, 1, &n))
			return;
		if (!n) {
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}

		for (i=0;i<2;i++) {
			unsigned char ch = cc.data[i] & 0x7f;
			if (!ch) continue;
#define SAFE_CH(ch) (((ch)<0x20)?'.':(ch))
			printf("%c", SAFE_CH(ch));
			fflush(stdout);
			total--;
		}
	}
}
