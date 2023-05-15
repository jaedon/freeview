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
 * $brcm_Workfile: audio_playback.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 6/14/12 3:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/audio/audio_playback.c $
 * 
 * 10   6/14/12 3:22p erickson
 * SWDEPRECATED-2704: add hdmi output
 *
 * 8   3/15/12 9:34a erickson
 * SW7429-86: add clean uninit
 *
 * 7   8/10/10 3:10p erickson
 * SW7420-934: rename NEXUS_AudioPlayback_ReadComplete to
 *  NEXUS_AudioPlayback_WriteComplete
 *
 * 6   5/7/10 2:58p jgarrett
 * SW7550-323: Programming output port sample rate for 32k/44.1k streams
 *  to avoid sample rate conversion
 *
 * 5   2/4/10 4:59p mward
 * SW7125-206: Support 40-byte WAVE_FORMAT_EXTENSIBLE and 18-byte header.
 *
 * 4   9/23/09 2:33p jgarrett
 * SW7405-3071: Adding additional command line options and support for
 *  mono or 8-bit wav files
 *
 * 3   8/15/08 3:54p jgarrett
 * PR 45698: Adding basic .wav support
 *
 * 2   8/11/08 2:19p erickson
 * PR45602: add file playback cmdline option, don't ReadComplete if
 *  bufferSize=0
 *
 * 1   3/11/08 4:39p jgarrett
 * PR 39983: Adding audio_playback
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
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#include "nexus_display.h"
#endif

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

#define SWAP32( a )  do{a=((a&0xFF)<<24|(a&0xFF00)<<8|(a&0xFF0000)>>8|(a&0xFF000000)>>24);}while(0)
#define SWAP16( a )  do{a=((a&0xFF)<<8|(a&0xFF00)>>8);}while(0)

typedef struct wave_header
{
    unsigned long riff;         /* 'RIFF' */
    unsigned long riffCSize;    /* size in bytes of file - 8 */
    unsigned long wave;         /* 'WAVE' */
    unsigned long fmt;          /* 'fmt ' */
    unsigned long headerLen;    /* header length (should be 16 for PCM) */
    unsigned short format;      /* 1 - pcm */
    unsigned short channels;    /* 1 - mono, 2 - sterio */
    unsigned long samplesSec;   /* samples / second */
    unsigned long bytesSec;     /* bytes / second */
    unsigned short chbits;      /* channels * bits/sample /8 */
    unsigned short bps;         /* bits per sample (8 or 16) */
                                /* Extensible format */
    unsigned short cbSize;      /* 2 Size of the extension (0 or 22)  */
    unsigned short wValidBitsPerSample; /* 2 Number of valid bits  */
    unsigned short dwChannelMask; /* 4 Speaker position mask  */
    unsigned char SubFormat[16];  /* SubFormat */

    unsigned long dataSig;      /* 'data' */
    unsigned long dataLen;      /* length of data */
}wave_header;

BDBG_MODULE(audio_playback);

static void data_callback(void *pParam1, int param2)
{
    /*
    printf("Data callback - channel 0x%08x\n", (unsigned)pParam1);
    */
    pParam1=pParam1;    /*unused*/
    BKNI_SetEvent((BKNI_EventHandle)param2);
}

int main(int argc, char **argv)
{
    NEXUS_PlatformSettings platformSettings;
    BERR_Code errCode;
    BKNI_EventHandle event;
    NEXUS_PlatformConfiguration config;
    NEXUS_AudioOutputSettings outputSettings;
    NEXUS_AudioPlaybackHandle handle;
    NEXUS_AudioPlaybackStartSettings settings;
#if NEXUS_HAS_HDMI_OUTPUT
    NEXUS_DisplayHandle display=NULL;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_HdmiOutputStatus hdmiStatus;
    unsigned timeout=100;
#endif
    size_t bytesToPlay = 48000*4*20;    /* 48 kHz, 4 bytes/sample, 20 seconds */
    size_t bytesPlayed=0;
    size_t offset=0;
    int16_t *pBuffer;
    size_t bufferSize;
    FILE *pFile = NULL;

    if (argc > 1)
    {
        pFile = fopen(argv[1], "rb");
        if ( NULL == pFile )
        {
            fprintf(stderr, "Unable to open '%s' for reading\n", argv[1]);
            return -1;
        }
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&config);

    BKNI_CreateEvent(&event);

    handle = NEXUS_AudioPlayback_Open(0, NULL);
    if ( NULL == handle )
    {
        fprintf(stderr, "Unable to open playback channel\n");
        errCode = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto done;
    }

#if NEXUS_HAS_HDMI_OUTPUT
    /* Look for HDMI */
    for ( timeout = 100; timeout > 0; timeout-- )
    {
        NEXUS_HdmiOutput_GetStatus(config.outputs.hdmi[0], &hdmiStatus);
        if ( hdmiStatus.connected )
        {
            NEXUS_Display_GetDefaultSettings(&displaySettings);
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            display = NEXUS_Display_Open(0, &displaySettings);
            if ( display )
            {
                NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(config.outputs.hdmi[0]));
            }
            break;
        }
        BKNI_Sleep(10);
    }        
#endif

    /* Connect DAC to plaback */
#if NEXUS_NUM_AUDIO_DACS
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(config.outputs.audioDacs[0]),
                               NEXUS_AudioPlayback_GetConnector(handle));
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(config.outputs.spdif[0]),
                               NEXUS_AudioPlayback_GetConnector(handle));
#endif
#if NEXUS_HAS_HDMI_OUTPUT
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(config.outputs.hdmi[0]),
        NEXUS_AudioPlayback_GetConnector(handle));
#endif

    NEXUS_AudioPlayback_GetDefaultStartSettings(&settings);
    if ( argc > 2 )
    {
        settings.sampleRate = atoi(argv[2]);
    }
    else
    {
        settings.sampleRate = 48000;
    }

    if ( argc > 3 )
    {
        settings.bitsPerSample = atoi(argv[3]);
    }
    else
    {
        settings.bitsPerSample = 16;
    }
    if ( argc > 4 )
    {
        settings.stereo = atoi(argv[4]);
    }
    else
    {
        settings.stereo = true;
    }
    settings.signedData = true;
    settings.dataCallback.callback = data_callback;
    settings.dataCallback.context = handle;
    settings.dataCallback.param = (int)event;

    /* If we have a wav file, get the sample rate from it */
    if ( pFile )
    {
        wave_header wh;

        /* read in the wave file header */
        fread(&wh.riff,4,1,pFile);
        fread(&wh.riffCSize,4,1,pFile);
        fread(&wh.wave,4,1,pFile);
        fread(&wh.fmt,4,1,pFile);
        fread(&wh.headerLen,4,1,pFile);
        fread(&wh.format,2,1,pFile);
        fread(&wh.channels,2,1,pFile);
        fread(&wh.samplesSec,4,1,pFile);
        fread(&wh.bytesSec,4,1,pFile);
        fread(&wh.chbits,2,1,pFile);
        fread(&wh.bps,2,1,pFile);

    #if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
        SWAP32(wh.riff);
        SWAP32(wh.riffCSize);
        SWAP32(wh.wave);
        SWAP32(wh.fmt);
        SWAP32(wh.headerLen);
        SWAP16(wh.format);
        SWAP16(wh.channels);
        SWAP32(wh.samplesSec);
        SWAP32(wh.bytesSec);
        SWAP16(wh.chbits);
        SWAP16(wh.bps);
    #endif

        /* Verify that this is a PCM WAV file. */
        if (wh.riff != 0x46464952) {
            fprintf(stderr, "RAW data file detected.\n");
            fseek(pFile, 0, SEEK_SET);
        }
        else
        {
            if (wh.wave != 0x45564157) {
                fprintf(stderr, "Not a WAV file.");
                errCode = BERR_TRACE(NEXUS_INVALID_PARAMETER);
                goto done;
            }

            if (wh.headerLen == 40 && wh.format == 0xfffe) { /* WAVE_FORMAT_EXTENSIBLE */
                fread(&wh.cbSize,2,1,pFile);                /* 2 Size of the extension (0 or 22)  */
                fread(&wh.wValidBitsPerSample,2,1,pFile);   /* 2 Number of valid bits  */
                fread(&wh.dwChannelMask,4,1,pFile);         /* 4 Speaker position mask  */
                fread(&wh.SubFormat,16,1,pFile);            /* SubFormat GUID */
                #if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
                SWAP16(wh.cbSize);
                SWAP16(wh.wValidBitsPerSample);
                SWAP32(wh.dwChannelMask);
                #endif
            }
            else if (wh.headerLen == 18 && wh.format == 1) { /* oddball WAVE format */
                fread(&wh.cbSize,2,1,pFile);                /* 2 Size of the extension (0 or 22) ?*/
            }
            else if (wh.headerLen != 16 && wh.format != 1) {
                fprintf(stderr, "Not PCM data in WAV file. headerLen = %lu, Format 0x%x\n",wh.headerLen,wh.format);
            }

            for (;;) {
                if (fread(&wh.dataSig,4,1,pFile) != 1)
                    break;
                if (fread(&wh.dataLen,4,1,pFile) != 1)
                    break;
        #if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
                SWAP32(wh.dataSig);
                SWAP32(wh.dataLen);
        #endif
                /* looking for 'data' chunk */
                if (wh.dataSig == 0x61746164) {
                    break;
                }
                else if (fseek(pFile, wh.dataLen, SEEK_CUR))
                {
                    break;
                }
            }
            if (wh.dataSig != 0x61746164) {
                fprintf(stderr, "No 'data' chunk found in WAV file.\n");
                errCode = BERR_TRACE(NEXUS_INVALID_PARAMETER);
                goto done;
            }
            if (wh.channels > 2) {
                fprintf(stderr, "Invalid number of channels (%u) specified.\n",wh.channels);
                errCode = BERR_TRACE(NEXUS_INVALID_PARAMETER);
                goto done;
            }
            settings.sampleRate = wh.samplesSec;
            settings.bitsPerSample = wh.bps;
            settings.stereo = (wh.channels>1);
            settings.signedData = (wh.bps != 8);    /* 8-bit .wav files are unsigned */
        }
    }
    printf("Stream sample rate %d, %d bits per sample, %s\n", settings.sampleRate, settings.bitsPerSample, settings.stereo?"stereo":"mono");

    switch ( settings.sampleRate )
    {
    case 32000:
    case 44100:
#if NEXUS_NUM_AUDIO_DACS
        NEXUS_AudioOutput_GetSettings(NEXUS_AudioDac_GetConnector(config.outputs.audioDacs[0]), &outputSettings);
        outputSettings.defaultSampleRate = settings.sampleRate;
        NEXUS_AudioOutput_SetSettings(NEXUS_AudioDac_GetConnector(config.outputs.audioDacs[0]), &outputSettings);
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS
        NEXUS_AudioOutput_GetSettings(NEXUS_SpdifOutput_GetConnector(config.outputs.spdif[0]), &outputSettings);
        outputSettings.defaultSampleRate = settings.sampleRate;
        NEXUS_AudioOutput_SetSettings(NEXUS_SpdifOutput_GetConnector(config.outputs.spdif[0]), &outputSettings);
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
        NEXUS_AudioOutput_GetSettings(NEXUS_HdmiOutput_GetAudioConnector(config.outputs.hdmi[0]), &outputSettings);
        outputSettings.defaultSampleRate = settings.sampleRate;
        NEXUS_AudioOutput_SetSettings(NEXUS_HdmiOutput_GetAudioConnector(config.outputs.hdmi[0]), &outputSettings);
#endif
        break;
    default:
        break;
    }

    errCode = NEXUS_AudioPlayback_Start(handle, &settings);
    BDBG_ASSERT(!errCode);

    if ( pFile )
    {
        printf("Starting playback of file '%s'\n", argv[1]);
    }
    else
    {
        printf("Starting playback for 20 seconds.\n");
    }

    do
    {
        unsigned i;

        /* Check available buffer space */
        errCode = NEXUS_AudioPlayback_GetBuffer(handle, (void **)&pBuffer, &bufferSize);
        if ( errCode )
        {
            printf("Error getting playback buffer\n");
            break;
        }

        if (bufferSize)
        {
            if ( pFile )
            {
                bufferSize = fread(pBuffer, 1, bufferSize, pFile);
                if ( 0 == bufferSize )
                {
                    break;
                }
            }
            else
            {
                /* Copy samples into buffer */
                bufferSize /= 4;
                for ( i=0; i<bufferSize; i++,bytesPlayed+=4 )
                {
                    pBuffer[2*i] = pBuffer[(2*i)+1] = samples[offset];
                    offset++;
                    if ( offset >= 48 )
                    {
                        offset = 0;
                    }
                }
                bufferSize *= 4;
            }

            errCode = NEXUS_AudioPlayback_WriteComplete(handle, bufferSize);
            if ( errCode )
            {
                printf("Error committing playback buffer\n");
                break;
            }
        }
        else
        {
            /* Wait for data callback */
            errCode = BKNI_WaitForEvent(event, 5000);
        }
    } while ( BERR_SUCCESS == errCode && bytesPlayed < bytesToPlay );

    printf("Waiting for buffer to empty\n");
    for ( ;; )
    {
        NEXUS_AudioPlaybackStatus status;
        NEXUS_AudioPlayback_GetStatus(handle, &status);
        if ( status.queuedBytes > 0 )
        {
            BKNI_Sleep(100);
        }
        else
        {
            break;
        }
    }

done:
    if (handle) {
        printf("Stopping playback\n");
        NEXUS_AudioPlayback_Stop(handle);
        NEXUS_AudioPlayback_Close(handle);
    }
#if NEXUS_HAS_HDMI_OUTPUT
    if ( display )
    {
        NEXUS_Display_Close(display);
    }
#endif
    NEXUS_Platform_Uninit();
    printf("Done\n");

    return errCode;
}
#else
int main(void)
{
    fprintf(stderr, "No audio playback support\n");
    return 0;
}
#endif

