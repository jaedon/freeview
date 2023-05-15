/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_player.c $
 * $brcm_Revision: 73 $
 * $brcm_Date: 2/21/12 5:12p $
 *
 * Module Description:
 *
 * BMedia library, player interface
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_player.c $
 * 
 * 73   2/21/12 5:12p vsilyaev
 * SW7425-2378: Support for Monkey Audio (APE) files
 * 
 * 72   12/7/11 4:00p vsilyaev
 * SW7425-1917: Added player for RMFF content
 * 
 * 71   7/11/11 4:57p vsilyaev
 * SWDTV-7845: Added WAV player
 * 
 * 70   5/19/11 10:05a erickson
 * SW7405-5317: bmedia_player_set_decoder_config should fail on
 * unsupported mode, not just WRN.
 * 
 * 69   4/19/11 5:28p vsilyaev
 * SW7422-14: Differentiate extra audio/video and 'other' tracks
 * 
 * 68   3/10/11 11:44a vsilyaev
 * SW7422-14: Increased limit on the frame/block size and print  warning
 * when limit is applied
 * 
 * 67   3/2/11 1:07p vsilyaev
 * SWBLURAY-24399: Updated logic to handle custom step size
 * 
 * 66   3/2/11 12:38p vsilyaev
 * SWBLURAY-24399: Added option to control step size in the TS player.
 * 
 * fix_bdvd_v3.0_SWBLURAY-24399/1   2/18/11 1:40p ialauder
 * Allow the index seek step to be configurable.
 * 
 * 65   11/16/10 5:52p vsilyaev
 * SWBLURAY-22285: Merged AAC ES player
 * 
 * 64   10/18/10 4:31p vsilyaev
 * SW7550-539: Apply timescale factor for mode without discontinuity
 * 
 * 63   9/28/10 1:56p erickson
 * SWDEPRECATED-1003: add
 * bmedia_player_decoder_mode.force_source_frame_rate
 *
 * fix_bdvd_v3.0_SWBLURAY-22285/1   11/16/10 3:28p ialauder
 * Add support for AAC elementary streams.
 * 
 * 62   9/23/10 3:48p vsilyaev
 * SWBLURAY-22622: Renamed chunk_size to the key_frame_distance
 * 
 * 61   9/23/10 3:02p vsilyaev
 * SWBLURAY-22622: Allow user to specify block size used by some players.
 * 
 * fix_bdvd_b3pvr_SWBLURAY-22622/1   9/22/10 1:18p ialauder
 * Add the ability to force the chuck size to a particular value.
 * 
 * 60   8/19/10 5:22p vsilyaev
 * SWBLURAY-21863: Added option to divide large video frame into multiple
 * PES packets
 * 
 * 57   5/4/10 11:35a vsilyaev
 * SW7405-4291: Use PTS to get position for WAV files
 * 
 * 56   4/8/10 6:56p vsilyaev
 * SW7405-3773: Fixed debug messages
 * 
 * 55   3/12/10 6:12p vsilyaev
 * SW3556-1077: Added code to auto-select between different players
 * 
 * 54   3/11/10 1:02p vsilyaev
 * SW7405-2697: Reassert time_scale if player was forced into the normal
 * play mode
 * 
 * 53   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 * 
 * 52   11/18/09 4:48p mward
 * SW7405-3260: In case there's no index, seek to zero is possible.
 * 
 * 51   10/21/09 5:34p erickson
 * SW7405-3260: check stream format in bmedia_player_seek in case there's
 * no index. fail an impossible seek.
 *
 * 50   10/1/09 12:07p vsilyaev
 * SW7405-3121: Merge changes for handling not interleaved AVI streams
 *
 * fix_bdvd_v1.5_pr15387/1   9/30/09 3:32p gooslin
 * Add support for indicating to the AVI player which streams should be
 * dropped
 *
 * 49   8/20/09 10:02a gmohile
 * PR 55991 : Add PES/VOB trickmode
 * 47   8/4/09 10:39a katrep
 * PR55207: Do not change the state if the trickmode request is not
 * successful
 *
 * 46   8/3/09 1:45p katrep
 * PR56611: fixed issue with transition from 5x-2x playback
 *
 * 45   7/9/09 11:54a vsilyaev
 * PR 56611 PR 56169: Fixed test for decoder fast forward mode
 *
 * 44   7/8/09 3:24p vsilyaev
 * PR 55989: Used size of the video decoder CDB to control size of the
 * data chunk
 *
 * 43   7/8/09 11:52a vsilyaev
 * PR 55989: Added modes to support OTF and fragmented trickmodes
 *
 * 42   6/23/09 5:51p vsilyaev
 * PR 55845: Added content type for the media player entry, this fixes key
 * frame extraction for AVI 2.0 (OpenDML) streams
 *
 * 41   6/22/09 1:04p vsilyaev
 * PR 55839: Return position based on the PTS value for no-index ASF and
 * AVI streams
 *
 * 40   6/19/09 4:56p vsilyaev
 * PR 56169: Allow fast decode trickmodes even without STC trickmodes
 *
 * 39   6/19/09 3:47p vsilyaev
 * PR 55989: Added decode_mode  paramtewr to to the set_direction method
 *
 * 38   4/21/09 12:48p vsilyaev
 * PR 52519: Added better debug messages for the failure cases
 *
 * 37   4/1/09 3:27p vsilyaev
 * PR 51351:  Extended to support Mpeg Layer1 and Layer2 audio
 *
 * 36   3/27/09 12:51p vsilyaev
 * PR 50311: Return readPosition in the Nexus playback status. In
 * combination with the position it could be used to measure depth of
 * decoder and playback buffers in units of time.
 *
 * 35   2/9/09 2:37p gmohile
 * PR 51813 : Truncate large entries due to corrupted index
 *
 * 34   2/3/09 3:03p vsilyaev
 * PR 51822: Added upper bound on number of bytes read when opening AVI
 * file
 *
 * 33   2/2/09 3:35p gmohile
 * PR 47815 : Add manual control of trickmodes
 *
 * 32   2/2/09 2:53p gmohile
 * PR 47815 : Add manual control of trickmodes
 *
 * 31   1/20/09 5:25p vsilyaev
 * PR 51351: Added code for on-the-fly indexing of MP3 streams
 *
 * 30   1/12/09 5:42p vsilyaev
 * PR 50763: Improved seek to position 0
 *
 * 29   12/19/08 3:56p vsilyaev
 * PR 50214: Allowed set_direction function to fail and added framework
 * for players to communicate about an errors
 *
 * 28   12/18/08 2:15p vsilyaev
 * PR 49605: Flush pipeline on transition from FF to SF
 *
 * 27   12/17/08 10:35a jtna
 * PR48342: set correct time_scale for slow FF
 *
 * 26   12/10/08 1:38a vsilyaev
 * PR 48760: Added ES player for accurate progress monitor and navigation
 *
 * 25   12/8/08 11:20a erickson
 * PR49930: add bmedia_player_lookup_pts
 *
 * 24   11/6/08 2:57p gmohile
 * PR 48571 : Use generic player methods for media_player_tell
 *
 * 23   10/20/08 7:40p vsilyaev
 * PR 48081: PR 48082: If no PTS valid, then use previous good know
 * position
 *
 * 22   10/16/08 6:32p vsilyaev
 * PR 47995: Added control over ID's used for media->PES conversion
 *
 * 21   10/7/08 5:21p vsilyaev
 * PR 47122: PR 47570: Added code to properly handle decoder fast forward
 * trickmodes
 *
 * 20   8/29/08 5:48p vsilyaev
 * PR 33812: Added support for navigation in the MPEG-2 TS file without an
 * index
 *
 * 19   7/2/08 12:16p erickson
 * PR36836: bmedia_player_update_position should be a no-op if there's no
 * index
 *
 * 18   6/12/08 3:09p vsilyaev
 * PR 43184: Added more debug output
 *
 * 17   6/4/08 10:18a vsilyaev
 * PR 43184: Don't detect EOS in the timeshifting mode
 *
 * 16   5/5/08 4:47p vsilyaev
 * PR 42355: Fixed slow-forward/frame advace
 *
 * 15   3/3/08 12:21p vsilyaev
 * PR 39818: Support of MKV format
 *
 * 14   1/10/08 2:48p erickson
 * PR36068: rename btm to brcm
 *
 * 13   12/20/07 5:24p vsilyaev
 * PR 38073: Improved handling of trickmodes
 *
 * 12   12/20/07 3:40p vsilyaev
 * PR 38073: Fixed handling of MPEG-2 TS streams
 *
 * 11   12/20/07 10:51a vsilyaev
 * PR 38073: Added missing bmedia_player_tell
 *
 * 10   12/20/07 10:28a vsilyaev
 * PR 38073: Updated to work with new bmedia_player
 *
 * PR38073/5   12/20/07 10:23a vsilyaev
 * PR 38073: Use new timestamp/seek logic only if NAV_PLAYER is enabled
 *
 * PR38073/4   12/19/07 5:01p vsilyaev
 * PR 38073: Moved time tracking code for host paced mode into the
 * bmedia_player
 *
 * PR38073/3   12/18/07 6:45p vsilyaev
 * PR 38073: Added handling of no-index streams
 *
 * PR38073/2   12/18/07 5:02p vsilyaev
 * PR 38073: Improved code to handle MPEG-2 TS packets
 *
 * PR38073/1   12/14/07 4:51p vsilyaev
 * PR 38073: Added NAV player into the media player
 *
 * 9   12/10/07 4:34p vsilyaev
 * PR 38073: Added generic (no index player)
 *
 * 8   7/16/07 6:09p vsilyaev
 * PR 29125: Added documentation
 *
 * 7   7/9/07 3:46p vsilyaev
 * PR 32846: Added ASF support
 *
 * 6   6/6/07 5:23p vsilyaev
 * PR 31887: Use asynchronous I/O for MP4 container
 *
 * 5   5/16/07 9:57p vsilyaev
 * PR 28631: Use scatter/gather object from the file buffer
 *
 * 4   5/11/07 4:05p vsilyaev
 * PR 29921: Added fast forward for MP4 container
 *
 * 3   5/8/07 2:20p vsilyaev
 * PR 29921: Fixed transitiones between normal play and trickmodes
 *
 * 2   5/1/07 3:23p vsilyaev
 * PR 28631: Integrated support for MP4 container
 *
 * 1   4/17/07 1:57p vsilyaev
 * PR 29921: AVI player
 *
 *
 *******************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bmedia_player.h"
#include "bmedia_player_generic.h"
#include "bmedia_player_es.h"
#include "bmp4_player.h"
#include "bmkv_player.h"
#include "bwav_player.h"
#include "bape_player.h"
#include "bmpeg2ts_player.h"
#include "bmpeg2pes_player.h"
#if B_HAS_AVI
#include "bavi_player.h"
#include "bavi_player_custom.h"
#endif

#if B_HAS_ASF
#include "basf_player.h"
#endif

#if B_HAS_RMFF
#include "brmff_player.h"
#endif

#if B_HAS_NAV_PLAYER
#include "bmedia_player_nav.h"
#endif



BDBG_MODULE(bmedia_player);

#define BDBG_MSG_FLOW(x)   /* BDBG_MSG(x) */

BDBG_OBJECT_ID(bmedia_player_t);

struct bmedia_player {
    BDBG_OBJECT(bmedia_player_t)
    void *player;
    const bmedia_player_methods *methods;

    /* data that is used to account in mapping from PTS value to the position in the stream, it accounts to scaling of PTS'es when doing trickmodes */
    bmedia_player_pos index_offset; /* player position that corresponds to the 0 PTS value, it's needed to account for the PTS wraparound */
    bmedia_time_scale time_scale; /* current scale of PTS values */
    bmedia_player_pos last_position; /* last known good position */
    uint32_t last_pts;
    bstream_mpeg_type stream_format;
    bool last_pts_valid;
    bool last_continous; /* if previous data sent to decoder was not continuous */
    bool last_mode_valid;
    bmedia_player_decoder_mode last_mode; /* saved previous mode */
    bmedia_time_scale max_decoder_rate; /* maximum fast forward rate supported by the decoder */
    bmedia_player_decoder_config decoder_features;
};

void
bmedia_player_init_stream(bmedia_player_stream *stream)
{
    unsigned i;
    BKNI_Memset(stream, 0, sizeof(*stream));
    stream->format = bstream_mpeg_type_es;
    stream->stream.es.audio_codec = baudio_format_unknown;
    stream->stream.es.video_codec = bvideo_codec_unknown;
    stream->stream.mpeg2ts.packet_size = 188;
    stream->stream.noindex.auto_rate = true;
    stream->stream.noindex.bitrate = 1024*1024;
    stream->stream.id.master = 0xE0;
    for(i=0;i<BMEDIA_PLAYER_MAX_TRACKS;i++) {
        stream->stream.id.other[i] = 0xC0+i;
        stream->drop[i] = 0;
        stream->stream.es.other_video[i] = bvideo_codec_unknown;
        stream->stream.es.other_audio[i] = baudio_format_unknown;
    }
    return;
}

static void
b_media_player_error_detected(void *cntx)
{
    BSTD_UNUSED(cntx);
    return;
}

static void
b_media_player_decoder_config_init(bmedia_player_decoder_config *config)
{
    BKNI_Memset(config, 0, sizeof(*config));
    config->max_decoder_rate = 1.2 * BMEDIA_TIME_SCALE_BASE;
    config->host_mode = bmedia_player_host_trick_mode_auto;
    config->mode_modifier = 1;
    config->brcm = true;
    config->video_buffer_size = 1024 * 1024;
    return;
}

void
bmedia_player_init_config(bmedia_player_config *config)
{
    BKNI_Memset(config, 0, sizeof(*config));
    config->error_detected = b_media_player_error_detected;
    config->decoder_features.brcm = true;
    config->timeshifting = false;
    config->prefered_read_size = BIO_BLOCK_SIZE;
    config->max_data_parsed = 1024*1024;
    config->reorder_timestamps = true;
    config->autoselect_player = true;
    config->max_pes_size = 0;
    config->key_frame_distance = 0;
    config->force_seek_step = 0;
    b_media_player_decoder_config_init(&config->decoder_features);
    return;
}

bmedia_player_t
bmedia_player_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream)
{
    bmedia_player_t player;
    const bmedia_player_methods *methods;
    bstream_mpeg_type type;
    BERR_Code rc;

    BSTD_UNUSED(rc);
    BDBG_ASSERT(stream);
    BDBG_ASSERT(config);
    BDBG_ASSERT(fd);

    type = stream->format;

    if(stream->without_index) {
        switch(type) {
        case bstream_mpeg_type_es:
            if(stream->stream.es.video_codec == bvideo_codec_unknown &&
                    (stream->stream.es.audio_codec == baudio_format_mp3 || stream->stream.es.audio_codec == baudio_format_mpeg ||
                    stream->stream.es.audio_codec == baudio_format_aac) &&
                    stream->stream.noindex.auto_rate) {
                methods = &bmedia_player_es;
            }  else {
                methods = &bmedia_player_generic;
            }
            break;
        case bstream_mpeg_type_pes:
        case bstream_mpeg_type_vob:
            if(stream->stream.noindex.auto_rate) {
                methods = &bmpeg2pes_player_methods;
                break;
            }
        case bstream_mpeg_type_ts:
            if(stream->stream.noindex.auto_rate) {
                methods = &bmpeg2ts_player_methods;
                break;
            }
            /* fell through */
        default:
            methods = &bmedia_player_generic;
            break;
        }
    } else {
        switch(type) {
#if B_HAS_AVI
        case bstream_mpeg_type_avi:
            /* try custom player first */
            methods = config->autoselect_player ? &bavi_player_custom_methods : &bavi_player_methods;
            break;
#endif
#if B_HAS_ASF
        case bstream_mpeg_type_asf:
            methods = &basf_player_methods;
            break;
#endif
#if B_HAS_RMFF
        case bstream_mpeg_type_rmff:
            methods = &brmff_player_methods;
            break;
#endif
        case bstream_mpeg_type_mp4:
            methods = &bmp4_player_methods;
            break;
        case bstream_mpeg_type_mkv:
            methods = &bmkv_player_methods;
            break;
        case bstream_mpeg_type_wav:
            methods = &bwav_player_methods;
            break;
        case bstream_mpeg_type_ape:
            methods = &bape_player_methods;
            break;
        case bstream_mpeg_type_ts:
#if B_HAS_NAV_PLAYER
            methods = &bmedia_player_nav;
            break;
#endif
        default:
            methods = NULL;
            break;
        }
    }
    if(!methods) {
        BDBG_WRN(("Unknown media type %u", (unsigned) type));
        return NULL;
    }
    player = BKNI_Malloc(sizeof(*player));
    if(!player) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(player, bmedia_player_t);
    player->index_offset = 0;
    player->time_scale = BMEDIA_TIME_SCALE_BASE;
    player->last_pts = 0;
    player->last_position = 0;
    player->last_pts_valid = false;
    player->last_continous = false;
    player->last_mode_valid = false;
    player->stream_format = stream->format;
    player->decoder_features = config->decoder_features;
    player->player = methods->create(fd, config, stream);
#if B_HAS_AVI
    if (!player->player && methods == &bavi_player_custom_methods ) {
        methods = &bavi_player_methods;
        player->player = methods->create(fd, config, stream); /* and if it fails than open with regular player */
    }
#endif
    if (!player->player) {
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_player;
    }
    player->methods = methods;
    return player;

err_player:
    BKNI_Free(player);
err_alloc:
    return NULL;
}

void
bmedia_player_destroy(bmedia_player_t player)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    player->methods->destroy(player->player);
    BDBG_OBJECT_DESTROY(player, bmedia_player_t);
    BKNI_Free(player);
    return;
}

int
bmedia_player_next(bmedia_player_t player, bmedia_player_entry *entry)
{
    int rc;
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    BDBG_ASSERT(entry);
    bmedia_player_init_entry(entry);
    rc = player->methods->next(player->player, entry);
    BDBG_MSG_FLOW(("bmedia_player_next:%#lx(%d) start:%lu length:%u type:%u timestamp:%u", (unsigned long)player, rc, (unsigned long)entry->length, (unsigned)entry->length, (unsigned)entry->type, (unsigned)entry->timestamp));

    if(entry->length >= BMEDIA_PLAYER_MAX_BLOCK){
        BDBG_WRN(("bmedia_player_next:%#lx truncated block from %u to %u", (unsigned long)player, (unsigned)entry->length, BMEDIA_PLAYER_MAX_BLOCK));
        entry->length = BMEDIA_PLAYER_MAX_BLOCK;
    }

    return rc;
}

void
bmedia_player_get_status(bmedia_player_t player, bmedia_player_status *status)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    status->index_error_cnt = 0;
    status->data_error_cnt = 0;
    status->position = BMEDIA_PLAYER_INVALID;
    player->methods->get_status(player->player, status);
    return;
}


#if B_HAS_NAV_PLAYER

int
bmedia_player_lookup_pts( bmedia_player_t player, bmedia_player_pos pos, uint32_t *p_pts )
{
    if(player->methods == &bmedia_player_nav) {
        return bmedia_player_nav_lookup_pts(player->player, pos, p_pts);
    }
    else {
        *p_pts = bmedia_time2pts(pos, player->time_scale);
        return 0;
    }
}

void
bmedia_player_tell(bmedia_player_t player, bmedia_player_pos *pos)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    BDBG_MSG_FLOW(("bmedia_player_tell>: %#lx", (unsigned long)player));
    BDBG_ASSERT(pos);
    if(player->methods == &bmedia_player_nav || player->methods == &bmpeg2ts_player_methods || player->methods == &bmpeg2pes_player_methods ||
        (player->methods == &bmedia_player_generic && !(player->stream_format==bstream_mpeg_type_wav || player->stream_format==bstream_mpeg_type_avi || player->stream_format==bstream_mpeg_type_asf))) {
        player->methods->tell(player->player, pos);
    } else {
        if(player->last_pts_valid) {
            *pos = player->index_offset + bmedia_pts2time(player->last_pts, player->time_scale);
            player->last_position = *pos;
            BDBG_MSG_FLOW(("bmedia_player_tell: %#lx offset:%lu time:%ld(pts:%lu,scale:%ld) -> %lu", (unsigned long)player, player->index_offset, bmedia_pts2time(player->last_pts, player->time_scale), player->last_pts, player->time_scale, *pos));
        } else {
            *pos = player->last_position;
        }
    }
    BDBG_MSG_FLOW(("bmedia_player_tell<: %#lx %u", (unsigned long)player, (unsigned)*pos));
    return;
}


#define B_FLAG(p,v) ((p)->v?#v:"")

int
bmedia_player_set_direction(bmedia_player_t player, bmedia_player_step direction, bmedia_time_scale time_scale, bmedia_player_decoder_mode *mode)
{
    int rc;
    bmedia_time_scale player_time_scale = time_scale;
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    BDBG_ASSERT(mode);

    BDBG_MSG(("bmedia_player_set_direction>: %#lx %d %d", (unsigned long)player, (int)direction, (int)time_scale));

    rc = 0;
    mode->brcm = false;
    mode->dqt = false;
    mode->tsm = true;
    mode->continuous = true;
    mode->discontinuity = false;
    mode->time_scale = time_scale;
    mode->host_paced = false;
    mode->fragmented = false;
    mode->force_source_frame_rate = 0;
    mode->display_frames = bmedia_player_decoder_frames_all;
    mode->reordering_mode = bmedia_player_reordering_mode_none;

    if(player->methods == &bmedia_player_nav) {
        rc = player->methods->set_direction(player->player, direction, player_time_scale, mode);
        goto done;
    }
    if(time_scale >= 0 && (time_scale <= BMEDIA_TIME_SCALE_BASE || (player->decoder_features.stc && time_scale <= player->decoder_features.max_decoder_rate))) {
        /* normal decode with slow motion  or fast forward */
        mode->discontinuity = player->time_scale > player->decoder_features.max_decoder_rate || player->time_scale<0;
        player_time_scale = BMEDIA_TIME_SCALE_BASE;
        direction = 0;
    } else {
        if(player->methods == &bmedia_player_generic) {
            rc = BERR_TRACE(BERR_NOT_SUPPORTED);
            goto error;
        }
        mode->continuous = false;
        mode->host_paced = true;
        mode->discontinuity = true;
    }
    if(mode->discontinuity) {
        if(player->time_scale == time_scale) {
            mode->discontinuity = false;
        }
    } else if(player->time_scale != time_scale) {
        mode->discontinuity = !(mode->continuous && player->last_continous);
    }
    if(mode->discontinuity) {
        bmedia_player_pos pos = BMEDIA_PLAYER_INVALID;

        if(player->methods != &bmpeg2ts_player_methods && player->methods != &bmpeg2pes_player_methods) {
            if(time_scale >= 0 && time_scale <= player->decoder_features.max_decoder_rate) {
                mode->host_paced = true;
                direction = 0;
            }
        }

        if(player->last_pts_valid) { /* get current position */
            bmedia_player_tell(player, &pos);
        }
        BDBG_MSG(("bmedia_player_set_direction: %#lx media_player %d %d", (unsigned long)player, (int)direction, (int)player_time_scale));

        mode->time_scale = time_scale;
        rc = player->methods->set_direction(player->player, direction, player_time_scale, mode);
        if(player_time_scale==BMEDIA_TIME_SCALE_BASE) {
            mode->time_scale = time_scale;
        }
        if(rc!=0) {rc=BERR_TRACE(rc);goto error;}
        if(pos != BMEDIA_PLAYER_INVALID) {
            bmedia_player_seek(player, pos); /* reseek player to the current position */
        }
        player->last_mode_valid = true;
        player->last_mode = *mode;
    } else if(player->last_mode_valid) {
        *mode = player->last_mode;
        mode->time_scale = time_scale; /* for STC trick, need to reapply the time_scale */
        mode->discontinuity = false;
    }

done:
    player->last_continous = mode->continuous;
    player->time_scale = player_time_scale;
error:
    BDBG_MSG(("bmedia_player_set_direction<: %#lx %s %d %d -> %d %s %s %s %s %s", (unsigned long)player, rc!=0?"ERROR":"", (int)direction, (int)time_scale, mode->time_scale, B_FLAG(mode,discontinuity), B_FLAG(mode, brcm), B_FLAG(mode, dqt), B_FLAG(mode, tsm), B_FLAG(mode, continuous)));
    return rc;
}

#define b_iabs(x) ((x)<0? -(x):(x))

void
bmedia_player_update_position(bmedia_player_t player, uint32_t pts)
{
#if defined (HUMAX_PLATFORM_BASE)
	uint32_t diff_pts = 0;
	uint32_t time = 0;
#endif
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);

    BDBG_MSG_FLOW(("bmedia_player_update_position>: %#lx PTS:%u", (unsigned long)player, (unsigned)pts));
    if (player->methods == &bmedia_player_generic) {
        /* if there's no index, don't update the index offset */
        player->last_pts = pts;
        player->last_pts_valid = true;
        return;
    } else if(player->methods == &bmedia_player_nav) {
        bmedia_player_nav_update_position(player->player, pts);
        return;
    } else if(player->methods == &bmpeg2ts_player_methods) {
        player->last_pts_valid = true;
        bmpeg2ts_player_update_position(player->player, pts);
        return;
    } else if(player->methods == &bmpeg2pes_player_methods) {
        player->last_pts_valid = true;
        bmpeg2pes_player_update_position(player->player, pts);
        return;
    }

    BDBG_MSG_FLOW(("bmedia_player_update_position>: %#lx PTS:%u %s%u %u POS:%u", (unsigned long)player, pts, player->last_pts_valid?"":"INVALID:", player->last_pts, pts - player->last_pts, player->index_offset + bmedia_pts2time(pts, player->time_scale)));
    if (!player->last_pts_valid || pts >= player->last_pts) { /* there was no wrap in the pts values */
        goto done;
    }
#if defined (HUMAX_PLATFORM_BASE)
	if((pts < player->last_pts) && player->time_scale<0){
		diff_pts = player->last_pts-pts;
		time = bmedia_pts2time(diff_pts, -player->time_scale);
		if(time < 100){
			pts = player->last_pts;
			goto done;
		}
	}
#endif

    BDBG_MSG(("bmedia_player_update_position: %#lx pts_wrap %u %u %u %u", (unsigned long)player, pts, player->last_pts, pts - player->last_pts, (uint32_t)(BMEDIA_PTS_MODULO-1)-player->last_pts));
    if (
        ((uint32_t)(BMEDIA_PTS_MODULO-1) -  player->last_pts) <  /* distance between last PTS and wrap pointer */
        (3*(BMEDIA_UPDATE_POSITION_INTERVAL*45)) /* 3 times of polling timer */
       ) { /* real wrap */
        uint32_t delta;

        delta = bmedia_pts2time((uint32_t)(BMEDIA_PTS_MODULO-1), b_iabs(player->time_scale));
        BDBG_MSG_FLOW(("bmedia_player_update_position: %#lx media index_offset %lu %s%lu", (unsigned long)player, (unsigned long)player->index_offset, player->time_scale<0?"-":"", (unsigned long)delta));
        /* there was a wrap of pts */
        if (player->time_scale >= 0) {
            player->index_offset += delta;
        } else {
            player->index_offset -= delta;
        }
    } else {
        BDBG_WRN(("bmedia_player_update_position: %#lx out of order PTS: %u %u %u %u", (unsigned long)player, pts, player->last_pts, pts - player->last_pts, (uint32_t)(BMEDIA_PTS_MODULO-1)-player->last_pts));
    }

done:
    BDBG_MSG_FLOW(("bmedia_player_update_position<: %#lx POS:%u", (unsigned long)player, player->index_offset + bmedia_pts2time(pts, player->time_scale)));
    player->last_pts = pts;
    player->last_pts_valid = true;
    return;
}

int
bmedia_player_seek(bmedia_player_t player, bmedia_player_pos pos)
{
    int rc;
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    BDBG_MSG_FLOW(("bmedia_player_seek>: %#lx %lu",  (unsigned long)player, pos));

    switch (player->stream_format) {
    case bstream_mpeg_type_avi:
	case bstream_mpeg_type_asf:
        if ((player->methods == &bmedia_player_generic) && (pos != 0)){
			BDBG_ERR(("seeking in stream format %d without index is not supported", player->stream_format));
			return -1;
        }
        break;
    default:
        break;
    }

    rc = player->methods->seek(player->player, pos);
    player->last_pts_valid = false;
    if(player->methods == &bmedia_player_nav) {
        /* no-op */
    } else if(player->methods == &bmpeg2ts_player_methods || player->methods == &bmpeg2pes_player_methods) {
        /* no op */
    } else {
        /* set stream position for PTS 0 */
        uint32_t delta;

        delta = bmedia_pts2time((uint32_t)(BMEDIA_PTS_MODULO-1), b_iabs(player->time_scale));
        if(delta==0)  { rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done; }
        BDBG_MSG_FLOW(("bmedia_player_seek: %#lx %ld %lu %lu (%lu->%lu)",  (unsigned long)player, player->time_scale, pos, delta, player->index_offset, (pos/delta)*delta));
        player->index_offset = (pos/delta)*delta;
        player->last_pts = bmedia_time2pts(pos, player->time_scale); /* save the expected PTS value, it's latter used in bmedia_player_update_position, to detect wrap in PTS values */
        if(rc==0) {
            player->last_position = pos;
        }
    }
done:
    BDBG_MSG_FLOW(("bmedia_player_seek<: %#lx %lu %d",  (unsigned long)player, pos, rc));
    return rc;
}

void
bmedia_player_get_decoder_config (bmedia_player_t player, bmedia_player_decoder_config *config)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    BDBG_ASSERT(config);
    *config  = player->decoder_features;
    if(player->methods == &bmedia_player_nav) {
        bmedia_player_nav_get_decoder_config(player->player, config);
    }
    return;
}


int
bmedia_player_set_decoder_config (bmedia_player_t player, const bmedia_player_decoder_config *config)
{
    int rc=0;

    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    BDBG_ASSERT(config);

    if(player->methods == &bmedia_player_nav) {
        rc = bmedia_player_nav_set_decoder_config(player->player, config);
    } else{
        if(config->host_mode!=bmedia_player_host_trick_mode_auto) {
            BDBG_ERR(("bmedia_player_set_decoder_config:%#lx mode %u not supported", (unsigned long)player, (unsigned)config->host_mode));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        player->decoder_features = *config;
    }

    return rc;
}

#else /* B_HAS_NAV_PLAYER */

int
bmedia_player_lookup_pts( bmedia_player_t player, bmedia_player_pos pos, uint32_t *p_pts )
{
    *p_pts = bmedia_time2pts(pos, player->time_scale);
    return 0;
}

int
bmedia_player_set_direction(bmedia_player_t player, bmedia_player_step direction, bmedia_time_scale time_scale, bmedia_player_decoder_mode *mode)
{
    int rc;
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    BSTD_UNUSED(mode);
    rc = player->methods->set_direction(player->player, direction, time_scale, mode);
    if(rc!=0) {rc=BERR_TRACE(rc);}
    return rc;
}

int
bmedia_player_seek(bmedia_player_t player, bmedia_player_pos pos)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    return player->methods->seek(player->player, pos);
}

void
bmedia_player_tell(bmedia_player_t player, bmedia_player_pos *pos)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);
    player->methods->tell(player->player, pos);
    return;
}

#define b_iabs(x) ((x)<0? -(x):(x))

void
bmedia_player_update_position(bmedia_player_t player, uint32_t pts)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_t);


    
    BDBG_MSG_FLOW(("bmedia_player_update_position>: %#lx PTS:%u", (unsigned long)player, (unsigned)pts));
    if (player->methods == &bmedia_player_generic) {
        
    
        /* if there's no index, don't update the index offset */
        player->last_pts = pts;
        player->last_pts_valid = true;
        return;
    } else if(player->methods == &bmpeg2ts_player_methods) {
    
        player->last_pts_valid = true;
        bmpeg2ts_player_update_position(player->player, pts);
    
        return;
    } else if(player->methods == &bmpeg2pes_player_methods) {
        

        player->last_pts_valid = true;
        bmpeg2pes_player_update_position(player->player, pts);
        return;
    }

    BDBG_MSG_FLOW(("bmedia_player_update_position>: %#lx PTS:%u %s%u %u POS:%u", (unsigned long)player, pts, player->last_pts_valid?"":"INVALID:", player->last_pts, pts - player->last_pts, player->index_offset + bmedia_pts2time(pts, player->time_scale)));
    if (!player->last_pts_valid || pts >= player->last_pts) { /* there was no wrap in the pts values */
        goto done;
    }

    BDBG_MSG(("bmedia_player_update_position: %#lx pts_wrap %u %u %u %u", (unsigned long)player, pts, player->last_pts, pts - player->last_pts, (uint32_t)(BMEDIA_PTS_MODULO-1)-player->last_pts));
    if (
        ((uint32_t)(BMEDIA_PTS_MODULO-1) -  player->last_pts) <  /* distance between last PTS and wrap pointer */
        (3*(BMEDIA_UPDATE_POSITION_INTERVAL*45)) /* 3 times of polling timer */
       ) { /* real wrap */
        uint32_t delta;


        delta = bmedia_pts2time((uint32_t)(BMEDIA_PTS_MODULO-1), b_iabs(player->time_scale));
        BDBG_MSG_FLOW(("bmedia_player_update_position: %#lx media index_offset %lu %s%lu", (unsigned long)player, (unsigned long)player->index_offset, player->time_scale<0?"-":"", (unsigned long)delta));
        /* there was a wrap of pts */
        if (player->time_scale >= 0) {
            player->index_offset += delta;
        } else {
            player->index_offset -= delta;
        }
    } else {
        BDBG_WRN(("bmedia_player_update_position: %#lx out of order PTS: %u %u %u %u", (unsigned long)player, pts, player->last_pts, pts - player->last_pts, (uint32_t)(BMEDIA_PTS_MODULO-1)-player->last_pts));
    }

done:
    BDBG_MSG_FLOW(("bmedia_player_update_position<: %#lx POS:%u", (unsigned long)player, player->index_offset + bmedia_pts2time(pts, player->time_scale)));
    player->last_pts = pts;
    player->last_pts_valid = true;
    return;
}


void
bmedia_player_get_decoder_config (bmedia_player_t player, bmedia_player_decoder_config *config)
{
    BSTD_UNUSED(player);
    BSTD_UNUSED(config);
    return;
}


int
bmedia_player_set_decoder_config (bmedia_player_t player, const bmedia_player_decoder_config *config)
{
    BSTD_UNUSED(player);
    BSTD_UNUSED(config);
    return 0;
}
#endif /* B_HAS_NAV_PLAYER */


bool
bmedia_player_stream_test(const bmedia_player_stream *stream, unsigned sub_stream)
{
    unsigned i;
    BDBG_ASSERT(stream);
    if (sub_stream == stream->master) {
        return true;
    }
    for(i=0;i<BMEDIA_PLAYER_MAX_TRACKS;i++) {
        if(sub_stream == stream->other[i]) {
            return true;
        }
    }
    return false;
}

uint8_t
bmedia_player_stream_get_id(const bmedia_player_stream *stream, unsigned sub_stream)
{
    unsigned i;
    BDBG_ASSERT(stream);
    if (sub_stream == stream->master) {
        return stream->stream.id.master;
    }
    for(i=0;i<BMEDIA_PLAYER_MAX_TRACKS;i++) {
        if(sub_stream == stream->other[i]) {
            return stream->stream.id.other[i];
        }
    }
    return 0;
}

int
bmedia_player_pos_delta(bmedia_player_pos future, bmedia_player_pos past)
{
    int delta = (int)future-(int)past;
    return delta;
}

bool
bmedia_player_pos_in_range(bmedia_player_pos future, bmedia_player_pos past, bmedia_player_pos range)
{
    int delta = (int)future-(int)past;
    if( (delta>=0 && (bmedia_player_pos)delta<=range) || (delta<0 && (bmedia_player_pos)(-delta)<=range)) {
        return true;
    }
    return false;
}

void
bmedia_player_init_status(bmedia_player_status *status)
{
    status->position = 0;
    status->bounds.first = 0;
    status->bounds.last = 0;
    status->direction = 0;
    status->index_error_cnt = 0;
    status->data_error_cnt = 0;
    status->format = bstream_mpeg_type_unknown;
    return;
}

void
bmedia_player_init_entry(bmedia_player_entry *entry)
{
    BKNI_Memset(entry, 0, sizeof(*entry));
    entry->type = bmedia_player_entry_type_no_data;
    entry->content = bmedia_player_entry_content_unknown;
    return;
}

void bmedia_player_init_sync_entry(bmedia_player_sync_entry *entry)
{
    BKNI_Memset(entry, 0, sizeof(*entry));
    return;
}

