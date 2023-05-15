/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NetworkMonitorState.h"

#include "IPersistentStore.h"
#include "INetworkMonitor.h"
#include "INetworkMonitorListener.h"

#include <nrdbase/ScopedMutex.h>

using namespace netflix;
using namespace ase;
using namespace std;

namespace netflix {
namespace ase {
DEFINE_LOGGER( NetworkMonitorStateLog );
}
}

NetworkMonitorStatePtr NetworkMonitorState::instance()
{
    static NetworkMonitorState* pNetworkMonitorState = new NetworkMonitorState();

    return pNetworkMonitorState;
}

void NetworkMonitorState::configure( shared_ptr<IAseConfigParameter> pConfigParameter )
{
    if ( mNetworkMonitorPtr )
        mNetworkMonitorPtr->configure( pConfigParameter );
}

NetworkMonitorState::NetworkMonitorState( )
    : mMutex( NETWORKMONITORSTATE_MUTEX, "NetworkMonitorState" )
{
}

void NetworkMonitorState::setPersistentStore( shared_ptr<IPersistentStore> pPersistentStore )
{
    ScopedMutex mutex( mMutex );

    mPersistentStorePtr = pPersistentStore;
}

shared_ptr<INetworkMonitor> NetworkMonitorState::getNetworkMonitor()
{
    ScopedMutex mutex( mMutex );

    return mNetworkMonitorPtr;
}

Variant NetworkMonitorState::getState()
{
    ScopedMutex mutex( mMutex );

    DEBUGLOG( NetworkMonitorStateLog, "NetworkMonitorState::getState() from %s",
                mNetworkMonitorPtr ? "network monitor" : "store" );

    if ( mNetworkMonitorPtr )
    {
        return mNetworkMonitorPtr->getState();
    }
    else
    {
        return loadState();
    }
}

void NetworkMonitorState::updateState( Variant& state )
{
    ScopedMutex mutex( mMutex );

    DEBUGLOG( NetworkMonitorStateLog, "NetworkMonitorState::updateState( ... ) %slistener",
                mNetworkMonitorListenerPtr ? "" : "no " );

    if ( mNetworkMonitorListenerPtr )
    {
        state["update"] = true;
        (*mNetworkMonitorListenerPtr)( state );
    }
}

bool NetworkMonitorState::setState( Variant const& state )
{
    ScopedMutex mutex( mMutex );

    bool bResult = false;

    DEBUGLOG( NetworkMonitorStateLog, "NetworkMonitorState::setState() to %s",
                mNetworkMonitorPtr ? "network monitor" : "store" );

    if ( mNetworkMonitorPtr )
    {
        mNetworkMonitorPtr->setState( state );

        bResult = true;
    }
    else
    {
        bResult = storeState( state );
    }

    if ( mNetworkMonitorListenerPtr )
    {
        (*mNetworkMonitorListenerPtr)( state );
    }

    return bResult;
}

void NetworkMonitorState::setNetworkMonitorListener( shared_ptr<INetworkMonitorListener> pListener )
{
    ScopedMutex mutex( mMutex );

    DEBUGLOG( NetworkMonitorStateLog, "NetworkMonitorState::setNetworkMonitorListener( %s )",
        pListener ? "<listener>" : "null" );

    mNetworkMonitorListenerPtr = pListener;

    if ( mNetworkMonitorListenerPtr )
    {
        if ( mNetworkMonitorPtr )
        {
            mNetworkMonitorPtr->sendUpdates( true );

            (*mNetworkMonitorListenerPtr)( mNetworkMonitorPtr->getState() );
        }
        else
        {
            (*mNetworkMonitorListenerPtr)( loadState() );
        }
    }
    else
    {
        if ( mNetworkMonitorPtr )
        {
            mNetworkMonitorPtr->sendUpdates( false );
        }
    }
}

Variant NetworkMonitorState::loadState()
{
    if ( mPersistentStorePtr )
    {
        return mPersistentStorePtr->loadVariantValue( IPersistentStore::DOMAIN_THROUGHPUT );
    }
    else
    {
        return Variant();
    }
}

bool NetworkMonitorState::storeState( Variant const& state )
{
    if ( !mPersistentStorePtr )
        return false;

    AseErrorCode err = mPersistentStorePtr->saveVariantValue( IPersistentStore::DOMAIN_THROUGHPUT, state );

    return ( err == AS_NO_ERROR );
}

void NetworkMonitorState::setNetworkMonitor( INetworkMonitorPtr pNetworkMonitor )
{
    ScopedMutex mutex( mMutex );

    DEBUGLOG( NetworkMonitorStateLog, "NetworkMonitorState::setNetworkMonitor( %s )",
        pNetworkMonitor ? "<monitor>" : "null" );

    mNetworkMonitorPtr = pNetworkMonitor;

    if ( mNetworkMonitorPtr && mNetworkMonitorListenerPtr )
    {
        mNetworkMonitorPtr->sendUpdates( true );

        (*mNetworkMonitorListenerPtr)( mNetworkMonitorPtr->getState() );
    }
}
