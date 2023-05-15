/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MfraBoxReader.h"

using namespace netflix;
using namespace ase;

const uint32_t TfraBoxReader::type = BoxReader::getFourcc( "tfra" );

MfraBoxReader::MfraBoxReader( BoxReader const& other )
    : BoxReader( other )
{
    readFullBoxHeader();
}

TfraBoxReader::TfraBoxReader( BoxReader const& other )
    : BoxReader( other )
{
    readFullBoxHeader();
}

bool TfraBoxReader::readHeader( uint32_t& trackId  )
{
    uint32_t reserved;
    if (!read( trackId )
        || !unpack32<26,2,2,2>( &reserved,
                                &mSizeOfTrafNum,
                                &mSizeOfTrunNum,
                                &mSizeOfSampleNum )
        || !read( mRemainingEntries ) )
        return false;

    ++mSizeOfTrafNum;
    ++mSizeOfTrunNum;
    ++mSizeOfSampleNum;

    return true;
}

bool TfraBoxReader::moreEntries() const
{
    return ( mRemainingEntries > 0 );
}

bool TfraBoxReader::goodForEntry() const
{
    size_t const bytes = ( ( mVersion == 1 ) ? 16 : 8 ) + mSizeOfTrafNum + mSizeOfTrunNum + mSizeOfSampleNum;
    return goodfor( bytes );
}

bool TfraBoxReader::readEntry(  ullong& time,
                                ullong& offset,
                                uint32_t& traf_number,
                                uint32_t& trun_number,
                                uint32_t& sample_number )
{
    if ( mVersion == 1 )
    {
        if ( !read( time ) || !read( offset  ) )
            return false;
    }
    else
    {
        uint32_t time32 = 0, offset32 = 0;
        if ( !read( time32 ) || !read( offset32  ) )
            return false;
        time = time32;
        offset = offset32;
    }

    if ( !read( traf_number, mSizeOfTrafNum )
        || !read( trun_number, mSizeOfTrunNum )
        || !read( sample_number, mSizeOfSampleNum ) )
        return false;

    --mRemainingEntries;

    return true;
}
