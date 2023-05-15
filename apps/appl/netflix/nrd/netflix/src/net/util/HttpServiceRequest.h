/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef HTTP_SERVICE_REQUEST_H
#define HTTP_SERVICE_REQUEST_H

#include <nrdbase/tr1.h>
#include <nrdbase/DataBuffer.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdnet/AseUrl.h>
#include <nrdnet/IAsyncHttpLib.h>


namespace netflix
{

class HttpServiceMetrics;

/**
 * The HTTP service request class
 */
class HttpServiceRequest
{
public:
    //The request method.
    IAsyncHttpRequest::Method mRequestMethod;

    //The request URL.
    AseUrl mRequestUrl;

    //The request headers and body.
    HttpHeaders mRequestHeaders;
    DataBuffer mRequestBody;

    AseTimeVal mTimeout;

    AseTimeVal mConnectTimeout;
    AseTimeVal mStallTimeout;
public:
    //Constructor.
    HttpServiceRequest()
    {
    }

    //Constructor.
    HttpServiceRequest(
        IAsyncHttpRequest::Method requestMethod,
        AseUrl requestUrl,
        HttpHeaders requestHeaders,
        DataBuffer requestBody): mRequestMethod(requestMethod),
        mRequestUrl(requestUrl),
        mRequestHeaders(requestHeaders),
        mRequestBody(requestBody)
    {
    }

    //Destructor.
    virtual ~HttpServiceRequest()
    {
    }

    virtual bool shouldSuspend() const { return false; }

    //Provide back the HTTP response headers.
    virtual void provideResponseHeaders(
        const HttpResponseHeaders & responseHeaders) = 0;

    //Provide back the HTTP response body.
    virtual void provideResponseBody(
        const DataBuffer & responseBody) = 0;

    //Indicate the HTTP transaction is finished.
    virtual void finish(
        int32_t nativeErrorCode,
        long httpStatusCode,
        HttpServiceMetrics * pHttpServiceMetrics) = 0;
};

//Type definition for HttpServiceCallback pointer.
typedef shared_ptr<HttpServiceRequest> HttpServiceRequestPtr;

}


#endif // HTTP_SERVICE_REQUEST_H
