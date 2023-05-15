/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Log.h>
#include <nrdbase/AseErrorCodes.h>
#include "StreamersLog.h"


namespace netflix {
namespace ase {


StreamersLog::StreamersLog() : mPrintHeader( true )
{
    mLogStartTime = AseTimeVal::now();
}

/** Destructor. */
StreamersLog::~StreamersLog()
{
}

void StreamersLog::addTcpConnectRecord(
                                    uint32_t trackId,
                                    uint32_t channelId,
                                    uint32_t connectionId,
                                    const char* pHostName,
                                    const char* pIpAddress,
                                    uint16_t portNumber,
                                    const AseTimeVal& rtt )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu TcpConnect %u %u %u %s %s %d %llu",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        channelId,
        connectionId,
        pHostName,
        pIpAddress,
        portNumber,
        rtt.ms() );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(channelId);
    NRDP_UNUSED(connectionId);
    NRDP_UNUSED(pHostName);
    NRDP_UNUSED(pIpAddress);
    NRDP_UNUSED(portNumber);
    NRDP_UNUSED(rtt);
#endif
}

void StreamersLog::addTcpDisconnectRecord(
                            uint32_t trackId,
                            uint32_t channelId,
                            uint32_t connectionId,
                            const char* pHostName,
                            const char* pIpAddress,
                            uint16_t portNumber,
                            int32_t connectionFailureCode,
                            int32_t lowLevelFailureCode,
                            const char* pDisconnectState,
                            const char* pMidResponse,
                            uint32_t totalIssuedRequestCount,
                            uint32_t totalCompletedRequestCount,
                            ullong totalRequestedByteCount,
                            ullong totalReceivedByteCount )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    const char* pConnectionFailureString;

    switch (connectionFailureCode)
    {
        case AS_NO_ERROR:
            pConnectionFailureString = "NoError";
            break;

        case AS_CONNECTION_ERROR:
            pConnectionFailureString = "ConnectionError";
            break;

        case AS_CONNECTION_TIMEOUT:
            pConnectionFailureString = "ConnectionTimeout";
            break;

        case AS_CONNECTION_RESET:
            pConnectionFailureString = "ConnectionReset";
            break;

        case AS_CONNECTION_CLOSED:
            pConnectionFailureString = "ConnectionClosed";
            break;

        default:
            pConnectionFailureString = "Other";
            break;
    }

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu TcpDisconnect %u %u %u %s %s %d %d %s %d %s %s %u %u %llu %llu",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        channelId,
        connectionId,
        pHostName,
        pIpAddress,
        portNumber,
        connectionFailureCode,
        pConnectionFailureString,
        lowLevelFailureCode,
        pDisconnectState,
        pMidResponse,
        totalIssuedRequestCount,
        totalCompletedRequestCount,
        totalRequestedByteCount,
        totalReceivedByteCount );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(channelId);
    NRDP_UNUSED(connectionId);
    NRDP_UNUSED(pHostName);
    NRDP_UNUSED(pIpAddress);
    NRDP_UNUSED(portNumber);
    NRDP_UNUSED(connectionFailureCode);
    NRDP_UNUSED(lowLevelFailureCode);
    NRDP_UNUSED(pDisconnectState);
    NRDP_UNUSED(pMidResponse);
    NRDP_UNUSED(totalIssuedRequestCount);
    NRDP_UNUSED(totalCompletedRequestCount);
    NRDP_UNUSED(totalRequestedByteCount);
    NRDP_UNUSED(totalReceivedByteCount);
#endif
}

void StreamersLog::addHttpRequestRecord(
                                uint32_t trackId,
                                uint32_t channelId,
                                uint32_t connectionId,
                                MediaType mediaType,
                                const char* streamId,
                                const char* pUrl,
                                const ByteRange& byteRange )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu HttpRequest %u %u %u %s %s %s %lld %lld %lld",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        channelId,
        connectionId,
        getMediaTypeString( mediaType ),
        streamId,
        pUrl,
        byteRange.getStart(),
        byteRange.getEnd(),
        byteRange.getSize() );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(channelId);
    NRDP_UNUSED(connectionId);
    NRDP_UNUSED(mediaType);
    NRDP_UNUSED(streamId);
    NRDP_UNUSED(pUrl);
    NRDP_UNUSED(byteRange);
#endif
}

void StreamersLog::addHttpResponseRecord(
                                uint32_t trackId,
                                uint32_t channelId,
                                uint32_t connectionId,
                                MediaType mediaType,
                                const char* streamId,
                                const char* pUrl,
                                int32_t responseCode )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu HttpResponse %u %u %u %s %s %s %d",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        channelId,
        connectionId,
        getMediaTypeString( mediaType ),
        streamId,
        pUrl,
        responseCode );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(channelId);
    NRDP_UNUSED(connectionId);
    NRDP_UNUSED(mediaType);
    NRDP_UNUSED(streamId);
    NRDP_UNUSED(pUrl);
    NRDP_UNUSED(responseCode);
#endif
}

void StreamersLog::addHttpDataRecord(
                                uint32_t trackId,
                                uint32_t channelId,
                                uint32_t connectionId,
                                MediaType mediaType,
                                const char* streamId,
                                int32_t streamBitRate,
                                const ByteRange& byteRange,
                                ullong averageThroughput0,
                                ullong averageThroughput1,
                                ullong averageThroughput2 )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu HttpData %u %u %u %s %s %d %lld %lld %lld %llu %llu %llu",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        channelId,
        connectionId,
        getMediaTypeString( mediaType ),
        streamId,
        streamBitRate,
        byteRange.getStart(),
        byteRange.getEnd(),
        byteRange.getSize(),
        averageThroughput0,
        averageThroughput1,
        averageThroughput2 );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(channelId);
    NRDP_UNUSED(connectionId);
    NRDP_UNUSED(mediaType);
    NRDP_UNUSED(streamId);
    NRDP_UNUSED(streamBitRate);
    NRDP_UNUSED(byteRange);
    NRDP_UNUSED(averageThroughput0);
    NRDP_UNUSED(averageThroughput1);
    NRDP_UNUSED(averageThroughput2);
#endif
}

void StreamersLog::addSchedulingBucketRecord(
                                uint32_t trackId,
                                const char* trackType,
                                uint32_t channelId,
                                uint32_t connectionId,
                                ullong outstandingByteCount,
                                ullong byteCountToTarget,
                                ullong averageThroughput,
                                ullong responseTime )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu SchedulingBucket %u %s %u %u %llu %llu %llu %llu",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        trackType,
        channelId,
        connectionId,
        outstandingByteCount,
        byteCountToTarget,
        averageThroughput,
        responseTime );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(trackType);
    NRDP_UNUSED(channelId);
    NRDP_UNUSED(connectionId);
    NRDP_UNUSED(outstandingByteCount);
    NRDP_UNUSED(byteCountToTarget);
    NRDP_UNUSED(averageThroughput);
    NRDP_UNUSED(responseTime);
#endif
}

void StreamersLog::addResponseTimeRecord(
                            uint32_t trackId,
                            const char* trackType,
                            uint32_t channelId,
                            uint32_t connectionId,
                            const AseTimeVal& responseTime )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu ResponseTime %u %s %u %u %llu",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        trackType,
        channelId,
        connectionId,
        responseTime.ms() );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(trackType);
    NRDP_UNUSED(channelId);
    NRDP_UNUSED(connectionId);
    NRDP_UNUSED(responseTime);
#endif
}

void StreamersLog::addUrlRequestTaskRecord(
                                uint32_t trackId,
                                const char* streamId,
                                const char* url,
                                const ByteRange& byteRange,
                                ullong totalSize )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu UrlRequestTask %u %s %s %lld %lld %lld %lld",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        streamId,
        url,
        byteRange.getStart(),
        byteRange.getEnd(),
        byteRange.getSize(),
        totalSize );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(streamId);
    NRDP_UNUSED(url);
    NRDP_UNUSED(byteRange);
    NRDP_UNUSED(totalSize);
#endif
}

void StreamersLog::addMediaRequestRecord(
                            uint32_t trackId,
                            const ByteRange& byteRange )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu MediaRequest %u %lld %lld %lld",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        byteRange.getStart(),
        byteRange.getEnd(),
        byteRange.getSize() );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(byteRange);
#endif
}

void StreamersLog::addMediaBufferAvailabilityRecord(
                            uint32_t trackId,
                            bool mediaBufferAvailable )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu MediaBufferAvailability %u %s",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        mediaBufferAvailable ? "True" : "False" );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(mediaBufferAvailable);
#endif
}

void StreamersLog::addStreamSwitchRecord(
                                uint32_t trackId,
                                int streamId,
                                uint32_t oldRate,
                                uint32_t newRate )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu StreamSwitch %u %d %u %u",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        streamId,
        oldRate,
        newRate );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(streamId);
    NRDP_UNUSED(oldRate);
    NRDP_UNUSED(newRate);
#endif
}

void StreamersLog::addUrlRedirectRecord(
                            uint32_t trackId,
                            const char* streamId,
                            const AseUrl& originalUrl,
                            const AseUrl& redirectUrl )
{
#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)
    printHeader();

    NTRACE(
        TRACE_STREAMERSLOG,
        "%llu UrlRedirect %u %s %s %s",
        (AseTimeVal::now() - mLogStartTime).ms(),
        trackId,
        streamId,
        originalUrl.str().c_str(),
        redirectUrl.str().c_str() );
#else
    NRDP_UNUSED(trackId);
    NRDP_UNUSED(streamId);
    NRDP_UNUSED(originalUrl);
    NRDP_UNUSED(redirectUrl);
#endif
}

void StreamersLog::printHeader()
{
    if (mPrintHeader)
    {
        NTRACE(
            TRACE_STREAMERSLOG,
            "#Start time: %llu",
            mLogStartTime.ms() );

        NTRACE(
            TRACE_STREAMERSLOG,
            "#TimeStamp TcpConnect TrackId ChannelId ConnectionId "
            "ServerHostName ServerIP ServerPort RTT (ms)" );

        NTRACE(
            TRACE_STREAMERSLOG,
           "#TimeStamp TcpDisconnect TrackId ChannelId ConnectionId "
           "ServerHostName ServerIP ServerPort ConnectionFailureCode ConnectionFailureDescription "
           "LowLevelFailureCode DisconnectState MidResponse TotalIssuedRequestCount "
           "TotalCompletedRequestCount TotalRequestedByteCount TotalReceivedByteCount" );

        NTRACE(
            TRACE_STREAMERSLOG,
            "#TimeStamp HttpRequest TrackId ChannelId ConnectionId MediaType StreamKey URL ByteRangeStart ByteRangeEnd ByteRangeSize" );

        NTRACE(
            TRACE_STREAMERSLOG,
             "#TimeStamp HttpResponse TrackId ChannelId ConnectionId MediaType StreamKey URL ResponseCode" );

        NTRACE(
            TRACE_STREAMERSLOG,
            "#TimeStamp HttpData TrackId ChannelId ConnectionId MediaType StreamKey StreamBitRate ByteRangeStart ByteRangeEnd "
            "ByteRangeSize AverageThroughput0 AverageThroughput1 AverageThroughput2" );

        NTRACE(
            TRACE_STREAMERSLOG,
            "#TimeStamp SchedulingBucket TrackId TrackType ChannelId ConnectionId OutstandingByteCount ByteCountToTarget "
            "AverageThroughput ResponseTime(ms)" );

        NTRACE(
            TRACE_STREAMERSLOG,
            "#TimeStamp ResponseTime TrackId TrackType ChannelId ConnectionId ResponseTime (ms)" );

        NTRACE(
            TRACE_STREAMERSLOG,
            "#TimeStamp UrlRequestTask TrackId StreamKey Url FirstByteRangeStart FirstByteRangeEnd FirstByteRangeSize TotalRequestSize" );

        NTRACE(
            TRACE_STREAMERSLOG,
            "#TimeStamp MediaRequest TrackId ByteRangeStart ByteRangeEnd ByteRangeSize" );

        NTRACE(
            TRACE_STREAMERSLOG,
            "#TimeStamp MediaBufferAvailability TrackId [True|False]" );

        NTRACE(
            TRACE_STREAMERSLOG,
            "#TimeStamp StreamSwitch TrackId StreamId OldStreamRate NewStreamRate" );

        NTRACE(
            TRACE_STREAMERSLOG,
            "#TimeStamp UrlRedirect TrackId StreamKey OriginalUrl RedirectUrl" );

        mPrintHeader = false;
    }
}

const char* StreamersLog::getMediaTypeString( MediaType mediaType )
{
    const char* pRetVal = NULL;

#if ENABLE_STREAMERS_LOG && defined(BUILD_DEBUG)

    switch (mediaType)
    {
        case MEDIA_UNKNOWN:
            pRetVal = "Unknow";
            break;

        case MEDIA_AUDIO:
            pRetVal = "Audio";
            break;

        case MEDIA_VIDEO:
            pRetVal = "Video";
            break;

        case MEDIA_TEXT:
            pRetVal = "Text";
            break;
    }
#else
    NRDP_UNUSED(mediaType);
#endif

    return pRetVal;
}

#if ENABLE_STREAMERS_LOG
StreamersLog gStreamersLog;
#endif

}}
