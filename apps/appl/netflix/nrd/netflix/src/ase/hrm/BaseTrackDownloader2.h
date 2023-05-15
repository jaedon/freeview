/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BASE_TRACK_DOWNLOADER_2_H
#define BASE_TRACK_DOWNLOADER_2_H

#include <nrdbase/StdInt.h>
#include <vector>

#include <nrdnet/AseUrl.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdnet/AseTimeVal.h>
#include "AseCommonDataTypes.h"
#include "ByteRange.h"
#include "IAseConfigParameter.h"
#include "ITransportReporter.h"
#include <nrdnet/IAsyncHttpLib.h>
#include "IThroughputMonitor.h"
#include "UrlRedirectMap.h"

#include "ByteRangeDownloaderEventListener.h"

namespace netflix {
namespace ase {

class HttpRequestManager;
typedef HttpRequestManager* HttpRequestManagerPtr;

/**
 *  @class BaseTrackDownloader2
 *
 *  @brief This class is the base for track downloader classes.
 */
class BaseTrackDownloader2 : public ByteRangeDownloaderEventListener
{
public:
    typedef shared_ptr<BaseTrackDownloader2> BaseTrackDownloader2Ptr;

    typedef enum
    {
        SCOR_TYPE = 0,
        MCPL_TYPE,
        MSPL_TYPE
    } Type;

public:
    /** Constructor. */
    BaseTrackDownloader2(
                    uint32_t trackId,
                    Type type,
                    MediaType mediaType,
                    TrackConfiguration const& config,
                    HttpRequestManagerPtr pHttpRequestManager,
                    IAsyncHttpClientPtr pAsyncHttpClient,
                    ITransportReporterPtr pTransportReporter,
                    UrlRedirectMapPtr pUrlRedirectMap );

    /** Destructor. */
    virtual ~BaseTrackDownloader2();

    /** Get the track id */
    uint32_t getTrackId() const
    {
        return mTrackId;
    }

    /** Get the track downloader type. */
    Type getType() const
    {
        return mType;
    }

    /** Get the media type */
    MediaType getMediaType() const
    {
        return mMediaType;
    }

    /** Get the TrackConfiguration */
    TrackConfiguration const& getTrackConfiguration() const
    {
        return mTrackConfiguration;
    }

    /** Re-configure the track downloader */
    virtual int32_t configure( IAseConfigParameterPtr   pAseConfigParameter,
                               TrackConfiguration const&     config );

    /** Init the track downloader. */
    virtual int32_t init();

    /** Deinitialize the track downloader. */
    virtual int32_t deinit();

    /** Test whether this downloader is in idle state */
    virtual bool idle() const = 0;

    /** Get the destination host/port that this downloader is working on */
    virtual AseUrl::HostPort getDestination() const = 0;

    /** Set the http connection timeouts */
    virtual void setTimeouts(
                        uint32_t httpConnectTimeout, uint32_t httpStallTimeout);

    /** Get the next timeout time. */
    virtual AseTimeVal getNextTimeout() = 0;

    /** Process selected connection. */
    virtual int32_t processSelectedHttpConnections(
                        const AseTimeVal& timeNow,
                        std::vector<IAsyncHttpConnectionPtr>& selectedHttpConnections,
                        bool& dataReceived,
                        bool& networkErrors ) = 0;

    /** Process timeout. */
    virtual int32_t processTimeout( const AseTimeVal& timeNow ) = 0;

    /** Pause downloading. */
    virtual int32_t pauseDownloading( const AseTimeVal& timeNow );

    /** Resume downloading. */
    virtual int32_t resumeDownloading( const AseTimeVal& timeNow );

    /** Check for URL changes on the frontmost task */
    virtual bool checkForUrlChanges() = 0;

    /** Get the downloading paused flag. */
    virtual bool getDownloadingPausedFlag() const;

    /** Get downloading byte range downloader count. */
    virtual uint32_t getDownloadingByteRangDownloaderCount() const;

    /** Notify the downloader that a new task is waiting */
    virtual void notifyNewTaskIsWaiting() = 0;

    /** ByteRangeDownloaderEventListener functions. */

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
                            const AseTimeVal& connectTime );

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
                            const AseTimeVal& failedTime );

    virtual void reportTcpDisconnect(
                            uint32_t byteRangeDownloaderId,
                            uint32_t connectionId,
                            uint32_t httpId,
                            int32_t statusCode,
                            int32_t lowLevelCode,
                            TcpDisconnectState disconnectState,
                            bool midResponse );

    virtual void reportBytesReceived(
                            uint32_t byteRangeDownloaderId,
                            const AseTimeVal& receivedTime,
                            uint32_t byteCount,
                            bool drainingSocketReceiveBuffer,
                            AseTimeVal const& earliestTime );

    virtual void reportDownloadingStarted(
                            uint32_t byteRangeDownloaderId );

    virtual void reportDownloadingStopped(
                            uint32_t byteRangeDownloaderId );

    virtual void reportHttpResponseTime(
                            uint32_t byteRangeDownloaderId,
                            const AseTimeVal& httpResponseTime );

    virtual void reportDownloadRecord(
                            uint32_t byteRangeDownloaderId,
                            const HttpAttempt& httpAttempt );

    virtual void reportDataReceived(
                            uint32_t byteRangeDownloaderId );

protected:
    /** Add an URL redirect entry.
     *
     *  @param originalUrl [In] The original URL.
     *  @param destinationUrl [In] The URL redirect destination.
     *  @return True if the entry could be added, false if too many redirects
     */
    bool addUrlRedirectEntry(
                const AseUrl& originalUrl,
                const AseUrl& destinationUrl );

    /** Remove an URL redirect entry.
     *
     *  @param originalUrl [In] The original URL.
     */
    void removeUrlRedirectEntry( const AseUrl& originalUrl );

    /** Check if an URL is redirected.
     *
     *  @param originalUrl [In] The original URL.
     */
    bool isUrlRedirected( const AseUrl& url ) const;

    /** Get the  URL redirect destination for a given original. if there is no URL redirect,
     *  the original URL is returned.
     *
     *  @param originalUrl [In] The original URL.
     */
    AseUrl getUrlRedirectDestination(
                    const AseUrl& originalUrl ) const;

    /** Add a URL failure entry
     *
     *  @param url [in] The failed URL
     *  @param status [in] The failure status
     */
    void addUrlFailureEntry(
                    const AseUrl& url,
                    uint32_t status );

    /** Remove a URL failure entry
     *  @param url [in] The now-working-again URL
     */
    void removeUrlFailureEntry( const AseUrl& url );

    /** Check is a URL is failed */
    int32_t getUrlFailureStatus(
                    const AseUrl& url,
                    uint32_t& status ) const;

    /** Set the throughput monitor */
    void setThroughputMonitor( IThroughputMonitorPtr pThroughputMonitor );

protected:

    /** The track ID. */
    uint32_t mTrackId;

    /** The track downloader type. */
    Type mType;

    /* The media type */
    MediaType mMediaType;

    /* The track TrackConfiguration */
    TrackConfiguration mTrackConfiguration;

    /** The pointer to the HTTP request manager. */
    HttpRequestManagerPtr mHttpRequestManagerPtr;

    /** The async HTTP library. */
    IAsyncHttpClientPtr mAsyncHttpClientPtr;

    /** The pointer to the transport reporter. */
    ITransportReporterPtr mTransportReporterPtr;

    /** The pointer to the TrackConfiguration parameter. */
    IAseConfigParameterPtr mAseConfigParameterPtr;

    /** The throughput monitor. */
    IThroughputMonitorPtr mThroughputMonitorPtr;

    /** The URL redirects map. */
    UrlRedirectMapPtr mUrlRedirectMapPtr;

    /** The paused flag. */
    bool mDownloadingPausedFlag;

    /** The downloading byte range downloader count. */
    uint32_t mDownloadingByteRangDownloaderCount;

    /** The flag indicates bytes have been received. */
    bool mBytesReceivedFlag;

    AseTimeVal mHttpConnectTimeout;
    AseTimeVal mHttpStallTimeout;

    /** TrackConfiguration parameters. */
    uint32_t mMediaBufferCheckInterval;
    uint32_t mMediaBufferTargetAvailableSize;
    uint32_t mDomainReconnectBackoffTime;
    uint32_t mSyncDomainReconnectBackoffTime;
    uint32_t mMaxSyncConnectFailures;
    uint32_t mTransportReportInterval;
    uint32_t mDownloadReportInterval;
    uint32_t mUrlReqeustGetInterval;
    ullong mInitialThroughput;
    uint32_t mThroughputFilterHalfLife;
    uint32_t mThroughputMeasurementInterval;
    AseTimeVal mIdleReconnectWindow;
    AseTimeVal mIdleDisconnectTime;
    int mProcess2xxLoopSleepMicros;
};

typedef BaseTrackDownloader2::BaseTrackDownloader2Ptr BaseTrackDownloader2Ptr;

/** Add an URL redirect entry. */
inline bool BaseTrackDownloader2::addUrlRedirectEntry(
                                    const AseUrl& originalUrl,
                                    const AseUrl& destinationUrl )
{
    return mUrlRedirectMapPtr->addRedirect( originalUrl, destinationUrl );
}

/** Remove an URL redirect entry. */
inline void BaseTrackDownloader2::removeUrlRedirectEntry( const AseUrl& originalUrl )
{
    mUrlRedirectMapPtr->removeRedirect( originalUrl );
}

/** Get the  URL redirect destination for a given original. */
inline AseUrl BaseTrackDownloader2::getUrlRedirectDestination( const AseUrl& originalUrl ) const
{
    return mUrlRedirectMapPtr->getRedirect( originalUrl );
}

/** Check if an URL is redirected. */
inline bool BaseTrackDownloader2::isUrlRedirected( const AseUrl& url ) const
{
    return mUrlRedirectMapPtr->isRedirected( url );
}

/** Add a URL failure entry */
inline void BaseTrackDownloader2::addUrlFailureEntry(
                                    const AseUrl& url,
                                    uint32_t status )
{
    mUrlRedirectMapPtr->addFailure( url, status );
}

/** Remove a URL failure entry */
inline void BaseTrackDownloader2::removeUrlFailureEntry( const AseUrl& url )
{
    mUrlRedirectMapPtr->removeFailure( url );
}

/** Check is a URL is failed */
inline int32_t BaseTrackDownloader2::getUrlFailureStatus(
                                    const AseUrl& url,
                                    uint32_t& status ) const
{
    return mUrlRedirectMapPtr->getFailureStatus( url, status );
}

inline void BaseTrackDownloader2::setThroughputMonitor( IThroughputMonitorPtr pThroughputMonitor )
{
    if ( mThroughputMonitorPtr && mBytesReceivedFlag)
    {
        mThroughputMonitorPtr->indicateReceptionStopped( mMediaType );
    }

    mBytesReceivedFlag = false;

    mThroughputMonitorPtr = pThroughputMonitor;
}

}}
#endif //BASE_TRACK_DOWNLOADER_2_H
