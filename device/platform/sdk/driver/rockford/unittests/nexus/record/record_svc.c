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
 * $brcm_Workfile: record_svc.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/4/11 6:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/record_svc.c $
 * 
 * Hydra_Software_Devel/2   3/4/11 6:23p vsilyaev
 * SW7422-14: Removed leaked history
 * 
 * Hydra_Software_Devel/1   3/4/11 5:57p vsilyaev
 * SW7422-14: SVC record case
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

static bool g_eos = false;
void eos(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    g_eos = true;
}

int main(int argc, char **argv) {
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PidChannelHandle pidChannelBase, pidChannelExt;
    NEXUS_FilePlayHandle playbackFile;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_FileRecordHandle recordFile;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordPidChannelSettings recordPidCfg;
    NEXUS_RecordSettings recordCfg;
    NEXUS_RecordHandle record;
    NEXUS_Error rc;
    const char *fname = "videos/test_sequenceB.ts";

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
    playbackSettings.playpumpSettings.maxDataRate = 2 * 1000000; /* prevent record overflow because of limited disk i/o speed */
    playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePause;
    playbackSettings.endOfStreamCallback.callback = eos;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    pidChannelBase = NEXUS_Playback_OpenPidChannel(playback, 0x31, NULL);
    BDBG_ASSERT(pidChannelBase);
    pidChannelExt = NEXUS_Playback_OpenPidChannel(playback, 0x32, NULL);
    BDBG_ASSERT(pidChannelExt);

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
    recordPidCfg.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH264;
    recordPidCfg.recpumpSettings.pidTypeSettings.video.pid = 0x31; 
    rc = NEXUS_Record_AddPidChannel(record, pidChannelBase, &recordPidCfg);
    BDBG_ASSERT(!rc);

    NEXUS_Record_GetDefaultPidChannelSettings(&recordPidCfg);
    recordPidCfg.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    recordPidCfg.recpumpSettings.pidTypeSettings.video.index = true;
    recordPidCfg.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH264_Svc;
    recordPidCfg.recpumpSettings.pidTypeSettings.video.pid = 0x32; 
    rc = NEXUS_Record_AddPidChannel(record, pidChannelExt, &recordPidCfg);
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
