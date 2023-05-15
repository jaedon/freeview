/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_LOCATIONSET_MONITOR_H
#define I_LOCATIONSET_MONITOR_H

#include <nrdnet/AseTimeVal.h>
#include <nrdase/AseCommonDataTypes.h>

#include <nrdbase/Variant.h>
#include <nrdbase/Time.h>

#include <nrdase/IThroughputMonitor.h>
#include <nrdase/IThroughputPredictor.h>

#include <string>

namespace netflix {

struct NetworkInterfaceInfo;

namespace ase {

/**
 * @class ILocationSetMonitorPtr
 * @brief: network more for each location set, which contains one or more locations
 */
class ILocationSetMonitor : public IThroughputMonitor
                          , public IThroughputPredictor
{
public:
    typedef shared_ptr<ILocationSetMonitor> ILocationSetMonitorPtr;

public:
    class Listener
    {
    public:
        typedef shared_ptr<Listener const> ListenerPtr;

        virtual void monitorUpdate( std::string const& key,
                                    Variant const& state,
                                    Variant const& params ) const = 0;

        virtual ~Listener() {}
    };

    typedef Listener::ListenerPtr ListenerPtr;

    virtual ~ILocationSetMonitor()
    {
    }

    /* ---------------------------------------------------------------------------------*/
    /*                          IThroughputMonitor                                      */
    /* ---------------------------------------------------------------------------------*/
    /**
     * @param[in] mediaType the media type of the connection
     * @param[in] bytes the number of data bytes received
     * @param[in] drainingSocketReceiveBuffer true if these bytes were deliberately left in
     *                                        the socket buffer for some reason and so may
     *                                        actually have arrived some time ago, but not before
     *                                        requestTime
     * @param[in] earliestTime the earliest time at which these bytes could have been received
     *                         when drainingSocketReceiveBuffer is true
     */
    virtual void indicateBytesReceived( MediaType mediaType,
                                        uint32_t bytes,
                                        bool drainingSocketReceiveBuffer,
                                        AseTimeVal earliestTime ) = 0;

    /**
     * indicate that no bytes are being received either because
     * there are no outstanding requests or because reading from
     * the provided domain has been stopped for rate control reasons
     */
    virtual void indicateReceptionStopped( MediaType mediaType ) = 0;

    /**
     * Add tcp connect info, such as rtt, and ip
     * @param[in] connectTime: tcp connect time
     * @param[in] ipAddr: ip address of the domain
     */
    virtual void addConnectInfo( AseTimeVal const& connectTime,
                                 std::string const& ipAddr ) = 0;


    virtual void addConnectionFailure() = 0;


    /* ---------------------------------------------------------------------------------*/
    /*                          Connection context                                      */
    /* ---------------------------------------------------------------------------------*/

    /**
     * Indicate the network connection information
      * @param[in] netinfo The network connection information
      */
    virtual void setNetworkInterfaceInfo( NetworkInterfaceInfo const& netinfo ) = 0;

    /**
     * set the client external ip address
     */
    virtual void setClientExternalIpAddress( std::string const& ipaddr ) = 0;

    /* ---------------------------------------------------------------------------------*/
    /*                          obtain statistics                                       */
    /* ---------------------------------------------------------------------------------*/

    /** obtain the short-term throughput
     * @param[in] now The current time
     * @return The short-term throughput in kbit/s, or zero if insufficient recent data
     */
    virtual uint32_t getShortTermThroughput( AseTimeVal const& now, uint32_t currentBufferLevel = 30000 ) = 0;

    /**
     * @param[out] avgConnectTime the average response time
     * @param[out] varConnectTime the variance response time
     */
    virtual void connectionStat(uint32_t& avgConnectTime, uint32_t& varConnectTime,
                                uint32_t& numberOfRecentConnectionFailures) = 0;

    virtual std::string getLocationSetKey() = 0;

    virtual std::string getIpAddr() = 0;

    virtual uint32_t getLevel() const = 0;

    /* ---------------------------------------------------------------------------------*/
    /*                          persistence                                             */
    /* ---------------------------------------------------------------------------------*/

    virtual Time getLastObservedTime() const = 0;

     /** purge all samples to force a re-measure, SimpleLocationSetMonitor only */
    virtual void purge() = 0;
    virtual void probeDone() = 0;
    virtual bool eligibleForProbing() const = 0;

    virtual Variant getState() = 0;

    virtual Variant getStateParams() const = 0;

    virtual void setState( Variant const& state ) = 0;

    /* ---------------------------------------------------------------------------------*/
    /*                          qa state listener                                       */
    /* ---------------------------------------------------------------------------------*/
    virtual void setListener( ListenerPtr pListener ) = 0;
};

typedef ILocationSetMonitor::ILocationSetMonitorPtr ILocationSetMonitorPtr;

}}
#endif
