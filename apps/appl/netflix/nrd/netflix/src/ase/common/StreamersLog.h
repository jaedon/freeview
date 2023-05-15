/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STREAMERS_LOG_H
#define STREAMERS_LOG_H

#include <nrdbase/StdInt.h>
#include <stdio.h>
#include <map>

#include <nrdnet/AseUrl.h>

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/ByteRange.h>
#include <nrdnet/AseTimeVal.h>

#define ENABLE_STREAMERS_LOG 0

namespace netflix {
namespace ase {


/**
 *  @class StreamersLog
 *
 *  @brief This class is the streamer's log class.
 */
class StreamersLog
{
public:
    StreamersLog();

    virtual ~StreamersLog();

    void addTcpConnectRecord(
                uint32_t trackId,
                uint32_t channelId,
                uint32_t connectionId,
                const char* pHostName,
                const char* pIpAddress,
                uint16_t portNumber,
                const AseTimeVal& rtt );

    void addTcpDisconnectRecord(
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
                ullong totalReceivedByteCount );

    void addHttpRequestRecord(
                uint32_t trackId,
                uint32_t channelId,
                uint32_t connectionId,
                MediaType mediaType,
                const char* streamId,
                const char* pUrl,
                const ByteRange& byteRange );

    void addHttpResponseRecord(
                uint32_t trackId,
                uint32_t channelId,
                uint32_t connectionId,
                MediaType mediaType,
                const char* streamId,
                const char* pUrl,
                int32_t responseCode );

    void addHttpDataRecord(
                uint32_t trackId,
                uint32_t channelId,
                uint32_t connectionId,
                MediaType mediaType,
                const char* streamId,
                int32_t streamBitRate,
                const ByteRange& byteRange,
                ullong averageThroughput0,
                ullong averageThroughput1,
                ullong averageThroughput2 );

    void addSchedulingBucketRecord(
                uint32_t trackId,
                const char* trackType,
                uint32_t channelId,
                uint32_t connectionId,
                ullong outstandingByteCount,
                ullong byteCountToTarget,
                ullong averageThroughput,
                ullong responseTime );

    void addResponseTimeRecord(
                uint32_t trackId,
                const char* trackType,
                uint32_t channelId,
                uint32_t connectionId,
                const AseTimeVal& responseTime );

    void addUrlRequestTaskRecord(
                uint32_t trackId,
                const char* streamId,
                const char* url,
                const ByteRange& byteRange,
                ullong totalSize );

    void addMediaRequestRecord(
                uint32_t trackId,
                const ByteRange& byteRange );

    void addMediaBufferAvailabilityRecord(
                uint32_t trackId,
                bool mediaBufferAvailable );

    void addStreamSwitchRecord(
                uint32_t trackId,
                int streamId,
                uint32_t oldRate,
                uint32_t newRate );

    void addUrlRedirectRecord(
                uint32_t trackId,
                const char* streamId,
                const AseUrl& originalUrl,
                const AseUrl& redirectUrl );

    const char* getMediaTypeString( MediaType mediaType );

    void printHeader();

protected:
    FILE* mLogFile;

    AseTimeVal mLogStartTime;

    bool mPrintHeader;
};

#if ENABLE_STREAMERS_LOG
extern StreamersLog gStreamersLog;
#endif

}}
#endif //STREAMERS_LOG_H
