/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_rtsp.h  $
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

#include "di_uart.h"
#include "mms_asf.h"
#include "rtsp_lib.h"

#include "stream_global.h"
#include "di_iptuner.h"
#include "util_url.h"
#include "stream_rtsp.h"
#include "transport_tcp.h"



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


#define MAX_DGRAM_SIZE 2000




/*
 * realloc() wrapper
 */
void *RTSP_LIB_Realloc(void *p,const size_t size)
{
	PrintEnter;
	p = DD_MEM_Realloc(p,size);
	if(!p) {
	PrintCriticalDebug("could not reallocate memory\n");
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
	struct RTSP_LinkedListHdr_s *newl;
	PrintEnter;
	newl = DD_MEM_Alloc(sizeof(struct RTSP_LinkedListHdr_s));
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
	for(cur = list ; cur ; cur = next) {
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
    #if 1 /* Fixed warning: cast discards qualifiers from pointer target type */
		PrintDebug("%02x ",((unsigned char *)str)[i]); // <--
    #else
		PrintDebug("%02x ",((unsigned char *)&str)[i]);
    #endif
		if(i % 16 == 15)
		{
			HINT32 j = i - 15;
			PrintDebug("	|");
			for(; j <= i ; j++ )
			{
    #if 1 /* Caution to warning: cast discards qualifiers from pointer target type */
				char c = ((char *)str)[j];
    #else
				char c = ((char *)&str)[j];
    #endif
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
  PrintEnter;
	if(!VK_strncasecmp(line,keyword,VK_strlen(keyword))) {
	HINT32 newlen;

	line += VK_strlen(keyword);
	newlen = VK_strchr(line,'\n') ? (HINT32)(VK_strchr(line,'\n') - line) : (HINT32)VK_strlen(line);
	if(line[newlen - 1] == '\r')
		newlen--;

	if(*line == '"') {
		char *last;
		last = line + newlen;
		while(*last != '"') last--;
		newlen = last - line - 1; /* 1 is for " */
		line++; /* points to inside "foo" */
	}

	if(*len < newlen + 1) {
		*buf = (char *)RTSP_LIB_Realloc(*buf,newlen + 1);
		*len = newlen + 1;
	}

	VK_memcpy(*buf,line,newlen);
	(*buf)[newlen] = 0;
	/* PrintDebug ("RTSP_LIB_ThisLineHasKeyword %s newlen = %d\n%s\n",keyword,newlen,*buf); */
	PrintExit;
	return TRUE;
	}
	else
	{
	PrintExit;
	return FALSE;
	}
}



char *RTSP_LIB_GetNextLine(char *data)
{
	PrintEnter;
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
struct RTSP_ACCESS_SYS_s *RTSP_LIB_NewRtspAccessSys(void)
{
	struct RTSP_ACCESS_SYS_s *st;
	PrintEnter;
	st = (struct RTSP_ACCESS_SYS_s *)DD_MEM_Alloc(sizeof(struct RTSP_ACCESS_SYS_s));
	VK_memset(st,0,sizeof(struct RTSP_ACCESS_SYS_s));
	PrintExit;
	return st;
}





struct RTSP_ACCESS_SYS_s *RTSP_LIB_InitRtspAccessSys(void)
{
	struct RTSP_ACCESS_SYS_s *stream;
	PrintEnter;
	stream = RTSP_LIB_NewRtspAccessSys();
	stream->serverinfo = RTSP_LIB_NewServerInfo();
	stream->Channel = RTSP_LIB_NewChannel();
	stream->stream_ctrl = RTSP_LIB_NewRtspCtrl();


	PrintExit;
	return stream;
}


void RTSP_LIB_FreeRtspAccessSys(struct RTSP_ACCESS_SYS_s *stream)
{
	PrintEnter;
	if(stream == NULL)
	{
		PrintError("arg is NULL\n");
		return;
	}
	if(stream->localfile)
	{
		DD_MEM_Free(stream->localfile);
		stream->localfile = NULL;
	}

	if(stream->stream_ctrl)
	{
		RTSP_LIB_FreeRtspCtrl(stream->stream_ctrl);
		stream->stream_ctrl = NULL;
	}

	if(stream->Channel)
	{
		RTSP_LIB_FreeChannel(stream->Channel);
		stream->Channel = NULL;
	}

	if(stream->datasocket[2])
	{
		RTSP_LIB_FreeChannel(stream->datasocket[2]);
		stream->datasocket[2] = NULL;
	}

	if(stream->serverinfo)
	{
		RTSP_LIB_FreeServerInfo(stream->serverinfo);
		stream->serverinfo = NULL;
	}
	if(stream)
	{
		DD_MEM_Free(stream);
		stream = NULL;
	}
	PrintExit;

}


/*
 * setup buffer, setup URL.
 */
struct RTSP_CTRL_s *RTSP_LIB_NewRtspCtrl(void)
{

	struct RTSP_CTRL_s *sc;
	PrintEnter;
	sc = (struct RTSP_CTRL_s *)DD_MEM_Alloc(sizeof(struct RTSP_CTRL_s));
	VK_memset(sc,0,sizeof(struct RTSP_CTRL_s));

	sc->write_buffer = DD_MEM_Alloc(BUF_SIZE);
	VK_memset(sc->write_buffer,0,BUF_SIZE);
	sc->write_buffer_size = BUF_SIZE;
	PrintExit;
	return sc;
}



struct RTSP_SERVER_INFO_s *RTSP_LIB_NewServerInfo(void)
{
	struct RTSP_SERVER_INFO_s *si;
	PrintEnter;
	si = 	(struct RTSP_SERVER_INFO_s *)DD_MEM_Alloc(sizeof(struct RTSP_SERVER_INFO_s));
	VK_memset(si,0,sizeof(struct RTSP_SERVER_INFO_s));
	PrintExit;
	return si;
}


void RTSP_LIB_FreeServerInfo(struct RTSP_SERVER_INFO_s *si)
{
	PrintEnter;
	if(!si) return;

	si->connect_host = NULL; /* wether host or proxy host */
	if(si->host)
	{
		DD_MEM_Free(si->host);
		si->host = NULL;
	}
	if(si->proxy_host)
	{
		DD_MEM_Free(si->proxy_host);
		si->proxy_host = NULL;
	}
	DD_MEM_Free(si);
	si = NULL;
	PrintExit;
}



struct RTSP_CHANNEL_t *RTSP_LIB_NewChannel(void)
{
	struct RTSP_CHANNEL_t *ns;
	PrintEnter;
	ns = (struct RTSP_CHANNEL_t *)DD_MEM_Alloc(sizeof(struct RTSP_CHANNEL_t));
	VK_memset(ns,0,sizeof(struct RTSP_CHANNEL_t));
	ns->buffer = DD_MEM_Alloc(BUF_SIZE);
	VK_memset(ns->buffer ,0, BUF_SIZE);
	ns->buffer_size = BUF_SIZE;
	ns->data_len = 0;
	ns->buffer_pos = 0;
	PrintExit;
	return ns;
}


void RTSP_LIB_FreeChannel(struct RTSP_CHANNEL_t *ns)
{
	PrintEnter;
	if(!ns) return;

	if(ns->buffer)
	{
		DD_MEM_Free(ns->buffer);
		ns->buffer = NULL;
	}
	DD_MEM_Free(ns);
	ns = NULL;
	PrintExit;
}





/*
 * free stream_ctrl;
 */
void RTSP_LIB_FreeRtspCtrl(struct RTSP_CTRL_s *sc)
{
	PrintEnter;
	if(!sc) return;

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

	if(sc->pSdpCtx)
	{
		DD_MEM_Free(sc->pSdpCtx);
		sc->pSdpCtx = NULL;
	}

	DD_MEM_Free(sc);
	sc = NULL;
	PrintExit;
}



/*
 * set server information
 */
void RTSP_LIB_SetServerInfo(struct RTSP_SERVER_INFO_s *serverinfo,
			char *target_host,HINT32 target_port,
			char *proxy_host,HINT32 proxy_port,HINT32 protocol_default_port)
{
	PrintEnter;
	if(!serverinfo) {
	PrintCriticalDebug("RTSP_LIB_SetServerInfo: argument NULL\n");
	return;
	}

	serverinfo->host = VK_StrDup(target_host);
	serverinfo->port = (target_port) ? target_port : protocol_default_port;

	serverinfo->connect_host = serverinfo->host;
	serverinfo->connect_port = serverinfo->port;

	if(proxy_host) {
	serverinfo->proxy_host = VK_StrDup(proxy_host);
	serverinfo->proxy_port = proxy_port;

	serverinfo->connect_host = serverinfo->proxy_host; /* reset host to connect() */
	serverinfo->connect_port = serverinfo->proxy_port; /* reset port to connect() */
	}
	else {
	serverinfo->proxy_host = NULL;
	serverinfo->proxy_port = 0;
	}
	PrintExit;
}





/*
 * connect to 'servername' with 'port'. only for control channel
 *		   return value :	socket number ... success
 *							 -1			... error
 */

HINT32 RTSP_LIB_NetConnectTCP(const char *servername,const HINT32 port)
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
HINT32 RTSP_LIB_SockRead(HINT32 sock,void *buf,size_t count)
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
	HINT32 len;
	size_t total = 0;
	HINT32 nRetryCount = 3;
	PrintEnter;

	while(total < count) { /* more data to recv. */

	len = RTSP_LIB_SockRead(sock,(HUINT8 *)buf + total,count - total);

	if(len < 0) {	   /* Error. timeout, syscall error */
		PrintCriticalDebug("Retrying time outed requested bytes = %d\n",count);
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
	else if(len == 0) { /* EOF */
		PrintCriticalDebug("met EOF when %d bytes are to come\n",count);
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
	return -1;
}


/*
 * send() wrapper
 */
HINT32 RTSP_LIB_SockWrite(HINT32 sock,void *buf,size_t count)
{
	return (int)net_Write(sock,NULL,buf,count);
}



HINT32 RTSP_LIB_SelectSocket(HINT32 sock,HINT32 timeout)
{

	fd_set fds;
	struct timeval tv;
	HINT32 ret;

	PrintEnter;

	FD_ZERO(&fds);
	FD_SET(sock,&fds);

	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	ret = select(sock + 1,&fds,NULL,NULL,&tv);

	if(ret == -1) {
	PrintDebug("RTSP_LIB_SelectCtrlChSocket: select() failed");
	}

	PrintExit;
	return ret;
}



HINT32 RTSP_LIB_SelectCtrlChSocket(struct RTSP_ACCESS_SYS_s *stream,HINT32 timeout)
{
	PrintEnter;
	if(stream->Channel->data_len) {
	return 1;
	}

	return RTSP_LIB_SelectSocket(stream->Channel->sock,timeout);
}




HINT32 RTSP_LIB_ConnectUDPChannel(struct RTSP_CHANNEL_t *Channel)
{
	HINT32 ret = 0;
	PrintEnter;
#if 1 /* warning: unused parameter 'Channel' */
	UNUSED_PARAM(Channel);
#else
	/* net_SetMcastHopLimit */
	/* ret = Channel->sock = net_ConnectUDP(servername, port, 1024); */
#endif
	PrintExit;
	return ret;
}



/*
 * read 'size' bytes from *resources*.
 *
 *	   *resources* are : [in this order]
 *					1. Channel->buffer (data which came with http header)
 *					2. Channel->sock   (network)
 *
 *			   return value : size which read.
 *							  -1 if get_data failed.
 */



HINT32 RTSP_LIB_ClearChannelBuffer(struct RTSP_CHANNEL_t *Channel)
{
	PrintEnter;
	Channel->data_len = 0;
	Channel->buffer_pos = 0;
	PrintExit;
	return 0;
}

HINT32 RTSP_LIB_ReadADgramAndFillChannelBuffer(struct RTSP_CHANNEL_t *Channel)
{
	PrintEnter;
	(void)RTSP_LIB_ClearChannelBuffer(Channel);

	if(Channel->is_udp == 1) { /* UDP MODE -> 모든 것을 channel buffer를 통해 처리*/

		/* get_data로 전환 가능 */
		Channel->dgram_size = net_Read(Channel->sock, NULL,(HUINT8 *)Channel->buffer, MAX_DGRAM_SIZE, FALSE);
		Channel->data_len = Channel->dgram_size;
	}
	else
	{
		PrintError( "You must be udp\n");
	}
	PrintExit;
	return 0;
}



HINT32 RTSP_LIB_ReadFromChannel(struct RTSP_CHANNEL_t *Channel, void *buffer, size_t size)
{
	HINT32 len; /* how many bytes are stored in 'buffer' */
	HINT32 ret;

	PrintEnter;

	PrintDebug( "read from channel buffer %u\n", size);


	/*PrintDebug ("read_data size = %d	Channel->data_len = %d,"
	  " Channel->buffer_pos = %d\n",
	  size,Channel->data_len, Channel->buffer_pos);*/

	len = 0;
	if(Channel->data_len) {
	/* there is a data to read in Channel->buffer */

	len = (size < Channel->data_len)
		? size : Channel->data_len; /* smaller */

	/* read push_backed data */
	PrintDebug( "VK_memcpy  buffer pos %u count %d datalen %u\n", Channel->buffer_pos,
	len, Channel->data_len);

	VK_memcpy((HUINT8 *)buffer,Channel->buffer + Channel->buffer_pos,len);

	Channel->buffer_pos += len;
	Channel->data_len -= len;

	if(Channel->data_len == 0) {
		Channel->buffer_pos = 0;
	}
	}

	if(len < (HINT32)size) {
		if(Channel->is_udp == 1) /* UDP mode */
		{
			PrintCriticalDebug( "you need to fill the channel buffer first\n");
			ret = 0;
		}
		else
		{
			/* read data with wait all */
			ret = RTSP_LIB_SockReadWithWaitAll(Channel->sock,(HUINT8 *)buffer + len, size - len);
		}

		if(ret < 0) { /* get_data mets timeout/error */
			PrintExit;
			return -1;
		}
		else {
			len += ret;
		}
	}

	PrintExit;

	return len;


}




/* read with waitall*/
HINT32 RTSP_LIB_ReadFromCtrlChannelWithWaitAll(struct RTSP_ACCESS_SYS_s *stream, void *buffer, size_t size)
{
	PrintEnter;
	return RTSP_LIB_ReadFromChannel(stream->Channel, buffer, size);
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
	struct RTSP_CHANNEL_t *Channel = stream->Channel;
	HINT32 len; /* how many bytes are stored in 'buffer' */
	HINT32 ret;
	PrintEnter;

	if((len = Channel->data_len)) {
	/* there is a data to read in Channel->buffer */

	len = ((HINT32)max < len) ? (HINT32)max : len; /* smaller */

	VK_memcpy((HUINT8 *)buffer,Channel->buffer + Channel->buffer_pos,len);

	Channel->buffer_pos += len;
	Channel->data_len -= len;

	if(Channel->data_len == 0) {
		Channel->buffer_pos = 0;
	}
	}

	/* still network stream can be read. */
	if(len < (HINT32)max) {
	if(!len || RTSP_LIB_SelectSocket(Channel->sock,0)) {/* just check there are some data now */
		ret = RTSP_LIB_SockRead(Channel->sock,(HUINT8 *)buffer + len,max - len);
		if(ret < 0) {
			PrintExit;
		return -1;
		}
		else {
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



/* copy data to the head of Channel */
HINT32 RTSP_LIB_PutDataForLaterRead(struct RTSP_ACCESS_SYS_s *stream,void *buffer,HINT32 size)
{
	struct RTSP_CHANNEL_t *Channel = stream->Channel;
	PrintEnter;

  /*기존의 내용을 뒤로 미룬다.*/
	if(Channel->data_len) {
	memmove(
		Channel->buffer + Channel->buffer_pos + size, /* target */
		Channel->buffer + Channel->buffer_pos, /* src */
		Channel->data_len);

	}

	/* 앞에 복사 */
	VK_memcpy(Channel->buffer + Channel->buffer_pos,buffer,size);
	Channel->data_len += size;

  PrintExit;
	return (Channel->data_len);
}



