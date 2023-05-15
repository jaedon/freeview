/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   rtsp_lib.c  $
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
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "di_iptuner20.h"
#include "di_iptuner_config.h"
#include "di_uart.h"
#include "stream_global.h"
#include "stream_rtsp.h"
#include "mms_asf.h"
#include "rtsp_lib.h"
#include "rtsp_ms.h"
#include "rtsp_session.h"
#include "util_url.h"
#include "transport_tcp.h"
#include "transport_udp.h"
#include "transport_rtp.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define RTSP_LIB_DEBUG
//#define RTSP_LIB_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
  #ifdef RTSP_LIB_DEBUG
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

#ifdef RTSP_LIB_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((HINT32 (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)


/*
 * realloc() wrapper
 */
void *RTSP_LIB_Realloc(void *p,const size_t size)
{
	PrintEnter;
	p = DD_MEM_Realloc(p,size);
	if (!p)
	{
		PrintError("%s(%d) Error> could not reallocate memory\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	PrintExit;
	return p;
}


/*
 * list manipulation functions
 * new, append, search and free
 */
struct RTSP_LinkedListHdr_s *RTSP_LIB_NewLinkedListHdr(void *p)
{
	struct RTSP_LinkedListHdr_s *newl = NULL;
	PrintEnter;
	newl = DD_MEM_Alloc(sizeof(struct RTSP_LinkedListHdr_s));
	if (!newl) {
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_memset(newl ,0, sizeof(struct RTSP_LinkedListHdr_s));
	newl->p = p;
	newl->next = NULL;
	newl->prev = NULL;
	PrintExit;
	return newl;
}

/*
 * append 'p' to 'list'
 */
struct RTSP_LinkedListHdr_s *RTSP_LIB_AppendToLinkedListHdr(struct RTSP_LinkedListHdr_s **list,void *p)
{
	/* non-recursive method for fast append */
	struct RTSP_LinkedListHdr_s **cur,**prev;
	PrintEnter;

	for(cur = prev = list ; *cur ; prev = cur,cur = &((*cur)->next));
	/* now *cur points to NULL */
	*cur = RTSP_LIB_NewLinkedListHdr(p);
	(*cur)->next = NULL;
	(*cur)->prev = *prev; /* if first element, prev is itself*/

	PrintExit;

	return *cur;
}


/*
 * count elements in RTSP_LinkedListHdr_s
 */
HINT32 RTSP_LIB_LinkedListCount(struct RTSP_LinkedListHdr_s *list)
{
	HINT32 num;
	PrintEnter;

	for(num = 0; list ; list = list->next)
	{
		num++;
	}

	PrintExit;
	return num;
}

/*
 * search RTSP_LinkedListHdr_s, using 'comp' function.
 */
struct RTSP_LinkedListHdr_s *RTSP_LIB_SearchFromLinkedListHdr(struct RTSP_LinkedListHdr_s *list,void *p,
				 HINT32 (*comp)(void *,void *))
{
	struct RTSP_LinkedListHdr_s *iter;
	PrintEnter;
	if(list == NULL) return NULL;

	/* non-recursive */
	for(iter = list ; iter ; iter = iter->next) {
	if(!comp(iter->p,p))
		break;
	}

	PrintExit;
	return iter;
}

void RTSP_LIB_FreeLikedList(struct RTSP_LinkedListHdr_s *list,void (*free_er)(void *))
{
	struct RTSP_LinkedListHdr_s *cur,*next;
	PrintEnter;
	if(list == NULL) return;

	/* non-recursive */
	for(cur = list ; cur ; cur = next)
	{
		next = cur->next;
		free_er(cur->p);
		DD_MEM_Free(cur);
		cur = NULL;
	}

	PrintExit;
}


#if 0
/* only4string debuginfo function */
void RTSP_LIB_DumpLinkedList(struct RTSP_LinkedListHdr_s *list)
{
	if(list == NULL) return;

	PrintDebug("%s\n",(char *)list->p);
	RTSP_LIB_DumpLinkedList(list->next);
}
#endif


/*
 * file utils
 */



void RTSP_LIB_DumpByteBuffer(const void *str,HINT32 size)
{
#if 0
	HINT32 i = 0;
	PrintEnter;
	for(; i < size ; i++)
	{
		PrintDebug("%02x ",((unsigned char *)str)[i]);
		if(i % 16 == 15)
		{
			HINT32 j = i - 15;
			PrintDebug("	|");
			for(; j <= i ; j++ )
			{
				char c = ((char *)str)[j];
				if(VK_isprint(c))
					PrintDebug("%c",c);
				else
					PrintDebug(" ");
			}
			PrintDebug("|\n");
		}
	}
	PrintExit;
#else
	UNUSED_PARAM(str);
	UNUSED_PARAM(size);
#endif
}


/* sdp common functions */
HBOOL RTSP_LIB_ThisLineHasKeyword(char *line,char *keyword,char **buf,HINT32 *len)
{
	HINT32 newlen;

	PrintEnter;

	if ((line == NULL) || (keyword == NULL))
	{
		PrintError("%s(%d) Error> Invalid arg\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	if (!VK_strncasecmp(line,keyword,VK_strlen(keyword)))
	{
		line += VK_strlen(keyword);
		newlen = VK_strchr(line,'\n') ? (HINT32)(VK_strchr(line,'\n') - line) : (HINT32)VK_strlen(line);
		if (line[newlen - 1] == '\r')
		{
			newlen--;
		}

		if (*line == '"')
		{
			char *last;
			last = line + newlen;
			while(*last != '"') last--;
			newlen = last - line - 1; /* 1 is for " */
			line++; /* points to inside "foo" */
		}

		if (*len < newlen + 1)
		{
			*buf = (char *)RTSP_LIB_Realloc(*buf,newlen + 1);
			*len = newlen + 1;
		}

		VK_memcpy(*buf,line,newlen);
		(*buf)[newlen] = 0;
		/* PrintDebug("RTSP_LIB_ThisLineHasKeyword %s newlen = %d\n%s\n",keyword,newlen,*buf); */
		PrintExit;
		return TRUE;
	}

	PrintExit;
	return FALSE;
}

char *RTSP_LIB_GetNextLine(char *data)
{
	PrintEnter;
	if (data == NULL)
	{
		PrintError("%s(%d) Error> Invalid arg\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	data = VK_strchr(data,'\n');
	if(data) data++;
	PrintExit;
	return data;
}

HINT32 RTSP_LIB_ShrinkBufferIfPossible(char **buf,HINT32 *buflen,HINT32 keep)
{
	PrintEnter;
	if(*buflen < keep) {
		*buflen = keep;
		*buf = RTSP_LIB_Realloc(*buf,keep);

		PrintExit;
		return keep;
	}
	else
	{
		PrintExit;
		return *buflen;
	}
}


/* channel manage functions */

/*
 * setup RTSP_ACCESS_SYS_s
 */
static struct RTSP_ACCESS_SYS_s *P_RTSP_LIB_NewRtspAccessSys(void)
{
	struct RTSP_ACCESS_SYS_s *st = NULL;
	PrintEnter;
	st = (struct RTSP_ACCESS_SYS_s *)DD_MEM_Alloc(sizeof(struct RTSP_ACCESS_SYS_s));
	if (st == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_memset(st,0,sizeof(struct RTSP_ACCESS_SYS_s));
	PrintExit;
	return st;
}


struct RTSP_ACCESS_SYS_s *RTSP_LIB_InitRtspAccessSys(void)
{
	struct RTSP_ACCESS_SYS_s *stream = NULL;

	PrintEnter;

	stream = P_RTSP_LIB_NewRtspAccessSys();
	if (stream == NULL)
	{
		PrintError("%s(%d) Error> Can't get access\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	stream->p_connectHost = NULL;
	stream->i_connectPort = 0;
	stream->p_host = NULL;
	stream->i_port = 0;
	stream->p_proxyHost = NULL;
	stream->i_proxyPort = 0;

	stream->ctrlChannel = RTSP_LIB_NewChannel();
	if (stream->ctrlChannel == NULL)
	{
		PrintError("%s(%d) Error> Can't get ctrl channel\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	stream->stream_ctrl = RTSP_LIB_NewRtspCtrl();
	if (stream->stream_ctrl == NULL)
	{
		PrintError("%s(%d) Error> Can't get stream ctrl\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	PrintExit;
	return stream;

func_error:
	PrintError("%s(%d) Error> function failed\n", __FUNCTION__, __LINE__);

	if (stream != NULL)
	{
#ifdef __RTSP_THIS_IS_UNUSED_VARS__
		if (stream->localfile)
		{
			DD_MEM_Free(stream->localfile);
			stream->localfile = NULL;
		}
#endif

		if (stream->stream_ctrl)
		{
			RTSP_LIB_FreeRtspCtrl(stream->stream_ctrl);
			stream->stream_ctrl = NULL;
		}

		if (stream->ctrlChannel)
		{
			RTSP_LIB_FreeChannel(stream->ctrlChannel);
			stream->ctrlChannel = NULL;
		}

		if (stream->dataChannel[2])
		{
			RTSP_LIB_FreeChannel(stream->dataChannel[2]);
			stream->dataChannel[2] = NULL;
		}

		stream->p_connectHost = NULL; /* wether host or proxy host */
		if (stream->p_host)
		{
			DD_MEM_Free(stream->p_host);
			stream->p_host = NULL;
		}
		if (stream->p_proxyHost)
		{
			DD_MEM_Free(stream->p_proxyHost);
			stream->p_proxyHost = NULL;
		}

		DD_MEM_Free(stream);
		stream = NULL;
	}
	PrintExit;
	return NULL;
}


void RTSP_LIB_FreeRtspAccessSys(struct RTSP_ACCESS_SYS_s *stream)
{
	PrintEnter;
	if(stream == NULL)
	{
		PrintError("%s(%d) Error> arg is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

#ifdef __RTSP_THIS_IS_UNUSED_VARS__
	if(stream->localfile)
	{
		DD_MEM_Free(stream->localfile);
		stream->localfile = NULL;
	}
#endif

	if(stream->stream_ctrl)
	{
		RTSP_LIB_FreeRtspCtrl(stream->stream_ctrl);
		stream->stream_ctrl = NULL;
	}

	if (stream->ctrlChannel)
	{
		RTSP_LIB_FreeChannel(stream->ctrlChannel);
		stream->ctrlChannel = NULL;
	}

	if (stream->dataChannel[2])
	{
		RTSP_LIB_FreeChannel(stream->dataChannel[2]);
		stream->dataChannel[2] = NULL;
	}

	stream->p_connectHost = NULL; /* wether host or proxy host */
	if (stream->p_host)
	{
		DD_MEM_Free(stream->p_host);
		stream->p_host = NULL;
	}
	if (stream->p_proxyHost)
	{
		DD_MEM_Free(stream->p_proxyHost);
		stream->p_proxyHost = NULL;
	}

	if(stream)
	{
		DD_MEM_Free(stream);
		stream = NULL;
	}
	PrintExit;
	return;
}


/*
 * setup buffer, setup URL.
 */
struct RTSP_CTRL_s *RTSP_LIB_NewRtspCtrl(void)
{
	struct RTSP_CTRL_s *sc = NULL;
	PrintEnter;
	sc = (struct RTSP_CTRL_s *)DD_MEM_Alloc(sizeof(struct RTSP_CTRL_s));
	if (sc == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_memset(sc,0,sizeof(struct RTSP_CTRL_s));

	sc->write_buffer = DD_MEM_Alloc(BUF_SIZE);
	if (sc->write_buffer == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		if (sc != NULL) /* CID #46207 */
		{
			DD_MEM_Free(sc);
			sc = NULL;
		}
		return NULL;
	}
	VK_memset(sc->write_buffer,0,BUF_SIZE);
	sc->write_buffer_size = BUF_SIZE;
	PrintExit;
	return sc;
}

struct RTSP_CHANNEL_t *RTSP_LIB_NewChannel(void)
{
	struct RTSP_CHANNEL_t *pChannel = NULL;

	PrintEnter;

	pChannel = (struct RTSP_CHANNEL_t *)DD_MEM_Alloc(sizeof(struct RTSP_CHANNEL_t));
	if (pChannel == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_memset(pChannel,0,sizeof(struct RTSP_CHANNEL_t));

	pChannel->p_buffer = DD_MEM_Alloc(BUF_SIZE);
	if (pChannel->p_buffer == NULL)
	{
		PrintError("%s(%d) Error> Out of memory\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(pChannel->p_buffer ,0, BUF_SIZE);

#if 0
	pChannel->i_bufferSize = BUF_SIZE; /* 102,400 */
#endif
	pChannel->i_dataLen = 0;
	pChannel->i_bufferPos = 0;

	PrintExit;
	return pChannel;

func_error:
	if (pChannel != NULL)
	{
		if (pChannel->p_buffer != NULL)
		{
			DD_MEM_Free(pChannel->p_buffer);
			pChannel->p_buffer = NULL;
		}

		DD_MEM_Free(pChannel);
		pChannel = NULL;
	}
	PrintExit;
	return NULL;
}


void RTSP_LIB_FreeChannel(struct RTSP_CHANNEL_t *pChannel)
{
	PrintEnter;

	if (pChannel == NULL)
	{
		PrintError("%s(%d) Error> argument NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	if (pChannel->p_buffer)
	{
		DD_MEM_Free(pChannel->p_buffer);
		pChannel->p_buffer = NULL;
	}
	DD_MEM_Free(pChannel);
	pChannel = NULL;
	PrintExit;
}


/*
 * free stream_ctrl;
 */
void RTSP_LIB_FreeRtspCtrl(struct RTSP_CTRL_s *sc)
{
	PrintEnter;
	if (sc == NULL)
	{
		PrintError("%s(%d) Error> argument NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	/* if(sc->url) free_url_t(sc->url); not malloc()ed in RTSP_LIB_NewRtspCtrl,
	   thus, free this outside RTSP_LIB_FreeRtspCtrl().*/
	if(sc->write_buffer)
	{
		DD_MEM_Free(sc->write_buffer);
		sc->write_buffer = NULL;
	}
	if(sc->retry_urlstr)
	{
		DD_MEM_Free(sc->retry_urlstr);
		sc->retry_urlstr = NULL;
	}

	if (sc->pSdpContext)
	{
		DD_MEM_Free(sc->pSdpContext);
		sc->pSdpContext = NULL;
	}

	if (sc->pWmsSdpCtx)
	{
		DD_MEM_Free(sc->pWmsSdpCtx);
		sc->pWmsSdpCtx = NULL;
	}

	DD_MEM_Free(sc);
	sc = NULL;
	PrintExit;
}

/*
 * connect to 'servername' with 'port'. only for control channel
 *		   return value :	socket number ... success
 *							 -1			... error
 */
HINT32 RTSP_LIB_ConnectTCPChannel(const char *servername,const HINT32 port)
{
	HINT32 ret = 0;
	PrintEnter;
	ret = net_ConnectTCP(servername, port);
	PrintExit;
	return ret;
}


/*
 * this is wrapper for recv, and has timeout.
 * return value:			length --> success
 *							  -1 --> error/timeout
 *							   0 --> eof
 */
 /* No Wait All */
static HINT32 P_RTSP_LIB_SocketRead(HINT32 sock,void *buf,size_t count)
{
	return (int)net_Read(sock,NULL,buf,count,FALSE);
}


/*
 * recv count bytes of data from sock to buf.
 * this is only used when count bytes of data are
 * supposed to come from sock.
 * this function must be used when 'count' bytes are guaranteed
 * to be received.
 *		  return value: length --> success.
 *						0	  --> eof
 *						-1	 --> timeout or error (fatal)
 */
 /* read with wait all */
HINT32 RTSP_LIB_SockReadWithWaitAll(HINT32 sock,void *buf,size_t count)
{
	HINT32 len = 0;
	size_t total = 0;
	HINT32 nRetryCount = 3;
	PrintEnter;

	while(total < count)
	{
		len = P_RTSP_LIB_SocketRead(sock,(HUINT8 *)buf + total,count - total);
		if (len < 0)
		{
			PrintError("%s(%d) Error> Retrying time outed requested bytes = %d\n", __FUNCTION__, __LINE__, count);
			nRetryCount--;

			if(nRetryCount > 0)
			{
				continue;
			}
			else
			{
				goto failed;
			}
		}
		else if (len == 0)  /* EOF */
		{
			PrintError("%s(%d) Error> met EOF when %d bytes are to come\n", __FUNCTION__, __LINE__, count);
			goto meteof;
		}

		total += len;
	}

	PrintExit;
	return total;

meteof:
	PrintExit;
	return 0;

failed:
	PrintExit;
	return (-1);
}


/*
 * send() wrapper
 */
HINT32 RTSP_LIB_SockWrite(HINT32 sock,void *buf,size_t count)
{
	return (int)net_Write(sock,NULL,buf,count);
}


static HINT32 P_RTSP_LIB_SelectSocket(HINT32 sock,HINT32 timeout)
{
	fd_set fds;
	struct timeval tv;
	HINT32 ret = 0;

	PrintEnter;

	if (sock < 0)
	{
		PrintError("%s(%d) Error> Invalid socket(%d)\n", __FUNCTION__, __LINE__, sock);
		;
	}

	FD_ZERO(&fds);
	FD_SET(sock,&fds);

	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	ret = select(sock + 1,&fds,NULL,NULL,&tv);
	if (ret == -1)
	{
		PrintError("%s(%d) Error> select() failed\n", __FUNCTION__, __LINE__);
		;
	}

	PrintExit;
	return ret;
}


HINT32 RTSP_LIB_SelectCtrlChannel(struct RTSP_ACCESS_SYS_s *stream, HINT32 timeout)
{
	PrintEnter;

	if (stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid arg\n", __FUNCTION__, __LINE__);
		return 0;
	}

	if (stream->ctrlChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL ctrl channel!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	if (stream->ctrlChannel->i_dataLen) {
		return 1;
	}

	return P_RTSP_LIB_SelectSocket(stream->ctrlChannel->i_sock,timeout);
}

HINT32 RTSP_LIB_ConnectRTPChannel(struct RTSP_CHANNEL_t *pChannel, char *pBindHost, HINT32 iBindPort, HBOOL isRTP)
{
	HINT32 nret = 0;

	PrintEnter;

	if ((pChannel == NULL) || (pBindHost == NULL))
	{
		PrintError("%s(%d) Error> Invalid NULL channel!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	PrintDebug("%s(%d) Bind[UDP/RTP:%d, Host:%s, Port:%d]\n", __FUNCTION__, __LINE__, isRTP, pBindHost, iBindPort);

	if (isRTP == TRUE)
	{
		nret = pChannel->i_sock = net_ConnectRTP(pBindHost, iBindPort);
	}
	else
	{
		nret = pChannel->i_sock = net_ConnectUDP(pBindHost, iBindPort, 1024);
	}

	if (pChannel->i_sock > 0)
	{
		net_RTPTuneSocket(pChannel->i_sock);
	}

func_error:
	PrintExit;
	return nret;
}


/*
 * read 'size' bytes from *resources*.
 *
 *	   *resources* are : [in this order]
 *					1. channel->buffer (data which came with http header)
 *					2. channel->sock   (network)
 *
 *			   return value : size which read.
 *							  -1 if get_data failed.
 */
HINT32 RTSP_LIB_ClearChannelBuffer(struct RTSP_CHANNEL_t *pChannel)
{
	PrintEnter;

	if (pChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL channel!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	pChannel->i_dataLen = 0;
	pChannel->i_bufferPos = 0;
	PrintExit;
	return 0;

func_error:
	PrintExit;
	return 0;
}

HINT32 RTSP_LIB_ReadADgramAndFillChannelBuffer(struct RTSP_CHANNEL_t *pDataChannel)
{
	PrintEnter;

	if (pDataChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL channel!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	(void)RTSP_LIB_ClearChannelBuffer(pDataChannel);

	if ((pDataChannel->transport_type == RTSP_TRANSPORT_UDP_UNICAST) ||
		(pDataChannel->transport_type == RTSP_TRANSPORT_RTP_UNICAST)) /* UDP MODE -> 모든 것을 channel buffer를 통해 처리*/
	{
		/* get_data로 전환 가능 */
		pDataChannel->i_dgramRead = net_Read(pDataChannel->i_sock, NULL,(HUINT8 *)pDataChannel->p_buffer, _RTSP_MAX_DGRAM_SIZE, FALSE);
		pDataChannel->i_dataLen = pDataChannel->i_dgramRead;
	}
	else
	{
		PrintError("%s(%d) Error> Transport protocol(%d) must be UDP or RTP\n", __FUNCTION__, __LINE__, pDataChannel->transport_type);
	}
	PrintExit;
	return 0;

func_error:
	PrintExit;
	return 0;
}

/* ctrl and data channel */
HINT32 RTSP_LIB_ReadFromChannel(struct RTSP_CHANNEL_t *pChannel, void *buffer, size_t size)
{
	HINT32 i_len = 0; /* how many bytes are stored in 'buffer' */
	HINT32 nret = 0;

	PrintEnter;

	if (pChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL channel!\n", __FUNCTION__, __LINE__);
		return 0;
	}
	PrintDebug("%s(%d) read from channel buffer size:%u, i_dataLen:%d\n", __FUNCTION__, __LINE__, size, pChannel->i_dataLen);

	/*PrintDebug("read_data size = %d	pChannel->i_dataLen = %d,"
	  " pChannel->i_bufferPos = %d\n",
	  size,pChannel->i_dataLen, pChannel->i_bufferPos);*/

	i_len = 0;
	if (pChannel->i_dataLen)
	{
		/* there is a data to read in channel->buffer */
		i_len = (pChannel->i_dataLen > size) ? size : pChannel->i_dataLen; /* select smaller */

		/* read push_backed data */
		PrintDebug("%s(%d) VK_memcpy  buffer pos %u count %d datalen %u\n",
				__FUNCTION__, __LINE__, pChannel->i_bufferPos, i_len, pChannel->i_dataLen);

		VK_memcpy((HUINT8 *)buffer,pChannel->p_buffer + pChannel->i_bufferPos,i_len);
		pChannel->i_bufferPos += i_len;
		pChannel->i_dataLen -= i_len;

		if (pChannel->i_dataLen == 0) {
			pChannel->i_bufferPos = 0;
		}
	}

	if (i_len < (HINT32)size)
	{
		if ((pChannel->transport_type == RTSP_TRANSPORT_UDP_UNICAST) ||
			(pChannel->transport_type == RTSP_TRANSPORT_RTP_UNICAST))  /* UDP mode */
		{
			PrintError("%s(%d) Error> you need to fill the channel buffer first\n", __FUNCTION__, __LINE__);
			nret = 0;
		}
		else if (pChannel->transport_type == RTSP_TRANSPORT_TCP)
		{
			/* read data with wait all */
			nret = RTSP_LIB_SockReadWithWaitAll(pChannel->i_sock,(HUINT8 *)buffer + i_len, size - i_len);
		}
		else
		{
			PrintError("%s(%d) Error> Unsupported transport protocol(%d)\n", __FUNCTION__, __LINE__, pChannel->transport_type);
			return (-1);
		}

		if (nret < 0) { /* get_data mets timeout/error */
			PrintExit;
			return (-1);
		}
		else {
			i_len += nret;
		}
	}

	PrintExit;
	return i_len;
}


/* read with waitall*/
HINT32 RTSP_LIB_ReadFromCtrlChannelWithWaitAll(struct RTSP_ACCESS_SYS_s *stream, void *buffer, size_t size)
{
	PrintEnter;
	return RTSP_LIB_ReadFromChannel(stream->ctrlChannel, buffer, size);
}


/*
 * different from read_data, this function's 3ed argument is
 * max byte to read from stream.
 * return value:			 len ... bytes stored in buffer
 *							 0 ...
 *							-1 ... timeout
 */
/* read with no waitall */
HINT32 RTSP_LIB_ReadFromCtrlChannelWithNoWait(struct RTSP_ACCESS_SYS_s *stream,void *buffer,size_t max)
{
	struct RTSP_CHANNEL_t *ctrlChannel = NULL;
	HINT32 len = 0; /* how many bytes are stored in 'buffer' */
	HINT32 ret = 0;

	PrintEnter;

	if (stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp stream!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	if (stream->ctrlChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL ctrl channel!\n", __FUNCTION__, __LINE__);
		return 0;
	}
	ctrlChannel = stream->ctrlChannel;

	len = ctrlChannel->i_dataLen;
	if (len)
	{
		/* there is a data to read in control channel->buffer */
		len = ((HINT32)max < len) ? (HINT32)max : len; /* smaller */
		VK_memcpy((HUINT8 *)buffer,ctrlChannel->p_buffer + ctrlChannel->i_bufferPos,len);

		ctrlChannel->i_bufferPos += len;
		ctrlChannel->i_dataLen -= len;

		if (ctrlChannel->i_dataLen == 0)
		{
			ctrlChannel->i_bufferPos = 0;
		}
	}

	/* still network stream can be read. */
	if (len < (HINT32)max)
	{
		if (!len || P_RTSP_LIB_SelectSocket(ctrlChannel->i_sock,0)) /* just check there are some data now */
		{
			ret = P_RTSP_LIB_SocketRead(ctrlChannel->i_sock,(HUINT8 *)buffer + len,max - len);
			if (ret < 0) {
				PrintExit;
				return (-1);
			}
			else
			{
				len += ret;
				/*
				   when sock was shut down by server and nothing was in buffer,
				   len == 0 and ret == 0 so return 0, this is OK.
				   */
			}
		}
	}
	PrintExit;
	return len;
}

/* copy data to the head of control channel */
HINT32 RTSP_LIB_PutMessageForLaterRead(struct RTSP_ACCESS_SYS_s *stream,void *buffer,HINT32 size)
{
	struct RTSP_CHANNEL_t *ctrlChannel = NULL;

	PrintEnter;

	if (stream == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL rtsp stream!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	if (stream->ctrlChannel == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL ctrl channel!\n", __FUNCTION__, __LINE__);
		return 0;
	}
	ctrlChannel = stream->ctrlChannel;

	/*기존의 내용을 뒤로 미룬다.*/
	if (ctrlChannel->i_dataLen) {
		memmove(
				ctrlChannel->p_buffer + ctrlChannel->i_bufferPos + size, /* target */
				ctrlChannel->p_buffer + ctrlChannel->i_bufferPos, /* src */
				ctrlChannel->i_dataLen);

	}

	/* 앞에 복사 */
	VK_memcpy(ctrlChannel->p_buffer + ctrlChannel->i_bufferPos,buffer,size);
	ctrlChannel->i_dataLen += size;

	PrintExit;
	return (ctrlChannel->i_dataLen);
}


HINT32 RTSP_LIB_CheckMessageResponseOK(HINT32 status_code)
{
	PrintEnter;
	return ((200 <= status_code) && (status_code <= 299)) ? 1 : 0;
}

