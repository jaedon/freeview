/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NflxIndexReader.h"

using namespace netflix;
using namespace ase;

nf_uuid_t const NflxIndexReader::type = make_uuid( 0x4e657466,
                                                   0x6c69,
                                                   0x7850,
                                                   0x6966,
                                                   0x66496e646578ULL );
/*
                    {   0x4e, 0x65, 0x74, 0x66,
                        0x6c, 0x69,
                        0x78, 0x50,
                        0x69, 0x66,
                        0x66, 0x49, 0x6e, 0x64, 0x65, 0x78 }
*/
nf_uuid_t const NflxTrackIndexReader::type = make_uuid( 0x4e657466,
                                                        0x6c69,
                                                        0x7854,
                                                        0x7261,
                                                        0x6b496e646578ULL );
/*
                    {   0x4e, 0x65, 0x74, 0x66,
                        0x6c, 0x69,
                        0x78, 0x54,
                        0x72, 0x61,
                        0x6b, 0x49, 0x6e, 0x64, 0x65, 0x78 }
*/

NflxIndexReader::NflxIndexReader( ullong offset, uint8_t const* pData, size_t size )
    : BoxReader( offset, pData, size )
{
}

NflxIndexReader::NflxIndexReader( BoxReader const& other )
    : BoxReader( other )
{
}

NflxTrackIndexReader::NflxTrackIndexReader( BoxReader const& other )
    : BoxReader( other )
{
}

bool NflxTrackIndexReader::readHeader( ullong& timescale )
{
    readFullBoxHeader();
    return read( timescale );
}
