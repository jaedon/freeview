/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MEDIA_REQUEST_TASK_H
#define MEDIA_REQUEST_TASK_H

#include <nrdbase/StdInt.h>

#include <list>

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/ByteRange.h>
#include <nrdase/IMediaConsumer.h>

#include <nrdnet/HttpResponseHeaders.h>

namespace netflix {
namespace ase {

typedef std::list<ByteRange> ByteRangeList;

/**
 *  @class MediaRequestTask
 *
 *  @brief This class represents a media request from media consumers.
 */
class MediaRequestTask
{
public:
    typedef shared_ptr<MediaRequestTask> MediaRequestTaskPtr;

    /** The buffer record. This is a record of the buffer for storing response body data. The
     *  actual buffer is not allocated until the buffer is actually needed.
     */
    struct BufferRecord
    {
        /** The pointer to the actual buffer allocated from MediaRequest. */
        AseBufferPtr mAseBufferPtr;

        /** The amount of data expected to be filled in this buffer. */
        uint32_t mExpectedDataAmount;

        /** The amount of data that has been filled in this buffer, it increases
         *  as data is filledinto the buffer. */
        uint32_t mFilledDataAmount;

        /* constructor */
        BufferRecord() : mExpectedDataAmount( 0 ), mFilledDataAmount( 0 ) { }
    };

public:
    /** Constructor. */
    MediaRequestTask( uint32_t id, MediaRequestPtr pMediaRequest );

    /** Destructor. */
    virtual ~MediaRequestTask();

    /** Get the media type for this request */
    //MediaType getMediaType() const;

    /** Get the stream key for this request. */
    //const std::string& getStreamKey() const;

    /** Get the media request task ID. */
    uint32_t getId() const;

    /** Get the abort timeout for this task - this is the time by which more data must be received */
    AseTimeVal getAbandonTime() const;

    /** Test if this request is abandoned */
    bool isAbandoned() const;

    /** Get the original media request. */
    MediaRequestPtr getMediaRequest();

    /** report that work on the request has started */
    void requestStarted( bool connecting, bool pipelined );

    /** report a fatal error for this request */
    void requestError( AseErrorCode status, uint32_t httpCode );

    /** Get a buffer to store received data.
     *
     *  @param requestRange [in] The actual HTTP request range.
     *  @param offset [in] Offset into the byte range of data that is now available
     *  @param pBuffer [out] The pointer to the start of the buffer to receive data.
     *                       If this pointer is NULL, that means no buffer is available, the user
     *                       should retry to get the buffer at a later time.
     *  @param bufferSize [out] The size of the buffer.
     *  @param bufferRecordIndex [Out] The index of the buffer record.
     *
     *  @return AS_NO_ERROR on succeess
     *          AS_UNKNOWN_ERROR on unknow error
     *          AS_INVALID_VALUE on invalid input values.
     *          AS_AGAIN if no buffer is available, the user then should retry to get the buffer at
     *                   a later time.
     */
    int32_t getBuffer(
            const ByteRange& requestRange,
            llong receptionOffset,
            unsigned char*& pBuffer,
            uint32_t& bufferSize,
            uint32_t& bufferRecordIndex );

    /** Commit the buffer that stores received data.
     *
     *  @param dataLength [In] The received data length.
     *  @param bufferRecordIndex [In] The buffer record index obtained from getBuffer().
     *
     *  @return AS_NO_ERROR on succeess
     *          AS_UNKNOWN_ERROR on unknow error
     */
    int32_t commitBuffer(
                uint32_t dataLength,
                uint32_t bufferRecordIndex );

    /** Provide headers associated with this media request */
    void provideResponseHeaders( HttpResponseHeaders headers );

    /** Get the time at which this request should be completed */
    AseTimeVal getCompletionDeadline() const;

protected:

    /** The media request task ID. */
    uint32_t                    mId;

    /** The media request from media consumers. */
    MediaRequestPtr             mMediaRequestPtr;

    /** The buffer record arrary pointer. */
    std::vector<BufferRecord>   mBufferRecordArray;

    /** Time by which more data must be received */
    AseTimeVal                  mAbandonTime;

    /** Time by which the whole request should be received */
    AseTimeVal                  mCompletionDeadline;

    /** True if request start has been reported to the MediaRequest */
    bool                        mRequestStarted;

    /** True if HTTP response headers have been reported to the MediaRequest */
    bool                        mHeadersReported;

    /** Total bytes received */
    size_t                      mBytesReceived;

    /** Total request bytes */
    size_t                      mRequestSize;

    /** Progress interval */
    size_t                      mProgressInterval;

    /** When to send next progress report */
    size_t                      mNextProgress;
};

typedef MediaRequestTask::MediaRequestTaskPtr MediaRequestTaskPtr;

inline AseTimeVal MediaRequestTask::getAbandonTime() const
{
    return mAbandonTime;
}

inline bool MediaRequestTask::isAbandoned() const
{
    return mMediaRequestPtr->isAbandoned();
}

inline AseTimeVal MediaRequestTask::getCompletionDeadline() const
{
    return mCompletionDeadline;
}


}}
#endif //MEDIA_REQUEST_TASK_H
