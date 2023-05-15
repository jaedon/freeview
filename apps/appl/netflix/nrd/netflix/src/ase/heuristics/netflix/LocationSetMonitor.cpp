/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/tr1.h>
#include <limits>

#include "LocationSetMonitor.h"
#include "LocationSetMonitorAggregate.h"
#include "NetworkMonitor.h"
#include "ThroughputHistory.h"

#include <nrdbase/Time.h>
#include <nrdbase/NetworkInterfaceInfo.h>

#include <nrdase/AseCommonUtil.h>

using namespace netflix::ase;
using namespace std;
using namespace netflix;

namespace netflix {
namespace ase {
DEFINE_LOGGER( LocationSetMonitorLog );
}}

void LocationSetMonitor::Params::initialize( IAseConfigParameterPtr pConfigParameters )
{
    int const intmax = std::numeric_limits<int32_t>::max();

    mThroughputHalfLives = pConfigParameters->loadConfigMap("throughputMeasurementWindowCurve", "b", "w");

    if ( mThroughputHalfLives.empty() )
    {
        mThroughputHalfLives[ 0 ] = pConfigParameters->getParameterAs<int32_t>( "throughputHalflife", 100, intmax, 5000 );
    }

    mThroughputWarmupTime = AseTimeVal::fromMS( pConfigParameters->getParameterAs<int32_t>( "throughputWarmupTime", 100, intmax, mThroughputHalfLives.begin()->second * 2 ) );
    mMinimumPauseTime = AseTimeVal::fromMS( pConfigParameters->getParameterAs<int32_t>( "minimumPauseTime", 0, intmax, 100 ) );
    mMinimumMeasurementTime = pConfigParameters->getParameterAs<int32_t>( "minimumMeasurementTime", 0, intmax, 1000 );
    mMinimumMeasurementBytes = pConfigParameters->getParameterAs<int32_t>( "minimumMeasurementBytes", 0, intmax, 64 * 1024 );
    mConnectionFailureExpirationTime =
        AseTimeVal::fromMS( pConfigParameters->getParameterAs<int32_t>("connectionFailureExpirationTime", 1, intmax, 60 * 1000) ); // 1 minute


    mShortTermThreshold = AseTimeVal::fromMS( pConfigParameters->getParameterAs<int32_t>( "shortTermThreshold", 1000, intmax, 30000 ) );
    mDistributionUpdateInterval = AseTimeVal::fromMS( pConfigParameters->getParameterAs<int32_t>( "distributionUpdateInterval", 100, intmax, 2000 ) );

    mThroughputCalculationInterval = AseTimeVal::fromMS( pConfigParameters->getParameterAs<int32_t>( "throughputCalculcationInterval", 0, intmax, 250 ) );

    mHistoryPredictionInterval = AseTimeVal::fromMS( pConfigParameters->getParameterAs<int32_t>( "historyPredictionInterval", 0, intmax, 250 ) );

    mLockPeriodAfterProbeComplete = AseTimeVal::fromMS( pConfigParameters->getParameterAs<int32_t>( "lockPeriodAfterProbeComplete", 0, intmax, DEFAULT_LOCK_PERIOD_AFTER_PROBE_COMPLETE) );

    mFastWarmup = pConfigParameters->getParameterAs<bool>( "fastThroughputWarmup", true );
}

LocationSetMonitorPtr LocationSetMonitor::create(   std::string const&              locationSetKey,
                                                    IAseConfigParameterPtr          pConfigParameters )
{
    LocationSetMonitorPtr pLocationSetMonitor;

    do
    {
        Params params;
        params.initialize( pConfigParameters );

        ThroughputHistory::Params tpparams;
        tpparams.initialize( pConfigParameters );

        ThroughputDistributionPredictor::Params tdpparams;
        tdpparams.initialize( pConfigParameters );

        ThroughputDistributionPredictorPtr pPredictor( new ThroughputDistributionPredictor( tdpparams ) );
        if ( !pPredictor ) break;

        ThroughputHistoryPtr pThroughputHistory( new ThroughputHistory( tpparams, pPredictor ) );
        if ( !pThroughputHistory ) break;

        pLocationSetMonitor.reset( new LocationSetMonitor(  locationSetKey,
                                                            pThroughputHistory,
                                                            params ) );
    } while( false );

    return pLocationSetMonitor;
}

LocationSetMonitor::LocationSetMonitor(
        std::string const& locationSetKey,
        ThroughputHistoryPtr pThroughputHistory,
        Params const& params )
    : mLocationSetKey(locationSetKey)
    , mParams( params )
    , mLevel( NetworkMonitor::UNKNOWN_LEVEL )
    , mAudioPaused( true )
    , mVideoPaused( true )
    , mVideoResuming( false )
    , mFirstReceptionTime( AseTimeVal::ZERO )
    , mAdjustedFirstReceptionTime( AseTimeVal::ZERO )
    , mLastReceptionTime( AseTimeVal::ZERO )
    , mReceptionStoppedTime( AseTimeVal::ZERO )
    , mLastHistoryUpdateTime( AseTimeVal::ZERO )
    , mBytesReceived( 0 )
    , mHistory( pThroughputHistory )
    , mThroughputCalculationTime( AseTimeVal::ZERO )
    , mThroughputCalculation( 0 )
    , mHistoryPredictionTime( AseTimeVal::ZERO )
    , mPrevProbeCompleteTime(AseTimeVal::ZERO)

{
    map<uint32_t,uint32_t>::const_iterator it = mParams.mThroughputHalfLives.begin();
    for( ; it != mParams.mThroughputHalfLives.end(); ++it )
    {
        DEBUGLOG( LocationSetMonitorLog, "Throughput half-life %ums for buffer size < %ums%s",
            it->second, it->first,
            params.mFastWarmup ? ", fast warmup" : "" );
        mThroughputs.push_back( make_pair( it->first, AverageThroughput( AseTimeVal::fromMS( it->second ), AseTimeVal::fromMS( 100 ), 0, true, params.mFastWarmup ) ) );
    }
}

void LocationSetMonitor::configure( IAseConfigParameterPtr pConfigParameters )
{
    mParams.initialize( pConfigParameters );

    ThroughputsList::iterator tpit = mThroughputs.begin();
    map<uint32_t,uint32_t>::iterator it = mParams.mThroughputHalfLives.begin();
    for( ; it != mParams.mThroughputHalfLives.end(); ++it )
    {
        if( tpit == mThroughputs.end() )
        {
            mThroughputs.push_back( make_pair( it->first, AverageThroughput( AseTimeVal::fromMS( it->second ), AseTimeVal::fromMS( 100 ), 0, true ) ) );
        }
        else if ( tpit->first < it->first )
        {
            tpit->second.setHalfLife( AseTimeVal::fromMS( it->second ) );
            ++tpit;
        }
        else if ( tpit->first > it->first )
        {
            tpit = mThroughputs.insert( tpit, make_pair( it->first, AverageThroughput( AseTimeVal::fromMS( it->second ), AseTimeVal::fromMS( 100 ), 0, true ) ) );
        }
        else
        {
            ++tpit;
        }
    }

    mHistory->configure( pConfigParameters );
}

void LocationSetMonitor::indicateBytesReceived( MediaType mediaType,
                                                uint32_t bytes,
                                                bool drainingSocketReceiveBuffer,
                                                AseTimeVal earliestTime )
{
    AseTimeVal const now = AseTimeVal::now();

#if 0
    DEBUGLOG( LocationSetMonitorLog, "indicateBytesReceived: %s, %d %u %d %llu",
        mLocationSetKey.c_str(),
        mediaType,
        bytes,
        drainingSocketReceiveBuffer,
        earliestTime.ms() );
#endif
    if ( drainingSocketReceiveBuffer )
    {
        DEBUGLOG( LocationSetMonitorLog, "indicateBytesReceived: %s, draining %d %ubytes %llums ago",
        mLocationSetKey.c_str(),
        mediaType,
        bytes,
        ( now - earliestTime ).ms() );
    }


    if ( mediaType == MEDIA_AUDIO )
    {
        mAudioPaused = false;

        // ignore audio when video is paused
        if ( mVideoPaused ) return;

        if ( !drainingSocketReceiveBuffer ) mBytesReceived += bytes;
    }
    else if ( mediaType != MEDIA_VIDEO )
    {
        // ignore other media when video is paused
        if ( mVideoPaused ) return;
    }

    if ( mFirstReceptionTime == AseTimeVal::ZERO )
    {
        mFirstReceptionTime = now;
        mAdjustedFirstReceptionTime = now;
#if 1
        DEBUGLOG( LocationSetMonitorLog, "indicateBytesReceived: first call: %s, %d %u %d %llu",
        mLocationSetKey.c_str(),
        mediaType,
        bytes,
        drainingSocketReceiveBuffer,
        earliestTime.ms() );
#endif
    }

    if ( mediaType == MEDIA_VIDEO )
    {
        if ( mVideoPaused && mReceptionStoppedTime != AseTimeVal::ZERO )
        {
            AseTimeVal const pause = ( now - mReceptionStoppedTime );
            if ( pause > mParams.mMinimumPauseTime )
            {
                mAdjustedFirstReceptionTime += ( now - mReceptionStoppedTime );
                mVideoResuming = true;
            }

            DEBUGLOG( LocationSetMonitorLog, "indicateBytesReceived: %s: video resumed%s",
                        mLocationSetKey.c_str(),
                        mVideoResuming ? "" : " (ignoring short pause)" );
        }

        mVideoPaused = false;

        mVideoResuming = mVideoResuming && drainingSocketReceiveBuffer;

        if ( !mVideoResuming )
        {
            mBytesReceived += bytes;
            //DEBUGLOG( LocationSetMonitorLog, "%u bytes received", bytes );
        }
        //else
        //{
        //    DEBUGLOG( LocationSetMonitorLog, "%u bytes ignored", bytes );
        //}
    }

    // update short and long term statistics
    bool const draining = drainingSocketReceiveBuffer && (( mediaType == MEDIA_AUDIO ) || mVideoResuming);
    ThroughputsList::iterator it = mThroughputs.begin();
    for( ; it != mThroughputs.end(); ++it )
    {
        if ( draining )
            it->second.bytesReceived( bytes, now, draining );
        else
            it->second.bytesReceivedEarlier( bytes, now, earliestTime );
    }

    if ( ( now - mLastHistoryUpdateTime ) > mParams.mDistributionUpdateInterval )
    {
        uint32_t const throughput = getShortTermThroughput( now, std::numeric_limits<uint32_t>::max() );
        if ( throughput > 0 )
        {
            // This may be a little work - push into a different thread ?
            mHistory->throughputMeasurement( throughput, now );

            mLastHistoryUpdateTime = now;

            DEBUGLOG( LocationSetMonitorLog, "%s: measurement %dkbit/s, state: %s",
                            mLocationSetKey.c_str(),
                            throughput,
                            getState().toJSON().c_str() );

            if ( mListenerPtr )
            {
                mListenerPtr->monitorUpdate( mLocationSetKey, getState(), getStateParams() );
            }
        }
    }

    mLastReceptionTime = now;
}

void LocationSetMonitor::indicateReceptionStopped( MediaType mediaType )
{
    if ( mediaType == MEDIA_AUDIO )
    {
        if ( mAudioPaused ) return;
        mAudioPaused = true;
    }

    if ( mediaType == MEDIA_VIDEO )
    {
        if ( mVideoPaused ) return;
        mVideoPaused = true;

        mReceptionStoppedTime = AseTimeVal::now();

        ThroughputsList::iterator it = mThroughputs.begin();
        for( ; it != mThroughputs.end(); ++it )
        {
            it->second.readingPaused( mReceptionStoppedTime );
        }

        DEBUGLOG( LocationSetMonitorLog, "Video reception paused" );
    }
}

void LocationSetMonitor::addConnectInfo( AseTimeVal const& connectTime, std::string const& ipAddr )
{
    mHistory->connectTimeMeasurement( connectTime, AseTimeVal::now() );

    mIpAddr = ipAddr;
}

void LocationSetMonitor::addConnectionFailure()
{
    AseTimeVal const now = AseTimeVal::now();

    mConnectionFailureTimePoints.push_back( now );

    mHistory->addConnectionFailure( now );
}

uint32_t LocationSetMonitor::predictThroughput( IThroughputPredictorPtr     pCurrentLocation,
                                                uint32_t                    currentThroughput,
                                                uint32_t                    currentBufferLevel,
                                                double&                     confidence )
{
    /** TODO */
    NRDP_UNUSED(currentBufferLevel);
    // All LocationSetMonitors should be of the same type
    // NO RTTI allowed
    //assert( !pCurrentLocation || dynamic_cast<LocationSetMonitor*>( pCurrentLocation.get() ) );

    LocationSetMonitor* const pCurrentLocationSetMonitor = static_cast<LocationSetMonitor*>( pCurrentLocation.get() );

    AseTimeVal const now = AseTimeVal::now();
    if ( now - mHistoryPredictionTime < mParams.mHistoryPredictionInterval )
    {
        confidence = mHistoryConfidence;
        return mHistoryPrediction;
    }

    uint32_t const throughput = mHistory->predictThroughput( now,
                                                             pCurrentLocationSetMonitor ? pCurrentLocationSetMonitor->mHistory : ThroughputHistoryPtr(),
                                                             currentThroughput,
                                                             confidence );

    DEBUGLOG( LocationSetMonitorLog, "%s: throughput prediction %dkbit/s, confidence %lf",
                 name().c_str(),
                 throughput,
                 confidence);

    // The const_casts aren't pretty, but the point is that predictThroughput reads the state of the
    // object, without changing it, and this is just a caching mechanism to avoid repeated computation
    const_cast<LocationSetMonitor*>( this )->mHistoryPredictionTime = now;
    const_cast<LocationSetMonitor*>( this )->mHistoryPrediction = throughput;
    const_cast<LocationSetMonitor*>( this )->mHistoryConfidence = confidence;

    return throughput;
}

double LocationSetMonitor::getConfidence()
{
    AseTimeVal const now = AseTimeVal::now();
    if ( now - mHistoryPredictionTime < mParams.mHistoryPredictionInterval )
    {
        return mHistoryConfidence;
    }

    const_cast<LocationSetMonitor*>( this )->mHistoryConfidence = mHistory->getConfidence( now );

    return mHistoryConfidence;
}

uint32_t LocationSetMonitor::getShortTermThroughput( AseTimeVal const& now, uint32_t currentBufferLevel )
{
    if ( ( mFirstReceptionTime == AseTimeVal::ZERO )
        || ( ( now - mLastReceptionTime ) > mParams.mShortTermThreshold ) )
    {
        DEBUGLOG( LocationSetMonitorLog, "getShortTermThroughput: %s: stale - use history", mLocationSetKey.c_str() );
        return 0;
    }

    uint32_t const elapsed = ( now - mFirstReceptionTime ).ms();
    uint32_t const interval = ( ( mVideoPaused ? mReceptionStoppedTime : now ) - mAdjustedFirstReceptionTime ).ms();

    // If we have not received enough data or not enough time, then use the historic bandwidth
    if ( ( interval == 0 ) || ( elapsed < mParams.mMinimumMeasurementTime && mBytesReceived < mParams.mMinimumMeasurementBytes ) )
    {
        DEBUGLOG( LocationSetMonitorLog, "getShortTermThroughput: %s: not enough data (%ums, %u bytes)- use history", mLocationSetKey.c_str(), elapsed, mBytesReceived );
        return 0;
    }

    if ( now - mThroughputCalculationTime >= mParams.mThroughputCalculationInterval )
    {
        ThroughputsList::iterator it = mThroughputs.begin();
        for( ; it != mThroughputs.end(); ++it )
        {
            if ( it->first >= currentBufferLevel )
                break;
        }

        if ( it == mThroughputs.end() ) --it;

        uint32_t const throughput = it->second.getThroughput( now ) / 1000;

        DEBUGLOG( LocationSetMonitorLog, "getShortTermThroughput: %s: calculated %ukbit/s, buffer %ums, half-life %llums %s",
                 mLocationSetKey.c_str(),
                 throughput,
                 currentBufferLevel,
                 it->second.getHalfLife().ms(),
                 mVideoPaused ? " (stopped)" : "" );

        const_cast<LocationSetMonitor*>( this )->mThroughputCalculationTime = now;
        const_cast<LocationSetMonitor*>( this )->mThroughputCalculation = throughput;
    }

    if ( interval < mParams.mThroughputWarmupTime.ms() )
    {
        uint32_t const throughput = mBytesReceived * 8 / interval;

        DEBUGLOG( LocationSetMonitorLog, "getShortTermThroughput: %s: warmup %ukbit/s over %ums vs %ukbit/s %s",
                 mLocationSetKey.c_str(),
                 throughput, interval,
                 mThroughputCalculation,
                 mVideoPaused ? " (stopped)" : "" );

        if ( throughput > mThroughputCalculation )
        {
            return throughput;
        }
    }
    else
    {
        DEBUGLOG( LocationSetMonitorLog, "getShortTermThroughput: %s: %ukbit/s, %s",
                     mLocationSetKey.c_str(),
                     mThroughputCalculation,
                     mVideoPaused ? " (stopped)" : "" );
    }

    return mThroughputCalculation;

}

void LocationSetMonitor::connectionStat( uint32_t& avgConnectTime, uint32_t& varConnectTime,
                                         uint32_t& numberOfRecentConnectionFailures)
{
    mHistory->connectTimeStat( AseTimeVal::now(), avgConnectTime, varConnectTime );

    AseTimeVal const now = AseTimeVal::now();
    AseTimeVal const failureExpirationTime = mParams.mConnectionFailureExpirationTime;
    while ( !mConnectionFailureTimePoints.empty() )
    {
        AseTimeVal const failureTime = mConnectionFailureTimePoints.front();
        if ( failureTime + failureExpirationTime < now )
        {
            // the failure is too old
            mConnectionFailureTimePoints.pop_front();
        }
        else
        {
            break;
        }
    }

    numberOfRecentConnectionFailures = mConnectionFailureTimePoints.size();
}

void LocationSetMonitor::setNetworkInterfaceInfo( NetworkInterfaceInfo const& netinfo )
{
    mHistory->setNetworkInterfaceInfo( netinfo );
}

void LocationSetMonitor::setClientExternalIpAddress( std::string const& ipaddr )
{
    mHistory->setClientExternalIpAddress( ipaddr );
}

string LocationSetMonitor::getLocationSetKey()
{
    return mLocationSetKey;
}

Time LocationSetMonitor::getLastObservedTime() const
{
    return mLastReceptionTime.convertTo<Time>();
}

void LocationSetMonitor::purge()
{
    mPrevProbeCompleteTime = AseTimeVal::ZERO;
}

void LocationSetMonitor::probeDone()
{
    mPrevProbeCompleteTime = AseTimeVal::now();
}

bool LocationSetMonitor::eligibleForProbing() const
{
    if ( mPrevProbeCompleteTime + mParams.mLockPeriodAfterProbeComplete > AseTimeVal::now() )
    {
        // in the lock period
        return false;
    }
    else
    {
        return true;
    }
}

Variant LocationSetMonitor::getState()
{
    return mHistory->getState();
}

Variant LocationSetMonitor::getStateParams() const
{
    return mHistory->getStateParams();
}

void LocationSetMonitor::setState( Variant const& state )
{
    mHistory->setState( state );

    mLastHistoryUpdateTime = mHistory->getLastHistoryUpdateTime();
    mLastReceptionTime = mLastHistoryUpdateTime;
}

void LocationSetMonitor::setListener( ILocationSetMonitor::ListenerPtr pListener )
{
    mListenerPtr = pListener;
}

std::string const& LocationSetMonitor::name() const
{
    return mLocationSetKey;
}

uint32_t LocationSetMonitor::getLevel() const
{
    return mLevel;
}

std::string LocationSetMonitor::getIpAddr()
{
    return mIpAddr;
}

void LocationSetMonitor::setLevel( uint32_t level )
{
    mLevel = level;
}

IThroughputDistributionPtr LocationSetMonitor::getDistribution( AseTimeVal const& now ) const
{
    return mHistory->getDistribution( now );
}
