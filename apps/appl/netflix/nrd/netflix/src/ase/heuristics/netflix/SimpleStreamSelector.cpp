/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "SimpleStreamSelector.h"
#include "IStreamingLocationSet.h"

#include <nrdase/IAseConfigParameter.h>
#include <nrdase/ILocationSelector.h>
#include <nrdase/AseCommonUtil.h>
#include <nrdbase/Thread.h>

#include <nrdase/DebugLogging.h>
#include <nrdase/Manifest.h>

#include <nrdbase/tr1.h>

#include <iostream>
#include <algorithm>

using namespace netflix::ase;
using namespace std;

namespace netflix {
namespace ase {
DEFINE_LOGGER( SimpleStreamSelectorLog );
}
}

#ifndef _MSC_VER
const uint32_t SimpleStreamSelector::defaultTotalBufferSize;
#endif

#define MIN_INIT_THROUGHPUT 1
#define DEFAULT_GOP_DL_TIME 3000

/**
 * 1. The implementation assumes one audio and one video track
 * 3. Support using startplay curve point
 */

SimpleStreamSelector::SimpleStreamSelector( IAseConfigParameterPtr pConfigParameter,
                                            IAseConfigParameterPtr pLocalConfigParameter,
                                            INetworkMonitorPtr pNetworkMonitor,
                                            ILocationSelectorPtr pLocationSelector)
                            : mFeasibilitySleepMicros(-1),
                              mConfigParameterPtr(pConfigParameter),
                              mLocalConfigParametePtr(pLocalConfigParameter),
                              mNetworkMonitorPtr(pNetworkMonitor),
                              mLocationSelectorPtr(pLocationSelector),
                              mStreamingState(PLAYER_STOPPED),
                              mSkipped(false),
                              mPrevBufferingStatus(IStreamSelector::BUFFER_LOW),
                              mPrevPredictBuffer(0),
                              mLastSwitchState(NO_ACTION),
                              mLastSwitchTime(AseTimeStamp(0,1)),
                              mRequiredBufferingLen(0),
                              mEstimatedBufferingTime(0),
                              mGoodQuality(false),
                              mMediaFragmentDuration(0),
                              mHeuristicState(STREAMING_STARTUP),
                              mCurrentBufferLength(AseTimeInterval::ZERO),
                              mInitSelect(true),
                              mMinAcceptableVideoBitrateDuringPlayback(-1),
                              mFirstSelectDuringPlayback(false)
{
    loadConfigParas();
}

void SimpleStreamSelector::obtainBufferingStats( uint32_t& requiredBufferingLen, uint32_t& estimatedBufferingTime )
{
    requiredBufferingLen = mRequiredBufferingLen;
    estimatedBufferingTime = mEstimatedBufferingTime;
}

void SimpleStreamSelector::getStreamingQuality( bool& goodQuality )
{
    goodQuality = mGoodQuality;
}

void SimpleStreamSelector::configure( IAseConfigParameterPtr pConfig )
{
    mConfigParameterPtr = pConfig;

    loadConfigParas();
}

void
SimpleStreamSelector::selectStream(int32_t trackId,
                                   std::vector<ManifestTrackPtr> const& trackList,
                                   int32_t& streamIndex,
                                   uint32_t& numberOfPredictedRebuffers)
{
#if 1
    assert(trackId >= 0);
    assert(trackId < (int32_t)trackList.size());
    numberOfPredictedRebuffers = 0;
    ManifestTrackPtr pManifestTrack = trackList[trackId];
    MediaType mediaType = pManifestTrack->getMediaType();
    vector<ManifestStreamPtr> const& manifestStreams = pManifestTrack->getStreams();
    uint32_t audioAllocatedThroughput, videoAllocatedThroughput;
    uint32_t averageBitrate;
    if ( mediaType == MEDIA_AUDIO )
    {
        uint32_t i = 0;
        for ( i = 0; i < manifestStreams.size(); ++i)
        {
            ManifestStreamPtr pManifestStream = manifestStreams[i];
            if ( isFeasibleSimple(pManifestStream->getNominalBitrate(), pManifestStream->getStreamKey(), MEDIA_AUDIO) != FEASIBLE )
                break;
        }
        if ( i >= manifestStreams.size() )
            --i;
        streamIndex = i;
    }
    else if ( mediaType == MEDIA_VIDEO )
    {
        bool hasValidStream = false;
        ManifestStreamPtr pManifestStream;
        streamIndex = 0;
        for (uint32_t i = 0; i < manifestStreams.size(); ++i)
        {
            pManifestStream = manifestStreams[i];
            obtainThroughput(pManifestStream->getStreamKey(), audioAllocatedThroughput, videoAllocatedThroughput);
            averageBitrate = pManifestStream->getNominalBitrate();
            if ( pManifestStream->enabled() )
            {
                if ( (isFeasibleForFirstStream(averageBitrate, pManifestStream->getStreamKey(), videoAllocatedThroughput) == FEASIBLE) ||
                        !hasValidStream )
                {
                    streamIndex = i;
                    hasValidStream = true;
                }
            }
        }

        pManifestStream = manifestStreams[streamIndex];
        averageBitrate = pManifestStream->getNominalBitrate();
        obtainThroughput(pManifestStream->getStreamKey(), audioAllocatedThroughput, videoAllocatedThroughput);
        if ( isFeasibleSimple(averageBitrate, pManifestStream->getStreamKey(), MEDIA_VIDEO) == UNFEASIBLE )
        {
            numberOfPredictedRebuffers =
                estimateNumberOfRebuffers(videoAllocatedThroughput * (100 + mBandwidthMargin) / 100,
                                          averageBitrate,
                                          DURATION_FOR_PREDICT_REBUFFER, mMaxBufferingAmountAllowed);
        }
    }

    return;

#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(trackList);
    streamIndex = 0;
    numberOfPredictedRebuffers = 0;
#endif
}



/**
 * 1.   SimpleStreamSelector assumes that the streams in ITrackInfoPtr are ordered based on their average bitrate
 *    from low to high.
 */
IStreamSelector::BufferingStatus
SimpleStreamSelector::selectStream( bool downSwitchFreeze,
                                    int32_t trackId,
                                    double currentPlayoutRate,
                                    AseTimeStamp const& currentPlayoutTime,
                                    std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                                    ullong availableBuffer,
                                    std::vector<ITrackInfoPtr> const& trackFutureInfo,
                                    StreamFeasibility& streamFeasibility,
                                    int32_t& streamId,
                                    int32_t& numOfBlocks,
                                    int32_t& predictedDlTimeForBackupStream,
                                    uint32_t& predictedRebuffer)
{
    assert(trackId >= 0);

    DEBUGLOG ( SimpleStreamSelectorLog,
                        "SimpleStreamSelector::selectStream "
                        "track id: %d "
                        "playback rate: %f, playback time: %llu ms, availableBuffer: %llu ",
                        trackId,
                        currentPlayoutRate, currentPlayoutTime.getValueInMs(), availableBuffer );
    AseTimeStamp playbackTime = currentPlayoutTime;
    // scan the buffered track info to obtain some metadata
    vector<TrackMetaInfo> trackMetaList =
                        scanTrackInfo( playbackTime, trackCurrentInfo );
    mCurrentBufferLength = calculateBufferLen(trackMetaList);

    // update the current streaming state
    updateCurrentStreamingState( currentPlayoutRate );

    BufferingStatus bufferingStatus = mPrevBufferingStatus;
    if ( mStreamingState == PLAYER_BUFFERING )
    {
        mLastSwitchState = NO_ACTION;
        // The player is buffering
        bufferingStatus = selectStreamForBuffering( trackId,
                                                    playbackTime,
                                                    trackMetaList,
                                                    trackCurrentInfo,
                                                    trackFutureInfo,
                                                    availableBuffer,
                                                    streamFeasibility,
                                                    streamId,
                                                    numOfBlocks,
                                                    predictedRebuffer);
        // no need to compute this during buffering
        predictedDlTimeForBackupStream = -1;

    }
    else if ( mStreamingState == PLAYER_PLAYING )
    {
        // The player is streaming and playing
        // Allocate 80% of buffer to video
        bufferingStatus = selectStreamDuringPlayback( downSwitchFreeze,
                                                      trackId, currentPlayoutRate, playbackTime,
                                                      trackMetaList,
                                                      trackCurrentInfo, availableBuffer,
                                                      trackFutureInfo,
                                                      streamFeasibility,
                                                      streamId, numOfBlocks,
                                                      predictedDlTimeForBackupStream,
                                                      predictedRebuffer);
    }

    DEBUGLOG(SimpleStreamSelectorLog, "leaving selectStream, buffering status: %d, predictedRebuffer: %u, number of blocks: %d",
                        bufferingStatus, predictedRebuffer, numOfBlocks);
    return bufferingStatus;
}

vector<TrackMetaInfo>
SimpleStreamSelector::scanTrackInfo( AseTimeStamp const& currentPlayoutTime,
                                     vector<ITrackBufferInfoPtr> const& trackCurrentInfo)
{
    vector<TrackMetaInfo> trackMetaList;

    for ( vector<ITrackBufferInfoPtr>::const_iterator iter = trackCurrentInfo.begin();
            iter != trackCurrentInfo.end();
            ++ iter )
    {
        int32_t trackId = (*iter)->trackId();
        AseTimeStamp nextAvailableFragmentTime = (*iter)->getFirstFragmentStartTime();
        AseTimeStamp streamingTime = nextAvailableFragmentTime;
        bool haveIncompleteFragment = false;

        for ( ITrackBufferInfo::const_iterator bufferFragmentIter = (*iter)->begin();
                bufferFragmentIter != (*iter)->end();
                ++bufferFragmentIter )
        {
            BufferedFragmentInfo bufferedFragmentInfo = *bufferFragmentIter;
            streamingTime += bufferedFragmentInfo.mDuration;
            if ( (int32_t)bufferedFragmentInfo.mReceived < bufferedFragmentInfo.mSize )
            {
                haveIncompleteFragment = true;
            }
            if ( !haveIncompleteFragment )
            {
                // All buffer following an incomplete fragment is not counted into the buffer used
                // by the heuristics
                nextAvailableFragmentTime += bufferedFragmentInfo.mDuration;
            }
            if ( !mMediaFragmentDuration )
                mMediaFragmentDuration = bufferedFragmentInfo.mDuration.getValueInMs();
        }
        TrackMetaInfo trackMetaInfo;
        if ( (*iter)->empty() || (nextAvailableFragmentTime <= currentPlayoutTime) )
        {
            trackMetaInfo.mCompleteBufferLen = AseTimeStamp::ZERO;
            trackMetaInfo.streamingTime = currentPlayoutTime;
            trackMetaInfo.mInCompleteBufferLen = AseTimeStamp::ZERO;
        }
        else
        {
            trackMetaInfo.mCompleteBufferLen = nextAvailableFragmentTime - currentPlayoutTime;
            trackMetaInfo.streamingTime = streamingTime;
            trackMetaInfo.mInCompleteBufferLen = streamingTime - currentPlayoutTime;
        }

        trackMetaInfo.mTrackId = trackId;
        trackMetaInfo.mMediaType = (*iter)->mediaType();
        trackMetaList.push_back( trackMetaInfo );
    }

#if 0
    DEBUGLOG( SimpleStreamSelectorLog, "tracks' metadata: " );
    for ( vector<TrackMetaInfo>::iterator iter = trackMetaList.begin();
            iter != trackMetaList.end();
            ++iter )
    {
        DEBUGLOG( SimpleStreamSelectorLog,
                        "track id: %d, media type: %d, buffer len: %llu ms, nextAvailableFragmentTime: %llu ms ",
                        iter->mTrackId, iter->mMediaType,
                        (iter->mCompleteBufferLen).getValueInMs(),
                        (iter->streamingTime).getValueInMs() );
    }
#endif


    return trackMetaList;
}

void SimpleStreamSelector::stop()
{
    mStreamingState = PLAYER_STOPPED;
    mLastSwitchTime = AseTimeStamp(0, 1);
    mLastSwitchState = NO_ACTION;
    mInitSelect = true;
}

void SimpleStreamSelector::skip()
{
    mSkipped = true;
    mLastSwitchTime = AseTimeStamp(0, 1);
    mLastSwitchState = NO_ACTION;
}


void SimpleStreamSelector::updateCurrentStreamingState(double currentPlayoutRate)
{
    if (mStreamingState == PLAYER_STOPPED)
    {
        mStreamingState = PLAYER_BUFFERING;
        mHeuristicState = STREAMING_STARTUP;
    }
    else if ( currentPlayoutRate > 0 )
    {
        if ( mStreamingState != PLAYER_PLAYING )
        {
            mFirstSelectDuringPlayback = true;
            mMinAcceptableVideoBitrateDuringPlayback = -1;
        }
        mStreamingState = PLAYER_PLAYING;
    }

    uint32_t bufferLevel = mCurrentBufferLength.getValueInMs();
    switch (mHeuristicState)
    {
        case STREAMING_STARTUP:
            if ( bufferLevel > mToStableThreshold )
                mHeuristicState = STABLE_STREAMING;
            break;
        case STABLE_STREAMING:
            if ( bufferLevel < mToUnstableThreshold )
            {
                if (!mSkipped)
                    mHeuristicState = UNSTABLE_STREAMING;
                else
                {
                    mSkipped = false;
                    mHeuristicState = STREAMING_STARTUP;
                }
            }
            break;
        case UNSTABLE_STREAMING:
            if ( bufferLevel > mToStableThreshold )
                mHeuristicState = STABLE_STREAMING;
            break;
        default:
            break;
    }

    return;
 }

bool SimpleStreamSelector::completeTrack( int32_t trackId, vector<ITrackInfoPtr> const& trackFutureInfo )
{
    bool trackCompleted = true;
    for ( vector<ITrackInfoPtr>::const_iterator iter = trackFutureInfo.begin();
            iter != trackFutureInfo.end();
            ++iter )
    {
        if ( trackId == (*iter)->trackId() )
        {
            trackCompleted = (*iter)->isCompleted();
            break;
        }
    }
    return trackCompleted;
}


IStreamSelector::BufferingStatus
SimpleStreamSelector::selectStreamDuringPlayback( bool downSwitchFreeze,
                            int32_t trackId,
                            double currentPlayoutRate,
                            AseTimeStamp const& currentPlayoutTime,
                            std::vector<TrackMetaInfo> const& trackMetaList,
                            std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                            ullong availableBuffer,
                            std::vector<ITrackInfoPtr> const& trackFutureInfo,
                            StreamFeasibility& streamFeasibility,
                            int32_t& streamId,
                            int32_t& numOfBlocks,
                            int32_t& predictedDlTimeForBackupStream,
                            uint32_t& predictedRebuffer)
{
    predictedDlTimeForBackupStream = -1;
    predictedRebuffer = 0;
    numOfBlocks = 1;
    DEBUGLOG( SimpleStreamSelectorLog,
              "selectStreamDuringPlayback: mLastSwitchState: %d, downSwitchFreeze: %u",
              mLastSwitchState, downSwitchFreeze );
    // separate a/v track
    TrackMetaInfo audioTrackMeta, videoTrackMeta;
    ITrackBufferInfoPtr pAudioTrackBufferInfo, pVideoTrackBufferInfo;
    ITrackInfoPtr pAudioTrackInfo, pVideoTrackInfo;
    separateAVTrackInfo( trackMetaList, trackCurrentInfo, trackFutureInfo,
                         audioTrackMeta, videoTrackMeta,
                         pAudioTrackBufferInfo, pVideoTrackBufferInfo,
                         pAudioTrackInfo, pVideoTrackInfo );

    IStreamInfoPtr pAudioStreamInfo = *(pAudioTrackInfo->current());
    if ( pAudioTrackInfo->trackId() == trackId )
    {
        // select an audio stream
        // return the currently used audio stream
        if ( !pAudioStreamInfo )
        {
            // The user just switches the audio track
            selectAudioStream(pAudioTrackInfo, streamId);
        }
        else
        {
            streamId = pAudioStreamInfo->streamId();
        }
        predictedRebuffer = mPrevPredictBuffer;
        return mPrevBufferingStatus;
    }

    // handling video now
    mGoodQuality = false;
    uint32_t audioBitrate = pAudioStreamInfo->getAverageBitrate() ;
    uint32_t currentBufLevel = videoTrackMeta.mCompleteBufferLen.getValueInMs();
    uint32_t connectTime;

    IStreamInfoPtr pVideoStreamInfo;
    uint32_t videoBitrate;
    ITrackInfo::const_iterator videoStreamIterator = pVideoTrackInfo->begin();

    uint32_t audioAllocatedThroughput, videoAllocatedThroughput;
    obtainThroughput((*videoStreamIterator)->streamKey(), audioAllocatedThroughput, videoAllocatedThroughput);
    uint32_t currentFragmentSize = (*videoStreamIterator)->getCurrentFragmentSize();
    predictedDlTimeForBackupStream = currentFragmentSize * 8 / videoAllocatedThroughput;

    videoStreamIterator = pVideoTrackInfo->current();
    ITrackInfo::const_iterator newVideoStreamIter = videoStreamIterator;

    if ( (*videoStreamIterator)->isAvailable() == false)
    {
        // if the current stream becomes unavailable, the player searches for the closest
        // available stream first
        if (getStreamWithLowerBitrate( videoStreamIterator, newVideoStreamIter ) == SEL_OK)
        {
            videoStreamIterator = newVideoStreamIter;
        }
        else if (getStreamWithHigherBitrate(videoStreamIterator, newVideoStreamIter) == SEL_OK)
        {
            videoStreamIterator = newVideoStreamIter;
        }

        if ( (*videoStreamIterator)->isAvailable() == false )
        {
            // should not happen: can't not find available stream?
            // have to ignore it
            DEBUGLOG(SimpleStreamSelectorLog, "ERROR: All streams become unavailable!");
            assert(0);
        }
    }

    ITrackInfo::const_iterator currentVideoStreamIter = videoStreamIterator;
    uint32_t currentVideoBitrate = (*currentVideoStreamIter)->getAverageBitrate();

    if ( mFirstSelectDuringPlayback )
    {
        // update the minimum acceptable bitrate during playback
        mMinAcceptableVideoBitrateDuringPlayback = min(mMinAcceptableVideoBitrate, (int32_t)currentVideoBitrate);
        DEBUGLOG(SimpleStreamSelectorLog,
                "update minAcceptableVideoBitrate, %d:%u, %d kbps",
                mMinAcceptableVideoBitrate, currentVideoBitrate,
                mMinAcceptableVideoBitrateDuringPlayback);
        mFirstSelectDuringPlayback = false;
    }


    BufferingStatus bufferingStatus = BUFFER_LOW;
    if ( canSkipVideoStream(currentBufLevel) )
    {
        // only allow skipping bitrate for seamless switch
        DEBUGLOG(SimpleStreamSelectorLog, "allow to skip bitrate during up switch");
        bufferingStatus = selectVideoStreamWithSkipping(
                                            videoStreamIterator,
                                            streamFeasibility,
                                            currentBufLevel,
                                            audioBitrate,
                                            currentPlayoutRate, currentPlayoutTime,
                                            pVideoTrackBufferInfo, pVideoTrackInfo,
                                            availableBuffer);

    }
    else
    {
        bufferingStatus = selectVideoStreamWithoutSkipping(
                                            videoStreamIterator,
                                            streamFeasibility,
                                            currentBufLevel,
                                            audioBitrate,
                                            currentPlayoutRate, currentPlayoutTime,
                                            pVideoTrackBufferInfo, pVideoTrackInfo,
                                            availableBuffer);
    }

    if ( downSwitchFreeze )
    {
        uint32_t selVideoBitrate = (*videoStreamIterator)->getAverageBitrate();
        if ( selVideoBitrate < currentVideoBitrate )
        {
            // Not to allow downswitch
            videoStreamIterator = currentVideoStreamIter;
            if ( mAllowBitrateProbingUnknownLocation )
                streamFeasibility = UNKNOWN_FEASIBILITY;
            else
                streamFeasibility = UNFEASIBLE;
        }
    }

    streamId = (*videoStreamIterator)->streamId();
    videoBitrate = (*videoStreamIterator)->getAverageBitrate();
    INetworkMonitor::PredictionSource source;
    obtainNetworkStat((*videoStreamIterator)->streamKey(),
                       videoAllocatedThroughput, connectTime,
                       source,
                       0, currentBufLevel);
    DEBUGLOG( SimpleStreamSelectorLog,
                    "select video stream id: %d, nominal bitrate: %u kbps, buffering status: %d"
                    ", stream feasibility: %u, predicted time for backup: %u",
                        streamId, videoBitrate, bufferingStatus,
                        streamFeasibility, predictedDlTimeForBackupStream );
    if ( videoStreamIterator != pVideoTrackInfo->current() )
    {
        DEBUGLOG( SimpleStreamSelectorLog,
                "playback stream switch old video stream: %u kbps,  new video stream %u kbps ",
                (*(pVideoTrackInfo->current()))->getAverageBitrate(),
                videoBitrate );
    }
    if ( (bufferingStatus == BUFFER_LOW) || (videoAllocatedThroughput < videoBitrate) )
    {
        predictedRebuffer = estimateNumberOfRebuffers(videoAllocatedThroughput * (100 + mBandwidthMargin) / 100,
                                                      videoBitrate,
                                                      DURATION_FOR_PREDICT_REBUFFER,
                                                      mMaxBufferingAmountAllowed);
    }
    else
    {
        predictedRebuffer = 0;
    }

    mPrevBufferingStatus = bufferingStatus;
    mPrevPredictBuffer = predictedRebuffer;
    return bufferingStatus;
}

void SimpleStreamSelector::selectAudioStream( ITrackInfoPtr pTrackInfo,
                                              int32_t& streamId )
{
    ITrackInfo::const_iterator currentStreamInfoIterator = pTrackInfo->current();
    ITrackInfo::const_iterator streamInfoIter = currentStreamInfoIterator;
    // the iterator pointing a stream which bitrate is higher than minimum init video bitrate
    // ITrackInfo::const_iterator validStreamInfoIter = pTrackInfo->end();
    uint32_t nominalBitrate = 0;

    assert( pTrackInfo->mediaType() == MEDIA_AUDIO );

    if ( currentStreamInfoIterator != pTrackInfo->end() )
    {
        // there is a audio stream selected
        streamId = (*currentStreamInfoIterator)->streamId();
    }
    else
    {
        // find the stream, which has the maximum bitrate among all streams,
        // whose bitrate is lower than audioAllocatedThroughput
        streamInfoIter = pTrackInfo->begin();
        streamId = (*streamInfoIter)->streamId();
        for ( streamInfoIter = pTrackInfo->begin(); streamInfoIter != pTrackInfo->end(); ++streamInfoIter )
        {
            nominalBitrate = (*streamInfoIter)->getAverageBitrate();
            if ( isFeasibleSimple(nominalBitrate, (*streamInfoIter)->streamKey(), MEDIA_AUDIO) != FEASIBLE )
            {
                break;
            }
            streamId = (*streamInfoIter)->streamId();
        }
        DEBUGLOG(SimpleStreamSelectorLog,
                "select the audio stream id: %d, nominal bitrate: %d kbps",
                streamId, nominalBitrate );
    }


    return;
}


SimpleStreamSelector::BufferingStatus
SimpleStreamSelector::selectStreamForBuffering( int32_t trackId,
                                   AseTimeStamp const& currentPlayoutTime,
                                   std::vector<TrackMetaInfo> const& trackMetaList,
                                   std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                                   std::vector<ITrackInfoPtr> const& trackFutureInfo,
                                   ullong availableBuffer,
                                   StreamFeasibility& streamFeasibility,
                                   int32_t& streamId,
                                   int32_t& numOfBlocks,
                                   uint32_t& predictedRebuffer)
{
    numOfBlocks = 1;

    BufferingStatus bufferingStatus = mPrevBufferingStatus;
    predictedRebuffer = mPrevPredictBuffer;

    // get the selected track's info
    ITrackInfoPtr pTrackInfo;
    for ( vector<ITrackInfoPtr>::const_iterator iter = trackFutureInfo.begin();
            iter != trackFutureInfo.end();
            ++iter )
    {
        if ( (*iter)->trackId() == trackId )
        {
            pTrackInfo = *iter;
        }
    }

    ITrackInfo::const_iterator streamInfoIter = pTrackInfo->current();
    // the iterator pointing a stream which bitrate is higher than minimum init video bitrate
    // ITrackInfo::const_iterator validStreamInfoIter = pTrackInfo->end();
    if ( pTrackInfo->mediaType() == MEDIA_AUDIO )
    {
        selectAudioStream(pTrackInfo, streamId);
    }
    else if ( pTrackInfo->mediaType() == MEDIA_VIDEO )
    {
        streamInfoIter = pTrackInfo->current();
        if ( (streamInfoIter == pTrackInfo->end()) || mInitSelect )
        {

            bufferingStatus = selectFirstVideoStream( pTrackInfo, streamId,
                                                     predictedRebuffer,
                                                     streamInfoIter);
            mInitSelect = false;
        }
        else
        {
            int32_t predictedDlTimeForBackupStream;
            int32_t goodQualityStreamId = -1;
            if ( mEnableGoodQualityBuffering )
            {
                bufferingStatus = selectStreamDuringPlayback(false, trackId, 1.0, currentPlayoutTime,
                                                             trackMetaList, trackCurrentInfo,
                                                             availableBuffer, trackFutureInfo,
                                                             streamFeasibility, goodQualityStreamId, numOfBlocks,
                                                             predictedDlTimeForBackupStream, predictedRebuffer);
                if (bufferingStatus == BUFFER_LOW)
                    goodQualityStreamId = -1;
            }
            bufferingStatus = selectVideoStreamForBuffering( pTrackInfo, streamId,
                                                             predictedRebuffer, numOfBlocks,
                                                             currentPlayoutTime, trackMetaList,
                                                             trackCurrentInfo, trackFutureInfo,
                                                             availableBuffer,
                                                             mCurrentBufferLength, goodQualityStreamId );
        }
        mPrevBufferingStatus = bufferingStatus;
        mPrevPredictBuffer = predictedRebuffer;
    }

    if ( predictedRebuffer > 0 )
        streamFeasibility = UNFEASIBLE;
    else
        streamFeasibility = FEASIBLE;

    return bufferingStatus;
}

IStreamSelector::BufferingStatus
SimpleStreamSelector::selectFirstVideoStream( ITrackInfoPtr pTrackInfo,
                                              int32_t& streamId,
                                              uint32_t& predictedRebuffer,
                                              ITrackInfo::const_iterator preSelectedStreamIter)
{
    uint32_t selStreamIndex = 0;
    int32_t preSelectedStreamIndex = -1;
    BufferingStatus bufferingStatus = mPrevBufferingStatus;
    predictedRebuffer = 0;
    // select the very first video stream
    ITrackInfo::const_iterator streamInfoIter = pTrackInfo->begin();
    streamId = (*streamInfoIter)->streamId();
    ITrackInfo::const_iterator selStreamInfoIter = streamInfoIter;
    bool hasValidStream = false;
    uint32_t audioAllocatedThroughput, videoAllocatedThroughput;
    uint32_t averageBitrate;

    uint32_t streamIndex = 0;
    for ( streamInfoIter = pTrackInfo->begin(); streamInfoIter != pTrackInfo->end(); ++streamInfoIter )
    {
        if ( (*streamInfoIter)->isAvailable() )
        {
            obtainThroughput((*streamInfoIter)->streamKey(), audioAllocatedThroughput, videoAllocatedThroughput);
            averageBitrate = (*streamInfoIter)->getAverageBitrate();
            if ( (isFeasibleForFirstStream(averageBitrate, (*streamInfoIter)->streamKey(), videoAllocatedThroughput) == FEASIBLE) ||
                    !hasValidStream )
            {
                selStreamInfoIter = streamInfoIter;
                selStreamIndex = streamIndex;
                hasValidStream = true;
            }

            if ( (preSelectedStreamIter != pTrackInfo->end()) &&
                    ((*preSelectedStreamIter)->streamKey() == (*streamInfoIter)->streamKey()) )
                preSelectedStreamIndex = streamIndex;

            ++streamIndex;
        }
    }

    if (!qualifyStreamSwitch((int32_t)selStreamIndex, preSelectedStreamIndex))
    {
        // avoid stream switch if the newly selected stream does not differ enough from the pre-selected stream
        selStreamInfoIter = preSelectedStreamIter;
    }

    streamId = (*selStreamInfoIter)->streamId();
    averageBitrate = (*selStreamInfoIter)->getAverageBitrate();
    obtainThroughput((*selStreamInfoIter)->streamKey(), audioAllocatedThroughput, videoAllocatedThroughput);
    if ( isFeasibleSimple(averageBitrate, (*selStreamInfoIter)->streamKey(), MEDIA_VIDEO) == UNFEASIBLE )
    {
        predictedRebuffer =
            estimateNumberOfRebuffers(videoAllocatedThroughput * (100 + mBandwidthMargin) / 100,
                    averageBitrate,
                    DURATION_FOR_PREDICT_REBUFFER, mMaxBufferingAmountAllowed);
    }
    mRequiredBufferingLen = adjustRequiredBufferingLen(*selStreamInfoIter, mMinPrebufLength);
    mEstimatedBufferingTime = averageBitrate * mRequiredBufferingLen / videoAllocatedThroughput;
    bufferingStatus = BUFFER_LOW;
    DEBUGLOG( SimpleStreamSelectorLog,
            "select the first video stream id: %d, nominal bitrate: %d kbps",
            streamId, averageBitrate );
    return bufferingStatus;
}

IStreamSelector::BufferingStatus
SimpleStreamSelector::selectVideoStreamForBuffering( ITrackInfoPtr pTrackInfo,
                                                     int32_t& streamId,
                                                     uint32_t& predictedRebuffer,
                                                     int32_t& numOfBlocks,
                                                     AseTimeStamp const& currentPlayoutTime,
                                                     std::vector<TrackMetaInfo> const& trackMetaList,
                                                     std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                                                     std::vector<ITrackInfoPtr> const& trackFutureInfo,
                                                     ullong availableBuffer,
                                                     AseTimeInterval const& minBuffer,
                                                     int32_t goodQualityStreamId)
{
    bool streamSwitch = false;

    BufferingStatus bufferingStatus = mPrevBufferingStatus;
    predictedRebuffer = mPrevPredictBuffer;

    ITrackInfo::const_iterator streamInfoIter = pTrackInfo->current();
    ITrackInfo::const_iterator selStreamInfoIter = streamInfoIter;

    uint32_t audioAllocatedThroughput, videoAllocatedThroughput;
    obtainThroughput((*streamInfoIter)->streamKey(), audioAllocatedThroughput, videoAllocatedThroughput);
    if ( videoAllocatedThroughput <= MIN_INIT_THROUGHPUT )
    {
        streamId = (*streamInfoIter)->streamId();
        predictedRebuffer = 0;
        return BUFFER_LOW;
    }


    if ( mEnableGoodQualityBuffering && ( goodQualityStreamId >= 0) )
    {
        for ( ITrackInfo::const_iterator iter = pTrackInfo->begin();
                iter != pTrackInfo->end();
                ++iter )
        {
            if ( (*iter)->streamId() == goodQualityStreamId)
            {
                streamInfoIter = iter;
                break;
            }
        }

    }
    else
    {

        // settle down on an available stream
        if ( (*streamInfoIter)->isAvailable() == false)
        {
            // if the current stream becomes unavailable, the player searches for the closest
            // available stream first
            if (getStreamWithLowerBitrate( streamInfoIter, selStreamInfoIter ) == SEL_OK)
            {
                streamInfoIter = selStreamInfoIter;
            }
            else if (getStreamWithHigherBitrate(streamInfoIter, selStreamInfoIter) == SEL_OK)
            {
                streamInfoIter = selStreamInfoIter;
            }

            if ( (*streamInfoIter)->isAvailable() == false )
            {
                // should not happen: can't not find available stream?
                // have to ignore it
                DEBUGLOG(SimpleStreamSelectorLog, "All streams become unavailable!");
            }
        }
        selStreamInfoIter = streamInfoIter;

        // re-check whether our current throughput can support the selected stream
        while ( streamInfoIter.hasPrevious() )
        {
            if ( (isFeasibleSimple( (*streamInfoIter)->getAverageBitrate(), (*streamInfoIter)->streamKey(), MEDIA_VIDEO ) == UNFEASIBLE ) ||
                    !(*streamInfoIter)->isAvailable() )
            {
                --streamInfoIter;
                streamSwitch = true;
            }
            else
            {
                break;
            }
        }

        if ( !(*streamInfoIter)->isAvailable() )
            streamInfoIter = selStreamInfoIter;
    }

    streamId = (*streamInfoIter)->streamId();
    uint32_t nominalBitrate = (*streamInfoIter)->getAverageBitrate();
    obtainThroughput((*streamInfoIter)->streamKey(), audioAllocatedThroughput, videoAllocatedThroughput);
    if ( isFeasibleSimple(nominalBitrate, (*streamInfoIter)->streamKey(), MEDIA_VIDEO) == FEASIBLE )
    {
        mRequiredBufferingLen = adjustRequiredBufferingLen(*streamInfoIter, mMinPrebufLength);
        if ( minBuffer.getValueInMs() > mRequiredBufferingLen )
            bufferingStatus = BUFFER_HAVE_ENOUGH;
    }
    else if ((*streamInfoIter)->empty())
    {
        // We don't have the stream index for this stream yet
        // So we ask the player to download the header and a block of data first
        mRequiredBufferingLen = adjustRequiredBufferingLen(*streamInfoIter, mMinPrebufLength);
    }
    else
    {
        // separate a/v track
        TrackMetaInfo audioTrackMeta, videoTrackMeta;
        ITrackBufferInfoPtr pAudioTrackBufferInfo, pVideoTrackBufferInfo;
        ITrackInfoPtr pAudioTrackInfo, pVideoTrackInfo;
        separateAVTrackInfo( trackMetaList, trackCurrentInfo, trackFutureInfo,
                audioTrackMeta, videoTrackMeta,
                pAudioTrackBufferInfo, pVideoTrackBufferInfo,
                pAudioTrackInfo, pVideoTrackInfo );
        // compute the required buffering
        computeRequiredBuffering( videoAllocatedThroughput, 0, currentPlayoutTime,
                pVideoTrackBufferInfo, streamInfoIter,
                availableBuffer, mMaxSimuLength,
                (uint32_t)minBuffer.getValueInMs(),
                mRequiredBufferingLen);
        if ( minBuffer.getValueInMs() >= mRequiredBufferingLen )
            bufferingStatus = BUFFER_HAVE_ENOUGH;
    }

    if ( (videoAllocatedThroughput < nominalBitrate) || (mRequiredBufferingLen >= mMaxBufferingAmountAllowed) )
    {
        predictedRebuffer =
            estimateNumberOfRebuffers(videoAllocatedThroughput * (100 + mBandwidthMargin) / 100,
                    nominalBitrate,
                    DURATION_FOR_PREDICT_REBUFFER,
                    mMaxBufferingAmountAllowed);
    }
    else
    {
        predictedRebuffer = 0;
    }

    numOfBlocks = 1;
    mEstimatedBufferingTime = nominalBitrate * mRequiredBufferingLen / videoAllocatedThroughput;
    DEBUGLOG( SimpleStreamSelectorLog,
            "buffering: select video stream id: %d, nominal bitrate: %d kbps, "
            "required buffering length: %u, estimated buffering time: %u",
            streamId, nominalBitrate, mRequiredBufferingLen, mEstimatedBufferingTime );
    if ( streamSwitch )
    {
        DEBUGLOG( SimpleStreamSelectorLog,
                "buffering stream switch: old video stream %d kbps,  new video stream %d kbps"
                "the number of predicted rebuffers: %u",
                (*(pTrackInfo->current()))->getAverageBitrate(),
                nominalBitrate, predictedRebuffer );
    }

    return bufferingStatus;
}


IStreamSelector::StreamFeasibility
SimpleStreamSelector::isMixedStreamFeasibleWrapper(
                                int32_t currentBufLevel, uint32_t audioBitrate,
                                double currentPlayoutRate, AseTimeStamp const& currentPlayoutTime,
                                ITrackBufferInfoPtr pTrackBufferInfo, ITrackInfo::const_iterator currentStreamIterator,
                                ullong availableBuffer, int32_t highStreamWindow)
{
    StreamFeasibility streamFeasibility =
                        isMixedStreamFeasible(currentBufLevel, audioBitrate,
                                              currentPlayoutRate, currentPlayoutTime,
                                              pTrackBufferInfo, currentStreamIterator,
                                              availableBuffer, highStreamWindow);

    if ( (streamFeasibility == UNKNOWN_FEASIBILITY) &&
            ((currentBufLevel <= (int32_t)mThroughputProbingEnterThreshold) || !mAllowBitrateProbingUnknownLocation) )
        streamFeasibility = UNFEASIBLE;

    return streamFeasibility;
}

IStreamSelector::StreamFeasibility
SimpleStreamSelector::isMixedStreamFeasible(
        int32_t currentBufLevel, uint32_t audioBitrate,
        double currentPlayoutRate, AseTimeStamp const& currentPlayoutTime,
        ITrackBufferInfoPtr pTrackBufferInfo, ITrackInfo::const_iterator currentStreamIterator,
        ullong availableBuffer, int32_t highStreamWindow)
{
    uint32_t availableThoughput;
    uint32_t connectTime;
    INetworkMonitor::PredictionSource predictionSource;
    obtainNetworkStat((*currentStreamIterator)->streamKey(), availableThoughput, connectTime,
                      predictionSource,
                      audioBitrate, currentBufLevel);

    uint32_t norminalBitrate = (*currentStreamIterator)->getAverageBitrate();

    DEBUGLOG( SimpleStreamSelectorLog,
            "isMixedStreamFeasible: throughput: %u kbps, connectTime: %u ms, "
            "currentBufLevel: %d ms, currentPlayoutRate: %f "
            "currentPlayoutTime: %llu ms, availableBuffer: %llu bytes"
            "nominal stream bitrate: %u kbps ",
            availableThoughput, connectTime,
            currentBufLevel, currentPlayoutRate,
            currentPlayoutTime.getValueInMs(), availableBuffer,
            norminalBitrate );

    ITrackInfo::const_iterator lowerBitrateStreamIterator = currentStreamIterator;
    AseTimeStamp playbackTime = currentPlayoutTime;
    int32_t newBufLevel;

    if ( currentPlayoutRate == 0.0 )
    {
        // The playback is paused
        currentPlayoutRate = 1.0;
    }

    NRDP_UNUSED(norminalBitrate);

    // a list to store the sizes of fragments the simulator virtually has downloaded
    deque<uint32_t> fragmentSizeList;

    StreamFeasibility streamFeasibility = UNFEASIBLE;

    StreamFeasibility currentStreamFeasibility;

    currentStreamFeasibility = isFeasible( availableThoughput, 0,
                currentPlayoutRate, playbackTime,
                currentBufLevel,
                pTrackBufferInfo, *currentStreamIterator,
                mLowestWatermarkLevel,
                availableBuffer,
                highStreamWindow,
                newBufLevel,
                fragmentSizeList);

    if ( currentStreamFeasibility == FEASIBLE )
    {
        if ( (newBufLevel <= currentBufLevel) &&
                (getStreamWithLowerBitrate( currentStreamIterator, lowerBitrateStreamIterator ) == SEL_OK) )
        {
            playbackTime += AseTimeStamp(highStreamWindow, 1000);

            streamFeasibility = isFeasible( availableThoughput, connectTime,
                        currentPlayoutRate, playbackTime,
                        newBufLevel,
                        pTrackBufferInfo, *lowerBitrateStreamIterator,
                        mLowestWatermarkLevel, availableBuffer,
                        mMaxSimuLength, newBufLevel,
                        fragmentSizeList);

        }
        else
        {
            streamFeasibility = FEASIBLE;
        }
    }
    else
    {
        streamFeasibility = currentStreamFeasibility;
    }

    DEBUGLOG( SimpleStreamSelectorLog,
            "isMixedStreamFeasible: feasible: %d", streamFeasibility );

    if ( (streamFeasibility == UNFEASIBLE) &&
            ((predictionSource == INetworkMonitor::LEVEL_HISTORY) || (predictionSource == INetworkMonitor::GLOBAL_HISTORY)) )
        streamFeasibility = UNKNOWN_FEASIBILITY;

    return streamFeasibility;

}


void SimpleStreamSelector::separateAVTrackInfo(
                              std::vector<TrackMetaInfo> const& trackMetaList,
                              std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                              std::vector<ITrackInfoPtr> const& trackFutureInfo,
                              TrackMetaInfo& audioTrackMeta, TrackMetaInfo& videoTrackMeta,
                              ITrackBufferInfoPtr& pAudioTrackBufferInfo,
                              ITrackBufferInfoPtr& pVideoTrackBufferInfo,
                              ITrackInfoPtr& pAudioTrackInfo,
                              ITrackInfoPtr& pVideoTrackInfo )
{
    for ( vector<TrackMetaInfo>::const_iterator iter = trackMetaList.begin();
            iter != trackMetaList.end();
            ++iter )
    {
        if ( iter->mMediaType == MEDIA_AUDIO )
            audioTrackMeta = *iter;
        else if ( iter->mMediaType == MEDIA_VIDEO )
            videoTrackMeta = *iter;
    }

    for ( vector<ITrackInfoPtr>::const_iterator iter = trackFutureInfo.begin();
            iter != trackFutureInfo.end();
            ++iter )
    {
        if ( (*iter)->mediaType() == MEDIA_AUDIO )
            pAudioTrackInfo = *iter;
        else if ( (*iter)->mediaType() == MEDIA_VIDEO )
            pVideoTrackInfo = *iter;
    }

    for ( vector<ITrackBufferInfoPtr>::const_iterator iter = trackCurrentInfo.begin();
            iter != trackCurrentInfo.end();
            ++iter )
    {
        if ( (*iter)->mediaType() == MEDIA_AUDIO )
            pAudioTrackBufferInfo = *iter;
        else if ( (*iter)->mediaType() == MEDIA_VIDEO )
            pVideoTrackBufferInfo = *iter;
    }
    return;
}

/**
 * obtainNetworkStat is used during playback. It returns available throughput for video stream
 */
void SimpleStreamSelector::obtainNetworkStat(
                            std::string const& streamKey,
                            uint32_t& availableThoughput, uint32_t& connectTime,
                            INetworkMonitor::PredictionSource& predictionSource,
                            uint32_t audioBitrate, uint32_t currentBufLevel)
{
    IStreamingLocationSetPtr pStreamingLocationSet;
    connectTime = 0;

    AseErrorCode retCode = mLocationSelectorPtr->
                                getPreferredLocationSet(streamKey,
                                                        ILocationSelector::HEURISTICS,
                                                        pStreamingLocationSet );
    if (ILocationSelector::locationSelectionFailed(retCode))
    {
        availableThoughput = MIN_INIT_THROUGHPUT;
        connectTime = 0;
        predictionSource = INetworkMonitor::SHORT_TERM;
        return;
    }

    uint32_t avgConnectTime = 0;
    uint32_t varConnectTime = 0;
    uint32_t numberOfConnectionFailures = 0;

    uint32_t totalThoughput = 0;
    double confidence;
    string locationSetKey;
    if ( pStreamingLocationSet )
    {
        locationSetKey = pStreamingLocationSet->getLocationSetKey();
        ILocationSetMonitorPtr pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();

        totalThoughput =
            mNetworkMonitorPtr->getPredictedThroughput( pLocationSetMonitor, currentBufLevel, predictionSource, confidence );

        pLocationSetMonitor->connectionStat(avgConnectTime, varConnectTime, numberOfConnectionFailures);
        if ( (confidence < mThroughputLowestConfidenceLevel) ||
                    (totalThoughput <= MIN_INIT_THROUGHPUT) )
        {
            DEBUGLOG(SimpleStreamSelectorLog, "stream: %s, location: %s, throughput: %u kbps, source %d, confidence: %f,current time: %llu",
                    streamKey.c_str(), locationSetKey.c_str(),
                    totalThoughput,
                    (int) predictionSource,
                    confidence,
                    AseTimeVal::now().seconds() );

            // the throughput measurement is too old
            totalThoughput = MIN_INIT_THROUGHPUT;
            avgConnectTime = 0;
            varConnectTime = 0;
        }
    }


    uint32_t marginThroughput;
    uint32_t bandwidthMargin = lookupBandwidthMargin(currentBufLevel);

    connectTime = avgConnectTime + 2 * (uint32_t)sqrt((float)varConnectTime);
    if ( connectTime < mConfigedMinConnectTime )
        connectTime = mConfigedMinConnectTime;

    marginThroughput = totalThoughput * (100 - bandwidthMargin) / 100 ;
    if ( marginThroughput > audioBitrate)
        availableThoughput = marginThroughput - audioBitrate;
    else
    {
        // We don't need to take additional margin if the bandwidth is so low
        availableThoughput = totalThoughput;
    }

    DEBUGLOG( SimpleStreamSelectorLog,
                "obtainNetworkStat: stream: %s, location: %s, bandwidth margin: %u, availableThoughput: %u kbps, buffer level: %u, source %d, connectTime: %u ms",
                streamKey.c_str(), locationSetKey.c_str(),
                bandwidthMargin,
                availableThoughput, currentBufLevel, (int) predictionSource, connectTime );

    return;
}

uint32_t SimpleStreamSelector::obtainThroughput( string const& streamKey,
            uint32_t& audioAllocatedThroughput, uint32_t& videoAllocatedThroughput)
{
    IStreamingLocationSetPtr pStreamingLocationSet;
    AseErrorCode retCode = mLocationSelectorPtr->
                                getPreferredLocationSet(streamKey,
                                                        ILocationSelector::HEURISTICS,
                                                        pStreamingLocationSet );

    if (ILocationSelector::locationSelectionFailed(retCode))
    {
        audioAllocatedThroughput = MIN_INIT_THROUGHPUT;
        videoAllocatedThroughput = MIN_INIT_THROUGHPUT;
        return MIN_INIT_THROUGHPUT;
    }

    uint32_t throughput = 0;
    INetworkMonitor::PredictionSource source = INetworkMonitor::SHORT_TERM;
    double confidence;
    if ( pStreamingLocationSet )
    {
        ILocationSetMonitorPtr pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();

        throughput = mNetworkMonitorPtr->getPredictedThroughput(pLocationSetMonitor, mCurrentBufferLength.getValueInMs(), source,confidence);
        if ( (confidence < mThroughputLowestConfidenceLevel) ||
                (throughput <= MIN_INIT_THROUGHPUT) )
        {
            // the throughput measurement is too old
            throughput = MIN_INIT_THROUGHPUT;
        }
    }

    uint32_t adjustedThroughput = throughput * ( 100 - lookupBandwidthMargin(mCurrentBufferLength.getValueInMs()) ) / 100;
    audioAllocatedThroughput = adjustedThroughput * mThroughputPercentForAudio / 100;
    videoAllocatedThroughput = adjustedThroughput - audioAllocatedThroughput;

    if ( adjustedThroughput <= MIN_INIT_THROUGHPUT )
    {
        audioAllocatedThroughput = (throughput > MIN_INIT_THROUGHPUT) ? throughput : MIN_INIT_THROUGHPUT;
        videoAllocatedThroughput = audioAllocatedThroughput;
    }
    DEBUGLOG( SimpleStreamSelectorLog,
                "obtainThroughput: %d kbps, source %d, video allocated throughput: %d kbps, "
                "audioAllocatedThroughput: %d kbps, current buffer level: %llu ",
                throughput, (int) source, videoAllocatedThroughput, audioAllocatedThroughput, mCurrentBufferLength.getValueInMs() );

    return throughput;
}

bool
SimpleStreamSelector::qualifyUpswitch(
            AseTimeStamp const& streamingTime,
            uint32_t currentBufLevel,
            IStreamInfoPtr pCurrentStreamInfo)
{
    uint32_t availableThoughput, connectTime;
    INetworkMonitor::PredictionSource predictionSource;
    obtainNetworkStat( pCurrentStreamInfo->streamKey(),
                      availableThoughput, connectTime,
                      predictionSource,
                      0, currentBufLevel );
    uint32_t nominalBitrate = pCurrentStreamInfo->getAverageBitrate();
    if ( (nominalBitrate > availableThoughput) &&
            (nominalBitrate > mBitrateThresholdForBufferBasedHeuristics) &&
            ((predictionSource == INetworkMonitor::SHORT_TERM) || (predictionSource == INetworkMonitor::LOCATION_HISTORY)) )
    {
        // The player already stretches based on its available throughput
        return false;
    }

    if ( (mLastSwitchState != DOWN_SWITCH) && (currentBufLevel >= mLowestBufForUpswitch) && (mHeuristicState != UNSTABLE_STREAMING) )
        return true;

    if ( (streamingTime > mLastSwitchTime + AseTimeStamp(mLockPeriodAfterDownswitch, 1000)) &&
            (currentBufLevel > mLowWatermarkLevel) )
        return true;

    return false;
}

/**
 * The player will only use a stream different from the pre-selected stream if the difference is significant
 */
bool
SimpleStreamSelector::qualifyStreamSwitch( int32_t selStreamIndex, int32_t preSelectedStreamIndex )
{
    if ( preSelectedStreamIndex < 0 )
        return true;

    bool qualifySwitch = false;
    if ( (uint32_t)selStreamIndex > preSelectedStreamIndex + mInitSelectionUpperBound )
    {
        qualifySwitch = true;
    }
    else if ( (uint32_t)preSelectedStreamIndex > selStreamIndex + mInitSelectionLowerBound )
    {
        qualifySwitch = true;
    }

    return qualifySwitch;
}

SimpleStreamSelector::StreamSelectorCode
SimpleStreamSelector::getStreamWithHigherBitrate( ITrackInfo::const_iterator currentStreamIterator,
                                                  ITrackInfo::const_iterator& newStreamIterator )
{
    ITrackInfo::const_iterator streamIterator = currentStreamIterator;
    StreamSelectorCode selCode = SEL_NOT_EXISTS;

    while ( streamIterator.hasNext() )
    {
        ++streamIterator;
        if ( (*streamIterator)->isAvailable() )
        {
            selCode = SEL_OK;
            break;
        }
    }

    if ( !(*streamIterator)->isAvailable() )
    {
        selCode = SEL_NOT_EXISTS;
        newStreamIterator = currentStreamIterator;
    }
    else
    {
        newStreamIterator = streamIterator;
    }
    return selCode;
}


SimpleStreamSelector::StreamSelectorCode
SimpleStreamSelector::getStreamWithLowerBitrate( ITrackInfo::const_iterator currentStreamIterator,
                                                 ITrackInfo::const_iterator& newStreamIterator )
{
    ITrackInfo::const_iterator streamIterator = currentStreamIterator;
    StreamSelectorCode selCode = SEL_NOT_EXISTS;
    while ( streamIterator.hasPrevious() )
    {
        --streamIterator;
        if ( (*streamIterator)->getAverageBitrate() < mMinAcceptableVideoBitrateDuringPlayback )
        {
            // can not use a bitrate lower than the minAcceptableVideoBitrate
            break;
        }

        if ( (*streamIterator)->isAvailable() )
        {
            selCode = SEL_OK;
            break;
        }
    }

    if ( selCode == SEL_OK )
        newStreamIterator = streamIterator;
    else
    {
        newStreamIterator = currentStreamIterator;
        selCode = SEL_NOT_EXISTS;
    }

    return selCode;
}

IStreamSelector::StreamFeasibility
SimpleStreamSelector::isFeasible( uint32_t availableThoughput, uint32_t connectTime,
                                  double currentPlayoutRate, AseTimeStamp const&  currentPlayoutTime,
                                  int32_t currentBufLevel,
                                  ITrackBufferInfoPtr pTrackBufferInfo, IStreamInfoPtr pStreamInfo,
                                  int32_t watermarkLevel, ullong& availableBuffer,
                                  int32_t playbackLen, int32_t& newBufLevel,
                                  deque<uint32_t>& fragmentSizeList)
{
    DEBUGLOG( SimpleStreamSelectorLog,
                    "isFeasible: throughput: %u kbps, connectTime: %u ms"
                    "currentBufLevel: %d ms, currentPlayoutRate: %f "
                    "currentPlayoutTime: %llu ms, availableBuffer: %llu bytes"
                    "nominal stream bitrate: %u kbps "
                    "playbackLen: %d, watermarkLevel: %d",
                    availableThoughput, connectTime,
                    currentBufLevel, currentPlayoutRate,
                    currentPlayoutTime.getValueInMs(), availableBuffer,
                    pStreamInfo->getAverageBitrate(), playbackLen, watermarkLevel );
    newBufLevel = currentBufLevel;
    AseTimeStamp playbackTime = currentPlayoutTime;

    if ( !isStreamEligible(pStreamInfo->getAverageBitrate()) )
    {
        return UNFEASIBLE;
    }

    if ( availableThoughput <= MIN_INIT_THROUGHPUT )
    {
        return UNKNOWN_FEASIBILITY;
    }

    if ( pStreamInfo->empty() )
    {
        // we have not downloaded the index yet
        DEBUGLOG( SimpleStreamSelectorLog, "has not downloaded the index yet");
        return isFeasibleSimple(pStreamInfo->getAverageBitrate(), pStreamInfo->streamKey(), MEDIA_VIDEO);
    }

    if ( pTrackBufferInfo->empty() )
    {
        // we have not buffered anything
        if ( (int32_t)availableThoughput <= pStreamInfo->getAverageBitrate() )
            return UNFEASIBLE;
        else
            return FEASIBLE;
    }


    StreamFeasibility streamFeasibility = FEASIBLE;

    // Find the playback and streaming positions either in pTrackBufferInfo or in pStreamInfo
    // streamingPosIter1: positon of current pending downloading tasks
    // streamingPosIter2: position of future simulated downloading tasks
    ITrackBufferInfo::const_iterator playbackPosIter1 = pTrackBufferInfo->end();
    ITrackBufferInfo::const_iterator streamingPosIter1 = pTrackBufferInfo->end();
    IStreamInfo::const_iterator playbackPosIter2 = pStreamInfo->end();
    IStreamInfo::const_iterator streamingPosIter2 = pStreamInfo->end();
    AseTimeStamp currentFragmentStartPts;
    bool endofStream = findPlaybackAndStreamingPositions(
                                                     playbackTime,
                                                     currentFragmentStartPts,
                                                     currentBufLevel,
                                                     pTrackBufferInfo, pStreamInfo,
                                                     playbackPosIter1,
                                                     playbackPosIter2,
                                                     streamingPosIter1,
                                                     streamingPosIter2);
    if ( endofStream )
    {
        // We've reached to the end of the content
        DEBUGLOG( SimpleStreamSelectorLog, "end of stream 2: feasible" );
        return FEASIBLE;
    }

    // the flag to control the draining for better throughput measurement
    bool bufferDraining = false;
    uint32_t playbackDuration;
    uint32_t downloadTime;
    ullong  bytesInCurrentFragment = 0;

    uint32_t simuCount = 0;
    uint32_t verboseLoggingCount = 0;
    int32_t prevBufLevel = currentBufLevel;

    while ( (playbackLen > 0) && ( streamingPosIter2 != pStreamInfo->end()) )
    {
        // play one fragment of video
        endofStream = playOneFragment( currentPlayoutRate,
                                       playbackTime,
                                       currentFragmentStartPts,
                                       pTrackBufferInfo, pStreamInfo,
                                       playbackPosIter1, playbackPosIter2,
                                       currentBufLevel, availableBuffer,
                                       playbackDuration,
                                       fragmentSizeList);
        // VERBOSE LOG
        if ( simuCount < verboseLoggingCount )
        {
            DEBUGLOG( SimpleStreamSelectorLog,
                "PlaybackTime:%llu ms, currentBufLevel: %d ms, playback length: %d ms, play duration: %u",
                playbackTime.getValueInMs(), currentBufLevel, playbackLen, playbackDuration);
        }

        if ( endofStream )
            break;

        playbackLen -= playbackDuration;

        if ( bufferDraining )
        {
            if ( availableBuffer > mDrainBufferSize )
            {
                // we've drained large enough, so we can start downloading again
                bufferDraining = false;
            }
        }

        if ( !bufferDraining )
        {
            // check how much video we can download for the past playbackDuration.
            // currentBufLevel and availableBuffer will be updated.
            if ( connectTime > playbackDuration )
            {
                downloadTime = 0;
                connectTime -= playbackDuration;
            }
            else
            {
                downloadTime = playbackDuration - connectTime;
                connectTime = 0;
            }

            simulateDownloading( availableThoughput, downloadTime,
                                 streamingPosIter1, streamingPosIter2,
                                 pTrackBufferInfo, pStreamInfo,
                                 currentBufLevel, availableBuffer,
                                 bytesInCurrentFragment,
                                 fragmentSizeList);
        }

        // VERBOSE LOG
        if (simuCount < verboseLoggingCount)
        {
            DEBUGLOG( SimpleStreamSelectorLog, "after downloading: currentBufLevel: %d ms", currentBufLevel);
        }

        if ( availableBuffer == 0 )
        {
            // we have used up all our buffer, so we need to drain a little bit
            // before resuming the downloading.
            bufferDraining = true;
        }

        // We finish downloading the current pending tasks before kicking the lowest water mark level rule
        if ( ((currentBufLevel < watermarkLevel) && (currentBufLevel < prevBufLevel)
                    && (simuCount > 0) && (streamingPosIter1 == pTrackBufferInfo->end()) ) ||
                (currentBufLevel < 1000) )
        {
            // This stream is not feasible
            DEBUGLOG( SimpleStreamSelectorLog, "currentBufLevel: %d, watermarkLevel: %d, prevBufLevel: %d",
                        currentBufLevel, watermarkLevel, prevBufLevel );
            streamFeasibility = UNFEASIBLE;
            break;
        }
        prevBufLevel = currentBufLevel;
        ++simuCount;

    }

    newBufLevel = currentBufLevel;
    DEBUGLOG( SimpleStreamSelectorLog,
            "isFeasible: feasible: %d, availableBuffer: %llu bytes, newBufLevel: %d",
                    streamFeasibility, availableBuffer, newBufLevel );
    return streamFeasibility;
}

bool SimpleStreamSelector::findPlaybackAndStreamingPositions(
                                            AseTimeStamp const& currentPlayoutTime,
                                            AseTimeStamp& currentFragmentStartPts,
                                            int32_t currentBufLevel,
                                            ITrackBufferInfoPtr pTrackBufferInfo,
                                            IStreamInfoPtr pStreamInfo,
                                            ITrackBufferInfo::const_iterator& playbackPosIter1,
                                            IStreamInfo::const_iterator& playbackPosIter2,
                                            ITrackBufferInfo::const_iterator& streamingPosIter1,
                                            IStreamInfo::const_iterator& streamingPosIter2)
{
    bool endofStream = false;
    playbackPosIter1 = pTrackBufferInfo->end();
    playbackPosIter2 = pStreamInfo->end();
    streamingPosIter1 = pTrackBufferInfo->end();
    streamingPosIter2 = pStreamInfo->end();

    searchTrackBufferInfo( currentPlayoutTime,
                           pTrackBufferInfo->getFirstFragmentStartTime(),
                           pTrackBufferInfo->begin(), pTrackBufferInfo->end(),
                           currentFragmentStartPts, playbackPosIter1 );


    AseTimeStamp streamingTime = currentPlayoutTime + AseTimeStamp(currentBufLevel, 1000);
    AseTimeStamp streamingFragmentStartPts, tmpPts;
    if ( playbackPosIter1 != pTrackBufferInfo->end() )
    {
        // the playback position is at pTrackBufferInfo
        searchTrackBufferInfo( streamingTime,
                               currentFragmentStartPts,
                               playbackPosIter1, pTrackBufferInfo->end(),
                               streamingFragmentStartPts, streamingPosIter1 );
        if ( streamingPosIter1 == pTrackBufferInfo->end() )
        {
            searchStreamInfo( streamingTime,
                              pStreamInfo->firstFragmentTime(),
                              pStreamInfo->begin(),
                              pStreamInfo->end(),
                              tmpPts,
                              streamingPosIter2 );
        }
        else
        {
            streamingPosIter2 = pStreamInfo->begin();
        }
    }
    else
    {
        // the playback postion is at pStreamInfo
        searchStreamInfo( currentPlayoutTime,
                          pStreamInfo->firstFragmentTime(),
                          pStreamInfo->begin(),
                          pStreamInfo->end(),
                          currentFragmentStartPts,
                          playbackPosIter2 );
        if ( playbackPosIter2 != pStreamInfo->end() )
        {
            searchStreamInfo( streamingTime,
                              currentFragmentStartPts,
                              playbackPosIter2,
                              pStreamInfo->end(),
                              tmpPts,
                              streamingPosIter2 );
        }

        if ( (playbackPosIter2 == pStreamInfo->end()) || (streamingPosIter2 == pStreamInfo->end()) )
        {
            endofStream = true;
        }
    }

    return endofStream;
}

// [fragmentStartPts, fragmentEndPts]
// find the fragment, which satisfies fragmentStartPts < targetTime <= fragmentEndPts
void SimpleStreamSelector::searchTrackBufferInfo( AseTimeStamp const& targetTime,
                             AseTimeStamp const& startFragmentPts,
                             ITrackBufferInfo::const_iterator startIter,
                             ITrackBufferInfo::const_iterator endIter,
                             AseTimeStamp& targetFragmentStartPts,
                             ITrackBufferInfo::const_iterator& targetFragmentIter )
{
    targetFragmentStartPts = startFragmentPts;
    AseTimeStamp nextFragmentStartPts = startFragmentPts;

    targetFragmentIter = endIter;
    for ( ITrackBufferInfo::const_iterator bufferFragmentIter = startIter;
            bufferFragmentIter != endIter;
            ++bufferFragmentIter )
    {
        targetFragmentStartPts = nextFragmentStartPts;
        nextFragmentStartPts += (*bufferFragmentIter).mDuration;
        if ( nextFragmentStartPts > targetTime )
        {
            targetFragmentIter = bufferFragmentIter;
            break;
        }
    }

    if ( targetFragmentIter == endIter )
        targetFragmentStartPts = nextFragmentStartPts;

/**
    DEBUGLOG( SimpleStreamSelectorLog,
            "searchTrackBufferInfo: targetTime: %llu ms, "
            "startFragmentPts: %llu ms, targetFragmentStartPts: %llu ms, ",
            targetTime.getValueInMs(), startFragmentPts.getValueInMs(),
            targetFragmentStartPts.getValueInMs() );
*/
    return;
}

void SimpleStreamSelector::searchStreamInfo( AseTimeStamp const& targetTime,
                                             AseTimeStamp const& startFragmentPts,
                                             IStreamInfo::const_iterator startIter,
                                             IStreamInfo::const_iterator endIter,
                                             AseTimeStamp& targetFragmentStartPts,
                                             IStreamInfo::const_iterator& targetFragmentIter )
{
    targetFragmentStartPts = startFragmentPts;
    AseTimeStamp nextFragmentStartPts = startFragmentPts;

    targetFragmentIter = endIter;
    for ( IStreamInfo::const_iterator fragmentIter = startIter;
            fragmentIter != endIter;
            ++fragmentIter )
    {
        targetFragmentStartPts = nextFragmentStartPts;
        nextFragmentStartPts += (*fragmentIter).mDuration;
        if ( nextFragmentStartPts > targetTime )
        {
            targetFragmentIter = fragmentIter;
            break;
        }
    }

    if ( targetFragmentIter == endIter )
        targetFragmentStartPts = nextFragmentStartPts;

/**
    DEBUGLOG( SimpleStreamSelectorLog,
            "searchStreamInfo: targetTime: %llu ms, "
            "startFragmentPts: %llu ms, targetFragmentStartPts: %llu ms, ",
            targetTime.getValueInMs(), startFragmentPts.getValueInMs(),
            targetFragmentStartPts.getValueInMs() );
*/
    return;
}

bool SimpleStreamSelector::playOneFragment(
                          double currentPlayoutRate, AseTimeStamp& currentPlayoutTime,
                          AseTimeStamp& currentFragmentStartPts,
                          ITrackBufferInfoPtr pTrackBufferInfo, IStreamInfoPtr pStreamInfo,
                          ITrackBufferInfo::const_iterator& playbackPosIter1,
                          IStreamInfo::const_iterator& playbackPosIter2,
                          int32_t& currentBufLevel,
                          ullong& availableBuffer,
                          uint32_t& playbackDuration,
                          deque<uint32_t>& fragmentSizeList)
{
    AseTimeStamp completeDuration, nextFragmentStartPts;

    AseTimeStamp fragmentDuration;
    uint32_t fragmentSize;
    if ( playbackPosIter1 != pTrackBufferInfo->end() )
    {
        fragmentDuration = (*playbackPosIter1).mDuration;
        fragmentSize = (*playbackPosIter1).mSize;
        ++playbackPosIter1;
        if ( playbackPosIter1 == pTrackBufferInfo->end() )
            playbackPosIter2 = pStreamInfo->begin();
    }
    else
    {
        fragmentDuration = (*playbackPosIter2).mDuration;
        if ( !fragmentSizeList.empty() )
        {
            fragmentSize = fragmentSizeList.front();
            fragmentSizeList.pop_front();
        }
        else
        {
            fragmentSize = (*playbackPosIter2).mSize;
        }
        ++playbackPosIter2;
    }

    nextFragmentStartPts = currentFragmentStartPts + fragmentDuration;
    completeDuration = nextFragmentStartPts - currentPlayoutTime;
    currentFragmentStartPts = nextFragmentStartPts;
    currentPlayoutTime = nextFragmentStartPts;
    if ( fragmentDuration.getValueInMs() > 0 )
        availableBuffer += fragmentSize * completeDuration.getValueInMs() / fragmentDuration.getValueInMs();
    currentBufLevel -= completeDuration.getValueInMs();
    playbackDuration = completeDuration.getValueInMs() / (uint32_t)currentPlayoutRate;

    if ( (playbackPosIter1 == pTrackBufferInfo->end()) &&
            (playbackPosIter2 == pStreamInfo->end()) )
        return true;
    else
        return false;

}

void SimpleStreamSelector::simulateDownloading(
                              uint32_t availableThoughput, uint32_t downloadTime,
                              ITrackBufferInfo::const_iterator& streamingPosIter1,
                              IStreamInfo::const_iterator& streamingPosIter2,
                              ITrackBufferInfoPtr pTrackBufferInfo, IStreamInfoPtr pStreamInfo,
                              int32_t& currentBufLevel,
                              ullong& availableBuffer,
                              ullong& bytesInCurrentFragment,
                              deque<uint32_t>& fragmentSizeList)
{
    ullong bytesDownloaded = availableThoughput / 8 * downloadTime;
    if (availableBuffer >= bytesDownloaded)
    {
        availableBuffer -= bytesDownloaded;
    }
    else
    {
        bytesDownloaded = availableBuffer;
        availableBuffer = 0;
    }

    bytesInCurrentFragment += bytesDownloaded;
    ullong remaining;

    while ( streamingPosIter1 != pTrackBufferInfo->end() )
    {
        remaining = (*streamingPosIter1).mSize - (*streamingPosIter1).mReceived;
        if ( bytesInCurrentFragment >= remaining )
        {
            bytesInCurrentFragment -= remaining;
            currentBufLevel += ((*streamingPosIter1).mDuration).getValueInMs();
        }
        else
        {
            break;
        }
        ++streamingPosIter1;
    }

    if ( streamingPosIter1 == pTrackBufferInfo->end() )
    {
        while ( streamingPosIter2 != pStreamInfo->end() )
        {
            if ( (llong)bytesInCurrentFragment >= (*streamingPosIter2).mSize )
            {
                bytesInCurrentFragment -= (*streamingPosIter2).mSize;
                currentBufLevel += ((*streamingPosIter2).mDuration).getValueInMs();
                fragmentSizeList.push_back( (*streamingPosIter2).mSize );
            }
            else
            {
                break;
            }
            ++streamingPosIter2;
        }
    }
    return;
}

AseTimeStamp SimpleStreamSelector::getNextFragmentTime( ITrackInfoPtr pTrackInfo )
{
    ITrackInfo::const_iterator streamInfoIter = pTrackInfo->current();
    if ( streamInfoIter == pTrackInfo->end() )
        streamInfoIter = pTrackInfo->begin();

    IStreamInfoPtr pStreamInfo = *streamInfoIter;
    return pStreamInfo->currentFragmentTime();
}

void SimpleStreamSelector::loadConfigParas()
{
    mMaxBufferingAmountAllowed = obtainParameter( "maxBufferingAllowed", 30000, MAX_INT32, defaultMaxBufferingAmountAllowed );
    mBandwidthMargin = obtainParameter( "bandwidthMargin", 0, MAX_BW_MARGIN, defaultBandwidthMargin );
    mMinPrebufLength = obtainParameter( "minPrebufSize", 0, mMaxBufferingAmountAllowed, DEFAULT_MIN_PREBUF_SIZE);
    mRequiredBufferingLen = mMinPrebufLength;

    mLowestWatermarkLevel = obtainParameter( "lowestWatermarkLevel", 0, MAX_INT32, defaultLowestWatermarkLevel);
    mLowWatermarkLevel = obtainParameter( "lowWatermarkLevel", 0, MAX_INT32, DEFAULT_LOW_WATERMARK_LEVEL );
    mHighWatermarkLevel = obtainParameter( "highWatermarkLevel", 0, MAX_INT32, defaultHighWatermarkLevel );
    mWatermarkLevelForSkipStart = obtainParameter( "watermarkLevelForSkipStart", 0, MAX_INT32, defaultWaterMarkLevelForSkipStart);
    mLockPeriodAfterDownswitch = obtainParameter("lockPeriodAfterDownswitch", 0, MAX_INT32, defaultLockPeriodAfterDownswitch);

    mHighStreamRetentionWindow
        = obtainParameter( "highStreamRetentionWindow", MIN_HIGH_STREAM_WINDOW, MAX_INT32, defaultHighStreamRetentionWindow);
    mHighStreamTransitionWindow
        = obtainParameter( "highStreamTransitionWindow", MIN_HIGH_STREAM_WINDOW, MAX_INT32, defaultHighStreamTransitionWindow);
    mMinInitVideoBitrate =
        mConfigParameterPtr->getParameterAs<int32_t>("minInitVideoBitrate", -1);
    mMaxInitVideoBitrate =
        mConfigParameterPtr->getParameterAs<int32_t>("maxInitVideoBitrate", -1);
    mMinAcceptableVideoBitrate =
        mConfigParameterPtr->getParameterAs<int32_t>("minAcceptableVideoBitrate", -1);
    mMaxSimuLength = obtainParameter("maxSimuLength", mHighStreamTransitionWindow, MAX_INT32, defaultMaxSimuLength );

    mThroughputLowestConfidenceLevel = mConfigParameterPtr->getParameterAs<double>(
                                    "throughputLowestConfidenceLevel", 0.2);

    mThroughputPercentForAudio = mConfigParameterPtr->getParameterAs<uint32_t>(
                                    "throughputPercentForAudio", 15);

    mConfigedMinConnectTime = mConfigParameterPtr->getParameterAs<uint32_t>(
                                    "configedMinConnectTime", 250);
    mSkipBitrateInUpswitch = mConfigParameterPtr->getParameterAs<bool>(
                                    "skipBitrateInUpswitch", false);

    mBitrateThresholdForBufferBasedHeuristics = mConfigParameterPtr->getParameterAs<uint32_t>(
                                    "bitrateThresholdForBufferBasedHeuristics", 520);

    mAllowBitrateProbingUnknownLocation = mConfigParameterPtr->getParameterAs<bool>(
                                    "allowBitrateProbingUnknownLocation", false);


    mMinRequiredBuffer = obtainParameter( "minRequiredBuffer", 8000, MAX_INT32, defaultMinRequiredBuffer );

    mDrainBufferSize = obtainParameter( "mediaBufferTargetAvailableSize", 0, MAX_INT32, defaultDrainBufferSize );
    uint32_t totalBufferSize = mLocalConfigParametePtr->getParameterAs<uint32_t>("totalBufferSize", defaultTotalBufferSize);
    uint32_t configedBufferSize = mConfigParameterPtr->getParameterAs<uint32_t>("configedVideoBufferSize", -1);
    mTotalBufferSize = (totalBufferSize > configedBufferSize) ? configedBufferSize : totalBufferSize;

    mMaxBufferingTime = obtainParameter( "maxBufferingTime", 0, MAX_INT32, defaultMaxBufferingTime );

    mThroughputProbingEnterThreshold =
                mConfigParameterPtr->getParameterAs<uint32_t>("throughputProbingEnterThreshold", DEFAULT_THROUGHPUT_ENTER_THRESHOLD);

    mPenaltyFactorForLongConnectTime =
                mConfigParameterPtr->getParameterAs<uint32_t>("penaltyFactorForLongConnectTime", 2);
    mLongConnectTimeThreshold =
                mConfigParameterPtr->getParameterAs<uint32_t>("longConnectTimeThreshold", 200);
    mAdditionalBufferingPerFailure =
                mConfigParameterPtr->getParameterAs<uint32_t>("additionalBufferingPerFailure", 8000);
    mAdditionalBufferingLongConnectTime =
                mConfigParameterPtr->getParameterAs<uint32_t>("additionalBufferingLongConnectTime", 2000);

    mToStableThreshold =
                mConfigParameterPtr->getParameterAs<uint32_t>("toStableThreshold", 30000);
    mToUnstableThreshold =
                mConfigParameterPtr->getParameterAs<uint32_t>("toUnstableThreshold", 15000);
    mLowestBufForUpswitch =
                mConfigParameterPtr->getParameterAs<uint32_t>("lowestBufForUpSwitch", 9000);

    mFeasibilitySleepMicros =
                mConfigParameterPtr->getParameterAs<int>("feasibilitySleepMicros", -1);

    mInitSelectionLowerBound =
                mConfigParameterPtr->getParameterAs<uint32_t>("initSelectionLowerBound", 0);
    mInitSelectionUpperBound =
                mConfigParameterPtr->getParameterAs<uint32_t>("initSelectionUpperBound", 2);

    mBandwidthMarginMap = mConfigParameterPtr->loadConfigMap("bandwidthMarginCurve", "b", "m");
    mInitialBitrateSelectionCurve = mConfigParameterPtr->loadConfigMap("initialBitrateSelectionCurve", "r", "d");

    mEnableGoodQualityBuffering =
                mConfigParameterPtr->getParameterAs<bool>("enableGoodQualityBuffering", false);
}

uint32_t SimpleStreamSelector::obtainParameter( std::string const& key, uint32_t min, uint32_t max, uint32_t defaultVal )
{
    string paraValue;
    paraValue = mConfigParameterPtr->queryParameter(key);
    uint32_t retValue;
    if ( fromString<uint32_t>( retValue, paraValue, std::dec ) )
    {
        validateValue<uint32_t>( retValue, min, max, defaultVal );
    }
    else
    {
        retValue = defaultVal;
    }
    return retValue;
}

uint32_t SimpleStreamSelector::adjustRequiredBufferingLen(IStreamInfoPtr pStreamInfo, uint32_t requiredBufferingLen)
{
    IStreamingLocationSetPtr pStreamingLocationSet;
    AseErrorCode retCode = pStreamInfo->getPreferredLocationSet(pStreamingLocationSet);
    if (ILocationSelector::locationSelectionFailed(retCode))
    {
        return mMaxBufferingAmountAllowed;
    }

    uint32_t avgConnectTime = 0;
    uint32_t varConnectTime = 0;
    uint32_t numberOfConnectionFailures = 0;

    if ( pStreamingLocationSet )
    {
        ILocationSetMonitorPtr pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();
        pLocationSetMonitor->connectionStat(avgConnectTime, varConnectTime, numberOfConnectionFailures);
    }

    uint32_t additionalBufferingTime = mAdditionalBufferingLongConnectTime;

    if ( avgConnectTime < mLongConnectTimeThreshold )
    {
        avgConnectTime = 0;
        additionalBufferingTime = 0;
    }

    uint32_t adjustedRequiredBufferingLen = requiredBufferingLen + additionalBufferingTime +
        avgConnectTime * mPenaltyFactorForLongConnectTime + mAdditionalBufferingPerFailure * numberOfConnectionFailures;

    if ( adjustedRequiredBufferingLen > mMaxBufferingAmountAllowed )
        adjustedRequiredBufferingLen = mMaxBufferingAmountAllowed;

    DEBUGLOG(SimpleStreamSelectorLog,
            "adjustedRequiredBufferingLen: connect time: %u, number of failures: %u, "
            "before: %u, after: %u",
            avgConnectTime, numberOfConnectionFailures, requiredBufferingLen, adjustedRequiredBufferingLen);

    return adjustedRequiredBufferingLen;
}

void SimpleStreamSelector::computeRequiredBuffering( uint32_t throughput, uint32_t connectTime,
                                   AseTimeStamp const& currentPlayoutTime,
                                   ITrackBufferInfoPtr pTrackBufferInfo,
                                   ITrackInfo::const_iterator currentStreamIterator,
                                   ullong availableBuffer, int32_t simuLen,
                                   uint32_t currentBufferingLen,
                                   uint32_t& requiredBufferingLen)
{

    uint32_t minBufferLen = std::max(currentBufferingLen, mMinPrebufLength);
    uint32_t maxBufferLen = mMaxBufferingAmountAllowed;
    uint32_t midBufferLen;
    int32_t newBufLevel;

    StreamFeasibility feasiblity = UNFEASIBLE;

    deque<uint32_t> fragmentSizeList;

    while ( maxBufferLen > minBufferLen + 2000 )
    {
        fragmentSizeList.clear();
        midBufferLen = (maxBufferLen + minBufferLen) / 2;
        feasiblity = isFeasible( throughput, connectTime, 1.0, currentPlayoutTime,
                                 midBufferLen,
                                 pTrackBufferInfo, *currentStreamIterator,
                                 mLowestWatermarkLevel,
                                 availableBuffer,
                                 simuLen,
                                 newBufLevel,
                                 fragmentSizeList);
        if ( feasiblity == FEASIBLE )
        {
            maxBufferLen = midBufferLen;
        }
        else
        {
            minBufferLen = midBufferLen;
        }
    }

    if ( feasiblity == FEASIBLE )
        requiredBufferingLen = minBufferLen;
    else
        requiredBufferingLen = maxBufferLen;

    requiredBufferingLen = adjustRequiredBufferingLen(*currentStreamIterator, requiredBufferingLen);
}

bool
SimpleStreamSelector::isStreamEligible(uint32_t nominalBitrate)
{
    uint32_t bufferCapacity = mTotalBufferSize / nominalBitrate * 8;
    return (bufferCapacity > mMinRequiredBuffer);
}


IStreamSelector::StreamFeasibility
SimpleStreamSelector::isFeasibleSimple(uint32_t nominalBitrate, std::string const& streamKey, MediaType mediaType)
{
    if ( !isStreamEligible(nominalBitrate) )
        return UNFEASIBLE;

    uint32_t audioAllocatedThroughput, videoAllocatedThroughput;
    obtainThroughput(streamKey, audioAllocatedThroughput, videoAllocatedThroughput);

    uint32_t availableThoughput;
    if (mediaType == MEDIA_AUDIO)
        availableThoughput = audioAllocatedThroughput;
    else
        availableThoughput = videoAllocatedThroughput;

    if ( availableThoughput <= MIN_INIT_THROUGHPUT )
        return UNKNOWN_FEASIBILITY;

    StreamFeasibility feasibility = UNFEASIBLE;
    if (availableThoughput > nominalBitrate)
        feasibility = FEASIBLE;

    return feasibility;
}

IStreamSelector::StreamFeasibility
SimpleStreamSelector::isFeasibleForFirstStream(uint32_t selectedBitrate, string const& streamKey, uint32_t availableThoughput)
{
    if ( (mMaxInitVideoBitrate > 0) && ((int32_t)selectedBitrate > mMaxInitVideoBitrate) )
        return UNFEASIBLE;

    if ( !isStreamEligible(selectedBitrate) )
        return UNFEASIBLE;

    StreamFeasibility feasibility = UNFEASIBLE;
    int32_t initVideoBitrate = max(mMinInitVideoBitrate, mMinAcceptableVideoBitrate);

    if ( (initVideoBitrate > 0) && ( (int32_t)selectedBitrate <= initVideoBitrate ) )
    {
        // enforce the minimum init video bitrate selection when the player either has good throughput
        // or does not have throughput estimate
        if ( (availableThoughput > selectedBitrate) || (availableThoughput <= MIN_INIT_THROUGHPUT) )
        {
            feasibility = FEASIBLE;
        }
        else
        {
            feasibility = UNFEASIBLE;
        }
    }
    else if ( mEnableGoodQualityBuffering )
    {
        feasibility = isFeasibleSimple(selectedBitrate, streamKey, MEDIA_VIDEO);
    }
    else
    {
        uint32_t maxBufferingTime = mMaxBufferingTime;
        uint32_t bufferingTime = selectedBitrate * mMinPrebufLength * 3 / 2 / availableThoughput;
        if ( !mInitialBitrateSelectionCurve.empty() )
            maxBufferingTime = lookupMaxBufferingTime(selectedBitrate);
        if ( bufferingTime < maxBufferingTime )
            feasibility = FEASIBLE;
        DEBUGLOG(SimpleStreamSelectorLog,
                "isFeasibleForFirstStream: bitrate: %u, throughput: %u kbps, buffering time, %u:%u, feasible: %u",
                selectedBitrate, availableThoughput,
                bufferingTime, maxBufferingTime, feasibility);
    }

    return feasibility;
}

uint32_t SimpleStreamSelector::estimateNumberOfRebuffers(
                                       uint32_t bandwidth,
                                       uint32_t avgBitrate,
                                       uint32_t duration,
                                       uint32_t maxBufferingLen)
{
    if ( bandwidth > avgBitrate )
        return 1;

    ullong bitrateDurationProduct = (avgBitrate - bandwidth) * (ullong)duration;
    ullong bitrateBufferingProduct = avgBitrate * (ullong)maxBufferingLen;
    ullong predictedNumRebuffers = bitrateDurationProduct / bitrateBufferingProduct;
    if (predictedNumRebuffers < 1)
        predictedNumRebuffers = 1;
    return (uint32_t)predictedNumRebuffers;
}


IStreamSelector::BufferingStatus
SimpleStreamSelector::selectVideoStreamWithoutSkipping(
                        ITrackInfo::const_iterator& videoStreamIterator,
                        StreamFeasibility& streamFeasibility,
                        uint32_t currentBufLevel,
                        uint32_t audioBitrate,
                        double currentPlayoutRate,
                        AseTimeStamp const& currentPlayoutTime,
                        ITrackBufferInfoPtr pVideoTrackBufferInfo,
                        ITrackInfoPtr pVideoTrackInfo,
                        ullong availableBuffer)
{
    BufferingStatus bufferingStatus = BUFFER_LOW;
    ITrackInfo::const_iterator newVideoStreamIter = videoStreamIterator;

    StreamFeasibility currentStreamFeasibility;

    currentStreamFeasibility = isMixedStreamFeasibleWrapper(
                currentBufLevel, audioBitrate,
                currentPlayoutRate, currentPlayoutTime,
                pVideoTrackBufferInfo, videoStreamIterator,
                availableBuffer, mHighStreamRetentionWindow);
    if ( currentStreamFeasibility == FEASIBLE )
    {
        DEBUGLOG( SimpleStreamSelectorLog, "1" );
        bufferingStatus = BUFFER_HAVE_ENOUGH;
        streamFeasibility = FEASIBLE;
        // mixed stream with the current stream is feasible. Try to upswitch
        SimpleStreamSelector::StreamSelectorCode selectorCode =
            getStreamWithHigherBitrate(videoStreamIterator, newVideoStreamIter);


        StreamFeasibility highStreamFeasibility;
        if ( qualifyUpswitch(getNextFragmentTime(pVideoTrackInfo),
                    currentBufLevel, *videoStreamIterator) &&
                ( selectorCode == SEL_OK) && !((*newVideoStreamIter)->empty()) )
        {
            highStreamFeasibility = isMixedStreamFeasibleWrapper(
                        currentBufLevel, audioBitrate,
                        currentPlayoutRate, currentPlayoutTime,
                        pVideoTrackBufferInfo, newVideoStreamIter,
                        availableBuffer, mHighStreamTransitionWindow);
            if ( highStreamFeasibility != UNFEASIBLE )
            {
                DEBUGLOG( SimpleStreamSelectorLog, "2" );
                videoStreamIterator = newVideoStreamIter;
                mLastSwitchState = UP_SWITCH;

                streamFeasibility = highStreamFeasibility;
            }
            else
            {
                mGoodQuality = true;
            }
        }

        if ( selectorCode != SEL_OK )
            mGoodQuality = true;
    }
    else if ( currentStreamFeasibility == UNKNOWN_FEASIBILITY )
    {
        streamFeasibility = currentStreamFeasibility;
    }
    else
    {
        streamFeasibility = UNFEASIBLE;
        // The player does not downswitch in low grade mode
        // mixed stream with the current stream is not feasible. Try to downswitch
        while ( getStreamWithLowerBitrate(videoStreamIterator, newVideoStreamIter) == SEL_OK )
        {
            videoStreamIterator = newVideoStreamIter;
            if ( isMixedStreamFeasibleWrapper(
                        currentBufLevel, audioBitrate,
                        currentPlayoutRate, currentPlayoutTime,
                        pVideoTrackBufferInfo, newVideoStreamIter,
                        availableBuffer, mHighStreamTransitionWindow
                        ) == FEASIBLE )
            {
                DEBUGLOG( SimpleStreamSelectorLog, "3" );
                // find a feasible stream with lower bitrate
                bufferingStatus = BUFFER_HAVE_ENOUGH;
                streamFeasibility = FEASIBLE;
                break;
            }
        }
        mLastSwitchTime = getNextFragmentTime(pVideoTrackInfo);
        mLastSwitchState = DOWN_SWITCH;
    }

    return bufferingStatus;
}

IStreamSelector::BufferingStatus SimpleStreamSelector::selectVideoStreamWithSkipping(
                        ITrackInfo::const_iterator& videoStreamIterator,
                        StreamFeasibility& streamFeasibility,
                        uint32_t currentBufLevel,
                        uint32_t audioBitrate,
                        double currentPlayoutRate,
                        AseTimeStamp const& currentPlayoutTime,
                        ITrackBufferInfoPtr pVideoTrackBufferInfo,
                        ITrackInfoPtr pVideoTrackInfo,
                        ullong availableBuffer)
{
    BufferingStatus bufferingStatus = BUFFER_LOW;
    int32_t currentStreamId = (*videoStreamIterator)->streamId();
    // always jumps to the good quality
    mGoodQuality = true;

    ITrackInfo::const_iterator streamInfoIter = pVideoTrackInfo->begin();
    bool firstAvailableStream = true;
    ITrackInfo::const_iterator selVideoStreamIter = videoStreamIterator;
    bool selectUnknownStream = false;
    ITrackInfo::const_iterator selUnknownVideoStreamIter = videoStreamIterator;
    uint32_t highStreamWindow;

    StreamFeasibility currentStreamFeasibility;

    for ( streamInfoIter = pVideoTrackInfo->begin(); streamInfoIter != pVideoTrackInfo->end(); ++streamInfoIter )
    {
        uint32_t averageBitrate = (*streamInfoIter)->getAverageBitrate();
        // evaluate each stream one by one
        if ( (*streamInfoIter)->isAvailable() && ((int32_t)averageBitrate >= mMinAcceptableVideoBitrateDuringPlayback) )
        {
            if ( firstAvailableStream )
            {
                // record the first available stream
                firstAvailableStream = false;
                selVideoStreamIter = streamInfoIter;

            }

            if ( (*streamInfoIter)->streamId() == currentStreamId )
            {
                highStreamWindow = mHighStreamRetentionWindow;
            }
            else
            {
                highStreamWindow = mHighStreamTransitionWindow;
            }

            currentStreamFeasibility = isMixedStreamFeasibleWrapper(
                                            currentBufLevel, audioBitrate,
                                            currentPlayoutRate, currentPlayoutTime,
                                            pVideoTrackBufferInfo, streamInfoIter,
                                            availableBuffer, highStreamWindow);
            switch ( currentStreamFeasibility )
            {
                case FEASIBLE:
                    selVideoStreamIter = streamInfoIter;
                    bufferingStatus = BUFFER_HAVE_ENOUGH;

                    streamFeasibility = FEASIBLE;
                    selectUnknownStream = false;
                    break;

                case UNFEASIBLE:
                    break;
                case UNKNOWN_FEASIBILITY:
                    if ( ((*streamInfoIter)->getAverageBitrate() > (*selVideoStreamIter)->getAverageBitrate()) &&
                            !selectUnknownStream )
                    {
                        // The player only selects the first unknown stream with higher bitrate than the current feasible stream
                        selectUnknownStream = true;
                        selUnknownVideoStreamIter = streamInfoIter;
                        bufferingStatus = BUFFER_HAVE_ENOUGH;

                        streamFeasibility = currentStreamFeasibility;
                    }
                    break;
                default:
                    break;
            }

            if ( mHeuristicState == UNSTABLE_STREAMING )
            {
                // select the lowest bitrate stream which is available
                DEBUGLOG( SimpleStreamSelectorLog, "unstable streaming state: select the lowest bitrate stream which is available" );
                streamFeasibility = currentStreamFeasibility;
                selVideoStreamIter = streamInfoIter;
                if ( streamFeasibility == UNFEASIBLE )
                    bufferingStatus = BUFFER_LOW;
                break;
            }
        }
    }

    if ( selectUnknownStream )
        videoStreamIterator = selUnknownVideoStreamIter;
    else
        videoStreamIterator = selVideoStreamIter;

    return bufferingStatus;
}

AseTimeInterval
SimpleStreamSelector::calculateBufferLen( vector<TrackMetaInfo> const& trackMetaList )
{
    // obtain the minimum buffer length among all tracks
    AseTimeInterval minBuffer = AseTimeStamp(1000 * 1000, 1);
    for ( vector<TrackMetaInfo>::const_iterator iter = trackMetaList.begin();
            iter != trackMetaList.end();
            ++iter )
    {
        if ( iter->mCompleteBufferLen < minBuffer )
        {
            minBuffer = iter->mCompleteBufferLen;
        }
    }

    return minBuffer;
}

bool
SimpleStreamSelector::canSkipVideoStream(uint32_t bufferLevel)
{
    bool qualify = false;
    if ( mSkipBitrateInUpswitch && (bufferLevel > mWatermarkLevelForSkipStart))
        qualify = true;

    if ( mHeuristicState == UNSTABLE_STREAMING )
        qualify = true;

    return qualify;
}

uint32_t
SimpleStreamSelector::lookupBandwidthMargin(uint32_t bufferLevel)
{
    uint32_t bandwidthMargin;
    if ( mBandwidthMarginMap.empty() )
    {
        if ( bufferLevel > mHighWatermarkLevel )
        {
            // safe zone
            bandwidthMargin = mBandwidthMargin >> 1;
        }
        else
        {
            bandwidthMargin = mBandwidthMargin;
        }
    }
    else
    {
        map<uint32_t, uint32_t>::iterator bandwidthMarginIter = mBandwidthMarginMap.upper_bound(bufferLevel);
        if ( bandwidthMarginIter != mBandwidthMarginMap.end() )
            bandwidthMargin = bandwidthMarginIter->second;
        else
            bandwidthMargin = mBandwidthMarginMap.rbegin()->second;
    }

    return bandwidthMargin;
}

uint32_t
SimpleStreamSelector::lookupMaxBufferingTime(uint32_t bitrate)
{
    map<uint32_t, uint32_t>::iterator bitrateIter = mInitialBitrateSelectionCurve.upper_bound(bitrate);

    uint32_t bufferingTime;
    if ( bitrateIter != mInitialBitrateSelectionCurve.end() )
        bufferingTime = bitrateIter->second;
    else
        bufferingTime = mInitialBitrateSelectionCurve.rbegin()->second;

    return bufferingTime;
}
