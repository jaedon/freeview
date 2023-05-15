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
 * $brcm_Workfile: tsdma_example.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/tsdma_example.c $
 * 
 * 5   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bstd.h"
#include "bsettop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "bkni.h"
#include "bkni_multi.h"

#include <assert.h>
#include <sys/stat.h>

BDBG_MODULE(rectest);

struct context {
        FILE		*disk_data;
        FILE		*disk_index;
        FILE		*playDiskHdl;
	int		OflowCnt;
	int		tsdmaBufSize;
	bdecode_t	decode;
	bplaypump_t	playTsdma,playDisk;
        brecpump_t pump, disk;
	BKNI_EventHandle playTsdma_event,tsdma_event, playDisk_event, disk_data_event, disk_index_event;
} g_context;

bool g_stopped = false;

void tsdmaPlaypumpCallback(void *context_);
void *tsdma_func(void *context);
void playFromDisk_read_callback(void *context);
void *playFromDisk_func(void *context_);

void tsdmaRecpumpCallback(void *context_);
void tsdmaOverflowCallback(void *context_);

/* This example shows how to decrypt a MCARD 3DES stream on BCM7038 platforms.
   It is needed because the 7038 does not have hardware decryption of a live
   stream, but does have hardware decryption for a stream fed through the 
   playback channel.
   Therefore to utilize this playback decription this example routes the stream
   through the TSDMA engine, out to playback where it is decrypted and then 
   back to a record channel for recording to disk.

   -------------     ---------     -----------      ---------
   | STREAM IN | --> | TSDMA | --> | PLAYBACK | --> | RECORD |
   -------------     ---------     -----------      ---------
                                                        |
                                                       \./
                     -----------     ------------     --------
                     | DECODER | <-- | PLAYBACK | <-- | DISK |
                     -----------     ------------     --------

*/

enum input_source {

        useQAM,
        useVSB,
        useStreamer
};

typedef struct tag_userSettings {
        enum input_source       streamSource;
        bool                    useEncryption;
	bool			liveOnly;	/* Play stream direct out to MVD */
	bool			noLive;		/* Don't direct stream to MVD */	
        unsigned long           key[6];
	bstream_mpeg		tsdma;		/* specify mpeg params of tsdma stream */
	bstream_mpeg		tsPb;		/* specify mpeg params of tsPb stream */
	bstream_mpeg		toDisk;		/* Specify mepg params of stream to / from disk */
	int			tsdmaBufSize;
	int			tsdmaDescriptors;	/* use if want to tweak system */
	int			defConfig;		/* which of predefined configs to use */
	int			qamFreq;		/* freq of qam channel */
}userSettings;

int setDefaultUserSettings( userSettings *usr ) {

	bstream_mpeg_init( &usr->tsPb );
	bstream_mpeg_init( &usr->toDisk );
	bstream_mpeg_init( &usr->tsdma );

	usr->streamSource	= useStreamer;
	usr->qamFreq 		= 0;
	usr->useEncryption	= false;
	usr->liveOnly		= true;
	usr->noLive		= false;

	usr->key[0]		= 0;
	usr->key[1]		= 0;
	usr->key[2]		= 0;
	usr->key[3]		= 0;
	usr->key[4]		= 0;
	usr->key[5]		= 0;

	usr->toDisk.video[0].pid = 0;
	usr->toDisk.audio[0].pid = 0;

	usr->tsdma.video[0].pid = 0;
	usr->tsdma.audio[0].pid = 0;

	usr->tsdmaBufSize	= 188 * 1024;
	usr->tsdmaDescriptors	= 0;		/* let code decide */ 

	return 0;
}

/*
  Init settings to decode a 3DES stream fed into the 
  97038 via the streamer input 
*/
uint8_t g_key[] = {0x4e, 0x61, 0xbc, 0x00, 0xa8, 0x7f, 0x39, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int setCliffordUserSettings( userSettings *usr ) {

	usr->streamSource	= useStreamer;
	usr->useEncryption	= true;
	usr->liveOnly		= false;
	usr->noLive		= true;

	bstream_mpeg_init( &usr->tsPb );
	bstream_mpeg_init( &usr->toDisk );
	bstream_mpeg_init( &usr->tsdma );

	usr->toDisk.video[0].pid = 0x51;
	usr->toDisk.audio[0].pid = 0x54;
	usr->toDisk.audio[0].format = baudio_format_ac3;
	usr->toDisk.pcr_pid	 = 0x54;

	usr->tsPb.video[0].pid = 0x51;
	usr->tsPb.audio[0].pid = 0x54;
	usr->tsPb.audio[0].format = baudio_format_ac3;
	usr->tsPb.pcr_pid	= 0x54;

        usr->tsPb.encryption.type = bencryption_type_3des;
        usr->tsPb.encryption.key_length = 128;
        BKNI_Memcpy(usr->tsPb.encryption.key, g_key, sizeof(g_key));

	usr->tsdma.video[0].pid = 0x51;
	usr->tsdma.audio[0].pid = 0x54;
	usr->tsdma.pcr_pid	= 0x51;

	usr->tsdmaBufSize	= 188 * 1024;
	usr->tsdmaDescriptors	= 0;		/* let code decide */ 

	return 0;
}

void prompt(const char *msg) {
	char c=0;
	while ( 1 ) {
		if( c != 10 ) printf("\n%s \n", msg );
		c = getchar();
		if( c == 'q' ) break;
	}
}

/* Func to carry out all initialization for tsdma recpump */
void initTsdmaRecpump( bstream_mpeg *tsdma_mpeg , userSettings *usr, bplaypump_status *status ) {

        brecpump_open_params 	recpump_open_params;


        brecpump_get_open_params( bconfig->resources.record_tsdma.objects[0] , &recpump_open_params);

        recpump_open_params.data.num_descriptors = usr->tsdmaDescriptors; /* Set descriptors */
        recpump_open_params.data.buffer_size	= usr->tsdmaBufSize;     /* Max TS_DMA size */
	recpump_open_params.data.buf = status->buffer_base;

        g_context.pump = brecpump_open( bconfig->resources.record_tsdma.objects[0] , &recpump_open_params );

        bstream_mpeg_init( &(*tsdma_mpeg) );

	if( usr->tsdma.video[0].pid ) {
		printf( "\nTsdma will only pass thru pids : 0x%2x 0x%2x \n", 
								usr->tsdma.video[0].pid,
								usr->tsdma.audio[0].pid );

	        tsdma_mpeg->video[0].pid = usr->tsdma.video[0].pid;
        	tsdma_mpeg->audio[0].pid = usr->tsdma.audio[0].pid;
        	tsdma_mpeg->audio[0].format = usr->tsdma.audio[0].format;
		tsdma_mpeg->pcr_pid	 = usr->tsdma.pcr_pid;

	}
	else {
		/* Pass ALL PIDS in the stream through TSDMA ! 	*/
		/* Warning if your tsdma buffer is not big      */
		/* enough, you	will quickly get an overflow    */
		/* condition 					*/
		printf( "\nTsdma is doing ALLPASS pids\n" );
	        tsdma_mpeg->video[0].pid = 0x0;
        	tsdma_mpeg->audio[0].pid = 0x0;	
	}

}

/* Func to carry out all initialization for disk recpump */

void initDiskRecpump( bstream_mpeg *disk_mpeg, userSettings *usr ) {

        g_context.disk = brecpump_open(B_ID(0), NULL );

        bstream_mpeg_init( &(*disk_mpeg) );

	disk_mpeg->video[0].pid = usr->toDisk.video[0].pid;
	disk_mpeg->audio[0].pid = usr->toDisk.audio[0].pid;
	disk_mpeg->audio[0].format = usr->toDisk.audio[0].format;
	disk_mpeg->pcr_pid = usr->toDisk.pcr_pid;

}

/*
  Init stuff to do with stream source - vsb , QAM or streamer
*/
int initStreamSource( bstream_mpeg *tsdma_mpeg,btuner_t tuner,bstream_t *enc_stream,userSettings *usr ) {

        bband_t 		band = NULL;
        btuner_qam_params	qam;
	int			rtnval = 0;

	if( usr->streamSource == useVSB ) {
		/* Not done yet */
	}
	else if ( usr->streamSource == useQAM ) {
	        btuner_qam_params_init(&qam, tuner);
        	qam.symbol_rate = 5056900;
	        qam.mode = 64;

		if( usr->toDisk.video[0].pid ) {
			/* set pid we want to pass through */
		}
			printf( "Tuning to %d\n" , usr->qamFreq );
		if( usr->qamFreq ) {

	        	band = btuner_tune_qam(tuner, usr->qamFreq, &qam);
		}
		else {
	        	band = btuner_tune_qam(tuner, 450000000, &qam);
		}
	}
	else {	/* Streamer */
	        band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
	}

        if (!band) {
		rtnval = 1;
		goto error;
	}

	printf( "\nOpening stream for pids : 0x%2x 0x%2x \n", 
								(*tsdma_mpeg).video[0].pid,
								(*tsdma_mpeg).audio[0].pid );
        *enc_stream = bstream_open(band, &(*tsdma_mpeg) );
        if (!enc_stream) {
		rtnval = 1;
		goto error;
	}
error:
	return rtnval;
}

void setupMpegStructForPlayback( bstream_mpeg *mpeg, userSettings *usr ) {

	switch( usr->streamSource ) {
		case useVSB:
		case useQAM:
		case useStreamer:
			printf( "copying mpeg settings for streamer \n" );
			*mpeg = usr->tsPb;
		break;

		default:
			printf("Unknown !\n" );
		break;
	}	

}
/*
Callback to be called by recpump code when it has Mpeg data 
available that may be written to disk.
*/
void writeMpegDataToDisk_callback(void *context_)
{
        struct context *context = (struct context *)context_;
        BKNI_SetEvent(context->disk_data_event);
}

/*
Callback to be called by recpump code when it has Mpeg index data 
available that may be written to disk.
*/
void writeMpegIndexToDisk_callback(void *context_)
{
        struct context *context = (struct context *)context_;
        BKNI_SetEvent(context->disk_index_event);
}

/* 
Function to write Mpeg data to disk file 
*/
void *writeMpegDataToDisk(void *context_)
{
        struct context *context = (struct context *)context_;

	/* Wait for 1st event before calling brecpump code, avoids timing issues */
        BKNI_WaitForEvent(context->disk_data_event, BKNI_INFINITE );
        while (1) {
                const void *buffer;
                size_t buffer_size;
                int n;
                if (brecpump_data_get_buffer(context->disk, &buffer, &buffer_size) || g_stopped )
                        break;
                if (!buffer_size) {
                        BKNI_WaitForEvent(context->disk_data_event, BKNI_INFINITE);
                        continue;
                }

                n = fwrite(buffer, 1, buffer_size, context->disk_data);
                if (n < 0 || brecpump_data_write_complete(context->disk, n))
                        break;
        }
        return NULL;
}

/* 
Function to write Index of Mpeg data to disk file 
*/
void *writeMpegIndexToDisk(void *context_)
{
        struct context *context = (struct context *)context_;

	/* Wait for 1st event before calling brecpump code, avoids timing issues */
        BKNI_WaitForEvent(context->disk_index_event, BKNI_INFINITE );
        while (1) {
                const void *buffer;
                size_t buffer_size;
                int n;
                if (brecpump_index_get_buffer(context->disk, &buffer, &buffer_size) || g_stopped )
                        break;
                if (!buffer_size) {
                        BKNI_WaitForEvent(context->disk_index_event, BKNI_INFINITE);
                        continue;
                }

                n = fwrite(buffer, 1, buffer_size, context->disk_index);
                if (n < 0 || brecpump_index_write_complete(context->disk, n))
                        break;
        }
        return NULL;
}

void printUsage( void ) {
	printf( "tsdma_example : \n" );
	printf( "\tProgram to show how to use tsdma engine to feed a 3DES encrypted stream\n" );
	printf( "\tinto the playback engine for decoding, before feeding back through the \n" );
	printf( "\trecord channel where it can be recorded with an index\n" );
	prompt("press enter to exit");
}

int
parseArgs( const char *argv[], int argc, userSettings *usrSettings ) {

        int i,keyNr;
        printf( "argc is %d\n" , argc );
	argc--;

        for( i=0; i < argc+1 ; i++ ) {
                printf( "arg %d is %s\n" , i, argv[i] );

		if( !strcmp("-h", argv[i] ) || !strcmp("-help", argv[i] ) ) {
			printUsage();
			goto help_exit;
		}
        }   

	return 0;

help_exit:
	return 1;
}


int main(int argc, const char *argv[]) {
        bresult                 rc;
        bdisplay_t              display;
        bdecode_window_t        window;
        bplaypump_params        playpump_params;
	bplaypump_status	play_status;		/* use to get playpumps buffer */
        bstream_mpeg            mpeg,playDiskMpeg;
        bstream_t               play_stream, enc_stream, playDiskStream;
        pthread_t               tsdmaThread,playDiskThread;
        pthread_t               disk_data_thread, disk_index_thread;

        btuner_t tuner;
        bstream_mpeg            tsdma_mpeg,     /* for setting tsdma params */
                                disk_mpeg;      /* for setting disk params  */

        brecpump_params         rec_params,     /* for tsdma channel    */
                                disk_params;    /* for recording to disk */

        bplaypump_open_params   playpump_open_params;
	userSettings		usrSettings;

	setDefaultUserSettings( &usrSettings );

	if( parseArgs( argv, argc, &usrSettings ) ) {
		return 0;
	}
	setCliffordUserSettings( &usrSettings );

        rc = bsettop_init(BSETTOP_VERSION);
        if (rc) goto error;

        g_context.decode = bdecode_open(B_ID(0));
        display 	= bdisplay_open(B_ID(0));
        window 		= bdecode_window_open(B_ID(0), display);
        tuner 		= btuner_open(B_ID(0));
	

        bplaypump_get_open_params(B_ID(0), &playpump_open_params);
        playpump_open_params.buffer_size	= usrSettings.tsdmaBufSize;   
	g_context.tsdmaBufSize 			= usrSettings.tsdmaBufSize;
	if( usrSettings.tsdmaDescriptors ) 
	        playpump_open_params.num_descriptors	= usrSettings.tsdmaDescriptors; 
	else
	        playpump_open_params.num_descriptors	= usrSettings.tsdmaBufSize / (4*1024); 

        playpump_open_params.alignment			= 12; 


	g_context.playTsdma = bplaypump_open(B_ID(0), &playpump_open_params );
	if (!g_context.playTsdma ) goto error;

	bplaypump_get_status( g_context.playTsdma, &play_status );	/* get buffer allocated by playpump */

	initTsdmaRecpump( &tsdma_mpeg, &usrSettings, &play_status );

	initDiskRecpump(  &disk_mpeg, &usrSettings );

	if( initStreamSource( &tsdma_mpeg, tuner, &enc_stream, &usrSettings ) ) goto error;

        BKNI_CreateEvent(&g_context.tsdma_event);
	BKNI_CreateEvent(&g_context.playTsdma_event);

        brecpump_params_init(&rec_params, g_context.pump);
        rec_params.callback_context	= &g_context;
        rec_params.data_write_callback	= tsdmaRecpumpCallback;
        rec_params.overflow		= tsdmaOverflowCallback;

	setupMpegStructForPlayback( &mpeg, &usrSettings );

	bplaypump_params_init(&playpump_params, g_context.playTsdma );

	playpump_params.read_callback		= tsdmaPlaypumpCallback;
	playpump_params.callback_context	= g_context.playTsdma_event;

	play_stream = bplaypump_start(g_context.playTsdma, &mpeg, &playpump_params);
	if (!play_stream) goto error;

       	g_context.disk_data	= fopen("videos/stream.mpg", "w+");
        assert(g_context.disk_data);
        g_context.disk_index	= fopen("videos/stream.sct", "w+");
       	assert(g_context.disk_index);

        BKNI_CreateEvent(&g_context.disk_data_event);
       	BKNI_CreateEvent(&g_context.disk_index_event);

        brecpump_params_init(&disk_params, g_context.disk);
        disk_params.callback_context		= &g_context;
        disk_params.data_write_callback		= writeMpegDataToDisk_callback;
        disk_params.index_write_callback	= writeMpegIndexToDisk_callback;

        pthread_create(&disk_data_thread, NULL, writeMpegDataToDisk, &g_context);
        pthread_create(&disk_index_thread, NULL, writeMpegIndexToDisk, &g_context);

       	BKNI_CreateEvent(&g_context.playDisk_event);
        g_context.playDisk = bplaypump_open(B_ID(1), NULL);

        bstream_mpeg_init(&playDiskMpeg);
	memcpy( &playDiskMpeg, &usrSettings.toDisk, sizeof(bstream_mpeg) );

        bplaypump_params_init(&playpump_params, g_context.playDisk);
       	playpump_params.read_callback = playFromDisk_read_callback;
        playpump_params.callback_context = g_context.playDisk_event;


	/* The smaller you make the TSDMA buffer, the more critical the starting */
	/* of the thread tsdma_func becomes, IF you want to avoid overflow on 	 */
	/* start up								 */
	if (pthread_create(&tsdmaThread, NULL, tsdma_func, &g_context))
		goto error;

       	playDiskStream = bplaypump_start(g_context.playDisk, &playDiskMpeg, &playpump_params);
        if (!playDiskStream) goto error;

        if (brecpump_start(g_context.disk, play_stream, &disk_params))
       	        goto error;

	if (bdecode_start( g_context.decode, playDiskStream, window))
		goto error;

        if (pthread_create(&playDiskThread, NULL, playFromDisk_func, &g_context))
       	        goto error;

        if (brecpump_start(g_context.pump, enc_stream, &rec_params))
                goto error;

	prompt("press 'q' to stop");

	/* Need this because can get in a deadlock condition where brecpump_get_buffer	*/
	/* keeps returning zero size, waits for an event, but never makes it to 	*/
	/* write complete								*/
	g_stopped = true;

	BDBG_MSG(( "Stopping play" ));
	bplaypump_stop(g_context.playDisk);
	BDBG_MSG(( "Set playDisk_event" ));
	BKNI_SetEvent(g_context.playDisk_event);

	BDBG_MSG(( "Joining playpump thread\n" ));
	pthread_join( playDiskThread, NULL);

	BDBG_MSG(( "Stopping disk\n" ));
	brecpump_stop(g_context.disk);

	BDBG_MSG(( "Set disk_data_event\n" ));
	BKNI_SetEvent(g_context.disk_data_event);
	BDBG_MSG(( "Set disk_index_event\n" ));
	BKNI_SetEvent(g_context.disk_index_event);

	BDBG_MSG(( "Stopping play" ));
	bplaypump_stop(g_context.playTsdma);
	BDBG_MSG(( "Stopping pump" ));
	brecpump_stop(g_context.pump);

	BDBG_MSG(( "Set tsdma_event\n" ));
	BKNI_SetEvent(g_context.tsdma_event);
	BDBG_MSG(( "Set play_event\n" ));
	BKNI_SetEvent(g_context.playTsdma_event);

	BDBG_MSG(( "Joining play tsdmaThread\n" ));
	pthread_join( tsdmaThread, NULL);

	BDBG_MSG(( "Joining disk_data_thread\n" ));
	pthread_join(disk_data_thread, NULL);
	BDBG_MSG(( "Joining disk_index_thread\n" ));
	pthread_join(disk_index_thread, NULL);


	fclose( g_context.disk_index );
	fclose( g_context.disk_data );

	return 0;
error:
	return 1;
}

/* Callback responsible for signalling that "playpump" has pulled
   the data out of the shared TSDMA buffer , and so now that area
   of memory can be freed for reuse.
*/
void tsdmaPlaypumpCallback(void *context)
{

	BKNI_SetEvent((BKNI_EventHandle)context);
}

/* 
   Callback to signal that TSDMA engine has transfeered data into
   the shared TSDMA buffer and so it is now available for "playpump"
   to pull the data out.
*/
void tsdmaRecpumpCallback(void *_context)
{
	struct context *context = (struct context *)_context;

	BKNI_SetEvent((BKNI_EventHandle)context->tsdma_event);
}

/*
  Callback to signal that the TSDMA buffer has overflowed,
  probably due to playpump not pulling the data out fast enough
*/
void tsdmaOverflowCallback(void *_context)
{
	struct context *context = (struct context *)_context;
        BDBG_WRN(( "\nOVERFLOW  event\n" ));
	context->OflowCnt++;
}

void printStats( long loop_cnt , brecpump_t recpump ) {

	static uint32_t	wrap_cnt=0,last_bytes = 0;
	uint64_t	bytesRecorded = 0;
	brecpump_status	rec_status;

	if( loop_cnt %120 == 0 ) {
		brecpump_get_status( recpump, &rec_status);
		if( rec_status.mpeg_bytes_recorded < last_bytes ) {
			wrap_cnt++;
		}
		bytesRecorded = ((uint64_t)wrap_cnt<<32) + rec_status.mpeg_bytes_recorded;	

	        printf( "Bytes recorded %llu \t wrap_cnt %u \n", bytesRecorded, wrap_cnt  );	
	        printf( "fifo depth	%lu \n", rec_status.mpeg_fifo_depth );	
	        printf( "fifo size      %lu \n", rec_status.mpeg_fifo_size );	
		last_bytes = rec_status.mpeg_bytes_recorded;
	}


}

int timedOutDecoderFlush( struct context *_context , size_t play_buffer_size ) {

	struct context *context = (struct context *)_context;
	size_t rec_buffer_size;
	const void *rec_buffer;
	int		rtnval = 1;

	BDBG_WRN(( "\n\nplay_buf_size is %d " , play_buffer_size ));
	if (bplaypump_read_complete(context->playTsdma, 0, play_buffer_size ))
		goto error;

	if (brecpump_data_get_buffer(context->pump, &rec_buffer, &rec_buffer_size))
                goto error;
	BDBG_WRN(( "left over recpump data is %d " , rec_buffer_size ));

        if( brecpump_data_write_complete(context->pump, rec_buffer_size ) )
	        goto error;

	BDBG_WRN(( "\n\nFlushing Decoder \n\n" ));
	bplaypump_flush(context->playTsdma);
	rtnval = 0;

error:
	return rtnval;
}

/* Main worker function to sync up the processes of TSDMA DMA'ing data
   into the shared buffer, and for playpump pulling it out of the
   share buffer.
*/
void *tsdma_func(void *context_)
{
	struct context *context = (struct context *)context_;
        BERR_Code err = BERR_SUCCESS;
	uint32_t	timeout_val = BKNI_INFINITE;	/* wait forever for receiving stream for first time */
	const void *rec_buffer, *play_buffer;
	size_t max_playpump_buf_size, recpump_buf_size, check_buf_size, usable_ppump_buf_size;

        printf("\nEntered tsdma_func\n" );
        err = BKNI_WaitForEvent(context->tsdma_event, timeout_val );
	BKNI_WaitForEvent(context->playTsdma_event, 1 );	/* clear out play_event if set */
        printf("\nCleared play event \n" );

	context->OflowCnt = 0;
	while (1) 
	{
		if( context->OflowCnt ) {
			/* need critical section */
			BDBG_WRN(("Detected Overflow: cnt is %d", context->OflowCnt ));
			context->OflowCnt = 0;
			if (bplaypump_get_buffer(context->playTsdma, (void *)&play_buffer, &max_playpump_buf_size)) {
				BDBG_WRN(("bplaypump_get_buffer failed. Couldn't get a free buffer "));
				break;
			}
			timedOutDecoderFlush( &(*context) , max_playpump_buf_size );
		}
		/* Get a buffer full of data from tsdma engine */
                if (brecpump_data_get_buffer(context->pump, &rec_buffer, &recpump_buf_size) || g_stopped )
                        break;

                if (!recpump_buf_size || recpump_buf_size < 4096 ) {
                        err = BKNI_WaitForEvent(context->tsdma_event, timeout_val );
                        if( err != BERR_SUCCESS ) {
                                if( err == BERR_TIMEOUT ) {
					if( timedOutDecoderFlush( &(*context) , max_playpump_buf_size ) ) break;

					timeout_val = BKNI_INFINITE;	/* wait forever until new data comes in */
				}
				else {
                                        BDBG_ERR(("Stg really bad happened !"));
                                        break;
				}
			}
			else {
					timeout_val = 100;
			}
                        continue;
                }

		if (bplaypump_get_buffer(context->playTsdma, (void *)&play_buffer, &max_playpump_buf_size)) {
			printf("bplaypump_get_buffer failed. Couldn't get a free buffer \n");
			break;
		}
		if (!max_playpump_buf_size ) {

			printStats( 120 , context->pump );	/* print buffer status when no buffer space */
			BKNI_WaitForEvent(context->playTsdma_event, BKNI_INFINITE);
			continue;
		}
		BDBG_MSG(("ply_buf has %d free bytes", max_playpump_buf_size ));

		/* The first call to get_buffer will return the entire playback buffer.
		If we use it, we're going to have to wait until the descriptor is complete,
		and then we're going to underflow. So use a max size. */

		usable_ppump_buf_size = max_playpump_buf_size;
		if ( usable_ppump_buf_size > recpump_buf_size ) {
			usable_ppump_buf_size = recpump_buf_size;
		}

		if( usable_ppump_buf_size > 4096 ) {
			usable_ppump_buf_size = 4096;
		}

		if (bplaypump_read_complete(context->playTsdma, 0, usable_ppump_buf_size )) {
			BDBG_WRN(("bplaypump_read_complete failed"));
			BDBG_WRN(("stopping tsdma playback "));
			break;
		}

		if (bplaypump_get_buffer(context->playTsdma, (void *)&play_buffer, &check_buf_size)) {
			BDBG_WRN(("bplaypump_get_buffer failed. Couldn't get a free buffer Rtn size %d\n", check_buf_size));
			break;
		}
		if (!check_buf_size ) {
			bdecode_status cur;
			BDBG_WRN(( "No free descriptors ! max_ppump_buf_sz %d rpump_buf_sz %d usable %d check %d",
				 max_playpump_buf_size, recpump_buf_size, usable_ppump_buf_size, check_buf_size ));

			bdecode_get_status( context->decode, &cur );
			BDBG_WRN(( "rec_buf rptr 0x%8x wptr 0x%8x diff %d ",
				 (void *)rec_buffer, (int)(void *)rec_buffer + recpump_buf_size, recpump_buf_size ));
			BDBG_WRN(( "ply_buf rptr 0x%8x wptr 0x%8x diff %d ", 
			(void *)play_buffer, (int)(void *)play_buffer + max_playpump_buf_size, max_playpump_buf_size ));
			BDBG_WRN(("decode video fifo size %d depth %d\n", cur.video_fifo_size, cur.video_fifo_depth ));
		}
		err = BKNI_WaitForEvent(context->playTsdma_event, 105 );	/* Wait max of 105 msec */
	        if( err != BERR_SUCCESS ) {
	            if( err == BERR_TIMEOUT ) {
			/* Query Decoder buffer */
			bdecode_status cur;
			BDBG_WRN(( "ply_event timed out (105ms), details: "));
			bdecode_get_status( context->decode, &cur );
			BDBG_WRN(( "  max_ppump_buf_sz %d rpump_buf_sz %d usable %d check %d", max_playpump_buf_size, recpump_buf_size, usable_ppump_buf_size, check_buf_size ));
			BDBG_WRN(( "  rec_buf rptr 0x%8x wptr 0x%8x diff %d ", (void *)rec_buffer, (int)(void *)rec_buffer + recpump_buf_size, recpump_buf_size ));
			BDBG_WRN(( "  ply_buf rptr 0x%8x wptr 0x%8x diff %d ", (void *)play_buffer, (int)(void *)play_buffer + max_playpump_buf_size, max_playpump_buf_size ));
			BDBG_WRN(("  decode video fifo size %d depth %d\n", cur.video_fifo_size, cur.video_fifo_depth ));
			}
		}

		/* Buffer can now be freed up  */
		if( brecpump_data_write_complete(context->pump, usable_ppump_buf_size ) ) 
			break;
	}
	BDBG_WRN(("tsdmaPlay thread exiting"));
	return NULL;
}

void playFromDisk_read_callback(void *context)
{
        /* We can't do the read and read_complete in the callback, so notify the thread */
        BKNI_SetEvent((BKNI_EventHandle)context);
}

void *playFromDisk_func(void *context_)
{
        struct context *context = (struct context *)context_;
        struct stat st;
        bdecode_status cur;
	long curPos,lastPos=0;

        bdecode_get_status( context->decode, &cur );

        BKNI_WaitForEvent(context->playDisk_event, BKNI_INFINITE);	/* wait for first event */

	/* Wait until we have enough data on disk to half fill */
	/* the video buffer before starting playpump */
	st.st_size = 0;
	while( st.st_size < cur.video_fifo_size / 2  ) {
		BKNI_Sleep(90);
		stat( "videos/stream.mpg", &st );
	}

       	context->playDiskHdl = fopen("videos/stream.mpg", "r");
        if (!context->playDiskHdl) goto error;


        while (1)
        {
                void *buffer;
                size_t buffer_size;
                int n;

                if (bplaypump_get_buffer(context->playDisk, &buffer, &buffer_size)) {
                        printf("bplaypump_get_buffer failed\n");
                        break;
                }
                if (!buffer_size) {
                        BKNI_WaitForEvent(context->playDisk_event, BKNI_INFINITE);
                        continue;
                }

                /* The first call to get_buffer will return the entire playback buffer.
                If we use it, we're going to have to wait until the descriptor is complete,
                and then we're going to underflow. So use a max size. */
#define MAX_READ 26320
                if (buffer_size > MAX_READ)
                        buffer_size = MAX_READ;

		stat( "videos/stream.mpg", &st );
		if( lastPos+MAX_READ > st.st_size ) {
			/* if no difference in size , dont try to read */
			BKNI_Sleep(100);
			continue;
		}
                n = fread(buffer, 1, buffer_size, context->playDiskHdl);
		lastPos = ftell( context->playDiskHdl );
                bdecode_get_status( context->decode, &cur );
                if (n < 0) break;
                if (n == 0) {
			stat( "videos/stream.mpg", &st );
			BKNI_Sleep(100);	/* wait for more data to come in */
                }
                else {
                        if (bplaypump_read_complete(context->playDisk, 0, n)) {
                                printf("bplaypump_read_complete failed\n");
                                break;
                        }
                }
        }
error:
        printf("play thread exiting\n");
        return NULL;
}

