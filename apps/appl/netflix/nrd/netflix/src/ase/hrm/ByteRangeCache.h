/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BYTE_RANGE_CACHE_H
#define BYTE_RANGE_CACHE_H

#include <nrdbase/StdInt.h>

#include <list>

#include "AseCommonDataTypes.h"
#include "ByteRange.h"
#include "IMediaConsumer.h"
#include "UrlRequestTask.h"

namespace netflix {
namespace ase {

/**
 * @class ByteRangeCache
 * @brief MediaRequest object for cache of early-requested data to get
 *        streaming started before the MediaPresentation has fully
 *        made up its mind what to request.
 *
 * This object can be wrapped in a MediaRequestTask and the UrlRequestTask
 * in the usual way to be provided to the track downloaders. Data returned
 * to this object will be cached and used to serve other UrlRequestTasks.
 *
 * A UrlRequestTask can be passed 'through' the cache, causing any relevant
 * data in the cache to be used to fulfill the task and any part of the task
 * that will later be fulfilled by the ByteRangeCache being queued in the
 * ByteRangeCache. Any part that will not be served by the ByteRangeCache
 * will be handed back.
 *
 * As TrackDownloaders request new UrlRequestTasks, the resulting tasks will
 * be passed through the cache by the HttpRequestManager before being handed
 * on to the track downloader.
 *
 * TBD: whether the ByteRangeCache needs to trigger fetching of new tasks
 * from the IMediaConsumer on completion of UrlRequestTasks, or whether the
 * pulls from the TrackDownloader will be sufficient.
 *
 */
class ByteRangeCache : public MediaRequest
{
public:
    typedef shared_ptr<ByteRangeCache> ByteRangeCachePtr;

    /** Construct a new ByteRangeCache
     *  @param [in] trackId             The track id
     *  @param [in] pInitialTask        The initial request on which to base
     *                                  the turbo cache
     *  @param [in] turboRequestSize    The size of the turbo cache (must be at
     *                                  least big enough for the initial task)
     *  @param [out] pByteRangeCache The newturbo cache request object
     *  @return AS_NO_ERROR on success or error code
     *          The ByteRangeCache object will own the initial task
     *          provided
     */
    static AseErrorCode construct(
                        uint32_t                trackId,
                        UrlRequestTaskPtr       pInitialTask,
                        uint32_t                turboRequestSize,
                        ByteRangeCachePtr&   pByteRangeCache );

    /** Serve a task from cache
     *  @param [in] pUrlRequestTask The task to serve from cache if possible
     *  @param [out] pRemainingUrlRequestTask Remainder of the task if it was partially served from cache
     *  @return true if the request was fully or partially served, false otherwise
     *
     *  The ByteRangeCache takes ownership of the provided task, and may provide
     *  a remaining task back.
     */
    bool serve( UrlRequestTaskPtr pUrlRequestTask, UrlRequestTaskPtr& pRemainingUrlRequestTask );

    /** Test if all tasks are completely fulfilled and all data received */
    bool complete() const { return mTasks.empty() && mCacheBuffersOutstanding == 0; }

    /** Get the track id */
    //uint32_t getTrackId() const { return mTrackId; }

    /*********** MediaRequest methods *****************/

    /** Get the URLs for the request */
    //virtual ManifestUrls const& getUrls() const;

    /** Get the byte range for the request */
    virtual ByteRange const& getByteRange( ) const;

    /** Get the size of response body buffers
     * For example, for IMediaBuffer, it is the size of a buffer block.
     */
    virtual size_t getBufferSize() const;

    /** Get a buffer for the response body - all provided buffers
     *  must have the same size
     */
    virtual AseErrorCode getResponseBodyBuffer(AseBufferPtr& pBuffer);

    /** Indicate first request started
     *  @param[in] connecting true if a new connection is being established
     *  @param[in] pipelined true if the request is pipelined on an existing connection
     */
    virtual void requestStarted( bool connecting, bool pipelined );

    /** Provide the MEDIA response body
     *
     *  @param[in] pBuffer  The buffer that contains the response
     *                      body data. This may not contain the
     *                      complete body, but will be full if this
     *                      is not the last portion of the response.
     *  @param[in] sequence The sequence number of the provided buffer
     *                      (body data may not be provided in sequence)
     *  @return: negative error codes; 1 if the request is completed
     */
    virtual int provideResponseBody(    AseBufferPtr const& pBuffer,
                                        int sequence,
                                        AseTimeVal& abandontime );

    /** Indicate request abandoned */
    virtual void abandoned();

    /** Indicate a fatal request error */
    virtual void requestError( AseErrorCode status, uint32_t httpCode );

    /** Indicate that the file size has been determined */
    virtual void provideFileSize( ullong size );

    virtual AseUrl getCurrentUrl() { return mMediaRequestPtr->getCurrentUrl(); }
    virtual bool selectUrl() { return mMediaRequestPtr->selectUrl(); }
    virtual bool hasUrl() const { return mMediaRequestPtr->hasUrl(); }
    virtual IThroughputMonitorPtr getThroughputMonitor() const { return mMediaRequestPtr->getThroughputMonitor(); }

    //virtual AseErrorCode getRequestData(DataBuffer& data) { return mMediaRequestPtr->getRequestData(data); }

    virtual void loadStarted() { return; }

protected:
    ByteRangeCache(  uint32_t trackId,
                    UrlRequestTaskPtr       pInitialTask,
                        uint32_t                turboRequestSize );

    void addTask( UrlRequestTaskPtr pUrlRequestTask );

    void serveDataToTasks( int first, int last, AseTimeVal& abandontime );

protected:
    struct TaskRecord
    {
        typedef std::vector<std::pair<ByteRange,uint32_t> > ByteRanges;

        UrlRequestTaskPtr       mUrlRequestTaskPtr;
        ByteRanges              mByteRanges;

        TaskRecord( UrlRequestTaskPtr pUrlRequestTask )
            : mUrlRequestTaskPtr( pUrlRequestTask )
        {}
    };

    typedef std::list<TaskRecord> TaskRecordList;

    uint32_t const              mByteRangeCacheSize;

    size_t const                mBufferSize;

    ByteRange                   mByteRange;

    MediaRequestPtr             mMediaRequestPtr;

    TaskRecordList              mTasks;

    std::vector<AseBufferPtr>   mCacheBuffers;

    uint32_t                    mCacheBuffersOutstanding;
};

typedef ByteRangeCache::ByteRangeCachePtr ByteRangeCachePtr;

} // namespace ase
} // namespace netflix

#endif /* BYTE_RANGE_CACHE_H */
