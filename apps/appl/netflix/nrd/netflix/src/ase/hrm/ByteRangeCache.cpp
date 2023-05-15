/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DebugLogging.h"
#include "HeapBuffer.h"

#include "ByteRangeCache.h"
#include "UrlRequestTask.h"

namespace netflix {
namespace ase {

DEFINE_LOGGER( ByteRangeCache );

}}

using namespace netflix;
using namespace netflix::ase;
using namespace std;

AseErrorCode ByteRangeCache::construct(     uint32_t            trackId,
                                            UrlRequestTaskPtr   pInitialTask,
                                            uint32_t            turboRequestSize,
                                            ByteRangeCachePtr&  pByteRangeCache )
{
    pByteRangeCache = ByteRangeCachePtr( new ByteRangeCache( trackId, pInitialTask, turboRequestSize ) );
    return pByteRangeCache ? AS_NO_ERROR : AS_NOT_ENOUGH_MEMORY;
}

ByteRangeCache::ByteRangeCache(     uint32_t            trackId,
                                    UrlRequestTaskPtr   pInitialTask,
                                    uint32_t            turboRequestSize )
    : MediaRequest( )
    , mByteRangeCacheSize( turboRequestSize )
    , mBufferSize( 4096 )
    , mMediaRequestPtr( pInitialTask->getMediaRequestTask()->getMediaRequest() )
    , mCacheBuffers( ( turboRequestSize + mBufferSize - 1 ) / mBufferSize )
    , mCacheBuffersOutstanding( mCacheBuffers.size() )
{
    ullong const start = pInitialTask->getMediaRequestTask()->getMediaRequest()->getByteRange().start();
    mByteRange = ByteRange( start, start + mByteRangeCacheSize - 1 );

    addTask( pInitialTask );

    setDownloadTrackId( trackId );

    DEBUGLOG( ByteRangeCache, "Constructed for url %s, track %d, range %lld-%lld",
                                    pInitialTask->getCurrentUrl().GetPrefix().c_str(),
                                    mDownloadTrackId,
                                    mByteRange.start(), mByteRange.end() );
}

bool ByteRangeCache::serve( UrlRequestTaskPtr pUrlRequestTask, UrlRequestTaskPtr& pRemainingUrlRequestTask )
{
    // Check if the task is for the same downloadable and overlaps our byte range
    // Check if the task intersects the cache byte range
    ByteRange const range = pUrlRequestTask
                                ->getMediaRequestTask()
                                    ->getMediaRequest()
                                        ->getByteRange();


    if ( pUrlRequestTask->getCurrentUrl() != getCurrentUrl() )
    {
        DEBUGLOG( ByteRangeCache, "Cannot serve request for %s byte range %lld-%lld, wrong file",
                        pUrlRequestTask->getCurrentUrl().GetPrefix().c_str(),
                        range.start(), range.end() );
        return false;
    }


    if( !mByteRange.overlaps( range ) )
    {
        DEBUGLOG( ByteRangeCache, "Cannot serve request for %s byte range %lld-%lld, wrong byte range",
                        pUrlRequestTask->getCurrentUrl().GetPrefix().c_str(),
                        range.start(), range.end() );
        return false;
    }

    DEBUGLOG( ByteRangeCache, "Serving request for byte range %lld-%lld",
                                    range.start(), range.end() );

    // If necessary split the task
    if ( !mByteRange.contains( range ) )
    {
        AseErrorCode retVal = pUrlRequestTask->truncate( mByteRange, pRemainingUrlRequestTask );
        if ( retVal != AS_NO_ERROR )
        {
            DEBUGLOG( ByteRangeCache, "Could not truncate UrlRequestTask" );
            return pUrlRequestTask.get();
        }

        DEBUGLOG( ByteRangeCache, "Splitting request, remaining ranges: %s",
                                        pRemainingUrlRequestTask->getByteRangesDebugString().c_str() );
    }

    // Queue the task
    addTask( pUrlRequestTask );

    // Report the start of the request
    pUrlRequestTask
        ->getMediaRequestTask()
            ->requestStarted( false /*connecting*/, true /*pipelined*/ );

    // Serve anything we can now
    AseTimeVal abandontime = AseTimeVal::INFINITE;

    serveDataToTasks( 0, mCacheBuffers.size() - 1, abandontime );

    return true;
}

ByteRange const& ByteRangeCache::getByteRange( ) const
{
    return mByteRange;
}

size_t ByteRangeCache::getBufferSize() const
{
    return mBufferSize;
}

void ByteRangeCache::requestStarted( bool connecting, bool pipelined )
{
    mMediaRequestPtr->requestStarted( connecting, pipelined );
}

AseErrorCode ByteRangeCache::getResponseBodyBuffer(AseBufferPtr& pBuffer)
{
    HeapBufferPtr pHeapBuffer;
    int err = HeapBuffer::construct( mBufferSize, pHeapBuffer );
    if ( err != 0 )
        return AS_NOT_ENOUGH_MEMORY;

    pBuffer = pHeapBuffer;

    return AS_NO_ERROR;
}

int ByteRangeCache::provideResponseBody(    AseBufferPtr const& pBuffer,
                                            int sequence,
                                            AseTimeVal& abandontime )
{
    assert( !mCacheBuffers[ sequence ] );

    abandontime = AseTimeVal::INFINITE;

    mCacheBuffers[ sequence ] = pBuffer;
    --mCacheBuffersOutstanding;

    serveDataToTasks( sequence, sequence, abandontime );

    return AS_NO_ERROR;
}

void ByteRangeCache::abandoned()
{
    TaskRecordList::iterator it = mTasks.begin();
    for( ; it != mTasks.end(); ++it )
    {
        it->mUrlRequestTaskPtr
                ->getMediaRequestTask()
                    ->getMediaRequest()
                        ->abandoned();
    }
}

void ByteRangeCache::requestError( AseErrorCode status, uint32_t httpCode )
{
    TaskRecordList::iterator it = mTasks.begin();
    for( ; it != mTasks.end(); ++it )
    {
        it->mUrlRequestTaskPtr
                ->getMediaRequestTask()
                    ->getMediaRequest()
                        ->requestError( status, httpCode );
    }
}

void ByteRangeCache::provideFileSize( ullong size )
{
    // Provide the file size to the tasks
    TaskRecordList::const_iterator it = mTasks.begin();
    for( ; it != mTasks.end(); ++it )
    {
        it->mUrlRequestTaskPtr
            ->getMediaRequestTask()
                ->getMediaRequest()
                    ->provideFileSize( size );
    }
}


void ByteRangeCache::addTask( UrlRequestTaskPtr pUrlRequestTask )
{
    if ( mTasks.empty() )
    {
        mMediaRequestPtr = pUrlRequestTask
                            ->getMediaRequestTask()
                                ->getMediaRequest();
    }

    mTasks.push_back( TaskRecord( pUrlRequestTask ) );

    TaskRecord::ByteRanges& ranges = mTasks.back().mByteRanges;

    // Get byte ranges from the task that are all within individual buffers
    while( !pUrlRequestTask->scheduled() )
    {
        ByteRange range;
        int32_t err = pUrlRequestTask->peekByteRange( mBufferSize, range );
        if ( err != AS_NO_ERROR )
        {
            // XXX Find a way to handle this error, if it can even happen
            assert( false );
        }

        assert( range.containedIn( mByteRange ) );

        ullong const relstart = range.start() - mByteRange.start();
        ullong const maxsize = ( ( relstart / mBufferSize ) + 1 ) * mBufferSize - relstart;
        ullong const size = ((ullong) range.size()) < maxsize ? range.size() : maxsize;

        err = pUrlRequestTask->getByteRange( size, range );
        if ( err != AS_NO_ERROR )
        {
            // XXX Find a way to handle this error, if it can even happen
            assert( false );
        }

        DEBUGLOG( ByteRangeCache, "...queueing byte range %lld-%lld", range.start(), range.end() );

        ranges.push_back( make_pair( range, (uint32_t) 0 ) );
    }
}

void ByteRangeCache::serveDataToTasks( int first, int last, AseTimeVal& abandontime )
{
    ByteRange const datarange(  mByteRange.start() + first * mBufferSize,
                                mByteRange.start() + last * mBufferSize + mBufferSize - 1 );

    TaskRecordList::iterator it = mTasks.begin();
    while( it != mTasks.end() )
    {
        TaskRecord::ByteRanges::iterator rit = it->mByteRanges.begin();
        while( rit != it->mByteRanges.end() )
        {
            ByteRange const range = rit->first;

            if ( range.overlaps( datarange ) )
            {
                assert( range.containedIn( datarange ) );

                ByteRange relative( range.relativeTo( mByteRange ) );

                ullong const i = relative.start() / mBufferSize;
                if ( mCacheBuffers[ i ] )
                {
                    // Calculate offset into buffer and number of bytes remaining
                    ullong offset = relative.start() - i * mBufferSize + rit->second;
                    ullong size = range.size() - rit->second;

                    if ( size > 0 && offset < mCacheBuffers[ i ]->getFilledSpaceSize() )
                    {
                        // Calculate the most we could provide
                        if ( mCacheBuffers[ i ]->getFilledSpaceSize() - offset < size )
                        {
                            size = mCacheBuffers[ i ]->getFilledSpaceSize() - offset;
                        }

                        // Get a buffer for the data
                        unsigned char* pBuffer;
                        uint32_t bufferSize, bufferRecordIndex;
                        uint32_t skipBytes;

                        while( size > 0 )
                        {
                            ByteRange range( rit->first.start() + rit->second, rit->first.end() );

                            int32_t err = it->mUrlRequestTaskPtr->getBuffer(    range,
                                                                                skipBytes,
                                                                                pBuffer,
                                                                                bufferSize,
                                                                                bufferRecordIndex );
                            if ( err != AS_NO_ERROR )
                            {
                                DEBUGLOG( ByteRangeCache, "getBuffer failed with %d", err );

                                break;
                            }


                            if ( skipBytes > 0 )
                            {
                                ullong const skip = ( size < skipBytes ) ? size : skipBytes;

                                err = it->mUrlRequestTaskPtr->commitSkippedBytes( range.prefix( skip ) );
                                if ( err != AS_NO_ERROR )
                                {
                                    DEBUGLOG( ByteRangeCache, "commit skipped bytes failed with %d", err );
                                    break;
                                }

                                offset += skip;
                                size -= skip;
                                rit->second += skip;

                                range = ByteRange( range.start() + skip, range.end() );
                            }

                            ullong const copy = ( size < bufferSize ) ? size : bufferSize;

                            ::memcpy(   pBuffer,
                                        mCacheBuffers[ i ]->getFilledSpaceStart() + offset,
                                        copy );

                            err = it->mUrlRequestTaskPtr->commitReceivedBytes(  range.prefix( copy ),
                                                                                bufferRecordIndex );

                            if ( err != AS_NO_ERROR )
                            {
                                DEBUGLOG( ByteRangeCache, "commit received bytes failed with %d", err );
                                break;
                            }

                            // XXX Here the abandontime could have been set in the sub-task
                            //     if so, we should return this as our own abandontime
                            AseTimeVal const taskabandontime = it->mUrlRequestTaskPtr
                                                                    ->getMediaRequestTask()
                                                                        ->getAbandonTime();
                            if ( taskabandontime < abandontime )
                            {
                                abandontime = taskabandontime;
                            }

                            offset += copy;
                            size -= copy;
                            rit->second += copy;

                            DEBUGLOG( ByteRangeCache, "Forwarding %llu bytes for %lld-%lld, total %u of %lld",
                                        copy,
                                        range.start(),
                                        range.end(),
                                        rit->second,
                                        range.size() );

                        }
                    }
                }
            }

            if ( rit->second == range.size() )
            {
                rit = it->mByteRanges.erase( rit );
            }
            else
            {
                ++rit;
            }
        }

        if ( it->mByteRanges.empty() )
        {
            it = mTasks.erase( it );
        }
        else
        {
            ++it;
        }
    }

    // Keep mMediaRequestPtr pointing to an active task
    if ( !mTasks.empty() )
    {
        mMediaRequestPtr = mTasks.front().mUrlRequestTaskPtr
                                ->getMediaRequestTask()
                                    ->getMediaRequest();
    }
}

