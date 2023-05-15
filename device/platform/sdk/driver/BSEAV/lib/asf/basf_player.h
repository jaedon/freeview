/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_player.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 7/18/12 2:51p $
 *
 * Module Description:
 *
 * ASF parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_player.h $
 * 
 * 7   7/18/12 2:51p jtna
 * SW7346-733: merge improved no-index handling
 * 
 * 6   7/9/07 3:38p vsilyaev
 * PR 32846: Updated ASF code to work with batom/SPF framework
 * 
 * 5   5/2/06 5:51p vsilyaev
 * PR 20683: Added support for the navigation in ASF files
 * 
 * 4   5/1/06 2:58p vsilyaev
 * PR 20683: Improved the asf_player support
 * 
 * 3   4/27/06 6:13p vsilyaev
 * PR 20683: Added support for the asf_player into the SettopApi
 * 
 * 2   4/12/06 10:33a vsilyaev
 * PR 20308: Improved parsing of compressed payload
 * 
 * 1   4/7/06 12:25p vsilyaev
 * PR 20683: added basf_player
 * 
 *******************************************************************************/
#ifndef _BASF_PLAYER_H__
#define _BASF_PLAYER_H__

#include "bmedia_player.h"
#ifdef __cplusplus
extern "C"
{
#endif


typedef struct basf_player *basf_player_t;


basf_player_t
basf_player_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream);
void basf_player_destroy(basf_player_t player);
int basf_player_next(basf_player_t player, bmedia_player_entry *entry);
int basf_player_seek(basf_player_t player, bmedia_player_pos pos);
void basf_player_tell(basf_player_t player, bmedia_player_pos *pos);
void basf_player_get_status(basf_player_t player, bmedia_player_status *status);
void basf_player_set_direction(basf_player_t player, bmedia_player_step direction);
int basf_player_seek(basf_player_t player, bmedia_player_pos pos);

extern const bmedia_player_methods basf_player_methods;

#ifdef __cplusplus
}
#endif

#endif /* _BASF_PLAYER_H__ */

