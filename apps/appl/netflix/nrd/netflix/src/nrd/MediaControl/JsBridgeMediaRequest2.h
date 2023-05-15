/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef JS_BRIDGE_MEDIA_REQUEST_H
#define JS_BRIDGE_MEDIA_REQUEST_H

#include <nrdase/IMediaConsumer.h>

#include "IJsBridgeMediaRequestListener.h"

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/IMediaBufferPool.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdnet/HttpResponseHeaders.h>
#include <nrddemux/Reader.h>

namespace netflix {
namespace mediacontrol {

/**
 * @class JsBridgeMediaRequest
 * @brief An MediaRequest object for MediaRequestBridge
 *
 */
class JsBridgeMediaRequest : public ase::MediaRequest
{
public:
    typedef shared_ptr<JsBridgeMediaRequest> JsBridgeMediaRequestPtr;

    /** Construct a new MediaStreamRequest object
     *  @param url [in] the url for the media request
     *  @param byteRange [in] The byte range of the request
     *  @param pMediaBufferPool [in] The media buffer pool
     *  @param mediaType
     */
    JsBridgeMediaRequest(   AseUrl const&               url,
                            ase::ByteRange const&       byteRange );


    virtual ase::MediaType          getMediaBufferType() const { return ase::MEDIA_UNKNOWN; }

    virtual void                    setListener( IJsBridgeMediaRequestListenerPtr pListener );

    uint32_t                        getBytesReceived() const;

    uint32_t                        getTotalBytes() const;

    bool                            received() const;

    /** Returns true if this object is a subclass of MediaSourceAppendable */
    virtual bool                    isAppendable() const { return false; }

    /** Indicates that media buffer space has been reserved for this request */
    virtual void                    bufferReserved() {}

    // MediaRequest interfaces
    virtual ase::ByteRange const&   getByteRange( ) const;
    virtual AseTimeVal              getCompletionDeadline() const;
    virtual void                    requestStarted( bool connecting, bool pipelined );
    virtual void                    reportHttpResponseTime( AseTimeVal const& httpResponseTime );
    virtual void                    abandoned();

    virtual void                    requestError( int32_t status, uint32_t httpCode );

    virtual void                    provideFileSize( ullong size );

    virtual void                    provideResponseHeaders( HttpResponseHeaders headers );

    /** Return true if this request has a URL assigned */
    virtual bool                    hasUrl() const;

    /** Select the URL and return true if it has changed */
    virtual bool                    selectUrl();

    /** Get the URL for this request */
    virtual AseUrl                  getCurrentUrl();

    virtual void                    swapUrl(AseUrl const& url);

    virtual void                    loadStarted();

public:
    virtual ~JsBridgeMediaRequest() {}

    void firstBytesReceived( AseTimeVal const& now );

    static uint32_t computeNumberOfBlocks(uint32_t fragmentSize, uint32_t bufferUnitSize);

protected:
    ase::ByteRange const        mByteRange;

    uint32_t const              mTotalBytes;

    AseTimeVal                  mLoadStartTime; // When the request is sent

    AseTimeVal                  mFirstByteReceptionTime;

    uint32_t                    mBytesReceived;

    AseTimeVal                  mRequestStartTime;

    bool                        mNewConnection;

    uint32_t                    mHttpResponseTime;

    HttpResponseHeaders         mResponseHeaders;

    IJsBridgeMediaRequestListenerPtr mListener;

    bool                        mHasUrlChanged;

    AseUrl                      mCurrentUrl;
};

inline void JsBridgeMediaRequest::swapUrl(AseUrl const& url)
{
    mHasUrlChanged = true;
    mCurrentUrl = url;
}

inline bool JsBridgeMediaRequest::hasUrl() const
{
    return !mCurrentUrl.IsEmpty();
}

inline bool JsBridgeMediaRequest::selectUrl()
{
    bool const result = mHasUrlChanged;
    mHasUrlChanged = false;
    return result;
}

inline AseUrl JsBridgeMediaRequest::getCurrentUrl()
{
    return mCurrentUrl;
}

inline bool JsBridgeMediaRequest::received() const
{
    return mTotalBytes == mBytesReceived;
}

inline void JsBridgeMediaRequest::provideResponseHeaders( HttpResponseHeaders headers )
{
    mResponseHeaders = headers;
}

}}
#endif

