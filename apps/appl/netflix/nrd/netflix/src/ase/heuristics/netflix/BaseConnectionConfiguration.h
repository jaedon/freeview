/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BASE_CONN_CONFIG_H
#define BASE_CONN_CONFIG_H

#include <nrdase/IConnectionConfiguration.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdase/INetworkMonitor.h>
#include <nrdase/IStreamingReporter.h>
#include <nrdase/AseCommonDataTypes.h>

#include <nrdase/DebugLogging.h>

namespace netflix {
namespace ase {
DECLARE_LOGGER( ConnectionConfigurationLog );

class BaseConnectionConfiguration : public IConnectionConfiguration
{
public:
    BaseConnectionConfiguration( IAseConfigParameterPtr pConfigParameter,
                                 IAseConfigParameterPtr pLocalConfigParameter,
                                 INetworkMonitorPtr pNetworkMonitor,
                                 IStreamingReporterPtr pStreamingReporter);

    virtual ~BaseConnectionConfiguration() {}

    // IConnectionConfiguration
    //
    virtual void configure( IAseConfigParameterPtr pConfigParameter );

    virtual void setPipelineSupported( bool pipeline );

    virtual TrackConfiguration const& getConfiguration( MediaType mediaType ) const;

    virtual AseTimeVal getNextTimeout();

protected:
    void updateMaxConnectionsUsingRtts( std::string const& rttsForMaxConnectionsLimit );


    AseTimeVal                     mTimeout;

    IAseConfigParameterPtr              mConfigParameterPtr;
    IAseConfigParameterPtr              mLocalConfigParametePtr;
    INetworkMonitorPtr                  mNetworkMonitorPtr;

    IStreamingReporterPtr               mStreamingReporterPtr;

    bool                                mPipelineEnabled;
    bool                                mPipelineSupported;

    uint32_t                            mMaxParallelConnections;
    uint32_t                            mTargetPrimaryConnections;

    int                                 mTimeoutIntervalMs;

    uint32_t                            mSocketReceiveBufferSize;

    TrackConfiguration    mAudioConfig;
    TrackConfiguration    mVideoConfig;
    TrackConfiguration    mHeadersConfig;
};


}}
#endif
