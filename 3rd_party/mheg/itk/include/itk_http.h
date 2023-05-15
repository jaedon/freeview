/*
 * Copyright (c) 2007 Strategy & Technology Limited. All Rights Reserved.
 * Copyright in the whole and every part of this file belongs to 
 * Strategy & Technology Limited ("the Owner") and may not be used, sold,
 * supplied, transferred, copied, reproduced in whole or in part, in any 
 * manner or form or in or on any media for the benefit of any person other 
 * than in accordance with the terms of the Owner's agreement or otherwise 
 * without the prior written consent of the Owner.
 */

#ifndef ITK_HTTP_H__
#define ITK_HTTP_H__
/**
 * @file
 * HTTP Interface.
 */
#include "itk_types.h"
#include "itk_crypto.h"

/**
 * @ingroup host
 * @defgroup itkhttp HTTP module
 * @{
 *
 *
 */

#define HTTP_MAX_URI_LENGTH (1024)
/**
 * HTTP Request Object.
 */
typedef struct itk_http_request_t
{
    /** 
     * Resets the request to the URI provided.
     * @param[in] thiz      A pointer to the request object.
     * @param[in] uri       The target object.
     */
    void (*reset)(struct itk_http_request_t *thiz, const char *uri);

    /**
     * Initiates the request.
     * The implementation is asynchronous and completes by calling the callback function
     * passed to #itk_http_t::newRequest.
     * @param[in] thiz           A pointer to the request object.
     * @param[in] content        A pointer to optional data to send with the request, generally 
                                 used for POST content.
     * @param[in] contentLength  The size of the content data in bytes.
     * @return      ITKE_OK             The request was sent successfully.
     * @return      ITKE_NO_RESOURCE    The request failed due to lack of resources (no callback should be generated).
     */
    itk_errcode_t (*send)(struct itk_http_request_t *thiz, void *content, size_t contentLength);

    /**
     * Stops a pending request.
     * The implementation is synchronous.
     * @param[in] thiz        A pointer to the request object.
     * @return    ITKE_OK     The request was aborted successfully.
     * @return    ITKE_FAIL   The request was not being processed.
     * @note This function is deprecated and will not be called by the client.
     *       Should a download still be in progress at the time the client releases the request,
     *       the host should abort the request and not generate further callbacks associated with it.
     */
    itk_errcode_t (*abort)(struct itk_http_request_t *thiz);

    /**
     * Adds a header to the request.
     * This function appends additional, free-format headers to the HTTP request.  It can be
     * used to add new headers and change existing headers.
     * Each header must be terminated by a CR/LF pair.
     * The header string contain multiple concatenated headers.  
     * If the header already exists it is replaced.
     * Adding a header with a string containing an empty header value removes the header.
     * @param[in] thiz      A pointer to the request object.
     * @param[in] header    A NULL terminated string variable containing the headers to append to the request.
     */
    void (*addHeader)(struct itk_http_request_t* thiz, const char *header);

    /**
     * Release resources associated with a request.
     * @param[in] thiz      A pointer to the request object.
     */
    void (*release)(struct itk_http_request_t *thiz);
} itk_http_request_t;


/**
 * Response types.
 */
typedef enum itk_http_response_t
{
    HTTP_HEADER,                /**< indicates the callback data is a response header */
    HTTP_CONTENT,               /**< indicates the callback data is response content */
    HTTP_END,                   /**< indicates the response has completed */
    HTTP_ERROR,                 /**< indicates the request has failed for internal reasons */
    HTTP_ERROR_NETWORK,         /**< indicates the request failed because of network issues */
    HTTP_ERROR_TLS              /**< indicates the https request failed because of TLS certificate issues */
} itk_http_response_t;

/**
 * Response callback function.
 * A function of this type is registered when a request is created with itk_http_t::newRequest.
 * The function is invoked when a server response is received.  The function parameter type
 * identifies what data is being signalled.  
 * If the type is HTTP_HEADER the data parameter points to the header data and dataLength 
 * is the length of the header.
 * If the type is HTTP_CONTENT the data parameter points to the returned content data and 
 * dataLength is the length of the content.  This can be file data for a GET request or server 
 * data for a POST request.  If a response is chunked there can be several HTTP_CONTENT responses.
 * If the type is HTTP_END the response has been processed.  This type should always be sent 
 * when that the request has been fully processed.  It can indicate the end of data transfer 
 * for a GET request or is sent after the last header of a HEAD request.  The data pointer 
 * will be NULL and dataLength zero.
 * If the type is HTTP_ERROR an internal error has occurred and the request has failed.
 * @param[in] context       The application context passed when the request was created.
 * @param[in] request       A pointer to the request object.
 * @param[in] type          The response type, HTTP_HEADER, HTTP_CONTENT, HTTP_END or HTTP_ERROR.
 * @param[in] data          The response data.
 * @param[in] dataLength    The response data length in bytes.
 */
typedef void (*itk_http_callback_t)(void *context, itk_http_request_t *request, 
                    itk_http_response_t type, void *data, size_t dataLength);
/**
 * HTTP status.
 */
typedef enum itk_http_status_t
{
    HTTP_STATUS_ACTIVE,       /**<The http has not been disabled and the network interface is physically connected and fully configured */
    HTTP_STATUS_INACTIVE,     /**<The http has not been disabled but the network interface is not physically connected or is not fully configured */
    HTTP_STATUS_DISABLED      /**<The http has been disabled through the action of the viewer */
} itk_http_status_t;

/**
 * HTTP Event
 */
typedef enum itk_http_event_t
{
    HTTP_STATUS_CHANGED=200,  /**< The http status has changed */
    HTTP_LOCAL_ERROR=201,     /**< Request failed because the interaction channel is not available. */
    HTTP_NETWORK_ERROR=202,   /**< Request failed because a remote server did not respond. */
    HTTP_REMOTE_ERROR=203     /**< Request failed due to an error on the server side. */
} itk_http_event_t;

/**
 * Event handler function.
 *
 * A function of this type is passed to the HTTP using the #itk_http_t::setEventHandler function.
 * When the HTTP status changes or a load error occurs the HTTP should call the function with 
 * the appropriate #itk_http_event_t value.
 * @param[in] context An application specific pointer passed to the HTTP in #itk_http_t::setEventHandler.
 * @param[in] httpEvent The event type:
 * - #HTTP_LOCAL_ERROR
 * - #HTTP_NETWORK_ERROR
 * - #HTTP_REMOTE_ERROR
 * - #HTTP_STATUS_CHANGED
 *
 * The HTTP shall indicate when a load or post request has failed by generating one 
 * of the following events:
 * - #HTTP_LOCAL_ERROR
 * - #HTTP_NETWORK_ERROR
 * - #HTTP_REMOTE_ERROR
 *
 * When the status of the HTTP changes it shall indicate this with an HTTP_STATUS_CHANGED event.
 * The #itk_http_t::getStatus method returns the current HTTP status which will be one of the following:
 * - #HTTP_STATUS_ACTIVE
 * - #HTTP_STATUS_INACTIVE
 * - #HTTP_STATUS_DISABLED
 *
 */
typedef void (*itk_http_event_fn_t)(void *context, itk_http_event_t httpEvent);

/**
 * HTTP Object.
 */
typedef struct itk_http_t
{
    /**
     * Sets an event handler with the HTTP which is called 
     * to indicate a HTTP Error events and HTTP Status change.
     *
     * @param[in]   thiz            A pointer to the http object.
     * @param[in]   eventHandler    The event handler function.
     * @param[in]   context         An application specific pointer that is passed back as
     *                              a parameter to the event handler function.
     */
    void (*setEventHandler)(struct itk_http_t *thiz, itk_http_event_fn_t eventHandler, void *context);

    /**
     * Set a TLS certificate manager used to verify server TLS certificates.  
     * The certificate mamanger is a container for all trusted CA certificates loaded from 
     * "DSM://auth.tls.<X>" files and can be used to verify the servers TLS certificate.
     * @param[in]   thiz    A pointer to the http object.
     * @param[in]   tls     A certificate manager object.
     */
    void (*setTLS)(struct itk_http_t *thiz, itk_cert_mngr_t *tls);

    /**
     * Create a request object.
     * The request object is used to generate HTTP requests.
     * @param[in]   thiz        A pointer to the http object.
     * @param[in]   method      A string identifying the method type, "GET", "HEAD" or "POST".
     * @param[in]   uri         The target object.
     * @param[in]   callback    A function pointer that will be called to return target data.
     * @param[in]   context     An application specific pointer that is passed back as a parameter
     *                          to the callback function.
     * @return  An HTTP request object if successful.
     * @return  NULL if unsuccessful.
     */
    itk_http_request_t *(*newRequest)(struct itk_http_t *thiz, 
                                    const char *method,
                                    const char *uri,
                                    itk_http_callback_t callback, 
                                    void *context);

    /**
     * Sets a cookie.
     * @param[in]   thiz        A pointer to the http object.
     * @param[in]   identity    A string identifying the cookie to set in the form "name.domain/path".
     * @param[in]   expiryDate  The expiry date of the cookie, expressed as a Modified Julian date or 
                                zero if the cookie is transient.
     * @param[in]   value       A buffer containing the value of the cookie.
     * @param[in]   valueLength The length of the cookie value in bytes.
     * @param[in]   secure      Cookies only sent over secure connection.
     */
    void (*setCookie)(struct itk_http_t *thiz, const char *identity, itk_uint32_t expiryDate, 
            const char * value, size_t valueLength, itk_int32_t secure);

    /**
     * Gets a cookie.
     * @param[in]   thiz        A pointer to the http object.
     * @param[in]   identity    A string identifying the cookie to set in the form "name.domain/path".
     * @param[in]   expiryDate  A buffer to store the expiry date of the cookie, expressed as a 
     *                          Modified Julian date or zero if the cookie is transient.
     * @param[in]   value       A buffer to store the value of the cookie.
     * @param[in]   valueBufLen The length of the value buffer in bytes.
     * @param[in]   valueLength The length of the cookie value in bytes.
     * @param[in]   secure      Cookies received over secure connection.
     * @return      ITKE_OK         The cookie was found and value returned.
     * @return      ITKE_NOT_FOUND  The cookie was not found.
     * @remark  The function may be called with a value pointer of NULL to obtain the cookie length and
     * this should be not be treated as an error.
     */
    itk_errcode_t (*getCookie)(struct itk_http_t *thiz, const char *identity, itk_uint32_t *expiryDate,
                char *value, size_t vBufLen, size_t *valueLength, itk_int32_t *secure );

    /**
     * Returns the HTTP status
     * @param[in]   thiz    A pointer to the http object.
     * @return  HTTP_STATUS_ACTIVE    The HTTP has not been disabled and the network interface is physically connected and fully configured
     * @return  HTTP_STATUS_INACTIVE  The HTTP has not been disabled but the network interface is not physically connected or is not fully configured
     * @return  HTTP_STATUS_DISABLED  The interaction channel has been disabled through the action of the viewer
     */
    itk_http_status_t (*getStatus)(struct itk_http_t *thiz);

} itk_http_t;

/** @} */

#endif
