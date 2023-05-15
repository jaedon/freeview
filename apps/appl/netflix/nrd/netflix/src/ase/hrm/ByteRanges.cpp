/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ByteRanges.h"

using namespace netflix;
using namespace ase;
using namespace std;

bool ByteRanges::disjoint( ByteRange const& range ) const
{
    if ( empty() || range.empty()
            || ( range.closed() && range.end() < front().start() )
            || ( back().closed() && range.start() > back().end() ) )
        return true;

    if ( contiguous() )
        return false;

    const_iterator it = begin();
    for( ; it != end(); ++it )
    {
        if ( !it->disjoint( range ) )
            return false;
    }
    return true;
}

bool ByteRanges::disjoint( ByteRanges const& ranges ) const
{
    if ( empty() || ranges.empty()
            || ( ranges.back().closed() && ranges.back().end() < front().start() )
            || ( back().closed() && back().end() < ranges.front().start() ) )
        return true;

    if ( contiguous() )
        return false;

    const_iterator it = begin();
    for( ; it != end(); ++it )
    {
        if ( !ranges.disjoint( *it ) )
            return false;
    }
    return true;
}

bool ByteRanges::contains( ByteRange const& range ) const
{
    ByteRangesVector::const_iterator it = mByteRanges.begin();
    for( ; it != mByteRanges.end(); ++it )
    {
        if ( it->contains( range ) )
            return true;
    }
    return false;
}

bool ByteRanges::contains( ByteRanges const& ranges ) const
{
    ByteRangesVector::const_iterator it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
        if ( !contains( *it ) )
            return false;
    }
    return true;
}

ByteRanges& ByteRanges::operator|=( ByteRange const& rhs )
{
    if ( !rhs.empty() )
    {
        if ( empty() )
        {
            mByteRanges.push_back( rhs );
        }
        else
        {
            ByteRangesVector::iterator it = mByteRanges.begin();
            ByteRangesVector::iterator pit;
            while ( it != mByteRanges.end() && *it < rhs) pit = it++;
            if ( it == mByteRanges.begin() || ( pit->closed() && pit->end() < rhs.start() - 1 ) )
            {
                pit = mByteRanges.insert( it, rhs );
                it = pit + 1;
            }
            else
            {
                *pit |= rhs;
            }

            if ( pit->open() )
            {
                mByteRanges.erase( it, mByteRanges.end() );
            }
            else
            {
                while( it != end() && ( pit->open() || pit->end() >= it->start() - 1 ) )
                {
                    (*pit) |= (*it);
                    it = mByteRanges.erase( it );
                }
            }
        }
    }

    return *this;
}

ByteRanges& ByteRanges::append( ByteRange const& rhs )
{
    assert( rhs.closed() && !rhs.empty() );

    if ( empty() )
    {
        mByteRanges.push_back( rhs );
        return *this;
    }
    else
    {
        ByteRangesVector::iterator it = mByteRanges.begin();
        for( ; it != mByteRanges.end(); ++it )
        {
            if ( it->end() == rhs.start() - 1 )
            {
                it->setEnd( rhs.end() );

                ByteRangesVector::iterator pit = it++;

                while ( it != mByteRanges.end() && pit->end() >= it->start() - 1 )
                {
                    pit->setEnd( it->end() );
                    it = mByteRanges.erase( it );
                }

                return *this;
            }
        }
    }

    return ( *this |= rhs );
}

ByteRanges& ByteRanges::operator-=( ByteRange const& rhs )
{
    ByteRangesVector::iterator it = mByteRanges.begin();
    while( it != mByteRanges.end() )
    {
        if ( it->overlaps( rhs ) )
        {
            ByteRanges result = *it - rhs;
            if ( result.contiguous() )
                *it++ = result.span();
            else if ( result.empty() )
                it = mByteRanges.erase( it );
            else
            {
                *it = result.front();
                it = mByteRanges.insert( ++it, result.back() );
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

ByteRanges& ByteRanges::operator&=( ByteRange const& rhs )
{
    ByteRangesVector::iterator it = mByteRanges.begin();
    while ( it != mByteRanges.end() )
    {
        *it &= rhs;
        if ( it->empty() )
        {
            it = mByteRanges.erase( it );
        }
        else
        {
            ++it;
        }
    }
    return *this;
}

namespace netflix {
namespace ase {

ByteRanges operator-( ByteRange const& lhs, ByteRange const& rhs )
{
    if ( lhs.disjoint( rhs ) )
        return ByteRanges( lhs );

    if ( rhs.contains( lhs ) )
        return ByteRanges();

    ByteRanges result;
    if ( rhs.start() > lhs.start() )
        result.append( ByteRange( lhs.start(), rhs.start() - 1 ) );
    if ( rhs.closed() && ( lhs.open() || rhs.end() < lhs.end() ) )
        result.append( ByteRange( rhs.end() + 1, lhs.end() ) );

    return result;
}

ByteRanges operator-( ByteRange const& lhs, ByteRanges const& rhs )
{
    ByteRanges result( lhs );
    ByteRanges::const_iterator it = rhs.begin();
    for( ; it != rhs.end(); ++it )
    {
        if ( result.empty() || ( result.back().closed() && it->start() > result.back().end() ) )
            break;

        result -= *it;
    }
    return result;
}

}} //namespace netflix::ase

