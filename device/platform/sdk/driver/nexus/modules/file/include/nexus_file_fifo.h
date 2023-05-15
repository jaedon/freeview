/***************************************************************************
 *     (c)2005-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_file_fifo.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 5/26/11 7:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $old_brcm_Log: /BSEAV/api/include/bsettop_fileio_fifo.h $
 *
 * 5   4/12/06 2:16p vsilyaev
 * PR 20680: Promoted bsettop_fileio_fifo API
 *
 * 4   12/23/05 4:10p vsilyaev
 * PR 18183: Renamed API functions
 *
 * 2   11/21/05 3:42p vsilyaev
 * PR 18183: Renamed size function to bounds, so it would remain useful
 * with trimmed files
 *
 * 1   11/17/05 4:22p vsilyaev
 * PR 18183: Added circular(FIFO) file
 *
 ***************************************************************************/
#ifndef NEXUS_FILE_FIFO_H__
#define NEXUS_FILE_FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nexus_file.h"

/*
Summary:
Type of record file suitable for timeshifting
*/
typedef struct NEXUS_FifoRecord *NEXUS_FifoRecordHandle;

/*
Summary:
Configuration parameters for the timeshifting buffer

Description:
Those numbers are used to limit size of the timishifting buffer to be occupied on the disk.
Usually software stack would try do not exceed soft limit of the timeshifting buffer, however
in certain cases, for example, if bitrates of the recorded stream suddenly increases, it could
go up to hardlimit. Once hardlimit is reached it would result in the overflow error.
*/
typedef struct NEXUS_FifoRecordLimit {
    off_t soft; /* soft limit of the timeshifting buffer, in bytes */
    off_t hard; /* hard limit of the timeshifting buffer, in bytes */
} NEXUS_FifoRecordLimit;


/*
Summary:
Configuration parameters for the timeshifting file
*/
typedef struct NEXUS_FifoRecordSettings {
    unsigned interval; /* Size of the timeshifting buffer, in seconds */
    unsigned snapshotInterval; /* interval between metadata writes, in seconds. 0 means metadata written only when file is closed */
    NEXUS_FifoRecordLimit data; /* Limits for the timeshifting data file */
    NEXUS_FifoRecordLimit index; /* Limits for the timeshifting index file */
} NEXUS_FifoRecordSettings;

/*
Summary:
This function opens new timeshifting buffer

Description:
The NEXUS_FifoRecordHandle can be used for only one record at a time.
The files will be created if they don't already exist, and truncated if they do.
The index file is mandatory for the timeshifting.

You close this handle by calling NEXUS_FileRecord_Close.
*/
NEXUS_FifoRecordHandle NEXUS_FifoRecord_Create(
    const char *mpegFileName, /* mpeg file name to create */
    const char *indexFileName /* index file name to create */
    );

/*
Summary:
This function returns a handle to be passed into the NEXUS_Record_Start().
*/
NEXUS_FileRecordHandle NEXUS_FifoRecord_GetFile(
    NEXUS_FifoRecordHandle fifo /* Handle returned by NEXUS_FifoRecord_Create */
    );

/*
Summary:
This function returns current position of the recorded file
*/
NEXUS_Error NEXUS_FifoRecord_GetPosition(
    NEXUS_FifoRecordHandle fifo, /* Handle returned by NEXUS_FifoRecord_Create */
    NEXUS_FilePosition *pFirst,  /* [out] first position in the file */
    NEXUS_FilePosition *pLast    /* [out] last position in the file */
    );

/*
Summary:
Open a playback file to pass to NEXUS_Playback_Start().

Description:
This function shall be used only for files created by NEXUS_FifoRecord_Create.
The NEXUS_FilePlayHandle can be used for only one playback at a time.

You close this handle by calling NEXUS_FilePlay_Close.
*/
NEXUS_FilePlayHandle NEXUS_FifoPlay_Open(
    const char *mpegFileName,
    const char *indexFileName,
    NEXUS_FifoRecordHandle writer /* Handle returned by NEXUS_FifoRecord_Create */
    );

/*
Summary:
This function returns current configuration of the timeshifting fifo
*/
void NEXUS_FifoRecord_GetSettings(
    NEXUS_FifoRecordHandle fifo, /* Handle returned by NEXUS_FifoRecord_Create */
    NEXUS_FifoRecordSettings *pSettings /* [out] current settings of timeshifting buffer */
    );

/*
Summary:
This function sets the new configuration of the timeshifting fifo
*/
NEXUS_Error NEXUS_FifoRecord_SetSettings(
    NEXUS_FifoRecordHandle fifo, /* Handle returned by NEXUS_FifoRecord_Create */
    const NEXUS_FifoRecordSettings *pSettings /* new settings of timeshifting buffer */
    );

#if defined(HUMAX_PLATFORM_BASE)

/*
Summary:
This function opens new timeshifting buffer for Usb Flash

Description:
The NEXUS_FifoRecordHandle can be used for only one record at a time.
The files will be created if they don't already exist, and truncated if they do.
The index file is mandatory for the timeshifting.

You close this handle by calling NEXUS_FileRecord_Close.
*/

NEXUS_FifoRecordHandle
NEXUS_FifoRecord_CreateForUsbFlash(
    const char *mpegFileName, /* mpeg file name to create */
    const char *indexFileName /* index file name to create */
    );

/*
Summary:
Open a playback file to pass to NEXUS_Playback_Start().

Description:
This function shall be used only for files created by NEXUS_FifoRecord_CreateForUsbFlash.
The NEXUS_FilePlayHandle can be used for only one playback at a time.

You close this handle by calling NEXUS_FilePlay_Close.
*/

NEXUS_FilePlayHandle
NEXUS_FifoPlay_OpenForUsbFlash(
    const char *mpegFileName,
    const char *indexFileName,
    NEXUS_FifoRecordHandle writer /* Handle returned by NEXUS_FifoRecord_CreateForUsbFlash */
    );

#endif /* HUMAX_PLATFORM_BASE */

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_FILE_FIFO_H__ */


