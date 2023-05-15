/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   mms_tcpudp.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	DI Stream (MMS) Header
 * Module:
 * Remarks:
 */	

/**
 * @ingroup MMS_TCPUDP_CONTROLLER
 */

/**
 * @author Jin Lee(jlee@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file mms_tcpudp.h
 */

#ifndef __MMS_TCPUDP_H__
#define __MMS_TCPUDP_H__ 

#include "htype.h"
#include "util_url.h"
#include "util_getaddrinfo.h"
#include "mms_asf.h"
#include "stream_global.h"
#include "di_iptuner20.h"

#include <pthread.h>

#define MMS_PROTO_AUTO 0
#define MMS_PROTO_TCP 1
#define MMS_PROTO_UDP 2

#define MMS_PACKET_ANY          0
#define MMS_PACKET_CMD          1
#define MMS_PACKET_HEADER       2
#define MMS_PACKET_MEDIA        3
#define MMS_PACKET_UDP_TIMING   4

#define MMS_TCPUDP_CMD_HEADERSIZE  48

#define MMS_BUFFER_SIZE 1000000

typedef struct mmstu_keepalive_thread_t mmstu_keepalive_thread_t;

typedef struct {
	int         i_proto;        /* MMS_PROTO_TCP, MMS_PROTO_UDP */
	int         i_handle_tcp;   /* TCP socket for communication with server */
	int         i_handle_udp;   /* Optional UDP socket for data(media/header packet) */
	/* send by server */
	char        sz_bind_addr [NI_MAXNUMERICHOST]; /* used by udp */
	URL_t   	url;
	ASF_HEADER_t    asfh;

	unsigned    i_timeout;

	/* */
	HUINT8     buffer_tcp[MMS_BUFFER_SIZE];
	int        i_buffer_tcp;

	HUINT8     buffer_udp[MMS_BUFFER_SIZE];
	int        i_buffer_udp;

	/* data necessary to send data to server */
	ASF_GUID_t      guid;
	int        iCommandLevel;
	int        iSequenceNum;
	HUINT32    iHeaderPktIdType;
	HUINT32    iMediaPktIdType;

	int         iPacketSeqNum;

	HUINT8     *pCommand;     /* latest command read */
	size_t      iCommandSize;      /* allocated at the begining */

	HUINT8     *pHeader;  /* allocated by mms_ReadPacket */
	size_t      iHeaderSize;

	HUINT8     *pMedia;   /* allocated by mms_ReadPacket */
	size_t      iMediaSize;
	size_t      iMediaUsedSize;

	/* extracted information */
	int         iCommandType; /* 0x03, 0x1e, 0x05, 0x20 */

	/* from 0x01 answer (not yet set) */
	char        *pServerVer;
	char        *pToolVer;
	char        *pUpdatePlayerURL;
	char        *pEncryptionType;

	/* from 0x06 answer */
	HUINT32		i_flags_broadcast;
	HUINT32		i_media_length;		/** secs */
	size_t		i_packet_length;	/** maxmum packet length (bytes)*/
	HUINT32		i_packet_count;		/** the number of ASF Data Packets in the content */
	int			i_max_bit_rate;		/** sum of the maximum bit rates of all streams in the current playlist entry(in bits per secs) */
	size_t		i_header_size;

	/* misc */
	HBOOL  		b_seekable;

	/* seek related */
	HBOOL		bDuringSeeking;
	HBOOL		bFeedASFHeaderFirst;
	HUINT8		*pBufferingBuffer;
	HINT32		nBufferingBufferLength;
	HINT32		nBufferingBufferDataStart;
	HINT32		nBufferingBufferDataEnd;
	

	mmstu_keepalive_thread_t *p_keepalive_thread;

	HULONG lock_netwrite;
	HULONG lock_readapacket;
} MMSTU_ACCESS_SYS_t;

struct mmstu_keepalive_thread_t
{
    STREAM_ACCESS_t *pAccess;
	pthread_t thread_id;
	HBOOL b_quit;
    HBOOL b_paused;
    HBOOL b_thread_error;
};

int  MMSTU_Open( STREAM_ACCESS_t *pAccess );
void  MMSTU_Close  ( STREAM_ACCESS_t * );
ssize_t MMSTU_Read( STREAM_ACCESS_t *, HUINT8 *, size_t );
int MMSTU_Seek( STREAM_ACCESS_t *, HINT64 );
int MMSTU_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val );

#endif /* __MMS_TCPUDP_H__ */
