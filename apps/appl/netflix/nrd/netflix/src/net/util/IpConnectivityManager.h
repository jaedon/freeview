/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IP_CONNECTIVITY_MANAGER
#define IP_CONNECTIVITY_MANAGER

#include <string>
#include <vector>

#include <nrdnet/NetConfiguration.h>
#include <nrdbase/Mutex.h>

namespace netflix
{


#define DEFAULT_DNS_SERVERS                     ""
#define DEFAULT_DNS_LOOKUP_TIMEOUT              1000
#define DEFAULT_DNS_LOOKUP_RETRY_COUNT          4
#define DEFAULT_DNS_CACHE_ENTRY_LIMIT           16
#define DEFAULT_DNS_CACHE_TIMEOUT               300000
#define DEFAULT_DNS_MINIMUM_TTL                 20000
#define DEFAULT_DNS_PARALLEL_LOOKUP_FLAG        false
#define DEFAULT_DNS_LOG_LOOKUPS_FLAG            false
#define DEFAULT_DNS_LOG_QUERIES_FLAG            false
#define DEFAULT_DNS_FALLBACK_SERVERS            "8.8.8.8"
#define DEFAULT_DNS_FALLBACK_DELAY              2000
#define DEFAULT_DNS_PREFETCH_HOSTS              "appboot.netflix.com;nrdp.nccp.netflix.com;api-global.netflix.com;secure.netflix.com"
#define DEFAULT_DNS_CONSECUTIVE_FAILURES_LIMIT  3
#define DEFAULT_SECONDARY_TCP_CONNECTION_DELAY  200
#define DEFAULT_FAST_TCP_TIMEOUT_DELAY          500
#define DEFAULT_FAST_TCP_TIMEOUT_MAX_COUNT      0
#define DEFAULT_SOCKET_REATTEMPT_INITIAL_DELAY  100
#define DEFAULT_SOCKET_REATTEMPT_MAXIMUM_DELAY  1000
#define DEFAULT_HTTP_CONNECTION_TIMEOUT         8000
#define DEFAULT_HTTP_STALL_TIMEOUT              8000
#define DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE      0
#define DEFAULT_PREFER_IP_V6_FLAG               false

#define DEFAULT_HST_NUMBER_CONNECTION_PER_SERVER      3
#define DEFAULT_HST_HTTP_PIPELINING_DEPATH            10
#define DEFAULT_HST_HTTP_PIPELINING_SUPPORT_THRESHOLD 6
#define DEFAULT_HST_HTTP_CONNECT_BACKOFF_TIME         60
#define DEFAULT_HST_HTTP_CONNECT_FAILURE_THRESHOLD    5

#define DEFAULT_TRAFFIC_SHAPER_SPEED                ((NetConfiguration::getTrafficShaperSpeed()*8) / 1024)
#define DEFAULT_TRAFFIC_SHAPER_MIN_SLEEP_INTERVAL   NetConfiguration::getTrafficShaperMinSleepInterval()
#define DEFAULT_TRAFFIC_SHAPER_MAX_SLEEP_INTERVAL   NetConfiguration::getTrafficShaperMaxSleepInterval()
#define DEFAULT_TRAFFIC_SHAPER_SOFT_CAPACITY        NetConfiguration::getTrafficShaperSoftCapacity()
#define DEFAULT_TRAFFIC_SHAPER_HARD_CAPACITY        NetConfiguration::getTrafficShaperHardCapacity()

class IpConnectivityListener
{
public:
    /** Inform the listener that the traffic shaper configuration has changed
     *
     *  It's safe to read from IpConnectivityManager using base class methods
     *  during this call.
     */
    virtual void trafficShaperConfigChanged() = 0;

    virtual ~IpConnectivityListener() {}
};

typedef shared_ptr<IpConnectivityListener> IpConnectivityListenerPtr;

class IpConnectivityManager
{
public:
    /** The IP connectivity policy. */
    enum Policy
    {
        IP_V4_ONLY = 0, //IPv4 only.
        IP_V6_ONLY,     //IPv6 only.
        IP_V4_V6,       //IP dual stack with preference on IPv4.
        IP_V6_V4        //IP dual stack with preference on IPv6.
    };

public:
    /** Constructor. */
    IpConnectivityManager();

    /** Destructor. */
    virtual ~IpConnectivityManager();

    /** Set the IP connectivity manager instance. */
    static void setInstance( IpConnectivityManager* pIpConnectivityManager );

    /** Destroy the IP connectivity manager instance. */
    static void destroy();

    /** Get the IP connectivity manager instance. */
    static IpConnectivityManager* instance();

    /** Load the configuration parameters. */
    virtual void load();

    /** Store the configuration parameters. */
    virtual void store();

    /** Set the DNS servers' IP addresses. */
    virtual void setDnsServers( const std::vector<std::string>& dnsServers );

    /** Get the DNS servers' IP addresses. */
    virtual std::vector<std::string> dnsServers();

    /** Set the DNS lookup timeout value, in ms. */
    virtual void setDnsLookupTimeout( uint32_t value );

    /** Get the DNS lookup timeout value, in ms. */
    virtual uint32_t getDnsLookupTimeout();

    /** Set the DNS lookup retry count. */
    virtual void setDnsLookupRetryCount( uint32_t value );

    /** Get the DNS lookup retry count. */
    virtual uint32_t getDnsLookupRetryCount();

    /** Set the DNS cache maximum number of entries. */
    virtual void setDnsCacheEntryLimit(uint32_t value);

    /** Get the DNS cache maximum number of entries. */
    virtual uint32_t getDnsCacheEntryLimit();

    /** Set the DNS cache timeout value, in ms. */
    virtual void setDnsCacheTimeout( uint32_t value );

    /** Get the DNS cache timeout value, in ms. */
    virtual uint32_t getDnsCacheTimeout();

    /** Set the DNS minimum TTL value, in ms. */
    virtual void setDnsMinimumTtl( uint32_t value );

    /** Get the DNS minimum TTL value, in ms */
    virtual uint32_t getDnsMinimumTtl();

    /** Set the DNS parallel lookup setting */
    virtual void setDnsParallelLookupFlag(bool flag);

    /** Get the DNS parallel lookup setting */
    virtual bool getDnsParallelLookupFlag();

    /** Set the DNS lookup logging setting */
    virtual void setDnsLogLookupsFlag(bool flag);

    /** Get the DNS lookup logging setting */
    virtual bool getDnsLogLookupsFlag();

    /** Set the DNS query logging setting */
    virtual void setDnsLogQueriesFlag(bool flag);

    /** Get the DNS query logging setting */
    virtual bool getDnsLogQueriesFlag();

    /** Set the DNS fallback servers */
    virtual void setDnsFallbackServers(std::vector<std::string> value);

    /** Get the DNS fallback servers */
    virtual std::vector<std::string> dnsFallbackServers();

    /** Set the DNS fallback delay, in ms */
    virtual void setDnsFallbackDelay(uint32_t value);

    /** Get the DNS fallback delay, in ms */
    virtual uint32_t getDnsFallbackDelay();

    /** Set the DNS hosts to prefetch */
    virtual void setDnsPrefetchHosts(std::vector<std::string> value);

    /** Get the DNS hosts to prefetch */
    virtual std::vector<std::string> getDnsPrefetchHosts();

    /** Set the DNS consecutive failures limit */
    virtual void setDnsConsecutiveFailuresLimit(uint32_t value);

    /** Get the DNS consecutive failures limit */
    virtual uint32_t getDnsConsecutiveFailuresLimit();

    /** Set the secondary TCP connection delay, in ms. */
    virtual void setSecondaryTcpConnectionDelay( uint32_t value );

    /** Get the secondary TCP connection delay, in ms. */
    virtual uint32_t getSecondaryTcpConnectionDelay();

    /** Set the fast TCP timeout delay value, in ms. */
    virtual void setFastTcpTimeoutDelay( uint32_t value );

    /** Get the fast TCP timeout delay value, in ms. */
    virtual uint32_t getFastTcpTimeoutDelay();

    /** Set the fast TCP timeout maximum socket count */
    virtual void setFastTcpTimeoutMaxCount( uint32_t value );

    /** Get the fast TCP timeout maximum socket count */
    virtual uint32_t getFastTcpTimeoutMaxCount();

    /** Get the initial socket reattempt delay */
    virtual uint32_t getInitialSocketReattemptDelay();

    /** Set the initial socket reattempt delay */
    virtual void setInitialSocketReattemptDelay( uint32_t value );

    /** Get the maximum socket reattempt delay */
    virtual uint32_t getMaximumSocketReattemptDelay();

    /** Set the maximum socket reattempt delay */
    virtual void setMaximumSocketReattemptDelay( uint32_t value );

    /** Set the HTTP connection timeout value, in ms. */
    virtual void setHttpConnectionTimeout( uint32_t value );

    /** Get the HTTP connection timeout value, in ms. */
    virtual uint32_t getHttpConnectionTimeout();

    /** Set the HTTP stall timeout value, in ms. */
    virtual void setHttpStallTimeout( uint32_t value );

    /** Get the HTTP stall timeout value, in ms. */
    virtual uint32_t getHttpStallTimeout();

    /** Set the socket receive buffer size. */
    virtual void setSocketReceiveBufferSize( uint32_t value );

    /** Get the socket receive buffer size. */
    virtual uint32_t getSocketReceiveBufferSize();

    /** Set the prefer IPv6 flag. */
    virtual void setPreferIpV6Flag( bool flag );

    /** Get the prefer IPv6 flag. */
    virtual bool getPreferIpV6Flag();

    /** Set the IP connectivity policy. */
    virtual void setIpConnectivityPolicy( Policy policy );

    /** Get the IP connectivity policy for a destination. */
    virtual Policy getIpConnectivityPolicy();

    //Set/Get the number of connection per server.
    virtual void setHstNumberOfConnectionPerServer( uint32_t count );
    virtual uint32_t getHstNumberOfConnectionPerServer();

    //Set/Get the HTTP pipelining depth.
    virtual void setHstHttpPipeliningDepth( uint32_t depth );
    virtual uint32_t getHstHttpPipeliningDepth();

    //Set/Get the HTTP pipelining support threshold.
    virtual void setHstHttpPipeliningSupportThreshold( uint32_t threshold );
    virtual uint32_t getHstHttpPipeliningSupportThreshold();

    //Set/Get the HTTP connect backoff time, in ms.
    virtual void setHstHttpConnectBackoffTime( uint32_t time );
    virtual uint32_t getHstHttpConnectBackoffTime();

    //Set/Get the HTTP connect failure threshold.
    virtual void setHstHttpConnectFailureThreshold( uint32_t threshold );
    virtual uint32_t getHstHttpConnectFailureThreshold();

    //Set/get the Traffic Shaper speed
    virtual void setTrafficShaperSpeed( uint32_t speed );
    virtual uint32_t getTrafficShaperSpeed();

    //Set/get the Traffic Shaper min/max sleep interval
    virtual void setTrafficShaperSleepIntervals( std::pair<uint32_t,uint32_t> sleepIntervals );
    virtual std::pair<uint32_t,uint32_t> getTrafficShaperSleepIntervals();

    //Set get the Traffic Shaper soft/hard capacitity
    virtual void setTrafficShaperCapacities( std::pair<uint32_t,uint32_t> capacities );
    virtual std::pair<uint32_t,uint32_t> getTrafficShaperCapacities();

    //Add/remove IpConnectivityListeners
    virtual void addIpConnectivityListener( IpConnectivityListenerPtr pListener );
    virtual void removeIpConnectivityListener( IpConnectivityListenerPtr pListener );

    //Signal a change in traffic shaper configuration
    virtual void trafficShaperConfigChanged();

    /** Utility functions to convert to/from strings and vectors of strings */
    std::string joinStringVector(const std::vector<std::string>& split);
    std::vector<std::string> splitString(const std::string & joined);

protected:
    /** The instance. */
    static IpConnectivityManager* mInstance;

    //HTTP/TCP/IP related configuration parameters.
    std::vector<std::string> mDnsServers;
    uint32_t mDnsLookupTimeout;
    uint32_t mDnsLookupRetryCount;
    uint32_t mDnsCacheEntryLimit;
    uint32_t mDnsCacheTimeout;
    uint32_t mDnsMinimumTtl;
    bool mDnsParallelLookupFlag;
    bool mDnsLogLookupsFlag;
    bool mDnsLogQueriesFlag;
    std::vector<std::string> mDnsFallbackServers;
    uint32_t mDnsFallbackDelay;
    std::vector<std::string> mDnsPrefetchHosts;
    uint32_t mDnsConsecutiveFailuresLimit;
    uint32_t mSecondaryTcpConnectionDelay;
    uint32_t mFastTcpTimeoutDelay;
    uint32_t mFastTcpTimeoutMaxCount;
    uint32_t mInitialSocketReattemptDelay;
    uint32_t mMaximumSocketReattemptDelay;
    uint32_t mHttpConnectionTimeout;
    uint32_t mHttpStallTimeout;
    uint32_t mSocketReceiveBufferSize;

    /** The flag indicates if IPv6 is preferred.
     *
     *  1 - IPv6 preferred.
     *  0 - IPv4 preferred.
     */
    bool mPreferIpV6Flag;

    Policy mIpConnectivityPolicy;

    //HST related configuration parameters.
    uint32_t mHstNumberOfConnectionPerServer;
    uint32_t mHstHttpPipeliningDepth;
    uint32_t mHstHttpPipeliningSupportThreshold;
    uint32_t mHstHttpConnectBackoffTime;
    uint32_t mHstHttpConnectFailureThreshold;

    //Traffic shaper configuration parameters
    uint32_t mTrafficShaperSpeed;
    uint32_t mTrafficShaperMinSleepInterval;
    uint32_t mTrafficShaperMaxSleepInterval;
    uint32_t mTrafficShaperSoftCapacityLimit;
    uint32_t mTrafficShaperHardCapacityLimit;

    //IpConnectivityListeners
    std::vector<IpConnectivityListenerPtr> mIpConnectivityListeners;
};

} // namespace netflix::base

#endif

