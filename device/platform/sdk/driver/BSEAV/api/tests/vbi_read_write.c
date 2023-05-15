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
 * $brcm_Workfile: vbi_read_write.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/vbi_read_write.c $
 * 
 * 3   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <stdio.h>

void print_cc(const bstream_vbi_cc *cc)
{
	int i;
	for (i=0;i<2;i++) {
		unsigned char ch = cc->data[i] & 0x7f;
		if (!ch) continue;
#define SAFE_CH(ch) (((ch)<0x20)?'.':(ch))
		printf("%c", SAFE_CH(ch));
		fflush(stdout);
	}
}

void vbi_callback(void *context)
{
	BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	btuner_analog_params analog;
	bstream_t stream;
	bstream_vbi_settings vbisettings;
	BKNI_EventHandle event;
	if (rc) goto error;

	BKNI_CreateEvent(&event);
	
	btuner_t tuner = btuner_open(B_ID(0));
	bdecode_t decode = bdecode_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);

	btuner_analog_params_init(&analog, tuner);
	stream = btuner_tune_rf(tuner, 411250000, &analog);
	bdecode_start(decode, stream, window);

	bstream_vbi_get_settings(stream, &vbisettings);
	vbisettings.cc.display = display;
	vbisettings.cc_data_ready_callback = vbi_callback;
	vbisettings.callback_context = event;
	bstream_vbi_set_settings(stream, &vbisettings);

#define DELAY_SIZE 1000
	bstream_vbi_cc delay_queue[DELAY_SIZE];
	int i=0;
	
	for (i=0;i<DELAY_SIZE;) {
		int n;
		if (bstream_vbi_cc_read(stream, &delay_queue[i], 1, &n))
			goto error;
		if (!n) {
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}
		print_cc(&delay_queue[i++]);
		if (i % 30 == 0)
			printf("in %d\n", i);
	}

	/* turn off vbi out */	
	bstream_vbi_get_settings(stream, &vbisettings);
	vbisettings.cc.display = NULL;
	bstream_vbi_set_settings(stream, &vbisettings);
	
	/* and play it all back out again */
	for (i=0;i<DELAY_SIZE;/*inc i inside*/) {
		int written, temp;
		/* try to write as much as possble, internal flow control will take over */
		if (bdisplay_vbi_cc_write(display, &delay_queue[i], DELAY_SIZE-i, &written))
			goto error;

		/* print out what was written & increment i */			
		temp = written;
		while (temp--) {
			print_cc(&delay_queue[i++]);
		}

		/* there is no event to say when space is available, so we don't
		want a busy loop. this is the minimum to yield to the scheduler. */		
		if (written == 0)
			BKNI_Sleep(1);
	}
	
	return 0;
error:
	return 1;
}

