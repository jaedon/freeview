/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BYTE_ByteRanges_H
#define BYTE_ByteRanges_H

#include <vector>

#include <nrdase/ByteRange.h>

namespace netflix {
namespace ase
{

/** Class that represents a set of ByteRanges */
class ByteRanges
{
public:
    typedef std::vector<ByteRange >            ByteRangesVector;
    typedef ByteRangesVector::const_iterator   const_iterator;

    // Empty range
    ByteRanges();

    // ByteRanges with single initial range
    ByteRanges( ByteRange const& range );

    ByteRanges& operator=( ByteRange const& range );

    bool operator==( ByteRange const& range );

    bool empty() const;

    bool contiguous() const;

    ByteRange span() const;

    ByteRange front() const;

    ByteRange back() const;

    bool disjoint( ByteRange const& range ) const;

    bool disjoint( ByteRanges const& ranges ) const;

    bool contains( ByteRange const& range ) const;

    bool contains( ByteRanges const& ranges ) const;

    int32_t size() const;

    void clear();

    // union
    ByteRanges operator|( ByteRange const& rhs ) const;
    ByteRanges& operator|=( ByteRange const& rhs );
    ByteRanges operator|( ByteRanges const& rhs ) const;
    ByteRanges& operator|=( ByteRanges const& rhs );

    // optimized union for adding a non-empty non-overlapping closed range
    ByteRanges& append( ByteRange const& rhs );

    // excision
    ByteRanges operator-( ByteRange const& rhs ) const;
    ByteRanges& operator-=( ByteRange const& rhs );
    ByteRanges operator-( ByteRanges const& rhs ) const;
    ByteRanges& operator-=( ByteRanges const& rhs );

    //intersection
    ByteRanges operator&( ByteRange const& rhs ) const;
    ByteRanges& operator&=( ByteRange const& rhs );
    ByteRanges operator&( ByteRanges const& rhs ) const;
    ByteRanges& operator&=( ByteRanges const& rhs );

    // iterators
    const_iterator begin() const    { return mByteRanges.begin(); }
    const_iterator end() const      { return mByteRanges.end(); }

protected:
    // Sorted dis-contiguous non-empty ByteRanges
    ByteRangesVector mByteRanges;
};

ByteRanges operator-( ByteRange const& lhs, ByteRange const& rhs );

ByteRanges operator-( ByteRange const& lhs, ByteRanges const& rhs );

// ByteRanges

inline ByteRanges::ByteRanges()
{
}

inline ByteRanges::ByteRanges( ByteRange const& range )
{
    mByteRanges.push_back( range );
}

inline ByteRanges& ByteRanges::operator=( ByteRange const& range )
{
    mByteRanges.clear();
    mByteRanges.push_back( range );
    return *this;
}

inline bool ByteRanges::operator==( ByteRange const& range )
{
    return contiguous() && front() == range;
}

inline bool ByteRanges::empty() const
{
    return mByteRanges.empty();
}

inline bool ByteRanges::contiguous() const
{
    return ( mByteRanges.size() == 1 );
}

inline ByteRange ByteRanges::span() const
{
    return empty() ? ByteRange() : ByteRange( mByteRanges.front().start(), mByteRanges.back().end() );
}

inline ByteRange ByteRanges::front() const
{
    return empty() ? ByteRange() : mByteRanges.front();
}

inline ByteRange ByteRanges::back() const
{
    return empty() ? ByteRange() : mByteRanges.back();
}


inline int32_t ByteRanges::size() const
{
    int32_t result = 0;
    const_iterator it = begin();
    for( ; it != end(); ++it )
    {
        if ( it->size() < 0 )
            return it->size();

        result += it->size();
    }

    return result;
}

inline void ByteRanges::clear()
{
    mByteRanges.clear();
}

inline ByteRanges ByteRanges::operator|( ByteRange const& rhs ) const
{
    ByteRanges result( *this );
    result |= rhs;
    return result;
}



inline ByteRanges ByteRanges::operator|( ByteRanges const& rhs ) const
{
    ByteRanges result( *this );
    result |= rhs;
    return result;
}

inline ByteRanges& ByteRanges::operator|=( ByteRanges const& rhs )
{
    const_iterator it = rhs.begin();
    while( it != rhs.end() ) *this |= *it++;
    return *this;
}

inline ByteRanges ByteRanges::operator-( ByteRange const& rhs ) const
{
    ByteRanges result( *this );
    result -= rhs;
    return result;
}

inline ByteRanges ByteRanges::operator-( ByteRanges const& rhs ) const
{
    ByteRanges result( *this );
    result -= rhs;
    return result;
}

inline ByteRanges& ByteRanges::operator-=( ByteRanges const& rhs )
{
    const_iterator it = rhs.begin();
    while( it != rhs.end() ) *this -= *it++;
    return *this;
}

inline ByteRanges ByteRanges::operator&( ByteRange const& rhs ) const
{
    ByteRanges ranges( *this );
    return ranges &= rhs;
}

inline ByteRanges ByteRanges::operator&( ByteRanges const& rhs ) const
{
    ByteRanges ranges( *this );
    return ranges &= rhs;
}



}} // namespace netflix::ase


#endif /* BYTE_RANGES_H */
