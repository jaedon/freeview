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

#include "htype.h"
#include "vkernel.h"
#include "stream_global.h"

#include "di_iptuner_config.h"
#include "di_uart.h"
#include "di_iptuner_common.h"
#include "stream_http.h"
#include "transport_tcp.h"

#if defined (SUPPORT_TLS_V1_PROTOCOL)
#include "transport_tls.h"
#endif

#include "util_md5.h"
#include "util_url.h"
#include "util_b64_encode.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define STREAM_HTTP_DEBUG
//#define STREAM_HTTP_CRITICAL_DEBUG
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
/* #define DumpHttpResponse(X)		while (0) ((int (*)(char *, ...)) 0) */
#define DumpHttpResponse(X)		((void)0) /* Changed with warning: statement with no effect */

#endif
#if defined(DUMP_HTTP_STREAM)
#define DumpHttpStream(X,len,pos)		Util_Print_Dump(X, len, pos,1);
#else
/* #define DumpHttpStream(X,len,pos)		while (0) ((int (*)(char *, ...)) 0) */
#define DumpHttpStream(X,len,pos)		((void)0) /* Changed with warning: statement with no effect */
#endif
#else

#define PrintData					while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug				while (0) ((int (*)(char *, ...)) 0)
#define PrintError				while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
/* #define DumpHttpResponse(X)		while (0) ((int (*)(char *, ...)) 0) */
#define DumpHttpResponse(X)		((void)0) /* Changed with warning: statement with no effect */

/* #define DumpHttpStream(X,len,pos)		while (0) ((int (*)(char *, ...)) 0) */
#define DumpHttpStream(X,len,pos)		((void)0) /* Changed with warning: statement with no effect */


#endif /* The END of DEBUG DEFINE */

#ifdef STREAM_HTTP_CRITICAL_DEBUG
#define PrintCriticalDebug      DI_UART_Print
#else
#define PrintCriticalDebug      while (0) ((int (*)(char *, ...)) 0)
#endif

#define HTTP_READ_REDIRECTON_ERROR_RETRYCOUNT	10
#define HTTP_CONNECT_REDIRECTON_ERROR_RETRYCOUNT	10
#define HTTP_CONNECT_ERROR_RETRYCOUNT	10

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
HINT32 s_nLastAnsCode=0;

static HINT8 *s_pcUserAgentInfo=NULL;
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
/* Start static function prototype */

static int INT_HTTP_OpenWithCookies ( STREAM_ACCESS_t *pAccess, char *cookies );

/** About HTTP Session */
static int INT_HTTP_Connect( STREAM_ACCESS_t *, HINT64 );
static int INT_HTTP_Request ( STREAM_ACCESS_t *pAccess, HINT64 i_tell );
static void INT_HTTP_Disconnect ( STREAM_ACCESS_t * );

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
static int INT_HTTP_ReadICYMeta( STREAM_ACCESS_t *pAccess );

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
/* End static function prototype */

int STREAM_HTTP_RequestWithPersist( STREAM_ACCESS_t *pAccess , HINT64 i_tell )
{
	int nRet = DI_IPT_SUCCESS;
	HTTP_ACCESS_SYS_t *pHttpSys=NULL;
	char		 *psz=NULL, *p=NULL;
	char		*psz_host = NULL;
	int		nPort = 0;
	PrintEnter;

	PrintDebug("Access (%d) pAccess->psz_path(%s)\n"
		, (HUINT32) pAccess
		, pAccess->psz_path);
	if(NULL == pAccess || pAccess->p_sys == NULL
	|| pAccess->psz_path == NULL)
	{
        PrintError("invalid pAccess Handle (NULL)\n" );
		nRet = DI_IPT_EGENERIC;
	}
	else
	{
		pHttpSys = (HTTP_ACCESS_SYS_t*)pAccess->p_sys;
		if(pAccess->info.i_size == pAccess->info.i_pos && pHttpSys->b_persist == TRUE)
		{
			pAccess->info.i_size = 0;
			pAccess->info.i_pos = 0;
			pAccess->info.b_eof = FALSE;
			pAccess->info.i_title = 0;
			pAccess->info.i_seekpoint = 0;

			/** Parse URI - remove spaces */

			p = psz = strdup( pAccess->psz_path );
			if ( p == NULL || psz == NULL )
			{
				PrintError("Memory Allocation Fail\n" );
				nRet = DI_IPT_EGENERIC;
				goto error;
			}

			while( (p = VK_strchr( p, ' ' )) != NULL )
			{
				*p = '+';
			}
			psz_host = VK_StrDup(pHttpSys->url.psz_host);
			if(psz_host == NULL)
			{
				PrintError("Memory Allocation Fail\n" );
				nRet = DI_IPT_EGENERIC;
				goto error;
			}
			nPort = pHttpSys->url.i_port;
			UTIL_UrlClean ( &pHttpSys->url);
			UTIL_UrlParse ( &pHttpSys->url, psz, 0 );
			if(psz != NULL)
			{
				VK_free ( psz );
				psz = NULL;
			}

			if( pHttpSys->url.psz_host == NULL || *pHttpSys->url.psz_host == '\0' )
			{
				PrintError("invalid host\n" );
				nRet = DI_IPT_EGENERIC;
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
				pHttpSys->b_ssl = FALSE;
				if( pHttpSys->url.i_port <= 0 )
				{
					pHttpSys->url.i_port = 80;
				}
			}
			if(VK_strcmp( pHttpSys->url.psz_host, psz_host))
			{
				PrintError("host is invalid\n" );
				nRet = DI_IPT_EGENERIC;
				goto error;
			}
			if(pHttpSys->url.i_port != nPort)
			{
				PrintError("port number is invalid\n" );
				nRet = DI_IPT_EGENERIC;
				goto error;
			}
		}
		else
		{
			PrintError("Can't reuse socket\n" );
			nRet = DI_IPT_EGENERIC;
			goto error;
		}
		if(psz_host != NULL)
		{
			DD_MEM_Free(psz_host);
			psz_host = NULL;
		}
	}
	if(INT_HTTP_Request ( pAccess, i_tell))
	{
		pAccess->b_alive = FALSE;
		return -2;
	}
	else
	{
		return DI_IPT_SUCCESS;
	}
error:
	if(psz_host != NULL)
	{
		DD_MEM_Free(psz_host);
		psz_host = NULL;
	}
	PrintExit;
	return nRet;

}


int STREAM_HTTP_Open ( STREAM_ACCESS_t *pAccess )
{
	int nRet = DI_IPT_SUCCESS;

	PrintEnter;

	if(NULL == pAccess)
	{
        PrintError ( "%s (%d) Error> invalid pAccess Handle (NULL)\n",__FUNCTION__,__LINE__ );
		nRet = DI_IPT_EGENERIC;
	}
	else
	{
#if SUPPORT_COOKIE
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

    return nRet;
}
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

    INT_HTTP_Disconnect ( pAccess );

#if SUPPORT_COOKIE
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
	int 	error_retrycount =0;
	HTTP_CHUNK_t *pHttpChunk = NULL;

    PrintEnter;

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

    if( pAccess->info.i_size >= 0 && i_len + pAccess->info.i_pos > pAccess->info.i_size )
    {
        if( ( i_len = pAccess->info.i_size - pAccess->info.i_pos ) == 0 )
        {
            pAccess->info.b_eof = TRUE;
			PrintDebug ("%s(%d) : stream_read returns for EOF\n",__FUNCTION__,__LINE__);
            PrintExit;
            return 0;
        }
    }

#ifdef	CONFIG_IPTUNER_XTREMEHSS_LIVE	/* To support UK Freeview MHEG-ICS live streaming contents - C.G. Kang */
	if ( pHttpSys->b_chunked == TRUE && pHttpSys->b_xtremehss_live == FALSE )
#else
	if ( pHttpSys->b_chunked == TRUE)
#endif
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
			PrintExit;
			return 0;
		}

		/*
		 * RFC-2616, 3.6.1 Chunked Transfer Coding
		 * Chunked-Body	= *chunk
		 *				   last-chunk
		 *				   trailer
		 *				   CRLF
		 * chunk		   = chunk-size [ chunk-extension ] CRLF
		 *				   chunk-data CRLF
		 * chunk-size	  = 1*HEX
		 * last-chunk	  = 1*("0") [ chunk-extension ] CRLF
		 * chunk-extension = *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
		 * chunk-ext-name  = token
		 * chunk-ext-val   = token | quoted-string
		 * chunk-data	  = chunk-size(OCTET)
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
				PrintDebug ("%s(%d) : Error> failed reading chunk-header line(last:%d, chnk:%d/%d)\n",__FUNCTION__,__LINE__, pHttpChunk->b_last_chunk, pHttpChunk->i_left_size, pHttpChunk->i_hexa_size);
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
					PrintDebug("%s(%d) : Error> failed reading chunk-header line\n",__FUNCTION__,__LINE__ );
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

    if( pHttpSys->i_icy_meta > 0 && pAccess->info.i_pos-pHttpSys->i_icy_offset > 0 )
    {
        HINT64 i_next = pHttpSys->i_icy_meta - (pAccess->info.i_pos - pHttpSys->i_icy_offset ) % pHttpSys->i_icy_meta;

        if( i_next == pHttpSys->i_icy_meta )
        {
            if( INT_HTTP_ReadICYMeta( pAccess ) )
            {
#ifdef ICY_PROTOCOL
		PrintDebug ("%s(%d) Error INT_HTTP_ReadICYMeta\n", __FUNCTION__,__LINE__);
#endif

                pAccess->info.b_eof = TRUE;
				PrintDebug ("%s(%d) : stream_read returns for EOF\n",__FUNCTION__,__LINE__);
                PrintExit;
                return 0;
            }
        }

        if( i_len > i_next )
		{
            i_len = i_next;
		}
    }

    i_read = net_Read (pHttpSys->fd, pHttpSys->p_vs, p_buffer, i_len, FALSE );
    if( i_read > 0 )
    {
        PrintDebug ("[%s:%d] i_read:%d pAccess->info.i_pos(%"PRId64"): \n",__FUNCTION__,__LINE__,i_read,pAccess->info.i_pos);
        DumpHttpStream(p_buffer, i_read, (HUINT32)pAccess->info.i_pos);
        pAccess->info.i_pos += i_read;

#ifdef	CONFIG_IPTUNER_XTREMEHSS_LIVE	/* To support UK Freeview MHEG-ICS live streaming contents - C.G. Kang */
		if ( pHttpSys->b_chunked && pHttpSys->b_xtremehss_live == FALSE )
#else
		if ( pHttpSys->b_chunked )
#endif
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
				psz = net_Gets( pHttpSys->fd, pHttpSys->p_vs );
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
        /* For Youtube Rediraction Issue */
        if( pHttpSys->b_continuous )
        {
			for(error_retrycount=0;error_retrycount<HTTP_READ_REDIRECTON_ERROR_RETRYCOUNT;error_retrycount++)
			{
				PrintDebug("##############################################\n");
				PrintDebug("######### [%s] (%d) Critical Section: Retry Connect #######\n", __FUNCTION__,__LINE__);
				PrintDebug("##############################################\n");

			    (void)INT_HTTP_Disconnect ( pAccess );

				if(INT_HTTP_Connect( pAccess, pAccess->info.i_pos ) ==DI_IPT_SUCCESS)
				{
	                i_read = STREAM_HTTP_Read( pAccess, p_buffer, i_len );
	                if(i_read != 0)
	                {
						break;
	                }
					VK_TASK_Sleep(200);
				}
			}
        }

        if( i_read == 0 )
	{
		pAccess->info.b_eof = TRUE;
		PrintDebug ("%s(%d) : stream_read returns for EOF\n",__FUNCTION__,__LINE__);
	}
    }

    if( pAccess->info.i_size != -1 )
    {
        pHttpSys->i_remaining -= i_read;
    }

    PrintExit;
    return i_read;
}


/*****************************************************************************
 * STREAM_HTTP_Seek: close and re-open a connection at the right place
 *****************************************************************************/
int STREAM_HTTP_Seek ( STREAM_ACCESS_t *pAccess, HINT64 i_pos )
{
	int 	error_retrycount =0;

	PrintDebug ("%s(%d) trying to seek to %"PRId64"\n",__FUNCTION__,__LINE__, i_pos );

	if(NULL == pAccess)
	{
		PrintError ("%s(%d) : Error> pAccess is NULL ! \n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}

	(void)INT_HTTP_Disconnect ( pAccess );

	if( pAccess->info.i_size && (HUINT64)i_pos >= (HUINT64)pAccess->info.i_size )
	{
		PrintDebug ( "%s (%d) seek to far\n",__FUNCTION__,__LINE__ );
		int retval = STREAM_HTTP_Seek ( pAccess, pAccess->info.i_size - 1 );
		if( retval == DI_IPT_SUCCESS )
		{
			HUINT8 p_buffer[2];
			int i_read=0;
			i_read = STREAM_HTTP_Read( pAccess, p_buffer, 1);
			if(i_read != 1)
			{
				PrintDebug("######### [%s] Read Fail in STREAM_HTTP_Seek #######\n", __FUNCTION__);
			}
			pAccess->info.b_eof  = FALSE;
		}
		return retval;
	}
	/* else { continue; } */

	if( INT_HTTP_Connect( pAccess, i_pos ) == -2 )
	{
		/* For Youtube Rediraction Issue */
		for(error_retrycount=0;error_retrycount<HTTP_CONNECT_REDIRECTON_ERROR_RETRYCOUNT;error_retrycount++)
		{
				PrintDebug("##############################################\n");
				PrintDebug("######### [%s] Critical Section: Retry Connect #######\n", __FUNCTION__);
				PrintDebug("##############################################\n");

		        (void)INT_HTTP_Disconnect ( pAccess );

				if(INT_HTTP_Connect( pAccess, i_pos ) ==DI_IPT_SUCCESS)
				{
					return DI_IPT_SUCCESS;
				}
				else
				{
					VK_TASK_Sleep(200);
				}
		}
		PrintError ( "%s(%d) seek failed\n",__FUNCTION__,__LINE__ );
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

/*****************************************************************************
 * HTTP_Open:
 *****************************************************************************/
static int INT_HTTP_OpenWithCookies ( STREAM_ACCESS_t *pAccess, char *cookies )
{
	HTTP_ACCESS_SYS_t *pHttpSys=NULL;
	char         *psz=NULL, *p=NULL;
	/* Only forward an store cookies if the corresponding option is activated */
#if SUPPORT_COOKIE
	HBOOL   b_forward_cookies = TRUE; // TODO : 쿠키를 사용하는 경우 고려될 필요 있으나 현재는 FALSE 처리함
#else
	HBOOL   b_forward_cookies = FALSE; // TODO : 쿠키를 사용하는 경우 고려될 필요 있으나 현재는 FALSE 처리함
#endif
	// var_CreateGetBool( pAccess, "http-forward-cookies" );
	char * saved_cookies = (b_forward_cookies ? (cookies) : NULL);
	int 	error_retrycount =0;
	int 	nRet =0;

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

	/* chunked-TE */
	pHttpSys->b_chunked = FALSE; /* Also, Initialization will be done at INT_HTTP_Connect() */
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
#if defined (SUPPORT_TLS_V1_PROTOCOL)
	pHttpSys->p_tls = NULL;
#endif


	/** for ICY Meta Data */
	pHttpSys->i_icy_meta = 0;
	pHttpSys->i_icy_offset = 0;
	pHttpSys->psz_icy_name = NULL;
	pHttpSys->psz_icy_genre = NULL;
	pHttpSys->psz_icy_title = NULL;


	pHttpSys->i_remaining = 0;
	pHttpSys->b_persist = FALSE;
	pHttpSys->i_redirect_count = 0;
	pHttpSys->i_connect_retry_count = 0;

	/** initial info structure */
	pAccess->info.i_size = -1;
	pAccess->info.i_pos  = 0;
	pAccess->info.b_eof  = FALSE;

	pHttpSys->cookies = saved_cookies;

	/** Parse URI - remove spaces */
	if(pAccess->psz_path == NULL)
	{
		goto error;
	}

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

	if( pHttpSys->url.psz_protocol == NULL)
	{
		VK_printf("@@@@@ [%s:%d] ERROR : pHttpSys->url.psz_protocol is NULL\n", __func__, __LINE__);
		VK_printf("@@@@@ [%s:%d] pHttpSys->url.psz_host = %s\n", __func__, __LINE__, pHttpSys->url.psz_host);
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
		pHttpSys->b_ssl = FALSE;
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

	if(s_pcUserAgentInfo != NULL)
	{
#if 0 /* warning: pointer targets in passing argument 1 of 'strdup' differ in signedness */
		pHttpSys->psz_user_agent = strdup (s_pcUserAgentInfo);
#else
		pHttpSys->psz_user_agent = strdup ((char *)s_pcUserAgentInfo);
#endif
	}
	else
	{
		pHttpSys->psz_user_agent = strdup (DI_IPTUNER_USER_AGENT_STRING);
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

connect:
	/** Connect */
	switch ( INT_HTTP_Connect ( pAccess, 0 ) )
	{
		case -1:
			goto error;

		case -2:
#if 0
			/** Retry with http 1.0 */
			PrintError ("%s (%d) switching to HTTP version 1.0\n",__FUNCTION__,__LINE__ );

			pHttpSys->i_version = 0;
			pHttpSys->b_seekable = FALSE;
			/** Try to connecting http1.0 */
#endif
			if( INT_HTTP_Connect( pAccess, 0 ) )
			{
				/* For Youtube Rediraction Issue */
				/* it is not "Redirection Logic" */
				for(error_retrycount=0;error_retrycount<HTTP_CONNECT_ERROR_RETRYCOUNT;error_retrycount++)
				{
					PrintError("##############################################\n");
					PrintError("######### [%s] Critical Section: Retry Connect #######\n", __FUNCTION__);
					PrintError("##############################################\n");
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

				if(error_retrycount >= HTTP_CONNECT_ERROR_RETRYCOUNT)
				{
					goto error;
				}
			}

#ifndef NDEBUG
		case 0:
			break;

		default:
			PrintError ( "%s(%d) : Error> You should not be here!!\n",__FUNCTION__,__LINE__ );
			abort();
#endif
	}

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
#if 0 /* redirection logic(below) is moved to INT_HTTP_Connect */
	if( ( pHttpSys->i_code == HTTP_STATUS_MOVED_PERMANENTLY 			/* 301 */
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

		INT_HTTP_Disconnect ( pAccess );

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

	/** SHOUTcast(ICY Protocol), ICECAST(icecast)*/
	if( !VK_strncmp( pHttpSys->psz_protocol, "ICY" ,sizeof("ICY")) || pHttpSys->b_icecast )
	{
#ifdef ICY_PROTOCOL
		PrintDebug ("%s(%d)  : ICY Protocol is supported.\n",__FUNCTION__,__LINE__);

		pAccess->info.i_size = 0xFFFFFFFFFFFFFF;
		pHttpSys->i_remaining = 0xFFFFFFFFFFFFFF;
		PrintDebug ("%s(%d)  : i_size(%"PRId64")\n",__FUNCTION__,__LINE__, pAccess->info.i_size);
#else
		PrintDebug ("%s(%d)  : Internet Radio(ICY) Protocol is not supported yet.\n",__FUNCTION__,__LINE__);
#endif
		if( pHttpSys->psz_mime && VK_strcasecmp( pHttpSys->psz_mime, "application/ogg" ) )
		{
			if( !VK_strcasecmp( pHttpSys->psz_mime, "video/nsv" ) ||
					!VK_strcasecmp( pHttpSys->psz_mime, "video/nsa" ) )
			{

			}
#ifdef ICY_PROTOCOL
			else if(!VK_strcasecmp( pHttpSys->psz_mime, "audio/aac" ))
			{
				pHttpSys->e_icy_codec = DI_MEDIA_AUDIO_FORMAT_AAC;
			}
			else if(!VK_strcasecmp( pHttpSys->psz_mime, "audio/aacp" ))
			{
				pHttpSys->e_icy_codec = DI_MEDIA_AUDIO_FORMAT_AAC;
			}
#endif

			else if( !VK_strcasecmp( pHttpSys->psz_mime, "audio/mpeg" ) )
			{
#ifdef ICY_PROTOCOL
				pHttpSys->e_icy_codec = DI_MEDIA_AUDIO_FORMAT_MP3;
#endif
			}
		}
		else if( !pHttpSys->psz_mime )
		{

		}
		/* else probably Ogg Vorbis */
	}

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
#ifdef ICY_PROTOCOL
	if(pHttpSys->i_icy_url != NULL)
	{
		DD_MEM_Free ( pHttpSys->i_icy_url );
		pHttpSys->i_icy_url = NULL;
	}
#endif

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

	INT_HTTP_Disconnect ( pAccess );

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
	if(pHttpSys->b_proxy == TRUE)
	{
		VK_MEM_Memcpy(&srv, &pHttpSys->proxy, sizeof(URL_t));
	}
	else
	{
		VK_MEM_Memcpy(&srv, &pHttpSys->url, sizeof(URL_t));
	}

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
				INT_HTTP_Disconnect ( pAccess );
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
				INT_HTTP_Disconnect ( pAccess );
				return -1;
			}

			DumpHttpResponse(psz);

			VK_sscanf( psz, "HTTP/%*u.%*u %3u", &i_status );
			if( psz != NULL )
			{
				DD_MEM_Free ( psz );
				psz = NULL;
			}

			if( ( i_status / 100 ) != 2 )
			{
				PrintError ( "HTTP/TLS tunnel through proxy denied\n" );
				INT_HTTP_Disconnect ( pAccess );
				return -1;
			}

			do
			{
				psz = net_Gets( pHttpSys->fd, NULL );
				if( psz == NULL )
				{
					PrintError ( "HTTP proxy connection failed\n" );
					INT_HTTP_Disconnect ( pAccess );
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
					INT_HTTP_Disconnect ( pAccess );
					return -1;
				}
			}
			while( i_status );
		}

#if defined (SUPPORT_TLS_V1_PROTOCOL)
		/* TLS/SSL handshake */
		pHttpSys->p_tls = tls_ClientCreate (pHttpSys->fd, srv.psz_host);
		if( pHttpSys->p_tls == NULL )
		{
			PrintError ( "%s(%d) Error> cannot establish HTTP/TLS session\n",__FUNCTION__,__LINE__ );
			INT_HTTP_Disconnect ( pAccess );
			return -1;
		}
		pHttpSys->p_vs = &pHttpSys->p_tls->sock;
#endif /* SUPPORT_TLS_V1_PROTOCOL */
	}
	if(INT_HTTP_Request ( pAccess, i_tell ))
	{
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
			if(pHttpSys->psz_location != NULL && pHttpSys->i_redirect_count < HTTP_CONNECT_REDIRECTON_ERROR_RETRYCOUNT)
			{
				PrintDebug ("%s(%d) : redirection to %s\n",__FUNCTION__,__LINE__, pHttpSys->psz_location );
				if(pAccess->psz_path != NULL)
				{
					DD_MEM_Free ( pAccess->psz_path );
					pAccess->psz_path = NULL;
				}
				pAccess->psz_path = VK_StrDup( pHttpSys->psz_location );
				p = psz = strdup( pAccess->psz_path );
				if ( p == NULL || psz == NULL )
				{
					return DI_IPT_EGENERIC;
				}

				while( (p = VK_strchr( p, ' ' )) != NULL )
				{
					*p = '+';
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
					return DI_IPT_EGENERIC;
				}

				(void)INT_HTTP_Disconnect ( pAccess );

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
					/* redirect to HTTP from HTTPS */
					pHttpSys->b_ssl = FALSE;

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
				return -2;
			}
		}
		else if((pAccess->info.i_httpstatecode == HTTP_STATUS_OK
		|| pAccess->info.i_httpstatecode == HTTP_STATUS_PARTIAL_CONTENT)
		&& ( pAccess->info.i_size == -1) && (pHttpSys->b_icecast != TRUE) )
		{
			if(pHttpSys->i_connect_retry_count < HTTP_CONNECT_ERROR_RETRYCOUNT )
			{
				PrintError("There is no Content Length Header - Retrying\n");
				pHttpSys->i_connect_retry_count++;
				(void)INT_HTTP_Disconnect ( pAccess );
				return INT_HTTP_Connect( pAccess, i_tell );
			}
			else
				return -2;
		}
		else
		{
			pHttpSys->i_connect_retry_count = 0;
			pHttpSys->i_redirect_count = 0;
		}
	}
	return 0;
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


static int INT_HTTP_Request ( STREAM_ACCESS_t *pAccess, HINT64 i_tell )
{
	HTTP_ACCESS_SYS_t   *pHttpSys = NULL;
	char    *psz =NULL;
	char 	*p=NULL;
	struct virtual_socket_t     *pvs = NULL;
	HTTP_CHUNK_t *pHttpChunk = NULL;

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
	PrintDebug("%s(%d) pAccess->info.i_size(%"PRId64"): \n",__FUNCTION__,__LINE__,pAccess->info.i_size);

	pHttpSys->b_persist = FALSE;
	pHttpSys->i_remaining = 0;

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

	/* Offset */
	if( pHttpSys->i_version == 1 && ! pHttpSys->b_continuous )
	{
		pHttpSys->b_persist = TRUE;
		PrintDebug ("[%s:%d] i_tell(%"PRId64") \n",__FUNCTION__,__LINE__,i_tell);
		net_Printf( pHttpSys->fd, pvs, "Range: bytes=%"PRIu64"-\r\n", i_tell );
	}

	/* Keep-Alive */
	if(pHttpSys->i_version == 1 && pAccess->b_use_persist == TRUE)
	{
		net_Printf( pHttpSys->fd, pvs, "Connection: keep-alive\r\n");
		pHttpSys->b_persist = TRUE;
		PrintDebug ("%s(%d) Use Persistent Connection %d\n",__FUNCTION__,__LINE__,pHttpSys->fd);
	}

	/* Cookies */
	if( pHttpSys->cookies )
	{
#if SUPPORT_COOKIE
    	net_Printf( pHttpSys->fd, pvs,
                        "Cookie: %s\r\n", pHttpSys->cookies);
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
		INT_HTTP_Disconnect ( pAccess );
		PrintExit;
		return DI_IPT_EGENERIC;
	}

	/* Read Answer */
	if( ( psz = net_Gets( pHttpSys->fd, pvs ) ) == NULL )
	{
		PrintError ( "%s (%d) Error> failed to read answer\n",__FUNCTION__,__LINE__ );
		goto error;
	}

	DumpHttpResponse(psz);

	if( !VK_strncmp( psz, "HTTP/1.", 7 ) )
	{
		pHttpSys->psz_protocol = strdup("HTTP");
		pHttpSys->i_code = VK_atoi( &psz[9] );

		/* we need to backup the answer code for forwarding to upper layer
		 * This code is used to know why is the http connection broken.
		 * So we store the latest answer code in temporary variable
		 */
		s_nLastAnsCode = pHttpSys->i_code ;
	}
	else if( !VK_strncmp( psz, "ICY", 3 ) )
	{
		pHttpSys->psz_protocol = strdup("ICY");
		pHttpSys->i_code = VK_atoi( &psz[4] );
		pHttpSys->b_reconnect = TRUE;
		/* we need to backup the answer code for forwarding to upper layer
		 * This code is used to know why is the http connection broken.
		 * So we store the latest answer code in temporary variable
		 */
		s_nLastAnsCode = pHttpSys->i_code ;
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

	PrintDebug ("%s (%d) protocol '%s' answer code %d\n",__FUNCTION__,__LINE__, pHttpSys->psz_protocol, pHttpSys->i_code );

	if( !VK_strcmp( pHttpSys->psz_protocol, "ICY" ) )
	{
		pHttpSys->b_seekable = FALSE;
	}
	/* else { continue ; } */
	if( pHttpSys->i_code != HTTP_STATUS_PARTIAL_CONTENT	/* 206 */
			&& pHttpSys->i_code != HTTP_STATUS_UNAUTHORIZED )	/* 401 */
	{
		pHttpSys->b_seekable = FALSE;
	}
	/* else { continue ; } */

	/* Authentication error - We'll have to display the dialog */
	if( pHttpSys->i_code == HTTP_STATUS_UNAUTHORIZED ) /* 401 */
	{
		PrintError ("%s (%d) Error> Authentication Erro\n",__FUNCTION__,__LINE__);

	}
	/* Other fatal error */
	else if( pHttpSys->i_code >= HTTP_STATUS_BAD_REQUEST ) /* 400 */
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
		psz = net_Gets( pHttpSys->fd, pvs );

		if( psz == NULL )
		{
			PrintError ( "%s (%d) Error> failed to read answer\n",__FUNCTION__,__LINE__ );
			goto error;
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

		/* PrintDebug ("Line=%s\n", psz ); */
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

		while( *p == ' ' )
		{
			p++;
		}

		if( !VK_strcasecmp( psz, "Content-Length" ) )
		{
			HINT64 i_size = i_tell + (pHttpSys->i_remaining = atoll( p ));
			if(i_size > pAccess->info.i_size)
			{
				pHttpSys->b_has_size = TRUE;
				pAccess->info.i_size = i_size;
			}
			PrintDebug ("(%s)(%d) this frame size=%"PRId64"\n",__FUNCTION__,__LINE__, pHttpSys->i_remaining );
		}
		else if( !VK_strcasecmp( psz, "Accept-Ranges" ) )
		{
			/*  Check whether the range is unacceptable or not */
			/* [Accept-Ranges: bytes] 인 chunked 도 존재함, redmine #73082 by dhkim2 */
			if( !VK_strncasecmp(p, "none", 4) )
			{
				/* 'bytes' is expected, not 'none' response */
				pHttpSys->b_range_accepted = FALSE;
			}
			pHttpSys->b_range_response = TRUE;
		}
		else if( !VK_strcasecmp( psz, "Content-Range" ) )
		{
			/* Response for Content-Range */
			/* e.g) Content-Range: bytes 10-20/13244 */
			HINT64 i_ntell = i_tell;
			HINT64 i_nend = (pAccess->info.i_size > 0)?(pAccess->info.i_size - 1):i_tell;
			HINT64 i_nsize = pAccess->info.i_size;
			HINT64 i_size=0;

#if defined(CONFIG_MHEG_SUPPORT) && defined(CONFIG_ICS)
			if(VK_strcasecmp(p, "bytes */*"))
			{
#endif
				VK_sscanf(p,"bytes %"PRId64"-%"PRId64"/%"PRId64,&i_ntell,&i_nend,&i_nsize);

				if(i_nend > i_ntell )
				{
					PrintDebug ("[%s:%d] i_ntell(%"PRId64") pAccess->info.i_pos(%"PRId64"): \n",__FUNCTION__,__LINE__,i_ntell,pAccess->info.i_pos);
					pHttpSys->i_remaining = i_nend+1-i_ntell;
					pAccess->info.i_pos = i_ntell;
					i_size = (i_nsize > i_nend) ? i_nsize : (i_nend + 1);

					if(i_size > pAccess->info.i_size)
					{
						/*
						 * Update file_size if there is no content-length like chunked TE
						 * Also, update the file_size if current size value is bigger than prev file_size
						 */
						pAccess->info.i_size = i_size;
					}
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
#if defined(CONFIG_MHEG_SUPPORT) && defined(CONFIG_ICS)
			}
#endif
		}
		else if( !VK_strcasecmp( psz, "Connection" ) )
		{
			int i = -1;
			PrintDebug ( "%s(%d) Connection: %s\n",__FUNCTION__,__LINE__,p );

			VK_sscanf(p, "close%n",&i);

			if( i >= 0 )
			{
				pHttpSys->b_persist = FALSE;
			}
		}
		else if( !VK_strcasecmp( psz, "Location" ) )
		{
			char * psz_new_loc=NULL;
			const char *psz_http_ext=NULL;

			if( p[0] == '/' )
			{
				psz_http_ext = pHttpSys->b_ssl ? "s" : "" ;

				if( pHttpSys->url.i_port == ( pHttpSys->b_ssl ? 443 : 80 ) )
				{
					if( asprintf(&psz_new_loc, "http%s://%s%s", psz_http_ext, pHttpSys->url.psz_host, p) < 0 )
					{
						goto error;
				}
				}
				else
				{
					if( asprintf(&psz_new_loc, "http%s://%s:%d%s", psz_http_ext, pHttpSys->url.psz_host, pHttpSys->url.i_port, p) < 0 )
					{
						goto error;
				}
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
		else if( !VK_strcasecmp( psz, "Content-Type" ) )
		{
			if(pHttpSys->psz_mime != NULL)
			{
				VK_free ( pHttpSys->psz_mime );
				pHttpSys->psz_mime = NULL;
			}

			if( !VK_strcasecmp(p, "audio/mpeg") )
			{
				pHttpSys->e_icy_codec = DI_MEDIA_AUDIO_FORMAT_MP3;
			}
			else if( !VK_strcasecmp(p, "audio/aac") )
			{
				pHttpSys->e_icy_codec = DI_MEDIA_AUDIO_FORMAT_AAC;
			}

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
			}
#ifdef	CONFIG_IPTUNER_XTREMEHSS_LIVE	/* To support UK Freeview MHEG-ICS live streaming contents - C.G. Kang */
			else if( !VK_strncasecmp( p, "XtremeHSS", 9 ) )
			{
				pHttpSys->b_xtremehss_live = TRUE;
			}
#endif
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

			pHttpSys->psz_icy_name = UTIL_EnsureUTF8( strdup( p ));
			PrintDebug ( "%s(%d) Icy-Name: %s\n",__FUNCTION__,__LINE__, pHttpSys->psz_icy_name );

			pHttpSys->b_icecast = TRUE; /* be on the safeside. set it here as well. */
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

			pHttpSys->psz_icy_genre = UTIL_EnsureUTF8( strdup( p ));
			PrintDebug ( "%s(%d) Icy-Genre: %s\n",__FUNCTION__,__LINE__, pHttpSys->psz_icy_genre );
		}
		else if( !VK_strncasecmp( psz, "Icy-Notice", 10 ) )
		{
			PrintDebug ( "%s(%d) Icy-Notice: %s\n",__FUNCTION__,__LINE__, p );
		}
#ifdef ICY_PROTOCOL
		else if( !VK_strncasecmp( psz, "Icy-Url", 7 ) )
		{
			if(pHttpSys->i_icy_url != NULL)
			{
				DD_MEM_Free ( pHttpSys->i_icy_url);
				pHttpSys->i_icy_url = NULL;
			}

			pHttpSys->i_icy_url = UTIL_EnsureUTF8( strdup( p ));
			PrintDebug ( "%s(%d) Icy-Url: %s\n",__FUNCTION__,__LINE__, pHttpSys->i_icy_url );
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
#endif
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
			{
				goto error;
			}
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

#ifdef	CONFIG_IPTUNER_XTREMEHSS_LIVE	/* To support UK Freeview MHEG-ICS live streaming contents - C.G. Kang */
	if (pHttpSys->b_xtremehss_live)
	{
		/* double-check! */
		if ((pHttpSys->i_remaining >= 999000000) && pHttpSys->psz_mime && VK_strstr(pHttpSys->psz_mime, "MP2T"))
		{
			pHttpSys->b_chunked = TRUE;
			pHttpSys->b_range_response = FALSE;
			INT_HTTP_PrepareChunking(pHttpSys);
		}
		else
		{
			pHttpSys->b_xtremehss_live = FALSE;
		}
	}
#endif

	if (pHttpSys->b_chunked == TRUE)
	{
		if (pHttpSys->b_range_response == FALSE)
		{
			pAccess->info.i_size = 0xFFFFFFFFFFFFFF;
			pHttpSys->i_remaining = 0xFFFFFFFFFFFFFF;
		}

		/* If it is chunked_livestreaming and has [Accept-Ranges: bytes] response */
		if ((pHttpSys->b_range_response == TRUE) && (pHttpSys->b_has_size == FALSE))
		{
			pAccess->info.i_size = 0xFFFFFFFFFFFFFF;
			pHttpSys->i_remaining = 0xFFFFFFFFFFFFFF;
		}
	}

	/* We close the stream for zero length data, unless of course the
	 * server has already promised to do this for us.
	 */
	if( pAccess->info.i_size != -1 && pHttpSys->i_remaining == 0 && pHttpSys->b_persist )
	{
		INT_HTTP_Disconnect ( pAccess );
	}

	/* 이 부분은 ICY Protocol을 제외한 Http Protocol에서 Size없이 무제한으로 Play되어야 하는 경우 문제가 됨.(Streaming) */
	if(( pAccess->info.i_size == -1) && (pHttpSys->b_icecast != TRUE))
	{
		/* eth link down up 시 네트워크에서 ㄷ더돌던 HTTP 200 OK 가 들어오는 경우가 잇다. */
		PrintDebug("There is no Content Length Header\n");
#if 1 /* broadcastng이거나 redirection인 경우 filesize가 0 or -1이어도 error로 리턴되면 안됨 */
		//   		goto error;
#else
		goto error;
#endif
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
	INT_HTTP_Disconnect ( pAccess );
	PrintExit;
	return DI_IPT_EGENERIC;
}

/*****************************************************************************
 * INT_HTTP_Disconnect :
 *****************************************************************************/
static void INT_HTTP_Disconnect ( STREAM_ACCESS_t *pAccess )
{
    HTTP_ACCESS_SYS_t *pHttpSys = NULL;

	PrintEnter;

    if(pAccess == NULL)
    {
        PrintError ( "%s (%d) Error> invalid pAccess Handle (NULL)\n",__FUNCTION__,__LINE__ );
		PrintExit;
    	return; /* DI_IPT_EGENERIC; */
    }

	if(pAccess->p_sys == NULL)
	{
		PrintError ("%s (%d) Error> pAccess->p_sys is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return; /* DI_IPT_EGENERIC; */
	}

    pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;

#if defined (SUPPORT_TLS_V1_PROTOCOL)
	if( pHttpSys->p_tls != NULL)
    {
        tls_ClientDelete( pHttpSys->p_tls );
        pHttpSys->p_tls = NULL;
        pHttpSys->p_vs = NULL;
    }
#endif

    if( pHttpSys->fd != -1)
    {
        net_Close(pHttpSys->fd);
        pHttpSys->fd = -1;
    }

	PrintExit;
	return;
}

/*****************************************************************************
 * "RFC 2617: Basic and Digest Access Authentication" header parsing
 *****************************************************************************/
static char *INT_HTTP_AuthGetParm ( const char *psz_header, const char *psz_param )
{
    char *psz_what=NULL;
	int nRet;
	int nSize;

#if 0
	nSize = VK_strlen(psz_param);
#else
	nSize = VK_strlen(psz_param);
#endif
	psz_what = (char*) DD_MEM_Alloc (nSize+3);
	if(psz_what == NULL)
	{
		return NULL;
	}

	VK_memset(psz_what,0,nSize+3);

    nRet = VK_snprintf( psz_what, nSize+3, "%s=\"", psz_param );
	if (nRet<0)
	{
		DD_MEM_Free(psz_what);
		return NULL;
	}

    psz_header = VK_strstr( psz_header, psz_what );

    if( psz_header )
    {
        const char *psz_end;
        psz_header += VK_strlen ( psz_what );
        psz_end = VK_strchr( psz_header, '"' );
        if( !psz_end ) /* Invalid since we should have a closing quote */
		{
			DD_MEM_Free(psz_what);
            return strdup( psz_header );
		}
		DD_MEM_Free(psz_what);
        return strndup ( psz_header, psz_end - psz_header );
    }
    else
    {
		DD_MEM_Free(psz_what);
        return NULL;
    }
}

static char *INT_HTTP_AuthGetParamNoQuotes ( const char *psz_header, const char *psz_param )
{
    char *psz_what;
	int nRet;
	int nSize;

#if 0 /* warning: cast discards qualifiers from pointer target type */
	nSize = VK_strlen (psz_param);
#else
	nSize = VK_strlen (psz_param);
#endif

	psz_what = (char*) DD_MEM_Alloc (nSize+2);
	if(psz_what == NULL)
	{
		return NULL;
	}

	VK_memset(psz_what,0,nSize+2);

    nRet = VK_snprintf( psz_what, nSize+2, "%s=\"", psz_param );
	if (nRet<0)
	{
		DD_MEM_Free(psz_what);
		return NULL;
	}

    psz_header = VK_strstr( psz_header, psz_what );
    if( psz_header )
    {
        const char *psz_end;
        psz_header += VK_strlen ( psz_what );
        psz_end = VK_strchr( psz_header, ',' );
        /* XXX: Do we need to filter out trailing space between the value and
         * the comma/end of line? */
        if( !psz_end ) /* Can be valid if this is the last parameter */
		{
			DD_MEM_Free(psz_what);
            return strdup( psz_header );
		}
		DD_MEM_Free(psz_what);
        return strndup( psz_header, psz_end - psz_header );
    }
    else
    {
		DD_MEM_Free(psz_what);
        return NULL;
    }
}

static void INT_HTTP_AuthParseHeader( STREAM_ACCESS_t *pAccess, const char *psz_header, HTTP_AUTH_t *p_auth )
{
	char *psz_tmp=NULL;

	UNUSED_PARAM(pAccess);

    /* 2 Basic Authentication Scheme */
    if( !VK_strncasecmp( psz_header, "Basic ", VK_strlen ( "Basic " ) ) )
    {
        psz_header += VK_strlen ( "Basic " );
        p_auth->psz_realm = INT_HTTP_AuthGetParm ( psz_header, "realm" );
        if( !p_auth->psz_realm )
		{
            PrintError ( "Basic Authentication: \n" "Mandatory 'realm' parameter is missing\n" );
		}
    }
    /* 3 Digest Access Authentication Scheme */
    else if( !VK_strncasecmp( psz_header, "Digest ", VK_strlen ( "Digest " ) ) )
    {
        if( p_auth->psz_nonce )
		{
			return;
        }
        psz_header += VK_strlen ( "Digest " );
        p_auth->psz_realm = INT_HTTP_AuthGetParm ( psz_header, "realm" );
        p_auth->psz_domain = INT_HTTP_AuthGetParm ( psz_header, "domain" );
        p_auth->psz_nonce = INT_HTTP_AuthGetParm ( psz_header, "nonce" );
        p_auth->psz_opaque = INT_HTTP_AuthGetParm ( psz_header, "opaque" );
        p_auth->psz_stale = INT_HTTP_AuthGetParamNoQuotes ( psz_header, "stale" );
        p_auth->psz_algorithm = INT_HTTP_AuthGetParamNoQuotes ( psz_header, "algorithm" );
        p_auth->psz_qop = INT_HTTP_AuthGetParm ( psz_header, "qop" );
        p_auth->i_nonce = 0;

        if( !p_auth->psz_realm )
		{
            PrintError ( "Digest Access Authentication: \n" "Mandatory 'realm' parameter is missing\n" );
		}
        if( !p_auth->psz_nonce )
		{
            PrintError ( "Digest Access Authentication: \n" "Mandatory 'nonce' parameter is missing" );
		}

        if( p_auth->psz_qop )
        {
            psz_tmp = VK_strchr( p_auth->psz_qop, ',' );
            if( psz_tmp != NULL )
        {
				*psz_tmp = '\0';
            }
        }
    }
    else
    {
        const char *psz_end = VK_strchr( psz_header, ' ' );
        if( psz_end )
		{
            PrintError ( "Unknown authentication scheme: '%*s'\n", psz_end - psz_header, psz_header );
		}
        else
		{
            PrintError ( "Unknown authentication scheme: '%s'\n", psz_header );
		}
    }
}

static char *INT_HTTP_AuthDigest ( STREAM_ACCESS_t *pAccess, URL_t *p_url,
                         HTTP_AUTH_t *p_auth, const char *psz_method )
{
	(void)pAccess;
	const char *psz_username = p_url->psz_username ?p_url->psz_username: "";
	const char *psz_password = p_url->psz_password ?p_url->psz_password: "";

	char *psz_HA1 = NULL;
	char *psz_HA2 = NULL;
	char *psz_response = NULL;
	struct md5_s md5;

	/* H(A1) */
	if( p_auth->psz_HA1 )
	{
		psz_HA1 = strdup( p_auth->psz_HA1 );
		if( !psz_HA1 ) goto error;
	}
	else
	{
		HUMAX_InitMD5( &md5 );
#if 0 /* warning: cast discards qualifiers from pointer target type */
		HUMAX_AddMD5( &md5, psz_username, VK_strlen ( psz_username ) );
#else
		HUMAX_AddMD5( &md5, psz_username, VK_strlen ( psz_username ) );
#endif
		HUMAX_AddMD5( &md5, ":", 1 );
		HUMAX_AddMD5( &md5, p_auth->psz_realm, VK_strlen ( p_auth->psz_realm ) );
		HUMAX_AddMD5( &md5, ":", 1 );
#if 0 /* warning: cast discards qualifiers from pointer target type */
		AddMD5( &md5, psz_password, VK_strlen ( psz_password ) );
#else
		HUMAX_AddMD5( &md5, psz_password, VK_strlen ( psz_password ) );
#endif
		HUMAX_EndMD5( &md5 );

		psz_HA1 = psz_md5_hash( &md5 );
		if( !psz_HA1 ) goto error;

		if( p_auth->psz_algorithm
				&& !VK_strcmp( p_auth->psz_algorithm, "MD5-sess" ) )
		{
			HUMAX_InitMD5( &md5 );
			HUMAX_AddMD5( &md5, psz_HA1, 32 );
			if(psz_HA1 != NULL)
			{
				DD_MEM_Free ( psz_HA1 );
				psz_HA1 = NULL;
			}

			HUMAX_AddMD5( &md5, ":", 1 );
			HUMAX_AddMD5( &md5, p_auth->psz_nonce, VK_strlen ( p_auth->psz_nonce ) );
			HUMAX_AddMD5( &md5, ":", 1 );
			HUMAX_AddMD5( &md5, p_auth->psz_cnonce, VK_strlen ( p_auth->psz_cnonce ) );
			HUMAX_EndMD5( &md5 );

			psz_HA1 = psz_md5_hash( &md5 );
			if( !psz_HA1 ) goto error;
			p_auth->psz_HA1 = strdup( psz_HA1 );
			if( !p_auth->psz_HA1 ) goto error;
		}
	}

	/* H(A2) */
	HUMAX_InitMD5( &md5 );
	if( *psz_method )
	{
#if 0
		AddMD5( &md5, psz_method, VK_strlen ( psz_method ) );
#else
		HUMAX_AddMD5( &md5, psz_method, VK_strlen ( psz_method ) );
#endif
	}
	HUMAX_AddMD5( &md5, ":", 1 );
	if( p_url->psz_path )
	{
		HUMAX_AddMD5( &md5, p_url->psz_path, VK_strlen ( p_url->psz_path ) );
	}
	else
	{
		HUMAX_AddMD5( &md5, "/", 1 );
	}

	if( p_auth->psz_qop && !VK_strcmp( p_auth->psz_qop, "auth-int" ) )
	{
		char *psz_ent;
		struct md5_s ent;
		HUMAX_InitMD5( &ent );
		HUMAX_AddMD5( &ent, "", 0 ); /* XXX: entity-body. should be ok for GET */
		HUMAX_EndMD5( &ent );
		psz_ent = psz_md5_hash( &ent );
		if( !psz_ent ) goto error;
		HUMAX_AddMD5( &md5, ":", 1 );
		HUMAX_AddMD5( &md5, psz_ent, 32 );
		if(psz_ent != NULL)
		{
			DD_MEM_Free ( psz_ent );
			psz_ent = NULL;
		}
	}
	HUMAX_EndMD5( &md5 );
	psz_HA2 = psz_md5_hash( &md5 );
	if( !psz_HA2 ) goto error;

	/* Request digest */
	HUMAX_InitMD5( &md5 );
	HUMAX_AddMD5( &md5, psz_HA1, 32 );
	HUMAX_AddMD5( &md5, ":", 1 );
	HUMAX_AddMD5( &md5, p_auth->psz_nonce, VK_strlen ( p_auth->psz_nonce ) );
	HUMAX_AddMD5( &md5, ":", 1 );
	if( p_auth->psz_qop
			&& ( !VK_strcmp( p_auth->psz_qop, "auth" )
				|| !VK_strcmp( p_auth->psz_qop, "auth-int" ) ) )
	{
		char psz_inonce[9];
		VK_snprintf( psz_inonce, 9, "%08x", p_auth->i_nonce );
		HUMAX_AddMD5( &md5, psz_inonce, 8 );
		HUMAX_AddMD5( &md5, ":", 1 );
		HUMAX_AddMD5( &md5, p_auth->psz_cnonce, VK_strlen ( p_auth->psz_cnonce ) );
		HUMAX_AddMD5( &md5, ":", 1 );
		HUMAX_AddMD5( &md5, p_auth->psz_qop, VK_strlen ( p_auth->psz_qop ) );
		HUMAX_AddMD5( &md5, ":", 1 );
	}
	HUMAX_AddMD5( &md5, psz_HA2, 32 );
	HUMAX_EndMD5( &md5 );
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
	char *pszResponse=NULL;
    HTTP_ACCESS_SYS_t *pHttpSys = pAccess->p_sys;
    struct virtual_socket_t     *pvs = pHttpSys->p_vs;

    const char *psz_username = p_url->psz_username ?p_url->psz_username: "";
    const char *psz_password = p_url->psz_password ?p_url->psz_password: "";

	PrintEnter;
	if(pAccess->p_sys == NULL)
	{
		PrintError("%s(%d) Error: pAccess->p_sys is already NULL!\n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}

    if(pstAuth->psz_nonce == NULL)
    {
        char *pcB64=NULL;
        char buf[VK_strlen( psz_username ) + VK_strlen( psz_password ) + 2];

		VK_snprintf( buf, sizeof( buf ), "%s:%s", psz_username, psz_password );

        pcB64 = UTIL_B64Encode ((const char*)buf );
        if( pcB64 != NULL )
        {
             net_Printf( pHttpSys->fd, pvs, "%sAuthorization: Basic %s\r\n", psz_prefix, pcB64 );
            if(pcB64 != NULL)
    {
            	DD_MEM_Free ( pcB64 );
            	pcB64 = NULL;
            }
        }

    }
    else
    {
    	if(    pstAuth->psz_algorithm
            && VK_strcmp( pstAuth->psz_algorithm, "MD5" )
            && VK_strcmp( pstAuth->psz_algorithm, "MD5-sess" ) )
        {
            PrintError ( "Digest Access Authentication: \n" "Unknown algorithm '%s'\n", pstAuth->psz_algorithm );
            return;
        }

        if( (pstAuth->psz_qop != NULL) ||
			(pstAuth->psz_cnonce == NULL) )
        {
            DD_MEM_Free ( pstAuth->psz_cnonce );
            pstAuth->psz_cnonce = strdup( "Some random string FIXME" );
        }

        pstAuth->i_nonce++;

        pszResponse = INT_HTTP_AuthDigest ( pAccess, p_url, pstAuth, "GET" );
        if( pszResponse == NULL)
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
                    psz_prefix, psz_username, pstAuth->psz_realm, pstAuth->psz_nonce, p_url->psz_path ?p_url->psz_path: "/",
                    pszResponse, pstAuth->psz_algorithm ? "algorithm=\"" : "", pstAuth->psz_algorithm ?pstAuth->psz_algorithm: "",
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

		if(pszResponse != NULL)
		{
	        DD_MEM_Free ( pszResponse );
	        pszResponse = NULL;
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
        DD_MEM_Free ( psz_nextnonce );
        DD_MEM_Free ( psz_qop );
        DD_MEM_Free ( psz_rspauth );
        DD_MEM_Free ( psz_cnonce );
        DD_MEM_Free ( psz_nc );

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

static int INT_HTTP_ReadICYMeta( STREAM_ACCESS_t *pAccess )
{
    HTTP_ACCESS_SYS_t *pHttpSys = (HTTP_ACCESS_SYS_t *) pAccess->p_sys;

    HUINT8 buffer=0;
    char *p=NULL;
    char *psz_meta=NULL;
    int i_read=0;

	if(pAccess->p_sys == NULL)
	{
		PrintError("%s(%d) Error: pAccess->p_sys is already NULL!\n",__FUNCTION__,__LINE__);
		PrintExit;
        return DI_IPT_EGENERIC;
	}

    /* STREAM_HTTP_Read meta data length */
    i_read = net_Read (pHttpSys->fd, pHttpSys->p_vs, &buffer, 1, TRUE );
    if( i_read <= 0 )
	{
        return DI_IPT_EGENERIC;
	}

    if( buffer == 0 )
	{
        return DI_IPT_SUCCESS;
	}

    i_read = buffer << 4;
    /* PrintDebug ("ICY meta size=%u\n", i_read); */

    psz_meta = DD_MEM_Alloc ( i_read + 1 );
    if( psz_meta == NULL )
	{
        return DI_IPT_EGENERIC;
	}

    if( net_Read( pHttpSys->fd, pHttpSys->p_vs, (HUINT8 *)psz_meta, i_read, TRUE ) != i_read )
	{
        return DI_IPT_EGENERIC;
	}

    psz_meta[i_read] = '\0'; /* Just in case */

    /* PrintDebug ("icy-meta=%s\n", psz_meta ); */

    /* Now parse the meta */
    /* Look for StreamTitle= */
    p = strcasestr ( psz_meta, "StreamTitle=" );

    if( p )
    {
        p += VK_strlen  ( "StreamTitle=" );
        if( *p == '\'' || *p == '"' )
        {
            char closing[] = { p[0], ';', '\0' };
            char *psz = VK_strstr( &p[1], closing );
            if( !psz )
			{
                psz = VK_strchr( &p[1], ';' );
			}

            if( psz )
			{
				*psz = '\0';
			}
        }
        else
        {
            char *psz = VK_strchr( &p[1], ';' );
            if( psz )
			{
				*psz = '\0';
			}
        }

        if( !pHttpSys->psz_icy_title || VK_strcmp( pHttpSys->psz_icy_title, &p[1] ) )
        {
            DD_MEM_Free ( pHttpSys->psz_icy_title );
            pHttpSys->psz_icy_title = UTIL_EnsureUTF8( strdup( &p[1] ));
#if 0 // for meta
            pAccess->info.i_update |= INPUT_UPDATE_META;
#endif
            PrintDebug ("New Title=%s\n", pHttpSys->psz_icy_title );
        }
    }

    if(psz_meta != NULL)
    {
	    DD_MEM_Free ( psz_meta );
	    psz_meta = NULL;
    }

    return DI_IPT_SUCCESS;
}

void STREAM_HTTP_GetProtoStatusCode (HINT32 *pHttpCode)
{
	*pHttpCode = s_nLastAnsCode;

	return;
}

void STREAM_HTTP_SetUserAgentInfo (HINT8 *pUserAgentInfo)
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

