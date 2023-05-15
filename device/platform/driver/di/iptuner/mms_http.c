/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   mms_http.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	MMS Over HTTP Implementation
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
#include "di_iptuner_common.h"
#include "di_uart.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>
#include "htype.h"
#include "vkernel.h"
#include "util_url.h"

#include "mms_asf.h"
#include "mms_buffer.h"
#include "mms_http.h"
#include "transport_tcp.h"
#include "util_b64_encode.h"
#include "stream_global.h"

//#define MMS_HTTP_DEBUG
//#define MMS_HTTP_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG

#ifdef MMS_HTTP_DEBUG
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

#ifdef MMS_HTTP_CRITICAL_DEBUG
#define PrintCriticalDebug      DI_UART_Print
#else
#define PrintCriticalDebug      while (0) ((int (*)(char *, ...)) 0)
#endif


#define UNUSED_PARAM(x)  ((void)x)

#define C_PACKET			0x4324		/* Stream Change Notification Packet */
#define C_PACKET_B			0x43A4		/* Stream Change Notification Packet(Include B field) */
#define D_PACKET			0x4424		/* Data Packet */
#define D_PACKET_B			0x44A4		/* Data Packet(Include B field) */
#define E_PACKET			0x4524		/* End of Stream Notification Packet */
#define E_PACKET_B			0x45A4		/* End of Stream Notification Packet(Include B field) */
#define H_PACKET			0x4824		/* Header Packet */
#define H_PACKET_B			0x48A4		/* Header Packet(Include B field) */
#define M_PACKET			0x4D24		/* Meta Data Packet */
#define M_PACKET_B			0x4DA4		/* Meta Data Packet(Include B field) */
#define P_PACKET			0x5024		/* Packet Pair Packet */
#define P_PACKET_B			0x50A4		/* Packet Pair Packet(Include B field) */
#define T_PACKET			0x5424		/* Test Data Notification Packet */
#define T_PACKET_B			0x54A4		/* Test Data Notification Packet(Include B field) */

#define SUPPORT_KEEPALIVE 0

/* TODO:
 *  - authentication
 */
/*****************************************************************************
 * Local Variable
 *****************************************************************************/
#if SUPPORT_KEEPALIVE
static int s_nCountforKeepalive = 0;
#endif

static HINT8 *s_pcUserAgentInfo=NULL;

/*****************************************************************************
 * Local prototypes
 *****************************************************************************/
static int INT_MMSH_Describe ( STREAM_ACCESS_t  *pAccess, char **ppsz_location );
static int INT_MMSH_Start( STREAM_ACCESS_t *pAccess, HINT64 i_pos );
static void INT_MMSH_Stop ( STREAM_ACCESS_t *pAccess );

static int INT_MMSH_GetPacket ( STREAM_ACCESS_t * pAccess, chunk_t *p_ck );
static void INT_MMSH_GetHeader ( STREAM_ACCESS_t *pAccess );

static int INT_MMSH_Restart ( STREAM_ACCESS_t *pAccess );
static int INT_MMSH_Reset ( STREAM_ACCESS_t *pAccess );

#if SUPPORT_KEEPALIVE
static int INT_MMSH_SendKeepAlive(STREAM_ACCESS_t *pAccess);
#endif

/****************************************************************************
 * Open: connect to ftp server and ask for file
 ****************************************************************************/
int MMSH_Open ( STREAM_ACCESS_t *pAccess )
{
	MMSH_ACCESS_SYS_t    *pMmshSys = NULL;
	char            *psz_location = NULL;
	int            nRetryCount = 0;
#if 0 /** HTTP Proxy */
	char            *psz_proxy;
#endif

	/* init pMmshSys */
	PrintEnter;

	pAccess->info.i_update = 0;
	pAccess->info.i_size = 0;
	pAccess->info.i_pos = 0;
	pAccess->info.b_eof = false;
	pAccess->info.i_title = 0;
	pAccess->info.i_seekpoint = 0;

Redirection:
	pAccess->p_sys = pMmshSys = DD_MEM_Alloc ( sizeof( MMSH_ACCESS_SYS_t ) );
	if( !pMmshSys )
	{
		PrintError ("%s(%d) : pMmshSys is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_ENOMEM;
	}

	VK_memset ( pMmshSys, 0, sizeof( MMSH_ACCESS_SYS_t ) );
	pMmshSys->p_header = NULL;
	pMmshSys->p_packet = NULL;

#if 0 // junsy  HTTP는 TCP와 구분할 필요 없음.. 상위단에서 식별 flag 필요함.
	pMmshSys->i_proto= MMS_PROTO_HTTP;
#endif
	pMmshSys->fd     = -1;
	pMmshSys->i_start= 0;

	/* Handle proxy */
	pMmshSys->b_proxy = false;
	VK_memset ( &pMmshSys->proxy, 0, sizeof(pMmshSys->proxy) );

#if 0 /*TODO: junsy : 현재 Proxy 고려하지 않음 */
	/* Check proxy */
	/* TODO reuse instead http-proxy from http access ? */
	psz_proxy = var_CreateGetString( pAccess, "mmsh-proxy" );
	if( !*psz_proxy )
	{
		char *psz_http_proxy = config_GetPsz( pAccess, "http-proxy" );
		if( psz_http_proxy && *psz_http_proxy )
		{
			DD_MEM_Free ( psz_proxy );
			psz_proxy = psz_http_proxy;
			var_SetString( pAccess, "mmsh-proxy", psz_proxy );
		}
		else
		{
			DD_MEM_Free ( psz_http_proxy );
		}
	}

	if( *psz_proxy )
	{
		pMmshSys->b_proxy = true;
		UTIL_UrlParse( &pMmshSys->proxy, psz_proxy, 0 );
	}
#ifdef HAVE_GETENV
	else
	{
		char *psz_proxy = getenv( "http_proxy" );
		if( psz_proxy && *psz_proxy )
		{
			pMmshSys->b_proxy = true;
			UTIL_UrlParse( &pMmshSys->proxy, psz_proxy, 0 );
		}
	}
#endif
	DD_MEM_Free ( psz_proxy );
#endif /* junsy */

	if( pMmshSys->b_proxy )
	{
		if( ( pMmshSys->proxy.psz_host == NULL ) ||
				( *pMmshSys->proxy.psz_host == '\0' ) )
		{
			PrintError( "invalid proxy host\n" );
			UTIL_UrlClean( &pMmshSys->proxy );
			if(pMmshSys != NULL)
			{
				DD_MEM_Free ( pMmshSys );
				pMmshSys =NULL;
				pAccess->p_sys = NULL;
			}

			PrintExit;
			return DI_IPT_EGENERIC;
		}

		if( pMmshSys->proxy.i_port <= 0 )
		{
			pMmshSys->proxy.i_port = 80;
		}

		PrintDebug ( "Using http proxy %s:%d\n", pMmshSys->proxy.psz_host, pMmshSys->proxy.i_port );
	}

	/* open a tcp connection */
	UTIL_UrlParse( &pMmshSys->url, pAccess->psz_path, 0 );

	if( ( pMmshSys->url.psz_host == NULL ) || ( *pMmshSys->url.psz_host == '\0' ) )
	{
		PrintError ( "invalid host\n" );

		UTIL_UrlClean( &pMmshSys->proxy );
		UTIL_UrlClean( &pMmshSys->url );

		if(pMmshSys != NULL)
		{
			DD_MEM_Free ( pMmshSys );
			pMmshSys =NULL;
			pAccess->p_sys = NULL;
		}

		PrintExit;
		return DI_IPT_EGENERIC;
	}

	if( pMmshSys->url.i_port <= 0 )
	{
		pMmshSys->url.i_port = 80;
	}

	if( INT_MMSH_Describe ( pAccess, &psz_location ) )
	{
		if( INT_MMSH_Describe ( pAccess, &psz_location ) )
		{
			UTIL_UrlClean( &pMmshSys->proxy );
			UTIL_UrlClean( &pMmshSys->url );
			if(pMmshSys != NULL)
			{
				DD_MEM_Free ( pMmshSys );
				pMmshSys =NULL;
				pAccess->p_sys = NULL;
			}

			PrintError("%s(%d) Error> INT_MMSH_Describe Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return DI_IPT_EGENERIC;
		}
	}

	/* Handle redirection */
	if( psz_location && *psz_location )
	{
#if 1 /* TODO: 현재는 redirection  기능을 임시로 구현함. 차후 추가적으로 변경방안 고려필요함 */
		PrintDebug ( "redirection to %s\n", psz_location );

		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}

		UTIL_UrlClean( &pMmshSys->url );
		if(pMmshSys != NULL)
		{
			DD_MEM_Free ( pMmshSys );
			pMmshSys =NULL;
			pAccess->p_sys = NULL;
		}


		pAccess->psz_path = VK_StrDup(psz_location);
		psz_location = NULL;

		if(nRetryCount <5)
		{
			nRetryCount++;
			goto Redirection;
		}

//		pAccess->pf_read = ReadRedirect;

		PrintExit;
		return DI_IPT_EGENERIC;
#else
		PrintError (" psz_location && *psz_location is true... Handle redirection \n");
		PrintExit;
		return DI_IPT_SUCCESS;
#endif
	}

	/* Start playing */
	if( INT_MMSH_Start ( pAccess, 0 ) )
	{
		PrintError ( "cannot start stream\n" );
		if(pMmshSys->p_header != NULL)
		{
			DD_MEM_Free ( pMmshSys->p_header );
			pMmshSys->p_header = NULL;
		}

		UTIL_UrlClean( &pMmshSys->proxy );
		UTIL_UrlClean( &pMmshSys->url );

		pMmshSys->p_packet = NULL;

		if(pMmshSys != NULL)
		{
			DD_MEM_Free ( pMmshSys );
			pMmshSys =NULL;
			pAccess->p_sys = NULL;
		}

		PrintExit;
		return DI_IPT_EGENERIC;
	}

	if( !pMmshSys->b_broadcast )
	{
		pAccess->info.i_size = pMmshSys->asfh.i_file_size;
	}

	if(pAccess->info.i_size == 0)
	{
		pAccess->info.i_size = 0xFFFFFFFFFFFFFF;
//		pHttpSys->i_remaining = 0xFFFFFFFFFFFFFF;
	}

	PrintExit;
	return DI_IPT_SUCCESS;
}

/*****************************************************************************
 * Close: free unused data structures
 *****************************************************************************/
void  MMSH_Close ( STREAM_ACCESS_t *pAccess )
{
	MMSH_ACCESS_SYS_t *pMmshSys = NULL;

	PrintEnter;

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}

	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;

	INT_MMSH_Stop( pAccess );

	if(pMmshSys->p_header != NULL)
	{
		DD_MEM_Free ( pMmshSys->p_header );
		pMmshSys->p_header =NULL;
	}

	pMmshSys->p_packet =NULL;
	UTIL_UrlClean( &pMmshSys->proxy );
	UTIL_UrlClean( &pMmshSys->url );
	DD_MEM_Free ( pMmshSys );
	pMmshSys =NULL;
	pAccess->p_sys = NULL;

//	UTIL_UrlClean( &pMmshSys->proxy );
//	UTIL_UrlClean( &pMmshSys->url );
//	DD_MEM_Free ( pMmshSys );

	PrintExit;
	return;
}

/*****************************************************************************
 * MMSH_Control:
 *****************************************************************************/
int MMSH_Control( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HINT32 val )
{
	MMSH_ACCESS_SYS_t *pMmshSys = NULL;

	PrintEnter;

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;

	switch (cmd)
	{
		case CMD_CONTROL_PAUSE :
			PrintError ("%s (%d) CMD_CONTROL_PAUSE \n",__FUNCTION__,__LINE__);
			break;
		case CMD_CONTROL_RESUME :
			PrintError ("%s (%d) CMD_CONTROL_RESUME \n",__FUNCTION__,__LINE__);
			break;
		case CMD_CONTROL_SPEED :
			PrintError ("%s (%d) CMD_CONTROL_SPEED (val=%d)\n",__FUNCTION__,__LINE__,(HINT32)val);
			break;

		case CMD_CONTROL_SEEK :
			PrintError ("%s (%d) CMD_CONTROL_SEEK (val(position)=%lu)\n",__FUNCTION__,__LINE__,(unsigned long)val);
			MMSH_Seek( pAccess, val );
			break;

		default :
			PrintError ("%s (%d) Error> Unknown Command.(%d). \n",__FUNCTION__,__LINE__,cmd);
			return  DI_IPT_EGENERIC;
		break;
	}
	return DI_IPT_SUCCESS;
}

/*****************************************************************************
 * MMSH_Seek: try to go at the right place
 *****************************************************************************/
int MMSH_Seek ( STREAM_ACCESS_t *pAccess, HINT64 i_pos )
{
	MMSH_ACCESS_SYS_t *pMmshSys = NULL;
	chunk_t      ck;
	off_t        i_offset=0;
	off_t        i_packet=0;

	PrintEnter;
	PrintDebug ( "seeking to %"PRId64"\n", i_pos );

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	VK_MEM_Memset(&ck, 0x0, sizeof(chunk_t));
	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;

	i_packet = ( i_pos - pMmshSys->i_header ) / pMmshSys->asfh.i_min_data_packet_size;
	i_offset = ( i_pos - pMmshSys->i_header ) % pMmshSys->asfh.i_min_data_packet_size;

	INT_MMSH_Stop ( pAccess );
	INT_MMSH_Start ( pAccess, i_pos );

	while ( pAccess->b_alive == TRUE)
	{
		if( INT_MMSH_GetPacket ( pAccess, &ck ) )
		{
			break;
		}

		/* skip headers */
		if( ck.i_type != H_PACKET )
		{
			break;
		}

		PrintError( "skipping header\n" );
	}

	pAccess->info.i_pos = i_pos;
	pAccess->info.b_eof = false;
	pMmshSys->i_packet_used += i_offset;

	PrintExit;
	return DI_IPT_SUCCESS;
}


/*****************************************************************************
 * MMSH_Read:
 *****************************************************************************/
ssize_t MMSH_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len )
{
	MMSH_ACCESS_SYS_t *pMmshSys = NULL;
	size_t       i_copy = 0;
	size_t       i_data = 0;
	int			nRetryErrorCount_DataRead = 0;
	int			nDataReadCount = 0;
	int			nTimeoutCount_DataRead = 0;
	chunk_t		tChunk;
	int			i_ret = -1;

	PrintEnter;

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	VK_MEM_Memset(&tChunk, 0x0, sizeof(chunk_t));
	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;

	if( pAccess->info.b_eof )
	{
		PrintError("pAccess->info.b_eof is %d\n",pAccess->info.b_eof);
		PrintExit;
		return 0;
	}

	while ( i_data < (size_t) i_len )
	{
		if( pAccess->info.i_pos < (pMmshSys->i_start + pMmshSys->i_header) )
		{
			if((pAccess->info.i_pos != 0) && (pMmshSys->i_start != 0))
			{
				PrintError ("\n\n\n### %s (%d) Critical Section(i_Start) ###\n\n\n",__FUNCTION__,__LINE__);
			}

			int i_offset = pAccess->info.i_pos - pMmshSys->i_start;

			i_copy = __MIN( pMmshSys->i_header - i_offset, (int)((size_t)i_len - i_data) );

			VK_memcpy( &p_buffer[i_data], &pMmshSys->p_header[i_offset], i_copy );

			i_data += i_copy;

			pAccess->info.i_pos += i_copy;
		}
		else if( pMmshSys->i_packet_used < pMmshSys->i_packet_length )
		{
			i_copy = __MIN( pMmshSys->i_packet_length - pMmshSys->i_packet_used, i_len - i_data );

			VK_memcpy( &p_buffer[i_data],
					&pMmshSys->p_packet[pMmshSys->i_packet_used],
					i_copy );

			i_data += i_copy;
			pMmshSys->i_packet_used += i_copy;
			pAccess->info.i_pos += i_copy;
		}
		else if( (pMmshSys->i_packet_length > 0) && ((int)pMmshSys->i_packet_used < pMmshSys->asfh.i_min_data_packet_size) )
		{
			i_copy = __MIN( pMmshSys->asfh.i_min_data_packet_size - pMmshSys->i_packet_used, i_len - i_data );

			VK_memset ( &p_buffer[i_data], 0, i_copy );

			i_data += i_copy;
			pMmshSys->i_packet_used += i_copy;
			pAccess->info.i_pos += i_copy;
		}
		else
		{
			i_ret = INT_MMSH_GetPacket ( pAccess, &tChunk );
			if( i_ret != DI_IPT_SUCCESS )
			{
				if( pMmshSys->b_broadcast )
				{
					if(((tChunk.i_type == E_PACKET) || (tChunk.i_type == E_PACKET_B)) && (tChunk.i_sequence != 0) )
					{
						i_ret = INT_MMSH_Restart( pAccess );
						if(i_ret == DI_IPT_EGENERIC)
						{
							PrintError (" Fail INT_MMSH_Restart \n");
							PrintExit;
							return DI_IPT_EGENERIC;
						}
					}
					else if((tChunk.i_type == C_PACKET) || (tChunk.i_type == C_PACKET_B))
					{
						i_ret = INT_MMSH_Reset( pAccess );
						if(i_ret == DI_IPT_EGENERIC)
						{
							PrintError (" Fail INT_MMSH_Reset \n");
							PrintExit;
							return DI_IPT_EGENERIC;
						}
					}
				}

				if(i_ret == DI_IPT_ETIMEOUT)
				{
					nTimeoutCount_DataRead++;
				}

				if(nTimeoutCount_DataRead >=3)
				{
					PrintError("### [%s] (%d) Fail Timeout ###\n", __FUNCTION__,__LINE__);
					pAccess->info.b_eof = true;
					PrintExit;
					return 0;
				}

				/* Retry */
				if(nRetryErrorCount_DataRead != 20)
				{
					if(nRetryErrorCount_DataRead==10)
					{
						PrintError("[%s] (%d) nRetryErrorCount_DataRead=10\n", __FUNCTION__,__LINE__);
					}

					VK_TASK_Sleep(20);
					PrintError("### Increae Retry (Count=%d) ###\n", nRetryErrorCount_DataRead);
					nRetryErrorCount_DataRead++;
					continue;
				}
				else
				{
					PrintError("[%s] (%d) nRetryErrorCount_DataRead=20\n", __FUNCTION__,__LINE__);

					PrintExit;
					return 0;
				}

				if( i_ret )
				{
					PrintDebug("[%s] (%d) Data Read Fail\n",__FUNCTION__,__LINE__);
					pAccess->info.b_eof = true;
					PrintExit;
					return 0;
				}
			}
			else
			{
				/* ASF Header를 잘못읽어 정확한 Position을 찾지 못하고 Data만 읽는 경우에 대해 대비하기 위함. */
				if(pMmshSys->i_packet_length > 16200) /* ASF 읽는 데이터와 Header데이터 포함하여 Size를 고려함 */
				{
					nDataReadCount = nDataReadCount+2;
				}
				else if(pMmshSys->i_packet_length > 8100)
				{
					nDataReadCount++;
				}

				if(nDataReadCount > 80)
				{
					PrintExit;
					return 0;
				}
			}

			if((tChunk.i_type != D_PACKET) && ( tChunk.i_type != D_PACKET_B ))
			{
				pMmshSys->i_packet_used = 0;
				pMmshSys->i_packet_length = 0;
			}
		}
	}

#if SUPPORT_KEEPALIVE
	if(s_nCountforKeepalive == 10)
	{
		PrintDebug ("### Send Keep-Alive ###\n");

		(void)INT_MMSH_SendKeepAlive(pAccess);

		s_nCountforKeepalive = 0;
	}
	else
	{
		s_nCountforKeepalive++;
	}
#endif

	PrintDebug (" return length (%d) \n",i_len);
	PrintExit;
	return( i_data );
}

/*****************************************************************************
 * MMSH_Read:
 *****************************************************************************/
#if 0
static ssize_t INT_MMSH_ReadRedirect( STREAM_ACCESS_t *pAccess, HUINT8 *p, size_t i_len )
{
	PrintEnter;
	UNUSED_PARAM (pAccess);
	UNUSED_PARAM (p);
	UNUSED_PARAM (i_len);
	PrintExit;
    return 0;
}
#endif

/* */
static int INT_MMSH_Restart( STREAM_ACCESS_t *pAccess )
{
	MMSH_ACCESS_SYS_t *pMmshSys = NULL;
	char *psz_location = NULL;

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;

	PrintDebug ( "Restart the stream\n" );
	if(pMmshSys->b_broadcast != TRUE)
	{
		pMmshSys->i_start = pAccess->info.i_pos;
	}
	else
	{
		pMmshSys->i_start = 0;
	}

	/* */
	PrintDebug ( "stoping the stream\n" );
	INT_MMSH_Stop( pAccess );

	/* */
	PrintDebug ( "describe the stream\n" );
	if( INT_MMSH_Describe( pAccess, &psz_location ) )
	{
		UTIL_UrlClean( &pMmshSys->proxy );
		UTIL_UrlClean( &pMmshSys->url );
		if(pMmshSys != NULL)
		{
			DD_MEM_Free ( pMmshSys );
			pMmshSys =NULL;
			pAccess->p_sys = NULL;
		}

		PrintError ( "describe failed\n" );

		return DI_IPT_EGENERIC;
	}
	/* */
	if( INT_MMSH_Start( pAccess, 0 ) )
	{
		PrintError ( "cannot start stream\n" );
		if(pMmshSys->p_header != NULL)
		{
			DD_MEM_Free ( pMmshSys->p_header );
			pMmshSys->p_header = NULL;
		}

		UTIL_UrlClean( &pMmshSys->proxy );
		UTIL_UrlClean( &pMmshSys->url );

		pMmshSys->p_packet = NULL;

		if(pMmshSys != NULL)
		{
			DD_MEM_Free ( pMmshSys );
			pMmshSys =NULL;
			pAccess->p_sys = NULL;
		}

		PrintError ( "Start failed\n" );

		return DI_IPT_EGENERIC;
	}
	return DI_IPT_SUCCESS;
}

static int INT_MMSH_Reset( STREAM_ACCESS_t *pAccess )
{
	MMSH_ACCESS_SYS_t *pMmshSys = NULL;
	int i=0;

	PrintEnter;

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;
	ASF_HEADER_t old_asfh = pMmshSys->asfh;

	PrintDebug ( "Reset the stream\n" );
	if(pMmshSys->b_broadcast != TRUE)
	{
		pMmshSys->i_start = pAccess->info.i_pos;
	}
	else
	{
		pMmshSys->i_start=0;
	}

	/* */
	pMmshSys->i_packet_sequence = 0;
	pMmshSys->i_packet_used = 0;
	pMmshSys->i_packet_length = 0;
	pMmshSys->p_packet = NULL;

	/* Get the next header FIXME memory loss ? */
	INT_MMSH_GetHeader( pAccess );

	if( pMmshSys->i_header <= 0 )
	{
		PrintError("%s(%d) Error> invalid i_header\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

	ASF_ParseHeader ( &pMmshSys->asfh, pMmshSys->p_header, pMmshSys->i_header );

	PrintDebug ( "packet count=%"PRId64" packet size=%d\n", pMmshSys->asfh.i_data_packets_count, pMmshSys->asfh.i_min_data_packet_size );

#if 1 // junsy
	ASF_SelectStream ( &pMmshSys->asfh,0,1,1,1);
#else
	ASF_SelectStream ( &pMmshSys->asfh,
			var_CreateGetInteger( pAccess, "mms-maxbitrate" ),
			var_CreateGetInteger( pAccess, "mms-all" ),
			var_CreateGetInteger( pAccess, "audio" ),
			var_CreateGetInteger( pAccess, "video" ) );
#endif

	/* Check we have comptible asfh */
	for( i = 1; i < 128; i++ )
	{
		ASF_STREAM_t *p_old = &old_asfh.stream[i];
		ASF_STREAM_t *p_new = &pMmshSys->asfh.stream[i];

		if( p_old->i_cat != p_new->i_cat || p_old->i_selected != p_new->i_selected )
		{
			break;
		}
		/* else { continue ; } */
	}

	if( i < 128 )
	{
		PrintError( "incompatible asf header, restart\n" );
		return INT_MMSH_Restart( pAccess );
	}

	/* */
	pMmshSys->i_packet_used = 0;
	pMmshSys->i_packet_length = 0;

	PrintExit;
	return DI_IPT_SUCCESS;
}

static int INT_MMSH_OpenConnection ( STREAM_ACCESS_t *pAccess )
{
    MMSH_ACCESS_SYS_t *pMmshSys = NULL;
    URL_t    srv;

	PrintEnter;

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;
	VK_MEM_Memset(&srv, 0x0, sizeof(URL_t));
	if(pMmshSys->b_proxy == TRUE)
	{
		VK_MEM_Memcpy(&srv, &pMmshSys->proxy, sizeof(URL_t));
	}
	else
	{
		VK_MEM_Memcpy(&srv, &pMmshSys->url, sizeof(URL_t));
	}

    if( ( pMmshSys->fd = net_ConnectTCP ( srv.psz_host, srv.i_port ) ) < 0 )
    {
        PrintError ( "cannot connect to %s:%d\n", srv.psz_host, srv.i_port );
		PrintExit;
        return DI_IPT_EGENERIC;
    }

    if ( pMmshSys->b_proxy )
    {
        net_Printf (pMmshSys->fd
					, NULL
					, "GET http://%s:%d%s HTTP/1.0\r\n"
					, pMmshSys->url.psz_host
					, pMmshSys->url.i_port
					, ( (pMmshSys->url.psz_path == NULL) || (*pMmshSys->url.psz_path == '\0') ) ?  "/" : pMmshSys->url.psz_path );

        /* Proxy Authentication */
        if( pMmshSys->proxy.psz_username && *pMmshSys->proxy.psz_username )
        {
            char *buf;
            char *b64;

            if ( asprintf( &buf, "%s:%s", pMmshSys->proxy.psz_username, pMmshSys->proxy.psz_password ? pMmshSys->proxy.psz_password : "" ) == -1 )
			{
				PrintExit;
                return DI_IPT_ENOMEM;
			}

            b64 = UTIL_B64Encode ((const char*)buf );
            DD_MEM_Free ( buf );

            net_Printf ( pMmshSys->fd, NULL, "Proxy-Authorization: Basic %s\r\n", b64 );

            DD_MEM_Free ( b64 );
        }
    }
    else
    {
        net_Printf ( pMmshSys->fd
					, NULL
					, "GET %s HTTP/1.0\r\n" "Host: %s:%d\r\n"
					, ( (pMmshSys->url.psz_path == NULL) || (*pMmshSys->url.psz_path == '\0') ) ?  "/" : pMmshSys->url.psz_path
					, pMmshSys->url.psz_host
					, pMmshSys->url.i_port );
    }
	PrintExit;
    return DI_IPT_SUCCESS;
}

/*****************************************************************************
 * INT_MMSH_Describe:
 *****************************************************************************/
static int INT_MMSH_Describe ( STREAM_ACCESS_t  *pAccess, char **ppsz_location )
{
	MMSH_ACCESS_SYS_t *pMmshSys = NULL;
	char         *psz_location = NULL;
	char         *psz=NULL;
	char 		*p=NULL;
	int          i_code=0;
	char		*TempPath=NULL;

	/* Reinit context */
	PrintEnter;

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;

	pMmshSys->b_broadcast = true;
	pMmshSys->i_request_context = 1;
	pMmshSys->i_packet_sequence = 0;
	pMmshSys->i_packet_used = 0;
	pMmshSys->i_packet_length = 0;
	pMmshSys->p_packet = NULL;

	ASF_GenerateGuid ( &pMmshSys->guid );

	if( INT_MMSH_OpenConnection ( pAccess ) )
	{
		PrintError("%s(%d) Error> INT_MMSH_OpenConnection Fail\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

    /* TODO :
	 * corresponding with service provider such as IRT/ZDF.
	 * They may use the particular string for distingushing
	 * valid client whether or not.
	 *
	 * User-Agent String (2009.10.19)
	 * 	HbbTV/1.1.1 (<capabilities>; [<vendorName>]; [<modelName>]; [<softwareVersion>];  [<hardwareVersion>]; <reserved>)
	 */
	 #if 0 /* Describe에서 fail */
	net_Printf ( pMmshSys->fd
				, NULL
				, "Accept: */*\r\n"
				"User-Agent: "DI_IPTUNER_USER_AGENT_STRING"\r\n"
				"Pragma: no-cache,rate=1.000000,stream-time=0,stream-offset=0:0,request-context=%d,max-duration=0\r\n"
				"Pragma: xClientGUID={"GUID_FMT"}\r\n"
				"Connection: Close\r\n"
				, pMmshSys->i_request_context++
				, GUID_PRINT( pMmshSys->guid ) );
	 #endif
	 /* TODO : FIXME (these strings are copied from MS-WMSP without modify) */
	 PrintDebug(
	 "Accept: */*\r\n"
	 "Accept-Language: en-US, *;q=0.1\r\n"
	 "User-Agent: NSPlayer/10.0.0.3802\r\n"
	 "Host: %s\r\n"
	 "X-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\n"
	 "Pragma: xClientGUID={"ASF_GUID_FMT"}\r\n"
	 "Supported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.predstrm, com.microsoft.wm.startupprofile\r\n"
	 "Pragma: no-cache,stream-time=0,stream-offset=0:0,packet-num=4294967295,max-duration=0\r\n"
	 "Pragma: LinkBW=2147483647,rate=1.000,AccelBW=1048576,AccelDuration=10000\r\n"
	 , pMmshSys->url.psz_host, ASF_GUID_PRINT( pMmshSys->guid ));

	 net_Printf(pMmshSys->fd, NULL,
//	 "GET /welcome.asf HTTP/1.0"
	 "Accept: */*\r\n"
	 "Accept-Language: en-US, *;q=0.1\r\n"
	 "User-Agent: NSPlayer/10.0.0.3802\r\n"
	 "Host: %s\r\n"
	 "X-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\n"
	 "Pragma: xClientGUID={"ASF_GUID_FMT"}\r\n"
//	 "Pragma: packet-pair-experiment=1\r\n"
	 "Supported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.predstrm, com.microsoft.wm.startupprofile\r\n"
	 "Pragma: no-cache,stream-time=0,stream-offset=0:0,packet-num=4294967295,max-duration=0\r\n"
	 "Pragma: LinkBW=2147483647,rate=1.000,AccelBW=1048576,AccelDuration=10000\r\n"
	 /* "Pragma: xClientGUID={11223344-1122-1122-1122-AABBCCDDEEFF}\r\n" */
	 /* "Accept-Language: en-US, *;q=0.1 122-1122-1122-AABBCCDDEEFF}\r\n" */
	 , pMmshSys->url.psz_host, ASF_GUID_PRINT( pMmshSys->guid ));

	if( net_Printf ( pMmshSys->fd, NULL, "\r\n" ) < 0 )
	{
		PrintError ( "failed to send request\n" );
		goto error;
	}

	/* Receive the http header */
	if( ( psz = net_Gets (pMmshSys->fd, NULL ) ) == NULL )
	{
		PrintError ( "%s(%d) Error> failed to read answer\n",__FUNCTION__,__LINE__ );
		goto error;
	}
#ifdef MMS_HTTP_DEBUG
	PrintData("%s(%d) : %s\n",__FUNCTION__,__LINE__,psz);
#endif

	if( VK_strncmp( psz, "HTTP/1.", 7 ) )
	{
		PrintError ( "%s(%d) Error> invalid HTTP reply '%s'\n",__FUNCTION__,__LINE__, psz );
		DD_MEM_Free ( psz );
		goto error;
	}

	i_code = VK_atoi( &psz[9] );

	if( i_code >= 400 )
	{
		PrintError ( "error: %s\n", psz );
		DD_MEM_Free ( psz );
		goto error;
	}

	PrintDebug ( "HTTP reply '%s'\n", psz );
	DD_MEM_Free ( psz );
	for( ;; )
	{
		psz = net_Gets ( pMmshSys->fd, NULL );
		PrintData ("%s(%d) : psz=%s\n",__FUNCTION__,__LINE__,psz);

		if( psz == NULL )
		{
			PrintError ( "failed to read answer\n" );
			goto error;
		}

		if( *psz == '\0' )
		{
			DD_MEM_Free ( psz );
			break;
		}

		if( ( p = VK_strchr( psz, ':' ) ) == NULL )
		{
			PrintError ( "malformed header line: %s\n", psz );
			DD_MEM_Free ( psz );
			goto error;
		}
		*p++ = '\0';
		while( *p == ' ' ) p++;

		/* FIXME FIXME test Content-Type to see if it's a plain stream or an
		 * asx FIXME */
		if( !VK_strcasecmp( psz, "Pragma" ) )
		{
			if(NULL != VK_strstr( p, "client"))
			{
				TempPath = VK_strstr( p, "client");
				VK_sscanf(TempPath+10, "%ud", &pMmshSys->i_client_id);
			}

			if( VK_strstr( p, "features" ) )
			{
				/* FIXME, it is a bit badly done here ..... */
				if( VK_strstr( p, "broadcast" ) )
				{
					PrintDebug ( "stream type = broadcast\n" );
					pMmshSys->b_broadcast = true;
				}
				else if( VK_strstr( p, "seekable" ) )
				{
					PrintDebug ( "stream type = seekable\n" );
					pMmshSys->b_broadcast = false;
				}
				else
				{
					PrintError( "unknow stream types (%s)\n", p );
					pMmshSys->b_broadcast = false;
				}
			}
		}
		else if( !VK_strcasecmp( psz, "Location" ) )
		{
			psz_location = strdup( p );
		}

		DD_MEM_Free ( psz );
	}

	/* Handle the redirection */
	if( ( (i_code == 301) || (i_code == 302) || (i_code == 303) || (i_code == 307) ) && psz_location && *psz_location )
	{
		PrintDebug ( "redirection to %s\n", psz_location );
		net_Close ( pMmshSys->fd );
		pMmshSys->fd = -1;
		pAccess->info.i_httpstatecode = i_code;
		*ppsz_location = psz_location;
		return DI_IPT_SUCCESS;
	}

	/* Read the asf header */
	INT_MMSH_GetHeader ( pAccess );

	if( pMmshSys->i_header <= 0 )
	{
		PrintError ( "header size == 0\n" );
		goto error;
	}
	/* close this connection */
	net_Close ( pMmshSys->fd );
	pMmshSys->fd = -1;

	/* *** parse header and get stream and their id *** */
	/* get all streams properties,
	 *
	 * TODO : stream bitrates properties(optional)
	 *        and bitrate mutual exclusion(optional) */
	ASF_ParseHeader ( &pMmshSys->asfh, pMmshSys->p_header, pMmshSys->i_header );
#ifdef MMS_HTTP_DEBUG
	PrintData ("%s(%d) : i_file_size            (%"PRId64")\n",__FUNCTION__,__LINE__,pMmshSys->asfh.i_file_size);
	PrintData ("%s(%d) : i_data_packets_count   (%"PRId64")\n",__FUNCTION__,__LINE__,pMmshSys->asfh.i_data_packets_count);
	PrintData ("%s(%d) : i_min_data_packet_size (%d)\n",__FUNCTION__,__LINE__,pMmshSys->asfh.i_min_data_packet_size);
	//ASF_STREAM_t stream[128];
	{
		int i=0;
		for( i = 1; i < 128; i++ )
		{
			if( pMmshSys->asfh.stream[i].i_cat != ASF_STREAM_UNKNOWN )
			{
				PrintData("%s(%d) : STREAM[%d].i_cat       (%s) \n",__FUNCTION__,__LINE__,i,pMmshSys->asfh.stream[i].i_cat==1?"ASF_VIDEO_STREAM":"ASF_AUDIO_STREAM");
				PrintData("%s(%d) : STREAM[%d].i_bitrate   (%d) \n",__FUNCTION__,__LINE__,i,pMmshSys->asfh.stream[i].i_bitrate);
				PrintData("%s(%d) : STREAM[%d].i_selected  (%d) \n",__FUNCTION__,__LINE__,i,pMmshSys->asfh.stream[i].i_selected);
			}
		}
	}

#endif
	/** Select Stream (ALL) */
	PrintDebug ("%s(%d) : Force Select STREAM (i_bitrate_max(0),b_all(1),b_audio(1),b_video(1)\n",__FUNCTION__,__LINE__);
	ASF_SelectStream ( &pMmshSys->asfh,0,1,1,1);

	PrintExit;
	return DI_IPT_SUCCESS;

error:
	if( pMmshSys->fd > 0 )
	{
		net_Close ( pMmshSys->fd  );
		pMmshSys->fd = -1;
	}
	PrintExit;
	return DI_IPT_EGENERIC;
}

static void INT_MMSH_GetHeader( STREAM_ACCESS_t *pAccess )
{
    MMSH_ACCESS_SYS_t *pMmshSys = NULL;
	chunk_t ck;
	int			nRetryCount_DataRead = 0;
	int			nTimeoutCount_DataRead = 0;
	int			i_ret = -1;

    /* Read the asf header */
	PrintEnter;

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}

	VK_MEM_Memset(&ck, 0x0, sizeof(chunk_t));
	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;

    pMmshSys->i_header = 0;

	if(pMmshSys->p_header != NULL)
	{
		DD_MEM_Free ( pMmshSys->p_header );
		pMmshSys->p_header =NULL;
	}

    pMmshSys->p_header = NULL;

	while(TRUE)
	{
		i_ret = INT_MMSH_GetPacket ( pAccess, &ck );
		if(ck.i_type == H_PACKET )
		{
	        		break;
		}

		if(i_ret == DI_IPT_ETIMEOUT)
		{
			nTimeoutCount_DataRead++;
		}

		if(nTimeoutCount_DataRead >=3)
		{
			PrintDebug("### Fail Timeout ###\n");
			pAccess->info.b_eof = true;
			PrintExit;
			return;
		}

		/* Retry */
		if(nRetryCount_DataRead != 20)
		{
			if(nRetryCount_DataRead==10)
			{
				PrintError("[%s] (%d) nRetryCount_DataRead=10\n", __FUNCTION__,__LINE__);
			}

			VK_TASK_Sleep(20);
			PrintDebug("### Increae Retry (Count=%d) ###\n", nRetryCount_DataRead);
			nRetryCount_DataRead++;
			continue;
		}
		else
		{
			PrintError("[%s] (%d) nRetryCount_DataRead=20\n", __FUNCTION__,__LINE__);

			ck.i_data = 0;
			PrintExit;
			return;
		}
	}

	if( ck.i_data > 0 )
	{
		pMmshSys->i_header += ck.i_data;
		pMmshSys->p_header = DD_MEM_Realloc( pMmshSys->p_header, pMmshSys->i_header );
		VK_memcpy( &pMmshSys->p_header[pMmshSys->i_header - ck.i_data],
		        ck.p_data, ck.i_data );
	}
	else
	{
		PrintError("Cannot read ASF File Header\n");
	}
    PrintDebug ( "complete header size=%d\n", pMmshSys->i_header );

	PrintExit;

	return;
}


/*****************************************************************************
 * Start stream
 ****************************************************************************/
static int INT_MMSH_Start( STREAM_ACCESS_t *pAccess, HINT64 i_pos )
{
    MMSH_ACCESS_SYS_t *pMmshSys = NULL;
    int  i_streams = 0;
    int  i_streams_selected = 0;
    int  i=0;
    char *psz = NULL;

	UNUSED_PARAM(i_pos);

    PrintDebug ( "starting stream\n" );

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;

    for( i = 1; i < 128; i++ )
    {
        if( pMmshSys->asfh.stream[i].i_cat == MMS_ASF_STREAM_UNKNOWN )
		{
            continue;
		}

        i_streams++;

        if( pMmshSys->asfh.stream[i].i_selected )
		{
            i_streams_selected++;
		}
    }
    if( i_streams_selected <= 0 )
    {
        PrintError ( "no stream selected\n" );
        return DI_IPT_EGENERIC;
    }

    if( INT_MMSH_OpenConnection ( pAccess ) )
	{
        return DI_IPT_EGENERIC;
	}
	if(pMmshSys->b_broadcast != TRUE)
	{
		if(s_pcUserAgentInfo != NULL)
		{
		    net_Printf ( pMmshSys->fd
						, NULL
						, "Accept: */*\r\n"
		                "User-Agent: %s\r\n", s_pcUserAgentInfo );
		}
		else
		{
		    net_Printf ( pMmshSys->fd
						, NULL
						, "Accept: */*\r\n"
		                "User-Agent: "DI_IPTUNER_USER_AGENT_STRING"\r\n" );
		}

	    if( pMmshSys->b_broadcast )
	    {
	        net_Printf ( pMmshSys->fd, NULL,
	                    "Pragma: no-cache,rate=1.000,request-context=%d\r\n",
	                    pMmshSys->i_request_context++ );
	    }
	    else
	    {
			#if 0
	        net_Printf ( pMmshSys->fd, NULL,
	                    "Pragma: no-cache,rate=1.000000,stream-time=0,stream-offset=%u:%u,request-context=%d,max-duration=0\r\n",
	                    (HUINT32)((i_pos >> 32)&0xffffffff),
	                    (HUINT32)(i_pos&0xffffffff),
	                    pMmshSys->i_request_context++ );
			#endif
			/* stream-time in milisec */
			net_Printf ( pMmshSys->fd, NULL,
	                    "Pragma: no-cache,rate=1.000,stream-time=%d,stream-offset=4294967295:4294967295,packet-num=4294967295,request-context=%d,max-duration=0\r\n",
	                    (HINT32)(i_pos*1000), pMmshSys->i_request_context++ );
	    }

	    net_Printf ( pMmshSys->fd, NULL,
	                "Pragma: xPlayStrm=1\r\n"
	                "Pragma: xClientGUID={"ASF_GUID_FMT"}\r\n"
	                "Pragma: stream-switch-count=%d\r\n"
	                "Pragma: stream-switch-entry=",
	                ASF_GUID_PRINT( pMmshSys->guid ),
	                i_streams);
	}
	else
	{
		PrintDebug("###i_client_id=%ud, i_playlist_gen_id=%ud, i_streams=%d, "ASF_GUID_FMT" ###\n",pMmshSys->i_client_id, pMmshSys->i_playlist_gen_id, i_streams, ASF_GUID_PRINT(pMmshSys->guid));

		PrintDebug(
		 "User-Agent: NSPlayer/10.0.0.3802\r\n"
		 "Host: %s\r\n"
		 "Accept: */*\r\n"
		 "Accept-Language: en-US, *;q=0.1\r\n"
		 "Pragma: xKeepAliveInPause=1\r\n"
		 "Pragma: client-id=%ud\r\n"
		 "Connection: Keep-Alive\r\n"
		 "Pragma: playlist-gen-id=%ud\r\n"
		 "Pragma: xClientGUID={"ASF_GUID_FMT"}\r\n"
		 "Pragma: xPlayStrm=1\r\n"
		 "Supported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.predstrm, com.microsoft.wm.startupprofile\r\n"
		 "Pragma: no-cache,stream-time=0,stream-offset=4294967295:4294967295,packet-num=4294967295,max-duration=0\r\n"
		 "Pragma: playlist-seek-id=%ud\r\n"
		 "Pragma: LinkBW=4294967295,rate=1.000,AccelBW=4294967295,AccelDuration=500000\r\n"
		 "Pragma: stream-switch-count=%d\r\n"
		 "Pragma: stream-switch-entry="
	     , pMmshSys->url.psz_host, pMmshSys->i_client_id, pMmshSys->i_playlist_gen_id, ASF_GUID_PRINT(pMmshSys->guid), pMmshSys->i_playlist_gen_id, i_streams);

		net_Printf(pMmshSys->fd, NULL,
	//	 "GET /welcome.asf HTTP/1.0"
		 "User-Agent: NSPlayer/10.0.0.3802\r\n"
		 "Host: %s\r\n"
		 "Accept: */*\r\n"
		 "Accept-Language: en-US, *;q=0.1\r\n"
		 "Pragma: xKeepAliveInPause=1\r\n"
		 "Pragma: client-id=%ld\r\n"
		 "Connection: Keep-Alive\r\n"
		 "Pragma: playlist-gen-id=%ld\r\n"
		 "Pragma: xClientGUID={"ASF_GUID_FMT"}\r\n"
		 "Pragma: xPlayStrm=1\r\n"
		 "Supported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.predstrm, com.microsoft.wm.startupprofile\r\n"
		 "Pragma: no-cache,stream-time=0,stream-offset=4294967295:4294967295,packet-num=4294967295,max-duration=0\r\n"
		 "Pragma: playlist-seek-id=%ld\r\n"
		 "Pragma: LinkBW=4294967295,rate=1.000,AccelBW=4294967295,AccelDuration=500000\r\n"
		 "Pragma: stream-switch-count=%d\r\n"
		 "Pragma: stream-switch-entry="
	     , pMmshSys->url.psz_host, pMmshSys->i_client_id, pMmshSys->i_playlist_gen_id, ASF_GUID_PRINT(pMmshSys->guid), pMmshSys->i_playlist_gen_id,
	     i_streams);
	}

    for( i = 1; i < 128; i++ )
    {
        if( pMmshSys->asfh.stream[i].i_cat != MMS_ASF_STREAM_UNKNOWN )
        {
            int i_select = 2;
            if( pMmshSys->asfh.stream[i].i_selected )
            {
                i_select = 0;
            }
            net_Printf ( pMmshSys->fd, NULL,
                        "ffff:%d:%d ", i, i_select );
			PrintDebug("ffff:%d:%d ", i, i_select );
        }
    }

    PrintDebug ("\r\n");
    net_Printf ( pMmshSys->fd, NULL, "\r\n" );
//    net_Printf( pMmshSys->fd, NULL, "Connection: Close\r\n" );

    if( net_Printf ( pMmshSys->fd, NULL, "\r\n" ) < 0 )
    {
        PrintError ( "failed to send request\n" );
        return DI_IPT_EGENERIC;
    }

    psz = net_Gets ( pMmshSys->fd, NULL );
    if( psz == NULL )
    {
        PrintError ( "cannot read data 0\n" );
        return DI_IPT_EGENERIC;
    }
#ifdef MMS_HTTP_DEBUG
	PrintData("%s(%d) : %s\n",__FUNCTION__,__LINE__,psz);
#endif

    if( VK_atoi( &psz[9] ) >= 400 )
    {
        PrintError ( "error: %s\n", psz );
        DD_MEM_Free ( psz );
        return DI_IPT_EGENERIC;
    }
    PrintDebug ( "HTTP reply '%s'\n", psz );
    DD_MEM_Free ( psz );

    /* FIXME check HTTP code */
    for( ;; )
    {
        psz = net_Gets ( pMmshSys->fd, NULL );
        if( psz == NULL )
        {
            PrintError ( "cannot read data 1\n" );
            return DI_IPT_EGENERIC;
        }
#ifdef MMS_HTTP_DEBUG
		PrintData("%s(%d) : %s\n",__FUNCTION__,__LINE__,psz);
#endif
        if( *psz == '\0' )
        {
            DD_MEM_Free ( psz );
            break;
        }
        PrintDebug ( "%s\n", psz );
        DD_MEM_Free ( psz );
    }

    pMmshSys->i_packet_used   = 0;
    pMmshSys->i_packet_length = 0;

    return DI_IPT_SUCCESS;
}

/*****************************************************************************
 * closing stream
 *****************************************************************************/
static void INT_MMSH_Stop ( STREAM_ACCESS_t *pAccess )
{
    MMSH_ACCESS_SYS_t *pMmshSys = pAccess->p_sys;

    PrintDebug ( "closing stream\n" );
    if((pMmshSys != NULL) && ( pMmshSys->fd > 0 ))
    {
        net_Close ( pMmshSys->fd );
        pMmshSys->fd = -1;
    }
}

#if SUPPORT_KEEPALIVE
/*****************************************************************************
 * Send KeepAlive
 ****************************************************************************/
static int INT_MMSH_SendKeepAlive(STREAM_ACCESS_t *pAccess)
{
    MMSH_ACCESS_SYS_t *pMmshSys = NULL;
#if 0
    int  i_streams = 0;
    int  i_streams_selected = 0;
    char *psz = NULL;
#endif
    PrintDebug ("Send Keep-Alive\n");

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	pMmshSys = (MMSH_ACCESS_SYS_t *)pAccess->p_sys;

#if 0
    PrintDebug ("GET http://%s:%d%s HTTP/1.0\r\n"
				, pMmshSys->url.psz_host, pMmshSys->url.i_port, ((pMmshSys->url.psz_path == NULL) || (*pMmshSys->url.psz_path == '\0')) ?  "/" : pMmshSys->url.psz_path );
#endif
    PrintDebug("POST %s HTTP/1.0\r\n", ((pMmshSys->url.psz_path == NULL) || (*pMmshSys->url.psz_path == '\0'))? "/" : pMmshSys->url.psz_path);
	PrintDebug("Accept: */*\r\n"
	"User-Agent: NSPlayer/10.0.0.3802\r\n"
	"Host: SampleServer\r\n"
	"Pragma: xClientGUID={"GUID_FMT"}\r\n"
	"X-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\n"
	"Pragma: xKeepAliveInPause=1\r\n"
	"Pragma: client-id=%ld\r\n"
	"Content-Length: 0\r\n"
	"Connection: Keep-Alive\r\n"
//	"Pragma: playlist-gen-id=%ld\r\n"
    , GUID_PRINT(pMmshSys->guid), pMmshSys->i_client_id);
#if 1
#if 1
    net_Printf ( pMmshSys->fd
				, NULL
				, "POST %s HTTP/1.0\r\n"
				, ( (pMmshSys->url.psz_path == NULL) || (*pMmshSys->url.psz_path == '\0') ) ?  "/" : pMmshSys->url.psz_path);
#else
    net_Printf (pMmshSys->fd
				, NULL
				, "GET http://%s:%d%s HTTP/1.0\r\n"
				, pMmshSys->url.psz_host
				, pMmshSys->url.i_port
				, ( (pMmshSys->url.psz_path == NULL) || (*pMmshSys->url.psz_path == '\0') ) ?  "/" : pMmshSys->url.psz_path );
#endif
#endif
	net_Printf(pMmshSys->fd, NULL,
	"Accept: */*\r\n"
	"User-Agent: NSPlayer/10.0.0.3802\r\n"
	"Host: SampleServer\r\n"
	"Pragma: xClientGUID={"GUID_FMT"}\r\n"
	"X-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\n"
	"Pragma: xKeepAliveInPause=1\r\n"
	"Pragma: client-id=%ld\r\n"
	"Content-Length: 0\r\n"
	"Connection: Keep-Alive\r\n"
//	"Pragma: playlist-gen-id=%ld\r\n"
    , GUID_PRINT(pMmshSys->guid), pMmshSys->i_client_id);

//    net_Printf( pMmshSys->fd, NULL, "Connection: Close\r\n" );

    if( net_Printf ( pMmshSys->fd, NULL, "\r\n" ) < 0 )
    {
        PrintError ( "failed to send request\n" );
        return DI_IPT_EGENERIC;
    }
#if 0
    psz = net_Gets ( pMmshSys->fd, NULL );
    if( psz == NULL )
    {
        PrintError ( "cannot read data 0\n" );
        return DI_IPT_EGENERIC;
    }
#ifdef MMS_HTTP_DEBUG
	PrintData("%s(%d) : %s\n",__FUNCTION__,__LINE__,psz);
#endif

    if( VK_atoi( &psz[9] ) >= 400 )
    {
        PrintError ( "error: %s\n", psz );
        DD_MEM_Free ( psz );
        return DI_IPT_EGENERIC;
    }
    PrintDebug ( "HTTP reply '%s'\n", psz );
    DD_MEM_Free ( psz );

    /* FIXME check HTTP code */
    for( ;; )
    {
        psz = net_Gets ( pMmshSys->fd, NULL );
        if( psz == NULL )
        {
            PrintError ( "cannot read data 1\n" );
            return DI_IPT_EGENERIC;
        }
#ifdef MMS_HTTP_DEBUG
		PrintData("%s(%d) : %s\n",__FUNCTION__,__LINE__,psz);
#endif
        if( *psz == '\0' )
        {
            DD_MEM_Free ( psz );
            break;
        }
        PrintDebug ( "%s\n", psz );
        DD_MEM_Free ( psz );
    }
#endif
    return DI_IPT_SUCCESS;
}
#endif

/*****************************************************************************
 * get packet
 *****************************************************************************/
static int INT_MMSH_GetPacket ( STREAM_ACCESS_t * pAccess, chunk_t *p_ck )
{
	int i_ret = DI_IPT_SUCCESS;
    MMSH_ACCESS_SYS_t *pMmshSys = pAccess->p_sys;
    int restsize = 0;
    int readsize=0;
    char *pTempPath=NULL;
#ifdef MMS_HTTP_DEBUG
    char *psz = NULL;
#endif

	PrintEnter;

	if(pMmshSys == NULL)
	{
		PrintError ("%s (%d) Error> pMmshSys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

    /* chunk_t */
    VK_memset ( p_ck, 0, sizeof( chunk_t ) );
    VK_memset ( pMmshSys->buffer, 0, sizeof( pMmshSys->buffer ) );

    /* Read the chunk header */
    /* Some headers are short, like C_PACKET. Reading 12 bytes will cause us
     * to lose synchronization with the stream. Just read to the length
     * (4 bytes), decode and then read up to 8 additional bytes to get the
     * entire header.
     */
	readsize = net_Read ( pMmshSys->fd, NULL, pMmshSys->buffer, 4, true );
    if( readsize < 4 )
    {
       PrintError ( "cannot read data 2(%d)\n", readsize );

		if(readsize >=0)
		{
			i_ret = DI_IPT_EGENERIC;
		}
		else
		{
			i_ret = readsize;
		}

       return i_ret;
    }
#ifdef MMS_HTTP_DEBUG
#if 0
	{
		int i=0;
		for(i=0;i<4;i++)
		{
			PrintData ("0x%x\n",(char*)pMmshSys->buffer[i]);
		}
	}
#endif
#endif

    p_ck->i_type = GetWordLE ( pMmshSys->buffer );
    p_ck->i_size = GetWordLE ( pMmshSys->buffer + 2);

#ifdef MMS_HTTP_DEBUG
	PrintData("%s(%d) i_type=0x%x\n",__FUNCTION__,__LINE__,p_ck->i_type);
	PrintData("%s(%d) i_size=0x%x, %d\n",__FUNCTION__,__LINE__,p_ck->i_size,p_ck->i_size);
#endif

    restsize = p_ck->i_size;

    if( restsize > 8 )
	{
        restsize = 8;
	}

    if( net_Read ( pMmshSys->fd, NULL, pMmshSys->buffer + 4, restsize, true ) < restsize )
    {
        PrintError ( "cannot read data 3\n" );
        return DI_IPT_EGENERIC;
    }

//	PrintData ("%s(%d) : info.i_size        =          %"PRId64"\n",__FUNCTION__,__LINE__,pMmsAccess->info.i_size);

#ifdef MMS_HTTP_DEBUG
//	if (restsize > 0)
//	{
//		PrintData ( "%s\n",pMmshSys->buffer);
//	}
#endif

    p_ck->i_sequence  = GetDWordLE( pMmshSys->buffer + 4);
    p_ck->i_unknown   = GetWordLE( pMmshSys->buffer + 8);

//	PrintData("### %s(%d) i_sequence=0x%x, %d ###\n",__FUNCTION__,__LINE__,p_ck->i_sequence,p_ck->i_sequence);
//	PrintData("### %s(%d) i_unknown=0x%x, %d ###\n",__FUNCTION__,__LINE__,p_ck->i_unknown,p_ck->i_unknown);

    /* Set i_size2 to 8 if this header was short, since a real value won't be
     * present in the buffer. Using 8 avoid reading additional data for the
     * packet.
     */
    if( restsize < 8 )
	{
        p_ck->i_size2 = 8;
	}
    else
	{
        p_ck->i_size2 = GetWordLE( pMmshSys->buffer + 10);
	}

    p_ck->p_data      = pMmshSys->buffer + 12;
    p_ck->i_data      = p_ck->i_size2 - 8;

#ifdef MMS_HTTP_DEBUG
    switch(p_ck->i_type)
    {
		case C_PACKET:
		case C_PACKET_B:
			{
				PrintDebug("### This is $C Packet ###\n");
				break;
			}
		case D_PACKET:
		case D_PACKET_B:
			{
				PrintDebug("### This is $D Packet ###\n");
				break;
			}
		case E_PACKET:
		case E_PACKET_B:
			{
				PrintDebug("### This is $E Packet ###\n");
				break;
			}
		case H_PACKET:
		case H_PACKET_B:
			{
				PrintDebug("### This is $H Packet ###\n");
				break;
			}
		case M_PACKET:
		case M_PACKET_B:
			{
				PrintDebug("### This is $M Packet ###\n");
				break;
			}
		case P_PACKET:
		case P_PACKET_B:
			{
				PrintDebug("### This is $P Packet ###\n");
				break;
			}
		case T_PACKET:
		case T_PACKET_B:
			{
				PrintDebug("### This is $T Packet ###\n");
				break;
			}
		default:
			{
				PrintDebug("### This is Unknown Packet ###\n");
		        PrintError ( "invalid chunk FATAL (0x%x)\n", p_ck->i_type );

				psz = net_Gets ( pMmshSys->fd, NULL );
			    if( psz == NULL )
			    {
			        PrintError ( "cannot read data 0\n" );
			        return DI_IPT_EGENERIC;
			    }

				PrintData("%s(%d) : %s %s\n",__FUNCTION__,__LINE__, pMmshSys->buffer, psz);

			    if( VK_atoi( &psz[9] ) >= 400 )
			    {
			        PrintError ( "error: %s\n", psz );
			        DD_MEM_Free ( psz );
			        return DI_IPT_EGENERIC;
			    }
			    PrintDebug ( "HTTP reply %s '%s'\n", pMmshSys->buffer, psz );
			    DD_MEM_Free ( psz );

			    /* FIXME check HTTP code */
			    for( ;; )
			    {
			        psz = net_Gets ( pMmshSys->fd, NULL );
			        if( psz == NULL )
			        {
			            PrintError ( "cannot read data 1\n" );
			            return DI_IPT_EGENERIC;
			        }

					PrintData("%s(%d) : %s\n",__FUNCTION__,__LINE__,psz);
			        if( *psz == '\0' )
			        {
			            DD_MEM_Free ( psz );
			            break;
			        }
			        PrintDebug ( "%s\n", psz );
			        DD_MEM_Free ( psz );
			    }

		        return DI_IPT_EGENERIC;
				break;
			}
    }
#endif

    if((p_ck->i_type == E_PACKET) || (p_ck->i_type == E_PACKET_B)) /* $E Packet */  // Transfer complete
    {
        if( p_ck->i_sequence == 0 )
        {
            PrintError( "EOF\n" );
			PrintExit;
            return DI_IPT_EGENERIC;
        }
        else
        {
            PrintError( "next stream following\n" );
			PrintExit;
            return DI_IPT_EGENERIC;
        }
    }
    else if((p_ck->i_type == C_PACKET) || (p_ck->i_type == C_PACKET_B)) /* $C Packet */
    {
        /* C_PACKET is CHUNK_TYPE_RESET: a new stream will follow with a sequence of 0 */
        PrintError( "next stream following (reset) seq=%d\n", p_ck->i_sequence  );
		PrintExit;
        return DI_IPT_EGENERIC;
    }

	PrintData("%s(%d) i_data=%d\n",__FUNCTION__,__LINE__,p_ck->i_data);

    if( (p_ck->i_data > 0) && ((readsize = net_Read(pMmshSys->fd, NULL, &pMmshSys->buffer[12], p_ck->i_data, true)) < p_ck->i_data) )
	{
        PrintError ( "cannot read data 4\n" );
		PrintExit;
        return DI_IPT_EGENERIC;
	}

	if((p_ck->i_type != H_PACKET) && (p_ck->i_type != D_PACKET) && (p_ck->i_type != D_PACKET_B))
	{
#ifdef MMS_HTTP_DEBUG
		if((p_ck->i_type == M_PACKET) || (p_ck->i_type == M_PACKET_B) || (p_ck->i_type == 0x525b)) /* $M(Metadata) Packet */
		{
			PrintDebug("DUMP\n=======================================\n%s(%d)\n=======================================\n", &pMmshSys->buffer[12], readsize);
		}
#endif

		if((p_ck->i_type == M_PACKET) || (p_ck->i_type == M_PACKET_B))
		{
			if(NULL != VK_strstr((char *)&pMmshSys->buffer[12], "broadcast"))
			{
				pTempPath = VK_strstr((char *)&pMmshSys->buffer[12], "broadcast");
				VK_sscanf(pTempPath+13, "%ud", &pMmshSys->i_broadcast_id);
			}

			if(NULL != VK_strstr((char *)&pMmshSys->buffer[12], "playlist"))
			{
				pTempPath = VK_strstr((char *)&pMmshSys->buffer[12], "playlist");
				VK_sscanf(pTempPath+16, "%ud", &pMmshSys->i_playlist_gen_id);
			}

//			PrintData("### i_broadcast_id=%ld, l_playlist_gen_id=%ld ###\n", pMmshSys->i_broadcast_id, pMmshSys->i_playlist_gen_id);
		}
	}

#if 0
    if( (pMmshSys->i_packet_sequence != 0) &&
        (p_ck->i_sequence != pMmshSys->i_packet_sequence) )
    {
        PrintError( "packet lost ? (%d != %d)\n", p_ck->i_sequence, pMmshSys->i_packet_sequence );
    }
#endif

    pMmshSys->i_packet_sequence = p_ck->i_sequence + 1;
    pMmshSys->i_packet_used   = 0;
    pMmshSys->i_packet_length = p_ck->i_data;
    pMmshSys->p_packet        = p_ck->p_data;

	PrintExit;
    return DI_IPT_SUCCESS;
}

void MMSH_SetUserAgentInfo (HINT8 *pUserAgentInfo)
{
	if(s_pcUserAgentInfo != NULL)
	{
		VK_MEM_Free(s_pcUserAgentInfo);
		s_pcUserAgentInfo = NULL;
	}

	if(pUserAgentInfo != NULL)
	{
		s_pcUserAgentInfo = VK_StrDup(pUserAgentInfo);
	}
}

