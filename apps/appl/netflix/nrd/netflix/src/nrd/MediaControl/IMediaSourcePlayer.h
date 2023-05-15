/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IMEDIA_SOURCE_PLAYER_H
#define IMEDIA_SOURCE_PLAYER_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdbase/tr1.h>
#include <nrdbase/NFErr.h>
#include <nrd/Rect.h>
#include <nrd/Ticks.h>
#include <nrd/IPlaybackDevice.h>
#include <nrd/NrdpMediaSource.h>
#include "IMediaSourcePlaybackReporter.h"

/*
 * Not desirable, but only for IAdaptiveStreamingPlayer::State which is used too
 * widely throughout the codes. TODO: remove IAdaptiveStreamPlayer entirely for
 * JSASE
 */
#include <nrd/IAdaptiveStreamingPlayer.h>

namespace netflix{

namespace mediacontrol{

class IMediaSourcePlayer
{
public:
    /**
     * Playback state.  It is desirable to use IMediaSourcePlayer::State, but
     * IAdaptiveStreamingPlayer::State is used too widely, and it requires so
     * many changes. So, for now, use IAdaptiveStreamingPlayer::State throughout
     * the codes
     */
    /*
    enum State {
        OPENING,
        PLAYING,
        PAUSED,
        STOPPED,
        CLOSED
    };
    */

   /**
     * @class Listener IAdaptiveStreamingPlayer.h
     * @brief Media control listeners are notified of changes and
     *        errors in playback.
     */
    class Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}

        /**
         * Notified when the media control state is changed.
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] state the new media control state.
         */
        virtual void stateChanged(ullong sessionId, IAdaptiveStreamingPlayer::State state) = 0;

        /**
         * Notified when media loading (opening) is completed.
         * @param[in] sessionId the session for which the event pertains to
         */
        virtual void openComplete(ullong sessionId) = 0;

        /**
         * Notified when end-of-stream is reached.
         * @param[in] sessionId the session for which the event pertains to
         */
        virtual void endOfStream(ullong sessionId) = 0;

        /**
         * Notified when a playback data underflow occurs.
         * @param[in] sessionId the session for which the event pertains to
         */
        virtual void underflow(ullong sessionId) = 0;

        /**
         * Notified when a playback has skipped within the buffer
         * @param[in] sessionId the session for which the event pertains to
         */
        virtual void skipped(ullong sessionId) = 0;

        /**
         * Notified when the playback time position is updated.
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] pts the new playback time position relative to the
         *            beginning of movie.
         * @param[in] monotimeInMS the timestamp of pts event get generated
         */
        virtual void updatePts(ullong sessionId,
                               const Ticks& pts,
                               const ullong& monotimeInMS) = 0;

        /**
         * Notified when the current video window size or location is changed.
         * @param[in] sessionId the session for which the event pertains to
         */
        virtual void videoWindowChanged(ullong sessionId,
                                        const Rect& actualRect) = 0;

        /**
         * Notified when the audio track change started
         */
        virtual void audioTrackChangeStarted(ullong sessionId) = 0;

        /**
         * Notified when the audio track change completed
         */
        virtual void audioTrackChangeComplete(ullong sessionId) = 0;

        /**
         * Notified when the audio track change is rejected
         */
        virtual void audioTrackChangeRejected(ullong sessionId) = 0;

        /**
         * Notified when an error condition happens.
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] error the error code.
         */
        virtual void error(ullong sessionId, const NFErr &error) = 0;

    };

    virtual ~IMediaSourcePlayer() {}

    /**
     * Shutdown any threads the player may be using
     */
    virtual void shutdown() = 0;

    /*
     * ***********
     * MediaSource
     * ***********
     */
    virtual void setMediaSource(shared_ptr<NrdpMediaSource>) = 0;

    /*
     * *************
     * Event listner
     * *************
     */

    /**
     * Add a listener to the media control.
     *
     * @param[in] l the listener.
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr addListener(shared_ptr<Listener> l) = 0;

    /**
     * Remove a listener from the media control.
     *
     * @param[in] l the listener.
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr removeListener(shared_ptr<Listener> l) = 0;

    /*
     * *************
     * Informational
     * *************
     */

    /**
     * obtain the availlable playback buffer range the player can skip without
     * rebuffering.
     *
     * @param[out] startPts start pts of the available buffer
     * @param[out] endPts end pts of the available buffer
     */
    virtual void getBufferRange(Ticks& startPts, Ticks& endPts) = 0;


    /**
     * obtain the video and audio media buffer pool size
     * @param [out] videoBufferPoolSize
     * @param [out] audioBufferPoolSize
     */
    virtual void getMediaBufferPoolSize(uint32_t& videoBufferPoolSize,
                                        uint32_t& audioBufferPoolSize) = 0;

    /**
     * Returns playback quality statistics
     */
    virtual void getPlaybackQualityStats(device::IPlaybackDevice::PlaybackQualityStats& audioStats,
                                         device::IPlaybackDevice::PlaybackQualityStats& videoStats) = 0;


    /*
     * ****************
     * Playback control
     * ****************
     */

    /**
     * Start opening the device and initializing all playback context.
     * When this function returns, the new state will be OPENING which
     * means that the open operation is in progress.
     *
     * When the open operation is completed, the state will become STOPPED
     * and all registered listeners will receive a Listener::openComplete()
     * call.
     *
     * Each invocation of this method should have a corresponding call
     * to close(). While the open operation is in progress, close() is the
     * only function that can be done successfully.
     *
     *
     * movieId : For Nova team to identify the content and get content resources
     *           via dpi - this is temporary hack
     */

    // sekwon TODO : consider providing moov header data for early device initialization
    virtual NFErr open(const device::IPlaybackDevice::CommonVideoAttributesFromManifest &videoAttributes,
                       const device::IPlaybackDevice::CommonAudioAttributesFromManifest &audioAttributes,
                       shared_ptr<IMediaSourcePlaybackReporter> playbackReporter,
                       ullong sessionId,
                       uint32_t videoPipelineId,
                       const std::string movieId) = 0;

    /**
     * Begin playback. When this function returns, the new state will
     * be PLAYING.
     *
     * Each invocation of this method should have a corresponding call
     * to stop().
     *
     * @param[in] pts location to start playback from, if invalid then
     *            playback will start from the beginning.
     * @param[in] licenseHandler Object to handle license challenge
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr play(const Ticks& pts, bool playAndPause = false) = 0;

    /**
     * Stop playback. This flushes the device but keeps any existing
     * playback context created by open(). When this function returns,
     * the new state will be STOPPED.
     *
     * After stopping, play(const Ticks& )
     * may be called to initiate playback from a different PTS.
     *
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr stop() = 0;

    /**
     * Pause playback. When this function returns, the new state will be
     * PAUSED.
     *
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr pause() = 0;

    /**
     * Skip the playback to the provided pts. The player needs to be in PAUSED
     * when the function being called. When this function returns the state will
     * be PAUSED;
     *
     * @param[in] pts location to skip to.
     * @return NFErr_OK on success
     *         NFErr_NotAllowed if the player is not in PAUSED state or the provided
     *         pts is not in the available buffer range
     */
    virtual NFErr skip(const Ticks& pts) = 0;

    /**
     * Unpause playback. When this function returns, the new state will be
     * PLAYING.
     *
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr unpause() = 0;

    /**
     * Close the device and clean up all playback context. When this
     * function returns, the new state will be CLOSED.
     *
     * If the media control is in PLAYING or PAUSED state, it will be stopped
     * first before getting closed.
     *
     * After closing, open(uint32_t, shared_ptr<INccpHandler>) may be called
     * to open the device for a different content.
     */
    virtual void close() = 0;

    /**
     * Called by JS to notify that audio switching should happen and flush audio
     * data and disable audio
     */
    virtual void disableAudio() = 0;

    /*
     * Called by JS to notify that new audio track buffering is done and ok to
     * enable audio for new audio track
     */
    virtual void enableAudio() = 0;

    /**
     * Returns the current PTS (the current playback time position relative
     * to the beginning of movie). Accuracy is not guaranteed.
     *
     * The current PTS is invalid while the playback state is in CLOSED or
     * OPENING. An invalid Ticks will be returned in such case.
     *
     * If the playback state is in STOPPED, the current PTS will point at the
     * latest time position right before playback is stopped. If there is no
     * playback activity since open, the current PTS will be invalid.
     *
     * @return the current PTS.
     */
    virtual Ticks getCurrentPts() const = 0;

    /**
     * Returns the playback state.
     *
     * @return the playback state.
     */
    virtual IAdaptiveStreamingPlayer::State getState() const = 0;


    /*
     * ********************
     * Presentation control
     * ********************
     */

    /**
     * Set the video window location.
     *
     * Without this function being called, the default behavior is playing video
     * in full-screen.
     *
     * To restore full-screen playback, the caller should specify (0,0,0,0)
     * as the target rectangle.
     *
     * This function can be called at any time, but the action to configure the
     * underlying device may be deferred if the device is not currently opened.
     * Once the underlying device is re-configured, a Listener event will be
     * posted to notify about the newly effective video window location.
     *
     * The actual rectangle (as attached in the Listener event) could be
     * different from the target rectangle specified, depending on the
     * limitations that the underlying device might have.
     *
     * The (x,y,width,height) values of the rectangles are specified in pixels,
     * and the (x,y) coordinates of the rectangles are relative to the
     * left-upper corner of the screen.
     *
     * A non-zero duration of transition may be specified to request transition
     * animation from the old video window location to the new video window
     * location (for example, zooming in or out). Depends on the underlying
     * device's capability, certain number of 'intermediate' video window
     * locations will be inserted to smooth out the transition.
     *
     * @param[in] targetRect the target rectangle for the new video window, or
     *            (0,0,0,0) if full-screen playback is requested.
     * @param[in] transitionDuration the duration (in milliseconds) of the
     *            transition from the previous video window to the new target
     *            video window. 0 means the video window should be relocated to
     *            the target as soon as possible.
     */
    virtual void setVideoWindow(Rect targetRect, uint32_t transitionDuration, uint32_t zOrder) = 0;

    /**
     * getVolume
     */
    virtual double getVolume() = 0;

    /**
     * setVolume
     */
    virtual void setVolume(double targetVolume,
                           uint32_t transitionDuration,
                           device::IAudioMixer::Ease ease) = 0;

};

} // namespace mediacontrol
} // namespace netflix

#endif
