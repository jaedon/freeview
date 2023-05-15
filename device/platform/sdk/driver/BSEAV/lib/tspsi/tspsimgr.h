/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: tspsimgr.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 1/18/07 12:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/tspsi/tspsimgr.h $
 * 
 * 4   1/18/07 12:10p ahulse
 * PR27186: Differentiate between parser and input bands. AKA, fixing
 * bmessage from playback source
 * 
 * 3   12/14/06 12:00p jjordan
 * PR26473: add get timeout capability
 * 
 * 2   10/11/05 6:23p mphillip
 * PR17300: Increased from 32 to 64 (after all, it's only RAM)
 * 
 * 1   2/7/05 11:34p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   11/4/04 1:02p erickson
 * PR13141: added pmt pid to PMT callback
 * 
 * Irvine_BSEAVSW_Devel/2   7/21/04 12:07p erickson
 * PR11682: refactored so that finer-grain control is available, without
 * breaking existing api
 * 
 * Irvine_BSEAVSW_Devel/2   9/16/03 2:40p erickson
 * support c++, added #ifndef wrapper
 * include bsettop.h because this header file is not a part of the settop
 * api, therefore
 * it shouldn't assume it
 *
 * Irvine_BSEAVSW_Devel/1   9/10/03 4:33p marcusk
 * initial version
 *
 ****************************************************************/
#ifndef TSPSIMGR_H__
#define TSPSIMGR_H__

#include "bsettop.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint16_t	pid;
	uint8_t		streamType;
	uint16_t  	ca_pid;
} EPID;

#define MAX_PROGRAM_MAP_PIDS	12
typedef struct
{
	uint16_t	program_number;
	uint16_t	map_pid;
	uint8_t		version;
	uint16_t	pcr_pid;
	uint16_t  	ca_pid;
	uint8_t		num_video_pids;
	EPID		video_pids[MAX_PROGRAM_MAP_PIDS];
	uint8_t		num_audio_pids;
	EPID		audio_pids[MAX_PROGRAM_MAP_PIDS];
	uint8_t		num_other_pids;
	EPID		other_pids[MAX_PROGRAM_MAP_PIDS];
} PROGRAM_INFO_T;

#define MAX_PROGRAMS_PER_CHANNEL 64
typedef struct
{
	uint8_t		version;
	uint16_t	transport_stream_id;
	uint32_t	sectionBitmap;
	uint16_t	num_programs;
	PROGRAM_INFO_T program_info[MAX_PROGRAMS_PER_CHANNEL];
} CHANNEL_INFO_T;

/**
Summary:
Populate a CHANNEL_INFO_T structure by scanning PSI information on
a band.

Description:
This call waits for a PAT, then waits for the PMT's for each program,
then builds the structure.

If you want finer-grain control, use the other tspsimgr functions below.
**/
bresult tsPsi_getChannelInfo( CHANNEL_INFO_T *p_chanInfo, bband_t band, bstream_t stream );

/**
Set the timeout values for various blocking operations in tspsimgr.
**/
void tsPsi_setTimeout( int patTimeout, int pmtTimeout );

/**
Get the timeout values for various blocking operations in tspsimgr.
**/
void tsPsi_getTimeout( int *patTimeout, int *pmtTimeout );

/**
Summary:
Synchronous call to read the PAT, using the patTimeout.

Description:
bufferSize should be >= TS_PSI_MAX_PSI_TABLE_SIZE in order to read the PAT.

Return Values:
Returns the number of bytes read.
-1 for an error.
0 for no PAT read.
>0 for successful PAT read.
**/
int tsPsi_getPAT(bband_t band, void *buffer, unsigned bufferSize, bstream_t stream );

/**
Callback used by tsPsi_getPMTs
**/
typedef void (*tsPsi_PMT_callback)(void *context, uint16_t pmt_pid, const void *pmt, unsigned pmtSize);

/**
Summary:
Read PMT's for each program specified in the PAT.

Description:
This will launch multiple bmessage_stream's and call the callback as each
PMT is read.
**/
bresult	tsPsi_getPMTs(bband_t band,
	const void *pat, unsigned patSize,
	tsPsi_PMT_callback callback, void *context, bstream_t stream);

/**
Summary:
Parse a PMT structure into a PROGRAM_INFO_T structure.
**/
void tsPsi_parsePMT(const void *pmt, unsigned pmtSize, PROGRAM_INFO_T *p_programInfo);

#ifdef __cplusplus
}
#endif

#endif /* TSPSIMGR_H__ */
