/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: audyssey.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 1/7/11 3:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/audio/audyssey/audyssey.c $
 * 
 * Hydra_Software_Devel/6   1/7/11 3:32p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 * PTS - STC difference
 * 
 * Hydra_Software_Devel/5   7/8/10 12:38p nvu
 * SW3548-2991: modification as requested by Mahendran
 * Maruthaveerapandian.
 * 
 * Hydra_Software_Devel/3   6/25/10 4:10p nvu
 * SW3548-2991: modifications to the strings used for run-time interactive
 * changes to ABX/ADV settings.
 * 
 * Hydra_Software_Devel/2   6/23/10 1:47p nvu
 * SW3548-2991: submit Audyssey certification application.
 * 
 * Hydra_Software_Devel/1   6/22/10 1:51p nvu
 * SW3548-2991: submit Audyssey certification application.
 * 
 * 48   6/1/10 3:56p erickson
 * SW3548-2962: add runtime bvn_usage(ID) command
 *
 * 47   5/10/10 12:59p vsilyaev
 * SW3548-2920: Added printout for End Of Stream
 *
 * 46   5/5/10 10:43a vsilyaev
 * SW7405-1260: Added settings for size of the audio decoder buffer
 *
 * 45   2/23/10 4:50p vsilyaev
 * SW3556-913: Added code that monitors state of the playback file and
 *  restarts playback (from last good position) in case of error
 *
 * 44   2/22/10 5:33p vsilyaev
 * SW3556-913: Added option to plug  Custom File I/O routines to inject
 *  errors
 *
 * 43   2/18/10 6:00p vsilyaev
 * SW3556-913: Added command to restart playback
 *
 * 42   2/12/10 5:56p jtna
 * SW3556-1051: added option to control playback timestamp reordering
 *
 * 41   12/30/09 3:19p vsilyaev
 * SW7408-17: Added options to select PCM and compressed audio outputs
 *
 * 40   12/30/09 2:13p erickson
 * SW7550-128: add closed caption feature (-cc on)
 *
 * 39   11/25/09 5:24p katrep
 * SW7405-2740: Add support for WMA pro drc
 *
 * 38   11/20/09 3:54p erickson
 * SW7405-3260: remove unused #include nexus_astm.h
 *
 * 37   10/20/09 4:32p erickson
 * SW7405-3260: allow trick modes on streams where video/audio pid == 0.
 *  this is normal for AVI/ASF stream ids.
 *
 * 36   10/19/09 11:24a gmohile
 * SW7405-3209 : Add support for streams where pcr pid is different from
 *  video pid
 *
 * 35   8/18/09 4:36p vsilyaev
 * PR 56809: Added option to control handling of video errors
 *
 * 34   7/21/09 5:57p vsilyaev
 * PR 56977: Nexus playback always control audio and video decoder.
 *  Provided way for the user to control the decoder slowmotion factor
 *  using the playback trick API.
 *
 * 33   7/15/09 7:27p vsilyaev
 * PR 55653: Added WAV format
 *
 * 32   6/22/09 5:26p jtna
 * PR55571: vdecoder trick modes require adecoder to be stopped
 *
 * 31   6/19/09 5:20p vsilyaev
 * PR 56169: Added option to set max decode rate
 *
 * 30   6/8/09 7:06a erickson
 * PR55617: add user-specific aspectRatio
 *
 * 29   5/22/09 6:28p vsilyaev
 * PR 55376 PR 52344: Use custom heap configuration when enabled
 *  AVC(H.264) Level 5.1 decoding
 *
 * 28   5/22/09 5:21p vsilyaev
 * PR 55376 PR 52344: Added option to enable processing of AVC(H.264)
 *  Level 5.1 video
 *
 * 27   5/15/09 1:23p vsilyaev
 * PR 55193: Updated way on how we report status
 *
 * 26   5/15/09 12:48p vsilyaev
 * PR 55193: Added function to return last parsed PTS by the media library
 *
 * 25   3/27/09 12:51p vsilyaev
 * PR 50311: Return readPosition in the Nexus playback status. In
 *  combination with the position it could be used to measure depth of
 *  decoder and playback buffers in units of time.
 *
 * 24   3/18/09 10:26a erickson
 * PR52350: add wxga/fha support with 50/60 hz option
 *
 * 23   3/6/09 3:38p vsilyaev
 * PR 52831: Added shutdown code
 *
 * 22   3/6/09 9:33a erickson
 * PR51743: added -ar and -graphics options, default DTV apps to panel
 *  output
 *
 * 21   2/27/09 5:05p vsilyaev
 * PR 52634: Added code to handle MPEG-2 TS streams with timesampts (e.g.
 *  192 byte packets)
 *
 * 20   2/20/09 2:06p vsilyaev
 * PR 51467: Added option to set size of the video decoder buffer
 *
 * 19   2/5/09 2:12p erickson
 * PR51151: update
 *
 * 18   1/26/09 1:15p vsilyaev
 * PR 51351: Added option to seek using minutes and seconds
 *
 * 17   1/26/09 11:26a vsilyaev
 * PR 51579: Added stream_processing and auto_bitrate options
 *
 * 16   1/23/09 5:12p erickson
 * PR48944: update
 *
 * 15   1/22/09 7:48p vsilyaev
 * PR 50848: Don't use globals for the command line options
 *
 * 14   1/20/09 4:28p erickson
 * PR48944: add -mad and -display_format options
 *
 * 13   1/8/09 10:48p erickson
 * PR50861: added avdStatusBlock
 *
 * 12   1/8/09 10:34p erickson
 * PR48944: add more options
 *
 * 11   1/8/09 9:36p erickson
 * PR50757: added NEXUS_VideoFrameRate support, both as a start setting
 *  and status
 *
 * 10   1/7/09 9:50p erickson
 * PR50763: fix segfault
 *
 * 9   1/6/09 1:11a erickson
 * PR50763: added sleep and ; delimiter
 *
 * 8   1/6/09 12:45a erickson
 * PR50763: added -bof, -eof options. added playback position to status.
 *  fix mkv, mp4.
 *
 * 7   1/5/09 12:49p erickson
 * PR50763: update for mkv testing
 *
 * 6   12/11/08 3:25p erickson
 * PR48944: fix playback index support
 *
 * 5   12/3/08 3:55p erickson
 * PR48944: update
 *
 * 4   11/26/08 8:52a erickson
 * PR48944: more status
 *
 * 3   11/19/08 1:30p erickson
 * PR48944: update
 *
 * 2   11/17/08 2:19p erickson
 * PR48944: update
 *
 * 1   11/17/08 12:34p erickson
 * PR48944: add utils
 *
 *****************************************************************************/

#if 0
  for 16bits per sample
  #undef  OUTPUT_24BIT
#else
  /*for 24bits per sample (aligned to 32bits)*/
  #define OUTPUT_24BIT
#endif

#if 1
  #define ENABLE_AUDYSSEY
#endif

#if 0
  #define CONNECT_ABX_TO_SPDIF_AND_I2SOUTPUT
#endif



#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_video_adj.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"

#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_audio_capture.h"
#include "nexus_spdif_output.h"

#include "nexus_audyssey_adv.h"
#include "nexus_audyssey_abx.h"

#include "nexus_audio_equalizer.h"

#include "nexus_component_output.h"
#include "nexus_surface.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "cmdline_args.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
#include "fileio_custom.h"

BDBG_MODULE(playback);


/* Globals: bad programming practice, but development of this app was very rushed... */
bool wavFile = true;  /*hardcoded for simplicity - see audio_capture.c for RAW implementation)*/
char *input_file=NULL, *config_file=NULL, *input_config=NULL; 
const char *pFileName = "audio_capture.wav";
FILE *pOutputFile, *file_params=NULL;


/*--------------------------------------------------------------------- Run time change in the user config params -------------------------------------------------------------------------------- */
/*------------------------------------------------------------Initialize the user configuration parameters through the config file----------------------------------------------------------------- */
/*------------------------------------------------------------Initialize the user configuration parameters through the config file----------------------------------------------------------------- */

static void parse_config_file(FILE *file_params, NEXUS_AudysseyAdvSettings *advSettings, NEXUS_AudysseyAbxSettings *abxSettings, NEXUS_AudioEqualizerSettings *eqSettings)
{
    char line[400];
    char *value;

    while(fgets( line, 400, file_params ))
    {
      
      value = strchr(line, '=');
      if (value==NULL)
      {
          printf("Not a config param : %s\n", line);
          continue;
      }

      *value = 0;
      value++;
      
      if(!strcmp(line,"ADV_ENABLE"))
          advSettings->enabled = atoi(value);
      else if(!strcmp(line,"ADV_GAINCOMPRESSION"))
          advSettings->gainCompression = atoi(value);
      else if(!strcmp(line,"ADV_CHANNELMASK"))
          advSettings->channelMask = atoi(value);
      else if(!strcmp(line,"ADV_REINITIALIZE"))
          advSettings->reInitialize = atoi(value);
      else if(!strcmp(line,"ADV_VOLUMESCALINGENABLED"))
          advSettings->volumeScalingEnabled = atoi(value);
      else if(!strcmp(line,"ADV_DYNAMICSURROUNDGAINENABLED"))
          advSettings->dynamicSurroundGainEnabled = atoi(value);
      else if(!strcmp(line,"ADV_HILOCOMPRESS"))
          advSettings->hiLoCompress = atoi(value);
      else if(!strcmp(line,"ADV_DYNAMICEQMODE"))
          advSettings->dynamicEqMode = atoi(value);
      else if(!strcmp(line,"ADV_VOLUME"))
          advSettings->volume = atoi(value);
      else if(!strcmp(line,"ADV_HEADROOMOFFSET"))
          advSettings->headroomOffset = atoi(value);

/*
      2 argument parameters not handled yet...
            else if (strstr(buf, "adv_chCalbGain(") == buf) {
                int index, x;
                if(sscanf(buf+ strlen("adv_chCalbGain("),"%d,%d", &index, &x)==2) {
                    printf("adv_chCalbGain requested => index=%d, value=%d\n", index, x);
                }
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.chCalbGain[index] = x;
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "adv_chCalcLevel(") == buf) {
                int index, x;
                if(sscanf(buf+ strlen("adv_chCalcLevel("),"%d,%d", &index, &x)==2) {
                    printf("adv_chCalcLevel requested => index=%d, value=%d\n", index, x);
                }
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.chCalcLevel[index] = x;
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
*/

      else if(!strcmp(line,"ABX_ENABLE"))
          abxSettings->enabled = atoi(value);
      else if(!strcmp(line,"ABX_HARMONICGAIN"))
          abxSettings->harmonicGain = atoi(value);
      else if(!strcmp(line,"ABX_FILTERSET"))
          abxSettings->filterSet = atoi(value);
      else if(!strcmp(line,"ABX_DRYGAIN"))
          abxSettings->dryGain = atoi(value);
      else if(!strcmp(line,"AEQ_NO_FILTERS"))
		  ;
          /*eqSettings->modeSettings.custom.numFilters = atoi(value);*/
	  /* k=0 */
      else if(!strcmp(line,"AEQ_F0_COEF_B0"))
          eqSettings->modeSettings.custom.coefficient0[0] = atoi(value);
      else if(!strcmp(line,"AEQ_F0_COEF_B1"))
          eqSettings->modeSettings.custom.coefficient1[0] = atoi(value);
      else if(!strcmp(line,"AEQ_F0_COEF_B2"))
          eqSettings->modeSettings.custom.coefficient2[0] = atoi(value);
      else if(!strcmp(line,"AEQ_F0_COEF_A1"))
          eqSettings->modeSettings.custom.coefficient3[0] = atoi(value);
      else if(!strcmp(line,"AEQ_F0_COEF_A2"))
          eqSettings->modeSettings.custom.coefficient4[0] = atoi(value);
	  /* k=1 */
      else if(!strcmp(line,"AEQ_F1_COEF_B0"))
          eqSettings->modeSettings.custom.coefficient0[1] = atoi(value);
      else if(!strcmp(line,"AEQ_F1_COEF_B1"))
          eqSettings->modeSettings.custom.coefficient1[1] = atoi(value);
      else if(!strcmp(line,"AEQ_F1_COEF_B2"))
          eqSettings->modeSettings.custom.coefficient2[1] = atoi(value);
      else if(!strcmp(line,"AEQ_F1_COEF_A1"))
          eqSettings->modeSettings.custom.coefficient3[1] = atoi(value);
      else if(!strcmp(line,"AEQ_F1_COEF_A2"))
          eqSettings->modeSettings.custom.coefficient4[1] = atoi(value);
	  /* k=2 */
      else if(!strcmp(line,"AEQ_F2_COEF_B0"))
          eqSettings->modeSettings.custom.coefficient0[2] = atoi(value);
      else if(!strcmp(line,"AEQ_F2_COEF_B1"))
          eqSettings->modeSettings.custom.coefficient1[2] = atoi(value);
      else if(!strcmp(line,"AEQ_F2_COEF_B2"))
          eqSettings->modeSettings.custom.coefficient2[2] = atoi(value);
      else if(!strcmp(line,"AEQ_F2_COEF_A1"))
          eqSettings->modeSettings.custom.coefficient3[2] = atoi(value);
      else if(!strcmp(line,"AEQ_F2_COEF_A2"))
          eqSettings->modeSettings.custom.coefficient4[2] = atoi(value);
	  /* k=3 */
      else if(!strcmp(line,"AEQ_F3_COEF_B0"))
          eqSettings->modeSettings.custom.coefficient0[3] = atoi(value);
      else if(!strcmp(line,"AEQ_F3_COEF_B1"))
          eqSettings->modeSettings.custom.coefficient1[3] = atoi(value);
      else if(!strcmp(line,"AEQ_F3_COEF_B2"))
          eqSettings->modeSettings.custom.coefficient2[3] = atoi(value);
      else if(!strcmp(line,"AEQ_F3_COEF_A1"))
          eqSettings->modeSettings.custom.coefficient3[3] = atoi(value);
      else if(!strcmp(line,"AEQ_F3_COEF_A2"))
          eqSettings->modeSettings.custom.coefficient4[3] = atoi(value);
	  /* k=4 */
      else if(!strcmp(line,"AEQ_F4_COEF_B0"))
          eqSettings->modeSettings.custom.coefficient0[4] = atoi(value);
      else if(!strcmp(line,"AEQ_F4_COEF_B1"))
          eqSettings->modeSettings.custom.coefficient1[4] = atoi(value);
      else if(!strcmp(line,"AEQ_F4_COEF_B2"))
          eqSettings->modeSettings.custom.coefficient2[4] = atoi(value);
      else if(!strcmp(line,"AEQ_F4_COEF_A1"))
          eqSettings->modeSettings.custom.coefficient3[4] = atoi(value);
      else if(!strcmp(line,"AEQ_F4_COEF_A2"))
          eqSettings->modeSettings.custom.coefficient4[4] = atoi(value);


      else
      {
          printf("Not a recognized config param : %s\n", line);
          continue;
      }

    }

    return;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------- Printing the current set values for the dolby volume user config parameters --------------------------------------------------------- */
static void print_settings(NEXUS_AudysseyAdvSettings advSettings, NEXUS_AudysseyAbxSettings abxSettings)
{
    printf("\n\n---------------- The current values of the configuration parameters are as follows: ----------------- \n");

    printf("ADV_ENABLE=%d\n", advSettings.enabled);
    printf("ADV_GAINCOMPRESSION=%d\n", advSettings.gainCompression);
    printf("ADV_CHANNELMASK=%d\n", advSettings.channelMask);
    printf("ADV_REINITIALIZE=%d\n", advSettings.reInitialize);
    printf("ADV_VOLUMESCALINGENABLED=%d\n", advSettings.volumeScalingEnabled);
    printf("ADV_DYNAMICSURROUNDGAINENABLED=%d\n", advSettings.dynamicSurroundGainEnabled);
    printf("ADV_HILOCOMPRESS=%d\n", advSettings.hiLoCompress);
    printf("ADV_DYNAMICEQMODE=%d\n", advSettings.dynamicEqMode);
    printf("ADV_VOLUME=%d\n", advSettings.volume);
    printf("ADV_HEADROOMOFFSET=%d\n", advSettings.headroomOffset);
    printf ("\n");

    printf("ABX_ENABLE=%d\n", abxSettings.enabled);
    printf("ABX_HARMONICGAIN=%d\n", abxSettings.harmonicGain);
    printf("ABX_FILTERSET=%d\n", abxSettings.filterSet);
    printf("ABX_DRYGAIN=%d\n", abxSettings.dryGain);
    printf ("\n\n");

    return;
}


/*--------------------------------------------------------------------------Command line parsing------------------------------------------------------------------------------------------------- */

/*-----------------------------------------------------------------------PCM Capture callback------------------------------------------------------------------------------------------------------ */

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
            errCode = NEXUS_AudioCapture_WriteComplete(capture, bufferSize);
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
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

static void start_video(const struct util_opts_t *opts, NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *videoProgram)
{
    NEXUS_Error rc;
    if (opts->videoPid) {
        rc = NEXUS_VideoDecoder_Start(videoDecoder, videoProgram);
        BDBG_ASSERT(!rc);
    }
    return;
}
static void stop_video(const struct util_opts_t *opts, NEXUS_VideoDecoderHandle videoDecoder)
{
    if (opts->videoPid) {
        NEXUS_VideoDecoder_Stop(videoDecoder);
    }
    return;
}

static void start_audio(const struct util_opts_t *opts, NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderHandle compressedDecoder, const NEXUS_AudioDecoderStartSettings *audioProgram,NEXUS_AudioCaptureHandle capture, FILE *pFile)
{
    NEXUS_Error rc;
    NEXUS_AudioCaptureStartSettings captureSettings;
    NEXUS_AudioCaptureSettings audiocaptureSettings;
/*    NEXUS_AudioDecoderStatus audioStatus; */

    if (opts->audioPid) {
#if B_HAS_ASF
        /* if DRC for WMA pro is available apply now */
        if(audioProgram->codec == NEXUS_AudioCodec_eWmaPro && opts->dynamicRangeControlValid ){

            NEXUS_AudioDecoderCodecSettings codecSettings;

            BDBG_WRN(("wma-pro drc enabled,ref peak %d,ref target %d,ave peak %d, ave target %d",
                      opts->dynamicRangeControl.peakReference,opts->dynamicRangeControl.peakTarget,
                      opts->dynamicRangeControl.averageReference,opts->dynamicRangeControl.averageTarget));
            NEXUS_AudioDecoder_GetCodecSettings(audioDecoder, audioProgram->codec, &codecSettings);
            codecSettings.codec = audioProgram->codec;
            codecSettings.codecSettings.wmaPro.dynamicRangeControlValid = true;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.peakReference = opts->dynamicRangeControl.peakReference;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.peakTarget = opts->dynamicRangeControl.peakTarget;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.averageReference = opts->dynamicRangeControl.averageReference;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.averageTarget = opts->dynamicRangeControl.averageTarget;
            NEXUS_AudioDecoder_SetCodecSettings(audioDecoder,&codecSettings);
            }
#endif

  /* Start the capture -- no data will be received until the decoder starts */
    NEXUS_AudioCapture_GetDefaultStartSettings(&captureSettings);
    captureSettings.dataCallback.callback = capture_callback;
    captureSettings.dataCallback.context = capture;
    captureSettings.dataCallback.param = (int)pFile;

    NEXUS_AudioCapture_GetSettings(capture,&audiocaptureSettings);
#ifndef OUTPUT_24BIT
    audiocaptureSettings.format=NEXUS_AudioCaptureFormat_e16BitStereo;
#else
    audiocaptureSettings.format=NEXUS_AudioCaptureFormat_e24BitStereo;
#endif
/*
I think the problem is that the example app captured in 16-bit mode and you're capturing in 24-bit mode.
You need to adjust the WAV header to reflect that.  I think you need to change lines 643 and 645 to say
you have 4 bytes per channel and 32 bits per sample respectively.  You also need to change line 415 to
multiply by 8 bytes per sample pair instead of 4.  
*/
    NEXUS_AudioCapture_SetSettings(capture,&audiocaptureSettings);

    NEXUS_AudioCapture_Start(capture, &captureSettings);

        if(opts->decodedAudio) {
            rc = NEXUS_AudioDecoder_Start(audioDecoder, audioProgram);
            BDBG_ASSERT(!rc);
        }
        if(compressedDecoder) {
            rc = NEXUS_AudioDecoder_Start(compressedDecoder, audioProgram);
            BDBG_ASSERT(!rc);
        }
    }
    return;
}

static void stop_audio(const struct util_opts_t *opts, NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderHandle compressedDecoder, NEXUS_AudioCaptureHandle capture, FILE *pOutputFile)
{
    fprintf(stderr, "Stopping capture\n");
    NEXUS_StopCallbacks(capture);
    NEXUS_AudioCapture_Stop(capture);

    if ( wavFile )
    {
        unsigned long fileLength;
        NEXUS_AudioDecoderStatus decoderStatus;

        fileLength = ftell(pOutputFile);

        printf("%lu bytes written to file\n", fileLength);
        if ( fileLength == 44 )
        {
            printf("Warning, empty file detected.  Double-check data source\n");
        }

        NEXUS_AudioDecoder_GetStatus(audioDecoder, &decoderStatus);

        printf("Sample Rate = %d \n", decoderStatus.sampleRate);
        printf("File Length = %ld \n", fileLength);
#ifndef OUTPUT_24BIT
        printf("Bits/Sample = 16 \n");
#else
        printf("Bits/Sample = 24 \n");
#endif
        printf(" \n");


        fseek(pOutputFile, 4, SEEK_SET);  /* Need to write file size - 4 to this offset */
        fileLength -= 4;
        fputc(fileLength & 0xff, pOutputFile);
        fputc((fileLength >> 8) & 0xff, pOutputFile);
        fputc((fileLength >> 16) & 0xff, pOutputFile);
        fputc((fileLength >> 24) & 0xff, pOutputFile);
        fseek(pOutputFile, 24, SEEK_SET); /* Need to write sample rate here */
        fputc(decoderStatus.sampleRate & 0xff, pOutputFile);
        fputc((decoderStatus.sampleRate>>8) & 0xff, pOutputFile);
        fputc((decoderStatus.sampleRate>>16) & 0xff, pOutputFile);
        fputc((decoderStatus.sampleRate>>24) & 0xff, pOutputFile);
        /* Need to write sampleRate * 4 here */
#ifndef OUTPUT_24BIT
        decoderStatus.sampleRate *= 4;  /*for 16bits per sample*/
#else
        decoderStatus.sampleRate *= 8;  /*for 24bits per sample (aligned to 32bits)*/
#endif
        fputc(decoderStatus.sampleRate & 0xff, pOutputFile);
        fputc((decoderStatus.sampleRate>>8) & 0xff, pOutputFile);
        fputc((decoderStatus.sampleRate>>16) & 0xff, pOutputFile);
        fputc((decoderStatus.sampleRate>>24) & 0xff, pOutputFile);
        fseek(pOutputFile, 40, SEEK_SET);  /* Need to write data size (file size - 44) to this offset */
        fileLength -= 40;
        fputc(fileLength & 0xff, pOutputFile);
        fputc((fileLength >> 8) & 0xff, pOutputFile);
        fputc((fileLength >> 16) & 0xff, pOutputFile);
        fputc((fileLength >> 24) & 0xff, pOutputFile);
    }

    if (opts->audioPid) {
        if(opts->decodedAudio) {
            NEXUS_AudioDecoder_Stop(audioDecoder);
            NEXUS_AudioCapture_Stop(capture);
        }
        if(compressedDecoder) {
            NEXUS_AudioDecoder_Stop(compressedDecoder);
        }
    }
    return;
}

#define  B_HAS_PLAYBACK_MONITOR 0
#if B_HAS_PLAYBACK_MONITOR
#include <pthread.h>
#include "bkni_multi.h"

typedef struct PlaybackMonitorState {
    bool terminate;
    pthread_t thread;
    BKNI_MutexHandle lock;
    const struct util_opts_t *opts;
    NEXUS_PlaybackHandle playback;
    NEXUS_VideoDecoderHandle videoDecoder;
    const NEXUS_VideoDecoderStartSettings *videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderHandle compressedDecoder;
    const NEXUS_AudioDecoderStartSettings *audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_FilePlayHandle customFile;
    NEXUS_FilePlayHandle stickyFile;
    const NEXUS_PlaybackStartSettings *playbackStartSettings;
} PlaybackMonitorState;

static void *
monitor_thread(void *state_)
{
    const PlaybackMonitorState *state=state_;
    while(!state->terminate) {
        NEXUS_PlaybackStatus status;
        NEXUS_PlaybackSettings playbackSettings;
        NEXUS_VideoDecoderSettings videoDecoderSettings;
        BERR_Code rc;
        bool failed;

        rc = NEXUS_Playback_GetStatus(state->playback, &status);
        BDBG_ASSERT(!rc);
        BKNI_Sleep(1000);
        printf("\n\n the playback status is:%d \n\n",status.state);
        FileIoSticky_GetFailBit(state->stickyFile, &failed);
        if(!failed) {
            continue;
        }
        BDBG_WRN(("restarting from %u", status.position));
        BKNI_AcquireMutex(state->lock);
        NEXUS_Playback_Stop(state->playback);
        FileIoSticky_ClearFailBit(state->stickyFile);
        if(state->customFile) {
            FileIoCustomProbabilities probabilities;
            FileIoCustom_GetProbabilities(state->customFile, NULL, &probabilities);
            probabilities.error = 0;
            probabilities.nodata = 0;
            probabilities.partial_read = 0;
            FileIoCustom_SetProbabilities(state->customFile, &probabilities, &probabilities);
        }

        if (state->opts->videoPid) {
            /* don't show black frame */
            NEXUS_VideoDecoder_GetSettings(state->videoDecoder, &videoDecoderSettings);
            videoDecoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilTsmLock;
            rc=NEXUS_VideoDecoder_SetSettings(state->videoDecoder, &videoDecoderSettings);
            BDBG_ASSERT(!rc);
        }
        /* stop decoder */
        stop_video(state->opts, state->videoDecoder);
        stop_audio(state->opts, state->audioDecoder, state->compressedDecoder, capture, pOutputFile);

        NEXUS_Playback_GetSettings(state->playback, &playbackSettings);
        playbackSettings.startPaused = true;
        rc = NEXUS_Playback_SetSettings(state->playback, &playbackSettings);
        BDBG_ASSERT(!rc);

        /* Start decoders */
        start_video(state->opts, state->videoDecoder, state->videoProgram);
        start_audio(state->opts, state->audioDecoder, state->compressedDecoder, state->audioProgram, capture, pOutputFile);


        /* start playback  */
        rc = NEXUS_Playback_Start(state->playback, state->file, state->playbackStartSettings);
        BDBG_ASSERT(!rc);

        /* seek into desired location */
        rc = NEXUS_Playback_Seek(state->playback, status.position);
        BDBG_ASSERT(!rc);

        /* start playing */
        rc = NEXUS_Playback_Play(state->playback);
        BDBG_ASSERT(!rc);
        BKNI_ReleaseMutex(state->lock);
    }
    return NULL;
}

static void
monitor_thread_start(PlaybackMonitorState *state)
{
    int rc;
    BKNI_CreateMutex(&state->lock);
    state->terminate = false;
    rc = pthread_create(&state->thread, NULL, monitor_thread, state);
    BDBG_ASSERT(rc==0);
    return;
}

static void
monitor_thread_stop(PlaybackMonitorState *state)
{
    state->terminate = true;
    pthread_join(state->thread, NULL);
}
#endif /* B_HAS_PLAYBACK_MONITOR */

static void play_endOfStreamCallback(void *context, int param)
{
    BSTD_UNUSED(param);
    BSTD_UNUSED(context);

    printf("endOfStream\n");
    return;
}


int main(int argc, const char *argv[]) {
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel = NULL, audioPidChannel = NULL, pcrPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder, compressedDecoder=NULL;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file,customFile=NULL, stickyFile=NULL;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_PlaybackStartSettings playbackStartSettings;
    NEXUS_PlaybackStatus pstatus;
/*    NEXUS_PlaybackStatus pstatus;*/
    NEXUS_VideoDecoderOpenSettings openSettings;
    NEXUS_Error rc;
/*    NEXUS_AudioCaptureStartSettings captureSettings;*/

    NEXUS_I2sOutputHandle i2so;

    NEXUS_AudysseyAdvHandle  adv;
    NEXUS_AudysseyAbxHandle  abx;
    NEXUS_AudysseyAdvSettings advSettings;
    NEXUS_AudysseyAbxSettings abxSettings;

	NEXUS_AudioEqualizerHandle eq1=NULL, eq2=NULL;
	NEXUS_AudioEqualizerSettings eqSettings, eqSettings_tmp;

    NEXUS_AudioCaptureHandle capture;

    struct util_opts_t opts;
    bool exit;
    NEXUS_SurfaceHandle framebuffer = NULL;
    NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;

/*-------------------------------------------------------------Initialization---------------------------------------------------------------------------------------------------------------------- */   
    if (cmdline_parse(argc, argv, &opts)) {
        return 0;
    }

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    if(opts.avc51) {
        platformSettings.videoDecoderModuleSettings.heapSize.general = (84*1024*1024);
        platformSettings.videoDecoderModuleSettings.numDecodes = 1;
    }
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

     pOutputFile = fopen(pFileName, "wb+");
        if ( NULL == pOutputFile )
        {
            fprintf(stderr, "Unable to open file '%s' for writing captured data\n", pFileName);
            return 0;
        }

    if(opts.loopback==1)
    {
        opts.endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    }
    else
    {
        opts.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay;
    }

    /* Setup WAV file if desired (*always little-endian) */
    if ( wavFile )
    {
        fwrite("RIFF", 4, 1, pOutputFile);    /* Byte 0..3 RIFF */
        fputc(0, pOutputFile);                /* Byte 4..7 file size - 4*/
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);
        fwrite("WAVE", 4, 1, pOutputFile);    /* Byte 8..11 WAVE */
        fwrite("fmt ", 4, 1, pOutputFile);    /* Byte 12..15 fmt */
        fputc(16, pOutputFile);               /* Byte 16..19 format chunk length (16 bytes) */
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);
        fputc(1, pOutputFile);                /* Byte 20..21 compression code (1=PCM) */
        fputc(0, pOutputFile);
        fputc(2, pOutputFile);                /* Byte 22..23 Number of channels (2) */
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);                /* Byte 24..27 Sample Rate (actual value later from decoder) */
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);                /* Byte 28..31 Average Bytes/Second (actual value later from decder) */
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);
#ifndef OUTPUT_24BIT
        fputc(2, pOutputFile);                /* Byte 32..33 Block Align (4 -- 2 bytes/channel * 2 channels) */
#else
        fputc(4, pOutputFile);                /* Byte 32..33 Block Align (32bit) */
#endif
        fputc(0, pOutputFile);
#ifndef OUTPUT_24BIT
        fputc(16, pOutputFile);               /* Byte 34..35 Bits Per Sample (16) */
#else
        fputc(32, pOutputFile);               /* Byte 34..35 Bits Per Sample (32) */
#endif
        fputc(0, pOutputFile);
        fwrite("data", 4, 1, pOutputFile);    /* Byte 36..39 data */
        fputc(0, pOutputFile);                /* Byte 40..43 data size - 4*/
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);
        fputc(0, pOutputFile);
    }

    if (cmdline_probe(&opts)) {
        return 1;
    }

    if (!opts.filename) {
        print_usage(argv[0]);
        return 0;
    }

    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    if ((opts.indexname && !strcmp(opts.indexname, "same")) ||
        opts.transportType == NEXUS_TransportType_eMkv ||
        opts.transportType == NEXUS_TransportType_eMp4
        )
    {
        opts.indexname = opts.filename;
    }

    file = NEXUS_FilePlay_OpenPosix(opts.filename, opts.indexname);
    if (!file) {
        fprintf(stderr, "can't open files:%s %s\n", opts.filename, opts.indexname);
        return -1;
    }
    if(opts.customFileIo) {
        customFile = file = FileIoCustom_Attach(file);
        BDBG_ASSERT(file);
    }
    if(opts.playbackMonitor) {
        stickyFile = file = FileIoSticky_Attach(file);
        BDBG_ASSERT(file);
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.modeSettings.Auto.behavior = opts.stcChannelMaster;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = opts.transportType;
    playbackSettings.playpumpSettings.timestamp.pacing = false;
    playbackSettings.playpumpSettings.timestamp.type = opts.tsTimestampType;

    playbackSettings.stcChannel = stcChannel;
    playbackSettings.stcTrick = opts.stcTrick;
    playbackSettings.beginningOfStreamAction = opts.beginningOfStreamAction;
    playbackSettings.endOfStreamAction = opts.endOfStreamAction;
    playbackSettings.endOfStreamCallback.callback = play_endOfStreamCallback;
    playbackSettings.enableStreamProcessing = opts.streamProcessing;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

#ifdef ENABLE_AUDYSSEY
    /* Audyssey */
    adv = NEXUS_AudysseyAdv_Open(NULL);
    abx = NEXUS_AudysseyAbx_Open(NULL);
  #if 1
    NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
    advSettings.enabled     = true;                 /* If true, processing is enabled.  Otherwise this stage is bypassed */
    advSettings.channelMask = 0xFF;                 /* Specifies the channels that will have gain applied.  
                                                       Default: 0xFF    All channels
                                                       Meaning of bits
                                                       0 -> L, 1 -> R, 2 -> C, 3 -> LFE
                                                       4 -> Ls, 5-> Rs, 6 -> Lb, 7 -> Rb */
    advSettings.reInitialize = false;               /* If true, processing will be reset/re-initialized. */
    advSettings.volumeScalingEnabled = false;       /* If true, dynamic volume scaling is enabled */
    advSettings.dynamicSurroundGainEnabled = true;  /* If true (default), surround channel gain is enabled.  */
    advSettings.hiLoCompress = false;               /* If true, there will be boost in calc cntr gain. */
    advSettings.dynamicEqMode = NEXUS_AudysseyAdvDynamicEqMode_eOff; /* Dynamic Equalizer Mode */
    advSettings.volume = 0;                         /* For Gain function and Dynamic EQ.  Default : 0. Range [0, -500] */
    advSettings.gainCompression = 180;              /* Gain Compression Factor.  Default :180, Range [1, 240] */
    advSettings.headroomOffset = 0;                 /* Headroom offset in dB.  Default : 0, Range -> -512 to 512 */
    advSettings.chCalbGain[8]  = 0;                 /* chCalbGain in dB. Default 0. Range -> -512 to 512 */
    advSettings.chCalcLevel[8] = 0;                 /* chCalcLevel in dB. Default 0. Range -> -512 to 512 */
    rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);

    NEXUS_AudysseyAbx_GetSettings(abx, &abxSettings);
    abxSettings.enabled = true;   /* If true, processing is enabled.  Otherwise this stage is bypassed */
    abxSettings.harmonicGain = 6; /* Default is 6(3 dB), Ranges from 0 to 30.  Every step increment is equivalent to 0.5 dB step*/
    abxSettings.filterSet = 1;    /* Default : 1, Range [1,2] 
                                    1->is for systems that roll-off lower (42/46 inch TV) 
                                    2->is for systems that roll-off higher(22 inch TV) */
    abxSettings.dryGain = 0;      /* Default is 0, Ranges from 0 to 30
                                    Typically always 0, Every step increament is equivalent to 0.5 dB step */
    rc = NEXUS_AudysseyAbx_SetSettings(abx, &abxSettings);
	
	NEXUS_AudioEqualizer_GetDefaultSettings(&eqSettings);
	eqSettings.mode = NEXUS_AudioEqualizerMode_eCustom;
	eqSettings.secondaryMode = NEXUS_AudioEqualizerMode_eDisabled;
	eqSettings.subsonicFilterEnabled = false;
	eqSettings.subwooferFilterEnabled = false;	
	/*eqSettings.modeSettings.custom.numFilters = 6;*/
  #endif


#endif

    i2so = NEXUS_I2sOutput_Open(0, NULL);


    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
    if(opts.audioCdb) {
        audioDecoderOpenSettings.fifoSize = opts.audioCdb*1024;
    }


#ifdef ENABLE_AUDYSSEY
    file_params = fopen(opts.config_file, "r");
    if (file_params)
    {
        parse_config_file(file_params, &advSettings, &abxSettings, &eqSettings);
        print_settings(advSettings, abxSettings); 
        printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");

        rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
        rc = NEXUS_AudysseyAbx_SetSettings(abx, &abxSettings);

        fclose(file_params);
    }
    else
    {
        printf("\n\n No config file provided - using defaults... \n\n\n");
    }
#endif

    /* Bring up audio decoders and outputs */
    /* audioDecoder = NEXUS_AudioDecoder_Open(0, &audioDecoderOpenSettings); */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    capture = NEXUS_AudioCapture_Open(0, NULL);
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*-----------------------------------------------------------Adding inputs to the corresponding output ports--------------------------------------------------------------------------------------- */
    BDBG_ASSERT(audioDecoder);
#ifdef ENABLE_AUDYSSEY

#define ADV_ABX_
/*#define ADV_ONLY*/
/*#define ABX_ONLY*/

	if (opts.audio_eq == true)
	{
		printf("Assigning equalizer 0 to DAC\n");
		eq1 = NEXUS_AudioEqualizer_Open(0, &eqSettings);
		assert(NULL != eq1);
		NEXUS_AudioOutput_SetEqualizer(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]), eq1);
/*		printf("Assign equalizer 1 to SPDIF");
		eq2 = NEXUS_AudioEqualizer_Open(1, &eqSettings);
		assert(NULL != eq2);
*/
		NEXUS_AudioOutput_SetEqualizer(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]), eq1);
	/*	NEXUS_AudioEqualizer_SetSettings(eq1, &eqSettings);
		NEXUS_AudioEqualizer_SetSettings(eq2, &eqSettings);
	*/
		NEXUS_AudioEqualizer_GetSettings(eq1, &eqSettings_tmp);
		printf("\nEQ1: NumFilters: %d\n", eqSettings_tmp.modeSettings.custom.numFilters);
		
		/*NEXUS_AudioEqualizer_GetSettings(eq2, &eqSettings_tmp);
		printf("EQ2: NumFilters: %d\n", eqSettings_tmp.modeSettings.custom.numFilters);
		*/
	}

#ifdef ADV_ABX_
    NEXUS_AudysseyAdv_AddInput(adv, NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudysseyAbx_AddInput(abx, NEXUS_AudysseyAdv_GetConnector(adv));
	NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]), NEXUS_AudysseyAbx_GetConnector(abx));
    NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(capture), NEXUS_AudysseyAbx_GetConnector(abx));
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),NEXUS_AudysseyAbx_GetConnector(abx));

#endif

/*  #ifdef CONNECT_ABX_TO_SPDIF_AND_I2SOUTPUT
                               NEXUS_AudysseyAbx_GetConnector(abx));
  #else
                               NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
  #endif

    NEXUS_AudioOutput_AddInput(NEXUS_I2sOutput_GetConnector(platformConfig.outputs.i2s[0]),
  #ifdef CONNECT_ABX_TO_SPDIF_AND_I2SOUTPUT
                               NEXUS_AudysseyAbx_GetConnector(abx));
  #else
                               NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
  #endif
*/

#ifdef ADV_ONLY
    NEXUS_AudysseyAdv_AddInput(adv, NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]), NEXUS_AudysseyAdv_GetConnector(adv));
    NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(capture), NEXUS_AudysseyAdv_GetConnector(adv));
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]), NEXUS_AudysseyAdv_GetConnector(adv));
#endif


#ifdef ABX_ONLY
    NEXUS_AudysseyAbx_AddInput(abx, NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]), NEXUS_AudysseyAbx_GetConnector(abx));
    NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(capture), NEXUS_AudysseyAbx_GetConnector(abx));
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]), NEXUS_AudysseyAbx_GetConnector(abx));
#endif

#else
    rc = NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
                                    NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    BDBG_ASSERT(!rc);

    if(opts.compressedAudio) {
        compressedDecoder = NEXUS_AudioDecoder_Open(1, &audioDecoderOpenSettings);
        BDBG_ASSERT(compressedDecoder);
        rc = NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                                        NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
        BDBG_ASSERT(!rc);
    } else {
        /* Send raw PCM data to SPDIF */
        rc = NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                                        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
        BDBG_ASSERT(!rc);
    }
#endif


    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = opts.displayType;
    displaySettings.format = opts.displayFormat;
    display = NEXUS_Display_Open(0, &displaySettings);
    if (opts.useCompositeOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
        BDBG_ASSERT(!rc);
    }
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if (opts.useComponentOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
        BDBG_ASSERT(!rc);
    }
#endif
#if NEXUS_NUM_PANEL_OUTPUTS
    if (opts.usePanelOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
        BDBG_ASSERT(!rc);
        NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    }
#endif
#if NEXUS_NUM_SCART_INPUTS
    if (opts.useScart1CompositeOutput) {
        printf("Turning on: scart 1 composite output\n");
        rc = NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
        BDBG_ASSERT(!rc);
    }
#endif
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*-------------------------------------------------------------------------Graphics and Video settings--------------------------------------------------------------------------------------------- */
    if (opts.graphics) {
        NEXUS_SurfaceCreateSettings surfaceCreateSettings;
        NEXUS_SurfaceMemory mem;
        NEXUS_GraphicsSettings graphicsSettings;
        NEXUS_VideoFormatInfo videoFormatInfo;
        unsigned i,j;

        NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);

        NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
        surfaceCreateSettings.width = 720;
        surfaceCreateSettings.height = videoFormatInfo.height;
        framebuffer = NEXUS_Surface_Create(&surfaceCreateSettings);
        NEXUS_Surface_GetMemory(framebuffer, &mem);
        for (i=0;i<surfaceCreateSettings.height;i++) {
            for (j=0;j<surfaceCreateSettings.width;j++) {
                /* create checker board */
                ((uint32_t*)((uint8_t*)mem.buffer + i*mem.pitch))[j] = (((i/50)%2) ^ ((j/50)%2)) ? 0x00000000 : 0xFFFFFFFF;
            }
        }
        NEXUS_Surface_Flush(framebuffer);

        NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
        graphicsSettings.enabled = true;
        graphicsSettings.clip.width = surfaceCreateSettings.width;
        graphicsSettings.clip.height = surfaceCreateSettings.height;
        rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
        BDBG_ASSERT(!rc);
    }

    window = NEXUS_VideoWindow_Open(display, 0);

    NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    windowSettings.contentMode = opts.contentMode;
    rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
    BDBG_ASSERT(!rc);

    if (opts.mad) {
        NEXUS_VideoWindowMadSettings madSettings;
        NEXUS_VideoWindow_GetMadSettings(window, &madSettings);
        madSettings.deinterlace = true;
        rc = NEXUS_VideoWindow_SetMadSettings(window, &madSettings);
        BDBG_ASSERT(!rc);
    }

    NEXUS_VideoDecoder_GetDefaultOpenSettings(&openSettings);
    if(opts.videoCdb) {
        openSettings.fifoSize = opts.videoCdb*1024;
    }
    if(opts.avc51) {
        openSettings.avc51Enabled = opts.avc51;
    }
    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, &openSettings);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
    displayVbiSettings.vbiSource = NEXUS_VideoDecoder_GetConnector(videoDecoder);
    displayVbiSettings.closedCaptionEnabled = opts.closedCaptionEnabled;
    displayVbiSettings.closedCaptionRouting = opts.closedCaptionEnabled;
    rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
    BDBG_ASSERT(!rc);

    /* Open the audio and video pid channels */
    if (opts.videoCodec != NEXUS_VideoCodec_eNone) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidSettings.pidSettings.allowTimestampReordering = opts.playpumpTimestampReordering;
        playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
        playbackPidSettings.pidTypeSettings.video.index = true;
        playbackPidSettings.pidTypeSettings.video.codec = opts.videoCodec;
        videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.videoPid, &playbackPidSettings);
    }

    if (opts.audioCodec != NEXUS_AudioCodec_eUnknown) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
        playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = opts.audioCodec;
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.audioPid, &playbackPidSettings);
    }

    if (opts.pcrPid && opts.pcrPid!=opts.videoPid && opts.pcrPid!=opts.audioPid) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther;
        pcrPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.pcrPid, &playbackPidSettings);
    }

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = opts.videoCodec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    videoProgram.frameRate = opts.videoFrameRate;
    videoProgram.aspectRatio = opts.aspectRatio;
    videoProgram.sampleAspectRatio.x = opts.sampleAspectRatio.x;
    videoProgram.sampleAspectRatio.y = opts.sampleAspectRatio.y;
    videoProgram.errorHandling = opts.videoErrorHandling;
    videoProgram.timestampMode = opts.decoderTimestampMode;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = opts.audioCodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------------------------- Start decoders ------------------------------------------------------------------------------------*/
    start_video(&opts, videoDecoder, &videoProgram);
    start_audio(&opts, audioDecoder, compressedDecoder, &audioProgram, capture, pOutputFile);
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*------------------------------------------------------------------------------------------ Start playback---------------------------------------------------------------------------------------- */
    NEXUS_Playback_GetDefaultStartSettings(&playbackStartSettings);
    if(opts.autoBitrate) {
        playbackStartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
    }
    rc = NEXUS_Playback_Start(playback, file, &playbackStartSettings);
    BDBG_ASSERT(!rc);
#if B_HAS_PLAYBACK_MONITOR
    {
        PlaybackMonitorState monitorState;
        BKNI_Memset(&monitorState, 0, sizeof(monitorState));
        monitorState.opts = &opts;
        monitorState.playback = playback;
        monitorState.videoDecoder = videoDecoder;
        monitorState.videoProgram = &videoProgram;
        monitorState.audioDecoder = audioDecoder;
        monitorState.compressedDecoder = compressedDecoder;
        monitorState.audioProgram = &audioProgram;
        monitorState.file = file;
        monitorState.stickyFile = stickyFile;
        monitorState.customFile = customFile;
        monitorState.playbackStartSettings = &playbackStartSettings;
        if(stickyFile) {
            monitor_thread_start(&monitorState);
        }
#endif
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*------------------------------------------------Run time modifications of the user config params if loopback is enabled-------------------------------------------------------------------------- */
if(opts.loopback==1)
{
    for(exit=false;!exit;) {
        char buffer[256];
        char *buf;

        char *value;

        NEXUS_PlaybackTrickModeSettings trickSettings;

        printf("audyssey>"); fflush(stdout);
        fgets(buffer, 256, stdin);
        if(feof(stdin)) {
            break;
        }

        buffer[strlen(buffer)-1] = 0;

        buf = strtok(buffer, ";");
        if (!buf) continue;
#if B_HAS_PLAYBACK_MONITOR
        if(stickyFile) {
            BKNI_AcquireMutex(monitorState.lock);
        }
#endif
        do {
            if (!strcmp(buf, "?")) {
                printf(
                "Commands:\n"
                "  play - resume normal playback\n"
                "  pause - pause playback\n"
                "  i - decode only I frames\n"
                "  ip - decode only I & P frames\n"
                "  all - decode all frames\n"
                "  fa - frame advance\n"
                "  fr - frame reverse\n"
                "  rate(rate) - set trick rate (floating point number, 1.0 is normal play)\n"
                "  seek(pos) - jump to position (in milliseconds)\n"
                "  sleep(sec) - sleep for given number of seconds\n"
                "  q - quit\n"
                "  st - print status\n"
                "  restart  - stop playback and restart play from the current position\n"
                );
            }
            else if (!strcmp(buf, "q")) {
                exit = true;
                break;
            }
/* AUDYSSEY COMMANDS **************************************************/
#define OBTAIN_VALUE_MACRO(cmd, value)             \
    value = strchr(buf, '=');                      \
    if (value == NULL)                             \
    {                                              \
        printf("Error, did not provide value!\n"); \
        break;                                     \
    }                                              \
    else                                           \
    {                                              \
        value++;                                   \
        printf("Command: %s = '%s' (%d)\n", cmd, value, atoi(value));  \
    }



#ifdef ENABLE_AUDYSSEY

            /* AudysseyAdv commands */
            else if (strstr(buf, "ADV_ENABLE") == buf) {
                OBTAIN_VALUE_MACRO("ADV_ENABLE", value);
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.enabled = atoi(value);
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ADV_GAINCOMPRESSION") == buf) {
                OBTAIN_VALUE_MACRO("ADV_GAINCOMPRESSION", value);
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.gainCompression = atoi(value);
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ADV_CHANNELMASK") == buf) {
                OBTAIN_VALUE_MACRO("ADV_CHANNELMASK", value);
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.channelMask = atoi(value);
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ADV_REINITIALIZE") == buf) {
                OBTAIN_VALUE_MACRO("ADV_REINITIALIZE", value);
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.reInitialize = atoi(value);
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ADV_VOLUMESCALINGENABLED") == buf) {
                OBTAIN_VALUE_MACRO("ADV_VOLUMESCALINGENABLED", value);
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.volumeScalingEnabled = atoi(value);
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ADV_DYNAMICSURROUNDGAINENABLED") == buf) {
                OBTAIN_VALUE_MACRO("ADV_DYNAMICSURROUNDGAINENABLED", value);
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.dynamicSurroundGainEnabled = atoi(value);
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ADV_HILOCOMPRESS") == buf) {
                OBTAIN_VALUE_MACRO("ADV_HILOCOMPRESS", value);
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.hiLoCompress = atoi(value);
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ADV_DYNAMICEQMODE") == buf) {
                OBTAIN_VALUE_MACRO("ADV_DYNAMICEQMODE", value);
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.dynamicEqMode = atoi(value);
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ADV_VOLUME") == buf) {
                OBTAIN_VALUE_MACRO("ADV_VOLUME", value);
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.volume = atoi(value);
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ADV_HEADROOMOFFSET") == buf) {
                OBTAIN_VALUE_MACRO("ADV_HEADROOMOFFSET", value);
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.headroomOffset = atoi(value);
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }

            /* Different syntax (like playback.c's command syntax, not like Audyssey config file syntax that Raptor team requested) */
            else if (strstr(buf, "adv_chCalbGain(") == buf) {
                int index, x;
                if(sscanf(buf+ strlen("adv_chCalbGain("),"%d,%d", &index, &x)==2) {
                    printf("adv_chCalbGain requested => index=%d, value=%d\n", index, x);
                }
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.chCalbGain[index] = x;
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "adv_chCalcLevel(") == buf) {
                int index, x;
                if(sscanf(buf+ strlen("adv_chCalcLevel("),"%d,%d", &index, &x)==2) {
                    printf("adv_chCalcLevel requested => index=%d, value=%d\n", index, x);
                }
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.chCalcLevel[index] = x;
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }

/*
            else if (strstr(buf, "adv_123(") == buf) {
                int x;
                if(sscanf(buf+ strlen("adv_123("),"%d", &x)==1) {
                    printf("adv_123 requested = %d\n", x);
                }
                NEXUS_AudysseyAdv_GetSettings(adv, &advSettings);
                advSettings.123 = x;
                rc = NEXUS_AudysseyAdv_SetSettings(adv, &advSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
*/

            /* AudysseyAbx commands */
            else if (strstr(buf, "ABX_ENABLE") == buf) {
                OBTAIN_VALUE_MACRO("ABX_ENABLE", value);
                NEXUS_AudysseyAbx_GetSettings(abx, &abxSettings);
                abxSettings.enabled = atoi(value);
                rc = NEXUS_AudysseyAbx_SetSettings(abx, &abxSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ABX_HARMONICGAIN") == buf) {
                OBTAIN_VALUE_MACRO("ABX_HARMONICGAIN", value);
                NEXUS_AudysseyAbx_GetSettings(abx, &abxSettings);
                abxSettings.harmonicGain = atoi(value);
                rc = NEXUS_AudysseyAbx_SetSettings(abx, &abxSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ABX_FILTERSET") == buf) {
                OBTAIN_VALUE_MACRO("ABX_FILTERSET", value);
                NEXUS_AudysseyAbx_GetSettings(abx, &abxSettings);
                abxSettings.filterSet = atoi(value);
                rc = NEXUS_AudysseyAbx_SetSettings(abx, &abxSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
            else if (strstr(buf, "ABX_DRYGAIN") == buf) {
                OBTAIN_VALUE_MACRO("ABX_DRYGAIN", value);
                NEXUS_AudysseyAbx_GetSettings(abx, &abxSettings);
                abxSettings.dryGain = atoi(value);
                rc = NEXUS_AudysseyAbx_SetSettings(abx, &abxSettings);
                if (rc != NEXUS_SUCCESS) { printf("Error occured (code 0x%X)\n", rc); }
            }
#endif

/**********************************************************************/

            else if (!strcmp(buf, "play")) {
                rc = NEXUS_Playback_Play(playback);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "fa")) {
                rc = NEXUS_Playback_FrameAdvance(playback, true);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "fr")) {
                rc = NEXUS_Playback_FrameAdvance(playback, false);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "pause")) {
                rc = NEXUS_Playback_Pause(playback);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "i")) {
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayI;
                trickSettings.skipControl = NEXUS_PlaybackSkipControl_eDecoder;
                trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                rc = NEXUS_Playback_TrickMode(playback, &trickSettings);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "ip")) {
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayIP;
                trickSettings.skipControl = NEXUS_PlaybackSkipControl_eDecoder;
                trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                rc = NEXUS_Playback_TrickMode(playback, &trickSettings);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "all")) {
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.mode = NEXUS_PlaybackHostTrickMode_eNormal;
                trickSettings.skipControl = NEXUS_PlaybackSkipControl_eDecoder;
                trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                rc = NEXUS_Playback_TrickMode(playback, &trickSettings);
                BDBG_ASSERT(!rc);
            }
            else if (strstr(buf, "sleep(") == buf) {
                unsigned msec;
                sscanf(buf+6, "%u", &msec);
                BKNI_Sleep(msec);
            }
            else if (strstr(buf, "rate(") == buf) {
                float rate;

                sscanf(buf+5, "%f", &rate);
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.rate = NEXUS_NORMAL_DECODE_RATE * rate;
                if(opts.maxDecoderRate) {
                    trickSettings.maxDecoderRate = opts.maxDecoderRate;
                }
                rc = NEXUS_Playback_TrickMode(playback, &trickSettings);
                BDBG_ASSERT(!rc);
            }
            else if (strstr(buf, "seek(") == buf) {
                unsigned pos;
                unsigned min,sec,msec;

                if(sscanf(buf+5,"%u:%u.%u", &min, &sec, &msec)==3) {
                    pos = (min*60+sec)*1000+msec;
                } if(sscanf(buf+5,"%u:%u", &min, &sec)==2) {
                    pos = (min*60+sec)*1000;
                } else {
                    sscanf(buf+5, "%u", &pos);
                }
                rc = NEXUS_Playback_Seek(playback, pos);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "st")) {
                NEXUS_VideoDecoderStatus vstatus;
                NEXUS_AudioDecoderStatus astatus;
                NEXUS_AudioDecoderStatus acstatus;
                NEXUS_PlaybackStatus pstatus;
                NEXUS_PlaypumpStatus pumpstatus;
                uint32_t stc;

                rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &vstatus);
                BDBG_ASSERT(!rc);
                if(opts.decodedAudio) {
                    rc = NEXUS_AudioDecoder_GetStatus(audioDecoder, &astatus);
                    BDBG_ASSERT(!rc);
                }
                if(compressedDecoder) {
                    rc = NEXUS_AudioDecoder_GetStatus(compressedDecoder, &acstatus);
                    BDBG_ASSERT(!rc);
                }

                rc = NEXUS_Playback_GetStatus(playback, &pstatus);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Playpump_GetStatus(playpump, &pumpstatus);
                NEXUS_StcChannel_GetStc(stcChannel, &stc);

                if (opts.videoPid) {
                    printf("video %u/%u (%u%%) pts=%u, stc=%u (diff %d) fps=%sHz\n", vstatus.fifoDepth, vstatus.fifoSize,
                        vstatus.fifoSize ? vstatus.fifoDepth * 100 / vstatus.fifoSize : 0,
                        vstatus.pts, stc, vstatus.ptsStcDifference,
                        g_videoFrameRateStrs[vstatus.frameRate].name);
                    if (vstatus.avdStatusBlock) {
                        printf("  avdStatusBlock=%#x\n", vstatus.avdStatusBlock);
                    }
                }
                if (opts.audioPid) {
                    if(opts.decodedAudio) {
                        printf("audio %u/%u (%u%%) pts=%u, stc=%u (diff %d)\n", astatus.fifoDepth, astatus.fifoSize,
                            astatus.fifoSize ? astatus.fifoDepth * 100 / astatus.fifoSize : 0,
                            astatus.pts, stc, astatus.ptsStcDifference);
                    }
                    if(compressedDecoder) {
                        printf("compressed %u/%u (%u%%) pts=%u, stc=%u (diff %d)\n", acstatus.fifoDepth, acstatus.fifoSize,
                            acstatus.fifoSize ? acstatus.fifoDepth * 100 / acstatus.fifoSize : 0,
                            acstatus.pts, stc, acstatus.ptsStcDifference);
                    }
                }
                printf("playback %u/%u (%u%%) pos=%u:%02u.%03u(%d:%d) last=%u:%02u.%03u\n", pstatus.fifoDepth, pstatus.fifoSize,
                    pstatus.fifoSize ? pstatus.fifoDepth * 100 / pstatus.fifoSize : 0,
                    (unsigned)pstatus.position/60000,
                    (unsigned)(pstatus.position%60000)/1000,
                    (unsigned)pstatus.position%1000,
                    (int)(pstatus.readPosition - pstatus.position),
                    (int)(pumpstatus.mediaPts - (opts.videoPid?vstatus.pts:astatus.pts))/45,
                    (unsigned)pstatus.last/60000,
                    (unsigned)(pstatus.last%60000)/1000,
                    (unsigned)pstatus.last%1000
                    );
            }
            else if (!strcmp(buf, "af")) {
                NEXUS_AudioDecoder_Flush(audioDecoder);
                if(compressedDecoder) {
                    NEXUS_AudioDecoder_Flush(compressedDecoder);
                }
            }
            else if (strcmp(buf, "restart")==0) {
                NEXUS_PlaybackStatus status;
                NEXUS_VideoDecoderSettings videoDecoderSettings;

                /* get current playback position */
                rc = NEXUS_Playback_GetStatus(playback, &status);
                BDBG_ASSERT(!rc);
                NEXUS_Playback_Stop(playback);

                if (opts.videoPid) {
                    /* don't show black frame */
                    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
                    videoDecoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilTsmLock;
                    rc=NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
                    BDBG_ASSERT(!rc);
                }
                /* stop decoder */
                stop_video(&opts, videoDecoder);
                stop_audio(&opts, audioDecoder, compressedDecoder, capture, pOutputFile);

                NEXUS_Playback_GetSettings(playback, &playbackSettings);
                playbackSettings.startPaused = true;
                rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
                BDBG_ASSERT(!rc);

                /* Start decoders */
                start_video(&opts, videoDecoder, &videoProgram);
                start_audio(&opts, audioDecoder, compressedDecoder, &audioProgram, capture, pOutputFile);

                /* start playback  */
                rc = NEXUS_Playback_Start(playback, file, &playbackStartSettings);
                BDBG_ASSERT(!rc);

                /* seek into desired location */
                rc = NEXUS_Playback_Seek(playback, status.position);
                BDBG_ASSERT(!rc);

                /* start playing */
                rc = NEXUS_Playback_Play(playback);
                BDBG_ASSERT(!rc);
            }
            else if (customFile && strstr(buf, "error_index(")==buf) {
                FileIoCustomProbabilities probabilities;
                FileIoCustom_GetProbabilities(customFile, NULL, &probabilities);
                sscanf(buf+strlen("error_index("),"%u,%u,%u", &probabilities.error, &probabilities.nodata, &probabilities.partial_read);
                FileIoCustom_SetProbabilities(customFile, NULL, &probabilities);
            }
            else if (customFile && strstr(buf, "error_data(")==buf) {
                FileIoCustomProbabilities probabilities;
                FileIoCustom_GetProbabilities(customFile , &probabilities, NULL);
                sscanf(buf+strlen("error_data("),"%u,%u,%u", &probabilities.error, &probabilities.nodata, &probabilities.partial_read);
                FileIoCustom_SetProbabilities(customFile, &probabilities, NULL);
            }
            else if (strstr(buf, "bvn_usage(") == buf) {
                unsigned id;

                if(sscanf(buf+10,"%u", &id)==1) {
                    NEXUS_DisplayModule_SetConfigurationId(id);
                    /* allow failure */
                }
            }
            else if (!*buf) {
                /* allow blank line */
            }
            else {
                printf("unknown command: %s\n", buf);
            }
        }
        while ((buf = strtok(NULL, ";")));
#if B_HAS_PLAYBACK_MONITOR
        if(stickyFile) {
            BKNI_ReleaseMutex(monitorState.lock);
        }
#endif
    }
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*------------------------------------------------------------------------Program stops at the end of the playback if loopback is not enabled------------------------------------------------------ */
else{
     #if B_HAS_PLAYBACK_MONITOR
                    if(stickyFile) {
                        BKNI_AcquireMutex(monitorState.lock);
                    }
     #endif

    rc = NEXUS_Playback_GetStatus(playback, &pstatus);
    BDBG_ASSERT(!rc);
    while(pstatus.state!=2) {
        sleep(1);
        rc = NEXUS_Playback_GetStatus(playback, &pstatus);
        BDBG_ASSERT(!rc);
     }
    
    #if B_HAS_PLAYBACK_MONITOR
            if(stickyFile) {
                BKNI_ReleaseMutex(monitorState.lock);
            }
    #endif

}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*------------------------------------------------------------------Stopping, Closing and Removing the used functionalities at the end of the run-------------------------------------------------- */
#if B_HAS_PLAYBACK_MONITOR
    if(stickyFile) {
        monitor_thread_stop(&monitorState);
    }
    }
#endif
    NEXUS_Playback_Stop(playback);
    stop_video(&opts, videoDecoder);
    stop_audio(&opts, audioDecoder, compressedDecoder, capture, pOutputFile);

    NEXUS_Playback_CloseAllPidChannels(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);


    NEXUS_VideoWindow_RemoveInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    if (framebuffer) {
        NEXUS_Surface_Destroy(framebuffer);
    }

	if (opts.audio_eq == true)
	{
		NEXUS_AudioEqualizer_Close (eq1);
/*		NEXUS_AudioEqualizer_Close (eq2);*/
	}
	NEXUS_AudioOutput_RemoveInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if(compressedDecoder) {
        NEXUS_AudioOutput_RemoveInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    } else {
        NEXUS_AudioOutput_RemoveInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }
    NEXUS_AudioOutput_RemoveInput(
        NEXUS_AudioCapture_GetConnector(capture),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));


    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    if(compressedDecoder) {
        NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
        NEXUS_AudioDecoder_Close(compressedDecoder);
    }
    NEXUS_Platform_Uninit();
#endif
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
    return 0;
}
