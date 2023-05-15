/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SIDX_READER_H
#define SIDX_READER_H

#include "BoxReader.h"

namespace netflix {
namespace ase {

class SidxReader : public BoxReader
{
public:
    static fourcc_t const type;

    SidxReader();

    SidxReader( ullong offset, uint8_t const* pData, size_t size );

    SidxReader( BoxReader const& other );

    bool goodForHeader() const;

    bool readHeader(    uint32_t& reference_ID,
                        uint32_t& timescale,
                        ullong& earliest_presentation_time,
                        ullong& first_offset );

    bool moreEntries() const;

    bool goodForEntry() const;

    bool readEntry( bool& reference_type,
                    uint32_t& referenced_size,
                    uint32_t& subsegment_duration,
                    bool& starts_with_SAP,
                    uint32_t& SAP_type,
                    uint32_t& SAP_delta_time );

protected:
    uint32_t mReferenceCount;
};

inline SidxReader::SidxReader()
    : BoxReader()
    , mReferenceCount( 0 )
{
}

inline SidxReader::SidxReader( ullong offset, uint8_t const* pData, size_t size )
    : BoxReader( offset, pData, size )
    , mReferenceCount( 0 )
{
}

inline SidxReader::SidxReader( BoxReader const& other )
    : BoxReader( other )
    , mReferenceCount( 0 )
{
}

inline bool SidxReader::goodForHeader() const
{
    return goodfor( 4 + 4 + 4 + 8 + 8 + 4 );
}

inline bool SidxReader::moreEntries() const
{
    return ( mReferenceCount > 0 );
}

inline bool SidxReader::goodForEntry() const{
    return goodfor( 12 );
}

}}
#endif
