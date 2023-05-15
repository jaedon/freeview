/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_STREAM_HEURISTIC_H
#define I_STREAM_HEURISTIC_H

#include <nrdase/IAseConfigParameter.h>
#include <nrdase/IStreamingReporter.h>
#include <nrdase/IPersistentStore.h>
#include <nrdase/IConnectionConfiguration.h>
#include <nrdase/IStreamSelector.h>
#include <nrdase/ILocationSelector.h>
#include <nrdase/ILocationSetMonitor.h>
#include <nrdase/IStreamingStatistics.h>

namespace netflix {
namespace ase {

class Manifest;
class IAseConfigParameter;
class IConnectionConfiguration;
class ILocationSetMonitorFactory;
class IStreamingReporter;

typedef shared_ptr<Manifest> ManifestPtr;
typedef shared_ptr<IAseConfigParameter> IAseConfigParameterPtr;
typedef shared_ptr<IConnectionConfiguration> IConnectionConfigurationPtr;
typedef shared_ptr<ILocationSetMonitorFactory> ILocationSetMonitorFactoryPtr;
typedef shared_ptr<IStreamingReporter> IStreamingReporterPtr;

/**
 * @class IStreamingHeuristic
 * @brief A mini package for binding all streaming heuristic components, which includes
 *        INetworkMonitor, IStreamSelector, and IUrlSelector
 */
class IStreamingHeuristic
{
public:
    typedef shared_ptr<IStreamingHeuristic> IStreamingHeuristicPtr;

    /**
     * @return IStreamSelectorPtr
     */
    virtual IStreamSelectorPtr getStreamSelector() = 0;

    /**
     * @return ILocationSelector
     */
    virtual ILocationSelectorPtr getLocationSelector() = 0;

    /**
     * @return The connection configuration object
     */
    virtual IConnectionConfigurationPtr getConnectionConfiguration() = 0;

    /**
     * @return the streaming statistics object
     */
    virtual IStreamingStatisticsPtr getStreamingStatistics() = 0;

    virtual ~IStreamingHeuristic() {}
};

typedef IStreamingHeuristic::IStreamingHeuristicPtr IStreamingHeuristicPtr;

int32_t constructStreamingHeuristic(
                ManifestPtr                 pManifest,
                IAseConfigParameterPtr      pConfigParameter,
                IAseConfigParameterPtr      pLocalConfigParameter,
                IPersistentStorePtr         pPersistentStore,
                IStreamingReporterPtr       pStreamingReporter,
                IStreamingHeuristicPtr&     pStreamingHeuristic );

ILocationSetMonitorFactoryPtr constructLocationSetMonitorFactory( IAseConfigParameterPtr pConfigParameter );


}}

#endif
