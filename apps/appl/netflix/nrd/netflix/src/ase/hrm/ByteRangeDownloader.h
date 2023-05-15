/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BYTE_RANGE_DOWNLOADER_H
#define BYTE_RANGE_DOWNLOADER_H

#include <list>
#include <map>
#include <string>

#include "AseCommonDataTypes.h"
#include <nrdnet/IAsyncHttpListeners.h>
#include <nrdnet/IAsyncHttpLib.h>
#include "AverageThroughput.h"
#include "AverageResponseTime.h"
#include "ByteRange.h"
#include "UrlRequestTask.h"
#include <nrdnet/IAsyncHttpListeners.h>
#include <nrdnet/NetCommonDataTypes.h>

namespace netflix {
namespace ase {

class ByteRangeDownloaderBaseState;
class ByteRangeDownloaderDisconnectedState;
class ByteRangeDownloaderWaitingToReconnectState;
class ByteRangeDownloaderConnectingState;
class ByteRangeDownloaderIdleState;
class ByteRangeDownloaderDownloadingState;
class ByteRangeDownloaderSuspendedState;
class ByteRangeDownloaderConnectionDownState;

class ByteRangeDownloaderEventListener;

typedef shared_ptr<ByteRangeDownloaderEventListener> ByteRangeDownloaderEventListenerPtr;

/**
 *  @class ByteRangeReceivingTask
 *
 *  @brief Byte range receiving task.
 */
class ByteRangeReceivingTask
{
public:
    /** Constructor. */
    ByteRangeReceivingTask(
            UrlRequestTaskPtr                   pUrlRequestTask,
            ByteRange const&                    range );

    /** Destructor. */
    virtual ~ByteRangeReceivingTask();

public:
    /** The URL request task. */
    UrlRequestTaskPtr                   mUrlRequestTaskPtr;

    /** The requested byte range. */
    ByteRange                           mByteRange;

    /** The received byte count. */
    ullong                              mReceivedByteCount;

    /** Data received since entering downloading state */
    bool mDataReceivedSinceEnteringDownloadingState;
};

typedef std::list<ByteRangeReceivingTask> ByteRangeReceivingTaskList;

/**
 *  @class ByteRangeDownloader
 *
 *  @brief This class is the byte range downloader class.
 */
class ByteRangeDownloader : public IHttpConnectionTraceListener
{
    friend class ByteRangeDownloaderBaseState;
    friend class ByteRangeDownloaderDisconnectedState;
    friend class ByteRangeDownloaderWaitingToReconnectState;
    friend class ByteRangeDownloaderConnectingState;
    friend class ByteRangeDownloaderIdleState;
    friend class ByteRangeDownloaderDownloadingState;
    friend class ByteRangeDownloaderSuspendedState;
    friend class ByteRangeDownloaderConnectionDownState;

public:
    typedef shared_ptr<ByteRangeDownloader> ByteRangeDownloaderPtr;

    /** Byte range downloader states. */
    typedef enum
    {
        DISCONNECTED_STATE = 0,
        CONNECTING_STATE,
        WAITING_TO_RECONNECT_STATE,
        IDLE_STATE,
        DOWNLOADING_STATE,
        SUSPENDED_STATE,
        CONNECTION_DOWN_STATE
    } State;

public:
    /** Constructor. */
    ByteRangeDownloader(
                uint32_t parentId,
                IAsyncHttpClientPtr pAsyncHttpClient,
                ByteRangeDownloaderEventListenerPtr pEventListener );

    /** Destructor. */
    virtual ~ByteRangeDownloader();

    /** get the byte range downloader ID. */
    uint32_t getId() const;

    /** Get the current state. */
    State getState() const;

    /** Get the current state elapsed time. */
    AseTimeVal getStateElapsedTime() const;

    /** Get the current state elapsed time. */
    AseTimeVal getStateElapsedTime( const AseTimeVal& timeNow ) const;

    /** Get the time since downloading started on the connection */
    AseTimeVal getDownloadingElapsedTime( AseTimeVal const& timeNow ) const;

    /** Set the socket receive buffer size. */
    void setSocketReceiveBufferSize( uint32_t bufferSize );

    /** Set the reconnect backoff time. */
    void setReconnectBackoffTime( uint32_t backoffTime, uint32_t syncBackoffTime, uint32_t maxSyncConnectFailures );

    /** Set the transport report interval, in ms. */
    void setTransportReportInterval( uint32_t interval );

    /** Set the download report interval, in ms */
    void setDownloadReportInterval( uint32_t interval );

    /** Set the initial throughput, in bps. */
    void setInitialThroughput( int32_t id, ullong throughput );

    /** Set the EWMA throughput half life, in ms. */
    void setThroughputFilterHalfLife( int32_t id, uint32_t halfLife );

    /** Set the EWMA throughput measurement interval, in ms. */
    void setThroughputMeasurementInterval( int32_t id, uint32_t interval );

    /** Set the sleep time for the ByteRangeDownloader 2xx response loop, in micros */
    void setProcess2xxLoopInterval( int interval );

    /** Set timeouts
     *  @param httpConnectTimeout [in] Timeout for TCP connection (DNS + TCP)
     *  @param httpStallTimeout [in] Timeout for HTTP response or data arrival
     *  @param minimumDuplicateTimeout [in] Minimum timeout for duplcating requests
     *  @param duplicateTimeoutMultiplier [in] Multiplier for calculating duplicate
     *                                         timeout, times 10
     */
    void setTimeouts( AseTimeVal const& httpConnectTimeout,
                      AseTimeVal const& httpStallTimeout,
                      AseTimeVal const& minimumDuplicateTimeout,
                      uint32_t const duplicateTimeoutMultiplier);

    /** Get the destination of the connection */
    const AseUrl::HostPort& getDestination() const;

    /** Get the first connection established time. */
    AseTimeVal getFirstConnectionEstablishedTime() const;

    /** Get the current connection established time. */
    AseTimeVal getCurrentConnectionEstablishedTime() const;

    /** Get a list of current request URLs. */
    std::list<AseUrl> getRequestUrlList() const;

    /** Get the last request URL. */
    const AseUrl& getLastRequestUrl() const;

    /** Get the last HTTP response code. */
    uint32_t getLastHttpResponseCode() const;

    /** Get the last URL redirect information. */
    void getLastUrlRedirectInfo(
                    AseUrl& originalUrl,
                    AseUrl& newUrl ) const ;

    /** Get the connection failure code. */
    int32_t getConnectionFailureCode() const;

    /** Get the low level failure code. */
    int32_t getLowLevelFailureCode() const;

    /** Get the flag indicates it failes to connect. */
    bool getFailedToConnectFlag() const;

    /** Get the TCP disconnect state. */
    TcpDisconnectState getTcpDisconnectState() const;

    /** Get the average throughput. */
    ullong getAverageThroughput( int32_t id );

    /** Get the average throughput. */
    ullong getAverageThroughput( int32_t id, const AseTimeVal& timeNow );

    /** Get the total issued request count. */
    uint32_t getTotalIssuedRequestCount();

    /** Get the total completed reqeust count. */
    uint32_t getTotalCompletedRequestCount() const;

    /** Get the total outstanding request count. */
    uint32_t getTotalOutstandingRequestCount() const;

    /** Get the total requested byte count. */
    ullong getTotalRequestedByteCount() const;

    /** Get the total received byte count. */
    ullong getTotalReceivedByteCount() const;

    /** Get the outstanding byte count. */
    ullong getTotalOutstandingByteCount() const;

    /** Get the downloading paused flag. */
    bool getDownloadingPausedFlag() const;

    /** Get the media buffer availablility flag. */
    bool getMediaBufferAvailableFlag() const;

    /** Check if we are downloading duplicate data */
    bool checkForDuplicateDownloading() const;

    /** Retrieve all the byte range receiving tasks. */
    void retrieveByteReceivingTasks(
                    ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Connect the byte range downloader to the host:port. */
    int32_t connect(
                const AseTimeVal& timeNow,
                AseUrl::HostPort destination );

    /** Reconnect the byte range downloader. */
    int32_t reconnect(
                const AseTimeVal& timeNow );

    /** Disconnect the byte range downloader. */
    int32_t disconnect(
                const AseTimeVal& timeNow,
                int32_t reason = AS_CONNECTION_CLOSED );

    /**
     *  Request a byte range of an URL.
     *
     *  An HTTP request is sent out right away. If the byte range
     *  is closed, a range request is sent.
     *
     *  @param timeNow [in] the current time
     *  @param pUrlRequestTask [in] the URL request task this request is for
     *  @param requestUrl [in[ the possibly redirected URL
     *  @param byteRange [in/out] the byte range. The returned byte range actually requested may differ
     *                            from the input byte range if some of it was already requested
     *  @param existingByteRanges [out] Already existing requests within the range of the request
     *  @param pipelined [out] true if the request was pipelined
     */
    int32_t requestByteRange(
                    const AseTimeVal& timeNow,
                    UrlRequestTaskPtr pUrlRequestTask,
                    const AseUrl& requestUrl,
                    ByteRange& byteRange,
                    ByteRanges& existingRequests,
                    bool& pipelined );

    /** Assign a byte range receiving task. */
    int32_t assignByteRangeReceivingTask(
                    const AseTimeVal& timeNow,
                    const ByteRangeReceivingTask& byteRangeReceivingTask );

    /** Get the next timeout time. */
    AseTimeVal getNextTimeout();

    /** Test whether the frontmost request is predicted to be late */
    bool isFrontRequestPredictedToBeLate( AseTimeVal const& timeNow );

    /** Test whether the frontmost request is predicted to be late */
    bool isFrontRequestAbandoned();

    /** Process selected connection. */
    int32_t processHttpConnection(
                    const AseTimeVal& timeNow );

    /** Process HTTP response event. */
    int32_t processHttpResponse(
                    const AseTimeVal& timeNow,
                    ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Process timeout
     *  @param timeNow [in] The time at whch the timeout occured
     *  @param byteRangeReceivingTaskList [out] Timeout out tasks to be re-issued on another connection
     *                                          Note: the BRD does not abandon the timeout out tasks
     *                                          and they may still complete on this connection. They
     *                                          are returned so that they may be duplicated on other
     *                                          connections.
     */
    int32_t processTimeout(
                    const AseTimeVal& timeNow,
                    ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Pause downloading. */
    int32_t pauseDownloading(
                    const AseTimeVal& timeNow );
    /** Resume downloading. */
    int32_t resumeDownloading(
                    const AseTimeVal& timeNow );

    /** Notify the availability of the media buffer. */
    void notifyMediaBufferAvailability(
                    const AseTimeVal& timeNow,
                    bool mediaBufferAvailable );

    bool active() const;

    ///IHttpConnectionTraceListener function.
    /** Report a successful TCP connect. */
    virtual void reportTcpConnect(
                            uint32_t connectionId,
                            uint32_t httpId,
                            const std::string& clientIpAddress,
                            uint16_t clientPort,
                            const std::string& serverHostName,
                            const std::string& serverIpAddress,
                            uint16_t serverPort,
                            const AseTimeVal& dnsLookupTime,
                            const AseTimeVal& connectTime );

    /** Report a failed TCP connect. */
    virtual void reportTcpFailure(
                            uint32_t connectionId,
                            uint32_t httpId,
                            int32_t statusCode,
                            int32_t lowLevelCode,
                            const std::string& clientIpAddress,
                            uint16_t clientPort,
                            const std::string& serverHostName,
                            const std::string& serverIpAddress,
                            uint16_t serverPort,
                            const AseTimeVal& dnsLookupTime,
                            const AseTimeVal& failedTime );

    /** Report a TCP disconnect. */
    virtual void reportTcpDisconnect(
                            uint32_t connectionId,
                            uint32_t httpId,
                            int32_t statusCode,
                            int32_t lowLevelCode,
                            TcpDisconnectState disconnectState,
                            bool midResponse );

    /** Report byte received. */
    virtual void reportBytesReceived(
                            uint32_t connctionId,
                            const AseTimeVal& receivedTime,
                            uint32_t arrivedByteCount,
                            uint32_t receivedByteCount,
                            bool ignoringAfterResume,
                            AseTimeVal const& earliestTime );

    /** Report an HTTP connect failure. */
    virtual void reportHttpConnectFailure(
                            int32_t failureCode,
                            const std::string& serverHostName );

    /** Report a TCP connect start. */
    virtual void reportTcpStart(
                            uint32_t connectionId,
                            uint32_t httpId,
                            const std::string& serverHostName,
                            const std::string& serverIpAddress,
                            uint16_t serverPort,
                            bool primaryFlag );

protected:
    /** Transit to the next state. */
    void transit(
            const AseTimeVal& timeNow,
            ByteRangeDownloaderBaseState* pNextState );

    /** Open the connection. */
    int32_t openConnection(
                    const AseTimeVal& timeNow );

    /** Close the connection. */
    int32_t closeConnection(
                    const AseTimeVal& timeNow, int32_t reason = AS_CONNECTION_CLOSED );

    /** Report download record. */
    void reportDownloadRecord(
                    HttpAttempt& httpAttempt,
                    const AseTimeVal& timeNow,
                    int32_t errorCode );

    /** Clear HTTP requests. */
    void clearHttpRequests(
                    const AseTimeVal& timeNow );

    /** Process HTTP 2XX response. */
    int32_t processHttp2XXResponse(
                    const AseTimeVal& timeNow,
                    ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Process HTTP 3XX response. */
    int32_t processHttp3XXResponse(
                    const AseTimeVal& timeNow,
                    ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Process HTTP 4XX response. */
    int32_t processHttp4XXResponse(
                    const AseTimeVal& timeNow,
                    ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Process HTTP 5XX response. */
    int32_t processHttp5XXResponse(
                    const AseTimeVal& timeNow,
                    ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Process HTTP invalid response. */
    int32_t processHttpInvalidResponse(
                    const AseTimeVal& timeNow,
                    ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Set the duplicate request timeout */
    void setDuplicateRequestTimeout( AseTimeVal const& timeNow );

protected:
    /** HTTP request task, represents an actual HTTP request. */
    class HttpRequestTask : public IHttpRequestTraceListener
    {
    public:
        typedef shared_ptr<HttpRequestTask> HttpRequestTaskPtr;
    public:
        /** Constructor. */
        HttpRequestTask(
                uint32_t urlRequestTaskId,
                const AseUrl& requestUrl,
                const ByteRange& byteRange,
                bool pipelined,
                int reportInterval );

        /** Destructor. */
        virtual ~HttpRequestTask();

        /** IHttpRequestTraceListener */
        virtual void reportBytesReceived(
                                AseTimeVal const& time,
                                uint32_t bytes,
                                bool header,
                                bool complete );

    public:
        /** The UrlRequestTask id */
        uint32_t mUrlRequestTaskId;

        /** The request URL. */
        AseUrl mRequestUrl;

        /** The requested byte range. */
        ByteRange mByteRange;

        /** The received byte count. */
        ullong mReceivedByteCount;

        /** The async HTTP request for this HTTP request task. */
        IAsyncHttpRequestPtr mAsyncHttpRequestPtr;

        /** The transport reporter HttpAttempt record */
        HttpAttempt mHttpAttempt;

        /** The flag indicates that this is a non-pipelined request
         *  which has valid response time.
         */
        bool mNonPipelined;

        /** The flag indicates that this is a fresh HTTP request task,
         *  i.e., nothing has been received for this task.
         */
        bool mFreshFlag;
    };

    typedef HttpRequestTask::HttpRequestTaskPtr HttpRequestTaskPtr;

    /** Type definition for HTTP request task list. */
    typedef std::list<HttpRequestTaskPtr> HttpRequestTaskList;

protected:
    /** Byte range downloader states. */

    //Disconnected state.
    static ByteRangeDownloaderDisconnectedState SM_DISCONNECTED_STATE;

    //Waiting to reconnect state.
    static ByteRangeDownloaderWaitingToReconnectState SM_WAITING_TO_RECONNECT_STATE;

    //Connecting state.
    static ByteRangeDownloaderConnectingState SM_CONNECTING_STATE;

    //Idle state.
    static ByteRangeDownloaderIdleState SM_IDLE_STATE;

    //Downloading state.
    static ByteRangeDownloaderDownloadingState SM_DOWNLOADING_STATE;

    //Suspended state.
    static ByteRangeDownloaderSuspendedState SM_SUSPENDED_STATE;

    //Connection down state.
    static ByteRangeDownloaderConnectionDownState SM_CONNECTION_DOWN_STATE;

    //The ID pool.
    static uint32_t mIdPool;

protected:
    /** The parent ID. */
    uint32_t mParentId;

    /** The ID. */
    uint32_t mId;

    /** The async HTTP client. */
    IAsyncHttpClientPtr mAsyncHttpClientPtr;

    /** The event listener. */
    ByteRangeDownloaderEventListenerPtr mEventListenerPtr;

    /** The configuration parameters. */

    /** The socket receive buffer size, in bytes. */
    uint32_t mSocketReceiveBufferSize;

    /** The reconnect backoff time, in ms. */
    uint32_t mReconnectBackoffTime;

    /** The reconnect backoff time after a synchronous failure */
    uint32_t mSyncReconnectBackoffTime;

    /** The transport report interval, in ms. */
    uint32_t mTransportReportInterval;

    /** The incremental download report interval */
    AseTimeVal mDownloadReportInterval;

    /** Max TCP synchronous connect failures */
    uint32_t mMaxTcpConnectFailures;

    /** The reconnect backoff time for the next reconnect */
    uint32_t mNextReconnectBackoffTime;

    /** The destination (host, port) */
    AseUrl::HostPort mDestination;

    /** The current state. */
    ByteRangeDownloaderBaseState* mCurrentStatePtr;

    /** The current state entering time. */
    AseTimeVal mCurrentStateEnteringTime;

    /** Time at which data was first received on this connection */
    AseTimeVal mConnectionDataReceivedTime;

    /** The downloading paused flag. */
    bool mDownloadingPausedFlag;

    /** The media buffer availability flag. */
    bool mMediaBufferAvailableFlag;

    /** The asyc HTTP connection. */
    IAsyncHttpConnectionPtr mAsyncHttpConnectionPtr;

    /** The first connection established time. */
    AseTimeVal mFirstConnectionEstablishedTime;

    /** The current connection established time. */
    AseTimeVal mCurrentConnectionEstablishedTime;

    /** The outstanding HTTP request task list. */
    HttpRequestTaskList mHttpRequestTaskList;

    /** The byte range receiving task list. */
    ByteRangeReceivingTaskList mByteRangeReceivingTaskList;

    /** The last request URL. */
    AseUrl mLastRequestUrl;

    /** The last HTTP response code. */
    uint32_t mLastHttpResponseCode;

    /** The last URL redirect original URL. */
    AseUrl mLastUrlRedirectOriginalUrl;

    /** The last URL redirect new URL. */
    AseUrl mLastUrlRedirectNewUrl;

    /** The connection failure code. */
    int32_t mConnectionFailureCode;

    /** The low level failure code. */
    int32_t mLowLevelFailureCode;

    /** The flag indicates that it failes to connect. */
    bool mFailedToConnectFlag;

    /** The TCP disconnect state. */
    TcpDisconnectState mTcpDisconnectState;

    /** Indicates whether the last connection attempt failed synchronously */
    bool mFastConnectionFailure;

    /** Consecutive TCP synchronous connect failure count */
    uint32_t mTcpConnectFailureCount;

    /** The initial throughputs, in bps. */
    ullong mInitialThroughputs[3];

    /** The EWMA throughput half life, in ms. */
    uint32_t mThroughputFilterHalfLives[3];

    /** Timeout from sending the HTTP request to receiving the response */
    AseTimeVal mHttpConnectTimeout;

    /** Timeout from data arrival to next data arrival */
    AseTimeVal mHttpStallTimeout;

    /** Minimum timeout from duplicating requests */
    AseTimeVal mMinimumDuplicateTimeout;

    /** Duplicate timeout as multiplier of the average response time */
    uint32_t mDuplicateTimeoutMultiplier;

    /** The EWMA throughput measurement interval, in ms. */
    uint32_t mThroughputMeasurementIntervals[3];

    /** The average throughput measurements.
     *  0 - for scheduling byte ranges.
     *  1 - for deciding if the throughput of a CDN is slow, this has
     *      much longer half-life time.
     *  2 - for informational purpose, this has the initial throughput
     *      of 0
     *  */
    AverageThroughput mAverageThroughputs[3];

    AverageResponseTime mAverageResponseTime;

    /** The total issued request count on the current connection. */
    uint32_t mTotalIssuedRequestCount;

    /** The total completed request count on the current connection. */
    uint32_t mTotalCompletedRequestCount;

    /** The total requested byte count on the current connection. */
    ullong mTotalRequestedByteCount;

    /** The total received byte count on the current connection. */
    ullong mTotalReceivedByteCount;

    /** Multi-purpose timers */
    AseTimeVal mTimerOne;

    int mProcess2xxLoopSleepMicros;
};

typedef ByteRangeDownloader::ByteRangeDownloaderPtr ByteRangeDownloaderPtr;

typedef std::map<uint32_t, ByteRangeDownloaderPtr> ByteRangeDownloaderMap;

inline bool ByteRangeDownloader::active() const
{
    return ((getState() == IDLE_STATE)
        || (getState() == DOWNLOADING_STATE)
        || (getState() == SUSPENDED_STATE ));
}

}}
#endif // BYTE_RANGE_DOWNLOADER_H
