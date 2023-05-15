/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: timeshift_jump.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 7/23/12 4:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/file/timeshift_jump.c $
 * 
 * Hydra_Software_Devel/10   7/23/12 4:38p jessem
 * SW7425-3568: Added BKNI_DestroyEvent(event).
 *
 * Hydra_Software_Devel/9   2/22/12 4:36p erickson
 * SW7425-2458: add clean shutdown, remove unnecessary code
 *
 * Hydra_Software_Devel/8   8/8/11 11:52a erickson
 * SWDEPRECATED-3044: speed up tests
 *
 * Hydra_Software_Devel/7   7/8/11 12:19p jtna
 * SW7405-5336: added basic sanity check for incoming streamer input
 *
 * Hydra_Software_Devel/6   10/22/10 11:13a randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 *
 * Hydra_Software_Devel/5   7/29/10 11:09a calvinho
 * SW7405-4653: Added Ip Streamer input support
 *
 * Hydra_Software_Devel/4   7/16/10 11:56a mward
 * SW7405-4621: NEXUS_Time no longer available to apps.
 *
 * Hydra_Software_Devel/3   7/30/09 12:12p jtna
 * PR54129: app must handle seek failures that are normal
 *
 * Hydra_Software_Devel/2   6/4/09 4:55p erickson
 * PR54129: allow cmdline to set large # of loops for overnight testing
 *
 * Hydra_Software_Devel/1   6/2/09 1:52p erickson
 * PR54129: add test app
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_RECORD
#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_file_fifo.h"
#include "nexus_record.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include "bstd.h"
#include "bkni.h"

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

BDBG_MODULE(timeshift_jump);

void first_pts(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(int argc, char **argv) {
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_FilePlayHandle playbackfile;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    unsigned short vidpid = 0x11;
    unsigned short audpid = 0x14;
    NEXUS_FifoRecordHandle fifofile;
    NEXUS_FileRecordHandle recordfile;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_RecordPidChannelSettings pidSettings;
    NEXUS_RecordSettings recordSettings;
    NEXUS_PidChannelHandle pidChannel[2];
    NEXUS_Error rc;
    NEXUS_FifoRecordSettings fifoRecordSettings;
    const char *datafilename = "videos/timeshift_jump.mpg";
    const char *indexfilename = "videos/timeshift_jump.nav";
    BKNI_EventHandle event;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    unsigned loops = 250;
    unsigned failcnt = 0;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    if (argc > 1) loops = atoi(argv[1]);

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

    /******************************
    * start record
    **/

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 1;
    ipsOpenSettings.remuxIndex = 1;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(record, &recordSettings);
    recordSettings.recpump = recpump;
    NEXUS_Record_SetSettings(record, &recordSettings);

    fifofile = NEXUS_FifoRecord_Create(datafilename, indexfilename);

    NEXUS_FifoRecord_GetSettings(fifofile, &fifoRecordSettings);
    fifoRecordSettings.interval = 20; /* 20 second buffer */
    rc = NEXUS_FifoRecord_SetSettings(fifofile, &fifoRecordSettings);
    BDBG_ASSERT(!rc);

    recordfile = NEXUS_FifoRecord_GetFile(fifofile);

    NEXUS_Record_GetDefaultPidChannelSettings(&pidSettings);
    pidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    pidSettings.recpumpSettings.pidTypeSettings.video.index = true;
    pidSettings.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    pidChannel[0] = NEXUS_PidChannel_Open(parserBand, vidpid, NULL);
    rc = NEXUS_Record_AddPidChannel(record, pidChannel[0], &pidSettings);
    BDBG_ASSERT(!rc);

    pidChannel[1] = NEXUS_PidChannel_Open(parserBand, audpid, NULL);
    rc = NEXUS_Record_AddPidChannel(record, pidChannel[1], NULL);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Record_Start(record, recordfile);
    BDBG_ASSERT(!rc);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = videoDecoder;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    /******************************
    * start playback
    **/

    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    playbackfile = NEXUS_FifoPlay_Open(datafilename, indexfilename, fifofile);
    if (!playbackfile) {
        fprintf(stderr, "can't open files:%s %s\n", datafilename, indexfilename);
        return -1;
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.beginningOfStreamAction = NEXUS_PlaybackLoopMode_ePlay; /* when play hits the beginning, switch back to play */
    playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay; /* when play hits the end, wait for record */
    playbackSettings.timeshifting = true;
    playbackSettings.stcChannel = stcChannel;
    playbackSettings.stcTrick = true;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    /* Bring up video display and outputs */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.firstPts.callback = first_pts;
    videoDecoderSettings.firstPts.context = event;
    NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2; /* must be told codec for correct handling */
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, vidpid, &playbackPidSettings);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    /* Start decoders */
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);

    /* Linking Playback to Record allows Playback to sleep until Record writes data. Avoids a busyloop near live. */
    rc = NEXUS_Record_AddPlayback(record, playback);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Playback_Start(playback, playbackfile, NULL);
    BDBG_ASSERT(!rc);

    while (--loops) {
        NEXUS_PlaybackStatus playbackStatus;
        struct timeval start, end;

        BKNI_Sleep(rand() % 1000);

        BKNI_ResetEvent(event);

        {
        unsigned pos;
        rc = NEXUS_Playback_GetStatus(playback, &playbackStatus);
        BDBG_ASSERT(!rc);
        pos = playbackStatus.first;
        if (playbackStatus.last - playbackStatus.first > fifoRecordSettings.interval * 1000 / 2) {
            /* once we've reached half of the timeshift buffer, we should not jump to the absolute beginning.
            the beginning of the file is truncated in blocks.
            if we jump to the absolute beginning, we might get an undesired beginningOfStream action when that truncation occurs. */
            pos += 5000;
        }
        BDBG_WRN(("Jump to beginning %u, %u...%u", pos, (unsigned)playbackStatus.first, (unsigned)playbackStatus.last));
        rc = NEXUS_Playback_Seek(playback, pos);
        }

        if (rc!=BERR_SUCCESS) { /* a failed seek is normal due to rare condition with trim */
            failcnt++;
            BDBG_WRN(("NEXUS_Playback_Seek failed. failure %d, loop %d", failcnt, loops));
            BDBG_ASSERT(failcnt<2); /* two consecutive failures are highly unlikely */
            continue;
        }
        else {
            failcnt = 0;
        }

        /* now wait for the "pcrlib: VideoRequestStc" message. if it doesn't come, die. */
        gettimeofday(&start,NULL);

        rc = BKNI_WaitForEvent(event, 10000);
        if (rc) {
            NEXUS_VideoDecoderStatus vdStatus;

            rc = NEXUS_Playback_GetStatus(playback, &playbackStatus);
            BDBG_ASSERT(!rc);

            rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &vdStatus);
            BDBG_ASSERT(!rc);

            BDBG_WRN(("No video: video decoder fifo %d/%d, playback %d/%d",
                vdStatus.fifoDepth, vdStatus.fifoSize,
                playbackStatus.fifoDepth, playbackStatus.fifoSize));
            BKNI_Fail();
        }

        gettimeofday(&end,NULL);
        BDBG_WRN(("time to first pts: %lu msec\n", (1000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000)));

    }

    printf("pass\n");
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    /* clean shutdown */
    NEXUS_Playback_Stop(playback);
    NEXUS_VideoDecoder_Stop(videoDecoder);

    NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_Record_RemovePlayback(record, playback);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Record_Destroy(record);
    NEXUS_FileRecord_Close(recordfile);
    NEXUS_FilePlay_Close(playbackfile);
    NEXUS_Playpump_Close(playpump);
    NEXUS_Recpump_Close(recpump);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(stcChannel);
	BKNI_DestroyEvent(event);
    NEXUS_Platform_Uninit();

    return 0;
}
