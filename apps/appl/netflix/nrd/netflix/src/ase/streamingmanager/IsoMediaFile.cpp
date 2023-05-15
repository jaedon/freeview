/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "IsoMediaFile.h"
#include "BoxReader.h"
#include "FtypBoxReader.h"
#include "FreeBoxReader.h"
#include "MoovBoxReader.h"

using namespace std;
using namespace netflix;
using namespace ase;

namespace netflix { namespace ase {

DEFINE_LOGGER( IsoMediaFile );

}}

static const fourcc_t MOOF_4CC = BoxReader::getFourcc( "moof" );

/*
 * At startup, this class implements the following logic:
 *      requests are issued progressively through the file looking for
 *      at least one of each of { moov, sidx }. If the current unfinished
 *      box is a required one ( moov or sidx ) then the request size is
 *      the remaining data plus a configured amount (default 4KB).
 *      Otherwise the request is from the start of the next box.
 *
 * This object has at most one outstanding request.
 *
 * Subsequently, when requests for index time ranges are received these
 * are added to the RequiredAseTimeRanges queue.
 *
 * When a request opportunity arises, the next required time range is
 * compared to the index to see what
 * remains to be requested and this remainder is requested (and added
 * to the requested time ranges structure).
 *
 * During the initial phase a new buffer is created for each request,
 * possibly containing the intial portion of a box which is to be
 * completed. A separate buffer is created into which the moov box will
 * be copied once it is completely received.
 *
 * IMPROVEME!   This could all be improved so that we handle data incrementally
 *              as it arrives. A better representation for the object state
 *              would drive the processing of incoming data and the generation
 *              of new requests.
 *
 *              This would require incremental parsing of the Segment Index
 *              but would allow playback to start before the index has been
 *              completely received in the parallel connections case.
 */

/* Redesign notes
 *
 * - The MediaFile will own the vector of MediaFragments: there will be a
 * getIndex() method to get a const reference to the vector. This means the
 * MediaFile can just append blocks to the index as they arrive.
 * - There will be an bool indexComplete() method which indicates whether
 * the entire index is available. There should be some way to pass this
 * information to the streaming heuristics (perhaps a bool complete()
 * method on IStreamInfo) so they can determine whether to target buffer
 * level zero at the end, or target some non-zero buffer level.
 * - The MediaFile will have the following states:
 * FAILED,
 * INIT,
 * FETCHING_HEADER,
 * HAVE_HEADER,
 * FETCHING_INDEX,
 * HAVE_INDEX
 *
 * The MediaStream::mediaHeaderDownloaded and
 * MediaStream::mediaIndexDownloaded functions can just check this state
 * (and/or I can provide helper functions on MediaFile for these checks).
 *
 * - MediaFile::doesNeedNewRequest will be false except in the two
 * FETCHING states. When in the fetching states the MediaStream should keep
 * giving requests to the MediaFile whenever doesNeedNewRequest returns
 * true. I will probably implement incremental index download with a single
 * request, meaning that you need parallel connections to begin data
 * downloading in parallel to the index. But it would not be so hard to do
 * it in multiple small requests: you would decide in MediaStream that, for
 * example, if you have less than 5 minutes index available to give another
 * request to the MediaFile instead of getting a MediaFragment,
 *
 * - Transition from INIT to FETCHING_HEADER will be triggered by calling
 * a new method fetchHeader()
 * - Transition from HAVE_HEADER to FETCHING_INDEX will be triggered by
 * calling a new method fetchIndex() (or if this method has been called
 * earlier then it will happen automatically). This allows you to delay
 * fetching the index for the DRM streams until the license has been
 * acquired.
 * - It may jump from FETCHING_HEADER direct to HAVE_INDEX if everything
 * arrives in one response (possible for very short clips).
 */

/* We would like to move to a model where we hand out small data buffers
 * and receive data incrementally. Box handlers should received data in
 * the form of BoxReader objects and indicate what data has been processed by
 * consuming these.
 */

namespace netflix { namespace ase {

struct FtypBoxHandler : public IsoMediaFile::BoxHandler
{
    virtual AseErrorCode init(      IsoMediaFilePtr     pIsoMediaFile,
                                    BoxReader const&    reader );

    virtual AseErrorCode process( unsigned char const *pData, size_t size );

};

struct FreeBoxHandler : public IsoMediaFile::BoxHandler
{
    virtual AseErrorCode init(      IsoMediaFilePtr     pIsoMediaFile,
                                    BoxReader const&    reader );

    virtual AseErrorCode process( unsigned char const *pData, size_t size );

};

struct MoovBoxHandler : public IsoMediaFile::BufferedBoxHandler
{
    virtual AseErrorCode doProcess();
};

struct IsoBoxHandlerFactory : public IsoMediaFile::BoxHandlerFactory
{
    virtual IsoMediaFile::BoxHandlerPtr createBoxHandler( BoxReader const& reader )
    {
        if ( reader.getType() == FtypBoxReader::type )
            return IsoMediaFile::BoxHandlerPtr( new FtypBoxHandler() );
        else if ( reader.getType() == FreeBoxReader::type )
            return IsoMediaFile::BoxHandlerPtr( new FreeBoxHandler() );
        else if ( reader.getType() == MoovBoxReader::type )
            return IsoMediaFile::BoxHandlerPtr( new MoovBoxHandler() );
        return IsoMediaFile::BoxHandlerPtr();
    }
} isoBoxHandlerFactory;

}}

#if 0
static const char* stateToString( IsoMediaFile::State state )
{
    return ( state == IsoMediaFile::STATE_INIT ) ? "Initialized"
            : ( state == IsoMediaFile::STATE_SEARCHING ) ? "Searching"
            : ( state == IsoMediaFile::STATE_LOADING ) ? "Loading"
            : ( state == IsoMediaFile::STATE_COMPLETE ) ? "Complete"
            : "unknown";
}
#endif

AseErrorCode IsoMediaFile::BufferedBoxHandler::init(    IsoMediaFilePtr     pIsoMediaFile,
                                                        BoxReader const&    reader )
{
    mIsoMediaFilePtr = pIsoMediaFile;
    AseErrorCode retVal = HeapBuffer::construct( (size_t) reader.getSize(), mBufferedBoxPtr );
    if ( retVal != AS_NO_ERROR )
        return retVal;

    mBoxOffset = reader.getOffset();

    uint8_t const* pStart;
    size_t size;

    reader.getboxdata( pStart, size );

    return process( pStart, size );
}

AseErrorCode IsoMediaFile::BufferedBoxHandler::process( uint8_t const *pData, size_t size )
{
    assert( mBufferedBoxPtr );

    mBufferedBoxPtr->copyin( pData, size );

    return doProcess();
}

AseErrorCode FtypBoxHandler::init(  IsoMediaFilePtr     pIsoMediaFile,
                                    BoxReader const&    reader )
{
    if ( !reader.complete() )
        return AS_MORE;

    return pIsoMediaFile->handleFtypBox( reader );
}

AseErrorCode FtypBoxHandler::process( unsigned char const *pData, size_t size )
{
    NRDP_UNUSED( pData );
    NRDP_UNUSED( size );
    assert( false );
    return 0;
}

AseErrorCode FreeBoxHandler::init(  IsoMediaFilePtr     pIsoMediaFile,
                                    BoxReader const&    reader )
{
    if ( !reader.complete() )
        return AS_MORE;

    return pIsoMediaFile->handleFreeBox( reader );
}

AseErrorCode FreeBoxHandler::process( unsigned char const *pData, size_t size )
{
    NRDP_UNUSED( pData );
    NRDP_UNUSED( size );
    assert( false );
    return 0;
}

AseErrorCode MoovBoxHandler::doProcess()
{
    if ( mBufferedBoxPtr->getFreeSpaceSize() != 0 )
        return AS_NO_ERROR;

    AseErrorCode retVal = mIsoMediaFilePtr.lock()->handleMoovBox( mBufferedBoxPtr );
    mBufferedBoxPtr.reset();
    return retVal;
}

IsoMediaFile::IsoMediaFile( ConstManifestStreamPtr pManifestSteam )
    : mInitialRequestSize( 4096 )
    , mManifestStreamPtr( pManifestSteam )
    , mBoxProcessingState( BOX_PROCESSING_INIT )
    , mFailure( AS_NO_ERROR )
    , mFileSize( 0 )
    , mDuration( AseTimeInterval::ZERO )
    , mIndexPtr( new MediaFileIndex )
    , mbIndexComplete( false )
    , mbParseMoovBox( false )
{
    registerBox( FtypBoxReader::type, isoBoxHandlerFactory );
    registerBox( FreeBoxReader::type, isoBoxHandlerFactory );
    registerBox( MoovBoxReader::type, isoBoxHandlerFactory );
    setBoxSearch( FtypBoxReader::type );
    setBoxSearch( MoovBoxReader::type );
}

AseErrorCode IsoMediaFile::initialize( IAseConfigParameterPtr pConfigParameters )
{
    mInitialRequestSize = pConfigParameters->getParameterAs<llong>( "initialFileRequestSize", 4096 );
    mbParseMoovBox = pConfigParameters->getParameterAs<bool>( "aseParseMoovBox", true );

    return AS_NO_ERROR;
}

void IsoMediaFile::registerBrand( fourcc_t brand, BrandHandler& handler )
{
    mBrandHandlers.push_back( make_pair( brand, &handler ) );
}

void IsoMediaFile::registerDefaultBrand( BrandHandler& handler )
{
    mDefaultBrandHandlers.push_back( &handler );
}

void IsoMediaFile::registerBox( fourcc_t box, BoxHandlerFactory& factory )
{
    mBoxHandlers.push_back( make_pair( box, &factory ) );
}

void IsoMediaFile::registerBox( nf_uuid_t const& box, BoxHandlerFactory& factory )
{
    mExtendedBoxHandlers.push_back( make_pair( box, &factory ) );
}

void IsoMediaFile::setBoxSearch( fourcc_t box )
{
    mRequiredBoxes.push_back( box );}

void IsoMediaFile::setBoxSearch( nf_uuid_t const& box )
{
    mRequiredUuidBoxes.push_back( box );
}

void IsoMediaFile::clearBoxSearch( fourcc_t box )
{
    vector<fourcc_t>::iterator it = find( mRequiredBoxes.begin(), mRequiredBoxes.end(), box );
    if ( it != mRequiredBoxes.end() )
    {
        mRequiredBoxes.erase( it );
    }
}

void IsoMediaFile::clearBoxSearch( nf_uuid_t const& box )
{
    vector<nf_uuid_t>::iterator it = find( mRequiredUuidBoxes.begin(), mRequiredUuidBoxes.end(), box );
    if ( it != mRequiredUuidBoxes.end() )
    {
        mRequiredUuidBoxes.erase( it );
    }
}

void IsoMediaFile::clearBoxSearch( BoxReader const& reader )
{
    if ( reader.isExtendedType() )
    {
        clearBoxSearch( reader.getExtendedType() );
    }
    else
    {
        clearBoxSearch( reader.getType() );
    }
}

bool IsoMediaFile::doesNeedNewRequest() const
{
    if ( mCurrentRequestPtr )
        return false;

    return ( searchBoxes() > 0 || !( mRequiredByteRanges - mDoneByteRanges ).empty() );
}

AseErrorCode IsoMediaFile::getNextRequest( MediaPresentationRequestPtr& pRequest )
{
    if ( !doesNeedNewRequest() )
        return AS_MEDIA_FILE_REQUEST_PENDING;

    // Get a required-and-not-done byte range
    Range<llong> range = ( mRequiredByteRanges - mDoneByteRanges ).front();

    // Try to add some search space if we need it
    if ( searchBoxes() > 0 )
    {
        if ( range.empty() )
        {
            llong const start = mDoneByteRanges.empty() ? 0 : mDoneByteRanges.back().start();
            if ( start == 0 && mInitialRequestHint.valid() )
            {
                range = Range<llong>( mInitialRequestHint.start(), mInitialRequestHint.end() + 1 );
            }
            else
            {
                range = Range<llong>( start, start + mInitialRequestSize );
            }
        }
        else
        {
            range |= Range<llong>( range.end(), range.end() + mInitialRequestSize );
            range = ( Ranges<llong>( range ) - mDoneByteRanges ).front();
        }
    }

    if ( range.empty() )
        return AS_PENDING;

    AseErrorCode retVal = createNewRequest( range, pRequest );
    if ( retVal != AS_NO_ERROR )
        return ( mFailure = retVal );

    return AS_NO_ERROR;
}

bool IsoMediaFile::good() const
{
    return ( mFailure == AS_NO_ERROR );
}

bool IsoMediaFile::hasDuration() const
{
    return !mDuration.isZero();
}

bool IsoMediaFile::hasHeader() const
{
    return ( mFailure == AS_NO_ERROR ) && mHeaderPtr;
}

bool IsoMediaFile::hasIndex() const
{
    return ( mFailure == AS_NO_ERROR ) && !mIndexPtr->empty();
}

bool IsoMediaFile::indexComplete() const
{
    return ( mFailure == AS_NO_ERROR ) && mbIndexComplete;
}

bool IsoMediaFile::getDuration( AseTimeInterval& duration ) const
{
    if ( mDuration.isZero() )
        return false;

    duration = mDuration;

    return true;
}

bool IsoMediaFile::getFragmentDuration( AseTimeInterval& fragmentDuration ) const
{
    if ( mFragmentDuration.isZero() )
        return false;

    fragmentDuration = mFragmentDuration;

    return true;
}

ByteRange IsoMediaFile::getHeaderRange() const
{
    if ( !indexComplete() )
        return ByteRange();

    Ranges<llong> const ranges = mDoneByteRanges - mIgnoredByteRanges;

    if ( ranges.empty() )
        return ByteRange();

    llong end = ranges.front().end();

    // Combine the ranges together if they are close
    Ranges<llong>::const_iterator it = ranges.begin();
    for( ++it; it != ranges.end(); ++it )
    {
        if ( ( it->start() - end ) > 8192 )
            break;

        end = it->end();
    }

    return ByteRange( ranges.front().start(), end - 1 );
}

void IsoMediaFile::provideHeaderRangeHint( ByteRange const& range )
{
    if ( range.valid() )
    {
        mInitialRequestHint = ByteRange( range.start(), range.end() + 128 );
    }
}

bool IsoMediaFile::getHeader( AseBufferPtr& pMediaHeader ) const
{
#if 1
    DEBUGLOG( IsoMediaFile, "[%s] getHeader() mFailure=%d, %s header, %s index, %s current request",
                mManifestStreamPtr->getStreamKey().c_str(),
                mFailure,
                mHeaderPtr ? "have" : "no",
                mbIndexComplete ? "have" : "no",
                mCurrentRequestPtr ? "have" : "no" );
#endif

    if ( ( mFailure != AS_NO_ERROR ) || ( !mHeaderPtr ) )
        return false;

    pMediaHeader = mHeaderPtr;

    return true;
}

/** query the media index given the time range
 * @param [out] pIndex The index vector
 * @param [out] indexStart start time of the first fragment
 * @param [out] indexEnd end of the last fragment in the index
 * @param [out] mediaEnd end time of the last fragment in the media
 * @param [out] mediaOffset the byte offset of the first fragment
 * @return true if index information was returned, false if not yet available
 */
bool IsoMediaFile::getIndex(    MediaFileIndexConstPtr& pIndex,
                                AseTimeStamp&           indexStart,
                                AseTimeStamp&           indexEnd,
                                ullong&               mediaOffset ) const
{
#if 1
    DEBUGLOG( IsoMediaFile, "[%s] getIndex() mFailure=%d, %s header, %s index, %s current request",
             mManifestStreamPtr->getStreamKey().c_str(),
             mFailure,
             ( mHeaderPtr ? "have" : "no" ),
             ( mbIndexComplete ? "have" : "no" ),
             ( mCurrentRequestPtr ? "have" : "no" ) );
#endif

    if ( /*!indexComplete()*/ !hasIndex() )
        return false;

    DEBUGLOG( IsoMediaFile, "[%s] Providing index, index start=%llu, index end=%llu, media end=%llu, size=%zu",
             mManifestStreamPtr->getStreamKey().c_str(),
             mIndexStart.getValueInMs(),
             mIndexEnd.getValueInMs(),
             mDuration.getValueInMs(),
             mIndexPtr ? mIndexPtr->size() : 0 );

    pIndex = mIndexPtr;
    indexStart = mIndexStart;
    indexEnd = mIndexEnd;
    mediaOffset = mMovieFragmentsOffset;

    return true;
}

AseErrorCode IsoMediaFile::getLastError() const
{
    return mFailure;
}

std::string const& IsoMediaFile::getStreamKey() const
{
    return mManifestStreamPtr->getStreamKey();
}


std::vector<unsigned char> const& IsoMediaFile::getFileToken( ) const
{
    return mFreeToken;
}

void IsoMediaFile::provideFailureCode( AseErrorCode code )
{
    mFailure = code;
    reset();
}

int IsoMediaFile::searchBoxes() const
{
    return ( mRequiredBoxes.size() + mRequiredUuidBoxes.size() );
}

AseErrorCode IsoMediaFile::createNewRequest( Range<llong> const& range, MediaPresentationRequestPtr& pRequest )
{
    ByteRange byterange( range.start(), range.end() - 1 );

    DEBUGLOG( IsoMediaFile, "[%s] queueNewRequest(): range=%s",
                mManifestStreamPtr->getStreamKey().c_str(),
                byterange.toByteRangeSpec().c_str() );

    AseErrorCode retVal = AS_NO_ERROR;

    if ( byterange.valid() )
    {
        // Check the request is really within the file size
        if ( mFileSize > 0 )
        {
            if ( ( (ullong)byterange.start() >= mFileSize )
                || ( byterange.closed() && ( (ullong)byterange.end() >= mFileSize ) ) )
            {
                ERRORLOG( IsoMediaFile, "Invalid range [%llu,%llu] provide for file size %llu",
                                                        byterange.start(), byterange.end(), mFileSize );
                return (mFailure = AS_INVALID_VALUE );
            }
        }

        int res = IsoMediaFileRequest::construct(
                                     static_pointer_cast<IsoMediaFile>(shared_from_this()),
                                     byterange,
                                     mCurrentRequestPtr );
        if ( res == 0 )
        {
            pRequest = mCurrentRequestPtr;
            mDoneByteRanges |= range;
        }
        else
        {
            mFailure = AS_NOT_ENOUGH_MEMORY;
            retVal = AS_NOT_ENOUGH_MEMORY;
        }
    }

    return retVal;

}

void IsoMediaFile::provideFileSize( ullong fileSize )
{
    if ( mFileSize != 0 && mFileSize != fileSize )
    {
        ERRORLOG( IsoMediaFile, "Inconsistent file size provided: %llu (was %llu)",
            fileSize, mFileSize );
    }
    else
    {
        mFileSize = fileSize;
    }
}

void IsoMediaFile::provideDuration( AseTimeInterval duration )
{
    if ( mDuration != AseTimeInterval::ZERO && mDuration != duration )
    {
        ERRORLOG( IsoMediaFile, "Inconsistent duration provided: %llums (was %llums)",
                 duration.getValueInMs(),
                 mDuration.getValueInMs() );
    }
    else
    {
        mDuration = duration;
    }
}

void IsoMediaFile::requestByteRange( ByteRange const& range )
{
    mRequiredByteRanges |= Range<llong>( range.getStart(), range.getEnd() + 1 );
}

void IsoMediaFile::ignoreByteRange( ByteRange const& range )
{
    Range<llong> const r( range.getStart(), range.getEnd() + 1 );
    mDoneByteRanges |= r;
    mIgnoredByteRanges |= r;
}

void IsoMediaFile::reset()
{
    // This means our request has been or will be forgotten
    if ( mCurrentRequestPtr )
    {
        mCurrentRequestPtr->ignore();
        ByteRange const byterange( mCurrentRequestPtr->getUndeliveredByteRange() );
        Range<llong> const range( byterange.start(), byterange.end() + 1 );
        mRequiredByteRanges |= range;
        mDoneByteRanges -= range;
        mCurrentRequestPtr.reset();

        DEBUGLOG( IsoMediaFile, "[%s] IsoMediaFile::reset() returning %lld-%lld to required ranges",
                    mManifestStreamPtr->getStreamKey().c_str(),
                    byterange.start(), byterange.end() );
    }
}

/** Handle response body data
 *
 *  Response body data is handled in strict file byte order
 *  Requests are issued serially to ensure this and the IsoMediaFileRequest reorders
 *  response body chunks. There may be gaps in the requested data for boxes that
 *  are not required.
 */
AseErrorCode IsoMediaFile::provideResponseBody(
                                            IsoMediaFileRequestPtr  /*pRequest*/,
                                            AseBufferPtr            pBuffer,
                                            ullong                offset,
                                            bool                    bRequestComplete )
{
    AseErrorCode retVal = AS_NO_ERROR;

    if ( bRequestComplete )
        mCurrentRequestPtr.reset();

    if ( mFailure )
        return mFailure;

    while( ( retVal == AS_NO_ERROR ) && pBuffer && !pBuffer->empty() )
    {
        switch( mBoxProcessingState )
        {
            case BOX_PROCESSING_INIT :
                // No state - we should be at the start of a box
                assert( !mCurrentBoxBufferPtr );
                assert( !mCurrentBoxHandlerPtr );

                mCurrentBoxBufferPtr.swap( pBuffer );
                mCurrentBoxOffset = offset;
                pBuffer.reset();

                mBoxProcessingState = BOX_PROCESSING_HEADER;

                // Fall through

            case BOX_PROCESSING_HEADER:
                // We have some data left in the current box buffer, but it wasn't enough to
                // start processing of the box
                assert( mCurrentBoxBufferPtr );
                assert( !mCurrentBoxHandlerPtr );

                retVal = handleNewBoxes( pBuffer, offset );
                break;

            case BOX_PROCESSING_BOX:

                {
                    // We are incrementally processing a box.
                    assert( !mCurrentBoxBufferPtr );
                    assert( mCurrentBoxHandlerPtr );
                    assert( mRemainingBytes > 0 );

                    retVal = mCurrentBoxHandlerPtr->process( pBuffer->getFilledSpaceStart(), pBuffer->getFilledSpaceSize() );
                    if ( retVal != AS_NO_ERROR )
                        break;

                    size_t const consumed = pBuffer->release( mRemainingBytes );
                    offset += consumed;
                    mRemainingBytes -= consumed;

                    if ( mRemainingBytes == 0 )
                    {
                        mCurrentBoxHandlerPtr.reset();
                        mBoxProcessingState = BOX_PROCESSING_INIT;
                    }
                }

                break;

            case BOX_PROCESSING_IGNORED_BOX:

                {
                    // There was no handler for the box, so we're ignoring it and waiting for the next one
                    assert( !mCurrentBoxBufferPtr );
                    assert( !mCurrentBoxHandlerPtr );
                    assert( mRemainingBytes > 0 );

                    size_t const consumed = pBuffer->release( mRemainingBytes );
                    offset += consumed;
                    mRemainingBytes -= consumed;

                    if ( mRemainingBytes == 0 )
                    {
                        mBoxProcessingState = BOX_PROCESSING_INIT;
                    }
                }

                break;
        }
    }

    if ( bRequestComplete && mBoxProcessingState == BOX_PROCESSING_IGNORED_BOX )
    {
        // Bytes past the end of the current request are ignored by adding them to the mDoneBytes list, so the next
        // data we get will be at the beginning of a box
        mBoxProcessingState = BOX_PROCESSING_INIT;
    }

    if ( retVal != AS_NO_ERROR )
        mFailure = retVal;

    return retVal;
}

AseErrorCode IsoMediaFile::handleNewBoxes(  AseBufferPtr    pBuffer,
                                            ullong        offset )
{
    while( ( mCurrentBoxBufferPtr && !mCurrentBoxBufferPtr->empty() )
            || ( pBuffer && !pBuffer->empty() ) )
    {
        // mCurrentBoxBufferPtr may or may not be null
        // pBuffer may or may not be null
        // but there is data in at least one of them

        if ( !mCurrentBoxBufferPtr )
        {
            mCurrentBoxBufferPtr = pBuffer;
            mCurrentBoxOffset = offset;
            pBuffer.reset();
        }
        else if ( pBuffer )
        {
            mCurrentBoxBufferPtr->shuffle();
            offset += mCurrentBoxBufferPtr->copyfrom( pBuffer );
        }

        // Now we have data in mCurrentBoxBufferPtr, and pBuffer may or may not contain more

        assert( mCurrentBoxBufferPtr );
        assert( !mCurrentBoxBufferPtr->empty() );

        BoxReader reader(   mCurrentBoxOffset,
                            mCurrentBoxBufferPtr->getFilledSpaceStart(),
                            mCurrentBoxBufferPtr->getFilledSpaceSize() );

        if ( !reader.readBoxHeader() )
        {
            if ( mCurrentBoxBufferPtr->getFilledSpaceSize() == mCurrentBoxBufferPtr->getCapacity() )
            {
                // This is a strange parse error, where we have no space in the buffer for more data
                // but we couldn't parse the header. If we don't fail here we'll spin because we can't
                // add more data to the buffer.
                return AS_PARSE_ERROR;
            }

            // Go round again for more data
            break;
        }

        DEBUGLOG( IsoMediaFile, "[%s] Box type '%c%c%c%c' at %llu, size %llu%s",
                 mManifestStreamPtr->getStreamKey().c_str(),
                 (char) (( reader.getType() >> 24 ) & 0xff),
                 (char) (( reader.getType() >> 16 ) & 0xff),
                 (char) (( reader.getType() >> 8 ) & 0xff),
                 (char) ( reader.getType() & 0xff),
                 mCurrentBoxOffset, reader.getSize(),
                 ( reader.complete() ? "" : ", partial" ) );

        if ( reader.getType() == MOOF_4CC && searchBoxes() > 0 )
        {
            DEBUGLOG( IsoMediaFile, "Header boxes expected before movie fragments" );
            return AS_PARSE_ERROR;
        }

        if ( reader.getSize() > sMaximumHeaderBoxSize )
        {
            DEBUGLOG( IsoMediaFile, "Unexpected large box found when looking for header boxes" );
            return AS_PARSE_ERROR;
        } else if ( reader.getSize() < sMinimumBoxSize) {
            DEBUGLOG( IsoMediaFile, "Unexpected small box found when looking for header boxes" );
            return AS_PARSE_ERROR;
        }

        // Don't need to search for it any more
        clearBoxSearch( reader );

        BoxHandlerPtr pBoxHandler = createBoxHandler( reader );
        if ( pBoxHandler )
        {
            if ( reader.getSize() > sMaximumSupportedHeaderBoxSize )
            {
                DEBUGLOG( IsoMediaFile, "Box is above maximum supported size (%llu)", sMaximumSupportedHeaderBoxSize );
                return AS_PARSE_ERROR;
            }

            if ( reader.complete() )
            {
                // Process the box in place
                AseErrorCode retVal = pBoxHandler->init( static_pointer_cast<IsoMediaFile>(shared_from_this()), reader );
                if ( retVal != AS_NO_ERROR )
                    return retVal;

                // Move on to next box
                mCurrentBoxBufferPtr->release( reader.getSize() );
                mCurrentBoxOffset += reader.getSize();
            }
            else
            {
                // Initialize the box handler
                AseErrorCode retVal = pBoxHandler->init( static_pointer_cast<IsoMediaFile>(shared_from_this()), reader );

                // Handler should be able to ask for complete box ...
                if ( retVal == AS_MORE )
                {
                    if ( mCurrentBoxBufferPtr->getFreeSpaceSize() == 0 )
                    {
                        // Buffer is full, but the box doesn't fit! We need an incremental handler for this case!
                        return AS_PARSE_ERROR;
                    }

                    assert( !pBuffer || pBuffer->empty() );

                    mBoxProcessingState = BOX_PROCESSING_HEADER;
                }
                else if ( retVal != AS_NO_ERROR )
                {
                    return retVal;
                }
                else
                {
                    mCurrentBoxHandlerPtr = pBoxHandler;

                    size_t const consumed = mCurrentBoxBufferPtr->getFilledSpaceSize();
                    mRemainingBytes = reader.getSize() - consumed;
                    mCurrentBoxBufferPtr->release( consumed );
                    mCurrentBoxBufferPtr.reset();
                    mBoxProcessingState = BOX_PROCESSING_BOX;
                }

                // Post the byte range to ensure we get the rest of the box
                requestByteRange( ByteRange( mCurrentBoxOffset, mCurrentBoxOffset + reader.getSize() - 1 ) );

                // Exit from box processing loop
                break;
            }
        }
        else
        {
            // Unrecognized box
            if ( reader.complete() )
            {
                // Move past the unwanted box
                mCurrentBoxBufferPtr->release( reader.getSize() );
                mCurrentBoxOffset += reader.getSize();
            }
            else
            {
                // Ignore the rest of this box
                size_t const consumed = mCurrentBoxBufferPtr->getFilledSpaceSize();
                mRemainingBytes = reader.getSize() - consumed;
                mCurrentBoxBufferPtr->release( consumed );

                mCurrentBoxBufferPtr.reset();
                mBoxProcessingState = BOX_PROCESSING_IGNORED_BOX;

                // Post this as an unwanted byte range
                ignoreByteRange( ByteRange( mCurrentBoxOffset, mCurrentBoxOffset + reader.getSize() - 1 ) );

                // Exit from box processing loop
                break;
            }
        }
    }

    return AS_NO_ERROR;
}

IsoMediaFile::BoxHandlerPtr IsoMediaFile::createBoxHandler( BoxReader const& reader )
{
    if ( reader.isExtendedType() )
    {
        vector<pair<nf_uuid_t,BoxHandlerFactory*> >::iterator it = mExtendedBoxHandlers.begin();
        for( ; it != mExtendedBoxHandlers.end() && it->first != reader.getExtendedType(); ++it ) ;
        if ( it != mExtendedBoxHandlers.end() )
        {
            return it->second->createBoxHandler( reader );
        }
    }
    else
    {
        vector<pair<fourcc_t,BoxHandlerFactory*> >::iterator it = mBoxHandlers.begin();
        for( ; it != mBoxHandlers.end() && it->first != reader.getType(); ++it ) ;
        if ( it != mBoxHandlers.end() )
        {
            return it->second->createBoxHandler( reader );
        }
    }

    return BoxHandlerPtr();
}

AseErrorCode IsoMediaFile::handleFtypBox( BoxReader const& reader )
{
    assert( reader.complete() );

    FtypBoxReader ftyp( reader );

    fourcc_t majorBrand;
    uint32_t minorVersion;
    vector<fourcc_t> compatibleBrands;

    if ( !ftyp.readBrands( majorBrand, minorVersion, compatibleBrands ) )
        return AS_PARSE_ERROR;

    DEBUGLOG( IsoMediaFile, "processing ftyp box, major brand %c%c%c%c",
                (char) (majorBrand >> 24),
                (char) ((majorBrand >> 16)& 0xff),
                (char)  ((majorBrand >> 8) & 0xff),
                (char) (majorBrand&0xff) );

    vector<fourcc_t>::iterator bit = compatibleBrands.begin();
    for( ; bit != compatibleBrands.end(); ++bit )
    {
        DEBUGLOG( IsoMediaFile, "...compatible brand %c%c%c%c",
                 (char) (*bit >> 24),
                 (char) ((*bit >> 16)&0xff),
                 (char) ((*bit >> 8)&0xff),
                 (char) ((*bit)&0xff) );
    }

    bool index = false;
    vector<std::pair<fourcc_t,BrandHandler*> >::iterator it = mBrandHandlers.begin();
    for( ; it != mBrandHandlers.end(); ++it )
    {
        if ( it->first == majorBrand
            || find( compatibleBrands.begin(), compatibleBrands.end(), it->first ) != compatibleBrands.end() )
        {
            AseErrorCode retVal = (*it->second)( static_pointer_cast<IsoMediaFile>(shared_from_this()), index );
            if ( retVal != AS_NO_ERROR )
                return retVal;
        }
    }

    vector<BrandHandler*>::iterator jt = mDefaultBrandHandlers.begin();
    for( ; jt != mDefaultBrandHandlers.end(); ++jt )
    {
        AseErrorCode retVal = (**jt)( static_pointer_cast<IsoMediaFile>(shared_from_this()), index );
        if ( retVal != AS_NO_ERROR )
            return retVal;
    }

    return ( index ? AS_NO_ERROR : AS_PARSE_ERROR );
}

AseErrorCode IsoMediaFile::handleFreeBox( BoxReader const& reader )
{
    assert( reader.complete() );

    FreeBoxReader free( reader );

    char freetoken[16];

    if ( free.readToken( freetoken ) )
    {
        mFreeToken.assign( freetoken, freetoken + 16 );
    }

    return AS_NO_ERROR;
}

AseErrorCode IsoMediaFile::handleMoovBox( AseBufferPtr pBuffer )
{
    if ( !mHeaderPtr )
    {
        mHeaderPtr = pBuffer;

        if ( mbParseMoovBox )
        {
            BoxReader reader( 0 /*offset*/, pBuffer->getFilledSpaceStart(), pBuffer->getFilledSpaceSize() );

            if ( !reader.readBoxHeader() )
                return AS_PARSE_ERROR;

            assert( reader.isType( MoovBoxReader::type ) );

            ullong duration = 0;
            ullong mvex_duration = 0;
            uint32_t timescale = 0;

            MoovBoxReader moovreader( reader );

            for( ; moovreader.good(); moovreader.nextBox() )
            {
                moovreader.readBoxHeader();
                if ( moovreader.isType( MvhdBoxReader::type ) )
                {
                    MvhdBoxReader mvhdreader ( moovreader );

                    ullong creation_time, modification_time;
                    uint32_t rate;
                    uint16_t volume;

                    if ( !mvhdreader.readParams( creation_time, modification_time, timescale, duration, rate, volume ) )
                        return AS_PARSE_ERROR;
                }
                else if ( moovreader.isType( MvexBoxReader::type ) )
                {
                    MvexBoxReader mvexreader( moovreader );

                    for( ; mvexreader.good(); mvexreader.nextBox() )
                    {
                        mvexreader.readBoxHeader();
                        if ( mvexreader.isType( MehdBoxReader::type ) )
                        {
                            MehdBoxReader mehdreader( mvexreader );

                            if ( !mehdreader.readDuration( mvex_duration ) )
                            {
                                return AS_PARSE_ERROR;
                            }

                            break;
                        }
                    }
                }
            }

            if ( timescale > 0 &&
                    ( ( mvex_duration > 0 ) || ( duration > 0 && duration != (ullong) -1 ) ) )
            {
                DEBUGLOG( IsoMediaFile, "[%s] Duration signalled in mehd: %llu / %u",
                                mManifestStreamPtr->getStreamKey().c_str(),
                                mvex_duration,
                                timescale );

                DEBUGLOG( IsoMediaFile, "[%s] Duration signalled in mvhd: %llu / %u",
                                mManifestStreamPtr->getStreamKey().c_str(),
                                duration,
                                timescale );

                DEBUGLOG( IsoMediaFile, "[%s] Current duration          : %llu / %llu",
                                mManifestStreamPtr->getStreamKey().c_str(),
                                mDuration.getNumberOfTicks(),
                                mDuration.getTicksPerSecond() );

                if ( !hasDuration() )
                {
                    if ( mvex_duration > 0 )
                    {
                        mDuration = AseTimeInterval( mvex_duration, timescale );
                    }
                    else
                    {
                        mDuration = AseTimeInterval( duration, timescale );
                    }
                }
            }
        }
    }

    return AS_NO_ERROR;
}
