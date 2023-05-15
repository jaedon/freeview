/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: examples_lib.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 10/30/12 4:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/examples_lib.c $
 * 
 * Hydra_Software_Devel/4   10/30/12 4:28p erickson
 * SW7400-3052: add hdmi output, remove unused code
 * 
 * Hydra_Software_Devel/3   1/7/11 3:33p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 * PTS - STC difference
 * 
 * Hydra_Software_Devel/2   2/2/10 3:50p mward
 * SW7125-195: fix include file order.
 * 
 * Hydra_Software_Devel/1   2/19/09 3:30p erickson
 * PR51841: move examples_lib out of nexus/examples
 * 
 * 32   12/10/08 2:45p vsilyaev
 * PR 49718: Print stream duration
 * 
 * 31   12/8/08 11:07a erickson
 * PR49930: remove static from handles for more test app flexibility
 *
 * 30   12/8/08 10:29a vsilyaev
 * PR 49718: Added playback position and print audio status only if
 *  decoder started
 *
 * 29   11/13/08 1:46p jgarrett
 * PR 48965: Allowing composite on 3549b0
 *
 * 28   10/24/08 10:58a erickson
 * PR42679: fix 93549
 *
 * 27   10/15/08 4:40p erickson
 * PR47818: fix if no playback
 *
 * 26   10/14/08 5:01p erickson
 * PR47818: set playback's stcChannel to test trick modes
 *
 * 25   10/6/08 5:26p vsilyaev
 * PR 46250: Print status of playpump
 *
 * 24   9/3/08 3:58p erickson
 * PR46250: propogate error codes on failed Start
 *
 * 23   8/25/08 3:45p jgarrett
 * PR 46116: Fixing support without playback
 *
 * 22   8/7/08 12:15p jgarrett
 * PR 45475: Fixing default pcr pid to match video pid
 *
 * 21   6/20/08 5:51p vsilyaev
 * PR 40921: Use NTSC resolution if no component output
 *
 * 20   6/19/08 6:07p cnovak
 * PR43697: Add uCOS support.
 *
 * 19   6/19/08 2:38p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 18   6/19/08 10:46a erickson
 * PR42678: switch to output_type=panel|component
 *
 * 17   6/7/08 1:40p jgarrett
 * PR 43318: Allowing for platforms without composite output
 *
 * 16   5/22/08 2:10p erickson
 * PR42679: fix warning
 *
 * 15   5/21/08 12:15p rjlewis
 * PR40352: Added VxWorks support.
 *
 * 14   5/21/08 11:49a erickson
 * PR42678: clean up for DTV
 *
 * 13   5/21/08 10:33a erickson
 * PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
 *
 * 12   5/13/08 9:13a erickson
 * PR42504: only stop the passthrough decoder if started
 *
 * 11   5/7/08 4:20p erickson
 * PR42329: remove unneeded pcr pidchannel code. remove bad parser band
 *  config (don't assume parser band == input band).
 *
 * 10   5/1/08 10:13a erickson
 * PR42349: add b_get_time for better example status
 *
 * 9   4/24/08 6:00p erickson
 * PR42054: default transportType to eTs
 *
 * 8   4/24/08 3:31p erickson
 * PR39453: allow examples_lib.c w/o audio
 *
 * 7   4/24/08 11:04a jrubio
 * PR42054: add DSS support for tuner ast
 *
 * 6   4/1/08 11:39a erickson
 * PR38442: use is_panel_output() for consistent DVO output selection
 *
 * 5   3/27/08 6:43p vsilyaev
 * PR 40818: Rearanged code to allow exercice system with shared heap
 *
 * 4   2/7/08 12:31p vsilyaev
 * PR 38682: Reordered display and video decoder
 *
 * 3   1/23/08 12:39p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_video_input.h"
#include "nexus_video_output.h"
#if NEXUS_HAS_AUDIO
#include "nexus_audio_mixer.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_spdif_output.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#endif
#include "nexus_video_window.h"
#include "nexus_parser_band.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "examples_lib.h"

#if NEXUS_HAS_AUDIO
NEXUS_AudioDecoderHandle g_audioDecoder, g_audioPassthrough;
NEXUS_AudioDacHandle g_audioDac;
NEXUS_SpdifOutputHandle g_spdif;
#endif
NEXUS_PidChannelHandle g_videoPid, g_audioPid, g_pcrPid;
NEXUS_PlaypumpHandle g_playpump;
#if NEXUS_HAS_PLAYBACK
NEXUS_PlaybackHandle g_playback;
#endif
NEXUS_VideoDecoderHandle g_videoDecoder;
NEXUS_DisplayHandle g_display;
NEXUS_VideoWindowHandle g_window;
NEXUS_StcChannelHandle g_stcChannel;
NEXUS_PlatformConfiguration g_platformConfig;

static bool g_initialized = false;
static bool g_started = false;
static bool g_usePassthroughAudioDecoder = false;

BDBG_MODULE(examples_lib);

bool is_panel_output()
{
    const char *pEnvVar = getenv("output_type");
    if (pEnvVar) {
        return !strcmp(pEnvVar, "panel");
    }

    /* panel_output=y is deprecrated. please use output_type=panel|component */
    pEnvVar = getenv("panel_output");
    return pEnvVar && (pEnvVar[0] == 'y' || pEnvVar[0] == 'Y');
}

void ExampleDecode_GetDefaultSettings(ExampleDecodeSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->transportType = NEXUS_TransportType_eTs;
}

/* Init the example decode engine */
NEXUS_Error InitExampleDecode(void)
{
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplaySettings displayCfg;
    NEXUS_Error rc;

    BDBG_ASSERT(false == g_initialized);

    NEXUS_Platform_GetConfiguration(&g_platformConfig);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    g_stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);
    BDBG_ASSERT(NULL != g_stcChannel);

#if NEXUS_HAS_AUDIO
    g_audioDac = g_platformConfig.outputs.audioDacs[0];
    BDBG_ASSERT(NULL != g_audioDac);
    g_spdif = g_platformConfig.outputs.spdif[0];
    BDBG_ASSERT(NULL != g_spdif);
    /* Don't attach outputs here, do that at start time based on format */

    g_audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    BDBG_ASSERT(NULL != g_audioDecoder);

    g_audioPassthrough = NEXUS_AudioDecoder_Open(1, NULL);
    BDBG_ASSERT(NULL != g_audioPassthrough);

#endif

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displayCfg);
    g_display = NEXUS_Display_Open(0, &displayCfg);
    BDBG_ASSERT(g_display);
    g_window = NEXUS_VideoWindow_Open(g_display, 0);
    BDBG_ASSERT(g_window);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(g_display, NEXUS_ComponentOutput_GetConnector(g_platformConfig.outputs.component[0]));
    if (rc) BERR_TRACE(rc);
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    rc = NEXUS_Display_AddOutput(g_display, NEXUS_HdmiOutput_GetVideoConnector(g_platformConfig.outputs.hdmi[0]));
    if (rc) BERR_TRACE(rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if ( g_platformConfig.outputs.composite[0] ) {
        rc = NEXUS_Display_AddOutput(g_display, NEXUS_CompositeOutput_GetConnector(g_platformConfig.outputs.composite[0]));
        if (rc) BERR_TRACE(rc);
    }
#endif

    g_initialized = true;

    return 0;
}

NEXUS_Error StartExampleDecode(ExampleDecodeSettings *pSettings)
{
    NEXUS_VideoDecoderStartSettings videoProgram;
#if NEXUS_HAS_AUDIO
    NEXUS_AudioDecoderStartSettings audioProgram;
#endif
    NEXUS_StcChannelSettings stcSettings;
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlaybackPidChannelSettings pidSettings;
#endif
    NEXUS_Error rc;

    /* Init automatically if this is the first time we're called */
    if ( false == g_initialized )
    {
        rc = InitExampleDecode();
        if (rc) return BERR_TRACE(rc);
    }

    /* bring up decoder and connect to display */
    g_videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    BDBG_ASSERT(NULL != g_videoDecoder);

    rc = NEXUS_VideoWindow_AddInput(g_window, NEXUS_VideoDecoder_GetConnector(g_videoDecoder));
    if (rc) return BERR_TRACE(rc);

    if ( true == g_started )
    {
        return BERR_TRACE(1);
    }

    /* if live, the parserBand must be configured outside of examples_lib */

    /* Store this for use on stop */
    g_playpump = pSettings->playpump;
#if NEXUS_HAS_PLAYBACK
    g_playback = pSettings->playback;
#endif

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = pSettings->videoCodec;
    if ( pSettings->videoPid )
    {
#if NEXUS_HAS_PLAYBACK
        if (pSettings->playback) {
            NEXUS_Playback_GetDefaultPidChannelSettings(&pidSettings);
            pidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
            pidSettings.pidTypeSettings.video.codec = pSettings->videoCodec;
            pidSettings.pidTypeSettings.video.decoder = g_videoDecoder;
            pidSettings.pidTypeSettings.video.index = true;
            videoProgram.pidChannel = NEXUS_Playback_OpenPidChannel(pSettings->playback, pSettings->videoPid, &pidSettings);
        }
        else
#endif
        if (pSettings->playpump) {
            videoProgram.pidChannel = NEXUS_Playpump_OpenPidChannel(pSettings->playpump, pSettings->videoPid, NULL);
        }
        else {
            videoProgram.pidChannel = NEXUS_PidChannel_Open(pSettings->parserBand, pSettings->videoPid, NULL);
        }
        g_videoPid = videoProgram.pidChannel;
        BDBG_ASSERT(videoProgram.pidChannel);
    }

#if NEXUS_HAS_AUDIO
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = pSettings->audioCodec;
    g_usePassthroughAudioDecoder = (audioProgram.codec == NEXUS_AudioCodec_eAc3);
    if ( pSettings->audioPid )
    {
#if NEXUS_HAS_PLAYBACK
        if (pSettings->playback) {
            NEXUS_Playback_GetDefaultPidChannelSettings(&pidSettings);
            pidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
            pidSettings.pidTypeSettings.audio.primary = g_audioDecoder;
            if ( g_usePassthroughAudioDecoder ) {
                pidSettings.pidTypeSettings.audio.secondary = g_audioPassthrough;
            } else {
                pidSettings.pidTypeSettings.audio.secondary = NULL;
            }
            audioProgram.pidChannel  = NEXUS_Playback_OpenPidChannel(pSettings->playback, pSettings->audioPid, &pidSettings);
        }
        else
#endif
        if (pSettings->playpump) {
            audioProgram.pidChannel = NEXUS_Playpump_OpenPidChannel(pSettings->playpump, pSettings->audioPid, NULL);
        }
        else {
            audioProgram.pidChannel =  NEXUS_PidChannel_Open(pSettings->parserBand, pSettings->audioPid, NULL);
        }
        g_audioPid = audioProgram.pidChannel;
        BDBG_ASSERT(audioProgram.pidChannel);
    }
#endif

    NEXUS_StcChannel_GetSettings(g_stcChannel, &stcSettings);
#if NEXUS_HAS_PLAYBACK
    if (pSettings->playback || pSettings->playpump)
#else
    if (pSettings->playpump)
#endif
    {
        stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
        stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
    }
    else {
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        if (pSettings->pcrPid) {
           stcSettings.modeSettings.pcr.pidChannel = g_pcrPid = NEXUS_PidChannel_Open(pSettings->parserBand, pSettings->pcrPid, NULL);
        }
        else {
            BDBG_WRN(("No PCR Pid -- defaulting PCR PID = Video PID"));
            stcSettings.modeSettings.pcr.pidChannel = g_videoPid;
        }
    }

    rc = NEXUS_StcChannel_SetSettings(g_stcChannel, &stcSettings);
    if (rc) return BERR_TRACE(rc);
    videoProgram.stcChannel = g_stcChannel;

#if NEXUS_HAS_PLAYBACK
    if (g_playback) {
        NEXUS_PlaybackSettings playbackSettings;
        NEXUS_Playback_GetSettings(g_playback, &playbackSettings);
        playbackSettings.stcChannel = g_stcChannel;
        rc = NEXUS_Playback_SetSettings(g_playback, &playbackSettings);
        if (rc) return BERR_TRACE(rc);
    }
#endif

#if NEXUS_HAS_AUDIO
    audioProgram.stcChannel = g_stcChannel;
    rc = NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(g_audioDac),
                               NEXUS_AudioDecoder_GetConnector(g_audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if (rc) return BERR_TRACE(rc);
    if ( g_usePassthroughAudioDecoder )
    {
        /* Only pass through AC3 */
        rc = NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(g_spdif),
                                   NEXUS_AudioDecoder_GetConnector(g_audioPassthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
        if (rc) return BERR_TRACE(rc);
    }
    else
    {
        rc = NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(g_spdif),
                                   NEXUS_AudioDecoder_GetConnector(g_audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
        if (rc) return BERR_TRACE(rc);
    }
#endif

    /* Start Decoders */
    if ( pSettings->videoPid )
    {
        rc = NEXUS_VideoDecoder_Start(g_videoDecoder, &videoProgram);
        if (rc) return BERR_TRACE(rc);
    }
#if NEXUS_HAS_AUDIO
    if ( pSettings->audioPid )
    {
        rc = NEXUS_AudioDecoder_Start(g_audioDecoder, &audioProgram);
        if (rc) return BERR_TRACE(rc);
        if ( g_usePassthroughAudioDecoder )
        {
            /* Only pass through AC3 */
            rc = NEXUS_AudioDecoder_Start(g_audioPassthrough, &audioProgram);
            if (rc) return BERR_TRACE(rc);
        }
    }
#endif

    g_started = true;
    return 0;
}

void StopExampleDecode(void)
{
    if ( false == g_started )
    {
        return;
    }
    g_started = false;

    NEXUS_VideoDecoder_Stop(g_videoDecoder);
#if NEXUS_HAS_AUDIO
    NEXUS_AudioDecoder_Stop(g_audioDecoder);
    if ( g_usePassthroughAudioDecoder ) {
        NEXUS_AudioDecoder_Stop(g_audioPassthrough);
    }
#endif
#if NEXUS_HAS_PLAYBACK
    if ( g_playback )
    {
        if ( g_videoPid )
            NEXUS_Playback_ClosePidChannel(g_playback, g_videoPid);
        if ( g_audioPid )
            NEXUS_Playback_ClosePidChannel(g_playback, g_audioPid);
        if ( g_pcrPid )
            NEXUS_Playback_ClosePidChannel(g_playback, g_pcrPid);
        g_playback = NULL;
    }
    else
#endif
    if ( g_playpump )
    {
        if ( g_videoPid )
            NEXUS_Playpump_ClosePidChannel(g_playpump, g_videoPid);
        if ( g_audioPid )
            NEXUS_Playpump_ClosePidChannel(g_playpump, g_audioPid);
        if ( g_pcrPid )
            NEXUS_Playpump_ClosePidChannel(g_playpump, g_pcrPid);
        g_playpump = NULL;
    }
    else
    {
        if ( g_videoPid )
            NEXUS_PidChannel_Close(g_videoPid);
        if ( g_audioPid )
            NEXUS_PidChannel_Close(g_audioPid);
        if ( g_pcrPid )
            NEXUS_PidChannel_Close(g_pcrPid);
    }
    g_pcrPid = g_videoPid = g_audioPid = NULL;

#if NEXUS_HAS_AUDIO
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(g_audioDac));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(g_spdif));
#endif
    NEXUS_VideoWindow_RemoveAllInputs(g_window);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(g_videoDecoder));
    NEXUS_VideoDecoder_Close(g_videoDecoder);
}

void UninitExampleDecode(void)
{
    NEXUS_VideoWindow_Close(g_window);
    NEXUS_Display_Close(g_display);
#if NEXUS_HAS_AUDIO
    NEXUS_AudioDecoder_Close(g_audioDecoder);
    NEXUS_AudioDecoder_Close(g_audioPassthrough);
#endif
    NEXUS_StcChannel_Close(g_stcChannel);
    g_stcChannel = NULL;
    g_initialized = false;
}

void PrintExampleDecodeStatus(void)
{
    NEXUS_VideoDecoderStatus status;
#if NEXUS_HAS_AUDIO
    NEXUS_AudioDecoderStatus audioStatus;
#endif
    uint32_t stc;

    NEXUS_VideoDecoder_GetStatus(g_videoDecoder, &status);
    NEXUS_StcChannel_GetStc(g_stcChannel, &stc);
    printf("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo=%d%%\n",
        status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference, status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0);
#if NEXUS_HAS_AUDIO
    NEXUS_AudioDecoder_GetStatus(g_audioDecoder, &audioStatus);
    if ( audioStatus.started ) {
        printf("audio0            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
    }
    NEXUS_AudioDecoder_GetStatus(g_audioPassthrough, &audioStatus);
    if ( audioStatus.started ) {
        printf("audio1            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
    }
#endif
#if NEXUS_HAS_PLAYBACK
    if (g_playpump) {
        NEXUS_PlaypumpStatus playpumpStatus;
        if(NEXUS_Playpump_GetStatus(g_playpump,&playpumpStatus)==NEXUS_SUCCESS) {
            printf("playpump fifo=%d%% ", playpumpStatus.fifoSize?(playpumpStatus.fifoDepth*100)/playpumpStatus.fifoSize:0);
        }
    }
    if (g_playback) {
        NEXUS_PlaybackStatus playbackStatus;
        if(NEXUS_Playback_GetStatus(g_playback,&playbackStatus)==NEXUS_SUCCESS) {
            printf("playback position=%u.%03u (%u,%u) sec", (unsigned)playbackStatus.position/1000, (unsigned)playbackStatus.position%1000, (unsigned)playbackStatus.first/1000, (unsigned)playbackStatus.last/1000 );
        }
    }
    if(g_playback || g_playpump) {
        printf("\n");
    }
#endif
}

#if defined(LINUX)
#include <sys/time.h>
#elif defined(VXWORKS)
#include <timeCommon.h>
#elif defined (NEXUS_BASE_OS_ucos)
#include "type.h"
#include "ucos.h"
#endif
unsigned b_get_time()
{
#if defined(LINUX)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
#elif defined(VXWORKS)
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);
    return tv.tv_sec*1000 + tv.tv_nsec/1000/1000;
#elif defined(NEXUS_BASE_OS_ucos)
    const unsigned msPerTick = 10;
    long osTicks = OSTimeGet();
    return (osTicks * msPerTick);
#else
#error Please implement b_get_time for your OS
#endif
}

