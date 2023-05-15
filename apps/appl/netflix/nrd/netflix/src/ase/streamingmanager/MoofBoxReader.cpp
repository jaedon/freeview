/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MoofBoxReader.h"

using namespace netflix;
using namespace ase;

fourcc_t const MoofBoxReader::type = BoxReader::getFourcc( "moof" );
fourcc_t const TrafBoxReader::type = BoxReader::getFourcc( "traf" );
fourcc_t const TfhdBoxReader::type = BoxReader::getFourcc( "tfhd" );
fourcc_t const TrunBoxReader::type = BoxReader::getFourcc( "trun" );
fourcc_t const TfdtBoxReader::type = BoxReader::getFourcc( "tfdt" );

bool TfhdBoxReader::readHeader(
                        uint32_t&   track_ID,
                        bool&       base_data_offset_present,
                        ullong&     base_data_offset,
                        bool&       sample_description_index_present,
                        uint32_t&   sample_description_index,
                        bool&       default_sample_duation_present,
                        uint32_t&   default_sample_duation,
                        bool&       default_sample_size_present,
                        uint32_t&   default_sample_size,
                        bool&       default_sample_flags_present,
                        uint32_t&   default_sample_flags,
                        bool&       duration_is_empty,
                        bool&       default_base_is_moof  )
{
    if ( !readFullBoxHeader() )
        return false;

    uint32_t const flags = getFlags();

    base_data_offset_present = flags & 0x1;
    sample_description_index_present = flags & 0x2;
    default_sample_duation_present = flags & 0x8;
    default_sample_size_present = flags & 0x10;
    default_sample_flags_present = flags & 0x20;
    duration_is_empty = flags & 0x010000;
    default_base_is_moof = flags & 0x020000;

    return read( track_ID )
            && ( !base_data_offset_present || read( base_data_offset ) )
            && ( !sample_description_index_present || read( sample_description_index ) )
            && ( !default_sample_duation_present || read( default_sample_duation ) )
            && ( !default_sample_size_present || read( default_sample_size ) )
            && ( !default_sample_flags_present || read( default_sample_flags ) );

}

bool TrunBoxReader::readHeader( uint32_t& sample_count,
                                bool& data_offset_present,
                                uint32_t& data_offset,
                                bool& first_sample_flags_present,
                                uint32_t& first_sample_flags,
                                bool& sample_duration_present,
                                bool& sample_size_present,
                                bool& sample_flags_present,
                                bool& sample_composition_time_offsets_present )
{
    if ( !readFullBoxHeader() || !read( mSampleCount ) )
        return false;

    sample_count = mSampleCount;

    uint32_t const flags = getFlags();


    data_offset_present = flags & 0x1;
    first_sample_flags_present = flags & 0x4;
    sample_duration_present = mSampleDurationPresent = flags & 0x100;
    sample_size_present = mSampleSizePresent = flags & 0x200;
    sample_flags_present = mSampleFlagsPresent = flags & 0x400;
    sample_composition_time_offsets_present = mSampleCompositionTimeOffsetsPresent = flags & 0x800;

    return ( !data_offset_present || read( data_offset ) )
                && ( !first_sample_flags_present || read( first_sample_flags ) );
}

bool TrunBoxReader::moreEntries() const
{
    return mSampleCount > 0;
}

bool TrunBoxReader::readEntry(  uint32_t& sample_duration,
                                uint32_t& sample_size,
                                uint32_t& sample_flags,
                                int32_t& sample_composition_time_offset )
{
    if ( mSampleCount == 0 )
        return false;

    --mSampleCount;

    return ( !mSampleDurationPresent || read( sample_duration ) )
        && ( !mSampleSizePresent || read( sample_size ) )
        && ( !mSampleFlagsPresent || read( sample_flags ) )
        && ( !mSampleCompositionTimeOffsetsPresent || read( sample_composition_time_offset ) );
}

bool TfdtBoxReader::readHeader( llong& baseMediaDecodeTime )
{
    if ( !readFullBoxHeader() )
        return false;

    if ( getVersion() == 1 )
    {
        return read( baseMediaDecodeTime );
    }

    int32_t baseMediaDecodeTime32;
    if ( !read( baseMediaDecodeTime32 ) )
        return false;

    baseMediaDecodeTime = baseMediaDecodeTime32;

    return true;

}

