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
 * $brcm_Workfile: playpump_encrypted_client.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 11/27/12 1:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/playpump_encrypted_client.c $
 * 
 * 6   11/27/12 1:40p erickson
 * SW7435-421: undo previous change. IPC comes up before nexus join and
 *  comes down after nexus uninit.
 * 
 * 5   10/16/12 4:42p erickson
 * SW7435-421: fix memory leak
 * 
 * 4   8/10/12 10:25a erickson
 * SW7425-2734: rework examples to use NEXUS_ClientResources for decoder
 *  ids
 * 
 * 3   2/9/12 12:38p erickson
 * SW7420-2237: remove NEXUS_SimpleVideoDecoder_SetWindowSettings
 * 
 * 2   11/29/11 11:41a erickson
 * SW7420-2129: use NULL for default settings now that default heap can be
 *  obtained via NEXUS_P_DefaultHeap
 * 
 * 1   10/31/11 6:07p bandrews
 * SW7231-391: added encrypted dvr examples
 * 
 * SW7420-2078/2   10/27/11 5:17p bandrews
 * SW7231-427: remove config of secondary codec; no longer required with
 *  fix to SAD
 * 
 * SW7420-2078/1   10/26/11 8:13p bandrews
 * SW7231-391: added dvr encryption client examples
 *****************************************************************************/
/* Nexus example app: playpump and decode */

#include "nexus_platform.h"
#include "nexus_platform_client.h"
#include "nexus_simple_video_decoder.h"
#include "nexus_simple_audio_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_playpump.h"
#include "nexus_security.h"
#include "nexus_dma.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#include "refsw_session_simple_client.h"

#if BCHP_CHIP == 7550
#define USE_CPD 1
#endif

BDBG_MODULE(playpump_encrypted_client);

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(int argc, char **argv)  
{
    NEXUS_ClientConfiguration clientConfig;
    NEXUS_ClientAuthenticationSettings authSettings;
    NEXUS_Error rc;
    FILE *file;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    const char *fname = "videos/cnnticker_3des.mpg";
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_SimpleVideoDecoderHandle videoDecoder;
    NEXUS_SimpleVideoDecoderStartSettings videoProgram;
    NEXUS_PidChannelHandle audioPidChannel;
    NEXUS_SimpleAudioDecoderHandle audioDecoder;
    NEXUS_SimpleAudioDecoderStartSettings audioProgram;
    static const uint8_t keys[16] = { 0x4e, 0x61, 0xbc, 0x00, 0xa8, 0x7f, 0x39, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    NEXUS_KeySlotHandle decKeyHandle;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;
    NEXUS_DmaHandle dma;
    NEXUS_SecurityKeySlotSettings keySettings;

    BSTD_UNUSED(argc);
    rc = simple_client_init(argv[0], &authSettings);
    printf("using certificate:'%s'\n", (char *)authSettings.certificate.data);
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) {
        printf("cannot join: %d\n", rc);
        return -1;
    }
    NEXUS_Platform_GetClientConfiguration(&clientConfig);

    dma = NEXUS_Dma_Open(NEXUS_ANY_ID, NULL);
    if (!dma) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto done; }

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
#if USE_CPD
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eCp;
#else
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
#endif
    decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!decKeyHandle) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto done; }

    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
#if USE_CPD
    algConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCpd;
#else
    /* Only used with m2m engine, use of CPS/CPD determines whether it encrypts or decrypts */
    algConfig.operation = NEXUS_SecurityOperation_eDecrypt;
#endif
    algConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
    algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    rc = NEXUS_Security_ConfigAlgorithm(decKeyHandle, &algConfig);
    if (rc) { rc = BERR_TRACE(rc); goto done; }

    /* Load AV keys */
    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, sizeof(keys));
    rc = NEXUS_Security_LoadClearKey(decKeyHandle, &key);
    if (rc) { rc = BERR_TRACE(rc); goto done; }

    BKNI_CreateEvent(&event);

    playpump = NEXUS_Playpump_Open(NEXUS_ANY_ID, NULL);
    if (!playpump) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto done; }

    /* use stdio for file I/O to keep the example simple. */
    file = fopen(fname, "rb");
    if (!file) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto done; }

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    playpumpSettings.securityDma = dma;
    playpumpSettings.securityContext = decKeyHandle;
    rc = NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
    if (rc) { rc = BERR_TRACE(rc); goto done; }

    rc = NEXUS_Playpump_Start(playpump);
    if (rc) { rc = BERR_TRACE(rc); goto done; }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
    if (!stcChannel) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto done; }

    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0x21, NULL);
    if (!videoPidChannel) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto done; }
    
    audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0x22, NULL);
    if (!audioPidChannel) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto done; }
    
    /* Bring up decoders */
    if (clientConfig.resources.simpleVideoDecoder.total) {
        videoDecoder = NEXUS_SimpleVideoDecoder_Acquire(clientConfig.resources.simpleVideoDecoder.id[0]);
    }
    else {
        videoDecoder = NULL;
    }
    if (!videoDecoder) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto done; }
    if (clientConfig.resources.simpleAudioDecoder.total) {
        audioDecoder = NEXUS_SimpleAudioDecoder_Acquire(clientConfig.resources.simpleAudioDecoder.id[0]);
    }
    else {
        audioDecoder = NULL;
    }

    NEXUS_SimpleVideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.settings.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.settings.pidChannel = videoPidChannel;
    videoProgram.settings.stcChannel = stcChannel;
    videoProgram.maxWidth = 720;
    videoProgram.maxHeight = 480;

    rc = NEXUS_SimpleVideoDecoder_Start(videoDecoder, &videoProgram);
    if (rc) { rc = BERR_TRACE(rc); goto error; }

    if (audioDecoder) {
        NEXUS_SimpleAudioDecoder_GetDefaultStartSettings(&audioProgram);
        audioProgram.primary.codec = NEXUS_AudioCodec_eMpeg;
        audioProgram.primary.pidChannel = audioPidChannel;
        audioProgram.primary.stcChannel = stcChannel;
        rc = NEXUS_SimpleAudioDecoder_Start(audioDecoder, &audioProgram);
        if (rc) { rc = BERR_TRACE(rc); goto error; }
    }

    while (1) {
        void *buffer;
        size_t buffer_size;
        int n;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
            break;
        if (buffer_size == 0) {
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            continue;
        }

        /* The first call to get_buffer will return the entire playback buffer.
        If we use it, we're going to have to wait until the descriptor is complete,
        and then we're going to underflow. So use a max size. */
#define MAX_READ (188*1024)
        if (buffer_size > MAX_READ)
            buffer_size = MAX_READ;

        n = fread(buffer, 1, buffer_size, file);
        if (n < 0) { rc = BERR_TRACE(ferror(file)); goto error; }
        if (n == 0) 
        {
            break;
        }
        else 
        {
            rc = NEXUS_Playpump_WriteComplete(playpump, 0, n);
            if (rc) { rc = BERR_TRACE(rc); goto error; }
            printf("played %d bytes\n", n);
        }
    }

    /* wait for end of decode, this is not the best way to do this, but it's an example */
    BKNI_Sleep(5000);

error:

    if (audioDecoder) {
        NEXUS_SimpleAudioDecoder_Stop(audioDecoder);
    }
    NEXUS_SimpleVideoDecoder_Stop(videoDecoder);
    NEXUS_Playpump_Stop(playpump);

done:

    /* Bring down system */
    if (videoPidChannel) NEXUS_Playpump_ClosePidChannel(playpump, videoPidChannel);
    if (audioPidChannel) NEXUS_Playpump_ClosePidChannel(playpump, audioPidChannel);
    if (videoDecoder) NEXUS_SimpleVideoDecoder_Release(videoDecoder);
    if (audioDecoder) NEXUS_SimpleAudioDecoder_Release(audioDecoder);
    if (file) fclose(file);
    if (playpump) NEXUS_Playpump_Close(playpump);
    if (stcChannel) NEXUS_StcChannel_Close(stcChannel);
    if (event) BKNI_DestroyEvent(event);
    if (decKeyHandle) NEXUS_Security_FreeKeySlot(decKeyHandle);
    if (dma) NEXUS_Dma_Close(dma);
    
    NEXUS_Platform_Uninit();
    simple_client_shutdown();
    
    return rc;
}

