/******************************************************************************
 *    (c)2008-2013 Broadcom Corporation
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
 * $brcm_Workfile: playback.c $
 * $brcm_Revision: SW7425-5116-12.4/2 $
 * $brcm_Date: 12/13/13 11:51a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/utils/playback.c $
 * 
 * SW7425-5116-12.4/2   12/13/13 11:51a mpeteri
 * SW7425-5116: remove unused variable
 *
 * SW7425-5116-12.4/1   12/6/13 4:02p mpeteri
 * SW7425-5116: back port changes to 12.4
 * 
 * SW7425-5116/1   12/6/13 11:27a mpeteri
 * SW7425-5116: changes to allow 96k output rate from the decoder
 *
 * 98   10/29/12 10:19a erickson
 * SW7435-443: give playback handle for compressed decoder
 * 
 * 97   10/22/12 8:27a jgarrett
 * SW7425-295: Merge to main branch
 * 
 * SW7429-295/1   10/19/12 11:01a mpeteri
 * SW7429-295: port DTS-CD changes to 7429
 * 
 * 96   10/19/12 1:33p erickson
 * SW7231-1032: don't assert on unpaused frame advance failure
 * 
 * 95   9/11/12 3:34p jgarrett
 * SW7425-3796: Merge to main branch
 * 
 * SW7425-3796/2   9/11/12 3:29p jgarrett
 * SW7425-3796: Handling DTS-CD as a passthrough case
 * 
 * 94   7/30/12 3:44p erickson
 * SW7425-2130: add #if !BDBG_NO_WRN
 * 
 * 93   7/17/12 12:57p erickson
 * SWDEPRECATED-3465: add #if NEXUS_NUM_COMPOSITE_OUTPUTS
 * 
 * 92   6/26/12 4:00p jgarrett
 * SW7425-3317: Improving DTS-CD detection logic
 * 
 * 91   6/7/12 12:07p jgarrett
 * SW7425-2889: Merge to main branch
 * 
 * SW7425-2889/1   6/5/12 4:30p mpeteri
 * SW7425-2889: Add support for DTS-CD
 * 
 * 90   5/25/12 10:35a jgarrett
 * SW7425-2075: Adding HBR audio for DTS-HD and MLP
 * 
 * 89   5/2/12 12:59p erickson
 * SW7231-786: add spdif/hdmi audio format status, poll on hdmi until
 *  connected before making EDID decision
 * 
 * 88   4/13/12 5:02p erickson
 * SW7346-737: add -hdmi_crc feature
 * 
 * 87   3/15/12 2:54p erickson
 * SW7425-967: add print of NEXUS_VideoDecoder3DTVStatus if
 *  NEXUS_VideoDecoder3DTVFormat is not eNone
 * 
 * 86   3/15/12 9:42a erickson
 * SW7429-84: add clean uninit
 * 
 * 85   3/14/12 11:22a vsilyaev
 * SW7425-2599: Added 'no_decoded_video' option
 * 
 * 84   3/6/12 8:55a erickson
 * SW7425-2404: add beginningOfStreamCallback, use WRN to see time for
 *  stream wrap around, remove unused bvn_usage()
 * 
 * 83   3/1/12 4:47p erickson
 * SW7425-2282: check bounds before seek
 * 
 * 82   2/22/12 1:52p vsilyaev
 * SW7425-2258: Improved handling of MJPEG decoders
 * 
 * 81   2/21/12 12:21p erickson
 * SW7208-183: can't call cmdline_probe until after NEXUS_Platform_Init
 *  because it calls thunked core module functions
 * 
 * 80   2/14/12 12:32p erickson
 * SW7425-2282: seek now fails if out of bounds
 * 
 * 79   1/31/12 5:07p rjlewis
 * SW7425-1136: Added HDMI support (and some cleanup).
 * 
 * 78   1/26/12 1:32p erickson
 * SW7425-2250: print hex PTS/STC values, add audio/video queue depth to
 *  status
 * 
 * 77   1/19/12 11:52a erickson
 * SW7346-629: add -cmp_crc and -avd_crc options
 * 
 * 76   1/11/12 6:13p vsilyaev
 * SW7425-2112: Added option to limit playback rate
 * 
 * 75   1/10/12 11:11a jgarrett
 * SW7231-524: Fixing multichannel audio playback
 * 
 * 74   11/9/11 5:10p erickson
 * SW7405-5317: add host(bcm,X)
 * 
 * 73   11/9/11 2:14p erickson
 * SW7335-1368: avoid round to zero, clarify 3rd param of host() is
 *  slowmotion rate, not overall rate
 * 
 * 72   10/19/11 3:52p erickson
 * SW7335-1368: don't allow non-zero rate to round down to zero (pause)
 * 
 * 71   10/12/11 11:44a erickson
 * SW7425-1264: don't abs rate
 * 
 * 70   8/29/11 10:32a erickson
 * SW7425-1205: add host(gop,X) option
 * 
 * 69   7/13/11 8:39p vsilyaev
 * SWDTV-7897: Added code to wrap PCM file with the WAV header
 * 
 * 68   7/7/11 5:53p jtna
 * SW7425-869: added runtime option to override
 *  NEXUS_VideoDecoderSourceOrientation
 * 
 * 67   5/13/11 12:17p erickson
 * SW7420-1873: remove shutdown attribute
 * 
 * 66   5/11/11 12:49p erickson
 * SW7405-5317: add host trick mode command
 * 
 * 65   4/22/11 5:49p vsilyaev
 * SW7425-416: Added option to override display orientation
 * 
 * 64   3/23/11 5:41p vsilyaev
 * SW7422-406: Unified parsing of command line parameters
 * 
 * 63   3/23/11 2:48p vsilyaev
 * SW7422-14: Added status for the enhanced layer FIFO
 * 
 * 62   3/15/11 2:15p erickson
 * SW7346-101: fix dss playback
 * 
 * 61   3/11/11 3:35p vsilyaev
 * SW7422-14: Added support for decoding SVC/MVC streams
 * 
 * 60   3/4/11 3:18p jgarrett
 * SW7422-146: Adding multichannel audio support
 * 
 * 59   1/7/11 3:31p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 58   11/23/10 8:39p vsilyaev
 * SW35230-2195: Added compile time option to capture and restore stream
 *  data with relative timing
 * 
 * 57   11/17/10 3:58p jgarrett
 * SW7408-137: Adding HDMI output
 * 
 * 56   10/27/10 3:26p erickson
 * SW7422-69: fix dtv
 * 
 * 55   10/22/10 12:17p vsilyaev
 * SW3548-3106: Added option to start playback in a paused state
 * 
 * 54   10/5/10 10:41a erickson
 * SW7422-65: add index
 * 
 * 53   9/8/10 12:06p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * 52   8/31/10 2:44p erickson
 * SWGIGGSVIZIO-57: add -fixed_bitrate option to set
 *  NEXUS_PlaybackMode_eFixedBitrate
 *
 * 51   8/9/10 6:48a erickson
 * SW7420-933: don't assert for fast or reverse trick modes
 *
 * 50   7/14/10 6:12p vsilyaev
 * SW3556-1131: Added basic support for CDXA format
 *
 * 49   6/18/10 4:38p vsilyaev
 * SW3548-2988: Don't open pidChannel if pid is 0
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

#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_video_decoder_extra.h"
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
#include "nexus_audio_playback.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_surface.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif
#include "cmdline_args.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "fileio_custom.h"
#include "bmedia_cdxa.h"
#include "nexus_file_pvr.h"

BDBG_MODULE(playback);

#if !NEXUS_HAS_PLAYBACK
int main(void)
{
    printf("This application is not supported on this platform (needs Playback)!\n");
    return 0;
}
#else

static void print_cmp_crc(NEXUS_DisplayHandle display);
static void print_avd_crc(NEXUS_VideoDecoderHandle videoDecoder);
static void print_hdmi_crc(NEXUS_HdmiOutputHandle display);

/* this should be enabled with conjunction of enabling  READ_TIMED_DATA in BSEAV/lib/utils/bsink_playback.c */
/* #define READ_TIMED_DATA 1 */

#if READ_TIMED_DATA
#include "nexus_memory.h"
#include "barena.h"
static void *b_nexus_alloc(balloc_iface_t alloc, size_t size)
{
    NEXUS_Error rc;
    void *b;
    BSTD_UNUSED(alloc);
    rc = NEXUS_Memory_Allocate(size, NULL, &b);
    if(rc==NEXUS_SUCCESS) {return b;}
    else {return NULL;}
}

static void b_nexus_free(balloc_iface_t alloc, void *ptr)
{
    BSTD_UNUSED(alloc);
    NEXUS_Memory_Free(ptr);
}

static const struct balloc_iface b_nexus_alloc_iface = {
    b_nexus_alloc, b_nexus_free
};

extern balloc_iface_t bsink_dump_reader_allocator;
#endif /* READ_TIMED_DATA */


static void start_video(const struct util_opts_t *opts, NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *videoProgram)
{
    NEXUS_Error rc;
    if (opts->common.decodedVideo && opts->common.videoPid) {
        rc = NEXUS_VideoDecoder_Start(videoDecoder, videoProgram);
        if(rc!=NEXUS_SUCCESS) {
            bool supported=true;
            NEXUS_VideoDecoder_IsCodecSupported(videoDecoder, videoProgram->codec, &supported);
            if(!supported) {
                unsigned i;
                bool found_supported=false;
                unsigned num_decoders = NEXUS_NUM_VIDEO_DECODERS;

                BDBG_LOG(("VideoDecoder:%u doesn't support codec %u", opts->common.videoDecoder, videoProgram->codec));
#if NEXUS_NUM_DSP_VIDEO_DECODERS
                num_decoders += NEXUS_NUM_DSP_VIDEO_DECODERS;
#endif

#if NEXUS_NUM_SID_VIDEO_DECODERS
                num_decoders += NEXUS_NUM_SID_VIDEO_DECODERS;
#endif

                for(i=0;i<num_decoders;i++) {
                    NEXUS_VideoDecoderHandle decoder;
                    if(i==opts->common.videoDecoder) {
                        continue;
                    }
                    decoder = NEXUS_VideoDecoder_Open(i, NULL);
                    if(decoder) {
                        NEXUS_VideoDecoder_IsCodecSupported(decoder, videoProgram->codec, &supported);
                        if(supported) {
                            if(!found_supported) {
                                found_supported = true;
                                BDBG_LOG(("try to run with the '-video_decoder n' option, where 'n' is the index of the video decoder that supports codec %u", videoProgram->codec));
                            }
                            BDBG_LOG(("VideoDecoder:%u supports codec %u", i, videoProgram->codec));
                        }
                        NEXUS_VideoDecoder_Close(decoder);
                    }
                }
            }
            BDBG_ASSERT(false);
        }
    }
    return;
}
static void stop_video(const struct util_opts_t *opts, NEXUS_VideoDecoderHandle videoDecoder)
{
    if (opts->common.videoPid) {
        NEXUS_VideoDecoder_Stop(videoDecoder);
    }
    return;
}

static void start_audio(const struct util_opts_t *opts, NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderHandle compressedDecoder, const NEXUS_AudioDecoderStartSettings *audioProgram)
{
    NEXUS_Error rc;
    if (opts->common.audioPid) {
#if B_HAS_ASF
        /* if DRC for WMA pro is available apply now */
        if(audioProgram->codec == NEXUS_AudioCodec_eWmaPro && opts->common.dynamicRangeControlValid ){

            NEXUS_AudioDecoderCodecSettings codecSettings;

            BDBG_WRN(("wma-pro drc enabled,ref peak %d,ref target %d,ave peak %d, ave target %d",
                      opts->common.dynamicRangeControl.peakReference,opts->common.dynamicRangeControl.peakTarget,
                      opts->common.dynamicRangeControl.averageReference,opts->common.dynamicRangeControl.averageTarget));
            NEXUS_AudioDecoder_GetCodecSettings(audioDecoder, audioProgram->codec, &codecSettings);
            codecSettings.codec = audioProgram->codec;
            codecSettings.codecSettings.wmaPro.dynamicRangeControlValid = true;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.peakReference = opts->common.dynamicRangeControl.peakReference;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.peakTarget = opts->common.dynamicRangeControl.peakTarget;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.averageReference = opts->common.dynamicRangeControl.averageReference;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.averageTarget = opts->common.dynamicRangeControl.averageTarget;
            NEXUS_AudioDecoder_SetCodecSettings(audioDecoder,&codecSettings);
            }
#endif

        if(opts->common.decodedAudio) {
            rc = NEXUS_AudioDecoder_Start(audioDecoder, audioProgram);
            BDBG_ASSERT(!rc);
        }
        if(compressedDecoder) {
            rc = NEXUS_AudioDecoder_Start(compressedDecoder, audioProgram);
            /* This may fail in some cases because we haven't atached any outputs.  Don't assert. */
            /*BDBG_ASSERT(!rc);*/
        }
    }
    return;
}

static void stop_audio(const struct util_opts_t *opts, NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderHandle compressedDecoder)
{
    if (opts->common.audioPid) {
        if(opts->common.decodedAudio) {
            NEXUS_AudioDecoder_Stop(audioDecoder);
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
        stop_audio(state->opts, state->audioDecoder, state->compressedDecoder);

        NEXUS_Playback_GetSettings(state->playback, &playbackSettings);
        playbackSettings.startPaused = true;
        rc = NEXUS_Playback_SetSettings(state->playback, &playbackSettings);
        BDBG_ASSERT(!rc);

        /* Start decoders */
        start_video(state->opts, state->videoDecoder, state->videoProgram);
        start_audio(state->opts, state->audioDecoder, state->compressedDecoder, state->audioProgram);

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

static void endOfStreamCallback(void *context, int param)
{
    BSTD_UNUSED(context);
    if (param) {
        BDBG_WRN(("end of stream"));
    }
    else {
        BDBG_WRN(("beginning of stream"));
    }
    return;
}

struct CdxaFile {
    struct NEXUS_FilePlay interface; /* must be the first member */
    bcdxa_file_t data;
    bcdxa_file_t index;
    NEXUS_FilePlayHandle parent;
};

static void Cdxa_File_Close(struct NEXUS_FilePlay *file_)
{
    struct CdxaFile *file = (void *)file_;
    bcdxa_file_destroy(file->data);
    if(file->index) {
        bcdxa_file_destroy(file->index);
    }
    file->parent->file.close(file->parent);
    BKNI_Free(file);
    return;
}

static NEXUS_FilePlayHandle Cdxa_File_Attach(NEXUS_FilePlayHandle parent)
{
    struct CdxaFile *file;
    file = BKNI_Malloc(sizeof(*file));
    BDBG_ASSERT(file);
    file->parent = parent;
    file->data = bcdxa_file_create(parent->file.data);
    file->interface.file.close = Cdxa_File_Close;
    BDBG_ASSERT(file->data);
    file->interface.file.data = bcdxa_file_get_file_interface(file->data);
    file->interface.file.index = NULL;
    file->index = NULL;
    if(parent->file.index) {
        file->index = bcdxa_file_create(parent->file.index);
        BDBG_ASSERT(file->index);
        file->interface.file.index = bcdxa_file_get_file_interface(file->index);
    }
    return &file->interface;
}

struct PcmFile {
    struct NEXUS_FilePlay interface; /* must be the first member */
    bpcm_file_t data;
    bpcm_file_t index;
    NEXUS_FilePlayHandle parent;
};

static void Pcm_File_Close(struct NEXUS_FilePlay *file_)
{
    struct PcmFile *file = (void *)file_;
    bpcm_file_destroy(file->data);
    if(file->index) {
        bpcm_file_destroy(file->index);
    }
    file->parent->file.close(file->parent);
    BKNI_Free(file);
    return;
}

static NEXUS_FilePlayHandle Pcm_File_Attach(NEXUS_FilePlayHandle parent, const bpcm_file_config *pcm_config)
{
    struct PcmFile *file;
    file = BKNI_Malloc(sizeof(*file));
    BDBG_ASSERT(file);
    file->parent = parent;
    file->data = bpcm_file_create(parent->file.data, pcm_config);
    BDBG_ASSERT(file->data);
    file->interface.file.data = bpcm_file_get_file_interface(file->data);
    if(parent->file.index) {
        file->index = bpcm_file_create(parent->file.index, pcm_config);
        BDBG_ASSERT(file->index);
        file->interface.file.index= bpcm_file_get_file_interface(file->index);
    } else {
        file->interface.file.index = NULL;
        file->index = NULL;
    }
    file->interface.file.close = Pcm_File_Close;
    return &file->interface;
}

#if NEXUS_NUM_HDMI_OUTPUTS
/* registered HDMI hotplug handler -- changes the format (to monitor's default) if monitor doesn't support current format */
static void hotplug_callback(void *pParam, int iParam)
{
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_HdmiOutputHandle hdmi = pParam;
    NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)iParam;
    NEXUS_Error rc;

    rc = NEXUS_HdmiOutput_GetStatus(hdmi, &hdmiStatus);
    BDBG_MSG(("HDMI event: %s", hdmiStatus.connected?"connected":"not connected"));

    /* the app can choose to switch to the preferred format, but it's not required. */
    if ( !rc && hdmiStatus.connected )
    {
        NEXUS_DisplaySettings displaySettings;
        
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] )
        {
            BDBG_WRN(("Current format not supported by attached monitor -- switching to preferred format (%d)", hdmiStatus.preferredVideoFormat));
            if (hdmiStatus.preferredVideoFormat >= NEXUS_VideoFormat_e480p) {
                BDBG_WRN(("Warning: This format may disable composite output!"));
            }
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    }
}
#endif

int main(int argc, const char *argv[])
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel = NULL, audioPidChannel = NULL, pcrPidChannel, videoExtPidChannel=NULL;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder, compressedDecoder=NULL;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file,customFile=NULL, stickyFile=NULL, cdxaFile=NULL, pcmFile=NULL;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_PlaybackStartSettings playbackStartSettings;
    NEXUS_VideoDecoderOpenSettings openSettings;
    NEXUS_Error rc;
    bool exit;
    NEXUS_SurfaceHandle framebuffer = NULL;
    NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;
    struct util_opts_t opts;

    if (cmdline_parse(argc, argv, &opts)) {
        return 0;
    }
    /* They MUST include a file to play */
    if (!opts.filename) {
        BDBG_ERR(("Missing playback file; See usage."));
        print_usage(argv[0]);
        return 0;
    }

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    if(opts.avc51) {
        platformSettings.videoDecoderModuleSettings.heapSize[0].general = (84*1024*1024);
        platformSettings.videoDecoderModuleSettings.numDecodes = 1;
    }

    if ( opts.common.maxAudioRate > 48000 )
    {
        platformSettings.audioModuleSettings.maxPcmSampleRate = opts.common.maxAudioRate;
    }
    else
    {
        platformSettings.audioModuleSettings.maxPcmSampleRate = 48000;
    }

    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    if (cmdline_probe(&opts.common, opts.filename, &opts.indexname)) {
        BERR_TRACE(NEXUS_UNKNOWN);
        NEXUS_Platform_Uninit();
        return -1;
    }

    if ((opts.common.transportType == NEXUS_TransportType_eWav || 
         opts.common.transportType == NEXUS_TransportType_eEs) &&
        opts.common.audioCodec == NEXUS_AudioCodec_eDtsLegacy)
    {
        opts.common.transportType = NEXUS_TransportType_eEs;
        opts.common.dtsCd = true;
    }

    if ((opts.indexname && !strcmp(opts.indexname, "same")) ||
        opts.common.transportType == NEXUS_TransportType_eMkv ||
        opts.common.transportType == NEXUS_TransportType_eMp4
        )
    {
        opts.indexname = opts.filename;
    }

    file = NEXUS_FilePlay_OpenPosix(opts.filename, opts.indexname);
    if (!file) {
        BDBG_ERR(("can't open files: '%s' '%s'", opts.filename, opts.indexname));
        NEXUS_Platform_Uninit();
        return -1;
    }
    
    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    if(opts.customFileIo) {
        customFile = file = FileIoCustom_Attach(file);
        BDBG_ASSERT(file);
    }
    if(opts.common.cdxaFile) {
        cdxaFile = file = Cdxa_File_Attach(file);
    }
    if(opts.common.pcm) {
        pcmFile = file = Pcm_File_Attach(file, &opts.common.pcm_config);
    }
    if(opts.playbackMonitor) {
        stickyFile = file = FileIoSticky_Attach(file);
        BDBG_ASSERT(file);
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.modeSettings.Auto.behavior = opts.stcChannelMaster;
    stcSettings.modeSettings.Auto.transportType = opts.common.transportType;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = opts.common.transportType;
    playbackSettings.playpumpSettings.timestamp.pacing = false;
    playbackSettings.playpumpSettings.timestamp.type = opts.common.tsTimestampType;
    if(opts.maxPlaybackDataRate) {
        playbackSettings.playpumpSettings.maxDataRate = opts.maxPlaybackDataRate;
    }
    playbackSettings.startPaused = opts.startPaused;
    playbackSettings.stcChannel = stcChannel;
    playbackSettings.stcTrick = opts.stcTrick;
    playbackSettings.beginningOfStreamAction = opts.beginningOfStreamAction;
    playbackSettings.beginningOfStreamCallback.callback = endOfStreamCallback;
    playbackSettings.beginningOfStreamCallback.param = 0;
    playbackSettings.endOfStreamAction = opts.endOfStreamAction;
    playbackSettings.endOfStreamCallback.callback = endOfStreamCallback;
    playbackSettings.endOfStreamCallback.param = 1;
    playbackSettings.enableStreamProcessing = opts.streamProcessing;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
    if(opts.common.audioCdb) {
        audioDecoderOpenSettings.fifoSize = opts.common.audioCdb*1024;
    }
    if (opts.common.multichannelAudio) {
        audioDecoderOpenSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_e5_1;
    }
    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, &audioDecoderOpenSettings);
    BDBG_ASSERT(audioDecoder);

    if ( opts.common.dtsCd )
    {
        NEXUS_AudioDecoderCodecSettings audioCodecSettings;
        NEXUS_AudioDecoder_GetCodecSettings(audioDecoder, NEXUS_AudioCodec_eDtsLegacy, &audioCodecSettings);
        audioCodecSettings.codecSettings.dts.littleEndian = true;
        rc = NEXUS_AudioDecoder_SetCodecSettings(audioDecoder, &audioCodecSettings);
        BDBG_ASSERT(!rc);
    }

    if (opts.common.audioPid) {
        rc = NEXUS_AudioOutput_AddInput(
            NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioConnectorType_eStereo));
        BDBG_ASSERT(!rc);
    }
    if (opts.common.compressedAudio) {
        audioDecoderOpenSettings.multichannelFormat = NEXUS_AudioMultichannelFormat_eNone;
        compressedDecoder = NEXUS_AudioDecoder_Open(1, &audioDecoderOpenSettings);
        BDBG_ASSERT(compressedDecoder);
        if ( opts.common.dtsCd )
        {
            NEXUS_AudioDecoderCodecSettings audioCodecSettings;
            NEXUS_AudioDecoder_GetCodecSettings(compressedDecoder, NEXUS_AudioCodec_eDtsLegacy, &audioCodecSettings);
            audioCodecSettings.codecSettings.dts.littleEndian = true;
            rc = NEXUS_AudioDecoder_SetCodecSettings(compressedDecoder, &audioCodecSettings);
            BDBG_ASSERT(!rc);
        }
        if ( opts.common.audioCodec == NEXUS_AudioCodec_eAc3Plus || opts.common.audioCodec == NEXUS_AudioCodec_eWmaPro )
        {
            /* These codecs pasthrough as part of decode (ac3+ is transcoded to ac3, wma pro is not transcoded) */
            BDBG_WRN(("spdif audio: decoder 0 -> compressed"));
            rc = NEXUS_AudioOutput_AddInput(
                NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioConnectorType_eCompressed));
        }
        else if ( opts.common.audioCodec && opts.common.audioCodec != NEXUS_AudioCodec_eMlp )
        {
            BDBG_WRN(("spdif audio: decoder 1 -> compressed"));
            rc = NEXUS_AudioOutput_AddInput(
                NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
                NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioConnectorType_eCompressed));
        }
        BDBG_ASSERT(!rc);
    }
    else
    {
        BDBG_WRN(("spdif audio: decoder 0 -> stereo"));
        rc = NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioConnectorType_eStereo));
        BDBG_ASSERT(!rc);
    }
#if NEXUS_NUM_HDMI_OUTPUTS
    if (opts.common.useHdmiOutput)
    {
#if !BDBG_NO_WRN
        static const char *g_audioConnectorTypeStr[NEXUS_AudioConnectorType_eMax] = { "stereo", "multichannel", "compressed", "compressed 4x", "compressed HBR", "mono" };
#endif
        NEXUS_HdmiOutputStatus hdmiStatus;
        NEXUS_AudioDecoderHandle hdmiDecoder = audioDecoder;
        NEXUS_AudioConnectorType connectorType = NEXUS_AudioConnectorType_eStereo;
        NEXUS_AudioInput connector;
        unsigned pollcnt = 10; /* 1 second = 10 loops of 100 msec */
        
        while (pollcnt--) {
            rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
            BDBG_ASSERT(!rc);
            if (hdmiStatus.connected) {
                break;
            }
            BKNI_Sleep(100);
        }

        if (!hdmiStatus.connected) {
            BDBG_WRN(("HDMI not connected"));
            /* if we don't know the EDID, output 2 ch pcm */
            connectorType = NEXUS_AudioDecoderConnectorType_eStereo;
        }
        else if ( opts.common.multichannelAudio && hdmiStatus.maxAudioPcmChannels > 2 )
        {
            connectorType = NEXUS_AudioConnectorType_eMultichannel;
        }
        else if ( opts.common.compressedAudio )
        {
            if ( hdmiStatus.audioCodecSupported[opts.common.audioCodec] )
            {
                /* Codec is directly supported */
                hdmiDecoder = (opts.common.audioCodec == NEXUS_AudioCodec_eWmaPro) ? audioDecoder:compressedDecoder;
                switch ( opts.common.audioCodec )
                {
                case NEXUS_AudioCodec_eMlp:
                    connectorType = NEXUS_AudioConnectorType_eCompressed16x;
                    break;
                case NEXUS_AudioCodec_eAc3Plus:
                    connectorType = NEXUS_AudioConnectorType_eCompressed4x;
                    break;
                case NEXUS_AudioCodec_eDtsHd:
                    {
                        NEXUS_HdmiOutputBasicEdidData basicEdid;
                        NEXUS_HdmiOutput_GetBasicEdidData(platformConfig.outputs.hdmi[0], &basicEdid);
                        if ( basicEdid.edidRevision >= 3 )  /* Check for HDMI 1.3 or later */
                        {
                            connectorType = NEXUS_AudioConnectorType_eCompressed16x;      /* Send DTS-HD as MA (should parse stream to select but this is simpler) */
                        }
                        else 
                        {
                            connectorType = NEXUS_AudioConnectorType_eCompressed4x;      /* Send DTS-HD as HRA not MA */
                        }
                    }
                    break;
                default:
                    connectorType = NEXUS_AudioConnectorType_eCompressed;
                    break;
                }
            }
            else if ( opts.common.audioCodec == NEXUS_AudioCodec_eAc3Plus && hdmiStatus.audioCodecSupported[NEXUS_AudioCodec_eAc3] )
            {
                /* AC3 is supported, convert ac3+ to ac3. */
                connectorType = NEXUS_AudioConnectorType_eCompressed;
            }
            else if ( opts.common.audioCodec == NEXUS_AudioCodec_eDtsLegacy || opts.common.audioCodec == NEXUS_AudioCodec_eDts )
            {
                if ( hdmiStatus.audioCodecSupported[NEXUS_AudioCodec_eDts] || hdmiStatus.audioCodecSupported[NEXUS_AudioCodec_eDtsHd] )
                {
                    hdmiDecoder = compressedDecoder;
                    connectorType = NEXUS_AudioConnectorType_eCompressed;
                }
            }
        }

        BDBG_WRN(("hdmi audio: decoder %d -> %s", hdmiDecoder==audioDecoder?0:1, g_audioConnectorTypeStr[connectorType]));
        connector = NEXUS_AudioDecoder_GetConnector(hdmiDecoder, connectorType);
        if ( NULL == connector )
        {
            /* This decoder doesn't support HBR output.  Try standard compressed instead */
            if ( connectorType == NEXUS_AudioConnectorType_eCompressed16x || connectorType == NEXUS_AudioConnectorType_eCompressed4x )
            {
                BDBG_WRN(("hdmi audio: decoder %d -> %s", hdmiDecoder==audioDecoder?0:1, g_audioConnectorTypeStr[NEXUS_AudioConnectorType_eCompressed]));
                connector = NEXUS_AudioDecoder_GetConnector(hdmiDecoder, NEXUS_AudioConnectorType_eCompressed);
            }
        }
        if ( NULL != connector )
        {
            rc = NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]), connector);
            BDBG_ASSERT(!rc);
        }
    }
#endif /*NEXUS_NUM_HDMI_OUTPUTS*/

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = opts.common.displayType;
    displaySettings.format = opts.common.displayFormat;
    if(opts.common.displayOrientation != NEXUS_VideoOrientation_e2D) {
        displaySettings.display3DSettings.overrideOrientation = true;
        displaySettings.display3DSettings.orientation = opts.common.displayOrientation;
    }
    if (opts.cmp_crc) {
        displaySettings.crcQueueSize = 60; /* This enables the CRC capture */
    }
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if (opts.common.useCompositeOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
        BDBG_ASSERT(!rc);
    }
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if (opts.common.useComponentOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
        BDBG_ASSERT(!rc);
    }
#endif
#if NEXUS_NUM_PANEL_OUTPUTS
    if (opts.common.usePanelOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
        BDBG_ASSERT(!rc);
        NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    }
#endif
#if NEXUS_NUM_SCART_INPUTS
    if (opts.common.useScart1CompositeOutput) {
        printf("Turning on: scart 1 composite output\n");
        rc = NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
        BDBG_ASSERT(!rc);
    }
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    if (opts.common.useHdmiOutput) {
        NEXUS_HdmiOutputSettings hdmiSettings;

        rc = NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
        BDBG_ASSERT(!rc);

        /* Install hotplug callback -- video only for now */
        NEXUS_HdmiOutput_GetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
        hdmiSettings.hotplugCallback.callback = hotplug_callback;
        hdmiSettings.hotplugCallback.context = platformConfig.outputs.hdmi[0];
        hdmiSettings.hotplugCallback.param = (int)display;
        if (opts.hdmi_crc) {
            hdmiSettings.crcQueueSize = 60; /* This enables the CRC capture */
        }
        NEXUS_HdmiOutput_SetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);

        /* Force a hotplug to switch to preferred format */
        hotplug_callback(platformConfig.outputs.hdmi[0], (int)display);
    }
#endif

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
    windowSettings.contentMode = opts.common.contentMode;
    rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
    BDBG_ASSERT(!rc);

    if (opts.common.mad) {
        NEXUS_VideoWindowMadSettings madSettings;
        NEXUS_VideoWindow_GetMadSettings(window, &madSettings);
        madSettings.deinterlace = true;
        rc = NEXUS_VideoWindow_SetMadSettings(window, &madSettings);
        BDBG_ASSERT(!rc);
    }

    NEXUS_VideoDecoder_GetDefaultOpenSettings(&openSettings);
    if(opts.common.videoCdb) {
        openSettings.fifoSize = opts.common.videoCdb*1024;
    }
    if(opts.avc51) {
        openSettings.avc51Enabled = opts.avc51;
    }
    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(opts.common.videoDecoder, &openSettings);
    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    if (opts.common.sourceOrientation != NEXUS_VideoDecoderSourceOrientation_e2D) {
        videoDecoderSettings.customSourceOrientation = true;
        videoDecoderSettings.sourceOrientation = opts.common.sourceOrientation;
    }
    rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    BDBG_ASSERT(!rc);
    if (opts.avd_crc) {
        NEXUS_VideoDecoderExtendedSettings settings;
        NEXUS_VideoDecoder_GetExtendedSettings(videoDecoder, &settings);
        settings.crcFifoSize = 30;
        rc = NEXUS_VideoDecoder_SetExtendedSettings(videoDecoder, &settings);
        BDBG_ASSERT(!rc);
    }
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
    displayVbiSettings.vbiSource = NEXUS_VideoDecoder_GetConnector(videoDecoder);
    displayVbiSettings.closedCaptionEnabled = opts.closedCaptionEnabled;
    displayVbiSettings.closedCaptionRouting = opts.closedCaptionEnabled;
    rc = NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
    BDBG_ASSERT(!rc);

    /* Open the audio and video pid channels */
    if (opts.common.videoCodec != NEXUS_VideoCodec_eNone && opts.common.videoPid!=0) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidSettings.pidSettings.allowTimestampReordering = opts.common.playpumpTimestampReordering;
        playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
        playbackPidSettings.pidTypeSettings.video.index = true;
        playbackPidSettings.pidTypeSettings.video.codec = opts.common.videoCodec;
        videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.common.videoPid, &playbackPidSettings);
    }

    if (opts.common.extVideoCodec != NEXUS_VideoCodec_eNone && opts.common.extVideoPid!=0) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidSettings.pidTypeSettings.video.index = true;
        playbackPidSettings.pidSettings.allowTimestampReordering = opts.common.playpumpTimestampReordering;
        playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
        playbackPidSettings.pidTypeSettings.video.codec = opts.common.extVideoCodec;
        videoExtPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.common.extVideoPid, &playbackPidSettings);
    }

    if (opts.common.audioCodec != NEXUS_AudioCodec_eUnknown && opts.common.audioPid!=0) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
        playbackPidSettings.pidTypeSettings.audio.secondary = compressedDecoder;
        playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = opts.common.audioCodec;
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.common.audioPid, &playbackPidSettings);
    }

    if (opts.common.pcrPid && opts.common.pcrPid!=opts.common.videoPid && opts.common.pcrPid!=opts.common.audioPid) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther;
        pcrPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.common.pcrPid, &playbackPidSettings);
    }

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = opts.common.videoCodec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    videoProgram.frameRate = opts.common.videoFrameRate;
    videoProgram.aspectRatio = opts.common.aspectRatio;
    videoProgram.sampleAspectRatio.x = opts.common.sampleAspectRatio.x;
    videoProgram.sampleAspectRatio.y = opts.common.sampleAspectRatio.y;
    videoProgram.errorHandling = opts.videoErrorHandling;
    videoProgram.timestampMode = opts.common.decoderTimestampMode;
    if(videoExtPidChannel) {
        videoProgram.enhancementPidChannel = videoExtPidChannel;
        videoProgram.codec = opts.common.extVideoCodec;
    }
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = opts.common.audioCodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;
    audioProgram.maxOutputRate = opts.common.maxAudioRate;

    /* Start decoders */
    start_video(&opts, videoDecoder, &videoProgram);
    start_audio(&opts, audioDecoder, compressedDecoder, &audioProgram);

    /* Start playback */
    NEXUS_Playback_GetDefaultStartSettings(&playbackStartSettings);
    if (opts.fixedBitrate) {
        playbackStartSettings.mode = NEXUS_PlaybackMode_eFixedBitrate;
        playbackStartSettings.bitrate = opts.fixedBitrate;
    }
    else if (opts.autoBitrate) {
        playbackStartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
    }
#if READ_TIMED_DATA 
    {
        barena_t arena;

        arena = barena_create(&b_nexus_alloc_iface, 2*1024*1024);
        BDBG_ASSERT(arena);
        bsink_dump_reader_allocator = barena_alloc_iface(arena);
    }
#endif
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

    if (opts.avd_crc) {
        print_avd_crc(videoDecoder);
    }
    if (opts.cmp_crc) {
        print_cmp_crc(display);
    }
    if (opts.hdmi_crc) {
        print_hdmi_crc(platformConfig.outputs.hdmi[0]);
    }

    for (exit=false;!exit;)
    {
        char buffer[256];
        char *buf;

        printf("playback>"); fflush(stdout);
        fgets(buffer, 256, stdin);
        if (feof(stdin)) break;
        buffer[strlen(buffer)-1] = 0; /* chop off \n */

        buf = strtok(buffer, ";");
        if (!buf) continue;

#if B_HAS_PLAYBACK_MONITOR
        if(stickyFile) {
            BKNI_AcquireMutex(monitorState.lock);
        }
#endif
        do {
            if (!strcmp(buf, "?") || !strcmp(buf, "help")) {
                printf(
                "Commands:\n"
                "  play - resume normal playback\n"
                "  pause - pause playback\n"
                "  i - decode only I frames\n"
                "  ip - decode only I & P frames\n"
                "  all - decode all frames\n"
                );
                printf(
                "  fa - frame advance\n"
                "  fr - frame reverse\n"
                "  rate(rate) - set trick rate (floating point number, 1.0 is normal play)\n"
                "  host(type[,modifier,slowrate]) - set host trick mode\n"
                "    type=i,ip,all; modifier=1,-1 for forward,reverse(i only); slowrate=decoder repeat rate (1=1x,2=2x)\n"
                );
                printf(
                "  seek(pos) - jump to position (in milliseconds)\n"
                "  sleep(msec) - sleep for given number of milli-seconds\n"
                "  st - print status\n"
                "  af - audio flush\n"
                "  restart  - stop playback and restart play from the current position\n"
                );
/*TODO* figure out what these do and add appropriate explanation*/
#if 0
                if (customFile) printf(
                "  error_index(error,nodata,partial_read) - get \n"
                "  error_data(error,nodata,partial_read) - set \n"
                );
#endif
                printf(
                "  q - quit\n"
                );
            }
            else if (!strcmp(buf, "q") || !strcmp(buf, "quit")) {
                exit = true;
                break;
            }
            else if (!strcmp(buf, "play")) {
                rc = NEXUS_Playback_Play(playback);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "fa")) {
                rc = NEXUS_Playback_FrameAdvance(playback, true);
                /* if paused, it will fail */
                BDBG_ASSERT(rc==BERR_NOT_SUPPORTED || rc == BERR_SUCCESS);
            }
            else if (!strcmp(buf, "fr")) {
                rc = NEXUS_Playback_FrameAdvance(playback, false);
                /* if paused, it will fail */
                BDBG_ASSERT(rc==BERR_NOT_SUPPORTED || rc == BERR_SUCCESS);
            }
            else if (!strcmp(buf, "pause")) {
                rc = NEXUS_Playback_Pause(playback);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "i")) {
                NEXUS_PlaybackTrickModeSettings trickSettings;
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayI;
                trickSettings.skipControl = NEXUS_PlaybackSkipControl_eDecoder;
                trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                rc = NEXUS_Playback_TrickMode(playback, &trickSettings);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "ip")) {
                NEXUS_PlaybackTrickModeSettings trickSettings;
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayIP;
                trickSettings.skipControl = NEXUS_PlaybackSkipControl_eDecoder;
                trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                rc = NEXUS_Playback_TrickMode(playback, &trickSettings);
                BDBG_ASSERT(!rc);
            }
            else if (!strcmp(buf, "all")) {
                NEXUS_PlaybackTrickModeSettings trickSettings;
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
                NEXUS_PlaybackTrickModeSettings trickSettings;
                float rate;

                sscanf(buf+5, "%f", &rate);
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.rate = NEXUS_NORMAL_DECODE_RATE * rate;
                if (!trickSettings.rate && rate) {
                    trickSettings.rate = rate>0?1:-1;
                }
                if(opts.maxDecoderRate) {
                    trickSettings.maxDecoderRate = opts.maxDecoderRate;
                }
                rc = NEXUS_Playback_TrickMode(playback, &trickSettings);
                /* don't assert for fast or reverse trick modes */
                if (trickSettings.rate >=0 && trickSettings.rate <= NEXUS_NORMAL_DECODE_RATE) {
                    BDBG_ASSERT(!rc);
                }
            }
            else if (strstr(buf, "host(") == buf) {
                char *s;
                char trick[64];
                int n, modifier, decoder_slowrate;
                NEXUS_PlaybackTrickModeSettings trickSettings;

                /* convert , and ) into spaces for sscanf */
                while ((s = strpbrk(buf, ",)"))) {
                    *s = ' ';
                }
                
                n = sscanf(buf+5, "%s %d %d", trick, &modifier, &decoder_slowrate);
                if (n < 2 || modifier == 0) modifier = 1;
                if (n < 3 || decoder_slowrate == 0) decoder_slowrate = 1;
                
                BDBG_MSG(("host(%s,%d,%d)", trick, modifier, decoder_slowrate));
                
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.skipControl = NEXUS_PlaybackSkipControl_eHost;
                trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                trickSettings.mode_modifier = modifier;
                if (!strcasecmp(trick, "ip")) {
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayIP;
                }
                else if (!strcasecmp(trick, "i")) {
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayI;
                }
                else if (!strcasecmp(trick, "all")) {
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_eNormal;
                }
                else if (!strcasecmp(trick, "gop")) {
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayGop;
                }
                else if (!strcasecmp(trick, "bcm")) {
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayBrcm;
                }
                else {
                    BDBG_WRN(("unknown trick mode: %s", trick));
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_eNone;
                }
                trickSettings.rate = NEXUS_NORMAL_DECODE_RATE / decoder_slowrate;
                if (decoder_slowrate && !trickSettings.rate) {
                    trickSettings.rate = 1;
                }
                NEXUS_Playback_TrickMode(playback, &trickSettings);
            }
            else if (strstr(buf, "seek(") == buf) {
                unsigned pos;
                unsigned min,sec,msec;
                NEXUS_PlaybackStatus status;

                if(sscanf(buf+5,"%u:%u.%u", &min, &sec, &msec)==3) {
                    pos = (min*60+sec)*1000+msec;
                } if(sscanf(buf+5,"%u:%u", &min, &sec)==2) {
                    pos = (min*60+sec)*1000;
                } else {
                    sscanf(buf+5, "%u", &pos);
                }
                
                rc = NEXUS_Playback_GetStatus(playback, &status);
                if (!rc) {
                    if (pos < status.first) {
                        pos = status.first;
                    }
                    else if (pos > status.last) {
                        pos = status.last;
                    }
                }
                
                rc = NEXUS_Playback_Seek(playback, pos);
                BDBG_ASSERT(!rc); /* because we've bounded it, it should not fail */
            }
            else if (!strcmp(buf, "st")) {
                NEXUS_VideoDecoderStatus vstatus;
                NEXUS_AudioDecoderStatus astatus;
                NEXUS_AudioDecoderStatus acstatus;
                NEXUS_PlaybackStatus pstatus;
                NEXUS_PlaypumpStatus pumpstatus;
                uint32_t stc;

                if(opts.common.decodedVideo) {
                    rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &vstatus);
                    BDBG_ASSERT(!rc);
                }
                if(opts.common.decodedAudio) {
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

                if (opts.common.decodedVideo && opts.common.videoPid) {
                    char enhancementInfo[64];
                    NEXUS_VideoDecoder3DTVStatus s3DTVStatus;
                    *enhancementInfo='\0';
                    if(vstatus.enhancementFifoSize) {
                        snprintf(enhancementInfo, sizeof(enhancementInfo),  " [%u/%u (%u%%)]",
                                vstatus.enhancementFifoDepth, vstatus.enhancementFifoSize,
                                vstatus.enhancementFifoDepth * 100 / vstatus.enhancementFifoSize);
                    }

                    printf("video %u/%u (%u%%)%s pts=%#x, stc=%#x (diff %d) fps=%sHz, queueDepth=%d\n", vstatus.fifoDepth, vstatus.fifoSize,
                        vstatus.fifoSize ? vstatus.fifoDepth * 100 / vstatus.fifoSize : 0,
                        enhancementInfo,
                        vstatus.pts, stc, vstatus.ptsStcDifference,
                        g_videoFrameRateStrs[vstatus.frameRate].name,
                        vstatus.queueDepth);
                    if (vstatus.avdStatusBlock) {
                        printf("  avdStatusBlock=%#x\n", vstatus.avdStatusBlock);
                    }
                    
                    rc = NEXUS_VideoDecoder_Get3DTVStatus(videoDecoder, &s3DTVStatus);
                    BDBG_ASSERT(!rc);
                    if (s3DTVStatus.format != NEXUS_VideoDecoder3DTVFormat_eNone) {
                        static const char *s_3dtvFormat[NEXUS_VideoDecoder3DTVFormat_eMax] = {"eNone","eSideBySideHalf","eTopAndBottomHalf","eFramePacking"};
                        switch (s3DTVStatus.codec) {
                        case NEXUS_VideoCodec_eMpeg2:    
                            if (s3DTVStatus.codecData.mpeg2.valid) {
                                printf("3dtv %s MPEG: stereoVideoFormatSignalingType %x\n", 
                                    s_3dtvFormat[s3DTVStatus.format],
                                    s3DTVStatus.codecData.mpeg2.jp3d.stereoVideoFormatSignalingType);
                                break;
                            }
                            /* fall through */
                        case NEXUS_VideoCodec_eH264:
                            if (s3DTVStatus.codecData.avc.valid) {
                                printf("3dtv %s AVC: %08x %d %d %d %d,%d %d,%d %d %d\n", 
                                    s_3dtvFormat[s3DTVStatus.format],
                                    s3DTVStatus.codecData.avc.data.flags,
                                    s3DTVStatus.codecData.avc.data.framePackingArrangementId,
                                    s3DTVStatus.codecData.avc.data.framePackingArrangementType,
                                    s3DTVStatus.codecData.avc.data.contentInterpretationType,
                                    s3DTVStatus.codecData.avc.data.frame0GridPositionX,
                                    s3DTVStatus.codecData.avc.data.frame0GridPositionY,
                                    s3DTVStatus.codecData.avc.data.frame1GridPositionX,
                                    s3DTVStatus.codecData.avc.data.frame1GridPositionY,
                                    s3DTVStatus.codecData.avc.data.framePackingArrangementReservedByte,
                                    s3DTVStatus.codecData.avc.data.framePackingArrangementRepetitionPeriod);
                                break;
                            }
                            /* fall through */
                        default: 
                            printf("3dtv %s: no valid metadata\n", s_3dtvFormat[s3DTVStatus.format]);
                            break;
                        }
                    }
                    /* else { printf("no 3dtv metadata\n"); } */
                }
                if (opts.common.audioPid) {
                    if(opts.common.decodedAudio) {
                        printf("audio %u/%u (%u%%) pts=%#x, stc=%#x (diff %d), queuedFrames=%d\n", astatus.fifoDepth, astatus.fifoSize,
                            astatus.fifoSize ? astatus.fifoDepth * 100 / astatus.fifoSize : 0,
                            astatus.pts, stc, astatus.ptsStcDifference,
                            astatus.queuedFrames);
                    }
                    if(compressedDecoder) {
                        printf("compressed %u/%u (%u%%) pts=%#x, stc=%#x (diff %d), queuedFrames=%d\n", acstatus.fifoDepth, acstatus.fifoSize,
                            acstatus.fifoSize ? acstatus.fifoDepth * 100 / acstatus.fifoSize : 0,
                            acstatus.pts, stc, acstatus.ptsStcDifference,
                            astatus.queuedFrames);
                    }
                }
                printf("playback %u/%u (%u%%) pos=%u:%02u.%03u(%d:%d) last=%u:%02u.%03u\n", pstatus.fifoDepth, pstatus.fifoSize,
                    pstatus.fifoSize ? pstatus.fifoDepth * 100 / pstatus.fifoSize : 0,
                    (unsigned)pstatus.position/60000,
                    (unsigned)(pstatus.position%60000)/1000,
                    (unsigned)pstatus.position%1000,
                    (int)(pstatus.readPosition - pstatus.position),
                    (int)(pumpstatus.mediaPts - (opts.common.videoPid?vstatus.pts:astatus.pts))/45,
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
            else if (!strcmp(buf, "restart")) {
                NEXUS_PlaybackStatus status;
                NEXUS_VideoDecoderSettings videoDecoderSettings;

                /* get current playback position */
                rc = NEXUS_Playback_GetStatus(playback, &status);
                BDBG_ASSERT(!rc);
                NEXUS_Playback_Stop(playback);

                if (opts.common.videoPid) {
                    /* don't show black frame */
                    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
                    videoDecoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilTsmLock;
                    rc=NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
                    BDBG_ASSERT(!rc);
                }
                /* stop decoder */
                stop_video(&opts, videoDecoder);
                stop_audio(&opts, audioDecoder, compressedDecoder);

                NEXUS_Playback_GetSettings(playback, &playbackSettings);
                playbackSettings.startPaused = true;
                rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
                BDBG_ASSERT(!rc);

                /* Start decoders */
                start_video(&opts, videoDecoder, &videoProgram);
                start_audio(&opts, audioDecoder, compressedDecoder, &audioProgram);


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
            else if (!*buf) {
                /* allow blank line */
            }
            else {
                printf("unknown command: '%s' (use '?' for list)\n", buf);
            }
        }
        while ((buf = strtok(NULL, ";")));

#if B_HAS_PLAYBACK_MONITOR
        if(stickyFile) {
            BKNI_ReleaseMutex(monitorState.lock);
        }
#endif
    }

#if B_HAS_PLAYBACK_MONITOR
    if(stickyFile) {
        monitor_thread_stop(&monitorState);
    }
    }
#endif
    NEXUS_Playback_Stop(playback);
    stop_video(&opts, videoDecoder);
    stop_audio(&opts, audioDecoder, compressedDecoder);

    NEXUS_Playback_CloseAllPidChannels(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display); /* must precede NEXUS_VideoDecoder_Close to auto-shutdown of inputs */
    NEXUS_VideoDecoder_Close(videoDecoder);
    if (framebuffer) {
        NEXUS_Surface_Destroy(framebuffer);
    }
    NEXUS_AudioDecoder_Close(audioDecoder);
    if(compressedDecoder) {
        NEXUS_AudioDecoder_Close(compressedDecoder);
    }
    NEXUS_Platform_Uninit();

    return 0;
}

static void print_cmp_crc(NEXUS_DisplayHandle display)
{
    /* loop forever printing CMP CRC's */
    for (;;) {
        NEXUS_DisplayCrcData data[16];
        unsigned num, i;
        int rc;

        rc = NEXUS_Display_GetCrcData(display, data, 16, &num);
        BDBG_ASSERT(!rc);
        if (!num) {
            BKNI_Sleep(10);
            continue;
        }

        for (i=0;i<num;i++) {
            printf("CMP CRC %x %x %x\n", data[i].cmp.luma, data[i].cmp.cb, data[i].cmp.cr);
        }
    }
}

static void print_avd_crc(NEXUS_VideoDecoderHandle videoDecoder)
{
    /* loop forever printing AVD CRC's */
    for (;;) {
        NEXUS_VideoDecoderCrc data[16];
        unsigned num, i;
        int rc;

        rc = NEXUS_VideoDecoder_GetCrcData(videoDecoder, data, 16, &num);
        BDBG_ASSERT(!rc);
        if (!num) {
            BKNI_Sleep(10);
            continue;
        }

        for (i=0;i<num;i++) {
            printf("AVD CRC %x %x %x; %x %x %x\n", data[i].top.luma, data[i].top.cr, data[i].top.cb, data[i].bottom.luma, data[i].bottom.cr, data[i].bottom.cb);
        }
    }
}

static void print_hdmi_crc(NEXUS_HdmiOutputHandle hdmiOutput)
{
    /* loop forever printing HDMI Tx CRC's */
    for (;;) {
        NEXUS_HdmiOutputCrcData data[16];
        unsigned num, i;
        int rc;

        rc = NEXUS_HdmiOutput_GetCrcData(hdmiOutput, data, 16, &num);
        BDBG_ASSERT(!rc);
        if (!num) {
            BKNI_Sleep(10);
            continue;
        }

        for (i=0;i<num;i++) {
            printf("HDMI CRC %x\n", data[i].crc);
        }
    }
}

#endif /*!NEXUS_HAS_PLAYBACK*/

/*
************************************************

examples / test cases

# basic decode of streamer input
nexus playback -video 0x31 -audio 0x34 -video_type mpeg -audio_type ac3 /mnt/hd/italyriviera_spiderman2_cc_q64.mpg

# playback using media probe
nexus playback /mnt/hd/cnnticker.mpg /mnt/hd/cnnticker.nav

************************************************
*/

