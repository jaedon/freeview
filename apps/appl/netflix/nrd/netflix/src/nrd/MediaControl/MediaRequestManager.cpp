/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaRequestManager.h"
#include "JsBridgeMediaRequest2.h"
#include "NrdpMediaSource.h"
#include <nrdase/HttpRequestManager.h>
#include <nrd/config.h>
#include <nrdbase/Log.h>
#include <nrdbase/ASError.h>

using namespace std;
using namespace netflix;
using namespace netflix::ase;
using namespace netflix::mediacontrol;

// TODO:
// Handle the case when the player does not have memory for first pending request, but
// need to schedule for requests of other media types, (on-the-fly audio switch case)


class MediaRequestManager::AppendBufferTask :
    public MediaRequestManager::MediaRequestManagerTask
{
public:
    AppendBufferTask(NrdpMediaSourcePtr mediaSource, uint32_t srcBufferId,
                     MediaSourceAppendablePtr pAppendable)
        : mMediaSource(mediaSource)
        , mSourceBufferId( srcBufferId )
        , mMediaSourceAppendablePtr( pAppendable )
    {}

    virtual ~AppendBufferTask() {}

    virtual void invoke( MediaRequestManager* pRequestManager )
    {
        pRequestManager->appendBufferImpl(mMediaSource, mSourceBufferId,
                                          mMediaSourceAppendablePtr);
    }

protected:
    NrdpMediaSourcePtr          mMediaSource;
    uint32_t                    mSourceBufferId;
    MediaSourceAppendablePtr    mMediaSourceAppendablePtr;
};

class MediaRequestManager::SwapUrlTask : public MediaRequestManager::MediaRequestManagerTask
{
public:
    SwapUrlTask( JsBridgeMediaRequestPtr pMediaRequest, AseUrl const& url )
        : mMediaRequestPtr( pMediaRequest )
        , mUrl( url )
    {}

    virtual ~SwapUrlTask() {}

    virtual void invoke( MediaRequestManager* pRequestManager )
    {
        NRDP_UNUSED( pRequestManager );

        mMediaRequestPtr->swapUrl( mUrl );
    }

protected:
    JsBridgeMediaRequestPtr mMediaRequestPtr;
    AseUrl                  mUrl;
};

class MediaRequestManager::AbandonRequestTask : public MediaRequestManager::MediaRequestManagerTask
{
public:
    AbandonRequestTask( JsBridgeMediaRequestPtr pMediaRequest )
        : mMediaRequestPtr( pMediaRequest )
    {}

    virtual ~AbandonRequestTask() {}

    virtual void invoke( MediaRequestManager* pRequestManager )
    {
        NRDP_UNUSED( pRequestManager );

        mMediaRequestPtr->setAbandoned();
    }

protected:
    JsBridgeMediaRequestPtr mMediaRequestPtr;
};

class MediaRequestManager::SetTimestampTask :
    public MediaRequestManager::MediaRequestManagerTask
{
public:
    SetTimestampTask(NrdpMediaSourcePtr mediaSource,  uint32_t srcBufferId,
                     int32_t timeOffset,uint32_t timescale)
        : mMediaSource(mediaSource)
        , mSrcBufferId( srcBufferId )
        , mTimeOffset( timeOffset )
        , mTimescale( timescale )
    {}

    virtual ~SetTimestampTask() {}

    virtual void invoke( MediaRequestManager* pRequestManager )
    {
        pRequestManager->setTimestampOffsetImpl(mMediaSource, mSrcBufferId, mTimeOffset, mTimescale);
    }

protected:
    NrdpMediaSourcePtr mMediaSource;
    uint32_t           mSrcBufferId;
    int32_t            mTimeOffset;
    uint32_t            mTimescale;
};

class MediaRequestManager::SourceBufferCompleteFlushTask
    : public MediaRequestManager::MediaRequestManagerTask
{
public:
    SourceBufferCompleteFlushTask( NrdpMediaSourcePtr mediaSource,  MediaType mediaType )
        : mMediaSource(mediaSource)
        , mMediaType( mediaType )
    {}

    virtual ~SourceBufferCompleteFlushTask() {}

    virtual void invoke( MediaRequestManager* pRequestManager )
    {
        NRDP_UNUSED(pRequestManager);
        mMediaSource->completeFlushSourceBuffer(mMediaType);
    }

protected:
    NrdpMediaSourcePtr  mMediaSource;
    MediaType           mMediaType;
};

class MediaRequestManager::SourceBuffersCompleteFlushTask
    : public MediaRequestManager::MediaRequestManagerTask
{
public:
    SourceBuffersCompleteFlushTask( NrdpMediaSourcePtr mediaSource )
        : mMediaSource(mediaSource)
    {}

    virtual ~SourceBuffersCompleteFlushTask() {}

    virtual void invoke( MediaRequestManager* pRequestManager )
    {
        NRDP_UNUSED(pRequestManager);
        NTRACE(TRACE_MEDIACONTROL, "SourceBuffersCompleteFlushTask" );
        mMediaSource->completeFlushSourceBuffers();
    }

protected:
    NrdpMediaSourcePtr mMediaSource;
};

MediaRequestManager::MediaRequestManager(IPersistentStorePtr pStorePtr)
                    : mMutex(NRDP_MEDIACONSUMER_MUTEX, "NrdpMediaRequestManager")
                    , mAudioBufferMonitor( MEDIA_AUDIO )
                    , mVideoBufferMonitor( MEDIA_VIDEO )
                    , mPendingRequests( 0 )
                    , mPendingHeaderRequests( 0 )
                    , mDownloadingPaused( false )
                    , mListener(MediaRequestManagerListenerPtr())
                    , mPersistentStorePtr(pStorePtr)
{
}

void MediaRequestManager::init( MediaRequestManagerListenerPtr  pListener,
                                ITransportReporterPtr           pTransportReporter)
{
    {
        ScopedMutex scopedMutex(mMutex);

        mListener = pListener;
    }

    HttpRequestManager::HttpRequestManagerPtr const hrm = HttpRequestManager::instance();

    // set a TransportReporter to HttpRequestManager
    hrm->setTransportReporter(
        static_pointer_cast<ITransportReporter>(pTransportReporter));

    // register this MediaRequestManager with HttpRequestManager
    AseErrorCode aserr =  hrm->addMediaConsumer(shared_from_this(),
                                                /*out*/mMediaConsumerId);
    if (aserr != AS_NO_ERROR)
    {
        Log::error(TRACE_MEDIACONTROL, "Unable to create media consumer for MediaRequestManager: %d", aserr);
        assert(false);
    }
}

void MediaRequestManager::configure( const IAseConfigParameterPtr &pConfig )
{
    // No Need to Implement
    NRDP_UNUSED(pConfig);
}

bool MediaRequestManager::getQueuedRequest( MediaRequestList& queue,
                                            BufferMonitor* monitor,
                                            JsBridgeMediaRequestPtr& pRequest )
{
    if ( !queue.empty() )
    {
        bool const appendable = queue.front()->isAppendable();
        if ( !monitor
                || !appendable
                || monitor->reserve( queue.front()->getRequestId(), queue.front()->getTotalBytes() ) )
        {
            pRequest = queue.front();
            queue.pop_front();
            if ( monitor && appendable ) pRequest->bufferReserved();
            return true;
        }
    }
    return false;
}

AseErrorCode MediaRequestManager::getNextRequest(uint32_t downloadTrackId, MediaRequestPtr& pRequest)
{
    {
        ScopedMutex scopedMutex(mMutex);
        JsBridgeMediaRequestPtr pBridgeRequest;

        DownloadTrackRecord * const pDownloadTrackRecord = getDownloadTrackRecord( downloadTrackId );
        if ( !pDownloadTrackRecord )
            return AS_NOT_ALlOWED;

        if ( mPendingRequests == 0 )
            return AS_PENDING;


        bool haveRequest = false;
        if ( mPendingHeaderRequests != 0 )
        {
            DownloadTrackMap::iterator it = mDownloadTrackMap.begin();
            for( ; it != mDownloadTrackMap.end(); ++it )
            {
                if ( it->second.mBufferMediaType == MEDIA_HEADERS && !it->second.mPendingRequests.empty() )
                {
                    pBridgeRequest = it->second.mPendingRequests.front();
                    it->second.mPendingRequests.pop_front();
                    haveRequest = true;
                    --mPendingHeaderRequests;
                    break;
                }
            }
        }

        if ( !haveRequest )
        {
            haveRequest = getQueuedRequest( pDownloadTrackRecord->mPendingRequests,
                                            bufferMonitor( pDownloadTrackRecord->mBufferMediaType ),
                                            pBridgeRequest );
        }


        if ( mListener )
        {
            if ( haveRequest && mDownloadingPaused )
            {
                mDownloadingPaused = false;
                mListener->onDownloadResumed(AseTimeVal::now());
            }
            else if ( !haveRequest && !mDownloadingPaused )
            {
                mDownloadingPaused = true;
                mListener->onDownloadPaused(AseTimeVal::now());
            }
        }

        if ( !haveRequest )
            return AS_PENDING;

        NTRACE(TRACE_MEDIACONTROL, "getNextRequest: request: %u download track: %u, byte range: %lld-%lld",
               pBridgeRequest->getRequestId(),
               pBridgeRequest->getDownloadTrackId(),
               //pBridgeRequest->getStartPts().getValueInMs(),
               pBridgeRequest->getByteRange().getStart(),
               pBridgeRequest->getByteRange().getEnd());

        --mPendingRequests;

        pRequest = pBridgeRequest;
    }

    if (mListener)
        mListener->onRequestScheduled(pRequest->getRequestId());

    pRequest->loadStarted();

    return AS_NO_ERROR;
}

void MediaRequestManager::getBufferStats( MediaType mediaType,
                                 ullong& usedBuffer, ullong& availableBuffer )
{
    // No Need to Implement
    NRDP_UNUSED(mediaType);
    usedBuffer = 0;
    availableBuffer = 128 * 1024 * 1024;
}


bool MediaRequestManager::isBufferAvailable(MediaType mediaType)
{
    // No Need to Implement
    NRDP_UNUSED(mediaType);
    return true;
}

#if 0
void MediaRequestManager::reportFailure(AseErrorCode errorCode, std::string const& message)
{
    // No Need to Implement
    // TOCHECK: The failure should be reported to each Media Request directly instead
    NRDP_UNUSED(errorCode);
    NRDP_UNUSED(message);
}

void MediaRequestManager::reportFailure(AseErrorCode errorCode, int32_t httpErr, std::string const& message)
{
    // No Need to Implement
    NRDP_UNUSED(errorCode);
    NRDP_UNUSED(httpErr);
    NRDP_UNUSED(message);
}
#endif

void MediaRequestManager::pipeliningDisabled()
{
    ScopedMutex scopedMutex(mMutex);
    ostringstream ostream;
    ostream.imbue(std::locale::classic());
    ostream << 0 << ":" << AseTimeVal::now().seconds();
    mPersistentStorePtr->saveValue( IPersistentStore::PIPELINE_DETECTION_RESULTS, ostream.str() );
    if (mListener)
        mListener->onPipeliningDisabled("hrm");
}

void MediaRequestManager::networkFailing(   uint32_t                downloadTrackId,
                                            AseErrorCode            latestStatus,
                                            AseUrl::HostPort const& destination,
                                            AseTimeVal const*       pLatestErrorTime,
                                            AseTimeVal const&       latestDataReceived )
{
    ScopedMutex scopedMutex(mMutex);
    if (mListener)
        mListener->onNetworkFailing( downloadTrackId, latestStatus, destination, pLatestErrorTime, latestDataReceived );
}

NFErr MediaRequestManager::addMediaRequest(shared_ptr<JsBridgeMediaRequest> pMediaRequest)
{
    bool notify = false;
    uint32_t const downloadTrackId = pMediaRequest->getDownloadTrackId();

    {
        ScopedMutex scopedMutex(mMutex);

        DownloadTrackRecord* const pDownloadTrackRecord = getDownloadTrackRecord( downloadTrackId );
        if ( !pDownloadTrackRecord )
            return NFErr_NotFound;

        notify = pDownloadTrackRecord->mPendingRequests.empty();

        pDownloadTrackRecord->mPendingRequests.push_back( pMediaRequest );

        ++mPendingRequests;
        if ( pDownloadTrackRecord->mBufferMediaType == MEDIA_HEADERS )
            ++mPendingHeaderRequests;

    }

    if ( notify )
    {
        HttpRequestManager::instance()->notifyNewRequest( mMediaConsumerId, downloadTrackId );
    }

    return NFErr_OK;
}

NFErr MediaRequestManager::flushMediaRequests( uint32_t downloadTrackId )
{
    ScopedMutex scopedMutex(mMutex);

    DownloadTrackRecord* const pDownloadTrackRecord = getDownloadTrackRecord( downloadTrackId );
    if ( !pDownloadTrackRecord )
        return NFErr_NotFound;

    MediaRequestList& requestList = pDownloadTrackRecord->mPendingRequests;

    mPendingRequests -= requestList.size();
    if ( pDownloadTrackRecord->mBufferMediaType == MEDIA_HEADERS )
        mPendingHeaderRequests -= requestList.size();

    mRequestsToFlush.insert( mRequestsToFlush.end(), requestList.begin(), requestList.end() );

    requestList.clear();

    NTRACE(TRACE_MEDIACONTROL, "MediaRequestManager::flushMediaRequests: track %u, remain requests: %u:%u",
            downloadTrackId,
            mPendingRequests, mPendingHeaderRequests);

    return NFErr_OK;
}

NFErr MediaRequestManager::flushMediaRequests( std::vector<JsBridgeMediaRequestPtr>& requests )
{
    ScopedMutex scopedMutex(mMutex);

    mRequestsToFlush.insert( mRequestsToFlush.end(), requests.begin(), requests.end() );

    return NFErr_OK;
}

void MediaRequestManager::printStats()
{
    NTRACE(TRACE_MEDIACONTROL, "MediaRequestManager::printStats remain requests: %u:%u",
            mPendingRequests, mPendingHeaderRequests );
    mAudioBufferMonitor.debug();
    mVideoBufferMonitor.debug();
}

NFErr MediaRequestManager::abortPendingMediaRequest( JsBridgeMediaRequestPtr pMediaRequest )
{
    ScopedMutex scopedMutex(mMutex);

    DownloadTrackRecord* const pDownloadTrackRecord = getDownloadTrackRecord( pMediaRequest->getDownloadTrackId() );
    if ( !pDownloadTrackRecord )
        return NFErr_NotFound;

    MediaRequestList& requestList = pDownloadTrackRecord->mPendingRequests;
    MediaRequestList::iterator const it = std::find(    requestList.begin(),
                                                        requestList.end(),
                                                        pMediaRequest );
    if ( it == requestList.end() )
        return NFErr_NotFound;

    requestList.erase( it );

    mRequestsToFlush.push_back( pMediaRequest );

    --mPendingRequests;

    if ( pDownloadTrackRecord->mBufferMediaType == MEDIA_HEADERS )
        --mPendingHeaderRequests;

    return NFErr_OK;
}


void MediaRequestManager::setConfig(IAseConfigParameterPtr      pConfigParameter,
                                    ase::IMediaBufferPoolPtr    pBufferPool)
{
    ScopedMutex scopedMutex(mMutex);
    //uint32_t const bufferFactor = 90;
    uint32_t const configedVideoBufferSize = pConfigParameter->getParameterAs<uint32_t>("configedVideoBufferSize", -1);
    uint32_t const configedAudioBufferSize = pConfigParameter->getParameterAs<uint32_t>("configedAudioBufferSize", -1);

    uint32_t const videoBufferPoolSize = pBufferPool->getBufferPoolSize(MEDIA_VIDEO);
    uint32_t const audioBufferPoolSize = pBufferPool->getBufferPoolSize(MEDIA_AUDIO);

    NTRACE(TRACE_MEDIACONTROL,"MediaRequestManager::setConfig: audio %uKB/%uKB, video %uKB/%uKB",
                                    audioBufferPoolSize / 1024, configedAudioBufferSize != (uint32_t) -1 ? configedAudioBufferSize / 1024 : 0,
                                    videoBufferPoolSize / 1024, configedVideoBufferSize != (uint32_t) -1 ? configedVideoBufferSize / 1024 : 0 );

    mAudioBufferMonitor.configure(  pBufferPool,
                                    std::min( configedAudioBufferSize, audioBufferPoolSize ),
                                    pConfigParameter->getParameterAs<uint32_t>("audioBufferTargetAvailableSize", 256 * 1024) );

    mVideoBufferMonitor.configure(  pBufferPool,
                                    std::min( configedVideoBufferSize, videoBufferPoolSize  ),
                                    pConfigParameter->getParameterAs<uint32_t>("mediaBufferTargetAvailableSize", 1024 * 1024) );
}

void MediaRequestManager::resetConfig()
{
    ScopedMutex scopedMutex(mMutex);
    NTRACE(TRACE_MEDIACONTROL, "MediaRequestManager::resetConfig");
    mAudioBufferMonitor.reset();
    mVideoBufferMonitor.reset();
}

void MediaRequestManager::appendBuffer(NrdpMediaSourcePtr mediaSource, uint32_t bufferId,
                                       MediaSourceAppendablePtr pAppendable)
{
    ScopedMutex scopedMutex(mMutex);

    MediaRequestManagerTaskPtr pTask( new AppendBufferTask(mediaSource, bufferId,
                                                           pAppendable));

    mTaskQueue.push( pTask );
}

void MediaRequestManager::appendBufferImpl(NrdpMediaSourcePtr mediaSource,
                                           uint32_t srcBufferId,
                                           MediaSourceAppendablePtr pAppendable)
{
    mediaSource->append(pAppendable, srcBufferId);
}

void MediaRequestManager::setSourceBufferTimestampOffset(NrdpMediaSourcePtr mediaSource,
                                                         uint32_t srcBufferId,
                                                         int32_t  timeOffset,
                                                         uint32_t timescale)
{
    ScopedMutex scopedMutex(mMutex);

    MediaRequestManagerTaskPtr pTask(new SetTimestampTask(mediaSource, srcBufferId, timeOffset,timescale));

    mTaskQueue.push( pTask );
}

void MediaRequestManager::setTimestampOffsetImpl(NrdpMediaSourcePtr mediaSource,
                                                 uint32_t srcBufferId,
                                                 int32_t timeOffset,
                                                 uint32_t timescale)
{
    mediaSource->setTimestampOffset(srcBufferId, timeOffset,timescale);
}

void MediaRequestManager::abandonMediaRequest( JsBridgeMediaRequestPtr pMediaRequest )
{
    ScopedMutex scopedMutex(mMutex);

    MediaRequestManagerTaskPtr pTask( new AbandonRequestTask( pMediaRequest ) );

    mTaskQueue.push( pTask );
}

void MediaRequestManager::completeFlushSourceBuffers( NrdpMediaSourcePtr mediaSource )
{
    ScopedMutex scopedMutex(mMutex);

    MediaRequestManagerTaskPtr pTask( new SourceBuffersCompleteFlushTask( mediaSource ) );

    mTaskQueue.push( pTask );
}

void MediaRequestManager::completeFlushSourceBuffer( NrdpMediaSourcePtr mediaSource, MediaType mediaType )
{
    ScopedMutex scopedMutex(mMutex);

    MediaRequestManagerTaskPtr pTask( new SourceBufferCompleteFlushTask( mediaSource, mediaType ) );

    mTaskQueue.push( pTask );
}

void MediaRequestManager::swapUrl( JsBridgeMediaRequestPtr pMediaRequest, AseUrl const& url )
{
    ScopedMutex scopedMutex(mMutex);

    MediaRequestManagerTaskPtr pTask( new SwapUrlTask( pMediaRequest, url ) );

    mTaskQueue.push( pTask );
}

/** Add a media source */
void MediaRequestManager::addMediaSource( NrdpMediaSourcePtr pMediaSource )
{
    ScopedMutex scopedMutex(mMutex);

    mNrdpMediaSources.push_back( pMediaSource );
}

/** Remove a media source */
void MediaRequestManager::removeMediaSource( NrdpMediaSourcePtr pMediaSource )
{
    ScopedMutex scopedMutex(mMutex);

    NrdpMediaSourceList::iterator const it = std::find( mNrdpMediaSources.begin(),
                                                        mNrdpMediaSources.end(),
                                                        pMediaSource );
    if ( it != mNrdpMediaSources.end() )
    {
        mRemovedNrdpMediaSources.push_back( *it );

        mNrdpMediaSources.erase( it );
    }
}

/** Add a download track */
NFErr MediaRequestManager::createDownloadTrack( ase::MediaType mediaType, ase::TrackConfiguration const& config, uint32_t& trackId )
{
    // add a media track to HRM
    HttpRequestManager::HttpRequestManagerPtr const mgr = HttpRequestManager::instance();
    AseErrorCode const aserr = mgr->addTrack(mMediaConsumerId, mediaType, config, /*out*/trackId);
    if ( aserr != AS_NO_ERROR )
        return NFErr( new ASError( aserr ) );

    {
        ScopedMutex scopedMutex(mMutex);

        mDownloadTrackMap.insert( make_pair( trackId, DownloadTrackRecord( mediaType ) ) );
    }

    return NFErr_OK;
}

/** Pause a download track */
NFErr MediaRequestManager::pauseDownloadTrack( uint32_t trackId )
{
    HttpRequestManager::HttpRequestManagerPtr const mgr = HttpRequestManager::instance();
    AseErrorCode const aserr = mgr->pauseTrack(trackId);
    if ( aserr != AS_NO_ERROR )
        return NFErr( new ASError( aserr ) );

    return NFErr_OK;
}

/** Resume a download track */
NFErr MediaRequestManager::resumeDownloadTrack( uint32_t trackId )
{
    HttpRequestManager::HttpRequestManagerPtr const mgr = HttpRequestManager::instance();
    AseErrorCode const aserr = mgr->resumeTrackAsync(trackId);
    if ( aserr != AS_NO_ERROR )
        return NFErr( new ASError( aserr ) );

    return NFErr_OK;
}

/** Reconfigure a download track */
NFErr MediaRequestManager::reconfigureDownloadTrack( uint32_t trackId, ase::TrackConfiguration const& config )
{
    HttpRequestManager::HttpRequestManagerPtr mgr = HttpRequestManager::instance();
    AseErrorCode aserr = mgr->reconfigureTrack(trackId, config);
    if ( aserr != AS_NO_ERROR )
        return NFErr( new ASError( aserr ) );

    return NFErr_OK;
}

/** Remove a download track */
NFErr MediaRequestManager::removeDownloadTrack( uint32_t downloadTrackId )
{
    HttpRequestManager::HttpRequestManagerPtr const mgr = HttpRequestManager::instance();
    AseErrorCode const aserr = mgr->removeTrack(downloadTrackId);
    if ( aserr != AS_NO_ERROR )
        return NFErr( new ASError( aserr ) );

    {
        ScopedMutex scopedMutex(mMutex);

        DownloadTrackRecord* const pDownloadTrackRecord = getDownloadTrackRecord( downloadTrackId );
        if ( pDownloadTrackRecord )
        {
            MediaRequestList& requestList = pDownloadTrackRecord->mPendingRequests;

            mPendingRequests -= requestList.size();
            if ( pDownloadTrackRecord->mBufferMediaType == MEDIA_HEADERS )
                mPendingHeaderRequests -= requestList.size();
        }

        (void) mDownloadTrackMap.erase( downloadTrackId );
    }

    return NFErr_OK;
}

bool MediaRequestManager::hasMediaDownloadTracks()
{
    DownloadTrackMap::iterator it = mDownloadTrackMap.begin();
    bool hasMediaTracks = false;
    for( ; it != mDownloadTrackMap.end(); ++it )
    {
        if ( it->second.mBufferMediaType == MEDIA_VIDEO ||
                it->second.mBufferMediaType == MEDIA_AUDIO )
        {
            hasMediaTracks = true;
            break;
        }
    }
    return hasMediaTracks;
}

void MediaRequestManager::performStreamingTasks()
{
    MediaRequestManagerTaskQueue tasks;
    NrdpMediaSourceList mediasources;
    {
        ScopedMutex scopedMutex(mMutex);

        mRequestsToFlush.clear();

        std::swap( mTaskQueue, tasks );

        mediasources = mNrdpMediaSources;

        mRemovedNrdpMediaSources.clear();
    }

    for( ; !mediasources.empty(); mediasources.pop_back() ) mediasources.back()->mediaSourceTasks();

    for( ; !tasks.empty(); tasks.pop() ) tasks.front()->invoke( this );
}

MediaRequestManager::DownloadTrackRecord* MediaRequestManager::getDownloadTrackRecord( uint32_t downloadTrackId )
{
    DownloadTrackMap::iterator const it = mDownloadTrackMap.find( downloadTrackId );
    return ( it == mDownloadTrackMap.end() ) ? 0 : &(it->second);
}

void MediaRequestManager::BufferMonitor::configure( ase::IMediaBufferPoolPtr    pBufferPool,
                                                    uint32_t                    sizelimit,
                                                    uint32_t                    drain )
{
    mBufferPoolPtr = pBufferPool;
    mUnitSize = mBufferPoolPtr->getUnitSize();
    mTotalUnits = pBufferPool->getBufferPoolSize( mMediaType ) / mUnitSize;
    mLimitUnits = sizelimit / mUnitSize;
    mDrainUnits = drain / mUnitSize;

    assert( mTotalUnits >= mLimitUnits );

    NTRACE(TRACE_MEDIACONTROL, "MediaRequestManager::BufferMonitor::configure, %u, unit=%uB, total=%uKB, limit=%uKB, drain=%uKB",
                mMediaType,
                mUnitSize,
                mTotalUnits * mUnitSize / 1024,
                mLimitUnits * mUnitSize / 1024,
                mDrainUnits * mUnitSize / 1024 );
}

void MediaRequestManager::BufferMonitor::reset()
{
    mBufferPoolPtr.reset();
    mUnitSize = 0;
    mTotalUnits = 0;
    mLimitUnits = 0;
    mDrainUnits = 0;
    mDraining = false;
}

bool MediaRequestManager::BufferMonitor::reserve( uint32_t requestid, uint32_t requestSize )
{
    NRDP_UNUSED( requestid );

    uint32_t const units = ( requestSize + mUnitSize - 1 ) / mUnitSize;
    uint32_t const free = mBufferPoolPtr->getUnreservedUnitCount( mMediaType ) - ( mTotalUnits - mLimitUnits );

    if ( free < ( mDraining ? std::max( mDrainUnits, units ) : units ) )
    {
        if ( !mDraining ) {
            NTRACE(TRACE_MEDIACONTROL, "MediaRequestManager::BufferMonitor(%u): limit=%u, free=%u, unreserved=%u: need %u -> draining",
                                            mMediaType,
                                            mLimitUnits,
                                            mBufferPoolPtr->getFreeUnitCount( mMediaType ),
                                            free,
                                            units );
        } else {
            NTRACE(TRACE_MEDIACONTROL, "MediaRequestManager::BufferMonitor(%u): limit=%u, free=%u, unreserved=%u: draining, need %u",
                                            mMediaType,
                                            mLimitUnits,
                                            mBufferPoolPtr->getFreeUnitCount( mMediaType ),
                                            free,
                                            std::max( mDrainUnits, units ) );
        }

        mDraining = true;
        return false;
    }
    else
    {
        mDraining = false;
        NTRACE(TRACE_MEDIACONTROL, "MediaRequestManager::BufferMonitor(%u): limit=%u, free=%u, unreserved=%u: reserving %u for request %u",
                                            mMediaType,
                                            mLimitUnits,
                                            mBufferPoolPtr->getFreeUnitCount( mMediaType ),
                                            free,
                                            units,
                                            requestid );

        bool const reserve = mBufferPoolPtr->reserveUnits( mMediaType, units );
        (void)reserve;
        assert( reserve );

        return true;
    }
}

void MediaRequestManager::BufferMonitor::debug()
{
    if ( mBufferPoolPtr )
    {
        uint32_t const free = mBufferPoolPtr->getUnreservedUnitCount( mMediaType ) - ( mTotalUnits - mLimitUnits );
        NRDP_UNUSED(free);
        NTRACE(TRACE_MEDIACONTROL, "MediaRequestManager::BufferMonitor(%u): limit=%u, free=%u, unreserved=%u",
                                                mMediaType,
                                                mLimitUnits,
                                                mBufferPoolPtr->getFreeUnitCount( mMediaType ),
                                                free );
    }
}


