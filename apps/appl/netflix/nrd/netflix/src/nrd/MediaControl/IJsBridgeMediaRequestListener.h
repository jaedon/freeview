/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef I_JS_BRIDGE_MEDIA_REQUEST_LISTENER_H
#define I_JS_BRIDGE_MEDIA_REQUEST_LISTENER_H

#include <nrdbase/AseErrorCodes.h>
#include <nrdnet/HttpResponseHeaders.h>

namespace netflix {
class AseTimeVal;

namespace mediacontrol {

/** A listener for changes in IAppendableMediaRequest
 *
 * event: enum(progress|error|complete)
 */

class IJsBridgeMediaRequestListener
{
public:
    typedef shared_ptr<IJsBridgeMediaRequestListener> IJsBridgeMediaRequestListenerPtr;

    virtual void onLoadStart(uint32_t requestId, const AseTimeVal &time) = 0;
    virtual void onFirstByteReceived(uint32_t requestId, const AseTimeVal &time,const AseTimeVal &startTime, HttpResponseHeaders headers, bool newConnection) = 0;
    virtual void onProgress(uint32_t requestId, const AseTimeVal &time,
                            uint32_t elapsedTimeInMs, uint32_t bytesReceived, uint32_t httpResponseTime ) = 0;
    virtual void onError(uint32_t requestId, const AseTimeVal &time,
                         AseErrorCode errcode, int32_t httpStatusCode ) = 0;
    virtual void onComplete(uint32_t requestId, const AseTimeVal &time,
                            uint32_t elapsedTimeInMs) = 0;
    virtual void onComplete(uint32_t requestId, const AseTimeVal &time,
                            uint32_t elapsedTimeInMs, DataBuffer const& data) = 0;

    virtual ~IJsBridgeMediaRequestListener() {}
};

typedef IJsBridgeMediaRequestListener::IJsBridgeMediaRequestListenerPtr IJsBridgeMediaRequestListenerPtr;

}}

#endif


