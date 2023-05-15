/***************************************************************************
 *     Copyright (c) 2003-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: record_test.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 12/22/06 11:32a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/record/record_test.c $
 * 
 * 10   12/22/06 11:32a rjlewis
 * PR26782: vxworks compile error.
 * 
 * 9   10/31/06 4:20p ahulse
 * PR25090: bargs need initializing via bargs_init()
 * 
 * 8   5/2/06 10:04a erickson
 * PR17108: fixed include path
 * 
 * 7   2/2/06 5:04p vsilyaev
 * PR 19233:Use common function for tune. Added diseqc_tone option
 * 
 * 6   12/23/05 4:10p vsilyaev
 * PR 18183: Renamed API functions
 * 
 * 5   11/30/05 3:13p vsilyaev
 * PR 18183: Added FIFO file support
 * 
 * 4   8/4/05 4:29p vsilyaev
 * PR 13097: Added video coding type.
 * 
 * 3   4/20/05 3:01p vsilyaev
 * PR 14724: Improved parsing of timestamp option
 * 
 * 2   2/24/05 4:01p erickson
 * PR14180: added more flexible go() command for vxworks
 * 
 * 1   2/7/05 7:39p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/22   1/21/05 11:32a vsilyaev
 * PR 13803: Fixed code to allow work on platforms without tuners.
 * 
 * Irvine_BSEAVSW_Devel/21   11/23/04 8:14p vsilyaev
 * PR 13351: Refactoring playback API.
 * 
 * Irvine_BSEAVSW_Devel/20   5/12/04 2:31p vsilyaev
 * PR 10964: Added DSS support and preview of recorded data.
 * 
 * Irvine_BSEAVSW_Devel/19   5/7/04 12:31p vsilyaev
 * PR 10939: Added support for encoder selection.
 * 
 * Irvine_BSEAVSW_Devel/18   3/17/04 8:30p vsilyaev
 * PR 9465: Added VSB support.
 * 
 * Irvine_BSEAVSW_Devel/17   3/15/04 12:54p vsilyaev
 * PR 10065: Use 'nav' as the default prefix for the index file.
 * 
 * Irvine_BSEAVSW_Devel/16   11/10/03 2:38p erickson
 * PR8563: added transport timestamp support to driver, settop api and
 * bcmplayer
 * 
 * Irvine_BSEAVSW_Devel/15   11/3/03 12:23p erickson
 * adapted examples and utils to settop api changes
 * 
 * Irvine_BSEAVSW_Devel/14   10/28/03 11:26a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/13   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 * 
 * Irvine_BSEAVSW_Devel/12   9/2/03 2:18p erickson
 * fixed encode api call
 * 
 * Irvine_BSEAVSW_Devel/11   8/18/03 12:09p vsilyaev
 * Fixed compilation issues.
 * 
 * Irvine_BSEAVSW_Devel/10   8/12/03 6:50p vsilyaev
 * Updated encoder settings and renamed bencode_get_settings to the
 * bencode_get.
 * 
 * Irvine_BSEAVSW_Devel/9   8/12/03 9:43a erickson
 * changes after pre-review meeting
 * 
 * Irvine_BSEAVSW_Devel/8   7/29/03 6:02p erickson
 * changed file open api
 * 
 * Irvine_BSEAVSW_Devel/7   7/25/03 8:15p vsilyaev
 * Fixed for new api.
 *
 * Irvine_BSEAVSW_Devel/6   7/24/03 4:37p vsilyaev
 * Added support for second record channel and SDS.
 *
 * Irvine_BSEAVSW_Devel/5   7/21/03 2:06p vsilyaev
 * Fixed for decoupled stream, bands and encoder.
 *
 * Irvine_BSEAVSW_Devel/4   7/1/03 7:19p vsilyaev
 * Added support to recording of encoded analog video.
 *
 * Irvine_BSEAVSW_Devel/3   6/25/03 11:23a vsilyaev
 * Added support for command line arguments, removed hardcoded defaults.
 *
 * Irvine_BSEAVSW_Devel/2   6/24/03 5:30p vsilyaev
 * Use brecord_file_t and bplayback_file_t instead bfile_in and bfile_out.
 *
 * Irvine_BSEAVSW_Devel/3   6/20/03 5:56p vsilyaev
 * Added playback support.
 *
 * Irvine_BSEAVSW_Devel/2   6/13/03 3:26p vsilyaev
 * Fixed warnings.
 *
 * Irvine_BSEAVSW_Devel/1   6/13/03 3:10p vsilyaev
 * SetTop reference API.
 *
 *
 ***************************************************************************/

#include <stdio.h>
#ifndef __vxworks
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "bsettop.h"
#include "bargs.h"
#include "btune.h"
#include "../src/pvr/bsettop_pvr_priv.h"
#include "bsettop_fileio_fifo.h"


static struct b_extra_arg {
	bool *live;
	bool *playback;
	bool *fifo;
	struct {
		bool live;
		bool playback;
		bool fifo;
	} storage;
} b_extra_arg = {
	NULL,NULL,NULL,
	{false,false,false}
};

static const struct bargs bextra_args[] = {
	{"live", bargNone, parse_set, "start live decode", (void **)&b_extra_arg.live, &b_extra_arg.storage.live},
	{"playback", bargNone, parse_set, "start playback", (void **)&b_extra_arg.playback, &b_extra_arg.storage.playback},
	{"fifo", bargNone, parse_set, "use circular buffer(FIFO) for record", (void **)&b_extra_arg.fifo, &b_extra_arg.storage.fifo},
	BARGS_TERMINATOR
};

static const bargs *record_args[] = {
	b_args,
	bqam_args,
	bmpeg_args,
	banalog_args,
	bstreamer_args,
	bsds_args,
	btuner_args,
	brecord_args,
	bvsb_args,
	bextra_args,
	NULL
};


static void
usage(const char *name)
{
	fprintf(stderr, "%s: Settop record application\n", name);
	barg_usage(name, record_args, "<filename> [indexname]");
	return;
}

static void 
invalid_arg(int arg, const char *argv[])
{
	fprintf(stderr, "Invalid argument %s\n", argv[arg]);
	usage(argv[0]);
	exit(1);
}

int 
main(int argc, const char *argv[])
{
	bresult rc;

	bstream_mpeg mpeg;
	bstream_t stream;
	brecord_file_t file;
	brecpump_t recpump;
	brecord_t record;
	int last_arg;
	const char *fname="";
	const char *indxname=NULL;
	brecord_params params;
	char strbuf[256];
	bfile_out_fifo_t fifo_file=NULL;
	btune_state tune_state;

	bargs_init();

	last_arg = barg_parse(argv, record_args, 1, argc);
	if (last_arg<0) {
		invalid_arg(-last_arg, argv);
	} else if (last_arg==0) { /* there is should be at least one extra parameter */
		fprintf(stderr, "filename should be provided\n");
		usage(argv[0]);
		exit(1);
	} else {
		fname = argv[last_arg++];
		if (argc>last_arg) {
			indxname = argv[last_arg++];
		} else {
			strcpy(strbuf, fname);
			strcat(strbuf, ".nav");
			indxname = strbuf;
		}
	}
	rc = bsettop_init(BSETTOP_VERSION); /* initialize global stuff */
	if (rc!=b_ok) {
		fprintf(stderr, "Initialization error\n");
		return 1;
	}
	btune_mpeg_init(&mpeg);

	stream = btune_tune(0, &mpeg, true, &tune_state);
	if (!stream) {
		return 1;
	}

	if(b_extra_arg.fifo && b_extra_arg.fifo) {
	 	fifo_file = bfile_fifo_out_create(fname, indxname);
		if (!fifo_file) {
			perror(fname);
			return 1;
		}
		file = bfile_fifo_out_file(fifo_file);
	} else {
		file = brecord_file_open(fname,indxname);
		if (!file) {
			perror(fname);
			return 1;
		}
	}

	/* get a record channel, if record channel not set use the same channel as tuner channel */
	recpump = brecpump_open(B_ID(brecord_arg.channel?*brecord_arg.channel:btuner_arg.channel?*btuner_arg.channel:0), NULL);
	if (!recpump) {
		fprintf(stderr, "Record is not avaliable\n");
		return 1;
	}
	record = brecord_open();
	if (!record) {
		fprintf(stderr, "Record is not avaliable\n");
		return 1;
	}

	brecord_params_init(&params, record);
	if (bmpeg_arg.timestamp) {
		params.timestamp_enabled = *bmpeg_arg.timestamp;
		printf("timestamp %d\n", params.timestamp_enabled );
	}

	if (brecord_start(record, recpump, stream, file, &params)!=b_ok) {
		fprintf(stderr, "Can't start record\n");
		return 1;
	}
	if( (b_extra_arg.live && *b_extra_arg.live) || (b_extra_arg.playback && *b_extra_arg.playback)) {
		bdecode_t decode = bdecode_open(B_ID(0));
		bdisplay_t display = bdisplay_open(B_ID(0));
		bdecode_window_t window = bdecode_window_open(B_ID(0), display);

		if (b_extra_arg.live && *b_extra_arg.live) {
			bdecode_start(decode, stream, window);
		} else if (b_extra_arg.playback && *b_extra_arg.playback) {
			bplayback_params play_params;
			bplaypump_t playpump;
			bplayback_t playback;
			bstream_t stream;
			bplayback_file_t file;

			playpump = bplaypump_open(B_ID(0), NULL);
			if (!playpump) {
				fprintf(stderr, "Can't open playpump\n");
				return 1;
			}
			playback = bplayback_open();
			if (!playback) {
				fprintf(stderr, "Can't open playback\n");
				return 1;
			}
			bplayback_params_init(&play_params, playback);
			if (bmpeg_arg.timestamp) {
			  play_params.timestamp_enabled = *bmpeg_arg.timestamp;
			  play_params.timestamp_active = *bmpeg_arg.timestamp;
			  if (bmpeg_arg.ignore_timestamp) {
			  	play_params.timestamp_active = !(*bmpeg_arg.ignore_timestamp);
			  }
			}

			/* wait for record to write something */
#ifdef __vxworks
			BKNI_Sleep(2000);
#else
			sleep(2);
#endif
			if (fifo_file) {
				file = bfile_fifo_in_open(fname,indxname,fifo_file);
			} else {
				file = bplayback_file_open(fname,indxname);
			}
			if (!file) {
				fprintf(stderr, "Can't open playback file\n");
				return 1;
			}
			stream = bplayback_start_timeshifting(playback, playpump, &mpeg, file, &play_params, record);
			if (!stream) {
				fprintf(stderr, "Can't start playback\n");
				return 1;
			}
			bdecode_start(decode, stream, window);
		}

	}

	printf("Record has started\nPress any key to stop record...");
#ifdef __vxworks
	printf("Decode started\n");
	while (1) {BKNI_Sleep(1000);}
#else
	getchar();

	bstream_close(stream); /* current record would be closed here and tuner could be tuned to another channel */
	brecord_close(record);

	bsettop_uninit();

#endif

	return 0;
}
