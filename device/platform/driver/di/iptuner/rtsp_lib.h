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


#ifndef __RTSPLIB_H__
#define __RTSPLIB_H__







#include "htype.h"
#include "vkernel.h"
#include "util_url.h"






#define TRUE 1
#define FALSE 0

#define UNUSED_PARAM(x)  ((void)x)





/*
 * functions to handle bitorder
 * 
 * bswap_* : swap upper and lower bits
 * put*_*  : put number to p in specified bitorder
 * get*_*  : get number from p in specified bitorder
 */


static inline HUINT16 __byteswap_16(HUINT16 x)
{
	return (x>>8) | (x<<8);
}

static inline HUINT32 __byteswap_32(HUINT32 x)
{
	x= ((x<<8)&0xFF00FF00) | ((x>>8)&0x00FF00FF);
	return (x>>16) | (x<<16);
}

static inline HUINT64 __byteswap_64(HUINT64 x)
{
	x= ((x<< 8)&0xFF00FF00FF00FF00ULL) | ((x>> 8)&0x00FF00FF00FF00FFULL);
	x= ((x<<16)&0xFFFF0000FFFF0000ULL) | ((x>>16)&0x0000FFFF0000FFFFULL);
	return (x>>32) | (x<<32);
}

static inline void put32_be(void *p,HUINT32 x)
{
	*((HUINT8 *)p	) = x >> 24 & 0xff;
	*((HUINT8 *)p + 1) = x >> 16 & 0xff;
	*((HUINT8 *)p + 2) = x >>  8 & 0xff;
	*((HUINT8 *)p + 3) = x	   & 0xff;
}

static inline void put32_le(void *p,HUINT32 x)
{
	*((HUINT8 *)p	) = x	   & 0xff;
	*((HUINT8 *)p + 1) = x >>  8 & 0xff;
	*((HUINT8 *)p + 2) = x >> 16 & 0xff;
	*((HUINT8 *)p + 3) = x >> 24 & 0xff;
}

static inline void put16_be(void *p,HUINT16 x)
{
	*((HUINT8 *)p	) = x >>8   & 0xff;
	*((HUINT8 *)p + 1) = x	   & 0xff;
}

static inline void put16_le(void *p,HUINT16 x)
{
	*((HUINT8 *)p	) = x	   & 0xff;
	*((HUINT8 *)p + 1) = x >>8   & 0xff;
}

static inline void put8(void *p,HUINT8 x)
{
	*(HUINT8 *)p	   = x	   & 0xff;
}


static inline HUINT32 get32_be(void *p)
{
	return *(HUINT8 *)p << 24 | *((HUINT8 *)p + 1) << 16 |
	*((HUINT8 *)p + 2) << 8 | *((HUINT8 *)p + 3);
}

static inline HUINT32 get32_le(void *p)
{
	return *(HUINT8 *)p | *((HUINT8 *)p + 1) << 8 |
	*((HUINT8 *)p + 2) << 16 | *((HUINT8 *)p + 3) << 24;
}

static inline HUINT64 get64_be(void *p)
{
	return (HUINT64)get32_be((HUINT8 *)p) << 32 | (HUINT64)get32_be((HUINT8 *)p + 4);
}

static inline HUINT64 get64_le(void *p)
{
	return (HUINT64)get32_le((HUINT8 *)p + 4) << 32 | (HUINT64)get32_le((HUINT8 *)p);
}

static inline HUINT16 get16_be(void *p)
{
	return *(HUINT8 *)p << 8 | *((HUINT8 *)p + 1);
}

static inline HUINT16 get16_le(void *p)
{
	return *(HUINT8 *)p | *((HUINT8 *)p + 1) << 8;
}

static inline HUINT8 get8(void *p)
{
	return *(HUINT8 *)p;
}

/* get n bytes little endian */
static inline HUINT64 getnb_le(void *p,HINT32 n)
{
	HUINT64 val = 0;
	for(; n > 0 ; n--) {
	val <<= 8;
	val += *((HUINT8 *)p + n - 1);
	}
	return val;
}

/* get n bytes big endian */
static inline HUINT64 getnb_be(void *p,HINT32 n)
{
	HUINT64 val = 0;
	HINT32 i = 0;
	for(; i < n; i++) {
	val <<= 8;
	val += *((HUINT8 *)p + i);
	}
	return val;
}





/*
 * Big/Little Endian to Machine Endian
 */
#ifdef WORDS_BIGENDIAN

#define be2me_16(x) (x)
#define be2me_32(x) (x)
#define be2me_64(x) (x)
#define le2me_16(x) __byteswap_16(x)
#define le2me_32(x) __byteswap_32(x)
#define le2me_64(x) __byteswap_64(x)

#else /* LITTLE ENDIAN */

#define be2me_16(x) __byteswap_16(x)
#define be2me_32(x) __byteswap_32(x)
#define be2me_64(x) __byteswap_64(x)
#define le2me_16(x) (x)
#define le2me_32(x) (x)
#define le2me_64(x) (x)

#endif


/* list head */
struct RTSP_LinkedListHdr_s {
	void *p;
	struct RTSP_LinkedListHdr_s *next;
	struct RTSP_LinkedListHdr_s *prev;
};



char *xstrcasechr(const char *s1,HINT32 c);
char *xstrstr(const char *s1,const char *s2);
char *xstrcasestr(const char *s1,const char *s2);



void *RTSP_LIB_Realloc(void *p,const size_t size);


/* list  */
struct RTSP_LinkedListHdr_s *RTSP_LIB_AppendToLinkedListHdr(struct RTSP_LinkedListHdr_s **list,void *p);
struct RTSP_LinkedListHdr_s *RTSP_LIB_SearchFromLinkedListHdr(struct RTSP_LinkedListHdr_s *list,void *p,
				 HINT32 (*comp)(void *,void *));
HINT32 RTSP_LIB_LinkedListCount(struct RTSP_LinkedListHdr_s *list);
struct RTSP_LinkedListHdr_s *RTSP_LIB_NewLinkedListHdr(void *p);
void RTSP_LIB_FreeLikedList(struct RTSP_LinkedListHdr_s *list,void (*free_er)(void *));

HINT32 get_filesize(const char *path,HUINT64 *size);
void RTSP_LIB_DumpByteBuffer(const void *str,HINT32 size);

/* sdp common functions */
HBOOL RTSP_LIB_ThisLineHasKeyword(char *line,char *keyword,char **buf,HINT32 *len);
char *RTSP_LIB_GetNextLine(char *data);
HINT32 RTSP_LIB_ShrinkBufferIfPossible(char **buf,HINT32 *buflen,HINT32 keep);

/* channel management functions */

/*
 * default ports. MMSH is MMS over HTTP, so HTTP port is used.
 */

#define	RTSP_PORT  554
#define 	RTSP_MAX_DATA_SOCKET 8


/*
 * protocol type listing
 */
enum {
	UNKNOWN_PROTOCOL = 0, /* unknown protocol					*/
	MMST,			 /* mms over tcp						*/
	MMSH,			 /* mms over http					   */
	HTTP,			 /* http								*/
	RTSP,			 /* rtsp (real time streaming protocol) */
	FTP,			  /* ftp								 */
	RTSP_REAL,		/* rtsp - real /helix				  */
	RTSP_WMS,		 /* rtsp - windows media server		 */
};



/*
 * buffer sizes. don't touch BUF_SIZE !!!
 */
enum {
	BUF_SIZE = 102400,	/* buffer for Channel->buffer, write_buffer */
	BUFSIZE_1K = 1024,
};


enum {
	RTSP_TRANSPORT_TCP,
	RTSP_TRANSPORT_UDP_UNICAST,
	RTSP_TRANSPORT_UDP_MULTICAST
};


enum {
	INT_MAX_BANDWIDTH = 0x7fffffff, /* INT MAX as maximum bandwidth */
};





struct RTSP_SERVER_INFO_s {
	char *connect_host; /* points to host or proxy_host */
	HINT32   connect_port;

	char *host;
	HINT32   port;

	char *proxy_host;
	HINT32   proxy_port;
};

struct RTSP_MS_SdpMsCtx_s {

  HINT32 nRawAsfHeaderLen;
  HUINT8 *pRawAsfHeader;

  struct RTSP_LinkedListHdr_s *streams; /* RTSP_SDP_MS_MediaDesc_s list */

};

struct RTSP_CHANNEL_t {
	HINT32 sock;				 /* socket to get stream from				  */
	
	HINT32 port;
	HINT32 is_udp;
	
	HUINT8 *buffer;		  /* buffer for data which read from network	*/
	HUINT32 buffer_size;	 /* size of malloc() [physical size of buffer] */
	HUINT32 data_len;		/* how many bytes to be read from now		 */
	HUINT32 buffer_pos;	  /* read by here so far						*/

	HINT32 dgram_size;
	HINT32 prev_rtp_seq_num;
	HINT32 is_started;


};


struct resumeinfo_t {
	HINT32 resume_req_success;	   /* seek request for resume succeeded  */
	HUINT64 resume_start_offset; /* start writing to file from here	*/
};


struct rtsp_keepalive_thread_t
{
	struct RTSP_ACCESS_SYS_s *p_stream;
	pthread_t thread_id;
	HBOOL b_quit;
	HBOOL b_paused;
	HBOOL b_thread_error;
};



/*
 * specifies stream.
 */
struct RTSP_ACCESS_SYS_s {
	struct RTSP_SERVER_INFO_s *serverinfo;   /* connection information	 */ /* new */
	struct RTSP_CHANNEL_t *Channel;		 /* socket and network buffer  */ /* new */
	struct RTSP_CHANNEL_t *datasocket[RTSP_MAX_DATA_SOCKET]; /* UDP or TCP Data Socket Channel */
	struct RTSP_CTRL_s *stream_ctrl; /* status of the stream	   */ /* new */
	struct resumeinfo_t *resumeinfo;   /* information for resuming   */ /* new */
	char *localfile;				   /* name of local file		 */ /* new */
	
	struct download_opts_t *dlopts;	/* download options		   */ /* pointer */
	URL_t *url;				 /* url to download.		   */ /* pointer */

	struct rtsp_keepalive_thread_t *p_keepalive_thread;

	HBOOL b_feed_asfheader_first; /* this is a flag for seek 0 and feed header first */
	HBOOL bDuringSeeking; /* when this flag is set, read function must be paused */
	HINT32 nResultQueryWithGivenSeqNum;
	HBOOL bResultForGivenSeqNum;
	HBOOL bInterleavedDataLefted;
	HBOOL b_continuous_send; /* this is a flag for avoid multiple rtsp command send only */
	HBOOL b_is_for_drm;
	HINT32 current_trick_speed;

	HULONG lock_read;
	HULONG lock_write;
	HULONG lock_sync;

	
	/* downloading functions */
	HINT32 (*start)(struct RTSP_ACCESS_SYS_s *);				   /* stream starter */
	HINT32 (*read)(struct RTSP_ACCESS_SYS_s *, HUINT8 *,size_t);  /* stream reader  */
	void (*close)(struct RTSP_ACCESS_SYS_s *);				  /* stream closer  */
};



/* * eunumuration of streaming status */
enum {
	RTSP_STREAMING_HANDSHAKING,	     
	RTSP_STREAMING_DOWNLOADING,	    
	RTSP_STREAMING_FINISHED,		
	RTSP_STREAMING_REWIND,			
	RTSP_STREAMING_RESUME_BUFFERING, 
	RTSP_STREAMING_NO_NEED_TO_DOWNLOAD,
	RTSP_STREAMING_OTHER_PROTOCOL,
	RTSP_STREAMING_REDIRECTED,
};

enum{
	LAST_PACKET_NOTARRIVED,
	LAST_PACKET_ARRIVED,
	LAST_PACKET_READ
};

/*
 * protocol unspecific datas.
 */
struct RTSP_CTRL_s {
	//HINT32 protocol;			   /* downloading protocol					   */

	HINT32 packet_length;		  /* length of each packet for this stream.	 */

	HUINT64 file_size;		 /* size of downloading file				   */
  
	HINT32 total_packets;		  /* number of packets to be received.		  */
	HINT32 packet_count;		   /* how many packet received				   */
	
	HUINT8 *write_buffer;	  /* data to write to file (buffer)			 */
	HUINT32 write_buffer_size; /* malloc()ed size							*/
	HUINT32 write_data_len;	/* how many bytes to be written from now	  */
	HUINT32 write_pos;		 /* written by here so far					 */

	HUINT32 bandwidth;	 /* bandwidth								  */
	HINT32 status;				 /* playing status							 */
	char *retry_urlstr;		 /* url to try again						   */
	HINT32 last_packet_status; /* not arrived, arrived, readed */
	
	struct RTSP_MS_SdpMsCtx_s* pSdpCtx;

	union {
		void *data;			   /* protocol specific datas goes here		  */
		struct rtsp_ctrl_t *rtsp_ctrl;/* protocol specific datas goes here		  */
	} protocol; /* Added because of warning: ISO C doesn't support unnamed structs/unions */
};





struct RTSP_ACCESS_SYS_s *RTSP_LIB_NewRtspAccessSys(void);
void free_RTSP_ACCESS_SYS_s(struct RTSP_ACCESS_SYS_s *st);

struct RTSP_ACCESS_SYS_s *RTSP_LIB_InitRtspAccessSys(void);
void RTSP_LIB_FreeRtspAccessSys(struct RTSP_ACCESS_SYS_s *stream);

struct RTSP_CHANNEL_t *RTSP_LIB_NewChannel(void);
void RTSP_LIB_FreeChannel(struct RTSP_CHANNEL_t *ns);

struct RTSP_SERVER_INFO_s *RTSP_LIB_NewServerInfo(void);
void RTSP_LIB_FreeServerInfo(struct RTSP_SERVER_INFO_s *si);

struct RTSP_CTRL_s *RTSP_LIB_NewRtspCtrl(void);
void RTSP_LIB_FreeRtspCtrl(struct RTSP_CTRL_s *sc);

struct resumeinfo_t *new_resumeinfo_t(void);
void free_resumeinfo_t(struct resumeinfo_t *ri);

void RTSP_LIB_SetServerInfo(struct RTSP_SERVER_INFO_s *serverinfo,
			char *target_host,HINT32 target_port,
			char *proxy_host,HINT32 proxy_port,HINT32 protocol_default_port);
void RTSP_LIB_SetServerInfo_by_proxy_string(struct RTSP_SERVER_INFO_s *serverinfo,
					char *host,HINT32 port,char *proxy_string,
					HINT32 protocol_default_port,HINT32 proxy_default_port);


HINT32 speed_valid_and_guess(const char *str,HINT32 *guessed_speed,char **reason_ret);
char *make_byterange_from_filesize(HUINT64 filesize);
HINT32 protocol_type_from_string(char *protocol);

HINT32 RTSP_LIB_SelectSocket(HINT32 sock,HINT32 timeout);




HINT32 RTSP_LIB_ReadADgramAndFillChannelBuffer(struct RTSP_CHANNEL_t *Channel);

HINT32 RTSP_LIB_NetConnectTCP(const char *servername,const HINT32 port);
HINT32 waiting_socket(HINT32 family,HINT32 port);
HINT32 accept_connection(HINT32 wait_sock);

HINT32 RTSP_LIB_SockRead(HINT32 sock,void *buf,size_t count);
HINT32 RTSP_LIB_SockWrite(HINT32 sock,void *buf,size_t count);

HINT32 RTSP_LIB_SelectCtrlChSocket(struct RTSP_ACCESS_SYS_s *stream,HINT32 timeout);
HINT32 RTSP_LIB_SockReadWithWaitAll(HINT32 sock, void *buf, size_t count);
HINT32 RTSP_LIB_ConnectUDPChannel(struct RTSP_CHANNEL_t *Channel);
HINT32 RTSP_LIB_ClearChannelBuffer(struct RTSP_CHANNEL_t *Channel);
HINT32 set_channel_buffer(struct RTSP_CHANNEL_t *Channel, void *buffer, size_t size);



HINT32 RTSP_LIB_ReadFromChannel(struct RTSP_CHANNEL_t *Channel, void *buffer, size_t size);
HINT32 RTSP_LIB_ReadFromCtrlChannelWithWaitAll(struct RTSP_ACCESS_SYS_s *stream, void *buffer, size_t size);
HINT32 RTSP_LIB_ReadFromCtrlChannelWithNoWait(struct RTSP_ACCESS_SYS_s *stream, void *buffer, size_t max);
HINT32 RTSP_LIB_PutDataForLaterRead(struct RTSP_ACCESS_SYS_s *stream,void *buffer,HINT32 size);


#endif /* __RTSPLIB_H__ */







