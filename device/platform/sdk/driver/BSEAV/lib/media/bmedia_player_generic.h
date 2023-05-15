/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_player_generic.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 12/20/07 10:29a $
 *
 * Module Description:
 *
 * BMedia library, generic(no index) player 
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_player_generic.h $
 * 
 * 2   12/20/07 10:29a vsilyaev
 * PR 38073: Updated to work with new bmedia_player
 * 
 * PR38073/1   12/18/07 5:02p vsilyaev
 * PR 38073: Improved code to handle MPEG-2 TS packets
 * 
 * 1   12/10/07 4:34p vsilyaev
 * PR 38073: Generic (no index player)
 * 
 *******************************************************************************/
#ifndef _BMEDIA_PLAYER_GENERIC_H__
#define _BMEDIA_PLAYER_GENERIC_H__

#include "bmedia_player.h"
#ifdef __cplusplus
extern "C"
{
#endif


extern const bmedia_player_methods bmedia_player_generic;

#ifdef __cplusplus
}
#endif


#endif /* _BMEDIA_PLAYER_GENERIC_H__ */

