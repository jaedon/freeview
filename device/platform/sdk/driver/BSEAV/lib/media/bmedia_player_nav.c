/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $bcm_Workfile: bmedia_player_nav.c $
 * $bcm_Revision: 1 $
 * $bcm_Date: 12/10/07 4:34p $
 *
 * Module Description:
 *
 * BMedia library, MPEG2-TS NAV player
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_player_nav.c $
 * 
 * 62   10/17/12 10:37a erickson
 * SW7425-3594: don't force a source rate because we assume decoder is
 * doing SW STC TSM on top of host trick mode
 * 
 * 61   10/9/12 3:14p erickson
 * SW7425-3594: default simulated_tsm = false
 * 
 * 60   9/4/12 4:45p erickson
 * SW7425-3594: adaptive rate trick modes requests simulated_tsm for
 * precision rate
 * 
 * 59   8/28/12 3:23p erickson
 * SW7346-994: don't rely on bcmplayer BNAV_Version detection because it's
 * not possible for file fifos with AVC
 * 
 * 58   6/11/12 12:34p erickson
 * SW7358-325: default navVersion to BNAV_Version_TimestampOnly if there
 * is no video codec
 * 
 * 57   6/4/12 5:41p vsilyaev
 * SW7425-1301: Properly indentify MPEG-2 streams/indices
 * 
 * 56   3/2/12 12:14p erickson
 * SW7425-2282: add BNAV_Player_Settings.strictFind to restore previous
 * behavior for BNAV_Player_FindIndexFromTimestamp, also extended to
 * BNAV_Player_FindIndexFromOffset
 * 
 * 55   2/21/12 3:43p erickson
 * SW7425-2439: set default BNAV_Version for timeshifting playback, clean
 * up bmedia_player_nav_get_status for failure or partial status
 * conditions
 * 
 * 54   2/14/12 4:06p erickson
 * SW7425-2363: propagate failure from BNAV_Player_Reset
 * 
 * 53   2/7/12 4:05p erickson
 * SW7425-2282: force bcmplayer to update bounds for non-timeshifting case
 *
 * 52   2/3/12 4:15p erickson
 * SW7425-2282: set BNAV_Player_Settings.changingBounds, don't print
 * BERR_TRACE for out of bounds seek
 *
 * 51   1/31/12 11:55a jtna
 * SW7335-1301: do not assume lower bound of 0 for wrapped fifo record
 * files
 *
 * 50   11/9/11 2:14p erickson
 * SW7335-1368: avoid round to zero
 *
 * 49   9/22/11 3:33p erickson
 * SW7425-1205: change default bmedia_player_host_trick_mode_auto to be
 * "max slow motion" based on customer request. if we get multiple
 * requests in the future, we will need to expose a param
 *
 * 48   8/29/11 12:17p erickson
 * SW7425-1205: add bmedia_player_host_trick_mode_auto option for max slow
 * motion, which results in higher unique picture fps
 *
 * 47   4/20/11 1:09p vsilyaev
 * SW7422-14: Handle case for the base only playback
 *
 * 46   3/23/11 3:00p vsilyaev
 * SW7422-14: Added facility to associate  stream id(pid) with a chunk of
 * payload
 *
 * 45   2/25/11 12:46p erickson
 * SW7125-818: use BNAV_GetEntrySize
 *
 * 44   12/3/10 2:20p ahulse
 * SW7335-939: Disable sanitycheck on first opening bcmplayer, do check
 * after nav version has been detected.
 *
 * 43   10/18/10 1:05p erickson
 * SW7405-4941: fix bmedia_player_nav_set_direction for STC trick in _auto
 * mode
 *
 * 42   10/15/10 10:44a erickson
 * SW7405-4941: fix bmedia_player_nav_set_direction for slow reverse.
 * refactor to remove left table-based trick mode logic.
 *
 * 41   9/28/10 1:56p erickson
 * SWDEPRECATED-1003: add
 * bmedia_player_decoder_mode.force_source_frame_rate
 *
 * 40   9/9/10 1:03p erickson
 * SW3548-3062: add bmedia_player_config.data_file_size so that player can
 * truncate index if it extends past the data file
 *
 * 39   9/2/10 4:26p erickson
 * SWDEPRECATED-1003: replace lookup tables with
 * bmedia_player_host_trick_mode_time_skip
 *
 * 38   8/26/10 3:24p dlwin
 * SW7125-603: Address Coverity 22331 issue for uninitialized fields in a
 * structure.
 *
 * 37   4/9/10 7:33a erickson
 * SW7405-4178: add debug or workaround code by selecting decoder trick
 * mode which corresponds to the host trick mode
 *
 * 36   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 *
 * 35   11/12/09 4:49p vsilyaev
 * SW7400-2604: Don't set position that outside of the current bounds of
 * the index file
 *
 * 34   11/5/09 7:32p vsilyaev
 * SW7400-2593: Updated lookup table and handling of slow rewind modes
 *
 * 33   11/4/09 3:17p vsilyaev
 * SW7400-2591: Added interpolation for slow_motion_rate
 *
 * 32   11/4/09 12:36p vsilyaev
 * SW7400-2591: Don't select skipP mode for AVC streams
 *
 * 31   9/24/09 1:40p gmohile
 * SW7405-2659 : DQT should be enabled for reverse trick modes
 *
 * 30   7/31/09 11:36a vsilyaev
 * PR 56688: When openining index seek player to the first avaliable entry
 *
 * 29   7/13/09 11:40a ahulse
 * PR56762: On decoder flush, set backwards search. Check if B frame on
 * Brm reverse to apply correct inc/decrement
 *
 * 28   7/8/09 11:51a vsilyaev
 * PR 55989: Added modes to support OTF and fragmented trickmodes
 *
 * 27   6/23/09 5:51p vsilyaev
 * PR 55845: Added content type for the media player entry, this fixes key
 * frame extraction for AVI 2.0 (OpenDML) streams
 *
 * 26   6/19/09 4:56p vsilyaev
 * PR 56169: Allow fast decode trickmodes even without STC trickmodes
 *
 * 25   6/4/09 4:05p erickson
 * PR54129: use bcmplayer's default normalPlayBufferSize which is a
 * multiple of 188 and 4096
 *
 * 24   5/13/09 10:04a erickson
 * PR54400: handle error from BNAV_Player_GetPosition differently
 *
 * 23   5/13/09 9:50a ahulse
 * PR55073: Apply correct slo mo rate from table
 *
 * 22   5/12/09 4:44p erickson
 * PR54400: check return code
 *
 * 21   4/27/09 2:38p erickson
 * PR54129: modify b_media_player_nav_set_location to not require I frame.
 * clarify usage of BNAV_Player_FindIFrameFromIndex.
 *
 * 20   4/16/09 1:31p erickson
 * PR54129: remember last succesful seek or tell position. use that if
 * tell fails. this gives better position info for circular record where
 * the beginning of the index might be truncated.
 *
 * 19   3/27/09 12:51p vsilyaev
 * PR 50311: Return readPosition in the Nexus playback status. In
 * combination with the position it could be used to measure depth of
 * decoder and playback buffers in units of time.
 *
 * 18   2/20/09 1:37p erickson
 * PR42679: fix DBG code
 *
 * 17   2/2/09 2:53p gmohile
 * PR 47815 : Add manual control of trickmodes
 *
 * 16   12/8/08 11:20a erickson
 * PR49930: add bmedia_player_lookup_pts
 *
 * 15   10/28/08 9:40a vsilyaev
 * PR 48386: Use previous index if index can't be acquired from PTS
 *
 * 14   10/13/08 10:44a erickson
 * PR47572: downgrade DBG to MSG
 *
 * 13   10/7/08 5:21p vsilyaev
 * PR 47122: PR 47570: Added code to properly handle decoder fast forward
 * trickmodes
 *
 *******************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bmedia_player.h"
#include "bmedia_player_nav.h"
#include "bkni.h"
#include "bcmplayer.h"
#include "bcmindexer.h"
#include "bcmindexer_nav.h"

BDBG_MODULE(bmedia_player_nav);

#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */


BDBG_OBJECT_ID(bmedia_player_nav);

struct bmedia_player_nav {
    BDBG_OBJECT(bmedia_player_nav)
    bfile_io_read_t fd;
    BNAV_Player_Handle bcm_player;
    bmedia_player_config config;
    bmedia_player_stream stream;
    bmedia_player_step direction;
    bmedia_time_scale time_scale;
    bool pts_valid; /* if PTS is valid */
    bool index_valid; /* if current index */
    uint32_t pts; /* current pts */
    long index; /* current index */
    eBpPlayModeParam mode; /* current mode */
    size_t index_entrysize; /* the sizeof each index entry. depends on BNAV_Version. */
    unsigned index_file_size; /* max accessible size of index file */
    unsigned long last_timestamp;       /* last timestamp in index file - playback duration in mSec */
    unsigned long last_position; /* last position of player, used in case tell fails */
    uint8_t ts_buf[188+4]; /* buffer to hold generated data injected by the bcmplayer */
};

#define GET_INDEX_FILEIO(fp) (((bmedia_player_nav_t)fp)->fd)
#define B_GET_OFF_T(hi,lo) ((lo)|(((off_t)(hi))<<32))

static long
bp_read(void *buffer, long size, long count, void *fp )
{
    bfile_io_read_t f = GET_INDEX_FILEIO(fp);

    return (long)f->read(f, buffer, (size_t)(count*size));
}

static long
bp_tell( void *fp )
{
    bfile_io_read_t f = GET_INDEX_FILEIO(fp);
    return (long)f->seek(f, 0, SEEK_CUR);
}

static int
bp_seek( void *fp, long offset, int origin )
{
    bfile_io_read_t f = GET_INDEX_FILEIO(fp);
    off_t rc = f->seek(f, (off_t)offset, origin);
    if ( rc == (off_t)-1) {
        return -1;
    }
    return 0;
}

#if defined(HUMAX_PLATFORM_BASE)
#define MAX_LAST_INDEX_CHECK_COUNT 10
#endif

static int
bp_bounds(BNAV_Player_Handle handle, void *fp, long *firstIndex, long *lastIndex)
{
    bmedia_player_nav_t player = fp;
    bfile_io_read_t f = GET_INDEX_FILEIO(fp);
    off_t first, last;
    unsigned index_entrysize = player->index_entrysize;
    int rc;

    BSTD_UNUSED(handle);
    rc = f->bounds(f, &first, &last);
    if (player->index_file_size) {
        /* we cannot assume first=0 for wrapped fifo record files. we must take what was populated via f->bounds() */
        last = player->index_file_size;
    }
    BDBG_ASSERT(index_entrysize);
    *firstIndex = first/index_entrysize;
    *lastIndex = (last-1)/index_entrysize;

#if defined(HUMAX_PLATFORM_BASE)
	/* if data of last index is broken(fill in with 0x00) because of power of loss, find valid last index and apply */
	if(player->bcm_player != NULL) {	
	    int lastIndexErrorCnt = 0;
		BNAV_Player_Settings settings;
			
		lastIndexErrorCnt = BNAV_Player_GetLastIndexStatus(player->bcm_player);
	    if(lastIndexErrorCnt > 0 && lastIndexErrorCnt < MAX_LAST_INDEX_CHECK_COUNT) {	    	
	    	BNAV_Player_GetSettings(player->bcm_player, &settings);    	
	    	if(*lastIndex-lastIndexErrorCnt == settings.lastIndex) {
	    		BDBG_ERR(("original last index [%ld] is broken. change to valid LastIndex [%ld]. Number of invalid index [%d]",*lastIndex, settings.lastIndex, lastIndexErrorCnt));
	    		*lastIndex = settings.lastIndex;
	    	}   	
	    }
	}
#endif
    return rc;
}

/* copied from NEXUS_P_ReadFirstNavEntry in nexus_file.c */
static int
b_read_first_naventry(bmedia_player_nav_t player, BNAV_Entry *pNavEntry)
{
    bfile_io_read_t f = GET_INDEX_FILEIO(player);
    off_t rc;

    rc = f->seek(f, 0, SEEK_SET);
    if ( rc == (off_t)-1) {
        return -1;
    }

    rc = f->read(f, pNavEntry, sizeof(*pNavEntry));
    if (rc != sizeof(*pNavEntry)) {
        return BERR_TRACE(-1);
    }
    
    return 0;
}

static bmedia_player_nav_t
bmedia_player_nav_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream)
{
    bmedia_player_nav_t  player;
    static const BNAV_DecoderFeatures features = {1,1,1,1};
    BNAV_Player_Settings cfg;
    BNAV_Player_PlayMode playMode;
    BNAV_Player_Position position;
    BERR_Code rc;
    long firstIndex, lastIndex;
    unsigned i;

    BSTD_UNUSED(fd);

    BDBG_ASSERT(config);
    BDBG_ASSERT(stream);

    BDBG_MSG_TRACE(("bmedia_player_nav_create:< %#lx", (unsigned long)fd));

    if(stream->stream.mpeg2ts.packet_size != 188 && stream->stream.mpeg2ts.packet_size != 188+4) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_packet_size;
    }

    player = BKNI_Malloc(sizeof(*player));
    if(!player) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(player, bmedia_player_nav);
    player->config = *config;
    player->stream = *stream;
    player->fd = fd;
    player->direction = 1;
    player->time_scale = BMEDIA_TIME_SCALE_BASE;
    player->pts_valid = false;
    player->index_valid = false;
    player->pts = 0;
    player->index = 0;
    player->mode = eBpPlayNormal;
    player->last_position = 0;
    player->index_file_size = 0;

    BNAV_Player_GetDefaultSettings(&cfg);
    cfg.videoPid = stream->master;
    cfg.filePointer = player;
    cfg.decoderFeatures = features;
    cfg.readCb = bp_read;
    cfg.tellCb = bp_tell;
    cfg.seekCb = bp_seek;
    cfg.boundsCb = bp_bounds;
    cfg.firstIndex = -1; /* fifo file may have firstIndex > 0 */
    cfg.changingBounds = config->timeshifting;
    
    /* read the first index entry ourselves */
    {
        BNAV_Entry navEntry;
        rc = b_read_first_naventry(player, &navEntry);
        if (!rc) {
            cfg.navVersion = BNAV_get_version(&navEntry);
        }
    }
    
    /* if there is no index, we can set BNAV_Version based on codec because we know this will be a latest version. */
    if (cfg.navVersion == BNAV_VersionUnknown) {
        /* This logic is duplicated in nexus_record.c. It is required to avoid waiting for the first NAV entry when timeshifting.
        this is not needed because we know the index will be the latest version. */
        switch (stream->stream.es.video_codec) {
        case bvideo_codec_h264_svc:
        case bvideo_codec_h264_mvc:
            cfg.navVersion = BNAV_Version_AVC_Extended;
            break;
        case bvideo_codec_h264:
            cfg.navVersion = BNAV_Version_AVC;
            break;
        case bvideo_codec_mpeg2:
            cfg.navVersion = BNAV_Version2;
            break;
        case bvideo_codec_vc1:
            cfg.navVersion = BNAV_Version_VC1_PES;
            break;
        case bvideo_codec_avs:
            cfg.navVersion = BNAV_Version_AVS;
            break;
        case bvideo_codec_unknown:
            if (!stream->without_index) {
                /* must be audio index, which is TimestampOnly */
                cfg.navVersion = BNAV_Version_TimestampOnly;
            }
            break;
        default:
#if defined(HUMAX_PLATFORM_BASE)
            cfg.navVersion = BNAV_Version_TimestampOnly;
#endif
            break;
        }
        if (cfg.navVersion == BNAV_VersionUnknown) {
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err_nav_detection;
        }
    }
    player->index_entrysize = BNAV_GetEntrySize(cfg.navVersion);
#if !defined(HUMAX_PLATFORM_BASE)
    bp_bounds(player->bcm_player, player, &firstIndex, &lastIndex);
    BDBG_MSG(("NAV Table: version %d, size %d, bounds %lu..%lu", cfg.navVersion, player->index_entrysize, firstIndex, lastIndex));
#endif
    
    cfg.transportTimestampEnabled = stream->stream.mpeg2ts.packet_size != 188;
    for(i=0;i<BMEDIA_PLAYER_MAX_TRACKS;i++) {
        if(stream->stream.es.other_video[i] != bvideo_codec_unknown) {
            cfg.enhancementVideoPid = stream->other[i];
        }
    }
    cfg.strictFind = true;
    BDBG_MSG(("videoPid:%#x enhancementVideoPid:%#x", cfg.videoPid, cfg.enhancementVideoPid));
    if (BNAV_Player_Open(&player->bcm_player, &cfg)!=BERR_SUCCESS) {
        BDBG_ERR(("Unable to open index file. Check if it's empty."));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto error_player;
    }

#if defined(HUMAX_PLATFORM_BASE)
    bp_bounds(player->bcm_player, player, &firstIndex, &lastIndex);
    BDBG_MSG(("NAV Table: version %d, size %d, bounds %lu..%lu", cfg.navVersion, player->index_entrysize, firstIndex, lastIndex));
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
    /* Check whether is delay record file -> if delay record file, adjust timestamp/byte offset */
    if(!config->timeshifting || (config->delayedRecordingStatus & 0x00000001))
    {
    	if (BNAV_Player_GetPositionInformation(player->bcm_player, 0, &position)) {
    		BDBG_ERR(("unable to get first index: 0"));
    		rc = BERR_TRACE(BERR_UNKNOWN);
    		goto err_nav_player;
    	}
    
    	if(position.fifoRecorded)
    	{
    	    uint64_t offset;
    		
            if (BNAV_Player_SetCurrentIndex(player->bcm_player, 0)) {
            	rc = BERR_TRACE(BERR_UNKNOWN);
            	goto err_nav_player;
            }
            BNAV_Player_GetLastSeqHeaderOffset(player->bcm_player, &offset); /* cutOffset is packet size aligned */
            /*offset = (config->delayRecordedOffsetAlign)*(offset/(config->delayRecordedOffsetAlign));*/
            BNAV_Player_SetCorrectionValues(player->bcm_player, offset, position.timestamp);
    	}
    }
#endif

    /* reposition bcmplayer and force it to recompute bounds because bplayback's bp_bounds computes
        based on state.index_entrysize, which is only known now. */
    BNAV_Player_SetCurrentIndex(player->bcm_player, firstIndex);
    BNAV_Player_GetSettings(player->bcm_player, &cfg);

    if (BNAV_Player_GetPositionInformation(player->bcm_player, cfg.lastIndex, &position)) {
        BDBG_ERR(("unable to get last index: %d", cfg.lastIndex));
    } else {
        player->last_timestamp = position.timestamp;
    }

#if defined(HUMAX_PLATFORM_BASE)
	/* if data of last index is broken(fill in with 0x00) because of power of loss, find valid last index and apply */
	BNAV_Player_SetLastIndexStatus(player->bcm_player, 0);
	
	if(player->last_timestamp == 0) {
		long i; 
		int errorCount = 0;
				
		for(i=cfg.lastIndex; i>firstIndex && cfg.lastIndex-i < MAX_LAST_INDEX_CHECK_COUNT; i--) 
		{
			if (BNAV_Player_GetPositionInformation(player->bcm_player, i, &position)) {
				BDBG_ERR(("unable to get last index: %d", i));
				break;
			}

			if(position.timestamp != 0) {
				player->last_timestamp = position.timestamp;			
				BNAV_Player_SetBounds(player->bcm_player, firstIndex, i);
				BNAV_Player_SetLastIndexStatus(player->bcm_player,errorCount);
				break;
			} else {
				errorCount++;
				BDBG_ERR(("unable to get last index [%ld]. becuase last index data is empty.", i));
			}
		}			
	}
#endif

    BKNI_Memset(&playMode, 0, sizeof(playMode)); /* to address Coverity warning */
    playMode.playMode = player->mode;
    playMode.playModeModifier = 0;
    playMode.loopMode = eBpSinglePlay;

    if(BNAV_Player_SetPlayMode(player->bcm_player, &playMode)!=BERR_SUCCESS) {
        goto err_nav_player;
    }
    BDBG_MSG_TRACE(("bmedia_player_nav_create:> %#lx %#lx", (unsigned long)fd, (unsigned long)player));

    /* if we're given a fixed data file size, we can set a fixed index file size which also allows us to
    truncate index entries which extend past the data file */
    if (config->data_file_size) {
        long index = BNAV_Player_FindIndexFromOffset(player->bcm_player, config->data_file_size>>32, config->data_file_size&0xFFFFFFFF);
        if (index > 0) {
            player->index_file_size = index * player->index_entrysize;
        }
    }

    return player;

err_nav_player:
    BNAV_Player_Close(player->bcm_player);
error_player:
err_nav_detection:
    BKNI_Free(player);
err_alloc:
err_packet_size:
    BDBG_MSG_TRACE(("bmedia_player_nav_create:> %#lx %#lx", (unsigned long)fd, NULL));
    return NULL;
}

static void
bmedia_player_nav_destroy(bmedia_player_nav_t player)
{
    BDBG_MSG_TRACE(("bmedia_player_nav_destroy:> %#lx", (unsigned long)player));
    BDBG_OBJECT_ASSERT(player, bmedia_player_nav);
    BNAV_Player_Close(player->bcm_player);
    BDBG_OBJECT_DESTROY(player, bmedia_player_nav);
    BDBG_MSG_TRACE(("bmedia_player_nav_destroy:< %#lx", (unsigned long)player));
    BKNI_Free(player);
    return;
}

static int
bmedia_player_nav_next(bmedia_player_nav_t player, bmedia_player_entry *entry)
{
    BNAV_Player_PlayEntry bcmEntry;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(player, bmedia_player_nav);
    BDBG_ASSERT(entry);
    BDBG_MSG_TRACE(("bmedia_player_nav_next:> %#lx", (unsigned long)player));


    bmedia_player_init_entry(entry);
    if (BNAV_Player_GetNextPlayEntry(player->bcm_player, &bcmEntry, player->ts_buf)!=BERR_SUCCESS) {
        BDBG_MSG(("bmedia_player_nav_next: %#lx BNAV_Player_GetNextPlayEntry failed", (unsigned long)player->bcm_player));
        rc = BERR_NOT_SUPPORTED;  /* don't use trace it's normal when end of file is reached */
        entry->type = bmedia_player_entry_type_end_of_stream;
        goto done;
    }

    if (bcmEntry.isInsertedPacket) {
        entry->type = bmedia_player_entry_type_embedded;
        entry->content = bmedia_player_entry_content_header;
        entry->embedded = player->ts_buf;
        entry->start = 0;
        entry->length = player->stream.stream.mpeg2ts.packet_size;
    } else {
        entry->type = bmedia_player_entry_type_file;
        entry->content = bmedia_player_entry_content_payload;
        entry->embedded = NULL;
        entry->length = bcmEntry.byteCount;
        entry->start = B_GET_OFF_T(bcmEntry.startOffsetHi, bcmEntry.startOffset);
        /* this is needed to catch "negative" length value from bcmplayer. */
        if (entry->length & 0x80000000) {
            BDBG_ERR(("Bad byteCount %#lx from player", entry->start));
            rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto err_byte_count;
        }
    }
    entry->filteredPid = bcmEntry.filteredPid;
done:
    BDBG_MSG_TRACE(("bmedia_player_nav_next:< %#lx %s %#lx:%u", (unsigned long)player, entry->type==bmedia_player_entry_type_file?"file":"packet", (unsigned long)entry->start, (unsigned)entry->length));
    return 0;
err_byte_count:
    BDBG_MSG_TRACE(("bmedia_player_nav_next:< %#lx", (unsigned long)player));
    return -1;
}

#define PTS_WINDOW 400
static void
bmedia_player_nav_tell(bmedia_player_nav_t player, bmedia_player_pos *pos)
{
    BERR_Code rc;
    BNAV_Player_Position playerpos;
    long index;

    BDBG_MSG_TRACE(("bmedia_player_nav_tell:> %#lx", (unsigned long)player));
    BDBG_OBJECT_ASSERT(player, bmedia_player_nav);
    BDBG_ASSERT(pos);

    if(player->pts_valid) {
        index = BNAV_Player_FindIndexFromPts(player->bcm_player, player->pts, PTS_WINDOW);
        BDBG_MSG_TRACE(("bmedia_player_nav_tell: %#lx %ld", (unsigned long)player, index));
        if(index!=-1) {
            player->index = index;
            player->index_valid = true;
        } else {
            index = player->index;
            if(player->index_valid) {
                BDBG_MSG_TRACE(("bmedia_player_nav_tell: %#lx lost index location %ld", (unsigned long)player, player->index));
            }
            player->index_valid = false;
        }
        rc = BNAV_Player_GetPositionInformation(player->bcm_player, index, &playerpos);
        if (rc!=BERR_SUCCESS) {
            /* don't use BERR_TRACE. failed seek is normal. */
            goto default_position;
        }
        *pos = playerpos.timestamp;
    } else {
        goto default_position;
    }

    BDBG_MSG_TRACE(("bmedia_player_nav_tell:< %#lx %u", (unsigned long)player, *pos));
    player->last_position = *pos;
    return;

default_position:
    *pos = player->last_position;
    BDBG_MSG_TRACE(("bmedia_player_nav_tell:< %#lx default %u", (unsigned long)player, *pos));
    return;
}

static void
bmedia_player_nav_get_status(bmedia_player_nav_t player, bmedia_player_status *status)
{
    long first, last;
    int rc;

    BDBG_MSG_TRACE(("bmedia_player_nav_get_status:> %#lx", (unsigned long)player));

    BDBG_OBJECT_ASSERT(player, bmedia_player_nav);
    BKNI_Memset(status, 0, sizeof(*status));
    status->direction = player->direction;
    rc = bp_bounds(player->bcm_player, player, &first, &last);
    if (rc<0) { rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto err_position; }
    if (first || last) {
        BNAV_Player_Position playerpos;
        rc = BNAV_Player_GetPositionInformation(player->bcm_player, first, &playerpos);
        if (!rc) {
            status->bounds.first = playerpos.timestamp;
        }
        rc = BNAV_Player_GetPositionInformation(player->bcm_player, last, &playerpos);
        if (!rc) {
            status->bounds.last = playerpos.timestamp;
        }
        rc = BNAV_Player_GetPosition(player->bcm_player, &playerpos);
        if (!rc) {
            status->position = playerpos.timestamp;
        }
    }

err_position:
    BDBG_MSG_TRACE(("bmedia_player_nav_get_status:< %#lx", (unsigned long)player));
    return;
}

static int
b_sign(bmedia_player_step direction)
{
    if(direction==0) {return 0;}
    else if (direction<0) {return -1;}
    else return 1;
}

static eBpDirectionParam
b_nav_rev_direction(bmedia_player_step direction)
{
    if(direction>=0) return eBpReverse;
    else return eBpForward;
}


int
bmedia_player_nav_lookup_pts( bmedia_player_nav_t player, bmedia_player_pos pos, uint32_t *p_pts )
{
    BNAV_Player_Position position;
    long index;

    index = BNAV_Player_FindIndexFromTimestamp(player->bcm_player, pos);
    if (index != -1) {
        if (!BNAV_Player_GetPositionInformation(player->bcm_player, index, &position)) {
            *p_pts = position.pts;
            return 0;
        }
    }
    return -1;
}

static BERR_Code
b_media_player_nav_set_location(bmedia_player_nav_t player, long index, eBpPlayModeParam mode)
{
    BERR_Code rc = 0;

    BDBG_MSG_TRACE(("b_media_player_nav_set_location: %#lx %ld", (unsigned long)player, index));
    if(mode!=eBpPlayBrcm) {
        long i_frame_index;

        /* Search for the nearest random access point (i.e. I frame) in the opposite direction.
        This is not required for the decoder; it will simply discard data until it gets to a random access point.
        It's required for the user experience; the user doesn't want to lose content on a trick mode transition because decoding
        must start at a random access point. */
        i_frame_index = BNAV_Player_FindIFrameFromIndex(player->bcm_player, index, b_nav_rev_direction(-player->direction));
        if (i_frame_index == -1) {
            i_frame_index = BNAV_Player_FindIFrameFromIndex(player->bcm_player, index, b_nav_rev_direction(player->direction));
        }
        if (i_frame_index != -1) {
            index = i_frame_index;
        }
        /* else, if we can't find the I frame, we should still use the index. */
    }
    if (BNAV_Player_SetCurrentIndex(player->bcm_player, index)!=BERR_SUCCESS) { rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto err_set_index;}
    player->index_valid = true;
    player->index = index;
    player->pts_valid = false;
    return BERR_SUCCESS;

err_set_index:
    return rc;
}

int
bmedia_player_nav_set_direction(bmedia_player_nav_t player,
        bmedia_player_step direction,
        bmedia_time_scale time_scale, /* desired speed in BMEDIA_TIME_SCALE_BASE units */
        bmedia_player_decoder_mode *mode /* new decoder mode */
        )
{
    BNAV_Player_PlayMode playMode, prevPlayMode;
    BERR_Code rc;
    BNAV_Player_Settings bcmPlayerSettings;

    BDBG_MSG_TRACE(("bmedia_player_nav_set_direction:> %#lx %d %d", (unsigned long)player, (int)direction, (int)time_scale));
    BDBG_OBJECT_ASSERT(player, bmedia_player_nav);
    BDBG_ASSERT(mode);

    BNAV_Player_GetPlayMode(player->bcm_player, &playMode);
    prevPlayMode = playMode; /* save it for diff */

    /* set our defaults */
    playMode.loopMode = eBpSinglePlay;
    playMode.playModeModifier = 1;
    playMode.playMode = eBpPlayNormal;
    playMode.disableExtraBOptimization = 0;
    
    mode->simulated_tsm = false;

    if(player->config.decoder_features.host_mode == bmedia_player_host_trick_mode_auto){
        unsigned slow_motion_rate = 1;

        BNAV_Player_GetSettings(player->bcm_player, &bcmPlayerSettings);

        if (time_scale > 0 && time_scale <= BMEDIA_TIME_SCALE_BASE && !player->config.decoder_features.stc) {
            /* for non-STC slow motion (time_scale > 0 && time_scale < BMEDIA_TIME_SCALE_BASE) */
            playMode.playMode = eBpPlayNormal;
            playMode.playModeModifier = 1000;
            slow_motion_rate = BMEDIA_TIME_SCALE_BASE/time_scale; /* lacks precision. STC has greater precision. */
        }
        else if (time_scale >= 0 && (time_scale <= BMEDIA_TIME_SCALE_BASE || (player->config.decoder_features.stc && time_scale <= player->config.decoder_features.max_decoder_rate))) {
            /* applies to the following
            1) pause (time_scale == 0) should not occur, but we capture all possible div-by-zero here
            2) normal play (time_scale == BMEDIA_TIME_SCALE_BASE )
            3) stc trick within max_decoder_rate (e.g. 4x)
            */
            playMode.playMode = eBpPlayNormal;
            playMode.playModeModifier = 1000;
            slow_motion_rate = 0; /* slow motion will be with STC, so no decoder slow motion */
            mode->time_scale = time_scale; /* time_scale will be preserved because slow_motion_rate was set to 0 */
        }
        else if (player->config.decoder_features.brcm &&
            (bcmPlayerSettings.navVersion == BNAV_VersionOriginal  ||
            bcmPlayerSettings.navVersion == BNAV_VersionOpenGopBs || 
            bcmPlayerSettings.navVersion == BNAV_Version2) && 
#if defined(HUMAX_PLATFORM_BASE) && defined (CONFIG_PVR_PLAYMODE_TIMESKIP)
            time_scale >= -1*BMEDIA_TIME_SCALE_BASE && time_scale < 0)
#else
            time_scale >= -4*BMEDIA_TIME_SCALE_BASE && time_scale < 0)
#endif
        {
            /* for MPEG reverse up to -4x we prefer BRCM trick modes.
            we could use BRCM trick modes for fwd speeds up to 4x, but we prefer STC trick. */
            playMode.playMode = eBpPlayBrcm;
            if (time_scale < -BMEDIA_TIME_SCALE_BASE) {
                playMode.playModeModifier = time_scale/BMEDIA_TIME_SCALE_BASE;
                slow_motion_rate = 1;
            }
            else {
                playMode.playModeModifier = -1; /* for slow rewind, set host to -1x. */
                slow_motion_rate = BMEDIA_TIME_SCALE_BASE/-time_scale; /* lacks precision */
            }
        }
        else {
            /* for all other auto modes, we use time skip. */
            int frameRate = 30; /* 30 fps is the forced source rate. */
            unsigned minSlowMotionRate = 4; /* this gives some visual consistency across both slow and fast trick modes. TODO: make programmable? */
            unsigned abs_time_scale = (time_scale < 0) ? (time_scale * -1) : time_scale;

            playMode.playMode = eBpPlayTimeSkip;
            mode->force_source_frame_rate = 0; /* if decoder is performing TSM, we don't need to control exact rate here. */

            /* this logic has the assumption that abs(mode_modifier) should not be lower than 1000 milliseconds and that
            the decoder has a minSlowMotionRate.
            if mode_modifier is set lower than how the stream is encoded, the system will not be able to achieve rates below frameRate.
            for most streams, a GOP every 1 second is a reasonable target for good channel change response.
            if you don't like this assumption, use bmedia_player_host_trick_mode_time_skip and set the mode_modifier as you please. */
            
            /* use max slow motion rate, not min. current max is hard-coded to 4x.
            this allows for higher fps for faster trick modes, but no less than 8fps for slower trick modes.
            if GOP is 30 frames, then it will not be accurate for speeds < 8x. we recommened STC trick modes up to 4x.
            */
            slow_motion_rate = frameRate * BMEDIA_TIME_SCALE_BASE / abs_time_scale;
            if (slow_motion_rate > 4) {
                slow_motion_rate = 4;
            }
            else if (!slow_motion_rate) {
                slow_motion_rate = 1;
            }
            playMode.playModeModifier = (1000 * time_scale * (int)slow_motion_rate) / (frameRate * BMEDIA_TIME_SCALE_BASE);
            BSTD_UNUSED(minSlowMotionRate);

            BDBG_MSG(("trick: rate=%d mode=%d mode_modifier=%d slow_motion_rate=%d",
                time_scale, playMode.playMode, playMode.playModeModifier, slow_motion_rate));
            BDBG_ASSERT(slow_motion_rate);
            
            /* we expect decoder to perform TSM on top of this host trick mode */
            mode->simulated_tsm = true;
        }

        /* set mode->time_scale to the required decoder slow motion rate, in BMEDIA_TIME_SCALE_BASE units.
        if slow_motion_rate is 0, then mode->time_scale was already set with a more precise rate. */
        if (slow_motion_rate) {
            mode->time_scale = BMEDIA_TIME_SCALE_BASE / slow_motion_rate;
            if (!mode->time_scale) {
                /* avoid round to zero */
                mode->time_scale = 1;
            }
        }
    }
    else {
        switch(player->config.decoder_features.host_mode){
        case bmedia_player_host_trick_mode_normal: playMode.playMode= eBpPlayNormal; break;
        case bmedia_player_host_trick_mode_I: playMode.playMode = eBpPlayI; break;
        case bmedia_player_host_trick_mode_skipB: playMode.playMode = eBpPlaySkipB; break;
        case bmedia_player_host_trick_mode_IP: playMode.playMode = eBpPlayIP; break;
        case bmedia_player_host_trick_mode_skipP: playMode.playMode = eBpPlaySkipP; break;
        case bmedia_player_host_trick_mode_brcm: playMode.playMode = eBpPlayBrcm; break;
        case bmedia_player_host_trick_mode_gop: playMode.playMode = eBpPlayDecoderGOPTrick; break;
        case bmedia_player_host_trick_mode_time_skip: playMode.playMode = eBpPlayTimeSkip; break;
        default: playMode.playMode= eBpPlayNormal; break;
        }
        playMode.playModeModifier = (int16_t)player->config.decoder_features.mode_modifier;

        /* don't modify mode->time_scale */
    }

    /* If anything regarding host or trick mode state changed, we need to flush.
       If only decoder mode changed, we don't need to flush. */
    if(prevPlayMode.playMode != playMode.playMode ||
       prevPlayMode.playModeModifier != playMode.playModeModifier ||
       b_sign(direction) != b_sign(player->direction) ) {
        mode->discontinuity = true;

        /* After decoder flush need to set backwards search for pts cache */
        BNAV_Player_SetBackwardPtsCacheSearch( player->bcm_player );

        if(player->pts_valid) {
           bmedia_player_pos last_pos;
           bmedia_player_nav_tell(player, &last_pos);
        }

        if(player->index_valid) {
            long index = player->index;
            long firstIndex, lastIndex;

            if ( playMode.playMode == eBpPlayBrcm && direction < 0 ) {
                    /* If Brcm mode, are currently on a B frame and doing reverse need to decrement by one */
                    if ( BNAV_Player_IndexIsBFrame( player->bcm_player , player->index ) == true ) index--;
                    else index++;
            }
            rc = bp_bounds(player->bcm_player, player, &firstIndex, &lastIndex);
            if(rc==BERR_SUCCESS) {
                if(index<firstIndex) {
                    index = firstIndex;
                } else if(index>lastIndex) {
                    index = lastIndex;
                }
            }
            BDBG_MSG(("index: %ld (%ld:%ld:%ld)", index, player->index, firstIndex, lastIndex));
            rc = b_media_player_nav_set_location(player, index, playMode.playMode);
            if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_set_location;}
        }
        BDBG_MSG(("loopMode:%u playModeModifier:%d playMode:%u", playMode.loopMode, playMode.playModeModifier, playMode.playMode));
        if(BNAV_Player_SetPlayMode(player->bcm_player, &playMode)!=BERR_SUCCESS) { rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto err_set_mode; }
    }
    else {
        BDBG_MSG(("no discontinuity"));
        mode->discontinuity = false;
    }
    mode->dqt = ((playMode.playMode==eBpPlayDecoderGOPTrick) && (playMode.playModeModifier < 0))?true:false;
    mode->tsm = playMode.playMode == eBpPlayNormal;
    mode->continuous = playMode.playMode == eBpPlayNormal;
    mode->brcm = playMode.playMode == eBpPlayBrcm;

    player->mode = playMode.playMode;

#if 0
    /* The following code is an option for debug or a work-around. If you encouter a problem with an I or IP host trick modes, you can enable
    a corresponding decoder trick mode to debug. If there is a problem in the stream, the indexing, or in the software, the corresponding decoder
    trick mode may be able to clean it up. */
    switch(playMode.playMode) {
    case eBpPlayI:
        mode->display_frames = bmedia_player_decoder_frames_I;
        break;
    case eBpPlayIP:
        mode->display_frames = bmedia_player_decoder_frames_IP;
        break;
    default:
        mode->display_frames = bmedia_player_decoder_frames_all;
        break;
    }
#endif

    player->direction = direction;
    player->time_scale = time_scale;
    BDBG_MSG_TRACE(("bmedia_player_nav_set_direction:< %#lx", (unsigned long)player));
    return BERR_SUCCESS;
err_set_location:
err_set_mode:
    BDBG_MSG_TRACE(("bmedia_player_nav_set_direction:< %#lx", (unsigned long)player));
    return rc;
}

void  bmedia_player_nav_get_decoder_config (bmedia_player_nav_t player, bmedia_player_decoder_config *config)
{
    *config = player->config.decoder_features;

    return;
}

int
bmedia_player_nav_set_decoder_config(bmedia_player_nav_t player, const bmedia_player_decoder_config *config)
{
    player->config.decoder_features = *config;

    return 0;
}

void
bmedia_player_nav_update_position(bmedia_player_nav_t player, uint32_t pts)
{
    BDBG_OBJECT_ASSERT(player, bmedia_player_nav);
    BDBG_MSG_TRACE(("bmedia_player_nav_update_position:> %#lx", (unsigned long)player));
    player->pts = pts;
    player->pts_valid = true;

    BDBG_MSG_TRACE(("bmedia_player_nav_update_position:< %#lx", (unsigned long)player));
    return;
}

static int
bmedia_player_nav_seek(bmedia_player_nav_t player, bmedia_player_pos pos)
{
    long index;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(player, bmedia_player_nav);
    BDBG_MSG_TRACE(("bmedia_player_nav_seek:> %#lx %u", (unsigned long)player, (unsigned long)pos));
    index = BNAV_Player_FindIndexFromTimestamp(player->bcm_player, pos);
    if(index==-1) {goto cant_find_index;} /* out of bounds. not an error. */
    rc = b_media_player_nav_set_location(player, index, player->mode);
    if(rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc); goto err_set_location;}

    player->last_position = pos;

    BDBG_MSG_TRACE(("bmedia_player_nav_seek:< %#lx", (unsigned long)player));
    return 0;
err_set_location:
cant_find_index:
    return -1;
}

static void *
b_media_player_nav_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream)
{
    return bmedia_player_nav_create(fd, config, stream);
}

static void
b_media_player_nav_destroy(void *player)
{
    bmedia_player_nav_destroy(player);
    return;
}

static int
b_media_player_nav_next(void *player, bmedia_player_entry *entry)
{
    return bmedia_player_nav_next(player, entry);
}

static void
b_media_player_nav_tell(void *player, bmedia_player_pos *pos)
{
    bmedia_player_nav_tell(player, pos);
    return;
}

static void
b_media_player_nav_get_status(void *player, bmedia_player_status *status)
{
    bmedia_player_nav_get_status(player, status);
    return;
}

static int
b_media_player_nav_seek(void *player, bmedia_player_pos pos)
{
    return bmedia_player_nav_seek(player, pos);
}

static int
b_media_player_nav_set_direction(void *player, bmedia_player_step direction, bmedia_time_scale time_scale, bmedia_player_decoder_mode *mode)
{
    return bmedia_player_nav_set_direction(player, direction, time_scale, mode);
}

const bmedia_player_methods bmedia_player_nav = {
    b_media_player_nav_create,
    b_media_player_nav_destroy,
    b_media_player_nav_next,
    b_media_player_nav_tell,
    b_media_player_nav_get_status,
    b_media_player_nav_set_direction,
    b_media_player_nav_seek
};


