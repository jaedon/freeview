/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_ip.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/15/07 2:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_ip.c $
 * 
 * 1   10/15/07 2:36p erickson
 * PR36068: initial
 * 
 * 5   9/4/07 10:19a jgarrett
 * PR 34578: Adding bsettop_ip_protocol_rtp_fec
 * 
 * 4   8/9/07 6:57p jgarrett
 * PR33786: Net DMA & IP Streamer refactoring into bnetaccel driver +
 * added new protocol for streaming over TCP
 * 
 * 3   7/9/07 5:33p jgarrett
 * PR 30010: Adding BRCM_UDP_SIZE_DGRAM_RECV
 * 
 * 2   7/6/07 6:08p jgarrett
 * PR 30010: Adding types for IPv6 and SOCK_BRCM_xxx definitions.
 * 
 * 1   4/19/07 7:28p jgarrett
 * PR 30010: Add TCP support
 *
 *******************************************************************************/
#ifndef BSETTOP_IP_PROTOCOL_H__
#define BSETTOP_IP_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Used to specify what protocol is in use with the session
 **/
typedef enum bsettop_ip_protocol {
	bsettop_ip_protocol_udp =0,				/* plain UDP */
	bsettop_ip_protocol_rtp,				/* RTP and RTCP */
	bsettop_ip_protocol_generic,			/* No IP protocol bound */ 
	bsettop_ip_protocol_rtp_no_rtcp,		/* RTP, but no RTCP */
	bsettop_ip_protocol_rtp_direct,			/* Feed RTP packet directly to PB */
	bsettop_ip_protocol_rtp_fec,            /* RTP Streaming with FEC */
	bsettop_ip_protocol_rtsp,				/* RTSP */
	bsettop_ip_protocol_http,				/* HTTP */
	bsettop_ip_protocol_udp_socket,         /* UDP Payload via Socket Interface */
	bsettop_ip_protocol_rtp_socket,         /* RTP Payload via Socket Interface */
	bsettop_ip_protocol_http_socket,        /* HTTP Payload via Socket Interface */
	bsettop_ip_protocol_udp6_socket,        /* IPv6 UDP Payload via Socket Interface */
	bsettop_ip_protocol_rtp6_socket,        /* IPv6 RTP Payload via Socket Interface */
	bsettop_ip_protocol_http6_socket,       /* IPv6 HTTP Payload via Socket Interface */
	bsettop_ip_protocol_tcp                 /* Streaming over TCP */
} bsettop_ip_protocol;

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_IP_PROTOCOL_H__ */
