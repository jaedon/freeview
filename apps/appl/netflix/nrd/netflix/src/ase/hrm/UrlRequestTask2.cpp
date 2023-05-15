/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "UrlRequestTask.h"

#include "DebugLogging.h"



namespace netflix {
namespace ase {


DECLARE_LOGGER( HttpRequestManager );

/**
 *  @class UrlRequestTask
 */

/** Constructor. */
UrlRequestTask::UrlRequestTask(
                    uint32_t id,
                    MediaRequestTaskPtr pMediaRequestTask )
                                                : mId( id )
                                                , mMediaRequestTaskPtr( pMediaRequestTask )
                                                , mHasRequestsInProgress( false )
                                                , mDuplicated( false )
{
    mCurrentUrl = mMediaRequestTaskPtr->getMediaRequest()->getCurrentUrl();
    mOriginalRequestedByteRange = pMediaRequestTask->getMediaRequest()->getByteRange();
    mUnscheduledRanges = mOriginalRequestedByteRange;
}

/** Destructor. */
UrlRequestTask::~UrlRequestTask()
{
}

bool UrlRequestTask::add( UrlRequestTaskPtr pOther )
{
    if ( mMediaRequestTaskPtr->getId() != pOther->getMediaRequestTask()->getId() )
        return false;

    mUnscheduledRanges |= pOther->mUnscheduledRanges;

    return true;
}

void UrlRequestTask::reset()
{
    ByteRanges const ranges = mOriginalRequestedByteRange - mReceivedByteRanges;

    DEBUGLOG( HttpRequestManager, "UrlRequestTask: %u, reset: unscheduled: %s", mId, getByteRangesDebugString( ranges ).c_str());

    mUnscheduledRanges = ranges;
    mHasRequestsInProgress = false;
}

/** Get and update the unscheduled data length */
llong UrlRequestTask::updateUnscheduledDataLength()
{
    if ( mDuplicated )
    {
        mUnscheduledRanges -= mReceivedByteRanges;
    }

    return getUnscheduledDataLength();
}

/** Peek a request byte range based on the length of the data to be requested. */
int32_t UrlRequestTask::peekByteRange(
                            uint32_t const requestDataLength,
                            ByteRange& byteRange )
{
    if ( mUnscheduledRanges.empty() )
        return AS_NOT_FOUND;

    byteRange = mUnscheduledRanges.front();

    if ( byteRange.size() > requestDataLength )
    {
        byteRange.setEnd( byteRange.start() + requestDataLength - 1 );
    }

    return AS_NO_ERROR;
}

/** Get a request byte range based on the length of the data to be requested. */
int32_t UrlRequestTask::getByteRange(
                            uint32_t const      requestDataLength,
                            ByteRange&          range )
{
    int32_t retVal = peekByteRange( requestDataLength, range );
    if ( retVal != AS_NO_ERROR )
        return retVal;

    mUnscheduledRanges -= range;

    mHasRequestsInProgress = true;

    DEBUGLOG(
        HttpRequestManager,
        "UrlRequestTask: %u Providing a byte range: %lld-%lld",
        mId,
        range.getStart(),
        range.getEnd() );

    //printByteRanges();

    return AS_NO_ERROR;
}

/** Returns an unrequested byte range. */
void UrlRequestTask::returnByteRange( ByteRange const& range, bool duplicated )
{
    mUnscheduledRanges |= range;

    mDuplicated = mDuplicated || duplicated;

    DEBUGLOG(
        HttpRequestManager,
        "UrlRequestTask: %u returned byte ranges %lld-%lld%s",
        mId,
        range.getStart(),
        range.getEnd(),
        duplicated ? " (duplicated)" : "" );

    //printByteRanges();
}


/** Returns unrequested byte ranges. */
void UrlRequestTask::returnByteRanges( ByteRanges const& ranges, bool duplicated )
{
    mUnscheduledRanges |= ranges;

    mDuplicated = mDuplicated || duplicated;

    DEBUGLOG(
        HttpRequestManager,
        "UrlRequestTask: %u returned byte ranges %s%s",
        mId,
        getByteRangesDebugString( ranges ).c_str(),
        duplicated ? " (duplicated)" : "" );

    //printByteRanges();

}

/** Get a buffer to store received data. */
int32_t UrlRequestTask::getBuffer(
                            ByteRange const& range,
                            uint32_t& skipBytes,
                            unsigned char*& pBuffer,
                            uint32_t& bufferSize,
                            uint32_t& bufferRecordIndex )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    uint32_t maximumBuffer = range.size();

    if( !mReceivedByteRanges.disjoint( range ) )
    {
        ByteRanges const needed = range - mReceivedByteRanges;
        if ( needed.empty() )
        {
            skipBytes = maximumBuffer;
            pBuffer = 0;
            maximumBuffer = 0;
        }
        else
        {
            skipBytes = needed.front().start() - range.start();
            maximumBuffer = needed.front().size();
        }
    }
    else
    {
        skipBytes = 0;
    }

    if ( maximumBuffer > 0 )
    {
        retVal = mMediaRequestTaskPtr->getBuffer(   ByteRange( range.start(), range.start() + skipBytes + maximumBuffer - 1 ),
                                                    skipBytes,
                                                    pBuffer,
                                                    bufferSize,
                                                    bufferRecordIndex );
    }
    else
    {
        pBuffer = 0;
        bufferSize = 0;
        bufferRecordIndex = 0;
        retVal = AS_NO_ERROR;
    }

/*
    DEBUGLOG(
        HttpRequestManager,
        "UrlRequestTask: %u, returning buffer size %u skip %u for range %lld-%lld, received %s",
        mId,
        bufferSize,
        skipBytes,
        range.getStart(),
        range.getEnd(),
        getByteRangesDebugString( mReceivedByteRanges ).c_str() );
*/

    return retVal;
}


/** Commit skipped bytes (already recieved on another request. */
int32_t UrlRequestTask::commitSkippedBytes(
                            ByteRange const&    range )
{
    NRDP_UNUSED( range );
    /*
    DEBUGLOG(
        HttpRequestManager,
        "UrlRequestTask: %u, skipped duplicate range %lld-%lld, received %s",
        mId,
        range.getStart(),
        range.getEnd(),
        getByteRangesDebugString( mReceivedByteRanges ).c_str() );
    */
    return AS_NO_ERROR;
}

/** Commit data to the buffer that stores received data. */
int32_t UrlRequestTask::commitReceivedBytes(
                            ByteRange const&             range,
                            uint32_t const              bufferRecordIndex )
{
    //Commit the buffer.
    mMediaRequestTaskPtr->commitBuffer( range.size(), bufferRecordIndex );

    assert( mReceivedByteRanges.disjoint( range ) );

    mReceivedByteRanges.append( range );

    /*
    DEBUGLOG(
        HttpRequestManager,
        "UrlRequestTask: %u, committed range %lld-%lld, received %s",
        mId,
        range.getStart(),
        range.getEnd(),
        getByteRangesDebugString( mReceivedByteRanges ).c_str() );
    */

    return AS_NO_ERROR;
}

/** Truncate this URL request task, splitting off the remainder into a separate task */
int32_t UrlRequestTask::truncate( ByteRange const& range, UrlRequestTaskPtr& pRemainingTask )
{
    assert( mReceivedByteRanges.empty() );
    assert( mUnscheduledRanges == mOriginalRequestedByteRange );

    ByteRange const truncated = mOriginalRequestedByteRange & range;
    ByteRanges const remaining = mOriginalRequestedByteRange - truncated;

    assert( !truncated.empty() );
    assert( remaining.empty() || remaining.contiguous() );

    mOriginalRequestedByteRange = truncated;
    mUnscheduledRanges = truncated;

    if ( !remaining.empty() )
    {
        pRemainingTask = UrlRequestTaskPtr( new UrlRequestTask( *this, remaining.front() ) );
        if ( !pRemainingTask )
        {
            return AS_NOT_ENOUGH_MEMORY;
        }
    }
    else
    {
        pRemainingTask.reset();
    }

    return AS_NO_ERROR;
}

UrlRequestTask::UrlRequestTask( UrlRequestTask const& other, ByteRange const& range )
    : mId( 0 )
    , mMediaRequestTaskPtr( other.mMediaRequestTaskPtr )
//    , mStreamingLocationSetPtr( other.mStreamingLocationSetPtr )
//    , mManifestUrl( other.mManifestUrl )
    , mCurrentUrl( other.mCurrentUrl )
    , mOriginalRequestedByteRange( range )
    , mHasRequestsInProgress( false )
    , mUnscheduledRanges( range )
{
}

/** Print out the byte range records, mainly for debug purpose. */
void UrlRequestTask::printByteRanges()
{
    DEBUGLOG( HttpRequestManager, "UrlRequestTask: %u, unscheduled: %s", mId, getByteRangesDebugString( mUnscheduledRanges ).c_str());
    DEBUGLOG( HttpRequestManager, "UrlRequestTask: %u, received: %s", mId, getByteRangesDebugString( mReceivedByteRanges ).c_str());
}

std::string UrlRequestTask::getByteRangesDebugString( ByteRanges const& ranges )
{
    std::ostringstream oss;
    ByteRanges::const_iterator it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
        if ( it != ranges.begin() ) oss << ',';
        oss << '[' << it->start() << '-' << it->end() << ']';
    }
    return oss.str();
}

}}
