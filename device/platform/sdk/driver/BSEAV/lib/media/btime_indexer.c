/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btime_indexer.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 7/9/12 3:48p $
 *
 * Module Description:
 *
 * media navigational indexer
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/btime_indexer.c $
 * 
 * 14   7/9/12 3:48p vsilyaev
 * SW7435-256: Use BLST_AAT implementation of AA Tree
 * 
 * 13   9/28/10 3:42p vsilyaev
 * SW7335-727: Faster reach 2 node count
 * 
 * 12   7/9/10 5:37p vsilyaev
 * SW7405-4565: Better communicate unknown bitrate
 * 
 * 11   6/28/10 6:54p vsilyaev
 * SW7335-761: Updated code to return default bitrate if there is only one
 * entry in the index
 * 
 * 10   6/1/10 5:45p vsilyaev
 * SW7405-4409: Fixed range check when evaluating bitrate
 * 
 * 9   1/13/10 6:20p jtna
 * SW3556-982: add separate function for calculating position
 * 
 * 8   1/13/10 5:55p jtna
 * SW3556-982: protect against returning a negative position
 * 
 * 7   7/13/09 1:46p vsilyaev
 * PR 55989: Improved trickmodes and navigation
 * 
 * 6   7/9/09 5:12p vsilyaev
 * PR 55989: Removed unneded debug output
 * 
 * 5   7/8/09 11:49a vsilyaev
 * PR 55989: Added support of trickmodes
 * 
 * 4   8/29/08 5:48p vsilyaev
 * PR 33812: Added support for navigation in the MPEG-2 TS file without an
 * index
 * 
 * 3   8/27/08 4:20p vsilyaev
 * PR 33812: Added code to recycle index nodes
 * 
 * 2   8/18/08 4:37p vsilyaev
 * PR 33812: Added seek and next methods for the no index MPEG2TS player
 * 
 * 1   8/17/07 3:48p vsilyaev
 * PR 33812: Initial version of time indexer
 * 
 *******************************************************************************/
#include "bstd.h"
#include "btime_indexer.h"
#include "bkni.h"
#include "blst_aa_tree.h"
#include "blst_slist.h"

BDBG_MODULE(btime_indexer);
#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */


BDBG_OBJECT_ID(btime_indexer_t);


typedef struct b_time_index_node {
	uint64_t offset;
	bmedia_player_pos time; /* continious time in 1us units */
	BLST_AA_TREE_ENTRY(b_time_index_node) time_node;
	BLST_AA_TREE_ENTRY(b_time_index_node) offset_node;
	BLST_S_ENTRY(b_time_index_node) list;
	uint32_t timestamp;
    bool discontinuity;
} b_time_index_node;

#define B_INDEX_TIME_CMP(node, t) ((t) > (node)->time ? 1 : (((t) == (node)->time) ? 0: -1))

BLST_AA_TREE_HEAD(b_index_time_tree, b_time_index_node);
BLST_AA_TREE_GENERATE_INSERT(b_index_time_tree, bmedia_player_pos, b_time_index_node, time_node, B_INDEX_TIME_CMP)
BLST_AA_TREE_GENERATE_FIRST(b_index_time_tree, b_time_index_node, time_node)
BLST_AA_TREE_GENERATE_LAST(b_index_time_tree, b_time_index_node, time_node)
BLST_AA_TREE_GENERATE_NEXT(b_index_time_tree, b_time_index_node, time_node)
BLST_AA_TREE_GENERATE_PREV(b_index_time_tree, b_time_index_node, time_node)
BLST_AA_TREE_GENERATE_FIND_SOME(b_index_time_tree, bmedia_player_pos, b_time_index_node, time_node, B_INDEX_TIME_CMP)
BLST_AA_TREE_GENERATE_REMOVE(b_index_time_tree, b_time_index_node, time_node)

#define B_INDEX_OFFSET_CMP(node, off) ((off) > (node)->offset? 1 : (((off) == (node)->offset) ? 0: -1))

BLST_AA_TREE_HEAD(b_index_offset_tree, b_time_index_node);
BLST_AA_TREE_GENERATE_INSERT(b_index_offset_tree, uint64_t, b_time_index_node, offset_node, B_INDEX_OFFSET_CMP)
BLST_AA_TREE_GENERATE_FIRST(b_index_offset_tree, b_time_index_node, offset_node)
BLST_AA_TREE_GENERATE_LAST(b_index_offset_tree, b_time_index_node, offset_node)
BLST_AA_TREE_GENERATE_FIND_SOME(b_index_offset_tree, uint64_t, b_time_index_node, offset_node, B_INDEX_OFFSET_CMP)
BLST_AA_TREE_GENERATE_NEXT(b_index_offset_tree, b_time_index_node, offset_node)
BLST_AA_TREE_GENERATE_PREV(b_index_offset_tree, b_time_index_node, offset_node)
BLST_AA_TREE_GENERATE_REMOVE(b_index_offset_tree, b_time_index_node, offset_node)

#define B_TIME_INDEX_NUM_NODES	((63*1024)/sizeof(b_time_index_node))

/* threshold in bmedia_player_pos units for index entries, entries are saved if and only if, distance exceeds this threshold */
#define B_TIME_INDEX_THRESHOLD  (5000)
/* this threshold is used to detect discontinuities, 1sec difference in consecutive values of timestamp */
#define B_TIME_INDEX_DISCONTINUITY (1000)

/* calculates difference between timestamps, accounting for wrap in timesamp values */
static int
b_time_timestamp_diff(uint32_t ts1, uint32_t ts2)
{
    int32_t diff = (int32_t)ts1 - (int32_t)ts2;
    return (diff)/(45000/BMEDIA_PLAYER_POS_SCALE);
}

static bmedia_player_pos 
b_time_timestamp_abs_diff(uint32_t ts1, uint32_t ts2)
{
    int diff = b_time_timestamp_diff(ts1, ts2);
    if(diff>=0) {
        return diff;
    } else {
        return -diff;
    }
}

struct btime_indexer {
	BDBG_OBJECT(btime_indexer_t)
	struct b_index_time_tree time_tree;
	struct b_index_offset_tree offset_tree;
	BLST_S_HEAD(b_time_index_nodes, b_time_index_node) free_nodes;
	b_time_index_node *last_index_node; /* previous node index node saved in the index*/
	b_time_index_node *last_position_node; /* previous node used to detect position */
	uint32_t last_timestamp;
	uint64_t last_offset;
	unsigned num_nodes; /* number of nodes added */
	b_time_index_node node_data[B_TIME_INDEX_NUM_NODES];
};



btime_indexer_t
btime_indexer_create(void)
{
	btime_indexer_t index;
	int i;

	index = BKNI_Malloc(sizeof(*index));
	if(!index) {
		BDBG_ERR(("btime_indexer_create: can't allocate %u bytes", sizeof(*index)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(index, btime_indexer_t);
	index->last_index_node = NULL;
    index->last_position_node = NULL;
	index->last_timestamp = 0;
	index->last_offset = 0;
	index->num_nodes = 0;
	BLST_S_INIT(&index->free_nodes);
	for(i=B_TIME_INDEX_NUM_NODES-1;i>=0;i--) {
		BLST_S_INSERT_HEAD(&index->free_nodes, &index->node_data[i], list);
	}
	BLST_AA_TREE_INIT(b_index_time_tree, &index->time_tree);
	BLST_AA_TREE_INIT(b_index_offset_tree, &index->offset_tree);
	return index;

err_alloc:
	return NULL;
}

void
btime_indexer_destroy(btime_indexer_t index)
{
	BDBG_OBJECT_ASSERT(index, btime_indexer_t);
    /* btime_indexer_dump(index); */
	BDBG_OBJECT_DESTROY(index, btime_indexer_t);
	BKNI_Free(index);
	return;
}

static void
b_time_index_trim(btime_indexer_t index)
{
	unsigned trimed_nodes;
	bmedia_player_pos trim_delta;

	BDBG_MSG(("b_time_index_trim:>%#lx %u", (unsigned long)index, index->num_nodes));

	for(trimed_nodes = 0, trim_delta = BINDEX_TIME_STEP*2;trimed_nodes<=B_TIME_INDEX_NUM_NODES/10 && trim_delta <= BINDEX_TIME_STEP*100;trim_delta*=2) {
		b_time_index_node *prev_node = BLST_AA_TREE_FIRST(b_index_time_tree, &index->time_tree);
		b_time_index_node *node;
		if(prev_node==NULL) {
			break;
		}
		while(NULL!=(node=BLST_AA_TREE_NEXT(b_index_time_tree, &index->time_tree, prev_node))) {
			bmedia_player_pos delta_time, delta_timestamp;
			bmedia_player_pos diff;
			delta_timestamp = b_time_timestamp_abs_diff(node->timestamp, prev_node->timestamp);
			delta_time = node->time - prev_node->time;
			if(delta_timestamp>=delta_time) {
				diff = delta_timestamp - delta_time;
			} else {
				diff = delta_time - delta_timestamp;
			}
			BDBG_MSG(("b_time_index_trim:%#lx %#lx %u:%u(%u:%u)", (unsigned long)index, (unsigned long)node, (unsigned)delta_time, (unsigned)delta_timestamp, (unsigned)diff, trim_delta));
			if(delta_time < trim_delta && diff <= 1*BMEDIA_PLAYER_POS_SCALE && node!=index->last_index_node && node!=index->last_position_node) { /* timing jitter is less then 1second, and not cached node */
				BLST_AA_TREE_REMOVE(b_index_time_tree, &index->time_tree, node);
				BLST_AA_TREE_REMOVE(b_index_offset_tree, &index->offset_tree, node);
				BLST_S_INSERT_HEAD(&index->free_nodes, node, list);
				trimed_nodes++;
				index->num_nodes--;
			    continue;
			}
			prev_node = node;
		}
	}
	BDBG_MSG(("b_time_index_trim:<%#lx %u %u(%u)", (unsigned long)index, index->num_nodes, trimed_nodes, (unsigned)trim_delta));
	return;
}

static b_time_index_node *
b_time_index_insert(btime_indexer_t index, b_time_index_node *last_node, unsigned time, uint32_t timestamp, uint64_t off, bool discontinuity)
{
	b_time_index_node *node;
	b_time_index_node *node_time;
	b_time_index_node *node_offset;

	BDBG_MSG(("b_time_index_insert:%#lx %u:%u %llu", (unsigned long)index, (unsigned)time, (unsigned)timestamp, off));

	node = BLST_S_FIRST(&index->free_nodes);
	if(!node) {
		b_time_index_trim(index);
		node = BLST_S_FIRST(&index->free_nodes);
		if(!node) { 
	        BDBG_WRN(("b_time_index_insert:%#lx can't acquire free node", (unsigned long)index));
            node = last_node;
            goto done;
        }
	}
    node->time = time;
    node->offset = off;
    node->timestamp = timestamp;
    node->discontinuity = discontinuity;
    node_time = BLST_AA_TREE_INSERT(b_index_time_tree, &index->time_tree, time, node);
	node_offset = BLST_AA_TREE_INSERT(b_index_offset_tree, &index->offset_tree, off, node);
    if(node_time == node && node_offset == node) {
        BLST_S_REMOVE_HEAD(&index->free_nodes, list);
        index->num_nodes++;
    } else {
        BDBG_WRN(("b_time_index_insert: %#lx duplicate %s(%u:%u) %s(%u:%u)", (unsigned long)index, node_time != node ? "":"time", (unsigned)node->time, (unsigned)node_time->time, node_offset != node ? "":"offset", (unsigned)node->offset, (unsigned)node_offset->offset));
        if(node_time == node) {
            BLST_AA_TREE_REMOVE(b_index_time_tree, &index->time_tree, node);
        }
        if(node_offset == node) {
            BLST_AA_TREE_REMOVE(b_index_offset_tree, &index->offset_tree, node);
        }
        node = last_node;
    }
done:
    BDBG_ASSERT(node);
	BDBG_MSG(("b_time_index_insert:%#lx %#lx %u:%u", (unsigned long)index, (unsigned long)node, (unsigned)index->num_nodes, B_TIME_INDEX_NUM_NODES));
	return node;
}

static unsigned 
b_time_index_get_byterate_with_valid(btime_indexer_t index, const b_time_index_node *first, const b_time_index_node *last, bool *valid)
{
    unsigned byterate=0;
    unsigned default_byterate = 1000000/8; 
    bmedia_player_pos delta;

    *valid = true;
    if(last != first) {
        if(last->time > first->time && last->offset > first->offset) {
            delta = last->time - first->time;
            byterate = (BMEDIA_PLAYER_POS_SCALE*(last->offset-first->offset))/delta;
        } 
        if(byterate<(32000/8) || byterate > (108000000/8)) { /* limit bytrate */
            const b_time_index_node *node_first;
            const b_time_index_node *node_last;
            BDBG_WRN(("b_time_index_get_byterate: %#lx out of order index nodes %u:%llu %u:%llu", (unsigned long)index, (unsigned)first->time, first->offset, (unsigned)last->time, last->offset));
            node_first = BLST_AA_TREE_FIRST(b_index_offset_tree, &index->offset_tree);
            node_last = BLST_AA_TREE_LAST(b_index_offset_tree, &index->offset_tree);
            if(node_first && node_last) {
                delta = last->time - first->time;
                if(delta>0) {
                    byterate = (BMEDIA_PLAYER_POS_SCALE*(node_last->offset-node_last->offset))/delta;
                }
                if(byterate<(32000/8) || byterate > (108000000/8)) { /* limit bytrate */
                    byterate = default_byterate;
                    *valid = false;
                }
            }
        }
    } else {
        byterate = default_byterate;
        *valid = false;
    }
    BDBG_MSG_TRACE(("b_time_index_get_byterate: %#lx %u first:%u:%llu:%u last:%u:%llu:%u", (unsigned long)index, byterate, (unsigned)first->time, first->offset, (unsigned)first->timestamp, (unsigned)last->time, last->offset, (unsigned)last->timestamp));
    return byterate;
}

static unsigned 
b_time_index_get_byterate(btime_indexer_t index, const b_time_index_node *first, const b_time_index_node *last)
{
    bool valid;
    return b_time_index_get_byterate_with_valid(index, first, last, &valid);
}

static bmedia_player_pos
b_time_duration_at(btime_indexer_t index, b_time_index_node *node, uint64_t off1, uint64_t off2)
{
	b_time_index_node *prev, *next;
    unsigned byterate;

    BDBG_ASSERT(off2 >= off1);
    if(off2==off1) {
        return 0;
    }
    next = NULL;
    if(off1 > node->offset ) {
        next = BLST_AA_TREE_NEXT(b_index_offset_tree, &index->offset_tree, node);
    } 
    prev = NULL;
    if(off2 < node->offset) {
        prev = BLST_AA_TREE_PREV(b_index_offset_tree, &index->offset_tree, node);
    }
    if(next==NULL) {
        next = node;
    }
    if(prev==NULL) {
        prev = node;
    }
    if(next==prev) {
        prev = BLST_AA_TREE_PREV(b_index_offset_tree, &index->offset_tree, node);
        if(prev==NULL) {
            prev = node;
            next = BLST_AA_TREE_NEXT(b_index_offset_tree, &index->offset_tree, node);
            if(next==NULL) {
                next = node;
            }
        }
    }
    byterate = b_time_index_get_byterate(index, prev, next);
    if(byterate==0) {return BMEDIA_PLAYER_INVALID;}
    return (BMEDIA_PLAYER_POS_SCALE*(off2 - off1))/byterate;
}

int 
btime_indexer_add(btime_indexer_t index, uint32_t timestamp, uint64_t off, bmedia_player_pos *position)
{
	b_time_index_node *node = NULL;
	b_time_index_node *next_node = NULL;
	unsigned time;

	BDBG_OBJECT_ASSERT(index, btime_indexer_t);
	BDBG_MSG_TRACE(("btime_indexer_add: %#lx %u:%llu", (unsigned  long)index, timestamp, off));
    node = index->last_index_node;
	if(node) {
		int timestamp_diff;
        bmedia_player_pos time_offset;
        bool discontinuity=false;

        if(off >= node->offset) {  /* adding timestamps in forward direction */
            for(;;) {
                next_node = BLST_AA_TREE_NEXT(b_index_offset_tree, &index->offset_tree, node);
                if(next_node==NULL || next_node->offset > off) {
                    break;
                }
                node = next_node;
                index->last_timestamp = node->timestamp;
                index->last_offset = node->offset;
            }
        } else { /* adding timestamps in backward direction */
            
            for(;;) {
                next_node = BLST_AA_TREE_PREV(b_index_offset_tree, &index->offset_tree, node);
                if(next_node==NULL) {
                    BDBG_WRN(("btime_indexer_add: %#lx out of bound offset %llu", (unsigned long)index, off));
                    goto done;
                }
                node = next_node;
                index->last_timestamp = node->timestamp;
                index->last_offset = node->offset;
                if(node->offset <= off) {
                    break;
                }
            }
        }
        timestamp_diff = b_time_timestamp_diff(timestamp, node->timestamp);
        time_offset = b_time_duration_at(index, node, node->offset, off);
        time = node->time;
        if( 0 >= timestamp_diff && timestamp_diff >= -B_TIME_INDEX_DISCONTINUITY) {
            /* just ignore jitter */
            *position = time;
            index->last_timestamp = timestamp;
            index->last_offset = off;
            goto done;
        }
        if(time_offset==BMEDIA_PLAYER_INVALID) {
            if( off > index->last_offset && timestamp_diff > 0  && timestamp_diff > B_TIME_INDEX_DISCONTINUITY && timestamp_diff < B_TIME_INDEX_THRESHOLD) {
                *position = time + timestamp_diff;
                index->last_index_node = b_time_index_insert(index, node, time + timestamp_diff, timestamp, off, discontinuity);
                index->last_timestamp = index->last_index_node->timestamp;
                index->last_offset = index->last_index_node->offset;
            } else {
                index->last_timestamp = timestamp;
                index->last_offset = off;
            }
            goto done;
        }
        if(timestamp_diff<0) {
            discontinuity = true;
        } else if(time_offset > B_TIME_INDEX_THRESHOLD) {
            unsigned factor =  timestamp_diff/(time_offset/100);
            if(factor > 500) { /* 500% error */
	            BDBG_MSG(("btime_indexer_add: %#lx factor:%u timestamp_diff:%d time_offset:%d", (unsigned  long)index, factor, timestamp_diff, time_offset));
                discontinuity = true;
            }
        }
        if(discontinuity) {
            int ts_diff = b_time_timestamp_diff(node->timestamp, index->last_timestamp);
			BDBG_MSG(("btime_indexer_add: %#lx detected discontinuity %u %u:%u at %llu:%llu", (unsigned long)index, node->time, timestamp, index->last_timestamp, off, index->last_offset));
            *position = time + time_offset;
            if(off>=index->last_offset && index->last_offset != node->offset && 0 <= ts_diff && ts_diff >= B_TIME_INDEX_DISCONTINUITY ) { /* could improve location of discontinuity */
                time += ts_diff;
                if(time>*position) {
                    *position = time;
                }
			    BDBG_MSG(("btime_indexer_add: %#lx inserting at discontinuity %u %u %llu", (unsigned long)index, time, index->last_timestamp, index->last_offset));
	            index->last_index_node = b_time_index_insert(index, node, time, index->last_timestamp, index->last_offset, discontinuity);
                index->last_timestamp = index->last_index_node->timestamp;
                index->last_offset = index->last_index_node->offset;
            }
        } else {
            *position = time + timestamp_diff;
            if(timestamp_diff < B_TIME_INDEX_THRESHOLD && 
                    (index->num_nodes>1 && timestamp_diff < B_TIME_INDEX_THRESHOLD/10) /* if there is not enough nodes add extra node sooner */
                    ) {
                index->last_index_node = node;
                /* keep when needed to add discontinuity */
                index->last_timestamp = timestamp;
                index->last_offset = off;
                goto done;
            }
        } 
        index->last_index_node = b_time_index_insert(index, node, *position, timestamp, off, discontinuity);
        index->last_timestamp = index->last_index_node->timestamp;
        index->last_offset = index->last_index_node->offset;
    } else {
        index->last_index_node = b_time_index_insert(index, NULL, 0, timestamp, off, true);
        index->last_offset = index->last_index_node->offset;
        index->last_timestamp = index->last_index_node->timestamp;
    }
done:
    return index->num_nodes;
}


static bool
b_time_index_get_fork_by_offset(btime_indexer_t index, uint64_t off, b_time_index_node **prev, b_time_index_node **next)
{
	b_time_index_node *node;

	BDBG_ASSERT(prev);
	BDBG_ASSERT(next);

	node = BLST_AA_TREE_FIND_SOME(b_index_offset_tree, &index->offset_tree, off);
	if(node) {
		if(node->offset>=off) {
			*next =  node;
			node = BLST_AA_TREE_PREV(b_index_offset_tree, &index->offset_tree, node);
			if(node) {
				*prev = node;
			} else {
				*prev = *next;
				node = BLST_AA_TREE_NEXT(b_index_offset_tree, &index->offset_tree, *prev);
				if(node) {
					*next = node;
				}	
			}
		} else {
			*prev = node;
			node = BLST_AA_TREE_NEXT(b_index_offset_tree, &index->offset_tree, node);
			if(node) {
				*next = node;
			} else {
				*next = *prev;
				node = BLST_AA_TREE_PREV(b_index_offset_tree, &index->offset_tree, *prev);
				if(node) {
					*prev = node;
				}	
			}
		}
		BDBG_MSG_TRACE(("b_time_index_get_fork_by_offset: %#lx:%llu prev:%u:%llu:%u next:%u:%llu:%u", (unsigned long)index, off, (*prev)->time, (*prev)->offset, (*prev)->timestamp, (*next)->time, (*next)->offset, (*next)->timestamp));
		return true;
	} else {
		return false;
	}
}

static bool
b_time_index_get_fork_by_time(btime_indexer_t index, bmedia_player_pos time, b_time_index_node **prev, b_time_index_node **next)
{
	b_time_index_node *node;

	BDBG_ASSERT(prev);
	BDBG_ASSERT(next);

	node = BLST_AA_TREE_FIND_SOME(b_index_time_tree, &index->time_tree, time);
	if(node) {
		BDBG_MSG_TRACE(("b_time_index_get_fork_by_time: %#lx %u node:%u:%llu:%u", (unsigned long)index, time, node->time, node->offset, node->timestamp));
		if(node->time>=time) {
			*next =  node;
			node = BLST_AA_TREE_PREV(b_index_time_tree, &index->time_tree, node);
			if(node) {
				*prev = node;
			} else {
				*prev = *next;
				node = BLST_AA_TREE_NEXT(b_index_time_tree, &index->time_tree, *prev);
				if(node) {
					*next = node;
				}	
			}
		} else {
			*prev = node;
			node = BLST_AA_TREE_NEXT(b_index_time_tree, &index->time_tree, node);
			if(node) {
				*next = node;
			} else {
				*next = *prev;
				node = BLST_AA_TREE_PREV(b_index_time_tree, &index->time_tree, *prev);
				if(node) {
					*prev = node;
				}	
			}
		}
		BDBG_MSG_TRACE(("b_time_index_get_fork_by_time: %#lx %u prev:%u:%llu:%u next:%u:%llu:%u", (unsigned long)index, time, (*prev)->time, (*prev)->offset, (*prev)->timestamp, (*next)->time, (*next)->offset, (*next)->timestamp));
		return true;
	} else {
		return false;
	}
}

void 
btime_indexer_seek(btime_indexer_t index, bmedia_player_pos time)
{
	b_time_index_node *prev;
	b_time_index_node *next;

	BDBG_OBJECT_ASSERT(index, btime_indexer_t);
	if(b_time_index_get_fork_by_time(index, time, &prev, &next)) {
		if(next->time < time) {
			prev = next;
		}
        index->last_index_node = prev;
		index->last_position_node = NULL;
		index->last_timestamp = prev->timestamp;
		index->last_offset = prev->offset;
        BDBG_MSG(("btime_indexer_seek: %#lx %u->%u:%llu:%u", (unsigned long)index, (unsigned)time, (unsigned)prev->time, prev->offset, (unsigned)prev->timestamp));
	} else {
        index->last_timestamp = 0;
        index->last_offset = 0;
		index->last_index_node = NULL;
        index->last_position_node = NULL;
	}
	return;
}

bool
btime_indexer_position_by_time(btime_indexer_t index, bmedia_player_pos time, btime_indexer_position *position)
{
	b_time_index_node *prev;
	b_time_index_node *next;

	BDBG_OBJECT_ASSERT(index, btime_indexer_t);
	BDBG_ASSERT(position);

	if(b_time_index_get_fork_by_time(index, time, &prev, &next)) {
		unsigned byterate = b_time_index_get_byterate(index, prev, next);
		position->time = time;
		position->next.time = next->time;
		position->next.timestamp = next->timestamp;
		position->next.offset = next->offset;
		position->prev.time = prev->time;
		position->prev.timestamp = prev->timestamp;
		position->prev.offset = prev->offset;
		if(byterate>0) {
            off_t delta = 0;
            BDBG_MSG(("btime_indexer_position_by_time: %#lx time %u..%u..%u", (unsigned long)index, prev->time, time, next->time));
            if(time >= prev->time) {
                if(time<=next->time) {
		            position->offset = prev->offset;
                    delta = byterate*(off_t)(time-prev->time)/BMEDIA_PLAYER_POS_SCALE;
                } else {
		            position->offset = next->offset;
                    delta = byterate*(off_t)(time-next->time)/BMEDIA_PLAYER_POS_SCALE;
                }
            } 
			BDBG_MSG(("btime_indexer_position_by_time: %#lx(%u bytes/sec) offset adjusted by %u bytes", (unsigned long)index, byterate, (unsigned)delta));
			position->offset += delta;
			position->byterate = byterate;
		} else {
			/* try to get other nodes into equation */
			b_time_index_node *node = BLST_AA_TREE_PREV(b_index_time_tree, &index->time_tree, prev);
			if(node) {
				prev = prev;
			} else {
				node = BLST_AA_TREE_NEXT(b_index_time_tree, &index->time_tree, next);
				if(node) {
					next = node;
				}
			}
		    position->offset = prev->offset;
			position->byterate = b_time_index_get_byterate(index, prev, next);
		}
		BDBG_MSG(("btime_indexer_position_by_time: %#lx %u:%llu prev:%u:%llu:%u next:%u:%llu:%u byterate:%u", (unsigned long)index, position->time, position->offset, position->prev.time, position->prev.offset, position->prev.timestamp, position->next.time, position->next.offset, position->next.timestamp, position->byterate));
		return true;
	}
	return false;
}

void
btime_indexer_get_status(btime_indexer_t index, btime_indexer_status *status)
{
	b_time_index_node *first, *last;

	BDBG_OBJECT_ASSERT(index, btime_indexer_t);
	BDBG_ASSERT(status);

	first = BLST_AA_TREE_FIRST(b_index_time_tree, &index->time_tree);
	last = BLST_AA_TREE_LAST(b_index_time_tree, &index->time_tree);
	status->nentries = index->num_nodes;
	status->byterate = 0;
	status->duration = 0;
    status->position = 0;
    status->byterate_valid = false;
	if(first && last) {
		status->byterate = b_time_index_get_byterate_with_valid(index, first, last, &status->byterate_valid);
		status->duration = last->time;
	}
    if(index->last_index_node) {
	    status->position = index->last_index_node->time;
    }
	BDBG_MSG(("btime_indexer_get_status: %#lx nentries:%u byterate:%u duration:%u", (unsigned long)index, status->nentries, status->byterate, status->duration));
	return;
}

bmedia_player_pos b_time_indexer_get_position(int timestamp_diff, unsigned node_time)
{
    if(timestamp_diff>0 || node_time >= (unsigned)-timestamp_diff) {
        return node_time + timestamp_diff;
    } else {
        return 0;
    }
}

int 
btime_indexer_get_time_by_location(btime_indexer_t index, const btime_indexer_location *location, bmedia_player_pos *pos)
{
	b_time_index_node *next;
	b_time_index_node *prev;
	b_time_index_node *node;
    int timestamp_diff;
    unsigned i;
    b_time_index_node *next_node;


	BDBG_OBJECT_ASSERT(index, btime_indexer_t);
	BDBG_ASSERT(location);
	BDBG_ASSERT(pos);
    BDBG_MSG(("btime_indexer_get_time_by_location: %#lx %lu:%u:%s %#lx:%lu", (unsigned long)index, (unsigned long)location->offset, (unsigned)location->timestamp, location->direction==btime_indexer_direction_forward?"forward":"backward", (unsigned long)index->last_position_node, (unsigned long)index->last_position_node?index->last_position_node->timestamp:0));
    
    if(index->last_position_node==NULL) {
        if(!b_time_index_get_fork_by_offset(index, location->offset, &prev, &next)) {
            *pos=0;
            return -1;
        }
        if(location->direction==btime_indexer_direction_backward) { /* search forward and then backward */
            for(node=prev,i=0;i<10;i++) {
                timestamp_diff = b_time_timestamp_diff(location->timestamp, node->timestamp);
                if( -B_TIME_INDEX_THRESHOLD <= timestamp_diff && timestamp_diff <=  B_TIME_INDEX_THRESHOLD) { /* if node is close enough */
                    *pos = b_time_indexer_get_position(timestamp_diff, node->time);
                    index->last_position_node = node;
                    goto done;
                }
                next_node = BLST_AA_TREE_NEXT(b_index_offset_tree, &index->offset_tree, node);
                if(!next_node) {
                    break;
                }
                if(timestamp_diff >= 0 && b_time_timestamp_diff(next_node->timestamp, node->timestamp)>=0) {
                    /* timestamps are continuous and in the order [PREV] .. [PTS] ..  [NEXT] */
                    *pos = b_time_indexer_get_position(timestamp_diff, node->time);
                    index->last_position_node = node;
                    goto done;
                }
                node = next;
            }
        }
        for(node=next;;) { /* search either forward or backward depend on the direction */
            timestamp_diff = b_time_timestamp_diff(location->timestamp, node->timestamp);
            if( -B_TIME_INDEX_THRESHOLD <= timestamp_diff && timestamp_diff <=  B_TIME_INDEX_THRESHOLD) { /* if node is close enough */
                *pos = b_time_indexer_get_position(timestamp_diff, node->time);
                index->last_position_node = node;
                goto done;
            }
            if(location->direction==btime_indexer_direction_forward) {
                next_node = BLST_AA_TREE_PREV(b_index_offset_tree, &index->offset_tree, node);
                if(next_node) {
                    if(timestamp_diff >= 0 && b_time_timestamp_diff(node->timestamp, next_node->timestamp)>=0) {
                        /* timestamps are continuous and in the order [PREV] .. [PTS] ..  [NEXT] */
                        *pos = b_time_indexer_get_position(timestamp_diff, node->time);
                        index->last_position_node = node;
                        goto done;
                    }
                }
             } else {
                next_node = BLST_AA_TREE_NEXT(b_index_offset_tree, &index->offset_tree, node);
                if(next_node) {
                    if(timestamp_diff >= 0 && b_time_timestamp_diff(next_node->timestamp, node->timestamp)>=0) {
                        /* timestamps are continuous and in the order [PREV] .. [PTS] ..  [NEXT] */
                        *pos = b_time_indexer_get_position(timestamp_diff, node->time);
                        index->last_position_node = node;
                        goto done;
                    }
                }
            }
            if(!next_node) {
                break;
            }
            node = next_node;
        }
        *pos = prev->time; /* use position solely based on the offset */
        goto done;
    } 
    for(node=index->last_position_node;;) {
        timestamp_diff = b_time_timestamp_diff(location->timestamp, node->timestamp);
        if( -B_TIME_INDEX_THRESHOLD <= timestamp_diff && timestamp_diff <=  B_TIME_INDEX_THRESHOLD) { /* if node is close enough */
            *pos = b_time_indexer_get_position(timestamp_diff, node->time);
            index->last_position_node = node;
            goto done;
        }
        if(location->direction==btime_indexer_direction_backward) {
            next_node = BLST_AA_TREE_PREV(b_index_offset_tree, &index->offset_tree, node);
            if(next_node) {
                if(timestamp_diff >= 0 && b_time_timestamp_diff(node->timestamp, next_node->timestamp)>=0) {
                    /* timestamps are continuous and in the order [PREV] .. [PTS] ..  [NEXT] */
                    *pos = b_time_indexer_get_position(timestamp_diff, node->time);
                    index->last_position_node = node;
                    goto done;
                }
            }
         } else {
            next_node = BLST_AA_TREE_NEXT(b_index_offset_tree, &index->offset_tree, node);
            if(next_node) {
                if(timestamp_diff >= 0 && b_time_timestamp_diff(next_node->timestamp, node->timestamp)>=0) {
                    /* timestamps are continuous and in the order [PREV] .. [PTS] ..  [NEXT] */
                    *pos = b_time_indexer_get_position(timestamp_diff, node->time);
                    index->last_position_node = node;
                    goto done;
                }
            }
        }
        if(!next_node) {
            break;
        }
        node = next_node;
    }
    /* return last known time, and reset cache */
    *pos = index->last_position_node->time;
    index->last_position_node = NULL;
done:
	return 0;
}


void 
btime_indexer_dump(btime_indexer_t index)
{
	b_time_index_node *node;
	b_time_index_node *prev;
	b_time_index_node *first;
	BDBG_OBJECT_ASSERT(index, btime_indexer_t);

	for(prev=NULL, first=node=BLST_AA_TREE_FIRST(b_index_offset_tree, &index->offset_tree);node;
		prev=node,node=BLST_AA_TREE_NEXT(b_index_offset_tree, &index->offset_tree, node)) {
		BDBG_WRN(("btime_indexer_dump: %#lx *offset* timestamp:%u offset:%llu time:%u bitrate:%u:%u", (unsigned long)index, node->timestamp, node->offset, node->time, prev?8*b_time_index_get_byterate(index,prev, node):0, 8*b_time_index_get_byterate(index,first,node)));
	}
	for(prev=NULL, first=node=BLST_AA_TREE_FIRST(b_index_time_tree, &index->time_tree);node;
		prev=node,node=BLST_AA_TREE_NEXT(b_index_time_tree, &index->time_tree, node)) {
		BDBG_WRN(("btime_indexer_dump: %#lx *time* timestamp:%u offset:%llu time:%u bitrate:%u:%u", (unsigned long)index, node->timestamp, node->offset, node->time, prev?8*b_time_index_get_byterate(index,prev, node):0, 8*b_time_index_get_byterate(index,first,node)));
	}
    node = BLST_AA_TREE_LAST(b_index_time_tree, &index->time_tree);
    if(node) {
        bmedia_player_pos last_time = (12*node->time)/10;
        uint64_t last_offset = (12*node->offset)/10;
	    bmedia_player_pos time;
	    uint64_t offset;

        for(offset=0;offset<last_offset;offset+=1024*1024) {
            if(!b_time_index_get_fork_by_offset(index, offset, &prev, &node)) {
                break;
            }
            BDBG_WRN(("offset: %llu prev: %llu:%u next: %llu:%u", offset, prev->offset, prev->time, node->offset, node->time));
        }
        for(time=0;time<last_time;time+=10*BMEDIA_PLAYER_POS_SCALE) {
            btime_indexer_position position;
            if(!b_time_index_get_fork_by_time(index, time, &prev, &node) || !btime_indexer_position_by_time(index, time, &position)) {
                break;
            }
            BDBG_WRN(("time:%lu offset:%llu prev: %llu:%u next: %llu:%u", time, position.offset, prev->offset, prev->time, node->offset, node->time));
        }
    }

	return;
}


