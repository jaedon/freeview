/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <SimpleLocationSetMonitorAggregate.h>
#include <SimpleLocationSetMonitor.h>

#include <algorithm>

using namespace std;
using namespace netflix;
using namespace netflix::ase;

SimpleLocationSetMonitorAggregatePtr SimpleLocationSetMonitorAggregate::create( IAseConfigParameterPtr /* pConfigParameters */, std::string const& name )
{
    SimpleLocationSetMonitorAggregatePtr pSimpleLocationSetMonitorAggregate( new SimpleLocationSetMonitorAggregate( name ) );

    return pSimpleLocationSetMonitorAggregate;
}

SimpleLocationSetMonitorAggregate::SimpleLocationSetMonitorAggregate( std::string const& name )
    : mAggregateName( name )
{
}

void SimpleLocationSetMonitorAggregate::configure( IAseConfigParameterPtr /* pConfigParameters */ )
{
}

void SimpleLocationSetMonitorAggregate::add( shared_ptr<SimpleLocationSetMonitor> pSimpleLocationSetMonitor )
{
    mMonitors.push_back( pSimpleLocationSetMonitor );
}

void SimpleLocationSetMonitorAggregate::remove( shared_ptr<SimpleLocationSetMonitor> pSimpleLocationSetMonitor )
{
    Monitors::iterator it = std::find( mMonitors.begin(), mMonitors.end(), pSimpleLocationSetMonitor );
    if ( it != mMonitors.end() )
        mMonitors.erase( it );
}


uint32_t SimpleLocationSetMonitorAggregate::predictThroughput(  IThroughputPredictorPtr     pCurrent,
                                                                uint32_t                    current,
                                                                uint32_t                    currentBufferLevel,
                                                                double&                     confidence )
{
    // Confidence-weighted estimate from the aggregated monitors
    if ( mMonitors.empty() )
    {
        confidence = 0.0;
        return 0;
    }

    double weightedTotalThroughput = 0;
    double totalConfidence = 0;

    Monitors::const_iterator it = mMonitors.begin();
    for( ; it != mMonitors.end(); ++it )
    {
        double thisConfidence;
        uint32_t throughput = (*it)->predictThroughput( pCurrent, current, currentBufferLevel, thisConfidence );

        weightedTotalThroughput = throughput * thisConfidence;
        totalConfidence += thisConfidence;
    }

    confidence = totalConfidence / mMonitors.size();

    return ( totalConfidence > 0 ) ? (uint32_t) ( weightedTotalThroughput / totalConfidence ) : 0;
}

double SimpleLocationSetMonitorAggregate::getConfidence()
{
    double confidence;

    (void) predictThroughput( IThroughputPredictorPtr(), 0, 0, confidence );

    return confidence;
}

std::string const& SimpleLocationSetMonitorAggregate::name() const
{
    return mAggregateName;
}
