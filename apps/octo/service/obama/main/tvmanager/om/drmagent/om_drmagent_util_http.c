/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_http.c $
 * $brcm_Revision: 12 $
 * $brcm_Date: 2/3/12 5:07p $
 *
 * Module Description:
 *
 * Playready library
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_http.c $
 *
 * 12   2/3/12 5:07p piyushg
 * SW7429-72: Potential memory leak fix during brutus exit
 *
 * 11   11/1/11 9:51a hugost
 * SW7425-1625: Added MP4 fragment support
 *
 * 10   10/25/11 2:42p piyushg
 * SW7335-1373: Remove backslash '\' characters from server response.
 * Current playready
 * PK XML parser is not able to process them, hence fails in retriving the
 * license.
 ***************************************************************************/



/***************************************************************************
****************************************************************************
This code file is copyed by Calypso PlayReady Module.
****************************************************************************
***************************************************************************/
#include <hlib.h>

#include <sys/types.h>                  /* standard includes */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

#include "_om_drmagent_util_http.h"                    /* local function decls */

#if defined(__vxworks)
#include <sockLib.h>                /* struct timeval */
#include <selectLib.h>              /* select() */
#endif

#if defined(__unix__) || defined(__vxworks)
#include <netinet/in.h>
#include <arpa/inet.h>              /* inet_pton */
#include <netdb.h>                  /* gethostbyname */
#include <unistd.h>
#include <sys/socket.h>             /* socket/bind/etc. */
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#endif

/* THIS MUST BE LAST */
//#include "cso_debug.h"
//#include "cso_mem_trace.h"


#define ASSERT
#define UNUSED_PARAM(x)  ((void)x)

/* local defines
*/
#define HTTP_NOTCONNECTED       (0x00)      /* connection status is off */
#define HTTP_CONNECTED          (0x01)      /* connection is on */
#define HTTP_KEEP_ALIVE_OFF     (0x00)      /* keep alive header off */
#define HTTP_KEEP_ALIVE_ON      (0x01)      /* keep alive header on */
#define HTTP_MAX_HDR_LINE       (0x400)     /* manipulation buffer defines */
#define HTTP_MAX_FILE_BUFF      (0xFFF)     /* max header line size */
#define MAX_HDR_INT_NUM         (16)        /* max digits in a header int */
#define HTTP_XMLRPC_BUFSIZE     (1024*64)   /* xml RPC buffer size */
#define HTTP_OK_RESPONSE        (200)       /* http 'OK' response code */

/* stringized/misc. defines
*/
/* URL encoded content type */
#define HDR_VALUE_URLENCODED_CT    "application/x-www-form-urlencoded"         /* specifies urlencoded posting */
/* Content type is xml.  Character set is utf-8 (used by wmdrm server?) */
#define HDR_VALUE_XML_UTF8_CT      "text/xml; charset=utf-8"         /* specifies urlencoded posting */

#define DRM_POST_PREFIX         "appsecurity=%d&nonsilent=%d&challenge="    /* drm specific post prefix, pre-challenge */
#define PROTO_FILE_TOKEN        "file:"                 /* protocol token present in file:// */
#define PROTO_HTTP_UNSAFE       ";@&<>\"#%{}|\\^~[]` "  /* invalid chars in http address */
#define LICRESPONSE_OPENED      "<LICENSERESPONSE>"     /* open xml tag in license response */
#define LICRESPONSE_CLOSED      "</LICENSERESPONSE>"    /* closed xml tag in license response */
#define HDR_TAG_CONTENT_LENGTH  "Content-Length"        /* header tag for content-length */
#define HDR_TAG_CONTENT_TYPE    "Content-Type"          /* header tag for content-type */
#define HDR_TAG_SOAP_ACTION     "SOAPAction"
#define HDR_TAG_HOST            "Host"                  /* Host Name */ /* by ykkim3 for Posting to PlayReady DRM Server */


/* zero-pad string macros
*/
#define CLRSTR_LEN(s, len)  bzero((s), (len));
#define CLRSTR(str)         CLRSTR_LEN((str), (HxSTD_StrLen((str))));

int8_t *s_pPrivateKeyPath = NULL;
int8_t *s_pCertificatePath = NULL;

/* for https */
#define BUFFER_SIZE     16383
/* replace 'from' char with 'to' in array
*/
	HUINT32
om_drmagent_burl_str_replace(HCHAR* str, HCHAR to, HCHAR from)
{
	HUINT32 idx;
	HUINT32 count = 0, len = HxSTD_StrLen(str);

	for(idx = 0; idx < len; idx++) {
		if (str[idx] == from) {
			str[idx] = to;
			count++;
		}
	}

	return (count);
}

/* get chunk from array, 'from' -> 'to' (TODO: 'to')
*/
	HCHAR*
om_drmagent_burl_str_mid(HCHAR* str, HUINT32 from)
{
	return (str) + (from);
}

/* find char in array, report position
*/
	HINT32
om_drmagent_burl_str_find(HCHAR* str, HCHAR ch)
{
	HINT32 idx;
	HINT32 count = 0, len = HxSTD_StrLen(str);

	for(idx = 0; idx < len; idx++) {
		if (str[idx] == ch) {
			count = idx;
			break;
		}
	}

	return ((count > 0) ? count : -1);
}

/* concat in source with length src_len, str
*/
	void
om_drmagent_burl_str_strncat(HCHAR* source, HUINT32 src_len, const HCHAR *str, HINT32 len)
{
	HINT32 _len = src_len;
	HCHAR* _s = source;

	if (!str) return;
	if (len == -1) len = HxSTD_StrLen(str);

	HxSTD_StrNCpy(_s+_len, str, len);
	_len += len;
	_s[_len] = 0;
}

/* set str length and pad with 'padchar'
*/
	void
om_drmagent_burl_str_setlen(HCHAR* str, HINT32 src_len, HINT32 length, HCHAR pad)
{
	HINT32 idx, l;
	HCHAR* temp;
	HINT32 _len = src_len;
	HCHAR* _s = str;

	if ((length < _len) && (length >= 0)) {
		_len = length; _s[_len] = 0;
	} else if (length > _len) {
		l = length - _len; temp = (HCHAR *)HLIB_STD_MemAlloc(l);
		for (idx = 0; idx < l; idx++) temp[idx] = pad;
		om_drmagent_burl_str_strncat(str, src_len, temp, l);
		HLIB_STD_MemFree(temp);
	}
}

/* look up the default port for service/protocol
*/
	HUINT32
om_drmagent_burl_lookup_port(const HCHAR *serv, const HCHAR *proto)
{
	struct servent* svt = getservbyname(serv, proto);
	return (svt != (struct servent*)NULL) ? (HUINT32)ntohs(svt->s_port) : 0;
}

/* clean up the members of the url struct
*/
	void
om_drmagent_burl_clear(bdrm_url* drm_url)
{
	drm_url->_port = 0; drm_url->_search = 0;
	CLRSTR_LEN(drm_url->_protocol, sizeof(drm_url->_protocol));
	CLRSTR_LEN(drm_url->_host, sizeof(drm_url->_host));

	drm_url->_url = NULL; drm_url->_query = NULL;
	CLRSTR_LEN(drm_url->_fragment, sizeof(drm_url->_fragment));
	CLRSTR_LEN(drm_url->_path, sizeof(drm_url->_path));
}

/* get path from url object
*/
	const HCHAR*
om_drmagent_burl_path(bdrm_url* drm_url)
{
	return (const HCHAR *)(drm_url->_path);
}

/* get search from url object
*/
	const HCHAR*
om_drmagent_burl_search(bdrm_url* drm_url)
{
	return (drm_url->_search==1) ? (&drm_url->_query[HxSTD_StrLen(drm_url->_path) + 1]) : (NULL);
}

/* get fragment from url object
*/
	const HCHAR*
om_drmagent_burl_fragment(bdrm_url* drm_url)
{
	return (drm_url->_search == 2) ? (&drm_url->_query[HxSTD_StrLen(drm_url->_path) + 1]) : (NULL);
}

/* get if absolute path from url object
*/
	HUINT32
om_drmagent_burl_is_absolute(bdrm_url* drm_url)
{
	return ((drm_url->_query) && (drm_url->_query[0] == '/'));
}

/* set url from char, parse elements and store in members.
   determine the protocol port number, based on proto prefix.
   */
	void
om_drmagent_burl_set(bdrm_url* drm_url, HCHAR *url)
{
	HINT32 colon;
	HCHAR *findhost = NULL, *s = NULL;
	HCHAR *find = NULL, *search = NULL;

	om_drmagent_burl_clear(drm_url); drm_url->_url = url;
	if (drm_url->_url == NULL) return;

	s = drm_url->_url;
	if (!HxSTD_StrNCmp(s, PROTO_FILE_TOKEN, HxSTD_StrLen(PROTO_FILE_TOKEN))) {
		HxSTD_StrNCpy(drm_url->_protocol,"file", MAX_PROTO); drm_url->_query = s + 5;
		return;
	}
	if ((findhost = HxSTD_StrStr(s, "//")) != NULL) {
		if ((findhost > s) && (*(findhost-1) == ':')) {
			HxSTD_StrNCpy(drm_url->_protocol, s, findhost-s-1);
		}
		findhost += 2;
		if ((find = HxSTD_StrChar(findhost, '/')) != NULL) {
			HxSTD_StrNCpy(drm_url->_host, findhost, find-findhost);
			drm_url->_query = find;
		} else { HxSTD_StrNCpy(drm_url->_host, findhost, MAX_HOST-1); drm_url->_query = NULL; }

		if ((colon = om_drmagent_burl_str_find(drm_url->_host, ':')) != -1) {
			drm_url->_port = atoi(om_drmagent_burl_str_mid(drm_url->_host, colon + 1));
			om_drmagent_burl_str_setlen(drm_url->_host, HxSTD_StrLen(drm_url->_host), colon, 0);
		} else drm_url->_port = om_drmagent_burl_lookup_port(drm_url->_protocol, "tcp");
	}
	else { drm_url->_port = 0; drm_url->_query = s;
	}
	if (drm_url->_query) {
		if ((search = HxSTD_StrChar(drm_url->_query, '?')) != NULL) {
			HxSTD_StrNCpy(drm_url->_path, drm_url->_query, search - drm_url->_query);
			drm_url->_search = 1;
		} else if ((search = HxSTD_StrChar(drm_url->_query, '#')) != NULL) {
			HxSTD_StrNCpy(drm_url->_path, drm_url->_query, search - drm_url->_query);
			drm_url->_search = 2;
		} else { CLRSTR(drm_url->_path); drm_url->_search = 0; }
	} else { CLRSTR(drm_url->_path); drm_url->_search = 0; }
}

/* translate from url to http encoding
*/
	void
om_drmagent_burl_to_encoding(HCHAR* drm_url, const HCHAR *str)
{
	HUINT32 n;
	HCHAR buf[4];
	HCHAR* result = drm_url;
	while (*str) {
		if ((n = strcspn(str, PROTO_HTTP_UNSAFE))) {
			om_drmagent_burl_str_strncat(result, HxSTD_StrLen(result), str, n);
			str += n; if (!*str) break;
		}
		snprintf(buf, sizeof(buf)/sizeof(HCHAR),"%%%02x", str[0]);
		om_drmagent_burl_str_strncat(result, HxSTD_StrLen(result), buf, -1);
		str++;
	}

	return;
}

/* translate url from http encoded array
*/
	void
om_drmagent_burl_from_encoding(HCHAR* drm_url, const HCHAR *str)
{
	HUINT32 ch, _len = 0;
	const HCHAR *next;
	HCHAR buf[3];

	buf[2] = 0;
	while ((next = HxSTD_StrChar(str, '%')) && next[1] && next[2]) {
		om_drmagent_burl_str_strncat(drm_url, _len, str, next-str); next++;     /* skip % */
		HxSTD_memcpy(buf, next, 2); sscanf(buf, "%x", &ch);
		_len = HxSTD_StrLen(drm_url); (*(drm_url+_len)) = (HCHAR)ch;
		_len++; (*(drm_url+_len)) = (HCHAR)0; str = next + 2;        /* skip 2 digits */
	}
	om_drmagent_burl_str_strncat(drm_url, HxSTD_StrLen(drm_url), str, -1);

	return;
}

/* print members of a url object
*/
	void
om_drmagent_burl_print(HCHAR *s)
{
	bdrm_url drm_url;

	om_drmagent_burl_set(&drm_url, s);
	HxLOG_Debug("URL=%s\n", s);

	HxLOG_Debug("%s,%s,%d,qry=%s,path=%s\n  search=%s, frag=%s\n",
			drm_url._protocol, drm_url._host, drm_url._port, drm_url._query,
			om_drmagent_burl_path(&drm_url), om_drmagent_burl_search(&drm_url),
			om_drmagent_burl_fragment(&drm_url));
}

STATIC HINT32 om_drmagent_peerverify_callback (HINT32 ok, X509_STORE_CTX *store)
{
	HCHAR data[256] = {0,};

	if (!ok)
	{
		X509 *cert = X509_STORE_CTX_get_current_cert (store);
		HINT32  depth = X509_STORE_CTX_get_error_depth (store);
		HINT32  err = X509_STORE_CTX_get_error (store);

		HxLOG_Debug("Error with certificate at depth: %i\n",depth);
		X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
		HxLOG_Debug("issuer   = %s\n",data);
		X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
		HxLOG_Debug("subject  = %s\n",data);
		HxLOG_Debug("err %i:%s\n",err, X509_verify_cert_error_string(err));
	}

	return ok;
}

HINT32 om_drmagent_passwd_cb(HCHAR *buf, HINT32 size, HINT32 rwflag, void *password)
{
	(void)(rwflag);

	HxSTD_StrNCpy(buf, (HCHAR *)(password), size);
	buf[size - 1] = '\0';
	return(HxSTD_StrLen(buf));
}

STATIC HINT32 om_drmagent_read_response ( bhttp_engine* http, HCHAR *buf, HUINT32 len )
{
	HINT32         done;
	HINT32         num_read;
	HINT32         read_len;
	HINT32         ssl_err;
	HCHAR		*buffer;
	HINT32 		total_read = 0;

	buffer = (HCHAR*)HLIB_STD_MemAlloc(BUFFER_SIZE);

	done = 0;
	while (!done)
	{
		HxSTD_MemSet(buffer,0,BUFFER_SIZE);
		read_len = sizeof(buffer);
		if((total_read + read_len) > (HINT32)len)
		{
			HxLOG_Debug("itotal_read + read_len = %d\n",total_read + read_len);
			read_len = len - total_read;
		}

		num_read = SSL_read(http->ssl,buffer,read_len);
		if(!num_read)
		{
			break;
		}
		else if(num_read > 0)
		{
			HxSTD_memcpy(buf+total_read, buffer, num_read);
			total_read += num_read;
			buffer[num_read] = '\0';
		}
		else
		{
			HxLOG_Debug("om_drmagent_read_response num_read = %d\n",num_read);
		}
		ssl_err = SSL_get_error(http->ssl,num_read);
		switch (ssl_err) {
			case SSL_ERROR_NONE:
				/*
				 ** We read some bytes.  Dump them to the console.
				 */
				break;
			case SSL_ERROR_ZERO_RETURN:
				/*
				 ** a normal closed socket
				 */
				done = 1;
				break;
			case SSL_ERROR_WANT_READ:
				/*
				 ** We need to read again
				 */
				HxLOG_Debug("om_drmagent_read_response SSL_ERROR_WANT_READ\n");
				break;
			default:
				/*
				 ** Something's wrong
				 */
				HxLOG_Error("SSL_read failed(%d), %d bytes read (%s)\n",ssl_err,num_read,ERR_error_string(ERR_get_error(),NULL));
				done = 1;
				break;
		}
	}

	buf[total_read] = '\0';

	HLIB_STD_MemFree(buffer);

	return total_read;
}


STATIC HINT32 om_drmagent_read_line_response ( bhttp_engine* http, HCHAR *buf )
{
	HINT32         done;
	HINT32         num_read;
	HINT32         ssl_err;
	HCHAR		ch;
	HINT32 		total_read = 0;

	done = 0;
	while (!done)
	{
		num_read = SSL_read(http->ssl,&ch,1);
		if(!num_read)
		{
			break;
		}
		else if(num_read > 0)
		{
			if (ch == '\n')
			{
				break;
			}

			HxSTD_memcpy(buf+total_read, &ch, num_read);
			total_read += num_read;
		}
		else
		{
			HxLOG_Debug("om_drmagent_read_line_response num_read = %d\n",num_read);
		}

		ssl_err = SSL_get_error(http->ssl,num_read);
		switch (ssl_err) {
			case SSL_ERROR_NONE:
				/*
				 ** We read some bytes.  Dump them to the console.
				 */
				break;
			case SSL_ERROR_ZERO_RETURN:
				/*
				 ** a normal closed socket
				 */
				done = 1;
				break;
			case SSL_ERROR_WANT_READ:
				/*
				 ** We need to read again
				 */
				HxLOG_Debug("om_drmagent_read_line_response SSL_ERROR_WANT_READ\n");
				break;
			default:
				/*
				 ** Something's wrong
				 */
				HxLOG_Error("SSL_read failed(%d), %d bytes read (%s)\n",ssl_err,num_read,ERR_error_string(ERR_get_error(),NULL));
				done = 1;
				break;
		}
		if((total_read + 1) > (64*1024))
		{
			HxLOG_Debug("total_read = %d\n",total_read);
			done = 1;
		}
	}

	buf[total_read] = '\0';

	return total_read;
}

STATIC HINT32 om_drmagent_send_request ( bhttp_engine* http, const HCHAR *buf )
{
	HINT32         num_written;
	size_t      request_len;

	request_len = HxSTD_StrLen(buf);
	num_written = SSL_write(http->ssl,buf,request_len);

	if (num_written <= 0)
	{
		HxLOG_Error("SSL_write failed: %s\n",ERR_error_string(ERR_get_error(),NULL));
		return 0;
	}

	if ((size_t)num_written < request_len)
	{
		HxLOG_Error("SSL_write incomplete\n");
		return 0;
	}

	return num_written;
}

STATIC void om_drmagent_ssl_cleanup ( bhttp_engine* http )
{
#if 0
	ERR_free_strings();
	EVP_cleanup();
#endif

	if(http->ssl)
	{
		HxLOG_Debug("!http->ssl\n");
		SSL_shutdown(http->ssl);
		SSL_clear(http->ssl);
		SSL_free(http->ssl);
		http->ssl = NULL;
	}

	if(http->ctx)
	{
		HxLOG_Debug("!http->ctx\n");
		SSL_CTX_free(http->ctx);
		http->ctx = NULL;
	}

	return;
}

STATIC HINT32 om_drmagent_ssl_connect ( bhttp_engine* http )
{
	long verify_res=0;

	/*
	 ** Now deal with SSL over the connection
	 */
	http->ssl = SSL_new(http->ctx);
	if (!http->ssl)
	{
		HxLOG_Error("SSL_new failed\n");
		return 0;
	}

	SSL_set_connect_state(http->ssl);

	if (!SSL_set_fd(http->ssl, http->_currentFd))
	{
		HxLOG_Error("SSL_set_fd failed: %s\n",ERR_error_string(ERR_get_error(),NULL));
		return 0;
	}

	HxLOG_Debug("om_drmagent_ssl_connect starting\n");

#if 1
	verify_res = SSL_get_verify_result(http->ssl);
	if( X509_V_OK != verify_res )
	{
		HxLOG_Error("SSL_get_verify_result failed: %s\n",ERR_error_string(ERR_get_error(),NULL));
	}
	else
	{
		HxLOG_Debug("SSL_get_verify_result success: %ld\n",verify_res);
	}
#endif

	if (SSL_connect(http->ssl) <= 0)
	{
		HxLOG_Error("om_drmagent_ssl_connect failed: %s\n",ERR_error_string(ERR_get_error(),NULL));
		return 0;
	}


	return 1;
}

STATIC HINT32 om_drmagent_ssl_init ( bhttp_engine* http )
{
	HxLOG_Debug("om_drmagent_ssl_init\n");

	if (!SSL_library_init())
	{
		HxLOG_Error("SSL_Library_init failed\n");
		return 0;
	}
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings ();

	http->ctx = SSL_CTX_new(SSLv23_client_method());
	if (!http->ctx)
	{
		HxLOG_Error("SSL_CTX_new failed\n");
		return 0;
	}

	(void)SSL_CTX_set_options(http->ctx, SSL_OP_ALL);


#if 1 /* load certificate & private key for 2 way ssl */
	if((s_pPrivateKeyPath != NULL) && (s_pCertificatePath != NULL))
	{
		SSL_CTX_set_default_passwd_cb(http->ctx, om_drmagent_passwd_cb);
		SSL_CTX_set_default_passwd_cb_userdata(http->ctx,  (void *)s_pPrivateKeyPath);

		if ( SSL_CTX_use_certificate_chain_file (http->ctx, s_pCertificatePath) != 1)
		{
			HxLOG_Error("SSL_CTX_use_certificate_chain_file failed: %s[file : %s]\n",ERR_error_string(ERR_get_error(),NULL), s_pCertificatePath);
			return 0;
		}

		if ( SSL_CTX_use_RSAPrivateKey_file(http->ctx, s_pPrivateKeyPath, SSL_FILETYPE_PEM) != 1)
		{
			HxLOG_Error("SSL_CTX_use_RSAPrivateKey_file failed: %s[file : %s]\n",ERR_error_string(ERR_get_error(),NULL), s_pPrivateKeyPath);
			return 0;
		}

		if (!SSL_CTX_check_private_key(http->ctx)) {
			HxLOG_Error("SSL_CTX_check_private_key failed: %s\n",ERR_error_string(ERR_get_error(),NULL));
			return 0;
		}
	}
#endif

	if ( SSL_CTX_load_verify_locations (http->ctx, NULL, "/etc/pki/tls") != 1)
	{
		HxLOG_Error("SSL_CTX_load_verify_locations failed\n");
		return 0;
	}

	if ( SSL_CTX_set_default_verify_paths (http->ctx) != 1)
	{
		HxLOG_Error("SSL_CTX_set_default_verify_paths filed\n");
		return 0;
	}

	SSL_CTX_set_verify (http->ctx, SSL_VERIFY_PEER, om_drmagent_peerverify_callback);
	SSL_CTX_set_verify_depth (http->ctx, 4);

	return 1;
}

/* add a new header name/value pair to the http context
*/
	void
om_drmagent_bhttpengine_set_headers(bhttp_engine* http, const HCHAR* name, HCHAR* value)
{
	HUINT32 name_len, value_len;
	bhttp_hdr_indiv* http_hdr;

	http_hdr = (bhttp_hdr_indiv*)HLIB_STD_MemAlloc(sizeof(bhttp_hdr_indiv));
	if(http_hdr == NULL)
	{
		HxLOG_Error( "Out of Memory\n");
		return;
	}
	name_len = HxSTD_StrLen(name);
	http_hdr->hdr_name = (HCHAR*)HLIB_STD_MemAlloc(name_len+1);
	if(http_hdr->hdr_name== NULL)
	{
		HLIB_STD_MemFree(http_hdr);
		HxLOG_Error( "Out of Memory\n");
		return;
	}
	HxSTD_MemSet(http_hdr->hdr_name, 0x0, name_len+1);
	HxSTD_StrNCpy(http_hdr->hdr_name, name, name_len+1);

	value_len = HxSTD_StrLen(value);

	http_hdr->hdr_value = (HCHAR*)HLIB_STD_MemAlloc(value_len+1);
	if(http_hdr->hdr_value== NULL)
	{
		HLIB_STD_MemFree(http_hdr->hdr_name);
		HLIB_STD_MemFree(http_hdr);
		HxLOG_Error( "Out of Memory\n");
		return;
	}
	HxSTD_MemSet(http_hdr->hdr_value, 0x0, value_len+1);
	HxSTD_StrNCpy(http_hdr->hdr_value, value, value_len+1);

	http_hdr->hdr_name_len = name_len;
	http_hdr->hdr_value_len = value_len;
	BLST_S_INSERT_HEAD(&http->_headers, http_hdr, link);
}

/* create a new instance of the http engine and init
*/
	void
om_drmagent_bhttpengine_init(bhttp_engine* http)
{
	BLST_S_INIT(&http->_headers);
	http->_printHttp = 0;
	http->_useSendRecv = 1;
	http->_currentFile = NULL;
	http->_currentFd = http->_outputFd = http->_inputFd = -1;
#ifdef _WINSOCKAPI_
	WSAStartup(MAKEWORD(1,1), &http->WinSockData);
#endif

	http->_isSecureSocket = 0;
	http->ctx = NULL;
	http->ssl = NULL;
}

/* destroy an existing http engine instance
*/
	void
om_drmagent_bhttpengine_uninit(bhttp_engine* http)
{
	UNUSED_PARAM(http);
#ifdef _WINSOCKAPI_
	WSACleanup();
#endif
}

/* close/shutdown opened descriptors for the current instance
*/
	void
om_drmagent_bhttpengine_close(bhttp_engine* http)
{
#ifndef NO_SOCKETS
	if (http->_currentFd >= 0)
		shutdown(http->_currentFd, 2);      /* SHUT_RDWR */
#endif
	if (http->_currentFile) {
		fclose(http->_currentFile);
		http->_currentFd = -1;
		http->_currentFile = NULL;
	}
	else if (http->_currentFd >= 0) {
#if defined(_WIN32)
		closesocket(http->_currentFd);
#else
		close(http->_currentFd);
#endif
		http->_currentFd = -1;
	}

	if(http->_isSecureSocket == 1)
	{
		om_drmagent_ssl_cleanup(http);
	}
	http->_isSecureSocket = 0;
}

/* returns connection state for the specified instance
*/
	HUINT8
om_drmagent_bhttpengine_is_connected(bhttp_engine* http)
{
	fd_set socket_set;
	struct timeval tv;

	if (http->_currentFile)
		return (HTTP_CONNECTED);
	if (!(http->_useSendRecv) && ((http->_inputFd >= 0) || (http->_outputFd >= 0)))
		return (HTTP_CONNECTED);
	if (http->_currentFd == -1)
		return (HTTP_NOTCONNECTED);

	FD_ZERO(&socket_set); FD_SET(http->_currentFd, &socket_set);
	tv.tv_sec = tv.tv_usec = 0;
	if (select(http->_currentFd + 1, &socket_set, NULL, NULL, &tv) < 0)
		return (HTTP_NOTCONNECTED);

	return (HTTP_CONNECTED);
}

/* add new header using a name/int value pair
*/
	void
om_drmagent_bhttpengine_set_header(bhttp_engine* http, const HCHAR* name, HUINT32 value)
{
	HCHAR strval[MAX_HDR_INT_NUM];
	snprintf(strval, MAX_HDR_INT_NUM, "%d", value);
	om_drmagent_bhttpengine_set_headers(http, name, strval);
}

/* return a value based on the header name, from the headers collection
*/
	const HCHAR*
om_drmagent_bhttpengine_get_header(bhttp_engine* http, const HCHAR* name)
{
	bhttp_hdr_indiv* http_hdr = NULL;
	const HCHAR* value = NULL;

	for (http_hdr = BLST_S_FIRST(&http->_headers); http_hdr;
			http_hdr = BLST_S_NEXT(http_hdr, link)) {
		if (strcasecmp(http_hdr->hdr_name, name) == 0) {
			value = http_hdr->hdr_value;
			break;
		}
	}

	return (value);
}

/* truncates white-space chars off end of 'str'
*/
	STATIC void
om_drmagent_bhttpengine_truncsp(HCHAR* str)
{
	size_t str_len;

	if(str == NULL) return;
	str_len = HxSTD_StrLen(str);
	while(str_len && (HUINT8)str[str_len - 1] <= ' ') {
		str_len--;
	}
	str[str_len] = 0;
}

/* skip up to 'chars'
*/
	HCHAR*
om_drmagent_bhttpengine_strskip(const HCHAR* str, const HCHAR* chars)
{
	return (HCHAR*)str + strspn(str, chars);
}

/* read one line at a time from the read end of the connection
*/
	HCHAR*
om_drmagent_bhttpengine_read_line(bhttp_engine* http, HCHAR* buf, HUINT32 len)
{
	HCHAR ch, *s;
	fd_set socket_set;
	HUINT32 rd = 0;

	if (!http->_currentFile) {
		while (rd < (len - 1)) {
			FD_ZERO(&socket_set);
#ifndef NO_SOCKETS
			if (http->_useSendRecv) {
				FD_SET(http->_currentFd, &socket_set);
				if (select(http->_currentFd + 1, &socket_set, NULL, NULL, NULL) < 1)
					return (NULL);                          /* invalid fd */
				if (recv(http->_currentFd, &ch, 1, 0) < 1)
					return (NULL);                          /* disconnected */
			}
			else
#endif
			{
				FD_SET(http->_inputFd, &socket_set);
				if (select(http->_inputFd + 1, &socket_set, NULL, NULL, NULL) < 1)
					return (NULL);                          /* invalid fd */
				if (read(http->_inputFd, &ch, 1) < 1)
					return (NULL);                          /* disconnected */
			}
			if (ch == '\n') break;
			buf[rd++] = ch;
		}
		buf[rd] = 0; s = buf;
	}
	else{
		if(http->_isSecureSocket)
		{
			rd = om_drmagent_read_line_response(http, buf);
			buf[rd] = 0; s = buf;
		}
		else
		{
			s = fgets(buf, len, http->_currentFile);
		}
	}

	om_drmagent_bhttpengine_truncsp(s);
	if ((http->_printHttp) && (s))
	{
		HxLOG_Debug("RX: %s\n", s);
	}

	return (s);
}

/* clean up the list of headers and free allocated ones
*/
	HINT32
om_drmagent_bhttpengine_headers_cleanup(bhttp_engine* http)
{
	bhttp_hdr_indiv* http_hdr, *lnk;
	HINT32 ret = 0, count = 0;

	for(http_hdr = BLST_S_FIRST(&http->_headers); http_hdr;
			http_hdr = lnk, count++) {
		HLIB_STD_MemFree(http_hdr->hdr_name);
		http_hdr->hdr_name = NULL;
		HLIB_STD_MemFree(http_hdr->hdr_value);
		http_hdr->hdr_value = NULL;
		lnk = BLST_S_NEXT(http_hdr, link); HLIB_STD_MemFree(http_hdr);
		http_hdr = NULL;
	}
	BLST_S_FIRST(&http->_headers) = NULL;

	return (ret);
}

/* clean up and refill the list of headers
*/
	HUINT32
om_drmagent_bhttpengine_read_headers(bhttp_engine* http)
{
	HUINT32 name_len, value_len;
	HCHAR* line, *colon, *value;
	HCHAR *buf = HLIB_STD_MemAlloc(HTTP_MAX_HDR_LINE);
	bhttp_hdr_indiv* http_hdr;

	om_drmagent_bhttpengine_headers_cleanup(http);
	while ((line = om_drmagent_bhttpengine_read_line(http,buf, HTTP_MAX_HDR_LINE))) { /* now read the headers */
		if (!*line) break;                                                /* we've finished reading the header */
		if ((colon = HxSTD_StrChar(line, ':')) != NULL) {
			value = om_drmagent_bhttpengine_strskip(colon + 1, " \t");
			http_hdr = (bhttp_hdr_indiv*)HLIB_STD_MemAlloc(sizeof(bhttp_hdr_indiv));

			name_len = colon - line;
			*colon = '\0';
			http_hdr->hdr_name = (HCHAR*)HLIB_STD_MemAlloc(name_len+1);
			HxSTD_StrNCpy(http_hdr->hdr_name, line, name_len+1);
			value_len = HxSTD_StrLen(value);

			http_hdr->hdr_value = (HCHAR*)HLIB_STD_MemAlloc(value_len+1);
			HxSTD_StrNCpy(http_hdr->hdr_value, value, value_len+1);
			http_hdr->hdr_name_len = name_len; http_hdr->hdr_value_len = value_len;
			BLST_S_INSERT_HEAD(&http->_headers, http_hdr, link);
#if 0
			HLIB_STD_MemFree(http_hdr->hdr_value);
			HLIB_STD_MemFree(http_hdr);
#endif
		}
	}

	if (!line)
	{
		HLIB_STD_MemFree(buf);
		return (-1);
	}

	HLIB_STD_MemFree(buf);
	return (0);
}

/* generic read function, wrapper on top of the IOs
*/
	HUINT32
om_drmagent_bhttpengine_read(bhttp_engine* http, HCHAR* buf, HUINT32 len)
{
	HUINT32 rd_len = 0;

	if (!http->_currentFile) {
#ifndef NO_SOCKETS
		if (http->_useSendRecv)
		{
			rd_len = recv(http->_currentFd, buf, len-1, 0);
		}
		else
#endif
		{
			rd_len = read(http->_inputFd, buf, len-1);
		}
	}
	else
	{
		if(http->_isSecureSocket)
		{
			rd_len = om_drmagent_read_response(http, buf, len-1);
		}
		else
		{
			rd_len = fread(buf, 1, len-1, http->_currentFile);
		}
	}
	buf[rd_len] = '\0';

	if (http->_printHttp)
	{
		HxLOG_Debug("RX: %.*s\n", rd_len, buf);
	}

	return (rd_len);
}

/* internal function used for write ops
*/
	HINT32
om_drmagent_bhttpengine_internal_write(bhttp_engine* http, const HCHAR* buf, HINT32 len)
{
	if (len == -1) len = HxSTD_StrLen(buf);
	if (http->_printHttp)
	{
		HxLOG_Debug("TX: %.*s\n", len, buf);
	}
	if (!http->_currentFile) {
#ifndef NO_SOCKETS
		if (http->_useSendRecv)
		{
			if(http->_currentFd > 0)
				return send(http->_currentFd, (HCHAR* )buf, len, 0);
			else
				return -1;

		}
		else
#endif
		{
			if(http->_outputFd > 0)
				return write(http->_outputFd, (HCHAR* )buf, len);
			else
				return -1;
		}
	}
	else
	{
		if(http->_isSecureSocket)
		{
			HINT32 wd_len = 0;
			wd_len = om_drmagent_send_request(http, buf);
			return wd_len;
		}
		else
		{
			return fwrite(buf, 1, len, http->_currentFile);
		}
	}
}

/* format and write to output all headers in the specified instance
*/
	HINT32
om_drmagent_bhttpengine_write_headers(bhttp_engine* http)
{
	bhttp_hdr_indiv* http_hdr;

	http->_wroteHeaders = 1;
	for (http_hdr = BLST_S_FIRST(&http->_headers);
			http_hdr; http_hdr = BLST_S_NEXT(http_hdr, link)) {
		if (om_drmagent_bhttpengine_internal_write(http, http_hdr->hdr_name, -1) == -1 ||
				om_drmagent_bhttpengine_internal_write(http, ": ", -1) == -1 ||
				om_drmagent_bhttpengine_internal_write(http, http_hdr->hdr_value, -1) == -1 ||
				om_drmagent_bhttpengine_internal_write(http, "\r\n", -1) == -1) {
			return (-1);
		}
	}
	if (om_drmagent_bhttpengine_internal_write(http, "\r\n", -1) == -1) {
		return (-1);
	}

	return (0);
}

/* wrapper on top of the write functions for headers and data
*/
	HINT32
om_drmagent_bhttpengine_write(bhttp_engine* http, const HCHAR* buf, HINT32 len)
{
	if (!http->_wroteHeaders)
		if (om_drmagent_bhttpengine_write_headers(http) == -1) {
			HxLOG_Error("Couldn't write headers, errno %d\n", errno);
			return (-1);
		}
	if (buf)
		return om_drmagent_bhttpengine_internal_write(http, buf, len);

	return(-1);
}

/* format and output one single line of clrf terminated data
*/
	HUINT32
om_drmagent_bhttpengine_writeline(bhttp_engine* http, const HCHAR* buf)
{
	if(om_drmagent_bhttpengine_write(http, buf, -1) < 0)
	{
		return (-1);
	}
	return (om_drmagent_bhttpengine_write(http, "\r\n", -1));
}

/* get buffer containing response body, accounting for content length
*/
	HINT32
om_drmagent_bhttpengine_read_body(bhttp_engine* http, HCHAR* buf, HINT32 len)
{
	HINT32 cl;
	const HCHAR* clstr;

	clstr = om_drmagent_bhttpengine_get_header(http, HDR_TAG_CONTENT_LENGTH);
	cl = (clstr) ? atoi(clstr) : (len);
	if (cl < len) len = cl;

	return om_drmagent_bhttpengine_read(http, buf, len);
}

/* write headers and data to a file
*/
	HUINT32
om_drmagent_bhttpengine_write_file(bhttp_engine* http, const HCHAR* filename)
{
	HUINT32 r;
	HCHAR buf[HTTP_MAX_FILE_BUFF+1];
	FILE* f = NULL;

	(void)HxSTD_MemSet(buf, 0, HTTP_MAX_FILE_BUFF+1);
	if ((f = fopen(filename, "r")) != NULL) {
		while (!feof(f)) {
			if (!(r = fread(buf, 1, HTTP_MAX_FILE_BUFF, f))) break;
			buf[r] = (HCHAR)'\0';
			if (om_drmagent_bhttpengine_write(http, buf, r) == -1) break;
			(void)HxSTD_MemSet(buf, 0, r);
		}
		fclose(f);
		return (0);
	}

	return (-1);
}

/* read http engine data from a file
*/
	HUINT32
om_drmagent_bhttpengine_read_file(bhttp_engine* http, const HCHAR* filename)
{
	HUINT32 n;
	HCHAR buf[HTTP_MAX_FILE_BUFF];
	FILE* f = NULL;

	if ((f = fopen(filename, "w+")) != NULL) {
		do {n = om_drmagent_bhttpengine_read(http, buf, sizeof(buf));
			if (n) fwrite(buf, 1, n, f);
		} while (n > 0);
		fclose(f);
		return (0);
	}

	return (-1);
}

/* print headers loaded by the current engine instance
*/
	void
om_drmagent_bhttpengine_print_headers(bhttp_engine* http)
{
	bhttp_hdr_indiv* http_hdr;

	for (http_hdr = BLST_S_FIRST(&http->_headers);
			http_hdr; http_hdr = BLST_S_NEXT(http_hdr, link)) {
		HxLOG_Debug("%s: %s\n", http_hdr->hdr_name, http_hdr->hdr_value);
	}
}

/* flush outputs for current file descriptor
*/
	void
om_drmagent_bhttpengine_flush(bhttp_engine* http)
{
	if (http->_currentFile != NULL)
		fflush(http->_currentFile);
}

/* For some older platforms like 7413(7405C0) which use gethostbyname() instead of getaddrinfo() to resolve host names */
#if 0//(BCHP_CHIP == 7405  && BCHP_VER == BCHP_VER_C0) || (BCHP_CHIP == 7335)
HINT32 TcpConnect(HCHAR *server, HINT32 port, HINT32 socket_type)
{
	HINT32 sd,rc;

	struct sockaddr_in localAddr, servAddr;
	struct hostent *h;

	HxLOG_Debug("TCP - Connection to %s:%d ...\n",server, port);

	h = gethostbyname(server);
	if (h == NULL) {
		HxLOG_Error("Unknown Host\n");
		return (-EINVAL);
	}

	servAddr.sin_family = h->h_addrtype;
	HxSTD_memcpy((HCHAR *) &servAddr.sin_addr.s_addr, h->h_addr_list[0],h->h_length);
	servAddr.sin_port = htons(port);

	/* Open Socket */
	sd = socket(AF_INET, socket_type, 0);
	if (sd<0) {
		HxLOG_Error("Socket Open Err\n");
		return -EINVAL;
	}

	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(0);

	if (bind(sd,(struct sockaddr *) &localAddr, sizeof(localAddr))) {
		HxLOG_Error("Socket Bind Err\n");
		return -EINVAL;
	}

	/* Connect to server */
	rc = connect(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
	if (rc<0) {
		HxLOG_Error("Connect Error: Server busy or unavailable\n");
		return -EBUSY;
	}

	HxLOG_Debug("TCP: Connection Success\n");
	return sd;
}
#endif


/* client connect function. resolve host and open a connection
   to the remote http server.
   */
	HINT32
om_drmagent_bhttpclient_connect(bhttp_engine* http, const HCHAR *host, const HCHAR *proto, HUINT32 port)
{
	/* For some older platforms like 7413(7405C0) which use gethostbyname() instead of getaddrinfo() to resolve host names */
#if 0//(BCHP_CHIP == 7405  && BCHP_VER == BCHP_VER_C0) || (BCHP_CHIP == 7335)
	http->_currentFd = TcpConnect((HCHAR*)host, port, SOCK_STREAM);
#else
	struct addrinfo hints;
	struct addrinfo *addrInfo = NULL;
	HCHAR portString[16];
	bdrm_url url;
	HCHAR *proxy_url = NULL;

	http->_currentFd = socket(AF_INET, SOCK_STREAM, 0);
	if(http->_currentFd < 0) {
		HxLOG_Error("cannot open socket\n");
		return (-1);
	}

	HxSTD_MemSet(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;    /* we dont know the family */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	HxSTD_MemSet(portString, 0, sizeof(portString));  /* getaddrinfo() requires port # in the string form */

#if 0
	setenv( "http_proxy", "http://iplayer-guests.bbc.co.uk:2323", 1);
#endif
	proxy_url = getenv( "http_proxy" );
	if(proxy_url)
	{
		om_drmagent_burl_set(&url, proxy_url);

		HxLOG_Debug("proxy host = %s, port = %d\n", url._host, url._port);

		snprintf(portString, sizeof(portString)-1, "%d", url._port);
		if (getaddrinfo(url._host, portString, &hints, &addrInfo) != 0) {
			HxLOG_Error("%s: ERROR: getaddrinfo failed for server:port: %s:%d, errno %d\n", __FUNCTION__, host, port, errno);
			return (-1);
		}
	}
	else
	{
		snprintf(portString, sizeof(portString)-1, "%d", port);
		if (getaddrinfo(host, portString, &hints, &addrInfo) != 0) {
			HxLOG_Error("%s: ERROR: getaddrinfo failed for server:port: %s:%d, errno %d\n", __FUNCTION__, host, port, errno);
			return (-1);
		}
	}

	if (connect(http->_currentFd, addrInfo->ai_addr, addrInfo->ai_addrlen) < 0) {
		HxLOG_Error("Cannot connect to %s:%d, errno %d. Try using gethostbyname() inplace of getaddrinfo()\n", host, port, errno);
		om_drmagent_bhttpengine_close(http);
		freeaddrinfo(addrInfo);
		return (-1);
	}

	freeaddrinfo(addrInfo);
#endif
	http->_currentFile = fdopen(http->_currentFd, "r+");
	if (!http->_currentFile) {
		om_drmagent_bhttpengine_close(http);
		return (-1);
	}

	if(proxy_url)
	{
		if(!HxSTD_memcmp(proto, "https", 5))
		{
			HUINT8 buf[HTTP_MAX_HDR_LINE] = {0,};
			HCHAR *line, *find;
			HCHAR response[MAX_RESPONSE_NUM];
			HINT32 responseCode = 0;

			snprintf(buf, HxSTD_StrLen("CONNECT : HTTP/1.%drnHost: :rnrn") + MAX_HOST + 5/*PORT*/ + 1/*0*/ + MAX_HOST + 5/*PORT*/ + 10/*Margine*/,
					"CONNECT %s:%d HTTP/1.%d\r\nHost: %s:%d\r\n\r\n", host, port, 0, host, port);
			HxLOG_Debug("buf = %s\n", buf);
			om_drmagent_bhttpengine_internal_write(http, buf, HTTP_MAX_HDR_LINE);
			om_drmagent_bhttpengine_flush(http);

			bzero(buf, HTTP_MAX_HDR_LINE);
			line = om_drmagent_bhttpengine_read_line(http, buf, HTTP_MAX_HDR_LINE);

			if(line)
			{
				if (HxSTD_StrNCmp(line, "HTTP/1.", 7) >= 0) {
					find = HxSTD_StrChar(line, ' ');  if (!find) return (-1);
					line = find + 1; find = HxSTD_StrChar(line, ' ');	if (!find) return (-1);
					if((find - line) < (HINT32)sizeof(response)) {
						HxSTD_StrNCpy(response, line, find - line);
						responseCode = atoi(response);
						HxLOG_Debug("responseCode <%d>\n", responseCode);

						om_drmagent_bhttpengine_read_headers(http);
					}
				}
			}
			om_drmagent_bhttpengine_headers_cleanup(http);
		}
	}

	if(!HxSTD_memcmp(proto, "https", 5))
	{
		http->_isSecureSocket = 1;

		if(!om_drmagent_ssl_init(http))
		{
			HxLOG_Error("om_drmagent_ssl_init failed\n");
			om_drmagent_bhttpengine_close(http);
			return (-1);
		}

		if(!om_drmagent_ssl_connect(http))
		{
			HxLOG_Error("om_drmagent_ssl_connect failed\n");
			om_drmagent_bhttpengine_close(http);
			return (-1);
		}
	}

	return (0);
}

/* client function to send a request. specify method to be either 'get',
   'post' or 'put', and urlstr to specify the url
   */
	HINT32
om_drmagent_bhttpclient_send_request(bhttp_engine* http, const HCHAR *method, HCHAR *urlstr)
{
	bdrm_url url;
	const HCHAR *query;
	HCHAR *buf = HLIB_STD_MemAlloc(HTTP_MAX_HDR_LINE);
	HUINT32 len = 0;
	HUINT8 keepalive = HTTP_KEEP_ALIVE_OFF;
	HCHAR *proxy_url = NULL;

	om_drmagent_burl_print(urlstr);
	om_drmagent_burl_set(&url, urlstr);
	if (http->_currentFd == -1) {
		if (om_drmagent_bhttpclient_connect(http, url._host, url._protocol, url._port) == -1)
		{
			HxLOG_Error("bdrm_http_status_failed_connect\n");
			HLIB_STD_MemFree(buf);
			return (-1);
		}
	} else keepalive = HTTP_KEEP_ALIVE_ON;

	query = url._query; if (!query || !*query) { query = "/"; }
	proxy_url = getenv( "http_proxy" );
	if(proxy_url)
	{
		if(!HxSTD_memcmp(url._protocol, "https", 5))
		{
			len = snprintf(buf, HTTP_MAX_HDR_LINE, "%s %s HTTP/1.0\r\n", method, query);
		}
		else
		{
			len = snprintf(buf, HTTP_MAX_HDR_LINE, "%s %s HTTP/1.0\r\n", method, urlstr);
		}
	}
	else
	{
		len = snprintf(buf, HTTP_MAX_HDR_LINE, "%s %s HTTP/1.0\r\n", method, query);
	}

	HxLOG_Debug("%s:%s buf = %s\n", url._host, url._protocol, buf);
	if (om_drmagent_bhttpengine_internal_write(http, buf, len) < 0)
	{
		HLIB_STD_MemFree(buf);
		return (-1);
	}

	om_drmagent_bhttpengine_headers_cleanup(http);

	http->_wroteHeaders = 0; http->_readHeaders = 0;

	if (HTTP_KEEP_ALIVE_ON == keepalive)
		om_drmagent_bhttpengine_set_headers(http, "Connection", "Keep-Alive");

	om_drmagent_bhttpengine_set_headers(http, "HOST", url._host);

	HLIB_STD_MemFree(buf);
	return 0;
}

/* handling function for the headers coming inbound from the server
*/
	HINT32
om_drmagent_bhttpclient_read_responsehdr(bhttp_engine* http)
{
	HCHAR *line, *find;
	HCHAR *buf = HLIB_STD_MemAlloc(HTTP_MAX_HDR_LINE);
	HCHAR response[MAX_RESPONSE_NUM];

	if(buf == NULL)
	{
		HxLOG_Error("Out of Memory\n");
		return (-1);
	}

	HxSTD_MemSet( buf, 0, ( sizeof( HCHAR ) * HTTP_MAX_HDR_LINE ));
	HxSTD_MemSet( response, 0, ( sizeof( HCHAR ) * MAX_RESPONSE_NUM ));

	if (http->_readHeaders) {
		HxLOG_Error("Cannot read headers twice.\n");
		HLIB_STD_MemFree(buf);
		return (-1);
	}

	http->_readHeaders = 1;     /* read the top line */
	line = om_drmagent_bhttpengine_read_line(http, buf, HTTP_MAX_HDR_LINE);
	if (!line)
	{
		HLIB_STD_MemFree(buf);
		return (-1);
	}

	HxLOG_Debug("om_drmagent_bhttpengine_read_line <%s>, buf<%s>\n", line, buf);

	find = HxSTD_StrChar(line, ' ');
	if (!find)
	{
		HLIB_STD_MemFree(buf);
		return (-1);
	}
	line = find + 1;
	find = HxSTD_StrChar(line, ' ');
	if (!find)
	{
		HLIB_STD_MemFree(buf);
		return (-1);
	}
	if((find - line) < (HINT32)sizeof(response)) {
		HxSTD_StrNCpy(response, line, find - line);
		http->_responseCode = atoi(response);
	}

	HLIB_STD_MemFree(buf);
	return om_drmagent_bhttpengine_read_headers(http);
}

/* 'http get' wrapper function
*/
	HINT32
om_drmagent_bhttpclient_get(bhttp_engine* http, HCHAR *urlstr)
{
	HINT32 res;

	if ((res = om_drmagent_bhttpclient_send_request(http, "GET", urlstr)) != 0) return (res);
	if ((res = om_drmagent_bhttpengine_write_headers(http)) != 0) return (res);

	return om_drmagent_bhttpclient_read_responsehdr(http);
}

/* 'http post' wrapper function
*/
	HINT32
om_drmagent_bhttpclient_post(bhttp_engine* http, HCHAR *url)
{
	return om_drmagent_bhttpclient_send_request(http, "POST", url);
}

/* 'http put' wrapper function
*/
	HINT32
om_drmagent_bhttpclient_put(bhttp_engine* http, HCHAR *url)
{
	return om_drmagent_bhttpclient_send_request(http, "PUT", url);
}

/* drm license post function; feeds an XML challenge to a server, parses
   the response looking for the license section, and passes it back to
   the drm engine for processing.
   */
	HINT32
om_drmagent_bhttpclient_license_post_default (HCHAR* url, HCHAR* chall, HUINT8 non_quiet,
		HUINT32 app_sec, HCHAR *action, HUINT8** resp,
		HUINT8** resp_start, HUINT32* out_resp_len)
{
	bhttp_engine http;
	HINT32 post_ret;

	HINT32 len = 0, licresp_len = 0;
	HINT32 resp_len = HTTP_XMLRPC_BUFSIZE;
	HCHAR* licresp_opened = NULL, *licresp_closed = NULL;
	HCHAR* buf = NULL;
	HCHAR* new_resp = NULL;
	HCHAR *temp = NULL;
	HCHAR *tmpbuf = NULL;
	HINT32 i;

	(void)(action);
	buf = (HCHAR*)HLIB_STD_MemAlloc(resp_len+1);
	if (buf == NULL) return (-1);

	HxSTD_MemSet(buf, 0x00, resp_len+1);

	new_resp = (HCHAR*)HLIB_STD_MemAlloc(resp_len+1);
	if (new_resp == NULL)
	{
		HLIB_STD_MemFree(buf);
		return (-1);
	}
	HxSTD_MemSet(new_resp, 0x00, resp_len+1);

	/* append drm specific tokens */
	HxLOG_Debug("posting to : %s\n", url);

	len += snprintf(buf, HxSTD_StrLen(DRM_POST_PREFIX)+10/*MAX_app_sec*/+5/*MAX_non_quiet*/, DRM_POST_PREFIX, app_sec, non_quiet);
	len += snprintf(buf + len, HxSTD_StrLen(chall)+1, "%s", chall);

	/* initialize http engine and post */
	om_drmagent_bhttpengine_init(&http);
	if ((post_ret = om_drmagent_bhttpclient_post(&http, url)) != 0) {
		HxLOG_Error("bhttp_license_post failed on POST\n");
		HLIB_STD_MemFree(buf);
		HLIB_STD_MemFree(new_resp);
		return (post_ret);
	}

	/* set headers, read response */
	om_drmagent_bhttpengine_set_headers(&http, HDR_TAG_CONTENT_TYPE, HDR_VALUE_URLENCODED_CT);
	om_drmagent_bhttpengine_set_header(&http,  HDR_TAG_CONTENT_LENGTH, len);
	om_drmagent_bhttpengine_write(&http, buf, len); om_drmagent_bhttpengine_flush(&http);
	if (om_drmagent_bhttpclient_read_responsehdr(&http)) {
		HxLOG_Error("failed on readResponseHeaderv");
		HLIB_STD_MemFree(buf);
		HLIB_STD_MemFree(new_resp);
		return (-1);
	}

	/* look for a license */
	bzero(buf, resp_len); len = om_drmagent_bhttpengine_read(&http, buf, resp_len);
#if 1
	HxLOG_Debug("HTTP default post:: resp_len <%d>, buf<%s>\n", resp_len, buf);
#endif

	/*Sometimes the response from server contains '\' characters, which the playready XML parser cannot process.
	  This causes the failure to retrive the license from the response. Below we will check the entire response for '\' characters and
	  if found, remove them from the reaponse. The modifed final response is stored in the same location */
	tmpbuf = HxSTD_StrStr((HCHAR *)buf, "\\");

	while(tmpbuf != NULL)
	{
		HxSTD_StrNCpy(new_resp, buf, resp_len+1);
		temp = tmpbuf + 1;
		i = tmpbuf - buf;
		HxSTD_StrNCpy(buf, new_resp, i-1 );
		buf[i] = '\0';
		strncat(buf, temp, HxSTD_StrLen(temp));
		tmpbuf = HxSTD_StrStr((HCHAR *)buf, "\\");
	}
	HLIB_STD_MemFree(new_resp);

	/* Note that in the case of an XML license, the SDK expect the license buffer to be pointing to <LICENSERESPONSE> tag. */

	licresp_opened = HxSTD_StrStr((HCHAR*)buf, LICRESPONSE_OPENED);                    /* look for response open tag */
	licresp_closed = HxSTD_StrStr((HCHAR*)buf, LICRESPONSE_CLOSED);                    /* look for response close tag */

	HxLOG_Debug("print :: buf<%s>\n", buf);
	/* process response */
	if ((licresp_opened != NULL) && (licresp_closed != NULL)) {
		HxLOG_Debug("-------->LICENSE TAGS FOUND\n");
		licresp_len = licresp_closed - licresp_opened + sizeof(LICRESPONSE_CLOSED);
		if (resp_start) { *resp_start = (HUINT8*)licresp_opened; }       /* ptr to beginning of license response */
		if (resp != NULL) { *resp = (HUINT8*)buf; }                      /* ptr to start of buffer */
		if (out_resp_len != NULL) { *out_resp_len = licresp_len; }              /* length of license response */

		if ((len == -1) || (http._responseCode != HTTP_OK_RESPONSE)) {
			HxLOG_Error("failed on responseCode != %d\n", HTTP_OK_RESPONSE);
			HLIB_STD_MemFree(buf);
			return (-1);
		}
	}
	else
	{
		HxLOG_Error("-------->LICENSE TAGS NOT FOUND\n");
		HLIB_STD_MemFree(buf);
		return (-1);
	}


	return 0;
}

/* drm license post function; feeds a SOAP challenge to a server, parses
   the response looking for the license section, and passes it back to
   the drm engine for processing.
   */
	HINT32
om_drmagent_bhttpclient_license_post_soap (HCHAR* url, HCHAR* chall, HUINT8 non_quiet,
		HUINT32 app_sec, HCHAR *action, HUINT8** resp,
		HUINT8** resp_start, HUINT32* out_resp_len)
{
	bhttp_engine http;
	HINT32 post_ret;

	HCHAR *line, *find;
	HCHAR response[MAX_RESPONSE_NUM];

	HINT32 len = 0;
	/*HINT32 licresp_len = 0;*/
	HINT32 resp_len = HTTP_XMLRPC_BUFSIZE;
	/*char* licresp_opened = NULL, *licresp_closed = NULL;*/

	HCHAR *buf = HLIB_STD_MemAlloc(resp_len + 1);

	UNUSED_PARAM(non_quiet);
	UNUSED_PARAM(app_sec);

	if (buf == NULL) return (-1);

	/* append drm specific tokens */
	HxLOG_Debug("posting to : %s\n", url);
	len += snprintf(buf + len, HxSTD_StrLen(chall) + 1, "%s", chall);

	/* initialize http engine and post */
	om_drmagent_bhttpengine_init(&http);
	if ((post_ret = om_drmagent_bhttpclient_post(&http, url)) != 0) {
		HxLOG_Error("bhttp_license_post failed on POST\n");
		HLIB_STD_MemFree(buf);
		return (post_ret);
	}

	/* set headers, read response */
	om_drmagent_bhttpengine_set_headers(&http, HDR_TAG_SOAP_ACTION, action);
	om_drmagent_bhttpengine_set_headers(&http, HDR_TAG_CONTENT_TYPE, HDR_VALUE_XML_UTF8_CT);
	om_drmagent_bhttpengine_set_header(&http,  HDR_TAG_CONTENT_LENGTH, len);
	om_drmagent_bhttpengine_write(&http, buf, len); om_drmagent_bhttpengine_flush(&http);

	do
	{
		bzero(buf, resp_len);
		line = om_drmagent_bhttpengine_read_line(&http, buf, HTTP_MAX_HDR_LINE);

		if(line)
		{
			if (HxSTD_StrNCmp(line, "HTTP/1.", 7) >= 0) {
				find = HxSTD_StrChar(line, ' ');  if (!find) return (-1);
				line = find + 1; find = HxSTD_StrChar(line, ' ');	if (!find) return (-1);
				if((find - line) < (HINT32)sizeof(response)) {
					HxSTD_StrNCpy(response, line, find - line);
					http._responseCode = atoi(response);
					HxLOG_Debug("http._responseCode <%d>\n", http._responseCode);

					om_drmagent_bhttpengine_read_headers(&http);
				}
			}
			else
			{
				break;
			}
		}
	} while(line != NULL);

	/* look for a license */
	len = HxSTD_StrLen(buf);

	bzero(buf + len, resp_len - len);
	len += om_drmagent_bhttpengine_read(&http, buf+len, resp_len-len);
#if 1
	HxLOG_Debug("HTTP :: resp_len <%d>, buf<%s>\n", len, buf);
#endif

	*out_resp_len = len;
	*resp = (HUINT8*)buf;
	*resp_start = (HUINT8*)buf;

	om_drmagent_bhttpengine_headers_cleanup(&http);
	om_drmagent_bhttpengine_close(&http);

	return 0;
}

HINT32 om_drmagent_bhttpclient_license_post_soap_ex (HCHAR* url, HCHAR* chall, HUINT8 non_quiet,
		HUINT32 app_sec, HCHAR *action, HUINT8** resp,
		HUINT8** resp_start, HUINT32* out_resp_len, bhttp_hdr_indiv *customheader, HINT32 nCustomheaders)
{
	HINT32 i = 0;
	bhttp_engine http;
	HINT32 post_ret;
	HINT32 ret=0;
	HCHAR *line, *find;
	HCHAR response[MAX_RESPONSE_NUM];

	HINT32 len = 0;
	/*HINT32 licresp_len = 0;*/
	HINT32 resp_len = HTTP_XMLRPC_BUFSIZE;
	/*char* licresp_opened = NULL, *licresp_closed = NULL;*/

	HCHAR *buf = HLIB_STD_MemAlloc(resp_len + 1);

	UNUSED_PARAM(non_quiet);
	UNUSED_PARAM(app_sec);

	if (buf == NULL) return (-1);

	/* append drm specific tokens */
	HxLOG_Debug("posting to : %s\n", url);
	len += snprintf(buf + len, HxSTD_StrLen(chall) + 1, "%s", chall);

	/* initialize http engine and post */
	om_drmagent_bhttpengine_init(&http);
	if ((post_ret = om_drmagent_bhttpclient_post(&http, url)) != 0) {
		HxLOG_Error("bhttp_license_post failed on POST\n");
		HLIB_STD_MemFree(buf);
		return (post_ret);
	}

	/* set headers, read response */
	om_drmagent_bhttpengine_set_headers(&http, HDR_TAG_SOAP_ACTION, action);
	om_drmagent_bhttpengine_set_headers(&http, HDR_TAG_CONTENT_TYPE, HDR_VALUE_XML_UTF8_CT);
	om_drmagent_bhttpengine_set_header(&http,  HDR_TAG_CONTENT_LENGTH, len);
	if( customheader != NULL && nCustomheaders > 0)
	{
		for(i = 0 ; i < nCustomheaders; i++)
		{
			if(customheader[i].hdr_name && customheader[i].hdr_value)
			{
				om_drmagent_bhttpengine_set_headers(&http, customheader[i].hdr_name, customheader[i].hdr_value);
				HxLOG_Debug("Custom Header Name : %s , Value : %s\n",customheader[i].hdr_name,customheader[i].hdr_value);
			}
		}
	}
	om_drmagent_bhttpengine_write(&http, buf, len); om_drmagent_bhttpengine_flush(&http);

	do
	{
		bzero(buf, resp_len);
		line = om_drmagent_bhttpengine_read_line(&http, buf, HTTP_MAX_HDR_LINE);

		if(line)
		{
			if (HxSTD_StrNCmp(line, "HTTP/1.", 7) >= 0) {
				find = HxSTD_StrChar(line, ' ');  if (!find) return (-1);
				line = find + 1; find = HxSTD_StrChar(line, ' ');	if (!find) return (-1);
				if((find - line) < (HINT32)sizeof(response)) {
					HxSTD_StrNCpy(response, line, find - line);
					http._responseCode = atoi(response);
					HxLOG_Debug("http._responseCode <%d>\n", http._responseCode);

					om_drmagent_bhttpengine_read_headers(&http);
				}
			}
			else
			{
				break;
			}
		}
	} while(line != NULL);

	/* look for a license */
	len = HxSTD_StrLen(buf);

	bzero(buf + len, resp_len - len);
	len += om_drmagent_bhttpengine_read(&http, buf+len, resp_len-len);
#if 1
	HxLOG_Debug("HTTP :: resp_len <%d>, buf<%s>\n", len, buf);
#endif

	*out_resp_len = len;
	*resp = (HUINT8 *)buf;
	*resp_start = (HUINT8 *)buf;
	if(http._responseCode != 200)
		ret = -1;
	om_drmagent_bhttpengine_headers_cleanup(&http);
	om_drmagent_bhttpengine_close(&http);

	return ret;
}

void om_drmagent_bhttpclient_set_certificate(HINT8*pPrivateKeyPath, HINT8 *pCertificatePath)
{
	s_pPrivateKeyPath = pPrivateKeyPath;
	s_pCertificatePath = pCertificatePath;
}


