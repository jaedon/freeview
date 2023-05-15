/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_ip.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 7/28/09 10:28a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_ip.h $
 * 
 * 7   7/28/09 10:28a vsilyaev
 * PR55693: Add DTCP/IP context
 * 
 * PR55693/1   7/27/09 6:53p lwhite
 * PR55693: Add DTCP/IP context
 * 
 * 6   6/24/09 6:47p vsilyaev
 * PR53773: Adding DTCP/IP + SSL Support in Brutus
 * 
 * PR53773/2   6/24/09 6:24p lwhite
 * PR53773: Adding DTCP/IP + SSL Support in Brutus
 * 
 * PR53773/1   6/24/09 5:22p lwhite
 * PR53773: Adding DTCP/IP + SSL Support in Brutus
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

typedef enum bsettop_ip_security_protocol {
	bsettop_ip_security_none =0,			 /* no security */
	bsettop_ip_security_ssl,                 /* TLS/SSL */
	bsettop_ip_security_dtcp_ip,             /* DTCP IP */
	bsettop_ip_security_radea                /* Rad EA security */
} bsettop_ip_security_protocol;

typedef struct bsettop_ip_ssl_info {
    char ca_file[128];           /* CA File */
    char ca_path[128];           /* CA Path - reserved - not yet implemented */
    char client_cert_file[128];   /* Certificate file */
    char client_cert_key[128];    /* Certificate key file */
    char password[16];          /* Password */
} bsettop_ip_ssl_info;

typedef struct bsettop_ip_dtcp_ip_info {
    void *dtcp_ip_ctx;                /* DTCP/IP security context */
    unsigned ake_server_port;          /* Port number for connecting to the DTCP/IP Authentication server */
} bsettop_ip_dtcp_ip_info; 

typedef struct bsettop_ip_security {
    bsettop_ip_security_protocol security_protocol;
    struct security_info {
        bsettop_ip_ssl_info ssl;
        bsettop_ip_dtcp_ip_info dtcp_ip;
    }security_info;
} bsettop_ip_security;

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_IP_PROTOCOL_H__ */
