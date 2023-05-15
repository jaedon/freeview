/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg2ts_player.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 6/19/09 3:47p $
 *
 * Module Description:
 *
 * player interface for MPEG2 Transport stream
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg2ts_player.h $
 * 
 * 3   6/19/09 3:47p vsilyaev
 * PR 55989: Added decode_mode  paramtewr to to the set_direction method
 * 
 * 2   8/29/08 5:48p vsilyaev
 * PR 33812: Added support for navigation in the MPEG-2 TS file without an
 * index
 * 
 * 1   8/17/07 3:48p vsilyaev
 * PR 33812: Initial version of player of  MPEG2TS streams (no index)
 * 
 *******************************************************************************/
#ifndef _BMPEG2TS_PLAYER_H__
#define _BMPEG2TS_PLAYER_H__

#include "bmedia_player.h"
#ifdef __cplusplus
extern "C"
{
#endif


typedef struct bmpeg2ts_player *bmpeg2ts_player_t;

bmpeg2ts_player_t bmpeg2ts_player_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream);
void bmpeg2ts_player_destroy(bmpeg2ts_player_t player);
int bmpeg2ts_player_next(bmpeg2ts_player_t player, bmedia_player_entry *entry);
void bmpeg2ts_player_tell(bmpeg2ts_player_t player, bmedia_player_pos *pos);
void bmpeg2ts_player_get_status(bmpeg2ts_player_t player, bmedia_player_status *status);
void bmpeg2ts_player_set_direction(bmpeg2ts_player_t player, bmedia_player_step direction, bmedia_time_scale time_scale, bmedia_player_decoder_mode *mode);
int bmpeg2ts_player_seek(bmpeg2ts_player_t player, bmedia_player_pos pos);
void bmpeg2ts_player_update_position(bmpeg2ts_player_t player, uint32_t pts);

extern const bmedia_player_methods bmpeg2ts_player_methods;

#ifdef __cplusplus
}
#endif


#endif /* _BMPEG2TS_PLAYER_H__ */

