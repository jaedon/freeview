/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg2ts_probe.c $
 * $brcm_Revision: 24 $
 * $brcm_Date: 4/20/12 2:27p $
 *
 * Module Description:
 *
 * MPEG-2 TS Parser/Demux library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg2ts_probe.c $
 * 
 * 24   4/20/12 2:27p vsilyaev
 * SW7405-4929: Added intentification/parsing of subtitle streams
 * 
 * 23   12/30/10 1:16p vsilyaev
 * SWBLURAY-23899: Merged support to control min size of parsed data.
 * 
 * fix_bdvd_b3pvr_SWBLURAY-23899/1   12/21/10 12:08p ialauder
 * Allow the size of data to parse to be be configuriable.
 * 
 * 18   4/23/10 2:12p vsilyaev
 * SWBLURAY-19982: Improved resyncing in the MPEG-2 TS stream
 * 
 * fix_bdvd_v3.0_issue19982/1   4/23/10 4:42p ialauder
 * Improve code to find TS sync bytes
 * 
 * 17   6/3/09 7:55p vsilyaev
 * PR 55417: Updated code for new semantics of the bfile_buffer_read
 * 
 * 16   3/18/09 10:21p vsilyaev
 * PR 53376: Skip junk that could preceed transport sync byte
 * 
 * 15   12/16/08 10:28a vsilyaev
 * PR 50148: Improved parsing of H.264 streams
 * 
 * 14   11/17/08 7:28p vsilyaev
 * PR 49199: Added option to probe files from arbitrary file offset
 * 
 * 13   11/12/08 12:37a vsilyaev
 * PR 48182: Improved robustness of bitrate/duration detection
 * 
 * 12   11/11/08 9:49p vsilyaev
 * PR 48182: Added code to detect stream duration, using parsed timestamps
 * 
 * 11   11/6/08 9:28a vsilyaev
 * PR 48318: Fixed dynamic memory management
 * 
 * 10   11/5/08 4:16p vsilyaev
 * PR 48701: Added test for duplicate PCR pids
 * 
 * 9   10/2/08 10:28a vsilyaev
 * PR 32813: Added 'tp' extension
 * 
 * 8   9/3/08 6:14p vsilyaev
 * PR 44493: Extended list of extension, use 'const' modifier for the
 * extension list
 * 
 * 7   7/21/08 1:18p vsilyaev
 * PR44493: Add support for 192 byte TS packets
 * 
 * PR44493/2   7/18/08 10:25p jtna
 * PR44493: Add support for 192 byte TS packets
 * 
 * PR44493/1   7/18/08 4:53p jtna
 * PR44493: Add support for 192 byte TS packets
 * 
 * 6   8/17/07 3:56p vsilyaev
 * PR 33812: Improved handling of resyncs in parsing
 * 
 * 5   7/25/07 6:28p vsilyaev
 * PR 32813: Added code to handle partially matches ES streams
 * 
 * 4   7/24/07 11:27p vsilyaev
 * PR 32813: Added PCR parsing and bitrate estimation
 * 
 * 3   7/24/07 7:20p vsilyaev
 * PR 32813: Added MPEG-2 PES probe
 * 
 * 2   7/23/07 5:53p vsilyaev
 * PR 32813: Fixed heap corruption
 * 
 * 1   7/23/07 11:33a vsilyaev
 * PR 32813: MPEG2-TS payload probe module
 * 
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bmpeg2ts_probe.h"
#include "bmpeg2ts_parser.h"
#include "bmpeg2pes_parser.h"
#include "bmedia_probe_demux.h"
#include "blst_slist.h"
#include "bkni.h"

BDBG_MODULE(bmpeg2ts_probe);
#define BDBG_MSG_TRACE(x)	/* BDBG_MSG(x) */

BDBG_OBJECT_ID(bmpeg2ts_probe_t);

typedef struct bmpeg2ts_probe *bmpeg2ts_probe_t; 

typedef struct b_mpeg2ts_probe_pid {
	bmpeg2ts_parser_pid ts; /* must be first */
	bmpeg2pes_parser pes;
	batom_accum_t pes_accum;
	enum {
		b_mpeg2ts_probe_state_pes, /* looking for PES data */
		b_mpeg2ts_probe_state_parse, /* scanning ES data */
		b_mpeg2ts_probe_state_done   /* done scanning */
	} state;
	bool pcr_pid;
	struct {
		uint32_t pcr;
		off_t off;
	} first,last;
	bmpeg2ts_probe_t probe;
	BLST_S_ENTRY(b_mpeg2ts_probe_pid) link;
} b_mpeg2ts_probe_pid;

struct bmpeg2ts_probe {
	BDBG_OBJECT(bmpeg2ts_probe_t)
	bmedia_probe_stream *stream;
	batom_factory_t factory;
	bmpeg2ts_parser_t parser;
	BLST_S_HEAD(b_mpeg2ts_probe_pids, b_mpeg2ts_probe_pid) pids;
	unsigned npids;
	bmedia_probe_demux demux;
	unsigned pkt_len;
	bool completed_pcr;
};

static bool 
bmpeg2ts_probe_header_match_len(batom_cursor *cursor, unsigned header_size)
{
	unsigned pkt,off;
    int byte;
    BDBG_CASSERT(BMPEG2TS_PROBE_PACKETS>0);

    if (bmpeg2ts_parser_sync_header(cursor,header_size, &off)) {
        if(off>BMPEG2TS_PKT_LEN*(BMPEG2TS_PROBE_PACKETS-1)) {
            return false;
        }
		batom_cursor_skip(cursor, header_size + BMPEG2TS_PKT_LEN); /* skip first packet */
	    for(pkt=0;pkt<BMPEG2TS_PROBE_PACKETS-1;pkt++) {

		    batom_cursor_skip(cursor, header_size); /* skip 4 byte timestamp */
            byte = batom_cursor_next(cursor);
		    if(byte==BMPEG2TS_PKT_SYNC) {
			    BDBG_CASSERT(BMPEG2TS_PKT_LEN>1);
			    batom_cursor_skip(cursor, BMPEG2TS_PKT_LEN-1);
			    continue;
		    } else if(byte==BATOM_EOF) {
			    break;
		    } else {
			    return false;
		    }
	    }
	    return pkt>0;
    } else {
        return false;
    }


}

bool 
bmpeg2ts_probe_header_match(batom_cursor *cursor)
{
	return bmpeg2ts_probe_header_match_len(cursor, 0);
}

bool 
bmpeg2ts192_probe_header_match(batom_cursor *cursor)
{
	return bmpeg2ts_probe_header_match_len(cursor, 4);
}

static bmpeg2ts_parser_action 
b_mpeg2ts_probe_ts_payload(bmpeg2ts_parser_pid *pid, unsigned flags, batom_accum_t src, batom_cursor *cursor, size_t len)
{
	b_mpeg2ts_probe_pid *probe_pid= (b_mpeg2ts_probe_pid *)pid;
	BDBG_MSG_TRACE(("b_mpeg2ts_probe_ts_payload: %#lx pid:%#x %u bytes", probe_pid->probe, probe_pid->ts.pid, len));
	if(flags&BMPEG2TS_PCR_FLAG) {
		bmpeg2ts_parser_status status;
		bmpeg2ts_parser_get_status(probe_pid->probe->parser, &status);
		probe_pid->last.pcr = pid->pcr_base;
		probe_pid->last.off = status.offset;
	    BDBG_MSG(("b_mpeg2ts_probe_ts_payload: %#lx pid:%#x pcr:%#lx(%#lx) ", probe_pid->probe, probe_pid->ts.pid, (unsigned)pid->pcr_base, (unsigned)status.offset));
		if(!probe_pid->pcr_pid || probe_pid->first.pcr > pid->pcr_base ) {
			probe_pid->pcr_pid = true;
			probe_pid->first.pcr = pid->pcr_base;
			probe_pid->first.off = status.offset;
		} else {
			probe_pid->probe->completed_pcr = true;
		}
	}
	if(probe_pid->state != b_mpeg2ts_probe_state_done) {
		return bmpeg2pes_parser_feed(&probe_pid->pes, flags, src, cursor, len);
	} else {
		return bmpeg2ts_parser_action_skip;
	}
}

static void 
b_mpeg2ts_probe_pes_packet(void *packet_cnxt, batom_accum_t src, batom_cursor *payload, size_t len, const bmpeg2pes_atom_info *info)
{
	b_mpeg2ts_probe_pid *probe_pid = packet_cnxt;
	bmpeg2ts_probe_t probe  = probe_pid->probe;
	unsigned stream_id;
	bmedia_track_type track_type;
	batom_cursor payload_start;
	batom_t packet = NULL;
	batom_accum_t pes_accum;
	size_t accum_len;

	BDBG_OBJECT_ASSERT(probe, bmpeg2ts_probe_t);
	BDBG_MSG_TRACE(("b_mpeg2ts_probe_pes_packet: %#lx stream %#x:%#x pes data %u:%u", (unsigned long)probe, (unsigned)probe_pid->ts.pid, (unsigned)info->pes_id, info->data_offset, len));
	BATOM_CLONE(&payload_start, payload);
	pes_accum = probe_pid->pes_accum;
	accum_len = batom_accum_len(pes_accum);
	if( accum_len>(8*1024) || (accum_len>0 && info->data_offset == 0)) {
		packet = batom_from_accum(pes_accum, NULL, NULL); 
	}
	batom_cursor_skip(payload, len);
	batom_accum_append(pes_accum, src, &payload_start, payload);
	if(packet) {
		bmedia_probe_track *track;
		BDBG_MSG(("b_mpeg2ts_probe_pes_packet: %#lx stream %#x:%#x pes payload %#lx:%u", (unsigned long)probe, (unsigned)probe_pid->ts.pid, (unsigned)info->pes_id, packet, packet?batom_len(packet):0));
		stream_id = info->pes_id;
		if((stream_id&0xE0)==0xC0) { /* audio stream */
			track_type = bmedia_track_type_audio;
		} else if( (stream_id&0xF0)==0xE0) { /* video stream */
			track_type = bmedia_track_type_video;
		} else {
			track_type = bmedia_track_type_other;
		}
		track = bmedia_probe_demux_data(&probe->demux, probe->factory, probe_pid->ts.pid, track_type, packet);
		if(track==NULL) {
			probe_pid->state = b_mpeg2ts_probe_state_parse;
		} else {
			probe_pid->state = b_mpeg2ts_probe_state_done;
		    batom_accum_clear(probe_pid->pes_accum);
			BDBG_ASSERT(probe->stream);
			bmedia_probe_add_track(probe->stream, track);
		}
	}
	return;
}


static bmpeg2ts_parser_action 
b_mpeg2ts_probe_new_pid(void *application_cnxt, uint16_t pid)
{
	bmpeg2ts_probe_t probe  = application_cnxt;
	b_mpeg2ts_probe_pid *probe_pid;
	int rc;

	BDBG_MSG(("b_mpeg2ts_probe_pid: %#lx new pid %#x", (unsigned long)probe, (unsigned)pid));

	BDBG_OBJECT_ASSERT(probe, bmpeg2ts_probe_t);
	/* ISO/IEC 13818-1 : 2000 (E) Table 2-3 . PID table */
	if(pid<0x10)  {
		goto skip;
	}

	probe_pid = BKNI_Malloc(sizeof(*probe_pid));
	if(!probe_pid) {
		goto err_alloc;
	}
	bmpeg2ts_parser_pid_init(&probe_pid->ts, pid);
	rc = bmpeg2pes_parser_init(probe->factory, &probe_pid->pes, BMPEG2PES_ID_ANY); /* any stream id */
	if(rc<0) {
		goto err_pes;
	}
	probe_pid->pes_accum = batom_accum_create(probe->factory);
	if(!probe_pid->pes_accum) {
		goto err_accum;
	}
	bmpeg2ts_parser_add_pid(probe->parser, &probe_pid->ts);
	probe_pid->probe = probe;
	probe_pid->ts.payload = b_mpeg2ts_probe_ts_payload; 
	probe_pid->state = b_mpeg2ts_probe_state_pes;
	probe_pid->pcr_pid = false;
	probe_pid->pes.packet_cnxt = probe_pid;
	probe_pid->pes.packet = b_mpeg2ts_probe_pes_packet;
	BLST_S_INSERT_HEAD(&probe->pids, probe_pid, link);
	probe->npids++;
	return bmpeg2ts_parser_action_consume;

err_accum:
	bmpeg2pes_parser_shutdown(&probe_pid->pes); 
err_pes:
	BKNI_Free(probe_pid);
err_alloc:
skip:
	return bmpeg2ts_parser_action_skip;

}

static bmedia_probe_base_t 
b_mpeg2ts_probe_create_len(batom_factory_t factory, unsigned header_size)
{
	bmpeg2ts_probe_t probe;
	bmpeg2ts_parser_cfg parser_cfg;

	probe = BKNI_Malloc(sizeof(*probe));
	if(!probe) {
		BDBG_ERR(("b_mpeg2ts_probe_create: can't allocate %u bytes", sizeof(*probe)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(probe, bmpeg2ts_probe_t);
	probe->stream = NULL;
	probe->factory = factory;
	BLST_S_INIT(&probe->pids);
	probe->npids = 0;
	probe->pkt_len = BMPEG2TS_PKT_LEN+header_size;
	bmpeg2ts_parser_default_cfg(&parser_cfg);
	parser_cfg.application_cnxt = probe;
	parser_cfg.unknown_pid = b_mpeg2ts_probe_new_pid;

	parser_cfg.header_size = header_size;
	probe->parser = bmpeg2ts_parser_create(factory, &parser_cfg);
	if(!probe->parser) {
		goto err_parser;
	}
	return (bmedia_probe_base_t)probe;

err_parser:
	BKNI_Free(probe);
err_alloc:
	return NULL;
}

static bmedia_probe_base_t 
b_mpeg2ts_probe_create(batom_factory_t factory)
{
	return b_mpeg2ts_probe_create_len(factory, 0);
}

static bmedia_probe_base_t 
b_mpeg2ts192_probe_create(batom_factory_t factory)
{
	return b_mpeg2ts_probe_create_len(factory, 4);
}

static void 
b_mpeg2ts_probe_destroy(bmedia_probe_base_t probe_)
{
	bmpeg2ts_probe_t probe = (bmpeg2ts_probe_t)probe_;

	BDBG_OBJECT_ASSERT(probe, bmpeg2ts_probe_t);
	BDBG_ASSERT(probe->stream==NULL); /* can't destroy probe in middle of parsing */

	bmpeg2ts_parser_destroy(probe->parser);
	BDBG_OBJECT_DESTROY(probe, bmpeg2ts_probe_t);
	BKNI_Free(probe);
	return;
}

const bmedia_probe_stream *
b_mpeg2ts_probe_parse(bmedia_probe_base_t probe_, bfile_buffer_t buf, batom_pipe_t pipe, const bmedia_probe_parser_config *config)
{
	bmpeg2ts_probe_t probe = (bmpeg2ts_probe_t)probe_;
	bmpeg2ts_probe_stream *stream;
	off_t off=0;
    off_t min_parse_off = config->min_parse_request;
	bmpeg2ts_parser_t parser;
	b_mpeg2ts_probe_pid *probe_pid;


	BDBG_OBJECT_ASSERT(probe, bmpeg2ts_probe_t);

	BDBG_ASSERT(probe->stream==NULL);
	parser = probe->parser;

	stream = BKNI_Malloc(sizeof(*stream));
	if(!stream) {
        (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto done;
	}
	bmedia_probe_stream_init(&stream->media, bstream_mpeg_type_ts);
	probe->stream = &stream->media;
	bmedia_probe_demux_init(&probe->demux);
	probe->completed_pcr = false;

	for(;;) {
		batom_t atom;
		bfile_buffer_result result;
		size_t feed_len;
		size_t atom_len;
		bmpeg2ts_parser_status status;
		const size_t read_len = BMEDIA_PROBE_FEED_SIZE;

		BDBG_MSG(("b_mpeg2ts_probe_parse: %#lx reading %u:%u", (unsigned long)probe, (unsigned)(config->parse_offset+off), read_len));
		atom = bfile_buffer_read(buf, config->parse_offset+off, read_len, &result);
		if(atom==NULL) {
			BDBG_MSG(("b_mpeg2ts_probe_parse: %#lx read completed %u at %u", (unsigned long)probe, result, (unsigned)(config->parse_offset+off)));
			break;
		}
		atom_len = batom_len(atom);
		BDBG_MSG(("b_mpeg2ts_probe_parse: %#lx read %u:%u -> %#lx", (unsigned long)probe, (unsigned)(config->parse_offset+off), atom_len, (unsigned long)atom));
		off += atom_len;
		batom_pipe_push(pipe, atom);
		feed_len = bmpeg2ts_parser_feed(parser, pipe);
		if(feed_len!=atom_len) {
			break;
		}
		bmpeg2ts_parser_get_status(parser, &status);
		if(status.nresyncs>8 || status.resync_bytes > 8*BMPEG2TS_PKT_LEN) {
			BDBG_MSG(("b_mpeg2ts_psi_probe_parse: %#lx too much(%u:%u) resyncs ",  (unsigned long)probe, status.nresyncs, status.resync_bytes));
			break;
		}
		BDBG_MSG(("b_mpeg2ts_probe_parse: %#lx parsed:%u pids:%u tracks:%u completed:%u", (unsigned long)probe, (unsigned)off, probe->npids, probe->demux.tracks, probe->demux.completed));
		if(
			(off>min_parse_off && probe->demux.tracks == probe->demux.completed && probe->demux.tracks==probe->npids && probe->completed_pcr) ||/* parsed and probed all pids */
			(off>3*1024*1024 && probe->demux.tracks == probe->demux.completed && probe->completed_pcr) /* parsed >3MBytes and probed all tracks */ ||
            (off>8*1024*1024) /* parsed >8MBytes */
			) { 
			break;
		}
	} 
	BDBG_MSG(("b_mpeg2ts_probe_parse: %#lx parsed %u bytes", (unsigned long)probe, (unsigned)off));
	bmedia_probe_demux_add_unknown(&probe->demux, &stream->media, NULL);
	bmedia_probe_demux_shutdown(&probe->demux);
    if(BLST_S_FIRST(&probe->pids)==NULL) {
        BKNI_Free(stream);
        stream = NULL;
        goto done;
    }
	while( NULL!=(probe_pid = BLST_S_FIRST(&probe->pids))) {
		bmedia_probe_track *track;

		BLST_S_REMOVE_HEAD(&probe->pids, link);
		bmpeg2ts_parser_remove_pid(parser, probe_pid->ts.pid);
		bmpeg2pes_parser_shutdown(&probe_pid->pes);
		batom_accum_destroy(probe_pid->pes_accum);
		if(probe_pid->pcr_pid) {
			track = BKNI_Malloc(sizeof(*track));
			if(track) {
				bmedia_probe_track_init(track);
				track->type = bmedia_track_type_pcr;
				track->number = probe_pid->ts.pid;
				bmedia_probe_add_track(&stream->media, track);
				if(probe_pid->last.pcr!=probe_pid->first.pcr) {
					unsigned bitrate;
					bitrate = (8*45000*(probe_pid->last.off - probe_pid->first.off))/(probe_pid->last.pcr - probe_pid->first.pcr);
					if(bitrate>stream->media.max_bitrate) {
						stream->media.max_bitrate = bitrate;
					}
				}
			}
		}
		BKNI_Free(probe_pid);
	}
	stream->pkt_len = probe->pkt_len;

done:
	probe->npids = 0;

    bmpeg2ts_parser_reset(parser);

	probe->stream = NULL;
	return &stream->media;
}


const bmedia_probe_file_ext bmpeg2ts_probe_ext[] =  {
	{"ts"},{"mpg"},{"trp"},{"mpeg"},{"tp"},
	{""}
};

const bmedia_probe_file_ext bmpeg2ts192_probe_ext[] =  {
	{"ts"},{"mpg"},{"trp"},{"mpeg"},{"tp"},{"m2ts"}, {"mts"},
	{""}
};

const bmedia_probe_format_desc bmpeg2ts_probe = {
	bstream_mpeg_type_ts,
	bmpeg2ts_probe_ext, /* ext_list */
	BMPEG2TS_PKT_LEN*BMPEG2TS_PROBE_PACKETS, /* read several transport packets */
	bmpeg2ts_probe_header_match, /* header_match */
	b_mpeg2ts_probe_create, /* create */
	b_mpeg2ts_probe_destroy, /* destroy */
	b_mpeg2ts_probe_parse, /* parse */
	bmedia_probe_basic_stream_free /* stream free */
};

const bmedia_probe_format_desc bmpeg2ts192_probe = {
	bstream_mpeg_type_ts,
	bmpeg2ts192_probe_ext, /* ext_list */
	(BMPEG2TS_PKT_LEN+4)*BMPEG2TS_PROBE_PACKETS, /* read several transport packets */
	bmpeg2ts192_probe_header_match, /* header_match */
	b_mpeg2ts192_probe_create, /* create */
	b_mpeg2ts_probe_destroy, /* destroy */
	b_mpeg2ts_probe_parse, /* parse */
	bmedia_probe_basic_stream_free /* stream free */
};

typedef struct b_mpeg2ts_pcr_pid {
	bmpeg2ts_parser_pid ts; /* must be first */
	struct b_mpeg2ts_pcr_parser *parser;
	struct bmedia_timestamp *timestamp;
	off_t sync_offset;
	off_t data_offset;
} b_mpeg2ts_pcr_pid;

BDBG_OBJECT_ID(b_mpeg2ts_pcr_parser);

typedef struct b_mpeg2ts_pcr_parser {
	bmedia_timestamp_parser parent;
	BDBG_OBJECT(b_mpeg2ts_pcr_parser)
	size_t packet_len;
	b_mpeg2ts_pcr_pid pcr_pid;
} b_mpeg2ts_pcr_parser;


static void
b_mpeg2ts_pcr_parser_destroy(bmedia_timestamp_parser_t parser_)
{
	b_mpeg2ts_pcr_parser *parser = (b_mpeg2ts_pcr_parser *)parser_;

	BDBG_OBJECT_DESTROY(parser, b_mpeg2ts_pcr_parser);
	BKNI_Free(parser);
	return;
}



static int 
b_mpeg2ts_pcr_parser_parse(bmedia_timestamp_parser_t parser_, batom_cursor *cursor, bmedia_timestamp *timestamp)
{
	b_mpeg2ts_pcr_parser *parser = (b_mpeg2ts_pcr_parser *)parser_;
	int skip;
	off_t next_data;
	int rc;

	BDBG_OBJECT_ASSERT(parser, b_mpeg2ts_pcr_parser);
	BDBG_ASSERT(cursor);
	BDBG_ASSERT(timestamp);

	next_data = parser->pcr_pid.data_offset + batom_cursor_size(cursor);
	parser->pcr_pid.timestamp = timestamp;
	/* 1. Resync with expected packet location */
	skip = (parser->pcr_pid.data_offset - parser->pcr_pid.sync_offset)%(int)parser->packet_len;
	BDBG_MSG_TRACE(("b_mpeg2ts_pcr_parser_parse: %#lx %u %u %d", (unsigned long)parser, (unsigned)parser->pcr_pid.sync_offset, (unsigned)parser->pcr_pid.data_offset, skip, parser->packet_len));
	if(skip!=0) {
		if(skip<0) {
			skip = -skip;
		} else {
			skip = parser->packet_len - skip;
		}
		BDBG_ASSERT(skip>0);
		batom_cursor_skip(cursor, skip);
		parser->pcr_pid.ts.flags = 0;
		parser->pcr_pid.ts.continuity_counter = 0;
		parser->pcr_pid.ts.npackets = 0;
	}
	for(;;) {
		rc = bmpeg2ts_parser_pid_feed(&parser->pcr_pid.ts, NULL, cursor);
		if(rc>=0) { /* some other pid */
			parser->pcr_pid.sync_offset = parser->pcr_pid.data_offset + batom_cursor_pos(cursor);
			batom_cursor_skip(cursor, parser->packet_len);
		} else if(rc==BMPEG2TS_RESULT_HOLD) {
			parser->pcr_pid.sync_offset = parser->pcr_pid.data_offset + batom_cursor_pos(cursor);
			rc = 1;
			break;
		} else if(rc==BMPEG2TS_RESULT_EOS) {
			rc = 0;
			break;
		} else if(rc==BMPEG2TS_RESULT_SYNC_ERROR) {
            unsigned skip_bytes = 0;
            if(parser->pcr_pid.ts.error_count>=BMPEG2TS_MAX_ERROR_COUNT) {
                goto done;
            }

            parser->pcr_pid.ts.flags = 0;
            parser->pcr_pid.ts.continuity_counter = 0;
            parser->pcr_pid.ts.npackets = 0;
            if(!bmpeg2ts_parser_sync_header(cursor, parser->pcr_pid.ts.header_size, &skip_bytes)) {
                goto done;
			}
		}
	}
done:
	parser->pcr_pid.data_offset = next_data;
	BDBG_MSG_TRACE(("b_mpeg2ts_pcr_parser_parse: %#lx %u %u -> %u:%u", (unsigned long)parser, (unsigned)parser->pcr_pid.sync_offset, (unsigned)parser->pcr_pid.data_offset, (unsigned)timestamp->offset, (unsigned)timestamp->timestamp));
	return rc;
}

static void 
b_mpeg2ts_pcr_parser_seek(bmedia_timestamp_parser_t parser_, off_t offset)
{
	b_mpeg2ts_pcr_parser *parser = (b_mpeg2ts_pcr_parser *)parser_;
	BDBG_OBJECT_ASSERT(parser, b_mpeg2ts_pcr_parser);

	BDBG_ASSERT(offset>=0);
	parser->pcr_pid.data_offset = offset;
	parser->pcr_pid.ts.flags = 0;
	parser->pcr_pid.ts.continuity_counter = 0;
	parser->pcr_pid.ts.npackets = 0;
	return;
}

static const bmedia_timestamp_parser_methods  b_mpeg2ts_pcr_parser_methods = {
	b_mpeg2ts_pcr_parser_seek,
	b_mpeg2ts_pcr_parser_parse,
	b_mpeg2ts_pcr_parser_destroy
};

static bmpeg2ts_parser_action 
b_mpeg2ts_pcr_parser_ts_payload(bmpeg2ts_parser_pid *pid, unsigned flags, batom_accum_t src, batom_cursor *cursor, size_t len)
{
	b_mpeg2ts_pcr_pid *pcr_pid= (b_mpeg2ts_pcr_pid*)pid;
	BSTD_UNUSED(cursor);
	BSTD_UNUSED(len);
	BSTD_UNUSED(src);
	BDBG_MSG_TRACE(("b_mpeg2ts_pcr_parser_ts_payload: %#lx pid:%#x %u bytes flags %#x", pcr_pid->parser, pcr_pid->ts.pid, len, (unsigned)flags));
	if(flags&BMPEG2TS_PCR_FLAG) {
		BDBG_ASSERT(pcr_pid->timestamp);
		pcr_pid->timestamp->timestamp = pid->pcr_base;
		pcr_pid->timestamp->offset = pcr_pid->data_offset + batom_cursor_pos(cursor);
		pcr_pid->timestamp = NULL; /* clear timestamp, it signals that timestamp was handled and is usefull to detect multiple PCR detections */
		return bmpeg2ts_parser_action_hold;
	}
	return bmpeg2ts_parser_action_skip;
}


bmedia_timestamp_parser_t 
bmpeg2ts_pcr_parser_create(uint16_t pid, size_t packet_len)
{
	b_mpeg2ts_pcr_parser *parser;
	BERR_Code rc;

	if(packet_len<BMPEG2TS_PKT_LEN) { rc = BERR_TRACE(BERR_INVALID_PARAMETER);goto err_size;}

	parser = BKNI_Malloc(sizeof(*parser));
	if(!parser) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_alloc;}

	BDBG_OBJECT_INIT(parser, b_mpeg2ts_pcr_parser);
	BDBG_MSG(("bmpeg2ts_pcr_parser_create: %#lx pid:%#x packet_len:%u", (unsigned long)parser, (unsigned)pid, packet_len));
	parser->parent.methods = &b_mpeg2ts_pcr_parser_methods;
	parser->packet_len = packet_len;
	bmpeg2ts_parser_pid_init(&parser->pcr_pid.ts, pid);
	parser->pcr_pid.parser = parser;
	parser->pcr_pid.ts.payload = b_mpeg2ts_pcr_parser_ts_payload; 
	parser->pcr_pid.sync_offset = 0;
	parser->pcr_pid.data_offset = 0;
	parser->pcr_pid.timestamp = NULL;
	parser->pcr_pid.ts.header_size = packet_len - BMPEG2TS_PKT_LEN;
	return &parser->parent;

err_alloc:
err_size:
	return NULL;
}

