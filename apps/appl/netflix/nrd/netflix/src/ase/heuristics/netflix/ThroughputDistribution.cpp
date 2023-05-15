/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/tr1.h>
#include <queue>
#include <limits>
#include "ThroughputDistribution.h"

#include <iostream>

#include <nrdbase/Time.h>
#include <nrdnet/AseTimeVal.h>

using namespace std;
using namespace netflix;
using namespace netflix::ase;

static llong variantAsInt( Variant const& v )
{
    return v.isInteger() ? v.integer() : ((llong) v.dbl() );
}

double ThroughputDistribution::effectiveCountForActiveTime( int effectiveCountHalflife,
                                                            int distributionUpdateInterval,
                                                            int maxConfidenceTime )
{
    double const effectiveCountAlpha( -log( 0.5 ) / effectiveCountHalflife );
    double const activeCountAlpha = exp( - effectiveCountAlpha * distributionUpdateInterval );
    int const maxConfidenceMeasurements = maxConfidenceTime / distributionUpdateInterval;
    return ( 1 - pow( activeCountAlpha, maxConfidenceMeasurements ) ) / ( 1 - activeCountAlpha );
}


ThroughputDistribution::Cluster::Cluster( int mean, int count, double weight )
    : mMean( mean )
    , mCount( count )
    , mWeight( weight )
    , mMergecost( 0.0 )
    , mInCostTree( false )
{
}

ThroughputDistribution::Cluster::Cluster( Cluster const& other )
    : mMean( other.mMean )
    , mCount( other.mCount )
    , mWeight( other.mWeight )
    , mMergecost( other.mMergecost )
    , mInCostTree( false )
{
}

int ThroughputDistribution::Cluster::mergepoint( Cluster const& other, double bandwidth )
{
    if ( mMean == other.mMean )
    {
        return mMean;
    }
    else
    {
        int const k = static_cast<int> ( ( other.mMean - mMean ) / bandwidth );
        double const w1 = mWeight * mCount;
        double const w2 = other.mWeight * other.mCount;
        if ( k > 3 )
        {
            return w1 > w2 ? mMean : other.mMean;
        }
        else
        {
            return static_cast<int> ( ( mMean * w1 + other.mMean * w2 ) / ( w1 + w2 ) );
        }
    }
}

void ThroughputDistribution::Cluster::merge( int mean, Cluster const& other )
{
    mMean = mean;
    int const count = mCount + other.mCount;
    mWeight = ( mCount * mWeight + other.mCount * other.mWeight ) / count;
    mCount = count;
}

void ThroughputDistribution::Cluster::updatemergecost( Cluster const& other, double bandwidth )
{
    int const x = mergepoint( other, bandwidth );
    int const xi = mMean;
    int const xj = other.mMean;
    int const h = (int) bandwidth;
    double const wi = mWeight * mCount;
    double const wj = other.mWeight * other.mCount;
    double const w = ( wi + wj );

    // The cost function is based on a square kernal for ease of computation
    // This could be replaced with something closer to the real kernal, for example
    // a triangular one. The real kernal does have a closed form solution for this cost
    // function but it is fairly complex and did not show obvious improvements in
    // simulations.

    // The partition below only has 3 or 4 forms based on the distance between clusters
    // so we could change this for a simnpler closed form version for each case

    // xi < x < xj
    // k = (int) ( xj - xi ) / h
    // k = 0,1            => xi-h < xj-h < xi+h < xj+h
    //                       xi-h < x-h < xj-h < xi+h < x+h < xj+h
    // k >= 2             => xi-h < xi+h < xj-h < xj+h
    // k1 = (int) ( x - xi ) / h
    // k2 = (int) ( xj - x ) / h
    //
    // k1 = 0, 1          =>

    std::priority_queue<int> partition;
    partition.push( xi - h );
    partition.push( xi + h );
    partition.push( xj - h );
    partition.push( xj + h );
    partition.push( x - h );
    partition.push( x + h );

    double loss = 0.0;
    int z = partition.top();
    partition.pop();

    while( !partition.empty() )
    {
        int y = partition.top();
        partition.pop();

        if ( y == z ) continue;

        double diff = 0.0;
        if ( y >= xi - h && y < xi + h ) diff += wi;
        if ( y >= xj - h && y < xj + h ) diff += wj;
        if ( y >= x - h && y < x + h ) diff -= w;
        loss += ( diff * diff ) * ( z - y );
        z = y;
    }

    mMergecost = loss;
}

double ThroughputDistribution::Cluster::kernel( double x )
{
    return ( x > -1.0 && x < 1.0 ) ? ( 0.75 * ( 1 - x*x ) ) : 0.0;
}

// cdf(z) = integral[-1,z]( 0.75 * ( 1 - x^^2 ) )
//        = 0.75 * [ x - x^^3 / 3 ][-1,z]
//        = 0.75 * ( ( z - z^^3 / 3 ) - ( -1 - -1 / 3 ) )
//        = 0.75 * ( ( z - z^^3 / 3 ) + 2/3 )
//        = 0.25 * ( 3z - z^^3 + 2 )

double ThroughputDistribution::Cluster::kernel_cdf( double x )
{
    return ( x <= -1.0 ) ? 0.0 : ( x >= 1.0 ) ? 1.0 : ( 2.0 + x * ( 3.0 - x * x ) ) / 4.0;
}

double ThroughputDistribution::Cluster::pdf( int x, double bandwidth ) const
{
    return mWeight * mCount * kernel( ( x - mMean ) / bandwidth ) / bandwidth;
}

double ThroughputDistribution::Cluster::cdf( int x, double bandwidth ) const
{
    return mWeight * mCount * kernel_cdf( ( x - mMean ) / bandwidth );
}

ThroughputDistribution::Params::Params()
    : mClusters( 20 )
    , mHalflife( 10 * 60 * 60 * 1000 / 2000 )
    , mWarmup( 3600 * 1000 / 2000 )
    , mEffectiveCountAlpha( -log( 0.5 ) / ( 10 * 3600 * 1000 ) )
{
}

void ThroughputDistribution::Params::initialize( IAseConfigParameterPtr pConfigParameters )
{
    int const intmax = std::numeric_limits<int32_t>::max();

    int const distributionUpdateInterval = pConfigParameters->getParameterAs<int32_t>( "distributionUpdateInterval", 100, intmax, 2000 );

    mClusters = pConfigParameters->getParameterAs<int32_t>( "distributionClusters", 2, intmax, 20 );
    mHalflife = pConfigParameters->getParameterAs<int32_t>( "distributionHalflife", 1, intmax, 10 * 60 * 60 ) * 1000 / distributionUpdateInterval;
    mWarmup = pConfigParameters->getParameterAs<int32_t>( "distributionWarmup", 0, intmax, 3600 ) * 1000 / distributionUpdateInterval;

    int effectiveCountHalflife = pConfigParameters->getParameterAs<int32_t>( "measurementHalflife", 1, intmax, 10 * 3600 ) * 1000;

    mEffectiveCountAlpha = -log( 0.5 ) / effectiveCountHalflife;
}

ThroughputDistribution::ThroughputDistribution( ThroughputDistribution::Params const& params )
    : mMaxClusters( params.mClusters )
    , mAlpha( 1.0 - exp( log( 0.5 ) / params.mHalflife ) )
    , mOneMinusAlpha( 1.0 - mAlpha )
    , mWarmup( params.mWarmup )
    , mEffectiveCountAlpha( params.mEffectiveCountAlpha )
    , mCount( 0 )
    , mWeightedCount( 0 )
    , mEffectiveCount( 0 )
    , mEffectiveCountTime( AseTimeVal::ZERO )
    , mAverage( 0.0 )
    , mAverageSquare( 0.0 )
    , mBandwidth( 1000.0 )
{
}

ThroughputDistribution::ThroughputDistribution( ThroughputDistribution const& other )
    : IThroughputDistribution(other)
    , mClustersByMean( other.mClustersByMean )
    , mMaxClusters( other.mMaxClusters )
    , mAlpha( other.mAlpha )
    , mOneMinusAlpha( other.mOneMinusAlpha )
    , mWarmup( other.mWarmup )
    , mEffectiveCountAlpha( other.mEffectiveCountAlpha )
    , mCount( other.mCount )
    , mWeightedCount( other.mWeightedCount )
    , mEffectiveCount( other.mEffectiveCount )
    , mEffectiveCountTime( other.mEffectiveCountTime )
    , mAverage( other.mAverage )
    , mAverageSquare( other.mAverageSquare )
    , mBandwidth( other.mBandwidth )
{
    if ( mClustersByMean.size() > 1 )
    {
        ClustersByMean::iterator it = mClustersByMean.begin();
        ClustersByMean::iterator const end = --mClustersByMean.end();
        for( ; it != end ; ++it )
        {
            insertincosttree( it );
        }
    }
}

ThroughputDistribution::~ThroughputDistribution()
{
    clearClusters();
}

void ThroughputDistribution::configure( Params const& params )
{
    mMaxClusters = params.mClusters;
    mAlpha = 1.0 - exp( log( 0.5 ) / params.mHalflife );
    mOneMinusAlpha = 1.0 - mAlpha;
    mWarmup = params.mWarmup;
    mEffectiveCountAlpha = params.mEffectiveCountAlpha;
}

void ThroughputDistribution::updateBandwidth()
{
    double const variance = ( mAverageSquare - mAverage * mAverage );
    if ( variance <= 0 || mClustersByMean.empty() )
    {
        mBandwidth = 100.0;
    }
    else
    {
        mBandwidth = 1.06 * sqrt( variance ) * pow( mClustersByMean.size(), -0.2 );
        //mBandwidth = 1.06 * sqrt( variance ) * pow( mWeightedCount, -0.2 );
        if ( mBandwidth < 100.0 ) mBandwidth = 100.0;
    }
}

void ThroughputDistribution::add( int throughput, AseTimeVal const& now )
{
    //cout << "ThroughputDistribution::add( " << throughput << ")" << endl;
    //cout << "ThroughputDistribution:" << __LINE__ << " " << debug() << endl;

    // Input conditions
    // mClustersByMean lists clusters in order of mean
    // mClusterByCost lists clusters in order of merge cost
    assert( mClustersByMean.empty() || ( mClustersByCost.size() == ( mClustersByMean.size() - 1 ) ) );

    // Update counters
    if ( mCount < mWarmup )
    {
        mCount += 1;
        mAverage = ( mAverage * ( mCount - 1 ) + throughput ) / mCount;
        mAverageSquare = ( mAverageSquare * ( mCount - 1 ) + ((double) throughput) * throughput ) / mCount;
    }
    else
    {
        mWeightedCount *= mOneMinusAlpha;
        mAverage = mOneMinusAlpha * mAverage + mAlpha * throughput;
        mAverageSquare = mOneMinusAlpha * mAverageSquare + mAlpha * ((double) throughput) * throughput;
    }

    mWeightedCount += 1;

    mEffectiveCount = effectiveCount( now ) + 1;
    mEffectiveCountTime = now;

    // Calculate bandwidth
    updateBandwidth();

    // Create new cluster with weight alpha/(1-alpha)
    double const weight = ( mCount < mWarmup ) ? ( ( mCount == 1 ) ? 1.0 : 1.0 / ( mCount - 1 ) )
                                               : mAlpha / mOneMinusAlpha;
    Cluster* pCluster = new Cluster( throughput, 1, weight );

    // Find surrounding clusters in mean order
    ClustersByMean::iterator next = mClustersByMean.lower_bound( pCluster );

    if ( next != mClustersByMean.end() && (*next)->mMean == pCluster->mMean )
    {
        ClustersByMean::iterator const cluster = next;

        // Add clusters
        (*cluster)->merge( (*cluster)->mMean, *pCluster );

        ClustersByMean::iterator following = cluster;
        ++following;

        // Re-calculate merge costs with successor
        if ( following != mClustersByMean.end() )
        {
            removefromcosttree( cluster );

            (*cluster)->updatemergecost( **following, mBandwidth );

            insertincosttree( cluster );
        }

        delete pCluster;

        //cout << "ThroughputDistribution:" << __LINE__ << " " << debug() << endl;

        assert( mClustersByCost.size() == ( mClustersByMean.size() - 1 ) );
    }
    else
    {
        ClustersByMean::iterator poshint = next;
        if ( next != mClustersByMean.begin() ) --poshint;

        ClustersByMean::iterator const cluster = mClustersByMean.insert( poshint, pCluster );

        next = cluster; ++next;

        if ( next != mClustersByMean.end() )
        {
            // Calculate merge cost for new cluster with next one
            pCluster->updatemergecost( **next, mBandwidth );

            insertincosttree( cluster );
        }

        // Calculate merge cost for preceding cluster with new one
        if ( cluster != mClustersByMean.begin() )
        {
            ClustersByMean::iterator prev = cluster; --prev;

            removefromcosttree( prev );

            (*prev)->updatemergecost( *pCluster, mBandwidth );

            insertincosttree( prev );
        }

        //cout << "ThroughputDistribution:" << __LINE__ << " " << debug() << endl;

        assert( mClustersByCost.size() == ( mClustersByMean.size() - 1 ) );
    }

    mergeclusters();

    //cout << "ThroughputDistribution:" << __LINE__ << " " << debug() << endl;

    // Normailze the weights
    if ( mCount > 1 )
    {
        double const multiplier = ( mCount < mWarmup ) ? ( mCount - 1 ) * 1.0 / mCount : mOneMinusAlpha;
        ClustersByMean::iterator it = mClustersByMean.begin();
        while( it != mClustersByMean.end() )
        {
            ClustersByMean::iterator next = it; ++next;

            (*it)->mWeight *= multiplier;

            if ( (*it)->mWeight == 0 )
            {
                removefromcosttree( it );
                mClustersByMean.erase( it );
            }

            it = next;
        }
    }
}

void ThroughputDistribution::mergeclusters()
{
    bool rebuiltcosttree = false;

    while ( (int) mClustersByMean.size() > mMaxClusters )
    {
        // Find cluster with lowest merge cost
        ClustersByMean::iterator merge = mClustersByMean.find( *mClustersByCost.begin() );

        if( merge == mClustersByMean.end() )
        {
            assert( !rebuiltcosttree );
            (void)(rebuiltcosttree);

            rebuildcosttree();
            rebuiltcosttree = true;
            continue;
        }

        ClustersByMean::iterator target = merge;
        ++target;

        assert( target != mClustersByMean.end() );

        assert( mClustersByCost.size() == ( mClustersByMean.size() - 1 ) );

        // We're about to change the costs
        removefromcosttree( target );
        removefromcosttree( merge );

        // Merge with next cluster
        int const mergedmean = (*merge)->mergepoint( **target, mBandwidth );
        if ( mergedmean < (*target)->mMean )
        {
            assert( mergedmean >= (*merge)->mMean );

            // Our position in the mean tree has not changed - we can do this in-place
            (*merge)->merge( mergedmean, **target );

            // remove target
            Cluster* pTarget = *target;
            mClustersByMean.erase( target );    // XXX don't use target after this!
            delete pTarget;
        }
        else
        {
            assert( mergedmean == (*target)->mMean );

            // Update the target and remove merge instead
            (*target)->merge( mergedmean, **merge );

            // remove merge
            Cluster* pMerge = *merge;
            mClustersByMean.erase( merge );    // XXX don't use merge after this!
            delete pMerge;

            merge = target;
        }

        //cout << "ThroughputDistribution:" << __LINE__ << " " << debug() << endl;

        // re-insert the merge into the cost ordering
        ClustersByMean::iterator next = merge;
        ++next;

        if ( next != mClustersByMean.end() )
        {
            (*merge)->updatemergecost( **next, mBandwidth );
            insertincosttree( merge );
        }

        assert( mClustersByCost.size() == ( mClustersByMean.size() - 1 ) );
    }
}

void ThroughputDistribution::insertincosttree( ClustersByMean::iterator cluster )
{
    assert( !(*cluster)->mInCostTree );

    (*cluster)->mCostPosition = mClustersByCost.insert( *cluster );

    (*cluster)->mInCostTree = true;

    // The last element in the means list should never be in the cost tree
    assert( ++cluster != mClustersByMean.end() );
}

void ThroughputDistribution::removefromcosttree( ClustersByMean::iterator cluster )
{
    if ( (*cluster)->mInCostTree )
    {
        mClustersByCost.erase( (*cluster)->mCostPosition );
        (*cluster)->mInCostTree = false;
    }
}

void ThroughputDistribution::rebuildcosttree()
{
    mClustersByCost.clear();

    ClustersByMean::iterator it = mClustersByMean.begin();
    ClustersByMean::iterator last = mClustersByMean.end(); --last;
    for( ; it != last; ++it )
    {
        (*it)->mInCostTree = false;
        insertincosttree( it );
    }

    (*last)->mInCostTree = false;
}

double ThroughputDistribution::pdf( int throughput ) const
{
    double value = 0.0;
    ClustersByMean::const_iterator it = mClustersByMean.begin();
    for( ; it != mClustersByMean.end(); ++it )
    {
        value += (*it)->pdf( throughput, mBandwidth );
    }

    return value;
}

double ThroughputDistribution::cdf( int throughput ) const
{
    double value = 0.0;
    ClustersByMean::const_iterator it = mClustersByMean.begin();
    for( ; it != mClustersByMean.end(); ++it )
    {
        value += (*it)->cdf( throughput, mBandwidth );
    }

    return value;
}

int ThroughputDistribution::invcdf( double quantile ) const
{
    // Do a binary search for the requested quantile
    if ( mClustersByMean.empty() )
        return 0;

    int min = (int) ((*mClustersByMean.begin())->mMean - mBandwidth);
    int max = (int) ((*mClustersByMean.rbegin())->mMean + mBandwidth);

    while( ( max - min ) > 50 )
    {
        int const mid = ( max + min ) / 2;
        if ( cdf( mid ) > quantile ) max = mid;
        else min = mid;
    }

    int const result = ( max + min ) / 2;

    return result < 0 ? 0 : result;
}

std::pair<int,int> ThroughputDistribution::support() const
{
    if ( mClustersByMean.empty() )
        return make_pair( (int) 0, (int) 0 );

    int const min = (int) ((*mClustersByMean.begin())->mMean - mBandwidth);
    int const max = (int) ((*mClustersByMean.rbegin())->mMean + mBandwidth);

    return make_pair( min, max );
}


double ThroughputDistribution::effectiveCount( AseTimeVal const& now ) const
{
    return mEffectiveCount * exp( - mEffectiveCountAlpha * ( now - mEffectiveCountTime ).ms() );
}

vector<int> ThroughputDistribution::modes() const
{
    vector<int> modes;

    // To calculate the modes we need to partition the space into regions
    // such that each region has a specific set of kernals with support
    // over the whole region.

    // For each partition boundary, record ( boundary position, ( +/-1, Cluster* ) )
    // record a +ve weight for where a cluster starts and a -ve weight where a cluster ends
    if ( !mClustersByMean.empty() )
    {
        priority_queue<pair<int,pair<int,Cluster const*> > > boundaries;
        ClustersByMean::const_iterator it = mClustersByMean.begin();
        for( ; it != mClustersByMean.end(); ++it )
        {
            boundaries.push( make_pair( -( static_cast<int> (static_cast<double> ((*it)->mMean) - mBandwidth) ), make_pair( 1, *it ) ) );
            boundaries.push( make_pair( -( static_cast<int> (static_cast<double> ((*it)->mMean) + mBandwidth) ), make_pair( -1, *it ) ) );
        }

        // The max of a combination of clusters turns out to be at the weighted average of their means
        int low = - boundaries.top().first;
        Cluster const* pCluster = boundaries.top().second.second;
        double weight = pCluster->mWeight * pCluster->mCount;
        double weightedmean = pCluster->mMean * weight;
        int count = 1;
        boundaries.pop();
        while( !boundaries.empty() )
        {
            int const high = - boundaries.top().first;

            if ( count > 0 && weight > 0 )
            {
                int mode = static_cast<int> ( weightedmean / weight );
                if ( mode > low && mode < high ) modes.push_back( mode );
            }

            int const c = boundaries.top().second.first;
            pCluster = boundaries.top().second.second;
            double const w = pCluster->mWeight * pCluster->mCount * c;
            weightedmean += pCluster->mMean * w;
            weight += w;
            count += c;

            low = high;

            boundaries.pop();
        }
    }

    return modes;
}

int ThroughputDistribution::highestmode( int max ) const
{
    int highestmode = 0;
    double highestvalue = 0;
    vector<int> const modepositions = modes();
    vector<int>::const_iterator it = modepositions.begin();
    for( ; it != modepositions.end(); ++it )
    {
        if ( max <= 0 || *it < max )
        {
            double const p = pdf( *it );
            if ( p > highestvalue )
            {
                highestmode = *it;
                highestvalue = p;
            }
        }
    }
    return highestmode;
}

/** Get the state as a Variant */
Variant ThroughputDistribution::getState() const
{
    double minWeight = 0.0;
    double maxWeight = 0.0;

    ClustersByMean::const_iterator it = mClustersByMean.begin();
    for( ; it != mClustersByMean.end(); ++it )
    {
        if ( minWeight == 0.0 || minWeight > (*it)->mWeight )
        {
            minWeight = (*it)->mWeight;
        }

        if ( maxWeight == 0.0 || maxWeight < (*it)->mWeight )
        {
            maxWeight = (*it)->mWeight;
        }
    }

    int const maxMultiplier = (int) ( maxWeight > 1.0 ? std::numeric_limits<int>::max() / maxWeight
                                            : std::numeric_limits<int>::max() );

    int const multiplier = (((int) ( minWeight * maxMultiplier ) > 1000 )) ? (int) ( 1000.0 / minWeight ) : maxMultiplier;

    Variant clusters;
    for( it = mClustersByMean.begin(); it != mClustersByMean.end(); ++it )
    {
        int const weight = (int) ( (*it)->mWeight * multiplier );
        if ( weight > 0 )
        {
            Variant cluster;
            cluster.push_back( (*it)->mMean );
            cluster.push_back( (*it)->mCount );
            cluster.push_back( weight );
            clusters.push_back( cluster );
        }
    }

    Variant state;
    state[ "c" ] = mCount;
    state[ "a" ] = mAverage;
    state[ "b" ] = mBandwidth;
    state[ "as" ] = mAverageSquare;
    state[ "cl" ] = clusters;
    state[ "m" ] = multiplier;
    state[ "ec" ] = mEffectiveCount;
    state[ "ect" ] = (llong) ( mEffectiveCountTime.convertTo<Time>().ms() );

    return state;
}

/** Set the state from a Variant */
void ThroughputDistribution::setState( Variant const& state )
{
    clearClusters();

    mCount = state["c"].integer();
    mAverage = state["a"].dbl();
    mAverageSquare = state["as"].dbl();
    if ( state.contains( "ec" ) )
    {
        mEffectiveCount = state["ec"].dbl();
        mEffectiveCountTime = AseTimeVal::convertFrom( Time::fromMS( variantAsInt( state["ect"] ) ) );
    }
    else
    {
        mEffectiveCount = 0;
        mEffectiveCountTime = AseTimeVal::ZERO;
    }

    double const multipler = state.mapValue( "m", 0, 1000000.0 );

    Variant const& clusters = state["cl"];
    for( int i = 0; i < clusters.size(); ++i )
    {
        Variant const& cluster = clusters[ i ];
        Cluster* pCluster = new Cluster( cluster[0].integer(), cluster[1].integer(), cluster[2].integer() * 1.0 / multipler );
        if ( pCluster->mCount * pCluster->mWeight > 0 )
        {
            mClustersByMean.insert( pCluster );
        }
    }

    updateBandwidth();

    if ( mClustersByMean.size() > 1 )
    {
        ClustersByMean::iterator it = mClustersByMean.begin();
        ClustersByMean::iterator const end = mClustersByMean.end();
        ClustersByMean::iterator next = it;
        ++next;
        for( ; next != end; ++it, ++next )
        {
            (void) (*it)->updatemergecost( **next, mBandwidth );
            insertincosttree( it );
        }
    }

    mergeclusters();
}

void ThroughputDistribution::setEffectiveCount( double effectiveCount, AseTimeVal const& effectiveCountTime )
{
    mEffectiveCount = effectiveCount;
    mEffectiveCountTime = effectiveCountTime;
}

void ThroughputDistribution::clearClusters()
{
    ClustersByMean::iterator it = mClustersByMean.begin();
    for( ; it != mClustersByMean.end(); ++it ) delete *it;

    mClustersByMean.clear();
    mClustersByCost.clear();
}

#if 0
std::string ThroughputDistribution::debug()
{
    std::ostringstream oss;
    oss << "by mean (" << mClustersByMean.size() << "): ";
    ClustersByMean::iterator it = mClustersByMean.begin();
    for( ; it != mClustersByMean.end(); ++it )
    {
        if ( it != mClustersByMean.begin() ) oss << ", ";
        oss << "[" << (*it)->mMean << ";" << (*it)->mMergecost << ";";
        if ( ! (*it)->mInCostTree ) oss << "n";
        else if ((*((*it)->mCostPosition))==(*it)) oss << "ok";
        else oss << "err";
        oss << "]";
    }

    oss << ", by cost (" << mClustersByCost.size() << "): ";
    ClustersByCost::iterator cit = mClustersByCost.begin();
    for( ; cit != mClustersByCost.end(); ++cit )
    {
        if ( cit != mClustersByCost.begin() ) oss << ", ";
        oss << "[" << (*cit)->mMean << ";" << (*cit)->mMergecost << "]";
    }
    return oss.str();
}
#endif
