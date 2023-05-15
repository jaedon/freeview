/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SIMPLE_STREAM_SELECTOR_H
#define SIMPLE_STREAM_SELECTOR_H

#include <nrdase/IStreamSelector.h>
#include <nrdase/ILocationSelector.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdase/INetworkMonitor.h>
#include <nrdase/AseCommonDataTypes.h>

#include <nrdase/DebugLogging.h>
#include <nrdase/Manifest.h>

#include <nrdbase/Variant.h>

#include <deque>
#include <vector>

namespace netflix {
namespace ase {

DECLARE_LOGGER( SimpleStreamSelectorLog );

struct TrackMetaInfo
{
    int32_t mTrackId;
    MediaType mMediaType;
    AseTimeInterval mCompleteBufferLen;
    AseTimeInterval mInCompleteBufferLen;
    AseTimeStamp streamingTime; //  start pts of the next fragment to download
};

/**
 * @class SimpleStreamSelector
 * @brief A reference implementation for IStreamSelector
 */
class SimpleStreamSelector : public IStreamSelector
{
public:
    typedef shared_ptr<SimpleStreamSelector> SimpleStreamSelectorPtr;

    SimpleStreamSelector( IAseConfigParameterPtr pConfigParameter,
                          IAseConfigParameterPtr pLocalConfigParameter,
                          INetworkMonitorPtr pNetworkMonitor,
                          ILocationSelectorPtr pLocationSelector);

    virtual void configure( IAseConfigParameterPtr pConfig );

    virtual BufferingStatus selectStream(
                                    bool downSwitchFreeze,
                                    int32_t trackId,
                                    double currentPlayoutRate,
                                    AseTimeStamp const& currentPlayoutTime,
                                    std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                                    ullong availableBuffer,
                                    std::vector<ITrackInfoPtr> const& trackFutureInfo,
                                    StreamFeasibility& streamFeasibility,
                                    int32_t& streamId,
                                    int32_t& numOfBlocks,
                                    int32_t& predictedDlTimeForBackupStream,
                                    uint32_t& predictedRebuffer);

    virtual void  selectStream(int32_t trackId,
                               std::vector<ManifestTrackPtr> const& trackList,
                               int32_t& streamIndex,
                               uint32_t& numberOfPredictedRebuffers);

    virtual void stop();

    virtual void skip();

    virtual void obtainBufferingStats( uint32_t& requiredBufferingLength, uint32_t& estimatedBufferingTime );
    virtual void getStreamingQuality( bool& goodQuality );

    ~SimpleStreamSelector() {}

protected:
    enum SwitchState
    {
        NO_ACTION,
        UP_SWITCH,
        DOWN_SWITCH
    };

    enum StreamSelectorCode
    {
        SEL_OK,
        SEL_NOT_EXISTS
    };

    enum HeuristicsState
    {
        STREAMING_STARTUP,
        STABLE_STREAMING,
        UNSTABLE_STREAMING
    };

    static const int32_t  MAX_INT32            = 0x7FFFFFFF;
    static const uint32_t MAX_UINT32           = 0xFFFFFFFF;
    static const uint32_t MAX_BW_MARGIN        = 100;
    static const uint32_t MIN_UINT32_GENERIC   = 1000;
    static const int32_t  MIN_INT32_GENERIC    = 1000;
    static const uint32_t MIN_HIGH_STREAM_WINDOW = 3000;
    static const uint32_t DURATION_FOR_PREDICT_REBUFFER = 3600 * 1000;
    static const uint32_t MIN_THROUGHPUT = 50;

    // default values for streaming config
    static const uint32_t defaultMaxBufferingTime = 2000;
    static const uint32_t defaultMaxBufferingAmountAllowed = 60 * 1000;
    static const uint32_t defaultBandwidthMargin = 10;

    static const uint32_t defaultLowestWatermarkLevel = 6000;
    static const uint32_t defaultHighWatermarkLevel = 30000; // 30 seconds
    static const uint32_t defaultWaterMarkLevelForSkipStart = 8000; // 8 seconds
    static const uint32_t defaultLockPeriodAfterDownswitch = 15 * 1000;
    static const uint32_t defaultMaxSimuLength = 30 * 3600 * 1000; // 30 hours
    static const uint32_t defaultHighStreamRetentionWindow = 90 * 1000;
    static const uint32_t defaultHighStreamTransitionWindow = 600 * 1000;
    static const uint32_t defaultMinRequiredBuffer = 30 * 1000;
    static const uint32_t defaultDrainBufferSize = 256 * 1024;
    static const uint32_t defaultTotalBufferSize = 32 * 1024 * 1024;
    // max buffering time, when predicting no rebuffering ahead
    uint32_t mMaxBufferingTime; // ms
    // movie length in ms. max buffering amount, even if predicting a rebuffering event ahead
    uint32_t mMaxBufferingAmountAllowed;
    uint32_t mBandwidthMargin;
    uint32_t mMinPrebufLength; // movie length in ms

    uint32_t mLowestWatermarkLevel;
    uint32_t mLowWatermarkLevel;
    uint32_t mHighWatermarkLevel;
    uint32_t mWatermarkLevelForSkipStart;
    uint32_t mLockPeriodAfterDownswitch;

    uint32_t mBitrateThresholdForBufferBasedHeuristics;

    uint32_t mMaxSimuLength; // maximum simulation length

    uint32_t mThroughputProbingEnterThreshold;

    uint32_t mHighStreamRetentionWindow;
    uint32_t mHighStreamTransitionWindow;

    // minimum/maximum init video bitrate for A/B testing
    int32_t mMinInitVideoBitrate;
    int32_t mMaxInitVideoBitrate;

    int32_t mMinAcceptableVideoBitrate;

    double   mThroughputLowestConfidenceLevel;
    uint32_t mThroughputPercentForAudio;

    uint32_t mConfigedMinConnectTime;

    // whether to skip bitrate during upswitch
    bool     mSkipBitrateInUpswitch;

    bool     mAllowBitrateProbingUnknownLocation;

    uint32_t mMinRequiredBuffer;

    uint32_t mDrainBufferSize;
    uint32_t mTotalBufferSize;

    uint32_t mPenaltyFactorForLongConnectTime;
    uint32_t mLongConnectTimeThreshold;
    uint32_t mAdditionalBufferingPerFailure;
    uint32_t mAdditionalBufferingLongConnectTime;

    /** parameters for the 3-phase algorithm */
    uint32_t mToStableThreshold;
    uint32_t mToUnstableThreshold;
    uint32_t mLowestBufForUpswitch;

    int mFeasibilitySleepMicros;

    std::map<uint32_t, uint32_t> mBandwidthMarginMap; // buffer length --> bandwidth margin
    std::map<uint32_t, uint32_t> mInitialBitrateSelectionCurve; // bitrate --> buffering time

    // the tolerance for the initial bitrate selection given a pre-selected stream
    uint32_t mInitSelectionLowerBound;
    uint32_t mInitSelectionUpperBound;

    /**
     * scan trackCurrentInfo to obtain the following summary information
     * 1. buffer length(ms)/size(bytes) per track;
     * @param[in] currentPlayoutTime The current playout time
     * @param[in] trackCurrentInfo Buffer state of currently playing tracks
     * @return a list of each track's meta buffer info
     */
    std::vector<TrackMetaInfo> scanTrackInfo(
                                AseTimeStamp const& currentPlayoutTime,
                                std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo );
    AseTimeInterval calculateBufferLen( std::vector<TrackMetaInfo> const& trackMetaList );

    /** update current streaming state
     * @param[in] currentPlayoutRate The current playout rate
     */
    void updateCurrentStreamingState( double currentPlayoutRate );

    /**
     * whether the provided track is completed
     */
    bool completeTrack( int32_t trackId,
                        std::vector<ITrackInfoPtr> const& trackFutureInfo );

    /**
     * estimate the number of future rebuffers
     * @param[in] bandwidth: kbps
     * @param[in] avgBitrate: average bitrate kbps
     * @param[in] duration: playback duration ms
     * @param[in] maxBufferingLen: maximum buffering length ms
     */
    uint32_t estimateNumberOfRebuffers(uint32_t bandwidth,
                                       uint32_t avgBitrate,
                                       uint32_t duration,
                                       uint32_t maxBufferingLen);

    /**
     * select the media stream for buffering
     * @param[in] trackId: id of the selected track
     * @param[in] currentPlayoutTime The current playout time
     * @param[in] trackMetaList a list of each track's meta buffer info
     * @param[in] trackCurrentInfo Buffer state of currently playing tracks
     * @param[in] trackFutureInfo Future data rate profiles of currently playing tracks
     * @param[in] availableBuffer available buffer in bytes
     * @param[out] streamId: id of the selected stream, -1 is invalid id
     * @param[out] numOfBlocks: suggested number of blocks to be downloaded
     */
    BufferingStatus selectStreamForBuffering( int32_t trackId,
                                   AseTimeStamp const& currentPlayoutTime,
                                   std::vector<TrackMetaInfo> const& trackMetaList,
                                   std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                                   std::vector<ITrackInfoPtr> const& trackFutureInfo,
                                   ullong availableBuffer,
                                   StreamFeasibility& streamFeasibility,
                                   int32_t& streamId,
                                   int32_t& numOfBlocks,
                                   uint32_t& predictedRebuffer);

    /**
     * select the media stream duing the playback
     */
    BufferingStatus selectStreamDuringPlayback( bool downSwitchFreeze, int32_t trackId,
                                     double currentPlayoutRate,
                                     AseTimeStamp const& currentPlayoutTime,
                                     std::vector<TrackMetaInfo> const& trackMetaList,
                                     std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                                     ullong availableBuffer,
                                     std::vector<ITrackInfoPtr> const& trackFutureInfo,
                                     StreamFeasibility& streamFeasibility,
                                     int32_t& streamId,
                                     int32_t& numOfBlocks,
                                     int32_t& predictedDlTimeForBackupStream,
                                     uint32_t& predictedRebuffer);

    /**
     * select the first video stream
     */
    BufferingStatus selectFirstVideoStream( ITrackInfoPtr pTrackInfo,
                                            int32_t& streamId,
                                            uint32_t& predictedRebuffer,
                                            ITrackInfo::const_iterator currentStreamIter);

    void selectAudioStream( ITrackInfoPtr pTrackInfo,
                            int32_t& streamId );



    /**
     * select the subsequent video stream during buffering
     */
    BufferingStatus selectVideoStreamForBuffering( ITrackInfoPtr pTrackInfo,
                                                     int32_t& streamId,
                                                     uint32_t& predictedRebuffer,
                                                     int32_t& numOfBlocks,
                                                     AseTimeStamp const& currentPlayoutTime,
                                                     std::vector<TrackMetaInfo> const& trackMetaList,
                                                     std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                                                     std::vector<ITrackInfoPtr> const& trackFutureInfo,
                                                     ullong availableBuffer,
                                                     AseTimeInterval const& minBuffer,
                                                     int32_t goodQualityStreamId);
    /**
     * Tests the feasibility of the mixed stream consisting of [playTime, playTime+highStreamWindow]
     * of the current stream, and [playTime+highStreamWindow, end of the movie] of the lower bitrate stream
     * @param[in] currentBufLevel: current buffer level
     * @param[in] currentPlayoutRate the current playback rate
     * @param[in] currentPlayoutTime the current playback time
     * @param[in] pTrackBufferInfo the current detailed info on buffered data
     * @param[in] currentStreamIterator the stream info iterator pointing at the current stream
     * @param[in] availableBuffer available buffer in bytes
     * @param[in] highStreamWindow
     * @return: stream feasibility
     *
     */
    StreamFeasibility isMixedStreamFeasible(
                                int32_t currentBufLevel, uint32_t audioBitrate,
                                double currentPlayoutRate, AseTimeStamp const& currentPlayoutTime,
                                ITrackBufferInfoPtr pTrackBufferInfo, ITrackInfo::const_iterator currentStreamIterator,
                                ullong availableBuffer, int32_t highStreamWindow);

    /**
     * Tests the feasibility of the mixed stream consisting of [playTime, playTime+highStreamWindow]
     * with both the corresponding location's throughput and the estimated throughput from its neighbors
     */
    StreamFeasibility isMixedStreamFeasibleWrapper(
                                int32_t currentBufLevel, uint32_t audioBitrate,
                                double currentPlayoutRate, AseTimeStamp const& currentPlayoutTime,
                                ITrackBufferInfoPtr pTrackBufferInfo, ITrackInfo::const_iterator currentStreamIterator,
                                ullong availableBuffer, int32_t highStreamWindow);


    /**
     * select a good video stream without skipping bitrates in upswitch
     * @param[in/out] videoStreamIterator:
     *                      [in] current video iterator,
     *                      [out] selected video iterator
     */
    BufferingStatus selectVideoStreamWithoutSkipping(
                        ITrackInfo::const_iterator& videoStreamIterator,
                        StreamFeasibility& streamFeasibility,
                        uint32_t currentBufLevel,
                        uint32_t audioBitrate,
                        double currentPlayoutRate,
                        AseTimeStamp const& currentPlayoutTime,
                        ITrackBufferInfoPtr pVideoTrackBufferInfo,
                        ITrackInfoPtr pVideoTrackInfo,
                        ullong availableBuffer);

    /**
     * select a good video stream with skipping
     * @param[in/out] videoStreamIterator:
     *                      [in] current video iterator,
     *                      [out] selected video iterator
     */
    BufferingStatus selectVideoStreamWithSkipping(
                        ITrackInfo::const_iterator& videoStreamIterator,
                        StreamFeasibility& streamFeasibility,
                        uint32_t currentBufLevel,
                        uint32_t audioBitrate,
                        double currentPlayoutRate,
                        AseTimeStamp const& currentPlayoutTime,
                        ITrackBufferInfoPtr pVideoTrackBufferInfo,
                        ITrackInfoPtr pVideoTrackInfo,
                        ullong availableBuffer);
    /**
     * Compute the required buffering based on the throughput and stream info
     */
    void computeRequiredBuffering( uint32_t throughput, uint32_t connectTime,
                                   AseTimeStamp const& currentPlayoutTime,
                                   ITrackBufferInfoPtr pTrackBufferInfo,
                                   ITrackInfo::const_iterator currentStreamIterator,
                                   ullong availableBuffer, int32_t simuLen,
                                   uint32_t currentBufferingLen,
                                   uint32_t& requiredBufferingLen);
    uint32_t adjustRequiredBufferingLen(IStreamInfoPtr pStreamInfo, uint32_t requiredBufferingLen);

    /**
     * This function simulates the playback process to decide
     * whether the current stream is feasible.
     * A stream is feasible if SimpleStreamSelector does not predict a future rebuffering event,
     * given the throughput, buffer information and et al.
     * @param[in] availableThoughput: available throughput for the simulation
     * @param[in] connectTime: estimated tcp connect time, a.k.a the switch cost
     * @param[in] currentPlayoutRate the current playback rate
     * @param[in] currentPlayoutTime the current playback time
     * @param[in] currentBufLevel: current buffer level
     * @param[in] pTrackBufferInfo the current detailed info on buffered data
     * @param[in] pStreamInfo the current stream
     * @param[in] watermarkLevel
     * @param[in/out] availableBuffer available buffer in bytes
     * @param[in] playbackLen playback length in ms
     * @param[out] newBufLevel the update buffer level
     * @param[in/out] fragmentSizeList: the list stores the sizes the simulator has virtually downloaded
     * @return Stream feasibility
     */
    StreamFeasibility isFeasible( uint32_t availableThoughput, uint32_t connectTime,
                     double currentPlayoutRate, AseTimeStamp const&  currentPlayoutTime,
                     int32_t currentBufLevel,
                     ITrackBufferInfoPtr pTrackBufferInfo, IStreamInfoPtr pStreamInfo,
                     int32_t watermarkLevel, ullong& availableBuffer,
                     int32_t playbackLen, int32_t& newBufLevel,
                     std::deque<uint32_t>& fragmentSizeList);

    /**
     * This function uses nominal bitrate and adjusted throughput to determine whether the stream is feasible
     */
    StreamFeasibility isFeasibleSimple( uint32_t nominalBitrate, std::string const& streamKey, MediaType mediaType );

    StreamFeasibility isFeasibleForFirstStream( uint32_t selectedBitrate, std::string const& streamKey, uint32_t availableThoughput);

    bool isStreamEligible(uint32_t nominalBitrate);

    /**
     * A few rules to judge whether it is eligible to even try to upswitch
     */
    bool qualifyUpswitch( AseTimeStamp const& streamingTime, uint32_t currentBufLevel,
                          IStreamInfoPtr pStreamInfo);

    bool qualifyStreamSwitch( int32_t selStreamIndex, int32_t preSelectedStreamIndex);

    bool canSkipVideoStream(uint32_t bufferLevel);

    uint32_t lookupBandwidthMargin(uint32_t bufferLevel);
    uint32_t lookupMaxBufferingTime(uint32_t bitrate);
    /**
     * Find the stream with the next higher bitrate. (compatibility is enforced outside)
     * @param[in] currentStreamIterator the stream info iterator pointing at the current stream
     * @param[out] newStreamIterator the stream info iterator pointing the new stream
     * @return: OK if finds a new stream
     */
    StreamSelectorCode getStreamWithHigherBitrate( ITrackInfo::const_iterator currentStreamIterator,
                                                  ITrackInfo::const_iterator& newStreamIterator );

    /**
     * Find the stram with the next lower bitrate. (compatibility is enforced outside)
     */
    StreamSelectorCode getStreamWithLowerBitrate( ITrackInfo::const_iterator currentStreamIterator,
                                                 ITrackInfo::const_iterator& newStreamIterator );

    /**
     * separate a/v track info
     */
    void separateAVTrackInfo( std::vector<TrackMetaInfo> const& trackMetaList,
                              std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                              std::vector<ITrackInfoPtr> const& trackFutureInfo,
                              TrackMetaInfo& audioTrackMeta, TrackMetaInfo& videoTrackMeta,
                              ITrackBufferInfoPtr& pAudioTrackBufferInfo,
                              ITrackBufferInfoPtr& pVideoTrackBufferInfo,
                              ITrackInfoPtr& pAudioTrackInfo,
                              ITrackInfoPtr& pVideoTrackInfo );

    /**
     *  adjust collected network statistics
     */
    void obtainNetworkStat( std::string const& streamKey,
                            uint32_t& availableThoughput, uint32_t& connectTime,
                            INetworkMonitor::PredictionSource& predictionSource,
                            uint32_t audioBitrate, uint32_t currentBufLevel);


    /**
     * find the playback and streaming positions in the provided pTrackBufferInfo and pStreamInfo
     * @param[in] currentPlayoutTime the current playback time
     * @param[in] currentBufLevel: current buffer level
     * @param[in] pTrackBufferInfo the current detailed info on buffered data
     * @param[in] pStreamInfo the current stream
     * @param[out] playbackPosIter1: valid if the playback position is in pTrackBufferInfo
     * @param[out] playbackPosIter2: valid if the playback position is in pStreamInfo
     * @param[out] streamingPosIter1: valid if the streaming position is in pTrackBufferInfo
     * @param[out] streamingPosIter2: valid if the streaming position is in pStreamInfo
     */
    bool findPlaybackAndStreamingPositions( AseTimeStamp const& currentPlayoutTime,
                                            AseTimeStamp& currentFragmentStartPts,
                                            int32_t currentBufLevel,
                                            ITrackBufferInfoPtr pTrackBufferInfo,
                                            IStreamInfoPtr pStreamInfo,
                                            ITrackBufferInfo::const_iterator& playbackPosIter1,
                                            IStreamInfo::const_iterator& playbackPosIter2,
                                            ITrackBufferInfo::const_iterator& streamingPosIter1,
                                            IStreamInfo::const_iterator& streamingPosIter2);

    /**
     * simulate playback of one fragment.
     * @param[in] currentPlayoutRate the current playback rate
     * @param[in/out] currentPlayoutTime the current playback time
     * @param[in/out] currentFragmentStartPts the start pts of the current fragment
     * @param[in] pTrackBufferInfo the current detailed info on buffered data
     * @param[in] pStreamInfo the current stream
     * @param[in/out] playbackPosIter1
     * @param[in/out] playbackPosIter2
     * @param[in/out] currentBufLevel current buffer level
     * @param[in/out] availableBuffer current available buffer
     * @param[out] playbackDuration the duration of the playback, which can be different from
     *                  media's fragment duration.
     * @param[in/out] fragmentSizeList: the list stores the sizes the simulator has virtually downloaded
     */
    bool playOneFragment( double currentPlayoutRate, AseTimeStamp& currentPlayoutTime,
                          AseTimeStamp& currentFragmentStartPts,
                          ITrackBufferInfoPtr pTrackBufferInfo, IStreamInfoPtr pStreamInfo,
                          ITrackBufferInfo::const_iterator& playbackPosIter1,
                          IStreamInfo::const_iterator& playbackPosIter2,
                          int32_t& currentBufLevel,
                          ullong& availableBuffer,
                          uint32_t& playbackDuration,
                          std::deque<uint32_t>& fragmentSizeList);

    /**
     * simulate downloading for the provided time
     * @param[in] availableThoughput: available throughput for the simulation
     * @param[in] downloadTime the provided downloading duration
     * @param[in/out] streamingPosIter1
     * @param[in/out] streamingPosIter2
     * @param[in] pTrackBufferInfo
     * @param[in] pStreamInfo the current stream
     * @param[in/out] currentBufLevel current buffer level
     * @param[in/out] availableBuffer current available buffer
     * @param[in/out] bytesInCurrentFragment the downloaded bytes in the current incomplete fragment
     * @param[in/out] fragmentSizeList: the list stores the sizes the simulator has virtually downloaded
     */
    void simulateDownloading( uint32_t availableThoughput, uint32_t downloadTime,
                              ITrackBufferInfo::const_iterator& streamingPosIter1,
                              IStreamInfo::const_iterator& streamingPosIter2,
                              ITrackBufferInfoPtr pTrackBufferInfo, IStreamInfoPtr pStreamInfo,
                              int32_t& currentBufLevel,
                              ullong& availableBuffer,
                              ullong& bytesInCurrentFragment,
                              std::deque<uint32_t>& fragmentSizeList);

    /**
     *
     */
    void searchTrackBufferInfo( AseTimeStamp const& targetTime,
                                AseTimeStamp const& startFragmentPts,
                                ITrackBufferInfo::const_iterator startIter,
                                ITrackBufferInfo::const_iterator endIter,
                                AseTimeStamp& targetFragmentStartPts,
                                ITrackBufferInfo::const_iterator& targetFragmentIter );

    /**
     *
     */
    void searchStreamInfo( AseTimeStamp const& targetTime,
                           AseTimeStamp const& startFragmentPts,
                           IStreamInfo::const_iterator startIter,
                           IStreamInfo::const_iterator endIter,
                           AseTimeStamp& targetFragmentStartPts,
                           IStreamInfo::const_iterator& targetFragmentIter );

    /** obtain the pts of the next undownloaded fragment */
    AseTimeStamp getNextFragmentTime( ITrackInfoPtr pTrackInfo );

    /** load parameters from the configuration service */
    void loadConfigParas();

    /** obtain and validate the config.ed para. */
    uint32_t obtainParameter( std::string const& key, uint32_t min, uint32_t max, uint32_t defaultVal );

    uint32_t obtainThroughput( std::string const& streamKey,
                               uint32_t& audioAllocatedThroughput, uint32_t& videoAllocatedThroughput);

    IAseConfigParameterPtr  mConfigParameterPtr;
    IAseConfigParameterPtr  mLocalConfigParametePtr;
    INetworkMonitorPtr      mNetworkMonitorPtr;
    ILocationSelectorPtr    mLocationSelectorPtr;

    StreamingState mStreamingState;
    bool           mSkipped;

    // previous buffering status
    BufferingStatus mPrevBufferingStatus;
    uint32_t        mPrevPredictBuffer;

    SwitchState mLastSwitchState;
    AseTimeStamp mLastSwitchTime;

    // required buffering length before declaring BUFFER_HAVE_ENOUGH
    uint32_t mRequiredBufferingLen;
    // estimated buffering time in ms
    uint32_t mEstimatedBufferingTime;
    bool     mGoodQuality;

    uint32_t mMediaFragmentDuration;

    HeuristicsState mHeuristicState;
    AseTimeInterval mCurrentBufferLength;
    bool            mInitSelect;

    int32_t mMinAcceptableVideoBitrateDuringPlayback;
    bool    mFirstSelectDuringPlayback; // the first video stream select after the playback starts

    bool            mEnableGoodQualityBuffering;
};

typedef SimpleStreamSelector::SimpleStreamSelectorPtr SimpleStreamSelectorPtr;

}}
#endif
