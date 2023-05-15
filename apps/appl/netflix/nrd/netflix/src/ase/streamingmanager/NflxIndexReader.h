/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NFLX_INDEX_READER_H
#define NFLX_INDEX_READER_H

#include "BoxReader.h"
#include "MfraBoxReader.h"

namespace netflix {
namespace ase {

class NflxTrackIndexReader;

class NflxIndexReader : public BoxReader
{
public:
    static nf_uuid_t const type;

    NflxIndexReader();

    NflxIndexReader( ullong offset, uint8_t const* pData, size_t size );

    NflxIndexReader( BoxReader const& other );

    bool readNflxTrackIndex( NflxTrackIndexReader& index );
};

/*  aligned(8) class NetflixTrackIndexBox
    extends fullbox("uuid", extended_type=6674654E-696C-5478-7261-6B496E646578) {
        int(64) timescale;
    }
*/

class NflxTrackIndexReader : public BoxReader
{
public:
    static nf_uuid_t const type;

    NflxTrackIndexReader();

    NflxTrackIndexReader( BoxReader const& other );

    bool readHeader( ullong& timescale );

    bool readTfra( TfraBoxReader& tfra );
};

inline NflxIndexReader::NflxIndexReader()
    : BoxReader()
{
}



inline bool NflxIndexReader::readNflxTrackIndex( NflxTrackIndexReader& index )
{
    return readBox( index );
}

inline NflxTrackIndexReader::NflxTrackIndexReader()
    : BoxReader()
{
}

inline bool NflxTrackIndexReader::readTfra( TfraBoxReader& tfra )
{
    return readBox( tfra );
}


}} // namespace netflix::ase

#endif
