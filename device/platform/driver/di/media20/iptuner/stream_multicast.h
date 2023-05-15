/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   stream_multicast.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	Multicast(IGMP) Stream Controller Header
 * Module:
 * Remarks:
 */

/**
 * @ingroup MULTICAST_STREAM_CONTROLLER
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
 * @file stream_multicast.h
 */

#ifndef __STREAM_MULTICAST_H__
#define __STREAM_MULTICAST_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <unistd.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <pthread.h>
#include <sys/time.h>

#include "htype.h"
//#include "di_media20.h"
#include "di_iptuner20.h"
#include "di_iptuner_config.h"
#include "di_iptuner_common.h"
#include "stream_global.h"
#include "transport_io.h"
#include "util_url.h"
#include "util_getaddrinfo.h"
#include "util_clock.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
#define _TS_SYNC_BYTE                   (0x47)
#define _TS_PACKET_SIZE                 (188)
#define _TS_PACKET_CNT_PER_UDP          (7)
#define _UDP_PKT_SIZE                   (_TS_PACKET_SIZE * _TS_PACKET_CNT_PER_UDP) /* 1316 */
#define _UDP_HEADER_SIZE                (8)
#define _RTP_HEADER_SIZE                (12)

/*
 * IP based minimum MTU size has at least (IPv4) or at least 1280 (IPv6)
 * MSS(Maximum Segment Size) = MaxMTU - 20(IP Header) - xxx (UDP or TCP Header) - ts(if ts option is used)
 * Reference http://en.wikipedia.org/wiki/Maximum_transmission_unit
 */
#define _MTU_PKT_SIZE                   (1500) /* less than mtd size < (1504:188*8) */
#define _RTP_PKT_SIZE                   (_UDP_PKT_SIZE + _RTP_HEADER_SIZE)
#define MULTICAST_MAX_READ              (_UDP_PKT_SIZE * 160)  /* 210560: Consider playpump buffer */
#if 1
#define MULTICAST_BUFFER_SIZE           (_MTU_PKT_SIZE)
#else
#define MULTICAST_BUFFER_SIZE           (MULTICAST_MAX_READ)
#endif
#define MULTICAST_RECV_TIMEOUT_USEC     (250000*4)


typedef enum
{
	MULTICAST_STREAMING_UDP	= 0,
	MULTICAST_STREAMING_RTP,
	MULTICAST_STREAMING_AUTO,
} MULTICAST_STREAMING_TYPE ;
/* End Macro definition */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef struct MCAST_SocketEventCallback_s
{
	void (*timeoutCallback)(void *context, HUINT32 uTunerID);
} MCAST_SocketEventCallback_t;

typedef struct MCAST_RtpEventCallback_s
{
	void (*rtpErrorCallback)(void);
} MCAST_RtpEventCallback_t;

typedef struct
{
	int                      fd;
	struct virtual_socket_t  *p_vs;
	DI_IPTUNER_t             *pTuner;
	URL_t                    tUrl;
	HBOOL                    bAVStream;      /* Is this SI stream or AV stream..? */
	MULTICAST_STREAMING_TYPE eStreamingType; /* Request protocol: Auto(But, unsupported), UDP, RTP */
	HBOOL                    bRtp;           /* Detected protocol: UDP or RTP */

	/* Debug */
	HBOOL          bDbgSave;
	FILE           *fpDbgSave;
	HUINT8         *pDbgSavePath;

	unsigned char  *local;
	int            local_length;
	unsigned char  *remote;
	int            remote_length;

	/* Don't divide buffer by UDP or RTP */
	/* Don't alloc buffer; Just use static sized buffer */
	/*
	HUINT8      rtpBuffer[MULTICAST_RTP_BUFFER_SIZE];
	HUINT8      udpBuffer[MULTICAST_UDP_BUFFER_SIZE];
	*/
	HUINT8      readBuffer[MULTICAST_BUFFER_SIZE];
	size_t      iReadSize;
	HUINT32     uReadCount;
	HUINT32     uSockTimeout;
	HUINT16     uLastRtpSeq;
	HUINT8      *pRtpHeader;

	/* lock */
	HULONG      lock_read;

	/* packet receive thread */
	pthread_t   iThreadID;
	HBOOL       bThreadRunning;
	HBOOL       bThreadQuit;
	HBOOL       bThreadError;

	/* Socket/streaming statistics */
	struct timeval tvZappingStartTime; /* streaming start time when open() */
	struct timeval tvZappingEndTime;   /* streaming end time when close() */
	HUINT32        tvZappingInterval;  /* Zapping interval between start and end */

	struct timeval tvFirstRecvTime;    /* 1st stream received time */
	struct timeval tvPrevRecvTime;     /* current stream received time after recv() */

	/* Socket/streaming  error */
	HBOOL          bRecvErrorOccurred; /* streaming error is occurred until we wait socket timeout */
	HUINT32        uRecvErrorInterval; /* No streaming interval time for streaming error  */
	struct timeval tvFirstRecvErrorTime;
	struct timeval tvPrevRecvErrorTime;

	HBOOL                       bUseTimeoutCB;
	MCAST_SocketEventCallback_t cbSocketEvent;

	/* RTP error callback */
	HBOOL                       bUseRtpCB;
	MCAST_RtpEventCallback_t    cbRtpEvent;
	HUINT16                     uExpectedRtpSeq;
	HUINT32                     uRtpCount;

	/* Statistics */
	HUINT32                     uCurRtpErrorCount;
	HUINT64                     ullAccRtpErrorCount;

	/* __CONTINUOUS_CNT_CHK__: TS packet checking */
	/*
	HUINT8 	                ucTSCount;
	HUINT32	                uTSErrorCount;
	*/

	/*
	 * Channel Event Callback
	 *   - Dont define the tuning event callback
	 *   - Just use pChannelCallbacks of DI_IPTUNER_t
	 *     + DI_MEDIA_ChannelTuningEventCallback
	 *     + DI_MEDIA_ChannelNotifyEventCallback
	 */
#if 0
	DI_MEDIA_ChannelEventCallback_t *pChannelCallbacks;
#endif
} MULTICAST_ACCESS_SYS_t;
/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*!
 *	\brief Join multicast stream
 *	\param pAccess: STREAM_ACCESS_t handle
 *	\return int: DI_IPT_SUCCESS if success
 *	\see
 */
int STREAM_MULTICAST_Open(STREAM_ACCESS_t *pAccess);

/*!
 *	\brief Leave multicast stream
 *	\param pAccess: STREAM_ACCESS_t handle
 *	\return void
 *	\see
 */
void STREAM_MULTICAST_Close(STREAM_ACCESS_t *pAccess);

/*!
 *	\brief Read multicast stream
 *	\param pMulticastSys: MULTICAST_ACCESS_SYS_t handle, pBuffer: reading buffer, iBufLen: buffer size
 *	\return size_t
 *	\see
 */
size_t STREAM_MULTICAST_Read(MULTICAST_ACCESS_SYS_t *pMulticastSys, HUINT8 *pBuffer, size_t iBufLen); /* not (STREAM_ACCESS_t *) */

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __STREAM_MULTICAST_H__ */
