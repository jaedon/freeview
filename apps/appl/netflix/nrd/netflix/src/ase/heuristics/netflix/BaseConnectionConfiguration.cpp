/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "BaseConnectionConfiguration.h"
#include <nrdase/AseCommonUtil.h>
#include <nrdase/AseCommonDataTypes.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdase/ILocationSetMonitor.h>

#include <string>

using namespace std;

namespace netflix {
namespace ase {

DEFINE_LOGGER( ConnectionConfigurationLog );

BaseConnectionConfiguration::BaseConnectionConfiguration(
                    IAseConfigParameterPtr pConfigParameter,
                    IAseConfigParameterPtr pLocalConfigParameter,
                    INetworkMonitorPtr     pNetworkMonitor,
                    IStreamingReporterPtr pStreamingReporter)
    : mTimeout( AseTimeVal::INFINITE )
    , mConfigParameterPtr( pConfigParameter )
    , mLocalConfigParametePtr( pLocalConfigParameter )
    , mNetworkMonitorPtr( pNetworkMonitor )
    , mStreamingReporterPtr(pStreamingReporter)
    , mPipelineSupported( true )
    , mAudioConfig( true, false, DEFAULT_MAX_CONNECTIONS, DEFAULT_SOCKET_BUFFER )
    , mVideoConfig( true, false, DEFAULT_MAX_CONNECTIONS, DEFAULT_SOCKET_BUFFER )
    , mHeadersConfig( false, false, 1, DEFAULT_SOCKET_BUFFER )
{
    mSocketReceiveBufferSize =  pLocalConfigParameter
                                    ->getParameterAs<uint32_t>( "socketReceiveBufferSize",
                                DEFAULT_SOCKET_BUFFER );

    mAudioConfig.mSocketBufferSize = pLocalConfigParameter
                                    ->getParameterAs<uint32_t>( "audioSocketReceiveBufferSize",
                                mSocketReceiveBufferSize );

    mVideoConfig.mSocketBufferSize = pLocalConfigParameter
                                    ->getParameterAs<uint32_t>( "videoSocketReceiveBufferSize",
                                mSocketReceiveBufferSize );

    mHeadersConfig.mSocketBufferSize = pLocalConfigParameter
                                    ->getParameterAs<uint32_t>( "headersSocketReceiveBufferSize",
                                mSocketReceiveBufferSize );


    if ( pConfigParameter )
    {
        configure( pConfigParameter );

        mPipelineSupported = mPipelineEnabled;
    }

}

void BaseConnectionConfiguration::configure( IAseConfigParameterPtr pConfigParameter )
{
    mPipelineEnabled = pConfigParameter
                                ->getParameterAs<bool>(
                                    "pipelineEnabled",
                                    DEFAULT_PIPELINE_ENABLED );

    mMaxParallelConnections = pConfigParameter
                                ->getParameterAs<uint32_t>(
                                    "maxParallelConnections",
                                    DEFAULT_MAX_PARALLEL_CONNECTIONS );
    if (!mMaxParallelConnections)
        mMaxParallelConnections = 1;

    mTargetPrimaryConnections = pConfigParameter
                                ->getParameterAs<uint32_t>(
                                    "targetPrimaryConnections",
                                    mMaxParallelConnections );
    if (!mTargetPrimaryConnections)
        mTargetPrimaryConnections = 1;

    string rttsForMaxConnectionsLimit =
                                pConfigParameter->queryParameter("rttsForMaxConnectionsLimit");
    updateMaxConnectionsUsingRtts( rttsForMaxConnectionsLimit );

    mTimeoutIntervalMs = pConfigParameter
                                ->getParameterAs<int>(
                                    "connectionConfigurationTimer",
                                    0 );

    mAudioConfig.mSocketBufferSize = pConfigParameter
                                    ->getParameterAs<uint32_t>( "audioSocketReceiveBufferSize",
                                mAudioConfig.mSocketBufferSize );

    mVideoConfig.mSocketBufferSize = pConfigParameter
                                    ->getParameterAs<uint32_t>( "videoSocketReceiveBufferSize",
                                mVideoConfig.mSocketBufferSize );

    mHeadersConfig.mSocketBufferSize = pConfigParameter
                                    ->getParameterAs<uint32_t>( "headersSocketReceiveBufferSize",
                                mHeadersConfig.mSocketBufferSize);
    
    DEBUGLOG(ConnectionConfigurationLog, "BaseConnectionConfiguration::configure socket receive buffer sizes: video:%u, audio:%u, header:%u", mVideoConfig.mSocketBufferSize, mAudioConfig.mSocketBufferSize, mHeadersConfig.mSocketBufferSize);

}

void BaseConnectionConfiguration::setPipelineSupported( bool pipeline )
{
    if ( mPipelineEnabled && pipeline != mPipelineSupported )
    {
        mTimeout = AseTimeVal::now();
    }

    mPipelineSupported = pipeline;

    if ( mVideoConfig.mPipeline && !pipeline )
    {
        mVideoConfig.mPipeline = false;
        mVideoConfig.mConnections = 1;
    }

    DEBUGLOG(ConnectionConfigurationLog, "BaseConnectionConfiguration::setPipelineSupported: pipeline %s %s, video %s range %s %d connections, audio %s range %s",
                mPipelineEnabled ? "enabled" : "disabled",
                mPipelineSupported ? "supported" : "unsupported",
                mVideoConfig.mOpenRange ? "open" : "closed",
                mVideoConfig.mPipeline ? "pipeline" : "non-pipeline",
                mVideoConfig.mConnections,
                mAudioConfig.mOpenRange ? "open" : "closed",
                mAudioConfig.mPipeline ? "pipeline" : "non-pipeline"  );
}

/** Get the configuration for a media type */
TrackConfiguration const&
BaseConnectionConfiguration::getConfiguration( MediaType mediaType ) const
{
    assert( mediaType == MEDIA_AUDIO || mediaType == MEDIA_VIDEO || mediaType == MEDIA_HEADERS );
    return ( mediaType == MEDIA_AUDIO ) ? mAudioConfig
            : ( mediaType == MEDIA_VIDEO ) ? mVideoConfig
            : mHeadersConfig;
}

AseTimeVal BaseConnectionConfiguration::getNextTimeout()
{
    if ( mTimeout == AseTimeVal::INFINITE && mTimeoutIntervalMs > 0 )
    {
        mTimeout = AseTimeVal::now() + AseTimeVal::fromMS( mTimeoutIntervalMs );
    }

    return mTimeout;
}

void
BaseConnectionConfiguration::updateMaxConnectionsUsingRtts( string const& rttsForMaxConnectionsLimit )
{
    DEBUGLOG(ConnectionConfigurationLog, "updateMaxConnectionsUsingRtts:: %s",
                    rttsForMaxConnectionsLimit.c_str());
    if (rttsForMaxConnectionsLimit == "" )
        return;

    vector<string> rttList;
    StringTokenizer::tokenize( rttsForMaxConnectionsLimit, rttList, ":" );
    uint32_t avgConnectTime = 0;
    uint32_t varConnectTime = 0;
    uint32_t numberOfConnectionFailures = 0;
    ILocationSetMonitorPtr pLocationSetMonitor = mNetworkMonitorPtr->getCurrentLocationSetMonitor();
    if ( pLocationSetMonitor )
        pLocationSetMonitor->connectionStat(avgConnectTime, varConnectTime, numberOfConnectionFailures);
    DEBUGLOG(ConnectionConfigurationLog, "average connect time: %u", avgConnectTime);

    uint32_t targetRtt;
    bool success = false;

    if ( (avgConnectTime > 0) && !rttList.empty() )
    {
        uint32_t i;
        for (i = 0; i < rttList.size(); ++i)
        {
            if ( fromString<uint32_t>(targetRtt, rttList[i], std::dec) )
            {
                success = true;
                DEBUGLOG(ConnectionConfigurationLog, "target RTT: %u ms", targetRtt);
                if ( avgConnectTime < targetRtt )
                {
                    break;
                }
            }
            else
            {
                success = false;
                break;
            }
        }

        uint32_t origMaxConnections = mMaxParallelConnections;
        uint32_t origTargetConnections = mTargetPrimaryConnections;

        if ( success )
        {
            mMaxParallelConnections = min( i+1, origMaxConnections );
            mTargetPrimaryConnections = min( i+1, origTargetConnections );
        }

        // TODO: resume this
//        mStreamingReporterPtr->connectionSelected1(origMaxConnections, mMaxParallelConnections, avgConnectTime);
        DEBUGLOG(ConnectionConfigurationLog,
                "original max number of connetions: %u, actual max number of connections: %u, rtt: %u",
                origMaxConnections, mMaxParallelConnections, avgConnectTime);

    }

}


}}
