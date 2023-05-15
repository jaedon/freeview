/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdase/ByteRange.h>
#include <sstream>

using namespace netflix;
using namespace netflix::ase;

std::pair<ByteRange,ByteRange> ByteRange::excise( ByteRange const& other ) const
{
    if ( !this->valid() )
        return std::make_pair( ByteRange(), ByteRange() );

    if ( !other.valid() )
        return std::make_pair( *this, ByteRange() );

    std::pair<ByteRange,ByteRange> result;

    if ( this->start() < other.start() )
    {
        result.first = ByteRange( this->start(), other.start() - 1 );
    }


    if ( other.closed() && ( ( this->open() ) || ( other.end() < this->end() ) ) )
    {
        result.second = ByteRange( other.end() + 1, this->open() ? -1 : this->end() );
    }

    return result;
}

std::string ByteRange::toByteRangeSpec() const
{
    std::ostringstream oss;

    if ( mStartIndex >= 0 )
    {
        oss << mStartIndex << '-';
        if ( mEndIndex >= 0 )
        {
            oss << mEndIndex;
        }
    }

    return oss.str();
}
