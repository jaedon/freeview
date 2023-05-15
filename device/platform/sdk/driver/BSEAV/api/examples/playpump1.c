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
 * $brcm_Workfile: playpump1.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/playpump1.c $
 * 
 * 8   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#ifdef __vxworks
#include <taskLib.h>
#include <semLib.h>
#include <usrConfig.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

/* This is an example of playpump with threads. If you want to see simpler
code with no thread, see playpump2.c. */

struct context {
	FILE *file;
	bplaypump_t play;
	BKNI_EventHandle event;
} g_context;

void read_callback(void *context);
void *play_func(void *context);

void prompt(const char *msg) {
	printf("\n%s\n", msg);
	getchar();
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bplaypump_params playpump_params;
	bplaypump_settings settings;
	bstream_mpeg mpeg;
	bstream_t stream;
#ifdef __vxworks
	int consumer_handle;
#else	
	pthread_t thread;
#endif	

	if (rc) goto error;

#ifdef __vxworks
	usrAtaConfig(0,0,"/ata0");
#endif

	BKNI_CreateEvent(&g_context.event);
	g_context.play = bplaypump_open(B_ID(0), NULL);
#ifdef __vxworks
	g_context.file = fopen("/ata0/stream.mpg", "r");
#else
	g_context.file = fopen("stream.mpg", "r");
#endif
	if (!g_context.file) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x21;

	bplaypump_params_init(&playpump_params, g_context.play);
	playpump_params.read_callback = read_callback;
	playpump_params.callback_context = g_context.event;

	stream = bplaypump_start(g_context.play, &mpeg, &playpump_params);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

#ifdef __vxworks
	consumer_handle = taskSpawn("playpump_task", 100, VX_FP_TASK, 20*1024, (FUNCPTR)play_func, (int)&g_context, 0,0,0,0,0,0,0,0,0);
	if (consumer_handle == (int)NULL)
	{
		printf("ERROR: Can't taskSpawn consumer thread!\n");
		goto error;
	}
#else
	if (pthread_create(&thread, NULL, play_func, &g_context))
		goto error;
#endif

	bplaypump_get(g_context.play, &settings);
	prompt("press enter to enter slow motion");
	settings.decode_rate = BPLAYPUMP_NORMALPLAY_SPEED/2;
	bplaypump_set(g_context.play, &settings);
	prompt("press enter to pause");
	settings.decode_rate = 0;
	bplaypump_set(g_context.play, &settings);
	prompt("press enter to resume normal play");
	settings.decode_rate = BPLAYPUMP_NORMALPLAY_SPEED;
	bplaypump_set(g_context.play, &settings);
	prompt("press enter to stop");

	bplaypump_stop(g_context.play);
	BKNI_SetEvent(g_context.event);
#ifndef __vxworks	
	pthread_join(thread, NULL);
#endif

	return 0;
error:
	return 1;
}

void read_callback(void *context)
{
	/* We can't do the read and read_complete in the callback, so notify the thread */
	BKNI_SetEvent((BKNI_EventHandle)context);
}

void *play_func(void *context_)
{
	struct context *context = (struct context *)context_;

	while (1) 
	{
		void *buffer;
		size_t buffer_size;
		int n;

		if (bplaypump_get_buffer(context->play, &buffer, &buffer_size)) {
			printf("bplaypump_get_buffer failed\n");
			break;
		}
		if (!buffer_size) {
			BKNI_WaitForEvent(context->event, BKNI_INFINITE);
			continue;
		}

		/* The first call to get_buffer will return the entire playback buffer.
		If we use it, we're going to have to wait until the descriptor is complete,
		and then we're going to underflow. So use a max size. */
#define MAX_READ 26320
		if (buffer_size > MAX_READ)
			buffer_size = MAX_READ;

		n = fread(buffer, 1, buffer_size, context->file);
		if (n < 0) break;
		if (n == 0) {
			BKNI_Sleep(1000); /* This is a simplistic way of making sure the MVD buffer
				is empty before flushing. See bsettop_playback for a more robust
				method. */
			fseek(context->file, 0, SEEK_SET);
			bplaypump_flush(context->play);
		}
		else {
			if (bplaypump_read_complete(context->play, 0, n)) {
				printf("bplaypump_read_complete failed\n");
				break;
			}
		}
	}
	printf("play thread exiting\n");
	return NULL;
}

/* 
NOTES: 
On legacy platforms, starting the thread after bdecode_start is 
important. If you need to start the thread before bdecode_start, be aware
that bplaypump_get_buffer will fail until bdecode_start is called.
This is a side-effect from the way the underlying drivers were designed.
If you want to start the thread immediately after bplaypump_start, another
alternative is to wait for the event at the beginning of the thread. The playback
callback is guaranteed to fire after a playback actually starts (in this case
when bdecode_start is called).
*/
