/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MCPL_TRACK_DOWNLOADER_2_H
#define MCPL_TRACK_DOWNLOADER_2_H

#include <map>
#include <list>

#include "AseCommonDataTypes.h"
#include "BaseTrackDownloader2.h"
#include "ByteRangeDownloader.h"
#include "UrlRedirectMap.h"

namespace netflix {
namespace ase {

/**
 *  @class McplTrackDownloader2
 *
 *  @brief This class is the multi connection pipelined track downloader class.
 */
class McplTrackDownloader2 : public BaseTrackDownloader2
{
public:
    typedef shared_ptr<McplTrackDownloader2> McplTrackDownloader2Ptr;

public:
    /** Constructor. */
    McplTrackDownloader2(
                    uint32_t trackId,
                    MediaType mediaType,
                    TrackConfiguration const& config,
                    HttpRequestManagerPtr pHttpRequestManager,
                    IAsyncHttpClientPtr pAsyncHttpClient,
                    ITransportReporterPtr pTransportReporter,
                    IAseConfigParameterPtr pAseConfigParameter,
                    UrlRedirectMapPtr pUrlRedirectMap );

    /** Destructor. */
    virtual ~McplTrackDownloader2();

    /** Re-configure the track downloader */
    virtual int32_t configure( IAseConfigParameterPtr pAseConfigParameter,
                               TrackConfiguration const& config );

    /** Init the track downloader. */
    virtual int32_t init();

    /** Deinitialize the track downloader. */
    virtual int32_t deinit();

    /** Test whether this downloader is in initializing state */
    virtual bool idle() const;

    /** Get the destination host/port that this downloader is working on */
    virtual AseUrl::HostPort getDestination() const;

    /** Get the next timeout time. */
    virtual AseTimeVal getNextTimeout();

    /** Process selected connection. */
    virtual int32_t processSelectedHttpConnections(
                            const AseTimeVal& timeNow,
                            std::vector<IAsyncHttpConnectionPtr>& selectedHttpConnections,
                            bool& dataReceived,
                            bool& networkErrors );

    /** Process timeout. */
    virtual int32_t processTimeout( const AseTimeVal& timeNow );

    /** Pause downloading. */
    virtual int32_t pauseDownloading( const AseTimeVal& timeNow );

    /** Resume downloading. */
    virtual int32_t resumeDownloading( const AseTimeVal& timeNow );

    /** Check for URL changes on the frontmost task */
    virtual bool checkForUrlChanges();

    /** Notify the downloader that a new task is waiting */
    virtual void notifyNewTaskIsWaiting();

    /** ByteRangeDownloaderEventListener functions. */
    virtual void reportHttpResponseTime(
                    uint32_t byteRangeDownloaderId,
                    const AseTimeVal& httpResponseTime );

protected:
    /** Multi connection pipelined track downloader states. */
    typedef enum
    {
        UNINITIALIZED_STATE = 0,         //Unconfigured state
        INIT_STATE,                     //Initial, configured state.
        WAITING_FOR_URL_REQUEST_STATE,  //Waiting for URL requests.
        CONNECTING_STATE,               //Establishing connections to new domain
        WORKING_STATE,                  //Working state, receiving bytes for URL requests.
        WAITING_FOR_MEDIA_BUFFER_STATE  //Waiting for media buffer.
    } State;

    /** A record for keeping track of URL request tasks for
     *  a URL (the original URLs, not redirect URLs).
     */
    class UrlRecord
    {
    public:
        /** Constructor. */
        UrlRecord();

        /** Check if the URL record is empty. */
        bool empty();

        /** Reset the record. */
        void reset();

    public:
        AseUrl mUrl;
        uint32_t    mUrlReqeustTaskCount;
        int32_t     mStatus;
        uint32_t    mHttpResponseCode;
    };

    /** The URL record list. */
    typedef std::list<UrlRecord> UrlRecordList;

    /** The scheduling bucket. */
    class SchedulingBucket
    {
    public:
        ByteRangeDownloaderPtr mByteRangeDownloaderPtr;
        ullong mAverageThroughput;
        ullong mByteCountToTarget;
    };

    /** The scheduling bucket list. */
    typedef std::list<SchedulingBucket> SchedulingBucketList;

protected:
    /** Transit to a new state */
    void transitState( const AseTimeVal& timeNow, State newState );

    /** Connect byte range downloaders
     *  @return true if all byte range downloaders were disconnected
     */
    bool connectByteRangeDownloaders( const AseTimeVal& timeNow );

    /** Disconnect byte range downloaders. */
    void disconnectByteRangeDownloaders( const AseTimeVal& timeNow );

    /** Disconnect byte range downloaders serving abandoned tasks. */
    void disconnectAbandonedByteRangeDownloaders( const AseTimeVal& timeNow );

    /** Set the configuration for a byte range downloader */
    void configureByteRangeDownloader( ByteRangeDownloaderPtr pByteRangeDownloader );

    /** Return all existing URL request tasks back to URL requuest manager. */
    void returnAllUrlRequestTasks();

    /** Return abandoned URL request tasks back to URL requuest manager. */
    void returnAbandonedUrlRequestTasks();

    /** Check for and close a slow connection */
    void closeSlowConnection( AseTimeVal const& timeNow );

    /** Get byte ranges for scheduling
     *  @param unscheduledBytes [in/out] Total number of unscheduled bytes available
     */
    int32_t getByteRanges( ullong& unscheduledBytes );

    /** Schedule requests for byte ranges. */
    int32_t scheduleByteRanges( const AseTimeVal& timeNow );

    /** Add a bucket to the scheduling list */
    void addBucketToSchedulingList(
                    SchedulingBucketList& bucketList,
                    SchedulingBucket& bucket );

    /** Update the URL record list. */
    int32_t updateUrlRecordListOnTaskComplete(
                    int32_t retVal,
                    uint32_t& httpResponseCode,
                    UrlRequestTaskPtr pUrlRequestTask );

    /** Return the byte ranges from a ByteRangeDownloader to the UrlRequestTask */
    void returnByteRangeReceivingTasks( ByteRangeDownloaderPtr pByteRangeDownloader );

    /** Process the connection failure from a byte range downloader .*/
    int32_t processConnectionFailure(
                        const AseTimeVal& timeNow,
                        ByteRangeDownloaderPtr pByteRangeDownloader );

    /* Report fatal request errors to all requests with the same URL */
    void reportErrorToUrlRequestTasks(  AseUrl const&   url,
                                        AseErrorCode    status,
                                        uint32_t        httpCode );

    /** Remove a completed URL request task
     *  @param pUrlRequestTask [in] The completed task
     *  @return true if it was removed, false if it wasn't found (meaning it
     *               was removed earlier but is hanging around due to duplicated
     *               requests)
     */
    bool removeCompletedUrlRequestTask(
                        UrlRequestTaskPtr pUrlRequestTask );

    /** Get the earliest byte range downloader timeout. */
    AseTimeVal getEarliestByteRangeDownloaderTimeout();

    /** Compute the next timeout time. */
    void computeNextTimeout();

    /** Count the number of currently active connections */
    uint32_t countActiveConnections() const;

    /** Count the number of unscheduled bytes */
    ullong countUnscheduledBytes() const;

    /** Fnd a Url Record */
    UrlRecordList::iterator findUrlRecord( AseUrl const& url );

    /** Get the name of a state. */
    const char* getStateName( State state ) const;

    /** Debug print of the UrlRecordList */
    void debugPrintUrlRecordList() const;

protected:

    /** The state. */
    State mState;

    /** The URL request task. */
    UrlRequestTaskList mUrlRequestTaskList;

    /** The original destination, before redirects */
    AseUrl::HostPort mOriginalDestination;

    /** The current connected destination. */
    AseUrl::HostPort mCurrentConnectedDestination;

    /** The target number of connections. */
    uint32_t mTargetConnectionCount;

    /** Time at which we entered the current state */
    AseTimeVal mCurentStateTime;

    /** Time at which we entered the CONNECTING state */
    AseTimeVal mConnectingTime;

    /** The byte range downloader map. */
    ByteRangeDownloaderMap mByteRangeDownloaderMap;

    /** The ID of the byte range downloader that is
     *  blocked to get a fresh HTTP response time.
     */
    uint32_t mBlockedByteRangeDownloaderId;

    /** The last measured HTTP response time. */
    AseTimeVal mLatestHttpResponseTime;

    /** The time when the latest measured HTTP response time is updated. */
    AseTimeVal mHttpResponseTimeUpdatedTime;

    /** The URL record list that keeps track of different URLs
     *  the track downloader is working on.
     */
    UrlRecordList mUrlRecordList;

    /** The flag indicates that the track downloader is blocked for getting
     *  more URL request tasks.
     */
    bool mGettingUrlRequestTasksBlockedFlag;

    /** The flag indicates that the media buffer is available. */
    bool mMediaBufferAvailableFlag;

    /** True if connections have been disconnected whilst idle */
    bool mDisconnectedWhileIdle;

    /** The reported total number of broken connections. */
    uint32_t mTotalConnectionBrokenCount;

    /** The average number of completed HTTP requests when
     *  connections are broken.
     */
    uint32_t mAverageCompletedHttpRequestCount;


    /** Configuration parameters. */
    uint32_t mMinResponseTime;
    uint32_t mResponseTimeFreshnessDuration;

    uint32_t mSchedulingTargetMultiplyFactor;
    uint32_t mSchedulingTriggerMultiplyFactor;

    uint32_t mTargetScheduleTime;

    uint32_t mMinRequestSize;

    uint32_t mPipeliningStabilityRequestThreshold;
    uint32_t mPipeliningStabilityConnectionThreshold;

    AseTimeVal mMinimumDuplicateTimeout;
    uint32_t mDuplicateTimeoutMultiplier;

    uint32_t mSlowConnectionThreshold;

    AseTimeVal mMinSlowConnectionDuration;

    bool mCheckBlockDeadlineForSlowConnections;

    uint32_t mByteRangeAlignment;

    /** Multi-purpose timers */
    AseTimeVal mTimerOne;
    AseTimeVal mTimerTwo;

    /** The next timeout time. */
    AseTimeVal mNextTimeout;
};

typedef McplTrackDownloader2::McplTrackDownloader2Ptr McplTrackDownloader2Ptr;

}}
#endif //MCPL_TRACK_DOWNLOADER_2_H
