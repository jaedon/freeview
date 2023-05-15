/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef LOCATION_SET_MONITOR_FACTORY_H
#define LOCATION_SET_MONITOR_FACTORY_H

#include <nrdase/ILocationSetMonitorFactory.h>
#include <nrdase/NetworkMonitor.h>

namespace netflix {
namespace ase {

DECLARE_LOGGER( NetworkMonitorLOG );

template<class _Monitor, class _Aggregate,int _Type>
class LocationSetMonitorFactory : public ILocationSetMonitorFactory
{
public:
    typedef shared_ptr<_Monitor>    _MonitorPtr;
    typedef shared_ptr<_Aggregate>  _AggregatePtr;

public:

    LocationSetMonitorFactory();

    virtual bool configure( IAseConfigParameterPtr pConfigParameter );

    virtual int getType() const { return _Type; }

    virtual ILocationSetMonitorPtr getLocationSetMonitor( std::string const& locationSetKey );

    virtual ILocationSetMonitorPtr getLocationSetMonitor( std::string const& locationSetKey, uint32_t level );

    virtual void getLocationSetMonitors( std::vector<ILocationSetMonitorPtr>& monitors ) const;

    virtual shared_ptr<IThroughputPredictor> getGlobalAggregate();

    virtual shared_ptr<IThroughputPredictor> getLevelAggregate( uint32_t level );

protected:

    _AggregatePtr getLevelAggregateImpl( uint32_t level );

protected:

    typedef std::map<std::string, _MonitorPtr> _Monitors;
    typedef std::map<uint32_t, _AggregatePtr>  _Aggregates;

    IAseConfigParameterPtr  mConfigParameterPtr;

    _Monitors               mLocationSetMonitors;
    _Aggregates             mLevelAggregates;
    _AggregatePtr           mGlobalAggregatePtr;
};

template<class _Monitor, class _Aggregate,int _Type>
LocationSetMonitorFactory<_Monitor,_Aggregate,_Type>::LocationSetMonitorFactory()
{
    DEBUGLOG( NetworkMonitorLOG, "LocationSetMonitorFactory() type %d", _Type );
}

template<class _Monitor, class _Aggregate,int _Type>
bool
LocationSetMonitorFactory<_Monitor,_Aggregate,_Type>::configure( IAseConfigParameterPtr pConfigParameter )
{
    int32_t const locationSetMonitorType = pConfigParameter->getParameterAs<int32_t>( "locationSetMonitorType", 0, 1, 0 );

    if ( locationSetMonitorType != _Type )
    {
        DEBUGLOG( NetworkMonitorLOG, "LocationSetMonitorFactory::configure failed, type=%d, config=%d", _Type, locationSetMonitorType );

        return false;
    }


    mConfigParameterPtr = pConfigParameter;

    if ( !mGlobalAggregatePtr )
    {
        mGlobalAggregatePtr = _Aggregate::create( pConfigParameter, "Global" );
    }
    else
    {
        mGlobalAggregatePtr->configure( pConfigParameter );
    }

    typename _Aggregates::iterator aggregateIt = mLevelAggregates.begin();
    for( ; aggregateIt != mLevelAggregates.end(); ++aggregateIt )
    {
        aggregateIt->second->configure( pConfigParameter );
    }

    typename _Monitors::iterator monitorIt = mLocationSetMonitors.begin();
    for( ; monitorIt != mLocationSetMonitors.end(); ++monitorIt )
    {
        monitorIt->second->configure( pConfigParameter );
    }

    DEBUGLOG( NetworkMonitorLOG, "LocationSetMonitorFactory::configure succeeded %d monitors",
    (int) mLocationSetMonitors.size() );

    return true;
}

template<class _Monitor, class _Aggregate,int _Type>
ILocationSetMonitorPtr
LocationSetMonitorFactory<_Monitor,_Aggregate,_Type>::getLocationSetMonitor( std::string const& key )
{
    return getLocationSetMonitor( key, NetworkMonitor::UNKNOWN_LEVEL );
}

template<class _Monitor, class _Aggregate,int _Type>
ILocationSetMonitorPtr
LocationSetMonitorFactory<_Monitor,_Aggregate,_Type>::getLocationSetMonitor(   std::string const& key,
                                                                                uint32_t level )
{
    _MonitorPtr pLocationSetMonitor;

    typename _Monitors::const_iterator const it = mLocationSetMonitors.find( key );

    if ( it == mLocationSetMonitors.end() )
    {
        DEBUGLOG( NetworkMonitorLOG, "LocationSetMonitorFactory::getLocationSetMonitor creating for %s", key.c_str() );

        pLocationSetMonitor = _Monitor::create(  key, mConfigParameterPtr );
        if ( pLocationSetMonitor )
        {
            mLocationSetMonitors[ key ] = pLocationSetMonitor;

            mGlobalAggregatePtr->add( pLocationSetMonitor );

            if ( level != NetworkMonitor::UNKNOWN_LEVEL )
            {
                getLevelAggregateImpl( level )->add( pLocationSetMonitor );
            }
        }
    }
    else
    {
        pLocationSetMonitor = it->second;

        uint32_t const current_level = pLocationSetMonitor->getLevel();
        if ( current_level != level )
        {
            // Location has moved levels
            if ( current_level != NetworkMonitor::UNKNOWN_LEVEL )
                getLevelAggregateImpl( current_level )->remove( pLocationSetMonitor );

            if ( level != NetworkMonitor::UNKNOWN_LEVEL )
                getLevelAggregateImpl( level )->add( pLocationSetMonitor );
        }
    }

    return pLocationSetMonitor;
}

template<class _Monitor, class _Aggregate,int _Type>
void
LocationSetMonitorFactory<_Monitor,_Aggregate,_Type>::getLocationSetMonitors(
                                        std::vector<ILocationSetMonitorPtr>& monitors ) const
{
    typename _Monitors::const_iterator it = mLocationSetMonitors.begin();
    for( ; it != mLocationSetMonitors.end(); ++it )
    {
        monitors.push_back( it->second );
    }
}

template<class _Monitor, class _Aggregate,int _Type>
shared_ptr<IThroughputPredictor>
LocationSetMonitorFactory<_Monitor,_Aggregate,_Type>::getGlobalAggregate()
{
    return mGlobalAggregatePtr;
}

template<class _Monitor, class _Aggregate,int _Type>
shared_ptr<IThroughputPredictor>
LocationSetMonitorFactory<_Monitor,_Aggregate,_Type>::getLevelAggregate( uint32_t level )
{
    return getLevelAggregateImpl( level );
}

template<class _Monitor, class _Aggregate,int _Type>
typename LocationSetMonitorFactory<_Monitor,_Aggregate,_Type>::_AggregatePtr
LocationSetMonitorFactory<_Monitor,_Aggregate,_Type>::getLevelAggregateImpl( uint32_t level )
{
    _AggregatePtr pAggregate;

    typename _Aggregates::const_iterator it = mLevelAggregates.find( level );
    if ( it == mLevelAggregates.end() )
    {
        std::ostringstream oss;
        oss << "Level " << level;
        pAggregate = _Aggregate::create( mConfigParameterPtr, oss.str() );

        mLevelAggregates[ level ] = pAggregate;
    }
    else
    {
        pAggregate = it->second;
    }

    return pAggregate;
}


}}

#endif
