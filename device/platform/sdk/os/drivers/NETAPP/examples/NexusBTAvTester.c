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
 *
 *****************************************************************************/
/**
 * @brief Nexus Bluetooth AV Source Test App
 * @author Steven Hartley (steven@broadcom.com)
 *
 * The following is a an app to test Nexus Audio Capture to Bluetooth streaming
 * to headset.
 * This example file is based on Nexus audio_capture.c and audio_playback.c
 * 
 * NOTE: You must run NetAppTester First to pair with the headset before running
 * this test app. After you killed NetAppTester, the paired information will be
 * saved in the NetApp database and calling NetAppOpen() would re-pair with the
 * bluetooth device and audio is streamed to the headset right away.
 * The test app can only playback a predetermined file for now... sorry :-(
 */

#include "netapp.h"
#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_capture.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#include "nexus_core_utils.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

/* the following define the input file and its characteristics -- these will vary by input file */
#define FILE_NAME "cnnticker.mpg"
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eMpeg
#define VIDEO_PID 0x21
#define AUDIO_PID 0x22

typedef struct
{
    NEXUS_AudioCaptureHandle    hCapture;
    NETAPP_HANDLE               hNetApp;
    bool                        bConnected;
} NEXUS_BT_AV_HANDLE;

static void capture_callback(void *pParam, int param)
{
    NEXUS_BT_AV_HANDLE *pHandle = (NEXUS_BT_AV_HANDLE *)pParam;

    NEXUS_Error errCode;

    for ( ;; )
    {
        void *pBuffer;
        size_t bufferSize;

        /* Check available buffer space */
        errCode = NEXUS_AudioCapture_GetBuffer(pHandle->hCapture, (void **)&pBuffer, &bufferSize);
        if ( errCode )
        {
            fprintf(stderr, "Error getting capture buffer\n");
            NEXUS_AudioCapture_Stop(pHandle->hCapture);
            return;
        }

        if ( bufferSize > 0 )
        {
            /* Write samples to NetApp */
            if (pHandle->bConnected)
            {
                NetAppBluetoothSendAudioBuffer(pHandle->hNetApp, pBuffer, bufferSize);
            }

            /*fprintf(stderr, "Data callback - Wrote %d bytes\n", (int)bufferSize);*/
            errCode = NEXUS_AudioCapture_ReadComplete(pHandle->hCapture, bufferSize);
            if ( errCode )
            {
                fprintf(stderr, "Error committing capture buffer\n");
                NEXUS_AudioCapture_Stop(pHandle->hCapture);
                return;
            }
        }
        else
        {
            break;
        }
    }
}


void netapp_callback(
    void                *pParam,        //!< The pParam from NetAppOpen
    NETAPP_CB_TYPE      tCbType,        //!< The Callback type
    const void          *pvBuffer,      //!< Pointer to data specific to the callback
    uint32_t            ulData0,        //!< Callback specific data 0
    NETAPP_RETCODE      tResult,        //!< Callback results from the
    NETAPP_IFACE        tIFace)         //!< Callback Info structure
{
    NEXUS_BT_AV_HANDLE *pContext = (NEXUS_BT_AV_HANDLE*)pParam;

    switch(tCbType)
    {
    case NETAPP_CB_CONNECT:
    {
        NETAPP_BT_AUDIO_FORMAT  tBtAudioFormat = { NETAPP_BT_AV_MODE_STEREO, 48000, 16 };
        printf("%s() Received NETAPP_CB_CONNECT (%s) result=%d\n",
            __FUNCTION__, (tIFace == NETAPP_IFACE_WIRELESS) ? "Wireless" : "Bluetooth", tResult);

        if (tIFace == NETAPP_IFACE_BLUETOOTH)
        {
            if (tResult == NETAPP_SUCCESS)
            {
                NETAPP_BT_DEV_INFO  *pDevInfo = (NETAPP_BT_DEV_INFO*) pvBuffer;
                NEXUS_AudioCaptureStatus captureStatus;
                printf("\n***CONNECTED AV HEADSET******\n");
                printf("Name:       %s\n", pDevInfo->cName);
                printf("HD Addr:    %s\n", pDevInfo->cAddr);
                printf("RSSI:       %d dB\n", pDevInfo->lRssi);
                printf("****************************\n");

                NEXUS_AudioCapture_GetStatus(pContext->hCapture, &captureStatus);
                tBtAudioFormat.ulSampleRate = captureStatus.sampleRate;
                NetAppBluetoothAvStart(pContext->hNetApp, true, &tBtAudioFormat);
                pContext->bConnected = true;
            }
        }
        break;
    }
    default:
        printf("Unhandled callback type %d\n", tCbType);
        break;
    }
}


void sample_rate_changed_callback(void *context, int param)
{
    printf("%s(): entered\n", __FUNCTION__);
}

#ifndef NEXUS_HAS_PLAYBACK
#error "This application is not supported on this platform!\n"
#endif

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Error rc;
#endif
    NEXUS_AudioCaptureStartSettings captureSettings;
    NEXUS_BT_AV_HANDLE context;
    NETAPP_SETTINGS     tNetAppSettings;
    NETAPP_INIT_SETTINGS tInitSettings;
    NETAPP_OPEN_SETTINGS tOpenSettings;

    NEXUS_AudioCaptureOpenSettings tCaptureOpenSettings;

    const char *fname = FILE_NAME;

    memset(&context, 0, sizeof(NEXUS_BT_AV_HANDLE));

    /************* NETAPP OPEN CODE *********************/
    /* Create NetApp */
    NetAppGetDefaultSettings(&tNetAppSettings);
    NetAppGetDefaultInitSettings(&tInitSettings);

    tOpenSettings.tCallback                 = netapp_callback;
    tNetAppSettings.bForceWiFi              = true;
    tNetAppSettings.tBtSettings.bAutoPair   = true;
    tInitSettings.WiFiIfacePrefix           = "sdh";
    if (NetAppOpen(&context.hNetApp, &tOpenSettings, &tInitSettings, &tNetAppSettings) != NETAPP_SUCCESS)
    {
        printf("NetApp Open Failed\n");
        goto done;
    }
    /************* NETAPP OPEN CODE END *****************/

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    file = NEXUS_FilePlay_OpenPosix(fname, NULL);
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = TRANSPORT_TYPE;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);


    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif

    /* Bring up video display and outputs */
    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
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

/************* AUDIO CAPTURE CODE *********************/
    NEXUS_AudioCapture_GetDefaultOpenSettings(&tCaptureOpenSettings);
        /* TODO: Please with the fifo size and threshold to fix BT auddio issues */
    tCaptureOpenSettings.fifoSize = 60*1024;
    tCaptureOpenSettings.threshold = 50;
    tCaptureOpenSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_eStereo;
    context.hCapture = NEXUS_AudioCapture_Open(0, &tCaptureOpenSettings);
    NEXUS_AudioOutput_AddInput(
            NEXUS_AudioCapture_GetConnector(context.hCapture),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    NEXUS_AudioCapture_GetDefaultStartSettings(&captureSettings);
    captureSettings.dataCallback.callback = capture_callback;
    captureSettings.dataCallback.context = &context;
    captureSettings.sampleRateCallback.callback = sample_rate_changed_callback;
    captureSettings.sampleRateCallback.context = &context;
/************* AUDIO CAPTURE CODE END*********************/

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    /* Open the audio and video pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = VIDEO_CODEC; /* must be told codec for correct handling */
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, VIDEO_PID, &playbackPidSettings);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
    audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, AUDIO_PID, &playbackPidSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = AUDIO_CODEC;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Start decoders */
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

    /* Start playback */
    NEXUS_Playback_Start(playback, file, NULL);
    NEXUS_AudioCapture_Start(context.hCapture, &captureSettings);
    printf("Press ENTER to quit\n");
    getchar();

done:
    if (context.hNetApp)
    {
        NetAppClose(context.hNetApp);
    }

    if (context.hCapture) {
        NEXUS_StopCallbacks(context.hCapture);
        NEXUS_AudioCapture_Stop(context.hCapture);
    }

    /* Bring down system */
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_Platform_Uninit();

    return 0;
}
