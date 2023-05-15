/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/UrlEncoder.h>
#include <nrdbase/Log.h>
#include <nrdbase/PerformanceMarker.h>

#include "DebugLogging.h"
#include "AseCommonUtil.h"

#include "UrlRequestTask.h"
#include "StreamersLog.h"
#include "ByteRangeDownloaderStates.h"
#include "ByteRangeDownloader.h"
#include "ByteRangeDownloaderEventListener.h"

#include <set>

namespace netflix {
namespace ase {

DEFINE_LOGGER( ByteRangeDownloader );

/**
 *  @class ByteRangeReceivingTask
 */

/** Constructor. */
ByteRangeReceivingTask::ByteRangeReceivingTask(
                            UrlRequestTaskPtr   pUrlRequestTask,
                            ByteRange const&    range ) :   mUrlRequestTaskPtr( pUrlRequestTask ),
                                                            mByteRange( range ),
                                                            mReceivedByteCount( 0 ),
                                                            mDataReceivedSinceEnteringDownloadingState( false )
{
}

/** Destructor. */
ByteRangeReceivingTask::~ByteRangeReceivingTask()
{
}


/** Byte range downloader states. */

ByteRangeDownloaderDisconnectedState ByteRangeDownloader::SM_DISCONNECTED_STATE;
ByteRangeDownloaderWaitingToReconnectState ByteRangeDownloader::SM_WAITING_TO_RECONNECT_STATE;
ByteRangeDownloaderConnectingState ByteRangeDownloader::SM_CONNECTING_STATE;
ByteRangeDownloaderIdleState ByteRangeDownloader::SM_IDLE_STATE;
ByteRangeDownloaderDownloadingState ByteRangeDownloader::SM_DOWNLOADING_STATE;
ByteRangeDownloaderSuspendedState ByteRangeDownloader::SM_SUSPENDED_STATE;
ByteRangeDownloaderConnectionDownState ByteRangeDownloader::SM_CONNECTION_DOWN_STATE;

//The ID pool.
uint32_t ByteRangeDownloader::mIdPool = 0;

/**
 *  @class ByteRangeDownloader
 */

/** Constructor. */
ByteRangeDownloader::ByteRangeDownloader(
                        uint32_t parentId,
                        IAsyncHttpClientPtr pAsyncHttpClient,
                        ByteRangeDownloaderEventListenerPtr pEventListener )
                                    : mParentId( parentId ),
                                      mId( mIdPool++ ),
                                      mAsyncHttpClientPtr( pAsyncHttpClient ),
                                      mEventListenerPtr( pEventListener ),
                                      mSocketReceiveBufferSize( 256 * 1024 ),
                                      mReconnectBackoffTime( 500 ),
                                      mSyncReconnectBackoffTime( 2000 ),
                                      mTransportReportInterval( 1000 ),
                                      mMaxTcpConnectFailures( 4 ),
                                      mCurrentStatePtr( NULL ),
                                      mDownloadingPausedFlag( false ),
                                      mFastConnectionFailure( false ),
                                      mTcpConnectFailureCount( 0 ),
                                      mMinimumDuplicateTimeout( 0 ),
                                      mDuplicateTimeoutMultiplier( 0 ),
                                      mAverageResponseTime( 5 ),
                                      mProcess2xxLoopSleepMicros( -1 )
{

    mInitialThroughputs[0] = 3000000;
    mInitialThroughputs[1] = 3000000;
    mInitialThroughputs[2] = 0;

    mThroughputFilterHalfLives[0] = 1000;
    mThroughputFilterHalfLives[1] = 20000;
    mThroughputFilterHalfLives[2] = 1000;

    mThroughputMeasurementIntervals[0] = 100;
    mThroughputMeasurementIntervals[1] = 100;
    mThroughputMeasurementIntervals[2] = 100;

    mHttpConnectTimeout = AseTimeVal::ZERO;
    mHttpStallTimeout = AseTimeVal::ZERO;

    mAverageThroughputs[0] = AverageThroughput(
                                    AseTimeVal::fromMS( mThroughputFilterHalfLives[0] ),
                                    AseTimeVal::fromMS( mThroughputMeasurementIntervals[0] ),
                                    mInitialThroughputs[0],
                                    true );

    mAverageThroughputs[1] = AverageThroughput(
                                    AseTimeVal::fromMS( mThroughputFilterHalfLives[1] ),
                                    AseTimeVal::fromMS( mThroughputMeasurementIntervals[1] ),
                                    mInitialThroughputs[1],
                                    true );

    mAverageThroughputs[2] = AverageThroughput(
                                    AseTimeVal::fromMS( mThroughputFilterHalfLives[2] ),
                                    AseTimeVal::fromMS( mThroughputMeasurementIntervals[2] ),
                                    mInitialThroughputs[2],
                                    true );

    transit( AseTimeVal::now(), &SM_DISCONNECTED_STATE );
}

/** Destructor. */
ByteRangeDownloader::~ByteRangeDownloader()
{
}

/** get the byte range downloader ID. */
uint32_t ByteRangeDownloader::getId() const
{
    return mId;
}

/** Get the current state. */
ByteRangeDownloader::State ByteRangeDownloader::getState() const
{
    return mCurrentStatePtr->state();
}

/** Get the current state elapsed time. */
AseTimeVal ByteRangeDownloader::getStateElapsedTime() const
{
    return AseTimeVal::now() - mCurrentStateEnteringTime;
}

/** Get the current state elapsed time. */
AseTimeVal ByteRangeDownloader::getStateElapsedTime( const AseTimeVal& timeNow ) const
{
    return timeNow - mCurrentStateEnteringTime;
}

/** Get the time since downloading started */
AseTimeVal ByteRangeDownloader::getDownloadingElapsedTime( AseTimeVal const& timeNow ) const
{
    return ( mConnectionDataReceivedTime == AseTimeVal::INFINITE )
                ? AseTimeVal::INFINITE
                : ( timeNow - mConnectionDataReceivedTime );
}


/** Set the socket receive buffer size. */
void ByteRangeDownloader::setSocketReceiveBufferSize( uint32_t bufferSize )
{
    mSocketReceiveBufferSize = bufferSize;
}

/** Set the reconnect backoff time. */
void ByteRangeDownloader::setReconnectBackoffTime( uint32_t backoffTime, uint32_t syncBackoffTime, uint32_t maxSyncConnectFailures )
{
    mReconnectBackoffTime = backoffTime;
    mSyncReconnectBackoffTime = syncBackoffTime;
    mMaxTcpConnectFailures = maxSyncConnectFailures;
}

/** Set the transport report interval, in ms. */
void ByteRangeDownloader::setTransportReportInterval( uint32_t interval )
{
    mTransportReportInterval = interval;
}

/** Set the download report interval, in ms */
void ByteRangeDownloader::setDownloadReportInterval( uint32_t interval )
{
    mDownloadReportInterval = ( interval > 0 ) ? AseTimeVal::fromMS( interval )
                                               : AseTimeVal::INFINITE;
}

/** Set the initial throughput, in bps. */
void ByteRangeDownloader::setInitialThroughput( int32_t id, ullong throughput )
{
    mInitialThroughputs[id] = throughput;
}

/** Set the EWMA throughput half life, in ms. */
void ByteRangeDownloader::setThroughputFilterHalfLife( int32_t id, uint32_t halfLife )
{
    mThroughputFilterHalfLives[id] = halfLife;
}

/** Set the EWMA throughput measurement interval, in ms. */
void ByteRangeDownloader::setThroughputMeasurementInterval( int32_t id,  uint32_t interval )
{
    mThroughputMeasurementIntervals[id] = interval;
}

/** Set the sleep time for the ByteRangeDownloader 2xx response loop, in micros */
void ByteRangeDownloader::setProcess2xxLoopInterval( int interval )
{
    mProcess2xxLoopSleepMicros = interval;
}


void ByteRangeDownloader::setTimeouts(AseTimeVal const& httpConnectTimeout,
                                      AseTimeVal const& httpStallTimeout,
                                      AseTimeVal const& minimumDuplicateTimeout,
                                      uint32_t const duplicateTimeoutMultiplier )
{
    mHttpConnectTimeout = httpConnectTimeout;
    mHttpStallTimeout = httpStallTimeout;
    mMinimumDuplicateTimeout = minimumDuplicateTimeout;
    mDuplicateTimeoutMultiplier = duplicateTimeoutMultiplier;
}

/** Get the destination of the connection */
const AseUrl::HostPort& ByteRangeDownloader::getDestination() const
{
    return mDestination;
}

/** Get the first connection established time. */
AseTimeVal ByteRangeDownloader::getFirstConnectionEstablishedTime() const
{
    return mFirstConnectionEstablishedTime;
}

/** Get the current connection established time. */
AseTimeVal ByteRangeDownloader::getCurrentConnectionEstablishedTime() const
{
    return mCurrentConnectionEstablishedTime;
}

/** Get a list of current request URLs. */
std::list<AseUrl> ByteRangeDownloader::getRequestUrlList() const
{
    std::list<AseUrl> retList;

    HttpRequestTaskList::const_iterator httpRequestTaskItr;

    httpRequestTaskItr = mHttpRequestTaskList.begin();

    while (httpRequestTaskItr != mHttpRequestTaskList.end())
    {
        retList.push_back( (*httpRequestTaskItr)->mRequestUrl );

        httpRequestTaskItr++;
    }

    return retList;
}

/** Get the last request URL. */
const AseUrl& ByteRangeDownloader::getLastRequestUrl() const
{
    return mLastRequestUrl;
}

/** Get the last HTTP response code. */
uint32_t ByteRangeDownloader::getLastHttpResponseCode() const
{
    return mLastHttpResponseCode;
}

/** Get the last URL redirect information. */
void ByteRangeDownloader::getLastUrlRedirectInfo(
                                        AseUrl& originalUrl,
                                        AseUrl& newUrl ) const
{
    originalUrl = mLastUrlRedirectOriginalUrl;
    newUrl = mLastUrlRedirectNewUrl;

    return;
}

/** Get the connection failure code. */
int32_t ByteRangeDownloader::getConnectionFailureCode() const
{
    return mConnectionFailureCode;
}

/** Get the low level failure code. */
int32_t ByteRangeDownloader::getLowLevelFailureCode() const
{
    return mLowLevelFailureCode;
}

/** Get the flag indicates it failes to connect. */
bool ByteRangeDownloader::getFailedToConnectFlag() const
{
    return mFailedToConnectFlag;
}

/** Get the TCP disconnect state. */
TcpDisconnectState ByteRangeDownloader::getTcpDisconnectState() const
{
    return mTcpDisconnectState;
}

/** Get the average throughput. */
ullong ByteRangeDownloader::getAverageThroughput( int32_t id )
{
    return mAverageThroughputs[id].getThroughput( AseTimeVal::now() );
}

/** Get the average throughput. */
ullong ByteRangeDownloader::getAverageThroughput( int32_t id, const AseTimeVal& timeNow )
{
    return mAverageThroughputs[id].getThroughput( timeNow );
}

/** Get the total issued request count. */
uint32_t ByteRangeDownloader::getTotalIssuedRequestCount()
{
    return mTotalIssuedRequestCount;
}

/** Get the total completed reqeust count. */
uint32_t ByteRangeDownloader::getTotalCompletedRequestCount() const
{
    return mTotalCompletedRequestCount;
}

/** Get the total outstanding request count. */
uint32_t ByteRangeDownloader::getTotalOutstandingRequestCount() const
{
    return mTotalIssuedRequestCount - mTotalCompletedRequestCount;
}

/** Get the total requested byte count. */
ullong ByteRangeDownloader::getTotalRequestedByteCount() const
{
    return mTotalRequestedByteCount;
}

/** Get the total received byte count. */
ullong ByteRangeDownloader::getTotalReceivedByteCount() const
{
    return mTotalReceivedByteCount;
}

/** Get the outstanding byte count. */
ullong ByteRangeDownloader::getTotalOutstandingByteCount() const
{
    return mTotalRequestedByteCount - mTotalReceivedByteCount;
}

/** Get the downloading paused flag. */
bool ByteRangeDownloader::getDownloadingPausedFlag() const
{
    return mDownloadingPausedFlag;
}

/** Get the media buffer availablility flag. */
bool ByteRangeDownloader::getMediaBufferAvailableFlag() const
{
    return mMediaBufferAvailableFlag;
}

/** Check if we are downloading duplicate data */
bool ByteRangeDownloader::checkForDuplicateDownloading() const
{
    if ( mByteRangeReceivingTaskList.empty() )
        return false;

    return ( mByteRangeReceivingTaskList.front().mUrlRequestTaskPtr->received( mByteRangeReceivingTaskList.front().mByteRange ) );
}

/** Retrieve all the byte range receiving tasks. */
void ByteRangeDownloader::retrieveByteReceivingTasks(
                                ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    byteRangeReceivingTaskList = mByteRangeReceivingTaskList;

    mByteRangeReceivingTaskList.clear();

    return;
}

/** Connect the byte range downloader to the host:port. */
int32_t ByteRangeDownloader::connect(
                                const AseTimeVal& timeNow,
                                AseUrl::HostPort destination )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert( getState() == DISCONNECTED_STATE || getState() == CONNECTION_DOWN_STATE );

    mDestination = destination;

    retVal = openConnection( timeNow );

    if (retVal == AS_NO_ERROR)
    {
        transit( timeNow, &SM_CONNECTING_STATE );
    }
    else
    {
        mFastConnectionFailure = true;

        transit( timeNow, &SM_CONNECTION_DOWN_STATE );
    }

    return retVal;
}

/** Reconnect the byte range downloader. */
int32_t ByteRangeDownloader::reconnect(
                                const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert( getState() == CONNECTION_DOWN_STATE || getState() == DISCONNECTED_STATE );

    mNextReconnectBackoffTime = mFastConnectionFailure ? mSyncReconnectBackoffTime : mReconnectBackoffTime;

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u %s will reconnect to %s:%d in %ums.",
        mParentId,
        mId,
        mCurrentStatePtr->name(),
        mDestination.first.c_str(),
        mDestination.second,
        ( getState() == CONNECTION_DOWN_STATE ) ? mNextReconnectBackoffTime : 0 );

    if ( getState() == CONNECTION_DOWN_STATE )
    {
        transit( timeNow, &SM_WAITING_TO_RECONNECT_STATE );

        retVal = AS_NO_ERROR;
    }
    else if ( getState() == DISCONNECTED_STATE )
    {
        retVal = openConnection( timeNow );

        if (retVal == AS_NO_ERROR)
        {
            transit( timeNow, &SM_CONNECTING_STATE );
        }
        else
        {
            mFastConnectionFailure = true;

            transit( timeNow, &SM_CONNECTION_DOWN_STATE );
        }
    }

    return retVal;
}

/** Disconnect the byte range downloader. */
int32_t ByteRangeDownloader::disconnect(
                                const AseTimeVal& timeNow, int32_t reason )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u disconnect from %s:%d (%s)",
        mParentId,
        mId,
        mDestination.first.c_str(),
        mDestination.second,
        mCurrentStatePtr->name() );

    if ((getState() == IDLE_STATE)
        || (getState() == DOWNLOADING_STATE)
        || (getState() == SUSPENDED_STATE ))
    {
        if ((getState() == DOWNLOADING_STATE) || (getState() == SUSPENDED_STATE))
        {
            reportDownloadRecord(
                    mHttpRequestTaskList.front()->mHttpAttempt,
                    timeNow,
                    reason );
        }

        closeConnection( timeNow, reason );
    }
    else if (getState() == CONNECTING_STATE)
    {
        closeConnection( timeNow, reason );
    }

    transit( timeNow, &SM_DISCONNECTED_STATE );

    retVal = AS_NO_ERROR;

    return retVal;
}

/** Request a byte range of an URL. */
int32_t ByteRangeDownloader::requestByteRange(
                                const AseTimeVal& timeNow,
                                UrlRequestTaskPtr pUrlRequestTask,
                                const AseUrl& requestUrl,
                                ByteRange& byteRange,
                                ByteRanges& existingRequests,
                                bool& pipelined )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert( getState() == IDLE_STATE
            || getState() == DOWNLOADING_STATE
            || getState() == SUSPENDED_STATE );

    assert( existingRequests.empty() );

    ByteRanges request( byteRange );

    uint32_t const urlRequestTaskId = pUrlRequestTask->getId();

    HttpRequestTaskList::const_iterator it = mHttpRequestTaskList.begin();
    for( ; it != mHttpRequestTaskList.end() && !request.empty(); ++it )
    {
        if (((*it)->mUrlRequestTaskId == urlRequestTaskId )
            && (*it)->mByteRange.overlaps( byteRange ) )
        {
            existingRequests |= ( byteRange & (*it)->mByteRange );

            request -= (*it)->mByteRange;
        }
    }

    if ( request.empty() )
        return AS_IN_PROGRESS;

    byteRange = request.front();

    //Create an HTTP request task.
    HttpRequestTaskPtr pHttpRequestTask = HttpRequestTaskPtr(
                                            new HttpRequestTask(
                                                    urlRequestTaskId,
                                                    requestUrl,
                                                    byteRange,
                                                    !mHttpRequestTaskList.empty(),
                                                    mTransportReportInterval ) );

    assert( pHttpRequestTask );

    //Create the range header.
    std::pair<std::string, std::string> rangeHeader;

    rangeHeader = convertByteRangeToHeader( byteRange );

    HttpHeaders headers;

    headers[ rangeHeader.first ] = rangeHeader.second;

    //Issue the get request.
    retVal = mAsyncHttpConnectionPtr->issueGetRequest(
                                            requestUrl,
                                            headers,
                                            pHttpRequestTask,
                                            pHttpRequestTask->mAsyncHttpRequestPtr );

    NTRACE(
        TRACE_STREAMINGSTAT,
        "%u %u issued an HTTP request, request URL: %s, byte range: %lld-%lld, ret: %d.",
        mParentId,
        mId,
        requestUrl.GetPrefix().c_str(),
        byteRange.getStart(),
        byteRange.getEnd(),
        retVal );

    if (retVal == AS_NO_ERROR)
    {
        pHttpRequestTask->mHttpAttempt.mTcpId = mAsyncHttpConnectionPtr->getId();
        pHttpRequestTask->mHttpAttempt.mTreq = timeNow;

        if (mHttpRequestTaskList.empty() && (getState() != SUSPENDED_STATE))
        {
            pHttpRequestTask->mNonPipelined = true;
        }

        mHttpRequestTaskList.push_back( pHttpRequestTask );

        if (byteRange.closed())
        {
            mTotalRequestedByteCount += byteRange.getSize();
        }

        mTotalIssuedRequestCount++;

        if (getState() == IDLE_STATE)
        {
            if (!mDownloadingPausedFlag && mMediaBufferAvailableFlag)
            {
                transit( timeNow, &SM_DOWNLOADING_STATE );
            }
            else
            {
                mAsyncHttpConnectionPtr->pauseReceiving();

                transit( timeNow, &SM_SUSPENDED_STATE );
            }
        }
    }
    else
    {
        mConnectionFailureCode = retVal;
        mLowLevelFailureCode = mAsyncHttpConnectionPtr->getLowLevelFailureCode();

        if (!mHttpRequestTaskList.empty())
        {
            if (mHttpRequestTaskList.front()->mAsyncHttpRequestPtr->getState()
                    >= IAsyncHttpRequest::WAITING_STATE)
            {
                reportDownloadRecord(
                        mHttpRequestTaskList.front()->mHttpAttempt,
                        timeNow,
                        retVal );
            }
        }

        transit( timeNow, &SM_CONNECTION_DOWN_STATE );
    }

    pipelined = !pHttpRequestTask->mNonPipelined;

    return retVal;
}

/** Assign a byte range receiving task. */
int32_t ByteRangeDownloader::assignByteRangeReceivingTask(
                                const AseTimeVal& timeNow,
                                const ByteRangeReceivingTask& byteRangeReceivingTask )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert( getState() == IDLE_STATE
            || getState() == DOWNLOADING_STATE
            || getState() == SUSPENDED_STATE );

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u was assigned a byte range receiving task, URL request task ID: %u, "
        "byte range: %lld-%lld",
        mParentId,
        mId,
        byteRangeReceivingTask.mUrlRequestTaskPtr->getId(),
        byteRangeReceivingTask.mByteRange.getStart(),
        byteRangeReceivingTask.mByteRange.getEnd() );

    mByteRangeReceivingTaskList.push_back( byteRangeReceivingTask );

    if ((getState() == SUSPENDED_STATE)
        && !mDownloadingPausedFlag
        && mMediaBufferAvailableFlag )
    {
        mAsyncHttpConnectionPtr->resumeReceiving();

        transit( timeNow, &SM_DOWNLOADING_STATE );
    }

    retVal = AS_NO_ERROR;

    return retVal;
}

/** Get the next timeout time. */
AseTimeVal ByteRangeDownloader::getNextTimeout()
{
    /*
    bool bAbandonTimer = false;
    */

    if ( getState() == DOWNLOADING_STATE )
    {
        assert( !mByteRangeReceivingTaskList.empty() );

        if ( mByteRangeReceivingTaskList.front().mDataReceivedSinceEnteringDownloadingState )
        {
            // We refresh the abandon timer every time around the select loop as it may have
            // been changed when processing data received in another ByteRangeDownloader
            AseTimeVal const abandontime = mByteRangeReceivingTaskList.front().mUrlRequestTaskPtr
                                            ->getMediaRequestTask()
                                                ->getAbandonTime();

            if ( abandontime < mTimerOne )
            {
                mTimerOne = abandontime;
                /*
                bAbandonTimer = true;
                */
            }
        }
    }

/*
    if ( mTimerOne != AseTimeVal::INFINITE )
    {
        DEBUGLOG(
            ByteRangeDownloader,
            "%u %u %s state, timout in %llums (%s)",
            mParentId,
            mId,
            mCurrentStatePtr->name(),
            ( mTimerOne - AseTimeVal::now() ).ms(),
            bAbandonTimer ? "abandon" : "other" );
    }
*/
    return mTimerOne;
}

/** Test whether the frontmost request is predicted to be late */
bool ByteRangeDownloader::isFrontRequestPredictedToBeLate( AseTimeVal const& timeNow )
{
    if ( getState() != DOWNLOADING_STATE && getState() != SUSPENDED_STATE )
        return false;

    assert( !mByteRangeReceivingTaskList.empty() );

    uint64_t throughput = getAverageThroughput( 0, timeNow );

    if ( throughput == 0 )
        return true;

    ByteRangeReceivingTask const& task = mByteRangeReceivingTaskList.front();

    uint32_t const bytes = task.mByteRange.size() - task.mReceivedByteCount;

    AseTimeVal const completion = timeNow + AseTimeVal::fromMS( ( bytes * 8 ) / ( throughput / 1000 ) );

    return ( completion > task.mUrlRequestTaskPtr->getCompletionDeadline() );
}

/** Test whether the frontmost request is predicted to be late */
bool ByteRangeDownloader::isFrontRequestAbandoned()
{
    if ( getState() != DOWNLOADING_STATE && getState() != SUSPENDED_STATE )
        return false;

    assert( !mByteRangeReceivingTaskList.empty() );

    return mByteRangeReceivingTaskList.front().mUrlRequestTaskPtr->isAbandoned();
}


/** Process selected connection. */
int32_t ByteRangeDownloader::processHttpConnection(
                                const AseTimeVal& timeNow )
{
    return mCurrentStatePtr->processHttpConnection(
                                        *this,
                                        timeNow );
}

/** Process HTTP response event. */
int32_t ByteRangeDownloader::processHttpResponse(
                                const AseTimeVal& timeNow,
                                ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    return mCurrentStatePtr->processHttpResponse(
                                        *this,
                                        timeNow,
                                        byteRangeReceivingTaskList );
}

/** Process timeout. */
int32_t ByteRangeDownloader::processTimeout(
                                const AseTimeVal& timeNow,
                                ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    return mCurrentStatePtr->processTimeout(
                                        *this,
                                        timeNow,
                                        byteRangeReceivingTaskList );
}

/** Pause downloading. */
int32_t ByteRangeDownloader::pauseDownloading(
                                const AseTimeVal& timeNow )
{
    return mCurrentStatePtr->pauseDownloading(
                                        *this,
                                        timeNow );
}

/** Resume downloading. */
int32_t ByteRangeDownloader::resumeDownloading(
                                const AseTimeVal& timeNow )
{
    return mCurrentStatePtr->resumeDownloading(
                                        *this,
                                        timeNow );
}

/** Notify the availability of the media buffer. */
void ByteRangeDownloader::notifyMediaBufferAvailability(
                                const AseTimeVal& timeNow,
                                bool mediaBufferAvailable )
{
    mCurrentStatePtr->notifyMediaBufferAvailability(
                                        *this,
                                        timeNow,
                                        mediaBufferAvailable );

    return;
}

/** Report a successful TCP connect. */
void ByteRangeDownloader::reportTcpConnect(
                                uint32_t connectionId,
                                uint32_t httpId,
                                const std::string& clientIpAddress,
                                uint16_t clientPort,
                                const std::string& serverHostName,
                                const std::string& serverIpAddress,
                                uint16_t serverPort,
                                const AseTimeVal& dnsLookupTime,
                                const AseTimeVal& connectTime )
{
    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u (info) TCP connect: %u %u, srd: %s:%d, dst: %s %s:%d, dns: %llums, connect: %llums.",
        mParentId,
        mId,
        connectionId,
        httpId,
        clientIpAddress.c_str(),
        clientPort,
        serverHostName.c_str(),
        serverIpAddress.c_str(),
        serverPort,
        dnsLookupTime.ms(),
        connectTime.ms() );

    mEventListenerPtr->reportTcpConnect(
                                mId,
                                connectionId,
                                httpId,
                                clientIpAddress,
                                clientPort,
                                serverHostName,
                                serverIpAddress,
                                serverPort,
                                dnsLookupTime,
                                connectTime );



#if ENABLE_STREAMERS_LOG

    gStreamersLog.addTcpConnectRecord(
                            mParentId,
                            mId,
                            mAsyncHttpConnectionPtr->getId(),
                            serverHostName.c_str(),
                            serverIpAddress.c_str(),
                            serverPort,
                            connectTime );
#endif


    mTcpConnectFailureCount = 0;

    return;
}

/** Report a failed TCP connect. */
void ByteRangeDownloader::reportTcpFailure(
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
                                const AseTimeVal& failedTime )
{

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u (info) TCP failure: %u %u, status: %d %d, src %s:%d, dst: %s %s:%d, dns: %llums, fail: %llums.",
        mParentId,
        mId,
        connectionId,
        httpId,
        statusCode,
        lowLevelCode,
        clientIpAddress.c_str(),
        clientPort,
        serverHostName.c_str(),
        serverIpAddress.c_str(),
        serverPort,
        dnsLookupTime.ms(),
        failedTime.ms() );

    mEventListenerPtr->reportTcpFailure(
                                mId,
                                connectionId,
                                httpId,
                                statusCode,
                                lowLevelCode,
                                clientIpAddress,
                                clientPort,
                                serverHostName,
                                serverIpAddress,
                                serverPort,
                                dnsLookupTime,
                                failedTime );

}

/** Report a TCP disconnect. */
void ByteRangeDownloader::reportTcpDisconnect(
                                uint32_t connectionId,
                                uint32_t httpId,
                                int32_t statusCode,
                                int32_t lowLevelCode,
                                TcpDisconnectState disconnectState,
                                bool midResponse )
{

#if defined(BUILD_DEBUG) || ENABLE_STREAMERS_LOG

    const char* const pStateString =
        ( disconnectState == TCP_DISCONNECT_BY_LOCAL ) ? "local"
    :   ( disconnectState == TCP_DISCONNECT_BY_REMOTE_WHILE_IDLE ) ? "remote, idle"
    :   ( disconnectState == TCP_DISCONNECT_BY_REMOTE_WHILE_SENDING ) ? "remote, sending"
    :   ( disconnectState == TCP_DISCONNECT_BY_REMOTE_WHILE_RECEIVING ) ? "remote, receiving"
    :   "unknown";

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u (info) TCP disconnect: %u %u, status: %d %d, %s %s, issued: %u, complete: %u, req: %llu, rcv: %llu.",
        mParentId,
        mId,
        connectionId,
        httpId,
        statusCode,
        lowLevelCode,
        pStateString,
        midResponse ? "mid-response" : "idle",
        mTotalIssuedRequestCount,
        mTotalCompletedRequestCount,
        mTotalRequestedByteCount,
        mTotalReceivedByteCount );

#if ENABLE_STREAMERS_LOG

    gStreamersLog.addTcpDisconnectRecord(
                            mParentId,
                            mId,
                            mAsyncHttpConnectionPtr->getId(),
                            mDestination.first.c_str(),
                            mAsyncHttpConnectionPtr->getServerIpAddress().c_str(),
                            mDestination.second,
                            mConnectionFailureCode,
                            mLowLevelFailureCode,
                            pStateString,
                            midResponse ? "True" : "False",
                            mTotalIssuedRequestCount,
                            mTotalCompletedRequestCount,
                            mTotalRequestedByteCount,
                            mTotalReceivedByteCount );
#endif

#endif

    mEventListenerPtr->reportTcpDisconnect(
                                mId,
                                connectionId,
                                httpId,
                                statusCode,
                                lowLevelCode,
                                disconnectState,
                                midResponse );

    mTcpDisconnectState = disconnectState;

}

/** Report byte received. */
void ByteRangeDownloader::reportBytesReceived(
                                        uint32_t connctionId,
                                        const AseTimeVal& receivedTime,
                                        uint32_t arrivedByteCount,
                                        uint32_t receivedByteCount,
                                        bool drainingSocketReceiveBuffer,
                                        AseTimeVal const& earliestTime )
{
    NRDP_UNUSED( connctionId );

/*    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u received bytes received report: %u %llu %d %u %s.",
        mParentId,
        mId,
        connctionId,
        receivedTime.ms(),
        arrivedByteCount == 0xFFFFFFFF ? -1 : (int32_t) arrivedByteCount,
        receivedByteCount,
        drainingSocketReceiveBuffer ? "True" : "False" );*/

    if (arrivedByteCount != 0xFFFFFFFF)
    {
        //Update the average throughput measurement.

        mAverageThroughputs[0].bytesReceived(
                                    arrivedByteCount,
                                    receivedTime,
                                    false );

        mAverageThroughputs[1].bytesReceived(
                                    arrivedByteCount,
                                    receivedTime,
                                    false );

        mAverageThroughputs[2].bytesReceived(
                                    arrivedByteCount,
                                    receivedTime,
                                    false );


        mEventListenerPtr->reportBytesReceived(
                                        mId,
                                        receivedTime,
                                        arrivedByteCount,
                                        drainingSocketReceiveBuffer,
                                        earliestTime );
    }
    else
    {
        mAverageThroughputs[0].bytesReceived(
                                    receivedByteCount,
                                    receivedTime,
                                    drainingSocketReceiveBuffer );

        mAverageThroughputs[1].bytesReceived(
                                    receivedByteCount,
                                    receivedTime,
                                    drainingSocketReceiveBuffer );

        mAverageThroughputs[2].bytesReceived(
                                    receivedByteCount,
                                    receivedTime,
                                    drainingSocketReceiveBuffer );

        mEventListenerPtr->reportBytesReceived(
                                        mId,
                                        receivedTime,
                                        receivedByteCount,
                                        drainingSocketReceiveBuffer,
                                        earliestTime );
    }
}

/** Report an HTTP connect failure. */
void ByteRangeDownloader::reportHttpConnectFailure(
                                        int32_t failureCode,
                                        const std::string& serverHostName )
{
#ifndef DEBUG
    NRDP_UNUSED(serverHostName);
    NRDP_UNUSED(failureCode);
#endif

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u (info) HTTP connect failure: %d %s",
        mParentId,
        mId,
        failureCode,
        serverHostName.c_str());
}

/** Report a TCP connect start. */
void ByteRangeDownloader::reportTcpStart(
                                uint32_t connectionId,
                                uint32_t httpId,
                                const std::string& serverHostName,
                                const std::string& serverIpAddress,
                                uint16_t serverPort,
                                bool primaryFlag )
{

#if defined(BUILD_DEBUG)
    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u (info) TCP connect start: %u %u, dst: %s %s:%d, %s.",
        mParentId,
        mId,
        connectionId,
        httpId,
        serverHostName.c_str(),
        serverIpAddress.c_str(),
        serverPort,
        primaryFlag ? "primary" : "secondary" );
#else
    NRDP_UNUSED( connectionId );
    NRDP_UNUSED( httpId );
    NRDP_UNUSED( serverHostName );
    NRDP_UNUSED( serverIpAddress );
    NRDP_UNUSED( serverPort );
    NRDP_UNUSED( primaryFlag );
#endif
}

/** Transit to the next state. */
void ByteRangeDownloader::transit(
                            const AseTimeVal& timeNow,
                            ByteRangeDownloaderBaseState* pNextState )
{
    if (mCurrentStatePtr)
    {
        mCurrentStatePtr->exit( *this, timeNow );
    }

    mCurrentStatePtr = pNextState;

    mCurrentStatePtr->enter( *this, timeNow );

    return;
}

/** Open the connection. */
int32_t ByteRangeDownloader::openConnection(
                                const AseTimeVal& timeNow )
{
    NRDP_UNUSED(timeNow);
    int32_t retVal = AS_UNKNOWN_ERROR;

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u opening a connection to %s:%d, track: %u",
        mParentId,
        mId,
        mDestination.first.c_str(),
        mDestination.second,
        mParentId );

    assert( !mAsyncHttpConnectionPtr );

    retVal = mAsyncHttpClientPtr->openConnection(
                                        UrlEncoder::decode( mDestination.first ),
                                        mDestination.second,
                                        mHttpConnectTimeout,
                                        mHttpStallTimeout,
                                        mSocketReceiveBufferSize,
                                        this,
                                        false,
                                        mAsyncHttpConnectionPtr );

    if ((retVal == AS_NO_ERROR) || (retVal == AS_IN_PROGRESS))
    {
        mAsyncHttpConnectionPtr->setUserData( (void*)(uintptr_t) mParentId );
        mAsyncHttpConnectionPtr->setAuxUserData( (void*)(uintptr_t) mId );

        retVal = AS_NO_ERROR;
    }
    else
    {
        mConnectionFailureCode = retVal;
        mLowLevelFailureCode = mAsyncHttpClientPtr->getLowLevelFailureCode();

        DEBUGLOG(
            ByteRangeDownloader,
            "%u %u failed to open a connection to %s:%d, track: %u, status %d %d",
            mParentId,
            mId,
            mDestination.first.c_str(),
            mDestination.second,
            mParentId,
            mConnectionFailureCode,
            mLowLevelFailureCode );

    }

    return retVal;
}

/** Close the connection. */
int32_t ByteRangeDownloader::closeConnection(
                                const AseTimeVal& timeNow, int32_t reason )
{
    NRDP_UNUSED(timeNow);
    int32_t retVal = AS_NO_ERROR;

    if (mAsyncHttpConnectionPtr)
    {
        DEBUGLOG(
            ByteRangeDownloader,
            "%u %u closing the connection ID: %d, reason %d",
            mParentId,
            mId,
            (int32_t) mAsyncHttpConnectionPtr->getId(),
            reason );

        mAsyncHttpClientPtr->closeConnection(   mAsyncHttpConnectionPtr,
                                                mHttpRequestTaskList.empty() && ( reason != AS_CONNECTION_SLOW ),
                                                reason );

        mAsyncHttpConnectionPtr.reset();
    }

    return retVal;
}

/** Report download record. */
void ByteRangeDownloader::reportDownloadRecord(
                                HttpAttempt& httpAttempt,
                                const AseTimeVal& timeNow,
                                int32_t errorCode )
{
    httpAttempt.mErrorCode = errorCode;
    httpAttempt.complete( timeNow );

    mEventListenerPtr->reportDownloadRecord(
                                    mId,
                                    httpAttempt );

    return;
}

/** Clear HTTP requests. */
void ByteRangeDownloader::clearHttpRequests(
                                const AseTimeVal& /* timeNow */ )
{
    HttpRequestTaskList::iterator httpRequestTaskItr;

    httpRequestTaskItr = mHttpRequestTaskList.begin();

    while (httpRequestTaskItr != mHttpRequestTaskList.end())
    {
        if ((*httpRequestTaskItr)->mAsyncHttpRequestPtr)
        {
            DEBUGLOG(
                ByteRangeDownloader,
                "%u %u freed an HTTP request, request URL: %s, "
                "byte range: %lld-%lld, received byte count: %llu.",
                mParentId,
                mId,
                (*httpRequestTaskItr)->mRequestUrl.str().c_str(),
                (*httpRequestTaskItr)->mByteRange.getStart(),
                (*httpRequestTaskItr)->mByteRange.getEnd(),
                (*httpRequestTaskItr)->mReceivedByteCount );

            (*httpRequestTaskItr)->mAsyncHttpRequestPtr.reset();
        }

        httpRequestTaskItr++;
    }

    return;
}

/** Process HTTP 2XX response. */
int32_t ByteRangeDownloader::processHttp2XXResponse(
                                    const AseTimeVal& timeNow,
                                    ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert (!mHttpRequestTaskList.empty());
    assert (!mByteRangeReceivingTaskList.empty());

    HttpRequestTaskList::iterator httpRequestTaskItr;
    ByteRangeReceivingTaskList::iterator byteRangeReceivingTaskItr;

    unsigned char* pBuffer;
    uint32_t skipBytes;
    uint32_t bufferSize;
    uint32_t bufferRecordIndex;

    /*
    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u received an HTTP 2XX response.",
        mParentId,
        mId );
    */

    httpRequestTaskItr = mHttpRequestTaskList.begin();

    byteRangeReceivingTaskItr = mByteRangeReceivingTaskList.begin();

    while (true)
    {
        ByteRange const range( byteRangeReceivingTaskItr->mByteRange.start() + byteRangeReceivingTaskItr->mReceivedByteCount,
                               byteRangeReceivingTaskItr->mByteRange.end() );

        assert( ((*httpRequestTaskItr)->mByteRange.getStart() + (*httpRequestTaskItr)->mReceivedByteCount) == (ullong) range.start());

        assert( range.containedIn( (*httpRequestTaskItr)->mByteRange ) );

        //Try to get a buffer for reading the data.
        //XXX Adjust the buffer request with the received bytes
        retVal = byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getBuffer(
                                                        range,
                                                        skipBytes,
                                                        pBuffer,
                                                        bufferSize,
                                                        bufferRecordIndex );

        if (retVal != AS_NO_ERROR)
        {
            DEBUGLOG(
                ByteRangeDownloader,
                "%u %u could not get free media buffer, ret: %d.",
                mParentId,
                mId,
                retVal );

            //There is no media buffer available.
            mAsyncHttpConnectionPtr->pauseReceiving();

            (*httpRequestTaskItr)->mHttpAttempt.stop( timeNow );

            mMediaBufferAvailableFlag = false;

            transit( timeNow, &SM_SUSPENDED_STATE );

            retVal = AS_MEDIA_BUFFER_UNAVAILABLE;

            break;
        }

        //Skip bytes if we need to
        uint32_t skippedBytes = skipBytes;
        if ( skipBytes > 0 )
        {
            retVal = (*httpRequestTaskItr)->mAsyncHttpRequestPtr->discardResponseBody( skippedBytes, /*limit*/ true );

            if ( skippedBytes > 0 && ( retVal == AS_NO_ERROR || retVal == 1 ) )
            {
                byteRangeReceivingTaskItr->
                    mUrlRequestTaskPtr->
                        commitSkippedBytes( range.prefix( skippedBytes ) );
            }
        }

        //Read the data to the buffer.
        if ( retVal == AS_NO_ERROR && pBuffer && bufferSize > 0 && skippedBytes == skipBytes )
        {
            retVal = (*httpRequestTaskItr)->mAsyncHttpRequestPtr->getResponseBody(
                                                                        pBuffer,
                                                                        bufferSize );
            if ( bufferSize > 0 && ( retVal == AS_NO_ERROR || retVal == 1 ) )
            {
                byteRangeReceivingTaskItr->
                    mUrlRequestTaskPtr->
                        commitReceivedBytes(    ByteRange( range.start() + skippedBytes, range.start() + bufferSize + skippedBytes - 1 ),
                                                bufferRecordIndex );
            }
        }
        else
        {
            bufferSize = 0;
        }

        if ((retVal != AS_NO_ERROR) && (retVal != 1))
        {
            mConnectionFailureCode = retVal;
            mLowLevelFailureCode = mAsyncHttpConnectionPtr->getLowLevelFailureCode();

            reportDownloadRecord(
                        (*httpRequestTaskItr)->mHttpAttempt,
                        timeNow,
                        mConnectionFailureCode );

            transit( timeNow, &SM_CONNECTION_DOWN_STATE );

            break;
        }

        //We actually handled some data.
        if (bufferSize > 0 || skippedBytes > 0)
        {
            (*httpRequestTaskItr)->mReceivedByteCount += bufferSize + skippedBytes;

            mEventListenerPtr->reportDataReceived( mId );

            mTotalReceivedByteCount += bufferSize + skippedBytes;

            byteRangeReceivingTaskItr->mReceivedByteCount += bufferSize + skippedBytes;

            byteRangeReceivingTaskItr->mDataReceivedSinceEnteringDownloadingState = true;

            if (byteRangeReceivingTaskItr->mReceivedByteCount == (ullong) byteRangeReceivingTaskItr->mByteRange.size() )
            {
                DEBUGLOG(
                    ByteRangeDownloader,
                    "%u %u completed a byte range receiving task, URL request task ID: %u, "
                    "byte range: %lld-%lld.",
                    mParentId,
                    mId,
                    byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId(),
                    byteRangeReceivingTaskItr->mByteRange.getStart(),
                    byteRangeReceivingTaskItr->mByteRange.getEnd()
                    );

                byteRangeReceivingTaskList.push_back( *byteRangeReceivingTaskItr );

                mByteRangeReceivingTaskList.pop_front();

                byteRangeReceivingTaskItr = mByteRangeReceivingTaskList.begin();
            }
        }

        if (retVal == 1)
        {
            NTRACE(
                    TRACE_STREAMINGSTAT,
                "%u %u completed an HTTP request, response code: %u, url: %s, byte range: %lld-%lld.",
                mParentId,
                mId,
                (*httpRequestTaskItr)->mAsyncHttpRequestPtr->getResponseCode(),
                (*httpRequestTaskItr)->mRequestUrl.GetPrefix().c_str(),
                (*httpRequestTaskItr)->mByteRange.getStart(),
                (*httpRequestTaskItr)->mByteRange.getEnd() );

            mTotalCompletedRequestCount++;

            reportDownloadRecord(
                    (*httpRequestTaskItr)->mHttpAttempt,
                    timeNow,
                    AS_NO_ERROR );

            mLastRequestUrl = (*httpRequestTaskItr)->mRequestUrl;

            mHttpRequestTaskList.pop_front();

            if (mHttpRequestTaskList.empty())
            {
                transit( timeNow, &SM_IDLE_STATE );
            }
            else if ( byteRangeReceivingTaskItr != mByteRangeReceivingTaskList.end()
                    && byteRangeReceivingTaskItr->mUrlRequestTaskPtr->isAbandoned() )
            {
                retVal = AS_ABANDONED;

                break;
            }
            else
            {
                if ( mHttpRequestTaskList.front()->mAsyncHttpRequestPtr->getState()
                            <= IAsyncHttpRequest::WAITING_STATE )
                {
                    if ( mMinimumDuplicateTimeout > 0 )
                    {
                        setDuplicateRequestTimeout( timeNow );
                    }

                    mAverageThroughputs[0].readingPaused( timeNow );
                    mAverageThroughputs[1].readingPaused( timeNow );
                    mAverageThroughputs[2].readingPaused( timeNow );
                }
            }

            retVal = AS_NO_ERROR;

            break;
        }


        if (byteRangeReceivingTaskItr == mByteRangeReceivingTaskList.end())
        {
            assert( mByteRangeReceivingTaskList.empty() );

            mAsyncHttpConnectionPtr->pauseReceiving();

            (*httpRequestTaskItr)->mHttpAttempt.stop( timeNow );

            transit( timeNow, &SM_SUSPENDED_STATE );

            retVal = AS_NO_ERROR;

            break;
        }

        //Break the loop either we did not actually get any data or we have completed
        //a byte range request in this loop.
        if ((bufferSize == 0 && skippedBytes == 0) || (!byteRangeReceivingTaskList.empty()))
        {
            retVal = AS_NO_ERROR;

            break;
        }

    }

    if ( !mHttpRequestTaskList.empty()
            && ( mHttpRequestTaskList.front()->mHttpAttempt.mTtrace != AseTimeVal::ZERO )
            && ( timeNow - mHttpRequestTaskList.front()->mHttpAttempt.mTtrace > mDownloadReportInterval ) )
    {
        mHttpRequestTaskList.front()->mHttpAttempt.stop( timeNow );

        mEventListenerPtr->reportDownloadRecord(
                                    mId,
                                    mHttpRequestTaskList.front()->mHttpAttempt );

        mHttpRequestTaskList.front()->mHttpAttempt.pop( timeNow );
    }

    return retVal;
}

/** Process HTTP 3XX response. */
int32_t ByteRangeDownloader::processHttp3XXResponse(
                                const AseTimeVal& timeNow,
                                ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    int32_t retVal = AS_NO_ERROR;

    uint32_t discardedBytes;

    assert (!mHttpRequestTaskList.empty());
    assert (!mByteRangeReceivingTaskList.empty());

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u received an HTTP 3XX response.",
        mParentId,
        mId );

    HttpRequestTaskList::iterator httpRequestTaskItr = mHttpRequestTaskList.begin();

    retVal = (*httpRequestTaskItr)->mAsyncHttpRequestPtr->discardResponseBody( discardedBytes );

    if ((retVal == 1) || (retVal == AS_NO_ERROR))
    {
        if (retVal == 1)
        {
            DEBUGLOG(
                ByteRangeDownloader,
                "%u %u completed an HTTP request, response code: %u, request URL: %s, byte range: %lld-%lld.",
                mParentId,
                mId,
                (*httpRequestTaskItr)->mAsyncHttpRequestPtr->getResponseCode(),
                (*httpRequestTaskItr)->mRequestUrl.str().c_str(),
                (*httpRequestTaskItr)->mByteRange.getStart(),
                (*httpRequestTaskItr)->mByteRange.getEnd() );

            mTotalCompletedRequestCount++;

            //Get the location header.
            std::string newLocation = (*httpRequestTaskItr)
                                        ->mAsyncHttpRequestPtr
                                            ->getResponseHeaderValue( "Location" );

            AseUrl newUrl( newLocation );

            if (!newUrl.IsEmpty() && newUrl.IsValid())
            {
                mLastUrlRedirectOriginalUrl = (*httpRequestTaskItr)->mRequestUrl;

                mLastUrlRedirectNewUrl = newUrl;

                if (mLastUrlRedirectNewUrl.HasRelativePath())
                {
                    mLastUrlRedirectNewUrl.Resolve( (*httpRequestTaskItr)->mRequestUrl );
                }

                (*httpRequestTaskItr)->mHttpAttempt.mLocation = mLastUrlRedirectNewUrl;

                DEBUGLOG(
                    ByteRangeDownloader,
                    "%u %u received a URL redirect, original URL: %s, redirect URL: %s, resolved URL: %s, ",
                    mParentId,
                    mId,
                    mLastUrlRedirectOriginalUrl.str().c_str(),
                    newUrl.str().c_str(),
                    mLastUrlRedirectNewUrl.str().c_str() );

                ByteRangeReceivingTaskList::iterator byteRangeReceivingTaskItr;

                byteRangeReceivingTaskItr = mByteRangeReceivingTaskList.begin();

                while ((byteRangeReceivingTaskItr != mByteRangeReceivingTaskList.end())
                       && (byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId()))
                {
                    DEBUGLOG(
                        ByteRangeDownloader,
                        "%u %u returned a byte range receiving task, URL request task ID: %u, "
                        "byte range: %lld-%lld.",
                        mParentId,
                        mId,
                        byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId(),
                        byteRangeReceivingTaskItr->mByteRange.getStart(),
                        byteRangeReceivingTaskItr->mByteRange.getEnd() );

                    byteRangeReceivingTaskList.push_back( *byteRangeReceivingTaskItr );

                    mByteRangeReceivingTaskList.pop_front();

                    byteRangeReceivingTaskItr = mByteRangeReceivingTaskList.begin();
                }

                reportDownloadRecord(
                            (*httpRequestTaskItr)->mHttpAttempt,
                            timeNow,
                            AS_NO_ERROR );

                mLastRequestUrl = (*httpRequestTaskItr)->mRequestUrl;

                mHttpRequestTaskList.pop_front();

                if (mHttpRequestTaskList.empty())
                {
                    transit( timeNow, &SM_IDLE_STATE );
                }
                else if (mByteRangeReceivingTaskList.empty())
                {
                    mAsyncHttpConnectionPtr->pauseReceiving();

                    transit( timeNow, &SM_SUSPENDED_STATE );
                }
                else if ( mByteRangeReceivingTaskList.front().mUrlRequestTaskPtr->isAbandoned() )
                {
                    retVal = AS_ABANDONED;
                }
                else
                {
                    if (mMinimumDuplicateTimeout > 0
                            && mHttpRequestTaskList.front()->mAsyncHttpRequestPtr->getState()
                                <= IAsyncHttpRequest::WAITING_STATE)
                    {
                        setDuplicateRequestTimeout( timeNow );
                    }
                }

                retVal = AS_HTTP_RESPONSE_3XX;
            }
            else
            {
                //There is no location header in the response.

                mConnectionFailureCode = AS_HTTP_PROTOCOL_ERROR;
                mLowLevelFailureCode = 0;

                reportDownloadRecord(
                            (*httpRequestTaskItr)->mHttpAttempt,
                            timeNow,
                            mConnectionFailureCode );

                transit( timeNow, &SM_CONNECTION_DOWN_STATE );

                retVal = mConnectionFailureCode;
            }
        }
    }
    else
    {
        //There is something during receiving.
        mConnectionFailureCode = retVal;
        mLowLevelFailureCode = mAsyncHttpConnectionPtr->getLowLevelFailureCode();

        reportDownloadRecord(
                    (*httpRequestTaskItr)->mHttpAttempt,
                    timeNow,
                    mConnectionFailureCode );

        transit( timeNow, &SM_CONNECTION_DOWN_STATE );
    }

    return retVal;
}

/** Process HTTP 4XX response. */
int32_t ByteRangeDownloader::processHttp4XXResponse(
                                const AseTimeVal& timeNow,
                                ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    int32_t retVal = AS_NO_ERROR;

    uint32_t discardedBytes;

    assert (!mHttpRequestTaskList.empty());
    assert (!mByteRangeReceivingTaskList.empty());

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u received an HTTP 4XX response.",
        mParentId,
        mId );

    HttpRequestTaskList::iterator httpRequestTaskItr = mHttpRequestTaskList.begin();

    retVal = (*httpRequestTaskItr)->mAsyncHttpRequestPtr->discardResponseBody( discardedBytes );

    if ((retVal == 1) || (retVal == AS_NO_ERROR))
    {
        if (retVal == 1)
        {
            DEBUGLOG(
                ByteRangeDownloader,
                "%u %u completed an HTTP request, response code: %u, request URL: %s, byte range: %lld-%lld.",
                mParentId,
                mId,
                (*httpRequestTaskItr)->mAsyncHttpRequestPtr->getResponseCode(),
                (*httpRequestTaskItr)->mRequestUrl.str().c_str(),
                (*httpRequestTaskItr)->mByteRange.getStart(),
                (*httpRequestTaskItr)->mByteRange.getEnd() );

            mTotalCompletedRequestCount++;

            ByteRangeReceivingTaskList::iterator byteRangeReceivingTaskItr;

            byteRangeReceivingTaskItr = mByteRangeReceivingTaskList.begin();

            while ((byteRangeReceivingTaskItr != mByteRangeReceivingTaskList.end())
                   && (byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId()
                                == (*httpRequestTaskItr)->mUrlRequestTaskId))
            {
                DEBUGLOG(
                    ByteRangeDownloader,
                    "%u %u returned a byte range receiving task, URL request task ID: %u, "
                    "byte range: %lld-%lld.",
                    mParentId,
                    mId,
                    byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId(),
                    byteRangeReceivingTaskItr->mByteRange.getStart(),
                    byteRangeReceivingTaskItr->mByteRange.getEnd() );

                byteRangeReceivingTaskList.push_back( *byteRangeReceivingTaskItr );

                mByteRangeReceivingTaskList.pop_front();

                byteRangeReceivingTaskItr = mByteRangeReceivingTaskList.begin();
            }

            reportDownloadRecord(
                    (*httpRequestTaskItr)->mHttpAttempt,
                    timeNow,
                    AS_NO_ERROR );

            mLastRequestUrl = (*httpRequestTaskItr)->mRequestUrl;

            mHttpRequestTaskList.pop_front();

            if (mHttpRequestTaskList.empty())
            {
                transit( timeNow, &SM_IDLE_STATE );
            }
            else if (mByteRangeReceivingTaskList.empty())
            {
                mAsyncHttpConnectionPtr->pauseReceiving();

                transit( timeNow, &SM_SUSPENDED_STATE );
            }
            else if ( mByteRangeReceivingTaskList.front().mUrlRequestTaskPtr->isAbandoned() )
            {
                retVal = AS_ABANDONED;
            }
            else
            {
                if (mMinimumDuplicateTimeout > 0
                        && mHttpRequestTaskList.front()->mAsyncHttpRequestPtr->getState()
                            <= IAsyncHttpRequest::WAITING_STATE)
                {
                    setDuplicateRequestTimeout( timeNow );
                }
            }

            retVal = AS_HTTP_RESPONSE_4XX;
        }
    }
    else
    {
        //There is something during receiving.
        mConnectionFailureCode = retVal;
        mLowLevelFailureCode = mAsyncHttpConnectionPtr->getLowLevelFailureCode();

        reportDownloadRecord(
                    (*httpRequestTaskItr)->mHttpAttempt,
                    timeNow,
                    mConnectionFailureCode );

        transit( timeNow, &SM_CONNECTION_DOWN_STATE );
    }

    return retVal;
}

/** Process HTTP 5XX response. */
int32_t ByteRangeDownloader::processHttp5XXResponse(
                                const AseTimeVal& timeNow,
                                ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    int32_t retVal = AS_NO_ERROR;

    uint32_t discardedBytes;

    assert (!mHttpRequestTaskList.empty());
    assert (!mByteRangeReceivingTaskList.empty());

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u received an HTTP 5XX response.",
        mParentId,
        mId );

    HttpRequestTaskList::iterator httpRequestTaskItr = mHttpRequestTaskList.begin();

    retVal = (*httpRequestTaskItr)->mAsyncHttpRequestPtr->discardResponseBody( discardedBytes );

    if ((retVal == 1) || (retVal == AS_NO_ERROR))
    {
        if (retVal == 1)
        {
            DEBUGLOG(
                ByteRangeDownloader,
                "%u %u completed an HTTP request, response code: %u, request URL: %s, byte range: %lld-%lld.",
                mParentId,
                mId,
                (*httpRequestTaskItr)->mAsyncHttpRequestPtr->getResponseCode(),
                (*httpRequestTaskItr)->mRequestUrl.str().c_str(),
                (*httpRequestTaskItr)->mByteRange.getStart(),
                (*httpRequestTaskItr)->mByteRange.getEnd() );

            mTotalCompletedRequestCount++;

            ByteRangeReceivingTaskList::iterator byteRangeReceivingTaskItr;

            byteRangeReceivingTaskItr = mByteRangeReceivingTaskList.begin();

            while ((byteRangeReceivingTaskItr != mByteRangeReceivingTaskList.end())
                   && (byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId()
                                == (*httpRequestTaskItr)->mUrlRequestTaskId))
            {
                DEBUGLOG(
                    ByteRangeDownloader,
                    "%u %u returned a byte range receiving task, URL request task ID: %u, "
                    "byte range: %lld-%lld, timeout duration: %lld.",
                    mParentId,
                    mId,
                    byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId(),
                    byteRangeReceivingTaskItr->mByteRange.getStart(),
                    byteRangeReceivingTaskItr->mByteRange.getEnd(),
                    /*byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getRequestTimeout() == AseTimeVal::INFINITE ?
                        -1 : (llong) byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getRequestTimeout().ms()*/ (llong)  -1 );

                byteRangeReceivingTaskList.push_back( *byteRangeReceivingTaskItr );

                mByteRangeReceivingTaskList.pop_front();

                byteRangeReceivingTaskItr = mByteRangeReceivingTaskList.begin();
            }

            reportDownloadRecord(
                        (*httpRequestTaskItr)->mHttpAttempt,
                        timeNow,
                        AS_NO_ERROR );

            mLastRequestUrl = (*httpRequestTaskItr)->mRequestUrl;

            mHttpRequestTaskList.pop_front();

            if (mHttpRequestTaskList.empty())
            {
                transit( timeNow, &SM_IDLE_STATE );
            }
            else if (mByteRangeReceivingTaskList.empty())
            {
                mAsyncHttpConnectionPtr->pauseReceiving();

                transit( timeNow, &SM_SUSPENDED_STATE );
            }
            else if ( mByteRangeReceivingTaskList.front().mUrlRequestTaskPtr->isAbandoned() )
            {
                retVal = AS_ABANDONED;
            }
            else
            {
                if (mMinimumDuplicateTimeout > 0
                        && mHttpRequestTaskList.front()->mAsyncHttpRequestPtr->getState()
                            <= IAsyncHttpRequest::WAITING_STATE)
                {
                    setDuplicateRequestTimeout( timeNow );
                }
            }


            retVal = AS_HTTP_RESPONSE_5XX;
        }
    }
    else
    {
        //There is something during receiving.
        mConnectionFailureCode = retVal;
        mLowLevelFailureCode = mAsyncHttpConnectionPtr->getLowLevelFailureCode();

        reportDownloadRecord(
                    (*httpRequestTaskItr)->mHttpAttempt,
                    timeNow,
                    mConnectionFailureCode );

        transit( timeNow, &SM_CONNECTION_DOWN_STATE );
    }

    return retVal;
}

void ByteRangeDownloader::setDuplicateRequestTimeout( AseTimeVal const& timeNow )
{
    // Set the timeout for duplicating the next request
    mTimerOne = timeNow + std::max( mMinimumDuplicateTimeout,
                                    AseTimeVal::fromMS( mDuplicateTimeoutMultiplier *
                                        mAverageResponseTime.getAverageResponseTime() / 10 ) );
/*
    DEBUGLOG( ByteRangeDownloader,
            "%u %u setting duplicate timeout to %llums (average response time %llums, %llums from timeNow)",
            mParentId,
            mId,
            ( mTimerOne - timeNow ).ms(),
            mAverageResponseTime.getAverageResponseTime(),
            ( AseTimeVal::now() - timeNow ).ms() );
*/
}

/** Process HTTP invalid response. */
int32_t ByteRangeDownloader::processHttpInvalidResponse(
                                const AseTimeVal& timeNow,
                                ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    int32_t retVal = AS_NO_ERROR;

    assert( !mHttpRequestTaskList.empty() );
    assert( !mByteRangeReceivingTaskList.empty() );

    DEBUGLOG(
        ByteRangeDownloader,
        "%u %u received an HTTP invalid response.",
        mParentId,
        mId );

    ByteRangeReceivingTaskList::iterator byteRangeReceivingTaskItr;

    byteRangeReceivingTaskItr = mByteRangeReceivingTaskList.begin();

    while (byteRangeReceivingTaskItr != mByteRangeReceivingTaskList.end())
    {
        DEBUGLOG(
            ByteRangeDownloader,
            "%u %u returned a byte range receiving task, URL request task ID: %u, "
            "byte range: %lld-%lld, timeout duration: %lld.",
            mParentId,
            mId,
            byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId(),
            byteRangeReceivingTaskItr->mByteRange.getStart(),
            byteRangeReceivingTaskItr->mByteRange.getEnd(),
            /*byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getRequestTimeout() == AseTimeVal::INFINITE ?
                -1 : (llong) byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getRequestTimeout().ms()*/ (llong)  -1 );

        byteRangeReceivingTaskList.push_back( *byteRangeReceivingTaskItr );

        mByteRangeReceivingTaskList.pop_front();

        byteRangeReceivingTaskItr = mByteRangeReceivingTaskList.begin();
    }

    mConnectionFailureCode = AS_HTTP_PROTOCOL_ERROR;
    mLowLevelFailureCode = 0;

    reportDownloadRecord(
                mHttpRequestTaskList.front()->mHttpAttempt,
                timeNow,
                mConnectionFailureCode );

    transit( timeNow, &SM_CONNECTION_DOWN_STATE );

    retVal = mConnectionFailureCode;

    return retVal;
}

/**
 *  @class HttpRequestTask
 */
/** Constructor. */
ByteRangeDownloader::HttpRequestTask::HttpRequestTask(
                                        uint32_t urlRequestTaskId,
                                        const AseUrl& requestUrl,
                                        const ByteRange& byteRange,
                                        bool pipelined,
                                        int reportInterval )
                                            : mUrlRequestTaskId( urlRequestTaskId ),
                                              mRequestUrl( requestUrl ),
                                              mByteRange( byteRange ),
                                              mReceivedByteCount( 0 ),
                                              mNonPipelined( false ),
                                              mFreshFlag( true )
{
    mHttpAttempt.mUrl = requestUrl;
    mHttpAttempt.mRange = byteRange;
    mHttpAttempt.mOpenRange = byteRange.open();
    mHttpAttempt.mPipelined = pipelined;
    mHttpAttempt.mBrecv = 0;
    mHttpAttempt.mTinterval = reportInterval;
}

/** Destructor. */
ByteRangeDownloader::HttpRequestTask::~HttpRequestTask()
{
}

/** ITraceListener */
void ByteRangeDownloader::HttpRequestTask::reportBytesReceived(
                                                        AseTimeVal const& time,
                                                        uint32_t bytes,
                                                        bool header,
                                                        bool complete )
{
    mHttpAttempt.add( time, bytes, header, complete );
}

}}
