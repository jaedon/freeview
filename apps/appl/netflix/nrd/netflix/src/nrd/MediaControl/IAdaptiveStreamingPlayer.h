/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef IADAPTIVESTREAMINGPLAYER_H
#define IADAPTIVESTREAMINGPLAYER_H

#include <vector>

#include <nrdbase/tr1.h>

#include <nrdbase/NFErr.h>

#include <nrdase/BitrateRange.h>
#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/AseBuffer.h>

#include "common.h"

#include <nrd/config.h>
#include <nrd/Rect.h>
#include <nrd/Ticks.h>
#include <nrd/IPlaybackDevice.h>

namespace netflix {

namespace ase {
class IAseConfigParameter;
class INetworkMonitorListener;
class Manifest;
class ManifestTrack;
class IStreamingHeuristic;
class AseBuffer;
}

struct NetworkInterfaceInfo;


namespace mediacontrol {

class IPlaybackReporter;

/**
 * @class AdaptiveStreamingPlayer AdaptiveStreamingPlayer.h
 * @brief Player class, independent of NCCP
 */
class IAdaptiveStreamingPlayer : public enable_shared_from_this<IAdaptiveStreamingPlayer>
{
public:
    /** Playback state. */
    enum State {
        OPENING,
        PLAYING,
        PAUSED,
        STOPPED,
        CLOSED
    };

    /** Network Profile. */
    enum NetworkProfile {
        WIRED,
        WIFI,
        MOBILE
    };

    struct MediaEntity
    {
        bool mValid;
        shared_ptr<std::vector<unsigned char> > mNewStreamHeader;
        shared_ptr<ase::AseBuffer> mDataBlock;
        std::map<std::string, uint32_t> mStreamAttributes;
        uint32_t mStreamId;
        // TODO: remove mManifestIndex, mTrackIndex and mStreamIndex
        uint32_t mManifestIndex;
        uint32_t mTrackIndex;
        uint32_t mStreamIndex;


        bool mStartOfFragment;
        bool mEndOfTrack;
        ullong mPtsInMS;

        MediaEntity() :
            mValid(false)
            , mNewStreamHeader(shared_ptr<std::vector<unsigned char> >())
            , mDataBlock(shared_ptr<ase::AseBuffer>())
            , mStreamId(0)
            , mManifestIndex(0)
            , mTrackIndex(0)
            , mStreamIndex(0)
            , mStartOfFragment(false)
            , mEndOfTrack(false)
            , mPtsInMS(0)
        {
        }

        void reset() { mValid = false;
                       mNewStreamHeader.reset();
                       mStreamAttributes.clear();
                       mDataBlock.reset(); }
    };

    struct AvPumpingContext
    {
        AvPumpingContext() { reset(); }

        void reset()
        {
            if(currAudioBlock_.mDataBlock.get())
            {
                currAudioBlock_.mDataBlock->inUse() = false;
            }
            if(currVideoBlock_.mDataBlock.get())
            {
                currVideoBlock_.mDataBlock->inUse() = false;
            }
            audioReadPTS_ = 0;
            videoReadPTS_ = 0;
            currAudioBlock_.reset();
            currVideoBlock_.reset();
            send1stVideoFrame_ = true;
            firstVideoFramePTS_ = INVALID_PTS;
        }

        uint32_t audioReadPTS_; // PTS of the latest read audio data block.
        uint32_t videoReadPTS_; // PTS of the latest read video data block.
        MediaEntity currAudioBlock_; // the audio data block to be delivered.
        MediaEntity currVideoBlock_; // the video data block to be delivered.
        // TODO: remove send1stVideoFrame_ and firstVideoFramePTS_
        bool send1stVideoFrame_; // true if in force-sending-1st-video-frame mode.
        uint32_t firstVideoFramePTS_;
    };


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
        virtual void stateChanged(ullong sessionId, State state) = 0;

        /**
         * Notified when media loading (opening) is completed.
         * @param[in] sessionId the session for which the event pertains to
         */
        virtual void openComplete(ullong sessionId) = 0;

        /**
         * Notified when a new stream gets selected.
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] manifest the index of the manifest whose stream has changed.
         * @param[in] track the index of the track whose stream has changed.
         * @param[in] stream the index of the selected stream.
         * @param[in] time the media time where the selected stream becomes effective.
         * @param[in] rebuffer prediction of number of rebuffering events.
         */
        virtual void streamSelected(ullong     sessionId,
                                    int          manifest,
                                    int          track,
                                    int          stream,
                                    const Ticks& time,
                                    uint32_t     rebuffer) = 0;

        /**
         * Notified when buffering the stream.
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] percentage the progress of buffering.
         */
        virtual void buffering(ullong sessionId, uint32_t percentage) = 0;


        /**
         * Notified when buffering is completed.
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] faststart true if the playback was for the faststart manifest
         */
        virtual void bufferingComplete(ullong sessionId, bool faststart) = 0;

        /**
         * Notified when the selected stream starts presenting.
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] manifest the index of the manifest whose stream has changed.
         * @param[in] track the index of the track whose stream has changed.
         * @param[in] stream the index of the selected stream.
         * @param[in] time the media time where the selected stream becomes effective.
         */
        virtual void streamPresenting(ullong     sessionId,
                                      int          manifest,
                                      int          track,
                                      int          stream,
                                      const Ticks& time) = 0;

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
        virtual void updatePts(ullong sessionId, const Ticks& pts,
                               const ullong& monotimeInMS) = 0;

        /**
         * Notified when the content duration is updated from the player
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] duration the new duration of the content
         */
        virtual void updateContentDuration(ullong sessionId, uint32_t duration) = 0;

        /**
         * Notified the measured bit-rate of the video that is currently
         * presented. This notification will be sent periodically.
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] bitsPerSecond the measured video bit-rate.
         */
        virtual void updateVideoBitrate(ullong sessionId, uint32_t bitsPerSecond) = 0;

        /**
         * Notified when the currently selected tracks are changed.
         *
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] trackId the new active audio track ID.
         */
        virtual void audioTrackChanged(ullong sessionId, uint32_t trackIndex,
                                       const std::string &trackId) = 0;

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
         * Notified when the video bitrate selection completed
         * @param[in] sessionId the session for which the event pertains to
         */
        virtual void videoBitrateSelectionCompleted(ullong sessionId) = 0;

        /**
         * Notified when the current video window size or location is changed.
         * @param[in] sessionId the session for which the event pertains to
         */
        virtual void videoWindowChanged(ullong sessionId,
                                        const Rect& actualRect) = 0;

        /**
         * Notified when an error condition happens.
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] error the error code.
         */
        virtual void error(ullong sessionId, const NFErr &error) = 0;

        /**
         * Notified when a fastplay streaming failure occurs. Listener should restart
         * playback in standard play mode
         * @param[in] sessionId the session for which the event pertains to
         * @param[in] error the original error that occurred
         * @param[in] pts Timestamp to restart playback at
         */
        virtual void restartPlayback(ullong sessionId, const NFErr &error,
                                     const Ticks &pts) = 0;

        /**
         * Notified when the streaming engine begins downloading from a new manifest
         * @parm[in] manifestIndex the index of the manifest switched to
         */
        virtual void manifestSelected(ullong sessionId, int manifestIndex) = 0;

        /**
         * Called when the hevc demultiplexer has debug information
         */

        virtual void hevcInfo(ullong sessionId, const std::string &info) = 0;
    };

    class LicenseHandler
    {
    public:
        virtual ~LicenseHandler() {}

        /* Provide the license challenge
         * TODEPRECATE
         * @param[in] drmType The DRM type
         * @param[in] challenge The DRM challenge
         * @param[in] drmHeader The DRM header parsed from the media
         * @return NFErr_OK for success
         */
        virtual NFErr provideChallenge(std::string const& drmType,
                                       std::string const& streamID,
                                       const std::vector<unsigned char>& challenge,
                                       const std::vector<unsigned char>& drmHeader) = 0;

        /**
         * New API for NrdpVideo
         */
        virtual NFErr provideChallenge(const std::vector<unsigned char>& challenge) = 0;

        /* Provide the license release (secure stop)
         * @param[in] drmType The DRM type
         * @param[in] release The DRM secure stop message
         */
        virtual void provideRelease(const std::string& drmType,
                                    const std::string& release) = 0;

        /**
         * Notifies LicenseHandler that the player has completed initial buffering
         * and bandwidth is available for authorization (if in faststart mode)
         */
        virtual void notifyBufferingComplete() = 0;

        /**
         * Checks to see if any errors have occured in the LicenseHandler that should
         * cause the player to stop. The player's close() should still be called
         * but this will allow early termination of data pumping
         * @returns NFErr_OK if no issues
         */
        virtual NFErr checkStatus() = 0;
    };

    virtual ~IAdaptiveStreamingPlayer() {}

    /**
     * Shutdown any threads the player may be using
     */
    virtual void shutdown() = 0;

    /** Gets the current network profile, which decides the streaming parameters
     * @return the current network profile
     */
    virtual NetworkProfile getNetworkProfile() const = 0;

    /** Set the network profile, which decides the streaming parameters
     * @param[in] nwProfile network profile
     */
    virtual void setNetworkProfile(NetworkProfile nwProfile) = 0;

    /**
     * Returns detailed streaming statistics
     */
    virtual shared_ptr<ase::StreamingStatReport> obtainStreamingStat( ) = 0;

    /**
     * Returns playback quality statistics
     */
    virtual void getPlaybackQualityStats(device::IPlaybackDevice::PlaybackQualityStats& audioStats,
                                         device::IPlaybackDevice::PlaybackQualityStats& videoStats) = 0;

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
     * As an alternative , the caller can start a playback session with a manifest
     * containing DRM free content for faster playback.
     * The caller should then call addManifest later to provide the manifest for the
     * DRM streans
     *
     * @param[in] pManifest the manifest to play.
     * @param[in] playbackReporter the playback reporter to send statistics to
     * @param[in] licenseHandler the handler for license requests
     * @param[in] video3DType ether 2D or 3D video
     *            (FIXME: should we put video3DType in Manifest?)
     * @param[in] defaultAudioTrack the default audio track
     * @param[in] sessionId the sessionID for this playback. Sent on listener events
     * @param[in] streamingType indicates the playback type
     * @param[in] pts The playback start PTS, if known
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr open(shared_ptr<ase::Manifest> pManifest,
                       shared_ptr<IPlaybackReporter> playbackReporter,
                       shared_ptr<LicenseHandler> licenseHandler, ullong sessionId,
                       netflix::Video3DType video3DType,
                       const std::string &defaultAudioTrack,
                       netflix::StreamingType streamingType,
                       ContainerType containerType,
                       const device::IPlaybackDevice::CommonVideoAttributesFromManifest &videoAttributes,
                       const device::IPlaybackDevice::CommonAudioAttributesFromManifest &audioAttributes,
                       Ticks const& pts ) = 0;

    /**
     * Notify the player that buffering is complete from the player bridge
     */
    virtual void completeBuffering(uint32_t actualStartPts) = 0;

    /**
     * feed drm header data from Media Request Bridge
     * This is for the new JS based streaming engine
     * @param[in] drmHeader the drm header
     */
    virtual NFErr feedDrmHeader(std::vector<unsigned char> const& drmHeader) = 0;

    /**
     * Abort opening the player, which can take a long time on some platforms.
     */
    virtual void abort() = 0;

    /**
     * Add a new period to the playback session
     * @param[in] pManifest the newly added manifest period to the playback
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr addManifest(shared_ptr<ase::Manifest> pManifest) = 0;

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
     * obtain the availlable playback buffer range the player can skip without
     * rebuffering.
     *
     * @param[out] startPts start pts of the available buffer
     * @param[out] endPts end pts of the available buffer
     */
    virtual void getBufferRange(Ticks& startPts, Ticks& endPts) = 0;

    /**
     * Returns the number of predicted future rebuffers from the current playback time
     * @return the number of predicted rebuffers
     */
    virtual uint32_t getPredictedFutureRebuffers() const = 0;

    /**
     * Set the available video bitrate ranges for each profile.
     * The player only utilizes the stream which bitrate
     *
     * falls into [minVideoBitrate, maxVideoBitrate] for each profile
     * @param[in] videoBitrateRanges bitrate ranges for each profile
     * @return NFErr_OK if set, error otherwise
     */
    virtual NFErr setVideoBitrateRanges(std::vector< shared_ptr<netflix::ase::BitrateRange> > const& videoBitrateRanges) = 0;

    /**
     * get the current bandwidth in kbps
     * @return the current bandwidth
     */
    virtual uint32_t getCurrentBandwidth() const = 0;

    /**
     * gets the maximum streaming buffer length
     *
     * @return the max buffer in ms
     */
    virtual uint32_t getMaxStreamingBuffer() const = 0;

    /**
     * dynamically adjust maximum/minimum streaming buffer length
     *
     * @param[in] powerSaving: whether the player in power saving mode
     * @param[in] maxBufferLen: maximum buffer length in ms
     * @param[in] minBufferLen: minimum buffer length in ms (ignored if powerSaving=false)
     */
    virtual void setStreamingBuffer(bool powerSaving,
                                    uint32_t maxBufferLen,
                                    uint32_t minBufferLen) = 0;

    /**
     * obtain the video and audio media buffer pool size
     * @param [out] videoBufferPoolSize
     * @param [out] audioBufferPoolSize
     */
    virtual void getMediaBufferPoolSize(uint32_t& videoBufferPoolSize,
                                        uint32_t& audioBufferPoolSize) = 0;

    /** Provide the license for the stream
     *
     * This method will be called after the object has called
     * LicenseHandler::provideChallenge to provide a license challenge
     *
     * @param[in] license The license
     * @param[out] the DRM session ID
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr provideLicense(const std::vector<unsigned char>& license,
                                 std::vector<unsigned char>& secureStopId,
                                 uint32_t& drmSessionId_) = 0;

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
     * Unpause playback. When this function returns, the new state will be
     * PLAYING.
     *
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr unpause() = 0;

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

    /**
     * Changes the currently selected (active) audio track.
     *
     * If the audio track is changed and the device does not support
     * dynamic audio switch while video is playing, -2 will be returned
     * and no changes will be made when the object is in PLAYING or
     * PAUSED state.
     *
     * @param[in] trackId the ID of the track
     *            to be selected.
     *
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr selectAudioTrack(const std::string &trackId) = 0;

    /**
     * Get the ID of the currently selected audio track. If the player is not
     * opened when this method is called, (-1) will be returned.
     *
     * @return the ID of the currently selected audio track.
     */
    virtual void getCurrentAudioTrack(std::string &) const = 0;

    /**
     * Two new APIs for on-the-fly audio switch in NRDP Video
     */
    virtual void startAudioTrackSwitch() = 0;
    virtual void completeAudioTrackSwitch() = 0;

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
     * Returns the total playback duration of the entire movie.
     *
     * The playback duration is invalid while the playback state is in
     * CLOSED or OPENING. An invalid Ticks will be returned in such case.
     *
     * @return the total playback duration of the entire movie.
     */
    virtual Ticks getPlayDuration() const = 0;

    /**
     * Returns the gop size of the video stream
     */
    virtual uint32_t getGopSize() const = 0;

    /**
     * Returns the playback state.
     *
     * @return the playback state.
     */
    virtual State getState() const = 0;

    /**
     * Returns if the player is at end of presentation
     *
     * @return true iff the player is at end of presentation
     */
    virtual bool endOfPresentation() const = 0;

    /**
     *
     * XXX: Needed at this level ?
     *
     * Get the display-aspect-ratio of movie. This information will be available
     * as soon as MediaControl opening is completed (i.e., when openComplete() of
     * the Listener interface is called to notify about the completion). This
     * information will not be available after MediaControl gets closed.
     *
     * If the display-aspect-ratio information is not available, zeros will be
     * returned in the output parameters.
     *
     * @param[out] aspectRatioX the X dimension of the display-aspect-ratio.
     * @param[out] aspectRatioY the Y dimension of the display-aspect-ratio.
     */
    virtual void getDisplayAspectRatio(uint32_t& aspectRatioX,
                                       uint32_t& aspectRatioY) const = 0;

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
    virtual void setVideoWindow(Rect targetRect, uint32_t transitionDuration) = 0;

    /**
     * Bring the video presentation plane to above the UI graphic plane. As a
     * result, the video content (within the video window) will be visible to
     * the user, while the graphic content within the same area will be hidden
     * underneath the video content.
     *
     * This functionality is optional. If it is not supported, NFErr_NotAllowed
     * will be returned and the video presentation plane remains below the UI
     * graphic plane all the time.
     *
     * @return NFErr_OK if successfully;
     *         NFErr_NotAllowed if this function is not supported.
     */
    virtual NFErr bringVideoToFront() = 0;

    /**
     * Send the video presentation plane to below the UI graphic plane. As a
     * result, the video content (within the video window) will be visible to
     * the user only if the graphic content within the same area has non-zero
     * transparency.
     *
     * This functionality is optional. If it is not supported, NFErr_NotAllowed
     * will be returned and the video presentation plane remains below the UI
     * graphic plane all the time.
     *
     * @return NFErr_OK if successfully;
     *         NFErr_NotAllowed if this function is not supported.
     */
    virtual NFErr sendVideoToBack() = 0;

    /**
     * set thenetwork interface information
     */
    virtual void networkChange( NetworkInterfaceInfo const& netinfo ) = 0;

    /**
     * set the client external ip address
     */
    virtual void externalIpAddressChange( std::string const& ipaddr ) = 0;

    /** Get the network monitor state */
    virtual Variant getNetworkMonitorState() = 0;

    /** Set the network monitor state */
    virtual void setNetworkMonitorState( Variant const& state ) = 0;

    /** Set the network monitor listener */
    virtual void setNetworkMonitorListener( shared_ptr<ase::INetworkMonitorListener> pListener ) = 0;

    /**
     * Prepares the player for playback of a manifest.
     *
     * @param[in] pManifest the manifest to play.
     * @param[in] video3DType ether 2D or 3D video
     * @param[in] defaultAudioTrack the default audio track
     * @param[in] playbackReporter the playback reporter to send statistics to
     * @return NFErr_OK on success
     *         otherwise an error indicating failure
     */
    virtual NFErr cachePrepare(shared_ptr<ase::Manifest> pManifest,
                               netflix::Video3DType video3DType,
                               const std::string &defaultAudioTrack,
                               shared_ptr<IPlaybackReporter> playbackReporter) = 0;

    /**
     * Flushes any media files prepared on the player
     */
    virtual void cacheFlush() = 0;

    /**
     * Requests the lists of movies prepared on the player
     * @return array of strings representing movieId's in the media file cache
     */
    virtual std::vector<std::string> cacheList() = 0;

    /**
     * Sets the max number of items to store in the media file cache
     * @param[in] maxItems max number of items to cache
     */
    virtual void cacheSetSize(int maxItems) = 0;

    /**
     * getVolume
     */
    virtual double getVolume() = 0;

    /**
     * setVolume
     */
    virtual void setVolume(double targetVolume, uint32_t transitionDuration, device::IAudioMixer::Ease ease) = 0;

};

}} // namespace netflix::mediacontrol


#endif
