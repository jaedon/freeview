/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DebugLogging.h"
#include "MediaRequestTask.h"

using namespace netflix;
using namespace netflix::ase;

DEFINE_LOGGER( MediaRequestTask );

/**
 *  @class MediaRequestTask
 */

/** Constructor. */
MediaRequestTask::MediaRequestTask( uint32_t id, MediaRequestPtr pMediaRequest ) :
                                                            mId( id ),
                                                            mMediaRequestPtr( pMediaRequest ),
                                                            mAbandonTime( AseTimeVal::INFINITE ),
                                                            mCompletionDeadline( pMediaRequest->getCompletionDeadline() ),
                                                            mRequestStarted( false ),
                                                            mHeadersReported( false ),
                                                            mBytesReceived( 0 ),
                                                            mRequestSize( mMediaRequestPtr->getByteRange().getSize() ),
                                                            mProgressInterval( pMediaRequest->getProgressInterval() ),
                                                            mNextProgress( mProgressInterval )
{
    uint32_t mediaBufferSize = mMediaRequestPtr->getBufferSize();
    uint32_t nBufferRecords = (uint32_t) (mRequestSize / ((llong) mediaBufferSize));

    //Initialize the buffer record array.
    if ((mRequestSize % ((llong) mediaBufferSize)) != 0)
    {
        ++nBufferRecords;
    }

    mBufferRecordArray.resize( nBufferRecords );

    uint32_t count;

    for (count = 0; count < mBufferRecordArray.size(); count++)
    {
        mBufferRecordArray[count].mExpectedDataAmount = mediaBufferSize;
    }

    if ((mRequestSize % ((llong) mediaBufferSize)) != 0)
    {
        mBufferRecordArray[count - 1].mExpectedDataAmount = (uint32_t) (mRequestSize % ((llong) mediaBufferSize));
    }
}

/** Destructor. */
MediaRequestTask::~MediaRequestTask()
{
}

#if 0
/** Get the media type for this request */
MediaType MediaRequestTask::getMediaType() const
{
    return mMediaRequestPtr->getMediaType();
}

/** Get the stream key for this request. */
const std::string& MediaRequestTask::getStreamKey() const
{
    return mMediaRequestPtr->getStreamKey();
}
#endif

/** Get the media request task ID. */
uint32_t MediaRequestTask::getId() const
{
    return mId;
}

/** Get the original media request. */
MediaRequestPtr MediaRequestTask::getMediaRequest()
{
    return mMediaRequestPtr;
}

/** report that work on the request has started */
void MediaRequestTask::requestStarted( bool connecting, bool pipelined )
{
    if ( !mRequestStarted )
    {
        mRequestStarted = true;

        mMediaRequestPtr->requestStarted( connecting, pipelined );
    }
}

void MediaRequestTask::requestError( AseErrorCode status, uint32_t httpCode )
{
    mMediaRequestPtr->requestError( status, httpCode );
}

/** Get a buffer to store received data. */
int32_t MediaRequestTask::getBuffer(
                            const ByteRange& requestRange,
                            llong receptionOffset,
                            unsigned char*& pBuffer,
                            uint32_t& bufferSize,
                            uint32_t& bufferRecordIndex )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    uint32_t const mediaBufferSize = mMediaRequestPtr->getBufferSize();

    //Work out the buffer record from the request range and received data length.

    assert( mMediaRequestPtr->getByteRange().contains( requestRange ) );

    // Offset of the to-be-received data into the media requests byte range
    llong const rangeOffset = receptionOffset + requestRange.start() - mMediaRequestPtr->getByteRange().start();

    // Index of the buffer that contains this start point
    uint32_t index = rangeOffset / mediaBufferSize;

    // Offset of the buffer into the media request's byte range
    llong bufferOffset = (llong) index * mediaBufferSize;

    // Offset into the buffer of the to-be-received data
    llong offset = rangeOffset - bufferOffset;

    assert( offset >= 0 && offset < mediaBufferSize );

    if (index < mBufferRecordArray.size())
    {
        //Allocate the buffer if it has not been allocated.
        if (!mBufferRecordArray[index].mAseBufferPtr)
        {
            mMediaRequestPtr->getResponseBodyBuffer( mBufferRecordArray[index].mAseBufferPtr );

            if (mBufferRecordArray[index].mAseBufferPtr)
            {
                assert( index == 0 ||  mBufferRecordArray[index].mAseBufferPtr->empty() );
            }
        }

        if (mBufferRecordArray[index].mAseBufferPtr)
        {
            //Get the pointer to the next-to-be provided byte in this buffer
            pBuffer = mBufferRecordArray[index].mAseBufferPtr->getFreeSpaceStart() + offset;

            bufferSize = requestRange.getSize() - receptionOffset;

            if ( bufferSize > mediaBufferSize - offset )
            {
                bufferSize = mediaBufferSize - offset;
            }

            bufferRecordIndex = index;

            retVal = AS_NO_ERROR;
        }
        else
        {
            //The buffer is not avaiable, the caller should retry later.
            pBuffer = NULL;

            retVal = AS_AGAIN;
        }
    }
    else
    {
        retVal = AS_INVALID_VALUE;
    }

    return retVal;
}

/** Commit the buffer that stores received data. */
int32_t MediaRequestTask::commitBuffer(
                                    uint32_t dataLength,
                                    uint32_t bufferRecordIndex )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    assert( bufferRecordIndex < mBufferRecordArray.size() );

    mBufferRecordArray[bufferRecordIndex].mFilledDataAmount += dataLength;

    assert( mBufferRecordArray[bufferRecordIndex].mFilledDataAmount
                <= mBufferRecordArray[bufferRecordIndex].mExpectedDataAmount );

    /*
    DEBUGLOG(
        MediaRequestTask,
        "committed buffer track: %u, expected: %u, filled: %u, index: %u",
        mMediaRequestPtr->getTrackId(),
        mBufferRecordArray[bufferRecordIndex].mExpectedDataAmount,
        mBufferRecordArray[bufferRecordIndex].mFilledDataAmount,
        bufferRecordIndex );
    */

    mBytesReceived += dataLength;
    if ( mProgressInterval && ( mBytesReceived >= mNextProgress || mBytesReceived == mRequestSize ) )
    {
        mMediaRequestPtr->reportProgress( mBytesReceived );

        mNextProgress = ( ( mBytesReceived / mProgressInterval ) + 1 ) * mProgressInterval;
    }

    //Check if the expected amount of data is received.
    if (mBufferRecordArray[bufferRecordIndex].mFilledDataAmount == mBufferRecordArray[bufferRecordIndex].mExpectedDataAmount)
    {
        //Commit the buffer only if the expected amount of data is received.
        mBufferRecordArray[bufferRecordIndex].mAseBufferPtr->commit( mBufferRecordArray[bufferRecordIndex].mExpectedDataAmount );

        mMediaRequestPtr->provideResponseBody(
                                    mBufferRecordArray[bufferRecordIndex].mAseBufferPtr,
                                    bufferRecordIndex,
                                    mAbandonTime );

        // XXX Should we release the buffer pointer here ? Otherwise we do not release the buffer until the whole task
        //     is completed.
    }

    retVal = AS_NO_ERROR;

    return retVal;
}

void MediaRequestTask::provideResponseHeaders( HttpResponseHeaders headers )
{
    if ( !mHeadersReported )
    {
        mHeadersReported = true;

        mMediaRequestPtr->provideResponseHeaders( headers );
    }
}

