/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <cstdlib>

#include <nrdbase/Configuration.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/Log.h>
#include <nrdbase/Time.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdbase/PerformanceMarker.h>

#include "AseCommonUtil.h"
#include "DebugLogging.h"
#include "EventsPerSecond.h"
#include <nrdnet/IpConnectivityManager.h>
#include <nrdnet/ConnectionPool.h>

#include "HttpRequestManager.h"
#include "McplTrackDownloader2.h"
#include "ScorTrackDownloader2.h"
#include "StreamersLog.h"
#include "ByteRangeCache.h"
#include "HttpPipeliningSupportDetector.h"

using namespace netflix::ase;
using namespace std;

/** TrackDownloader lookups
 *
 * Remove/Pause/Resume track - by track id - rare - could require Media Consumer Id
 * computerSelectTimout() - iterate over all tracks - very common
 * processSelectedHttpConnections() - find tracks by id - very common
 * processTimeout() - iterate over tracks - very common
 * getUrlRequestTask() - look up by id - find LocationSelector for a track
 */

#if defined(NRDP_HAS_TRACING)
static char const * mediaTypeString( MediaType mediaType )
{
    return  ( ( mediaType == MEDIA_AUDIO ) ? "audio"
            : ( mediaType == MEDIA_VIDEO ) ? "video"
            : ( mediaType == MEDIA_HEADERS ) ? "headers"
            : "other" );
}
#endif

namespace netflix {
namespace ase {

#define HTTP_REQUEST_MANAGER_COMMAND_TIME_OUT (3000)
//#define ENABLE_CONNECTION_POOL 1

DEFINE_LOGGER( HttpRequestManager );

HttpRequestManager* HttpRequestManager::sInstance = new HttpRequestManager();

class HttpRequestManager::ConfigureCommand : public HttpRequestManager::Command
{
public:
    ConfigureCommand(   IAseConfigParameterPtr  pAseConfigParameter,
                        IPersistentStorePtr     pPersistentStore )
        : mAseConfigParameterPtr( pAseConfigParameter )
        , mPersistentStorePtr( pPersistentStore )
    {}

    int32_t invoke() { return HttpRequestManager::instance()->configureImpl( mAseConfigParameterPtr, mPersistentStorePtr ); }

    virtual char const* name() { return "Configure"; }

protected:
    IAseConfigParameterPtr  mAseConfigParameterPtr;
    IPersistentStorePtr     mPersistentStorePtr;
};

class HttpRequestManager::AddMediaConsumerCommand : public HttpRequestManager::Command
{
    public:
        AddMediaConsumerCommand( IMediaConsumerPtr pMediaConsumer )
            : mMediaConsumerPtr( pMediaConsumer )
        {}

    int32_t invoke() { return HttpRequestManager::instance()->addMediaConsumerImpl( mMediaConsumerPtr ); }

    virtual char const* name() { return "AddMediaConsumer"; }

    protected:
        IMediaConsumerPtr           mMediaConsumerPtr;
};

class HttpRequestManager::RemoveMediaConsumerCommand : public HttpRequestManager::Command
{
    public:
        RemoveMediaConsumerCommand( uint32_t                mediaConsumerId )
            : mMediaConsumerId( mediaConsumerId )
        {}

    int32_t invoke() { return HttpRequestManager::instance()->removeMediaConsumerImpl( mMediaConsumerId ); }

    virtual char const* name() { return "RemoveMediaConsumer"; }

    protected:
        uint32_t    mMediaConsumerId;
};


class HttpRequestManager::AddTrackCommand : public HttpRequestManager::Command
{
    public :
        AddTrackCommand(    uint32_t                    mediaConsumerId,
                            MediaType                   mediaType,
                            TrackConfiguration const&   config)
            : mMediaConsumerId( mediaConsumerId )
            , mMediaType( mediaType )
            , mConfiguration( config )
        {}

    int32_t invoke() { return HttpRequestManager::instance()->addTrackImpl( mMediaConsumerId, mMediaType, mConfiguration ); }

    virtual char const* name() { return "AddTrack"; }

    protected:
        uint32_t            mMediaConsumerId;
        MediaType           mMediaType;
        TrackConfiguration  mConfiguration;
};

class HttpRequestManager::RemoveTrackCommand : public HttpRequestManager::Command
{
    public:
        RemoveTrackCommand( uint32_t                trackId )
            : mTrackId( trackId )
        {}

    int32_t invoke() { return HttpRequestManager::instance()->removeTrackImpl( mTrackId ); }

    virtual char const* name() { return "RemoveTrack"; }

    protected:
        uint32_t    mTrackId;
};

class HttpRequestManager::PauseTrackCommand : public HttpRequestManager::Command
{
    public:
        PauseTrackCommand(  uint32_t                trackId )
            : mTrackId( trackId )
        {}

    int32_t invoke() { return HttpRequestManager::instance()->pauseTrackImpl( mTrackId ); }

    virtual char const* name() { return "PauseTrack"; }

    protected:
        uint32_t    mTrackId;
};

class HttpRequestManager::ResumeTrackCommand : public HttpRequestManager::Command
{
    public:
        ResumeTrackCommand( uint32_t                trackId )
            : mTrackId( trackId )
        {}

    int32_t invoke() { return HttpRequestManager::instance()->resumeTrackImpl( mTrackId ); }

    virtual char const* name() { return "ResumeTrack"; }

    protected:
        uint32_t    mTrackId;
};

class HttpRequestManager::ReconfigureTrackCommand : public HttpRequestManager::Command
{
    public:
        ReconfigureTrackCommand(    uint32_t                    trackId,
                                    TrackConfiguration const&   config)
            : mTrackId( trackId )
            , mConfiguration( config )
        {}

    int32_t invoke() { return HttpRequestManager::instance()->reconfigureTrackImpl( mTrackId, mConfiguration ); }

    virtual char const* name() { return "ReconfigureTrack"; }

    protected:
        uint32_t            mTrackId;
        TrackConfiguration  mConfiguration;
};

class HttpRequestManager::TransportReporterChangeCommand : public HttpRequestManager::Command
{
    public:
        TransportReporterChangeCommand( ITransportReporterPtr   pTransportReporter )
            : mTransportReporterPtr( pTransportReporter )
        {}

    int32_t invoke() { return HttpRequestManager::instance()->transportReporterChangeImpl( mTransportReporterPtr ); }

    virtual char const* name() { return "TransportReporterChange"; }

    protected:
        ITransportReporterPtr   mTransportReporterPtr;
};

class HttpRequestManager::ExtraRequestHeadersChangeCommand : public HttpRequestManager::Command
{
    public:
        ExtraRequestHeadersChangeCommand(   HttpHeaders const&      httpHeaders )
            : mHttpHeaders( httpHeaders )
        {}

    int32_t invoke() { return HttpRequestManager::instance()->extraRequestHeadersChangeImpl( mHttpHeaders ); }

    virtual char const* name() { return "ExtraRequestHeaders"; }

    protected:
        HttpHeaders mHttpHeaders;
};

class HttpRequestManager::NotifyNewRequestCommand : public HttpRequestManager::Command
{
    public:
        NotifyNewRequestCommand( uint32_t consumerId, uint32_t trackId )
            : mConsumerId( consumerId )
            , mTrackId( trackId )
        {}

    int32_t invoke() { return HttpRequestManager::instance()->notifyNewRequestImpl( mConsumerId, mTrackId ); }

    virtual char const* name() { return "NotifyNewRequest"; }

    protected:
        uint32_t    mConsumerId;
        uint32_t    mTrackId;

};

HttpRequestManager::MediaConsumerRecord::MediaConsumerRecord( IMediaConsumerPtr pMediaConsumer, uint32_t maxRedirects )
    : mMediaConsumerPtr( pMediaConsumer )
    , mByteRangeCacheTrackId( 0 )
    , mUrlRedirectMapPtr( new UrlRedirectMap( maxRedirects ) )
    , mLastUrlChangeCheckTime( AseTimeVal::ZERO )
{
    //DEBUGLOG( HttpRequestManager, "MediaConsumerRecord constructed %p", this );
}

HttpRequestManager::MediaConsumerRecord::~MediaConsumerRecord()
{
    //DEBUGLOG( HttpRequestManager, "MediaConsumerRecord destructed %p", this );
}

HttpRequestManagerPtr HttpRequestManager::instance()
{
    return sInstance;
}

/** Constructor. */
HttpRequestManager::HttpRequestManager()
                                    : mRunningFlag( false ),
                                      mNextTrackId( 1 ),
                                      mNextMediaConsumerId( 1 ),
                                      mDidPipelingSupportCheck( false ),
                                      mDisablePipelinePending( false ),
                                      mPipelineEnabled( true ),
                                      mAggregateBandwidthInterval( 10000 ),
                                      mTraceRouteEnabled( false ),
                                      mPipelineStreamChanges( false ),
                                      mMinLoopSleepMicros( -1 ),
                                      mCommandSleepMicros( -1 ),
                                      mBytesReceivedFlag( false ),
                                      mPreviousDownloadingTrackDownloaderCount( 0 ),
                                      mDownloadingTrackDownloaderCount( 0 ),
                                      mAggregateBandwidthTrace( 0 ),
                                      mCommandMutex(HTTPREQUESTMGRCOMMAND_MUTEX, "HttpRequestManagerCommand"),
                                      mSignalMutex(HTTPREQUESTMGRSIGNAL_MUTEX, "HttpRequestManagerSignal"),
                                      mWakeCount( 0 ),
                                      mAsyncCommandMutex(HTTPREQUESTMGRASYNCCOMMAND_MUTEX, "HttpRequestManagerAsyncCommand"),
                                      mMediaRequestTaskIdPool( 0 ),
                                      mUrlRequestTaskIdPool( 1 ),
                                      mMaximumRedirects( 25 ),
                                      mNonFatalErrorReportInterval( AseTimeVal::fromMS( 1000 ) )
{
}

int32_t HttpRequestManager::configureImpl(  IAseConfigParameterPtr  pAseConfigParameter,
                                            IPersistentStorePtr     pPersistentStore )
{

    DEBUGLOG( HttpRequestManager, "HttpRequestManager::configure()" );

    int32_t retVal = AS_NO_ERROR;

    mAseConfigParameterPtr = pAseConfigParameter;

    mPipelineEnabled = mAseConfigParameterPtr
            ->getParameterAs<bool>( "pipelineEnabled", DEFAULT_PIPELINE_ENABLED );

    mAggregateBandwidthInterval = mAseConfigParameterPtr
            ->getParameterAs<uint32_t>( "bwSampleWindow", 1000, 600000, 10000 );

    mAggregateBandwidthTrace.setInterval( mAggregateBandwidthInterval );

    mTraceRouteEnabled = mAseConfigParameterPtr
            ->getParameterAs<bool>( "traceRouteEnabled", false );

    mByteRangeCacheSize = mAseConfigParameterPtr
            ->getParameterAs<uint32_t>( "turboCacheSize", 0, 1024*1024, 96 * 1024 );

    mUrlChangeCheckInterval  = AseTimeVal::fromMS( mAseConfigParameterPtr
                ->getParameterAs<uint32_t>( "locationSelectionInterval", 0, 2000, 250 ) );

    mMinSelectInterval = AseTimeVal::fromMS( mAseConfigParameterPtr
                ->getParameterAs<uint32_t>( "minSelectInterval", 0, 250, 0 ) );

    mPipelineStreamChanges = mAseConfigParameterPtr
                ->getParameterAs<bool>( "pipelineStreamChanges", false );

    mMinLoopSleepMicros = mAseConfigParameterPtr
                ->getParameterAs<int>( "minLoopSleepMicros", -1 );

    mCommandSleepMicros = mAseConfigParameterPtr
                ->getParameterAs<int>( "commandSleepMicros", -1 );

    MediaConsumerRecordMap::iterator mediaConsumerItr = mMediaConsumerRecordMap.begin();
    for( ; mediaConsumerItr != mMediaConsumerRecordMap.end(); ++mediaConsumerItr )
    {
        mediaConsumerItr->second->configure( pAseConfigParameter );

        std::vector<uint32_t>::iterator trackit = mediaConsumerItr->second->mTrackIds.begin();
        for( ; trackit != mediaConsumerItr->second->mTrackIds.end(); ++trackit )
        {
            TrackDownloaderMap::iterator trackDownloaderItr = mTrackDownloaderMap.find( *trackit );
            if ( trackDownloaderItr != mTrackDownloaderMap.end() )
            {
                retVal = reconfigureTrackImpl(  *trackit,
                                                checkTrackConfiguration( trackDownloaderItr->second->getTrackConfiguration() ) );
            }
        }

        if ( retVal != AS_NO_ERROR )
            break;
    }

    if ( !mHttpPipeliningSupportDetectorPtr )
    {
        mHttpPipeliningSupportDetectorPtr.reset(
            new HttpPipeliningSupportDetector( pPersistentStore, pAseConfigParameter ) );
    }
    else
    {
        mHttpPipeliningSupportDetectorPtr->setConfigParameter( pAseConfigParameter );
    }

    if ( mPipelineEnabled )
    {
        mHttpPipeliningSupportDetectorPtr->getPipelineState( mPipelineEnabled );
        DEBUGLOG( HttpRequestManager, "HttpRequestManager::configure(): pipeline state %u", mPipelineEnabled );
        if ( !mPipelineEnabled )
            disablePipelining();
    }

    mMaximumRedirects = mAseConfigParameterPtr
            ->getParameterAs<uint32_t>( "hrmMaximumRedirects", 2, 1000, 25 );

    mNonFatalErrorReportInterval = AseTimeVal::fromMS( mAseConfigParameterPtr
            ->getParameterAs<uint32_t>( "nonFatalErrorReportInterval", 100, 10000, 1000 ) );

    return retVal;
}

/** Destructor. */
HttpRequestManager::~HttpRequestManager()
{
    DEBUGLOG( HttpRequestManager, "~HttpRequestManager" );
}

/** Start the HTTP request manager. */
int32_t HttpRequestManager::start()
{
    int32_t retVal;

    if ( mRunningFlag )
    {
        return AS_NO_ERROR;
    }

    //Initialize the async HTTP client.
    retVal = initAsyncHttpClient();

    if (retVal == AS_NO_ERROR)
    {
        //Start the thread.
        mThread.reset( new HttpRequestManagerThread );
        mThread->Start();
    }

    return retVal;
}

/** Stop the HTTP request manager. */
int32_t HttpRequestManager::stop()
{
    int32_t retVal = AS_NO_ERROR;

    DEBUGLOG( HttpRequestManager, "HttpRequestManager::stop()%s running, %zu consumers, %zu track downloaders, %zu track records",
        mRunningFlag ? "" : " not",
        mTrackDownloaderMap.size(),
        mTrackRecordMap.size(),
        mMediaConsumerRecordMap.size() );

    if ( mRunningFlag )
    {
        //Signal the thread to exit.
        mRunningFlag = false;

        mAsyncHttpClientPtr->wake();

        mThread->Wait();

        TrackDownloaderMap::iterator trackDownloaderItr;

        trackDownloaderItr = mTrackDownloaderMap.begin();

        while (trackDownloaderItr != mTrackDownloaderMap.end())
        {
            trackDownloaderItr->second->deinit();

            trackDownloaderItr++;
        }

        mTrackDownloaderMap.clear();

        mTrackRecordMap.clear();

        mMediaConsumerRecordMap.clear();

        reportAggregateBandwidth();

        deinitAsyncHttpClient();

        retVal = AS_NO_ERROR;
    }

    return retVal;
}

/** The thread procedure. */
void HttpRequestManager::Run( Thread& thread )
{
    NRDP_UNUSED(thread);
    int32_t retVal;

    AseTimeVal timeNow;

    mRunningFlag = true;

    EventsPerSecond selectRate( "HttpRequestManager select()" );

    while (mRunningFlag)
    {
        PERFORMANCE_MARKER_SCOPED("hrm.loop");

        computeSelectTimeout();

        if ( mMinSelectInterval != AseTimeVal::ZERO )
        {
            timeNow = AseTimeVal::now();
            if ( ( timeNow - mLastSelectTime ) < mMinSelectInterval && mSelectTimeout != AseTimeVal::ZERO )
            {
                AseTimeVal const sleep = mLastSelectTime + mMinSelectInterval - timeNow;

                ::usleep( ( std::min( mSelectTimeout, sleep ) ).us() );

                mLastSelectTime += mMinSelectInterval;

                timeNow = AseTimeVal::now();

                if (mEarliestTimeout > timeNow)
                {
                    mSelectTimeout = mEarliestTimeout - timeNow;
                }
                else
                {
                    mEarliestTimeout = timeNow;

                    mSelectTimeout = AseTimeVal::ZERO;
                }
            }
            else
            {
                mLastSelectTime = timeNow;
            }
        }

        /*
        DEBUGLOG(
            HttpRequestManager,
            "Calling select with %llu ms timeout",
            mSelectTimeout.ms() );
        */

        processPreSelectTasks();

        selectRate.count();

        retVal = mAsyncHttpClientPtr->select(
                                        mSelectedHttpConnections,
                                        mSelectTimeout );

        timeNow = AseTimeVal::now();

        /*
        DEBUGLOG(
            HttpRequestManager,
            "Select returned with retVal=%d, %u selected connections, timeout was %llums",
            retVal, mSelectedHttpConnections.size(), mSelectTimeout.ms() );
        */

        if ((retVal == IAsyncHttpClient::SELECT_WAKE) || (retVal == IAsyncHttpClient::SELECT_WAKE_AND_CONNECT))
        {
            if (mRunningFlag)
            {
                PERFORMANCE_MARKER_SCOPED("hrm.loop.cmd");

                processCommand();

                processPostSelectTasks( timeNow );



                continue;
            }
            else
            {
                break;
            }
        }

        if ((retVal == IAsyncHttpClient::SELECT_CONNECT) || (retVal == IAsyncHttpClient::SELECT_WAKE_AND_CONNECT))
        {
            PERFORMANCE_MARKER_SCOPED("hrm.loop.conn");

            processSelectedHttpConnections( timeNow );
        }

        if ((retVal == 0) || (timeNow >= mEarliestTimeout))
        {
            PERFORMANCE_MARKER_SCOPED("hrm.loop.timeout");

            processTimeout( timeNow );
        }

        {
            PERFORMANCE_MARKER_SCOPED("hrm.loop.tasks");

            processPostSelectTasks( timeNow );
        }


    }

    return;
}

/** Initialize the async HTTP client. */
int32_t HttpRequestManager::initAsyncHttpClient()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    //Initialize the socket based async HTTP client.

    DEBUGLOG(
        HttpRequestManager,
        "had IP configuration parameters, "
        "DnsLookupTimeout: %u, DnsLookupRetryCount: %u, DnsCacheEntryLimit: %u ,"
        "DnsCacheTimeout: %u, DnsMinimumTtl: %u, DnsParallelLookupFlag: %s, DnsLogLookupsFlag: %s, "
        "DnsLogQueriesFlag: %s, SecondaryTcpConnectDelay: %u, HttpConnectTimeout: %u, "
        "HttpStallTimeout: %u, SocketReceiveBufferSize: %u.",
        IpConnectivityManager::instance()->getDnsLookupTimeout(),
        IpConnectivityManager::instance()->getDnsLookupRetryCount(),
        IpConnectivityManager::instance()->getDnsCacheEntryLimit(),
        IpConnectivityManager::instance()->getDnsCacheTimeout(),
        IpConnectivityManager::instance()->getDnsMinimumTtl(),
        IpConnectivityManager::instance()->getDnsParallelLookupFlag() ? "true" : "false",
        IpConnectivityManager::instance()->getDnsLogLookupsFlag() ? "true" : "false",
        IpConnectivityManager::instance()->getDnsLogQueriesFlag() ? "true" : "false",
        IpConnectivityManager::instance()->getSecondaryTcpConnectionDelay(),
        IpConnectivityManager::instance()->getHttpConnectionTimeout(),
        IpConnectivityManager::instance()->getHttpStallTimeout(),
        IpConnectivityManager::instance()->getSocketReceiveBufferSize() );

    if ( Configuration::reuseSocketConnection() ){
        ConnectionPool::reset(Configuration::maxNumReusedSocketConnection());
    }

    if (mTraceRouteEnabled)
    {
        retVal = constructAsyncHttpClient(
                                mAsyncHttpClientPtr,
                                IpConnectivityManager::instance(),
                                NULL,
                                this,
                                this,
                                Configuration::reuseSocketConnection()
            );
    }
    else
    {
        retVal = constructAsyncHttpClient(
                                mAsyncHttpClientPtr,
                                IpConnectivityManager::instance(),
                                NULL,
                                this,
                                NULL,
                                Configuration::reuseSocketConnection()
            );
    }

    mAsyncHttpClientPtr->setExtraRequestHeaders( mExtraRequestHeaders );

    if (retVal == AS_NO_ERROR)
    {
        retVal = mAsyncHttpClientPtr->init();

        if (retVal != AS_NO_ERROR)
        {
            mAsyncHttpClientPtr.reset();
        }
    }

    return retVal;
}

/** Deinitialize the async HTTP client. */
int32_t HttpRequestManager::deinitAsyncHttpClient()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    ConnectionPool::reset(0);

    retVal = mAsyncHttpClientPtr->deinit();

    mAsyncHttpClientPtr.reset();

    return retVal;
}

/** Compute the select() timeout value. */
void HttpRequestManager::computeSelectTimeout()
{
    AseTimeVal timeNow = AseTimeVal::now();

    //Find the earliest timeout among timers here and times from all
    //track downloaders.
    mEarliestTimeout = AseTimeVal::INFINITE;

    TrackDownloaderMap::iterator trackDownloaderItr;

    trackDownloaderItr = mTrackDownloaderMap.begin();

    while (trackDownloaderItr != mTrackDownloaderMap.end())
    {
        AseTimeVal trackDownloaderTimeout = trackDownloaderItr->second->getNextTimeout();

        if (trackDownloaderTimeout < mEarliestTimeout)
        {
            mEarliestTimeout = trackDownloaderTimeout;
        }

        trackDownloaderItr++;
    }

    if (mEarliestTimeout > timeNow)
    {
        mSelectTimeout = mEarliestTimeout - timeNow;
    }
    else
    {
        mEarliestTimeout = timeNow;

        mSelectTimeout = AseTimeVal::ZERO;
    }
}

/** Process selected connections. */
void HttpRequestManager::processSelectedHttpConnections( const AseTimeVal& timeNow )
{
    int32_t retVal;

    std::map<ullong, std::vector<IAsyncHttpConnectionPtr> > trackHttpConnectionVectorMap;

    std::map<ullong, std::vector<IAsyncHttpConnectionPtr> >::iterator trackHttpConnectionVectorItr;

    //Group the selected connections based on track ID.
    std::vector<IAsyncHttpConnectionPtr>::iterator asyncHttpConnectionItr;

    asyncHttpConnectionItr = mSelectedHttpConnections.begin();

    while (asyncHttpConnectionItr != mSelectedHttpConnections.end())
    {
        void* ptr = (*asyncHttpConnectionItr)->getUserData();

        uintptr_t trackId = (uintptr_t) ptr;

        trackHttpConnectionVectorItr = trackHttpConnectionVectorMap.find( trackId );

        if (trackHttpConnectionVectorItr != trackHttpConnectionVectorMap.end())
        {
            trackHttpConnectionVectorItr->second.push_back( *asyncHttpConnectionItr );
        }
        else
        {
            std::vector<IAsyncHttpConnectionPtr> asyncHttpConnectionVector;

            asyncHttpConnectionVector.push_back( *asyncHttpConnectionItr );

            trackHttpConnectionVectorMap[ trackId ] = asyncHttpConnectionVector;
        }

        asyncHttpConnectionItr++;
    }

    //Let track downloaders process the connections.
    trackHttpConnectionVectorItr = trackHttpConnectionVectorMap.begin();

    while (trackHttpConnectionVectorItr != trackHttpConnectionVectorMap.end())
    {

        TrackDownloaderMap::iterator const trackDownloaderItr = mTrackDownloaderMap.find( trackHttpConnectionVectorItr->first );
        TrackRecordMap::iterator const trackRecordItr = mTrackRecordMap.find( trackHttpConnectionVectorItr->first );
        if ( trackDownloaderItr != mTrackDownloaderMap.end()
                && trackRecordItr != mTrackRecordMap.end() )
        {
            bool dataReceived = false;
            bool networkErrors = false;

            retVal = trackDownloaderItr->second->
                            processSelectedHttpConnections( timeNow,
                                                            trackHttpConnectionVectorItr->second,
                                                            dataReceived,
                                                            networkErrors );

            if ( dataReceived )
            {
                trackRecordItr->second.mLastDataReceivedTime = timeNow;
            }

            if ( trackRecordItr->second.mLastNonFatalErrorTime != AseTimeVal::ZERO
                    && dataReceived
                    && !networkErrors )
            {
                trackRecordItr->second.mLastNonFatalErrorTime = AseTimeVal::ZERO;

                trackRecordItr
                    ->second.mMediaConsumerRecordPtr
                        ->mMediaConsumerPtr
                            ->networkFailing(   trackHttpConnectionVectorItr->first,
                                                AS_NO_ERROR,
                                                trackDownloaderItr->second->getDestination(),
                                                0 /* pLatestErrorTime */,
                                                timeNow );
            }

            assert(( retVal == AS_NO_ERROR ) || (retVal == AS_HTTP_PIPELINING_NOT_STABLE));

            if (retVal == AS_HTTP_PIPELINING_NOT_STABLE)
            {
                mDisablePipelinePending = true;
            }
        }

        trackHttpConnectionVectorItr++;
    }

    return;
}

/** Process timeout. */
void HttpRequestManager::processTimeout( const AseTimeVal& timeNow )
{
    int32_t retVal;

    //Let track downloaders process the timeout.
    TrackDownloaderMap::iterator trackDownloaderItr;

    trackDownloaderItr = mTrackDownloaderMap.begin();

    while (trackDownloaderItr != mTrackDownloaderMap.end())
    {
        retVal = trackDownloaderItr->second->processTimeout( timeNow );

        if (retVal == AS_HTTP_PIPELINING_NOT_STABLE)
        {
            mDisablePipelinePending = true;
        }

        trackDownloaderItr++;
    }
}

/** Prepare to issue a command */
int32_t HttpRequestManager::issueCommandSync( CommandPtr pCommand )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    mCommandMutex.lock();

    if ( mRunningFlag )
    {
        mSignalMutex.lock();

        mPendingCommand = pCommand;

        ++mWakeCount;

        mAsyncHttpClientPtr->wake();

        //A second timeout.
        Time timeout( HTTP_REQUEST_MANAGER_COMMAND_TIME_OUT );

        NFErr nfErr;

        nfErr = mSignalCondVar.wait( &mSignalMutex, timeout );

        if (nfErr.ok())
        {
            retVal = mCommandRetVal;
        }
        else
        {
            if (nfErr.peekCode() == NFErr_TimedOut)
            {
                ERRORLOG(
                    HttpRequestManager, "TimedOut in HttpRequestManager::issueCommandSync( %s )", pCommand->name() );
                    retVal = AS_TIMEOUT;

                /* This can happen if the HRM thread is blocked somehow
                 * Possibly, the HRM thread is blocked on a mutex in MediaConsumer::getNextRequest
                 * or another MediaConsumer call, where the mutex is already held by the
                 * caller of this syncronous comand
                 */
                // assert( false );
            }

            retVal = AS_TIMEOUT;
        }

        mPendingCommand.reset();

        mSignalMutex.unlock();
    }
    else
    {
        retVal = pCommand->invoke();
    }

    mCommandMutex.unlock();

    return retVal;
}

int32_t HttpRequestManager::issueCommandAsync( CommandPtr pCommand )
{
    int32_t retVal = AS_NO_ERROR;

    mAsyncCommandMutex.lock();

    if ( mRunningFlag )
    {
        mPendingAsyncCommands.push( pCommand );

        mAsyncHttpClientPtr->wake();
    }
    else
    {
        retVal = pCommand->invoke();
    }

    mAsyncCommandMutex.unlock();

    return retVal;
}

int32_t HttpRequestManager::configure(  IAseConfigParameterPtr  pAseConfigParameter,
                                        IPersistentStorePtr     pPersistentStore )
{
    return issueCommandSync( CommandPtr( new ConfigureCommand( pAseConfigParameter, pPersistentStore ) ) );
}

int32_t HttpRequestManager::addMediaConsumer(   IMediaConsumerPtr           pMediaConsumer,
                                                uint32_t&                   mediaConsumerId )
{
    int32_t retVal = issueCommandSync( CommandPtr( new AddMediaConsumerCommand( pMediaConsumer ) ) );
    if ( retVal >= 0 )
    {
        mediaConsumerId = retVal;
        retVal = AS_NO_ERROR;
    }

    return retVal;
}

int32_t HttpRequestManager::removeMediaConsumer( uint32_t mediaConsumerId )
{
    return issueCommandSync( CommandPtr( new RemoveMediaConsumerCommand( mediaConsumerId ) ) );
}

/** Add a track, this results in the creation of a track downloader. */
int32_t HttpRequestManager::addTrack(   uint32_t    mediaConsumerId,
                                        MediaType   mediaType,
                                        TrackConfiguration const& config,
                                        uint32_t&   trackId )
{
    int32_t retVal = issueCommandSync( CommandPtr( new AddTrackCommand(
                    mediaConsumerId, mediaType, config ) ) );

    if ( retVal >= 0 )
    {
        trackId = retVal;
        retVal = AS_NO_ERROR;
    }

    DEBUGLOG(
        HttpRequestManager,
        "added a track, consumer %u, id: %u, media type: %s, return: %d.",
        mediaConsumerId,
        trackId,
        mediaTypeString( mediaType ),
        retVal );

    return retVal;
}

char const * mediaTypeString( MediaType mediaType )
{
    return  ( ( mediaType == MEDIA_AUDIO ) ? "audio"
            : ( mediaType == MEDIA_VIDEO ) ? "video"
            : ( mediaType == MEDIA_HEADERS ) ? "headers"
            : "other" );
}

/** Remove a track. */
int32_t HttpRequestManager::removeTrack( uint32_t trackId )
{
    DEBUGLOG(
        HttpRequestManager,
        "removing a track, ID: %u.",
        trackId );

    int32_t retVal = issueCommandSync( CommandPtr( new RemoveTrackCommand( trackId ) ) );

    DEBUGLOG(
        HttpRequestManager,
        "removed a track, ID: %u, return: %d.",
        trackId,
        retVal );

    return retVal;
}

/** Pause the downloading of a track. */
int32_t HttpRequestManager::pauseTrack( uint32_t trackId )
{
    DEBUGLOG(
        HttpRequestManager,
        "pausing a track, ID: %u.",
        trackId );

    int32_t retVal = issueCommandSync( CommandPtr( new PauseTrackCommand( trackId ) ) );

    DEBUGLOG(
        HttpRequestManager,
        "paused a track, ID: %u, return: %d.",
        trackId,
        retVal );

    return retVal;
}

/** Resume the downloading of a track. */
int32_t HttpRequestManager::resumeTrack( uint32_t trackId )
{
    DEBUGLOG(
        HttpRequestManager,
        "resuming a track, ID: %u.",
        trackId );

    int32_t retVal = issueCommandSync( CommandPtr( new ResumeTrackCommand( trackId ) ) );

    DEBUGLOG(
        HttpRequestManager,
        "resumed a track, ID: %u, return: %d.",
        trackId,
        retVal );

    return retVal;
}

/** Reconfigure a track. */
int32_t HttpRequestManager::reconfigureTrack( uint32_t trackId, TrackConfiguration const& config )
{
    DEBUGLOG(
        HttpRequestManager,
        "reconfiguring a track, ID: %u.",
        trackId );

    int32_t retVal = issueCommandSync( CommandPtr( new ReconfigureTrackCommand( trackId, config ) ) );

    DEBUGLOG(
        HttpRequestManager,
        "reconfigured a track, ID: %u, return: %d.",
        trackId,
        retVal );

    return retVal;
}

int32_t HttpRequestManager::resumeTrackAsync( uint32_t trackId )
{
    return resumeTrackImpl( trackId );
}

int32_t HttpRequestManager::setTransportReporter( ITransportReporterPtr pTransportReporter )
{
    return issueCommandSync( CommandPtr( new TransportReporterChangeCommand( pTransportReporter ) ) );
}

int32_t HttpRequestManager::setExtraRequestHeaders( HttpHeaders const& httpHeaders )
{
    return issueCommandSync( CommandPtr( new ExtraRequestHeadersChangeCommand( httpHeaders ) ) );
}

int32_t HttpRequestManager::notifyNewRequest( uint32_t mediaConsumerId, uint32_t trackId )
{
    return issueCommandAsync( CommandPtr( new NotifyNewRequestCommand( mediaConsumerId, trackId ) ) );
}

/** Report the aggregate bandwidth. */
void HttpRequestManager::reportAggregateBandwidth()
{
    if (mAggregateBandwidthInterval > 0)
    {
        if (mAggregateBandwidthTrace.mTrace.size() > 0)
        {
            DEBUGLOG(
                HttpRequestManager,
                "reported the aggregate bandwidth, trace account: %zu",
                mAggregateBandwidthTrace.mTrace.size() );

            mAggregateBandwidthTrace.stop( AseTimeVal::now() );

            mTransportReporterPtr->aggregateTrace( mAggregateBandwidthTrace );

            mAggregateBandwidthTrace.reset();
        }
    }
}

/** Report some bytes are received. */
void HttpRequestManager::reportBytesReceived(
                                uint32_t trackId,
                                uint32_t bytesCount,
                                const AseTimeVal& receivedTime )
{
    NRDP_UNUSED(trackId);

    mBytesReceivedFlag = true;

    //Update aggregate trace
    if ( mAggregateBandwidthInterval > 0 )
    {
        mAggregateBandwidthTrace.add( receivedTime, bytesCount );
    }
}

void HttpRequestManager::reportNetworkFailure( uint32_t trackId, AseErrorCode status, AseUrl::HostPort const& destination )
{
    TrackRecordMap::iterator const it = mTrackRecordMap.find( trackId );
    if ( it != mTrackRecordMap.end() )
    {
        AseTimeVal const now = AseTimeVal::now();

        it->second.mLastNonFatalErrorCode = status;
        it->second.mLastNonFatalErrorTime = now;

        if ( it->second.mLastNonFatalErrorReportTime == AseTimeVal::ZERO
                || ( now - it->second.mLastNonFatalErrorReportTime ) >= mNonFatalErrorReportInterval )
        {
            it->second.mMediaConsumerRecordPtr
                ->mMediaConsumerPtr
                    ->networkFailing( trackId, status, destination, &now, it->second.mLastDataReceivedTime );

            it->second.mLastNonFatalErrorReportTime = now;
        }
    }
}


/** Report downloading started on a track. */
void HttpRequestManager::reportTrackDownloadingStarted(
                                    uint32_t trackId,
                                    MediaType /* mediaType */ )
{
    NRDP_UNUSED(trackId);

    assert( mDownloadingTrackDownloaderCount < mTrackDownloaderMap.size() );

    mDownloadingTrackDownloaderCount++;
/*
    DEBUGLOG(
        HttpRequestManager,
        "received reading started report, track ID: %u, downloading track count: %u.",
        trackId,
        mDownloadingTrackDownloaderCount );
*/
}

/** Report downloading stopped on a track.*/
void HttpRequestManager::reportTrackDownloadingStopped(
                                    uint32_t trackId,
                                    MediaType /*mediaType*/ )
{
    NRDP_UNUSED(trackId);

    assert( mDownloadingTrackDownloaderCount > 0 );

    mDownloadingTrackDownloaderCount--;
/*
    DEBUGLOG(
        HttpRequestManager,
        "received reading stopped report, track ID: %u, downloading track count: %u.",
        trackId,
        mDownloadingTrackDownloaderCount );
*/
}

/** Get an URL request task for a specific track. */
int32_t HttpRequestManager::getUrlRequestTask(
                                    uint32_t trackId,
                                    bool bInitialTask,
                                    UrlRequestTaskPtr& pUrlRequestTask )
{
    int32_t retVal = AS_NO_ERROR;
/*
    DEBUGLOG( HttpRequestManager, "getUrlRequestTask, trackId=%d, bInitialTask=%s",
                trackId, ( bInitialTask ? "true" : "false" ) );
*/

    if ( mDisablePipelinePending )
        return AS_AGAIN;

    do
    {
        //Get a pending URL request task.
        retVal = getPendingUrlRequestTask(
                                trackId, bInitialTask,
                                pUrlRequestTask );

        if ( retVal != AS_NO_ERROR )
        {
            break;
        }

        assert( pUrlRequestTask->getDownloadTrackId() == trackId );

        (void) pUrlRequestTask->selectUrl();

        if ( pUrlRequestTask->isAbandoned() )
        {
            // Request was cancelled or location selection failed
            continue;
        }

        if ( !pUrlRequestTask->hasUrl() )
        {
            // Queue this for later
            retVal = AS_AGAIN;

            mTrackRecordMap[trackId].mPendingUrlRequestTaskList.push_front( pUrlRequestTask );

            break;
        }

        // XXX Implement non-pipelining of stream changes ?


        mTrackRecordMap[trackId].mOutstandingUrlRequestTaskCount++;

        pUrlRequestTask->setId( mUrlRequestTaskIdPool++ );

        ByteRange requestByteRange;

        pUrlRequestTask->peekByteRange(
                                0xFFFFFFFF,
                                requestByteRange );

        DEBUGLOG(
            HttpRequestManager,
            "providing a URL request task, id: %u, track: %u, initial: %s, url: %s byte range: %lld-%lld",
            pUrlRequestTask->getId(),
            pUrlRequestTask->getDownloadTrackId(),
            bInitialTask ? "true" : "false",
            pUrlRequestTask->getCurrentUrl().GetPrefix().c_str(),
            requestByteRange.getStart(),
            requestByteRange.getEnd() );

        PERFTRACE("provide a URL reqeust task, track: %u, byte range: %lld-%lld, time: %llu\n",
                pUrlRequestTask->getDownloadTrackId(),
                requestByteRange.getStart(),
                requestByteRange.getEnd(),
                Time::mono().val());

    } while (false );

    return retVal;
}

/** Return an URL request task. */
void HttpRequestManager::returnUrlRequestTask( UrlRequestTaskPtr pUrlRequestTask )
{
    DEBUGLOG(
        HttpRequestManager,
        "returned a URL reqeust task 1, id: %u req: %s unscheduled: %lld abandoned: %s.",
        pUrlRequestTask->getId(),
        pUrlRequestTask->hasRequestsInProgress() ? "yes" : "no",
        pUrlRequestTask->getUnscheduledDataLength(),
        pUrlRequestTask->getMediaRequestTask()->getMediaRequest()->isAbandoned() ? "yes" : "no" );

    // Reset the UrlRequestTask - this clears any record of outstanding requests
    pUrlRequestTask->reset();

    TrackRecordMap::iterator trackRecordItr;

    trackRecordItr = mTrackRecordMap.find( pUrlRequestTask->getDownloadTrackId() );
    assert( trackRecordItr != mTrackRecordMap.end() );

    if ( !pUrlRequestTask->getMediaRequestTask()->getMediaRequest()->isAbandoned() )
    {
        // Push to our queue if the request is not abandoned

        //Try to combine this URL request task's byte ranges to the front of the pending list
        //if they are from the same media request task.

        if ( trackRecordItr->second.mPendingUrlRequestTaskList.empty()
                || !trackRecordItr->second.mPendingUrlRequestTaskList.front()->add( pUrlRequestTask ) )
        {
            pUrlRequestTask->setId( 0 );

            trackRecordItr->second.mPendingUrlRequestTaskList.push_front( pUrlRequestTask );
        }

    }

    trackRecordItr->second.mOutstandingUrlRequestTaskCount--;

    return;
}

/** Report the completion of an URL request task. */
void HttpRequestManager::completeUrlRequestTask( UrlRequestTaskPtr pUrlRequestTask )
{
    DEBUGLOG(
        HttpRequestManager,
        "completed a URL reqeust task, id: %u.",
        pUrlRequestTask->getId() );

    TrackRecordMap::iterator trackRecordItr;

    trackRecordItr = mTrackRecordMap.find( pUrlRequestTask->getDownloadTrackId() );
    assert( trackRecordItr != mTrackRecordMap.end() );

    assert( trackRecordItr->second.mOutstandingUrlRequestTaskCount > 0 );

    trackRecordItr->second.mOutstandingUrlRequestTaskCount--;
}

/** Check the media buffer availability. */
bool HttpRequestManager::checkMediaBufferAvailability( uint32_t trackId )
{
    TrackRecordMap::const_iterator trackRecordItr = mTrackRecordMap.find( trackId );

    assert( trackRecordItr != mTrackRecordMap.end() );
    return trackRecordItr->second.mMediaConsumerRecordPtr->mMediaConsumerPtr->isBufferAvailable( trackRecordItr->second.mMediaType );
}

/** Report the route packet traces. */
void HttpRequestManager::reportRoute(
                            const std::string& destinationIpAddress,
                            const std::vector<PacketTrace>& routePacketTraces )
{
    if ( mTransportReporterPtr )
        mTransportReporterPtr->reportRoute(
                                destinationIpAddress,
                                routePacketTraces );
}

/** Report a successful DNS resolution. */
void HttpRequestManager::reportHostNameResolved(
                            DnsQueryType dnsQueryType,
                            const std::vector<std::string>& nameServers,
                            const AseTimeVal& lookupTime,
                            const std::vector<std::string>& cnames,
                            const std::vector<IpAddressTtlRecord>& ipAddressTtlRecords )
{
    if ( mTransportReporterPtr )
        mTransportReporterPtr->reportHostNameResolved(
                                dnsQueryType,
                                nameServers,
                                (int) lookupTime.ms(),
                                cnames,
                                ipAddressTtlRecords );
}

/** Report a successful DNS resolution. */
void HttpRequestManager::reportDnsLookupFailure(
                            DnsQueryType dnsQueryType,
                            const std::vector<std::string>& nameServers,
                            const AseTimeVal& lookupTime,
                            const std::string& hostName,
                            int32_t failureCode )
{
    if ( mTransportReporterPtr )
        mTransportReporterPtr->reportDnsLookupFailure(
                                    dnsQueryType,
                                    nameServers,
                                    (int) lookupTime.ms(),
                                    hostName,
                                    failureCode );
}

/** Process a command. */
void HttpRequestManager::processCommand()
{
    std::queue<CommandPtr> commands;

    mAsyncCommandMutex.lock();
    std::swap( mPendingAsyncCommands, commands );
    mAsyncCommandMutex.unlock();

    while( !commands.empty() )
    {
        commands.front()->invoke();

        commands.pop();

        (void) mAsyncHttpClientPtr->receiveSignalByteInternal();
    }

    mSignalMutex.lock();

    while( mWakeCount )
    {
        (void) mAsyncHttpClientPtr->receiveSignalByteInternal();

        --mWakeCount;
    }

    if ( mPendingCommand )
    {
        mCommandRetVal = mPendingCommand->invoke();

        mPendingCommand.reset();

        mSignalCondVar.signal();
    }

    mSignalMutex.unlock();
}

int32_t HttpRequestManager::addMediaConsumerImpl(   IMediaConsumerPtr           pMediaConsumer )
{
    uint32_t mediaConsumerId = mNextMediaConsumerId++;

    mNextMediaConsumerId &= 0x7fffffff;

    assert( mMediaConsumerRecordMap.find( mediaConsumerId ) == mMediaConsumerRecordMap.end() );

    DEBUGLOG( HttpRequestManager, "adding media consumer id %u", mediaConsumerId );

    MediaConsumerRecordPtr pMediaConsumerRecord( new MediaConsumerRecord( pMediaConsumer, mMaximumRedirects ) );

    mMediaConsumerRecordMap[ mediaConsumerId ] = pMediaConsumerRecord;

    if ( !mPipelineEnabled )
        pMediaConsumer->pipeliningDisabled();

    return mediaConsumerId;
}

int32_t HttpRequestManager::removeMediaConsumerImpl( uint32_t mediaConsumerId )
{
    MediaConsumerRecordMap::iterator it = mMediaConsumerRecordMap.find( mediaConsumerId );
    if ( it == mMediaConsumerRecordMap.end() )
    {
        DEBUGLOG( HttpRequestManager,
                  "removingMediaConsumerImpl consumer id %u not found", mediaConsumerId );
        return AS_NOT_FOUND;
    }

    vector<uint32_t> const trackIds = it->second->mTrackIds;
    vector<uint32_t>::const_iterator trit = trackIds.begin();
    for( ; trit != trackIds.end(); ++trit )
    {
        (void) removeTrackImpl( *trit );
    }

    mMediaConsumerRecordMap.erase( it );

    return AS_NO_ERROR;
}


/** Add a track, this results in the creation of a track downloader. */
int32_t HttpRequestManager::addTrackImpl(
                                    uint32_t    mediaConsumerId,
                                    MediaType   mediaType,
                                    TrackConfiguration const& config )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    MediaConsumerRecordMap::const_iterator it = mMediaConsumerRecordMap.find( mediaConsumerId );
    if ( it == mMediaConsumerRecordMap.end() )
    {
        DEBUGLOG( HttpRequestManager, "addTrackImpl: media consumer id %u not found",
                  mediaConsumerId );
        return AS_NOT_FOUND;
    }

    MediaConsumerRecord& mediaConsumerRecord = *(it->second);

    uint32_t trackId = mNextTrackId++;

    mNextTrackId &= 0x7fffffff;

    assert( mTrackRecordMap.find( trackId ) == mTrackRecordMap.end() );

    mTrackRecordMap[trackId] = TrackRecord( it->second, mediaType );

    DEBUGLOG( HttpRequestManager, "Socket buffer for track %d is %u", trackId, config.mSocketBufferSize );

    BaseTrackDownloader2Ptr pTrackDownloader;
    retVal = createTrackDownloader( mediaConsumerRecord, trackId, mediaType, config, pTrackDownloader );
    if ( pTrackDownloader )
    {
        mTrackDownloaderMap[ trackId ] = pTrackDownloader;
    }

    if ( retVal == AS_HTTP_PIPELINING_NOT_STABLE )
    {
        retVal = disablePipelining();
    }

    if ( retVal == AS_NO_ERROR )
    {
        mediaConsumerRecord.mTrackIds.push_back( trackId );
    }

    return ( retVal == AS_NO_ERROR ) ? trackId : retVal;
}

int32_t HttpRequestManager::createTrackDownloader(  MediaConsumerRecord const& consumer,
                                                    uint32_t trackId,
                                                    MediaType mediaType,
                                                    TrackConfiguration const& config,
                                                    BaseTrackDownloader2Ptr& pTrackDownloader )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    TrackConfiguration actualconfig = checkTrackConfiguration( config );

    if (actualconfig.mOpenRange)
    {
        //Create single connection open range track downloader.
        pTrackDownloader = BaseTrackDownloader2Ptr(
                                    new ScorTrackDownloader2(
                                                        trackId,
                                                        mediaType,
                                                        actualconfig,
                                                        this,
                                                        mAsyncHttpClientPtr,
                                                        mTransportReporterPtr,
                                                        mAseConfigParameterPtr,
                                                        consumer.mUrlRedirectMapPtr ) );

        if ( pTrackDownloader )
        {
            retVal = pTrackDownloader->init();

            DEBUGLOG(
                HttpRequestManager,
                "created a SCOR track downloader for track: %u.",
                trackId );
            PERFTRACE("created a SCOR track downloader for track: %u, time=%llu\n",
                    trackId, Time::mono().val());
        }
    }
    else
    {
        //Create multi connection pipelined track downloader.
        pTrackDownloader = BaseTrackDownloader2Ptr(
                                    new McplTrackDownloader2(
                                                        trackId,
                                                        mediaType,
                                                        actualconfig,
                                                        this,
                                                        mAsyncHttpClientPtr,
                                                        mTransportReporterPtr,
                                                        mAseConfigParameterPtr,
                                                        consumer.mUrlRedirectMapPtr ) );

        if ( pTrackDownloader )
        {
            retVal = pTrackDownloader->init();

            DEBUGLOG(
                HttpRequestManager,
                "created a MCPL track downloader for track: %u, %d max connections",
                trackId,
                config.mConnections );
            PERFTRACE(
                "created a MCPL track downloader for track: %u. time= %llu\n",
                trackId, Time::mono().val());
        }
    }

    return pTrackDownloader ? retVal : AS_NOT_ENOUGH_MEMORY;
}

/** Remove a track. */
int32_t HttpRequestManager::removeTrackImpl( uint32_t trackId )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    TrackDownloaderMap::iterator trackDownloaderItr;

    trackDownloaderItr = mTrackDownloaderMap.find( trackId );

    if (trackDownloaderItr != mTrackDownloaderMap.end())
    {
        trackDownloaderItr->second->deinit();

        mTrackDownloaderMap.erase( trackDownloaderItr );

        DEBUGLOG(
                 HttpRequestManager,
                 "removed a track downloader for track: %u.",
                 trackId );
    }
    else
    {
        DEBUGLOG(
                 HttpRequestManager,
                 "could not find the track downlaoder for track: %u to remove.",
                 trackId );
    }

    TrackRecordMap::iterator trackRecordItr;

    trackRecordItr = mTrackRecordMap.find( trackId );

    if (trackRecordItr != mTrackRecordMap.end())
    {
        MediaConsumerRecordPtr const pMediaConsumerRecord = trackRecordItr->second.mMediaConsumerRecordPtr;
        vector<uint32_t>& trackIds = pMediaConsumerRecord->mTrackIds;
        vector<uint32_t>::iterator trackIdItr = std::find(  trackIds.begin(),
                                                            trackIds.end(),
                                                            trackId );


        if ( trackIdItr != trackIds.end() )
            trackIds.erase( trackIdItr );

        if ( pMediaConsumerRecord->mByteRangeCacheTrackId == trackId )
            pMediaConsumerRecord->mByteRangeCacheTrackId = 0;

        mTrackRecordMap.erase( trackRecordItr );

        DEBUGLOG(
                 HttpRequestManager,
                 "removed a track record, ID: %u",
                 trackId );

        retVal = AS_NO_ERROR;
    }
    else
    {
        DEBUGLOG(
                 HttpRequestManager,
                 "could not find the track record, ID: %u to remove.",
                 trackId );

        retVal = AS_NOT_FOUND;
    }

    return retVal;
}

/** Pause the downloading of a track. */
int32_t HttpRequestManager::pauseTrackImpl( uint32_t trackId )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    TrackDownloaderMap::iterator trackDownloaderItr;

    trackDownloaderItr = mTrackDownloaderMap.find( trackId );

    if (trackDownloaderItr != mTrackDownloaderMap.end())
    {
        trackDownloaderItr->second->pauseDownloading( AseTimeVal::now() );

        DEBUGLOG(
                 HttpRequestManager,
                 "paused the track downlaoder for track: %u.",
                 trackId );

        retVal = AS_NO_ERROR;
    }
    else
    {
        DEBUGLOG(
                 HttpRequestManager,
                 "could not find the track downlaoder for track: %u to pause.",
                 trackId );

        retVal = AS_NOT_FOUND;
    }

    return retVal;
}

/** Resume the downloading of a track. */
int32_t HttpRequestManager::resumeTrackImpl( uint32_t trackId )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    TrackDownloaderMap::iterator trackDownloaderItr;

    trackDownloaderItr = mTrackDownloaderMap.find( trackId );

    if (trackDownloaderItr != mTrackDownloaderMap.end())
    {
        trackDownloaderItr->second->resumeDownloading( AseTimeVal::now() );

        DEBUGLOG(
                 HttpRequestManager,
                 "resumed the track downlaoder for track: %u.",
                 trackId );

        retVal = AS_NO_ERROR;
    }
    else
    {
        DEBUGLOG(
                 HttpRequestManager,
                 "could not find the track downlaoder for track: %u to resume.",
                 trackId );

        retVal = AS_NOT_FOUND;
    }

    return retVal;
}

int32_t HttpRequestManager::reconfigureTrackImpl( uint32_t trackId, TrackConfiguration const& config )
{
    int32_t retVal = AS_NO_ERROR;

    TrackDownloaderMap::iterator const trackDownloaderItr = mTrackDownloaderMap.find( trackId );
    TrackRecordMap::iterator const trackRecordItr = mTrackRecordMap.find( trackId );
    if ( trackDownloaderItr != mTrackDownloaderMap.end()
            && trackRecordItr != mTrackRecordMap.end() )
    {
        DEBUGLOG( HttpRequestManager, "Reconfiguring track %d: socket buffer is %u", trackDownloaderItr->first, config.mSocketBufferSize );

        retVal = trackDownloaderItr->second->configure( mAseConfigParameterPtr, config );

        if ( retVal != AS_NO_ERROR )
        {
            DEBUGLOG( HttpRequestManager, "Could not re-configure track downloader %d - re-opening",
                        trackDownloaderItr->first );

            bool downloadingPaused = trackDownloaderItr->second->getDownloadingPausedFlag();

            trackDownloaderItr->second->deinit();

            retVal = createTrackDownloader( *trackRecordItr->second.mMediaConsumerRecordPtr,
                                            trackId,
                                            trackDownloaderItr->second->getMediaType(),
                                            config,
                                            trackDownloaderItr->second );

            if ( retVal == AS_HTTP_PIPELINING_NOT_STABLE )
            {
                retVal = disablePipelining();
            }

            if ( retVal == AS_NO_ERROR && downloadingPaused )
            {
                trackDownloaderItr->second->pauseDownloading( AseTimeVal::now() );
            }
        }
    }

    return retVal;
}

int32_t HttpRequestManager::transportReporterChangeImpl( ITransportReporterPtr pTransportReporter )
{
    mTransportReporterPtr = pTransportReporter;

    return AS_NO_ERROR;
}

int32_t HttpRequestManager::extraRequestHeadersChangeImpl( const HttpHeaders& httpHeaders )
{
    mExtraRequestHeaders = httpHeaders;

    return AS_NO_ERROR;
}

int32_t HttpRequestManager::notifyNewRequestImpl( uint32_t /*mediaConsumerId*/, uint32_t trackId )
{
    TrackDownloaderMap::iterator const trackDownloaderItr = mTrackDownloaderMap.find( trackId );

    if ( trackDownloaderItr != mTrackDownloaderMap.end() )
    {
        trackDownloaderItr->second->notifyNewTaskIsWaiting();
    }

    return AS_NO_ERROR;
}

/** Process pre select tasks. */
void HttpRequestManager::processPreSelectTasks()
{
    mPreviousDownloadingTrackDownloaderCount = mDownloadingTrackDownloaderCount;

    mBytesReceivedFlag = false;
}

/** Process post select tasks. */
void HttpRequestManager::processPostSelectTasks( const AseTimeVal& timeNow )
{
    if ( mDisablePipelinePending )
    {
        disablePipelining();

        mDisablePipelinePending = false;
    }

    MediaConsumerRecordMap::iterator mediaConsumerItr = mMediaConsumerRecordMap.begin();
    for( ; mediaConsumerItr != mMediaConsumerRecordMap.end(); ++mediaConsumerItr )
    {
        MediaConsumerRecord& mediaConsumerRecord = *(mediaConsumerItr->second);

        mediaConsumerRecord.mMediaConsumerPtr->performStreamingTasks();

        checkForUrlChanges( mediaConsumerItr->first, mediaConsumerRecord, timeNow );
    }
}

/** Disable pipelining */
int32_t HttpRequestManager::disablePipelining()
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    mPipelineEnabled = false;

    /** Reconfigure the tracks */
    TrackDownloaderMap::iterator it = mTrackDownloaderMap.begin();
    for( ; it != mTrackDownloaderMap.end(); ++it )
    {
        if ( it->second->getTrackConfiguration().mPipeline )
        {
            TrackRecordMap::iterator const trackRecordItr = mTrackRecordMap.find( it->first );
            if ( trackRecordItr != mTrackRecordMap.end() )
            {

                bool downloadingPaused = it->second->getDownloadingPausedFlag();

                it->second->deinit();

                retVal = createTrackDownloader( *trackRecordItr->second.mMediaConsumerRecordPtr,
                                                it->second->getTrackId(),
                                                it->second->getMediaType(),
                                                it->second->getTrackConfiguration(),
                                                it->second );
                if ( retVal != AS_NO_ERROR )
                    break;

                retVal = it->second->init();
                if ( retVal != AS_NO_ERROR )
                    break;

                if ( downloadingPaused )
                {
                    it->second->pauseDownloading( AseTimeVal::now() );
                }
            }
        }
    }

    /** Inform the media consumers */
    MediaConsumerRecordMap::iterator mit = mMediaConsumerRecordMap.begin();
    for( ; mit != mMediaConsumerRecordMap.end(); ++mit )
    {
        mit->second->mMediaConsumerPtr->pipeliningDisabled();
    }

    return retVal;
}

int32_t HttpRequestManager::checkForUrlChanges( uint32_t              mediaConsumerId,
                                                      MediaConsumerRecord&  mediaConsumerRecord,
                                                      AseTimeVal const&     timeNow )
{
    (void)mediaConsumerId;
    int32_t retVal = AS_NOT_FOUND;

    vector<uint32_t> const& trackIds = mediaConsumerRecord.mTrackIds;
    bool const bPeriodicSwitchAllowed = ( timeNow > mediaConsumerRecord.mLastUrlChangeCheckTime + mUrlChangeCheckInterval );

    vector<uint32_t>::const_iterator trackIdItr = trackIds.begin();
    for( ; trackIdItr != trackIds.end(); ++trackIdItr )
    {
        TrackDownloaderMap::iterator const trackDownloaderItr = mTrackDownloaderMap.find( *trackIdItr );
        TrackRecordMap::iterator const trackRecordItr = mTrackRecordMap.find( *trackIdItr );

        if ( trackDownloaderItr == mTrackDownloaderMap.end() || trackRecordItr == mTrackRecordMap.end() )
        {
            //DEBUGLOG( HttpRequestManager, "checkForLocationSwitches %u - track %d not found", mediaConsumerId, *trackIdItr );
            continue;
        }

        if ( trackRecordItr->second.mOutstandingUrlRequestTaskCount > 0 )
        {
            if ( bPeriodicSwitchAllowed )
            {
                mediaConsumerRecord.mLastUrlChangeCheckTime = timeNow;

                if ( trackDownloaderItr->second->checkForUrlChanges() )
                {
                    bool const downloadingPaused = trackDownloaderItr->second->getDownloadingPausedFlag();

                    trackDownloaderItr->second->deinit();

                    trackDownloaderItr->second->init();

                    if (downloadingPaused)
                    {
                        trackDownloaderItr->second->pauseDownloading( timeNow );
                    }

                    assert( trackRecordItr->second.mOutstandingUrlRequestTaskCount == 0 );
                }
            }
        }
        else if ( !trackRecordItr->second.mPendingUrlRequestTaskList.empty() )
        {
            // XXX Previous code supported a location selection check here if there were no oustanding
            //     requests but there were pending requests, but I think the track downloader
            //     will always pull the request and the location selection will happen then
        }
    }

    return retVal;
}

/** Get a pending URL request task. */
int32_t HttpRequestManager::getPendingUrlRequestTask(
                                    uint32_t trackId,
                                    bool bInitialTask,
                                    UrlRequestTaskPtr& pPendingUrlRequestTask )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    pPendingUrlRequestTask.reset();

    TrackRecordMap::iterator const trackRecordItr = mTrackRecordMap.find( trackId );

    if ( trackRecordItr == mTrackRecordMap.end() )
        return AS_AGAIN;

    UrlRequestTaskList& taskList = trackRecordItr->second.mPendingUrlRequestTaskList;

    while ( !taskList.empty()
                && taskList.front()->isAbandoned() )
    {
        taskList.pop_front();
    }

    UrlRequestTaskList::iterator it = taskList.begin();
    for( ; it != taskList.end(); ++it )
    {
        pPendingUrlRequestTask = *it;

        assert( pPendingUrlRequestTask->getId() == 0 );

        (void) pPendingUrlRequestTask->selectUrl();

        if ( pPendingUrlRequestTask->hasUrl() )
        {
            (void) taskList.erase( it );
            retVal = AS_NO_ERROR;
            break;
        }

        pPendingUrlRequestTask.reset();
    }

    if ( !pPendingUrlRequestTask )
    {
        UrlRequestTaskPtr pTempUrlRequestTask;

        retVal = getNewUrlRequestTask(  trackRecordItr->second.mMediaConsumerRecordPtr->mMediaConsumerPtr,
                                        trackId,
                                        pTempUrlRequestTask );

        if (retVal == AS_NO_ERROR)
        {
            // Check that this is for an existing track
            uint32_t requestTrackId = pTempUrlRequestTask->getDownloadTrackId();
            TrackRecordMap::iterator requestTrackRecordItr = mTrackRecordMap.find( requestTrackId );

            if( requestTrackRecordItr == mTrackRecordMap.end() )
            {
                DEBUGLOG( HttpRequestManager, "Discarding request for unknown track id %d", requestTrackId );

                pTempUrlRequestTask.reset();
            }
            else
            {
                //Do the pipeline check if necessary
                if ( mPipelineEnabled
                        && mHttpPipeliningSupportDetectorPtr
                        && !mDidPipelingSupportCheck
                        && shouldDisablePipelining( pTempUrlRequestTask->getCurrentUrl() ) )
                {
                    mDisablePipelinePending = true;

                    queueTaskToTrack( requestTrackRecordItr, pTempUrlRequestTask, true );

                    pTempUrlRequestTask.reset();

                    retVal = AS_AGAIN;
                }
                else
                {
                    ByteRangeCachePtr& pByteRangeCache = trackRecordItr->second.mMediaConsumerRecordPtr->mByteRangeCachePtr;

                    // Handle this request with the cache if we can
                    if ( pByteRangeCache &&
                            ( pByteRangeCache->getDownloadTrackId() == requestTrackId ) )
                    {
                        UrlRequestTaskPtr pRemainingUrlRequestTask;

                        if ( pByteRangeCache->serve( pTempUrlRequestTask, pRemainingUrlRequestTask ) )
                        {
                            pTempUrlRequestTask = pRemainingUrlRequestTask;
                        }
                        else if ( pByteRangeCache->complete() )
                        {
                            DEBUGLOG( HttpRequestManager, "Deleting byte range cache" );
                            pByteRangeCache.reset();
                        }
                    }
                }
            }

            if ( pTempUrlRequestTask )
            {
                // Now either serve or queue the request
                if ( trackId == requestTrackId )
                {
                    pPendingUrlRequestTask = pTempUrlRequestTask;
                }
                else
                {
                    queueTaskToTrack( requestTrackRecordItr, pTempUrlRequestTask, true );

                    retVal = AS_AGAIN;
                }
            }
            else
            {
                retVal = AS_AGAIN;
            }
        }
    }

    // Check if we want to create a byte range
    if ( pPendingUrlRequestTask && bInitialTask && trackRecordItr->second.mMediaType == MEDIA_VIDEO )
    {
        ByteRangeCachePtr& pByteRangeCache = trackRecordItr->second.mMediaConsumerRecordPtr->mByteRangeCachePtr;
        if ( !pByteRangeCache
                && pPendingUrlRequestTask->getMediaRequestTask()->getMediaRequest()->getByteRange().getStart() == 0
                && pPendingUrlRequestTask->getMediaRequestTask()->getMediaRequest()->getByteRange().getSize() < mByteRangeCacheSize )
        {
            DEBUGLOG( HttpRequestManager, "Creating byte range cache for track %d, trigger request track %u",
                                            trackId, pPendingUrlRequestTask->getDownloadTrackId() );

            AseErrorCode cacheRetVal = ByteRangeCache::construct(   trackId,
                                                                    pPendingUrlRequestTask,
                                                                    mByteRangeCacheSize,
                                                                    pByteRangeCache );
            if ( cacheRetVal == AS_NO_ERROR )
            {
                trackRecordItr->second.mMediaConsumerRecordPtr->mByteRangeCacheTrackId = trackId;

                MediaRequestTaskPtr pTurboMediaRequestTask( new MediaRequestTask(
                                                            mMediaRequestTaskIdPool++,
                                                            pByteRangeCache ) );
                if ( pTurboMediaRequestTask )
                {
                    UrlRequestTaskPtr pTurboUrlRequestTask = UrlRequestTaskPtr(
                                                                new UrlRequestTask(
                                                                0,
                                                                pTurboMediaRequestTask ) );
                    if ( pTurboUrlRequestTask )
                    {
                        // Pass the turbo cache request instead
                        DEBUGLOG( HttpRequestManager, "Created byte range cache for track %d, cache request track %u",
                                                        trackId,
                                                        pTurboUrlRequestTask->getDownloadTrackId() );
                        pPendingUrlRequestTask = pTurboUrlRequestTask;
                    }
                }
            }
            else
            {
                DEBUGLOG( HttpRequestManager, "Failed to create byte range cache for track %d (%d)", trackId, cacheRetVal );
            }
        }
    }

    return retVal;
}

bool HttpRequestManager::shouldDisablePipelining( AseUrl const& url )
{
    AseErrorCode result = mHttpPipeliningSupportDetectorPtr->getPipelineState( mPipelineEnabled );
    if ( result == AS_AGAIN )
    {
        DEBUGLOG( HttpRequestManager, "Performing pipelining check using %s",
                                            url.str().c_str() );
        mPipelineEnabled = mHttpPipeliningSupportDetectorPtr
                                ->runCheckPipelineSupport( url );
    }

    mDidPipelingSupportCheck = true;

    return !mPipelineEnabled;
}

void HttpRequestManager::queueTaskToTrack(  HttpRequestManager::TrackRecordMap::iterator trackRecordItr,
                                            UrlRequestTaskPtr pUrlRequestTask,
                                            bool notify )
{
    trackRecordItr->second.mPendingUrlRequestTaskList.push_back( pUrlRequestTask );

    if ( notify )
    {
        TrackDownloaderMap::iterator const trackDownloaderItr = mTrackDownloaderMap.find( trackRecordItr->first );

        if ( trackDownloaderItr != mTrackDownloaderMap.end() )
        {
            // Inform downloader there is a task waiting
            // This is purely an optimization - if we didn't do this the waiting track downloader
            // would still get the task next time its timeout fired
            trackDownloaderItr->second->notifyNewTaskIsWaiting();
        }
    }
}

/** Get a new URL request task from a media request obtained from the media consumer. */
int32_t HttpRequestManager::getNewUrlRequestTask(   IMediaConsumerPtr   pMediaConsumer,
                                                    uint32_t            trackId,
                                                    UrlRequestTaskPtr&  pNewUrlRequestTask )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    //There is no pending URL request task. Get one from the media consumer.
    MediaRequestPtr pMediaRequest;
    retVal = pMediaConsumer->getNextRequest( trackId, pMediaRequest );
    if (retVal == AS_NO_ERROR)
    {

#if ENABLE_STREAMERS_LOG
        gStreamersLog.addMediaRequestRecord(
                pMediaRequest->getDownloadTrackId(),
                pMediaRequest->getByteRange() );
#endif

        (void) pMediaRequest->selectUrl();

        if ( pMediaRequest->hasUrl() )
        {

            DEBUGLOG( HttpRequestManager, "New UrlRequestTask for %s, %lld-%lld",
                        pMediaRequest->getCurrentUrl().GetPrefix().c_str(),
                        pMediaRequest->getByteRange().start(),
                        pMediaRequest->getByteRange().end() );

            //Create a media request task.
            MediaRequestTaskPtr pMediaRequestTask( new MediaRequestTask(
                        mMediaRequestTaskIdPool++,
                        pMediaRequest ) );

            if (pMediaRequestTask)
            {
                //Create a new URL request task.
                pNewUrlRequestTask = UrlRequestTaskPtr( new UrlRequestTask(
                            0,
                            pMediaRequestTask ) );

                if (pNewUrlRequestTask)
                {
                    retVal = AS_NO_ERROR;
                }
                else
                {
                    retVal = AS_NOT_ENOUGH_MEMORY;
                }
            }
            else
            {
                retVal = AS_NOT_ENOUGH_MEMORY;
            }
        }
        else
        {
            pMediaRequest->abandoned();

            retVal = AS_AGAIN;
        }
    }
    else
    {
        retVal = AS_AGAIN;
    }

    return retVal;
}

TrackConfiguration HttpRequestManager::checkTrackConfiguration( TrackConfiguration const& config )
{
    TrackConfiguration result( config );

    if ( !mPipelineEnabled && result.mPipeline )
    {
        result.mPipeline = false;

        if ( result.mConnections > 1 )
        {
            result.mOpenRange = true;
            result.mConnections = 1;
        }
    }

    return result;
}

}}
