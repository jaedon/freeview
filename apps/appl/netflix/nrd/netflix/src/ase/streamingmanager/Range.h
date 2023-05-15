/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef RANGE_H
#define RANGE_H

#include <vector>

namespace netflix {
namespace ase
{

template<typename T>
class Ranges;

/** Class that represents a value range [ start, end ) */
template<typename T>
class Range
{
public:
    /** Construct empty range */
    Range();

    Range( const T& start, const T& end );

    bool empty() const;

    T start() const;
    T end() const;
    T size() const;

    bool operator==( const Range& rhs ) const;

    // Ranges are ordered by starting value
    bool operator<( const Range<T>& rhs ) const;
    bool operator<=( const Range<T>& rhs ) const;
    bool operator>( const Range<T>& rhs ) const;
    bool operator>=( const Range<T>& rhs ) const;

    bool contains( const Range<T>& rhs ) const;
    bool containedBy( const Range<T>& rhs ) const;
    bool overlaps( const Range<T>& rhs ) const;
    bool adjacent( const Range<T>& rhs ) const;
    bool disjoint( const Range<T>& rhs ) const;

    // union (must not be disjoint)
    Range<T> operator|( const Range<T>& rhs ) const;
    Range<T>& operator|=( const Range<T>& rhs );

    // intersection
    Range<T> operator&( const Range<T>& rhs ) const;
    Range<T>& operator&=( const Range<T>& rhs );

    // excision
    //Range<T> operator-( const Range<T>& rhs ) const;
    //Range<T>& operator-=( const Range<T>& rhs );

protected:
    bool    mEmpty;
    T       mStart;
    T       mEnd;
};

/** Class that represents a set of value ranges */
template<typename T>
class Ranges
{
public:
    typedef std::vector<Range<T> >                  RangesVector;
    typedef typename RangesVector::iterator         iterator;
    typedef typename RangesVector::const_iterator   const_iterator;

    // Empty range
    Ranges();

    // Ranges with single initial range
    Ranges( const Range<T>& range );

    bool empty() const;

    bool contiguous() const;

    Range<T> span() const;

    Range<T> front() const;

    Range<T> back() const;

    // union
    Ranges<T> operator|( const Range<T>& rhs ) const;
    Ranges<T>& operator|=( const Range<T>& rhs );
    Ranges<T> operator|( const Ranges<T>& rhs ) const;
    Ranges<T>& operator|=( const Ranges<T>& rhs );

    // excision
    Ranges<T> operator-( const Range<T>& rhs ) const;
    Ranges<T>& operator-=( const Range<T>& rhs );
    Ranges<T> operator-( const Ranges<T>& rhs ) const;
    Ranges<T>& operator-=( const Ranges<T>& rhs );

    // iterators
    iterator begin()                { return mRanges.begin(); }
    iterator end()                  { return mRanges.end(); }
    const_iterator begin() const    { return mRanges.begin(); }
    const_iterator end() const      { return mRanges.end(); }

protected:
    // Sorted dis-contiguous non-empty ranges
    RangesVector mRanges;
};

template<typename T>
Ranges<T> operator-( Range<T> const& lhs, Range<T> const& rhs );

template<typename T>
inline Range<T>::Range()
    : mEmpty( true )
{
}

template<typename T>
inline Range<T>::Range( const T& start, const T& end )
    : mEmpty( false )
    , mStart( start )
    , mEnd( end )
{
    assert( start <= end );
}

template<typename T>
inline bool Range<T>::empty() const
{
    return mEmpty;
}

template<typename T>
inline T Range<T>::start() const
{
    return mStart;
}

template<typename T>
inline T Range<T>::end() const
{
    return mEnd;
}

template<typename T>
inline T Range<T>::size() const
{
    return mEmpty ? (T) 0 : ( mEnd - mStart );
}

template<typename T> inline bool Range<T>::operator==( const Range<T>& rhs ) const
{
    if ( mEmpty )
        return rhs.mEmpty;

    return !rhs.mEmpty && mStart == rhs.mStart && mEnd == rhs.mEnd;
}

template<typename T> inline bool Range<T>::operator<( const Range<T>& rhs ) const
{
    return mStart < rhs.mStart;
}

template<typename T> inline bool Range<T>::operator<=( const Range<T>& rhs ) const
{
    return mStart <= rhs.mStart;
}

template<typename T> inline bool Range<T>::operator>( const Range<T>& rhs ) const
{
    return mStart > rhs.mStart;
}

template<typename T> inline bool Range<T>::operator>=( const Range<T>& rhs ) const
{
    return mStart >= rhs.mStart;
}

template<typename T> inline bool Range<T>::contains( const Range<T>& rhs ) const
{
    return !empty() && !rhs.empty() && ( mStart <= rhs.mStart && mEnd >= rhs.mEnd );
}


template<typename T> inline bool Range<T>::containedBy( const Range<T>& rhs ) const
{
    return rhs.contains( *this );
}

template<typename T> bool Range<T>::overlaps( const Range<T>& rhs ) const
{
    return !empty() && !rhs.empty()
            && ! ( ( mStart >= rhs.mEnd ) || ( mEnd <= rhs.mStart ) );
}

template<typename T> bool Range<T>::adjacent( const Range<T>& rhs ) const
{
    return !empty() && !rhs.empty()
        && ( ( mStart == rhs.mEnd ) || ( mEnd == rhs.mStart ) );
}

template<typename T> bool Range<T>::disjoint( const Range<T>& rhs ) const
{
    return empty() || rhs.empty()
            || ( mStart > rhs.mEnd )
            || ( mEnd < rhs.mStart );
}

template<typename T> Range<T> Range<T>::operator&( const Range<T>& rhs ) const
{
    Range<T> result( *this );
    result &= rhs;
    return result;
}

template<typename T> Range<T>& Range<T>::operator&=( const Range<T>& rhs )
{
    if ( !overlaps( rhs ) )
    {
        mEmpty = true;
    }
    else
    {
        mStart = std::max( mStart, rhs.mStart );
        mEnd = std::min( mEnd, rhs.mEnd );
    }

    return *this;
}
/*
template<typename T> Range<T> Range<T>::operator-( const Range<T>& rhs ) const
{
    Range<T> result( *this );
    result -= rhs;
    return result;
}

template<typename T> Range<T>& Range<T>::operator-=( const Range<T>& rhs )
{
    if ( containedBy( rhs ) )
    {
        mEmpty = true;
    }
    else if ( overlaps( rhs ) )
    {
        if ( rhs < *this )
            mStart = rhs.mEnd;
        else
        {
            mEnd = rhs.mStart;
        }
    }

    return *this;
}
*/

template<typename T> Range<T> Range<T>::operator|( const Range<T>& rhs ) const
{
    Range<T> result( *this );
    result |= rhs;
    return result;
}

template<typename T> Range<T>& Range<T>::operator|=( const Range<T>& rhs )
{
    assert( !disjoint( rhs ) );
    mStart = std::min( mStart, rhs.mStart );
    mEnd = std::max( mEnd, rhs.mEnd );
    return *this;
}

// Ranges

template<typename T> inline Ranges<T>::Ranges()
{
}

template<typename T> inline Ranges<T>::Ranges( const Range<T>& range )
{
    mRanges.push_back( range );
}

template<typename T> inline bool Ranges<T>::empty() const
{
    return mRanges.empty();
}

template<typename T> inline bool Ranges<T>::contiguous() const
{
    return ( mRanges.size() == 1 );
}

template<typename T> Range<T> Ranges<T>::span() const
{
    return empty() ? Range<T>() : Range<T>( mRanges.front().start(), mRanges.back().end() );
}

template<typename T> Range<T> Ranges<T>::front() const
{
    return empty() ? Range<T>() : mRanges.front();
}

template<typename T> Range<T> Ranges<T>::back() const
{
    return empty() ? Range<T>() : mRanges.back();
}

template<typename T> Ranges<T> Ranges<T>::operator|( const Range<T>& rhs ) const
{
    Ranges result( *this );
    result |= rhs;
    return result;
}

template<typename T> Ranges<T>& Ranges<T>::operator|=( const Range<T>& rhs )
{
    typename RangesVector::iterator it = mRanges.begin();
    while( it != mRanges.end() && *it < rhs ) ++it;
    if ( it == mRanges.begin() || (it-1)->disjoint( rhs ) )
    {
        it = mRanges.insert( it, rhs );
    }
    else
    {
        *(--it) |= rhs;
    }

    typename RangesVector::iterator itt = it;
    ++itt;
    while( itt != mRanges.end() && ( !it->disjoint( *itt ) ) )
    {
        (*it) |= (*itt);
        itt = mRanges.erase( itt );
    }
    return *this;
}

template<typename T> Ranges<T> Ranges<T>::operator|( const Ranges<T>& rhs ) const
{
    Ranges result( *this );
    result |= rhs;
    return result;
}

template<typename T> Ranges<T>& Ranges<T>::operator|=( const Ranges<T>& rhs )
{
    typename RangesVector::iterator it = rhs.begin();
    while( it != rhs.end() ) *this |= *it++;
    return *this;
}

template<typename T> Ranges<T> Ranges<T>::operator-( const Range<T>& rhs ) const
{
    Ranges result( *this );
    result -= rhs;
    return result;
}

template<typename T> Ranges<T>& Ranges<T>::operator-=( const Range<T>& rhs )
{
    typename RangesVector::iterator it = mRanges.begin();
    while( it != mRanges.end() )
    {
        if ( it->overlaps( rhs ) )
        {
            Ranges result = *it - rhs;
            if ( result.contiguous() )
                *it++ = result.span();
            else if ( result.empty() )
                it = mRanges.erase( it );
            else
            {
                *it = result.mRanges[0];
                it = mRanges.insert( ++it, result.mRanges[1] );
                ++it;
            }
        }
        else
        {
            ++it;
        }

    }

    return *this;
}

template<typename T> Ranges<T> Ranges<T>::operator-( const Ranges<T>& rhs ) const
{
    Ranges result( *this );
    result -= rhs;
    return result;
}

template<typename T> Ranges<T>& Ranges<T>::operator-=( const Ranges<T>& rhs )
{
    typename RangesVector::const_iterator it = rhs.mRanges.begin();
    while( it != rhs.mRanges.end() ) *this -= *it++;
    return *this;
}

template<typename T>
Ranges<T> operator-( Range<T> const& lhs, Range<T> const& rhs )
{
    if ( lhs.disjoint( rhs ) )
        return Ranges<T>( lhs );

    if ( rhs.contains( lhs ) )
        return Ranges<T>();

    Ranges<T> result;
    if ( rhs.start() > lhs.start() )
        result |= Range<T>( lhs.start(), rhs.start() );
    if ( rhs.end() < lhs.end() )
        result |= Range<T>( rhs.end(), lhs.end() );

    return result;
}



}} // namespace ase


#endif /* RANGE_H */
