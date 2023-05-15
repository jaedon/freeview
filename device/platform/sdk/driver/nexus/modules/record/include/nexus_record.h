/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_record.h $
 * $brcm_Revision: 13 $
 * $brcm_Date: 2/6/12 1:25p $
 *
 * Module Description:
 *  PVR record module
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/record/include/nexus_record.h $
 * 
 * 13   2/6/12 1:25p erickson
 * SW7425-2325: add NEXUS_RecordSettings.pollingTimer
 *
 * 12   1/5/12 12:47p erickson
 * SW7231-578: add NEXUS_RecordStatus.indexParsingErrors
 *
 * 11   12/14/10 3:18p erickson
 * SW7420-1285: add null_allowed attribute
 *
 * 10   6/2/10 5:31p erickson
 * SW7400-2789: add NEXUS_RecordSettings.allowLargeTimeGaps
 *
 * 9   9/17/09 10:11a erickson
 * SW7405-3026: allow Record module to build and run without Playback
 *  module
 *
 * 8   7/23/09 4:18p erickson
 * PR56980: fix units for NEXUS_RecordSettings.writeAllTimeout. improve
 *  comments on NEXUS_Record_StartAppend.
 *
 * 7   2/23/09 11:10a erickson
 * PR52034: added NEXUS_RecordStatus.picturesIndexed
 *
 * 6   1/7/09 9:49p erickson
 * PR49501: added NEXUS_Record_StartAppend
 *
 * 5   12/17/08 1:40p erickson
 * PR50461: add NEXUS_RecordSettings.writeAllTimeout
 *
 * 4   6/25/08 6:09p vsilyaev
 * PR 41869: Added user provided recpumpSettings to allow set secutity
 *  context
 *
 * 3   1/23/08 8:36p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 5:16p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/5   11/29/07 2:27p erickson
 * PR35457: doc update
 *
 * Nexus_Devel/4   11/14/07 10:44a vsilyaev
 * PR 36788: Fixed typo
 *
 * Nexus_Devel/3   11/14/07 10:41a erickson
 * PR36788: added NEXUS_Record_GetStatus
 *
 * Nexus_Devel/2   11/9/07 6:02p vsilyaev
 * PR 36788: Added timeshifting
 *
 * Nexus_Devel/1   11/7/07 5:09p vsilyaev
 * PR 36788: PVR record module
 *
 **************************************************************************/
#ifndef NEXUS_RECORD_H__
#define NEXUS_RECORD_H__

#include "nexus_types.h"
#include "nexus_recpump.h"
#include "nexus_file.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for the Record interface.
**/
typedef struct NEXUS_Record *NEXUS_RecordHandle;

/**
Summary:
Index type used in NEXUS_RecordSettings
**/
typedef enum NEXUS_RecordIndexType
{
    NEXUS_RecordIndexType_eNone,  /* no index */
    NEXUS_RecordIndexType_eBcm    /* Broadcom index created by bcmindexer and
                                     playable with bcmplayer. See bcmplayer source and docs.*/
} NEXUS_RecordIndexType;

/*
Summary:
Parameters passed into NEXUS_Record_SetSettings.

Description:
This structure must be initialized by NEXUS_Record_GetDefaultSettings or NEXUS_Record_GetSettings.
*/
typedef struct NEXUS_RecordSettings
{
    NEXUS_RecordIndexType indexFormat;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecpumpSettings recpumpSettings; /* configuration for the recpump. Note that this will overwrite any call to NEXUS_Recpump_SetSettings that you make. */
    bool stopOnOverflow;
    NEXUS_CallbackDesc overflowCallback;    /* Called whenever a buffer
                                overflow is detected. Recorded data will be corrupt.
                                stopOnOverflow member of NEXUS_RecordSettings controls whethey
                                record keeps recording anyway or stops recording on overflow . If
                                it stops on overflow, no corrupt data will be written to disk. */
    NEXUS_CallbackDesc errorCallback; /* Called whenever the
                                record thread can no longer write data because of an error.
                                Record stops after calling this callback. Read NEXUS_RecordStatus.error for specific error. */
    unsigned writeAllTimeout; /* If non-zero, NEXUS_Record_Stop will loop up to writeAllTimeout milliseconds waiting for all
                                the data to be written out. Record will still honor the recpumpSettings.atomSize and will drop any trailing data which is not atomSize aligned.
                                100 milliseconds is usually sufficient, but it can vary depending on the storage performance.
                                Default writeAllTimeout is zero (meaning no loop will be run). */
    bool allowLargeTimeGaps; /* By default, any large gap in the recording (e.g. from a disconnected frontend) will cause
                                the NAV index timestamp to be paused. This means that the gap will not appear in the timestamps on playback.
                                Set allowLargeTimeGaps to true if you want to disable this feature. Any gap in the recording will
                                appear in the index timestamps. */
    unsigned pollingTimer;   /* Internal timer use to poll recpump for low-bitrate streams.
                                This works in conjunction with recpump.data.dataReadyThreshold. Units of milliseconds. */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    bool fifoRecord;	
#endif
} NEXUS_RecordSettings;

/**
Summary:
Record-terminating error
**/
typedef enum NEXUS_RecordErrorType
{
    NEXUS_RecordErrorType_eNone,
    NEXUS_RecordErrorType_eDiskFull,    /* no more space left on disk */
    NEXUS_RecordErrorType_eMaxFileSize, /* file has reach maximum size allowed by file system */
    NEXUS_RecordErrorType_eUnknown
} NEXUS_RecordErrorType;

/*
Summary:
Create a new record object.
*/
NEXUS_RecordHandle NEXUS_Record_Create(void);


/*
Summary:
Destroy a record object.
*/
void NEXUS_Record_Destroy(
    NEXUS_RecordHandle record
    );

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_Record_GetDefaultSettings(
    NEXUS_RecordSettings *pSettings /* [out] */
    );

/**
Summary:
Returns current configuration of record
**/
void NEXUS_Record_GetSettings(
    NEXUS_RecordHandle record,
    NEXUS_RecordSettings *pSettings /* [out] */
    );

/**
Summary:
Changes configuraration of record.

Description:
This function updates configuration of the record engine. Some configuration (namely, recpump and recpumpSettings) could not be changed
unless record is stopped.
**/

NEXUS_Error NEXUS_Record_SetSettings(
    NEXUS_RecordHandle record,
    const NEXUS_RecordSettings *pSettings
    );

/*
Summary:
Start recording.
*/
NEXUS_Error NEXUS_Record_Start(
    NEXUS_RecordHandle record,
    NEXUS_FileRecordHandle file
    );

/**
Summary:
Settings for NEXUS_Record_StartAppend
**/
typedef struct NEXUS_RecordAppendSettings {
    uint64_t startingOffset; /* Set to the size of the data file being appended to. */
    unsigned timestamp; /* Set to the last timestamp in the index file being appended to. */
} NEXUS_RecordAppendSettings;

/**
Summary:
Get default settings
**/
void NEXUS_Record_GetDefaultAppendSettings(
    NEXUS_RecordAppendSettings *pSettings /* [out] */
    );

/**
Summary:
Start an appended recording.

Description:
NEXUS_FileRecordHandle should have been opened with NEXUS_FileRecord_AppendPosix.
The NEXUS_RecordStartAppendSettings settings are necessary if you are appending to an index.

The application is responsible for trimming the existing recorded file to both packet (e.g. 188 byte)
and direct I/O page (e.g. 4096 byte) alignment. The packet alignment is required by transport so that the
new stream with the appended data is a valid transport file. The direct I/O page alignment is required so that
direct I/O can be performed with the appended data.

The only way to satisfy both alignment requirements is to truncate the file to the least common multiple of both
numbers (e.g. truncate the file by filesize(file) % (188/4*4096)).
For an 8 Mbps stream, a maximum 192K truncation will only be a fraction of a second.

Nexus cannot automatically handle pid and codec changes. If you want to playback the file without any application
intervention, you must append the same pids and codecs-per-pid.

See nexus/examples/record_append.c for a working example.
**/
NEXUS_Error NEXUS_Record_StartAppend(
    NEXUS_RecordHandle record,
    NEXUS_FileRecordHandle file,
    const NEXUS_RecordAppendSettings *pSettings /* only required if there is an index */
    );

/*
Summary:
Stop recording.
*/
void NEXUS_Record_Stop(
    NEXUS_RecordHandle record
    );

/**
Summary:
**/
typedef struct NEXUS_RecordPidChannelSettings
{
    NEXUS_RecpumpAddPidChannelSettings recpumpSettings;
} NEXUS_RecordPidChannelSettings;


/*
Summary:
Add a pid channel to be recorded.

Description:
You can add a pid channel before or after NEXUS_Recpump_Start.
You cannot change the pid channel being indexed after starting.
*/
NEXUS_Error NEXUS_Record_AddPidChannel(
    NEXUS_RecordHandle record,
    NEXUS_PidChannelHandle pidChannel,
    const NEXUS_RecordPidChannelSettings *pSettings /* attr{null_allowed=y} NULL is allowed for default settings. */
    );

/*
Summary:
Remove a pid channel which is being recorded.

Description:
You can remove a pid channel before or after NEXUS_Recpump_Stop.
*/
NEXUS_Error NEXUS_Record_RemovePidChannel(
    NEXUS_RecordHandle record,
    NEXUS_PidChannelHandle pidChannel
    );

/**
Summary:
Remove all pid channel which are being recorded.
**/
void NEXUS_Record_RemoveAllPidChannels(
    NEXUS_RecordHandle record
    );

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
You can use default settings for all pids except an indexed video pid.
**/
void NEXUS_Record_GetDefaultPidChannelSettings(
    NEXUS_RecordPidChannelSettings *pSettings /* [out] */
    );

/**
The Record module can be compiled without a Playback module.
If so, there's no playback handle. However, we still need a playback handle for the function stubs.
**/
#if !NEXUS_HAS_PLAYBACK
typedef void *NEXUS_PlaybackHandle;
#endif

/**
Summary:
Add a NEXUS_PlaybackHandle for timeshifting control.
**/
NEXUS_Error NEXUS_Record_AddPlayback(
    NEXUS_RecordHandle record,
    NEXUS_PlaybackHandle playback
    );

/**
Summary:
Remove a NEXUS_PlaybackHandle which was added for timeshifting control.
**/
NEXUS_Error NEXUS_Record_RemovePlayback(
    NEXUS_RecordHandle record,
    NEXUS_PlaybackHandle playback
    );

/**
Summary:
Remove all NEXUS_PlaybackHandle's which were added for timeshifting control.
**/
void NEXUS_Record_RemoveAllPlaybacks(
    NEXUS_RecordHandle record
    );

/**
Summary:
Status returned by NEXUS_Record_GetStatus
**/
typedef struct NEXUS_RecordStatus
{
    NEXUS_RecpumpStatus recpumpStatus;
    unsigned long lastTimestamp;    /* timestamp, in ms of last frame recorded. */
    unsigned picturesIndexed;       /* number of pictures that have been indexed */
    NEXUS_RecordErrorType error;    /* record-terminating error. */
    unsigned indexParsingErrors;    /* number of SCT parsing errors detected by bcmindexer since open or reset.
                                       these errors could result from either errors in the stream or errors in the indexing HW/FW/SW. */
} NEXUS_RecordStatus;

/**
Summary:
Get current status of record
**/
NEXUS_Error NEXUS_Record_GetStatus(
    NEXUS_RecordHandle record,
    NEXUS_RecordStatus *pStatus /* [out] */
    );


#if  defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_RECORD)
NEXUS_Error NEXUS_Record_Switch2Linear(
    NEXUS_RecordHandle record
    );

NEXUS_Error NEXUS_Record_TpitIndexerEnable(
    NEXUS_RecordHandle record,
    bool tpitEnable
    );
#endif

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_RECORD_H__ */

