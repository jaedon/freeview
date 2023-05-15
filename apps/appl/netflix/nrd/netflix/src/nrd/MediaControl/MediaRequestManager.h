/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef MEDIA_CONSUMER_H
#define MEDIA_CONSUMER_H

#include <nrdase/IMediaConsumer.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdase/ITransportReporter.h>
#include <nrdase/IPersistentStore.h>
#include <nrdase/IMediaBufferPool.h>
#include <nrdase/AseTimeStamp.h>
#include <nrdbase/NFErr.h>

#include "MediaRequestManagerListener.h"

#include <list>
#include <set>
#include <queue>

namespace netflix {
namespace mediacontrol {

class JsBridgeMediaRequest;
typedef shared_ptr<JsBridgeMediaRequest> JsBridgeMediaRequestPtr;

class NrdpMediaSource;
typedef shared_ptr<NrdpMediaSource> NrdpMediaSourcePtr;

class MediaSourceAppendable;
typedef shared_ptr<MediaSourceAppendable> MediaSourceAppendablePtr;

class MediaRequestManager : public ase::IMediaConsumer
{
public:
    typedef shared_ptr<MediaRequestManager> MediaRequestManagerPtr;

    MediaRequestManager(ase::IPersistentStorePtr pStorePtr);

    /** Initilize and rehister with HRM */
    void init(  MediaRequestManagerListenerPtr  pListener,
                ase::ITransportReporterPtr      pTransportReporter );

    // IMediaConsumer interfaces
    virtual void configure( const ase::IAseConfigParameterPtr &pConfig );

    /** Perform streaming tasks */
    virtual void performStreamingTasks();

    /** Get a new Request object
     *  @param pRequest [out] The new MediaRequest object
     *  @return 0   for success, negative error code.
     */
    virtual AseErrorCode getNextRequest(    uint32_t downloadTrackId,
                                            ase::MediaRequestPtr& pRequest );

    /** Get the buffer statistics
     * @param mediaType [in] the media type
     * @param usedBuffer [out] the current used buffer size in bytes
     * @param availableBuffer [out] the avaialable buffer size in bytes
     */
    virtual void getBufferStats( ase::MediaType mediaType,
                                 ullong& usedBuffer, ullong& availableBuffer );


    /** Check whether the buffer is available */
    virtual bool isBufferAvailable(ase::MediaType mediaType);

#if 0
    /** report irrecoverable failure
     * @param errorCode [in] the error code
     * @param message [in] the error message
     */
    virtual void reportFailure(AseErrorCode errorCode, std::string const& message);

    /**
     * report irrecoverable failure with http error code
     * @param errorCode [in] the error code
     * @param httpErr [in] the http error code
     */
    virtual void reportFailure(AseErrorCode errorCode, int32_t httpErr, std::string const& message);
#endif

    /**
     * report that pipelining as disabled
     */
    virtual void pipeliningDisabled();

    /**
     * Report network failures
     */
    virtual void networkFailing(    uint32_t                downloadTrackId,
                                    AseErrorCode            latestStatus,
                                    AseUrl::HostPort const& destination,
                                    AseTimeVal const*       pLatestErrorTime,
                                    AseTimeVal const&       latestDataReceived );

    NFErr addMediaRequest(JsBridgeMediaRequestPtr pMediaRequest);
    /**
     * flush all media requests for a given track id
     */
    NFErr flushMediaRequests( uint32_t downloadTrackId );

    NFErr flushMediaRequests( std::vector<JsBridgeMediaRequestPtr>& requests );
    /**
     *  remove the media request with the given requestId from the pending queue,
     *  if the request has not been sent out yet.
     */
    NFErr abortPendingMediaRequest( JsBridgeMediaRequestPtr pMediaRequest );

    void setConfig(ase::IAseConfigParameterPtr  pConfigParameter,
                   ase::IMediaBufferPoolPtr     pBufferPool);
    void resetConfig();

    void bufferReleased( uint32_t requestId, ase::MediaType mediaType, uint32_t requestSize);

    /** Schedule an append to the media source */
    void appendBuffer(NrdpMediaSourcePtr mediaSource, uint32_t srcBufferId,
                      MediaSourceAppendablePtr pAppendable);

    /** Schedule setting the media source buffer timestamp */
    void setSourceBufferTimestampOffset(NrdpMediaSourcePtr mediaSource,
                                        uint32_t srcBufferId,
                                        int32_t timeOffset,
                                        uint32_t timescale);

    /** Flush all the source buffers */
    void completeFlushSourceBuffers( NrdpMediaSourcePtr mediaSource );

    /** Flush a source buffer */
    void completeFlushSourceBuffer( NrdpMediaSourcePtr mediaSource, ase::MediaType mediaType );

    /** Schedule a swapUrl call */
    void swapUrl( JsBridgeMediaRequestPtr pMediaRequest, AseUrl const& url );

    /** Schedule a request for abort */
    void abandonMediaRequest( JsBridgeMediaRequestPtr pMediaRequest );

    /** Add a media source */
    void addMediaSource( NrdpMediaSourcePtr pMediaSource );

    /** Remove a media source */
    void removeMediaSource( NrdpMediaSourcePtr pMediaSource );

    /** Add a download track */
    NFErr createDownloadTrack( ase::MediaType type, ase::TrackConfiguration const& config, uint32_t& trackId );

    /** Pause a download track */
    NFErr pauseDownloadTrack( uint32_t trackId );

    /** Resume a download track */
    NFErr resumeDownloadTrack( uint32_t trackId );

    /** has some download tracks for media */
    bool hasMediaDownloadTracks();

    /** Reconfigure a download track */
    NFErr reconfigureDownloadTrack( uint32_t trackId, ase::TrackConfiguration const& config );

    /** Remove a download track */
    NFErr removeDownloadTrack( uint32_t trackId );

    void printStats();

protected:

    class MediaRequestManagerTask
    {
    public:
        MediaRequestManagerTask() {}

        virtual ~MediaRequestManagerTask() {}

        virtual void invoke( MediaRequestManager* pRequestManager ) = 0;
    };

    typedef shared_ptr<MediaRequestManagerTask> MediaRequestManagerTaskPtr;

    typedef std::queue<MediaRequestManagerTaskPtr> MediaRequestManagerTaskQueue;

    class AppendBufferTask;
    class SwapUrlTask;
    class AbandonRequestTask;
    class SetTimestampTask;
    class SourceBuffersCompleteFlushTask;
    class SourceBufferCompleteFlushTask;

    typedef std::list<JsBridgeMediaRequestPtr> MediaRequestList;

    typedef std::vector<NrdpMediaSourcePtr> NrdpMediaSourceList;

    class DownloadTrackRecord
    {
    public:
        DownloadTrackRecord( ase::MediaType bufferType )
            : mBufferMediaType( bufferType )
        {
        }

        ~DownloadTrackRecord() {}

        ase::MediaType      mBufferMediaType;

        MediaRequestList    mPendingRequests;
    };

    typedef std::map<uint32_t,DownloadTrackRecord> DownloadTrackMap;

    class BufferMonitor
    {
    public:
        BufferMonitor( ase::MediaType mediaType )
            : mMediaType( mediaType )
            , mUnitSize( 0 )
            , mTotalUnits( 0 )
            , mLimitUnits( 0 )
            , mDrainUnits( 0 )
            , mDraining( false )
        {}

        void configure( ase::IMediaBufferPoolPtr    pBufferPool,
                        uint32_t                    sizelimit,
                        uint32_t                    drain );

        void reset();

        /** Returns true if the space could be reserved */
        bool reserve( uint32_t requestid, uint32_t requestSize );

        void debug();

    protected:
        ase::MediaType              mMediaType;
        ase::IMediaBufferPoolPtr    mBufferPoolPtr;
        uint32_t                    mUnitSize;
        uint32_t                    mTotalUnits;
        uint32_t                    mLimitUnits;
        uint32_t                    mDrainUnits;
        bool                        mDraining;
    };

protected:

    void appendBufferImpl(NrdpMediaSourcePtr mediaSource, uint32_t srcBufferId,
                          MediaSourceAppendablePtr pAppendable);

    void setTimestampOffsetImpl(NrdpMediaSourcePtr mediaSource, uint32_t srcBufferId,
                                int32_t timeOffset,uint32_t timescale);

    bool getQueuedRequest(  MediaRequestList&           queue,
                            BufferMonitor*              monitor,
                            JsBridgeMediaRequestPtr&    pRequest );

    DownloadTrackRecord* getDownloadTrackRecord( uint32_t downloadTrackId );
    BufferMonitor* bufferMonitor( ase::MediaType mediaType );

protected:

    mutable Mutex       mMutex;

    uint32_t            mMediaConsumerId;

    DownloadTrackMap    mDownloadTrackMap;

    BufferMonitor       mAudioBufferMonitor;
    BufferMonitor       mVideoBufferMonitor;

    uint32_t            mPendingRequests;
    uint32_t            mPendingHeaderRequests;

    bool                mDownloadingPaused;

    ullong              mAccumulatedBytesScheduled;

    MediaRequestManagerListenerPtr mListener;

    ase::IPersistentStorePtr mPersistentStorePtr;

    MediaRequestManagerTaskQueue mTaskQueue;

    NrdpMediaSourceList mNrdpMediaSources;

    NrdpMediaSourceList mRemovedNrdpMediaSources;

    std::vector<JsBridgeMediaRequestPtr> mRequestsToFlush;

};

typedef MediaRequestManager::MediaRequestManagerPtr MediaRequestManagerPtr;

inline MediaRequestManager::BufferMonitor* MediaRequestManager::bufferMonitor( ase::MediaType mediaType )
{
    return ( mediaType == ase::MEDIA_AUDIO ) ? &mAudioBufferMonitor
            : ( mediaType == ase::MEDIA_VIDEO ) ? &mVideoBufferMonitor
            : 0;
}

}}


#endif
