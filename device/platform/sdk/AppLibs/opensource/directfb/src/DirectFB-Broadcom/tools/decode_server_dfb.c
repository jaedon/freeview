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
 * $brcm_Workfile: decode_server_dfb.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/2 $
 * $brcm_Date: 3/21/12 2:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/tools/decode_server_dfb.c $
 * 
 * DirectFB_1_4_15_Port/2   3/21/12 2:45p christ
 * SW7425-2675: DirectFB-1.4.15: decode_server_dfb fails to run on 7425
 *
 * DirectFB_1_4_15_Port/1   2/27/12 4:17p robertwm
 * SW7231-690: DirectFB-1.4.15: Add decode server/client test to help with
 *  DFB platform and Nexus MP usage.
 *
 * Based on version 25 of Nexus decode_server.c
 *****************************************************************************/
#include "platform_init.h"

#include "nexus_types.h"
#include "nexus_platform.h"
#if BCMNEXUS_HAS_IPC
#include "nexus_platform_server.h"
#endif
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_decoder.h"
#include "nexus_video_input.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_mixer.h"
#include "nexus_ac3_encode.h"
#if NEXUS_HAS_SIMPLE_DECODER
#include "nexus_simple_video_decoder_server.h"
#include "nexus_simple_audio_decoder_server.h"
#endif
#include "nexus_stc_channel.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "directfb.h"


BDBG_MODULE(decode_server_dfb);


static void print_usage(void)
{
    printf(
    "usage: nexus decode_server_dfb [--help|-h] [-unprotected]\n"
    "options:\n"
    "  --help|-h      print this help screen\n"
    "  -unprotected   run clients in unprotected mode\n"
    "  -timeout X     exit after X seconds. default is to prompt for user.\n"
    );
}

int main(int argc, char **argv)
{
#if BCMNEXUS_HAS_IPC && NEXUS_HAS_SIMPLE_DECODER
    NEXUS_PlatformSettings nexusPlatformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformStartServerSettings serverSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_SimpleVideoDecoderHandle simpleVideoDecoder;
    NEXUS_AudioDecoderHandle audioDecoder0, audioDecoder1;
    NEXUS_AudioMixerHandle audioMixer;
    NEXUS_Ac3EncodeHandle ac3Encoder;
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
    int heapIdx,heapIdxLarge;

    DFB_PlatformSettings platformSettings;
    DFB_PlatformStatus   dfbPlatformStatus;
    IDirectFB *dfb;

    while (curarg < argc) {
        if (!strcmp(argv[curarg], "--help") || !strcmp(argv[curarg], "-h")) {
            print_usage();
            return 0;
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


    /* Get DFB Platform Status back first */
    DFB_Platform_GetStatus(&dfbPlatformStatus);

    if (dfbPlatformStatus.initOrJoinNexus == true)
    {
        fprintf(stderr, "%s(): ERROR: DFB Platform code already initialised!  This must be the first app to run on the system!\n", __FUNCTION__);
        return -1;
    }

    NEXUS_Platform_GetDefaultSettings(&nexusPlatformSettings);

#if BCMNEXUS_PROTECTED_CLIENT_SUPPORT
    /* Find a heap which fills up to the end of the bmem partition so we know
     * we can squeeze in our own heap in this partition */
    for (heapIdxLarge = 0; heapIdxLarge < NEXUS_MAX_HEAPS; ++heapIdxLarge)
    {
        if (nexusPlatformSettings.heap[heapIdxLarge].size == -1)
                break;
    }

    /* Find spare slot */
    for (heapIdx = 0; heapIdx < NEXUS_MAX_HEAPS; heapIdx++)
    {
        if (nexusPlatformSettings.heap[heapIdx].size == 0)
                break;
    }

    /* Create small heap with full access for CPU for playback buffers */
    nexusPlatformSettings.heap[heapIdx].memcIndex = nexusPlatformSettings.heap[heapIdxLarge].memcIndex;
    nexusPlatformSettings.heap[heapIdx].subIndex  = nexusPlatformSettings.heap[heapIdxLarge].subIndex;
    nexusPlatformSettings.heap[heapIdx].size = 9 * 1024 * 1024;
    nexusPlatformSettings.heap[heapIdx].memoryType = NEXUS_MemoryType_eFull;

    printf("Creating Nexus heap[%d] memc=%d, sub=%d, size=%d, type=0x%08x\n", heapIdx,
                nexusPlatformSettings.heap[heapIdx].memcIndex,
                nexusPlatformSettings.heap[heapIdx].subIndex,
                nexusPlatformSettings.heap[heapIdx].size,
                nexusPlatformSettings.heap[heapIdx].memoryType);
#endif

    rc = NEXUS_Platform_Init(&nexusPlatformSettings);
    if (rc) return -1;
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* open display and connect video decoder to a window */
    display = NEXUS_Display_Open(0, NULL);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
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
    ac3Encoder = NEXUS_Ac3Encode_Open(NULL);
    audioPlayback[0] = NEXUS_AudioPlayback_Open(NEXUS_ANY_ID, NULL);
    audioPlayback[1] = NEXUS_AudioPlayback_Open(NEXUS_ANY_ID, NULL);

    /* create simple audio decoder */
    {
        NEXUS_SimpleAudioDecoderServerSettings settings;

#define ENABLE_AC3_TRANSCODING (0)  /* Change this to 1 to enable AC3 Transcoding.  Disconnected nodes are not supported on all platforms. */
#if ENABLE_AC3_TRANSCODING
        NEXUS_Ac3Encode_AddInput(ac3Encoder, NEXUS_AudioDecoder_GetConnector(audioDecoder0, NEXUS_AudioDecoderConnectorType_eMultichannel));
#endif

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
#if NEXUS_NUM_AUDIO_DACS
        rc = NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]), NEXUS_AudioMixer_GetConnector(audioMixer));
        BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
#define HDMI_HAS_MIXED_PCM 1
#if HDMI_HAS_MIXED_PCM
        rc = NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]), NEXUS_AudioMixer_GetConnector(audioMixer));
        BDBG_ASSERT(!rc);
#else
        settings.hdmi.outputs[0] = platformConfig.outputs.hdmi[0];
        /* leave settings.hdmi.input[] = NULL, which will output unmixed PCM */
#endif
#endif

#if NEXUS_NUM_SPDIF_OUTPUTS
        /* for this example, SPDIF will be an unmixed output, which is configurable per codec */
        settings.spdif.outputs[0] = platformConfig.outputs.spdif[0];
        /* if input[] is unspecified, it will be PCM from primary decoder */
        settings.spdif.input[NEXUS_AudioCodec_eAc3] = NEXUS_AudioDecoder_GetConnector(audioDecoder1, NEXUS_AudioDecoderConnectorType_eCompressed);
        settings.spdif.input[NEXUS_AudioCodec_eAc3Plus] = NEXUS_AudioDecoder_GetConnector(audioDecoder0, NEXUS_AudioDecoderConnectorType_eCompressed);
#if ENABLE_AC3_TRANSCODING
        settings.spdif.input[NEXUS_AudioCodec_eAacAdts] =
        settings.spdif.input[NEXUS_AudioCodec_eAacLoas] =
        settings.spdif.input[NEXUS_AudioCodec_eAacPlusAdts] =
        settings.spdif.input[NEXUS_AudioCodec_eAacPlusLoas] = NEXUS_Ac3Encode_GetConnector(ac3Encoder);
#endif
#endif
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
#if BCMNEXUS_PROTECTED_CLIENT_SUPPORT
    serverSettings.unauthenticatedConfiguration.mode = unprotected_clients ? NEXUS_ClientMode_eUnprotected : NEXUS_ClientMode_eUntrusted;

    /* Also allow access to the graphics off-screen heap for DFB slaves */
    serverSettings.unauthenticatedConfiguration.heap[0] = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
    serverSettings.unauthenticatedConfiguration.heap[1] = platformConfig.heap[heapIdx]; /* for purposes of example, allow access to main heap */
#endif

    for (heapIdx = 0; heapIdx < NEXUS_MAX_HEAPS; heapIdx++)
        printf("Nexus heap[%d]=%p\n", heapIdx, (void *)platformConfig.heap[heapIdx]);


    rc = NEXUS_Platform_StartServer(&serverSettings);
    BDBG_ASSERT(!rc);

    /* Now we can initialise DFB platform code telling it that we have already initialised Nexus as a master. */
    DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eMasterNexusInitialized, &platformSettings);
    platformSettings.display[0].handle = display;
    platformSettings.displayWindow[0].handle = window;
    platformSettings.displayOutput[0].init = false;

#if NEXUS_NUM_HDMI_OUTPUTS
    /* Indicate that we have a HDMI output connected to primary display */
    platformSettings.displayOutput[0].init = true;
    platformSettings.displayOutput[0].handle = platformConfig.outputs.hdmi[0];
    platformSettings.displayOutput[0].initSettings.outputType = DFB_PlatformDisplayOutputType_eHdmi;
    platformSettings.displayOutput[0].initSettings.outputIndex = 0;
    platformSettings.displayOutput[0].initSettings.displayIndex = 0;
#endif

    platformSettings.displayOutput[1].init = false;

#if NEXUS_NUM_COMPONENT_OUTPUTS
    /* Indicate that we have a component output connected to primary display */
    if (platformConfig.outputs.component[0]){
        platformSettings.displayOutput[1].init   = true;
        platformSettings.displayOutput[1].handle = platformConfig.outputs.component[0];
        platformSettings.displayOutput[1].initSettings.outputType = DFB_PlatformDisplayOutputType_eComponent;
        platformSettings.displayOutput[1].initSettings.outputIndex = 0;
        platformSettings.displayOutput[1].initSettings.displayIndex = 0;
    }
#endif

    /* Indicate that we have don't have a composite output connected to primary display */
    platformSettings.displayOutput[2].init = false;

    /* Now setup the heap handles for the DFB master */
    platformSettings.displayHeapHandle   = NEXUS_Platform_GetFramebufferHeap(0);
    platformSettings.offscreenHeapHandle = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
    platformSettings.paletteHeapHandle   = platformSettings.offscreenHeapHandle;

    /* Now initialise DFB with settings from Nexus */
    DFB_Platform_Init(&platformSettings);

    /* Finally we need to ensure that the DFB master application is the same as the Nexus server */
    DirectFBInit( &argc, &argv );
    DirectFBCreate(&dfb);

#if LOCAL_CLIENT
    run(0, NULL);
#else
    /* could run settop 101 here */
    printf("Press ENTER to shutdown server\n");
    getchar();
#endif

    dfb->Release( dfb );
    DFB_Platform_Uninit();

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

    NEXUS_Ac3Encode_RemoveAllInputs(ac3Encoder);
    NEXUS_AudioInput_Shutdown(NEXUS_Ac3Encode_GetConnector(ac3Encoder));
    NEXUS_Ac3Encode_Close(ac3Encoder);
    NEXUS_AudioMixer_RemoveAllInputs(audioMixer);
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder0, NEXUS_AudioDecoderConnectorType_eMultichannel));
    NEXUS_AudioPlayback_Close(audioPlayback[0]);
    NEXUS_AudioPlayback_Close(audioPlayback[1]);
    NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(audioMixer));
    NEXUS_AudioMixer_Close(audioMixer);
    NEXUS_AudioDecoder_Close(audioDecoder0);
    NEXUS_AudioDecoder_Close(audioDecoder1);

    NEXUS_Platform_Uninit();
#else
    fprintf(stderr, "%s(): Simple video decoder API not available to run this application!\n", argv[0]);
#endif
    return 0;
}

