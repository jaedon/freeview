/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg2pes_parser.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 1/19/11 4:54p $
 *
 * Module Description:
 *
 * MPEG-2 PES Parser/Demux library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg2pes_parser.c $
 * 
 * 14   1/19/11 4:54p vsilyaev
 * SWGERRARD-1457: Reduced noise on EOS
 * 
 * 13   1/6/10 4:37p gmohile
 * SW3556-975 : Fix start code detection
 * 
 * 12   8/26/09 3:10p gmohile
 * SWDEPRECATED-3878 : Add file offset to status
 * 
 * 11   8/21/09 10:19a gmohile
 * PR 55991 : Add VOB trick modes
 * 
 * 10   1/20/09 4:20p gmohile
 * PR 50725 : Seperate mpeg1 pts parsing
 * 
 * 9   1/14/09 12:31p gmohile
 * PR 50725 : Add duration detection for mpeg1 streams
 * 
 * 8   1/13/09 3:45p gmohile
 * PR 50725 : Add duration probe for PES and VOB streams
 * 
 * 7   11/4/08 11:19a vsilyaev
 * PR 48182: Demoted debug message
 * 
 * 6   7/26/07 2:02p vsilyaev
 * PR 32813: In the critical path use inline functions for cursor
 * navigation
 * 
 * 5   7/25/07 6:28p vsilyaev
 * PR 32813: Added code to handle partially matches ES streams
 * 
 * 4   7/25/07 1:50p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 3   7/24/07 7:20p vsilyaev
 * PR 32813: Added MPEG-2 PES probe
 * 
 * 2   7/23/07 11:36a vsilyaev
 * PR 32813: Added MPEG-2 TS payload probe
 * 
 * 1   7/17/07 4:26p vsilyaev
 * PR 32813: MPEG-2 TS/PES/PSI parser
 * 
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bmpeg2pes_parser.h"
#include "biobits.h"
#include "bkni.h"

#define BDBG_MSG_TRACE(x)	/* BDBG_WRN(x) */

BDBG_MODULE(bmpeg2pes_parser);

typedef enum {
	b_mpeg2pes_parser_result_match, 
	b_mpeg2pes_parser_result_nosync, 
	b_mpeg2pes_parser_result_nomatch, 
	b_mpeg2pes_parser_result_error, 
	b_mpeg2pes_parser_result_more
} b_mpeg2pes_parser_result;


bmpeg2ts_parser_action 
b_mpeg2pes_parser_stream_drop(bmpeg2pes_parser_stream *stream, unsigned flags, batom_accum_t src, batom_cursor *payload_start, size_t len)
{
	BSTD_UNUSED(payload_start);
	BSTD_UNUSED(len);
	BSTD_UNUSED(stream);
	BSTD_UNUSED(flags);
	BSTD_UNUSED(src);
	return bmpeg2ts_parser_action_skip;
}


void 
bmpeg2pes_parser_stream_init(bmpeg2pes_parser_stream *stream, uint16_t stream_id)
{
	BDBG_ASSERT(stream);
	stream->stream_id = stream_id;
	stream->flags = 0;
	stream->npackets = 0;
	stream->pts = 0;	
	stream->payload = b_mpeg2pes_parser_stream_drop;
	
	return;
}


int
bmpeg2pes_parser_stream_feed(bmpeg2pes_parser_stream *stream, batom_accum_t src, batom_cursor *cursor)
{
	batom_checkpoint check;
	int result=BMPEG2TS_RESULT_EOS;

	BDBG_ASSERT(cursor);

	/* ISO/IEC 13818-1 */
	
	while(1){
		uint32_t word;
		unsigned flags = stream->flags;
		unsigned off;
		int payload_len;       

		BATOM_SAVE(cursor, &check);

		word = batom_cursor_uint32_be(cursor);
		if( (word&0xFFFFFF00) != 0x00000100) {
            if(BATOM_IS_EOF(cursor)) {goto do_exit;}
			BDBG_MSG(("b_mpeg2pes_parser_stream_feed: %#lx invalid packet_start_code_prefix 0x%08x!= 0x000001XX", (unsigned long)stream, word));
			goto err_out_of_sync;
		}
		word &= 0xFF;

		payload_len = batom_cursor_uint16_be(cursor);

		switch(bmpeg2pes_decode_stream_id(word)) {
		case bmpeg2pes_stream_id_type_invalid:
			goto err_out_of_sync;
		case bmpeg2pes_stream_id_type_raw:	
			result = word;
			goto next_packet;			
		default:
			break;
		}		

		if(word != stream->stream_id) {
			result = word;
			goto next_packet;
		}

		word = batom_cursor_uint24_be(cursor);
		flags |= word&BMPEG2PES_DATA_ALIGMENT;							

		off = 0;
		if (B_GET_BIT(word, 15)) {
			uint32_t pts;
			uint32_t pts_word;
			bmpeg2ts_parser_action action;

			pts_word = batom_cursor_byte(cursor);

			pts = 
			 /* pts_32_30 */(B_GET_BITS(pts_word, 3, 1)<<29);
			pts_word = batom_cursor_uint32_be(cursor);

			pts |= 
			 /* pts_29_15 */(B_GET_BITS(pts_word, 31, 17)<<14) |
			 /* pts_14_1 */B_GET_BITS(pts_word, 15, 2);
			flags |= BMPEG2PES_PTS_VALID;
			off = 5;
			stream->pts = pts;

			action = stream->payload(stream, flags, src, cursor, payload_len);
			if (action==bmpeg2ts_parser_action_skip) {					
					goto next_packet;
			} else { /* bmpeg2ts_parser_action_hold */
				batom_cursor_rollback(cursor, &check);
				result = BMPEG2TS_RESULT_HOLD; 
				goto do_exit;
			}
		}
		word =  word&0xFF; /* PES_header_data_length */
		if(word<off) {
			goto err_out_of_sync;
		}

next_packet:
		batom_cursor_rollback(cursor, &check);
		BATOM_SKIP(cursor, payload_len+6)
;       
		if(BATOM_IS_EOF(cursor)) {
			return result;
		}
		stream->flags = flags;
		
	}	

do_exit:	
	return result;

err_out_of_sync:

	batom_cursor_rollback(cursor, &check);	
	result = BMPEG2TS_RESULT_SYNC_ERROR;
	goto do_exit;
}

static void 
b_mpeg2pes_drop_packet (void *packet_cnxt, batom_accum_t src, batom_cursor *payload_start, size_t len, const bmpeg2pes_atom_info *info)
{
	BSTD_UNUSED(packet_cnxt);
	BSTD_UNUSED(info);
	BSTD_UNUSED(src);
	BSTD_UNUSED(info);
	batom_cursor_skip(payload_start, len);
	return;
}

void 
bmpeg2pes_parser_reset(bmpeg2pes_parser *pes)
{
	pes->state = bmpeg2pes_parser_state_sync;
	pes->pkt_len = 0;
	pes->data_len = 0;
	pes->info.pes_id = 0;
	pes->info.flags = 0;
	pes->info.pts = 0;
	pes->info.data_offset = 0;
	pes->hold_enable = false;
	batom_accum_clear(pes->pes_header);
	return;
}

int
bmpeg2pes_parser_init(batom_factory_t factory, bmpeg2pes_parser *pes, uint8_t id)
{
	pes->pes_header = batom_accum_create(factory);
	if(!pes->pes_header) {
		return -1;
	}
	pes->id = id;
	pes->packet_cnxt = NULL;
	pes->packet = b_mpeg2pes_drop_packet;
	bmpeg2pes_parser_reset(pes);
	return 0;
}

bmpeg2pes_stream_id_type 
bmpeg2pes_decode_stream_id(unsigned stream_id)
{
	BDBG_ASSERT(stream_id<0x100);
	switch(stream_id) {
	case 0xBC: /* program_stream_map */
	case 0xBE: /* padding_stream */
	case 0xBF: /* private_stream_2 */ 
	case 0xF0: /* ECM_stream */
	case 0xF1: /* EMM_stream */
	case 0xF2: /* ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A or ISO/IEC 13818- 6_DSMCC_stream */
	case 0xF8: /* ITU-T Rec. H.222.1 type E */ 
	case 0xFF: /* program_stream_directory */
		return bmpeg2pes_stream_id_type_raw;
	default:
		if(stream_id >= 0xBD) {
			return bmpeg2pes_stream_id_type_data;
		} else {
			return bmpeg2pes_stream_id_type_invalid;
		} 
	}
}

static b_mpeg2pes_parser_result
b_mpeg2pes_parser_hdr(bmpeg2pes_parser *pes, batom_cursor *cursor)
{
	/* table 2-17 ISO/IEC 13818-1  */
	if(batom_cursor_reserve(cursor, 9)==9) {
		uint32_t word;
		unsigned flags = pes->info.flags;
		unsigned off;

		word = batom_cursor_uint32_be(cursor);
		if( (word&0xFFFFFF00) != 0x00000100) {
			BDBG_MSG(("b_mpeg2pes_parser_hdr: %#lx invalid packet_start_code_prefix 0x%08x!= 0x000001XX", (unsigned long)pes, word));
			return b_mpeg2pes_parser_result_nosync;
		}
		word &= 0xFF;
		pes->info.pes_id = word;
		if(pes->id!=BMPEG2PES_ID_ANY && pes->id!=word) {
			BDBG_WRN(("b_mpeg2pes_parser_hdr: %#lx unknown pes id %#x(%#x)", (unsigned long)pes, pes->info.pes_id, pes->id));
			return b_mpeg2pes_parser_result_nomatch;
		}
		pes->pkt_len = batom_cursor_uint16_be(cursor);
		switch(bmpeg2pes_decode_stream_id(word)) {
		case bmpeg2pes_stream_id_type_invalid:
			return b_mpeg2pes_parser_result_nosync;
		case bmpeg2pes_stream_id_type_raw:
			pes->data_len = pes->pkt_len;
			batom_cursor_skip(cursor, 6);
			return b_mpeg2pes_parser_result_match;
		default:
			break;
		}
		word = batom_cursor_uint24_be(cursor);
		flags |= word&BMPEG2PES_DATA_ALIGMENT;

		off = 0;
		if (B_GET_BIT(word, 15)) {
			uint32_t pts;
			uint32_t pts_word;
			pts_word = batom_cursor_byte(cursor);
			pts = 
			 /* pts_32_30 */(B_GET_BITS(pts_word, 3, 1)<<29);
			pts_word = batom_cursor_uint32_be(cursor);

			pts |= 
			 /* pts_29_15 */(B_GET_BITS(pts_word, 31, 17)<<14) |
			 /* pts_14_1 */B_GET_BITS(pts_word, 15, 2);
			flags |= BMPEG2PES_PTS_VALID;
			off = 5;
			pes->info.pts = pts;
		}
		else
		  flags &= ~BMPEG2PES_PTS_VALID;

		word =  word&0xFF; /* PES_header_data_length */
		if(word<off) {
			return b_mpeg2pes_parser_result_error;
		}
		batom_cursor_skip(cursor, word-off);
		if(BATOM_IS_EOF(cursor)) {
			return b_mpeg2pes_parser_result_more;
		}
		pes->info.flags = flags;
		word+=3; /* three bytes extra */
		if(pes->pkt_len>word) {
			pes->data_len = pes->pkt_len - word;
		} else {
			pes->data_len = 0;
		}
		BDBG_MSG_TRACE(("b_mpeg2pes_parser_hdr: %#lx %#x:%#x pes_len:%u", (unsigned long)pes, pes->id, pes->info.pes_id, pes->data_len));
		return b_mpeg2pes_parser_result_match;
	}
	return b_mpeg2pes_parser_result_more;
}
#define B_STR_FLAG(v,name) ((v)?#name " ":"")

bmpeg2ts_parser_action 
bmpeg2pes_parser_feed(bmpeg2pes_parser *pes, unsigned ts_flags, batom_accum_t src, batom_cursor *payload_start, size_t len)
{
	batom_cursor cursor;
	batom_accum_t pes_header;

	BDBG_ASSERT(pes);
	BDBG_ASSERT(payload_start);
	BDBG_ASSERT(src);
	BDBG_MSG_TRACE(("bmpeg2pes_parser_feed: %#lx %#x:%#x %#lx:%u:%#x %s%s%s%s",(unsigned long)pes, pes->id, pes->info.pes_id, (unsigned long)payload_start, len, ts_flags, B_STR_FLAG(ts_flags&BMPEG2TS_ERROR, ERR), B_STR_FLAG(ts_flags&BMPEG2TS_DISCONTINUITY, DISC),B_STR_FLAG(ts_flags&BMPEG2TS_MARKED_DISCONTINUITY, MDISC), B_STR_FLAG(ts_flags&BMPEG2TS_PAYLOAD_UNIT_START,START)));

	if (pes->hold_enable) {
		return bmpeg2ts_parser_action_hold;
	}
	BDBG_MSG_TRACE(("bmpeg2pes_parser_feed: %#lx %#x:%#x state:%u pes_len:%u data_offset:%u", (unsigned long)pes, pes->id, pes->info.pes_id, pes->state, pes->data_len, pes->info.data_offset));

	/* ISO/IEC 13818-1 */
	/* Table 2-18 -- PES packet */
	if(pes->state == bmpeg2pes_parser_state_data) {
		if(pes->data_len > 0) {
			if(pes->info.data_offset >= pes->data_len) {
				BDBG_MSG_TRACE(("bmpeg2pes_parser_feed: %#lx %#x:%#x end of bounded PES pes_len:%u data_offset:%u", (unsigned long)pes, pes->id, pes->info.pes_id, pes->data_len, pes->info.data_offset));
				pes->state = bmpeg2pes_parser_state_hdr;
				if(pes->info.data_offset > pes->data_len) {
					pes->state = bmpeg2pes_parser_state_sync;
					ts_flags |= BMPEG2PES_DISCONTINUITY;
				}
			}
		} else if(len >= 4) {
			uint32_t scode;
			BATOM_CLONE(&cursor, payload_start);
			scode = BATOM_UINT32_BE(&cursor);
			if( scode == (0x100u | pes->info.pes_id)) {
				BDBG_MSG(("bmpeg2pes_parser_feed: %#lx detected new pes packet %#x len (%u)", (unsigned long)pes, scode, pes->info.data_offset));
				pes->state = bmpeg2pes_parser_state_hdr;
			}
		}
	}
	if(ts_flags&BMPEG2TS_PAYLOAD_UNIT_START) {
		if(pes->state == bmpeg2pes_parser_state_data) {
			BDBG_MSG_TRACE(("bmpeg2pes_parser_feed: %#lx %#x:%#x discontinuty at pes_len:%u data_offset:%u", (unsigned long)pes, pes->id, pes->info.pes_id, pes->data_len, pes->info.data_offset));
			ts_flags |= BMPEG2PES_DISCONTINUITY;
		}
		pes->state = bmpeg2pes_parser_state_hdr;
		pes->info.data_offset = 0;
	} 
	switch(pes->state) {
	case bmpeg2pes_parser_state_data:
		pes->info.flags |= ts_flags;
		if(len>0) {
			pes->packet(pes->packet_cnxt, src , payload_start, len, &pes->info);
		}
		pes->info.flags = 0;
		pes->info.data_offset += len;
		break;
	case bmpeg2pes_parser_state_hdr:
		pes_header = pes->pes_header;
		BATOM_CLONE(&cursor, payload_start);
		batom_cursor_skip(payload_start, len);
		batom_accum_append(pes_header, src, &cursor, payload_start);
		batom_cursor_from_accum(&cursor,pes_header);
		switch(b_mpeg2pes_parser_hdr(pes, &cursor)) {
		case b_mpeg2pes_parser_result_match:
			batom_accum_trim(pes_header, &cursor);
			len = batom_accum_len(pes_header);
			if(len>0) {
				batom_cursor_from_accum(&cursor, pes_header);
				pes->packet(pes->packet_cnxt, pes_header, &cursor, len, &pes->info);
			}
			batom_accum_clear(pes_header);
			pes->info.data_offset += len;
			pes->state = bmpeg2pes_parser_state_data;
			break;
		case b_mpeg2pes_parser_result_more:
			break;
		case b_mpeg2pes_parser_result_nosync:
		case b_mpeg2pes_parser_result_nomatch:
		case b_mpeg2pes_parser_result_error:
			pes->state = bmpeg2pes_parser_state_sync;
			pes->info.flags = BMPEG2PES_DISCONTINUITY;
			batom_accum_clear(pes_header);
			break;
		}
		break;
	case bmpeg2pes_parser_state_sync:
		batom_cursor_skip(payload_start, len);
		break;
	}
		
	return bmpeg2ts_parser_action_consume;
}


void 
bmpeg2pes_parser_flush(bmpeg2pes_parser *pes)
{
	batom_accum_t pes_header;

	BDBG_ASSERT(pes);
	pes_header = pes->pes_header;
	if(batom_accum_len(pes_header)) {
		batom_accum_clear(pes_header);
		pes->info.flags |= BMPEG2PES_DISCONTINUITY;
	}
	pes->state = bmpeg2pes_parser_state_sync;
	return;
}

void 
bmpeg2pes_parser_shutdown(bmpeg2pes_parser *pes)
{
	BDBG_ASSERT(pes);
	batom_accum_destroy(pes->pes_header);
	return;
}

BDBG_OBJECT_ID(bmpeg2pes_demux_t);
struct bmpeg2pes_demux {
	BDBG_OBJECT(bmpeg2pes_demux_t)
	enum {b_mpeg2pes_demux_state_data, b_mpeg2pes_demux_state_hdr, b_mpeg2pes_demux_state_sync, b_mpeg2pes_demux_state_resync} state;
	unsigned packet_len;
	unsigned packet_off;
	batom_accum_t accum;
	bmpeg2pes_parser parser;
	bmpeg2pes_demux_status status;
};

void 
bmpeg2pes_demux_default_config(bmpeg2pes_demux_config *config)
{
	config->packet_cnxt = NULL;
	config->packet = b_mpeg2pes_drop_packet;
	return;
}

bmpeg2pes_demux_t
bmpeg2pes_demux_create(batom_factory_t factory, const bmpeg2pes_demux_config *config)
{
	bmpeg2pes_demux_t demux;
	int rc;

	BDBG_ASSERT(factory);
	BDBG_ASSERT(config);

	demux = BKNI_Malloc(sizeof(*demux));
	if(!demux) {
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(demux, bmpeg2pes_demux_t);
	demux->accum = batom_accum_create(factory);
	if(!demux->accum) {
		goto err_accum;
	}
	rc = bmpeg2pes_parser_init(factory, &demux->parser, BMPEG2PES_ID_ANY);
	if(rc!=0) {
		goto err_parser;
	}
	demux->parser.packet_cnxt= config->packet_cnxt;
	demux->parser.packet = config->packet;
	bmpeg2pes_demux_reset(demux);
	return demux;

err_parser:
	batom_accum_destroy(demux->accum);
err_accum:
	BKNI_Free(demux);
err_alloc:
	return NULL;
}

void 
bmpeg2pes_demux_reset(bmpeg2pes_demux_t demux)
{
	BDBG_OBJECT_ASSERT(demux, bmpeg2pes_demux_t);
	demux->state = b_mpeg2pes_demux_state_hdr;
	demux->packet_len = 0;
	demux->packet_off = 0;
	demux->status.nresyncs = 0;
	demux->status.data_offset=0;
	batom_accum_clear(demux->accum);
	bmpeg2pes_parser_reset(&demux->parser);
	return;
}

void 
bmpeg2pes_demux_seek(bmpeg2pes_demux_t demux, uint64_t offset)
{
	BDBG_OBJECT_ASSERT(demux, bmpeg2pes_demux_t);
	
	bmpeg2pes_demux_reset(demux);
	demux->status.data_offset = offset;
	return;
}

void 
bmpeg2pes_demux_destroy(bmpeg2pes_demux_t demux)
{
	BDBG_OBJECT_ASSERT(demux, bmpeg2pes_demux_t);
	batom_accum_destroy(demux->accum);
	bmpeg2pes_parser_shutdown(&demux->parser);
	BDBG_OBJECT_DESTROY(demux, bmpeg2pes_demux_t);
	BKNI_Free(demux);
	return;
}

static bmpeg2pes_stream_id_type
b_mpeg2pes_demux_get_scode(batom_cursor *cursor)
{
	uint32_t scode=0xFFFFFFFFul;
	int byte;

	for(;;) {
		BATOM_NEXT(byte, cursor);
		if(byte!=BATOM_EOF) {
			scode = (scode << 8)|byte;
			if((scode&0xFFFFFF00ul)==0x100) {
				BDBG_MSG_TRACE(("b_mpeg2pes_demux_get_scode: scode:%#x", scode));
				return scode&0xFF;
			}
		} else {
			return 0;
		}
	}
}

void 
bmpeg2pes_demux_get_status(bmpeg2pes_demux_t demux, bmpeg2pes_demux_status *status)
{
	BDBG_OBJECT_ASSERT(demux, bmpeg2pes_demux_t);
	BDBG_ASSERT(status);
	*status = demux->status;
	return;
}

size_t 
bmpeg2pes_demux_feed(bmpeg2pes_demux_t pes, batom_pipe_t pipe)
{
	size_t feed_len=0;
	batom_t atom;
	batom_accum_t accum;
	uint32_t word;
	BDBG_OBJECT_ASSERT(pes, bmpeg2pes_demux_t);

	atom = batom_pipe_pop(pipe);
	accum = pes->accum;
	do {
		batom_cursor cursor;
		batom_cursor start;
		size_t len;

		BDBG_MSG(("bmpeg2pes_demux_feed:%#lx atom:%#lx:%u", (unsigned long)pes, (unsigned long)atom, atom?batom_len(atom):0));
		if(atom) {
			batom_accum_add_atom(accum, atom);
			feed_len += batom_len(atom);
			batom_release(atom);
		}
		for(;;) {
			bmpeg2pes_stream_id_type id_type;
			batom_cursor_from_accum(&cursor, accum);			
			BDBG_MSG_TRACE(("bmpeg2pes_demux_feed:%#lx state:%u accum:%u packet_len:%u packet_off:%u", (unsigned long)pes, (unsigned long)pes->state, batom_accum_len(accum), pes->packet_len, pes->packet_off));
			BATOM_CLONE(&start, &cursor);
			switch(pes->state) {
			case b_mpeg2pes_demux_state_data:
				if(pes->packet_len>0) {
					BDBG_ASSERT(pes->packet_off < pes->packet_len);
					len = batom_cursor_skip(&cursor, pes->packet_len-pes->packet_off); 
					BDBG_ASSERT(pes->packet_off<=pes->packet_len);
				} else {
					if(pes->packet_off==0) {
						batom_cursor_skip(&cursor, sizeof(uint32_t)+sizeof(uint16_t));
					}
					/* search for PES start code or end of data */
					for(;;) {
						word = b_mpeg2pes_demux_get_scode(&cursor);
						len = batom_cursor_pos(&cursor);
						if(word < 0xB9) { /* GM : system start codes are above 0xB9 */
							if(BATOM_IS_EOF(&cursor))  {
								goto next;
							}
						} else {
							BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
							pes->state = b_mpeg2pes_demux_state_hdr;
							break;
						} 
					}
					BDBG_ASSERT(len>=4);
					len -= 4;
				}				
				if(len>0) {
					bmpeg2ts_parser_action  action;
					action = bmpeg2pes_parser_feed(&pes->parser, pes->packet_off==0?BMPEG2TS_PAYLOAD_UNIT_START:0, accum, &start, len);
					pes->packet_off+=len;
					if(pes->packet_off==pes->packet_len) {
						pes->state = b_mpeg2pes_demux_state_hdr;
					}
					pes->status.data_offset += len;
					batom_accum_trim(accum, &start);
					if(action==bmpeg2ts_parser_action_hold) {
						goto exit;
					}
				} else {
					goto next;
				}
				break;
			case b_mpeg2pes_demux_state_hdr:
				word = batom_cursor_uint32_be(&cursor);
				BDBG_MSG_TRACE(("bmpeg2pes_demux_feed:%#lx pes_scode:%#x", (unsigned long)pes, word));
				pes->packet_off = 0;				
				pes->packet_len = batom_cursor_uint16_be(&cursor);
				
				if(BATOM_IS_EOF(&cursor)) {
					goto next;
				}
				if( (word&0xFFFFFF00) == 0x100) {
					word &= 0xFF;
					if(bmpeg2pes_decode_stream_id(word)!=bmpeg2pes_stream_id_type_invalid) {
						pes->state = b_mpeg2pes_demux_state_data;
						if(pes->packet_len!=0) {
							pes->packet_len+=sizeof(uint32_t) /* scode */ +sizeof(uint16_t) /* packet_len */;
						}
						BDBG_MSG_TRACE(("bmpeg2pes_demux_feed:%#lx packet_len:%u", (unsigned long)pes, pes->packet_len));
					} else /* if(word >= 0xB0) */ {
						pes->state = b_mpeg2pes_demux_state_sync;
					}
				} else if((word>>24)==0) {
					pes->state = b_mpeg2pes_demux_state_sync;
				} else {
					pes->state = b_mpeg2pes_demux_state_resync;
				}
				break;
			case b_mpeg2pes_demux_state_resync:
				pes->status.nresyncs ++;
				BDBG_MSG(("bmpeg2pes_demux_feed:%#lx resync:%u", (unsigned long)pes, pes->status.nresyncs));
			case b_mpeg2pes_demux_state_sync:
				for(;;) {
					id_type = bmpeg2pes_decode_stream_id(b_mpeg2pes_demux_get_scode(&cursor));
					len = batom_cursor_pos(&cursor);
					if(id_type!=bmpeg2pes_stream_id_type_invalid) {
						BDBG_MSG_TRACE(("bmpeg2pes_demux_feed:%#lx sync_off:%u", (unsigned long)pes, len));
						pes->state = b_mpeg2pes_demux_state_hdr;
						break;
					}
					if(BATOM_IS_EOF(&cursor))  {
						len++;
						if(len>4) { /* we need to read at least 4 bytes */
							break;
						} else {
							goto next;
						}
					}
				}
				BDBG_ASSERT(len>=4);
				len -= 4;
				batom_cursor_skip(&start, len);
				pes->status.data_offset += len;
				batom_accum_trim(accum, &start);
				break;
			}
		}
next:
		;
	} while(NULL!=(atom=batom_pipe_pop(pipe)));
exit:
	return feed_len;
}

