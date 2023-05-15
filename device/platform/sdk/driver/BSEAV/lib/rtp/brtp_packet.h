/***************************************************************************
 *     Copyright (c) 2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brtp_packet.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/18/06 4:39p $
 *
 * Module Description:
 *
 * RTP parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/rtp/brtp_packet.h $
 * 
 * 3   9/18/06 4:39p vsilyaev
 * PR 23826: More work for RTP timing
 * 
 * 2   8/25/06 2:10p vsilyaev
 * PR 23826: Added MPEG-4 AAC audio parser
 * 
 * 1   8/22/06 4:35p vsilyaev
 * PR 23826: RTP H.264 library
 * 
 *******************************************************************************/
#ifndef _BRTP_PACKET_H__
#define _BRTP_PACKET_H__

#include "blst_squeue.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define B_RTP_LOAD8(ptr, off)  ((((uint8_t *)(ptr))[(off)+0])) 
#define B_RTP_LOAD16(ptr, off)  (((uint16_t)((uint8_t *)(ptr))[(off)+0]<<8) | ((uint16_t)((uint8_t *)(ptr))[(off)+1]))

#define B_RTP_LOAD32(ptr, off)  (((uint32_t)((uint8_t *)(ptr))[(off)+0]<<24) | ((uint32_t)((uint8_t *)(ptr))[(off)+1]<<16) | ((uint32_t)((uint8_t *)(ptr))[(off)+2]<<8) | ((uint32_t)((uint8_t *)(ptr))[(off)+3]))

#define B_RTP_GET_BIT(w,b)  ((w)&(1<<((sizeof(w)*8-1)-(b))))
#define B_RTP_GET_BITS(w,f,l)  (((w)>>((sizeof(w)*8-1)-(l)))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(l+1-f))))



#define B_GET_BIT(w,b)  ((w)&(1<<(b)))
#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))

#define B_SET_BIT(name,v,b)  (((unsigned)((v)!=0)<<(b)))
#define B_SET_BITS(name,v,e,b)  (((unsigned)(v))<<(b))
#define B_RTP_TIMESTAMP(pkt) B_RTP_LOAD32(pkt,4)

#define B_RTP_PKT_SEQUENCE(pkt) B_RTP_LOAD16((pkt)->data,2)
#define B_RTP_PT(pkt) B_RTP_GET_BITS(B_RTP_LOAD32(pkt,0), 9, 15)


#define B_RTP_PKT_TIMESTAMP(pkt) B_RTP_TIMESTAMP((pkt)->data)
#define B_RTP_PKT_MARKER(pkt)  B_RTP_GET_BIT(B_RTP_LOAD8((pkt)->data,1), (8-8))



struct brtp_packet {
	BLST_SQ_ENTRY(brtp_packet) queue; /* time ordered queue */
	const uint8_t *data; /* pointer to the payload */
	size_t len; /* length of the packet */
	unsigned ref_cnt; /* reference counter */
	void *cookie; /* user supplied cookie */
};

#ifdef __cplusplus
}
#endif

#endif /* _BRTP_PACKET_H__ */

