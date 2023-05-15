/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsink_playback_debug.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 11/23/10 8:34p $
 *
 * Module Description:
 *
 * ATOM playback data sink
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/utils/bsink_playback_debug.h $
 * 
 * 1   11/23/10 8:34p vsilyaev
 * SW35230-2195: Format of the timed stream data
 * 
 * 	
 ***************************************************************************/
#ifndef __BSINK_PLAYBACK_DUMP_H__
#define __BSINK_PLAYBACK_DUMP_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* size of the packet is always multipler of 4, if payload length is not multiplier of four, then packet is padded */
#define BSINK_PLAYBACK_SYNC 0xBCDABEEF 
typedef struct bsink_playback_packet {
    uint32_t sync; /* 0xBCDABEEF */
    uint32_t timestamp; /* timestamp in ms */
    uint32_t sequence; /* used to combine packet into the block */
    uint32_t length; /*  length of payload */
} bsink_playback_packet;


#ifdef __cplusplus
}
#endif

#endif /* __BSINK_PLAYBACK_DUMP_H__ */

