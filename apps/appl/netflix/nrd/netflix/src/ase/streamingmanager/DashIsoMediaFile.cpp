/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DashIsoMediaFile.h"
#include "BoxReader.h"
#include "SidxReader.h"
#include <nrdase/ITrackInfo.h>

using namespace std;
using namespace netflix;
using namespace ase;

static const fourcc_t DASH_4CC = BoxReader::getFourcc( "dash" );

namespace netflix { namespace ase {

struct DashBrandHandler : public IsoMediaFile::BrandHandler
{
    virtual AseErrorCode operator()( IsoMediaFilePtr pIsoMediaFile, bool& index )
    {
        NRDP_UNUSED(pIsoMediaFile);
        return mpDashIsoMediaFile->handleDashBrand( index );
    }

    // To avoid downcasting ...
    DashBrandHandler( DashIsoMediaFile* pDashIsoMediaFile ) : mpDashIsoMediaFile( pDashIsoMediaFile ) {}
    DashIsoMediaFile* mpDashIsoMediaFile;
};

class SidxBoxHandler : public IsoMediaFile::BufferedBoxHandler
{
public:
    virtual AseErrorCode doProcess();

    SidxBoxHandler( DashIsoMediaFile* pDashIsoMediaFile );

protected:
    typedef enum
    {
        SIDX_PARSE_UNINIT = 0,
        SIDX_PARSE_INIT,
        SIDX_PARSE_ENTRIES,
        SIDX_PARSE_COMPLETE
    } ParseState;

protected:
    DashIsoMediaFile*   mpDashIsoMediaFile;
    ParseState          mParseState;
    SidxReader          mSidxReader;
    ullong            mAnchor;
    uint32_t            mReference_ID;
    uint32_t            mTimescale;
    ullong            mEarliest_presentation_time;
    ullong            mPresentation_time;
    ullong            mBox_offset;

};

struct SidxBoxHandlerFactory : public IsoMediaFile::BoxHandlerFactory
{
    virtual IsoMediaFile::BoxHandlerPtr createBoxHandler( BoxReader const& reader )
    {
        NRDP_UNUSED( reader );
        return IsoMediaFile::BoxHandlerPtr( new SidxBoxHandler( mpDashIsoMediaFile ) );
    }

    SidxBoxHandlerFactory( DashIsoMediaFile* pDashIsoMediaFile )
        : mpDashIsoMediaFile( pDashIsoMediaFile )
    {}

    DashIsoMediaFile* mpDashIsoMediaFile;
};

}}

SidxBoxHandler::SidxBoxHandler( DashIsoMediaFile* pDashIsoMediaFile )
    : mpDashIsoMediaFile( pDashIsoMediaFile )
    , mParseState( SIDX_PARSE_UNINIT )
{
}

AseErrorCode SidxBoxHandler::doProcess()
{
    DEBUGLOG( IsoMediaFile, "[%s] Segment index: state %d, data %zu of %zu",
                                mpDashIsoMediaFile->mManifestStreamPtr->getStreamKey().c_str(),
                                mParseState,
                                mBufferedBoxPtr->getFilledSpaceSize(),
                                mBufferedBoxPtr->getCapacity() );

    // Give the reader everything in the buffer
    if ( mParseState == SIDX_PARSE_UNINIT )
    {
        mSidxReader = SidxReader( mBoxOffset, mBufferedBoxPtr->getFilledSpaceStart(), mBufferedBoxPtr->getFilledSpaceSize() );
        mSidxReader.readBoxHeader();
        mParseState = SIDX_PARSE_INIT;
    }
    else
    {
        if ( !mSidxReader.extend(   mBufferedBoxPtr->getFilledSpaceStart(),
                                    mBufferedBoxPtr->getFilledSpaceSize() ) )
        {
            assert( false );
        }
    }

    switch ( mParseState )
    {
        case SIDX_PARSE_UNINIT:
            assert( false );

        case SIDX_PARSE_INIT:

            if ( !mSidxReader.goodForHeader() )
                return AS_NO_ERROR;

            if ( !mSidxReader.readHeader(    mReference_ID,
                                                    mTimescale,
                                                    mEarliest_presentation_time,
                                                    mBox_offset ) )
                return AS_PARSE_ERROR;

            // For embedded index the anchor is the next byte after the index
            mAnchor = mSidxReader.getOffset() + mSidxReader.getSize();

            mPresentation_time = 0;  // ignore earliest_presentation_time
            mpDashIsoMediaFile->mIndexStart = AseTimeInterval( mPresentation_time, mTimescale );
            mpDashIsoMediaFile->mMovieFragmentsOffset = mAnchor + mBox_offset;

            mParseState = SIDX_PARSE_ENTRIES;

            // Fall through

        case SIDX_PARSE_ENTRIES:

            while( mSidxReader.moreEntries() && mSidxReader.goodForEntry() )
            {
                bool        reference_type;
                uint32_t    referenced_size;
                uint32_t    subsegment_duration;
                bool        starts_with_SAP;
                uint32_t    SAP_type;
                uint32_t    SAP_delta_time;

                if ( !mSidxReader.readEntry(    reference_type,
                                                referenced_size,
                                                subsegment_duration,
                                                starts_with_SAP,
                                                SAP_type,
                                                SAP_delta_time ) )
                    return AS_PARSE_ERROR;

                if ( reference_type || !starts_with_SAP )
                    return AS_PARSE_ERROR;

                mPresentation_time += subsegment_duration;

                AseTimeInterval const dur( subsegment_duration, mTimescale );
                MediaFragmentInfo const frag( dur, referenced_size );

                mpDashIsoMediaFile->mIndexPtr->push_back( frag );

                if ( mpDashIsoMediaFile->mFragmentDuration.isZero() )
                {
                    mpDashIsoMediaFile->mFragmentDuration = dur;
                }
            }

            DEBUGLOG( IsoMediaFile, "[%s] Segment index size now %zu, presentation time %llu",
                                mpDashIsoMediaFile->mManifestStreamPtr->getStreamKey().c_str(),
                                mpDashIsoMediaFile->mIndexPtr->size(),
                                mPresentation_time );

            mpDashIsoMediaFile->mIndexEnd = AseTimeInterval( mPresentation_time, mTimescale );

            if ( mSidxReader.moreEntries() )
                break;

            mpDashIsoMediaFile->mbIndexComplete = true;

            mParseState = SIDX_PARSE_COMPLETE;

            DEBUGLOG( IsoMediaFile, "Segment index: state %d, done!", mParseState );

            break;

        case SIDX_PARSE_COMPLETE:

            // This implies extra bytes at the end of the box
            // Let's follow Postel's rule and ignore them
            break;
    }

    return AS_NO_ERROR;
}

AseErrorCode DashIsoMediaFile::construct(   IAseConfigParameterPtr pConfigParameters,
                                            ConstManifestStreamPtr pManifestSteam,
                                            IMediaFilePtr& pIsoMediaFile )
{
    DashIsoMediaFilePtr pNewObject( new DashIsoMediaFile( pManifestSteam ) );

    AseErrorCode res = pNewObject->initialize( pConfigParameters );
    if ( res != AS_NO_ERROR )
        return res;

    pIsoMediaFile = pNewObject;
    return AS_NO_ERROR;
}

DashIsoMediaFile::DashIsoMediaFile( ConstManifestStreamPtr pManifestSteam )
    : IsoMediaFile( pManifestSteam )
    , mpDashBrandHandler( )
    , mpSidxBoxHandlerFactory( )
{
    mpDashBrandHandler.reset( new DashBrandHandler( this ) );
    mpSidxBoxHandlerFactory.reset( new SidxBoxHandlerFactory( this ) );

    registerBrand( DASH_4CC, *mpDashBrandHandler );
}

AseErrorCode DashIsoMediaFile::initialize( IAseConfigParameterPtr pConfigParameters )
{
    AseErrorCode retVal = this->IsoMediaFile::initialize( pConfigParameters );
    if ( retVal != AS_NO_ERROR )
        return retVal;

    if ( mpDashBrandHandler.get() == 0 || mpSidxBoxHandlerFactory.get() == 0 )
        return AS_NOT_ENOUGH_MEMORY;

    return AS_NO_ERROR;
}

AseErrorCode DashIsoMediaFile::handleDashBrand( bool& index )
{
    DEBUGLOG( IsoMediaFile, "Initializing DASH brand, index=%d", index );

    if ( !index )
    {
        registerBox( SidxReader::type, *mpSidxBoxHandlerFactory );
        setBoxSearch( SidxReader::type );
        index = true;
    }

    return AS_NO_ERROR;
}

DashIsoMediaFile::~DashIsoMediaFile()
{
}
