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
 * $brcm_Workfile: bsettop_audio_capture.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 11/1/10 11:37a $
 *
 * Module Description: Audio capture can capture PCM sample of the decoded audio in
 * raw PCM format. To enable Audio capture export enviornment variable audio_capture=y.
 * To enable capture in .wav format in addition audio capture=y define audio_capture_wav=y 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_audio_capture.c $
 * 
 * 3   11/1/10 11:37a jgarrett
 * SW7408-137: Adding compressed audio capture option
 * 
 * 2   10/22/09 10:50a jrubio
 * SW7340-54: Add PEP for 7340/7342
 * 
 * 1   8/25/09 11:24a katrep
 * SW7405-2934:add suppoort for audio capture to a file
 * 
 *
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* atoi */
#include "bsettop_impl.h"
#include "nexus_core_utils.h"


BDBG_MODULE(audio_capture);


#if BCHP_CHIP==7405 || BCHP_CHIP == 7335 || BCHP_CHIP==7325 || BCHP_CHIP== 7340 || BCHP_CHIP == 7342 || BCHP_CHIP == 7408

#define CAPTURE_FILE_NAME  "audio_capture.dat"
#define CAPTURE_FILE_NAME_WAV  "audio_capture.wav"

static FILE *g_pFile;

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
            BDBG_ERR(("Error getting capture buffer\n"));
            NEXUS_AudioCapture_Stop(capture);
            return;
        }

        if ( bufferSize > 0 )
        {
            /* Write samples to disk */
            if ( 1 != fwrite(pBuffer, bufferSize, 1, pFile) )
            {
                BDBG_ERR(("Error writing to disk\n"));
                NEXUS_AudioCapture_Stop(capture);
                return;
            }

            /*fprintf(stderr, "Data callback - Wrote %d bytes\n", (int)bufferSize);*/
            errCode = NEXUS_AudioCapture_WriteComplete(capture, bufferSize);
            if ( errCode )
            {
                BDBG_ERR(("Error committing capture buffer\n"));
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

int baudio_capture_open(baudio_decode_t audio)
{
    BDBG_ASSERT(audio);
    if(bsettop_get_config("audio_capture_wav") )
    {
        BDBG_WRN(("opening %s",CAPTURE_FILE_NAME_WAV));
        g_pFile = fopen(CAPTURE_FILE_NAME_WAV,"wb+");
    }
    else
    {
        BDBG_WRN(("opening %s",CAPTURE_FILE_NAME));
        g_pFile = fopen(CAPTURE_FILE_NAME, "wb+");
    }
    if ( NULL == g_pFile)
    {
        BDBG_ERR(("Unable to open file '%s' for writing\n", CAPTURE_FILE_NAME));
        return -1;
    }

    audio->nAudioCapture = NEXUS_AudioCapture_Open(0, NULL);
    if ( NULL == audio->nAudioCapture )
    {
        BDBG_ERR(("Unable to open capture channel\n"));
        return -1;
    }
    return 0;
}

int baudio_capture_close(baudio_decode_t audio)
{
    BDBG_WRN(("closing audio capture"));
    BDBG_ASSERT(audio);
    fflush(g_pFile);
    fclose(g_pFile);
    /*NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioCapture_GetConnector(audio->nAudioCapture));*/
    NEXUS_AudioOutput_Shutdown(NEXUS_AudioCapture_GetConnector(audio->nAudioCapture));
    NEXUS_AudioCapture_Close(audio->nAudioCapture);
    audio->nAudioCapture = NULL;
    return 0;
}


int baudio_capture_start(baudio_decode_t audio)
{
    NEXUS_AudioCaptureStartSettings captureSettings;

    BDBG_WRN(("Start audio capture"));

    /* Setup WAV file if desired (*always little-endian) */
    if ( bsettop_get_config("audio_capture_wav") )
    {
        fwrite("RIFF", 4, 1, g_pFile);    /* Byte 0..3 RIFF */
        fputc(0, g_pFile);                /* Byte 4..7 file size - 4*/
        fputc(0, g_pFile);
        fputc(0, g_pFile);
        fputc(0, g_pFile);
        fwrite("WAVE", 4, 1, g_pFile);    /* Byte 8..11 WAVE */
        fwrite("fmt ", 4, 1, g_pFile);    /* Byte 12..15 fmt */
        fputc(16, g_pFile);               /* Byte 16..19 format chunk length (16 bytes) */
        fputc(0, g_pFile);
        fputc(0, g_pFile);
        fputc(0, g_pFile);
        fputc(1, g_pFile);                /* Byte 20..21 compression code (1=PCM) */
        fputc(0, g_pFile);
        fputc(2, g_pFile);                /* Byte 22..23 Number of channels (2) */
        fputc(0, g_pFile);
        fputc(0, g_pFile);                /* Byte 24..27 Sample Rate (actual value later from decoder) */
        fputc(0, g_pFile);
        fputc(0, g_pFile);
        fputc(0, g_pFile);
        fputc(0, g_pFile);                /* Byte 28..31 Average Bytes/Second (actual value later from decder) */
        fputc(0, g_pFile);
        fputc(0, g_pFile);
        fputc(0, g_pFile);
        fputc(2, g_pFile);                /* Byte 32..33 Block Align (4 -- 2 bytes/channel * 2 channels) */
        fputc(0, g_pFile);
        fputc(16, g_pFile);               /* Byte 34..35 Bits Per Sample (16) */
        fputc(0, g_pFile);
        fwrite("data", 4, 1, g_pFile);    /* Byte 36..39 data */
        fputc(0, g_pFile);                /* Byte 40..43 data size - 4*/
        fputc(0, g_pFile);
        fputc(0, g_pFile);
        fputc(0, g_pFile);
    }
    /* Connect capture to decoder */
    if ( bsettop_get_config("audio_capture_compressed") )
    {
        NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(audio->nAudioCapture),
                                   NEXUS_AudioDecoder_GetConnector(audio->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(audio->nAudioCapture),
                                   NEXUS_AudioDecoder_GetConnector(audio->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }
    /* Start the capture -- no data will be received until the decoder starts */
    NEXUS_AudioCapture_GetDefaultStartSettings(&captureSettings);
    captureSettings.dataCallback.callback = capture_callback;
    captureSettings.dataCallback.context = audio->nAudioCapture;
    captureSettings.dataCallback.param = (int)g_pFile;
    NEXUS_AudioCapture_Start(audio->nAudioCapture, &captureSettings);
    return 0;
}

int baudio_capture_stop(baudio_decode_t audio)
{
    BDBG_WRN(("Stop audio capture"));
    NEXUS_StopCallbacks(audio->nAudioCapture);
        /* After StopCallbacks, we are guaranteed no more callbacks will arrive.  If we're writing raw data, we're done.
       If we're writing a .wav file, seek back to the beginning and finish up the header */
    if ( bsettop_get_config("audio_capture_wav") )
    {
        unsigned long fileLength;
        NEXUS_AudioDecoderStatus decoderStatus;

        fileLength = ftell(g_pFile);

        BDBG_WRN(("%lu bytes written to file\n", fileLength));
        if ( fileLength == 44 )
        {
            BDBG_WRN(("Warning, empty file detected.  Double-check data source\n"));
        }

        NEXUS_AudioDecoder_GetStatus(audio->nAudioDecoder, &decoderStatus);

        fseek(g_pFile, 4, SEEK_SET);  /* Need to write file size - 4 to this offset */
        fileLength -= 4;
        fputc(fileLength & 0xff, g_pFile);
        fputc((fileLength >> 8) & 0xff, g_pFile);
        fputc((fileLength >> 16) & 0xff, g_pFile);
        fputc((fileLength >> 24) & 0xff, g_pFile);
        fseek(g_pFile, 24, SEEK_SET); /* Need to write sample rate here */
        fputc(decoderStatus.sampleRate & 0xff, g_pFile);
        fputc((decoderStatus.sampleRate>>8) & 0xff, g_pFile);
        fputc((decoderStatus.sampleRate>>16) & 0xff, g_pFile);
        fputc((decoderStatus.sampleRate>>24) & 0xff, g_pFile);
        /* Need to write sampleRate * 4 here */
        decoderStatus.sampleRate *= 4;
        fputc(decoderStatus.sampleRate & 0xff, g_pFile);
        fputc((decoderStatus.sampleRate>>8) & 0xff, g_pFile);
        fputc((decoderStatus.sampleRate>>16) & 0xff, g_pFile);
        fputc((decoderStatus.sampleRate>>24) & 0xff, g_pFile);
        fseek(g_pFile, 40, SEEK_SET);  /* Need to write data size (file size - 44) to this offset */
        fileLength -= 40;
        fputc(fileLength & 0xff, g_pFile);
        fputc((fileLength >> 8) & 0xff, g_pFile);
        fputc((fileLength >> 16) & 0xff, g_pFile);
        fputc((fileLength >> 24) & 0xff, g_pFile);
    }
    NEXUS_AudioCapture_Stop(audio->nAudioCapture);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioCapture_GetConnector(audio->nAudioCapture));
    return 0;
}

#else
int baudio_capture_open(baudio_decode_t audio)
{
    BSTD_UNUSED(audio);
    return 0;
}
int baudio_capture_close(baudio_decode_t audio)
{
    BSTD_UNUSED(audio);
    return 0;
}
int baudio_capture_start(baudio_decode_t audio)
{
    BSTD_UNUSED(audio);
    return 0;
}
int baudio_capture_stop(baudio_decode_t audio)
{
    BSTD_UNUSED(audio);
    return 0;
}
#endif


