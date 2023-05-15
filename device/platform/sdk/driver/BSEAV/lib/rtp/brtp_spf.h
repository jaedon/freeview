/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brtp_spf.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 5/1/07 11:33a $
 *
 * Module Description:
 *
 * RTP parser library using Stackable Parsing Framework
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/rtp/brtp_spf.h $
 * 
 * 2   5/1/07 11:33a jjordan
 * PR30200: add copy to user callback
 * 
 * 1   4/25/07 9:15a jjordan
 * PR30200: adopt Stackable Parsing Framework in playpump_ip and brtp
 * modules
 * 
 *******************************************************************************/
#ifndef _BRTP_SPF_H__
#define _BRTP_SPF_H__

#include "blst_squeue.h"
#include "bioatom.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* RTP parser object */
typedef struct brtp *brtp_t;

/* RTP packet object */
typedef struct brtp_packet *brtp_packet_t;

/* configuration for the RTP parser */
typedef struct brtp_cfg {
	unsigned npackets; /* maximum number of allocated packets */
	unsigned wait_time; /* maximun time, that packet could stay in the reordering queue, ms */
	batom_factory_t factory;
	int (*copy_to_user)(void *dst, void *src, unsigned size);
} brtp_cfg;

typedef struct brtp_session_cfg {
	uint8_t pt_mask;
	uint8_t pt_value;
	uint32_t ssrc_mask;
	uint32_t ssrc_value;
} brtp_session_cfg;

typedef struct brtp_pkt_info {
	uint32_t ssrc;
	uint32_t timestamp;
	uint16_t sequence;
	uint16_t packet_size;
} brtp_pkt_info;

typedef enum brtp_enqueue {brtp_enqueue_queued, brtp_enqueue_invalid, brtp_enqueue_discard, brtp_enqueue_overflow} brtp_enqueue;

/*
Summary:
	Get the default config for the brtp module
*/
void brtp_default_cfg(brtp_cfg *cfg);

/*
Summary:
	Create the brtp module instance
*/
brtp_t brtp_create(const brtp_cfg *cfg);

/*
Summary:
	Destroy the brtp module instance
*/
void brtp_destroy(brtp_t rtp);

/*
Summary:
	Get the default brtp session config
*/
void brtp_default_session_cfg(brtp_t rtp, brtp_session_cfg *cfg);

/*
Summary:
	Start brtp processing with particular session settings, using the provided output pipe
*/
void brtp_start(brtp_t rtp, batom_pipe_t pipe, const brtp_session_cfg *cfg);
	
/*
Summary:
	Stop the brtp session
*/
void brtp_stop(brtp_t rtp);

/*
Summary:
	Feed data to the particular rtp session, using the pipe provided as the source of data
*/
brtp_enqueue brtp_feed(brtp_t rtp, batom_pipe_t pipe_in);

/*
Summary:
	Gets packet info needed for RTCP stats
*/
int brtp_get_pkt_info(brtp_t rtp, brtp_pkt_info *info, size_t max_entries);


#ifdef __cplusplus
}
#endif


#endif /* _BRTP_SPF_H__ */

