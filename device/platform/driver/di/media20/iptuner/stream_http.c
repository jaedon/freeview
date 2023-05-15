/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   stream_http.c  $
 * Version:         $Revision:   1.0  $
 * Original Author:     SungYong Jeon $
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date: Mon May 20 10:20:48 KST 2009 $
 * File Description:    HTTP Stream Controller
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                               */
/* All rights reserved.                                             */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

#include <assert.h>

//#define _GNU_SOURCE // for strcasestr,asprintf
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>

#include "htype.h"
#include "di_uart.h"
#include "vkernel.h"
#include "stream_global.h"

#include "di_iptuner_config.h"
#include "di_iptuner_common.h"
#include "stream_http.h"
#include "transport_tcp.h"
#include "transport_tls.h"

#include "util_md5.h"
#include "util_url.h"
#include "util_b64_encode.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define STREAM_HTTP_DEBUG
//#define DUMP_HTTP_RESPONSE
//#define DUMP_HTTP_STREAM

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef STREAM_HTTP_DEBUG
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

#if defined(DUMP_HTTP_RESPONSE)
#define DumpHttpResponse(X)		Util_Print_Dump(X, VK_strlen(X), 0,1);
#else
#define DumpHttpResponse(X)		while (0) ((int (*)(char *, ...)) 0)
#endif
#if defined(DUMP_HTTP_STREAM)
#define DumpHttpStream(X,len,pos)		Util_Print_Dump(X, len, pos,1);
#else
#define DumpHttpStream(X,len,pos)		while (0) ((int (*)(char *, ...)) 0)
#endif
#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define DumpHttpResponse(X)		while (0) ((int (*)(char *, ...)) 0)
#define DumpHttpStream(X,len,pos)		while (0) ((int (*)(char *, ...)) 0)

#endif /* The END of DEBUG DEFINE */


#define HTTP_READ_REDIRECTON_ERROR_RETRYCOUNT	10
#define HTTP_CONNECT_MAX_REDIRECTION	10
#define HTTP_CONNECT_ERROR_RETRYCOUNT	10
#define HTTP_CONNECT_ERROR_WAITTIME		100


#define UNUSED_PARAM(x)  ((void)x)
/* End #define */

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

static int INT_HTTP_OpenWithCookies ( STREAM_ACCESS_t *pAccess, char *cookies );

/** About HTTP Session */
#if 0 /* __RECONNECT_WITH_ORIGINAL_URL_YOUVIEW__ */
static int INT_HTTP_PrepareConnect(STREAM_ACCESS_t *pAccess, const char *pUrl);
#endif
static int INT_HTTP_Connect( STREAM_ACCESS_t *pAccess, HINT64 i_tell );
static int INT_HTTP_Request ( STREAM_ACCESS_t *pAccess, HINT64 i_tell );
static int INT_HTTP_Disconnect ( STREAM_ACCESS_t *pAccess);

/**
 * About HTTP Cookie
 * Small Cookie utilities. Cookies support is partial.
 */
#if 0
/* XXX : test code (cookie code) : by junsy */
static char *INT_HTTP_GetCookieContent ( const char * cookie );
static char *INT_HTTP_GetCookieDomain  ( const char * cookie );
static char *INT_HTTP_GetCookieName ( const char * cookie );
#endif
#if 0 /* Commented with warning: 'INT_HTTP_AppendCookie' defined but not used */
static void INT_HTTP_AppendCookie ( char * cookies, char * cookie );
#endif /* Commented with warning: 'INT_HTTP_AppendCookie' defined but not used */

/* About Authentication */
static char *INT_HTTP_AuthGetParm ( const char *psz_header, const char *psz_param );
static char *INT_HTTP_AuthGetParamNoQuotes ( const char *psz_header, const char *psz_param );
static char *INT_HTTP_AuthDigest ( STREAM_ACCESS_t *pAccess, URL_t *p_url, HTTP_AUTH_t *p_auth, const char *psz_method );
static void INT_HTTP_AuthParseHeader ( STREAM_ACCESS_t *pAccess, const char *psz_header, HTTP_AUTH_t *p_auth );
static void INT_HTTP_AuthReply ( STREAM_ACCESS_t *p_acces, const char *psz_prefix, URL_t *p_url, HTTP_AUTH_t *p_auth );
static int INT_HTTP_AuthCheckReply ( STREAM_ACCESS_t *pAccess, const char *psz_header, URL_t *p_url, HTTP_AUTH_t *p_auth );
static void INT_HTTP_AuthReset ( HTTP_AUTH_t *p_auth );

/* HTTP chunked-TE */
static HBOOL INT_HTTP_CheckHex(HINT8 ch);

static void INT_HTTP_DlnaRequestField(STREAM_ACCESS_t *pAccess, HINT64 i_tell );
static int INT_HTTP_DlnaResponseField(STREAM_ACCESS_t *pAccess, char *pcFieldName, char *pcValue, HINT64 i_tell);
/* End static function prototype */


int STREAM_HTTP_Open ( STREAM_ACCESS_t *pAccess )
{
	int nRet = DI_IPT_SUCCESS;

	PrintEnter;

	PrintDebug ("%s(%d) pAccess (%d) pAccess->psz_path(%s) pAccess->psz_user_agent(%s)\n"
		,__FUNCTION__
		,__LINE__
		, (HUINT32) pAccess
		, pAccess->psz_path
		, pAccess->psz_user_agent );
	if(NULL == pAccess)
	{
        PrintError ( "%s (%d) Error> invalid pAccess Handle (NULL)\n",__FUNCTION__,__LINE__ );
		nRet = DI_IPT_EGENERIC;
	}
	else
	{
#if SUPPORT_HUMAX_COOKIE
		if(NULL != pAccess->psz_cookie)
		{
			nRet = INT_HTTP_OpenWithCookies( pAccess, pAccess->psz_cookie);
		}
		else
		{
			nRet = INT_HTTP_OpenWithCookies( pAccess, NULL );
		}
#else
    	nRet = INT_HTTP_OpenWithCookies( pAccess, NULL );
#endif
		if ( nRet != DI_IPT_SUCCESS)
		{
			PrintError ( "%s (%d) Error> INT_HTTP_OpenWithCookies fail.\n",__FUNCTION__,__LINE__ );
		}
	}

	PrintExit;
    return nRet;
}

#ifdef CONFIG_DLNA_CERTIFICATION
void STREAM_HTTP_Disconnect ( STREAM_ACCESS_t *pAccess )
{
	(void)INT_HTTP_Disconnect ( pAccess );
}
#endif

/*****************************************************************************
 * STREAM_HTTP_Close:
 *****************************************************************************/
void STREAM_HTTP_Close ( STREAM_ACCESS_t *pAccess )
{
	HTTP_ACCESS_SYS_t *pHttpSys=NULL;

	PrintEnter;

	if(pAccess == NULL)
	{
		PrintError ( "%s (%d) Error> invalid pAccess Handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return;
	}

	pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;

	if(pHttpSys==NULL)
	{
		PrintError ( "%s (%d) Error> invalid pHttpSys Handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return;
	}

	UTIL_UrlClean ( &pHttpSys->url );

	INT_HTTP_AuthReset ( &pHttpSys->auth );

	UTIL_UrlClean ( &pHttpSys->proxy );

	INT_HTTP_AuthReset ( &pHttpSys->proxy_auth );

	if(pHttpSys->psz_protocol != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_protocol );
		pHttpSys->psz_protocol = NULL;
	}

	if(pHttpSys->psz_mime != NULL)
	{
		VK_free ( pHttpSys->psz_mime );
		pHttpSys->psz_mime = NULL;
	}

	if(pHttpSys->psz_pragma != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_pragma );
		pHttpSys->psz_pragma = NULL;
	}

	if(pHttpSys->psz_location != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_location );
		pHttpSys->psz_location = NULL;
	}

	if(pHttpSys->psz_icy_name != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_name );
		pHttpSys->psz_icy_name = NULL;
	}

	if(pHttpSys->psz_icy_genre != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_genre );
		pHttpSys->psz_icy_genre = NULL;
	}

	if(pHttpSys->psz_icy_title != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_title );
		pHttpSys->psz_icy_title = NULL;
	}

	if(pHttpSys->psz_user_agent != NULL)
	{
		VK_free ( pHttpSys->psz_user_agent );
		pHttpSys->psz_user_agent = NULL;
	}
	/** free youview indexfile timeline field */
	if(pHttpSys->psz_index != NULL)
	{
		VK_free ( pHttpSys->psz_index );
		pHttpSys->psz_index = NULL;
	}

	/** free youview bytepersecond timeline field */
	if(pHttpSys->psz_bps != NULL)
	{
		VK_free ( pHttpSys->psz_bps );
		pHttpSys->psz_bps = NULL;
	}

	(void)INT_HTTP_Disconnect ( pAccess );

	if(pHttpSys->p_tls != NULL)
	{
		if(pHttpSys->p_tls->p_ctx != NULL)
		{
			(void)TLS_Client_Release(pHttpSys->p_tls);
		}

		if(pHttpSys->p_tls->psz_ca_name != NULL)
		{
		    DD_MEM_Free ( pHttpSys->p_tls->psz_ca_name);
			pHttpSys->p_tls->psz_ca_name = NULL;
		}

		if(pHttpSys->p_tls->psz_ca_path != NULL)
		{
		    DD_MEM_Free ( pHttpSys->p_tls->psz_ca_path);
			pHttpSys->p_tls->psz_ca_path = NULL;
		}

		if(pHttpSys->p_tls->psz_certi_path != NULL)
		{
		    DD_MEM_Free ( pHttpSys->p_tls->psz_certi_path);
			pHttpSys->p_tls->psz_certi_path = NULL;
		}

		if(pHttpSys->p_tls->psz_prikey_path != NULL)
		{
		    DD_MEM_Free ( pHttpSys->p_tls->psz_prikey_path);
			pHttpSys->p_tls->psz_prikey_path = NULL;
		}

		if(pHttpSys->p_tls->psz_prikey_passwd != NULL)
		{
		    DD_MEM_Free ( pHttpSys->p_tls->psz_prikey_passwd);
			pHttpSys->p_tls->psz_prikey_passwd = NULL;
		}

		if(pHttpSys->p_tls->cbCerficate != NULL)
		{
			pHttpSys->p_tls->cbCerficate->callback = NULL;
			DD_MEM_Free(pHttpSys->p_tls->cbCerficate);
			pHttpSys->p_tls->cbCerficate = NULL;
		}

	    DD_MEM_Free ( pHttpSys->p_tls );
		pHttpSys->p_tls = NULL;
	}

#if SUPPORT_HUMAX_COOKIE
	/* DHKIM: pHttpSys->cookies 주소에 대한 Memory Free는 상위에서 해제함. */
	pHttpSys->cookies = NULL;
#else
	if( pHttpSys->cookies )
	{
		/* junsy : test code : delete cookie*/
		PrintError("%s(%d) Error> Cookie is not supported yet.",__FUNCTION__,__LINE__);
	}
#endif

	if(pHttpSys)
	{
		DD_MEM_Free ( pHttpSys );
		pAccess->p_sys=NULL;
	}

	PrintDebug ("%s(%d) pAccess (%d) \n",__FUNCTION__,__LINE__, (HUINT32) pAccess);
	PrintExit;

	return;
}

/*****************************************************************************
 * STREAM_HTTP_Read: STREAM_HTTP_Read up to i_len bytes from the http connection and place in
 * p_buffer. Return the actual number of bytes read
 *****************************************************************************/

ssize_t STREAM_HTTP_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len )
{
	HTTP_ACCESS_SYS_t *pHttpSys = NULL;
	char *psz = NULL;
	int i_read = 0;
#if defined(HDR_FOX_T2) || defined(HD_FOX_T2) || defined(HD5500T_PATCH)
	int 	error_retrycount =0;
#endif
	HTTP_CHUNK_t *pHttpChunk = NULL;

#if 0 /* for CANVAS : dragon */
	PrintEnter;
#endif

	if(NULL == pAccess)
	{
		PrintError ("%s(%d) : Error> pAccess is NULL ! \n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}

	if(NULL == p_buffer)
	{
		PrintError ("%s(%d) : Error> p_buffer is NULL ! \n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}

	if (NULL == pAccess->p_sys)
	{
		PrintError ("%s(%d) : Error> pAccess->p_sys is NULL ! \n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}

	pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
	if( pHttpSys->fd < 0 )
	{
		pAccess->info.b_eof = TRUE;
		PrintDebug ("%s(%d) : stream_read returns for EOF\n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}

	if( pAccess->info.i_size >= 0 && i_len + pAccess->info.i_pos > pAccess->info.i_size ) /* && (pHttpSys->b_has_size) */
	{
		if( ( i_len = pAccess->info.i_size - pAccess->info.i_pos ) == 0 )
		{
			pAccess->info.b_eof = TRUE;
			PrintDebug ("%s(%d) : stream_read returns for EOF\n",__FUNCTION__,__LINE__);
			PrintExit;
			return 0;
		}
	}

	if (pHttpSys->b_chunked == TRUE)
	{
		/* If chunked-TE is enabled and prev left size isn't remained,
		   this is new chunk -> get the next chunk-size */
		pHttpChunk = &pHttpSys->t_chunk;
		if ((pHttpChunk->i_left_size <= 0) && (pHttpChunk->b_last_chunk == TRUE))
		{
			PrintError("%s(%d) : Got last chunk --> EOF(left:%llu)\n",
						__FUNCTION__,__LINE__,
						pHttpChunk->i_left_size);
			pAccess->info.b_eof = TRUE;
			if(pAccess->t_dlna.b_time_seek == TRUE)
			{
				pAccess->info.i_size = pAccess->info.i_pos;
			}

			PrintExit;
			return 0;
		}

		/*
		 * RFC-2616, 3.6.1 Chunked Transfer Coding
		 * Chunked-Body    = *chunk
		 *                   last-chunk
		 *                   trailer
		 *                   CRLF
		 * chunk           = chunk-size [ chunk-extension ] CRLF
		 *                   chunk-data CRLF
		 * chunk-size      = 1*HEX
		 * last-chunk      = 1*("0") [ chunk-extension ] CRLF
		 * chunk-extension = *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
		 * chunk-ext-name  = token
		 * chunk-ext-val   = token | quoted-string
		 * chunk-data      = chunk-size(OCTET)
		 * trailer		 = *(entity-header CRLF)
		 */
		if( pHttpChunk->i_left_size <= 0 )
		{
			/*
			 * <chunk-size>
			 *  Read the chunk header.
			 *  Chunk-size field has hex digits string indicating the size of the chunk.
			 */
			psz = net_Gets( pHttpSys->fd, pHttpSys->p_vs );
			if( psz == NULL )
			{
				/* fatal error - end of file */
				PrintDebug ("%s(%d) : failed reading chunk-header line\n",__FUNCTION__,__LINE__ );
				pAccess->info.b_eof = TRUE;
				PrintExit;
				return 0;
			}
			DumpHttpResponse(psz);

			if (0)
			{
				/* Checking hex string has no-meaning because chunk-size or chunk-data has also hexa value.
				   Use maximum chunk-size check and continued CR/LF check */
				if (INT_HTTP_CheckHex(psz[0]) == FALSE)
				{
					PrintDebug("%s(%d) : failed reading chunk-header line\n",__FUNCTION__,__LINE__ );
					;
				}
			}

			/* Read chunk length (hex value) */
			pHttpChunk->i_hexa_size = pHttpChunk->i_left_size = strtoll( psz, NULL, 16 ); /* or strtoul() */
			if(psz != NULL)
			{
				DD_MEM_Free ( psz );
				psz = NULL;
			}

			if (pHttpChunk->e_state != HTTP_CHUNK_STATE_START)
			{
				pHttpChunk->e_state = HTTP_CHUNK_STATE_START;
			}
			pHttpChunk->i_chunk_hcount++;
#if defined (STREAM_HTTP_CHUNKED_DEBUG)
			pHttpChunk->i_total_hbytes += pHttpChunk->i_hexa_size;
			PrintDebug("%s(%d) : chunked-TE[h-cnt:%llu, h-recv:%llu]\n",
						__FUNCTION__, __LINE__,
						pHttpChunk->i_chunk_hcount, pHttpChunk->i_total_hbytes);
#endif

			/*
			 * <last-chunk>
			 *  Chunked-Body = *chunk
			 *                last-chunk
			 *                trailer
			 *                CRLF
			 */
			if (pHttpChunk->i_hexa_size <= 0)
			{
				if (pHttpChunk->i_hexa_size < 0)
				{
					PrintError("%s(%d) : Error> Got invalid (chunk:%lld) \n",
								__FUNCTION__,__LINE__,
								pHttpChunk->i_hexa_size);
					pHttpChunk->i_hexa_size = pHttpChunk->i_left_size = 0;
				}
				pHttpChunk->b_last_chunk = TRUE;
				pAccess->info.b_eof = TRUE;
				PrintDebug ("%s(%d) : stream_read returns for EOF\n",__FUNCTION__,__LINE__);
				PrintExit;
				return 0;
			}

			/*
			 * <trailer>
			 *  Nothing todo for trailer; Getting trailer header in A/V streaming has no meaning,
			 *  because that is end_of_stream and playback will be finished.
			 *
			 * trailer      = *(entity-header CRLF)
			 */
		}

		if( i_len > pHttpChunk->i_left_size )
		{
			i_len = pHttpChunk->i_left_size;
		}
	}
	else if( pAccess->info.i_size != -1 && (HINT64)i_len > pHttpSys->i_remaining)
	{
		/* Only ask for the remaining length */
		i_len = (size_t)pHttpSys->i_remaining;
		if(i_len == 0)
		{
			pAccess->info.b_eof = TRUE;
			PrintDebug ("%s(%d) : stream_read returns for EOF\n",__FUNCTION__,__LINE__);
			PrintExit;
			return 0;
		}
	}

	i_read = net_Read (pHttpSys->fd, pHttpSys->p_vs, p_buffer, i_len, FALSE );
	if( i_read > 0 )
	{
#if 0 /* for CANVAS : dragon */
		PrintDebug ("[%s:%d] i_read:%d pAccess->info.i_pos(%"PRId64"): \n",__FUNCTION__,__LINE__,i_read,pAccess->info.i_pos);
#endif
		DumpHttpStream(p_buffer, i_read, (HUINT32)pAccess->info.i_pos);
		pAccess->info.i_pos += i_read;

		if ( pHttpSys->b_chunked )
		{
			/* Dont wait until to reading full chunk-data */
			/* Remained chunk-data will be read at next stream_read() */
			pHttpChunk->i_left_size -= i_read;
			if (pHttpChunk->e_state != HTTP_CHUNK_STATE_RECV)
			{
				pHttpChunk->e_state = HTTP_CHUNK_STATE_RECV;
			}

#if defined (STREAM_HTTP_CHUNKED_DEBUG)
			pHttpChunk->i_chunk_rcount++;
			if (i_read > 0)
			{
				pHttpChunk->i_total_rbytes += i_read;
			}
			PrintDebug("%s(%d) : chunked-TE[r-cnt:%llu, r-recv:%llu]\n",
						__FUNCTION__, __LINE__,
						pHttpChunk->i_chunk_rcount, pHttpChunk->i_total_rbytes);
#endif

			if (pHttpChunk->i_left_size <= 0)
			{
				/*
				 * Read last \r\n or \n chars by consuming last empty line,
				 * because each chunk is composed by...
				 * chunk = chunk-size [ chunk-extension ] CRLF
				 *  chunk-data CRLF
				 */
				char *psz = net_Gets( pHttpSys->fd, pHttpSys->p_vs );
				if(psz != NULL)
				{
					DD_MEM_Free ( psz );
					psz = NULL;
				}
			}
		}
	}
	else if( i_read == 0 )
	{
		/*
		 * I very much doubt that this will work.
		 * If i_read == 0, the connection *IS* dead, so the only
		 * sensible thing to do is INT_HTTP_Disconnect () and then retry.
		 * Otherwise, I got recv() completely wrong. -- Courmisch
		 */
		if( pHttpSys->b_continuous )
		{
			INT_HTTP_Request ( pAccess, 0 );
			pHttpSys->b_continuous = FALSE;
			i_read = STREAM_HTTP_Read( pAccess, p_buffer, i_len );
			pHttpSys->b_continuous = TRUE;
		}

		/* for YouView Back-off */
		(void)INT_HTTP_Disconnect ( pAccess );
		if( pHttpSys->b_reconnect )
		{
			if( INT_HTTP_Connect( pAccess, pAccess->info.i_pos ) )
			{
				PrintError ("%s (%d) Error> reconnection failed\n",__FUNCTION__,__LINE__ );
				PrintExit;
				return 0;
			}
			else
			{
				pHttpSys->b_reconnect = FALSE;
				i_read = STREAM_HTTP_Read( pAccess, p_buffer, i_len );
				pHttpSys->b_reconnect = TRUE;
			}
		}
		if( i_read == 0 )
		{
			pAccess->info.b_eof = TRUE;
		}
	}

    if( pAccess->info.i_size != -1 ) /* && (pHttpSys->b_has_size) */
	{
		pHttpSys->i_remaining -= i_read;
	}

#if 0 /* for CANVAS : dragon */
	PrintExit;
#endif
	return i_read;
}


/*****************************************************************************
 * STREAM_HTTP_Seek: close and re-open a connection at the right place
 *****************************************************************************/
int STREAM_HTTP_Seek ( STREAM_ACCESS_t *pAccess, HINT64 i_pos )
{
	HTTP_ACCESS_SYS_t *pHttpSys=NULL;

	PrintDebug ("%s(%d) trying to seek to %"PRId64"\n",__FUNCTION__,__LINE__, i_pos );

	if(NULL == pAccess)
	{
		PrintError ("%s(%d) : Error> pAccess is NULL ! \n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}

	pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
	if (pHttpSys==NULL)
	{
		PrintError("%s(%d) Error> invalid pHttpSys Handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return -1;
	}
	if ((pHttpSys->b_chunked == TRUE) && (pHttpSys->b_range_accepted == FALSE) && (pAccess->t_dlna.b_dlna == FALSE))
	{
		return DI_IPT_SUCCESS;
	}

	(void)INT_HTTP_Disconnect ( pAccess );

	if( pAccess->info.i_size && (HUINT64)i_pos >= (HUINT64)pAccess->info.i_size )
	{
		PrintDebug ( "%s (%d) seek to far\n",__FUNCTION__,__LINE__ );
		int retval = STREAM_HTTP_Seek ( pAccess, pAccess->info.i_size - 1 );
		if( retval == DI_IPT_SUCCESS )
		{
			HUINT8 p_buffer[2];
			if( STREAM_HTTP_Read( pAccess, p_buffer, 1) < 0 )
			{
				retval = DI_IPT_EGENERIC;
			}
			pAccess->info.b_eof  = FALSE;
		}
		return retval;
	}
	/* else { continue; } */

#if 0 /* __RECONNECT_WITH_ORIGINAL_URL_YOUVIEW__ */
	if(INT_HTTP_PrepareConnect(pAccess, pAccess->psz_path))
	{
		PrintError ( "%s(%d) INT_HTTP_SetConnectInfo failed\n",__FUNCTION__,__LINE__ );
		pAccess->info.b_eof = TRUE;
		return DI_IPT_EGENERIC;
	}
#endif

	if( INT_HTTP_Connect( pAccess, i_pos ) ) /* Back-Off */
	{
		PrintError ( "%s(%d) seek failed!!\n",__FUNCTION__,__LINE__ );
		pAccess->info.b_eof = TRUE;
		return DI_IPT_EGENERIC;
	}
	/* else { continue; } */

	return DI_IPT_SUCCESS;
}

/*****************************************************************************
 * STREAM_HTTP_Control:
 * XXX:
 *  This function is not used yet.
 * 	Because HTTP Controller does not support to seek by specific time.
 * 	It only works by byte offsets.
 *****************************************************************************/
int STREAM_HTTP_Control( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2)
{
	int ret=DI_IPT_SUCCESS;
	HTTP_ACCESS_SYS_t *pHttpSys = NULL;

	PrintEnter;

	if(NULL == pAccess)
	{
		PrintError ("%s(%d) : Error> pAccess is NULL ! \n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}

	if(NULL == pAccess->p_sys)
	{
		PrintError ("%s(%d) : Error> pAccess->p_sys is NULL ! \n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}

	pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;

	switch (cmd)
	{
		case CMD_CONTROL_PAUSE :
			PrintError ("%s (%d) CMD_CONTROL_PAUSE is unused cmd yet.\n",__FUNCTION__,__LINE__);
			break;
		case CMD_CONTROL_RESUME :
			PrintError ("%s (%d) CMD_CONTROL_RESUME is unused cmd yet.",__FUNCTION__,__LINE__);
			break;
		case CMD_CONTROL_SPEED :
			PrintError ("%s (%d) CMD_CONTROL_SPEEE is unused cmd yet. (%d)\n",__FUNCTION__,__LINE__,(HINT32)val);
			break;
		case CMD_CONTROL_SEEK :
			PrintError ("%s (%d) CMD_CONTROL_SEEK (val(position)=%"PRId64")\n",__FUNCTION__,__LINE__,val2);
			PrintExit;
			return STREAM_HTTP_Seek( pAccess, 3700000 );
		default :
			PrintError ("%s (%d) Error> Unknown Command.(%d). \n",__FUNCTION__,__LINE__,cmd);
			PrintExit;
			return  DI_IPT_EGENERIC;
			break;
	}
	PrintExit;
	return ret;
}

#if 0 /* __RECONNECT_WITH_ORIGINAL_URL_YOUVIEW__ */ /* defined but not used */
static int INT_HTTP_PrepareConnect(STREAM_ACCESS_t *pAccess, const char *pUrl)
{
	HTTP_ACCESS_SYS_t *pHttpSys=NULL;
	char		 *psz=NULL, *p=NULL;
	PrintEnter;
	p = psz = strdup( pUrl );
	if ( p == NULL || psz == NULL )
	{
		goto error;
	}
	while( (p = VK_strchr( p, ' ' )) != NULL )
	{
		*p = '+';
	}
	pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
	if(pHttpSys == NULL)
	{
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	/**********************************************
	 * YouView TimeLine : X-IndexFileLocation
	 **********************************************/
	if(pHttpSys->psz_index != NULL)
	{
		if(pAccess->psz_index_file != NULL)
		{
			PrintError ("%s(%d) Error> We have already index file path for X-IndexFileLocation Header.\n",__FUNCTION__,__LINE__);
			DD_MEM_Free ( pAccess->psz_index_file);
		}

		pAccess->psz_index_file = UTIL_UrlAllocWithNewName (pHttpSys->psz_location, pHttpSys->psz_index);
		/* Cleanup after first catch */
		VK_free ( pHttpSys->psz_index);
		pHttpSys->psz_index = NULL;
	}

	/**********************************************
	 * YouView TimeLine : X-BytePerSecond
	 **********************************************/
	if(pHttpSys->psz_bps != NULL)
	{
		if(pAccess->psz_bps_value != NULL)
		{
			PrintError ("%s(%d) Error> We have already bps value for X-BytePerSecond Header.\n",__FUNCTION__,__LINE__);
			DD_MEM_Free( pAccess->psz_bps_value);
		}
		pAccess->psz_bps_value = pHttpSys->psz_bps;
		/* Cleanup after first catch */
		VK_free ( pHttpSys->psz_bps);
	}

	UTIL_UrlClean (&pHttpSys->url);
	UTIL_UrlParse ( &pHttpSys->url, psz, 0 );

	if(psz != NULL)
	{
		VK_free ( psz );
		psz = NULL;
	}

	if( pHttpSys->url.psz_host == NULL || *pHttpSys->url.psz_host == '\0' )
	{
		PrintError ( "%s (%d) Error> invalid host\n",__FUNCTION__,__LINE__ );
		goto error;
	}

	/** if URI Prefix is https, change port number to 443 */
	if(!VK_strncmp(pHttpSys->url.psz_protocol,"https",5))
	{
		/* HTTP over SSL/TLS */
		pHttpSys->b_ssl = TRUE;
		if( pHttpSys->url.i_port <= 0 )
		{
			pHttpSys->url.i_port = 443;
		}
	}
	/** http Public port */
	else
	{
		if( pHttpSys->url.i_port <= 0 )
		{
			pHttpSys->url.i_port = 80;
		}
	}

	/* TODO :
	 * corresponding with service provider such as IRT/ZDF.
	 * They may use the particular string for distingushing
	 * valid client whether or not.
	 *
	 * User-Agent String (2009.10.19)
	 * 	HbbTV/1.1.1 (<capabilities>; [<vendorName>]; [<modelName>]; [<softwareVersion>];  [<hardwareVersion>]; <reserved>)
	 */
	if(pAccess->psz_user_agent != NULL)
	{
		pHttpSys->psz_user_agent = strdup (pAccess->psz_user_agent);
	}

	/* Check proxy */

	/* TODO : Proxy Server URL(String Format) is not supported */
	psz = NULL;
	if( psz )
	{
		pHttpSys->b_proxy = TRUE;
		UTIL_UrlParse( &pHttpSys->proxy, psz, 0 );
		DD_MEM_Free ( psz );
		psz = NULL;
	}
#ifdef HAVE_GETENV
	else
	{
		/* in the case of ENV proxy value */
		psz = getenv( "http_proxy" );
		if( psz )
		{
			pHttpSys->b_proxy = TRUE;
			UTIL_UrlParse ( &pHttpSys->proxy, psz, 0 );
		}
	}
#endif

	/* in case of Using Proxy Server */
	if( psz ) /* No, this is NOT a use-after-free error */
	{
		/* Setting Proxy Server password */
		psz = NULL;// TODO junsy: password string //var_CreateGetNonEmptyString( pAccess, "http-proxy-pwd" );
		if( psz )
		{
			pHttpSys->proxy.psz_password = pHttpSys->psz_proxy_passbuf = psz;
		}
	}

	/* in case of Using Proxy Server */
	if( pHttpSys->b_proxy )
	{
		if( pHttpSys->proxy.psz_host == NULL || *pHttpSys->proxy.psz_host == '\0' )
		{
			PrintError ("%s (%d) Error> invalid proxy host\n",__FUNCTION__,__LINE__ );
			goto error;
		}
		if( pHttpSys->proxy.i_port <= 0 )
		{
			pHttpSys->proxy.i_port = 80;
		}
	}

	PrintDebug ("%s(%d) : http: server='%s' port=%d file='%s\n",__FUNCTION__,__LINE__, pHttpSys->url.psz_host, pHttpSys->url.i_port, pHttpSys->url.psz_path );

#ifdef STREAM_HTTP_DEBUG /* For Debug by junsy */
	if( pHttpSys->b_proxy )
	{
		PrintDebug ("%s(%d) : proxy %s:%d\n",__FUNCTION__,__LINE__, pHttpSys->proxy.psz_host, pHttpSys->proxy.i_port );
	}

	/* Using Login Password,ID */
	if( pHttpSys->url.psz_username && *pHttpSys->url.psz_username )
	{
		PrintDebug ("%s(%d) : user='%s'\n",__FUNCTION__,__LINE__, pHttpSys->url.psz_username );
	}
#endif

	pHttpSys->b_reconnect = FALSE; // TODO : reconnectable ?? //var_CreateGetBool( pAccess, "http-reconnect" );
	pHttpSys->b_continuous = FALSE; //TODO : continuousable ?? var_CreateGetBool( pAccess, "http-continuous" );

	PrintExit;
	return DI_IPT_SUCCESS;

error:
	UTIL_UrlClean( &pHttpSys->url );
	UTIL_UrlClean( &pHttpSys->proxy );
	if(pHttpSys->psz_user_agent != NULL)
	{
		VK_free ( pHttpSys->psz_user_agent );
		pHttpSys->psz_user_agent = NULL;
	}

	if(pHttpSys->psz_proxy_passbuf != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_proxy_passbuf );
		pHttpSys->psz_proxy_passbuf = NULL;
	}

	if(pHttpSys->psz_protocol != NULL)
	{
		VK_free ( pHttpSys->psz_protocol );
		pHttpSys->psz_protocol = NULL;
	}

	if(pHttpSys->psz_mime != NULL)
	{
		VK_free ( pHttpSys->psz_mime );
		pHttpSys->psz_mime = NULL;
	}

	if(pHttpSys->psz_pragma != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_pragma );
		pHttpSys->psz_pragma = NULL;
	}

	if(pHttpSys->psz_location != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_location );
		pHttpSys->psz_location = NULL;
	}

	if(pHttpSys->psz_icy_name != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_name );
		pHttpSys->psz_icy_name = NULL;
	}

	if(pHttpSys->psz_icy_genre != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_genre );
		pHttpSys->psz_icy_genre = NULL;
	}

	if(pHttpSys->psz_icy_title != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_title );
		pHttpSys->psz_icy_title = NULL;
	}
	if(pHttpSys->i_icy_url != NULL)
	{
		DD_MEM_Free ( pHttpSys->i_icy_url );
		pHttpSys->i_icy_url = NULL;
	}

#if SUPPORT_COOKIE
	/* DHKIM: pHttpSys->cookies 주소에 대한 Memory Free는 상위에서 해제함. */
	pHttpSys->cookies = NULL;
#else
	if(pHttpSys->cookies != NULL)
	{
		DD_MEM_Free ( pHttpSys->cookies );
		pHttpSys->cookies = NULL;
	}
#endif
	PrintExit;
	return DI_IPT_EGENERIC;
}
#endif /* __RECONNECT_WITH_ORIGINAL_URL_YOUVIEW__ */

/*****************************************************************************
 * HTTP_Open:
 *****************************************************************************/
static int INT_HTTP_OpenWithCookies ( STREAM_ACCESS_t *pAccess, char *cookies )
{
	HTTP_ACCESS_SYS_t *pHttpSys=NULL;
	char         *psz=NULL, *p=NULL;
	/* Only forward an store cookies if the corresponding option is activated */
#if SUPPORT_HUMAX_COOKIE
	HBOOL   b_forward_cookies = TRUE; // TODO : 쿠키를 사용하는 경우 고려될 필요 있으나 현재는 FALSE 처리함
#else
	HBOOL   b_forward_cookies = FALSE; // TODO : 쿠키를 사용하는 경우 고려될 필요 있으나 현재는 FALSE 처리함
#endif
	// var_CreateGetBool( pAccess, "http-forward-cookies" );
	char * saved_cookies = (b_forward_cookies ? (cookies) : NULL);
#if 0 /* YouView backoff */
	int 	error_retrycount =0;
	int 	nRet =0;
#endif
	int	nRet=DI_IPT_SUCCESS;

	PrintEnter;

	if(NULL == pAccess)
	{
		PrintError ( "%s (%d) Error> invalid pAccess Handle\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	/* Set up pAccess */
	pAccess->info.i_size = 0;
	pAccess->info.i_pos = 0;
	pAccess->info.b_eof = FALSE;
	pAccess->info.i_title = 0;
	pAccess->info.i_seekpoint = 0;

	pAccess->p_sys = (void*)DD_MEM_Alloc (sizeof(HTTP_ACCESS_SYS_t));
	if(pAccess->p_sys == NULL)
	{
		return DI_IPT_EGENERIC;
	}

	pHttpSys = (HTTP_ACCESS_SYS_t*)pAccess->p_sys;
	VK_memset ( pHttpSys, 0, sizeof( HTTP_ACCESS_SYS_t));

	pHttpSys->fd = -1;
	pHttpSys->b_proxy = FALSE;
	pHttpSys->psz_proxy_passbuf = NULL;
	pHttpSys->i_version = 1;
	pHttpSys->b_seekable = TRUE;
	pHttpSys->psz_mime = NULL;
	pHttpSys->psz_pragma = NULL;
	pHttpSys->b_mms = FALSE;
	pHttpSys->b_icecast = FALSE;
	pHttpSys->psz_location = NULL;
	pHttpSys->psz_user_agent = NULL;
	pHttpSys->b_pace_control = TRUE;
	pHttpSys->b_cache_control = FALSE;

	/* chunked-TE */
	pHttpSys->b_chunked = FALSE; /* Also, does at INT_HTTP_Connect() */
	pHttpSys->t_chunk.b_last_chunk = FALSE;    /* got last-chunk;received zero hexa value */
	pHttpSys->t_chunk.b_trailer = FALSE;       /* default false; existed chunk trailer; no meaning at streaming, not cgi */
	pHttpSys->t_chunk.i_hexa_size = 0;
	pHttpSys->t_chunk.i_left_size = 0;
	pHttpSys->t_chunk.e_state = HTTP_CHUNK_STATE_NA;
	pHttpSys->t_chunk.encoding_type = HTTP_ENCODING_IDENTITY; /* no encoding; deflate or gzip is unsupported */
	pHttpSys->t_chunk.i_chunk_hcount = 0;
#if defined (STREAM_HTTP_CHUNKED_DEBUG)
	pHttpSys->t_chunk.i_chunk_rcount = 0;
	pHttpSys->t_chunk.i_total_hbytes = 0;
	pHttpSys->t_chunk.i_total_rbytes = 0;
#endif

	/** for virtual socket interface */
	pHttpSys->p_vs = NULL;
	/** SSL/TLS Support */
	pHttpSys->b_ssl = FALSE;

	/** for ICY Meta Data */
	pHttpSys->i_icy_meta = 0;
	pHttpSys->i_icy_offset = 0;
	pHttpSys->psz_icy_name = NULL;
	pHttpSys->psz_icy_genre = NULL;
	pHttpSys->psz_icy_title = NULL;


	pHttpSys->i_remaining = 0;
	pHttpSys->b_persist = FALSE;

	/** initial info structure */
	pAccess->info.i_size = -1;
	pAccess->info.i_pos  = 0;
	pAccess->info.b_eof  = FALSE;

	pHttpSys->cookies = saved_cookies;
	pHttpSys->i_redirect_count = 0;

	pHttpSys->i_connect_retry_count = 0;

	/* for Back-Off algo */
	pHttpSys->i_backoff_retry_count = 0;
	pHttpSys->i_backoff_retry_delay = 0;
	pAccess->b_immediately_exit = FALSE;
	pHttpSys->i_backoff_retry_totaldelay = 0;
	pHttpSys->i_backoff_retry_endtime = 60 * 60 * 1000; /* Default 1 Hour */
	/** Parse URI - remove spaces */
	if(pAccess->psz_path == NULL)
	{
		goto error;
	}
#if 0 /* __USING_PREPARECONNECT__ */
	if(INT_HTTP_PrepareConnect(pAccess, pAccess->psz_path))
	{
		goto error;
	}
#else
	p = psz = strdup( pAccess->psz_path );
	if ( p == NULL || psz == NULL )
	{
		goto error;
	}

	while( (p = VK_strchr( p, ' ' )) != NULL )
	{
		*p = '+';
	}

	UTIL_UrlParse ( &pHttpSys->url, psz, 0 );
	if(psz != NULL)
	{
		VK_free ( psz );
		psz = NULL;
	}

	if( pHttpSys->url.psz_host == NULL || *pHttpSys->url.psz_host == '\0' )
	{
		PrintError ( "%s (%d) Error> invalid host\n",__FUNCTION__,__LINE__ );
		goto error;
	}

	/** if URI Prefix is https, change port number to 443 */
	/* HTTP over SSL/TLS */
	if(VK_strstr(pAccess->psz_path, "https://") != NULL)
	{
		pHttpSys->b_ssl = TRUE;
		if( pHttpSys->url.i_port <= 0 )
		{
			pHttpSys->url.i_port = 443;
		}
	}
	else
	{
		if( pHttpSys->url.i_port <= 0 )
		{
			pHttpSys->url.i_port = 80;
		}
	}

	pHttpSys->p_tls = DD_MEM_Alloc(sizeof(tls_session_t));
	if(pHttpSys->p_tls == NULL)
	{
		goto error;
	}

	VK_MEM_Memset(pHttpSys->p_tls, 0x0, sizeof(tls_session_t));
	pHttpSys->p_tls->p_ctx = NULL;
	pHttpSys->p_tls->p_ssl = NULL;

	if(pAccess->psz_ca_name != NULL)
	{
		pHttpSys->p_tls->psz_ca_name = strdup(pAccess->psz_ca_name);
	}
	else
	{
		pHttpSys->p_tls->psz_ca_name = NULL;
	}

	if(pAccess->psz_ca_path != NULL)
	{
		pHttpSys->p_tls->psz_ca_path = strdup(pAccess->psz_ca_path);
	}
	else
	{
		pHttpSys->p_tls->psz_ca_path = NULL;
	}

	if(pAccess->psz_certi_path != NULL)
	{
		pHttpSys->p_tls->psz_certi_path = strdup(pAccess->psz_certi_path);
	}
	else
	{
		pHttpSys->p_tls->psz_certi_path = NULL;
	}

	if(pAccess->psz_prikey_path != NULL)
	{
		pHttpSys->p_tls->psz_prikey_path = strdup(pAccess->psz_prikey_path);
	}
	else
	{
		pHttpSys->p_tls->psz_prikey_path = NULL;
	}

	if(pAccess->psz_prikey_passwd != NULL)
	{
		pHttpSys->p_tls->psz_prikey_passwd = strdup(pAccess->psz_prikey_passwd);
	}
	else
	{
		pHttpSys->p_tls->psz_prikey_passwd = NULL;
	}

	if(pAccess->cbTlsCerficate != NULL)
	{
		pHttpSys->p_tls->cbCerficate = (Tls_CertificateCallbackDesc*) DD_MEM_Alloc(sizeof(Tls_CertificateCallbackDesc));
		if(pHttpSys->p_tls->cbCerficate)
		{
			VK_memset (pHttpSys->p_tls->cbCerficate, 0, sizeof(Tls_CertificateCallbackDesc));
			pHttpSys->p_tls->cbCerficate->callback = pAccess->cbTlsCerficate->callback;
			pHttpSys->p_tls->cbCerficate->context = pAccess->cbTlsCerficate->context;
			pHttpSys->p_tls->cbCerficate->userdata = pAccess->cbTlsCerficate->userdata;
			PrintDebug ("%s(%d) Register Tls Certificate callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pHttpSys->p_tls->cbCerficate);
		}
		else
		{
			PrintError("%s(%d) Fail Register Tls Certificate Callback\n",__FUNCTION__,__LINE__);
		}
	}
	else
	{
		pHttpSys->p_tls->cbCerficate = NULL;
	}
#if 0 /* support always tls because have inserted default root certis */
	if (( pHttpSys->p_tls->psz_ca_name == NULL ) && ( pHttpSys->p_tls->psz_ca_path == NULL ) && (pAccess->cbTlsCerficate == NULL))
	{
		PrintError("%s(%d) Not Support TLS(Because ca path or name or tls callback is null.) \n",__FUNCTION__,__LINE__);
	}
	else
#endif
	if(pHttpSys->b_ssl == TRUE)
	{
		nRet = TLS_Client_Setup(0, pHttpSys->p_tls);
		if(nRet != DI_IPT_SUCCESS)
		{
			PrintError ( "%s (%d) Error> Fail TLS_Client_Setup\n",__FUNCTION__,__LINE__ );
			goto error;
		}
	}

	/* TODO :
	 * corresponding with service provider such as IRT/ZDF.
	 * They may use the particular string for distingushing
	 * valid client whether or not.
	 *
	 * User-Agent String (2009.10.19)
	 * 	HbbTV/1.1.1 (<capabilities>; [<vendorName>]; [<modelName>]; [<softwareVersion>];  [<hardwareVersion>]; <reserved>)
	 */
	if(pAccess->psz_user_agent != NULL)
	{
		pHttpSys->psz_user_agent = strdup (pAccess->psz_user_agent);
		/* User-agent for ZDF-HD chunked live streaming */
		/* pHttpSys->psz_user_agent = strdup ("Opera/9.80 (Linux 7335b0-smp; U; HbbTV/1.1.1 (+PVR;Humax;iCord MINI;1.00.32;1.0;)ce-html/1.0 hdplusSmartTV/1.0 (NETRANGEMMH; );"); */
	}

	/* Check proxy */

	/* TODO : Proxy Server URL(String Format) is not supported */
	psz = NULL;
	if( psz )
	{
		pHttpSys->b_proxy = TRUE;
		UTIL_UrlParse( &pHttpSys->proxy, psz, 0 );
		DD_MEM_Free ( psz );
		psz = NULL;
	}
#ifdef HAVE_GETENV
	else
	{
		/* in the case of ENV proxy value */
		psz = getenv( "http_proxy" );
		if( psz )
		{
			pHttpSys->b_proxy = TRUE;
			UTIL_UrlParse ( &pHttpSys->proxy, psz, 0 );
		}
	}
#endif

	/* in case of Using Proxy Server */
	if( psz ) /* No, this is NOT a use-after-free error */
	{
		/* Setting Proxy Server password */
		psz = NULL;// TODO junsy: password string //var_CreateGetNonEmptyString( pAccess, "http-proxy-pwd" );
		if( psz )
		{
			pHttpSys->proxy.psz_password = pHttpSys->psz_proxy_passbuf = psz;
		}
	}

	/* in case of Using Proxy Server */
	if( pHttpSys->b_proxy )
	{
		if( pHttpSys->proxy.psz_host == NULL || *pHttpSys->proxy.psz_host == '\0' )
		{
			PrintError ("%s (%d) Error> invalid proxy host\n",__FUNCTION__,__LINE__ );
			goto error;
		}
		if( pHttpSys->proxy.i_port <= 0 )
		{
			pHttpSys->proxy.i_port = 80;
		}
	}

	PrintDebug ("%s(%d) : http: server='%s' port=%d file='%s\n",__FUNCTION__,__LINE__, pHttpSys->url.psz_host, pHttpSys->url.i_port, pHttpSys->url.psz_path );

#ifdef STREAM_HTTP_DEBUG /* For Debug by junsy */
	if( pHttpSys->b_proxy )
	{
		PrintDebug ("%s(%d) : proxy %s:%d\n",__FUNCTION__,__LINE__, pHttpSys->proxy.psz_host, pHttpSys->proxy.i_port );
	}

	/* Using Login Password,ID */
	if( pHttpSys->url.psz_username && *pHttpSys->url.psz_username )
	{
		PrintDebug ("%s(%d) : user='%s'\n",__FUNCTION__,__LINE__, pHttpSys->url.psz_username );
	}
#endif

	pHttpSys->b_reconnect = FALSE; // TODO : reconnectable ?? //var_CreateGetBool( pAccess, "http-reconnect" );
	pHttpSys->b_continuous = FALSE; //TODO : continuousable ?? var_CreateGetBool( pAccess, "http-continuous" );
#endif

#if 0 /* For Back-Off */
connect:
#endif
	/** Connect */
	switch ( INT_HTTP_Connect ( pAccess, 0 ) )
	{
		case -1:
			goto error;

		case -2:
#if	1 /* for YouView Back-Off */
			goto error;
#else
			/** Retry with http 1.0 */
			PrintDebug ("%s (%d) switching to HTTP version 1.0\n",__FUNCTION__,__LINE__ );
			pHttpSys->i_version = 0;
			pHttpSys->b_seekable = FALSE;

			/** Try to connecting http1.0 */
			if( INT_HTTP_Connect( pAccess, 0 ) )
			{
				/* For Youtube Rediraction Issue */
				for(error_retrycount=0;error_retrycount<HTTP_CONNECT_REDIRECTON_ERROR_RETRYCOUNT;error_retrycount++)
				{
					PrintDebug("##############################################\n");
					PrintDebug("######### [%s] Critical Section: Retry Connect #######\n", __FUNCTION__);
					PrintDebug("##############################################\n");

					if(pAccess->info.i_httpstatecode == HTTP_STATUS_MOVED_PERMANENTLY 	/* 301 */
							|| pAccess->info.i_httpstatecode == HTTP_STATUS_FOUND 				/* 302 */
							|| pAccess->info.i_httpstatecode == HTTP_STATUS_SEE_OTHER 			/* 303 */
							|| pAccess->info.i_httpstatecode == HTTP_STATUS_TEMP_REDIRECT) 		/* 307 */
					{
						nRet = INT_HTTP_Connect( pAccess, 0 );
						if(nRet == DI_IPT_SUCCESS)
						{
							break;
						}
						else
						{
							VK_TASK_Sleep(200);
						}
					}
					else
					{
						goto error;

					}
				}

				if(error_retrycount == HTTP_CONNECT_REDIRECTON_ERROR_RETRYCOUNT)
				{
					goto error;
				}
			}
#endif

#ifndef NDEBUG
		case 0:
			break;

		default:
			PrintError ( "%s(%d) : Error> You should not be here!!\n",__FUNCTION__,__LINE__ );
			abort();
#endif
	}

#if 0 /* For Back-Off */
	if( pHttpSys->i_code == HTTP_STATUS_UNAUTHORIZED ) /* 401*/
	{
		/* for Authentication : junsy*/
#if 0
		char *psz_login = NULL, *psz_password = NULL;
#endif
#if 0
		int i_ret;
#endif
		PrintError ("%s(%d) : Error> HTTP ERROR 401 !!\n",__FUNCTION__,__LINE__);
		/* FIXME ? */
		if( pHttpSys->url.psz_username && pHttpSys->url.psz_password &&
				pHttpSys->auth.psz_nonce && pHttpSys->auth.i_nonce == 0 )
		{
			goto connect;
		}
		PrintDebug ("%s(%d) : Error> Please enter a valid login name and a password for realm %s.",__FUNCTION__,__LINE__, pHttpSys->auth.psz_realm );
		PrintDebug ("%s(%d) : Error> authentication failed for realm %s\n",__FUNCTION__,__LINE__, pHttpSys->auth.psz_realm );
		/* for Authentication : junsy*/
#if 0
		i_ret = intf_UserLoginPassword( pAccess, _("HTTP authentication"),
				psz_msg, &psz_login, &psz_password );
		if( i_ret == DIALOG_OK_YES )
		{
			PrintDebug ("retrying with user=%s, pwd=%s\n",
					psz_login, psz_password );
			if( psz_login ) pHttpSys->url.psz_username = strdup( psz_login );
			if( psz_password ) pHttpSys->url.psz_password = strdup( psz_password );
			DD_MEM_Free ( psz_login );
			DD_MEM_Free ( psz_password );
			goto connect;
		}
		else
		{
			DD_MEM_Free ( psz_login );
			DD_MEM_Free ( psz_password );
			goto error;
		}
#else
		/** login fail */
		goto error;
#endif
	}
#endif

#if 0 /* move to Connect */
	/**********************************
	 * HTTP 30x Code Processing
	 * 	302 : Redirection
	 **********************************/
	if( ( pHttpSys->i_code == HTTP_STATUS_MOVED_PERMANENTLY 	/* 301 */
				|| pHttpSys->i_code == HTTP_STATUS_FOUND 				/* 302 */
				|| pHttpSys->i_code == HTTP_STATUS_SEE_OTHER 			/* 303 */
				|| pHttpSys->i_code == HTTP_STATUS_TEMP_REDIRECT ) 		/* 307 */
			&& pHttpSys->psz_location && *pHttpSys->psz_location )
	{
		PrintDebug ("%s(%d) : HTTP ERROR %d !!\n",__FUNCTION__,__LINE__,pHttpSys->i_code);
		PrintDebug ("%s(%d) : redirection to %s\n",__FUNCTION__,__LINE__, pHttpSys->psz_location );

		/** Do not accept redirection outside of HTTP works */
		if( VK_strncmp( pHttpSys->psz_location, "http", 4 )
				|| ( ( pHttpSys->psz_location[4] != ':' ) /* HTTP */
					&& VK_strncmp( pHttpSys->psz_location + 4, "s:", 2 ) /* HTTP/SSL */ ) )
		{
			PrintError ( "%s (%d) Error> insecure redirection ignored\n",__FUNCTION__,__LINE__ );
			goto error;
		}

		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free ( pAccess->psz_path );
			pAccess->psz_path = NULL;
		}

		pAccess->psz_path = VK_StrDup( pHttpSys->psz_location );

		/**********************************
		 * YouView TimeLine
		 **********************************/
		if(pHttpSys->psz_index != NULL)
		{
			if(pAccess->psz_index_file != NULL)
			{
				PrintError ("%s(%d) Error> We have already index file path for X-IndexFileLocation Header.\n",__FUNCTION__,__LINE__);
				DD_MEM_Free ( pAccess->psz_index_file);
			}

			pAccess->psz_index_file = UTIL_UrlAllocWithNewName (pHttpSys->psz_location, pHttpSys->psz_index);
			/* Cleanup after first catch */
			VK_free ( pHttpSys->psz_index);
		}

		if(pHttpSys->psz_bps != NULL)
		{
			if(pAccess->psz_bps_value != NULL)
			{
				PrintError ("%s(%d) Error> We have already index file path for X-IndexFileLocation Header.\n",__FUNCTION__,__LINE__);
				DD_MEM_Free( pAccess->psz_bps_value);
			}
			pAccess->psz_bps_value = pHttpSys->psz_bps;
			/* Cleanup after first catch */
			VK_free ( pHttpSys->psz_bps);
		}

		/** Clean up current STREAM_HTTP_Open() run */
		UTIL_UrlClean( &pHttpSys->url );

		INT_HTTP_AuthReset ( &pHttpSys->auth );
		UTIL_UrlClean( &pHttpSys->proxy );

		if(pHttpSys->psz_proxy_passbuf != NULL)
		{
			DD_MEM_Free ( pHttpSys->psz_proxy_passbuf );
			pHttpSys->psz_proxy_passbuf = NULL;
		}

		INT_HTTP_AuthReset ( &pHttpSys->proxy_auth );
		if(pHttpSys->psz_mime != NULL)
		{
			VK_free ( pHttpSys->psz_mime );
			pHttpSys->psz_mime = NULL;
		}

		if(pHttpSys->psz_pragma != NULL)
		{
			DD_MEM_Free ( pHttpSys->psz_pragma );
			pHttpSys->psz_pragma = NULL;
		}

		if(pHttpSys->psz_location != NULL)
		{
			DD_MEM_Free ( pHttpSys->psz_location );
			pHttpSys->psz_location = NULL;
		}

		if(pHttpSys->psz_user_agent != NULL)
		{
			VK_free ( pHttpSys->psz_user_agent );
			pHttpSys->psz_user_agent = NULL;
		}

		(void) INT_HTTP_Disconnect ( pAccess );

		if(pHttpSys != NULL)
		{
			DD_MEM_Free ( pHttpSys );
			pHttpSys = NULL;
			pAccess->p_sys = NULL;
		}

		/* Do new STREAM_HTTP_Open() run with new data */
		return INT_HTTP_OpenWithCookies( pAccess, cookies );
	}
#endif
	if( pHttpSys->b_mms )
	{
		PrintDebug ("%s (%d) this is actually a live mms server, BAIL\n",__FUNCTION__,__LINE__ );
		goto error;
	}

	/** Internet Radio set up */
	if( pHttpSys->b_inet_radio )
	{
		if( pHttpSys->b_icecast )
		{
			PrintDebug ("%s(%d)  : ICY Protocol detected\n",__FUNCTION__,__LINE__);
		}
		else
		{
			PrintDebug ("%s(%d)  : Assumed as Internet Radio\n",__FUNCTION__,__LINE__);
		}
		pAccess->info.i_size = 0xFFFFFFFFFFFFFF;
		pHttpSys->i_remaining = 0xFFFFFFFFFFFFFF;
		PrintDebug ("%s(%d)  : i_size(%"PRId64")\n",__FUNCTION__,__LINE__, pAccess->info.i_size);
	}

	/** boundless HTTP chunked-TE with no content-length and unsupported RANGE */
	if ((pHttpSys->b_chunked == TRUE) && (pHttpSys->b_range_accepted == FALSE) && (pAccess->t_dlna.b_dlna == FALSE))
	{
		pAccess->info.i_size = 0xFFFFFFFFFFFFFF;
		pHttpSys->i_remaining = 0xFFFFFFFFFFFFFF;
		PrintDebug ("%s(%d)  : i_size(%"PRId64")\n",__FUNCTION__,__LINE__, pAccess->info.i_size);
	}

	/** looking for codec type */
	if( pHttpSys->psz_mime )
	{
		if(!VK_strcasecmp( pHttpSys->psz_mime, "audio/aac" ))
		{
			pHttpSys->e_inetradio_codec = DI_AUDFORMAT_AAC;
		}
		else if(!VK_strcasecmp( pHttpSys->psz_mime, "audio/aacp" ))
		{
			pHttpSys->e_inetradio_codec = DI_AUDFORMAT_AAC;
		}
		else if( !VK_strcasecmp( pHttpSys->psz_mime, "audio/mpeg" ) )
		{
			pHttpSys->e_inetradio_codec = DI_AUDFORMAT_MP3;
		}
	}

#if 0
	else if( !VK_strcasecmp( pAccess->psz_access, "unsv" ) &&
			pHttpSys->psz_mime &&
			!VK_strcasecmp( pHttpSys->psz_mime, "misc/ultravox" ) )
	{
		DD_MEM_Free ( pAccess->psz_demux );
		/* Grrrr! detect ultravox server and force NSV demuxer */
		pAccess->psz_demux = strdup( "nsv" );
	}
	else if( !VK_strcmp( pAccess->psz_access, "itpc" ) )
	{
		DD_MEM_Free ( pAccess->psz_demux );
		pAccess->psz_demux = strdup( "podcast" );
	}
	else if( pHttpSys->psz_mime &&
			!VK_strncasecmp( pHttpSys->psz_mime, "application/xspf+xml", 20 ) &&
			( memchr( " ;\t", pHttpSys->psz_mime[20], 4 ) != NULL ) )
	{
		DD_MEM_Free ( pAccess->psz_demux );
		pAccess->psz_demux = strdup( "xspf-open" );
	}
#endif

	if( pHttpSys->b_reconnect )
	{
		PrintDebug ("auto re-connect enabled\n" );
	}

	/* PTS delay */
	//    var_Create( pAccess, "http-caching", DI_IPT_VAR_INTEGER |DI_IPT_VAR_DOINHERIT );

	PrintExit;
	return DI_IPT_SUCCESS;

error:
	UTIL_UrlClean( &pHttpSys->url );
	UTIL_UrlClean( &pHttpSys->proxy );

	if(pHttpSys->psz_user_agent != NULL)
	{
		VK_free ( pHttpSys->psz_user_agent );
		pHttpSys->psz_user_agent = NULL;
	}

	if(pHttpSys->psz_proxy_passbuf != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_proxy_passbuf );
		pHttpSys->psz_proxy_passbuf = NULL;
	}

	if(pHttpSys->psz_protocol != NULL)
	{
		VK_free ( pHttpSys->psz_protocol );
		pHttpSys->psz_protocol = NULL;
	}

	if(pHttpSys->psz_mime != NULL)
	{
		VK_free ( pHttpSys->psz_mime );
		pHttpSys->psz_mime = NULL;
	}

	if(pHttpSys->psz_pragma != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_pragma );
		pHttpSys->psz_pragma = NULL;
	}

	if(pHttpSys->psz_location != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_location );
		pHttpSys->psz_location = NULL;
	}

	if(pHttpSys->psz_icy_name != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_name );
		pHttpSys->psz_icy_name = NULL;
	}

	if(pHttpSys->psz_icy_genre != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_genre );
		pHttpSys->psz_icy_genre = NULL;
	}

	if(pHttpSys->psz_icy_title != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_title );
		pHttpSys->psz_icy_title = NULL;
	}
	if(pHttpSys->i_icy_url != NULL)
	{
		DD_MEM_Free ( pHttpSys->i_icy_url );
		pHttpSys->i_icy_url = NULL;
	}

	if(pHttpSys->p_tls != NULL)
	{
		if(pHttpSys->p_tls->psz_ca_name != NULL)
		{
			DD_MEM_Free ( pHttpSys->p_tls->psz_ca_name );
			pHttpSys->p_tls->psz_ca_name = NULL;
		}

		if(pHttpSys->p_tls->psz_ca_path != NULL)
		{
			DD_MEM_Free ( pHttpSys->p_tls->psz_ca_path );
			pHttpSys->p_tls->psz_ca_path = NULL;
		}

		if(pHttpSys->p_tls->psz_certi_path != NULL)
		{
			DD_MEM_Free ( pHttpSys->p_tls->psz_certi_path );
			pHttpSys->p_tls->psz_certi_path = NULL;
		}

		if(pHttpSys->p_tls->psz_prikey_path != NULL)
		{
			DD_MEM_Free ( pHttpSys->p_tls->psz_prikey_path );
			pHttpSys->p_tls->psz_prikey_path = NULL;
		}

		if(pHttpSys->p_tls->psz_prikey_passwd != NULL)
		{
			DD_MEM_Free ( pHttpSys->p_tls->psz_prikey_passwd );
			pHttpSys->p_tls->psz_prikey_passwd = NULL;
		}

		if(pHttpSys->p_tls->cbCerficate != NULL)
		{
			pHttpSys->p_tls->cbCerficate->callback = NULL;
			DD_MEM_Free(pHttpSys->p_tls->cbCerficate);
			pHttpSys->p_tls->cbCerficate = NULL;
		}

		DD_MEM_Free ( pHttpSys->p_tls );
		pHttpSys->p_tls = NULL;
	}

#if SUPPORT_HUMAX_COOKIE
	/* DHKIM: pHttpSys->cookies 주소에 대한 Memory Free는 상위에서 해제함. */
	pHttpSys->cookies = NULL;
#else
	if(pHttpSys->cookies != NULL)
	{
		DD_MEM_Free ( pHttpSys->cookies );
		pHttpSys->cookies = NULL;
	}
#endif

	(void) INT_HTTP_Disconnect ( pAccess );

	if(pHttpSys != NULL)
	{
		DD_MEM_Free ( pHttpSys );
		pHttpSys = NULL;
		pAccess->p_sys = NULL;
	}

	PrintExit;
	return DI_IPT_EGENERIC;
}

/*****************************************************************************
 * INT_HTTP_Connect:
 *****************************************************************************/
static int INT_HTTP_Connect ( STREAM_ACCESS_t *pAccess, HINT64 i_tell )
{
	HTTP_ACCESS_SYS_t   *pHttpSys = NULL;
	URL_t      srv;
	char		 *psz=NULL, *p=NULL;
	int	nTlsRet = DI_IPT_SUCCESS;

	PrintEnter;

	if(pAccess == NULL)
	{
		PrintError ( "%s (%d) Error> invalid pAccess Handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
	VK_MEM_Memset(&srv, 0x0, sizeof(URL_t));
	if(pHttpSys->b_cache_control)
	{
		p = psz = strdup( pAccess->psz_path );
		if ( p == NULL || psz == NULL )
		{
			return DI_IPT_EGENERIC;
		}
		while( (p = VK_strchr( p, ' ' )) != NULL )
		{
			*p = '+';
		}
		pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
		if(pHttpSys == NULL)
		{
			PrintExit;
			return DI_IPT_EGENERIC;
		}

		UTIL_UrlClean (&pHttpSys->url);
		UTIL_UrlParse ( &pHttpSys->url, psz, 0 );

		if(psz != NULL)
		{
			VK_free ( psz );
			psz = NULL;
		}

		if( pHttpSys->url.psz_host == NULL || *pHttpSys->url.psz_host == '\0' )
		{
			PrintError ( "%s (%d) Error> invalid host\n",__FUNCTION__,__LINE__ );
			return -1;
		}

		/** if URI Prefix is https, change port number to 443 */
		if(!VK_strncmp(pHttpSys->url.psz_protocol,"https",5))
		{
			/* HTTP over SSL/TLS */
			pHttpSys->b_ssl = TRUE;
			if( pHttpSys->url.i_port <= 0 )
			{
				pHttpSys->url.i_port = 443;
			}
		}
		/** http Public port */
		else
		{
			if( pHttpSys->url.i_port <= 0 )
			{
				pHttpSys->url.i_port = 80;
			}
		}
	}
	if(pHttpSys->b_proxy == TRUE)
	{
		VK_MEM_Memcpy(&srv, &pHttpSys->proxy, sizeof(URL_t));
	}
	else
	{
		VK_MEM_Memcpy(&srv, &pHttpSys->url, sizeof(URL_t));
	}

	/* Clean previous auth-information if needs re-connection */
	INT_HTTP_AuthReset ( &pHttpSys->auth );

	if(pHttpSys->psz_proxy_passbuf != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_proxy_passbuf );
		pHttpSys->psz_proxy_passbuf = NULL;
	}

	INT_HTTP_AuthReset ( &pHttpSys->proxy_auth );

	/* Clean info */
	if(pHttpSys->psz_location != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_location );
		pHttpSys->psz_location = NULL;
	}

	if(pHttpSys->psz_mime != NULL)
	{
		VK_free ( pHttpSys->psz_mime );
		pHttpSys->psz_mime = NULL;
	}

	if(pHttpSys->psz_pragma != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_pragma );
		pHttpSys->psz_pragma = NULL;
	}

	if(pHttpSys->psz_icy_genre != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_genre );
		pHttpSys->psz_icy_genre = NULL;
	}

	if(pHttpSys->psz_icy_name != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_name );
		pHttpSys->psz_icy_name = NULL;
	}

	if(pHttpSys->psz_icy_title != NULL)
	{
		DD_MEM_Free ( pHttpSys->psz_icy_title );
		pHttpSys->psz_icy_title = NULL;
	}

	pHttpSys->psz_location = NULL;
	pHttpSys->psz_mime = NULL;
	pHttpSys->psz_pragma = NULL;
	pHttpSys->b_mms = FALSE;

	/* chunked transfer-encoding (HTTP1.1) */
	pHttpSys->b_chunked = FALSE;
	pHttpSys->t_chunk.b_last_chunk = FALSE;    /* got last-chunk;received zero hexa value */
	pHttpSys->t_chunk.b_trailer = FALSE;       /* default false; existed chunk trailer; no meaning at streaming, not cgi */
	pHttpSys->t_chunk.i_hexa_size = 0;
	pHttpSys->t_chunk.i_left_size = 0;
	pHttpSys->t_chunk.e_state = HTTP_CHUNK_STATE_NA;
	pHttpSys->t_chunk.encoding_type = HTTP_ENCODING_IDENTITY; /* no encoding; deflate or gzip is unsupported */
	pHttpSys->t_chunk.i_chunk_hcount = 0;
#if defined (STREAM_HTTP_CHUNKED_DEBUG)
	pHttpSys->t_chunk.i_chunk_rcount = 0;
	pHttpSys->t_chunk.i_total_hbytes = 0;
	pHttpSys->t_chunk.i_total_rbytes = 0;
#endif

	/* ICY */
	pHttpSys->i_icy_meta = 0;
	pHttpSys->i_icy_offset = i_tell;
	pHttpSys->psz_icy_name = NULL;
	pHttpSys->psz_icy_genre = NULL;
	pHttpSys->psz_icy_title = NULL;
	/* offset related */
	pHttpSys->b_has_size = FALSE;
	pHttpSys->b_range_accepted = TRUE; /* default true; */
	pHttpSys->b_range_response = FALSE;
	pHttpSys->i_remaining = 0;
	pHttpSys->b_persist = FALSE;

	pAccess->info.i_size = -1;
	PrintDebug ("[%s:%d] i_tell(%"PRId64") pAccess->info.i_pos(%"PRId64") \n",__FUNCTION__,__LINE__,i_tell,pAccess->info.i_pos);
	pAccess->info.i_pos  = i_tell;
	pAccess->info.b_eof  = FALSE;
	pAccess->b_alive = TRUE;
	pAccess->t_dtcpip.b_dtcp_pcpparsing = FALSE;

	pHttpSys->b_inet_radio = FALSE;
	pHttpSys->b_content_length = FALSE;
	pHttpSys->b_content_range = FALSE;
	pHttpSys->b_content_mime = FALSE;

	/* STREAM_HTTP_Open connection */
	pHttpSys->fd = net_ConnectTCP (srv.psz_host, srv.i_port );
	if( pHttpSys->fd == -1 )
	{
		PrintError ( "%s(%d) : Error> cannot connect to %s:%d\n",__FUNCTION__,__LINE__, srv.psz_host, srv.i_port );
		PrintExit;
		return -1;
	}

	/* Initialize TLS/SSL session */
	if( pHttpSys->b_ssl == TRUE )
	{
		/* CONNECT to establish TLS tunnel through HTTP proxy */
		if( pHttpSys->b_proxy )
		{
			char *psz=NULL;
			unsigned i_status = 0;

			if( pHttpSys->i_version == 0 )
			{
				/* CONNECT is not in HTTP/1.0 */
				(void) INT_HTTP_Disconnect ( pAccess );
				return -1;
			}

			net_Printf( pHttpSys->fd, NULL,
					"CONNECT %s:%d HTTP/1.%d\r\nHost: %s:%d\r\n\r\n",
					pHttpSys->url.psz_host, pHttpSys->url.i_port,
					pHttpSys->i_version,
					pHttpSys->url.psz_host, pHttpSys->url.i_port);

			psz = net_Gets( pHttpSys->fd, NULL );
			if( psz == NULL )
			{
				PrintError ( "cannot establish HTTP/TLS tunnel\n" );
				(void) INT_HTTP_Disconnect ( pAccess );
				return -1;
			}

			DumpHttpResponse(psz);

#if 0 /* Remove Prevent Defect */
			VK_sscanf( psz, "HTTP/%*u.%*u %3u", &i_status );
#else
			{
				char *pszTmp = psz;
				char acTmpStatus[4];
				int nSizePsz = VK_strlen(psz);
				if( VK_strstr(pszTmp, "HTTP/") )
				{
					int idx=5;
					while( pszTmp[idx] != '.' ) { if((idx+1) <= nSizePsz) ++idx; else break; }
					while( pszTmp[idx] != ' ' ) { if((idx+1) <= nSizePsz) ++idx; else break; }
					VK_MEM_Memcpy(acTmpStatus, &pszTmp[idx+1], 3); /* Copy Error Code */
					acTmpStatus[3] = '\0';
					i_status = (unsigned)VK_atoi(acTmpStatus);
				}
			}
#endif
			if( psz != NULL )
			{
				DD_MEM_Free ( psz );
				psz = NULL;
			}

			if( ( i_status / 100 ) != 2 )
			{
				PrintError ( "HTTP/TLS tunnel through proxy denied\n" );
				(void) INT_HTTP_Disconnect ( pAccess );
				return -1;
			}

			do
			{
				psz = net_Gets( pHttpSys->fd, NULL );
				if( psz == NULL )
				{
					PrintError ( "HTTP proxy connection failed\n" );
					(void)INT_HTTP_Disconnect ( pAccess );
					return -1;
				}

				DumpHttpResponse(psz);

				if( *psz == '\0' )
					i_status = 0;

				if( psz != NULL )
				{
					DD_MEM_Free ( psz );
					psz = NULL;
				}

				if( pAccess->b_alive != TRUE)
				{
					PrintError ( "%s (%d) Error> pAccess->b_alive is false!! \n",__FUNCTION__,__LINE__ );
					(void)INT_HTTP_Disconnect ( pAccess );
					return -1;
				}
			}
			while( i_status );
		}

		/* TLS/SSL handshake */
		nTlsRet = TLS_Client_Connect(pHttpSys->p_tls, pHttpSys->fd, srv.psz_host);
		if( nTlsRet != DI_IPT_SUCCESS )
		{
			PrintError ( "%s(%d) Error> cannot establish HTTP/TLS session\n",__FUNCTION__,__LINE__ );
			(void)INT_HTTP_Disconnect ( pAccess );
			return -1;
		}
		pHttpSys->p_vs = &pHttpSys->p_tls->sock;
	}

	if(INT_HTTP_Request ( pAccess, i_tell ))
	{
		pAccess->b_alive = FALSE;

		/* Error Event Callback */
		if (pAccess->callback)
		{
			if (pAccess->callback->callback)
			{
				pAccess->callback->callback (pAccess->callback->context, pHttpSys->i_code, NULL);
			}
		}
		return -2;
	}
	else
	{
		if(pAccess->info.i_httpstatecode == HTTP_STATUS_MOVED_PERMANENTLY 	/* 301 */
			|| pAccess->info.i_httpstatecode == HTTP_STATUS_FOUND 				/* 302 */
			|| pAccess->info.i_httpstatecode == HTTP_STATUS_SEE_OTHER 			/* 303 */
			|| pAccess->info.i_httpstatecode == HTTP_STATUS_TEMP_REDIRECT) 		/* 307 */
		{
			char *psz=NULL, *p=NULL;
			if(pHttpSys->psz_location != NULL && pHttpSys->i_redirect_count < HTTP_CONNECT_MAX_REDIRECTION)
			{
				/** Do not accept redirection outside of HTTP works */
				if( VK_strncmp( pHttpSys->psz_location, "http", 4 )
						|| ( ( pHttpSys->psz_location[4] != ':' ) /* HTTP */
							&& VK_strncmp( pHttpSys->psz_location + 4, "s:", 2 ) /* HTTP/SSL */ ) )
				{
					PrintError ( "%s (%d) Error> insecure redirection ignored\n",__FUNCTION__,__LINE__ );
					return -2;
				}

				(void)INT_HTTP_Disconnect(pAccess);

				PrintDebug ("%s(%d) : redirection to %s\n",__FUNCTION__,__LINE__, pHttpSys->psz_location );

				if((pHttpSys->b_ssl == FALSE) && (VK_strstr(pHttpSys->psz_location, "https://") != NULL))
				{
					nTlsRet = TLS_Client_Setup(0, pHttpSys->p_tls);
					if(nTlsRet != DI_IPT_SUCCESS)
					{
						PrintError ( "%s (%d) Error> Fail TLS_Client_Setup\n",__FUNCTION__,__LINE__ );
						return -1;
					}
					pHttpSys->b_ssl = TRUE;
					pHttpSys->p_vs = &pHttpSys->p_tls->sock;
				}
				UTIL_UrlClean( &pHttpSys->url );
				if(pAccess->psz_path != NULL)
				{
					DD_MEM_Free ( pAccess->psz_path );
					pAccess->psz_path = NULL;
				}

				pAccess->psz_path = VK_StrDup( pHttpSys->psz_location );
				p = psz = VK_StrDup( pHttpSys->psz_location );

				if ( p == NULL || psz == NULL )
				{
					pAccess->b_alive = FALSE;
					return -2;
				}

				while( (p = VK_strchr( p, ' ' )) != NULL )
				{
					*p = '+';
				}

				/**********************************************
				 * YouView TimeLine : X-IndexFileLocation
				 **********************************************/
				if(pHttpSys->psz_index != NULL)
				{
					if(pAccess->psz_index_file != NULL)
					{
						PrintError ("%s(%d) Error> We have already index file path for X-IndexFileLocation Header.\n",__FUNCTION__,__LINE__);
						DD_MEM_Free ( pAccess->psz_index_file);
						pAccess->psz_index_file = NULL;
					}

					pAccess->psz_index_file = UTIL_UrlAllocWithNewName (pHttpSys->psz_location, pHttpSys->psz_index);
					/* Cleanup after first catch */
					VK_free ( pHttpSys->psz_index);
					pHttpSys->psz_index = NULL;
				}

				/**********************************************
				 * YouView TimeLine : X-BytePerSecond
				 **********************************************/
				if(pHttpSys->psz_bps != NULL)
				{
					if(pAccess->psz_bps_value != NULL)
					{
						PrintError ("%s(%d) Error> We have already bps value for X-BytePerSecond Header.\n",__FUNCTION__,__LINE__);
						DD_MEM_Free( pAccess->psz_bps_value);
						pAccess->psz_bps_value = NULL;
					}

					pAccess->psz_bps_value = (char*)VK_StrDup(pHttpSys->psz_bps);
					/* Cleanup after first catch */
					VK_free ( pHttpSys->psz_bps);
					pHttpSys->psz_bps = NULL;
				}

				UTIL_UrlClean( &pHttpSys->url );
				UTIL_UrlParse ( &pHttpSys->url, psz, 0 );
				if(psz != NULL)
				{
					VK_free ( psz );
					psz = NULL;
				}

				if( pHttpSys->url.psz_host == NULL || *pHttpSys->url.psz_host == '\0' )
				{
					PrintError ( "%s (%d) Error> invalid host\n",__FUNCTION__,__LINE__ );
					UTIL_UrlClean( &pHttpSys->url );
					pAccess->b_alive = FALSE;
					return -2;
				}

				/** if URI Prefix is https, change port number to 443 */
				if(!VK_strncmp(pHttpSys->url.psz_protocol,"https",5))
				{
					/* HTTP over SSL/TLS */
					pHttpSys->b_ssl = TRUE;
					if( pHttpSys->url.i_port <= 0 )
					{
						pHttpSys->url.i_port = 443;
					}
				}
				/** http Public port */
				else
				{
					if( pHttpSys->url.i_port <= 0 )
					{
						pHttpSys->url.i_port = 80;
					}
				}
				if(pHttpSys->psz_location != NULL)
				{
					DD_MEM_Free ( pHttpSys->psz_location );
					pHttpSys->psz_location = NULL;
				}
				pHttpSys->i_redirect_count++;
				return INT_HTTP_Connect( pAccess, i_tell );
			}
			else
			{
				pAccess->b_alive = FALSE;
				return -2;
			}
		}
		else if(pHttpSys->i_code == HTTP_STATUS_SERVICE_UNAVAILABLE /* STATUS 503 */
		|| pHttpSys->i_code == HTTP_STATUS_INTERNAL_SERVER_ERROR /* STATUS 500 */
		|| pHttpSys->i_code == HTTP_STATUS_GATEWAY_TIMEOUT) /* STATUS 504 */
		{
			/* Error Event Callback */
			if ( pAccess->callback)
			{
				if ( pAccess->callback->callback )
				{
					pAccess->callback->callback (pAccess->callback->context, pHttpSys->i_code, NULL);
				}
			}

			if(pHttpSys->i_backoff_retry_count < HTTP_CONNECT_ERROR_RETRYCOUNT)
			{
				struct timeval tvstart,tvend;
				HUINT32 ulDelayTime = 0;
				gettimeofday(&tvstart,NULL);
				pAccess->b_retrying = TRUE;
				if(pHttpSys->i_backoff_retry_delay > 0 )
				{
					pHttpSys->i_backoff_retry_count++;
					PrintError ("%s(%d) : Back-Off ( Retry-After : %u ms )\n",__FUNCTION__,__LINE__, pHttpSys->i_backoff_retry_delay );
				}
				else
				{
					pHttpSys->i_backoff_retry_count++;
					// pHttpSys->i_backoff_retry_delay = (unsigned int)((pow(4,pHttpSys->i_backoff_retry_count)*100));
					/* pow() 대체 */
					pHttpSys->i_backoff_retry_delay = 1 << pHttpSys->i_backoff_retry_count;
					pHttpSys->i_backoff_retry_delay *= pHttpSys->i_backoff_retry_delay;
					pHttpSys->i_backoff_retry_delay *= 100;
					PrintError ("%s(%d) : Back-Off ( Wait for %u ms )\n",__FUNCTION__,__LINE__, pHttpSys->i_backoff_retry_delay);
				}
				do
				{
					gettimeofday(&tvend,NULL);
					ulDelayTime = ((tvend.tv_sec * 1000) + (tvend.tv_usec/1000)) - ((tvstart.tv_sec * 1000) + (tvstart.tv_usec/1000));
					// PrintError ("%s(%d) : Time for Waiting : %u ms -> %u ms \n",__FUNCTION__,__LINE__, ulDelayTime, pHttpSys->i_backoff_retry_delay );
					if(pAccess->b_immediately_exit == TRUE)
					{
						pAccess->b_retrying = FALSE;
						pAccess->b_alive = FALSE;
						return -2;
					}
					usleep(HTTP_CONNECT_ERROR_WAITTIME);
					pHttpSys->i_backoff_retry_totaldelay += HTTP_CONNECT_ERROR_WAITTIME;
					if(pHttpSys->i_backoff_retry_totaldelay >= pHttpSys->i_backoff_retry_endtime)
					{
						pAccess->b_retrying = FALSE;
						pAccess->b_alive = FALSE;
						return -2;
					}
				} while(ulDelayTime < pHttpSys->i_backoff_retry_delay);
				pHttpSys->i_backoff_retry_delay = 0;
				if( INT_HTTP_Connect( pAccess, i_tell ) )
				{
					pAccess->b_retrying = FALSE;
					pAccess->b_alive = FALSE;
					return -2;
				}
				else
				{
					pAccess->b_retrying = FALSE;
					pHttpSys->i_backoff_retry_count = 0;
					pHttpSys->i_backoff_retry_totaldelay = 0;
				}
			}
			else
			{
				pAccess->b_retrying = FALSE;
				pAccess->b_alive = FALSE;
				return -2;
			}
		}
		else if((pAccess->info.i_httpstatecode == HTTP_STATUS_OK
		|| pAccess->info.i_httpstatecode == HTTP_STATUS_PARTIAL_CONTENT)
		&& ( pAccess->info.i_size == -1) && (pHttpSys->b_inet_radio != TRUE) )
		{
			if (pHttpSys->b_chunked == TRUE)
			{
				PrintError("%s(%d) There is no Content Length Header -> This is chunked TE\n", __FUNCTION__, __LINE__); // <-- JLEE!!
				pAccess->b_retrying = FALSE;
				pAccess->b_alive = FALSE;
				return 0;
			}
			else if(pHttpSys->i_connect_retry_count < HTTP_CONNECT_ERROR_RETRYCOUNT )
			{
				pAccess->b_retrying = TRUE;
				if(pAccess->b_immediately_exit == TRUE)
				{
					pAccess->b_retrying = FALSE;
					pAccess->b_alive = FALSE;
					return -2;
				}
				PrintError("%s(%d) Error> There is no Content Length Header - Retrying\n", __FUNCTION__, __LINE__); // <-- JLEE!!
				pHttpSys->i_connect_retry_count++;
				(void)INT_HTTP_Disconnect ( pAccess );
				return INT_HTTP_Connect( pAccess, i_tell );
			}
			else
			{
				pAccess->b_retrying = FALSE;
				pAccess->b_alive = FALSE;
				return -2;
			}
		}
		/* normal response code */
		else
		{
			/**********************************************
			 * YouView TimeLine : X-IndexFileLocation
			 **********************************************/
			if(pHttpSys->psz_index != NULL)
			{
				if(pAccess->psz_index_file != NULL)
				{
					PrintError ("%s(%d) Error> We have already index file path for X-IndexFileLocation Header.\n",__FUNCTION__,__LINE__);
					DD_MEM_Free ( pAccess->psz_index_file);
					pAccess->psz_index_file = NULL;
				}

				if ( NULL != VK_strstr(pHttpSys->psz_index,"http://"))
				{
					/* X-IndexFileLocation has the index asset as fully described uri form. ex> http://xxx.xxx.xxx/xxx.idx */
					pAccess->psz_index_file = (char*)VK_StrDup(pHttpSys->psz_index);
					if(!pAccess->psz_index_file)
						PrintError ("%s(%d) Error> VK_StrDup fails for indexfile \n",__FUNCTION__,__LINE__);
				}
				else
				{
					pAccess->psz_index_file = UTIL_UrlAllocWithNewName (pHttpSys->psz_location, pHttpSys->psz_index);
				}
				/* Cleanup after first catch */
				VK_free ( pHttpSys->psz_index);
				pHttpSys->psz_index = NULL;
			}

			/**********************************************
			 * YouView TimeLine : X-BytePerSecond
			 **********************************************/
			if(pHttpSys->psz_bps != NULL)
			{
				if(pAccess->psz_bps_value != NULL)
				{
					PrintError ("%s(%d) Error> We have already bps value for X-BytePerSecond Header.\n",__FUNCTION__,__LINE__);
					DD_MEM_Free( pAccess->psz_bps_value);
					pAccess->psz_bps_value = NULL;
				}

				pAccess->psz_bps_value = (char*)VK_StrDup(pHttpSys->psz_bps);
				/* Cleanup after first catch */
				VK_free ( pHttpSys->psz_bps);
				pHttpSys->psz_bps = NULL;
			}
		}
		pHttpSys->i_connect_retry_count = 0;
		pHttpSys->i_backoff_retry_count = 0;
		pHttpSys->i_redirect_count= 0;
		return 0;
	}
}

static void INT_HTTP_PrepareChunking(HTTP_ACCESS_SYS_t *pHttpSys)
{
	HTTP_CHUNK_t *pHttpChunk = NULL;

	if (NULL == pHttpSys)
	{
		PrintError ("%s(%d) : Error> pHttpSys is NULL ! \n",__FUNCTION__,__LINE__);
		return;
	}
	pHttpChunk = &pHttpSys->t_chunk;

	/* chunked transfer-encoding (HTTP1.1) */
	pHttpChunk->b_last_chunk = FALSE;    /* got last-chunk;received zero hexa value */
	pHttpChunk->b_trailer = FALSE;       /* default false; existed chunk trailer; no meaning at streaming, not cgi */
	pHttpChunk->e_state = HTTP_CHUNK_STATE_START; /* we're ready to prepare hex recving */
	pHttpChunk->i_hexa_size = 0;
	pHttpChunk->i_left_size = 0;

	/* chunked debug flags */
	pHttpChunk->encoding_type = HTTP_ENCODING_IDENTITY; /* no encoding; deflate or gzip is unsupported */
	pHttpChunk->i_chunk_hcount = 0;
#if defined (STREAM_HTTP_CHUNKED_DEBUG)
	pHttpChunk->i_chunk_rcount = 0;
	pHttpChunk->i_total_hbytes = 0;
	pHttpChunk->i_total_rbytes = 0;
#endif
	return;
}

static HBOOL INT_HTTP_CheckHex(HINT8 ch)
{
	HBOOL bret = FALSE;

	if ((ch >= _DEF_HEX_NUM_0 && ch <= _DEF_HEX_NUM_9) ||
		(ch >= _DEF_HEX_UPP_A && ch <= _DEF_HEX_UPP_F) ||
		(ch >= _DEF_HEX_LOW_A && ch <= _DEF_HEX_LOW_F) )
	{
		bret = TRUE;
	}
    return bret;
}

/*
 * Parse a content-length header
 */
static int INT_HTTP_Parser_Length(char *p, HINT64 *length)
{
	HINT64 len;

	for (len = 0; *p && VK_isdigit(*p); ++p)
	{
		len = len * 10 + (*p - '0');
	}

	if (*p)
	{
		return DI_IPT_EGENERIC;
	}

	*length = len;
	return DI_IPT_SUCCESS;
}

/*
 * Parse a content-range header
 */
static int INT_HTTP_Parser_TimeRange(char *p, HINT64 *offset, HINT64 *length, HINT64 *size)
{
	HINT64		lFirstOffset = 0;
	HINT64		lEndOffset = 0;
	HINT64		lLength = 0;
	int			first_dec = 0;
	int			last_dec = 0;
	int			len_dec = 0;
	char		chour[2];
	char		cMin[2];
	char		cSec[2];
	char		cMsec[3];
	unsigned int	nLoopCount = 0;
	HBOOL		bNptTimeMode = FALSE;	/* FALSE: 0.000-1234.123/1234.567, TRUE: 00:00:00.000-01:02:03.123/01:02:03.123 */

	if (VK_strncasecmp(p, "npt=", 4) != 0)
	{
		return DI_IPT_EGENERIC;
	}

	p += 4;

	p += 1;
	if (*p == ':')
	{
		bNptTimeMode = TRUE;
		p += 1;
	}
	else
	{
		bNptTimeMode = FALSE;
		p += 1;
	}

	if ((*p == ':') || (bNptTimeMode == TRUE))
	{
		p -= 2;

		PrintDebug("### use npt time table => 00:00:00.000-01:02:03.123/01:02:03.123 ###\n");

		/* First Offset */
		VK_MEM_Memset(chour, 0x0, sizeof(chour));
		VK_MEM_Memset(cMin, 0x0, sizeof(cMin));
		VK_MEM_Memset(cSec, 0x0, sizeof(cSec));
		VK_MEM_Memset(cMsec, 0x0, sizeof(cMsec));

		for(nLoopCount = 0; nLoopCount <sizeof(chour); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' '))
			{
				chour[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lFirstOffset = lFirstOffset + (3600 * VK_atoi(chour));
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		/* offset move (":") */
		p += 1;

		for(nLoopCount = 0; nLoopCount <sizeof(cMin); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' '))
			{
				cMin[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lFirstOffset = lFirstOffset + (60 * VK_atoi(cMin));
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		/* offset move (":") */
		p += 1;

		for(nLoopCount = 0; nLoopCount <sizeof(cSec); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' ') && (*p != '.'))
			{
				cSec[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lFirstOffset = lFirstOffset + VK_atoi(cSec);
			lFirstOffset = lFirstOffset*1000;
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		/* offset move (".") */
		p += 1;

		for(nLoopCount = 0; nLoopCount <sizeof(cMsec); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' ') && (*p != '.'))
			{
				cMsec[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lFirstOffset = lFirstOffset + VK_atoi(cMsec);
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		PrintDebug ("### lFirstOffset = %"PRId64" ###\n",lFirstOffset);

		/* offset move ("-") */
		p += 1;

		/* End Offset */
		VK_MEM_Memset(chour, 0x0, sizeof(chour));
		VK_MEM_Memset(cMin, 0x0, sizeof(cMin));
		VK_MEM_Memset(cSec, 0x0, sizeof(cSec));
		VK_MEM_Memset(cMsec, 0x0, sizeof(cMsec));

		for(nLoopCount = 0; nLoopCount <sizeof(chour); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' '))
			{
				chour[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lEndOffset = lEndOffset + (3600 * VK_atoi(chour));
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		/* offset move (":") */
		p += 1;

		for(nLoopCount = 0; nLoopCount <sizeof(cMin); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' '))
			{
				cMin[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lEndOffset = lEndOffset + (60 * VK_atoi(cMin));
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		/* offset move (":") */
		p += 1;

		for(nLoopCount = 0; nLoopCount <sizeof(cSec); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' ') && (*p != '.'))
			{
				cSec[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lEndOffset = lEndOffset + VK_atoi(cSec);
			lEndOffset = lEndOffset*1000;
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		/* offset move (".") */
		p += 1;

		for(nLoopCount = 0; nLoopCount <sizeof(cMsec); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' ') && (*p != '.'))
			{
				cMsec[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lEndOffset = lEndOffset + VK_atoi(cMsec);
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		PrintDebug ("### lEndOffset = %"PRId64" ###\n",lEndOffset);

		/* offset move ("/") */
		p += 1;

		/* Length Offset */
		VK_MEM_Memset(chour, 0x0, sizeof(chour));
		VK_MEM_Memset(cMin, 0x0, sizeof(cMin));
		VK_MEM_Memset(cSec, 0x0, sizeof(cSec));
		VK_MEM_Memset(cMsec, 0x0, sizeof(cMsec));

		for(nLoopCount = 0; nLoopCount <sizeof(chour); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' '))
			{
				chour[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lLength = lLength + (3600 * VK_atoi(chour));
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		/* offset move (":") */
		p += 1;

		for(nLoopCount = 0; nLoopCount <sizeof(cMin); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' '))
			{
				cMin[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lLength = lLength + (60 * VK_atoi(cMin));
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		/* offset move (":") */
		p += 1;

		for(nLoopCount = 0; nLoopCount <sizeof(cSec); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' ') && (*p != '.'))
			{
				cSec[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lLength = lLength + VK_atoi(cSec);
			lLength = lLength*1000;
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		/* offset move (".") */
		p += 1;

		for(nLoopCount = 0; nLoopCount <sizeof(cMsec); nLoopCount++)
		{
			if((*p != ':') && (*p != '-') && (*p != '/') && (*p != ' ') && (*p != '.'))
			{
				cMsec[nLoopCount] = *p;
			}
			else
			{
				break;
			}

			++p;
		}

		if(nLoopCount >= 1)
		{
			lLength = lLength + VK_atoi(cMsec);
		}
		else
		{
			return DI_IPT_EGENERIC;
		}

		PrintDebug ("### lLength = %"PRId64" ###\n",lLength);

		*length = lEndOffset - lFirstOffset + 1;
		*offset = lFirstOffset;
		*size = lLength;
	}
	else
	{
		p -= 2;

		PrintDebug("### use npt time table => 0.000-1234.123/1234.567 ###\n");

		if (*p == '*')
		{
			lFirstOffset = lEndOffset = -1;
			++p;
		}
		else
		{
			for (lFirstOffset = 0; *p && VK_isdigit(*p); ++p)
			{
				lFirstOffset = lFirstOffset * 10 + *p - '0';
			}

			first_dec = 0;
			if ( *p == '.' )
			{
				int c = 0; p++;
				for (first_dec = 0; *p && VK_isdigit(*p); ++p)
				{
					if ( c < 3 )
					{
						first_dec = first_dec * 10 + *p - '0';
					}
					c++;
				}

				for ( ; c < 3 ; c ++ )
				{
					first_dec = first_dec * 10 ;
				}
			}

			lFirstOffset = lFirstOffset *1000 + first_dec;

			if (*p != '-')
			{
				return DI_IPT_EGENERIC;
			}

			for (lEndOffset = 0, ++p; *p && VK_isdigit(*p); ++p)
			{
				lEndOffset = lEndOffset * 10 + *p - '0';
			}

			last_dec = 0;
			if ( *p == '.' )
			{
				int c = 0; p++;
				for (last_dec = 0; *p && VK_isdigit(*p); ++p)
				{
					if ( c < 3 )
					{
						last_dec = last_dec * 10 + *p - '0';
					}

					c++;
				}

				for ( ; c < 3 ; c ++ )
				{
					last_dec = last_dec * 10 ;
				}
			}

			lEndOffset = lEndOffset *1000 + last_dec;
		}


		if (lFirstOffset > lEndOffset || *p != '/')
		{
			return DI_IPT_EGENERIC;
		}

		// hoonie : record 0 byte length
		if (*(p+1) == '0' )
		{
			*size = 0;
		}

		for (lLength = 0, ++p; *p && VK_isdigit(*p); ++p)
		{
			lLength = lLength * 10 + *p - '0';
		}

		len_dec = 0;
		if ( *p == '.' )
		{
			int c = 0; p++;

			for (len_dec = 0; *p && VK_isdigit(*p); ++p)
			{
				if ( c < 3 )
				{
					len_dec = len_dec * 10 + *p - '0';
				}
				c++;
			}

			for ( ; c < 3 ; c ++ )
			{
				len_dec = len_dec * 10 ;
			}
		}

		lLength = lLength *1000 + len_dec;


		if (lFirstOffset == -1)
		{
			*length = 0;
		}
		else
		{
			*length = lEndOffset - lFirstOffset + 1;
		}

		*offset = lFirstOffset;
		*size = lLength;
	}

	return DI_IPT_SUCCESS;
}

static void INT_HTTP_DlnaRequestField(STREAM_ACCESS_t *pAccess, HINT64 i_tell )
{
	HTTP_ACCESS_SYS_t		*pHttpSys = NULL;
	struct virtual_socket_t	*pvs = NULL;
	char					cNptStartTime[20];
	HINT64					i_seektime = 0;

	if(pAccess == NULL)
	{
		PrintError ( "%s (%d) Error> invalid pAccess Handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return;
	}

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}

	VK_MEM_Memset(cNptStartTime, 0x0, sizeof(cNptStartTime));

	pHttpSys = (HTTP_ACCESS_SYS_t *) pAccess->p_sys;

	if(pHttpSys != NULL)
	{
		pvs = pHttpSys->p_vs;

		/* Seek Info */
		if((pAccess->t_dlna.b_limited_seek == TRUE) && (i_tell == 0))
		{
			PrintDebug ("[%s:%d] ignore seek operation \n",__FUNCTION__,__LINE__);
		}
		else
		{
			if(pAccess->t_dlna.b_time_seek == TRUE)
			{
				/* Offset */
				if(i_tell == 0)
				{
					pHttpSys->b_persist = TRUE;
					PrintDebug ("[%s:%d] i_tell(%"PRId64") \n",__FUNCTION__,__LINE__,i_tell);
					net_Printf( pHttpSys->fd, pvs, "TimeSeekRange.dlna.org: npt=0.000-\r\n" );
				}
				else
				{
					if(pAccess->t_dlna.i_byterate <= 0)
					{
						pAccess->t_dlna.i_byterate = 1024*1024*4;
					}

					i_seektime = (i_tell*1000)/pAccess->t_dlna.i_byterate;
					pHttpSys->b_persist = TRUE;

					/* Time Base Seek의 경우 Server에서 마지막 근처로 이동시 정확한 Position을 Catch못하는 경우가 발생하여 이에 대해 처리함. */
					if(((pAccess->t_dlna.i_duration -1) <= i_seektime/1000) && (i_seektime > 500))
					{
						i_seektime = i_seektime - 450;
					}

					VK_snprintf(cNptStartTime, sizeof(cNptStartTime)-1, "%"PRIu64".%03d", i_seektime/1000, (int)(i_seektime%1000));
					PrintDebug ("[%s:%d] pNptStartTime(%s) \n",__FUNCTION__,__LINE__,cNptStartTime);
					net_Printf( pHttpSys->fd, pvs, "TimeSeekRange.dlna.org: npt=%s-\r\n", cNptStartTime );
				}
			}
			else if(pAccess->t_dlna.b_byte_seek == TRUE)
			{
				/* Offset */
				if( pHttpSys->i_version == 1 && ! pHttpSys->b_continuous )
				{
					pHttpSys->b_persist = TRUE;
					PrintDebug ("[%s:%d] i_tell(%"PRId64") \n",__FUNCTION__,__LINE__,i_tell);
					net_Printf( pHttpSys->fd, pvs, "Range: bytes=%"PRIu64"-\r\n", i_tell );
				}
			}
			else
			{
				PrintError("%s(%d) Error> Unsupport seek(byte:%d, time:%d)\n", __func__, __LINE__, pAccess->t_dlna.b_byte_seek, pAccess->t_dlna.b_time_seek);
			}
		}

		/* request Content Info */
		net_Printf( pHttpSys->fd, pvs, "getcontentFeatures.dlna.org: 1\r\n" );

		/* request Content Info */
		net_Printf( pHttpSys->fd, pvs, "transferMode.dlna.org: Streaming\r\n" );

		/* Server Side Trick Setting */
		if(pAccess->t_dlna.i_server_trickmode == STREAM_DLNA_SPEED_NORMAL)
		{
			net_Printf( pHttpSys->fd, pvs, "PlaySpeed.dlna.org: speed=1\r\n" );
		}
		else if(pAccess->t_dlna.i_server_trickmode == STREAM_DLNA_SPEED_TRICK)
		{
			net_Printf( pHttpSys->fd, pvs, "PlaySpeed.dlna.org: speed=%s\r\n", pAccess->t_dlna.sz_trick_type );
		}

		/* Seek Info */
		if(pAccess->t_dlna.b_limited_seek == TRUE)
		{
			net_Printf( pHttpSys->fd, pvs, "getAvailableSeekRange.dlna.org: 1\r\n" );
		}

		/* request ifoFileURI */
		net_Printf( pHttpSys->fd, pvs, "PRAGMA: getIfoFileURI.dlna.org\r\n" );
	}

    return;
}

static int INT_HTTP_DlnaResponseField(STREAM_ACCESS_t *pAccess, char *pcFieldName, char *pcValue, HINT64 i_tell)
{
	HTTP_ACCESS_SYS_t   *pHttpSys = NULL;
	HINT64		offset = 0;
	HINT64		length = 0;
	HINT64		size = 0;
	HINT32		nRet = 0;
	HINT8		pcNptStartPos[16];
	HINT8		pcNptEndPos[16];
	HINT8		pcNptTmpString[48];
	HINT8		*pcTempAddr = NULL;
	HINT8		*pszTmp = NULL;
	HINT8		acTmpStatus[32];
	HINT32		nSizePsz = 0;
	HINT32		idx=0;
	HINT32		nStoreIdx = 0;

	if(pAccess == NULL)
	{
		PrintError ( "%s (%d) Error> invalid pAccess Handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	pHttpSys = (HTTP_ACCESS_SYS_t *) pAccess->p_sys;

	if((pHttpSys != NULL) && (pcFieldName != NULL))
	{
		/* parsing dlna response */
		if( !VK_strcasecmp( pcFieldName, "contentFeatures.dlna.org" ) )
		{
			PrintDebug ("### (%s)(%d) contentFeatures.dlna.org=%s ###\n",__FUNCTION__,__LINE__, pcValue );

			if(pAccess->psz_protocolinfo != NULL)
			{
				if(VK_strstr(pAccess->psz_protocolinfo, pcValue) == NULL)
				{
					DD_MEM_Free(pAccess->psz_protocolinfo);
					pAccess->psz_protocolinfo = NULL;

					pAccess->psz_protocolinfo = VK_StrDup(pcValue);
				}
			}
			else
			{
				pAccess->psz_protocolinfo = VK_StrDup(pcValue);
			}
		}
		else if( !VK_strcasecmp( pcFieldName, "peerManager.dlna.org" ) )
		{
			PrintDebug ("### (%s)(%d) peerManager.dlna.org=%s ###\n",__FUNCTION__,__LINE__, pcValue );
		}
		else if( !VK_strcasecmp( pcFieldName, "friendlyName.dlna.org" ) )
		{
			PrintDebug ("### (%s)(%d) friendlyName.dlna.org=%s ###\n",__FUNCTION__,__LINE__, pcValue );
		}
		else if( !VK_strcasecmp( pcFieldName, "scid.dlna.org" ) )
		{
			PrintDebug ("### (%s)(%d) scid.dlna.org=%s ###\n",__FUNCTION__,__LINE__, pcValue );
		}
		else if( !VK_strcasecmp( pcFieldName, "scmsFlag.dlna.org" ) )
		{
			PrintDebug ("### (%s)(%d) scmsFlag.dlna.org=%s ###\n",__FUNCTION__,__LINE__, pcValue );
		}
		else if( !VK_strcasecmp( pcFieldName, "transferMode.dlna.org" ) )
		{
			PrintDebug ("### (%s)(%d) transferMode.dlna.org=%s ###\n",__FUNCTION__,__LINE__, pcValue );
		}
		else if( !VK_strcasecmp( pcFieldName, "availableSeekRange.dlna.org" ) )
		{
			PrintDebug ("### (%s)(%d) availableSeekRange.dlna.org=%s ###\n",__FUNCTION__,__LINE__, pcValue );
			pAccess->t_dlna.i_limited_mode = VK_atoi(&pcValue[0]);

			VK_MEM_Memset(pcNptStartPos, 0x0, sizeof(pcNptStartPos));
			VK_MEM_Memset(pcNptEndPos, 0x0, sizeof(pcNptEndPos));
			VK_MEM_Memset(pcNptTmpString, 0x0, sizeof(pcNptTmpString));

			if(VK_strstr( pcValue, "bytes=" ) != NULL )
			{
				/* Content-Range: bytes 10-20/13244 */
				pszTmp = VK_strstr( pcValue, "bytes=" );

				if(pszTmp != NULL )
				{
					nSizePsz = VK_strlen(pszTmp);

					PrintDebug("%s:(%d) pszTmp : %s\n",__FUNCTION__,__LINE__, pszTmp);

					idx=6;
					nStoreIdx = 0;
					VK_MEM_Memset(acTmpStatus, '\0', sizeof(acTmpStatus));

					/* start-Range parsing */
					while(idx < nSizePsz)
					{
						if(pszTmp[idx] == '-')
						{
							++idx;
							break;
						}
						acTmpStatus[nStoreIdx++] = pszTmp[idx];
						++idx;
					}
					pAccess->t_dlna.i_first_pos = atoll(acTmpStatus);

					/* nsize parsing */
					nStoreIdx=0;
					VK_MEM_Memset(acTmpStatus, '\0', sizeof(acTmpStatus));
					while(idx < nSizePsz)
					{
						acTmpStatus[nStoreIdx++] = pszTmp[idx];
						++idx;
					}
					pAccess->t_dlna.i_end_pos = atoll(acTmpStatus);

					PrintDebug("%s:(%d) i_first_pos(%"PRId64") i_end_pos(%"PRId64")\n",__FUNCTION__,__LINE__, pAccess->t_dlna.i_first_pos, pAccess->t_dlna.i_end_pos);
					PrintDebug ("### (%s)(%d) i_first_pos=%"PRId64", i_end_pos=%"PRId64" ###\n",__FUNCTION__,__LINE__, pAccess->t_dlna.i_first_pos, pAccess->t_dlna.i_end_pos);
				}
			}

			if((VK_strstr( pcValue, "npt" ) != NULL ) && (VK_strstr( pcValue, "bytes" ) != NULL ))
			{
				/* npt: npt=00:01:02:33-00:05:06:33 */
				pszTmp = VK_strstr( pcValue, "npt=" );
				if( pszTmp != NULL )
				{
					idx=4;
					nStoreIdx = 0;

					nSizePsz = VK_strlen(pszTmp);

					PrintDebug("%s:(%d) p : %s\n",__FUNCTION__,__LINE__, pszTmp);

					/* start-Range parsing */
					while(idx < nSizePsz)
					{
						if(pszTmp[idx] == '-')
						{
							++idx;
							break;
						}
						pcNptStartPos[nStoreIdx++] = pszTmp[idx];
						++idx;
					}

					/* nsize parsing */
					nStoreIdx=0;
					while(idx < nSizePsz)
					{
						pcNptEndPos[nStoreIdx++] = pszTmp[idx];
						++idx;
					}

					PrintDebug("%s:(%d) pcNptStartPos=%s pcNptEndPos=%s\n",__FUNCTION__,__LINE__, pcNptStartPos, pcNptEndPos);

					VK_snprintf((char *)pcNptTmpString, sizeof(pcNptTmpString)-1, "npt=%s-%s", pcNptStartPos, pcNptEndPos);
					nRet = INT_HTTP_Parser_TimeRange((char *)pcNptTmpString, &pAccess->t_dlna.i_first_pos, &length, &pAccess->t_dlna.i_end_pos);
					PrintDebug ("### (%s)(%d) pcNptTmpString=%s, pcNptStartPos=%s, pcNptEndPos=%s ###\n",__FUNCTION__,__LINE__, pcNptTmpString, pcNptStartPos, pcNptEndPos);
					PrintDebug ("### (%s)(%d) i_first_pos=%"PRId64", i_end_pos=%"PRId64" ###\n",__FUNCTION__,__LINE__, pAccess->t_dlna.i_first_pos, pAccess->t_dlna.i_end_pos);
				}
			}
			else if(VK_strstr( pcValue, "npt=" ) != NULL )
			{
				/* npt: npt=00:01:02:33-00:05:06:33 */
				pszTmp = VK_strstr( pcValue, "npt=" );

				if( pszTmp != NULL )
				{
					nSizePsz = VK_strlen(pszTmp);

					PrintDebug("%s:(%d) p : %s\n",__FUNCTION__,__LINE__, pszTmp);

					idx=4;
					nStoreIdx = 0;

					/* start-Range parsing */
					while(idx < nSizePsz)
					{
						if(pszTmp[idx] == '-')
						{
							++idx;
							break;
						}
						pcNptStartPos[nStoreIdx++] = pszTmp[idx];
						++idx;
					}

					/* nsize parsing */
					nStoreIdx=0;
					while(idx < nSizePsz)
					{
						pcNptEndPos[nStoreIdx++] = pszTmp[idx];
						++idx;
					}

					PrintDebug("%s:(%d) pcNptStartPos=%s pcNptEndPos=%s\n",__FUNCTION__,__LINE__, pcNptStartPos, pcNptEndPos);

					VK_snprintf((char *)pcNptTmpString, sizeof(pcNptTmpString)-1, "npt=%s-%s", pcNptStartPos, pcNptEndPos);
					nRet = INT_HTTP_Parser_TimeRange((char *)pcNptTmpString, &pAccess->t_dlna.i_first_pos, &length, &pAccess->t_dlna.i_end_pos);
					PrintDebug ("### (%s)(%d) pcNptTmpString=%s, pcNptStartPos=%s, pcNptEndPos=%s ###\n",__FUNCTION__,__LINE__, pcNptTmpString, pcNptStartPos, pcNptEndPos);
					PrintDebug ("### (%s)(%d) i_first_pos=%"PRId64", i_end_pos=%"PRId64" ###\n",__FUNCTION__,__LINE__, pAccess->t_dlna.i_first_pos, pAccess->t_dlna.i_end_pos);
				}
			}
		}
		else if( !VK_strcasecmp( pcFieldName, "TimeSeekRange.dlna.org" ) )
		{
			nRet = INT_HTTP_Parser_TimeRange(pcValue, &offset, &length, &size);
			PrintDebug ("### (%s)(%d) nRet=%d, offset=%"PRId64", length=%"PRId64", size=%"PRId64" ###\n",__FUNCTION__,__LINE__, nRet, offset, length, size );
			if((nRet == 0) && (pAccess->t_dlna.i_duration == 0))
			{
				pAccess->t_dlna.i_duration = size;
			}

			if(pAccess->t_dlna.i_byterate <= 0)
			{
				pAccess->t_dlna.i_byterate = 1024*1024*4;
			}

			pHttpSys->i_remaining = (((length-1) * pAccess->t_dlna.i_byterate) /1000)-1;

			pcTempAddr = (HINT8 *)VK_strstr( pcValue, "bytes=" );
			if(pcTempAddr != NULL )
			{
				/* e.g) bytes=10-20/13244 */
				HINT64 i_ntell = 0;
				HINT64 i_nend = 0;
				HINT64 i_nsize = 0;

				/* Content-Range: bytes 10-20/13244 */
				pszTmp = pcTempAddr;
				nSizePsz = VK_strlen(pszTmp);

				PrintDebug("%s:(%d) p : %s\n",__FUNCTION__,__LINE__, pszTmp);

				int idx=6;
				int nStoreIdx = 0;
				VK_MEM_Memset(acTmpStatus, '\0', sizeof(acTmpStatus));

				/* start-Range parsing */
				while(idx < nSizePsz)
				{
					if(pszTmp[idx] == '-')
					{
						++idx;
						break;
					}
					acTmpStatus[nStoreIdx++] = pszTmp[idx];
					++idx;
				}
				i_ntell = atoll(acTmpStatus);

				/* end-Range parsing */
				nStoreIdx = 0;
				VK_MEM_Memset(acTmpStatus, '\0', sizeof(acTmpStatus));
				while(idx < nSizePsz)
				{
					if(pszTmp[idx] == '/')
					{	++idx;
						break;
					}
					acTmpStatus[nStoreIdx++] = pszTmp[idx];
					++idx;
				}
				i_nend = atoll(acTmpStatus);

				/* nsize parsing */
				nStoreIdx=0;
				VK_MEM_Memset(acTmpStatus, '\0', sizeof(acTmpStatus));
				while(idx < nSizePsz)
				{
					acTmpStatus[nStoreIdx++] = pszTmp[idx];
					++idx;
				}
				i_nsize = atoll(acTmpStatus);

				PrintDebug("%s:(%d) i_ntell(%"PRId64") i_nend(%"PRId64"), i_nsize(%"PRId64")\n",__FUNCTION__,__LINE__, i_ntell, i_nend, i_nsize);

				if(i_nend > i_ntell )
				{
					PrintDebug ("[%s:%d] i_ntell(%"PRId64") pAccess->info.i_pos(%"PRId64"): \n",__FUNCTION__,__LINE__,i_ntell,pAccess->info.i_pos);
//					pAccess->info.i_pos = i_ntell; /* current offset position */
//					pHttpSys->i_remaining = i_nend+1-i_ntell; /* file_size - start_offset */
					HINT64 i_size = (i_nsize > i_nend) ? i_nsize : (i_nend + 1);

					if((pAccess->info.i_size < length) && (pAccess->info.i_size > 0) && (pAccess->t_dlna.b_time_seek == TRUE))
					{
						PrintDebug("Not Change i_size\n");
					}
					else
					{
						/*
						 * Update file_size if there is no content-length like chunked TE
						 * Also, update the file_size if current size value is bigger than prev file_size
						 */
						pAccess->info.i_size = i_size;
					}

					PrintDebug("b_content_range set\n");
					pHttpSys->b_content_range = TRUE;

					PrintDebug ( "%s(%d) stream size=%"PRId64",pos=%"PRId64",remaining=%"PRId64"\n"
							,__FUNCTION__
							,__LINE__
							,i_nsize
							,i_ntell
							,pHttpSys->i_remaining);
				}

				if (pHttpSys->b_has_size == FALSE)
				{
					pHttpSys->b_has_size = TRUE;
				}
			}
			else
			{
				if(size > 0)
				{
					if (pHttpSys->b_has_size == FALSE)
					{
						pHttpSys->b_has_size = TRUE;
					}
				}

				if((pAccess->info.i_size > 0) && (pHttpSys->b_chunked == TRUE) && (pHttpSys->t_chunk.b_last_chunk == TRUE))
				{
					PrintDebug("Not Change i_size\n");
				}
				else
				{
					pAccess->info.i_size = (size * pAccess->t_dlna.i_byterate) /1000;
				}
			}

			PrintDebug ("### (%s)(%d) nRet=%d, length=%"PRId64", remaining=%"PRId64", size=%"PRId64" ###\n",__FUNCTION__,__LINE__, nRet, length, pHttpSys->i_remaining, pAccess->info.i_size );
		}
		else if(!VK_strcasecmp( pcFieldName, "Content-Length") && (pAccess->t_dlna.b_time_seek == TRUE))
		{
			nRet = INT_HTTP_Parser_Length(pcValue, &length);
			if((nRet == 0) && (pAccess->t_dlna.i_duration == 0))
			{
				pAccess->t_dlna.i_duration = length;
			}

			pHttpSys->i_remaining = length;
			PrintDebug("b_content_length set\n");
			pHttpSys->b_content_length = TRUE;

			PrintDebug ("(%s)(%d) this frame size=%"PRId64"\n",__FUNCTION__,__LINE__, pHttpSys->i_remaining );
			PrintDebug ("### (%s)(%d) nRet=%d, length=%"PRId64", size=%"PRId64" ###\n",__FUNCTION__,__LINE__, nRet, length, pAccess->info.i_size );
		}
		else if(!VK_strcasecmp( pcFieldName, "Content-Length"))
		{
			nRet = INT_HTTP_Parser_Length(pcValue, &length);
			if((pAccess->info.i_size < length) && (pAccess->info.i_size > 0) && (pAccess->t_dlna.b_time_seek == TRUE))
			{
				PrintDebug("ignore size setting\n");
			}

			if(pAccess->info.i_size <= 0)
			{
				HINT64 i_size = i_tell + (pHttpSys->i_remaining = length);
				pHttpSys->b_has_size = TRUE;
				pAccess->info.i_size = i_size;
				pHttpSys->b_content_length = TRUE;

				PrintDebug ("(%s)(%d) this frame size=%"PRId64"\n",__FUNCTION__,__LINE__, pHttpSys->i_remaining );
			}
		}
		else
		{
	    	return DI_IPT_EGENERIC;
		}
	}

    return DI_IPT_SUCCESS;
}

static int INT_HTTP_Request ( STREAM_ACCESS_t *pAccess, HINT64 i_tell )
{
	HTTP_ACCESS_SYS_t   *pHttpSys = NULL;
	char	*psz =NULL;
	char 	*p=NULL;
	char	*p_str=NULL;
	struct virtual_socket_t	 *pvs = NULL;
	HTTP_CHUNK_t *pHttpChunk = NULL;
	int		nDlnaResponseRet = 0;
	int		i_retry = 0;
	HBOOL bRemainedRepsonse = FALSE;

	PrintEnter;

	if(pAccess == NULL)
	{
		PrintError ( "%s (%d) Error> invalid pAccess Handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	pHttpSys = (HTTP_ACCESS_SYS_t *) pAccess->p_sys;
	pvs = pHttpSys->p_vs;

	PrintDebug ("%s(%d) socket %d\n",__FUNCTION__,__LINE__,pHttpSys->fd);

	pHttpSys->b_cache_control = FALSE;
	pHttpSys->b_persist = FALSE;
	pHttpSys->i_remaining = 0;

	pHttpSys->b_inet_radio = FALSE;
	pHttpSys->b_content_length = FALSE;
	pHttpSys->b_content_range = FALSE;
	pHttpSys->b_content_mime = FALSE;

	i_retry = 0;
func_request_again:
	i_retry++;
	if( pHttpSys->b_proxy )
	{
		if( pHttpSys->url.psz_path )
		{
			net_Printf(  pHttpSys->fd, NULL,
						"GET http://%s:%d%s HTTP/1.%d\r\n",
						pHttpSys->url.psz_host, pHttpSys->url.i_port,
						pHttpSys->url.psz_path, pHttpSys->i_version );
		}
		else
		{
			net_Printf( pHttpSys->fd, NULL,
						"GET http://%s:%d/ HTTP/1.%d\r\n",
						pHttpSys->url.psz_host, pHttpSys->url.i_port,
						pHttpSys->i_version );
		}
	}
	else
	{
		const char *psz_path = pHttpSys->url.psz_path;
		if( !psz_path || !*psz_path )
		{
			psz_path = "/";
		}
		if( pHttpSys->url.i_port != (pvs ? 443 : 80) )
		{
			net_Printf( pHttpSys->fd, pvs,
						"GET %s HTTP/1.%d\r\nHost: %s:%d\r\n",
						psz_path, pHttpSys->i_version, pHttpSys->url.psz_host,
						pHttpSys->url.i_port );
		}
		else
		{
			net_Printf( pHttpSys->fd, pvs,
						"GET %s HTTP/1.%d\r\nHost: %s\r\n",
						psz_path, pHttpSys->i_version, pHttpSys->url.psz_host );
		}
	}

	/* User Agent */
	net_Printf( pHttpSys->fd, pvs, "User-Agent: %s\r\n", pHttpSys->psz_user_agent );

	if(pAccess->t_dlna.b_dlna == TRUE)
	{
		(void)INT_HTTP_DlnaRequestField(pAccess, i_tell );
	}
	else
	{
		/* Offset */
		if( pHttpSys->i_version == 1 && ! pHttpSys->b_continuous )
		{
			pHttpSys->b_persist = TRUE;
			PrintDebug ("[%s:%d] i_tell(%"PRId64") \n",__FUNCTION__,__LINE__,i_tell);
			net_Printf( pHttpSys->fd, pvs, "Range: bytes=%"PRIu64"-\r\n", i_tell );
		}
	}

	/* Cookies */
	if( pHttpSys->cookies )
	{
#if SUPPORT_HUMAX_COOKIE
		net_Printf( pHttpSys->fd, pvs, "Cookie: %s\r\n", pHttpSys->cookies);
#endif
#if 0 /* junsy TODO: 쿠키 처리 필요함 (추후 고려) */
		/* junsy : test code : delete cookie*/
		PrintError("%s(%d) Error> Cookie is not supported yet.",__FUNCTION__,__LINE__);
#endif
	}

	/* Authentication */
	if( pHttpSys->url.psz_username || pHttpSys->url.psz_password )
	{
		INT_HTTP_AuthReply( pAccess, "", &pHttpSys->url, &pHttpSys->auth );
	}

	/* Proxy Authentication */
	if( pHttpSys->proxy.psz_username || pHttpSys->proxy.psz_password )
	{
		INT_HTTP_AuthReply( pAccess, "Proxy-", &pHttpSys->proxy, &pHttpSys->proxy_auth );
	}

	/* ICY meta data request */
	net_Printf( pHttpSys->fd, pvs, "Icy-MetaData: 1\r\n" );

	if( net_Printf( pHttpSys->fd, pvs, "\r\n" ) < 0 )
	{
		PrintError ( "%s (%d) Error> failed to send request\n",__FUNCTION__,__LINE__ );
		(void)INT_HTTP_Disconnect ( pAccess );
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	/* Read Answer */
	if( ( psz = net_Gets( pHttpSys->fd, pvs ) ) == NULL )
	{
		PrintError ( "%s (%d) Error> failed to read answer\n",__FUNCTION__,__LINE__ );
		if(pAccess->t_dlna.b_dlna == TRUE)
		{
			/* Wifi Fail에 대한 처리 대응 */
			PrintDebug( "%s (%d) Retry Read\n",__FUNCTION__,__LINE__ );

			if( ( psz = net_Gets( pHttpSys->fd, pvs ) ) == NULL )
			{
				PrintError ( "%s (%d) Error> failed to read answer(retry)\n",__FUNCTION__,__LINE__ );
				goto error;
			}
		}
		else
		{
			goto error;
		}
	}

	DumpHttpResponse(psz);

	if( !VK_strncmp( psz, "HTTP/1.", 7 ) )
	{
		pHttpSys->psz_protocol = strdup("HTTP");
		pHttpSys->i_code = VK_atoi( &psz[9] );
		pAccess->info.i_httpstatecode = pHttpSys->i_code;
	}
	else if( !VK_strncmp( psz, "ICY", 3 ) )
	{
		pHttpSys->psz_protocol = strdup("ICY");
		pHttpSys->i_code = VK_atoi( &psz[4] );
		pHttpSys->b_reconnect = TRUE;
	}
	else
	{
		PrintError ( "%s (%d) Error> invalid HTTP reply '%s'\n",__FUNCTION__,__LINE__, psz );
		if( psz != NULL )
		{
		DD_MEM_Free ( psz );
			psz = NULL;
		}
		goto error;
	}

	/* DLNA_CTT: Request again for 1xx response; */
	/* (pAccess->t_dlna.b_dlna == TRUE) */
	if ((pAccess->t_dlna.b_dlna == TRUE) &&
		((pHttpSys->i_code == HTTP_STATUS_CONTINUE) || (pHttpSys->i_code == HTTP_STATUS_SWITCHING)))
	{

		/* We must request again for 1xx response; So, read(cleaning) prev remained message*/
		bRemainedRepsonse = TRUE;
		do
		{
			psz = net_Gets( pHttpSys->fd, NULL );
			if( psz == NULL )
			{
				PrintError("%s(%d) Error> Can't read HTTP response\n", __FUNCTION__, __LINE__);
				bRemainedRepsonse = FALSE;
			}
			else
			{
				DumpHttpResponse(psz);
			  	if( *psz == '\0' )
			  	{
					bRemainedRepsonse = FALSE;
				}
			}

			if( psz != NULL )
			{
				DD_MEM_Free ( psz );
				psz = NULL;
			}
		} while( bRemainedRepsonse );

		/* Request again */
		goto func_request_again;
	}
	PrintDebug ("%s (%d) protocol '%s' answer code %d\n",__FUNCTION__,__LINE__, pHttpSys->psz_protocol, pHttpSys->i_code );

	if( !VK_strcmp( pHttpSys->psz_protocol, "ICY" ) )
	{
		pHttpSys->b_seekable = FALSE;
	}
	/* else { continue ; } */

	if( pHttpSys->i_code != HTTP_STATUS_PARTIAL_CONTENT		/* 206 */
		&& pHttpSys->i_code != HTTP_STATUS_UNAUTHORIZED )	/* 401 */
	{
		pHttpSys->b_seekable = FALSE;
	}
	/* else { continue ; } */

	/* Authentication error - We'll have to display the dialog */
	if( pHttpSys->i_code == HTTP_STATUS_UNAUTHORIZED ) /* 401 */
	{
		PrintError ("%s (%d) Error> Authentication Error\n",__FUNCTION__,__LINE__);
		 /* for YouView Back-Off */
		if( psz != NULL )
		{
			DD_MEM_Free ( psz );
			psz = NULL;
		}
		goto error;
	}
	/* Other fatal error */
	else if(pHttpSys->i_code == HTTP_STATUS_MULTIPLE_CHOICE
		|| pHttpSys->i_code == HTTP_STATUS_UNUSED
		|| pHttpSys->i_code == HTTP_STATUS_CREATED
		|| (pHttpSys->i_code >= HTTP_STATUS_BAD_REQUEST && pHttpSys->i_code < HTTP_STATUS_INTERNAL_SERVER_ERROR)
		|| pHttpSys->i_code == HTTP_STATUS_NOT_IMPLEMENTED
		|| pHttpSys->i_code == HTTP_STATUS_BAD_GATEWAY
		|| pHttpSys->i_code == HTTP_STATUS_VERSION_NOT_SUPPORTED
		|| pHttpSys->i_code == HTTP_STATUS_MHEG_UNKNOWN) /* 300/306/, 4xx or 500/501/502/505/622 */
	{
		PrintError ( "%s (%d) Error> error: %s\n",__FUNCTION__,__LINE__, psz );
		if( psz != NULL )
		{
			DD_MEM_Free ( psz );
			psz = NULL;
		}
		goto error;
	}

	if( psz != NULL )
	{
		DD_MEM_Free ( psz );
		psz = NULL;
	}

	pAccess->info.i_httpstatecode = pHttpSys->i_code;

	for( ;; )
	{
		/* Getting & Parsing HTTP Response Messages */
		psz = net_Gets( pHttpSys->fd, pvs );

		if( psz == NULL )
		{
			PrintError ( "%s (%d) Error> failed to read answer\n",__FUNCTION__,__LINE__ );

			if(pAccess->t_dlna.b_dlna == TRUE)
			{
				/* Wifi Fail에 대한 처리 대응 */
				PrintDebug( "%s (%d) Retry Read\n",__FUNCTION__,__LINE__ );

				if( ( psz = net_Gets( pHttpSys->fd, pvs ) ) == NULL )
				{
					PrintError ( "%s (%d) Error> failed to read answer(retry)\n",__FUNCTION__,__LINE__ );
					goto error;
				}
			}
			else
			{
				goto error;
			}
		}

		DumpHttpResponse(psz);

		if( pAccess->b_alive != TRUE)
		{
			if( psz != NULL )
			{
				DD_MEM_Free ( psz );
				psz = NULL;
			}
			PrintError ( "%s (%d) Error> pAccess->b_alive is false!! \n",__FUNCTION__,__LINE__ );
			goto error;
		}
		if( *psz == '\0' )
		{
			if( psz != NULL )
			{
				DD_MEM_Free ( psz );
				psz = NULL;
			}
			break;
		}

		if( ( p = VK_strchr( psz, ':' ) ) == NULL )
		{
			PrintError ( "%s (%d) Error> malformed header line: %s\n",__FUNCTION__,__LINE__, psz );
			if( psz != NULL )
			{
				DD_MEM_Free ( psz );
				psz = NULL;
			}
			goto error;
		}

		*p++ = '\0';

		while( *p == ' ' ) p++;

		if(pAccess->t_dlna.b_dlna == TRUE)
		{
			nDlnaResponseRet = INT_HTTP_DlnaResponseField(pAccess, psz, p, i_tell);
			if( nDlnaResponseRet == DI_IPT_SUCCESS)
			{
				if(psz != NULL)
				{
					DD_MEM_Free ( psz );
					psz = NULL;
				}

				continue;
			}
		}

		/* Put known tokens into stream handler */
		if( !VK_strcasecmp( psz, "Content-Length" ) )
		{
			HINT64 i_size = i_tell + (pHttpSys->i_remaining = atoll( p ));
			if(i_size > pAccess->info.i_size) {
				pHttpSys->b_has_size = TRUE;
				pAccess->info.i_size = i_size;
			}

			PrintDebug("b_content_length set\n");
			pHttpSys->b_content_length = TRUE;

			PrintDebug ("(%s)(%d) this frame size=%"PRId64"\n",__FUNCTION__,__LINE__, pHttpSys->i_remaining );
		}
		else if( !VK_strcasecmp( psz, "Accept-Ranges" ) )
		{
			/*  Check whether the range is unacceptable or not */
			if( !VK_strncasecmp(p, "none", 4) )
			{
				/* 'bytes' is expected, not 'none' response */
				pHttpSys->b_range_accepted = FALSE;
				PrintError( "%s(%d) Error> Unexpected Accept-Ranges[%s]\n",__FUNCTION__,__LINE__, p);
			}
			pHttpSys->b_range_response = TRUE;
		}
		else if( !VK_strcasecmp( psz, "Content-Range" ) )
		{
			/* Response for Content-Range */
			/* e.g) Content-Range: bytes 10-20/13244 */
			HINT64 i_ntell = i_tell; /* request start_offset */
			HINT64 i_nend = (pAccess->info.i_size > 0)?(pAccess->info.i_size - 1):i_tell;
			HINT64 i_nsize = pAccess->info.i_size;

			/* Content-Range: bytes 10-20/13244 */
#if 0 /* Remove Prevent Defect */
			VK_sscanf(p,"bytes %"PRId64"-%"PRId64"/%"PRId64,&i_ntell,&i_nend,&i_nsize);
#else
			{
				char *pszTmp = p;
				char acTmpStatus[21];
				int nSizePsz = VK_strlen(p);

				PrintDebug("%s:(%d) p : %s\n",__FUNCTION__,__LINE__, p);

				if( VK_strstr(pszTmp, "bytes ") )
				{
					int idx=6;
					int nStoreIdx = 0;
					VK_MEM_Memset(acTmpStatus, '\0', sizeof(acTmpStatus));

					/* start-Range parsing */
					while(idx < nSizePsz)
					{
						if(pszTmp[idx] == '-')
						{
							++idx;
							break;
						}
						acTmpStatus[nStoreIdx++] = pszTmp[idx];
						++idx;
					}
					i_ntell = atoll(acTmpStatus);

					/* end-Range parsing */
					nStoreIdx = 0;
					VK_MEM_Memset(acTmpStatus, '\0', sizeof(acTmpStatus));
					while(idx < nSizePsz)
					{
						if(pszTmp[idx] == '/')
						{	++idx;
							break;
						}
						acTmpStatus[nStoreIdx++] = pszTmp[idx];
						++idx;
					}
					i_nend = atoll(acTmpStatus);

					/* nsize parsing */
					nStoreIdx=0;
					VK_MEM_Memset(acTmpStatus, '\0', sizeof(acTmpStatus));
					while(idx < nSizePsz)
					{
						acTmpStatus[nStoreIdx++] = pszTmp[idx];
						++idx;
					}
					i_nsize = atoll(acTmpStatus);

					PrintDebug("%s:(%d) i_ntell(%"PRId64") i_nend(%"PRId64"), i_nsize(%"PRId64")\n",__FUNCTION__,__LINE__, i_ntell, i_nend, i_nsize);

				}
			}
#endif

			if(i_nend > i_ntell )
			{
				PrintDebug ("[%s:%d] i_ntell(%"PRId64") pAccess->info.i_pos(%"PRId64"): \n",__FUNCTION__,__LINE__,i_ntell,pAccess->info.i_pos);
				pAccess->info.i_pos = i_ntell; /* current offset position */
				pHttpSys->i_remaining = i_nend+1-i_ntell; /* file_size - start_offset */
				HINT64 i_size = (i_nsize > i_nend) ? i_nsize : (i_nend + 1);

				if(i_size > pAccess->info.i_size)
				{
					/*
					 * Update file_size if there is no content-length like chunked TE
					 * Also, update the file_size if current size value is bigger than prev file_size
					 */
					pAccess->info.i_size = i_size;
				}
				PrintDebug("b_content_range set\n");
				pHttpSys->b_content_range = TRUE;


				PrintDebug ( "%s(%d) stream size=%"PRId64",pos=%"PRId64",remaining=%"PRId64"\n"
						,__FUNCTION__
						,__LINE__
						,i_nsize
						,i_ntell
						,pHttpSys->i_remaining);
			}

			if (pHttpSys->b_has_size == FALSE)
			{
				pHttpSys->b_has_size = TRUE;
			}
		}
		else if( !VK_strcasecmp( psz, "Connection" ) )
		{
#if 0 /* Remove Prevent*/
			int i = -1;

			PrintDebug ( "%s(%d) Connection: %s\n",__FUNCTION__,__LINE__,p );

			VK_sscanf(p, "close%n",&i);

			if( i >= 0 )
			{
				pHttpSys->b_persist = FALSE;
			}
#else
			PrintDebug ( "%s(%d) Connection: %s\n",__FUNCTION__,__LINE__,p );

			if(VK_memcmp(p, "close", 5) == 0)
			{
				pHttpSys->b_persist = FALSE;
			}
#endif
		}
		else if( !VK_strcasecmp( psz, "Location" ) )
		{
			char * psz_new_loc;

			/* This does not follow RFC 2068, but yet if the url is not absolute,
			 * handle it as everyone does. */
			if( p[0] == '/' )
			{
				const char *psz_http_ext = NULL;

				if(VK_strstr(p, "https//") != NULL)
				{
					psz_http_ext = "s";
				}
				else
				{
					psz_http_ext = "";
				}

				if( pHttpSys->url.i_port == ( (VK_strstr(p, "https//") != NULL) ? 443 : 80 ) )
				{
					if( asprintf(&psz_new_loc, "http%s://%s%s", psz_http_ext, pHttpSys->url.psz_host, p) < 0 )
						goto error;
				}
				else
				{
					if( asprintf(&psz_new_loc, "http%s://%s:%d%s", psz_http_ext, pHttpSys->url.psz_host, pHttpSys->url.i_port, p) < 0 )
						goto error;
				}
			}
			else
			{
				psz_new_loc = strdup( p );
			}

			if(pHttpSys->psz_location != NULL)
			{
				DD_MEM_Free ( pHttpSys->psz_location );
				pHttpSys->psz_location = NULL;
			}
			pHttpSys->psz_location = psz_new_loc;
		}
		/*****************************************
		 * YouView Specific Timeline Header
		 * 		X-IndexFileLocation
		 *****************************************/
		else if( !VK_strcasecmp( psz, "X-IndexFileLocation" ) )
		{
			if(pHttpSys->psz_index == NULL)
			{
				pHttpSys->psz_index = strdup (p);
				PrintDebug ("X-IndexFileLocation: %s\n", pHttpSys->psz_index);
			}
		}
		/*****************************************
		 * YouView Specific Timeline Header
		 * 		X-BytesPerSecond
		 *****************************************/
		else if( !VK_strcasecmp( psz, "X-BytesPerSecond" ) )
		{
			if(pHttpSys->psz_bps == NULL)
			{
				pHttpSys->psz_bps = strdup (p);
				PrintDebug ("X-BytesPerSecond: %s\n", pHttpSys->psz_bps);
			}
		}
		else if( !VK_strcasecmp( psz, "Retry-After" ) )  /* for Back-Off algo. YouView */
		{
			PrintDebug ( "%s(%d) Retry-After: %s\n",__FUNCTION__,__LINE__, p );
			if( !VK_strcasecmp( p, "GMT" ) )
			{
				PrintError ( "%s(%d) Error> GMT Format is not supported yet\n",__FUNCTION__,__LINE__ );
			}
			else
			{
				pHttpSys->i_backoff_retry_delay = VK_atoi(p) * 1000; /* convert to millisec */
			}
		}
		else if( !VK_strcasecmp( psz, "Cache-Control" ) )  /* for Reconnect with Original URL */
		{
			// TODO:
			PrintDebug ( "%s(%d) Cache-Control: %s\n",__FUNCTION__,__LINE__, p );
			pHttpSys->b_cache_control = TRUE;
		}
		else if( !VK_strcasecmp( psz, "Expires" ) )  /* for Reconnect with Original URL */
		{
			// TODO:
			PrintDebug ( "%s(%d) Expires: %s\n",__FUNCTION__,__LINE__, p );
			pHttpSys->b_cache_control = TRUE;
		}
		else if( !VK_strcasecmp( psz, "Content-Type" ) )
		{
			if(pHttpSys->psz_mime != NULL)
			{
				VK_free ( pHttpSys->psz_mime );
				pHttpSys->psz_mime = NULL;
			}

			//if there is not content length and content range, check mime type.
			//mime type is a kind of audio, set the inet radio flag.
			pHttpSys->b_content_mime = TRUE;
			PrintDebug( "b_content_mime set\n" );

			pHttpSys->psz_mime = strdup( p );
			PrintDebug ( "Content-Type: %s\n", pHttpSys->psz_mime );
		}
		else if( !VK_strcasecmp( psz, "Content-Encoding" ) )
		{
			PrintDebug ( "%s(%d) Content-Encoding: %s\n",__FUNCTION__,__LINE__, p );
			if( VK_strcasecmp( p, "identity" ) )
			{
				PrintError ( "%s(%d) Error> Compressed content not supported. Rebuild with zlib support.\n",__FUNCTION__,__LINE__ );
			}
		}
		else if( !VK_strcasecmp( psz, "Pragma" ) )
		{
			if( !VK_strcasecmp( psz, "Pragma: features" ) )
			{
				pHttpSys->b_mms = TRUE;
			}

			if(pHttpSys->psz_pragma != NULL)
			{
				DD_MEM_Free ( pHttpSys->psz_pragma );
				pHttpSys->psz_pragma = NULL;
			}

			pHttpSys->psz_pragma = strdup( p );

			PrintDebug ( "%s(%d) Pragma: %s\n",__FUNCTION__,__LINE__, pHttpSys->psz_pragma );
		}
		else if( !VK_strcasecmp( psz, "Server" ) )
		{
			PrintDebug ( "%s(%d) Server: %s\n",__FUNCTION__,__LINE__, p );

			if( !VK_strncasecmp( p, "Icecast", 7 ) || !VK_strncasecmp( p, "Nanocaster", 10 ) )
			{
				/* Remember if this is Icecast
				 * we need to force demux in this case without breaking
				 *  autodetection */

				/* Let live 365 streams (nanocaster) piggyback on the icecast
				 * routine. They look very similar */

				pHttpSys->b_reconnect = TRUE;
				pHttpSys->b_pace_control = FALSE;
				pHttpSys->b_icecast = TRUE;
				pHttpSys->b_inet_radio = TRUE;
			}
		}
		else if( !VK_strcasecmp( psz, "Transfer-Encoding" ) )
		{
			/*
			 * RFC-2616, section 3.6 Transfer Coding
			 * transfer-coding    = "chunked" | transfer-extension
			 * transfer-extension = token *( ";" parameter )
			 */
			PrintDebug ( "%s(%d) Transfer-Encoding: %s\n",__FUNCTION__,__LINE__, p );
			if( !VK_strncasecmp( p, "chunked", 7 ) )
			{
				pHttpSys->b_chunked = TRUE;
				INT_HTTP_PrepareChunking(pHttpSys);
			}
			else
			{
				PrintError( "%s(%d) Error> Unsupported TE[%s]\n",__FUNCTION__,__LINE__, p);
			}

#if 0 /* Move to the outside of loop */
			if (((pHttpSys->b_chunked == TRUE) && (pHttpSys->b_range_accepted == FALSE)) ||
				((pHttpSys->b_chunked == TRUE) && (pHttpSys->b_range_response == FALSE)))
			{
				pAccess->info.i_size = 0xFFFFFFFFFFFFFF;
				pHttpSys->i_remaining = 0xFFFFFFFFFFFFFF;
			}
#endif
			pHttpChunk = &pHttpSys->t_chunk;

			/* Check transfer-coding */
			if (pHttpSys->b_chunked == TRUE)
			{
				/* Whenever a transfer-coding is applied to a message-body,
				 * the set of transfer-codings MUST include "chunked" */
				if (!VK_strncasecmp(p, "identity", 8)) /* identity(no encoding) */
				{
					pHttpChunk->encoding_type = HTTP_ENCODING_IDENTITY;
				}
				else if (!VK_strncasecmp(p, "deflate", 7)) /* deflate: unsupported */
				{
					pHttpChunk->encoding_type = HTTP_ENCODING_DEFLATE;
				}
				else if (!VK_strncasecmp(p, "gzip", 4)) /* gzip: unsupported */
				{
					pHttpChunk->encoding_type = HTTP_ENCODING_GZIP;
				}
				else if (!VK_strncasecmp(p, "x-gzip", 6)) /* x-gzip: unsupported */
				{
					pHttpChunk->encoding_type = HTTP_ENCODING_XGZIP;
				}
				else if (!VK_strncasecmp(p, "compress", 8)) /* compress: unsupported */
				{
					pHttpChunk->encoding_type = HTTP_ENCODING_COMPRESS;
				}
				else if (!VK_strncasecmp(p, "x-compress", 10)) /* x-compress: unsupported */
				{
					pHttpChunk->encoding_type = HTTP_ENCODING_XCOMPRESS;
				}
			}
		}
		else if( !VK_strcasecmp( psz, "Icy-MetaInt" ) )
		{
			pHttpSys->i_icy_meta = VK_atoi( p );

			PrintDebug ( "Icy-MetaInt: %s\n", p );

			if( pHttpSys->i_icy_meta < 0 )
			{
				pHttpSys->i_icy_meta = 0;
			}

			if( pHttpSys->i_icy_meta > 0 )
			{
				pHttpSys->b_icecast = TRUE;
				pHttpSys->b_inet_radio = TRUE;
			}

			PrintError ( "%s(%d) ICY metaint=%d\n",__FUNCTION__,__LINE__, pHttpSys->i_icy_meta );
		}
		else if( !VK_strcasecmp( psz, "Icy-Name" ) )
		{
			if(pHttpSys->psz_icy_name != NULL)
			{
				DD_MEM_Free ( pHttpSys->psz_icy_name );
				pHttpSys->psz_icy_name = NULL;
			}

			p_str = strdup( p );
			pHttpSys->psz_icy_name = UTIL_EnsureUTF8( p_str );
			if(pHttpSys->psz_icy_name == NULL)
			{
				DD_MEM_Free(p_str);
				p_str = NULL;
				PrintDebug ( "%s(%d) Icy-Name detected, but invalid\n",__FUNCTION__,__LINE__);
			}
			else
			{
				PrintDebug ( "%s(%d) Icy-Name: %s\n",__FUNCTION__,__LINE__, pHttpSys->psz_icy_name );
			}

			pHttpSys->b_icecast = TRUE; /* be on the safeside. set it here as well. */
			pHttpSys->b_inet_radio = TRUE;
			pHttpSys->b_reconnect = TRUE;
			pHttpSys->b_pace_control = FALSE;
		}
		else if( !VK_strcasecmp( psz, "Icy-Genre" ) )
		{
			if(pHttpSys->psz_icy_genre != NULL)
			{
				DD_MEM_Free ( pHttpSys->psz_icy_genre );
				pHttpSys->psz_icy_genre = NULL;
			}

			p_str = strdup( p );
			pHttpSys->psz_icy_genre = UTIL_EnsureUTF8( p_str);
			if(pHttpSys->psz_icy_genre == NULL)
			{
				DD_MEM_Free ( p_str );
				p_str = NULL;
				PrintDebug ( "%s(%d) Icy-Genre detected, but invalid string\n",__FUNCTION__,__LINE__);
			}
			else
			{
				PrintDebug ( "%s(%d) Icy-Genre: %s\n",__FUNCTION__,__LINE__, pHttpSys->psz_icy_genre );
			}
		}
		else if( !VK_strncasecmp( psz, "Icy-Notice", 10 ) )
		{
			PrintDebug ( "%s(%d) Icy-Notice: %s\n",__FUNCTION__,__LINE__, p );
		}
		else if( !VK_strncasecmp( psz, "Icy-Url", 7 ) )
		{
			if(pHttpSys->i_icy_url != NULL)
			{
				DD_MEM_Free ( pHttpSys->i_icy_url);
				pHttpSys->i_icy_url = NULL;
			}
			p_str = strdup( p );
			pHttpSys->i_icy_url = UTIL_EnsureUTF8( p_str );
			if(pHttpSys->i_icy_url == NULL)
			{
				DD_MEM_Free(p_str);
				p_str = NULL;
				PrintDebug ( "%s(%d) Icy-Url detected, but invalid string\n",__FUNCTION__,__LINE__);
			}
			else
			{
				PrintDebug ( "%s(%d) Icy-Url: %s\n",__FUNCTION__,__LINE__, pHttpSys->i_icy_url );
			}
		}
		else if( !VK_strncasecmp( psz, "Icy-Br", 6 ) )
		{
			PrintDebug ( "%s(%d) Icy-Br: %s\n",__FUNCTION__,__LINE__, p );
			pHttpSys->i_icy_br = VK_atoi( p );
		}
		else if( !VK_strncasecmp( psz, "Icy-Pub", 7 ) )
		{
			PrintDebug ( "%s(%d) Icy-Pub: %s\n",__FUNCTION__,__LINE__, p );
			pHttpSys->i_icy_pub = VK_atoi( p );
		}
		else if( !VK_strncasecmp( psz, "icy-", 4 ) ||
				!VK_strncasecmp( psz, "ice-", 4 ) ||
				!VK_strncasecmp( psz, "x-audiocast", 11 ) )
		{
			PrintDebug ( "%s(%d) Meta-Info: %s: %s\n",__FUNCTION__,__LINE__, psz, p );
		}
		else if( !VK_strcasecmp( psz, "Set-Cookie" ) )
		{
			if( pHttpSys->cookies )
			{
				PrintDebug ( "%s(%d) Accepting Cookie: %s\n",__FUNCTION__,__LINE__, p );
#if 0
				INT_HTTP_AppendCookie( pHttpSys->cookies, strdup(p) );
#endif
			}
			else
			{
				PrintDebug ( "%s(%d) We have a Cookie we won't remember: %s\n",__FUNCTION__,__LINE__, p );
			}
		}
		else if( !VK_strcasecmp( psz, "www-authenticate" ) )
		{
			PrintDebug ( "%s(%d) Authentication header: %s\n",__FUNCTION__,__LINE__, p );
			INT_HTTP_AuthParseHeader( pAccess, p, &pHttpSys->auth );
		}
		else if( !VK_strcasecmp( psz, "proxy-authenticate" ) )
		{
			PrintDebug ( "%s(%d) Proxy authentication header: %s\n",__FUNCTION__,__LINE__, p );
			INT_HTTP_AuthParseHeader( pAccess, p, &pHttpSys->proxy_auth );
		}
		else if( !VK_strcasecmp( psz, "authentication-info" ) )
		{
			PrintDebug ( "%s(%d) Authentication Info header: %s\n",__FUNCTION__,__LINE__, p );
			if( INT_HTTP_AuthCheckReply( pAccess, p, &pHttpSys->url, &pHttpSys->auth ) )
				goto error;
		}
		else if( !VK_strcasecmp( psz, "proxy-authentication-info" ) )
		{
			PrintDebug ( "%s(%d) Proxy Authentication Info header: %s\n",__FUNCTION__,__LINE__, p );
			if( INT_HTTP_AuthCheckReply( pAccess, p, &pHttpSys->proxy, &pHttpSys->proxy_auth ) )
			{
				goto error;
			}
		}

		if(psz != NULL)
		{
			DD_MEM_Free ( psz );
			psz = NULL;
		}
	}

	if(((pHttpSys->b_content_length != TRUE) && (pHttpSys->b_content_range != TRUE))
	&& (pHttpSys->b_content_mime == TRUE) && (pAccess->t_dlna.b_dlna != TRUE) && (pHttpSys->b_chunked != TRUE))
	{
		pHttpSys->b_inet_radio = TRUE;
	}

	if ((pAccess->t_dlna.b_dlna == FALSE) && (pHttpSys->b_chunked == TRUE) && (pHttpSys->b_content_range == FALSE))
	{
		if ((pHttpSys->b_range_accepted == FALSE) || (pHttpSys->b_range_response == FALSE))
		{
			pAccess->info.i_size = (0xFFFFFFFFFFFFFF-1);
			pHttpSys->i_remaining = (0xFFFFFFFFFFFFFF-1);
		}

		/* For the case that chunked live streaming has [Accept-Ranges: bytes] response header */
		if ((pHttpSys->b_range_response == TRUE) && (pHttpSys->b_has_size == FALSE))
		{
			pAccess->info.i_size = (0xFFFFFFFFFFFFFF-1);
			pHttpSys->i_remaining = (0xFFFFFFFFFFFFFF-1);
		}
	}

	/* We close the stream for zero length data, unless of course the
	 * server has already promised to do this for us.
	 */
	if (pHttpSys->b_has_size)
	{
		if( pAccess->info.i_size != -1 && pHttpSys->i_remaining == 0 && pHttpSys->b_persist )
		{
			(void)INT_HTTP_Disconnect ( pAccess );
		}
	}

	/* 이 부분은 ICY Protocol을 제외한 Http Protocol에서 Size없이 무제한으로 Play되어야 하는 경우 문제가 됨.(Streaming) */
	if(( pAccess->info.i_size == -1) && (pHttpSys->b_inet_radio != TRUE))
	{
			/* eth link down up 시 네트워크에서 ㄷ더돌던 HTTP 200 OK 가 들어오는 경우가 잇다. */
			PrintDebug("There is no Content Length Header\n");
		if (pHttpSys->b_chunked == TRUE)
		{
			PrintDebug("Transfer-Encoding: chunked\n");
		}
		//	goto error;
	}

//	PrintDebug("%s(%d) pAccess->info.i_update(%d): \n",__FUNCTION__,__LINE__,pAccess->info.i_update);
	PrintDebug("%s(%d) pAccess->info.i_size(%"PRId64"): \n",__FUNCTION__,__LINE__,pAccess->info.i_size);
	PrintDebug("%s(%d) pAccess->info.i_pos(%"PRId64"): \n",__FUNCTION__,__LINE__,pAccess->info.i_pos);
//	PrintDebug("%s(%d) pAccess->info.b_eof(%d): \n",__FUNCTION__,__LINE__,pAccess->info.b_eof);
//	PrintDebug("%s(%d) pAccess->info.i_title(%d): \n",__FUNCTION__,__LINE__,pAccess->info.i_title);
//	PrintDebug("%s(%d) pAccess->info.i_seekpoint(%d): \n",__FUNCTION__,__LINE__,pAccess->info.i_seekpoint);
//	PrintDebug("%s(%d) pAccess->info.b_prebuffered(%d): \n",__FUNCTION__,__LINE__,pAccess->info.b_prebuffered);

	PrintExit;
	return DI_IPT_SUCCESS;

error:
	(void)INT_HTTP_Disconnect ( pAccess );
	PrintExit;
	return DI_IPT_EGENERIC;
}

/*****************************************************************************
 * INT_HTTP_Disconnect :
 *****************************************************************************/
static int INT_HTTP_Disconnect ( STREAM_ACCESS_t *pAccess )
{
    HTTP_ACCESS_SYS_t *pHttpSys = NULL;

	PrintEnter;

    if(pAccess == NULL)
    {
        PrintError ( "%s (%d) Error> invalid pAccess Handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
    	return DI_IPT_EGENERIC;
    }

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_IPT_EGENERIC;
	}

    pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;

    if(( pHttpSys->p_tls != NULL) && (pHttpSys->b_ssl == TRUE))
    {
        (void)TLS_Client_Disconnect(pHttpSys->p_tls );
        pHttpSys->p_vs = NULL;
    }

    if( pHttpSys->fd != -1)
    {
        net_Close(pHttpSys->fd);
        pHttpSys->fd = -1;
    }

	if(pAccess->psz_protocolinfo != NULL)
	{
	    DD_MEM_Free ( pAccess->psz_protocolinfo);
		pAccess->psz_protocolinfo = NULL;
	}

	PrintExit;
	return DI_IPT_SUCCESS;
}

/*****************************************************************************
 * "RFC 2617: Basic and Digest Access Authentication" header parsing
 *****************************************************************************/
static char *INT_HTTP_AuthGetParm ( const char *psz_header, const char *psz_param )
{
	const char *pcEnd;
    char pcWhat[VK_strlen (psz_param)+3];

    VK_snprintf( pcWhat, VK_strlen(psz_param)+3, "%s=\"", psz_param );
    psz_header = VK_strstr( psz_header, pcWhat );

	if(psz_header == NULL)
	{
		return NULL;
	}
	else
	{
		psz_header += VK_strlen ( pcWhat );
        pcEnd = VK_strchr( psz_header, '"' );
        if(pcEnd == NULL)
		{
            return strdup( psz_header );
		}
        return strndup ( psz_header, pcEnd - psz_header );
	}
}

static char *INT_HTTP_AuthGetParamNoQuotes ( const char *psz_header, const char *psz_param )
{
    char pcWhat[VK_strlen (psz_param)+2];
    VK_snprintf( pcWhat, VK_strlen (psz_param)+2, "%s=", psz_param );
    psz_header = VK_strstr( psz_header, pcWhat );
    if( psz_header )
    {
        const char *psz_end;
        psz_header += VK_strlen ( pcWhat );
        psz_end = VK_strchr( psz_header, ',' );
        /* XXX: Do we need to filter out trailing space between the value and
         * the comma/end of line? */
        if( !psz_end ) /* Can be valid if this is the last parameter */
            return strdup( psz_header );
        return strndup( psz_header, psz_end - psz_header );
    }
    else
    {
        return NULL;
    }
}

static void INT_HTTP_AuthParseHeader( STREAM_ACCESS_t *pAccess, const char *psz_header, HTTP_AUTH_t *pstAuth )
{
    /* FIXME: multiple auth methods can be listed (comma seperated) */
	UNUSED_PARAM(pAccess);

    /* 2 Basic Authentication Scheme */
    if( !VK_strncasecmp( psz_header, "Basic ", VK_strlen ( "Basic " ) ) )
    {
        PrintDebug ( "Using Basic Authentication\n" );
        psz_header += VK_strlen ( "Basic " );
        pstAuth->psz_realm = INT_HTTP_AuthGetParm ( psz_header, "realm" );
        if( !pstAuth->psz_realm )
		{
            PrintError ( "Basic Authentication: \n" "Mandatory 'realm' parameter is missing\n" );
		}
    }
    /* 3 Digest Access Authentication Scheme */
    else if( !VK_strncasecmp( psz_header, "Digest ", VK_strlen ( "Digest " ) ) )
    {
        PrintDebug ( "Using Digest Access Authentication\n" );
        if( pstAuth->psz_nonce ) return; /* FIXME */
        psz_header += VK_strlen ( "Digest " );
        pstAuth->psz_qop = INT_HTTP_AuthGetParm ( psz_header, "qop" );
        pstAuth->psz_algorithm = INT_HTTP_AuthGetParamNoQuotes ( psz_header, "algorithm" );
        pstAuth->psz_stale = INT_HTTP_AuthGetParamNoQuotes ( psz_header, "stale" );
        pstAuth->psz_opaque = INT_HTTP_AuthGetParm ( psz_header, "opaque" );
        pstAuth->psz_nonce = INT_HTTP_AuthGetParm ( psz_header, "nonce" );
		pstAuth->psz_domain = INT_HTTP_AuthGetParm ( psz_header, "domain" );
        pstAuth->psz_realm = INT_HTTP_AuthGetParm ( psz_header, "realm" );

        pstAuth->i_nonce = 0;

		if( pstAuth->psz_nonce == NULL)
		{
            PrintError ( "Digest Access Authentication: \n" "Mandatory 'nonce' parameter is missing" );
		}

        if( pstAuth->psz_realm == NULL)
		{
            PrintError ( "Digest Access Authentication: \n" "Mandatory 'realm' parameter is missing\n" );
		}

        if( pstAuth->psz_qop ) /* FIXME: parse the qop list */
        {
            char *pcTemp = VK_strchr( pstAuth->psz_qop, ',' );
            if( pcTemp != NULL)
			{
				*pcTemp = '\0';
            }
        }
    }
    else
    {
        const char *pcEnd = VK_strchr( psz_header, ' ' );
        if( pcEnd == NULL)
		{
            PrintError ( "Unknown authentication scheme: '%s'\n", psz_header );
		}
        else
		{
            PrintError ( "Unknown authentication scheme: '%*s'\n", pcEnd - psz_header, psz_header );
		}
    }
}

static char *INT_HTTP_AuthDigest ( STREAM_ACCESS_t *pAccess, URL_t *p_url,
                         HTTP_AUTH_t *p_auth, const char *psz_method )
{
	(void)pAccess;
	char *psz_HA1 = NULL;
	char *psz_HA2 = NULL;
	char *psz_response = NULL;
	struct md5_s md5;
	const char *pcPassword = p_url->psz_password ?p_url->psz_password: "";
	const char *pcUsername = p_url->psz_username ?p_url->psz_username: "";

	/* H(A1) */
	if(p_auth->psz_HA1 != NULL)
	{
		psz_HA1 = strdup( p_auth->psz_HA1 );
		if( psz_HA1 == NULL)
		{
			goto error;
		}
	}
	else
	{
		InitMD5( &md5 );
		AddMD5( &md5, pcUsername, VK_strlen ( pcUsername ) );
		AddMD5( &md5, ":", 1 );
		AddMD5( &md5, p_auth->psz_realm, VK_strlen ( p_auth->psz_realm ) );
		AddMD5( &md5, ":", 1 );
		AddMD5( &md5, pcPassword, VK_strlen ( pcPassword ) );
		EndMD5( &md5 );

		psz_HA1 = psz_md5_hash( &md5 );
		if( psz_HA1 == NULL )
		{
			goto error;
		}

		if( p_auth->psz_algorithm
				&& !VK_strcmp( p_auth->psz_algorithm, "MD5-sess" ) )
		{
			InitMD5( &md5 );
			AddMD5( &md5, psz_HA1, 32 );
			if(psz_HA1 != NULL)
			{
				DD_MEM_Free ( psz_HA1 );
				psz_HA1 = NULL;
			}

			AddMD5( &md5, ":", 1 );
			AddMD5( &md5, p_auth->psz_nonce, VK_strlen ( p_auth->psz_nonce ) );
			AddMD5( &md5, ":", 1 );
			AddMD5( &md5, p_auth->psz_cnonce, VK_strlen ( p_auth->psz_cnonce ) );
			EndMD5( &md5 );

			psz_HA1 = psz_md5_hash( &md5 );
			if( psz_HA1 == NULL )
			{
				goto error;
			}
			p_auth->psz_HA1 = strdup( psz_HA1 );
			if( p_auth->psz_HA1 == NULL )
			{
				goto error;
			}
		}
	}

	/* H(A2) */
	InitMD5( &md5 );
	if( *psz_method )
	{
		AddMD5( &md5, psz_method, VK_strlen ( psz_method ) );
	}
	AddMD5( &md5, ":", 1 );

	if( p_url->psz_path == NULL)
	{
		AddMD5( &md5, "/", 1 );
	}
	else
	{
		AddMD5( &md5, p_url->psz_path, VK_strlen ( p_url->psz_path ) );
	}

	if( p_auth->psz_qop && !VK_strcmp( p_auth->psz_qop, "auth-int" ) )
	{
		char *pcEnt;
		struct md5_s stEnt;

		InitMD5(&stEnt);
		AddMD5(&stEnt, "", 0); /* XXX: entity-body. should be ok for GET */
		EndMD5(&stEnt);
		pcEnt = psz_md5_hash( &stEnt );
		if(pcEnt == NULL)
		{
			goto error;
		}

		AddMD5( &md5, ":", 1 );
		AddMD5( &md5, pcEnt, 32 );
		if(pcEnt != NULL)
		{
			DD_MEM_Free ( pcEnt );
			pcEnt = NULL;
		}
	}
	EndMD5( &md5 );
	psz_HA2 = psz_md5_hash( &md5 );
	if( psz_HA2 == NULL )
	{
		goto error;
	}

	/* Request digest */
	InitMD5( &md5 );
	AddMD5( &md5, psz_HA1, 32 );
	AddMD5( &md5, ":", 1 );
	AddMD5( &md5, p_auth->psz_nonce, VK_strlen ( p_auth->psz_nonce ) );
	AddMD5( &md5, ":", 1 );
	if( (p_auth->psz_qop != NULL)
			&& ( !VK_strcmp( p_auth->psz_qop, "auth" )
				|| !VK_strcmp( p_auth->psz_qop, "auth-int" ) ) )
	{
		char pcInonce[9];
		VK_snprintf( pcInonce, 9, "%08x", p_auth->i_nonce );
		AddMD5( &md5, pcInonce, 8 );
		AddMD5( &md5, ":", 1 );
		AddMD5( &md5, p_auth->psz_cnonce, VK_strlen ( p_auth->psz_cnonce ) );
		AddMD5( &md5, ":", 1 );
		AddMD5( &md5, p_auth->psz_qop, VK_strlen ( p_auth->psz_qop ) );
		AddMD5( &md5, ":", 1 );
	}
	AddMD5( &md5, psz_HA2, 32 );
	EndMD5( &md5 );
	psz_response = psz_md5_hash( &md5 );

error:
	if(psz_HA1 != NULL)
	{
		DD_MEM_Free ( psz_HA1 );
		psz_HA1 = NULL;
	}

	if(psz_HA2 != NULL)
	{
		DD_MEM_Free ( psz_HA2 );
		psz_HA2 = NULL;
	}

	return psz_response;
}


static void INT_HTTP_AuthReply ( STREAM_ACCESS_t *pAccess, const char *psz_prefix, URL_t *p_url, HTTP_AUTH_t *pstAuth )
{
	HTTP_ACCESS_SYS_t *pHttpSys = NULL;
	struct virtual_socket_t     *pvs = NULL;

	const char *psz_username = p_url->psz_username ?p_url->psz_username: "";
	const char *psz_password = p_url->psz_password ?p_url->psz_password: "";

	PrintEnter;
	if(pAccess == NULL)
	{
		PrintError("%s(%d) Error: pAccess is NULL!\n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}
	if(pAccess->p_sys == NULL)
	{
		PrintError("%s(%d) Error: pAccess->p_sys is already NULL!\n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}

	pHttpSys = pAccess->p_sys;

	pvs = pHttpSys->p_vs;

	if( pstAuth->psz_nonce )
	{
		/* Digest Access Authentication */
		char *pcResponse;

		if(    pstAuth->psz_algorithm
				&& VK_strcmp( pstAuth->psz_algorithm, "MD5" )
				&& VK_strcmp( pstAuth->psz_algorithm, "MD5-sess" ) )
		{
			PrintError ( "Digest Access Authentication: \n" "Unknown algorithm '%s'\n", pstAuth->psz_algorithm );
			return;
		}

		if( pstAuth->psz_qop || !pstAuth->psz_cnonce )
		{
			/* FIXME: needs to be really random to prevent man in the middle
			 * attacks */
			DD_MEM_Free ( pstAuth->psz_cnonce );
			pstAuth->psz_cnonce = strdup( "Some random string FIXME" );
		}
		pstAuth->i_nonce ++;

		pcResponse = INT_HTTP_AuthDigest ( pAccess, p_url, pstAuth, "GET" );
		if(pcResponse == NULL)
		{
			return;
		}

		net_Printf( pHttpSys->fd, pvs,
				"%sAuthorization: Digest "
				"username=\"%s\", "
				"realm=\"%s\", "
				"nonce=\"%s\", "
				"uri=\"%s\", "
				"response=\"%s\", "
				"%s%s%s"
				"%s%s%s"
				"%s%s%s"
				"%s%s%s"
				"%s%08x%s"
				"\r\n",
				psz_prefix,
				psz_username,
				pstAuth->psz_realm,
				pstAuth->psz_nonce,
				p_url->psz_path ?p_url->psz_path: "/",
				pcResponse,
				pstAuth->psz_algorithm ? "algorithm=\"" : "",
				pstAuth->psz_algorithm ?pstAuth->psz_algorithm: "",
				pstAuth->psz_algorithm ? "\", " : "",
				pstAuth->psz_cnonce ? "cnonce=\"" : "",
				pstAuth->psz_cnonce ?pstAuth->psz_cnonce: "",
				pstAuth->psz_cnonce ? "\", " : "",
				pstAuth->psz_opaque ? "opaque=\"" : "",
				pstAuth->psz_opaque ?pstAuth->psz_opaque: "",
				pstAuth->psz_opaque ? "\", " : "",
				pstAuth->psz_qop ? "qop=\"" : "",
				pstAuth->psz_qop ?pstAuth->psz_qop: "",
				pstAuth->psz_qop ? "\", " : "",
				pstAuth->i_nonce ? "nc=\"" : "uglyhack=\"",
				pstAuth->i_nonce,
				pstAuth->i_nonce ? "\"" : "\""
					);

		if(pcResponse != NULL)
		{
			DD_MEM_Free ( pcResponse );
			pcResponse = NULL;
		}
	}
	else
	{
		/* Basic Access Authentication */
		char buf[VK_strlen ( psz_username ) + VK_strlen ( psz_password ) + 2];
		char *b64;

		VK_snprintf( buf, sizeof( buf ), "%s:%s", psz_username, psz_password );

		b64 = UTIL_B64Encode ((const char*)buf );
		if( b64 != NULL )
		{
			net_Printf( pHttpSys->fd, pvs, "%sAuthorization: Basic %s\r\n", psz_prefix, b64 );
			if(b64 != NULL)
			{
				DD_MEM_Free ( b64 );
				b64 = NULL;
			}
		}
	}
	PrintExit;
	return;
}


static int INT_HTTP_AuthCheckReply ( STREAM_ACCESS_t *pAccess, const char *psz_header, URL_t *p_url, HTTP_AUTH_t *p_auth )
{
    int i_ret = DI_IPT_EGENERIC;
    char *psz_nextnonce = INT_HTTP_AuthGetParm ( psz_header, "nextnonce" );
    char *psz_qop = INT_HTTP_AuthGetParamNoQuotes ( psz_header, "qop" );
    char *psz_rspauth = INT_HTTP_AuthGetParm ( psz_header, "rspauth" );
    char *psz_cnonce = INT_HTTP_AuthGetParm ( psz_header, "cnonce" );
    char *psz_nc = INT_HTTP_AuthGetParamNoQuotes ( psz_header, "nc" );

    if( psz_cnonce )
    {
        char *psz_digest=NULL;

        if( VK_strcmp( psz_cnonce, p_auth->psz_cnonce ) )
        {
            PrintError ( "%s (%d) Error> HTTP Digest Access Authentication: server replied with a different client nonce value.\n",__FUNCTION__,__LINE__ );
            goto error;
        }

        if( psz_nc )
        {
            int i_nonce;
            i_nonce = strtol( psz_nc, NULL, 16 );
            if( i_nonce != p_auth->i_nonce )
            {
                PrintError ( "%s (%d) Error> HTTP Digest Access Authentication: server replied with a different nonce count value.\n",__FUNCTION__,__LINE__ );
                goto error;
            }
        }

        if( psz_qop && p_auth->psz_qop && VK_strcmp( psz_qop, p_auth->psz_qop ) )
		{
            PrintError ( "%s (%d) Error> HTTP Digest Access Authentication: server replied using a different 'quality of protection' option\n",__FUNCTION__,__LINE__ );
		}

        /* All the clear text values match, let's now check the response digest */
        psz_digest = INT_HTTP_AuthDigest ( pAccess, p_url, p_auth, "" );
		if(NULL == psz_digest)
		{
            PrintError ( "%s (%d) Error> INT_HTTP_AuthDigest returns null pointer .\n",__FUNCTION__,__LINE__ );
			goto error;
		}

        if( VK_strcmp( psz_digest, psz_rspauth ) )
        {
            PrintError ( "%s (%d) Error> HTTP Digest Access Authentication: server replied with an invalid response digest (expected value: %s).\n",__FUNCTION__,__LINE__, psz_digest );
            DD_MEM_Free ( psz_digest );
            psz_digest = NULL;
            goto error;
        }

        if(psz_digest != NULL)
        {
        DD_MEM_Free ( psz_digest );
	        psz_digest = NULL;
        }
    }

    if( psz_nextnonce )
    {
    	if(p_auth->psz_nonce != NULL)
    	{
        DD_MEM_Free ( p_auth->psz_nonce );
	        p_auth->psz_nonce =NULL;
	 }

        p_auth->psz_nonce = psz_nextnonce;
        psz_nextnonce = NULL;
    }

    i_ret = DI_IPT_SUCCESS;

error:
	if(psz_nextnonce != NULL)
	{
		DD_MEM_Free ( psz_nextnonce );
		psz_nextnonce = NULL;
	}

	if(psz_qop != NULL)
	{
		DD_MEM_Free ( psz_qop );
		psz_qop = NULL;
	}

	if(psz_rspauth != NULL)
	{
		DD_MEM_Free ( psz_rspauth );
		psz_rspauth = NULL;
	}

	if(psz_cnonce != NULL)
	{
		DD_MEM_Free ( psz_cnonce );
		psz_cnonce = NULL;
	}

	if(psz_nc != NULL)
	{
		DD_MEM_Free ( psz_nc );
		psz_nc = NULL;
	}

    return i_ret;
}

static void INT_HTTP_AuthReset ( HTTP_AUTH_t *p_auth )
{
	if ( p_auth == NULL ) return;

	if ( p_auth->psz_realm )
	{
		FREENULL( p_auth->psz_realm );
	}
	if ( p_auth->psz_domain )
	{
		FREENULL( p_auth->psz_domain );
	}
	if ( p_auth->psz_nonce )
	{
		FREENULL( p_auth->psz_nonce );
	}
	if ( p_auth->psz_opaque )
	{
		FREENULL( p_auth->psz_opaque );
	}
	if ( p_auth->psz_stale )
	{
		FREENULL( p_auth->psz_stale );
	}
	if ( p_auth->psz_algorithm )
	{
		FREENULL( p_auth->psz_algorithm );
	}
	if ( p_auth->psz_qop )
	{
		FREENULL( p_auth->psz_qop );
	}
	p_auth->i_nonce = 0;
	if ( p_auth->psz_cnonce )
	{
		FREENULL( p_auth->psz_cnonce );
	}
	if ( p_auth->psz_HA1 )
	{
		FREENULL( p_auth->psz_HA1 );
	}

	return;
}

