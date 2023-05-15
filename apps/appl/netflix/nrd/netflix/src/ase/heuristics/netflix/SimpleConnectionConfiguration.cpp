/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SimpleConnectionConfiguration.h"

#include <nrdase/ILocationSetMonitor.h>


namespace netflix {
namespace ase {

DEFINE_LOGGER( SimpleConnectionConfiguration );

SimpleConnectionConfiguration::SimpleConnectionConfiguration(
                            IAseConfigParameterPtr pConfigParameter,
                            IAseConfigParameterPtr pLocalConfigParameter,
                            INetworkMonitorPtr pNetworkMonitor,
                            IStreamingReporterPtr pStreamingReporter)
    : BaseConnectionConfiguration(IAseConfigParameterPtr(), pLocalConfigParameter, pNetworkMonitor, pStreamingReporter)
{
    if (pConfigParameter)
        configure( pConfigParameter );
}

void SimpleConnectionConfiguration::configure( IAseConfigParameterPtr pConfigParameter )
{
    BaseConnectionConfiguration::configure( pConfigParameter );

    mSingleConnectionThresholdKbps = pConfigParameter
                                ->getParameterAs<uint32_t>(
                                    "singleConnectionThreshold",
                                    0 );
    if ( mPipelineEnabled && mPipelineSupported )
    {
        mVideoConfig.mOpenRange = false;
        mVideoConfig.mPipeline = true;
        mVideoConfig.mConnections = mMaxParallelConnections;
    }
    else
    {
        mVideoConfig.mOpenRange = true;
        mVideoConfig.mPipeline = false;
    }

    mAudioConfig.mSocketBufferSize = pConfigParameter
                                    ->getParameterAs<uint32_t>( "audioSocketReceiveBufferSize",
                                mAudioConfig.mSocketBufferSize );

    mVideoConfig.mSocketBufferSize = pConfigParameter
                                    ->getParameterAs<uint32_t>( "videoSocketReceiveBufferSize",
                                mVideoConfig.mSocketBufferSize );

    mHeadersConfig.mSocketBufferSize = pConfigParameter
                                    ->getParameterAs<uint32_t>( "headersSocketReceiveBufferSize",
                                mHeadersConfig.mSocketBufferSize);    
    
    DEBUGLOG(ConnectionConfigurationLog, "SimpleConnectionConfiguration::configure: pipeline %s %s, video %s range %s %d connections, audio %s range %s",
                mPipelineEnabled ? "enabled" : "disabled",
                mPipelineSupported ? "supported" : "unsupported",
                mVideoConfig.mOpenRange ? "open" : "closed",
                mVideoConfig.mPipeline ? "pipeline" : "non-pipeline",
                mVideoConfig.mConnections,
                mAudioConfig.mOpenRange ? "open" : "closed",
                mAudioConfig.mPipeline ? "pipeline" : "non-pipeline"  );
}

/** Handle the timeout event */
void SimpleConnectionConfiguration::processTimeout()
{
    AseTimeVal const now = AseTimeVal::now();

    if ( mSingleConnectionThresholdKbps > 0 && mVideoConfig.mPipeline )
    {
        ILocationSetMonitorPtr pLocationSetMonitor = mNetworkMonitorPtr->getCurrentLocationSetMonitor();
        if ( pLocationSetMonitor )
        {
            uint32_t const throughput = pLocationSetMonitor->getShortTermThroughput( now );

            if ( throughput > mSingleConnectionThresholdKbps )
            {
                mVideoConfig.mPipeline = false;
                mVideoConfig.mConnections = 1;
            }
        }
    }

    mTimeout = ( mTimeoutIntervalMs > 0 ) ? now + AseTimeVal::fromMS( mTimeoutIntervalMs )
                    : AseTimeVal::INFINITE;
}


}} // namespace netflix::ase
