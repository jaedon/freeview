/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_NETWORK_MONITOR_H
#define I_NETWORK_MONITOR_H

#include <nrdbase/Variant.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/Manifest.h>
#include "ILocationSetMonitor.h"
#include "INetworkMonitorListener.h"

#include <string>

namespace netflix {

struct NetworkInterfaceInfo;

namespace ase {

/**
 * @class INetworkMonitor
 * @brief The interface for network monitoring
 *
 * It is expected that subclasses of this class will be defined
 * implementing specific monitoring algorithms
 */
class INetworkMonitor
{
public:
    typedef shared_ptr<INetworkMonitor> INetworkMonitorPtr;
    typedef shared_ptr<INetworkMonitor const> INetworkMonitorConstPtr;

public:

    enum PredictionSource
    {
        SHORT_TERM = 0,
        LOCATION_HISTORY,
        LEVEL_HISTORY,
        GLOBAL_HISTORY
    };

   /**
     * Notify location set switch
     * TODO: double check whether we need it
     */
    virtual void switchLocationSet( ILocationSetMonitorPtr pLocationSetMonitor ) = 0;

    /**
     * obtain the location set monitor given the provided location set key.
     * Create one if the location set monitor does not exist.
     */
    virtual ILocationSetMonitorPtr getLocationSetMonitor( std::string const& locationSetKey ) = 0;

    virtual ILocationSetMonitorPtr getCurrentLocationSetMonitor() const = 0;

    /**
     * purge all location set monitors
     *
     */
    virtual void purge() = 0;

    virtual void close() = 0;

    virtual void storeState() = 0;

    /**
     * add new serving locations to Network Monitor
     */
    virtual void addServingLocations(std::vector<ManifestServingLocationPtr> const& servingLocations) = 0;
    virtual int32_t addServingLocationsSync(std::vector<ManifestServingLocationPtr> const& servingLocations) = 0;

    /**
     * obtain the predicted throughput
     * @param[in] preferredLocationSetKey: location set key for the preferred location
     * @param[in] currentBufferLevel: current buffer level in ms
     * @param[out] source: the source of the prediction
     * @param[out] confidence: the confidence of the throughput estimate
     */
    virtual uint32_t getPredictedThroughput(    std::string const&  preferredLocationSetKey,
                                                uint32_t currentBufferLevel,
                                                PredictionSource&   source,
                                                double&             confidence) = 0;

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
                                                double&                 confidence) = 0;

    virtual ~INetworkMonitor() {}
};

typedef INetworkMonitor::INetworkMonitorPtr INetworkMonitorPtr;
typedef INetworkMonitor::INetworkMonitorConstPtr INetworkMonitorConstPtr;

}}

#endif
