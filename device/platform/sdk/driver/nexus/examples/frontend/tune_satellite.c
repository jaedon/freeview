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
 * $brcm_Workfile: tune_satellite.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 10/8/12 1:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/frontend/tune_satellite.c $
 * 
 * 14   10/8/12 1:08p erickson
 * SW7425-3782: rework mtsif
 * 
 * 13   8/7/12 5:37p mphillip
 * SW7231-924: Merge NEXUS_Frontend_Acquire changes to main
 * 
 * cng_unittest/1   7/30/12 5:00p cng
 * SW7231-924: Use NEXUS_Frontend_Acquire
 * 
 * 12   5/10/12 7:31p jtna
 * SW7425-2752: update nexus examples to use
 *  NEXUS_ParserBandSourceType_eMtsif for MTSIF-based frontends
 * 
 * 11   1/25/12 4:59p rjlewis
 * SW7425-1136: Added HDMI support.
 * 
 * 10   3/22/10 2:35a randyjew
 * SW7468-143: Support for boards without component and/or composite
 * 
 * 9   7/13/09 4:48p erickson
 * PR56773: added NEXUS_Frontend_GetDiseqcStatus
 *
 * 8   4/1/09 4:00p erickson
 * PR53078: clean up app
 *
 * 7   2/18/09 2:01p erickson
 * PR51841: remove examples_lib
 *
 * 5   8/5/08 1:14p erickson
 * PR45425: don't assume straight IB->PB mapping in example apps.
 *  explicitly map.
 *
 * 4   4/4/08 4:02p erickson
 * PR35551: separate qam and vsb tune examples
 *
 * 2   2/12/08 2:33p jgarrett
 * PR 39009: Unifying status values
 *
 *****************************************************************************/
/* Example to tune a satellite channel using nexus */

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
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USE_DVB 1 /* else use DSS */

#define PARSER_BAND NEXUS_ParserBand_e2; /* Change this value to match your platform */

/* the following define the input and its characteristics -- these will vary by input type */
#ifdef USE_DVB
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x31
#define AUDIO_PID 0x34
#define FREQ 1119000000
#define SATELLITE_MODE NEXUS_FrontendSatelliteMode_eDvb
#define TONE_MODE true
#define VOLTAGE NEXUS_FrontendDiseqcVoltage_e13v
#else
#define TRANSPORT_TYPE NEXUS_TransportType_eDssEs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eMpeg
#define VIDEO_PID 0x78
#define AUDIO_PID 0x79
#define FREQ 1396820000
#define SATELLITE_MODE NEXUS_FrontendSatelliteMode_eDss
#define TONE_MODE false
#define VOLTAGE NEXUS_FrontendDiseqcVoltage_e18v
#endif

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendSatelliteStatus status;
    NEXUS_FrontendDiseqcStatus disqecStatus;

    BSTD_UNUSED(param);

    fprintf(stderr, "Frontend(%p) - lock callback\n", (void*)frontend);

    NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
    fprintf(stderr, "  demodLocked = %d\n", status.demodLocked);
    NEXUS_Frontend_GetDiseqcStatus(frontend, &disqecStatus);
    fprintf(stderr, "  diseqc tone = %d, voltage = %d\n", disqecStatus.toneEnabled, disqecStatus.voltage);
}

int main(void)
{
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendDiseqcSettings diseqcSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendAcquireSettings settings;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_AudioDecoderHandle audioDecoder;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    NEXUS_Error rc;

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
    settings.capabilities.satellite = true;
    frontend = NEXUS_Frontend_Acquire(&settings);
    if (!frontend) {
        fprintf(stderr, "Unable to find satellite-capable frontend\n");
        return -1;
    }

    NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
    satSettings.frequency = FREQ;
    satSettings.mode = SATELLITE_MODE;
    satSettings.lockCallback.callback = lock_callback;
    satSettings.lockCallback.context = frontend;
    NEXUS_Frontend_GetUserParameters(frontend, &userParams);

    /* Map a parser band to the demod's input band. */
    parserBand = PARSER_BAND;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    if (userParams.isMtsif) {
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
        parserBandSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(frontend); /* NEXUS_Frontend_TuneXyz() will connect this frontend to this parser band */
    }
    else {
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    }
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    NEXUS_Frontend_GetDiseqcSettings(frontend, &diseqcSettings);
    diseqcSettings.toneEnabled = TONE_MODE;
    diseqcSettings.voltage = VOLTAGE;
    NEXUS_Frontend_SetDiseqcSettings(frontend, &diseqcSettings);
    printf("Set DiseqcSettings\n");

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

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
       NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
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

    while (1) {
        NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
        NEXUS_VideoDecoderStartSettings videoProgram;
        NEXUS_AudioDecoderStartSettings audioProgram;

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

        rc = NEXUS_Frontend_TuneSatellite(frontend, &satSettings);
        BDBG_ASSERT(!rc);

        while (1)
        {
            NEXUS_FrontendSatelliteStatus satStatus;
            rc = NEXUS_Frontend_GetSatelliteStatus(frontend, &satStatus);
            if (rc) {
                printf("unable to read status\n");
            }
            else {
                printf(
                    "Sat Status\n"
                    "  symbolRate %d\n"
                    "  locked  %d\n",
                        satStatus.settings.symbolRate,
                        satStatus.demodLocked);
                if(satStatus.demodLocked)
                    break;
            }
            BKNI_Sleep(1000);
        }

        NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
        NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

        printf("Press enter to tune again\n");
        getchar();

        NEXUS_AudioDecoder_Stop(audioDecoder);
        NEXUS_VideoDecoder_Stop(videoDecoder);
        NEXUS_PidChannel_Close(videoPidChannel);
        NEXUS_PidChannel_Close(audioPidChannel);
    }

    return 0;
}
