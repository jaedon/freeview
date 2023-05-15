/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SidxReader.h"

using namespace std;
using namespace netflix;
using namespace ase;

fourcc_t const SidxReader::type = BoxReader::getFourcc( "sidx" );

/*
 aligned(8) class SegmentIndexBox extends FullBox('sidx', version, 0) {
     unsigned int(32) reference_ID;
     unsigned int(32) timescale;
     if (version==0)
     {
         unsigned int(32) earliest_presentation_time;
         unsigned int(32) first_offset;
     }
     else
     {
         unsigned int(64) earliest_presentation_time;
         unsigned int(64) first_offset;
     }
     unsigned int(16) reserved = 0;
     unsigned int(16) reference_count;
     for(i=1; i <= reference_count; i++)
     {
         bit (1)			reference_type;
         unsigned int(31)	referenced_size;
         unsigned int(32)	subsegment_duration;
         bit(1)				starts_with_SAP;
         unsigned int(3)	SAP_type;
         unsigned int(28)	SAP_delta_time;
     }
 }

*/

bool SidxReader::readHeader(    uint32_t& reference_ID,
                                uint32_t& timescale,
                                ullong& earliest_presentation_time,
                                ullong& first_offset )
{
    if ( !readFullBoxHeader() || !read( reference_ID ) || !read( timescale ) )
        return false;

    if ( getVersion() == 0 )
    {
        uint32_t presentation_time32 = 0, offset32 = 0;
        if ( !read( presentation_time32 ) || !read( offset32 ) )
            return false;
        earliest_presentation_time = presentation_time32;
        first_offset = offset32;
    }
    else
    {
        if ( !read( earliest_presentation_time ) || !read( first_offset ) )
            return false;
    }

    uint16_t reserved, reference_count16 = 0;
    if ( !read( reserved ) || !read( reference_count16 ) )
        return false;

    mReferenceCount = reference_count16;
    return true;
}

bool SidxReader::readEntry(    bool& reference_type,
                               uint32_t& referenced_size,
                               uint32_t& subsegment_duration,
                               bool& starts_with_SAP,
                               uint32_t& SAP_type,
                               uint32_t& SAP_delta_time )
{
    uint32_t reference_type32 = 0, starts_with_SAP32 = 0;

    if (!good()
        || !moreEntries()
        || !unpack32<1,31>( &reference_type32, &referenced_size )
        || !read( subsegment_duration )
        || !unpack32<1,3,28>( &starts_with_SAP32, &SAP_type, &SAP_delta_time ) )
        return false;

    --mReferenceCount;
    reference_type = ( reference_type32 != 0 );
    starts_with_SAP = ( starts_with_SAP32 != 0 );
    return true;
}
