/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg2ts_parser.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 11/24/10 4:55p $
 *
 * Module Description:
 *
 * MPEG-2 TS Parser/Demux library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg2ts_parser.h $
 * 
 * 9   11/24/10 4:55p vsilyaev
 * SW3548-3115: Resolved handling of out-of-sync errors when extracting
 * PCR information
 * 
 * 8   11/2/10 12:02p vsilyaev
 * SW7342-265: Limit amount of debug output on corrupted streams
 * 
 * 7   4/23/10 2:11p vsilyaev
 * SWBLURAY-19982: Improved resyncing in the MPEG-2 TS stream
 * 
 * fix_bdvd_v3.0_issue19982/1   4/23/10 4:42p ialauder
 * Improve code to find TS sync bytes
 * 
 * 6   7/21/08 1:18p vsilyaev
 * PR44493: Add support for 192 byte TS packets
 * 
 * PR44493/2   7/18/08 10:25p jtna
 * PR44493: Add support for 192 byte TS packets
 * 
 * PR44493/1   7/18/08 4:52p jtna
 * PR44493: Add support for 192 byte TS packets
 * 
 * 5   8/17/07 3:55p vsilyaev
 * PR 33812: Improved handling of resyncs in parsing
 * 
 * 4   7/24/07 11:27p vsilyaev
 * PR 32813: Added PCR parsing and bitrate estimation
 * 
 * 3   7/19/07 12:10a vsilyaev
 * PR 32813: Added function to return status
 * 
 * 2   7/18/07 3:06p vsilyaev
 * PR 32813: Use bhash.h file for the pid container
 * 
 * 1   7/17/07 4:17p vsilyaev
 * PR 32813: MPEG-2 TS/PES/PSI parser
 * 
 * 
 *******************************************************************************/
#ifndef __BMPEG2TS_PARSER_H__
#define __BMPEG2TS_PARSER_H__


#include "bioatom.h"
#include "bhash.h"
#ifdef __cplusplus
extern "C"
{
#endif


typedef enum {bmpeg2ts_parser_action_hold, bmpeg2ts_parser_action_consume, bmpeg2ts_parser_action_skip} bmpeg2ts_parser_action;
typedef struct bmpeg2ts_parser_pid bmpeg2ts_parser_pid;

#define BMPEG2TS_PKT_LEN	    (188)
#define BMPEG2TS_PKT_SYNC	    (0x47)
#define BMPEG2TS_PROBE_PACKETS	8

#define BMPEG2TS_LIMIT_ERROR_COUNT    4
#define BMPEG2TS_MAX_ERROR_COUNT    255
struct bmpeg2ts_parser_pid {
	BHASH_ENTRY(bmpeg2ts_parser_pid) hash_link;
	uint16_t pid;
	uint8_t  continuity_counter;
    uint8_t  error_count;
	uint32_t flags;
	unsigned npackets;
	uint32_t pcr_base;
	uint16_t pcr_ext;
	uint16_t packet_size;
	uint16_t header_size;
	bmpeg2ts_parser_action (*payload)(bmpeg2ts_parser_pid *pid, unsigned flags, batom_accum_t src, batom_cursor *payload_start, size_t len);
};

/* following bit fields must match bits from the MPEG-2 TS header ISO/IEC 13818-1 , Table 2-6 . Transport Stream adaptation field */
#define BMPEG2TS_MARKED_DISCONTINUITY (1<<7)
#define BMPEG2TS_RANDOM_ACCESS_INDICATOR (1<<6)
#define BMPEG2TS_ELEMENTARY_STREAM_PRIORITY (1<<5)
#define BMPEG2TS_PCR_FLAG	(1<<4)
#define BMPEG2TS_SPLICING_POINT_FLAG	(1<<2)

/* this bit shall match location in 32 bit field of payload_unit_start_indicator */
#define BMPEG2TS_PAYLOAD_UNIT_START		(1<<22)

/* these are artificial flags */
#define BMPEG2TS_DISCONTINUITY 		(1<<1)
#define BMPEG2TS_ERROR 				(1<<0)

#define BMPEG2TS_RESULT_EOS			(-1)
#define BMPEG2TS_RESULT_HOLD		(-2)
#define BMPEG2TS_RESULT_SYNC_ERROR 	(-3)

void bmpeg2ts_parser_pid_init(bmpeg2ts_parser_pid *parser_pid, uint16_t pid);
int bmpeg2ts_parser_pid_feed(bmpeg2ts_parser_pid *ts, batom_accum_t src, batom_cursor *cursor);

typedef struct bmpeg2ts_parser  *bmpeg2ts_parser_t;
typedef struct bmpeg2ts_parser_cfg {
	void *application_cnxt;
	bmpeg2ts_parser_action (*unknown_pid)(void *application_cnxt, uint16_t pid);
	unsigned header_size;
} bmpeg2ts_parser_cfg;

typedef struct bmpeg2ts_parser_status {
	unsigned npackets;
	unsigned nresyncs;
	unsigned resync_bytes;
	uint64_t offset; /* current offset in the stream */
} bmpeg2ts_parser_status;

void bmpeg2ts_parser_default_cfg(bmpeg2ts_parser_cfg *cfg);
bmpeg2ts_parser_t bmpeg2ts_parser_create(batom_factory_t factory, const bmpeg2ts_parser_cfg *cfg);
void bmpeg2ts_parser_destroy(bmpeg2ts_parser_t ts);
void bmpeg2ts_parser_reset(bmpeg2ts_parser_t ts);
void bmpeg2ts_parser_flush(bmpeg2ts_parser_t ts);
size_t bmpeg2ts_parser_feed(bmpeg2ts_parser_t ts, batom_pipe_t pipe);
void bmpeg2ts_parser_add_pid(bmpeg2ts_parser_t ts, bmpeg2ts_parser_pid *pid);
void bmpeg2ts_parser_remove_pid(bmpeg2ts_parser_t ts, uint16_t pid);
void bmpeg2ts_parser_get_status(bmpeg2ts_parser_t ts, bmpeg2ts_parser_status *status);
void bmpeg2ts_parser_seek(bmpeg2ts_parser_t ts, uint64_t offset);
bool bmpeg2ts_parser_sync_header(batom_cursor *cursor, unsigned header_size, unsigned * offset);
#ifdef __cplusplus
}
#endif


#endif  /* __BMPEG2TS_PARSER_H__ */

