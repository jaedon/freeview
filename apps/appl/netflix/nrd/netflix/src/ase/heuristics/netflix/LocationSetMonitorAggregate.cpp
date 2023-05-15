/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <LocationSetMonitorAggregate.h>
#include <LocationSetMonitor.h>
#include <AggregateThroughputDistribution.h>

#include <algorithm>

using namespace std;
using namespace netflix;
using namespace netflix::ase;

LocationSetMonitorAggregatePtr
LocationSetMonitorAggregate::create( IAseConfigParameterPtr pConfigParameters, std::string const& name )
{
    ThroughputDistributionPredictor::Params tdpparams;
    tdpparams.initialize( pConfigParameters );

    return LocationSetMonitorAggregatePtr( new LocationSetMonitorAggregate( name, tdpparams ) );
}


LocationSetMonitorAggregate::LocationSetMonitorAggregate( std::string const& name,
                                                    ThroughputDistributionPredictor::Params const& tdpparams  )
    : mAggregateName( name )
    , mPredictor( tdpparams )
{
}

void LocationSetMonitorAggregate::configure( IAseConfigParameterPtr pConfigParameters )
{
    ThroughputDistributionPredictor::Params tdpparams;
    tdpparams.initialize( pConfigParameters );

    mPredictor.configure( tdpparams );
}

void LocationSetMonitorAggregate::add( shared_ptr<LocationSetMonitor> pLocationSetMonitor )
{
    LocationSetMonitors::iterator it = std::find( mLocationSetMonitors.begin(), mLocationSetMonitors.end(), pLocationSetMonitor );
    if ( it == mLocationSetMonitors.end() )
    {
        mLocationSetMonitors.push_back( pLocationSetMonitor );
    }
}

void LocationSetMonitorAggregate::remove( shared_ptr<LocationSetMonitor> pLocationSetMonitor )
{
    LocationSetMonitors::iterator it = std::find( mLocationSetMonitors.begin(), mLocationSetMonitors.end(), pLocationSetMonitor );
    if ( it != mLocationSetMonitors.end() )
    {
        mLocationSetMonitors.erase( it );
    }
}

std::string const& LocationSetMonitorAggregate::name() const
{
    return mAggregateName;
}

uint32_t LocationSetMonitorAggregate::predictThroughput(
                                    IThroughputPredictorPtr     /* pCurrent */,
                                    uint32_t                    current,
                                    uint32_t                    currentBufferLevel,
                                    double&                     confidence )
{
    NRDP_UNUSED(currentBufferLevel);
    AseTimeVal const now = AseTimeVal::now();

    return mPredictor.predictThroughput( AggregateThroughputDistribution<DistributionIterator>(
                                                        DistributionIterator( mLocationSetMonitors.begin(), now ),
                                                        DistributionIterator( mLocationSetMonitors.end(), now ) ),
                        0 /* Current throughput distribution */,
                        current,
                        confidence );
}

double LocationSetMonitorAggregate::getConfidence()
{
    AseTimeVal const now = AseTimeVal::now();

    return mPredictor.getConfidence( AggregateThroughputDistribution<DistributionIterator>(
                                                        DistributionIterator( mLocationSetMonitors.begin(), now ),
                                                        DistributionIterator( mLocationSetMonitors.end(), now ) ) );
}


shared_ptr<IThroughputDistribution> LocationSetMonitorAggregate::DistributionIterator::operator*() const
{
    return (*mIterator)->getDistribution( mTime );
}
