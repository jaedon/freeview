/**************************************************************************************/
/**
 * @file hxtransport_http.c
 * defines APIs's body
 *
 * APIs for the transport module using Http
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com), Inseon Son(isson@humaxdigital.com)
 * @date    2011/01/24
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
**************************************************************************************
**/
#include <hlib.h>
#include "_hlib_transport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

#include "hlib_transport_int.h"

#define HxTRANSPORT_MAX_BUF_SIZE			(1024*100)
#define HxTRANSPORT_HTTP_TIMEOUT_SEC	(30)
#define HxTRANSPORT_HTTP_STREAM_TIMEOUT 30

/*******************************************************************************************
 * Definitions
 ******************************************************************************************/
#undef	HxLOG_DOMAIN
#define HxLOG_DOMAIN	DOMAIN_TVSVC_PAL_TRANSPORT

typedef enum {
	eHxHTTP_STATE_IDLE,
	eHxHTTP_STATE_DOWNLOADING,
	eHxHTTP_STATE_DOWNLOAD_FAILED,
	eHxHTTP_STATE_DOWNLOAD_STOPPED,
	eHxHTTP_STATE_DOWNLOAD_FINISHED,
} HxHttpState_e;

typedef struct {
	HINT8		**buf;
	HINT8		**header;
	FILE		*fp;
	HFLOAT64	downloadedSize;
	HBOOL		bStop;
	const HINT8 *szFilePath;
	HUINT32 http_get_size;
	HUINT32 http_get_max_size;
	HUINT32 http_get_header_size;
	HUINT32 http_get_header_max_size;
	HxHttpState_e eState;
	HxHttp_callback callback;
	void *handle;
	HINT32 calltime;
} HxTransport_HttpFile_t;

typedef struct {
	CURL	*curl;
	HxTransport_HttpFile_t *fileinfo;
} HxTransport_HttpHandle_t;


/*******************************************************************************************
 * Static
 ******************************************************************************************/
inline static CURL *__curl_handle(HxTRANSPORT_Http_t *ptHttp)
{
	return (CURL *)((HxTransport_HttpHandle_t *)ptHttp)->curl;
}

inline static HxTransport_HttpHandle_t *__http_handle(HxTRANSPORT_Http_t *ptHttp)
{
	return (HxTransport_HttpHandle_t *)ptHttp;
}

inline static HxTRANSPORT_Httpheader_t *__tp_httphdr(struct curl_slist *slist)
{
	return (HxTRANSPORT_Httpheader_t *)slist;
}

inline static struct curl_slist *__tp_curlhttphdr(HxTRANSPORT_Httpheader_t *ptHttpHeader)
{
	return (struct curl_slist *)ptHttpHeader;
}


inline static HxTRANSPORT_Httpform_t **__tp_httppost(struct curl_httppost **form)
{
	return (HxTRANSPORT_Httpform_t **)form;
}

inline static struct curl_httppost **__tp_curlhttppost(HxTRANSPORT_Httpform_t **ptForm)
{
	return (struct curl_httppost **)ptForm;
}

static size_t __http_download_callback(void *buffer, size_t size, size_t nmemb, void *stream)
{
	HxTransport_HttpFile_t *ptFile = (HxTransport_HttpFile_t *)stream;

	if ( !ptFile )
		return -1;

	if (!ptFile->fp )
	{
		ptFile->fp = fopen((const char *)ptFile->szFilePath, "wb");
		if ( !ptFile->fp )
			return -1;

		ptFile->eState = eHxHTTP_STATE_DOWNLOADING;
	}

	if ( ptFile->bStop == TRUE )
	{
		fclose(ptFile->fp);
		ptFile->fp = NULL;
		(void)remove(ptFile->szFilePath);
		ptFile->eState = eHxHTTP_STATE_DOWNLOAD_STOPPED;
		return -1;
	}

	ptFile->downloadedSize += ((HINT32)size*(HINT32)nmemb);
	ptFile->calltime += 1;

	if(ptFile->calltime > 5)
	{
		ptFile->calltime = 0;
		if(ptFile->callback) (* ptFile->callback)(eHxTRANSPORT_EVENT_PROGRESS_CHANGED, (HINT32)ptFile->handle, (HINT32)ptFile->downloadedSize);
	}

	return fwrite(buffer, size, nmemb, ptFile->fp);
}

static size_t get_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	HxTransport_HttpFile_t *ptHandle = (HxTransport_HttpFile_t *)stream;
	const HUINT32 ulDataSize = size * nmemb;

	if ( (ptHandle->http_get_size + ulDataSize) >= ptHandle->http_get_max_size )
	{
		HINT8 *pTempBuf = (HINT8 *)HLIB_MEM_Realloc(*(ptHandle->buf),
								(ptHandle->http_get_size+ulDataSize)*2);
		if ( !pTempBuf )
		{
			HLIB_MEM_Free(*(ptHandle->buf));
			return 0;
		}
		else
		{
			*(ptHandle->buf) = pTempBuf;
		}

		ptHandle->http_get_max_size = (ptHandle->http_get_size+ulDataSize)*2;
	}

	memcpy(*(ptHandle->buf)+ptHandle->http_get_size, ptr, ulDataSize);
	ptHandle->http_get_size += ulDataSize;

	return ulDataSize;
}
static size_t get_header_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	HxTransport_HttpFile_t *ptHandle = (HxTransport_HttpFile_t *)stream;
	const HUINT32 ulDataSize = size * nmemb;

	if ( (ptHandle->http_get_header_size + ulDataSize) >= ptHandle->http_get_header_max_size )
	{
		HINT8 *pTempBuf = (HINT8 *)HLIB_MEM_Realloc(*(ptHandle->header),
								(ptHandle->http_get_header_size+ulDataSize)*2);
		if ( !pTempBuf )
		{
			HLIB_MEM_Free(*(ptHandle->header));
			return 0;
		}
		else
		{
			*(ptHandle->header) = pTempBuf;
		}

		ptHandle->http_get_header_max_size = (ptHandle->http_get_header_size+ulDataSize)*2;
	}

	memcpy(*(ptHandle->header)+ptHandle->http_get_header_size, ptr, ulDataSize);
	ptHandle->http_get_header_size += ulDataSize;

	return ulDataSize;
}

/*******************************************************************************************
 * Interfaces
 ******************************************************************************************/
void HLIB_TRANSPORT_HttpInit(void)
{
	curlSo(curl_global_init(CURL_GLOBAL_ALL));
}

void HLIB_TRANSPORT_HttpDestory(void)
{
	curlSo(curl_global_cleanup());
}

void HLIB_TRANSPORT_HttpSetListener(HxTRANSPORT_Http_t *handle, HxHttp_callback callback, void *hAct)
{
	if ( callback )
	{
		__http_handle(handle)->fileinfo->callback = callback;
		__http_handle(handle)->fileinfo->handle = hAct;
	}
}

HxTRANSPORT_Http_t *HLIB_TRANSPORT_HttpOpen(void)
{
	HxTransport_HttpHandle_t *pHandle = NULL;
	CURL *ctx = NULL;

	pHandle = (HxTransport_HttpHandle_t *)HLIB_MEM_Malloc(sizeof(HxTransport_HttpHandle_t));

	pHandle->fileinfo = (HxTransport_HttpFile_t *)HLIB_MEM_Malloc(sizeof(HxTransport_HttpFile_t));
	if ( !pHandle->fileinfo )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Creating the dummy filedata is failed.\n",
				__FUNCTION__);
		HLIB_MEM_Free(pHandle);
		return NULL;
	}

	pHandle->fileinfo->buf = NULL;
	pHandle->fileinfo->fp = NULL;
	pHandle->fileinfo->downloadedSize = 0;
	pHandle->fileinfo->bStop = FALSE;
	pHandle->fileinfo->szFilePath = NULL;
	pHandle->fileinfo->http_get_size = 0;
	pHandle->fileinfo->http_get_max_size = 0;
	pHandle->fileinfo->callback = NULL;
	pHandle->fileinfo->calltime = 0;
	pHandle->fileinfo->eState = eHxHTTP_STATE_IDLE;

	ctx = curlSo(curl_easy_init());
	if ( !ctx )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Open error.\n", __FUNCTION__);
		HLIB_MEM_Free(pHandle->fileinfo);
		HLIB_MEM_Free(pHandle);
		return NULL;
	}

#if defined(CONFIG_DEBUG)
	/* turn off the debugging. if you want to display msg of curl, chane 0L -> 1L */
	curlSo(curl_easy_setopt(ctx, CURLOPT_VERBOSE, 0L));
#endif
	curlSo(curl_easy_setopt(ctx, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4));

	/* signal problem, So set NOSIGNAL */
	curlSo(curl_easy_setopt(ctx, CURLOPT_NOSIGNAL, 1L));

	pHandle->curl = ctx;

	return  (HxTRANSPORT_Http_t *)pHandle;
}

void HLIB_TRANSPORT_HttpClose(HxTRANSPORT_Http_t *ptHttp)
{
	if ( ptHttp )
	{
		curlSo(curl_easy_cleanup(__curl_handle(ptHttp)));

		if ( __http_handle(ptHttp)->fileinfo )
		{
			HLIB_MEM_Free(__http_handle(ptHttp)->fileinfo);
		}

		HLIB_MEM_Free(__http_handle(ptHttp));
	}
}

HERROR HLIB_TRANSPORT_HttpSetUserPwd(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUserpwd)
{
	CURLcode curlRet = CURLE_OK;

	if ( !ptHttp || !szUserpwd )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_USERPWD, szUserpwd));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: UserPwd setting error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpSetCertificateFile(HxTRANSPORT_Http_t *ptHttp,
							const HINT8 *szCertFile, HxTRANSPORT_Certtype_e certType,
							const HINT8 *szPassPharse)
{
	CURLcode curlRet 	= CURLE_OK;
	HINT8 *szCertType	= NULL;

	if ( !ptHttp || !szCertFile )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	switch (certType)
	{
		case eHxTP_HTTP_CERTIFICATE_DER:
			szCertType = "DER";
			break;
		case eHxTP_HTTP_CERTIFICATE_PEM:
			szCertType = "PEM";
			break;
		case eHxTP_HTTP_CERTIFICATE_P12:
			szCertType = "P12";
			break;
		default:
			szCertType = "PEM";
			break;
	}

	/* cert is stored PEM or DER coded in file... */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_SSLCERTTYPE, szCertType));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid certificate type.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* set the cert for client authentication */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_SSLCERT, szCertFile));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid certificate file.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* if the certificate has a pass pharse, you have to set it to the Http trnasport */
	if ( szPassPharse )
	{
		curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_KEYPASSWD, szPassPharse));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid certificate key password.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpSetCertificateKeyFile(HxTRANSPORT_Http_t *ptHttp,
							const HINT8 *szCertKeyFile, HxTRANSPORT_Certtype_e certType)
{
	CURLcode curlRet 	= CURLE_OK;
	HINT8 *szCertType	= NULL;

	if ( !ptHttp || !szCertKeyFile )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	switch (certType)
	{
		case eHxTP_HTTP_CERTIFICATE_DER:
			szCertType = "DER";
			break;
		case eHxTP_HTTP_CERTIFICATE_PEM:
			szCertType = "PEM";
			break;
		case eHxTP_HTTP_CERTIFICATE_P12:
			szCertType = "P12";
			break;
		default:
			szCertType = "PEM";
			break;
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_SSLKEYTYPE, szCertType));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalild certificate type.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* set the private key */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_SSLKEY, szCertKeyFile));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalild certificate key file.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpSetCACertificateFile(HxTRANSPORT_Http_t *ptHttp,
							const HINT8 *szCACertFile)
{
	CURLcode curlRet	= CURLE_OK;

	if ( !ptHttp || !szCACertFile )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	/* set the file with the certs vaildating the server */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_CAINFO, szCACertFile));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid CA certificate file.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpSetPeerVeryfication(HxTRANSPORT_Http_t *ptHttp,
							HBOOL bVerify)
{
	CURLcode curlRet	= CURLE_OK;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_SSL_VERIFYPEER, bVerify));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid CA varification \n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}
	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpSetHostVeryfication(HxTRANSPORT_Http_t *ptHttp,
							HBOOL bVerify)
{
	CURLcode curlRet	= CURLE_OK;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_SSL_VERIFYHOST, bVerify));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid CA varification \n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}
	return HxTRANSPORT_RETURN(eHxTPE_OK);
}


HxTRANSPORT_Httpheader_t *HLIB_TRANSPORT_HttpAppendHeader(
								HxTRANSPORT_Httpheader_t *ptHttpHeader,
								const HINT8 *szHeader)
{
	if ( !szHeader )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
		return NULL;
	}

	ptHttpHeader = ( HxTRANSPORT_Httpheader_t *)curlSo(curl_slist_append(
			__tp_curlhttphdr(ptHttpHeader), szHeader));

	return ptHttpHeader;
}

HERROR HLIB_TRANSPORT_HttpFreeHeader(HxTRANSPORT_Httpheader_t *ptHttpHeader)
{
	curlSo(curl_slist_free_all(__tp_curlhttphdr(ptHttpHeader)));

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpSetHeaderToTP(HxTRANSPORT_Http_t *ptHttp,
								HxTRANSPORT_Httpheader_t *ptHttpHeader)
{
	CURLcode curlRet	= CURLE_OK;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_HTTPHEADER,
			__tp_curlhttphdr(ptHttpHeader)));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Adding a HTTP header is failed.\n",
				__FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpGet(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri,
								HINT8 **ppcBuf, HFLOAT64 *pflDownloadSize)
{
	CURLcode curlRet = CURLE_OK;
	HxTransport_HttpHandle_t *pHttp = __http_handle(ptHttp);


	if ( !ptHttp || !szUri || !ppcBuf )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* the progress bar option will be disabled */
	curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_NOPROGRESS , 1L));

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_WRITEFUNCTION, get_callback));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid callback type.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	*ppcBuf = HLIB_MEM_Malloc(HxTRANSPORT_MAX_BUF_SIZE);
	if ( *ppcBuf == NULL )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid buffer.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_OUT_OF_MEMORY);
	}

	HxSTD_MemSet(*ppcBuf, 0x00, HxTRANSPORT_MAX_BUF_SIZE);

	pHttp->fileinfo->buf = ppcBuf;
	pHttp->fileinfo->http_get_max_size = HxTRANSPORT_MAX_BUF_SIZE;
	pHttp->fileinfo->http_get_size = 0;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_WRITEDATA, __http_handle(ptHttp)->fileinfo));
	if ( curlRet != CURLE_OK )
	{
		if ( *ppcBuf )
		{
			HLIB_MEM_Free(*ppcBuf);
			*ppcBuf =  NULL;
			 pHttp->fileinfo->buf = NULL;
		}

		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid buffer.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* receive a data from a server */
	curlRet = curlSo(curl_easy_perform(__curl_handle(ptHttp)));
	if ( curlRet != CURLE_OK )
	{
		if ( *ppcBuf )
		{
			HLIB_MEM_Free(*ppcBuf);
			*ppcBuf = NULL;
  			 pHttp->fileinfo->buf = NULL;
		}

		HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a HTTP data is failed.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	if ( pflDownloadSize )
	{
		curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptHttp), CURLINFO_SIZE_DOWNLOAD,
					pflDownloadSize));
		if ( curlRet != CURLE_OK )
		{
			if ( *ppcBuf )
			{
				HLIB_MEM_Free(*ppcBuf);
				*ppcBuf = NULL;
  			 	pHttp->fileinfo->buf = NULL;
			}

			HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a size of HTTP data is failed.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

void HLIB_TRANSPORT_HttpFreeBuffer(HINT8 *pBuf)
{
	if ( pBuf )
	{
		HLIB_MEM_Free(pBuf);
	}
}

HERROR HLIB_TRANSPORT_HttpGetWithCallback(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri,
								HxHttpGet_Callback *callback, HFLOAT64 *pflDownloadSize)
{
	CURLcode curlRet = CURLE_OK;

	if ( !ptHttp || !szUri || !callback )
	{
		HxLOG_Debug("[TRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* the progress bar option will be disabled */
	curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_NOPROGRESS , 1L));

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_WRITEFUNCTION, callback));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid callback type.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* receive a data from a server */
	curlRet = curlSo(curl_easy_perform(__curl_handle(ptHttp)));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a HTTP data is failed.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	if ( pflDownloadSize )
	{
		curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptHttp), CURLINFO_SIZE_DOWNLOAD,
					pflDownloadSize));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a size of HTTP data is failed.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpGetHeaderAndBody (HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri
															, HINT8 **ppcHeader, HUINT32 ulMaxHeaderSize
															, HINT8 **ppcBuf, HUINT32 ulMaxBufSize
															, HFLOAT64 *pflDownloadSize)
{
	CURLcode curlRet = CURLE_OK;
	HxTransport_HttpHandle_t *pHttp = __http_handle(ptHttp);


	if ( !ptHttp || !szUri || !ppcBuf || !ppcHeader || !*ppcBuf || !*ppcHeader)
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HLIB_TRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* A parameter set to 1 tells the library to follow any Location: header that the server sends as part of a HTTP header  */
	curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_FOLLOWLOCATION , 1L));
	/* the progress bar option will be disabled */
//	curlSo(url_easy_setopt(__curl_handle(ptHttp), CURLOPT_NOPROGRESS , 1L));

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_HEADERFUNCTION, get_header_callback));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid callback type.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	pHttp->fileinfo->header = ppcHeader;
	pHttp->fileinfo->http_get_header_max_size = ulMaxHeaderSize;
	pHttp->fileinfo->http_get_header_size = 0;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_HEADERDATA, __http_handle(ptHttp)->fileinfo));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid buffer.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_WRITEFUNCTION, get_callback));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid callback type.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	pHttp->fileinfo->buf = ppcBuf;
	pHttp->fileinfo->http_get_max_size = ulMaxBufSize;
	pHttp->fileinfo->http_get_size = 0;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_WRITEDATA, __http_handle(ptHttp)->fileinfo));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid buffer.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* receive a data from a server */
	curlRet = curlSo(curl_easy_perform(__curl_handle(ptHttp)));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a HTTP data is failed.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	if ( pflDownloadSize )
	{
		curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptHttp), CURLINFO_SIZE_DOWNLOAD,
					pflDownloadSize));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a size of HTTP data is failed.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

#define HTTP_RETRY_MAX_NUM 5
HERROR HLIB_TRANSPORT_HttpGetAndStore(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri,
								const HINT8 *szDownlaodPath, HFLOAT64 *pflDownloadSize)
{
	CURLcode curlRet	= CURLE_OK;
	HINT32 nHttpCode	= 0;
	HINT32	i;

	if ( !ptHttp || !szUri || !szDownlaodPath )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* A parameter set to 1 tells the library to follow any Location: header that the server sends as part of a HTTP header  */
	curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_FOLLOWLOCATION , 1L));

	/* Check for url validation before file download */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_NOBODY, 1L));
	if(curlRet == CURLE_OK)
	{
		curlRet = curlSo(curl_easy_perform(__curl_handle(ptHttp)));
		curlRet |= curlSo(curl_easy_getinfo(__curl_handle(ptHttp), CURLINFO_HTTP_CODE, (long *)&nHttpCode));
		if(curlRet != CURLE_OK)
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Cannot retrieve http header : Error code [%d].\n", __FUNCTION__, curlRet);
			return HxTRANSPORT_RETURN(curlRet);
		}
		else if(nHttpCode != eHxTP_HTTP_RESCODE_OK)
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: HTTP response : [%d].\n", __FUNCTION__, nHttpCode);
			return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
		}
	}
	curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_NOBODY, 0L));

	/* the progress bar option will be disabled */
	curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_NOPROGRESS , 1L));

	/* set the callback function to the Http transport module */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_WRITEFUNCTION, __http_download_callback));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid callback function.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	__http_handle(ptHttp)->fileinfo->szFilePath = szDownlaodPath;

	/* set the file handle to the HTTP transport module */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_WRITEDATA , __http_handle(ptHttp)->fileinfo));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid file stream.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_LOW_SPEED_LIMIT,1));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid file stream.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_LOW_SPEED_TIME,HxTRANSPORT_HTTP_STREAM_TIMEOUT));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid file stream.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_SSL_VERIFYPEER, 0L));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: ssl verifypeer.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_SSL_VERIFYHOST, 0L));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: ssl verityhost.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	for(i=0; i<HTTP_RETRY_MAX_NUM; i++)
	{
		if( __http_handle(ptHttp)->fileinfo->eState != eHxHTTP_STATE_DOWNLOAD_STOPPED )
		{
			/* receive a data from a server */
			curlRet = curlSo(curl_easy_perform(__curl_handle(ptHttp)));
			if ( curlRet != CURLE_OK )
			{
				double dl_size = 0;
				struct stat st={0};
				HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a HTTP data is failed.\n", __FUNCTION__);
				curlSo(curl_easy_getinfo(__curl_handle(ptHttp), CURLINFO_CONTENT_LENGTH_DOWNLOAD, &dl_size));
				if( dl_size!=(-1) && !stat(szDownlaodPath,&st) )
				{
					if( st.st_size != dl_size )
					{
						HxLOG_Debug(" st.st_size = %d \n",st.st_size);
						curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_RESUME_FROM,st.st_size));
						HxLOG_Debug("[%s:%d] ===retry_perform == \n",__FUNCTION__,__LINE__);
						HLIB_STD_TaskSleep(200);
					}
				}

			}
			else
			{
				// Success
				break;
			}
		}
	}

	if( curlRet != CURLE_OK )
	{
		return HxTRANSPORT_RETURN(curlRet);
	}

	if( __http_handle(ptHttp)->fileinfo->callback) (* __http_handle(ptHttp)->fileinfo->callback)(eHxTRANSPORT_EVENT_PROGRESS_CHANGED,
		(HINT32)__http_handle(ptHttp)->fileinfo->handle, (HINT32)__http_handle(ptHttp)->fileinfo->downloadedSize );

	__http_handle(ptHttp)->fileinfo->eState =eHxHTTP_STATE_DOWNLOAD_FINISHED;

	if ( pflDownloadSize )
	{
		curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptHttp), CURLINFO_SIZE_DOWNLOAD,
					pflDownloadSize));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a size of HTTP data is failed.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
	}

	if ( __http_handle(ptHttp)->fileinfo->fp )
	{
		fclose(__http_handle(ptHttp)->fileinfo->fp);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}


HERROR HLIB_TRANSPORT_HttpDownloadStop(HxTRANSPORT_Http_t *ptHttp, HBOOL *bStop)
{
	if(ptHttp == NULL)
	{
		return ERR_FAIL;
	}

	if(__http_handle(ptHttp)->fileinfo)
	{
		__http_handle(ptHttp)->fileinfo->bStop = TRUE;
	}

	if(bStop)
		*bStop = TRUE;

	return ERR_OK;
}

HERROR HLIB_TRANSPORT_HttpPut(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri,
								const HINT8 *szFilepath, HFLOAT64 *pflUploadSize)
{
	CURLcode curlRet	= CURLE_OK;
	FILE *curlFile		= NULL;

	if ( !ptHttp || !szUri )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlFile = fopen(szFilepath, "rb");
	if ( !curlFile )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid a file path.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_FILE_OPEN_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		fclose(curlFile);
		return HxTRANSPORT_RETURN(curlRet);
	}

    /* enable uploading */
    curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_UPLOAD, 1L));

    /* HTTP PUT please */
    curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_PUT, 1L));

	/* now specify which file to upload */
    curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_READDATA, curlFile));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid file stream.\n", __FUNCTION__);
		fclose(curlFile);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* put data to a server */
	curlRet = curlSo(curl_easy_perform(__curl_handle(ptHttp)));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Putting a HTTP data is failed.\n", __FUNCTION__);
		fclose(curlFile);
		return HxTRANSPORT_RETURN(curlRet);
	}

	if ( pflUploadSize )
	{
		curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptHttp), CURLINFO_SIZE_UPLOAD,
					pflUploadSize));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a size of HTTP data is failed.\n",
					__FUNCTION__);
			fclose(curlFile);
			return HxTRANSPORT_RETURN(curlRet);
		}
	}

	fclose(curlFile);
	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpAddFormforFile(HxTRANSPORT_Httpform_t **ptFirst,
								HxTRANSPORT_Httpform_t **ptLast,
								const HINT8 *szCopyName, const HINT8 *szFilePath)
{
	CURLFORMcode curlRet	= 0;

	curlRet = curlSo(curl_formadd(__tp_curlhttppost(ptFirst), __tp_curlhttppost(ptLast),
				CURLFORM_COPYNAME, szCopyName, CURLFORM_FILE, szFilePath, CURLFORM_END));

	if ( curlRet != (CURLFORMcode)CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Adding a form for HTTP POST is failed.\n",
				__FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_HTTP_FORM_ERROR);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpAddFormforContents(HxTRANSPORT_Httpform_t **ptFirst,
								HxTRANSPORT_Httpform_t **ptLast,
								const HINT8 *szCopyName, const HINT8 *szContents)
{
	CURLFORMcode curlRet	= 0;

	curlRet = curlSo(curl_formadd(__tp_curlhttppost(ptFirst), __tp_curlhttppost(ptLast),
				CURLFORM_COPYNAME, szCopyName, CURLFORM_COPYCONTENTS,
				szContents, CURLFORM_END));
	if ( curlRet != (CURLFORMcode)CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Adding a form for HTTP POST is failed.\n",
				__FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_HTTP_FORM_ERROR);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpFreeForm(HxTRANSPORT_Httpform_t *ptForm)
{
	curlSo(curl_formfree((struct curl_httppost *)ptForm));

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpSetPostformToTP(HxTRANSPORT_Http_t *ptHttp,
								HxTRANSPORT_Httpform_t *ptHttpform)
{
	CURLcode curlRet 		= CURLE_OK;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_HTTPPOST,
				__tp_curlhttppost(ptHttpform)));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Setting a form data is failed.\n",
				__FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_HTTP_FORM_ERROR);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpHead (HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri
															, HINT8 **ppcHeader, HUINT32 ulMaxHeaderSize)
{
	CURLcode curlRet = CURLE_OK;
	HxTransport_HttpHandle_t *pHttp = __http_handle(ptHttp);


	if ( !ptHttp || !szUri || !ppcHeader ||  !*ppcHeader)
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HLIB_TRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/*CURLOPT_NOBODY: TRUE to exclude the body from the output. Request method is then set to HEAD */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_NOBODY, TRUE));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HLIB_TRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* A parameter set to 1 tells the library to follow any Location: header that the server sends as part of a HTTP header  */
	curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_FOLLOWLOCATION , 1L));
	/* the progress bar option will be disabled */
//	curlSo(url_easy_setopt(__curl_handle(ptHttp), CURLOPT_NOPROGRESS , 1L));

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_HEADERFUNCTION, get_header_callback));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid callback type.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	pHttp->fileinfo->header = ppcHeader;
	pHttp->fileinfo->http_get_header_max_size = ulMaxHeaderSize;
	pHttp->fileinfo->http_get_header_size = 0;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_HEADERDATA, __http_handle(ptHttp)->fileinfo));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid buffer.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* receive a data from a server */
	curlRet = curlSo(curl_easy_perform(__curl_handle(ptHttp)));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a HTTP data is failed.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpPost(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri,
					const HINT8 *szData, HUINT32 ulDataSize)
{
	CURLcode curlRet	= CURLE_OK;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	/* the progress bar option will be disabled */
	curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_NOPROGRESS , 1L));

	if ( szData != NULL )
	{
		curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_POSTFIELDS, szData));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Setting a POST fields is failed.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
		curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_POSTFIELDSIZE,
					ulDataSize));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Setting a POST fields  size is failed.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
	}

	/* post data to a server */
	curlRet = curlSo(curl_easy_perform(__curl_handle(ptHttp)));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: HTTP POST is failed..\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpPostAndRecvResponse(HxTRANSPORT_Http_t *ptHttp,
			const HINT8 *szUri, const HINT8 *szData, HUINT32 ulDataSize,
								HINT8 **ppcBuf, HFLOAT64 *pflDownloadSize)
{
	CURLcode curlRet	= CURLE_OK;

	if ( !ptHttp || !szUri || !ppcBuf )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	/* the progress bar option will be disabled */
	curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_NOPROGRESS , 1L));

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_WRITEFUNCTION,
					get_callback));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid callback type.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	*ppcBuf = HLIB_MEM_Malloc(HxTRANSPORT_MAX_BUF_SIZE);
	if ( *ppcBuf == NULL )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid buffer.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_OUT_OF_MEMORY);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_WRITEDATA, ppcBuf));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Invalid buffer.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	if ( szData != NULL )
	{
		curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_POSTFIELDS, szData));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Setting a POST fields is failed.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
		curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_POSTFIELDSIZE,
				ulDataSize));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Setting POST fields size is failed.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
	}

	/* post data to a server */
	curlRet = curlSo(curl_easy_perform(__curl_handle(ptHttp)));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: HTTP POST is failed..\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	if ( pflDownloadSize )
	{
		curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptHttp), CURLINFO_SIZE_DOWNLOAD,
					pflDownloadSize));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting size of HTTP data is failed.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpGetHttpResponseCode(HxTRANSPORT_Http_t *ptHttp,
								HxTRANSPORT_HttpResponse_e *peResCode)
{
	CURLcode curlRet	= CURLE_OK;
	HUINT32 ulHttpCode 	= 0;

	if ( !ptHttp || !peResCode )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptHttp), CURLINFO_HTTP_CODE, &ulHttpCode));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Getting a HTTP resonpse code is failed.\n",
				__FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	if ( peResCode ) *peResCode = ulHttpCode;

	HxLOG_Debug("[HxTRANSPORT][%s]: HTTP Response code = %x.\n", __FUNCTION__, ulHttpCode);

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpSetTimeout(HxTRANSPORT_Http_t *ptHttp, HUINT32 ulTimeoutSec)
{
	CURLcode curlRet	= CURLE_OK;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptHttp), CURLOPT_TIMEOUT, ulTimeoutSec));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Setting the maximu time is failed.\n",
				__FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_HttpDownloadedSize(HxTRANSPORT_Http_t *ptHttp, HFLOAT64*pSize)
{
	if ( pSize == NULL || __http_handle(ptHttp)->fileinfo->eState != eHxHTTP_STATE_DOWNLOADING)
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	*pSize = __http_handle(ptHttp)->fileinfo->downloadedSize;

	return ERR_OK;
}

HERROR HLIB_TRANSPORT_HttpHttpGetFileSize(HxTRANSPORT_Http_t *ptHttp, HFLOAT64 *pflFileSize)
{
	CURLcode curlRet	= CURLE_OK;
	HFLOAT64 filesize = 0;

	if ( ptHttp == NULL || pflFileSize == NULL || __http_handle(ptHttp)->fileinfo->eState != eHxHTTP_STATE_DOWNLOADING)
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptHttp), CURLINFO_CONTENT_LENGTH_DOWNLOAD, &filesize));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Debug("[HxTRANSPORT][%s] Error: Downloading a HTTP data is failed.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	*pflFileSize = filesize;

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}


