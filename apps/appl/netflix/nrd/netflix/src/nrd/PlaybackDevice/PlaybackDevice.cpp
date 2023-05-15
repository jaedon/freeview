/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "PlaybackDevice.h"
#include "ESManagerError.h"

#include <cstdlib>
#include <cstring>
#include <sstream>

#include <nrdbase/MutexRanks.h>
#include <nrdbase/Time.h>

#include "Demultiplexers/Mp4Demultiplexer.h"
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Log.h>
#include <nrd/NrdConfiguration.h>
#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/IDeviceError.h>
#include "DeviceConstants.h"
#include "EventQueue.h"
#include "PlaybackDeviceError.h"
#include "ProducerConsumerList.h"
#include "DrmManager.h"

#ifdef ENABLE_MCSIM_INPUT_DUMP
#include "MCSimInputFileMaker.h"
#define MCSIM_INPUT_DUMP_OPEN MCSimInputFileMaker::open
#define MCSIM_INPUT_DUMP_CLOSE MCSimInputFileMaker::close
#define MCSIM_INPUT_DUMP_FEEDDATA MCSimInputFileMaker::feedData
#define MCSIM_INPUT_DUMP_FEEDSTREAMHEADER MCSimInputFileMaker::feedStreamHeader
#define MCSIM_INPUT_DUMP_SETSTREAMINFO MCSimInputFileMaker::setStreamInfo
#else // ENABLE_MCSIM_INPUT_DUMP
#define MCSIM_INPUT_DUMP_OPEN(...)
#define MCSIM_INPUT_DUMP_CLOSE(...)
#define MCSIM_INPUT_DUMP_FEEDDATA(...)
#define MCSIM_INPUT_DUMP_FEEDSTREAMHEADER(...)
#define MCSIM_INPUT_DUMP_SETSTREAMINFO(...)
#endif // ENABLE_MCSIM_INPUT_DUM


using namespace netflix;
using namespace netflix::device;

static const llong  INVALID_TIMESTAMP = (  -1);


PlaybackDevice::PlaybackDevice(shared_ptr<esplayer::IElementaryStreamManager> ESManager,
                               shared_ptr<ISystem> NrdSystem )
    : ESManager_(ESManager),
      NrdSystem_(NrdSystem),
      playbackGroup_(NULL),
      videoFrameStepping_(false),
      lastPostedPresentationTimestamp_(INVALID_TIMESTAMP),
      drmHeaderReceived_(false),
      requested_speed_(0),
      format3D_(esplayer::NOT_3D),
      initialized_( false ),
      primaryVideoAttr_( NULL ),
      maxImageWidth_( 0 ),
      maxImageHeight_( 0 ),
      maxFrameRateValue_( 0 ),
      maxFrameRateScale_( 1 ),
      targetVolume_( 1.0 )
{
    endOfStream_[AUDIO] = false;
    endOfStream_[VIDEO] = false;
    streamPlayers_[AUDIO] = NULL;
    streamPlayers_[VIDEO] = NULL;
    numRemainingFeedDatasToReport[AUDIO] = 0;
    numRemainingFeedDatasToReport[VIDEO] = 0;


    streamDataBlocks_[AUDIO].reset(new ProducerConsumerList<Mp4Demultiplexer::DataBlock>(
                                       DeviceConstants::MAX_NUM_AUDIO_DATA_BLOCKS_IN_USE,
                                       &mDataBlockCleanup));
    streamDataBlocks_[VIDEO].reset(new
                                   ProducerConsumerList<Mp4Demultiplexer::DataBlock>(
                                       DeviceConstants::MAX_NUM_VIDEO_DATA_BLOCKS_IN_USE,
                                       &mDataBlockCleanup));
}

PlaybackDevice::~PlaybackDevice()
{
    close();
}

NFErr PlaybackDevice::open(const std::vector<StreamInfo>& streams,
                           ContainerType containerType, DrmType /*drmType*/,
                           uint32_t maxImageWidth, uint32_t maxImageHeight)
{
    assert(containerType == PIFF_CONTAINER || containerType == DASH_CONTAINER);
    uint32_t maxFrameRateValue = 0;
    uint32_t maxFrameRateScale = 1;

    // Make sure the device is closed
    close();

    // Init State Variables
    //
    endOfStream_[AUDIO] = false;
    endOfStream_[VIDEO] = false;
    videoFrameStepping_ = false;
    lastPostedPresentationTimestamp_ = INVALID_TIMESTAMP;
    containerType_ = containerType;

    // Record information about the streams that we may receive during adaptive
    // streaming.
    //
    NFErr err = analyzeStreamInfo(streams,
                                  0 /*manifest index = 0 on open */,
                                  primaryVideoAttr_,
                                  maxImageWidth,
                                  maxImageHeight,
                                  maxFrameRateValue,
                                  maxFrameRateScale);
    if (!err.ok())
        return err;

    maxImageWidth_ = maxImageWidth;
    maxImageHeight_ = maxImageHeight;

    bool HEVC_Level_5_0_Supported_ = false;
    bool HEVC_Level_5_1_Supported_ = false;
    ISystem::Capability systemCapability = NrdSystem_->getCapability();
    std::vector<ContentProfile>::iterator iter;
    for(iter =  systemCapability.videoProfiles.begin() ;
        iter != systemCapability.videoProfiles.end() ;
        iter++)
    {
        if( *iter == HEVC_MAIN_L50_DASH_CENC || *iter == HEVC_MAIN10_L50_DASH_CENC )
            HEVC_Level_5_0_Supported_ = true;
        if( *iter == HEVC_MAIN_L51_DASH_CENC || *iter == HEVC_MAIN10_L51_DASH_CENC )
            HEVC_Level_5_1_Supported_ = true;
    }

    // if ( temporal layered && only support up to L50 && !seamlessFrameRateSwitch)
    // then use lower layer's frame rate
    if (primaryVideoAttr_ &&
        primaryVideoAttr_->temporalLayerStream_ &&
        (!HEVC_Level_5_1_Supported_ && HEVC_Level_5_0_Supported_ ) )
    {
        maxFrameRateValue /= 2;
    }

    maxFrameRateValue_ = maxFrameRateValue;
    maxFrameRateScale_ = maxFrameRateScale;

    return err;
}

NFErr PlaybackDevice::open(ContainerType)
{
    return NFErr_OK;
}

NFErr PlaybackDevice::initializeVideoPlayer(const std::vector<unsigned char>& headerData,
                                            const CommonVideoAttributesFromManifest &videoAttributes)
{
    NRDP_UNUSED(headerData);
    NRDP_UNUSED(videoAttributes);
    return NFErr_OK;
}

NFErr PlaybackDevice::initializeAudioPlayer(const std::vector<unsigned char>& headerData,
                                            const CommonAudioAttributesFromManifest &audioAttributes)
{
    NRDP_UNUSED(headerData);
    NRDP_UNUSED(audioAttributes);
    return NFErr_OK;
}

NFErr PlaybackDevice::initializePlayers()
{
    NFErr err;

    // Create an event queue
    //
    eventQueue_.reset(new EventQueue(DeviceConstants::EVENT_QUEUE_SIZE));

    // Initialize elementary stream player callbacks.
    //
    playerCallbacks_[AUDIO].reset(new ESPlayerCallback(*this, esplayer::AUDIO));
    playerCallbacks_[VIDEO].reset(new ESPlayerCallback(*this, esplayer::VIDEO));

    // Create elementary stream player PlaybackGroup. Audio and video
    // elementary stream playback is controlled via the playback
    // group.
    //
    err = ESManager_->createPlaybackGroup(playbackGroup_);
    if(!err.ok())
    {
        close();
        return err;
    }

    // Initialize audio and video elementary stream players.
    //
    err = initAudioPlayer();
    if(err != NFErr_OK)
    {
        close();
        err.push(NFErr_Bad);
        return err;
    }

    if((err = initVideoPlayer(primaryVideoAttr_,
                              maxImageWidth_,
                              maxImageHeight_,
                              maxFrameRateValue_,
                              maxFrameRateScale_)) != NFErr_OK)
    {
        close();
        err.push(NFErr_Bad);
        return err;
    }

    // Initialize the MP4 demultiplexer
    //

    // get video capability of device
    ISystem::Capability systemCapability = NrdSystem_->getCapability();

    if (containerType_ == PIFF_CONTAINER || containerType_ == DASH_CONTAINER) {
        // format3D_ was set in analyzeStreamInfo() based on whether mvc
        // configuration boxes were parsed in the video stream header. If the
        // video is 3D, the format3D_ value may have been updated in
        // initVideoPlayer if the device's video player implementation
        // requests MVC_SPLIT.

        // as of 13.4, NRDP 4.1, seamlessFramerateSwitching is removed since
        // most of devices can not support it, and support of HEVC level 5.1
        // becomes broader. Therefore, defaulted to false.
        demuxer_.reset(new Mp4Demultiplexer(*streamDataBlocks_[AUDIO],
                                            *streamDataBlocks_[VIDEO],
                                            format3D_,
                                            systemCapability.videoProfiles,
                                            false,
                                            eventQueue_
                           ));
        // for any after object creation works, such as passing shared pointer
        demuxer_->init();

    } else {
        close();
        return NFErr_Bad;
    }

    // The callbacks are disabled by default so that we can initialize the
    // demultiplexer after the video player. In the case when the video player
    // wants MVC_SPLIT, we don't know until we try to initialize a player.
    playerCallbacks_[AUDIO]->enable();
    playerCallbacks_[VIDEO]->enable();

    MCSIM_INPUT_DUMP_OPEN(streams, containerType, maxImageWidth, maxImageHeight);

    initialized_ = true;

    return NFErr_OK;
}

NFErr PlaybackDevice::analyzeStreamInfo(const std::vector<StreamInfo>& streams,
                                        uint32_t manifestIndex,
                                        VideoAttributes*& primaryVideoAttr,
                                        uint32_t& maxImageWidth,
                                        uint32_t& maxImageHeight,
                                        uint32_t& maxFrameRateValue,
                                        uint32_t& maxFrameRateScale)
{
    NFErr err;

    // save passed streams into member variable. This will be used to set
    // additional information that manifest has, but stream header does not
    streams_[manifestIndex] = streams;

    format3D_ = esplayer::NOT_3D;
    primaryVideoAttr = NULL;

    // We shouldn't have already received stream info for this manifest.
    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::analyzeStreamInfo: Receiving %zu streams "
           "for manifestIndex: %d", streams.size(), manifestIndex);
    assert(streamsAttributes_.find(manifestIndex) == streamsAttributes_.end());
    streamsAttributes_[manifestIndex].resize(streams.size());

    uint32_t i;


    for (i = 0; i < streams.size(); i++) {
        streamsAttributes_[manifestIndex][i].streamIndex_ = i;
        streamsAttributes_[manifestIndex][i].audioAttributes_ = NULL;
        streamsAttributes_[manifestIndex][i].videoAttributes_ = NULL;

        if (streams[i].headerData_)
        {
            std::vector<unsigned char>* requestedDrmHeader = NULL;

            if (streams[i].streamType_ == VIDEO && drmHeader_.size() == 0)
                requestedDrmHeader = &drmHeader_;

            if ((err = parseStreamHeader(containerType_, streams[i].streamType_,
                                         *streams[i].headerData_,
                                         streamsAttributes_[manifestIndex][i],
                                         requestedDrmHeader)) != NFErr_OK)
            {
                Log::error(TRACE_MEDIAPLAYBACK, "Error: Parse stream headers failed.");
                return err;
            }

            if (streamsAttributes_[manifestIndex][i].videoAttributes_->format3D_ != esplayer::NOT_3D) {
                NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::open: Media is 3D with type. %d",
                       streamsAttributes_[manifestIndex][i].videoAttributes_->format3D_);
                format3D_ = streamsAttributes_[manifestIndex][i].videoAttributes_->format3D_;
            }
        }

        double maxFrameRate = 0.0;
        if (streams[i].streamType_ == VIDEO) {
            // find the first video attributes available as the primary
            if (primaryVideoAttr == NULL)
                primaryVideoAttr = streamsAttributes_[manifestIndex][i].videoAttributes_;

            // get maximum video-frame width/height among all video streams

            if (streams[i].videoImageWidth_ > maxImageWidth)
                maxImageWidth = streams[i].videoImageWidth_;

            if (streams[i].videoImageHeight_ > maxImageHeight)
                maxImageHeight = streams[i].videoImageHeight_;

            if ( (streams[i].videoFrameRateValue_ / streams[i].videoFrameRateScale_) > maxFrameRate ){
                // NCCP has maxFrameRate. But Netflix will not switch framerate
                // during the playback.  So, basically, each stream's framerate
                // is maxframerate
                maxFrameRate = streams[i].videoFrameRateValue_ / streams[i].videoFrameRateScale_;
                maxFrameRateValue = streams[i].videoFrameRateValue_ ;
                maxFrameRateScale = streams[i].videoFrameRateScale_;
            }
        }
    }

    // If this is manifest index 0, we need a video header to parse and we need
    // valid max image width and height so we can open the playback device. If
    // this is not manifest index 0, the playback device is already open. We do
    // not need a header to parse and we don't care what the max image width and
    // height are since the device has already allocated the video decoder.
    if (manifestIndex == 0 &&
        (primaryVideoAttr == NULL || maxImageWidth == 0 || maxImageHeight == 0)) {
        Log::error(TRACE_MEDIAPLAYBACK, "PlaybackDevice::analyzeStreamInfo error: "
                   "Failed to locate video attributes for manifest 0.");
        return NFErr_Bad;
    }

    drmHeaderReceived_ = ( drmHeader_.size() > 0 );

    return NFErr_OK;
}

NFErr PlaybackDevice::setStreamInfo(const std::vector<StreamInfo>& streams, uint32_t manifestIndex)
{
    VideoAttributes* primaryVideoAttr = NULL;
    uint32_t maxImageWidth = 0, maxImageHeight = 0;
    uint32_t maxFrameRateValue = 0, maxFrameRateScale = 1;

    MCSIM_INPUT_DUMP_SETSTREAMINFO(streams, manifestIndex);
    return analyzeStreamInfo(streams,
                             manifestIndex,
                             primaryVideoAttr,
                             maxImageWidth,
                             maxImageHeight,
                             maxFrameRateValue,
                             maxFrameRateScale);
}

void PlaybackDevice::close()
{
    // Release stream players before the playback group so that the playback
    // group destructer destroys the players.
    if(streamPlayers_[AUDIO]) {
        playbackGroup_->destroyStreamPlayer(streamPlayers_[AUDIO]);
        streamPlayers_[AUDIO] = NULL;
    }

    if(streamPlayers_[VIDEO]) {
        playbackGroup_->destroyStreamPlayer(streamPlayers_[VIDEO]);
        streamPlayers_[VIDEO] = NULL;
    }

    if(playbackGroup_)
    {
        ESManager_->destroyPlaybackGroup(playbackGroup_);
        playbackGroup_ = NULL;
    }
    numRemainingFeedDatasToReport[AUDIO] = 10;
    numRemainingFeedDatasToReport[VIDEO] = 10;

    // Release callbacks after the playback group so that the stream players
    // will not be calling on the callbacks.
    playerCallbacks_[AUDIO].reset();
    playerCallbacks_[VIDEO].reset();
    if (demuxer_)
        demuxer_->close();
    demuxer_.reset();
    eventQueue_.reset();
    drmHeader_.clear();
    nflxHeader_.clear();
    drmHeaderReceived_ = false;
    primaryVideoAttr_ = NULL;
    initialized_ = false;

    uint32_t j;
    std::map<int,std::vector<StreamAttributes> >::iterator it;
    for(it = streamsAttributes_.begin(); it != streamsAttributes_.end(); it++)
    {
        for( j = 0; j < it->second.size(); j++)
        {
            if(it->second[j].audioAttributes_)
                delete it->second[j].audioAttributes_;

            if (it->second[j].videoAttributes_)
            {
                if (it->second[j].videoAttributes_->mvcAttributes_)
                {
                    delete it->second[j].videoAttributes_->mvcAttributes_;
                }
                delete it->second[j].videoAttributes_;
            }
        }
    }
    streamsAttributes_.clear();

    streams_.clear();

    format3D_ = esplayer::NOT_3D;

    MCSIM_INPUT_DUMP_CLOSE();
}

NFErr PlaybackDevice::initVideoPlayer(VideoAttributes *videoAttr,
                                      uint32_t maxImageWidth,
                                      uint32_t maxImageHeight,
                                      uint32_t maxFrameRateValue,
                                      uint32_t maxFrameRateScale)
{
    NFErr err;

    NRDP_UNUSED(maxFrameRateValue);
    NRDP_UNUSED(maxFrameRateScale);

    esplayer::VideoAttributes ESVideoAttr;
    esplayer::MediaAttributes ESMediaAttr;
    esplayer::StreamPlayerInitData ESInitData;

    if (videoAttr == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "PlaybackDevice::initVideoESPlayer: "
                   "videoAttr = NULL");
        return NFErr_Bad;
    }

    ESMediaAttr.mVideoAttributes = &ESVideoAttr;
    ESMediaAttr.mAudioAttributes = NULL;
    ESMediaAttr.mStreamType = esplayer::VIDEO;

    ESInitData.mMaxWidth = maxImageWidth;
    ESInitData.mMaxHeight = maxImageHeight;
    ESInitData.mMaxVideoFrameRateValue = maxFrameRateValue;
    ESInitData.mMaxVideoFrameRateScale = maxFrameRateScale;
    ESInitData.mColorPrimaries = videoAttr->colorPrimaries_;
    ESInitData.mTransferCharacteristics = videoAttr->transferCharacteristics_;
    ESInitData.mMatrixCoefficient = videoAttr->matrixCoefficients_;
    ESInitData.mFullRange = videoAttr->fullRange_;
    ESInitData.mDrmHeader = drmHeader_.size() ? &(drmHeader_[0]) : 0;
    ESInitData.mDrmHeaderLen = drmHeader_.size();

    const bool sendStreamInfo = NrdConfiguration::sendStreamInfoEvents();
    if (sendStreamInfo
#ifdef NRDP_HAS_TRACING
        || true
#endif
        )
    {
        const std::string info = StringFormatter::sformat("Movie-wide StreamInfo for ESPlayer: max width %d, max height %d, framerate value/scale  %d/%d, color primaries %d, transfer characteristics %d, matrix coefficient %d, fullrange %d, drm header length %d",
                                                          ESInitData.mMaxWidth,
                                                          ESInitData.mMaxHeight,
                                                          ESInitData.mMaxVideoFrameRateValue,
                                                          ESInitData.mMaxVideoFrameRateScale,
                                                          ESInitData.mColorPrimaries,
                                                          ESInitData.mTransferCharacteristics,
                                                          ESInitData.mMatrixCoefficient,
                                                          ESInitData.mFullRange,
                                                          ESInitData.mDrmHeaderLen);

#ifdef NRDP_HAS_TRACING
        Log::Message msg(TRACE_MEDIAPLAYBACK, Log::Trace);
        msg.m_message = info;
        msg.send();
#endif
        if (sendStreamInfo)
            eventQueue_->postEvent(EVENT_STREAM_INFO, 0, 0, info);

    }

    Mp4Demultiplexer::copyAttributes(videoAttr,
                                     ESMediaAttr,
                                     videoAttr->format3D_);

    ESMediaAttr.mVideoAttributes->mFrameRateValue = ESInitData.mMaxVideoFrameRateValue;
    ESMediaAttr.mVideoAttributes->mFrameRateScale = ESInitData.mMaxVideoFrameRateScale;
    ESInitData.mInitialStreamAttributes = &ESMediaAttr;

    assert(playerCallbacks_[VIDEO].get());
    err = playbackGroup_->createStreamPlayer(ESInitData,
                                             playerCallbacks_[VIDEO],
                                             streamPlayers_[VIDEO]);
    if(!err.ok()){
        IDeviceError* deviceErr = reinterpret_cast<IDeviceError*> (err.peek().get());
        esplayer::Status status = deviceErr->getCategoryCode();
        if (status == esplayer::SPLIT_MVC_REQUIRED) {
            format3D_ = esplayer::MVC_SPLIT;
            ESMediaAttr.mVideoAttributes->mFormat3D = format3D_;
            err = playbackGroup_->createStreamPlayer(ESInitData,
                                                     playerCallbacks_[VIDEO],
                                                     streamPlayers_[VIDEO]);
        }
    }
    return err;
}

// At initialization time, we don't have header information about the audio
// stream.  We initialize the player with defaults.  When the audio stream is
// received, the media attributes delivered with the first sample allow the
// decoder to re-initialize.
NFErr PlaybackDevice::initAudioPlayer()
{
    NFErr err;
    esplayer::AudioAttributes ESAudioAttr;
    esplayer::MediaAttributes ESMediaAttr;
    esplayer::StreamPlayerInitData ESInitData;

    ESAudioAttr.mNumberOfChannels = 0;
    ESAudioAttr.mSamplesPerSecond = 0;
    ESAudioAttr.mBitrate = 0;
    ESAudioAttr.mBlockAlignment = 0;
    ESAudioAttr.mCodecSpecificData = 0;
    ESAudioAttr.mCodecSpecificDataLen = 0;
    ESMediaAttr.mAudioAttributes = &ESAudioAttr;
    ESMediaAttr.mVideoAttributes = NULL;
    ESMediaAttr.mStreamType = esplayer::AUDIO;
    ESInitData.mInitialStreamAttributes = &ESMediaAttr;
    ESInitData.mMaxWidth = ESInitData.mMaxHeight = 0;
    ESInitData.mDrmHeader = drmHeader_.size() ? &(drmHeader_[0]) : 0;
    ESInitData.mDrmHeaderLen = drmHeader_.size();

    assert(playerCallbacks_[AUDIO].get());
    err = playbackGroup_->createStreamPlayer(ESInitData, playerCallbacks_[AUDIO], streamPlayers_[AUDIO]);
    return err;
}

NFErr PlaybackDevice::feedStreamHeader(const std::vector<unsigned char>& headerData,
                                       uint32_t streamIndex,
                                       uint32_t manifestIndex,
                                       StreamType streamType)
{
    assert( initialized_ );

    NFErr err;


    if((streamsAttributes_.find(manifestIndex) == streamsAttributes_.end()) ||
       (streamsAttributes_[manifestIndex].size() <= streamIndex))
    {
        Log::warn(TRACE_MEDIAPLAYBACK,
                "PlaybackDevice::feedStreamHeader: manifest index %u",
                manifestIndex);
        assert(streamsAttributes_.find(manifestIndex) != streamsAttributes_.end());
        Log::warn(TRACE_MEDIAPLAYBACK,
                "PlaybackDevice::feedStreamHeader: streamsAttributes_ size: %zu, index: %u",
                streamsAttributes_[manifestIndex].size(), streamIndex);
        assert(streamsAttributes_[manifestIndex].size() > streamIndex);
        return NFErr_Bad;
    }

    MCSIM_INPUT_DUMP_FEEDSTREAMHEADER(headerData, streamIndex, manifestIndex,
                                      streamType);

    if (streamsAttributes_[manifestIndex][streamIndex].audioAttributes_ == NULL &&
        streamsAttributes_[manifestIndex][streamIndex].videoAttributes_ == NULL)
    {
        err = parseStreamHeader(containerType_, streamType, headerData,
                                streamsAttributes_[manifestIndex][streamIndex], NULL);
        if (!err.ok())
        {
            Log::warn(TRACE_MEDIAPLAYBACK,
                      "PlaybackDevice::feedStreamHeader: Error in parseStreamHeader");
            return err;
        }
    }

    // before feeding data block, set additional informations from manifest to
    // streamsAttributes.  Information in streamsAttributes are only from header
    // of stream. It lacks information from NCCP
    if(streamsAttributes_[manifestIndex][streamIndex].audioAttributes_){
        // audio bitrate
        streamsAttributes_[manifestIndex][streamIndex].audioAttributes_->nominalBitrateInKbps_ =
            streams_[manifestIndex][streamIndex].nominalBitrateInKbps_;
    } else if (streamsAttributes_[manifestIndex][streamIndex].videoAttributes_) {
        // video bitrate
        streamsAttributes_[manifestIndex][streamIndex].videoAttributes_->nominalBitrateInKbps_ =
            streams_[manifestIndex][streamIndex].nominalBitrateInKbps_;
        /*
        printf(" StreamInfo from feedStreamHeader bitrate %d\n",
            streams_[manifestIndex][streamIndex].nominalBitrateInKbps_);
        */

        // video framerate
        streamsAttributes_[manifestIndex][streamIndex].videoAttributes_->frameRateValue_ =
            streams_[manifestIndex][streamIndex].videoFrameRateValue_;
        streamsAttributes_[manifestIndex][streamIndex].videoAttributes_->frameRateScale_ =
            streams_[manifestIndex][streamIndex].videoFrameRateScale_;
        /*
        printf(" StreamInfo from PBD::feedStreamHeader video framerate %d/%d\n",
               streams_[manifestIndex][streamIndex].videoFrameRateValue_,
               streams_[manifestIndex][streamIndex].videoFrameRateScale_);
        */
    }

    if(drmHeader_.size() > 0)
    {
        drmHeaderReceived_ = true;
    }
    return NFErr_OK;
}



NFErr PlaybackDevice::feedVideoStreamHeader(const std::vector<unsigned char>& headerData,
                                            const VideoAttributesFromManifest& additionalVideoAttributes,
                                            uint32_t downloadableId)
{
    NRDP_UNUSED(headerData);
    NRDP_UNUSED(additionalVideoAttributes);
    NRDP_UNUSED(downloadableId);
    return NFErr_OK;
}

NFErr PlaybackDevice::feedAudioStreamHeader(const std::vector<unsigned char>& headerData,
                                            const AudioAttributesFromManifest& additionalAudioAttributes,
                                            uint32_t downloadableId)
{
    NRDP_UNUSED(headerData);
    NRDP_UNUSED(additionalAudioAttributes);
    NRDP_UNUSED(downloadableId);
    return NFErr_OK;
}

NFErr PlaybackDevice::feedDrmHeader(const std::vector<unsigned char>& drmHeader)
{
    NRDP_UNUSED(drmHeader);
    return NFErr_OK;
}

NFErr PlaybackDevice::feedData(IBufferManager::Buffer& dataBuffer,
                               StreamType streamType,
                               uint32_t downloadableId)
{
    NRDP_UNUSED(dataBuffer);
    NRDP_UNUSED(streamType);
    NRDP_UNUSED(downloadableId);
    return NFErr_OK;
}

NFErr PlaybackDevice::feedData(IBufferManager::Buffer& dataBuffer,
                               uint32_t streamIndex,
                               uint32_t manifestIndex,
                               StreamType streamType)
{
    assert( initialized_ );

    if (endOfStream_[streamType])
        return NFErr_NotAllowed;

    if(streamsAttributes_.find(manifestIndex) == streamsAttributes_.end())
    {
        Log::warn(TRACE_MEDIAPLAYBACK, "PlaybackDevice::feedData() received data for"
                  " unknown manifest %u", manifestIndex);
        assert(streamsAttributes_.find(manifestIndex) != streamsAttributes_.end());
        return NFErr_ArrayOutOfBoundary;
    }

    if (streamIndex >= streamsAttributes_[manifestIndex].size())
        return NFErr_ArrayOutOfBoundary;

    if(playerCallbacks_[AUDIO]->playbackCompletedReceived())
    {
        // We've already received playback complete for this stream.  This could
        // occur if we got a last minute on-the-fly audio change. Set the block to
        // not in use and return OK.
        NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::feedData() received %s data after"
               " playback complete received", streamType == AUDIO ? "audio" : "video");
        *dataBuffer.inUse_ = false;
        return NFErr_OK;
    }


    Mp4Demultiplexer::DataBlock block;
    block.setDataBuffer(dataBuffer);
    block.setStreamAttributes(&streamsAttributes_[manifestIndex][streamIndex]);

    if (streamDataBlocks_[streamType]->push_back(block) == false) {
        return NFErr_Pending;
    }

    if(numRemainingFeedDatasToReport[streamType] > 0)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "FeedData success: type %s, timestamp %d",
               streamType == AUDIO ? "Audio" : "Video", dataBuffer.timestamp_);
        numRemainingFeedDatasToReport[streamType]--;
    }

    MCSIM_INPUT_DUMP_FEEDDATA(dataBuffer, streamIndex, manifestIndex,
                              streamType);

    return NFErr_OK;
}

NFErr PlaybackDevice::setSpeed(int32_t speed)
{
    assert( initialized_ );

    if (speed < 0)
        return NFErr_Bad; // reverse playback not supported

    videoFrameStepping_ = false;

    if (speed == 0) {

        playbackGroup_->setPlaybackState(esplayer::IPlaybackGroup::PAUSE);

    } else {

        if(playbackGroup_->getPlaybackState() == esplayer::IPlaybackGroup::PAUSE)
        {
            // We're about to transition from pause to play. Make sure that the
            // underflow reported flag in the callback is not set. If it is set,
            // the callback will prevent the device from reporting underflow.
            playerCallbacks_[VIDEO]->resetUnderflowReported();
            playerCallbacks_[AUDIO]->resetUnderflowReported();
        }
        lastPostedPresentationTimestamp_ = INVALID_TIMESTAMP;
        if(playbackGroup_->setPlaybackState(esplayer::IPlaybackGroup::PLAY) == false)
        {
            // Stream players aren't ready to start playback. The pumping loop
            // will wait a few ms and try again.
            return NFErr_NotReady;
        }
        PERFTRACE("MCQ2 - PlaybackDevice::setSpeed time=%llu\n", Time::mono().val());
    }

    return NFErr_OK;
}

// The audio player callback calls enable audio when the audio player has been
// disabled due to an on-the-fly audio track change. The audio player was disabled
// to do the switch.  It should be enabled now because it is ready for playback.
void PlaybackDevice::enableAudio()
{
    streamPlayers_[AUDIO]->enable();
}

// The order of events inside flush is important.  The playbackGroup's state
// must first be set to PAUSE so that the stream players don't report an
// underflow before they get the message to flush. Next, the audio and video
// callbacks need to be set to beginFlush so that they raise a flag to not allow
// their respective players to getNextSample. Then, playbackGroup_->flush is
// called which flushes the audio and video stream players.  When the method
// returns, the audio and video players have flushed and are running again but
// cannot get into the Mp4Demultiplexer via getNextMediaSample because the
// callbacks are still in the flush state which prevents getNextMediaSample from
// succeeding. Since the audio and video consumer threads are guaranteed to not
// be accessing the stream data block lists via the Mp4Demultiplexer, we can
// safely clear the data block list and free all the stream data blocks that
// were in use.  Once that's done we can safely re-enable the audio and video
// player callbacks.
NFErr PlaybackDevice::flush()
{
    // nothing to flush if we're not initialized
    if (!initialized_)
        return NFErr_OK;

    playbackGroup_->setPlaybackState(esplayer::IPlaybackGroup::PAUSE);
    playerCallbacks_[AUDIO]->beginFlush();
    playerCallbacks_[VIDEO]->beginFlush();
    playbackGroup_->flush();
    streamDataBlocks_[AUDIO]->clear();
    streamDataBlocks_[VIDEO]->clear();
    demuxer_->setDiscontinuity();
    endOfStream_[AUDIO] = false;
    endOfStream_[VIDEO] = false;
    videoFrameStepping_ = false;
    requested_speed_ = 0;
    lastPostedPresentationTimestamp_ = INVALID_TIMESTAMP;
    playerCallbacks_[AUDIO]->endFlush();
    playerCallbacks_[VIDEO]->endFlush();

    return NFErr_OK;
}

NFErr PlaybackDevice::flushAudio()
{
    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::flushAudio");
    streamPlayers_[AUDIO]->disable();
    playerCallbacks_[AUDIO]->beginFlush();
    streamPlayers_[AUDIO]->flush();
    streamDataBlocks_[AUDIO]->clear();
    demuxer_->setAudioDiscontinuity();
    endOfStream_[AUDIO] = false;
    playerCallbacks_[AUDIO]->endFlush();

    // We enable the stream player here and the playback group is in the PLAY
    // state, but the stream player may not have gotten any samples yet. The
    // stream player has to be smart enough to not begin playback or report
    // underflow until it is ready to begin playback.
    streamPlayers_[AUDIO]->enable();

    return NFErr_OK;
}

NFErr PlaybackDevice::endOfStream(StreamType type)
{
    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::endOfStream received");

    // Don't send endOfStream if playback complete has already been received. In
    // the case when playback complete is received on the player thread but the
    // audio is being flushed for on-the-fly stream switch, if we send
    // endOfStream, the player may report playbackComplete again.
    //
    // Because flushing of the decoder and renderer threads guarantees that they
    // have progressed beyond the point that they could raise our
    // playbackComplete flag, we can be sure that playbackComplete will not be
    // signalled from completion of a track pre-flush if
    // playbackCompleteReceived is not raised here.
    if (!endOfStream_[type] && !playerCallbacks_[type]->playbackCompletedReceived()) {
        endOfStream_[type] = true;
        demuxer_->deliverEndOfStream(type);
    }

    return NFErr_OK;
}

NFErr PlaybackDevice::getEvent(EventID& eventID, ullong& eventParam1,
                               ullong& eventParam2, std::string& eventParam3)
{
    int eventid;
    NFErr err;

    err = eventQueue_->getEvent(eventid, eventParam1, eventParam2, eventParam3) ? NFErr_OK : NFErr_NotFound;
    eventID = static_cast<EventID>(eventid);
    return err;
}

NFErr PlaybackDevice::getChallenge(std::vector<unsigned char>& challengeData,
                                   std::vector<unsigned char>& nflxHeader)
{
    NFErr err = NFErr_OK;

    // If drmHeader_ is empty, this is clear content.  The AdaptiveStreamingPlayer
    // interprets a NotAllowed error to mean that the content is clear.
    if (drmHeader_.size() == 0) {
        return NFErr_NotAllowed;
    }

    // return the netflix header
    nflxHeader = nflxHeader_;

    challengeData.resize(0);

    const std::string str = DrmManager::CONTENTID_FOR_SINGLE_SESSION_MODE;
    /*
     * For single session drm usage with legacy player, always one playback is
     * outstanding. Therefore, if there is any drm session left in drmmanager,
     * it must be aborted playback, but license cleaning may have not been
     * performed because pumping thread does not call clear license if pumping
     * thread was not running.
     *
     * So, we need to flush all outstanding drm sessions before acquiring new
     * challenge. If not, since we are using same "fake" keyid, decryption can
     * be failing because it can get wrong license with same fake id.
     */
    err = DrmManager::instance()->flushDrmSessions(); // not really need to check error status
    err = DrmManager::instance()->generateChallenge(str, STANDARD_LICENSE, drmHeader_, challengeData, mSessionId);
    return err;
}

NFErr PlaybackDevice::storeLicense(const std::vector<unsigned char>& licenseData,
                                   /*out*/std::vector<unsigned char>& secureStopID,
                                   /*out*/uint32_t& drmSessionId)
{
    NFErr err = NFErr_OK;

    err =  DrmManager::instance()->provideLicense(mSessionId, licenseData, secureStopID_);

    if (err.ok()) {
        // copy drmSessionId_ to sessionID
        secureStopID = secureStopID_;
        drmSessionId = mSessionId;
        return err;
    }
    err.push(NFErr_DRMFailed);
    return err;
}

NFErr PlaybackDevice::clearLicense()
{
    NFErr err;

    // Previously, we cleared the license from HDS. This may not be needed since
    // with new drm architecture, we clears the license when app shut down.
    err = NFErr_OK;

    if (!err.ok()) {
        err.push(NFErr_DRMFailed);
    }
    return err;
}

uint32_t PDNATIVE_EXECUTE_FRAME_STEPPING = 0;
uint32_t PDNATIVE_EXECUTE_CONFIG_SET = 1;
uint32_t PDNATIVE_EXECUTE_CONFIG_GET = 2;

uint32_t PDNATIVE_CONFIG_PTS_EVENT_PERIOD = 15;
uint32_t PDNATIVE_CONFIG_EVENT_QUEUE_SIZE = 16;

NFErr PlaybackDevice::execute(uint32_t id, uint32_t& param1, uint32_t& param2)
{
    if (id == PDNATIVE_EXECUTE_FRAME_STEPPING) {
        assert(playbackGroup_);
        if (playbackGroup_->getPlaybackState() != esplayer::IPlaybackGroup::PAUSE) {
            playbackGroup_->setPlaybackState(esplayer::IPlaybackGroup::PAUSE);
        } else {
            videoFrameStepping_ = true;
        }

        return NFErr_OK;
    }

    if (id == PDNATIVE_EXECUTE_CONFIG_SET) {
#define TRY_SET_CONFIG(__name__)\
        if(param1 == PDNATIVE_CONFIG_##__name__)\
        {\
            DeviceConstants::__name__ = param2;   \
            return NFErr_OK;\
        }

        TRY_SET_CONFIG(PTS_EVENT_PERIOD);
        TRY_SET_CONFIG(EVENT_QUEUE_SIZE);

        return NFErr_NotFound;
    }

    if (id == PDNATIVE_EXECUTE_CONFIG_GET) {
#define TRY_GET_CONFIG(__name__)\
        if(param1 == PDNATIVE_CONFIG_##__name__)\
        {\
            param2 = DeviceConstants::__name__;   \
            return NFErr_OK;\
        }

        TRY_GET_CONFIG(PTS_EVENT_PERIOD);
        TRY_GET_CONFIG(EVENT_QUEUE_SIZE);

        return NFErr_NotFound;
    }

    return NFErr_NotFound;
}

NFErr PlaybackDevice::parseStreamHeader(
    ContainerType containerType, StreamType streamType,
    const std::vector<unsigned char>& headerData, StreamAttributes& streamsAttr,
    std::vector<unsigned char>* drmHeader)
{

    NFErr err;
    NRDP_UNUSED(drmHeader);

    if (streamType == AUDIO)
        streamsAttr.audioAttributes_ = new AudioAttributes;

    if (streamType == VIDEO)
        streamsAttr.videoAttributes_ = new VideoAttributes;

    switch (containerType) {
    case PIFF_CONTAINER:
    case DASH_CONTAINER:
        err = Mp4Demultiplexer::parseStreamHeader(
            &headerData[0],headerData.size(), streamsAttr, &drmHeader_,
            DeviceConstants::PLAYREADY_SYSTEM_ID, &nflxHeader_);
        if (!err.ok())
            return err;
        break;

    default:
        assert(false);
        // unexpected
    }

    return NFErr_OK;
}

void PlaybackDevice::playbackCompleteReported(esplayer::MediaType esMediaType)
{
    // Convert IESP::MediaType to IPlaybackDevice::StreamType.
    StreamType streamType = (esMediaType == esplayer::AUDIO ? AUDIO : VIDEO);

    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::playbackCompleteReported() "
           "media type: %s", esMediaType == esplayer::AUDIO ? "audio": "video");

    // Shut down if audio is done
    if(streamType == AUDIO) {

        // We should only be receiving one playbackComplete event from each
        // elementary stream player.
        if (eventQueue_->postEvent(EVENT_END_OF_STREAM)) {
            NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::playbackCompleteReported() "
                   "posted end of stream event.");
        }
    }
}

bool PlaybackDevice::reportUnderflow()
{
    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::underflowReported(): "
           " posting EVENT_DATA_EXHAUSTED");
    if (eventQueue_->postEvent(EVENT_DATA_EXHAUSTED)) {
        numRemainingFeedDatasToReport[AUDIO] = 10;
        numRemainingFeedDatasToReport[VIDEO] = 10;
        return true;
    }
    return false;
}

void PlaybackDevice::positionUpdateReported(const llong& pts_in, const ullong& monotimeInMS)
{
    llong pts = (pts_in >= 0) ? pts_in : 0;
    if (lastPostedPresentationTimestamp_ == INVALID_TIMESTAMP ||
        (pts >= lastPostedPresentationTimestamp_ + DeviceConstants::PTS_EVENT_PERIOD)) {
        if(lastPostedPresentationTimestamp_ == INVALID_TIMESTAMP)
        {
            PERFTRACE("MCQ2 - PlaybackDevice::first PTS reported time=%llu\n", Time::mono().val());
        }

        NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::positionUpdateReported(): "
               " posting EVENT_PTS_UPDATE, pts: %lld generated at %lld", pts, monotimeInMS);
        if (eventQueue_->postEvent(EVENT_PTS_UPDATE, pts, monotimeInMS)){
            lastPostedPresentationTimestamp_ = pts;
            demuxer_->setCurrentPlaybackPosition(pts);
        }
    }
}

void PlaybackDevice::errorReported(esplayer::Status errorCategory,
                                   ullong errorCode,
                                   const std::string& errorMessage)
{
    Log::error(TRACE_MEDIAPLAYBACK, "PlaybackDevice::errorReported: code: %llx, message: %s",
              errorCode, errorMessage.c_str());
    eventQueue_->postEvent(EVENT_ERROR, errorCategory, errorCode, errorMessage);
}

Rect PlaybackDevice::setVideoWindow(Rect targetRect, uint32_t transitionDuration)
{
    assert( initialized_ );

    Rect rect_NRD;
    esplayer::IPlaybackGroup::Rect rect_IESP, targetRect_IESP;

    targetRect_IESP.x = targetRect.x;
    targetRect_IESP.y = targetRect.y;
    targetRect_IESP.width = targetRect.width;
    targetRect_IESP.height = targetRect.height;

    rect_IESP = playbackGroup_->setVideoWindow(targetRect_IESP, transitionDuration);

    rect_NRD.x = rect_IESP.x;
    rect_NRD.y = rect_IESP.y;
    rect_NRD.width = rect_IESP.width;
    rect_NRD.height = rect_IESP.height;

    return rect_NRD;
}

void PlaybackDevice::bringVideoToFront()
{
    if(playbackGroup_)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::bringVideoToFront() called");
        playbackGroup_->bringVideoToFront();
    }
}

void PlaybackDevice::sendVideoToBack()
{
    if(playbackGroup_)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::sendVideoToBack() called");
        playbackGroup_->sendVideoToBack();
    }
}

NFErr PlaybackDevice::setDisplayArea(uint32_t /*x*/, uint32_t /*y*/, uint32_t /*width*/, uint32_t /*height*/)
{
    return NFErr_NotAllowed; // TODO
}

bool PlaybackDevice::drmHeaderReceived()
{
    return drmHeaderReceived_;
}

void PlaybackDevice::getPlaybackQualityStats(PlaybackQualityStats& audioStats,
                                             PlaybackQualityStats& videoStats)
{
    if ( initialized_ )
    {
        playerCallbacks_[AUDIO]->getPlaybackQualityStats(audioStats);
        playerCallbacks_[VIDEO]->getPlaybackQualityStats(videoStats);
    }
}

double PlaybackDevice::getVolume()
{
    if(streamPlayers_[AUDIO])
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::getVolume() called");
        return streamPlayers_[AUDIO]->getVolume();
    } else {
        return targetVolume_;
    }
}

void PlaybackDevice::setVolume(double targetVolume, uint32_t transitionDuration, IAudioMixer::Ease ease)
{
    targetVolume_ = targetVolume;
    if(streamPlayers_[AUDIO])
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::setVolume(%f, %d, %d) called",
               targetVolume, transitionDuration, ease);
        return streamPlayers_[AUDIO]->setVolume(targetVolume, transitionDuration, ease);
    }
}
