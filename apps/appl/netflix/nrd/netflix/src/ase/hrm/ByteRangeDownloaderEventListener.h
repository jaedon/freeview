/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BYTE_RANGE_DOWNLOADER_EVENT_LISTENER_H
#define BYTE_RANGE_DOWNLOADER_EVENT_LISTENER_H

#include <string>

#include <nrdnet/AseTimeVal.h>
#include <nrdnet/NetCommonDataTypes.h>
#include "HttpAttempt.h"
#include "ByteRangeDownloader.h"

namespace netflix {
namespace ase {

/**
 *  @class ByteRangeDownloaderEventListener
 *
 *  @brief This class is the byte range downloader event listener class.
 */
class ByteRangeDownloaderEventListener : public enable_shared_from_this<ByteRangeDownloaderEventListener>
{
public:
    typedef shared_ptr<ByteRangeDownloaderEventListener> ByteRangeDownloaderEventListenerPtr;

public:
    virtual ~ByteRangeDownloaderEventListener(){}

    virtual void reportTcpConnect(
                            uint32_t byteRangeDownloaderId,
                            uint32_t connectionId,
                            uint32_t httpId,
                            const std::string& clientIpAddress,
                            uint16_t clientPort,
                            const std::string& serverHostName,
                            const std::string& serverIpAddress,
                            uint16_t serverPort,
                            const AseTimeVal& dnsLookupTime,
                            const AseTimeVal& connectTime ) = 0;

    virtual void reportTcpFailure(
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
                            const AseTimeVal& failedTime ) = 0;

    virtual void reportTcpDisconnect(
                            uint32_t byteRangeDownloaderId,
                            uint32_t connectionId,
                            uint32_t httpId,
                            int32_t statusCode,
                            int32_t lowLevelCode,
                            TcpDisconnectState disconnectState,
                            bool midResponse ) = 0;

    virtual void reportBytesReceived(
                            uint32_t byteRangeDownloaderId,
                            const AseTimeVal& receivedTime,
                            uint32_t byteCount,
                            bool drainingSocketReceiveBuffer,
                            AseTimeVal const& earliestTime ) = 0;

    virtual void reportDownloadingStarted(
                            uint32_t byteRangeDownloaderId ) = 0;

    virtual void reportDownloadingStopped(
                            uint32_t byteRangeDownloaderId ) = 0;

    virtual void reportHttpResponseTime(
                            uint32_t byteRangeDownloaderId,
                            const AseTimeVal& httpResponseTime ) = 0;

    virtual void reportDownloadRecord(
                            uint32_t byteRangeDownloaderId,
                            const HttpAttempt& httpAttempt ) = 0;

    virtual void reportDataReceived(
                            uint32_t byteRangeDownloaderId ) = 0;

};

typedef ByteRangeDownloaderEventListener::ByteRangeDownloaderEventListenerPtr
                                                    ByteRangeDownloaderEventListenerPtr;

}}
#endif // BYTE_RANGE_DOWNLOADER_EVENT_LISTENER_H
