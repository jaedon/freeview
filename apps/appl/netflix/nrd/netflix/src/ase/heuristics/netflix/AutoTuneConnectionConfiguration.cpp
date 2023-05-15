/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/** To use this Connectino Configuration Manager
 * We need to set
 * pipelineEnabled=true;
 * maxPendingBufferPercent > 0
 * autoTuneConnectionsEnabled = true
 * connectionConfigurationTimer > 0
 */

#include "AutoTuneConnectionConfiguration.h"
#include <nrdase/AseCommonUtil.h>
#include <nrdase/ILocationSetMonitor.h>

using namespace std;

namespace netflix {
namespace ase {

AutoTuneConnectionConfiguration::AutoTuneConnectionConfiguration(
                                      IAseConfigParameterPtr pConfigParameter,
                                      IAseConfigParameterPtr pLocalConfigParameter,
                                      IPersistentStorePtr pPersistentStore,
                                      INetworkMonitorPtr pNetworkMonitor,
                                      IStreamingStatisticsPtr pStreamingStatistics,
                                      IStreamingReporterPtr pStreamingReporter)
                            : BaseConnectionConfiguration(IAseConfigParameterPtr(), pLocalConfigParameter, pNetworkMonitor, pStreamingReporter ),
                              mPersistentStorePtr(pPersistentStore),
                              mStreamingStatisticsPtr(pStreamingStatistics),
                              mMinParallelConnections(1),
                              mInternalState(INIT_STATE)
{
    configure( pConfigParameter );
}

void AutoTuneConnectionConfiguration::configure( IAseConfigParameterPtr pConfigParameter )
{
    BaseConnectionConfiguration::configure( pConfigParameter );

    mProbingWatermarkLevel =
        pConfigParameter->getParameterAs<int>("probingWatermarkLevel", 45000);
    mProbingLength =
        pConfigParameter->getParameterAs<int>("probingLength", 60000);
    mImprovementThreshold =
        pConfigParameter->getParameterAs<int>("improvementThresholdForNewSetting", 5);

    string lkgNumConnectionsStr =
        mPersistentStorePtr->loadValue( IPersistentStore::LKG_NUM_CONNECTIONS );
    DEBUGLOG(ConnectionConfigurationLog, "load lkgNumConnections: %s",
                    lkgNumConnectionsStr.c_str());
    mDefaultNumConnections = fromString<uint32_t>( lkgNumConnectionsStr, DEFAULT_MAX_CONNECTIONS );
    DEBUGLOG(ConnectionConfigurationLog, "default: %u, min: %u, max: %u",
            mDefaultNumConnections, mMinParallelConnections, mMaxParallelConnections);
    if ( mDefaultNumConnections < mMinParallelConnections )
        mDefaultNumConnections = mMinParallelConnections;
    if ( mDefaultNumConnections > mMaxParallelConnections )
        mDefaultNumConnections = mMaxParallelConnections;

    mLkgNumConnections = mDefaultNumConnections;
    mLkgBandwidth = 0;


    if ( mPipelineEnabled && (mLkgNumConnections > 1) )
    {
        setVideoConfig(mLkgNumConnections);
    }
    DEBUGLOG(ConnectionConfigurationLog,
            "AutoTuneConnectionConfiguration constructed: "
            "max # of connections: %u, default number of Connections: %u",
            mMaxParallelConnections, mLkgNumConnections);
}

AutoTuneConnectionConfiguration::~AutoTuneConnectionConfiguration()
{
    ostringstream ostream;
    ostream.imbue(std::locale::classic());
    ostream << mLkgNumConnections;
    mPersistentStorePtr->saveValue( IPersistentStore::LKG_NUM_CONNECTIONS, ostream.str() );
    DEBUGLOG(ConnectionConfigurationLog, "saved lkgNumConnections: %s", ostream.str().c_str());
}

void AutoTuneConnectionConfiguration::processTimeout()
{
    switch ( mInternalState )
    {
        case INIT_STATE:
            processInInitState();
            break;
        case PROBING:
            processInProbingState();
            break;
        case SCHEDULED:
            processInScheduledState();
        default:
            break;
    }

    mTimeout = ( mTimeoutIntervalMs > 0 ) ? AseTimeVal::now() + AseTimeVal::fromMS( mTimeoutIntervalMs )
                        : AseTimeVal::INFINITE;

    return;
}

void AutoTuneConnectionConfiguration::processInInitState()
{
    if ( mPipelineEnabled && mPipelineSupported )
    {
        mProbingNumConnections = selectNextProbingNumber(false);
        if ( mProbingNumConnections > 0 )
        {
            DEBUGLOG(ConnectionConfigurationLog, "probing started");
            mInternalState = SCHEDULED;
        }
    }
}

void AutoTuneConnectionConfiguration::processInScheduledState()
{
    if ( !mPipelineEnabled || !mPipelineSupported )
        return;

    if ( mStreamingStatisticsPtr->streamingBufferLevel() <= mProbingWatermarkLevel )
        return;

    AseTimeVal const now = AseTimeVal::now();
    mProbingStartTime = now;
    if ( !mLkgBandwidth )
    {
        ILocationSetMonitorPtr pLocationSetMonitor = mNetworkMonitorPtr->getCurrentLocationSetMonitor();
        if ( pLocationSetMonitor )
            mLkgBandwidth = pLocationSetMonitor->getShortTermThroughput( now );
    }

    setVideoConfig(mProbingNumConnections);
    mInternalState = PROBING;
    DEBUGLOG(ConnectionConfigurationLog,
            "processInScheduledState: connections: %u",
            mVideoConfig.mConnections);
}

void AutoTuneConnectionConfiguration::processInProbingState()
{
    AseTimeVal const now = AseTimeVal::now();
    if ( now <= mProbingStartTime + AseTimeVal::fromMS(mProbingLength) )
    {
        // The player has not probed long enough
        return;
    }

    uint32_t currentBandwidth = 0;
    ILocationSetMonitorPtr pLocationSetMonitor = mNetworkMonitorPtr->getCurrentLocationSetMonitor();
    if ( pLocationSetMonitor )
        currentBandwidth = pLocationSetMonitor->getShortTermThroughput(now);

    bool result;
    DEBUGLOG(ConnectionConfigurationLog,
            "processInProbingState, currentBandwidth: %u, lkgBandwidth: %u"
            "lkgNumConnections: %u, currentConnections: %u",
            currentBandwidth, mLkgBandwidth,
            mLkgNumConnections, mProbingNumConnections);
    if ( currentBandwidth > (100 + mImprovementThreshold) * mLkgBandwidth / 100 )
    {
        mLkgNumConnections = mProbingNumConnections;
        mLkgBandwidth = currentBandwidth;
        result = true;
    }
    else
    {
        result = false;
    }

    mProbingNumConnections = selectNextProbingNumber(result);
    DEBUGLOG(ConnectionConfigurationLog,
            "lkgNumConnections: %u, result: %u, "
            "next probing number: %d",
            mLkgNumConnections, result, mProbingNumConnections);
    if ( mProbingNumConnections > 0 )
    {
        // continue the probing
        setVideoConfig( (uint32_t)mProbingNumConnections );
        mInternalState = SCHEDULED;
    }
    else
    {
        // the probing is completed
        setVideoConfig( mLkgNumConnections );
        mInternalState = COMPLETED;
        mStreamingReporterPtr->connectionSelected2( mDefaultNumConnections,
                                mLkgNumConnections, mMaxParallelConnections);
        DEBUGLOG( ConnectionConfigurationLog,
                "probing completed: default number of connections: %u, lkg number of connections: %u, max number of connections: %u",
                mDefaultNumConnections, mLkgNumConnections, mMaxParallelConnections );
    }
    return;
}

void AutoTuneConnectionConfiguration::setVideoConfig( uint32_t numOfConnections )
{
    if ( numOfConnections > 1 )
    {
        mVideoConfig.mOpenRange = false;
        mVideoConfig.mPipeline = true;
        mVideoConfig.mConnections = numOfConnections;
    }
    else
    {
        mVideoConfig.mOpenRange = true;
        mVideoConfig.mPipeline = false;
        mVideoConfig.mConnections = 1;
    }
    return;
}

void AutoTuneConnectionConfiguration::reportPipeliningInstable()
{
    mInternalState = COMPLETED;
    setVideoConfig(1);
    DEBUGLOG( ConnectionConfigurationLog,
            "reportPipeliningInstable: fall back to single connection");
    return;
}

int32_t AutoTuneConnectionConfiguration::selectNextProbingNumber(bool result)
{
    int32_t probingNumConnectons = -1;

    if ( mInternalState == INIT_STATE )
    {
        if ( mLkgNumConnections < mMaxParallelConnections )
        {
            probingNumConnectons = mLkgNumConnections + 1;
        }
        else if ( mLkgNumConnections > mMinParallelConnections )
        {
            probingNumConnectons = mLkgNumConnections - 1;
        }
    }
    else if ( result )
    {
        // the previous test succeeds
        if ( (mLkgNumConnections > mDefaultNumConnections) &&
                            (mLkgNumConnections < mMaxParallelConnections) )
        {
            probingNumConnectons = mLkgNumConnections + 1;
        }
        if ( (mLkgNumConnections < mDefaultNumConnections) &&
                (mLkgNumConnections > mMinParallelConnections) )
        {
            probingNumConnectons = mLkgNumConnections - 1;
        }
    }
    else
    {
        // the previous test fails
        if ( mLkgNumConnections == mDefaultNumConnections )
        {
            // No previous test has succeeded
            if ( (mProbingNumConnections > (int32_t)mDefaultNumConnections) &&
                    (mDefaultNumConnections > mMinParallelConnections) )
            {
                // The up probing is completed.
                // If the down probing is complerted, the whole probing is over
                probingNumConnectons = mDefaultNumConnections - 1;
            }
            // no further probing is the down probing fails
        }
        else
        {
            // After at least one successful probing, the player terminates
            // at the first failed one.
            // mLkgNumConnections is the selected number of connections,
        }
    }
    return probingNumConnectons;

}

}}

