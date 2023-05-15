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
 * $brcm_Workfile: decode_svc_mvc.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 6/25/12 3:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/decode_svc_mvc.c $
 * 
 * Hydra_Software_Devel/5   6/25/12 3:29p jtna
 * SW7422-170: decrease CDB size requirement
 * 
 * Hydra_Software_Devel/4   6/12/12 6:12p jtna
 * SW7425-2448: don't assume pcr pid = video pid
 * 
 * Hydra_Software_Devel/3   4/12/12 4:14p jtna
 * SW7425-2448: update app to do both svc and mvc decode
 * 
 * Hydra_Software_Devel/2   10/6/11 3:29p jtna
 * SW7422-170: updated decode_mvc app
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_hdmi_output.h"
#include "nexus_component_output.h"
#include "nexus_core_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstd.h"
#include "bkni.h"

typedef enum {
    mode_avc = 0,
    mode_svc,
    mode_svc3d,
    mode_mvc
} t_mode;

BDBG_MODULE(decode_svc_mvc);

#define VPID 0x31
#define EPID 0x32
#define APID 0x34
#define PCRPID 0x31

static void print_usage(const char* appname)
{
    fprintf(stderr, "Usage: nexus %s (avc/svc/svc3d/mvc)\n", appname);
}

int main(int argc, const char *argv[])  {
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, enhancePidChannel, audioPidChannel, pcrPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoFormat videoFormat;
    NEXUS_VideoCodec videoCodec = NEXUS_VideoCodec_eUnknown;
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_VideoDecoderExtendedSettings videoDecoderExtendedSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_Error rc;
    t_mode mode;
    int i;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    platformSettings.transportModuleSettings.maxDataRate.parserBand[parserBand] = 55*1024*1024;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    if (argc<2) {
        print_usage(argv[0]);
        return -1;
    }
    for (i=1; i<argc; i++) {
        if (strcmp(argv[i], "avc") == 0) {
            mode = mode_avc; 
            videoCodec = NEXUS_VideoCodec_eH264;
            videoFormat = NEXUS_VideoFormat_e720p; /* AVC defaults to 720p */
            break;
        }
        else if (strcmp(argv[i], "svc") == 0) {
            mode = mode_svc;
            videoCodec = NEXUS_VideoCodec_eH264_Svc;
            videoFormat = NEXUS_VideoFormat_e720p; /* not every TV supports 1080p. default to 720p */
            break;
        }
        else if (strcmp(argv[i], "svc3d") == 0) {
            mode = mode_svc3d;
            videoCodec = NEXUS_VideoCodec_eH264_Svc;
            videoFormat = NEXUS_VideoFormat_e3D_1080p_24Hz;
            break;
        }
        else if (strcmp(argv[i], "mvc") == 0) {
            mode = mode_mvc;
            videoCodec = NEXUS_VideoCodec_eH264_Mvc;
            videoFormat = NEXUS_VideoFormat_e3D_1080p_24Hz;
            break;
        }
        else {
            fprintf(stderr, "Unrecognized option: %s\n", argv[i]);
        }
    }
    if (videoCodec == NEXUS_VideoCodec_eUnknown) {
        print_usage(argv[0]);
        return -1;
    }

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, VPID, NULL);
    enhancePidChannel = NEXUS_PidChannel_Open(parserBand, EPID, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, APID, NULL);
    pcrPidChannel = NEXUS_PidChannel_Open(parserBand, PCRPID, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = pcrPidChannel;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder start structures */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = videoCodec;
    videoProgram.pidChannel = videoPidChannel;
    if (mode!=mode_avc) {
        videoProgram.enhancementPidChannel = enhancePidChannel;
    }
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.aspectRatio = NEXUS_DisplayAspectRatio_e4x3;
    displaySettings.format = videoFormat;
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
    if (videoFormatInfo.isFullRes3d) {
        displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eHdmiDvo; /* HDMI master mode is required */
    }
    display = NEXUS_Display_Open(0, &displaySettings);
    if (!display) {
        BDBG_ERR(("feature not supported"));
        return -1;
    }

    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));

    window = NEXUS_VideoWindow_Open(display, 0);

    /* Bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    videoDecoderOpenSettings.svc3dSupported = (mode==mode_svc3d);
    videoDecoderOpenSettings.fifoSize = 3 * 1024 * 1024; /* this is stream-dependent */
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    if (mode==mode_svc3d) {
        NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
        videoDecoderSettings.customSourceOrientation = true;
        videoDecoderSettings.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightFullFrame;
        NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);

        NEXUS_VideoDecoder_GetExtendedSettings(videoDecoder, &videoDecoderExtendedSettings);
        videoDecoderExtendedSettings.svc3dEnabled = true;
        NEXUS_VideoDecoder_SetExtendedSettings(videoDecoder, &videoDecoderExtendedSettings);
    }

    /* Start decoders */
    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    if (rc) {
        BDBG_ERR(("feature not supported"));
        return -1;
    }
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

#if 1
    while (1) {
        NEXUS_VideoDecoderStatus videoStatus;
        uint32_t stc;

        NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
        NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);
        BKNI_Printf("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            videoStatus.source.width, videoStatus.source.height, videoStatus.pts, stc, videoStatus.ptsStcDifference, videoStatus.fifoSize?(videoStatus.fifoDepth*100)/videoStatus.fifoSize:0);
        BKNI_Sleep(1000);
    }
#else
    printf("Press ENTER to stop decode\n");
    getchar();

    /* example shutdown */
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_PidChannel_Close(enhancePidChannel);
    NEXUS_PidChannel_Close(pcrPidChannel);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}
