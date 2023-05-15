/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg1_probe.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 4/20/12 2:28p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg1_probe.c $
 * 
 * 14   4/20/12 2:28p vsilyaev
 * SW7405-4929: Added intentification/parsing of subtitle streams
 * 
 * 13   9/20/10 4:38p vsilyaev
 * SW7335-854: Better handle short streams
 * 
 * 12   9/20/10 4:04p vsilyaev
 * SW7335-854: Fixed dereference of the NULL pointer
 * 
 * 11   5/14/10 3:57p vsilyaev
 * SW3548-2918: Added range limit into the bmedia_video_scan_scode
 * function
 * 
 * 10   6/3/09 7:55p vsilyaev
 * PR 55417: Updated code for new semantics of the bfile_buffer_read
 * 
 * 9   4/10/09 5:23p mward
 * PR54060: Compiler warning building with DEBUG=n.
 * 
 * 8   3/6/09 11:25a vsilyaev
 * PR 52903: Verify result from batom_userdata
 * 
 * 7   1/20/09 4:20p gmohile
 * PR 50725 : Seperate mpeg1 pts parsing
 * 
 * 6   12/12/08 12:30p vsilyaev
 * PR 49306: Added code to detect streams bitrate
 * 
 * 5   11/17/08 7:28p vsilyaev
 * PR 49199: Added option to probe files from arbitrary file offset
 * 
 * 4   9/3/08 6:15p vsilyaev
 * PR 44493: Extended list of extension, use 'const' modifier for the
 * extension list
 * 
 * 3   7/25/07 6:28p vsilyaev
 * PR 32813: Added code to handle partially matches ES streams
 * 
 * 2   7/23/07 11:36a vsilyaev
 * PR 32813: Added MPEG-2 TS payload probe
 * 
 * 1   7/21/07 9:51p vsilyaev
 * PR 32813: MPEG1 probe module
 * 
  * 
 *******************************************************************************/
#include "bstd.h"
#include "bmpeg1_probe.h"
#include "bmpeg1_parser.h"
#include "bmedia_probe_demux.h"
#include "bkni.h"
#include "biobits.h"

BDBG_MODULE(bmpeg1_probe);

typedef struct bmpeg1_probe  *bmpeg1_probe_t; 

struct bmpeg1_probe {
	bmpeg1_parser_handler pack_handler; /* must be first */
	BDBG_OBJECT(bmpeg1_probe_t)
	batom_factory_t factory;
	bmedia_probe_demux demux;
	bmedia_probe_stream *stream;
	bmpeg1_parser_t parser;
    off_t file_offset;
    struct {
        bool valid;
        uint8_t stream_id;
        uint32_t first_pts;
        uint32_t last_pts;
        off_t first_offset;
        off_t last_offset;
    } file_pts;

};

BDBG_OBJECT_ID(bmpeg1_probe_t);

static bool 
b_mpeg1_probe_header_match(batom_cursor *header)
{
	bool result=false;
	uint32_t temp;
	/* ISO/IEC 11172-1: 1993 (E) , 2.4.3.2 Pack Layer */
	temp = batom_cursor_uint32_be(header);
	if(temp!=0x01BA) {
		goto fail;
	}
	temp = batom_cursor_uint32_be(header);
	if(B_GET_BITS(temp, 31,28) != 0x02) { 
		goto fail;
	}
	if(B_GET_BIT(temp, 24)==0 || B_GET_BIT(temp, 8)==0) { /* mark bits */
		goto fail;
	}
	temp = batom_cursor_uint32_be(header);
	if(B_GET_BIT(temp, 0)==0 || B_GET_BITS(temp, 24,23)!=0x3) { /* mark bits */
		goto fail;
	}
	temp = batom_cursor_uint32_be(header);
	if(temp!=0x01BB) {
		goto fail;
	}
	if(BATOM_IS_EOF(header)) {
		goto fail;
	}
	result = true;
fail:
	BDBG_MSG(("b_mpeg1_probe_header_match: %s at %u", result?"passed":"failed", batom_cursor_pos(header)));
	return result;

}


static bmpeg1_parser_action 
b_mpeg1_probe_packet(bmpeg1_parser_handler *handler, unsigned stream_id, batom_t packet)
{
	bmedia_track_type track_type;
	bmedia_probe_track *track;
	bmpeg1_probe_t	probe = (bmpeg1_probe_t)handler;
    const bmedia_packet_header *hdr;

	BDBG_OBJECT_ASSERT(probe, bmpeg1_probe_t);
    BDBG_ASSERT(packet);
	if((stream_id&0xE0)==0xC0) { /* audio stream */
		track_type = bmedia_track_type_audio;
	} else if( (stream_id&0xF0)==0xE0) { /* video stream */
		track_type = bmedia_track_type_video;
	}  else {
		batom_release(packet);
		goto done;
	}
    hdr = batom_userdata(packet);
    BDBG_ASSERT(hdr);
    if(hdr->pts_valid) {
        if(probe->file_pts.valid) {
            if(probe->file_pts.stream_id == stream_id) {
                probe->file_pts.last_pts = hdr->pts;
                probe->file_pts.last_offset = probe->file_offset;
            }
        } else {
            probe->file_pts.valid = true;
            probe->file_pts.stream_id = stream_id;
            probe->file_pts.first_pts = hdr->pts;
            probe->file_pts.first_offset = probe->file_offset;
            probe->file_pts.last_pts = hdr->pts;
            probe->file_pts.last_offset = probe->file_offset;
        }
    }
   
	track = bmedia_probe_demux_data(&probe->demux, probe->factory, stream_id, track_type, packet);
    if(track || (probe->file_pts.valid && probe->file_pts.stream_id == stream_id)) {
        bmedia_probe_stream *stream = probe->stream;
        if(!stream) {
            stream = BKNI_Malloc(sizeof(*stream));
            if(!stream) {
                BKNI_Free(track);
                goto done;
            }
            bmedia_probe_stream_init(stream, bstream_mpeg_type_mpeg1);
            probe->stream = stream;
        }
        if(track) {
            track->number = stream_id;
            bmedia_probe_add_track(stream, track);
        }
	}
done:
	return bmpeg1_parser_action_none;
}


static bmedia_probe_base_t 
b_mpeg1_probe_create(batom_factory_t factory)
{
	bmpeg1_probe_t	probe;
	bmpeg1_parser_cfg cfg;

	BDBG_ASSERT(factory);
	probe = BKNI_Malloc(sizeof(*probe));
	if(!probe) {
		BDBG_ERR(("b_mpeg1_probe_create: can't allocate %u bytes", sizeof(*probe)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(probe, bmpeg1_probe_t);
	probe->stream = NULL;
	probe->factory = factory;
	bmpeg1_parser_default_cfg(&cfg);
	probe->parser = bmpeg1_parser_create(factory, &cfg);
	if(!probe->parser) {
		goto err_parser;
	}
	bmpeg1_parser_install_handler(probe->parser, &probe->pack_handler, 0x00, 0x00, b_mpeg1_probe_packet);  /* get all payload */
	return (bmedia_probe_base_t)probe;
err_parser:
	BKNI_Free(probe);
err_alloc:
	return NULL;
}

static void 
b_mpeg1_probe_destroy(bmedia_probe_base_t probe_)
{
	bmpeg1_probe_t probe = (bmpeg1_probe_t)probe_;

	BDBG_OBJECT_ASSERT(probe, bmpeg1_probe_t);
	bmpeg1_parser_remove_handler(probe->parser, &probe->pack_handler);
	bmpeg1_parser_destroy(probe->parser);
	BDBG_OBJECT_DESTROY(probe, bmpeg1_probe_t);
	BKNI_Free(probe);
	return;
}

const bmedia_probe_stream *
b_mpeg1_probe_parse(bmedia_probe_base_t probe_, bfile_buffer_t buf, batom_pipe_t pipe, const bmedia_probe_parser_config *config)
{
	bmpeg1_probe_t probe = (bmpeg1_probe_t)probe_;
	off_t off;
	const size_t read_len = BMEDIA_PROBE_FEED_SIZE;
	bmedia_probe_stream *stream;

	BDBG_OBJECT_ASSERT(probe, bmpeg1_probe_t);
	BDBG_ASSERT(probe->stream==NULL);
	bmedia_probe_demux_init(&probe->demux);
    probe->file_pts.valid = false;

	for(off=0;;) {
		batom_t atom;
		bfile_buffer_result result;
		size_t feed_len;
		size_t atom_len;

		BDBG_MSG(("b_mpeg1_probe_parse: %#lx reading %u:%u", (unsigned long)probe, (unsigned)(config->parse_offset+off), read_len));
		atom = bfile_buffer_read(buf, config->parse_offset+off, read_len, &result);
		if(atom==NULL) {
			break;
		}
		atom_len = batom_len(atom);
		BDBG_MSG(("b_mpeg1_probe_parse: %#lx read %u:%u -> %#lx", (unsigned long)probe, (unsigned)(config->parse_offset+off), atom_len, (unsigned long)atom));
		off += atom_len;
		batom_pipe_push(pipe, atom);
        probe->file_offset = off;
		feed_len = bmpeg1_parser_feed(probe->parser, pipe);
		if( feed_len!=atom_len) {
			break;
		}
		if(off>128*1024 && probe->demux.tracks == probe->demux.completed) { /* if parsed >128K and probed all tracke */
			break;
		}
	}
	BDBG_MSG(("b_mpeg1_probe_parse: %#lx finished parsing at %u tracks:%u parsed:%u", (unsigned long)probe, (unsigned)off, probe->demux.tracks, probe->stream?probe->stream->ntracks:0));
	stream = probe->stream;
	probe->stream = NULL;
	if(stream) {
        bmedia_probe_demux_add_unknown(&probe->demux, stream, NULL);
        if(probe->file_pts.valid && probe->file_pts.first_pts < probe->file_pts.last_pts) {
            stream->max_bitrate = (8*45000*(probe->file_pts.last_offset - probe->file_pts.first_offset))/(probe->file_pts.last_pts - probe->file_pts.first_pts);
        }
    }
	bmpeg1_parser_reset(probe->parser);
	bmedia_probe_demux_shutdown(&probe->demux);
	/* return result of parsing */
	return stream;
}

static const bmedia_probe_file_ext b_mpeg1_ext[] =  {
	{"mpg"}, {"vcd"}, {"mpeg"},
	{""}
};

const bmedia_probe_format_desc bmpeg1_probe = {
	bstream_mpeg_type_mpeg1,
	b_mpeg1_ext, /* ext_list */
	4*4, /* mpeg1 header box shall be smaller then that */
	b_mpeg1_probe_header_match, /* header_match */
	b_mpeg1_probe_create, /* create */
	b_mpeg1_probe_destroy, /* destroy */
	b_mpeg1_probe_parse, /* parse */
	bmedia_probe_basic_stream_free /* stream free */
};


typedef struct b_mpeg1_stream_id {
	bmpeg1_parser_stream pes; /* must be first */
	struct b_mpeg1_pts_parser *parser;
	struct bmedia_timestamp *timestamp;
	off_t sync_offset;
	off_t data_offset;
} b_mpeg1_stream_id;


BDBG_OBJECT_ID(b_mpeg1_pts_parser);

typedef struct b_mpeg1_pts_parser {
	bmedia_timestamp_parser parent;
	BDBG_OBJECT(b_mpeg1_pts_parser)
	size_t packet_len;
	b_mpeg1_stream_id stream;
} b_mpeg1_pts_parser;


static void
b_mpeg1_pts_parser_destroy(bmedia_timestamp_parser_t parser_)
{
	b_mpeg1_pts_parser *parser = (b_mpeg1_pts_parser *)parser_;

	BDBG_OBJECT_DESTROY(parser, b_mpeg1_pts_parser);
	BKNI_Free(parser);
	return;
}


static int 
b_mpeg1_pts_parser_parse(bmedia_timestamp_parser_t parser_, batom_cursor *cursor, bmedia_timestamp *timestamp)
{
	b_mpeg1_pts_parser *parser = (b_mpeg1_pts_parser *)parser_;
	off_t next_data;
	int rc;
	
	BDBG_OBJECT_ASSERT(parser, b_mpeg1_pts_parser);
	BDBG_ASSERT(cursor);
	BDBG_ASSERT(timestamp);

	next_data = parser->stream.data_offset + batom_cursor_size(cursor);
	parser->stream.timestamp = timestamp;

	for(;;) {
		rc = bmpeg1_parser_stream_feed(&parser->stream.pes, NULL, cursor);
		if(rc>=0) { /* some other pid */
			parser->stream.sync_offset = parser->stream.data_offset + batom_cursor_pos(cursor);			
		} else if(rc==BMPEG1_RESULT_HOLD) {
			BDBG_ASSERT(parser->stream.timestamp==NULL); /* verify that we payload handler handled timestamp */
			parser->stream.sync_offset = parser->stream.data_offset + batom_cursor_pos(cursor);
			rc = 1;
			break;
		} else if(rc==BMPEG1_RESULT_EOS) {
			rc = 0;
			break;
		} else if(rc==BMPEG1_RESULT_SYNC_ERROR) {
			batom_cursor packet_start;
			
			parser->stream.pes.flags = 0;				

			for(;;) {
				unsigned scode=0;			
				BATOM_CLONE(&packet_start, cursor);				
				scode = BATOM_UINT32_BE(&packet_start);				
				if(((scode & 0xFFFFFF00) == 0x100) && (b_mpeg1_test_stream_id(scode&0xFF))){			
					break;
				} else if(BATOM_IS_EOF(&packet_start)) {
					rc = 0;
					goto done;
				}
				BATOM_NEXT(scode, cursor); /* advance scanning position */
				BDBG_ASSERT((int)scode!=BATOM_EOF);
			}
		}
	}
done:
	parser->stream.data_offset = next_data;
	
	return rc;
}


static void 
b_mpeg1_pts_parser_seek(bmedia_timestamp_parser_t parser_, off_t offset)
{
	b_mpeg1_pts_parser *parser = (b_mpeg1_pts_parser *)parser_;
	BDBG_OBJECT_ASSERT(parser, b_mpeg1_pts_parser);

	BDBG_ASSERT(offset>=0);
	
	parser->stream.data_offset = offset;
	parser->stream.pes.flags = 0;

	return;
}


static const bmedia_timestamp_parser_methods  b_mpeg1_pts_parser_methods = {
	b_mpeg1_pts_parser_seek,
	b_mpeg1_pts_parser_parse,
	b_mpeg1_pts_parser_destroy
};


static bmpeg1_parser_action
b_mpeg1_pts_parser_pes_payload(bmpeg1_parser_stream *stream, unsigned flags, batom_accum_t src, batom_cursor *cursor, size_t len)
{
	b_mpeg1_stream_id *stream_id= (b_mpeg1_stream_id*)stream;	

	BSTD_UNUSED(src);
	BSTD_UNUSED(len);
#if !BDBG_DEBUG_BUILD
    BSTD_UNUSED(flags);
#endif
	BDBG_MSG(("b_mpeg1_pts_parser_pes_payload: %#lx pid:%#x  flags %#x", stream_id->parser, stream_id->pes.stream_id, (unsigned)flags));
	
	BDBG_ASSERT(stream_id->timestamp);
	stream_id->timestamp->timestamp = stream->pts;
	stream_id->timestamp->offset = stream_id->data_offset + batom_cursor_pos(cursor);
	stream_id->timestamp = NULL; /* clear timestamp, it signals that timestamp was handled and is usefull to detect multiple PCR detections */
	
	return bmpeg1_parser_action_return;
}

bmedia_timestamp_parser_t 
bmpeg1_pts_parser_create(uint16_t stream_id, size_t packet_len)
{
	b_mpeg1_pts_parser *parser;
	BERR_Code rc;

	parser = BKNI_Malloc(sizeof(*parser));

	if(!parser) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_alloc;}

	BDBG_OBJECT_INIT(parser, b_mpeg1_pts_parser);
	BDBG_MSG(("bmpeg1_pts_parser_create: %#lx stream id:%#x packet_len:%u", (unsigned long)parser, (unsigned)stream_id, packet_len));
	parser->parent.methods = &b_mpeg1_pts_parser_methods;
	parser->packet_len = packet_len;
	bmpeg1_parser_stream_init(&parser->stream.pes, stream_id);
	parser->stream.parser = parser;
	parser->stream.pes.payload = b_mpeg1_pts_parser_pes_payload; 
	parser->stream.sync_offset = 0;
	parser->stream.data_offset = 0;
	parser->stream.timestamp = NULL;
	
	return &parser->parent;

err_alloc:
	return NULL;
}
