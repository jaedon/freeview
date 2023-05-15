/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_index.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 12/15/10 1:35p $
 *
 * Module Description:
 *
 * BMedia library, incremental index interface
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_index.h $
 * 
 * 2   12/15/10 1:35p vsilyaev
 * SW35230-2279: Validate size of the index entries
 * 
 * 1   5/7/07 5:35p vsilyaev
 * PR 29921: Generic timebased index cache routines
 * 
 * 
 *******************************************************************************/
#ifndef _BMEDIA_INDEX_H_
#define _BMEDIA_INDEX_H_

#include "bmedia_player.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bmedia_index *bmedia_index_t;

bmedia_index_t bmedia_index_create(bmedia_player_pos duration, size_t entry_size, size_t nentries);
void bmedia_index_destroy(bmedia_index_t index);
bmedia_player_pos bmedia_index_next(bmedia_index_t index);
void bmedia_index_add(bmedia_index_t index, bmedia_player_pos time, const void *sample);
bmedia_player_pos bmedia_index_get_safe(bmedia_index_t index, bmedia_player_pos time, void *sample, size_t sample_size);
#define bmedia_index_get(index,time,sample) bmedia_index_get_safe((index), (time), (sample), sizeof(*(sample)))

#ifdef __cplusplus
}
#endif


#endif /* _BMEDIA_INDEX_H_ */

