/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, 0xInc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, 0xInc. is strictly
 * prohibited.
 */

#include "NflxHeaderReader.h"

using namespace netflix;
using namespace ase;

nf_uuid_t const NflxHeaderReader::type = make_uuid( 0x4e657466, 0x6c69, 0x7850, 0x6966, 0x665374726d21ULL );

bool NflxHeaderReader::readNflxHeader(  llong& fileSize,
                                        llong& timescale,
                                        llong& duration,
                                        llong& offsetToFirstMoof,
                                        llong& sizeOfAllMoofAndMDat,
                                        llong& offsetToNflxIdx,
                                        int32_t& nflxIdxSize,
                                        llong& offsetToFirstSidx,
                                        int32_t& firstSidxSize,
                                        nf_uuid_t& fileId )
{
    if ( !good()
            || !read( fileSize )
            || !read( timescale )
            || !read( duration )
            || !read( offsetToFirstMoof )
            || !read( sizeOfAllMoofAndMDat )
            || ( hasExtendedFields() &&
                    (      !read( offsetToNflxIdx )
                        || !read( nflxIdxSize )
                        || !read( offsetToFirstSidx )
                        || !read( firstSidxSize )
                        || !read( fileId ) ) ) )
        return false;

    return true;
}
