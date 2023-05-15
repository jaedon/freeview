/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "McMediaSink.h"

#include <algorithm>

#include <nrdbase/LexicalCast.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/Log.h>
#include <nrdbase/Time.h>

#include <nrdase/Manifest.h>

using namespace std;
using namespace netflix;
using namespace netflix::ase;
using namespace netflix::mediacontrol;

static const Time QUEUE_STATE_LOG_INTERVAL(2000);

AdaptiveStreamingPlayer::
    McMediaSink::McMediaSink(
            const shared_ptr<const Manifest> pManifest,
            IAseConfigParameterPtr pConfigParameter,
            IAseConfigParameterPtr pLocalConfigParameter,
            bool stereoVideo) :
      mMutex(MCMEDIASINK_MUTEX, "McMediaSink"),
      mConfigParameterPtr(pConfigParameter),
      mLocalConfigParameterPtr(pLocalConfigParameter),
      mIsStereoVideo(stereoVideo),
      mCurrentVideoPlaybackPos(-1),
      mCurrentAudioPlaybackPos(-1),
      mNextVideoDeliveryPos(-1),
      mNextAudioDeliveryPos(-1),
      mVideoTrailingBufferSize(0),
      mAudioTrailingBufferSize(0),
      mCurrentVideoGopStartPts(0),
      mCurrentAudioGopStartPts(0),
      mNumDeliverCallsSinceLastLog(0),
      mNumReadBlockCallsSinceLastLog(0)
{
    mCurrentPlayoutTime = AseTimeStamp::ZERO;
    mLastBufferUpdateTime = AseTimeStamp::ZERO;
    mManifestMap[pManifest->getManifestIndex()] = pManifest;
    loadConfigParas();
}

AdaptiveStreamingPlayer::McMediaSink::~McMediaSink()
{
    flush();
}

NFErr AdaptiveStreamingPlayer::McMediaSink::addManifest(
        shared_ptr<const Manifest> pManifest)
{
    mManifestMap[pManifest->getManifestIndex()] = pManifest;
    return NFErr_OK;
}

bool AdaptiveStreamingPlayer::McMediaSink::getFirstSelStream(
           uint32_t manifestIndex,
    /*out*/uint32_t &firstSelTrackIndex,
    /*out*/uint32_t &firstSelStreamIndex,
    /*out*/shared_ptr<vector<unsigned char> > &firstSelStreamHeader)
{
    ScopedMutex scopedMutex(mMutex);

    map<uint32_t, StreamHeaderWrapper>::iterator iter =
        mFirstSelStreamHeaderMap.find(manifestIndex);

    iter = mFirstSelStreamHeaderMap.find(manifestIndex);
    if ( iter == mFirstSelStreamHeaderMap.end() )
        return false;

    StreamHeaderWrapper streamHeaderWrapper = iter->second;

    firstSelStreamHeader = streamHeaderWrapper.streamHeader;
    firstSelTrackIndex = streamHeaderWrapper.trackIndex;
    firstSelStreamIndex = streamHeaderWrapper.streamIndex;
    return true;
}

void AdaptiveStreamingPlayer::McMediaSink::flush()
{
    ScopedMutex scopedMutex(mMutex);

    // we have to make sure we clear the Media buffers in order allocated
    while (!mVideoMediaQueue.empty())
        mVideoMediaQueue.pop_front();
    while (!mAudioMediaQueue.empty())
        mAudioMediaQueue.pop_front();

    mCurrentPlayoutTime = AseTimeStamp::ZERO;

    mCurrentVideoPlaybackPos = -1;
    mCurrentAudioPlaybackPos = -1;
    mNextVideoDeliveryPos = -1;
    mNextAudioDeliveryPos = -1;
    mVideoTrailingBufferSize = 0;
    mAudioTrailingBufferSize = 0;
    mCurrentVideoStreamKey = "";
    mCurrentAudioStreamKey = "";
    mCurrentVideoGopStartPts = 0;
    mCurrentAudioGopStartPts = 0;
}

void AdaptiveStreamingPlayer::McMediaSink::flushMedia(MediaType mediaType)
{
    ScopedMutex scopedMutex(mMutex);
    if ( mediaType == MEDIA_VIDEO )
    {
        while (!mVideoMediaQueue.empty())
            mVideoMediaQueue.pop_front();
        mCurrentVideoPlaybackPos = -1;
        mNextVideoDeliveryPos = -1;
        mVideoTrailingBufferSize = 0;
        mCurrentVideoStreamKey = "";
        mCurrentVideoGopStartPts = 0;
    }
    else if ( mediaType == MEDIA_AUDIO )
    {
        while (!mAudioMediaQueue.empty())
            mAudioMediaQueue.pop_front();
        mCurrentAudioPlaybackPos = -1;
        mNextAudioDeliveryPos = -1;
        mAudioTrailingBufferSize = 0;
        mCurrentAudioStreamKey = "";
        mCurrentAudioGopStartPts = 0;
    }
    LogQueueState();
}

NFErr AdaptiveStreamingPlayer::McMediaSink::readBlock(MediaType mediaType,
                                                      MediaEntity& block)
{
    ScopedMutex scopedMutex(mMutex);

    mNumReadBlockCallsSinceLastLog++;
    LogQueueState();
    NFErr retCode = NFErr_OK;

    switch(mediaType)
    {
    case MEDIA_VIDEO:
        if ( invalidIndex(mVideoMediaQueue, mNextVideoDeliveryPos) )
        {
            retCode = NFErr_Pending;
        }
        else
        {
            block = mVideoMediaQueue[mNextVideoDeliveryPos];
            string streamKey = computeStreamKey(block);

            if ( mCurrentVideoStreamKey != streamKey )
            {
                // If this is a new stream, make sure the header has been
                // delivered to the device already. If it has, it won't appear
                // in the mVideoHeaderMap anymore.
                mCurrentVideoStreamKey = streamKey;
                if(mVideoHeaderMap.find(streamKey) != mVideoHeaderMap.end())
                {
                    retCode = NFErr_Pending;
                    break;
                }
            }
            ++mNextVideoDeliveryPos;
            if(block.mDataBlock.get())
            {
                block.mDataBlock->inUse() = true;
            }
            retCode = block.mEndOfTrack? NFErr_EndOfStream : NFErr_OK;
        }
        break;
    case MEDIA_AUDIO:
        if ( invalidIndex(mAudioMediaQueue, mNextAudioDeliveryPos) )
        {
            retCode = NFErr_Pending;
        }
        else
        {
            block = mAudioMediaQueue[mNextAudioDeliveryPos];
            string streamKey = computeStreamKey(block);
            if ( mCurrentAudioStreamKey != streamKey )
            {
                // If this is a new stream, make sure the header has been
                // delivered to the device already. If it has, it won't appear
                // in the mAudioHeaderMap anymore.
                mCurrentAudioStreamKey = streamKey;
                if(mAudioHeaderMap.find(streamKey) != mAudioHeaderMap.end())
                {
                    retCode = NFErr_Pending;
                    break;
                }
            }
            ++mNextAudioDeliveryPos;
            if(block.mDataBlock.get())
            {
                block.mDataBlock->inUse() = true;
            }
            retCode = block.mEndOfTrack? NFErr_EndOfStream : NFErr_OK;
        }
        break;
    default:
        assert(false); // unexpected
    }

    return retCode;
}

// FIXME provide period index too
void AdaptiveStreamingPlayer::McMediaSink::updateAudioPlayPosition(
    /*out*/uint32_t& audioManifestIndex,
    /*out*/uint32_t& audioStreamIndex,
    /*out*/uint32_t& audioGopStartPTS,
    uint32_t currentPts)
{
    ScopedMutex scopedMutex(mMutex);

    mCurrentPlayoutTime.set(currentPts, 1000);
    updatePlaybackPosition(mAudioMediaQueue, mCurrentAudioPlaybackPos,
                          mNextAudioDeliveryPos,
                          mAudioTrailingBufferSize, currentPts);

    uint32_t audioTrackIndex;

    if ( !invalidIndex(mAudioMediaQueue, mCurrentAudioPlaybackPos) )
    {
        NTRACE(TRACE_MEDIACONTROL, "updatePlaybackPosition: audio %d:%u:%llu:%u",
                                mCurrentAudioPlaybackPos, mAudioTrailingBufferSize,
                                mAudioMediaQueue[mCurrentAudioPlaybackPos].mPtsInMS,
                                currentPts);
        MediaEntity block = mAudioMediaQueue[mCurrentAudioPlaybackPos];
        audioStreamIndex = block.mStreamIndex;
        audioManifestIndex = block.mManifestIndex;
        audioGopStartPTS = block.mPtsInMS;
    }
    else if (mCurrentAudioStreamKey != "")
    {
        // We've played to the end of the movie
        parseStreamKey(mCurrentAudioStreamKey, audioManifestIndex, audioTrackIndex, audioStreamIndex);
        audioGopStartPTS = mCurrentAudioGopStartPts;
    }
    else
    {
        // we don't receive anything yet
        audioManifestIndex = 0;
        audioStreamIndex = 0;
        audioGopStartPTS = 0;
    }
}

// FIXME provide period index too
void AdaptiveStreamingPlayer::McMediaSink::updateVideoPlayPosition(
    /*out*/uint32_t& videoManifestIndex,
    /*out*/uint32_t& videoStreamIndex,
    /*out*/uint32_t& videoGopStartPTS,
    /*out*/uint32_t& videoGopBitrate,
    uint32_t currentPts)
{
    ScopedMutex scopedMutex(mMutex);

    mCurrentPlayoutTime.set(currentPts, 1000);
    updatePlaybackPosition(mVideoMediaQueue, mCurrentVideoPlaybackPos,
                          mNextVideoDeliveryPos,
                          mVideoTrailingBufferSize, currentPts);

    uint32_t videoTrackIndex;
    if ( !invalidIndex(mVideoMediaQueue, mCurrentVideoPlaybackPos) )
    {
        NTRACE(TRACE_MEDIACONTROL, "updatePlaybackPosition: video %d:%u:%llu:%u",
                                mCurrentVideoPlaybackPos, mVideoTrailingBufferSize,
                                mVideoMediaQueue[mCurrentVideoPlaybackPos].mPtsInMS,
                                currentPts);
        MediaEntity block = mVideoMediaQueue[mCurrentVideoPlaybackPos];
        videoStreamIndex = block.mStreamIndex;
        videoManifestIndex = block.mManifestIndex;
        videoGopStartPTS = block.mPtsInMS;
        shared_ptr<const Manifest> pManifest =
                            mManifestMap[videoManifestIndex];
        const vector<ManifestTrackPtr>& tracks = pManifest->getTracks();
        const vector<ManifestStreamPtr>& streams =
                            tracks[block.mTrackIndex]->getStreams();
        ManifestStreamPtr selectedStream = streams[videoStreamIndex];
        videoGopBitrate = selectedStream->getNominalBitrate();
    }
    else if ( mCurrentVideoStreamKey != "" )
    {
        // We've played to the end of the movie
        parseStreamKey(mCurrentVideoStreamKey, videoManifestIndex, videoTrackIndex, videoStreamIndex);
        videoGopStartPTS = mCurrentVideoGopStartPts;
        videoGopBitrate = 0;
    }
    else
    {
        videoStreamIndex = 0;
        videoManifestIndex = 0;
        videoGopStartPTS = 0;
        videoGopBitrate = 0;
    }
}

//
// McMediaSink
//

bool AdaptiveStreamingPlayer::McMediaSink::canPlayType(
    ase::MimeType& /*mime*/) const
{
    return true; // FIXME: always true? - Wei
}

void AdaptiveStreamingPlayer::McMediaSink::deliver(
    ase::AseBufferPtr const& dataBlock,
    int32_t manifestIndex,
    int32_t trackIndex,
    int32_t streamIndex,
    bool startOfFragment,
    ase::AseTimeStamp const& pts)
{
    ScopedMutex scopedMutex(mMutex);

    if ( startOfFragment )
    {
        NTRACE(TRACE_MEDIACONTROL,
                "start of fragment: track id: %d, pts: %llu ms",
                trackIndex,
                pts.getValueInMs());
    }

    MediaEntity entity;
    entity.mValid = true;
    entity.mDataBlock = dataBlock;
    entity.mManifestIndex = manifestIndex;
    entity.mTrackIndex = trackIndex;
    entity.mStreamIndex = streamIndex;
    entity.mStartOfFragment = startOfFragment;
    entity.mEndOfTrack = false;
    entity.mPtsInMS = pts.getValueInMs();

    shared_ptr<const Manifest> pManifest =
                        mManifestMap[manifestIndex];
    const vector<ManifestTrackPtr>& tracks = pManifest->getTracks();
    MediaType mediaType = tracks[trackIndex]->getMediaType();

    switch(mediaType)
    {
    case MEDIA_AUDIO:
        mAudioMediaQueue.push_back(entity);
        if ( mNextAudioDeliveryPos == -1 )
            mNextAudioDeliveryPos = 0;
        if ( startOfFragment )
            mCurrentAudioGopStartPts = entity.mPtsInMS;

        break;
    case MEDIA_VIDEO:
        mVideoMediaQueue.push_back(entity);
        if ( mNextVideoDeliveryPos == -1 )
        {
            mNextVideoDeliveryPos = 0;
        }
        if ( startOfFragment )
            mCurrentVideoGopStartPts = entity.mPtsInMS;
        break;
    default:
        assert(false); // unexpected
    }

    mNumDeliverCallsSinceLastLog++;
    LogQueueState();
}

void AdaptiveStreamingPlayer::McMediaSink::newStream(
    unsigned char const* mediaHeader,
    size_t mediaHeaderSize,
    int32_t manifestIndex,
    int32_t trackIndex,
    int32_t streamIndex)
{
    NTRACE(TRACE_MEDIACONTROL, "newStream: manifest: %d, track: %d, stream: %d",
                manifestIndex, trackIndex, streamIndex);
    PERFTRACE("new stream header received: manifest: %d, track: %d, stream: %d, time=%llu \n",
                manifestIndex, trackIndex, streamIndex, Time::mono().val());
    shared_ptr< vector<unsigned char> > newStreamHeader(
        new vector<unsigned char>(mediaHeaderSize));
    memcpy(&((*newStreamHeader)[0]), mediaHeader, mediaHeaderSize);

    ScopedMutex scopedMutex(mMutex);

    MediaEntity entity;
    entity.mValid = true;
    entity.mNewStreamHeader = newStreamHeader;
    entity.mManifestIndex = manifestIndex;
    entity.mTrackIndex = trackIndex;
    entity.mStreamIndex = streamIndex;
    entity.mStartOfFragment = false;
    entity.mEndOfTrack = false;
    entity.mPtsInMS = 0;

    shared_ptr<const Manifest> pManifest =
                        mManifestMap[manifestIndex];
    const vector<ManifestTrackPtr>& tracks = pManifest->getTracks();
    MediaType mediaType = tracks[trackIndex]->getMediaType();

    string streamKey = computeStreamKey(entity);

    switch(mediaType)
    {
    case MEDIA_AUDIO:
        mAudioHeaderMap[streamKey] = entity;
        break;
    case MEDIA_VIDEO:
        mVideoHeaderMap[streamKey] = entity;
        break;
    default:
        assert(false); // unexpected
    }

    if ( mediaType == MEDIA_VIDEO )
    {
        map<uint32_t, StreamHeaderWrapper>::iterator iter =
            mFirstSelStreamHeaderMap.find(manifestIndex);

        if (iter == mFirstSelStreamHeaderMap.end() )
        {
            StreamHeaderWrapper streamHeaderWrapper;
            streamHeaderWrapper.streamHeader = newStreamHeader;
            streamHeaderWrapper.trackIndex = trackIndex;
            streamHeaderWrapper.streamIndex = streamIndex;
            mFirstSelStreamHeaderMap[manifestIndex] = streamHeaderWrapper;
        }
    }
}

void AdaptiveStreamingPlayer::McMediaSink::moveDeliveredHeaderFromMap(std::map<std::string, MediaEntity>& map,
                                                                      std::map<std::string, MediaEntity>::iterator it)
{
    mDeliveredStreamHeaders.push_back(it->second);
    map.erase(it);
}

// Removes the stream header from the McMediaSinks internal mapping.
shared_ptr<vector<unsigned char> >
AdaptiveStreamingPlayer::McMediaSink::getNewStreamHeader(MediaType& type,
                                                         int32_t& manifestIndex,
                                                         int32_t& trackIndex,
                                                         int32_t& streamIndex)
{
    ScopedMutex scopedMutex(mMutex);

    shared_ptr<vector<unsigned char> > header;
    std::map<std::string, MediaEntity>::iterator it;
    std::map<std::string, MediaEntity> *map = NULL;


    if(mVideoHeaderMap.empty() != true)
    {
        type = MEDIA_VIDEO;
        it = mVideoHeaderMap.begin();
        map = &mVideoHeaderMap;
    } else if(mAudioHeaderMap.empty() != true) {
        type = MEDIA_AUDIO;
        it = mAudioHeaderMap.begin();
        map = &mAudioHeaderMap;
    } else {
        return header;
    }

    streamIndex = it->second.mStreamIndex;
    trackIndex = it->second.mTrackIndex;
    manifestIndex = it->second.mManifestIndex;
    header = it->second.mNewStreamHeader;
    NTRACE(TRACE_MEDIACONTROL, "deliver new header: manifest: %d, track: %d, stream: %d",
                        manifestIndex, trackIndex, streamIndex);
    moveDeliveredHeaderFromMap(*map, it);

    return header;
}

void AdaptiveStreamingPlayer::McMediaSink::getCurrentCdnId(uint32_t& vcdnid, uint32_t& acdnid) const
{
    ScopedMutex scopedMutex(mMutex);
    vcdnid = 0;
    acdnid = 0;
    if ( !invalidIndex(mVideoMediaQueue, mCurrentVideoPlaybackPos) )
    {
        MediaEntity block = mVideoMediaQueue[mCurrentVideoPlaybackPos];
        vcdnid = block.mDataBlock->getCdnId();
    }
    if ( !invalidIndex(mAudioMediaQueue, mCurrentAudioPlaybackPos) )
    {
        MediaEntity block = mAudioMediaQueue[mCurrentAudioPlaybackPos];
        acdnid = block.mDataBlock->getCdnId();
    }

    return;
}

void AdaptiveStreamingPlayer::McMediaSink::endOfTrack(int32_t manifestIndex, int32_t trackIndex)
{
    ScopedMutex scopedMutex(mMutex);
    NTRACE(TRACE_MEDIACONTROL, "McMediaSink::endOfTrack track id %d", trackIndex);

    MediaEntity entity;
    entity.mValid = true;
    entity.mManifestIndex = manifestIndex;
    entity.mTrackIndex = trackIndex;
    entity.mStreamIndex = 0;
    entity.mStartOfFragment = false;
    entity.mEndOfTrack = true;
    entity.mPtsInMS = 0;

    shared_ptr<const Manifest> pManifest = mManifestMap[manifestIndex];
    const vector<ManifestTrackPtr>& tracks = pManifest->getTracks();
    MediaType mediaType = tracks[trackIndex]->getMediaType();

    switch(mediaType)
    {
    case MEDIA_AUDIO:
        if ( !mAudioMediaQueue.empty() )
            entity.mStreamIndex = mAudioMediaQueue.back().mStreamIndex;
        if ( !mAudioMediaQueue.back().mEndOfTrack )
            mAudioMediaQueue.push_back(entity);
        break;
    case MEDIA_VIDEO:
        if ( !mVideoMediaQueue.empty() )
            entity.mStreamIndex = mVideoMediaQueue.back().mStreamIndex;
        if ( !mVideoMediaQueue.back().mEndOfTrack )
            mVideoMediaQueue.push_back(entity);
        break;
    default:
        assert(false); // unexpected
    }
}

void AdaptiveStreamingPlayer::McMediaSink::currentPlaybackInfo(
    double& currentPlayoutRate,
    ase::AseTimeStamp& currentPlayoutTime)
{
    ScopedMutex scopedMutex(mMutex);

    currentPlayoutRate = 1.0;
    currentPlayoutTime = mCurrentPlayoutTime;
}

void AdaptiveStreamingPlayer::McMediaSink::LogQueueState()
{
    if(mNumDeliverCallsSinceLastLog > mLogTriggerThreshold ||
       mNumReadBlockCallsSinceLastLog > mLogTriggerThreshold)
    {
        NTRACE(TRACE_MEDIACONTROL, "ASP::McMediaSink: Audio queue: %zu; Video queue: %zu",
               mAudioMediaQueue.size(), mVideoMediaQueue.size());
        mNumDeliverCallsSinceLastLog = 0;
        mNumReadBlockCallsSinceLastLog = 0;
    }
}

NFErr AdaptiveStreamingPlayer::McMediaSink::skip(AseTimeStamp const& pts)
{
    ScopedMutex scopedMutex(mMutex);
    NTRACE(TRACE_MEDIACONTROL, "McMediaSink::skip to %llu ms",
            pts.getValueInMs() );
    AseTimeStamp startPts, endPts;
    getBufferRange(startPts, endPts);
    if ( (pts < startPts) || (pts > endPts) || (startPts == endPts) )
        return NFErr_NotAllowed;

    mCurrentVideoStreamKey = "";
    mCurrentVideoGopStartPts = 0;
    mCurrentAudioStreamKey = "";
    mCurrentAudioGopStartPts = 0;
    skipMedia(pts, mAudioMediaQueue, mAudioTrailingBufferSize, mCurrentAudioPlaybackPos);
    mNextAudioDeliveryPos = mCurrentAudioPlaybackPos;

    // Skip video media until the video PTS is beyond that of audio.  We can
    // drop a few samples of audio to align the video and audio
    // timestamps. Dropping video samples is more complicated.
    AseTimeStamp audioPts(mAudioMediaQueue[mCurrentAudioPlaybackPos].mPtsInMS, 1000);
    skipMedia(audioPts, mVideoMediaQueue, mVideoTrailingBufferSize, mCurrentVideoPlaybackPos);
    mNextVideoDeliveryPos = mCurrentVideoPlaybackPos;

    NTRACE(TRACE_MEDIACONTROL, "update video playback time: %llu",
            mVideoMediaQueue[mCurrentVideoPlaybackPos].mPtsInMS );
    NTRACE(TRACE_MEDIACONTROL, "update audio playback time: %llu",
            mAudioMediaQueue[mCurrentAudioPlaybackPos].mPtsInMS );

    mCurrentPlayoutTime.set(mVideoMediaQueue[mCurrentVideoPlaybackPos].mPtsInMS, 1000);
    //updateStreamingBuffer();

    return NFErr_OK;
}

void AdaptiveStreamingPlayer::McMediaSink::skipMedia(
            AseTimeStamp const& pts,
            deque<MediaEntity> const& mediaQueue,
            uint32_t& trailingBufferSize,
            int32_t& updatedPlaybackPos)
{
    trailingBufferSize = 0;
    updatedPlaybackPos = 0;

    int32_t tmpIter = updatedPlaybackPos;
    uint32_t tmpTrailingBufferSize = 0;
    MediaEntity block;
    while ( true )
    {
        if ( mediaQueue[updatedPlaybackPos].mStartOfFragment )
        {
            tmpIter = updatedPlaybackPos;
            tmpTrailingBufferSize = trailingBufferSize;
            if (mediaQueue[updatedPlaybackPos].mPtsInMS >= pts.getValueInMs())
            {
                break;
            }
        }

        block = mediaQueue[updatedPlaybackPos];
        if (block.mDataBlock.get())
            trailingBufferSize += block.mDataBlock->getCapacity();
        ++updatedPlaybackPos;
        if ( updatedPlaybackPos + 1 >= (int32_t)mediaQueue.size() )
        {
            updatedPlaybackPos = tmpIter;
            trailingBufferSize = tmpTrailingBufferSize;
            break;
        }
    }
    return;
}


void AdaptiveStreamingPlayer::McMediaSink::getBufferRange(
        AseTimeStamp& startPts, AseTimeStamp& endPts)
{
    ScopedMutex scopedMutex(mMutex);
    if (mVideoMediaQueue.empty() || mAudioMediaQueue.empty())
        return;

    // get the pts of the first gop boundary
    uint32_t startPos;
    for ( startPos = 0; startPos < mVideoMediaQueue.size(); ++startPos )
    {
        if ( mVideoMediaQueue[startPos].mStartOfFragment )
            break;
    }
    ullong videoStartPts = mVideoMediaQueue[startPos].mPtsInMS;
    for ( startPos = 0; startPos < mAudioMediaQueue.size(); ++startPos )
    {
        if ( mAudioMediaQueue[startPos].mStartOfFragment )
            break;
    }
    ullong audioStartPts = mAudioMediaQueue[startPos].mPtsInMS;
    startPts = AseTimeStamp(max(videoStartPts, audioStartPts), 1000);


    ullong videoEndPts, audioEndPts;

    MediaEntity lastVideoBlock = mVideoMediaQueue.back();
    videoEndPts = lastVideoBlock.mPtsInMS;
    if ( lastVideoBlock.mEndOfTrack )
    {
        if (mVideoMediaQueue.size() > 1)
        {
            lastVideoBlock = mVideoMediaQueue[mVideoMediaQueue.size() - 2];
            videoEndPts = lastVideoBlock.mPtsInMS;
        }
        else
        {
            videoEndPts = 0;
        }
    }

    MediaEntity lastAudioBlock = mAudioMediaQueue.back();
    audioEndPts = lastAudioBlock.mPtsInMS;
    if ( lastAudioBlock.mEndOfTrack )
    {
        if (mAudioMediaQueue.size() > 1)
        {
            lastAudioBlock = mAudioMediaQueue[mAudioMediaQueue.size() - 2];
            audioEndPts = lastAudioBlock.mPtsInMS;
        }
        else
            audioEndPts = 0;
    }

    NTRACE(TRACE_MEDIACONTROL, "McMediaSink::getBufferRange, videoEndPts: %llu ms, audioEndPts: %llu ms",
            videoEndPts, audioEndPts);
    ullong minEndPts = min(videoEndPts, audioEndPts);
    if ( minEndPts < mMinPrebufLength )
        endPts = startPts;
    else
    {
        endPts = AseTimeStamp(minEndPts - mMinPrebufLength, 1000);
    }

    if ( endPts < startPts )
        endPts = startPts;

    NTRACE(TRACE_MEDIACONTROL,
            "McMediaSink::getBufferRange: start pts: %llu ms, end pts: %llu ms",
            startPts.getValueInMs(), endPts.getValueInMs());

    return;
}

bool AdaptiveStreamingPlayer::McMediaSink::enableSkipBack()
{
    uint32_t minVideoBufferRequiredForSkipBack;
    if ( mIsStereoVideo )
    {
        minVideoBufferRequiredForSkipBack =
            mConfigParameterPtr->getParameterAs<int>(
                "minVideoBufferRequiredFor3DSkipBack", 32 * 1024 * 1024);
    }
    else
    {
        minVideoBufferRequiredForSkipBack =
            mConfigParameterPtr->getParameterAs<int>(
                "minVideoBufferRequiredForSkipBack", 24 * 1024 * 1024);
    }
    uint32_t minAudioBufferRequiredForSkipBack =
        mConfigParameterPtr->getParameterAs<int>(
            "minAudioBufferRequiredForSkipBack", 1024 * 1024);

    bool enableVideoSkipBack = false;
    if ( ( mTotalVideoBufferSize >= minVideoBufferRequiredForSkipBack)
        && ( mTotalAudioBufferSize >= minAudioBufferRequiredForSkipBack) )
    {
        enableVideoSkipBack = true;
    }
    NTRACE(TRACE_MEDIACONTROL, "video skip back enabled: %u", enableVideoSkipBack);
    return enableVideoSkipBack;
}

void AdaptiveStreamingPlayer::McMediaSink::loadConfigParas()
{
    mTotalVideoBufferSize =
        mLocalConfigParameterPtr->getParameterAs<int>("totalBufferSize", 32 * 1024 * 1024);
    mTotalAudioBufferSize =
        mLocalConfigParameterPtr->getParameterAs<int>("totalAudioBufferSize", 4 * 1024 * 1024);
    mMinPrebufLength =
        mConfigParameterPtr->getParameterAs<int>("minPrebufSize", 4004);
    bool enableVideoSkipBack = enableSkipBack();
    if (enableVideoSkipBack)
    {
        mMaxTrailingBufferLen =
            mConfigParameterPtr->getParameterAs<int>("maxTrailingBufferLen", 15000);
        if ( mIsStereoVideo )
        {
            mMaxVideoTrailingBuffer =
                mConfigParameterPtr->getParameterAs<int>("max3DVideoTrailingBufferSize", 12 * 1024 * 1024);
        }
        else
        {
            mMaxVideoTrailingBuffer =
                mConfigParameterPtr->getParameterAs<int>("maxVideoTrailingBufferSize", 8 * 1024 * 1024);
        }
        mMaxAudioTrailingBuffer =
            mConfigParameterPtr->getParameterAs<int>("maxAudioTrailingBufferSize", 384 * 1024);
    }
    else
    {
        // don't maintain a back buffer when skip back is not enabled
        mMaxTrailingBufferLen = 2002;
        mMaxVideoTrailingBuffer = 0;
        mMaxAudioTrailingBuffer = 0;
    }
    NTRACE( TRACE_MEDIACONTROL,
            "loadConfigParas: mMaxTrailingBufferLen: %u, mMaxVideoTrailingBuffer: %u, mMaxAudioTrailingBuffer: %u",
            mMaxTrailingBufferLen, mMaxVideoTrailingBuffer, mMaxAudioTrailingBuffer);
}

void AdaptiveStreamingPlayer::McMediaSink::updateStreamingBuffer()
{
    ScopedMutex scopedMutex(mMutex);

    NTRACE( TRACE_MEDIACONTROL, "updateStreamingBuffer, video playback pos: %d, audio playback pos: %d",
            mCurrentVideoPlaybackPos, mCurrentAudioPlaybackPos );
    if ( mCurrentPlayoutTime == mLastBufferUpdateTime ||
            mVideoMediaQueue.empty() ||
            mAudioMediaQueue.empty() ||
            (mCurrentVideoPlaybackPos <= 0) ||
            (mCurrentAudioPlaybackPos <= 0) )
        return;

    uint32_t currentPts = mCurrentPlayoutTime.getValueInMs();

    mLastBufferUpdateTime = mCurrentPlayoutTime;

    uint32_t videoTrailingBufferLen =
        (currentPts > mVideoMediaQueue.front().mPtsInMS) ?
                currentPts - mVideoMediaQueue.front().mPtsInMS : 0;
    uint32_t audioTrailingBufferLen =
        (currentPts > mAudioMediaQueue.front().mPtsInMS) ?
                currentPts - mAudioMediaQueue.front().mPtsInMS : 0;

    NTRACE(TRACE_MEDIACONTROL, "enter updateStreamingBuffer: current pts: %u ms, "
                               "video buffer size: %zu, audio buffer size: %zu "
                               "video trailing buffer len: %u, video buffer trailing buffer size: %u bytes"
                               "audio trailing buffer len: %u, audio buffer trailing buffer size: %u bytes",
                               currentPts,
                               mVideoMediaQueue.size(), mAudioMediaQueue.size(),
                               videoTrailingBufferLen, mVideoTrailingBufferSize,
                               audioTrailingBufferLen, mAudioTrailingBufferSize);

    bool success;
    while ( (mVideoTrailingBufferSize > mMaxVideoTrailingBuffer) ||
            (videoTrailingBufferLen > mMaxTrailingBufferLen) )
    {
        // pop one gop of video
        success = popOneGopData(mVideoMediaQueue, mCurrentVideoPlaybackPos, mNextVideoDeliveryPos, mVideoTrailingBufferSize);
        if ( mVideoMediaQueue.empty() )
            break;

        videoTrailingBufferLen =
                        (currentPts > mVideoMediaQueue.front().mPtsInMS) ?
                                     currentPts - mVideoMediaQueue.front().mPtsInMS : 0;

        if ((mCurrentVideoPlaybackPos < (int32_t)mVideoMediaQueue.size()) &&
                (mCurrentVideoPlaybackPos >= 0))
        {
                    NTRACE(TRACE_MEDIACONTROL, "updateVideoBuffer %d:%d:%zu:%llu:%llu:%u",
                            mCurrentVideoPlaybackPos,
                            mNextVideoDeliveryPos,
                            mVideoMediaQueue.size(),
                            mVideoMediaQueue[mCurrentVideoPlaybackPos].mPtsInMS,
                            mVideoMediaQueue.front().mPtsInMS,
                            mVideoTrailingBufferSize);
        }
        NTRACE(TRACE_MEDIACONTROL, "updated buffer stats: video trainling buffer size:%u, length: %u ms",
                                mVideoTrailingBufferSize, videoTrailingBufferLen);
        if (!success)
            break;

    }

    while ( (mAudioTrailingBufferSize > mMaxAudioTrailingBuffer) ||
            (audioTrailingBufferLen > mMaxTrailingBufferLen) )
    {
        // pop one audio gop
        success = popOneGopData(mAudioMediaQueue, mCurrentAudioPlaybackPos, mNextAudioDeliveryPos, mAudioTrailingBufferSize);

        if( mAudioMediaQueue.empty() )
            break;

        audioTrailingBufferLen =
                        (currentPts > mAudioMediaQueue.front().mPtsInMS) ?
                                 currentPts - mAudioMediaQueue.front().mPtsInMS : 0;
        NTRACE(TRACE_MEDIACONTROL, "updated buffer stats: audio trainling buffer size:%u, length: %u ms",
                                mAudioTrailingBufferSize, audioTrailingBufferLen);
        if (!success)
            break;
    }

    NTRACE(TRACE_MEDIACONTROL, "leaving updateStreamingBuffer: current pts: %u ms, "
                               "video buffer size: %zu, audio buffer size: %zu "
                               "video trailing buffer len: %u, video buffer trailing buffer size: %u bytes "
                               "audio trailing buffer len: %u, audio buffer trailing buffer size: %u bytes",
                               currentPts,
                               mVideoMediaQueue.size(), mAudioMediaQueue.size(),
                               videoTrailingBufferLen, mVideoTrailingBufferSize,
                               audioTrailingBufferLen, mAudioTrailingBufferSize);

    // TODO: check whether mCurrentVideoPlaybackPos and mCurrentAudioPlaybackPos is still at the gop boundary?
}

bool AdaptiveStreamingPlayer::McMediaSink::popOneGopData(
                deque<MediaEntity>& mediaQueue,
                int32_t& currentPlayPos,
                int32_t& currentDeliveryPos,
                uint32_t& trailingBufferSize )
{
    bool success = true;
    // pop the first block
    NTRACE(TRACE_MEDIACONTROL, "popOneGopData %d:%d:%zu:%u",
            currentPlayPos, currentDeliveryPos,
            mediaQueue.size(),
            trailingBufferSize);
    assert(currentDeliveryPos <= (int32_t)mediaQueue.size());
    assert(currentPlayPos <= currentDeliveryPos);
    MediaEntity block = mediaQueue.front();

    if ( block.mEndOfTrack || block.mDataBlock->inUse() )
    {
        NTRACE(TRACE_MEDIACONTROL, "popOneGopData: end of track: %u, in use: %u",
                        block.mEndOfTrack, block.mDataBlock->inUse());
        return false;
    }

    if ( trailingBufferSize >= block.mDataBlock->getCapacity() )
    {
        trailingBufferSize -= block.mDataBlock->getCapacity();
        mediaQueue.pop_front();
        --currentPlayPos;
        --currentDeliveryPos;
    }
    else
    {
        return false;
    }

    block = mediaQueue.front();
    while ( (currentPlayPos > 0) &&
                !block.mStartOfFragment &&
                (trailingBufferSize >= block.mDataBlock->getCapacity()) )
    {
        if ( block.mDataBlock->inUse() )
        {
            NTRACE(TRACE_MEDIACONTROL, "popOneGopData, block in use");
            success = false;
            break;
        }

        // the poping stops at the beginning of the next gop or the current playback position
        trailingBufferSize -= block.mDataBlock->getCapacity();
        mediaQueue.pop_front();
        --currentPlayPos;
        --currentDeliveryPos;

        if ( mediaQueue.empty() )
        {
            success = false;
            break;
        }
        block = mediaQueue.front();
        if ( block.mEndOfTrack )
        {
            success = false;
            break;
        }
    }
    return success;
}


void AdaptiveStreamingPlayer::McMediaSink::updatePlaybackPosition(
                                    deque<MediaEntity> const& mediaQueue,
                                    int32_t& currentGopStartPos,
                                    int32_t currentDeliveryPos,
                                    uint32_t& trailingBufferSize,
                                    uint32_t currentPts)
{
    if ( mediaQueue.empty() )
        return;

    NTRACE(TRACE_MEDIACONTROL, "updatePlaybackPosition in: %d:%u:%u",
                        currentGopStartPos, trailingBufferSize, currentPts);

    if (currentGopStartPos < 0)
        currentGopStartPos = 0;

    int32_t currentPlayPos = currentGopStartPos;
    uint32_t tmpTrailingBufferSize = trailingBufferSize;

    MediaEntity block;
    while ( (currentPlayPos < (int32_t)mediaQueue.size()) &&
                (currentPlayPos < currentDeliveryPos) )
    {
        // invariant: mustbe(currentGopStartPos at the start position of a gop)
        // invariant: mustbe(mediaQueue[currentGopStartPos].mPtsInMS <= currentPts)
        block = mediaQueue[currentPlayPos];
        if ( block.mStartOfFragment && (block.mPtsInMS <= currentPts) )
        {
            currentGopStartPos = currentPlayPos;
            trailingBufferSize = tmpTrailingBufferSize;
        }
        if ( (block.mPtsInMS >= currentPts) || block.mEndOfTrack )
        {
            break;
        }
        tmpTrailingBufferSize += block.mDataBlock->getCapacity();
        ++currentPlayPos;
    }
    if ( currentGopStartPos < (int32_t)mediaQueue.size() )
    {
        NTRACE(TRACE_MEDIACONTROL, "updatePlaybackPosition out: %d:%d:%u:%llu",
                                    currentGopStartPos, currentDeliveryPos,
                                    trailingBufferSize, mediaQueue[currentGopStartPos].mPtsInMS);
    }
    return;
}

bool AdaptiveStreamingPlayer::McMediaSink::invalidIndex(deque<MediaEntity> const& mediaQueue,
                                                        int32_t index) const
{
    if ( mediaQueue.empty() )
        return true;
    if ( (index < 0) || (index >= (int32_t)mediaQueue.size()) )
        return true;

    return false;
}

string AdaptiveStreamingPlayer::McMediaSink::computeStreamKey(MediaEntity const& mediaEntity)
{
    ostringstream strm;
    strm.imbue(std::locale::classic());
    strm << mediaEntity.mManifestIndex << "." << mediaEntity.mTrackIndex << "."
         << mediaEntity.mStreamIndex;
    return strm.str();
}

void AdaptiveStreamingPlayer::McMediaSink::parseStreamKey(
        string const& streamKey, uint32_t& manifestIndex,
        uint32_t& trackIndex, uint32_t& streamIndex)
{
    vector<string> tokens;
    StringTokenizer::tokenize( streamKey, tokens, "." );
    assert(tokens.size() == 3);
    manifestIndex = lexical_cast<uint32_t>(tokens[0]);
    trackIndex = lexical_cast<uint32_t>(tokens[1]);
    streamIndex = lexical_cast<uint32_t>(tokens[2]);
}

