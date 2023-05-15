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
 * $brcm_Workfile: live_client.c $
 * $brcm_Revision: 13 $
 * $brcm_Date: 8/10/12 10:25a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/live_client.c $
 * 
 * 13   8/10/12 10:25a erickson
 * SW7425-2734: rework examples to use NEXUS_ClientResources for decoder
 *  ids
 * 
 * 12   8/2/12 10:10a erickson
 * SW7429-227: add app ipc for refsw_server support
 * 
 * 10   11/7/11 5:10p bandrews
 * SW7231-409: use open instead of acquire
 * 
 * 9   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 * 
 * 8   9/13/11 1:24p erickson
 * SW7420-1148: change client modes to unprotected/protected/untrusted
 * 
 * 7   8/25/11 10:54a erickson
 * SW7420-1148: default server to using untrusted clients
 * 
 * 6   8/22/11 11:32a erickson
 * SW7125-946: NEXUS_SimpleAudioDecoder_Start may fail if audio codec not
 *  supported
 * 
 * 5   8/11/11 1:25p erickson
 * SW7420-1123: readd #include nexus_platform.h for
 *  NEXUS_Platform_GetStreamerInputBand
 * 
 * 4   8/10/11 3:03p erickson
 * SW7420-1123: move NEXUS_Platform_Uninit to nexus_platform_client.h so
 *  clients don't have to #include nexus_platform.h
 * 
 * 3   6/16/11 12:10p erickson
 * SW7425-458: fix live TSM for audio
 * 
 * 2   2/18/11 10:43a erickson
 * SW7420-1123: add debug
 *
 * 1   2/10/11 5:08p erickson
 * SW7420-1123: add live_client
 *
 *****************************************************************************/
#include "nexus_platform_client.h"
#include "nexus_platform.h" /* required for NEXUS_Platform_GetStreamerInputBand */
#include "nexus_simple_video_decoder.h"
#include "nexus_simple_audio_decoder.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#include "nexus_record.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(live_client);

#include "refsw_session_simple_client.h"

int main(int argc, char **argv)  
{
    NEXUS_ClientConfiguration clientConfig;
    NEXUS_ClientAuthenticationSettings authSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_SimpleVideoDecoderHandle videoDecoder;
    NEXUS_SimpleVideoDecoderStartSettings videoProgram;
    NEXUS_SimpleAudioDecoderHandle audioDecoder;
    NEXUS_SimpleAudioDecoderStartSettings audioProgram;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_FileRecordHandle file;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordHandle record = NULL;
    NEXUS_Error rc;
    unsigned timeout = 0;
    int curarg = 1;
    bool loop = false;
    const char *streamfile = "videos/stream.mpg";
    const char *indexfile = "videos/stream.nav";
    bool start_record = false;

    while (argc > curarg) {
        if (!strcmp(argv[curarg], "-timeout") && argc>curarg+1) {
            timeout = strtoul(argv[++curarg], NULL, 0);
        }
        else if (!strcmp(argv[curarg], "-loop")) {
            loop = true;
        }
        else if (!strcmp(argv[curarg], "-record")) {
            start_record = true;
        }
        curarg++;
    }

    rc = simple_client_init(argv[0], &authSettings);
    printf("using certificate:'%s'\n", (char *)authSettings.certificate.data);
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) {
        printf("cannot join: %d\n", rc);
        return -1;
    }
    NEXUS_Platform_GetClientConfiguration(&clientConfig);
    
    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_Open(NEXUS_ANY_ID);
    BDBG_ASSERT(parserBand);
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.mode = NEXUS_StcChannelMode_ePcr;
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel;
    stcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
    BDBG_ASSERT(stcChannel);
    
    if (start_record) {
        NEXUS_RecordSettings recordSettings;
        NEXUS_RecordPidChannelSettings pidSettings;

        recpump = NEXUS_Recpump_Open(0, NULL);
        record = NEXUS_Record_Create();
        NEXUS_Record_GetSettings(record, &recordSettings);
        recordSettings.recpump = recpump;
        NEXUS_Record_SetSettings(record, &recordSettings);
        file = NEXUS_FileRecord_OpenPosix(streamfile, indexfile);

        NEXUS_Record_GetDefaultPidChannelSettings(&pidSettings);
        pidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
        pidSettings.recpumpSettings.pidTypeSettings.video.index = true;
        pidSettings.recpumpSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
        NEXUS_Record_AddPidChannel(record, videoPidChannel, &pidSettings);
        NEXUS_Record_AddPidChannel(record, audioPidChannel, NULL);
    }

    /* Bring up audio decoders and outputs */
    if (clientConfig.resources.simpleVideoDecoder.total) {
        videoDecoder = NEXUS_SimpleVideoDecoder_Acquire(clientConfig.resources.simpleVideoDecoder.id[0]);
    }
    else {
        videoDecoder = NULL;
    }
    if (!videoDecoder) {
        BDBG_ERR(("video decoder not available"));
        return -1;
    }
    if (clientConfig.resources.simpleAudioDecoder.total) {
        audioDecoder = NEXUS_SimpleAudioDecoder_Acquire(clientConfig.resources.simpleAudioDecoder.id[0]);
    }
    else {
        audioDecoder = NULL;
    }
    if (!audioDecoder) {
        BDBG_ERR(("audio decoder not available"));
        return -1;
    }

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_SimpleVideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.settings.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.settings.pidChannel = videoPidChannel;
    videoProgram.settings.stcChannel = stcChannel;
    NEXUS_SimpleAudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.primary.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.primary.pidChannel = audioPidChannel;
    audioProgram.primary.stcChannel = stcChannel;

    do {
        if (record) {
            /* start record */
            NEXUS_Record_Start(record, file);
        }

        /* Start decoders */
        rc = NEXUS_SimpleVideoDecoder_Start(videoDecoder, &videoProgram);
        BDBG_ASSERT(!rc);
        if (audioDecoder) {
            NEXUS_SimpleAudioDecoder_Start(audioDecoder, &audioProgram);
            /* decode may fail if audio codec not supported */
        }

        if (!timeout) {
            printf("Press ENTER to stop.\n");
            getchar();
        }
        else {
            /* auto close */
            BKNI_Sleep(timeout);
        }

        /* Bring down system */
        NEXUS_SimpleVideoDecoder_Stop(videoDecoder);
        if (audioDecoder) {
            NEXUS_SimpleAudioDecoder_Stop(audioDecoder);
        }
        if (record) {
            NEXUS_Record_Stop(record);
        }
    } while (loop);

    NEXUS_SimpleVideoDecoder_Release(videoDecoder);
    if (audioDecoder) {
        NEXUS_SimpleAudioDecoder_Release(audioDecoder);
    }
    if (record) {
        NEXUS_FileRecord_Close(file);
        NEXUS_Record_Destroy(record);
        NEXUS_Recpump_Close(recpump);
    }
    NEXUS_ParserBand_Close(parserBand);
    NEXUS_Platform_Uninit();
    simple_client_shutdown();

    return 0;
}
