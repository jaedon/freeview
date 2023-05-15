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
 * $brcm_Workfile: video_primer.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/3/11 6:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/video_primer.c $
 * 
 * Hydra_Software_Devel/3   8/3/11 6:25p jtna
 * SW7425-1040: do not set pcr offsetThreshold to 0; it's no longer needed
 * 
 * Hydra_Software_Devel/2   7/28/10 5:20p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/1   12/23/09 3:45p jtna
 * SW7405-3672: added video_primer
 * 
******************************************************************************/
/* unittest to check basic video primer functionality that is not explicitly tested in hd_fcc_demo.c */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_primer.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_stc_channel.h"
#include "nexus_timebase.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_ir_input.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

#define NUM_PROGRAMS 9
struct {
    unsigned short video_pid; /* MPEG2 */
    unsigned short audio_pid; /* AC3 */
} g_programs[NUM_PROGRAMS] = {
    {0x900, 0x901},
    {0xa00, 0xa01},
    {0x7c0, 0x7c1},
    {0x940, 0x941},
    {0xa40, 0xa41},
    {0x880, 0x881},
    {0x800, 0x801},
    {0x840, 0x841},
    {0x8c0, 0x8c1}
  };

typedef enum PrimerMode {
    PrimerMode_eFast,
    PrimerMode_ePreview,
    PrimerMode_ePreroll
} PrimerMode;

PrimerMode primerMode = PrimerMode_eFast;

int main(void)
{
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderPrimerHandle primer[NUM_PROGRAMS];
    NEXUS_VideoDecoderStartSettings videoProgram[NUM_PROGRAMS];
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Timebase timebase = NEXUS_Timebase_e0;
    unsigned i, index;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_Error rc;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram[NUM_PROGRAMS];
    NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    BKNI_Memset(primer, 0, sizeof(primer));
    BKNI_Memset(videoProgram, 0, sizeof(videoProgram));
    BKNI_Memset(audioProgram, 0, sizeof(audioProgram));

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_ParserBand_GetSettings(NEXUS_ParserBand_e0, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.maxDataRate = 42 * 1024 * 1024; /* higher bandwidth required to cover high-bitrate stream */
    rc = NEXUS_ParserBand_SetSettings(NEXUS_ParserBand_e0, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* Open the pid channels */
    for (i=0;i<NUM_PROGRAMS;i++) {
        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[i]);
        videoProgram[i].codec = NEXUS_VideoCodec_eMpeg2;
        videoProgram[i].pidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, g_programs[i].video_pid, NULL);
        if (primerMode == PrimerMode_ePreroll) {
            videoProgram[i].prerollRate = 2;
        }

        NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
        stcSettings.autoConfigTimebase = false; /* must do it manually */
        stcSettings.timebase = timebase;
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        stcSettings.modeSettings.pcr.pidChannel = videoProgram[i].pidChannel;
        stcSettings.stcIndex = 0;
#ifdef IP_STREAMER_SUBST
        IpsAddStcSettings(ips, &stcSettings);
#endif
        videoProgram[i].stcChannel = NEXUS_StcChannel_Open(i, &stcSettings);

        NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram[i]);
        audioProgram[i].codec = NEXUS_AudioCodec_eAc3;
        audioProgram[i].pidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, g_programs[i].audio_pid, NULL);
        audioProgram[i].stcChannel = videoProgram[i].stcChannel;
    }

    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddAudioDecoderOpenSettings(ips, &audioDecoderOpenSettings);
#endif
    audioDecoder = NEXUS_AudioDecoder_Open(0, &audioDecoderOpenSettings);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    /* Bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));

    window = NEXUS_VideoWindow_Open(display, 0);
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = platformConfig.outputs.audioDacs[0];
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    /* Bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    videoDecoderOpenSettings.fifoSize = 3 * 1024 * 1024; /* larger CDB needed */
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif    
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    if (primerMode != PrimerMode_eFast) {
        NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
        videoDecoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture;
        NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    }

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = videoDecoder;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

#if TEST1
    /* Test1: Open a single primer. Start, stop, then start again with same or different program. 
       StartDecodeWithPrimer should proceed without hitch */

    primer[0] = NEXUS_VideoDecoder_OpenPrimer(videoDecoder);

    while (1) {
        index = rand() % NUM_PROGRAMS;
        printf("starting primer %d: video pid 0x%x\n", index, g_programs[index].video_pid);
        NEXUS_VideoDecoder_StartPrimer(videoDecoder, primer[0], &videoProgram[index]);
        getchar();

        printf("stopping primer %d\n", index);
        NEXUS_VideoDecoder_StopPrimer(videoDecoder, primer[0]);
        getchar();

        index = rand() % NUM_PROGRAMS;
        printf("starting primer %d: video pid 0x%x\n", index, g_programs[index].video_pid);
        NEXUS_VideoDecoder_StartPrimer(videoDecoder, primer[0], &videoProgram[index]);
        getchar();

        printf("starting program %d: video pid 0x%x, audio pid 0x%x\n", 
            index, g_programs[index].video_pid, g_programs[index].audio_pid);

        /* start live */
        NEXUS_Timebase_GetSettings(timebase, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
        timebaseSettings.sourceSettings.pcr.pidChannel = videoProgram[index].pidChannel;
        timebaseSettings.sourceSettings.pcr.maxPcrError = 0xff;
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
#ifdef IP_STREAMER_SUBST
        IpsAddTimebaseSettings(ips, &timebaseSettings);
#endif
        NEXUS_Timebase_SetSettings(timebase, &timebaseSettings);
        NEXUS_VideoDecoder_StartDecodeWithPrimer(videoDecoder, primer[0]);
        NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram[index]);

        getchar();
        
        NEXUS_AudioDecoder_Stop(audioDecoder);
        NEXUS_VideoDecoder_Stop(videoDecoder);
    }

    NEXUS_VideoDecoder_ClosePrimer(videoDecoder, primer[0]);
#endif

#if 1
    /* Test2: After calling StartDecodeWithPrimer, stop and start several primers. Optionally, close and open pidChannels as well.
       The current live decode should not be affected (ie. macroblock) */

    #define NUM_PRIMERS 5
    #define OPEN_CLOSE_PID_CHANNELS 1
    #define INTERMEDIATE_STOPS 0 /* prompt before each stop */
    for (i=0; i<NUM_PRIMERS; i++) {
        primer[i] = NEXUS_VideoDecoder_OpenPrimer(videoDecoder);
        NEXUS_VideoDecoder_StartPrimer(videoDecoder, primer[i], &videoProgram[i]);
    }

    index = 0;

    NEXUS_Timebase_GetSettings(timebase, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
    timebaseSettings.sourceSettings.pcr.pidChannel = videoProgram[index].pidChannel;
    timebaseSettings.sourceSettings.pcr.maxPcrError = 0xff;
    timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
#ifdef IP_STREAMER_SUBST
    IpsAddTimebaseSettings(ips, &timebaseSettings);
#endif    
    NEXUS_Timebase_SetSettings(timebase, &timebaseSettings);

    NEXUS_VideoDecoder_StartDecodeWithPrimer(videoDecoder, primer[index]);
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram[index]);

    printf("Press ENTER to start\n");
    getchar();

    while (1) {
        NEXUS_AudioDecoder_Stop(audioDecoder);
        NEXUS_VideoDecoder_Stop(videoDecoder);
        /* start primer on old channel */
        NEXUS_VideoDecoder_StartPrimer(videoDecoder, primer[index], &videoProgram[index]);

        if (++index == NUM_PRIMERS) index = 0;

        printf("starting program %d: video pid 0x%x, audio pid 0x%x\n", 
            index, g_programs[index].video_pid, g_programs[index].audio_pid);

        NEXUS_Timebase_GetSettings(timebase, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
        timebaseSettings.sourceSettings.pcr.pidChannel = videoProgram[index].pidChannel;
        timebaseSettings.sourceSettings.pcr.maxPcrError = 0xff;
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
#ifdef IP_STREAMER_SUBST
        IpsAddTimebaseSettings(ips, &timebaseSettings);
#endif    
        NEXUS_Timebase_SetSettings(timebase, &timebaseSettings);

        NEXUS_VideoDecoder_StartDecodeWithPrimer(videoDecoder, primer[index]);
        NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram[index]);
        getchar();

        for (i=0; i<NUM_PRIMERS; i++) {
            if (i!=index) {
                printf("stopping primer %d during decode\n", i);
                NEXUS_VideoDecoder_StopPrimer(videoDecoder, primer[i]);
#if OPEN_CLOSE_PID_CHANNELS
                NEXUS_PidChannel_Close(videoProgram[i].pidChannel);
                NEXUS_PidChannel_Close(audioProgram[i].pidChannel);
#endif
            }
        }
#if INTERMEDIATE_STOPS
        getchar();
#endif

        for (i=0; i<NUM_PRIMERS; i++) {
            if (i!=index) {
#if OPEN_CLOSE_PID_CHANNELS
                videoProgram[i].pidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, g_programs[i].video_pid, NULL);

                NEXUS_StcChannel_GetSettings(videoProgram[i].stcChannel, &stcSettings);
                stcSettings.modeSettings.pcr.pidChannel = videoProgram[i].pidChannel; /* setting this to the wrong pidChannel will result in something that looks like a low framerate */
#ifdef IP_STREAMER_SUBST
                IpsAddStcSettings(ips, &stcSettings);
#endif        
                NEXUS_StcChannel_SetSettings(videoProgram[i].stcChannel, &stcSettings);

                audioProgram[i].pidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, g_programs[i].audio_pid, NULL);
#endif
                printf("starting primer %d during decode\n", i);
                NEXUS_VideoDecoder_StartPrimer(videoDecoder, primer[i], &videoProgram[i]);
            }
        }
#if INTERMEDIATE_STOPS
        getchar();
#endif
        
    }

#endif
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    return 0;
}
