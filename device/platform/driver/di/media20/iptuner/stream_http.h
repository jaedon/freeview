/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_iptuner.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  Apr 17 03:08:21 KST 2009 $
 * File Description:	     HTTP Stream Controller Header
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __STREAM_HTTP_H__
#define __STREAM_HTTP_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "di_iptuner_config.h"
#include "di_iptuner20.h"

#include "htype.h"
#include "transport_io.h"
#include "util_url.h"

#include "di_iptuner_common.h"
#include "di_media20.h"

#include "transport_tls.h"
#include "stream_global.h"

#include <unistd.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
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
/** Below MACRO is for chunked hexa value check
 */
#define _DEF_HEX_CR      0x0d /* HTTP CR/LF */
#define _DEF_HEX_LF      0x0a /* unused because net_Gets() read to '\' */
#define _DEF_HEX_NUM_0   0x30 /* number: 0 to 9 */
#define _DEF_HEX_NUM_9   0x39
#define _DEF_HEX_UPP_A   0x41 /* upper case: A to F */
#define _DEF_HEX_UPP_F   0x46
#define _DEF_HEX_LOW_A   0x61 /* lower case: a to f */
#define _DEF_HEX_LOW_F   0x66


/** Below MACRO is HTTP Status-Code in HTTP1.1.
      - 1xx: Informational - Request received, continuing process
      - 2xx: Success - The action was successfully received,
        understood, and accepted
      - 3xx: Redirection - Further action must be taken in order to
        complete the request
      - 4xx: Client Error - The request contains bad syntax or cannot
        be fulfilled
      - 5xx: Server Error - The server failed to fulfill an apparently
        valid request
*/
/** See also http://www.w3.org/Protocols/rfc2616/ */

typedef enum
{
	HTTP_STATUS_CONTINUE						=	100 , /* Section 10.1.1: Continue */
	HTTP_STATUS_SWITCHING						=	101 , /* Section 10.1.2: Switching Protocols */
	HTTP_STATUS_OK								=	200 , /* Section 10.2.1: OK */
	HTTP_STATUS_CREATED							=	201 , /* Section 10.2.2: Created */
	HTTP_STATUS_ACCEPTED						=	202 , /* Section 10.2.3: Accepted */
	HTTP_STATUS_NO_AUTH_INFO					=	203 , /* Section 10.2.4: Non-Authoritative Information */
	HTTP_STATUS_NO_CONTENT						=	204 , /* Section 10.2.5: No Content */
	HTTP_STATUS_RESET_CONTENT					=	205 , /* Section 10.2.6: Reset Content */
	HTTP_STATUS_PARTIAL_CONTENT					=	206 , /* Section 10.2.7: Partial Content */
	HTTP_STATUS_MULTIPLE_CHOICE					=	300 , /* Section 10.3.1: Multiple Choices */
	HTTP_STATUS_MOVED_PERMANENTLY				=	301 , /* Section 10.3.2: Moved Permanently */
	HTTP_STATUS_FOUND							=	302 , /* Section 10.3.3: Found */
	HTTP_STATUS_SEE_OTHER						=	303 , /* Section 10.3.4: See Other */
	HTTP_STATUS_NOT_MODIFIED					=	304 , /* Section 10.3.5: Not Modified */
	HTTP_STATUS_USE_PROXY						=	305 , /* Section 10.3.6: Use Proxy */
	HTTP_STATUS_UNUSED							=	306 , /* Section 10.3.7: Unused field */
	HTTP_STATUS_TEMP_REDIRECT					=	307 , /* Section 10.3.8: Temporary Redirect */
	HTTP_STATUS_BAD_REQUEST						=	400 , /* Section 10.4.1: Bad Request */
	HTTP_STATUS_UNAUTHORIZED					=	401 , /* Section 10.4.2: Unauthorized */
	HTTP_STATUS_PAYMENT_REQUIRED				=	402 , /* Section 10.4.3: Payment Required */
	HTTP_STATUS_FORBIDDEN						=	403 , /* Section 10.4.4: Forbidden */
	HTTP_STATUS_NOT_FOUND						=	404 , /* Section 10.4.5: Not Found */
	HTTP_STATUS_METHOD_NOT_ALLOWED				=	405 , /* Section 10.4.6: Method Not Allowed */
	HTTP_STATUS_NOT_ACCEPTABLE					=	406 , /* Section 10.4.7: Not Acceptable */
	HTTP_STATUS_PROXY_AUTH_REQUIRED				=	407 , /* Section 10.4.8: Proxy Authentication Required */
	HTTP_STATUS_REQUEST_TIMEOUT					=	408 , /* Section 10.4.9: Request Time-out */
	HTTP_STATUS_CONFILICT						=	409 , /* Section 10.4.10: Conflict */
	HTTP_STATUS_GONE							=	410 , /* Section 10.4.11: Gone */
	HTTP_STATUS_LENGTH_REQUIRED					=	411 , /* Section 10.4.12: Length Required */
	HTTP_STATUS_PRECONDITION_FAILED				=	412 , /* Section 10.4.13: Precondition Failed */
	HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE		=   413 , /* Section 10.4.14: Request Entity Too Large */
	HTTP_STATUS_REQUEST_URI_TOO_LARGE			=   414 , /* Section 10.4.15: Request-URI Too Large */
	HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE			=   415 , /* Section 10.4.16: Unsupported Media Type */
	HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE = 	416 , /* Section 10.4.17: Requested range not satisfiable */
	HTTP_STATUS_EXPECTATION_FAILED				=   417 , /* Section 10.4.18: Expectation Failed */
	HTTP_STATUS_INTERNAL_SERVER_ERROR			=   500 , /* Section 10.5.1: Internal Server Error */
	HTTP_STATUS_NOT_IMPLEMENTED					=   501 , /* Section 10.5.2: Not Implemented */
	HTTP_STATUS_BAD_GATEWAY						=   502 , /* Section 10.5.3: Bad Gateway */
	HTTP_STATUS_SERVICE_UNAVAILABLE				=   503 , /* Section 10.5.4: Service Unavailable */
	HTTP_STATUS_GATEWAY_TIMEOUT					=   504 , /* Section 10.5.5: Gateway Time-out */
	HTTP_STATUS_VERSION_NOT_SUPPORTED			=   505 , /* Section 10.5.6: HTTP Version not supported */
	HTTP_STATUS_MHEG_UNKNOWN					=   622 /* Mheg Specific */
} HTTP_STATUS_CODE ;

/*
 * HTTP/1.1 transfer/content-coding; But unsupported.
 * Just mark whether unexpected encoding type is used or not
 * when receiving 'Transfer-Encoding' or 'Content-Encoding' field.
 */
typedef enum
{
	/*
	 * Reference Encoding (RFC-2616, section 3.6/14.3)
 	 *   + Encoding: compress, gzip
	 *   + Encoding:
	 *   + Encoding: *
	 *   + Encoding: compress;q=0.5, gzip;q=1.0
	 *   + Encoding: gzip;q=1.0, identity; q=0.5, *;q=0
	 */
	HTTP_ENCODING_IDENTITY=0,  /* default: identity (no encoding) */
	HTTP_ENCODING_DEFLATE,     /* unsupported; deflate(zlib); RFC-1950/1951 */
	HTTP_ENCODING_GZIP,        /* unsupported; gzip; RFC-1952 */
	HTTP_ENCODING_XGZIP,       /* unsupported; x-gzip; RFC-1952 */
	HTTP_ENCODING_COMPRESS,    /* unsupported; compress */
	HTTP_ENCODING_XCOMPRESS    /* unsupported; x-compress */
} HTTP_ENCODING_TYPE_e;

typedef enum
{
	HTTP_CHUNK_STATE_NA=0,     /* default: N/A */
	HTTP_CHUNK_STATE_START,    /* started */
	HTTP_CHUNK_STATE_RECV,     /* stream receiving state after chunk hex */
	HTTP_CHUNK_STATE_LAST,     /* last-chunk --> eof */
	HTTP_CHUNK_STATE_TRAILER   /* if trailer is existed */
} HTTP_CHUNK_STATE_e;
/* End Macro definition */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/* RFC 2617: Basic and Digest Access Authentication */
typedef struct {
    char *psz_realm;
    char *psz_domain;
    char *psz_nonce;
    char *psz_opaque;
    char *psz_stale;
    char *psz_algorithm;
    char *psz_qop;
    int i_nonce;
    char *psz_cnonce;
    char *psz_HA1; /* stored H(A1) value if algorithm = "MD5-sess" */
} HTTP_AUTH_t;

typedef struct {
	HBOOL 	b_last_chunk;     /* got last-chunk;received zero hexa value */
	HBOOL 	b_trailer;        /* default false; existed chunk trailer; no meaning at streaming, not cgi */
	HINT64  i_hexa_size;      /* each chunk size */
	HINT64  i_left_size;      /* remaining chunk size */

	HTTP_CHUNK_STATE_e e_state;
	HTTP_ENCODING_TYPE_e encoding_type; /* dbg: '' or 'identity' is expected; deflate/gzip is unsupported */
	HUINT64  i_chunk_hcount;  /* dbg: counting received each chunk thru HTTP response (by checking chunk-size) */
#if defined (STREAM_HTTP_CHUNKED_DEBUG)
	HUINT64  i_chunk_rcount;  /* dbg: counting received each chunk thru real net_read() (by checking chunk-size) */
	HUINT64  i_total_hbytes;  /* dbg: received total chunk bytes thru HTTP response (by accumulating chunk-data) */
	HUINT64  i_total_rbytes;  /* dbg: received total chunk bytes thru real net_read() (by accumulating chunk-data) */
#endif
} HTTP_CHUNK_t;

typedef struct
{
	int fd;
	tls_session_t *p_tls;
	struct virtual_socket_t    *p_vs;

	/* From uri */
	URL_t 		url;
	char    	*psz_user_agent;
	HTTP_AUTH_t auth;

	/* Proxy */
	HBOOL 		b_proxy;
	URL_t  		proxy;
	HTTP_AUTH_t proxy_auth;
	char       	*psz_proxy_passbuf;

	/* */
	int        i_code;
#if 0 /* This (const) cause many warnings: cast discards qualifiers from pointer target type */
	const char *psz_protocol;
#else
	char *psz_protocol;
#endif
	int        i_version;
	HBOOL      b_range_accepted; /* default true; */
	HBOOL      b_range_response; /* whether the Accept-Range field is found */

	char       *psz_mime;
	char       *psz_pragma;
	char       *psz_location;

#if 1 /* DRAGON TEST : support Youview Specific TimeLine */
	char       *psz_index;			/* support X-IndexFileLocation Header (youview only) */
	char       *psz_bps;			/* support X-BytesPerSecond Header (youview only) */
#endif

	HBOOL 		b_mms;
	HBOOL 		b_ssl;

	HBOOL 	     b_chunked; /* HTTP1.1 chunked transfer-encoding */
	HTTP_CHUNK_t t_chunk;

	int        	i_icy_meta;
	HINT64    	i_icy_offset;
	char       	*psz_icy_name;
	char       	*psz_icy_genre;
	char       	*psz_icy_title;


	char		*i_icy_url;
	int			i_icy_br;
	int			i_icy_pub;
	HBOOL		b_content_range;
	HBOOL		b_content_length;
	HBOOL		b_content_mime;
	HBOOL 		b_icecast;
	HBOOL		b_inet_radio;

	DI_AUDIO_FORMAT e_inetradio_codec;


	/* file size is always needed when using VFIO based bmedia */
	HBOOL		b_has_size;       /* updated with content-length or end-offset through range response(chunk) */
	HINT64 		i_remaining;      /* file_size - start_offset */
	HBOOL 		b_seekable;
	HBOOL 		b_reconnect;
	HBOOL 		b_continuous;
	HBOOL 		b_pace_control;
	HBOOL 		b_persist;

	HBOOL		b_cache_control;
	/* for Back-Off Algo. & Redirection */
	unsigned int i_redirect_count;
	unsigned int i_connect_retry_count;
	unsigned int i_backoff_retry_count;
	unsigned int i_backoff_retry_delay;
	unsigned int i_backoff_retry_totaldelay;
	unsigned int i_backoff_retry_endtime;

	char * cookies;
} HTTP_ACCESS_SYS_t;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
int 	STREAM_HTTP_Open ( STREAM_ACCESS_t *pAccess );
void 	STREAM_HTTP_Close ( STREAM_ACCESS_t *pAccess );
ssize_t	STREAM_HTTP_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len );
#if 0 /* junsy :  for zib library */
ssize_t ReadCompressed( struct access_t *, HUINT8 *, size_t );
#endif
int 	STREAM_HTTP_Seek ( STREAM_ACCESS_t *pAccess, HINT64 i_pos );
int 	STREAM_HTTP_Control( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2);
#if 0
void 	STREAM_HTTP_GetProtoStatusCode (HINT32 *pHttpCode);
#endif
/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __STREAM_HTTP_H__ */
