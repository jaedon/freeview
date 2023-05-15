/******************************************************************************
 *    (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: decode_server.c $
 * $brcm_Revision: 33 $
 * $brcm_Date: 10/11/12 9:54a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/decode_server.c $
 * 
 * 33   10/11/12 9:54a erickson
 * SW7425-2734: report simpleAudioDecoder.id[0], add -frontend option and
 *  default off
 * 
 * 32   8/10/12 10:25a erickson
 * SW7425-2734: rework examples to use NEXUS_ClientResources for decoder
 *  ids
 * 
 * 31   6/11/12 10:49a erickson
 * SW7425-3174: enable frontend for tune_qam_client
 * 
 * 30   6/6/12 11:41a erickson
 * SW7425-2981: rework simple audio decoder input[] configuration
 * 
 * 29   5/11/12 3:32p erickson
 * SW7425-3017: bad checkin
 * 
 * 27   5/2/12 5:06p erickson
 * SW7425-2981: simplify, use refsw_server as model of configurability
 *
 * 26   4/20/12 10:08a erickson
 * SW7425-2882: create fully mapped heap and notify using app ipc
 *
 * 25   1/30/12 3:10p rjlewis
 * SW7425-1136: Added HDMI Support.
 *
 * 24   11/29/11 11:40a erickson
 * SW7420-1148: fix warning
 *
 * 23   11/18/11 1:24p erickson
 * SW7420-1148: add -timeout for automation
 *
 * 22   11/2/11 1:04p erickson
 * SW7231-438: standardize cmdline options
 *
 * 21   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 *
 * 20   9/13/11 1:23p erickson
 * SW7420-1148: change client modes to unprotected/protected/untrusted
 *
 * 19   8/25/11 10:54a erickson
 * SW7420-1148: default server to using untrusted clients
 *
 * 18   6/2/11 5:32p erickson
 * SW7420-1889: rework SimpleAudioPlayback to have separate
 *  Acquire/Release functions
 *
 * 17   4/26/11 4:27p jgarrett
 * SW7425-437: Making AC3 Transcode conditional
 *
 * 16   4/25/11 10:00p jgarrett
 * SW7425-437: Fixing shutdown sequence for audio
 *
 * 15   2/28/11 3:11p erickson
 * SW7420-1123: refactor server init/uninit for simpler, cleaner app
 *  init/uninit
 *
 * 14   2/15/11 2:23p erickson
 * SW7420-1135: fix warning
 *
 * 13   2/9/11 11:45a erickson
 * SW7405-5105: refactor SimpleAudioDecoder to support external, static
 *  configuration of mixed PCM. this allows decode and playback to start
 *  independently.
 *
 * 12   1/26/11 3:15p erickson
 * SW7420-1135: add HDMI output support
 *
 * 11   1/24/11 3:59p erickson
 * SW7405-5105: add simple audio playback client and server example code
 *
 * 10   1/19/11 2:03p erickson
 * SW7420-1123: socket-based usermode IPC
 *
 * 9   12/10/10 3:13p erickson
 * SW7420-1148: rename
 *
 * 8   12/7/10 5:47p erickson
 * SW7420-1148: refactor kernel mode proxy for process isolation
 *
 * 7   11/17/10 3:49p erickson
 * SW7420-1135: fix the cleanup
 *
 * 6   11/1/10 2:29p erickson
 * SW7420-1135: add TSM support
 *
 * 5   10/29/10 4:32p erickson
 * SW7420-1135: fix cleanup
 *
 * 4   10/22/10 11:19a erickson
 * SW7420-1135: update
 *
 * 3   10/20/10 4:05p erickson
 * SW7420-1135: add audio impl
 *
 * 2   10/13/10 10:34a erickson
 * SW7420-1135: add
 *
 * 1   9/29/10 9:34a erickson
 * SW7420-1123: add linux usermode IPC
 *
 *****************************************************************************/
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_platform_server.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_decoder.h"
#include "nexus_video_input.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_mixer.h"
#include "nexus_hdmi_output.h"
#include "nexus_simple_video_decoder_server.h"
#include "nexus_simple_audio_decoder_server.h"
#include "nexus_stc_channel.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

BDBG_MODULE(decode_server);

static void print_usage(void)
{
    printf(
    "usage: nexus decode_server [--help|-h] [-unprotected]\n"
    "options:\n"
    "  --help|-h      print this help screen\n"
    "  -unprotected   run clients in unprotected mode\n"
    "  -timeout X     exit after X seconds. default is to prompt for user.\n"
    "  -frontend      enable frontend\n"
    );
}

int main(int argc, char **argv)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformStartServerSettings serverSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_SimpleVideoDecoderHandle simpleVideoDecoder;
    NEXUS_AudioDecoderHandle audioDecoder0, audioDecoder1;
    NEXUS_AudioMixerHandle audioMixer;
    NEXUS_SimpleAudioDecoderHandle simpleAudioDecoder;
    NEXUS_AudioPlaybackHandle audioPlayback[2];
    NEXUS_SimpleAudioPlaybackHandle simpleAudioPlayback[2];
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_DisplaySettings displaySettings;
#endif
    NEXUS_Error rc;
    int curarg = 1;
    bool unprotected_clients = false;
    unsigned timeout = 0;
    bool openFrontend = false;

    while (curarg < argc) {
        if (!strcmp(argv[curarg], "--help") || !strcmp(argv[curarg], "-h")) {
            print_usage();
            return 0;
        }
        else if (!strcmp(argv[curarg], "-frontend")) {
            openFrontend = true;
        }
        else if (!strcmp(argv[curarg], "-timeout") && argc>curarg+1) {
            timeout = strtoul(argv[++curarg], NULL, 0);
        }
        else if (!strcmp(argv[curarg], "-unprotected")) {
            /* NOTE: unprotected clients cannot crash */
            unprotected_clients = true;
        }
        curarg++;
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    /* following refsw_server.c model */
    platformSettings.heap[NEXUS_MAX_HEAPS-1].size = 8 * 1024 * 1024;
    platformSettings.heap[NEXUS_MAX_HEAPS-1].memcIndex = 0;
    platformSettings.heap[NEXUS_MAX_HEAPS-1].guardBanding = false; /* corruptions shouldn't cause server failures */
    platformSettings.heap[NEXUS_MAX_HEAPS-1].memoryType = NEXUS_MemoryType_eFull; /* requires for packet blit and playpump */
    platformSettings.openFrontend = openFrontend;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* open display and connect video decoder to a window */
    display = NEXUS_Display_Open(0, NULL);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    BDBG_ASSERT(!rc);
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

    /* create simple video decoder */
    {
        NEXUS_SimpleVideoDecoderServerSettings settings;
        NEXUS_SimpleVideoDecoder_GetDefaultServerSettings(&settings);
        settings.videoDecoder = videoDecoder;
        settings.window[0] = window; /* SimpleVideoDecoder will do the connection */
        simpleVideoDecoder = NEXUS_SimpleVideoDecoder_Create(0, &settings);
    }

    /* create audio decoders */
    audioDecoder0 = NEXUS_AudioDecoder_Open(0, NULL);
    audioDecoder1 = NEXUS_AudioDecoder_Open(1, NULL);
    audioMixer = NEXUS_AudioMixer_Open(NULL);
    {
        NEXUS_AudioPlaybackOpenSettings openSettings;
        NEXUS_AudioPlayback_GetDefaultOpenSettings(&openSettings);
        openSettings.heap = platformConfig.heap[NEXUS_MAX_HEAPS-1]; /* client must have access */
        audioPlayback[0] = NEXUS_AudioPlayback_Open(NEXUS_ANY_ID, &openSettings);
        audioPlayback[1] = NEXUS_AudioPlayback_Open(NEXUS_ANY_ID, &openSettings);
    }

    /* create simple audio decoder */
    {
        NEXUS_SimpleAudioDecoderServerSettings settings;
        unsigned i;

        NEXUS_SimpleAudioDecoder_GetDefaultServerSettings(&settings);
        settings.primary = audioDecoder0;
        settings.secondary = audioDecoder1;

        /* any mixed output must be connected outside of the simple decoder and are not configurable per codec.
        they are used for primary decoder PCM output as well as PCM playback. */
        rc = NEXUS_AudioMixer_AddInput(audioMixer, NEXUS_AudioDecoder_GetConnector(audioDecoder0, NEXUS_AudioDecoderConnectorType_eStereo));
        BDBG_ASSERT(!rc);
        rc = NEXUS_AudioMixer_AddInput(audioMixer, NEXUS_AudioPlayback_GetConnector(audioPlayback[0]));
        BDBG_ASSERT(!rc);
        rc = NEXUS_AudioMixer_AddInput(audioMixer, NEXUS_AudioPlayback_GetConnector(audioPlayback[1]));
        BDBG_ASSERT(!rc);
        rc = NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]), NEXUS_AudioMixer_GetConnector(audioMixer));
        BDBG_ASSERT(!rc);
        
        /* for this example, HDMI/SPDIF will get compressed for ac3, mixed pcm for all other codecs. */
        for (i=0;i<NEXUS_AudioCodec_eMax;i++) {
            switch (i) {
            case NEXUS_AudioCodec_eAc3:
                settings.hdmi.input[i] = 
                settings.spdif.input[i] = NEXUS_AudioDecoder_GetConnector(audioDecoder1, NEXUS_AudioDecoderConnectorType_eCompressed);
                break;
            case NEXUS_AudioCodec_eUnknown: /* used for playback-only case */
            default:
                settings.hdmi.input[i] = 
                settings.spdif.input[i] = NEXUS_AudioMixer_GetConnector(audioMixer);
                break;
            }    
        }
        settings.hdmi.outputs[0] = platformConfig.outputs.hdmi[0];
        settings.spdif.outputs[0] = platformConfig.outputs.spdif[0];
        
        simpleAudioDecoder = NEXUS_SimpleAudioDecoder_Create(0, &settings);
    }

    /* create simple audio playback. it is linked to the audio decoder for timebase.
    but it is acquired separately by the app.
    if more than one SimpleAudioDecoder is created, the user should create an index scheme to separate them. */
    {
        NEXUS_SimpleAudioPlaybackServerSettings settings;

        NEXUS_SimpleAudioPlayback_GetDefaultServerSettings(&settings);
        settings.decoder = simpleAudioDecoder;
        settings.playback = audioPlayback[0];
        simpleAudioPlayback[0] = NEXUS_SimpleAudioPlayback_Create(0, &settings);

        NEXUS_SimpleAudioPlayback_GetDefaultServerSettings(&settings);
        settings.decoder = simpleAudioDecoder;
        settings.playback = audioPlayback[1];
        simpleAudioPlayback[1] = NEXUS_SimpleAudioPlayback_Create(1, &settings);
    }

    NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
    serverSettings.allowUnauthenticatedClients = true; /* client is written this way */
    serverSettings.unauthenticatedConfiguration.mode = unprotected_clients ? NEXUS_ClientMode_eUnprotected : NEXUS_ClientMode_eProtected;
    serverSettings.unauthenticatedConfiguration.heap[NEXUS_MAX_HEAPS-1] = platformConfig.heap[NEXUS_MAX_HEAPS-1]; /* for purposes of example, allow access to main heap */
    serverSettings.unauthenticatedConfiguration.resources.simpleVideoDecoder.id[0] = 0;
    serverSettings.unauthenticatedConfiguration.resources.simpleVideoDecoder.total = 1;
    serverSettings.unauthenticatedConfiguration.resources.simpleAudioDecoder.id[0] = 0;
    serverSettings.unauthenticatedConfiguration.resources.simpleAudioDecoder.total = 1;
    rc = NEXUS_Platform_StartServer(&serverSettings);
    BDBG_ASSERT(!rc);

    if (!timeout) {
        printf("Press ENTER to shutdown decode_server\n");
        getchar();
    }
    else {
        /* auto close */
        BKNI_Sleep(timeout*1000);
    }

    /* stop the server before closing resources that may be in use by clients.
    if it's an untrusted client, handle verification may fail the call. but a trusted client bypasses the
    check and could kill the server. */
    NEXUS_Platform_StopServer();

    NEXUS_SimpleVideoDecoder_Destroy(simpleVideoDecoder);
    NEXUS_SimpleAudioPlayback_Destroy(simpleAudioPlayback[0]);
    NEXUS_SimpleAudioPlayback_Destroy(simpleAudioPlayback[1]);
    NEXUS_SimpleAudioDecoder_Destroy(simpleAudioDecoder);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);

    NEXUS_AudioMixer_RemoveAllInputs(audioMixer);
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder0, NEXUS_AudioDecoderConnectorType_eMultichannel));
    NEXUS_AudioPlayback_Close(audioPlayback[0]);
    NEXUS_AudioPlayback_Close(audioPlayback[1]);
    NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(audioMixer));
    NEXUS_AudioMixer_Close(audioMixer);
    NEXUS_AudioDecoder_Close(audioDecoder0);
    NEXUS_AudioDecoder_Close(audioDecoder1);

    NEXUS_Platform_Uninit();
    return 0;
}

