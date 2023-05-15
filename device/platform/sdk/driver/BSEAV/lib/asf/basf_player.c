/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_player.c $
 * $brcm_Revision: 49 $
 * $brcm_Date: 7/27/12 2:53p $
 *
 * Module Description:
 *
 * ASF parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_player.c $
 * 
 * 49   7/27/12 2:53p jtna
 * SW7346-733: fix use of uninitialized variable
 * 
 * 48   7/18/12 2:51p jtna
 * SW7346-733: merge improved no-index handling
 * 
 * 47   9/22/11 1:16p vsilyaev
 * SW7425-1306: Estimate bitrate for files without speficied duration
 * 
 * 46   4/4/11 4:48p vsilyaev
 * SWDEPRECATED-3651: Use preroll when returning position to the user
 * 
 * 45   4/4/11 9:31a gmohile
 * SWDTV-6121 : Fix infinite looping in indexless seek
 * 
 * 44   3/4/11 2:28p gmohile
 * SWDTV-5713 : For no index files, Calculate bitrate using data offsets
 * and duration
 * 
 * 43   4/19/10 11:53a vsilyaev
 * SW3548-2890: Added missing typecasts where multiplying 32-bit integeres
 * expecting 64-bit result
 * 
 * 42   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 * 
 * 41   1/11/10 7:40p vsilyaev
 * SW3556-913: Return an error if weren't able to read index
 * 
 * 40   11/23/09 6:09p vsilyaev
 * SW3548-2588: Properly indentify embedded data
 * 
 * 39   10/14/09 5:33p gmohile
 * SW7405-3208 : Add player support for non indexed asf files
 * 
 * 38   6/23/09 5:51p vsilyaev
 * PR 55845: Added content type for the media player entry, this fixes key
 * frame extraction for AVI 2.0 (OpenDML) streams
 * 
 * 37   6/19/09 3:47p vsilyaev
 * PR 55989: Added decode_mode  paramtewr to to the set_direction method
 * 
 * 36   3/27/09 12:51p vsilyaev
 * PR 50311: Return readPosition in the Nexus playback status. In
 * combination with the position it could be used to measure depth of
 * decoder and playback buffers in units of time.
 * 
 * 35   3/6/09 12:50p vsilyaev
 * PR 52903: Fixed memory leak
 * 
 * 34   2/20/09 4:43p vsilyaev
 * PR 52353: Removed leaked debug code
 * 
 * 33   2/20/09 4:20p vsilyaev
 * PR 52353: Proper account for preroll when accessing the index entries
 * 
 * 32   12/19/08 3:56p vsilyaev
 * PR 50214: Allowed set_direction function to fail and added framework
 * for players to communicate about an errors
 * 
 * 31   11/4/08 9:57a vsilyaev
 * PR 48608: Merged code that alligns audio/video streams after seek.
 * 
 * 30   10/21/08 3:19p vsilyaev
 * PR 48160: Fixed handling of ASF streams with multiple file and data
 * headers (e.g. concataneted streams)
 * 
 * 29   10/2/08 6:00p vsilyaev
 * PR36319 PR34771 PR47073 PR46504: Reduced memory requirements needed to
 * reorder frames in ASF files
 * 
 * 28   8/5/08 12:44p vsilyaev
 * PR 45288: Discard index if less then 2 entries
 * 
 * 27   5/13/08 5:53p vsilyaev
 * PR 42119: Seek file to 0 before reading any data
 * 
 * 26   4/10/08 2:43p gmohile
 * PR 40767 : If seeking to pos 0, then set file_offset to begining of
 * data offset in file. this prevents any loss of audio data before the
 * first video packet
 * 
 * 25   12/20/07 5:22p vsilyaev
 * PR 38073: Set entry type
 * 
 * 24   10/29/07 3:43p jtna
 * PR 35950: Coverity Defect ID:4048 UNINIT
 * 
 * 23   7/23/07 5:51p vsilyaev
 * PR 32813: Fixed comments and unnecessary code
 * 
 * 22   7/17/07 4:31p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 21   7/11/07 6:24p vsilyaev
 * PR 32846: Insert EOF after each seek
 * 
 * 20   7/11/07 5:27p vsilyaev
 * PR 32846: Fixed some typos
 * 
 * 19   7/11/07 4:04p vsilyaev
 * PR 32846: Insert special entry at the end of file to cause detection
 * end of data in the ASF parser
 * 
 * 18   7/9/07 3:38p vsilyaev
 * PR 32846: Updated ASF code to work with batom/SPF framework
 * 
 * 16   2/15/07 11:39a dlwin
 * PR 27935: Fixed a problem reported by customer, where
 * p_ply_asf_get_status() returns invalid timestamp when transitioning
 * from Host trickmode to normal play.
 * 
 * 15   2/7/07 8:58p dlwin
 * PR 27684: ASF parser crashes on playback of "Matrix".
 * 
 * 14   11/13/06 12:05p vsilyaev
 * PR 22578: Improved  EOF diagnostic
 * 
 * 13   9/13/06 1:11p vsilyaev
 * PR 24056: Added code to contruct 'PTS Marked' PES stream for VC1
 * Simple&Main Profiles
 * 
 * 12   8/3/06 2:58p dlwin
 * PR 22578: Fixed compiler warning.
 * 
 * 11   7/28/06 8:51p vsilyaev
 * PR 22578: Improved handling of cases where index entry pointes either
 * to multiple key frames and/or incomplete key frame
 * 
 * 10   7/24/06 5:30p dlwin
 * PR 20683: Fixed handling of embedded data.
 * 
 * 9   7/19/06 5:55p vsilyaev
 * PR 20683: Fixed calculation of first index entry. Fixed rew -> seek ->
 * play transition.
 * 
 * 8   7/10/06 4:32p vsilyaev
 * PR 20308: Fixed compiler warnings
 * 
 * 7   5/19/06 5:08p vsilyaev
 * PR 20683: Return status->bounds.last such it could be used for a seek
 * 
 * 6   5/18/06 6:26p vsilyaev
 * PR 20683: Make EOF silent error
 * 
 * 5   5/2/06 5:51p vsilyaev
 * PR 20683: Added support for the navigation in ASF files
 * 
 * 4   5/1/06 2:58p vsilyaev
 * PR 20683: Improved the asf_player support
 * 
 * 3   4/27/06 6:13p vsilyaev
 * PR 20683: Added support for the asf_player into the SettopApi
 * 
 * 2   4/12/06 10:33a vsilyaev
 * PR 20308: Improved parsing of compressed payload
 * 
 * 1   4/7/06 12:25p vsilyaev
 * PR 20683: added basf_player
 * 
 *******************************************************************************/
#include "bstd.h"
#include "basf_player.h"
#include "basf_parser.h"
#include "bfile_cache.h"
#include "btime_indexer.h"
#include "bkni.h"

BDBG_MODULE(basf_player);
#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */

#define B_ASF_GET_DWORD(p,off) \
			(basf_dword)(((uint8_t *)(p))[(off)+0]) | \
			((basf_dword)(((uint8_t *)(p))[(off)+1])<<8) | \
			((basf_dword)(((uint8_t *)(p))[(off)+2])<<16) | \
			((basf_dword)(((uint8_t *)(p))[(off)+3])<<24) 

#define B_ASF_GET_WORD(p,off) \
			(basf_word)(((uint8_t *)(p))[(off)+0]) | \
			((basf_word)(((uint8_t *)(p))[(off)+1])<<8) 

/* convert convert from 0.1us to 1 ms */
#define B_ASF_PLAYER_SCALE(x) ((x)/10000)


#define B_ASF_SIMPLE_INDEX_ENTRY (4+2)
/* cache in read multipler of sector */
#define B_ASF_SIMPLE_INDEX_CACHE (512*(B_ASF_SIMPLE_INDEX_ENTRY/2))
BDBG_OBJECT_ID(basf_player_t);

enum b_asf_player_state {b_asf_player_header=0, b_asf_player_data, b_asf_player_embedded};

struct basf_player {
	BDBG_OBJECT(basf_player_t)
	basf_qword index_entry_time_interval;
	basf_qword index_entry_time_interval_scaled;
	basf_dword index_entries_count;
	basf_data_info data;
	basf_file_info file;
	bfile_io_read_t  fd;
	bfile_cache_t  idx_cache;
	unsigned long pos; /* current position in the index */
	bool need_eos; /* indicates if we need to send out-of-dataobject block of data in order to indicate end of stream */
	int step;
	unsigned long prev_pkt_number; /* last entry in the cache */
	bmedia_player_config config;
	bmedia_player_stream stream;
	bmedia_player_bounds bounds;
	enum b_asf_player_state player_state;
	basf_qword file_offset;
	basf_qword last_data_offset;
	basf_qword last_read_data_offset;

	size_t chunk_size;
	uint8_t embedded_data[BIO_BLOCK_SIZE];
	size_t embedded_length;

	struct {
		bool index_found;
		bfile_segment idx; /* ASF index */
	} acquisition;

        /*Parser used for non-indexed files*/
	basf_parser_t parser_asf;
	batom_pipe_t pipe_asf;
	batom_factory_t factory;
	bool has_index;
	btime_indexer_t time_index; /* self-created index if ASF index object not found */
	bool trick_next;
	struct {
		bool seek_end;
		basf_qword prev_offset;
		basf_qword start, end;
		unsigned long pos; /* Corresponding position in miliseconds */	
		unsigned long timestamp;
		unsigned master_stream;
	}non_indexed;
};

static void 
b_asf_player_atom_free(batom_t atom, void *user)
{
	void *ptr = *(void **)user;
	BDBG_MSG_TRACE(("free_asf %#lx", (unsigned long)atom));
	BSTD_UNUSED(atom);
	BKNI_Free(ptr);
	return;
}

static const batom_user b_asf_player_atom = {
	b_asf_player_atom_free,
	sizeof(void **)
};


static void 
b_asf_index_simple_index(void *application_cnxt, const basf_simple_index *index)
{
	basf_player_t player = application_cnxt;
	BDBG_OBJECT_ASSERT(player, basf_player_t);
	player->index_entry_time_interval = index->index_entry_time_interval;
	player->index_entries_count = index->index_entries_count;
	bfile_segment_set(&player->acquisition.idx, index->offset, index->index_entries_count*B_ASF_SIMPLE_INDEX_ENTRY);
	player->index_entry_time_interval_scaled = B_ASF_PLAYER_SCALE(player->index_entry_time_interval);
	return;
}

static void
b_asf_player_payload_no_index(void *sink_cnxt, batom_accum_t source, const batom_cursor *payload, batom_cursor *replicated_data, const struct basf_payload_info *info)
{
	basf_player_t player = sink_cnxt;
	bmedia_player_pos dummy;
	BSTD_UNUSED(source);
	BSTD_UNUSED(payload);
	BSTD_UNUSED(replicated_data);

	if(player->non_indexed.seek_end){
		return;
	}
	
	if (info->stream_number==player->stream.master) {
		uint64_t offset;
		offset = player->non_indexed.prev_offset - player->data.data_offset;

		btime_indexer_add(player->time_index, info->pres_time*45, offset, &dummy);
		player->non_indexed.seek_end = true;
		player->non_indexed.timestamp = info->pres_time;
	}
}

static void
b_asf_player_payload_trick(void *sink_cnxt, batom_accum_t source, const batom_cursor *payload, batom_cursor *replicated_data, const struct basf_payload_info *info)
{
	basf_player_t player = sink_cnxt;
	basf_dword remainder;

	BSTD_UNUSED(source);
	BSTD_UNUSED(payload);
	BSTD_UNUSED(replicated_data);
	
	if(player->stream.master!=info->stream_number || (player->non_indexed.start && player->non_indexed.end)) {
		return;
	}

	remainder = info->media_object_size - info->payload_length;

	if(player->step>=0){
		if(info->key_frame && info->offset_into_media_object==0) {
			player->non_indexed.start = player->non_indexed.prev_offset;
			player->non_indexed.end = player->non_indexed.prev_offset + player->file.pkt_size + remainder + (player->file.pkt_size-(remainder%player->file.pkt_size));
			player->non_indexed.timestamp = info->pres_time;
			BDBG_MSG(("keyframe %llu:%llu:%llu, time %u", player->non_indexed.start, player->non_indexed.end, (player->non_indexed.end-player->non_indexed.start-player->data.data_offset)/player->file.pkt_size, info->pres_time));
		}
	} else {
		if (info->key_frame && (info->offset_into_media_object+info->payload_length==info->media_object_size)) {
			player->non_indexed.end = player->non_indexed.prev_offset + player->file.pkt_size;
			if (info->offset_into_media_object) {
				player->non_indexed.start = player->non_indexed.prev_offset - remainder /* actual start of key frame */
					- (player->file.pkt_size - remainder%player->file.pkt_size); /* synced to beginning of packet */
			}
			else { /* entire keyframe in single packet */
				player->non_indexed.start = player->non_indexed.prev_offset;
			}
			player->non_indexed.timestamp = info->pres_time;
			BDBG_MSG(("keyframe %llu:%llu:%llu, time %u", player->non_indexed.start, player->non_indexed.end, (player->non_indexed.end-player->non_indexed.start-player->data.data_offset)/player->file.pkt_size, info->pres_time));
		}
	}
}

static void
b_asf_player_no_index(void *sink_cnxt, batom_accum_t source, const batom_cursor *payload, batom_cursor *replicated_data, const struct basf_payload_info *info)
{
	basf_player_t player = sink_cnxt;
	if (!player->trick_next) {
		b_asf_player_payload_no_index(sink_cnxt, source, payload, replicated_data, info);
	}
	else {
		b_asf_player_payload_trick(sink_cnxt, source, payload, replicated_data, info);
	}
}

static int
b_asf_index_open_asf(basf_player_t player)
{
	/* basf_parser_t asf; */
	basf_parser_cfg cfg;
	size_t feed_size = 4*4*4096; /* enlarged buffer to workaround for PR 27684, need to see if other streams work too */
	size_t len;
	void *buf;
	ssize_t read_result;
	size_t feed_len;
	off_t seek_result;
	bool first = true;
	basf_qword end_of_data;
	struct basf_parser_info info;
	bfile_io_read_t fd = player->fd;
	batom_t atom;
	/* batom_pipe_t pipe_asf; */
/* 	batom_factory_t factory; */


	player->factory = batom_factory_create(bkni_alloc, 16);
	if(!player->factory) {
		goto err_factory;
	}


	player->acquisition.index_found = false;
	player->acquisition.idx.len = 0;
	player->acquisition.idx.start = 0;
	seek_result = fd->seek(fd, 0, SEEK_SET);
	if (seek_result!=0) {
		BDBG_WRN(("b_asf_index_open_asf:%#lx can't seek to start", (unsigned long)player));
		goto err_first_seek;
	}

	basf_parser_default_cfg(&cfg);
	cfg.simple_index = b_asf_index_simple_index;
	cfg.application_cnxt = player;
	player->parser_asf = basf_parser_create(player->factory, &cfg);
	if (!player->parser_asf) {
		BDBG_ERR(("b_asf_index_simple_index: %#lx can't create parser", (unsigned long)player)); 
		goto err_asf;
	}
	player->pipe_asf = batom_pipe_create(player->factory);
	if (!player->pipe_asf) {
		goto err_pipe;
	}
	fd->seek(fd, 0, SEEK_SET);
	for(;;) {
		len = feed_size;
		buf = BKNI_Malloc(len);
		BDBG_MSG_TRACE(("alloc %#lx", (unsigned long)buf));
		if (!buf) {
			BDBG_ERR(("b_asf_index_simple_index: %#lx alloc_failed %u", (unsigned long)player, len));
			goto err_alloc;
		}
		read_result = fd->read(fd, buf, len);
		if (read_result>0) {
			BDBG_ASSERT(read_result<=(ssize_t)len);
			len = read_result;
		}
		if (read_result<=0) {
			BDBG_WRN(("b_asf_index_simple_index: %#lx index not found", (unsigned long)player));
			BKNI_Free(buf);		
			goto err_index;				
		}
		atom = batom_from_range(player->factory, buf, read_result, &b_asf_player_atom, &buf);
		if(!atom) {
			BKNI_Free(buf);
			goto err_atom;
		}
		batom_pipe_push(player->pipe_asf, atom);
		feed_len = basf_parser_feed(player->parser_asf, player->pipe_asf);
		if (feed_len!=len) {
			BDBG_WRN(("b_asf_index_simple_index: %#lx parser has failed", (unsigned long)player));
			goto err_feed;
		}
		basf_parser_get_info(player->parser_asf, &info);
		if(bfile_segment_test(&player->acquisition.idx)) {
			break;
		}
		if (info.state==basf_parser_state_data_object && first) {
			first=false;
			player->data = info.data;
			player->file = info.file;		
			end_of_data = info.data.data_offset + info.file.pkt_size*(basf_qword)(info.data.total_data_packets);
			BDBG_MSG(("end_of_data %lu", (unsigned long)end_of_data));
			seek_result = fd->seek(fd, end_of_data, SEEK_SET);
			if (seek_result != (off_t)end_of_data) {
				BDBG_WRN(("b_asf_index_simple_index: %#lx file seek to %ld has failed", (unsigned long)player, (long)end_of_data));
				goto err_seek;
			}
			basf_parser_seek(player->parser_asf, end_of_data);
		}
	}
	if (player->acquisition.idx.len>0 &&
	    player->index_entries_count>1 &&
	    player->index_entry_time_interval_scaled>0) {
		/* index test was passed */
	        /* Parser is not required if index is found*/
		basf_parser_destroy(player->parser_asf);
		batom_pipe_destroy(player->pipe_asf);
		batom_factory_destroy(player->factory);
		player->has_index = true;
		return 0;
	}
	BDBG_WRN(("invalid index"));
	/* invalid or no index, Use non-indexed player  */
err_index:
	player->has_index = false;
	return 0;
err_seek:
err_feed:
err_atom:
err_alloc:
	batom_pipe_destroy(player->pipe_asf);
err_pipe:
	basf_parser_destroy(player->parser_asf);
err_asf:
err_first_seek:
	batom_factory_destroy(player->factory);
err_factory:
	return BERR_TRACE(-1);
}


basf_player_t
basf_player_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream)
{
	struct basf_player *player;
	int rc;
	off_t file_first, file_last;

	BDBG_ASSERT(fd);
	BDBG_ASSERT(config);
	BDBG_ASSERT(stream && stream->format == bstream_mpeg_type_asf);

	player = BKNI_Malloc(sizeof(*player));
	if (!player) { return NULL; }
	BDBG_OBJECT_INIT(player, basf_player_t);
	player->fd = fd;
	player->config = *config;
	player->stream = *stream;
	rc = b_asf_index_open_asf(player);
	if (rc<0) {
		goto err_index;
	}
	if(player->has_index){
		player->idx_cache = bfile_cache_create(fd, &player->acquisition.idx, B_ASF_SIMPLE_INDEX_CACHE, B_ASF_SIMPLE_INDEX_ENTRY);
		if(!player->idx_cache) {
			goto err_cache;
		}
	}
	else {
		basf_parser_payload_sink sink;
		basf_parser_default_payload_sink(&sink);
		sink.sink_cnxt = player;
		sink.payload = b_asf_player_no_index;
		basf_parser_set_payload_sink(player->parser_asf, &sink);

		player->time_index = btime_indexer_create();
		if (!player->time_index) {
			goto err_time_index;
		}
	}

	player->trick_next = false;
	player->pos = 0;
	player->need_eos = true;
	player->prev_pkt_number = BMEDIA_PLAYER_INVALID;
	player->step = 0;
	player->player_state = b_asf_player_header;
	player->file_offset = player->data.data_offset;
	player->last_data_offset = player->data.data_offset + player->file.pkt_size*(basf_qword)player->data.total_data_packets;
	player->last_read_data_offset = player->last_data_offset & (~(BIO_BLOCK_SIZE-1));
	/* if we use O_DIRECT scheme last 4K worh of data can't be read from the disk, so we use index file (non O_DIRECT) in order to read this data */
	if (player->fd->bounds(player->fd, &file_first, &file_last)==0) {
		if ((basf_qword)file_last >= player->last_read_data_offset + BIO_BLOCK_SIZE ) {
			/* however if file tail is long enough we could still use O_DIRECT in order to read a data */
			player->last_read_data_offset = player->last_data_offset;
		}
	}
	player->chunk_size = (15*BIO_BLOCK_SIZE) - (15*BIO_BLOCK_SIZE)%player->file.pkt_size;
	BDBG_MSG(("basf_player_create: %#lx pkt_size %u chunk_size %u", (unsigned long)player, (unsigned)player->file.pkt_size, player->chunk_size));
	player->bounds.first = 0;
	if(player->has_index) {
		player->bounds.last = (bmedia_player_pos) B_ASF_PLAYER_SCALE(player->index_entry_time_interval*player->index_entries_count);
	} else {
        player->bounds.last = (bmedia_player_pos) B_ASF_PLAYER_SCALE(player->file.duration);
        if(player->bounds.last) {
            player->file.bitrate = 1000*8*((player->last_data_offset - player->file_offset)/player->bounds.last);
        } else {
            player->file.bitrate = 0;
         }
    }
	if(player->bounds.last >= player->file.preroll) {
		player->bounds.last -= player->file.preroll;
	} else {
		player->bounds.last = 0;
	}
	if (player->bounds.first >= player->bounds.last)  {
		BDBG_ERR(("Invalid index bounds %u %u", (unsigned)player->bounds.first, (unsigned)player->bounds.last));
		goto err_bounds;
	}
	
	BDBG_MSG(("basf_player_create: first %u last %u entry_time_interval %u entries_count %u offset %u", (unsigned)player->bounds.first, (unsigned)player->bounds.last, (unsigned)player->index_entry_time_interval, (unsigned)player->index_entries_count, (unsigned)player->file.preroll));
	return player;

err_bounds:
	if(player->has_index) {
		bfile_cache_destroy(player->idx_cache);	
	} else {
		batom_pipe_destroy(player->pipe_asf);
		basf_parser_destroy(player->parser_asf);
		batom_factory_destroy(player->factory);
	}	
err_cache:	
err_time_index:
err_index:
	BKNI_Free(player);
	return NULL;
}

void
basf_player_destroy(basf_player_t player)
{
	BDBG_OBJECT_ASSERT(player, basf_player_t);
	if(player->has_index) {
		bfile_cache_destroy(player->idx_cache);	
	} else {
		batom_pipe_destroy(player->pipe_asf);
		basf_parser_destroy(player->parser_asf);
		batom_factory_destroy(player->factory);
		btime_indexer_destroy(player->time_index);
	}
	BDBG_OBJECT_DESTROY(player, basf_player_t);
	BKNI_Free(player);
	return;
}


static const uint8_t *
b_asf_player_cache_entry(basf_player_t player, unsigned pos)
{
	int rc;
	const uint8_t *buf;

	BDBG_MSG_TRACE(("b_asf_player_cache_entry>: %#xl %u", (unsigned long)player, pos));
	rc = bfile_cache_seek(player->idx_cache, pos*B_ASF_SIMPLE_INDEX_ENTRY);
	if(rc==0) {
		buf = bfile_cache_next(player->idx_cache);
	} else {
		buf = NULL;
	}
	BDBG_MSG_TRACE(("b_asf_player_cache_entry<: %#xl %u:%#lx", (unsigned long)player, pos, (unsigned long)buf));
	return buf;
}


static int
b_asf_player_parser_feed(basf_player_t player, basf_qword offset)
{
	void *buf;
	batom_t atom;
	bfile_io_read_t fd = player->fd;
	size_t len, feed_len, feed_size = player->file.pkt_size;
	ssize_t read_result;
	int rc=0;

	fd->seek(fd, offset, SEEK_SET);
	
	player->non_indexed.seek_end = false;
	player->non_indexed.prev_offset = offset;
	player->non_indexed.start = player->non_indexed.end = 0;
	BDBG_ASSERT((offset-player->data.data_offset)%player->file.pkt_size==0);

		len = feed_size;
		buf = BKNI_Malloc(len);
		if (!buf) {
			BDBG_ERR(("b_asf_player_parser_feed: %#lx alloc_failed %u", (unsigned long)player, len));
			return BERR_TRACE(-1);
		}		
		read_result = fd->read(fd, buf, len);
	if (read_result!=(ssize_t)len) {
			BDBG_WRN(("b_asf_player_parser_feed: %#lx reached end of file", (unsigned long)player));
			BKNI_Free(buf);	
			return -1;
		}
		atom = batom_from_range(player->factory, buf, read_result, &b_asf_player_atom, &buf);
		if(!atom) {
			BKNI_Free(buf);
			return BERR_TRACE(-1);
		}
	basf_parser_seek(player->parser_asf, offset);
		batom_pipe_push(player->pipe_asf, atom);
		feed_len = basf_parser_feed(player->parser_asf, player->pipe_asf);
		if (feed_len!=len) {
			BDBG_WRN(("b_asf_player_parser_feed: %#lx parser has failed", (unsigned long)player));
			return BERR_TRACE(-1);
		}

	return rc;
}

static int
b_asf_player_trick_no_index(basf_player_t player, bmedia_player_entry *entry)
{
	int rc=0;
	int skip=0;
	basf_qword offset;

	skip = (int)((player->file.bitrate/8)*(player->step/(BMEDIA_PLAYER_POS_SCALE)));
	if (skip>=0) {
	skip -= (skip%player->file.pkt_size);
	}
	else {
		skip += (-skip%player->file.pkt_size);
	}
	offset = player->file_offset+skip;

	player->non_indexed.start = 0;
	player->non_indexed.end = 0;
	player->non_indexed.timestamp = 0;
	player->trick_next = true;
    
	for (;;) {
		if (offset<player->data.data_offset || offset>player->last_data_offset-player->file.pkt_size) {
			return -1;
		}
	rc = b_asf_player_parser_feed(player, offset);
	if(rc) {return rc;}
	
		if (player->non_indexed.start && player->non_indexed.end) { /* found keyframe */
			if (player->step>=0) {
				offset = player->non_indexed.end;
			} else {
				offset = player->non_indexed.start - player->file.pkt_size;
			}
			break;
		}

		if(player->step>=0){
			offset += player->file.pkt_size;
		} else {
			offset -= player->file.pkt_size;
		}
	}
	player->trick_next = false;
	player->file_offset = offset;

	entry->start = player->non_indexed.start;
	entry->length = player->non_indexed.end - entry->start;
	entry->timestamp = (bmedia_player_pos)player->non_indexed.timestamp;

	return rc;
}

int
basf_player_next(basf_player_t player, bmedia_player_entry *entry)
{
	basf_dword pkt_number;
	basf_word pkt_cnt;
	const uint8_t *buf;
	unsigned pos=0;
	off_t seek_result;
	ssize_t read_result;
	bfile_io_read_t  fd = player->fd;

	BDBG_OBJECT_ASSERT(player, basf_player_t);
	BDBG_MSG_TRACE(("basf_player_next:> %#lx %u %d", (unsigned long)player, (unsigned)player->pos, (int)player->step));
	bmedia_player_init_entry(entry);
	entry->type = bmedia_player_entry_type_file;
	/* send a haader first */
	switch(player->player_state) {
	case b_asf_player_data:
		break;
	case b_asf_player_header:
		entry->start =  0 ;
		entry->length = player->data.data_offset;
		entry->content = bmedia_player_entry_content_header;
		player->player_state = b_asf_player_data;
		goto done;
	case b_asf_player_embedded:
		entry->start =  player->last_read_data_offset;
		entry->length = player->embedded_length;
		entry->embedded = player->embedded_data;
		entry->type = bmedia_player_entry_type_embedded;
		entry->content = bmedia_player_entry_content_payload;
		player->player_state = b_asf_player_data;
		goto done;
	}
	/* now send a data payload */
	entry->content = bmedia_player_entry_content_payload;
	if (player->step == 0) {
		off_t new_offset;
		/* normal decode */
#if defined (HUMAX_PLATFORM_BASE)
/*************************
*	Don't let it die even if the offset is invalid!
*************************/
		if(player->file_offset > player->last_data_offset)
		{
			player->file_offset = player->last_data_offset;
		}
#else
		BDBG_ASSERT(player->file_offset <= player->last_data_offset);
#endif
		if (player->file_offset == player->last_data_offset) {
			BDBG_MSG(("basf_player_next:%#lx normal play eof", (unsigned long)player));
            entry->type = bmedia_player_entry_type_end_of_stream;
            return 0;
		}
		new_offset = player->file_offset + player->chunk_size;
		entry->start = player->file_offset;
		if (new_offset>(off_t)player->last_read_data_offset) {
			if (player->last_read_data_offset > player->file_offset) {
				/* read last piece of the data */
				entry->length = player->last_read_data_offset - player->file_offset;
				player->file_offset += entry->length;
			} else {
				BDBG_ASSERT(player->last_data_offset >= player->file_offset);
				entry->length = player->last_data_offset - player->file_offset;
				BDBG_MSG(("basf_player_next:%#lx reading %u bytes of embedded data", (unsigned long)player, (unsigned)entry->length));
				BDBG_ASSERT(entry->length <= BIO_BLOCK_SIZE);
				seek_result = fd->seek(fd, entry->start, SEEK_SET);
				if (seek_result != entry->start) {
					BDBG_ERR(("basf_player_next:%#lx error seeking to the data %lu %lu", (unsigned long)player, (unsigned long)seek_result, (unsigned long)entry->start));
					return BERR_TRACE(-1);
				}
				read_result = fd->read(fd, player->embedded_data, entry->length);
				if (read_result!=(ssize_t)entry->length) {
					BDBG_ERR(("basf_player_next:%#lx error reaing the data %ld %ld", (unsigned long)player, (long)read_result, (long)entry->length));
					return BERR_TRACE(-1);
				}
				entry->embedded = player->embedded_data;
		        entry->type = bmedia_player_entry_type_embedded;
				player->file_offset += entry->length;
				BDBG_ASSERT(player->file_offset == player->last_data_offset); /* we shall have just read last byte of data */
			}
		} else {
			if (!player->has_index) {
				b_asf_player_parser_feed(player, player->file_offset);
			}
			entry->length = player->chunk_size;
			player->file_offset = new_offset;
		}			
		goto done;
	}

	if(!player->has_index) {
		int rc;		
		rc = b_asf_player_trick_no_index(player, entry);
		if(rc){
			goto eos;
		}
		goto no_index;
	}

	if (player->pos == BMEDIA_PLAYER_INVALID) {
		BDBG_MSG(("basf_player_next:%#lx eof", (unsigned long)player));
		goto eos;
	}
	pos = player->pos;
	for(;;) {
		BDBG_ASSERT(pos < player->index_entries_count);
		buf = b_asf_player_cache_entry(player, pos);
        if(buf==NULL) {
            return BERR_TRACE(-1);
        }
		pkt_number = B_ASF_GET_DWORD(buf, 0);
		if (pkt_number != player->prev_pkt_number) {
			player->prev_pkt_number = pkt_number;
			pkt_cnt = B_ASF_GET_WORD(buf, 4); 
			break; /* bingo, found new entry */
		}
		/* check of end of file conditions */
		if (player->step>0) {
			pos++; /* advance to the next entry */
			if (pos >= player->index_entries_count) {
				BDBG_MSG(("basf_player_next: %#lx  end of file reached", (unsigned long)player));
				goto eos;
			}
		} else {
			if (pos==0) {
				BDBG_MSG(("basf_player_next: %#lx beginning of file reached", (unsigned long)player));
				goto eos;
			}
			pos--; /* advance to the previous entry */
		}
	}
	BDBG_MSG(("basf_player_next:%#x load entry %u %u:%u", (unsigned long)player, pos, pkt_number, pkt_cnt));
	entry->start = player->data.data_offset + pkt_number*(basf_qword)player->file.pkt_size;
	entry->length = player->file.pkt_size*(pkt_cnt+1); /* read one extra packet */
	/* validate entry start and length */
	if (entry->start + player->file.pkt_size > (off_t)player->last_data_offset) {
		entry->start = player->last_data_offset - player->file.pkt_size;
	}
	if((off_t)(entry->start + entry->length) > (off_t) player->last_data_offset) {
		entry->length = player->last_data_offset - entry->start;
	}

    entry->timestamp = (bmedia_player_pos) (B_ASF_PLAYER_SCALE(player->index_entry_time_interval*pos)-player->file.preroll);

no_index:
	/* test if we reached non readable offset */
	if ((off_t) (entry->start + entry->length) > (off_t)player->last_read_data_offset) {
		size_t read_length;
		off_t read_offset;

		if (entry->start < (off_t)player->last_read_data_offset) {
			read_length = (entry->start + entry->length) - player->last_read_data_offset;
			entry->length = player->last_read_data_offset - entry->start;
			read_offset = player->last_read_data_offset;
			player->player_state = b_asf_player_embedded; /* this will cause embedded data to be passed on the next basf_player_next call */
		} else {
			read_length = entry->length;
			read_offset = entry->start;
			entry->embedded = player->embedded_data;
		}
		BDBG_ASSERT(read_length <= BIO_BLOCK_SIZE);
		seek_result = fd->seek(fd, read_offset, SEEK_SET);
		if (seek_result != read_offset) {
			BDBG_ERR(("basf_player_next:%#lx error seeking to the data %lu %lu", (unsigned long)player, (unsigned long)seek_result, (unsigned long)read_offset));
			return BERR_TRACE(-1);
		}
		read_result = fd->read(fd, player->embedded_data, read_length);
		if ((size_t) read_result!=read_length) {
			BDBG_ERR(("basf_player_next:%#lx error reaing the data %ld %ld", (unsigned long)player, (long)read_result, (long)read_length));
			return BERR_TRACE(-1);
		}
		player->embedded_length = read_length;
	}

	/* advance to new entry */	
	if(player->has_index) {
		pos += player->step;
		if (pos < player->index_entries_count) { 
			player->pos = pos;
		} else { /*  This also catches case where player->step is negative and pos goes over zero and becomes large positive number */
			player->pos = BMEDIA_PLAYER_INVALID;
		}
	}
done:
	BDBG_MSG_TRACE(("basf_player_next:< %#lx %u (%lu:%u:%lu) %s", (unsigned long)player, (unsigned)player->pos, (unsigned long)entry->start, (unsigned)entry->length, (unsigned long)entry->timestamp, entry->embedded?"embedded":""));
	return 0;
eos:
	if(player->need_eos) {
		player->need_eos = false;
		entry->start =  0 ;
		entry->length = player->data.data_offset;
		goto done;
	}
    entry->type = bmedia_player_entry_type_end_of_stream;
	return 0;
}

#define B_ASF_PLAYER_ADVANCE_BRACKETS  (2*BMEDIA_PLAYER_POS_SCALE)

/* sync offset to closest previous packet boundary and adjust delta accordingly */
static void b_asf_player_advance_sync(
	basf_player_t player,
	uint64_t offset,
	bmedia_player_pos delta,
	uint32_t byterate,
	uint64_t *offset_sync,        /* [out] */
	bmedia_player_pos *delta_sync /* [out] */
)
{
	uint64_t offset_adj = (offset%player->file.pkt_size);
	*offset_sync = offset - offset_adj + player->data.data_offset;
	*delta_sync = delta - (offset_adj*BMEDIA_PLAYER_POS_SCALE)/(uint64_t)byterate;
}

static int
b_asf_player_advance(basf_player_t player, bmedia_player_pos destination)
{
	int rc = 0;
	bmedia_player_pos destination_min, destination_max;
	bmedia_player_pos step;
	bmedia_player_pos step_size = BINDEX_SEEK_STEP;

	if (player->config.force_seek_step != 0) {
		step_size = player->config.force_seek_step;
	}

	for(step=step_size;;) {
		btime_indexer_position position;
		uint64_t offset, offset_sync, prev_offset_sync = 0;
		bmedia_player_pos delta, delta_sync, seek_pos;

		destination_min = destination>=step ? destination-step : 0;
		destination_max = destination+step;
		
		if(btime_indexer_position_by_time(player->time_index, destination, &position)!=true) {
			BDBG_WRN(("b_asf_player_advance: %#lx can't get position for %u", (unsigned long)player, destination));
			rc = -1;
			break;
		}
		BDBG_MSG(("b_asf_player_advance: %#lx pos %u:%u, dest %u:%u:%u", (unsigned long)player, position.prev.time, position.next.time, destination_min, destination, destination_max));

		while(position.prev.time >= destination_min && position.next.time <= destination_max) {
			BDBG_MSG(("b_asf_player_advance: %#lx step %u -> %u", (unsigned long)player, step, step/2));
			step /= 2;
			if(step<B_ASF_PLAYER_ADVANCE_BRACKETS) {
				goto done;
			}
			destination_min = destination>=step ? destination-step : 0;
			destination_max = destination+step;
		}
		if(position.next.time < destination ) {
			if(destination >= position.next.time + step) {
				delta = step;
			} else if (destination > position.next.time) {
				delta = destination - position.next.time;
			} else {
				delta = 0;
			}

			offset = position.next.offset + ((uint64_t)position.byterate * delta)/BMEDIA_PLAYER_POS_SCALE;
			b_asf_player_advance_sync(player, offset, delta, position.byterate, &offset_sync, &delta_sync);
			seek_pos = position.next.time+delta_sync;
			btime_indexer_seek(player->time_index, seek_pos);
			BDBG_MSG(("b_asf_player_advance: %#lx forward: time:%u->%u offset:%llu->%llu", (unsigned long)player, position.next.time, seek_pos, position.next.offset, offset_sync));
		} 
		else {
			if(destination >= position.prev.time && destination <= position.next.time) {
				if(position.next.time - destination >= destination - position.prev.time) {
					delta = destination - position.prev.time;
					offset = position.prev.offset + ((uint64_t)position.byterate * delta)/BMEDIA_PLAYER_POS_SCALE;
					b_asf_player_advance_sync(player, offset, delta, position.byterate, &offset_sync, &delta_sync);
					seek_pos = position.prev.time+delta_sync;
					btime_indexer_seek(player->time_index, seek_pos);
					BDBG_MSG(("b_asf_player_advance: %#lx forward: time:%u->%u offset:%llu->%llu", (unsigned long)player, position.prev.time, seek_pos, position.prev.offset, offset_sync));
				} else {
					delta = position.next.time - destination;
					if(delta<B_ASF_PLAYER_ADVANCE_BRACKETS) {
						delta = B_ASF_PLAYER_ADVANCE_BRACKETS;
						if(delta > position.next.time) {
							delta = position.next.time;
						}
					}
					offset = position.next.offset - ((uint64_t)position.byterate * delta)/BMEDIA_PLAYER_POS_SCALE;
					b_asf_player_advance_sync(player, offset, delta, position.byterate, &offset_sync, &delta_sync);
					seek_pos = position.next.time-delta_sync;
					btime_indexer_seek(player->time_index, seek_pos);
					BDBG_MSG(("b_asf_player_advance: %#lx reverse: time:%u->%u offset:%llu->%llu", (unsigned long)player, position.next.time, seek_pos, position.next.offset, offset_sync));
				}
			} else {
				continue;
			}
		}
		BDBG_ASSERT((offset_sync-player->data.data_offset)%player->file.pkt_size==0);

		if (offset_sync > player->last_data_offset-player->file.pkt_size) {
			offset_sync = player->last_data_offset-player->file.pkt_size;
		}
		if (prev_offset_sync==offset_sync) {
			goto done;
		}
		prev_offset_sync = offset_sync;

		do {
			rc = b_asf_player_parser_feed(player, offset_sync);
			if(rc!=0) {
				BDBG_WRN(("b_asf_player_advance: %#lx parser feed error", (unsigned long)player));
				goto done;
			}
			offset_sync += player->file.pkt_size;
		} while (!player->non_indexed.seek_end && offset_sync<player->last_data_offset);
        
	}
	
done:
	return rc;
}


int 
b_asf_player_seek_no_index(basf_player_t player, bmedia_player_pos pos)
{
	basf_qword offset;
	int rc=0;
	btime_indexer_position position;
	bmedia_player_pos dest;    
	off_t first, last;

	if(pos<=player->bounds.first){
		offset = player->data.data_offset;
		goto end;
	} else if(pos >= player->bounds.last) {
		offset = player->last_data_offset - player->file.pkt_size;
		goto end;
	}

	rc = player->fd->bounds(player->fd, &first, &last);
	if (rc>=0) {
		if (last < (off_t)player->last_data_offset) {
			if(player->file.bitrate>0) {
				bmedia_player_pos last_pos = (last*BMEDIA_PLAYER_POS_SCALE) / (player->file.bitrate/8);
				if (pos >= last_pos) {
					offset = last - player->data.data_offset;
					offset -= offset%player->file.pkt_size;
					offset += player->data.data_offset;
					goto end;
				}
			}
		}
	}

	player->trick_next = false;
	rc = b_asf_player_advance(player, pos);
	if (rc<0) { rc=BERR_TRACE(rc);goto error;}

	rc = btime_indexer_position_by_time(player->time_index, pos, &position);
	if (rc!=true) { rc=BERR_TRACE(BERR_UNKNOWN);goto error; }
	btime_indexer_seek(player->time_index, position.time);
	offset = position.offset;
	offset -= (offset%player->file.pkt_size);
	offset += player->data.data_offset;   

	if (offset > player->last_data_offset-player->file.pkt_size) {
		offset = player->last_data_offset-player->file.pkt_size;
	}
	BDBG_MSG(("b_asf_player_seek_no_index: offset %llu", (unsigned long) player, offset));
	b_asf_player_parser_feed(player, offset);
	dest = player->non_indexed.seek_end ? (player->non_indexed.timestamp - (bmedia_player_pos)player->file.preroll) : 0;
	BDBG_MSG(("b_asf_player_seek_no_index: %#lx target %u, actual %u", (unsigned long) player, pos, dest));   
			
end:
	player->file_offset = offset;
	player->player_state = b_asf_player_header;
	player->need_eos = true;

	return 0;
    
error:
	BDBG_WRN(("b_asf_player_seek_no_index: %#lx seek error", (unsigned long)player));
	return -1;
}

int 
basf_player_seek(basf_player_t player, bmedia_player_pos pos)
{
	unsigned index;
	basf_dword pkt_number;
	const uint8_t *buf;
	int rc = 0;

	BDBG_OBJECT_ASSERT(player, basf_player_t);
	BDBG_MSG_TRACE(("basf_player_seek:> %#lx %ld", (unsigned long)player, (long)pos));

	if (pos >= player->bounds.last || pos < player->bounds.first) {
		BDBG_MSG_TRACE(("basf_player_seek:< %#lx %ld", (unsigned long)player, (long)-1));
		return BERR_TRACE(-1);
	}
	if(!player->has_index) {
		return b_asf_player_seek_no_index(player, pos);
	}
	index = (unsigned) ((pos + player->file.preroll) / player->index_entry_time_interval_scaled);
	if (index > player->index_entries_count) {
		index = player->index_entries_count-1;
	}
	player->pos = index;
	player->prev_pkt_number = BMEDIA_PLAYER_INVALID;

	buf = b_asf_player_cache_entry(player, index);
	if (buf) { 
		pkt_number = B_ASF_GET_DWORD(buf, 0);
	} else {
		pkt_number = 0;
		if(bfile_cache_is_file_error(player->idx_cache)) {
            rc = BERR_TRACE(-1);
        }
	}

	if(pos == player->bounds.first) {
		pkt_number = 0;
	}

	player->need_eos = true;
	player->file_offset = player->data.data_offset + pkt_number*(basf_qword)player->file.pkt_size;
	BDBG_MSG_TRACE(("basf_player_seek: %#lx seek file (%lu)%lu", (unsigned long)player, (unsigned long)player->file_offset, (unsigned long)index));

	player->player_state = b_asf_player_header; /* set state to header in order to resent the ASF header */
	BDBG_MSG_TRACE(("basf_player_seek:< %#lx %ld:%lu", (unsigned long)player, (long)player->pos,(unsigned long) B_ASF_PLAYER_SCALE(player->index_entry_time_interval*pos)));
	return rc;
}

void 
basf_player_tell(basf_player_t player, bmedia_player_pos *pos)
{
	BDBG_OBJECT_ASSERT(player, basf_player_t);
	if(player->pos != BMEDIA_PLAYER_INVALID) {
	    *pos = (bmedia_player_pos) B_ASF_PLAYER_SCALE(player->index_entry_time_interval*player->pos); 
    } else {
		*pos = BMEDIA_PLAYER_INVALID;
	}
	return;
}

void 
basf_player_get_status(basf_player_t player, bmedia_player_status *status)
{
	BDBG_OBJECT_ASSERT(player, basf_player_t);
	BDBG_MSG_TRACE(("basf_player_get_status:> %#lx", (unsigned long)player));
	status->bounds = player->bounds;
	status->bounds.last--;
	status->direction = player->step;
    basf_player_tell(player, &status->position);

	BDBG_MSG_TRACE(("basf_player_get_status:< %#lx %ld:%ld %ld", (unsigned long)player, (long)player->bounds.first, (long)player->bounds.last, (long)player->step));
	return;
}

void 
basf_player_set_direction(basf_player_t player, bmedia_player_step direction)
{
	BDBG_OBJECT_ASSERT(player, basf_player_t);

	BDBG_MSG_TRACE(("basf_player_set_direction:> %#lx %ld", (unsigned long)player, (long)direction));
	if (direction==0) {
		player->step=0;
	} else {
		if(!player->has_index){
			player->step = direction;
		} else {
			player->step = (bmedia_player_step) (direction/(int)player->index_entry_time_interval_scaled);
			if (player->step==0) {
				if (direction<0) {
					player->step=-1;
				} else {
					player->step=1;
				}
			}
		}
	}
	player->player_state = b_asf_player_header; /* set state to header in order to resent the ASF header */
	BDBG_MSG_TRACE(("basf_player_set_direction:< %#lx %ld", (unsigned long)player, (long)player->step));
	return;
}


static void *
b_asf_player_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream)
{
	BSTD_UNUSED(config);
	BSTD_UNUSED(stream);
	return basf_player_create(fd, config, stream);
}

static void
b_asf_player_destroy(void *player)
{
	basf_player_destroy(player);
}

static int
b_asf_player_next(void *player, bmedia_player_entry *entry)
{
	return basf_player_next(player, entry);
}

static void 
b_asf_player_tell(void *player, bmedia_player_pos *pos)
{
	basf_player_tell(player, pos);
	return;
}

static void 
b_asf_player_get_status(void *player, bmedia_player_status *status)
{
	basf_player_get_status(player, status);
	return;
}

static int
b_asf_player_set_direction(void *player, bmedia_player_step direction, bmedia_time_scale time_scale, bmedia_player_decoder_mode *mode)
{
	BSTD_UNUSED(time_scale);
    BSTD_UNUSED(mode);
	basf_player_set_direction(player, direction);
	return 0;
}

static int  
b_asf_player_seek(void *player, bmedia_player_pos pos)
{  
	return basf_player_seek(player, pos);
}

const bmedia_player_methods basf_player_methods = {
	b_asf_player_create,
	b_asf_player_destroy,
	b_asf_player_next,
	b_asf_player_tell,
	b_asf_player_get_status,
	b_asf_player_set_direction,
	b_asf_player_seek
};


