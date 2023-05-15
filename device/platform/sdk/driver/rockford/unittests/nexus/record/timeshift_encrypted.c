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
 * $brcm_Workfile: timeshift_encrypted.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/27/12 3:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/timeshift_encrypted.c $
 * 
 * Hydra_Software_Devel/2   3/27/12 3:58p jtna
 * SW7344-275: remove unneeded #if 0
 * 
 * Hydra_Software_Devel/1   3/27/12 3:56p jtna
 * SW7344-275: add timeshift_encrypted.c
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
#include "nexus_security.h"
#include "nexus_dma.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"

#define USE_SATELLITE 0 /* if 1, use satellite frontend. if 0, use streamer */
#define USE_ENCRYPTION 1

#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x31
#define AUDIO_PID 0x34
#if USE_SATELLITE
#define FREQ 1119000000
#define SATELLITE_MODE NEXUS_FrontendSatelliteMode_eDvb
#define TONE_MODE true
#define VOLTAGE NEXUS_FrontendDiseqcVoltage_e13v
#endif

void end_of_stream(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    BKNI_Printf("end_of_stream\n");
}

void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    BKNI_EventHandle event = (BKNI_EventHandle)param;
    NEXUS_FrontendSatelliteStatus status;

    BKNI_Printf("lock callback\n");
    NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
    if (status.demodLocked) {
        BKNI_SetEvent(event);
    }
}


int main(void) {
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_RECORD
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record;
    NEXUS_RecordSettings recordSettings;
    NEXUS_FifoRecordHandle fifofile;
    NEXUS_FileRecordHandle recordfile;
    NEXUS_FifoRecordSettings fifoRecordSettings;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_FilePlayHandle playbackfile;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_PidChannelHandle pidChannel[2]; /* use for record */
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel, pcrPidChannel; /* used for playback */
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_RecordPidChannelSettings recordPidSettings;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    const char *fname = "videos/stream_3des.mpg";
    const char *index = "videos/stream_3des.nav";
    static const uint8_t keys[16] = { 0x4e, 0x61, 0xbc, 0x00, 0xa8, 0x7f, 0x39, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    NEXUS_KeySlotHandle encKeyHandle, decKeyHandle;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_DmaHandle dma;   
    NEXUS_Error rc;
#if USE_SATELLITE
    NEXUS_FrontendHandle frontend = NULL;
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendDiseqcSettings diseqcSettings;
    BKNI_EventHandle lockEvent;
    unsigned i;
#endif

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
    rc = NEXUS_Security_LoadClearKey(encKeyHandle, &key);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Security_LoadClearKey(decKeyHandle, &key);
    BDBG_ASSERT(!rc);

#if USE_SATELLITE
    BSTD_UNUSED(inputBand);
    for (i=0; i<NEXUS_MAX_FRONTENDS; i++) {
        NEXUS_FrontendCapabilities capabilities;
        frontend = platformConfig.frontend[i];
        if (frontend) {
            NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
            if (capabilities.satellite) { break; }
        }
    }
    BDBG_ASSERT(frontend);

    rc = BKNI_CreateEvent(&lockEvent);
    BDBG_ASSERT(!rc);

    NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
    satSettings.frequency = FREQ;
    satSettings.mode = SATELLITE_MODE;
    satSettings.lockCallback.callback = lock_callback;
    satSettings.lockCallback.context = frontend;
    satSettings.lockCallback.param = (int)lockEvent;
    NEXUS_Frontend_GetUserParameters(frontend, &userParams);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    NEXUS_Frontend_GetDiseqcSettings(frontend, &diseqcSettings);
    diseqcSettings.toneEnabled = TONE_MODE;
    diseqcSettings.voltage = VOLTAGE;
    NEXUS_Frontend_SetDiseqcSettings(frontend, &diseqcSettings);

    rc = NEXUS_Frontend_TuneSatellite(frontend, &satSettings);
    BDBG_ASSERT(!rc);

    BKNI_Printf("Waiting for tuner lock...\n");
    rc = BKNI_WaitForEvent(lockEvent, 3000);
    if (rc) {
        BKNI_Printf("Tuner lock timeout\n");
        return -1;
    }
#else
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#endif

    recpump = NEXUS_Recpump_Open(0, NULL);
    record = NEXUS_Record_Create();

    NEXUS_Record_GetSettings(record, &recordSettings);
    recordSettings.recpump = recpump;
#if USE_ENCRYPTION
    recordSettings.recpumpSettings.securityDma = dma;
    recordSettings.recpumpSettings.securityContext = encKeyHandle;
#endif
    NEXUS_Record_SetSettings(record, &recordSettings);

    fifofile = NEXUS_FifoRecord_Create(fname, index);

    NEXUS_FifoRecord_GetSettings(fifofile, &fifoRecordSettings);
    fifoRecordSettings.interval = 30;
    rc = NEXUS_FifoRecord_SetSettings(fifofile, &fifoRecordSettings);
    BDBG_ASSERT(!rc);

    recordfile = NEXUS_FifoRecord_GetFile(fifofile);

    /* configure the video pid for indexing */
    NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
    recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    recordPidSettings.recpumpSettings.pidTypeSettings.video.index = true;
    recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = VIDEO_CODEC;
    pidChannel[0] = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[0], &recordPidSettings);

    /* the audio pid requires no special configuration */
    pidChannel[1] = NEXUS_PidChannel_Open(parserBand, AUDIO_PID, NULL);
    NEXUS_Record_AddPidChannel(record, pidChannel[1], NULL);

    NEXUS_Record_Start(record, recordfile);

    /* Nexus is now recording to disk */

    BKNI_Printf("starting record...\n");
    BKNI_Sleep(3000);

    BKNI_Printf("starting playback...\n");

    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);


    playbackfile = NEXUS_FifoPlay_Open(fname, index, fifofile);
    if (!playbackfile) {
        BKNI_Printf("can't open files: %s %s\n", fname, index);
        return -1;
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.autoConfigTimebase = false;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* PCR pid channel is from live parser band, even when timeshifting */
    pcrPidChannel = pidChannel[0];

    NEXUS_Timebase_GetSettings(stcSettings.timebase, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
    timebaseSettings.sourceSettings.pcr.pidChannel = pcrPidChannel;
    rc = NEXUS_Timebase_SetSettings(stcSettings.timebase, &timebaseSettings);
    BDBG_ASSERT(!rc);

    /* TODO: consider adding random trickmode operations. for now, it's just regular play */
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = TRANSPORT_TYPE;
    playbackSettings.endOfStreamCallback.callback = end_of_stream;
    playbackSettings.endOfStreamCallback.context = NULL;
    playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay; /* when play hits the end, wait for record */
    playbackSettings.timeshifting = true;
    playbackSettings.stcChannel = stcChannel;
    playbackSettings.stcTrick = true;
#if USE_ENCRYPTION
    playbackSettings.playpumpSettings.securityDma = dma;
    playbackSettings.playpumpSettings.securityContext = decKeyHandle;
#endif
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

    /* Linking Playback to Record allows Playback to sleep until Record writes data. Avoids a busyloop near live. */
    rc = NEXUS_Record_AddPlayback(record, playback);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Playback_Start(playback, playbackfile, NULL);
    BDBG_ASSERT(!rc);

    BKNI_Printf("playback started. running for 3 minutes...\n");
    BKNI_Sleep(180*1000);
    
    NEXUS_Playback_Stop(playback);
    NEXUS_Record_RemovePlayback(record, playback);
    NEXUS_Record_Stop(record);
    NEXUS_Record_RemoveAllPidChannels(record);
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_PidChannel_Close(pidChannel[0]);
    NEXUS_PidChannel_Close(pidChannel[1]);
    NEXUS_Playback_ClosePidChannel(playback, videoPidChannel);
    NEXUS_Playback_ClosePidChannel(playback, audioPidChannel);
    NEXUS_FilePlay_Close(playbackfile);
    NEXUS_FileRecord_Close(recordfile);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);
    NEXUS_Dma_Close(dma);
    NEXUS_Security_FreeKeySlot(encKeyHandle);
    NEXUS_Security_FreeKeySlot(decKeyHandle);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
#if USE_SATELLITE
    BKNI_DestroyEvent(lockEvent);
#endif
    NEXUS_Platform_Uninit();

#endif
    return 0;
}
