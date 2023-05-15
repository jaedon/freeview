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
 * $brcm_Workfile: encode_93740.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/encode_93740.c $
 * 
 * 3   9/6/12 6:03p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <sys/times.h>

int 
AnalogChannelFrequency(unsigned long channel, float *freq)
{
	unsigned long int_freq;

	/* CATV channels lineup */
	if (channel>=2 && channel<=6) {
		int_freq = 5525 + (channel-1)*600;
	} else if (channel>=7 && channel<=13) {
		int_freq = 17525 + (channel-7)*600;
	} else if (channel>=14 && channel<=22) {
		int_freq = 12125 + (channel - 14)*600;
	} else if (channel>=23 && channel<=94) {
		int_freq = 21725 + (channel - 23)*600;
	} else if (channel>=95 && channel<=97) {
		int_freq = 9125 + (channel - 95)*600;
	} else if (channel>=97 && channel<=100) {
		int_freq = 103.25 + (channel - 97)*600;
	} else if (channel>=100 && channel<=125) {
		int_freq = 649.25 + (channel - 100)*600;
	} else {
		return -1;
	}
	*freq =(int_freq+200)/100.0;
	return 0;
}

void next_channel(unsigned long *chn)
{
	(*chn)++;
	if (*chn>100) {
		*chn=3;
	}
}

int 
do_sleep(unsigned delay)
{
	fd_set rfds;
	struct timeval tv;
	int retval;

															           
	printf("\r....encoding..press enter to exit...\r");
	fflush(stdout);

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	/* Wait up to five seconds. */
	tv.tv_sec = delay;
	tv.tv_usec = 0;
	retval = select(1, &rfds, NULL, NULL, &tv);
	printf("\r%70s\r","");
	fflush(stdout);

	return retval;
}

int main(void) 
{
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner0 = btuner_open(B_ID(0));
	btuner_t tuner1 = btuner_open(B_ID(1));
	bencode_t encode0 = bencode_open(B_ID(0));
	bencode_t encode1 = bencode_open(B_ID(1));
	bdecode_t decode0 = bdecode_open(B_ID(0));
	bdecode_t decode1 = bdecode_open(B_ID(1));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window0 = bdecode_window_open(B_ID(0), display);
	bdecode_window_t window1 = bdecode_window_open(B_ID(1), display);
	unsigned long chn0 = 3, chn1 = 40;
	float freq;
	int iter;

	btuner_analog_params analog;
	bencode_settings encode_settings;
	bstream_t stream0=NULL, stream1=NULL;

	bencode_settings_init(&encode_settings, bencode_quality_high, encode0);
	btuner_analog_params_init(&analog, tuner0);

	for(iter=0;;iter++) {
		if (stream0) {
			printf("Stopping encode %d\n", 0);
			bstream_close(stream0);
		}
	
		printf("(%d)Starting encoding encoder %d analog channel %ld\n", iter, 0 , chn0);
		AnalogChannelFrequency(chn0, &freq);
		stream0 = btuner_tune_rf(tuner1, freq, &analog);
		stream0 = bencode_start(encode0, stream0, &encode_settings);
		bdecode_start(decode0, stream0, window0);
		if (do_sleep(5)) {
			break;
		}
		next_channel(&chn0);

		if (stream1) {
			printf("Stopping encode %d\n", 1);
			bstream_close(stream1);
		}

		printf("(%d)Starting encoding encoder %d analog channel %ld\n", iter, 1 , chn1);
		AnalogChannelFrequency(chn1, &freq);
		stream1 = btuner_tune_rf(tuner0, freq, &analog);
		stream1 = bencode_start(encode1, stream1, &encode_settings);
		bdecode_start(decode1, stream1, window1);
		if (do_sleep(5)) {
			break;
		}
		next_channel(&chn1);
	}
	printf("Done, Completed %d iterations\n", iter);

	return 0;
}
