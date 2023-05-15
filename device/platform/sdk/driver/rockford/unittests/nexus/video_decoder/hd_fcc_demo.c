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
 * $brcm_Workfile: hd_fcc_demo.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 8/12/11 5:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/hd_fcc_demo.c $
 * 
 * Hydra_Software_Devel/10   8/12/11 5:43p jtna
 * SW7425-1070: update apps with ptsOffset, diffCorrection and syncChannel
 * options
 * 
 * Hydra_Software_Devel/9   8/3/11 6:25p jtna
 * SW7425-1040: do not set pcr offsetThreshold to 0; it's no longer needed
 * 
 * Hydra_Software_Devel/8   5/11/11 5:20p jtna
 * SW7425-568: increase parserband maxdatarate for 40nm platforms
 * 
 * Hydra_Software_Devel/7   9/10/10 12:34p jtna
 * SW7405-4723: more automated testing
 * 
 * Hydra_Software_Devel/6   9/2/10 5:19p jtna
 * SW7405-4723: better automated testing
 * 
 * Hydra_Software_Devel/5   8/23/10 3:20p jtna
 * SW7405-4723: update apps
 * 
 * Hydra_Software_Devel/4   7/28/10 4:54p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/3   10/9/09 5:47p jtna
 * SW7405-3119: add options to control primer transition behavior
 * 
 * Hydra_Software_Devel/2   8/20/09 10:44a erickson
 * PR54880: handle ir key repeats
 *
 * Hydra_Software_Devel/1   8/12/09 4:42p erickson
 * PR54880: add fcc demo apps
 *
 * 3   7/30/09 10:54a erickson
 * PR54880: enable TSM
 *
 * 2   6/29/09 3:15p erickson
 * PR54880: add #include
 *
 * 1   5/19/09 10:31a erickson
 * PR54880: add video_primer
 *
******************************************************************************/

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
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_sync_channel.h"

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

#define TOTAL_PRIMERS 4
struct {
    unsigned short video_pid; /* MPEG */
    unsigned short audio_pid; /* AC3 */
    unsigned parser;
} g_programs[TOTAL_PRIMERS] = {
#if 0
    /* italyriviera */
    {0x11, 0x14, 0},
    {0x31, 0x34, 0},
    {0x11, 0x14, 1},
    {0x31, 0x34, 1}
#else
    /* Ch81.trp */
    {0x21, 0x24, 0},
    {0x31, 0x34, 0},
    {0x41, 0x44, 0},
    {0x51, 0x54, 0}
#endif
    };

#define USE_SYNC_CHANNEL 0
#define USE_PTS_STC_DIFF_CORRECTION 1
#define USE_DECODER_PTS_OFFSET 0

void change_primer_settings(NEXUS_VideoDecoderPrimerHandle *primer, unsigned i);
static void ir_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

#define MAX_DATA_RATE 35*1024*1024

int main(void)
{
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderPrimerHandle primer[TOTAL_PRIMERS];
    NEXUS_VideoDecoderStartSettings videoProgram[TOTAL_PRIMERS];
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Timebase timebase = NEXUS_Timebase_e0;
    unsigned index = 0;
    unsigned i;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    NEXUS_Error rc;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendQamSettings qamSettings;
    NEXUS_FrontendHandle frontend;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_IrInputHandle irHandle;
    NEXUS_IrInputSettings irSettings;
    BKNI_EventHandle event;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram[TOTAL_PRIMERS];
    NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.transportModuleSettings.maxDataRate.parserBand[0] = MAX_DATA_RATE; 
    platformSettings.transportModuleSettings.maxDataRate.parserBand[1] = MAX_DATA_RATE;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* PB0 = streamer */
    NEXUS_ParserBand_GetSettings(NEXUS_ParserBand_e0, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.maxDataRate = MAX_DATA_RATE; /* higher bandwidth required for simultaneous HD and SD programs */
    rc = NEXUS_ParserBand_SetSettings(NEXUS_ParserBand_e0, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex=0;
    ipsOpenSettings.remuxIndex=0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    frontend = platformConfig.frontend[0];
    if (frontend) {
        NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
        qamSettings.frequency = 765 * 1000000;
        qamSettings.mode = NEXUS_FrontendQamMode_e64;
        qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
        rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
        BDBG_ASSERT(!rc);
        NEXUS_Frontend_GetUserParameters(frontend, &userParams);
        NEXUS_ParserBand_GetSettings(NEXUS_ParserBand_e1, &parserBandSettings);
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
        parserBandSettings.maxDataRate = MAX_DATA_RATE;
        rc = NEXUS_ParserBand_SetSettings(NEXUS_ParserBand_e1, &parserBandSettings);
        BDBG_ASSERT(!rc);
    }
    else {
        fprintf(stderr, "frontend handle is NULL\n");
    }

    /* Open the pid channels */
    for (i=0;i<TOTAL_PRIMERS;i++) {
        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[i]);
        videoProgram[i].codec = NEXUS_VideoCodec_eMpeg2;
        videoProgram[i].pidChannel = NEXUS_PidChannel_Open(g_programs[i].parser + NEXUS_ParserBand_e0, g_programs[i].video_pid, NULL);

        NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
        stcSettings.autoConfigTimebase = false; /* must do it manually */
        stcSettings.timebase = timebase;
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        stcSettings.modeSettings.pcr.pidChannel = videoProgram[i].pidChannel;
        stcSettings.stcIndex = 0;
 #ifdef IP_STREAMER_SUBST
        if(i < 2) {
            IpsAddStcSettings(ips, &stcSettings);
        }
#endif
        videoProgram[i].stcChannel = NEXUS_StcChannel_Open(i, &stcSettings);

        NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram[i]);
        audioProgram[i].codec = NEXUS_AudioCodec_eAc3;
        audioProgram[i].pidChannel = NEXUS_PidChannel_Open(g_programs[i].parser + NEXUS_ParserBand_e0, g_programs[i].audio_pid, NULL);
        audioProgram[i].stcChannel = videoProgram[i].stcChannel;
    }

    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddAudioDecoderOpenSettings(ips, &audioDecoderOpenSettings);
#endif
    audioDecoderOpenSettings.fifoSize *= 2;
    audioDecoder = NEXUS_AudioDecoder_Open(0, &audioDecoderOpenSettings);

    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    /* bring up display */
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = platformConfig.outputs.audioDacs[0];
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    /* larger CDB needed for MPEG HD fcc */
    videoDecoderOpenSettings.fifoSize = 4 * 1024 * 1024;
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif    
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

#if USE_DECODER_PTS_OFFSET
{
    /* these should cancel out */
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.ptsOffset = 500*45;
    NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);

    NEXUS_AudioDecoderSettings audioDecoderSettings;
    NEXUS_AudioDecoder_GetSettings(audioDecoder, &audioDecoderSettings);
    audioDecoderSettings.ptsOffset = 500*45;
    NEXUS_AudioDecoder_SetSettings(audioDecoder, &audioDecoderSettings);
}
#endif

    for (i=0;i<TOTAL_PRIMERS;i++) {
        NEXUS_VideoDecoderPrimerSettings settings;
        primer[i] = NEXUS_VideoDecoder_OpenPrimer(videoDecoder);
        NEXUS_VideoDecoder_StartPrimer(videoDecoder, primer[i], &videoProgram[i]);

        NEXUS_VideoDecoder_GetPrimerSettings(primer[i], &settings);
#if USE_PTS_STC_DIFF_CORRECTION
        settings.pastTolerance = 600;
        settings.ptsStcDiffCorrectionEnabled = true;
#endif
        NEXUS_VideoDecoder_SetPrimerSettings(primer[i], &settings);
    }

    BKNI_CreateEvent(&event);

    NEXUS_IrInput_GetDefaultSettings(&irSettings);
    irSettings.mode = NEXUS_IrInputMode_eRemoteA;
    irSettings.dataReady.callback = ir_callback;
    irSettings.dataReady.context = event;
    irHandle = NEXUS_IrInput_Open(0, &irSettings);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = videoDecoder;
    ipsStartSettings.aDecoder = audioDecoder;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

#if USE_SYNC_CHANNEL
{
    NEXUS_SyncChannelHandle syncChannel;
    NEXUS_SyncChannelSettings syncChannelSettings;
    NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
    syncChannel = NEXUS_SyncChannel_Create(&syncChannelSettings);

    NEXUS_SyncChannel_GetSettings(syncChannel, &syncChannelSettings);
    syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(videoDecoder);
    syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
    NEXUS_SyncChannel_SetSettings(syncChannel, &syncChannelSettings);
}
#endif

    i = 0;
    while (1) {
        NEXUS_TimebaseSettings timebaseSettings;

        NEXUS_Timebase_GetSettings(timebase, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
        timebaseSettings.sourceSettings.pcr.pidChannel = videoProgram[index].pidChannel;
        timebaseSettings.sourceSettings.pcr.maxPcrError = 0xff;
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
        NEXUS_Timebase_SetSettings(timebase, &timebaseSettings);

        printf("starting program %d: video pid 0x%x, audio pid 0x%x\n", index, g_programs[index].video_pid, g_programs[index].audio_pid);
        NEXUS_VideoDecoder_StartDecodeWithPrimer(videoDecoder, primer[index]);

        NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram[index]);

#if 0 /* IR input */
        while (1) {
            NEXUS_IrInputEvent irEvent;
            unsigned num;
            bool overflow;

            NEXUS_IrInput_GetEvents(irHandle, &irEvent, 1, &num, &overflow);
            if (num == 0) {
                BKNI_WaitForEvent(event, 0xFFFFFFFF);
            }
            else if (!irEvent.repeat) {
                break;
            }
        }
#elif 1 /* Enter key */
        getchar();
#else /* automated */
        BKNI_Sleep(rand()%5000+500);
        i++;

        #define COUNT 5000
        if (i%COUNT==0) {
            change_primer_settings(primer, i/COUNT);
        }
#endif
        NEXUS_AudioDecoder_Stop(audioDecoder);
        NEXUS_VideoDecoder_Stop(videoDecoder);
        NEXUS_VideoDecoder_StartPrimer(videoDecoder, primer[index], &videoProgram[index]);

        if (++index == TOTAL_PRIMERS) index = 0;
    }

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    return 0;
}

struct settings {
    unsigned past;
    unsigned future;
} g_settings[] = {
    {0, 0}, /* default */
    {1, 0}, /* always race */
    {0, 1}, /* always mute */
    {200, 1000}, /* don't race more than 200ms */
    {1000, 0}, /* always race */
    {0, 1000} /* always mute */
};

void change_primer_settings(NEXUS_VideoDecoderPrimerHandle *primer, unsigned i)
{
    unsigned idx, past, future;
    NEXUS_VideoDecoderPrimerSettings settings;
    idx = i % (sizeof(g_settings)/sizeof(*g_settings));
    past = g_settings[idx].past;
    future = g_settings[idx].future;
    
    for (i=0; i<TOTAL_PRIMERS; i++) {
        NEXUS_VideoDecoder_GetPrimerSettings(primer[i], &settings);
        settings.pastTolerance = past;
        settings.futureTolerance = future;
        NEXUS_VideoDecoder_SetPrimerSettings(primer[i], &settings);
    }
    printf("change primer settings [%d] (%d,%d)\n", idx, past, future);
}
