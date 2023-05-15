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
 * $brcm_Workfile: commercial_splice.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 7/29/10 11:16a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/commercial_splice.c $
 * 
 * Hydra_Software_Devel/4   7/29/10 11:16a calvinho
 * SW7405-4653: Added Ip Streamer input support and changed wait time
 * definition
 * 
 * Hydra_Software_Devel/3   4/1/09 10:46a erickson
 * PR53077: removed getchar
 *
 * Hydra_Software_Devel/2   3/13/09 12:08p erickson
 * PR53077: add varying pid/codec support. wait for first pts before
 * waiting for insertion end point. mute decoders until end point occurs.
 *
 * Hydra_Software_Devel/1   3/12/09 12:58p erickson
 * PR53077: add commercial_splice
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_playpump.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_component_output.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

/**
Usage:
nexus commercial_splice none
nexus commercial_splice pts duration file
**/

static void setevent_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int pump(NEXUS_PlaypumpHandle playpump, FILE *file)
{
    while (1) {
        void *buffer;
        size_t buffer_size;
        int n;
        NEXUS_Error rc;

        rc = NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size);
        BDBG_ASSERT(!rc);
        if (buffer_size == 0) {
            return 1;
        }

        /* The first call to get_buffer will return the entire playback buffer.
        If we use it, we're going to have to wait until the descriptor is complete,
        and then we're going to underflow. So use a max size. */
#define MAX_READ (188*1024)
        if (buffer_size > MAX_READ)
            buffer_size = MAX_READ;

        n = fread(buffer, 1, buffer_size, file);
        BDBG_ASSERT(n >= 0);
        if (n == 0) {
            return 0;
        }
        else {
            rc = NEXUS_Playpump_ReadComplete(playpump, 0, n);
            BDBG_ASSERT(!rc);
        }
    }
}

#define PTS_SPLICE_THRESHOLD (45000 / 4) /* 1/4 second */

void wait_for_pts(NEXUS_VideoDecoderHandle videoDecoder, uint32_t pts, bool start)
{
    unsigned dist, lastdist = 0;
    NEXUS_VideoDecoderStatus status;
    while (1) {
        NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
        if (status.pts >= pts && (!start || (status.pts < pts + PTS_SPLICE_THRESHOLD))) {
            break;
        }
        dist = (pts - status.pts) / 45000;
        if (dist != lastdist) {
            printf("%d seconds to go...\n", dist);
            lastdist = dist;
        }
    }
}

void wait_for_clip_end(NEXUS_VideoDecoderHandle videoDecoder)
{
    NEXUS_VideoDecoderStatus status;
    while (1) {
        NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
        if (status.queueDepth == 0) {
            break;
        }
    }
}

uint32_t get_cur_pts(NEXUS_VideoDecoderHandle videoDecoder){
    NEXUS_VideoDecoderStatus status;
    NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
    return status.pts;
}

int main(int argc, char **argv)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderSettings audioDecoderSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_PidChannelHandle liveVideoPidChannel, liveAudioPidChannel, commercialVideoPidChannel, commercialAudioPidChannel;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    FILE *file;
    BKNI_EventHandle playpumpEvent;
    BKNI_EventHandle firstPtsEvent;
    NEXUS_Error rc;
    uint32_t pts = 0;
    unsigned duration = 0;
    const char *fname = NULL;
    NEXUS_VideoCodec commercialVideoCodec = NEXUS_VideoCodec_eMpeg2;
    NEXUS_AudioCodec commercialAudioCodec = NEXUS_AudioCodec_eAc3;
    uint16_t commercialVideoPid = 0x31;
    uint16_t commercialAudioPid = 0x34;
    NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    if (argc > 1 && !strcmp(argv[1], "none")) {
        pts = 0;
    }
    else if (argc > 3) {
        pts = strtoul(argv[1], NULL, 0);
        duration = strtoul(argv[2], NULL, 0);
        fname = argv[3];
    }
    if (argc > 7) {
        commercialVideoPid = strtoul(argv[4], NULL, 0);
        commercialVideoCodec = !strcasecmp(argv[5], "avc") ? NEXUS_VideoCodec_eH264 : NEXUS_VideoCodec_eMpeg2;
        commercialAudioPid = strtoul(argv[6], NULL, 0);
        commercialAudioCodec = !strcasecmp(argv[7], "ac3") ? NEXUS_AudioCodec_eAc3 : NEXUS_AudioCodec_eMpeg;
        printf("got %#x %d %#x %d\n", commercialVideoPid, commercialVideoCodec, commercialAudioPid, commercialAudioCodec);
    }

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&playpumpEvent);
    BKNI_CreateEvent(&firstPtsEvent);

    /* set up live stream */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    stcChannel = NEXUS_StcChannel_Open(0, NULL);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 1;
    ipsOpenSettings.remuxIndex = 1;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* Open the audio and video pid channels */
    liveVideoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x31, NULL);
    liveAudioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x34, NULL);

    /* decoders and display */
    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddAudioDecoderOpenSettings(ips, &audioDecoderOpenSettings);
#endif
    audioDecoder = NEXUS_AudioDecoder_Open(0, &audioDecoderOpenSettings);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);

#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = platformConfig.outputs.audioDacs[0];
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    /* This is needed so that the transition between live and playback has no black frames */
    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture;
    videoDecoderSettings.firstPts.callback = setevent_callback;
    videoDecoderSettings.firstPts.context = firstPtsEvent;
    rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    BDBG_ASSERT(!rc);

    playpump = NEXUS_Playpump_Open(0, NULL);

    /* use stdio for file I/O to keep the example simple. */
    if (fname) {
        file = fopen(fname, "rb");
        if (!file) {
            fprintf(stderr, "can't open file:%s\n", fname);
            return -1;
        }

        NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
        playpumpSettings.dataCallback.callback = setevent_callback;
        playpumpSettings.dataCallback.context = playpumpEvent;
        NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
        NEXUS_Playpump_Start(playpump);
        commercialVideoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, commercialVideoPid, NULL);
        commercialAudioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, commercialAudioPid, NULL);

        /* get the playback buffer for the commercial insertion primed. this will reduce initial transition time. */
        pump(playpump, file);
    }

    /******************************************************
    * start live decode
    **/
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = liveVideoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = liveAudioPidChannel;
    audioProgram.stcChannel = stcChannel;

    NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = liveVideoPidChannel; /* PCR happens to be on video pid */
#ifdef IP_STREAMER_SUBST
    IpsAddStcSettings(ips, &stcSettings);
#endif    
    rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
    BDBG_ASSERT(!rc);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = videoDecoder;
    ipsStartSettings.aDecoder = audioDecoder;
    ipsStartSettings.stcChannel = stcChannel;
    IpsStart(ips, &ipsStartSettings);
#endif

    BKNI_ResetEvent(firstPtsEvent);

    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
    BDBG_ASSERT(!rc);

    if (!pts) {
        goto done;
    }

    /* wait for first picture to be delivered */
    BKNI_WaitForEvent(firstPtsEvent, 0xffffffff);
    pts = get_cur_pts(videoDecoder) + 45000 * pts;

    wait_for_pts(videoDecoder, pts, true);

    printf("starting commercial insertion\n");
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);

    NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* pvr */
    stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;
    rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
    BDBG_ASSERT(!rc);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = commercialVideoCodec;
    videoProgram.pidChannel = commercialVideoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = commercialAudioCodec;
    audioProgram.pidChannel = commercialAudioPidChannel;
    audioProgram.stcChannel = stcChannel;

    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
    BDBG_ASSERT(!rc);
    
    while (1) {
        if (!pump(playpump, file)) {
            break;
        }
        BKNI_WaitForEvent(playpumpEvent, 0xffffffff);
    }

    printf("waiting for commercial insertion to end\n");
    wait_for_clip_end(videoDecoder);

    printf("resuming live decode in muted state\n");
    
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = liveVideoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = liveAudioPidChannel;
    audioProgram.stcChannel = stcChannel;

    NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = liveVideoPidChannel; /* PCR happens to be on video pid */
#ifdef IP_STREAMER_SUBST
    IpsAddStcSettings(ips, &stcSettings);
#endif
    rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
    BDBG_ASSERT(!rc);

    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.mute = true;
    rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    BDBG_ASSERT(!rc);
    NEXUS_AudioDecoder_GetSettings(audioDecoder, &audioDecoderSettings);
    audioDecoderSettings.muted = true;
    rc = NEXUS_AudioDecoder_SetSettings(audioDecoder, &audioDecoderSettings);
    BDBG_ASSERT(!rc);

    BKNI_ResetEvent(firstPtsEvent);
    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
    BDBG_ASSERT(!rc);

    /* wait for first picture to be delivered */
    BKNI_WaitForEvent(firstPtsEvent, 0xffffffff);

    /* now wait for end point pts to occur. this ensures we don't see any bit of the replaced commercial. */
    wait_for_pts(videoDecoder, pts + (duration * 45000), false);

    printf("unmuting live decode\n");
    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.mute = false;
    rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    BDBG_ASSERT(!rc);
    NEXUS_AudioDecoder_GetSettings(audioDecoder, &audioDecoderSettings);
    audioDecoderSettings.muted = false;
    rc = NEXUS_AudioDecoder_SetSettings(audioDecoder, &audioDecoderSettings);
    BDBG_ASSERT(!rc);

done:
    /* loop forever */
    while (1);

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    return 0;
}

