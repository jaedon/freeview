/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef NETWORK_MONITOR_STATE_H
#define NETWORK_MONITOR_STATE_H

#include <string>
#include <nrdase/AseCommonDataTypes.h>
#include <nrdbase/Variant.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdbase/Mutex.h>
#include <nrdase/DebugLogging.h>

namespace netflix {
namespace ase {

DECLARE_LOGGER( NetworkMonitorStateLog );

class INetworkMonitor;
class IPersistentStore;
class INetworkMonitorListener;
class IAseConfigParameter;

/**
 * @class NetworkMonitorState
 * @brief Manages external update and listeners for the network monitor state
 */
class NetworkMonitorState
{
public:
    typedef NetworkMonitorState* NetworkMonitorStatePtr;

    /** Get the singleton instance of this object */
    static NetworkMonitorStatePtr instance();

    /** Initialize the network monitor */
    void configure( shared_ptr<IAseConfigParameter> pConfigParameter );

    /** Get the current network monitor */
    shared_ptr<INetworkMonitor> getNetworkMonitor();

    /** Set the persistent store */
    void setPersistentStore( shared_ptr<IPersistentStore> pPersistentStore );

    // Methods for use by external listeners

    /* Get the state from store or current network monitor
     * @return The current state
     */
    Variant getState();

    /* Set the state into store or into current network monitor
     * @param state The state to set
     * @return true if this was set/stored, false if not (too big, or not store available)
     */
    bool setState( Variant const& state );

    /* Send a state update to listeners - this contains only modified location sets
     * @param state The update to set
     * @return true if this was set/stored, false if not (too big)
     */
    void updateState( Variant& state );

    /* Set the network monitor listener (there can only be one)
     * @param pListener The network monitor listener
     */
    void setNetworkMonitorListener( shared_ptr<INetworkMonitorListener> pListener );

    // Methods for use by NetworkMonitor

    /* Get the state from store
     * @return The current state
     */
    Variant loadState();

    /* Set the state into store
     * @param state The state to set
     * @return true if this was set/stored, false if not (too big)
     */
    bool storeState( Variant const& state );

    /* Set the current network monitor
     * @param pNetmon The current network monitor
     */
    void setNetworkMonitor( shared_ptr<INetworkMonitor> pNetworkMonitor );

protected:

    NetworkMonitorState( );

protected:

    Mutex                         mMutex;

    shared_ptr<IPersistentStore>        mPersistentStorePtr;

    shared_ptr<INetworkMonitor>         mNetworkMonitorPtr;

    shared_ptr<INetworkMonitorListener> mNetworkMonitorListenerPtr;


};

typedef NetworkMonitorState::NetworkMonitorStatePtr NetworkMonitorStatePtr;

}}

#endif // NETWORK_MONITOR_STATE_H
