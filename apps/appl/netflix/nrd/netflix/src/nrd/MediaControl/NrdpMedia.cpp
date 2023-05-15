/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NrdpMedia.h"
#include "NrdApplication.h"
#include <nrd/ISystem.h>

using namespace std;
using namespace netflix;
using namespace netflix::ase;
using namespace netflix::mediacontrol;
using namespace netflix::device;
using namespace netflix::util;

static const uint32_t MAX_DEVICE_NOT_READY_ERROR_WAIT_TIME = 60000; // 60000ms for max wait time
static const Time SLEEP_WAITING_READY_FOR_PLAY = 30;               // 30ms for sleep

/*
 * ************************
 * Local utitlity functions
 * ************************
 */
static void checkProfileDuplication(vector<ContentProfile>& profiles)
{
    vector<ContentProfile>::iterator iter1;
    for (iter1 = profiles.begin(); iter1 != profiles.end(); iter1 ++)
    {
        vector<ContentProfile>::iterator iter2 = iter1;
        iter2 ++;

        // Duplicated inclusion of profiles is unexpected.
        assert(find(iter2, profiles.end(), *iter1) == profiles.end());
    }
}

static void checkContainerConsistency(ContainerType& containerType, ContainerType myType)
{
    assert(myType != UNKNOWN_CONTAINER);

    if (containerType == UNKNOWN_CONTAINER)
        containerType = myType;

    assert(myType == containerType);
}

static void checkDrmConsistency(DrmType& drmType, DrmType myType)
{
    if (myType != NO_DRM)
    {
        if (drmType == NO_DRM)
            drmType = myType;

        assert(myType == drmType);
    }
}

static void checkProfiles(vector<ContentProfile>& videoProfiles,
                          vector<ContentProfile>& audioProfiles)
{
    // at least one video profile and audio profile is required.
    ASSERT(videoProfiles.size() > 0, "The DPI doesn't support any video profiles");
    ASSERT(audioProfiles.size() > 0, "The DPI doesn't support any audio profiles");

    checkProfileDuplication(videoProfiles);
    checkProfileDuplication(audioProfiles);

    VideoCodecType videoCodecType = UNKNOWN_VIDEO_CODEC;
    (void)(videoCodecType);
    ContainerType containerType = UNKNOWN_CONTAINER;
    DrmType drmType = NO_DRM;

    vector<ContentProfile>::iterator iter;

    for (iter = videoProfiles.begin(); iter != videoProfiles.end(); iter ++)
    {
        checkContainerConsistency(containerType, profileContainerType(*iter));
        checkDrmConsistency(drmType, profileDrmType(*iter));
    }

    for (iter = audioProfiles.begin(); iter != audioProfiles.end(); iter ++)
    {
        AudioCodecType myAudioCodecType = profileAudioCodecType(*iter);
        NRDP_UNUSED(myAudioCodecType);
        assert(myAudioCodecType != UNKNOWN_AUDIO_CODEC);

        checkContainerConsistency(containerType, profileContainerType(*iter));
        checkDrmConsistency(drmType, profileDrmType(*iter));
    }
}
/*
 * ***********************
 * constructor/desctructor
 * ***********************
 */
NrdpMediaSourcePlayer::NrdpMediaSourcePlayer()
  : state_(IAdaptiveStreamingPlayer::CLOSED)
  , abortOpen_(false)
  , deviceOpened_(false)
  , playbackStarted_(false)
  , dataExhausted_(false)
  , endOfPresentation_(false)
  , currentPts_(INVALID_PTS)
  , latestReportedPTS_(INVALID_PTS)
  , abortSetSpeed_(false)
  , audioSwitchingInProgress_(false)
  , deviceErrorReported(NFErr_OK)
  , newVideoWindowPending_(false)
  , newVolumePending_(false)
  , targetVolumePending_(0)
  , transitionDurationPending_(0)
  , easePending_(IAudioMixer::EaseLinear)
  , videoWindowTransitionDurationPending_(0)
  , videoWindowZOrderPending_(0)
  , controlMutex_(ASPCONTROL_MUTEX, "ASPControl")
  , stateMutex_(ASPSTATE_MUTEX, "ASPState")
  , nrdpVideoMutex_(NRDP_VIDEO_MUTEX, "NrdpVideo")
  , playbackDeviceMutex_(ASPPLAYBACK_MUTEX, "ASPPlayback")
  , audioTrackSwitchMutex_(ASPTRACKSWITCH_MUTEX, "ASPTrackSwitch")
  , mPipelineId(-1)
{
    // create playbackdevice
    playbackDevice_.reset(new MediaSourcePlaybackDevice(nrdApp()->getDeviceLib()->getESManager(),
                                                        nrdApp()->getDeviceLib()->getSystem()));

    // get bufferManager
    bufferManager_ = nrdApp()->getDeviceLib()->getBufferManager();

    // Playback profiles sanity check.
    ISystem::Capability systemCapability = nrdApp()->getSystem()->getCapability();
    checkProfiles(systemCapability.videoProfiles, systemCapability.audioProfiles);

    // Start the event thread.
    eventThread_.reset(new NrdpMediaEventThread(*this));
    eventThread_->start();
}

NrdpMediaSourcePlayer::~NrdpMediaSourcePlayer()
{
    if(eventThread_){
        eventThread_->stop();
        eventThread_->Wait();
    }
}

void NrdpMediaSourcePlayer::setMediaSource(shared_ptr<NrdpMediaSource> pMediaSource)
{
    mediaSource_ = pMediaSource;

    ScopedMutex playbackLock(playbackDeviceMutex_);
    if (playbackDevice_.get())
    {
        playbackDevice_->setMediaSource(pMediaSource);
    }
}

/*
 * ****************************
 * IMediaSourcePlayer interface
 * ****************************
 */
void NrdpMediaSourcePlayer::shutdown()
{
    if(eventThread_)
        eventThread_->stop();

    playbackDevice_.reset();
    bufferManager_.reset();
    mediaSource_.reset();
    playbackReporter_.reset();
}

/*
 * *************
 * Event listner
 * *************
 */
NFErr NrdpMediaSourcePlayer::addListener(shared_ptr<Listener> l)
{
   // forward to event thread
    if(eventThread_)
        eventThread_->addListener(l);
    return NFErr_OK;
}

NFErr NrdpMediaSourcePlayer::removeListener(shared_ptr<Listener> l)
{
    // forward to event thread
    if(eventThread_)
        eventThread_->removeListener(l);
    return NFErr_OK;
}

/*
 * *************
 * Informational
 * *************
 */
void NrdpMediaSourcePlayer::getBufferRange(Ticks& startPts, Ticks& endPts)
{
    if (!mediaSource_.get())
        return;

    AseTimeStamp startTimeStamp, endTimeStamp;
    mediaSource_->querySourceBufferTimeRange(startTimeStamp, endTimeStamp);
    startPts = Ticks(startTimeStamp.getValueInMs(), Ticks::RES_1KHZ);
    endPts = Ticks(endTimeStamp.getValueInMs(), Ticks::RES_1KHZ);
    return;
}

void NrdpMediaSourcePlayer::getMediaBufferPoolSize(uint32_t& videoBufferPoolSize,
                                                  uint32_t& audioBufferPoolSize)
{
    if (!bufferManager_.get()){
        return;
    }

    videoBufferPoolSize = bufferManager_->getBufferPoolSize(device::IBufferManager::VIDEO);
    audioBufferPoolSize =  bufferManager_->getBufferPoolSize(device::IBufferManager::AUDIO);
    return;
}

void NrdpMediaSourcePlayer::getPlaybackQualityStats(device::IPlaybackDevice::PlaybackQualityStats& audioStats,
                                                    device::IPlaybackDevice::PlaybackQualityStats& videoStats)
{
    if (deviceOpened_)
    {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        if (playbackDevice_.get())
        {
            playbackDevice_->getPlaybackQualityStats(audioStats, videoStats);
        }
    }
}

// Variables exposed to XB1VideoESPlayer for SmartGlass support.
#if defined(_XBOX_ONE)
    extern std::string curTitleContentId_;
#endif
/*
 * ****************
 * Playback control
 * ****************
 */
NFErr NrdpMediaSourcePlayer::open(
    const device::IPlaybackDevice::CommonVideoAttributesFromManifest &videoAttributes,
    const device::IPlaybackDevice::CommonAudioAttributesFromManifest &audioAttributes,
    shared_ptr<IMediaSourcePlaybackReporter> playbackReporter,
    ullong sessionId,
    uint32_t videoPipelineId,
    const std::string movieId)
{
    ScopedMutex scopedMutexControl(controlMutex_);

#if defined(_XBOX_ONE)
    // Expose contentID to XB1VideoESPlayer for SmartGlass support.
    curTitleContentId_ = movieId;
#endif
    NRDP_UNUSED(movieId);

    NTRACE(TRACE_MEDIACONTROL,
           "NrdpMediaSourcePlayer::open %s videoAttributes codecType(%u),3DType(%u),res(%u:%u),pasp(%u:%u),framerate(%u:%u)",
           movieId.c_str(),
           videoAttributes.codecType_,
           videoAttributes.video3DType_,
           videoAttributes.maxVideoWidth_,
           videoAttributes.maxVideoHeight_,
           videoAttributes.pixelAspectRatioX_,
           videoAttributes.pixelAspectRatioY_,
           videoAttributes.videoFrameRateValue_,
           videoAttributes.videoFrameRateScale_);

    NTRACE(TRACE_MEDIACONTROL
           ,"NrdpMediaSourcePlayer::open %s audioAttribues codecType(%u),channels(%u),samplerate(%u)"
           ,movieId.c_str()
           ,audioAttributes.codecType_
           ,audioAttributes.mNumberOfChannels_
           ,audioAttributes.mSamplesPerSecond_);

    // open time state initialization
    NFErr err;
    mPipelineId = videoPipelineId;
    audioSwitchingInProgress_ = false;
    playbackReporter_  = playbackReporter;
    deviceErrorReported = NFErr_OK;

    {
        ScopedMutex scopedMutexState(stateMutex_);
        if (state_ != IAdaptiveStreamingPlayer::CLOSED)
        {
            return NFErr_MC_InvalidStateTransition_ShouldBeClosed;
        }
        state_ = IAdaptiveStreamingPlayer::OPENING;
        eventThread_->setSessionId(sessionId);
        eventThread_->postStateChangedEvent(IAdaptiveStreamingPlayer::OPENING);
    }

    nrdApp()->getSystem()->notifyApplicationState(ISystem::MEDIA_LOADING);

    if(!abortOpen_){
        // open the device
        ScopedMutex playbackLock(playbackDeviceMutex_);
        NTRACE(TRACE_MEDIASTARTUP, "open(#1): before opening playback device");
        PERFTRACE("load(#5): before opening playback device, time=%llu\n", Time::mono().val());
        err = playbackDevice_->open(videoAttributes, audioAttributes, videoPipelineId);
        NTRACE(TRACE_MEDIASTARTUP, "open(#2): playback device opened");
        PERFTRACE("load(#6): playback device opened, time=%llu\n", Time::mono().val());
    } else {
        return NFErr_Aborted;
    }

    if (err.ok())
    {
        // when device opening is successful
        deviceOpened_ = true;
        playbackReporter_->reportKeyStartplayEvent("deviceopened");

        // Enable processing of device events as soon as device is opened. This
        // should be called before any setSpeed is called because, error such as
        // decryption error can happen as soon as device thread runs
        eventThread_->enableDeviceEvents();

        {
            ScopedMutex scopedMutexState(stateMutex_);
            state_ = IAdaptiveStreamingPlayer::STOPPED;
            eventThread_->postStateChangedEvent(IAdaptiveStreamingPlayer::STOPPED);
            eventThread_->postOpenCompleteEvent();
        }

        // set pending audio volume
        if (newVolumePending_)
        {
            ScopedMutex playbackLock(playbackDeviceMutex_);
            NTRACE(TRACE_MEDIACONTROL, "setting pending volume");
            playbackDevice_->setVolume(targetVolumePending_, transitionDurationPending_, easePending_);
            newVolumePending_ = false;
        }
        // set pending video window to requested or full if no pending request
        Rect actualRect;
        if (newVideoWindowPending_)
        {
            ScopedMutex playbackLock(playbackDeviceMutex_);
            actualRect = playbackDevice_->setVideoWindow(
                videoWindowTargetRectPending_,
                videoWindowTransitionDurationPending_,
                videoWindowZOrderPending_);

            newVideoWindowPending_ = false;
        }
        else
        {
            ScopedMutex playbackLock(playbackDeviceMutex_);
            // Set the video window to full screen upon open.
            Rect rect = {0,0,0,0};
            actualRect = playbackDevice_->setVideoWindow(rect, 0, 0);
        }
        eventThread_->postVideoWindowChangedEvent(actualRect);

        // return ok
        return err;
    }

    // device open failure
    err.push(NFErr_MC_OpenDeviceFailure);
    Log::error(TRACE_MEDIACONTROL, "Failed to open playback device.");
    reportError(err);
    return err;
}

NFErr NrdpMediaSourcePlayer::play(const Ticks& pts, bool playAndPause)
{
    NRDP_UNUSED(pts);
    abortSetSpeed_ = false;
    {
        ScopedMutex stateLock(controlMutex_);
        NTRACE(TRACE_MEDIASTARTUP
               , "[%d] NrdpMediaSourcePlayer::%s: pts=%llu, playAndPause=%d"
               , mPipelineId, __func__, pts.value(Ticks::RES_1KHZ), playAndPause?1:0);

        NFErr err;
        if(!deviceErrorReported.ok()){
            NTRACE(TRACE_MEDIASTARTUP,
                   "[%d] NrdpMediaSourcePlayer::%s return with deviceErrorReported", mPipelineId, __func__);
            eventThread_->disableDeviceEvents(); // disable event coming from device
            return deviceErrorReported;
        }

        PERFTRACE("NrdpMedia play(): time=%llu\n", Time::mono().val());
        {
            ScopedMutex stateLock(stateMutex_);
            if(state_ == IAdaptiveStreamingPlayer::PLAYING){
                return NFErr_OK;
            }

            if ( (state_ == IAdaptiveStreamingPlayer::OPENING) ||
                 (state_ == IAdaptiveStreamingPlayer::CLOSED) ) {
                return NFErr_MC_InvalidStateTransition_ShouldBeStopped;
            }
        }

        // Enable processing of device events as soon as device is opened. This
        // should be called before any setSpeed is called because, error such as
        // decryption error can happen as soon as device thread runs
        eventThread_->enableDeviceEvents();

        if(playAndPause){
            err = setSpeed(IPlaybackDevice::PAUSED_SPEED);
            if( err != NFErr_OK) {
                // if speed change is not ready now, return pending. Old PlayerBridge
                // was not handling this error, and was dependent on PumpingThread's
                // retrying. New bridge for player should handle this error correctly.
                eventThread_->disableDeviceEvents();
                return err;
            }
            ScopedMutex stateLock(stateMutex_);
            state_ = IAdaptiveStreamingPlayer::PAUSED;
            eventThread_->postStateChangedEvent(IAdaptiveStreamingPlayer::PAUSED);
        } else {
            err = setSpeed(IPlaybackDevice::NORMAL_SPEED);
            if( err != NFErr_OK) {
                // if speed change is not ready now, return pending. Old PlayerBridge
                // was not handling this error, and was dependent on PumpingThread's
                // retrying. New bridge for player should handle this error correctly.
                eventThread_->disableDeviceEvents();
                return err;
            }
            ScopedMutex stateLock(stateMutex_);
            state_ = IAdaptiveStreamingPlayer::PLAYING;
            eventThread_->postStateChangedEvent(IAdaptiveStreamingPlayer::PLAYING);
            dataExhausted_ = false;
        }

        nrdApp()->getSystem()->notifyApplicationState(ISystem::BUFFERING);
    }
    return NFErr_OK;
}

NFErr NrdpMediaSourcePlayer::skip(const Ticks& pts)
{
    abortSetSpeed_ = true;
    {
        ScopedMutex scopedMutexControl(controlMutex_);
        NTRACE(TRACE_MEDIACONTROL, "[%d] NrdpMediaSourcePlayer::%s: pts=%llu"
               , mPipelineId, __func__, pts.value(Ticks::RES_1KHZ));
        {
            ScopedMutex scopedMutexState(stateMutex_);
            if (state_ != IAdaptiveStreamingPlayer::PAUSED)
                return NFErr_NotAllowed;
        }

        Ticks startPts, endPts;
        getBufferRange(startPts, endPts);
        if ( !pts.valid() || (pts < startPts) || (pts > endPts) || (startPts == endPts) )
            return NFErr_NotAllowed;

        eventThread_->postSkippedEvent();

        // Disable processing of device events
        eventThread_->disableDeviceEvents();

        // the pts is valid and fall in the range
        ullong value = pts.value(Ticks::RES_1KHZ);
        AseTimeStamp seekTime = AseTimeStamp(value, 1000);

        // move the mediaSource to new seekTime. When skip the mediaSource_,
        // disableSampleFeeding by DPI so that no new data after skip is flushed
        // by playbackDevice_->flush()
        if(mediaSource_){
            mediaSource_->disableSampleFeeding();
            mediaSource_->skip(seekTime);
        }
        if (playbackDevice_.get())
        {
            ScopedMutex playbackLock(playbackDeviceMutex_);
            playbackDevice_->flush();
        }
        if(mediaSource_){
            mediaSource_->enableSampleFeeding();
        }

        playbackStarted_ = false;
        playbackReporter_->playbackStopped(NFErr_Skip, AseTimeStamp(currentPts_, 1000));
        playbackReporter_->skipStarted();

        // Enable processing of device events after data pumping is started.
        eventThread_->enableDeviceEvents();
    }
    return NFErr_OK;
}

NFErr NrdpMediaSourcePlayer::stop()
{
    abortSetSpeed_ = true; // this should be outside of mutex
    {
        ScopedMutex scopedMutexControl(controlMutex_);
        NTRACE(TRACE_MEDIASTARTUP
               , "[%d] NrdpMediaSourcePlayer::%s time=%llu pts %u"
               , mPipelineId, __func__, Time::mono().val(), currentPts_);
        {
            ScopedMutex scopedMutexState(stateMutex_);

            // allow stopping while STOPPED since we may need to flush the mediaSource
            // again, if JS started buffering a new position
            if (state_ != IAdaptiveStreamingPlayer::PLAYING &&
                state_ != IAdaptiveStreamingPlayer::STOPPED &&
                state_ != IAdaptiveStreamingPlayer::PAUSED)
            {
                Log::warn(TRACE_MEDIACONTROL,
                          "[%d] NrdpMedia::stop InvalidStateTransition: state_ = %d",
                          mPipelineId, state_);
                return NFErr_MC_InvalidStateTransition_ShouldBePlayingOrPaused;
            }

            state_ = IAdaptiveStreamingPlayer::STOPPED;
        }


        // NrdJs will guarantee that no-in-flight mediaSource buffer will be
        // appened after flush.  And then, call playback device's flush need to
        // called to flush out data in the device.  When skip the mediaSource_,
        // disableSampleFeeding by DPI so that no new data after skip is flushed
        // by playbackDevice_->flush()
        NTRACE(TRACE_MEDIACONTROL, "[%d] NrdpMedia::flush source buffer", mPipelineId);
        if(mediaSource_){
            mediaSource_->disableSampleFeeding();
            mediaSource_->flushSourceBuffers();
        }
        if (playbackDevice_.get())
        {
            ScopedMutex playbackLock(playbackDeviceMutex_);
            playbackDevice_->flush();
        }
        if(mediaSource_){
            mediaSource_->enableSampleFeeding();
        }

        playbackStarted_ = false;
        nrdApp()->getSystem()->notifyApplicationState(ISystem::STOPPING);

        // Disable processing of device events
        eventThread_->disableDeviceEvents();
        eventThread_->postStateChangedEvent(IAdaptiveStreamingPlayer::STOPPED);

        NTRACE(TRACE_MEDIASTARTUP, "[%d] stopped NrdpMediaSourcePlayer: time=%llu", mPipelineId, Time::mono().val());
    }
    return NFErr_OK;
}

NFErr NrdpMediaSourcePlayer::pause()
{
    abortSetSpeed_ = false;
    {
        ScopedMutex scopedMutexControl(controlMutex_);
        NTRACE(TRACE_MEDIASTARTUP
               , "[%d] NrdpMediaSourcePlayer::%s time=%llu pts %u"
               , mPipelineId, __func__, Time::mono().val(), currentPts_);

        if(!deviceErrorReported.ok()){
            NTRACE(TRACE_MEDIASTARTUP,
                   "[%d] NrdpMediaSourcePlayer::%s return with deviceErrorReported", mPipelineId, __func__);
            eventThread_->disableDeviceEvents(); // disable event coming from device
            return deviceErrorReported;
        }

        {
            ScopedMutex stateLock(stateMutex_);
            if (state_ == IAdaptiveStreamingPlayer::PAUSED)
                return NFErr_OK;

            if (state_ != IAdaptiveStreamingPlayer::PLAYING){
                Log::warn(TRACE_MEDIACONTROL, "[%d] NrdpMediaSourcePlayer::pause InvalidStateTransition: state_ = %d", mPipelineId, state_);
                return NFErr_MC_InvalidStateTransition_ShouldBePlaying;
            }
        }

        NFErr err = setSpeed(IPlaybackDevice::PAUSED_SPEED);
        if (err != NFErr_OK){
            // underlying playback device currently does not return for PAUSE request.
            // TODO: study if other operation is needed in this case.
            return err;
        }

        // notify playback reporter
        playbackReporter_->playbackStateChange(AseTimeStamp(currentPts_, 1000),
                                               IMediaPipelineReporter::PAUSED_STATE);

        state_ = IAdaptiveStreamingPlayer::PAUSED;
        eventThread_->postStateChangedEvent(IAdaptiveStreamingPlayer::PAUSED);

        nrdApp()->getSystem()->notifyApplicationState(ISystem::PAUSED);
        return err;
    }
}

NFErr NrdpMediaSourcePlayer::unpause()
{
    abortSetSpeed_ = false;
    {
        ScopedMutex scopedMutexControl(controlMutex_);
        NTRACE(TRACE_MEDIASTARTUP
               , "[%d] NrdpMediaSourcePlayer::%s time=%llu pts %u"
               , mPipelineId, __func__, Time::mono().val(), currentPts_);
        {
            ScopedMutex stateLock(stateMutex_);
            if (state_ == IAdaptiveStreamingPlayer::PLAYING)
                return NFErr_OK;

            if (state_ != IAdaptiveStreamingPlayer::PAUSED) {
                Log::warn(TRACE_MEDIACONTROL, "[%d] NrdpMediaSourcePlayer::unpause InvalidStateTransition: state_ = %d", mPipelineId, state_);
                return NFErr_MC_InvalidStateTransition_ShouldBePaused;
            }
        }

        NFErr err = setSpeed(IPlaybackDevice::NORMAL_SPEED);
        if( err != NFErr_OK) {
            // if speed change is not ready now, return pending. Old PlayerBridge
            // was not handling this error, and was dependent on PumpingThread's
            // retrying. New bridge for player should handle this error correctly.
            return err;
        }

        state_ = IAdaptiveStreamingPlayer::PLAYING;
        eventThread_->postStateChangedEvent(IAdaptiveStreamingPlayer::PLAYING);
        dataExhausted_ = false;

        nrdApp()->getSystem()->notifyApplicationState(ISystem::PLAYING);

        // notify playback reporter
        playbackReporter_->playbackStateChange(AseTimeStamp(currentPts_, 1000),
                                               IMediaPipelineReporter::UNPAUSED_STATE);
    }
    return NFErr_OK;
}

void NrdpMediaSourcePlayer::close()
{
    abortSetSpeed_ = true;
    {
        ScopedMutex scopedMutexControl(controlMutex_);
        NTRACE(TRACE_MEDIASTARTUP
               ,"[%d] NrdpMediaSourcePlayer::close(): time=%llu pts %u"
               , mPipelineId, Time::mono().val(), currentPts_);

        IAdaptiveStreamingPlayer::State state;
        {
            ScopedMutex scopedMutexState(stateMutex_);
            state = state_;
        }

        if (state == IAdaptiveStreamingPlayer::PLAYING || state == IAdaptiveStreamingPlayer::PAUSED)
            stop();

        // this is aborting case when play is issued and device is not ready
        // yet to play, but user wants to close.
        if (state == IAdaptiveStreamingPlayer::STOPPED)
        {
            NTRACE(TRACE_MEDIACONTROL
                   , "[%d] NrdpMedia::%s in STOPPED state : closing when play is issued but not yet started"
                   , mPipelineId, __func__);
            if(mediaSource_){
                mediaSource_->disableSampleFeeding();
                mediaSource_->flushSourceBuffers();
            }
            if (playbackDevice_.get())
            {
                ScopedMutex playbackLock(playbackDeviceMutex_);
                playbackDevice_->flush();
            }
            if(mediaSource_){
                mediaSource_->enableSampleFeeding();
            }
            // Disable processing of device events
            eventThread_->disableDeviceEvents();
        }

        if (state == IAdaptiveStreamingPlayer::CLOSED)
        {
            NTRACE(TRACE_MEDIACONTROL, "[%d] NrdpMedia already closed", mPipelineId);
            // post event that we are closed
            eventThread_->postStateChangedEvent(IAdaptiveStreamingPlayer::CLOSED);
            return;
        }

        {
            ScopedMutex scopedMutexState(stateMutex_);
            assert(!abortOpen_);
            assert(state_ == IAdaptiveStreamingPlayer::OPENING || state_ == IAdaptiveStreamingPlayer::STOPPED);

            // If media loading is not finished yet, abortOpen any ongoing transaction or
            // data streaming.
            if (state_ == IAdaptiveStreamingPlayer::OPENING)
                abortOpen_ = true;

            state_ = IAdaptiveStreamingPlayer::CLOSED;
        }

        nrdApp()->getSystem()->notifyApplicationState(ISystem::MEDIA_UNLOADING);

        // We log playback quality stats to playback reporter here so that they can
        // be included in the endplay log.
        if (deviceOpened_)
        {
            ScopedMutex playbackLock(playbackDeviceMutex_);
            if (playbackDevice_.get())
            {
                // report quality stat before closing
                IPlaybackDevice::PlaybackQualityStats audioStats, videoStats;
                playbackDevice_->getPlaybackQualityStats(audioStats, videoStats);
                playbackReporter_->setPlaybackQualityStats(audioStats, videoStats);
                // close playback device
                playbackDevice_->close();
                deviceOpened_ = false;
            }
        }

        eventThread_->postStateChangedEvent(IAdaptiveStreamingPlayer::CLOSED);

        nrdApp()->getSystem()->notifyApplicationState(ISystem::BROWSING);
        NTRACE(TRACE_MEDIASTARTUP, "[%d] closed NrdpMediaSourcePlayer: time=%llu", mPipelineId, Time::mono().val());

        // Reset everything. It means, after this point, open can be issued bring
        // state back to clean stopped state.
        abortOpen_ = false;
        playbackStarted_ = false;
        dataExhausted_ = false;
        endOfPresentation_ = false;
        mPipelineId = -1;
        newVideoWindowPending_ = false;
        newVolumePending_ = false;
        videoWindowZOrderPending_ = 0;
        videoWindowTransitionDurationPending_ = 0;
        currentPts_ = INVALID_PTS;
        latestReportedPTS_ = INVALID_PTS;
        abortSetSpeed_ = false;
        audioSwitchingInProgress_ = false;
        eventThread_->setSessionId(0);
        deviceErrorReported = NFErr_OK;
    }
}

void NrdpMediaSourcePlayer::disableAudio()
{
    ScopedMutex scopedMutexSwitch(audioTrackSwitchMutex_);

    if(audioSwitchingInProgress_){
        NTRACE(TRACE_MEDIACONTROL,
               "[%d] NrdpMediaSourcePlayer::disableAudio rejected: Previous on-the-fly audio track switch is in progress",
               mPipelineId);
        eventThread_->postAudioTrackChangeRejectedEvent();
        return;
    } else {
        audioSwitchingInProgress_ = true;
    }

    // Flush mediaSourceBuffer and device buffer. Always flush mediaSourceBuffer
    // first in order not for device to get old data
    if(mediaSource_){
        mediaSource_->flushSourceBuffer(MEDIA_AUDIO);
    }
    if(playbackDevice_.get()){
        ScopedMutex playbackLock(playbackDeviceMutex_);
        playbackDevice_->flushAudio();
        NTRACE(TRACE_MEDIACONTROL,
               "[%d] NrdpMediaSourcePlayer::disableAudio On-the-fly audio track switch: flush audio in device completed time=%llu ms"
               , mPipelineId, Time::mono().val());
    }
    eventThread_->postAudioTrackChangeStartedEvent();
}

void NrdpMediaSourcePlayer::enableAudio()
{
    ScopedMutex scopedMutexSwitch(audioTrackSwitchMutex_);
    audioSwitchingInProgress_ = false;
    NTRACE(TRACE_MEDIACONTROL
           ,"[%d] NrdpMediaSourcePlayer::enableAudio On-the-fly audio track switch completed: time=%llu ms"
           , mPipelineId, Time::mono().val());
    eventThread_->postAudioTrackChangeCompleteEvent();
}

Ticks NrdpMediaSourcePlayer::getCurrentPts() const
{
    uint32_t currentPts = currentPts_;

    if (currentPts == INVALID_PTS)
        return Ticks();
    else
        return Ticks(currentPts, Ticks::RES_1KHZ);
}

IAdaptiveStreamingPlayer::State NrdpMediaSourcePlayer::getState() const
{
    return state_;
}

/*
 * ********************
 * Presentation control
 * ********************
 */
void NrdpMediaSourcePlayer::setVideoWindow(Rect targetRect, uint32_t transitionDuration, uint32_t zOrder)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    NTRACE(TRACE_MEDIACONTROL
           , "NrdpMediaSourcePlayer::%s rec %d/%d/%d/%d, duration %d, zorder %d"
           , __func__, targetRect.x, targetRect.y, targetRect.width, targetRect.height, transitionDuration, zOrder);

    ScopedMutex scopedMutexState(stateMutex_);
    if (!deviceOpened_ )
    {
        videoWindowTargetRectPending_ = targetRect;
        videoWindowTransitionDurationPending_ = transitionDuration;
        videoWindowZOrderPending_ = zOrder;
        newVideoWindowPending_ = true;
    }
    else
    {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        Rect actualRect = playbackDevice_->setVideoWindow(targetRect, transitionDuration, zOrder);
        eventThread_->postVideoWindowChangedEvent(actualRect); //TODO : should I send actual zOrder also?
    }
}

double NrdpMediaSourcePlayer::getVolume()
{
    ScopedMutex scopedMutexControl(controlMutex_);
    // if device is not open, assume that volume is 100% percent
    double volume = 1.0;
    if (!deviceOpened_) {
        if (newVolumePending_) {
            volume = targetVolumePending_;
        }
        return volume;
    }

    if(playbackDevice_.get()){
        volume = playbackDevice_->getVolume();
    }
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourcePlayer::getVolume: %f", volume);
    return volume;
}

void NrdpMediaSourcePlayer::setVolume(double targetVolume,
               uint32_t transitionDuration,
               device::IAudioMixer::Ease ease)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    NTRACE(TRACE_MEDIACONTROL, "NrdpMediaSourcePlayer::setVolume targetVolume %f transitionDuration %d",
           targetVolume, transitionDuration);
    ScopedMutex scopedMutexState(stateMutex_);

    if ( targetVolume > 1.0 ) {
        targetVolume = 1.0;
    }

    if ( targetVolume < 0.0 ) {
        targetVolume = 0.0;
    }

    if (!deviceOpened_) {
        targetVolumePending_ = targetVolume;
        transitionDurationPending_ = transitionDuration;
        easePending_ = ease;
        newVolumePending_ = true;
    } else {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        playbackDevice_->setVolume(targetVolume, transitionDuration, ease);
    }
}
void NrdpMediaSourcePlayer::reportError(NFErr err)
{
    // this is called from NrdpMediaEventThread

    NTRACE(TRACE_MEDIACONTROL, "[%d] reportError %s", mPipelineId, err.toString().c_str());

    // report playback failure
    if (playbackReporter_.get())
        playbackReporter_->playbackFailure(err);

    eventThread_->postErrorEvent(err); // post error notification to the user (to nrdjs).

    deviceErrorReported = err;

 }

/*
 * ***************
 * Internal method
 * ***************
 */
void NrdpMediaSourcePlayer::updatePTS(ullong pts, ullong monotimeInMS)
{
    currentPts_ = static_cast<uint32_t>(pts);

    NTRACE(TRACE_MEDIAPLAYBACK, "NrdpMediaSourcePlayer::%s %d", __func__,  currentPts_);

    if(!mediaSource_){
        Log::error(TRACE_MEDIACONTROL, "[%d] NrdpMediaSourcePlayer::updatePTS mediaSource NULL", mPipelineId);
        return;
    }
    mediaSource_->updatePts(currentPts_);

    // If the current presentation time position makes sense (it should be greater
    // than the last recorded PTS used in playdata generation), report the elapsed
    // playtime (from the last recorded PTS to the current presentation time
    // position) to the playdata persistor, as well as adding it to the total
    // play-time count.
    if (currentPts_ > latestReportedPTS_)
    {
        // update playback reporter
        uint32_t vcdnid = 0;
        uint32_t acdnid = 0;
        playbackReporter_->updatePlaybackPts(AseTimeStamp(currentPts_, 1000),
                                             vcdnid, acdnid);

        // Call the listener functions to notify of the current presentation position.
        if(latestReportedPTS_ == 0)
        {
            PERFTRACE("MCQ2 - ASP:: posting first PTS to listeners time=%llu\n",
                      Time::mono().val());
        }
    }

    // notify the MC of the latest PTS
    // (do not guard against latestReportedPTS as rewind will be behind that)
    eventThread_->postUpdatePTSEvent(currentPts_, monotimeInMS);
    latestReportedPTS_ = currentPts_;

    if (!playbackStarted_)
    {
        playbackStarted_ = true;
        // notify PlaybackReporter
        playbackReporter_->playbackStarted("");
        PERFTRACE("playback started: time=%llu\n", Time::mono().val());
    }
}

NFErr NrdpMediaSourcePlayer::setSpeed(int32_t speed)
{
    NTRACE(TRACE_MEDIASTARTUP, "NrdpMediaSourcePlayer::%s %d", __func__, speed);
    NFErr err;
    if( speed == IPlaybackDevice::PAUSED_SPEED ) {
        err = playbackDevice_->setSpeed(IPlaybackDevice::PAUSED_SPEED);
        if( err != NFErr_OK) {
            return err;
        }
    } else if (speed == IPlaybackDevice::NORMAL_SPEED ) {
        uint64_t commandArrivalTime = Time::mono().val();
        while(!abortSetSpeed_){
            if(!deviceErrorReported.ok()){
                err = deviceErrorReported;
                break;
            }
            err = playbackDevice_->setSpeed(IPlaybackDevice::NORMAL_SPEED);
            if(err == NFErr_OK){
                return err;
            }
            uint64_t currentTime = Time::mono().val(); // ms
            if( currentTime - commandArrivalTime > MAX_DEVICE_NOT_READY_ERROR_WAIT_TIME ){
                err = NFErr_MC_DevicePlaybackSetSpeedTimeout;
                err.push(NFErr_MC_DevicePlaybackError);
                reportError(err);
                break;
            }
            Thread::Sleep(SLEEP_WAITING_READY_FOR_PLAY);
        }
    }
    abortSetSpeed_ = false;
    return err;
}
