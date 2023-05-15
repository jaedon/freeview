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
 * $brcm_Workfile: pcmplay.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 9/6/12 6:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/pcmplay.c $
 * 
 * 6   9/6/12 6:03p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <unistd.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#include <stdio.h>

/* Play PCM from stdin */

void pcm_callback(void *context)
{
	static int i=0;
	BKNI_SetEvent((BKNI_EventHandle)context);

	i++;
	if( (i%1000) == 0 )
		printf(".");
}

int main(int argc, char **argv)
{
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display = bdisplay_open(B_ID(0));
	bpcm_play_t pcm_play = bpcm_play_open(B_ID(0));
	bpcm_play_settings settings;
	BKNI_EventHandle event;
	if (rc) goto error;

	BKNI_CreateEvent(&event);

	bpcm_play_settings_init(&settings, pcm_play);
	settings.callback = pcm_callback;
	settings.callback_context = event;
	if (argc >= 2) settings.pcm.sample_rate = atoi(argv[1]);
	if (argc >= 3) settings.pcm.bits_per_sample = atoi(argv[2]);
	if (argc >= 4) settings.pcm.channels = atoi(argv[3]);
	printf("pcmplay: %d samples/sec, %d bits/sample, %d channels\n",
		settings.pcm.sample_rate, settings.pcm.bits_per_sample, settings.pcm.channels);
	
	if (bpcm_play_start(pcm_play, display, &settings))
		goto error;

	while (1) {
		void *buffer;
		size_t size;
		ssize_t nread;

		if (bpcm_play_get_buffer(pcm_play, &buffer, &size))
			goto error;

		if (!size) {
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}

		/* read as much as possible from stdin */
		nread = read(0, buffer, size);
		if (!nread) break;
		if (nread < 0) goto error;
        
#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
#define SWAP16( a )  do{a=((a&0xFF)<<8|(a&0xFF00)>>8);}while(0)
		if (settings.pcm.bits_per_sample == 16)
		{		    
			int i;
			unsigned short *buf = buffer;

			for (i=0; i<nread/2; i++)
				SWAP16(buf[i]);
		}
#endif

		if (bpcm_play_write_complete(pcm_play, nread))
			goto error;
	}

	return 0;
error:
	return 1;
}
