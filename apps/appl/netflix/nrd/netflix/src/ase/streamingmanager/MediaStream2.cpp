/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#include "MediaStream2.h"
#include "MediaFragment.h"
#include "MediaStreamRequest.h"
#include "MediaFileFactory.h"

#include "IStreamingLocationSet.h"

using namespace netflix::ase;
using namespace std;

namespace netflix {
namespace ase {
DEFINE_LOGGER( MediaStreamLog );
}
}

int MediaStream::construct( shared_ptr<MediaTrack> pMediaTrack,
                            shared_ptr<MediaPresentation>    pMediaPresentation,
                            ManifestStreamPtr pManifestStream,
                            IMediaSinkPtr const&           pMediaSink,
                            int32_t     manifestId,
                            int32_t     streamId,
                            IMediaBufferPoolPtr const&   pMediaBufferPool,
                            ILocationSelectorPtr const&  pLocationSelector,
                            IAseConfigParameterPtr pConfigParameters,
                            AseTimeStamp const& timestampOffset,
                            IMediaFilePtr pPreExistingMediaFile,
                            MediaStreamPtr&               pMediaStream )
{
    IMediaFilePtr pMediaFile;
    AseErrorCode retCode = AS_NO_ERROR;

    if ( pPreExistingMediaFile.get() )
        pMediaFile = pPreExistingMediaFile;
    else
        retCode = MediaFileFactory::createMediaFile( pConfigParameters, pManifestStream, pMediaFile );

    if ( retCode == AS_NO_ERROR )
    {
        pMediaStream.reset( new MediaStream( pMediaTrack, pMediaPresentation,
                                             pManifestStream,
                                             pConfigParameters,
                                             pMediaFile, pMediaSink,
                                             pMediaBufferPool,
                                             pLocationSelector,
                                             timestampOffset,
                                             manifestId,
                                             streamId ) );
    }

    return retCode;
}

MediaStream::MediaStream( shared_ptr<MediaTrack> pMediaTrack,
                          shared_ptr<MediaPresentation>    pMediaPresentation,
                          ManifestStreamPtr pManifestStream,
                          IAseConfigParameterPtr pConfigParameters,
                          IMediaFilePtr pMediaFile,
                          IMediaSinkPtr const&           pMediaSink,
                          IMediaBufferPoolPtr const&   pMediaBufferPool,
                          ILocationSelectorPtr const&  pLocationSelector,
                          AseTimeStamp const& timestampOffset,
                          int32_t manifestId,
                          int32_t streamId) :
                          mManifestStreamPtr( pManifestStream ),
                          mMediaTrackPtr( pMediaTrack ),
                          mMediaPresentationPtr(pMediaPresentation),
                          mConfigParameterPtr(pConfigParameters),
                          mMediaFilePtr(pMediaFile),
                          mMediaSinkPtr(pMediaSink),
                          mMediaBufferPoolPtr(pMediaBufferPool),
                          mLocationSelectorPtr(pLocationSelector),
                          mIndexComplete( false ),
                          mMediaHeaderPtr(),
                          mHeaderDelivered(false),
                          mGopSize(0),
                          mStreamingTime( timestampOffset ),
                          mNextFragmentId(0),
                          mManifestId(manifestId),
                          mStreamId(streamId),
                          mRemainingBlocks(0),
                          mCurrentFragmentStartPts( timestampOffset ),
                          mCurrentFragmentStartByteOffset(0),
                          mCurrentFragmentIndex(-1),
                          mPrevFragmentStartPts(timestampOffset),
                          mPrevFragmentStartByteOffset(0),
                          mPrevFragmentIndex(-1),
                          mFirstTimeAdjustStreamingTime(true),
                          mInitFragmentStartPts(timestampOffset),
                          mInitFragmentStartByteOffset(0),
                          mInitFragmentIndex(-1),
                          mDuration(0),
                          mAvailable(true),
                          mPredictDlTimeForBackup(-1),
                          mTimeStampOffset(timestampOffset)
{
    getConfiguration();
}

void MediaStream::configure( IAseConfigParameterPtr pConfigParameters )
{
    mConfigParameterPtr = pConfigParameters;

    getConfiguration();
}

void MediaStream::getConfiguration()
{
    mMaxDataDryPeriod = mConfigParameterPtr->getParameterAs<uint32_t>("maxDataDryPeriod", 2000);
    // Minimum time we will wait for a block on a slow connection before closing the connnection
    mMinBlockCompleteTime =
                mConfigParameterPtr->getParameterAs<uint32_t>("minBlockCompleteTime", 0, 100000, 10000);

    // Fraction of buffer, in %, that we will wait for a block on a slow connection before closing the connnection
    mBlockReceptionBufferBudget =
                mConfigParameterPtr->getParameterAs<uint32_t>("blockReceptionBufferBudget", 0, 100, 50);

    // Do not immediately abandon this block before the player has spent abandonBlockControlTime on it
    mAbandonBlockControlTime =
                mConfigParameterPtr->getParameterAs<uint32_t>("abandonBlockControlTime", 2002);

    // Do not set a block abandon timer unless we have received a minimum fraction of a block
    mAbandonBlockControlPct =
                mConfigParameterPtr->getParameterAs<uint32_t>("abandonBlockControlPct", 0, 100, 20);

    bool const abandonBlockTimerEnable =
                mConfigParameterPtr->getParameterAs<bool>("abandonBlockTimerEnable", false );

    if ( !abandonBlockTimerEnable )
        mAbandonBlockControlPct = 100;


    // With the current abandon block implementation, we need to enforce the rule of not forwarding
    // a block before downloading complete when abanddon block feature is used
    if ( abandonBlockTimerEnable )
        mForwardDataBeforeComplete = false;
    else
    {
        mForwardDataBeforeComplete =
            mConfigParameterPtr->getParameterAs<bool>("forwardDataBeforeComplete", true);
    }

}

AseTimeStamp MediaStream::updateStreamingTime( AseTimeStamp const& currentTime )
{
    //printf("MediaStream::updateStreamingTime: current time: %llu ms\n",
      //  currentTime.getValueInMs() );
    mStreamingTime = currentTime;
    if ( !mMediaIndexPtr
            || mMediaIndexPtr->empty()
            || !mIndexComplete
            || ( mCurrentFragmentStartPts == currentTime ) )
        return mStreamingTime;

    AseTimeStamp currentFragmentTime = mCurrentFragmentStartPts;
    ullong currentFragmentByteOffset = mCurrentFragmentStartByteOffset;
    int index = 0;
    /*printf( "MediaStream::updateStreamingTime, streaming time: %llu ms, current fragment time: %llu ms, byte offset: %llu, index: %u\n",
        mStreamingTime.getValueInMs(),
        mCurrentFragmentStartPts.getValueInMs(), mCurrentFragmentStartByteOffset,
        mCurrentFragmentIndex );*/

    if ( mCurrentFragmentStartPts < mStreamingTime )
    {
        // resolving the problem that a/v duration can be different
        int32_t maxIndex;
        if ( mFirstTimeAdjustStreamingTime )
        {
            maxIndex = mMediaIndexPtr->size() - 2;
            mFirstTimeAdjustStreamingTime = false;
        }
        else
            maxIndex = mMediaIndexPtr->size();

        // search forward
        for ( index = mCurrentFragmentIndex; index < maxIndex; ++index )
        {
            if ( currentFragmentTime >= mStreamingTime )
            {
                break;
            }
            else
            {
                currentFragmentTime += (*mMediaIndexPtr)[index].mDuration;
                currentFragmentByteOffset += (*mMediaIndexPtr)[index].mSize;
            }
        }
    }
    else if ( mCurrentFragmentStartPts > mStreamingTime )
    {
        DEBUGLOG( MediaStreamLog,
                "Rewind: updateStreamingTime, streaming time: %llu ms, "
                "current fragment time: %llu ms, byte offset: %llu, index: %u",
                mStreamingTime.getValueInMs(),
                mCurrentFragmentStartPts.getValueInMs(), mCurrentFragmentStartByteOffset,
                mCurrentFragmentIndex );
        // search backward
        for ( index = mCurrentFragmentIndex; index > 0; --index )
        {
            if ( currentFragmentTime <= mStreamingTime )
            {
                break;
            }
            else
            {
                currentFragmentTime -= (*mMediaIndexPtr)[index-1].mDuration;
                currentFragmentByteOffset -= (*mMediaIndexPtr)[index-1].mSize;
            }
        }
    }

    if ( (( index > 0 ) && (currentFragmentTime > mStreamingTime)) || (index == (int)mMediaIndexPtr->size()) )
    {
        mCurrentFragmentIndex = index - 1;
        mCurrentFragmentStartPts = currentFragmentTime - (*mMediaIndexPtr)[index-1].mDuration;
        mCurrentFragmentStartByteOffset = currentFragmentByteOffset - (*mMediaIndexPtr)[index-1].mSize;
    }
    else
    {
        mCurrentFragmentIndex = index;
        mCurrentFragmentStartPts = currentFragmentTime;
        mCurrentFragmentStartByteOffset = currentFragmentByteOffset;
    }

    /*printf( "MediaStream::updateStreamingTime, trackid: %d, current streaming time: %llu ms, byte offset: %llu, index: %u\n",
                mMediaTrackPtr->trackId(),
                mCurrentFragmentStartPts.getValueInMs(), mCurrentFragmentStartByteOffset,
                mCurrentFragmentIndex );*/

    mStreamingTime = mCurrentFragmentStartPts;

    return mStreamingTime;
}

bool MediaStream::hasPendingRequest(uint32_t currentBufferLen)
{
    NRDP_UNUSED(currentBufferLen);
    // The stream either needs to download its header, its index or a remaining block from the
    // previous getNextRequest()
    if ( !mMediaFilePtr->good() )
    {
        AseErrorCode error = mMediaFilePtr->getLastError();
        if ( error == AS_PARSE_ERROR )
        {
            error = mMediaHeaderPtr ? AS_MEDIA_INDEX_ERROR : AS_MEDIA_HEADER_ERROR;
        }
        mMediaTrackPtr.lock()->reportFailure( error, mStreamId, -1 );
        return false;
    }

    return ( !mMediaFilePtr->hasHeader()
                || !mMediaFilePtr->indexComplete()
                || (mRemainingBlocks > 0) );
}

AseErrorCode MediaStream::getNextRequest( AseTimeStamp& newStreamingTime,
                                          MediaPresentationRequestPtr& pRequest )
{
    // get next request for the pending request
    return getNextRequest(0, newStreamingTime, mPredictDlTimeForBackup, 0, pRequest);
}


AseErrorCode MediaStream::getNextRequest(   int32_t             numOfBlocks,
                                            AseTimeStamp&       newStreamingTime,
                                            int32_t predictedDlTimeForBackupStream,
                                            uint32_t alternativeFragmentSize,
                                            MediaPresentationRequestPtr&   pRequest)
{
    DEBUGLOG( MediaStreamLog,
            "[%s] MediaStream::getNextRequest, numOfBlocks: %d, streaming time: %llu ms",
                                    streamKey().c_str(), numOfBlocks, mStreamingTime.getValueInMs());
    AseErrorCode retCode = AS_PENDING;
    if ( numOfBlocks > 0)
        mRemainingBlocks = numOfBlocks;

    newStreamingTime = mStreamingTime;
    mPredictDlTimeForBackup = predictedDlTimeForBackupStream;

    MediaStreamRequestPtr pMSRequest;

    if ( getHeader()
            && getIndex()
            // FIXME Make 60s index horizon configurable!
            // FIXME mIndexEndPts == mIndexEndPts ???
            && mIndexComplete
            && (mRemainingBlocks > 0) )
    {
        // normal behaviour
        MediaFragmentPtr pMediaFragment;
        retCode = createMediaStreamRequest( pMediaFragment, newStreamingTime,
                                            predictedDlTimeForBackupStream,
                                            alternativeFragmentSize, pMSRequest );
        if ( retCode == AS_NO_ERROR )
        {
            mMediaTrackPtr.lock()->provideMediaFragment(pMediaFragment);
        }

        pRequest = pMSRequest;
    }
    else
    {
        retCode = mMediaFilePtr->getNextRequest( pRequest );
        DEBUGLOG( MediaStreamLog, "[%s] MediaStream::getNextRequest: Making a media file download request %d",
            streamKey().c_str(),
            retCode );
    }

    if ( pRequest )
    {
        pRequest->setOpenRangeRequest( ( mMediaTrackPtr.lock()->mediaType() == MEDIA_AUDIO ) );
    }

    return retCode;
}


int32_t MediaStream::computeNumOfBlocksToRequest(
                        uint32_t maxRequestSize, uint32_t minRequestSize,
                        int32_t maxRequestLen) const
{
    if ( !mMediaIndexPtr
            || mMediaIndexPtr->empty()
            || !mIndexComplete )
        return 0;

    int32_t numOfBlocks = 0;
    uint32_t requestByteSize = 0;
    uint32_t requestTimeLen = 0;

    int32_t nextFragmentIndex = mCurrentFragmentIndex;

    while ( nextFragmentIndex < (int32_t)mMediaIndexPtr->size() )
    {
        requestByteSize += (*mMediaIndexPtr)[nextFragmentIndex].mSize;
        if ( requestByteSize > maxRequestSize )
            break;

        requestTimeLen += (*mMediaIndexPtr)[nextFragmentIndex].mDuration.getValueInMs();
        ++numOfBlocks;
        if ( (requestByteSize >= minRequestSize) ||
                ((maxRequestLen > 0) && ((int32_t)requestTimeLen > maxRequestLen)) )
        {
            break;
        }
        ++nextFragmentIndex;
    }

    if ( (nextFragmentIndex == (int32_t)mMediaIndexPtr->size()) && (numOfBlocks > 1) )
        --numOfBlocks;

    return numOfBlocks;
}



bool MediaStream::deliverHeader(uint32_t& duration, uint32_t& gopSize)
{
    if ( mHeaderDelivered )
    {
        gopSize = mGopSize;
        duration = mDuration;
        return true;
    }

    AseTimeInterval fragmentDuration;
    if ( getHeader() && mMediaFilePtr->getFragmentDuration(fragmentDuration) )
    {
        mGopSize = fragmentDuration.getValueInMs();
        gopSize = mGopSize;
        duration = mDuration;
        mHeaderDelivered = true;
    }

    return mHeaderDelivered;
}

bool MediaStream::indexComplete()
{
    bool complete = false;
    if (getIndex())
        complete = mMediaFilePtr->indexComplete();

    return complete;
}

void MediaStream::
completeForwarding( AseTimeStamp const& fragmentStartPts,
                    AseTimeStamp const& duration,
                    uint32_t numOfBlocks, bool lastFragment)
{
    // TODO: check usage of numOfBlocks
    mMediaTrackPtr.lock()->completeForwarding(fragmentStartPts, duration, numOfBlocks, lastFragment);
}

uint32_t MediaStream::getContentDuration()
{
    return mDuration;
}

bool MediaStream::getHeader()
{
    if ( mMediaHeaderPtr )
        return true;

    if ( !mMediaFilePtr->hasHeader() )
        return false;

    if ( !mMediaFilePtr->getHeader( mMediaHeaderPtr ) )
        return false;

    mMediaSinkPtr->newStream( mMediaHeaderPtr->getFilledSpaceStart(),
                              mMediaHeaderPtr->getFilledSpaceSize(),
                              mManifestId,
                              mMediaTrackPtr.lock()->trackId(),
                              mStreamId );

    if ( mDuration == 0 )
    {
        AseTimeInterval duration;
        if ( mMediaFilePtr->getDuration( duration ) )
        {
            mDuration = duration.getValueInMs();

            mMediaTrackPtr.lock()->updateContentDuration( mDuration );
            mMediaTrackPtr.lock()->
                reportNewStreamDuration(mManifestStreamPtr->getStreamKey(), mDuration );
        }
    }

    return true;
}

bool MediaStream::getIndex()
{
    if ( mMediaIndexPtr && mIndexComplete )
        return true;

    if ( !mMediaFilePtr->hasIndex() )
        return false;

    bool index = mMediaFilePtr->getIndex(   mMediaIndexPtr,
                                            mInitFragmentStartPts,
                                            mIndexEndPts,
                                            mInitFragmentStartByteOffset );

    mInitFragmentStartPts += mTimeStampOffset;

    if ( index )
    {
        mIndexComplete = mMediaFilePtr->indexComplete();

        if ( mMediaIndexPtr && mIndexComplete)
        {
            mInitFragmentIndex = 0;

            mCurrentFragmentStartPts = mInitFragmentStartPts;
            mCurrentFragmentStartByteOffset = mInitFragmentStartByteOffset;
            mCurrentFragmentIndex = mInitFragmentIndex;

            //outputMediaIndex();                 // debug purpose

            updateStreamingTime(mStreamingTime);
        }
    }
    else
    {
        mCurrentFragmentStartPts = mTimeStampOffset;
        mCurrentFragmentStartByteOffset = 0;
    }

    return index;
}

std::vector<unsigned char> const& MediaStream::getFileToken() const
{
    return mMediaFilePtr->getFileToken();
}

void MediaStream::reportFailure(AseErrorCode errorCode, uint32_t fragmentId)
{
    mMediaTrackPtr.lock()->reportFailure(errorCode, mStreamId, fragmentId);
}

// IStreamInfo
int32_t MediaStream::streamId() const
{
    return mStreamId;
}

MediaType MediaStream::mediaType() const
{
   return mMediaTrackPtr.lock()->mediaType();
}

bool MediaStream::empty() const
{
    if ( mMediaIndexPtr.get() )
        return mMediaIndexPtr->empty();
    else
        return true;
}

bool MediaStream::complete() const
{
    return mIndexComplete;
}

AseTimeStamp MediaStream::firstFragmentTime() const
{
    return mInitFragmentStartPts;
}

AseTimeStamp MediaStream::currentFragmentTime() const
{
    return mCurrentFragmentStartPts;
}


std::vector<StartplayPoint>  MediaStream::getStartplayCurve() const
{
    return mManifestStreamPtr->getStartplayCurve();
}

std::string const&
MediaStream::streamKey() const
{
    return mManifestStreamPtr->getStreamKey();
}

AseErrorCode MediaStream::getPreferredLocationSet(IStreamingLocationSetPtr& pStreamingLocationSet)
{
    return mLocationSelectorPtr->
                    getPreferredLocationSet(mManifestStreamPtr->getStreamKey(),
                                            ILocationSelector::HEURISTICS,
                                            pStreamingLocationSet );
}

shared_ptr<ILocationSetMonitor> MediaStream::getPreferredLocationSetMonitor()
{
    IStreamingLocationSetPtr pStreamingLocationSet;
    AseErrorCode retCode = getPreferredLocationSet(pStreamingLocationSet);
    if ( retCode == AS_NO_ERROR )
    {
        return pStreamingLocationSet->getLocationSetMonitor();
    }
    else
    {
        return shared_ptr<ILocationSetMonitor>();
    }
}

int MediaStream::getAverageBitrate() const
{
    return mManifestStreamPtr->getNominalBitrate();
}

bool MediaStream::isAvailable() const
{
    return mAvailable && mManifestStreamPtr->enabled();
}

IStreamInfo::const_iterator MediaStream::begin()
{
    const_iterator iter = const_iterator(shared_from_this(), 0);
    return iter;
}

IStreamInfo::const_iterator MediaStream::end()
{
    const_iterator iter = const_iterator( shared_from_this(), mMediaIndexPtr ? mMediaIndexPtr->size() : 0 );
    return iter;
}

IStreamInfo::const_iterator& MediaStream::increment( const_iterator& it )
{
    it.setIndex(it.getIndex() + 1);
    return it;
}

MediaFragmentInfo MediaStream::dereference( const_iterator& it )
{
    int const index = it.getIndex();

    if ( mMediaIndexPtr && (index >= 0) && (index < (int32_t)mMediaIndexPtr->size()) )
        return (*mMediaIndexPtr)[it.getIndex()];
    else
        return MediaFragmentInfo();
}

bool MediaStream::equal( const_iterator const& lhs, const_iterator const& rhs )
{
    if ( !mMediaIndexPtr )
        return true;

    int const lindex = lhs.getIndex();
    int const rindex = rhs.getIndex();

    if ( ((lindex < 0) || (lindex >= (int32_t)mMediaIndexPtr->size())) &&
        ((rindex < 0) || (rindex >= (int32_t)mMediaIndexPtr->size())) )
        return true;

    return (lindex == rindex);
}

AseErrorCode MediaStream::createMediaStreamRequest( MediaFragmentPtr&       pMediaFragment,
                                                    AseTimeStamp&           newStreamingTime,
                                                    int32_t                 predictedDlTimeForBackupStream,
                                                    uint32_t                alternativeFragmentSize,
                                                    MediaStreamRequestPtr&  pRequest)
{
    uint32_t cdnid = 0;
    IStreamingLocationSetPtr pStreamingLocationSet;
    AseErrorCode retCode = getPreferredLocationSet(pStreamingLocationSet);
    if ( retCode == AS_NO_ERROR )
    {
        cdnid = pStreamingLocationSet->getPrimaryCdnId();
    }

    retCode = createMediaFragment(pMediaFragment, cdnid, predictedDlTimeForBackupStream,alternativeFragmentSize);

    if ( retCode != AS_NO_ERROR )
        return retCode;

    ByteRange byteRange;
    retCode = pMediaFragment->getNextRequestRange(byteRange);

    MediaStreamRequest::construct( mManifestStreamPtr->getUrls(), byteRange,
                                      mMediaBufferPoolPtr, pMediaFragment,
                                      mManifestStreamPtr->getStreamKey(),
                                      mMediaTrackPtr.lock()->mediaType(),
                                      cdnid,
                                      pRequest );
    pMediaFragment->setMediaRequest(pRequest);

    newStreamingTime = mCurrentFragmentStartPts;

    return AS_NO_ERROR;
}

AseErrorCode MediaStream::createMediaFragment(MediaFragmentPtr& pMediaFragment,
                                              uint32_t cdnid, int32_t predictedDlTimeForBackupStream,
                                              uint32_t alternativeFragmentSize )
{
    int32_t nextFragmentIndex = mCurrentFragmentIndex;
    AseTimeInterval fragmentDuration = AseTimeStamp::ZERO;
    int32_t fragmentSize = 0;

    int32_t numOfFragments = 0;
    deque<ullong> startOfFragmentsByteOffsets;
    ullong fragmentStartBytesOffset = mCurrentFragmentStartByteOffset;
    int32_t currentGopSize = 0;
    while ( (mRemainingBlocks > 0) && (nextFragmentIndex < (int32_t)mMediaIndexPtr->size()) )
    {
        fragmentDuration += (*mMediaIndexPtr)[nextFragmentIndex].mDuration;
        currentGopSize = (*mMediaIndexPtr)[nextFragmentIndex].mSize;
        fragmentSize += currentGopSize;
        ++nextFragmentIndex;
        --mRemainingBlocks;
        ++numOfFragments;
        startOfFragmentsByteOffsets.push_back(fragmentStartBytesOffset);
        DEBUGLOG(MediaPresentationLog, "[%s] MediaStream::createMediaFragment: media type: %u, fragment boundary: %llu",
                                        streamKey().c_str(),
                                        mMediaTrackPtr.lock()->mediaType(), fragmentStartBytesOffset);
        fragmentStartBytesOffset += currentGopSize;
    }

    if ( !numOfFragments )
    {
#ifdef BUILD_DEBUG
        static bool enableDebugLogging = true;
        if (enableDebugLogging)
        {
            DEBUGLOG(MediaPresentationLog, "AS_MEDIA_END: stream id: %d, remaining blocks: %u, next fragment index: %d, fragment size: %zu",
                                mStreamId, mRemainingBlocks, nextFragmentIndex, mMediaIndexPtr->size());
            enableDebugLogging = false;
        }
#endif
        return AS_MEDIA_END;
    }

    MediaFragmentInfo fragmentInfo(fragmentDuration, fragmentSize);
    ByteRange byteRange( mCurrentFragmentStartByteOffset, mCurrentFragmentStartByteOffset + fragmentSize - 1 );

    DEBUGLOG(MediaPresentationLog, "[%s] MediaStream::createMediaFragment: index: %u, track id: %d, stream id: %d, byte range [%lld, %lld], fragment pts: %llu ms, number of fragments: %u",
                                streamKey().c_str(),
                                mCurrentFragmentIndex, mMediaTrackPtr.lock()->trackId(), mStreamId,
                                byteRange.start(), byteRange.end(),
                                mCurrentFragmentStartPts.getValueInMs(),
                                numOfFragments);
    mCurrentFragmentIndex = nextFragmentIndex;

    bool lastFragment = false;
    if ( nextFragmentIndex == (int32_t)mMediaIndexPtr->size() )
    {
        lastFragment = true;
        mMediaTrackPtr.lock()->complete();
    }

    MediaFragment::construct( mNextFragmentId,
                              mManifestId,
                              mMediaTrackPtr.lock()->trackId(),
                              mMediaTrackPtr.lock()->mediaType(),
                              cdnid,
                              mCurrentFragmentStartPts,
                              fragmentInfo,
                              static_pointer_cast<MediaStream>(shared_from_this()),
                              mMediaPresentationPtr.lock(),
                              mMediaSinkPtr,
                              mMediaBufferPoolPtr->getUnitSize(),
                              mMediaBufferPoolPtr,
                              lastFragment,
                              byteRange,
                              numOfFragments,
                              startOfFragmentsByteOffsets,
                              predictedDlTimeForBackupStream,
                              mMaxDataDryPeriod,
                              mForwardDataBeforeComplete,
                              mMinBlockCompleteTime,
                              mBlockReceptionBufferBudget,
                              mAbandonBlockControlTime,
                              mAbandonBlockControlPct,
                              alternativeFragmentSize,
                              pMediaFragment );

    ++mNextFragmentId;

    mCurrentFragmentStartPts += fragmentDuration;
    mCurrentFragmentStartByteOffset += fragmentSize;

    return AS_NO_ERROR;
}

void MediaStream::outputMediaIndex()
{
    DEBUGLOG(MediaPresentationLog,
            "MediaStream::outputMediaIndex track id: %d, stream id: %d, start pts: %llu",
            mMediaTrackPtr.lock()->trackId(),
            mStreamId, mCurrentFragmentStartPts.getValueInMs() );
    uint32_t index = 0;
    AseTimeStamp pts = mCurrentFragmentStartPts;
    ullong byteOffset = mCurrentFragmentStartByteOffset;
    DEBUGLOG(MediaPresentationLog, "My url is %s",
            mManifestStreamPtr->getUrls()[0]->getUrl().str().c_str() );
    for ( vector<MediaFragmentInfo>::const_iterator iter = mMediaIndexPtr->begin();
            iter != mMediaIndexPtr->end();
            ++iter )
    {
        DEBUGLOG(MediaPresentationLog,
                "index: %u, timestamp: %llu ms, byte offset: %llu bytes, size: %d bytes",
                index, pts.getValueInMs(), byteOffset, iter->mSize);
        ++index;
        pts += iter->mDuration;
        byteOffset += iter->mSize;
    }

    return;
}

void MediaStream::reset()
{
    mStreamingTime = mTimeStampOffset;
    mRemainingBlocks = 0;
    mCurrentFragmentStartPts = mInitFragmentStartPts;
    mCurrentFragmentStartByteOffset = mInitFragmentStartByteOffset;
    mCurrentFragmentIndex = mInitFragmentIndex;
    mFirstTimeAdjustStreamingTime = true;
    mMediaFilePtr->reset();
}

pair<int, int> MediaStream::getVideoSize() const
{
    return mManifestStreamPtr->getVideoSize();
}

bool MediaStream::isCompatible(MediaStreamPtr pStream)
{
    pair<int, int> myVideoSize = mManifestStreamPtr->getVideoSize();
    pair<int, int> videoSize = pStream->getVideoSize();
    if ( (myVideoSize.first == videoSize.first)
                    && (myVideoSize.second == videoSize.second) )
        return true;
    else
        return false;
}

void MediaStream::enable()
{
    DEBUGLOG(MediaPresentationLog, "[%s] MediaStream::enable: stream id: %u", streamKey().c_str(), mStreamId);
    mAvailable = true;
    return;
}

void MediaStream::disable()
{
    DEBUGLOG(MediaPresentationLog, "[%s] MediaStream::disable: stream id: %u", streamKey().c_str(), mStreamId);
    mAvailable = false;
    return;
}

ManifestStreamPtr MediaStream::getManifestStream() const
{
    return mManifestStreamPtr;
}

uint32_t MediaStream::getCurrentFragmentSize() const {
    return mIndexComplete ? (*mMediaIndexPtr)[ mCurrentFragmentIndex ].mSize : 0;
}

