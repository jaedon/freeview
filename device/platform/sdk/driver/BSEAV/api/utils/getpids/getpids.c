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
 * $brcm_Workfile: getpids.c $
 * $brcm_Revision: 12 $
 * $brcm_Date: 9/6/12 6:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/getpids/getpids.c $
 * 
 * 12   9/6/12 6:03p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bsettop_types.h"
#include "tspsimgr.h"
#ifdef DSS_SUPPORT
#include "dsstspsimgr.h"
#endif
#include "bsettop.h"

void printinfo(CHANNEL_INFO_T *info) {
	int i,j;

	printf(
		"\n"
		"version %d\n"
		"transport_stream_id %d\n"
		"sectionBitmap %d\n"
		"num_programs %d\n\n",
		info->version,
		info->transport_stream_id,
		info->sectionBitmap,
		info->num_programs);
	for (i=0;i<info->num_programs;i++) {
		printf(
			"program_number %d\n"
			"  version %d\n"
			"  pcr pid 0x%x\n"
			"  video_pids:",
			info->program_info[i].program_number,
			info->program_info[i].version,
			info->program_info[i].pcr_pid);
		for (j=0;j<info->program_info[i].num_video_pids;j++)
			printf(" 0x%x/%d",
				info->program_info[i].video_pids[j].pid,
				info->program_info[i].video_pids[j].streamType);
		printf("\n");

		printf("  audio_pids:");
		for (j=0;j<info->program_info[i].num_audio_pids;j++)
			printf(" 0x%x/%d",
				info->program_info[i].audio_pids[j].pid,
				info->program_info[i].audio_pids[j].streamType);
		printf("\n");

		printf("  other_pids:");
		for (j=0;j<info->program_info[i].num_other_pids;j++)
			printf(" 0x%x/%d",
				info->program_info[i].other_pids[j].pid,
				info->program_info[i].other_pids[j].streamType);
		printf("\n");
	}
}

void printUsage(void) {
	printf(
	"Usage: getpids INPUT PIDTYPE\n"
	"  INPUT:\n"
	"     STR1|STR2|STR3\n"
	"     DS1|DS2 (default)\n"
	"     SDS <freq> (where freq is in MHz)\n"
	"     QAM64|256 <freq> (where freq is in MHz)\n"
	"     playback_filename\n"
	"     Band#\n"
	"  PIDTYPE:\n"
	"     DVB (default)\n"
	"     DSS\n"
	);

}

/* coverity[ -tainted_string_sink_content : arg-0 ] */
bstream_t start_playback(const char *fname, bstream_mpeg_type mpeg_type) {
	bplayback_file_t file;
	bstream_t stream;
	bplayback_t playback;
	bplaypump_t playpump;
	bplayback_params playback_params;
	bstream_mpeg mpeg;

	file = bplayback_file_open(fname, NULL);
	if (!file) {
		return NULL;
	}
	playpump = bplaypump_open(B_ID(0), NULL);
	if (!playpump) {
		return NULL;
	}
	playback = bplayback_open();
	if (!playback) {
		return NULL;
	}
	bstream_mpeg_init(&mpeg);
	mpeg.mpeg_type = mpeg_type; /* all other parameters are irrelevant for our needs */
	bplayback_params_init(&playback_params, playback);
	playback_params.route_through_parser_band = true;
	stream = bplayback_start(playback, playpump, &mpeg, file, &playback_params);
	
	return stream;
}

int main(int argc, char **argv) {
	bresult ret;
	bband_t band = NULL;
	bband_settings band_settings;
	btuner_t tuner = NULL;
	bool isDss = false;
	bstream_mpeg_type mpeg_type = bstream_mpeg_type_ts;
	CHANNEL_INFO_T *info = NULL;
	int rc;
	const char *fname="";
	bstream_t stream = NULL;
	bstream_status stream_status;

	if (argc >= 2 && !strcmp(argv[1], "--help")) {
		printUsage();
		exit(0);
	}

	if (argc >= 3 && !strcasecmp(argv[2], "DSS")) {
		isDss = true;
		mpeg_type = bstream_mpeg_type_dss_es;
	}

	ret = bsettop_init(BSETTOP_VERSION);
	if (ret) {
        printf("settop failed to init!\n");
        return -1;
    }
	
	if (argc >= 2 && !strcasecmp(argv[1], "STR1"))
		band = bstreamer_attach(B_ID(0), mpeg_type);
	else if (argc >= 2 && !strcasecmp(argv[1], "STR2"))
		band = bstreamer_attach(B_ID(1), mpeg_type);
	else if (argc >= 2 && !strcasecmp(argv[1], "STR3"))
		band = bstreamer_attach(B_ID(2), mpeg_type);
	else if ((argc >= 2 && !strcasecmp(argv[1], "DS1")) || argc < 2)
		band = bstreamer_attach(B_ID(10), mpeg_type);
	else if (argc >= 2 && !strcasecmp(argv[1], "DS2"))
		band = bstreamer_attach(B_ID(11), mpeg_type);
	else if (argc >= 3 && !strcasecmp(argv[1], "SDS")) {
		tuner = btuner_open(B_ID(2));
		if (!tuner)
			tuner = btuner_open(B_ID(3));
		if (tuner) {
			btuner_sds_params sds;
			int freq = atoi(argv[2]) * 1000000;
			btuner_sds_params_init(&sds, tuner);
			sds.symbol_rate = 20000000;
			band = btuner_tune_sds(tuner, freq, &sds);
		}
	}
	else if (argc >= 3 && (!strcasecmp(argv[1], "QAM64") ||
		!strcasecmp(argv[1], "QAM256"))) 
	{
		tuner = btuner_open(B_ID(0));
		if (tuner) {
			btuner_qam_params qam;
			int freq = atoi(argv[2]) * 1000000;
			btuner_qam_params_init(&qam, tuner);
			qam.mode = atoi(&argv[1][3]);
			band = btuner_tune_qam(tuner, freq, &qam);
		}
	}
	else if (argc >= 2) {
		fname = argv[1];
		stream = start_playback(fname, mpeg_type);
		
		/* if playback start fails, assume it's another param */
		if (!stream) {
			band = bstreamer_attach(B_ID(atoi(argv[1])+100), mpeg_type);
		}
	}

	if (!band && !stream) {
		printf("Unable to get stream.\n");
		return -1;
	}
	
	if( band ) {
		bband_get(band, &band_settings);
		printf("Reading from band %d[%s], %s\n",
			band_settings.bandnum, fname, isDss?"DSS":"DVB");
	}
	else if( stream ) {
		bstream_get_mpeg_parameters(stream, &stream_status);
		printf("Reading from stream %d[%s], %s\n",
			stream_status.parser_band, fname, isDss?"DSS":"DVB");
		
	}

	if ( (info = (CHANNEL_INFO_T *) malloc(sizeof(CHANNEL_INFO_T)))
		== NULL )
	{
		printf("malloc failed!\n");
		exit(1);
	}

	if (isDss) {
#ifdef DSS_SUPPORT
		rc = dssTsPsi_getChannelInfo(info, band->bandnum);
		if (!rc)
			printinfo(info);
		else
			printf("dsstsPsi_getChannelInfo() failed: %d\n", rc);
#else
		printf("DSS support isn't enabled.\n");
		if (info)
			free(info);
		return 1;
#endif
	}
	else {
		rc = tsPsi_getChannelInfo(info, band, stream);
		if (!rc)
			printinfo(info);
		else
			printf("tsPsi_getChannelInfo() failed: %d\n", rc);
	}
	
	if (tuner) {
		btuner_close(tuner);
		tuner = NULL;
	}
	if (info)
		free(info);

	return 0;
}
