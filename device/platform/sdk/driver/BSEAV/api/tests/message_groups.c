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
 * $brcm_Workfile: message_groups.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/message_groups.c $
 * 
 * 4   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

void message_callback(void *context)
{
	BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(int argc, char **argv) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bband_t band;
	bmessage_stream_params params;
#define TOTAL 3
	bmessage_stream_t msg[TOTAL];
	const void *buffer[TOTAL];
	size_t size[TOTAL];
	BKNI_EventHandle event;
	int i,j;
	if (rc) goto error;

	if (argc < 2) {
		printf("Usage: message_groups PID\n");
		exit(1);
	}
	
	BKNI_CreateEvent(&event);

#if 0
	band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
#else
	{
		btuner_qam_params qam;
		btuner_t tuner = btuner_open(B_ID(0));
		btuner_qam_params_init(&qam, tuner);
		band = btuner_tune_qam(tuner, 765000000, &qam);
	}
#endif
	if (!band) goto error;

	for (i=0;i<TOTAL;i++) {
		msg[i] = bmessage_open(bmessage_format_psi);
		if (!msg[i]) goto error;
	}

	bmessage_stream_params_init(&params, msg[0]);
	params.band = band;
	params.pid = strtoul(argv[1], NULL, 0);
	params.filter_group = true;
	params.data_ready_callback = message_callback;
	params.callback_context = event;

	params.filter.mask[0] = 0;
	params.filter.coef[0] = 0x40;
	if (bmessage_start(msg[0], &params)) goto error;
	
	params.filter.mask[0] = 0;
	params.filter.coef[0] = 0x41;
	if (bmessage_start(msg[1], &params)) goto error;
	
	params.filter.mask[0] = 0;
	params.filter.coef[0] = 0x42;
	if (bmessage_start(msg[2], &params)) goto error;

	for (j=0;j<1000;) {
		int i;
		bool something = false;
		for (i=0;i<TOTAL;i++)
			size[i] = 0;
			
		for (i=0;i<TOTAL;i++) {
			if (bmessage_get_buffer(msg[i], &buffer[i], &size[i])) goto error;
			if (size[i]) {
				something = true;
//				break; /* this break is optional, depending on how you want to test */
			}
		}
		
		if (!something) {
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}
		
		for (i=0;i<TOTAL;i++) {
			if (size[i]) {
				int n;
				fprintf(stderr, "msg%d: %p read %p, 0x%x bytes\n", i, msg[i], buffer[i], size[i]);
#if 1
				for (n=0;n<size[i];n++)
					printf("%02x", ((unsigned char *)buffer[i])[n]);
				printf("\n");
#endif
				bmessage_read_complete(msg[i], size[i]);
			}
		}
		
		j++;
	}

	for (i=0;i<TOTAL;i++)
		bmessage_stop(msg[i]);
	bsettop_uninit();

	return 0;
error:
	return 1;
}
