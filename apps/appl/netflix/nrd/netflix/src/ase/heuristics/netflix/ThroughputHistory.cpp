/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ThroughputHistory.h"
#include "ThroughputDistributionPredictor.h"

#include <nrdase/IAseConfigParameter.h>
#include <nrdbase/NetworkInterfaceInfo.h>

#include <nrdbase/tr1.h>
#include <limits>

using namespace netflix;
using namespace ase;

using namespace std;

namespace netflix {
namespace ase {
DEFINE_LOGGER( ThroughputHistoryLog );
}}

static llong variantAsInt( Variant const& v )
{
    return v.isInteger() ? v.integer() : ((llong) v.dbl() );
}

ThroughputHistory::NetworkConnectionKey
ThroughputHistory::networkConnectionKey( NetworkInterfaceInfo const& netinfo )
{
    ostringstream keytype;
    string keyipaddr;

    keytype << netinfo.networkType_;

    if ( !netinfo.countryCode_.empty() )
    {
        keytype << ';' << netinfo.countryCode_ << ';' << netinfo.networkCode_;
    }
    else if ( !netinfo.ssid_.empty() )
    {
        keytype << ';' << netinfo.ssid_;
    }

    return NetworkConnectionKey( keytype.str() );
}

ThroughputHistory::Params::Params()
    : mTimeOfDayGranularity( 4 )
    , mConnectTimeAlpha( 1.0 - pow( 0.5, 1.0 / 20 ) )
{}

void ThroughputHistory::Params::initialize( IAseConfigParameterPtr pConfigParameters )
{
    int const intmax = std::numeric_limits<int32_t>::max();

    mTimeOfDayGranularity = pConfigParameters->getParameterAs<int32_t>( "historyTimeOfDayGranularity", 0, 12, 4 );

    if ( 24 % mTimeOfDayGranularity != 0 )
    {
        mTimeOfDayGranularity = 0;
    }

    int32_t const connectTimeHalflife = pConfigParameters->getParameterAs<int32_t>( "connectTimeHalflife", 1, intmax, 20 );
    mConnectTimeAlpha = 1.0 - pow( 0.5, 1.0 / connectTimeHalflife );

    mThroughputDistributionParams.initialize( pConfigParameters );

    mConnectionFailureExpirationTime = AseTimeVal::fromMS( pConfigParameters->getParameterAs<uint32_t>("connectionFailureExpirationTime", 1, intmax, 60 * 1000) );
}

ThroughputHistory::ThroughputHistory(    Params const& params,
                                        ThroughputDistributionPredictorPtr pPredictor )
    : mTimeOfDayGranularity( params.mTimeOfDayGranularity )
    , mConnectTimeAlpha( params.mConnectTimeAlpha )
    , mConnectionFailureExpirationTime( params.mConnectionFailureExpirationTime )
    , mPredictor( pPredictor )
    , mHaveCurrentNetworkConnection( false )
{
}

void ThroughputHistory::configure( IAseConfigParameterPtr pConfigParameters )
{
    Params params;
    params.initialize( pConfigParameters );


}

uint32_t ThroughputHistory::predictThroughput(   AseTimeVal const&      now,
                                                ThroughputHistoryPtr   pCurrentHistory,
                                                uint32_t               currentThroughput,
                                                double&                confidence )
{
    if ( !mCurrentNetworkConnectionHistory )
    {
        confidence = 0.0;
        return 0;
    }

    IThroughputDistributionPtr pCurrentDistribution;
    if ( pCurrentHistory )
    {
        pCurrentDistribution = pCurrentHistory->getDistribution( now );
    }

    int const todindex = timeOfDayIndex( now );

    assert( todindex < (int) mCurrentNetworkConnectionHistory->mTimeOfDayHistory.size() );

    double timelessconfidence;

    int const todthroughput = mPredictor->predictThroughput(   *mCurrentNetworkConnectionHistory
                                                ->mTimeOfDayHistory[ todindex ]
                                                    .mThroughputDistributionPtr,
                                                    pCurrentDistribution.get(),
                                                    currentThroughput,
                                                    confidence );

    int throughput = mPredictor->predictThroughput(   *mCurrentNetworkConnectionHistory
                                                ->mTimelessHistory
                                                    .mThroughputDistributionPtr,
                                                    pCurrentDistribution.get(),
                                            currentThroughput,
                                            timelessconfidence );

    if ( confidence < 0.5 * timelessconfidence )
    {
        confidence = timelessconfidence;
    }
    else
    {
        throughput = todthroughput;
    }

    if ( mCurrentNetworkConnectionHistory->mLastConnectionFailureTime
            + mConnectionFailureExpirationTime > now )
    {
        confidence = 0.0;
    }

    return throughput;
}

double ThroughputHistory::getConfidence( AseTimeVal const& now )
{
    if ( !mCurrentNetworkConnectionHistory )
    {
        return 0.0;
    }

    int const todindex = timeOfDayIndex( now );

    assert( todindex < (int) mCurrentNetworkConnectionHistory->mTimeOfDayHistory.size() );

    double const confidence = mPredictor->getConfidence( *mCurrentNetworkConnectionHistory
                                                ->mTimeOfDayHistory[ todindex ]
                                                    .mThroughputDistributionPtr );
    double const timelessconfidence = mPredictor->getConfidence( *mCurrentNetworkConnectionHistory
                                                ->mTimelessHistory
                                                    .mThroughputDistributionPtr );

    return ( confidence < 0.5 * timelessconfidence ) ? timelessconfidence : confidence;

}

ThroughputHistory::HistoryData* ThroughputHistory::getHistoryData( AseTimeVal const& now ) const
{
    if ( !mCurrentNetworkConnectionHistory )
    {
        return 0;
    }

    int const todindex = timeOfDayIndex( now );

    assert( todindex < (int) mCurrentNetworkConnectionHistory->mTimeOfDayHistory.size() );

    double const confidence = mPredictor->getConfidence( *mCurrentNetworkConnectionHistory
                                                ->mTimeOfDayHistory[ todindex ]
                                                    .mThroughputDistributionPtr );
    double const timelessconfidence = mPredictor->getConfidence( *mCurrentNetworkConnectionHistory
                                                ->mTimelessHistory
                                                    .mThroughputDistributionPtr );

    return ( confidence < 0.5 * timelessconfidence )
                ? &mCurrentNetworkConnectionHistory->mTimelessHistory
                : &mCurrentNetworkConnectionHistory->mTimeOfDayHistory[ todindex ];
}

ThroughputHistory::NetworkConnectionHistoryPtr ThroughputHistory::getHistoryForNetwork( NetworkConnectionKey const& key )
{
    ThroughputHistoryMap::iterator it = mHistory.find( key );
    if ( it == mHistory.end() )
    {
        NetworkConnectionHistoryPtr pHistory( new NetworkConnectionHistory( mTimeOfDayGranularity, mThroughputDistributionParams ) );

        if ( pHistory )
        {
            mHistory[ key ] = pHistory;
        }
        return pHistory;
    }
    else
    {
        return it->second;
    }
}

IThroughputDistributionPtr ThroughputHistory::getDistribution( AseTimeVal const& now ) const
{
    HistoryData* const pHistory = getHistoryData( now );

    return pHistory ? (IThroughputDistributionPtr) ( pHistory->mThroughputDistributionPtr ) : IThroughputDistributionPtr();
}

void ThroughputHistory::connectTimeStat( AseTimeVal const& now, uint32_t& avgConnectTime, uint32_t& varConnectTime) const
{
    HistoryData* const pHistory = getHistoryData( now );
    if ( !pHistory )
    {
        avgConnectTime = 0;
        varConnectTime = 0;
    }
    else
    {
        avgConnectTime = pHistory->mAverageConnectTime / 1000;
        varConnectTime = ( pHistory->mAverageSquareConnectTime - pHistory->mAverageConnectTime * pHistory->mAverageConnectTime ) / 1000;
    }
}

void ThroughputHistory::setNetworkInterfaceInfo( NetworkInterfaceInfo const& netinfo )
{
    NetworkConnectionKey const newkey = networkConnectionKey( netinfo );

    DEBUGLOG( ThroughputHistoryLog, "setNetworkInterfaceInfo: newkey=%s, currentkey=%s",
                newkey.c_str(),
                mCurrentNetworkConnectionKey.c_str());

    if ( !mHaveCurrentNetworkConnection || ( newkey != mCurrentNetworkConnectionKey ) )
    {
        mHaveCurrentNetworkConnection = true;

        mCurrentNetworkConnectionKey = newkey;

        mCurrentNetworkConnectionHistory = getHistoryForNetwork( newkey );
    }
}

void ThroughputHistory::setClientExternalIpAddress( std::string const& /*ipaddr*/ )
{
#if 0
    if ( !mCurrentNetworkConnectionKey.second.empty() )
    {
        if ( mCurrentNetworkConnectionKey.second != ipaddr )
        {
            if ( mCurrentNetworkConnectionKey.second != "unknown" )
            {
                NetworkConnectionKey const newkey( mCurrentNetworkConnectionKey.first, ipaddr );

                mCurrentNetworkConnectionKey = newkey;
                mCurrentNetworkConnectionHistory = getHistoryForNetwork( newkey );
            }
            else
            {
                NetworkConnectionKey const oldkey = mCurrentNetworkConnectionKey;
                NetworkConnectionKey const newkey( mCurrentNetworkConnectionKey.first, ipaddr );

                ThroughputHistoryMap::iterator it = mHistory.find( newkey );
                if ( it != mHistory.end() )
                {
                    mCurrentNetworkConnectionKey = newkey;
                    mCurrentNetworkConnectionHistory = it->second;
                }
                else
                {
                    // Rename the 'unknown' entry
                    mHistory[ newkey ] = mCurrentNetworkConnectionHistory;
                }

                // Delete the old 'unknown' entry
                it = mHistory.find( oldkey );
                if ( it != mHistory.end() )
                {
                    mHistory.erase( it );
                }
            }
        }
    }
#endif
}

void ThroughputHistory::throughputMeasurement( uint32_t throughput, AseTimeVal const& now )
{
    if ( !mCurrentNetworkConnectionHistory )
        return;

    mCurrentNetworkConnectionHistory->mTimelessHistory.throughputMeasurement( throughput, now );

    int const todindex = timeOfDayIndex( now );
    assert( todindex < (int) mCurrentNetworkConnectionHistory->mTimeOfDayHistory.size() );

    mCurrentNetworkConnectionHistory->mTimeOfDayHistory[ todindex ].throughputMeasurement( throughput, now );
}

void ThroughputHistory::connectTimeMeasurement( AseTimeVal const& connectTime, AseTimeVal const& now )
{
    if ( !mCurrentNetworkConnectionHistory )
        return;

    mCurrentNetworkConnectionHistory->mTimelessHistory.connectTimeMeasurement( connectTime, mConnectTimeAlpha );

    int const todindex = timeOfDayIndex( now );
    assert( todindex < (int) mCurrentNetworkConnectionHistory->mTimeOfDayHistory.size() );

    mCurrentNetworkConnectionHistory->mTimeOfDayHistory[ todindex ].connectTimeMeasurement( connectTime, mConnectTimeAlpha );
}

void ThroughputHistory::addConnectionFailure( AseTimeVal when )
{
    if ( mCurrentNetworkConnectionHistory )
    {
        mCurrentNetworkConnectionHistory->mLastConnectionFailureTime = when;
    }
}

AseTimeVal ThroughputHistory::getLastHistoryUpdateTime() const
{
    AseTimeVal latest = AseTimeVal::ZERO;

    ThroughputHistoryMap::const_iterator it = mHistory.begin();
    for( ; it != mHistory.end(); ++it )
    {
        if ( it->second->mTimelessHistory.mLastDistributionUpdateTime > latest )
            latest = it->second->mTimelessHistory.mLastDistributionUpdateTime;

        vector<HistoryData>::const_iterator hit = it->second->mTimeOfDayHistory.begin();
        for( ; hit != it->second->mTimeOfDayHistory.end(); ++hit )
            if ( hit->mLastDistributionUpdateTime > latest )
                latest = hit->mLastDistributionUpdateTime;
    }

    return latest;
}

Variant ThroughputHistory::getState( ) const
{
    Variant connections;

    ThroughputHistoryMap::const_iterator it = mHistory.begin();
    for( ; it != mHistory.end(); ++it )
    {
        Variant connection;
        connection["k"] = Variant( it->first );
        connection["h"] = it->second->getState();

        connections.push_back( connection );
    }

    return connections;
}

Variant ThroughputHistory::getStateParams() const
{
    Variant params;

    ThroughputDistributionPredictor::Params const& tdpparams = mPredictor->getParams();

    params["cf"] = tdpparams.mConfidenceFactor; /* confidence factor from ThroughoutDistributionPredictor */
    params["cm"] = tdpparams.mMinimumConfidence;/* minimum confidence from ThroughoutDistributionPredictor */
    params["eca"] = mThroughputDistributionParams.mEffectiveCountAlpha; /* effective count alpha from ThroughputDistribution */

    return params;
}

void ThroughputHistory::setState( Variant const& state )
{
    if ( state.isStringMap() )
    {
        if ( mCurrentNetworkConnectionHistory )
        {
            mCurrentNetworkConnectionHistory->mTimelessHistory.setState( state );
        }
    }
    else if ( state.isArray() )
    {
        for( int i = 0; i < state.size(); ++i )
        {
            NetworkConnectionKey const key( state[ i ]["k"][0].string() );
            NetworkConnectionHistoryPtr pHistory = getHistoryForNetwork( key );
            if ( pHistory )
            {
                pHistory->setState( state[ i ]["h"] );
            }
        }
    }
}

ThroughputHistory::NetworkConnectionHistory::NetworkConnectionHistory(
                        int timeOfDayGranularity,
                        ThroughputDistribution::Params const& tpparams )
    : mTimelessHistory( tpparams )
    , mTimeOfDayHistory()
    , mLastConnectionFailureTime( AseTimeVal::ZERO )
{
    for( int i = 0; i < 24 / timeOfDayGranularity; ++i )
    {
        mTimeOfDayHistory.push_back( ThroughputHistory::HistoryData( tpparams ) );
    }
}

void ThroughputHistory::NetworkConnectionHistory::configure( int timeOfDayGranularity,
                                                                ThroughputDistribution::Params const& tpparams )
{
    if ( (int) mTimeOfDayHistory.size() != timeOfDayGranularity )
    {
        vector<HistoryData> todHistory;

        for( int i = 0; i < 24 / timeOfDayGranularity; ++i )
        {
            todHistory.push_back( mTimeOfDayHistory[ i * mTimeOfDayHistory.size() / timeOfDayGranularity ].clone() );
        }

        mTimeOfDayHistory = todHistory;
    }

    mTimelessHistory.configure( tpparams );

    vector<HistoryData>::iterator it = mTimeOfDayHistory.begin();
    for( ; it != mTimeOfDayHistory.end(); ++it )
    {
        it->configure( tpparams );
    }
}

Variant ThroughputHistory::NetworkConnectionHistory::getState() const
{
    Variant history;
    history["g"] = mTimelessHistory.getState();

    Variant tod;
    std::vector<HistoryData>::const_iterator it = mTimeOfDayHistory.begin();
    for( ; it != mTimeOfDayHistory.end(); ++it )
    {
        tod.push_back( it->getState() );
    }

    history["tod"] = tod;
    history["lft"] = mLastConnectionFailureTime.convertTo<Time>().ms();

    return history;
}

void ThroughputHistory::NetworkConnectionHistory::setState( Variant const& state )
{
    mTimelessHistory.setState( state["g"] );

    Variant const& tod = state["tod"];
    if ( tod.isArray() )
    {
        for( int i = 0; i < (int) mTimeOfDayHistory.size(); ++i )
        {
            mTimeOfDayHistory[ i ].setState( tod[ i * tod.size() / mTimeOfDayHistory.size() ] );
        }
    }

    if ( state.contains( "lft" ) )
        mLastConnectionFailureTime = AseTimeVal::convertFrom( Time::fromMS( variantAsInt( state["lft"] ) ) );
}

ThroughputHistory::HistoryData::HistoryData( ThroughputDistribution::Params const& tpparams )
    : mThroughputDistributionPtr( new ThroughputDistribution( tpparams ) )
    , mLastDistributionUpdateTime( AseTimeVal::ZERO )
    , mAverageConnectTime( 0 )
    , mAverageSquareConnectTime( 0 )
{
}

void ThroughputHistory::HistoryData::configure( ThroughputDistribution::Params const& tpparams )
{
    mThroughputDistributionPtr->configure( tpparams );
}

void ThroughputHistory::HistoryData::throughputMeasurement( uint32_t throughput, AseTimeVal const& now )
{
    mThroughputDistributionPtr->add( throughput, now );
    mLastDistributionUpdateTime = now;
}

void ThroughputHistory::HistoryData::connectTimeMeasurement( AseTimeVal const& connectTime, double alpha )
{
    ullong const connectTimeUs = connectTime.us();
    mAverageConnectTime = static_cast<ullong> (mAverageConnectTime * ( 1.0 - alpha ) + alpha * connectTimeUs);
    mAverageSquareConnectTime = static_cast<ullong> ( mAverageSquareConnectTime * ( 1.0 - alpha ) + alpha * connectTimeUs * connectTimeUs);
}

Variant ThroughputHistory::HistoryData::getState() const
{
    Variant state;

    if ( mLastDistributionUpdateTime != AseTimeVal::ZERO )
    {
        state["t"] = (llong) ( mLastDistributionUpdateTime.convertTo<Time>().ms() );
        state["cn"] = (llong) mAverageConnectTime;
        state["cs"] = (llong) mAverageSquareConnectTime;
        state["d"] = mThroughputDistributionPtr->getState();
    }

    return state;
}

void ThroughputHistory::HistoryData::setState( Variant const& state )
{
    if ( !state.isNull() )
    {
        mLastDistributionUpdateTime = AseTimeVal::convertFrom( Time::fromMS( variantAsInt( state["t"] ) ) );
        mAverageConnectTime = variantAsInt( state["cn"] );
        mAverageSquareConnectTime = variantAsInt( state["cs"] );
        mThroughputDistributionPtr->setState( state["d"] );

        if ( state.contains( "c" ) )
        {
            mThroughputDistributionPtr->setEffectiveCount( variantAsInt( state["c"] ),
                                                 AseTimeVal::convertFrom( Time::fromMS( variantAsInt( state["ct"] ) ) ) );
        }
    }
}

ThroughputHistory::HistoryData ThroughputHistory::HistoryData::clone()
{
    HistoryData data( *this );

    data.mThroughputDistributionPtr.reset( new ThroughputDistribution( *mThroughputDistributionPtr ) );

    return data;
}

void ThroughputHistory::setCurrentNetworkConnectionHistory( NetworkConnectionKey const& key, NetworkConnectionHistoryPtr pCurrentNetworkConnectionHistory )
{
    DEBUGLOG( ThroughputHistoryLog, "setCurrentNetworkConnectionHistory( %s )", key.c_str() );

    mCurrentNetworkConnectionKey = key;
    mCurrentNetworkConnectionHistory = pCurrentNetworkConnectionHistory;
}
