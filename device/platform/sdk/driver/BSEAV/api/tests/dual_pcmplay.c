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
 * $brcm_Workfile: dual_pcmplay.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/dual_pcmplay.c $
 * 
 * 2   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <unistd.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#include <stdio.h>
#include <fcntl.h>

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
	bpcm_play_t pcm_play0 = bpcm_play_open(B_ID(0));
	bpcm_play_t pcm_play1 = bpcm_play_open(B_ID(1));
	bpcm_play_settings settings;
	BKNI_EventHandle event;
	int file0, file1;
	if (rc) goto error;

	BKNI_CreateEvent(&event);
	
	file0 = open(argv[1], O_RDONLY);
	file1 = open(argv[2], O_RDONLY);

	bpcm_play_settings_init(&settings, pcm_play0);
	settings.callback = pcm_callback;
	settings.callback_context = event;
	if (bpcm_play_start(pcm_play0, display, &settings))
		goto error;

	bpcm_play_settings_init(&settings, pcm_play1);
	settings.callback = pcm_callback;
	settings.callback_context = event; /* same event */
	if (bpcm_play_start(pcm_play1, display, &settings))
		goto error;
		
	while (1) {
		void *buffer0, *buffer1;
		size_t size0, size1;
		ssize_t nread0 = 0, nread1 = 0;

		if (bpcm_play_get_buffer(pcm_play0, &buffer0, &size0))
			goto error;
		if (bpcm_play_get_buffer(pcm_play1, &buffer1, &size1))
			goto error;

		if (!size0 && !size1) {
			BKNI_WaitForEvent(event, BKNI_INFINITE);
			continue;
		}

		if (size0) {
			nread0 = read(file0, buffer0, size0);
			if (nread0 < 0) goto error;
			if (nread0 && bpcm_play_write_complete(pcm_play0, nread0))
				goto error;
		}
		if (size1) {
			nread1 = read(file1, buffer1, size1);
			if (nread1 < 0) goto error;
			if (nread1 && bpcm_play_write_complete(pcm_play1, nread1))
				goto error;
		}
		
		if (size0 && size1 && !nread0 && !nread1) {
			break;
		}
	}

	return 0;
error:
	return 1;
}
