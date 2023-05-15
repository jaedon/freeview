/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   stream_multicast.c  $
 * Version:		2.0 (for Media 2.0 with 40nm)
 * Original Author:	Jin Lee
 * Current Author:	$Author: jlee@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     MULTICAST - STREAM Controller
 * Module:
 * Remarks:
 */

/**
 * @defgroup MULTICAST_STREAM_CONTROLLER
 * @brief MULTICAST STREAM Controller.
 */

/**
 * @author Jin Lee (jlee@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @brief MULTICAST STREAM Controller.
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
 * @file stream_multicast.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <sys/time.h>
#include "vkernel.h"
#include "taskdef.h"

#include "htype.h"
#include "di_uart.h"
#include "vkernel.h"
#include "di_iptuner_config.h"
#include "di_iptuner_common.h"
#include "stream_global.h"
#include "stream_multicast.h"
#include "transport_udp.h"
/*#include "transport_rtp.h"*/
#include "util_url.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define STREAM_MULTICAST_DEBUG
#define STREAM_MULTICAST_CRITICAL_DEBUG

//#ifdef IPTUNER_ALL_DEBUG
#if 1
	#ifdef STREAM_MULTICAST_DEBUG
	#define PrintData		DI_UART_Print
	#define PrintDebug		DI_UART_Print
	#define PrintError		DI_UART_Print
	#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
	#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
	#else
	#define PrintData		while (0) ((int (*)(char *, ...)) 0)
	#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
	#define PrintEnter
	#define PrintExit
	#define PrintError		DI_UART_Print
	#endif
#else
	#define PrintData		while (0) ((int (*)(char *, ...)) 0)
	#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
	#define PrintError		while (0) ((int (*)(char *, ...)) 0)
	#define PrintEnter
	#define PrintExit
#endif /* The END of DEBUG DEFINE */

#ifdef STREAM_MULTICAST_CRITICAL_DEBUG
#define PrintCriticalDebug       DI_UART_Print
#else
#define PrintCriticalDebug       while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)          ((void)x)

/* RTP macros */
#define _RTP_VERSION             (2)     /* only support version 2 RTP header */
#define _RTP_NEXT_SEQNUM         (1)
#define _RTP_PAYLOAD_TYPE_MP2T   (33)    /* only support PT=33 */

/* Take brtp routines */
#define _RTP_LOAD8(ptr, off)     ((((HUINT8 *)(ptr))[(off)+0]))
#define _RTP_LOAD16(ptr, off)    (((HUINT16)((HUINT8 *)(ptr))[(off)+0]<<8) | ((HUINT16)((HUINT8 *)(ptr))[(off)+1]))
#define _RTP_LOAD32(ptr, off)    (((HUINT32)((HUINT8 *)(ptr))[(off)+0]<<24) | ((HUINT32)((HUINT8 *)(ptr))[(off)+1]<<16) | ((HUINT32)((HUINT8 *)(ptr))[(off)+2]<<8) | ((HUINT32)((HUINT8 *)(ptr))[(off)+3]))
#define _RTP_GET_BIT(w,b)        ((w)&(1<<((sizeof(w)*8-1)-(b))))
#define _RTP_GET_BITS(w,f,l)     (((w)>>((sizeof(w)*8-1)-(l)))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(l+1-f))))
#define _RTP_TIMESTAMP(pkt)      _RTP_LOAD32(pkt,4)
#define _RTP_PKT_SEQUENCE(pkt)   _RTP_LOAD16((pkt)->data,2)
#define _RTP_PT(pkt)             _RTP_GET_BITS(_RTP_LOAD32(pkt,0), 9, 15)
#define _RTP_PKT_TIMESTAMP(pkt)  _RTP_TIMESTAMP((pkt)->data)
#define _RTP_PKT_MARKER(pkt)     _RTP_GET_BIT(_RTP_LOAD8((pkt)->data,1), (8-8))
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef struct MCAST_RtpHeader_s
{
	HUINT8  csrc_count;   /* 16:4, CSRC count            */
	HBOOL   extension;    /* 16:1, header extension flag */
	HUINT8  padding;      /* 16:1, padding flag          */
	HUINT8  version;      /* 16:2, packet type           */
	HUINT8  payload_type; /* 16:7, payload type          */
	HUINT8  marker;       /* 16:1, marker bit            */
	HUINT16 sequence_num; /* 16, sequence number         */
	HUINT32 timestamp;    /* 32, timestamp               */
	HUINT32 ssrc;         /* 32, synchronization source  */

	struct {
		HUINT16 defined_by_profile;
		HUINT16 length;
	} extension_header;
} MCAST_RtpHeader_t;

typedef struct MCAST_RtpPacket_s
{
	HUINT8 header[_RTP_HEADER_SIZE];
	HUINT8 payload[_UDP_PKT_SIZE];
} MCAST_RtpPacket_t;

typedef struct MCAST_UdpPacket_s
{
	HUINT8 payload[_UDP_PKT_SIZE];
} MCAST_UdpPacket_t;
/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
/* Start static function prototype */
static int INT_MULTICAST_JoinIGMP(MULTICAST_ACCESS_SYS_t *pMulticastSys, URL_t *ptUrl, MULTICAST_STREAMING_TYPE eStreamingType);
static void INT_MULTICAST_LeaveIGMP(MULTICAST_ACCESS_SYS_t *pMulticastSys, URL_t *ptUrl);
/* End static function prototype */


static int INT_MULTICAST_JoinIGMP(MULTICAST_ACCESS_SYS_t *pMulticastSys, URL_t *ptUrl, MULTICAST_STREAMING_TYPE eStreamingType)
{
	int isUDP = 0;

	PrintEnter;

	if (pMulticastSys == NULL)
	{
		PrintError("%s(%d) Error> Invalid pMulticastSys arg!\n",__FUNCTION__,__LINE__ );
		return DI_IPT_EGENERIC;
	}

	if (ptUrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid tURL arg!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if (eStreamingType == MULTICAST_STREAMING_AUTO)
	{
		PrintError("%s(%d) Error> Multicast protocol auto detection is unsupported!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	isUDP = ( eStreamingType == MULTICAST_STREAMING_UDP ) ? 1 : 0;

	/* Later, this will be merged into the net_Subscribe () function */
	/* And net_Unsubscribe () will be created if implicity IGMP leave is needed, not just socket close */
	pMulticastSys->fd = net_JoinIGMP(ptUrl->psz_host, ptUrl->i_port);
	if (pMulticastSys->fd < 0)
	{
		PrintError("%s(%d) Error> failed to open a connection (dup)\n",__FUNCTION__,__LINE__ );
		return DI_IPT_EGENERIC;
	}
	PrintDebug("%s(%d) IGMP join to \"%s:%d\"\n",__FUNCTION__,__LINE__, ptUrl->psz_host, ptUrl->i_port );

	pMulticastSys->eStreamingType = eStreamingType;

#if 0
	pMulticastSys->pReadBuffer = (HUINT8*)DD_MEM_Alloc(MULTICAST_BUFFER_SIZE);
	if(pMulticastSys->pReadBuffer==NULL)
	{
		PrintError("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,MULTICAST_BUFFER_SIZE);
		return DI_IPT_ENOMEM;
	}
#else
	VK_memset(pMulticastSys->readBuffer, 0, MULTICAST_BUFFER_SIZE);
#endif
	pMulticastSys->iReadSize = MULTICAST_BUFFER_SIZE;
	pMulticastSys->uReadCount = 0;

	PrintData("%s(%d) connection successful\n",__FUNCTION__,__LINE__ );
	PrintExit;
	return DI_IPT_SUCCESS;

func_error:
	PrintData("%s(%d) Error! Failed\n",__FUNCTION__,__LINE__ );
	/*
	if (pMulticastSys == NULL)
	{
		INT_MULTICAST_LeaveIGMP(pMulticastSys);
	}
	*/
	return DI_IPT_EGENERIC;
}

static void INT_MULTICAST_LeaveIGMP(MULTICAST_ACCESS_SYS_t *pMulticastSys, URL_t *ptUrl)
{
	PrintEnter;

	if (pMulticastSys == NULL)
	{
		PrintError("%s(%d) Error> Invalid pMulticastSys arg!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if (ptUrl == NULL)
	{
		PrintError("%s(%d) Error> Invalid tURL arg!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if (pMulticastSys->fd < 0)
	{
		PrintError("%s(%d) Error> Invalid fd(%d)!\n", __FUNCTION__, __LINE__, pMulticastSys->fd);
		goto func_error;
	}
	(void)net_LeaveIGMP(pMulticastSys->fd, ptUrl->psz_host);

	/* release or reinitialize multicast stream handle */
#if 0
	if (pMulticastSys->pReadBuffer != NULL)
	{
		DD_MEM_Free(pMulticastSys->pReadBuffer);
		pMulticastSys->pReadBuffer = NULLL;
	}
#else
	VK_memset(pMulticastSys->readBuffer, 0, MULTICAST_BUFFER_SIZE);
#endif

	PrintExit;
	return;

func_error:
	PrintExit;
	return;
}

static void INT_MULTICAST_PrintRtpHeader(MCAST_RtpHeader_t *pRtpHeader)
{
	if (pRtpHeader)
	{
		PrintDebug("rtp_hdr.version      : %d\n", (int)pRtpHeader->version);
		PrintDebug("rtp_hdr.padding      : %d\n", (int)pRtpHeader->padding);
		PrintDebug("rtp_hdr.extension    : %d\n", (int)pRtpHeader->extension);
		PrintDebug("rtp_hdr.csrc_count   : %d\n", (int)pRtpHeader->csrc_count);
		PrintDebug("rtp_hdr.marker       : %d\n", (int)pRtpHeader->marker);
		PrintDebug("rtp_hdr.payload_type : %d\n", (int)pRtpHeader->payload_type);
		PrintDebug("rtp_hdr.seq_num      : %u\n", pRtpHeader->sequence_num);
		PrintDebug("rtp_hdr.ts           : %u\n", pRtpHeader->timestamp);
		PrintDebug("rtp_hdr.ssrc         : %u\n", pRtpHeader->ssrc);
	}

	/*
	 * RTP#(N-1) header
	 *   + version      : 2
	 *   + padding      : 0
	 *   + extension    : 0
	 *   + csrc_count   : 0
	 *   + marker       : 1
	 *   + payload_type : 33
	 *   + seq_num      : 51437
	 *   + ts           : 1118366346
	 *   + ssrc         : 2998055602
	 *
	 * RTP#(N) header
	 *   + version      : 2
	 *   + padding      : 0
	 *   + extension    : 0
	 *   + csrc_count   : 0
	 *   + marker       : 1
	 *   + payload_type : 33
	 *   + seq_num      : 51438
	 *   + ts           : 1118366404
	 *   + ssrc         : 2998055602
	 *
	 * RTP#(N+1) header
	 *   + version      : 2
	 *   + padding      : 0
	 *   + extension    : 0
	 *   + csrc_count   : 0
	 *   + marker       : 1
	 *   + payload_type : 33
	 *   + seq_num      : 51439
	 *   + ts           : 1118366462
	 *   + ssrc         : 2998055602
	 */
	return;
}

static void INT_MULTICAST_ParseRtpHeader(HUINT8 *pBuf)
{
	MCAST_RtpHeader_t rtpHeader;

	/*
	PrintDebug("0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
			  pBuf[0],pBuf[1],pBuf[2],pBuf[3],
			  pBuf[4],pBuf[5],pBuf[6],pBuf[7],
			  pBuf[8],pBuf[9],pBuf[10],pBuf[11]);
	*/

	/* Take brtp routines */
	rtpHeader.version    = (pBuf[0] & 0xC0) >> 6;
	rtpHeader.padding    = (pBuf[0] & 0x20) >> 5;
	rtpHeader.extension  = (pBuf[0] & 0x10) >> 4;
	rtpHeader.csrc_count = (pBuf[0] & 0x0F) >> 0;

	rtpHeader.marker       = (pBuf[1] & 0x80) >> 7;
	rtpHeader.payload_type = (pBuf[1] & 0x7F) >> 0;

	rtpHeader.sequence_num = (pBuf[2] << 8);
	rtpHeader.sequence_num |= pBuf[3];

	rtpHeader.timestamp = (pBuf[4] << 24);
	rtpHeader.timestamp |= (pBuf[5] << 16);
	rtpHeader.timestamp |= (pBuf[6] << 8);
	rtpHeader.timestamp |= (pBuf[7] << 0);

	rtpHeader.ssrc = (pBuf[8] << 24);
	rtpHeader.ssrc |= (pBuf[8] << 16);
	rtpHeader.ssrc |= (pBuf[8] << 8);
	rtpHeader.ssrc |= (pBuf[8] << 0);

	if(rtpHeader.version != _RTP_VERSION) {
		PrintError("%s(%d) Error> RTP version not supported!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if(rtpHeader.padding) {
		PrintError("%s(%d) Error> RTP padding not supported!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if(rtpHeader.extension) {
		PrintError("%s(%d) Error> RTP header extensions not supported!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if(rtpHeader.csrc_count) {
		PrintError("%s(%d) Error> RTP non-zero csrc_count not supported!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if(rtpHeader.payload_type != _RTP_PAYLOAD_TYPE_MP2T) {
		PrintError("%s(%d) Error> Unsupported RTP payload type!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	INT_MULTICAST_PrintRtpHeader(&rtpHeader);
	return;

func_error:
	PrintError("%s(%d) Error> RTP header failed\n",__FUNCTION__,__LINE__ );
	return;
}

/*****************************************************************************
 * STREAM_MULTICAST_Open:
 *   - Initialize pAccess
 *   - Allocate/Initialize pMulticastSys handle
 *   - IGMP join with pMulticastSys, not pAccess
 *****************************************************************************/
int STREAM_MULTICAST_Open(STREAM_ACCESS_t *pAccess)
{
	HINT32 nRet = DI_IPT_EGENERIC;
	HBOOL bAdjustSocketBuffer = FALSE;
	MULTICAST_ACCESS_SYS_t   *pMulticastSys;
	MULTICAST_STREAMING_TYPE eStreamingType;
	int i_status;
	int new_rmem_max = 0; /* _LINUX_PROC_RMEM_MAX */

	PrintEnter;

	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> invalid stream access handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	PrintDebug ("%s(%d) pAccess (%d) psz_path(%s)\n"
				,__FUNCTION__
				,__LINE__
				, (HUINT32) pAccess
				, pAccess->psz_path);

	/** Parse URI - remove spaces */
	if (pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) Error> invalid pAccess URL(NULL)\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}
	pAccess->b_alive = TRUE;
	pAccess->proto = PROTOCOL_UNKNOWN;

	/* Setup pAccess handle */
	/* Following almost vars has no meaning
	   at boundless or infinite streaming like multicast */
	pAccess->info.i_update    = 0;	     /* Has no meaning at mcast */
	pAccess->info.i_size      = -1;	     /* Current received/read size */
	pAccess->info.i_pos       = 0;	     /* Current/start pos, but has no meaning at mcast */
	pAccess->info.b_eof       = FALSE;   /* eof, but has b_eof meaning at mcast */
	pAccess->info.i_title     = 0;	     /* Has no meaning at mcast */
	pAccess->info.i_seekpoint = 0;       /* Has no meaning at mcast */

	/* Allocate pMulticastSys handle */
	pAccess->p_sys = pMulticastSys = (MULTICAST_ACCESS_SYS_t *)DD_MEM_Alloc ( sizeof( MULTICAST_ACCESS_SYS_t ) );
	if (pMulticastSys == NULL)
	{
		PrintError("%s(%d) Error> invalid stream access handle (NULL)\n",__FUNCTION__,__LINE__ );
		nRet = DI_IPT_ENOMEM;
		goto func_error;
	}
	VK_memset(pMulticastSys, 0, sizeof(MULTICAST_ACCESS_SYS_t));

	/* Initialize pMulticastSys */
	pMulticastSys->fd             = (-1);
	pMulticastSys->bRtp           = FALSE;
	pMulticastSys->p_vs           = NULL;    /* Dont use virtual socket interface */
	pMulticastSys->uSockTimeout   = MULTICAST_RECV_TIMEOUT_USEC;    /* socket uStreamInterval: 1(sec) */
	pMulticastSys->bAVStream      = TRUE;
	pMulticastSys->uLastRtpSeq    = 0x0;
	pMulticastSys->pRtpHeader     = NULL;    /* MCAST_RtpHeader_t */

	/* receive thread */
	pMulticastSys->iThreadID      = 0x0;
	pMulticastSys->bThreadRunning = FALSE;
	pMulticastSys->bThreadQuit    = FALSE;
	pMulticastSys->bThreadError   = FALSE;

	/* socket/RTP statistics or error */
	VK_memset(&pMulticastSys->tvZappingStartTime, 0, sizeof(pMulticastSys->tvZappingStartTime));
	VK_memset(&pMulticastSys->tvZappingEndTime, 0, sizeof(pMulticastSys->tvZappingEndTime));
	pMulticastSys->tvZappingInterval  = 0;

	VK_memset(&pMulticastSys->tvFirstRecvTime, 0, sizeof(pMulticastSys->tvFirstRecvTime));
	VK_memset(&pMulticastSys->tvPrevRecvTime, 0, sizeof(pMulticastSys->tvPrevRecvTime));
#if 0
	VK_memset(&pMulticastSys->tvCurrRecvTime, 0, sizeof(pMulticastSys->tvCurrRecvTime));
#endif

	pMulticastSys->bRecvErrorOccurred = FALSE;
	pMulticastSys->uRecvErrorInterval = 0;
	VK_memset(&pMulticastSys->tvFirstRecvErrorTime, 0, sizeof(pMulticastSys->tvFirstRecvErrorTime));
	VK_memset(&pMulticastSys->tvPrevRecvErrorTime, 0, sizeof(pMulticastSys->tvPrevRecvErrorTime));
#if 0
	VK_memset(&pMulticastSys->tvCurrRecvErrorTime, 0, sizeof(pMulticastSys->tvCurrRecvErrorTime));
#endif

	pMulticastSys->bUseTimeoutCB  = FALSE;
	pMulticastSys->bUseRtpCB      = FALSE;
	pMulticastSys->uExpectedRtpSeq  = 0x0;
	pMulticastSys->uRtpCount      = 0x0;

	/* Debug */
	pMulticastSys->bDbgSave     = FALSE;
	pMulticastSys->fpDbgSave    = NULL;
	pMulticastSys->pDbgSavePath = NULL;

	/* Record open() time */
	UTIL_GetCurrentTimeval(&pMulticastSys->tvZappingStartTime);


	//nVKRet = VK_SEM_Create (&pMulticastSys->lock_read , "MULTICASTReadSema", VK_SUSPENDTYPE_PRIORITY);
	//if ( VK_OK != nVKRet )
	//{
	//	PrintError ("%s (%d) Error> SEMA (lock_read) creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
	//	nRet = DI_IPT_EGENERIC;
	//	goto func_error;
	//}

	/* Parse URL and get server addr/port and path */
	UTIL_UrlParse(&pMulticastSys->tUrl, pAccess->psz_path, 0);
	if ((pMulticastSys->tUrl.psz_host == NULL)
		|| (*pMulticastSys->tUrl.psz_host == '\0')
		|| (pMulticastSys->tUrl.i_port <= 0)
		|| (pMulticastSys->tUrl.i_port > 65535))
	{
		PrintError("%s(%d) Error> invalid server name or port num(%d)\n",__FUNCTION__,__LINE__, pMulticastSys->tUrl.i_port);
		UTIL_UrlClean( &pMulticastSys->tUrl );
		//VK_SEM_Destroy (pMulticastSys->lock_read);
		DD_MEM_Free ( pMulticastSys );
		nRet = DI_IPT_EGENERIC;
		goto func_error;
	}

	if (!VK_strncmp(pMulticastSys->tUrl.psz_protocol,"udp",3))
	{
		eStreamingType = MULTICAST_STREAMING_UDP;
		pAccess->proto = PROTOCOL_UDP; // PROTOCOL_UDP_MCAST;
		pMulticastSys->bRtp = FALSE;
	}
	else if (!VK_strncmp(pMulticastSys->tUrl.psz_protocol,"rtp",3))
	{
		eStreamingType = MULTICAST_STREAMING_RTP;
		pAccess->proto = PROTOCOL_RTP; // PROTOCOL_RTP_MCAST;
		pMulticastSys->bRtp = TRUE;
	}
	else
	{
		/* Can't support streaming auto detection or undefined URL */
		eStreamingType = MULTICAST_STREAMING_AUTO;
		pAccess->proto = PROTOCOL_UNKNOWN;
	}

	if (eStreamingType == MULTICAST_STREAMING_AUTO)
	{
		PrintError ("%s (%d) Error> Currently, protocol auto detection(RTP/UDP) is unsupported\n", __FUNCTION__, __LINE__);
		nRet = DI_IPT_EGENERIC;
		goto func_error;
	}

	if (bAdjustSocketBuffer == TRUE)
	{
		/*
		 * dma: for Linux kernel tuning.
		 * Linux kernel set socket receive buffer to 64K by default.
		 * It works with low bit rate stream up to ~14Mbps.
		 * With higher bit rate (19 or above)Mbps,
		 * Linux starts dropping UDP packets.
		 * This has been verified by the following
		 * /proc/net/snmp and /proc/net/udp
		 *
		 * We need to increase our UDP socket receive buffer size by using setsockopt().  Therefore,
		 * /proc/sys/net/core/rmem_max needs to be changed.
		 *
		 * NOTE: the following will set rmem_max to 200K if it was 64K.
		 */
		PrintDebug("%s(%d) --> Adjust /proc/rmem_max\n", __FUNCTION__, __LINE__);
		new_rmem_max = net_SetMulticastRMEM();
	}

	/* IGMP Join */
	PrintDebug("%s(%d) --> +IGMP Join(%s:%d:%d)\n",
				__FUNCTION__, __LINE__,
				pMulticastSys->tUrl.psz_host,
				pMulticastSys->tUrl.i_port,
				eStreamingType);
	i_status = INT_MULTICAST_JoinIGMP(pMulticastSys, &pMulticastSys->tUrl, eStreamingType );
	if (i_status != 0)
	{
		PrintError("%s(%d) Error> cannot connect to server\n",__FUNCTION__,__LINE__ );
		UTIL_UrlClean( &pMulticastSys->tUrl );
		//VK_SEM_Destroy (pMulticastSys->lock_read);
		DD_MEM_Free ( pMulticastSys );
		return DI_IPT_EGENERIC;
	}
	PrintDebug("%s(%d) --> -IGMP Join\n",__FUNCTION__,__LINE__);

	if ((bAdjustSocketBuffer == TRUE) && (new_rmem_max))
	{
		net_SetMulticastBuffer(pMulticastSys->fd, new_rmem_max);
	}

	/* Streaming thread will be located at ip_tuner_multicast */
	/* Here, just do IGMP joining */

	PrintExit;
	return DI_IPT_SUCCESS;

func_error:
	if (pAccess)
	{
		pAccess->b_alive = FALSE;
	}
	/*
	STREAM_MULTICAST_Close(pAccess);
	*/
	PrintExit;
	return nRet;
}

/*****************************************************************************
 * STREAM_MULTICAST_Close:
 *   - Initialize pAccess handle if it is needed
 *   - IGMP leave with pMulticastSys, not pAccess
 *   - Deallocate pMulticastSys handle
 *****************************************************************************/
void STREAM_MULTICAST_Close(STREAM_ACCESS_t *pAccess)
{
	MULTICAST_ACCESS_SYS_t *pMulticastSys = NULL;
	HLONG timediff = 0;

	PrintEnter;

	if (pAccess == NULL)
	{
		PrintError("%s(%d) Error> invalid stream access handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return;
	}
	pAccess->b_alive = FALSE;

	pMulticastSys = (MULTICAST_ACCESS_SYS_t *)pAccess->p_sys;
	if (pMulticastSys == NULL)
	{
		PrintError("%s(%d) Error> invalid multicastSys handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return;
	}

	(void)INT_MULTICAST_LeaveIGMP(pMulticastSys, &pMulticastSys->tUrl);
	UTIL_UrlClean(&pMulticastSys->tUrl);
	//VK_SEM_Destroy (pMulticastSys->lock_read);

	UTIL_GetCurrentTimeval(&pMulticastSys->tvZappingEndTime);
	timediff = UTIL_GetTimevalDiff(&pMulticastSys->tvZappingEndTime, &pMulticastSys->tvZappingStartTime);
	PrintError("%s(%d) [zapping time]> %lu (msec)\n",__FUNCTION__,__LINE__, timediff);

	/* free memory */
	if (pMulticastSys != NULL)
	{
		DD_MEM_Free(pMulticastSys);
		pAccess->p_sys = NULL;
	}

	PrintDebug ("%s(%d) pAccess (%d) \n",__FUNCTION__,__LINE__, (HUINT32) pAccess);
	PrintExit;
	return;
}

/*****************************************************************************
 * STREAM_MULTICAST_Read:
 *   - Receive UDP/RTP packet w/ waiting mode(until the timeout)
 *   - Extract payload if RTP streaming is used
 *   - Copy TS payload to caller's buffer
 *****************************************************************************/
size_t STREAM_MULTICAST_Read(MULTICAST_ACCESS_SYS_t *pMulticastSys, HUINT8 *pBuffer, size_t iReqLen) /* (STREAM_ACCESS_t *), timeout */
{
	size_t iPktSize = 0;
	HINT32 nAdjustStartPosition = 0;
	/* buffer has size of >= [_MTU_PKT_SIZE] */
	HUINT8 pktBuffer[_MTU_PKT_SIZE];
#ifdef __MULTICAST_EXTRACT_PAYLOAD_ONLY__ /*(0)*/
	/* copy payload only except RTP header; overhead action */
	HUINT8 payloadBuffer[_UDP_PKT_SIZE];
#endif
	HUINT8 *ptr = NULL; /* For getting payload only, skipping RTP header */
	HUINT32 uTotalBytes = 0;
	HUINT32 uPrevTotalBytes = 0;
	HINT32 iRecvBytes = 0;
	HUINT32	uLoopCnt = 0;
	HUINT32	uMaxLoopCnt = 5;
	HUINT16 uCurRtpSeq = 0;
	HBOOL bDbgParseRTP = TRUE;
	HBOOL bBreakByLooping = TRUE;
	/*HBOOL bBreakByTimeout = FALSE;*/

	/* Dont wait stream error; This will be done at di_ipt_mcast */
	/* We have to avoid blocking time at streaming level */
	HBOOL bWaitForStreamError = FALSE;

	/* timeout checking for stream error will be done at di_ipt_mcast */
	HBOOL bDbgCheckTimeout = FALSE;
	struct timeval tvStartTime  = {0,0};
	struct timeval tvCurrentTime  = {0,0};
	HUINT32 uStreamInterval = 0; /* normal interval between prev recv() and current recv() */
	HUINT32 uErrorInterval = 0; /* stream error interval time (usec) */

	PrintEnter;

	UTIL_GetCurrentTimeval(&tvStartTime);
	PrintDebug("%s(%d) Start time (absolute) is %ld seconds\n", __FUNCTION__, __LINE__, tvStartTime.tv_sec);

	if (pMulticastSys == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL pMulticastSys!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	if (pMulticastSys->fd < 0)
	{
		PrintError("%s(%d) Error> Invalid fd(%d)!\n", __FUNCTION__, __LINE__, pMulticastSys->fd);
		goto func_error;
	}

	if (MULTICAST_STREAMING_RTP == pMulticastSys->eStreamingType)
	{
		iPktSize = _RTP_PKT_SIZE;
		nAdjustStartPosition = _RTP_HEADER_SIZE;
	}
	else
	{
		iPktSize = _UDP_PKT_SIZE;
		nAdjustStartPosition = 0;
	}

	/* Read socket until given buffer is full */
	/* Socket timeout value have 1 (sec) */
	for(;;)
	{
		if (pMulticastSys->bThreadRunning == FALSE)
		{
			PrintDebug("%s(%d) Terminate(tot:%u, prev:%u, reqlen:%u)!\n", __FUNCTION__, __LINE__,
						uTotalBytes, uPrevTotalBytes, iReqLen);
			goto func_done;
		}

		uLoopCnt++;
		if (bBreakByLooping == TRUE)
		{
			if ((uPrevTotalBytes == uTotalBytes)
				&& uMaxLoopCnt
				&& (uLoopCnt > uMaxLoopCnt))
			{
				PrintError("%s(%d) Error> Too many looping(loop:%d > max:%d)!\n",
							__FUNCTION__, __LINE__,
							uLoopCnt, uMaxLoopCnt);
				goto func_error;
			}
		}

		uPrevTotalBytes = uTotalBytes;
		PrintDebug("%s(%d) Received Bytes(tot:%u, prev:%u, reqlen:%u)!\n",
					__FUNCTION__, __LINE__,
					uTotalBytes, uPrevTotalBytes, iReqLen);

		/* The request length must have 1500 or above value */
		if ((uTotalBytes+_UDP_PKT_SIZE) >= iReqLen)
		{
			PrintDebug("%s(%d) Read stream successfully(tot:%u, prev:%u, reqlen:%u)!\n",
						__FUNCTION__, __LINE__,
						uTotalBytes, uPrevTotalBytes, iReqLen);
			goto func_done;
		}

		if (pMulticastSys->fd < 0)
		{
			PrintError("%s(%d) Error> Invalid fd(%d)!\n", __FUNCTION__, __LINE__, pMulticastSys->fd);
			goto func_error;
		}

		VK_memset(pktBuffer, 0, _MTU_PKT_SIZE);
		ptr = pktBuffer;

		/* Record recv() time */
		UTIL_GetCurrentTimeval(&tvCurrentTime);

		/*
		 * Receive payload; extract RTP header if RTP streaming
		 * ssize_t net_Recvfrom(int nfd, const struct virtual_socket_t *vs,
		 *                      HUINT8 *restrict p_buf, size_t i_buflen,
		 *                      HBOOL waitall)
		 */
		PrintDebug("%s(%d) --> +net_Recvfrom(pktlen:%d)\n", __FUNCTION__, __LINE__, iPktSize);
		iRecvBytes = (HINT32)net_Recvfrom(pMulticastSys->fd,
		                                  /* NULL, */
		                                  (HUINT8 *)pktBuffer,
		                                  iPktSize,
		                                  TRUE,
		                                  pMulticastSys->uSockTimeout);
		PrintDebug("%s(%d) --> -net_Recvfrom(recv:%d)\n", __FUNCTION__, __LINE__, iRecvBytes);

		if (iRecvBytes <= 0)
		{
			if (bDbgCheckTimeout == TRUE)
			{
				if (pMulticastSys->tvPrevRecvErrorTime.tv_usec == 0)
				{
					/* first time */
					pMulticastSys->tvFirstRecvErrorTime = tvCurrentTime;
					pMulticastSys->tvPrevRecvErrorTime = tvCurrentTime;
				}
				else
				{
					uErrorInterval = UTIL_GetTimevalDiff(&tvCurrentTime, &pMulticastSys->tvFirstRecvErrorTime);
					if (uErrorInterval >= 3000)
					{
						if (pMulticastSys->bAVStream == TRUE)
						{
							pMulticastSys->bRecvErrorOccurred = TRUE;
						}
					}
					pMulticastSys->tvPrevRecvErrorTime = tvCurrentTime;
				}
			}

			if (iRecvBytes == 0)
			{
				PrintError("%s(%d) Error> eof(fd:%d) --> retry\n", __FUNCTION__, __LINE__, pMulticastSys->fd);

				/*
				 * We dont wait anymore; we already wait socket level timeout
				 * (The socket timeout, 1(sec) is enough time)
				 * --> Decision for waiting will be done at di_ipt_mcast
				 */
				if (bWaitForStreamError == TRUE)
				{
					/* Retry again;
					   Is enough or too large sleep time..? */
					VK_TASK_Sleep(10);
					continue;
				}
				else
				{
					goto func_error;
				}
			}
			else if (iRecvBytes < 0)
			{
				/* Errno processing for EINTR or EAGAIN, ...
				   will be done at socket level, not streaming level */
				PrintError("%s(%d) Error> errno(fd:%d)\n", __FUNCTION__, __LINE__, pMulticastSys->fd);

				/*
				 * We dont wait anymore; we already wait socket level timeout
				 * (The socket timeout, 1(sec) is enough time)
				 * --> Decision for waiting will be done at di_ipt_mcast
				 */
				if (bWaitForStreamError == TRUE)
				{
					/* Retry again;
					   Is enough or too large sleep time..? */
					VK_TASK_Sleep(10);
					continue;
				}
				else
				{
					goto func_error;
				}
			}
		}
		else
		{
			if (pMulticastSys->uReadCount > 0xFFFFFF00)
			{
				pMulticastSys->uReadCount = 0;
			}
			pMulticastSys->uReadCount++; /* buffer read count, diff with loop count */

			if (bDbgCheckTimeout == TRUE)
			{
				pMulticastSys->tvPrevRecvErrorTime.tv_usec = 0;
				pMulticastSys->tvFirstRecvErrorTime.tv_usec = 0;
				pMulticastSys->bRecvErrorOccurred = FALSE;

				UTIL_GetCurrentTimeval(&tvCurrentTime);
				uStreamInterval = UTIL_GetTimevalDiff(&tvCurrentTime, &tvStartTime);
			}

			if (iRecvBytes == (_RTP_HEADER_SIZE + _UDP_PKT_SIZE))
			{
				/* RTP packet size */
				if ((pMulticastSys->uReadCount == 1)
					&& (pMulticastSys->bRtp == FALSE))
				{
					PrintError("%s(%d) Error> unmatched RTP pkt size(%d)\n",
										__FUNCTION__, __LINE__,
										iRecvBytes);
					;
				}

				if (bDbgParseRTP == TRUE)
				{
					INT_MULTICAST_ParseRtpHeader(pktBuffer);
				}

				if ((pMulticastSys->bAVStream == TRUE)
					&& (pMulticastSys->bUseRtpCB == TRUE))
				{
					uCurRtpSeq = _RTP_LOAD16(pktBuffer,2);
					if (pMulticastSys->uExpectedRtpSeq != 0)
					{
						/* Check RTP sequence */
						/* Callback if there is RTP sequence error */
						if (pMulticastSys->uExpectedRtpSeq != uCurRtpSeq)
						{
							PrintError("%s(%d) Error> unmatched RTP seq(cur:%d, exp:%d)\n",
										__FUNCTION__, __LINE__,
										uCurRtpSeq,
										pMulticastSys->uExpectedRtpSeq);
							/*
							pMulticastSys->cbRtpEvent.rtpErrorCallback();
							*/
						}
					}
					pMulticastSys->uLastRtpSeq = uCurRtpSeq;
					pMulticastSys->uExpectedRtpSeq = (uCurRtpSeq + _RTP_NEXT_SEQNUM);
					pMulticastSys->uRtpCount++;
				}

	#ifdef __MULTICAST_EXTRACT_PAYLOAD_ONLY__
				VK_memcpy(payloadBuffer, pktBuffer + _RTP_HEADER_SIZE, iRecvBytes);
	#else
				iRecvBytes -= _RTP_HEADER_SIZE;
				ptr += _RTP_HEADER_SIZE;
	#endif
			}
			else if(iRecvBytes == _UDP_PKT_SIZE)
			{
				/* UDP packet size */
				if ((pMulticastSys->uReadCount == 1)
					&& (pMulticastSys->bRtp == TRUE))
				{
					PrintError("%s(%d) Error> unmatched UDP pkt size(%d)\n",
										__FUNCTION__, __LINE__,
										iRecvBytes);
					;
				}

	#ifdef __MULTICAST_EXTRACT_PAYLOAD_ONLY__
				VK_memcpy(payloadBuffer, pktBuffer, iRecvBytes);
	#else
				;
	#endif
			}
			else
			{
				PrintError("%s(%d) Error> incorrect pkt size(recv:%d)\n",
							__FUNCTION__, __LINE__,
							iRecvBytes);
				continue;
			}

			/* Copy TS payload only to pumping */
			VK_memcpy((HUINT8*)((unsigned long)pBuffer+uTotalBytes), ptr, iRecvBytes);
			uTotalBytes += iRecvBytes;
			continue;
		}
	}

func_done:
	PrintExit;
	return uTotalBytes;

func_error:
	PrintError("%s(%d) Error> failed(tot:%u)!\n", __FUNCTION__, __LINE__, uTotalBytes);
	PrintExit;
	return 0; /* (-1) */
}
