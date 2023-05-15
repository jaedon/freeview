/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   transport_rtp.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	JungWook Chai
 * Current Author:	$Author: juchae@humaxdigital.com $
 * Date:			$Date: Thur Oct 29 10:20:48 KST 2009 $
 * File Description:	Real-Time STREAMMING IP Tuner Header File
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.							  */
/* All rights reserved.											*/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <errno.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include "di_iptuner20.h"
#include "di_iptuner_config.h"
#include "di_uart.h"
#include "util_url.h"
#include "rtsp_lib.h"
#include "transport_rtp.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define RTP_SESSION_DEBUG
//#define RTP_SESSION_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
  #ifdef RTP_SESSION_DEBUG
  #define PrintData		DI_UART_Print
  #define PrintDebug		DI_UART_Print
  #define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
  #define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
  #define PrintError		DI_UART_Print
  #else
  #define PrintData		while (0) ((int (*)(char *, ...)) 0)
  #define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
  #define PrintEnter		while (0) ((int (*)(char *, ...)) 0)
  #define PrintExit		while (0) ((int (*)(char *, ...)) 0)
  #define PrintError		DI_UART_Print
  #endif
#else
  #define PrintData		while (0) ((int (*)(char *, ...)) 0)
  #define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
  #define PrintError		while (0) ((int (*)(char *, ...)) 0)
  #define PrintEnter
  #define PrintExit
#endif /* END DEBUG DEFINE */

#ifdef RTP_SESSION_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((HINT32 (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)

#define _UDP_RMEM_MAX                  (1048560) /* 1,048,560 */

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


typedef struct RTP_Header_s
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
} RTP_Header_t;

typedef struct RTP_Packet_s
{
	HUINT8 header[_RTP_HEADER_SIZE];
	HUINT8 payload[_UDP_PKT_SIZE];
} RTP_Packet_t;

typedef struct RTSP_UdpPacket_s
{
	HUINT8 payload[_UDP_PKT_SIZE];
} RTSP_UdpPacket_t;

static void P_RTP_TuneRTPSocket(int fd)
{
	int	 i_new_rmax = _UDP_RMEM_MAX;
	/*
	 * Linux kernel tuning: set socket receive buffer to 100k by default.
	 * It works with low bit rate stream up to ~14Mbps. With higher bit rate (19 or above)Mbps,
	 * Linux starts dropping UDP packets. We need to increase our UDP socket receive buffer size
	 * by using setsockopt(). Therefore, /proc/sys/net/core/rmem_max needs to be changed.
	 */
	FILE *fp = NULL;
	char  m_buf[80];
	int   i_cur_rmax = 0;
	int   i_size = i_new_rmax;
	socklen_t i_sklen = 0;
	size_t i_lineLen = 0;

	fp = fopen("/proc/sys/net/core/rmem_max", "rw");
	if (fp == NULL)
	{
		PrintError("%s(%d) Error> Can't open /proc/sys/net/core/rmem_max(rw)\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	else
	{
		i_lineLen = fread(m_buf, 1, sizeof(m_buf)-1, fp);
		if (i_lineLen == 0)
		{
			PrintError("%s(%d) Error> Can't read rmem_max\n", __FUNCTION__, __LINE__);
			goto func_error;
		}
		m_buf[i_lineLen] = '\0';
		i_cur_rmax = strtol(m_buf, (char **)NULL, 10);
		fclose(fp);
		fp = NULL;

		PrintDebug("%s(%d) current rmem_max %d\n", __FUNCTION__, __LINE__, i_cur_rmax);
		if (i_cur_rmax < _UDP_RMEM_MAX)
		{
			/* it is the default value, make it bigger */
			PrintDebug("%s(%d) Increasing default rmem_max from %d to rmem_max %d\n", __FUNCTION__, __LINE__, i_cur_rmax, _UDP_RMEM_MAX);
			i_lineLen = VK_snprintf(m_buf, sizeof(m_buf), "%d", _UDP_RMEM_MAX);
			fp = fopen("/proc/sys/net/core/rmem_max", "w");
			if (fp)
			{
				fwrite(m_buf, 1, i_lineLen, fp);
				fclose(fp);
				fp = NULL;
				i_new_rmax = _UDP_RMEM_MAX;
			}
			else
			{
				i_new_rmax = i_cur_rmax;
			}
		}
		else
		{
			i_new_rmax = i_cur_rmax;
		}
	}

	/* now increase socket receive buffer size */
	i_sklen = sizeof(i_size);
	if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &i_size, &i_sklen) == 0)
	{
		PrintDebug("%s(%d) current socket receive buffer size = %d KB\n", __FUNCTION__, __LINE__, i_size/1024);
	}

	if (i_size < i_new_rmax)
	{
		i_size = i_new_rmax;
		if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &i_size, i_sklen) != 0)
		{
			PrintError("%s(%d) ERROR: can't set UDP receive buffer to %d, errno=%d\n", __FUNCTION__, __LINE__, i_size, errno);
		}
		i_sklen = sizeof(i_size);
		if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &i_size, &i_sklen) == 0)
		{
			PrintDebug("%s(%d) new socket receive buffer size = %d KB\n", __FUNCTION__, __LINE__, i_size/1024);
		}
	}

func_error:
	if (fp != NULL) /* CID #46217 */
	{
		fclose(fp);
		fp = NULL;
	}
	return;
}

void net_RTPTuneSocket(int fd)
{
	P_RTP_TuneRTPSocket(fd);
}

int net_RTPConnect(char *pBindHost, int iBindPort)
{
	HBOOL bConnectWait = FALSE;
	struct sockaddr_in st_sin;
	struct timeval t_tmval;
	int i_err;
	int i_errLen;
	int i_recvBuffSize;
	int nfd = (-1);
	fd_set t_fset;

	PrintEnter;

	if ((pBindHost == NULL) || (iBindPort == 0))
	{
		PrintError("%s(%d) Error> Invalid arg(port:%d)\n", __FUNCTION__, __LINE__, iBindPort);
		return -1;
	}
	PrintDebug("%s(%d) args(host:%s, port:%d)\n", __FUNCTION__, __LINE__, pBindHost, iBindPort);

	if (iBindPort <= 1023)
	{
		PrintError("%s(%d) Error> The portNum has small value(%d)\n", __FUNCTION__, __LINE__, iBindPort);
		; /* return -1; */
	}


	nfd = socket (PF_INET, SOCK_DGRAM, 0);
	if (nfd < 0)
	{
		PrintError("%s(%d) Error> Can't create socket\n", __FUNCTION__, __LINE__);
		return -1;
	}
	VK_memset(&st_sin, 0, sizeof(st_sin));
	st_sin.sin_family = AF_INET;
	if (!pBindHost || !VK_strcmp (pBindHost, "0.0.0.0"))
	{
		st_sin.sin_addr.s_addr = htonl (INADDR_ANY);
	}
	else
	{
#if HAVE_INET_PTON
		inet_pton (AF_INET, pBindHost, &st_sin.sin_addr);
#elif HAVE_INET_ATON
		inet_aton (pBindHost, &st_sin.sin_addr);
#endif
	}
	st_sin.sin_port = htons (iBindPort);

	/* Increase the socket rx buffer size to maximum -- this is UDP */
	i_recvBuffSize = 240 * 1024;
	if (setsockopt (nfd, SOL_SOCKET, SO_RCVBUF, &i_recvBuffSize, sizeof (i_recvBuffSize)) != 0)
	{
		PrintError("%s(%d) Error> Couldn't set receive socket buffer size\n", __FUNCTION__, __LINE__);
		;
	}

	P_RTP_TuneRTPSocket(nfd);

	/* datagram socket */
	if (bind (nfd, (struct sockaddr *) &st_sin, sizeof (st_sin)) != 0)
	{
		if (errno != EINPROGRESS)
		{
			PrintError("%s(%d) Error> bind() failed, err:%d,%s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
			goto func_error;
		}
	}

	if (bConnectWait == TRUE)
	{
		t_tmval.tv_sec = 5; /* 1 second timeout */
		t_tmval.tv_usec = 0;

		FD_ZERO (&t_fset);
		FD_SET (nfd, &t_fset);

		i_err = select (nfd + 1, &t_fset, NULL, NULL, &t_tmval);
		if (i_err < 0)
		{
			PrintError("%s(%d) Error> select() failed, err:%d,%s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
			goto func_error;
		}
		else if (i_err == 0)
		{
			PrintError("%s(%d) Error> Timeout! No data from host %s\n", __FUNCTION__, __LINE__, pBindHost);
			goto func_error;
		}

		i_errLen = sizeof (i_err);
		getsockopt (nfd, SOL_SOCKET, SO_ERROR, &i_err, (socklen_t *) &i_errLen);
		if (i_err)
		{
			PrintError("%s(%d) Error> getsockopt() failed, err:%d\n", __FUNCTION__, __LINE__, i_err);
			goto func_error;
		}
	}

	PrintExit;
	return nfd;

func_error:
	if (nfd >= 0) /* CID #46218 */
	{
		net_Close (nfd);
		return -1;
	}
	PrintExit;
	return (-1);
}

int net_ConnectRTP(char *pBindHost, int iBindPort)
{
	return net_RTPConnect(pBindHost, iBindPort);
}

static void P_RTP_PrintRtpHeader(RTP_Header_t *pRtpHeader)
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

static void P_RTP_ParseRtpHeader(HUINT8 *pBuf, RTP_Header_t *rtpHeader)
{
	HBOOL bShowHeader = FALSE;

	/*
	PrintDebug("0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
			  pBuf[0],pBuf[1],pBuf[2],pBuf[3],
			  pBuf[4],pBuf[5],pBuf[6],pBuf[7],
			  pBuf[8],pBuf[9],pBuf[10],pBuf[11]);
	*/

	if (rtpHeader == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtpHeader\n", __FUNCTION__, __LINE__);
		return;
	}

	/* Take brtp routines */
	rtpHeader->version    = (pBuf[0] & 0xC0) >> 6;
	rtpHeader->padding    = (pBuf[0] & 0x20) >> 5;
	rtpHeader->extension  = (pBuf[0] & 0x10) >> 4;
	rtpHeader->csrc_count = (pBuf[0] & 0x0F) >> 0;

	rtpHeader->marker       = (pBuf[1] & 0x80) >> 7;
	rtpHeader->payload_type = (pBuf[1] & 0x7F) >> 0;

	rtpHeader->sequence_num = (pBuf[2] << 8);
	rtpHeader->sequence_num |= pBuf[3];

	rtpHeader->timestamp = (pBuf[4] << 24);
	rtpHeader->timestamp |= (pBuf[5] << 16);
	rtpHeader->timestamp |= (pBuf[6] << 8);
	rtpHeader->timestamp |= (pBuf[7] << 0);

	rtpHeader->ssrc = (pBuf[8] << 24);
	rtpHeader->ssrc |= (pBuf[8] << 16);
	rtpHeader->ssrc |= (pBuf[8] << 8);
	rtpHeader->ssrc |= (pBuf[8] << 0);

	if(rtpHeader->version != _RTP_VERSION) {
		PrintError("%s(%d) Error> RTP version not supported!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if(rtpHeader->padding) {
		PrintError("%s(%d) Error> RTP padding not supported!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if(rtpHeader->extension) {
		PrintError("%s(%d) Error> RTP header extensions not supported!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if(rtpHeader->csrc_count) {
		PrintError("%s(%d) Error> RTP non-zero csrc_count not supported!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if(rtpHeader->payload_type != _RTP_PAYLOAD_TYPE_MP2T) {
		PrintError("%s(%d) Error> Unsupported RTP payload type!\n",__FUNCTION__,__LINE__ );
		goto func_error;
	}

	if (bShowHeader)
	{
		P_RTP_PrintRtpHeader(rtpHeader);
	}
	return;

func_error:
	PrintError("%s(%d) Error> RTP header failed\n",__FUNCTION__,__LINE__ );
	return;
}


#if 0
/* <------------------------------------------ debug ------------------------------------------> */
#undef TRACE_HEX
#define TRACE_HEX(x) VK_printf x;

/* P_RTP_DumpHex("hexdump|", p_buffer, 1024); */
static unsigned long P_RTP_DumpHex(char *string, unsigned char *pBuf, unsigned long length)
{
    static const char *_gHexChars = "0123456789abcdef";

    /* Check the length */
    if (length > 0)
    {
        /* Sanitise the input */
        if (length > 64*1024)
        {
        	TRACE_HEX(("Size of Dumping data is too much! %s, %d\n", __FILE__, __LINE__));
            /* k_fatal(0, K_LOCAL); */
        }
        else
        {
            unsigned char	buf [90];
            unsigned long	i;

			TRACE_HEX(("################################################################################## \n"));
            TRACE_HEX(("# Hex Dump of \"%s\". %ld (0x%0lx) bytes\n",string, length, length));
			TRACE_HEX(("################################################################################## \n"));

            /* Hex dump of the message body */
            for (i = 0; i < length; i += 16)
            {
                unsigned long	j;
                int		len;

                /* Print the byte u64Offset */
			#if 0
                len = VK_sprintf ((char *)buf,  "%05lx: ", i);   /* Byte address */
			#else
                len = VK_sprintf ((char *)buf,  "hex| %05lx: ", i);
			#endif
                /* Print hex values */
                for (j = 0; j < 16; j++)
                {
                    if (i+j < length)
                    {
                        buf[len++] = _gHexChars [((pBuf[i+j] >> 4) & 0xF)];
                        buf[len++] = _gHexChars [(pBuf[i+j] & 0xF)];
                    }
                    else
                    {
                        buf[len++] = '.';
                        buf[len++] = '.';
                    }
                    buf[len++] = ' ';
                }

                /* Print the ASCII equivelents */
                for (j = 0; j < 16; j++)
                {
                    if (i+j < length)
                        buf[len++] = VK_isprint(pBuf[i+j]) ? pBuf[i+j] : '.';
                    else
                        buf[len++] = ' ';
                }
                buf[len] = '\0';

				TRACE_HEX (("%s\n", buf));
            }
        }
    }

    return length;
}
/* <------------------------------------------ debug ------------------------------------------> */
#endif

static HINT32 P_RTP_ReadSmallRtpPacket(struct RTSP_CHANNEL_t *pRtpChannel, HUINT8 *pBuffer, size_t i_request) /* Under 1500 bytes */
{
	HBOOL bDebug = FALSE;
	HINT32 i_read = 0;
	size_t i_packetSize = 0;
	HINT32 i_adjustStartPosition = 0;
	HUINT8 m_packetBuffer[_MTU_PKT_SIZE];
	HUINT8 *ptr = NULL; /* For getting payload only, skipping RTP header */
	int				fromlen;
	struct sockaddr *from; // struct sockaddr_in from;
	RTP_Header_t rtpHeader;
	HINT32 i_uncopied = 0;

	PrintEnter;

	fromlen = sizeof(from); // <==
	VK_memset(&rtpHeader, 0, sizeof (rtpHeader));

	if (pRtpChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid arg\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (pRtpChannel->i_sock < 0)
	{
		PrintError("%s(%d) Error> Invalid fd(%d)!\n", __FUNCTION__, __LINE__, pRtpChannel->i_sock);
		goto func_error;
	}

	if (i_request > _MTU_PKT_SIZE)
	{
		PrintError("%s(%d) Error> Too large request buffer[req:%d]\n", __FUNCTION__, __LINE__, i_request);
		goto func_error;
	}

	if (RTSP_TRANSPORT_RTP_UNICAST == pRtpChannel->transport_type)
	{
		i_packetSize = _RTP_PKT_SIZE;
		i_adjustStartPosition = _RTP_HEADER_SIZE;
	}
	else
	{
		i_packetSize = _UDP_PKT_SIZE;
		i_adjustStartPosition = 0;
	}
	PrintData("%s(%d) [req:%d, protocol:%d, fd:%d, pkt:%d, adj:%d]\n", __FUNCTION__, __LINE__,
			i_request,
			pRtpChannel->transport_type,
			pRtpChannel->i_sock,
			i_packetSize,
			i_adjustStartPosition);

	/* Read socket until given p_buffer is full */
	/* Socket timeout value have 1 (sec) */
	if (1) /* for(;;) */
	{
		/* Read socket until given p_buffer is full */
		/* Socket timeout value have 1 (sec) */
		VK_memset(m_packetBuffer, 0, _MTU_PKT_SIZE);
		ptr = m_packetBuffer;

		i_read = recvfrom(pRtpChannel->i_sock,
							(void *)m_packetBuffer,
#if 0
							i_request,
#else
							i_packetSize,
#endif
							0, /* consider MSG_DONTWAIT usage */
							(struct sockaddr *) &from,
							(socklen_t*) &fromlen);
		if (i_read <= 0)
		{
			PrintError("%s(%d) Error> recv() failed(read:%d/size:%d)\n", __FUNCTION__, __LINE__, i_read, i_request);
			goto func_error;
		}

		/* Moved the pointer */
		if (i_read == (_RTP_HEADER_SIZE + _UDP_PKT_SIZE)) /* RTP packet size */
		{
			if (RTSP_TRANSPORT_UDP_UNICAST == pRtpChannel->transport_type)
			{
				PrintError("%s(%d) Error> Unmatched UDP size(fd:%d) --> eof\n", __FUNCTION__, __LINE__, pRtpChannel->i_sock);
			}

			VK_memset(&rtpHeader, 0, sizeof (rtpHeader));
			P_RTP_ParseRtpHeader(m_packetBuffer, &rtpHeader);

			if (pRtpChannel->i_prevRtpSeq != 0)
			{
				if ((pRtpChannel->i_prevRtpSeq+1) != rtpHeader.sequence_num)
				{
					PrintError("%s(%d) Error> Unmatched RTP seq(prev:%d, cur:%d)\n", __FUNCTION__, __LINE__, pRtpChannel->i_prevRtpSeq, rtpHeader.sequence_num);
					;
				}
			}
			pRtpChannel->i_prevRtpSeq = rtpHeader.sequence_num;

			if (bDebug == TRUE)
			{
				/*
				 * rtp_hdr.version      : 2
				 * rtp_hdr.padding      : 0
				 * rtp_hdr.extension    : 0
				 * rtp_hdr.csrc_count   : 0
				 * rtp_hdr.marker       : 0
				 * rtp_hdr.payload_type : 33
				 * rtp_hdr.seq_num      : 38830
				 * rtp_hdr.ts           : 859033552
				 * rtp_hdr.ssrc         : 353703189
				 */
				if (rtpHeader.padding != 0)
				{
					PrintError("%s(%d) Error> Unexpected RTP header(padding:%d, seq:%d)\n", __FUNCTION__, __LINE__, rtpHeader.padding, rtpHeader.sequence_num);
				}

				if (rtpHeader.extension != 0)
				{
					PrintError("%s(%d) Error> Unexpected RTP header(extension:%d, seq:%d)\n", __FUNCTION__, __LINE__, rtpHeader.extension, rtpHeader.sequence_num);
				}

				if (rtpHeader.csrc_count != 0)
				{
					PrintError("%s(%d) Error> Unexpected RTP header(csrc_count:%d, seq:%d)\n", __FUNCTION__, __LINE__, rtpHeader.csrc_count, rtpHeader.sequence_num);
				}

				if (rtpHeader.marker != 0)
				{
					PrintError("%s(%d) Error> Unexpected RTP header(marker:%d, seq:%d)\n", __FUNCTION__, __LINE__, rtpHeader.marker, rtpHeader.sequence_num);
				}

				if (rtpHeader.payload_type != 33)
				{
					PrintError("%s(%d) Error> Unexpected RTP header(payload_type:%d, seq:%d)\n", __FUNCTION__, __LINE__, rtpHeader.payload_type, rtpHeader.sequence_num);
				}
			}
			i_read -= _RTP_HEADER_SIZE;
			ptr += _RTP_HEADER_SIZE;
		}
		else if (i_read == _UDP_PKT_SIZE)  /* RTP packet size */
		{
			if (RTSP_TRANSPORT_RTP_UNICAST == pRtpChannel->transport_type)
			{
				PrintError("%s(%d) Error> Unmatched RTP size(fd:%d) --> eof\n", __FUNCTION__, __LINE__, pRtpChannel->i_sock);
			}
		}

		/* Copy TS payload only to pumping */
		if (i_read <= (HINT32)i_request)
		{
			PrintError("%s(%d) Error> Critical unexpected size(read:%d/size:%d)\n", __FUNCTION__, __LINE__, i_read, i_request);
			VK_memcpy((HUINT8*)((unsigned long)pBuffer), ptr, i_read);
		}
		else
		{
			PrintError("%s(%d) Error> Critical unexpected size(read:%d/size:%d)\n", __FUNCTION__, __LINE__, i_read, i_request);

			if (0)
			{
				VK_memcpy((HUINT8*)((unsigned long)pBuffer), ptr, (i_read-i_request));

				i_uncopied = (i_read - i_request);
				if (i_uncopied > 0)
				{
					(void)RTSP_LIB_ClearChannelBuffer(pRtpChannel);

					pRtpChannel->i_dataLen = i_uncopied;
					VK_memcpy((HUINT8*)pRtpChannel->p_buffer, ptr+i_request, i_uncopied);
				}
			}
			else
			{
				VK_memcpy((HUINT8*)((unsigned long)pBuffer), ptr, i_read);
			}
		}
	}

	PrintError("%s(%d) --(read:%d, req:%d, diff:%d)\n", __FUNCTION__, __LINE__, i_read, i_request, (i_request-i_read));
	PrintExit;
	return i_read;

func_error:
	PrintError("%s(%d) Error> failed(read:%d, req:%d, diff:%d)\n", __FUNCTION__, __LINE__, i_read, i_request);
	PrintExit;
	return 0;
}

int net_RTPReadChannel(struct RTSP_CHANNEL_t *pRtpChannel, HUINT8 *pBuffer, size_t i_request)
{
	HINT32 nret = 0;
	HINT32 i_read = 0;
	HUINT32 i_total = 0;
	HUINT32 i_recvCnt = 0;
	HUINT32 i_errLoopCnt = 0;
	HUINT32 i_errLoopMax = 100;
	fd_set t_fds;
	struct timeval 	i_sockTimeout;
	struct sockaddr *from = NULL;
	socklen_t	from_len = 0;

	size_t i_packetSize = 0;
	HINT32 i_adjustStartPosition = 0;
	HUINT8 m_packetBuffer[_MTU_PKT_SIZE];
	HUINT8 *ptr = NULL; /* For getting payload only, skipping RTP header */

	HBOOL b_copyPartially = FALSE;
	HINT32 i_remain = 0;
	HINT32 i_uncopied = 0;
	RTP_Header_t rtpHeader;

	PrintEnter;

	VK_memset(&rtpHeader, 0, sizeof (rtpHeader));
	if ((pRtpChannel == NULL) || (pRtpChannel->transport_type != RTSP_TRANSPORT_RTP_UNICAST))
	{
		PrintError("%s(%d) Error> Invalid arg\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (pRtpChannel->i_sock <= 0)
	{
		PrintError("%s(%d) Error> Invalid fd(%d)!\n", __FUNCTION__, __LINE__, pRtpChannel->i_sock);
		goto func_error;
	}

	i_total = 0;
	if (i_request < _MTU_PKT_SIZE)
	{
		VK_memset(m_packetBuffer, 0, _MTU_PKT_SIZE);
		ptr = m_packetBuffer;

		PrintDebug("%s(%d) small remained data[req:%d]\n", __FUNCTION__, __LINE__, i_request);
		i_read = P_RTP_ReadSmallRtpPacket(pRtpChannel, m_packetBuffer, i_request); /* Under 1500 bytes */
		if (i_read != (HINT32)i_request)
		{
			PrintError("%s(%d) Error> Unmatched small buffer[small:%d != req:%d]\n", __FUNCTION__, __LINE__, i_read, i_request);
		}

		if (i_read > 0)
		{
			if (i_read <= (HINT32)i_request)
			{
				VK_memcpy((HUINT8*)((unsigned long)pBuffer+i_total), ptr, i_read);
				i_total += i_read;
				PrintDebug("%s(%d) copy small size[read:%d <= remain:%d, total:%d]\n", __FUNCTION__, __LINE__,i_read, i_request, i_total);
			}
			else
			{
				/* Copy remain size */
				PrintError("%s(%d) Error> More bigger size[read:%d > remain:%d, total:%d]\n", __FUNCTION__, __LINE__,i_read, i_request, i_total);
				VK_memcpy((HUINT8*)((unsigned long)pBuffer+i_total), ptr, i_request);
				i_total += i_request;

				/* If uncopied data is remain, move it to channel's rtp buffer */
				i_uncopied = (i_read - i_request);
				if (i_uncopied > 0)
				{
					(void)RTSP_LIB_ClearChannelBuffer(pRtpChannel);

					pRtpChannel->i_dataLen = i_uncopied;
					VK_memcpy((HUINT8*)pRtpChannel->p_buffer, ptr+i_request, i_uncopied);
				}
				PrintError("%s(%d) Error> Remained uncopied[uncopied:%d, read:%d > remain:%d, total:%d]\n", __FUNCTION__, __LINE__, i_uncopied, i_read, i_request, i_total);
			}
		}

		PrintError("%s(%d) Error> --Too small buffer[req:%d, i_total:%u]\n", __FUNCTION__, __LINE__, i_request, i_total);
		return i_total;
	}

	if (pRtpChannel->i_dataLen > 0)
	{
		PrintError("%s(%d) Error(fatal)> Error> Error> There is small prev remained buffer[req:%d]\n", __FUNCTION__, __LINE__, pRtpChannel->i_dataLen);
		goto func_error;
	}


	if (RTSP_TRANSPORT_RTP_UNICAST == pRtpChannel->transport_type)
	{
		i_packetSize = _RTP_PKT_SIZE;
		i_adjustStartPosition = _RTP_HEADER_SIZE;
	}
	else
	{
		i_packetSize = _UDP_PKT_SIZE;
		i_adjustStartPosition = 0;
	}
	PrintData("%s(%d) [req:%d, protocol:%d, fd:%d, pkt:%d, adj:%d]\n", __FUNCTION__, __LINE__,
			i_request,
			pRtpChannel->transport_type,
			pRtpChannel->i_sock,
			i_packetSize,
			i_adjustStartPosition);


	/* Read socket until given p_buffer is full */
	/* Socket timeout value have 1 (sec) */
	while (1)
	{
		//PrintDebug("%s(%d) Received Bytes(err:%d, total:%u, i_request:%u)!\n", __FUNCTION__, __LINE__, i_errLoopCnt, i_total, i_request);
		i_recvCnt++;

		if (i_recvCnt > 1000) /* 1316*X */
		{
			PrintError("%s(%d) Error> Too many receive[err:%d, total:%u, req:%u:last:%d]\n", __FUNCTION__, __LINE__, i_recvCnt, i_errLoopCnt, i_total, i_request, i_read);
			goto func_error;
		}

		if (pRtpChannel->i_sock < 0)
		{
			PrintError("%s(%d) Error> Invalid fd(%d)!\n", __FUNCTION__, __LINE__, pRtpChannel->i_sock);
			goto func_error;
		}

		/* The request length must have 1500 or above value */
		if (i_total >= i_request)
		{
			PrintDebug("%s(%d) Read stream successfully(total:%u, req:%u, loop(%d, err:%d)!\n", __FUNCTION__, __LINE__, i_total, i_request, i_recvCnt, i_errLoopCnt);
			if (i_total > i_request)
			{
				PrintError("%s(%d) Error> Invalid receive size(total:%u > req:%u, loop(%d, err:%d)!\n", __FUNCTION__, __LINE__, i_total, i_request, i_recvCnt, i_errLoopCnt);
				;
			}
			goto func_done;
		}

		b_copyPartially = FALSE;
		if ((i_total+_UDP_PKT_SIZE) >= i_request)
		{
			PrintDebug("%s(%d) Read exeed size(cnt:%d, total:%u, req:%u, diff:%d, loop:%d,%d)!\n", __FUNCTION__, __LINE__, i_recvCnt, i_total, i_request, (i_request-i_total), i_recvCnt, i_errLoopCnt);

			b_copyPartially = TRUE;
			i_remain = (i_request - i_total);
			if (i_remain%188 != 0)
			{
				PrintError("%s(%d) Error> Not 188x small size[remain:%d, req:%d, total:%d]\n", __FUNCTION__, __LINE__,i_remain, i_request, i_total);
				;
			}
			VK_memset(m_packetBuffer, 0, _MTU_PKT_SIZE);
			ptr = m_packetBuffer;

			PrintError("%s(%d) Error> ++Too small remained size[remain:%d, req:%d, total:%d, cnt:%d]\n", __FUNCTION__, __LINE__, i_remain, i_request, i_total, i_recvCnt);
			i_read = P_RTP_ReadSmallRtpPacket(pRtpChannel, m_packetBuffer, i_remain); /* Under 1500 bytes */
			PrintError("%s(%d) Error> --Too small remained size[read:%d, remain:%d, total:%d]\n", __FUNCTION__, __LINE__,i_read, i_remain, i_total);
			if (i_read != i_remain)
			{
				PrintError("%s(%d) Error> ++Unmatched small buffer[small:%d != remain:%d]\n", __FUNCTION__, __LINE__, i_read, i_remain);
				;
			}

			if (i_read > 0)
			{
				if (i_read <= i_remain)
				{
					VK_memcpy((HUINT8*)((unsigned long)pBuffer+i_total), ptr, i_read);
					i_total += i_read;
					PrintDebug("%s(%d) copy small size[read:%d <= remain:%d, total:%d]\n", __FUNCTION__, __LINE__,i_read, i_remain, i_total);
				}
				else
				{
					/* Copy remain size */
					PrintError("%s(%d) Error> More bigger size[read:%d > remain:%d, total:%d]\n", __FUNCTION__, __LINE__,i_read, i_remain, i_total);
					VK_memcpy((HUINT8*)((unsigned long)pBuffer+i_total), ptr, i_remain);
					i_total += i_remain;

					/* If uncopied data is remain, move it to channel's rtp buffer */
					i_uncopied = (i_read - i_remain);
					if (i_uncopied > 0)
					{
						(void)RTSP_LIB_ClearChannelBuffer(pRtpChannel);

						pRtpChannel->i_dataLen = i_uncopied;
						VK_memcpy((HUINT8*)pRtpChannel->p_buffer, ptr+i_remain, i_uncopied);
					}
					PrintError("%s(%d) Error> Remained uncopied[uncopied:%d, read:%d > remain:%d, total:%d]\n", __FUNCTION__, __LINE__, i_uncopied, i_read, i_remain, i_total);
				}
			}
			if (i_total != i_request)
			{
				PrintError("%s(%d) Error> --Unmatched small buffer(total:%d, max:%d, diff:%d)\n", __FUNCTION__, __LINE__, i_total, i_request, (i_request-i_total));
				;
			}
			goto func_done;
		}

/*func_read:*/
		if (i_total >= i_request) /* Check again because of above smaller packet(under MTU size) */
		{
			PrintDebug("%s(%d) Read stream successfully(total:%u, req:%u, loop(%d, err:%d)!\n", __FUNCTION__, __LINE__, i_total, i_request, i_recvCnt, i_errLoopCnt);
			if (i_total > i_request)
			{
				PrintError("%s(%d) Error> Invalid receive size(total:%u > req:%u, loop(%d, err:%d)!\n", __FUNCTION__, __LINE__, i_total, i_request, i_recvCnt, i_errLoopCnt);
				;
			}
			goto func_done;
		}
		VK_memset(m_packetBuffer, 0, _MTU_PKT_SIZE);
		ptr = m_packetBuffer;

		/*
		 * Receive payload; extract RTP header if RTP streaming
		 * ssize_t net_Recvfrom(int nfd, const struct virtual_socket_t *vs,
		 *                      HUINT8 *restrict p_buf, size_t i_buflen,
		 *                      HBOOL waitall)
		 */
		i_read = recvfrom(pRtpChannel->i_sock,
							  m_packetBuffer,
#if 0
							  _MTU_PKT_SIZE,
#else
							  i_packetSize,
#endif
							  0,
							  from,
							  &from_len);

		if (i_read > 0)
		{
			if (i_read == (_RTP_HEADER_SIZE + _UDP_PKT_SIZE)) /* RTP packet size */
			{
				if (RTSP_TRANSPORT_UDP_UNICAST == pRtpChannel->transport_type)
				{
					PrintError("%s(%d) Error> Unmatched UDP size(fd:%d) --> eof\n", __FUNCTION__, __LINE__, pRtpChannel->i_sock);
				}

				/* Parse RTP header */
				VK_memset(&rtpHeader, 0, sizeof (rtpHeader));
				P_RTP_ParseRtpHeader(m_packetBuffer, &rtpHeader);

				if (pRtpChannel->i_prevRtpSeq != 0)
				{
					if ((pRtpChannel->i_prevRtpSeq+1) != rtpHeader.sequence_num)
					{
						PrintError("%s(%d) Error> Unmatched RTP seq(prev:%d, cur:%d)\n", __FUNCTION__, __LINE__, pRtpChannel->i_prevRtpSeq, rtpHeader.sequence_num);
						;
					}
				}
				pRtpChannel->i_prevRtpSeq = rtpHeader.sequence_num;

				if (0)
				{
					/*
					 * rtp_hdr.version      : 2
					 * rtp_hdr.padding      : 0
					 * rtp_hdr.extension    : 0
					 * rtp_hdr.csrc_count   : 0
					 * rtp_hdr.marker       : 0
					 * rtp_hdr.payload_type : 33
					 * rtp_hdr.seq_num      : 38830
					 * rtp_hdr.ts           : 859033552
					 * rtp_hdr.ssrc         : 353703189
					 */
					if (rtpHeader.padding != 0)
					{
						PrintError("%s(%d) Error> Unexpected RTP header(padding:%d, seq:%d)\n", __FUNCTION__, __LINE__, rtpHeader.padding, rtpHeader.sequence_num);
					}

					if (rtpHeader.extension != 0)
					{
						PrintError("%s(%d) Error> Unexpected RTP header(extension:%d, seq:%d)\n", __FUNCTION__, __LINE__, rtpHeader.extension, rtpHeader.sequence_num);
					}

					if (rtpHeader.csrc_count != 0)
					{
						PrintError("%s(%d) Error> Unexpected RTP header(csrc_count:%d, seq:%d)\n", __FUNCTION__, __LINE__, rtpHeader.csrc_count, rtpHeader.sequence_num);
					}

					if (rtpHeader.marker != 0)
					{
						PrintError("%s(%d) Error> Unexpected RTP header(marker:%d, seq:%d)\n", __FUNCTION__, __LINE__, rtpHeader.marker, rtpHeader.sequence_num);
					}

					if (rtpHeader.payload_type != 33)
					{
						PrintError("%s(%d) Error> Unexpected RTP header(payload_type:%d, seq:%d)\n", __FUNCTION__, __LINE__, rtpHeader.payload_type, rtpHeader.sequence_num);
					}
				}
				i_read -= _RTP_HEADER_SIZE;
				ptr += _RTP_HEADER_SIZE;
			}
			else if (i_read == _UDP_PKT_SIZE)  /* UDP packet size */
			{
				if (RTSP_TRANSPORT_RTP_UNICAST == pRtpChannel->transport_type)
				{
					PrintError("%s(%d) Error> Unmatched RTP size(fd:%d) --> eof\n", __FUNCTION__, __LINE__, pRtpChannel->i_sock);
				}
			}
			else
			{
				PrintError("%s(%d) Error> incorrect pkt size(recv:%d)\n", __FUNCTION__, __LINE__, i_read);
				;
			}

			/* Copy TS payload only to pumping */
			VK_memcpy((HUINT8*)((unsigned long)pBuffer+i_total), ptr, i_read);
			i_total += i_read;
			continue;
		}
		else if (i_read==0)
		{
			PrintError("%s(%d) Error> connection closed(recv:%d, total:%d, max:%d)\n", __FUNCTION__, __LINE__, i_read, i_total, i_request);
			goto func_wait;
		}
		else
		{
			i_errLoopCnt++;
			PrintError("%s(%d) Error> read func_error(%d, total:%d, errloop:%d,%d)\n", __FUNCTION__, __LINE__, i_read, i_total, i_errLoopCnt, i_errLoopMax);

			if (i_errLoopCnt > i_errLoopMax)
			{
				PrintError("%s(%d) Error> Too many retries(%d>max:%d, fd:%d) --> err:%d,%s\n", __FUNCTION__, __LINE__, i_errLoopCnt, i_errLoopMax, pRtpChannel->i_sock, errno, strerror(errno));
				goto func_error;
			}

			if (errno == EINTR)
			{
				continue;
			}
			else if (errno != EAGAIN)
			{
				PrintError("%s(%d) Error> recvfrom error(%d,%s)\n", __FUNCTION__, __LINE__, errno, strerror(errno));
				goto func_error;
			}
		}


func_wait:
		FD_ZERO(&t_fds);
		FD_SET(pRtpChannel->i_sock, &t_fds);
		i_sockTimeout.tv_sec = 0;
		i_sockTimeout.tv_usec = _RTP_RECV_TIMEOUT;

		nret = select(pRtpChannel->i_sock+1, &t_fds, NULL, NULL, &i_sockTimeout);
		if (nret == 0)
		{
			PrintError("%s(%d) Error> Receive timeout\n", __FUNCTION__, __LINE__);
			continue;
		}
		else if (nret<0 && errno != EINTR)
		{
			PrintError("%s(%d) Error> select error(%d,%s)\n", __FUNCTION__, __LINE__, errno, strerror(errno));
			goto func_error;
		}
	}

func_done:
	if (i_total != i_request)
	{
		PrintError("%s(%d) Error> Can't read all of pkts(total:%d, max:%d, diff:%d)\n", __FUNCTION__, __LINE__, i_total, i_request, (i_request-i_total));
		;
	}
	PrintError("%s(%d) --(total:%d, max:%d, diff:%d), last(read:%d, cnt:%d,%d)\n", __FUNCTION__, __LINE__, i_total, i_request, (i_request-i_total), i_read, i_recvCnt, i_errLoopCnt);
	PrintExit;
	return i_total;

func_error:
	PrintError("%s(%d) Error> failed(total:%d, max:%d)\n", __FUNCTION__, __LINE__, i_total, i_request);
	if (i_total != 0)
	{
		PrintExit;
		return i_total;
	}

	PrintExit;
	return (-1);
}
