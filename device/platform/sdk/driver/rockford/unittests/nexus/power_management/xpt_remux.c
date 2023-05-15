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
 * $brcm_Workfile: xpt_remux.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/15/11 6:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/power_management/xpt_remux.c $
 * 
 * Hydra_Software_Devel/2   2/15/11 6:00p jtna
 * SW7420-1320: update
 * 
 * Hydra_Software_Devel/1   1/18/11 4:18p jtna
 * SW7420-1320: added xpt_remux.c and xpt_packetsub.c
 * 
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
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
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_remux.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#define AUTOMATED_TEST 1
#define MAX_SLEEP_TIME 4000
#define LOOPS 8000

#define TEST_PROMPT(x)                           \
    if(x) {                                      \
        sleep = rand()%MAX_SLEEP_TIME;           \
        BKNI_Sleep(sleep);                       \
    }                                            \
    else {                                       \
        printf("Press ENTER to proceed\n");      \
        getchar();                               \
    }                                            \

#define DECODERS_START()                                            \
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);          \
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);            \
    if (audioProgram.codec == NEXUS_AudioCodec_eAc3) {              \
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram); \
    }
    
#define DECODERS_STOP()                                \
    NEXUS_VideoDecoder_Stop(videoDecoder);             \
    NEXUS_AudioDecoder_Stop(pcmDecoder);               \
    if (audioProgram.codec == NEXUS_AudioCodec_eAc3) { \
        NEXUS_AudioDecoder_Stop(compressedDecoder);    \
    }
    
int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand, parserBandRemux;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel, pcrPidChannel;
    NEXUS_PidChannelHandle videoPidChannelRemux, audioPidChannelRemux;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_RemuxHandle remux;
    NEXUS_RemuxSettings remuxSettings, oldRemuxSettings;
    unsigned sleep, loops;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

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
    pcrPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL); /* used for clock recovery */

    /* Configure remux */
    NEXUS_Remux_GetDefaultSettings(&remuxSettings);
    remuxSettings.insertNullPackets = false; /* this is needed to prevent the video from freezing when remux is toggled */
    remux = NEXUS_Remux_Open(0, &remuxSettings);

    NEXUS_Remux_AddPidChannel(remux, videoPidChannel);
    NEXUS_Remux_AddPidChannel(remux, audioPidChannel);

    /* Toggle all bool members in the remux settings. This will trigger a bunch of BXPT functions, and should not hang or fail */
    NEXUS_Remux_GetSettings(remux, &remuxSettings);
    NEXUS_Remux_GetSettings(remux, &oldRemuxSettings);
    remuxSettings.parallelOutput        = !remuxSettings.parallelOutput;
    remuxSettings.invertClock           = !remuxSettings.invertClock;
    remuxSettings.invertSync            = !remuxSettings.invertSync;
    remuxSettings.byteSync              = !remuxSettings.byteSync;
    remuxSettings.allPass               = !remuxSettings.allPass;
    remuxSettings.bypass                = !remuxSettings.bypass;
    remuxSettings.pcrCorrectionEnabled  = !remuxSettings.pcrCorrectionEnabled;
    remuxSettings.pacing                = !remuxSettings.pacing;
    remuxSettings.enablePcrJitterAdjust = !remuxSettings.enablePcrJitterAdjust;
    remuxSettings.insertNullPackets     = !remuxSettings.insertNullPackets;
    NEXUS_Remux_SetSettings(remux, &remuxSettings);
    NEXUS_Remux_SetSettings(remux, &oldRemuxSettings);

    NEXUS_Remux_Start(remux);

    /* Map remux output to a parser band via loopback route */
    parserBandRemux = NEXUS_ParserBand_e1;
    NEXUS_ParserBand_GetSettings(parserBandRemux, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eRemux;
    parserBandSettings.sourceTypeSettings.remux = remux;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBandRemux, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannelRemux = NEXUS_PidChannel_Open(parserBandRemux, 0x11, NULL);
    audioPidChannelRemux = NEXUS_PidChannel_Open(parserBandRemux, 0x14, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannelRemux;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = audioPidChannelRemux;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if (audioProgram.codec == NEXUS_AudioCodec_eAc3) {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else {
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }

    /* Bring up display and outputs */
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
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if(platformConfig.outputs.component[0]){
        NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    }
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS    
    if (platformConfig.outputs.svideo[0]) {
        NEXUS_Display_AddOutput(display, NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[0]));
    }
#endif    
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    /*goto test2;*/

    /* For Test1, use the pidchannels from the inputband, not the remux */
    videoProgram.pidChannel = videoPidChannel;
    audioProgram.pidChannel = audioPidChannel;
    DECODERS_START();

    /* Test1: toggle remux, while decoders are running from the inputband. The decoders should keep going with no interruption */
    loops = LOOPS;
    while (loops--) {
        TEST_PROMPT(AUTOMATED_TEST);
        NEXUS_Remux_Stop(remux);
        /* Remux clocks are down at this point */
        printf("Test1: power down (loops %4d)\n", loops);
        TEST_PROMPT(AUTOMATED_TEST);
        NEXUS_Remux_Start(remux);
    }

test2:
    /* now run the decoders from the remux */
    DECODERS_STOP();
    videoProgram.pidChannel = videoPidChannelRemux;
    audioProgram.pidChannel = audioPidChannelRemux;
    DECODERS_START();

    /* Test2: toggle remux, while decoders are running from the inputband. Video will freeze each time remux is stopped. Macroblocking is expected when the remux is restarted */
    loops = LOOPS;
    while (loops--) {
        TEST_PROMPT(AUTOMATED_TEST);
        NEXUS_Remux_Stop(remux);
        /* Remux clocks are down at this point */
        printf("Test2: power down (loops %4d)\n", loops);
        TEST_PROMPT(AUTOMATED_TEST);
        NEXUS_Remux_Start(remux);
    }
    
    /* Test3: stop decoders, stop remux, (remux clocks down), start remux, start decoders. repeat */
    loops = LOOPS;
    while (loops--) {
        TEST_PROMPT(AUTOMATED_TEST);
        /* Stop the decoders. This calls BXPT_Rave_DisableContext */
        DECODERS_STOP();
        /* RAVE refcnt is 0 at this point */

        NEXUS_Remux_Stop(remux);
        /* Remux clocks are down at this point */

        /* Close the pidchannels. This calls BXPT_SetParserEnable and BXPT_DisablePidChannel.
           BXPT_DisablePidChannel doesn't affect power management */
        NEXUS_PidChannel_Close(videoPidChannel);
        NEXUS_PidChannel_Close(audioPidChannel);

        /* PARSER refcnt is 0 at this point. This should shut down the 216M */
        printf("Test3: power down (loops %4d)\n", loops);
        
        TEST_PROMPT(AUTOMATED_TEST);

        videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
        audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
        videoProgram.pidChannel = videoPidChannel;
        audioProgram.pidChannel = audioPidChannel;

        NEXUS_Remux_Start(remux);

        DECODERS_START();
    }

    /* Test4: stop decoders, stop remux, (remux clocks down), close pidchannels, (216M down), open pidchannels, start remux, start decoders. repeat */
    loops = LOOPS;
    while (loops--) {
        
        TEST_PROMPT(AUTOMATED_TEST);

        /* Stop the decoders. This calls BXPT_Rave_DisableContext */
        DECODERS_STOP();
        /* RAVE refcnt is 0 at this point */

        NEXUS_Remux_Stop(remux);
        /* Remux clocks are down at this point */

        /* Close the pidchannels. This calls BXPT_SetParserEnable and BXPT_DisablePidChannel.
           BXPT_DisablePidChannel doesn't affect power management */
        NEXUS_PidChannel_Close(videoPidChannel);
        NEXUS_PidChannel_Close(audioPidChannel);

        /* PARSER refcnt is 0 at this point. This should shut down the 216M */
        printf("Test4: power down (loops %4d)\n", loops);
        
        TEST_PROMPT(AUTOMATED_TEST);
        videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
        audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
        videoProgram.pidChannel = videoPidChannel;
        audioProgram.pidChannel = audioPidChannel;

        NEXUS_Remux_Start(remux);

        DECODERS_START();
    }

    /* Test5: close pidchannels, stop decoders, (216M down), stop remux, (remux clocks down), open pidchannels, start remux, start decoders. repeat. */
    loops = LOOPS;
    while (loops--) {
        
        TEST_PROMPT(AUTOMATED_TEST);

        /* Close the pidchannels. This calls BXPT_SetParserEnable and BXPT_DisablePidChannel.
           BXPT_DisablePidChannel doesn't affect power management */
        NEXUS_PidChannel_Close(videoPidChannel);
        NEXUS_PidChannel_Close(audioPidChannel);
        /* PARSER refcnt is 0 at this point */

        /* Stop the decoders. This calls BXPT_Rave_DisableContext */
        DECODERS_STOP();
        /* RAVE refcnt is 0 at this point. This should shut down the 216M */

        NEXUS_Remux_Stop(remux);
        /* Remux clocks are down at this point */

        printf("Test5: power down (loops %4d)\n", loops);
        
        TEST_PROMPT(AUTOMATED_TEST);

        videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
        audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
        videoProgram.pidChannel = videoPidChannel;
        audioProgram.pidChannel = audioPidChannel;

        NEXUS_Remux_Start(remux);

        DECODERS_START();
    }

    /* shutdown */
    DECODERS_START();
    NEXUS_Remux_Stop(remux);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_Remux_Close(remux);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_PidChannel_Close(pcrPidChannel);
    NEXUS_PidChannel_Close(videoPidChannelRemux);
    NEXUS_PidChannel_Close(audioPidChannelRemux);
    NEXUS_Platform_Uninit();

    return 0;
}
