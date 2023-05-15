/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmkv_player.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 12/2/09 11:26a $
 *
 * Module Description:
 *
 * MKV library, player interface
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmkv_player.h $
 * 
 * 5   12/2/09 11:26a vsilyaev
 * SW7405-3368: Fixed typos
 * 
 * dev_bdvd_v3.0_issue15380/1   12/2/09 11:23a gooslin
 * Fix typos
 * 
 * 4   11/17/09 12:09p vsilyaev
 * SW7405-3368: Added code to insert the AttachmentLink UID
 * 
 * 3   11/5/09 8:14p vsilyaev
 * SW7405-3349: Sent codec private data and duration among with auxilary
 * payload
 * 
 * 2   12/19/08 3:57p vsilyaev
 * PR 50214: Allowed set_direction function to fail and added framework
 * for players to communicate about an errors
 * 
 * 1   3/3/08 11:33a vsilyaev
 * PR 39818: Support of MKV format
 * 
 * 
 *******************************************************************************/
#ifndef _BMKV_PLAYER_H__
#define _BMKV_PLAYER_H__

#include "bmedia_player.h"
#ifdef __cplusplus
extern "C"
{
#endif


typedef struct bmkv_player *bmkv_player_t;

#define B_MKV_AUX_TAG_PAYLOAD   0
#define B_MKV_AUX_TAG_CODEC_PRIVATE 1
#define B_MKV_AUX_TAG_DURATION  2
#define B_MKV_AUX_TAG_ATTACHMENT_LINK   3

bmkv_player_t bmkv_player_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream);
void bmkv_player_destroy(bmkv_player_t player);
int bmkv_player_next(bmkv_player_t player, bmedia_player_entry *entry);
void bmkv_player_tell(bmkv_player_t player, bmedia_player_pos *pos);
void bmkv_player_get_status(bmkv_player_t player, bmedia_player_status *status);
int bmkv_player_set_direction(bmkv_player_t player, bmedia_player_step direction, bmedia_time_scale time_scale);
int bmkv_player_seek(bmkv_player_t player, bmedia_player_pos pos);

extern const bmedia_player_methods bmkv_player_methods;

#ifdef __cplusplus
}
#endif


#endif /* _BMKV_PLAYER_H__ */

