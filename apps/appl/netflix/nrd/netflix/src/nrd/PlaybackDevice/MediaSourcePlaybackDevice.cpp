/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DeviceConstants.h"
#include "EventQueue.h"
#include "MediaSourcePlaybackDevice.h"

using namespace std;
using namespace netflix;
using namespace netflix::device;
using namespace netflix::device::esplayer;

static const llong  INVALID_TIMESTAMP = (  -1);

MediaSourcePlaybackDevice::MediaSourcePlaybackDevice(shared_ptr<esplayer::IElementaryStreamManager> ESManager,
                                                     shared_ptr<ISystem> nrdSystem)
  : mNrdSystem(nrdSystem)
  , mESManager(ESManager)
  , mPlaybackGroup(NULL)
{
    mEndOfStream[AUDIO] = false;
    mEndOfStream[VIDEO] = false;
    mStreamPlayer[AUDIO] = NULL;
    mStreamPlayer[VIDEO] = NULL;
}

MediaSourcePlaybackDevice::~MediaSourcePlaybackDevice()
{
    close();
}

NFErr MediaSourcePlaybackDevice::open(
    const device::IPlaybackDevice::CommonVideoAttributesFromManifest &videoAttributes,
    const device::IPlaybackDevice::CommonAudioAttributesFromManifest &audioAttributes,
    uint32_t videoPipelineId)
{
    NTRACE(TRACE_MEDIASTARTUP, "MediaSourcePlaybackDevice::%s", __func__);

    NFErr err;

    // Init state variables
    mEndOfStream[AUDIO] = false;
    mEndOfStream[VIDEO] = false;
    mLastPostedPresentationTimestamp = INVALID_TIMESTAMP;

    // Create an event queue
    mEventQueue.reset(new EventQueue(DeviceConstants::EVENT_QUEUE_SIZE));

    // Create elementary stream player callbacks.
    // note that ESPlayerCallbackImpl is taking PlaybackDevice via *this.
    mPlayerCallback[AUDIO].reset(new ESPlayerCallback(*this, esplayer::AUDIO));
    mPlayerCallback[VIDEO].reset(new ESPlayerCallback(*this, esplayer::VIDEO));

    ////
    // Create elementary stream player PlaybackGroup. Audio and video
    //
    // elementary stream playback is controlled via the playback
    // group.
    //

    err = mESManager->createPlaybackGroup(mPlaybackGroup, videoPipelineId);
    if(!err.ok()){
        close();
        return err;
    }

    // Construct esplayer::StreamPlayerInitData
    esplayer::VideoAttributes ESVideoAttr;
    esplayer::AudioAttributes ESAudioAttr;
    esplayer::MediaAttributes ESMediaAttrForVideo;
    esplayer::MediaAttributes ESMediaAttrForAudio;
    esplayer::StreamPlayerInitData ESInitData;

    NTRACE(TRACE_MEDIACONTROL, "MediaSourcePlaybackDevice::open video attributes"
           "codecType %u, video3DType %u, maxWidth %u, maxHeight %u, PixelAspectRatio %u:%u, FramRate %u/%u",
           videoAttributes.codecType_,
           videoAttributes.video3DType_,
           videoAttributes.maxVideoWidth_,
           videoAttributes.maxVideoHeight_,
           videoAttributes.pixelAspectRatioX_,
           videoAttributes.pixelAspectRatioY_,
           videoAttributes.videoFrameRateValue_,
           videoAttributes.videoFrameRateScale_);

    ////
    // Create IElementaryStreamPlayer for video
    switch(videoAttributes.codecType_){
    case H264_VIDEO:
        ESVideoAttr.mCodecParam = "avc1";
        break;
    case H265_VIDEO:
    case H265_10BIT_VIDEO:
    case HDRA_HDR_VIDEO:
        ESVideoAttr.mCodecParam = "hev1";
        break;
    case DOLBYVISION_HDR_VIDEO:
        ESVideoAttr.mCodecParam = "dvhe";
        break;
    default:
        assert(0);
        break;
    }
    ESVideoAttr.mImageWidth = videoAttributes.maxVideoWidth_;
    ESVideoAttr.mImageHeight = videoAttributes.maxVideoHeight_;
    ESVideoAttr.mPixelAspectRatioX = videoAttributes.pixelAspectRatioX_;
    ESVideoAttr.mPixelAspectRatioY = videoAttributes.pixelAspectRatioY_;
    ESVideoAttr.mFrameRateValue = videoAttributes.videoFrameRateValue_;
    ESVideoAttr.mFrameRateScale = videoAttributes.videoFrameRateScale_;
    ESVideoAttr.mFormat3D = (videoAttributes.video3DType_== VIDEO_3D?
                             esplayer::MVC_COMBINED:
                             esplayer::NOT_3D);
    if( (videoAttributes.codecType_ == H265_10BIT_VIDEO) ||
        (videoAttributes.codecType_ == DOLBYVISION_HDR_VIDEO)){
        ESVideoAttr.mBitsDepthLuma = 10;
        ESVideoAttr.mBitsDepthChroma = 10;
    } else {
        ESVideoAttr.mBitsDepthLuma = 8;
        ESVideoAttr.mBitsDepthChroma = 8;
    }

    ESMediaAttrForVideo.mStreamType = esplayer::VIDEO;
    ESMediaAttrForVideo.mVideoAttributes = &ESVideoAttr;
    ESMediaAttrForVideo.mAudioAttributes = NULL;
    ESInitData.mInitialStreamAttributes = &ESMediaAttrForVideo;
    ESInitData.mMaxWidth = videoAttributes.maxVideoWidth_;
    ESInitData.mMaxHeight = videoAttributes.maxVideoHeight_;
    ESInitData.mMaxVideoFrameRateValue = videoAttributes.videoFrameRateValue_;
    ESInitData.mMaxVideoFrameRateScale = videoAttributes.videoFrameRateScale_;

    err = mPlaybackGroup->createStreamPlayer(ESInitData, mPlayerCallback[VIDEO], mStreamPlayer[VIDEO]);
    if(!err.ok()){
        IDeviceError* deviceErr = reinterpret_cast<IDeviceError*> (err.peek().get());
        esplayer::Status status = deviceErr->getCategoryCode();
        if (status == esplayer::SPLIT_MVC_REQUIRED) {
            ESMediaAttrForVideo.mVideoAttributes->mFormat3D = esplayer::MVC_SPLIT;
            err = mPlaybackGroup->createStreamPlayer(ESInitData,
                                                     mPlayerCallback[VIDEO],
                                                     mStreamPlayer[VIDEO]);
            if(!err.ok())
                return err;
        }
        if(!err.ok())
            return err;
    }

    // The callbacks are disabled by default so that we can initialize the
    // demultiplexer after the video player. In the case when the video player
    // wants MVC_SPLIT, we don't know until we try to initialize a player.
    mPlayerCallback[VIDEO]->enable();

    ////
    // Create IElementaryStreamPlayer for audio
    NTRACE(TRACE_MEDIAPLAYBACK
           ,"MediaSourcePlaybackDevice::open audio codec type %u, channels %d, sample/sec %d\n"
           , audioAttributes.codecType_
           , audioAttributes.mNumberOfChannels_
           , audioAttributes.mSamplesPerSecond_);

    switch(audioAttributes.codecType_){
    case DDPLUS_AUDIO:
        ESAudioAttr.mCodecParam = "ec-3";
        break;
    case HEAAC_AUDIO:
        ESAudioAttr.mCodecParam = "mp4a";
        break;
    case OGG_VORBIS_AUDIO:
        ESAudioAttr.mCodecParam = "vorbis";
        break;
    default:
        assert(0);
        break;
    }
    ESMediaAttrForAudio.mStreamType = esplayer::AUDIO;
    ESMediaAttrForAudio.mVideoAttributes = NULL;
    ESAudioAttr.mNumberOfChannels = audioAttributes.mNumberOfChannels_;
    ESAudioAttr.mSamplesPerSecond = audioAttributes.mSamplesPerSecond_;
    ESMediaAttrForAudio.mAudioAttributes = &ESAudioAttr;
    ESInitData.mInitialStreamAttributes = &ESMediaAttrForAudio;
    err = mPlaybackGroup->createStreamPlayer(ESInitData, mPlayerCallback[AUDIO], mStreamPlayer[AUDIO]);
    if(!err.ok()){
        return err;
    } else {
        mPlayerCallback[AUDIO]->enable();
    }

    mPlaybackDeviceInitialized = true;

    return err;
}

void  MediaSourcePlaybackDevice::close()
{
    NTRACE(TRACE_MEDIASTARTUP, "MediaSourcePlaybackDevice::%s", __func__);

    // Release stream players before the playback group so that the playback
    // group destructer destroys the players.
    if(mStreamPlayer[AUDIO]) {
        mPlaybackGroup->destroyStreamPlayer(mStreamPlayer[AUDIO]);
        mStreamPlayer[AUDIO] = NULL;
    }
    if(mStreamPlayer[VIDEO]) {
        mPlaybackGroup->destroyStreamPlayer(mStreamPlayer[VIDEO]);
        mStreamPlayer[VIDEO] = NULL;
    }
    if(mPlaybackGroup){
        mESManager->destroyPlaybackGroup(mPlaybackGroup);
        mPlaybackGroup = NULL;
    }

    // Release callbacks after the playback group so that the stream players
    // will not be calling on the callbacks.
    mPlayerCallback[AUDIO].reset();
    mPlayerCallback[VIDEO].reset();

    // Release event queue
    mEventQueue.reset();

    // Reset misc state variables
    mPlaybackDeviceInitialized = false;
}

void MediaSourcePlaybackDevice::setMediaSource(shared_ptr<mediacontrol::NrdpMediaSource> pMediaSource)
{
    mMediaSource = pMediaSource;
}

shared_ptr<mediacontrol::NrdpMediaSource> MediaSourcePlaybackDevice::getMediaSource()
{
    return mMediaSource;
}

NFErr MediaSourcePlaybackDevice::setSpeed(int32_t speed)
{
    assert( mPlaybackDeviceInitialized );

    if (speed < 0)
        return NFErr_Bad; // reverse playback not supported

    if (speed == 0) {
        mPlaybackGroup->setPlaybackState(esplayer::IPlaybackGroup::PAUSE);
    } else {
        if(mPlaybackGroup->getPlaybackState() == esplayer::IPlaybackGroup::PAUSE)
        {
            // We're about to transition from pause to play. Make sure that the
            // underflow reported flag in the callback is not set. If it is set,
            // the callback will prevent the device from reporting underflow.
            mPlayerCallback[VIDEO]->resetUnderflowReported();
            mPlayerCallback[AUDIO]->resetUnderflowReported();
        }
        if(mPlaybackGroup->setPlaybackState(esplayer::IPlaybackGroup::PLAY) == false)
        {
            // Stream players aren't ready to start playback. The pumping loop
            // will wait a few ms and try again.
            return NFErr_NotReady;
        }
        PERFTRACE("MCQ2 - PlaybackDevice::setSpeed time=%llu\n", Time::mono().val());
    }
    return NFErr_OK;
}

NFErr MediaSourcePlaybackDevice::flush()
{
    NTRACE(TRACE_MEDIASTARTUP, "MediaSourcePlaybackDevice::%s", __func__);

    // nothing to flush if we're not initialized
    if (!mPlaybackDeviceInitialized){
        Log::warn(TRACE_MEDIAPLAYBACK, "PlaybackDevice not initialized at %s", __func__);
        return NFErr_OK;
    }

    mPlaybackGroup->setPlaybackState(esplayer::IPlaybackGroup::PAUSE);

    mPlayerCallback[AUDIO]->beginFlush();
    mPlayerCallback[VIDEO]->beginFlush();
    mPlaybackGroup->flush();

    mEndOfStream[AUDIO] = false;
    mEndOfStream[VIDEO] = false;

    mLastPostedPresentationTimestamp = INVALID_TIMESTAMP;

    mPlayerCallback[AUDIO]->endFlush();
    mPlayerCallback[VIDEO]->endFlush();

    return NFErr_OK;
}

NFErr MediaSourcePlaybackDevice::flushAudio()
{
    NTRACE(TRACE_MEDIASTARTUP, "MediaSourcePlaybackDevice::%s", __func__);

    if(!mPlaybackDeviceInitialized)
        return NFErr_Bad;

    mStreamPlayer[AUDIO]->disable();
    mPlayerCallback[AUDIO]->beginFlush();
    mStreamPlayer[AUDIO]->flush();
    mEndOfStream[AUDIO] = false;
    mPlayerCallback[AUDIO]->endFlush();

    // We enable the stream player here and the playback group is in the PLAY
    // state, but the stream player may not have gotten any samples yet. The
    // stream player has to be smart enough to not begin playback or report
    // underflow until it is ready to begin playback.
    mStreamPlayer[AUDIO]->enable();

    return NFErr_OK;
}

void MediaSourcePlaybackDevice::enableAudio()
{
    mStreamPlayer[AUDIO]->enable();
}

NFErr MediaSourcePlaybackDevice::endOfStream(IPlaybackDevice::StreamType type)
{
    NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourcePlaybackDevice::endOfStream received");

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
    if (!mEndOfStream[type] && !mPlayerCallback[type]->playbackCompletedReceived()) {
        mEndOfStream[type] = true;
    }

    return NFErr_OK;
}

NFErr MediaSourcePlaybackDevice::getEvent(IPlaybackDevice::EventID& eventID,
                                          ullong& eventParam1,
                                          ullong& eventParam2,
                                          std::string& eventParam3)
{
    int eventid;
    NFErr err;
    err = mEventQueue->getEvent(eventid, eventParam1, eventParam2, eventParam3) ? NFErr_OK : NFErr_NotFound;
    eventID = static_cast<IPlaybackDevice::EventID>(eventid);
    return err;
}

Rect MediaSourcePlaybackDevice::setVideoWindow(Rect targetRect, uint32_t transitionDuration, uint32_t zOrder)
{
    assert( mPlaybackDeviceInitialized );

    Rect rect_NRD;
    esplayer::IPlaybackGroup::Rect rect_IESP, targetRect_IESP;

    targetRect_IESP.x = targetRect.x;
    targetRect_IESP.y = targetRect.y;
    targetRect_IESP.width = targetRect.width;
    targetRect_IESP.height = targetRect.height;

    rect_IESP = mPlaybackGroup->setVideoWindow(targetRect_IESP, transitionDuration, zOrder);

    rect_NRD.x = rect_IESP.x;
    rect_NRD.y = rect_IESP.y;
    rect_NRD.width = rect_IESP.width;
    rect_NRD.height = rect_IESP.height;

    return rect_NRD;
}

double MediaSourcePlaybackDevice::getVolume()
{
    if(mStreamPlayer[AUDIO])
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourcePlaybackDevice::getVolume() called");
        return mStreamPlayer[AUDIO]->getVolume();
    } else {
        return mTargetVolume;
    }
}

void MediaSourcePlaybackDevice::setVolume(double targetVolume,
                                          uint32_t transitionDuration,
                                          IAudioMixer::Ease ease)
{
    mTargetVolume = targetVolume;
    if(mStreamPlayer[AUDIO])
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourcePlaybackDevice::setVolume(%f, %d, %d) called",
               targetVolume, transitionDuration, ease);
        return mStreamPlayer[AUDIO]->setVolume(targetVolume, transitionDuration, ease);
    }
}

void MediaSourcePlaybackDevice::getPlaybackQualityStats(IPlaybackDevice::PlaybackQualityStats& audioStats,
                                                        IPlaybackDevice::PlaybackQualityStats& videoStats)
{
    if ( mPlaybackDeviceInitialized )
    {
        mPlayerCallback[AUDIO]->getPlaybackQualityStats(audioStats);
        mPlayerCallback[VIDEO]->getPlaybackQualityStats(videoStats);
    }
}

bool MediaSourcePlaybackDevice::reportUnderflow()
{
    NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourcePlaybackDevice::underflowReported():posting EVENT_DATA_EXHAUSTED");
    return mEventQueue->postEvent(IPlaybackDevice::EVENT_DATA_EXHAUSTED);
}

void MediaSourcePlaybackDevice:: playbackCompleteReported(esplayer::MediaType esMediaType)
{
    // Convert IESP::MediaType to IPlaybackDevice::StreamType.
    IPlaybackDevice::StreamType streamType = (esMediaType == esplayer::AUDIO ?
                                              IPlaybackDevice::AUDIO :
                                              IPlaybackDevice::VIDEO);

    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::playbackCompleteReported() "
           "media type: %s", esMediaType == esplayer::AUDIO ? "audio": "video");

    // Shut down if audio is done
    if(streamType == IPlaybackDevice::AUDIO) {

        // We should only be receiving one playbackComplete event from each
        // elementary stream player.
        if (mEventQueue->postEvent(IPlaybackDevice::EVENT_END_OF_STREAM)) {
            NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourcePlaybackDevice::playbackCompleteReported() "
                   "posted end of stream event.");
        }
    }
}

void MediaSourcePlaybackDevice::positionUpdateReported(const llong& pts_in, const ullong& monotimeInMS)
{
    llong pts = (pts_in >= 0) ? pts_in : 0;
    if (mLastPostedPresentationTimestamp == INVALID_TIMESTAMP ||
        (pts >= mLastPostedPresentationTimestamp + DeviceConstants::PTS_EVENT_PERIOD)) {
        if(mLastPostedPresentationTimestamp == INVALID_TIMESTAMP)
        {
            PERFTRACE("MCQ2 - MediaSourcePlaybackDevice::first PTS reported time=%llu\n", Time::mono().val());
        }

        NTRACE(TRACE_MEDIAPLAYBACK, "MediaSourcePlaybackDevice::positionUpdateReported(): "
               " posting EVENT_PTS_UPDATE, pts: %lld generated at %lld", pts, monotimeInMS);
        if (mEventQueue->postEvent(IPlaybackDevice::EVENT_PTS_UPDATE, pts, monotimeInMS)){
            mLastPostedPresentationTimestamp = pts;
        }
    }
}

void MediaSourcePlaybackDevice::errorReported(esplayer::Status errorCategory,
                                              ullong errorCode,
                                              const std::string& errorMessage)
{
    Log::error(TRACE_MEDIAPLAYBACK, "MediaSourcePlaybackDevice::errorReported: code: %llx, message: %s",
               errorCode, errorMessage.c_str());
    mEventQueue->postEvent(IPlaybackDevice::EVENT_ERROR, errorCategory, errorCode, errorMessage);
}
