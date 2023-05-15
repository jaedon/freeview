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
 * $brcm_Workfile: playback_pid_parser.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/playback_pid_parser.c $
 * 
 * 3   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

/* This example routes a playback through a parser band in order
to perform PSI scan. */

void message_callback(void *context)
{
	BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(int argc, char **argv) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bplaypump_t playpump = bplaypump_open(B_ID(0), NULL);
	bplayback_t playback = bplayback_open();
	bplayback_file_t file;
	bstream_mpeg mpeg;
	bstream_t stream;
	bplayback_params playback_params;
	bstream_status stream_status;
	bmessage_stream_t msg;
	bmessage_stream_params params;
	const void *buffer;
	size_t size;
	BKNI_EventHandle event;
	const char *stream_name = "stream.mpg";
	if (rc) goto error;
	
	BKNI_CreateEvent(&event);
	
	/* allow for user specified-stream */
	if (argc > 1) stream_name = argv[1];

	file = bplayback_file_open(stream_name, NULL);
	if (!file) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x21;

	bplayback_params_init(&playback_params, playback);
	playback_params.route_through_parser_band = true; /* this is optional for 740x platforms */
	playback_params.index_format = bindex_format_bcm;

	stream = bplayback_start(playback, playpump, &mpeg, file, &playback_params);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;
		
	bstream_get_mpeg_parameters(stream, &stream_status);
	
	msg = bmessage_open(bmessage_format_psi);
	if (!msg) goto error;

	bmessage_stream_params_init(&params, msg);
	params.stream = stream;
	params.pid = 0x0;
	/* use the default filter */
	params.data_ready_callback = message_callback;
	params.callback_context = event;

	if (bmessage_start(msg, &params)) goto error;

	/* This example only needs to do one read */
	while (1) {
		if (bmessage_get_buffer(msg, &buffer, &size)) goto error;
		printf("read %p %d\n", buffer, size);
		if (!size) {
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}
		else
			break;
	}

	printf("Read %d bytes of PAT on pid 0\n", size);

	/* All consumed */
	bmessage_read_complete(msg, size);

	bmessage_stop(msg);
	bdecode_stop(decode);
	bplayback_stop(playback);
	bsettop_uninit();
	return 0;
error:
	return 1;
}
