/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: record_stress_test.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 2/17/12 10:00a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/record_stress_test.c $
 * 
 * Hydra_Software_Devel/8   2/17/12 10:00a mward
 * SW7125-1237:  keep NUM_RECORDS <= NEXUS_NUM_PARSER_BANDS
 * 
 * Hydra_Software_Devel/7   1/17/12 10:26a erickson
 * SW7231-578: assert on index parsing errors
 * 
 * Hydra_Software_Devel/6   1/5/12 12:47p erickson
 * SW7231-578: add check for index parsing errors, but leave ASSERT
 * commented out for now
 * 
 * Hydra_Software_Devel/5   9/12/11 1:32p erickson
 * SW7405-4989: fix warning
 * 
 * Hydra_Software_Devel/4   8/31/11 12:14p jtna
 * SW7405-4989: add standard header and general clean-up
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#if NEXUS_HAS_RECORD
#include "nexus_record.h"
#endif
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

BDBG_MODULE(record_stress_test);

#if NEXUS_NUM_PARSER_BANDS < 7
#define NUM_RECORDS NEXUS_NUM_PARSER_BANDS
#else
#define NUM_RECORDS 7
#endif
#define MAX_NUM_RECORDS 7

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

static void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    BDBG_ERR(("Overflow"));
    BKNI_Fail();
}

int main(void) 
{
#if NEXUS_HAS_RECORD
    NEXUS_InputBand inputBand;
    NEXUS_FileRecordHandle file[NUM_RECORDS];
    NEXUS_RecpumpHandle recpump[NUM_RECORDS];
    NEXUS_RecordHandle record[NUM_RECORDS];
    NEXUS_ParserBand parserBand[NUM_RECORDS];
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_RecordPidChannelSettings pidSettings;
    NEXUS_RecordSettings recordSettings;
    NEXUS_PidChannelHandle pidChannel[NUM_RECORDS][2];
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle filePlay;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    const char fname[MAX_NUM_RECORDS][19] = {"videos/stream0.mpg", "videos/stream1.mpg", "videos/stream2.mpg", "videos/stream3.mpg", "videos/stream4.mpg", "videos/stream5.mpg", "videos/stream6.mpg"};
    const char index[MAX_NUM_RECORDS][19] = {"videos/stream0.nav", "videos/stream1.nav", "videos/stream2.nav", "videos/stream3.nav", "videos/stream4.nav", "videos/stream5.nav", "videos/stream6.nav"};

    /* Any available hd mpeg stream can be used */
    const char *fnamePlay = "videos/symphonyAvcHD.mpg";
    unsigned long i=0, j=0;
    int rc;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    filePlay = NEXUS_FilePlay_OpenPosix(fnamePlay, NULL);
    if (!filePlay) {
        fprintf(stderr, "can't open file:%s\n", fnamePlay);
        return -1;
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    /* Bring up video display and outputs */
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* Open the audio and video pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eH264; /* must be told codec for correct handling */
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x1522, &playbackPidSettings);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
    audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, 0x1523, &playbackPidSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
       the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 1;
    ipsOpenSettings.remuxIndex = 1;
    ips = IpsOpen(&ipsOpenSettings);
#endif

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    parserBand[0] = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand[0], &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;

    /* configure the video pid for indexing */
    NEXUS_Record_GetDefaultPidChannelSettings(&pidSettings);
    pidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    pidSettings.recpumpSettings.pidTypeSettings.video.index = true;
    pidSettings.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    for(i=0; i<NUM_RECORDS; i++){
#ifdef IP_STREAMER_SUBST
        if (i==0) continue;
#endif
        parserBand[i] = (NEXUS_ParserBand)(NEXUS_ParserBand_e0 + i);
        rc = NEXUS_ParserBand_SetSettings(parserBand[i], &parserBandSettings);
        BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
        IpsAddParserBandSettings(ips, &parserBandSettings);
#endif
        
        recpump[i] = NEXUS_Recpump_Open(i, NULL);
        record[i] = NEXUS_Record_Create();

        NEXUS_Record_GetSettings(record[i], &recordSettings);
        recordSettings.recpump = recpump[i];
        recordSettings.overflowCallback.callback = overflow_callback;
        recordSettings.overflowCallback.context = NULL;
        rc = NEXUS_Record_SetSettings(record[i], &recordSettings);
        BDBG_ASSERT(!rc);

        file[i] = NEXUS_FileRecord_OpenPosix(fname[i], index[i]);

        /* open video pid channel*/
        pidChannel[i][0] = NEXUS_PidChannel_Open(parserBand[i], 0x31, NULL);
        rc = NEXUS_Record_AddPidChannel(record[i], pidChannel[i][0], &pidSettings);
        BDBG_ASSERT(!rc);
        
        /* open audio pid channel*/
        pidChannel[i][1] = NEXUS_PidChannel_Open(parserBand[i], 0x34, NULL);
        rc = NEXUS_Record_AddPidChannel(record[i], pidChannel[i][1], NULL);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Record_Start(record[i], file[i]);
        BDBG_ASSERT(!rc);
    }

    /* Start decoders */
    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
    BDBG_ASSERT(!rc);

    /* Start playback */
    rc = NEXUS_Playback_Start(playback, filePlay, NULL);
    BDBG_ASSERT(!rc);
    videoProgram.codec = NEXUS_VideoCodec_eH264;
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    
    /* File system stress test */
    for(i=0; i<100; i++) {
        unsigned j;
        
        NEXUS_VideoDecoder_Stop(videoDecoder);
        NEXUS_AudioDecoder_Stop(audioDecoder);
        NEXUS_Playback_Stop(playback);
        NEXUS_FilePlay_Close(filePlay);

        filePlay = NEXUS_FilePlay_OpenPosix(fnamePlay, NULL);
        if (!filePlay) {
            fprintf(stderr, "can't open file:%s\n", fnamePlay);
            return -1;
        }
        rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
        BDBG_ASSERT(!rc);
        rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Playback_Start(playback, filePlay, NULL);
        BDBG_ASSERT(!rc);
        
        BKNI_Sleep(500);
        BDBG_WRN(("loop1: %d", i));
        
        for(j=0;j<NUM_RECORDS;j++){
            NEXUS_RecordStatus status;
            rc = NEXUS_Record_GetStatus(record[j], &status);
            BDBG_ASSERT(!rc);
            BDBG_ASSERT(status.indexParsingErrors == 0);
        }
    }
    BDBG_WRN(("End of file system stress test"));

    /* OS time slicing stress test */
    for(i=0; i<100; i++) {
        BDBG_WRN(("loop2: %d", i));
        for(j=0; j<10000000; j++) {
            ;
        }
    }
    BDBG_WRN(("End of OS timeslicing stress test"));

    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(filePlay);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(stcChannel);

    for(i=0; i<NUM_RECORDS; i++){
        NEXUS_Record_Stop(record[i]);
        NEXUS_Record_RemoveAllPidChannels(record[i]);
        NEXUS_PidChannel_Close(pidChannel[i][0]);
        NEXUS_PidChannel_Close(pidChannel[i][1]);
        NEXUS_FileRecord_Close(file[i]);
        NEXUS_Record_Destroy(record[i]);
        NEXUS_Recpump_Close(recpump[i]);
    }

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
   
    NEXUS_Platform_Uninit();
    
#endif
    return 0;
}

