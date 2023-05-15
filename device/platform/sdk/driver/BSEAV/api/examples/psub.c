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
 * $brcm_Workfile: psub.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/psub.c $
 * 
 * 2   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>

#define DEFAULT_PSUB_BUFFER_SIZE	( 5640 )	/* 30 * 188 */

#define NUM_PACKETSUBS 4

static char *BufferFiles[ 4 ] =
{
	"psub_tests/test1/psub_t1f0.mpg",
	"psub_tests/test1/psub_t1f1.mpg",
	"psub_tests/test1/psub_t1f2.mpg",
	"psub_tests/test1/psub_t1f3.mpg"
};


int LoadBuffer( 
	char *FileName,
	uint8_t *Buffer
	)
{
	int BytesRead = 0;
	FILE *Fp = fopen( FileName, "rb" );


	if( ferror( Fp ))
	{
		printf( "Can't open file %s\n", FileName );
		return 0;
	}

	BytesRead = fread( Buffer, 1, DEFAULT_PSUB_BUFFER_SIZE, Fp );
	if( ferror( Fp ))
	{
		printf( "Error reading file %s\n", FileName );
		fclose( Fp );
		return 0;
	}

	fclose( Fp );
	return BytesRead;
}



int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	brecpump_t recpump = brecpump_open(B_ID(0), NULL);
	brecord_t record = brecord_open();
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	brecord_file_t file = brecord_file_open("videos/stream.mpg", "videos/stream.nav");
	brecord_params params;
	void * buffer[NUM_PACKETSUBS];
	int i;
	int BytesLoaded;
	if (rc) goto error;


	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x31;
	mpeg.audio[0].pid = 0x34;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x31;

	brecord_params_init(&params, record);
	params.index_format = bindex_format_bcm;

	band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (brecord_start(record, recpump, stream, file, &params))
		goto error;

	for( i = 0; i < NUM_PACKETSUBS; i++ )	{
		bstream_packetsub_allocate_buffer(stream, &buffer[i], DEFAULT_PSUB_BUFFER_SIZE, 0x100 + i);
		BytesLoaded = LoadBuffer(BufferFiles[ i ], buffer[i] );
		bstream_packetsub_start(stream, BytesLoaded, 0x100 + i);	
	}

	printf("press enter to stop packet sub\n");
	getchar(); /* press enter to stop psub*/


	for( i = 0; i < NUM_PACKETSUBS; i++ )	{	
		bstream_packetsub_stop(stream, 0x100 + i);	
		bstream_packetsub_free_buffer(stream, 0x100 + i);	
	}

	if (brecord_stop(record))
		goto error;

	return 0;
error:
	return 1;
}

