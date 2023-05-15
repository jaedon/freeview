/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_player_generic.c $
 * $brcm_Revision: 20 $
 * $brcm_Date: 2/18/10 12:05p $
 *
 * Module Description:
 *
 * BMedia library, generic(no index) player
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_player_generic.c $
 * 
 * 20   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 * 
 * 19   2/2/10 12:55p vsilyaev
 * SW7408-63: Don't seend trailing zeros for WAV files
 * 
 * 18   7/8/09 11:51a vsilyaev
 * PR 55989: Added modes to support OTF and fragmented trickmodes
 * 
 * 17   6/23/09 5:51p vsilyaev
 * PR 55845: Added content type for the media player entry, this fixes key
 * frame extraction for AVI 2.0 (OpenDML) streams
 * 
 * 16   6/19/09 3:47p vsilyaev
 * PR 55989: Added decode_mode  paramtewr to to the set_direction method
 * 
 * 15   6/4/09 4:04p erickson
 * PR54129: change length to multiple of 188 and BIO_BLOCK_SIZE
 *
 * 14   2/11/09 2:56p vsilyaev
 * PR 38073: Removed unreachable code
 *
 * 13   2/11/09 2:52p vsilyaev
 * PR 38073: Don't fail of streams where file size is unknown
 *
 * 12   1/20/09 2:38p erickson
 * PR50725: fix time scaling
 *
 * 11   12/19/08 3:57p vsilyaev
 * PR 50214: Allowed set_direction function to fail and added framework
 * for players to communicate about an errors
 *
 * 10   12/16/08 10:28a vsilyaev
 * PR 50148: Improved parsing of H.264 streams
 *
 * 9   6/5/08 4:30p vsilyaev
 * PR 43184: Don't detect EOS in the timeshifting mode
 *
 * 8   5/21/08 3:25p vsilyaev
 * PR 41960: Fixed handling of media files
 *
 * 7   5/16/08 12:13p vsilyaev
 * PR 42365: Added more debug output
 *
 * 6   5/13/08 12:15p vsilyaev
 * PR 42365: Fixed address used to preload last 4096 bytes of the stream
 *
 * 5   5/12/08 7:07p vsilyaev
 * PR 42365: Added code to send stuffing bytes at the end of payload
 *
 * 4   2/15/08 5:23p vsilyaev
 * PR 38073: Fixed warnings
 *
 * 3   12/20/07 3:40p vsilyaev
 * PR 38073: Fixed handling of MPEG-2 TS streams
 *
 * 2   12/20/07 10:29a vsilyaev
 * PR 38073: Updated to work with new bmedia_player
 *
 * PR38073/3   12/19/07 5:01p vsilyaev
 * PR 38073: Moved time tracking code for host paced mode into the
 * bmedia_player
 *
 * PR38073/2   12/18/07 6:45p vsilyaev
 * PR 38073: Added handling of no-index streams
 *
 * PR38073/1   12/18/07 5:02p vsilyaev
 * PR 38073: Improved code to handle MPEG-2 TS packets
 *
 * 1   12/10/07 4:34p vsilyaev
 * PR 38073: Generic (no index player)
 *
 *******************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bmedia_player.h"
#include "bmedia_player_generic.h"
#include "bkni.h"

BDBG_MODULE(bmedia_player_generic);
typedef struct bmedia_player_generic *bmedia_player_generic_t;

BDBG_OBJECT_ID(bmedia_player_generic);

/* 256 bytes of filling */
static const uint8_t bmedia_player_generic_stuffing[256] = {0};

struct bmedia_player_generic {
    BDBG_OBJECT(bmedia_player_generic)
    enum {bmedia_player_generic_mode_file, bmedia_player_generic_mode_stuffing} mode;
    off_t offset;
    size_t  length;
    size_t  stuffing_bytes;
    size_t  stuffing_offset;
    bfile_io_read_t fd;
    bmedia_player_config config;
    bmedia_player_stream stream;
    bmedia_player_step direction;
    bmedia_time_scale time_scale;
    off_t first, last; /* offset of the file */
    uint8_t last_block[BIO_BLOCK_SIZE+BIO_BLOCK_SIZE]; /* we need to get BIO_BLOCK_SIZE alligned block in memory */
};

static bmedia_player_generic_t
bmedia_player_generic_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream)
{
    BERR_Code rc;
    bmedia_player_generic_t  player;

    BSTD_UNUSED(rc);
    BDBG_ASSERT(fd);
    BDBG_ASSERT(config);
    BDBG_ASSERT(stream);

    if(stream->stream.noindex.bitrate==0) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_stream;
    }
    player = BKNI_Malloc(sizeof(*player));
    if(!player) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(player, bmedia_player_generic);
    player->config = *config;
    player->stream = *stream;
    player->length = (188/4) * BIO_BLOCK_SIZE;
    player->stuffing_bytes = 16384;
    player->stuffing_offset = 0;
    player->fd = fd;
    player->mode = bmedia_player_generic_mode_file;
    if(player->fd->bounds(player->fd, &player->first, &player->last)<0) {
        player->first = 0;
        player->last = ((uint64_t) 1)<<62; /* some very large number */
    }
    player->offset = player->first;

    return player;

err_alloc:
err_stream:
    return NULL;
}

static void
bmedia_player_generic_destroy(bmedia_player_generic_t player)
{
    BDBG_OBJECT_DESTROY(player, bmedia_player_generic);
    BKNI_Free(player);
    return;
}

static int
bmedia_player_generic_next(bmedia_player_generic_t player, bmedia_player_entry *entry)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_generic);
    bmedia_player_init_entry(entry);
    switch(player->mode) {
    default:
        BDBG_ASSERT(0);
        /* keep going */
    case bmedia_player_generic_mode_file:
        entry->start = player->offset;
        entry->type = bmedia_player_entry_type_file;
        entry->embedded = NULL;

        if((off_t)(player->offset + player->length) <= player->last || player->config.timeshifting) {
            entry->length = player->length;
            player->offset += player->length;
            break;
        }
        if(player->offset < player->last) { /* sending last packet */
            entry->length = player->last - player->offset;
            entry->length -= entry->length%BIO_BLOCK_SIZE; /* ensure that block is alligned */
            player->offset += entry->length;
            if(entry->length>0) {
                break;
            }
            if(player->offset!=player->last) { /* we need to read last bytes, since playback rejects not BIO_BLOCK_SIZE reads, try to read it here  */
                ssize_t rc;
                uint8_t *last_block = player->last_block;
                off_t offset;

                offset = player->offset;
                player->offset = player->last;
                last_block += BIO_BLOCK_SIZE - 1;
                last_block -= (((unsigned long)last_block) % BIO_BLOCK_SIZE);
                BDBG_MSG(("bmedia_player_generic_next:%#lx alligning addr %#lx to %#lx", (unsigned long)player, (unsigned long)player->last_block, (unsigned long)last_block));
                player->fd->seek(player->fd, offset, SEEK_SET);
                rc = player->fd->read(player->fd, last_block, BIO_BLOCK_SIZE);
                BDBG_MSG(("bmedia_player_generic_next:%#lx read %u bytes from %lu to %#lx[%02x,%02x,%02x,%02x]", (unsigned long)player, (unsigned long)rc, (unsigned long)offset, (unsigned long)last_block, last_block[0], last_block[1], last_block[2], last_block[3]));
                player->fd->seek(player->fd, offset, SEEK_SET);
                if(rc>=0) {
                    entry->start = player->offset;
                    entry->type  = bmedia_player_entry_type_embedded;
                    entry->embedded = last_block;
                    entry->length = (size_t)rc;
                    break;
                }
            }
        }
        player->mode = bmedia_player_generic_mode_stuffing;
        player->stuffing_offset = 0;
        /* keep going */
    case bmedia_player_generic_mode_stuffing:
        if(player->stuffing_offset >= player->stuffing_bytes) {
            entry->type = bmedia_player_entry_type_end_of_stream;
            return 0; /* sent all data */
        }
        switch(player->stream.format) {
        default:
        case bstream_mpeg_type_es:
        case bstream_mpeg_type_ts:
        case bstream_mpeg_type_vob:
        case bstream_mpeg_type_pes:
        case bstream_mpeg_type_dss_es:
        case bstream_mpeg_type_dss_pes:
        case bstream_mpeg_type_mpeg1:
            entry->start = 0;
            entry->type  = bmedia_player_entry_type_embedded;
            entry->embedded = bmedia_player_generic_stuffing;
            entry->length = sizeof(bmedia_player_generic_stuffing);
            BDBG_MSG(("bmedia_player_generic_next: %#lx stuffing_offset %u:%u", (unsigned long)player, player->stuffing_offset, player->stuffing_bytes));
            player->stuffing_offset += sizeof(bmedia_player_generic_stuffing);
            break;
        case bstream_mpeg_type_asf:
        case bstream_mpeg_type_avi:
        case bstream_mpeg_type_flv:
        case bstream_mpeg_type_wav:
            /* for media files we can't just add 0, since this would greatly upset parsers, however we need to indicate EOS, so we send empty packet here */
            entry->start = 0;
            entry->type  = bmedia_player_entry_type_embedded;
            entry->embedded = bmedia_player_generic_stuffing;
            entry->length = 0;
            BDBG_MSG(("bmedia_player_generic_next: %#lx media EOS", (unsigned long)player));
            player->stuffing_offset = player->stuffing_bytes;
            break;
        }
        break;
    }
    return 0;
}

static void
bmedia_player_generic_tell(bmedia_player_generic_t player, bmedia_player_pos *pos)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_generic);
    *pos = (bmedia_player_pos)((BMEDIA_PLAYER_POS_SCALE*8*(uint64_t)player->offset)/player->stream.stream.noindex.bitrate);
    return;
}

static void
bmedia_player_generic_get_status(bmedia_player_generic_t player, bmedia_player_status *status)
{
    off_t first, last;
    BDBG_OBJECT_ASSERT(player, bmedia_player_generic);
    status->direction = player->direction;
    status->bounds.first = 0;
    status->bounds.last = 0;
    if(player->fd->bounds(player->fd, &first, &last)<0) { goto error; }
    status->bounds.first = (bmedia_player_pos) ((BMEDIA_PLAYER_POS_SCALE*8*(uint64_t)first)/player->stream.stream.noindex.bitrate);
    status->bounds.last = (bmedia_player_pos) ((BMEDIA_PLAYER_POS_SCALE*8*(uint64_t)last)/player->stream.stream.noindex.bitrate);
error:
    return;
}

static void
bmedia_player_generic_set_direction(bmedia_player_generic_t player, bmedia_player_step direction, bmedia_time_scale time_scale, bmedia_player_decoder_mode *mode)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_generic);
    BSTD_UNUSED(mode);
    player->direction = direction;
    player->time_scale = time_scale;
    return;
}

static int
bmedia_player_generic_seek(bmedia_player_generic_t player, bmedia_player_pos pos)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_generic);
    player->offset = (off_t)((pos*(uint64_t)player->stream.stream.noindex.bitrate)/(BMEDIA_PLAYER_POS_SCALE*8));
    player->offset -= player->offset%BIO_BLOCK_SIZE; /* keep offset alligned */
    BDBG_MSG(("bmedia_player_generic:%#lx pos %u bitrate %u offset %u", (unsigned long)player, (unsigned)pos, (unsigned)player->stream.stream.noindex.bitrate, (unsigned)player->offset));
    player->mode = bmedia_player_generic_mode_file;
    return 0;
}

static void *
b_media_player_generic_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream)
{
    return bmedia_player_generic_create(fd, config, stream);
}

static void
b_media_player_generic_destroy(void *player)
{
    bmedia_player_generic_destroy(player);
    return;
}

static int
b_media_player_generic_next(void *player, bmedia_player_entry *entry)
{
    return bmedia_player_generic_next(player, entry);
}

static void
b_media_player_generic_tell(void *player, bmedia_player_pos *pos)
{
    bmedia_player_generic_tell(player, pos);
    return;
}

static void
b_media_player_generic_get_status(void *player, bmedia_player_status *status)
{
    bmedia_player_generic_get_status(player, status);
    return;
}

static int
b_media_player_generic_set_direction(void *player, bmedia_player_step direction, bmedia_time_scale time_scale, bmedia_player_decoder_mode *mode)
{
    bmedia_player_generic_set_direction(player, direction, time_scale, mode);
    return 0;
}

static int
b_media_player_generic_seek(void *player, bmedia_player_pos pos)
{
    return bmedia_player_generic_seek(player, pos);
}

const bmedia_player_methods bmedia_player_generic = {
    b_media_player_generic_create,
    b_media_player_generic_destroy,
    b_media_player_generic_next,
    b_media_player_generic_tell,
    b_media_player_generic_get_status,
    b_media_player_generic_set_direction,
    b_media_player_generic_seek
};

