/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: tspsimgr2.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 10/9/12 9:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/tspsi2/tspsimgr2.h $
 * 
 * 4   10/9/12 9:05p tokushig
 * SW7231-749: remove settop references from tspsimgr2
 * 
 * 3   2/27/12 4:17p tokushig
 * SW7405-5581: break out qam tuner from tuner files
 * 
 * 2   2/22/12 5:34p tokushig
 * SW7405-5581: tspsimgr change for brutus1/2 coexistence
 * 
 * 1   2/22/12 2:26p tokushig
 * SW7405-5581: nexus based tspsimgr
 * 
 * brutus2_psi/1   2/12/12 4:53p tokushig
 * SW7405-5581: modify to use Nexus directly instead of settop api
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
#ifndef TSPSIMGR2_H__
#define TSPSIMGR2_H__

#include "berr.h"
#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bresult {
    b_ok=0,
    berr_out_of_memory=1,
    berr_invalid_parameter=2,
    berr_not_supported=3,
    berr_not_available=4,
    berr_busy=5,
    berr_external_error=6,
    berr_invalid_state=7,
	berr_timeout=8
} bresult;

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
BERR_Code tsPsi_getChannelInfo2(CHANNEL_INFO_T * pChanInfo, NEXUS_ParserBand band);

/**
Set the timeout values for various blocking operations in tspsimgr.
**/
void tsPsi_setTimeout2( int patTimeout, int pmtTimeout );

/**
Get the timeout values for various blocking operations in tspsimgr.
**/
void tsPsi_getTimeout2( int *patTimeout, int *pmtTimeout );

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
int tsPsi_getPAT2(NEXUS_ParserBand band, void * pPatBuffer, unsigned patBufferSize);

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
BERR_Code tsPsi_getPMTs2(NEXUS_ParserBand     band, 
                         const void         * pPatBuffer, 
                         unsigned             patBufferSize,
                         tsPsi_PMT_callback   callback, 
                         void               * context);

/**
Summary:
Parse a PMT structure into a PROGRAM_INFO_T structure.
**/
void tsPsi_parsePMT2(const void *pmt, unsigned pmtSize, PROGRAM_INFO_T *p_programInfo);

#ifdef __cplusplus
}
#endif

#endif /* TSPSIMGR_H__ */
