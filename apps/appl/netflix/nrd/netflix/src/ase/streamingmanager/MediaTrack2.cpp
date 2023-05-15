/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "StreamersLog.h"
#include "MediaTrack2.h"
#include "TrackBufferInfo.h"
#include "NetworkMonitor.h"
#include "MediaPresentationRequest.h"

using namespace std;
using namespace netflix::ase;

AseErrorCode MediaTrack::construct( shared_ptr<MediaPresentation>            pMediaPresentation,
                          ManifestTrackPtr   pManifestTrack,
                          bool                          fastStart,
                          IMediaSinkPtr const&          pMediaSink,
                          int32_t                       manifestId,
                          int32_t                       trackId,
                          int32_t                       downloaderId,
                          IMediaBufferPoolPtr const&   pMediaBufferPool,
                          IStreamingReporterPtr const& pStreamingReporter,
                          ILocationSelectorPtr const&  pLocationSelector,
                          IAseConfigParameterPtr pConfigParameters,
                          IAseConfigParameterPtr pLocalConfigParameter,
                          vector<shared_ptr<IMediaFile> > const& mediaFiles,
                          MediaTrackPtr&                pMediaTrack)
{
    pMediaTrack.reset( new MediaTrack( pMediaPresentation, pManifestTrack,
                                       fastStart,
                                       pMediaSink, pMediaBufferPool,
                                       pStreamingReporter, pLocationSelector,
                                       pConfigParameters, pLocalConfigParameter,
                                       manifestId, trackId, downloaderId,
                                       mediaFiles) );

    return pMediaTrack->initialize();
}

MediaTrack::MediaTrack( shared_ptr<MediaPresentation> pMediaPresentation,
                        ManifestTrackPtr   pManifestTrack,
                        bool                           fastStart,
                        IMediaSinkPtr const&           pMediaSink,
                        IMediaBufferPoolPtr const&   pMediaBufferPool,
                        IStreamingReporterPtr const& pStreamingReporter,
                        ILocationSelectorPtr const&  pLocationSelector,
                        IAseConfigParameterPtr pConfigParameters,
                        IAseConfigParameterPtr pLocalConfigParameter,
                        int32_t                      manifestId,
                        int32_t                      trackId,
                        int32_t                      downloaderId,
                        vector<shared_ptr<IMediaFile> > const& mediaFiles)
            : mMediaPresentationPtr(pMediaPresentation),
              mManifestTrackPtr( pManifestTrack ),
              mMediaSinkPtr( pMediaSink ),
              mMediaBufferPoolPtr(pMediaBufferPool),
              mConfigParameterPtr(pConfigParameters),
              mLocalConfigParametePtr(pLocalConfigParameter),
              mFirstPendingFragment(shared_ptr<MediaFragment>()),
              mPreviousBlockAbandonTime(AseTimeVal::now()),
              mMediaBufferSize(0),
              mTotalMediaBufferSize(mMediaBufferPoolPtr->getUnitSize() * mMediaBufferPoolPtr->getUnitCount()),
              mStreamingReporterPtr(pStreamingReporter),
              mLocationSelectorPtr(pLocationSelector),
              mPreviousMediaStream( shared_ptr<MediaStream>() ),
              mIndexForCurrentStream(-1),
              mIndexForPreviousStream(-1),
              mIndexForStreamDownloadingHeaders(-1),
              mCachedThroughput(0),
              mLongtermBw(0),
              mSelStreamIndex(-1),
              mIndexForPreviousHeaderDownloading(-1),
              mBrokenStreamId(0),
              mAllHeadersDownloaded(false),
              mFirstIndexDownloaded(false),
              mHeaderDelivered(false),
              mSwitchingTrack(false),
              mCurrentPlaybackTime(AseTimeStamp::INFINITE),
              mCurrentStreamingTime(AseTimeStamp::INFINITE),
              mManifestId(manifestId),
              mTrackId(trackId),
              mDownloaderTrackId(downloaderId),
              mDuration(0),
              mPendingBufferLen(AseTimeStamp::ZERO),
              mCompleted(false),
              mDlNonEmptyBlocks(0),
              mEndOfTrack(false),
              mRebuffer(0),
              mProbingStream(false),
              mLicenseAcquired(false),
              mFastStart(fastStart),
              mPaused(false)
              ,mMediaFiles(mediaFiles)
              ,mForwardingCheckInterval(1000)
              ,mNextTimeToCheckForwarding( AseTimeVal::now() + AseTimeVal::fromMS(mForwardingCheckInterval) )
              ,mNumOfPlayCompleted(0)
              ,mPendingMediaFile(false)
{
    mAvailableMediaBufferSize = mTotalMediaBufferSize;

    // construct media streams
    mMediaStreams.clear();
    mOldMediaStreams.clear();

    mTmpTrack = MediaTrackPtr();
    mNextManifestTrackInfo.reset();

    getConfiguration();
}

void MediaTrack::configure( IAseConfigParameterPtr pConfigParameters )
{
    mConfigParameterPtr = pConfigParameters;

    getConfiguration();

    std::vector<MediaStreamPtr>::iterator it = mMediaStreams.begin();
    for( ; it != mMediaStreams.end(); ++it )
    {
        (*it)->configure( pConfigParameters );
    }
}

void MediaTrack::getConfiguration()
{
    mMaxPendingBufferPercentage =
        mConfigParameterPtr->getParameterAs<uint32_t>( "maxPendingBufferPercent", 10 );
    mMaxPendingBufferLen =
        mConfigParameterPtr->getParameterAs<uint32_t>( "maxPendingBufferLength", 6000 );
    mMinPendingBufferLen = mConfigParameterPtr->getParameterAs<uint32_t>("minPendingBufferLen", 3000);
    mMinBufferLenForHeaderDownloading =
        mConfigParameterPtr->getParameterAs<uint32_t>("minBufferLenForHeaderDownloading", 6000);
    mRequiredBufferingLen =
        mConfigParameterPtr->getParameterAs<uint32_t>("minPrebufSize",
                                                      DEFAULT_MIN_PREBUF_SIZE);
    // TODO: disable the multi-block downloading feature for now
    mMinRequestSize =
        mConfigParameterPtr->getParameterAs<uint32_t>("minMediaRequestSize",
                                                    128 * 1024);
    mGuardDurationForFirstManifest =
            mConfigParameterPtr->getParameterAs<int>(
                "guardDurationForFirstManifest", 30 * 1000);

    mMaxNonDrmContentPlayLen = mConfigParameterPtr
            ->getParameterAs<uint32_t>( "maxNonDrmContentPlayLen", 300 * 1000 );

    mAbandonBlockGuardInterval = mConfigParameterPtr
            ->getParameterAs<uint32_t>("cancelBlockGuardInterval", 15 * 1000); // 15 seconds

    mLowWatermarkLevel = mConfigParameterPtr
            ->getParameterAs<uint32_t>("lowWatermarkLevel", DEFAULT_LOW_WATERMARK_LEVEL);

    mAbandonThreshold = mConfigParameterPtr
            ->getParameterAs<uint32_t>("abandonThreshold", DEFAULT_ABANDON_THRESHOLD);

    mEnableAbandonBlock = mConfigParameterPtr
            ->getParameterAs<bool>("enableAbortBlock", false);


    mMinAcceptableVideoBitrate = mConfigParameterPtr
            ->getParameterAs<int32_t>("minAcceptableVideoBitrate", -1);

    DEBUGLOG( MediaPresentationLog, "MediaTrack %d maxPendingBufferPercent %d maxPendingBufferLength %d minPendingBufferLen %d",
        mTrackId,
        mMaxPendingBufferPercentage,
        mMaxPendingBufferLen,
        mMinPendingBufferLen );
}

AseErrorCode MediaTrack::initialize()
{
    vector<ManifestStreamPtr> const& manifestStreams = mManifestTrackPtr->getStreams();
    MediaStreamPtr pMediaStream;
    AseErrorCode retCode;
    bool failed = false;
    int32_t streamId = 0;
    mLowestBitrate = 5000000;
    // The manifestStreams are already sorted based on bitrate
    assert(manifestStreams.size() > 0);
    if ( manifestStreams.empty() )
        return AS_NOT_ALlOWED;

    for ( vector<ManifestStreamPtr>::const_iterator iter = manifestStreams.begin();
            iter != manifestStreams.end();
            ++iter )
    {
        shared_ptr<IMediaFile> pPreExistingMediaFile = IMediaFilePtr();
        if ( (*iter)->enabled() )
        {
            pPreExistingMediaFile = findPreExistingMediaFile(mMediaFiles, (*iter)->getStreamKey(), streamId);
        }
        retCode =
            MediaStream::construct( shared_from_this(), mMediaPresentationPtr.lock(), *iter, mMediaSinkPtr,
                                    mManifestId, streamId,
                                    mMediaBufferPoolPtr, mLocationSelectorPtr,
                                    mConfigParameterPtr,
                                    mManifestTrackPtr->getTimestampOffset(),
                                    pPreExistingMediaFile,
                                    pMediaStream );
        if ( retCode != AS_NO_ERROR )
        {
            failed = true;
            break;
        }
        mMediaStreams.push_back(pMediaStream);
        if ( (*iter)->getNominalBitrate() < (int32_t)mLowestBitrate )
            mLowestBitrate = (*iter)->getNominalBitrate();

        DEBUGLOG(MediaPresentationLog, "media id: %d, norminal bitrate: %d kbps, enabled: %u",
            streamId, pMediaStream->getAverageBitrate(),
            (*iter)->enabled());
        ++streamId;
    }

    mLowestBitrate = (mMinAcceptableVideoBitrate > (int32_t)mLowestBitrate) ?
                            mMinAcceptableVideoBitrate : mLowestBitrate;

    if ( manifestStreams.empty() )
        failed = true;

    if ( !failed )
    {
        mMediaType = mManifestTrackPtr->getMediaType();
        mTrackBufferInfoPtr.reset( new TrackBufferInfo( shared_from_this(), mTrackId, mMediaType, mMediaFragments ) );
        DEBUGLOG(MediaPresentationLog,
                "track id: %u, media type: %u, lowest bitrate: %u kbps",
                mTrackId, mMediaType, mLowestBitrate);
        return AS_NO_ERROR;
    }
    else
        return AS_MEDIA_NOT_SUPPORTED;
}

ManifestTrackPtr MediaTrack::getManifestTrack() const
{
    return mManifestTrackPtr;
}


// ITrackInfo
int32_t MediaTrack::trackId() const
{
    return mTrackId;
}

MediaType MediaTrack::mediaType() const
{
    return mMediaType;
}



ITrackBufferInfoPtr MediaTrack::trackBufferInfo() const
{
    return mTrackBufferInfoPtr;
}


AseErrorCode MediaTrack::getNextRequest(MediaPresentationRequestPtr& pRequest)
{
    bool eligibleForDeliverData = !mFastStart || (mNumOfPlayCompleted == 0);

    AseErrorCode retCode = AS_PENDING;

    if ( !mHeaderDelivered && eligibleForDeliverData )
        deliverHeader();

    if ( (mMediaType == MEDIA_AUDIO) && mSwitchingTrack )
    {
        if ( (numNonEmptyFragments() >= MIN_NONEMPTY_AUDIO_FRAGMENTS + 1) ||
                endOfTrack() )
        {
            mStreamingReporterPtr->audioTrackSwitchComplete();
            mSwitchingTrack = false;
        }
    }

    AseTimeVal now = AseTimeVal::now();
    if ( now > mNextTimeToCheckForwarding )
    {
        mNextTimeToCheckForwarding = now + AseTimeVal::fromMS(mForwardingCheckInterval);
        if ( mFirstPendingFragment.get() )
            mFirstPendingFragment->startForwarding();
    }

    MediaPresentationPtr pMediaPresentation = mMediaPresentationPtr.lock();
    if ( pMediaPresentation.get() == NULL )
    {
        return AS_NOT_ALlOWED;
    }


    if ( mLicenseAcquired && mFastStart && !hasPendingRequest() && !mSwitchingTrack && !mPaused)
    {
        // only pause the track for migration after all pending requests have been sent out
        mPaused = true;
    }

    do
    {
        if ( endOfTrack() && !mFastStart )
        {
            retCode = AS_NOT_ALlOWED;
            break;
        }

        if ( (mCurrentStreamingTime == AseTimeStamp::INFINITE) && eligibleForDeliverData )
        {
            // Has not received seek time yet. Only request for header
            retCode = getNextRequestWithoutSeekTime(pRequest);
            break;
        }

        if ( mFastStart && (mMediaType == MEDIA_VIDEO) )
        {
            // check to see whether to schedule the request for the drm header
            retCode = getDrmHeaderRequest(pRequest);
            if ( retCode == AS_NO_ERROR )
                break;

            if ( checkForMigration() )
            {
                // migrate to drm manifest
                AseErrorCode migrateResult = migrate();
                if ( migrateResult != AS_NO_ERROR )
                {
                    pMediaPresentation->reportFailure(migrateResult, "manifest switch failure");
                }

                migrateResult = pMediaPresentation->migrate();
                if ( migrateResult != AS_NO_ERROR )
                {
                    pMediaPresentation->reportFailure(migrateResult, "manifest switch failure");
                }

                break;
            }
         }

        if ( !eligibleForDeliverData )
        {
            // wait for migration during seek when initial play is fast play
            break;
        }

        // check pending request from the current stream first
        bool pendingMediaRequest;
        retCode = requestFromCurrentStream(pRequest, pendingMediaRequest);
        if ((retCode == AS_NO_ERROR) || pendingMediaRequest)
        {
            // get a request or have pending media request
            break;
        }

        if ( mPaused || waitForDrmManifest())
        {
            // just finish of the pending requests during pause.
            // Waiting for the migration
            break;
        }

        // check whether to abandon pending request if bandwidth has been dropped significantly
#ifdef TEST_CANCEL_BLOCK
        streamingHealthCheck();
#endif

        uint32_t currentBufferLen, pendingBufferLen;
        obtainBufferingLen(currentBufferLen, pendingBufferLen);

        if ( tooMuchPendingBuffer() )
        {
            // We only try to make a header request when the player has scheduled too much in the future
            if ( currentBufferLen > mMinBufferLenForHeaderDownloading )
                retCode = getNextHeaderRequest(pRequest);
            else
                retCode = AS_PENDING;
            break;
        }

        if ( pMediaPresentation->isBufferingState() )
        {
            retCode = formMediaRequest(pRequest);
        }
        else
        {
            if ( currentBufferLen > mMinBufferLenForHeaderDownloading )
            {
                // try to make a request for header
                retCode = getNextHeaderRequest(pRequest);
            }
            if ( retCode != AS_NO_ERROR )
            {
                // unable to make a request for header
                retCode = formMediaRequest(pRequest);
            }

        }

    } while (false);

    return retCode;
}




bool MediaTrack::probingStream() const
{
    return mProbingStream;
}

AseErrorCode MediaTrack::setStreamingTime( AseTimeStamp const& streamingTime, AseTimeStamp& newStreamingTime )
{
    if ( (mMediaType == MEDIA_VIDEO) && mFastStart && (mNumOfPlayCompleted > 0) )
        return AS_PENDING;

    newStreamingTime = streamingTime;
    for ( vector<MediaStreamPtr>::iterator iter = mMediaStreams.begin();
            iter != mMediaStreams.end();
            ++ iter )
    {
        if ( (*iter)->indexComplete() )
        {
            newStreamingTime = (*iter)->updateStreamingTime(streamingTime);
            break;
        }
    }

    newStreamingTime = updateStreamingTimeInStreams( newStreamingTime );
    return AS_NO_ERROR;
}

void MediaTrack::updatePlaybackTime( AseTimeStamp const& playbackTime )
{
    mCurrentPlaybackTime = playbackTime;
    // mPreviousMediaStream is the media stream before the fragment currently being played
    mPreviousMediaStream = mTrackBufferInfoPtr->updatePlaybackTime( playbackTime );
}

void MediaTrack::obtainBufferingLen(uint32_t& completedBuffer, uint32_t& pendingBuffer) const
{
    if ( (mCurrentStreamingTime == AseTimeStamp::INFINITE) ||
            (mCurrentPlaybackTime == AseTimeStamp::INFINITE) )
    {
        completedBuffer = 0;
        pendingBuffer = 0;
        return;
    }

    if ( mCurrentStreamingTime >= mCurrentPlaybackTime )
    {
        AseTimeStamp bufferingLen = mCurrentStreamingTime - mCurrentPlaybackTime;
        if ( bufferingLen > mPendingBufferLen )
            completedBuffer =  (bufferingLen - mPendingBufferLen).getValueInMs();
        else
            completedBuffer = 0;
        pendingBuffer = mPendingBufferLen.getValueInMs();
    }
    else
    {
        completedBuffer = 0;
        pendingBuffer = 0;
    }

    if (mEndOfTrack && (completedBuffer == 0))
    {
        AseTimeStamp startTime = mTrackBufferInfoPtr->getFirstFragmentStartTime();
        completedBuffer = (mCurrentStreamingTime - startTime).getValueInMs();
        pendingBuffer = 0;
    }

    return;
}

void MediaTrack::
completeForwarding( AseTimeStamp const& fragmentStartPts,
        AseTimeStamp const& duration, uint32_t numOfBlocks, bool lastFragment)
{
    DEBUGLOG(MediaPresentationLog,
                    "completeForwarding: pending buffer len: %llu ms"
                    ", duration: %llu ms",
                    mPendingBufferLen.getValueInMs(),
                    duration.getValueInMs() );

    mPendingBufferLen = mPendingBufferLen > duration ?
                                mPendingBufferLen - duration :
                                AseTimeStamp::ZERO;
    DEBUGLOG(MediaPresentationLog,
                    "completeForwarding: updated pending buffer len: %llu ms",
                    mPendingBufferLen.getValueInMs());

    mStreamingReporterPtr->updateStreamingPts(mManifestId, mTrackId, fragmentStartPts + duration);

    if ( mMediaType == MEDIA_VIDEO )
        mMediaPresentationPtr.lock()->checkBufferingStatus();

    if ( lastFragment || (mEndOfTrack && (mPendingBufferLen == AseTimeStamp::ZERO)) )
    {
        DEBUGLOG( MediaPresentationLog, "track: %d end of track", mTrackId );
        // only notifies endOfTrack for the last period
        if ( mMediaPresentationPtr.lock()->finalManifest() )
        {
            mMediaSinkPtr->endOfTrack(mManifestId, mTrackId);
            mMediaPresentationPtr.lock()->videoEndOfTrack();
            mEndOfTrack = true;
        }
    }

    if ( numOfBlocks > 0 )
        ++mDlNonEmptyBlocks;

    if ( mFirstPendingFragment.get() )
        mFirstPendingFragment = mFirstPendingFragment->getNextFragment();
}

void MediaTrack::videoEndOfTrack()
{
    if ( mMediaType == MEDIA_AUDIO )
    {
        mEndOfTrack = true;
        if (mPendingBufferLen == AseTimeStamp::ZERO)
        {
            mMediaSinkPtr->endOfTrack(mManifestId, mTrackId);
        }

    }

}

void MediaTrack::reset()
{
    mMediaFragments.clear();
    mMediaBufferSize = 0;
    mAvailableMediaBufferSize = mTotalMediaBufferSize;

    mTrackBufferInfoPtr.reset( new TrackBufferInfo( shared_from_this(), mTrackId, mMediaType, mMediaFragments ) );

    mPreviousMediaStream = shared_ptr<MediaStream>();
    mIndexForCurrentStream = -1;
    mIndexForPreviousStream = -1;
    mIndexForStreamDownloadingHeaders = -1;
    mSelStreamIndex = -1;
    mCurrentPlaybackTime = AseTimeStamp::INFINITE;
    mCurrentStreamingTime = AseTimeStamp::INFINITE;
    mPendingBufferLen = AseTimeStamp::ZERO;

    mPendingMediaFile = false;

    mOldMediaStreams.clear();

    mCompleted = false;
    mDlNonEmptyBlocks = 0;
    mEndOfTrack = false;
    mRebuffer = 0;
    mProbingStream = false;

    if ( mFastStart && (mTmpTrack.get() != NULL) )
        mTmpTrack->reset();

    for ( vector<MediaStreamPtr>::iterator iter = mMediaStreams.begin();
            iter != mMediaStreams.end();
            ++ iter )
        (*iter)->reset();

    mSwitchingTrack = false;
    ++mNumOfPlayCompleted;
}

AseTimeStamp MediaTrack::updateStreamingTimeInStreams( AseTimeStamp const& streamingTime )
{
    //printf("track id: %d, previous streaming time: %llu\n",
      //  mTrackId, mCurrentStreamingTime.getValueInMs() );

    for ( vector<MediaStreamPtr>::iterator iter = mMediaStreams.begin();
            iter != mMediaStreams.end();
            ++iter )
        mCurrentStreamingTime = (*iter)->updateStreamingTime( streamingTime );

    //printf("track id: %d, current streaming time: %llu\n",
      //  mTrackId, mCurrentStreamingTime.getValueInMs() );
    return mCurrentStreamingTime;
}

void MediaTrack::reportFailure( AseErrorCode errorCode, uint32_t brokenStreamId, uint32_t fragmentId )
{
    NRDP_UNUSED(fragmentId);
    if ( errorCode != AS_ABANDONED )
    {
        mBrokenStreamId = brokenStreamId;
        mMediaPresentationPtr.lock()->reportFailure(errorCode, string());
    }
    else
    {
        AseTimeStamp streamingTime = AseTimeStamp::INFINITE;
        while ( !mMediaFragments.empty() )
        {
            MediaFragmentPtr const pMediaFragment = mMediaFragments.back();

            if ( !pMediaFragment->isAbandoned() )
                break;

            popMediaFragment();

            streamingTime = pMediaFragment->getStartPts();
        }

        if ( streamingTime != AseTimeStamp::INFINITE )
            updateStreamingTimeInStreams(streamingTime);
    }
}

MediaFragmentPtr MediaTrack::findFragmentInQueue(uint32_t streamId, uint32_t fragmentId)
{
    MediaFragmentPtr pMediaFragment = mFirstPendingFragment;
    MediaFragmentPtr pMatchedFragment = MediaFragmentPtr();
    while (pMediaFragment.get())
    {
        if ( (pMediaFragment->getMediaStream()->streamId() == (int32_t)streamId) &&
                (pMediaFragment->getFragmentId() == fragmentId) )
        {
            pMatchedFragment = pMediaFragment;
            break;
        }

        pMediaFragment = pMediaFragment->getNextFragment();
    }

    return pMatchedFragment;
}


void MediaTrack::provideMediaFragment( MediaFragmentPtr const& pMediaFragment )
{
    if ( mMediaFragments.empty() )
    {
        pMediaFragment->startForwarding( );
        mFirstPendingFragment = pMediaFragment;
    }
    else
    {
        MediaFragmentPtr pPrevMediaFragment = mMediaFragments.back();
        if ( pPrevMediaFragment->downloadedAndForwarded() )
        {
            pMediaFragment->startForwarding();
            mFirstPendingFragment = pMediaFragment;
        }
        pPrevMediaFragment->setNextFragment( pMediaFragment );
        pMediaFragment->setPrevFragment(pPrevMediaFragment);
    }

    assert( pMediaFragment );
    mMediaFragments.push_back(pMediaFragment);
    mMediaBufferSize += pMediaFragment->getFragmentInfo().mSize;
    mAvailableMediaBufferSize = mTotalMediaBufferSize > mMediaBufferSize ?
                                    mTotalMediaBufferSize - mMediaBufferSize :
                                    0;
    mPendingBufferLen += pMediaFragment->getDuration();

    DEBUGLOG(MediaPresentationLog, "provideMediaFragment: pending buffer len: %llu ms",
                        mPendingBufferLen.getValueInMs());

    return;
}

void MediaTrack::popMediaFragment()
{
    if ( !mMediaFragments.empty() )
    {
        MediaFragmentPtr pMediaFragment = mMediaFragments.back();
        mMediaBufferSize = mMediaBufferSize > (uint32_t)pMediaFragment->getFragmentInfo().mSize ?
                                mMediaBufferSize - pMediaFragment->getFragmentInfo().mSize :
                                0;
        mAvailableMediaBufferSize = mTotalMediaBufferSize > mMediaBufferSize ?
                                        mTotalMediaBufferSize - mMediaBufferSize :
                                        0;
        if ( mPendingBufferLen > pMediaFragment->getDuration() )
            mPendingBufferLen -= pMediaFragment->getDuration();
        else
            mPendingBufferLen = AseTimeStamp::ZERO;
        mMediaFragments.pop_back();
    }
}

void MediaTrack::decrBufferSize(uint32_t size)
{
    mMediaBufferSize -= size;
    mAvailableMediaBufferSize = mTotalMediaBufferSize > mMediaBufferSize ?
                                    mTotalMediaBufferSize - mMediaBufferSize :
                                    0;
    return;
}

// ITrackInfo
ITrackInfo::const_iterator MediaTrack::begin()
{
    int32_t index = 0;
    if ( mMediaStreams.empty() )
        index = -1;

    const_iterator iter = const_iterator(shared_from_this(), index);
    return iter;
}

ITrackInfo::const_iterator MediaTrack::end()
{
    int32_t index = mMediaStreams.size();
    const_iterator iter = const_iterator(shared_from_this(), index);
    return iter;
}

ITrackInfo::const_iterator MediaTrack::current()
{
    const_iterator iter = const_iterator(shared_from_this(), mSelStreamIndex);
    return iter;
}

bool MediaTrack::isCompleted()
{
    return mCompleted;
}

void MediaTrack::complete()
{
    mCompleted = true;
    mCurrentStreamingTime = AseTimeStamp::INFINITE;
}

ITrackInfo::const_iterator& MediaTrack::increment( const_iterator& it )
{
    it.setIndex(getNextAvailableIndex(it.getIndex()));
    return it;
}

ITrackInfo::const_iterator& MediaTrack::decrement( const_iterator& it )
{
    it.setIndex(getPrevAvailableIndex(it.getIndex()));
    return it;
}

IStreamInfoPtr MediaTrack::dereference( const_iterator& it )
{
    if ( isValid(it.getIndex()) )
        return mMediaStreams[it.getIndex()];
    else
        return IStreamInfoPtr();
}
bool MediaTrack::equal( const_iterator const& lhs, const_iterator const& rhs )
{
    if ( !isValid(lhs.getIndex()) && !isValid(rhs.getIndex()) )
        return true;

    return (lhs.getIndex() == rhs.getIndex());
}

bool MediaTrack::hasNext( const_iterator const& it )
{
    if ( !isValid(it.getIndex()) || it.getIndex() >= (int32_t)mMediaStreams.size() - 1 )
        return false;

    if ( isValid(getNextAvailableIndex(it.getIndex())) )
        return true;
    else
        return false;

}

bool MediaTrack::hasPrevious( const_iterator const& it )
{
    if ( !isValid(it.getIndex()) || it.getIndex() <= 0 )
        return false;

    if ( isValid(getPrevAvailableIndex(it.getIndex())) )
        return true;
    else
        return false;

}

bool MediaTrack::isValid(int index) const
{
    if ( (index >= 0) && (index < (int32_t)mMediaStreams.size()) )
        return true;
    else
        return false;
}

int32_t MediaTrack::getNextAvailableIndex( int32_t index )
{
    int32_t nextIndex = index + 1;
    while ( nextIndex < (int32_t)mMediaStreams.size() )
    {
        MediaStreamPtr pNextMediaStream = mMediaStreams[nextIndex];
        if ( pNextMediaStream->isAvailable() )
        {
            break;
        }
        ++nextIndex;
    }
    return nextIndex;
}

int32_t MediaTrack::getPrevAvailableIndex( int32_t index )
{
    int32_t prevIndex = index - 1;
    while ( prevIndex >= 0 )
    {
        MediaStreamPtr pPrevMediaStream = mMediaStreams[prevIndex];
        if ( pPrevMediaStream->isAvailable() )
        {
            break;
        }
        --prevIndex;
    }
    return prevIndex;
}


bool MediaTrack::deliverHeader()
{
    if ( mMediaType == MEDIA_AUDIO )
    {
        return false;
    }

    if ( mHeaderDelivered )
        return true;

    uint32_t gopSize;
    for ( vector<MediaStreamPtr>::iterator iter = mMediaStreams.begin();
            iter != mMediaStreams.end();
            ++iter )
    {
        if ( (*iter)->deliverHeader(mDuration, gopSize) )
        {
            mStreamingReporterPtr->openCompleted(mDuration, gopSize);
            mStreamingReporterPtr->reportFileToken( (*iter)->getFileToken() );
            mHeaderDelivered = true;
            break;
        }
    }
    return mHeaderDelivered;
}


void MediaTrack::updateContentDuration(uint32_t duration)
{
    if ( mDuration < duration )
    {
        mDuration = duration;
        mMediaPresentationPtr.lock()->updateContentDuration(duration);
    }
}

int32_t MediaTrack::brokenStreamId() const
{
    return mBrokenStreamId;
}

bool MediaTrack::hasPendingTasks() const
{
    uint32_t numOfFragments = mMediaFragments.size();
    if ( numOfFragments > 0 )
    {
        MediaFragmentPtr pLastMediaFragment = mMediaFragments[numOfFragments - 1];
        return !(pLastMediaFragment->downloadedAndForwarded());
    }
    else
    {
        return false;
    }
}

bool MediaTrack::hasHeaderToDownload( uint32_t throughput, int32_t& streamId )
{
    if ( mAllHeadersDownloaded )
        return false;

    bool downloadHeader = false;
    streamId = -1;
    MediaStreamPtr pMediaStream;

    // search for the stream having the header at the very beginning
    if (mIndexForPreviousHeaderDownloading == -1)
    {
        for (uint32_t i = 0; i < mMediaStreams.size(); ++i)
        {
            pMediaStream = mMediaStreams[i];
            if (!pMediaStream->isAvailable())
                continue;

            if (pMediaStream->indexComplete())
            {
                DEBUGLOG(MediaPresentationLog, "has downloaded header for stream %u", i);
                mIndexForPreviousHeaderDownloading = i;
                break;
            }
        }
    }

    if ( mIndexForPreviousHeaderDownloading != -1 )
        downloadHeader = hasHeaderToDownload( throughput, streamId, mIndexForPreviousHeaderDownloading );

    if (downloadHeader)
        return true;
    else
    {
        downloadHeader = hasHeaderToDownload( throughput, streamId, 0 );
    }

    return downloadHeader;
}

bool MediaTrack::hasHeaderToDownload( uint32_t throughput, int32_t& streamId, uint32_t startIndex )
{
    bool downloadHeader = false;
    MediaStreamPtr pMediaStream;
    streamId = mMediaStreams.size();
    uint32_t numOfStreamsWithIndex = 0;
    for ( uint32_t i = 0; i < mMediaStreams.size(); ++i )
    {
        pMediaStream = mMediaStreams[i];

        if (!pMediaStream->isAvailable())
            continue;

        if ( pMediaStream->indexComplete() )
        {
            ++numOfStreamsWithIndex;
            if ( !mHeaderByteRangeHint.valid() )
            {
                mHeaderByteRangeHint = pMediaStream->getHeaderRange();
            }
        }

         if ( i >= startIndex &&
                !(pMediaStream->indexComplete()) &&
                ((pMediaStream->getAverageBitrate() < (int32_t)throughput) ||
                                   (pMediaStream->getAverageBitrate() < 500)) ) // FIXME: hard-code 500
        {
            if ( mHeaderByteRangeHint.valid() )
            {
                pMediaStream->provideHeaderRangeHint( mHeaderByteRangeHint );
            }
            downloadHeader = true;
            streamId = i;
            break;
        }
    }

    mIndexForPreviousHeaderDownloading = streamId;
    if ( numOfStreamsWithIndex == mMediaStreams.size() )
        mAllHeadersDownloaded = true;

    return downloadHeader;
}

int MediaTrack::getPlaybackBitrate()
{
    return mTrackBufferInfoPtr->getPlaybackBitrate();
}

int MediaTrack::getStreamingBitrate()
{
    if ( (mSelStreamIndex >= 0) && (mSelStreamIndex < (int32_t)mMediaStreams.size()) )
    {
        MediaStreamPtr currentStream = mMediaStreams[mSelStreamIndex];
        return currentStream->getAverageBitrate();
    }
    else
    {
        return 0;
    }
}

uint32_t MediaTrack::numNonEmptyFragments() const
{
    DEBUGLOG(MediaPresentationLog, "MediaTrack::numNonEmptyFragments %u", mDlNonEmptyBlocks);
    return mDlNonEmptyBlocks;
}

void MediaTrack::setNumNonEmptyFragments(uint32_t numOfBlocks)
{
    mDlNonEmptyBlocks = numOfBlocks;
}

void MediaTrack::reportNewStreamDuration(const string& streamKey, uint32_t duration)
{
    mMediaPresentationPtr.lock()->reportNewStreamDuration(streamKey, duration);
}

void MediaTrack::setVideoBitrateRanges(vector< shared_ptr<BitrateRange> > const& videoBitrateRanges)
{
    if ( videoBitrateRanges.empty() || (mMediaType != MEDIA_VIDEO) )
        return;

    DEBUGLOG(MediaPresentationLog, "setVideoBitrateRanges: %zu", videoBitrateRanges.size());

    bool existEnabledStream = false;
    mVideoBitrateRanges = videoBitrateRanges;

    int32_t lowestBitrate = 5000 * 1000;

    for ( vector<MediaStreamPtr>::iterator iter = mMediaStreams.begin();
            iter != mMediaStreams.end();
            ++iter )
    {
        int bitrate = (*iter)->getAverageBitrate();
        ContentProfile streamContentProfile = (*iter)->getManifestStream()->getContentProfile();
        bool inRange = false;

        for ( vector<shared_ptr<BitrateRange> >::const_iterator bitrateIter = videoBitrateRanges.begin();
                bitrateIter != videoBitrateRanges.end();
                ++bitrateIter )
        {
            shared_ptr<BitrateRange> bitrateRange = *bitrateIter;

            int32_t minVideoBitrate = bitrateRange->mMinVideoBitrate;
            int32_t maxVideoBitrate = bitrateRange->mMaxVideoBitrate;
            ContentProfile contentProfile = bitrateRange->mContentProfile;
            if ( (streamContentProfile == contentProfile) &&
                    (bitrate >= minVideoBitrate) && (bitrate <= maxVideoBitrate) )
            {
                inRange = true;
                break;
            }
        }
        if ( inRange )
        {
            (*iter)->enable();
            if ( (*iter)->getAverageBitrate() < lowestBitrate )
                lowestBitrate = (*iter)->getAverageBitrate();
            if ( (*iter)->isAvailable() )
                existEnabledStream = true;
        }
        else
            (*iter)->disable();
    }

    if ( !existEnabledStream )
    {
        if ( !videoBitrateRanges.empty() )
        {
            ERRORLOG(MediaPresentationLog, "provided invalid bitrate ranges. enable all streams");
        }

        for ( vector<MediaStreamPtr>::iterator iter = mMediaStreams.begin();
                iter != mMediaStreams.end();
                ++iter )
        {
            (*iter)->enable();
        }
    }
    else
    {
        mLowestBitrate = (uint32_t)lowestBitrate;
    }

}

void MediaTrack::selectStreams(vector<uint32_t> const& inStreamIndexs)
{
    if ( inStreamIndexs.empty() )
        return;

    DEBUGLOG(MediaPresentationLog, "selectStreams: track id: %u, indexes: ",
                        mTrackId );
    for (vector<uint32_t>::const_iterator iter = inStreamIndexs.begin();
            iter != inStreamIndexs.end();
            ++iter)
    {
        DEBUGLOG(MediaPresentationLog, "%u", *iter);
    }

    vector<uint32_t> streamIndexs = inStreamIndexs;
    sort(streamIndexs.begin(), streamIndexs.end());
    vector<MediaStreamPtr>::iterator streamIter = mMediaStreams.begin();
    vector<uint32_t>::iterator iter = streamIndexs.begin();
    while (streamIter != mMediaStreams.end())
    {
        int32_t streamId = (*streamIter)->streamId();
        int32_t selStreamId = (int32_t)(*iter);
        if ( streamId == selStreamId )
        {
            (*streamIter)->enable();
            ++streamIter;
            if ( iter != streamIndexs.end() )
                ++iter;
        }
        else if ( streamId < selStreamId )
        {
            (*streamIter)->disable();
            ++streamIter;
        }
        else if ( iter != streamIndexs.end() )
        {
            ++iter;
        }
    }
}

void MediaTrack::setDownloadTrackId(int32_t trackId)
{
    mDownloaderTrackId = trackId;
    if (mTmpTrack.get() != NULL)
        mTmpTrack->setDownloadTrackId(trackId);
}

AseErrorCode MediaTrack::getNextRequestWithoutSeekTime(MediaPresentationRequestPtr& pRequest)
{
    AseErrorCode retCode = AS_PENDING;
    if (!mFirstIndexDownloaded)
    {
        AseTimeStamp newStreamingTime;
        if ( isValid(mIndexForStreamDownloadingHeaders) )
        {
            // is currently downloading header/index for a stream
            MediaStreamPtr pCurrentStream = mMediaStreams[mIndexForStreamDownloadingHeaders];
            uint32_t completedBuffer, pendingBuffer;
            obtainBufferingLen(completedBuffer, pendingBuffer);
            if ( pCurrentStream->hasPendingRequest(completedBuffer) )
            {
                retCode = pCurrentStream->getNextRequest(newStreamingTime, pRequest);
                if ( retCode == AS_NO_ERROR )
                {
                    pRequest->setDownloadTrackId( mDownloaderTrackId );
                }
            }
            else
            {
                // has completed the header/index request
                // waiting for the seek time
                retCode = AS_PENDING;
                mFirstIndexDownloaded = true;
                mIndexForStreamDownloadingHeaders = -1;
            }
        }
        else
        {
            int32_t streamId;
            uint32_t predictedRebuffer;
            IStreamSelector::StreamFeasibility streamFeasibility;
            int32_t predictedDlTimeForBackupStream;

            mMediaPresentationPtr.lock()->selectStream(mTrackId, streamId, mCachedThroughput, mLongtermBw,
                                                       predictedRebuffer, streamFeasibility,
                                                       predictedDlTimeForBackupStream);

            assert(streamId >= 0);
            findStreamUsingId(streamId, mIndexForStreamDownloadingHeaders);

            MediaStreamPtr pCurrentStream = findAlternativeValidStream(mIndexForStreamDownloadingHeaders);

            retCode = pCurrentStream->getNextRequest(   0,
                                                        newStreamingTime,
                                                        predictedDlTimeForBackupStream,
                                                        mMediaStreams[ 0 ]->getCurrentFragmentSize(),
                                                        pRequest);
            if ( retCode == AS_NO_ERROR )
            {
                pRequest->setDownloadTrackId( mDownloaderTrackId );
            }
        }
    }

    return retCode;
}

AseErrorCode MediaTrack::requestFromCurrentStream(MediaPresentationRequestPtr& pRequest, bool& pendingRequest)
{
    AseErrorCode retCode = AS_PENDING;
    MediaStreamPtr pCurrentStream;
    pendingRequest = false;
    AseTimeStamp newStreamingTime;
    AseTimeStamp currentStreamingTime = mCurrentStreamingTime;
    do
    {
        uint32_t completedBuffer, pendingBuffer;
        obtainBufferingLen(completedBuffer, pendingBuffer);
        if ( isValid(mIndexForCurrentStream) )
        {
            pCurrentStream = mMediaStreams[mIndexForCurrentStream];
            pendingRequest = pCurrentStream->hasPendingRequest(completedBuffer);
            if ( pendingRequest )
            {
                retCode = pCurrentStream->getNextRequest(newStreamingTime, pRequest);
            }
        }

        if (retCode == AS_NO_ERROR)
        {
            // got a request for media downloading
            // no need to check whether to form a request for header downloading
            pRequest->setDownloadTrackId( mDownloaderTrackId );
            updateStreamingTimeInStreams( newStreamingTime );
            if ( mIndexForPreviousStream != mIndexForCurrentStream )
            {
                DEBUGLOG(MediaPresentationLog, "stream selected: track id: %d, stream id: %d, rebuffer:%u",
                        mTrackId, mIndexForCurrentStream, mRebuffer );
                mStreamingReporterPtr->streamSelected( mManifestId, mTrackId, mIndexForCurrentStream,
                        currentStreamingTime, mCachedThroughput, mLongtermBw, mRebuffer );
                mSelStreamIndex = mIndexForCurrentStream;
                mIndexForPreviousStream = mIndexForCurrentStream;
            }
            break;
        }

        retCode = formHeaderRequest(pRequest);

    } while (false);

    return retCode;
}

AseErrorCode MediaTrack::getNextHeaderRequest(MediaPresentationRequestPtr& pRequest)
{
    if ( mMediaType != MEDIA_VIDEO )
        return AS_PENDING;

    AseErrorCode retCode = formHeaderRequest(pRequest);
    if ( retCode == AS_NO_ERROR )
        return retCode;

    int32_t streamId;
    AseTimeStamp newStreamingTime;
    if ( hasHeaderToDownload(mCachedThroughput, streamId) )
    {
        MediaStreamPtr pCurrentStream = findStreamUsingId(streamId, mIndexForStreamDownloadingHeaders);
        retCode = pCurrentStream->getNextRequest(newStreamingTime, pRequest);
        if ( retCode == AS_NO_ERROR )
        {
            pRequest->setDownloadTrackId( mDownloaderTrackId );
        }
    }
    else
    {
        retCode = AS_PENDING;
    }

    return retCode;
}


AseErrorCode MediaTrack::formHeaderRequest(MediaPresentationRequestPtr& pRequest)
{
    AseErrorCode retCode = AS_PENDING;
    if ( isValid(mIndexForStreamDownloadingHeaders) )
    {
        MediaStreamPtr pCurrentStream = mMediaStreams[mIndexForStreamDownloadingHeaders];
        AseTimeStamp newStreamingTime;
        uint32_t completedBuffer, pendingBuffer;
        obtainBufferingLen(completedBuffer, pendingBuffer);
        if ( pCurrentStream->hasPendingRequest(completedBuffer) )
        {
            // has pending request for header downloading
            retCode = pCurrentStream->getNextRequest(newStreamingTime, pRequest);
            if ( retCode == AS_NO_ERROR )
            {
                pRequest->setDownloadTrackId( mDownloaderTrackId );
            }
        }
        else
        {
            mIndexForStreamDownloadingHeaders = -1;
        }
    }

    return retCode;
}

AseErrorCode MediaTrack::formMediaRequest(MediaPresentationRequestPtr& pRequest)
{
    MediaStreamPtr pCurrentStream;
    IStreamSelector::StreamFeasibility streamFeasibility = IStreamSelector::UNKNOWN_FEASIBILITY;
    int32_t predictedDlTimeForBackupStream = -1;
    if ( mPendingMediaFile && isValid(mIndexForCurrentStream) )
    {
        pCurrentStream = mMediaStreams[mIndexForCurrentStream];
    }
    else
    {
        int32_t streamId;
        uint32_t predictedRebuffer;
        mMediaPresentationPtr.lock()->selectStream(mTrackId, streamId, mCachedThroughput, mLongtermBw,
                predictedRebuffer, streamFeasibility,
                predictedDlTimeForBackupStream);
        mRebuffer = predictedRebuffer;
        assert(streamId >= 0);
        pCurrentStream = findStreamUsingId(streamId, mIndexForCurrentStream);
    }

    AseTimeStamp newStreamingTime;
    AseTimeStamp currentStreamingTime = mCurrentStreamingTime;
    // compute the number of blocks based on gop size
    int32_t requiredBufferingLen = -1;
    if ( mMediaPresentationPtr.lock()->isBufferingState() )
        requiredBufferingLen = mRequiredBufferingLen;

    int32_t numOfBlocks = 1;
    if ( mMediaType == MEDIA_VIDEO )
    {
        numOfBlocks = pCurrentStream->computeNumOfBlocksToRequest(
                                             mAvailableMediaBufferSize, mMinRequestSize,
                                             requiredBufferingLen);
    }

    AseErrorCode retCode = AS_PENDING;
    retCode = pCurrentStream->getNextRequest(numOfBlocks,
                                             newStreamingTime,
                                             predictedDlTimeForBackupStream,
                                             mMediaStreams[ 0 ]->getCurrentFragmentSize(),
                                             pRequest);
    if ( retCode == AS_NO_ERROR )
    {
        pRequest->setDownloadTrackId( mDownloaderTrackId );

        updateStreamingTimeInStreams( newStreamingTime );

        mPendingMediaFile = false;

        if ( mIndexForPreviousStream != mIndexForCurrentStream )
        {
            DEBUGLOG(MediaPresentationLog, "stream selected: track id: %d, stream id: %d, rebuffer:%u",
                    mTrackId, mIndexForCurrentStream, mRebuffer );
            mStreamingReporterPtr->streamSelected( mManifestId, mTrackId, mIndexForCurrentStream,
                    currentStreamingTime, mCachedThroughput, mLongtermBw, mRebuffer );
            mSelStreamIndex = mIndexForCurrentStream;
            mIndexForPreviousStream = mIndexForCurrentStream;
        }
    }
    else
    {
        if ( numOfBlocks == 0 )
            mPendingMediaFile = true;
    }

    if ( (retCode == AS_NO_ERROR) && (pCurrentStream.get()) && (numOfBlocks > 0) )
    {
        if (streamFeasibility == IStreamSelector::UNKNOWN_FEASIBILITY)
        {
            mProbingStream = true;
        }
        else
        {
            mProbingStream = false;
        }
    }

    return retCode;
}

void MediaTrack::streamingHealthCheck()
{
    do
    {
        if ( !mEnableAbandonBlock
                || (mMediaType == MEDIA_AUDIO)
                || (mFirstPendingFragment.get() == NULL)
                || (mMediaPresentationPtr.lock()->isBufferingState())
                || allPendingRequestsAtLowest() )
            break;

        bool bitrateSwitchFreeze = mMediaPresentationPtr.lock()->bitrateSwitchFreeze() && mProbingStream;
        if (bitrateSwitchFreeze)
            break;

        // guard interval to prevent deadlock
        AseTimeVal now = AseTimeVal::now();
        if ( mPreviousBlockAbandonTime + AseTimeVal::fromMS(mAbandonBlockGuardInterval) > now )
        {
            break;
        }

        MediaFragmentPtr pMediaFragment = needToAbandonPendingRequest();
        if ( pMediaFragment.get() )
        {
            DEBUGLOG(MediaPresentationLog,
                "AbandonBlock: %lldms", pMediaFragment->getStartPts().getValueInMs() );
            abandonPendingRequests(pMediaFragment);
            mPreviousBlockAbandonTime = now;
        }

    } while (false);
}

MediaFragmentPtr MediaTrack::needToAbandonPendingRequest()
{
    uint32_t completedBuffer, pendingBuffer;
    obtainBufferingLen(completedBuffer, pendingBuffer);
    MediaFragmentPtr pFirstAbandonFragment = MediaFragmentPtr();

    if ( completedBuffer <= mAbandonThreshold )
    {
        // avoid abandoing blocks while the buffer level is low
        return pFirstAbandonFragment;
    }

    uint32_t bufferCushion = 2002;
    uint32_t minRequiredBuffer = (mLowWatermarkLevel > bufferCushion) ?
                                            (mLowWatermarkLevel - bufferCushion) : 0;

    while ( mFirstPendingFragment != pFirstAbandonFragment )
    {
        if (!needToAbandon(mFirstPendingFragment, pFirstAbandonFragment, completedBuffer, minRequiredBuffer))
        {
            break;
        }

        if ( pFirstAbandonFragment.get() == NULL )
            pFirstAbandonFragment = mMediaFragments.back();
        else
            pFirstAbandonFragment = pFirstAbandonFragment->getPrevFragment();
    }

    if ( (mFirstPendingFragment == pFirstAbandonFragment) &&
            mFirstPendingFragment->hasStartedForwardingData() )
    {
        DEBUGLOG(MediaPresentationLog,
                "first pending fragment: %lld ms",
                mFirstPendingFragment->getStartPts().getValueInMs());
        // can not abandon a fragment which has started forwarding data
        pFirstAbandonFragment = mFirstPendingFragment->getNextFragment();
    }

    return pFirstAbandonFragment;
}

bool MediaTrack::needToAbandon(MediaFragmentPtr pFirstFragment,
                                MediaFragmentPtr pLastFragment,
                                uint32_t completedBuffer,
                                uint32_t minRequiredBuffer)
{

    uint32_t downloadTime = 0;
    while( pFirstFragment != pLastFragment )
    {
        shared_ptr<ILocationSetMonitor> pLocationSetMonitor =
            NetworkMonitor::instance()->getCurrentLocationSetMonitor();
        uint32_t throughput = pLocationSetMonitor->getShortTermThroughput(AseTimeVal::now());
        downloadTime += pFirstFragment->predictCompleteTime(throughput);
        completedBuffer += pFirstFragment->getDuration().getValueInMs();
        pFirstFragment = pFirstFragment->getNextFragment();
    }

    uint32_t predictedBufferLen = completedBuffer > downloadTime ?
                                    completedBuffer - downloadTime : 0;

    bool abandonRequest = false;
    if ( predictedBufferLen < minRequiredBuffer )
    {
        DEBUGLOG(MediaPresentationLog,
                "needToAbandon: predicted buffer len: %u ms, min required buffer: %u ms",
                predictedBufferLen, minRequiredBuffer);
        abandonRequest = true;
    }

    return abandonRequest;

}

bool MediaTrack::allPendingRequestsAtLowest()
{
    MediaFragmentPtr pMediaFragment = mFirstPendingFragment;
    bool allLowestBitrate = true;
    while ( pMediaFragment.get() )
    {
        if ( pMediaFragment->getMediaStream()->getAverageBitrate() > (int32_t)mLowestBitrate )
        {
            allLowestBitrate = false;
            break;
        }
        pMediaFragment = pMediaFragment->getNextFragment();
    }

    return allLowestBitrate;
}

/** Not to abandon all pending fragments */
void MediaTrack::abandonPendingRequests( MediaFragmentPtr pFirstAbandonFragment )
{
    AseTimeStamp streamingTime = pFirstAbandonFragment->getStartPts();
    DEBUGLOG(MediaPresentationLog, "abandonPendingRequests: %lld ms",
                                streamingTime.getValueInMs() );

    while ( !mMediaFragments.empty() )
    {
        MediaFragmentPtr pMediaFragment = mMediaFragments.back();
        pMediaFragment->abandonImmediately();
        popMediaFragment();
        if ( pMediaFragment->getStartPts() == streamingTime )
        {
            break;
        }
    }

    if ( mFirstPendingFragment->getStartPts() == streamingTime )
        mFirstPendingFragment.reset();

    updateStreamingTimeInStreams(streamingTime);
}


/** TODO: also use the predicted buffering level to decide whether to make new request
*/
bool MediaTrack::tooMuchPendingBuffer()
{
    uint32_t completedBuffer, pendingBufferLen;
    obtainBufferingLen(completedBuffer, pendingBufferLen);

    uint32_t targetPendingBufferLen = completedBuffer * mMaxPendingBufferPercentage / 100;
    if ( targetPendingBufferLen < mMinPendingBufferLen )
        targetPendingBufferLen = mMinPendingBufferLen;
    if ( targetPendingBufferLen > mMaxPendingBufferLen )
        targetPendingBufferLen = mMaxPendingBufferLen;

    if ( mMediaPresentationPtr.lock()->isBufferingState() &&
            (pendingBufferLen > 0) )
    {
        return true;
    }

    if ( endOfTrack() || (pendingBufferLen > targetPendingBufferLen) )
        return true;
    else
        return false;

}

MediaStreamPtr MediaTrack::findStreamUsingId(int32_t streamId, int32_t& updatedStreamIndex)
{
    MediaStreamPtr pCurrentStream;

    if ((uint32_t)streamId >= mMediaStreams.size())
        streamId = mMediaStreams.size() - 1;

    // find the stream with the given streamId
    for ( uint32_t i = 0; i < mMediaStreams.size(); ++i )
    {
        if ( mMediaStreams[i]->streamId() == streamId )
        {
            updatedStreamIndex = i;
            pCurrentStream = mMediaStreams[i];
            break;
        }
    }

    return pCurrentStream;
}

MediaStreamPtr MediaTrack::findAlternativeValidStream(int32_t& updatedStreamIndex)
{
    MediaStreamPtr pCurrentStream;

    if (updatedStreamIndex >= (int32_t)mMediaStreams.size() )
        updatedStreamIndex = mMediaStreams.size() - 1;

    bool findStream = false;
    for (uint32_t i = updatedStreamIndex; i < mMediaStreams.size(); ++i)
    {
        if ( mMediaStreams[i]->isAvailable() )
        {
            findStream = true;
            updatedStreamIndex = i;
            pCurrentStream = mMediaStreams[i];
            break;
        }
    }

    if ( !findStream && (updatedStreamIndex > 0))
    {
        for ( int32_t i = updatedStreamIndex - 1; i >= 0; --i )
        {
            if ( mMediaStreams[i]->isAvailable() )
            {
                findStream = true;
                updatedStreamIndex = i;
                pCurrentStream = mMediaStreams[i];
                break;
            }
        }
    }

    assert(findStream);

    return pCurrentStream;
}

void MediaTrack::startSwitch(AseTimeStamp const& playbackTime)
{
    AseTimeStamp newStreamingTime;
    setStreamingTime(playbackTime, newStreamingTime);
    updatePlaybackTime(playbackTime);
    mDlNonEmptyBlocks = 0;
    if ( mMediaType == MEDIA_AUDIO )
        mSwitchingTrack = true;
    return;
}

/** -------------------------------------------------------------------- */
/**                         McQueen Functions                            */

AseErrorCode MediaTrack::scheduleMigrate(ManifestTrackPtr pManifestTrack,
                         int32_t manifestId,
                         int32_t trackId)
{
    DEBUGLOG(MediaPresentationLog, "scheduleMigrate, manifest id: %d, track id: %d",
                        manifestId, trackId);
    mNextManifestTrackInfo.pManifestTrack = pManifestTrack;
    mNextManifestTrackInfo.manifestId = manifestId;
    mNextManifestTrackInfo.trackId = trackId;

    AseErrorCode retCode = AS_NO_ERROR;
    if (mMediaType == MEDIA_VIDEO)
    {
        retCode = MediaTrack::construct( mMediaPresentationPtr.lock(), pManifestTrack,
                                         false, mMediaSinkPtr,
                                         manifestId, trackId,
                                         mDownloaderTrackId, mMediaBufferPoolPtr,
                                         mStreamingReporterPtr, mLocationSelectorPtr,
                                         mConfigParameterPtr, mLocalConfigParametePtr,
                                         mMediaFiles, mTmpTrack );
    }

    return retCode;

}

void MediaTrack::notifyLicenseAcquired()
{
    mLicenseAcquired = true;
    if ( mFastStart && !hasPendingRequest() && !mSwitchingTrack)
    {
        // only pause the track for migration after all pending requests have been sent out
        // ready for migration after receiving the license
        mPaused = true;
        DEBUGLOG(MediaPresentationLog, "notifyLicenseAcquired: pause the track");
    }
}

// Don't have any pending request, so it's safe to migrate
bool MediaTrack::readyForMigration() const
{
    DEBUGLOG(MediaPresentationLog, "track id: %u readyForMigration: %u, %u, %u",
            mTrackId,
            mPaused, hasPendingRequest(), hasPendingTasks());

    return (mPaused && !hasPendingRequest() && !hasPendingTasks());
}

bool MediaTrack::hasPendingRequest() const
{
    bool pendingRequest = false;
    if ( isValid(mIndexForCurrentStream) )
    {
        uint32_t completedBuffer, pendingBuffer;
        obtainBufferingLen(completedBuffer, pendingBuffer);
        MediaStreamPtr pCurrentStream = mMediaStreams[mIndexForCurrentStream];
        pendingRequest = pCurrentStream->hasPendingRequest(completedBuffer);

    }

    return pendingRequest;
}

bool MediaTrack::checkForMigration()
{
    if ( mMediaType == MEDIA_AUDIO )
    {
        // use video track to trigger the migration
        return false;
    }

    DEBUGLOG(MediaPresentationLog, "checkForMigration: %u, %u, %u",
            mLicenseAcquired,
            mNextManifestTrackInfo.manifestId,
            mNextManifestTrackInfo.trackId);

    if ( mLicenseAcquired &&
            readyForMigration() &&
            mMediaPresentationPtr.lock()->audioTrackReadyForMigration() &&
            (mNextManifestTrackInfo.manifestId >= 0) &&
            (mNextManifestTrackInfo.trackId >= 0) )
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool MediaTrack::waitForDrmManifest()
{
    bool waitForDrmManifest = false;
    do
    {
        if ( !mFastStart )
            break;

        AseTimeStamp seekTime = this->seekTime();
        if ( (seekTime.getValueInMs() > 0) && (mDuration > 0) &&
                (seekTime.getValueInMs() + mGuardDurationForFirstManifest >= mDuration) )
        {
            DEBUGLOG(MediaPresentationLog, "refusing request: wait for manifest");
            waitForDrmManifest = true;
            break;
        }

        if (seekTime.getValueInMs() + mMaxNonDrmContentPlayLen < mCurrentStreamingTime.getValueInMs())
        {
            DEBUGLOG(MediaPresentationLog, "refusing request: streaming non-drm too long");
            waitForDrmManifest = true;
            break;
        }
    } while(false);

    return waitForDrmManifest;

}

shared_ptr<IMediaFile>
MediaTrack::findPreExistingMediaFile(
                        std::vector<shared_ptr<IMediaFile> > const& mediaFiles,
                        std::string const& streamKey,
                        int32_t streamId)
{
    IMediaFilePtr pMediaFile = IMediaFilePtr();

    for ( vector<IMediaFilePtr>::const_iterator iter = mediaFiles.begin();
            iter != mediaFiles.end();
            ++iter )
    {
        if ( (*iter)->getStreamKey() == streamKey )
        {
            pMediaFile = *iter;
            mSelStreamIndex = streamId;
            break;
        }
    }

#ifdef NRDP_HAS_TRACING
    if ( pMediaFile )
        DEBUGLOG(MediaPresentationLog, "find existing media file: %s, %d", streamKey.c_str(), streamId);
#endif
    return pMediaFile;
}

AseErrorCode MediaTrack::getDrmHeaderRequest(MediaPresentationRequestPtr& pRequest)
{
    AseErrorCode retCode = AS_NOT_ALlOWED;

    do
    {
        if (mLicenseAcquired)
        {
            break;
        }

        if (mTmpTrack.get() == NULL)
        {
            // has not received the migrate schedule yet
            break;
        }

        uint32_t currentBufferLen, pendingBufferLen;
        obtainBufferingLen(currentBufferLen, pendingBufferLen);
        // TODO: change 1 minute into a parameter
        if ( (currentBufferLen <= mMinBufferLenForHeaderDownloading) &&
                (mCurrentPlaybackTime.getValueInMs() < 60 * 1000) &&
                (mNumOfPlayCompleted == 0) )
        {
            // Avoid drm header request when buffer level is too low during the first playback attempt
            DEBUGLOG(MediaPresentationLog, "getDrmHeaderRequest: buffer too low");
            break;
        }

        if ( mTmpTrack->deliverHeader() )
        {
            DEBUGLOG(MediaPresentationLog, "getDrmHeaderRequest: has delivered the header for the drm manifest");
            break;
        }

        DEBUGLOG(MediaPresentationLog, "getDrmHeaderRequest: track id: %u",
                mTmpTrack->trackId());
        retCode = mTmpTrack->getNextRequest(pRequest);

    } while(false);

    return retCode;
}


AseErrorCode MediaTrack::migrate()
{
    ManifestTrackPtr pManifestTrack = mNextManifestTrackInfo.pManifestTrack;
    int32_t manifestId = mNextManifestTrackInfo.manifestId;
    int32_t trackId = mNextManifestTrackInfo.trackId;
    mNextManifestTrackInfo.reset();

    AseErrorCode retCode1 = AS_NOT_FOUND;
    AseErrorCode retCode2 = AS_NO_ERROR;
    int32_t oldAverageBitrate = getStreamingBitrate();
    DEBUGLOG(MediaPresentationLog, "migrate: old bitrate: %d kbps", oldAverageBitrate);
    if ( oldAverageBitrate == 0 )
        retCode1 = AS_NO_ERROR;

    mManifestTrackPtr = pManifestTrack;
    mIndexForCurrentStream = -1;
    mPendingMediaFile = false;
    mIndexForPreviousStream = -1;
    mIndexForPreviousHeaderDownloading = -1;
    mSelStreamIndex = -1;

    mAllHeadersDownloaded = false;

    mManifestId = manifestId;
    mTrackId = trackId;
    mDuration = 0;

    vector<ManifestStreamPtr> const& manifestStreams = pManifestTrack->getStreams();
    int32_t streamId = 0;
    MediaStreamPtr pMediaStream;
    // back up the old media streams before switching to new media streams
    mOldMediaStreams = mMediaStreams;
    mMediaStreams.clear();
    for ( vector<ManifestStreamPtr>::const_iterator iter = manifestStreams.begin();
            iter != manifestStreams.end();

            ++iter )
    {
        shared_ptr<IMediaFile> pPreExistingMediaFile = IMediaFilePtr();
         if ( (*iter)->enabled() )
         {
             pPreExistingMediaFile = findPreExistingMediaFile(mMediaFiles, (*iter)->getStreamKey(), streamId);
         }
        retCode2 =
            MediaStream::construct( shared_from_this(), mMediaPresentationPtr.lock(), *iter, mMediaSinkPtr,
                                mManifestId, streamId,
                                mMediaBufferPoolPtr, mLocationSelectorPtr,
                                mConfigParameterPtr,
                                mManifestTrackPtr->getTimestampOffset(),
                                pPreExistingMediaFile,
                                pMediaStream );
        if ( retCode2 != AS_NO_ERROR )
        {
            break;
        }

        pMediaStream->updateStreamingTime(mCurrentStreamingTime);
        mMediaStreams.push_back(pMediaStream);
        DEBUGLOG(MediaPresentationLog, "migrate: bitrate: %d kbps", (*iter)->getNominalBitrate());
        if ( (*iter)->getNominalBitrate() == oldAverageBitrate )
        {
            mSelStreamIndex = streamId;
            DEBUGLOG(MediaPresentationLog, "migrate: new sel stream index: %d ", mSelStreamIndex);
            retCode1 = AS_NO_ERROR;
        }

        ++streamId;
    }

    mCompleted = false;
    mDlNonEmptyBlocks = 0;
    mEndOfTrack = false;
    mFastStart = false;
    mPaused = false;

    mTrackBufferInfoPtr->setTrackId(mTrackId);
    if ( (retCode1 == AS_NO_ERROR) && (retCode2 == AS_NO_ERROR) )
    {
        setVideoBitrateRanges(mVideoBitrateRanges);
        return AS_NO_ERROR;
    }
    else
        return AS_NOT_FOUND;
}


/**                         McQueen Functions                            */
/** -------------------------------------------------------------------- */

