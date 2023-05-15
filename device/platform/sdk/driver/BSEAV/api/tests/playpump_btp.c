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
 * $brcm_Workfile: playpump_btp.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/playpump_btp.c $
 * 
 * 5   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

void play_callback(void *context)
{
	BKNI_SetEvent((BKNI_EventHandle)context);
}

void print_usage() { 
	printf(
	"Usage: playpump_btp [-host] [-rev] [-decoder X] TRICKMODEFILE VIDPID\n"
	"  -host is for a host trick mode (default is a broadcom trick mode)\n"
	"  -dqt is for a DQT trick mode\n"
	"  -decoder X applies a decoder slow motion of X (default 1)\n"
	"  -rev sets field reverse to true (default to forward)\n"
	);
}

int main(int argc, char **argv) {
	bresult rc;
	bdecode_t decode;
	bdisplay_t display;
	bdecode_window_t window;
	bplaypump_params playpump_params;
	FILE *file;
	bplaypump_t play;
	bstream_mpeg mpeg;
	bstream_t stream;
	BKNI_EventHandle event;
	unsigned short vidpid;
	bplaypump_settings ps;
	int curarg = 1;
	int decoder_slow = 1;
	bool is_rev = false;
	bool is_dqt = false;
	bool is_bcm = true;

	/* parse params before starting settop api */
	while (curarg < argc) {
		if (!strcmp("-host", argv[curarg])) {
			is_bcm = false;
		}
		else if (!strcmp("-dqt", argv[curarg])) {
			is_dqt = true;
		}
		else if (!strcmp("-rev", argv[curarg])) {
			is_rev = true;
		}
		else if (!strcmp("-decoder", argv[curarg]) && curarg+1 < argc) {
			decoder_slow = atoi(argv[++curarg]);
		}
		else {
			/* the remainder must be TRICKMODEFILE and VIDPID */
			break;
		}
		curarg++;
	}
	
	if (curarg + 2 > argc) {
		print_usage();
		exit(1);
	}
	
	rc = bsettop_init(BSETTOP_VERSION);
	if (rc) goto error;
	BKNI_CreateEvent(&event);
	decode = bdecode_open(B_ID(0));
	display = bdisplay_open(B_ID(0));
	window = bdecode_window_open(B_ID(0), display);
	play = bplaypump_open(B_ID(0), NULL);
	file = fopen(argv[curarg++], "r");
	if (!file) goto error;
	
	vidpid = strtoul(argv[curarg++], NULL, 0);

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = vidpid;
	mpeg.pcr_pid = vidpid;

	bplaypump_params_init(&playpump_params, play);
	playpump_params.read_callback = play_callback;
	playpump_params.callback_context = event;

	stream = bplaypump_start(play, &mpeg, &playpump_params);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	bplaypump_get(play, &ps);
	if (is_dqt)
		ps.state = bplaypump_state_display_queue_trick_mode;
	else if (is_bcm)
		ps.state = bplaypump_state_brcm_trick_mode;
	else
		ps.state = bplaypump_state_host_trick_mode;
	ps.forward = !is_rev;
	ps.decode_rate = 100/decoder_slow;
	bplaypump_set(play, &ps);
	
	while (1) {
		void *buffer;
		size_t buffer_size;
		int n;

		if (bplaypump_get_buffer(play, &buffer, &buffer_size))
			break;
		if (buffer_size == 0) {
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}

		/* The first call to get_buffer will return the entire playback buffer.
		If we use it, we're going to have to wait until the descriptor is complete,
		and then we're going to underflow. So use a max size. */
#define MAX_READ (188*1024)
		if (buffer_size > MAX_READ)
			buffer_size = MAX_READ;

		n = fread(buffer, 1, buffer_size, file);
		if (n < 0) goto error;
		if (n == 0) {
			BKNI_Sleep(1000); /* This is a simplistic way of making sure the MVD buffer
				is empty before flushing. */
			fseek(file, 0, SEEK_SET);
			bplaypump_flush(play);
		}
		else {
			bplaypump_read_complete(play, 0, n);
		}
	}
	/* loops forever */

error:
	return 1;
}
