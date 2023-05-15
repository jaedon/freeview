/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   transport_rtp.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	JungWook Chai
 * Current Author:	$Author: juchae@humaxdigital.com $
 * Date:			$Date: Thur Oct 29 10:20:48 KST 2009 $
 * File Description:	RTP Header File
 * Module: 		
 * Remarks:	
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.							  */
/* All rights reserved.											*/
/*******************************************************************/
#ifndef __TRANSPORT_RTP_H__
#define __TRANSPORT_RTP_H__

/*
#include "htype.h"
#include "vkernel.h"
#include "util_url.h"
*/

#define TRUE 1
#define FALSE 0

#define UNUSED_PARAM(x)  ((void)x)


/***************************************************************
 * RTP
 ***************************************************************/
#define _TS_SYNC_BYTE                   (0x47)
#define _TS_PACKET_SIZE                 (188)
#define _TS_PACKET_CNT_PER_UDP          (7)
#define _UDP_HEADER_SIZE                (8)
#define _RTP_HEADER_SIZE                (12)
#define _UDP_PKT_SIZE                   (_TS_PACKET_SIZE * _TS_PACKET_CNT_PER_UDP) /* 1316 */
#define _RTP_PKT_SIZE                   (_UDP_PKT_SIZE + _RTP_HEADER_SIZE)
#define _MTU_PKT_SIZE                   (1500) /* less than mtd size < (1504:188*8) */

/*
 * IP based minimum MTU size has at least (IPv4) or at least 1280 (IPv6)
 * MSS(Maximum Segment Size) = MaxMTU - 20(IP Header) - xxx (UDP or TCP Header) - ts(if ts option is used)
 */
#define _TS_MAX_READ              (_UDP_PKT_SIZE * 160)  /* 210560: Consider playpump buffer */
#define _TS_BUFFER_SIZE           (_MTU_PKT_SIZE) /* _TS_MAX_READ */
#define _RTP_RECV_TIMEOUT        (100*1000)

/* RTP macros */
#define _RTP_VERSION             (2)     /* only support version 2 RTP header */
#define _RTP_NEXT_SEQNUM         (1)     
#define _RTP_PAYLOAD_TYPE_MP2T   (33)    /* only support PT=33 */

void net_RTPTuneSocket(int fd);
int net_RTPConnect(char *pBindHost, int iBindPort);
int net_ConnectRTP(char *pBindHost, int iBindPort);
int net_RTPReadChannel(struct RTSP_CHANNEL_t *pRtpChannel, HUINT8 *pBuffer, size_t i_request);

#endif /* __TRANSPORT_RTP_H__ */


