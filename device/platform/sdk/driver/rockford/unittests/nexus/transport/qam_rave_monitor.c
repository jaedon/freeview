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
 * $brcm_Workfile: qam_rave_monitor.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/30/09 10:05a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/qam_rave_monitor.c $
 * 
 * Hydra_Software_Devel/2   3/30/09 10:05a erickson
 * PR53662: add pid params
 *
 * Hydra_Software_Devel/1   3/30/09 9:30a erickson
 * PR53662: added rave_monitor test code
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
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(tune_qam);

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendQamStatus qamStatus;

    BSTD_UNUSED(param);

    fprintf(stderr, "Lock callback, frontend 0x%08x\n", (unsigned)frontend);

    NEXUS_Frontend_GetQamStatus(frontend, &qamStatus);
    fprintf(stderr, "QAM Lock callback, frontend 0x%08x - lock status %d, %d\n", (unsigned)frontend,
            qamStatus.fecLock, qamStatus.receiverLock);
}

#include "rave_monitor.c"

int main(int argc, char **argv)
{
    unsigned handle0, handle1;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendQamSettings qamSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_AudioDecoderHandle audioDecoder;
    unsigned i;
    /* default freq & qam mode */
    unsigned freq = 357;
    NEXUS_FrontendQamMode qamMode = NEXUS_FrontendQamMode_e256;
    unsigned vidpid = 0x11;
    unsigned audpid = 0x14;

    /* allow cmdline freq & qam mode for simple test */
    if (argc > 1) {
        freq = atoi(argv[1]);
    }
    if (argc > 2 && atoi(argv[2]) == 64) {
        qamMode = NEXUS_FrontendQamMode_e64;
    }
    if (argc > 3) {
        vidpid = strtoul(argv[3], NULL, 0);
    }
    if (argc > 3) {
        audpid = strtoul(argv[4], NULL, 0);
    }

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
    {
        NEXUS_FrontendCapabilities capabilities;
        frontend = platformConfig.frontend[i];
        if (frontend) {
            NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
            /* Does this frontend support qam? */
            if ( capabilities.qam )
            {
                break;
            }
        }
    }

    if (NULL == frontend )
    {
        fprintf(stderr, "Unable to find QAM-capable frontend\n");
        return 0;
    }

    NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
    qamSettings.frequency = freq * 1000000;
    qamSettings.mode = qamMode;
    qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
    qamSettings.lockCallback.callback = lock_callback;
    qamSettings.lockCallback.context = frontend;

    NEXUS_Frontend_GetUserParameters(frontend, &userParams);

    /* Map a parser band to the demod's input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

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
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    handle0 = b_start_monitor(0, NEXUS_ParserBand_e0, parserBandSettings.sourceTypeSettings.inputBand);
    handle1 = b_start_monitor(1, NEXUS_ParserBand_e1, parserBandSettings.sourceTypeSettings.inputBand);

    while (1) {
        NEXUS_VideoDecoderStartSettings videoProgram;
        NEXUS_AudioDecoderStartSettings audioProgram;
        NEXUS_PidChannelHandle videoPidChannel0;
        NEXUS_PidChannelHandle audioPidChannel0;
        NEXUS_PidChannelHandle videoPidChannel1;
        NEXUS_PidChannelHandle audioPidChannel1;
        NEXUS_Error rc;

        videoPidChannel0 = NEXUS_PidChannel_Open(parserBand, vidpid, NULL);
        audioPidChannel0 = NEXUS_PidChannel_Open(parserBand, audpid, NULL);
        videoPidChannel1 = NEXUS_PidChannel_Open(NEXUS_ParserBand_e1, vidpid, NULL);
        audioPidChannel1 = NEXUS_PidChannel_Open(NEXUS_ParserBand_e1, audpid, NULL);

        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
        videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
        videoProgram.pidChannel = videoPidChannel0;
        videoProgram.stcChannel = stcChannel;
        NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
        audioProgram.codec = NEXUS_AudioCodec_eAc3;
        audioProgram.pidChannel = audioPidChannel0;
        audioProgram.stcChannel = stcChannel;

        NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        stcSettings.modeSettings.pcr.pidChannel = videoPidChannel0; /* PCR happens to be on video pid */
        NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);

        rc = NEXUS_Frontend_TuneQam(frontend, &qamSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
        BDBG_ASSERT(!rc);
        rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
        BDBG_ASSERT(!rc);

        b_add_monitor_pid_channel(handle0, videoPidChannel0);
        b_add_monitor_pid_channel(handle0, audioPidChannel0);
        b_add_monitor_pid_channel(handle1, videoPidChannel1);
        b_add_monitor_pid_channel(handle1, audioPidChannel1);

        printf("Press enter to tune again\n");
        getchar();

        b_remove_monitor_pid_channel(handle0, videoPidChannel0);
        b_remove_monitor_pid_channel(handle0, audioPidChannel0);
        b_remove_monitor_pid_channel(handle1, videoPidChannel1);
        b_remove_monitor_pid_channel(handle1, audioPidChannel1);

        NEXUS_AudioDecoder_Stop(audioDecoder);
        NEXUS_VideoDecoder_Stop(videoDecoder);
        NEXUS_PidChannel_Close(videoPidChannel0);
        NEXUS_PidChannel_Close(audioPidChannel0);
        NEXUS_PidChannel_Close(videoPidChannel1);
        NEXUS_PidChannel_Close(audioPidChannel1);
    }

    return 0;
}
