/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef URL_REQUEST_TASK_H
#define URL_REQUEST_TASK_H

#include <string>
#include <list>

#include <nrdbase/StdInt.h>
#include <nrdnet/HttpResponseHeaders.h>

#include <nrdnet/AseTimeVal.h>
#include <nrdase/MediaRequestTask.h>
#include <nrdase/ITransportReporter.h>

#include <nrdase/ByteRanges.h>

namespace netflix {
namespace ase {

/**
 *  @class UrlRequestTask
 *
 *  @brief This class represents a URL request.
 */
class UrlRequestTask
{
public:
    typedef shared_ptr<UrlRequestTask> UrlRequestTaskPtr;

    /** Constructor. */
    UrlRequestTask( uint32_t id, MediaRequestTaskPtr pMediaRequestTask );

    /** Destructor. */
    virtual ~UrlRequestTask();

    /** Set the URL request task ID. */
    void setId( uint32_t id );

    /** Get the URL request task ID. */
    uint32_t getId() const;

    /** Get the media type for this request */
    //MediaType getMediaType() const;

    /** Get the stream key for this request. */
    //const std::string& getStreamKey() const;

    /** Get the media request task. */
    MediaRequestTaskPtr getMediaRequestTask();

    /** Get the media track ID assicated with the media request. */
    uint32_t getDownloadTrackId();

    /** Get the current URL for the task */
    AseUrl const& getCurrentUrl() const;

    /** Set the streaming location set */
    //void setStreamingLocationSet( IStreamingLocationSetPtr pStreamingLocationSet );

    /** Get the streaming location set */
    //IStreamingLocationSetPtr getStreamingLocationSet() const;

    /** Set the selected manifest URLs. */
    //void setManifestUrl( ConstManifestUrlPtr pManifestUrl );

    /** Get the selected manifest URLs. */
    //ConstManifestUrlPtr getManifestUrl() const;

    /** Get the throughput monitor (may be null) */
    IThroughputMonitorPtr getThroughputMonitor() const;

    /** Test if this task is abandoned */
    bool isAbandoned() const;

    /** Test if this task has an assigned url */
    bool hasUrl() const;

    /** Perform URL selection for this task */
    bool selectUrl();

    /** Prepend a request byte range list. */
    //void prependByteRangeList( const ByteRangeList& byteRangeList );

    /** Add another request in to this one - returns true if sucessful, false if not */
    bool add( UrlRequestTaskPtr pOther );

    /** Regenerate the request byte range list.
     *
     *  This function re-generate the unscheduled request byte range list from the recieved byte
     *  ranges. This assumes no requests are scheduled.
     */
    void reset();

    /** Get and update the unscheduled data length
     *  The unscheduled data length may be updated here if duplicate ranges have been returned and
     *  data has arrived since then
     */
    llong updateUnscheduledDataLength();

    /** Peek a request byte range based on the length of the data to be requested.
     *
     *  The returned byte range may be shorter than the request data length. This could happen if
     *  the request has been fragmented and the first available byte range is shorter than the
     *  request data length.
     *
     *  @param requestDataLength [In] The request data length.
     *  @param byteRange [Out] The returned byte range.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR on unknow error
     *          AS_NOT_FOUND if no byte range is available.
     */
    int32_t peekByteRange(
                    uint32_t const requestDataLength,
                    ByteRange& byteRange );

    /** Get a request byte range based on the length of the data to be requested.
     *
     *  The returned byte range may be shorter than the request data length. This could happen if
     *  the request has been fragmented and the first available byte range is shorter than the
     *  request data length.
     *
     *  @param requestDataLength [In] The request data length.
     *  @param range [Out] The returned byte range.
     *
     *  @return AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR on unknow error
     *          AS_NOT_FOUND if no byte range is available.
     */
    int32_t getByteRange(
                    uint32_t const      requestDataLength,
                    ByteRange& range );

    /** Returns an unrequested byte range
     *
     *  @param byteRange [In] The returned byte range.
     *  @param duplicated [in] true if these bytes are being duplicated due to a slow response
     */
    void returnByteRange( ByteRange const& range, bool duplicated = false );

    /** Returns unrequested byte ranges.
     *
     *  @param byteRanges [In] The returned byte ranges.
     *  @param duplicated [in] true if these bytes are being duplicated due to a slow response
     */
    void returnByteRanges( ByteRanges const& ranges, bool duplicated = false );

    /** Get a buffer to store received data. It's assumed that the data available
     *  immediately follows the last committed data.
     *
     *  @param range [in] The byte range for which data is available
     *  @param pBuffer [out] The pointer to the start of the buffer for receive data.
     *                       If this pointer is NULL, that means no buffer is available, the user
     *                       should retry to get the buffer at a later time.
     *  @param skipBytes [out] Number of bytes of input data to skip before writing to the buffer
     *                         (this data has been already received from another request)
     *  @param bufferSize [out] The size of the buffer.
     *  @param bufferRecordIndex [out] The index of the buffer record.
     *
     *  @return AS_NO_ERROR on succeess
     *          AS_UNKNOWN_ERROR on unknow error
     *          AS_INVALID_VALUE on invalid input values.
     *          AS_AGAIN if no buffer is available, the user then should retry to get the buffer at
     *                   a later time.
     */
    int32_t getBuffer(
                ByteRange const& range,
                uint32_t& skipBytes,
                unsigned char*& pBuffer,
                uint32_t& bufferSize,
                uint32_t& bufferRecordIndex );

    /** Indicate bytes skipped for a request
     *
     *  @param range [in] The byte range for which bytes are skipped
     *  @param skipBytes [in] The number of bytes skipped
     *
     *  @return AS_NO_ERROR on succeess
     *          AS_UNKNOWN_ERROR on unknow error
     *          AS_INVALID_VALUE on invalid input values.
     */
    int32_t commitSkippedBytes(  ByteRange const&    range );


    /** Commit data to the previously obtained buffer
     *
     *  @param range [in] The byte range for which bytes are committed
     *  @param dataLength [in] The received data length, including any skipped bytes
     *  @param bufferRecordIndex [in] The buffer record index obtained from getBuffer().
     *
     *  @return AS_NO_ERROR on succeess
     *          AS_UNKNOWN_ERROR on unknow error
     */
    int32_t commitReceivedBytes(
                ByteRange const&    range,
                uint32_t const              bufferRecordIndex );

    /** Provide HTTP response headers for a request contributing to this UrlRequestTask */
    void provideResponseHeaders( HttpResponseHeaders headers );

    /** Do we have requests in progress */
    bool hasRequestsInProgress() const;

    /** Get the unscheduled data length. */
    llong getUnscheduledDataLength() const;

    /** Check if this URL request task is fully scheduled. */
    bool scheduled() const;

    /** Check if this URL request task is completed. */
    bool completed() const;

    /** Check if byte ranges from this were every duplicated */
    bool duplicated() const;

    /** Check if a specific byte range has been received */
    bool received( ByteRange const& range ) const;

    /** Truncate this URL request task, splitting off the remainder into a separate task
     * @param [in] range The task will keep the intersection of this range and its current ranges
     * @param [out] pRemainingTask A new UrlRequestTask for ranges outside the provided range
     *                              This task will have id zero
     * @return AS_NO_ERROR on success
     *         AS_NOT_SUPPORTED if the task could not be split because this would require splitting
     *                          of an already scheduled byte range
     *         AS_NOT_ENOUGH_MEMORY if the new task object could not be created
     */
    int32_t truncate( ByteRange const& range, UrlRequestTaskPtr& pRemainingTask );

    /** Get the time at which this request should be completed */
    AseTimeVal getCompletionDeadline() const;

    /** Get a debug string listing the unscheduled byte ranges */
    std::string getByteRangesDebugString( ) const
        { return getByteRangesDebugString( mUnscheduledRanges ); }
    static std::string getByteRangesDebugString( ByteRanges const& ranges );

protected:
    /** Copy constructor - DOES NOT copy byte ranges */
    UrlRequestTask( UrlRequestTask const& other, ByteRange const& range );

    /** Print out the byte range records, mainly for debug purpose. */
    void printByteRanges();

protected:
    /** The URL request task ID. */
    uint32_t mId;

    /** The media request task. */
    MediaRequestTaskPtr mMediaRequestTaskPtr;

    /** The current streaming location set */
    //IStreamingLocationSetPtr mStreamingLocationSetPtr;

    /** The current URL */
    AseUrl mCurrentUrl;

    /** The originally requested data length */
    ByteRange mOriginalRequestedByteRange;

    /** Do we have requests in progress */
    bool mHasRequestsInProgress;

    /** The unscheduled byte ranges */
    ByteRanges mUnscheduledRanges;

    /** The received byte ranges */
    ByteRanges mReceivedByteRanges;

    /** Indictates whether any bytes have ever been duplicated for this request */
    bool mDuplicated;
};

typedef UrlRequestTask::UrlRequestTaskPtr UrlRequestTaskPtr;

typedef std::list<UrlRequestTaskPtr> UrlRequestTaskList;

/** Set the URL request task ID. */
inline void UrlRequestTask::setId( uint32_t id )
{
    mId = id;
}

/** Get the URL request task ID. */
inline uint32_t UrlRequestTask::getId() const
{
    return mId;
}

#if 0
inline MediaType UrlRequestTask::getMediaType() const
{
    return mMediaRequestTaskPtr->getMediaType();
}

/** Get the stream key for this request. */
inline const std::string& UrlRequestTask::getStreamKey() const
{
    return mMediaRequestTaskPtr->getStreamKey();
}
#endif

/** Get the media request task. */
inline MediaRequestTaskPtr UrlRequestTask::getMediaRequestTask()
{
    return mMediaRequestTaskPtr;
}

/** Get the media track ID assicated with the media request. */
inline uint32_t UrlRequestTask::getDownloadTrackId()
{
    return mMediaRequestTaskPtr->getMediaRequest()->getDownloadTrackId();
}

/** Get the current URL for the task */
inline AseUrl const& UrlRequestTask::getCurrentUrl() const
{
    return mCurrentUrl;
}

#if 0
/** Set the streaming location set */
inline void UrlRequestTask::setStreamingLocationSet( IStreamingLocationSetPtr pStreamingLocationSet )
{
    mStreamingLocationSetPtr = pStreamingLocationSet;
}

/** Get the streaming location set */
inline IStreamingLocationSetPtr UrlRequestTask::getStreamingLocationSet() const
{
    return mStreamingLocationSetPtr;
}

/** Set the manifest URLs */
inline void UrlRequestTask::setManifestUrl( ConstManifestUrlPtr pManifestUrl )
{
    mManifestUrl = pManifestUrl;
}

/** Get the selected manifest URLs. */
inline ConstManifestUrlPtr UrlRequestTask::getManifestUrl() const
{
    return mManifestUrl;
}
#endif

inline IThroughputMonitorPtr UrlRequestTask::getThroughputMonitor() const
{
    return mMediaRequestTaskPtr->getMediaRequest()->getThroughputMonitor();
}

/** Test if this task is abandoned */
inline bool UrlRequestTask::isAbandoned() const
{
    return mMediaRequestTaskPtr->isAbandoned();
}

inline bool UrlRequestTask::hasUrl() const
{
    return mMediaRequestTaskPtr->getMediaRequest()->hasUrl();
}

inline bool UrlRequestTask::selectUrl()
{
    MediaRequestPtr const pMediaRequest = mMediaRequestTaskPtr->getMediaRequest();
    if ( pMediaRequest->selectUrl() )
    {
        if ( pMediaRequest->hasUrl() )
            mCurrentUrl = mMediaRequestTaskPtr->getMediaRequest()->getCurrentUrl();
        else
            mCurrentUrl = AseUrl();

        return true;
    }

    return false;
}

/** Do we have requests in progress */
inline bool UrlRequestTask::hasRequestsInProgress() const
{
    return mHasRequestsInProgress;
}

/** Get the unscheduled data length. */
inline llong UrlRequestTask::getUnscheduledDataLength() const
{
    return mUnscheduledRanges.size();
}

/** Check if this URL request task is fully scheduled. */
inline bool UrlRequestTask::scheduled() const
{
    return (mUnscheduledRanges.size() == 0);
}

/** Check if this URL request task is completed. */
inline bool UrlRequestTask::completed() const
{
    return (mReceivedByteRanges.contiguous()
                && mReceivedByteRanges.size() == mOriginalRequestedByteRange.size());
}

inline bool UrlRequestTask::duplicated() const
{
    return mDuplicated;
}

inline bool UrlRequestTask::received( ByteRange const& range ) const
{
    return mReceivedByteRanges.contains( range );
}

inline AseTimeVal UrlRequestTask::getCompletionDeadline() const
{
    return mMediaRequestTaskPtr->getCompletionDeadline();
}

inline void UrlRequestTask::provideResponseHeaders( HttpResponseHeaders headers )
{
    mMediaRequestTaskPtr->provideResponseHeaders( headers );
}

}}
#endif //URL_REQUEST_TASK_H
