/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef THROUGHPUT_DISTRIBUTION
#define THROUGHPUT_DISTRIBUTION

#include <vector>
#include <set>
#include <nrdbase/Variant.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdase/IAseConfigParameter.h>
#include "IThroughputDistribution.h"

namespace netflix {
namespace ase {

/** Class representing the throughput probability distribution function for a location
 */
class ThroughputDistribution : public IThroughputDistribution
{
public:
    typedef shared_ptr<ThroughputDistribution> ThroughputDistributionPtr;

    struct Params
    {
        int mClusters;
        int mHalflife;
        int mWarmup;
        double mEffectiveCountAlpha;

        Params();

        void initialize( IAseConfigParameterPtr pConfigParameters );
    };

    ThroughputDistribution( Params const& params );

    ThroughputDistribution( ThroughputDistribution const& other );

    ~ThroughputDistribution();

    void configure( Params const& params );

    void add( int throughput, AseTimeVal const& now );

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

    /** Get the state as a Variant */
    Variant getState() const;

    /** Set the state from a Variant */
    void setState( Variant const& state );

    /** Set the effective count */
    void setEffectiveCount( double effectiveCount, AseTimeVal const& effectiveCountTime );

    /** Calculate the effective count associated with regular measurements over a given period */
    static double effectiveCountForActiveTime(  int effectiveCountHalflife,
                                                int distributionUpdateInterval,
                                                int maxConfidenceTime );

protected:
    void updateBandwidth();

    void mergeclusters();

protected:
    class Cluster;

    struct ClusterMeanComparator
    {
        bool operator()( Cluster* const& lhs, Cluster* const& rhs ) const;
    };

    struct ClusterCostComparator
    {
        bool operator()( Cluster* const& lhs, Cluster* const& rhs ) const;
    };

    /** Clusters, in order of increasing mean */
    typedef std::set<Cluster*,ClusterMeanComparator> ClustersByMean;


    /** Clusters by increasing merge cost */
    typedef std::multiset<Cluster*,ClusterCostComparator> ClustersByCost;

    class Cluster
    {
    public:
        Cluster( int mean, int count, double weight );

        Cluster( Cluster const& other );

        /** Calculate merge position */
        int mergepoint( Cluster const& other, double bandwidth );

        /** Merge clusters */
        void merge( int mean, Cluster const& other );

        /** Calculate cluster merge cost */
        void updatemergecost( Cluster const& other, double bandwidth );

        /** Evaluate the cluster pdf */
        double pdf( int x, double bandwidth ) const;

        /** Evaluate the cluster cdf */
        double cdf( int x, double bandwidth ) const;

    protected:
        static double kernel( double x );

        static double kernel_cdf( double x );

    public:
        int     mMean;      // mean for this cluster
        int     mCount;     // number of kernels in this cluster
        double  mWeight;    // individual kernel weight
        double  mMergecost; // cost to merge with next cluster

        bool                        mInCostTree;
        ClustersByCost::iterator    mCostPosition;
    };

protected:

    void insertincosttree( ClustersByMean::iterator cluster );

    void removefromcosttree( ClustersByMean::iterator cluster );

    void rebuildcosttree();

    void clearClusters();

    //std::string debug();

protected:
    ClustersByMean  mClustersByMean;
    ClustersByCost  mClustersByCost;

    int             mMaxClusters;   // maximum number of clusters
    double          mAlpha;         // alpha value for exponentialy weighted moving average
    double          mOneMinusAlpha; // 1 - alpha
    int             mWarmup;        // number of measurements before starting moving average
    double          mEffectiveCountAlpha;
    int             mCount;         // number of measurements so far (during warmup)
    double          mWeightedCount; // exponentially weighted count of measurements
    double          mEffectiveCount;// exponentially time weighted count of measurements
    AseTimeVal mEffectiveCountTime; // time at which the effective count was last updated
    double          mAverage;       // ewma sum of measurements
    double          mAverageSquare; // ewma sum of squared of measurements
    double          mBandwidth;     // current kernel bandwidth
};


typedef ThroughputDistribution::ThroughputDistributionPtr ThroughputDistributionPtr;

inline bool ThroughputDistribution::ClusterMeanComparator::operator()( ThroughputDistribution::Cluster* const& lhs, ThroughputDistribution::Cluster* const& rhs ) const
{
    return lhs->mMean < rhs->mMean;
}

inline bool ThroughputDistribution::ClusterCostComparator::operator()( ThroughputDistribution::Cluster* const& lhs, ThroughputDistribution::Cluster* const& rhs ) const
{
    return lhs->mMergecost < rhs->mMergecost;
}

} // namespace ase
} // namespace netflix

#endif
