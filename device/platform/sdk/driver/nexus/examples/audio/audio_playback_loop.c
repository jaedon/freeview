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
 * $brcm_Workfile: audio_playback_loop.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/9/12 11:06a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/audio/audio_playback_loop.c $
 * 
 * 2   2/9/12 11:06a jgarrett
 * SW7346-672: Correcting buffer size and setting sample rate by default
 * 
 * 1   5/26/11 2:31p jgarrett
 * SW7420-1885: Adding loop around playback example
 * 
 *****************************************************************************/
#include <stdio.h>
#include "nexus_platform.h"
#include "nexus_platform_features.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "nexus_audio_playback.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include <string.h>
#include <stdlib.h>

#if NEXUS_NUM_AUDIO_PLAYBACKS
/* 1KHz sine wave at 48 KHz */
static int16_t samples[48] =
{
0,
4276,
8480,
12539,
16383,
19947,
23169,
25995,
28377,
30272,
31650,
32486,
32767,
32486,
31650,
30272,
28377,
25995,
23169,
19947,
16383,
12539,
8480,
4276,
0,
-4277,
-8481,
-12540,
-16384,
-19948,
-23170,
-25996,
-28378,
-30273,
-31651,
-32487,
-32767,
-32487,
-31651,
-30273,
-28378,
-25996,
-23170,
-19948,
-16384,
-12540,
-8481,
-4277
};

BDBG_MODULE(audio_playback);

int main(void)
{
    BERR_Code errCode;
    NEXUS_AudioPlaybackOpenSettings openSettings;
    NEXUS_PlatformConfiguration config;

    NEXUS_AudioPlaybackHandle handle;
    NEXUS_AudioPlaybackStartSettings settings;
    NEXUS_AudioPlaybackSettings pbSettings;

    unsigned i, offset=0;
    int16_t *pBuffer;
    size_t bufferSize;
    size_t fifoSize = 48*16*256;

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&config);

    NEXUS_AudioPlayback_GetDefaultOpenSettings(&openSettings);
    openSettings.fifoSize = fifoSize;
    handle = NEXUS_AudioPlayback_Open(0, &openSettings);
    if ( NULL == handle )
    {
        fprintf(stderr, "Unable to open playback channel\n");
        return 0;
    }

    /* Connect DAC to plaback */
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(config.outputs.audioDacs[0]),
                               NEXUS_AudioPlayback_GetConnector(handle));
    #if NEXUS_NUM_SPDIF_OUTPUTS
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(config.outputs.spdif[0]),
                               NEXUS_AudioPlayback_GetConnector(handle));
    #endif

    NEXUS_AudioPlayback_GetDefaultStartSettings(&settings);
    settings.sampleRate = 0;    /* Allow changes */
    settings.loopAround = true;
    settings.signedData = true;

    /* Fill available buffer space */
    errCode = NEXUS_AudioPlayback_GetBuffer(handle, (void **)&pBuffer, &bufferSize);
    if ( errCode )
    {
        printf("Error getting playback buffer\n");
        return -1;
    }

    if (bufferSize) 
    {
        /* Copy samples into buffer */
        bufferSize /= 4;
        for ( i=0; i<bufferSize; i++ )
        {
            pBuffer[2*i] = pBuffer[(2*i)+1] = samples[offset];
            offset++;
            if ( offset >= 48 )
            {
                offset = 0;
            }
        }
        bufferSize *= 4;

        errCode = NEXUS_AudioPlayback_WriteComplete(handle, bufferSize);
        if ( errCode )
        {
            printf("Error committing playback buffer\n");
            return -1;
        }
    }

    NEXUS_AudioPlayback_GetSettings(handle, &pbSettings);
    pbSettings.sampleRate = 48000;  /* Our default rate */
    errCode = NEXUS_AudioPlayback_SetSettings(handle, &pbSettings);
    BDBG_ASSERT(!errCode);    

    errCode = NEXUS_AudioPlayback_Start(handle, &settings);
    BDBG_ASSERT(!errCode);

    for ( ;; )
    {
        NEXUS_AudioPlayback_GetSettings(handle, &pbSettings);
        printf("Enter new sample rate or 0 to quit\n");
        scanf("%u", &pbSettings.sampleRate);
        if ( 0 == pbSettings.sampleRate )
            break;
        NEXUS_AudioPlayback_SetSettings(handle, &pbSettings);
    }

    printf("Stopping playback\n");
    NEXUS_AudioPlayback_Stop(handle);

    printf("Done\n");
    return 0;
}
#else
int main(int argc, char **argv)
{
    fprintf(stderr, "No audio playback support\n");
    argc=argc;
    argv=argv;
    return 0;
}
#endif

