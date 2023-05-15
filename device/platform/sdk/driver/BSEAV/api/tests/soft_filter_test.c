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
 * $brcm_Workfile: soft_filter_test.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/soft_filter_test.c $
 * 
 * 3   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

void message_callback(void *context)
{
	BKNI_SetEvent((BKNI_EventHandle)context);
}

void print_message(unsigned char *buffer, int size)
{
	int i;
	for (i=0;i<size;i++)
		printf("0x%02x,", buffer[i]);
	printf("\n");
}

unsigned char pat[] = {
0x00,0xb0,0xe9,0x03,0xec,0xed,0x00,0x00,0x00,0x01,0xe0,0x6a,0x00,0x02,0xe0,0x6b,
0x00,0x03,0xe0,0xaa,0x00,0x04,0xe0,0xab,0x00,0x05,0xe1,0xaa,0x00,0x06,0xe1,0xab,
0x00,0x07,0xe1,0xac,0x00,0x08,0xe1,0xea,0x00,0x09,0xe1,0xeb,0x00,0x0a,0xe1,0xec,
0x00,0x0b,0xe2,0x2a,0x00,0x0c,0xe2,0x2b,0x00,0x0d,0xe2,0x2c,0x00,0x0e,0xe2,0x6a,
0x00,0x0f,0xe2,0x6b,0x00,0x10,0xe2,0x6c,0x00,0x11,0xe2,0xaa,0x00,0x12,0xe2,0xab,
0x00,0x13,0xe2,0xac,0x00,0x14,0xe3,0x2a,0x00,0x15,0xe3,0x2b,0x00,0x16,0xe3,0x2c,
0x00,0x17,0xe3,0x6a,0x00,0x18,0xe3,0x6b,0x00,0x19,0xe3,0x6c,0x00,0x1a,0xe4,0x2a,
0x00,0x1b,0xe4,0x2b,0x00,0x1c,0xe4,0x2c,0x00,0x1d,0xe4,0x6a,0x00,0x1e,0xe4,0x6b,
0x00,0x1f,0xe4,0x6c,0x00,0x20,0xe4,0xaa,0x00,0x21,0xe4,0xab,0x00,0x22,0xe4,0xac,
0x00,0x23,0xe4,0xea,0x00,0x24,0xe4,0xeb,0x00,0x25,0xe4,0xec,0x00,0x26,0xe5,0x2a,
0x00,0x27,0xe5,0x2b,0x00,0x28,0xe5,0x2c,0x00,0x29,0xe5,0x6a,0x00,0x2a,0xe5,0x6b,
0x00,0x2b,0xe5,0x6c,0x00,0x2c,0xe5,0xaa,0x00,0x2d,0xe5,0xab,0x00,0x2e,0xe5,0xac,
0x00,0x2f,0xe3,0xaa,0x00,0x30,0xe3,0xab,0x00,0x31,0xe3,0xac,0x00,0x32,0xe0,0x2a,
0x00,0x33,0xe0,0x2b,0x00,0x34,0xe0,0x2c,0x00,0x35,0xe0,0x6c,0x00,0x36,0xe0,0xac,
0x00,0x3d,0xe0,0xad,0x00,0x00,0xef,0xfe,0x30,0x7b,0x39,0x82};

int check_contents(unsigned char *buffer, int size)
{
	int i=0;
	while (size >= sizeof(pat)) {
		for (i=0;i<sizeof(pat);i++)
			if (buffer[i] != pat[i])
				goto error;
		size -= sizeof(pat);
	}
	if (size)
		goto error;
	return 0;
error:
	print_message(buffer, size);
	print_message(pat, sizeof(pat));
	return -1;
}

int main() {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(B_ID(0));
	btuner_qam_params qam;
	bband_t band;
	bmessage_stream_t msg;
	bmessage_stream_params params;
	const void *buffer;
	size_t size;
	BKNI_EventHandle event;
    int success;
    
    if (rc) goto error;

	BKNI_CreateEvent(&event);

	msg = bmessage_open(bmessage_format_ts);
	if (!msg) goto error;

	btuner_qam_params_init(&qam, tuner);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	/* This is a cox feed */
	band = btuner_tune_qam(tuner, 669000000, &qam);
	if (!band) goto error;

	bmessage_stream_params_init(&params, msg);
	params.band = band;
	params.pid = 0x0;
	
	/* must set up a filter */
	params.filter.coef[0] = 0x00;
	params.filter.coef[3] = 0x03;
	params.filter.coef[4] = 0xec;
	params.filter.mask[0] = 0x00;
	params.filter.mask[3] = 0x00;
	params.filter.mask[4] = 0x00;

	params.data_ready_callback = message_callback;
	params.callback_context = event;
	
	success = 0;

	while (1) {
		int k;
		if (bmessage_start(msg, &params)) goto error;
		for (k=0;k<4;) {
			/* The first read after the first start will be ok.
			The first read after the second and subsequent starts will return
			garbage data. The Settop API will screen out this data. */
			if (bmessage_get_buffer(msg, &buffer, &size)) goto error;
			if (!size) {
				BKNI_WaitForEvent(event, BKNI_INFINITE);
				continue;
			}
#if 0
			unsigned i;
			printf("%d: Read %d bytes of PAT on pid 0\n", k, size);
#endif

			if (check_contents(buffer, size)) {
				printf("softfilter test failed\n");
				goto error;
			}
			else {
				printf("%d good\n", ++success);
			}

			/* All consumed */
			bmessage_read_complete(msg, size);
			k++;
		}
		bmessage_stop(msg);
	}

	return 0;
error:
	printf("error\n");
	return 1;
}
