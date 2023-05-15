/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: allpass_record_from_playback.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 10/18/11 2:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/allpass_record_from_playback.c $
 * 
 * Hydra_Software_Devel/7   10/18/11 2:20p gmullen
 * SW7425-1383: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1383/1   10/14/11 4:12p gmullen
 * SW7425-1383: Updated allPass usages to use new API
 * 
 * Hydra_Software_Devel/6   2/8/10 4:48p mward
 * SW7125-195:  NEXUS_Playback_Destroy() will close and invalidate the
 * allPassPidChannel.  Remove the pid channels from record first while
 * the shared pid channel is still valid.
 * 
 * Hydra_Software_Devel/5   10/14/09 10:48a jtna
 * SW7405-3205: remove record pid channels during system shutdown; this
 * fixes a reported memleak
 * 
 * Hydra_Software_Devel/4   10/14/09 10:31a jtna
 * SW7405-3205: return 0 on test termination
 * 
 * Hydra_Software_Devel/3   6/1/09 12:48p erickson
 * PR42679: make finite
 *
 * Hydra_Software_Devel/2   12/17/08 1:39p erickson
 * PR50461: modify test to play and record every byte of the file
 *
 * Hydra_Software_Devel/1   12/1/08 12:48p erickson
 * PR49676: added allpass_record_from_playback.c test
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_playback.h"
#include "nexus_record.h"
#include "nexus_file.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

bool g_eos = false;
void eos(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    g_eos = true;
}

int main(int argc, char **argv) {
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PidChannelHandle allPassPidChannel;
    NEXUS_FilePlayHandle playbackFile;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidCfg;
    NEXUS_FileRecordHandle recordFile;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordPidChannelSettings recordPidCfg;
    NEXUS_RecordSettings recordCfg;
    NEXUS_RecordHandle record;
    NEXUS_Error rc;
    const char *fname = "videos/mummy_MI_5element_q64.mpg";

    if (argc > 1) fname = argv[1];

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    playbackFile = NEXUS_FilePlay_OpenPosix(fname, NULL);
    if (!playbackFile) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.playpumpSettings.allPass = true;
    playbackSettings.playpumpSettings.acceptNullPackets = true;
    playbackSettings.playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eNone;
    playbackSettings.playpumpSettings.maxDataRate = 50 * 1000000; /* prevent record overflow because of limited disk i/o speed */
    playbackSettings.startPaused = false;
    playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePause;
    playbackSettings.endOfStreamCallback.callback = eos;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidCfg);
    NEXUS_Playpump_GetAllPassPidChannelIndex(playpump, (unsigned *) &playbackPidCfg.pidSettings.pidSettings.pidChannelIndex );
    allPassPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0 /* don't care */, &playbackPidCfg);

    recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(record, &recordCfg);
    recordCfg.recpump = recpump;
    recordCfg.writeAllTimeout = 1000; /* 1 second to write everything */
    rc = NEXUS_Record_SetSettings(record, &recordCfg);

    recordFile = NEXUS_FileRecord_OpenPosix("videos/stream.mpg", "videos/stream.nav");

    NEXUS_Record_GetDefaultPidChannelSettings(&recordPidCfg);
    recordPidCfg.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    recordPidCfg.recpumpSettings.pidTypeSettings.video.index = true;
    recordPidCfg.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    recordPidCfg.recpumpSettings.pidTypeSettings.video.pid = 0x11; /* indexing by pid, not pid channel, is required for allPass */

    rc = NEXUS_Record_AddPidChannel(record, allPassPidChannel, &recordPidCfg);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Record_Start(record, recordFile);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Playback_Start(playback, playbackFile, NULL);
    BDBG_ASSERT(!rc);

    while (1) {
        NEXUS_PlaybackStatus pstatus;
        NEXUS_RecordStatus rstatus;

        rc = NEXUS_Playback_GetStatus(playback, &pstatus);
        BDBG_ASSERT(!rc);
        printf("playback bytesPlayed %d\n", (unsigned)pstatus.bytesPlayed);

        rc = NEXUS_Record_GetStatus(record, &rstatus);
        BDBG_ASSERT(!rc);
        printf("record bytesRecorded data=%d, index=%d\n", (unsigned)rstatus.recpumpStatus.data.bytesRecorded, (unsigned)rstatus.recpumpStatus.index.bytesRecorded);

        if (g_eos || pstatus.bytesPlayed > 500*1000000) /* maximum of 500 MB */
            break;
        else
            BKNI_Sleep(1000);
    }

    /* Bring down system */
    NEXUS_Record_Stop(record);
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(playbackFile);
    NEXUS_FileRecord_Close(recordFile);
    NEXUS_Record_RemoveAllPidChannels(record);
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_RemoveAllPidChannels(recpump);
	NEXUS_Playback_Destroy(playback);
	NEXUS_Playpump_Close(playpump);
    NEXUS_Recpump_Close(recpump);
    NEXUS_Platform_Uninit();

#endif
    return 0;
}
