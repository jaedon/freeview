/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: decode_ipstream.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 11/30/11 4:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/decode_ipstream.c $
 * 
 * Hydra_Software_Devel/4   11/30/11 4:16p jtna
 * SW7405-4289: update comment
 * 
 * Hydra_Software_Devel/3   11/30/11 3:19p jtna
 * SW7405-4289: remove old DTV code. install decodeError callback.
 * 
 * Hydra_Software_Devel/2   1/7/11 3:34p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 * PTS - STC difference
 * 
 * Hydra_Software_Devel/1   5/3/10 5:52p jtna
 * SW7405-4289: added decode_ipstream.c
 * 
******************************************************************************/
/* nexus example app: bare minimum needed to do IP streaming over "live mode" (UDP/RTP) */

#include "b_os_lib.h"
#include "b_playback_ip_lib.h"

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_playpump.h"
#include "nexus_timebase.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

#define IP_NETWORK_JITTER 300

#define APPSETTING_IPADDRESS   "224.1.1.21" /* multicast range is [224.0.0.0, 239.255.255.255] */
#define APPSETTING_PORT        1234
#define APPSETTING_PROTOCOL    B_PlaybackIpProtocol_eRtp

unsigned g_vpid = 0x21;
unsigned g_apid = 0x24;

void decodeError(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    BSTD_UNUSED(videoDecoder);
    BSTD_UNUSED(param);
    BKNI_Printf("decode error\n");
}

int main(int argc, char *argv[])
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaypumpOpenPidChannelSettings pidChannelSettings;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_AudioOutput audioOutput;
    NEXUS_AudioOutputSettings audioOutputSettings;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_AudioDecoderSettings audioDecoderSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle pcmDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_Error rc;

    B_PlaybackIpHandle playbackIp;
    B_PlaybackIpSessionOpenSettings ipSessionOpenSettings;
    B_PlaybackIpSessionOpenStatus ipSessionOpenStatus;
    B_PlaybackIpSessionStartSettings ipSessionStartSettings;
    B_PlaybackIpSessionStartStatus ipSessionStartStatus;

    if (argc>3) {
        fprintf(stderr, "Usage: nexus %s videopid audiopid\n", argv[0]);
        exit(1);
    }
    if (argc>1) {
        unsigned i = 1;
        g_vpid = strtoul(argv[i++], NULL, 0);
        g_apid = strtoul(argv[i++], NULL, 0);
    }

    /* App must initialize OS lib. Required by B_PlaybackIp_SessionStart() */
    B_Os_Init();

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if 0 /* this was the streamer code, which this example app replaces */
    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
#endif

    /* for IP "live mode" (e.g. UDP/RTP),  IP applib feeds network data directly to playpump. playpump must be set to FIFO mode (default) */
    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);

    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidChannelSettings);
    pidChannelSettings.pidType = NEXUS_PidType_eVideo;
    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, g_vpid, &pidChannelSettings);

    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidChannelSettings);
    pidChannelSettings.pidType = NEXUS_PidType_eAudio;
    pidChannelSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eAc3;
    audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, g_apid, &pidChannelSettings);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    /* offset threshold: uses upper 32 bits (183ticks/msec) of PCR clock */
    stcSettings.modeSettings.pcr.offsetThreshold = IP_NETWORK_JITTER * 183; 
    /* max pcr error: uses upper 32 bits (183ticks/msec) of PCR clock */
    stcSettings.modeSettings.pcr.maxPcrError = 0xFFFFFFFF; /* Settop API had it set to this value */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    /*  PCR Offset "Jitter Adjustment" is not suitable for use with IP channels Channels, so disable it */
    stcSettings.modeSettings.pcr.disableJitterAdjustment = true;
    /* Disable Auto Timestamp correction for PCR Jitter */
    stcSettings.modeSettings.pcr.disableTimestampCorrection = true;
    /* We just configured the Timebase, so turn off auto timebase config */
    stcSettings.autoConfigTimebase = false;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder A/V programs */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
    audioDecoderOpenSettings.fifoSize = 512 * 1024; /* increased CDB for dejitter buffer */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, &audioDecoderOpenSettings);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    /* Bring up display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    videoDecoderOpenSettings.fifoSize = 5 * 1024 * 1024; /* increased CDB for dejitter buffer */
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* watch for decode errors. they do not / should not occur with this example. 
       however, using ip_streamer_subst, decode errors appear to be unavoidable, 
       possibly due to ip_playback -> remux -> parserband flow of data */
    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.decodeError.callback = decodeError;
    videoDecoderSettings.decodeError.context = videoDecoder;
    rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    BDBG_ASSERT(!rc);

    /* To absorb high IP network jitter:
       - DPCR & PCR offset blocks are programmed w/ increased thresholds for errors 
       - Both display & audio outputs are decoupled from input time base & run from a free clock
       - A/V CDBs sizes are increased to absorb the network jitter
       - A/V decodes are delayed by the amount of jitter buffer depth */

    /* program timebase 0: increase track range & max pcr errors */
    NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
    timebaseSettings.sourceSettings.pcr.pidChannel = videoPidChannel;
    timebaseSettings.sourceSettings.pcr.maxPcrError = IP_NETWORK_JITTER * 91; /* in milliseconds: based on 90Khz clock */
    timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e244ppm;
    rc = NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);
    BDBG_ASSERT(!rc);

    /* decouple display and audio output from input timebase */
    NEXUS_Timebase_GetSettings(NEXUS_Timebase_e1, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
    rc = NEXUS_Timebase_SetSettings(NEXUS_Timebase_e1, &timebaseSettings);
    BDBG_ASSERT(!rc);

    audioOutput = NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]);
    NEXUS_AudioOutput_GetSettings(audioOutput, &audioOutputSettings);
    audioOutputSettings.timebase = NEXUS_Timebase_e1;
    rc = NEXUS_AudioOutput_SetSettings(audioOutput, &audioOutputSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetSettings(display, &displaySettings);
    displaySettings.timebase = NEXUS_Timebase_e1;
    rc = NEXUS_Display_SetSettings(display, &displaySettings);
    BDBG_ASSERT(!rc);

    /*  delay the start of decoder as per the network jitter by increasing the PTS Offset */
    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45; /* in 45Khz clock */
    rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    BDBG_ASSERT(!rc);
        
    NEXUS_AudioDecoder_GetSettings(pcmDecoder, &audioDecoderSettings);
    audioDecoderSettings.ptsOffset = IP_NETWORK_JITTER * 45;
    rc = NEXUS_AudioDecoder_SetSettings(pcmDecoder, &audioDecoderSettings);
    BDBG_ASSERT(!rc);


    /* Open IP applib and start a session */
    playbackIp = B_PlaybackIp_Open(NULL);
    BDBG_ASSERT(playbackIp);

    BKNI_Memset(&ipSessionOpenSettings, 0, sizeof(ipSessionOpenSettings));
    BKNI_Memset(&ipSessionOpenStatus, 0, sizeof(ipSessionOpenStatus));
    strncpy(ipSessionOpenSettings.socketOpenSettings.ipAddr, APPSETTING_IPADDRESS, sizeof(ipSessionOpenSettings.socketOpenSettings.ipAddr)-1);
    ipSessionOpenSettings.socketOpenSettings.port = APPSETTING_PORT;
    ipSessionOpenSettings.socketOpenSettings.protocol = APPSETTING_PROTOCOL;
    ipSessionOpenSettings.ipMode = B_PlaybackIpClockRecoveryMode_ePushWithPcrSyncSlip;
    ipSessionOpenSettings.maxNetworkJitter = 300;
    ipSessionOpenSettings.networkTimeout = 1;  /* timeout in 1 sec during network outage events */
    rc = B_PlaybackIp_SessionOpen(playbackIp, &ipSessionOpenSettings, &ipSessionOpenStatus);
    BDBG_ASSERT(!rc);

    BKNI_Memset(&ipSessionStartSettings, 0, sizeof(ipSessionStartSettings));
    ipSessionStartSettings.nexusHandles.playpump = playpump;
    ipSessionStartSettings.nexusHandles.videoDecoder = videoDecoder;
    ipSessionStartSettings.nexusHandles.stcChannel = stcChannel;
    ipSessionStartSettings.nexusHandles.primaryAudioDecoder = pcmDecoder;
    ipSessionStartSettings.nexusHandlesValid = true;
    rc = B_PlaybackIp_SessionStart(playbackIp, &ipSessionStartSettings, &ipSessionStartStatus);
    while (rc == B_ERROR_IN_PROGRESS) { /* alternatively, we could wait for the callback */
        BKNI_Sleep(100);
        rc = B_PlaybackIp_SessionStart(playbackIp, &ipSessionStartSettings, &ipSessionStartStatus);
    }

    /* Start playpump and decoders */
    rc = NEXUS_Playpump_Start(playpump);
    BDBG_ASSERT(!rc);

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);

#if 1
    /* Print status while decoding */
    for (;;) {
        NEXUS_VideoDecoderStatus status;
        NEXUS_AudioDecoderStatus audioStatus;
        uint32_t stc;

        NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
        NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);
        printf("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference, status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0);
        NEXUS_AudioDecoder_GetStatus(pcmDecoder, &audioStatus);
        printf("audio0            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
        BKNI_Sleep(1000);
    }
#else
    printf("Press ENTER to stop decode\n");
    getchar();

    /* example shutdown */
    NEXUS_AudioDecoder_Stop(pcmDecoder);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}

