/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef BYTE_RANGE_H
#define BYTE_RANGE_H

/** @file ByteRange.h */

#include <nrdbase/StdInt.h>
#include <utility>  // for std::pair
#include <string>
#include <assert.h>

namespace netflix {
namespace ase
{
/**
* @class ByteRange
* @brief A class representing a byte range as found in HTTP
*/
class ByteRange
{
public:
    /** Create an empty byte range */
    ByteRange();

    /** Create a single byte range [start-end]
     *  @param start [in] The index of the first byte (starting from 0)
     *  @param end [in] The index of the last byte (starting from 0)
     *                  end = -1 indicates open-ended
     */
    ByteRange( llong start, llong end = -1 );

    /** True if the byte range is empty */
    bool empty() const;

    /** True if the byte range is valid & non-empty */
    bool valid() const;

    /** True if the byte range is closed */
    bool closed() const;

    /** True if the byte range is open() */
    bool open() const;

    /** Get the start index */
    llong getStart() const;
    llong start() const { return getStart(); }    // synonym

    /** Set the start index */
    void setStart( llong start );

    /** Get the end index */
    llong getEnd() const;
    llong end() const { return getEnd(); }        // synonym

     /** Set the end index */
    void setEnd( llong start );

    /** Get the size */
    llong getSize() const;
    llong size() const { return getSize(); }      // synonym

    /** Equal operator */
    bool operator==( ByteRange const& rhs );

    /** Reset the byte ragne */
    void reset();

    /** Check if this byte range is contiguous with the next
     *  (this->mEndIndex + 1) == next.mStartIndex
     */
    bool contiguousWith( ByteRange const& next ) const;

    /** Check if this byte range is contained in another */
    bool containedIn( ByteRange const& other ) const;
    bool containedBy( ByteRange const& rhs ) const { return containedIn( rhs ); } // synonym

    /** Check if this byte range intersects another */
    bool overlaps( ByteRange const& other ) const;

    /** Check if this byte range contains another */
    bool contains( ByteRange const& other ) const;

    /** Excise one byte range from another */
    std::pair<ByteRange,ByteRange> excise( ByteRange const& other ) const;

    /** Join two bytes ranges */
    ByteRange& operator+=( ByteRange const& other );

    /** Get the intersection with anothe byte range */
    ByteRange intersection( ByteRange const& other );

    /** Get this byte range relative to another (containing) one */
    ByteRange relativeTo( ByteRange const& other ) const;

    // Byte Ranges are ordered by starting value
    bool operator<( ByteRange const& rhs ) const;
    bool operator<=( ByteRange const& rhs ) const;
    bool operator>( ByteRange const& rhs ) const;
    bool operator>=( ByteRange const& rhs ) const;

    bool adjacent( ByteRange const& rhs ) const;
    bool disjoint( ByteRange const& rhs ) const;

    // union (must not be disjoint)
    ByteRange operator|( ByteRange const& rhs ) const;
    ByteRange& operator|=( ByteRange const& rhs );

    // intersection
    ByteRange operator&( ByteRange const& rhs ) const;
    ByteRange& operator&=( ByteRange const& rhs );

    // Returns a prefix of this range of the provided size
    ByteRange prefix( llong const size ) const;

    /** Convert to HTTP byte-range-spec */
    std::string toByteRangeSpec() const;

protected:
    llong   mStartIndex;
    llong   mEndIndex;
};

inline ByteRange::ByteRange()
    : mStartIndex( -1 )
    , mEndIndex( -2 )
{
}

inline ByteRange::ByteRange( llong start, llong end )
    : mStartIndex( start )
    , mEndIndex( end )
{
}

inline bool ByteRange::empty() const
{
    return mStartIndex < 0;
}

inline llong ByteRange::getStart() const
{
    return mStartIndex;
}

inline void ByteRange::setStart( llong start )
{
    mStartIndex = start;
}

inline llong ByteRange::getEnd() const
{
    return mEndIndex;
}

inline void ByteRange::setEnd( llong end )
{
    mEndIndex = end;
}

inline bool ByteRange::valid() const
{
    return !empty() && ( mEndIndex < 0 || mEndIndex >= mStartIndex );
}

inline bool ByteRange::closed() const
{
    return valid() && mEndIndex >= 0;
}

inline bool ByteRange::open() const
{
    return valid() && mEndIndex < 0;
}

inline llong ByteRange::getSize() const
{
    return mStartIndex < 0 ? - 2
                    : mEndIndex < 0 ? -1
                    : mEndIndex - mStartIndex + 1;
}

inline bool ByteRange::operator==( ByteRange const& rhs )
{
    return (mStartIndex == rhs.mStartIndex) && (mEndIndex == rhs.mEndIndex);
}

inline void ByteRange::reset()
{
    mStartIndex = -1;
    mEndIndex = -2;
}

inline bool ByteRange::contiguousWith( ByteRange const& next ) const
{
    return (this->valid() && next.valid()
        && this->closed()
        && next.valid()
        && ((this->mEndIndex + 1) == next.mStartIndex));
}

inline bool ByteRange::containedIn( ByteRange const& other ) const
{
    return ( this->valid() && other.valid()
                && ( this->getStart() >= other.getStart() )
                && ( other.open()
                    || ( this->closed() && ( this->getEnd() <= other.getEnd() ) ) ) );
}

inline bool ByteRange::overlaps( ByteRange const& other ) const
{
    return ( this->valid() && other.valid()
                && (   ( this->closed() && other.closed() &&
                               ( this->start() > other.start() ? this->start() : other.start() )
                            <= ( this->end() < other.end() ? this->end() : other.end() ) )
                    || ( this->open() && other.open() )
                    || ( this->open() && other.end() >= this->start() )
                    || ( other.open() && this->end() >= other.start() ) ) );
}

inline bool ByteRange::contains( ByteRange const& other ) const
{
    return other.containedIn( *this );
}

inline ByteRange ByteRange::intersection( ByteRange const& other )
{
    if ( !overlaps( other ) )
        return ByteRange();

    return ByteRange(   start() > other.start() ? start() : other.start(),
                        open() && other.open() ? -1
                            : open() ? other.end()
                            : other.open() ? end()
                            : end() < other.end() ? end() : other.end() );
}

inline ByteRange& ByteRange::operator+=( ByteRange const& other )
{
    setStart( ( start() < other.start() ) ? start() : other.start() );
    if ( closed() )
        setEnd( ( other.closed() && end() > other.end() ) ? end() : other.end() );
    return *this;
}

inline ByteRange ByteRange::relativeTo( ByteRange const& other ) const
{
    if (!other.contains( *this ) )
        return ByteRange();

    return ByteRange( start() - other.start(), open() ? -1 : end() - other.start() );
}

// Byte Ranges are ordered by starting value
inline bool ByteRange::operator<( ByteRange const& rhs ) const
{
    return empty() || ( !rhs.empty() && start() < rhs.start() );
}

inline bool ByteRange::operator<=( ByteRange const& rhs ) const
{
    return empty() || ( !rhs.empty() && start() <= rhs.start() );
}

inline bool ByteRange::operator>( ByteRange const& rhs ) const
{
    return rhs < *this;
}

inline bool ByteRange::operator>=( ByteRange const& rhs ) const
{
    return rhs <= *this;
}

inline bool ByteRange::adjacent( ByteRange const& rhs ) const
{
    return !empty() && !rhs.empty() &&
            ( ( closed() && end() + 1 == rhs.start() )
                || ( rhs.closed() && rhs.end() + 1 == start() ) );
}

inline bool ByteRange::disjoint( ByteRange const& rhs ) const
{
    return empty() || rhs.empty()
            || ( rhs.closed() && start() > rhs.end() )
            || ( closed() && end() < rhs.start() );
}

// union (must not be disjoint)
inline ByteRange ByteRange::operator|( ByteRange const& rhs ) const
{
    ByteRange range( *this );
    return range |= rhs;
}

inline ByteRange& ByteRange::operator|=( ByteRange const& rhs )
{
    assert( open() || rhs.start() <= end() + 1 );
    assert( rhs.open() || rhs.end() + 1 >= start() );
    setStart( start() < rhs.start() ? start() : rhs.start() );
    setEnd( open() || rhs.open() ? -1 : ( end() > rhs.end() ? end() : rhs.end() ) );
    return *this;
}

// intersection
inline ByteRange ByteRange::operator&( ByteRange const& rhs ) const
{
    ByteRange range( *this );
    return range &= rhs;
}

inline ByteRange& ByteRange::operator&=( ByteRange const& rhs )
{
    if ( !disjoint( rhs ) )
    {
        setStart( start() > rhs.start() ? start() : rhs.start() );
        setEnd( ( open() && rhs.open() ? -1 :
                    ( open() ? rhs.end() :
                        rhs.open() ? end() :
                        end() < rhs.end() ? end() : rhs.end() ) ) );
    }
    else
    {
        reset();
    }
    return *this;
}

inline ByteRange ByteRange::prefix( llong const prefixsize ) const
{
    assert( prefixsize <= size() );
    return ByteRange( start(), start() + prefixsize - 1 );
}

}} /* end namespace ase */

#endif
