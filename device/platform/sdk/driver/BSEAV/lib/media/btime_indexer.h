/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btime_indexer.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 7/9/10 6:30p $
 *
 * Module Description:
 *
 * media navigational indexer
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/btime_indexer.h $
 * 
 * 7   7/9/10 6:30p vsilyaev
 * SW7405-4565: when advancing into the file increased step from 1 minute
 * to 10 minutes
 * 
 * 6   7/9/10 5:37p vsilyaev
 * SW7405-4565: Better communicate unknown bitrate
 * 
 * 5   7/13/09 1:46p vsilyaev
 * PR 55989: Improved trickmodes and navigation
 * 
 * 4   7/8/09 11:49a vsilyaev
 * PR 55989: Added support of trickmodes
 * 
 * 3   8/29/08 5:48p vsilyaev
 * PR 33812: Added support for navigation in the MPEG-2 TS file without an
 * index
 * 
 * 2   8/18/08 4:37p vsilyaev
 * PR 33812: Added seek and next methods for the no index MPEG2TS player
 * 
 * 1   8/17/07 3:48p vsilyaev
 * PR 33812: Initial version of time indexer
 * 
 *******************************************************************************/
#ifndef _BTIME_INDEXER_H__
#define _BTIME_INDEXER_H__

#include "bmedia_player.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct btime_indexer *btime_indexer_t;

typedef struct btime_indexer_position {
	uint64_t offset; /* interpolated position */
	bmedia_player_pos time; /* interpolated time */
	unsigned byterate; /* interpolated byterate bytes/sec */
	struct {
		bmedia_player_pos time; 
		uint32_t timestamp;
		uint64_t offset;
	} prev, next;  /* data that was used for interpolation */
} btime_indexer_position;

typedef struct btime_indexer_status {
	unsigned nentries;
	unsigned byterate;
    bool byterate_valid; /* if set to false then byterate is not calculated based on the stream, but from other factors */
	bmedia_player_pos duration;
    bmedia_player_pos position; /* last position */
} btime_indexer_status;

typedef enum btime_indexer_direction {
    btime_indexer_direction_forward,
    btime_indexer_direction_backward
} btime_indexer_direction;

typedef struct btime_indexer_location {
    uint32_t timestamp;
    btime_indexer_direction direction;
    uint64_t offset;
} btime_indexer_location;

/* create index every 60 seconds (one minute) */
#define BINDEX_TIME_STEP	(60*BMEDIA_PLAYER_POS_SCALE)
/* when seeking create index entry every 10 minutes */
#define BINDEX_SEEK_STEP    (10*BINDEX_TIME_STEP)

btime_indexer_t btime_indexer_create(void);
void btime_indexer_destroy(btime_indexer_t index);
int btime_indexer_add(btime_indexer_t index, uint32_t timestamp, uint64_t off, bmedia_player_pos *position);
void btime_indexer_seek(btime_indexer_t index, bmedia_player_pos time);
bool btime_indexer_position_by_time(btime_indexer_t index, bmedia_player_pos time, btime_indexer_position *position);
void btime_indexer_get_status(btime_indexer_t index, btime_indexer_status *status);
void btime_indexer_dump(btime_indexer_t index);
int btime_indexer_get_time_by_location(btime_indexer_t index, const btime_indexer_location *location, bmedia_player_pos *pos);

#ifdef __cplusplus
}
#endif


#endif /* _BTIME_INDEXER_H__ */

