/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_core_curlapi.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_CURLAPI_H__
#define __NX_CORE_CURLAPI_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>

#include <curl/curl.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

#define NX_CORE_CURL_URL_LENGTH_MAX 128
#define NX_CORE_CURL_URL_POST_PARAM_LENGTH_MAX 128

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	nxCurlSo(func)		(NX_CORE_CURLAPI_GetInstance())->_##func

/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef struct
{
	HUINT32		ulSemId;			//	Semaphore가 필요없을것 같지만 나중을 대비해서 우선 만들어 놓음.

	void		*pvCurlSo;
	CURLcode	(*_curl_global_init)(long flags);
	void		(*_curl_global_cleanup)(void);
	CURL		*(*_curl_easy_init)(void);
	CURLcode	(*_curl_easy_setopt)(CURL *curl, CURLoption option, ...);
	void		(*_curl_easy_cleanup)(CURL *curl);
	CURLcode	(*_curl_easy_perform)(CURL *curl);
	CURLcode	(*_curl_easy_getinfo)(CURL *curl, CURLINFO info, ...);
	struct curl_slist	*(*_curl_slist_append)(struct curl_slist *, const char *);
	void		(*_curl_slist_free_all)(struct curl_slist *);

	CURLFORMcode	(*_curl_formadd)(struct curl_httppost **httppost, struct curl_httppost **last_post, ...);
	void		(*_curl_formfree)(struct curl_httppost *form);

    CURLM*      (*_curl_multi_init)(void);
    CURLMcode   (*_curl_multi_perform)(CURLM *multi_handle, int *running_handles);
    CURLMcode   (*_curl_multi_remove_handle)(CURLM *multi_handle, CURL *easy_handle);
    CURLMcode   (*_curl_multi_add_handle)(CURLM *multi_handle, CURL *easy_handle);
    CURLMcode   (*_curl_multi_wait)(CURLM *multi_handle,  struct curl_waitfd extra_fds[], unsigned int extra_nfds, int timeout_ms, int *numfds);
    CURLMcode   (*_curl_multi_fdset)(CURLM *multi_handle,fd_set *read_fd_set,fd_set *write_fd_set,fd_set *exc_fd_set,int *max_fd);
    CURLMsg*    (*_curl_multi_info_read)(CURLM *multi_handle,int *msgs_in_queue);
    CURLMcode   (*_curl_multi_cleanup)(CURLM *multi_handle );
    CURLMcode   (*_curl_multi_timeout)(CURLM *multi_handle, long *timeout);
} curl_t;

typedef struct
{
	HxTRANSPORT_Http_t 		*pHttp;
	HxTRANSPORT_Ftp_t		*pFtp;
	HINT8					*url;
	HBOOL					 bStop;
	HINT8					*path;
	HFLOAT64				 fDownloadSize;
}NxHttpContext_t;

typedef enum CURLAPI_METHOD
{
    eCurlReqMethodNone,
    eCurlReqMethodGET,
    eCurlReqMethodPOST,
    MAX_METHOD
} CurlApi_ReqMethod_e;

typedef struct NxCurlApiRes
{
    void*   reqParam; // What is This Data type?
    void*   resData; // response Data
    HINT32  resStatus;
} NxCurlApiRes;

typedef void (*CURL_RES_CALLBACK)(NxCurlApiRes* res);

typedef struct NxCurlApiReq
{
    CurlApi_ReqMethod_e eReqMethod;
    HCHAR   strUrl[NX_CORE_CURL_URL_LENGTH_MAX];
    HCHAR   strPostParam[NX_CORE_CURL_URL_POST_PARAM_LENGTH_MAX];

    void*   reqParam;
    CURL_RES_CALLBACK   pfnResCallback;
} NxCurlApiReq;

/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/

extern curl_t*	NX_CORE_CURLAPI_GetInstance(void);
HERROR	NX_HTTP_INIT(void);

extern HERROR NX_CORE_CURLAPI_DoMultiInit(void);
extern HERROR NX_CORE_CURLAPI_DoMultiTest(void);
extern HERROR NX_CORE_CURLAPI_Req(NxCurlApiReq *stReq);


#endif // __NX_CORE_CURLAPI_H__



