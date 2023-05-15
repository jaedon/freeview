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
 * $brcm_Workfile: timeshift_encrypted.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 1/26/12 3:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/dvr/timeshift_encrypted.c $
 * 
 * 5   1/26/12 3:00p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 4   12/8/11 3:13p mphillip
 * SW7346-481: 7346/7425 B0 support exposed a lack of
 *  NEXUS_Security_GetDefaultClearKey calls
 * 
 * 3   3/22/10 2:35a randyjew
 * SW7468-143: Support for boards without component and/or composite
 * 
 * 2   2/19/09 2:00p erickson
 * PR51841: refactor
 *
 * 7   2/19/09 11:54a erickson
 * PR51841: refactor app
 *
 * 6   2/4/09 2:07p erickson
 * PR51841: fix dtv
 *
 * 5   2/4/09 2:00p erickson
 * PR51841: use FileFifo, fix shutdown
 *
 *****************************************************************************/
/* Nexus example app: timeshifting with FileFifo */

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
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_RECORD
#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_file_fifo.h"
#include "nexus_record.h"
#endif
#include "nexus_security.h"
#include "nexus_dma.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

/* the following define the input file and its characteristics -- these will vary by input file */
#define DATA_FILE "videos/stream_3des.mpg"
#define INDEX_FILE "videos/stream_3des.nav"
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x31
#define AUDIO_PID 0x34

int main(void)
{
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_RECORD
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
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
    NEXUS_FilePlayHandle playbackfile;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_DisplaySettings displaySettings;
#endif

    NEXUS_FifoRecordHandle fifofile;
    NEXUS_FileRecordHandle recordfile;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_RecordPidChannelSettings pidSettings;
    NEXUS_RecordSettings recordSettings;
    NEXUS_PidChannelHandle pidChannel[2];
    NEXUS_Error rc;

    const char *datafilename = DATA_FILE;
    const char *indexfilename = INDEX_FILE;
    static const uint8_t keys[16] = { 0x4e, 0x61, 0xbc, 0x00, 0xa8, 0x7f, 0x39, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    NEXUS_KeySlotHandle decKeyHandle, encKeyHandle;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;
    NEXUS_DmaHandle dma;
    NEXUS_SecurityKeySlotSettings keySettings;

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    dma = NEXUS_Dma_Open(0, NULL);

    /******************************
    * Allocate AV keyslots
    **/
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    encKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!encKeyHandle) {
        printf("\nAllocate enc keyslot failed \n");
        return 1;
    }
    decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!decKeyHandle) {
        printf("\nAllocate enc keyslot failed \n");
        return 1;
    }

    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
    algConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
    algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    algConfig.operation = NEXUS_SecurityOperation_eEncrypt;
    NEXUS_Security_ConfigAlgorithm(encKeyHandle, &algConfig);

    algConfig.operation = NEXUS_SecurityOperation_eDecrypt;
    NEXUS_Security_ConfigAlgorithm(decKeyHandle, &algConfig);

    /* Load AV keys */
    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, sizeof(keys));
    if (NEXUS_Security_LoadClearKey(encKeyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }
    if (NEXUS_Security_LoadClearKey(decKeyHandle, &key) != 0) {
        printf("\nLoad decryption key failed \n");
        return 1;
    }

    /******************************
    * start record
    **/

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(record, &recordSettings);
    recordSettings.recpump = recpump;
    recordSettings.recpumpSettings.securityDma = dma;
    recordSettings.recpumpSettings.securityContext = encKeyHandle;
    NEXUS_Record_SetSettings(record, &recordSettings);

    fifofile = NEXUS_FifoRecord_Create(datafilename, indexfilename);
    recordfile = NEXUS_FifoRecord_GetFile(fifofile);

    NEXUS_Record_GetDefaultPidChannelSettings(&pidSettings);
    pidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    pidSettings.recpumpSettings.pidTypeSettings.video.index = true;
    pidSettings.recpumpSettings.pidTypeSettings.video.codec = VIDEO_CODEC;

    pidChannel[0] = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[0], &pidSettings);

    pidChannel[1] = NEXUS_PidChannel_Open(parserBand, AUDIO_PID, NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[1], NULL);

    NEXUS_Record_Start(record, recordfile);

    printf("record started. press enter to start timeshifted playback...\n");
    getchar();

    /******************************
    * start playback
    **/

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    playbackfile = NEXUS_FifoPlay_Open(datafilename, indexfilename, fifofile);
    if (!playbackfile) {
        fprintf(stderr, "can't open files:%s %s\n", datafilename, indexfilename);
        return -1;
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = TRANSPORT_TYPE;
    playbackSettings.playpumpSettings.securityDma = dma;
    playbackSettings.playpumpSettings.securityContext = decKeyHandle;
    playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay; /* when play hits the end, wait for record */
    playbackSettings.timeshifting = true;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
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
    audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, AUDIO_PID, &playbackPidSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
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

    /* Linking Playback to Record allows Playback to sleep until Record writes data. Avoids a busyloop near live. */
    NEXUS_Record_AddPlayback(record, playback);

    rc = NEXUS_Playback_Start(playback, playbackfile, NULL);
    BDBG_ASSERT(!rc);

#if 1
    for(;;) {
        NEXUS_PlaybackStatus playbackStatus;

        rc = NEXUS_Playback_GetStatus(playback, &playbackStatus);
        BDBG_ASSERT(!rc);

        printf("fifo %d%% position=%u.%03u sec\n",
            playbackStatus.fifoSize ? playbackStatus.fifoDepth * 100 / playbackStatus.fifoSize : 0,
            (unsigned)playbackStatus.position/1000, (unsigned)playbackStatus.position%1000);
        BKNI_Sleep(1000);
    }
#else
    printf("timeshifting playback started. press ENTER to stop.\n");
    getchar();
#endif

    /* Bring down system */
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_Record_RemovePlayback(record, playback);
    NEXUS_Record_Stop(record);
    NEXUS_Record_RemoveAllPidChannels(record);
    NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
    NEXUS_Playback_ClosePidChannel(playback, audioPidChannel);
    NEXUS_PidChannel_Close(pidChannel[0]);
    NEXUS_PidChannel_Close(pidChannel[1]);

    NEXUS_FilePlay_Close(playbackfile);
    NEXUS_FileRecord_Close(recordfile);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]));
#endif
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

#else
	printf("This application is not supported on this platform!\n");
#endif
    return 0;
}
