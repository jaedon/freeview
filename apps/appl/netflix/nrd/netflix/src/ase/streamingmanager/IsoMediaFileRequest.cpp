/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "IsoMediaFileRequest.h"
#include "IsoMediaFile.h"
#include <nrdase/ILocationSelector.h>
#include <nrdase/ILocationSetMonitor.h>
#include <nrdase/IStreamingLocationSet.h>

using namespace netflix;
using namespace ase;

AseErrorCode IsoMediaFileRequest::construct(    IsoMediaFilePtr         pMediaFile,
                                                ByteRange               byteRange,
                                                IsoMediaFileRequestPtr& pRequest )
{
    //DEBUGLOG( IsoMediaFile, "IsoMediaFileRequest::construct(): range=%s",
    //         byteRange.toByteRangeSpec().c_str() );

    // Validate byteRange
    if ( !byteRange.valid() || !byteRange.closed() )
        return AS_INVALID_VALUE;

    // Create object
    pRequest.reset( new IsoMediaFileRequest (   pMediaFile,
                                                byteRange ) );

    return AS_NO_ERROR;
}

IsoMediaFileRequest::IsoMediaFileRequest(
                            IsoMediaFilePtr        pMediaFile,
                            ByteRange               byteRange )

    : MediaPresentationRequest( true )
    , mIsoMediaFilePtr( pMediaFile )
    , mByteRange( byteRange )
    , mDelivered( 0 )
    , mbIgnore( false )
{
}

void IsoMediaFileRequest::ignore()
{
    mResponseBuffers.clear();
    mbIgnore = true;
}

#if 0
ManifestUrls const& IsoMediaFileRequest::getUrls( ) const
{
    return mUrls;
}
#endif

ByteRange const& IsoMediaFileRequest::getByteRange( ) const
{
    return mByteRange;
}

ByteRange IsoMediaFileRequest::getUndeliveredByteRange() const
{
    return ByteRange( mByteRange.getStart() + mDelivered * sBufferSize, mByteRange.getEnd() );
}

size_t IsoMediaFileRequest::getBufferSize() const
{
    return sBufferSize;
}

AseErrorCode IsoMediaFileRequest::getResponseBodyBuffer( AseBufferPtr& pBuffer )
{
    HeapBufferPtr pHeapBuffer;
    AseErrorCode const retVal = HeapBuffer::construct( sBufferSize, pHeapBuffer );
    pBuffer = pHeapBuffer;
    return retVal;
}

int IsoMediaFileRequest::provideResponseBody(
                                AseBufferPtr const& pBuffer,
                                int sequence,
                                AseTimeVal& abandontime )
{
    //DEBUGLOG( IsoMediaFile, "IsoMediaFileRequest::provideResponseBody(%llu-%llu:%d, %d): %p",
    //            mByteRange.getStart(), mByteRange.getEnd(), sequence, mDelivered, pBuffer.get() );

    abandontime = AseTimeVal::INFINITE;

    if ( mbIgnore )
        return 1;

    IsoMediaFilePtr const pIsoMediaFile = mIsoMediaFilePtr.lock();
    if ( !pIsoMediaFile )
    {
        abandontime = AseTimeVal::ZERO;
        return 1;
    }

    sequence -= mDelivered;

    if ( (unsigned int) sequence > mResponseBuffers.size() )
    {
        mResponseBuffers.resize( sequence );
    }

    if ( (unsigned int) sequence == mResponseBuffers.size() )
    {
        mResponseBuffers.push_back( pBuffer );
    }
    else
    {
        mResponseBuffers[ sequence ] = pBuffer;
    }

    // Deliver the buffers in order to the ISO Media File
    while( !mResponseBuffers.empty() && mResponseBuffers.front() )
    {
        ullong const offset = mByteRange.getStart() + mDelivered * sBufferSize;
        bool const bLastBuffer = ( ( ( mByteRange.getSize() - 1 ) / sBufferSize ) == mDelivered );

        //DEBUGLOG( IsoMediaFile, "IsoMediaFileRequest providing respose buffer %d: offset %llu, %slast",
        //                            mDelivered,
        //                            offset,
        //                            ( bLastBuffer ? "" : "not " ) );

        int retCode = pIsoMediaFile->provideResponseBody(   shared_from_this(),
                                                            mResponseBuffers.front(),
                                                            offset,
                                                            bLastBuffer );
        if ( retCode != AS_NO_ERROR )
            return retCode;

        ++mDelivered;
        mResponseBuffers.erase( mResponseBuffers.begin() );
    }

    return mDelivered > ( ( mByteRange.getSize() - 1 ) / sBufferSize ) ? 1 : AS_NO_ERROR;
}

void IsoMediaFileRequest::abandoned()
{
    //DEBUGLOG( IsoMediaFile, "IsoMediaFileRequest::provideFailureCode(): %d",
    //         code );

    if ( !mbIgnore )
    {
        IsoMediaFilePtr const pIsoMediaFile = mIsoMediaFilePtr.lock();
        if ( pIsoMediaFile ) pIsoMediaFile->provideFailureCode( AS_ABANDONED );
    }
}

void IsoMediaFileRequest::provideFileSize( ullong size )
{
    IsoMediaFilePtr const pIsoMediaFile = mIsoMediaFilePtr.lock();
    if ( pIsoMediaFile ) pIsoMediaFile->provideFileSize( size );
}

void IsoMediaFileRequest::requestError( AseErrorCode status, uint32_t httpCode )
{
    if ( hasUrl() )
    {
        mStreamingLocationSetPtr->reportHttpError( mCurrentUrl, status, httpCode );

        mCurrentUrl.reset();
    }
}

AseUrl IsoMediaFileRequest::getCurrentUrl()
{
    return mCurrentUrl->getUrl();
}

bool IsoMediaFileRequest::selectUrl()
{
    assert( getLocationSelector() );

    IsoMediaFilePtr const pIsoMediaFile = mIsoMediaFilePtr.lock();
    if ( !pIsoMediaFile ) return false;

    bool urlChanged = false;

    AseErrorCode errorCode =
        getLocationSelector()->
            getPreferredLocationSet(    pIsoMediaFile->getStreamKey(),
                                        ILocationSelector::HEADER_DOWNLOADING,
                                        mStreamingLocationSetPtr );
    if ( errorCode == AS_NO_ERROR )
    {
        setThroughputMonitor( mStreamingLocationSetPtr->getLocationSetMonitor() );

        ConstManifestUrlPtr const latestUrl = mStreamingLocationSetPtr->getDownloadableUrl();
        if ( !mCurrentUrl.get() || (latestUrl->getUrl() != mCurrentUrl->getUrl()) )
        {
            mCurrentUrl = latestUrl;
            urlChanged = true;
        }
    }
    else if ( !isAbandoned() )
    {
        setAbandoned();

        mCurrentUrl.reset();

        urlChanged = true;

        IsoMediaFilePtr const pIsoMediaFile = mIsoMediaFilePtr.lock();
        if ( pIsoMediaFile ) pIsoMediaFile->provideFailureCode( errorCode );
    }

    return urlChanged;
}


bool IsoMediaFileRequest::hasUrl() const
{
    return mCurrentUrl.get();
}

