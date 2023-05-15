/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NflxIsoMediaFile.h"
#include "NflxHeaderReader.h"
#include "NflxIndexReader.h"
#include <nrdase/ITrackInfo.h>

using namespace std;
using namespace netflix;
using namespace netflix::ase;

DEFINE_LOGGER( netflix::ase::NflxIsoMediaFile );

static const fourcc_t PIFF_4CC = BoxReader::getFourcc( "piff" );

namespace netflix { namespace ase {

struct NflxBrandHandler : public IsoMediaFile::BrandHandler
{
    virtual AseErrorCode operator()( IsoMediaFilePtr pIsoMediaFile, bool& index )
    {
        NRDP_UNUSED(pIsoMediaFile);
        return mpNflxIsoMediaFile->handleNflxBrand( index );
    }

    // This is just to avoid a downcast ...
    NflxBrandHandler( NflxIsoMediaFile* pNflxIsoMediaFile )
        : mpNflxIsoMediaFile( pNflxIsoMediaFile ) {}

    NflxIsoMediaFile* mpNflxIsoMediaFile;
};

struct NflxHeaderBoxHandler : public IsoMediaFile::BoxHandler
{
    virtual AseErrorCode init(  IsoMediaFilePtr     pIsoMediaFile,
                                BoxReader const&    reader )
    {
        NRDP_UNUSED(pIsoMediaFile);
        return mpNflxIsoMediaFile->handleNflxHeaderBox( reader );
    }

    virtual AseErrorCode process( uint8_t const *pData, size_t size )
    {
        NRDP_UNUSED( pData );
        NRDP_UNUSED( size );
        assert( false );
        return 0;
    }

    // This is just to avoid a downcast ...
    NflxHeaderBoxHandler( NflxIsoMediaFile* pNflxIsoMediaFile )
        : mpNflxIsoMediaFile( pNflxIsoMediaFile ) {}

    NflxIsoMediaFile* mpNflxIsoMediaFile;
};

class NflxIndexBoxHandler : public IsoMediaFile::BufferedBoxHandler
{
public:
    virtual AseErrorCode doProcess();

    NflxIndexBoxHandler( NflxIsoMediaFile* pNflxIsoMediaFile );

protected:
    typedef enum
    {
        NFLX_IDX_PARSE_UNINIT = 0,
        NFLX_IDX_PARSE_INIT,
        NFLX_IDX_PARSE_FIRST_ENTRY,
        NFLX_IDX_PARSE_ENTRIES,
        NFLX_IDX_PARSE_COMPLETE
    } ParseState;

protected:
    NflxIsoMediaFile*   mpNflxIsoMediaFile;
    ParseState          mParseState;
    NflxIndexReader     mNflxIndexReader;
    TfraBoxReader       mTfra;
    ullong            mPresentationTime;
    ullong            mOffset;
    ullong            mIndexTimescale;
};

struct NflxBoxHandlerFactory : public IsoMediaFile::BoxHandlerFactory
{
    virtual IsoMediaFile::BoxHandlerPtr createBoxHandler( BoxReader const& reader )
    {
        if ( reader.isType( NflxHeaderReader::type ) )
            return IsoMediaFile::BoxHandlerPtr( new NflxHeaderBoxHandler( mpNflxIsoMediaFile ) );
        else if ( reader.isType( NflxIndexReader::type ) )
            return IsoMediaFile::BoxHandlerPtr( new NflxIndexBoxHandler( mpNflxIsoMediaFile ) );
        return IsoMediaFile::BoxHandlerPtr();
    }

    // This is just to avoid a downcast ...
    NflxBoxHandlerFactory( NflxIsoMediaFile* pNflxIsoMediaFile )
        : mpNflxIsoMediaFile( pNflxIsoMediaFile ) {}

    NflxIsoMediaFile* mpNflxIsoMediaFile;
};

}}

NflxIndexBoxHandler::NflxIndexBoxHandler( NflxIsoMediaFile* pNflxIsoMediaFile )
    : mpNflxIsoMediaFile( pNflxIsoMediaFile )
    , mParseState( NFLX_IDX_PARSE_UNINIT )
    , mOffset( 0 )
{
}

AseErrorCode NflxIndexBoxHandler::doProcess()
{
    size_t const minbytes = 20       // Netflix index box header (extended type)
                            + 24     // Netflix track index box header (full extended type)
                            + 8      // Timescale
                            + 12     // Tfra header (full box)
                            + 12;    // Tfra initial fields

    // Give the reader everything in the buffer
    if ( mParseState == NFLX_IDX_PARSE_UNINIT )
    {
        mNflxIndexReader = NflxIndexReader( mBoxOffset, mBufferedBoxPtr->getFilledSpaceStart(), mBufferedBoxPtr->getFilledSpaceSize() );
        mNflxIndexReader.readBoxHeader();
        mParseState = NFLX_IDX_PARSE_INIT;
    }
    else
    {
        if ( !mNflxIndexReader.extend(  mBufferedBoxPtr->getFilledSpaceStart(),
                                        mBufferedBoxPtr->getFilledSpaceSize() ) )
        {
            assert( false );
        }
    }

    switch( mParseState )
    {
        case NFLX_IDX_PARSE_UNINIT:
            assert( false );

        case NFLX_IDX_PARSE_INIT:

            if ( !mNflxIndexReader.goodfor( minbytes ) )
                break;

            {
                NflxTrackIndexReader nflxTrackIndexReader;
                uint32_t trackId;

                if (!mNflxIndexReader.readNflxTrackIndex( nflxTrackIndexReader )
                    || !nflxTrackIndexReader.readHeader( mIndexTimescale )
                    || !nflxTrackIndexReader.readTfra( mTfra )
                    || !mTfra.readHeader( trackId ) )
                {
                    return AS_PARSE_ERROR;
                }

                mParseState = NFLX_IDX_PARSE_ENTRIES;
            }

            // Fall through
        case NFLX_IDX_PARSE_FIRST_ENTRY:
        case NFLX_IDX_PARSE_ENTRIES:
            {
                if ( !mTfra.extend( mBufferedBoxPtr->getFilledSpaceStart(),
                                    mBufferedBoxPtr->getFilledSpaceSize() ) )
                {
                    assert( false );
                }

                while( mTfra.moreEntries() && mTfra.goodForEntry() )
                {
                    ullong time, offset;
                    uint32_t traf_number, trun_number, sample_number;
                    if ( !mTfra.readEntry( time, offset, traf_number, trun_number, sample_number ) )
                        return AS_PARSE_ERROR;

                    if ( mParseState == NFLX_IDX_PARSE_FIRST_ENTRY )
                    {
                        mpNflxIsoMediaFile->mIndexStart = AseTimeInterval( time, mIndexTimescale );
                        mpNflxIsoMediaFile->mIndexOffset = offset;
                        mParseState = NFLX_IDX_PARSE_ENTRIES;
                    }

                    AseTimeInterval const dur( time - mPresentationTime, mIndexTimescale );
                    MediaFragmentInfo const frag( dur, offset - mOffset );

                    mpNflxIsoMediaFile->mIndexPtr->push_back( frag );

                    mPresentationTime = time;
                    mOffset = offset;
                }

                mpNflxIsoMediaFile->mIndexEnd = AseTimeInterval( mPresentationTime, mIndexTimescale );

                if ( mTfra.moreEntries() )
                    break;

                AseTimeInterval const lastdur = mpNflxIsoMediaFile->mDuration - AseTimeInterval( mPresentationTime, mIndexTimescale );
                MediaFragmentInfo const lastfrag( lastdur, mpNflxIsoMediaFile->mFileSize - mOffset );

                mpNflxIsoMediaFile->mIndexPtr->push_back( lastfrag );

                mpNflxIsoMediaFile->mbIndexComplete = true;

                mParseState = NFLX_IDX_PARSE_COMPLETE;
                break;
            }

        case NFLX_IDX_PARSE_COMPLETE:
            // This implies extra bytes at the end of the box
            // Let's follow Postel's rule and ignore them
            break;

    }

    return AS_NO_ERROR;
}

AseErrorCode NflxIsoMediaFile::construct(   IAseConfigParameterPtr pConfigParameters,
                                            ConstManifestStreamPtr pManifestSteam,
                                            IMediaFilePtr& pIsoMediaFile )
{
    NflxIsoMediaFilePtr pNewObject( new NflxIsoMediaFile( pManifestSteam ) );

    AseErrorCode res = pNewObject->initialize( pConfigParameters );
    if ( res != AS_NO_ERROR )
        return res;

    pIsoMediaFile = pNewObject;

    return AS_NO_ERROR;
}

NflxIsoMediaFile::NflxIsoMediaFile( ConstManifestStreamPtr pManifestSteam )
    : DashIsoMediaFile( pManifestSteam )
    , mpNflxBrandHandler( )
    , mpNflxBoxHandlerFactory( )
    , mIndexOffset( 0 )
    , mIndexSize( 0 )
    , mbNeedNflxIndex( false )
{
    mpNflxBrandHandler = new NflxBrandHandler( this );
    mpNflxBoxHandlerFactory = new NflxBoxHandlerFactory( this );

    // Some Netflix Piff files are not marked as such: so treat this as the default if no
    // other brand is recognized.
    registerDefaultBrand( *mpNflxBrandHandler );
    //registerBrand( PIFF_4CC, *mpNflxBrandHandler );
    registerBox( NflxHeaderReader::type, *mpNflxBoxHandlerFactory );
}

AseErrorCode NflxIsoMediaFile::initialize( IAseConfigParameterPtr pConfigParameters )
{
    AseErrorCode retVal = this->DashIsoMediaFile::initialize( pConfigParameters );
    if ( retVal != AS_NO_ERROR )
        return retVal;

    if ( !mpNflxBrandHandler || !mpNflxBoxHandlerFactory )
        return AS_NOT_ENOUGH_MEMORY;

    return AS_NO_ERROR;
}

AseErrorCode NflxIsoMediaFile::handleNflxBrand( bool& index )
{
    // We could push the Netflix header here, but we don't really need it
    // setBoxSearch( NflxHeaderReader::type )
    if ( index )
    {
        DEBUGLOG( NflxIsoMediaFile, "Saw piff brand but another brand is providing the index" );
    }
    else
    {
        DEBUGLOG( NflxIsoMediaFile, "Looking for Netflix index for piff brand file" );
        registerBox( NflxIndexReader::type, *mpNflxBoxHandlerFactory );

        // No higher priority brand provides an index
        setBoxSearch( NflxHeaderReader::type );
        setBoxSearch( NflxIndexReader::type );
        index = true;
        mbNeedNflxIndex = true;
    }

    return AS_NO_ERROR;
}

AseErrorCode NflxIsoMediaFile::handleNflxHeaderBox( BoxReader const& reader )
{
    if ( !reader.complete() )
        return AS_MORE;

    NflxHeaderReader nflxReader( reader );

    llong fileSize;
    llong timescale;
    llong duration;
    llong offsetToFirstMoof;
    llong sizeOfAllMoofAndMDat;
    llong offsetToNflxIdx;
    int32_t nflxIdxSize;
    llong offsetToFirstSidx;
    int32_t firstSidxSize;
    nf_uuid_t fileId;

    if ( !nflxReader.readNflxHeader(    fileSize,
                                        timescale,
                                        duration,
                                        offsetToFirstMoof,
                                        sizeOfAllMoofAndMDat,
                                        offsetToNflxIdx,
                                        nflxIdxSize,
                                        offsetToFirstSidx,
                                        firstSidxSize,
                                        fileId ) )
    {
        return AS_PARSE_ERROR;
    }

    DEBUGLOG( NflxIsoMediaFile,
                "fileSize=%lld, timescale=%lld, duration=%lld,"
                "offsetToFirstMoof=%lld,sizeOfAllMoofAndMDat=%lld,"
                "offsetToNflxIdx=%lld,nflxIdxSize=%d,offsetToFirstSidx=%lld,"
                "firstSidxSize=%d",
             fileSize,
             timescale,
             duration,
             offsetToFirstMoof,
             sizeOfAllMoofAndMDat,
             offsetToNflxIdx,
             nflxIdxSize,
             offsetToFirstSidx,
             firstSidxSize );

    provideFileSize( fileSize );

    provideDuration( AseTimeInterval( duration, timescale ) );

    clearBoxSearch( NflxHeaderReader::type );

    if ( nflxReader.hasExtendedFields() )
    {
        mIndexOffset = offsetToNflxIdx;
        mIndexSize = nflxIdxSize;

        // No longer need to search for the index
        clearBoxSearch( NflxIndexReader::type );

        // But if the brand indicates we need the Nflx index then we'd better get it!
        if ( mbNeedNflxIndex )
            requestByteRange( ByteRange( mIndexOffset, mIndexOffset + mIndexSize - 1 ) );
    }

    return AS_NO_ERROR;
}

NflxIsoMediaFile::~NflxIsoMediaFile()
{
    if ( mpNflxBrandHandler )
        delete mpNflxBrandHandler;
    if ( mpNflxBoxHandlerFactory )
        delete mpNflxBoxHandlerFactory;
}
