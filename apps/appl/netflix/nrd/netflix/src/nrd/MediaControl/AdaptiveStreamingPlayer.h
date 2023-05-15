/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ADAPTIVESTREAMINGPLAYER_H
#define ADAPTIVESTREAMINGPLAYER_H

#include <nrdbase/ConditionVariable.h>
#include <nrdbase/Mutex.h>

#include <nrdase/IStreamingReporter.h>
#include <nrdase/AseBuffer.h>
#include <nrdase/AseCommonDataTypes.h>

#include <nrd/IPlaybackDevice.h>
#include <nrd/IBufferManager.h>
#include <nrd/config.h>

#include "common.h"
#include "IAdaptiveStreamingPlayer.h"
#include "IPlaybackReporter.h"

namespace netflix {

namespace ase {
class AseBuffer;
class IStreamingHeuristic;
class ILocationSetMonitor;
class Manifest;
class ManifestTrack;
class StreamingManager;
}

namespace mediacontrol {

class EventThread;
class MediaFileCache;
class PlayerState;
class PumpingThread;
class StreamingConfigParameter;
class LocalConfigParameter;
class StreamingPersistentStore;
class McMediaBuffer;
class McMediaBufferPool;

/**
 * @class AdaptiveStreamingPlayer AdaptiveStreamingPlayer.h
 * @brief Media Playback Controller Internal Class.
 */
class AdaptiveStreamingPlayer : public IAdaptiveStreamingPlayer, public ase::IStreamingReporter
{
public:
    /** Constructor. */
    AdaptiveStreamingPlayer();

    /** Destructor. */
    virtual ~AdaptiveStreamingPlayer();

    virtual void shutdown();

    //================================================================================
    // IAdaptiveStreamingPlayer interface implementation.
    //================================================================================

    virtual NFErr open(shared_ptr<ase::Manifest> pManifest,
                       shared_ptr<IPlaybackReporter> playbackReporter,
                       shared_ptr<LicenseHandler> licensehandler, ullong sessionId,
                       netflix::Video3DType video3DType,
                       const std::string &defaultAudioTrack, netflix::StreamingType streamingType,
                       ContainerType containerType,
                       const device::IPlaybackDevice::CommonVideoAttributesFromManifest &videoAttributes,
                       const device::IPlaybackDevice::CommonAudioAttributesFromManifest &audioAttributes,
                       Ticks const& pts);
    virtual void completeBuffering(uint32_t actualStartPts);
    virtual NFErr feedDrmHeader(std::vector<unsigned char> const& drmHeader);
    virtual void abort();
    virtual NFErr addManifest(shared_ptr<ase::Manifest> pManifest);
    virtual void close();
    virtual NFErr play(const Ticks& pts, bool playAndPause );
    virtual NFErr skip(const Ticks& pts);
    virtual void getBufferRange(Ticks& startPts, Ticks& endPts);
    virtual uint32_t getPredictedFutureRebuffers() const;
    virtual NFErr stop();
    virtual NFErr pause();
    virtual NFErr unpause();
    virtual NFErr addListener(shared_ptr<Listener> l);
    virtual NFErr removeListener(shared_ptr<Listener> l);
    virtual NFErr selectAudioTrack(const std::string&);
    virtual void getCurrentAudioTrack(std::string &) const;

    // Two NrdpVideo APIs
    virtual void startAudioTrackSwitch() {}
    virtual void completeAudioTrackSwitch()  {}

    virtual Ticks getCurrentPts() const;
    virtual Ticks getPlayDuration() const;
    virtual uint32_t getGopSize() const;
    virtual State getState() const { return state_; }
    virtual bool endOfPresentation() const { return endOfPresentation_; }
    virtual void getDisplayAspectRatio(uint32_t& aspectRatioX,
                                       uint32_t& aspectRatioY) const;
    virtual void setVideoWindow(Rect targetRect, uint32_t transitionDuration);
    virtual NFErr bringVideoToFront();
    virtual NFErr sendVideoToBack();

    virtual NFErr provideLicense(const std::vector<unsigned char>& license,
                                 std::vector<unsigned char>& secureStopId,
                                 uint32_t& drmSessionId);

    virtual void networkChange( NetworkInterfaceInfo const& netinfo );
    virtual void externalIpAddressChange( std::string const& ipaddr );
    virtual Variant getNetworkMonitorState();
    virtual void setNetworkMonitorState( Variant const& state );
    virtual void setNetworkMonitorListener( shared_ptr<ase::INetworkMonitorListener> pListener );

    virtual NFErr cachePrepare(shared_ptr<ase::Manifest> pManifest,
                               netflix::Video3DType video3DType,
                               const std::string &defaultAudioTrack,
                               shared_ptr<IPlaybackReporter> playbackReporter);
    virtual void cacheFlush();
    virtual std::vector<std::string> cacheList();
    virtual void cacheSetSize(int maxItems);
    virtual double getVolume();
    virtual void setVolume(double targetVolume, uint32_t transitionDuration, device::IAudioMixer::Ease ease);

    virtual shared_ptr<ase::StreamingStatReport> obtainStreamingStat( );
    virtual void getPlaybackQualityStats(device::IPlaybackDevice::PlaybackQualityStats& audioStats,
                                         device::IPlaybackDevice::PlaybackQualityStats& videoStats);

    virtual NetworkProfile getNetworkProfile() const;
    virtual void setNetworkProfile(NetworkProfile nwProfile);
    virtual NFErr setVideoBitrateRanges(std::vector< shared_ptr<netflix::ase::BitrateRange> > const& videoBitrateRanges);
    virtual uint32_t getCurrentBandwidth() const;
    virtual uint32_t getMaxStreamingBuffer() const;
    virtual void setStreamingBuffer(bool powerSaving,
                                    uint32_t maxBufferLen,
                                    uint32_t minBufferLen);
    virtual void getMediaBufferPoolSize(uint32_t& videoBufferPoolSize, uint32_t& audioBufferPoolSize);

    //================================================================================
    // IStreamingReporter interface implementation.
    //================================================================================
    virtual void openCompleted(uint32_t duration, uint32_t gopSize);
    virtual void updateContentDuration(uint32_t duration);
    virtual void notifyManifestSelected( uint32_t manifestIndex );
    virtual void bufferingStarted();
    virtual void skipStarted() {}
    virtual void bufferingComplete(uint32_t actualStartPts,
                                   ullong predictedBufferingTime,
                                   uint32_t initialBW, uint32_t actualBW);
    virtual void bufferingStatus(uint32_t totalBufferingTime,
                                 uint32_t currentBufferingTime);
    virtual void streamingFailure(int32_t manifestIndex,
                                  int32_t trackIndex, int32_t streamIndex,
                                  AseErrorCode error, int32_t httpErr,
                                  const std::string &errorMsg);
    virtual void streamingFailureReasons(AseErrorCode networkError, int32_t httpErr);
    virtual void reportStreamingServerFailure(uint32_t cdnId);
    virtual void streamSelected(int32_t manifestIndex,
                                int32_t trackIndex, int32_t streamIndex,
                                const ase::AseTimeStamp &movieTime,
                                uint32_t bandwidth, uint32_t longtermBw,
                                uint32_t rebuffer);
    virtual void updateStreamingPts(int32_t manifestIndex,
                                    int32_t trackIndex,
                                    const ase::AseTimeStamp &movieTime);
    virtual void updateBufferLevel(uint32_t abuflbytes, uint32_t vbuflbytes,
                                   uint32_t predictedFutureRebuffers,
                                   uint32_t currentBandwidth);
    virtual void updateStreamingStat( shared_ptr<ase::StreamingStatReport> pStreamingStat);
    virtual void serverSelected( uint32_t selcdnid,
                                 std::string const& selcdnname,
                                 uint32_t selcdnrtt,
                                 uint32_t selcdnbw,
                                 std::string const& domainbwdata,
                                 std::string const& testreason,
                                 std::string const& selreason,
                                 uint32_t fastselthreshold );

    virtual void audioTrackSwitchStarted(std::string const& oldLangCode,
                                         uint32_t oldNumChannels,
                                         std::string const& newLangCode,
                                         uint32_t newNumChannels);
    virtual void audioTrackSwitchComplete();
    virtual void flushComplete(ase::MediaType mediaType);
    virtual void connectionSelected1( uint32_t origmaxnum, uint32_t actualmaxnum, uint32_t rtt );
    virtual void connectionSelected2( uint32_t defaultnumconn, uint32_t selnumconn,
                                      uint32_t maxnumconn );
    virtual void videoBitrateSelectionCompleted();
    virtual void reportKeyStartplayEvent( std::string const& eventName );
    virtual void pipeliningDisabled();
    virtual void serverSwitch(
                        uint32_t                        cdnid,
                        shared_ptr<ase::ILocationSetMonitor> pLocationSetMonitor,
                        ServerSelectionReason           reason,
                        uint32_t                        bitrate );

    virtual void reportFileToken( std::vector<unsigned char> const& filetoken );

private:

    class McMediaSink;

    enum { VIDEO_AT_FRONT, VIDEO_AT_BACK } videoZOrderPending_;

private:
    class ManifestMetaData
    {
    public:
        ManifestMetaData()
        {
            audioSelStreamIndex_ = -1;
            videoSelStreamIndex_ = -1;
            audioPresTrack_ = -1;
            audioPresStreamIndex_ = -1;
            videoPresStreamIndex_ = -1;

            defaultAudioIndex_ = -1;
            currentAudioIndex_ = -1;
            activeAudioIndex_ = -1;
            currentVideoIndex_ = -1;
        }

    public:
        volatile uint32_t audioSelStreamIndex_;
        volatile uint32_t videoSelStreamIndex_;
        volatile uint32_t audioPresTrack_;
        volatile uint32_t audioPresStreamIndex_;
        volatile uint32_t videoPresStreamIndex_;

        int32_t defaultAudioIndex_;
        int32_t currentAudioIndex_;
        int32_t activeAudioIndex_;
        int32_t currentVideoIndex_;

        std::vector<uint32_t> audioTracksIndex_;
        std::vector<uint32_t> videoTracksIndex_;
        std::vector<uint32_t> firstStreamIndexInTrack_;
    };

private:
    static const uint32_t LICENSE_ACQ_PERCENTAGE      = 10;

    volatile State    state_;
    volatile netflix::StreamingType     streamingType_;
    ContainerType     containerType_;

    // state variables shared between AdaptiveStreamingPlayer and PumpingThread
    shared_ptr<PlayerState> playerState_;

    volatile bool     abort_;
    volatile bool     storeError_;
    volatile bool     endOfPresentation_;
    volatile bool     dataExhausted_;
    volatile bool     openComplete_;
    volatile bool     deviceOpened_;
    volatile bool     deviceInitialized_;
    volatile bool     newVideoWindowPending_;
    volatile bool     newVideoZOrderPending_;
    volatile bool     newVolumePending_;
    volatile double   targetVolumePending_;
    volatile uint32_t transitionDurationPending_;
    volatile device::IAudioMixer::Ease easePending_;
    volatile bool     playbackStarted_;

    volatile bool     havePts_;
    volatile uint32_t currentPts_;
    volatile uint32_t playDuration_;
    volatile uint32_t gopSize_;
    volatile uint32_t reportedVideoBitrate_;
    volatile uint32_t latestReportedPTS_;
    volatile bool     playbackSkipping_;
    // boolean to suppress buffering events
    volatile bool     suppressBufferingEvents_;

    volatile uint32_t predictedFutureRebuffers_;
    volatile uint32_t currentBandwidth_;
    shared_ptr<ase::StreamingStatReport> streamingStat_;
    volatile uint32_t maxStreamingBufferLen_;
    volatile uint32_t minStreamingBufferLen_;
    volatile bool     powerSaving_;
    volatile uint32_t videoBufferPoolSize_;
    volatile uint32_t audioBufferPoolSize_;
    volatile NetworkProfile networkProfile_;
    uint32_t          minimumDataInDevice_;

    volatile bool     pendingBufferingComplete_;

    Video3DType video3DType_;
    std::vector<shared_ptr<netflix::ase::BitrateRange> > videoBitrateRanges_;

    std::vector<unsigned char> challengeData_;
    std::vector<unsigned char> drmHeader_;

    std::vector<shared_ptr<ase::Manifest> >         manifests_;
    std::vector<shared_ptr<ManifestMetaData> >      manifestData_;
    volatile uint32_t                               currentManifestIndex_;
    volatile uint32_t                               lastAudioManifestIndex_;
    volatile uint32_t                               lastVideoManifestIndex_;

    shared_ptr<IPlaybackReporter>                   playbackReporter_;

    shared_ptr<StreamingConfigParameter>            streamingConfigParameter_;
    shared_ptr<LocalConfigParameter>                localConfigParameter_;
    shared_ptr<ase::IStreamingHeuristic>            streamingHeuristic_;
    shared_ptr<McMediaSink>                         mediaSink_;
    shared_ptr<McMediaBufferPool>                   mediaBufferPool_;
    shared_ptr<StreamingPersistentStore>            persistentStore_;
    shared_ptr<ase::StreamingManager>               streamingManager_;
    shared_ptr<MediaFileCache>                      mediaFileCache_;
    shared_ptr<device::IPlaybackDevice>             playbackDevice_;
    shared_ptr<device::IBufferManager>              bufferManager_;
    shared_ptr<PumpingThread>                       pumpingThread_;
    shared_ptr<EventThread>                         eventThread_;
    shared_ptr<LicenseHandler>                      licenseRetriever_;

    Time licenseAcquireTime_;

    Rect videoWindowTargetRectPending_;
    volatile uint32_t videoWindowTransitionDurationPending_;

    mutable Mutex stateMutex_;
    mutable Mutex threadMutex_;
    mutable Mutex streamingManagerMutex_;
    mutable Mutex streamingStatMutex_;
    mutable Mutex playbackDeviceMutex_;
    mutable Mutex audioTrackSwitchMutex_;
    mutable Mutex metaDataMutex_;
    mutable Mutex manifestMutex_;
    mutable Mutex openCompleteMutex_;

    mutable ConditionVariable openCompleteSignal_;


    // mutex and condition so we can block in faststart if play() is called with a
    // nonzero pts

    // This type is used to assist logging of situations where:
    //
    // 1) the next audio block has the earliest PTS (and should therefore be
    // selected for feeding to the PlaybackDevice) but there is no audio block
    // available to feed,
    //
    // 2) the next video block has the earliest PTS but there is no video block
    // available to feed,
    //
    // 3) there are no video or audio blocks available to feed.
    //
    enum GetBlockOutcomeForLogging {
        DeliveredSelectedBlock,
        VideoSelectedButNoBlockAvailable,
        AudioSelectedButNoBlockAvailable
    };
    GetBlockOutcomeForLogging lastGetBlockOutcomeForLogging_;
    uint32_t numGetBlockCallsSinceLastLog_;
    static const uint32_t logTriggerThreshold_ = 300;

private:
    void initParameters(uint32_t seekTime);

    void bufferingComplete();
    /**
     * update the streaming config parameters based on
     * 1. network profile
     * 2. whether the stream is 2D or 3D
     */
    NFErr updateConfigParameters();

    //================================================================================
    // Functions called from the pumping thread and interlocked with control state.
    //================================================================================

    /** Signal completion of media loading and handle state transition. */
    void notifyLoadingComplete();

    /**
     * Start halting playback (to start buffering) or stop halting playback (to resume
     * playback after buffering is completed.
     *
     * @param[in] halted true to start halting playback, false to stop halting.
     */
    void haltPlayback(bool halted);

    //================================================================================
    // Functions used to start/stop playback device during playback skip
    //================================================================================
    NFErr startPlaybackDevice(ullong pts);
    NFErr stopPlaybackDevice();

    //================================================================================
    // Media loading/unloading procedures.
    //================================================================================

    /**
     * Load Media.
     * @return true on success; false on failure.
     */
    bool load();

    NFErr initializePlayers();

    /** Unload Media. */
    void unload();

    /**
     * Create the streaming manager.
     *
     * @param[in] defaultAudioIndex index of the default selected audio track.
     *
     * @return NFErr_OK on success; NFErr_MC_InitStreamFailure on failure.
     */
    NFErr createStreamingManager(uint32_t defaultAudioIndex);

    /**
     * Wait for the signal from the Media Presentation that the first header
     * has been downloaded and delivered to the Media Sink
     *
     * @param[out] firstSelTrackIndex   Index into current manifest for the track to which
     *                                  the first stream header delivered to the media sink belongs
     * @param[out] firstSelStreamIndex  Index into the track for the stream whose media header was
     *                                  delivered
     * @param[out] firstSelStreamHeader The stream header itself
     *
     * @return NFErr_OK on success; NFErr_NotReady if no stream header (timeout)
     */
    NFErr waitForFirstHeader(   /*out*/uint32_t& firstSelTrackIndex,
                                /*out*/uint32_t& firstSelStreamIndex,
                                /*out*/shared_ptr<std::vector<unsigned char> >& firstSelStreamHeader );
    /**
     * Open the playback device and the device buffer manager. The media streams
     * are used as input to configure the playback device.
     *
     * @param streamInfos The stream infos structure constructed from the manifest
     *
     * @param lastVideoProfile The last video profile in the manifest
     *
     * @return NFErr_OK on success; NFErr_MC_OpenDeviceFailure on failure.
     */
    NFErr openPlaybackDevice(   std::vector<device::IPlaybackDevice::StreamInfo> const& streamInfos,
                                ContentProfile lastVideoProfile );

    /**
     * Create and initialize the media buffer pool
     */
    NFErr createBufferPool();

    //================================================================================
    // Licensing procedures.
    //================================================================================

    /**
     * Called by pumping thread to generate a drm challenge for a license request
     * @return NFErr_OK on success
     */
    NFErr generateChallenge();

    /**
     * Called by pumping thread to acquire playback license for the playback device.
     * @return NFErr_OK on success; NFErr_MC_AcquireLicenseFailure on failure.
     */
    NFErr acquireLicense();

    /**
     * Called by pumping thread to clear any license set in playback device
     */
    void clearLicense();

    //================================================================================
    // Data pumping procedures.
    //================================================================================

    /**
     * Called by either pumping thread or license thread to start the streaming manager
     * on play or after a stall
     */
    AseErrorCode startStreamingManager(int manifestIndex, uint32_t pts);

    /**
     * Called by pumping thread to set the new playback speed
     * @return NFErr_OK on success
     */
    NFErr setPlaybackSpeed(uint32_t speed);

    /**
     * Called by pumping thread to see if audio tracks need to be switched on the fly
     */
    bool checkForSwitchAudioTrack();

    /**
     * Gets one audio and one video data block for delivery.
     *
     * @param[in] avPumpingContext the audio/video data pumping context. A side
     * effect of this method is that the avPumpingContext.currAudioBlock_ and
     * .currVideoBlock_ are set to the next blocks in the streams.
     */
    void getBlocks(AvPumpingContext& avPumpingContext);

    void readBlock(ase::MediaType mediaType,
                   MediaEntity& dataBlock,
                   uint32_t& readPTS,
                   bool& readPtsUpdated);

    /**
     * Called by the pumping loop. Asks the McMediaSink for any new headers that
     * it has been received. Feeds the headers to the playback device for
     * parsing. Returns false if parsing any header failed.
     */
    bool feedHeaders();

    /**
     * delivers a stream header for the playback device. Returns false if
     * parsing the header failed. Reports NFErr_MC_DevicePlaybackError if the
     * playback device reports a parsing error.
     */
    bool feedStreamHeader(device::IPlaybackDevice::StreamType streamType,
                          uint32_t streamIndex,  uint32_t manifestIndex,
                          shared_ptr<std::vector<unsigned char> > streamHeader);

    /**
     * delivers a data block for the playback device
     */
    NFErr feedBlock(device::IPlaybackDevice::StreamType streamType, uint32_t streamIndex,
                    MediaEntity *blockToDeliver);

    /**
     * update the error count in the persistent store
     */
    void updateErrorCount();

    /**
     * Flush the entire playback pipeline, including flush of the playback device,
     * discarding any undelivered data blocks, and stop of the streaming manager.
     * Playback is paused after flush is done.
     *
     * @param avPumpingContext the audio/video data pumping context.
     * @param currSpeed the current playback speed.
     */
    void flush(AvPumpingContext& avPumpingContext,
               uint32_t& currSpeed);

    /**
     * Kick off the re-buffering (flushy or flush-less) procedure.
     *
     * @param avPumpingContext the audio/video data pumping context.
     * @param currSpeed the current playback speed.
     */
    void rebuffer(AvPumpingContext& avPumpingContext,
                  uint32_t& currSpeed);

    /** End buffering state and start playback (if it is not paused by user). */
    void endBuffering();

    //================================================================================
    // Event handling procedures.
    //================================================================================

    /**
     * Update the current presentation time position according to the received
     * PTS events from the playback device.
     *
     * @param[in] pts the current presentation timestamp in milliseconds.
     * @param[in] monotime the time that pts is posted via event
     */
    void updatePTS(ullong pts, ullong monotime);
    void videoPTSUpdate(ullong pts, ullong monotime);
    void audioPTSUpdate();

    //================================================================================
    // Helpers.
    //================================================================================

    /**
     * helpers to get current manifest data
     * setup the streaming configs for this playback
     */
    void setupStreamingConfigParams();

    void setupMobileParameters();

    /**
     * report an error to threads, listeners and playback reporter
     */
    void reportError(NFErr err);

    /**
     * helpers to get current manifest data
     */
    void getManifest(/*out*/shared_ptr<ase::Manifest> &manifest,
                     /*out*/shared_ptr<ManifestMetaData> &mmd,
                     uint32_t manifestIndex) const;

    /**
     * @param[in] manifest the manifest to process
     * @returns a new ManifestMetaData processed from the manifest
     */
    static NFErr processManifest(/*out*/shared_ptr<ManifestMetaData> &manifestMetaData,
                                 shared_ptr<ase::Manifest> manifest,
                                 netflix::Video3DType video3DType,
                                 const std::string &defaultAudioTrack);

    /**
     * Populate the manifest metadata with the global index of the first stream of
     * each track
     */
    void calculateStreamIndices( uint32_t manifestIndex );

    /**
     * Build a streamInfos array from the manifest
     * @returns the last video profile seen
     */
    ContentProfile buildStreamInfos(
        /*out*/std::vector<device::IPlaybackDevice::StreamInfo> &streamInfos,
        uint32_t manifestIndex,
        uint32_t firstSelectedTrackIndex, uint32_t firstSelectedStreamIndex,
        shared_ptr<std::vector<unsigned char> > mediaHeader);


    /* For fast start, at playback device open time we don't have the max video
     * width and height taken across all of the encoded streams that match
     * profiles supported by the device.  We only have what we might receive
     * during the manifest 0 period.  We therefore have to examine the video
     * content profiles reported by the device's ISystem capabilities and assume
     * that we will encounter dimensions up to the maximum possible for the
     * supported content profiles. */
    void getDeviceMaxVideoDimensions(std::vector<device::IPlaybackDevice::StreamInfo> const& streamInfos,
                                     uint32_t& maxImageWidth,
                                     uint32_t& maxImageHeight);

    /**
     * Gets the current video track from the manifest
     */
    shared_ptr<ase::ManifestTrack> getCurrentVideoTrack();

    /**
     * Notify listeners of any new tracks
     */
    void notifyTracksChanged();

    void obtainSlowPumpingParas(uint32_t& slowPumpingEnterThreshold, uint32_t& slowPumpingExitThreshold, uint32_t& sleepDuringSlowPumping);

    friend class EventThread;
    friend class PumpingThread;
};

}} // namespace netflix::mediacontrol

#endif // ADAPTIVESTREAMINGPLAYER_H

