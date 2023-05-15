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
 * $brcm_Workfile: mtsif_mapping.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/15/12 5:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/frontend/mtsif_mapping.c $
 * 
 * Hydra_Software_Devel/2   5/15/12 5:45p jtna
 * SW7425-2752: fix warnings, add extra test case
 * 
 * Hydra_Software_Devel/1   5/10/12 6:55p jtna
 * SW7425-2752: add mtsif_mapping.c unittest
 * 
 *****************************************************************************/

#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
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
#include "nexus_core_utils.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(tune_qam);

/* the following define the input and its characteristics -- these will vary by input stream */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x11
#define AUDIO_PID 0x14

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendQamStatus qamStatus;

    BSTD_UNUSED(param);

    fprintf(stderr, "Lock callback, frontend 0x%08x\n", (unsigned)frontend);

#if 0 /* appears to cause a problem in certain situations */
    NEXUS_Frontend_GetQamStatus(frontend, &qamStatus);
    fprintf(stderr, "QAM Lock callback, frontend 0x%08x - lock status %d, %d\n", (unsigned)frontend,
            qamStatus.fecLock, qamStatus.receiverLock);
#else
    BSTD_UNUSED(qamStatus);
#endif
}

typedef struct frontend_caps_t {
    bool isMtsif;
    bool qam;
    bool oob;
} frontend_caps_t;

NEXUS_VideoDecoderHandle videoDecoder;
NEXUS_AudioDecoderHandle audioDecoder;
NEXUS_VideoDecoderStartSettings videoProgram;
NEXUS_AudioDecoderStartSettings audioProgram;
NEXUS_StcChannelHandle stcChannel;
NEXUS_StcChannelSettings stcSettings;
NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
bool avStarted = false;
#define min(A,B) ((A)<(B)?(A):(B))

void start_av(NEXUS_ParserBand parserBand)
{
    NEXUS_Error rc;

    BDBG_ASSERT(avStarted==false);

    videoPidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, AUDIO_PID, NULL);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = AUDIO_CODEC;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
    BDBG_ASSERT(!rc);
    avStarted = true;
}

int check_av(void)
{
    NEXUS_VideoDecoderStatus status;
    NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
    return status.numDecoded;
}

void stop_av(void)
{
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_VideoDecoder_Flush(videoDecoder);

#if 1
    while (check_av()!=0) {
        BKNI_Printf("decoder status not reset...\n");
        NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
        NEXUS_VideoDecoder_Stop(videoDecoder);
        NEXUS_VideoDecoder_Flush(videoDecoder);
        BKNI_Sleep(500);
    }
#endif

    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    avStarted = false;
}

int main(int argc, char **argv)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendQamSettings qamSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    NEXUS_Error rc;
    unsigned i;
    unsigned freq = 765;
    NEXUS_FrontendQamMode qamMode = NEXUS_FrontendQamMode_e64;
    frontend_caps_t caps[NEXUS_MAX_FRONTENDS];
    unsigned frontendIndex = 0;
    const unsigned count = min(NEXUS_MAX_FRONTENDS, NEXUS_MAX_PARSER_BANDS);

    if (argc > 1) {
        frontendIndex = atoi(argv[1]);
    }
    if (argc > 2) {
        parserBand = NEXUS_ParserBand_e0 + atoi(argv[2]);
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = true;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) { return -1; }

    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_Memset(&caps, 0, sizeof(caps));
    
    for (i=0; i<NEXUS_MAX_FRONTENDS; i++) {
        NEXUS_FrontendCapabilities capabilities;
        frontend = platformConfig.frontend[i];
        if (frontend) {
            NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
            NEXUS_Frontend_GetUserParameters(frontend, &userParams);
            BKNI_Printf("frontend[%02u]: chip %#x, rev %#x, id %#x, param %u, qam %u, mtsif %u\n", i, 
                userParams.chipId, userParams.chipRevision, userParams.id, userParams.param1, capabilities.qam, userParams.isMtsif);
            caps[i].isMtsif = userParams.isMtsif;
            caps[i].qam = capabilities.qam;
            caps[i].oob = capabilities.outOfBand;
        }
        else {
            BKNI_Printf("frontend[%02u]: NULL\n");
        }        
    }

    NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
    qamSettings.frequency = freq * 1000000;
    qamSettings.mode = qamMode;
    switch (qamMode) {
    default:
    case NEXUS_FrontendQamMode_e64: qamSettings.symbolRate = 5056900; break;
    case NEXUS_FrontendQamMode_e256: qamSettings.symbolRate = 5360537; break;
    case NEXUS_FrontendQamMode_e1024: qamSettings.symbolRate = 0; /* TODO */ break;
    }
    qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
    qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e6Mhz;
#if 0
    qamSettings.lockCallback.callback = lock_callback;
    qamSettings.lockCallback.context = frontend;
#else
    BSTD_UNUSED(lock_callback);
#endif
  
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif

    /* bring up display */
    NEXUS_Platform_GetConfiguration(&platformConfig);
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif

#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format.
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    }
#endif

    window = NEXUS_VideoWindow_Open(display, 0);
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* "manual" mode */
#if 1
    BSTD_UNUSED(count);
    while (1) {
        BKNI_Printf("using frontend%u, host PB%u\n", frontendIndex, parserBand);
        frontend = platformConfig.frontend[frontendIndex];

        NEXUS_ParserBand_GetSettings((NEXUS_ParserBand)parserBand, &parserBandSettings);
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
        parserBandSettings.sourceTypeSettings.mtsif = frontend;
        NEXUS_ParserBand_SetSettings((NEXUS_ParserBand)parserBand, &parserBandSettings);

        qamSettings.lockCallback.context = frontend;
        rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
        BDBG_ASSERT(!rc);
        
        start_av((NEXUS_ParserBand)parserBand);
        BKNI_Printf("Press enter to tune again\n");

        getchar();
        stop_av();

        #if 1 /* optional if we're re-using same mapping */
        NEXUS_Frontend_Untune(frontend);
        #endif

        #if 0 /* extra mappings */
        frontendIndex++;
        parserBand += 2;
        #endif
    }
#endif


    /* 1. tune without any MTSIF connection, then start video. the tune should pass, but no data should be coming through */
#if 0
    for (i=0; i<NEXUS_MAX_FRONTENDS; i++) { /* tune all frontends first */
        frontend = platformConfig.frontend[i];
        if (caps[i].isMtsif && caps[i].qam) {
            BKNI_Printf("tune frontend %u\n", i);
            qamSettings.lockCallback.context = frontend;
            rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
            BDBG_ASSERT(!rc);
            BKNI_Sleep(3000); /* HAB craps out without a delay in-between */
        }
    }

    for (i=0; i<NEXUS_MAX_PARSER_BANDS; i++) {
        BKNI_Printf("decode on PB %u\n", i);
        start_av((NEXUS_ParserBand)i);
        BKNI_Sleep(3000);
        BDBG_ASSERT(check_av()==0);
        stop_av();
    }
#endif

    /* 2. do a straight-forward 1-1 mapping, tune, then start video. do not untune the previous tuner. we should see video from all parsers */
#if 0
    for (i=0; i<count; i++) {
        frontend = platformConfig.frontend[i];
        if (caps[i].isMtsif && caps[i].qam) {
            NEXUS_ParserBand_GetSettings((NEXUS_ParserBand)i, &parserBandSettings);
            parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
            parserBandSettings.sourceTypeSettings.mtsif = frontend;
            NEXUS_ParserBand_SetSettings((NEXUS_ParserBand)i, &parserBandSettings);
            
            BKNI_Printf("tune frontend %u\n", i);
            qamSettings.lockCallback.context = frontend;
            rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
            BDBG_ASSERT(!rc);

            BKNI_Printf("decode on PB %u\n", i);
            start_av((NEXUS_ParserBand)i);
            BKNI_Sleep(3000);
            BDBG_ASSERT(check_av()>0);
            stop_av();
        }
    }

    for (i=0; i<count; i++) {
        frontend = platformConfig.frontend[i];
        NEXUS_Frontend_Untune(frontend);
        BKNI_Sleep(3000);
    }
#endif

    /* 3. repeat the previous test, but untune the previous tuner */
#if 0
    for (i=0; i<count; i++) {
        frontend = platformConfig.frontend[i];
        if (caps[i].isMtsif && caps[i].qam) {
            if (i>0) { /* check video on previous PB. since we untuned, there should not be video */
                start_av((NEXUS_ParserBand)i-1);
                BKNI_Sleep(3000);
                BDBG_ASSERT(check_av()==0);
                stop_av();
            }
            
            NEXUS_ParserBand_GetSettings((NEXUS_ParserBand)i, &parserBandSettings);
            parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
            parserBandSettings.sourceTypeSettings.mtsif = frontend;
            NEXUS_ParserBand_SetSettings((NEXUS_ParserBand)i, &parserBandSettings);
            
            BKNI_Printf("tune frontend %u\n", i);
            qamSettings.lockCallback.context = frontend;
            rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
            BDBG_ASSERT(!rc);

            BKNI_Printf("decode on PB %u\n", i);
            start_av((NEXUS_ParserBand)i);
            BKNI_Sleep(3000);
            BDBG_ASSERT(check_av()>0);
            stop_av();

            NEXUS_Frontend_Untune(frontend);
        }
    }
#endif

    /* 4. do an inverse mapping (e.g. frontend[15] to PB0) */
#if 0
    for (i=count-1; ; i--) {
        unsigned j = count-1-i;
        frontend = platformConfig.frontend[i];
        if (caps[i].isMtsif && caps[i].qam) {
            NEXUS_ParserBand_GetSettings((NEXUS_ParserBand)j, &parserBandSettings);
            parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
            parserBandSettings.sourceTypeSettings.mtsif = frontend;
            NEXUS_ParserBand_SetSettings((NEXUS_ParserBand)j, &parserBandSettings);
            
            BKNI_Printf("tune frontend %u\n", i);
            qamSettings.lockCallback.context = frontend;
            rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
            BDBG_ASSERT(!rc);

            BKNI_Printf("decode on PB %u\n", j);
            start_av((NEXUS_ParserBand)j);
            BKNI_Sleep(3000);
            BDBG_ASSERT(check_av()>0);
            stop_av();
        }
        if (i==0) { break; }
    }

    for (i=0; i<count; i++) {
        frontend = platformConfig.frontend[i];
        NEXUS_Frontend_Untune(frontend);
        BKNI_Sleep(3000);
    }
#endif

    /* 5. 1-to-many connection, i.e. one frontend to many parserbands */
#if 0
    frontend = platformConfig.frontend[frontendIndex];
    BDBG_ASSERT(caps[frontendIndex].isMtsif && caps[frontendIndex].qam);
   
    for (i=0; i<NEXUS_NUM_PARSER_BANDS/2; i++) {
        NEXUS_ParserBand_GetSettings((NEXUS_ParserBand)i, &parserBandSettings);
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
        parserBandSettings.sourceTypeSettings.mtsif = frontend;
        NEXUS_ParserBand_SetSettings((NEXUS_ParserBand)i, &parserBandSettings);
    }

    BKNI_Printf("tune frontend %u\n", i);
    qamSettings.lockCallback.context = frontend;
    rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
    BDBG_ASSERT(!rc);

    for (i=0; i<NEXUS_NUM_PARSER_BANDS/2; i++) {
        BKNI_Printf("decode on PB %u\n", i);
        start_av((NEXUS_ParserBand)i);
        BKNI_Sleep(3000);
        BDBG_ASSERT(check_av()>0);
        stop_av();
    }
    NEXUS_Frontend_Untune(frontend);
#endif

    BKNI_Printf("success\n");
    
    return 0;
}
