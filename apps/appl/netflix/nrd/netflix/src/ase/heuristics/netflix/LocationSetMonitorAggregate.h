/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef LOCATION_SET_MONITOR_AGGREGATE_H
#define LOCATION_SET_MONITOR_AGGREGATE_H

#include <nrdnet/AseTimeVal.h>
#include <nrdase/AseCommonDataTypes.h>

#include <nrdbase/Variant.h>
#include <nrdbase/Time.h>

#include <ThroughputDistributionPredictor.h>

#include <string>

namespace netflix {
namespace ase {

class LocationSetMonitor;
typedef shared_ptr<LocationSetMonitor> LocationSetMonitorPtr;

class LocationSetMonitorAggregate : public IThroughputPredictor
{
public:
    typedef shared_ptr<LocationSetMonitorAggregate> LocationSetMonitorAggregatePtr;

public:
    static LocationSetMonitorAggregatePtr create( IAseConfigParameterPtr pConfigParameters, std::string const& name );

    ~LocationSetMonitorAggregate() {}

    void configure( IAseConfigParameterPtr pConfigParameters );

    /**
     * obtain the predicted throughput from historical data
     * @param[in] current The current short-term throughput
     * @param[out] confidence: a float number between 0 and 1 to indicate the confidence
     *                         level of the estimated throughput
     */
    virtual uint32_t predictThroughput( IThroughputPredictorPtr     pCurrent,
                                        uint32_t                    current,
                                        uint32_t                    currentBufferLevel,
                                        double&                     confidence );

    /**
     * get the confidence value for the historical information
     */
    virtual double getConfidence();

    virtual std::string const& name() const;

    void add( shared_ptr<LocationSetMonitor> pLocationSetMonitor );

    void remove( shared_ptr<LocationSetMonitor> pLocationSetMonitor );

protected:
    LocationSetMonitorAggregate(
                            std::string const& name,
                            ThroughputDistributionPredictor::Params const& tdpparams );

    typedef std::vector<LocationSetMonitorPtr> LocationSetMonitors;

    /** Iterator that traverses location set monitors getting the currently interesting
     *  throughput distribution
     */
    struct DistributionIterator
    {
        LocationSetMonitors::iterator   mIterator;
        AseTimeVal                 mTime;

        DistributionIterator( LocationSetMonitors::iterator it, AseTimeVal const& now );

        DistributionIterator( DistributionIterator const& other );

        bool operator==( DistributionIterator const& other ) const;

        bool operator!=( DistributionIterator const& other ) const;

        shared_ptr<IThroughputDistribution> operator*() const;

        DistributionIterator& operator++();

        DistributionIterator operator++( int );
    };

protected:
    std::string const mAggregateName;

    LocationSetMonitors             mLocationSetMonitors;

    ThroughputDistributionPredictor mPredictor;
};

typedef LocationSetMonitorAggregate::LocationSetMonitorAggregatePtr LocationSetMonitorAggregatePtr;

inline LocationSetMonitorAggregate::DistributionIterator::DistributionIterator(
            LocationSetMonitorAggregate::LocationSetMonitors::iterator it,
            AseTimeVal const& now )
    : mIterator( it )
    , mTime( now )
{
}

inline LocationSetMonitorAggregate::DistributionIterator::DistributionIterator(
            LocationSetMonitorAggregate::DistributionIterator const& other )
    : mIterator( other.mIterator )
    , mTime( other.mTime )
{
}

inline bool LocationSetMonitorAggregate::DistributionIterator::operator==(
            LocationSetMonitorAggregate::DistributionIterator const& other ) const
{
    return mIterator == other.mIterator;
}

inline bool LocationSetMonitorAggregate::DistributionIterator::operator!=(
            LocationSetMonitorAggregate::DistributionIterator const& other ) const
{
    return mIterator != other.mIterator;
}

inline LocationSetMonitorAggregate::DistributionIterator&
LocationSetMonitorAggregate::DistributionIterator::operator++()
{
    ++mIterator;
    return *this;
}

inline LocationSetMonitorAggregate::DistributionIterator
LocationSetMonitorAggregate::DistributionIterator::operator++( int )
{
    DistributionIterator res( *this );
    ++mIterator;
    return res;
}


} // namespace ase
} // namespace netflix

#endif  /* LOCATION_SET_MONITOR_AGGREGATE_H */
