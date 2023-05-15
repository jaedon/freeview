/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_test.c $
 * $brcm_Revision: 78 $
 * $brcm_Date: 5/2/12 11:33a $
 *
 * Module Description:
 *
 * BMedia library, unit test
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/build/bmedia_test.c $
 * 
 * 78   5/2/12 11:33a vsilyaev
 * SW7405-5640: Added support for multiple audio tracks, process all
 * probed audio tracks
 * 
 * 77   2/21/12 5:12p vsilyaev
 * SW7425-2378: Support for Monkey Audio (APE) files
 * 
 * 76   12/7/11 4:00p vsilyaev
 * SW7425-1917: Added player for RMFF content
 * 
 * 75   11/18/11 2:28p vsilyaev
 * SW7425-1752: Added PES/ES conversion
 * 
 * 74   9/29/11 10:49a vsilyaev
 * FWAVD-169: Don't use EOSS (E055) sequence to prevent confusion
 * 
 * 73   9/28/11 9:04a vsilyaev
 * FWAVD-169: Added  handling for EndOfFile condition
 * 
 * 72   8/25/11 6:21p vsilyaev
 * SWDTV-8388: Added OGG support
 * 
 * 71   8/26/10 5:01p vsilyaev
 * SW7420-876: Fixed parsing of small MP4 files
 * 
 * 70   7/14/10 6:10p vsilyaev
 * SW3556-1131: Added basic support for CDXA format
 * 
 * 69   6/29/10 10:52a vsilyaev
 * SW7468-69: Updated inline help
 * 
 * 68   5/10/10 6:28p vsilyaev
 * SW7405-3773: Added support for WMC1 video
 * 
 * 67   5/6/10 3:42p vsilyaev
 * SW7405-3773: Added support for demuxing fragments from fragmented MP4
 * container
 * 
 * 66   2/18/10 7:09p vsilyaev
 * SW3556-913: Added more debug logging
 * 
 * 65   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 * 
 * 64   2/12/10 5:37p vsilyaev
 * SW7468-112: Fixed compile problem when ASF directory is not present
 * 
 * 63   2/2/10 3:51p vsilyaev
 * SW7468-69: Added RMFF support
 * 
 * 62   1/21/10 5:56p vsilyaev
 * SW3556-1003: Added option to disable timestamp reordering inside bmedia
 * 
 * 61   1/12/10 7:38p vsilyaev
 * SW3556-913: Improved testing of 'slow' I/O
 * 
 * 60   1/12/10 1:59p vsilyaev
 * SW3556-913: Fixed typo
 * 
 * 59   11/5/09 8:14p vsilyaev
 * SW7405-3349: Sent codec private data and duration among with auxilary
 * payload
 * 
 * 58   7/8/09 11:51a vsilyaev
 * PR 55989: Added modes to support OTF and fragmented trickmodes
 * 
 * 57   6/24/09 10:09a vsilyaev
 * PR 55845: Print timestamp of entry
 * 
 * 56   6/23/09 5:51p vsilyaev
 * PR 55845: Added content type for the media player entry, this fixes key
 * frame extraction for AVI 2.0 (OpenDML) streams
 * 
 * 55   6/9/09 12:44p vsilyaev
 * PR 55554: Fixed warnings
 * 
 * 54   6/8/09 7:09p vsilyaev
 * PR 55554: Fixed handling of huge frames in MP4 stream
 * 
 * 53   6/4/09 7:38p vsilyaev
 * PR 32813: Flush stdout after printing filename
 * 
 * 52   6/4/09 2:45p vsilyaev
 * PR 32813: Improved benchmark function
 * 
 * 51   6/2/09 7:38p vsilyaev
 * PR 55417: Added support for read method returning no data or completing
 * with a partial read
 * 
 * 50   3/2/09 5:13p vsilyaev
 * PR 52434: Added handling of bmedia_player_entry_type_noop
 * 
 * 49   2/17/09 11:17a vsilyaev
 * PR 32813: Use stderr for errors
 * 
 * 48   2/3/09 3:01p vsilyaev
 * PR 32813: Flush stdout after printing stream information
 * 
 * 47   1/29/09 3:19p vsilyaev
 * PR 32813: Use stdout to print parser stream information
 * 
 * 46   1/23/09 9:07p vsilyaev
 * PR 51351: Fixed navigation and resycing in CBR streams with variable
 * frame length
 * 
 * 45   1/13/09 2:51p vsilyaev
 * PR 51013: Fixed handling of 0 len blocks
 * 
 * 44   12/16/08 10:28a vsilyaev
 * PR 50148: Improved parsing of H.264 streams
 * 
 * 43   12/10/08 12:49p vsilyaev
 * PR 49718: Fixed error reporting/handling from MKV player
 * 
 * 42   12/10/08 1:39a vsilyaev
 * PR 48760: Added ES player for accurate progress monitor and navigation
 * 
 * 41   12/4/08 3:28p vsilyaev
 * PR 49993: Added code to inform application about errors detected during
 * stream parsing
 * 
 * 40   11/4/08 1:09p vsilyaev
 * PR 47995: Added control over PES id for audio stream
 * 
 * 39   10/2/08 6:00p vsilyaev
 * PR36319 PR34771 PR47073 PR46504: Reduced memory requirements needed to
 * reorder frames in ASF files
 * 
 * 38   10/1/08 3:34p vsilyaev
 * PR 47282: PR 46848: Added support for SimpleBlock
 * 
 * 37   9/18/08 3:27p vsilyaev
 * PR 47105: Added support for layered buffer cache
 * 
 * 36   9/12/08 6:56p vsilyaev
 * PR 31887: Log reads and seeks that are used by bmedia framework
 * 
 * 35   9/12/08 12:39p vsilyaev
 * PR 46175: Added code to handle case when AVI frame is larger then PVR
 * FIFO size
 * 
 * 34   9/10/08 6:35p vsilyaev
 * PR 46175: Added code to validate size of the RIFF child elements
 * 
 * 33   8/29/08 5:48p vsilyaev
 * PR 33812: Added support for navigation in the MPEG-2 TS file without an
 * index
 * 
 * 32   8/12/08 5:14p vsilyaev
 * PR 31188: Improved extracting of the PES data
 * 
 * 31   3/5/08 2:32p vsilyaev
 * PR 39818: Improved seek/step interraction
 * 
 * 30   3/4/08 12:18p vsilyaev
 * PR 39818: Removed unreferenced code
 * 
 * 29   3/3/08 12:20p vsilyaev
 * PR 39818: Support of MKV format
 * 
 * 28   12/20/07 10:29a vsilyaev
 * PR 38073: Updated to work with new bmedia_player
 * 
 * PR38073/1   12/20/07 10:20a vsilyaev
 * PR 38073: Updated to work with new bmedia_player
 * 
 * 27   7/26/07 2:06p vsilyaev
 * PR 32813: Added benchmark
 * 
 * 
 *******************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bfifo.h"
#include "bfile_stdio.h"
#include "biobits.h"

#include "bioatom.h"
#include "bpool.h"
#include "barena.h"
#include "bioatom.h"
#include "bpool.h"
#include "bmedia_filter.h"
#include "bmedia_player.h"
#include "bfile_stdio.h"
#include "bfile_buffer.h"
#include "bfile_async.h"
#include "bmedia_probe.h"
#include "bmpeg2ts_parser.h"
#include "bmpeg2pes_parser.h"
#include "bmpeg2psi_parser.h"
#include "ts_psi.h"
#include "ts_pat.h"
#include "ts_pmt.h"
#include "bfile_buffered.h"
#include "bmedia_cdxa.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>

BDBG_MODULE(bmedia_test);

#if 1
#define BUF_SIZE	(4096*15)
#else
#define BUF_SIZE	197*7
#endif

void run_test(const char *fname);

static void
usage(const char *name, const char *opt)
{
	if (opt) {
		printf("Unknown option %s\n", opt);
	}
	printf(
			"%s: MEDIA tool\n"
			"Usage: %s [options] <input> <pes_out>\n"
			"where options:\n",
			name, name);
	printf("-help - this help\n"
			"-msg <module> - set message level debug output for the module\n"
			"-type <STRING> - type of media stream AVI, MPEG1, ASF, MP4, etc\n");
#if !B_PROBE_ONLY

	printf(
			"-video <N> - extract video from the stream N\n"
			"-audio <N> - extract audio from the stream N\n"
			"-video_id <id> - id for the video stream, if 0 stream saved in ES format used and 0x01 in RAW format\n"
			"-audio_id <id> - id for the video stream, if 0 stream saved in ES format used and 0x01 in RAW format\n"
		  );
	printf(
			"-player - enable use of the media player\n"
			"-step - pause after each block\n"
			"-feed <n> - feed blocks of <n> bytes\n"
			"-direction <x> - set player's direction\n"
			"-time_scale <x> - set stream timescale\n"
			"-seek <ms> - seek stream to location\n"
			"-seek2 <ms> - second seek stream to location\n"
			"-async - use asynchronous file I/O where possible\n"
			"-fifo_size - size of the emulated FIFO\n"
			"-buffer - use common buffer for probe and player\n"
			"-quiet - reduce number of messages\n");

	printf(
			"-probe - auto detect file format\n"
			"-probe-only - auto detect file format\n"
			"-slow_read_factor - factor for artificially slow read speed\n"
			"-benchmark fname - run a MPEG-2 TS parsing benchmark\n"
			"-fragmented - enable fragmented decoder mode\n"
			"-otf - enable on the fly PVR decoder mode\n"
			"-no-timestamp-reordering - don't reorder timestamps in the video stream\n"
		  );
#endif

	exit(0);
}

typedef struct b_media_feed_status {
	size_t alloc_cnt;
	size_t free_cnt;
	size_t alloc_size;
	size_t alloc_size_peak;
	size_t size;
} b_media_feed_status;

typedef struct b_media_feed_state {
	b_media_feed_status status;
	const struct media_cfg *cfg;
	batom_factory_t factory;
	batom_pipe_t pipe_media;
	bmedia_filter_t filter;
} b_media_feed_state;

typedef struct b_media_block {
	size_t block_size;
	b_media_feed_state *state;
	uint8_t data[1]; /* variable size data */
} b_media_block;

static b_media_block *
b_media_alloc(b_media_feed_state *state, size_t size)
{
	b_media_block *block;
	block = BKNI_Malloc(sizeof(*block)+size);
	if(block) {
		block->block_size = size;
		block->state = state;
		state->status.alloc_cnt ++;
		state->status.size += size;
		state->status.alloc_size += size;
		if(state->status.alloc_size_peak < state->status.alloc_size) {
			state->status.alloc_size_peak = state->status.alloc_size;
		}
	}
	return block;
}


static void
b_media_free(b_media_block *block)
{
	b_media_feed_state *state;
	BDBG_ASSERT(block);
	state = block->state;
	state->status.alloc_size -= block->block_size;
	state->status.free_cnt ++;
	block->block_size = 0;
	BKNI_Free(block);
	return;
}
	
static void 
b_atom_free_media(batom_t atom, void *user)
{
	b_media_block *block = *(b_media_block **)user;
	BDBG_MSG(("free_media %#lx", (unsigned long)atom));
	BSTD_UNUSED(atom);
	b_media_free(block);
	return;
}

static const batom_user b_atom_media = {
	b_atom_free_media,
	sizeof(b_media_block *)
};

typedef struct stream_cfg {
	unsigned pes_id;
	unsigned stream_id;
    unsigned codec;
} stream_cfg;

#define MAX_AUDIO_TRACKS    8
typedef struct media_cfg {
	bstream_mpeg_type stream_type;
	stream_cfg video;
	stream_cfg audio[MAX_AUDIO_TRACKS];
	bmedia_player_step direction;
	bmedia_time_scale time_scale;
	bool step;
	bool player;
	bool async;
	bool probe;
	bool probe_only;
	bool buffer;
    bool fragmented;
    bool otf;
    bool no_timestamp_reordering;
	bmedia_player_pos seek;
	bmedia_player_pos seek2;
	size_t feed;
	size_t fifo_size;
	bool fifo_size_set;
    unsigned slow_read_factor;
    unsigned other_streams[8];
} media_cfg;


static const uint8_t eos_data[256] = {
#if 0
       0xE0, 0x55,
#endif
       0x00, 0x00
};


static void 
b_media_async(void *cntx, bmedia_player_entry *entry)
{
	b_media_feed_state *state = cntx;
	b_media_block *block;
	batom_t atom;

	if(entry->type == bmedia_player_entry_type_error) {
		return;
	}
	BDBG_ASSERT(state);
	BDBG_ASSERT(entry);
	block = b_media_alloc(state, entry->length);
	BDBG_ASSERT(block);

	if(entry->atom) {
		batom_cursor cursor;
		int rc;

		batom_cursor_from_atom(&cursor, entry->atom);
		rc = batom_cursor_copy(&cursor, block->data, entry->length);
		BDBG_ASSERT(rc==(int)entry->length);
		batom_release(entry->atom);
	}	
	atom = batom_from_range(state->factory, block->data, entry->length, &b_atom_media, &block);
	BDBG_ASSERT(atom);
	batom_pipe_push(state->pipe_media, atom);

	return;
}

static void 
media_test_stream_error(void *cntx)
{
	b_media_feed_state *state=cntx;
    bmedia_filter_status status;

	bmedia_filter_get_status(state->filter, &status);
    BDBG_WRN(("stream error: sync:%u resync:%u format:%u", status.errors.sync_errors, status.errors.resync_events, status.errors.format_errors));
    return;
}

struct b_test_slow_fileio_read {
	struct bfile_io_read ops; /* shall be the first member */
    unsigned count;
    unsigned delay_factor;
    off_t off;
    uint64_t bytes_read;
    bfile_io_read_t master;
};

static ssize_t 
b_test_slow_read(bfile_io_read_t fd, void *buf, size_t length)
{
    ssize_t result;
	struct b_test_slow_fileio_read *f=(void *)fd;

    if(f->delay_factor>0) {
        f->count ++;

        if(f->count > f->delay_factor) {
            BDBG_WRN(("slow_read: %u:%u -> NO DATA", (unsigned)f->off, length));
            f->count = 0;
            return BFILE_ERROR_NO_DATA;
        }
        if(f->count == f->delay_factor/2) {
            if(length>1) {
                BDBG_WRN(("slow_read: %u:%u -> %u", (unsigned)f->off, length, length/2));
                length/=2;
            }
        }
    }
	result = f->master->read(f->master, buf, length );
    if(result>0) {
        f->bytes_read += result;
    }
    BDBG_WRN(("read:%s: %u:%u -> %u", f->delay_factor?"data":"index",(unsigned)f->off, length, result));
    /* breakpoint(); */
    return result;
}

static off_t 
b_test_slow_seek(bfile_io_read_t fd, off_t offset, int whence)
{
	struct b_test_slow_fileio_read *f=(void *)fd;
    f->off = offset;
	return f->master->seek(f->master, offset, whence);
}

static int 
b_test_slow_bounds(bfile_io_read_t fd, off_t *first, off_t *last)
{
	struct b_test_slow_fileio_read *f=(void *)fd;
	return f->master->bounds(f->master, first, last);
}

static const struct bfile_io_read b_test_slow_fileio_read_ops = {
	b_test_slow_read,
	b_test_slow_seek,
	b_test_slow_bounds,
	BIO_DEFAULT_PRIORITY
};

static bfile_io_read_t 
b_test_slow_read_attach(struct b_test_slow_fileio_read *file, bfile_io_read_t master, unsigned factor)
{
    file->ops = b_test_slow_fileio_read_ops;
    file->master = master;
    file->count = 0;
    file->delay_factor = factor;
    file->off = 0;
    file->bytes_read = 0;
    return &file->ops;
}

static const char *
b_media_player_type2str(bmedia_player_entry_type type)
{
    switch(type) {
    case bmedia_player_entry_type_file:  return "file";
    case bmedia_player_entry_type_embedded: return "embdedded";
    case bmedia_player_entry_type_noop: return "noop";
    case bmedia_player_entry_type_atom: return "atom";
    case bmedia_player_entry_type_async: return "async";
    case bmedia_player_entry_type_no_data: return "no-data";
    case bmedia_player_entry_type_error: return "error";
    case bmedia_player_entry_type_end_of_stream: return "end-of-stream";
    }
    return NULL;
}

static const char *
b_media_player_content2str(bmedia_player_entry_content content)
{
    switch(content) {
    case bmedia_player_entry_content_unknown:  return "unknown";
    case bmedia_player_entry_content_header: return "header";
    case bmedia_player_entry_content_payload: return "payload"; 
    }
    return NULL;
}
 
static void 
media_feed(batom_factory_t factory, FILE *fin_data, FILE *fout, bfile_io_read_t raw_fd_index, const media_cfg *cfg, b_media_feed_status *status)
{
	batom_pipe_t pipe_pes;
	bmedia_filter_cfg filter_cfg;
	bmedia_stream_t	video=NULL;
	bmedia_stream_t	audio[MAX_AUDIO_TRACKS];
	int rc;
	size_t len = cfg->feed;
	batom_t atom;
    bmedia_player_decoder_mode decoder_mode;
    unsigned media_bytes=0, pes_bytes=0;
	b_media_feed_state state;
    bool eof_marker = false;
    unsigned i;

    BKNI_Memset(audio, 0, sizeof(audio));

	state.factory = factory;
	state.status.alloc_cnt = 0;
	state.status.free_cnt = 0;
	state.status.alloc_size = 0;
	state.status.alloc_size_peak = 0;
	state.status.size = 0;
	state.cfg = cfg;
	state.pipe_media= batom_pipe_create(factory);
	BDBG_ASSERT(state.pipe_media);
	
	bmedia_filter_default_cfg(&filter_cfg);
	filter_cfg.eos_len = sizeof(eos_data);
	filter_cfg.eos_data = eos_data;
    filter_cfg.stream_error = media_test_stream_error;
    filter_cfg.application_cnxt  = &state;
	state.filter = bmedia_filter_create(factory, bkni_alloc, &filter_cfg);
	BDBG_ASSERT(state.filter);


	switch(cfg->stream_type) {
	case bstream_mpeg_type_es:
	case bstream_mpeg_type_mp4:
	case bstream_mpeg_type_mkv:
		pipe_pes = bmedia_filter_start(state.filter, bstream_mpeg_type_pes);
		break;
	default:
		pipe_pes = bmedia_filter_start(state.filter, cfg->stream_type);
		break;
	}
	BDBG_ASSERT(pipe_pes);

	if(cfg->video.stream_id) {
        bmedia_stream_cfg stream_cfg;

        bmedia_stream_default_cfg(&stream_cfg);
        stream_cfg.reorder_timestamps = !cfg->no_timestamp_reordering;
		video = bmedia_filter_stream_open(state.filter, cfg->video.stream_id, cfg->video.pes_id, &stream_cfg);
		BDBG_ASSERT(video);
	}
    for(i=0;i<MAX_AUDIO_TRACKS;i++) {
        if(!cfg->audio[i].stream_id) {
            break;
        }
        audio[i] = bmedia_filter_stream_open(state.filter, cfg->audio[i].stream_id, cfg->audio[i].pes_id, NULL);
        BDBG_ASSERT(audio[i]);
    }
	if(cfg->player) {
		/* with a player */
        unsigned i;
		bmedia_player_t player=NULL;
		bfile_io_read_t  raw_fd_data=NULL;
		bfile_io_read_t  fd_data=NULL;
		bmedia_player_entry entry;
		bmedia_player_stream stream;
		bfile_buffer_t buffer;
		bfile_buffer_cfg buffer_cfg;
		bmedia_player_config player_config;
        bfile_io_read_t fd_index;
        struct b_test_slow_fileio_read slow_fileio_data, slow_fileio_index;
        unsigned other_track=0;

		raw_fd_data = bfile_stdio_read_attach(fin_data);
        if(cfg->slow_read_factor) {
            fd_data = b_test_slow_read_attach(&slow_fileio_data, raw_fd_data, cfg->slow_read_factor);
            fd_index = b_test_slow_read_attach(&slow_fileio_index, raw_fd_index, 0);
        } else {
            fd_data = raw_fd_data;
            fd_index = raw_fd_index;
        }

		bfile_buffer_default_cfg(&buffer_cfg);
		if(cfg->fifo_size_set) {
			buffer_cfg.buf_len = cfg->fifo_size;
		}
		buffer_cfg.buf = BKNI_Malloc(buffer_cfg.buf_len);
		BDBG_ASSERT(buffer_cfg.buf!=NULL);
		buffer_cfg.async = true;
		buffer_cfg.async_read = bfile_async_read;
		buffer_cfg.fd = fd_data;
		buffer = bfile_buffer_create(factory, &buffer_cfg);
		BDBG_ASSERT(buffer);

		bmedia_player_init_stream(&stream);
		stream.format = cfg->stream_type;
		stream.master = cfg->video.stream_id;
        stream.stream.id.master = cfg->video.pes_id;
        for(i=0;i<MAX_AUDIO_TRACKS;i++) {
            if(!cfg->audio[i].stream_id) {
                break;
            }
            stream.other[other_track] = cfg->audio[i].stream_id;
            stream.stream.id.other[other_track] = cfg->audio[i].pes_id;
            other_track++;
        }

        if(cfg->stream_type==bstream_mpeg_type_es) {
            stream.stream.es.video_codec = cfg->video.codec;
            stream.stream.es.audio_codec = cfg->audio[0].codec;
        }
        for(i=0;i<sizeof(cfg->other_streams)/sizeof(cfg->other_streams[0]);i++) {
            if(cfg->other_streams[i]==0) {
                break;
            }
            if(other_track>=sizeof(stream.other)/sizeof(*stream.other)) {
                BDBG_WRN(("no space to other track:%u %u", i, cfg->other_streams[i]));
                break;
            }
            stream.other[other_track] = cfg->other_streams[i];
            stream.stream.id.other[other_track] = 0xD0+i;
            other_track++;
        }

		bmedia_player_init_config(&player_config);
		player_config.buffer = buffer;
        player_config.factory = factory;
		if(cfg->async) {
			player_config.cntx = &state;
			player_config.atom_ready = b_media_async;
		}
        player_config.decoder_features.fragmented = cfg->fragmented;
        player_config.decoder_features.otf = cfg->otf;
        if(cfg->otf) {
            stream.stream.es.video_codec = bvideo_codec_mpeg2;
        }
		switch(cfg->stream_type) {
		case bstream_mpeg_type_es:
		case bstream_mpeg_type_ts:
			stream.without_index = true;
			break;
		default:
			break;
		}
		player = bmedia_player_create(fd_index, &player_config, &stream);
		if(!player) {
			fprintf(stdout, "Can't use index for the media file\n");
			if(cfg->step) {
				getchar();
			}
            bfile_buffer_destroy(buffer);
            BKNI_Free(buffer_cfg.buf);
            bfile_stdio_read_detach(fd_data);
			fd_index->seek(fd_index, 0, SEEK_SET);
			goto no_player;
		}
        if(cfg->step) { getchar(); }
		if(cfg->seek!=BMEDIA_PLAYER_INVALID) {
			bmedia_player_seek(player, cfg->seek);
			if(cfg->step) {
				getchar();
			}
		}
		if(cfg->seek2!=BMEDIA_PLAYER_INVALID) {
			bmedia_player_seek(player, cfg->seek2);
			if(cfg->step) {
				getchar();
			}
		}
		if(cfg->direction!=0) {
			bmedia_filter_set_keyframeonly(state.filter, true);
		}
		bmedia_player_set_direction(player, cfg->direction, cfg->time_scale, &decoder_mode);
		for(;;) {
			int rc;
			rc = bmedia_player_next(player, &entry);
			if(rc<0) {
                BDBG_MSG(("Player: File Error"));
				break;
			}
            BDBG_MSG(("entry: %u[%s] %u[%s] %u(%lld:%u) %#lx %#lx", entry.type, b_media_player_type2str(entry.type), entry.content, b_media_player_content2str(entry.content), entry.timestamp, entry.start, entry.length, (unsigned long)entry.atom, (unsigned long)entry.embedded));
			if(entry.type == bmedia_player_entry_type_async) {
				bfile_async_process();
			} else if(entry.type == bmedia_player_entry_type_noop) {
				continue;
			} else if(entry.type == bmedia_player_entry_type_error) {
                BDBG_MSG(("File Error"));
				break;
            } else if(entry.type == bmedia_player_entry_type_end_of_stream) {
                BDBG_MSG(("End Of Stream"));
                break;
			} else {
	            b_media_block *block;


				rc = fseek(fin_data, entry.start, SEEK_SET);
				BDBG_ASSERT(rc==0);
				BDBG_MSG(("seek %#llu size %u",  entry.start, entry.length));
				rc = bmedia_filter_seek(state.filter, entry.start);
				BDBG_ASSERT(rc==0);
                if(entry.entry) {
                    unsigned i;
                    for(i=0;i<sizeof(entry.entry->resync)/sizeof(entry.entry->resync[0]);i++) {
                        if(entry.entry->resync[i].stream_id) {
                            bmedia_filter_set_offset(state.filter, entry.entry->resync[i].timestamp, entry.entry->resync[i].stream_id);
                        }
                    }
                }
				block = b_media_alloc(&state, entry.length);
				BDBG_ASSERT(block);
				if(entry.embedded) {
					BKNI_Memcpy(block->data, entry.embedded, entry.length);
					rc = entry.length;
				} else if(entry.atom) {
					batom_cursor cursor;
	
					batom_cursor_from_atom(&cursor, entry.atom);
					rc = batom_cursor_copy(&cursor, block->data, entry.length);
					BDBG_ASSERT(rc==(int)entry.length);
					batom_release(entry.atom);
				} else {
					rc = fread(block->data, 1, entry.length, fin_data);
					BDBG_ASSERT(rc==(int)entry.length);
				}
                media_bytes += rc;
				atom = batom_from_range(factory, block->data, rc, &b_atom_media, &block);
                if(atom) {
				    batom_pipe_push(state.pipe_media, atom);
                } else {
				    b_media_free(block);
                }
			}
            for(;;) {
                bool pes=false;
	            bool result;
				result = bmedia_filter_feed(state.filter, state.pipe_media);
				BDBG_MSG(("feed %u alloc: %u:%u %u (%u:%u)", result, state.status.free_cnt, state.status.alloc_cnt, state.status.alloc_size, state.status.alloc_size_peak, state.status.size));
				while(NULL!=(atom=batom_pipe_pop(pipe_pes))) {
					batom_cursor cursor;
					unsigned i;

					BDBG_MSG(("pes %u", batom_len(atom)));
					batom_cursor_from_atom(&cursor, atom);
					for(i=0;i<cursor.count;i++) {
                        pes_bytes += cursor.vec[i].len;
						fwrite(cursor.vec[i].base, 1, cursor.vec[i].len, fout);
					}
					batom_release(atom);
					fflush(fout);
                    pes=true;
				}
                BDBG_MSG(("media_bytes:%u pes_bytes:%u", media_bytes, pes_bytes));
                if(batom_pipe_peek(state.pipe_media)==NULL && (!pes || result)) {
                    BDBG_MSG(("finished: pes:%u result:%u", pes, result));
                    break;
                }
			} 
			if(state.status.alloc_size >= cfg->fifo_size) {
				BDBG_WRN(("Flushing FIFO at %u:%u", state.status.alloc_size, cfg->fifo_size));
				bmedia_filter_clear(state.filter);
				BDBG_WRN(("After FIFO flush %u", state.status.alloc_size));
			}
			if(cfg->step) { getchar(); }
		}
#if 0
		getchar();
		bmedia_player_next(player, &entry);
		getchar();
		bmedia_player_next(player, &entry);
		getchar();
		bmedia_player_next(player, &entry);
		getchar();
		bmedia_player_set_direction(player, 33);
		getchar();
		bmedia_player_seek(player, 40 * 1000);
		getchar();
		bmedia_player_next(player, &entry);
		getchar();
		bmedia_player_next(player, &entry);
		getchar();
		bmedia_player_next(player, &entry);
		getchar();
		bmedia_player_next(player, &entry);
		getchar();
		bmedia_player_next(player, &entry);
		getchar();
#endif

		bmedia_player_destroy(player);
		bfile_buffer_destroy(buffer);
		BKNI_Free(buffer_cfg.buf);
		bfile_stdio_read_detach(raw_fd_data);
        if(cfg->slow_read_factor) {
            BDBG_WRN(("bytes read index:%u data:%u", (unsigned)slow_fileio_index.bytes_read, (unsigned)slow_fileio_data.bytes_read));
        }
		goto done;
	}
no_player:
	for(;;) {
	    b_media_block *block;
        bool endoffile=false;
        bool pes=false;

		block = b_media_alloc(&state, len);
		BDBG_ASSERT(block);
		rc = fread(block->data, 1, len, fin_data);
		if (rc<=0) {
            endoffile = true;
			b_media_free(block);
            if(!eof_marker) {
                eof_marker = true;
                atom = batom_empty(factory, NULL, NULL);
            } else {
                atom = NULL;
            }
		} else {
            media_bytes += rc;
            atom = batom_from_range(factory, block->data, rc, &b_atom_media, &block);
            if(!atom) {
                b_media_free(block);
                continue;
            }
        }
		BDBG_MSG(("push %#lx", atom));
        if(atom) {
		    batom_pipe_push(state.pipe_media, atom);
        }
        for(;;) {
	        bool result;
            pes=false;
            result = bmedia_filter_feed(state.filter, state.pipe_media);
		    BDBG_MSG(("feed %u alloc: %u:%u %u (%u:%u)", result, state.status.free_cnt, state.status.alloc_cnt, state.status.alloc_size, state.status.alloc_size_peak, state.status.size));
			while(NULL!=(atom=batom_pipe_pop(pipe_pes))) {
				batom_cursor cursor;
				unsigned i;

				BDBG_MSG(("pes %u", batom_len(atom)));
				batom_cursor_from_atom(&cursor, atom);
				for(i=0;i<cursor.count;i++) {
                    pes_bytes += cursor.vec[i].len;
                    if(cursor.vec[i].len>0) {
                        pes=true;
                    }
					fwrite(cursor.vec[i].base, 1, cursor.vec[i].len, fout);
				}
				batom_release(atom);
				fflush(fout);
			}
#if 0
			{
				batom_factory_stats factory_stats;
				batom_factory_get_stats(factory, &factory_stats);
				BDBG_WRN(("status: atoms[live:%u allocated:%u freed:%u] alloc[pool:%u/%u/%u arena:%u/%u/%u alloc:%u/%u]", factory_stats.atom_live, factory_stats.atom_allocated, factory_stats.atom_freed, factory_stats.alloc_pool, factory_stats.free_pool, factory_stats.full_pool, factory_stats.alloc_arena, factory_stats.free_arena, factory_stats.full_arena, factory_stats.alloc_alloc, factory_stats.free_alloc));
			}
#endif
            BDBG_MSG(("media_bytes:%u pes_bytes:%u", media_bytes, pes_bytes));
            if(batom_pipe_peek(state.pipe_media)==NULL && (!pes || result)) {
                BDBG_MSG(("finished: pes:%u result:%u", pes, result));
                break;
            }
		} 
		if(state.status.alloc_size >= cfg->fifo_size) {
			BDBG_WRN(("Flushing FIFO at %u:%u", state.status.alloc_size, cfg->fifo_size));
			bmedia_filter_clear(state.filter);
            BDBG_WRN(("After FIFO flush %u", state.status.alloc_size));
		}
		if(cfg->step) { getchar(); }
        if(endoffile) {
            if(!pes) {
                break;
            }
        }
	}
done:
    bmedia_filter_flush(state.filter);
    BDBG_MSG(("flush completed"));
	*status = state.status;
    for(i=0;i<MAX_AUDIO_TRACKS;i++) {
        if(audio[i]) {
            bmedia_filter_stream_close(state.filter, audio[i]);
        }
    }
	if(video) {
		bmedia_filter_stream_close(state.filter, video);
	}
    {
        bmedia_filter_status status;
	    bmedia_filter_get_status(state.filter, &status);
        if(status.errors.sync_errors  || status.errors.resync_events || status.errors.format_errors) {
            BDBG_WRN(("parsing errors: sync:%u resync:%u format:%u", status.errors.sync_errors, status.errors.resync_events, status.errors.format_errors));
        }
    }
	bmedia_filter_stop(state.filter);
	bmedia_filter_destroy(state.filter);
	batom_pipe_destroy(state.pipe_media);
	return;
}


int 
main(int argc, const char *argv[])
{
	FILE *fin=NULL, *fout=NULL;
	const char *file=NULL;
	const char *pes=NULL;
	int arg;
	int file_arg=0;
	batom_factory_t factory;
	batom_factory_stats factory_stats;
	char pes_name[1024];
	media_cfg cfg;
	bfile_buffered_t file_buffered;
	bfile_io_read_t  raw_fd=NULL;
	bfile_io_read_t  fd=NULL;
    int audio_track=-1;
    unsigned i;


	BKNI_Init();
	BDBG_Init();
	BDBG_SetLevel(BDBG_eWrn);

	bfile_async_init();

    BKNI_Memset(&cfg, 0, sizeof(cfg));
	cfg.stream_type = bstream_mpeg_type_unknown;
	cfg.video.stream_id = 0;
	cfg.video.pes_id = 0xE0;
    cfg.video.codec = bvideo_codec_unknown;
    for(i=0;i<MAX_AUDIO_TRACKS;i++) {
        cfg.audio[i].stream_id = 0;
        cfg.audio[i].pes_id = 0xC0+i;
        cfg.audio[i].codec = baudio_format_unknown;
    }
	cfg.player = false;
	cfg.direction = 0;
	cfg.time_scale = BMEDIA_TIME_SCALE_BASE;
	cfg.fifo_size = (188/4)*4096*8; /* ~ 1.5MBytes */
	cfg.fifo_size_set = false; 
	cfg.seek = BMEDIA_PLAYER_INVALID;
	cfg.seek2 = BMEDIA_PLAYER_INVALID;
	cfg.step = false;
	cfg.feed = BUF_SIZE;
	cfg.async = false;
	cfg.probe = false;
	cfg.buffer = false;
    cfg.fragmented = false;
    cfg.no_timestamp_reordering = false;
    cfg.otf = false;
    cfg.slow_read_factor = 0;
#if B_PROBE_ONLY
	cfg.probe_only = true;
	BDBG_SetLevel(BDBG_eErr);
#else
	cfg.probe_only = false;
#endif

	arg=1;
	while (argc>arg) {
		if (!strcmp("-help",argv[arg])) {
			usage(argv[0], NULL);
		} else if (!strcmp("-msg",argv[arg]) && argc>arg+1) {
			arg++;
			BDBG_SetModuleLevel(argv[arg], BDBG_eMsg); 
		} else if (*argv[arg]!='\0' && (*argv[arg]!='-' || argv[arg][1]=='\0'))  {
			switch(file_arg) {
			case 0: 
				{
					char *dot;
					file = argv[arg]; 
					strcpy(pes_name, file);
					dot = strrchr(pes_name, '.');
					if (dot) {
						strcpy(dot+1,"pes");
						pes = pes_name;
					}
				}
				break;
			case 1: pes = argv[arg]; break;
			default:
				usage(argv[0], argv[arg]);
			}
			file_arg++;
		} else if (!strcmp("-msg",argv[arg]) && argc>arg+1) {
			arg++;
			BDBG_SetModuleLevel(argv[arg], BDBG_eMsg); 
		} else if (!strcmp("-video",argv[arg]) && argc>arg+1) {
			arg++;
			cfg.video.stream_id = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-audio",argv[arg]) && argc>arg+1) {
			arg++;
            audio_track ++;
            if(audio_track<0 || audio_track>=MAX_AUDIO_TRACKS) {
                BDBG_ERR(("out of audio tracks %d(%u)", audio_track, MAX_AUDIO_TRACKS));
                audio_track = MAX_AUDIO_TRACKS-1;
            }
			cfg.audio[audio_track].stream_id = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-video_id",argv[arg]) && argc>arg+1) {
			arg++;
			cfg.video.pes_id = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-audio_id",argv[arg]) && argc>arg+1) {
            unsigned track = audio_track>=0?audio_track:0;
			arg++;
			cfg.audio[track].pes_id = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-player",argv[arg]) && argc>arg) {
			cfg.player = true;
		} else if (!strcmp("-step",argv[arg]) && argc>arg) {
			cfg.step = true;
		} else if (!strcmp("-async",argv[arg]) && argc>arg) {
			cfg.async = true;
		} else if (!strcmp("-quiet",argv[arg]) && argc>arg) {
			BDBG_SetLevel(BDBG_eErr);
		} else if (!strcmp("-probe",argv[arg]) && argc>arg) {
			cfg.probe = true;
		} else if (!strcmp("-buffer",argv[arg]) && argc>arg) {
			cfg.buffer = true;
		} else if (!strcmp("-probe-only",argv[arg]) && argc>arg) {
			cfg.probe_only = true;
		} else if (!strcmp("-fragmented",argv[arg]) && argc>arg) {
			cfg.fragmented = true;
		} else if (!strcmp("-otf",argv[arg]) && argc>arg) {
			cfg.otf = true;
		} else if (!strcmp("-direction",argv[arg]) && argc>arg) {
			arg++;
			cfg.direction = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-time_scale",argv[arg]) && argc>arg) {
			arg++;
			cfg.time_scale = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-fifo_size",argv[arg]) && argc>arg) {
			arg++;
			cfg.fifo_size_set = true;
			cfg.fifo_size = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-seek",argv[arg]) && argc>arg+1) {
			arg++;
			cfg.seek = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-seek2",argv[arg]) && argc>arg+1) {
			arg++;
			cfg.seek2 = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-feed",argv[arg]) && argc>arg+1) {
			arg++;
			cfg.feed = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-audio_codec",argv[arg]) && argc>arg+1) {
            unsigned track = audio_track>=0?audio_track:0;
			arg++;
			cfg.audio[track].codec = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-video_codec",argv[arg]) && argc>arg+1) {
			arg++;
			cfg.video.codec = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-slow_read_factor",argv[arg]) && argc>arg+1) {
			arg++;
			cfg.slow_read_factor = strtol(argv[arg],NULL,0);
		} else if (!strcmp("-benchmark",argv[arg]) && argc>arg+1) {
			arg++;
            run_test(argv[arg]);
            return 0;
		} else if (!strcmp("-type",argv[arg]) && argc>arg+1) {
			arg++;
			if(!strcasecmp(argv[arg],"mpeg1")) {
				cfg.stream_type = bstream_mpeg_type_mpeg1;
			} else if(!strcasecmp(argv[arg],"avi")) {
				cfg.stream_type = bstream_mpeg_type_avi;
			} else if(!strcasecmp(argv[arg],"mp4")) {
				cfg.stream_type = bstream_mpeg_type_mp4;
			} else if(!strcasecmp(argv[arg],"flv")) {
				cfg.stream_type = bstream_mpeg_type_flv;
			} else if(!strcasecmp(argv[arg],"asf")) {
				cfg.stream_type = bstream_mpeg_type_asf;
			} else if(!strcasecmp(argv[arg],"ts")) {
				cfg.stream_type = bstream_mpeg_type_ts;
			} else if(!strcasecmp(argv[arg],"mkv")) {
				cfg.stream_type = bstream_mpeg_type_mkv;
			} else if(!strcasecmp(argv[arg],"es")) {
				cfg.stream_type = bstream_mpeg_type_es;
			} else if(!strcasecmp(argv[arg],"rmff")) {
				cfg.stream_type = bstream_mpeg_type_rmff;
			} else if(!strcasecmp(argv[arg],"wav")) {
				cfg.stream_type = bstream_mpeg_type_wav;
			} else if(!strcasecmp(argv[arg],"mp4_fragment")) {
				cfg.stream_type = bstream_mpeg_type_mp4_fragment;
			} else if(!strcasecmp(argv[arg],"ogg")) {
				cfg.stream_type = bstream_mpeg_type_ogg;
			} else if(!strcasecmp(argv[arg],"cdxa")) {
				cfg.stream_type = bstream_mpeg_type_cdxa;
			} else if(!strcasecmp(argv[arg],"flac")) {
				cfg.stream_type = bstream_mpeg_type_flac;
			} else if(!strcasecmp(argv[arg],"ape")) {
				cfg.stream_type = bstream_mpeg_type_ape;
			} else {
				printf("Unknown stream type '%s'\n", argv[arg]);
				usage(argv[0], NULL);
			}
		} else {
			usage(argv[0], argv[arg]);
		}
		arg++;
	}
	if (!file || (!pes && !cfg.probe_only)) {
		usage(argv[0], NULL); 
	}
	if(!cfg.probe_only) {
		fprintf(stderr, "converting %s to %s %s", file, pes, cfg.player?"Using player ":"");
		if (cfg.video.stream_id) {
			fprintf(stderr, "video:%d[0x%02x] ", cfg.video.stream_id, cfg.video.pes_id);
		}
        for(i=0;i<MAX_AUDIO_TRACKS;i++) {
            if (cfg.audio[i].stream_id) {
                fprintf(stderr, "audio:%d[0x%02x] ", cfg.audio[i].stream_id, cfg.audio[i].pes_id);
            }
        }
	} else {
		if(strcmp(file,"-")!=0) {
			fprintf(stderr, "analyzing %s", file);
		}
	}
	fprintf(stderr, "\n");

	factory = batom_factory_create(bkni_alloc, 256);
	BDBG_ASSERT(factory);
	bfile_async_init();
	if(cfg.buffer) {
		bfile_buffered_cfg file_buffered_cfg;
		
		bfile_buffered_default_cfg(&file_buffered_cfg);
        file_buffered_cfg.nsegs = 256;
        file_buffered_cfg.buf_len = 256*4*BIO_BLOCK_SIZE;
		file_buffered = bfile_buffered_create(factory, &file_buffered_cfg);
	} else {
		file_buffered = NULL;
	}


    if(!cfg.probe_only || strcmp(file,"-")!=0) {
		fin = fopen(file,"rb");
		if (!fin) {
			perror(file);return 1;
		}
		raw_fd = bfile_stdio_read_attach(fin);
		BDBG_ASSERT(raw_fd);
		if(file_buffered) {
			fd = bfile_buffered_attach(file_buffered, raw_fd);
		} else {
			fd = raw_fd;
		}
	}	

	if(cfg.probe_only || cfg.probe || cfg.stream_type==bstream_mpeg_type_unknown) {
		bmedia_probe_t probe;
		bmedia_probe_config probe_config;
		const bmedia_probe_stream *stream;
		const bmedia_probe_track *track;
		char stream_info[512];
		bool iter = fin==NULL;

		probe = bmedia_probe_create();
		BDBG_ASSERT(probe);

		for(;;) {
			if(iter) {
				char *nl;
				file = fgets(pes_name, sizeof(pes_name), stdin);
				if(!file) {
					break;
				}
				nl = strchr(file, '\n');
				if(nl) {
					*nl = '\0';
				}
				fin = fopen(file,"rb");
				if(!fin) {
					continue;
				}
				fprintf(stdout, "%s :\t", file);
                fflush(stdout);
                raw_fd = bfile_stdio_read_attach(fin);
                BDBG_ASSERT(raw_fd);
                if(file_buffered) {
                    fd = bfile_buffered_attach(file_buffered, raw_fd);
                } else {
                    fd = raw_fd;
                }
			}
		
			bmedia_probe_default_cfg(&probe_config);
			probe_config.file_name = file;
			probe_config.type = cfg.stream_type;
			if(file_buffered) {
				probe_config.buffer = bfile_buffered_get_buffer(file_buffered);
			}
            if(cfg.stream_type==bstream_mpeg_type_cdxa) {
                bcdxa_file_t cdxa_file = bcdxa_file_create(fd);
                BDBG_ASSERT(cdxa_file);
                probe_config.type = bstream_mpeg_type_unknown;
                stream = bmedia_probe_parse(probe, bcdxa_file_get_file_interface(cdxa_file), &probe_config);
                bcdxa_file_destroy(cdxa_file);
            } else {
                stream = bmedia_probe_parse(probe, fd, &probe_config);
            }
			if(cfg.step) {
				getchar();
			}
			if(!stream) {
				fprintf(stdout, "unknown stream format\n");
				goto next;
			}
			bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
			fprintf(stdout,"%s\n", stream_info);
            fflush(stdout);
			if(!cfg.probe_only) {
		        unsigned audio_track[MAX_AUDIO_TRACKS];
		        unsigned video_track,track_no;
                BKNI_Memset(audio_track,0,sizeof(audio_track));
				cfg.stream_type = stream->type;
				for(video_track=0,track_no=1,track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link),track_no++) {
                    unsigned i;
					switch(track->type) {
					case bmedia_track_type_audio:
                        for(i=0;i<MAX_AUDIO_TRACKS;i++) {
                            if(audio_track[i]==0 && (cfg.audio[i].stream_id == 0 || cfg.audio[i].stream_id==track_no)) {
                                audio_track[i] = track->number;
                                cfg.audio[i].codec = track->info.audio.codec;
                                break;
                            }
                        }
						break;
					case bmedia_track_type_video:
						if(video_track==0 && (cfg.video.stream_id == 0 || cfg.video.stream_id==track_no)) {
							video_track = track->number;
                            cfg.video.codec = track->info.video.codec;
						}
						break;
                    case bmedia_track_type_other:
                        for(i=0;i<sizeof(cfg.other_streams)/sizeof(cfg.other_streams[0]);i++) {
                            if(cfg.other_streams[i]==0) {
                                cfg.other_streams[i] = track->number;
                                break;
                            }
                        }
                        break;
					default:
						break;
					}
				}
				BDBG_WRN(("Detected format:%u video:%u audio:%u",  cfg.stream_type, video_track, audio_track[0]));
				cfg.video.stream_id = video_track;
                for(i=0;i<MAX_AUDIO_TRACKS;i++) {
                    if(!audio_track[i]) {
                        break;
                    }
				    cfg.audio[i].stream_id = audio_track[i];
                }
				if(cfg.step) {
					getchar();
				}
			}
			bmedia_probe_stream_free(probe, stream);
next:
			if(!iter) {
				break;
			}
			if(file_buffered) {
				bfile_buffered_detach(file_buffered);
			}
			bfile_stdio_read_detach(raw_fd);
			fclose(fin);
			fin = NULL;
			raw_fd = NULL;
			fd = NULL;
		}
		bmedia_probe_destroy(probe);
	} 
    if(!cfg.probe_only) {
		b_media_feed_status status;
		fout = fopen(pes,"wb");
		if (!fout) {
			perror(pes);return 1;
		}
		media_feed(factory, fin, fout, fd, &cfg, &status);
		BDBG_WRN(("alloc_cnt: %u free_cnt: %u alloc_size: %u(%u:%u)", status.alloc_cnt, status.free_cnt, status.alloc_size, status.alloc_size_peak, status.size));
	}


	if(fin) {
		if(file_buffered) {
			bfile_buffered_detach(file_buffered);
		}
		bfile_stdio_read_detach(raw_fd);
		fclose(fin);
	}
	if(fout) {
		fclose(fout);
	}
	if(file_buffered) {
		bfile_buffered_destroy(file_buffered);
	}
	batom_factory_get_stats(factory, &factory_stats);
	BDBG_WRN(("status: atoms[live:%u allocated:%u freed:%u] alloc[pool:%u/%u arena:%u/%u alloc:%u/%u]", factory_stats.atom_live, factory_stats.atom_allocated, factory_stats.atom_freed, factory_stats.alloc_pool, factory_stats.free_pool, factory_stats.alloc_arena, factory_stats.free_arena, factory_stats.alloc_alloc, factory_stats.free_alloc));

	batom_factory_dump(factory);
	batom_factory_destroy(factory);
	bfile_async_shutdown();
	BDBG_Uninit();
	BKNI_Uninit();

	return 0;
}

