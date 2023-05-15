/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AppendableMediaRequest.h"

#include <nrdbase/Log.h>

// TODO:
// * clean up: remove the pts and stream header information from IMediaBlockProvider

using namespace netflix;
using namespace netflix::ase;
using namespace std;
using namespace netflix::mediacontrol;

AppendableMediaRequest::AppendableMediaRequest(
                                               AseUrl const&        url,
                                               ByteRange const&     byteRange,
                                               IMediaBufferPoolPtr  pMediaBufferPool,
                                               MediaType            mediaType)
                        : JsBridgeMediaRequest( url, byteRange )
                        , mMediaBufferPoolPtr(pMediaBufferPool)
                        , mMediaType(mediaType)
                        , mData( computeNumberOfBlocks( mTotalBytes, pMediaBufferPool->getUnitSize() ) )
                        , mNextBlock( 0 )
                        , mReservedUnits( 0 )
{
    NTRACE(TRACE_MEDIACONTROL, "AppendableMediaRequest created");
}

AppendableMediaRequest::~AppendableMediaRequest()
{
    mMediaBufferPoolPtr->unreserveUnits( mMediaType, mReservedUnits );
    NTRACE(TRACE_MEDIACONTROL,"AppendableMediaRequest deleted with %u reserved blocks", mReservedUnits );
}

void AppendableMediaRequest::bufferReserved()
{
    mReservedUnits = mData.size();
}

size_t AppendableMediaRequest::getBufferSize() const
{
    return mMediaBufferPoolPtr->getUnitSize();
}

AseErrorCode AppendableMediaRequest::getResponseBodyBuffer(AseBufferPtr& pMediaBuffer)
{
    if ( mReservedUnits ) --mReservedUnits;
    return mMediaBufferPoolPtr->allocate(mMediaType, /**deprecated*/0, pMediaBuffer);
}

int AppendableMediaRequest::provideResponseBody( AseBufferPtr const& pBuffer, int sequence, AseTimeVal& abandontime )
{
    if ( isAbandoned() )
    {
        abandontime = AseTimeVal::ZERO;
        return 0;
    }
    else
    {
        abandontime = AseTimeVal::INFINITE;
    }

    AseTimeVal const now = AseTimeVal::now();

    if ( mFirstByteReceptionTime == AseTimeVal::ZERO )
        firstBytesReceived( now );

    assert( !mData[ sequence ] );

    mData[ sequence ] = pBuffer;

    mBytesReceived += pBuffer->getFilledSpaceSize();

    if ( mListener )
    {
        uint32_t elapsedTimeInMs = now > mFirstByteReceptionTime ? (now-mFirstByteReceptionTime).ms() : 0;
        if ( mBytesReceived == mTotalBytes )
        {
            mListener->onComplete(mRequestId, now, elapsedTimeInMs);

            NTRACE(TRACE_MEDIACONTROL,
                    "Complete downloading for media: %u, request: %u, byte range: %lld-%lld",
                    mMediaType,
                    mRequestId,
                    mByteRange.getStart(), mByteRange.getEnd());
        }
        else
        {
            mListener->onProgress(mRequestId, now, elapsedTimeInMs, mBytesReceived, mHttpResponseTime);

            mHttpResponseTime = 0;
        }
    }

    // If this is the frontmost block, tell the MediaSourceAppendable base class
    if ( sequence == static_cast<int>(mNextBlock) )
        newBlocksAvailable();

    return 0;
}

MediaType AppendableMediaRequest::getMediaBufferType() const
{
    return mMediaType;
}

bool AppendableMediaRequest::forwardBlocks()
{
    for ( ; mNextBlock < mData.size() && mData[ mNextBlock ]; ++mNextBlock )
    {
        forwardBlock( mData[ mNextBlock ] );

        mData[ mNextBlock ].reset();
    }

    return ( mNextBlock == mData.size() );
}







