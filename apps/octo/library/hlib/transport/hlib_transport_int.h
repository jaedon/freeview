/*************************************************************
 * @file		hlib_transport_int.h
 * @date		2012/08/10
 * @author		humax
 * @brief		Internal Header for Curl

 * http://www.humaxdigital.com
 *************************************************************/
#include <htype.h>
#include <curl/curl.h>

#ifndef	__HLIB_TRANSPORT_INT_H__
#define	__HLIB_TRANSPORT_INT_H__

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
} curl_t;

#define	curlSo(func)		(HLIB_TRANSPORT_GetInstance())->_##func

extern curl_t*	HLIB_TRANSPORT_GetInstance(void);

#endif

