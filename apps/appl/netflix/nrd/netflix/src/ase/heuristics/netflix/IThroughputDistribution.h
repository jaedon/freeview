/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ITHROUGHPUT_DISTRIBUTION
#define ITHROUGHPUT_DISTRIBUTION

#include <nrdnet/AseTimeVal.h>

namespace netflix {
namespace ase {

/** Abstract class representing a throughput probability distribution function
 */
class IThroughputDistribution
{
public:
    typedef shared_ptr<IThroughputDistribution> IThroughputDistributionPtr;

    virtual ~IThroughputDistribution() {}

    /** Pdf evaluation operation
     * @param throughput [in] A throughput value
     * @result the probability density at the provided throughput
     */
    virtual double pdf( int throughput ) const = 0;

    /** Cdf evaluation operator
     * @param throughput [in] A throughput value
     * @rusult the probability that throughput is less than or equal to the provided value
     */
    virtual double cdf( int throughput ) const = 0;

    /** Get the time weighted effective number of measurements
     * @result The time-weighted effective number of measurements
     */
    virtual double effectiveCount( AseTimeVal const& now ) const = 0;

    /** Evalute the inverse cdf */
    virtual int invcdf( double quantile ) const = 0;

    /** Find the modes of the distribution */
    virtual std::vector<int> modes() const = 0;

    /** Find the highest mode of the distribution less than a maximum */
    virtual int highestmode( int max = -1 ) const = 0;

    /** Get the support of the distribution */
    virtual std::pair<int,int> support() const = 0;
};

typedef IThroughputDistribution::IThroughputDistributionPtr IThroughputDistributionPtr;

} // namespace ase
} // namespace netflix

#endif /* ITHROUGHPUT_DISTRIBUTION */
