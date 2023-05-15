/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg2ts_parser.c $
 * $brcm_Revision: 27 $
 * $brcm_Date: 2/7/12 12:50p $
 *
 * Module Description:
 *
 * MPEG-2 TS Parser/Demux library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg2ts_parser.c $
 * 
 * 27   2/7/12 12:50p vsilyaev
 * SW7425-2324: Demoted private function
 * 
 * 26   8/3/11 3:06p vsilyaev
 * SWDTV-8098: Improved debug output
 * 
 * 25   3/11/11 4:48p vsilyaev
 * SW7231-55: Short-circuit trivial resync
 * 
 * 24   3/1/11 5:53p vsilyaev
 * SW7405-4718: Limit amount of debug output for out-of-sync error
 * 
 * 23   11/24/10 4:55p vsilyaev
 * SW3548-3115: Resolved handling of out-of-sync errors when extracting
 * PCR information
 * 
 * 22   11/12/10 2:22p vsilyaev
 * SW7342-265: Improve recovery from a lost sync
 * 
 * 21   11/2/10 12:02p vsilyaev
 * SW7342-265: Limit amount of debug output on corrupted streams
 * 
 * 20   4/23/10 2:11p vsilyaev
 * SWBLURAY-19982: Improved resyncing in the MPEG-2 TS stream
 * 
 * fix_bdvd_v3.0_issue19982/1   4/23/10 4:42p ialauder
 * Improve code to find TS sync bytes
 * 
 * 19   6/19/09 3:45p vsilyaev
 * PR 55989: Fixed typo in parsing PCR extension
 * 
 * 18   6/5/09 1:31p vsilyaev
 * PR 44493: Fixed resync code, so it wouldn't corrupt cursor's state
 * 
 * 17   1/29/09 7:50p vsilyaev
 * PR 51089: Don't parse packets with TEI bit set
 * 
 * 16   1/29/09 10:48a vsilyaev
 * PR 51089: Demote debug message for transport_error indicator
 * 
 * 15   1/14/09 1:47p jtna
 * PR51089: discard rest of packet if transport_error_indicator=1
 * 
 * 14   11/25/08 4:43p vsilyaev
 * PR 48701: Relaxed handling of duplicated pids
 * 
 * 13   11/5/08 4:16p vsilyaev
 * PR 48701: Added test for duplicate PCR pids
 * 
 * 12   11/4/08 11:21a vsilyaev
 * PR 48182: Pass to the consumer PCR even if there is no payload
 * 
 * 11   7/21/08 1:18p vsilyaev
 * PR44493: Add support for 192 byte TS packets
 * 
 * PR44493/2   7/18/08 10:25p jtna
 * PR44493: Add support for 192 byte TS packets
 * 
 * PR44493/1   7/18/08 4:52p jtna
 * PR44493: Add support for 192 byte TS packets
 * 
 * 10   8/17/07 3:54p vsilyaev
 * PR 33812: Improved handling of resyncs in parsing
 * 
 * 9   7/26/07 2:48p vsilyaev
 * PR 32813: One more function inlined
 * 
 * 8   7/26/07 2:02p vsilyaev
 * PR 32813: In the critical path use inline functions for cursor
 * navigation
 * 
 * 7   7/24/07 11:27p vsilyaev
 * PR 32813: Added PCR parsing and bitrate estimation
 * 
 * 6   7/23/07 5:58p vsilyaev
 * PR 32813: Fixed memory leak
 * 
 * 5   7/23/07 11:36a vsilyaev
 * PR 32813: Added MPEG-2 TS payload probe
 * 
 * 4   7/19/07 12:09a vsilyaev
 * PR 32813: Added function to return status
 * 
 * 3   7/18/07 3:06p vsilyaev
 * PR 32813: Use bhash.h file for the pid container
 * 
 * 2   7/18/07 10:48a vsilyaev
 * PR 32813: Fixed handling of NULL pid
 * 
 * 1   7/17/07 4:17p vsilyaev
 * PR 32813: MPEG-2 TS/PES/PSI parser
 * 
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bmpeg2ts_parser.h"
#include "biobits.h"
#include "bkni.h"
#include "blst_slist.h"
BDBG_MODULE(bmpeg2ts_parser);

#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */

#define BMPEG2TS_NULL_PID	(0x1FFF)

static bmpeg2ts_parser_action 
b_mpeg2ts_parser_pid_drop(bmpeg2ts_parser_pid *pid, unsigned flags, batom_accum_t src, batom_cursor *payload_start, size_t len)
{
	BSTD_UNUSED(payload_start);
	BSTD_UNUSED(len);
	BSTD_UNUSED(pid);
	BSTD_UNUSED(flags);
	BSTD_UNUSED(src);
	return bmpeg2ts_parser_action_skip;
}

void 
bmpeg2ts_parser_pid_init(bmpeg2ts_parser_pid *parser_pid, uint16_t pid)
{
	BDBG_ASSERT(parser_pid);
	parser_pid->pid = pid;
	parser_pid->continuity_counter = 0;
	parser_pid->flags = 0;
	parser_pid->npackets = 0;
	parser_pid->pcr_base = 0;
	parser_pid->pcr_ext = 0;
	parser_pid->packet_size = BMPEG2TS_PKT_LEN;
	parser_pid->header_size = 0;
	parser_pid->payload = b_mpeg2ts_parser_pid_drop;
    parser_pid->error_count = 0;

	/* verify B_GET_BITS */
	BDBG_CASSERT(B_GET_BITS(0xDE,7,4)==0xD); 
	BDBG_CASSERT(B_GET_BITS(0xDE,3,0)==0xE); 
	BDBG_CASSERT(B_GET_BITS(0xDE,3,3)==0x1); 
	BDBG_CASSERT(B_GET_BITS(0xDE,1,1)==0x1); 
	BDBG_CASSERT(B_GET_BITS(0xDE,0,0)==0x0); 
	BDBG_CASSERT(B_GET_BITS(0xDE,6,0)==0x5E); 
	BDBG_CASSERT(!B_GET_BIT(0xDE,0)); 
	BDBG_CASSERT(B_GET_BIT(0xDE,1)); 
	BDBG_CASSERT(B_GET_BIT(0xDE,7)); 
	BDBG_CASSERT(!B_GET_BIT(0xDE,5)); 

	BDBG_CASSERT(B_SET_BIT( _ , 1, 0) == 1);
	BDBG_CASSERT(B_SET_BIT( _ , 2, 0) == 1);
	BDBG_CASSERT(B_SET_BIT( _ , 1, 1) == 2);
	BDBG_CASSERT(B_SET_BIT( _ , 1, 31) == (1u<<31));
	BDBG_CASSERT(B_SET_BITS( _ , 1, 0, 0) == 1);
	BDBG_CASSERT(B_SET_BITS( _ , 1, 1, 0) == 1);
	BDBG_CASSERT(B_SET_BITS( _ , 1, 1, 1) == 2);
	BDBG_CASSERT(B_SET_BITS( _ , 0x55, 7, 1) == 0xAA);
	return;
}

#define B_NEXT_CONT_COUNTER(c) (((c)+1)&0x0F)

int
bmpeg2ts_parser_pid_feed(bmpeg2ts_parser_pid *ts, batom_accum_t src, batom_cursor *cursor)
{
	batom_checkpoint check;
	unsigned ts_continuity_counter;
	int result=BMPEG2TS_RESULT_EOS;
	unsigned flags;

	BDBG_ASSERT(cursor);

	/* ISO/IEC 13818-1 */

	ts_continuity_counter = ts->continuity_counter;
	flags = ts->flags;
	while(BATOM_RESERVE(cursor, ts->packet_size) >= ts->packet_size) {

		uint32_t word;
		unsigned pid;
		unsigned adaptation_field_control;
		unsigned continuity_counter;
		int payload_len; /* offset into the transport packet */

		BATOM_SAVE(cursor, &check);
		batom_cursor_skip(cursor, ts->header_size);
		word = BATOM_UINT32_BE(cursor);
	
		if (B_GET_BITS(word, 31, 24)!=BMPEG2TS_PKT_SYNC) {
			goto err_out_of_sync;
		}
		/* Table 2-3 -- ITU-T Rec. H.222.0 | ISO/IEC 13818 transport packet */
		pid = B_GET_BITS(word, 20, 8);
		if (B_GET_BIT(word, 23) /* transport_error_indicator */) { /* don't check pid value if TEI bit is set */
			goto ts_error;
		}
		if (pid!=ts->pid) {
			goto other_pid;
		}
		flags |= word & BMPEG2TS_PAYLOAD_UNIT_START; /* payload_unit_start_indicator */
		continuity_counter = B_GET_BITS(word, 3, 0);
		adaptation_field_control = B_GET_BITS(word, 5, 4);
		if (continuity_counter != ts_continuity_counter && ts->npackets>0) {
			goto cc_error;
		}
after_cc:
		payload_len = BMPEG2TS_PKT_LEN - 4 ;
		if (adaptation_field_control==0x01) {
after_adaptation_payload: 
			{
				bmpeg2ts_parser_action action;
				action = ts->payload(ts, flags, src, cursor, payload_len);
				flags = 0; /* clear flags */
				if (action==bmpeg2ts_parser_action_consume) {
					goto next_cc;
				} else if (action==bmpeg2ts_parser_action_skip) {
					BATOM_SKIP(cursor, payload_len);
					goto next_cc;
				} else { /* bmpeg2ts_parser_action_hold */
					batom_cursor_rollback(cursor, &check);
					result = BMPEG2TS_RESULT_HOLD; 
					break;
				}
			}
		} else {
			goto adaptation_payload;
		}
next_cc:
		ts_continuity_counter = B_NEXT_CONT_COUNTER(continuity_counter);
done:
		ts->npackets++;
		ts->flags = flags; /* save flags */
		;
		continue;
other_pid:
		BDBG_MSG_TRACE(("bmpeg2ts_parser_pid: %#lx other pid %#x(%#x) pos:%u", (unsigned long)ts, (unsigned)pid, (unsigned)ts->pid, batom_cursor_pos(cursor)));
		result = pid;
		BATOM_ROLLBACK(cursor, &check);
		break;
ts_error:
		BDBG_MSG(("bmpeg2ts_parser_pid_feed: %#lx transport_error_indicator", (unsigned long)ts));
		batom_cursor_skip(cursor, BMPEG2TS_PKT_LEN-4);
		if (pid==ts->pid) { /* only mark error that belongs to the parsed pid */
	        flags |= BMPEG2TS_ERROR;
        }
        goto done;
cc_error:
		if (!(adaptation_field_control==0x00 || adaptation_field_control==0x02)) {
			if(B_NEXT_CONT_COUNTER(continuity_counter)==ts_continuity_counter) {
				BDBG_MSG_TRACE(("bmpeg2ts_parser_pid_feed: %#lx continuity counter duplicate %#x(%#x) pid:%#x ap:%u pos:%u", (unsigned long)ts, (unsigned)continuity_counter, (unsigned)ts->continuity_counter, ts->pid, adaptation_field_control, batom_cursor_pos(cursor)));
			} else {
				BDBG_MSG(("bmpeg2ts_parser_pid_feed: %#lx continuity counter error %#x(%#x)", (unsigned long)ts, (unsigned)continuity_counter, (unsigned)ts->continuity_counter));
			}
			flags |= BMPEG2TS_DISCONTINUITY;
		}
		goto after_cc;
adaptation_payload:
		if(adaptation_field_control!=0x00) { /* 00 reserved for future use */
			unsigned adaptation_field_length = batom_cursor_byte(cursor);
			payload_len = BMPEG2TS_PKT_LEN - 5 - adaptation_field_length ;
			if (adaptation_field_length>0) {
				word = batom_cursor_byte(cursor);
				flags |= word & ( /* adding bit fields from the adaptation field  */
						BMPEG2TS_MARKED_DISCONTINUITY | 
						BMPEG2TS_RANDOM_ACCESS_INDICATOR  | 
						BMPEG2TS_ELEMENTARY_STREAM_PRIORITY |
						BMPEG2TS_PCR_FLAG |
						BMPEG2TS_SPLICING_POINT_FLAG);
				if(word&BMPEG2TS_PCR_FLAG && adaptation_field_length>6) {
					adaptation_field_length -= 6;
					ts->pcr_base = batom_cursor_uint32_be(cursor);
					word = batom_cursor_uint16_be(cursor);
					ts->pcr_ext = B_GET_BITS(word, 8, 0);
				}
				batom_cursor_skip(cursor, adaptation_field_length-1);
			}
			if (payload_len>=0) {
				if(adaptation_field_control==3) {
					goto after_adaptation_payload;
				} 
				batom_cursor_skip(cursor, payload_len);
                if((flags&BMPEG2TS_PCR_FLAG)!=0) {
                    payload_len = 0;
					goto after_adaptation_payload;
                }
			} else {
				BDBG_WRN(("bmpeg2ts_parser_pid_feed: %#lx invalid adaptation_field_length=%u", (unsigned long)ts, adaptation_field_length));
				batom_cursor_rollback(cursor, &check);
				batom_cursor_skip(cursor, BMPEG2TS_PKT_LEN);
				if(adaptation_field_control==3) {
					goto next_cc;
				} 
			}
		} else {
			batom_cursor_skip(cursor, BMPEG2TS_PKT_LEN-4);
		}
		goto done;
	}
do_exit:
	ts->continuity_counter = ts_continuity_counter;
	return result;
err_out_of_sync:

	batom_cursor_rollback(cursor, &check);
    if(ts->error_count < BMPEG2TS_MAX_ERROR_COUNT) {
        ts->error_count++;
    } 
    if(ts->error_count<=BMPEG2TS_LIMIT_ERROR_COUNT || ts->error_count==(BMPEG2TS_MAX_ERROR_COUNT-1) ) {
        char  buf[48];
        BKNI_Snprintf(buf, sizeof(buf), /*bmpeg2ts_parser_pid_feed:*/"%#lx:out_of_sync:%#x:%u %u/%u", (unsigned long)ts, ts->pid, ts->npackets, ts->error_count, BMPEG2TS_LIMIT_ERROR_COUNT);
        batom_cursor_dump(cursor, buf);
    }
	result = BMPEG2TS_RESULT_SYNC_ERROR;
	goto do_exit;
}




typedef BHASH_HEAD(b_mpeg2ts_pid_hash, bmpeg2ts_parser_pid, 16) b_mpeg2ts_pid_hash;

#define B_MPEG2TS_PID_KEY2HASH(pid) (pid)
#define B_MPEG2TS_PID_KEY_COMPARE(ts_pid,parser_pid) ((ts_pid)==(parser_pid)->pid)
BHASH_GENERATE(b_mpeg2ts_pid_hash, uint16_t, bmpeg2ts_parser_pid, hash_link, B_MPEG2TS_PID_KEY2HASH, B_MPEG2TS_PID_KEY_COMPARE)

BDBG_OBJECT_ID(bmpeg2ts_parser_t);

struct bmpeg2ts_parser {
	BDBG_OBJECT(bmpeg2ts_parser_t)
	b_mpeg2ts_pid_hash pids;
	bool resync;
	batom_accum_t accum;
	unsigned npackets;
	unsigned last_pid_npackets;
	bmpeg2ts_parser_pid *last_pid;
	bmpeg2ts_parser_pid null_pid;
	bmpeg2ts_parser_cfg cfg;
	unsigned resync_bytes;
	unsigned nresyncs;
	uint64_t seek_offset;
};

static bmpeg2ts_parser_action 
b_mpeg2ts_parser_no_action(void *application_cnxt, uint16_t pid)
{
	BSTD_UNUSED(application_cnxt);
	BSTD_UNUSED(pid);
	return bmpeg2ts_parser_action_skip;
}


void 
bmpeg2ts_parser_default_cfg(bmpeg2ts_parser_cfg *cfg)
{
	cfg->application_cnxt = NULL;
	cfg->unknown_pid = b_mpeg2ts_parser_no_action;
	cfg->header_size = 0;
	return;
}


void 
bmpeg2ts_parser_seek(bmpeg2ts_parser_t ts, uint64_t offset)
{
	bmpeg2ts_parser_pid *pid;
	BDBG_OBJECT_ASSERT(ts, bmpeg2ts_parser_t);
	ts->seek_offset = offset;
	ts->npackets = 0;
	ts->nresyncs = 0;
	ts->resync_bytes = 0;
	ts->resync = true;
	batom_accum_clear(ts->accum);
	BHASH_FOREACH_BEGIN(b_mpeg2ts_pid_hash,&ts->pids,bmpeg2ts_parser_pid,pid,hash_link) 
		pid->flags = 0;
		pid->continuity_counter = 0;
		pid->npackets = 0;
	BHASH_FOREACH_END()
	return;
}

void 
bmpeg2ts_parser_flush(bmpeg2ts_parser_t ts)
{
	BDBG_OBJECT_ASSERT(ts, bmpeg2ts_parser_t);
	bmpeg2ts_parser_seek(ts, ts->seek_offset);
	return;
}

void 
bmpeg2ts_parser_reset(bmpeg2ts_parser_t ts)
{
	BDBG_OBJECT_ASSERT(ts, bmpeg2ts_parser_t);
	ts->last_pid = &ts->null_pid;
	ts->last_pid_npackets = 0;
	bmpeg2ts_parser_pid_init(&ts->null_pid, BMPEG2TS_NULL_PID);
	ts->null_pid.header_size = ts->cfg.header_size;
	BHASH_INIT(b_mpeg2ts_pid_hash,&ts->pids);
	bmpeg2ts_parser_seek(ts, 0);
	return;
}

bmpeg2ts_parser_t 
bmpeg2ts_parser_create(batom_factory_t factory, const bmpeg2ts_parser_cfg *cfg)
{
	bmpeg2ts_parser_t  ts;
	ts = BKNI_Malloc(sizeof(*ts));
	if(!ts) {
		BDBG_ERR(("bmpeg2ts_parser_create: can't allocate %u bytes", sizeof(*ts)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(ts, bmpeg2ts_parser_t);
	ts->cfg = *cfg;
	ts->accum = batom_accum_create(factory);
	if(!ts->accum) {
		goto err_accum;
	}
	bmpeg2ts_parser_reset(ts);
	return ts;
err_accum:
	BKNI_Free(ts);
err_alloc:
	return NULL;
}

void 
bmpeg2ts_parser_destroy(bmpeg2ts_parser_t ts)
{
	BDBG_OBJECT_ASSERT(ts, bmpeg2ts_parser_t);
	batom_accum_destroy(ts->accum);
	BDBG_OBJECT_DESTROY(ts, bmpeg2ts_parser_t);
	BKNI_Free(ts);
	return;
}

void 
bmpeg2ts_parser_add_pid(bmpeg2ts_parser_t ts, bmpeg2ts_parser_pid *pid)
{
	BDBG_OBJECT_ASSERT(ts, bmpeg2ts_parser_t);
    BDBG_MSG_TRACE(("bmpeg2ts_parser_add_pid: %#lx %#lx(%u)", (unsigned long)ts, (unsigned long)pid, (unsigned)pid->pid));
	BDBG_ASSERT(pid->pid<=BMPEG2TS_NULL_PID);
	if(BHASH_FIND(b_mpeg2ts_pid_hash, &ts->pids, pid->pid)==NULL) {
        pid->header_size = ts->cfg.header_size;
        pid->packet_size = BMPEG2TS_PKT_LEN+ts->cfg.header_size;
        BHASH_ADD(b_mpeg2ts_pid_hash,&ts->pids, pid->pid, pid);
    } else {
        BDBG_WRN(("bmpeg2ts_parser_add_pid: %#lx duplicated pid %#x(%#lx)", (unsigned long)ts, (unsigned)pid->pid, (unsigned long)(BHASH_FIND(b_mpeg2ts_pid_hash, &ts->pids, pid->pid))));
    }
	return ;
}

void 
bmpeg2ts_parser_remove_pid(bmpeg2ts_parser_t ts, uint16_t pid)
{
	BDBG_OBJECT_ASSERT(ts, bmpeg2ts_parser_t);
	BDBG_ASSERT(pid<=BMPEG2TS_NULL_PID);
	BDBG_ASSERT(BHASH_FIND(b_mpeg2ts_pid_hash, &ts->pids, pid)!=NULL);
	ts->last_pid = &ts->null_pid; /* clear last_pid so it wouldn't point to the deleted pid */
	if(!BHASH_REMOVE(b_mpeg2ts_pid_hash, &ts->pids, pid)) {
		BDBG_WRN(("bmpeg2ts_parser_remove_pid: %#lx removing unknown pid %u", (unsigned long)ts, pid));
	}
	return;
}

static void
b_mpeg2ts_parser_resync(bmpeg2ts_parser_t ts, batom_cursor *cursor, const bmpeg2ts_parser_pid *pid_parser)
{
    batom_cursor data;

    BATOM_CLONE(&data, cursor);
    /* check if we are at start of TS packet */
    if(batom_cursor_skip(&data, ts->cfg.header_size) == ts->cfg.header_size && batom_cursor_next(&data)==BMPEG2TS_PKT_SYNC) {  
        ts->resync = false;
    } else {
        unsigned skip_bytes = 0;
        if(bmpeg2ts_parser_sync_header(cursor, ts->cfg.header_size, &skip_bytes)) {
            ts->resync = false;
        }
        if(skip_bytes) {
            ts->nresyncs++;
            ts->resync_bytes+=skip_bytes;
            if(!ts->resync && pid_parser->error_count<2*BMPEG2TS_LIMIT_ERROR_COUNT) {
                BDBG_WRN(("bmpeg2ts_parser_feed: %#lx reacquired sync at %#x %u:%u at %u", (unsigned long)ts, pid_parser->pid, pid_parser->error_count, 2*BMPEG2TS_MAX_ERROR_COUNT, (ts->npackets*(BMPEG2TS_PKT_LEN+ts->cfg.header_size)) + ts->resync_bytes + ts->seek_offset));
            }
        }
    }
	return;
}

size_t 
bmpeg2ts_parser_feed(bmpeg2ts_parser_t ts, batom_pipe_t pipe)
{
	size_t len=0;
	batom_t atom;
	batom_accum_t accum;
	bmpeg2ts_parser_pid *pid_parser;
	BDBG_OBJECT_ASSERT(ts, bmpeg2ts_parser_t);

	atom = batom_pipe_pop(pipe);
	accum = ts->accum;
	pid_parser = ts->last_pid;
	do {
		batom_cursor cursor;

		BDBG_MSG(("bmpeg2ts_parser_feed:%#lx atom:%#lx", (unsigned long)ts, (unsigned long)atom));
		if(atom) {
			batom_accum_add_atom(accum, atom);
			len += batom_len(atom);
			batom_release(atom);
		}
		batom_cursor_from_accum(&cursor, accum);
		if(ts->resync) {
		    b_mpeg2ts_parser_resync(ts, &cursor, pid_parser);
		}
		for(;;) {
			int pid_result;
			unsigned npackets = pid_parser->npackets;
			bmpeg2ts_parser_action  action;
			ts->last_pid_npackets = npackets;
			ts->last_pid = pid_parser;
			pid_result = bmpeg2ts_parser_pid_feed(pid_parser, accum, &cursor);
			ts->last_pid_npackets = pid_parser->npackets;
			ts->npackets += pid_parser->npackets - npackets;
			if(pid_result>=0) { /* got unknown pid */
				bmpeg2ts_parser_pid *pid;
				BDBG_ASSERT(pid_result<=BMPEG2TS_NULL_PID);
				pid = BHASH_FIND(b_mpeg2ts_pid_hash, &ts->pids, pid_result);
				if(pid) { /* known pid */
					pid_parser = pid;
					ts->last_pid = pid_parser;
					continue;
				} 
				if(pid_result==BMPEG2TS_NULL_PID) {
					pid_parser = &ts->null_pid;
					continue;
				}
				action = ts->cfg.unknown_pid(ts->cfg.application_cnxt, pid_result);
				if(action==bmpeg2ts_parser_action_skip) {
					BATOM_SKIP(&cursor, (int)(BMPEG2TS_PKT_LEN+ts->cfg.header_size));
					ts->npackets++;
					continue;
				} else if(action==bmpeg2ts_parser_action_consume) {
					pid = BHASH_FIND(b_mpeg2ts_pid_hash, &ts->pids, pid_result);
					if(pid) { /* known pid */
						pid_parser = pid;
						continue;
					} else {
						BDBG_ERR(("bmpeg2ts_parser_feed: %#lx action bmpeg2ts_parser_action_consume shall result in new PID registered"));
						BDBG_ASSERT(0);
						/* allow execution for release version */
						batom_cursor_skip(&cursor, BMPEG2TS_PKT_LEN+ts->cfg.header_size);
						continue;
					} 
				} else  /* if(action==bmpeg2ts_parser_action_hold) */ {
					goto trim_and_exit;
				}
			} else if(pid_result==BMPEG2TS_RESULT_EOS) {
				break;
			} else if(pid_result==BMPEG2TS_RESULT_HOLD) {
				goto trim_and_exit;
			} else  if(pid_result==BMPEG2TS_RESULT_SYNC_ERROR) {
                if(pid_parser->error_count<2*BMPEG2TS_LIMIT_ERROR_COUNT) {
				    BDBG_WRN(("bmpeg2ts_parser_feed: %#lx out of sync %#x %u:%u at %u", (unsigned long)ts, pid_parser->pid, pid_parser->error_count, 2*BMPEG2TS_MAX_ERROR_COUNT, (ts->npackets*(BMPEG2TS_PKT_LEN+ts->cfg.header_size)) + ts->resync_bytes + ts->seek_offset));
                }
				b_mpeg2ts_parser_resync(ts, &cursor, pid_parser);
			}
		}
		batom_accum_trim(accum, &cursor);
		continue;
trim_and_exit:
		batom_accum_trim(accum, &cursor);
		break;
	} while(NULL!=(atom=batom_pipe_pop(pipe)));
	return len;
}

void 
bmpeg2ts_parser_get_status(bmpeg2ts_parser_t ts, bmpeg2ts_parser_status *status)
{
	unsigned npackets;
	BDBG_OBJECT_ASSERT(ts, bmpeg2ts_parser_t);
	status->npackets = ts->npackets;
	status->nresyncs = ts->nresyncs;
	status->resync_bytes = ts->resync_bytes;
	npackets = ts->npackets + (ts->last_pid->npackets - ts->last_pid_npackets);
	status->offset =  ts->seek_offset + (npackets * ((uint64_t)BMPEG2TS_PKT_LEN+ts->cfg.header_size)) + ts->resync_bytes;
	return;
}

bool
bmpeg2ts_parser_sync_header(batom_cursor *cursor, unsigned header_size, unsigned *offset)
{
    unsigned skip_bytes;
    unsigned pkt = 0;   
    int byte;    
    bool pkt_found = false;
    batom_cursor sync;

    for (skip_bytes = 0; ; skip_bytes++) {
        pkt = 0;
        BATOM_CLONE(&sync, cursor);
        batom_cursor_skip(&sync, skip_bytes + header_size); /* skip over header and non TS sync */
        
        byte = batom_cursor_next(&sync);
        if(byte==BATOM_EOF) {
            break;
        } else if(byte != BMPEG2TS_PKT_SYNC) {  
            for(;;) { /* fast loop to find first 0x47 */
                byte = batom_cursor_next(&sync);
                if(byte==BATOM_EOF) {
                    goto done;
                } else if(byte==BMPEG2TS_PKT_SYNC) {
                    break;
                }
                skip_bytes++;
            }
            continue; /* back to beginning to skip header, etc */
        } 
        BDBG_MSG(("[%u]try sync at %u", BMPEG2TS_PKT_LEN+header_size, skip_bytes));
        BATOM_CLONE(&sync, cursor);
        batom_cursor_skip(&sync, skip_bytes); 
        for (pkt_found = true, pkt = 0; pkt < BMPEG2TS_PROBE_PACKETS - 1 && pkt_found; pkt++)
        {
            batom_cursor_skip(&sync, header_size); /* skip over header */
            byte = batom_cursor_next(&sync);
            if(byte==BMPEG2TS_PKT_SYNC) {
                BDBG_CASSERT(BMPEG2TS_PKT_LEN>1);
                batom_cursor_skip(&sync, BMPEG2TS_PKT_LEN-1);
                continue;
            } else if(byte==BATOM_EOF) {
                goto done;
            } else {
                pkt_found = false;
            }
        }
        if (pkt == BMPEG2TS_PROBE_PACKETS - 1)
        {
            goto done;
        }
    } 
done:
    BDBG_MSG(("found sync at %u(%u)", skip_bytes, pkt));
    batom_cursor_skip(cursor, skip_bytes); /* skip cursor to the first TS PKT */
    *offset = skip_bytes;
    return pkt > 0;
}

