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
 * $brcm_Workfile: audio_client.c $
 * $brcm_Revision: 12 $
 * $brcm_Date: 11/27/12 1:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/audio_client.c $
 * 
 * 12   11/27/12 1:40p erickson
 * SW7435-421: undo previous change. IPC comes up before nexus join and
 *  comes down after nexus uninit.
 * 
 * 11   10/16/12 4:42p erickson
 * SW7435-421: fix memory leak
 * 
 * 10   6/6/12 11:39a erickson
 * SW7425-2981: convert to pcm playback-only client
 * 
 * 9   5/11/12 4:07p erickson
 * SW7231-807: make audio_client work with refsw_server
 * 
 * 8   11/29/11 11:41a erickson
 * SW7420-2129: use NULL for default settings now that default heap can be
 *  obtained via NEXUS_P_DefaultHeap
 * 
 * 7   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 * 
 * 6   8/10/11 3:03p erickson
 * SW7420-1123: move NEXUS_Platform_Uninit to nexus_platform_client.h so
 *  clients don't have to #include nexus_platform.h
 * 
 * 5   6/2/11 5:33p erickson
 * SW7420-1889: rework SimpleAudioPlayback to have separate
 *  Acquire/Release functions
 * 
 * 4   2/18/11 10:43a erickson
 * SW7420-1123: add debug
 *
 * 3   1/27/11 4:01p erickson
 * SW7405-5105: update
 *
 * 2   1/24/11 4:03p erickson
 * SW7405-5105: fix callback
 *
 * 1   1/24/11 3:59p erickson
 * SW7405-5105: add simple audio playback client and server example code
 *
 *****************************************************************************/
#include "nexus_platform_client.h"
#include "nexus_simple_audio_playback.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "refsw_session_simple_client.h"

BDBG_MODULE(audio_client);

static void complete(void *context, int param)
{
    BSTD_UNUSED(param);
    BDBG_WRN(("pcm callback"));
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(int argc, char **argv)  {
    NEXUS_ClientConfiguration platformConfig;
    NEXUS_ClientAuthenticationSettings authSettings;
    NEXUS_SimpleAudioPlaybackHandle audioPlayback;
    NEXUS_SimpleAudioPlaybackStartSettings startSettings;
    FILE *file;
    BKNI_EventHandle event;
    int rc;
    unsigned timeout = 0;
    int curarg = 1;
    bool loop = false;
    const char *filename = "audio.pcm";

    while (argc > curarg) {
        if (!strcmp(argv[curarg], "-timeout") && argc>curarg+1) {
            timeout = strtoul(argv[++curarg], NULL, 0);
        }
        else if (!strcmp(argv[curarg], "-loop")) {
            loop = true;
        }
        else {
            filename = argv[curarg];
        }
        curarg++;
    }

    rc = simple_client_init(argv[0], &authSettings);
    if (!rc) {
        /* use app ipc value w/ refsw_server */
    }
    printf("using certificate:'%s'\n", (char *)authSettings.certificate.data);
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) {
        printf("cannot join: %d\n", rc);
        return -1;
    }
    
    NEXUS_Platform_GetClientConfiguration(&platformConfig);

    audioPlayback = NEXUS_SimpleAudioPlayback_Acquire(NEXUS_ANY_ID);
    BDBG_ASSERT(audioPlayback);
    
    BKNI_CreateEvent(&event);
    file = fopen(filename, "r");
    if (!file) {
        BDBG_ERR(("unable to open %s", filename));
        return -1;
    }

    NEXUS_SimpleAudioPlayback_GetDefaultStartSettings(&startSettings);
    startSettings.dataCallback.callback = complete;
    startSettings.dataCallback.context = event;
    /* TODO: support WAV files for dynamic config */
    startSettings.sampleRate = 44100;
    startSettings.bitsPerSample = 16;
    startSettings.stereo = true;
    startSettings.loopAround = false;
    rc = NEXUS_SimpleAudioPlayback_Start(audioPlayback, &startSettings);
    if (rc) {rc = BERR_TRACE(rc);goto err_start;}

    while (1) {
        void *buffer;
        size_t size;
        int n;

        rc = NEXUS_SimpleAudioPlayback_GetBuffer(audioPlayback, &buffer, &size);
        BDBG_ASSERT(!rc);
        if (size == 0) {
            BKNI_WaitForEvent(event, 1000);
            continue;
        }
        n = fread(buffer, 1, size, file);
        if (n < 0) {
            break;
        }
        else if (n == 0) {
            if (loop) {
                fseek(file, 0, SEEK_SET);
                continue;
            }
            else {
                /* eof */
                break;
            }
        }

        rc = NEXUS_SimpleAudioPlayback_WriteComplete(audioPlayback, n);
        BDBG_ASSERT(!rc);
        printf("play %d bytes\n", n);
    }

    NEXUS_SimpleAudioPlayback_Stop(audioPlayback);
    
err_start:
    fclose(file);
    BKNI_DestroyEvent(event);    

    NEXUS_SimpleAudioPlayback_Release(audioPlayback);
    NEXUS_Platform_Uninit();
    simple_client_shutdown();

    return 0;
}
