/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NRD_IP_CONNECTIVITY_MANAGER
#define NRD_IP_CONNECTIVITY_MANAGER

#include <string>

#include <nrdbase/Mutex.h>
#include <nrdnet/IpConnectivityManager.h>

namespace netflix
{

namespace util
{

class NrdIpConnectivityManager : public IpConnectivityManager
{
public:
    /** Constructor. */
    NrdIpConnectivityManager();

    /** Lock the connectivity manager. */
    void lock();

    /** Unlock the connectivity manager. */
    void unlock();

    /** Load the configuration parameters. */
    virtual void load();

    /** Store the configuration parameters. */
    virtual void store();

    /** Get the DNS servers */
    virtual std::vector<std::string> dnsServers();

    /** Get the DNS lookup timeout value, in ms. */
    virtual uint32_t getDnsLookupTimeout();

    /** Get the DNS lookup retry count. */
    virtual uint32_t getDnsLookupRetryCount();

    /** Get the DNS cache maximum number of entries. */
    virtual uint32_t getDnsCacheEntryLimit();

    /** Get the DNS cache timeout value, in ms. */
    virtual uint32_t getDnsCacheTimeout();

    /** Get the DNS minimum TTL value, in ms */
    virtual uint32_t getDnsMinimumTtl();

    /** Get the DNS parallel lookup setting */
    virtual bool getDnsParallelLookupFlag();

    /** Get the DNS lookup logging setting */
    virtual bool getDnsLogLookupsFlag();

    /** Get the DNS query logging setting */
    virtual bool getDnsLogQueriesFlag();

    /** Get the DNS fallback servers */
    virtual std::vector<std::string> dnsFallbackServers();

    /** Get the DNS fallback delay, in ms */
    virtual uint32_t getDnsFallbackDelay();

    /** Get the DNS hosts to prefetch */
    virtual std::vector<std::string> getDnsPrefetchHosts();

    /** Get the DNS consecutive failures limit */
    virtual uint32_t getDnsConsecutiveFailuresLimit();

    /** Get the secondary TCP connection delay, in ms. */
    virtual uint32_t getSecondaryTcpConnectionDelay();

    /** Get the fast TCP timeout delay, in ms. */
    virtual uint32_t getFastTcpTimeoutDelay();

    /** Get the fast TCP timeout delay, in ms. */
    virtual uint32_t getFastTcpTimeoutMaxCount();

    /** Get the initial socket reattempt delay */
    virtual uint32_t getInitialSocketReattemptDelay();

    /** Get the maximum socket reattempt delay */
    virtual uint32_t getMaximumSocketReattemptDelay();

    /** Get the HTTP connection timeout value, in ms. */
    virtual uint32_t getHttpConnectionTimeout();

    /** Get the HTTP stall timeout value, in ms. */
    virtual uint32_t getHttpStallTimeout();

    /** Get the socket receive buffer size. */
    virtual uint32_t getSocketReceiveBufferSize();

    /** Get the IP connectivity policy for a destination. */
    virtual Policy getIpConnectivityPolicy();

    //Get the number of connection per server.
    virtual uint32_t getHstNumberOfConnectionPerServer();

    //Get the HTTP pipelining depth.
    virtual uint32_t getHstHttpPipeliningDepth();

    //Get the HTTP pipelining support threshold.
    virtual uint32_t getHstHttpPipeliningSupportThreshold();

    //Get the HTTP connect backoff time, in ms.
    virtual uint32_t getHstHttpConnectBackoffTime();

    //Get the HTTP connect failure threshold.
    virtual uint32_t getHstHttpConnectFailureThreshold();

    //Get the Traffic Shaper speed
    virtual uint32_t getTrafficShaperSpeed();

    //Get the Traffic Shaper min/max sleep interval
    virtual std::pair<uint32_t,uint32_t> getTrafficShaperSleepIntervals();

    //Get the Traffic Shaper soft/hard capacitity
    virtual std::pair<uint32_t,uint32_t> getTrafficShaperCapacities();

    //Add/remove IpConnectivityListeners
    virtual void addIpConnectivityListener( IpConnectivityListenerPtr pListener );
    virtual void removeIpConnectivityListener( IpConnectivityListenerPtr pListener );

    //Signal a change in traffic shaper configuration
    virtual void trafficShaperConfigChanged();

protected:
    /** Destructor. */
    ~NrdIpConnectivityManager();

protected:
    /** The instance. */
    static IpConnectivityManager* mInstance;

    /** The protection mutex. */
    Mutex mMutex;
};

}} // namespace netflix::util

#endif

