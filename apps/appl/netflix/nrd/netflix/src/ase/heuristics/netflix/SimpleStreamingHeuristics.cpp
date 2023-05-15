/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SimpleStreamingHeuristics.h"
#include "SimpleStreamSelector.h"
#include "SimpleConnectionConfiguration.h"
#include "AutoTuneConnectionConfiguration.h"
#include "SimpleStreamingStatistics.h"
#include "SimpleLocationSelector.h"
#include "SimpleLocationSetMonitor.h"
#include "LocationSetMonitor.h"
#include "LocationSetMonitorFactory.h"
#include "LocationSetMonitorAggregate.h"
#include "SimpleLocationSetMonitorAggregate.h"

#include "HttpRequestManager.h"
#include "NetworkMonitor.h"

#include <nrdbase/tr1.h>

#include <limits>

namespace netflix {
namespace ase {

DEFINE_LOGGER( StreamingHeuristics );

SimpleStreamingHeuristic::SimpleStreamingHeuristic( ManifestPtr                 pManifest,
                                                    IAseConfigParameterPtr      pConfigParameter,
                                                    IAseConfigParameterPtr      pLocalConfigParameter,
                                                    IPersistentStorePtr         pPersistentStore,
                                                    IStreamingReporterPtr       pStreamingReporter )
{
    mapSimpleParameters( pConfigParameter );

    HttpRequestManager::instance()->configure( pConfigParameter, pPersistentStore );
    NetworkMonitor::instance()->configure( pConfigParameter, pPersistentStore );

    NetworkMonitor::instance()->addServingLocations( pManifest->getServingLocations() );

    SimpleLocationSelector::construct(pManifest, pConfigParameter,
                                      pStreamingReporter, NetworkMonitor::instance(), mSimpleLocationSelectorPtr);

    mStreamSelectorPtr.reset( new SimpleStreamSelector(pConfigParameter, pLocalConfigParameter, NetworkMonitor::instance(), mSimpleLocationSelectorPtr) );

    mStreamingStatisticsPtr.reset( new SimpleStreamingStatistics() );

    if ( pConfigParameter->getParameterAs<bool>("autoTuneConnectionsEnabled", false) )
    {
        mConnectionConfigurationPtr.reset( new AutoTuneConnectionConfiguration(
                                    pConfigParameter, pLocalConfigParameter,
                                    pPersistentStore,
                                    NetworkMonitor::instance(), mStreamingStatisticsPtr,
                                    pStreamingReporter) );
    }
    else
    {
        mConnectionConfigurationPtr.reset( new SimpleConnectionConfiguration(
                                    pConfigParameter, pLocalConfigParameter,
                                    NetworkMonitor::instance(),
                                    pStreamingReporter) );
    }
    return;
}

SimpleStreamingHeuristic::~SimpleStreamingHeuristic()
{
}

IStreamSelectorPtr SimpleStreamingHeuristic::getStreamSelector()
{
    return mStreamSelectorPtr;
}

shared_ptr<ILocationSelector> SimpleStreamingHeuristic::getLocationSelector()
{
    return mSimpleLocationSelectorPtr;
}

IConnectionConfigurationPtr SimpleStreamingHeuristic::getConnectionConfiguration()
{
    return mConnectionConfigurationPtr;
}

IStreamingStatisticsPtr SimpleStreamingHeuristic::getStreamingStatistics()
{
    return mStreamingStatisticsPtr;
}

void SimpleStreamingHeuristic::mapSimpleParameters( IAseConfigParameterPtr pConfigParameters )
{
    /* Input:
     *      distributionUpdateInterval (ms)
     *      minProbeTime (ms)
     *      probeTime (ms)
     *      waitTime (ms)
     *      throughputHighConfidenceLevel
     *
     * Output:
     *      distributionUpdateInterval
     *      measurementHalfLife
     *      minMeasurementTime
     *      throughputLowConfidenceLevel
     *      throughputHighConfidenceLevel
     */

    int const intmax = std::numeric_limits<int32_t>::max();

    int32_t const minProbeTime = pConfigParameters->getParameterAs<int32_t>( "minProbeTime", 0 );

    if ( minProbeTime != 0 )
    {
        int32_t const interval = pConfigParameters->getParameterAs<int32_t>( "distributionUpdateInterval", 100, intmax, 2000 );
        int32_t const probeTime = pConfigParameters->getParameterAs<int32_t>( "probeTime", 500, intmax, 10000 );
        int32_t const waitTime = pConfigParameters->getParameterAs<int32_t>( "waitTime", 30000, intmax, 10 * 60 * 1000 );
        double const highconfidence = pConfigParameters->getParameterAs<double>( "throughputHighConfidenceLevel", 0.0, 1.0, 0.9 );

        double const nlow           = minProbeTime * 1.0 / interval;
        double const nhigh          = probeTime * 1.0 / interval;
        double const alpha          = -log( nlow / nhigh ) / waitTime;
        double const halflife       = - log( 0.5 ) / alpha;
        double const x              = exp( - alpha * interval );
        double const clow           = ( 1.0 - pow( x, nlow ) ) / ( 1.0 - x );
        double const chigh          = ( 1.0 - pow( x, nhigh ) ) / ( 1.0 - x );
        double const cfactor        = ( 1.0 - 1.0 / sqrt( chigh ) ) / highconfidence;
        double const cmax           = pow( ( 1.0 / ( 1.0 - cfactor ) ), 2.0 );
        double const lowconfidence  = ( 1.0 - 1.0 / sqrt( clow ) ) / cfactor;
        double const n              = log( 1.0 - ( 1.0 - x ) * cmax ) / log( x );

        pConfigParameters->setParameter( "distributionUpdateInterval", interval );
        pConfigParameters->setParameter( "measurementHalfLife", (int) halflife );
        pConfigParameters->setParameter( "minMeasurementTime", (int) ( n * interval ) );
        pConfigParameters->setParameter( "throughputLowConfidenceLevel", lowconfidence );
        pConfigParameters->setParameter( "throughputHighConfidenceLevel", highconfidence );

        DEBUGLOG( StreamingHeuristics,  "distributionUpdateInterval=%dms, "
                                        "measurementHalfLife=%dms, "
                                        "minMeasurementTime=%dms, "
                                        "throughputLowConfidenceLevel=%lf, "
                                        "throughputHighConfidenceLevel=%lf",
                                        pConfigParameters->getParameterAs<int32_t>( "distributionUpdateInterval", -1 ),
                                        pConfigParameters->getParameterAs<int32_t>( "measurementHalfLife", -1 ),
                                        pConfigParameters->getParameterAs<int32_t>( "minMeasurementTime", -1 ),
                                        pConfigParameters->getParameterAs<double>( "throughputLowConfidenceLevel", 0.0 ),
                                        pConfigParameters->getParameterAs<double>( "throughputHighConfidenceLevel", 0.0 ));

    }
}

int32_t constructStreamingHeuristic(
                ManifestPtr                 pManifest,
                IAseConfigParameterPtr      pConfigParameter,
                IAseConfigParameterPtr      pLocalConfigParameter,
                IPersistentStorePtr         pPersistentStore,
                IStreamingReporterPtr       pStreamingReporter,
                IStreamingHeuristicPtr&     pStreamingHeuristic )
{
    pStreamingHeuristic.reset( new
                    SimpleStreamingHeuristic(pManifest, pConfigParameter, pLocalConfigParameter, pPersistentStore, pStreamingReporter ) );
    return 0;
}

ILocationSetMonitorFactoryPtr constructLocationSetMonitorFactory( IAseConfigParameterPtr pConfigParameter )
{
    typedef LocationSetMonitorFactory<SimpleLocationSetMonitor, SimpleLocationSetMonitorAggregate,0>
            SimpleLocationSetMonitorFactory;

    typedef LocationSetMonitorFactory<LocationSetMonitor,LocationSetMonitorAggregate,1>
            LocationSetMonitorFactory;

    int32_t const locationSetMonitorType = pConfigParameter->getParameterAs<int32_t>( "locationSetMonitorType", 0, 1, 0 );

    ILocationSetMonitorFactoryPtr pLocationSetMonitorFactory;

    switch( locationSetMonitorType )
    {
        case 1:
            pLocationSetMonitorFactory.reset( new LocationSetMonitorFactory() );
            break;

        default:
            pLocationSetMonitorFactory.reset( new SimpleLocationSetMonitorFactory() );
            break;
    }

    return pLocationSetMonitorFactory;
}



}}
