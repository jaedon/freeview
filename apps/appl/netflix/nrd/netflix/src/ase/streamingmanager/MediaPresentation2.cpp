/*
 *
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaPresentation2.h"
#include "MediaTrack2.h"
#include "StreamingCommand.h"
#include "NetworkMonitor.h"
#include "MediaPresentationRequest.h"
#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/AseCommonUtil.h>

#include <algorithm>
#include <functional>
#include <list>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/Log.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdase/ILocationSetMonitor.h>

using namespace netflix;
using namespace netflix::ase;
using namespace std;

/** -------------------------------------------------------------------- */
// 1. McQueen:
// * schedule migrate inside video track as soon as MP receives the new manifest;
// * video track schedules header request for the new manifest, gets notified of the license, and
// migrates to the new manifest.
// * It notifies the MP to make the switch on manifest. MP triggers audio track to migrate
//   to use the new manifest
// 2. selectStream is both data and timer driven. selectStream is called when either a gop has been
//    downloaded or timeout. buffering status is data driven. The player updates buffering status when
//    a new gop has been downloaded.

namespace netflix {
namespace ase {
DEFINE_LOGGER( MediaPresentationLog );
}
}

AseErrorCode MediaPresentation::construct(
                              shared_ptr<StreamingManager> pStreamingManager,
                              ManifestPtr pManifest,
                              IMediaSinkPtr const& pMediaSink,
                              IMediaBufferPoolPtr const& pMediaBufferPool,
                              IStreamingHeuristicPtr const& pStreamingHeuristic,
                              IAseConfigParameterPtr const& pConfigParameter,
                              IAseConfigParameterPtr pLocalConfigParameter,
                              IStreamingReporterPtr const& pStreamingReporter,
                              IPersistentStorePtr const& pPersistentStore,
                              MediaPresentationPtr&     pMediaPresentation )
{
    // Create object
    pMediaPresentation.reset( new MediaPresentation ( pStreamingManager,
                                                      pManifest,
                                                      pMediaSink,
                                                      pMediaBufferPool,
                                                      pStreamingHeuristic,
                                                      pConfigParameter,
                                                      pLocalConfigParameter,
                                                      pStreamingReporter,
                                                      pPersistentStore) );
    return AS_NO_ERROR;
}



MediaPresentation::MediaPresentation( shared_ptr<StreamingManager> pStreamingManager,
                       ManifestPtr pManifest,
                       IMediaSinkPtr const& pMediaSink,
                       IMediaBufferPoolPtr const& pMediaBufferPool,
                       IStreamingHeuristicPtr const& pStreamingHeuristic,
                       IAseConfigParameterPtr const& pConfigParameter,
                       IAseConfigParameterPtr pLocalConfigParameter,
                       IStreamingReporterPtr const& pStreamingReporter,
                       IPersistentStorePtr const& pPersistentStore)
                : mStreamingManagerPtr(pStreamingManager),
                  mMediaConsumerState(MEDIA_CONSUMER_STOPPED_STATE),
                  mStreamingSpeed(1),
                  mStreamingReporterPtr(pStreamingReporter),
                  mPersistentStorePtr(pPersistentStore),
                  mMediaSinkPtr(pMediaSink),
                  mFailed(false),
                  mMediaBufferPoolPtr(pMediaBufferPool),
                  mTotalMediaBufferSize(mMediaBufferPoolPtr->getUnitSize() * mMediaBufferPoolPtr->getUnitCount()),
                  mMinMediaBufferLen(30000),
                  mNetworkMonitorPtr(NetworkMonitor::instance()),
                  mLocationSelectorPtr(pStreamingHeuristic->getLocationSelector()),
                  mStreamSelectorPtr(pStreamingHeuristic->getStreamSelector()),
                  mConnectionConfigurationPtr(pStreamingHeuristic->getConnectionConfiguration()),
                  mStreamingStatPtr(pStreamingHeuristic->getStreamingStatistics()),
                  mConfigParamterPtr(pConfigParameter),
                  mLocalConfigParametePtr(pLocalConfigParameter),
                  mSeekTime( AseTimeStamp::INFINITE ),
                  mActualStartPts(AseTimeStamp::INFINITE),
                  mAccumulatedBytesDownloaded(0),
                  mStatusUpdateInterval(2000),
                  mNextStatusUpdateTime( AseTimeVal::now() + AseTimeVal::fromMS(mStatusUpdateInterval)),
                  mPrevBufferLen(0),
                  mPredictedBufferingTime(-1),
                  mInitBandwidth(-1),
                  mNumOfPlayAttempts(0),
                  mPredictedRebuffers(0),
                  mVideoDlTrackId(-1),
                  mAudioDlTrackId(-1),
                  mHeadersDlTrackId(-1),
                  mVideoTrackDownloaderPaused(false),
                  mInternalStreamingState(DOWNLOADING),
                  mPaused(false),
                  mLicenseAcquired(false),
                  mWaitForManifest(false),
                  mContentDuration(0),
                  mPrebufferingStartTime(0),
                  mHasSleptForPerfDevice(false),
                  mSleepStartTime(AseTimeVal::ZERO),
                  mSleepCounter(0),
                  mCmdMutex(MPCMD_MUTEX, "MpCmdMutex")
{
    mManifestPtrs.clear();
    mManifestPtrs.push_back(pManifest);

    mCommandQueuePtr.reset(new StreamingCommandQueue());

    getConfiguration();
}

void MediaPresentation::configure( const IAseConfigParameterPtr &pConfig )
{
    DEBUGLOG( MediaPresentationLog, "MediaPresentation::configure()" );

    mConfigParamterPtr = pConfig;

    getConfiguration();

    mStreamSelectorPtr->configure( pConfig );

    mLocationSelectorPtr->configure( pConfig );

    std::vector<MediaTrackPtr>::iterator it = mMediaTracks.begin();
    for( ; it != mMediaTracks.end(); ++it )
    {
        (*it)->configure( pConfig );
    }
}

void MediaPresentation::getConfiguration()
{
    mLowWatermarkLevel = mConfigParamterPtr->getParameterAs<uint32_t>("lowWatermarkLevel", 15000);
    mMediaBufferHighTargetAvailableSize = mConfigParamterPtr->getParameterAs<uint32_t>(
                                                            "mediaBufferTargetAvailableSize",
                                                            1024 * 1024 );
    mAudioBufferHighTargetAvailableSize = mConfigParamterPtr->getParameterAs<uint32_t>(
                                                            "audioBufferTargetAvailableSize",
                                                            8 * 1024, mMediaBufferHighTargetAvailableSize, 256 * 1024 );

    uint32_t socketReceiveBufferSize = mLocalConfigParametePtr->getParameterAs<uint32_t>(
                                                            "socketReceiveBufferSize", 256 * 1024);
    mMediaBufferLowTargetAvailableSize = mConfigParamterPtr->getParameterAs<uint32_t>(
                                                            "socketReceiveBufferSize", socketReceiveBufferSize);
    mMaxStreamingSkew = mConfigParamterPtr->getParameterAs<uint32_t>(
                                    "maxStreamingSkew", 2002);

    mMaxMediaBufferLenAllowed = mConfigParamterPtr->getParameterAs<uint32_t>("maxMediaBufferAllowed", 240 * 1000);
    mMaxMediaBufferLenAllowedInFastplay = mConfigParamterPtr->getParameterAs<uint32_t>("maxMediaBufferAllowedInFastplay", 30 * 1000);
    mEffectiveMaxMediaBufferLen = mMaxMediaBufferLenAllowed;
    mMaxMediaBufferLenConfiged = mMaxMediaBufferLenAllowed;
    if (mMediaBufferLowTargetAvailableSize < 256 * 1024)
        mMediaBufferLowTargetAvailableSize = 256 * 1024;

    mPrebufferTimeLimit = mConfigParamterPtr->getParameterAs<uint32_t>("prebufferTimeLimit", 60000);
    mMinPrebufLength = mConfigParamterPtr->getParameterAs<uint32_t>("minPrebufSize", DEFAULT_MIN_PREBUF_SIZE);

    mMinDownloadSizeAtResume = 2 * mMediaBufferLowTargetAvailableSize;

    mBufferingProgressUpdateInterval = mConfigParamterPtr->getParameterAs<uint32_t>("bufferingProgressUpdateInterval", 5);

    mConfigedVideoBufferSize = mConfigParamterPtr->getParameterAs<uint32_t>("configedVideoBufferSize", -1);
    mConfigedAudioBufferSize = mConfigParamterPtr->getParameterAs<uint32_t>("configedAudioBufferSize", -1);

    mBufferThresholdForShortSleep = mConfigParamterPtr->getParameterAs<uint32_t>("bufferThresholdForShortSleep", 3600000);
    mShortSleepForPerfDevice = mConfigParamterPtr->getParameterAs<uint32_t>("shortSleepForPerfDevice", 1000);

    mPauseStreamingAfterBufferingComplete = mConfigParamterPtr->getParameterAs<bool>("pauseStreamingAfterBC", false);
}

/** -------------------------------------------------------------------- */
/**             Commands from Streaming Manager                          */
// MediaPresentation::open --> HttpRequestManager::start
AseErrorCode MediaPresentation::open(
                    const vector<uint32_t>& trackIndexes,
                    bool fastplay,
                    vector<shared_ptr<IMediaFile> > const& mediaFiles )
{
    mFastStart = fastplay;
    mPreExistingMediaFiles = mediaFiles;

    if ( mMediaConsumerState != IMediaConsumer::MEDIA_CONSUMER_STOPPED_STATE )
        return AS_NOT_ALlOWED;

    AseErrorCode retCode = constructMediaTracks(trackIndexes);
    if ( retCode == AS_NO_ERROR )
    {
        DEBUGLOG( MediaPresentationLog, "MediaPresentation::opening fastplay: %u", fastplay );
        mMediaConsumerState = MEDIA_CONSUMER_BUFFERING_STATE;
        mLocationSelectorPtr->notifyMediaConsumerState( mMediaConsumerState );
        mLocationSelectorPtr->setFastplay(fastplay);

        if ( mFastStart )
        {
            mEffectiveMaxMediaBufferLen =
                min( min(mMaxMediaBufferLenConfiged, mMaxMediaBufferLenAllowed), mMaxMediaBufferLenAllowedInFastplay);
        }
        else
        {
            mEffectiveMaxMediaBufferLen = min(mMaxMediaBufferLenConfiged, mMaxMediaBufferLenAllowed);
        }

    }

    return retCode;
}

AseErrorCode MediaPresentation::start( AseTimeStamp const& seekTime )
{
    ullong usedBuffer, availableVideoBuffer, availableAudioBuffer;
    getBufferStats(MEDIA_VIDEO, usedBuffer, availableVideoBuffer);
    getBufferStats(MEDIA_AUDIO, usedBuffer, availableAudioBuffer);
    DEBUGLOG( MediaPresentationLog, "MediaPresentation::start, seek time: %llu ms, "
                                    "available a/v buffer: (%llu, %llu) bytes ",
                        seekTime.getValueInMs(), availableAudioBuffer, availableVideoBuffer );

    ++mNumOfPlayAttempts;
    mMediaConsumerState = MEDIA_CONSUMER_BUFFERING_STATE;
    mHasSleptForPerfDevice = false;
    mSleepStartTime = AseTimeVal::ZERO;
    mSleepCounter = 0;
    mLocationSelectorPtr->notifyMediaConsumerState( mMediaConsumerState );

    shared_ptr<SetSeekTimeCommand>
        pSetSeekTimeCmd(new SetSeekTimeCommand(static_pointer_cast<MediaPresentation>(shared_from_this()), seekTime));
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->addCommand(pSetSeekTimeCmd);
    }

    return AS_NO_ERROR;
}

/** Streaming Manager needs to stop HttpRequestManager first */
// HttpRequestManager stop --> MediaPresentation stop
AseErrorCode MediaPresentation::stop( )
{
    if ( mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE )
        return AS_NOT_ALlOWED;
    DEBUGLOG( MediaPresentationLog, "MediaPresentation::stopping ");

    mMediaConsumerState = MEDIA_CONSUMER_STOPPED_STATE;
    mLocationSelectorPtr->notifyMediaConsumerState( mMediaConsumerState );
    mStreamSelectorPtr->stop();

    // process all streaming commands
    if ( mCommandQueuePtr->mNumOfCommands > 0 )
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->runCommands();
    }


    mFailed = false;
    mSeekTime = AseTimeStamp::INFINITE;
    mActualStartPts = mSeekTime;
    mPredictedBufferingTime = -1;
    mInitBandwidth = -1;
    mWaitForManifest = false;
    DEBUGLOG( MediaPresentationLog, "stopping all media tracks" );
    for ( vector<MediaTrackPtr>::iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        (*iter)->reset();
    }

    mPaused = false;
    mVideoDlTrackId = -1;
    mAudioDlTrackId = -1;
    mHeadersDlTrackId = -1;
    mVideoTrackDownloaderPaused = false;
    mPredictedRebuffers = 0;

    mLocationSelectorPtr->notifyMediaConsumerState( MEDIA_CONSUMER_STOPPED_STATE );
    mLocationSelectorPtr->notifyMediaPlaybackState( 0, false );

    mStreamingSpeed = 1;
    DEBUGLOG( MediaPresentationLog, "MediaPresentation::stopped ");
    return AS_NO_ERROR;
}

AseErrorCode MediaPresentation::close()
{
    mMediaConsumerState = MEDIA_CONSUMER_STOPPED_STATE;
    mLocationSelectorPtr->notifyMediaConsumerState( mMediaConsumerState );
    mFailed = false;
    // cleanup tracks and streams
    for ( vector<MediaTrackPtr>::iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
        (*iter)->reset();

    mStreamDurations.clear();
    mPreExistingMediaFiles.clear();

    DEBUGLOG( MediaPresentationLog, "MediaPresentation::closed " );
    return AS_NO_ERROR;
}

AseErrorCode MediaPresentation::switchTracks( const vector<uint32_t>& trackIndexes )
{
    shared_ptr<SwitchTracksCommand>
        pSwitchTracksCmd(new SwitchTracksCommand(static_pointer_cast<MediaPresentation>(shared_from_this()), trackIndexes));
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->addCommand(pSwitchTracksCmd);
    }
    return AS_NO_ERROR;
}

void MediaPresentation::skip()
{
    shared_ptr<SkipCommand>
        pSkipCmd(new SkipCommand(static_pointer_cast<MediaPresentation>(shared_from_this())));
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->addCommand(pSkipCmd);
    }
}


AseErrorCode MediaPresentation::rebuffer()
{
    DEBUGLOG(MediaPresentationLog, "rebuffer received!");

    mStreamingSpeed = 1;
    mSleepCounter = 0;

    if ( mMediaConsumerState != MEDIA_CONSUMER_PLAYING_STATE )
        return AS_NOT_ALlOWED;

    shared_ptr<RebufferCommand>
        pRebufferCmd(new RebufferCommand(static_pointer_cast<MediaPresentation>(shared_from_this())));
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->addCommand(pRebufferCmd);
    }

    return AS_NO_ERROR;
}

AseErrorCode MediaPresentation::addManifest(ManifestPtr pManifest)
{
    DEBUGLOG(MediaPresentationLog, "added a new manifest");
    shared_ptr<AddManifestCommand>
        pAddManifestCmd(new AddManifestCommand(static_pointer_cast<MediaPresentation>(shared_from_this()), pManifest));
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->addCommand(pAddManifestCmd);
    }
    return AS_NO_ERROR;
}

void MediaPresentation::notifyLicenseAcquired( uint32_t manifestManifestIndex )
{
    NRDP_UNUSED(manifestManifestIndex);
    DEBUGLOG(MediaPresentationLog, "license acquired");
    shared_ptr<NotifyLicenseAcquiredCommand>
        pNotifyLicenseAcquiredCommand(new NotifyLicenseAcquiredCommand(static_pointer_cast<MediaPresentation>(shared_from_this()) ));
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->addCommand(pNotifyLicenseAcquiredCommand);
    }
}

void MediaPresentation::setVideoBitrateRanges( vector<shared_ptr<BitrateRange> > const& videoBitrateRanges )
{
    shared_ptr<SetVideoBitrateRangeCommand>
        pSetVideoBitrateRangeCmd(new SetVideoBitrateRangeCommand(static_pointer_cast<MediaPresentation>(shared_from_this()), videoBitrateRanges));
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->addCommand(pSetVideoBitrateRangeCmd);
    }
}


uint32_t MediaPresentation::getMaxStreamingBuffer() const
{
    return mMaxMediaBufferLenConfiged;
}


void MediaPresentation::setStreamingBuffer(bool powerSaving,
                                              uint32_t maxBufferLen,
                                              uint32_t minBufferLen)
{
    NRDP_UNUSED(powerSaving);
    shared_ptr<SetStreamingBuffer>
        pSetStreamingBuffer(new SetStreamingBuffer(static_pointer_cast<MediaPresentation>(shared_from_this()), maxBufferLen, minBufferLen));
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->addCommand(pSetStreamingBuffer);
    }
    return;
}

void MediaPresentation::updateDownloaderTrackId(uint32_t videoDownloaderTrackId,
                                                uint32_t audioDownloaderTrackId,
                                                uint32_t headersDownloaderTrackId)
{
    mVideoDlTrackId = (int32_t) videoDownloaderTrackId;
    mAudioDlTrackId = (int32_t) audioDownloaderTrackId;
    mHeadersDlTrackId = (int32_t) headersDownloaderTrackId;
    shared_ptr<UpdateDownloaderTrackId>
        pUpdateDownloaderTrackId(new UpdateDownloaderTrackId(static_pointer_cast<MediaPresentation>(shared_from_this())));
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->addCommand(pUpdateDownloaderTrackId);
    }
    return;
}

void MediaPresentation::setStreamingSpeed(uint32_t speed)
{
    mStreamingSpeed = speed;
    if ( speed > 0 )
    {
        NTRACE(TRACE_STREAMINGSTAT, " Has slept %u times. ", mSleepCounter);
    }
}


/**             Commands from Streaming Manager                          */
/** -------------------------------------------------------------------- */




/** -------------------------------------------------------------------- */
/**             IMediaConsumer API implementation                        */
AseErrorCode MediaPresentation::getNextRequest( uint32_t downloadTrackId, MediaRequestPtr& pMediaRequest )
{
    MediaPresentationRequestPtr pRequest;

    if (mFailed)
        return AS_NOT_ALlOWED;

    // process all streaming commands
    if ( mCommandQueuePtr->mNumOfCommands > 0 )
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        mCommandQueuePtr->runCommands();
        if ( mUnfinishedStreamingCommandPtr )
        {
            mCommandQueuePtr->addCommand(mUnfinishedStreamingCommandPtr);
            mUnfinishedStreamingCommandPtr.reset();

        }
    }
    AseTimeVal now = AseTimeVal::now();

    if ( (mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE) || mPaused )
        return AS_PENDING;

    switch (mMediaConsumerState)
    {
        case MEDIA_CONSUMER_PLAYING_STATE:
            updatePlaybackTime();
            break;
        case MEDIA_CONSUMER_BUFFERING_STATE:
        case MEDIA_CONSUMER_REBUFFERING_STATE:
            checkBufferingStatus();
            break;
        default:
            break;
    }

    if ( now > mNextStatusUpdateTime )
    {
        mNextStatusUpdateTime = now + AseTimeVal::fromMS(mStatusUpdateInterval);
        statusUpdate();
        NTRACE(TRACE_STREAMINGSTAT,
                "download track ids: %u:%u:%u",
                downloadTrackId, mVideoDlTrackId, mAudioDlTrackId);
        uint32_t playableLength;
        bool goodQuality;
        getMediaPlaybackState(playableLength, goodQuality);
        mLocationSelectorPtr->notifyMediaPlaybackState(playableLength, goodQuality);
    }

    AseErrorCode retCode = AS_PENDING;
    MediaTrackPtr pMediaTrack;
    // obtain the minimum streaming time between two tracks
    AseTimeStamp minStreamingTime = AseTimeStamp::INFINITE;
    AseTimeStamp streamingTime;
    AseTimeStamp audioStreamingTime, videoStreamingTime;
    for ( vector<MediaTrackPtr>::const_iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        streamingTime = (*iter)->streamingTime();
        if ( (streamingTime < minStreamingTime) && !(*iter)->endOfTrack() )
            minStreamingTime = streamingTime;

        if ( (*iter)->mediaType() == MEDIA_VIDEO )
            videoStreamingTime = streamingTime;
        else
            audioStreamingTime = streamingTime;
    }
    if ( mVideoTrackDownloaderPaused &&
            (audioStreamingTime + AseTimeStamp(mMaxStreamingSkew, 1000) >= videoStreamingTime) )
    {
        // After the on-the-fly audio track switch, audio track has caught up with the video track
        mStreamingManagerPtr.lock()->resumeVideoTrackDownloader();
        mVideoTrackDownloaderPaused = false;
    }

    uint32_t currentBufferLen, maxPendingBufferLen;
    obtainMixedBufferLen(currentBufferLen, maxPendingBufferLen);
    if ( !mHasSleptForPerfDevice && (currentBufferLen > mBufferThresholdForShortSleep) )
    {
        mHasSleptForPerfDevice = true;
        mSleepStartTime = now;
        return AS_PENDING;
    }
    else if ( mSleepStartTime > AseTimeVal::ZERO )
    {
        if ( mSleepStartTime + AseTimeVal::fromMS(mShortSleepForPerfDevice) < now )
        {
            // Has slept enough time
            mSleepStartTime = AseTimeVal::ZERO;
        }
        else
            return AS_PENDING;
    }

    if ( mStreamingSpeed == 0 )
    {
        ++mSleepCounter;
        return AS_PENDING;
    }

    if ( (int32_t) downloadTrackId != mVideoDlTrackId && (int32_t) downloadTrackId != mAudioDlTrackId )
        return AS_PENDING;

    for ( vector<MediaTrackPtr>::const_iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        if ( (*iter)->mediaType() == MEDIA_VIDEO)
        {
            if ( mVideoTrackDownloaderPaused || (int32_t) downloadTrackId != mVideoDlTrackId )
                continue;
        }
        else
        {
            if ( (int32_t) downloadTrackId != mAudioDlTrackId )
                continue;
        }

        streamingTime = (*iter)->streamingTime();
        // We are downloading Audio/Video in balance
        if ( (streamingTime == AseTimeStamp::INFINITE) ||
                (streamingTime <= minStreamingTime + AseTimeStamp(mMaxStreamingSkew, 1000)) )
        {
            retCode = (*iter)->getNextRequest(pRequest);
            if ( retCode == AS_NO_ERROR )
            {
                pMediaTrack = (*iter);
                break;
            }
        }
    }

    bool const needsToMigrate = (mLicenseAcquired && mFastStart);
    if ( (retCode != AS_NO_ERROR) && needsToMigrate )
    {
        for ( vector<MediaTrackPtr>::const_iterator iter = mMediaTracks.begin();
                iter != mMediaTracks.end();
                ++iter )
        {
            if ( (*iter)->mediaType() == MEDIA_VIDEO && (int32_t) downloadTrackId != mVideoDlTrackId )
                continue;

            if ( (*iter)->mediaType() == MEDIA_AUDIO && (int32_t) downloadTrackId != mAudioDlTrackId )
                continue;

            retCode = (*iter)->getNextRequest(pRequest);
            if (retCode == AS_NO_ERROR)
            {
                pMediaTrack = (*iter);
                break;
            }
        }
    }


    if ( retCode == AS_NO_ERROR )
    {
        if (mFastStart)
        {
            NTRACE(TRACE_STREAMINGSTAT,
                "media request isHeader: %u, streaming time: %llu ms "
                "media type: %u, consumer state: %u",
                pRequest->headersRequest(), pMediaTrack->streamingTime().getValueInMs(),
                pMediaTrack->mediaType(), mMediaConsumerState);
        }

        if ( pRequest->headersRequest() && pMediaTrack->streamingTime() != AseTimeStamp::INFINITE )
        {
            if ( (pMediaTrack->mediaType() == MEDIA_VIDEO) ||
                    (mMediaConsumerState == MEDIA_CONSUMER_BUFFERING_STATE) )
            {
                // for audio header, only use the header track downloader in buffering
                pRequest->setDownloadTrackId( mHeadersDlTrackId );
            }
        }
        NTRACE(TRACE_STREAMINGSTAT,
                "create a media request track id: %u, track downloader id: %u, "
                "byte range: %lld-%lld",
                pMediaTrack->trackId(), /*pRequest->getStreamKey().c_str(),*/ pRequest->getDownloadTrackId(),
                pRequest->getByteRange().getStart(), pRequest->getByteRange().getEnd());
        pRequest->setLocationSelector(mLocationSelectorPtr);
    }

    pMediaRequest = pRequest;

    return retCode;

}

void MediaPresentation::getBufferStats(
        MediaType mediaType,
        ullong& usedBuffer, ullong& availableBuffer )
{
    availableBuffer = mMediaBufferPoolPtr->getUnitSize() *
                            mMediaBufferPoolPtr->getFreeUnitCount(mediaType);

    if ( mediaType == MEDIA_VIDEO )
        usedBuffer = mTotalMediaBufferSize - availableBuffer;
    else
        usedBuffer = 0;
    return;
}

void MediaPresentation::getMediaPlaybackState(
                                uint32_t& playableLength,
                                bool& goodQuality )
{
    playableLength = 0;
    goodQuality = false;
    if ( mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE )
        return;

    MediaTrackPtr pVideoTrack = obtainMediaTrack(MEDIA_VIDEO);
    MediaTrackPtr pAudioTrack = obtainMediaTrack(MEDIA_AUDIO);
    uint32_t audioBufferLen, pendingAudioBufferLen;
    uint32_t videoBufferLen, pendingVideoBufferLen;
    pVideoTrack->obtainBufferingLen(videoBufferLen, pendingVideoBufferLen);
    pAudioTrack->obtainBufferingLen(audioBufferLen, pendingAudioBufferLen);

    playableLength = (videoBufferLen > audioBufferLen) ? audioBufferLen : videoBufferLen;

    mStreamSelectorPtr->getStreamingQuality(goodQuality);
    return;
}

uint32_t MediaPresentation::getPlayableLen( MediaType mediaType )
{
    if ( mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE )
        return 0;

    MediaTrackPtr pMediaTrack = obtainMediaTrack(mediaType);
    uint32_t completedBuffer, pendingBuffer;
    pMediaTrack->obtainBufferingLen(completedBuffer, pendingBuffer);
    return completedBuffer;
}

void MediaPresentation::reportFailure(AseErrorCode errorCode, string const& message)
{
    DEBUGLOG(MediaPresentationLog,
            "reportFailure: error code: %d, state: %u",
            errorCode, mMediaConsumerState);

    if ( mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE )
        return;
    reportFailure(errorCode, 200, message);
}


void MediaPresentation::reportFailure(AseErrorCode errorCode, int32_t httpErr, string const& message)
{
    DEBUGLOG(MediaPresentationLog,
            "reportFailure: error code: %d, state: %u",
            errorCode, mMediaConsumerState);

    if ( mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE )
        return;

    ManifestPtr pCurrentManifest = mManifestPtrs.front();
    reportFailure(errorCode, pCurrentManifest->getManifestIndex(),
                  httpErr, message);

    return;
}

void
MediaPresentation::reportFailure(AseErrorCode errorCode, int32_t manifestIndex,
                                 int32_t httpErr, string const& message)
{
    DEBUGLOG(MediaPresentationLog,
            "reportFailure: error code: %d, state: %u",
            errorCode, mMediaConsumerState);

    if ( mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE )
        return;
    bool prevState = mFailed;
    mFailed = true;
    if ( !prevState )
    {
        // only report the error once
        // TODO: report broken stream id from Media Track when it receives the error report
        int32_t brokenStreamId = -1;
#if 0
        if ( mCurrentMediaTrack.get() )
            brokenStreamId = mCurrentMediaTrack->brokenStreamId();
#endif

        mStreamingReporterPtr->
            streamingFailure(manifestIndex,
                             /**TODO mCurrentTrackId*/ 0, brokenStreamId, errorCode, httpErr, message);

    }
}

void MediaPresentation::pipeliningDisabled()
{
    // Need to implement: update our stored state to say that pipelining is disabled
    DEBUGLOG(MediaPresentationLog,"pipeliningDisabled");

    mStreamingReporterPtr->pipeliningDisabled();
}

bool MediaPresentation::isBufferAvailable(MediaType mediaType)
{
    if ( mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE )
        return true;

    updatePlaybackTime();
    if ( mInternalStreamingState == DOWNLOADING )
    {
        if ( (mAccumulatedBytesDownloaded >= mMinDownloadSizeAtResume)
                && (tooMuchMediaBuffer(mediaType)) )
        {
            DEBUGLOG(MediaPresentationLog, "too much media buffer:%llu, draining",
                                mAccumulatedBytesDownloaded);
            mInternalStreamingState = DRAINING;
            mAccumulatedBytesDownloaded = 0;
        }
    }
    else if ( mInternalStreamingState == DRAINING )
    {
        ullong usedBuffer, availableVideoBuffer, availableAudioBuffer;
        getBufferStats(MEDIA_VIDEO, usedBuffer, availableVideoBuffer);
        getBufferStats(MEDIA_AUDIO, usedBuffer, availableAudioBuffer);

        // normal streaming mode
        if ( !tooMuchMediaBuffer(mediaType) )
        {
            if ( (mediaType == MEDIA_AUDIO) && (availableAudioBuffer >= mAudioBufferHighTargetAvailableSize) )
            {
                DEBUGLOG(MediaPresentationLog, "have drained enough for audio, downloading");
                mInternalStreamingState = DOWNLOADING;
            }

            if ( (mediaType == MEDIA_VIDEO) && (availableVideoBuffer >= mMediaBufferHighTargetAvailableSize) )
            {
                DEBUGLOG(MediaPresentationLog, "have drained enough for video, downloading");
                mInternalStreamingState = DOWNLOADING;
            }
        }
    }


    if ( isBufferingState() && (mInternalStreamingState == DRAINING) )
    {
        bufferingComplete("bufferFull");
    }

    if ( mInternalStreamingState == DOWNLOADING )
        return true;
    else
        return false;
}

void MediaPresentation::networkFailing( uint32_t                downloadTrackId,
                                        AseErrorCode            latestStatus,
                                        AseUrl::HostPort const& destination,
                                        AseTimeVal const*       pLatestErrorTime,
                                        AseTimeVal const&       latestDataReceived )
{
    (void)downloadTrackId;
    DEBUGLOG(MediaPresentationLog,"networkFailing( %d, %d, %s:%u, %lldms ago, %llums ago )",
        downloadTrackId,
        latestStatus,
        destination.first.c_str(),
        destination.second,
        pLatestErrorTime ? ( AseTimeVal::now() - *pLatestErrorTime ).ms() : -1,
        ( AseTimeVal::now() - latestDataReceived ).ms() );

    mLocationSelectorPtr->networkFailing(   latestStatus,
                                            destination,
                                            pLatestErrorTime,
                                            latestDataReceived );

}

/**             IMediaConsumer API implementation                        */
/** -------------------------------------------------------------------- */




/** -------------------------------------------------------------------- */
/**             Streaming Commands Implementation                        */
AseErrorCode MediaPresentation::runSwitchTracksCommand(const vector<uint32_t>& trackIndexes)
{
    AseErrorCode retCode = AS_NO_ERROR;

    DEBUGLOG(MediaPresentationLog, "runSwitchTracksCommand");

    retCode = switchMediaTracks(trackIndexes);

    if ( (retCode == AS_NO_ERROR) && (mMediaConsumerState != MEDIA_CONSUMER_STOPPED_STATE) )
    {
        MediaTrackPtr pVideoTrack = obtainMediaTrack(MEDIA_VIDEO);
        MediaTrackPtr pAudioTrack = obtainMediaTrack(MEDIA_AUDIO);
        AseTimeStamp playbackTime = pVideoTrack->playbackTime();
        AseTimeStamp TwoSeconds = AseTimeStamp(2000, 1000);
        if ( playbackTime > TwoSeconds )
            playbackTime -= TwoSeconds;
        else
            playbackTime = AseTimeStamp::ZERO;

        DEBUGLOG( MediaPresentationLog,
                "set audio track's streaming time to be %llu ms",
                playbackTime.getValueInMs() );
        pAudioTrack->startSwitch(playbackTime);
        // flush audio data in media sink
        mMediaSinkPtr->flushMedia(MEDIA_AUDIO);
        DEBUGLOG( MediaPresentationLog, "On-the-fly audio track switch: flush media sink, "
                "time=%llu ms", AseTimeVal::now().ms());
        mStreamingReporterPtr->flushComplete(MEDIA_AUDIO);
        ullong usedBuffer, availableVideoBuffer, availableAudioBuffer;
        getBufferStats(MEDIA_VIDEO, usedBuffer, availableVideoBuffer);
        getBufferStats(MEDIA_AUDIO, usedBuffer, availableAudioBuffer);
        DEBUGLOG( MediaPresentationLog,
                "available a/v buffer: (%llu, %llu) bytes ",
                availableAudioBuffer, availableVideoBuffer );
        // resume giving request to UrlRequestManager
        mPaused = false;
        mInternalStreamingState = DOWNLOADING;
        DEBUGLOG(MediaPresentationLog, "On-the-fly audio track switch: resume giving out new requests, "
                "time=%llu ms", AseTimeVal::now().ms());
        mVideoTrackDownloaderPaused = true;

    }

    if ( (retCode == AS_NO_ERROR) &&
            mFastStart && (mManifestPtrs.size() > 1) )
    {
        retCode = scheduleMigrate(mManifestPtrs[1], MEDIA_AUDIO);
        if ( retCode != AS_NO_ERROR )
        {
            reportFailure(retCode, "fail to schedule audio migration");
        }
    }


    return retCode;
}

AseErrorCode MediaPresentation::runSetSeekTimeCommand(AseTimeStamp const& seekTime)
{
    if ( mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE )
        return AS_NOT_ALlOWED;

    mSeekTime = seekTime;
    mActualStartPts = mSeekTime;

    AseErrorCode retCode = AS_NO_ERROR;
    if ( !mFastStart )
        retCode = checkStreamDuration();

    if ( retCode == AS_NO_ERROR )
    {
        retCode = updateStreamingTime(mSeekTime);
    }
    else
    {
        reportFailure(retCode, "");
    }

    if ( retCode == AS_NO_ERROR )
    {
        mPrebufferingStartTime = AseTimeVal::now().ms();
        DEBUGLOG(MediaPresentationLog, "set seek time: %llu ms", seekTime.getValueInMs());
        mStreamingReporterPtr->bufferingStarted();
    }
    else
    {
        mUnfinishedStreamingCommandPtr.reset(new SetSeekTimeCommand(static_pointer_cast<MediaPresentation>(shared_from_this()), seekTime));
    }

    return retCode;
}

AseErrorCode MediaPresentation::runRebufferCommand()
{
    if ( mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE )
        return AS_NOT_ALlOWED;

    mMediaConsumerState = MEDIA_CONSUMER_REBUFFERING_STATE;
    mLocationSelectorPtr->notifyMediaConsumerState( mMediaConsumerState );
    mPrebufferingStartTime = AseTimeVal::now().ms();
    mStreamingReporterPtr->bufferingStarted();
    mStreamSelectorPtr->stop();
    mStreamingManagerPtr.lock()->resumeVideoTrackDownloader();
    mVideoTrackDownloaderPaused = false;

    double playbackRate;
    AseTimeStamp playbackTime;
    mMediaSinkPtr->currentPlaybackInfo( playbackRate, playbackTime );
    mActualStartPts = playbackTime;

    for ( vector<MediaTrackPtr>::iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        (*iter)->setNumNonEmptyFragments(0);
    }

    return AS_NO_ERROR;
}

AseErrorCode MediaPresentation::runSkipCommand()
{
    if ( mMediaConsumerState == MEDIA_CONSUMER_STOPPED_STATE )
        return AS_NOT_ALlOWED;
    mStreamSelectorPtr->skip();
    return AS_NO_ERROR;
}

AseErrorCode MediaPresentation::scheduleMigrate(ManifestPtr pManifest, MediaType mediaType)
{
    MediaTrackPtr pMediaTrack = obtainMediaTrack(mediaType);

    vector<ManifestTrackPtr> const& manifestTracks = pManifest->getTracks();
    // migrate video media track
    uint32_t trackId;
    ManifestTrackPtr pCurrentManifestTrack = pMediaTrack->getManifestTrack();
    ManifestTrackPtr pManifestTrack =
                        obtainManifestTrack(mediaType, manifestTracks,
                                            pCurrentManifestTrack, trackId);

    AseErrorCode retCode = AS_NO_ERROR;

    if ( pManifestTrack == ManifestTrackPtr() )
        retCode = AS_NOT_FOUND;
    else
        retCode = pMediaTrack->scheduleMigrate(pManifestTrack, pManifest->getManifestIndex(), trackId);

    return retCode;
}

AseErrorCode MediaPresentation::runAddManifestCommand(ManifestPtr pManifest)
{
    mManifestPtrs.push_back(pManifest);
    AseErrorCode retCode = AS_NO_ERROR;
    DEBUGLOG(MediaPresentationLog, "runAddManifestCommand");

    retCode = scheduleMigrate(pManifest, MEDIA_VIDEO);
    if ( retCode != AS_NO_ERROR )
    {
        reportFailure(retCode, "fail to schedule video migration");
        return retCode;
    }
    retCode = scheduleMigrate(pManifest, MEDIA_AUDIO);
    if ( retCode != AS_NO_ERROR )
    {
        reportFailure(retCode, "fail to schedule audio migration");
        return retCode;
    }

    return retCode;
}

AseErrorCode MediaPresentation::runNotifyLicenseAcquiredCommand()
{
    for ( vector<MediaTrackPtr>::iterator iter = mMediaTracks.begin();
                    iter != mMediaTracks.end();
                    ++iter )
    {
        (*iter)->notifyLicenseAcquired();
    }

    mLicenseAcquired = true;

    return AS_NO_ERROR;
}

AseErrorCode MediaPresentation::runSetVideoBitrateRangeCommand
            (vector<shared_ptr<BitrateRange> > const& videoBitrateRanges)
{
    MediaTrackPtr pVideoTrack = obtainMediaTrack(MEDIA_VIDEO);
    pVideoTrack->setVideoBitrateRanges(videoBitrateRanges);
    mStreamingReporterPtr->videoBitrateSelectionCompleted();
    return AS_NO_ERROR;
}

AseErrorCode MediaPresentation::runSetStreamingBufferCommand
            (uint32_t maxBufferLen,
            uint32_t minBufferLen)
{
    if ( (maxBufferLen < mLowWatermarkLevel) && (maxBufferLen > 0) )
        maxBufferLen = mLowWatermarkLevel;
    if ( (minBufferLen < mLowWatermarkLevel) && (minBufferLen > 0) )
        minBufferLen = mLowWatermarkLevel;

    if ( maxBufferLen > 0 )
        mMaxMediaBufferLenConfiged = maxBufferLen;
    if ( minBufferLen > 0 )
        mMinMediaBufferLen = minBufferLen;

    if ( mFastStart )
    {
        mEffectiveMaxMediaBufferLen =
            min( min(mMaxMediaBufferLenConfiged, mMaxMediaBufferLenAllowed), mMaxMediaBufferLenAllowedInFastplay);
    }
    else
    {
        mEffectiveMaxMediaBufferLen = min(mMaxMediaBufferLenConfiged, mMaxMediaBufferLenAllowed);
    }

    DEBUGLOG(MediaPresentationLog, "setStreamingBuffer: (%u, %u) ms",
            mMinMediaBufferLen, mEffectiveMaxMediaBufferLen);

    return AS_NO_ERROR;
}

AseErrorCode MediaPresentation::runUpdateDownloaderTrackIdCommand()
{
    MediaTrackPtr pVideoTrack = obtainMediaTrack(MEDIA_VIDEO);
    MediaTrackPtr pAudioTrack = obtainMediaTrack(MEDIA_AUDIO);
    pVideoTrack->setDownloadTrackId(mVideoDlTrackId);
    pAudioTrack->setDownloadTrackId(mAudioDlTrackId);
    DEBUGLOG(MediaPresentationLog,
            "update track downloader ids: audio:%u, video:%u",
            mAudioDlTrackId, mVideoDlTrackId);
    return AS_NO_ERROR;
}
/**             Streaming Commands Implementation                        */
/** -------------------------------------------------------------------- */

/** -------------------------------------------------------------------- */
/**             Misc Public APIs                                         */
void MediaPresentation::bufferFull()
{
    if (isBufferingState())
    {
        bufferingComplete("bufferFull");
    }
    mInternalStreamingState = DRAINING;
    mAccumulatedBytesDownloaded = 0;
    DEBUGLOG(MediaPresentationLog, "buffer full, draining");
    return;
}

void MediaPresentation::updateDataReception(uint32_t size)
{
    mAccumulatedBytesDownloaded += size;
}

bool MediaPresentation::isBufferingState() const
{
    if ( (mMediaConsumerState == MEDIA_CONSUMER_BUFFERING_STATE) ||
            (mMediaConsumerState == MEDIA_CONSUMER_REBUFFERING_STATE) )
        return true;
    else
        return false;
}

void MediaPresentation::reportNewStreamDuration(const string& streamKey, uint32_t duration)
{
    DEBUGLOG(MediaPresentationLog,
            "reportNewStreamDuration: stream key: %s, duration: %u",
             streamKey.c_str(), duration);

    if ( mStreamDurations.empty() || mFastStart )
    {
        // disable the duration check during fast start
        mStreamDurations[streamKey] = duration;
        return;
    }

    if ( mSeekTime == AseTimeStamp::INFINITE )
        return;

    bool durationError = false;
    ostringstream ostream;
    ostream.imbue(std::locale::classic());

    for ( map<string, uint32_t>::iterator iter = mStreamDurations.begin();
            iter != mStreamDurations.end();
            ++iter )
    {
        uint32_t currentDuration = (*iter).second;
        if ( (mSeekTime.getValueInMs() > duration) || (mSeekTime.getValueInMs() > currentDuration) )
        {
            if (duration + MAX_DURATION_MISMATCH < currentDuration)
            {
                ostream << "stream with key: " << streamKey << ", duration: " << duration
                        << " ms too short, seek time: " << mSeekTime.getValueInMs() << " ms";
                durationError = true;
                break;
            }
            else if (currentDuration + MAX_DURATION_MISMATCH < duration)
            {
                ostream << "stream with key: " << ((*iter).first) << ", duration: " << currentDuration
                        << " ms too short, seek time: " << mSeekTime.getValueInMs() << " ms";
                durationError = true;
                break;
            }
        }
    }

    if ( durationError )
    {
        DEBUGLOG(MediaPresentationLog, "%s", ostream.str().c_str());
        reportFailure(AS_MEDIA_DURATION_ERROR, ostream.str());
    }
    else
    {
        mStreamDurations[streamKey] = duration;
    }
    return;
}

bool
MediaPresentation::finalManifest()
{
    ScopedMutex scopedMutexResource(mCmdMutex);
    ManifestPtr pManifest = mManifestPtrs.front();
    return pManifest->getFinalPeriod();
}

void
MediaPresentation::updateContentDuration(uint32_t duration)
{
    DEBUGLOG(MediaPresentationLog, "updateContentDuration: %u ms", duration);
    mContentDuration = duration;
    mStreamingReporterPtr->updateContentDuration(duration);
}

void
MediaPresentation::selectStream(int32_t trackId, int32_t& streamId,
                                uint32_t& bandwidth, uint32_t& longtermBw,
                                uint32_t& predictedRebuffer,
                                IStreamSelector::StreamFeasibility& streamFeasibility,
                                int32_t& predictedDlTimeForBackupStream)
{
    double playbackRate;
    AseTimeStamp playbackTime;
    ullong availableBuffer;
    obtainPlaybackStats(playbackRate, playbackTime, availableBuffer);

    MediaTrackPtr pVideoTrack = obtainMediaTrack(MEDIA_VIDEO);
    bool bitrateSwitchFreeze = (mLocationSelectorPtr->bitrateSwitchFreeze() && !pVideoTrack->probingStream());
    int32_t numOfBlocks;
    mBufferingStats.mBufferingStatus  = mStreamSelectorPtr->selectStream( bitrateSwitchFreeze,
                                                          trackId,
                                                          playbackRate, playbackTime,
                                                          mTrackCurrentInfo,
                                                          availableBuffer,
                                                          mTrackFutureInfo,
                                                          streamFeasibility,
                                                          streamId,
                                                          numOfBlocks,
                                                          predictedDlTimeForBackupStream,
                                                          predictedRebuffer);
    mPredictedRebuffers = predictedRebuffer;
    mStreamSelectorPtr->obtainBufferingStats(mBufferingStats.mRequiredBufferingLen, mBufferingStats.mEstimateBufferingTime);
    double confidence = 0.0;
    string locationSetKey;
    uint32_t currentBufferLen, maxPendingBufferLen;
    obtainMixedBufferLen(currentBufferLen, maxPendingBufferLen);
    uint32_t currentThroughput = obtainCurrentThroughput(currentBufferLen, confidence, locationSetKey);
    bandwidth = currentThroughput;
    longtermBw = currentThroughput;

    return;
}

void MediaPresentation::checkBufferingStatus()
{
    if (!isBufferingState())
        return;

    do
    {
        MediaTrackPtr pVideoTrack = obtainMediaTrack(MEDIA_VIDEO);
        MediaTrackPtr pAudioTrack = obtainMediaTrack(MEDIA_AUDIO);
        if ( pVideoTrack->endOfTrack() && pAudioTrack->endOfTrack() )
        {
            bufferingComplete("endOfMedia");
            break;
        }

        DEBUGLOG(MediaPresentationLog, "buffering status: %u, location selection status: %u",
                mBufferingStats.mBufferingStatus, mLocationSelectorPtr->locationSelectionComplete());

        double confidence = 0.0;
        string locationSetKey;
        uint32_t currentBufferLen, maxPendingBufferLen;
        obtainMixedBufferLen(currentBufferLen, maxPendingBufferLen);
        uint32_t currentThroughput = obtainCurrentThroughput(currentBufferLen, confidence, locationSetKey);

        if ( (mBufferingStats.mBufferingStatus == IStreamSelector::BUFFER_HAVE_ENOUGH)
                && (mLocationSelectorPtr->locationSelectionComplete())
                && (currentBufferLen >= mMinPrebufLength) )
        {
            if ( mFastStart || pAudioTrack->endOfTrack() ||
                     (pAudioTrack->numNonEmptyFragments() >= MIN_NONEMPTY_AUDIO_FRAGMENTS) )
            {
                // add a few extra conditions to work around a/v pts out of sync issue
                bufferingComplete("bufferComplete");
                break;
            }
        }
        else
        {
            // report buffering progress
            uint32_t requiredBufferingLen = mBufferingStats.mRequiredBufferingLen;
            uint32_t estimatedBufferingTime = mBufferingStats.mEstimateBufferingTime;
            if ( (mPredictedBufferingTime <= 0) && (currentThroughput > 0)  )
            {
                mPredictedBufferingTime = estimatedBufferingTime;
                mInitBandwidth = currentThroughput;
            }
            if ( requiredBufferingLen > 0 )
            {
                uint32_t reportBufferLen = currentBufferLen > requiredBufferingLen ?
                                                requiredBufferingLen : currentBufferLen;

                if ( (reportBufferLen >= mPrevBufferLen) &&
                        ((reportBufferLen - mPrevBufferLen) * 100 >= mBufferingProgressUpdateInterval * requiredBufferingLen) )
                {
                    mStreamingReporterPtr->bufferingStatus( requiredBufferingLen, reportBufferLen );
                    mPrevBufferLen = reportBufferLen;
                    DEBUGLOG( MediaPresentationLog, "buffering status: total: %u ms, current: %u ms",
                            requiredBufferingLen, currentBufferLen );
                }
            }
        }

        AseTimeVal now = AseTimeVal::now();
        if ( mPrebufferTimeLimit && mPrebufferingStartTime &&
                (now.ms() >= mPrebufferingStartTime + mPrebufferTimeLimit) )
        {
            DEBUGLOG( MediaPresentationLog, "reach prebuffer time limit");
            if (currentBufferLen > mMinPrebufLength)
            {
                bufferingComplete("bufferComplete-timelimit");
                mPrebufferingStartTime = 0;
                break;
            }
        }
    } while(false);

    return;
}

bool MediaPresentation::audioTrackReadyForMigration() const
{
    MediaTrackPtr pAudioTrack = obtainMediaTrack(MEDIA_AUDIO);
    return pAudioTrack->readyForMigration();
}

void MediaPresentation::videoEndOfTrack()
{
    MediaTrackPtr pAudioTrack = obtainMediaTrack(MEDIA_AUDIO);
    pAudioTrack->videoEndOfTrack();
}

AseErrorCode MediaPresentation::migrate()
{
    MediaTrackPtr pAudioTrack = obtainMediaTrack(MEDIA_AUDIO);
    AseErrorCode migrateResult = pAudioTrack->migrate();
    if ( migrateResult != AS_NO_ERROR )
    {
        return migrateResult;
    }

    mManifestPtrs.pop_front();
    ManifestPtr pCurrentManifest = mManifestPtrs.front();
    mFastStart = false;
    mEffectiveMaxMediaBufferLen = min(mMaxMediaBufferLenConfiged, mMaxMediaBufferLenAllowed);
    shared_ptr<StreamingManager> pStreamingManager = mStreamingManagerPtr.lock();
    assert(pStreamingManager != shared_ptr<StreamingManager>());
    pStreamingManager->notifyManifestSelected(pCurrentManifest->getManifestIndex());
    mStreamingReporterPtr->notifyManifestSelected(pCurrentManifest->getManifestIndex());

    mStreamDurations.clear();
    DEBUGLOG(MediaPresentationLog, "switch manifests completed");
    return AS_NO_ERROR;
}

void MediaPresentation::performStreamingTasks()
{
    mMediaSinkPtr->updateStreamingBuffer();
}

/**             Misc Public APIs                                         */
/** -------------------------------------------------------------------- */

/** -------------------------------------------------------------------- */
/**             Internal helper functions                                */
AseErrorCode MediaPresentation::constructMediaTracks( vector<uint32_t> const& trackIndexes )
{
    // parse current mainfest period to construct active media tracks
    ManifestPtr pManifest = mManifestPtrs.front();
    int32_t manifestId = pManifest->getManifestIndex();
    vector<ManifestTrackPtr> const& manifestTracks = pManifest->getTracks();
    ManifestTrackPtr pManifestTrack;
    AseErrorCode retCode;
    MediaTrackPtr pMediaTrack;

    mMediaTracks.clear();
    for ( vector<uint32_t>::const_iterator iter = trackIndexes.begin();
            iter != trackIndexes.end();
            ++iter )
    {
        pManifestTrack = manifestTracks[*iter];

        retCode =
            MediaTrack::construct( static_pointer_cast<MediaPresentation>(shared_from_this()), pManifestTrack,
                                   mFastStart, mMediaSinkPtr, manifestId, *iter,
                                   pManifestTrack->getMediaType() == MEDIA_VIDEO ? mVideoDlTrackId : mAudioDlTrackId,
                                   mMediaBufferPoolPtr,
                                   mStreamingReporterPtr,mLocationSelectorPtr,
                                   mConfigParamterPtr, mLocalConfigParametePtr,
                                   mPreExistingMediaFiles,
                                   pMediaTrack );
        if ( retCode == AS_NO_ERROR )
        {
            mMediaTracks.push_back(pMediaTrack);
        }
        else
        {
            return retCode;
        }
    }

    // create track buffer info and track info list
    mTrackCurrentInfo.clear();
    mTrackFutureInfo.clear();
    for ( vector<MediaTrackPtr>::iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        mTrackFutureInfo.push_back(*iter);
        mTrackCurrentInfo.push_back( (*iter)->trackBufferInfo() );
    }
    return AS_NO_ERROR;
}

AseErrorCode MediaPresentation::switchMediaTracks( vector<uint32_t> const& trackIndexes)
{
    DEBUGLOG(MediaPresentationLog, "switchMediaTracks");

    ManifestPtr pManifest = mManifestPtrs.front();
    int32_t manifestId = pManifest->getManifestIndex();
    vector<ManifestTrackPtr> const& manifestTracks = pManifest->getTracks();
    std::vector<MediaTrackPtr> prevMediaTracks;
    prevMediaTracks = mMediaTracks;
    ManifestTrackPtr pManifestTrack;
    AseErrorCode retCode = AS_NO_ERROR;

    mMediaTracks.clear();
    for ( vector<uint32_t>::const_iterator iter = trackIndexes.begin();
          iter != trackIndexes.end();
          ++iter )
    {
        MediaTrackPtr pMediaTrack = MediaTrackPtr();
        DEBUGLOG(MediaPresentationLog, "switchMediaTracks: track index=%d", *iter);
        for ( vector<MediaTrackPtr>::iterator trackIter = prevMediaTracks.begin();
              trackIter != prevMediaTracks.end();
              ++trackIter )
        {
            if ( (*trackIter)->trackId() == (int32_t)(*iter) )
            {
                pMediaTrack = *trackIter;
                break;
            }
        }
        if ( pMediaTrack )
        {
            DEBUGLOG(MediaPresentationLog, "switchMediaTracks: ... existing track, type %u, dnld id %u",
                     pMediaTrack->mediaType(),
                     pMediaTrack->mediaType() == MEDIA_VIDEO ? mVideoDlTrackId : mAudioDlTrackId );

            mMediaTracks.push_back(pMediaTrack);

            pMediaTrack->setDownloadTrackId( pMediaTrack->mediaType() == MEDIA_VIDEO ? mVideoDlTrackId : mAudioDlTrackId );
        }
        else
        {
            pManifestTrack = manifestTracks[*iter];

            retCode =
                MediaTrack::construct( static_pointer_cast<MediaPresentation>(shared_from_this()), pManifestTrack,
                                       mFastStart, mMediaSinkPtr, manifestId, *iter,
                                       pManifestTrack->getMediaType() == MEDIA_VIDEO ? mVideoDlTrackId : mAudioDlTrackId,
                                       mMediaBufferPoolPtr, mStreamingReporterPtr, mLocationSelectorPtr,
                                       mConfigParamterPtr, mLocalConfigParametePtr,
                                       mPreExistingMediaFiles,
                                       pMediaTrack );
            if ( retCode == AS_NO_ERROR )
            {
                DEBUGLOG(MediaPresentationLog, "switchMediaTracks: ... new track, type %u, dnld id %u",
                         pMediaTrack->mediaType(),
                         pMediaTrack->mediaType() == MEDIA_VIDEO ? mVideoDlTrackId : mAudioDlTrackId );
                if (pMediaTrack->mediaType() == MEDIA_VIDEO)
                {
                    retCode = AS_NOT_ALlOWED;
                    assert(0);
                    break;
                }
                if (mLicenseAcquired)
                    pMediaTrack->notifyLicenseAcquired();
                mMediaTracks.push_back(pMediaTrack);
            }
            else
            {
                break;
            }
        }
    }

    if ( retCode == AS_NO_ERROR)
    {
        // create track buffer info and track info list
        mTrackCurrentInfo.clear();
        mTrackFutureInfo.clear();
        for ( vector<MediaTrackPtr>::iterator iter = mMediaTracks.begin();
              iter != mMediaTracks.end();
              ++iter )
        {
            mTrackFutureInfo.push_back(*iter);
            mTrackCurrentInfo.push_back( (*iter)->trackBufferInfo() );
        }
    }
    else
    {
        mMediaTracks = prevMediaTracks;
    }
    return retCode;

}


MediaTrackPtr MediaPresentation::obtainMediaTrack( MediaType mediaType ) const
{
    MediaTrackPtr pMediaTrack = MediaTrackPtr();
    for ( vector<MediaTrackPtr>::const_iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        if ( (*iter)->mediaType() == mediaType )
        {
            pMediaTrack = *iter;
            break;
        }
    }
    return pMediaTrack;
}

ManifestTrackPtr
MediaPresentation::obtainManifestTrack(MediaType mediaType,
                                       vector<ManifestTrackPtr> const& manifestTracks,
                                       ManifestTrackPtr pCurrentManifestTrack,
                                       uint32_t& trackId)
{
    DEBUGLOG(MediaPresentationLog, "obtainManifestTrack: media type: %u, current track key: %s",
            mediaType, pCurrentManifestTrack->getTrackKey().c_str());
    ManifestTrackPtr pManifestTrack = ManifestTrackPtr();

    for ( uint32_t i = 0; i < manifestTracks.size(); ++i )
    {
        DEBUGLOG(MediaPresentationLog, "obtainManifestTrack: new track media type: %u, track key: %s",
                mediaType, manifestTracks[i]->getTrackKey().c_str());
        if ( (manifestTracks[i]->getMediaType() == mediaType) &&
                (manifestTracks[i]->getTrackKey() == pCurrentManifestTrack->getTrackKey()) &&
                 compatibleManifestTracks(manifestTracks[i], pCurrentManifestTrack) )
        {
            DEBUGLOG(MediaPresentationLog, "obtainManifestTrack: media type: %u, track key: %s",
                        mediaType, manifestTracks[i]->getTrackKey().c_str());
            pManifestTrack = manifestTracks[i];
            trackId = i;
            break;
        }
    }
    return pManifestTrack;
}


bool
MediaPresentation::compatibleManifestTracks(ManifestTrackPtr pManifestTrack1, ManifestTrackPtr pManifestTrack2)
{
    if ( pManifestTrack1->getMediaType() != pManifestTrack2->getMediaType() )
        return false;

    bool compatible = false;
    if ( pManifestTrack1->getMediaType() == MEDIA_VIDEO )
    {
        if ( pManifestTrack1->isStereoVideo() == pManifestTrack2->isStereoVideo() )
        {
            compatible = true;
        }
    }
    else if ( pManifestTrack1->getMediaType() == MEDIA_AUDIO )
    {
        ManifestStreamPtr pManifestStream1, pManifestStream2;
        pManifestStream1 = pManifestTrack1->getStreams()[0];
        pManifestStream2 = pManifestTrack2->getStreams()[0];
        DEBUGLOG(MediaPresentationLog,
                "compatibility test: language: %s:%s,"
                "num of channels: %d:%d",
                pManifestTrack1->getLanguage().c_str(), pManifestTrack2->getLanguage().c_str(),
                pManifestStream1->getAudioChannels()[0], pManifestStream2->getAudioChannels()[0]);
        compatible = true;
    }

    return compatible;
}


AseErrorCode MediaPresentation::checkStreamDuration()
{
    if ( mStreamDurations.empty() )
    {
        return AS_NO_ERROR;
    }
    DEBUGLOG(MediaPresentationLog, "checkStreamDuration");

    bool durationError = false;
    for ( map<string, uint32_t>::iterator iter = mStreamDurations.begin();
            iter != mStreamDurations.end();
            ++iter )
    {
        uint32_t currentDuration = (*iter).second;
        if (mSeekTime.getValueInMs() > currentDuration + MAX_DURATION_MISMATCH)
        {
            DEBUGLOG(MediaPresentationLog,
                    "stream duration error: seek time: %llu ms, current stream's duration: %u",
                    mSeekTime.getValueInMs(), currentDuration);
            durationError = true;
            break;
        }
        else
        {
            DEBUGLOG(MediaPresentationLog,
                    "stream duration, seek time: %llu ms, current stream's duration: %u",
                    mSeekTime.getValueInMs(), currentDuration);
        }
    }
    if ( durationError )
    {
        mFailed = true;
        DEBUGLOG(MediaPresentationLog, "return AS_MEDIA_DURATION_ERROR");
        return AS_MEDIA_DURATION_ERROR;
    }
    else
    {
        return AS_NO_ERROR;
    }
}


AseErrorCode MediaPresentation::updateStreamingTime(AseTimeStamp const& seekTime)
{
    // pick the video track
    MediaTrackPtr pVideoTrack = obtainMediaTrack(MEDIA_VIDEO);

    AseTimeStamp videoStreamingTime;

    AseErrorCode retCode = pVideoTrack->setStreamingTime(seekTime, videoStreamingTime);
    if ( retCode != AS_NO_ERROR )
        return retCode;

    videoStreamingTime.changeTimescale(1000);
    mActualStartPts = videoStreamingTime;
    DEBUGLOG(MediaPresentationLog, "updateStreamingTime: video streaming time: %llu ms",
            videoStreamingTime.getValueInMs());
    //printf("seek time: %llu ms, video streaming time: %llu ms\n",
      //  seekTime.getValueInMs(), videoStreamingTime.getValueInMs() );
    AseTimeStamp streamingTime;
    for ( vector<MediaTrackPtr>::iterator iter = mMediaTracks.begin();
                    iter != mMediaTracks.end();
                    ++iter )
    {
        // audio track's time is earlier than video track's
        (*iter)->setStreamingTime(videoStreamingTime, streamingTime);
        (*iter)->updatePlaybackTime(videoStreamingTime);
        if ( (*iter)->mediaType() == MEDIA_AUDIO )
        {
            DEBUGLOG(MediaPresentationLog, "updateStreamingTime: audio streaming time: %llu ms",
                    streamingTime.getValueInMs());
        }

    }
    return AS_NO_ERROR;
}

void MediaPresentation::updatePlaybackTime()
{
    // obtain the playback time/rate
    double playbackRate;
    AseTimeStamp playbackTime;
    mMediaSinkPtr->currentPlaybackInfo( playbackRate, playbackTime );

    for ( vector<MediaTrackPtr>::iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        if ( playbackTime < mActualStartPts )
            playbackTime = mActualStartPts;
        (*iter)->updatePlaybackTime(playbackTime);
    }
}

void MediaPresentation::bufferingComplete(string const& reason)
{
#ifndef BUILD_DEBUG
    NRDP_UNUSED(reason);
#endif
    uint32_t audioBufferLen, videoBufferLen;
    obtainBufferLen(audioBufferLen, videoBufferLen);
    double confidence = 0.0;
    string locationSetKey;
    if ( (audioBufferLen > 0) && (videoBufferLen > 0) )
    {
        statusUpdate();
        NTRACE( TRACE_STREAMINGSTAT,
                "buffering complete, reason: %s", reason.c_str() );
        mStreamingReporterPtr->
            bufferingComplete(mActualStartPts.getValueInMs(),
                              mPredictedBufferingTime, mInitBandwidth,
                              obtainCurrentThroughput(videoBufferLen, confidence, locationSetKey));
        if (mPauseStreamingAfterBufferingComplete)
            setStreamingSpeed(0);
        mMediaConsumerState = MEDIA_CONSUMER_PLAYING_STATE;
        mLocationSelectorPtr->notifyMediaConsumerState( mMediaConsumerState );
    }
    return;
}

void MediaPresentation::statusUpdate(bool enableTraceLogging)
{
    double confidence = 0.0;
    string locationSetKey;
    uint32_t currentBufferLen, maxPendingBufferLen;
    obtainMixedBufferLen(currentBufferLen, maxPendingBufferLen);
    uint32_t currentThroughput = obtainCurrentThroughput(currentBufferLen, confidence, locationSetKey);
    uint32_t avgConnectTime, varConnectTime, numberOfConnectionFailures;
    ILocationSetMonitorPtr pLocationSetMonitor = mNetworkMonitorPtr->getCurrentLocationSetMonitor();
    pLocationSetMonitor->connectionStat(avgConnectTime, varConnectTime, numberOfConnectionFailures);

    if ( enableTraceLogging )
    {
        uint32_t streamingDiff = obtainBufferingDifference();
        NTRACE(TRACE_STREAMINGSTAT, "status update playing %llu ms, "
                                    "buffering status: %u, location selection status: %u, "
                                    "current bandwidth: (%u, %u) kbps, "
                                    "confidence: %f, location set key: %s "
                                    "streaming time diff: %u ms ",
                                    mMediaTracks[0]->playbackTime().getValueInMs(),
                                    mBufferingStats.mBufferingStatus, mLocationSelectorPtr->locationSelectionComplete(),
                                    currentThroughput, currentThroughput,
                                    confidence,
                                    locationSetKey.c_str(),
                                    streamingDiff);
        NRDP_UNUSED(streamingDiff);
        PERFTRACE("status update playing %llu ms, "
                  "current bandwidth: (%u, %u) kbps\n",
                  mMediaTracks[0]->playbackTime().getValueInMs(),
                  currentThroughput, currentThroughput);
    }

    shared_ptr<StreamingStatReport> pStreamingStatReport(new StreamingStatReport);
    pStreamingStatReport->mPlaybackTimeInMs = mMediaTracks[0]->playbackTime().getValueInMs();
    shared_ptr<LocationStat>
        pLocationStat(new LocationStat(currentThroughput, confidence, avgConnectTime, locationSetKey));
    pStreamingStatReport->mCurrentLocationStatPtr = pLocationStat;
    pStreamingStatReport->mStreamingMediaStatList.clear();

    mStreamingStatPtr->updatePlaybackPts(mMediaTracks[0]->playbackTime());
    uint32_t abuflbytes = 0;
    uint32_t vbuflbytes = 0;
    shared_ptr<StreamingMediaStat> pStreamingMediaStat;
    for (vector<MediaTrackPtr>::iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        MediaTrackPtr pMediaTrack = *iter;
        uint32_t completedBuffer, pendingBuffer;
        pMediaTrack->obtainBufferingLen(completedBuffer, pendingBuffer);
        if ( pMediaTrack->mediaType() == MEDIA_VIDEO )
            vbuflbytes = pMediaTrack->bufferSize();
        else
            abuflbytes = pMediaTrack->bufferSize();

        ullong usedBuffer, availableBuffer;
        getBufferStats(pMediaTrack->mediaType(), usedBuffer, availableBuffer);

        if ( enableTraceLogging )
        {
            NTRACE(TRACE_STREAMINGSTAT, "status update track id: %d, media type: %u "
                                        "playing %u kbps, streaming %d kbps at %llu ms "
                                        "has (%u, %u) ms, %u bytes buffer, available %llu bytes",
                                        pMediaTrack->trackId(), pMediaTrack->mediaType(),
                                        pMediaTrack->getPlaybackBitrate(),
                                        pMediaTrack->getStreamingBitrate(),
                                        pMediaTrack->streamingTime().getValueInMs(),
                                        completedBuffer, pendingBuffer,
                                        pMediaTrack->bufferSize(),
                                        availableBuffer);
            PERFTRACE("status update track id: %d, media type: %u "
                      "playing %u kbps, streaming %d kbps at %llu ms "
                      "has (%u, %u) ms, %u bytes buffer, available %llu bytes\n",
                      pMediaTrack->trackId(), pMediaTrack->mediaType(),
                      pMediaTrack->getPlaybackBitrate(),
                      pMediaTrack->getStreamingBitrate(),
                      pMediaTrack->streamingTime().getValueInMs(),
                      completedBuffer, pendingBuffer,
                      pMediaTrack->bufferSize(),
                      availableBuffer);
        }
        mStreamingStatPtr->updateStreamingPts(pMediaTrack->trackId(), pMediaTrack->streamingTime());
        pStreamingMediaStat.reset(new StreamingMediaStat(pMediaTrack->mediaType(),
                                                         pMediaTrack->getPlaybackBitrate(), pMediaTrack->getStreamingBitrate(),
                                                         pMediaTrack->streamingTime().getValueInMs(),
                                                         completedBuffer, pendingBuffer,
                                                         pMediaTrack->bufferSize(),
                                                         availableBuffer));
        pStreamingStatReport->mStreamingMediaStatList.push_back(pStreamingMediaStat);
    }

    uint32_t predictedRebuffer = 0;
    MediaTrackPtr pVideoTrack = obtainMediaTrack(MEDIA_VIDEO);
    uint32_t playbackTime = pVideoTrack->playbackTime().getValueInMs();
    if ( (mContentDuration > playbackTime) && (mPredictedRebuffers > 0) )
    {
        predictedRebuffer = mPredictedRebuffers * (mContentDuration - playbackTime) / (3600 * 1000);
        if ( predictedRebuffer == 0 )
            predictedRebuffer = 1;
    }

    if ( enableTraceLogging )
    {
        DEBUGLOG(MediaPresentationLog, "updateBufferLevel: %u, %u",
                        predictedRebuffer, currentThroughput);
    }
    mStreamingReporterPtr->updateBufferLevel(abuflbytes, vbuflbytes,
                                             predictedRebuffer,
                                             currentThroughput );
    mStreamingReporterPtr->updateStreamingStat(pStreamingStatReport);
    return;
}

bool
MediaPresentation::tooMuchMediaBuffer(MediaType mediaType)
{
    bool metric1 = false;
    bool metric2 = false;

    if (mEffectiveMaxMediaBufferLen > 0)
    {
        uint32_t audioBufferLen, videoBufferLen;
        obtainBufferLen(audioBufferLen, videoBufferLen);
        if ( (videoBufferLen >= mEffectiveMaxMediaBufferLen) && (audioBufferLen >= mEffectiveMaxMediaBufferLen) )
            metric1 = true;
        else
            metric1 = false;
    }

    uint32_t audioBufferSize, videoBufferSize;
    obtainBufferSize(audioBufferSize, videoBufferSize);

    uint32_t bufferSize, bufferPoolSize;

    switch (mediaType)
    {
        case MEDIA_AUDIO:
            bufferPoolSize = mConfigedAudioBufferSize;
            bufferSize = (mInternalStreamingState == DOWNLOADING) ? audioBufferSize : audioBufferSize + mAudioBufferHighTargetAvailableSize;
            break;
        case MEDIA_VIDEO:
            bufferPoolSize = mConfigedVideoBufferSize;
            bufferSize = (mInternalStreamingState == DOWNLOADING) ? videoBufferSize : videoBufferSize + mMediaBufferHighTargetAvailableSize;
            break;
        default:
            bufferSize = bufferPoolSize = 0;
            assert(0);
            break;
    }
    if ( bufferSize >= bufferPoolSize )
        metric2 = true;
    else
        metric2 = false;

    return (metric1 || metric2);
}


void
MediaPresentation::obtainMixedBufferLen(uint32_t& minBufferLen, uint32_t& maxPendingBufferLen) const
{
    minBufferLen = 1000 * 1000 * 1000;
    maxPendingBufferLen = 0;
    for ( vector<MediaTrackPtr>::const_iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        uint32_t bufferLen, pendingBufferLen;
        (*iter)->obtainBufferingLen(bufferLen, pendingBufferLen);
        if (bufferLen < minBufferLen)
            minBufferLen = bufferLen;
        if (pendingBufferLen > maxPendingBufferLen)
            maxPendingBufferLen = pendingBufferLen;
    }
}

void MediaPresentation::obtainBufferLen(uint32_t& audioBufferLen, uint32_t& videoBufferLen)
{
    MediaTrackPtr pVideoTrack = obtainMediaTrack(MEDIA_VIDEO);
    MediaTrackPtr pAudioTrack = obtainMediaTrack(MEDIA_AUDIO);
    uint32_t pendingAudioBufferLen, pendingVideoBufferLen;
    pVideoTrack->obtainBufferingLen(videoBufferLen, pendingVideoBufferLen);
    pAudioTrack->obtainBufferingLen(audioBufferLen, pendingAudioBufferLen);
    return;
}

void MediaPresentation::obtainBufferSize(uint32_t& audioBufferSize, uint32_t& videoBufferSize)
{
    MediaTrackPtr pVideoTrack = obtainMediaTrack(MEDIA_VIDEO);
    MediaTrackPtr pAudioTrack = obtainMediaTrack(MEDIA_AUDIO);
    audioBufferSize = pAudioTrack->bufferSize();
    videoBufferSize = pVideoTrack->bufferSize();
    return;
}


void
MediaPresentation::obtainPlaybackStats(double& playbackRate,
                                       AseTimeStamp& playbackTime,
                                       ullong& availableBuffer)
{
    // obtain the playback time/rate
    mMediaSinkPtr->currentPlaybackInfo( playbackRate, playbackTime );
    if ( mMediaConsumerState == IMediaConsumer::MEDIA_CONSUMER_PLAYING_STATE )
    {
        playbackRate = 1.0;
    }
    else
    {
        playbackRate = 0;
    }

    if ( playbackTime < mActualStartPts )
        playbackTime = mActualStartPts;

    for ( vector<MediaTrackPtr>::iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        (*iter)->updatePlaybackTime(playbackTime);
    }

    ullong usedBuffer;
    getBufferStats(MEDIA_VIDEO, usedBuffer, availableBuffer);

    return;
}


uint32_t
MediaPresentation::obtainCurrentThroughput(uint32_t currentBufferLevel, double& confidence, string& locationSetKey)
{
    ILocationSetMonitorPtr pLocationSetMonitor = mNetworkMonitorPtr->getCurrentLocationSetMonitor();

    if ( pLocationSetMonitor.get() != NULL )
    {
        locationSetKey = pLocationSetMonitor->getLocationSetKey();
        INetworkMonitor::PredictionSource source;
        return mNetworkMonitorPtr->getPredictedThroughput( pLocationSetMonitor, currentBufferLevel, source, confidence );
    }
    else
    {
        confidence = 0.0;
        locationSetKey = "";
        return 0;
    }
}

bool
MediaPresentation::bitrateSwitchFreeze() const
{
    return mLocationSelectorPtr->bitrateSwitchFreeze();
}

uint32_t
MediaPresentation::obtainBufferingDifference()
{
    uint32_t minStreamingTime = 1000 * 1000 * 1000;
    uint32_t maxStreamingTime = 0;
    for ( vector<MediaTrackPtr>::const_iterator iter = mMediaTracks.begin();
            iter != mMediaTracks.end();
            ++iter )
    {
        uint32_t streamingTime = (*iter)->streamingTime().getValueInMs();
        if ( streamingTime < minStreamingTime )
            minStreamingTime = streamingTime;
        if ( streamingTime > maxStreamingTime )
            maxStreamingTime = streamingTime;
    }

    return (maxStreamingTime - minStreamingTime);
}

/**             Internal helper functions                                */
/** -------------------------------------------------------------------- */
