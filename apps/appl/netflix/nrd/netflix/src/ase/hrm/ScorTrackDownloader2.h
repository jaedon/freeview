/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SCOR_TRACK_DOWNLOADER_2_H
#define SCOR_TRACK_DOWNLOADER_2_H

#include "BaseTrackDownloader2.h"
#include "ByteRangeDownloader.h"

namespace netflix {
namespace ase {

/**
 *  @class ScorTrackDownloader2
 *
 *  @brief This class is the the single connection open range track downloader class.
 */
class ScorTrackDownloader2 : public BaseTrackDownloader2
{
public:
    typedef shared_ptr<ScorTrackDownloader2> ScorTrackDownloader2Ptr;

public:
    /** Constructor. */
    ScorTrackDownloader2(
                    uint32_t trackId,
                    MediaType mediaType,
                    TrackConfiguration const& config,
                    HttpRequestManagerPtr pHttpRequestManager,
                    IAsyncHttpClientPtr pAsyncHttpClient,
                    ITransportReporterPtr pTransportReporter,
                    IAseConfigParameterPtr pAseConfigParameter,
                    UrlRedirectMapPtr pUrlRedirectMap );

    /** Destructor. */
    virtual ~ScorTrackDownloader2();

    /** Init the track downloader. */
    virtual int32_t init();

    /** Re-configure the track downloader */
    virtual int32_t configure( IAseConfigParameterPtr pAseConfigParameter,
                               TrackConfiguration const&  config );

    /** Deinitialize the track downloader. */
    virtual int32_t deinit();

    /** Test whether this downloader is in initializing state */
    virtual bool idle() const;

    /** Get the destination host/port that this downloader is working on */
    AseUrl::HostPort getDestination() const;

    /** Get the next timeout time. */
    virtual AseTimeVal getNextTimeout();

    /** Process selected connection. */
    virtual int32_t processSelectedHttpConnections(
                        const AseTimeVal& timeNow,
                        std::vector<IAsyncHttpConnectionPtr>& selectedHttpConnections,
                        bool& dataReceived,
                        bool& networkErrors );

    /** Process timeout. */
    virtual int32_t processTimeout( const AseTimeVal& timeNow );

    /** Pause downloading. */
    virtual int32_t pauseDownloading( const AseTimeVal& timeNow );

    /** Resume downloading. */
    virtual int32_t resumeDownloading( const AseTimeVal& timeNow );

    virtual bool checkForUrlChanges();

    /** Notify the downloader that a new task is waiting */
    virtual void notifyNewTaskIsWaiting();

protected:
    /** Single connection open range track downloader states. */
    typedef enum
    {
        UNINITIALIZED_STATE,
        WAITING_FOR_URL_REQUEST_STATE,  //Waiting for URL requests.
        WORKING_STATE,                  //Working state, receiving bytes for URL requests.
        WAITING_FOR_MEDIA_BUFFER_STATE  //Waiting for media buffer.
    } State;

protected:
    /** Transit to a new state */
    void transitState( const AseTimeVal& timeNow, State newState );

    /** Configure the byte range downloader */
    void configureByteRangeDownloader();

     /** Process a HTTP connection. */
    void processHttpConnection( const AseTimeVal& timeNow, bool& networkErrors );

    /** Process a HTTP response. */
    void processHttpResponse( const AseTimeVal& timeNow, bool& dataReceived, bool& networkErrors );

    /** Return all URL request tasks. */
    void returnAllUrlRequestTasks();

    /** Report an error on URL request tasks */
    void reportErrorToUrlRequestTasks(  AseUrl const&   url,
                                        AseErrorCode    status,
                                        uint32_t        httpCode );

    /** Get the name of a state. */
    const char* getStateName( State state ) const;

protected:
    /** The state. */
    State mState;

    /** The time at which we entered the current state */
    AseTimeVal mStateEnterTime;

    //The last receiving URL. This is always the original URL, not redirect URL.
    AseUrl mLastReceivingUrl;

    //The last receiving byte ragne.
    ByteRange mLastReceivingByteRange;

    //The URL request task.
    UrlRequestTaskList mUrlRequestTaskList;

    /** The byte range downloader. */
    ByteRangeDownloaderPtr mByteRangeDownloaderPtr;

    /** Indicates whether the last connection attempt succeeded */
    bool mLastConnectionSuccessful;

    /** Multi-purpose timers */
    AseTimeVal mTimerOne;

    /** Indicates whether the connection was closed in WAITING_FOR_MEDIA_BUFFER state */
    bool mDisconnectedWhileIdle;
};

typedef ScorTrackDownloader2::ScorTrackDownloader2Ptr ScorTrackDownloader2Ptr;

}}
#endif //SCOR_TRACK_DOWNLOADER_2_H
