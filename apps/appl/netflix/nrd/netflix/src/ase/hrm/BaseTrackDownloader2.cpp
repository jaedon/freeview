/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DebugLogging.h"
#include "HttpRequestManager.h"
#include "IAseConfigParameter.h"
#include "ITransportReporter.h"
#include "BaseTrackDownloader2.h"


namespace netflix {
namespace ase {

DEFINE_LOGGER( BaseTrackDownloader );

/** Constructor. */
BaseTrackDownloader2::BaseTrackDownloader2(
                            uint32_t trackId,
                            Type type,
                            MediaType mediaType,
                            TrackConfiguration const& config,
                            HttpRequestManagerPtr pHttpRequestManager,
                            IAsyncHttpClientPtr pAsyncHttpClient,
                            ITransportReporterPtr pTransportReporter,
                            UrlRedirectMapPtr pUrlRedirectMap )
                                    : mTrackId( trackId ),
                                      mType( type ),
                                      mMediaType( mediaType ),
                                      mTrackConfiguration( config ),
                                      mHttpRequestManagerPtr( pHttpRequestManager ),
                                      mAsyncHttpClientPtr( pAsyncHttpClient ),
                                      mTransportReporterPtr( pTransportReporter ),
                                      mUrlRedirectMapPtr( pUrlRedirectMap ),
                                      mDownloadingPausedFlag( false ),
                                      mDownloadingByteRangDownloaderCount( 0 ),
                                      mBytesReceivedFlag( false ),
                                      mHttpConnectTimeout(AseTimeVal::ZERO),
                                      mHttpStallTimeout(AseTimeVal::ZERO),
                                      mProcess2xxLoopSleepMicros( -1 )
{
}

int32_t BaseTrackDownloader2::configure(    IAseConfigParameterPtr pAseConfigParameter,
                                            TrackConfiguration const& config )
{
    mAseConfigParameterPtr = pAseConfigParameter;

    mTrackConfiguration = config;

    mMediaBufferCheckInterval = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "mediaBufferCheckInterval",
                                                            200 );

    mMediaBufferTargetAvailableSize = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "mediaBufferTargetAvailableSize",
                                                            1024 * 1024 );

    mDomainReconnectBackoffTime = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "domainReconnectBackoffTime",
                                                            200 );

    mSyncDomainReconnectBackoffTime = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "syncDomainReconnectBackoffTime",
                                                            2000 );

    mMaxSyncConnectFailures = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "maxSyncConnectFailures",
                                                            4 );

    mTransportReportInterval = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "transportReportInterval",
                                                            1000, 60000, 1000 );

    mDownloadReportInterval = mAseConfigParameterPtr->getParameterAs<uint32_t>(
                                                            "downloadReportInterval",
                                                            0, 3600 * 1000, 10000 );

    mUrlReqeustGetInterval = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "urlRequestGetInterval",
                                                            100 );

    mInitialThroughput = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "initialThroughput",
                                                            3000000 );

    mThroughputFilterHalfLife = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "throughputFilterHalfLife",
                                                            1000 );

    mThroughputMeasurementInterval = mAseConfigParameterPtr->getParameterAs<int>(
                                                            "throughputMeasurementInterval",
                                                            100 );

    mIdleReconnectWindow = AseTimeVal::fromMS( mAseConfigParameterPtr->getParameterAs<int>(
                                                    "idleReconnectWindow",
                                                    0, 60 * 60 * 1000, 5000 ) );

    mIdleDisconnectTime = AseTimeVal::fromMS( mAseConfigParameterPtr->getParameterAs<int>(
                                                    "idleDisconnectTime",
                                                    0, 60 * 60 * 1000,
#if defined(NRDP_LIMIT_SOCKET_COUNT)
                                                    5000
#else
                                                    25000
#endif
                                                    ) );

    mProcess2xxLoopSleepMicros = mAseConfigParameterPtr->getParameterAs<int>(
                                                    "process2xxLoopSleepMicros",
                                                    -1 );
    return AS_NO_ERROR;
}

/** Destructor. */
BaseTrackDownloader2::~BaseTrackDownloader2()
{
}

/** Init the track downloader. */
int32_t BaseTrackDownloader2::init()
{
    return AS_NO_ERROR;
}

/** Deinitialize the track downloader. */
int32_t BaseTrackDownloader2:: deinit()
{
    return AS_NO_ERROR;
}

void BaseTrackDownloader2::setTimeouts(
                            uint32_t httpConnectTimeout, uint32_t httpStallTimeout)
{
    mHttpConnectTimeout = AseTimeVal::fromMS(httpConnectTimeout);
    mHttpStallTimeout = AseTimeVal::fromMS(httpStallTimeout);
}


int32_t BaseTrackDownloader2::pauseDownloading( const AseTimeVal& /* timeNow */ )
{
    DEBUGLOG(
        BaseTrackDownloader,
        "%u paused downloading.",
        mTrackId );

    mDownloadingPausedFlag = true;

    return AS_NO_ERROR;
}

/** Resume downloading. */
int32_t BaseTrackDownloader2::resumeDownloading( const AseTimeVal& /* timeNow */ )
{
    DEBUGLOG(
        BaseTrackDownloader,
        "%u resumed downloading.",
        mTrackId );

    mDownloadingPausedFlag = false;

    return AS_NO_ERROR;
}


/** Get the downloading paused flag. */
bool BaseTrackDownloader2::getDownloadingPausedFlag() const
{
    return mDownloadingPausedFlag;
}

/** Get downloading byte range downloader count. */
uint32_t BaseTrackDownloader2::getDownloadingByteRangDownloaderCount() const
{
    return mDownloadingByteRangDownloaderCount;
}

/** ByteRangeDownloaderEventListener functions. */

void BaseTrackDownloader2::reportTcpConnect(
                                uint32_t byteRangeDownloaderId,
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
#ifndef BUILD_DEBUG
    NRDP_UNUSED(byteRangeDownloaderId);
    NRDP_UNUSED(clientPort);
#endif
    DEBUGLOG(
        BaseTrackDownloader,
        "%u (info) TCP connect report: %u %u %u, src: %s:%u, dst: %s %s:%d, dns: %lldms, connect: %lldms.",
        mTrackId,
        byteRangeDownloaderId,
        connectionId,
        httpId,
        clientIpAddress.c_str(),
        clientPort,
        serverHostName.c_str(),
        serverIpAddress.c_str(),
        serverPort,
        (dnsLookupTime == AseTimeVal::INFINITE) ? -1 : (llong) dnsLookupTime.ms(),
        (connectTime == AseTimeVal::INFINITE) ? -1 : (llong) connectTime.ms() );

    if( mThroughputMonitorPtr )
    {
        mThroughputMonitorPtr->addConnectInfo(
                                        connectTime,
                                        serverIpAddress );
    }

    AseUrl::HostPort hostPort;

    hostPort.first = serverHostName;
    hostPort.second = serverPort;

    mTransportReporterPtr->tcpConnect(
                                mTrackId,
                                connectionId,
                                httpId,
                                hostPort,
                                clientIpAddress,
                                serverIpAddress,
                                mMediaType,
                                (int) connectTime.ms(),
                                (int) dnsLookupTime.ms() );

    mHttpRequestManagerPtr->reportAggregateBandwidth();
}

void BaseTrackDownloader2::reportTcpFailure(
                                uint32_t byteRangeDownloaderId,
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
#ifndef BUILD_DEBUG
    NRDP_UNUSED(byteRangeDownloaderId);
    NRDP_UNUSED(clientPort);
#endif
    DEBUGLOG(
        BaseTrackDownloader,
        "%u (info) TCP failure report: %u %u %u, status: %d %d, src: %s:%d, dst: %s %s:%d dns: %lldms, timeout: %lldms.",
        mTrackId,
        byteRangeDownloaderId,
        connectionId,
        httpId,
        statusCode,
        lowLevelCode,
        clientIpAddress.c_str(),
        clientPort,
        serverHostName.c_str(),
        serverIpAddress.c_str(),
        serverPort,
        (dnsLookupTime == AseTimeVal::INFINITE) ? -1 : (llong) dnsLookupTime.ms(),
        (failedTime == AseTimeVal::INFINITE) ? -1 : (llong) failedTime.ms() );

    AseUrl::HostPort hostPort;

    hostPort.first = serverHostName;
    hostPort.second = serverPort;

    //For DNS failures, we don't need to report to transport reporter because it has been
    //reported through DNS trace listener.
    mTransportReporterPtr->tcpFailure(
                            mTrackId,
                            connectionId,
                            httpId,
                            hostPort,
                            clientIpAddress,
                            serverIpAddress,
                            mMediaType,
                            (failedTime == AseTimeVal::INFINITE) ? -1 : (int32_t) failedTime.ms(),
                            (dnsLookupTime == AseTimeVal::INFINITE) ? -1 : (int32_t) dnsLookupTime.ms(),
                            statusCode,
                            lowLevelCode );

    if( mThroughputMonitorPtr )
        mThroughputMonitorPtr->addConnectionFailure();
}

void BaseTrackDownloader2::reportTcpDisconnect(
                                uint32_t byteRangeDownloaderId,
                                uint32_t connectionId,
                                uint32_t httpId,
                                int32_t statusCode,
                                int32_t lowLevelCode,
                                TcpDisconnectState disconnectState,
                                bool midResponse )
{
#ifndef BUILD_DEBUG
    NRDP_UNUSED(byteRangeDownloaderId);
    NRDP_UNUSED(lowLevelCode);
#endif
    DEBUGLOG(
        BaseTrackDownloader,
        "%u (info) TCP disconnect report: %u %u %u, status: %d %d, state: %d %s.",
        mTrackId,
        byteRangeDownloaderId,
        connectionId,
        httpId,
        statusCode,
        lowLevelCode,
        (int32_t) disconnectState,
        midResponse ? "mid-response" : "idle" );

    mTransportReporterPtr->tcpDisconnect(
                                mTrackId,
                                connectionId,
                                httpId,
                                statusCode,
                                disconnectState,
                                midResponse );
}

void BaseTrackDownloader2::reportBytesReceived(
                                uint32_t byteRangeDownloaderId,
                                AseTimeVal const& receivedTime,
                                uint32_t byteCount,
                                bool drainingSocketReceiveBuffer,
                                AseTimeVal const& earliestTime )
{
    NRDP_UNUSED(byteRangeDownloaderId);
#if 0
    DEBUGLOG(
        BaseTrackDownloader,
        "%u received bytes received report: %u %s %llu %u %s.",
        mTrackId,
        byteRangeDownloaderId,
        domainKey.c_str(),
        receivedTime.ms(),
        bytesCount,
        drainingSocketReceiveBuffer ? "True" : "False" );
#endif

    if (byteCount > 0)
    {
        mHttpRequestManagerPtr->reportBytesReceived(
                                    mTrackId,
                                    byteCount,
                                    receivedTime );

        mTransportReporterPtr->addNetworkBytes(byteCount);

        if( mThroughputMonitorPtr )
        {

            mThroughputMonitorPtr->indicateBytesReceived(
                                        mMediaType,
                                        byteCount,
                                        drainingSocketReceiveBuffer,
                                        earliestTime );
        }

        mBytesReceivedFlag = true;
    }
}

void BaseTrackDownloader2::reportDownloadingStarted(
                                uint32_t byteRangeDownloaderId )
{
#ifndef BUILD_DEBUG
    NRDP_UNUSED(byteRangeDownloaderId);
#endif

    mDownloadingByteRangDownloaderCount++;

    if (mDownloadingByteRangDownloaderCount == 1)
    {
        mHttpRequestManagerPtr->reportTrackDownloadingStarted(
                                                mTrackId,
                                                mMediaType );
    }

    DEBUGLOG(
        BaseTrackDownloader,
        "%u received downloading started report: %u %u.",
        mTrackId,
        byteRangeDownloaderId,
        mDownloadingByteRangDownloaderCount );
}

void BaseTrackDownloader2::reportDownloadingStopped(
                                uint32_t byteRangeDownloaderId )
{
#ifndef BUILD_DEBUG
    NRDP_UNUSED(byteRangeDownloaderId);
#endif
    assert( mDownloadingByteRangDownloaderCount > 0 );

    mDownloadingByteRangDownloaderCount--;

    if (mDownloadingByteRangDownloaderCount == 0)
    {
        mHttpRequestManagerPtr->reportTrackDownloadingStopped(
                                                mTrackId,
                                                mMediaType );

        if (mBytesReceivedFlag)
        {
            if( mThroughputMonitorPtr )
            {
                mThroughputMonitorPtr->indicateReceptionStopped(
                                            mMediaType );
            }

            mBytesReceivedFlag = false;
        }
    }

    DEBUGLOG(
        BaseTrackDownloader,
        "%u received downloading stopped report: %u %u.",
        mTrackId,
        byteRangeDownloaderId,
        mDownloadingByteRangDownloaderCount );
}

void BaseTrackDownloader2::reportHttpResponseTime(
                                uint32_t byteRangeDownloaderId,
                                const AseTimeVal& httpResponseTime )
{
#ifndef BUILD_DEBUG
    NRDP_UNUSED(byteRangeDownloaderId);
    NRDP_UNUSED(httpResponseTime);
#endif
    DEBUGLOG(
        BaseTrackDownloader,
        "%u received HTTP response time report: %u %llu.",
        mTrackId,
        byteRangeDownloaderId,
        httpResponseTime.ms() );
}

void BaseTrackDownloader2::reportDownloadRecord(
                                uint32_t byteRangeDownloaderId,
                                const HttpAttempt& httpAttempt )
{
#ifndef BUILD_DEBUG
    NRDP_UNUSED(byteRangeDownloaderId);
#endif
    DEBUGLOG(
        BaseTrackDownloader,
        "%u received download record report from byte range downloader %u, "
        "connection ID: %u, Brecv: %d.",
        mTrackId,
        byteRangeDownloaderId,
        httpAttempt.mTcpId,
        httpAttempt.mBrecv );

    mTransportReporterPtr->downloadRecord( mTrackId, httpAttempt );
}

void BaseTrackDownloader2::reportDataReceived(
                                uint32_t byteRangeDownloaderId )
{
    NRDP_UNUSED(byteRangeDownloaderId);
}

}}
