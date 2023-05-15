/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaStreamRequest.h"

#include <nrdase/ILocationSelector.h>
#include <nrdase/IStreamingLocationSet.h>

using namespace netflix::ase;
using namespace std;
using namespace netflix;

int MediaStreamRequest::construct( ManifestUrls const&        urls,
                          ByteRange const&              byteRange,
                          IMediaBufferPoolPtr const&   pMediaBufferPool,
                          shared_ptr<IMediaFragment>    pFragment,
                          std::string const& streamKey,
                          MediaType mediaType,
                          uint32_t cdnid,
                          MediaStreamRequestPtr&    pRequest )
{
    pRequest.reset(new
            MediaStreamRequest(urls, byteRange, pMediaBufferPool,
                               pFragment, streamKey, mediaType, cdnid) );
    return AS_NO_ERROR;
}

MediaStreamRequest::MediaStreamRequest( ManifestUrls const&        urls,
                          ByteRange const&              byteRange,
                          IMediaBufferPoolPtr const&   pMediaBufferPool,
                          shared_ptr<IMediaFragment>    pFragment,
                          std::string const& streamKey,
                          MediaType mediaType,
                          uint32_t cdnid) :
                        MediaPresentationRequest( false ),
                        mMediaFragmentPtr(pFragment),
                        mMediaBufferPoolPtr(pMediaBufferPool),
                        mUrls(urls), mByteRange(byteRange),
                        mStreamKey( streamKey ),
                        mMediaType(mediaType),
                        mCdnId(cdnid)
{
}


#if 0
ManifestUrls const& MediaStreamRequest::getUrls( ) const
{
    return mUrls;
}
#endif

ByteRange const& MediaStreamRequest::getByteRange( ) const
{
    return mByteRange;
}

AseTimeVal MediaStreamRequest::getCompletionDeadline() const
{
    return mMediaFragmentPtr->getCompletionDeadline();
}

size_t MediaStreamRequest::getBufferSize() const
{
    return mMediaBufferPoolPtr->getUnitSize();
}

AseErrorCode MediaStreamRequest::getResponseBodyBuffer(AseBufferPtr& pMediaBuffer)
{
    if ( !mMediaFragmentPtr->isBufferAvailable() )
    {
        return AS_NOT_ENOUGH_MEMORY;
    }

    AseErrorCode retCode = mMediaBufferPoolPtr->allocate(mMediaType, mCdnId, pMediaBuffer);
    if ( retCode == AS_NOT_ENOUGH_MEMORY )
        mMediaFragmentPtr->bufferFull();

    return retCode;
}

int MediaStreamRequest::provideResponseBody(   AseBufferPtr const& pBuffer,
                                        int sequence,
                                        AseTimeVal& timeout )
{
    return mMediaFragmentPtr->provideResponseBody( pBuffer, sequence, timeout );
}

void MediaStreamRequest::abandoned()
{
    mMediaFragmentPtr->provideFailureCode( AS_ABANDONED );
}

void MediaStreamRequest::requestError( AseErrorCode status, uint32_t httpCode )
{
    if ( hasUrl() )
    {
        mStreamingLocationSetPtr->reportHttpError( mCurrentUrl, status, httpCode );

        mCurrentUrl.reset();
    }
}

void MediaStreamRequest::provideFileSize( ullong /*size*/ )
{
}

AseUrl MediaStreamRequest::getCurrentUrl()
{
    return mCurrentUrl->getUrl();
}

bool MediaStreamRequest::selectUrl()
{
    assert( getLocationSelector() );

    bool urlChanged = false;

    ILocationSelector::LocationSelectionRequestSource requestSource =
                    (mMediaType == MEDIA_VIDEO) ?
                            ILocationSelector::VIDEO_DOWNLOADING : ILocationSelector::AUDIO_DOWNLOADING;

    AseErrorCode errorCode =
        getLocationSelector()->
            getPreferredLocationSet(mStreamKey, requestSource, mStreamingLocationSetPtr);
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

        mMediaFragmentPtr->provideFailureCode( errorCode );
    }

    return urlChanged;
}


bool MediaStreamRequest::hasUrl() const
{
    return static_cast<bool>(mCurrentUrl);
}

