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
 * $brcm_Workfile: message_pes.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/message_pes.c $
 * 
 * 5   9/6/12 6:06p erickson
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

void overflow_callback(void *context)
{
    printf("Got overflow !\n" );
}


int main() {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bband_t band;
	bmessage_stream_t msg = NULL;
	bstream_t stream;
	bstream_mpeg mpeg;
	bmessage_stream_params params;

	const void *buffer;
	size_t size;
	BKNI_EventHandle event;
	FILE *recfile;
	int loop=5000;	/* increase this to record a longer duration stream */

	if (rc) goto error;

	BKNI_CreateEvent(&event);

    recfile = fopen("pes_stream.mpg", "w+");
    if( !recfile ) goto error;

	bmessage_stream_params_init(&params, msg);
	params.pid = 0;
	/* use the default filter */
	params.data_ready_callback = message_callback;
	params.callback_context = event;
	params.overflow = overflow_callback;
	params.buffer_size = 128;

#ifdef USE_QAM
	{
	btuner_qam_params qam;
	btuner_t tuner = btuner_open(bconfig->resources.qam.objects[0]);

	btuner_qam_params_init(&qam, tuner);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	printf("Decoding from a QAM demod...\n");
	band = btuner_tune_qam(tuner, 765000000, &qam);
	}
#else
	{
	btuner_vsb_params   vsb;
	btuner_t tuner = btuner_open(bconfig->resources.vsb.objects[0]);
	btuner_vsb_params_init(&vsb, tuner);
	printf("Tuning to VSB channel 575 MHz\n" );
	band = btuner_tune_vsb(tuner, 575000000, &vsb);
	}
#endif	

	params.pid = 0x31;		/* set this to the pid you want to PES filter on */

	if (!band) goto error;

	params.band = band;

	msg = bmessage_open(bmessage_format_pes); 
	if (!msg) goto error;

	if (bmessage_start(msg, &params)) goto error;

	/* Record a short clip to file 	*/
	/* can playback with : 			*/
	/* 	./playback -video 1 -mpeg_type ES pes_stream.mpg */
	/* to confirm recording.		*/
	while (loop--) {
		if (bmessage_get_buffer(msg, &buffer, &size)) goto error;
		if (!size) {
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		} else {
			fwrite( buffer, 1, size, recfile );
			bmessage_read_complete(msg, size);
		}
	}

	/* All consumed */

	bmessage_stop(msg);

	fclose( recfile );

	bsettop_uninit();

	return 0;
error:
	return 1;
}
