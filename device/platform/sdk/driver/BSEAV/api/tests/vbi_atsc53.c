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
 * $brcm_Workfile: vbi_atsc53.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/vbi_atsc53.c $
 * 
 * 2   9/6/12 6:05p erickson
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
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bstream_t stream;
	bstream_mpeg mpeg;
	bstream_vbi_settings vbisettings;
	BKNI_EventHandle event;
	bplayback_t playback = bplayback_open(B_ID(0));
	bplayback_file_t file;
	bplayback_params playback_params;
	if (rc) goto error;

	BKNI_CreateEvent(&event);

	file = bplayback_file_open("FRI0328_short.trp", NULL);
	if (!file) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0;
	mpeg.pcr_pid = 0x21;

	bplayback_params_init(&playback_params, playback);
	playback_params.index_format = bindex_format_bcm;

	stream = bplayback_start(playback, &mpeg, file, &playback_params);
	if (!stream) goto error;


	if (bdecode_start(decode, stream, window))
		goto error;

	bstream_vbi_get_settings(stream, &vbisettings);
	vbisettings.cc_data_ready_callback = vbi_callback;
	vbisettings.callback_context = event;
	bstream_vbi_set_settings(stream, &vbisettings);

	print_some_vbi_cc(stream, 2000, event);

	printf("4\n");
	bdecode_stop(decode);

	
	return 0;
error:
	return 1;
}

void print_some_vbi_cc(bstream_t stream, int total, BKNI_EventHandle event)
{
	while (total > 0) {
		int n;
		int i;

		bstream_vbi_cc cc;
		if (bstream_vbi_cc_read(stream, &cc, 1, &n)) {
			return;
		}
		if (!n) {
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}
		printf("%d: ", cc.field);
		for (i=0;i<2;i++) {
			printf("%02x ", cc.data[i]);
		}
		for (i=0;i<2;i++) {
			unsigned char ch = cc.data[i] & 0x7f;
			if (!ch) continue;
#define SAFE_CH(ch) (((ch)<0x20)?'.':(ch))
			printf("%c ", SAFE_CH(ch));
		}
		printf("\n");
		total--;
		fflush(stdout);
	}
}
