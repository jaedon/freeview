/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "DebugLogging.h"
#include "AseCommonUtil.h"
#include <nrdbase/PerformanceMarker.h>
#include "HttpRequestManager.h"
#include "McplTrackDownloader2.h"

using namespace std;

namespace netflix {
namespace ase {

DEFINE_LOGGER( McplTrackDownloader2 );

/**
 *  @class McplTrackDownloader2
 *
 *  @brief This class is the multi connection pipelined track downloader class.
 */

/** Constructor. */
McplTrackDownloader2::McplTrackDownloader2(
                                uint32_t trackId,
                                MediaType mediaType,
                                TrackConfiguration const& config,
                                HttpRequestManagerPtr pHttpRequestManager,
                                IAsyncHttpClientPtr pAsyncHttpClient,
                                ITransportReporterPtr pTransportReporter,
                                IAseConfigParameterPtr pAseConfigParameter,
                                UrlRedirectMapPtr pUrlRedirectMap )
                                            : BaseTrackDownloader2(
                                                            trackId,
                                                            MCPL_TYPE,
                                                            mediaType,
                                                            config,
                                                            pHttpRequestManager,
                                                            pAsyncHttpClient,
                                                            pTransportReporter,
                                                            pUrlRedirectMap )
                                            , mState( UNINITIALIZED_STATE )
                                            , mDisconnectedWhileIdle( false )
{
    (void) configure( pAseConfigParameter, config );
}

int32_t McplTrackDownloader2::configure(    IAseConfigParameterPtr pAseConfigParameter,
                                            TrackConfiguration const& config )
{
    // Check the new connection config is compatible with the old
    // To change socket buffer size, we have to close and re-open all the connections anyway,
    // so we might as well have the caller destroy this TrackDownloader and create a new one
    if ( ( mState != UNINITIALIZED_STATE ) &&
            ( config.mOpenRange
            || config.mPipeline != mTrackConfiguration.mPipeline
            || config.mSocketBufferSize != mTrackConfiguration.mSocketBufferSize ) )
    {
        DEBUGLOG( McplTrackDownloader2,
                "%u Can't reconfigure. OpenRange(%d), Pipeline(%d,%d), SocketBuffer(%d,%d)",
                mTrackId,
                config.mOpenRange,
                config.mPipeline, mTrackConfiguration.mPipeline,
                config.mSocketBufferSize, mTrackConfiguration.mSocketBufferSize );

        return AS_NOT_SUPPORTED;
    }

    int32_t retVal = BaseTrackDownloader2::configure( pAseConfigParameter, config );
    if ( retVal != AS_NO_ERROR )
        return retVal;

    mMinResponseTime = mAseConfigParameterPtr->getParameterAs<int>(
                                                        "minResponseTime",
                                                        100 );

    mResponseTimeFreshnessDuration = mAseConfigParameterPtr->getParameterAs<int>(
                                                        "responseTimeFreshnessDuration",
                                                        30000 );

    mSchedulingTargetMultiplyFactor = mAseConfigParameterPtr->getParameterAs<int>(
                                                        "schedulingTargetMultiplyFactor",
                                                        5 );

    mSchedulingTriggerMultiplyFactor = mAseConfigParameterPtr->getParameterAs<int>(
                                                        "schedulingTriggerMultiplyFactor",
                                                        3 );

    mTargetScheduleTime = mAseConfigParameterPtr->getParameterAs<int>(
                                                        "targetScheduleTime",
                                                        2000 );

    mMinRequestSize = mAseConfigParameterPtr->getParameterAs<int>(
                                                        "minRequestSize",
                                                        4 * 1024 );

    mPipeliningStabilityRequestThreshold = mAseConfigParameterPtr->getParameterAs<int>(
                                                        "pipeliningStabilityRequestThreshold",
                                                        5 );

    mPipeliningStabilityConnectionThreshold = mAseConfigParameterPtr->getParameterAs<int>(
                                                        "pipeliningStabilityConnectionThreshold",
                                                        6 );

    mMinimumDuplicateTimeout = AseTimeVal::fromMS( mAseConfigParameterPtr->getParameterAs<int>(
                                                    "minimumDuplicateTimeout",
                                                    0, 10000, 0) );

    mDuplicateTimeoutMultiplier = mAseConfigParameterPtr->getParameterAs<int>(
                                                    "duplicateTimeoutMultiplier",
                                                    0, 100, 30 );

    mSlowConnectionThreshold = mAseConfigParameterPtr->getParameterAs<int>(
                                                    "slowConnectionThreshold",
                                                    0, 50, 0 );

    mMinSlowConnectionDuration = AseTimeVal::fromMS( mAseConfigParameterPtr->getParameterAs<int>(
                                                    "minSlowConnectionDuration",
                                                    0, 10000, 1000 ) );


    mCheckBlockDeadlineForSlowConnections = mAseConfigParameterPtr->getParameterAs<bool>(
                                                    "checkBlockDeadlineForSlowConnections", true );

    mByteRangeAlignment = mAseConfigParameterPtr->getParameterAs<uint32_t>(
                                                    "byteRangeAlignment", 1, 65536, 4096 );


    if ( mState != UNINITIALIZED_STATE )
    {
        AseTimeVal const timeNow = AseTimeVal::now();

        while ( mTrackConfiguration.mConnections < (uint32_t) mByteRangeDownloaderMap.size() )
        {
            ByteRangeDownloaderMap::iterator byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

            ByteRangeDownloader::State const brdstate = byteRangeDownloaderItr->second->getState();

            if ( brdstate == ByteRangeDownloader::DOWNLOADING_STATE
                    || brdstate == ByteRangeDownloader::SUSPENDED_STATE )
            {
                returnByteRangeReceivingTasks( byteRangeDownloaderItr->second );
            }

            byteRangeDownloaderItr->second->disconnect( timeNow );

            DEBUGLOG(
                McplTrackDownloader2,
                "%u disconnected byte range downloader %u from %s:%d on config change",
                mTrackId,
                byteRangeDownloaderItr->first,
                mCurrentConnectedDestination.first.c_str(),
                mCurrentConnectedDestination.second );

            if ( byteRangeDownloaderItr->first == mBlockedByteRangeDownloaderId )
            {
                mBlockedByteRangeDownloaderId = 0xFFFFFFFF;
            }

            mByteRangeDownloaderMap.erase( byteRangeDownloaderItr );
        }

        // Adjust configuration of existing byte range downloaders
        ByteRangeDownloaderMap::iterator byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();
        for( ; byteRangeDownloaderItr != mByteRangeDownloaderMap.end(); ++byteRangeDownloaderItr )
        {
            configureByteRangeDownloader( byteRangeDownloaderItr->second );
        }

        while ( mTrackConfiguration.mConnections > (uint32_t) mByteRangeDownloaderMap.size() )
        {
            // Establish a new connection
            ByteRangeDownloaderPtr pByteRangeDownloader(
                                        new ByteRangeDownloader(
                                                mTrackId,
                                                mAsyncHttpClientPtr,
                                                shared_from_this() ) );
            if ( !pByteRangeDownloader )
                break;

            configureByteRangeDownloader( pByteRangeDownloader );

            mByteRangeDownloaderMap[ pByteRangeDownloader->getId() ] = pByteRangeDownloader;
        }

        mTargetConnectionCount = mByteRangeDownloaderMap.size();
    }

    return AS_NO_ERROR;
}

/** Destructor. */
McplTrackDownloader2::~McplTrackDownloader2()
{
}

/** Test whether this downloader is in idle state */
bool McplTrackDownloader2::idle() const
{
    return ( mState == INIT_STATE );
}

void McplTrackDownloader2::configureByteRangeDownloader( ByteRangeDownloaderPtr pByteRangeDownloader )
{
    pByteRangeDownloader->setSocketReceiveBufferSize( mTrackConfiguration.mSocketBufferSize );
    pByteRangeDownloader->setReconnectBackoffTime( mDomainReconnectBackoffTime, mSyncDomainReconnectBackoffTime, mMaxSyncConnectFailures );
    pByteRangeDownloader->setTransportReportInterval( mTransportReportInterval );
    pByteRangeDownloader->setDownloadReportInterval( mDownloadReportInterval );
    pByteRangeDownloader->setThroughputFilterHalfLife( 0, mThroughputFilterHalfLife );
    pByteRangeDownloader->setThroughputMeasurementInterval( 0, mThroughputMeasurementInterval );
    pByteRangeDownloader->setThroughputFilterHalfLife( 2, mThroughputFilterHalfLife );
    pByteRangeDownloader->setThroughputMeasurementInterval( 2, mThroughputMeasurementInterval );
    pByteRangeDownloader->setProcess2xxLoopInterval( mProcess2xxLoopSleepMicros );
    pByteRangeDownloader->setTimeouts(  mHttpConnectTimeout,
                                        mHttpStallTimeout,
                                        mTrackConfiguration.mConnections > 1 ? mMinimumDuplicateTimeout : AseTimeVal::ZERO,
                                        mTrackConfiguration.mConnections > 1 ? mDuplicateTimeoutMultiplier : 0);
}

/** Init the track downloader. */
int32_t McplTrackDownloader2::init()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    uint32_t maxConnectionCount;

    maxConnectionCount = mTrackConfiguration.mConnections;

    mTargetConnectionCount = 0;

    //Create byte range downloaders.
    while (maxConnectionCount > 0)
    {
        ByteRangeDownloaderPtr pByteRangeDownloader(
                                    new ByteRangeDownloader(
                                            mTrackId,
                                            mAsyncHttpClientPtr,
                                            shared_from_this() ) );

        if (pByteRangeDownloader)
        {
            configureByteRangeDownloader( pByteRangeDownloader );

            mByteRangeDownloaderMap[ pByteRangeDownloader->getId() ] = pByteRangeDownloader;

            mTargetConnectionCount++;

            DEBUGLOG(
                McplTrackDownloader2,
                "%u created a byte range downloader: %u.",
                mTrackId,
                pByteRangeDownloader->getId() );
        }
        else
        {
            break;
        }

        maxConnectionCount--;
    }

    if (mTrackConfiguration.mConnections == 1 || mTargetConnectionCount > 1)
    {
        AseTimeVal const now = AseTimeVal::now();

        mState = INIT_STATE;
        mBlockedByteRangeDownloaderId = 0xFFFFFFFF;
        mGettingUrlRequestTasksBlockedFlag = false;
        mMediaBufferAvailableFlag = true;
        mTotalConnectionBrokenCount =  0;
        mDownloadingByteRangDownloaderCount = 0;
        mAverageCompletedHttpRequestCount = 0;
        mLatestHttpResponseTime = AseTimeVal::fromMS( mMinResponseTime );
        mHttpResponseTimeUpdatedTime = now;
        mConnectingTime = AseTimeVal::ZERO;
        mTimerOne = AseTimeVal::ZERO;
        mTimerTwo = AseTimeVal::INFINITE;
        mNextTimeout = AseTimeVal::ZERO;
        mDownloadingPausedFlag = false;
        mCurentStateTime = now;

        retVal = AS_NO_ERROR;
    }
    else if (mTargetConnectionCount == 1)
    {
        DEBUGLOG(
            McplTrackDownloader2,
            "%u was only able to create one byte range downloader, fell back to SCOR mode.",
            mTrackId );

        retVal = AS_HTTP_PIPELINING_NOT_STABLE;
    }
    else
    {
        retVal = AS_NOT_ENOUGH_MEMORY;
    }

    DEBUGLOG(
        McplTrackDownloader2,
        "%u initialized, ret: %d.",
        mTrackId,
        retVal );

    return retVal;
}

/** Deinitialize the track downloader. */
int32_t McplTrackDownloader2::deinit()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    returnAllUrlRequestTasks();

    disconnectByteRangeDownloaders(
                        AseTimeVal::now() );

    mByteRangeDownloaderMap.clear();

    retVal = AS_NO_ERROR;

    DEBUGLOG(
        McplTrackDownloader2,
        "%u deinitialized, ret: %d.",
        mTrackId,
        retVal );

    return retVal;
}

AseUrl::HostPort McplTrackDownloader2::getDestination() const
{
    return mOriginalDestination;
}

/** Get the next timeout time. */
AseTimeVal McplTrackDownloader2::getNextTimeout()
{
    return mNextTimeout;
}

/** Process selected connection. */
int32_t McplTrackDownloader2::processSelectedHttpConnections(
                                    const AseTimeVal& timeNow,
                                    std::vector<IAsyncHttpConnectionPtr>& selectedHttpConnections,
                                    bool& dataReceived,
                                    bool& networkErrors )
{
    PERFORMANCE_MARKER_SCOPED("mcpl.processselected");

    int32_t retVal = AS_NO_ERROR;

    dataReceived = false;
    networkErrors = false;

    uintptr_t byteRangeDownloaderId;

    uint32_t httpResponseCode;

    std::vector<IAsyncHttpConnectionPtr>::iterator asyncHttpConnectionItr;

    asyncHttpConnectionItr = selectedHttpConnections.begin();

    bool bCheckForDuplicateDownloads = false;
    bool bAbandonedTasks = false;

    while (asyncHttpConnectionItr != selectedHttpConnections.end())
    {
        void* ptr = (*asyncHttpConnectionItr)->getAuxUserData();

        byteRangeDownloaderId = (uintptr_t) ptr;

        IAsyncHttpConnection::Event httpConnectionEvent;

        httpConnectionEvent = (*asyncHttpConnectionItr)->retrieveOutstandingEvent();

        if (httpConnectionEvent == IAsyncHttpConnection::STATE_CHANGE_EVENT)
        {
            PERFORMANCE_MARKER_SCOPED("mcpl.processselected.conn");

            //Process the HTTP connection.
            retVal = mByteRangeDownloaderMap[byteRangeDownloaderId]->processHttpConnection(
                                                                                    timeNow );

            if (retVal == AS_NO_ERROR)
            {
                //A connection is established.

                //If necessary, set our timer for leaving CONNECTING state
                if ( mState == CONNECTING_STATE && mTimerOne == AseTimeVal::INFINITE )
                {
                    // Allow double the first TCP connect time
                    mTimerOne = timeNow + ( timeNow - mConnectingTime );
                }

                if (!mMediaBufferAvailableFlag)
                {
                    mByteRangeDownloaderMap[byteRangeDownloaderId]->notifyMediaBufferAvailability(
                                                                        timeNow,
                                                                        false  );
                }
            }
            else
            {
                retVal = processConnectionFailure(
                                    timeNow,
                                    mByteRangeDownloaderMap[byteRangeDownloaderId] );

                networkErrors = networkErrors || ( mState != WAITING_FOR_URL_REQUEST_STATE
                                    && mState != WAITING_FOR_MEDIA_BUFFER_STATE );
            }

            if (retVal != AS_NO_ERROR)
            {
                break;
            }
        }
        else if (httpConnectionEvent == IAsyncHttpConnection::RESPONSE_AVAILABLE_EVENT)
        {
            PERFORMANCE_MARKER_SCOPED("mcpl.processselected.resp");

            ByteRangeReceivingTaskList byteRangeReceivingTaskList;

            //Process the HTTP response.
            retVal = mByteRangeDownloaderMap[byteRangeDownloaderId]->processHttpResponse(
                                                                        timeNow,
                                                                        byteRangeReceivingTaskList );


            httpResponseCode = mByteRangeDownloaderMap[byteRangeDownloaderId]->
                                                            getLastHttpResponseCode();

            if (retVal == AS_NO_ERROR || retVal == AS_ABANDONED )
            {
                if ( retVal == AS_NO_ERROR )
                    dataReceived = true;

                if ( retVal == AS_ABANDONED )
                {
                    DEBUGLOG(
                        McplTrackDownloader2,
                        "%u ByteRangeDownloader %u indicates stopping at abandoned request",
                        mTrackId, (uint32_t) byteRangeDownloaderId );

                    returnByteRangeReceivingTasks( mByteRangeDownloaderMap[byteRangeDownloaderId] );

                    mByteRangeDownloaderMap[byteRangeDownloaderId]->disconnect( timeNow, AS_ABANDONED );

                    mByteRangeDownloaderMap[byteRangeDownloaderId]->reconnect( timeNow );

                    if (mBlockedByteRangeDownloaderId == byteRangeDownloaderId )
                    {
                        mBlockedByteRangeDownloaderId = 0xFFFFFFFF;
                    }

                    bAbandonedTasks = true;

                    retVal = AS_NO_ERROR;
                }

                if (!byteRangeReceivingTaskList.empty())
                {
                    assert( byteRangeReceivingTaskList.size() == 1 );

                    UrlRequestTaskPtr pUrlRequestTask;

                    pUrlRequestTask = byteRangeReceivingTaskList.front().mUrlRequestTaskPtr;

                    if ( pUrlRequestTask->duplicated() )
                    {
                        bCheckForDuplicateDownloads = true;
                    }

                    if (pUrlRequestTask->completed())
                    {
                        if ( removeCompletedUrlRequestTask( pUrlRequestTask ) )
                        {
                            PERFORMANCE_MARKER_SCOPED("mcpl.processselected.completed");

                            retVal = updateUrlRecordListOnTaskComplete(
                                                    retVal,
                                                    httpResponseCode,
                                                    pUrlRequestTask );

                            mHttpRequestManagerPtr->completeUrlRequestTask( pUrlRequestTask );

                            if (mUrlRequestTaskList.empty())
                            {
                                assert(
                                    mUrlRecordList.empty()
                                    && countUnscheduledBytes() == 0 );

                                transitState( timeNow,  WAITING_FOR_URL_REQUEST_STATE );

                                break;
                            }
                        }
                    }
                }
            }
            else if (retVal == AS_MEDIA_BUFFER_UNAVAILABLE)
            {
                ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

                //The media buffer is unavailable, notify all other channels.
                byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

                while (byteRangeDownloaderItr != mByteRangeDownloaderMap.end())
                {
                    if (byteRangeDownloaderItr->first != byteRangeDownloaderId)
                    {
                        byteRangeDownloaderItr->second->notifyMediaBufferAvailability(
                                                                        timeNow,
                                                                        false  );
                    }

                    byteRangeDownloaderItr++;
                }

                mMediaBufferAvailableFlag = false;

                break;
            }
            else if ((retVal == AS_HTTP_RESPONSE_3XX)
                     || (retVal == AS_HTTP_RESPONSE_4XX)
                     || (retVal == AS_HTTP_RESPONSE_5XX)
                     || (retVal == AS_HTTP_PROTOCOL_ERROR))
            {
                assert( !byteRangeReceivingTaskList.empty() );

                if (retVal == AS_HTTP_RESPONSE_3XX)
                {
                    AseUrl originalUrl;
                    AseUrl newUrl;

                    mByteRangeDownloaderMap[byteRangeDownloaderId]->getLastUrlRedirectInfo(
                                                                                originalUrl,
                                                                                newUrl );

                    if ( !addUrlRedirectEntry( originalUrl, newUrl ) )
                    {
                        retVal = AS_HTTP_TOO_MANY_REDIRECTS;

                        reportErrorToUrlRequestTasks(   byteRangeReceivingTaskList.front().mUrlRequestTaskPtr->getCurrentUrl(),
                                                        AS_HTTP_TOO_MANY_REDIRECTS,
                                                        0 );
                    }
                }
                else
                {
                    // when there is no URL from UrlRequestTask, URL can not be
                    // removed from RedirectionEntry. Instead, we log error.
                    if(!byteRangeReceivingTaskList.front().mUrlRequestTaskPtr){
                        ERRORLOG(McplTrackDownloader2, "byteRangeReceivingTaskList.front().mUrlRequestTaskPtr is NULL when retVal is %d", retVal);
                    }else if(!byteRangeReceivingTaskList.front().mUrlRequestTaskPtr->hasUrl()){
                        ERRORLOG(McplTrackDownloader2, "byteRangeReceivingTaskList.front().mUrlRequestTaskPtr->hasUrl() is false when retVal is %d", retVal);
                    } else {
                        removeUrlRedirectEntry(
                            byteRangeReceivingTaskList.front().mUrlRequestTaskPtr->getCurrentUrl() );
                    }

                    reportErrorToUrlRequestTasks(   byteRangeReceivingTaskList.front().mUrlRequestTaskPtr->getCurrentUrl(),
                                                    retVal,
                                                    httpResponseCode );

                }

                retVal = updateUrlRecordListOnTaskComplete(
                                retVal,
                                httpResponseCode,
                                byteRangeReceivingTaskList.front().mUrlRequestTaskPtr );
            }
            else
            {
                retVal = processConnectionFailure(
                                    timeNow,
                                    mByteRangeDownloaderMap[byteRangeDownloaderId] );

                networkErrors = true;
            }
        }

        if (retVal != AS_NO_ERROR)
        {
            break;
        }

        asyncHttpConnectionItr++;
    }

    if (retVal == AS_NO_ERROR)
    {
        PERFORMANCE_MARKER_SCOPED("mcpl.processselected.success");

        if ( mState == CONNECTING_STATE && countActiveConnections() == mTargetConnectionCount )
        {
            transitState( timeNow, WORKING_STATE );
        }

        if ( bCheckForDuplicateDownloads )
        {
            // Check if any byte range downloaders should be stopped from downloading duplicate data
            ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

            byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

            for ( ;byteRangeDownloaderItr != mByteRangeDownloaderMap.end(); ++byteRangeDownloaderItr)
            {
                if ( byteRangeDownloaderItr->second->checkForDuplicateDownloading() )
                {
                    DEBUGLOG(
                        McplTrackDownloader2,
                        "%u disconnecting downloader %u due to completed duplicated tasks",
                        mTrackId,
                        byteRangeDownloaderItr->first );

                    returnByteRangeReceivingTasks( byteRangeDownloaderItr->second );

                    byteRangeDownloaderItr->second->disconnect( timeNow );

                    byteRangeDownloaderItr->second->connect(    timeNow,
                                                                mCurrentConnectedDestination );
                }
            }
        }

        if ( bAbandonedTasks )
        {
            disconnectAbandonedByteRangeDownloaders( timeNow );

            returnAbandonedUrlRequestTasks();

            if (mUrlRequestTaskList.empty())
            {
                assert(
                    mUrlRecordList.empty()
                    && countUnscheduledBytes() == 0 );

                transitState( timeNow,  WAITING_FOR_URL_REQUEST_STATE );
            }
        }

        if (mState == WORKING_STATE || mState == CONNECTING_STATE)
        {
            if ( mSlowConnectionThreshold > 0 )
            {
                closeSlowConnection( timeNow );
            }

            retVal = scheduleByteRanges( timeNow );
        }

        mTimerTwo = getEarliestByteRangeDownloaderTimeout();
    }
    else if (retVal == AS_MEDIA_BUFFER_UNAVAILABLE)
    {
        transitState( timeNow, WAITING_FOR_MEDIA_BUFFER_STATE );

        retVal = AS_NO_ERROR;
    }
    else if (retVal == AS_HTTP_RESPONSE_3XX)
    {
        returnAllUrlRequestTasks();

        disconnectByteRangeDownloaders( timeNow );

        transitState( timeNow, INIT_STATE );

        retVal = AS_NO_ERROR;
    }
    else if ((retVal == AS_HTTP_RESPONSE_4XX)
             || (retVal == AS_HTTP_RESPONSE_5XX)
             || (retVal == AS_HTTP_PROTOCOL_ERROR)
             || (retVal == AS_HTTP_TOO_MANY_REDIRECTS))
    {
        returnAllUrlRequestTasks();

        disconnectByteRangeDownloaders( timeNow );

        transitState( timeNow, INIT_STATE );

        retVal = AS_NO_ERROR;
    }
    else if (retVal != AS_HTTP_PIPELINING_NOT_STABLE)
    {
        assert( false );
    }

    computeNextTimeout();
/*
    DEBUGLOG(
        McplTrackDownloader2,
        "%u processed connections, retVal %d, unschedule %llu",
        mTrackId,
        retVal,
        countUnscheduledBytes() );
*/

    return retVal;
}

/** Process timeout. */
int32_t McplTrackDownloader2::processTimeout( const AseTimeVal& timeNow )
{
    int32_t retVal = AS_NO_ERROR;

    if (timeNow < mNextTimeout)
    {
        return retVal;
    }

    switch (mState)
    {
        case UNINITIALIZED_STATE:
            break;

        case INIT_STATE:
        {
            assert( mUrlRequestTaskList.empty()
                    || (mUrlRequestTaskList.size() == 1) );

            /*DEBUGLOG(   McplTrackDownloader2,
                        "%u processing timeout in INIT state %llu, %llu",
                        mTrackId,
                        mTimerOne.ms(),
                        mTimerTwo.ms() );
            */
            if (timeNow >= mTimerOne)
            {
                UrlRequestTaskPtr pUrlRequestTask;

                if (mUrlRequestTaskList.empty())
                {
                    //Try to get a URL request task.
                    retVal = mHttpRequestManagerPtr->getUrlRequestTask(
                                                            mTrackId, true /* initial task */,
                                                            pUrlRequestTask );

                    if (retVal == AS_NO_ERROR)
                    {
                        DEBUGLOG(
                            McplTrackDownloader2,
                            "%u got a URL request task %u, URL: %s, 1, byte ranges: %s",
                            mTrackId,
                            pUrlRequestTask->getId(),
                            pUrlRequestTask->getCurrentUrl().GetPrefix().c_str(),
                            pUrlRequestTask->getByteRangesDebugString().c_str() );

                        mUrlRequestTaskList.push_back( pUrlRequestTask );

                        assert( !pUrlRequestTask->hasRequestsInProgress() );
                    }
                }

                if (!mUrlRequestTaskList.empty())
                {
                    pUrlRequestTask = mUrlRequestTaskList.front();

                    AseUrl requestUrl;

                    AseUrl destinationUrl;

                    requestUrl = pUrlRequestTask->getCurrentUrl();

                    mOriginalDestination = requestUrl.GetHostPort();

                    //Check if there was any URL redirects for this request URL.
                    destinationUrl = getUrlRedirectDestination( requestUrl );

                    mCurrentConnectedDestination = destinationUrl.GetHostPort();

                    setThroughputMonitor( pUrlRequestTask->getThroughputMonitor() );

                    UrlRecord urlRecord;

                    urlRecord.mUrl = requestUrl;
                    urlRecord.mUrlReqeustTaskCount = 1;
                    urlRecord.mStatus = AS_NO_ERROR;

                    mUrlRecordList.push_back( urlRecord );

                    pUrlRequestTask
                        ->getMediaRequestTask()
                            ->requestStarted( true /*connecting*/, false /*pipelined*/ );

                    connectByteRangeDownloaders( timeNow );

                    transitState( timeNow, CONNECTING_STATE );
                }
                else
                {
                    mTimerOne = timeNow + AseTimeVal::fromMS( mUrlReqeustGetInterval );
                }
            }

            retVal = AS_NO_ERROR;

            break;
        }

        case WAITING_FOR_URL_REQUEST_STATE:
        {
            assert(
                mUrlRecordList.empty()
                && mUrlRequestTaskList.empty() );

            if (timeNow >= mTimerTwo)
            {
                // XXX New code is included below to handle the abandon case in this state, however
                //     from the assert above it appears this cannot happen: requests cannot be in progress
                //     during WAITING_FOR_URL_REQUEST_STATE after all.
                //
                //     This would leave the other cause of a timeout here that a byteRangeDownloader is trying
                //     to automatically reconnect.

                bool bAbandonedTasks = false;

                ByteRangeReceivingTaskList byteRangeReceivingTaskList;

                ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

                byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

                while (byteRangeDownloaderItr != mByteRangeDownloaderMap.end())
                {
                    retVal = byteRangeDownloaderItr->second->processTimeout( timeNow, byteRangeReceivingTaskList );

                    if (retVal != AS_NO_ERROR)
                    {
                        DEBUGLOG(
                            McplTrackDownloader2,
                            "%u processed timeout in waiting for URL request task state, "
                            "downloader returned: %d.",
                            mTrackId,
                            retVal );

                        assert( retVal != AS_TIMEOUT);

                        if (retVal == AS_ABANDONED)
                        {
                            assert( false ); // Don't think this can happen after all

                            // Inform the first UrlRequestTask that it has been aborted
                            ByteRangeReceivingTaskList abandonedTaskList;

                            byteRangeDownloaderItr->second->retrieveByteReceivingTasks( abandonedTaskList );

                            // We do not need to return the byte range to the UrlRequestTask because the byte ranges
                            // will be regenerated when we return the UrlRequestTask to the HttpRequestManager
                            // below

                            assert( !abandonedTaskList.empty() );

                            abandonedTaskList.front().mUrlRequestTaskPtr
                                ->getMediaRequestTask()
                                    ->getMediaRequest()
                                        ->abandoned();

                            bAbandonedTasks = true;
                        }
                        else if ( !bAbandonedTasks )
                        {
                            processConnectionFailure( timeNow, byteRangeDownloaderItr->second );
                        }
                    }

                    byteRangeDownloaderItr++;
                }


                if (bAbandonedTasks)
                {
                    // A byte range downloader aborted a block due to passing the abort time
                    // without receiving data

                    // XXX If this code can execute it needs the same treatment as the code below

                    returnAllUrlRequestTasks();

                    disconnectByteRangeDownloaders( timeNow );

                    transitState( timeNow, INIT_STATE );

                    retVal = AS_NO_ERROR;

                    break;
                }
                else
                {
                    mTimerTwo = getEarliestByteRangeDownloaderTimeout();
                }
            }

            if (timeNow >= mTimerOne)
            {
                UrlRequestTaskPtr pUrlRequestTask;

                retVal = mHttpRequestManagerPtr->getUrlRequestTask(
                                                        mTrackId,
                                                        pUrlRequestTask );

                if (retVal == AS_NO_ERROR)
                {
                    DEBUGLOG(
                        McplTrackDownloader2,
                        "%u got a URL request task %u, URL: %s, 2, byte ranges: %s",
                        mTrackId,
                        pUrlRequestTask->getId(),
                        pUrlRequestTask->getCurrentUrl().str().c_str(),
                        pUrlRequestTask->getByteRangesDebugString().c_str()  );

                    //Add the URL request task to the pending list.
                    mUrlRequestTaskList.push_back( pUrlRequestTask );

                    AseUrl requestUrl;
                    AseUrl destinationUrl;

                    requestUrl = pUrlRequestTask->getCurrentUrl();

                    destinationUrl = getUrlRedirectDestination( requestUrl );

                    if (checkUrlDestination(
                                destinationUrl,
                                mCurrentConnectedDestination.first,
                                mCurrentConnectedDestination.second ))
                    {
                        // Reconnect any disconnected byte range downloaders
                        bool const connecting = connectByteRangeDownloaders( timeNow );

                        pUrlRequestTask
                            ->getMediaRequestTask()
                                ->requestStarted( connecting, false /*pipelined*/ );

                        UrlRecord urlRecord;

                        urlRecord.mUrl = requestUrl;
                        urlRecord.mUrlReqeustTaskCount = 1;
                        urlRecord.mStatus = AS_NO_ERROR;

                        mUrlRecordList.push_back( urlRecord );

                        transitState( timeNow, WORKING_STATE );

                        retVal = scheduleByteRanges( timeNow );

                        mTimerTwo = getEarliestByteRangeDownloaderTimeout();
                    }
                    else
                    {
                        disconnectByteRangeDownloaders( timeNow );

                        transitState( timeNow, INIT_STATE );
                    }
                }
                else
                {
                    mTimerOne = timeNow + AseTimeVal::fromMS( mUrlReqeustGetInterval );

                    if ( !mDisconnectedWhileIdle
                            && mIdleDisconnectTime != AseTimeVal::ZERO
                            && ( timeNow - mCurentStateTime ) > mIdleDisconnectTime )
                    {
                        disconnectByteRangeDownloaders( timeNow );

                        mDisconnectedWhileIdle = true;
                    }

                    retVal = AS_NO_ERROR;
                }
            }

            break;
        }

        case CONNECTING_STATE:
        case WORKING_STATE:
        {
            if (timeNow >= mTimerTwo)
            {
                bool bAbandonedTasks = false;

                ByteRangeReceivingTaskList byteRangeReceivingTaskList;

                ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

                byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

                while (byteRangeDownloaderItr != mByteRangeDownloaderMap.end())
                {
                    retVal = byteRangeDownloaderItr->second->processTimeout( timeNow, byteRangeReceivingTaskList );

                    if (retVal != AS_NO_ERROR)
                    {
                        DEBUGLOG(
                            McplTrackDownloader2,
                            "%u processed timeout in working state, downloader returned: %d.",
                            mTrackId,
                            retVal );

                        // ByteRangeDownloader returns an error from processTimeout in two cases
                        // - it's waiting to reconnect and the reconnect failed
                        // - the abandon time for the MediaRequestTask associated with a request
                        //   has passed
                        //
                        // For the first case, we will just explicitly ask the byte range downloader to
                        // reconnect, which just puts it back into the waiting to reconnect state
                        // (it will just repeatedly try to reconnect until that works or something
                        // else happens)
                        //
                        // For the second case, we will reset everything and handle this below
                        if (retVal == AS_ABANDONED)
                        {
                            // Inform the first UrlRequestTask that it has been aborted
                            ByteRangeReceivingTaskList abandonedTaskList;

                            byteRangeDownloaderItr->second->retrieveByteReceivingTasks( abandonedTaskList );

                            // We do not need to return the byte range to the UrlRequestTask because the byte ranges
                            // will be regenerated when we return the UrlRequestTask to the HttpRequestManager
                            // below

                            assert( !abandonedTaskList.empty() );

                            abandonedTaskList.front().mUrlRequestTaskPtr
                                ->getMediaRequestTask()
                                    ->getMediaRequest()
                                        ->abandoned();

                            byteRangeDownloaderItr->second->disconnect( timeNow, AS_ABANDONED );

                            bAbandonedTasks = true;

                            byteRangeDownloaderItr->second->reconnect( timeNow );
                        }
                        else if ( !bAbandonedTasks )
                        {
                            processConnectionFailure( timeNow, byteRangeDownloaderItr->second );
                        }
                    }

                    byteRangeDownloaderItr++;
                }

                // Return the duplicated tasks to their UrlRequestManagers
                ByteRangeReceivingTaskList::iterator taskIt = byteRangeReceivingTaskList.begin();
                for( ; taskIt != byteRangeReceivingTaskList.end();++taskIt )
                {
                    if ( !taskIt->mUrlRequestTaskPtr->completed() )
                    {
                        taskIt->mUrlRequestTaskPtr->returnByteRange( taskIt->mByteRange, /*duplicated*/ true );
                    }
                }

                if (bAbandonedTasks)
                {
                    DEBUGLOG(
                            McplTrackDownloader2,
                            "%u have abandoned tasks and %u remaining active connections",
                            mTrackId,
                            countActiveConnections() );

                    // A byte range downloader aborted a block due to passing the abort time
                    // without receiving data

                    // Close any other ByteRangeDownloaders working on abandoned tasks
                    // Note, usually they will all have thrown a timeout above, since they all have the same
                    // timeout for the block to be abandoned. However, it seems possible that some of the brds
                    // don't get the timeout all at the same time, so we will close them here.
                    disconnectAbandonedByteRangeDownloaders( timeNow );

                    returnAbandonedUrlRequestTasks();

                    if (mUrlRequestTaskList.empty())
                    {
                        assert( mUrlRecordList.empty()
                                    && countUnscheduledBytes() == 0 );

                        transitState( timeNow,  WAITING_FOR_URL_REQUEST_STATE );
                    }

                    retVal = AS_NO_ERROR;

                    break;
                }
                else
                {
                    mTimerTwo = getEarliestByteRangeDownloaderTimeout();
                }
            }

            if ( timeNow >= mTimerOne )
            {
                // This should only happen in CONNECTING state
                assert( mState == CONNECTING_STATE );

                transitState( timeNow, WORKING_STATE );
            }

            retVal = AS_NO_ERROR;

            break;
        }

        case WAITING_FOR_MEDIA_BUFFER_STATE:
        {
            if (timeNow >= mTimerTwo)
            {
                ByteRangeReceivingTaskList byteRangeReceivingTaskList;

                ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

                byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

                while (byteRangeDownloaderItr != mByteRangeDownloaderMap.end())
                {
                    retVal = byteRangeDownloaderItr->second->processTimeout( timeNow, byteRangeReceivingTaskList );

                    if (retVal != AS_NO_ERROR)
                    {
                        DEBUGLOG(
                            McplTrackDownloader2,
                            "%u processed timeout in waiting for media buffer state, "
                            "downloader returned: %d.",
                            mTrackId,
                            retVal );

                        assert( retVal != AS_TIMEOUT);

                        byteRangeDownloaderItr->second->reconnect( timeNow );
                    }

                    byteRangeDownloaderItr++;
                }

                // Return the duplicated tasks to their UrlRequestManagers
                ByteRangeReceivingTaskList::iterator taskIt = byteRangeReceivingTaskList.begin();
                for( ; taskIt != byteRangeReceivingTaskList.end();++taskIt )
                {
                    if ( !taskIt->mUrlRequestTaskPtr->completed() )
                    {
                        taskIt->mUrlRequestTaskPtr->returnByteRange( taskIt->mByteRange );
                    }
                }

                mTimerTwo = getEarliestByteRangeDownloaderTimeout();
            }

            if (timeNow >= mTimerOne)
            {
                //In WAITING_FOR_MEDIA_BUFFER_STATE, it is time to check media
                //buffer availability.

                if (mHttpRequestManagerPtr->checkMediaBufferAvailability( mTrackId ))
                {
                    // Reconnect any disconnected byte range downloaders
                    (void) connectByteRangeDownloaders( timeNow );

                    ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

                    byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

                    while (byteRangeDownloaderItr != mByteRangeDownloaderMap.end())
                    {
                        byteRangeDownloaderItr->second->notifyMediaBufferAvailability(
                                                                            timeNow,
                                                                            true );

                        byteRangeDownloaderItr++;
                    }

                    mMediaBufferAvailableFlag = true;

                    transitState( timeNow, WORKING_STATE );
                }
                else
                {
                    mTimerOne = timeNow + AseTimeVal::fromMS( mMediaBufferCheckInterval );

                    if ( !mDisconnectedWhileIdle
                            && mIdleDisconnectTime != AseTimeVal::ZERO
                            && ( timeNow - mCurentStateTime ) > mIdleDisconnectTime )
                    {
                        disconnectByteRangeDownloaders( timeNow );

                        mDisconnectedWhileIdle = true;
                    }
                }
            }

            retVal = AS_NO_ERROR;

            break;
        }
    }

    computeNextTimeout();
/*
    DEBUGLOG(
        McplTrackDownloader2,
        "%u processed timeout, retVal %d, unschedule %llu",
        mTrackId,
        retVal,
        countUnscheduledBytes() );
*/
    return retVal;
}

/** Pause downloading. */
int32_t McplTrackDownloader2::pauseDownloading( const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    BaseTrackDownloader2::pauseDownloading( timeNow );

    ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

    byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

    while (byteRangeDownloaderItr != mByteRangeDownloaderMap.end())
    {
        byteRangeDownloaderItr->second->pauseDownloading( timeNow );

        byteRangeDownloaderItr++;
    }

    mTimerTwo = getEarliestByteRangeDownloaderTimeout();

    computeNextTimeout();

    retVal = AS_NO_ERROR;

    return retVal;
}

/** Resume downloading. */
int32_t McplTrackDownloader2::resumeDownloading( const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    BaseTrackDownloader2::resumeDownloading( timeNow );

    ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

    byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

    while (byteRangeDownloaderItr != mByteRangeDownloaderMap.end())
    {
        byteRangeDownloaderItr->second->resumeDownloading( timeNow );

        byteRangeDownloaderItr++;
    }

    mTimerTwo = getEarliestByteRangeDownloaderTimeout();

    computeNextTimeout();

    retVal = AS_NO_ERROR;

    return retVal;
}

/** Check for URL changes on the frontmost task */
bool McplTrackDownloader2::checkForUrlChanges()
{
    if ( mUrlRequestTaskList.empty() )
        return false;

    return mUrlRequestTaskList.front()->selectUrl();
}

/** Notify the downloader that a new task is waiting */
void McplTrackDownloader2::notifyNewTaskIsWaiting()
{
    if ( mState == WAITING_FOR_URL_REQUEST_STATE || mState == INIT_STATE )
    {
        // Reset our timeout so that we will be called to process the new request right away
        mTimerOne = AseTimeVal::now();

        computeNextTimeout();
    }
}

void McplTrackDownloader2::reportHttpResponseTime(
                                    uint32_t byteRangeDownloaderId,
                                    const AseTimeVal& httpResponseTime )
{
    BaseTrackDownloader2::reportHttpResponseTime(
                                        byteRangeDownloaderId,
                                        httpResponseTime );

    mBlockedByteRangeDownloaderId = 0xFFFFFFFF;

    mLatestHttpResponseTime = httpResponseTime;
    mHttpResponseTimeUpdatedTime = AseTimeVal::now();

    mUrlRequestTaskList.front()
        ->getMediaRequestTask()
            ->getMediaRequest()
                ->reportHttpResponseTime( httpResponseTime );
}

/** change state */
void McplTrackDownloader2::transitState( const AseTimeVal& timeNow, State newState )
{
    if (mState != newState)
    {
        if (newState == INIT_STATE)
        {
            mGettingUrlRequestTasksBlockedFlag = false;

            mMediaBufferAvailableFlag = true;
        }

        DEBUGLOG(
            McplTrackDownloader2,
            "%u transited from %s to %s.",
            mTrackId,
            getStateName( mState ),
            getStateName( newState ) );

        mState = newState;

        if (mState == INIT_STATE)
        {
            mTimerOne = timeNow;
            mTimerTwo = AseTimeVal::INFINITE;
        }
        else if (mState == WAITING_FOR_URL_REQUEST_STATE)
        {
            mTimerOne = timeNow;
            mTimerTwo = getEarliestByteRangeDownloaderTimeout();
            mDisconnectedWhileIdle = false;
        }
        else if (mState == CONNECTING_STATE)
        {
            mConnectingTime = timeNow;
            mTimerOne = AseTimeVal::INFINITE;
            mTimerTwo = getEarliestByteRangeDownloaderTimeout();
        }
        else if (mState == WORKING_STATE)
        {
            mTimerOne = AseTimeVal::INFINITE;
            mTimerTwo = getEarliestByteRangeDownloaderTimeout();
        }
        else if (mState == WAITING_FOR_MEDIA_BUFFER_STATE)
        {
            mTimerOne = timeNow + AseTimeVal::fromMS( mMediaBufferCheckInterval );
            mTimerTwo = getEarliestByteRangeDownloaderTimeout();
            mDisconnectedWhileIdle = false;
        }

        mCurentStateTime = timeNow;
    }

    return;
}

/** Connect byte range downloaders. */
bool McplTrackDownloader2::connectByteRangeDownloaders( const AseTimeVal& timeNow )
{
    int32_t retVal;

    bool connecting = true;

    ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

    byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

    for ( ; byteRangeDownloaderItr != mByteRangeDownloaderMap.end(); ++byteRangeDownloaderItr )
    {
        if ( byteRangeDownloaderItr->second->getState() != ByteRangeDownloader::DISCONNECTED_STATE
                && byteRangeDownloaderItr->second->getState() != ByteRangeDownloader::CONNECTION_DOWN_STATE )
        {

                DEBUGLOG(
            McplTrackDownloader2,
            "%u byte range downloader %u already connected to %s:%d, state %u",
            mTrackId,
            byteRangeDownloaderItr->first,
            mCurrentConnectedDestination.first.c_str(),
            mCurrentConnectedDestination.second,
            byteRangeDownloaderItr->second->getState() );

            connecting = false;

            continue;
        }


        byteRangeDownloaderItr->second->setInitialThroughput( 0, mInitialThroughput );
        byteRangeDownloaderItr->second->setTimeouts(
                mHttpConnectTimeout,
                mHttpStallTimeout,
                mTrackConfiguration.mConnections > 1 ? mMinimumDuplicateTimeout : AseTimeVal::ZERO,
                mTrackConfiguration.mConnections > 1 ? mDuplicateTimeoutMultiplier : 0 );

        DEBUGLOG(
            McplTrackDownloader2,
            "%u connecting byte range downloader %u to %s:%d, initial throughput: %llu",
            mTrackId,
            byteRangeDownloaderItr->first,
            mCurrentConnectedDestination.first.c_str(),
            mCurrentConnectedDestination.second,
            mInitialThroughput );

        retVal = byteRangeDownloaderItr->second->connect(
                                                    timeNow,
                                                    mCurrentConnectedDestination );



        if (retVal != AS_NO_ERROR)
        {
            DEBUGLOG(
                McplTrackDownloader2,
                "%u error %d connecting byte range downloader %u to %s:%d, initial throughput: %llu",
                mTrackId,
                retVal,
                byteRangeDownloaderItr->first,
                mCurrentConnectedDestination.first.c_str(),
                mCurrentConnectedDestination.second,
                mInitialThroughput );

            byteRangeDownloaderItr->second->reconnect( timeNow );

            mHttpRequestManagerPtr
                ->reportNetworkFailure( mTrackId,
                                        retVal,
                                        mOriginalDestination );
        }
    }

    return connecting;
}

/** Disconnect byte range downloaders. */
void McplTrackDownloader2::disconnectByteRangeDownloaders( const AseTimeVal& timeNow )
{
    ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

    ullong throughputCount = 0;

    mInitialThroughput = 0;

    byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

    for ( ; byteRangeDownloaderItr != mByteRangeDownloaderMap.end(); ++byteRangeDownloaderItr )
    {
        ByteRangeDownloader::State const brdstate = byteRangeDownloaderItr->second->getState();

        if ( brdstate == ByteRangeDownloader::DISCONNECTED_STATE )
        {
            DEBUGLOG(
            McplTrackDownloader2,
            "%u byte range downloader %u already disconnected from %s:%d",
            mTrackId,
            byteRangeDownloaderItr->first,
            mCurrentConnectedDestination.first.c_str(),
            mCurrentConnectedDestination.second );

            continue;
        }

        ullong currentThroughput;

        currentThroughput = byteRangeDownloaderItr->second->getAverageThroughput(
                                                                            0,
                                                                            timeNow );

        mInitialThroughput += currentThroughput;

        throughputCount++;

        if ( brdstate == ByteRangeDownloader::DOWNLOADING_STATE
                || brdstate == ByteRangeDownloader::SUSPENDED_STATE )
        {
            returnByteRangeReceivingTasks( byteRangeDownloaderItr->second );
        }

        byteRangeDownloaderItr->second->disconnect( timeNow );

        DEBUGLOG(
            McplTrackDownloader2,
            "%u disconnected byte range downloader %u from %s:%d, average throughput: %llu.",
            mTrackId,
            byteRangeDownloaderItr->first,
            mCurrentConnectedDestination.first.c_str(),
            mCurrentConnectedDestination.second,
            currentThroughput );
    }

    if (throughputCount > 0)
    {
        mInitialThroughput /= throughputCount;
    }
    else
    {
        mInitialThroughput = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "initialThroughput",
                                                            3000000 );
    }
}

void McplTrackDownloader2::disconnectAbandonedByteRangeDownloaders( const AseTimeVal& timeNow )
{
    ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

    byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

    for ( ; byteRangeDownloaderItr != mByteRangeDownloaderMap.end(); ++byteRangeDownloaderItr )
    {
        ByteRangeDownloader::State const brdstate = byteRangeDownloaderItr->second->getState();

        if ( brdstate == ByteRangeDownloader::DOWNLOADING_STATE
                || brdstate == ByteRangeDownloader::SUSPENDED_STATE )
        {
            if ( byteRangeDownloaderItr->second->isFrontRequestAbandoned() )
            {
                byteRangeDownloaderItr->second->disconnect( timeNow, AS_ABANDONED );

                DEBUGLOG(
                    McplTrackDownloader2,
                    "%u disconnected byte range downloader %u from %s:%d, front task is abandoned",
                    mTrackId,
                    byteRangeDownloaderItr->first,
                    mCurrentConnectedDestination.first.c_str(),
                    mCurrentConnectedDestination.second );
            }
        }
    }
}


uint32_t McplTrackDownloader2::countActiveConnections() const
{
    uint32_t count = 0;

    ByteRangeDownloaderMap::const_iterator byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

    for ( ; byteRangeDownloaderItr != mByteRangeDownloaderMap.end(); ++byteRangeDownloaderItr )
    {
        if ( byteRangeDownloaderItr->second->active() )
            ++count;
    }

    return count;
}

/** Return all existing URL request tasks back to URL requuest manager. */
void McplTrackDownloader2::returnAllUrlRequestTasks()
{
    //Return all existing URL requests in the reverse order they were requested..

    UrlRequestTaskList::reverse_iterator urlRequestTaskItr;

    urlRequestTaskItr = mUrlRequestTaskList.rbegin();

    while (urlRequestTaskItr != mUrlRequestTaskList.rend())
    {
        mHttpRequestManagerPtr->returnUrlRequestTask( *urlRequestTaskItr );

        urlRequestTaskItr++;
    }

    mUrlRequestTaskList.clear();

    mUrlRecordList.clear();
}

/** Return abandoned URL request tasks back to URL requuest manager. */
void McplTrackDownloader2::returnAbandonedUrlRequestTasks()
{
    UrlRequestTaskList remainingTasks;

    UrlRequestTaskList::reverse_iterator urlRequestTaskItr;

    urlRequestTaskItr = mUrlRequestTaskList.rbegin();

    while (urlRequestTaskItr != mUrlRequestTaskList.rend())
    {
        if ( (*urlRequestTaskItr)->isAbandoned() )
        {
            uint32_t httpResponseCode = 0;
            (void) updateUrlRecordListOnTaskComplete( AS_NO_ERROR, httpResponseCode, *urlRequestTaskItr );

            DEBUGLOG( McplTrackDownloader2, "%u returning abandoned UrlRequestTask %d", mTrackId, (*urlRequestTaskItr)->getId() );
            mHttpRequestManagerPtr->returnUrlRequestTask( *urlRequestTaskItr );
        }
        else
        {
            DEBUGLOG( McplTrackDownloader2, "%u keeping non-abandoned UrlRequestTask %d", mTrackId, (*urlRequestTaskItr)->getId() );
            remainingTasks.push_front( *urlRequestTaskItr );
        }

        urlRequestTaskItr++;
    }

    mUrlRequestTaskList = remainingTasks;
}

void McplTrackDownloader2::reportErrorToUrlRequestTasks(    AseUrl const&   url,
                                                            AseErrorCode    status,
                                                            uint32_t        httpCode )
{
    UrlRequestTaskList::iterator it = mUrlRequestTaskList.begin();
    for( ; it != mUrlRequestTaskList.end(); ++it )
    {
        if ( (*it)->getCurrentUrl() == url )
        {
            (*it)->getMediaRequestTask()
                ->getMediaRequest()
                    ->requestError( status, httpCode );
        }
    }
}

void McplTrackDownloader2::debugPrintUrlRecordList() const
{
    UrlRecordList::const_iterator urlRecordItr = mUrlRecordList.begin();

    DEBUGLOG(
        McplTrackDownloader2,
        "URL records: %zu",
        mUrlRecordList.size() );

    while (urlRecordItr != mUrlRecordList.end())
    {
        DEBUGLOG(
            McplTrackDownloader2,
            "... record: %s, %d tasks, status: %d",
            urlRecordItr->mUrl.GetPrefix().c_str(),
            urlRecordItr->mUrlReqeustTaskCount,
            urlRecordItr->mStatus );

        urlRecordItr++;
    }
}

McplTrackDownloader2::UrlRecordList::iterator McplTrackDownloader2::findUrlRecord( AseUrl const& url )
{
    UrlRecordList::iterator urlRecordItr = mUrlRecordList.begin();

    for ( ; urlRecordItr != mUrlRecordList.end(); ++urlRecordItr)
    {
        if (urlRecordItr->mUrl == url)
            break;
    }

    return urlRecordItr;
}

/** Get byte ranges for scheduling. */
int32_t McplTrackDownloader2::getByteRanges( ullong& unscheduledBytes )
{
    int32_t retVal = AS_AGAIN;

    assert( !mUrlRecordList.empty() );

    if (mGettingUrlRequestTasksBlockedFlag)
        return retVal;

    UrlRequestTaskPtr pUrlRequestTask;

    retVal = mHttpRequestManagerPtr->getUrlRequestTask(
                                            mTrackId,
                                            pUrlRequestTask );

    if (retVal == AS_NO_ERROR)
    {
        AseUrl const& url = pUrlRequestTask->getCurrentUrl();

        DEBUGLOG(
            McplTrackDownloader2,
            "%u got a URL request task %u, URL: %s, 3, byte ranges: %s",
            mTrackId,
            pUrlRequestTask->getId(),
            url.GetPrefix().c_str(),
            pUrlRequestTask->getByteRangesDebugString().c_str()  );

        debugPrintUrlRecordList();

        UrlRecordList::iterator const urlRecordItr = findUrlRecord( url );

        if (urlRecordItr != mUrlRecordList.end())
        {
            //Add the URL request task to the pending list.
            mUrlRequestTaskList.push_back( pUrlRequestTask );

            assert( !pUrlRequestTask->hasRequestsInProgress() );

            unscheduledBytes += pUrlRequestTask->getUnscheduledDataLength();

            urlRecordItr->mUrlReqeustTaskCount++;
        }
        else
        {
            //This URL request task has a different URL, check if we'd like to request from the
            //current server. We only request from the current server if one of the following is
            //ture:
            //(1) This URL has not been redirected, this URL contains the same host name and port
            //    number as the current server host name and port number.
            //(2) This URL has been redirected, the redirect URL contains the same host name and
            //    port number as the current server host name and port number.

            //The destinationUrl is the same as the original URL if there is no redirect.
            AseUrl const destinationUrl = getUrlRedirectDestination( url );

            if (checkUrlDestination(
                        destinationUrl,
                        mCurrentConnectedDestination.first,
                        mCurrentConnectedDestination.second ))
            {
                //Add the URL request task to the pending list.
                mUrlRequestTaskList.push_back( pUrlRequestTask );

                assert( !pUrlRequestTask->hasRequestsInProgress() );

                unscheduledBytes += pUrlRequestTask->getUnscheduledDataLength();

                UrlRecord urlRecord;

                urlRecord.mUrl = url;
                urlRecord.mUrlReqeustTaskCount = 1;
                urlRecord.mStatus = AS_NO_ERROR;

                mUrlRecordList.push_back( urlRecord );
            }
            else
            {
                //It is unlikely that this URL request task will be to the current
                //server, return the URL request tasks.
                mHttpRequestManagerPtr->returnUrlRequestTask( pUrlRequestTask );

                mGettingUrlRequestTasksBlockedFlag = true;
            }
        }
    }

    return retVal;
}

/** Schedule requests for byte ranges. */
int32_t McplTrackDownloader2::scheduleByteRanges( const AseTimeVal& timeNow )
{
    PERFORMANCE_MARKER_SCOPED("mspl.schedulebyteranges");

    int32_t retVal = AS_NO_ERROR;

    SchedulingBucketList workingBucketList;
    SchedulingBucketList workingBucketList2;
    SchedulingBucketList::iterator workingBucketItr;

    SchedulingBucketList schedulingBucketList;
    SchedulingBucketList::iterator schedulingBucketItr;

    ullong unscheduledBytes = 0;

    UrlRequestTaskList::const_iterator it = mUrlRequestTaskList.begin();
    for( ; it != mUrlRequestTaskList.end(); ++it )
    {
        unscheduledBytes += (*it)->updateUnscheduledDataLength();
    }

    uint32_t availableThroughputCount = 0;
    ullong averageThroughput = 0;

    ullong currentTotalOutstandingByteCount = 0;

    ullong totalByteCountToTarget = 0;
    ullong totalTargetByteCount = 0;

    ullong targetByteCount;

    bool channelIdle = false;

    UrlRequestTaskList::iterator urlRequestTaskItr;

    ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

    if ((mBlockedByteRangeDownloaderId != 0xFFFFFFFF)
        && (mByteRangeDownloaderMap[mBlockedByteRangeDownloaderId]->getState()
            == ByteRangeDownloader::IDLE_STATE))
    {
        mBlockedByteRangeDownloaderId = 0xFFFFFFFF;
    }

    //Create the working bucket list from the working channels and work out:
    // (*) The current average throughput
    // (*) The total outstanding byte count.
    // (*) If any working channel is in idle state.
    byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

    while (byteRangeDownloaderItr != mByteRangeDownloaderMap.end())
    {
        // In CONNECTING STATE we schedule across all the connections, in the hope they will connect soon
        // and get some bytes to request
        ByteRangeDownloader::State const brdstate = byteRangeDownloaderItr->second->getState();

        if ((   (brdstate == ByteRangeDownloader::IDLE_STATE)
            ||  (mTrackConfiguration.mPipeline && (brdstate == ByteRangeDownloader::DOWNLOADING_STATE))
            ||  (mState == CONNECTING_STATE && brdstate == ByteRangeDownloader::CONNECTING_STATE ))
           &&  (byteRangeDownloaderItr->first != mBlockedByteRangeDownloaderId))
        {
            SchedulingBucket workingBucket;

            workingBucket.mByteRangeDownloaderPtr = byteRangeDownloaderItr->second;
            workingBucket.mAverageThroughput = byteRangeDownloaderItr->second->
                                                            getAverageThroughput(
                                                                            0,
                                                                            timeNow );

            if (workingBucket.mAverageThroughput != 0)
            {
                averageThroughput += workingBucket.mAverageThroughput;
                availableThroughputCount++;
            }

            workingBucket.mByteCountToTarget = 0;

            workingBucketList.push_back( workingBucket );

            currentTotalOutstandingByteCount += byteRangeDownloaderItr->second->
                                                                getTotalOutstandingByteCount();

            if (byteRangeDownloaderItr->second->getState() == ByteRangeDownloader::IDLE_STATE)
            {
                channelIdle = true;
            }
        }

        byteRangeDownloaderItr++;
    }

    workingBucketList2 = workingBucketList;

    //Check if we need to force to get a fresh response time.
    if ((timeNow - mHttpResponseTimeUpdatedTime) > AseTimeVal::fromMS( mResponseTimeFreshnessDuration) )
    {
        //Only force to block if all channels are not working and not idle.
        if ((workingBucketList.size() == mTargetConnectionCount) && (!channelIdle))
        {
            mBlockedByteRangeDownloaderId = workingBucketList.front().mByteRangeDownloaderPtr->getId();

            workingBucketList.pop_front();
        }

        mHttpResponseTimeUpdatedTime = timeNow + AseTimeVal::fromMS( mResponseTimeFreshnessDuration );
    }

    if (!workingBucketList.empty())
    {
        if (availableThroughputCount == 0)
        {
            //NOTE: THIS NEVER HAPPENS as all ByteRangeDownloaders start with default, non-zero, throughput

            //This is the case where not a single throughtput measurement is available.

            retVal = AS_NO_ERROR;

            //Try to get some byte ranges for initial scheduling.
            while ((retVal == AS_NO_ERROR) && (unscheduledBytes == 0))
            {
                retVal = getByteRanges( unscheduledBytes );
            }

            DEBUGLOG(
                McplTrackDownloader2,
                "%u gathered %llu bytes for initial scheduling.",
                mTrackId,
                unscheduledBytes );

            targetByteCount = (currentTotalOutstandingByteCount
                               + unscheduledBytes
                               + mTargetConnectionCount ) / mTargetConnectionCount;

            //Create the scheduling bucket list from the working bucket list.
            workingBucketItr = workingBucketList.begin();

            while (workingBucketItr != workingBucketList.end())
            {
                if (targetByteCount > workingBucketItr->mByteRangeDownloaderPtr->getTotalOutstandingByteCount())
                {
                    workingBucketItr->mByteCountToTarget = targetByteCount
                              - workingBucketItr->mByteRangeDownloaderPtr->getTotalOutstandingByteCount();

                    totalTargetByteCount += targetByteCount;
                    totalByteCountToTarget += workingBucketItr->mByteCountToTarget;

                    addBucketToSchedulingList( schedulingBucketList, *workingBucketItr );
                }

                workingBucketList.pop_front();

                workingBucketItr = workingBucketList.begin();
            }
        }
        else
        {
            //There is at least one throughput measurement is available.

            averageThroughput /= availableThroughputCount;


            //Create the scheduling bucket list from the working bucket list.
            workingBucketItr = workingBucketList.begin();

            while (workingBucketItr != workingBucketList.end())
            {
                if (workingBucketItr->mAverageThroughput == 0)
                {
                    workingBucketItr->mAverageThroughput = averageThroughput;
                }

                ullong averageResponseTime = mLatestHttpResponseTime.ms();

                if (averageResponseTime < (ullong) mMinResponseTime)
                {
                    averageResponseTime = (ullong) mMinResponseTime;
                }

                targetByteCount = std::max( workingBucketItr->mAverageThroughput
                                            * averageResponseTime
                                            / 8000,
                                            (ullong) 1 );

                if (targetByteCount * mSchedulingTriggerMultiplyFactor
                    > workingBucketItr->mByteRangeDownloaderPtr->getTotalOutstandingByteCount())
                {
                    ullong target = std::max(
                                        targetByteCount * mSchedulingTargetMultiplyFactor,
                                        workingBucketItr->mAverageThroughput * mTargetScheduleTime / 8000 );

                    // NOTE: time = max( averageResponseTime, mTargetScheduleTime )
                    //       target = workingBucketItr->mAverageThroughput * time / 8000
                    //
                    // i.e. target is only affected by response time if it grows bigger than mTargetScheduleTime (default 2000ms)

                    workingBucketItr->mByteCountToTarget = target
                                                           - workingBucketItr->
                                                                mByteRangeDownloaderPtr->
                                                                    getTotalOutstandingByteCount();

                    totalTargetByteCount += target;
                    totalByteCountToTarget += workingBucketItr->mByteCountToTarget;

                    addBucketToSchedulingList( schedulingBucketList, *workingBucketItr );
                }

                workingBucketList.pop_front();

                workingBucketItr = workingBucketList.begin();
            }
        }
    }

    bool printSchedulingBuckets = true;
    bool adjustTarget = true;

    while (!schedulingBucketList.empty())
    {
        //Try to get enough byte ranges for scheduling.
        while ((retVal == AS_NO_ERROR) && (unscheduledBytes < totalByteCountToTarget))
        {
            retVal = getByteRanges( unscheduledBytes );
        }

        if (unscheduledBytes == 0)
        {
            break;
        }

        if (printSchedulingBuckets)
        {
            DEBUGLOG( McplTrackDownloader2, "%u %llu unscheduled bytes",
                        mTrackId,
                        unscheduledBytes );

            schedulingBucketItr = schedulingBucketList.begin();

            while (schedulingBucketItr != schedulingBucketList.end())
            {
                DEBUGLOG(
                    McplTrackDownloader2,
                    "%u scheduling bucket, brd: %u, outstanding bytes: %llu, bytes to target: %llu, throughput: %llu, throughput2: %llu, blocked: %s.",
                    mTrackId,
                    schedulingBucketItr->mByteRangeDownloaderPtr->getId(),
                    schedulingBucketItr->mByteRangeDownloaderPtr->getTotalOutstandingByteCount(),
                    schedulingBucketItr->mByteCountToTarget,
                    schedulingBucketItr->mAverageThroughput,
                    schedulingBucketItr->mByteRangeDownloaderPtr->getAverageThroughput( 2, timeNow ),
                    (schedulingBucketItr->mByteRangeDownloaderPtr->getId() == mBlockedByteRangeDownloaderId) ? "Yes" : "No" );

                schedulingBucketItr++;
            }

            printSchedulingBuckets = false;
        }

        if ((unscheduledBytes < totalByteCountToTarget) && adjustTarget)
        {
            //The byte ranges are not enough to meet the overall target, scale the
            //individual target down.

            SchedulingBucketList::reverse_iterator schedulingBucketReverseItr;
            uint32_t bucketCount;
            ullong remainPendingByteCount;

            schedulingBucketReverseItr = schedulingBucketList.rbegin();
            bucketCount = schedulingBucketList.size();
            remainPendingByteCount = unscheduledBytes;

            ullong newTotalTargetByteCount = totalTargetByteCount -
                                                    ( totalByteCountToTarget -
                                                        unscheduledBytes );

            while (bucketCount > 1)
            {
                ullong target = schedulingBucketReverseItr->mByteCountToTarget +
                                    schedulingBucketReverseItr->
                                        mByteRangeDownloaderPtr->
                                            getTotalOutstandingByteCount();

                target = target * newTotalTargetByteCount / totalTargetByteCount;

                if (target > schedulingBucketReverseItr->
                                        mByteRangeDownloaderPtr->
                                                getTotalOutstandingByteCount())
                {
                    schedulingBucketReverseItr->mByteCountToTarget =
                                                target - schedulingBucketReverseItr->
                                                                mByteRangeDownloaderPtr->
                                                                    getTotalOutstandingByteCount();
                }
                else
                {
                    schedulingBucketReverseItr->mByteCountToTarget = 0;
                }

                remainPendingByteCount -= schedulingBucketReverseItr->mByteCountToTarget;

                schedulingBucketReverseItr++;
                bucketCount--;
            }

            schedulingBucketReverseItr->mByteCountToTarget = remainPendingByteCount;

            adjustTarget = false;

            printSchedulingBuckets = true;
        }

        if (printSchedulingBuckets)
        {
            schedulingBucketItr = schedulingBucketList.begin();

            while (schedulingBucketItr != schedulingBucketList.end())
            {
                DEBUGLOG(
                    McplTrackDownloader2,
                    "%u scheduling bucket, brd: %u, outstanding bytes: %llu, bytes to target: %llu, throughput: %llu, throughput2: %llu, blocked: %s.",
                    mTrackId,
                    schedulingBucketItr->mByteRangeDownloaderPtr->getId(),
                    schedulingBucketItr->mByteRangeDownloaderPtr->getTotalOutstandingByteCount(),
                    schedulingBucketItr->mByteCountToTarget,
                    schedulingBucketItr->mAverageThroughput,
                    schedulingBucketItr->mByteRangeDownloaderPtr->getAverageThroughput( 2, timeNow ),
                    (schedulingBucketItr->mByteRangeDownloaderPtr->getId() == mBlockedByteRangeDownloaderId) ? "Yes" : "No" );

                schedulingBucketItr++;
            }

            printSchedulingBuckets = false;
        }

        urlRequestTaskItr = mUrlRequestTaskList.begin();

        schedulingBucketItr = schedulingBucketList.begin();

        while (schedulingBucketItr != schedulingBucketList.end())
        {
            // Skip scheduling to connections not yet connected - this can only happen in CONNECTING_STATE
            if ( schedulingBucketItr->mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::CONNECTING_STATE )
            {
                assert( mState == CONNECTING_STATE );
                schedulingBucketItr = schedulingBucketList.erase( schedulingBucketItr );
                if (schedulingBucketItr == schedulingBucketList.end())
                {
                    schedulingBucketItr = schedulingBucketList.begin();
                }

                continue;
            }

            //Find the first URL request task that has unscheduled byte ranges.
            while ((urlRequestTaskItr != mUrlRequestTaskList.end())
                    && (*urlRequestTaskItr)->scheduled())
            {
                urlRequestTaskItr++;
            }

            //If there is no available byte range for scheduling, break here, it goes
            //to the beginning of the outer loop and try to get more URL request tasks.
            if (urlRequestTaskItr == mUrlRequestTaskList.end())
            {
                assert( unscheduledBytes == 0 );

                break;
            }

            //At this point, urlRequestTaskItr points to a URL request task that has
            //byte ranges to schedule.

            //Check the request size which is the byte count to target for the current
            //scheduling bucket.
            ullong requestSize = schedulingBucketItr->mByteCountToTarget;

            //Try to meet the minimum request size.
            if (requestSize < mMinRequestSize)
            {
                requestSize = mMinRequestSize;
            }

            //Check the available byte range from the URL request task.
            ByteRange byteRange;

            retVal = (*urlRequestTaskItr)->peekByteRange(
                                                    0xFFFFFFFF,
                                                    byteRange );

            assert( retVal == AS_NO_ERROR );

            if ((requestSize + mMinRequestSize) >= (ullong) byteRange.getSize())
            {
                //Take the whole range.
                (*urlRequestTaskItr)->getByteRange(
                                                0xFFFFFFFF,
                                                byteRange );
            }
            else
            {
                //Take the partial range.

                //But round end down to byte range alignment boundary, if possible
                ullong const requestNextByte = byteRange.start() + requestSize;
                ullong const requestNextBoundary = ( requestNextByte / (ullong) mByteRangeAlignment ) * (ullong) mByteRangeAlignment;
                requestSize = ( requestNextBoundary > (ullong) byteRange.start() ) ? ( requestNextBoundary - byteRange.start() ) : requestSize;

                (*urlRequestTaskItr)->getByteRange(
                                                requestSize,
                                                byteRange );
            }

            assert( unscheduledBytes >= (ullong) byteRange.size() );

            AseUrl requestUrl;
            ByteRange requestedByteRange = byteRange;
            ByteRanges existingByteRanges;
            bool pipelined;

            requestUrl = getUrlRedirectDestination(
                                (*urlRequestTaskItr)->getCurrentUrl() );

            retVal = schedulingBucketItr->mByteRangeDownloaderPtr->requestByteRange(
                                timeNow,
                                *urlRequestTaskItr,
                                requestUrl,
                                /*in/out*/requestedByteRange,
                                /*out*/existingByteRanges,
                                /*out*/pipelined );

            if (retVal == AS_NO_ERROR)
            {
                DEBUGLOG(
                    McplTrackDownloader2,
                    "%u scheduled %llu bytes of %llu to byte range downloader %u%s, ret: %d.",
                    mTrackId,
                    requestedByteRange.size(),
                    byteRange.size(),
                    schedulingBucketItr->mByteRangeDownloaderPtr->getId(),
                    ( requestedByteRange.end() + 1 ) % 4096 == 0 ? ", aligned" : "",
                    retVal );

                (*urlRequestTaskItr)
                    ->getMediaRequestTask()
                        ->requestStarted( false /*connecting*/, pipelined );

                ullong const scheduled = requestedByteRange.size() + existingByteRanges.size();
                unscheduledBytes -= scheduled;

                if ( scheduled < (ullong) byteRange.size() )
                {
                    DEBUGLOG(
                        McplTrackDownloader2,
                        "%u byte range downloader %u rejected %lld bytes of request due to fragmentation",
                        mTrackId,
                        schedulingBucketItr->mByteRangeDownloaderPtr->getId(),
                        byteRange.size() - scheduled );

                    (*urlRequestTaskItr)->returnByteRanges( byteRange - requestedByteRange - existingByteRanges );
                }

                ByteRangeReceivingTask byteRangeReceivingTask(
                                                    (*urlRequestTaskItr),
                                                    requestedByteRange );

                schedulingBucketItr->mByteRangeDownloaderPtr->assignByteRangeReceivingTask(
                                                     timeNow,
                                                     byteRangeReceivingTask );

                if (schedulingBucketItr->mByteCountToTarget < ((ullong) requestedByteRange.size() + 4096))
                {
                    totalByteCountToTarget -= schedulingBucketItr->mByteCountToTarget;
                    schedulingBucketItr->mByteCountToTarget = 0;
                }
                else
                {
                    totalByteCountToTarget -= requestedByteRange.getSize();
                    schedulingBucketItr->mByteCountToTarget -= requestedByteRange.size();
                }

                if (schedulingBucketItr->mByteCountToTarget != 0)
                {
                    schedulingBucketItr++;
                }
                else
                {
                    schedulingBucketItr = schedulingBucketList.erase( schedulingBucketItr );
                }
            }
            else if ( retVal == AS_IN_PROGRESS )
            {
                // All the bytes we requested are already in progress
                unscheduledBytes -= byteRange.getSize();

                DEBUGLOG(
                        McplTrackDownloader2,
                        "%u byte range downloader %u rejected request (%lld bytes) as duplicate, %llu bytes remain",
                        mTrackId,
                        schedulingBucketItr->mByteRangeDownloaderPtr->getId(),
                        byteRange.size(),
                        unscheduledBytes );
            }
            else
            {
                DEBUGLOG(
                    McplTrackDownloader2,
                    "%u failed scheduling %llu bytes to byte range downloader %u, ret: %d, %llu bytes remain",
                    mTrackId,
                    byteRange.getSize(),
                    schedulingBucketItr->mByteRangeDownloaderPtr->getId(),
                    retVal,
                    unscheduledBytes );

                (*urlRequestTaskItr)->returnByteRange( byteRange );

                retVal = processConnectionFailure(
                                            timeNow,
                                            schedulingBucketItr->mByteRangeDownloaderPtr );

                /* The failure may have caused other byte ranges in progress to be returned to the UrlRequestTasks */
                unscheduledBytes = countUnscheduledBytes();

                /* Need to go back to the first UrlRequestTask because of this */
                urlRequestTaskItr = mUrlRequestTaskList.begin();

                if (retVal == AS_HTTP_PIPELINING_NOT_STABLE)
                {
                    break;
                }
                else
                {
                    schedulingBucketItr = schedulingBucketList.erase( schedulingBucketItr );
                }
            }

            if (schedulingBucketItr == schedulingBucketList.end())
            {
                schedulingBucketItr = schedulingBucketList.begin();
            }
        }

        if (retVal == AS_HTTP_PIPELINING_NOT_STABLE)
        {
            break;
        }
    }

    if (retVal != AS_HTTP_PIPELINING_NOT_STABLE)
    {
        retVal = AS_NO_ERROR;
    }

    return retVal;
}

/** Add a bucket to the scheduling list */
void McplTrackDownloader2::addBucketToSchedulingList(
                                SchedulingBucketList& bucketList,
                                SchedulingBucket& bucket )
{
    SchedulingBucketList::iterator schedulingBucketItr;

    schedulingBucketItr = bucketList.begin();

    /** The bucket list has two parts, the first part contains the idle channels sorted by
     *  the idle duration from the longest to the shortest. The second part contains non-idle
     *  channels sorted by the time to idle from the shortest to the longest.
     */

    if (bucket.mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::IDLE_STATE)
    {
        while ((schedulingBucketItr != bucketList.end())
               && (schedulingBucketItr->mByteRangeDownloaderPtr->getState()
                   == ByteRangeDownloader::IDLE_STATE)
               && (schedulingBucketItr->mByteRangeDownloaderPtr->getStateElapsedTime()
                   > bucket.mByteRangeDownloaderPtr->getStateElapsedTime()))
        {
            schedulingBucketItr++;
        }
    }
    else
    {
        while ((schedulingBucketItr != bucketList.end())
               && (schedulingBucketItr->mByteRangeDownloaderPtr->getState()
                   == ByteRangeDownloader::IDLE_STATE ))
        {
            schedulingBucketItr++;
        }

        //This is sorted based on the time to idle.
        while ((schedulingBucketItr != bucketList.end())
               && (bucket.mAverageThroughput != 0)
               && ((schedulingBucketItr->mAverageThroughput == 0)
                   || (schedulingBucketItr->mByteRangeDownloaderPtr->getTotalOutstandingByteCount() * 8000 / schedulingBucketItr->mAverageThroughput
                       < (bucket.mByteRangeDownloaderPtr->getTotalOutstandingByteCount() * 8000 / bucket.mAverageThroughput))))
        {
            schedulingBucketItr++;
        }
    }

    bucketList.insert(
                    schedulingBucketItr,
                    bucket );

}

/** Check for slow connections */
void McplTrackDownloader2::closeSlowConnection( AseTimeVal const& timeNow )
{
    // Get the current throughputs
    uint64_t totalThroughput = 0;
    int connectionCount = 0;

    vector<pair<ByteRangeDownloaderMap::iterator,uint64_t> > throughputs;

    ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

    byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

    while (byteRangeDownloaderItr != mByteRangeDownloaderMap.end())
    {
        ByteRangeDownloader::State const brdstate = byteRangeDownloaderItr->second->getState();

        if ( ( brdstate == ByteRangeDownloader::IDLE_STATE )
            || ( brdstate == ByteRangeDownloader::DOWNLOADING_STATE ) )
        {
            uint64_t const throughput = byteRangeDownloaderItr->second->getAverageThroughput( 1, timeNow );

            totalThroughput += throughput;
            ++connectionCount;

            if ( brdstate == ByteRangeDownloader::DOWNLOADING_STATE )
            {
                AseTimeVal const downloadingTime = byteRangeDownloaderItr->second->getDownloadingElapsedTime( timeNow );
                if ( downloadingTime != AseTimeVal::INFINITE && downloadingTime > mMinSlowConnectionDuration )
                {
                    throughputs.push_back( make_pair( byteRangeDownloaderItr, throughput ) );
                }
            }
        }

        ++byteRangeDownloaderItr;
    }

    // Check if any connections are slower than average by specified threshold
    if ( connectionCount > 1 && !throughputs.empty() )
    {
        uint64_t threshold = 0;

        vector<pair<ByteRangeDownloaderMap::iterator,uint64_t> >::const_iterator it = throughputs.begin();
        for( ; it != throughputs.end(); ++it )
        {
            threshold = ( totalThroughput - it->second ) * mSlowConnectionThreshold / ( ( connectionCount - 1 ) * 100 );

#if 0
            DEBUGLOG( McplTrackDownloader2,
                        "%u evaluating brd %u for slow closure, %llukbit/s vs %llukbit/s average, %spredicted to be late",
                        mTrackId,
                        it->first->first,
                        it->second / 1000ULL,
                        ( totalThroughput - it->second ) / ( connectionCount - 1 ) / 1000ULL,
                        it->first->second->isFrontRequestPredictedToBeLate( timeNow ) ? "" : "not " );
#endif

            if ( it->second < threshold )
                break;
        }

        if ( it != throughputs.end() )
        {
            // Check if the current request is going to be late
            if ( !mCheckBlockDeadlineForSlowConnections
                    || it->first->second->isFrontRequestPredictedToBeLate( timeNow ) )
            {
                // Close and re-open the worst connection
                DEBUGLOG(
                        McplTrackDownloader2,
                        "%u disconnecting downloader %u - too slow (%llukbit/s vs %llukbit/s average)",
                        mTrackId,
                        it->first->first,
                        it->second / 1000ULL,
                        ( totalThroughput - it->second ) / ( connectionCount - 1 ) / 1000ULL );

                returnByteRangeReceivingTasks( it->first->second );

                it->first->second->disconnect( timeNow, AS_CONNECTION_SLOW );

                it->first->second->connect( timeNow,
                                            mCurrentConnectedDestination );
            }
        }
    }
}

/** Update the URL record list. */
int32_t McplTrackDownloader2::updateUrlRecordListOnTaskComplete(
                                int32_t retVal,
                                uint32_t& httpResponseCode,
                                UrlRequestTaskPtr pUrlRequestTask )
{
    UrlRecordList::iterator urlRecordItr = findUrlRecord( pUrlRequestTask->getCurrentUrl() );

    assert( urlRecordItr != mUrlRecordList.end() );

    if (retVal == AS_NO_ERROR)
    {
        urlRecordItr->mUrlReqeustTaskCount--;

        if (urlRecordItr->mUrlReqeustTaskCount == 0)
        {
            if (urlRecordItr == mUrlRecordList.begin())
            {
                mUrlRecordList.pop_front();

                if (!mUrlRecordList.empty())
                {
                    if (mUrlRecordList.front().mStatus != AS_NO_ERROR)
                    {
                        retVal = mUrlRecordList.front().mStatus;
                        httpResponseCode = mUrlRecordList.front().mHttpResponseCode;
                    }
                }
                else
                {
                    mGettingUrlRequestTasksBlockedFlag = false;
                }
            }
            else
            {
                mUrlRecordList.erase( urlRecordItr );
            }
        }
    }
    else
    {
        if (urlRecordItr == mUrlRecordList.begin())
        {
            //Do nothing.
        }
        else
        {
            urlRecordItr->mStatus = retVal;
            urlRecordItr->mHttpResponseCode = httpResponseCode;

            mGettingUrlRequestTasksBlockedFlag = true;

            retVal = AS_NO_ERROR;
        }
    }

    return retVal;
}

/** Return the byte ranges from a ByteRangeDownloader to the UrlRequestTask */
void McplTrackDownloader2::returnByteRangeReceivingTasks( ByteRangeDownloaderPtr pByteRangeDownloader )
{
    ByteRangeReceivingTaskList byteRangeReceivingTaskList;

    pByteRangeDownloader->retrieveByteReceivingTasks( byteRangeReceivingTaskList );

    ByteRangeReceivingTaskList::iterator byteRangeReceivingTaskItr;

    byteRangeReceivingTaskItr = byteRangeReceivingTaskList.begin();

    while (byteRangeReceivingTaskItr != byteRangeReceivingTaskList.end())
    {
        ByteRange const range(  byteRangeReceivingTaskItr->mByteRange.start() + byteRangeReceivingTaskItr->mReceivedByteCount,
                                byteRangeReceivingTaskItr->mByteRange.end() );

        DEBUGLOG(
            McplTrackDownloader2,
            "%u returned the byte range: %lld-%llu to URL request task: %u.",
            mTrackId,
            range.start(),
            range.end(),
            byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId() );

        if ( !byteRangeReceivingTaskItr->mUrlRequestTaskPtr->completed() && !range.empty() )
        {
            byteRangeReceivingTaskItr->mUrlRequestTaskPtr->returnByteRange( range );
        }

        byteRangeReceivingTaskItr++;
    }
}

/** Process the connection failure from a byte range downloader .*/
int32_t McplTrackDownloader2::processConnectionFailure(
                                    const AseTimeVal& timeNow,
                                    ByteRangeDownloaderPtr pByteRangeDownloader )
{
    DEBUGLOG(
        McplTrackDownloader2,
        "%u connection failure on byte range downloader %u, %d %d %s.",
        mTrackId,
        pByteRangeDownloader->getId(),
        pByteRangeDownloader->getConnectionFailureCode(),
        pByteRangeDownloader->getLowLevelFailureCode(),
        pByteRangeDownloader->getFailedToConnectFlag() ? "connecting" : "mid-connection" );

    int32_t retVal = AS_NO_ERROR;

    returnByteRangeReceivingTasks( pByteRangeDownloader );

    if (mBlockedByteRangeDownloaderId == pByteRangeDownloader->getId())
    {
        DEBUGLOG(
            McplTrackDownloader2,
            "%u reset the blocked byte range downloader.",
            mTrackId );

        mBlockedByteRangeDownloaderId = 0xFFFFFFFF;
    }

    if (!pByteRangeDownloader->getFailedToConnectFlag()
        && (pByteRangeDownloader->getTotalCompletedRequestCount() > 0)
        && (pByteRangeDownloader->getTotalCompletedRequestCount() < pByteRangeDownloader->getTotalIssuedRequestCount()))
    {
        mAverageCompletedHttpRequestCount = (mTotalConnectionBrokenCount
                                                * mAverageCompletedHttpRequestCount
                                                + pByteRangeDownloader->getTotalCompletedRequestCount())
                                            / (mTotalConnectionBrokenCount + 1);

        mTotalConnectionBrokenCount++;

        DEBUGLOG(
            McplTrackDownloader2,
            "%u updated the average competed HTTP request count: %u, connection broken count: %u, "
            "HTTP pipelining stability threshold: %u %u.",
            mTrackId,
            mAverageCompletedHttpRequestCount,
            mTotalConnectionBrokenCount,
            mPipeliningStabilityRequestThreshold,
            mPipeliningStabilityConnectionThreshold );

        if ((mAverageCompletedHttpRequestCount < mPipeliningStabilityRequestThreshold)
            && (mTotalConnectionBrokenCount > mPipeliningStabilityConnectionThreshold))
        {
            retVal = AS_HTTP_PIPELINING_NOT_STABLE;
        }
    }

    if ( mState != WAITING_FOR_URL_REQUEST_STATE
            && mState != WAITING_FOR_MEDIA_BUFFER_STATE )
    {
        AseErrorCode const failureCode = pByteRangeDownloader->getConnectionFailureCode();
        if ( failureCode == AS_DNS_NOT_FOUND || failureCode == AS_CONNECTION_REFUSED )
        {
            AseUrl::HostPort const& destination = pByteRangeDownloader->getDestination();
            UrlRequestTaskList::iterator it = mUrlRequestTaskList.begin();
            for( ; it != mUrlRequestTaskList.end(); ++it )
            {
                AseUrl const url = getUrlRedirectDestination( (*it)->getCurrentUrl() );

                if ( checkUrlDestination( url, destination.first, destination.second ) )
                {
                    (*it)->getMediaRequestTask()->requestError( failureCode, 0 );
                }
            }
        }
        else
        {
            mHttpRequestManagerPtr
                ->reportNetworkFailure( mTrackId,
                                        failureCode,
                                        mOriginalDestination );

        }
    }

    if ( ( retVal == AS_NO_ERROR )
            && ( ( mState != WAITING_FOR_URL_REQUEST_STATE
                && mState != WAITING_FOR_MEDIA_BUFFER_STATE )
                || ( timeNow - mCurentStateTime ) < mIdleReconnectWindow ) )
    {
        pByteRangeDownloader->reconnect( timeNow );
    }

    return retVal;
}

/** Remove a completed URL request task. */
bool McplTrackDownloader2::removeCompletedUrlRequestTask(
                                    UrlRequestTaskPtr pUrlRequestTask )
{
    UrlRequestTaskList::iterator urlRequestTaskItr;

    urlRequestTaskItr = mUrlRequestTaskList.begin();

    while (urlRequestTaskItr != mUrlRequestTaskList.end())
    {
        if ((*urlRequestTaskItr)->getId() == pUrlRequestTask->getId())
        {
            mUrlRequestTaskList.erase( urlRequestTaskItr );
            return true;
        }

        urlRequestTaskItr++;
    }

    return false;
}

/** Get the earliest byte range downloader timeout. */
AseTimeVal McplTrackDownloader2::getEarliestByteRangeDownloaderTimeout()
{
    AseTimeVal retTimeVal = AseTimeVal::INFINITE;

    ByteRangeDownloaderMap::iterator byteRangeDownloaderItr;

    byteRangeDownloaderItr = mByteRangeDownloaderMap.begin();

    while (byteRangeDownloaderItr != mByteRangeDownloaderMap.end())
    {
        AseTimeVal timeout;

        timeout = byteRangeDownloaderItr->second->getNextTimeout();

        if (retTimeVal > timeout)
        {
            retTimeVal = timeout;
        }

        byteRangeDownloaderItr++;
    }

    return retTimeVal;
}

/** Compute the next timeout time. */
void McplTrackDownloader2::computeNextTimeout()
{
    mNextTimeout = mTimerOne;

    if (mNextTimeout > mTimerTwo)
    {
        mNextTimeout = mTimerTwo;
    }

#if 0
    DEBUGLOG(
        McplTrackDownloader2,
        "%u computed the next timeout: %llu, now: %llu.",
        mTrackId,
        mTimerOne.ms(),
        timeNow.ms() );
#endif
}

/** Count the number of unscheduled bytes */
ullong McplTrackDownloader2::countUnscheduledBytes() const
{
    ullong unscheduled = 0;
    UrlRequestTaskList::const_iterator it = mUrlRequestTaskList.begin();
    for( ; it != mUrlRequestTaskList.end(); ++it )
    {
        unscheduled += (*it)->getUnscheduledDataLength();
    }

    return unscheduled;
}

/** Get the name of a state. */
const char* McplTrackDownloader2::getStateName( State state ) const
{
    const char* pRetVal = "Unknown State";

    switch (state)
    {
        case UNINITIALIZED_STATE:
            pRetVal = "Uninitialized State";
            break;

        case INIT_STATE:
            pRetVal = "Init State";
            break;

        case WAITING_FOR_URL_REQUEST_STATE:
            pRetVal = "Waiting For URL Request State";
            break;

        case CONNECTING_STATE:
            pRetVal = "Connecting State";
            break;

        case WORKING_STATE:
            pRetVal = "Working State";
            break;

        case WAITING_FOR_MEDIA_BUFFER_STATE:
            pRetVal = "Waiting For Media Buffer State";
            break;
    }

    return pRetVal;
}

/** A record for keeping track of URL request tasks for a URL */

/** Constructor. */
McplTrackDownloader2::UrlRecord::UrlRecord() : mUrlReqeustTaskCount( 0 ),
                                               mStatus( AS_NO_ERROR ),
                                               mHttpResponseCode( 0 )
{
}

/** Check if the URL record is empty. */
bool McplTrackDownloader2::UrlRecord::empty()
{
    return mUrl.IsEmpty();
}

/** Reset the record. */
void McplTrackDownloader2::UrlRecord::reset()
{
    mUrl = AseUrl();
    mUrlReqeustTaskCount = 0;
    mStatus = AS_NO_ERROR;
    mHttpResponseCode = 0;
}



}}
