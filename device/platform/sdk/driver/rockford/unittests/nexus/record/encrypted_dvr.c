/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: encrypted_dvr.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 11/16/12 6:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/encrypted_dvr.c $
 * 
 * Hydra_Software_Devel/3   11/16/12 6:11p mward
 * SW7435-56:  Stub security tests if !NEXUS_HAS_SECURITY.
 * 
 * Hydra_Software_Devel/2   6/27/12 5:56p jtna
 * SW7425-2688: relax assert when playing back with bogus or no key
 * 
 * Hydra_Software_Devel/1   4/3/12 2:45p jtna
 * SW7425-2688: added encrypted_dvr.c/.str
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_timebase.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_RECORD
#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_file_fifo.h"
#include "nexus_record.h"
#endif
#if NEXUS_HAS_SECURITY
#include "nexus_security.h"
#endif
#include "nexus_dma.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"

#include <stdio.h>
#include "string.h"
#include "bstd.h"
#include "bkni.h"

#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x31
#define AUDIO_PID 0x34

int main(int argc, char **argv) {
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_RECORD && NEXUS_HAS_SECURITY
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_RecordSettings recordSettings;
    NEXUS_RecordStatus recordStatus;
    NEXUS_FileRecordHandle recordfile;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_FilePlayHandle playbackfile;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_RecordPidChannelSettings recordPidSettings;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStatus videoDecoderStatus;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    const char *fname = "videos/stream_encrypted.mpg";
    const char *index = "videos/stream_encrypted.nav";
    const uint8_t keys[16] = { 0x4e, 0x61, 0xbc, 0x00, 0xa8, 0x7f, 0x39, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    const uint8_t bogusKeys[16] = { 0xff, 0x61, 0xbc, 0x00, 0xa8, 0x7f, 0x39, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    NEXUS_KeySlotHandle encKeyHandle, decKeyHandle, bogusKeyHandle;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_DmaHandle dma;   
    NEXUS_Error rc;
    unsigned count, numDecoded = 0, numDecodeErrors = 0;
    uint64_t numBytesDecoded = 0;
    int i = 0;
    bool aes = false; /* if true, 3DES. otherwise, AES */

    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i], "-aes")) {
            aes = true;
            BKNI_Printf("Using AES encryption\n");
        }
        if (!strcmp(argv[i], "-3des")) {
            aes = false;
            BKNI_Printf("Using 3DES encryption\n");
        }
    }

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    dma = NEXUS_Dma_Open(0, NULL);

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    encKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    BDBG_ASSERT(encKeyHandle);
    decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    BDBG_ASSERT(decKeyHandle);
    bogusKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    BDBG_ASSERT(bogusKeyHandle);

    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
    if (!aes) {
        algConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
    }
    else {
        algConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
    }
    algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    algConfig.operation = NEXUS_SecurityOperation_eEncrypt;
    NEXUS_Security_ConfigAlgorithm(encKeyHandle, &algConfig);
    algConfig.operation = NEXUS_SecurityOperation_eDecrypt;
    NEXUS_Security_ConfigAlgorithm(decKeyHandle, &algConfig);
    NEXUS_Security_ConfigAlgorithm(bogusKeyHandle, &algConfig);

    /* Load AV keys */
    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, sizeof(keys));
    rc = NEXUS_Security_LoadClearKey(encKeyHandle, &key);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Security_LoadClearKey(decKeyHandle, &key);
    BDBG_ASSERT(!rc);
    
    key.keySize = sizeof(bogusKeys);
    BKNI_Memcpy(key.keyData, bogusKeys, sizeof(bogusKeys));
    rc = NEXUS_Security_LoadClearKey(bogusKeyHandle, &key);
    BDBG_ASSERT(!rc);
    
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();

    NEXUS_Record_GetSettings(record, &recordSettings);
    recordSettings.recpump = recpump;
    recordSettings.recpumpSettings.securityDma = dma;
    recordSettings.recpumpSettings.securityContext = encKeyHandle;
    NEXUS_Record_SetSettings(record, &recordSettings);

    recordfile = NEXUS_FileRecord_OpenPosix(fname, index);
    if (!recordfile) {
        BKNI_Printf("Cannot open files: %s %s\n", fname, index);
        return -1;
    }

    /* configure the video pid for indexing */
    NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
    recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    recordPidSettings.recpumpSettings.pidTypeSettings.video.index = true;
    recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = VIDEO_CODEC;
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);
    NEXUS_Record_AddPidChannel(record, videoPidChannel, &recordPidSettings);

    /* the audio pid requires no special configuration */
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, AUDIO_PID, NULL);
    NEXUS_Record_AddPidChannel(record, audioPidChannel, NULL);

    NEXUS_Record_Start(record, recordfile);

    BKNI_Printf("Record started. Recording for 1 minute...\n");
    BKNI_Sleep(60*1000);
    NEXUS_Record_GetStatus(record, &recordStatus);
    BDBG_ASSERT(recordStatus.recpumpStatus.data.bytesRecorded > 0);

    NEXUS_Record_Stop(record);
    NEXUS_Record_RemoveAllPidChannels(record);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_FileRecord_Close(recordfile);
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);

    BKNI_Printf("Starting playback...\n");

    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    playbackfile = NEXUS_FilePlay_OpenPosix(fname, index);
    if (!playbackfile) {
        BKNI_Printf("Cannot open files: %s %s\n", fname, index);
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
    playbackSettings.stcChannel = stcChannel;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

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
    playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
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
    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
    BDBG_ASSERT(!rc);

    /* check normal playback */
    BKNI_Printf("Checking normal playback for 30 seconds...\n");
    rc = NEXUS_Playback_Start(playback, playbackfile, NULL);
    BDBG_ASSERT(!rc);

    count = 6;
    numDecoded = numBytesDecoded = numDecodeErrors = 0;
    while (count--) {
        BKNI_Sleep(5000);      

        NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoDecoderStatus);

        BKNI_Printf("pictures %u, bytes %u\n", videoDecoderStatus.numDecoded, (unsigned)videoDecoderStatus.numBytesDecoded);
        BKNI_Printf("errors %u, watchdogs %u\n", videoDecoderStatus.numDecodeErrors, videoDecoderStatus.numWatchdogs);

        BDBG_ASSERT(videoDecoderStatus.numDecoded > numDecoded);
        BDBG_ASSERT(videoDecoderStatus.numBytesDecoded > numBytesDecoded);
        BDBG_ASSERT(videoDecoderStatus.numDecodeErrors==0);
        BDBG_ASSERT(videoDecoderStatus.numWatchdogs==0);
        
        numDecoded = videoDecoderStatus.numDecoded;
        numBytesDecoded = videoDecoderStatus.numBytesDecoded;
    }

    /* check playback with wrong key */
    BKNI_Printf("Checking playback with wrong key for 30 seconds...\n");
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_Playback_Stop(playback);
    
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpumpSettings.securityDma = dma;
    playbackSettings.playpumpSettings.securityContext = bogusKeyHandle;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Playback_Start(playback, playbackfile, NULL);
    BDBG_ASSERT(!rc);
    
    count = 6;
    numBytesDecoded = 0;
    while (count--) {
        BKNI_Sleep(5000);      

        NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoDecoderStatus);
        BKNI_Printf("bytes %u\n", (unsigned)videoDecoderStatus.numBytesDecoded);
        BKNI_Printf("decoded %u, errors %u, watchdogs %u\n", videoDecoderStatus.numDecoded, videoDecoderStatus.numDecodeErrors, videoDecoderStatus.numWatchdogs);

#if 0 /* relax this requirement. it does not really matter whether this assert is true or not, as long as the system stays alive */
        /* only assert for numBytesDecoded. don't assert for other status.
           when playing back with a wrong or no key, there aren't many useful videoDecoder status members that can be used. 
           numDecodeErrors, numDecoded, numPicturesReceived are typically always 0, but they could occasionally be non-zero */
        BDBG_ASSERT(videoDecoderStatus.numBytesDecoded > numBytesDecoded);
#endif
        numBytesDecoded = videoDecoderStatus.numBytesDecoded;
    }

    /* check playback with no security */
    BKNI_Printf("Checking playback with no key for 30 seconds...\n");
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpumpSettings.securityDma = NULL;
    playbackSettings.playpumpSettings.securityContext = NULL;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    /* close the security-related stuff to be extra sure */
    NEXUS_Dma_Close(dma);
    NEXUS_Security_FreeKeySlot(encKeyHandle);
    NEXUS_Security_FreeKeySlot(decKeyHandle);
    NEXUS_Security_FreeKeySlot(bogusKeyHandle);

    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Playback_Start(playback, playbackfile, NULL);
    BDBG_ASSERT(!rc);

    count = 6;
    numBytesDecoded = 0;
    while (count--) {
        BKNI_Sleep(5000);      

        NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoDecoderStatus);
        BKNI_Printf("bytes %u\n", (unsigned)videoDecoderStatus.numBytesDecoded);
        BKNI_Printf("decoded %u, errors %u, watchdogs %u\n", videoDecoderStatus.numDecoded, videoDecoderStatus.numDecodeErrors, videoDecoderStatus.numWatchdogs);
#if 0
        BDBG_ASSERT(videoDecoderStatus.numBytesDecoded > numBytesDecoded);
        numBytesDecoded = videoDecoderStatus.numBytesDecoded;
#endif
    }
        
    NEXUS_Playback_Stop(playback);
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
    NEXUS_Playback_ClosePidChannel(playback, audioPidChannel);
    NEXUS_FilePlay_Close(playbackfile);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

#endif
    return 0;
}
