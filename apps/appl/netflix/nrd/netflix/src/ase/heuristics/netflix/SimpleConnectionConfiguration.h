/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SIMPLE_CONNECTION_CONFIGURATION_H
#define SIMPLE_CONNECTION_CONFIGURATION_H

#include "BaseConnectionConfiguration.h"
#include <nrdase/DebugLogging.h>

namespace netflix {
namespace ase {

DECLARE_LOGGER( SimpleConnectionConfiguration );

class SimpleConnectionConfiguration : public BaseConnectionConfiguration
{
public:
    typedef shared_ptr<SimpleConnectionConfiguration> SimpleConnectionConfigurationPtr;

    SimpleConnectionConfiguration(  IAseConfigParameterPtr pConfigParameter,
                                    IAseConfigParameterPtr pLocalConfigParameter,
                                    INetworkMonitorPtr pNetworkMonitor,
                                    IStreamingReporterPtr pStreamingReporter);
    virtual ~SimpleConnectionConfiguration() {}

    // IConnectionConfiguration

    virtual void configure( IAseConfigParameterPtr pConfigParameter );

    /** Handle the timeout event */
    virtual void processTimeout();

    virtual void reportPipeliningInstable() {}

protected:
    uint32_t                            mSingleConnectionThresholdKbps;
};

typedef SimpleConnectionConfiguration::SimpleConnectionConfigurationPtr SimpleConnectionConfigurationPtr;


}} /* namespace netflix::ase */

#endif /* SIMPLE_CONNECTION_CONFIGURATION_H */
