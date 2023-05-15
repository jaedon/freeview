/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MEDIA_PRESENTATION_H
#define MEDIA_PRESENTATION_H

#include <vector>
#include <map>
#include <deque>

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/IMediaBufferPool.h>
#include <nrdase/IMediaConsumer.h>
#include <nrdase/AseTimeStamp.h>

#include "StreamingManager.h"
#include "StreamingCommandQueue.h"

#include <nrdase/IPersistentStore.h>

#include <nrdase/DebugLogging.h>

#include <nrdbase/Mutex.h>

namespace netflix {
namespace ase {

DECLARE_LOGGER( MediaPresentationLog );

class INetworkMonitor;
class MediaTrack;

typedef shared_ptr<MediaTrack> MediaTrackPtr;
typedef shared_ptr<INetworkMonitor> INetworkMonitorPtr;

/**
 * @class MediaPresentation
 * @brief The root of the object tree managing streaming
 *
 * The Media(Presentation|Track|Stream) object tree manages the actual
 * streaming process. The HttpRequestManager repeatedly calls the
 * IMediaConsumer::getNextRequest method on the MediaPresentation
 * and this call cascades down the tree towards the object that is most
 * in need of data.
 */
class MediaPresentation : public IMediaConsumer
{
public:
    typedef shared_ptr<MediaPresentation> MediaPresentationPtr;


    /** construct the media presentation object, the head of the media presentation tree
     * @param[in] pManifest The media manifest
     * @param[in] pMediaSink The media sink accepting a/v data
     * @param[in] pMediaBufferPool The buffer allocator for media, i.e. audio and video
     * @param[in] pStreamingHeuristic A streaming heuristic plugin, which includes
     *                                INetworkMonitor, IStreamSelector, ILocationSelector
     * @param[in] pConfigParameter The interface to provide streaming parameters.
     * @param[in] pStreamingReporter reporter for streaming related events and metrics
     * @param[out] pMediaPresentation The new media presentation object
     * @return 0 on success or negative error codes to be defined
     */
    static AseErrorCode construct(
                              shared_ptr<StreamingManager> pStreamingManager,
                              ManifestPtr   pManifest,
                              IMediaSinkPtr const& pMediaSink,
                              IMediaBufferPoolPtr const& pMediaBufferPool,
                              IStreamingHeuristicPtr const& pStreamingHeuristic,
                              IAseConfigParameterPtr const& pConfigParameter,
                              IAseConfigParameterPtr pLocalConfigParameter,
                              IStreamingReporterPtr const& pStreamingReporter,
                              IPersistentStorePtr   const& pPersistentStore,
                              MediaPresentationPtr&     pMediaPresentation );

    // IMediaConsumer
    // These functions are called from HttpReqeustManager's thread
    virtual void configure( const IAseConfigParameterPtr &pConfig );
    virtual void performStreamingTasks();
    virtual AseErrorCode getNextRequest( uint32_t downloadTrackId, MediaRequestPtr& request );
    virtual void getBufferStats( MediaType mediaType,
                                 ullong& usedBuffer, ullong& availableBuffer );
    virtual uint32_t getPlayableLen( MediaType mediaType );
    virtual void reportFailure(AseErrorCode errorCode, std::string const& message);
    virtual void reportFailure(AseErrorCode errorCode, int32_t httpErr, std::string const& message);
    virtual void pipeliningDisabled();
    virtual bool isBufferAvailable(MediaType mediaType);
    virtual void networkFailing(    uint32_t                dwonloadTrackId,
                                    AseErrorCode            latestStatus,
                                    AseUrl::HostPort const& destination,
                                    AseTimeVal const*       pLatestErrorTime,
                                    AseTimeVal const&       latestDataReceived );

    /** -------------------------------------------------------------------- */
    // Commands from Streaming Manager
    AseErrorCode open( const std::vector<uint32_t>& trackIndexes, bool fastplay,
                       std::vector<shared_ptr<IMediaFile> > const& mediaFiles);
    AseErrorCode start( AseTimeStamp const& seekTime );
    AseErrorCode stop();
    void skip();
    AseErrorCode close();
    void pause();
    void resume();
    // TODO: add manifest index in the API
    AseErrorCode switchTracks( const std::vector<uint32_t>& trackIndexes );
    AseErrorCode rebuffer();
    AseErrorCode addManifest(ManifestPtr pManifest);
    void notifyLicenseAcquired( uint32_t manifestManifestIndex );

    void setVideoBitrateRanges(std::vector< shared_ptr<BitrateRange> > const& videoBitrateRanges);
    uint32_t getMaxStreamingBuffer() const;
    void setStreamingBuffer(bool powerSaving, uint32_t maxBufferLen, uint32_t minBufferLen);
    void updateDownloaderTrackId(   uint32_t videoDownloaderTrackId,
                                    uint32_t audioDownloaderTrackId,
                                    uint32_t headersDownloaderTrackId );

    void setStreamingSpeed( uint32_t speed );
    // Commands from Streaming Manager
    /** -------------------------------------------------------------------- */

    /** -------------------------------------------------------------------- */
    /**             Streaming Commands Implementation                        */
    AseErrorCode runSwitchTracksCommand(const std::vector<uint32_t>& trackIndexes);
    AseErrorCode runSetSeekTimeCommand(AseTimeStamp const& seekTime);
    AseErrorCode runRebufferCommand();
    AseErrorCode runSkipCommand();
    AseErrorCode runAddManifestCommand(ManifestPtr pManifest);
    AseErrorCode runNotifyLicenseAcquiredCommand();
    AseErrorCode runSetVideoBitrateRangeCommand
                    (std::vector<shared_ptr<BitrateRange> > const& videoBitrateRanges);
    AseErrorCode runSetStreamingBufferCommand(uint32_t maxBufferLen, uint32_t minBufferLen);
    AseErrorCode runUpdateDownloaderTrackIdCommand();


    /**             Streaming Commands Implementation                        */
    /** -------------------------------------------------------------------- */

    // feedback from MediaStreamRequest
    // report that one of A/V buffer is full
    void bufferFull();
    // report the amount of data received
    void updateDataReception(uint32_t size);
    bool isBufferingState() const;


    AseTimeStamp seekTime() const
    {
        return mSeekTime;
    }

    void reportNewStreamDuration(const std::string& streamKey, uint32_t duration);

    // return true if the current manifest is the final one
    bool finalManifest();

    void updateContentDuration(uint32_t duration);

    void selectStream(int32_t trackId, int32_t& streamId,
                      uint32_t& bandwidth, uint32_t& longtermBw,
                      uint32_t& predictedRebuffer,
                      IStreamSelector::StreamFeasibility& streamFeasibility,
                      int32_t& predictedDlTimeForBackupStream);
    void checkBufferingStatus();

    bool bitrateSwitchFreeze() const;

    // McQueen related functions
    bool audioTrackReadyForMigration() const;
    AseErrorCode migrate();

    void videoEndOfTrack();

public:
    virtual ~MediaPresentation() {}

protected:
    static const uint32_t MAX_DURATION_MISMATCH = 10 * 1000;

    MediaPresentation( shared_ptr<StreamingManager> pStreamingManager,
                       ManifestPtr   pManifest,
                       IMediaSinkPtr const& pMediaSink,
                       IMediaBufferPoolPtr const& pMediaBufferPool,
                       IStreamingHeuristicPtr const& pStreamingHeuristic,
                       IAseConfigParameterPtr const& pConfigParameter,
                       IAseConfigParameterPtr pLocalConfigParameter,
                       IStreamingReporterPtr const& pStreamingReporter,
                       IPersistentStorePtr const& pPersistentStore);

    void getConfiguration();

    struct BufferingStats
    {
    public:
        BufferingStats() :
            mBufferingStatus(IStreamSelector::BUFFER_LOW),
            mRequiredBufferingLen(-1),
            mEstimateBufferingTime(-1)
        {}

        IStreamSelector::BufferingStatus mBufferingStatus;
        uint32_t mRequiredBufferingLen;
        uint32_t mEstimateBufferingTime;
    };

    /** construct media tracks based on the trackIndexes */
    AseErrorCode constructMediaTracks(std::vector<uint32_t> const& trackIndexes);

    AseErrorCode switchMediaTracks(std::vector<uint32_t> const& trackIndexes);

    /** obtain the track using the provided media type */
    MediaTrackPtr obtainMediaTrack( MediaType mediaType ) const;

    /** check whether the streams' duration are valid */
    AseErrorCode checkStreamDuration();
    /**
     * @param[out] minBufferLen: the minimum buffering length among all tracks
     * @param[out] maxPendingBufferLen: the maximum pending buffer length among all tracks*/
    void obtainMixedBufferLen(uint32_t& minBufferLen, uint32_t& maxPendingBufferLen) const;

    /**
     * @param[out] audioBufferLen
     * @param[out] videoBufferLen
     */
    void obtainBufferLen(uint32_t& audioBufferLen, uint32_t& videoBufferLen);
    void obtainBufferSize(uint32_t& audioBufferSize, uint32_t& videoBufferSize);

    /**
     * @return the maximum different between streaming time of different tracks
     */
    uint32_t obtainBufferingDifference();

    void statusUpdate(bool enableTraceLogging = true);

    AseErrorCode updateStreamingTime(AseTimeStamp const& mSeekTime);
    void updatePlaybackTime();

    void reportFailure(AseErrorCode errorCode, int32_t manifestIndex,
                       int32_t httpErr, std::string const& message);

    void bufferingComplete(std::string const& reason);

    /** Judge whether the player has too much media buffer based on mMaxMediaBufferLen */
    bool tooMuchMediaBuffer(MediaType mediaType);

    ManifestTrackPtr obtainManifestTrack(MediaType mediaType,
                                         std::vector<ManifestTrackPtr> const& manifestTracks,
                                         ManifestTrackPtr pManifestTrack,
                                         uint32_t& trackId);
    bool compatibleManifestTracks(ManifestTrackPtr pManifestTrack1, ManifestTrackPtr pManifestTrack2);

    AseErrorCode scheduleMigrate(ManifestPtr pManifest, MediaType mediaType);

    void obtainPlaybackStats(double& playbackRate,
                             AseTimeStamp& playbackTime,
                             ullong& availableBuffer);

    uint32_t obtainCurrentThroughput(uint32_t currentBufferLevel, double& confidence, std::string& locationSetKey);

    void getMediaPlaybackState(
                                uint32_t& playableLength,
                                bool& goodQuality );

    weak_ptr<StreamingManager>           mStreamingManagerPtr;

    volatile IMediaConsumer::State mMediaConsumerState;

    volatile uint32_t              mStreamingSpeed;

    std::deque<ManifestPtr> mManifestPtrs;

    IStreamingReporterPtr       mStreamingReporterPtr;
    IPersistentStorePtr         mPersistentStorePtr;
    IMediaSinkPtr               mMediaSinkPtr;

    shared_ptr<StreamingCommandQueue> mCommandQueuePtr;

    volatile bool               mFailed;

    IMediaBufferPoolPtr         mMediaBufferPoolPtr;
    ullong                    mTotalMediaBufferSize;
    volatile uint32_t           mMinMediaBufferLen;

    uint32_t           mMaxMediaBufferLenConfiged; // in ms
    uint32_t                    mMaxMediaBufferLenAllowed;
    uint32_t                    mMaxMediaBufferLenAllowedInFastplay;
    /**
     * During McQueen playback: mEffectiveMaxMediaBufferLen = min(mMaxMediaBufferLenConfiged, mMaxMediaBufferLenAllowed, mMaxMediaBufferLenAllowedInFastplay)
     * During drm playback: mEffectiveMaxMediaBufferLen = min(mMaxMediaBufferLenConfiged, mMaxMediaBufferLenAllowed)
     */
    uint32_t                    mEffectiveMaxMediaBufferLen;

    // heuristics
    INetworkMonitorPtr          mNetworkMonitorPtr;
    ILocationSelectorPtr        mLocationSelectorPtr;
    IStreamSelectorPtr          mStreamSelectorPtr;
    IConnectionConfigurationPtr mConnectionConfigurationPtr;
    IStreamingStatisticsPtr     mStreamingStatPtr;

    IAseConfigParameterPtr      mConfigParamterPtr;
    IAseConfigParameterPtr      mLocalConfigParametePtr;

    // active media tracks
    std::vector<MediaTrackPtr>  mMediaTracks;

    AseTimeStamp                mSeekTime;
    AseTimeStamp                mActualStartPts;

    std::vector<ITrackBufferInfoPtr>    mTrackCurrentInfo;
    std::vector<ITrackInfoPtr>  mTrackFutureInfo;
    std::vector<uint32_t> mNewTrackIndexes;

    std::vector<shared_ptr<IMediaFile> > mPreExistingMediaFiles;

    /** -------------------------------------------------------------------- */
    /**                     config parameters                                */

    uint32_t                    mLowWatermarkLevel;
    uint32_t                    mBufferingProgressUpdateInterval; // percentage to update buffering progress
    uint32_t                    mMediaBufferHighTargetAvailableSize;
    uint32_t                    mAudioBufferHighTargetAvailableSize;
    uint32_t                    mMediaBufferLowTargetAvailableSize;
    // minimum download size when the player resumes downloading
    uint32_t                    mMinDownloadSizeAtResume;
    ullong                      mAccumulatedBytesDownloaded;

    uint32_t                    mMaxStreamingSkew;

    /**                     config parameters                                */
    /** -------------------------------------------------------------------- */

    // parameters to control status update interval
    uint32_t                    mStatusUpdateInterval;
    AseTimeVal             mNextStatusUpdateTime;

    uint32_t                    mPrevBufferLen;

    int32_t                     mPredictedBufferingTime;
    int32_t                     mInitBandwidth;

    int32_t                     mNumOfPlayAttempts;
    uint32_t                    mPredictedRebuffers;

    volatile int32_t            mVideoDlTrackId;
    volatile int32_t            mAudioDlTrackId;
    volatile int32_t            mHeadersDlTrackId;

    bool                        mVideoTrackDownloaderPaused;

    enum InternalStreamingState { DOWNLOADING,
                          DRAINING } mInternalStreamingState;

    bool                        mPaused;

    // a map storing all streams' duration
    std::map<std::string, uint32_t> mStreamDurations;

    volatile bool               mLicenseAcquired;

    volatile bool            mFastStart;
    bool                     mWaitForManifest;
    uint32_t                 mContentDuration;

    BufferingStats          mBufferingStats;
    /** maximum buffering time */
    ullong                 mPrebufferingStartTime; // time when buffering starts
    uint32_t                 mPrebufferTimeLimit; // maximum buffer time in ms
    uint32_t                 mMinPrebufLength;

    uint32_t                 mConfigedVideoBufferSize;
    uint32_t                 mConfigedAudioBufferSize;

    uint32_t                 mBufferThresholdForShortSleep;
    uint32_t                 mShortSleepForPerfDevice;
    bool                     mPauseStreamingAfterBufferingComplete;
    bool                     mHasSleptForPerfDevice;
    AseTimeVal          mSleepStartTime;
    uint32_t                 mSleepCounter;

    shared_ptr<StreamingCommand> mUnfinishedStreamingCommandPtr;

    mutable Mutex mCmdMutex;
};

typedef MediaPresentation::MediaPresentationPtr MediaPresentationPtr;

inline void MediaPresentation::pause()
{
    mPaused = true;
}

inline void MediaPresentation::resume()
{
    mPaused = false;
}

}}
#endif
