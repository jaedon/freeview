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
 * $brcm_Workfile: createindex_withgap.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 10:07a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/tests/createindex_withgap.c $
 * 
 * 2   9/7/12 10:07a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>		/* strcmp() */
#include <sys/stat.h>
#if !defined(__vxworks)
#include <sys/timeb.h>	/* ftime() */
#endif
#include "tsindexer.h"
#include "bcmindexer.h"
#include "brcm_dbg.h"

#define NUM_STREAM_BYTES_TO_READ	(188*1024)
#define NUM_SC_BYTES_TO_READ		(sizeof(sIndexEntry)*1024)

void TimetoString( long ctime, char *outStr )
{
	long	hh,mm,ss;
	char 	*p;
	
	ss = ctime % 60;
	mm = (ctime/60) % 60;
	hh = (ctime/3600) % 24;

	p = outStr;	
	*p++ = (hh/10) + 0x30;
	*p++ = (hh%10) + 0x30;
	*p++ = ':';
	*p++ = (mm/10) + 0x30;
	*p++ = (mm%10) + 0x30;
	*p++ = ':';
	*p++ = (ss/10) + 0x30;
	*p++ = (ss%10) + 0x30;
	*p++ = 0;
}

double getms( void )
{
#if defined(__vxworks)
	return 0;
#else
	double  usec1;
	struct timeb t;

	ftime( &t );

	usec1 = t.time + t.millitm/1000.0;
	return usec1;
#endif
}

void OffsetToString( long offset, char *outStr )
{
	long mb, kb, b;

	b = offset % 1000;
	kb = (offset/1000)%1000;
	mb = (offset/(1000*1000)) % (1000*1000);

	sprintf( outStr, "%ld,%03ld,%03ld", mb, kb, b );
}

/**
* Feed from tsindexer to bcmindexer.
**/
static unsigned long write_to_bcmindexer( const void *p_bfr, unsigned long entrySize,
	unsigned long numEntries, void *fp )
{
	return
		BNAV_Indexer_Feed(
			(BNAV_Indexer_Handle)fp,
			(BSCT_Entry *)p_bfr,
			(numEntries * entrySize) / sizeof(BSCT_Entry))
		* sizeof(BSCT_Entry) / entrySize;
}

void printHeader()
{
	printf("\ncreateindex, built on %s\n\n", __DATE__);
	printf("Broadcom Corp. Confidential\n");
	printf("Copyright 1998-2003 Broadcom Corp. All Rights Reserved.\n\n");
}

/**
* This is a modification of createindex which allows
* the stuff of gigabytes of bogus MPEG into the
* index in order to test wrap-detection.
*
* This was needed to test PR7218.
**/
#define FAKE_GAP

#ifdef FAKE_GAP
off_t fake_data_sent = 0;
#endif

static FILE *g_mpegFile = NULL; // global hack
int mpegSizeCallback(BNAV_Indexer_Handle handle, unsigned long *hi, unsigned long *lo)
{
	off_t o = ftello(g_mpegFile);
	if (o == -1)
		return -1;

#ifdef FAKE_GAP
	o += fake_data_sent;
#endif
	*hi = o >> 32;
	*lo = o & 0xFFFFFFFF;
	return 0;
}

/**
* This is the entry point for VxWorks.
*
* input: 0=ts,1=pes,2=sct
* output: 0=sct,1=bcm
**/
int createindex(const char *inputfile, const char *outputfile, int pid,
	int input, int output, BNAV_Version navVersion)
{
	unsigned char *bfr;
	FILE	*fin, *fout;
	sTsIndexer *indexer = NULL;
	BNAV_Indexer_Handle bcmindexer = NULL;
	unsigned long readBytes;
	unsigned long totalBytes = 0;
	double	time1, time2;
	int pesId = 0;
	int generateBcmIndex = 0;
	int sctInput = 0;
	unsigned long numBytesToRead;

	printHeader();

	time1 = getms();

	if (input == 1)
		pesId = pid;
	else if (input == 2)
		sctInput = 1;
	if (sctInput)
		numBytesToRead = NUM_SC_BYTES_TO_READ;
	else
		numBytesToRead = NUM_STREAM_BYTES_TO_READ;
	if (output == 1)
		generateBcmIndex = 1;

	if (pid == 0 && !sctInput)
		printf(
		"WARNING\n"
		"WARNING\n"
		"WARNING: Are you sure you want PID == 0 ??????????????\n"
		"WARNING\n"
		"WARNING\n"
		"\n"
		);
	if (sctInput && !generateBcmIndex) {
		printf("If your input is a start code index, then -bcm is required.\n");
		exit(1);
	}

	bfr = malloc( numBytesToRead );

	if (!strcmp(inputfile, "-")) {
		fin = stdin;
		inputfile = "stdin";
	}
	else if( (fin = fopen(inputfile, "rb" )) == NULL )
	{
		printf("Unable to open input file %s\n", inputfile);
		exit(255);
	}

	if (!strcmp(outputfile, "-")) {
		fout = stdout;
		outputfile = "stdout";
	}
	else if( (fout = fopen(outputfile, "wb" )) == NULL )
	{
		printf("Unable to open output file %s\n", outputfile);
		fclose(fin);
		exit(255);
	}

	printf("Processing %s 0x%X from %s input file '%s'.\n",
		pesId?"StreamID":"PID",
		pesId?pesId:pid,
		pesId?"MPEG PES":sctInput?"SCT":"MPEG TS",
		inputfile);
	if (generateBcmIndex)
		printf("Writing BCM index (ver %d) to '%s'.\n", navVersion, outputfile);
	else
		printf("Writing SCT index to '%s'.\n", outputfile);

	if (generateBcmIndex)
	{
		BNAV_Indexer_Settings settings;
		BNAV_Indexer_GetDefaultSettings(&settings);
		settings.writeCallback = (INDEX_WRITE_CB)fwrite;
		settings.filePointer = (void *)fout;
		settings.navVersion = navVersion;
		if (!sctInput) {
			settings.mpegSizeCallback = mpegSizeCallback;
			g_mpegFile = fin;
		}
		if (BNAV_Indexer_Open(&bcmindexer, &settings))
		{
			exit(1);
		}
	}
	if (!sctInput)
	{
		if (generateBcmIndex)
			/* pass through to bcmindex */
			indexer = tsindex_allocate( (INDEX_WRITE_CB)write_to_bcmindexer, (void *)bcmindexer, (unsigned short)pid, -1 );
		else
			indexer = tsindex_allocate( (INDEX_WRITE_CB)fwrite, (void *)fout, (unsigned short)pid, -1 );
		if( pesId )
			tsindex_setPesId( indexer, (unsigned char)pesId );
	}

	while( (readBytes = fread( bfr, 1, numBytesToRead, fin )) != 0 )
	{
		totalBytes += readBytes;

		if ( totalBytes%(1*numBytesToRead) == 0 )
		{
			double rate1; char timeStr[32], offsetStr[32];
			time2 = getms() - time1;
			rate1 = (totalBytes/1024)/time2;
			TimetoString( (long)time2, timeStr );
			OffsetToString( totalBytes, offsetStr );
			printf( "Byte Offset: %s Rate: %0.1fK/s, Elapsed Time: %s      \r", offsetStr, rate1, timeStr );
		}

		if (sctInput)
		{
			BNAV_Indexer_Feed(bcmindexer, (BSCT_Entry*)bfr, readBytes/sizeof(BSCT_Entry) );
		}
		else if( pesId )
		{
			tsindex_feedPes( indexer, bfr, readBytes );
		}
		else
		{
			tsindex_feed( indexer, bfr, readBytes );
		}

#ifdef FAKE_GAP
#define START_OF_JUNK (10 * 1024 * 1024)
#define AMOUNT_OF_JUNK1 ((unsigned long)1024 * 1024 * 1024)
#define AMOUNT_OF_JUNK2 16
		{
		static int doonce = 0;
		if (sctInput || pesId) {
			BRCM_DBG_ERR(("FAKE_GAP must use TS in"));
			exit(1);
		}
		if (!doonce && totalBytes >= START_OF_JUNK) {
			int i,j;
			unsigned char junkbuf[188];

			doonce = 1;
			junkbuf[0] = 0x47; // just enough to fake it out
			printf("\nFeeding junk\n");
			for (j=0;j<AMOUNT_OF_JUNK2;j++)
			for (i=0;i<AMOUNT_OF_JUNK1;i+=188) {
				tsindex_feed(indexer, junkbuf, 188);
				fake_data_sent += 188;
			}
		}
		}
#endif
	}

	if (bcmindexer)
		BNAV_Indexer_Close( bcmindexer );
	if (indexer)
		tsindex_free( indexer );

	fclose( fin );
	fclose( fout );
	free( bfr );
	printf("\n");
	exit(0);
}

#if !defined(__vxworks)
void printUsage()
{
	printf(
	"Parses an MPEG stream or SCT index and creates either a SCT index\n"
	"  or Broadcom index.\n"
	"\n"
	"Usage: createindex inputfile outputfile [pid] [-ts|-pes|-sct] [-bcm [-bcmver VERSION]]\n"
	"\n"
	"  inputfile    Filename of input mpeg (either TS or PES stream), or\n"
	"                 input startcode index (if -sct is specified or ends with\n"
	"                 .sct or .idx)\n"
	"  outputfile   Filename of output index file that will be created.\n"
	"                 If -bcm option is used, or file ends with .nav, it will be\n"
	"                 a NAV table, otherwise a start code index.\n"
	"  pid          Pid (or stream id in PES mode). Not needed if inputfile is\n"
	"                 a startcode index. Prefix with 0x for hex, otherwise decimal.\n"
	"\n"
	"Optional Parameters:\n"
	"  -ts          Inputfile is a TS stream\n"
	"  -pes         Inputfile is a PES stream\n"
	"  -sct         Inputfile is a startcode index\n"
	"  -bcm         Generate a BCM index file from either\n"
	"                 a TS stream or startcode index.\n"
	"  -bcmver VERSION   Specify the BCM index format. See eBcmNavVersion.\n"
	);
}

int main( int argc, char **argv )
{
	brcmDbgSetOutputLevel(kBrcmDbgOutputWrn);

	if (argc < 3) {
		printHeader();
		printUsage();
		return 1;
	}
	else {
		int pid = 0;
		int nextparam = 3;
		int input = -1;
		int output = -1;
		const char *source = argv[1];
		const char *dest = argv[2];
		BNAV_Version navVersion = BNAV_VersionLatest;

		if (argc>3 && argv[3][0] != '-') {
			if (strstr(argv[3], "0x") == argv[3])
				sscanf( argv[3], "0x%X", &pid );
			else
				pid = atoi( argv[3] );
			++nextparam;
		}

		while (nextparam < argc) {
			if (!strcmp(argv[nextparam], "-ts"))
				input = 0;
			else if (!strcmp(argv[nextparam], "-pes"))
				input = 1;
			else if (!strcmp(argv[nextparam], "-sct"))
				input = 2;
			else if (!strcmp(argv[nextparam], "-bcm"))
				output = 1;
			else if (!strcmp(argv[nextparam], "-bcmver") && nextparam+1 < argc)
				navVersion = atoi(argv[++nextparam]);
			nextparam++;
		}

		if (input == -1) {
			char *s;
			// check the suffix of the inputfile
			if (((s = strstr(source, ".sct")) && !s[4]) ||
				((s = strstr(source, ".idx")) && !s[4]))
			{
				input = 2;
			}
			else if ((s = strstr(source, ".pes")) && !s[4])
			{
				input = 1;
			}
			else
				input = 0;
		}
		if (output == -1) {
			char *s;
			if (((s = strstr(dest, ".nav")) && !s[4]) ||
				((s = strstr(dest, ".bcm")) && !s[4]))
				output = 1;
			else
				output = 0;
		}

		return createindex(source,dest,pid,input,output,navVersion);
	}
}
#endif

