/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: allpassrec.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 2/15/07 1:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/allpassrec.c $
 * 
 * 7   2/15/07 1:19p erickson
 * PR19790: added LDPC support
 * 
 * 6   8/15/06 1:29p erickson
 * PR19838: added vsb. added sds diseqc tone.
 * 
 * 5   8/1/06 4:25p jrubio
 * PR23047: Added streamer input
 * 
 * 4   10/10/05 5:12p erickson
 * PR17044: added qpsk/qpsk_dss/8psk option, fixed crash
 * 
 * 3   9/19/05 1:53p erickson
 * PR17044: updated str logic
 * 
 * 2   9/7/05 2:01p erickson
 * PR17044: remove n from filename, added QAM support
 * 
 * 1   9/7/05 11:03a erickson
 * PR17044: added new allpassrec util
 * 
 ****************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <string.h>

#define BUFSIZE 128
char buf[BUFSIZE];
	
#define MAXSESSIONS 1
struct session {
	/* handles */
	btuner_t tuner;
	bband_t band;
	bstream_t stream;
	brecpump_t recpump;
	brecord_t record;
	brecord_params record_params;
	brecord_file_t file;
	
	/* tuner and acq params */
	int freq;
	enum {
		btuner_type_sds,
		btuner_type_qam,
		btuner_type_vsb,
        btuner_streamer
	} type;
	btuner_sds_params sds;
	btuner_qam_params qam;
	btuner_vsb_params vsb;
} g_sessions[MAXSESSIONS];

int total = 0;

/* removes trailing \n if present */
void chop(char *s)
{
	int len = strlen(s);
	if (len && s[len-1] == '\n')
		s[len-1] = 0;
}

void config_sds(struct session *session)
{
	session->type = btuner_type_sds;
	btuner_sds_params_init(&session->sds, session->tuner);
	
	printf("Enter mode (qpsk, 8psk, qpsk_dss, qpsk_ldpc): ");fflush(stdout);
	fgets(buf, BUFSIZE, stdin);
	chop(buf);
	if (!strcasecmp(buf, "qpsk")) {
		session->sds.mode = btuner_sds_mode_qpsk_dvb;
	}
	else if (!strcasecmp(buf, "qpsk_dss")) {
		session->sds.mode = btuner_sds_mode_qpsk_dss;
	}
	else if (!strcasecmp(buf, "qpsk_ldpc")) {
		btuner_sds_coderate coderate = {4,5,0};
		session->sds.mode = btuner_sds_mode_qpsk_ldpc;
		/* TODO: allow ldpc config. for now, hardcode */
		session->sds.ldpc_pilot = true;
		session->sds.coderate = coderate;
	}
	else {
		session->sds.mode = btuner_sds_mode_turbo_8psk;
	}
	
	/* TODO: code rate */
	
	printf("Enter frequency (in Hz or MHz): ");fflush(stdout);
	fgets(buf, BUFSIZE, stdin);
	session->freq = atoi(buf);
	if (session->freq < 20000)
		session->freq *= 1000000;
	
	printf("Enter symbol rate (in Baud or MBaud): ");fflush(stdout);
	fgets(buf, BUFSIZE, stdin);
	session->sds.symbol_rate = atoi(buf);
	if (session->sds.symbol_rate < 1000)
		session->sds.symbol_rate *= 1000000;
	
	printf("Enter diseqc polarity (13 or 18): ");fflush(stdout);
	fgets(buf, BUFSIZE, stdin);
	session->sds.diseqc_polarity = atoi(buf);

	printf("Enter diseqc tone ('on' or 'off'): ");fflush(stdout);
	fgets(buf, BUFSIZE, stdin);
	chop(buf);
	session->sds.diseqc_tone = !strcmp(buf, "on");
}

void config_qam(struct session *session)
{
	session->type = btuner_type_qam;
	btuner_qam_params_init(&session->qam, session->tuner);
	
	printf("Enter modulation (64, 256 or 1024): ");fflush(stdout);
	fgets(buf, BUFSIZE, stdin);
	session->qam.mode = atoi(buf);

	printf("Enter frequency (in Hz or MHz): ");fflush(stdout);
	fgets(buf, BUFSIZE, stdin);
	session->freq = atoi(buf);
	if (session->freq < 20000)
		session->freq *= 1000000;
	
	if (session->qam.mode != 64) {
		printf("Enter symbol rate (in Baud or MBaud): ");fflush(stdout);
		fgets(buf, BUFSIZE, stdin);
		session->qam.symbol_rate = atoi(buf);
		if (session->qam.symbol_rate < 1000)
			session->qam.symbol_rate *= 1000000;
	}	
}

void config_vsb(struct session *session)
{
	session->type = btuner_type_vsb;
	btuner_vsb_params_init(&session->vsb, session->tuner);
	
	printf("Enter modulation (8 or 16): ");fflush(stdout);
	fgets(buf, BUFSIZE, stdin);
	session->vsb.mode = atoi(buf);

	printf("Enter frequency (in Hz or MHz): ");fflush(stdout);
	fgets(buf, BUFSIZE, stdin);
	session->freq = atoi(buf);
	if (session->freq < 20000)
		session->freq *= 1000000;
}

int main(void) {
	int i;
	bresult rc;
	
	printf(
	"allpassrec\n"
	"(c)2005-2007 Broadcom Corp.\n"
	"\n"
	"Bringing up system...\n"
	"\n"
	);
		
	rc = bsettop_init(BSETTOP_VERSION);
	if (rc) exit(1);
	
	for (total=0;total<MAXSESSIONS;total++) {
		bstream_mpeg mpeg;
		int tuner;
		char *filename;
		struct session *session = &g_sessions[total];
		
		printf(
		"\n"
		"If you do not want tuner enter -1 \n"
		"Enter tuner number, ENTER to start, or 'q' to quit: "
		);
		fflush(stdout);
		fgets(buf, BUFSIZE, stdin);
		
		if (buf[0] == 0 || buf[0] == '\n')
			break;
		if (buf[0] == 'q') {
			printf("Quit.\n");
			exit(0);
		}
		
		tuner = atoi(buf);
		if (tuner != -1 ) {       
			session->tuner = btuner_open(B_ID(tuner));
			if (!session->tuner) {
				printf("Unable to open tuner %d\n", tuner);
				exit(1);
			}
		} 
		
		session->recpump = brecpump_open(B_ID(total), NULL);
		if (!session->recpump) {
			printf("Unable to open recpump %d\n", total);
			exit(1);
		}
		session->record = brecord_open();
		if (!session->recpump) {
			printf("Unable to open record %d\n", total);
			exit(1);
		}
		
		brecord_params_init(&session->record_params, session->record);
		
		printf("Enter tuner type ('sds', 'qam', 'vsb' or 'streamer' ): ");fflush(stdout);
		fgets(buf, BUFSIZE, stdin);
		chop(buf);
		if (!strcmp(buf, "sds")) {
			config_sds(session);
		}
		else if (!strcmp(buf, "qam")) {
			config_qam(session);
		}
		else if (!strcmp(buf, "vsb")) {
			config_vsb(session);
		}
		else if (!strcmp(buf, "streamer")) {
			session->type = btuner_streamer;
		}
		else {
			printf("Unrecognized tuner type\n");
			exit(1);
		}
		
		printf("Enter capture filename: ");fflush(stdout);
		fgets(buf, BUFSIZE, stdin);
		chop(buf);
		filename = strdup(buf);
	
		printf("Opening file %s\n", filename);		
		session->file = brecord_file_open(filename, NULL);
		if (!session->file) {
			printf("Unable to open files.\n");
			exit(1);
		}
		/* free the strdup's */
		free(filename);
		
		switch (session->type) {
		case btuner_type_sds:
			session->band = btuner_tune_sds(session->tuner, session->freq, &session->sds);
			break;
		case btuner_type_qam:
			session->band = btuner_tune_qam(session->tuner, session->freq, &session->qam);
			break;
		case btuner_type_vsb:
			session->band = btuner_tune_vsb(session->tuner, session->freq, &session->vsb);
			break;
		case btuner_streamer:
			session->band = bstreamer_attach( B_ID(0), bstream_mpeg_type_ts );
			break;
		default: 
			printf("unknown type\n");
			exit(1);
		}
			
		if (!session->band) {
			printf("Unable to tune\n");
			exit(1);
		}
		
		/* init only will result in all pass */
		bstream_mpeg_init(&mpeg);
		session->stream = bstream_open(session->band, &mpeg);
		if (!session->stream) {
			printf("Unable to create stream\n");
			exit(1);
		}
	}
	
	for (i=0;i<total;i++) {
		printf("Starting record %d...\n", i);
		rc = brecord_start(g_sessions[i].record, g_sessions[i].recpump, g_sessions[i].stream, g_sessions[i].file, &g_sessions[i].record_params);
		if (rc) {
			printf("Unable to start record %d\n", i);
			exit(1);
		}
	}
	
	if (total) {	
		sleep(3); /* let the messages clear */
		printf("\n\nRecording started. Press enter to stop...\n");
		fgets(buf, BUFSIZE, stdin);
	}
	
	for (i=0;i<total;i++) {
		printf("Stopping record %d...\n", i);
		brecord_stop(g_sessions[i].record);
		bstream_close(g_sessions[i].stream);
		brecord_file_close(g_sessions[i].file);
		brecord_close(g_sessions[i].record);
		brecpump_close(g_sessions[i].recpump);
		if (g_sessions[i].type != btuner_streamer) {
			btuner_close(g_sessions[i].tuner);
		}
	}
	bsettop_uninit();
	
	printf("Done\n");
	return 0;
}
