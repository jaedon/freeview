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
 * $brcm_Workfile: audio_capture.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 5/18/12 9:41a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/audio/audio_capture.c $
 * 
 * 10   5/18/12 9:41a jgarrett
 * SW7435-174: Using AudioCaptureStatus for output sample rate
 * 
 * 9   8/10/10 3:58p erickson
 * SW7420-934: rename NEXUS_AudioCapture_WriteComplete to
 *  NEXUS_AudioCapture_ReadComplete
 * 
 * 8   2/4/09 3:51p erickson
 * PR51841: refactor
 *
 * 7   10/1/08 11:06p jgarrett
 * PR 46611: Converting to use callbacks
 *
 * 6   8/25/08 2:09p jgarrett
 * PR 45602: Adding SPDIF output
 *
 * 5   8/15/08 3:16p jgarrett
 * PR 45698: Removing printf
 *
 * 4   8/15/08 3:15p jgarrett
 * PR 45698: Converting to polled logic
 *
 * 3   8/14/08 4:02p jgarrett
 * PR 45698: Adding .wav support and stop via getchar()
 *
 * 2   8/11/08 2:16p erickson
 * PR45602: fix event, add fflush and printf, don't WriteComplete
 *  bufferSize=0
 *
 * 1   7/25/08 5:08p jgarrett
 * PR 44645: Adding audio capture example
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
#include "nexus_pid_channel.h"
#include "nexus_stc_channel.h"
#include "nexus_parser_band.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_capture.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_core_utils.h"
#include <string.h>
#include <stdlib.h>

static void capture_callback(void *pParam, int param)
{
    NEXUS_AudioCaptureHandle capture = pParam;
    FILE *pFile = (FILE *)param;
    NEXUS_Error errCode;

    for ( ;; )
    {
        void *pBuffer;
        size_t bufferSize;

        /* Check available buffer space */
        errCode = NEXUS_AudioCapture_GetBuffer(capture, (void **)&pBuffer, &bufferSize);
        if ( errCode )
        {
            fprintf(stderr, "Error getting capture buffer\n");
            NEXUS_AudioCapture_Stop(capture);
            return;
        }

        if ( bufferSize > 0 )
        {
            /* Write samples to disk */
            if ( 1 != fwrite(pBuffer, bufferSize, 1, pFile) )
            {
                fprintf(stderr, "Error writing to disk\n");
                NEXUS_AudioCapture_Stop(capture);
                return;
            }

            /*fprintf(stderr, "Data callback - Wrote %d bytes\n", (int)bufferSize);*/
            errCode = NEXUS_AudioCapture_ReadComplete(capture, bufferSize);
            if ( errCode )
            {
                fprintf(stderr, "Error committing capture buffer\n");
                NEXUS_AudioCapture_Stop(capture);
                return;
            }
        }
        else
        {
            break;
        }
    }
}

int main(int argc, char **argv)
{
    FILE *pFile;
    const char *pFileName = "audio_capture.dat";
    NEXUS_PlatformConfiguration config;
    bool wavFile=false;
    int numSeconds=30;

    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_AudioDecoderHandle decoder;
    NEXUS_AudioCaptureHandle capture;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_AudioDecoderStartSettings decoderSettings;
    NEXUS_AudioCaptureStartSettings captureSettings;

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&config);

    if ( argc > 1 )
    {
        pFileName = argv[1];
        if ( strlen(pFileName) > 4 )
        {
            if ( strstr(pFileName, ".wav") == pFileName + (strlen(pFileName) - 4) )
            {
                /* Filename ends in .wav, write as .wav file */
                wavFile = true;
            }
        }
    }
    if ( argc > 2 )
    {
        numSeconds = atoi(argv[2]);
        if ( numSeconds <= 0 )
        {
            fprintf(stderr, "usage: %s <filename> <duration in seconds>\n", argv[0]);
            return -1;
        }
    }

    pFile = fopen(pFileName, "wb+");
    if ( NULL == pFile )
    {
        fprintf(stderr, "Unable to open file '%s' for writing\n", pFileName);
        return 0;
    }

    decoder = NEXUS_AudioDecoder_Open(0, NULL);
    if ( NULL == decoder )
    {
        fprintf(stderr, "Unable to open decoder 0\n");
        return 0;
    }
    capture = NEXUS_AudioCapture_Open(0, NULL);
    if ( NULL == capture )
    {
        fprintf(stderr, "Unable to open capture channel\n");
        return 0;
    }

    /* Setup WAV file if desired (*always little-endian) */
    if ( wavFile )
    {
        fwrite("RIFF", 4, 1, pFile);    /* Byte 0..3 RIFF */
        fputc(0, pFile);                /* Byte 4..7 file size - 4*/
        fputc(0, pFile);
        fputc(0, pFile);
        fputc(0, pFile);
        fwrite("WAVE", 4, 1, pFile);    /* Byte 8..11 WAVE */
        fwrite("fmt ", 4, 1, pFile);    /* Byte 12..15 fmt */
        fputc(16, pFile);               /* Byte 16..19 format chunk length (16 bytes) */
        fputc(0, pFile);
        fputc(0, pFile);
        fputc(0, pFile);
        fputc(1, pFile);                /* Byte 20..21 compression code (1=PCM) */
        fputc(0, pFile);
        fputc(2, pFile);                /* Byte 22..23 Number of channels (2) */
        fputc(0, pFile);
        fputc(0, pFile);                /* Byte 24..27 Sample Rate (actual value later from decoder) */
        fputc(0, pFile);
        fputc(0, pFile);
        fputc(0, pFile);
        fputc(0, pFile);                /* Byte 28..31 Average Bytes/Second (actual value later from decder) */
        fputc(0, pFile);
        fputc(0, pFile);
        fputc(0, pFile);
        fputc(2, pFile);                /* Byte 32..33 Block Align (4 -- 2 bytes/channel * 2 channels) */
        fputc(0, pFile);
        fputc(16, pFile);               /* Byte 34..35 Bits Per Sample (16) */
        fputc(0, pFile);
        fwrite("data", 4, 1, pFile);    /* Byte 36..39 data */
        fputc(0, pFile);                /* Byte 40..43 data size - 4*/
        fputc(0, pFile);
        fputc(0, pFile);
        fputc(0, pFile);
    }

    /* Connect DAC to decoder */
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(config.outputs.audioDacs[0]),
                               NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(config.outputs.spdif[0]),
                               NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo));
    /* Connect capture to decoder */
    NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(capture),
                               NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo));
    /* Start the capture -- no data will be received until the decoder starts */
    NEXUS_AudioCapture_GetDefaultStartSettings(&captureSettings);
    captureSettings.dataCallback.callback = capture_callback;
    captureSettings.dataCallback.context = capture;
    captureSettings.dataCallback.param = (int)pFile;
    NEXUS_AudioCapture_Start(capture, &captureSettings);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the StcChannel to do TSM with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Start the audio decoder */
    NEXUS_AudioDecoder_GetDefaultStartSettings(&decoderSettings);
    decoderSettings.codec = NEXUS_AudioCodec_eAc3;
    decoderSettings.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
    decoderSettings.stcChannel = stcChannel;
    NEXUS_AudioDecoder_Start(decoder, &decoderSettings);

    fprintf(stderr, "Started capture for %d seconds.\n", numSeconds);
    BKNI_Sleep(numSeconds * 1000);

    fprintf(stderr, "Stopping capture\n");
    NEXUS_StopCallbacks(capture);
    NEXUS_AudioCapture_Stop(capture);

    /* After StopCallbacks, we are guaranteed no more callbacks will arrive.  If we're writing raw data, we're done.
       If we're writing a .wav file, seek back to the beginning and finish up the header */
    if ( wavFile )
    {
        unsigned long fileLength;
        NEXUS_AudioCaptureStatus captureStatus;

        fileLength = ftell(pFile);

        printf("%lu bytes written to file\n", fileLength);
        if ( fileLength == 44 )
        {
            printf("Warning, empty file detected.  Double-check data source\n");
        }

        NEXUS_AudioCapture_GetStatus(capture, &captureStatus);

        fseek(pFile, 4, SEEK_SET);  /* Need to write file size - 4 to this offset */
        fileLength -= 4;
        fputc(fileLength & 0xff, pFile);
        fputc((fileLength >> 8) & 0xff, pFile);
        fputc((fileLength >> 16) & 0xff, pFile);
        fputc((fileLength >> 24) & 0xff, pFile);
        fseek(pFile, 24, SEEK_SET); /* Need to write sample rate here */
        fputc(captureStatus.sampleRate & 0xff, pFile);
        fputc((captureStatus.sampleRate>>8) & 0xff, pFile);
        fputc((captureStatus.sampleRate>>16) & 0xff, pFile);
        fputc((captureStatus.sampleRate>>24) & 0xff, pFile);
        /* Need to write sampleRate * 4 here */
        captureStatus.sampleRate *= 4;
        fputc(captureStatus.sampleRate & 0xff, pFile);
        fputc((captureStatus.sampleRate>>8) & 0xff, pFile);
        fputc((captureStatus.sampleRate>>16) & 0xff, pFile);
        fputc((captureStatus.sampleRate>>24) & 0xff, pFile);
        fseek(pFile, 40, SEEK_SET);  /* Need to write data size (file size - 44) to this offset */
        fileLength -= 40;
        fputc(fileLength & 0xff, pFile);
        fputc((fileLength >> 8) & 0xff, pFile);
        fputc((fileLength >> 16) & 0xff, pFile);
        fputc((fileLength >> 24) & 0xff, pFile);
    }

    printf("Stopping decoder\n");
    NEXUS_AudioDecoder_Stop(decoder);

    fflush(pFile);
    fclose(pFile);

    printf("Done\n");
    return 0;
}

