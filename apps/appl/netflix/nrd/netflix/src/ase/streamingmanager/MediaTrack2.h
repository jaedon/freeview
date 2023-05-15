/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MEDIA_TRACK_H
#define MEDIA_TRACK_H

#include <deque>

#include <nrdase/AseCommonDataTypes.h>
#include "MediaPresentation2.h"
#include <nrdase/ITrackInfo.h>
#include <nrdase/IMediaSink.h>

#define TEST_CANCEL_BLOCK

namespace netflix {
namespace ase {

class MediaStream;
class MediaFragment;
class TrackBufferInfo;
class MediaPresentationRequest;

typedef shared_ptr<MediaStream> MediaStreamPtr;
typedef shared_ptr<MediaFragment> MediaFragmentPtr;
typedef shared_ptr<TrackBufferInfo> TrackBufferInfoPtr;
typedef shared_ptr<MediaPresentationRequest> MediaPresentationRequestPtr;

struct NextManifestTrackInfo
{
    void reset()
    {
        pManifestTrack = ManifestTrackPtr();
        manifestId = -1;
        trackId = -1;
    }

    ManifestTrackPtr pManifestTrack;
    int32_t          manifestId;
    int32_t          trackId;
};

/**
 * @class MediaTrack
 * @brief Class that handles an active track
 *
 * The MediaTrack object maintains the state for a single track and interacts
 * with the MediaStream objects. It maintains a list of MediaFragment objects
 * corresponding to the sequence of fragments for this track which have
 * requests in progress (i.e. the sequence for which a decision has been made).
 *
 *
 */
class MediaTrack : public ITrackInfo,
                   public enable_shared_from_this<MediaTrack>
{
public:
    typedef shared_ptr<MediaTrack> MediaTrackPtr;

    /** construct the media track object
     * @param pMediaPresentation [in] The parent media presentation object.
     * @param pManifestTrack [in] The track manifest.
     * @param pMediaSink [in] The media sink.
     * @param trackId [in] the unique id for the track
     * @param pConfigParameters [in] Configuration parameters
     * @param pMediaTrack [out] The new media track object.
     * @return 0 success, negative error codes to be defined
     */
    static AseErrorCode construct( shared_ptr<MediaPresentation>            pMediaPresentation,
                          ManifestTrackPtr             pManifestTrack,
                          bool                         fastStart,
                          IMediaSinkPtr const&         pMediaSink,
                          int32_t                      manifestId,
                          int32_t                      trackId,
                          int32_t                       downloaderId,
                          IMediaBufferPoolPtr const&   pMediaBufferPool,
                          IStreamingReporterPtr const& pStreamingReporter,
                          ILocationSelectorPtr const&  pLocationSelector,
                          IAseConfigParameterPtr        pConfigParameters,
                          IAseConfigParameterPtr        pLocalConfigParameter,
                          std::vector<shared_ptr<IMediaFile> > const& mediaFiles,
                          MediaTrackPtr&                pMediaTrack);

    void configure( IAseConfigParameterPtr pConfigParameters );

    /**
     * Migrate this media track to use a different manifest track
     */
    AseErrorCode migrate();

    void videoEndOfTrack();

    AseErrorCode scheduleMigrate(ManifestTrackPtr pManifestTrack,
                         int32_t manifestId,
                         int32_t trackId);

    void notifyLicenseAcquired();
    // whether the track is ready for manifest migration
    bool readyForMigration() const;

    /**
     * return the manifest track of the media track
     */
    ManifestTrackPtr getManifestTrack() const;


    /** Get a new request object */
    AseErrorCode getNextRequest(MediaPresentationRequestPtr& pRequest);

    // ITrackInfo public interfaces
    /** Get the unique id */
    virtual int32_t trackId() const;
    /** Get the media type */
    virtual MediaType mediaType() const;
    /** An iterator to the current selected stream */
    virtual const_iterator begin();
    /** An iterator to the stream after the last one */
    virtual const_iterator end();
    /** the iterator to the current selected stream */
    virtual const_iterator current();
    virtual bool isCompleted();

    // completed scheduing downloading for this track
    void complete();

    /** obtain the track buffer info */
    ITrackBufferInfoPtr trackBufferInfo() const;

    /** update the current streaming time from media presentation */
    AseErrorCode setStreamingTime( AseTimeStamp const& streamingTime, AseTimeStamp& newStreamingTime );

    /** update the current playback time, and detaled track info accordingly */
    void updatePlaybackTime( AseTimeStamp const& playbackTime );

    int getPlaybackBitrate();
    int getStreamingBitrate();

    AseTimeStamp playbackTime() const { return mCurrentPlaybackTime; }
    AseTimeStamp streamingTime() const { return mCurrentStreamingTime; }

    /** obtain the buffering length for this track
     * @param[out] completedBuffer: computed buffer length
     * @param[out] pendingBuffer: the length of pending buffer requested
     */
    void obtainBufferingLen(uint32_t& completedBuffer, uint32_t& pendingBuffer) const;

    /** obtain the number of non empty downloaded fragments */
    uint32_t numNonEmptyFragments() const;
    void setNumNonEmptyFragments(uint32_t numOfBlocks);

    /** Provide a media fragment from the media stream */
    void provideMediaFragment( MediaFragmentPtr const& pMediaFragment );

    void popMediaFragment();


    void reportFailure(AseErrorCode errorCode, uint32_t brokenStreamId, uint32_t fragmentId);


    AseTimeStamp seekTime() const
    {
        return mMediaPresentationPtr.lock()->seekTime();
    }

    void reset();

    int32_t brokenStreamId() const;

    void updateContentDuration(uint32_t duration);

    /**
     * just completed forwarding a fragment of the provided duration
     */
    void completeForwarding(AseTimeStamp const& fragmentStartPts,
            AseTimeStamp const& duration,
            uint32_t numOfBlocks, bool lastFragment);

    /**
     * Whether the track has a media header to download
     * @param[in] throughput: It does not need to download the header if the average bitrate is
     *                        higher than the throughput
     * @param[out] streamId: the id of the selected stream
     */
    bool hasHeaderToDownload( uint32_t throughput, int32_t& streamId );

    bool endOfTrack() { return mCompleted || mEndOfTrack; }

    void decrBufferSize(uint32_t size);

    uint32_t bufferSize() { return mMediaBufferSize; }

    void reportNewStreamDuration(const std::string& streamKey, uint32_t duration);

    void setVideoBitrateRanges(std::vector< shared_ptr<BitrateRange> > const& videoBitrateRanges);
    void selectStreams(std::vector<uint32_t> const& streamIndexs);

    bool hasPendingTasks() const;
    bool hasPendingRequest() const;

    void setDownloadTrackId(int32_t trackId);

    bool probingStream() const;

    // deliver the header to media sink
    bool deliverHeader();

    /** update the streaming time of all streams */
    AseTimeStamp updateStreamingTimeInStreams( AseTimeStamp const& streamingTime );

    void startSwitch(AseTimeStamp const& playbackTime);
public:
    virtual ~MediaTrack() {}

protected:
    MediaTrack( shared_ptr<MediaPresentation>              pMediaPresentation,
                ManifestTrackPtr                pManifestTrack,
                bool                            fastStart,
                IMediaSinkPtr const&            pMediaSink,
                IMediaBufferPoolPtr const&      pMediaBufferPool,
                IStreamingReporterPtr const&    pStreamingReporter,
                ILocationSelectorPtr const&  pLocationSelector,
                IAseConfigParameterPtr          pConfigParameters,
                IAseConfigParameterPtr          pLocalConfigParameter,
                int32_t                         manifestId,
                int32_t                         trackId,
                int32_t                         downloaderId,
                std::vector<shared_ptr<IMediaFile> > const& mediaFiles);

    // ITrackInfo protected interfaces
    virtual const_iterator& increment( const_iterator& it );
    virtual const_iterator& decrement( const_iterator& it );
    virtual IStreamInfoPtr dereference( const_iterator& it );
    virtual bool equal( const_iterator const& lhs, const_iterator const& rhs );
    virtual bool hasNext( const_iterator const& it );
    virtual bool hasPrevious( const_iterator const& it );

    // whether the index is a valid index for mMediaStreams
    bool isValid(int index) const;
    bool hasHeaderToDownload( uint32_t throughput, int32_t& streamId, uint32_t startIndex );

    int32_t getNextAvailableIndex( int32_t index );
    int32_t getPrevAvailableIndex( int32_t index );

    AseErrorCode getNextRequestWithoutSeekTime(MediaPresentationRequestPtr& pRequest);
    AseErrorCode requestFromCurrentStream(MediaPresentationRequestPtr& pRequest, bool& pendingRequest);
    AseErrorCode getNextHeaderRequest(MediaPresentationRequestPtr& pRequest);
    AseErrorCode formHeaderRequest(MediaPresentationRequestPtr& pRequest);
    AseErrorCode formMediaRequest(MediaPresentationRequestPtr& pRequest);
    bool tooMuchPendingBuffer();
    MediaStreamPtr findStreamUsingId(int32_t streamId, int32_t& updatedStreamIndex);
    MediaStreamPtr findAlternativeValidStream(int32_t& updatedStreamIndex);

    // functions for cancel pending requests feature
    void streamingHealthCheck();
    MediaFragmentPtr needToAbandonPendingRequest();
    bool needToAbandon(MediaFragmentPtr pFirstFragment, MediaFragmentPtr pLastFragment,
                        uint32_t completedBuffer, uint32_t minRequiredBuffer);
    bool allPendingRequestsAtLowest();
    void abandonPendingRequests(MediaFragmentPtr pMediaFragment);
    // find the media fragment using streamId and fragmentId
    MediaFragmentPtr findFragmentInQueue(uint32_t streamId, uint32_t fragmentId);

    AseErrorCode initialize();

    // Internal functions for McQueen
    bool checkForMigration();
    AseErrorCode getDrmHeaderRequest(MediaPresentationRequestPtr& pRequest);
    // The player waits for the drm manifest for the following reasons
    // 1. The seek time is within [duration - 30 seconds, duration] of the non-drm manifest;
    // 2. The player has played the non-drm clip too long
    bool waitForDrmManifest();

    shared_ptr<IMediaFile> findPreExistingMediaFile(
                        std::vector<shared_ptr<IMediaFile> > const& mediaFiles,
                        std::string const& streamKey, int32_t streamId);

    void getConfiguration();

protected:

    weak_ptr<MediaPresentation>     mMediaPresentationPtr;
    ManifestTrackPtr                mManifestTrackPtr;

    IMediaSinkPtr                   mMediaSinkPtr;
    std::vector<MediaStreamPtr>     mOldMediaStreams;
    std::vector<MediaStreamPtr>     mMediaStreams;

    TrackBufferInfoPtr                mTrackBufferInfoPtr;

    IMediaBufferPoolPtr             mMediaBufferPoolPtr;
    IAseConfigParameterPtr          mConfigParameterPtr;
    IAseConfigParameterPtr          mLocalConfigParametePtr;

    /** The queue of buffered media fragments for this track */
    std::deque<MediaFragmentPtr>         mMediaFragments;
    /** the first fragment that has not been fully downloaded and delivered */
    MediaFragmentPtr                mFirstPendingFragment;
    AseTimeVal                      mPreviousBlockAbandonTime;
    uint32_t                        mAbandonBlockGuardInterval;
    // used media buffer size
    ullong                        mMediaBufferSize;
    // total media buffer size
    ullong                        mTotalMediaBufferSize;
    ullong                        mAvailableMediaBufferSize;
    IStreamingReporterPtr           mStreamingReporterPtr;
    ILocationSelectorPtr            mLocationSelectorPtr;
    weak_ptr<MediaStream>  mPreviousMediaStream;
    // the index for the current stream being used. It can be
    // selected by the selector, or selected for downloading the header
    int32_t mIndexForCurrentStream;
    int32_t mIndexForPreviousStream;
    int32_t mIndexForStreamDownloadingHeaders;

    uint32_t mCachedThroughput;
    uint32_t mLongtermBw;

    int32_t mSelStreamIndex; // the selected stream's index
    int32_t mIndexForPreviousHeaderDownloading; // stream index for previous header downloading
    uint32_t mBrokenStreamId;
    bool    mAllHeadersDownloaded;
    bool    mFirstIndexDownloaded;
    bool    mHeaderDelivered;
    bool    mSwitchingTrack;        // for on-the-fly audio switch only
    ByteRange                       mHeaderByteRangeHint;

    AseTimeStamp                    mCurrentPlaybackTime;
    AseTimeStamp                    mCurrentStreamingTime;

    int32_t                         mManifestId;
    int32_t                         mTrackId;
    volatile int32_t                mDownloaderTrackId;  // track id of the track downloader
    MediaType                       mMediaType;

    uint32_t                        mDuration;
    AseTimeStamp                    mPendingBufferLen; // ms

    bool                            mCompleted;

    uint32_t                        mDlNonEmptyBlocks;

    bool                            mEndOfTrack;

    uint32_t                        mRebuffer;

    uint32_t                        mMaxPendingBufferPercentage;
    uint32_t                        mMaxPendingBufferLen;
    uint32_t                        mMinPendingBufferLen;
    uint32_t                        mMinBufferLenForHeaderDownloading;
    // maximum playable length for the non-drm content
    uint32_t                        mMaxNonDrmContentPlayLen;

    uint32_t                        mGuardDurationForFirstManifest;

    uint32_t                        mRequiredBufferingLen;

    uint32_t                        mLowWatermarkLevel;

    uint32_t                        mAbandonThreshold;
    // The minimum size of a media request. The media request should be for
    // the smallest number of blocks that together exceed this size and smaller than remaining buffer size,
    // or the mininum number of prebuffer blocks during buffering, whichever is smaller.
    uint32_t                        mMinRequestSize;

    bool                            mProbingStream;


    bool                            mEnableAbandonBlock;

    int32_t                         mMinAcceptableVideoBitrate;

    // parameters for McQueen
    bool                            mLicenseAcquired;
    bool                            mFastStart;
    // temporary track for downloading the drm header
    MediaTrackPtr                   mTmpTrack;
    NextManifestTrackInfo           mNextManifestTrackInfo;
    // Just finish of pending request during the paused state. Prepare for the migration to the drm manifest
    bool                            mPaused;


    std::vector< shared_ptr<BitrateRange> > mVideoBitrateRanges;

    std::vector< shared_ptr<IMediaFile> > mMediaFiles;

    uint32_t                       mLowestBitrate;

    uint32_t                       mForwardingCheckInterval;
    AseTimeVal                mNextTimeToCheckForwarding;


    uint32_t                       mNumOfPlayCompleted;

    bool                           mPendingMediaFile;
};

typedef MediaTrack::MediaTrackPtr MediaTrackPtr;

}}
#endif
