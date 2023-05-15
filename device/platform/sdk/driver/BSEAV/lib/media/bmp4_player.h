/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmp4_player.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 6/20/11 7:49p $
 *
 * Module Description:
 *
 * MP4 library, player interface
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmp4_player.h $
 * 
 * 6   6/20/11 7:49p vsilyaev
 * SWBLURAY-26273: Added hooks to deliver content for external decryption
 * 
 * 5   4/8/10 6:58p vsilyaev
 * SW7405-3773: Initial support for playback of MPEG4-Part12 fragmented
 * files
 * 
 * 4   6/6/07 5:23p vsilyaev
 * PR 31887: Use asynchronous I/O for MP4 container
 * 
 * 3   5/16/07 9:57p vsilyaev
 * PR 28631: Use scatter/gather object from the file buffer
 * 
 * 2   5/11/07 4:05p vsilyaev
 * PR 29921: Added fast forward for MP4 container
 * 
 * 1   4/26/07 2:19p vsilyaev
 * PR 28631: MP4 player
 * 
 * 
 *******************************************************************************/
#ifndef _BMP4_PLAYER_H__
#define _BMP4_PLAYER_H__

#include "bmedia_player.h"
#ifdef __cplusplus
extern "C"
{
#endif


typedef struct bmp4_player *bmp4_player_t;

bmp4_player_t bmp4_player_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream);
void bmp4_player_destroy(bmp4_player_t player);
int bmp4_player_next(bmp4_player_t player, bmedia_player_entry *entry);
void bmp4_player_tell(bmp4_player_t player, bmedia_player_pos *pos);
void bmp4_player_get_status(bmp4_player_t player, bmedia_player_status *status);
int bmp4_player_set_direction(bmp4_player_t player, bmedia_player_step direction, bmedia_time_scale time_scale);
int bmp4_player_seek(bmp4_player_t player, bmedia_player_pos pos);

extern const bmedia_player_methods bmp4_player_methods;

typedef struct bmp4_player_drm_info {
    enum {bmp4_player_drm_info_frame, bmp4_player_drm_info_track_fragment} type;
    union {
        struct {
            off_t offset;
            size_t length;
        } track_fragment;
    } data;
} bmp4_player_drm_info;

#ifdef __cplusplus
}
#endif


#endif /* _BMP4_PLAYER_H__ */

