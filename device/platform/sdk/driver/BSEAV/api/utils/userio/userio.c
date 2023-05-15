/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: userio.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 4/12/10 11:25a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/userio/userio.c $
 * 
 * 4   4/12/10 11:25a calvinho
 * SW7125-278: Added comment to escape coverity flag
 * 
 * 3   10/31/06 4:20p ahulse
 * PR25090: bargs need initializing via bargs_init()
 * 
 * 2   5/9/06 11:27a jrubio
 * PR18491: added UHF/RF mods
 * 
 * 1   2/7/05 7:40p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/8   7/6/04 3:53p erickson
 * PR11771: settop api dataflow redesign
 *
 * Irvine_BSEAVSW_Devel/7   5/27/04 2:17p erickson
 * PR11237: converted to BCHP_CHIP
 *
 * Irvine_BSEAVSW_Devel/6   12/17/03 4:22p erickson
 * PR8850: allow util to work with synchronous read (which is all we have
 * right now for 7038)
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "bsettop.h"
#include "bargs.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

static const bargs *decode_args[] = {
	buserio_args,
	NULL
};

static void
usage(const char *name)
{
	fprintf(stderr, "%s: Settop userio application\n", name);
	barg_usage(name, decode_args, "");
	return;
}

static void
invalid_arg(int arg, const char *argv[])
{
	fprintf(stderr, "Invalid argument %s\n", argv[arg]);
	usage(argv[0]);
	exit(1);
}

void user_input_callback(void *context)
{
	BKNI_SetEvent((BKNI_EventHandle)context);
}

int
main(int argc, const char *argv[])
{
	bresult rc;
	int last_arg;

	bargs_init();

	if (argc<2) {
		usage(argv[0]);
		return 1;
	}

	last_arg = barg_parse(argv, decode_args, 1, argc);
	if (last_arg<0) {
		invalid_arg(-last_arg, argv);
	} else if (last_arg!=0) { /* there is should be at least one parameter */
		invalid_arg(last_arg, argv);
		return 0;
	}
	rc = bsettop_init(BSETTOP_VERSION); /* initialize global stuff */
	if (rc!=b_ok) {
		fprintf(stderr, "Initialization error\n");
		return 1;
	}

	if (buserio_arg.message) {
	buser_output_t uo;

	 uo = buser_output_open(B_ID(0));
	 if (!uo) {
		 fprintf(stderr, "Can't open user output\n");
		 return 1;
	 }

	 rc = buser_output_display_message(uo, buserio_arg.message);
	 if (rc==b_ok) {
		 printf("message '%s' displayed", buserio_arg.message);
	 } else {
		 fprintf(stderr, "message '%s' can't be displayed", buserio_arg.message);
		 return 1;
	 }
	}

	if (buserio_arg.input) {
		buser_input_t ui;
		buser_input_settings settings;
		BKNI_EventHandle event;

		ui = buser_input_open(B_ID((int)*buserio_arg.input));
		if (!ui) {
			fprintf(stderr, "Can't open user input");
			return 1;
		}
		BKNI_CreateEvent(&event);
		buser_input_get_settings(ui, &settings);
		settings.data_ready_callback = user_input_callback;
		settings.callback_context = event;
		buser_input_set_settings(ui, &settings);

		for(;;) {
			buser_input_event ui_event;
			int nevents;

			rc = buser_input_get_event(ui, &ui_event, 1,(unsigned *) &nevents);
			if (rc!=b_ok) {
				fprintf(stderr, "Can't read event from remote");
				return 1;
			}
			if (nevents==0) {
        /* coverity[unchecked_value] */
				BKNI_WaitForEvent(event, BKNI_INFINITE);
				continue;
			}
			printf("Event<0x%08x>\n", ui_event.code);
		}

		buser_input_close(ui);
	}
	return 0;
}


