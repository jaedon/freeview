/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SIMPLE_STREAMING_HEURISTIC_H
#define SIMPLE_STREAMING_HEURISTIC_H

#include <nrdase/IStreamingHeuristic.h>
#include <nrdase/DebugLogging.h>

namespace netflix {
namespace ase {

DECLARE_LOGGER( StreamingHeuristics );

class SimpleStreamSelector;
class SimpleLocationSelector;

typedef shared_ptr<SimpleStreamSelector> SimpleStreamSelectorPtr;
typedef shared_ptr<SimpleLocationSelector> SimpleLocationSelectorPtr;

/**
 * @class SimpleStreamingHeuristic
 * @brief A mini package for binding SimpleNetworkMonitor, SimpleStreamSelector, and SimpleLocationSelector
 */
class SimpleStreamingHeuristic : public IStreamingHeuristic
{
public:
    typedef shared_ptr<SimpleStreamingHeuristic> SimpleStreamingHeuristicPtr;

    SimpleStreamingHeuristic(   ManifestPtr                 pManifest,
                                IAseConfigParameterPtr      pConfigParameter,
                                IAseConfigParameterPtr      pLocalConfigParameter,
                                IPersistentStorePtr         pPersistentStore,
                                IStreamingReporterPtr       pStreamingReporter );

    virtual IStreamSelectorPtr getStreamSelector();

    virtual shared_ptr<ILocationSelector> getLocationSelector();

    virtual IConnectionConfigurationPtr getConnectionConfiguration();

    virtual IStreamingStatisticsPtr getStreamingStatistics();

    virtual ~SimpleStreamingHeuristic();

protected:
    void mapSimpleParameters( IAseConfigParameterPtr pConfigParameters );

protected:

    SimpleStreamSelectorPtr             mStreamSelectorPtr;
    SimpleLocationSelectorPtr           mSimpleLocationSelectorPtr;

    IConnectionConfigurationPtr         mConnectionConfigurationPtr;
    IStreamingStatisticsPtr             mStreamingStatisticsPtr;
};

typedef SimpleStreamingHeuristic::SimpleStreamingHeuristicPtr SimpleStreamingHeuristicPtr;

}}
#endif
