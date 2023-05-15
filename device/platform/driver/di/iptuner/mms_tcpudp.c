/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   mms_tcpudp.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:	     MMS over TCP/UDP Controller
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include "di_iptuner_config.h"
#include "di_uart.h"

#include "di_iptuner_common.h"
#include "htype.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <inttypes.h>
#include <poll.h>
#include "vkernel.h"
#include "taskdef.h"

#include "transport_tcp.h"
#include "transport_udp.h"
#include "util_url.h"
#include "stream_global.h"
#include "di_iptuner.h"

#include "mms_asf.h"
#include "mms_buffer.h"
#include "mms_tcpudp.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */

//#define MMS_TCPUDP_DEBUG
//#define MMS_TCPUDP_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG

#ifdef MMS_TCPUDP_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#define PrintError		DI_UART_Print
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
#endif /* END DEBUG DEFINE */


#ifdef MMS_TCPUDP_CRITICAL_DEBUG
#define PrintCriticalDebug      DI_UART_Print
#else
#define PrintCriticalDebug      while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)

#define MMSTU_STACK_SIZE            (32*1024)
#define MMSTU_TASK_PRIORITY         (10)
#define MMSTU_MSG_QUEUE_SIZE        (10)
#define MMSTU_KEEPALIVE_SLEEP_TIME 	(10000) 	/* 1000msec == 1sec */
#define MMSTU_STRING_BUF_LEN        (4096)


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
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
static int INT_MMS_Open ( STREAM_ACCESS_t  *pAccess, URL_t *p_url, int  i_proto );
static int INT_MMS_Start ( STREAM_ACCESS_t  *pAccess, HUINT32 i_packet );
static int INT_MMS_Stop ( STREAM_ACCESS_t *pAccess );
static void INT_MMS_Close ( STREAM_ACCESS_t *pAccess );

static int  INT_MMS_ReadCommand ( STREAM_ACCESS_t *pAccess, int i_command1, int i_command2 );
static int INT_MMS_SendCommand ( STREAM_ACCESS_t *pAccess, int i_command,
                            HUINT32 i_prefix1, HUINT32 i_prefix2,
                            HUINT8 *p_data, int i_data_old );
static int INT_MMS_ReadMediaHeader ( STREAM_ACCESS_t *pAccess, int i_type );
static int  INT_MMS_ReceivePacket  ( STREAM_ACCESS_t *pAccess );

static void INT_MMS_KeepAliveThread ( void *pParam );
static int INT_MMS_ExclusiveReadMediaHeader(STREAM_ACCESS_t *pAccess, int i_type);

/* End static function prototype */


static void GETUTF16 (HUINT16 *p, char ** psz, int size)
{
	int i;
	*psz = (char*) DD_MEM_Alloc ( size + 1);
	if(*psz == NULL)
	{
		return;
	}

	for( i = 0; i < size; i++ )
	{
		(*psz)[i] = p[i];
	}
	(*psz)[size] = '\0';
	p += ( size );

	return;
}


int  MMSTU_Open ( STREAM_ACCESS_t *pAccess )
{
	MMSTU_ACCESS_SYS_t   *pMmstuSys;
	int             i_proto;
	int             i_status;
	HINT32 nVKRet;

	PrintEnter;

	/* Set up pAccess */
	pAccess->info.i_update = 0;
	pAccess->info.i_size = 0;
	pAccess->info.i_pos = 0;
	pAccess->info.b_eof = false;
	pAccess->info.i_title = 0;
	pAccess->info.i_seekpoint = 0;
	pAccess->p_sys = pMmstuSys = (MMSTU_ACCESS_SYS_t *)DD_MEM_Alloc ( sizeof( MMSTU_ACCESS_SYS_t ) );

	if( !pMmstuSys )
	{
		return DI_IPT_ENOMEM;
	}

	VK_memset ( pMmstuSys, 0, sizeof( MMSTU_ACCESS_SYS_t ) );

	pMmstuSys->i_timeout = 10000; // 10secs

	nVKRet = VK_SEM_Create (&pMmstuSys->lock_netwrite , "MMSTcpUdpWriteSema", VK_SUSPENDTYPE_PRIORITY);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> SEMA (lock_netwrite) creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
		return DI_IPT_EGENERIC;
	}

	nVKRet = VK_SEM_Create (&pMmstuSys->lock_readapacket , "MMSTcpUdpReadSema", VK_SUSPENDTYPE_PRIORITY);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> SEMA (lock_readapacket) creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
		return DI_IPT_EGENERIC;
	}

	/* create and start GWM timer task */

	/* *** Parse URL and get server addr/port and path *** */
	UTIL_UrlParse( &pMmstuSys->url, pAccess->psz_path, 0 );

	if( pMmstuSys->url.psz_host == NULL || *pMmstuSys->url.psz_host == '\0' )
	{
		PrintError ( "%s (%d) Error> invalid server name\n",__FUNCTION__,__LINE__ );
		UTIL_UrlClean( &pMmstuSys->url );
		(void) VK_SEM_Destroy (pMmstuSys->lock_netwrite);
		DD_MEM_Free ( pMmstuSys );
		return DI_IPT_EGENERIC;
	}

	if( pMmstuSys->url.i_port <= 0 )
	{
		pMmstuSys->url.i_port = 1755;
	}

	/* *** connect to this server *** */
	/* look at  requested protocol (udp/tcp) */
	i_proto = MMS_PROTO_AUTO;

	/* connect */
	if( i_proto == MMS_PROTO_AUTO )
	{   /* first try with TCP and then UDP*/
		if( ( i_status = INT_MMS_Open ( pAccess, &pMmstuSys->url, MMS_PROTO_TCP ) ))
		{
			i_status = INT_MMS_Open ( pAccess, &pMmstuSys->url, MMS_PROTO_UDP );
		}
	}
	else
	{
		PrintDebug ( "%s (%d) !!!!  CONNECTION protocol i_proto(%d)\n",__FUNCTION__,__LINE__,i_proto);
		i_status = INT_MMS_Open ( pAccess, &pMmstuSys->url, i_proto );
	}

	if( i_status )
	{
		PrintError ( "%s (%d) Error> cannot connect to server\n",__FUNCTION__,__LINE__ );
		UTIL_UrlClean( &pMmstuSys->url );

		VK_SEM_Destroy (pMmstuSys->lock_netwrite);

		DD_MEM_Free ( pMmstuSys );
		return DI_IPT_EGENERIC;
	}

	PrintDebug ( "%s (%d) connected to %s:%d\n",__FUNCTION__,__LINE__, pMmstuSys->url.psz_host, pMmstuSys->url.i_port );
	/*
	 * i_flags_broadcast
	 *  yy xx ?? ??
	 *  broadcast    yy=0x02, xx= 0x00
	 *  pre-recorded yy=0x01, xx= 0x80 if video, 0x00 no video
	 */
	if( pMmstuSys->i_packet_count <= 0 && pMmstuSys->asfh.i_data_packets_count > 0 )
	{
		pMmstuSys->i_packet_count = pMmstuSys->asfh.i_data_packets_count;
	}
	/* else { continue; } */
	if( pMmstuSys->i_packet_count <= 0 || ( pMmstuSys->i_flags_broadcast >> 24 ) == 0x02 )
	{
		pMmstuSys->b_seekable = false;
	}
	else
	{
		PrintError ("%s(%d) This content is seekable !!!!!!!!!!!!\n",__FUNCTION__,__LINE__);
		pMmstuSys->b_seekable = true;
		pAccess->info.i_size =
			(HUINT64)pMmstuSys->i_header +
			(HUINT64)pMmstuSys->i_packet_count * (HUINT64)pMmstuSys->i_packet_length;
	}

#if 1 /* for trick by junsy*/
	/* TODO : b_seekable 과 b_trickable 구분필요함. mms-spec 32 pages */
#endif
	/* *** Start stream *** */
	if( INT_MMS_Start ( pAccess, 0xffffffff ) < 0 )
	{
		PrintError ( "%s (%d) Error> cannot start stream\n",__FUNCTION__,__LINE__ );
		MMSTU_Close ( pAccess );
		return DI_IPT_EGENERIC;
	}
	/* else { continue; } */

	/* Keep the connection alive when paused */
	pMmstuSys->p_keepalive_thread = (mmstu_keepalive_thread_t*)DD_MEM_Alloc (sizeof(mmstu_keepalive_thread_t));
	pMmstuSys->p_keepalive_thread->pAccess = pAccess;
	pMmstuSys->p_keepalive_thread->b_paused = false;
	pMmstuSys->p_keepalive_thread->b_thread_error = false;
	pMmstuSys->p_keepalive_thread->b_quit = false;

	nVKRet = VK_TASK_Create ( INT_MMS_KeepAliveThread
				, AUDIO_CLIP_TASK_PRIORITY  // temp priority TODO:
				, MMSTU_STACK_SIZE
				, "MMSTU_KeepAliveTask"
				, (void *)pMmstuSys->p_keepalive_thread
				, &pMmstuSys->p_keepalive_thread->thread_id
				, 0);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s(%d) Error> KeepAliveThread task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		pMmstuSys->p_keepalive_thread->b_thread_error = true; // FIXME : junsy
		return DI_IPT_ETHREAD;
	}
#if 0 /* event ?? */
	else
	{
		nVKRet = VK_EVENT_Create (&s_ulTunerEventID, "DNPTunerEvent");
		if ( VK_OK != nVKRet )
		{
			PrintError("[%s] Error(0x%x) in VK_EVENT_Create()\n\r", __FUNCTION__,nVKRet);
			return -1;
		}
		else
		{
			s_ucIsInitialized = TRUE;
		}
	}
#endif
	(void)VK_TASK_Start(pMmstuSys->p_keepalive_thread->thread_id);

	PrintExit;
	return DI_IPT_SUCCESS;
}


/*****************************************************************************
 * Close: free unused data structures
 *****************************************************************************/
void MMSTU_Close ( STREAM_ACCESS_t *pAccess )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = (MMSTU_ACCESS_SYS_t *)pAccess->p_sys;

	PrintEnter;

	if( pMmstuSys->p_keepalive_thread )
	{
		pMmstuSys->p_keepalive_thread->b_quit = true;

//		pthread_join (pMmstuSys->p_keepalive_thread->thread_id, NULL);
	}

	/* close connection with server */
	INT_MMS_Close ( pAccess );

	/* free memory */
//	UTIL_UrlClean( &pMmstuSys->url );

	VK_SEM_Destroy (pMmstuSys->lock_netwrite);
	VK_SEM_Destroy (pMmstuSys->lock_readapacket);
	DD_MEM_Free ( pMmstuSys );
	PrintExit;
	return;
}

/*****************************************************************************
 * MMSTU_Control:
 *****************************************************************************/
int MMSTU_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = (MMSTU_ACCESS_SYS_t *)pAccess->p_sys;

	PrintEnter;

	switch (cmd)
	{
		case CMD_CONTROL_PAUSE :
			PrintError ("%s (%d) CMD_CONTROL_PAUSE \n",__FUNCTION__,__LINE__);
			/** Enter Pause State */
#if 0
			(void) INT_MMS_Stop ( pAccess );
//			lock( pMmstuSys->p_keepalive_thread );
			pMmstuSys->p_keepalive_thread->b_paused = true;
//			unlock( pMmstuSys->p_keepalive_thread );
#endif
			break;
		case CMD_CONTROL_RESUME :
			PrintError ("%s (%d) CMD_CONTROL_RESUME \n",__FUNCTION__,__LINE__);
#if 0
			MMSTU_Seek( pAccess, pAccess->info.i_pos );
//			lock( pMmstuSys->p_keepalive_thread );
			pMmstuSys->p_keepalive_thread->b_paused = false;
//			unlock( pMmstuSys->p_keepalive_thread );
#endif
			break;
		case CMD_CONTROL_SPEED :
			PrintError ("%s (%d) CMD_CONTROL_SPEED (val=%d)\n",__FUNCTION__,__LINE__,(HINT32)val);
			{
				int speed=1;
				/* convert DI_MEDIA_TrickSpeed_e to decimal rate)*/
				switch(val)
				{
					case DI_MEDIA_TRICK_PAUSE :
						speed = 0;
						break;
					case DI_MEDIA_TRICK_1RWSTEP :
						speed = -1;
						break;
					case DI_MEDIA_TRICK_2RWSTEP :
						speed = -2;
						break;
					case DI_MEDIA_TRICK_3RWSTEP :
						speed = -3;
						break;
					case DI_MEDIA_TRICK_4RWSTEP :
						speed = -4;
						break;
					case DI_MEDIA_TRICK_5RWSTEP :
						speed = -5;
						break;
					case DI_MEDIA_TRICK_1FFSTEP :
						speed = 1;
						break;
					case DI_MEDIA_TRICK_2FFSTEP :
						speed = 2;
						break;
					case DI_MEDIA_TRICK_3FFSTEP :
						speed = 3;
						break;
					case DI_MEDIA_TRICK_4FFSTEP :
						speed = 4;
						break;
					case DI_MEDIA_TRICK_5FFSTEP :
						speed = 5;
						break;
					default:
						break;
				}

				PrintDebug ("%s (%d) speed (X %d)\n",__FUNCTION__,__LINE__,speed);

				var_buffer_t buffer;
				HUINT32    i_packet;
				i_packet =  (pAccess->info.i_pos - pMmstuSys->i_header ) / pMmstuSys->i_packet_length;
				mms_buffer_initwrite( &buffer, 0 );

				PrintDebug ("%s (%d) pAccess->info.i_pos(%"PRId64")\n",__FUNCTION__,__LINE__,pAccess->info.i_pos);
				/* seekpoint (8 bytes) */
				mms_buffer_add64 ( &buffer, pAccess->info.i_pos ); /* seek point in second TODO: first*/
				/* asfOffset (4 bytes) */
				mms_buffer_add32 ( &buffer, 0xffffffff );
				/* locationId (4 bytes) */
				mms_buffer_add32 ( &buffer, i_packet ); // begin from start
				/* frameOffset (4 bytes) : ignore*/
				mms_buffer_add8 ( &buffer, 0xff ); // stream time limit
				mms_buffer_add8 ( &buffer, 0xff ); //  on 3bytes ...
				mms_buffer_add8 ( &buffer, 0xff ); //
				mms_buffer_add8 ( &buffer, 0x00 ); // don't use limit
				/* playIncarnation (4 bytes) */
				mms_buffer_add32 ( &buffer, pMmstuSys->i_media_packet_id_type ); //  on 3bytes ...
				/* rate (8 bytes) -5 ~ + 5 */
				mms_buffer_add64 ( &buffer, speed);

				INT_MMS_SendCommand ( pAccess, 0x28, pMmstuSys->i_command_level, 0x0001ffff, buffer.p_data, buffer.i_data );
				mms_buffer_free ( &buffer );
			}
			break;

		case CMD_CONTROL_SEEK :
			PrintError ("%s (%d) CMD_CONTROL_SEEK (val(position)=%lu) is NOT SUPPORT yet.!! \n",__FUNCTION__,__LINE__,(unsigned long)val);
			MMSTU_Seek( pAccess, val );

			break;

		default :
			PrintError ("%s (%d) Error> Unknown Command.(%d). \n",__FUNCTION__,__LINE__,cmd);
			return  DI_IPT_EGENERIC;
		break;
	}
	return DI_IPT_SUCCESS;
}

/*****************************************************************************
 * MMSTU_Seek: try to go at the right place
 *****************************************************************************/
int MMSTU_Seek ( STREAM_ACCESS_t * pAccess, HINT64 i_pos )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = pAccess->p_sys;
	var_buffer_t buffer;
	/* A DOUBLE is an 8-byte, double-precision, floating-point number that represents a double-precision, 64-bit [IEEE754] */
	double 	dRequestTime;
	HINT32	ret = DI_IPT_SUCCESS;
	HINT32	int_ret = 0;


	if( i_pos < 0 )
	{
		PrintError ( "%s(%d) Invalid Position Value\n",__FUNCTION__,__LINE__);
		ret = DI_IPT_EGENERIC;
		goto failed;
	}

	dRequestTime = i_pos;


	/* stop MMSTU_Read */
	pMmstuSys->bDuringSeeking = TRUE;


	(void) INT_MMS_Stop ( pAccess );
	PrintDebug ( "%s(%d) stream stopped (seek)\n",__FUNCTION__,__LINE__ );

	/* *** restart stream *** */
	mms_buffer_initwrite( &buffer, 0 );
	mms_buffer_addmemory(&buffer, &dRequestTime, 8);/* seek point in second */   /* position fields*/
	mms_buffer_add32( &buffer, 0xffffffff );					 /* asfOffset */
	mms_buffer_add32( &buffer, 0xffffffff ); /* begin from start*/ /* locationId */
	mms_buffer_add8( &buffer, 0x00 ); /* stream time limit */
	mms_buffer_add8( &buffer, 0x00 ); /*  on 3bytes ... */
	mms_buffer_add8( &buffer, 0x00 );
	mms_buffer_add8( &buffer, 0x00 ); /* don't use limit */
	mms_buffer_add32( &buffer, pMmstuSys->i_media_packet_id_type );
	mms_buffer_add32( &buffer, 1000000 ); /*dwAccelBandwidth 1000Kbps*/
	mms_buffer_add32( &buffer, 60000 ); /* dwAccelDuration 60 sec */
	mms_buffer_add32( &buffer, 1000000 ); /* dwLinkBandwidth 1000Kbps*/

	(void) INT_MMS_SendCommand ( pAccess, 0x07, pMmstuSys->i_command_level, 0x0001ffff, buffer.p_data, buffer.i_data );
	PrintDebug ( "%s(%d) send restart command sec(%f)\n",__FUNCTION__,__LINE__ , dRequestTime);

	mms_buffer_free( &buffer );


	while(pAccess->b_alive == TRUE)
	{
		/* read till reportplay */
		int_ret = INT_MMS_ExclusiveReadMediaHeader ( pAccess, MMS_PACKET_CMD );

		if(int_ret < 0)
		{
			ret = DI_IPT_EGENERIC;
			goto failed;
		}
		else if(int_ret == MMS_PACKET_CMD)
		{
			switch( pMmstuSys->i_command )
			{
				case 0x03:
					PrintError ( "%s(%d) Error> socket closed by server\n",__FUNCTION__,__LINE__ );
					pAccess->info.b_eof = true;
					ret = DI_IPT_EGENERIC;
					goto failed;
				case 0x1e:
					/* ignore EOF Stream */
					PrintDebug("Ignore EOF Stream\n");
					break;
				case 0x05: /* LinkMacToViewerReportStartedPlaying */
					/*check HRESULT */
					/*http://msdn.microsoft.com/en-us/library/cc231198(PROT.10).aspx*/
					if(GetDWordLE( pMmstuSys->p_cmd + 40 ) & 0x80000000)
					{
						PrintDebug("LinkMacToViewerReportStartedPlaying is FAILED\n");
						ret = DI_IPT_EGENERIC;
						goto failed;
					}
					else
					{
						PrintDebug("LinkMacToViewerReportStartedPlaying is SUCCESSED\n");
						ret = DI_IPT_SUCCESS;
						goto success;
					}
					break;
				case 0x20:
					/* XXX not too dificult to be done EXCEPT that we
					 * need to restart demuxer... and I don't see how we
					 * could do that :p */
					PrintError( "%s(%d) reinitialization needed --> unsupported\n",__FUNCTION__,__LINE__ );
					pAccess->info.b_eof = true;
					ret = DI_IPT_EGENERIC;
					goto failed;
				default:
					break;
			}
		}

	}

success:

	PrintError ( "%s(%d) Streaming restarted\n",__FUNCTION__,__LINE__ );

	pMmstuSys->bFeedASFHeaderFirst = TRUE;
	pAccess->info.b_eof = false;

failed:

	return ret;
}

/*****************************************************************************
 * Read:
 *****************************************************************************/
ssize_t MMSTU_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len )
{
#if 1 /* normal read operation */
	MMSTU_ACCESS_SYS_t *pMmstuSys = (MMSTU_ACCESS_SYS_t *)pAccess->p_sys;
	size_t      i_data;
	size_t      i_copy;

	PrintEnter;
	if( pAccess->info.b_eof )
	{
		PrintExit;
		return 0;
	}

	i_data = 0;

	/* *** now send data if needed *** */
	PrintDebug ("%s(%d) i_header(%d) i_packet_length(%d) i_media(%d) i_media_used (%d) i_pos(%"PRId64") try read(%d) \n"
				,__FUNCTION__
				,__LINE__
				,pMmstuSys->i_header
				,pMmstuSys->i_packet_length
				,pMmstuSys->i_media
				,pMmstuSys->i_media_used
				,pAccess->info.i_pos
				,i_len);

	/* fail 이라면 Partial Packet을 넣어준다. */
	/* 현재 Position이 header size보다 작은경우 header를 먼저 카피해준다.*/
	//if( pAccess->info.i_pos < pMmstuSys->i_header || s_bFeedASFHeaderFirst == TRUE)
	if( pMmstuSys->bFeedASFHeaderFirst == TRUE)
	{
		PrintDebug("===FEED ASF HEADER FIRST\n");
		//i_copy = __MIN( i_len, pMmstuSys->i_header - pAccess->info.i_pos );
		i_copy = (size_t)__MIN( (HINT32)i_len, pMmstuSys->asfh.nRawAsfHeaderLen);

		//VK_memcpy( &p_buffer[i_data], &pMmstuSys->p_header[pAccess->info.i_pos], i_copy );
		VK_memcpy( p_buffer, pMmstuSys->asfh.pRawAsfHeader, i_copy );

		i_data += i_copy;

		pAccess->info.i_pos += i_copy;

		pMmstuSys->bFeedASFHeaderFirst = FALSE;

		/* Flushing Buffer must be done care fully */
		pMmstuSys->i_media_used = pMmstuSys->i_packet_length;
		pMmstuSys->bDuringSeeking = FALSE;
	}


	while ( i_data < i_len )
	{
		if(pMmstuSys->bDuringSeeking == TRUE)
		{
			continue;
		}

		/* 버퍼의 자료를 먼저 복사 ; i_media_used */
		if( pMmstuSys->i_media_used < pMmstuSys->i_media )
		{
			i_copy = __MIN( i_len - i_data , pMmstuSys->i_media - pMmstuSys->i_media_used );

			VK_memcpy( &p_buffer[i_data], &pMmstuSys->p_media[pMmstuSys->i_media_used], i_copy );

			i_data += i_copy;

			pMmstuSys->i_media_used += i_copy;

			pAccess->info.i_pos += i_copy;
		}
		/* 한 패킷의 길이는 i_packet_length여야 하므로 모자라다면 0으로 Padding  */
		else if( pMmstuSys->p_media != NULL && pMmstuSys->i_media_used < pMmstuSys->i_packet_length )
		{
			i_copy = __MIN( i_len - i_data, pMmstuSys->i_packet_length - pMmstuSys->i_media_used);

			VK_memset ( &p_buffer[i_data], 0, i_copy );

			i_data += i_copy;

			pMmstuSys->i_media_used += i_copy;

			pAccess->info.i_pos += i_copy;
		}/* 패킷을 읽어옴 */
		else if( pAccess->info.b_eof || INT_MMS_ExclusiveReadMediaHeader ( pAccess, MMS_PACKET_MEDIA ) < 0 )
		{
			PrintError("%s(%d) !!!> EOF or INT_MMS_ReadMediaHeader Error \n",__FUNCTION__,__LINE__);
			break;
		}
	}
	PrintExit;

	return i_data;
#else // Block Read ???????
	MMSTU_ACCESS_SYS_t *pMmstuSys = (MMSTU_ACCESS_SYS_t *)pAccess->p_sys;

	if( pAccess->info.b_eof )
		return NULL;

	if( pAccess->info.i_pos < pMmstuSys->i_header )
	{
		const size_t i_copy = pMmstuSys->i_header - pAccess->info.i_pos;

		block_t *p_block = block_New( pAccess, i_copy );
		if( !p_block )
			return NULL;

		VK_memcpy( p_block->p_buffer, &pMmstuSys->p_header[pAccess->info.i_pos], i_copy );
		pAccess->info.i_pos += i_copy;
		return p_block;
	}
	else if( pMmstuSys->p_media && pMmstuSys->i_media_used < __MAX( pMmstuSys->i_media, pMmstuSys->i_packet_length ) )
	{
		size_t i_copy = 0;
		size_t i_padding = 0;

		if( pMmstuSys->i_media_used < pMmstuSys->i_media )
			i_copy = pMmstuSys->i_media - pMmstuSys->i_media_used;
		if( __MAX( pMmstuSys->i_media, pMmstuSys->i_media_used ) < pMmstuSys->i_packet_length )
			i_padding = pMmstuSys->i_packet_length - __MAX( pMmstuSys->i_media, pMmstuSys->i_media_used );

		block_t *p_block = block_New( pAccess, i_copy + i_padding );
		if( !p_block )
			return NULL;

		if( i_copy > 0 )
			VK_memcpy( &p_block->p_buffer[0], &pMmstuSys->p_media[pMmstuSys->i_media_used], i_copy );
		if( i_padding > 0 )
			VK_memset( &p_block->p_buffer[i_copy], 0, i_padding );

		pMmstuSys->i_media_used += i_copy + i_padding;
		pAccess->info.i_pos += i_copy + i_padding;
		return p_block;
	}

	INT_MMS_ReadMediaHeader ( pAccess, MMS_PACKET_MEDIA );
	return NULL;

#endif
}

/****************************************************************************
 * INT_MMS_Open : Open a connection with the server over mmst or mmsu
 ****************************************************************************/
static int INT_MMS_Open ( STREAM_ACCESS_t  *pAccess, URL_t *p_url, int  i_proto )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = pAccess->p_sys;
	int           b_udp = ( i_proto == MMS_PROTO_UDP ) ? 1 : 0;
	int 		len=0;

	var_buffer_t buffer;
	char         *tmpBuffer=NULL;
	HUINT16     *p;
	int          i_server_version;
	int          i_tool_version;
	int          i_update_player_url;
	int          i_encryption_type;
	int          i;
	int          i_streams;
	int          i_first;
	char         *mediapath;
	char 		tmpString[512];
	int 		 length;


	PrintEnter;

	pMmstuSys->bFeedASFHeaderFirst = TRUE;



	/* *** Open a TCP connection with server *** */
	PrintDebug ( "%s(%d) waiting for connection...\n",__FUNCTION__,__LINE__ );
	pMmstuSys->i_handle_tcp = net_ConnectTCP (p_url->psz_host, p_url->i_port );
	if( pMmstuSys->i_handle_tcp < 0 )
	{
		PrintError ( "%s(%d) Error> failed to open a connection (tcp)\n",__FUNCTION__,__LINE__ );
		return DI_IPT_EGENERIC;
	}

	PrintDebug ( "%s(%d) connection(tcp) with \"%s:%d\" successful\n",__FUNCTION__,__LINE__, p_url->psz_host, p_url->i_port );
	/* *** Bind port if UDP protocol is selected *** */
	if( b_udp )
	{
		PrintDebug ( "%s(%d) UDP Protocol is selected\n",__FUNCTION__,__LINE__);
		if( net_GetSockAddress ( pMmstuSys->i_handle_tcp, pMmstuSys->sz_bind_addr, NULL ) )
		{
			net_Close ( pMmstuSys->i_handle_tcp );
			return DI_IPT_EGENERIC;
		}

		pMmstuSys->i_handle_udp = net_ListenUDP1 ( (const char*)pMmstuSys->sz_bind_addr, 7000 );

		if( pMmstuSys->i_handle_udp < 0 )
		{
			PrintError ( "%s(%d) Error> failed to open a connection (udp)\n",__FUNCTION__,__LINE__ );
			net_Close ( pMmstuSys->i_handle_tcp );
			return DI_IPT_EGENERIC;
		}

		PrintDebug ( "%s(%d) connection(udp) at \"%s:%d\" successful\n",__FUNCTION__,__LINE__, pMmstuSys->sz_bind_addr, 7000 );
	}

	/* *** Init context for mms prototcol *** */
	ASF_GenerateGuid ( &pMmstuSys->guid );    /* used to identify client by server */

	PrintDebug ( "%s(%d) generated guid: "ASF_GUID_FMT"\n",__FUNCTION__,__LINE__, ASF_GUID_PRINT( pMmstuSys->guid ) );

	pMmstuSys->i_command_level = 1;          /* updated after 0x1A command */
	pMmstuSys->i_seq_num = 0;
	pMmstuSys->i_media_packet_id_type  = 0x04;
	pMmstuSys->i_header_packet_id_type = 0x02;
	pMmstuSys->i_proto = i_proto;
	pMmstuSys->i_packet_seq_num = 0;
	pMmstuSys->p_header = NULL;
	pMmstuSys->i_header = 0;
	pMmstuSys->p_media = NULL;
	pMmstuSys->i_media = 0;
	pMmstuSys->i_media_used = 0;

	pAccess->info.i_pos = 0;
	pMmstuSys->i_buffer_tcp = 0;
	pMmstuSys->i_buffer_udp = 0;
	pMmstuSys->p_cmd = NULL;
	pMmstuSys->i_cmd = 0;
	pAccess->info.b_eof = false;

	/* *** send command 1 : connection request *** */
	mms_buffer_initwrite( &buffer, 0 );
	mms_buffer_add16( &buffer, 0x001c );
	mms_buffer_add16( &buffer, 0x0003 );

	tmpBuffer = (char*) DD_MEM_Alloc (MMSTU_STRING_BUF_LEN);
	if(tmpBuffer==NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,4096);
		return DI_IPT_ENOMEM;
	}

	VK_memset  (tmpBuffer, 0, MMSTU_STRING_BUF_LEN);
	length = VK_strlen ("NSPlayer/7.0.0.1956; {") + ASF_GUID_FMT_STRING_SIZE + VK_strlen("}; Host: ") + VK_strlen (p_url->psz_host);

#if 0 /* warning: comparison between pointer and integer */
	if(length > tmpBuffer)
#else
	/* Cant use malloc_usable_size() or _msize() because cant get alloc-size exactly. */
	/* Just use static requesting size or definition value for malloc                 */
	if(length > MMSTU_STRING_BUF_LEN)
#endif
	{
		PrintError ("%s(%d) Error>The  Agent string is too big.!!\n",__FUNCTION__,__LINE__);
		DD_MEM_Free(tmpBuffer);
		return DI_IPT_ENOMEM;
	}

	(void) VK_snprintf( tmpBuffer, length+1/* for null-char*/, "NSPlayer/7.0.0.1956; {"ASF_GUID_FMT"}; Host: %s", ASF_GUID_PRINT( pMmstuSys->guid ), p_url->psz_host );

	mms_buffer_addUTF16( &buffer, tmpBuffer );

	DD_MEM_Free(tmpBuffer);

	/** Send LinkViewerToMacConnect */
	(void) INT_MMS_SendCommand ( pAccess,
			0x01,          /* connection request */
			0x00000000,    /* flags, FIXME */
			0x0004000b,    /* ???? */
			buffer.p_data,
			buffer.i_data );

	/** Receive LinkMacToViewerReportConnectedEX */
	if( INT_MMS_ReadCommand ( pAccess, 0x01, 0 ) < 0 )
	{
		mms_buffer_free( &buffer );
		INT_MMS_Close ( pAccess );
		return DI_IPT_EGENERIC;
	}

	i_server_version = GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 32 );
	i_tool_version = GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 36 );
	i_update_player_url = GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 40 );
	i_encryption_type = GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 44 );
	p = (HUINT16*)( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 48 );

	GETUTF16( p, &pMmstuSys->psz_server_version, i_server_version );
	GETUTF16( p, &pMmstuSys->psz_tool_version, i_tool_version );
	GETUTF16( p, &pMmstuSys->psz_update_player_url, i_update_player_url );
	GETUTF16( p, &pMmstuSys->psz_encryption_type, i_encryption_type );

	PrintDebug (
			"%s(%d) 0x01 --> server_version:\"%s\" tool_version:\"%s\" update_player_url:\"%s\" encryption_type:\"%s\"\n",
			__FUNCTION__,__LINE__,
			pMmstuSys->psz_server_version,
			pMmstuSys->psz_tool_version,
			pMmstuSys->psz_update_player_url,
			pMmstuSys->psz_encryption_type );

	/* *** should make an 18 command to make data timing *** */

	/* *** send command 2 : transport protocol selection *** */
	mms_buffer_reinitwrite( &buffer, 0 );
	mms_buffer_add32( &buffer, 0x00000000 );
	mms_buffer_add32( &buffer, 0x000a0000 );
	mms_buffer_add32( &buffer, 0x00000002 );

	VK_memset (tmpString,0,sizeof(tmpString));

	if( b_udp )
	{
		len = VK_strlen(pMmstuSys->sz_bind_addr) + 7 + 4 + 1;
		if( len > (int)sizeof(tmpString) )
		{
			PrintError ("%s(%d) Error> can't make mms command\n",__FUNCTION__,__LINE__);
			mms_buffer_free( &buffer );
			INT_MMS_Close ( pAccess );
			return DI_IPT_EGENERIC;
		}
		(void)VK_snprintf( tmpString, len,"\\\\%s\\UDP\\%d", pMmstuSys->sz_bind_addr, 7000 ); // FIXME
	}
	else
	{
		len = VK_strlen("\\\\192.168.0.1\\TCP\\1242");
		(void)VK_snprintf( tmpString,len+1, "\\\\192.168.0.1\\TCP\\1242"  );
	}

	mms_buffer_addUTF16( &buffer, tmpString );
	mms_buffer_add16( &buffer, '0' );

	(void) INT_MMS_SendCommand ( pAccess,
			0x02,          /* connexion request */
			0x00000000,    /* flags, FIXME */
			0xffffffff,    /* ???? */
			buffer.p_data,
			buffer.i_data );

	/* *** response from server, should be 0x02 or 0x03 *** */
	INT_MMS_ReadCommand ( pAccess, 0x02, 0x03 );
	if( pMmstuSys->i_command == 0x03 )
	{
		PrintError ( "%s(%d) Error> %s protocol selection failed\n",__FUNCTION__,__LINE__, b_udp ? "UDP" : "TCP" );
		mms_buffer_free( &buffer );
		INT_MMS_Close ( pAccess );
		return DI_IPT_EGENERIC;
	}
	else if( pMmstuSys->i_command != 0x02 )
	{
		PrintError ( "%s(%d) Error> received command isn't 0x02 in reponse to 0x02\n",__FUNCTION__,__LINE__ );
	}

	/* *** send command 5 : media file name/path requested *** */
	mms_buffer_reinitwrite( &buffer, 0 );
	mms_buffer_add64( &buffer, 0 );

	/* media file path shouldn't start with / character */
	mediapath = p_url->psz_path;
	if ( *mediapath == '/' )
	{
		mediapath++;
	}
	/* else { continue; } */
	mms_buffer_addUTF16( &buffer, mediapath );

	(void) INT_MMS_SendCommand ( pAccess,
			0x05,
			pMmstuSys->i_command_level,
			0xffffffff,
			buffer.p_data,
			buffer.i_data );

	/* *** wait for reponse *** */
	INT_MMS_ReadCommand ( pAccess, 0x1a, 0x06 );

	/* test if server send 0x1A answer */
	if( pMmstuSys->i_command == 0x1A )
	{
		PrintError( "%s(%d) Error> id/password requested (not yet supported)\n",__FUNCTION__,__LINE__ );
		/*  FIXME */
		mms_buffer_free( &buffer );
		INT_MMS_Close ( pAccess );
		return DI_IPT_EGENERIC;
	}
	if( pMmstuSys->i_command != 0x06 )
	{
		PrintError( "%s(%d) Error> unknown answer (0x%x instead of 0x06)\n",__FUNCTION__,__LINE__, pMmstuSys->i_command );

		mms_buffer_free( &buffer );
		INT_MMS_Close ( pAccess );
		return( -1 );
	}

	/* Received LinkMacToViewerReportOpenFile (0x06) */
	/*  1 for file ok, 2 for authen ok */
	switch( GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE ) )
	{
		case 0x0001:
			PrintDebug ( "%s(%d) media file name/path accepted\n",__FUNCTION__,__LINE__ );
			break;
		case 0x0002:
			PrintDebug ( "%s(%d) authentication accepted\n",__FUNCTION__,__LINE__ );
			break;
		case -1:
		default:
			PrintError( "%s(%d) Error> error while asking for file %d\n",__FUNCTION__,__LINE__, GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE ) );

			mms_buffer_free( &buffer );
			INT_MMS_Close ( pAccess );
			return DI_IPT_EGENERIC;
	}


	pMmstuSys->i_flags_broadcast = GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 12 );
	/* fileDuration fields */
	pMmstuSys->i_media_length 	= GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 24 );
	/* filePacketSize fields */
	pMmstuSys->i_packet_length 	= GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 44 );
	/* filePacketCount fields */
	pMmstuSys->i_packet_count 	= GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 48 );
	/* fileBitRate fields */
	pMmstuSys->i_max_bit_rate 	= GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 56 );
	/* fileHeaderSize fields */
	pMmstuSys->i_header_size 	= GetDWordLE( pMmstuSys->p_cmd + MMS_CMD_HEADERSIZE + 60 );

	PrintDebug ( "%s (%d)"
			"answer 0x06 flags:0x%8.8"PRIx32" media_length:%"PRIu32"s "
			"packet_length:%zul packet_count:%"PRId32" max_bit_rate:%d "
			"header_size:%zu\n",
			__FUNCTION__,__LINE__,
			pMmstuSys->i_flags_broadcast,
			pMmstuSys->i_media_length,
			(unsigned)pMmstuSys->i_packet_length,
			pMmstuSys->i_packet_count,
			pMmstuSys->i_max_bit_rate,
			pMmstuSys->i_header_size );

	/* *** send command 15 *** */

	mms_buffer_reinitwrite( &buffer, 0 );
	mms_buffer_add32( &buffer, 0 );
	mms_buffer_add32( &buffer, 0x8000 );
	mms_buffer_add32( &buffer, 0xffffffff );
	mms_buffer_add32( &buffer, 0x00 );
	mms_buffer_add32( &buffer, 0x00 );
	mms_buffer_add32( &buffer, 0x00 );
	mms_buffer_add64( &buffer, (((HUINT64)0x40ac2000)<<32) );
	mms_buffer_add32( &buffer, pMmstuSys->i_header_packet_id_type );
	mms_buffer_add32( &buffer, 0x00 );

	(void) INT_MMS_SendCommand ( pAccess, 0x15, pMmstuSys->i_command_level, 0x00, buffer.p_data, buffer.i_data );

	/* *** wait for reponse *** */
	/* Commented out because it fails on some stream (no 0x11 answer) */
#if 0
	INT_MMS_ReadCommand ( pAccess, 0x11, 0 );

	if( pMmstuSys->i_command != 0x11 )
	{
		PrintError(
				"unknown answer (0x%x instead of 0x11)\n",
				pMmstuSys->i_command );
		var_buffer_free( &buffer );
		INT_MMS_Close ( pAccess );
		return( -1 );
	}
#endif

	/* *** now read header packet *** */
	/* XXX could be split over multiples packets */
	PrintDebug ( "%s(%d) reading header\n",__FUNCTION__,__LINE__ );
	for( ;; )
	{
		if( INT_MMS_ExclusiveReadMediaHeader ( pAccess, MMS_PACKET_HEADER ) < 0 )
		{
			PrintError( "%s(%d) Error> cannot receive header\n",__FUNCTION__,__LINE__ );
			mms_buffer_free( &buffer );
			INT_MMS_Close ( pAccess );
			return DI_IPT_EGENERIC;
		}
		if( pMmstuSys->i_header >= pMmstuSys->i_header_size )
		{
			PrintDebug ( "header complete(%zu)\n", pMmstuSys->i_header );
			break;
		}
		PrintDebug ( "%s (%d) header incomplete (%zu/%zu), reading more\n",__FUNCTION__,__LINE__, pMmstuSys->i_header, pMmstuSys->i_header_size );
	}

	/* *** parse header and get stream and their id *** */
	/* get all streams properties,
	 *
	 * TODO : stream bitrates properties(optional)
	 *        and bitrate mutual exclusion(optional) */

	ASF_ParseHeader ( &pMmstuSys->asfh, pMmstuSys->p_header, pMmstuSys->i_header );
	pMmstuSys->asfh.pRawAsfHeader = DD_MEM_Alloc(pMmstuSys->i_header); /* Must be Free in INT_MMS_Close */
	VK_memcpy(pMmstuSys->asfh.pRawAsfHeader, pMmstuSys->p_header, pMmstuSys->i_header);
	pMmstuSys->asfh.nRawAsfHeaderLen = pMmstuSys->i_header;

#ifdef MMS_HTTP_DEBUG
    PrintData ("%s(%d) : i_file_size            (%d)\n",__FUNCTION__,__LINE__,pMmstuSys->asfh.i_file_size);
    PrintData ("%s(%d) : i_data_packets_count   (%d)\n",__FUNCTION__,__LINE__,pMmstuSys->asfh.i_data_packets_count);
    PrintData ("%s(%d) : i_min_data_packet_size (%d)\n",__FUNCTION__,__LINE__,pMmstuSys->asfh.i_min_data_packet_size);
    //ASF_STREAM_t stream[128];
    {
        int i=0;
        for( i = 1; i < 128; i++ )
        {
            if( pMmstuSys->asfh.stream[i].i_cat != ASF_STREAM_UNKNOWN )
            {
                PrintData("%s(%d) : STREAM[%d].i_cat       (%s) \n",__FUNCTION__,__LINE__,i,pMmstuSys->asfh.stream[i].i_cat==1?"ASF_VIDEO_STREAM":"ASF_AUDIO_STREAM");
                PrintData("%s(%d) : STREAM[%d].i_bitrate   (%d) \n",__FUNCTION__,__LINE__,i,pMmstuSys->asfh.stream[i].i_bitrate);
                PrintData("%s(%d) : STREAM[%d].i_selected  (%d) \n",__FUNCTION__,__LINE__,i,pMmstuSys->asfh.stream[i].i_selected);
            }
        }
    }
#endif
	/** Select Stream (ALL) */
    PrintDebug ("%s(%d) : Force Select STREAM (i_bitrate_max(0),b_all(1),b_audio(1),b_video(1)\n",__FUNCTION__,__LINE__);
    ASF_SelectStream ( &pMmstuSys->asfh,0,1,1,1);


	/* *** now select stream we want to receive *** */
	/* TODO take care of stream bitrate TODO */
	i_streams = 0;
	i_first = -1;
	mms_buffer_reinitwrite ( &buffer, 0 );
	/* for now, select first audio and video stream */
	for( i = 1; i < 128; i++ )
	{
		if( pMmstuSys->asfh.stream[i].i_cat != MMS_ASF_STREAM_UNKNOWN )
		{
			i_streams++;
			if( i_first != -1 )
			{
				mms_buffer_add16( &buffer, 0xffff );
				mms_buffer_add16( &buffer, i );
			}
			else
			{
				i_first = i;
			}

			if( pMmstuSys->asfh.stream[i].i_selected )
			{
				mms_buffer_add16( &buffer, 0x0000 );
				PrintData ( "%s (%d) selecting stream[0x%x] %s (%d kb/s)\n"
							, __FUNCTION__
							,__LINE__
							, i
							, ( pMmstuSys->asfh.stream[i].i_cat == MMS_ASF_STREAM_AUDIO  ) ?  "audio" : "video"
							, pMmstuSys->asfh.stream[i].i_bitrate / 1024);
			}
			else
			{
				mms_buffer_add16( &buffer, 0x0002 );
				PrintData ( "%s (%d) ignoring stream[0x%x] %s (%d kb/s)\n"
							, __FUNCTION__
							,__LINE__
							, i
							, ( pMmstuSys->asfh.stream[i].i_cat == MMS_ASF_STREAM_AUDIO  ) ?  "audio" : "video"
							, pMmstuSys->asfh.stream[i].i_bitrate / 1024);
			}
		}
	}

	if( i_streams == 0 )
	{
		PrintError( "%s (%d) Error> cannot find any stream\n",__FUNCTION__,__LINE__ );
		mms_buffer_free( &buffer );
		INT_MMS_Close ( pAccess );
		return DI_IPT_EGENERIC;
	}
	(void) INT_MMS_SendCommand ( pAccess, 0x33,
			i_streams,
			0xffff | ( i_first << 16 ),
			buffer.p_data, buffer.i_data );

	INT_MMS_ReadCommand ( pAccess, 0x21, 0 );
	if( pMmstuSys->i_command != 0x21 )
	{
		PrintError( "%s (%d) Error> unknown answer (0x%x instead of 0x21)\n",__FUNCTION__,__LINE__, pMmstuSys->i_command );

		mms_buffer_free( &buffer );
		INT_MMS_Close ( pAccess );
		return DI_IPT_EGENERIC;
	}


	mms_buffer_free( &buffer );

	PrintData ( "%s (%d) connection successful\n",__FUNCTION__,__LINE__ );
	PrintExit;

	return DI_IPT_SUCCESS;
}

/****************************************************************************
 * INT_MMS_Start : Start streaming
 ****************************************************************************/
static int INT_MMS_Start ( STREAM_ACCESS_t  *pAccess, HUINT32 i_packet )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = (MMSTU_ACCESS_SYS_t *)pAccess->p_sys;
	var_buffer_t    buffer;

	/* *** start stream from packet 0 *** */
	mms_buffer_initwrite( &buffer, 0 );
	mms_buffer_add64( &buffer, 0 ); /* seek point in second */
	mms_buffer_add32( &buffer, 0xffffffff );
	mms_buffer_add32( &buffer, i_packet ); // begin from start
	mms_buffer_add8( &buffer, 0xff ); // stream time limit
	mms_buffer_add8( &buffer, 0xff ); //  on 3bytes ...
	mms_buffer_add8( &buffer, 0xff ); //
	mms_buffer_add8( &buffer, 0x00 ); // don't use limit
	mms_buffer_add32( &buffer, pMmstuSys->i_media_packet_id_type );

	(void) INT_MMS_SendCommand ( pAccess, 0x07, pMmstuSys->i_command_level, 0x0001ffff,
			buffer.p_data, buffer.i_data );

	mms_buffer_free( &buffer );

	INT_MMS_ReadCommand ( pAccess, 0x05, 0 );

	if( pMmstuSys->i_command != 0x05 )
	{
		PrintError( "%s (%d) Error>unknown answer (0x%x instead of 0x05)\n",__FUNCTION__,__LINE__, pMmstuSys->i_command );
		return -1;
	}
	else
	{


  		/* do this on MMSTU_Read */
		/*
		*if( INT_MMS_ExclusiveReadMediaHeader ( pAccess, MMS_PACKET_MEDIA ) < 0 )
		*{
		*	return -1;
		*}
		*/

		PrintDebug ( "%s (%d)streaming started\n",__FUNCTION__,__LINE__ );
		return 0;
	}
}

/****************************************************************************
 * INT_MMS_Stop : Stop streaming
 ****************************************************************************/
static int INT_MMS_Stop ( STREAM_ACCESS_t  *pAccess )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = (MMSTU_ACCESS_SYS_t *) pAccess->p_sys;

	/* *** stop stream but keep connection alive *** */
	(void) INT_MMS_SendCommand ( pAccess,
			0x09,
			pMmstuSys->i_command_level,
			0x001fffff,
			NULL, 0 );
	return( 0 );
}

/****************************************************************************
 * INT_MMS_Close : Close streaming and connection
 ****************************************************************************/
static void INT_MMS_Close ( STREAM_ACCESS_t  *pAccess )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = (MMSTU_ACCESS_SYS_t *)pAccess->p_sys;

	PrintDebug ( "%s(%d) Connection closed\n",__FUNCTION__,__LINE__ );

	/* *** tell server that we will disconnect *** */
	(void) INT_MMS_SendCommand ( pAccess,
			0x0d,
			pMmstuSys->i_command_level,
			0x00000001,
			NULL, 0 );

	/* *** close sockets *** */
	net_Close ( pMmstuSys->i_handle_tcp );

	if( pMmstuSys->i_proto == MMS_PROTO_UDP )
	{
		net_Close ( pMmstuSys->i_handle_udp );
	}

	if(pMmstuSys->p_cmd != NULL)
	{
		FREENULL( pMmstuSys->p_cmd );
	}

	if(pMmstuSys->p_media != NULL)
	{
		FREENULL( pMmstuSys->p_media );
	}

	if(pMmstuSys->p_header != NULL)
	{
		FREENULL( pMmstuSys->p_header );
	}

	if(pMmstuSys->psz_server_version != NULL)
	{
		FREENULL( pMmstuSys->psz_server_version );
	}

	if(pMmstuSys->psz_tool_version != NULL)
	{
		FREENULL( pMmstuSys->psz_tool_version );
	}

	if(pMmstuSys->psz_update_player_url != NULL)
	{
		FREENULL( pMmstuSys->psz_update_player_url );
	}

	if(pMmstuSys->psz_encryption_type != NULL)
	{
		FREENULL( pMmstuSys->psz_encryption_type );
	}

	if(pMmstuSys->asfh.pRawAsfHeader)
	{
		DD_MEM_Free(pMmstuSys->asfh.pRawAsfHeader);
		pMmstuSys->asfh.pRawAsfHeader = NULL;
	}

	return;
}

/****************************************************************************
 *
 * MMS specific functions
 *
 ****************************************************************************/
static int INT_MMS_SendCommand ( STREAM_ACCESS_t *pAccess, int i_command,
                            HUINT32 i_prefix1, HUINT32 i_prefix2,
                            HUINT8 *p_data, int i_data_old )
{
	var_buffer_t buffer;
	MMSTU_ACCESS_SYS_t *pMmstuSys = pAccess->p_sys;
	int i_data_by8, i_ret;
	int i_data = i_data_old;

	PrintEnter;
	while( i_data & 0x7 ) i_data++;
	i_data_by8 = i_data >> 3;

	/* first init buffer */
	mms_buffer_initwrite( &buffer, 0 );

	/*************************************
	 *    TCPMessageHeader
	 *************************************/
	mms_buffer_add32( &buffer, 0x00000001 );    					/* start sequence */
	mms_buffer_add32( &buffer, 0xB00BFACE );						/* session ID */
	/* size after protocol type */
	mms_buffer_add32( &buffer, i_data + MMS_CMD_HEADERSIZE - 16 );	/* messageLength */
	mms_buffer_add32( &buffer, 0x20534d4d );    /* protocol "MMS " */ /*seal*/
	mms_buffer_add32( &buffer, i_data_by8 + 4 );						/* chunkCount */
	mms_buffer_add32( &buffer, pMmstuSys->i_seq_num ); pMmstuSys->i_seq_num++;	/* seq + MBZ (always 0)*/
	mms_buffer_add64( &buffer, 0 );										/* timeSent : Receiver SHOULD ignore this fields */

	/* Start LinkViewerToMacXXXXXX Message */
	/*************************************
	 * make LinkViewerToMacXXXXXX Message
	 *************************************/
	mms_buffer_add32( &buffer, i_data_by8 + 2 );			/* chunkLength */
	mms_buffer_add32( &buffer, 0x00030000 | i_command ); 	/* MID : dir | command */
	mms_buffer_add32( &buffer, i_prefix1 );    				/* command specific */
	mms_buffer_add32( &buffer, i_prefix2 );    				/* command specific */

	/* specific command data */
	if( p_data && i_data > 0 )
	{
		mms_buffer_addmemory( &buffer, p_data, i_data_old );
	}

	/* Append padding to the command data */
	mms_buffer_add64( &buffer, 0 );

	/* send it */
	(void) VK_SEM_Get ( pMmstuSys->lock_netwrite );

	i_ret = net_Write ( pMmstuSys->i_handle_tcp, NULL, buffer.p_data, buffer.i_data - ( 8 - ( i_data - i_data_old ) ) );

	(void) VK_SEM_Release ( pMmstuSys->lock_netwrite);

	if( i_ret != buffer.i_data - ( 8 - ( i_data - i_data_old ) ) )
	{
		PrintError( "%s(%d) Error > failed to send command\n",__FUNCTION__,__LINE__ );
		return DI_IPT_EGENERIC;
	}
	mms_buffer_free( &buffer );

	PrintExit;
	return DI_IPT_SUCCESS;
}

static int INT_MMS_FillBuffer ( STREAM_ACCESS_t *pAccess )
{
	MMSTU_ACCESS_SYS_t    *pMmstuSys = pAccess->p_sys;
	int             i_ret;
	struct pollfd   ufd[2];
	unsigned        timeout, nfd;

	/* FIXME when using udp */
	ssize_t i_tcp, i_udp;
	ssize_t i_tcp_read, i_udp_read;
	int i_try = 0;

	i_tcp = MMS_BUFFER_SIZE/2 - pMmstuSys->i_buffer_tcp;

	if( pMmstuSys->i_proto == MMS_PROTO_UDP )
	{
		i_udp = MMS_BUFFER_SIZE/2 - pMmstuSys->i_buffer_udp;
	}
	else
	{
		i_udp = 0;  /* there isn't udp socket */
	}

	if( ( i_udp <= 0 ) && ( i_tcp <= 0 ) )
	{
		PrintError ( "%s(%d) Error> nothing to read %d:%d\n",__FUNCTION__,__LINE__, (int)i_tcp, (int)i_udp );
		return 0;
	}
	else
	{
		//PrintDebug ( "%s(%d) ask for tcp:%d udp:%d\n",__FUNCTION__,__LINE__, i_tcp, i_udp );
	}

	/* Find if some data is available */
	do
	{
		i_try++;

		/* Initialize file descriptor set */
		VK_memset  (ufd, 0, sizeof (ufd));
		nfd = 0;

		if( i_tcp > 0 )
		{
			ufd[nfd].fd = pMmstuSys->i_handle_tcp;
			ufd[nfd].events = POLLIN;
			nfd++;
		}
		if( i_udp > 0 )
		{
			ufd[nfd].fd = pMmstuSys->i_handle_tcp;
			ufd[nfd].events = POLLIN;
			nfd++;
		}

		/* We'll wait 0.5 second if nothing happens */
		timeout = 500;

		if( i_try * timeout > pMmstuSys->i_timeout )
		{
			PrintError("%s(%d) Error> no data received\n",__FUNCTION__,__LINE__);
			return -1;
		}

		if( i_try > 3 && (pMmstuSys->i_buffer_tcp > 0 || pMmstuSys->i_buffer_udp > 0) )
		{
			return -1;
		}

		if ( pAccess->b_alive == FALSE )
		{
			PrintError ( "%s (%d) Error> MMS Alive False... Quit!!!!\n",__FUNCTION__,__LINE__ );
			return -1;
		}

		//PrintDebug ( "%s (%d) trying again (select)\n",__FUNCTION__,__LINE__ );

	} while ( !(i_ret = poll ( ufd, nfd, timeout)) || (i_ret < 0 && errno == EINTR) );

	if( i_ret < 0 )
	{
		PrintError( "%s (%d) Error> network poll error.\n",__FUNCTION__,__LINE__ ); /* %m ?? */
		return -1;
	}

	i_tcp_read = i_udp_read = 0;

	if( ( i_tcp > 0 ) && ufd[0].revents )
	{
		i_tcp_read = recv ( pMmstuSys->i_handle_tcp, pMmstuSys->buffer_tcp + pMmstuSys->i_buffer_tcp, i_tcp + MMS_BUFFER_SIZE/2, 0 );
	}
	/* else { continue;} */

	if( i_udp > 0 && ufd[i_tcp > 0].revents )
	{
		i_udp_read = recv ( pMmstuSys->i_handle_udp, pMmstuSys->buffer_udp + pMmstuSys->i_buffer_udp, i_udp + MMS_BUFFER_SIZE/2, 0 );
	}
	/* else { continue;} */

#ifdef MMS_TCPUDP_DEBUG
	if( pMmstuSys->i_proto == MMS_PROTO_UDP )
	{
		PrintDebug ( "%s(%d) filling buffer TCP:%d+%d UDP:%d+%d\n",__FUNCTION__,__LINE__, pMmstuSys->i_buffer_tcp, i_tcp_read, pMmstuSys->i_buffer_udp, i_udp_read );
	}
	else
	{
		//PrintDebug ( "%s(%d) filling buffer TCP:%d+%d\n",__FUNCTION__,__LINE__, pMmstuSys->i_buffer_tcp, i_tcp_read );
	}
#endif

	if ( i_tcp_read > 0 )
	{
		pMmstuSys->i_buffer_tcp += i_tcp_read;
	}
	if ( i_udp_read > 0 )
	{
		pMmstuSys->i_buffer_udp += i_udp_read;
	}

	return i_tcp_read + i_udp_read;
}

static int  INT_MMS_ParseCommand ( STREAM_ACCESS_t *pAccess, HUINT8 *p_data, size_t i_data, int *pi_used )
{
 #define GET32( i_pos ) \
    ( pMmstuSys->p_cmd[i_pos] + ( pMmstuSys->p_cmd[i_pos +1] << 8 ) + \
      ( pMmstuSys->p_cmd[i_pos + 2] << 16 ) + \
      ( pMmstuSys->p_cmd[i_pos + 3] << 24 ) )

    MMSTU_ACCESS_SYS_t        *pMmstuSys = pAccess->p_sys;
    HUINT32    i_length;
    HUINT32    i_id;

	PrintEnter;
    VK_free( pMmstuSys->p_cmd );
    pMmstuSys->i_cmd = i_data;
    pMmstuSys->p_cmd = DD_MEM_Alloc ( i_data );
    VK_memcpy( pMmstuSys->p_cmd, p_data, i_data );

    *pi_used = i_data; /* by default */

    if( i_data < MMS_CMD_HEADERSIZE )
    {
        PrintError ( "%s (%d) Error> truncated command (header incomplete)\n",__FUNCTION__,__LINE__ );
        pMmstuSys->i_command = 0;
        return -1;
    }
    i_id =  GetDWordLE( p_data + 4 );
    i_length = GetDWordLE( p_data + 8 ) + 16;

    if( i_id != 0xb00bface || i_length < 16 )
    {
        PrintError( "%s (%d) Error> incorrect command header (0x%"PRIx32")\n",__FUNCTION__,__LINE__, i_id );
        pMmstuSys->i_command = 0;
        return -1;
    }

    if( i_length > pMmstuSys->i_cmd )
    {
        PrintError ( "%s (%d) Error> truncated command (missing %zu bytes)\n",__FUNCTION__,__LINE__, (size_t)i_length - i_data  );
        pMmstuSys->i_command = 0;
        return -1;
    }
    else if( i_length < pMmstuSys->i_cmd )
    {
        pMmstuSys->i_cmd = i_length;
        *pi_used = i_length;
    }

	/* 0~31 bytes are TCP Message Header and 32~ bytes are MMS CMD Message */
    PrintDebug ( "%s (%d) recv command start_sequence:0x%8.8x command_id:0x%8.8x length:%d len8:%d sequence 0x%8.8x len8_II:%d dir_comm:0x%8.8x\n"
			, __FUNCTION__
			, __LINE__
			, GET32( 0 )
			, GET32( 4 )
			, GET32( 8 )
             /* 12: protocol type "MMS " */
            , GET32( 16 )
            , GET32( 20 )
             /* 24: unknown (0) */
             /* 28: unknown (0) */
            , GET32( 32 )
            , GET32( 36 )
             /* 40: switches */
             /* 44: extra */ );

    pMmstuSys->i_command = GET32( 36 ) & 0xffff;
#undef GET32

	PrintExit;
    return MMS_PACKET_CMD;
}

static int  INT_MMS_ParsePacket ( STREAM_ACCESS_t *pAccess, HUINT8 *p_data, size_t i_data, int *pi_used )
{
    MMSTU_ACCESS_SYS_t        *pMmstuSys = pAccess->p_sys;
    int i_packet_seq_num;
    size_t i_packet_length;
    HUINT32 i_packet_id;

    HUINT8  *p_packet;

	PrintEnter;
    *pi_used = i_data; /* default */
    if( i_data <= 8 )
    {
        PrintError ( "%s (%d) Error> truncated packet (header incomplete)\n",__FUNCTION__,__LINE__ );
        return -1;
    }

    i_packet_id = p_data[4];
    i_packet_seq_num = GetDWordLE( p_data );
    i_packet_length = GetWordLE( p_data + 6 );

    //PrintError ( "------->i_packet_length=%d, i_data=%d\n", i_packet_length, i_data );

    if( i_packet_length > i_data || i_packet_length <= 8)
    {
//        PrintError ( "%s(%d) Error> truncated packet (Declared %d bytes, Actual %d bytes)\n",__FUNCTION__,__LINE__, i_packet_length, i_data  );
        *pi_used = 0;
        return -1;
    }
    else if( i_packet_length < i_data )
    {
        *pi_used = i_packet_length;
    }

    if( i_packet_id == 0xff )
    {
        PrintError ( "%s (%d) Error> receive MMS UDP pair timing\n",__FUNCTION__,__LINE__ );
        return( MMS_PACKET_UDP_TIMING );
    }

    if( i_packet_id != pMmstuSys->i_header_packet_id_type &&
        i_packet_id != pMmstuSys->i_media_packet_id_type )
    {
        PrintError ( "%s (%d) Error> incorrect Packet Id Type (0x%x)\n",__FUNCTION__,__LINE__, i_packet_id );
        return -1;
    }

    /* we now have a media or a header packet */
    p_packet = DD_MEM_Alloc ( i_packet_length - 8 ); // don't bother with preheader
    VK_memcpy( p_packet, p_data + 8, i_packet_length - 8 );

    if( i_packet_seq_num != pMmstuSys->i_packet_seq_num )
    {
#if 0
        /* FIXME for udp could be just wrong order ? */
        PrintError ( "detected packet lost (%d != %d)\n", i_packet_seq_num, pMmstuSys->i_packet_seq_num );
#endif
    }

    pMmstuSys->i_packet_seq_num = i_packet_seq_num + 1;

    if( i_packet_id == pMmstuSys->i_header_packet_id_type )
    {
        if( pMmstuSys->p_header )
        {
            pMmstuSys->p_header = realloc( pMmstuSys->p_header,
                                          pMmstuSys->i_header + i_packet_length - 8 );
            VK_memcpy( &pMmstuSys->p_header[pMmstuSys->i_header],
                    p_packet,
                    i_packet_length - 8 );
            pMmstuSys->i_header += i_packet_length - 8;

            VK_free( p_packet );
        }
        else
        {
            pMmstuSys->p_header = p_packet;
            pMmstuSys->i_header = i_packet_length - 8;
        }
        PrintDebug ( "receive header packet (%d bytes)\n", i_packet_length - 8 );

		PrintExit;
        return MMS_PACKET_HEADER;
    }
    else
    {
        FREENULL( pMmstuSys->p_media );
        pMmstuSys->p_media = p_packet;
        pMmstuSys->i_media = i_packet_length - 8;
        pMmstuSys->i_media_used = 0;
        //PrintDebug ( "receive media packet (%d bytes)\n", i_packet_length - 8 );

		PrintExit;
        return MMS_PACKET_MEDIA;
    }
}

static int INT_MMS_ReceivePacket ( STREAM_ACCESS_t *pAccess )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = pAccess->p_sys;
	int i_packet_tcp_type;
	int i_packet_udp_type;

	PrintEnter;
	for( ;; )
	{
		HBOOL b_refill = true;
		/* first if we need to refill buffer */
		if( pMmstuSys->i_buffer_tcp >= MMS_CMD_HEADERSIZE )
		{
			if( GetDWordLE ( pMmstuSys->buffer_tcp + 4 ) == 0xb00bface  )
			{
				if( GetDWordLE( pMmstuSys->buffer_tcp + 8 ) + 16 <= (HUINT32)pMmstuSys->i_buffer_tcp )
				{
					b_refill = false;
				}
			}
			else if( GetWordLE ( pMmstuSys->buffer_tcp + 6 ) <= pMmstuSys->i_buffer_tcp )
			{
				b_refill = false;
			}
		}

		if( pMmstuSys->i_proto == MMS_PROTO_UDP && pMmstuSys->i_buffer_udp >= 8 &&
				GetWordLE( pMmstuSys->buffer_udp + 6 ) <= pMmstuSys->i_buffer_udp )
		{
			b_refill = false;
		}

		if( b_refill && (INT_MMS_FillBuffer ( pAccess ) < 0 ) )
		{
			PrintError ( "%s(%d) Error> cannot fill buffer\n",__FUNCTION__,__LINE__ );
			return -1;
		}

		i_packet_tcp_type = -1;
		i_packet_udp_type = -1;

		if( pMmstuSys->i_buffer_tcp > 0 )
		{
			int i_used;

			if( GetDWordLE( pMmstuSys->buffer_tcp + 4 ) == 0xb00bface )
			{
				i_packet_tcp_type = INT_MMS_ParseCommand ( pAccess, pMmstuSys->buffer_tcp, pMmstuSys->i_buffer_tcp, &i_used );

			}
			else
			{
				i_packet_tcp_type = INT_MMS_ParsePacket ( pAccess, pMmstuSys->buffer_tcp, pMmstuSys->i_buffer_tcp, &i_used );
			}

			if( i_used > 0 && (i_used < MMS_BUFFER_SIZE) )
			{
				memmove ( pMmstuSys->buffer_tcp, pMmstuSys->buffer_tcp + i_used, MMS_BUFFER_SIZE - i_used );
			}
			pMmstuSys->i_buffer_tcp -= i_used;
		}
		else if( pMmstuSys->i_buffer_udp > 0 )
		{
			int i_used;

			i_packet_udp_type = INT_MMS_ParsePacket ( pAccess, pMmstuSys->buffer_udp, pMmstuSys->i_buffer_udp, &i_used );

			if( i_used > 0 && i_used < MMS_BUFFER_SIZE )
			{
				memmove( pMmstuSys->buffer_udp, pMmstuSys->buffer_udp + i_used, MMS_BUFFER_SIZE - i_used );
			}

			pMmstuSys->i_buffer_udp -= i_used;
		}

		if( i_packet_tcp_type == MMS_PACKET_CMD && pMmstuSys->i_command == 0x1b )
		{
			(void) INT_MMS_SendCommand ( pAccess, 0x1b, 0, 0, NULL, 0 );
			i_packet_tcp_type = -1;
		}

		if( i_packet_tcp_type != -1 )
		{
//			PrintDebug ("%s(%d) %d/%d\n",__FUNCTION__,__LINE__, (MMS_BUFFER_SIZE-(pMmstuSys->i_buffer_tcp)), MMS_BUFFER_SIZE );
			PrintExit;
			return i_packet_tcp_type;
		}
		else if( i_packet_udp_type != -1 )
		{
			PrintExit;
			return i_packet_udp_type;
		}
	}
}

static int INT_MMS_ReceiveCommand ( STREAM_ACCESS_t *pAccess )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = pAccess->p_sys;

	for( ;; )
	{
		int i_used;
		int i_status;

		if( INT_MMS_FillBuffer ( pAccess ) < 0 )
		{
			PrintError ( "%s (%d) Error> cannot fill buffer\n",__FUNCTION__,__LINE__ );
			return DI_IPT_EGENERIC;
		}
		if( pMmstuSys->i_buffer_tcp > 0 )
		{
			i_status = INT_MMS_ParseCommand ( pAccess, pMmstuSys->buffer_tcp,
					pMmstuSys->i_buffer_tcp, &i_used );
			if( i_used < MMS_BUFFER_SIZE )
			{
				memmove( pMmstuSys->buffer_tcp, pMmstuSys->buffer_tcp + i_used,
						MMS_BUFFER_SIZE - i_used );
			}
			pMmstuSys->i_buffer_tcp -= i_used;

			if( i_status < 0 )
			{
				return DI_IPT_EGENERIC;
			}

			if( pMmstuSys->i_command == 0x1b )
			{
				(void) INT_MMS_SendCommand ( pAccess, 0x1b, 0, 0, NULL, 0 );
			}
			else
			{
				break;
			}
		}
		else
		{
			return DI_IPT_EGENERIC;
		}
	}

	return DI_IPT_SUCCESS;
}

#define MMS_RETRY_MAX       10
#define MMS_RETRY_SLEEP     5 /* 5secs */

static int INT_MMS_ReadCommand ( STREAM_ACCESS_t *pAccess, int i_command1, int i_command2 )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = pAccess->p_sys;
	int i_count;
	int i_status;

	PrintEnter;
	for( i_count = 0; i_count < MMS_RETRY_MAX; )
	{
		i_status = INT_MMS_ReceiveCommand ( pAccess );
		if( i_status < 0 || pMmstuSys->i_command == 0 )
		{
			i_count++;
			sleep( MMS_RETRY_SLEEP );
		}
		else if( i_command1 == 0 && i_command2 == 0)
		{
			PrintExit;
			return DI_IPT_SUCCESS;
		}
		else if( pMmstuSys->i_command == i_command1 ||
				pMmstuSys->i_command == i_command2 )
		{
			PrintExit;
			return DI_IPT_SUCCESS;
		}
		else
		{
			switch( pMmstuSys->i_command )
			{
				case 0x03:
					PrintError ( "%s (%d) Error> socket closed by server\n",__FUNCTION__,__LINE__ );
					pAccess->info.b_eof = true;
					PrintExit;
					return DI_IPT_EGENERIC;
				case 0x1e:
					PrintError ( "%s (%d) Error> end of media stream\n",__FUNCTION__,__LINE__ );
					PrintExit;
					pAccess->info.b_eof = true;
					return DI_IPT_EGENERIC;
				default:
					break;
			}
		}
	}
	pAccess->info.b_eof = true;
	PrintError ( "%s (%d) Error> failed to receive command (aborting)\n",__FUNCTION__,__LINE__ );

	PrintExit;
	return DI_IPT_EGENERIC;
}

static int INT_MMS_ExclusiveReadMediaHeader(STREAM_ACCESS_t *pAccess, int i_type)
{
	HINT32 ret = 0;
	MMSTU_ACCESS_SYS_t *pMmstuSys = pAccess->p_sys;

	PrintEnter;
	VK_SEM_Get(pMmstuSys->lock_readapacket);

	ret= INT_MMS_ReadMediaHeader(pAccess, i_type);

	VK_SEM_Release(pMmstuSys->lock_readapacket);
	PrintExit;
	return ret;
}


static int INT_MMS_ReadMediaHeader ( STREAM_ACCESS_t *pAccess, int i_type )
{
	MMSTU_ACCESS_SYS_t *pMmstuSys = pAccess->p_sys;
	int          i_count;

	PrintEnter;

	for( i_count = 0; i_count < MMS_RETRY_MAX; )
	{
		int i_status;

		if( pAccess->b_alive == FALSE)
		{
			PrintError ( "%s(%d) Error> MMS Parser is not alive !!!\n",__FUNCTION__,__LINE__);
			return -1;
		}

		i_status = INT_MMS_ReceivePacket ( pAccess );
		if( i_status < 0 )
		{
			i_count++;
			PrintError ( "%s(%d) Error> cannot receive header (%d/%d)\n",__FUNCTION__,__LINE__, i_count, MMS_RETRY_MAX );

			sleep ( MMS_RETRY_SLEEP );

		}
		else if( i_status == i_type || i_type == MMS_PACKET_ANY )
		{
			return i_type;
		}
		else if( i_status == MMS_PACKET_CMD ) /* default cmd processor */
		{
			switch( pMmstuSys->i_command )
			{
				case 0x03:
					PrintError ( "%s(%d) Error> socket closed by server\n",__FUNCTION__,__LINE__ );
					pAccess->info.b_eof = true;
					return -1;
				case 0x1e:
					PrintError ( "%s(%d) Error> end of media stream\n",__FUNCTION__,__LINE__ );
					pAccess->info.b_eof = true;
					return -1;
				case 0x20:
					/* XXX not too dificult to be done EXCEPT that we
					 * need to restart demuxer... and I don't see how we
					 * could do that :p */
					PrintError( "%s(%d) reinitialization needed --> unsupported\n",__FUNCTION__,__LINE__ );
					pAccess->info.b_eof = true;
					return -1;
				default:
					break;
			}
		}
	}

	PrintError( "%s(%d) Error> cannot receive %s (aborting)\n",__FUNCTION__,__LINE__, ( i_type == MMS_PACKET_HEADER ) ? "header" : "media data" );
	pAccess->info.b_eof = true;

	PrintExit;
	return -1;
}

static void INT_MMS_KeepAliveThread ( void *pParam )
{
	mmstu_keepalive_thread_t *p_thread = (mmstu_keepalive_thread_t *) pParam;
	STREAM_ACCESS_t *pAccess = p_thread->pAccess;

	HBOOL b_paused;
	HBOOL b_was_paused = false;

//	lock( p_thread );
	PrintEnter;
	while ( true != p_thread->b_quit ) //object alive ??( p_thread) )
	{
		b_paused = p_thread->b_paused;

		if( b_paused && b_was_paused )
		{
			(void) INT_MMS_SendCommand ( pAccess, 0x1b, 0, 0, NULL, 0 );
		}

		b_was_paused = b_paused;
#if 0
		object_timedwait( p_thread, mdate() + 10000000 );
#else
		/* TODO : How many seconds.... , just 10 secs*/
		sleep(10);
//		(void)VK_TASK_Sleep (MMSTU_KEEPALIVE_SLEEP_TIME);
#endif
	}
	p_thread->b_quit = false;
	PrintExit;
//	unlock ( p_thread );
	return ;
}
