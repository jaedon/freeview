/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef HTTP_REQUEST_MANAGER_H
#define HTTP_REQUEST_MANAGER_H

#include <nrdbase/StdInt.h>
#include <map>
#include <queue>

#include <nrdbase/Thread.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/NetworkInterfaceInfo.h>

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/ITransportReporter.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdnet/IAsyncHttpLib.h>
//#include <nrdase/ILocationSelector.h>

#include <nrdase/AverageThroughput.h>
#include <nrdase/UrlRequestTask.h>

#include "UrlRedirectMap.h"

namespace netflix {
namespace ase {

class UrlRequestTask;
typedef shared_ptr<UrlRequestTask> UrlRequestTaskPtr;

class UrlRequestManager;
typedef shared_ptr<UrlRequestManager> UrlRequestManagerPtr;

class ByteRangeCache;
typedef shared_ptr<ByteRangeCache> ByteRangeCachePtr;

class ManifestServingLocation;
typedef shared_ptr<ManifestServingLocation> ManifestServingLocationPtr;

class BaseTrackDownloader2;
typedef shared_ptr<BaseTrackDownloader2> BaseTrackDownloader2Ptr;

class HttpPipeliningSupportDetector;
typedef shared_ptr<HttpPipeliningSupportDetector> HttpPipeliningSupportDetectorPtr;

class IPersistentStore;
typedef shared_ptr<IPersistentStore> IPersistentStorePtr;

/**
 * @class
 * @brief
 */
class HttpRequestManager : public IRouteListener,
                           public IDnsTraceListener
{
public:
    typedef HttpRequestManager* HttpRequestManagerPtr;

public:

    /** Returns the singleton instance of the HttpRequestManager */
    static HttpRequestManagerPtr instance();

    /** Destructor. */
    virtual ~HttpRequestManager();

    /*======= Thread functions ========*/

    /** Start the HTTP request manager. */
    int32_t start();

    /** Stop the HTTP request manager. */
    int32_t stop();

    /*======= Commands ========*/

    /** Configure the HTTP request manager
     *
     *  This can be called at any time to re-configure the HttpRequestManager
     *
     *  @param pAseConfigParameter [In] Configuration.
     *  @param pPersistentStore [in] Persistent store object
     *
     *  @return 0 on success or -ve error code
     */
    int32_t configure(  IAseConfigParameterPtr  pAseConfigParameter,
                        IPersistentStorePtr     pPersistentStore );

    /** Add a media consumer */
    int32_t addMediaConsumer(   IMediaConsumerPtr   pMediaConsumer,
                                uint32_t&           mediaConsumerId );

    /** Remove a media consumer */
    int32_t removeMediaConsumer( uint32_t mediaConsumerId );

    /** Add a track, this results in the creation of a track downloader
     *  @param mediaConsumerId [in] The media consumer for this track
     *  @Param mediaTYpe [in] The media type for the track
     *  @param trackId [out] The allocated track id on success
     *  @return AS_NO_ERROR on success
     *          AS_NOT_FOUND if the mediaConsumerId is not recognozed
     *          AS_NOT_ENOUGH_MEMORY if memory allocation errors encountered
     */
    int32_t addTrack(   uint32_t                                    mediaConsumerId,
                        MediaType                     mediaType,
                        TrackConfiguration const&   config,
                        uint32_t&                                   trackId );

    /** Remove a track. */
    int32_t removeTrack( uint32_t trackId );

    /** Pause the downloading of a track. */
    int32_t pauseTrack( uint32_t trackId );

    /** Resume the downloading of a track. */
    int32_t resumeTrack( uint32_t trackId );

    /** Resume the downloading of a track (async version) */
    int32_t resumeTrackAsync( uint32_t trackId );

    /** Reconfigure a track */
    int32_t reconfigureTrack( uint32_t track, TrackConfiguration const& config );

    /** Set the transport reporter */
    int32_t setTransportReporter( ITransportReporterPtr pTransportReporter );

    /** Set the extra HTTP request headers. */
    int32_t setExtraRequestHeaders( const HttpHeaders& httpHeaders );

    /** Norify that new requests are available for a track */
    int32_t notifyNewRequest( uint32_t mediaConsumerId, uint32_t trackId );

    /*======= Reporting ========*/

    /** Report the aggregate bandwidth. */
    void reportAggregateBandwidth();

    /** Report some bytes are received. */
    void reportBytesReceived(
                    uint32_t trackId,
                    uint32_t bytesCount,
                    const AseTimeVal& receivedTime );

    /** Report a non-fatal network failure */
    void reportNetworkFailure( uint32_t trackId, AseErrorCode status, AseUrl::HostPort const& destination );

    /** Report downloading started on a track. */
    void reportTrackDownloadingStarted( uint32_t trackId, MediaType mediaType );

    /** Report downloading stopped on a track.*/
    void reportTrackDownloadingStopped( uint32_t trackId, MediaType mediaType );

    /** Get an URL request task for a specific track.
     *
     *  @param trackId [In] The track Id for which an URL request task is requested.
     *  @param pUrlRequestTask [Out] The URL request task.
     *
     *  @return AS_NO_ERROR on success,
     *          AS_UNKNOWN_ERROR on unknown error.
     *          AS_AGAIN if no URL request task is available.
     */
    int32_t getUrlRequestTask(
                        uint32_t trackId,
                        UrlRequestTaskPtr& pUrlRequestTask );

    /** Get an URL request task for a specific track.
     *
     *  @param trackId [In] The track Id for which an URL request task is requested.
     *  @param bInitialTask [in] True if the requester is in an initial state, about to open new connections
     *  @param pUrlRequestTask [Out] The URL request task.
     *
     *  @return AS_NO_ERROR on success,
     *          AS_UNKNOWN_ERROR on unknown error.
     *          AS_AGAIN if no URL request task is available.
     */
    int32_t getUrlRequestTask(
                        uint32_t trackId,
                        bool bInitialTask,
                        UrlRequestTaskPtr& pUrlRequestTask );

    /** Return an URL request task. This should be called strictly in the reverse
     *  order of getUrlRequestTask() is called.
     *
     *  @param pUrlRequestTask [In] The URL request task.
     */
    void returnUrlRequestTask( UrlRequestTaskPtr pUrlRequestTask );

    /** Report the completion of an URL request task.
     *
     *  @param pUrlRequestTask [In] The URL request task.
     */
    void completeUrlRequestTask( UrlRequestTaskPtr pUrlRequestTask );

    /** Check the media buffer availability.
     *
     *  @param mediaType [In] The media type.
     *
     *  @return true or false.
     */
    bool checkMediaBufferAvailability( uint32_t trackId );

    /** IRouteListener function. */

    /** Report the route packet traces.
     *
     *  @param destinationIpAddress [Out] The destination IP address.
     *  @param routePacketTraces [In] The route packet traces.
     */
    virtual void reportRoute(
                        const std::string& destinationIpAddress,
                        const std::vector<PacketTrace>& routePacketTraces );

    /** The DNS listener. */

    /** Report a successful DNS resolution. */
    virtual void reportHostNameResolved(
                    DnsQueryType dnsQueryType,
                    const std::vector<std::string>& nameServers,
                    const AseTimeVal& lookupTime,
                    const std::vector<std::string>& cnames,
                    const std::vector<IpAddressTtlRecord>& ipAddressTtlRecords );

    /** Report a successful DNS resolution. */
    virtual void reportDnsLookupFailure(
                    DnsQueryType dnsQueryType,
                    const std::vector<std::string>& nameServers,
                    const AseTimeVal& lookupTime,
                    const std::string& hostName,
                    int32_t failureCode );

protected:

    static HttpRequestManager* sInstance;

    /** Constructor. */
    HttpRequestManager();

    /** The thread procedure. */
    void Run( Thread& thread );

    /** Initialize the async HTTP client.
     *
     *  This function may be overwritten to instantiate different type of async
     *  HTTP client library.
     */
    int32_t initAsyncHttpClient();

    /** Deinitialize the async HTTP client. */
    int32_t deinitAsyncHttpClient();

    /** Compute the select() timeout value. */
    void computeSelectTimeout();

    /** Process selected connections. */
    void processSelectedHttpConnections( const AseTimeVal& timeNow );

    /** Process timeout. */
    void processTimeout( const AseTimeVal& timeNow );

    void configurationChange( MediaType mediaType );

    /** Process a command. */
    void processCommand();

    int32_t configureImpl( IAseConfigParameterPtr pAseConfigParameter,
                            IPersistentStorePtr pPersistentStore );

    int32_t addMediaConsumerImpl(   IMediaConsumerPtr           pMediaConsumer );

    int32_t removeMediaConsumerImpl( uint32_t mediaConsumerId );

    /** Add a track, this results in the creation of a track downloader. */
    int32_t addTrackImpl(   uint32_t mediaConsumerId,
                            MediaType mediaType,
                            TrackConfiguration const& config );

    /** Remove a track. */
    int32_t removeTrackImpl( uint32_t trackId );

    /** Pause the downloading of a track. */
    int32_t pauseTrackImpl( uint32_t trackId );

    /** Resume the downloading of a track. */
    int32_t resumeTrackImpl( uint32_t trackId );

    /** Reconfigure a track */
    int32_t reconfigureTrackImpl( uint32_t track, TrackConfiguration const& config );

    int32_t transportReporterChangeImpl( ITransportReporterPtr pTransportReporter );

    int32_t extraRequestHeadersChangeImpl( const HttpHeaders& httpHeaders );

    int32_t notifyNewRequestImpl( uint32_t mediaConsumerId, uint32_t trackId );

    /** Create a new TrackDownloader */
    int32_t createTrackDownloader( uint32_t trackId, MediaType mediaType );

    /** Fall back a track to SCOR track downloader. */
    int32_t fallbackToScor(
                    uint32_t trackId,
                    MediaType mediaType );

    /** Process pre select tasks. */
    void processPreSelectTasks();

    /** Process post select tasks. */
    void processPostSelectTasks( const AseTimeVal& timeNow );

    /** Disable pipelining */
    int32_t disablePipelining();

    /** Get a pending URL request task. */
    int32_t getPendingUrlRequestTask(
                        uint32_t trackId, bool bInitialTask,
                        UrlRequestTaskPtr& pPendingUrlRequestTask );

    /** Get a new URL request task from a media request obtained from
     *  the media consumer. */
    int32_t getNewUrlRequestTask(   IMediaConsumerPtr   pMediaConsumer,
                                    uint32_t            trackId,
                                    UrlRequestTaskPtr&  pNewUrlRequestTask );


    /** Check if the configuration is compatible with pipelining state and modify */
    TrackConfiguration checkTrackConfiguration( TrackConfiguration const& config );

#if 0
    /** Perform a location selection
     *  @param trackId [in] The track id to select for
     *  @param pUrlRequestTask [in[ The Url Request Task
     *  @param pLocationSelector [in/out] Location selector. If non-null, selection will fail
     *                                    with AS_NOT_ALLOWED if this UrlRequestTask has a different
     *                                    Location selector
     *  @param pStreamingLocationSet [out] The selected Location Set
     */
    int32_t selectLocation( uint32_t                    trackId,
                            UrlRequestTaskPtr           pUrlRequestTask,
                            ILocationSelectorPtr&       pLocationSelector,
                            IStreamingLocationSetPtr&   pStreamingLocationSet );
#endif

protected:

    /** Type definition for track downloader map. */
    typedef std::map<uint32_t, BaseTrackDownloader2Ptr> TrackDownloaderMap;

    /** The Media consumer record */
    class MediaConsumerRecord
    {
    public:
        typedef shared_ptr<MediaConsumerRecord> MediaConsumerRecordPtr;

        MediaConsumerRecord( IMediaConsumerPtr pMediaConsumer, uint32_t maxRedirects = 25  );

        ~MediaConsumerRecord();

        void configure( IAseConfigParameterPtr pAseConfigParameter );

    public:
        IMediaConsumerPtr               mMediaConsumerPtr;
        ByteRangeCachePtr               mByteRangeCachePtr;
        uint32_t                        mByteRangeCacheTrackId;
        UrlRedirectMapPtr               mUrlRedirectMapPtr;

        std::vector<uint32_t>           mTrackIds;

        /** The last time location selection was performed */
        AseTimeVal                      mLastUrlChangeCheckTime;
    };

    typedef MediaConsumerRecord::MediaConsumerRecordPtr MediaConsumerRecordPtr;

    typedef std::map<uint32_t,MediaConsumerRecordPtr> MediaConsumerRecordMap;

    /** The track record. */
    class TrackRecord
    {
    public:
        TrackRecord()
            : mOutstandingUrlRequestTaskCount( 0 )
        {
        }

        TrackRecord(    MediaConsumerRecordPtr      mediaConsumerRecordPtr,
                        MediaType   mediaType )
            : mMediaConsumerRecordPtr( mediaConsumerRecordPtr )
            , mMediaType( mediaType )
            , mOutstandingUrlRequestTaskCount( 0 )
            , mLastNonFatalErrorCode( AS_NO_ERROR )
            , mLastNonFatalErrorTime( AseTimeVal::ZERO )
            , mLastNonFatalErrorReportTime( AseTimeVal::ZERO )
            , mLastDataReceivedTime( AseTimeVal::ZERO )
        {
        }

    public:
        /** Media Consumer Id */
        MediaConsumerRecordPtr      mMediaConsumerRecordPtr;

        /** The track media type. */
        MediaType     mMediaType;

        /** The outstanding URL request task count. */
        uint32_t                    mOutstandingUrlRequestTaskCount;
#if 0
        /** The current stream key. */
        std::string                 mCurrentStreamKey;

        /** The current streaming location set. */
        IStreamingLocationSetPtr    mCurrentStreamingLocationSetPtr;

        /** The current location selector */
        ILocationSelectorPtr        mCurrentLocationSelectorPtr;
#endif
        /** The pending URL request task list for the track. */
        UrlRequestTaskList          mPendingUrlRequestTaskList;

        /** The last non-fatal error */
        AseErrorCode                mLastNonFatalErrorCode;

        /** The last time a non-fatal error occured */
        AseTimeVal                  mLastNonFatalErrorTime;

        /** The last time non-fatal errors were reported to the consumer */
        AseTimeVal                  mLastNonFatalErrorReportTime;

        /** The time at which data was last received */
        AseTimeVal                  mLastDataReceivedTime;
    };

    /** The track record map. */
    typedef std::map<uint32_t, TrackRecord> TrackRecordMap;

public:

    /** The command base class. */
    class Command
    {
        public:
            typedef shared_ptr<Command> CommandPtr;

            virtual int32_t invoke() = 0;

            virtual ~Command() {}

            virtual char const* name() { return "unknown"; }

        protected:
            Command() {}
    };

    typedef Command::CommandPtr CommandPtr;

    int32_t issueCommandSync( CommandPtr pCommand );

    int32_t issueCommandAsync( CommandPtr pCommand );

protected:

    class ConfigureCommand;
    class AddMediaConsumerCommand;
    class RemoveMediaConsumerCommand;
    class AddTrackCommand;
    class RemoveTrackCommand;
    class PauseTrackCommand;
    class ResumeTrackCommand;
    class ReconfigureTrackCommand;
    class TransportReporterChangeCommand;
    class ExtraRequestHeadersChangeCommand;
    class NotifyNewRequestCommand;

    /** Create a new track downloader with the specified id, type and config */
    int32_t createTrackDownloader(  MediaConsumerRecord const& consumer,
                                    uint32_t trackId,
                                    MediaType mediaType,
                                    TrackConfiguration const& config,
                                    BaseTrackDownloader2Ptr& pTrackDownloader );

    /** Check whether location swicthes are needed for specified tracks */
    int32_t checkForUrlChanges( uint32_t                mediaConsumerId,
                                      MediaConsumerRecord&    mediaConsumerRecord,
                                      AseTimeVal const&  timeNow );

    /** Get a Track Record by media type */
    TrackRecordMap::iterator findTrackRecord( MediaType mediaType );

    /** Get a Track Downloader by media type */
    BaseTrackDownloader2Ptr findTrackDownloader( std::vector<uint32_t> const& trackIds, MediaType mediaType );

    /** Queu a task against a track */
    void queueTaskToTrack(  TrackRecordMap::iterator trackRecordItr,
                            UrlRequestTaskPtr pUrlRequestTask,
                            bool notify );

    /** Check whether we should disable pipelining */
    bool shouldDisablePipelining( AseUrl const& url );

    class HttpRequestManagerThread : public Thread
    {
    public:
        HttpRequestManagerThread()
                    : Thread(&THREAD_HTTPREQUEST_MANAGER)
        {}

        virtual void Run() { HttpRequestManager::instance()->Run( *this ); }
    };

protected:

    /** The HttpRequestManager thread */
    shared_ptr<HttpRequestManagerThread> mThread;

    /** The pointer to the configuration parameter. */
    IAseConfigParameterPtr mAseConfigParameterPtr;

    /** The pointer to the transport reporter.*/
    ITransportReporterPtr mTransportReporterPtr;

    /** The async HTTP library. */
    IAsyncHttpClientPtr mAsyncHttpClientPtr;

    /** Pipelining support detector */
    HttpPipeliningSupportDetectorPtr mHttpPipeliningSupportDetectorPtr;

    /** The extra HTTP request headers. */
    HttpHeaders mExtraRequestHeaders;

    /** The running flag. */
    volatile bool mRunningFlag;

    /** The select() timeout value. */
    AseTimeVal mSelectTimeout;

    /** The earliest timeout value. */
    AseTimeVal mEarliestTimeout;

    /** The selected HTTP connections. */
    std::vector<IAsyncHttpConnectionPtr> mSelectedHttpConnections;

    /** Next track id */
    uint32_t mNextTrackId;

    /** The track downloader map. */
    TrackDownloaderMap mTrackDownloaderMap;

    /** The track record map. */
    TrackRecordMap mTrackRecordMap;

    /** Next Media Consumer Id */
    uint32_t mNextMediaConsumerId;

    /** The media consumer map */
    MediaConsumerRecordMap mMediaConsumerRecordMap;

    /** Have we run an explicit pipelining support check */
    bool        mDidPipelingSupportCheck;

    /** Pipelinging should be disabled */
    bool        mDisablePipelinePending;

    /** Configuration parameters. */
    bool        mPipelineEnabled;
    uint32_t    mAggregateBandwidthInterval;
    bool        mTraceRouteEnabled;
    AseTimeVal  mUrlChangeCheckInterval;
    AseTimeVal  mMinSelectInterval;
    bool        mPipelineStreamChanges;

    int mMinLoopSleepMicros;
    int mCommandSleepMicros;

    /** The flag indicates the bytes have been received. */
    bool mBytesReceivedFlag;

    /** Previous number of downloading tack downloader. */
    uint32_t mPreviousDownloadingTrackDownloaderCount;

    /** Total number of downloading track downloader. */
    uint32_t mDownloadingTrackDownloaderCount;

    /** Aggregate bandwidth trace */
    BwEntry mAggregateBandwidthTrace;

    /** The mutex to guarantee exclusive issuing of commands. */
    Mutex mCommandMutex;

    Mutex mSignalMutex;

    ConditionVariable mSignalCondVar;

    /** The pending synchronous command. */
    CommandPtr  mPendingCommand;

    /** Number of pending synchronous wakes */
    uint32_t    mWakeCount;

    Mutex mAsyncCommandMutex;

    /** Pending asycnronous commands */
    std::queue<CommandPtr>  mPendingAsyncCommands;

    /** The synchronouscommand return value. */
    int32_t mCommandRetVal;

    /** The media request task ID pool. */
    uint32_t mMediaRequestTaskIdPool;

    /** The URL request task ID pool. */
    uint32_t mUrlRequestTaskIdPool;

    /** The turbo cache size */
    uint32_t             mByteRangeCacheSize;

    /** The last time we called select() */
    AseTimeVal     mLastSelectTime;

    /** Maximum number of http redircts */
    uint32_t            mMaximumRedirects;

    /** Report interval for non-fatal network failures */
    AseTimeVal          mNonFatalErrorReportInterval;

};

typedef HttpRequestManager::HttpRequestManagerPtr HttpRequestManagerPtr;

inline
void HttpRequestManager::MediaConsumerRecord::configure( IAseConfigParameterPtr pAseConfigParameter )
{
    mMediaConsumerPtr->configure( pAseConfigParameter );
}

inline
int32_t HttpRequestManager::getUrlRequestTask(
                        uint32_t trackId,
                        UrlRequestTaskPtr& pUrlRequestTask )
{
    return getUrlRequestTask( trackId, false, pUrlRequestTask );
}

inline
HttpRequestManager::TrackRecordMap::iterator
HttpRequestManager::findTrackRecord( MediaType mediaType )
{
    TrackRecordMap::iterator it = mTrackRecordMap.begin();
    for( ; it != mTrackRecordMap.end(); ++it )
    {
        if ( it->second.mMediaType == mediaType )
            break;
    }
    return it;
}

inline
BaseTrackDownloader2Ptr
HttpRequestManager::findTrackDownloader( std::vector<uint32_t> const& trackIds, MediaType mediaType )
{
    std::vector<uint32_t>::const_iterator trackIdItr = trackIds.begin();
    for( ; trackIdItr != trackIds.end(); ++trackIdItr )
    {
        TrackRecordMap::const_iterator trackRecordItr = mTrackRecordMap.find( *trackIdItr );
        if ( trackRecordItr != mTrackRecordMap.end() && trackRecordItr->second.mMediaType == mediaType )
        {
            TrackDownloaderMap::const_iterator trackDownloaderItr = mTrackDownloaderMap.find( *trackIdItr );
            if ( trackDownloaderItr != mTrackDownloaderMap.end() )
            {
                return trackDownloaderItr->second;
            }
        }
    }

    return BaseTrackDownloader2Ptr();
}

}}
#endif // HTTP_REQUEST_MANAGER_H
