/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, 0xInc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, 0xInc. is strictly
 * prohibited.
 */

#include "MoovBoxReader.h"

using namespace std;
using namespace netflix;
using namespace ase;

fourcc_t const MoovBoxReader::type = BoxReader::getFourcc( "moov" );
fourcc_t const MvhdBoxReader::type = BoxReader::getFourcc( "mvhd" );
fourcc_t const MvexBoxReader::type = BoxReader::getFourcc( "mvex" );
fourcc_t const MehdBoxReader::type = BoxReader::getFourcc( "mehd" );
fourcc_t const TrakBoxReader::type = BoxReader::getFourcc( "trak" );
fourcc_t const MdiaBoxReader::type = BoxReader::getFourcc( "mdia" );
fourcc_t const MdhdBoxReader::type = BoxReader::getFourcc( "mdhd" );
fourcc_t const EdtsBoxReader::type = BoxReader::getFourcc( "edts" );
fourcc_t const ElstBoxReader::type = BoxReader::getFourcc( "elst" );


bool MvhdBoxReader::readParams( ullong& creation_time,
                                ullong& modification_time,
                                uint32_t& timescale,
                                ullong& duration,
                                uint32_t& rate,
                                uint16_t& volume )
{
    if ( !readFullBoxHeader() )
        return false;

    if ( getVersion() == 1 )
    {
        if ( !read( creation_time )
            || !read( modification_time )
            || !read( timescale )
            || !read( duration )
            || !read( rate )
            || !read( volume ) )
            return false;
    }
    else
    {
        uint32_t creation_time32 = 0, modification_time32 = 0, duration32 = 0;
        if ( !read( creation_time32 )
            || !read( modification_time32 )
            || !read( timescale )
            || !read( duration32 )
            || !read( rate )
            || !read( volume ) )
            return false;

        creation_time = creation_time32;
        modification_time = modification_time32;
        duration = ( duration32 == 0xffffffff ) ? (ullong) -1 : duration32;
    }

    return true;
}

bool MvhdBoxReader::readMatrix( MvhdBoxReader::MatrixType& matrix )
{
    for( int i = 0; i < 9; ++i )
        if ( !read( matrix[ i ] ) )
            return false;
    return true;
}

bool MehdBoxReader::readDuration( ullong& fragment_duration )
{
    if ( !readFullBoxHeader() )
        return false;

    if ( getVersion() == 1 )
    {
        if ( !read( fragment_duration ) )
            return false;
    }
    else
    {
        uint32_t fragment_duration32 = 0;
        if ( !read( fragment_duration32 ) )
            return false;

        fragment_duration = fragment_duration32;
    }

    return true;
}

bool MdhdBoxReader::readParams( ullong& creation_time,
                                ullong& modification_time,
                                uint32_t& timescale,
                                ullong& duration,
                                language_t& language )
{
    if ( !readFullBoxHeader() )
        return false;

    if ( getVersion() == 1 )
    {
        if ( !read( creation_time )
            || !read( modification_time )
            || !read( timescale )
            || !read( duration ) )
            return false;
    }
    else
    {
        uint32_t creation_time32, modification_time32, duration32;

        if ( !read( creation_time32 )
            || !read( modification_time32 )
            || !read( timescale )
            || !read( duration32 ) )
            return false;

        creation_time = creation_time32;
        modification_time = modification_time32;
        duration = duration32;
    }

    uint32_t l1, l2, l3, pre_defined;
    if ( !unpack32<6,5,5,16>( &l1, &l2, &l3, &pre_defined ) )
        return false;

    language[ 0 ] = (char) ( l1 + 0x60 );
    language[ 1 ] = (char) ( l2 + 0x60 );
    language[ 2 ] = (char) ( l3 + 0x60 );

    return true;

}

bool ElstBoxReader::readHeader()
{
    return readFullBoxHeader() && read( mRemainingEntries );
}

bool ElstBoxReader::readEntry(  ullong& segment_duration,
                                llong& media_time,
                                int16_t& media_rate_integer,
                                int16_t& media_rate_fraction )
{
    --mRemainingEntries;

    if ( getVersion() == 1 )
    {
        if ( !read( segment_duration )
            || !read( media_time ) )
            return false;
    }
    else
    {
        uint32_t segment_duration32;
        int32_t media_time32;
        if ( !read( segment_duration32 )
            || !read( media_time32 ) )
            return false;

        segment_duration = segment_duration32;
        media_time = media_time32;
    }

    return read( media_rate_integer ) && read( media_rate_fraction );
}

