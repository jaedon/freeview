/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <nrdase/INetworkMonitor.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdase/IPersistentStore.h>
#include <nrdase/ILocationSetMonitorFactory.h>
#include <nrdase/DebugLogging.h>
#include <nrdase/Manifest.h>
#include <nrdbase/NetworkInterfaceInfo.h>

#include <deque>
#include <map>
#include <string>
#include <sstream>

namespace netflix {
namespace ase {

DECLARE_LOGGER( NetworkMonitorLOG );

class INetworkMonitorListener;
class IThroughputPredictor;

typedef shared_ptr<INetworkMonitorListener> INetworkMonitorListenerPtr;
typedef shared_ptr<IThroughputPredictor> IThroughputPredictorPtr;
typedef shared_ptr<IPersistentStore> IPersistentStorePtr;

/**
 * @class NetworkMonitor
 * @brief The NetworkMonitor uses LocationSetMonitor objects to record network history and make predictions
 *
 * Network monitor state = { "#mandatory" : [ "type" ],
 *                           "type" : "integer(0,1)",
 *                           "params" : <location monitor-specific parameters>,
 *                           "monitors" : { "#any" : <location monitor state> }
 *                         }
 */
class NetworkMonitor : public INetworkMonitor
                     , public ILocationSetMonitor::Listener
                     , public enable_shared_from_this<NetworkMonitor>
{
public:
    typedef shared_ptr<NetworkMonitor> NetworkMonitorPtr;

    static uint32_t const           UNKNOWN_LEVEL;

    static NetworkMonitorPtr instance();

    virtual void configure( IAseConfigParameterPtr pConfigParameter,
                            IPersistentStorePtr pPersistentStore );

    virtual ~NetworkMonitor();

    /**
     * INetworkMonitor interfaces
     */
    virtual void switchLocationSet( ILocationSetMonitorPtr pLocationSetMonitor );
    virtual ILocationSetMonitorPtr getCurrentLocationSetMonitor() const;
    virtual void close();
    virtual void purge();
    virtual void getState( Variant* pState);
    virtual void setState( Variant const& state );
    virtual void addServingLocations(std::vector<ManifestServingLocationPtr> const& servingLocations);
    virtual int32_t addServingLocationsSync(std::vector<ManifestServingLocationPtr> const& servingLocations);
    virtual void storeState();
    virtual ILocationSetMonitorPtr getLocationSetMonitor( std::string const& locationSetKey );


    /**
     * obtain the predicted throughput
     * @param[in] preferredLocationSetKey: location set key for the preferred location
     * @param[in] currentBufferLevel: current buffer level in ms
     * @param[out] source: the source of the prediction
     * @param[out] confidence: the confidence of the throughput estimate
     */
    virtual uint32_t getPredictedThroughput(    std::string const&  preferredLocationSetKey,
                                                uint32_t            currentBufferLevel,
                                                PredictionSource&   source,
                                                double&             confidence );

    /**
     * obtain the predicted throughput
     * @param[in] pPreferredLocationSetMonitor: location set monitor for the preferred location
     * @param[in] currentBufferLevel: current buffer level in ms
     * @param[out] source: the source of the prediction
     * @param[out] confidence: the confidence of the throughput estimate
     */
    virtual uint32_t getPredictedThroughput(    ILocationSetMonitorPtr  pPreferredLocationSetMonitor,
                                                uint32_t                currentBufferLevel,
                                                PredictionSource&       source,
                                                double&                 confidence );


    /**
     * set the network interface information
     */
    virtual void networkChange( NetworkInterfaceInfo const& netinfo );

    /**
     * set the client external ip address
     */
    virtual void externalIpAddressChange( std::string const& ipaddr );

    /**
     * set the network monitor listener
     */
    virtual void setNetworkMonitorListener( INetworkMonitorListenerPtr pListener );

    /**
     * ILocationSetMonitor::Listener interfaces
     */
    virtual void monitorUpdate( std::string const& key, Variant const& state, Variant const& params ) const;

protected:
    NetworkMonitor();

    void loadState();

    virtual Variant getStateSinceTime( Time oldest );

    IThroughputPredictorPtr getLevelAggregate( uint32_t level ) const;

    int32_t configureImpl( IAseConfigParameterPtr pAseConfigParameter, IPersistentStorePtr pPersistentStore );
    int32_t networkChangeImpl( NetworkInterfaceInfo const& netinfo );
    int32_t externalIpAddressChangeImpl( std::string const& ipaddr );
    int32_t getStateImpl( Variant* pState );
    int32_t setStateImpl( Variant const& state );
    int32_t setNetworkMonitorListenerImpl( INetworkMonitorListenerPtr pListener );

    class ConfigureCommand;
    class AddServingLocationsCommand;
    class NetworkChangeCommand;
    class ExternalIpAddressChangeCommand;
    class GetStateCommand;
    class SetStateCommand;
    class SetNetworkMonitorListenerCommand;

protected:

    static NetworkMonitorPtr            sNetworkMonitorInstancePtr;

    IPersistentStorePtr                 mPersistentStorePtr;

    IAseConfigParameterPtr              mConfigParameterPtr;

    int                                 mNetworkMonitorType;

    ILocationSetMonitorFactoryPtr       mLocationSetMonitorFactoryPtr;

    ILocationSetMonitorPtr              mCurrentLocationSetMonitorPtr;

    bool                                mHaveNetworkInterfaceInfo;

    NetworkInterfaceInfo          mNetworkInterfaceInfo;

    bool                                mHaveClientExternalIpAddress;

    std::string                         mClientExternalIpAddresss;

    INetworkMonitorListenerPtr          mNetworkMonitorListenerPtr;
};

typedef NetworkMonitor::NetworkMonitorPtr NetworkMonitorPtr;

inline NetworkMonitorPtr NetworkMonitor::instance()
{
    return sNetworkMonitorInstancePtr;
}

}}



#endif
