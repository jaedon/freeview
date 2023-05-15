/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef AGGREGATE_THROUGHPUT_DISTRIBUTION
#define AGGREGATE_THROUGHPUT_DISTRIBUTION

#include "IThroughputDistribution.h"

#include <nrdnet/AseTimeVal.h>

namespace netflix {
namespace ase {

template<class _Iterator>
class AggregateThroughputDistribution : public IThroughputDistribution
{
public:
    typedef shared_ptr<AggregateThroughputDistribution> AggregateThroughputDistributionPtr;

    AggregateThroughputDistribution( _Iterator begin, _Iterator end )
        : mBegin( begin ), mEnd( end )
    {}

    virtual ~AggregateThroughputDistribution() {}

    /** Pdf evaluation operation
     * @param throughput [in] A throughput value
     * @result the probability density at the provided throughput
     */
    virtual double pdf( int throughput ) const;

    /** Cdf evaluation operator
     * @param throughput [in] A throughput value
     * @rusult the probability that throughput is less than or equal to the provided value
     */
    virtual double cdf( int throughput ) const;

    /** Get the time weighted effective number of measurements
     * @result The time-weighted effective number of measurements
     */
    virtual double effectiveCount( AseTimeVal const& now ) const;

    /** Evalute the inverse cdf */
    virtual int invcdf( double quantile ) const;

    /** Find the modes of the distribution */
    virtual std::vector<int> modes() const;

    /** Find the highest mode of the distribution less than a maximum */
    virtual int highestmode( int max = -1 ) const;

    /** Get the support of the distribution */
    virtual std::pair<int,int> support() const;

protected:
    _Iterator const   mBegin;
    _Iterator const   mEnd;
};

template<class _Iterator>
double AggregateThroughputDistribution<_Iterator>::pdf( int throughput ) const
{
    double p = 0;
    AseTimeVal const now = AseTimeVal::now();
    double totalCount = 0.0;
    for( _Iterator it = mBegin; it != mEnd; ++it )
    {
        double const effectiveCount = (*it)->effectiveCount( now );
        p += (*it)->pdf( throughput ) * effectiveCount;
        totalCount += effectiveCount;
    }

    return ( totalCount > 0.0 ) ? p / totalCount : 0.0;
}

template<class _Iterator>
double AggregateThroughputDistribution<_Iterator>::cdf( int throughput ) const
{
    double p = 0;
    AseTimeVal const now = AseTimeVal::now();
    double totalCount = 0.0;
    for( _Iterator it = mBegin; it != mEnd; ++it )
    {
        double const effectiveCount = (*it)->effectiveCount( now );
        p += (*it)->cdf( throughput ) * effectiveCount;
        totalCount += effectiveCount;
    }

    return ( totalCount > 0.0 ) ? p / totalCount : 0.0;
}

template<class _Iterator>
double AggregateThroughputDistribution<_Iterator>::effectiveCount( AseTimeVal const& now ) const
{
    double totalCount = 0.0;
    for( _Iterator it = mBegin; it != mEnd; ++it )
    {
        totalCount += (*it)->effectiveCount( now );
    }
    return totalCount;
}

template<class _Iterator>
int AggregateThroughputDistribution<_Iterator>::invcdf( double quantile ) const
{
    // Do a binary search for the requested quantile
    std::pair<int,int> const supp = support( );
    int min = supp.first;
    int max = supp.second;

    while( ( max - min ) > 50 )
    {
        int const mid = ( max + min ) / 2;
        if ( cdf( mid ) > quantile ) max = mid;
        else min = mid;
    }

    int const result = ( max + min ) / 2;

    return result < 0 ? 0 : result;
}

template<class _Iterator>
std::vector<int> AggregateThroughputDistribution<_Iterator>::modes( ) const
{
    // Not currently supported
    return std::vector<int>();
}

template<class _Iterator>
int AggregateThroughputDistribution<_Iterator>::highestmode( int /* max */ ) const
{
    // Not currently supported
    return 0;
}

template<class _Iterator>
std::pair<int,int> AggregateThroughputDistribution<_Iterator>::support( ) const
{
    int min = 0;
    int max = 0;

    _Iterator it = mBegin;
    for( bool bFirst = true; it != mEnd; ++it )
    {
        IThroughputDistributionPtr const pDistribution = *it;
        if ( pDistribution )
        {
            std::pair<int,int> const support = (*it)->support();
            min = ( bFirst || support.first < min ) ? support.first : min;
            max = ( bFirst || support.second > max ) ? support.second : max;
            bFirst = false;
        }
    }

    return std::make_pair( min, max );
}


} // namespace ase
} // namespace netflix

#endif /* AGGREGATE_THROUGHPUT_DISTRIBUTION */
