/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SIMPLE_LOCATION_SET_MONITOR_AGGREGATE_H
#define SIMPLE_LOCATION_SET_MONITOR_AGGREGATE_H

#include <nrdnet/AseTimeVal.h>
#include <nrdase/AseCommonDataTypes.h>

#include <nrdase/DebugLogging.h>
#include <nrdase/IAseConfigParameter.h>


#include <nrdbase/Variant.h>
#include <nrdbase/Time.h>

#include <nrdase/IThroughputPredictor.h>

#include <string>

namespace netflix {
namespace ase {

class SimpleLocationSetMonitor;

class SimpleLocationSetMonitorAggregate : public IThroughputPredictor
{
public:
    typedef shared_ptr<SimpleLocationSetMonitorAggregate> SimpleLocationSetMonitorAggregatePtr;

public:
    static SimpleLocationSetMonitorAggregatePtr create( IAseConfigParameterPtr pConfigParameters, std::string const& name );

    ~SimpleLocationSetMonitorAggregate() {}

    void configure( IAseConfigParameterPtr pConfigParameters );

    void add( shared_ptr<SimpleLocationSetMonitor> pLocationSetMonitor );

    void remove( shared_ptr<SimpleLocationSetMonitor> pLocationSetMonitor );

    // IThroughputPredictor

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

protected:
    SimpleLocationSetMonitorAggregate( std::string const& name );

protected:
    typedef std::vector<shared_ptr<SimpleLocationSetMonitor> > Monitors;

    std::string const mAggregateName;

    Monitors mMonitors;
};

typedef SimpleLocationSetMonitorAggregate::SimpleLocationSetMonitorAggregatePtr SimpleLocationSetMonitorAggregatePtr;

} // namespace ase
} // namespace netflix

#endif  /* SIMPLE_LOCATION_SET_MONITOR_AGGREGATE_H */
