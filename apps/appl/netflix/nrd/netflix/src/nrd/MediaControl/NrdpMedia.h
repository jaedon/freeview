/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef NRDP_MEDIA_SOURCE_PLAYER_H
#define NRDP_MEDIA_SOURCE_PLAYER_H

#include "NrdpMediaEventThread.h"


#include <nrd/IMediaSourcePlayer.h>
#include <nrd/MediaSourcePlaybackDevice.h>
#include "IMediaSourcePlaybackReporter.h"
#include <nrd/Rect.h>

/*
 * Not desirable, but only for IAdaptiveStreamingPlayer::State which is used too
 * widely throughout the codes. TODO: remove IAdaptiveStreamPlayer entirely for
 * JSASE
 */
#include <nrd/IAdaptiveStreamingPlayer.h>

namespace netflix{

namespace mediacontrol{
class MediaSourceEventThread;
class NrdpMediaSource;

class NrdpMediaSourcePlayer : public IMediaSourcePlayer
{
public:
    NrdpMediaSourcePlayer();
    virtual ~NrdpMediaSourcePlayer();

    /*
     * ***********
     * MediaSource
     * ***********
     */
    void setMediaSource(shared_ptr<NrdpMediaSource>);

    /*
     * ****************************
     * IMediaSourcePlayer interface
     * ****************************
     */
    virtual void shutdown();

    /*
     * *************
     * Event listner
     * *************
     */
    virtual NFErr addListener(shared_ptr<Listener> l);

    virtual NFErr removeListener(shared_ptr<Listener> l);

    /*
     * *************
     * Informational
     * *************
     */
    virtual void getBufferRange(Ticks& startPts, Ticks& endPts);

    virtual void getMediaBufferPoolSize(uint32_t& videoBufferPoolSize,
                                        uint32_t& audioBufferPoolSize);

    virtual void getPlaybackQualityStats(device::IPlaybackDevice::PlaybackQualityStats& audioStats,
                                         device::IPlaybackDevice::PlaybackQualityStats& videoStats);

    /*
     * ****************
     * Playback control
     * ****************
     */
    virtual NFErr open(const device::IPlaybackDevice::CommonVideoAttributesFromManifest &videoAttributes,
                       const device::IPlaybackDevice::CommonAudioAttributesFromManifest &audioAttributes,
                       shared_ptr<IMediaSourcePlaybackReporter> playbackReporter,
                       ullong sessionId,
                       uint32_t videoPipelineId,
                       const std::string movieId);

    virtual NFErr play(const Ticks& pts, bool playAndPause = false);

    virtual NFErr skip(const Ticks& pts);

    virtual NFErr stop();

    virtual NFErr pause();

    virtual NFErr unpause();

    virtual void close();

    virtual void disableAudio();

    virtual void enableAudio();

    virtual Ticks getCurrentPts() const;

    virtual IAdaptiveStreamingPlayer::State getState() const;

    /*
     * ********************
     * Presentation control
     * ********************
     */
    virtual void setVideoWindow(Rect targetRect, uint32_t transitionDuration, uint32_t zOrder = 0);

    virtual double getVolume();

    virtual void setVolume(double targetVolume,
                           uint32_t transitionDuration,
                           device::IAudioMixer::Ease ease);


    /**
     * report an error to threads, listeners and playback reporter
     */
    void reportError(NFErr err);

private:
    /*
     * internal method
     */
     /**
     * Update the current presentation time position according to the received
     * PTS events from the playback device.
     *
     * @param[in] pts the current presentation timestamp in milliseconds.
     * @param[in] monotime the time that pts is posted via event
     */
    void updatePTS(ullong pts, ullong monotime);

    NFErr setSpeed(int32_t speed);

private:
    volatile IAdaptiveStreamingPlayer::State state_;
    volatile bool abortOpen_;
    volatile bool deviceOpened_;
    volatile bool playbackStarted_;
    volatile bool dataExhausted_;
    volatile bool endOfPresentation_;
    volatile uint32_t currentPts_;
    volatile uint32_t latestReportedPTS_;
    volatile bool abortSetSpeed_;
    volatile bool audioSwitchingInProgress_;

    NFErr deviceErrorReported;

    shared_ptr<IMediaSourcePlaybackReporter>        playbackReporter_;
    shared_ptr<NrdpMediaSource>                     mediaSource_;
    shared_ptr<device::MediaSourcePlaybackDevice>   playbackDevice_;
    shared_ptr<device::IBufferManager>              bufferManager_;
    shared_ptr<NrdpMediaEventThread>                eventThread_;

    /*
     * presentation control state
     */
    enum {
        VIDEO_AT_FRONT,
        VIDEO_AT_BACK
    } videoZOrderPending_;

    volatile bool     newVideoWindowPending_;
    volatile bool     newVolumePending_;
    volatile double   targetVolumePending_;
    volatile uint32_t transitionDurationPending_;
    volatile device::IAudioMixer::Ease easePending_;
    /*volatile*/ Rect videoWindowTargetRectPending_;
    volatile uint32_t videoWindowTransitionDurationPending_;
    volatile uint32_t videoWindowZOrderPending_;
    mutable Mutex controlMutex_;
    mutable Mutex stateMutex_;
    mutable Mutex nrdpVideoMutex_;
    mutable Mutex playbackDeviceMutex_;
    mutable Mutex audioTrackSwitchMutex_;

    int32_t mPipelineId;

    friend class NrdpMediaEventThread;
};
}
}

#endif
