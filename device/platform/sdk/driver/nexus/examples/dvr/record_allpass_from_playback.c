/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: record_allpass_from_playback.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 8/9/12 12:56p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/dvr/record_allpass_from_playback.c $
 * 
 * 5   8/9/12 12:56p erickson
 * SW7405-5681: add bandhold
 * 
 * 4   10/18/11 4:26p gmullen
 * SW7425-1383: Use Nexus API to get allPass PID channel number
 * 
 * SW7425-1383/1   10/18/11 3:50p gmullen
 * SW7425-1383: Use new API to get allPass PID channel number
 * 
 * 3   6/1/09 1:24p erickson
 * PR35457: set better input stream
 *
 * 2   2/19/09 3:19p erickson
 * PR51841: remove examples_lib
 *
 * 1   10/15/08 5:08p katrep
 * PR47690: Adding allpass record from playback
 *
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#if NEXUS_HAS_RECORD
#include "nexus_record.h"
#endif
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <assert.h>

int main(void) {
#if NEXUS_HAS_RECORD
    int rc;
    NEXUS_FileRecordHandle file;
    NEXUS_FilePlayHandle playfile;
    NEXUS_RecpumpOpenSettings recpumpOpenSettings;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_PidChannelHandle pidCh0;
    NEXUS_PlaybackPidChannelSettings pidCfg0;
    NEXUS_RecordSettings recordCfg;
    const char *fname = "videos/allpass_stream.mpg";
    const char *index = "videos/allpass_stream.nav";
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    const char *playfname = "videos/mummy_MI_5element_q64.mpg";
    NEXUS_PlatformSettings platformSettings;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;

    /* set up the source */
    playpump = NEXUS_Playpump_Open(NEXUS_ANY_ID, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    playfile = NEXUS_FilePlay_OpenPosix(playfname, NULL);
    if (!playfile) {
        fprintf(stderr, "can't open file:%s\n", playfname);
        return -1;
    }

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.playpumpSettings.allPass=true;
    playbackSettings.playpumpSettings.acceptNullPackets=true;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    NEXUS_Playback_GetDefaultPidChannelSettings(&pidCfg0);
    NEXUS_Playpump_GetAllPassPidChannelIndex(playbackSettings.playpump, (unsigned *) &pidCfg0.pidSettings.pidSettings.pidChannelIndex );
    pidCh0 = NEXUS_Playback_OpenPidChannel(playback, 0x00, &pidCfg0);   /* PID is ignored in allPass mode */

    NEXUS_Recpump_GetDefaultOpenSettings(&recpumpOpenSettings);
    /* set threshold to 80%. with band hold enabled, it's not actually a dataready threshold. it's
    a bandhold threshold. we are relying on the timer that's already in record. */
    recpumpOpenSettings.data.dataReadyThreshold = recpumpOpenSettings.data.bufferSize * 8 / 10;
    recpumpOpenSettings.index.dataReadyThreshold = recpumpOpenSettings.index.bufferSize * 8 / 10;
    recpump = NEXUS_Recpump_Open(NEXUS_ANY_ID, &recpumpOpenSettings);
    record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(record, &recordCfg);
    recordCfg.recpump = recpump;
    /* enable bandhold. required for record from playback. */
    recordCfg.recpumpSettings.bandHold = NEXUS_RecpumpFlowControl_eEnable;
    NEXUS_Record_SetSettings(record, &recordCfg);

    file = NEXUS_FileRecord_OpenPosix(fname,  index);

    NEXUS_Record_AddPidChannel(record,pidCh0,NULL);

    NEXUS_Record_Start(record, file);

    /* Start playback */
    NEXUS_Playback_Start(playback, playfile, NULL);

    printf("press ENTER to stop record\n");
    getchar();

    NEXUS_Playback_Stop(playback);
    NEXUS_Record_Stop(record);
    NEXUS_Record_RemoveAllPidChannels(record);
    NEXUS_Playback_ClosePidChannel(playback, pidCh0);
    NEXUS_Playback_Destroy(playback);
    NEXUS_FileRecord_Close(file);
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);
    NEXUS_Platform_Uninit();

#endif
    return 1;
}