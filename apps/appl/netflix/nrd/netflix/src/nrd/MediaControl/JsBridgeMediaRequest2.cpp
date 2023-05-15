/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "JsBridgeMediaRequest2.h"

#include <nrdbase/Log.h>

// TODO:
// * clean up: remove the pts and stream header information from IMediaBlockProvider

using namespace netflix;
using namespace netflix::ase;
using namespace std;
using namespace netflix::mediacontrol;

JsBridgeMediaRequest::JsBridgeMediaRequest(
                                               AseUrl const&        url,
                                               ByteRange const&     byteRange )
                        : mByteRange(byteRange)
                        , mTotalBytes(byteRange.size())
                        , mLoadStartTime(AseTimeVal::ZERO)
                        , mFirstByteReceptionTime(AseTimeVal::ZERO)
                        , mBytesReceived(0)
                        , mRequestStartTime(AseTimeVal::ZERO)
                        , mNewConnection(false)
                        , mHttpResponseTime( 0 )
                        , mListener(IJsBridgeMediaRequestListenerPtr())
                        , mHasUrlChanged(false)
                        , mCurrentUrl(url)

{
}


ByteRange const& JsBridgeMediaRequest::getByteRange( ) const
{
    return mByteRange;
}

AseTimeVal JsBridgeMediaRequest::getCompletionDeadline() const
{
    // No Need to Implement. This is for the block abandon feature.
    return AseTimeVal::INFINITE;
}

void JsBridgeMediaRequest::reportHttpResponseTime( AseTimeVal const& httpResponseTime )
{
    mHttpResponseTime = httpResponseTime.ms();
}

void JsBridgeMediaRequest::requestStarted( bool connecting, bool pipelined )
{
    if ( !pipelined )
    {
        mRequestStartTime = AseTimeVal::now();
        mNewConnection = connecting;
    }
}

void JsBridgeMediaRequest::abandoned()
{
    if (mListener)
        mListener->onError(mRequestId, AseTimeVal::now(), AS_ABANDONED, 0);
}

void JsBridgeMediaRequest::provideFileSize( ullong /*size*/ )
{
}

void JsBridgeMediaRequest::requestError( int32_t status, uint32_t httpCode )
{
    if ( hasUrl() )
    {
        if (mListener)
            mListener->onError(mRequestId, AseTimeVal::now(), status, httpCode );

        mCurrentUrl = AseUrl();
    }
}

uint32_t JsBridgeMediaRequest::getTotalBytes() const
{
    return mTotalBytes;
}

void JsBridgeMediaRequest::loadStarted()
{
    mLoadStartTime = AseTimeVal::now();
    if (mListener)
        mListener->onLoadStart(mRequestId, mLoadStartTime);
}

void JsBridgeMediaRequest::firstBytesReceived( AseTimeVal const& now  )
{
    mFirstByteReceptionTime = now;
    if (mListener)
        mListener->onFirstByteReceived(mRequestId, mFirstByteReceptionTime,mRequestStartTime,mResponseHeaders, mNewConnection );
}

void JsBridgeMediaRequest::setListener(IJsBridgeMediaRequestListenerPtr pListener)
{
    mListener = pListener;
}

uint32_t JsBridgeMediaRequest::getBytesReceived() const
{
    return mBytesReceived;
}






