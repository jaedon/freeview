/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NFLX_HEADER_READER_H
#define NFLX_HEADER_READER_H

#include "BoxReader.h"

namespace netflix {
namespace ase {

/*

 aligned(8) class NetflixHeaderBox extends fullbox("uuid",
 extended_type=6674654E-696C-5078-6966-665374726D21, version = 0,  flags = 0)
 {
 int(64) fileSize;
 int(64) timescale;
 int(64) duration;
 int(64) offsetToFirstMoof;
 if ( version == 1 )
 {
    int(64) sizeOfAllMoofAndMDat;
    int(64) offsetToNflxIdx;
    int(32) nflxIdxSize;
    int(64) offsetToFirstSidx;
    int(32) firstSidxSize;
    GUID fileId;
 }

*/

class NflxHeaderReader : public BoxReader
{
public:
    NflxHeaderReader( BoxReader const& other );

    static nf_uuid_t const type;

    bool hasExtendedFields() const;

    bool readNflxHeader(    llong& fileSize,
                            llong& timescale,
                            llong& duration,
                            llong& offsetToFirstMoof,
                            llong& sizeOfAllMoofAndMDat,
                            llong& offsetToNflxIdx,           // extended version
                            int32_t& nflxIdxSize,               // extended version
                            llong& offsetToFirstSidx,         // extended version
                            int32_t& firstSidxSize,             // extended version
                            nf_uuid_t& fileId );                // extended version
};

inline NflxHeaderReader::NflxHeaderReader( BoxReader const& other )
    : BoxReader( other )
{
    readFullBoxHeader();
}

inline bool NflxHeaderReader::hasExtendedFields() const
{
    return ( getVersion() > 0 );
}

}} // namespace netflix::ase

#endif
