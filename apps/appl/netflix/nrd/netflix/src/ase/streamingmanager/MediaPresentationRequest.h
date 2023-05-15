/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MEDIA_PRESENTATION_REQUEST_H
#define MEDIA_PRESENTATION_REQUEST_H

/** @file MediaPresentationRequest.h */

#include <nrdase/IMediaConsumer.h>

namespace netflix {
namespace ase {

class ILocationSelector;
typedef shared_ptr<ILocationSelector> ILocationSelectorPtr;

class MediaPresentationRequest : public MediaRequest
{
public:
    typedef shared_ptr<MediaPresentationRequest> MediaPresentationRequestPtr;

    bool headersRequest() const;

    ILocationSelectorPtr getLocationSelector() const;

    void setLocationSelector( ILocationSelectorPtr pLocationSelector );

    void setThroughputMonitor( IThroughputMonitorPtr pThroughputMonitor );

    virtual void requestStarted( bool connecting, bool pipelined );

    virtual IThroughputMonitorPtr getThroughputMonitor() const;

    virtual AseErrorCode getRequestData(DataBuffer&) { return AS_NOT_ALlOWED; }

    virtual void loadStarted() { return; }

protected:
    MediaPresentationRequest( bool bHeaders );

protected:
    bool const              mHeadersRequest;
    ILocationSelectorPtr    mLocationSelectorPtr;
    IThroughputMonitorPtr   mThroughputMonitorPtr;
};

typedef MediaPresentationRequest::MediaPresentationRequestPtr MediaPresentationRequestPtr;

inline MediaPresentationRequest::MediaPresentationRequest(bool bHeaders)
    : MediaRequest( )
    , mHeadersRequest( bHeaders )
{
}

inline bool MediaPresentationRequest::headersRequest() const
{
    return mHeadersRequest;
}

inline ILocationSelectorPtr MediaPresentationRequest::getLocationSelector() const
{
    return mLocationSelectorPtr;
}

inline void MediaPresentationRequest::setLocationSelector( ILocationSelectorPtr pLocationSelector )
{
    mLocationSelectorPtr = pLocationSelector;
}

inline IThroughputMonitorPtr MediaPresentationRequest::getThroughputMonitor() const
{
    return mThroughputMonitorPtr;
}

inline void MediaPresentationRequest::setThroughputMonitor( IThroughputMonitorPtr pThroughputMonitor )
{
    mThroughputMonitorPtr = pThroughputMonitor;
}

inline void MediaPresentationRequest::requestStarted( bool /*connecting*/, bool /*pipelined*/ )
{
}


} // namespace ase
} // namespace netflix


#endif
