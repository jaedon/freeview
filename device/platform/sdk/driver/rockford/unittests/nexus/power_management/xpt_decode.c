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
 * $brcm_Workfile: xpt_decode.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 2/15/11 6:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/power_management/xpt_decode.c $
 * 
 * Hydra_Software_Devel/3   2/15/11 6:00p jtna
 * SW7420-1320: update
 * 
 * Hydra_Software_Devel/2   1/12/11 3:59p jtna
 * SW7420-1320: fix shutdown
 * 
 * Hydra_Software_Devel/1   1/12/11 12:12p jtna
 * SW7420-1320: add power_management unittests
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
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

/* 4000 sleep and 5000 loops is about 5.5 hours per while-loop */
#define AUTOMATED_TEST 1
#define MAX_SLEEP_TIME 4000
#define LOOPS (5000*10)
#define EXTRA_LOGS 1 /* extra debug logs */

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
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
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
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
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }

    /* At this point, only the 216M and 108M XPT clocks are running */

test1:
    /* Test1: stop decoders, close pidchannels, (216M down), open pidchannels, start decoders. repeat */
    loops = LOOPS;
    while (loops--) {
        
#if AUTOMATED_TEST
        sleep = rand()%MAX_SLEEP_TIME;
        BKNI_Sleep(sleep);
#else
        printf("Press ENTER to stop decode\n");
        getchar();
#endif

        /* Stop the decoders. This calls BXPT_Rave_DisableContext */
        NEXUS_AudioDecoder_Stop(pcmDecoder);
        if (audioProgram.codec == NEXUS_AudioCodec_eAc3) {
            NEXUS_AudioDecoder_Stop(compressedDecoder);
        }
        NEXUS_VideoDecoder_Stop(vdecode);
        /* RAVE refcnt is 0 at this point */
#if EXTRA_LOGS
        printf("Decoders stopped\n");
#endif

        /* Close the pidchannels. This calls BXPT_SetParserEnable and BXPT_DisablePidChannel.
           BXPT_DisablePidChannel doesn't affect power management */
        NEXUS_PidChannel_Close(videoPidChannel);
        NEXUS_PidChannel_Close(audioPidChannel);

        /* PARSER refcnt is 0 at this point. This should shut down the 216M */
        printf("Test1: power down (loops %4d)\n", loops);

#if AUTOMATED_TEST
        sleep = rand()%MAX_SLEEP_TIME;
        BKNI_Sleep(sleep);
#else
        printf("Press ENTER to start decode\n");
        getchar();
#endif

        videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
        audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
        videoProgram.pidChannel = videoPidChannel;
        audioProgram.pidChannel = audioPidChannel;
#if EXTRA_LOGS
        printf("PidChannels opened\n");
#endif

        NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
        NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
        if (audioProgram.codec == NEXUS_AudioCodec_eAc3) {
            NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
        }
#if EXTRA_LOGS
        printf("Decoders started\n");
#endif
    }

test2:
    /* Test2: close pidchannels, stop decoders, (216M down), open pidchannels, start decoders. repeat. */
    loops = LOOPS;
    while (loops--) {
        
#if AUTOMATED_TEST
        sleep = rand()%MAX_SLEEP_TIME;
        BKNI_Sleep(sleep);
#else
        printf("Press ENTER to stop decode\n");
        getchar();
#endif

        /* Close the pidchannels. This calls BXPT_SetParserEnable and BXPT_DisablePidChannel.
           BXPT_DisablePidChannel doesn't affect power management */
        NEXUS_PidChannel_Close(videoPidChannel);
        NEXUS_PidChannel_Close(audioPidChannel);
        /* PARSER refcnt is 0 at this point */
#if EXTRA_LOGS
        printf("PidChannels closed\n");
#endif

        /* Stop the decoders. This calls BXPT_Rave_DisableContext */
        NEXUS_AudioDecoder_Stop(pcmDecoder);
        if (audioProgram.codec == NEXUS_AudioCodec_eAc3) {
            NEXUS_AudioDecoder_Stop(compressedDecoder);
        }
        NEXUS_VideoDecoder_Stop(vdecode);
        /* RAVE refcnt is 0 at this point. This should shut down the 216M */

        printf("Test2: power down (loops %4d)\n", loops);

#if AUTOMATED_TEST
        sleep = rand()%MAX_SLEEP_TIME;
        BKNI_Sleep(sleep);
#else
        printf("Press ENTER to start decode\n");
        getchar();
#endif

        videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
        audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
        videoProgram.pidChannel = videoPidChannel;
        audioProgram.pidChannel = audioPidChannel;
#if EXTRA_LOGS
        printf("PidChannels opened\n");
#endif

        NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
        NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
        if (audioProgram.codec == NEXUS_AudioCodec_eAc3) {
            NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
        }
#if EXTRA_LOGS
        printf("Decoders started\n");
#endif
    }

test3:
    /* Test3: Test1 but randomly toggle closing/opening the pidchannels */
    loops = LOOPS;
    while (loops--) {
        bool reopen = false;
        
#if AUTOMATED_TEST
        sleep = rand()%MAX_SLEEP_TIME;
        BKNI_Sleep(sleep);
#else
        printf("Press ENTER to stop decode\n");
        getchar();
#endif

        /* Stop the decoders. This calls BXPT_Rave_DisableContext */
        NEXUS_AudioDecoder_Stop(pcmDecoder);
        if (audioProgram.codec == NEXUS_AudioCodec_eAc3) {
            NEXUS_AudioDecoder_Stop(compressedDecoder);
        }
        NEXUS_VideoDecoder_Stop(vdecode);
        /* RAVE refcnt is 0 at this point */
#if EXTRA_LOGS
        printf("Decoders stopped\n");
#endif
        reopen = rand()%2;

        if (reopen) {
            /* Close the pidchannels. This calls BXPT_SetParserEnable and BXPT_DisablePidChannel.
               BXPT_DisablePidChannel doesn't affect power management */
            NEXUS_PidChannel_Close(videoPidChannel);
            NEXUS_PidChannel_Close(audioPidChannel);
            printf("Test3: power down (loops %4d)\n", loops);
        }
        else {
            printf("Test3: continue   (loops %4d)\n", loops);
        }

        
#if AUTOMATED_TEST
        sleep = rand()%MAX_SLEEP_TIME;
        BKNI_Sleep(sleep);
#else
        printf("Press ENTER to start decode\n");
        getchar();
#endif

        if (reopen) {
            videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
            audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
            videoProgram.pidChannel = videoPidChannel;
            audioProgram.pidChannel = audioPidChannel;
        }
#if EXTRA_LOGS
        printf("PidChannels opened\n");
#endif

        NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
        NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
        if (audioProgram.codec == NEXUS_AudioCodec_eAc3) {
            NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
        }
#if EXTRA_LOGS
        printf("Decoders started\n");
#endif
    }


    /* shutdown */
    NEXUS_VideoDecoder_Stop(vdecode);
    NEXUS_AudioDecoder_Stop(pcmDecoder);
    if (audioProgram.codec == NEXUS_AudioCodec_eAc3) {
        NEXUS_AudioDecoder_Stop(compressedDecoder);
    }
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_Platform_Uninit();

    return 0;
}
