/***************************************************************************
 *     Copyright (c) 1998-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: playwave.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 8/25/06 1:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/playwave/playwave.c $
 * 
 * 4   8/25/06 1:38p rjlewis
 * PR20604: don't init before checking the parameters.
 * 
 * 3   8/25/06 11:52a rjlewis
 * PR20604: use "go" wrapper instead (common to all utils now).
 * 
 * 2   10/25/05 3:46p wesleyl
 * PR17328: Added 97398/vxworks support
 * 
 * 1   2/7/05 7:39p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   11/3/03 12:23p erickson
 * adapted examples and utils to settop api changes
 *
 * Irvine_BSEAVSW_Devel/1   9/25/03 11:32a erickson
 * added settop api based playwave util
 *
 * SanJose_Linux_Devel/8   4/22/03 9:56a erickson
 * added more options
 * changed buffering from 1 giant buffer to 16K segments
 *
 ****************************************************************/
#include  "bsettop.h"
#include "bpcmfile.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int g_silent = 0;

void printUsage()
{
	printf(
	"Usage: playwave [OPTIONS] wave_file [channel_number]\n"
	"  OPTIONS:\n"
	"    -h,--help     Print this help screen\n"
	"    -d DISPLAY    Display index (for dual output systems like 7320)\n"
	"\n"
	"  channel_number can be 1, 2, or 3. Default is 1.\n"
	"  If wave_file is -, it uses stdin.\n"
	);
}

int main(int argc, char *argv[])
{
	int i;
	FILE *f;
	int channel = 1;
	int displayindex = 0;
	const char *filename = NULL;
	bdisplay_t display;
	bpcm_play_t pcmplay;
	bresult result;

	/**
	* Read cmdline parameters. There is no short circuit because -h can be anywhere.
	**/
	for (i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			printUsage();
			return 0;
		}
		else if (!strcmp(argv[i], "-d") && argc > i+1)
			displayindex = atoi(argv[++i]);
		else if (!filename)
			filename = argv[i];
		else
			channel = atoi(argv[i]);
	}

	if (!filename) goto error;

	result = bsettop_init(BSETTOP_VERSION);
	if (result) return 1;

	display = bdisplay_open(B_ID(displayindex));
	if (!display) goto error;

	channel--; // convert to 0 based
	pcmplay = bpcm_play_open(B_ID(channel));
	if (!pcmplay) goto error;

	/* open the file */
	if (!strcmp(filename, "-")) {
		f = stdin;
		if (!g_silent) printf("Reading stdin\n");
	}
	else {
		if (!g_silent) printf("Opening file %s\n",filename);
		f = fopen(filename,"rb");
		if (f == NULL)
		{
			printf("Could not open file %s: %d\n",filename, errno);
			return -1;
		}
	}

	// play it
	return bpcm_play_wave(pcmplay, display, f);

error:
	printUsage();
	return 1;
}

