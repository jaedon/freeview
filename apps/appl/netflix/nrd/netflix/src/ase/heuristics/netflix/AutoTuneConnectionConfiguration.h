/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef AUTO_TUNE_CONNECTION_CONFIGURATION_H
#define AUTO_TUNE_CONNECTION_CONFIGURATION_H

#include "BaseConnectionConfiguration.h"
#include <nrdase/IAseConfigParameter.h>
#include <nrdase/IPersistentStore.h>
#include <nrdase/INetworkMonitor.h>
#include <nrdase/IStreamingStatistics.h>
#include <nrdase/IStreamingReporter.h>

namespace netflix {
namespace ase {

class AutoTuneConnectionConfiguration : public BaseConnectionConfiguration
{
public:
    typedef shared_ptr<AutoTuneConnectionConfiguration> AutoTuneConnectionConfigurationPtr;

    AutoTuneConnectionConfiguration(  IAseConfigParameterPtr pConfigParameter,
                                      IAseConfigParameterPtr pLocalConfigParameter,
                                      IPersistentStorePtr pPersistentStore,
                                      INetworkMonitorPtr pNetworkMonitor,
                                      IStreamingStatisticsPtr pStreamingStatistics,
                                      IStreamingReporterPtr pStreamingReporter);
    virtual ~AutoTuneConnectionConfiguration();

    // IConnectionConfiguration
    virtual void configure( IAseConfigParameterPtr pConfigParameter );

    /** Handle the timeout event */
    virtual void processTimeout();

    virtual void reportPipeliningInstable();

protected:
    void processInInitState();
    void processInProbingState();
    void processInScheduledState();

    void setVideoConfig( uint32_t numOfConnections );
    /**
     * select the next number of connections to probe
     * @param[in] result: the result of previous test
     * @return the next number of connections to probe, -1 complete the probing
     */
    int32_t selectNextProbingNumber(bool result);


    IPersistentStorePtr                 mPersistentStorePtr;
    IStreamingStatisticsPtr             mStreamingStatisticsPtr;

    uint32_t                            mDefaultNumConnections;
    uint32_t                            mLkgNumConnections; // last known good number of connections
    uint32_t                            mLkgBandwidth; // bandwidth for last known good number of connections
    AseTimeVal                     mProbingStartTime;
    int32_t                             mProbingNumConnections; // the probing # of connections
    uint32_t                            mProbingWatermarkLevel;
    uint32_t                            mProbingLength;
    uint32_t                            mImprovementThreshold;

    uint32_t                            mMinParallelConnections;

    enum InternalState { INIT_STATE,
                         SCHEDULED,
                         PROBING,
                         COMPLETED } mInternalState;
};

typedef AutoTuneConnectionConfiguration::AutoTuneConnectionConfigurationPtr AutoTuneConnectionConfigurationPtr;


}} /* namespace netflix::ase */

#endif /* AUTO_TUNE_CONNECTION_CONFIGURATION_H */
