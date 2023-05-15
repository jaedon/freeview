/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "AdaptiveStreamingPlayer.h"

#include <cstring>
#include <locale>
#include <sstream>
#include <algorithm>
#include <unistd.h>

#include <nrd/config.h>
#include <nrd/IDeviceLib.h>
#include <nrd/ISystem.h>

#include <nrdbase/Assert.h>
#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/LexicalCast.h>
#include <nrdbase/Version.h>
#include <nrdbase/ASError.h>

#include <nrdase/PrintManifest.h>
#include <nrdase/StreamingManager.h>

#include <nrdnet/TrafficShaper.h>
#include <nrd/DrmManager.h>

#include "EventThread.h"
#include "ManifestUtilities.h"
#include "McMediaBufferPool.h"
#include "McMediaSink.h"
#include "MediaFileCache.h"
#include "NrdApplication.h"
#include "NrdConfiguration.h"
#include "PlayerState.h"
#include "PumpingThread.h"
#include "StreamingConfigParameter.h"
#include "LocalConfigParameter.h"
#include "StreamingPersistentStore.h"
#include "SystemData.h"

#ifdef NRDP_HAS_ESPLAYER
# include "PlaybackDevice.h"
#endif

using namespace std;
using namespace netflix;
using namespace netflix::ase;
using namespace netflix::device;
using namespace netflix::mediacontrol;
using namespace netflix::util;

#define BUFFER_POOL_UNIT_SIZE (4096)

namespace {

void checkProfileDuplication(vector<ContentProfile>& profiles)
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

#if 0
void checkVideoCodecConsistency(VideoCodecType& videoCodecType, VideoCodecType myType)
{
    assert(myType != UNKNOWN_VIDEO_CODEC);

    if (videoCodecType == UNKNOWN_VIDEO_CODEC)
        videoCodecType = myType;

    assert(myType == videoCodecType);
}
#endif

void checkContainerConsistency(ContainerType& containerType, ContainerType myType)
{
    assert(myType != UNKNOWN_CONTAINER);

    if (containerType == UNKNOWN_CONTAINER)
        containerType = myType;

    assert(myType == containerType);
}

void checkDrmConsistency(DrmType& drmType, DrmType myType)
{
    if (myType != NO_DRM)
    {
        if (drmType == NO_DRM)
            drmType = myType;

        assert(myType == drmType);
    }
}

void checkProfiles(vector<ContentProfile>& videoProfiles,
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
        //checkVideoCodecConsistency(videoCodecType, profileVideoCodecType(*iter));
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

uint32_t convertManifestTrack(shared_ptr<Manifest> oldManifest,
                              shared_ptr<Manifest> newManifest,
                              uint32_t oldTrackIndex)
{
    const vector<ManifestTrackPtr> oldTracks = oldManifest->getTracks();
    string trackId = oldTracks[oldTrackIndex]->getTrackKey();
    MediaType mediaType = oldTracks[oldTrackIndex]->getMediaType();

    const vector<ManifestTrackPtr> newTracks = newManifest->getTracks();
    uint32_t newTrackIndex = 0;
    for (uint32_t i=0; i<newTracks.size(); ++i)
    {
        if (newTracks[i]->getMediaType() != mediaType)
            continue;   // don't bump newTrackIndex if the mediaType is wrong

        if (newTracks[i]->getTrackKey() == trackId)
            return newTrackIndex;
        ++newTrackIndex;
    }

    return -1;
}

} // namespace anonymous

AdaptiveStreamingPlayer::AdaptiveStreamingPlayer() :
    videoZOrderPending_(VIDEO_AT_BACK),
    state_(CLOSED),
    streamingType_(NORMAL_PLAY),
    playerState_(new PlayerState()),
    abort_(false),
    storeError_(false),
    endOfPresentation_(false),
    dataExhausted_(false),
    openComplete_(false),
    deviceOpened_(false),
    deviceInitialized_(false),
    newVideoWindowPending_(false),
    newVideoZOrderPending_(false),
    newVolumePending_(false),
    targetVolumePending_(0),
    transitionDurationPending_(0),
    easePending_(IAudioMixer::EaseLinear),
    playbackStarted_(false),
    havePts_(false),
    currentPts_(INVALID_PTS),
    playDuration_(INVALID_DURATION),
    gopSize_(0),
    reportedVideoBitrate_(0),
    latestReportedPTS_(INVALID_PTS),
    playbackSkipping_(false),
    suppressBufferingEvents_(false),
    predictedFutureRebuffers_(0),
    currentBandwidth_(0),
    streamingStat_(shared_ptr<StreamingStatReport>()),
    maxStreamingBufferLen_(0),
    minStreamingBufferLen_(0),
    powerSaving_(0),
    videoBufferPoolSize_(0),
    audioBufferPoolSize_(0),
    networkProfile_(WIRED),
    minimumDataInDevice_(1000),
    pendingBufferingComplete_(false),
    currentManifestIndex_(0),
    lastAudioManifestIndex_(-1),
    lastVideoManifestIndex_(-1),
    videoWindowTransitionDurationPending_(0),
    stateMutex_(ASPSTATE_MUTEX, "ASPState"),
    threadMutex_(ASPTHREAD_MUTEX, "ASPThread"),
    streamingManagerMutex_(ASPSTREAMING_MUTEX, "ASPStreaming"),
    streamingStatMutex_(ASPSTREAMINGSTAT_MUTEX, "ASPStreamingStat"),
    playbackDeviceMutex_(ASPPLAYBACK_MUTEX, "ASPPlayback"),
    audioTrackSwitchMutex_(ASPTRACKSWITCH_MUTEX, "ASPTrackSwitch"),
    metaDataMutex_(ASPMETADATA_MUTEX, "ASPMetaData"),
    manifestMutex_(ASPMANIFEST_MUTEX, "ASPManifest"),
    openCompleteMutex_(ASPOPENCOMPLETE_MUTEX, "ASPOpenComplete"),
    lastGetBlockOutcomeForLogging_(DeliveredSelectedBlock),
    numGetBlockCallsSinceLastLog_(0)
{
    videoBitrateRanges_ = NrdConfiguration::videoBitrateRanges();

    // Get the playback device and the device buffer manager interfaces.
#ifdef NRDP_HAS_ESPLAYER
    playbackDevice_.reset(new PlaybackDevice(nrdApp()->getDeviceLib()->getESManager(),
                                             nrdApp()->getDeviceLib()->getSystem()));
#else
    playbackDevice_ = nrdApp()->getDeviceLib()->getPlaybackDevice();
#endif
    bufferManager_ = nrdApp()->getDeviceLib()->getBufferManager();

    streamingConfigParameter_.reset(new StreamingConfigParameter);
    localConfigParameter_.reset(new LocalConfigParameter);
    persistentStore_.reset(new StreamingPersistentStore);

    ISystem::Capability systemCapability = nrdApp()->getSystem()->getCapability();

    // Playback profiles sanity check.
    checkProfiles(systemCapability.videoProfiles,
                  systemCapability.audioProfiles);

    // Start the event thread.
    eventThread_.reset(new EventThread(*this, playerState_));
    eventThread_->start();

    // create MediaFileCache
    mediaFileCache_.reset(new MediaFileCache());

    // Initialize the Streaming Manager
    (void) StreamingManager::configure( persistentStore_, streamingConfigParameter_ );

    // Set extra HTTP request headers.
    std::string xDeviceKey = "X-Device";
    std::string xDeviceValue = std::string(NRDP_VERSION_NUMBER) + std::string(" ");

    std::string esnPrefix = nrdApp()->esnPrefix();

    //Remove the ending '-' if there is one.
    if (!esnPrefix.empty() && (esnPrefix[esnPrefix.length() - 1] == '-'))
        esnPrefix.erase( esnPrefix.length() - 1 );

    xDeviceValue += esnPrefix;
    StreamingManager::addExtraHttpHeader( xDeviceKey, xDeviceValue );
}

AdaptiveStreamingPlayer::~AdaptiveStreamingPlayer()
{
    // stop and join the threads
    (void) StreamingManager::deinitialize();

    eventThread_->stop();
    eventThread_->Wait();
}

void AdaptiveStreamingPlayer::shutdown()
{
    (void) StreamingManager::deinitialize();

    eventThread_->stop();
}

//================================================================================
// IAdaptiveStreamingPlayer interface implementation.
//================================================================================

NFErr AdaptiveStreamingPlayer::open(shared_ptr<ase::Manifest> manifest,
                                    shared_ptr<IPlaybackReporter> playbackReporter,
                                    shared_ptr<LicenseHandler> licenseHandler,
                                    ullong sessionId, netflix::Video3DType video3DType,
                                    const std::string &defaultAudioTrack,
                                    StreamingType streamingType,
                                    ContainerType containerType,
                                    const IPlaybackDevice::CommonVideoAttributesFromManifest &videoAttributes,
                                    const IPlaybackDevice::CommonAudioAttributesFromManifest &audioAttributes,
                                    Ticks const& pts)
{
    // These two parameters if for the NrdpVideo
    NRDP_UNUSED(videoAttributes);
    NRDP_UNUSED(audioAttributes);

    NTRACE(TRACE_MEDIACONTROL,
           "AdaptiveStreamingPlayer::open( %s, videoType = %d, streamingType = %u )",
           manifest->getContentId().c_str(), video3DType, streamingType);

    {
        ScopedMutex scopedMutexState(stateMutex_);

        if (state_ != CLOSED)
        {
            Log::warn(TRACE_MEDIACONTROL, "ASPlayer::open InvalidStateTransition: "
                       "state_ = %d, playerState error %s", state_,
                       playerState_->m_error ?  "true" : "false");
            return NFErr_MC_InvalidStateTransition_ShouldBeClosed;
        }

        state_ = OPENING;

        eventThread_->setSessionId(sessionId);
        eventThread_->postStateChangedEvent(OPENING);
    }

    nrdApp()->getSystem()->notifyApplicationState(ISystem::MEDIA_LOADING);

    // Some variables must be properly initialized or reset before we start the
    // data pumping thread.

    playerState_->resetOnOpen();

    streamingType_ = streamingType;
    containerType_ = containerType;
    havePts_ = pts.valid();
    currentPts_ = havePts_ ? pts.value(Ticks::RES_1KHZ) : 0;
    licenseRetriever_  = licenseHandler;
    latestReportedPTS_ = 0;
    playbackSkipping_ = false;
    suppressBufferingEvents_ = false;
    video3DType_ = video3DType;
    playbackReporter_  = playbackReporter;

#ifdef BUILD_DEBUG
    // print the manifest
    std::ostringstream os;
    os << *manifest;
    NTRACE(TRACE_MEDIACONTROL, "open Manifest: %s", os.str().c_str());
#endif

    // process the manifest
    shared_ptr<ManifestMetaData> mmd;
    NFErr err = processManifest(/*out*/mmd, manifest, video3DType, defaultAudioTrack);
    if (!err.ok())
    {
        // report the error
        err.push(NFErr_MC_BadManifest);
        reportError(err);
        return err;
    }

    {
        ScopedMutex manifestLock(manifestMutex_);

        // add initial manifest to our list of manifests
        manifests_.clear();
        manifests_.push_back(manifest);
        manifestData_.clear();
        manifestData_.push_back(mmd);
    }

    // update manifest's index
    currentManifestIndex_ = 0;
    manifest->setManifestIndex(currentManifestIndex_);
    manifest->setDuration(AseTimeInterval(120 * 1000, 1000));

    // setup streaming config
    setupStreamingConfigParams();

    {
        ScopedMutex threadLock(threadMutex_);

        // Start the data pumping thread.
        pumpingThread_.reset(
                new PumpingThread(*this, playerState_, playbackReporter_, (streamingType_ != NORMAL_PLAY)));
        pumpingThread_->start();
    }

    return NFErr_OK;
}

void AdaptiveStreamingPlayer::completeBuffering(uint32_t actualStartPts)
{
    NRDP_UNUSED(actualStartPts);
}

NFErr AdaptiveStreamingPlayer::feedDrmHeader(std::vector<unsigned char> const& drmHeader)
{
    NRDP_UNUSED(drmHeader);
    return NFErr_OK;
}


NFErr AdaptiveStreamingPlayer::addManifest(shared_ptr<Manifest> manifest)
{
    NTRACE(TRACE_MEDIACONTROL, "AdaptiveStreamingPlayer::addManifest %s",
           manifest->getContentId().c_str());

    {
        ScopedMutex scopedMutexState(stateMutex_);

        // ignore if we're not in right state
        if (state_ == CLOSED)
        {
            NTRACE(TRACE_MEDIACONTROL, "Ignoring addManifest, player closed");
            return NFErr_MC_InvalidStateTransition_ShouldBePlayingOrPaused;
        }
    }

    ScopedMutex lock(manifestMutex_);

    uint32_t manifestIndex = manifests_.size();

    // create a manifest meta data for this manifest
    shared_ptr<ManifestMetaData> mmd;
    NFErr err = processManifest(/*out*/mmd, manifest, video3DType_, string());
    if (!err.ok())
    {
        NTRACE(TRACE_MEDIACONTROL, "processManifest returned %s", err.toString().c_str());
        return err;
    }

    // hand off new manifest to streaming manager
    manifests_.push_back(manifest);
    manifestData_.push_back(mmd);

    manifest->setManifestIndex(manifestIndex);
    streamingManager_->addManifest(manifest);
    mediaSink_->addManifest(manifest);

    // calculate the global stream index offsets
    calculateStreamIndices( manifestIndex );

    // build the stream infos for the new manifest
    // Note we can leave the stream header NULL since that is only necessary for
    // first manifest
    vector<IPlaybackDevice::StreamInfo> streamInfos;
    buildStreamInfos(/*out*/streamInfos, manifestIndex,
                     -1, -1, shared_ptr<vector<unsigned char> >());

    // give streamInfos to playback device
    {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        err = playbackDevice_->setStreamInfo(streamInfos, manifestIndex);
        if (!err.ok())
        {
            NTRACE(TRACE_MEDIACONTROL, "setStreamInfo returned %s",
                   err.toString().c_str());
        }
        return err;
    }
}

void AdaptiveStreamingPlayer::abort()
{
    // no need to do anything here; this method is for external players.
}

void AdaptiveStreamingPlayer::close()
{
    NTRACE(TRACE_MEDIASTARTUP, "AdaptiveStreamingPlayer::close(): time=%llu pts %u",
           Time::mono().val(), currentPts_);

    State state;
    {
        ScopedMutex scopedMutexState(stateMutex_);
        state = state_;
    }

    if (state == PLAYING || state == PAUSED)
        stop();

    if (state == CLOSED)
    {
        NTRACE(TRACE_MEDIACONTROL, "AdaptiveStreamingPlayer already closed");

        // post event that we are closed
        eventThread_->postStateChangedEvent(CLOSED);

        return;
    }

    {
        ScopedMutex scopedMutexState(stateMutex_);
        assert(!abort_);
        assert(state_ == OPENING || state_ == STOPPED);

        // If media loading is not finished yet, abort any ongoing transaction or
        // data streaming.

        if (state_ == OPENING)
            abort_ = true;

        state_ = CLOSED;
    }

    // We log playback quality stats to playback reporter here so that they can
    // be included in the endplay log.
    if (deviceOpened_)
    {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        if (playbackDevice_.get())
        {
            IPlaybackDevice::PlaybackQualityStats audioStats, videoStats;
            playbackDevice_->getPlaybackQualityStats(audioStats, videoStats);
            playbackReporter_->setPlaybackQualityStats(audioStats, videoStats);
        }
    }

    nrdApp()->getSystem()->notifyApplicationState(ISystem::MEDIA_UNLOADING);

    {
        ScopedMutex lock(threadMutex_);
        if (pumpingThread_.get())
        {
            // Terminate the pumping thread. Media will also be unloaded along with it.
            NTRACE(TRACE_MEDIASTARTUP, "before stopping pumpingThread_: time=%llu",
                   Time::mono().val());
            pumpingThread_->stop();
            pumpingThread_->Wait();
            pumpingThread_.reset();
            NTRACE(TRACE_MEDIASTARTUP, "stopped pumpingThread_: time=%llu",
                   Time::mono().val());
        }
    }

    openComplete_ = false;
    playbackStarted_ = false;
    licenseRetriever_.reset();

    eventThread_->postStateChangedEvent(CLOSED);
    abort_ = false;

    nrdApp()->getSystem()->notifyApplicationState(ISystem::BROWSING);
    NTRACE(TRACE_MEDIASTARTUP, "closed AdaptiveStreamingPlayer: time=%llu",
           Time::mono().val());

    // reset sessionId
    eventThread_->setSessionId(0);
}

NFErr AdaptiveStreamingPlayer::play(const Ticks& pts, bool playAndPause )
{
    NTRACE(TRACE_MEDIASTARTUP, "ASP play(): time=%llu playAndPause=%d",
           Time::mono().val(), playAndPause?1:0);
    PERFTRACE("ASP play(): time=%llu\n", Time::mono().val());
    {
        ScopedMutex stateLock(stateMutex_);

        // If we're faststarting and already playing, ignore this
        bool faststart = (streamingType_ != NORMAL_PLAY);
        if (faststart && state_ == PLAYING)
        {
            NTRACE(TRACE_MEDIACONTROL, "faststart already playing, ignore second play()");
            return NFErr_OK;
        }

        // Note, pumpingThread should already be mutex protected against close
        if ((state_ != STOPPED) || !pumpingThread_.get() || playerState_->m_error)
        {
            Log::error(TRACE_MEDIACONTROL, "ASPlayer::play InvalidStateTransition: "
                       "state_ = %d, playerState error %s", state_,
                       playerState_->m_error ?  "true" : "false");
            return NFErr_MC_InvalidStateTransition_ShouldBeStopped;
        }
        if (playAndPause) {
            state_ = PAUSED;
            eventThread_->postStateChangedEvent(PAUSED);
        } else {
            state_ = PLAYING;
            eventThread_->postStateChangedEvent(PLAYING);
        }
    }

    nrdApp()->getSystem()->notifyApplicationState(ISystem::BUFFERING);

    // If the specified "pts" argument is invalid, the target seek time is 0 (i.e.,
    // start from the beginning of movie). The target seek time should be capped
    // at MAX_PTS (equals to MAX_UINT32 - 1, so the upper limit of representable
    // time position is about 1193 hours).
    uint32_t seekTime = 0;
    if (pts.valid())
    {
        ullong value = pts.value(Ticks::RES_1KHZ);

        if (value > playDuration_)
            seekTime = playDuration_;
        else
            seekTime = static_cast<uint32_t>(value);
    }

    if ( havePts_ && currentPts_ != seekTime )
    {
        Log::warn(TRACE_MEDIACONTROL, "currentPts_ %u not equal to seekTime %u",
                  currentPts_, seekTime);
        // don't return an error here
        // This can happen if bufferingComplete sets an actual startPts not equal
        // to the original seekTime
    }

    playbackSkipping_ = false;
    suppressBufferingEvents_ = false;
    predictedFutureRebuffers_ = 0;
    dataExhausted_ = true; // no data to start with

    // Reset the target playback speed to paused before data pumping is started.
    pumpingThread_->setPlaybackSpeed(IPlaybackDevice::PAUSED_SPEED);

    AseErrorCode retCode = startStreamingManager(currentManifestIndex_, seekTime);
    if (retCode != AS_NO_ERROR)
    {
        ScopedMutex scopedMutexResource(streamingManagerMutex_);
        streamingManager_.reset();

        // idle the pumping thread
        playerState_->m_error = true;

        // any failures from streaming manager should call streamingFailure() which
        // will post the error event to our listener

        NFErr err(new ASError(retCode));
        if ( retCode == AS_MEDIA_DURATION_ERROR )
        {
            return err.push(NFErr_MC_EncodingError);
        }
        else
        {
            return err.push(NFErr_MC_InitStreamFailure);
        }
    }

    // start the device
    startPlaybackDevice(seekTime);

    if ( pendingBufferingComplete_ )
    {
        pendingBufferingComplete_ = false;
        bufferingComplete();
    }

    return NFErr_OK;
}

NFErr AdaptiveStreamingPlayer::skip(const Ticks& pts)
{
    NTRACE(TRACE_MEDIACONTROL, "skip(): time=%llu", Time::mono().val());

    {
        ScopedMutex scopedMutexState(stateMutex_);
        if (state_ != PAUSED)
            return NFErr_NotAllowed;
    }

    shared_ptr<ManifestMetaData> mmd = manifestData_[currentManifestIndex_];

    {
        ScopedMutex scopedMutexSwitch(audioTrackSwitchMutex_);

        bool switchAlreadyInProgress = playerState_->m_waitOnAudioSwitch ||
            mmd->currentAudioIndex_ != static_cast<int32_t>(mmd->activeAudioIndex_);

        if (switchAlreadyInProgress)
        {
            NTRACE(TRACE_MEDIACONTROL,
                   "On-the-fly audio track switch in progress, not allowing skip");
            return NFErr_NotAllowed;
        }
    }

    Ticks startPts, endPts;
    getBufferRange(startPts, endPts);

    if ( !pts.valid() || (pts < startPts) || (pts > endPts) || (startPts == endPts) )
        return NFErr_NotAllowed;

    // the pts is valid and fall in the range
    ullong value = pts.value(Ticks::RES_1KHZ);

    suppressBufferingEvents_ = true;

    playbackSkipping_ = true;
    stopPlaybackDevice();
    playbackReporter_->playbackStopped(NFErr_Skip, AseTimeStamp(currentPts_, 1000));

    playbackReporter_->skipStarted();
    AseTimeStamp seekTime = AseTimeStamp(value, 1000);
    mediaSink_->skip(seekTime);
    streamingManager_->skip();

    playbackSkipping_ = false;

    startPlaybackDevice(value);

    playerState_->m_bufferingComplete = true;

    return NFErr_OK;
}

NFErr AdaptiveStreamingPlayer::startPlaybackDevice(ullong pts)
{
    NTRACE(TRACE_MEDIASTARTUP, "startPlaybackDevice: time=%llu", Time::mono().val());
    PERFTRACE("startPlaybackDevice: time=%llu\n", Time::mono().val());

    initParameters(pts);

    // Let the pumping thread start data pumping.
    // Note, this should already be mutex protected against close
    pumpingThread_->startDataPumping();

    // Enable processing of device events after data pumping is started.
    eventThread_->enableDeviceEvents();

    return NFErr_OK;
}

NFErr AdaptiveStreamingPlayer::stopPlaybackDevice()
{
    NTRACE(TRACE_MEDIASTARTUP, "stopPlaybackDevice: time=%llu", Time::mono().val());

    abort_ = true;

    // Disable processing of device events before data pumping gets stopped.
    eventThread_->disableDeviceEvents();

    // Note, pumping thread should already be mutex protected against close here
    if (pumpingThread_->stopDataPumping())
    {
        NTRACE(TRACE_MEDIASTARTUP, "stopped data pumping: time=%llu", Time::mono().val());
    }

    playbackStarted_ = false;
    abort_ = false;

    return NFErr_OK;
}

void AdaptiveStreamingPlayer::initParameters(uint32_t seekTime)
{
    // Some variables must be properly initialized or reset before we let the
    // pumping thread start data pumping.
    bool resetBufferingComplete = (seekTime != currentPts_);
    NTRACE(TRACE_MEDIACONTROL, "initParameters: resetBufferingComplete %s",
           resetBufferingComplete ? "true" : "false");

    playerState_->resetOnPlay(resetBufferingComplete);

    storeError_        = false;    // whether the error in this session is stored
    endOfPresentation_ = false;    // presentation is restarted.
    currentPts_        = seekTime;
    havePts_           = true;
    latestReportedPTS_ = currentPts_;

    // update playback reporter
    playbackReporter_->playbackReposition(AseTimeStamp(currentPts_, 1000));

    reportedVideoBitrate_ = 0;

    // XXX - which manifestIndex;
    shared_ptr<ManifestMetaData> mmd = manifestData_[currentManifestIndex_];

    mmd->audioSelStreamIndex_  = INVALID_STREAM_INDEX; // no stream is selected yet.
    mmd->videoSelStreamIndex_  = INVALID_STREAM_INDEX; // no stream is selected yet.
    mmd->audioPresTrack_       = INVALID_STREAM_INDEX; // no stream is presented yet.
    mmd->audioPresStreamIndex_ = INVALID_STREAM_INDEX; // no stream is presented yet.
    mmd->videoPresStreamIndex_ = INVALID_STREAM_INDEX; // no stream is presented yet.

  //  assert(mmd->currentAudioIndex_ >= 0);
    mmd->activeAudioIndex_ = mmd->currentAudioIndex_;
  //  assert(mmd->currentVideoIndex_ >= 0);
}

void AdaptiveStreamingPlayer::getBufferRange(Ticks& startPts, Ticks& endPts)
{
    ScopedMutex scopedMutexResource(streamingManagerMutex_);
    // if we don't have a media sink, return
    if (!mediaSink_.get())
        return;

    AseTimeStamp startTimeStamp, endTimeStamp;
    mediaSink_->getBufferRange(startTimeStamp, endTimeStamp);
    startPts = Ticks(startTimeStamp.getValueInMs(), Ticks::RES_1KHZ);
    endPts = Ticks(endTimeStamp.getValueInMs(), Ticks::RES_1KHZ);
}

uint32_t AdaptiveStreamingPlayer::getPredictedFutureRebuffers() const
{
    return predictedFutureRebuffers_;
}

NFErr AdaptiveStreamingPlayer::stop()
{
    NTRACE(TRACE_MEDIASTARTUP, "AdaptiveStreamingPlayer::stop(): time=%llu",
           Time::mono().val());
    assert(!abort_);

    {
        ScopedMutex scopedMutexState(stateMutex_);

        if (state_ == STOPPED)
            return NFErr_OK;

        if (state_ != PLAYING && state_ != PAUSED)
        {
            Log::warn(TRACE_MEDIACONTROL, "ASPlayer::stop InvalidStateTransition: "
                       "state_ = %d, playerState error %s", state_,
                       playerState_->m_error ?  "true" : "false");
            return NFErr_MC_InvalidStateTransition_ShouldBePlayingOrPaused;
        }

        state_ = STOPPED;
        havePts_ = false;
    }

    nrdApp()->getSystem()->notifyApplicationState(ISystem::STOPPING);

    // stop the playback device
    stopPlaybackDevice();

    updateErrorCount();

    eventThread_->postStateChangedEvent(STOPPED);

    pendingBufferingComplete_ = false;

    NTRACE(TRACE_MEDIASTARTUP, "stopped AdaptiveStreamingPlayer: time=%llu",
           Time::mono().val());
    return NFErr_OK;
}

NFErr AdaptiveStreamingPlayer::pause()
{
    ScopedMutex stateLock(stateMutex_);

    if (state_ == PAUSED)
        return NFErr_OK;

    if (state_ != PLAYING)
    {
        Log::warn(TRACE_MEDIACONTROL, "ASPlayer::pause InvalidStateTransition: "
                   "state_ = %d, playerState error %s", state_,
                   playerState_->m_error ?  "true" : "false");
        return NFErr_MC_InvalidStateTransition_ShouldBePlaying;
    }

    // Note, pumping thread should already be mutex protected against close here
    pumpingThread_->setPlaybackSpeed(IPlaybackDevice::PAUSED_SPEED);

    // notify playback reporter
    playbackReporter_->playbackStateChange(AseTimeStamp(currentPts_, 1000),
                                           IMediaPipelineReporter::PAUSED_STATE);

    state_ = PAUSED;
    eventThread_->postStateChangedEvent(PAUSED);

    nrdApp()->getSystem()->notifyApplicationState(ISystem::PAUSED);

    return NFErr_OK;
}

NFErr AdaptiveStreamingPlayer::unpause()
{
    ScopedMutex stateLock(stateMutex_);

    if (state_ == PLAYING)
        return NFErr_OK;

    if (state_ != PAUSED)
    {
        Log::warn(TRACE_MEDIACONTROL, "ASPlayer::unpause InvalidStateTransition: "
                   "state_ = %d, playerState error %s", state_,
                   playerState_->m_error ?  "true" : "false");
        return NFErr_MC_InvalidStateTransition_ShouldBePaused;
    }

    // Note, pumping thread should already be mutex protected against close here
    if (playerState_->m_halted)
    {
        pumpingThread_->setPlaybackSpeed(IPlaybackDevice::PAUSED_SPEED);
        nrdApp()->getSystem()->notifyApplicationState(ISystem::BUFFERING);
    }
    else
    {
        pumpingThread_->setPlaybackSpeed(IPlaybackDevice::NORMAL_SPEED);
        nrdApp()->getSystem()->notifyApplicationState(ISystem::PLAYING);
    }

    // notify playback reporter
    playbackReporter_->playbackStateChange(AseTimeStamp(currentPts_, 1000),
                                           IMediaPipelineReporter::UNPAUSED_STATE);

    state_ = PLAYING;
    eventThread_->postStateChangedEvent(PLAYING);

    return NFErr_OK;
}

NFErr AdaptiveStreamingPlayer::addListener(shared_ptr<Listener> l)
{
    // forward to event thread
    eventThread_->addListener(l);
    return NFErr_OK;
}

NFErr AdaptiveStreamingPlayer::removeListener(shared_ptr<Listener> l)
{
    // forward to event thread
    eventThread_->removeListener(l);
    return NFErr_OK;
}

NFErr AdaptiveStreamingPlayer::selectAudioTrack(const string& trackId)
{
    ScopedMutex scopedMutexState(stateMutex_);

    if (state_ == OPENING || state_ == CLOSED)
        return NFErr_NotAllowed;

    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, currentManifestIndex_);
    const vector<ManifestTrackPtr>& tracks = manifest->getTracks();
    uint32_t trackIndex = 0;
    bool findAudioTrack = false;
    for (trackIndex = 0; trackIndex < mmd->audioTracksIndex_.size(); ++trackIndex)
    {
        ManifestTrackPtr track = tracks[mmd->audioTracksIndex_[trackIndex]];
        if (track->getTrackKey() == trackId) {
            findAudioTrack = true;
            break;
        }
    }

    if ( findAudioTrack )
    {
        NTRACE(TRACE_MEDIACONTROL, "ASP::selectAudioTrack: %s, %d", trackId.c_str(), trackIndex);
    }
    else
    {
        NTRACE(TRACE_MEDIACONTROL, "ASP::selectAudioTrack: no match track for track id: %s", trackId.c_str());
        return NFErr_NotAllowed;
    }

    if (mmd->currentAudioIndex_ == static_cast<int>(trackIndex)) {
        NTRACE(TRACE_MEDIACONTROL, "ASP::selectAudioTrack: already selected");
        return NFErr_OK;
    }

    NFErr retCode = NFErr_OK;

    {
        ScopedMutex scopedMutexSwitch(audioTrackSwitchMutex_);
        bool onTheFlySwitchIsAllowed = true;

        if (state_ == PLAYING || state_ == PAUSED)
        {
            bool switchAlreadyInProgress = playerState_->m_waitOnAudioSwitch ||
                mmd->currentAudioIndex_ != static_cast<int32_t>(mmd->activeAudioIndex_);

            onTheFlySwitchIsAllowed = !switchAlreadyInProgress && !dataExhausted_;

            if (onTheFlySwitchIsAllowed)
            {
                NTRACE(TRACE_MEDIACONTROL, "On-the-fly audio track switch started: "
                       "time=%llu ms, old track id: %d, new track id: %d",
                       Time::mono().val(),
                       mmd->audioTracksIndex_[mmd->currentAudioIndex_],
                       mmd->audioTracksIndex_[trackIndex]);
            }
            else
            {
                NTRACE(TRACE_MEDIACONTROL, "On-the-fly audio track switch not started "
                       "because dataExhausted = %u, switchAlreadyInProgress = %u, "
                       "time=%llu ms, "
                       "old track id: %d, new track id: %d",
                       dataExhausted_, switchAlreadyInProgress,
                       Time::mono().val(),
                       mmd->audioTracksIndex_[mmd->currentAudioIndex_],
                       mmd->audioTracksIndex_[trackIndex]);
            }
        }

        if (onTheFlySwitchIsAllowed)
        {
            mmd->currentAudioIndex_ = trackIndex;
        }
        else
        {
            retCode = NFErr_NotAllowed;
        }
    }

    // Inform the bridge that tracks changed now. There's nothing
    // left to do because we're not playing back.
    if (state_ != PLAYING && state_ != PAUSED)
    {
        notifyTracksChanged();
    }

    // For the playing state case, postTracksChangedEvent is handled
    // in audioTrackSwitchComplete.

    return retCode;
}

// The video track index is stored in tracks[0], audio is stored in tracks[1].
void AdaptiveStreamingPlayer::getCurrentAudioTrack(std::string &trackId) const
{
    if (!manifestData_.empty())
    {
        shared_ptr<Manifest> manifest;
        shared_ptr<ManifestMetaData> mmd;
        getManifest(/*out*/manifest, /*out*/mmd, currentManifestIndex_);
        const vector<ManifestTrackPtr>& tracks = manifest->getTracks();
        ManifestTrackPtr track = tracks[mmd->audioTracksIndex_[mmd->currentAudioIndex_]];
        trackId = track->getTrackKey();
    }
    else
    {
        trackId = "";
    }
}

Ticks AdaptiveStreamingPlayer::getCurrentPts() const
{
    uint32_t currentPts = currentPts_;

    if (currentPts == INVALID_PTS)
        return Ticks();
    else
        return Ticks(currentPts, Ticks::RES_1KHZ);
}

Ticks AdaptiveStreamingPlayer::getPlayDuration() const
{
    uint32_t playDuration = playDuration_;

    if (playDuration == INVALID_DURATION)
        return Ticks();
    else
        return Ticks(playDuration, Ticks::RES_1KHZ);
}

uint32_t AdaptiveStreamingPlayer::getGopSize() const
{
    return gopSize_;
}

void AdaptiveStreamingPlayer::getDisplayAspectRatio(uint32_t& aspectRatioX,
                                                    uint32_t& aspectRatioY) const
{
    ScopedMutex scopedMutexState(stateMutex_);

    if (state_ == OPENING || state_ == CLOSED)
    {
        aspectRatioX = 0;
        aspectRatioY = 0;
    }
    else
    {
        shared_ptr<Manifest> manifest;
        shared_ptr<ManifestMetaData> mmd;
        getManifest(/*out*/manifest, /*out*/mmd, currentManifestIndex_);

        // get the video track
        const vector<ManifestTrackPtr> tracks = manifest->getTracks();
        const vector<ManifestStreamPtr> streams =
            tracks[mmd->videoTracksIndex_[mmd->currentVideoIndex_]]->getStreams();

        pair<int,int> aspect = streams[0]->getPixelAspectRatio();
        int pixelAspectRatioX = aspect.first;
        int pixelAspectRatioY = aspect.second;

        pair<int,int> videoSize = streams[0]->getVideoSize();

        int videoImageWidth_ = videoSize.first;
        int videoImageHeight_ = videoSize.second;

        aspectRatioX = pixelAspectRatioX * videoImageWidth_;
        aspectRatioY = pixelAspectRatioY * videoImageHeight_;
    }
}

void AdaptiveStreamingPlayer::setVideoWindow(Rect targetRect, uint32_t transitionDuration)
{
    ScopedMutex scopedMutexState(stateMutex_);

    if (!deviceInitialized_)
    {
        videoWindowTargetRectPending_ = targetRect;
        videoWindowTransitionDurationPending_ = transitionDuration;
        newVideoWindowPending_ = true;
    }
    else
    {
        ScopedMutex playbackLock(playbackDeviceMutex_);

        Rect actualRect =
            playbackDevice_->setVideoWindow(targetRect, transitionDuration);

        eventThread_->postVideoWindowChangedEvent(actualRect);
    }
}

NFErr AdaptiveStreamingPlayer::bringVideoToFront()
{
    return NFErr_NotAllowed;
}

NFErr AdaptiveStreamingPlayer::sendVideoToBack()
{
    return NFErr_NotAllowed;
}

IAdaptiveStreamingPlayer::NetworkProfile AdaptiveStreamingPlayer::getNetworkProfile() const
{
    ScopedMutex lock(metaDataMutex_);
    return networkProfile_;
}

void AdaptiveStreamingPlayer::setNetworkProfile(NetworkProfile nwProfile)
{
    ScopedMutex lock(metaDataMutex_);
    networkProfile_ = nwProfile;
}

NFErr AdaptiveStreamingPlayer::setVideoBitrateRanges(vector< shared_ptr<BitrateRange> > const& videoBitrateRanges)
{
    if (playerState_->m_waitOnVideoBitrateSelection)
        return NFErr_NotAllowed;

    playerState_->m_waitOnVideoBitrateSelection = true;
    {
        ScopedMutex scopedMutexResource(streamingManagerMutex_);
        if (streamingManager_.get())
            streamingManager_->setVideoBitrateRanges(videoBitrateRanges);

        videoBitrateRanges_ = videoBitrateRanges;
    }
    return NFErr_OK;
}

uint32_t AdaptiveStreamingPlayer::getCurrentBandwidth() const
{
    return currentBandwidth_;
}

uint32_t AdaptiveStreamingPlayer::getMaxStreamingBuffer() const
{
    ScopedMutex scopedMutexResource(streamingManagerMutex_);
    if ( streamingManager_.get() )
        return streamingManager_->getMaxStreamingBuffer();
    else
        return maxStreamingBufferLen_;
}

void AdaptiveStreamingPlayer::setStreamingBuffer(bool powerSaving,
                                                 uint32_t maxBufferLen,
                                                 uint32_t minBufferLen)
{
    maxStreamingBufferLen_ = maxBufferLen;
    minStreamingBufferLen_ = minBufferLen;
    powerSaving_ = powerSaving;
    {
        ScopedMutex scopedMutexResource(streamingManagerMutex_);
        if ( streamingManager_.get() )
            streamingManager_->setStreamingBuffer(powerSaving_, maxStreamingBufferLen_, minStreamingBufferLen_);
    }
}


void AdaptiveStreamingPlayer::getMediaBufferPoolSize(
        uint32_t& videoBufferPoolSize, uint32_t& audioBufferPoolSize)
{
    ScopedMutex lock(metaDataMutex_);
    videoBufferPoolSize = videoBufferPoolSize_;
    audioBufferPoolSize = audioBufferPoolSize_;
    return;
}

//////////
//
// IStreamingReporter
//
//////////
void AdaptiveStreamingPlayer::openCompleted(uint32_t duration, uint32_t gopSize)
{
    NTRACE(TRACE_MEDIACONTROL, "openCompleted, duration: %u ms",
                duration );
    playDuration_ = duration;
    gopSize_ = gopSize;

    // forward to playback reporter
    playbackReporter_->openCompleted(duration, gopSize);
    openComplete_ = true;
    openCompleteSignal_.signal();
}

void AdaptiveStreamingPlayer::updateContentDuration(uint32_t duration)
{
    playDuration_ = duration;
    eventThread_->postUpdateContentDurationEvent(duration);
    playbackReporter_->updateContentDuration(duration);
}

void AdaptiveStreamingPlayer::notifyManifestSelected(uint32_t manifestIndex)
{
    NTRACE(TRACE_MEDIACONTROL, "notifyManifestSelected %d", manifestIndex);
    ScopedMutex lock(manifestMutex_);

    uint32_t lastManifest = currentManifestIndex_;
    currentManifestIndex_ = manifestIndex;

    shared_ptr<Manifest> oldManifest = manifests_[lastManifest];
    shared_ptr<ManifestMetaData> oldmmd = manifestData_[lastManifest];

    shared_ptr<Manifest> newManifest = manifests_[manifestIndex];
    shared_ptr<ManifestMetaData> newmmd = manifestData_[manifestIndex];

    // convert the currentAudioIndex
    newmmd->currentAudioIndex_ = convertManifestTrack(
        oldManifest, newManifest, oldmmd->audioTracksIndex_[oldmmd->currentAudioIndex_]);
    if (newmmd->currentAudioIndex_ == -1)
    {
        Log::error(TRACE_MEDIACONTROL,
                   "Unable to find audio track on manifest switch, reverting to default");
        newmmd->currentAudioIndex_ = newmmd->defaultAudioIndex_;
    }
    newmmd->activeAudioIndex_ = newmmd->currentAudioIndex_;

#define GET_TRACK_KEY(manifest, mmd) \
    manifest->getTracks()[mmd->audioTracksIndex_[mmd->currentAudioIndex_]]->getTrackKey()

    // make sure the new track matches
    assert(GET_TRACK_KEY(oldManifest, oldmmd) == GET_TRACK_KEY(newManifest, newmmd));

    // currentVideoIndex should've been set when the manifest was processed on add

    // notify the playback reporter
    playbackReporter_->notifyManifestSelected(manifestIndex);

    // notify listeners
    eventThread_->postManifestSelectedEvent(manifestIndex);
}

void AdaptiveStreamingPlayer::bufferingStarted()
{
    NTRACE(TRACE_MEDIACONTROL, "bufferingStarted");

    // forward to playback reporter
    playbackReporter_->bufferingStarted();

    if ( !suppressBufferingEvents_ )
        eventThread_->postBufferingEvent(1);
}

void AdaptiveStreamingPlayer::bufferingComplete(uint32_t actualStartPts,
                                                ullong predictedBufferingTime,
                                                uint32_t initialBW, uint32_t actualBW)
{
    NTRACE(TRACE_MEDIASTARTUP, "bufferingComplete: state: %u", state_);
    // update our current pts
    currentPts_ = actualStartPts;

    // forward to playback reporter
    playbackReporter_->bufferingComplete(actualStartPts, predictedBufferingTime,
                                         initialBW, actualBW);
    playbackReporter_->reportKeyStartplayEvent("bufferingcomplete");

    {
        ScopedMutex stateLock(stateMutex_);

        if ( (state_ == PAUSED) || (state_ == PLAYING) )
        {
            bufferingComplete();

        }
        else
        {
            // In the case that buffering complete comes earlier than play(), store the state
            pendingBufferingComplete_ = true;
        }
    }
}

void AdaptiveStreamingPlayer::bufferingComplete()
{
    if (!playerState_->m_bufferingComplete)
    {
        NTRACE(TRACE_MEDIASTARTUP, "bufferingComplete(): time=%llu",
                Time::mono().val());
        PERFTRACE("bufferingComplete(): time=%llu\n",
                Time::mono().val());

        playerState_->m_bufferingComplete = true;

        // notify the license retriever that buffering is done in case it was waiting
        // for bandwidth availability
        if (licenseRetriever_.get())
            licenseRetriever_->notifyBufferingComplete();
    }
}


void AdaptiveStreamingPlayer::bufferingStatus(uint32_t totalBufferingTime,
                                              uint32_t currentBufferingTime)
{
    // forward to playback reporter
    playbackReporter_->bufferingStatus(totalBufferingTime, currentBufferingTime);

    // The LICENSE_ACQ_PERCENTAGE setting defines how much percentage in buffering
    // progress is contributed by the license acquisition step. The remaining portion
    // (100 - LICENSE_ACQ_PERCENTAGE) is contributed by the progress of stream data
    // buffering (reported by the stream manager).

    if (totalBufferingTime > 0)
    {
        uint32_t percentage = ((100 - LICENSE_ACQ_PERCENTAGE) * currentBufferingTime)
                              / totalBufferingTime;

        if (playerState_->m_licenseAcquired)
            percentage += LICENSE_ACQ_PERCENTAGE;

        if ( !suppressBufferingEvents_ )
            eventThread_->postBufferingEvent(percentage);
    }
}

void AdaptiveStreamingPlayer::streamingFailureReasons(AseErrorCode networkError, int32_t httpErr)
{
    playbackReporter_->streamingFailureReasons(networkError, httpErr);
}

void AdaptiveStreamingPlayer::streamingFailure(int32_t manifestIndex,
                                               int32_t trackIndex, int32_t streamIndex,
                                               AseErrorCode error, int32_t httpErr,
                                               const string &errorMsg)
{
    // forward to playback reporter
    playbackReporter_->streamingFailure(manifestIndex, trackIndex, streamIndex, error,
                                        httpErr, errorMsg);
    Log::warn(TRACE_MEDIACONTROL, "streamingFailure: %d %d %s", error, httpErr,
              errorMsg.c_str());

    // set the top error depending on the AseErrorCode
    NFErr err(new ASError(error));
    switch (error)
    {
        case AS_SOCKET_ERROR:
        case AS_DNS_ERROR:
        case AS_DNS_TIMEOUT:
        case AS_DNS_NOT_FOUND:
        case AS_CONNECTION_NOT_OPEN:
        case AS_CONNECTION_ERROR:
        case AS_CONNECTION_TIMEOUT:
        case AS_CONNECTION_RESET:
        case AS_CONNECTION_CLOSED:
        case AS_CONNECTION_REFUSED:
        case AS_CONNECTION_NET_UNREACHABLE:
        case AS_HTTP_CONNECTION_STALL:
        case AS_HTTP_PROTOCOL_ERROR:
        case AS_HTTP_PROTOCOL_1_0:
        case AS_HTTP_RESPONSE_3XX:
        case AS_HTTP_RESPONSE_4XX:
        case AS_HTTP_RESPONSE_5XX:
        case AS_HTTP_PIPELINING_NOT_SUPPORTED:
        case AS_HTTP_PIPELINING_NOT_STABLE:
        case AS_NETWORK_FAILURE:
        case AS_HTTP_CLIENT_ERROR:
        case AS_HTTP_SERVER_ERROR:
        case AS_DOMAIN_TIMEOUT:
        case AS_DOMAIN_CONNECTION_ERROR:
        case AS_DOMAIN_SELECTION_FAILURE:
        case AS_DOMAIN_SELECTION_FAILURE_FAILED_TOO_LONG:
        case AS_DOMAIN_SELECTION_FAILURE_FAILED_TOO_MANY:
        case AS_DOMAIN_SELECTION_FAILURE_BAD_STREAM:
        case AS_DOMAIN_SELECTION_FAILURE_NO_AVAILABLE_LOCATION:
        case AS_DOMAIN_SELECTION_FAILURE_FAILED_TOO_MANY_CONSECUTIVE_TIMES:
        case AS_INCOMPATIBLE_URL:
        case AS_HTTP_ALL_URLS_FAILED:
            err.push(NFErr_MC_ConnectionFailure);
            break;
        case AS_INIT_FAILURE:
            err.push(NFErr_MC_StreamingInitFailure);
            break;

        default:
            err.push(NFErr_MC_ConnectionFailure);
            break;
    }

    // push the streaming failure on the error stack
    err.push(NFErr_MC_StreamingFailure);

    // if we get a streaming failure for the fastplay non-drm streams,
    // add to the error to the bridge who will restart the playback as non-faststart
    if ((streamingType_ == FAST_PLAY) && (manifestIndex == 0))
    {
        Log::warn(TRACE_MEDIACONTROL,
                  "streamingFailure in fastplay non-drm manifest at pts %d", currentPts_);

        uint32_t pts = (currentPts_ == INVALID_PTS) ? 0 : currentPts_;
        eventThread_->postRestartPlaybackEvent(err, Ticks(pts, Ticks::RES_1KHZ));
    }
    else
    {
        eventThread_->postErrorEvent(err);
    }

    updateErrorCount();

    playerState_->m_error = true;
}

void AdaptiveStreamingPlayer::reportStreamingServerFailure(uint32_t cdnId)
{
    playbackReporter_->reportStreamingServerFailure(cdnId);

}

void AdaptiveStreamingPlayer::streamSelected(int32_t manifestIndex,
                                             int32_t trackIndex, int32_t streamIndex,
                                             const AseTimeStamp &movieTime,
                                             uint32_t bandwidth, uint32_t longtermBw,
                                             uint32_t rebuffer)
{
    // always forward the stream selected message to the playback reporter
    playbackReporter_->streamSelected(manifestIndex,
                                      trackIndex, streamIndex,
                                      movieTime, bandwidth, longtermBw,
                                      rebuffer);


    if ((state_ != OPENING) && (state_ != PLAYING) && (state_ != PAUSED))
    {
        NTRACE(TRACE_MEDIACONTROL, "streamSelected ignored: AdaptiveStreamingPlayer state %d", state_);
        return;
    }

    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, manifestIndex);

    uint32_t index = mmd->firstStreamIndexInTrack_[trackIndex] + streamIndex;

    const vector<ManifestTrackPtr> &tracks = manifest->getTracks();
    MediaType mediaType = tracks[trackIndex]->getMediaType();

    switch (mediaType)
    {
        case MEDIA_AUDIO:
            if (index == mmd->audioSelStreamIndex_)
            {
                NTRACE(TRACE_MEDIACONTROL, "streamSelected: audio unchanged %d", index);
                return; // stream unchanged, no notification necessary
            }
            mmd->audioSelStreamIndex_ = index;
            break;

        case MEDIA_VIDEO:
            if (index == mmd->videoSelStreamIndex_)
            {
                NTRACE(TRACE_MEDIACONTROL, "streamSelected: video unchanged %d", index);
                return; // stream unchanged, no notification necessary
            }
            mmd->videoSelStreamIndex_ = index;
            break;

        default:
            assert(false); // unexpected.
    }

    // Post notification to the user about a new stream (audio or video) being
    // selected by the stream manager (the new stream content will be presented when
    // the new stream data reaches the end of the playback pipeline).
    // FIXME: post manifest index too?
    eventThread_->postStreamSelectedEvent(manifestIndex, trackIndex, streamIndex,
                                          movieTime.getValueInMs(), rebuffer);
}

void AdaptiveStreamingPlayer::updateStreamingPts(int32_t manifestIndex,
                                              int32_t trackIndex,
                                              const AseTimeStamp &movieTime)
{
    // forward to playback reporter
    playbackReporter_->updateStreamingPts(manifestIndex, trackIndex, movieTime);
}

void AdaptiveStreamingPlayer::updateBufferLevel(uint32_t abuflbytes, uint32_t vbuflbytes,
                                                uint32_t predictedFutureRebuffers,
                                                uint32_t currentBandwidth)
{
    playbackReporter_->updateBufferLevel(abuflbytes, vbuflbytes, predictedFutureRebuffers,
                                         currentBandwidth);
    if ( predictedFutureRebuffers_ != predictedFutureRebuffers ) {
        NTRACE(TRACE_MEDIACONTROL, "updateBufferLevel, rebuffer count change: %u, %u",
                                    predictedFutureRebuffers_, predictedFutureRebuffers);
    }

    predictedFutureRebuffers_ = predictedFutureRebuffers;
    currentBandwidth_ = currentBandwidth;
}

void AdaptiveStreamingPlayer::updateStreamingStat( shared_ptr<StreamingStatReport> pStreamingStat)
{
    ScopedMutex scopedMutexResource(streamingStatMutex_);
    streamingStat_ = pStreamingStat;
}

shared_ptr<StreamingStatReport> AdaptiveStreamingPlayer::obtainStreamingStat( )
{
    ScopedMutex scopedMutexResource(streamingStatMutex_);
    return streamingStat_;
}

void AdaptiveStreamingPlayer::getPlaybackQualityStats(device::IPlaybackDevice::PlaybackQualityStats& audioStats,
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

void AdaptiveStreamingPlayer::serverSelected( uint32_t selcdnid,
                                              std::string const& selcdnname,
                                              uint32_t selcdnrtt,
                                              uint32_t selcdnbw,
                                              std::string const& domainbwdata,
                                              std::string const& testreason,
                                              std::string const& selreason,
                                              uint32_t fastselthreshold )
{
    playbackReporter_->serverSelected(selcdnid, selcdnname,
                                      selcdnrtt, selcdnbw,
                                      domainbwdata,
                                      testreason, selreason,
                                      fastselthreshold);
}


void AdaptiveStreamingPlayer::connectionSelected1( uint32_t origmaxnum, uint32_t actualmaxnum, uint32_t rtt )
{
    playbackReporter_->connectionSelected1(origmaxnum, actualmaxnum, rtt);
}

void
AdaptiveStreamingPlayer::connectionSelected2(uint32_t defaultnumconn, uint32_t selnumconn, uint32_t maxnumconn)
{
    playbackReporter_->connectionSelected2( defaultnumconn, selnumconn, maxnumconn );
}

void
AdaptiveStreamingPlayer::videoBitrateSelectionCompleted()
{
    NTRACE(TRACE_MEDIACONTROL, "videoBitrateSelectionCompleted");
    playerState_->m_waitOnVideoBitrateSelection = false;
    eventThread_->postVideoBitrateSelectionCompleteEvent();
}

void
AdaptiveStreamingPlayer::reportKeyStartplayEvent(string const& eventName)
{
    NTRACE(TRACE_MEDIACONTROL, "reportKeyStartplayEvent: %s", eventName.c_str());
    playbackReporter_->reportKeyStartplayEvent(eventName);
    return;
}

void
AdaptiveStreamingPlayer::pipeliningDisabled()
{
    playbackReporter_->pipeliningDisabled();
    return;
}

void AdaptiveStreamingPlayer::serverSwitch(
                        uint32_t                        cdnid,
                        shared_ptr<ILocationSetMonitor> pLocationSetMonitor,
                        ServerSelectionReason           reason,
                        uint32_t                        bitrate )
{
    playbackReporter_->serverSwitch( cdnid, pLocationSetMonitor, reason, bitrate );
}

void AdaptiveStreamingPlayer::reportFileToken( std::vector<unsigned char> const& filetoken )
{
    playbackReporter_->reportFileToken( filetoken );
}

//================================================================================
// Functions called from the pumping thread and interlocked with control state.
//================================================================================

void AdaptiveStreamingPlayer::notifyLoadingComplete()
{
    ScopedMutex scopedMutexState(stateMutex_);

    assert(state_ == OPENING || state_ == CLOSED);

    if (state_ == OPENING)
    {
        state_ = STOPPED;
        eventThread_->postStateChangedEvent(STOPPED);
        eventThread_->postOpenCompleteEvent();

        ScopedMutex playbackLock(playbackDeviceMutex_);

        if (newVideoZOrderPending_)
        {
            if (videoZOrderPending_ == VIDEO_AT_FRONT)
                playbackDevice_->bringVideoToFront();
            else
                playbackDevice_->sendVideoToBack();

            newVideoZOrderPending_ = false;
        }
    }
}

void AdaptiveStreamingPlayer::haltPlayback(bool halted)
{
    ScopedMutex scopedMutexState(stateMutex_);

    NTRACE(TRACE_MEDIACONTROL, "haltPlayback %s", halted ? "true" : "false");

    assert(state_ == PLAYING || state_ == PAUSED || state_ == STOPPED);

    playerState_->m_halted = halted;

    if (state_ == STOPPED)
        return;

    // Note, this is called from the pumpingThread
    if (state_ == PLAYING && !playerState_->m_halted)
    {
        pumpingThread_->setPlaybackSpeed(IPlaybackDevice::NORMAL_SPEED);
        nrdApp()->getSystem()->notifyApplicationState(ISystem::PLAYING);
    }
    else
    {
        pumpingThread_->setPlaybackSpeed(IPlaybackDevice::PAUSED_SPEED);

        if (state_ == PAUSED)
            nrdApp()->getSystem()->notifyApplicationState(ISystem::PAUSED);
        else
            nrdApp()->getSystem()->notifyApplicationState(ISystem::BUFFERING);
    }
}

//================================================================================
// Media loading/unloading procedures.
//================================================================================

bool AdaptiveStreamingPlayer::load()
{
    NFErr err;
    AseErrorCode retCode = AS_NO_ERROR;

    // XXX - use resetOnPlay()?
    playerState_->m_waitOnAudioSwitch = false;
    playerState_->m_waitOnVideoBitrateSelection = false;

    NTRACE(TRACE_MEDIASTARTUP,
           "load(#3): begin create streaming manager, time=%llu",
           Time::mono().val());
    PERFTRACE("load(#3): begin create streaming manager, time=%llu\n",
           Time::mono().val());

    shared_ptr<ManifestMetaData> mmd = manifestData_[currentManifestIndex_];

    do {
        // calculate global stream indices
        calculateStreamIndices( currentManifestIndex_ );

        // Create the stream manager with the audio/video media streams vectors.
        err = createStreamingManager(mmd->defaultAudioIndex_);
        if (!err.ok())
            break;

        NTRACE(TRACE_MEDIASTARTUP,
               "load(#3.1): created streaming manager, time=%llu",
               Time::mono().val());
        PERFTRACE("load(#3.1): created streaming manager, time=%llu\n",
               Time::mono().val());

        // Wait for the first header
        uint32_t firstSelTrackIndex, firstSelStreamIndex;
        shared_ptr<std::vector<unsigned char> > firstSelStreamHeader;

        err = waitForFirstHeader( firstSelTrackIndex,
                                  firstSelStreamIndex,
                                  firstSelStreamHeader );
        if (!err.ok())
            break;

        // build up the streamInfos from the manifest
        vector<IPlaybackDevice::StreamInfo> streamInfos;
        ContentProfile lastVideoProfile = buildStreamInfos(/*out*/streamInfos,
                                                           currentManifestIndex_,
                                                           firstSelTrackIndex,
                                                           firstSelStreamIndex,
                                                           firstSelStreamHeader);


        // Start the streaming manager if we have a start PTS
        // and we successfully matched an audio stream to the MediaContol request
        bool faststart = (streamingType_ != NORMAL_PLAY);
        if ( havePts_ && mmd->currentAudioIndex_ >= 0 && faststart )
        {
            retCode = startStreamingManager( currentManifestIndex_, currentPts_ );
            if ( retCode != AS_NO_ERROR )
                break;
        }

        // Open playback device if it's not opened yet.
        err = openPlaybackDevice( streamInfos, lastVideoProfile );
        if (!err.ok())
            break;

        NTRACE(TRACE_MEDIASTARTUP, "load(#7): load completed, time=%llu",
               Time::mono().val());
        PERFTRACE("load(#7): load completed, time=%llu\n",
               Time::mono().val());

        return true;

    } while( false );


    Log::warn(TRACE_MEDIASTARTUP, "load(#7.1): error %s, ase error %d, time=%llu",
              err.toString().c_str(), retCode, Time::mono().val());

    // call unload() to cleanup anything we may have created
    unload();

    return false;
}

void AdaptiveStreamingPlayer::unload()
{
    NTRACE(TRACE_MEDIACONTROL, "unloading AdaptiveStreamingPlayer");

    // let go of the media sink before destroying the buffer pool
    {
        ScopedMutex scopedMutexResource(streamingManagerMutex_);

        // Destroy the stream manager.
        if (streamingManager_.get())
            streamingManager_->close();
        streamingManager_.reset();

        streamingHeuristic_.reset();

        if (mediaSink_.get())
            mediaSink_.reset();
    }

    if (mediaBufferPool_.get())
    {
        mediaBufferPool_->close();
        mediaBufferPool_.reset();
    }
    {
        ScopedMutex lock(metaDataMutex_);
        videoBufferPoolSize_ = 0;
        audioBufferPoolSize_ = 0;
    }

    if (deviceOpened_)
    {
        // Close the playback device and device buffer manager.
        ScopedMutex playbackLock(playbackDeviceMutex_);
        if (playbackDevice_.get())
            playbackDevice_->close();
        deviceOpened_ = false;
        deviceInitialized_ = false;
    }
    NTRACE(TRACE_MEDIACONTROL, "closed playback device");

    // Destroy manifests
    {
        ScopedMutex lock(manifestMutex_);
        manifests_.clear();
        manifestData_.clear();
        currentManifestIndex_ = -1;
    }

    currentPts_ = INVALID_PTS;
    playDuration_ = INVALID_DURATION;
    NTRACE(TRACE_MEDIACONTROL, "unloaded AdaptiveStreamingPlayer.");
}

NFErr AdaptiveStreamingPlayer::createStreamingManager(uint32_t defaultAudioIndex)
{
    // create the buffer pool
    NFErr err = createBufferPool();
    if (!err.ok())
        return err;

    {
        ScopedMutex scopedMutexResource(streamingManagerMutex_);
        err = updateConfigParameters();
        if (!err.ok())
            return err;

        NTRACE(TRACE_MEDIACONTROL, "StreamingConfigParameter: %s",
               streamingConfigParameter_->toString().c_str());

        ManifestTrackPtr videoTrack = getCurrentVideoTrack();
        if (!videoTrack.get())
            return NFErr_Bad;

        shared_ptr<Manifest> manifest;
        shared_ptr<ManifestMetaData> mmd;
        getManifest(/*out*/manifest, /*out*/mmd, currentManifestIndex_);

        mediaSink_.reset(
            new McMediaSink(manifest, streamingConfigParameter_,
                            localConfigParameter_,
                            videoTrack->isStereoVideo()));

        // Create the stream manager, with the resource-mutex locked.
        constructStreamingHeuristic(
            manifest,
            streamingConfigParameter_,
            localConfigParameter_,
            persistentStore_,
            static_pointer_cast<AdaptiveStreamingPlayer>(shared_from_this()),
            streamingHeuristic_);

        AseErrorCode ret = StreamingManager::construct(
            manifest,
            streamingConfigParameter_,
            localConfigParameter_,
            streamingHeuristic_,
            mediaSink_,
            mediaBufferPool_,
            static_pointer_cast<AdaptiveStreamingPlayer>(shared_from_this()),
            playbackReporter_,
            persistentStore_,
            (streamingType_ == FAST_PLAY),
            /*out*/streamingManager_);

        if (ret != AS_NO_ERROR)
        {
            Log::warn(TRACE_MEDIACONTROL,
                       "Failed to construct the streaming manager: %u", ret);
            return NFErr_MC_InitStreamFailure;
        }

        (void) streamingManager_->networkChange(nrdApp()->systemData()->getNetworkInterfaceInfo());

        NTRACE(TRACE_MEDIACONTROL, "streaming manager constructed");
    }

    if (!abort_)
    {
        // get the manifest data
        shared_ptr<Manifest> currentManifest;
        shared_ptr<ManifestMetaData> mmd;
        getManifest(/*out*/currentManifest, /*out*/mmd, currentManifestIndex_);

        vector<uint32_t> trackIndices;
        trackIndices.push_back(mmd->videoTracksIndex_[mmd->currentVideoIndex_]);
        trackIndices.push_back(mmd->audioTracksIndex_[defaultAudioIndex]);

        if ( !videoBitrateRanges_.empty() )
            setVideoBitrateRanges(videoBitrateRanges_);

        streamingManager_->setStreamingBuffer(powerSaving_, maxStreamingBufferLen_, minStreamingBufferLen_);

        // get media files from cache
        vector<shared_ptr<IMediaFile> > mediaFiles;
        NFErr mferr = mediaFileCache_->getMediaFiles(currentManifest->getContentId(),
                                                     /*out*/mediaFiles);
        if (!mferr.ok())
        {
            NTRACE(TRACE_MEDIACONTROL, "WARNING: unable to get mediaFiles for %s: %s",
                   currentManifest->getContentId().c_str(), mferr.toString().c_str());
            mediaFiles.clear();
        }

        AseErrorCode ret = streamingManager_->open(trackIndices, mediaFiles);
        if (ret != AS_NO_ERROR)
        {
            err.push(new ASError(ret));
            err.push(NFErr_MC_InitStreamFailure);
        }
    }
    else
        err = NFErr_MC_InitStreamFailure;

    if (!err.ok())
    {
        playerState_->m_error = true;

        // Destroy the stream manager, with the resource-mutex locked.
        ScopedMutex scopedMutexResource(streamingManagerMutex_);
        streamingManager_.reset();

        if (!abort_)
        {
            Log::error(TRACE_MEDIACONTROL, "Failed to open streaming manager: %s",
                       err.toString().c_str());
        }
    }
    else
        playbackReporter_->reportKeyStartplayEvent("streamingmanagercreated");

    return err;
}

void AdaptiveStreamingPlayer::getDeviceMaxVideoDimensions(
    vector<IPlaybackDevice::StreamInfo> const& streamInfos,
    uint32_t& maxImageWidth,
    uint32_t& maxImageHeight)
{
    // from StreamInfo, figure out what video profile group this stream is in
    VideoCodecType currentVideoCodecType = UNKNOWN_VIDEO_CODEC;
    std::vector<IPlaybackDevice::StreamInfo>::const_iterator streamIt;
    for(streamIt = streamInfos.begin(); streamIt != streamInfos.end(); streamIt++){
        if( streamIt->streamType_ == IPlaybackDevice::VIDEO){
            currentVideoCodecType = profileVideoCodecType(streamIt->contentProfile_);
            break;
        }
    }

    ISystem::Capability capabilities = nrdApp()->getSystem()->getCapability();

    uint32_t maxWidthForProfile = 0;
    uint32_t maxHeightForProfile = 0;
    bool bProfileRecognized;

    std::vector<ContentProfile>::iterator it;
    for(it = capabilities.videoProfiles.begin();
        it != capabilities.videoProfiles.end(); it++)
    {
        // here, we need to skip unrelated profile against current input video profile
        if (currentVideoCodecType != profileVideoCodecType(*it)){
            continue;
        }

        // Returns false and 1920x1080 if it doesn't recognize the profile.
        bProfileRecognized =
            getMaxFrameDimensionsForVideoProfile(*it, maxWidthForProfile, maxHeightForProfile);
        assert(bProfileRecognized);
        (void)bProfileRecognized;

        if(maxWidthForProfile > maxImageWidth) {
            maxImageWidth = maxWidthForProfile;
        }
        if(maxHeightForProfile > maxImageHeight) {
            maxImageHeight = maxHeightForProfile;
        }
    }
}

NFErr AdaptiveStreamingPlayer::waitForFirstHeader( /*out*/uint32_t& firstSelTrackIndex,
                                                   /*out*/uint32_t& firstSelStreamIndex,
                                                   /*out*/shared_ptr<std::vector<unsigned char> >& firstSelStreamHeader )
{
    // get the first media header from the media sink

    static const Time WAIT_WHILE_IDLING(100);

    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, currentManifestIndex_);

    while ( !openComplete_ )
    {
        ScopedMutex scopedMutex(openCompleteMutex_);
        openCompleteSignal_.wait(&openCompleteMutex_, WAIT_WHILE_IDLING);
        if (abort_)
            break;
    }

    mediaSink_->getFirstSelStream(manifest->getManifestIndex(),
                                  /*out*/firstSelTrackIndex,
                                  /*out*/firstSelStreamIndex,
                                  /*out*/firstSelStreamHeader);

    NTRACE(TRACE_MEDIASTARTUP, "load(#4), obtain the video stream header");
    PERFTRACE("load(#4), obtain the video stream header: time=%llu\n",
              Time::mono().val());

    // if we didn't get a stream header, it must have timed out
    if (!firstSelStreamHeader.get())
        return NFErr_NotReady;

    playbackReporter_->reportKeyStartplayEvent("firstheaderdled");

    return NFErr_OK;
}

NFErr AdaptiveStreamingPlayer::openPlaybackDevice( vector<IPlaybackDevice::StreamInfo> const& streamInfos,
                                                   ContentProfile lastVideoProfile )
{
    // For fast start, manifest 0 does not contain all the streams that
    // subsequent manifests will contain.  We should determine max width and
    // height according to the maxumimums possible in the video content profiles
    // reported by the device's ISystem:Capability. If this isn't fast start,
    // the playback device will get the max dimensions from the streamInfos.
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;

    // this should not be called. Max video size should come from downloadables, not device profiles
    //getDeviceMaxVideoDimensions(/*in*/streamInfos, /*out*/maxWidth, /*out*/maxHeight);

    NFErr err;
    if (!abort_)
    {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        NTRACE(TRACE_MEDIASTARTUP,
                "load(#5): before opening playback device, stream info size: %zu",
                streamInfos.size());
        PERFTRACE("load(#5): before opening playback device, time=%llu\n",
                  Time::mono().val());
        err = playbackDevice_->open(streamInfos,
                                    profileContainerType(lastVideoProfile),
                                    profileDrmType(lastVideoProfile),
                                    maxWidth, maxHeight);
        NTRACE(TRACE_MEDIASTARTUP,
                "load(#6): playback device opened");
        PERFTRACE("load(#6): playback device opened, time=%llu\n", Time::mono().val());
    }
    else
        err = NFErr_Aborted;

    if (err.ok())
    {
        deviceOpened_ = true;
        playbackReporter_->reportKeyStartplayEvent("deviceopened");

        playerState_->m_playbackDeviceOpen = true;

        return err;
    }

    err.push(NFErr_MC_OpenDeviceFailure);

    // If an error happened during loading, notify the user about the error
    // (if aborting is in progress, we should assume that the error is
    // caused by the abort operation, so it should not be reported to the
    // user).
    if (!abort_)
    {
        Log::error(TRACE_MEDIACONTROL, "Failed to open playback device.");
        reportError(err);
    }

    return err;
}

NFErr AdaptiveStreamingPlayer::initializePlayers()
{
    NFErr err = playbackDevice_->initializePlayers();

    if ( err.ok() )
    {
        deviceInitialized_ = true;

        if (newVolumePending_)
        {
            NTRACE(TRACE_MEDIACONTROL, "setting pending volume");
            playbackDevice_->setVolume(targetVolumePending_, transitionDurationPending_, easePending_);
            newVolumePending_ = false;
        }

        // set video window to requested or full if no pending request
        Rect actualRect;
        if (newVideoWindowPending_)
        {
            actualRect = playbackDevice_->setVideoWindow(
                videoWindowTargetRectPending_,
                videoWindowTransitionDurationPending_);

            newVideoWindowPending_ = false;
        }
        else
        {
            // Set the video window to full screen upon open.
            Rect rect = {0,0,0,0};
            actualRect = playbackDevice_->setVideoWindow(rect, 0);
        }
        eventThread_->postVideoWindowChangedEvent(actualRect);

        return err;
    }

    err.push(NFErr_MC_OpenDeviceFailure);

    // If an error happened during loading, notify the user about the error
    // (if aborting is in progress, we should assume that the error is
    // caused by the abort operation, so it should not be reported to the
    // user).
    if (!abort_)
    {
        Log::error(TRACE_MEDIACONTROL, "Failed to open playback device.");
        reportError(err);
    }

    return err;
}

NFErr AdaptiveStreamingPlayer::createBufferPool()
{
    if (!bufferManager_)
        return NFErr_MC_InitStreamFailure;

    mediaBufferPool_.reset(new McMediaBufferPool(bufferManager_));

    uint32_t videoMemorySize = streamingConfigParameter_->getParameterAs<uint32_t>
                                    ("videoMemorySize", -1);
    uint32_t videoVirtualMemorySize = streamingConfigParameter_->getParameterAs<uint32_t>
                                    ("videoVirtualMemorySize", -1);
    uint32_t audioMemorySize = streamingConfigParameter_->getParameterAs<uint32_t>
                                    ("audioMemorySize", -1);
    mediaBufferPool_->setBufferPoolSizes(videoMemorySize, videoVirtualMemorySize, audioMemorySize);

    AseErrorCode aseErr = mediaBufferPool_->open(BUFFER_POOL_UNIT_SIZE, 0);
    if (aseErr!= AS_NO_ERROR)
    {
        NFErr err(new ASError(aseErr));
        if (aseErr == AS_NOT_ENOUGH_MEMORY)
            return err.push(NFErr_MC_BufferAllocationFailure);
        else
            return err.push(NFErr_MC_InitStreamFailure);
    }

    {
        ScopedMutex lock(metaDataMutex_);
        videoBufferPoolSize_ = mediaBufferPool_->getBufferPoolSize(MEDIA_VIDEO);
        audioBufferPoolSize_ = mediaBufferPool_->getBufferPoolSize(MEDIA_AUDIO);
    }

    return NFErr_OK;
}

//================================================================================
// License thread procedures.
//================================================================================

NFErr AdaptiveStreamingPlayer::generateChallenge()
{
    // If user abort request is detected early at this point, we can skip getting
    // challenge data from the playback device.
    if (abort_)
    {
        Log::info(TRACE_MEDIACONTROL,
                  "abort detected in AdaptiveStreamingPlayer::generateChallenge()");
        return NFErr_MC_AcquireLicenseFailure;
    }

    licenseAcquireTime_ = Time::mono();

    NTRACE(TRACE_MEDIASTARTUP, "generateChallenge(#1): get challenge, time=%llu",
           Time::mono().val());

    // clear any existing challengeData before getting a new one
    challengeData_.clear();
    drmHeader_.clear();

    // Get challenge data from the playback device.
    NFErr err;
    {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        err = playbackDevice_->getChallenge(/*out*/challengeData_, /*out*/drmHeader_);
    }

    // If "NFErr_NotAllowed" is returned, the movie streams are not DRM protected.
    if (err == NFErr_NotAllowed)
    {
        NTRACE(TRACE_MEDIASTARTUP, "generateChallenge(##): not protected, time=%llu",
               Time::mono().val());

        // call the license retriever with NO_DRM to start any other services that
        // come after license acquisition
        // XXX - could break this out to a separate api to make it cleaner
        if (licenseRetriever_.get())
        {
            licenseRetriever_->provideChallenge(
                drmTypeToString(NO_DRM), string(), challengeData_, drmHeader_);
        }

        // for clear content, no license required
        playerState_->m_licenseAcquired = true;
        playerState_->m_licenseNeedClear = false;
    }
    else if (!err.ok())
    {
        Log::warn(TRACE_MEDIACONTROL, "getChallenge() returned %s",
                  err.toString().c_str());
    }

    return err;
}

NFErr AdaptiveStreamingPlayer::acquireLicense()
{
    // check for abort
    if (abort_)
    {
        Log::info(TRACE_MEDIACONTROL,
                  "abort detected in AdaptiveStreamingPlayer::acquireLicense()");
        return NFErr_Aborted;
    }

    NFErr err;

    // Acquire the movie license via NCCP, with the challenge data and the stream ID
    // (a.k.a. dlid) of the first video stream as input arguments.

    // If user abort is not requested at this point, do license acquisition.
    //
    // NOTE: the concern as stated for IAuthorizationManager::authorize() call
    //       is also applicable here.

    // The stream ID (a.k.a. dlid) of the first video stream is used to
    // acquire license as required by NCCP.

    NTRACE(TRACE_MEDIASTARTUP, "acquireLicense(#1): retrieve, time=%llu",
           Time::mono().val());

    // when acquiring license, we want to use the last manifest not current manifest
    // (current manifest is the nondrm manifest)
    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    {
        ScopedMutex lock(manifestMutex_);
        uint32_t manifestIndex = manifests_.size()-1;

        getManifest(/*out*/manifest, /*out*/mmd, manifestIndex);
    }

    // find an enabled stream
    string streamKey;
    uint32_t videoTrackIndex = mmd->videoTracksIndex_[mmd->currentVideoIndex_];
    const vector<shared_ptr<ManifestTrack> > &tracks = manifest->getTracks();
    shared_ptr<ManifestTrack> videoTrack = tracks[videoTrackIndex];
    const vector<shared_ptr<ManifestStream> > &videoStreams = videoTrack->getStreams();
    vector<shared_ptr<ManifestStream> >::const_iterator sit;
    for (sit = videoStreams.begin(); sit != videoStreams.end(); ++sit)
    {
        if ((*sit)->enabled())
        {
            streamKey = (*sit)->getStreamKey();
            break;
        }
    }

    // make sure we got a streamKey
    if (streamKey.empty())
    {
        Log::warn(TRACE_MEDIACONTROL, "Unable to find enabled stream in video track: %d",
                  videoTrackIndex);
        err = NFErr_Bad;
    }

    string downloadableId;
    ContentProfile profile = UNKNOWN_PROFILE;
    if (err.ok())
    {
        if (!ManifestUtilities::parseStreamKey(/*out*/downloadableId, /*out*/profile,
                                               streamKey))
        {
            Log::warn(TRACE_MEDIACONTROL, "Bad StreamKey in Manifest: %s",
                      streamKey.c_str());
            err = NFErr_Bad;
        }
    }

    if (err.ok())
    {
        DrmType drmType = profileDrmType(profile);
        if (licenseRetriever_.get())
        {
            err = licenseRetriever_->provideChallenge(
                drmTypeToString(drmType), downloadableId, challengeData_, drmHeader_);
        }

        NTRACE(TRACE_MEDIACONTROL, "provideChallenge returned %s",
               err.toString().c_str());
    }

    if (!err.ok())
    {
        Log::info(TRACE_MEDIACONTROL, "acquireLicense(#2) retriever returned %s",
               err.toString().c_str());

        // caller should call reportError
    }
    else
    {
        // done with the challenge, we can release it
        challengeData_.clear();
        drmHeader_.clear();
    }

    return err;
}

NFErr AdaptiveStreamingPlayer::provideLicense(const vector<unsigned char>& license,
                                              /*out*/vector<unsigned char>& secureStopId,
                                              /*out*/uint32_t& drmSessionId)
{
    // report license acquisition time to PlaybackReporter
    Time licenseTime = Time::mono() - licenseAcquireTime_;
    playbackReporter_->setLicenseTime(licenseTime.ms());

    // If the license acquisition is successful (regardless of whether user abort
    // is requested or not), store the license response into the playback device.

    NTRACE(TRACE_MEDIASTARTUP, "provideLicense: store license, time=%llu",
           Time::mono().val());

    NFErr err;
    {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        err = playbackDevice_->storeLicense(license, /*out*/secureStopId, /*out*/drmSessionId);
    }
    if (!err.ok())
    {
        err.push(NFErr_MC_OpenDeviceFailure); // consider this an open failure
        Log::error(TRACE_MEDIASTARTUP, "provideLicense: store license failed: %s",
                   err.toString().c_str());
        reportError(err);
        return err;
    }

    playbackReporter_->reportKeyStartplayEvent("licensestored");

    // use the manifest index of last manifest
    uint32_t manifestIndex = 0;
    {
        ScopedMutex lock(manifestMutex_);
        manifestIndex = manifests_.size()-1;
    }

    // notify streaming manager that license has been acquired
    {
        ScopedMutex scopedMutexResource(streamingManagerMutex_);
        if (streamingManager_.get())
            streamingManager_->notifyLicenseAcquired(manifestIndex);
    }

    NTRACE(TRACE_MEDIASTARTUP, "provideLicense: store success, time=%llu",
           Time::mono().val());
    playerState_->m_licenseAcquired = true;
    playerState_->m_licenseNeedClear = true;

    {
        ScopedMutex threadLock(threadMutex_);
        if (pumpingThread_.get())
            pumpingThread_->pinch();
    }

    return NFErr_OK;
}

void AdaptiveStreamingPlayer::clearLicense()
{
    ScopedMutex playbackLock(playbackDeviceMutex_);
    playbackDevice_->clearLicense();
}

AseErrorCode AdaptiveStreamingPlayer::startStreamingManager(int manifestIndex,
                                                            uint32_t pts)
{
    // get the manifest data
    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, manifestIndex);

    // Can't start the streaming manager if we don't have a current audio track
    if ( mmd->currentAudioIndex_ < 0 )
    {
        NTRACE(TRACE_MEDIACONTROL, "startStreamingManager: no audio track yet" );
        return AS_NO_ERROR;
    }

    // Start a reading session of the stream manager.
    vector<uint32_t> trackIndices;
    trackIndices.push_back(mmd->videoTracksIndex_[mmd->currentVideoIndex_]);
    trackIndices.push_back(mmd->audioTracksIndex_[mmd->currentAudioIndex_]);

    AseErrorCode retCode;
    retCode = streamingManager_->switchTracks(trackIndices);
    if (retCode != AS_NO_ERROR)
    {
        Log::error(TRACE_MEDIACONTROL,
                   "ASP unable to switch streaming manager tracks: %d", retCode);
        NFErr err(NFErr_MC_StreamingInitFailure);
        if(retCode == AS_MEDIA_DURATION_ERROR)
        {
            err.push(NFErr_MC_EncodingError);
            err.push(NFErr_MC_MediaDurationError);
        }
        reportError(err);
        return retCode;
    }

    // start streaming manager
    AseTimeStamp seekTime = AseTimeStamp(pts, 1000);
    retCode = streamingManager_->start(seekTime);
    if (retCode != AS_NO_ERROR)
    {
        Log::error(TRACE_MEDIACONTROL, "ASP unable to start streaming manager: %d",
                   retCode);
        NFErr err(NFErr_MC_StreamingInitFailure);
        if(retCode == AS_MEDIA_DURATION_ERROR)
        {
            err.push(NFErr_MC_EncodingError);
            err.push(NFErr_MC_MediaDurationError);
        }
        reportError(err);
    }
    return retCode;
}

NFErr AdaptiveStreamingPlayer::setPlaybackSpeed(uint32_t speed)
{
    ScopedMutex lock(playbackDeviceMutex_);

    PERFTRACE("MCQ2 - ASP::setPlaybackSpeed time=%llu\n", Time::mono().val());

    return playbackDevice_->setSpeed(speed);
}

bool AdaptiveStreamingPlayer::checkForSwitchAudioTrack()
{
    // If the requested audio track is different from the currently active audio
    // track, start the dynamic audio track switch procedure.
    //
    // Note: if the device does not have the capability to switch audio on the fly,
    //       (currentAudioIndex_ == activeAudioIndex_) would be guaranteed here.

    // get the manifest data
    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, currentManifestIndex_);
    if (mmd->currentAudioIndex_ == static_cast<int32_t>(mmd->activeAudioIndex_))
    {
        return false; // no switch necessary
    }

    ScopedMutex scopedMutexSwitch(audioTrackSwitchMutex_);

    playerState_->m_waitOnAudioSwitch = true;
    playerState_->m_audioEndOfStream = false;
    playerState_->m_suspendAudioDataFeed = true;

    {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        playbackDevice_->flushAudio();
        NTRACE(TRACE_MEDIACONTROL,
               "On-the-fly audio track switch: flush audio in playbackDevice_ "
               "completed time=%llu ms", Time::mono().val());
    }

    mmd->activeAudioIndex_ = mmd->currentAudioIndex_;

    std::vector<uint32_t> currentTrackIndexes;
    currentTrackIndexes.push_back(mmd->videoTracksIndex_[mmd->currentVideoIndex_]);
    currentTrackIndexes.push_back(mmd->audioTracksIndex_[mmd->currentAudioIndex_]);

    streamingManager_->switchTracks(currentTrackIndexes);

    return true; // tracks switched
}

void AdaptiveStreamingPlayer::getBlocks(AvPumpingContext &avPumpingContext)
{
    bool readPtsUpdated;

    // If audio stream hasn't reached EOS yet, and there is no audio data block
    // cached, try to read an audio data block from the stream manager.

    ScopedMutex scopedMutexSwitch(audioTrackSwitchMutex_);

    if (!playerState_->m_suspendAudioDataFeed &&
        (avPumpingContext.audioReadPTS_ != MAX_PTS) &&
        !avPumpingContext.currAudioBlock_.mValid)
    {
        readBlock(MEDIA_AUDIO,
                  avPumpingContext.currAudioBlock_,
                  avPumpingContext.audioReadPTS_,
                  readPtsUpdated);
        if (readPtsUpdated && avPumpingContext.audioReadPTS_ == MAX_PTS)
        {
            NTRACE(TRACE_MEDIACONTROL, "audioPTS to MAX_PTS");
        }
    }

    // If video stream hasn't reached EOS yet, and there is no video data block
    // cached, try to read a video data block from the stream manager.

    if (avPumpingContext.videoReadPTS_ != MAX_PTS &&
       !avPumpingContext.currVideoBlock_.mValid)
    {
        readBlock(MEDIA_VIDEO,
                  avPumpingContext.currVideoBlock_,
                  avPumpingContext.videoReadPTS_,
                  readPtsUpdated);
        if (readPtsUpdated && avPumpingContext.videoReadPTS_ == MAX_PTS)
        {
            NTRACE(TRACE_MEDIACONTROL, "videoPTS to MAX_PTS");
        }

        if (avPumpingContext.send1stVideoFrame_ && readPtsUpdated)
        {
            // If we are in force-sending-1st-video-frame mode, check whether the 1st
            // video frame is completely delivered. If so, go back to normal mode.
            // Also go back to normal mode if the video stream has reached EOS.

            if (avPumpingContext.firstVideoFramePTS_ == INVALID_PTS)
                avPumpingContext.firstVideoFramePTS_ = avPumpingContext.videoReadPTS_;

            if (avPumpingContext.videoReadPTS_ == MAX_PTS ||
                avPumpingContext.videoReadPTS_ != avPumpingContext.firstVideoFramePTS_)
            {
                avPumpingContext.send1stVideoFrame_ = false;
            }
        }
    }

    // check for end of streams
    if (avPumpingContext.videoReadPTS_ == MAX_PTS &&
        playerState_->m_videoEndOfStream == false)
    {
        playerState_->m_videoEndOfStream = true;

        // notify playback device we hit end of stream
        ScopedMutex playbackLock(playbackDeviceMutex_);
        NTRACE(TRACE_MEDIACONTROL, "AdaptiveStreamingPlayer::getBlocks(): "
               "notify playback device of video end of stream");
        playbackDevice_->endOfStream(IPlaybackDevice::VIDEO);
    }

    if (avPumpingContext.audioReadPTS_ == MAX_PTS &&
        playerState_->m_audioEndOfStream == false)
    {
        playerState_->m_audioEndOfStream = true;

        // notify playback device we hit end of stream
        ScopedMutex playbackLock(playbackDeviceMutex_);
        NTRACE(TRACE_MEDIACONTROL, "AdaptiveStreamingPlayer::getBlocks(): "
               "notify playback device of audio end of stream");
        playbackDevice_->endOfStream(IPlaybackDevice::AUDIO);
    }
}

void AdaptiveStreamingPlayer::readBlock(
    MediaType mediaType,
    MediaEntity& dataBlock,
    uint32_t& readPTS,
    bool& readPtsUpdated)
{
    NFErr err = mediaSink_->readBlock(mediaType, dataBlock);
    readPtsUpdated = false;

    if (err.ok())
    {
        if (dataBlock.mDataBlock.get())
        {
            readPtsUpdated = true;
            readPTS = dataBlock.mPtsInMS;
        }
    }
    else
    {
        // If we failed to read a new data block, it could be either the stream
        // manager is temporarily out-of-data, or the end-of-stream is reached.

        if (err == NFErr_EndOfStream)
        {
            // Set the read PTS to maximum so the other stream can be selected.

            readPTS = MAX_PTS;
            readPtsUpdated = true;
        }
        else
        {
            // The stream manager is temporarily out-of-data.
            assert(err == NFErr_Pending);
        }
    }
}

bool AdaptiveStreamingPlayer::feedHeaders()
{
    shared_ptr<std::vector<unsigned char> > pHeader;
    MediaType type;
    IPlaybackDevice::StreamType ipdStreamType;
    int32_t manifestIndex, streamIndex, trackIndex;
    NFErr err;

    while (1)
    {
        pHeader = mediaSink_->getNewStreamHeader(type, /*out*/manifestIndex,
                                                 /*out*/trackIndex, /*out*/streamIndex);
        if (pHeader.get() == NULL)
        {
            break;
        }
        else
        {
            type == MEDIA_VIDEO ?  ipdStreamType = IPlaybackDevice::VIDEO :
                ipdStreamType = IPlaybackDevice::AUDIO;

            shared_ptr<Manifest> manifest;
            shared_ptr<ManifestMetaData> mmd;
            getManifest(/*out*/manifest, /*out*/mmd, manifestIndex);

            streamIndex += mmd->firstStreamIndexInTrack_[trackIndex];
            if (!feedStreamHeader(ipdStreamType, manifestIndex, streamIndex, pHeader))
                return false;
        }
    }
    return true;
}

bool AdaptiveStreamingPlayer::feedStreamHeader(
    IPlaybackDevice::StreamType streamType,
    uint32_t manifestIndex,
    uint32_t streamIndex,
    shared_ptr<vector<unsigned char> > streamHeader)
{
    ScopedMutex playbackLock(playbackDeviceMutex_);

    NFErr err = playbackDevice_->feedStreamHeader(
        *streamHeader, streamIndex, manifestIndex, streamType);
    if (!err.ok())
    {
        err.push(NFErr_MC_DevicePlaybackError);
        reportError(err);
        return false;
    }
    return true;
}

NFErr AdaptiveStreamingPlayer::feedBlock(
    IPlaybackDevice::StreamType streamType, uint32_t streamIndex,
    MediaEntity *blockToDeliver)
{
    IBufferManager::BufferDataType bufferDataType =
        (streamType == IPlaybackDevice::AUDIO) ?
        IBufferManager::AUDIO : IBufferManager::VIDEO;

    // create a buffer from the data block
    IBufferManager::Buffer buffer(&(blockToDeliver->mDataBlock->inUse()));
    buffer.bufferDataType_ = bufferDataType;
    buffer.byteBuffer_ = const_cast<unsigned char*>(
        blockToDeliver->mDataBlock->getFilledSpaceStart());
    buffer.capacity_ = blockToDeliver->mDataBlock->getCapacity();
    buffer.dataSize_ = blockToDeliver->mDataBlock->getFilledSpaceSize();
    buffer.cdnId_ = blockToDeliver->mDataBlock->getCdnId();
    buffer.timestamp_ = blockToDeliver->mPtsInMS;
    buffer.allocSeq_ = blockToDeliver->mDataBlock->getSeq();
    buffer.flags_ = blockToDeliver->mStartOfFragment?
        IBufferManager::Buffer::START_OF_GOP : 0;
    buffer.private_ = blockToDeliver->mDataBlock->getHandle();

    // feed the buffer to the playback device
    ScopedMutex playbackLock(playbackDeviceMutex_);
    NFErr err = playbackDevice_->feedData(buffer, streamIndex,
                                          blockToDeliver->mManifestIndex, streamType);

    // report these errors
    if ((err == NFErr_Bad) || (err == NFErr_DRMFailed))
    {
        if (err == NFErr_DRMFailed)
            err.push(NFErr_MC_DeviceDecryptionError);
        else
            err.push(NFErr_MC_DevicePlaybackError);

        reportError(err);
    }

    return err;
}

void AdaptiveStreamingPlayer::flush(AvPumpingContext& avPumpingContext,
                                    uint32_t& currSpeed)
{
    // In order to simplify the memory allocator implementation, the order of data
    // memory buffers releasing should follow the guide-line of "first allocated
    // first released".

    // Flush the playback device first, since the data memory buffers that are already
    // delivered to the playback device are allocated the earliest.

    {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        playbackDevice_->flush();
    }

    // The current audio and video data blocks (if not yet delivered) should be reset
    // to release the memory they hold next, along with other AvPumpingContext members.
    avPumpingContext.reset();

    // Finally stop the stream manager, which will free all buffered data blocks in
    // order (from oldest to newest). Besides, the latest measured network bandwidth
    // records are collected from the stream manager.

    if (!playbackSkipping_)
    {
        pumpingThread_->unlock();
        streamingManager_->stop();
        // we need to flush media sink after Streaming Manager is stopped.
        // Otherwise some data may be pushed into media sink
        mediaSink_->flush();
        pumpingThread_->lock();
    }

    // The current speed setting must be reset to IPlaybackDevice::PAUSED_SPEED to
    // reflect the fact that playback device is automatically paused after flush.

    currSpeed = IPlaybackDevice::PAUSED_SPEED;
}

void AdaptiveStreamingPlayer::rebuffer(AvPumpingContext& /* avPumpingContext */,
                                       uint32_t& /* currSpeed */)
{
    NTRACE(TRACE_MEDIACONTROL, "ASPlayer::rebuffer");

    // Reset the "bufferingComplete_" flag since buffering is to be restarted.
    playerState_->m_bufferingComplete = false;
    // notify playback reporter
    playbackStarted_ = false;
    playbackReporter_->playbackStopped(NFErr_Rebuffer, AseTimeStamp(currentPts_, 1000));

    streamingManager_->rebuffer();

    // Start halting playback.
    haltPlayback(true);
}

void AdaptiveStreamingPlayer::endBuffering()
{
    NTRACE(TRACE_MEDIASTARTUP, "endBuffering(): time=%llu openandpause=%d", Time::mono().val(), (state_==PLAYING) ? 0:1);
    PERFTRACE("endBuffering(): time=%llu\n", Time::mono().val());
    playbackReporter_->reportKeyStartplayEvent("endbuffering");

    // Reset the "dataExhausted_" flag since we just finished buffering.
    dataExhausted_ = false;

    // Stop halting playback.
    haltPlayback(false);

    // check to see if we are in nonDrmPlayback
    bool faststart = (streamingType_ != NORMAL_PLAY);
    bool faststartManifest = faststart && (currentManifestIndex_ == 0);

    // Post buffering-complete notification to the user.
    if ( !suppressBufferingEvents_ )
    {
        eventThread_->postBufferingEvent(100);
        eventThread_->postBufferingCompleteEvent(faststartManifest);
    }
    suppressBufferingEvents_ = false;
}

//================================================================================
// Event handling procedures.
//================================================================================

void AdaptiveStreamingPlayer::audioPTSUpdate()
{
    assert(!playerState_->m_waitOnAudioSwitch);

    // Update the current presentation time position in the stream manager. The
    // stream manager will return the indices of the currently presented
    // audio/video streams, as well as the start PTS of the currently presented
    // audio/video GOP.

    uint32_t audioManifestIndex;
    uint32_t audioStreamIndex;
    uint32_t audioGopStartPTS;
    mediaSink_->updateAudioPlayPosition(/*out*/audioManifestIndex,
                                        /*out*/audioStreamIndex,
                                        /*out*/audioGopStartPTS,
                                        currentPts_);
    assert(audioStreamIndex != INVALID_STREAM_INDEX);

    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, audioManifestIndex);

    uint32_t audioTrack = mmd->audioTracksIndex_[mmd->activeAudioIndex_];

    // If the currently presented audio stream changes, call the listener functions
    // to notify of the newly presented stream.
    if ((audioManifestIndex != lastAudioManifestIndex_) ||
        (audioStreamIndex != mmd->audioPresStreamIndex_) ||
        (audioTrack != mmd->audioPresTrack_))
    {
        lastAudioManifestIndex_ = audioManifestIndex;

        // notify PlaybackReporter of stream change
        playbackReporter_->playbackStreamChange(audioManifestIndex, audioTrack,
                                                audioStreamIndex,
                                                AseTimeStamp(audioGopStartPTS, 1000));

        NTRACE(TRACE_MEDIACONTROL,
               "streamPresenting audio: manifest %d track %d stream %d at %d",
               audioManifestIndex, audioTrack, audioStreamIndex, audioGopStartPTS);

        eventThread_->postStreamPresentingEvent(audioManifestIndex, audioTrack,
                                                audioStreamIndex, audioGopStartPTS);
        mmd->audioPresStreamIndex_ = audioStreamIndex;
        mmd->audioPresTrack_ = audioTrack;
    }
}

void AdaptiveStreamingPlayer::videoPTSUpdate(ullong /*pts*/, ullong monotimeInMS)
{
    // Update the current presentation time position in the stream manager. The
    // stream manager will return the indices of the currently presented
    // audio/video streams, as well as the start PTS of the currently presented
    // audio/video GOP.

    uint32_t videoManifestIndex;
    uint32_t videoStreamIndex;
    uint32_t videoGopStartPTS;
    uint32_t videoGopBitrate;
    mediaSink_->updateVideoPlayPosition(/*out*/videoManifestIndex,
                                        /*out*/videoStreamIndex,
                                        /*out*/videoGopStartPTS,
                                        /*out*/videoGopBitrate,
                                        currentPts_);
    assert(videoStreamIndex != INVALID_STREAM_INDEX);

    if (reportedVideoBitrate_ != videoGopBitrate)
    {
        eventThread_->postUpdateVideoBitrateEvent(videoGopBitrate);
        reportedVideoBitrate_ = videoGopBitrate;
    }

    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, videoManifestIndex);

    uint32_t videoTrack = mmd->videoTracksIndex_[mmd->currentVideoIndex_];

    // are we swtiching video streams?
    if ((videoManifestIndex != lastVideoManifestIndex_) ||
        (videoStreamIndex != mmd->videoPresStreamIndex_))
    {
        lastVideoManifestIndex_ = videoManifestIndex;

        // If there was a valid video stream presented previously, the amount of
        // playtime between the last recorded PTS (used in playdata generation) and
        // the beginning of the currently presented video GOP should be reported to
        // the playdata persistor with the ID of the previously presented stream,
        // as well as adding it to the total play-time count.

        // If the last recorded PTS doesn't make sense,
        // just ignore this amount of playtime.

        // notify PlaybackReporter of stream change
        playbackReporter_->playbackStreamChange(videoManifestIndex, videoTrack,
                                                videoStreamIndex,
                                                AseTimeStamp(videoGopStartPTS, 1000));

        // update playback pts if on to next GOP
        if (videoGopStartPTS >= latestReportedPTS_)
        {
            NTRACE(TRACE_MEDIACONTROL, "Updating to next GOP %d", videoGopStartPTS);
            uint32_t vcdnid, acdnid;
            mediaSink_->getCurrentCdnId(vcdnid, acdnid);

            // update playback reporter
            playbackReporter_->updatePlaybackPts(AseTimeStamp(videoGopStartPTS, 1000),
                                                 vcdnid, acdnid);

            // update the listener to the gop start
            eventThread_->postUpdatePTSEvent(videoGopStartPTS, monotimeInMS);
        }

        // Reset the last recorded PTS reported to our listeners to the
        // beginning of the newly presented video stream.
        latestReportedPTS_ = videoGopStartPTS;

        NTRACE(TRACE_MEDIACONTROL,
               "streamPresenting video: manifest %d track %d stream %d at %d",
               videoManifestIndex, videoTrack, videoStreamIndex, videoGopStartPTS);

        eventThread_->postStreamPresentingEvent(videoManifestIndex, videoTrack,
                                                videoStreamIndex, videoGopStartPTS);

        mmd->videoPresStreamIndex_ = videoStreamIndex;
    }
}

void AdaptiveStreamingPlayer::updatePTS(ullong pts, ullong monotimeInMS)
{
    ScopedMutex scopedMutexSwitch(audioTrackSwitchMutex_);

    // The play-duration must be valid.
    assert(playDuration_ != INVALID_DURATION);

    // PTS should not be greater than the play-duration.
    if (pts > playDuration_)
        currentPts_ = playDuration_;
    else
        currentPts_ = static_cast<uint32_t>(pts);

    if (!playerState_->m_waitOnAudioSwitch)
    {
        audioPTSUpdate();
    }
    videoPTSUpdate(currentPts_, monotimeInMS);

    // If the current presentation time position makes sense (it should be greater
    // than the last recorded PTS used in playdata generation), report the elapsed
    // playtime (from the last recorded PTS to the current presentation time
    // position) to the playdata persistor, as well as adding it to the total
    // play-time count.
    if (currentPts_ > latestReportedPTS_)
    {
        // update playback reporter
        uint32_t vcdnid, acdnid;
        mediaSink_->getCurrentCdnId(vcdnid, acdnid);
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
        {
            // pause the streaming thread for low end CE between end buffering and
            // first pts update
            ScopedMutex scopedMutexResource(streamingManagerMutex_);
            if ( streamingManager_ )
                streamingManager_->setStreamingSpeed(1);
        }
        PERFTRACE("playback started: time=%llu\n", Time::mono().val());
    }

}

//================================================================================
// Helpers.
//================================================================================

void AdaptiveStreamingPlayer::getManifest(/*out*/shared_ptr<Manifest> &manifest,
                                          /*out*/shared_ptr<ManifestMetaData> &mmd,
                                          uint32_t manifestIndex) const
{
    ScopedMutex lock(manifestMutex_);
    manifest = manifests_[manifestIndex];
    mmd = manifestData_[manifestIndex];
}

static bool queryBool(shared_ptr<StreamingConfigParameter> scp, const char *key,
                      bool defaultValue = true)
{
    string param = scp->queryParameter(key);

    if (param.empty())
        return defaultValue;

    return ((param == "true") || (param == "1"));
}

void AdaptiveStreamingPlayer::setupStreamingConfigParams()
{
    ISystem::Capability scap = nrdApp()->getSystem()->getCapability();

    // set some streaming config params
    streamingConfigParameter_.reset(new StreamingConfigParameter);
    localConfigParameter_.reset(new LocalConfigParameter);
    localConfigParameter_->setParameter(
        "socketReceiveBufferSize", lexical_cast<string>(scap.tcpReceiveBufferSize));
    localConfigParameter_->setParameter("audioSocketReceiveBufferSize", "32768");

    size_t const maxNetworkMonitorStore = streamingConfigParameter_->getParameterAs<size_t>( "maxNetworkMonitorStore", 64, 65536, 4096 );

    persistentStore_->setMaximumSize( StreamingPersistentStore::DOMAIN_THROUGHPUT, maxNetworkMonitorStore );

    // check for any artificial delay we need to have
    string delayParam = streamingConfigParameter_->queryParameter("artificialDelay");
    if (!delayParam.empty())
    {
        int delay = lexical_cast<uint32_t>(delayParam);
        if (delay != 0)
        {
            usleep(delay*1000); // sleep that many milliseconds
        }
    }

    // check to see if we should kill secure stop
#if defined(NRDP_PLATFORM_X360) || defined(NRDP_PLATFORM_PSP2)
    bool useSecureStop = false;
#else
    bool useSecureStop = queryBool(streamingConfigParameter_, "useSecureStop", true);
#endif
    {
        NTRACE(TRACE_MEDIACONTROL, "useSecureStop = %s", useSecureStop ? "true" : "false");

        ScopedMutex playbackLock(playbackDeviceMutex_);

        DrmManager::instance()->enableSecureStop(useSecureStop);
    }

    // give params to MediaFileCache
    mediaFileCache_->configure(streamingConfigParameter_);
}

void AdaptiveStreamingPlayer::reportError(NFErr err)
{
    NTRACE(TRACE_MEDIACONTROL, "reportError %s", err.toString().c_str());

    // report playback failure
    if (playbackReporter_.get())
        playbackReporter_->playbackFailure(err);

    // set error in player state
    playerState_->m_error = true;

    eventThread_->postErrorEvent(err); // post error notification to the user.
    updateErrorCount(); // update error count
}


NFErr AdaptiveStreamingPlayer::processManifest(
    /*out*/shared_ptr<AdaptiveStreamingPlayer::ManifestMetaData> &mmd,
    shared_ptr<Manifest> manifest, netflix::Video3DType video3DType,
    const std::string &defaultAudioTrack)
{
    mmd.reset(new ManifestMetaData);

    const vector<ManifestTrackPtr>& tracks = manifest->getTracks();

    uint32_t trackIndex;
    bool validManifest = true;
    for (trackIndex = 0; trackIndex < tracks.size(); ++trackIndex)
    {
        ManifestTrackPtr track = tracks[trackIndex];
        if ( track->getStreams().empty() )
        {
            validManifest = false;
            break;
        }

        MediaType mediaType = track->getMediaType();
        switch(mediaType)
        {
        case MEDIA_AUDIO:
            mmd->audioTracksIndex_.push_back(trackIndex);

            // if this matches the passed in defaultAudioTrack OR
            // if the defaultAudioIndex hasn't been set already and this is a
            // native language track
            // set it as default
            if (!defaultAudioTrack.empty() &&
                 (track->getTrackKey() == defaultAudioTrack))
            {
                mmd->defaultAudioIndex_ = mmd->audioTracksIndex_.size() - 1;
                mmd->currentAudioIndex_ = mmd->defaultAudioIndex_;
            }
            else if ((mmd->defaultAudioIndex_ == -1) && track->isNativeLanguage())
            {
                mmd->defaultAudioIndex_ = mmd->audioTracksIndex_.size() - 1;
            }

            break;

        case MEDIA_VIDEO:
            mmd->videoTracksIndex_.push_back(trackIndex);

            if (mmd->currentVideoIndex_ == -1)
            {
                // remember the initial video track that matches the 3D type
                if ((track->isStereoVideo() && (video3DType == VIDEO_3D)) ||
                    (!track->isStereoVideo() && (video3DType == VIDEO_2D)))
                {
                    mmd->currentVideoIndex_ = trackIndex;
                }
            }

            break;

        default:
            break;
        }
    }

    if ( !validManifest )
        return NFErr_BadParameter;

    // if no default audio index has been selected, go with index 0
    if (mmd->defaultAudioIndex_ == -1)
    {
        Log::warn(TRACE_MEDIACONTROL, "No default audio track found, using 0");

        mmd->defaultAudioIndex_ = 0;
    }

    // must have a matching video track
    if (mmd->currentVideoIndex_ == -1)
    {
        Log::error(TRACE_MEDIACONTROL, "No matching video track found for %s",
                   video3DType ? "3D" : "2D");

        return NFErr_BadParameter;
    }

    // calculate the gap between cdn weights
    std::vector<ManifestServingLocationPtr> cdns(manifest->getServingLocations());
    int delta = 0;
    if (cdns.size() > 1)
    {
        uint32_t maxWeight = cdns[0]->getWeight();
        uint32_t minWeight = maxWeight;
        for ( uint32_t i = 1; i < cdns.size(); ++i )
        {
            uint32_t weight = cdns[i]->getWeight();
            minWeight = (weight < minWeight) ? weight : minWeight;
            maxWeight = (weight > maxWeight) ? weight : maxWeight;
        }

        delta = ((int)maxWeight - (int)minWeight) / (cdns.size() - 1);
    }
    // set the bandwidth gap into systemdata
    map<string, string> params = nrdApp()->serviceParameters("streaming");
    string oldGap = params["bandwidthGapBetweenDomains"], newGap = lexical_cast<string>(delta);
    if (oldGap != newGap) {
        params["bandwidthGapBetweenDomains"] = newGap;
        nrdApp()->systemData()->setServiceParameters("streaming", params);
    }

    return NFErr_OK;
}

void AdaptiveStreamingPlayer::calculateStreamIndices( uint32_t manifestIndex )
{
    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, manifestIndex);

    mmd->firstStreamIndexInTrack_.clear();

    uint32_t streamIndexOffset=0;
    const vector<ManifestTrackPtr>& tracks = manifest->getTracks();
    for (uint32_t track=0; track<tracks.size(); ++track)
    {
        // remember the offset of the first stream for this track
        mmd->firstStreamIndexInTrack_.push_back(streamIndexOffset);

        const vector<ManifestStreamPtr>& streams = tracks[track]->getStreams();
        streamIndexOffset += streams.size();
    }
}

ContentProfile AdaptiveStreamingPlayer::buildStreamInfos(
    /*out*/std::vector<IPlaybackDevice::StreamInfo> &streamInfos,
    uint32_t manifestIndex, uint32_t firstSelTrackIndex, uint32_t firstSelStreamIndex,
    shared_ptr<std::vector<unsigned char> > mediaHeader)
{
    ContentProfile lastVideoProfile = UNKNOWN_PROFILE;

    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, manifestIndex);

    // build up our streamInfo array
    const vector<ManifestTrackPtr>& tracks = manifest->getTracks();
    for (uint32_t track=0; track<tracks.size(); ++track)
    {
        const vector<ManifestStreamPtr>& streams = tracks[track]->getStreams();

        // we only care about AUDIO/VIDEO
        MediaType mediaType = tracks[track]->getMediaType();
        if ((mediaType != MEDIA_AUDIO) && (mediaType != MEDIA_VIDEO))
            continue;

        for (uint32_t stream=0; stream<streams.size(); ++stream)
        {
            IPlaybackDevice::StreamInfo streamInfo;

            if (track == firstSelTrackIndex && stream == firstSelStreamIndex)
            {
                streamInfo.headerData_ = mediaHeader.get();
                NTRACE(TRACE_MEDIACONTROL, "stream header data size: %zu",
                   streamInfo.headerData_->size());
            }
            else
            {
                streamInfo.headerData_ = NULL;
            }

            pair<int,int> videoSize = streams[stream]->getVideoSize();
            streamInfo.videoImageWidth_ = videoSize.first;
            streamInfo.videoImageHeight_ = videoSize.second;

            pair<int,int> par = streams[stream]->getPixelAspectRatio();
            streamInfo.videoPixelAspectRatioX_ = par.first;
            streamInfo.videoPixelAspectRatioY_ = par.second;

            streamInfo.nominalBitrateInKbps_ = streams[stream]->getNominalBitrate();
            //printf("StreamInfo at ASP nominalBitrate %d\n", streamInfo.nominalBitrateInKbps_);

            streamInfo.videoFrameRateValue_ = streams[stream]->getVideoFrameRate().first;
            streamInfo.videoFrameRateScale_ = streams[stream]->getVideoFrameRate().second;
/*
            printf("StreamInfo at ASP frameRate %d/%d\n",
                   streams[stream]->getVideoFrameRate().first,
                   streams[stream]->getVideoFrameRate().second);
*/


            string downloadableId;
            ContentProfile profile;
            if (ManifestUtilities::parseStreamKey(/*out*/downloadableId, /*out*/profile,
                                                  streams[stream]->getStreamKey()))
            {
                streamInfo.contentProfile_ = profile;
            }
            else
            {
                streamInfo.contentProfile_ = UNKNOWN_PROFILE;
            }

            switch (mediaType)
            {
                case MEDIA_AUDIO:
                    streamInfo.streamType_ = IPlaybackDevice::AUDIO;
                    break;

                case MEDIA_VIDEO:
                    streamInfo.streamType_ = IPlaybackDevice::VIDEO;

                    // remember the last video profile we saw
                    lastVideoProfile = profile;
                    break;

                default:
                    streamInfo.streamType_ = IPlaybackDevice::AUDIO;
                    assert(false); // unexpected
            }

            streamInfos.push_back(streamInfo);
        }
    }

    return lastVideoProfile;
}

shared_ptr<ManifestTrack> AdaptiveStreamingPlayer::getCurrentVideoTrack()
{
    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, currentManifestIndex_);

    // make sure we have a video track
    if (mmd->currentVideoIndex_ < 0)
    {
        Log::warn(TRACE_MEDIACONTROL,
                  "getCurrentVideoTrack() called with no video track selected");
        return shared_ptr<ManifestTrack>();
    }

    uint32_t trackIndex = mmd->videoTracksIndex_[mmd->currentVideoIndex_];
    const vector<ManifestTrackPtr> &tracks = manifest->getTracks();

    return tracks[trackIndex];
}

void AdaptiveStreamingPlayer::notifyTracksChanged()
{
    shared_ptr<Manifest> manifest;
    shared_ptr<ManifestMetaData> mmd;
    getManifest(/*out*/manifest, /*out*/mmd, currentManifestIndex_);
    const vector<shared_ptr<ManifestTrack> > &tracks = manifest->getTracks();
    shared_ptr<ManifestTrack> audioTrack = tracks[mmd->audioTracksIndex_[mmd->currentAudioIndex_]];
    eventThread_->postAudioTrackChangedEvent(mmd->currentAudioIndex_, audioTrack->getTrackKey());
}

void AdaptiveStreamingPlayer::updateErrorCount()
{
    ostringstream ostream;
    ostream.imbue(std::locale::classic());

    if (!storeError_ && playerState_->m_error)
    {
        storeError_ = true;
        string playbackHistory  = persistentStore_->loadValue(
            IPersistentStore::NUM_OF_CONSECUTIVE_PLAYBACK_FAILURES );

        uint32_t numPlaybackFailures = 0;
        if ( playbackHistory != "" )
            numPlaybackFailures = lexical_cast<uint32_t>(playbackHistory);
        ++numPlaybackFailures;

        ostream << numPlaybackFailures;
        persistentStore_->saveValue(
            IPersistentStore::NUM_OF_CONSECUTIVE_PLAYBACK_FAILURES, ostream.str() );
    }
    else if (!playerState_->m_error)
    {
        ostream << 0;
        persistentStore_->saveValue(
            IPersistentStore::NUM_OF_CONSECUTIVE_PLAYBACK_FAILURES, ostream.str() );
    }
}

void AdaptiveStreamingPlayer::audioTrackSwitchStarted(
                string const& /*oldLangCode*/, uint32_t /*oldNumChannels*/,
                string const& /*newLangCode*/, uint32_t /*newNumChannels*/)
{
    NTRACE(TRACE_MEDIACONTROL, "audioTrackSwitchStarted");
}

void AdaptiveStreamingPlayer::audioTrackSwitchComplete()
{
    ScopedMutex scopedMutexSwitch(audioTrackSwitchMutex_);

    playerState_->m_waitOnAudioSwitch = false;

    // Inform the bridge that the track switch is complete.
    notifyTracksChanged();

    shared_ptr<ManifestMetaData> mmd = manifestData_[currentManifestIndex_];
    NTRACE(TRACE_MEDIACONTROL, "On-the-fly audio track switch completed: "
           "time=%llu ms, new track id: %d", Time::mono().val(),
           mmd->audioTracksIndex_[mmd->currentAudioIndex_]);
}

void AdaptiveStreamingPlayer::flushComplete(MediaType mediaType)
{
    if (mediaType == MEDIA_AUDIO)
    {
        playerState_->m_suspendAudioDataFeed = false;
    }
}

void AdaptiveStreamingPlayer::setupMobileParameters()
{
    ScopedMutex lock(metaDataMutex_);
    streamingConfigParameter_->setParameter("maxBufferingTime", "4000");
    streamingConfigParameter_->setParameter("minPrebufSize", "6000");
    streamingConfigParameter_->setParameter("bandwidthMargin", "20");
    streamingConfigParameter_->setParameter(
            "initThroughputMeasureDataSize", "65536");
    streamingConfigParameter_->setParameter(
            "maxThroughputMeasureTime", "10000");
    streamingConfigParameter_->setParameter(
            "highStreamTransitionWindow", "1200000");
    streamingConfigParameter_->setParameter(
            "highStreamRetentionWindow", "180000");
    streamingConfigParameter_->setParameter("tcpConnectTimeout", "15000");
    streamingConfigParameter_->setParameter(
            "httpConnectionStallTimeout", "15000");
}

NFErr AdaptiveStreamingPlayer::updateConfigParameters()
{
    ISystem::Capability systemCapability = nrdApp()->getSystem()->getCapability();

    int maxVideoBufferSize = bufferManager_->getBufferPoolSize(IBufferManager::VIDEO);
    localConfigParameter_->setParameter(
            "totalBufferSize", lexical_cast<string>(maxVideoBufferSize));
    int maxAudioBufferSize = bufferManager_->getBufferPoolSize(IBufferManager::AUDIO);
    localConfigParameter_->setParameter(
            "totalAudioBufferSize", lexical_cast<string>(maxAudioBufferSize));

    if ( networkProfile_ == MOBILE )
        setupMobileParameters();

    // how to know a stream is 3D
    ManifestTrackPtr videoTrack = getCurrentVideoTrack();
    if (!videoTrack.get())
        return NFErr_Bad;

    // set up config parameters for standard devices
#if 0
    streamingConfigParameter_->setParameter("pauseStreamingAfterBC", "true");
    streamingConfigParameter_->setParameter("minSelectInterval", "20");
    streamingConfigParameter_->setParameter("maxSimuLength", "600000");
#endif

#if 0
    // example configure parameters
    streamingConfigParameter_->setParameter(
            "configedVideoBufferSize", "16388608");
    streamingConfigParameter_->setParameter(
            "configedAudioBufferSize", "2048576");
    streamingConfigParameter_->setParameter("throughputMeasurementWindowCurve", "[{\"w\" : 2000, \"b\" : 30000},{\"w\" : 300000, \"b\": 300000}]");
    streamingConfigParameter_->setParameter("minInitVideoBitrate", "1000");
    streamingConfigParameter_->setParameter("bandwidthMarginCurve", "[{\"m\" : 50, \"b\" : 15000},{\"m\" : 20, \"b\": 20000},{\"m\" : 0, \"b\": 30000}] ");
    streamingConfigParameter_->setParameter("initialBitrateSelectionCurve", "[{\"d\" : 1000, \"r\" : 6000},{\"d\" : 3000, \"r\": 1100},{\"d\" : 6000, \"r\": 600}] ");
#endif

#if 0
    // sample set up for nrdlib-3155
    streamingConfigParameter_->setParameter("minPrebufSize", "8000");
    streamingConfigParameter_->setParameter("maxBufferingTime", "4000");
    streamingConfigParameter_->setParameter("bandwidthMarginCurve", "[{\"m\" : 30, \"b\" : 20000},{\"m\" : 10, \"b\": 45000},{\"m\" : 0, \"b\": 120000}] ");
    streamingConfigParameter_->setParameter("enableGoodQualityBuffering", "true");
    streamingConfigParameter_->setParameter("lowestBufForUpSwitch", "15000");
#endif

    // This needs to be the last line
    streamingConfigParameter_->reloadSystemParameters();


    minimumDataInDevice_ = streamingConfigParameter_->getParameterAs<uint32_t>("minimumDataInDevice", 1000);

    return NFErr_OK;
}

void
AdaptiveStreamingPlayer::obtainSlowPumpingParas(uint32_t& slowPumpingEnterThreshold,
                                                uint32_t& slowPumpingExitThreshold,
                                                uint32_t& sleepDuringSlowPumping)
{
    slowPumpingEnterThreshold =
        streamingConfigParameter_->getParameterAs<uint32_t>("slowPumpingEnterThreshold", 200);
    slowPumpingExitThreshold =
        streamingConfigParameter_->getParameterAs<uint32_t>("slowPumpingExitThreshold", 2000);
    sleepDuringSlowPumping =
        streamingConfigParameter_->getParameterAs<uint32_t>("sleepDuringSlowPumping", 1000);

}

void AdaptiveStreamingPlayer::networkChange( NetworkInterfaceInfo const& netinfo )
{
    ScopedMutex scopedMutexResource(streamingManagerMutex_);

    if ( streamingManager_ )
        streamingManager_->networkChange( netinfo );
}

void AdaptiveStreamingPlayer::externalIpAddressChange( std::string const& ipaddr )
{
    ScopedMutex scopedMutexResource(streamingManagerMutex_);

    if ( streamingManager_ )
        streamingManager_->externalIpAddressChange( ipaddr );
}

Variant AdaptiveStreamingPlayer::getNetworkMonitorState()
{
    Variant state;

    ScopedMutex scopedMutexResource(streamingManagerMutex_);

    if ( streamingManager_ )
        streamingManager_->getNetworkMonitorState( &state );

    return state;
}

void AdaptiveStreamingPlayer::setNetworkMonitorState( Variant const& state )
{
    ScopedMutex scopedMutexResource(streamingManagerMutex_);

    if ( streamingManager_ )
        streamingManager_->setNetworkMonitorState( state );
}

void AdaptiveStreamingPlayer::setNetworkMonitorListener( shared_ptr<INetworkMonitorListener> pListener )
{
    ScopedMutex scopedMutexResource(streamingManagerMutex_);

    if ( streamingManager_ )
        streamingManager_->setNetworkMonitorListener( pListener );
}

NFErr AdaptiveStreamingPlayer::cachePrepare(
    shared_ptr<Manifest> manifest, netflix::Video3DType video3DType,
    const std::string &defaultAudioTrack, shared_ptr<IPlaybackReporter> playbackReporter)
{
    NTRACE(TRACE_MEDIACONTROL, "AdaptiveStreamingPlayer::cachePrepare videoType = %d",
           video3DType);
    NFErr err;

    mediaFileCache_->configure(streamingConfigParameter_);
    mediaFileCache_->setTransportReporter(playbackReporter);
    playbackReporter_ = playbackReporter;

    // process the manifest
    shared_ptr<ManifestMetaData> mmd;
    err = processManifest(/*out*/mmd, manifest, video3DType, defaultAudioTrack);
    if (!err.ok())
    {
        // report the error
        err.push(NFErr_MC_BadManifest);
        reportError(err);
        return err;
    }

    // get the audio/video track indexes
    std::vector<uint32_t> trackIndexes;
    trackIndexes.push_back(mmd->videoTracksIndex_[mmd->currentVideoIndex_]);
    trackIndexes.push_back(mmd->audioTracksIndex_[mmd->defaultAudioIndex_]);

    // create a streamingHeuristic
    shared_ptr<IStreamingHeuristic> streamingHeuristic;
    int32_t aserr = constructStreamingHeuristic(
        manifest, streamingConfigParameter_, localConfigParameter_, persistentStore_,
        static_pointer_cast<AdaptiveStreamingPlayer>(shared_from_this()),
        /*out*/streamingHeuristic);
    if (aserr != AS_NO_ERROR)
    {
        NTRACE(TRACE_MEDIACONTROL, "Unable to create streamingHeuristic: %d", aserr);
        return NFErr(new ASError(aserr));
    }

    err = mediaFileCache_->prepare(manifest, trackIndexes, streamingHeuristic);

    return err;
}

void AdaptiveStreamingPlayer::cacheFlush()
{
    NTRACE(TRACE_MEDIACONTROL, "AdaptiveStreamingPlayer::cacheFlush");
    mediaFileCache_->flush();
}

std::vector<std::string> AdaptiveStreamingPlayer::cacheList()
{
    NTRACE(TRACE_MEDIACONTROL, "AdaptiveStreamingPlayer::cacheList");
    return mediaFileCache_->getContentList();
}

void AdaptiveStreamingPlayer::cacheSetSize(int maxItems)
{
    NTRACE(TRACE_MEDIACONTROL, "AdaptiveStreamingPlayer::cacheSetSize %d", maxItems);
    mediaFileCache_->setCacheSize(maxItems);
}

double AdaptiveStreamingPlayer::getVolume()
{
    // if device is not open, assume that volume is 100% percent
    double volume = 1.0;
    if (!deviceInitialized_) {
        if (newVolumePending_) {
            volume = targetVolumePending_;
        }
        return volume;
    }

    if(playbackDevice_.get()){
        volume = playbackDevice_->getVolume();
    }
    NTRACE(TRACE_MEDIACONTROL, "AdaptiveStreamingPlayer::getVolume: %f", volume);
    return volume;
}

void AdaptiveStreamingPlayer::setVolume(double targetVolume, uint32_t transitionDuration, IAudioMixer::Ease ease)
{
    NTRACE(TRACE_MEDIACONTROL, "AdaptiveStreamingPlayer::setVolume targetVolume %f transitionDuration %d",
           targetVolume, transitionDuration);
    ScopedMutex scopedMutexState(stateMutex_);

    if ( targetVolume > 1.0 ) {
        targetVolume = 1.0;
    }

    if ( targetVolume < 0 ) {
        targetVolume = 0;
    }

    if (!deviceInitialized_) {
        targetVolumePending_ = targetVolume;
        transitionDurationPending_ = transitionDuration;
        easePending_ = ease;
        newVolumePending_ = true;
    } else {
        ScopedMutex playbackLock(playbackDeviceMutex_);
        playbackDevice_->setVolume(targetVolume, transitionDuration, ease);
    }
}
