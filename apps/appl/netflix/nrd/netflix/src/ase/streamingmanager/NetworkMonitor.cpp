/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NetworkMonitor.h"
#include "SimpleLocationSetMonitor.h"
#include "LocationSetMonitor.h"
#include "IThroughputPredictor.h"
#include "LocationSetMonitorAggregate.h"
#include "SimpleLocationSetMonitorAggregate.h"
#include "IStreamingHeuristic.h"
#include "HttpRequestManager.h"

#include <nrdase/AseCommonUtil.h>
#include <nrdbase/NetworkInterfaceInfo.h>

#include <functional>
#include <queue>
#include <limits>

using namespace netflix::ase;
using namespace std;
using namespace netflix;

namespace netflix {
namespace ase {
DEFINE_LOGGER( NetworkMonitorLOG );

class NetworkMonitor::ConfigureCommand : public HttpRequestManager::Command
{
public:
    ConfigureCommand(   IAseConfigParameterPtr  pAseConfigParameter,
                        IPersistentStorePtr     pPersistentStore )
        : mAseConfigParameterPtr( pAseConfigParameter )
        , mPersistentStorePtr( pPersistentStore )
    {}

    int32_t invoke() { return NetworkMonitor::instance()->configureImpl( mAseConfigParameterPtr, mPersistentStorePtr ); }

    virtual char const* name() { return "NetworkMonitor::Configure"; }

protected:
    IAseConfigParameterPtr  mAseConfigParameterPtr;
    IPersistentStorePtr     mPersistentStorePtr;
};

class NetworkMonitor::AddServingLocationsCommand : public HttpRequestManager::Command
{
public:
    AddServingLocationsCommand( std::vector<ManifestServingLocationPtr> const& servingLocations )
        : mServingLocations( servingLocations )
    {}

    int32_t invoke() { return NetworkMonitor::instance()->addServingLocationsSync( mServingLocations ); }

    virtual char const* name() { return "NetworkMonitor::AddServingLocations"; }

protected:
    std::vector<ManifestServingLocationPtr> mServingLocations;
};

class NetworkMonitor::NetworkChangeCommand : public HttpRequestManager::Command
{
    public:
        NetworkChangeCommand(   NetworkInterfaceInfo const&     netinfo )
            : mNetworkInterfaceInfo( netinfo )
        {}

    int32_t invoke() { return NetworkMonitor::instance()->networkChangeImpl( mNetworkInterfaceInfo ); }

    virtual char const* name() { return "NetworkMonitor::NetworkChange"; }

    protected:
        NetworkInterfaceInfo mNetworkInterfaceInfo;
};

class NetworkMonitor::ExternalIpAddressChangeCommand : public HttpRequestManager::Command
{
    public:
        ExternalIpAddressChangeCommand( std::string const&      ipAddr )
            : mExternalIpAddress( ipAddr )
        {}

    int32_t invoke() { return NetworkMonitor::instance()->externalIpAddressChangeImpl( mExternalIpAddress ); }

    virtual char const* name() { return "NetworkMonitor::ExternalIpAddressChange"; }

    protected:
        std::string mExternalIpAddress;
};

class NetworkMonitor::GetStateCommand : public HttpRequestManager::Command
{
    public:
        GetStateCommand(  Variant* pState )
            : mStatePtr( pState )
        {}

    int32_t invoke() { return NetworkMonitor::instance()->getStateImpl( mStatePtr ); }

    virtual char const* name() { return "NetworkMonitor::GetState"; }

    protected:
        Variant* mStatePtr;
};

class NetworkMonitor::SetStateCommand : public HttpRequestManager::Command
{
    public:
        SetStateCommand(  Variant const&          state )
            : mState( state )
        {}

    int32_t invoke() { return NetworkMonitor::instance()->setStateImpl( mState ); }

    virtual char const* name() { return "NetworkMonitor::SetState"; }

    protected:
        Variant mState;
};

class NetworkMonitor::SetNetworkMonitorListenerCommand : public HttpRequestManager::Command
{
    public:
        SetNetworkMonitorListenerCommand(   INetworkMonitorListenerPtr  pListener )
            : mNetworkMonitorListener( pListener )
        {}

    int32_t invoke() { return NetworkMonitor::instance()->setNetworkMonitorListenerImpl( mNetworkMonitorListener ); }

    virtual char const* name() { return "NetworkMonitor::SetNetworkMonitorListener"; }

    protected:
        INetworkMonitorListenerPtr mNetworkMonitorListener;
};

}
}

uint32_t const NetworkMonitor::UNKNOWN_LEVEL = numeric_limits<uint32_t>::max();

NetworkMonitorPtr NetworkMonitor::sNetworkMonitorInstancePtr( new NetworkMonitor );

int32_t NetworkMonitor::configureImpl( IAseConfigParameterPtr pConfigParameter, IPersistentStorePtr pPersistentStore )
{
    DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::configure()" );

    mPersistentStorePtr = pPersistentStore;

    if ( !mLocationSetMonitorFactoryPtr )
    {
        DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::configure() - creating new factory" );

        mLocationSetMonitorFactoryPtr = constructLocationSetMonitorFactory( pConfigParameter );

        mLocationSetMonitorFactoryPtr->configure( pConfigParameter );

        loadState();
    }
    else if ( !mLocationSetMonitorFactoryPtr->configure( pConfigParameter ) )
    {
        DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::configure() - creating new factory" );

        string currentLocationSet = mCurrentLocationSetMonitorPtr
                                        ? mCurrentLocationSetMonitorPtr->getLocationSetKey()
                                        : string();

        Variant state = getStateSinceTime( Time( 0 ) );

        mCurrentLocationSetMonitorPtr.reset();

        mLocationSetMonitorFactoryPtr = constructLocationSetMonitorFactory( pConfigParameter );

        mLocationSetMonitorFactoryPtr->configure( pConfigParameter );

        mNetworkMonitorType = mLocationSetMonitorFactoryPtr->getType();

        setStateImpl( state );

        if ( !currentLocationSet.empty() )
        {
            DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::configure() resetting current location set monitor: %s",
                currentLocationSet.c_str() );

            mCurrentLocationSetMonitorPtr = mLocationSetMonitorFactoryPtr->getLocationSetMonitor( currentLocationSet );
        }
    }
    else
    {
        DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::configure() - reconfigured ok" );
    }

    return 0;
}

void NetworkMonitor::configure(  IAseConfigParameterPtr  pAseConfigParameter,
                                 IPersistentStorePtr     pPersistentStore )
{
    (void) HttpRequestManager::instance()->issueCommandSync( HttpRequestManager::CommandPtr( new ConfigureCommand( pAseConfigParameter, pPersistentStore ) ) );
}

NetworkMonitor::NetworkMonitor()
    : mNetworkMonitorType( 0 )
    , mHaveNetworkInterfaceInfo( false )
    , mHaveClientExternalIpAddress( false )
{
}

int32_t NetworkMonitor::addServingLocationsSync(std::vector<ManifestServingLocationPtr> const& servingLocations )
{
    DEBUGLOG( NetworkMonitorLOG, "addServingLocations:" );

    for ( vector<ManifestServingLocationPtr>::const_iterator iter = servingLocations.begin();
            iter != servingLocations.end();
            ++iter )
    {
        ILocationSetMonitorPtr pLocationSetMonitor = mLocationSetMonitorFactoryPtr->
                                                        getLocationSetMonitor( (*iter)->getLocationKey(), (*iter)->getLevel() );

        if ( pLocationSetMonitor )
        {
            if ( mHaveNetworkInterfaceInfo )
                pLocationSetMonitor->setNetworkInterfaceInfo( mNetworkInterfaceInfo );
            if ( mHaveClientExternalIpAddress )
                pLocationSetMonitor->setClientExternalIpAddress( mClientExternalIpAddresss );

            if ( mNetworkMonitorListenerPtr )
            {
                pLocationSetMonitor->setListener( shared_from_this() );
            }

            // Temporary workaround for MediaPresentation looking for througput before current Location has been set
            if ( !mCurrentLocationSetMonitorPtr )
            {
                mCurrentLocationSetMonitorPtr = pLocationSetMonitor;
            }

#if defined(NRDP_HAS_TRACING)
            double confidence = 0.0;
            uint32_t throughput = pLocationSetMonitor->predictThroughput( IThroughputPredictorPtr(), 0, 0, confidence );

            DEBUGLOG( NetworkMonitorLOG, "...%s at level %d, %ukbit/s, confidence %lf",
                                                                (*iter)->getLocationKey().c_str(),
                                                                (*iter)->getLevel(),
                                                                throughput,
                                                                confidence );
#endif
        }
    }

    return 0;
}

void NetworkMonitor::addServingLocations(std::vector<ManifestServingLocationPtr> const& servingLocations)
{
    (void) HttpRequestManager::instance()->issueCommandSync( HttpRequestManager::CommandPtr( new AddServingLocationsCommand( servingLocations ) ) );
}

NetworkMonitor::~NetworkMonitor()
{
}

ILocationSetMonitorPtr NetworkMonitor::getLocationSetMonitor( std::string const& locationSetKey )
{
    return mLocationSetMonitorFactoryPtr->getLocationSetMonitor( locationSetKey );
}

IThroughputPredictorPtr NetworkMonitor::getLevelAggregate( uint32_t level ) const
{
    return mLocationSetMonitorFactoryPtr->getLevelAggregate( level );
}


void NetworkMonitor::switchLocationSet( ILocationSetMonitorPtr pLocationSetMonitor )
{
    mCurrentLocationSetMonitorPtr = pLocationSetMonitor;
}

shared_ptr<ILocationSetMonitor> NetworkMonitor::getCurrentLocationSetMonitor() const
{
    return mCurrentLocationSetMonitorPtr;
}

uint32_t NetworkMonitor::getPredictedThroughput(    std::string const&  preferredLocationSetKey,
                                                    uint32_t            currentBufferLevel,
                                                    PredictionSource&   source,
                                                    double&             confidence )
{
    return getPredictedThroughput( getLocationSetMonitor( preferredLocationSetKey ), currentBufferLevel, source, confidence );
}

/*
 * 1) If short-term throughput data is available for the preferred location, this is used
 *    short term data is available if data was received in the last 30s for this location
 *    and, either minimumMeasurementTime of reception time has elapsed, or
 *    minimumMeasurementBytes have been received
 * 2) Otherwise, if long-term throughput data is available for the preferred location, this is
 *    combined with current throughput data (if any) and level or global history, according to the
 *    chosen algorithm
 * 3) If no long-term throughput data is available for the preferred location, and the current location
 *    is the same level, then the current location data is used
 * 4) Otherwise, historical data for the level is used
 */

uint32_t NetworkMonitor::getPredictedThroughput(    ILocationSetMonitorPtr  pPreferredLocationSetMonitor,
                                                    uint32_t            currentBufferLevel,
                                                    PredictionSource&       source,
                                                    double&                 confidence )
{
    AseTimeVal const now = AseTimeVal::now();

    // Check for short-term throughput
    int throughput = pPreferredLocationSetMonitor->getShortTermThroughput( now, currentBufferLevel );
    if ( throughput > 0 )
    {
        source = SHORT_TERM;
        confidence = 1.0;
        return throughput;
    }

    int current = 0;
    if ( mCurrentLocationSetMonitorPtr &&
           mCurrentLocationSetMonitorPtr != pPreferredLocationSetMonitor )
    {
        current = mCurrentLocationSetMonitorPtr->getShortTermThroughput( now, currentBufferLevel );
    }

    throughput = pPreferredLocationSetMonitor->predictThroughput( mCurrentLocationSetMonitorPtr, current, currentBufferLevel, confidence );
    if ( throughput > 0 && confidence > 0.1 )
    {
        source = LOCATION_HISTORY;
        return throughput;
    }

    IThroughputPredictorPtr pLevelAggregate = mLocationSetMonitorFactoryPtr->
                                                    getLevelAggregate( pPreferredLocationSetMonitor->getLevel() );
    if ( pLevelAggregate )
    {
        throughput = pLevelAggregate->predictThroughput( mCurrentLocationSetMonitorPtr, current, 0, confidence );
        if ( throughput > 0 && confidence > 0.1 )
        {
            source = LEVEL_HISTORY;
            return throughput;
        }
    }

    source = GLOBAL_HISTORY;
    return mLocationSetMonitorFactoryPtr->
                getGlobalAggregate()->
                    predictThroughput( mCurrentLocationSetMonitorPtr, current, 0, confidence );
}

int32_t NetworkMonitor::networkChangeImpl( NetworkInterfaceInfo const& netinfo )
{
    DEBUGLOG( NetworkMonitorLOG, "setNetworkInterfaceInfo( %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s )",
                netinfo.defaultInterfaceName_.c_str(),
                netinfo.carrier_.c_str(),
                netinfo.countryCode_.c_str(),
                netinfo.networkCode_.c_str(),
                netinfo.networkType_.c_str(),
                netinfo.networkSpec_.c_str(),
                netinfo.ssid_.c_str(),
                netinfo.wirelessChannel_.c_str(),
                netinfo.ipVersionsAvailable_.c_str(),
                netinfo.ipVersionPreferred_.c_str(),
                netinfo.ipVersionPolicy_.c_str(),
                netinfo.additionalInfo_.c_str() );

    mNetworkInterfaceInfo = netinfo;
    mHaveNetworkInterfaceInfo = true;

    vector<ILocationSetMonitorPtr> monitors;
    mLocationSetMonitorFactoryPtr->getLocationSetMonitors( monitors );
    vector<ILocationSetMonitorPtr>::const_iterator it = monitors.begin();
    for( ; it != monitors.end(); ++it )
    {
        (*it)->setNetworkInterfaceInfo( netinfo );
    }

    return 0;
}

void NetworkMonitor::networkChange( NetworkInterfaceInfo const& netinfo )
{
    (void) HttpRequestManager::instance()->issueCommandSync( HttpRequestManager::CommandPtr( new NetworkChangeCommand( netinfo ) ) );
}

int32_t NetworkMonitor::externalIpAddressChangeImpl( std::string const& ipaddr )
{
    DEBUGLOG( NetworkMonitorLOG, "setClientExternalIpAddress( %s )",
                ipaddr.c_str() );


    mClientExternalIpAddresss = ipaddr;
    mHaveClientExternalIpAddress = true;

    vector<ILocationSetMonitorPtr> monitors;
    mLocationSetMonitorFactoryPtr->getLocationSetMonitors( monitors );
    vector<ILocationSetMonitorPtr>::const_iterator it = monitors.begin();
    for( ; it != monitors.end(); ++it )
    {
        (*it)->setClientExternalIpAddress( ipaddr );
    }

    return 0;
}

void NetworkMonitor::externalIpAddressChange( std::string const& ipaddr )
{
    (void) HttpRequestManager::instance()->issueCommandSync( HttpRequestManager::CommandPtr( new ExternalIpAddressChangeCommand( ipaddr ) ) );
}

void NetworkMonitor::purge()
{
    vector<ILocationSetMonitorPtr> monitors;
    mLocationSetMonitorFactoryPtr->getLocationSetMonitors( monitors );
    vector<ILocationSetMonitorPtr>::const_iterator it = monitors.begin();
    for( ; it != monitors.end(); ++it )
    {
        (*it)->purge();
    }
    return;
}

void NetworkMonitor::close()
{
    DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::close()" );

    if ( mLocationSetMonitorFactoryPtr )
    {
        // Save the LocationSets to persistent store
        storeState();

        mLocationSetMonitorFactoryPtr.reset();
    }
}

void NetworkMonitor::loadState()
{
    if ( mPersistentStorePtr )
        setStateImpl( mPersistentStorePtr->loadVariantValue( IPersistentStore::DOMAIN_THROUGHPUT ) );
}

void NetworkMonitor::storeState()
{
    if ( !mPersistentStorePtr || !mLocationSetMonitorFactoryPtr )
        return;

    // Sort locations by age
    DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::storeState()" );

    priority_queue<Time,vector<Time>,std::greater<Time> > locationtimes;
    vector<ILocationSetMonitorPtr> monitors;
    mLocationSetMonitorFactoryPtr->getLocationSetMonitors( monitors );
    vector<ILocationSetMonitorPtr>::const_iterator it = monitors.begin();
    for( ; it != monitors.end(); ++it )
    {
        DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::storeState(): %s, observed at %lld",
                (*it)->name().c_str(),
                (*it)->getLastObservedTime().ms() );
        Time const locationtime = (*it)->getLastObservedTime();
        if ( locationtime != Time() )
        {
            locationtimes.push( locationtime );
        }
    }

    // Remove locations until the result fits in store
    while( !locationtimes.empty() )
    {
        Variant const state = getStateSinceTime( locationtimes.top() );

        AseErrorCode const err = mPersistentStorePtr->saveVariantValue( IPersistentStore::DOMAIN_THROUGHPUT, state );

        DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::storeState(): %s", ( err == AS_NO_ERROR ) ? "ok" : "too large" );

        if ( err == AS_NO_ERROR )
            break;

        locationtimes.pop();
    }
}

Variant NetworkMonitor::getStateSinceTime( Time oldest )
{
    Variant state;
    state["type"] = mNetworkMonitorType;

    Variant monitorsState;
    vector<ILocationSetMonitorPtr> monitors;
    mLocationSetMonitorFactoryPtr->getLocationSetMonitors( monitors );
    vector<ILocationSetMonitorPtr>::const_iterator it = monitors.begin();
    for( ; it != monitors.end(); ++it )
    {
        if ( (*it)->getLastObservedTime() >= oldest )
        {
            Variant const monitor = (*it)->getState();
            DEBUGLOG( NetworkMonitorLOG, "getStateSinceTime: %s: %s", (*it)->getLocationSetKey().c_str(),
                monitor.toJSON().c_str() );
            if ( !monitor.isNull() )
                monitorsState[ (*it)->getLocationSetKey() ] = monitor;
        }
    }

    state["monitors"] = monitorsState;
    if ( !monitors.empty() )
    {
        state["params"] = monitors.front()->getStateParams();
    }

    return state;
}

int32_t NetworkMonitor::getStateImpl( Variant* pState)
{
    *pState = getStateSinceTime( Time( 1 ) );
    return 0;
}

void NetworkMonitor::getState( Variant* pState )
{
    (void) HttpRequestManager::instance()->issueCommandSync( HttpRequestManager::CommandPtr( new GetStateCommand( pState ) ) );
}

int32_t NetworkMonitor::setStateImpl( Variant const& state )
{
    DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::setState: type %d, state %s", mNetworkMonitorType, state.toJSON().c_str() );

    if ( state.isNull() )
        return 0;

    // Ignore state if we have been asked to change monitor type
    if ( state["type"].integer() != mNetworkMonitorType )
        return 0;

    // Get the state for the LocationSetMonitors
    Variant monitors = state["monitors"];

    Variant::StringMapIterator it = monitors.stringMapBegin();
    const Variant::StringMapIterator end = monitors.stringMapEnd();
    for( ; it != end; ++it )
    {
        ILocationSetMonitorPtr pLocationSetMonitor = mLocationSetMonitorFactoryPtr->getLocationSetMonitor( it->first );
        if ( pLocationSetMonitor )
        {
            DEBUGLOG( NetworkMonitorLOG, "NetworkMonitor::setState: %s: %s",
                        pLocationSetMonitor->getLocationSetKey().c_str(),
                        it->second.toJSON().c_str() );

            pLocationSetMonitor->setState( it->second );

            if ( mNetworkMonitorListenerPtr )
            {
                pLocationSetMonitor->setListener( shared_from_this() );
            }
        }
    }

    return 0;
}

void NetworkMonitor::setState( Variant const& state )
{
    (void) HttpRequestManager::instance()->issueCommandSync( HttpRequestManager::CommandPtr( new SetStateCommand( state ) ) );
}

int32_t NetworkMonitor::setNetworkMonitorListenerImpl( shared_ptr<INetworkMonitorListener> pListener )
{
    if ( ( mNetworkMonitorListenerPtr && !pListener )
        || ( !mNetworkMonitorListenerPtr && pListener ) )
    {
        vector<ILocationSetMonitorPtr> monitors;
        mLocationSetMonitorFactoryPtr->getLocationSetMonitors( monitors );
        vector<ILocationSetMonitorPtr>::const_iterator it = monitors.begin();
        for( ; it != monitors.end(); ++it )
        {
            (*it)->setListener( pListener ? ILocationSetMonitor::ListenerPtr( shared_from_this() ) : ILocationSetMonitor::ListenerPtr() );
        }
    }

    mNetworkMonitorListenerPtr = pListener;

    return 0;
}

void NetworkMonitor::setNetworkMonitorListener( INetworkMonitorListenerPtr pListener )
{
    (void) HttpRequestManager::instance()->issueCommandSync( HttpRequestManager::CommandPtr( new SetNetworkMonitorListenerCommand( pListener ) ) );
}

void NetworkMonitor::monitorUpdate( std::string const& key, Variant const& locationstate, Variant const& params ) const
{
    if ( mNetworkMonitorListenerPtr )
    {
        Variant state;
        state["type"] = mNetworkMonitorType;

        Variant monitors;
        monitors[ key ] = locationstate;

        state["monitors"] = monitors;
        state["params"] = params;
        state["update"] = true;

        DEBUGLOG( NetworkMonitorLOG, "monitorUpdate( %s )", state.toJSON().c_str() );

        (*mNetworkMonitorListenerPtr)( state );
    }
}

