/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "BufferedMediaRequest.h"

#include <nrdbase/Log.h>
#include <nrdase/AseDataBuffer.h>

// TODO:
// * clean up: remove the pts and stream header information from IMediaBlockProvider

using namespace netflix;
using namespace netflix::ase;
using namespace std;
using namespace netflix::mediacontrol;

BufferedMediaRequest::BufferedMediaRequest( AseUrl const&       url,
                                            ByteRange const&    byteRange,
                                            uint32_t            progressInterval )
                        : JsBridgeMediaRequest( url, byteRange )
                        , mProgressInterval( progressInterval )
{
}

size_t BufferedMediaRequest::getBufferSize() const
{
    return mTotalBytes;
}

AseErrorCode BufferedMediaRequest::getResponseBodyBuffer(AseBufferPtr& pMediaBuffer)
{
    assert( mDataBuffer.size() == 0 );

    mDataBuffer.reserve( mTotalBytes );

    if ( !mDataBuffer.constData() )
        return AS_NOT_ENOUGH_MEMORY;

    AseDataBufferPtr pAseDataBuffer;

    AseDataBuffer::construct( mDataBuffer, pAseDataBuffer );

    pMediaBuffer = pAseDataBuffer;

    if ( !pMediaBuffer )
    {
        mDataBuffer.reserve( 0 );
        return AS_NOT_ENOUGH_MEMORY;
    }

    return AS_NO_ERROR;
}

void BufferedMediaRequest::reportProgress( size_t bytesReceived )
{
    AseTimeVal const now = AseTimeVal::now();

    if ( mFirstByteReceptionTime == AseTimeVal::ZERO )
        firstBytesReceived( now );

    mBytesReceived = bytesReceived;

    if ( mListener )
    {
        uint32_t elapsedTimeInMs = now > mFirstByteReceptionTime ? (now-mFirstByteReceptionTime).ms() : 0;

        mListener->onProgress(mRequestId, now, elapsedTimeInMs, mBytesReceived, mHttpResponseTime);

        mHttpResponseTime = 0;
    }
}

int BufferedMediaRequest::provideResponseBody( AseBufferPtr const& pBuffer, int sequence, AseTimeVal& abandontime )
{
    NRDP_UNUSED( pBuffer );
    NRDP_UNUSED( sequence );
    assert( sequence == 0 );
    assert( pBuffer->getFilledSpaceStart() == mDataBuffer.constData() );
    assert( pBuffer->getFilledSpaceSize() == mTotalBytes );
    assert( mBytesReceived == mTotalBytes );

    mDataBuffer.resize( mTotalBytes );

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

    if ( mListener )
    {
        uint32_t elapsedTimeInMs = now > mFirstByteReceptionTime ? (now-mFirstByteReceptionTime).ms() : 0;

        mListener->onComplete(mRequestId, now, elapsedTimeInMs, mDataBuffer );

        NTRACE(TRACE_MEDIACONTROL,
                "Complete downloading for request: %u, byte range: %lld-%lld",
                mRequestId,
                mByteRange.getStart(), mByteRange.getEnd());
    }

    return 0;
}

// helper functions

