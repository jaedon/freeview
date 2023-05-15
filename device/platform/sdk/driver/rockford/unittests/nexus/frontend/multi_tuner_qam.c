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
 * $brcm_Workfile: multi_tuner_qam.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/10/10 11:33a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/frontend/multi_tuner_qam.c $
 * 
 * Hydra_Software_Devel/2   8/10/10 11:33a jtna
 * SW7125-247: modify frequency list
 * 
 * Hydra_Software_Devel/1   3/26/10 2:53p jtna
 * SW7125-247: added multi_tuner_qam.c
 * 
 *****************************************************************************/
/* Unit test for multi-tuner platforms (eg. 97125 with 3255). 
   During live decode, tuning the other tuners to other frequencies should not affect 
   (eg. macroblocking or other artifcats) the live decode. This unit test checks this. */

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
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_FRONTENDS 3
#define NUM_PROGRAMS (NUM_FRONTENDS+1)

/* you need (NUM_FRONTENDS+1) unique frequencies for this test */
struct {
    unsigned freq; /* MHz */
    NEXUS_FrontendQamMode qamMode;
    unsigned short video_pid; /* MPEG */
    unsigned short audio_pid; /* AC3 */
} g_programs[NUM_PROGRAMS] = {
    {237, NEXUS_FrontendQamMode_e256, 0x7c0, 0x7c1},
    {267, NEXUS_FrontendQamMode_e256, 0x800, 0x801},
    {591, NEXUS_FrontendQamMode_e256, 0x840, 0x841},
    {657, NEXUS_FrontendQamMode_e256, 0x800, 0x801},
    };

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendQamStatus qamStatus;

#if 0
    NEXUS_Frontend_GetQamStatus(frontend, &qamStatus);
    fprintf(stderr, "QAM Lock callback, frontend [%d] 0x%08x - lock status %d, %d\n", param, (unsigned)frontend,
            qamStatus.fecLock, qamStatus.receiverLock);
#else
    BSTD_UNUSED(frontend);
    BSTD_UNUSED(qamStatus);
    BSTD_UNUSED(param);
#endif
}

int main(int argc, char **argv)
{
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend[NUM_FRONTENDS];
    NEXUS_FrontendQamSettings qamSettings[NUM_PROGRAMS];
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel[NUM_FRONTENDS];
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_PidChannelHandle videoPidChannel[NUM_PROGRAMS], audioPidChannel[NUM_PROGRAMS];
    unsigned i,j;

    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);
    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    BKNI_Memset(frontend, 0, sizeof(frontend));
    if (NEXUS_GetEnv("ds0_video")==NULL) {
        fprintf(stderr, "Set ds0_video=y to allow 3rd tuner to be used for video\n");
    }

    /* find QAM-capable frontends */
    for (i=0,j=0; i<NEXUS_MAX_FRONTENDS; i++) {
        NEXUS_FrontendHandle fh;
        NEXUS_FrontendCapabilities capabilities;
        fh = platformConfig.frontend[i];
        if (fh) {
            NEXUS_Frontend_GetCapabilities(fh, &capabilities);
            if (capabilities.qam) {
                frontend[j] = fh;
                fprintf(stdout, "frontend[%d] = platformConfig.frontend[%d]\n", j, i);
                if (++j==NUM_FRONTENDS) break;
            }
        }
    }

    /* set common QAM settings */
    for (i=0; i<NUM_FRONTENDS; i++) {
        NEXUS_Frontend_GetDefaultQamSettings(&qamSettings[i]);
        qamSettings[i].frequency = g_programs[i].freq * 1000000;
        qamSettings[i].mode = g_programs[i].qamMode;
        qamSettings[i].annex = NEXUS_FrontendQamAnnex_eB;
        qamSettings[i].lockCallback.callback = lock_callback;
        qamSettings[i].lockCallback.context = frontend[i];
        qamSettings[i].lockCallback.param = i;
    }

    /* open STC channels */
    for (i=0; i<NUM_FRONTENDS; i++) {
        NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
        stcSettings.autoConfigTimebase = false; /* must do it manually */
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        stcSettings.timebase = NEXUS_Timebase_e0;
        stcSettings.stcIndex = 0;
        stcChannel[i] = NEXUS_StcChannel_Open(i, &stcSettings);
    }
    
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* open all pidchannels */
    for (i=0; i<NUM_PROGRAMS; i++) {
        videoPidChannel[i] = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0+i, g_programs[i].video_pid, NULL);
        audioPidChannel[i] = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0+i, g_programs[i].audio_pid, NULL);
    }

    /* start with tuning all frontends */
    for (i=0; i<NUM_FRONTENDS; i++) {
        NEXUS_Frontend_TuneQam(frontend[i], &qamSettings[i]);
    }

    /* wait and assert lock on all frontends */
    BKNI_Sleep(3000);
    for (i=0; i<NUM_FRONTENDS; i++) {
        NEXUS_FrontendQamStatus qamStatus;
        NEXUS_Frontend_GetQamStatus(frontend[i], &qamStatus);
        BDBG_ASSERT(qamStatus.fecLock);
        BDBG_ASSERT(qamStatus.receiverLock);
    }
    printf("All frontends locked\n");

    /* do a live decode using one tuner, while moving the other tuners. the live decode should not macroblock or show any artifacts */
    for (i=0; i<NUM_FRONTENDS; i++) {
        NEXUS_VideoDecoderStartSettings videoProgram;
        NEXUS_AudioDecoderStartSettings audioProgram;

        /* start the live decode */
        NEXUS_Frontend_GetUserParameters(frontend[i], &userParams);
        parserBand = NEXUS_ParserBand_e0 + i;
        NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
        parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;
        NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
        videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
        videoProgram.pidChannel = videoPidChannel[i];
        videoProgram.stcChannel = stcChannel[i];
        NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
        audioProgram.codec = NEXUS_AudioCodec_eAc3;
        audioProgram.pidChannel = audioPidChannel[i];
        audioProgram.stcChannel = stcChannel[i];

        NEXUS_StcChannel_GetSettings(stcChannel[i], &stcSettings);
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        stcSettings.modeSettings.pcr.pidChannel = videoPidChannel[i]; /* PCR happens to be on video pid */
        NEXUS_StcChannel_SetSettings(stcChannel[i], &stcSettings);

        NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
        NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

        printf("Current live is frontend[%d]\n", i);

        /* move all other tuners */
        for (j=0; j<NUM_FRONTENDS; j++) {
            if (i==j) continue;

            printf("   Press Enter to move frontend[%d]", j);
            getchar();
            qamSettings[j].frequency = g_programs[NUM_FRONTENDS].freq * 1000000;
            qamSettings[j].mode = g_programs[NUM_FRONTENDS].qamMode;
            NEXUS_Frontend_TuneQam(frontend[j], &qamSettings[j]);
            BKNI_Sleep(2000);

            printf("   Press Enter to move back");
            getchar();
            /* move back */
            qamSettings[j].frequency = g_programs[j].freq * 1000000;
            qamSettings[j].mode = g_programs[j].qamMode;
            NEXUS_Frontend_TuneQam(frontend[j], &qamSettings[j]);
            BKNI_Sleep(2000);
        }

        printf("   Press Enter to switch live");
        getchar();

        NEXUS_AudioDecoder_Stop(audioDecoder);
        NEXUS_VideoDecoder_Stop(videoDecoder);
    }

    printf("Test completed. Press Enter to exit\n");
    getchar();
    return 0;
}
