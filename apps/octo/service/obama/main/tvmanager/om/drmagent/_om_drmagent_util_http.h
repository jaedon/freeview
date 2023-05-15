/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_http.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 10/25/11 3:19p $
 *
 * Module Description:
 *
 * Playready library
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_http.h $
 *
 * 3   10/25/11 3:19p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/


/***************************************************************************
 ****************************************************************************
 This code file is copyed by Calypso PlayReady Module.
 ****************************************************************************
 ***************************************************************************/

#ifndef __OM_DRMAGENT_UTIL_HTTP_H__
#define __OM_DRMAGENT_UTIL_HTTP_H__

#include <stdint.h>                 /* standard includes*/
#include <stdio.h>

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <hlib.h>

#if 0
#include "blst_slist.h"             /* brcm linked list */
#endif

#if defined(_WIN32)
    #include <io.h>                 /* open/close */
#endif

/* external buffer sizes
*/
#define MAX_RESPONSE_NUM    (0x04)  /* max response buff size */
#define MAX_PROTO           (0x08)  /* proto:// max size */
#define MAX_HOST            (0x40)  /* host max size */
#define MAX_QUERY           (0xFF)  /* max query buff size */
#define MAX_FRAGMENT        (0x20)  /* fragment/label max size */
#define MAX_PATH            (0x80)  /* path max size */

/* http headers singly linked list
*/
#define BLST_S_HEAD(name, type) struct name { struct type *sl_first;}
#define BLST_S_ENTRY(type) struct { struct type *sl_next; }
#define BLST_S_INIT(phead)    (phead)->sl_first = NULL
#define BLST_S_FIRST(phead) (phead)->sl_first
#define BLST_S_NEXT(pitem, field) (pitem->field.sl_next)
#define BLST_S_INSERT_HEAD(phead, pitem, field) do { \
                (pitem)->field.sl_next = (phead)->sl_first; \
                (phead)->sl_first = (pitem); \
               } while(0)

typedef BLST_S_HEAD(bhttp_hdr_lst, bhttp_hdr_indiv) bhttp_hdr_lst;

/* individual http header structure
*/
typedef struct bhttp_hdr_indiv {
    BLST_S_ENTRY(bhttp_hdr_indiv) link; /* list link */
    HCHAR* hdr_name;               /* header name */
    HCHAR* hdr_value;                    /* header value */
    HUINT32 hdr_name_len;              /* header name length */
    HUINT32 hdr_value_len;             /* header value length */
} bhttp_hdr_indiv;

/* http engine context structure
*/
typedef struct bhttp_engine {
    HUINT8 _useSendRecv;           /* if 1, use _currentFd, else _inputFd/_outputFd; */
    FILE *_currentFile;             /* use this if !NULL, else _currentFd */
    HINT32 _currentFd;             /* currently used file descriptor */
    HINT32 _inputFd, _outputFd;    /* i/o file descriptors */
    HUINT8 _printHttp;             /* toggle debug headers */
    bhttp_hdr_lst _headers;         /* singly linked list of http headers */
    HUINT8 _wroteHeaders;          /* bool for write header status */
    HUINT8 _readHeaders;           /* bool for read header status */
    HUINT32 _responseCode;         /* header response code */
#ifdef _WINSOCKAPI_
    WSADATA WinSockData;            /* winsock data descriptor */
#endif
	HINT32 _isSecureSocket;
	SSL_CTX     *ctx;
	SSL         *ssl;
} bhttp_engine;

/* url object context
*/
typedef struct bdrm_url {
    HUINT8 _search;                /* is this a search */
    HUINT16 _port;                 /* port value */
    HCHAR* _url;                     /* full url ptr */
    HCHAR* _query;                   /* ptr into query */
    HCHAR _protocol[MAX_PROTO];      /* protocol prefix */
    HCHAR _host[MAX_HOST];           /* server host */
    HCHAR _fragment[MAX_FRAGMENT];   /* label after '#' */
    HCHAR _path[MAX_PATH];           /* remote server path */
} bdrm_url;

/* HTTP ENGINE APIs
*/

/* clean up the list of headers and free allocated ones
*/
int om_drmagent_bhttpengine_headers_cleanup (
    bhttp_engine* http              /* [in] http engine instance */
    );

/* read one line at a time from the read end of the connection
*/
char* om_drmagent_bhttpengine_read_line (
    bhttp_engine* http,             /* [in] http engine instance */
    HCHAR* buf,                      /* [out] line buffer */
    HUINT32 len                    /* [in] line buffer length */
    );

/* add new header using a name/int value pair
*/
void om_drmagent_bhttpengine_set_header (
    bhttp_engine* http,             /* [in] http engine instance */
    const HCHAR* name,               /* [in] header name */
    HUINT32 value                  /* [in] numeric header value */
    );

/* add a new header name/value pair to the http context
*/
void om_drmagent_bhttpengine_set_headers (
    bhttp_engine* http,             /* [in] http engine instance */
    const HCHAR* name,               /* [in] header name */
    HCHAR* value                     /* [in] stringized header value */
    );

/* clean up and refill the list of headers
*/
uint32_t om_drmagent_bhttpengine_read_headers (
    bhttp_engine* http              /* [in] http engine instance */
    );

/* format and write to output all headers in the specified instance
*/
int32_t om_drmagent_bhttpengine_write_headers (
    bhttp_engine* http              /* [in] http engine instance */
    );

/* close/shutdown opened descriptors for the current instance
*/
void om_drmagent_bhttpengine_close (
    bhttp_engine* http              /* [in] http engine instance */
    );

/* internal function used for write ops
*/
int32_t om_drmagent_bhttpengine_internal_write (
    bhttp_engine* http,             /* [in] http engine instance */
    const HCHAR* buf,                /* [in] internal buffer */
    HINT32 len                     /* [in] internal buffer length */
    );

/* wrapper on top of the write functions for headers and data
*/
int32_t om_drmagent_bhttpengine_write (
    bhttp_engine* http,             /* [in] http engine instance */
    const HCHAR* buf,                /* [in] write buffer */
    HINT32 len                     /* [in] write buffer length */
    );

/* flush outputs for current file descriptor
*/
void om_drmagent_bhttpengine_flush (
    bhttp_engine* http              /* [in] http engine instance */
    );

/* generic read function, wrapper on top of the IOs
*/
uint32_t om_drmagent_bhttpengine_read (
    bhttp_engine* http,             /* [in] http engine instance */
    HCHAR* buf,                      /* [out] read buffer */
    HUINT32 len                    /* [in] read buffer length */
    );

/* create a new instance of the http engine and init
*/
void om_drmagent_bhttpengine_init (
    bhttp_engine* http              /* [in] http engine instance */
    );

/* set url from char, parse elements and store in members.
   determine the protocol port number, based on proto prefix.
*/
void om_drmagent_burl_set (
    bdrm_url* drm_url,              /* [in] url object instance */
    HCHAR *drm_surl                  /* [in] stringized drm url */
    );

/* URL PROCESSOR APIs
*/

/* print members of a url object
*/
void om_drmagent_burl_print (
    HCHAR *drm_surl                  /* [in] stringized drm url */
    );

/* translate url from http encoded array
*/
void om_drmagent_burl_from_encoding (
    HCHAR* drm_surl,                 /* [out] stringized drm url */
    const HCHAR *str                 /* [in] stringized urlencoded drm url */
    );

/* translate from url to http encoding
*/
void om_drmagent_burl_to_encoding (
    HCHAR* drm_surl,                 /* [out] stringized urlencoded drm url */
    const HCHAR *str                 /* [in] stringized drm url */
    );

/* HTTP CLIENT APIs
*/

/* 'http post' wrapper function
*/
int32_t om_drmagent_bhttpclient_post (
    bhttp_engine* http,             /* [in] http engine instance */
    HCHAR *url                       /* [in] url to post to */
    );

/* handling function for the headers coming inbound from the server
*/
int32_t om_drmagent_bhttpclient_read_responsehdr (
    bhttp_engine* http              /* [in] http engine instance */
    );

/* drm license post function; feeds an XML drm challenge to a server, parses
   the response looking for the license section, and passes it back to
   the drm engine for processing.
*/
int32_t om_drmagent_bhttpclient_license_post_default (
    HCHAR* url,                      /* [in] url to post to */
    HCHAR* chall,                    /* [in] drm challenge */
    uint8_t non_quiet,              /* [in] indicates acq quietness */
    HUINT32 app_sec,               /* [in] application security level */
    HCHAR* 	 action,				/* [in] soap action  */
    HUINT8** resp,           /* [out] drm response */
    HUINT8** resp_start,     /* [out] ptr to license response start */
    HUINT32* resp_len              /* [out] drm response length */
    );

/* drm license post function; feeds a SOAP drm challenge to a server, parses
   the response looking for the license section, and passes it back to
   the drm engine for processing.
*/
int32_t om_drmagent_bhttpclient_license_post_soap (
    char* url,                      /* [in] url to post to */
    char* chall,                    /* [in] drm challenge */
    uint8_t non_quiet,              /* [in] indicates acq quietness */
    uint32_t app_sec,               /* [in] application security level */
    char* 	 action,				/* [in] soap action  */
    unsigned char** resp,           /* [out] drm response */
    unsigned char** resp_start,     /* [out] ptr to license response start */
    uint32_t* resp_len              /* [out] drm response length */
    );

int32_t om_drmagent_bhttpclient_license_post_soap_ex (HCHAR* url, HCHAR* chall, HUINT8 non_quiet,
		HUINT32 app_sec, HCHAR *action, HUINT8** resp,
		HUINT8** resp_start, HUINT32* out_resp_len, bhttp_hdr_indiv *customheader, HINT32 nCustomheaders);

void om_drmagent_bhttpclient_set_certificate(HINT8 *pPrivateKeyPath, HINT8 *pCertificatePath);

int32_t om_drmagent_bhttpclient_send_request(bhttp_engine* http, const HCHAR *method, HCHAR *urlstr) ;
const char* om_drmagent_bhttpengine_get_header(bhttp_engine* http, const HCHAR* name) ;


#endif /*__DRMHTTP_H__*/

