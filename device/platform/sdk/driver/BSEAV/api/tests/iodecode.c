/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: iodecode.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 7:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/iodecode.c $
 * 
 * 1   2/7/05 7:32p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   7/13/04 11:47a dlwin
 * PR11771: settop api dataflow redesign, requires changes to API
 * 
 * Irvine_BSEAVSW_Devel/2   12/23/03 4:39p vsilyaev
 * PR 8850: Set user input object into the async mode.
 * 
 * Irvine_BSEAVSW_Devel/5   10/28/03 11:27a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/4   9/2/03 2:18p erickson
 * fixed warnings
 * 
 * Irvine_BSEAVSW_Devel/3   8/18/03 12:09p vsilyaev
 * Fixed compilation issues.
 * 
 * Irvine_BSEAVSW_Devel/2   7/30/03 3:56p vsilyaev
 * Fixed bug with select and arg parsing.
 * 
 *
 ***************************************************************************/

#include "bsettop.h"
#include <stdio.h>

#define	IO_REMOTE	0
#define	IO_KEYPAD	3
void remote_input_callback(void *context)
{
	printf(" +++ Remote Input Callback, context=%lx\n", (unsigned long) context);
}

void keypad_input_callback(void *context)
{
	printf(" +++ Keypad Input Callback, context=%lx\n", (unsigned long) context);
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	buser_input_t remote = buser_input_open(B_ID(IO_REMOTE));
	buser_input_t keypad = buser_input_open(B_ID(IO_KEYPAD));
	buser_output_t led = buser_output_open(B_ID(0));
	btuner_t tuner = btuner_open(B_ID(0));
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	buser_input_settings settings;
	int n=0;
	buser_input_event event;
	unsigned nevents;
	btuner_qam_params qam;
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	if (rc) goto error;

	btuner_qam_params_init(&qam, tuner);
	qam.symbol_rate = 5056900;
	qam.mode = 64;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x21;

#if USE_STREAMER
	printf("Decoding from a streamer...\n");
	band = bstreamer_attach( B_ID(2), bstream_mpeg_type_ts );
#else
	printf("Decoding from a QAM demod...\n");
	band = btuner_tune_qam(tuner, 765000000, &qam);
#endif

	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;


	buser_output_display_message(led, "idle");

	buser_input_get_settings(remote, &settings);
	settings.data_ready_callback = remote_input_callback;
	settings.callback_context = 0xBABE0000;
	buser_input_set_settings(remote, &settings);
	buser_input_get_settings(keypad, &settings);
	settings.data_ready_callback = keypad_input_callback;
	settings.callback_context = 0xBABEFFFF;
	buser_input_set_settings(keypad, &settings);
	for(;;) {
		buser_input_event remote_ui_event, keypad_ui_event;
		int remote_nevents, keypad_nevents;

		buser_output_set_led(led, n%4, n>4?false:true);
		rc = buser_input_get_event(remote, &remote_ui_event, 1, &remote_nevents);
		if (rc!=b_ok) {
			fprintf(stderr, "Can't read event from remote");
			return 1;
		}
		rc = buser_input_get_event(keypad, &keypad_ui_event, 1, &keypad_nevents);
		if (rc!=b_ok) {
			fprintf(stderr, "Can't read event from remote");
			return 1;
		}
		if (remote_nevents == 0 && keypad_nevents == 0) {
			usleep(100000);
			continue;
		}
		if( remote_nevents != 0 )
		{
			printf("Remote Event<%04x>\n", remote_ui_event.code);
		}
		if( keypad_nevents != 0 )
		{
			printf("Keypad Event<%04x>\n", keypad_ui_event.code);
		}
		n = (n+1)%8;
	}

error:
	return 1;
}



