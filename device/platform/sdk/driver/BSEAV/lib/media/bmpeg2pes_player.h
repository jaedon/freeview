/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg2pes_player.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/4/09 6:59p $
 *
 * Module Description:
 *
 * player interface for MPEG2 PES
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg2pes_player.h $
 * 
 * 1   8/4/09 6:59p gmohile
 * PR 55991 : Add pes player
 * 
 *
 *******************************************************************************/
#ifndef _BMPEG2PES_PLAYER_H__
#define _BMPEG2PES_PLAYER_H__

#include "bmedia_player.h"
#ifdef __cplusplus
extern "C"
{
#endif


typedef struct bmpeg2pes_player *bmpeg2pes_player_t;

bmpeg2pes_player_t bmpeg2pes_player_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream);
void bmpeg2pes_player_destroy(bmpeg2pes_player_t player);
int bmpeg2pes_player_next(bmpeg2pes_player_t player, bmedia_player_entry *entry);
void bmpeg2pes_player_tell(bmpeg2pes_player_t player, bmedia_player_pos *pos);
void bmpeg2pes_player_get_status(bmpeg2pes_player_t player, bmedia_player_status *status);
void bmpeg2pes_player_set_direction(bmpeg2pes_player_t player, bmedia_player_step direction, bmedia_time_scale time_scale, bmedia_player_decoder_mode *mode);
int bmpeg2pes_player_seek(bmpeg2pes_player_t player, bmedia_player_pos pos);
void bmpeg2pes_player_update_position(bmpeg2pes_player_t player, uint32_t pts);

extern const bmedia_player_methods bmpeg2pes_player_methods;

#ifdef __cplusplus
}
#endif


#endif /* _BMPEG2PES_PLAYER_H__ */

