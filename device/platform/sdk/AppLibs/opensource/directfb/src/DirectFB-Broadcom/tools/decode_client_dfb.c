/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: decode_client_dfb.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/2 $
 * $brcm_Date: 3/19/12 3:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/tools/decode_client_dfb.c $
 * 
 * DirectFB_1_4_15_Port/2   3/19/12 3:38p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree.
 *
 * DirectFB_1_4_15_Port/1   2/27/12 4:17p robertwm
 * SW7231-690: DirectFB-1.4.15: Add decode server/client test to help with
 *  DFB platform and Nexus MP usage.
 *
 * Based off of version 10 of Nexus decode_client.c
 *****************************************************************************/

/* This is a Nexus/DFB app that acts as a client and plays back video/audio from a designated TS, A/V PID's and codec's. */

#define inline __inline__

#include "platform_init.h"
#include "bcmnexus_app_defines.h"

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_AUDIO_DACS
#include "nexus_audio_dac.h"
#endif
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#if NEXUS_NUM_SPDIF_OUTPUTS
#include "nexus_spdif_output.h"
#endif
#include "nexus_component_output.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif
#if NEXUS_HAS_SIMPLE_DECODER
#include "nexus_simple_video_decoder.h"
#include "nexus_simple_audio_decoder.h"
#endif

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

#if NEXUS_HAS_PLAYBACK

/* Set DFB_PLATFORM_INIT to 1 to allow the DirectFB Platform code to initialise/join Nexus, rather than having to explicitly initialise/join Nexus */
#define DFB_PLATFOM_INIT    0

void print_usage(void)
{
     printf ("DirectFB/Nexus Decode Client Demo\n\n");
     printf ("Usage: decode_client_dfb [options]\n\n");
     printf ("Options:\n\n");
     printf ("  --file                       MPEG2 transport stream for playback.\n");
     printf ("  --apid                       Audio PID (decimal).\n");
     printf ("  --vpid                       Video PID (decimal).\n");
     printf ("  --vcodec                     Video Codec (decimal): 0=MPEG-2,   1=H.264, 2=MPEG-1, 3=VC-1,\n");
     printf ("  --acodec                     Audio Codec (decimal): 0=MPEG-1/2, 1=AC-3,  2=AC3+,   3=AAC\n");
     printf ("  --help                       Print usage information.\n");
     printf ("  --dfb-help                   Output DirectFB usage information.\n\n");
}

typedef struct vcodec_t
{
    NEXUS_VideoCodec codec;
    char *name;
} vcodec_t;

typedef struct acodec_t
{
    NEXUS_AudioCodec codec;
    char *name;
} acodec_t;

static const vcodec_t videoCodecs[] =
{
    { NEXUS_VideoCodec_eMpeg2,   "MPEG-2" },
    { NEXUS_VideoCodec_eH264,    "H.264"  },
    { NEXUS_VideoCodec_eMpeg1,   "MPEG-1" },
    { NEXUS_VideoCodec_eVc1,     "VC-1"   },
};

static const acodec_t audioCodecs[] =
{
    { NEXUS_AudioCodec_eMpeg,    "MPEG"   },
    { NEXUS_AudioCodec_eAc3,     "AC-3"   },
    { NEXUS_AudioCodec_eAc3Plus, "AC-3+"  },
    { NEXUS_AudioCodec_eAac,     "AAC"    },
};


/* This helper function is used to initialise the platform.
   If "initDFBfirst" is "true", then we initialise the platform using the DFB platform API.
   If "initDFBfirst" is "false", then we initialise Nexus first and then inform the DFB platform layer that Nexus has already been initialised.
*/
static int InitPlatform(bool initDFBfirst)
{
    DFB_PlatformClientType clientType;
    DFB_PlatformSettings   dfbPlatformSettings;
    DFB_PlatformStatus     dfbPlatformStatus;

    /* Get DFB Platform Status back first */
    DFB_Platform_GetStatus(&dfbPlatformStatus);

    if (dfbPlatformStatus.initOrJoinNexus == false)
    {
        fprintf(stderr, "%s(): ERROR: Nexus server not already initialised!  Have you run the \"decode_server_dfb\" application first?\n", __FUNCTION__);
        return -1;
    }

    /* Initialise Nexus using the DFB Platform code... */
    if (initDFBfirst)
    {
        clientType = DFB_PlatformClientType_eSlaveNexusUninitialized;
    }
    else    /* Initialise Nexus and then DFB platform code... */
    {
        clientType = DFB_PlatformClientType_eSlaveNexusInitialized;

        /* Now we join Nexus as a nexus client */
        NEXUS_Platform_Join();
    }
    /* Initialise DirectFB Platform code correctly... */
    DFB_Platform_GetDefaultSettings(clientType, &dfbPlatformSettings);
    DFB_Platform_Init(&dfbPlatformSettings);

    return 0;
}

/* This helper function instantiates an instance of a playpump interface. */
static NEXUS_PlaypumpHandle openPlaypump(int id)
{
    NEXUS_PlaypumpOpenSettings playpumpOpenSettings;
#if BCMNEXUS_PROTECTED_CLIENT_SUPPORT
    NEXUS_ClientConfiguration clientConfig;
#endif

    NEXUS_Playpump_GetDefaultOpenSettings(&playpumpOpenSettings);

    /* If we are a Nexus client and the Nexus server supports setting untrusted heap configuration for the client(s),
       then we can get the second heap that has been specially setup by the "decode_server_dfb" application for
       playing back video.  This helps to isolates the system, by preventing all untrusted clients from accessing the
       default main heap.
    */
#if BCMNEXUS_PROTECTED_CLIENT_SUPPORT
    NEXUS_Platform_GetClientConfiguration(&clientConfig);
    playpumpOpenSettings.heap = clientConfig.heap[1];
    playpumpOpenSettings.boundsHeap = clientConfig.heap[1];
#endif
    return NEXUS_Playpump_Open(id, &playpumpOpenSettings);
}

/* This helper function provides an instantiation of a video decoder interface.
   In addition, it will ensure that a video window is connected to the output
   of the video decoder (either explictly or implicitly).
*/
static void *openVideoDecoder(int id, NEXUS_VideoWindowHandle window)
{
    void *videoDecoder = NULL;

#if NEXUS_HAS_SIMPLE_DECODER
    (void)window;
    videoDecoder = NEXUS_SimpleVideoDecoder_Acquire(id);
#else
    videoDecoder = NEXUS_VideoDecoder_Open(id, NULL); /* take default capabilities */
    if (videoDecoder)
        NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
#endif
    if (!videoDecoder)
        fprintf(stderr, "video decoder not available\n");

    return videoDecoder;
}

/* This helper function will provide a video pid channel interface. */
static NEXUS_PidChannelHandle openVideoPidChannel(NEXUS_PlaybackHandle playback, int vpid, int vcodecIdx, void *videoDecoder)
{
    NEXUS_PidChannelHandle videoPidChannel = NULL;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;

    (void)videoDecoder;

    if (vpid != -1)
    {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidSettings.pidTypeSettings.video.codec = videoCodecs[vcodecIdx].codec; /* must be told codec for correct handling */
        playbackPidSettings.pidTypeSettings.video.index = true;
#if NEXUS_HAS_SIMPLE_DECODER
#if BCMNEXUS_PLAYBACK_PID_CHANNEL_HAS_SIMPLE_DECODER
        playbackPidSettings.pidTypeSettings.video.simpleDecoder = videoDecoder;
#endif
#else
        playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
#endif
        videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, vpid, &playbackPidSettings);
    }
    return videoPidChannel;
}

/* Open up an audio decoder instance and connect the outputs up */
static void *openAudioDecoder(int id)
{
    void *audioDecoder;
#if !NEXUS_HAS_SIMPLE_DECODER
    NEXUS_AudioDecoderHandle    audioDecoder;
    NEXUS_PlatformConfiguration platformConfig;

    audioDecoder = NEXUS_AudioDecoder_Open(id, NULL);

    if (audioDecoder == NULL)
        fprintf(stderr, "%s(): ERROR: Cannot open audio decoder!\n", __FUNCTION__);
    else
    {
        NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_NUM_AUDIO_DACS
        NEXUS_AudioOutput_AddInput(
            NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
        NEXUS_AudioOutput_AddInput(
            NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif
    }
#else
    audioDecoder = NEXUS_SimpleAudioDecoder_Acquire(id);
#endif
    if (!audioDecoder)
        fprintf(stderr, "audio decoder not available\n");

    return audioDecoder;
}

/* This helper function will provide an audio pid channel interface instantiation. */
static NEXUS_PidChannelHandle openAudioPidChannel(NEXUS_PlaybackHandle playback, int apid, int acodecIdx, void *audioDecoder)
{
    NEXUS_PidChannelHandle audioPidChannel = NULL;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;

    (void)acodecIdx;
    (void)audioDecoder;

    if (apid != -1)
    {
        /* Step 7b: Open the audio pid channel */
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
#if NEXUS_HAS_SIMPLE_DECODER
#if BCMNEXUS_PLAYBACK_PID_CHANNEL_HAS_SIMPLE_DECODER
        playbackPidSettings.pidTypeSettings.audio.simpleDecoder = audioDecoder;
#endif
#else
        playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
#endif
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, apid, &playbackPidSettings);
    }
    return audioPidChannel;
}

/* This helper function will provide an STC channel interface instantiation. */
static NEXUS_StcChannelHandle openStcChannel(int id)
{
    NEXUS_StcChannelSettings stcSettings;

    NEXUS_StcChannel_GetDefaultSettings(id, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    return NEXUS_StcChannel_Open(id, &stcSettings);
}

/* This helper function will start a video decoder instance. */
static NEXUS_Error startVideoDecoder(void *videoDecoder, NEXUS_PidChannelHandle videoPidChannel, int vcodecIdx, NEXUS_StcChannelHandle stcChannel)
{
    NEXUS_Error rc;

#if NEXUS_HAS_SIMPLE_DECODER
    NEXUS_SimpleVideoDecoderStartSettings videoProgram;
    (void)stcChannel;

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_SimpleVideoDecoder_GetDefaultStartSettings(&videoProgram);
#if BCMNEXUS_SIMPLE_VIDEO_DECODER_HAS_START_SETTINGS
    videoProgram.settings.codec = videoCodecs[vcodecIdx].codec;
    videoProgram.settings.pidChannel = videoPidChannel;
    videoProgram.settings.stcChannel = stcChannel;
#else
    videoProgram.codec = videoCodecs[vcodecIdx].codec;
    videoProgram.pidChannel = videoPidChannel;
#endif
    rc = NEXUS_SimpleVideoDecoder_Start(videoDecoder, &videoProgram);
#else
    NEXUS_VideoDecoderStartSettings videoProgram;

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = videoCodecs[vcodecIdx].codec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
#endif
    return rc;
}

/* This helper function will start an audio decoder instance. */
static NEXUS_Error startAudioDecoder(void *audioDecoder, NEXUS_PidChannelHandle audioPidChannel, int acodecIdx, NEXUS_StcChannelHandle stcChannel,
        void *videoDecoder)
{
    NEXUS_Error rc;

    (void)videoDecoder;
#if NEXUS_HAS_SIMPLE_DECODER
    NEXUS_SimpleAudioDecoderStartSettings audioProgram;
    (void)stcChannel;

    NEXUS_SimpleAudioDecoder_GetDefaultStartSettings(&audioProgram);
#if BCMNEXUS_SIMPLE_AUDIO_DECODER_HAS_START_SETTINGS
    audioProgram.primary.codec = audioCodecs[acodecIdx].codec;
    audioProgram.primary.pidChannel = audioPidChannel;
    audioProgram.primary.stcChannel = stcChannel;
#else
    audioProgram.codec = audioCodecs[acodecIdx].codec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.tsm.video = videoDecoder; /* link the two decoders */
#endif
    rc = NEXUS_SimpleAudioDecoder_Start(audioDecoder, &audioProgram);
#else
    NEXUS_AudioDecoderStartSettings audioProgram;

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = audioCodecs[acodecIdx].codec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
#endif
    return rc;
}

/* This helper function will stop a video decoder instance. */
static void stopVideoDecoder(void *videoDecoder)
{
#if NEXUS_HAS_SIMPLE_DECODER
    NEXUS_SimpleVideoDecoder_Stop(videoDecoder);
#else
    NEXUS_VideoDecoder_Stop(videoDecoder);
#endif
}

/* This helper function will stop an audio decoder instance. */
static void stopAudioDecoder(void *audioDecoder)
{
#if NEXUS_HAS_SIMPLE_DECODER
    NEXUS_SimpleAudioDecoder_Stop(audioDecoder);
#else
    NEXUS_AudioDecoder_Stop(audioDecoder);
#endif
}

/* This helper function will close a video decoder instance. */
static void closeVideoDecoder(void *videoDecoder, NEXUS_VideoWindowHandle window)
{
#if NEXUS_HAS_SIMPLE_DECODER
    (void)window;
    NEXUS_SimpleVideoDecoder_Release(videoDecoder);
#else
    NEXUS_VideoWindow_RemoveInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
#endif
}

/* This helper function will close an audio decoder instance. */
static void closeAudioDecoder(void *audioDecoder)
{
#if !NEXUS_HAS_SIMPLE_DECODER
    NEXUS_PlatformConfiguration platformConfig;

    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_NUM_AUDIO_DACS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]));
#endif
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
#else
    NEXUS_SimpleAudioDecoder_Release(audioDecoder);
#endif
}

#endif /* NEXUS_HAS_PLAYBACK */


/*** Main function ***/

int main(int argc, char *argv[])
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_Error rc;
    DFB_PlatformSettings dfbPlatformSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_PidChannelHandle audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoCodec vcodecIdx = 0; /* Default to MPEG-2 Video */
    NEXUS_AudioCodec acodecIdx = 0; /* Default to MPEG Audio */
    void *videoDecoder;
    void *audioDecoder;
    char fname[64];
    int n,apid=-1,vpid=-1;
    char inputc;
    bool bInput = true;
#if DFB_PLATFOM_INIT
    bool initDFBfirst = true;
#else
    bool initDFBfirst = false;
#endif

    /* parse command line */
    for (n = 1; n < argc; n++) {
        if (strncmp (argv[n], "--", 2) == 0) {
            if (strcmp (argv[n] + 2, "help") == 0) {
                print_usage();
                return 0;
            }
            else
            {
                if (strcmp (argv[n] + 2, "file") == 0 &&
                    ++n < argc &&
                    sscanf (argv[n], "%s", fname) == 1) {
                    continue;
                }
                else if (strcmp (argv[n] + 2, "vpid") == 0 &&
                         ++n < argc &&
                         sscanf (argv[n], "%d", &vpid) == 1) {
                    continue;
                }
                else if (strcmp (argv[n] + 2, "apid") == 0 &&
                         ++n < argc &&
                         sscanf (argv[n], "%d", &apid) == 1) {
                    continue;
                }
                else if (strcmp (argv[n] + 2, "vcodec") == 0 &&
                         ++n < argc &&
                         sscanf (argv[n], "%u", &vcodecIdx) == 1) {
                    if (vcodecIdx > sizeof(videoCodecs)/sizeof(videoCodecs[0])) {
                        fprintf(stderr, "ERROR: Invalid video codec specified!\n");
                        print_usage();
                        return -1;
                    }
                    continue;
                }
                else if (strcmp (argv[n] + 2, "acodec") == 0 &&
                         ++n < argc &&
                         sscanf (argv[n], "%u", &acodecIdx) == 1) {
                    if (acodecIdx > sizeof(audioCodecs)/sizeof(audioCodecs[0])) {
                        fprintf(stderr, "ERROR: Invalid audio codec specified!\n");
                        print_usage();
                        return -1;
                    }
                    continue;
                }
            }
        }
    }

    if (argc < 2)
    {
        printf("Need to supply at least a filename & one PID!!\n\n");
        print_usage();
        return -1;
    }

    printf("Playback of %s - audio %d(codec=%s) - video %d(codec=%s)\n", fname, apid, audioCodecs[acodecIdx].name, vpid, videoCodecs[vcodecIdx].name);

    /* Initialise the platform */
    rc = InitPlatform(initDFBfirst);

    if (rc)
        return rc;

    /*** We need to initialise the playing back of A/V from a transport stream file.  Here are the steps to do it... ***/

    /* Step 1: Open up a playpump instance */
    playpump = openPlaypump(NEXUS_ANY_ID);
    assert(playpump);

    /* Step 2: Open up a playback instance */
    playback = NEXUS_Playback_Create();
    assert(playback);

    /* Step 3: Open up the transport stream file for playing back */
    file = NEXUS_FilePlay_OpenPosix(fname, NULL);
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }

    /* Step 4: Open up an STC channel instance */
    stcChannel = openStcChannel(0);
    assert(stcChannel);

    /* Step 5: Now associate the playpump and STC channels with the playback instance */
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* Step 6: Open up a video decoder */
    DFB_Platform_GetSettings(&dfbPlatformSettings);
    window = dfbPlatformSettings.displayWindow[0].handle;

    videoDecoder = openVideoDecoder(NEXUS_ANY_ID, window);
    assert(videoDecoder);

    /* Step 7: Open up a video pid channel */
    videoPidChannel = openVideoPidChannel(playback, vpid, vcodecIdx, videoDecoder);

    /* Step 8: Open up an audio decoder */
    audioDecoder = openAudioDecoder(NEXUS_ANY_ID);

    /* Step 9: Open up an audio pid channel */
    audioPidChannel = openAudioPidChannel(playback, apid, acodecIdx, audioDecoder);

    /* Step 10: Start the video decoder */
    if (videoPidChannel)
    {
        rc = startVideoDecoder(videoDecoder, videoPidChannel, vcodecIdx, stcChannel);
        assert(!rc);
    }

    /* Step 11: Start the audio decoder */
    if (audioPidChannel)
    {
        rc = startAudioDecoder(audioDecoder, audioPidChannel, acodecIdx, stcChannel, videoDecoder);
        assert(!rc);
    }

    /* Step 12: Start playback */
    NEXUS_Playback_Start(playback, file, NULL);

    /* Get the display handle from the DFB platform layer */
    display = dfbPlatformSettings.display[0].handle;

    while (bInput)
    {
        printf("Q) Quit Application\n");
        printf("  ==> \n");

        inputc = getchar();
        switch (inputc)
        {
            case 'Q':
            case 'q':
                bInput = false;
                break;
        }
    }

    /*** Bring down the system in the correct order... ***/

    /* Step 1: Stop the Video decoder first */
    if (videoPidChannel)
        stopVideoDecoder(videoDecoder);

    /* Step 2: Stop the Audio decoder next */
    if (audioPidChannel)
        stopAudioDecoder(audioDecoder);

    /* Step 3: Stop the playback of the stream */
    NEXUS_Playback_Stop(playback);

    /* Step 4: Close the Video PID channel */
    if (videoPidChannel)
        NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);

    /* Step 5: Close the Audio PID channel */
    if (audioPidChannel)
        NEXUS_Playback_ClosePidChannel(playback, audioPidChannel);

    /* Step 6: Close the Video decoder and remove and connected windows */
    closeVideoDecoder(videoDecoder, window);

    /* Step 7: Close the Audio decoder and any connected outputs */
    closeAudioDecoder(audioDecoder);

    /* Step 8: Close the STC channel used by playback and simple A/V decoders */
    NEXUS_StcChannel_Close(stcChannel);

    /* Step 9: Close the stream file */
    NEXUS_FilePlay_Close(file);

    /* Step 10: Close the playback instance */
    NEXUS_Playback_Destroy(playback);

    /* Step 11: Close the playpump instance */
    NEXUS_Playpump_Close(playpump);

    /* Step 12: Uninitialise the DirectFB Platform layer */
    DFB_Platform_Uninit();

    /* Step 13: If we initialised Nexus first, then we need to uninitialise it too */
    if (!initDFBfirst)
        NEXUS_Platform_Uninit();
#else
    fprintf(stderr, "ERROR: Cannot run %s() as Nexus playback is not supported!\n", argv[0]);
#endif
    return 0;
}
