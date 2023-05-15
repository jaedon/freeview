/***************************************************************************
 *     Copyright (c) 2003-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: userio.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 8/16/05 11:48a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/userio.c $
 * 
 * 4   8/16/05 11:48a erickson
 * PR13823: rewrite userio callback handlers to be valid
 * 
 * 3   4/5/05 10:16a erickson
 * PR13823: made example more crossplatform by testing for valid handles
 * 
 * 2   2/22/05 1:55p erickson
 * PR14180: now working on vxworks
 * 
 * 1   2/7/05 7:03p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   7/21/04 4:55p erickson
 * PR12026: cleaned up warnings
 * 
 * Irvine_BSEAVSW_Devel/3   7/6/04 3:54p erickson
 * PR11771: settop api dataflow redesign
 *
 * Irvine_BSEAVSW_Devel/2   1/22/04 5:09p vsilyaev
 * PR 8850: Set input devies to the async mode.
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
#include "bstd.h"
#include "bkni.h"

buser_output_t led;

void callback(void *context)
{
	/* generic callback for all ui handles. this is the ideal callback impl: just set an event. */
	BKNI_SetEvent((BKNI_EventHandle)context);
}

/* process commands are called from the main context, not the callback */
void process_remote(buser_input_t ui)
{
	buser_input_event event;
	unsigned nevents;
	buser_input_get_event(ui, &event, 1, &nevents);
	if (nevents) {
		if (led)
			buser_output_display_message(led, "remt");
		printf("Got remote %d\n", (int)event.code);
	}
}

void process_keypad(buser_input_t ui)
{
	buser_input_event event;
	unsigned nevents;
	buser_input_get_event(ui, &event, 1, &nevents);
	if (nevents) {
		if (led)
			buser_output_display_message(led, "keyp");
		printf("Got keypad %d\n", (int)event.code);
	}
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	buser_input_t remote = buser_input_open(B_ID(0));
	buser_input_t keypad = buser_input_open(B_ID(3));
	buser_input_settings settings;
	BKNI_EventHandle event;
	int n=0;
	
	if (rc) return -1;
	
	BKNI_CreateEvent(&event);

	led = buser_output_open(B_ID(0));

	if (remote) {
		buser_input_get_settings(remote, &settings);
		settings.data_ready_callback = callback;
		settings.callback_context = event;
		buser_input_set_settings(remote, &settings);
	}
	if (keypad) {
		buser_input_get_settings(keypad, &settings);
		settings.data_ready_callback = callback;
		settings.callback_context = event;
		buser_input_set_settings(keypad, &settings);
	}
	if (led) {
		buser_output_display_message(led, "idle");
	}
	
	for(;;) {
		if (led) {
			buser_output_set_led(led, n%4, n>=4?false:true);
			n = (n+1)%8;
		}
		
		/* wait for event or timeout to update leds */
		BKNI_WaitForEvent(event, 500);
		
		if (remote) process_remote(remote);
		if (keypad) process_keypad(keypad);
	}
	return 0;
}
