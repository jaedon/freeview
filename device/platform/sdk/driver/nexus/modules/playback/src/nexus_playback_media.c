/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_playback_media.c $
 * $brcm_Revision: 93 $
 * $brcm_Date: 10/12/12 3:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/playback/src/nexus_playback_media.c $
 * 
 * 93   10/12/12 3:00p vsilyaev
 * SW7231-1023: Fixed typo
 * 
 * 92   10/12/12 2:17p vsilyaev
 * SW7231-1023: Use parsingErrorCallback for 'soft' (recoverable) errors
 * 
 * 91   10/3/12 1:21p ahulse
 * SW7425-3903,SW7550-739: mark inserted packets as non encrypted
 * 
 * 90   4/13/12 12:47p vsilyaev
 * SW7425-2378: Fixed handling of APE container
 * 
 * 89   4/12/12 4:00p jgarrett
 * SW7425-2378: Adding Monkey's Audio and MLP
 * 
 * 88   2/7/12 7:34p jgarrett
 * SW7425-2136: Adding FLAC support
 * 
 * 87   12/27/11 3:33p vsilyaev
 * SW7420-2198: Verify index type before doing accurate seek
 * 
 * 86   10/25/11 11:36a vsilyaev
 * SWDTV-8388: Added NEXUS_TransportType_eOgg type
 * 
 * 85   8/31/11 2:44p vsilyaev
 * SW7420-1978: Add selection of index type for playback of MPEG-2 TS
 *  files
 * 
 * 84   8/12/11 9:40a erickson
 * SW7358-83: add NEXUS_PlaybackTrickModeSettings.brcmTrickMode
 * 
 * 83   6/17/11 4:55p vsilyaev
 * SWDTV-7046: Properly set PES ID for audio only MP4/MKV streams
 * 
 * 82   5/26/11 7:32p vsilyaev
 * SW7425-646: Added configurable 'gap' for timeshifting
 * 
 * 81   5/12/11 3:43p jtna
 * SW7550-739: replace all instances of 'NEXUS_HAS_DMA &&
 *  NEXUS_HAS_SECURITY' with 'NEXUS_ENCRYPTED_DVR_WITH_M2M'. replace some
 *  instances of 'NEXUS_HAS_DMA' with 'NEXUS_NUM_DMA_CHANNELS'
 * 
 * 80   4/22/11 12:08p vsilyaev
 * SW7422-14: Fixed detection of the 'master' video
 * 
 * 79   4/19/11 5:28p vsilyaev
 * SW7422-14: Differentiate extra audio/video and 'other' tracks
 * 
 * 78   3/23/11 3:00p vsilyaev
 * SW7422-14: Added facility to associate  stream id(pid) with a chunk of
 *  payload
 * 
 * 77   2/28/11 9:48a erickson
 * SW7125-818: add #include to fix warning
 *
 * 76   2/25/11 12:47p erickson
 * SW7125-818: use BNAV_GetEntrySize
 *
 * 75   2/24/11 1:36p vsilyaev
 * SWDTV-5485: Differentiates ES and ES->PES types
 *
 * 74   2/22/11 7:53p vsilyaev
 * SW7422-107: Added FLV mapping
 *
 * 73   2/15/11 6:25p dlwin
 * SW7405-5140: Fixed ES stream playback issue with Index file. Playback
 *  of ES stream did not work with Index file.
 *
 * 72   10/22/10 3:21p vsilyaev
 * SW35230-1600: Don't dereference non-existing structure members
 *
 * 71   10/22/10 11:48a vsilyaev
 * SW35230-1600: Fixed typo
 *
 * 70   10/22/10 11:46a vsilyaev
 * SW35230-1600: When decrypting payload clear file buffer on transitions.
 *
 * 69   9/24/10 1:55p vsilyaev
 * SWGERRARD-557: Properly handle case when MP3 playback supplied with
 *  self-indexing scheme
 *
 * 68   9/20/10 11:37a gmohile
 * SW7325-806 : Increase nest timer timeout to 10ms
 *
 * 62   7/23/10 2:13p vsilyaev
 * SW7405-4654: Fixed resource cleaning on error during
 *  NEXUS_PlaybackStart
 *
 * 61   7/20/10 5:24p gmohile
 * SW3548-2994 : Add MKV DRM support
 *
 * 60   7/19/10 4:24p gmohile
 * SW3548-2994 : Add support for encrypted mkv tracks
 *
 * 59   5/6/10 3:45p vsilyaev
 * SW7405-3773: Added support for demuxing fragments from fragmented MP4
 *  container
 *
 * 58   4/14/10 5:48p jtna
 * SW7125-318: Coverity Defect ID:20563 USE_AFTER_FREE
 *
 * 57   3/26/10 1:59p vsilyaev
 * SW7550-313: Flush video decoder on loop, this needed to prevent using
 *  stale PTS when crossing through file boundary in frame reverse mode
 *
 * 56   3/11/10 5:26p vsilyaev
 * SW3556-913: Always use header file to declare external functions
 *
 * 55   3/5/10 7:13p jtna
 * SW3556-913: support for ErrorHandlingMode_eAbort
 *
 * 54   3/4/10 11:44a vsilyaev
 * SW3556-913: Demoted EOS debug message
 *
 * 53   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 *
 * 52   2/11/10 2:54p erickson
 * SW7550-252: fix dma/security tests
 *
 * 51   8/20/09 10:15a gmohile
 * PR 55991 : Add PES/VOB trickmode
 *
 * 50   8/19/09 3:29p erickson
 * PR57840: start timer during eWaitingRecord state so that endOfStream
 *  callback can be fired
 *
 * 49   7/31/09 4:24p erickson
 * PR57235: cleanup comments
 *
 * 48   7/29/09 2:05p mward
 * PR57192: Reduce stack use in b_play_mpeg2ts_probe_index.
 *
 * 47   7/15/09 7:24p vsilyaev
 * PR 55653: Added WAV format
 *
 * 46   7/9/09 11:34a vsilyaev
 * PR 55989: Added autodetection between MPEG2-TS stream and NAV index
 *
 * 45   7/8/09 3:25p vsilyaev
 * PR 55989: Used size of the video decoder CDB to control size of the
 *  data chunk
 *
 * 44   7/8/09 11:59a vsilyaev
 * PR 55989: Added support for OTF trickmodes
 *
 * 43   6/22/09 4:09p vsilyaev
 * PR 55554: PR 55417: Fixed handling of bmedia_player_entry_type_error
 *
 * 42   6/8/09 7:09p vsilyaev
 * PR 55554: Fixed handling of huge frames in MP4 stream
 *
 * 41   6/4/09 5:11p erickson
 * PR54129: remove SEEK_CUR which results in loss of packet alignment when
 *  a file fifo is trimmed. consolidate logic in
 *  NEXUS_Playback_P_EndOfDataFile.
 *
 * 40   6/2/09 7:38p vsilyaev
 * PR 55417: Added support for read method returning no data or completing
 *  with a partial read
 *
 * 39   5/19/09 3:02p vsilyaev
 * PR 55299: Fixed PES PID mapping for the case of multiple audio tracks
 *
 * 36   3/17/09 5:41p vsilyaev
 * PR 46190: Adding mappings to the PCM audio codec
 *
 * 35   3/10/09 4:44p erickson
 * PR52946: move fix to the right spot
 *
 * 34   3/10/09 2:57p vsilyaev
 * PR 52946: Pass proper packet size for the transport streams
 *
 * 33   3/9/09 12:28p mphillip
 * PR52786: Add b_play_media_send_eos to send
 *  NEXUS_Playpump_ReadComplete(0,0) to signal end of data
 *
 * 32   3/6/09 10:27a erickson
 * PR52857: add missing breaks
 *
 * 31   3/6/09 9:44a erickson
 * PR52854: fix warnings
 *
 * 30   3/5/09 5:46p vsilyaev
 * PR 52579: Updated circular FIFO/timeshifting code
 *
 * 29   3/2/09 5:09p vsilyaev
 * PR 52434: Added handling of bmedia_player_entry_type_noop
 *
 * 28   2/4/09 1:10p erickson
 * PR51841: NEXUS_Playback_Start should fail for MKV or MP4 files without
 *  an index
 *
 * 27   1/26/09 11:38a vsilyaev
 * PR 51579: Added ES to PES packetization and support for capturing of
 *  streams produced by the playback module
 *
 * 26   1/15/09 2:28p erickson
 * PR50763: remove non-error ERR
 *
 * 25   1/12/09 5:44p vsilyaev
 * PR 50763: Improved seek to position 0 after rewind reached begining of
 *  the file
 *
 * 24   12/21/08 8:31p nickh
 * PR50605: Fix compile errors when running without Security
 *
 * 23   12/19/08 5:59p vsilyaev
 * PR 50214: Added callbacks and counters for parsing index files
 *
 * 22   12/10/08 2:57p vsilyaev
 * PR 48760: Removed duplicated information
 *
 * 21   12/10/08 12:49p vsilyaev
 * PR 49718: Fixed error reporting/handling from MKV player
 *
 * 20   12/10/08 2:06a vsilyaev
 * PR 48760: Using ES player for accurate progress monitor of MP3 streams
 *
 * 19   12/8/08 11:20a erickson
 * PR49930: implement NEXUS_PlaybackSettings.accurateSeek
 *
 * 18   12/2/08 2:23p jgarrett
 * PR 47993: Adding VOB format
 *
 * 17   9/12/08 1:16p vsilyaev
 * PR 46832: Verify data that comes from the media_player
 *
 * 16   9/3/08 4:55p vsilyaev
 * PR 45454: Don't use auto-index playback in default configuration
 *
 * 15   8/29/08 5:48p vsilyaev
 * PR 33812: Added support for navigation in the MPEG-2 TS file without an
 *  index
 *
 * 14   7/24/08 3:43p vishk
 * PR 45134: Playback of a recorded stream loops continuously at the end
 *  of the stream.
 *
 * 13   7/17/08 11:24a vsilyaev
 * PR 44857: strenghtened test for bcmplayer generated packets
 *
 * 12   7/3/08 1:07p vsilyaev
 * PR 44381: Added code to limi nestendess of recursive calls
 *
 * 11   6/25/08 7:10p vsilyaev
 * PR 41869: Added markings of BTP packet
 *
 * 10   6/24/08 11:50a vsilyaev
 * PR 38347: Use separate timestamps for audio and video streams
 *
 * 9   6/12/08 3:02p erickson
 * PR43606: rename NEXUS_PlaybackStreamSettings to
 *  NEXUS_PlaybackStartSettings
 *
 * 8   6/4/08 10:18a vsilyaev
 * PR 43184: Don't detect EOS in the timeshifting mode
 *
 * 7   5/16/08 11:50a erickson
 * PR40832: avoid bcmplayer warning for no video pid
 *
 * 6   4/10/08 5:07p erickson
 * PR40832: add WRN if timeshifting not true in certain context
 *
 * 5   4/3/08 2:02p vsilyaev
 * PR 39818: Optimized MKV handling
 *
 * 4   3/8/08 7:45a erickson
 * PR40103: convert to NEXUS_TaskCallback
 *
 * 3   3/3/08 3:36p vsilyaev
 * PR 39818: Support of MKV format
 *
 * 2   2/26/08 10:04a erickson
 * PR38810: fix LoopMode ePlay
 *
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/15   1/10/08 2:49p erickson
 * PR36068: 3563 has no brcm trick mode support
 *
 * Nexus_Devel/14   1/8/08 2:23p vsilyaev
 * PR 35824: Fixed resync on transition between modes
 *
 * Nexus_Devel/13   1/7/08 5:09p erickson
 * PR35824: add new continuous record support
 *
 * Nexus_Devel/12   12/21/07 5:55p vsilyaev
 * PR 38073: Fixed fa -> fb -> play transitions
 *
 * Nexus_Devel/11   12/20/07 5:24p vsilyaev
 * PR 38073: Improved handling of trickmodes
 *
 * Nexus_Devel/10   12/20/07 1:24p vsilyaev
 * PR 38073: Fixed handling of loop
 *
 * Nexus_Devel/PR38073/1   12/19/07 10:39a vsilyaev
 * PR 38073: Modifed code for exlusive use of media player
 *
 * Nexus_Devel/8   12/18/07 6:45p vsilyaev
 * PR 38073: Added handling of no-index streams
 *
 * Nexus_Devel/7   12/18/07 4:57p vsilyaev
 * PR 38073: Updated playback module to work exclusively with media player
 *
 * Nexus_Devel/6   12/7/07 11:55a vsilyaev
 * PR 37574: Improved handling media formats
 *
 * Nexus_Devel/5   12/6/07 5:24p vsilyaev
 * PR 37574: Added handling of segmented feed
 *
 * Nexus_Devel/4   11/1/07 9:41a erickson
 * PR36633: base enum changes
 *
 * Nexus_Devel/3   10/30/07 5:05p vsilyaev
 * PR 36404: Added audio tracking for slow motion modes
 *
 * Nexus_Devel/2   10/16/07 4:59p vsilyaev
 * PR 35824: Rearranged code
 *
 * $copied_brcm_Log: /BSEAV/api/src/pvr/bsettop_playback.c $
 * $copied_brcm_Revision: 172 $
 * $copied_brcm_Date: 10/1/07 11:13a $
 **************************************************************************/
#include "nexus_playback_module.h"
#include "nexus_playback_impl.h"
#include "biobits.h"
#include "nexus_platform_features.h"

#if B_HAS_DIVX_DRM
#include "bdrm_decrypt.h"
#endif

BDBG_MODULE(nexus_playback_media);

#define BDBG_MSG_FLOW(X) /* BDBG_MSG(X) */

static void
b_play_media_send_eos(NEXUS_PlaybackHandle p);

static void
b_play_media_nest_timer(void *playback)
{
    NEXUS_PlaybackHandle p = playback;
    BDBG_ASSERT(p->state.media.nest_count==0);
    p->state.media.nest_timer = NULL;
    if (b_play_control(p, eControlDataIO)) {
        p->state.io_size = B_MEDIA_NEST_MAGIC;
        BDBG_MSG(("b_play_media_nest_timer: %#lx interrupted",(unsigned long)p));
        return;
    }
    BDBG_MSG(("b_play_media_nest_timer: %#lx continue",(unsigned long)p));
    b_play_next_frame(p);
    return;
}

static void
b_play_handle_player_error(NEXUS_PlaybackHandle p, bool endofstream /* true of endofstream, false if file error */)
{

    BDBG_MSG(("b_play_handle_player_error: %#lx %s", (unsigned long)p, endofstream?"End Of Stream":"File Error"));
    if(!endofstream) {
        BDBG_MSG(("b_play_handle_player_error: %#lx forcing %s", (unsigned long)p, (p->params.playErrorHandling==NEXUS_PlaybackErrorHandlingMode_eAbort)?"abort":"EOF"));

        p->state.index_error_cnt++;
        NEXUS_TaskCallback_Fire(p->errorCallback);

        if (p->params.playErrorHandling == NEXUS_PlaybackErrorHandlingMode_eAbort) {
            NEXUS_Playback_P_AbortPlayback(p);
            return;
        }
    }
    /* If we are doing normal play speed or slower, then we should wait.
    This code won't work for a decode skip mode + slow motion which is slower than normal play, but that's unlikely. */
#if defined(HUMAX_PLATFORM_BASE)
    /* Allow wating more data in timeshift forward trick mode */
    if (p->params.timeshifting && p->state.trickmode_params.rate > 0) {
#else
    if (p->params.timeshifting && (p->state.trickmode_params.rate > 0 && p->state.trickmode_params.rate <= NEXUS_NORMAL_PLAY_SPEED)) {
#endif
        bmedia_player_pos pos;
        bmedia_player_status status;

        b_play_update_location(p);
        bmedia_player_tell(p->media_player, &pos);
        bmedia_player_get_status(p->media_player, &status);

        /* only go into eWaitingRecord state if we are certain about the current decoder postion and we know
        we can reliably start waiting. */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_CI)
        if ((p->params.playpumpSettings.allPass == false && p->state.validPts && pos >= status.bounds.first)
			||(p->params.playpumpSettings.allPass != false && pos >= status.bounds.first)) {
#else
        if (p->state.validPts && pos >= status.bounds.first) {
#endif
            BDBG_MSG(("index->wait_for_record"));
            p->state.read_size = 0; /* this is used as a flag that we are waiting for index */
            p->state.state = eWaitingRecord;
            if (!p->state.timer) {
                p->state.timer = NEXUS_ScheduleTimer(B_FRAME_DISPLAY_TIME * 5, b_play_timer, p);
            }
            return;
        }
    }
    BDBG_MSG(("b_play_handle_player_error-> wait_for_drain"));
    b_play_media_send_eos(p);
    return;
}

static void
b_play_media_next_frame(NEXUS_PlaybackHandle p)
{
    int rc;
    off_t offset;

    BDBG_ASSERT(p->media_player);
    if (b_play_control(p, eControlFrame)) {
        return ;
    }
    if(p->state.media.nest_count>4) {
        if(p->state.media.nest_timer==NULL) {
            /* schedule a call-out and unwind a stack */
            p->state.media.nest_timer = NEXUS_ScheduleTimer(10, b_play_media_nest_timer, p);
            BDBG_MSG(("b_play_media_next_frame: %#lx nest:%u schedule timer %#lx",(unsigned long)p, p->state.media.nest_count, (unsigned long)p->state.media.nest_timer));
            if(p->state.media.nest_timer) {
                p->state.state = eWaitingIo; /* Reuse eWaitingIo state */
                return;
            }
        } else {
            BDBG_WRN(("b_play_media_next_frame: %#lx detected fork in control flow",(unsigned long)p));
        }
    }
    p->state.media.nest_count++;
    rc = bmedia_player_next(p->media_player, &p->state.media.entry);
    if (rc!=0) {
        b_play_handle_player_error(p, false);
        goto done;
    }
    if(p->state.media.entry.type == bmedia_player_entry_type_async) {
        p->state.state = eWaitingIo;
        goto done;
    }

    if(p->state.media.entry.type==bmedia_player_entry_type_file) {
       off_t seek_result;
       offset = p->state.media.entry.start;
       p->state.frame.offset = offset;
#ifdef DIRECT_IO_SUPPORT
        p->state.io.file_behind = offset - B_IO_ALIGN_TRUNC(offset);
        p->state.io.next_data = 0;
        BDBG_MSG_FLOW((">>>>>> seek to %lld (aligned=%lld + pre=%d)",
            (uint64_t)offset,
            (uint64_t)B_IO_ALIGN_TRUNC(offset), p->state.io.file_behind));
        offset = B_IO_ALIGN_TRUNC(offset);
#endif
        p->state.io.last_file_offset = offset;
        seek_result = p->file->file.data->seek(p->file->file.data, offset, SEEK_SET);
        if (offset != seek_result) {
            BDBG_ERR(("seek wasn't able to find mpeg data"));
            BDBG_MSG(("b_play_next_media_frame -> wait_for_drain"));
            b_play_media_send_eos(p);
            goto done;
        }
    }

    p->state.frame.cur = 0;
    p->state.frame.size = p->state.media.entry.length;
    BDBG_ASSERT((int)p->state.frame.size  >=0);
    b_play_media_send_meta(p);
done:
    p->state.media.nest_count--;
    return;
}

void
bplay_p_clear_buffer(NEXUS_PlaybackHandle p)
{
    BSTD_UNUSED(p);
#if NEXUS_HAS_SECURITY
    if(p->params.playpumpSettings.securityContext &&  p->state.media.buffer) {
        /* if there is decryption [in place] and file buffer used, it should be cleared on transition to prevent double decryption */
        bfile_buffer_clear(p->state.media.buffer);
    }
#endif
    return;
}


NEXUS_Error
b_play_media_handle_loop_condition(NEXUS_PlaybackHandle p, bool is_beginning, NEXUS_PlaybackLoopMode loopmode, const bmedia_player_bounds *bounds)
{
    int rc;
    int pos;

    BDBG_ASSERT(p->media_player);
    switch(loopmode) {
    case NEXUS_PlaybackLoopMode_eLoop:
        if (is_beginning) {
            BDBG_MSG(("loop to end (index %ld)", bounds->last));
            b_play_flush(p);
            rc = bmedia_player_seek(p->media_player, bounds->last);
            if (rc!=0) {
                BDBG_WRN(("b_play_media_handle_loop_condition: bmedia_player_seek failed %d", rc));
                rc = bmedia_player_seek(p->media_player, bounds->first/2 + bounds->last/2);
            }
        } else {
            BDBG_MSG(("loop to beginning"));
            b_play_flush(p);
            rc = bmedia_player_seek(p->media_player, bounds->first);
            if (rc!=0) {
                BDBG_WRN(("b_play_media_handle_loop_condition: bmedia_player_seek failed %d", rc));
                rc = bmedia_player_seek(p->media_player, bounds->first/2 + bounds->last/2);
            }
        }
        bplay_p_clear_buffer(p);
        break;
    case NEXUS_PlaybackLoopMode_ePlay:
        if (is_beginning) {
            /* if the beginning has been trimmed, we should gap so that we avoid getting chopped again by bfile_fifo. */
            pos = (bounds->first == 0) ? bounds->first : bounds->first + p->params.timeshiftingSettings.beginningOfStreamGap;
        } else {
            /* play at EOF requires a gap for record, otherwise, there's really nothing we can do. */
            if (!p->params.timeshifting) {
                BDBG_WRN(("endOfStreamAction == NEXUS_PlaybackLoopMode_ePlay without timeshifting has been converted to ePause"));
                bplay_p_pause(p);
                break;
            } else {
                /* The recommended endOfStream action for timeshifting is to switch to live decode. This can only be done
                by the application. This code provides a gap to reduce stuttering. */
                if (bounds->last >= p->params.timeshiftingSettings.endOfStreamGap) {
                    pos = bounds->last - p->params.timeshiftingSettings.endOfStreamGap;
                }
                else {
                    pos = 0;
                }
            }
        }
        BDBG_MSG(("seek pos %u in %u...%u", pos, bounds->first, bounds->last));

        bplay_p_clear_buffer(p);
        bplay_p_play_from(p, pos);
        break;
    default:
    case NEXUS_PlaybackLoopMode_ePause:
#if defined (HUMAX_PLATFORM_BASE)
		if(is_beginning) {
			b_play_flush(p);
			rc = bmedia_player_seek(p->media_player, bounds->first);
			if (rc!=0) {
				BDBG_WRN(("b_play_media_handle_loop_condition: bmedia_player_seek failed %d", rc));
				rc = bmedia_player_seek(p->media_player, bounds->first/2 + bounds->last/2);
			}
		}
#endif
        bplay_p_pause(p);
        break;
    }
    return NEXUS_SUCCESS;
}

/* this function called when it's time to feed new frame into the playback buffer,
it should be called without pending I/O */
void
b_play_next_frame(NEXUS_PlaybackHandle p)
{
    p->state.decoder_flushed = false; /* clear flushed state */
    b_play_media_next_frame(p);
    return;
}

static void
b_play_media_pts_timer(void *playback)
{
    NEXUS_PlaybackHandle p = playback;
    p->state.media.pts_timer = NEXUS_ScheduleTimer(BMEDIA_UPDATE_POSITION_INTERVAL, b_play_media_pts_timer, p);
    b_play_update_location(p);
    return;
}

void
b_play_media_time_test(void)
{
#if 0
    uint32_t time, pts;
    bmedia_dword delta;

    delta = bmedia_pts2time((uint32_t)(BMEDIA_PTS_MODULO-1), BMEDIA_TIME_SCALE_BASE);
    pts = bmedia_time2pts(delta, BMEDIA_TIME_SCALE_BASE);
    time = bmedia_pts2time(pts, BMEDIA_TIME_SCALE_BASE);
    BDBG_MSG(("modulo_test %u %u %u", delta, pts, time));
    BDBG_ASSERT(time == delta);

    pts = bmedia_time2pts(delta+100, BMEDIA_TIME_SCALE_BASE);
    time = bmedia_pts2time(pts, BMEDIA_TIME_SCALE_BASE);
    BDBG_MSG(("modulo_test %u %u %u", delta, pts, time));
    BDBG_ASSERT(time == 99);

    pts = bmedia_time2pts(delta, -BMEDIA_TIME_SCALE_BASE);
    time = bmedia_pts2time(pts, -BMEDIA_TIME_SCALE_BASE);
    BDBG_MSG(("modulo_test %u %u %u", delta, pts, time));
    BDBG_ASSERT(time == delta);

    pts = bmedia_time2pts(delta+100, -BMEDIA_TIME_SCALE_BASE);
    time = bmedia_pts2time(pts, -BMEDIA_TIME_SCALE_BASE);
    BDBG_MSG(("modulo_test %u %u %u", delta, pts, time));
    BDBG_ASSERT(time == 99);

    pts = bmedia_time2pts(1000, BMEDIA_TIME_SCALE_BASE);
    time = bmedia_pts2time(pts, BMEDIA_TIME_SCALE_BASE);
    BDBG_MSG(("time_test: %u %u", (unsigned)pts, (unsigned)time));
    BDBG_ASSERT(time == 1000);
    pts = bmedia_time2pts(1000, -BMEDIA_TIME_SCALE_BASE);
    time = bmedia_pts2time(pts, -BMEDIA_TIME_SCALE_BASE);
    BDBG_MSG(("time_test: %u %u", (unsigned)pts, (unsigned)time));
    BDBG_ASSERT(time == 1000);

    pts = bmedia_time2pts(337023, BMEDIA_TIME_SCALE_BASE);
    time = bmedia_pts2time(pts, BMEDIA_TIME_SCALE_BASE);
    BDBG_MSG(("time_test: %u %u", (unsigned)pts, (unsigned)time));
    BDBG_ASSERT(time == 337023);

    pts = bmedia_time2pts(337023, -BMEDIA_TIME_SCALE_BASE);
    time = bmedia_pts2time(pts, -BMEDIA_TIME_SCALE_BASE);
    BDBG_MSG(("time_test: %u %u", (unsigned)pts, (unsigned)time));
    BDBG_ASSERT(time == 337023);

    pts = bmedia_time2pts(2*delta+337023, BMEDIA_TIME_SCALE_BASE);
    time = bmedia_pts2time(pts, BMEDIA_TIME_SCALE_BASE);
    BDBG_MSG(("time_test: %u %u %u", (unsigned)pts, (unsigned)time, 337023+2*delta));
    BDBG_ASSERT(337023-time<10);

    pts = bmedia_time2pts(2*delta+337023, BMEDIA_TIME_SCALE_BASE);
    time = bmedia_pts2time(pts, BMEDIA_TIME_SCALE_BASE);
    BDBG_MSG(("time_test: %u %u %u", (unsigned)pts, (unsigned)time, 337023+2*delta));
    BDBG_ASSERT(337023-time<10);


    return;
#endif
}

static void
b_play_media_async_atom_ready(void *cntx, bmedia_player_entry *entry)
{
    NEXUS_PlaybackHandle p = cntx;

    p->state.frame.cur = 0;
    p->state.media.entry = *entry;
    p->state.frame.size = p->state.media.entry.length;
    BDBG_ASSERT((int)p->state.frame.size  >=0);
    if (b_play_control(p, eControlDataIO)) {
        p->state.io_size = B_MEDIA_ATOM_MAGIC;
        goto done;
    }

    if (p->state.state != eWaitingIo) {
        BDBG_WRN(("b_play_media_async_atom_ready got unexpected data"));
        goto done;
    }

    b_play_media_send_meta(p);

done:
    return;
}

/* this function is called when we want to send generated atom (scatteg/gather packet) */
static void
b_play_media_send_atom(NEXUS_PlaybackHandle p)
{
    NEXUS_Error rc;
    size_t size;
    void *vaddr;

    BDBG_ASSERT(p->state.media.entry.atom);

    BDBG_ASSERT(p->params.playpump);

    for(;;) {
        if (NEXUS_Playpump_GetBuffer(p->params.playpump, &vaddr, &size)!=NEXUS_SUCCESS || size == 0) {
            goto keep_waiting;
        }
        size = batom_cursor_copy(&p->state.media.cursor, vaddr, size);
        BDBG_MSG_FLOW(("sending %d out of %d bytes left in packet",
            size, p->state.packet.size-p->state.packet.cur));

        /* Can't be in eWaitingPlayback or we risk a deadlock from playpump callback. */
        p->state.state = eTransition;
        b_play_capture_write(p, vaddr, size);
        rc = NEXUS_Playpump_ReadComplete(p->params.playpump, 0, size);
        if (rc!=NEXUS_SUCCESS) {
            rc = BERR_TRACE(rc);
            BDBG_ERR(("b_play_media_send_atom NEXUS_Playpump_ReadComplete error %#x, playback aborted", (unsigned)rc));
            p->state.state = eAborted;
            NEXUS_TaskCallback_Fire(p->errorCallback);
            return;
        }

        p->state.packet.cur+=size;
        if (p->state.packet.cur >= p->state.packet.size) {
            batom_release(p->state.media.entry.atom);
            p->state.media.entry.atom = NULL;
            /* we're done sending the packet, so go get the next frame */
            b_play_next_frame(p);
            return;
        }
        /* keep writing data */
    }
    return;
keep_waiting:
    p->state.state = eWaitingPlayback;
    p->state.data_source = b_play_media_send_atom; /* schedule to call us when space is avaliable */
    return;
}



void
b_play_media_send_meta(NEXUS_PlaybackHandle p)
{
    NEXUS_Error rc;
    size_t size;
    void *vaddr;
    NEXUS_PlaypumpSegment segment;

    BDBG_ASSERT(p->media_player);
    BDBG_ASSERT(p->params.playpump);

    /* handle no data cases prior to sending segment */
    switch(p->state.media.entry.type) {
    case bmedia_player_entry_type_atom:
    case bmedia_player_entry_type_embedded:
    case bmedia_player_entry_type_file:
        if(p->state.media.entry.length!=0) {
            break;
        }
        /* keep going */
    case bmedia_player_entry_type_noop:
    case bmedia_player_entry_type_no_data:
        b_play_media_next_frame(p); /* keep on reading  */
        return;
    case bmedia_player_entry_type_end_of_stream:
        b_play_handle_player_error(p, true);
        return;
    case bmedia_player_entry_type_error:
        b_play_handle_player_error(p, false);
        return;
    case bmedia_player_entry_type_async:
        BDBG_ASSERT(0); /* keep going */
    }

    if(p->state.media.segmented) {
        for(;;) {
            if (NEXUS_Playpump_GetBuffer(p->params.playpump, &vaddr, &size)!=NEXUS_SUCCESS || size == 0) {
                goto keep_waiting;
            }
            if (vaddr > (void *)p->state.media.last_segment_hdr) {
                /* skip buffer */
                BDBG_MSG(("b_play_media_send_meta: skip %#lx %lu", (unsigned long)vaddr, (unsigned long)size));
                rc = NEXUS_Playpump_ReadComplete(p->params.playpump, size, 0);
                if (rc!=NEXUS_SUCCESS) {
                    BDBG_ERR(("b_play_media_send_meta: NEXUS_Playpump_ReadComplete error %#x, playback aborted", (unsigned)rc));
                    p->state.state = eAborted;
                    NEXUS_TaskCallback_Fire(p->errorCallback);
                    return;
                }
                continue;
            }
            if(size<sizeof(segment)) {
                goto keep_waiting;
            }
            BKNI_Memset(&segment, 0, sizeof(segment));
            segment.length = p->state.media.entry.length + sizeof(segment);
            segment.offset = p->state.media.entry.start;
            segment.timestamp = p->state.media.entry.timestamp;
            segment.signature = NEXUS_PLAYPUMP_SEGMENT_SIGNATURE;
            if(p->state.media.entry.entry){
                unsigned i;
                /* Video timestamp is always on slot 0*/
                segment.timestamp = p->state.media.entry.entry->resync[0].timestamp;
                segment.timestamp_delta[0].stream_id = p->state.media.entry.entry->resync[0].stream_id;
                /* extra  slots used to pass timestamp */
                for(i=1; i<sizeof(segment.timestamp_delta)/sizeof(segment.timestamp_delta[0]); i++){
                    segment.timestamp_delta[i].stream_id = p->state.media.entry.entry->resync[i].stream_id;
                    segment.timestamp_delta[i].timestamp_delta = p->state.media.entry.entry->resync[i].timestamp - segment.timestamp;
                }
            }

            BKNI_Memcpy(vaddr, &segment, sizeof(segment)); /* vaddr might be not alligned, therefore use temporary buffer */
            BDBG_MSG_FLOW(("segment at %#lx %#lx %lu(%lu)", (unsigned long)vaddr, (unsigned long)segment.signature, (unsigned long)segment.offset, (unsigned long)segment.length));
            /* Can't be in eWaitingPlayback or we risk a deadlock from playpump callback. */
            p->state.state = eTransition;
            rc = NEXUS_Playpump_ReadComplete(p->params.playpump, 0, sizeof(segment));
            if (rc!=NEXUS_SUCCESS) {
                BDBG_ERR(("b_play_media_send_meta: NEXUS_Playpump_ReadComplete error %#x, playback aborted", (unsigned)rc));
                p->state.state = eAborted;
                NEXUS_TaskCallback_Fire(p->errorCallback);
                return;
            }
            break;
        }
    }
    switch(p->state.media.entry.type) {
    case bmedia_player_entry_type_embedded:
        BDBG_ASSERT(p->state.media.entry.embedded);
        p->state.packet.size = p->state.media.entry.length;
        p->state.packet.cur = 0;
        p->state.packet.buf = p->state.media.entry.embedded;
/* Also see define in nexus/modules/transport/.. */
#if NEXUS_NUM_DMA_CHANNELS && NEXUS_HAS_SECURITY
#define NEXUS_ENCRYPTED_DVR_WITH_M2M 1
#endif
#if NEXUS_ENCRYPTED_DVR_WITH_M2M
        if( p->params.playpumpSettings.securityContext && p->params.playpumpSettings.transportType==NEXUS_TransportType_eTs && (p->state.packet.size==188 || p->state.packet.size == 192) ) {
            /* if playing back encrypted data, we need to mark inserted packets as encrypted and then playpump would see it,
             * bypass decryption, and then clear encryption marking. Life is weird. */
            uint8_t timestampoffset = p->state.packet.size==188?0:4;
            ((uint8_t *)p->state.packet.buf)[3 + timestampoffset] = B_GET_BITS(((uint8_t *)p->state.packet.buf)[3 + timestampoffset],5,0) | B_SET_BITS( transport_scrambling_control, 0x03, 7, 6); /* set adaptation field */
        }
#endif
        b_play_send_packet(p);
        break;
    case bmedia_player_entry_type_atom:
        BDBG_ASSERT(p->state.media.entry.atom);
        batom_cursor_from_atom(&p->state.media.cursor, p->state.media.entry.atom);
        p->state.packet.size = p->state.media.entry.length;
        p->state.packet.cur = 0;
        b_play_media_send_atom(p);
        break;
    case bmedia_player_entry_type_file:
        b_play_send_frame(p);
        break;

    /* should have handled previously */
    case bmedia_player_entry_type_end_of_stream:
    case bmedia_player_entry_type_no_data:
    case bmedia_player_entry_type_error:
    case bmedia_player_entry_type_async:
    case bmedia_player_entry_type_noop:
        BDBG_ASSERT(0);
        break;
    }
    return;
keep_waiting:
    p->state.state = eWaitingPlayback;
    p->state.data_source = b_play_media_send_meta; /* schedule to call us when space is avaliable */
    return;
}

static void
b_play_media_send_eos(NEXUS_PlaybackHandle p)
{
    NEXUS_Error rc;
    size_t size;
    void *vaddr;

    BDBG_ASSERT(p->media_player);
    BDBG_ASSERT(p->params.playpump);

    if(!p->state.media.segmented) {
        if (NEXUS_Playpump_GetBuffer(p->params.playpump, &vaddr, &size)!=NEXUS_SUCCESS || size == 0) {
            goto keep_waiting;
        }
        BDBG_MSG(("b_play_media_send_eos: skip %#lx %lu", (unsigned long)vaddr, (unsigned long)size));
        rc = NEXUS_Playpump_ReadComplete(p->params.playpump, 0, 0);
        if (rc!=NEXUS_SUCCESS) {
            BDBG_ERR(("b_play_media_send_eos: NEXUS_Playpump_ReadComplete error %#x, playback aborted", (unsigned)rc));
            p->state.state = eAborted;
            NEXUS_TaskCallback_Fire(p->errorCallback);
            return;
        }
    } else {
        BDBG_MSG(("b_play_media_send_eos: not supported"));
    }
    b_play_start_drain(p);
    return;
keep_waiting:
    p->state.state = eWaitingPlayback;
    p->state.data_source = b_play_media_send_eos; /* schedule to call us when space is avaliable */
    return;
}

static void
b_play_media_io_read_complete(void *playback_, ssize_t size)
{
    NEXUS_PlaybackHandle playback = playback_;
    void (*read_cont)(void *cont, ssize_t size);
    void *cntx;
    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    /* acquire mutex and call continuation */
    BDBG_ASSERT(playback->state.media.async_read.read_cont);
    read_cont = playback->state.media.async_read.read_cont;
    cntx = playback->state.media.async_read.cntx;
    playback->state.media.async_read.read_cont=NULL;
    playback->state.media.async_read.cntx=NULL;
    read_cont(cntx, size);
}

static void
b_play_media_io_async_read(void *sync_cnxt, bfile_io_read_t fd, void *buf, size_t length, void (*read_cont)(void *cont, ssize_t size), void *cntx)
{
    NEXUS_PlaybackHandle playback = sync_cnxt;
    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    BDBG_ASSERT(playback->state.media.async_read.read_cont==NULL);
    BDBG_ASSERT(playback->state.media.async_read.cntx==NULL);
    /* save context and call into async I/O */
    playback->state.media.async_read.read_cont=read_cont;
    playback->state.media.async_read.cntx=cntx;
    NEXUS_File_AsyncRead(fd, buf, length, NEXUS_MODULE_SELF, b_play_media_io_read_complete, playback);
    return;
}

void
b_play_stop_media(NEXUS_PlaybackHandle playback)
{
    if(playback->state.media.entry.atom) {
        batom_release(playback->state.media.entry.atom);
        playback->state.media.entry.atom = NULL;
    }
    if(playback->state.media.pts_timer) {
        NEXUS_CancelTimer(playback->state.media.pts_timer);
        playback->state.media.pts_timer = NULL;
    }
    if(playback->state.media.nest_timer) {
        NEXUS_CancelTimer(playback->state.media.nest_timer);
        playback->state.media.nest_timer = NULL;
    }
    bmedia_player_destroy(playback->media_player);
    playback->media_player = NULL;
    if(playback->state.media.buffer) {
        BDBG_ASSERT(playback->state.media.factory);
        bfile_buffer_destroy(playback->state.media.buffer);
        batom_factory_destroy(playback->state.media.factory);
        BDBG_ASSERT(playback->state.media.buf);
        BKNI_Free(playback->state.media.buf);
        playback->state.media.factory = NULL;
        playback->state.media.buffer = NULL;
        playback->state.media.buf = NULL;
    }
    return;
}

static void
b_play_media_player_error(void *cntx)
{
    NEXUS_PlaybackHandle playback = cntx;
    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    BDBG_MSG(("b_play_media_player_error: %#lx", (unsigned long)playback));
    NEXUS_TaskCallback_Fire(playback->parsingErrorCallback);
    return;
}

void
b_play_update_media_player_config(NEXUS_PlaybackHandle p, bmedia_player_decoder_config *config)
{
    const NEXUS_Playback_P_PidChannel *pid;

    for (pid = BLST_S_FIRST(&p->pid_list); pid ; pid = BLST_S_NEXT(pid, link)) {
        NEXUS_VideoDecoderStatus videoStatus;
        if(pid->cfg.pidSettings.pidType==NEXUS_PidType_eVideo && pid->cfg.pidTypeSettings.video.decoder) {
            NEXUS_Error rc = NEXUS_VideoDecoder_GetStatus(pid->cfg.pidTypeSettings.video.decoder, &videoStatus);
            if(rc==BERR_SUCCESS) {
                config->video_buffer_size = videoStatus.fifoSize;
            }
        }
    }
    config->fragmented = true;
#if NEXUS_OTFPVR
    config->otf = true;
#endif
    return;
}

#include "bcmindexer.h"
#include "bcmplayer.h"
#include "../src/bcmindexerpriv.h"

static NEXUS_PlaybackMpeg2TsIndexType
b_play_mpeg2ts_probe_index(bfile_io_read_t fd, NEXUS_PlaybackHandle playback)
{
    NEXUS_TransportTimestampType timestampType = playback->params.playpumpSettings.timestamp.type;
    int rc;
    const BNAV_Entry *entry;
    size_t nav_size;
    int nav_version;
    unsigned nav_count;
    unsigned nav_total_count;
    unsigned off;
    unsigned ts_header;
    unsigned ts_count;
    unsigned ts_total_count;

    fd->seek(fd, 0, SEEK_SET);
    rc = fd->read(fd, playback->state.media.probe_buf, sizeof(playback->state.media.probe_buf));
    fd->seek(fd, 0, SEEK_SET);
    if(rc!=sizeof(playback->state.media.probe_buf)) {
        return NEXUS_PlaybackMpeg2TsIndexType_eAutoDetect;
    }
    entry = (void *)playback->state.media.probe_buf;
    nav_version = BNAV_get_version( entry );
    nav_size = BNAV_GetEntrySize(nav_version);
    for(nav_total_count=0,nav_count=0;(uint8_t *)entry+nav_size<playback->state.media.probe_buf+sizeof(playback->state.media.probe_buf);
         nav_total_count++,entry = (void *)((uint8_t *)entry + nav_size)) {
        if(nav_version == BNAV_get_version(entry)) {
            nav_count++;
        }
    }
    ts_header = timestampType==NEXUS_TransportTimestampType_eNone?0:4;
    for(ts_total_count=0,ts_count=0,off=ts_header;off<sizeof(playback->state.media.probe_buf)-1;off+=ts_header+188,ts_total_count++) {
        if(playback->state.media.probe_buf[off]==0x47) {
            ts_count++;
        }
    }
    BDBG_MSG(("b_play_mpeg2ts_probe_index: ts_count:%u/%u nav_count:%u/%u", ts_count,ts_total_count, nav_count, nav_total_count));
    if(nav_count==nav_total_count) {
        return NEXUS_PlaybackMpeg2TsIndexType_eNav;
    } else if(ts_count==ts_total_count) {
        return NEXUS_PlaybackMpeg2TsIndexType_eSelf;
    } else {
        return NEXUS_PlaybackMpeg2TsIndexType_eAutoDetect;
    }
}

static const struct {
    NEXUS_VideoCodec nexusVideoCodec;
    bvideo_codec media_video_codec;
} NEXUS_Playback_P_VideoCodecMap[]= {
    {NEXUS_VideoCodec_eMpeg1, bvideo_codec_mpeg1},
    {NEXUS_VideoCodec_eMpeg2, bvideo_codec_mpeg2},
    {NEXUS_VideoCodec_eMpeg4Part2, bvideo_codec_mpeg4_part2},
    {NEXUS_VideoCodec_eH263, bvideo_codec_h263},
    {NEXUS_VideoCodec_eH264, bvideo_codec_h264},
    {NEXUS_VideoCodec_eH264_Svc, bvideo_codec_h264_svc},
    {NEXUS_VideoCodec_eH264_Mvc, bvideo_codec_h264_mvc},
    {NEXUS_VideoCodec_eVc1, bvideo_codec_vc1},
    {NEXUS_VideoCodec_eVc1SimpleMain, bvideo_codec_vc1_sm},
    {NEXUS_VideoCodec_eDivx311, bvideo_codec_divx_311},
    {NEXUS_VideoCodec_eAvs, bvideo_codec_avs}
};

static bvideo_codec
NEXUS_Playback_P_VideoCodecMap_ToMedia(NEXUS_VideoCodec videoCodec)
{
    unsigned i;
    for(i=0;i<sizeof(NEXUS_Playback_P_VideoCodecMap)/sizeof(*NEXUS_Playback_P_VideoCodecMap);i++) {
        if(NEXUS_Playback_P_VideoCodecMap[i].nexusVideoCodec == videoCodec) {
            return NEXUS_Playback_P_VideoCodecMap[i].media_video_codec;
        }
    }
    return bvideo_codec_unknown;
}

static const struct {
    NEXUS_AudioCodec nexusAudioCodec;
    baudio_format media_audio_codec;
} NEXUS_Playback_P_AudioCodecMap[]= {
    {NEXUS_AudioCodec_eMpeg, baudio_format_mpeg},
    {NEXUS_AudioCodec_eMp3, baudio_format_mp3},
    {NEXUS_AudioCodec_eAac, baudio_format_aac},
    {NEXUS_AudioCodec_eAacLoas, baudio_format_aac},
    {NEXUS_AudioCodec_eAacPlus, baudio_format_aac_plus},
    {NEXUS_AudioCodec_eAacPlusAdts, baudio_format_aac_plus_adts},
    {NEXUS_AudioCodec_eAc3,  baudio_format_ac3},
    {NEXUS_AudioCodec_eAc3Plus,  baudio_format_ac3_plus},
    {NEXUS_AudioCodec_eDts,  baudio_format_dts},
    {NEXUS_AudioCodec_eLpcmDvd,  baudio_format_lpcm_dvd},
    {NEXUS_AudioCodec_eLpcmHdDvd,  baudio_format_lpcm_hddvd},
    {NEXUS_AudioCodec_eLpcmBluRay, baudio_format_lpcm_bluray},
    {NEXUS_AudioCodec_eDtsHd,  baudio_format_dts_hd},
    {NEXUS_AudioCodec_eWmaStd, baudio_format_wma_std},
    {NEXUS_AudioCodec_eWmaPro, baudio_format_wma_pro},
    {NEXUS_AudioCodec_eAvs,  baudio_format_avs},
    {NEXUS_AudioCodec_ePcm,  baudio_format_pcm}
};

static baudio_format
NEXUS_Playback_P_AudioCodecMap_ToMedia(NEXUS_AudioCodec audioCodec)
{
    unsigned i;
    for(i=0;i<sizeof(NEXUS_Playback_P_AudioCodecMap)/sizeof(*NEXUS_Playback_P_AudioCodecMap);i++) {
        if(NEXUS_Playback_P_AudioCodecMap[i].nexusAudioCodec == audioCodec) {
            return NEXUS_Playback_P_AudioCodecMap[i].media_audio_codec;
        }
    }
    return baudio_format_unknown;
}


NEXUS_Error
b_play_start_media(NEXUS_PlaybackHandle playback, NEXUS_FilePlayHandle file, const NEXUS_PlaypumpStatus *playpump_status,const NEXUS_PlaybackStartSettings *params)
{
    bmedia_player_stream stream;
    bmedia_player_config player_config;
    unsigned i;
    const NEXUS_Playback_P_PidChannel *pid;
    const NEXUS_Playback_P_PidChannel *master_pid;
    NEXUS_Error rc=NEXUS_SUCCESS;
    NEXUS_PlaypumpSettings pumpCfg;
    bmedia_player_decoder_mode mode;

    playback->media_player = NULL;
    playback->state.index_type = NEXUS_PlaybackMpeg2TsIndexType_eAutoDetect;
    playback->state.media.last_segment_hdr = (uint8_t*)playpump_status->bufferBase + playpump_status->fifoSize - sizeof(NEXUS_PlaypumpSegment);
    playback->state.media.time_scale = BMEDIA_TIME_SCALE_BASE;
    playback->state.media.buffer = NULL;
    playback->state.media.buf = NULL;
    playback->state.media.factory = NULL;
    playback->state.media.pts_timer = NEXUS_ScheduleTimer(BMEDIA_UPDATE_POSITION_INTERVAL, b_play_media_pts_timer, playback);

    b_play_media_time_test();
    bmedia_player_init_stream(&stream);
    switch(playback->params.playpumpSettings.transportType) {
    case NEXUS_TransportType_eAsf:
        stream.format = bstream_mpeg_type_asf;
        playback->state.media.segmented = true;
        break;
    case NEXUS_TransportType_eAvi:
        stream.format = bstream_mpeg_type_avi;
        playback->state.media.segmented = true;
        break;
    case NEXUS_TransportType_eMp4:
        stream.format = bstream_mpeg_type_mp4;
        playback->state.media.segmented = true;
        break;
    case NEXUS_TransportType_eMkv:
        stream.format = bstream_mpeg_type_mkv;
        playback->state.media.segmented = true;
        break;
    case NEXUS_TransportType_eTs:
        stream.format = bstream_mpeg_type_ts;
        playback->state.media.segmented = false;
        break;
    case NEXUS_TransportType_eDssPes:
    case NEXUS_TransportType_eDssEs:
        stream.format = bstream_mpeg_type_dss_es;
        playback->state.media.segmented = false;
        break;
    case NEXUS_TransportType_eMpeg2Pes:
        stream.format = bstream_mpeg_type_pes;
        playback->state.media.segmented = false;
        break;
    case NEXUS_TransportType_eMpeg1Ps:
        stream.format = bstream_mpeg_type_ts;
        playback->state.media.segmented = false;
        break;
    case NEXUS_TransportType_eVob:
        stream.format = bstream_mpeg_type_vob;
        playback->state.media.segmented = false;
        break;
    case NEXUS_TransportType_eWav:
        stream.format = bstream_mpeg_type_wav;
        playback->state.media.segmented = true;
        break;
    case NEXUS_TransportType_eMp4Fragment:
        stream.format = bstream_mpeg_type_mp4_fragment;
        playback->state.media.segmented = true;
        break;
    case NEXUS_TransportType_eRmff:
        stream.format = bstream_mpeg_type_rmff;
        playback->state.media.segmented = true;
        break;
    case NEXUS_TransportType_eFlv:
        stream.format = bstream_mpeg_type_flv;
        playback->state.media.segmented = true;
        break;
    case NEXUS_TransportType_eOgg:
        stream.format = bstream_mpeg_type_ogg;
        playback->state.media.segmented = false;
        break;
    case NEXUS_TransportType_eFlac:
        stream.format = bstream_mpeg_type_flac;
        playback->state.media.segmented = false;
        break;
    case NEXUS_TransportType_eApe:
        stream.format = bstream_mpeg_type_ape;
        playback->state.media.segmented = false;
        break;
    default:
        BDBG_WRN(("Unknown transport type %u, defaulting to ES", playback->params.playpumpSettings.transportType));
        /* fallthrough */
    case NEXUS_TransportType_eEs:
        stream.format = bstream_mpeg_type_es;
        playback->state.media.segmented = false;
        break;
    }

    master_pid = NULL;

    /* choose a master track */
    for(pid = BLST_S_FIRST(&playback->pid_list); pid ; pid = BLST_S_NEXT(pid, link)) {
        switch(pid->cfg.pidSettings.pidType) {
        case NEXUS_PidType_eVideo:
            if(pid->cfg.pidTypeSettings.video.codec==NEXUS_VideoCodec_eH264) {
                /* H264 (base layer) overwrite master pid */
                master_pid = pid;
            } else {
                if(master_pid==NULL || master_pid->cfg.pidSettings.pidType!=NEXUS_PidType_eVideo) { /* Video overwrites audio */
                    master_pid = pid;
                }
            }
            break;
        case NEXUS_PidType_eAudio:
            if(master_pid==NULL) {
                master_pid = pid;
            }
            break;
        default: break;
        }
    }
    if(master_pid) {
        stream.master = master_pid->pid;
    } else {
        /* if there's no masgter pid, then we can still do playback-only work. setting to the NULL
        pid tells media framework plugs that we have not forgotten a param in their API. */
        stream.master = 0x1fff;
    }

    for(i=0,pid = BLST_S_FIRST(&playback->pid_list); pid ; pid = BLST_S_NEXT(pid, link)) {
        bool statusValid;
        NEXUS_PidChannelStatus status;
        baudio_format audio_codec = baudio_format_unknown; 
        bvideo_codec video_codec = bvideo_codec_unknown;

        switch(pid->cfg.pidSettings.pidType) {
        case NEXUS_PidType_eVideo:
            video_codec = NEXUS_Playback_P_VideoCodecMap_ToMedia(pid->cfg.pidTypeSettings.video.codec);
            break;
        case NEXUS_PidType_eAudio:
            audio_codec = NEXUS_Playback_P_AudioCodecMap_ToMedia(pid->cfg.pidSettings.pidTypeSettings.audio.codec);
            break;
        default: break;
        }
        statusValid = NEXUS_PidChannel_GetStatus(pid->pidChn, &status)==NEXUS_SUCCESS;
        if(pid==master_pid) {
            stream.stream.es.video_codec = video_codec;
            stream.stream.es.audio_codec = audio_codec;
            if(statusValid) {
                BDBG_MSG(("%s:%lx mapping track %u(master) -> %#x:%#x", "b_play_start_media", (unsigned)playback, (unsigned)pid->pid, (unsigned)status.remappedPid, (unsigned)status.pid));
                stream.stream.id.master = status.remappedPid;
            }
        } else {
            if(i<BMEDIA_PLAYER_MAX_TRACKS) {
                stream.stream.es.other_video[i] = video_codec;
                stream.stream.es.other_audio[i] = audio_codec;
                if(statusValid) {
                    BDBG_MSG(("%s:%lx mapping track %u(%u) -> %#x:%#x", "b_play_start_media", (unsigned)playback, (unsigned)pid->pid, (unsigned)i, (unsigned)status.remappedPid, (unsigned)status.pid));
                    stream.stream.id.other[i] = status.remappedPid;
                }
                stream.other[i] = pid->pid; 
            } else {
                BDBG_WRN(("%s:%#lx track:%u exceeded max number of tracks %u>=%u", "b_play_start_media", (unsigned)playback, (unsigned)pid->pid, (unsigned)i, BMEDIA_PLAYER_MAX_TRACKS));
            }
            i++;
        } 
    }

    if( (playback->params.playpumpSettings.transportType==NEXUS_TransportType_eMp4 ||
         playback->params.playpumpSettings.transportType==NEXUS_TransportType_eApe ||
         playback->params.playpumpSettings.transportType==NEXUS_TransportType_eMkv) &&
        file->file.index==NULL
       )
    {
        BDBG_ERR(("NEXUS_FilePlayHandle must be opened with an index for this transport type"));
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto error_file_index;
    }

    if(file->file.index && params->mode == NEXUS_PlaybackMode_eIndexed && playback->params.playpumpSettings.transportType==NEXUS_TransportType_eTs) {
        switch(params->mpeg2TsIndexType) {
        case NEXUS_PlaybackMpeg2TsIndexType_eAutoDetect:
            playback->state.index_type = b_play_mpeg2ts_probe_index(file->file.index, playback);
            break;
        case NEXUS_PlaybackMpeg2TsIndexType_eNav:
        case NEXUS_PlaybackMpeg2TsIndexType_eSelf:
            playback->state.index_type = (NEXUS_PlaybackMpeg2TsIndexType)params->mpeg2TsIndexType;
            break;
        default:
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto error_file_index;
        }
    }
    if( playback->params.playpumpSettings.transportType==NEXUS_TransportType_eMp4 ||
        playback->params.playpumpSettings.transportType==NEXUS_TransportType_eApe ||
        playback->params.playpumpSettings.transportType==NEXUS_TransportType_eMkv ||
        (playback->params.playpumpSettings.transportType==NEXUS_TransportType_eEs && (file->file.index || params->mode == NEXUS_PlaybackMode_eAutoBitrate || playback->params.enableStreamProcessing)) ||
        ((params->mode != NEXUS_PlaybackMode_eIndexed || file->file.index==NULL) && playback->params.playpumpSettings.transportType==NEXUS_TransportType_eTs) ||
        (playback->params.playpumpSettings.transportType==NEXUS_TransportType_eTs && playback->state.index_type==NEXUS_PlaybackMpeg2TsIndexType_eSelf) ||
    ((playback->params.playpumpSettings.transportType==NEXUS_TransportType_eMpeg2Pes || playback->params.playpumpSettings.transportType==NEXUS_TransportType_eVob) && params->mode==NEXUS_PlaybackMode_eIndexed && file->file.index)
    )
      {
          bfile_buffer_cfg buffer_cfg;
          playback->state.media.factory = batom_factory_create(bkni_alloc, 16);
          if(!playback->state.media.factory) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error_factory; }
          bfile_buffer_default_cfg(&buffer_cfg);
          playback->state.media.buf = BKNI_Malloc(buffer_cfg.buf_len+BIO_BLOCK_SIZE);
          if(!playback->state.media.buf) {
              BDBG_ERR(("b_play_start_media: %#lx can't allocate %u bytes for buffer", (unsigned long)playback, (unsigned)buffer_cfg.buf_len+BIO_BLOCK_SIZE));
              rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
              goto error_media;
          }
          buffer_cfg.buf = (void*)B_IO_ALIGN_ROUND((unsigned long)playback->state.media.buf);
          buffer_cfg.fd = playback->file->file.data;
          buffer_cfg.async = true;
          buffer_cfg.sync_cnxt = playback;
          /* since async I/O is not synchronized with playback, we need to pass async I/O over custom layer that would acquire lock before calling back into the bfile_buffer */
          buffer_cfg.async_read = b_play_media_io_async_read;
          playback->state.media.buffer = bfile_buffer_create(playback->state.media.factory, &buffer_cfg);
          if(!playback->state.media.buffer) {
              rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
              goto error_buffer;
          }
      }

#if B_HAS_DIVX_DRM
    if(playback->params.playpumpSettings.securityContext && playback->params.playpumpSettings.transportType==NEXUS_TransportType_eMkv){
        stream.cntx = playback->params.playpumpSettings.securityContext;
        stream.decrypt_callback = bdrm_decrypt_mkv_payload;
    }
#endif

    bmedia_player_init_config(&player_config);
    player_config.buffer = playback->state.media.buffer;
    player_config.factory = playback->state.media.factory;
    player_config.cntx = playback;
    player_config.atom_ready = b_play_media_async_atom_ready;
    player_config.error_detected = b_play_media_player_error;
    player_config.timeshifting = playback->params.timeshifting;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    player_config.delayRecordedOffsetAlign = params->delayRecordedOffsetAlign;
    player_config.delayedRecordingStatus = params->delayedRecordingStatus;
#endif
    b_play_update_media_player_config(playback, &player_config.decoder_features);

    if(playback->params.playpumpSettings.transportType==NEXUS_TransportType_eTs) {
        stream.stream.mpeg2ts.packet_size = playback->params.playpumpSettings.timestamp.type == NEXUS_TransportTimestampType_eNone?188:192;
    }

    if(params->mode == NEXUS_PlaybackMode_eIndexed && file->file.index) {
        if(playback->state.index_type==NEXUS_PlaybackMpeg2TsIndexType_eSelf || playback->params.playpumpSettings.transportType==NEXUS_TransportType_eMpeg2Pes || playback->params.playpumpSettings.transportType==NEXUS_TransportType_eVob || playback->params.playpumpSettings.transportType==NEXUS_TransportType_eEs)  {
            playback->state.media.segmented = false;
            stream.without_index = true;
            stream.stream.noindex.auto_rate = true;
            stream.stream.noindex.bitrate = params->bitrate;
            if(playback->params.enableStreamProcessing) {
                stream.stream.noindex.auto_rate = true;
                stream.stream.id.master = 0xC0;
           }
           else if( playback->params.playpumpSettings.transportType==NEXUS_TransportType_eEs)
           {
                stream.stream.id.master = 0x0;
           }
        }
        if (!player_config.timeshifting) {
            off_t first, last;
            if (!file->file.data->bounds(file->file.data, &first, &last)) {
                player_config.data_file_size = last;
            }
        }
        playback->media_player = bmedia_player_create(file->file.index, &player_config, &stream);
        if (!playback->media_player) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error_player; }
    } else {
        playback->state.media.segmented = false;
        stream.without_index = true;
        stream.stream.noindex.auto_rate = params->mode == NEXUS_PlaybackMode_eAutoBitrate;
        if(playback->params.playpumpSettings.transportType==NEXUS_TransportType_eEs) {
            if(playback->params.enableStreamProcessing) {
                stream.stream.noindex.auto_rate = true;
                stream.stream.id.master = 0xC0;
            } else {
                if(params->mode == NEXUS_PlaybackMode_eAutoBitrate) {
                    stream.stream.id.master = 0;
                }
            }
        }
        stream.stream.noindex.bitrate = params->bitrate;
        playback->media_player = bmedia_player_create(file->file.data, &player_config, &stream);
        if (!playback->media_player) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error_player; }
    }
    bmedia_player_set_direction(playback->media_player, 0, BMEDIA_TIME_SCALE_BASE, &mode); /* normal decode */
    NEXUS_Playpump_GetSettings(playback->params.playpump, &pumpCfg);
    playback->actualTransportType = pumpCfg.transportType;
    pumpCfg.mode = playback->state.media.segmented ? NEXUS_PlaypumpMode_eSegment : NEXUS_PlaypumpMode_eFifo;
    rc = NEXUS_Playpump_SetSettings(playback->params.playpump, &pumpCfg);
    if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto error_playpump; }
    playback->state.direction = 1;
    playback->state.data_source = b_play_next_frame;
    return NEXUS_SUCCESS;

error_playpump:
    bmedia_player_destroy(playback->media_player);
    playback->media_player = NULL;
error_player:
    if(playback->state.media.buffer) {
        BDBG_ASSERT(playback->state.media.factory);
        bfile_buffer_destroy(playback->state.media.buffer);
        batom_factory_destroy(playback->state.media.factory);
        playback->state.media.factory = NULL;
        playback->state.media.buffer = NULL;
    }
error_buffer:
    if(playback->state.media.buf) {
        BKNI_Free(playback->state.media.buf);
        playback->state.media.buf = NULL;
    }
error_media:
    if(playback->state.media.factory) {
        batom_factory_destroy(playback->state.media.factory);
        playback->state.media.factory = NULL;
    }
error_file_index:
error_factory:
    if(playback->state.media.pts_timer) {
        NEXUS_CancelTimer(playback->state.media.pts_timer);
        playback->state.media.pts_timer = NULL;
    }
    return rc;
}

