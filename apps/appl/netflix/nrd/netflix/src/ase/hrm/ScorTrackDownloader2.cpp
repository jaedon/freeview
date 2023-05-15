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

#include "HttpRequestManager.h"
#include "ScorTrackDownloader2.h"


namespace netflix {
namespace ase {

DEFINE_LOGGER( ScorTrackDownloader2 );

/** Constructor. */
ScorTrackDownloader2::ScorTrackDownloader2(
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
                                                SCOR_TYPE,
                                                mediaType,
                                                config,
                                                pHttpRequestManager,
                                                pAsyncHttpClient,
                                                pTransportReporter,
                                                pUrlRedirectMap )
                                , mState( UNINITIALIZED_STATE )
                                , mLastConnectionSuccessful( false )
{
    assert( config.mOpenRange );
    assert( config.mConnections == 1 );

    (void) configure( pAseConfigParameter, config );
}

/** Destructor. */
ScorTrackDownloader2::~ScorTrackDownloader2()
{
}

int32_t ScorTrackDownloader2::configure(    IAseConfigParameterPtr pAseConfigParameter,
                                            TrackConfiguration const& config )
{
    if ( !config.mOpenRange
        || ( ( mState != UNINITIALIZED_STATE ) && config.mSocketBufferSize != mTrackConfiguration.mSocketBufferSize ) )
        return AS_NOT_SUPPORTED;

    int32_t retVal = BaseTrackDownloader2::configure( pAseConfigParameter, config );
    if ( retVal != AS_NO_ERROR )
        return retVal;

    if ( mState != UNINITIALIZED_STATE )
    {
        configureByteRangeDownloader();
    }

    return AS_NO_ERROR;
}


/** Test whether this downloader is in initializing state */
bool ScorTrackDownloader2::idle() const
{
    return ( mState == WAITING_FOR_URL_REQUEST_STATE );
}

/** Init the track downloader. */
int32_t ScorTrackDownloader2::init()
{
    int32_t retVal = AS_NO_ERROR;

    mByteRangeDownloaderPtr.reset(new ByteRangeDownloader(
                                            mTrackId,
                                            mAsyncHttpClientPtr,
                                            shared_from_this() ) );

    if (mByteRangeDownloaderPtr)
    {
        mState = WAITING_FOR_URL_REQUEST_STATE;
        mStateEnterTime = AseTimeVal::now();
        mTimerOne = AseTimeVal::ZERO;
        mDownloadingPausedFlag = false,

        configureByteRangeDownloader();

        DEBUGLOG(
            ScorTrackDownloader2,
            "%u created byte range downloader: %u.",
            mTrackId,
            mByteRangeDownloaderPtr->getId() );
    }
    else
    {
        retVal = AS_NOT_ENOUGH_MEMORY;
    }

     DEBUGLOG(
        ScorTrackDownloader2,
        "%u initialized, ret: %d.",
        mTrackId,
        retVal );

    return retVal;
}

void ScorTrackDownloader2::configureByteRangeDownloader()
{
    mByteRangeDownloaderPtr->setSocketReceiveBufferSize( mTrackConfiguration.mSocketBufferSize );
    mByteRangeDownloaderPtr->setReconnectBackoffTime( mDomainReconnectBackoffTime, mSyncDomainReconnectBackoffTime, mMaxSyncConnectFailures );
    mByteRangeDownloaderPtr->setTransportReportInterval( mTransportReportInterval );
    mByteRangeDownloaderPtr->setDownloadReportInterval( mDownloadReportInterval );
    mByteRangeDownloaderPtr->setInitialThroughput( 0, mInitialThroughput );
    mByteRangeDownloaderPtr->setThroughputFilterHalfLife( 0, mThroughputFilterHalfLife );
    mByteRangeDownloaderPtr->setThroughputMeasurementInterval( 0, mThroughputMeasurementInterval );
    mByteRangeDownloaderPtr->setProcess2xxLoopInterval( mProcess2xxLoopSleepMicros );
    mByteRangeDownloaderPtr->setTimeouts(   mHttpConnectTimeout,
                                            mHttpStallTimeout,
                                            AseTimeVal::ZERO,
                                            0 );
}

/** Deinitialize the track downloader. */
int32_t ScorTrackDownloader2::deinit()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    returnAllUrlRequestTasks();

    retVal = mByteRangeDownloaderPtr->disconnect( AseTimeVal::now() );

    DEBUGLOG(
        ScorTrackDownloader2,
        "%u freed byte range downloader: %u.",
        mTrackId,
        mByteRangeDownloaderPtr->getId() );

    mByteRangeDownloaderPtr.reset();

    DEBUGLOG(
        ScorTrackDownloader2,
        "%u deinitialized, ret: %d.",
        mTrackId,
        retVal );

    return retVal;
}

AseUrl::HostPort ScorTrackDownloader2::getDestination() const
{
    return mLastReceivingUrl.GetHostPort();
}

/** Get the next timeout time. */
AseTimeVal ScorTrackDownloader2::getNextTimeout()
{
    return mTimerOne;
}

/** Notify the downloader that a new task is waiting */
void ScorTrackDownloader2::notifyNewTaskIsWaiting()
{
    if ( mState == WAITING_FOR_URL_REQUEST_STATE )
    {
        // Reset our timeout so that we will be called to process the new request right away
        mTimerOne = AseTimeVal::now();
    }
}

/** Process selected connection. */
int32_t ScorTrackDownloader2::processSelectedHttpConnections(
                                    const AseTimeVal& timeNow,
                                    std::vector<IAsyncHttpConnectionPtr>& selectedHttpConnections,
                                    bool& dataReceived,
                                    bool& networkErrors )
{
    int32_t retVal = AS_NO_ERROR;

    dataReceived = false;
    networkErrors = false;

    assert( mState == WORKING_STATE );
    assert( selectedHttpConnections.size() == 1 );

    //Check the HTTP connection event type.

    IAsyncHttpConnection::Event httpConnectionEvent;

    httpConnectionEvent = selectedHttpConnections[0]->retrieveOutstandingEvent();

    if (httpConnectionEvent == IAsyncHttpConnection::STATE_CHANGE_EVENT)
    {
        //Process the HTTP connection.
        processHttpConnection( timeNow, networkErrors );
    }
    else if (httpConnectionEvent == IAsyncHttpConnection::RESPONSE_AVAILABLE_EVENT)
    {
       //Process the HTTP response.
        processHttpResponse( timeNow, dataReceived, networkErrors );
    }

    return retVal;
}

/** Process timeout. */
int32_t ScorTrackDownloader2::processTimeout( const AseTimeVal& timeNow )
{
    int32_t retVal = AS_NO_ERROR;

    if (timeNow >= mTimerOne)
    {
        switch (mState)
        {
            case UNINITIALIZED_STATE:
                break;

            case WAITING_FOR_URL_REQUEST_STATE:
            {
                //In this state, there cannot be any URL request task.
                assert( mUrlRequestTaskList.empty() );

                //In this state, the byte range downloader can only be in two states.
                assert( (mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::DISCONNECTED_STATE)
                        || (mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::SUSPENDED_STATE) );

                UrlRequestTaskPtr pUrlRequestTask;

                //Try to get a URL request task.
                retVal = mHttpRequestManagerPtr->getUrlRequestTask(
                                                        mTrackId,
                                                        pUrlRequestTask );

                if (retVal == AS_NO_ERROR)
                {
                    DEBUGLOG(
                        ScorTrackDownloader2,
                        "%u got a URL request task %u, URL: %s, 1, byte ranges: %s",
                        mTrackId,
                        pUrlRequestTask->getId(),
                        pUrlRequestTask->getCurrentUrl().GetPrefix().c_str(),
                        pUrlRequestTask->getByteRangesDebugString().c_str() );

                    AseUrl requestUrl = pUrlRequestTask->getCurrentUrl();

                    if (mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::SUSPENDED_STATE)
                    {
                        //The byte range downloader is in suspended state, check if the byte range
                        //from this URL request task is contiguous with the previous one.
                        ByteRange requestByteRange;

                        pUrlRequestTask->peekByteRange(
                                                0xFFFFFFFF,
                                                requestByteRange );


                        //Check if the byte ranges are contiguous.
                        //(1) It has to be the same URL.
                        //(2) The byte ranges are contiguous.
                        if ((mLastReceivingUrl == requestUrl)
                            && (mLastReceivingByteRange.contiguousWith( requestByteRange )))
                        {
                            pUrlRequestTask->getByteRange(
                                                    0xFFFFFFFF,
                                                    requestByteRange );

                            //The byte range is contiguous, create a byte range receiving
                            //task and assign it to the byte range downloader.
                            ByteRangeReceivingTask byteRangeReceivingTask(
                                                                pUrlRequestTask,
                                                                requestByteRange );

                            mByteRangeDownloaderPtr->assignByteRangeReceivingTask(
                                                                 timeNow,
                                                                 byteRangeReceivingTask );

                            mUrlRequestTaskList.push_back( pUrlRequestTask );

                            mLastReceivingByteRange = requestByteRange;

                            transitState( timeNow, WORKING_STATE );
                        }
                        else
                        {
                            //The byte range is not contiguous, disconnect the byte
                            //range downloader. The byte range downloader will connect
                            //again next.
                            mByteRangeDownloaderPtr->disconnect( timeNow );
                        }
                    }

                    if (mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::DISCONNECTED_STATE)
                    {
                        //The byte range downloader is in disconnected state, it will
                        //try to connect to the server.

                        mLastConnectionSuccessful = false;

                        //Check if there was any URL redirects for this request URL.
                        AseUrl destinationUrl;

                        destinationUrl = getUrlRedirectDestination( requestUrl );

                        setThroughputMonitor( pUrlRequestTask->getThroughputMonitor() );

                        pUrlRequestTask
                            ->getMediaRequestTask()
                                ->requestStarted( true /*connecting*/, false /*pipelined*/ );

                        //Append the URL request task to the pending list.
                        mUrlRequestTaskList.push_back( pUrlRequestTask );

                        //Try to connect to the server.
                        mByteRangeDownloaderPtr->setTimeouts(   mHttpConnectTimeout,
                                                                mHttpStallTimeout,
                                                                AseTimeVal::ZERO,
                                                                0 );

                        retVal = mByteRangeDownloaderPtr->connect(
                                                            timeNow,
                                                            destinationUrl.GetHostPort() );

                        DEBUGLOG(
                            ScorTrackDownloader2,
                            "%u connect byte range downloader %u to %s:%d, returned %d.",
                            mTrackId,
                            mByteRangeDownloaderPtr->getId(),
                            destinationUrl.GetHostPort().first.c_str(),
                            destinationUrl.GetHostPort().second,
                            retVal );

                        if (retVal == AS_NO_ERROR)
                        {
                            transitState( timeNow, WORKING_STATE );
                        }
                        else
                        {
                            //The connection failed.
                            mByteRangeDownloaderPtr->disconnect( timeNow );

                            mUrlRequestTaskList.pop_back();

                            mHttpRequestManagerPtr->returnUrlRequestTask( pUrlRequestTask );

                            removeUrlRedirectEntry( requestUrl );

                            mHttpRequestManagerPtr
                                ->reportNetworkFailure( mTrackId,
                                                        retVal,
                                                        requestUrl.GetHostPort() );

                            mTimerOne = timeNow + AseTimeVal::fromMS( mSyncDomainReconnectBackoffTime );
                        }
                    }
                }
                else
                {
                    mTimerOne = timeNow + AseTimeVal::fromMS( mUrlReqeustGetInterval );
                }

                break;
            }

            case WORKING_STATE:
            {
                ByteRangeReceivingTaskList byteRangeReceivingTaskList;

                retVal = mByteRangeDownloaderPtr->processTimeout( timeNow, byteRangeReceivingTaskList );

                if (retVal != AS_NO_ERROR)
                {
                    if (retVal == AS_ABANDONED)
                    {
                        // Inform the first UrlRequestTask that it has been aborted
                        ByteRangeReceivingTaskList abandonedTaskList;

                        mByteRangeDownloaderPtr->retrieveByteReceivingTasks( abandonedTaskList );

                        // We do not need to return the byte range to the UrlRequestTask because the byte ranges
                        // will be regenerated when we return the UrlRequestTask to the HttpRequestManager
                        // below

                        assert( !abandonedTaskList.empty() );

                        abandonedTaskList.front().mUrlRequestTaskPtr
                            ->getMediaRequestTask()
                                ->getMediaRequest()
                                    ->abandoned();

                        mByteRangeDownloaderPtr->disconnect( timeNow, AS_ABANDONED );
                    }
                    else
                    {
                        //A byte range receiving task timed out, report domain timeout failure.
                        mHttpRequestManagerPtr->reportNetworkFailure(   mTrackId,
                                                                        retVal,
                                                                        mUrlRequestTaskList.front()->getCurrentUrl().GetHostPort() );

                        mByteRangeDownloaderPtr->disconnect( timeNow );
                    }

                    returnAllUrlRequestTasks();

                    mTimerOne = timeNow;

                    transitState( timeNow, WAITING_FOR_URL_REQUEST_STATE );
                }

                // The SCOR track downloader ignores any tasks returned for duplication in byteRangeReceivingTaskList

                break;
            }

            case WAITING_FOR_MEDIA_BUFFER_STATE:
            {
                //It is time to check media buffer availability.

                if (mHttpRequestManagerPtr->checkMediaBufferAvailability( mTrackId ))
                {
                    if ( mDisconnectedWhileIdle )
                    {
                        mTimerOne = timeNow;

                        transitState( timeNow, WAITING_FOR_URL_REQUEST_STATE );
                    }
                    else
                    {
                        mByteRangeDownloaderPtr->notifyMediaBufferAvailability(
                                                                    timeNow,
                                                                    true );

                        transitState( timeNow, WORKING_STATE );
                    }
                }
                else
                {
                    mTimerOne = timeNow + AseTimeVal::fromMS( mMediaBufferCheckInterval );

                    if ( !mDisconnectedWhileIdle &&
                            ( timeNow - mStateEnterTime ) > mIdleDisconnectTime )
                    {
                        mByteRangeDownloaderPtr->disconnect( timeNow );

                        returnAllUrlRequestTasks();

                        mDisconnectedWhileIdle = true;
                    }
                }

                break;
            }
        }
    }

    return AS_NO_ERROR;
}

/** Pause downloading. */
int32_t ScorTrackDownloader2::pauseDownloading( const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    BaseTrackDownloader2::pauseDownloading( timeNow );

    retVal = mByteRangeDownloaderPtr->pauseDownloading( timeNow );

    return retVal;
}

/** Resume downloading. */
int32_t ScorTrackDownloader2::resumeDownloading( const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    BaseTrackDownloader2::resumeDownloading( timeNow );

    retVal = mByteRangeDownloaderPtr->resumeDownloading( timeNow );

    return retVal;
}

bool ScorTrackDownloader2::checkForUrlChanges()
{
    if ( mUrlRequestTaskList.empty() )
        return false;

    return mUrlRequestTaskList.front()->selectUrl();
}

/** Transit to a new state */
void ScorTrackDownloader2::transitState(
                                const AseTimeVal& timeNow,
                                State newState )
{
    DEBUGLOG(
        ScorTrackDownloader2,
        "%u transited from %s to %s.",
        mTrackId,
        getStateName( mState ),
        getStateName( newState ) );

    if (mState != newState)
    {
        if (newState == WAITING_FOR_URL_REQUEST_STATE)
        {
            assert( mUrlRequestTaskList.empty() );
        }
        else if (newState == WORKING_STATE)
        {
            mTimerOne = mByteRangeDownloaderPtr->getNextTimeout();
        }
        else if (newState == WAITING_FOR_MEDIA_BUFFER_STATE)
        {
            mTimerOne = timeNow + AseTimeVal::fromMS( mMediaBufferCheckInterval );

            mDisconnectedWhileIdle = false;
        }

        mState = newState;

        mStateEnterTime = timeNow;
    }

    return;
}

 /** Process a HTTP connection. */
void ScorTrackDownloader2::processHttpConnection( const AseTimeVal& timeNow, bool& networkErrors )
{
    int32_t retVal;

    networkErrors = false;

    retVal = mByteRangeDownloaderPtr->processHttpConnection( timeNow );

    if (retVal == AS_NO_ERROR)
    {
        //The connection is established.
        assert( mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::IDLE_STATE );

        mLastConnectionSuccessful = true;

        UrlRequestTaskPtr pUrlRequestTask = mUrlRequestTaskList.front();

        //Issue the HTTP request.
        ByteRange byteRange;

        pUrlRequestTask->peekByteRange(
                                0xFFFFFFFF,
                                byteRange );

        ByteRange requestByteRange( byteRange.getStart(), -1 );
        ByteRanges existingRequests;
        bool pipelined;

        AseUrl requestUrl;

        requestUrl = getUrlRedirectDestination(
                            pUrlRequestTask->getCurrentUrl() );

        retVal = mByteRangeDownloaderPtr->requestByteRange(
                                            timeNow,
                                            pUrlRequestTask,
                                            requestUrl,
                                            requestByteRange,
                                            existingRequests,
                                            pipelined );

        if (retVal == AS_NO_ERROR)
        {
            //The HTTP request is issued successfully, create and assign the corresponding
            //byte range receiving task.
            mUrlRequestTaskList.front()->getByteRange(
                                                0xFFFFFFFF,
                                                byteRange );

            ByteRangeReceivingTask byteRangeReceivingTask(
                                                pUrlRequestTask,
                                                byteRange );

            mByteRangeDownloaderPtr->assignByteRangeReceivingTask(
                                                 timeNow,
                                                 byteRangeReceivingTask );

            mLastReceivingUrl = pUrlRequestTask->getCurrentUrl();

            mLastReceivingByteRange = byteRange;

            mTimerOne = mByteRangeDownloaderPtr->getNextTimeout();
        }
        else
        {
            //It failed to issue the HTTP request.
            mByteRangeDownloaderPtr->disconnect( timeNow );

            returnAllUrlRequestTasks();

            mTimerOne = timeNow + AseTimeVal::fromMS( mDomainReconnectBackoffTime );

            transitState( timeNow, WAITING_FOR_URL_REQUEST_STATE );
        }
    }
    else
    {
        DEBUGLOG(
            ScorTrackDownloader2,
            "%u connection failure on byte range downloader %u, %d %d %s.",
            mTrackId,
            mByteRangeDownloaderPtr->getId(),
            mByteRangeDownloaderPtr->getConnectionFailureCode(),
            mByteRangeDownloaderPtr->getLowLevelFailureCode(),
            mByteRangeDownloaderPtr->getFailedToConnectFlag() ? "connecting" : "mid-connection" );

        networkErrors = true;

        AseErrorCode const failureCode = mByteRangeDownloaderPtr->getConnectionFailureCode();
        AseUrl::HostPort const& destination = mByteRangeDownloaderPtr->getDestination();
        if ( failureCode == AS_DNS_NOT_FOUND || failureCode == AS_CONNECTION_REFUSED )
        {
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
                                        mUrlRequestTaskList.front()->getCurrentUrl().GetHostPort());

        }

        //The TCP connection failed somehow.
        mByteRangeDownloaderPtr->disconnect( timeNow );

        returnAllUrlRequestTasks();

        if ( retVal == AS_HTTP_CONNECTION_STALL )
        {
            mTimerOne = timeNow;
        }
        else
        {
            uint32_t const timeoutMs =  ( retVal == AS_HTTP_CONNECTION_STALL ) ? 0
                                        : ( mLastConnectionSuccessful || retVal == AS_HTTP_CONNECTION_TIMEOUT ) ?
                                        mDomainReconnectBackoffTime : mSyncDomainReconnectBackoffTime;

            mTimerOne = timeNow + AseTimeVal::fromMS( timeoutMs );
        }

        transitState( timeNow, WAITING_FOR_URL_REQUEST_STATE );
    }

    return;
}

/** Process a HTTP response. */
void ScorTrackDownloader2::processHttpResponse( const AseTimeVal& timeNow, bool& dataReceived, bool& networkErrors )
{
    int32_t retVal;

    dataReceived = false;

    ByteRangeReceivingTaskList byteRangeReceivingTaskList;

    assert( mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::DOWNLOADING_STATE );

    retVal = mByteRangeDownloaderPtr->processHttpResponse(
                                                timeNow,
                                                byteRangeReceivingTaskList );

    if (retVal == AS_NO_ERROR)
    {
        assert( (mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::DOWNLOADING_STATE)
                || (mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::SUSPENDED_STATE)
                || (mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::IDLE_STATE) );

        dataReceived = true;

        //Report the domain status when there are finished byte range receiving task.
        if (!byteRangeReceivingTaskList.empty())
        {
            assert( byteRangeReceivingTaskList.size() == 1 );

            ByteRangeReceivingTaskList::iterator byteRangeReceivingTaskItr;

            byteRangeReceivingTaskItr = byteRangeReceivingTaskList.begin();

            while (byteRangeReceivingTaskItr != byteRangeReceivingTaskList.end())
            {
                if (byteRangeReceivingTaskItr->mUrlRequestTaskPtr->completed())
                {
                    DEBUGLOG(
                        ScorTrackDownloader2,
                        "%u completed a URL request task %u.",
                        mTrackId,
                        byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId() );

                    assert( byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId() ==
                                                        mUrlRequestTaskList.front()->getId());

                    mHttpRequestManagerPtr->completeUrlRequestTask(
                                                byteRangeReceivingTaskItr->mUrlRequestTaskPtr );

                    mUrlRequestTaskList.pop_front();
                }

                byteRangeReceivingTaskItr++;
            }
        }

        if (mByteRangeDownloaderPtr->getState() != ByteRangeDownloader::IDLE_STATE)
        {
            //Try to get more URL request task if not blocked for doing that.
            UrlRequestTaskPtr pUrlRequestTask;

            retVal = mHttpRequestManagerPtr->getUrlRequestTask(
                                                    mTrackId,
                                                    pUrlRequestTask );

            if (retVal == AS_NO_ERROR)
            {
                DEBUGLOG(
                    ScorTrackDownloader2,
                    "%u got a URL request task %u, URL: %s, 2, byte ranges: %s",
                    mTrackId,
                    pUrlRequestTask->getId(),
                    pUrlRequestTask->getCurrentUrl().GetPrefix().c_str(),
                    pUrlRequestTask->getByteRangesDebugString().c_str() );

                mUrlRequestTaskList.push_back( pUrlRequestTask );
            }

            UrlRequestTaskList::iterator urlRequestTaskItr;

            urlRequestTaskItr = mUrlRequestTaskList.begin();

            while ((urlRequestTaskItr != mUrlRequestTaskList.end())
                   && ((*urlRequestTaskItr)->scheduled()))
            {
                urlRequestTaskItr++;
            }

            if (urlRequestTaskItr != mUrlRequestTaskList.end())
            {
                pUrlRequestTask = *urlRequestTaskItr;

                AseUrl requestUrl;

                requestUrl = pUrlRequestTask->getCurrentUrl();

                ByteRange requestByteRange;

                pUrlRequestTask->peekByteRange(
                                        0xFFFFFFFF,
                                        requestByteRange );

                //Check if the byte ranges are contiguous.
                //(1) It has to be the same URL.
                //(2) The byte ranges are contiguous.
                if ((mLastReceivingUrl == requestUrl)
                    && (mLastReceivingByteRange.contiguousWith( requestByteRange )))
                {
                    //Create the byte range receiving task and assign it to the byte
                    //range downloader.
                    pUrlRequestTask->getByteRange(
                                            0xFFFFFFFF,
                                            requestByteRange );

                    ByteRangeReceivingTask byteRangeReceivingTask(
                                                        pUrlRequestTask,
                                                        requestByteRange );

                    mByteRangeDownloaderPtr->assignByteRangeReceivingTask(
                                                         timeNow,
                                                         byteRangeReceivingTask );

                    mLastReceivingByteRange = requestByteRange;

                    mTimerOne = mByteRangeDownloaderPtr->getNextTimeout();
                }
                else
                {
                    if (mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::DOWNLOADING_STATE)
                    {
                        mTimerOne = mByteRangeDownloaderPtr->getNextTimeout();
                    }
                    else
                    {
                        //If the byte range is not contiguous, return all the URL request
                        //tasks and transit to waiting for URL request tasks state.
                        mByteRangeDownloaderPtr->disconnect( timeNow );

                        returnAllUrlRequestTasks();

                        mTimerOne = timeNow;

                        transitState( timeNow, WAITING_FOR_URL_REQUEST_STATE );
                    }
                }
            }
            else
            {
                if (mByteRangeDownloaderPtr->getState() == ByteRangeDownloader::DOWNLOADING_STATE)
                {
                    mTimerOne = mByteRangeDownloaderPtr->getNextTimeout();
                }
                else
                {
                    //If there is more URL request task, transit to waiting for URL request task state.
                    mTimerOne = timeNow + AseTimeVal::fromMS( mUrlReqeustGetInterval );

                    transitState( timeNow, WAITING_FOR_URL_REQUEST_STATE );

                }
            }
        }
        else
        {
            //The HTTP request is finished.
            mByteRangeDownloaderPtr->disconnect( timeNow );

            returnAllUrlRequestTasks();

            mTimerOne = timeNow;

            transitState( timeNow, WAITING_FOR_URL_REQUEST_STATE );
        }
    }
    else if (retVal == AS_MEDIA_BUFFER_UNAVAILABLE)
    {
        transitState( timeNow, WAITING_FOR_MEDIA_BUFFER_STATE );
    }
    else if (retVal == AS_HTTP_RESPONSE_3XX)
    {
        dataReceived = true;

        AseUrl originalUrl;
        AseUrl newUrl;

        mByteRangeDownloaderPtr->getLastUrlRedirectInfo(
                                    originalUrl,
                                    newUrl );

        if ( !addUrlRedirectEntry( originalUrl, newUrl ) )
        {
            retVal = AS_HTTP_TOO_MANY_REDIRECTS;

            AseUrl const& url = mUrlRequestTaskList.front()->getCurrentUrl();

            removeUrlRedirectEntry( url );

            reportErrorToUrlRequestTasks( url, retVal, 0 );

        }

        mByteRangeDownloaderPtr->disconnect( timeNow );

        returnAllUrlRequestTasks();

        mTimerOne = timeNow;

        transitState( timeNow, WAITING_FOR_URL_REQUEST_STATE );
    }
    else if ((retVal == AS_HTTP_RESPONSE_4XX)
             || (retVal == AS_HTTP_RESPONSE_5XX)
             || (retVal == AS_HTTP_PROTOCOL_ERROR))
    {
        dataReceived = true;

        mByteRangeDownloaderPtr->disconnect( timeNow );

        AseUrl const& url = mUrlRequestTaskList.front()->getCurrentUrl();

        removeUrlRedirectEntry( url );

        reportErrorToUrlRequestTasks( url, retVal, mByteRangeDownloaderPtr->getLastHttpResponseCode() );

        returnAllUrlRequestTasks();

        mTimerOne = timeNow;

        transitState( timeNow, WAITING_FOR_URL_REQUEST_STATE );
    }
    else
    {
        networkErrors = true;

        mByteRangeDownloaderPtr->disconnect( timeNow );

        returnAllUrlRequestTasks();

        mTimerOne = timeNow +( ( retVal == AS_ABANDONED) ? 0 : AseTimeVal::fromMS( mDomainReconnectBackoffTime ) );

        transitState( timeNow, WAITING_FOR_URL_REQUEST_STATE );
    }
}

/** Return all URL request tasks. */
void ScorTrackDownloader2::returnAllUrlRequestTasks()
{
    //Return all existing URL requests in the reverse order they were requested..
    UrlRequestTaskList::reverse_iterator urlRequestTaskItr;

    urlRequestTaskItr = mUrlRequestTaskList.rbegin();

    while (urlRequestTaskItr != mUrlRequestTaskList.rend())
    {
        DEBUGLOG(
            ScorTrackDownloader2,
            "%u returned a URL request task %u.",
            mTrackId,
            (*urlRequestTaskItr)->getId() );

        mHttpRequestManagerPtr->returnUrlRequestTask( *urlRequestTaskItr );

        urlRequestTaskItr++;
    }

    mUrlRequestTaskList.clear();

    return;
}

void ScorTrackDownloader2::reportErrorToUrlRequestTasks(    AseUrl const&   url,
                                                            AseErrorCode    status,
                                                            uint32_t        httpCode )
{
    UrlRequestTaskList::iterator it = mUrlRequestTaskList.begin();
    for( ; it != mUrlRequestTaskList.end(); ++it )
    {
        if ( (*it)->getCurrentUrl() == url )
        {
            (*it)->getMediaRequestTask()->requestError( status, httpCode );
        }
    }
}


/** Get the name of a state. */
const char* ScorTrackDownloader2::getStateName( State state ) const
{
    const char* pRetVal = "Unknown State";

    switch (state)
    {
        case UNINITIALIZED_STATE:
            pRetVal = "Uninitialized State";
            break;

        case WAITING_FOR_URL_REQUEST_STATE:
            pRetVal = "Waiting For URL Request State";
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

}}
