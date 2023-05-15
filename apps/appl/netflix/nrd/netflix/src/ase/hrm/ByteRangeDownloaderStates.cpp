/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DebugLogging.h"
#include <nrdbase/AseErrorCodes.h>
#include "StreamersLog.h"
#include "ByteRangeDownloaderStates.h"
#include "ByteRangeDownloaderEventListener.h"



namespace netflix {
namespace ase {

DEFINE_LOGGER( ByteRangeDownloaderStates );

/** Enter the state. */
void ByteRangeDownloaderBaseState::enter(
                                    ByteRangeDownloader& byteRangeDownloader,
                                    const AseTimeVal& timeNow )
{
    byteRangeDownloader.mCurrentStateEnteringTime = timeNow;
    byteRangeDownloader.mTimerOne = AseTimeVal::INFINITE;

    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u entered %s.",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        name() );
}

/** Exit the state. */
void ByteRangeDownloaderBaseState::exit(
                                    ByteRangeDownloader& byteRangeDownloader,
                                    const AseTimeVal& /* timeNow */ )
{
#ifndef BUILD_DEBUG
    NRDP_UNUSED(byteRangeDownloader);
#endif
    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u exit %s.",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        name() );
}

/** Process HTTP connection event. */
int32_t ByteRangeDownloaderBaseState::processHttpConnection(
                                        ByteRangeDownloader& /* byteRangeDownloader */,
                                        const AseTimeVal& /* timeNow */ )
{
    assert( false );

    return AS_NO_ERROR;
}

/** Process HTTP response event. */
int32_t ByteRangeDownloaderBaseState::processHttpResponse(
                                        ByteRangeDownloader& /* byteRangeDownloader */,
                                        const AseTimeVal& /* timeNow */,
                                        ByteRangeReceivingTaskList& /* byteRangeReceivingTaskList */ )
{
    assert( false );

    return AS_NO_ERROR;
}

/** Process timeout. */
int32_t ByteRangeDownloaderBaseState::processTimeout(
                                        ByteRangeDownloader& byteRangeDownloader,
                                        const AseTimeVal& /* timeNow */,
                                        ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
#ifndef BUILD_DEBUG
    NRDP_UNUSED(byteRangeDownloader);
#endif
    NRDP_UNUSED(byteRangeReceivingTaskList);

    assert( byteRangeDownloader.mTimerOne == AseTimeVal::INFINITE );

    return AS_NO_ERROR;
}

/** Pause downloading. */
int32_t ByteRangeDownloaderBaseState::pauseDownloading(
                                        ByteRangeDownloader& byteRangeDownloader,
                                        const AseTimeVal& /* timeNow */ )
{
    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u paused downloading, current paused flag: %d, ",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mDownloadingPausedFlag );

    byteRangeDownloader.mDownloadingPausedFlag = true;

    return AS_NO_ERROR;
}

/** Resume downloading. */
int32_t ByteRangeDownloaderBaseState::resumeDownloading(
                                        ByteRangeDownloader& byteRangeDownloader,
                                        const AseTimeVal& /* timeNow */ )
{
    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u resumed downloading, current paused flag: %d, ",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mDownloadingPausedFlag );

    byteRangeDownloader.mDownloadingPausedFlag = false;

    return AS_NO_ERROR;
}

/** Notify the availability of the media buffer. */
int32_t ByteRangeDownloaderBaseState::notifyMediaBufferAvailability(
                                        ByteRangeDownloader& byteRangeDownloader,
                                        const AseTimeVal& /* timeNow  */,
                                        bool mediaBufferAvailable )
{
    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u was notified the media buffer availability, old: %s, new: %s.",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mMediaBufferAvailableFlag ? "Yes" : "No",
        mediaBufferAvailable ? "Yes" : "No" );

    byteRangeDownloader.mMediaBufferAvailableFlag = mediaBufferAvailable;

    return AS_NO_ERROR;
}

/**
 *  @class ByteRangeDownloaderDisconnectedState
 */

/** Return the state. */
ByteRangeDownloader::State ByteRangeDownloaderDisconnectedState::state() const
{
    return ByteRangeDownloader::DISCONNECTED_STATE;
}

/** Return the state name. */
const char* ByteRangeDownloaderDisconnectedState::name() const
{
    return "Disconnected State";
}

/** Enter the state. */
void ByteRangeDownloaderDisconnectedState::enter(
                                            ByteRangeDownloader& byteRangeDownloader,
                                            const AseTimeVal& timeNow )
{
    ByteRangeDownloaderBaseState::enter( byteRangeDownloader, timeNow );

    assert( !byteRangeDownloader.mAsyncHttpConnectionPtr );

    byteRangeDownloader.clearHttpRequests( timeNow );

    byteRangeDownloader.mMediaBufferAvailableFlag = true;

    byteRangeDownloader.mFirstConnectionEstablishedTime = AseTimeVal::INFINITE;
    byteRangeDownloader.mCurrentConnectionEstablishedTime = AseTimeVal::INFINITE;

    byteRangeDownloader.mHttpRequestTaskList.clear();
    byteRangeDownloader.mByteRangeReceivingTaskList.clear();

    byteRangeDownloader.mConnectionFailureCode = 0;
    byteRangeDownloader.mLowLevelFailureCode = 0;
    byteRangeDownloader.mFailedToConnectFlag = false;
    byteRangeDownloader.mTcpDisconnectState = TCP_DISCONNECT_UNKNOWN_STATE;

    byteRangeDownloader.mInitialThroughputs[0] = byteRangeDownloader.getAverageThroughput(
                                                                                0,
                                                                                timeNow );

    byteRangeDownloader.mInitialThroughputs[1] = byteRangeDownloader.getAverageThroughput(
                                                                                1,
                                                                                timeNow );

    byteRangeDownloader.mTotalIssuedRequestCount = 0;
    byteRangeDownloader.mTotalCompletedRequestCount = 0;

    byteRangeDownloader.mTotalRequestedByteCount = 0;
    byteRangeDownloader.mTotalReceivedByteCount = 0;

    byteRangeDownloader.mConnectionDataReceivedTime = AseTimeVal::INFINITE;
}

/** Exit the state. */
void ByteRangeDownloaderDisconnectedState::exit(
                                            ByteRangeDownloader& byteRangeDownloader,
                                            const AseTimeVal& timeNow )
{
    byteRangeDownloader.mAverageThroughputs[0] = AverageThroughput(
                            AseTimeVal::fromMS( byteRangeDownloader.mThroughputFilterHalfLives[0] ),
                            AseTimeVal::fromMS( byteRangeDownloader.mThroughputMeasurementIntervals[0] ),
                            byteRangeDownloader.mInitialThroughputs[0],
                            true );

    byteRangeDownloader.mAverageThroughputs[1] = AverageThroughput(
                            AseTimeVal::fromMS( byteRangeDownloader.mThroughputFilterHalfLives[1] ),
                            AseTimeVal::fromMS( byteRangeDownloader.mThroughputMeasurementIntervals[1] ),
                            byteRangeDownloader.mInitialThroughputs[1],
                            true );


    byteRangeDownloader.mAverageThroughputs[2] = AverageThroughput(
                            AseTimeVal::fromMS( byteRangeDownloader.mThroughputFilterHalfLives[2] ),
                            AseTimeVal::fromMS( byteRangeDownloader.mThroughputMeasurementIntervals[2] ),
                            0,
                            true );

    ByteRangeDownloaderBaseState::exit( byteRangeDownloader, timeNow );
}

/**
 *  @class ByteRangeDownloaderWaitingToReconnectState
 */

/** Return the state. */
ByteRangeDownloader::State ByteRangeDownloaderWaitingToReconnectState::state() const
{
    return ByteRangeDownloader::WAITING_TO_RECONNECT_STATE;
}

/** Return the state name. */
const char* ByteRangeDownloaderWaitingToReconnectState::name() const
{
    return "Waiting To Reconnect State";
}

/** Enter the state. */
void ByteRangeDownloaderWaitingToReconnectState::enter(
                                                    ByteRangeDownloader& byteRangeDownloader,
                                                    const AseTimeVal& timeNow )
{
    ByteRangeDownloaderBaseState::enter( byteRangeDownloader, timeNow );

    byteRangeDownloader.mCurrentConnectionEstablishedTime = AseTimeVal::INFINITE;

    byteRangeDownloader.mMediaBufferAvailableFlag = true;

    byteRangeDownloader.mHttpRequestTaskList.clear();
    byteRangeDownloader.mByteRangeReceivingTaskList.clear();

    byteRangeDownloader.mConnectionFailureCode = 0;
    byteRangeDownloader.mLowLevelFailureCode = 0;
    byteRangeDownloader.mFailedToConnectFlag = false;
    byteRangeDownloader.mTcpDisconnectState = TCP_DISCONNECT_UNKNOWN_STATE;

    byteRangeDownloader.mTotalIssuedRequestCount = 0;
    byteRangeDownloader.mTotalCompletedRequestCount = 0;

    byteRangeDownloader.mTotalRequestedByteCount = 0;
    byteRangeDownloader.mTotalReceivedByteCount = 0;

    byteRangeDownloader.mConnectionDataReceivedTime - AseTimeVal::INFINITE;

    byteRangeDownloader.mTimerOne = timeNow
                                    + AseTimeVal::fromMS( byteRangeDownloader.mNextReconnectBackoffTime );
}

/** Process timeout. */
int32_t ByteRangeDownloaderWaitingToReconnectState::processTimeout(
                                                        ByteRangeDownloader& byteRangeDownloader,
                                                        const AseTimeVal& timeNow,
                                                        ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    NRDP_UNUSED( byteRangeReceivingTaskList );

    int32_t retVal = AS_UNKNOWN_ERROR;

    if (timeNow >= byteRangeDownloader.mTimerOne)
    {
        retVal = byteRangeDownloader.openConnection( timeNow );

        if (retVal == AS_NO_ERROR)
        {
            byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_CONNECTING_STATE );
        }
        else
        {
            byteRangeDownloader.mFastConnectionFailure = true;

            byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_CONNECTION_DOWN_STATE );
        }
    }
    else
    {
        retVal = AS_NO_ERROR;
    }

    return retVal;
}

/**
 *  @class ByteRangeDownloaderConnectingState
 */

/** Return the state. */
ByteRangeDownloader::State ByteRangeDownloaderConnectingState::state() const
{
    return ByteRangeDownloader::CONNECTING_STATE;
}

/** Return the state name. */
const char* ByteRangeDownloaderConnectingState::name() const
{
    return "Connecting State";
}

/** Process selected connection. */
int32_t ByteRangeDownloaderConnectingState::processHttpConnection(
                                                ByteRangeDownloader& byteRangeDownloader,
                                                const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u processed HTTP connection event, connection state: %u.",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mAsyncHttpConnectionPtr->getState() );

    if (byteRangeDownloader.mAsyncHttpConnectionPtr->getState() == IAsyncHttpConnection::IDLE_STATE)
    {
        DEBUGLOG(
            ByteRangeDownloaderStates,
            "%u %u established the connection %u to %s:%d, actual socket receive buffer size: %u. ",
            byteRangeDownloader.mParentId,
            byteRangeDownloader.mId,
            byteRangeDownloader.mAsyncHttpConnectionPtr->getId(),
            byteRangeDownloader.mDestination.first.c_str(),
            byteRangeDownloader.mDestination.second,
            byteRangeDownloader.mAsyncHttpConnectionPtr->getActualSocketReceiveBufferSize() );

        if (byteRangeDownloader.mFirstConnectionEstablishedTime == AseTimeVal::INFINITE)
        {
            byteRangeDownloader.mFirstConnectionEstablishedTime = timeNow;
        }

        byteRangeDownloader.mCurrentConnectionEstablishedTime = timeNow;

        //Transit to idle state despite of paused and media buffer availability flag.
        byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_IDLE_STATE );

        retVal = AS_NO_ERROR;
    }
    else
    {
        byteRangeDownloader.mFailedToConnectFlag = true;

        byteRangeDownloader.mConnectionFailureCode =
                    byteRangeDownloader.mAsyncHttpConnectionPtr->getConnectionFailureCode();

        byteRangeDownloader.mLowLevelFailureCode =
                    byteRangeDownloader.mAsyncHttpConnectionPtr->getLowLevelFailureCode();

        byteRangeDownloader.mFastConnectionFailure = ( byteRangeDownloader.mConnectionFailureCode != AS_HTTP_CONNECTION_TIMEOUT );

        DEBUGLOG(
            ByteRangeDownloaderStates,
            "%u %u failed to established the connection %u to %s:%d, status %d %d, %s",
            byteRangeDownloader.mParentId,
            byteRangeDownloader.mId,
            byteRangeDownloader.mAsyncHttpConnectionPtr->getId(),
            byteRangeDownloader.mDestination.first.c_str(),
            byteRangeDownloader.mDestination.second,
            byteRangeDownloader.mConnectionFailureCode,
            byteRangeDownloader.mLowLevelFailureCode,
            byteRangeDownloader.mFastConnectionFailure ? "fast failure" : "slow failure" );

        byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_CONNECTION_DOWN_STATE );

        retVal = byteRangeDownloader.mConnectionFailureCode;

        assert( retVal != AS_NO_ERROR );
    }

    return retVal;
}

/**
 *  @class ByteRangeDownloaderIdleState
 */

/** Return the state. */
ByteRangeDownloader::State ByteRangeDownloaderIdleState::state() const
{
    return ByteRangeDownloader::IDLE_STATE;
}

/** Return the state name. */
const char* ByteRangeDownloaderIdleState::name() const
{
    return "Idle State";
}

/** Process selected connection. */
int32_t ByteRangeDownloaderIdleState::processHttpConnection(
                                        ByteRangeDownloader& byteRangeDownloader,
                                        const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert( (byteRangeDownloader.mAsyncHttpConnectionPtr->getState()
                                    == IAsyncHttpConnection::FAILED_STATE)
            || (byteRangeDownloader.mAsyncHttpConnectionPtr->getState()
                                    == IAsyncHttpConnection::CLOSED_STATE) );

    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u processed HTTP connection event, connection state: %u.",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mAsyncHttpConnectionPtr->getState() );

    byteRangeDownloader.mConnectionFailureCode =
                            byteRangeDownloader.mAsyncHttpConnectionPtr->getConnectionFailureCode();

    byteRangeDownloader.mLowLevelFailureCode =
                            byteRangeDownloader.mAsyncHttpConnectionPtr->getLowLevelFailureCode();

    byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_CONNECTION_DOWN_STATE );

    retVal = byteRangeDownloader.mConnectionFailureCode;

    assert( retVal != AS_NO_ERROR );

    return retVal;
}

/**
 *  @class ByteRangeDownloaderDownloadingState
 */

/** Return the state. */
ByteRangeDownloader::State ByteRangeDownloaderDownloadingState::state() const
{
    return ByteRangeDownloader::DOWNLOADING_STATE;
}

/** Return the state name. */
const char* ByteRangeDownloaderDownloadingState::name() const
{
    return "Downloading State";
}

/** Enter the state. */
void ByteRangeDownloaderDownloadingState::enter(
                                            ByteRangeDownloader& byteRangeDownloader,
                                            const AseTimeVal& timeNow )
{
    ByteRangeDownloaderBaseState::enter( byteRangeDownloader, timeNow );

    assert( !byteRangeDownloader.mHttpRequestTaskList.empty() );

    byteRangeDownloader.mEventListenerPtr->reportDownloadingStarted(
                                                byteRangeDownloader.mId );

    if (byteRangeDownloader.mMinimumDuplicateTimeout > 0 &&
        byteRangeDownloader.
            mHttpRequestTaskList.
                    front()->mAsyncHttpRequestPtr->getState() <= IAsyncHttpRequest::WAITING_STATE)
    {
        byteRangeDownloader.setDuplicateRequestTimeout( timeNow );
    }

    if ( !byteRangeDownloader.mByteRangeReceivingTaskList.empty() )
    {
        byteRangeDownloader.mByteRangeReceivingTaskList.front()
            .mDataReceivedSinceEnteringDownloadingState = false;
    }
}

/** Exit the state. */
void ByteRangeDownloaderDownloadingState::exit(
                                            ByteRangeDownloader& byteRangeDownloader,
                                            const AseTimeVal& timeNow )
{
    byteRangeDownloader.mEventListenerPtr->reportDownloadingStopped(
                                                byteRangeDownloader.mId );

    //Stop (pause) the average throughput.
    byteRangeDownloader.mAverageThroughputs[0].readingPaused( timeNow );
    byteRangeDownloader.mAverageThroughputs[1].readingPaused( timeNow );
    byteRangeDownloader.mAverageThroughputs[2].readingPaused( timeNow );

    byteRangeDownloader.mTimerOne = AseTimeVal::INFINITE;
}

/** Process HTTP connection event. */
int32_t ByteRangeDownloaderDownloadingState::processHttpConnection(
                                                    ByteRangeDownloader& byteRangeDownloader,
                                                    const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert( (byteRangeDownloader.mAsyncHttpConnectionPtr->getState()
                                            == IAsyncHttpConnection::FAILED_STATE)
            || (byteRangeDownloader.mAsyncHttpConnectionPtr->getState()
                                            == IAsyncHttpConnection::CLOSED_STATE) );

    assert( !byteRangeDownloader.mHttpRequestTaskList.empty() );

    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u processed HTTP connection event, connection state: %u.",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mAsyncHttpConnectionPtr->getState() );

    byteRangeDownloader.mConnectionFailureCode =
                            byteRangeDownloader.mAsyncHttpConnectionPtr->getConnectionFailureCode();

    byteRangeDownloader.mLowLevelFailureCode =
                            byteRangeDownloader.mAsyncHttpConnectionPtr->getLowLevelFailureCode();

    if (byteRangeDownloader.
            mHttpRequestTaskList.
                front()->mAsyncHttpRequestPtr->getState() >= IAsyncHttpRequest::WAITING_STATE)
    {
        byteRangeDownloader.reportDownloadRecord(
                                byteRangeDownloader.mHttpRequestTaskList.front()->mHttpAttempt,
                                timeNow,
                                byteRangeDownloader.mConnectionFailureCode );
    }

    byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_CONNECTION_DOWN_STATE );

    retVal = byteRangeDownloader.mConnectionFailureCode;

    assert( retVal != AS_NO_ERROR );

    return retVal;
}

/** Process HTTP response event. */
int32_t ByteRangeDownloaderDownloadingState::processHttpResponse(
                                ByteRangeDownloader& byteRangeDownloader,
                                const AseTimeVal& timeNow,
                                ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    int32_t retVal = AS_NO_ERROR;

    IAsyncHttpRequestPtr pHttpRequest;
    IAsyncHttpRequest::State requestState;

    ByteRangeDownloader::HttpRequestTaskPtr pHttpRequestTask = byteRangeDownloader.
                                                                mHttpRequestTaskList.
                                                                    front();

    pHttpRequest = byteRangeDownloader.mAsyncHttpConnectionPtr->getTriggeringRequest();

    assert( pHttpRequest->getId() == pHttpRequestTask->mAsyncHttpRequestPtr->getId() );

    requestState = pHttpRequest->getState();
/*
    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u processed HTTP response event, response state: %u.",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        requestState );
*/
    if (requestState >= IAsyncHttpRequest::RECEIVING_BODY_STATE)
    {
        byteRangeDownloader.mLastHttpResponseCode = pHttpRequest->getResponseCode();

        if ( byteRangeDownloader.mConnectionDataReceivedTime == AseTimeVal::INFINITE )
        {
            byteRangeDownloader.mConnectionDataReceivedTime = timeNow;
        }

        if (pHttpRequestTask->mFreshFlag)
        {
            // Clear the duplicate request timer (abandon timer will be refreshed in getTimeout())
            byteRangeDownloader.mTimerOne = AseTimeVal::INFINITE;

            pHttpRequestTask->mHttpAttempt.mHttpCode = (int) byteRangeDownloader.mLastHttpResponseCode;
            pHttpRequestTask->mHttpAttempt.mServerTcpInfo = pHttpRequest->getResponseHeaderValue( "X-TCP-Info" );

            assert( !byteRangeDownloader.mByteRangeReceivingTaskList.empty() );
            byteRangeDownloader.mByteRangeReceivingTaskList.front().mUrlRequestTaskPtr->provideResponseHeaders( pHttpRequest->getResponseHeaders() );

            if (pHttpRequestTask->mNonPipelined)
            {
                AseTimeVal const responseTime( pHttpRequest->getFirstByteReceivedTime() - pHttpRequest->getFirstByteSentTime() );
                byteRangeDownloader.mEventListenerPtr->reportHttpResponseTime(
                                                            byteRangeDownloader.mId,
                                                            responseTime );

                byteRangeDownloader.mAverageResponseTime.addResponseTimeSample( responseTime.ms() );
            }

#if ENABLE_STREAMERS_LOG
            ManifestStreamPtr pManifestStream =
                                    pHttpRequestTask->mOriginalManifestUrl->getParent();

            ManifestTrackPtr pManifestTrack = pManifestStream->getParent();

            gStreamersLog.addHttpResponseRecord(
                                    byteRangeDownloader.mParentId,
                                    byteRangeDownloader.mId,
                                    byteRangeDownloader.mAsyncHttpConnectionPtr->getId(),
                                    pManifestTrack->getMediaType(),
                                    pManifestStream->getStreamKey().c_str(),
                                    pHttpRequestTask->mOriginalManifestUrl->getLocationKey().c_str(),
                                    pHttpRequestTask->mRequestUrl.str().c_str(),
                                    byteRangeDownloader.mLastHttpResponseCode );
#endif

            pHttpRequestTask->mFreshFlag = false;
        }

        if ((byteRangeDownloader.mLastHttpResponseCode == 200)
              && (pHttpRequestTask->mByteRange.getEnd() == -1))
        {
            if ((pHttpRequestTask->mAsyncHttpRequestPtr->getChunkedTransferFlag())
                || (pHttpRequestTask->mAsyncHttpRequestPtr->getContentLength() > 0))
            {
                retVal = byteRangeDownloader.processHttp2XXResponse(
                                                            timeNow,
                                                            byteRangeReceivingTaskList );
            }
            else
            {
                retVal = byteRangeDownloader.processHttpInvalidResponse(
                                                            timeNow,
                                                            byteRangeReceivingTaskList );
            }
        }
        else if (byteRangeDownloader.mLastHttpResponseCode == 206)
        {

            if ((pHttpRequestTask->mByteRange.getEnd() == -1)
                || (pHttpRequestTask->mByteRange.getSize()
                    == pHttpRequestTask->mAsyncHttpRequestPtr->getContentLength()))
            {
                retVal = byteRangeDownloader.processHttp2XXResponse(
                                                            timeNow,
                                                            byteRangeReceivingTaskList );
            }
            else
            {
                retVal = byteRangeDownloader.processHttpInvalidResponse(
                                                            timeNow,
                                                            byteRangeReceivingTaskList );
            }
        }
        else if ((byteRangeDownloader.mLastHttpResponseCode >= 300)
                 && (byteRangeDownloader.mLastHttpResponseCode <= 399))
        {
            retVal = byteRangeDownloader.processHttp3XXResponse(
                                                        timeNow,
                                                        byteRangeReceivingTaskList );
        }
        else if ((byteRangeDownloader.mLastHttpResponseCode >= 400)
                 && (byteRangeDownloader.mLastHttpResponseCode <= 499))
        {
            retVal = byteRangeDownloader.processHttp4XXResponse(
                                                        timeNow,
                                                        byteRangeReceivingTaskList );
        }
        else if ((byteRangeDownloader.mLastHttpResponseCode >= 500)
                && (byteRangeDownloader.mLastHttpResponseCode <= 599))
        {
            retVal = byteRangeDownloader.processHttp5XXResponse(
                                                        timeNow,
                                                        byteRangeReceivingTaskList );
        }
        else
        {
            retVal = byteRangeDownloader.processHttpInvalidResponse(
                                                        timeNow,
                                                        byteRangeReceivingTaskList );
        }
    }

    return retVal;
}

/** Process timeout. */
int32_t ByteRangeDownloaderDownloadingState::processTimeout(
                                                ByteRangeDownloader& byteRangeDownloader,
                                                const AseTimeVal& timeNow,
                                                ByteRangeReceivingTaskList& byteRangeReceivingTaskList )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    if (timeNow > byteRangeDownloader.mTimerOne)
    {
        DEBUGLOG(
            ByteRangeDownloaderStates,
            "%u %u processed HTTP timeout event, connection state: %u, request state %u, %llu-%llu %llu",
            byteRangeDownloader.mParentId,
            byteRangeDownloader.mId,
            byteRangeDownloader.mAsyncHttpConnectionPtr->getState(),
            byteRangeDownloader.mHttpRequestTaskList.front()->mAsyncHttpRequestPtr->getState(),
            byteRangeDownloader.mHttpRequestTaskList.front()->mByteRange.start(),
            byteRangeDownloader.mHttpRequestTaskList.front()->mByteRange.end(),
            byteRangeDownloader.mHttpRequestTaskList.front()->mReceivedByteCount);


        if (byteRangeDownloader.
                mHttpRequestTaskList.
                    front()->mAsyncHttpRequestPtr->getState() == IAsyncHttpRequest::WAITING_STATE)
        {
            // Http response timer - we duplicate requests when this fires
            assert(
                byteRangeDownloader.
                    mHttpRequestTaskList.
                        front()->mByteRange.contains(
                byteRangeDownloader.mByteRangeReceivingTaskList.front().mByteRange ) );

            byteRangeReceivingTaskList.insert( byteRangeReceivingTaskList.end(),
                                                byteRangeDownloader.mByteRangeReceivingTaskList.begin(),
                                                byteRangeDownloader.mByteRangeReceivingTaskList.end() );

            retVal = AS_NO_ERROR;
        }
        else if ( byteRangeDownloader.mByteRangeReceivingTaskList.front().mDataReceivedSinceEnteringDownloadingState
                && byteRangeDownloader.mByteRangeReceivingTaskList.front().mUrlRequestTaskPtr
                                            ->getMediaRequestTask()
                                                ->getAbandonTime() < timeNow )
        {
            byteRangeDownloader.mConnectionFailureCode = AS_ABANDONED;
            byteRangeDownloader.mLowLevelFailureCode = 0;

            retVal = AS_ABANDONED;
        }
        else
        {
            retVal = AS_NO_ERROR;
        }
    }
    else
    {
        retVal = AS_NO_ERROR;
    }

    byteRangeDownloader.mTimerOne = AseTimeVal::INFINITE;

    return retVal;
}

/** Pause downloading. */
int32_t ByteRangeDownloaderDownloadingState::pauseDownloading(
                                                ByteRangeDownloader& byteRangeDownloader,
                                                const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert( !byteRangeDownloader.mDownloadingPausedFlag );

    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u paused downloading, current paused flag: %d, ",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mDownloadingPausedFlag );

    byteRangeDownloader.mDownloadingPausedFlag = true;

    byteRangeDownloader.mAsyncHttpConnectionPtr->pauseReceiving();

    byteRangeDownloader.mHttpRequestTaskList.front()->mHttpAttempt.stop( timeNow );

    byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_SUSPENDED_STATE );

    retVal = AS_NO_ERROR;

    return retVal;
}

/** Notify the availability of the media buffer. */
int32_t ByteRangeDownloaderDownloadingState::notifyMediaBufferAvailability(
                                                ByteRangeDownloader& byteRangeDownloader,
                                                const AseTimeVal& timeNow,
                                                bool mediaBufferAvailable )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u was notified the media buffer availability, old: %s, new: %s.",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mMediaBufferAvailableFlag ? "Yes" : "No",
        mediaBufferAvailable ? "Yes" : "No" );

    byteRangeDownloader.mMediaBufferAvailableFlag = mediaBufferAvailable;

    if (!byteRangeDownloader.mMediaBufferAvailableFlag)
    {
        byteRangeDownloader.mAsyncHttpConnectionPtr->pauseReceiving();

        byteRangeDownloader.mHttpRequestTaskList.front()->mHttpAttempt.stop( timeNow );

        byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_SUSPENDED_STATE );
    }

    retVal = AS_NO_ERROR;

    return retVal;
}

/**
 *  @class ByteRangeDownloaderSuspendedState
 */

/** Return the state. */
ByteRangeDownloader::State ByteRangeDownloaderSuspendedState::state() const
{
    return ByteRangeDownloader::SUSPENDED_STATE;
}

/** Return the state name. */
const char* ByteRangeDownloaderSuspendedState::name() const
{
    return "Suspended State";
}

/** Enter the state. */
void ByteRangeDownloaderSuspendedState::enter(
                                    ByteRangeDownloader& byteRangeDownloader,
                                    const AseTimeVal& timeNow )
{
    ByteRangeDownloaderBaseState::enter( byteRangeDownloader, timeNow );

    assert( !byteRangeDownloader.mHttpRequestTaskList.empty());

    //Because of the suspension, the first HTTP request task in the queue if it is
    //a non-pipelined request, its response time is no longer accurate. Therefore,
    //clear the non-pipelined flag, so that the response time is not reported.
    byteRangeDownloader.mHttpRequestTaskList.front()->mNonPipelined = false;
}

/** Exit the state. */
void ByteRangeDownloaderSuspendedState::exit(
                                            ByteRangeDownloader& byteRangeDownloader,
                                            const AseTimeVal& timeNow )
{
#if 0
    //Reset the timeout of byte range receiving tasks.
    ByteRangeReceivingTaskList::iterator byteRangeReceivingTaskItr;

    byteRangeReceivingTaskItr = byteRangeDownloader.mByteRangeReceivingTaskList.begin();

    while (byteRangeReceivingTaskItr != byteRangeDownloader.mByteRangeReceivingTaskList.end())
    {
        if (byteRangeReceivingTaskItr->mTimeoutTime == AseTimeVal::ZERO)
        {
            byteRangeReceivingTaskItr->mTimeoutTime = timeNow
                                                      + byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getRequestTimeout();
        }
        else
        {
            byteRangeReceivingTaskItr->mTimeoutTime += timeNow
                                                       - byteRangeDownloader.mCurrentStateEnteringTime;
        }

        if (byteRangeReceivingTaskItr->mTimeoutTime != AseTimeVal::INFINITE)
        {
            DEBUGLOG(
                ByteRangeDownloaderStates,
                "%u %u reset the timeout for the byte range receiving task, URL request task ID: %u, "
                "byte range: %lld-%lld, timeout duration: %lld, timeout time: %lld.",
                byteRangeDownloader.mParentId,
                byteRangeDownloader.mId,
                byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getId(),
                byteRangeReceivingTaskItr->mByteRange.getStart(),
                byteRangeReceivingTaskItr->mByteRange.getEnd(),
                byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getRequestTimeout() == AseTimeVal::INFINITE ?
                    -1 : (llong) (byteRangeReceivingTaskItr->mUrlRequestTaskPtr->getRequestTimeout().ms()),
                byteRangeReceivingTaskItr->mTimeoutTime == AseTimeVal::INFINITE ?
                    -1 : (llong) (byteRangeReceivingTaskItr->mTimeoutTime.ms()) );

        }

        byteRangeReceivingTaskItr++;
    }
#endif

    ByteRangeDownloaderBaseState::exit( byteRangeDownloader, timeNow );
}

/** Process HTTP connection event. */
int32_t ByteRangeDownloaderSuspendedState::processHttpConnection(
                                                ByteRangeDownloader& byteRangeDownloader,
                                                const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert( (byteRangeDownloader.mAsyncHttpConnectionPtr->getState()
                                            == IAsyncHttpConnection::FAILED_STATE)
            || (byteRangeDownloader.mAsyncHttpConnectionPtr->getState()
                                            == IAsyncHttpConnection::CLOSED_STATE) );

    assert( !byteRangeDownloader.mHttpRequestTaskList.empty() );

    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u processed HTTP connection event, connection state: %u.",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mAsyncHttpConnectionPtr->getState() );

    byteRangeDownloader.mConnectionFailureCode =
                            byteRangeDownloader.mAsyncHttpConnectionPtr->getConnectionFailureCode();

    byteRangeDownloader.mLowLevelFailureCode =
                            byteRangeDownloader.mAsyncHttpConnectionPtr->getLowLevelFailureCode();

    if (byteRangeDownloader.
            mHttpRequestTaskList.
                front()->mAsyncHttpRequestPtr->getState() >= IAsyncHttpRequest::WAITING_STATE)
    {
        byteRangeDownloader.reportDownloadRecord(
                                byteRangeDownloader.mHttpRequestTaskList.front()->mHttpAttempt,
                                timeNow,
                                byteRangeDownloader.mConnectionFailureCode );
    }

    byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_CONNECTION_DOWN_STATE );

    retVal = byteRangeDownloader.mConnectionFailureCode;

    assert( retVal != AS_NO_ERROR );

    return retVal;
}

/** Resume downloading. */
int32_t ByteRangeDownloaderSuspendedState::resumeDownloading(
                                                ByteRangeDownloader& byteRangeDownloader,
                                                const AseTimeVal& timeNow )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert( byteRangeDownloader.mDownloadingPausedFlag );

    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u resumed downloading, current paused flag: %d, ",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mDownloadingPausedFlag );

    byteRangeDownloader.mDownloadingPausedFlag = false;

    if ((byteRangeDownloader.mMediaBufferAvailableFlag)
        && (!byteRangeDownloader.mByteRangeReceivingTaskList.empty()))
    {
        byteRangeDownloader.mAsyncHttpConnectionPtr->resumeReceiving();

        byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_DOWNLOADING_STATE );
    }

    retVal = AS_NO_ERROR;

    return retVal;
}

/** Notify the availability of the media buffer. */
int32_t ByteRangeDownloaderSuspendedState::notifyMediaBufferAvailability(
                                                ByteRangeDownloader& byteRangeDownloader,
                                                const AseTimeVal& timeNow,
                                                bool mediaBufferAvailable )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    DEBUGLOG(
        ByteRangeDownloaderStates,
        "%u %u was notified the media buffer availability, old: %s, new: %s.",
        byteRangeDownloader.mParentId,
        byteRangeDownloader.mId,
        byteRangeDownloader.mMediaBufferAvailableFlag ? "Yes" : "No",
        mediaBufferAvailable ? "Yes" : "No" );

    byteRangeDownloader.mMediaBufferAvailableFlag = mediaBufferAvailable;

    if (!byteRangeDownloader.mDownloadingPausedFlag
         && byteRangeDownloader.mMediaBufferAvailableFlag
         && (!byteRangeDownloader.mByteRangeReceivingTaskList.empty()))
    {
        byteRangeDownloader.mAsyncHttpConnectionPtr->resumeReceiving();

        byteRangeDownloader.transit( timeNow, &byteRangeDownloader.SM_DOWNLOADING_STATE );
    }

    retVal = AS_NO_ERROR;

    return retVal;
}

/**
 *  @class ByteRangeDownloaderConnectionDownState
 */

/** Return the state. */
ByteRangeDownloader::State ByteRangeDownloaderConnectionDownState::state() const
{
    return ByteRangeDownloader::CONNECTION_DOWN_STATE;
}

/** Return the state name. */
const char* ByteRangeDownloaderConnectionDownState::name() const
{
    return "Connection Down State";
}

/** Enter the state. */
void ByteRangeDownloaderConnectionDownState::enter(
                                                ByteRangeDownloader& byteRangeDownloader,
                                                const AseTimeVal& timeNow )
{
    ByteRangeDownloaderBaseState::enter( byteRangeDownloader, timeNow );

    byteRangeDownloader.clearHttpRequests( timeNow );

    byteRangeDownloader.closeConnection( timeNow, byteRangeDownloader.mConnectionFailureCode );

    byteRangeDownloader.mConnectionDataReceivedTime = AseTimeVal::INFINITE;
}

/** Exit the state. */
void ByteRangeDownloaderConnectionDownState::exit(
                                                ByteRangeDownloader& byteRangeDownloader,
                                                const AseTimeVal& timeNow )
{
    ByteRangeDownloaderBaseState::exit( byteRangeDownloader, timeNow );

    byteRangeDownloader.mFailedToConnectFlag = false;
    byteRangeDownloader.mFastConnectionFailure = false;
}

}}

