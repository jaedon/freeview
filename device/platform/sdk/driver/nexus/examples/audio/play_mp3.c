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
 * $brcm_Workfile: play_mp3.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 4/28/09 12:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/dvr/play_mp3.c $
 * 
 * 5   4/28/09 12:39p vsilyaev
 * PR 54166: Set playbackSettings.enableStreamProcessing to 'true' for
 *  seek support
 * 
 * 4   12/10/08 2:57p vsilyaev
 * PR 48760: Removed duplicated information
 * 
 * 3   12/10/08 1:40a vsilyaev
 * PR 48760: Using ES player for accurate progress monitor of MP3 streams
 * 
 * 2   4/9/08 1:09p jgarrett
 * PR 41557: Shutting down audio connector prior to closing
 * 
 * 1   4/9/08 10:46a erickson
 * PR41557: added play_mp3 example
 *
 *****************************************************************************/
/* Nexus example app: play and decode MP3 file to audio dacs */

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_playpump.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#endif
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

static void eof(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(int argc, char **argv) {
#if NEXUS_HAS_PLAYBACK
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings pidSettings;
    NEXUS_AudioDecoderHandle pcmDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    BKNI_EventHandle event;
    NEXUS_PlaybackStartSettings playbackStartSettings;
    const char *fname = "audio/test.mp3";

    if (argc > 1) {
        fname = argv[1];
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    file = NEXUS_FilePlay_OpenPosix(fname, NULL);
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        goto error;
    }
    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eEs;
    playbackSettings.endOfStreamCallback.callback = eof;
    playbackSettings.endOfStreamCallback.context = event;
    playbackSettings.enableStreamProcessing = true; /* this should be set to true if application needs to seek in the MP3 file, it'd cause decoder to receive MPEG-2 PES data */
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder,
        NEXUS_AudioDecoderConnectorType_eStereo));

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = NEXUS_AudioCodec_eMp3;

    NEXUS_Playback_GetDefaultPidChannelSettings(&pidSettings);
    pidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    pidSettings.pidSettings.pidTypeSettings.audio.codec = audioProgram.codec;
    pidSettings.pidTypeSettings.audio.primary = pcmDecoder;
    audioProgram.pidChannel = NEXUS_Playback_OpenPidChannel(playback, 1, &pidSettings);

    NEXUS_Playback_GetDefaultStartSettings(&playbackStartSettings);
#if 0
    playbackStartSettings.mode = NEXUS_PlaybackMode_eFixedBitrate;
    playbackStartSettings.bitrate = 128000;
#else
    playbackStartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
#endif

    NEXUS_Playback_Start(playback, file, &playbackStartSettings);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    for(;;) {
        NEXUS_PlaybackStatus playbackStatus;
        NEXUS_Error rc;

        rc = BKNI_WaitForEvent(event, 1000);
        if(rc!=NEXUS_TIMEOUT) {
            break;
        }

        if(NEXUS_Playback_GetStatus(playback,&playbackStatus)==NEXUS_SUCCESS) {
            printf("audio position=%u.%03u (%u,%u) sec \n", (unsigned)playbackStatus.position/1000, (unsigned)playbackStatus.position%1000, (unsigned)playbackStatus.first/1000, (unsigned)playbackStatus.last/1000 );
        }
    }

    NEXUS_AudioDecoder_Stop(pcmDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));

    NEXUS_Playback_ClosePidChannel(playback, audioProgram.pidChannel);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_FilePlay_Close(file);
    BKNI_DestroyEvent(event);
    NEXUS_Platform_Uninit();

    return 0;

error:
#endif
    return 1;
}
