
/**
  @file	  port_itk_http.c														\n
  @brief	  itk file for HTTP porting layer							\n

Description: ITK에서 HTTP Porting 에 사용되는 기능을 정의한 Body파일	\n
Module: mheg	/itk								 						\n

Copyright (c) 2008 HUMAX Co., Ltd.											\n
All rights reserved.														\n

[Description]

*/

/*
 ** This implementation of the HTTP interface uses curl.
 ** This example implementation of libcurl adaptation code is known to work
 ** with version 7.19.5.
 **
 ** This product includes software developed by the OpenSSL Project
 ** for use in the OpenSSL Toolkit. (http://www.openssl.org/)
 **
 ** To fully implement the itk_http API the setInteractionChannelStatus function
 ** will need to be implemented in order to signal that the availability of the
 ** interaction channel has changed.
 */

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <vkernel.h>

#include <openssl/x509v3.h>
#include <openssl/ssl.h>
#include "curl/curl.h"

#include "port_itk_main.h"
#include "port_itk_console.h"
#include "port_itk_http.h"


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
#define ItkTrace(message)		ItkLogTrace(ITK_LOG_HTTP, message)
#define ItkPrint(message)		ItkLogPrint(ITK_LOG_HTTP, message)
#define ItkData(message)		ItkLogData(ITK_LOG_HTTP, message)
#define ItkAssert(message) 		ItkLogAssert(ITK_LOG_HTTP, message)
#define ItkInfo(message)		ItkLogInfo(ITK_LOG_HTTP, message)
#define ItkWarn(message)		ItkLogWarn(ITK_LOG_HTTP, message)
#define ItkError(message) 		ItkLogError(ITK_LOG_HTTP, message)
#define ItkFatal(c) 			ItkLogFatal(ITK_LOG_HTTP, c)
#else
#define ItkTrace(message)
#define ItkPrint(message)
#define ItkData(message)
#define ItkAssert(message)
#define ItkInfo(message)
#define ItkWarn(message)
#define ItkError(message)
#define ItkFatal(c)
#endif

#define HTTP_HEAP_SIZE          (1*1024*1024)           /**< Memory allocation for cURL and this adaptation implementation */
#define HTTP_THREAD_STACK_SIZE  (64*1024)               /**< HTTP thread stack size */
#define HTTP_THREAD_PRIORITY    VK_TASK_PRIORITY_MW_REF // ITK_PRIORITY_HIGH
#define HTTP_TIMEOUT            40                      /**< DTG Interaction Channel Spec value "at least 30" - we use 40 to avoid races with the test timer */
#define MAXURI                  HTTP_MAX_URI_LENGTH+1   /**< DTG Interaction Channel Spec value plus space for null termination */
#define MAX_COOKIE_SIZE			5000                    /**< RFC 2109 */
#define MAX_COOKIES             0                       /**< max no. of cookies to store, 0 = unlimited. D-Book requires at least 32. */
#define MAXMETHOD               16                      /**< buffer size for http method name */
#define CURL_MAX_REQUESTS		10						/**< how many files to download at once */
#define CURL_SLEEP_PERIOD       5                       /**< Miliseconds for which cURL thread sleeps when cURL has no data for the moment */

//#define ITK_HTTP_USE_PROXY
//#define ITK_HTTP_USE_VERIFY_CALLBACK

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/**
 * Implementation of ITK Http interface
 */

/* List of allowed cypher suites for https */
#define CURL_CYPHERLIST  "NULL-MD5:NULL-SHA:EXP-DES-CBC-SHA:DES-CBC-SHA:DES-CBC3-SHA:AES128-SHA:AES256-SHA"

/* running status */
typedef enum http_req_state_t
{
	REQ_IDLE,       /**< new request, not sent yet; or completed request */
	REQ_PENDING,    /**< request send() call made, not yet picked up by curl */
	REQ_RUNNING,    /**< request has been passed to curl to process */
	REQ_CALLBACK    /**< request has been finished by curl, callback is being processed */
} http_req_state_t;

/** internal request struct */
typedef struct http_request_t
{
	itk_http_request_t vtable;          /**< api jump table */
	struct http_t *http;                /**< the http to which it belongs */
	char uri[MAXURI];                   /**< uri of request */
	char method[MAXMETHOD];             /**< the type of request */
	itk_http_callback_t callback;       /**< callback function */
	void *context;                      /**< callback context */
	struct curl_slist *headers;         /**< request headres */
	void *content;                      /**< the content to send if a post message */
	size_t contentLength;               /**< the length of the content in bytes */
	http_req_state_t state;             /**< enum indicating running state */
	CURL *curlEasyHandle;               /**< the associated curl handle */
	HBOOL abort;						// bool_t abort;                     /**< flag indicating request has been aborted */
	HBOOL released;						// bool_t released;                  /**< flag indicating request has been released */
} http_request_t;

/** http request list */
typedef struct http_req_entry_t
{
	http_request_t *req;
	struct http_req_entry_t *next;
	struct http_req_entry_t *prev;
} http_req_entry_t;

typedef struct http_req_list_t
{
	http_req_entry_t *root;
	itk_int32_t count;
	itk_int32_t running;
} http_req_list_t;

/** internal cookie data */
typedef struct http_cookie_t
{
	char *name;
	char *domain;
	char *path;
	int expiryDate;
	char *value;
	HBOOL secure;		// bool_t secure;
	char *curl_format;
	struct http_cookie_t *next;
} http_cookie_t;

/** internal http struct */
typedef struct http_t
{
	itk_http_t vtable;                        /**< api jump table */
	//itk_os_t *os;                         /**< os interface */
	//itk_heap_t *heap;                     /**< http heap */
	itk_cert_mngr_t *tls;                   /**< tls certificate manager */
	itk_http_event_fn_t eventHandler;       /**< eventHandler function */
	void *eventContext;                     /**< eventHandler context */
	itk_http_status_t status;               /**< the http status */
	char *curlCookies;                      /**< zero terminated buffer holding all curl format cookies */
	http_cookie_t *cookieJar;               /**< linked list of cookie structs */
	itk_int32_t cookieCount;                /**< no. of cookies stored */
    itk_int32_t cookieLimit;                /**< max no. of cookies stored */
	unsigned long lock;						// itk_semaphore_t *lock;	/**< data protection semaphore */
	unsigned long idle;						// itk_semaphore_t *idle;	/**< request synchronisation semaphore */
	unsigned long thread;					// itk_thread_t *thread;	/**< worker thread to handle requests and callbacks */
	HBOOL quit;								// bool_t quit;				/**< flag to signal thread exit */
	http_req_list_t req_list;				/**< list of http requests */
	CURLM *curlMultiHandle;					/**< Curl Multi handle */
	CURLSH *curlShareHandle;					/**< Curl Share handle, used to share DNS cache, cookies etc among cURL easy handles */
	char *proxyURI;
} http_t;

#if defined(ITK_HTTP_USE_VERIFY_CALLBACK)
/** verify callback for avstream **/
typedef void (*ITK_HTTP_VerifyExternalCallback)(HUINT32 ulTunerId, void *userdata, void *verifyhandle, HINT32 *result);

typedef struct ITK_HTTP_VerifyExternalCallback_s
{
	ITK_HTTP_VerifyExternalCallback		verifycallback;
	void 								*userdata;
} ITK_HTTP_VerifyExternalCallback_t;
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#if defined(ITK_HTTP_USE_VERIFY_CALLBACK)
ITK_HTTP_VerifyExternalCallback_t	*g_stVerifyExternalCallback = NULL;
#endif


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static itk_http_t *s_pstHttpObj = NULL;


/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/

// Request list management ---- Note, http lock must be held while calling these
#if 0		// not used
static void request_list_init( http_t *http );
static void request_list_term( http_t *http );
#endif
static HBOOL request_list_add( http_t *http, http_request_t *req );
static HBOOL request_list_check( http_t *http, http_request_t *req );
static void request_list_remove( http_t *http, http_request_t *req );
#if 0		// not used
static int request_list_count( http_t *http, http_req_state_t state );
#endif
// local work funcs
static void http_lock( http_t *theHttp );
static void http_unlock( http_t *theHttp );

static int debug_callback( CURL *handle, curl_infotype type, char *msg, size_t msglen, void *userp );
static CURLcode sslctx_callback( CURL *curl, void *_sslctx, void *parm );
static int verify_callback( X509_STORE_CTX *ctx, void *arg );
static size_t header_callback( void *ptr, size_t size, size_t nmemb, void *userp );
static size_t curl_callback( void *ptr, size_t size, size_t nmemb, void *userp );
static void parseReturnedCookies( http_t *http, char *curlCookie );
static void deleteCookie( http_t *http, http_cookie_t *cookie );
static http_cookie_t *findCookie( http_t *theHttp, const char *identity );
static void curl_thread( void *http );

itk_errcode_t local_itk_Http_CreateResource(http_t *theHttp);

#if defined(ITK_HTTP_USE_VERIFY_CALLBACK)
void verify_callback_for_avstream(HUINT32 ulTunerId, void *userdata, void *verifyhandle, HINT32 *result);
#endif

// http request API ---- http lock must be held while changing requests or the request list
/**
 * Resets the request to the uri provided.
 */
static void req_reset(struct itk_http_request_t *thiz, const char *uri)
{
	http_request_t *theReq = (http_request_t*)thiz;
	if( theReq == NULL )
	{
		HxLOG_Error("ERROR : theReq is NULL\n");
		return;
	}
	HxLOG_Assert(theReq);
	HxLOG_Assert(uri);

	HxLOG_Debug("http_lock!! \n");

	http_lock(theReq->http);
	HxLOG_Debug("IC: reset request [%p] to %s\n",theReq,uri);
	if( !request_list_check( theReq->http, theReq ) )
	{
		// not a valid request!
		http_unlock( theReq->http );
		return;
	}

	if( theReq->state == REQ_IDLE || theReq->state == REQ_PENDING )
	{
		memset(theReq->uri,0,MAXURI);
		strncpy(theReq->uri,uri,MAXURI);
		theReq->uri[MAXURI-1] = '\0';
		theReq->state = REQ_IDLE;
	}
	else
	{
		HxLOG_Debug("IC: Cannot reset request in state %d\n",theReq->state);
	}

	http_unlock(theReq->http);
}

/**
 * Begin the process of sending the request and downloading response.
 * Implementation is asynchronous and completes by calling the callback function
 * passed to #itk_http_t::newRequest.
 */
static itk_errcode_t req_send(itk_http_request_t *thiz, void *content, size_t contentLength)
{
	http_request_t *theReq = (http_request_t*)thiz;
	itk_errcode_t err;
	if( theReq == NULL )
	{
		HxLOG_Error("ERROR : theReq is NULL\n");
		return ITKE_FAIL;
	}
	HxLOG_Assert(theReq);

	HxLOG_Debug("\n");

	http_lock(theReq->http);
	HxLOG_Debug("IC: send request [%p]\n",theReq);

	if( !request_list_check( theReq->http, theReq ) )
	{
		// not a valid request!
		http_unlock( theReq->http );
		return ITKE_NOT_FOUND;
	}

	if( theReq->http->status != HTTP_STATUS_ACTIVE )
	{
		theReq->http->eventHandler( theReq->http->eventContext, HTTP_LOCAL_ERROR );
		err = ITKE_NO_RESOURCE;
	}
	else if( theReq->state == REQ_IDLE )
	{
		/* assign post data */
		theReq->content = content;
		theReq->contentLength = contentLength;
		theReq->state = REQ_PENDING;  // curl will pick up pending requests on its next loop.
		VK_SEM_Release(theReq->http->idle);		// theReq->http->idle->signal( theReq->http->idle );
		err = ITKE_OK;
	}
	else
	{
		HxLOG_Debug("IC: Cannot send request in state %d\n",theReq->state);
		err = ITKE_NO_RESOURCE;
	}
	http_unlock(theReq->http);
	return err;
}

/**
 * Stops a pending request
 *
 * Implementation is synchronous
 * @param[in] thiz      A pointer to the request object.
 */
static itk_errcode_t req_abort(itk_http_request_t *thiz)
{
	http_request_t *theReq = (http_request_t*)thiz;
	itk_errcode_t err = ITKE_FAIL;
	if( theReq == NULL )
	{
		HxLOG_Error("ERROR : theReq is NULL\n");
		return ITKE_FAIL;
	}
	HxLOG_Assert(theReq);

	HxLOG_Debug("\n");

	http_lock( theReq->http );
	HxLOG_Debug("IC: abort request [%p]\n",theReq);

	if( !request_list_check( theReq->http, theReq ) )
	{
		// not a valid request!
		http_unlock( theReq->http );
		return ITKE_NOT_FOUND;
	}

	switch( theReq->state )
	{
		case REQ_IDLE:
		case REQ_PENDING:
			// request has not been sent yet. Reset it so it won't be sent.
			theReq->state = REQ_IDLE;
			err = ITKE_OK;
			break;

		case REQ_RUNNING:
			// curl is running this one, flag it to cancel the callback.
			theReq->abort = TRUE;
			err = ITKE_OK;
			break;

		case REQ_CALLBACK:
		default:
			// It's too late, callback already happened. Can't abort it now.
			HxLOG_Debug("IC: Cannot abort request in state %d.\n",theReq->state);
			err = ITKE_FAIL;
			break;
	}

	http_unlock(theReq->http);
	return err;
}

/**
 * Adds a header to the request.
 * This function appends additional, free-format headers to the HTTP request.  It can be
 * used to add new headers and change existing headers.
 * Each header must be terminated by a CR/LF pair.
 * The header string contain multiple concatonated headers.
 * If the header already exisists it is replaced.
 * Adding a header with a string containing empty header value removes the header.
 */
static void req_addHeader(itk_http_request_t* thiz, const char *header)
{
	http_request_t *theReq = (http_request_t*)thiz;
	if( theReq == NULL )
	{
		HxLOG_Error("ERROR : theReq is NULL\n");
		return;
	}
	HxLOG_Assert(theReq);
	HxLOG_Assert(header);

	HxLOG_Debug("\n");

	http_lock( theReq->http );
	//HxLOG_Debug("IC: add header to request [%p], %s\n",theReq,header);

	if( !request_list_check( theReq->http, theReq ) )
	{
		// not a valid request!
		http_unlock( theReq->http );
		return;
	}

	if( theReq->state == REQ_IDLE || theReq->state == REQ_PENDING )
	{
		theReq->headers = curl_slist_append(theReq->headers, header);
	}
	else
	{
		HxLOG_Debug("Cannot add header in state %d\n",theReq->state);
	}

	http_unlock( theReq->http );
}

/**
 * Release resources associated with a request.
 * @param[in] thiz      A pointer to the request object.
 */
static void req_release(itk_http_request_t *thiz)
{
	http_request_t *theReq = (http_request_t*)thiz;
	http_t *theHttp;
	if( theReq == NULL )
	{
		HxLOG_Error("ERROR : theReq is NULL\n");
		return ;
	}

	theHttp = theReq->http;
	if( theHttp  == NULL )
	{
		HxLOG_Error("ERROR : theHttp is NULL\n");
		return ;
	}

	HxLOG_Assert(theReq);
	HxLOG_Assert(theHttp);

	HxLOG_Debug("\n");

	/* flag pending request as released */
	http_lock( theHttp );
	HxLOG_Debug("IC: release request [%p]\n",theReq);

	if( !request_list_check( theHttp, theReq ) )
	{
		// not a valid request!
		http_unlock( theHttp );
		return;
	}

	// we can't release the request while curl is processing it, so just flag it.
	// the curl thread will spot the flag and abort and release it.
	if( theReq->state == REQ_RUNNING )
	{
		HxLOG_Debug("IC: release flagged  %p\n", theReq);
		theReq->released = TRUE;
		http_unlock( theHttp );
		return;
	}

	// callback happens under lock, so we can't end up in here during callback processing.
	// Other states are safe to delete the request.

	curl_slist_free_all(theReq->headers);
	theReq->headers=NULL;

	request_list_remove( theHttp, theReq );
	HLIB_STD_MemFree(theReq);	// theReq->http->heap->free( theHttp->heap, theReq );

	http_unlock( theHttp );
}

// http interface functions  ----------------------------------

/**
 * Create a request object.
 */
static itk_http_request_t *http_newRequest(itk_http_t *thiz, const char *method,
		const char *uri, itk_http_callback_t callback, void *context)
{
	http_t *theHttp = (http_t*)thiz;
	http_request_t *theReq = NULL;
	if( theHttp == NULL )
	{
		HxLOG_Error("ERROR : http is NULL\n");
		return NULL;
	}
	if( uri == NULL )
	{
		HxLOG_Error("ERROR : uri is NULL\n");
		return NULL;
	}
	if( callback == NULL )
	{
		HxLOG_Error("ERROR : callback is NULL\n");
		return NULL;
	}
	HxLOG_Assert(theHttp);
	HxLOG_Assert(uri);
	HxLOG_Assert(callback);
	HxLOG_Assert(method);

	HxLOG_Debug("\n");

	if( strlen(uri) > HTTP_MAX_URI_LENGTH )
	{
		HxLOG_Error("ERROR : uri length > HTTP_MAX_URI_LENGTH\n");
		return NULL;
	}

	http_lock( theHttp );

	/* create request */
	theReq = HLIB_STD_MemAlloc(sizeof(http_request_t));		// theReq = theHttp->heap->zalloc(theHttp->heap,sizeof(http_request_t),"IC Request");
	HxLOG_Debug("IC: new request [%p] for %s\n",theReq,uri);
	if (theReq)
	{
		/* init request */
		memset(theReq,0,sizeof(http_request_t));
		theReq->vtable.reset = req_reset;
		theReq->vtable.send = req_send;
		theReq->vtable.abort = req_abort;
		theReq->vtable.addHeader = req_addHeader;
		theReq->vtable.release = req_release;
		theReq->http = theHttp;
		strncpy(theReq->uri,uri,MAXURI);
		theReq->uri[MAXURI - 1] = '\0';
		strncpy(theReq->method,method,MAXMETHOD);
		theReq->method[MAXMETHOD - 1] = '\0';
		theReq->callback = callback;
		theReq->context = context;

		if( request_list_add( theHttp, theReq ) != TRUE )
		{
			// failed to add it. :( abort!
			HLIB_STD_MemFree(theReq);	// theHttp->heap->free( theHttp->heap, theReq );
			theReq = NULL;
		}
	}

	http_unlock( theHttp );
	return (itk_http_request_t*)theReq;
}

/**
 * Sets an event handler with the HTTP which is called
 * to indicate a HTTP Error events and HTTP Status change.
 */
static void http_setEventHandler(itk_http_t *thiz, itk_http_event_fn_t eventHandler, void *context)
{
	http_t *theHttp = (http_t*)thiz;
	if( theHttp == NULL )
	{
		HxLOG_Error("ERROR : http is NULL\n");
		return;
	}
	if( eventHandler == NULL )
	{
		HxLOG_Error("ERROR : eventHandler is NULL\n");
		return;
	}
	HxLOG_Assert(theHttp);
	HxLOG_Assert(eventHandler);

	HxLOG_Debug("http_lock!! \n");

	http_lock( theHttp );
	theHttp->eventHandler = eventHandler;
	theHttp->eventContext = context;
	http_unlock( theHttp );
}

/**
 * Set certificate object for a TLS session.
 */
static void http_setTLS(itk_http_t *thiz, itk_cert_mngr_t *tls)
{
	http_t *theHttp = (http_t*)thiz;

	if( theHttp == NULL )
	{
		HxLOG_Error("ERROR : http is NULL\n");
		return;
	}
	HxLOG_Assert(tls);

	HxLOG_Debug("http_lock!! \n");

	http_lock( theHttp );
	theHttp->tls=tls;

#if defined(ITK_HTTP_USE_VERIFY_CALLBACK)
	if(g_stVerifyExternalCallback)
	{
		HLIB_STD_MemFree(g_stVerifyExternalCallback);
	}

	g_stVerifyExternalCallback = HLIB_STD_MemAlloc(sizeof(ITK_HTTP_VerifyExternalCallback_t));
	memset(g_stVerifyExternalCallback, 0x0, sizeof(ITK_HTTP_VerifyExternalCallback_t));

	g_stVerifyExternalCallback->verifycallback = verify_callback_for_avstream;
	g_stVerifyExternalCallback->userdata = (void*)tls;

	// TODO: check memory leak.
	PORT_ITK_PostMsg(eMHEG_HTTP_REGISTER_CALLBACK, 0, 0, (HINT32)g_stVerifyExternalCallback, 0, 0);
#endif

	http_unlock( theHttp );
}

/**
 * Sets a cookie.
 * This must only be called under lock to avoid races on the cookie list.
 */
static void http_setCookie(itk_http_t *thiz, const char *identity, itk_uint32_t expiryDate,
		const char * value, size_t valueLength, itk_int32_t secure)
{
	http_t *theHttp = (http_t*)thiz;
	char *p, *pid,*domain, *name,*path, cookie[MAX_COOKIE_SIZE],id[MAX_COOKIE_SIZE];
	http_cookie_t *newCookie = NULL, *last;

	if( theHttp == NULL )
	{
		HxLOG_Error("ERROR : http is NULL\n");
		return;
	}
	if( identity == NULL )
	{
		HxLOG_Error("ERROR : identity is NULL\n");
		return;
	}
	if( value == NULL )
	{
		HxLOG_Error("ERROR : value is NULL\n");
		return;
	}

	HxLOG_Assert(theHttp);
	HxLOG_Assert(identity);
	HxLOG_Assert(value);
	UNUSED(valueLength);

	HxLOG_Debug("\n");

	HxLOG_Debug("http_setCookie: %s %ld %s\n",identity,expiryDate,value);

	if (strlen(value)>MAXURI)
	{
		HxLOG_Error("ERROR : value > MAXURI\n");
		return;
	}

	// replacing existing cookie?
	last = findCookie( theHttp, identity );
	if( last != NULL )
		deleteCookie( theHttp, last );

	// check if we have too many cookies
    if( theHttp->cookieLimit != 0 && theHttp->cookieCount >= theHttp->cookieLimit )
    {
        // if we're full, must delete the oldest cookie
        last = theHttp->cookieJar;
        while( last->next != NULL )
            last = last->next;
        HxLOG_Debug("Full; deleteCookie %s\n",last->name);
        deleteCookie( theHttp, last );
    }

	// allocate a new cookie struct
	newCookie = HLIB_STD_MemAlloc( sizeof(http_cookie_t) );		// newCookie = theHttp->heap->zalloc( theHttp->heap, sizeof(http_cookie_t), "cookie" );
	if( newCookie == NULL )
	{
		HxLOG_Error("ERROR : newCookie is NULL\n");
		return;
	}

	memset(newCookie, 0, sizeof(http_cookie_t));
	newCookie->expiryDate = expiryDate;
	newCookie->secure = ( secure ? TRUE : FALSE );
	newCookie->value = HLIB_STD_MemAlloc( strlen(value)+1 );		// newCookie->value = theHttp->heap->zalloc( theHttp->heap, strlen(value)+1, "cookie-value" );
	if( newCookie->value == NULL )
	{
		HLIB_STD_MemFree(newCookie);		// theHttp->heap->free( theHttp->heap, newCookie );
		HxLOG_Error("ERROR : newCookie->value is NULL\n");
		return;
	}

	memset(newCookie->value, 0, strlen(value)+1);
	strncpy( newCookie->value, value, strlen(value));		// strcpy_s( newCookie->value, strlen(value)+1, value );
	/* parse id = name,domain/path */

	memset(id,0,MAXURI);
	strncpy(id,identity,MAXURI);
	id[MAXURI - 1] = '\0';
	pid = id;
	p = strchr(pid,',');
	if( p == NULL )
	{
		// bad format.
		HLIB_STD_MemFree(newCookie->value);	// theHttp->heap->free( theHttp->heap, newCookie->value );
		HLIB_STD_MemFree(newCookie);			// theHttp->heap->free( theHttp->heap, newCookie );
		return;
	}

	*p = '\0';
	name = pid;
	pid = ++p;
	p = strchr(pid,'/');
	if (p==NULL)
		path="";
	else
	{
		*p = '\0';
		path = ++p;
	}
	domain = pid;
	if( domain[0] == '.' ) // skip any leading dot
		domain = ++pid;

	/* convert cookie to curl format (netscape format!) */
	snprintf( cookie, MAXURI, "%s\tFALSE\t/%s\t%s\t%d\t%s\t%s\r\n",
			domain,path,(secure ? "TRUE" : "FALSE"),(int)expiryDate,name,value);
	// sprintf_s(cookie,MAXURI,"%s\tFALSE\t/%s\t%s\t%d\t%s\t%s\r\n",
	//    domain,path,(secure ? "TRUE" : "FALSE"),expiryDate,name,value);
	HxLOG_Debug("setcookie: %s\n",cookie);

	newCookie->domain = HLIB_STD_MemAlloc( strlen(domain)+1 );			// newCookie->domain = theHttp->heap->zalloc( theHttp->heap, strlen(domain)+1, "cookie-domain" );
	newCookie->name = HLIB_STD_MemAlloc( strlen(name)+1 );				// newCookie->name = theHttp->heap->zalloc( theHttp->heap, strlen(name)+1, "cookie-name" );
	newCookie->path = HLIB_STD_MemAlloc( strlen(path)+1 );				// newCookie->path = theHttp->heap->zalloc( theHttp->heap, strlen(path)+1, "cookie-path" );
	newCookie->curl_format = HLIB_STD_MemAlloc( strlen(cookie)+1 );		// newCookie->curl_format = theHttp->heap->zalloc( theHttp->heap, strlen(cookie)+1, "cookie-curl" );

	if( newCookie->domain == NULL || newCookie->name == NULL ||
			newCookie->path == NULL || newCookie->curl_format == NULL )
	{
		// at least one alloc failed!
		if( newCookie->domain )
			HLIB_STD_MemFree(newCookie->domain);			// theHttp->heap->free( theHttp->heap, newCookie->domain );
		if( newCookie->name )
			HLIB_STD_MemFree(newCookie->name);			// theHttp->heap->free( theHttp->heap, newCookie->name );
		if( newCookie->path )
			HLIB_STD_MemFree(newCookie->path);			// theHttp->heap->free( theHttp->heap, newCookie->path );
		if( newCookie->curl_format )
			HLIB_STD_MemFree(newCookie->curl_format);	// theHttp->heap->free( theHttp->heap, newCookie->curl_format );
		HLIB_STD_MemFree(newCookie->value);				// theHttp->heap->free( theHttp->heap, newCookie->value );
		HLIB_STD_MemFree(newCookie);						// theHttp->heap->free( theHttp->heap, newCookie );
		return;
	}

	memset(newCookie->domain, 0, strlen(domain)+1);
	memset(newCookie->name, 0, strlen(name)+1);
	memset(newCookie->path, 0, strlen(path)+1);
	memset(newCookie->curl_format, 0, strlen(cookie)+1);

	strncpy( newCookie->name, name, strlen(name));				// strcpy_s( newCookie->name, strlen(name)+1, name );
	strncpy( newCookie->domain, domain, strlen(domain));			// strcpy_s( newCookie->domain, strlen(domain)+1, domain );
	strncpy( newCookie->path, path, strlen(path));				// strcpy_s( newCookie->path, strlen(path)+1, path );
	strncpy( newCookie->curl_format, cookie, strlen(cookie));		// strcpy_s( newCookie->curl_format, strlen(cookie)+1, cookie );

	/* add it to our jar */
	newCookie->next = theHttp->cookieJar;
	theHttp->cookieJar = newCookie;
	theHttp->cookieCount++;
    HxLOG_Debug("setCookie: count now %d\n",theHttp->cookieCount);
}

/**
 * Gets a cookie.
 * This must only be called under lock to avoid races on the cookie list.
 */
static itk_errcode_t http_getCookie(itk_http_t *thiz, const char *identity, itk_uint32_t *expiryDate,
		char *value, size_t vBufLen, size_t *valueLength, itk_int32_t *secure)
{
	http_t *theHttp = (http_t*)thiz;
	http_cookie_t *cookie;
	if( theHttp == NULL )
	{
		HxLOG_Error("ERROR : http is NULL\n");
		return ITKE_FAIL;
	}
	if( identity == NULL )
	{
		HxLOG_Error("ERROR : identity is NULL\n");
		return ITKE_FAIL;
	}
	HxLOG_Assert(theHttp);
	HxLOG_Assert(identity);

	HxLOG_Debug("\n");

	cookie = findCookie( theHttp, identity );
	if( cookie != NULL )
	{
		if( expiryDate != NULL )
			*expiryDate = cookie->expiryDate;
		if ( secure != NULL)
			*secure = ( cookie->secure == TRUE ? 1 : 0 );
		if( value != NULL )
		{
			strncpy( value, cookie->value, vBufLen);
			value[vBufLen - 1] = '\0';
		}
		if( valueLength != NULL )
			*valueLength = strlen(cookie->value);
		return ITKE_OK;
	}

	HxLOG_Error("ERROR : http_getCookie\n");
	return ITKE_NOT_FOUND;
}

/**
 * external access to set cookie
 */
static void http_ext_setCookie(itk_http_t *thiz, const char *identity, itk_uint32_t expiryDate,
		const char * value, size_t valueLength, itk_int32_t secure)
{
	http_t *theHttp = (http_t*)thiz;
	if( theHttp == NULL )
	{
		HxLOG_Error("ERROR : http is NULL\n");
		return;
	}
	HxLOG_Assert(theHttp);

	HxLOG_Debug("http_lock!! \n");

	http_lock( theHttp );
	http_setCookie( thiz, identity, expiryDate, value, valueLength, secure );
	http_unlock( theHttp );
}

/**
 * external access to get cookie
 */
static itk_errcode_t http_ext_getCookie(itk_http_t *thiz, const char *identity, itk_uint32_t *expiryDate,
		char *value, size_t vBufLen, size_t *valueLength, itk_int32_t *secure)
{
	http_t *theHttp = (http_t*)thiz;
	itk_errcode_t result;
	if( theHttp == NULL )
	{
		HxLOG_Error("ERROR : http is NULL\n");
		return ITKE_FAIL;
	}
	HxLOG_Assert(theHttp);

	HxLOG_Debug("http_lock !! \n");

	http_lock( theHttp );
	result = http_getCookie( thiz, identity, expiryDate, value, vBufLen, valueLength, secure );
	http_unlock( theHttp );
	return result;
}

/**
 * Returns the IC status
 */
static itk_http_status_t http_getStatus(itk_http_t *thiz)
{
	http_t *theHttp = (http_t*)thiz;
	if( theHttp == NULL )
	{
		HxLOG_Error("ERROR : http is NULL\n");
		return HTTP_STATUS_DISABLED;
	}
	HxLOG_Assert(theHttp);
	return theHttp->status;
}

// http supporting functions --------------------------------------------

/**
 * Sets the HTTP status
 * @param[in]   thiz    A pointer to the http object.
 * @param[in]   status  The new status of the interaction channel which will be one of the following:
 *      HTTP_STATUS_ACTIVE    The HTTP has not been disabled and the network interface is physically connected and fully configured
 *      HTTP_STATUS_INACTIVE  The HTTP has not been disabled but the network interface is not physically connected or is not fully configured
 *      HTTP_STATUS_DISABLED  The interaction channel has been disabled through the action of the viewer
 */
void setInteractionChannelStatus(itk_http_t *http, itk_http_status_t status)
{
	http_t *theHttp = (http_t *)http;

	HxLOG_Debug("\n");

	if (theHttp->eventHandler != NULL)
	{
		theHttp->eventHandler(theHttp->eventContext, HTTP_STATUS_CHANGED);
	}
	theHttp->status = status;
}


/**
 * Sets the HTTP proxy URI
 * @param[in]   thiz    A pointer to the http object.
 * @param[in]   proxy   The uri of the proxy server or NULL to disable the proxy.
 */
void setProxyHost( itk_http_t *http, char *proxy )
{
    http_t *theHttp = (http_t *)http;

	if (theHttp->proxyURI != NULL)
	{
		HLIB_STD_MemFree(theHttp->proxyURI);
		theHttp->proxyURI = NULL;
	}

	if (proxy != NULL)
	{
		theHttp->proxyURI = HLIB_STD_MemAlloc(strlen(proxy)+1);
		if (!theHttp->proxyURI)
		{
			HxLOG_Error("ERROR : proxyURI cannot created because not enough memory!!\r\n");
			return;
		}
//		ItkFatal((theHttp->proxyURI);

		strncpy(theHttp->proxyURI, proxy, strlen(proxy));
		theHttp->proxyURI[strlen(proxy)] = '\0';
	}
}

/**Create an http manager
*/
itk_http_t *local_itk_Http_CreateItkHttp(void)
{
	http_t *theHttp;
	itk_errcode_t	nErrCode = ITKE_OK;

	/* create http */
	theHttp = HLIB_STD_MemAlloc(sizeof(http_t));
	if (!theHttp)
	{
		HxLOG_Error("ERROR : The http object cannot create because not enough memory!!\r\n");
		return (itk_http_t*)NULL;
	}

	/* init http */
	memset(theHttp,0,sizeof(http_t));
	theHttp->vtable.setEventHandler = http_setEventHandler;
	theHttp->vtable.setTLS = http_setTLS;
	theHttp->vtable.newRequest = http_newRequest;
	theHttp->vtable.setCookie = http_ext_setCookie;
	theHttp->vtable.getCookie  = http_ext_getCookie;
	theHttp->vtable.getStatus = http_getStatus;
	theHttp->cookieJar = NULL;
	theHttp->curlCookies = NULL;
	theHttp->cookieCount = 0;
	theHttp->cookieLimit = MAX_COOKIES;
#if defined (CONFIG_DEBUG)
	theHttp->status = HTTP_STATUS_ACTIVE;
#else
	theHttp->status = HTTP_STATUS_INACTIVE;
#endif

#if defined(ITK_HTTP_USE_PROXY)
	setProxyHost(theHttp, (char*)"iplayer-guests.bbc.co.uk:2323");
#else
	theHttp->proxyURI = NULL;
#endif

	nErrCode = local_itk_Http_CreateResource(theHttp);
	if (nErrCode != ITKE_OK)
	{
		HxLOG_Error("ERROR : local_itk_Http_CreateResource!!\r\n");
		HLIB_STD_MemFree(theHttp);
		return (itk_http_t*)NULL;
	}

	return (itk_http_t*)theHttp;
}

itk_errcode_t local_itk_Http_CreateResource(http_t *theHttp)
{
	if (theHttp == NULL)
	{
		HxLOG_Error("ERROR : http is NULL\n");
		return ITKE_FAIL;
	}

	/* create http lock */
	if (VK_SEM_CreateWithCount(&theHttp->lock, 1,	"sHostIC_lock", VK_SUSPEND_FIFO)!=VK_OK)
	{
		HxLOG_Error("ERROR : VK_SEM_CreateWithCount !!!\n");
		return ITKE_FAIL;
	}

	/* create http idle */
	if (VK_SEM_CreateWithCount(&theHttp->idle, 0, "sHostIC_idle", VK_SUSPEND_FIFO)!=VK_OK)
	{
		HxLOG_Error("ERROR : VK_SEM_CreateWithCount !!!\n");
		return ITKE_FAIL;
	}

	/* create curl thread */
	//theHttp->thread = VK_TASK_JOINABLE;
	if (VK_TASK_Create(curl_thread, HTTP_THREAD_PRIORITY, HTTP_THREAD_STACK_SIZE, "tHostIC", theHttp, &theHttp->thread, 1)!=VK_OK)
	{
		HxLOG_Error("ERROR : VK_TASK_Create !!!\n");
		return ITKE_FAIL;
	}

	VK_TASK_Start(theHttp->thread);

	return ITKE_OK;
}

/**
 * Free resources associated with the http.
 */
void deleteItkHttp(itk_http_t *thiz)
{
	http_t *theHttp = (http_t*)thiz;
	http_cookie_t *cookie, *next;

	HxLOG_Debug("\n");

	if (thiz)
	{
		/* signal exit */
		theHttp->quit = TRUE;
		VK_SEM_Release(theHttp->idle);			// theHttp->idle->signal( theHttp->idle );
		/* wait for exit */
		VK_TASK_Join(theHttp->thread);			// theHttp->thread->join( theHttp->thread, ITK_WAIT );

		/* free resources */
		theHttp->thread = 0; /* must be 0 */ 	// theHttp->thread->release( theHttp->thread );
		VK_SEM_Destroy(theHttp->lock); 			// theHttp->lock->release( theHttp->lock );
		VK_SEM_Destroy(theHttp->idle);			// theHttp->idle->release( theHttp->idle );

		if( theHttp->curlCookies )
			HLIB_STD_MemFree(theHttp->curlCookies);	// theHttp->heap->free( theHttp->heap, theHttp->curlCookies );

		if( theHttp->cookieJar )
		{
			// free each cookie...
			next = theHttp->cookieJar;
			while( next != NULL )
			{
				cookie = next;
				next = cookie->next;
				if( cookie->curl_format )
					HLIB_STD_MemFree(cookie->curl_format);		// theHttp->heap->free( theHttp->heap,cookie->curl_format );
				if( cookie->domain )
					HLIB_STD_MemFree(cookie->domain);			// theHttp->heap->free( theHttp->heap,cookie->domain );
				if( cookie->name )
					HLIB_STD_MemFree(cookie->name);				// theHttp->heap->free( theHttp->heap,cookie->name );
				if( cookie->path )
					HLIB_STD_MemFree(cookie->path);				// theHttp->heap->free( theHttp->heap,cookie->path );
				if( cookie->value )
					HLIB_STD_MemFree(cookie->value);				// theHttp->heap->free( theHttp->heap,cookie->value );
				HLIB_STD_MemFree(cookie);						// theHttp->heap->free( theHttp->heap,cookie );
			}
		}


		if (theHttp->proxyURI)
		{
			HLIB_STD_MemFree(theHttp->proxyURI);
			theHttp->proxyURI = NULL;
		}

		/* free http */
		HLIB_STD_MemFree(theHttp);

		/* reset the manager */
		s_pstHttpObj = NULL;
	}

#if defined(ITK_HTTP_USE_VERIFY_CALLBACK)
	if(g_stVerifyExternalCallback)
	{
		HLIB_STD_MemFree(g_stVerifyExternalCallback);
		g_stVerifyExternalCallback= NULL;
	}
#endif
}

/**Lock function
*/
static void http_lock(http_t *theHttp)
{
	if( theHttp == NULL )
	{
		HxLOG_Error("ERROR : theHttp is NULL !!!\n");
		return;
	}

	if( VK_SEM_Get(theHttp->lock)!=VK_OK )
	{
		HxLOG_Error("ERROR : VK_SEM_Get !!!\n");
		return;
	}

	HxLOG_Debug("(%ld)\n", theHttp->lock);
}
/**Unlock function
*/
static void http_unlock(http_t *theHttp)
{
	if( theHttp == NULL )
	{
		HxLOG_Error("ERROR : theHttp is NULL !!!\n");
		return;
	}

	if( VK_SEM_Release(theHttp->lock)!=VK_OK )
	{
		HxLOG_Error("ERROR : VK_SEM_Release !!!\n");
		return;
	}

	HxLOG_Debug("(%ld)\n", theHttp->lock);
}

// Request list management ---- Note, http lock must be held while calling these
#if 0		// not used
static void request_list_init( http_t *http )
{
	if( http != NULL )
	{
		http->req_list.root = NULL;
		http->req_list.count = 0;
	}
}

static void request_list_term( http_t *http )
{
	if( http != NULL )
	{
		if( http->req_list.count > 0 )
			WARN(("http: request list not empty when terminated!\n");
	}
}
#endif

static HBOOL request_list_add( http_t *http, http_request_t *req )
{
	http_req_entry_t *newNode;

	if( http == NULL || req == NULL )
		return FALSE;

	newNode = HLIB_STD_MemAlloc( sizeof(http_req_entry_t) );		// newNode = http->heap->zalloc( http->heap, sizeof(http_req_entry_t), MEMINFO );
	if( newNode == NULL )
		return FALSE;

	HxLOG_Debug("\n");

	memset(newNode,0,sizeof(http_req_entry_t));
	newNode->req = req;
	newNode->next = http->req_list.root;
	if( http->req_list.root != NULL )
	{
		http->req_list.root->prev = newNode;
	}
	http->req_list.root = newNode;
	http->req_list.count++;
	return TRUE;
}

static HBOOL request_list_check( http_t *http, http_request_t *req )
{
	http_req_entry_t *node;

	if( http == NULL || req == NULL )
		return FALSE;

	HxLOG_Debug("\n");

	node = http->req_list.root;
	while( node != NULL )
	{
		if( node->req == req )
			return TRUE;
		node = node->next;
	}

	return FALSE;
}

static void request_list_remove( http_t *http, http_request_t *req )
{
	http_req_entry_t *node;

	if( http == NULL || req == NULL )
		return;

	HxLOG_Debug("\n");

	node = http->req_list.root;
	while( node != NULL )
	{
		if( node->req == req )
		{
			if( node->prev == NULL )  // first in the list?
				http->req_list.root = node->next;
			else
				node->prev->next = node->next;

			if( node->next != NULL ) // not at end of the list
				node->next->prev = node->prev;

			HLIB_STD_MemFree(node); 		// http->heap->free( http->heap, node );
			http->req_list.count--;
			break;
		}
		node = node->next;
	}
}

#if 0		// not used
static int request_list_count( http_t *http, http_req_state_t state )
{
	http_req_entry_t *node;
	int count = 0;

	if( http == NULL || http->req_list.count == 0 )
		return 0;

	node = http->req_list.root;
	while( node != NULL )
	{
		if( node->req->state == state )
			count++;
		node = node->next;
	}

	return count;
}
#endif

// ---------------- CURL THREAD ------------------

/**
 * This static variable used only to pass on the heap to memory allocation functions.
 * This variable shall not be used for any other purposes.
 */
static http_t *sHttp = NULL;

/**
 * Memory functions used by cURL. These functions allocates and deallocate
 * from the http heap.
 */

static void *malloc_callback(size_t size)
{
	void *ptr = HLIB_STD_MemAlloc(size);		// sHttp->heap->alloc( sHttp->heap, size, CMEMINFO );
	HxLOG_Info("(%p), size(%d)\n", ptr, size);
	return ptr;
}

static void free_callback(void *ptr)
{
	HxLOG_Info("(%p)\n", ptr);
	HLIB_STD_MemFree(ptr);		// sHttp->heap->free( sHttp->heap, ptr );
}

static void *realloc_callback(void *ptr, size_t size)
{
	void *ptr_re = HLIB_STD_MemRealloc(ptr, size);		// sHttp->heap->realloc( sHttp->heap, ptr, size );
	HxLOG_Info("(%p) size (%d)\n", ptr_re, size);
	return ptr_re;
}

static char *strdup_callback(const char *str)
{
	size_t len = strlen(str) + 1;
	char *dstStr = HLIB_STD_MemAlloc(len);		// sHttp->heap->alloc( sHttp->heap, len, CMEMINFO );
	if (NULL != dstStr)
	{
		strncpy( dstStr, str, len);	dstStr[len-1] = '\0';	// strcpy_s( dstStr, len, str );
	}

	HxLOG_Info("(%p), (%s), len(%d)\n", dstStr, dstStr, len);

	return dstStr;
}

static void *calloc_callback(size_t nmemb, size_t size)
{
	void *ptr = HLIB_STD_MemCalloc(size * nmemb);		// sHttp->heap->zalloc( sHttp->heap, size * nmemb, CMEMINFO );
	HxLOG_Info("(%p), size(%d), nmemb (%d)\n", ptr, size, nmemb);
	return ptr;
}


/**
 * Set up curl options for a request.
 */
static void setCurlEasyOpt(http_request_t *theReq, CURL *curlHandle)
{
	HBOOL https = FALSE;		// bool_t https = FALSE;
	http_cookie_t *cookie;

	HxLOG_Debug("\n");

	curl_easy_reset( curlHandle );
	theReq->curlEasyHandle = curlHandle;
	/* set url */
	curl_easy_setopt( curlHandle, CURLOPT_URL, theReq->uri );

	if (theReq->http->proxyURI != NULL)
	{
		HxLOG_Debug("IC: set proxyURI [%s]\n", theReq->http->proxyURI);
		curl_easy_setopt( curlHandle, CURLOPT_PROXY, theReq->http->proxyURI );
	}
	/* set callback */
	curl_easy_setopt( curlHandle, CURLOPT_WRITEFUNCTION, curl_callback );
	curl_easy_setopt( curlHandle, CURLOPT_WRITEDATA, theReq );
	/* set header callback */
	curl_easy_setopt( curlHandle, CURLOPT_HEADERFUNCTION, header_callback );
	curl_easy_setopt( curlHandle, CURLOPT_HEADERDATA, theReq );
	/* set ssl callback */
	curl_easy_setopt( curlHandle, CURLOPT_SSL_CTX_FUNCTION, sslctx_callback );
	curl_easy_setopt( curlHandle, CURLOPT_SSL_CTX_DATA, theReq );

	curl_easy_setopt( curlHandle, CURLOPT_FRESH_CONNECT, 1 );
	/* ssl options */
	if( strncmp( theReq->uri,"https://", 8 ) == 0 )
	{
		https = TRUE;
		/* set protocol version to TLS */
		curl_easy_setopt( curlHandle, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1 );
		/* set ciphers to use */
		curl_easy_setopt( curlHandle, CURLOPT_SSL_CIPHER_LIST, CURL_CYPHERLIST );
		/* verify the servers certificate */
		curl_easy_setopt( curlHandle, CURLOPT_SSL_VERIFYPEER, 1 );
		/* dont insist on the url matching the cert name */
		curl_easy_setopt( curlHandle, CURLOPT_SSL_VERIFYHOST, 1 );
		/* don't allow reuse of https connection in case certificates change on carousel */
		curl_easy_setopt( curlHandle, CURLOPT_FRESH_CONNECT, 1 );
		curl_easy_setopt( curlHandle, CURLOPT_SSL_SESSIONID_CACHE, 0 );
		/* the following entries will prevent curl referencing any local certificates */
		curl_easy_setopt( curlHandle, CURLOPT_CAINFO, NULL );
		curl_easy_setopt( curlHandle, CURLOPT_CAPATH, NULL );
	}

#ifndef NDEBUG
	/* set debug on, debug func and callback context */
	curl_easy_setopt( curlHandle, CURLOPT_VERBOSE, TRUE );
	curl_easy_setopt( curlHandle, CURLOPT_DEBUGFUNCTION, debug_callback );
	curl_easy_setopt( curlHandle, CURLOPT_DEBUGDATA, theReq );
#endif

	/* spec options */
	curl_easy_setopt( curlHandle, CURLOPT_TIMEOUT, HTTP_TIMEOUT );
	curl_easy_setopt( curlHandle, CURLOPT_CONNECTTIMEOUT, HTTP_TIMEOUT );
	/* enable cookies */
	curl_easy_setopt( curlHandle, CURLOPT_COOKIEFILE, "" );

	/* IC020 : caused by firewall, curl crash using multiple muti-handles when resolve host failed */
	// for test (swkim) : curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1);
	
	/* set cookies user defined cookies */
	cookie = theReq->http->cookieJar;

	// delete curl's cookies so that it doesn't contain others we don't know about.
	curl_easy_setopt(curlHandle, CURLOPT_COOKIELIST,"ALL");

	while( cookie != NULL )
	{
		if( cookie->secure )
		{
			if( https )  // only send secure cookies over https
				curl_easy_setopt( curlHandle, CURLOPT_COOKIELIST, cookie->curl_format );
		}
		else
		{
			curl_easy_setopt( curlHandle, CURLOPT_COOKIELIST, cookie->curl_format );
		}
		cookie = cookie->next;
	}
	/* show all cookies */
	//curl_easy_setopt( curlHandle, CURLOPT_COOKIEJAR, "-" );
	//curl_easy_setopt( curlHandle, CURLOPT_COOKIELIST, "FLUSH" );

	/* set headers */
	curl_easy_setopt( curlHandle, CURLOPT_HTTPHEADER, theReq->headers );

	/* don't follow redirections, this is handled by redkey interaction channel */
	curl_easy_setopt( curlHandle, CURLOPT_FOLLOWLOCATION, FALSE );

	/* set method */
	if( strcmp( theReq->method, "POST" ) == 0 )
	{
		curl_easy_setopt( curlHandle, CURLOPT_POST, TRUE );
		/* set content */
		if( theReq->content )
		{
			curl_easy_setopt( curlHandle, CURLOPT_POSTFIELDSIZE, theReq->contentLength );
			curl_easy_setopt( curlHandle, CURLOPT_POSTFIELDS, theReq->content );
		}
	}
	else if( strcmp( theReq->method, "HEAD" ) == 0 )
	{
		curl_easy_setopt( curlHandle, CURLOPT_NOBODY, TRUE );
	}
	else /* default is GET */
	{
	}
}

/**
 * Add all pending requests to the curl multi handler, ready to perform.
 * Only called under lock!
 */
static void http_send_pending_requests( http_t *theHttp )
{
	http_req_entry_t *node;
	http_request_t *req;
	CURL *curlHandle;

	HxLOG_Debug("\n");

	node = theHttp->req_list.root;
	while( node != NULL )
	{
		req = node->req;
		if( req->state == REQ_PENDING && theHttp->req_list.running < CURL_MAX_REQUESTS )
		{
			// this request is ready to go!
			curlHandle = curl_easy_init();
			if( NULL == curlHandle )
			{
				HxLOG_Debug( "IC: curl_easy_init() failed.\n" );
				break;
			}
			curl_easy_setopt( curlHandle, CURLOPT_SHARE, theHttp->curlShareHandle );
			setCurlEasyOpt( req, curlHandle );
			curl_multi_add_handle( theHttp->curlMultiHandle, curlHandle );
			theHttp->req_list.running++;
			req->state = REQ_RUNNING;
		}
		node = node->next;
	}
}

/**
 * halt any aborted requests via the curl multi handler.
 * Only called under lock!
 */
static void http_halt_aborted_requests( http_t *theHttp )
{
	http_req_entry_t *node;
	http_request_t *req;

	HxLOG_Debug("\n");

	node = theHttp->req_list.root;
	while( node != NULL )
	{
		req = node->req;
		if( req->state == REQ_RUNNING && (req->abort == TRUE || req->released == TRUE) )
		{
			// this request must be halted!
			HxLOG_Debug("IC: curl aborting request [%p]\n",req);
			curl_multi_remove_handle( theHttp->curlMultiHandle, req->curlEasyHandle );
			curl_easy_setopt( req->curlEasyHandle, CURLOPT_SHARE, NULL );
			curl_easy_cleanup( req->curlEasyHandle );
			req->curlEasyHandle = NULL;
			req->state = REQ_IDLE;
			theHttp->req_list.running--;
		}
		node = node->next;
	}
}

/**
 * Free up any requests that were released during download.
 * The curl handle will already have been tidied up.
 * Only called under lock!
 */
static void http_flush_requests( http_t *theHttp )
{
	http_req_entry_t *node;
	http_request_t *req;

	HxLOG_Debug("\n");

	node = theHttp->req_list.root;
	while( node != NULL )
	{
		req = node->req;
		node = node->next;
		if( req->released == TRUE )
		{
			HxLOG_Debug("IC: flush released request [%p]\n",req);
			curl_slist_free_all( req->headers);
			req->headers = NULL;
			request_list_remove( req->http, req );
			HLIB_STD_MemFree(req);		// req->http->heap->free( req->http->heap, req );
			req = NULL;
		}
	}
}

/**
 * handle any requests that are completed. Only called under lock.
 */
static void http_process_completed_request( http_t *theHttp, CURLMsg *curlMsg )
{
	CURL *curlHandle = curlMsg->easy_handle;
	CURLcode code = curlMsg->data.result;
	CURLcode cookieCode;
	struct curl_slist *cookieList = NULL;
	http_req_entry_t *node;
	http_request_t *req = NULL;
	itk_http_response_t errorType;

	HxLOG_Debug("\n");

	// find the matching request
	node = theHttp->req_list.root;
	while( node != NULL && node->req->curlEasyHandle != curlHandle )
		node = node->next;

	// clean up the handle
	cookieCode = curl_easy_getinfo( curlHandle, CURLINFO_COOKIELIST, &cookieList );
	curl_multi_remove_handle( theHttp->curlMultiHandle, curlHandle );
	curl_easy_setopt( curlHandle, CURLOPT_SHARE, NULL );
	curl_easy_cleanup( curlHandle );
	curlHandle = NULL;
	theHttp->req_list.running--;

	if( node == NULL )
	{
		// No matching request found. This should never happen.
		return;
	}

	req = node->req;
	req->curlEasyHandle = NULL;
	if( req->released || req->abort )
	{
		// if aborted or released, don't bother doing anything.
		curl_slist_free_all( req->headers);
		req->headers = NULL;
		req->state = REQ_IDLE;
		return;
	}

	// otherwise, processing begins!
	req->state = REQ_CALLBACK;

	// get cookies and add to list
	if( CURLE_OK == code && CURLE_OK == cookieCode )
	{
		struct curl_slist *cookie = cookieList;
		while( cookie )
		{
			/* add cookie to our jar */
			parseReturnedCookies( req->http, cookie->data );
			cookie = cookie->next;
		}
	}

	if( cookieList != NULL )
	{
		curl_slist_free_all( cookieList );
		cookieList = NULL;
	}

	/* clean up */
	curl_slist_free_all( req->headers);
	req->headers=NULL;

	if( CURLE_OK == code )
	{
		/* notify end */
		HxLOG_Debug("IC: HTTP request Finished: %s %s\n",req->method, req->uri);
		if( req->callback )
			req->callback( req->context, (itk_http_request_t*)req, HTTP_END, NULL, 0);
	}
	else
	{
		/* notify error */
		HxLOG_Debug("IC: HTTP request failed: %s %s\n",req->method, req->uri);
		HxLOG_Debug("IC: The error was #%d: %s\n",code,curl_easy_strerror( code ) );

		errorType = HTTP_ERROR;  // the general case
		switch( code )
		{
			case CURLE_SSL_CACERT:
				errorType = HTTP_ERROR_TLS;
				break;
			case CURLE_SSL_CONNECT_ERROR:
				errorType = HTTP_ERROR_TLS;
				if( theHttp->eventHandler )
					theHttp->eventHandler( theHttp->eventContext, HTTP_REMOTE_ERROR );
				break;
			case CURLE_BAD_CONTENT_ENCODING:
			case CURLE_FILESIZE_EXCEEDED:
			case CURLE_HTTP_RETURNED_ERROR:
			case CURLE_PARTIAL_FILE:
				if( theHttp->eventHandler )
					theHttp->eventHandler( theHttp->eventContext, HTTP_REMOTE_ERROR );
				break;
			case CURLE_COULDNT_RESOLVE_HOST:
			case CURLE_COULDNT_RESOLVE_PROXY:
			case CURLE_COULDNT_CONNECT:
			case CURLE_OPERATION_TIMEOUTED:
			case CURLE_SEND_ERROR:
			case CURLE_RECV_ERROR:
				errorType = HTTP_ERROR_NETWORK;
				if( theHttp->eventHandler )
					theHttp->eventHandler( theHttp->eventContext, HTTP_NETWORK_ERROR );
				break;
			default:
				if( theHttp->eventHandler )
					theHttp->eventHandler(theHttp->eventContext, HTTP_LOCAL_ERROR);
				break;
		}

		if( req->callback )
			req->callback( req->context, (itk_http_request_t*)req, errorType, NULL, 0);
	}

	req->state = REQ_IDLE;
}

/** Entry point to curl thread, main loop.
*/
static void curl_thread( void *http )
{
	http_t *theHttp = (http_t*)http;
	int runningHandles; /* number of requests still not completed and being handled by curl_multi handle */
	CURLMsg *curlMsg = NULL; /* reply structure from curl_multi_info_read() identifies completed requests */

	HxLOG_Assert(theHttp);

	/*
	 ** using this static variable just to pass on the heap to memory allocation functions.
	 ** This variable shall not be used for any other purposes.
	 */
	 sHttp = theHttp;

	/* init curl */
	curl_global_init_mem(CURL_GLOBAL_ALL,
			malloc_callback,
			free_callback,
			realloc_callback,
			strdup_callback,
			calloc_callback);

	/* init curl share */
	theHttp->curlShareHandle = curl_share_init();
	HxLOG_Assert(theHttp->curlShareHandle);
	if( NULL == theHttp->curlShareHandle )
	{
		curl_global_cleanup();
		HxLOG_Debug("Cannot init curlShare\n\r");
		return;
	}

	//curl_share_setopt(theHttp->curlShareHandle,CURLSHOPT_USERDATA,theHttp);
	curl_share_setopt(theHttp->curlShareHandle,CURLSHOPT_SHARE,CURL_LOCK_DATA_COOKIE);
	//curl_share_setopt(theHttp->curlShareHandle,CURLSHOPT_LOCKFUNC,http_curlshLock);
	//curl_share_setopt(theHttp->curlShareHandle,CURLSHOPT_UNLOCKFUNC,http_curlshUnlock);

	/* Initialize the cURL multi handle that will do multiple simultaneous download */
	theHttp->curlMultiHandle = curl_multi_init();
	HxLOG_Assert(theHttp->curlMultiHandle);
	if( NULL == theHttp->curlMultiHandle )
	{
		curl_share_cleanup(theHttp->curlShareHandle);
		curl_global_cleanup();
		HxLOG_Debug("Cannot init curlMulti\n\r");
		return;
	}

	while( !theHttp->quit )
	{
		if( 0 == theHttp->req_list.count )
		{
			/* wait for something to do */
			VK_SEM_Get(theHttp->idle);		// theHttp->idle->wait( theHttp->idle, ITK_WAIT);
			if( theHttp->quit )
			{
				// we were woken up by a quit request
				break;
			}
		}

		HxLOG_Debug("http_lock !!\n");
		http_lock( theHttp );

		http_send_pending_requests( theHttp );
		http_halt_aborted_requests( theHttp );
		// trigger asynchronous curl processing
		while( CURLM_CALL_MULTI_PERFORM == curl_multi_perform(theHttp->curlMultiHandle, &runningHandles));
		// handle completed requests
		while( NULL != (curlMsg = curl_multi_info_read(theHttp->curlMultiHandle, &runningHandles)))
			http_process_completed_request( theHttp, curlMsg );
		// clear any released-while-running requests
		http_flush_requests( theHttp );

		http_unlock( theHttp );

		VK_TASK_Sleep(CURL_SLEEP_PERIOD);	// theHttp->os->sleep( theHttp->os, CURL_SLEEP_PERIOD );
	}

	HxLOG_Debug("http_lock !!\n");
	/* free curl */
	http_lock( theHttp );

	HxLOG_Debug("curlMultiHandle (%p) !!\n", theHttp->curlMultiHandle );

	curl_multi_cleanup( theHttp->curlMultiHandle );
	/* free any active handles */
	if( theHttp->req_list.count > 0 )
	{
		http_req_entry_t *node = theHttp->req_list.root;
		while( node != NULL )
		{
			if( node->req->curlEasyHandle != NULL )
			{
				HxLOG_Debug("curlEasyHandle (%p) !!\n", node->req->curlEasyHandle );

				curl_easy_setopt( node->req->curlEasyHandle, CURLOPT_SHARE, NULL );
				curl_easy_cleanup( node->req->curlEasyHandle );

				node->req->curlEasyHandle = NULL;
			}
			node = node->next;
		}
	}

	HxLOG_Debug("http_flush_requests!!\n");

	http_flush_requests( theHttp );
	HxLOG_Debug("curlShareHandle (%p) !!\n", theHttp->curlShareHandle);
	curl_share_cleanup( theHttp->curlShareHandle );
	curl_global_cleanup();
	sHttp = NULL;
	http_unlock( theHttp );
}

// Supporting functions ---------------------------------------

static http_cookie_t *findCookie( http_t *theHttp, const char *identity )
{
	http_cookie_t *cookie;
	char id[MAX_COOKIE_SIZE];

	if( theHttp == NULL || identity == NULL )
		return NULL;

	HxLOG_Debug("\n");

	cookie = theHttp->cookieJar;
	while( cookie )
	{
		// strcpy_s( id, MAX_COOKIE_SIZE, cookie->name );
		// strcat_s( id, MAX_COOKIE_SIZE, "," );
		// strcat_s( id, MAX_COOKIE_SIZE, cookie->domain );
		// strcat_s( id, MAX_COOKIE_SIZE, "/" );
		// strcat_s( id, MAX_COOKIE_SIZE, cookie->path );

		/* look at each cookie in turn */
		memset(id,0,MAX_COOKIE_SIZE); // null을 보장하기 위함

		strncpy(id,cookie->name, MAX_COOKIE_SIZE - 1);
		strncat(id,",", MAX_COOKIE_SIZE - strlen(id) - 1);
		strncat(id,cookie->domain, MAX_COOKIE_SIZE - strlen(id) - 1);
		strncat(id,"/", MAX_COOKIE_SIZE - strlen(id) - 1);
		strncat(id,cookie->path, MAX_COOKIE_SIZE - strlen(id) - 1);
		if( strcmp( id, identity ) == 0 ) // match!
			return cookie;

		/* try it with a leading domain dot */
		// strcpy_s( id, MAX_COOKIE_SIZE, cookie->name );
		// strcat_s( id, MAX_COOKIE_SIZE, ",." );
		// strcat_s( id, MAX_COOKIE_SIZE, cookie->domain );
		// strcat_s( id, MAX_COOKIE_SIZE, "/" );
		// strcat_s( id, MAX_COOKIE_SIZE, cookie->path );

		memset(id,0,MAX_COOKIE_SIZE);
		strncpy(id,cookie->name, MAX_COOKIE_SIZE - 1);
		strncat(id,",.", MAX_COOKIE_SIZE - strlen(id) - 1);
		strncat(id,cookie->domain, MAX_COOKIE_SIZE - strlen(id) - 1);
		strncat(id,"/", MAX_COOKIE_SIZE - strlen(id) - 1);
		strncat(id,cookie->path, MAX_COOKIE_SIZE - strlen(id) - 1);
		if( strcmp( id, identity ) == 0 ) // match!
			return cookie;

		cookie = cookie->next;
	}

	return NULL;
}

static void deleteCookie( http_t *http, http_cookie_t *cookie )
{
	http_cookie_t *prev, *ck;

	if( http == NULL || cookie == NULL )
		return;

	HxLOG_Debug("\n");

	// remove the cookie from the jar

	if( http->cookieJar == cookie )
	{
		// it's the first cookie
		http->cookieJar = cookie->next;
	}
	else
	{
		ck = http->cookieJar;
		prev = NULL;
		while( ck != NULL )
		{
			if( ck == cookie )
				break;
			prev = ck;
			ck = ck->next;
		}
		if( ck != NULL )
		{
			if (prev != NULL)
				prev->next = cookie->next;
		}
	}

	// clear cookie contents
	if( cookie->name != NULL )
		HLIB_STD_MemFree(cookie->name);			// http->heap->free( http->heap, cookie->name );
	if( cookie->domain != NULL )
		HLIB_STD_MemFree(cookie->domain);		// http->heap->free( http->heap, cookie->domain );
	if( cookie->path != NULL )
		HLIB_STD_MemFree(cookie->path);			// http->heap->free( http->heap, cookie->path );
	if( cookie->value != NULL )
		HLIB_STD_MemFree(cookie->value);			// http->heap->free( http->heap, cookie->value );
	if( cookie->curl_format != NULL )
		HLIB_STD_MemFree(cookie->curl_format);	// http->heap->free( http->heap, cookie->curl_format );

	HLIB_STD_MemFree(cookie);					// http->heap->free( http->heap, cookie );
	http->cookieCount--;
    HxLOG_Debug("deleteCookie: count now %d\n",http->cookieCount);
}

//remove leading and trailing quote marks from a string, if present
static void strip_quotes( char *str )
{
	size_t n, max;

	HxLOG_Debug("\n");

	max = strlen( str );
	if( str[0] != '\"' || str[max-1] != '\"' )
		return;

	for( n = 1; n < max-1; n++ )
		str[n-1] = str[n];
	str[n-1] = 0;
}

static void parseReturnedCookies( http_t *http, char *curlCookie )
{
	char domain[1048], path[1048], name[1048], value[MAX_COOKIE_SIZE];
	char identity[MAX_COOKIE_SIZE], flag[16], secure[16];
	int expiryDate, count;

	if( curlCookie == NULL || http == NULL )
		return;

	HxLOG_Debug("\n");

	HxLOG_Debug("http: decode cookie: %s\n",curlCookie);
	// format:
	// domain [TAB] flag [TAB] path [TAB] secure [TAB] expiry [TAB] name [TAB] value \r\n
	count = sscanf( curlCookie, "%s\t%s\t%s\t%s\t%d\t%s\t%s\r\n",
			domain, flag, path, secure, &expiryDate, name, value );

	if( count != 7 )
	{
		HxLOG_Debug("http: Failed to decode cookie: %s\n",curlCookie);
		return;
	}

	// there might be quotes around values - if so, strip them.
	strip_quotes( domain );
	strip_quotes( path );
	strip_quotes( value );
	strip_quotes( secure );

	snprintf( identity, MAX_COOKIE_SIZE, "%s,%s%s", name, domain, path );	// sprintf_s( identity, MAX_COOKIE_SIZE, "%s,%s%s", name, domain, path );
	http_setCookie( (itk_http_t *)http, identity, expiryDate, value,
			strlen(value), (secure[0] == 'F' ? 0 : 1) );
}

/**Function called when data loaded
*/
static size_t curl_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	http_request_t *theReq = (http_request_t*)userp;

	if( !theReq  )
	{
		HxLOG_Error("ERROR : theReq is NULL\n");
		return 0;
	}
	HxLOG_Assert(theReq);

	HxLOG_Debug("theReq -> abort (%d), released (%d), callback (%p)\n", theReq->abort, theReq->released, theReq->callback);

	if( TRUE != theReq->abort && TRUE != theReq->released && theReq->callback )
	{
		/* notify content */
		theReq->callback( theReq->context, (itk_http_request_t*)theReq,
				HTTP_CONTENT, ptr, size*nmemb );
	}

	HxLOG_Debug("(%d)\n", size*nmemb);

	return size * nmemb;
}

/**Function called when response header is received
*/
static size_t header_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	http_request_t *theReq = (http_request_t*)userp;

	if( !theReq )
	{
		HxLOG_Error("ERROR : theReq is NULL\n");
		return 0;
	}
	HxLOG_Assert(theReq);

	HxLOG_Debug("theReq -> abort (%d), released (%d), callback (%p)\n", theReq->abort, theReq->released, theReq->callback);

	if( TRUE != theReq->abort && TRUE != theReq->released && theReq->callback )
	{
		/* notify header */
		theReq->callback( theReq->context, (itk_http_request_t*)theReq,
				HTTP_HEADER, ptr, size*nmemb );
	}

	HxLOG_Debug("(%d)\n", size*nmemb);

	return size * nmemb;
}

#if defined(ITK_HTTP_USE_VERIFY_CALLBACK)
void verify_callback_for_avstream(HUINT32 ulTunerId, void *userdata, void *verifyhandle, HINT32 *result)
{
	itk_validation_t valid;
	itk_cert_mngr_t	*tls = (itk_cert_mngr_t*)userdata;
	X509_STORE_CTX	*ctx = (X509_STORE_CTX *)verifyhandle;
	HxLOG_Debug("ctx (%p)\n", ctx);

	HxLOG_Debug("verify_callback_for_avstream : %p\n",ctx);
	/* Note:
	 * This assumes void ptr of verifyCertificate is the cert store context.
	 * We don't know the data length, hence passing zero which is not ideal.
	 */
	valid = tls->verifyCertificate( tls, ctx, 0 );
	*result = (valid == ITK_VALID)?1:0;
}
#endif

/**
 * Verify certificate
 */
static int verify_callback(X509_STORE_CTX *ctx, void *arg)
{
	itk_validation_t valid;
	http_request_t *theReq = (http_request_t*)arg;
	HxLOG_Debug("ctx (%p), arg (%p)\n", ctx, arg);

	HxLOG_Debug("verify_callback : %p %p\n",ctx,arg);
	/* Note:
	 * This assumes void ptr of verifyCertificate is the cert store context.
	 * We don't know the data length, hence passing zero which is not ideal.
	 */
	valid = theReq->http->tls->verifyCertificate( theReq->http->tls, ctx, 0 );
	return (valid == ITK_VALID)?1:0;
}

/**Function called before the initialization of an SSL connection.
 * sslctx is a pointer to an openSSL SSL_CTX.
 * parm is a user pointer set with CURLOPT_SSL_CTX_DATA.
 */
static CURLcode sslctx_callback(CURL *curl, void *_sslctx, void *parm)
{
	http_request_t *theReq = (http_request_t*)parm;
	SSL_CTX *ctx = _sslctx;

	if( !theReq )
	{
		HxLOG_Error("ERROR : theReq is NULL\n");
		return CURLE_SSL_CACERT;
	}
	HxLOG_Assert(theReq);
	UNUSED(curl);

	/* do we have a tls ?*/
	if( theReq->http->tls == NULL )
	{
		HxLOG_Error("ERROR : theReq->http->tls is NULL\n");
		return CURLE_SSL_CACERT;
	}

	HxLOG_Debug("ctx (%p)\n", ctx);
	/* set up connection */
	//SSL_CTX_set_quiet_shutdown( ctx, 1 );
	//SSL_CTX_set_cipher_list( ctx, "RC4-MD5" );
	//SSL_CTX_set_mode( ctx, SSL_MODE_AUTO_RETRY );
	//SSL_CTX_set_verify_depth( ctx, 9 );

	SSL_CTX_set_verify( ctx, SSL_VERIFY_PEER, NULL );
	SSL_CTX_set_cert_verify_callback( ctx, verify_callback, theReq );
	return CURLE_OK ;
}


CURLcode sslctx_callback_wrapper(CURL *curl, void *_sslctx, void *parm)
{
	return sslctx_callback(curl, _sslctx, parm);
}

/**Function called to show curl debug output
*/
static int debug_callback(CURL *handle, curl_infotype type, char *msg, size_t msglen, void *userp)
{
	UNUSED(userp);
	UNUSED(handle);

#if 0 //turn off debug

	switch(type)
	{
		case CURLINFO_DATA_IN: //The data is protocol data received from the peer.
			{
				size_t i = 0;
				for( i = 0; i < msglen; i++ )
					HxLOG_Debug("%02x",(unsigned char*)msg[i]);
			}
			break;
		case CURLINFO_DATA_OUT: //The data is protocol data sent to the peer.
			//        break;
		case CURLINFO_TEXT: //The data is informational text.
		case CURLINFO_HEADER_IN: // The data is header (or header-like) data received from the peer.
			//        break;
		case CURLINFO_HEADER_OUT: //The data is header (or header-like) data sent to the peer.
			{
				size_t i = 0;
				for( i = 0; i < msglen; i++ )
					HxLOG_Debug("%c",msg[i]);
			}
			break;
		default:
			break;
	}
#else
	UNUSED(type);
	UNUSED(msg);
	UNUSED(msglen);
#endif
	return 0;
}

#define _____PUBLIC_APIs________________________________________________________________


void PORT_ITK_Http_RegisterObject(void)
{
	ITK_registerManager(ITK_HTTP, s_pstHttpObj);
}

void PORT_ITK_Http_CreateObject(void)
{
	s_pstHttpObj = local_itk_Http_CreateItkHttp();

	if (s_pstHttpObj)
		ITK_registerManager(ITK_HTTP, s_pstHttpObj);
}

void	PORT_ITK_Http_Notify(HBOOL bStatus)
{
	http_t *theHttp = sHttp;
	itk_http_status_t status = (itk_http_status_t)((bStatus == TRUE) ? HTTP_STATUS_ACTIVE : HTTP_STATUS_INACTIVE);

	if (theHttp != NULL)
	{
		HxLOG_Debug("http status (%s -> %s)\n", (theHttp->status == HTTP_STATUS_ACTIVE)? "HTTP_STATUS_ACTIVE" : "HTTP_STATUS_INACTIVE", (status == HTTP_STATUS_ACTIVE)? "HTTP_STATUS_ACTIVE" : "HTTP_STATUS_INACTIVE");

		if (status != theHttp->status)
		{
			setInteractionChannelStatus((itk_http_t *)theHttp, status);
		}
	}
}

