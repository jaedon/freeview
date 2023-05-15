/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "IpConnectivityManager.h"

#include <iterator>
#include <sstream>
#include <algorithm>

namespace netflix
{


IpConnectivityManager* IpConnectivityManager::mInstance = NULL;

/** Constructor. */
IpConnectivityManager::IpConnectivityManager() : mDnsServers( splitString( DEFAULT_DNS_SERVERS ) ),
                                                 mDnsLookupTimeout( DEFAULT_DNS_LOOKUP_TIMEOUT ),
                                                 mDnsLookupRetryCount( DEFAULT_DNS_LOOKUP_RETRY_COUNT ),
                                                 mDnsCacheEntryLimit( DEFAULT_DNS_CACHE_ENTRY_LIMIT ),
                                                 mDnsCacheTimeout( DEFAULT_DNS_CACHE_TIMEOUT ),
                                                 mDnsMinimumTtl( DEFAULT_DNS_MINIMUM_TTL ),
                                                 mDnsParallelLookupFlag( DEFAULT_DNS_PARALLEL_LOOKUP_FLAG ),
                                                 mDnsLogLookupsFlag( DEFAULT_DNS_LOG_LOOKUPS_FLAG ),
                                                 mDnsLogQueriesFlag( DEFAULT_DNS_LOG_QUERIES_FLAG ),
                                                 mDnsFallbackServers( splitString( DEFAULT_DNS_FALLBACK_SERVERS ) ),
                                                 mDnsFallbackDelay( DEFAULT_DNS_FALLBACK_DELAY ),
                                                 mDnsPrefetchHosts( splitString( DEFAULT_DNS_PREFETCH_HOSTS ) ),
                                                 mDnsConsecutiveFailuresLimit( DEFAULT_DNS_CONSECUTIVE_FAILURES_LIMIT ),
                                                 mSecondaryTcpConnectionDelay( DEFAULT_SECONDARY_TCP_CONNECTION_DELAY ),
                                                 mFastTcpTimeoutDelay( DEFAULT_FAST_TCP_TIMEOUT_DELAY ),
                                                 mFastTcpTimeoutMaxCount( DEFAULT_FAST_TCP_TIMEOUT_MAX_COUNT ),
                                                 mInitialSocketReattemptDelay( DEFAULT_SOCKET_REATTEMPT_INITIAL_DELAY ),
                                                 mMaximumSocketReattemptDelay( DEFAULT_SOCKET_REATTEMPT_MAXIMUM_DELAY ),
                                                 mHttpConnectionTimeout( DEFAULT_HTTP_CONNECTION_TIMEOUT ),
                                                 mHttpStallTimeout( DEFAULT_HTTP_STALL_TIMEOUT ),
                                                 mSocketReceiveBufferSize( DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE ),
                                                 mPreferIpV6Flag( DEFAULT_PREFER_IP_V6_FLAG ),
                                                 mIpConnectivityPolicy( IP_V4_ONLY ),
                                                 mHstNumberOfConnectionPerServer( DEFAULT_HST_NUMBER_CONNECTION_PER_SERVER ),
                                                 mHstHttpPipeliningDepth( DEFAULT_HST_HTTP_PIPELINING_DEPATH ),
                                                 mHstHttpPipeliningSupportThreshold( DEFAULT_HST_HTTP_PIPELINING_SUPPORT_THRESHOLD ),
                                                 mHstHttpConnectBackoffTime( DEFAULT_HST_HTTP_CONNECT_BACKOFF_TIME ),
                                                 mHstHttpConnectFailureThreshold( DEFAULT_HST_HTTP_CONNECT_FAILURE_THRESHOLD ),
                                                 mTrafficShaperSpeed( DEFAULT_TRAFFIC_SHAPER_SPEED ),
                                                 mTrafficShaperMinSleepInterval( DEFAULT_TRAFFIC_SHAPER_MIN_SLEEP_INTERVAL ),
                                                 mTrafficShaperMaxSleepInterval( DEFAULT_TRAFFIC_SHAPER_MAX_SLEEP_INTERVAL ),
                                                 mTrafficShaperSoftCapacityLimit( DEFAULT_TRAFFIC_SHAPER_SOFT_CAPACITY ),
                                                 mTrafficShaperHardCapacityLimit( DEFAULT_TRAFFIC_SHAPER_HARD_CAPACITY )
{
}

/** Destructor. */
IpConnectivityManager::~IpConnectivityManager()
{
}

/** Set the IP connectivity manager instance. */
void IpConnectivityManager::setInstance( IpConnectivityManager* pIpConnectivityManager )
{
    mInstance = pIpConnectivityManager;
}

/** Destroy the IP connectivity manager instance. */
void IpConnectivityManager::destroy()
{
    delete mInstance;

    mInstance = NULL;
}

/** Get the IP connectivity manager instance. */
IpConnectivityManager* IpConnectivityManager::instance()
{
    return mInstance;
}

/** Load the configuration parameters. */
void IpConnectivityManager::load()
{
    return;
}

/** Store the configuration parameters. */
void IpConnectivityManager::store()
{
    return;
}

/** Set the DNS servers' IP addresses. */
void IpConnectivityManager::setDnsServers( const std::vector<std::string>& dnsServers )
{
    mDnsServers = dnsServers;
}

/** Get the DNS servers' IP addresses. */
std::vector<std::string> IpConnectivityManager::dnsServers()
{
    return mDnsServers;
}

/** Set the DNS lookup timeout value, in ms. */
void IpConnectivityManager::setDnsLookupTimeout( uint32_t value )
{
    mDnsLookupTimeout = value;
}

/** Get the DNS lookup timeout value, in ms. */
uint32_t IpConnectivityManager::getDnsLookupTimeout()
{
    return mDnsLookupTimeout;
}

/** Set the DNS lookup retry count. */
void IpConnectivityManager::setDnsLookupRetryCount( uint32_t value )
{
    mDnsLookupRetryCount = value;
}

/** Get the DNS lookup retry count. */
uint32_t IpConnectivityManager::getDnsLookupRetryCount()
{
    return mDnsLookupRetryCount;
}

/** Set the DNS cache maximum number of entries. */
void IpConnectivityManager::setDnsCacheEntryLimit(uint32_t value)
{
    mDnsCacheEntryLimit = value;
}

/** Get the DNS cache maximum number of entries. */
uint32_t IpConnectivityManager::getDnsCacheEntryLimit()
{
    return mDnsCacheEntryLimit;
}

/** Set the DNS cache timeout value, in ms. */
void IpConnectivityManager::setDnsCacheTimeout( uint32_t value )
{
    mDnsCacheTimeout = value;
}

/** Get the DNS cache timeout value, in ms. */
uint32_t IpConnectivityManager::getDnsCacheTimeout()
{
    return mDnsCacheTimeout;
}

/** Set the DNS minimum TTL value, in ms */
void IpConnectivityManager::setDnsMinimumTtl( uint32_t value )
{
    mDnsMinimumTtl = value;
}

/** Get the DNS minimum TTL value, in ms */
uint32_t IpConnectivityManager::getDnsMinimumTtl()
{
    return mDnsMinimumTtl;
}

/** Set the DNS parallel lookup setting */
void IpConnectivityManager::setDnsParallelLookupFlag(bool flag)
{
    mDnsParallelLookupFlag = flag;
}

/** Get the DNS parallel lookup setting */
bool IpConnectivityManager::getDnsParallelLookupFlag()
{
    return mDnsParallelLookupFlag;
}

/** Set the DNS lookup logging setting */
void IpConnectivityManager::setDnsLogLookupsFlag(bool flag)
{
    mDnsLogLookupsFlag = flag;
}

/** Get the DNS lookup logging setting */
bool IpConnectivityManager::getDnsLogLookupsFlag()
{
    return mDnsLogLookupsFlag;
}

/** Set the DNS query logging setting */
void IpConnectivityManager::setDnsLogQueriesFlag(bool flag)
{
    mDnsLogQueriesFlag = flag;
}

/** Get the DNS query logging setting */
bool IpConnectivityManager::getDnsLogQueriesFlag()
{
    return mDnsLogQueriesFlag;
}

/** Set the DNS fallback servers */
void IpConnectivityManager::setDnsFallbackServers(std::vector<std::string> value)
{
    mDnsFallbackServers = value;
}

/** Get the DNS fallback servers */
std::vector<std::string> IpConnectivityManager::dnsFallbackServers()
{
    return mDnsFallbackServers;
}

/** Set the DNS fallback delay, in ms */
void IpConnectivityManager::setDnsFallbackDelay(uint32_t value)
{
    mDnsFallbackDelay = value;
}

/** Get the DNS fallback delay, in ms */
uint32_t IpConnectivityManager::getDnsFallbackDelay()
{
    return mDnsFallbackDelay;
}

/** Set the DNS hosts to prefetch */
void IpConnectivityManager::setDnsPrefetchHosts(std::vector<std::string> value)
{
    mDnsPrefetchHosts = value;
}

/** Get the DNS hosts to prefetch */
std::vector<std::string> IpConnectivityManager::getDnsPrefetchHosts()
{
    return mDnsPrefetchHosts;
}

/** Set the DNS consecutive failures limit */
void IpConnectivityManager::setDnsConsecutiveFailuresLimit(uint32_t value)
{
    mDnsConsecutiveFailuresLimit = value;
}

/** Get the DNS consecutive failures limit */
uint32_t IpConnectivityManager::getDnsConsecutiveFailuresLimit()
{
    return mDnsConsecutiveFailuresLimit;
}

/** Get the secondary TCP connection delay, in ms. */
void IpConnectivityManager::setSecondaryTcpConnectionDelay( uint32_t value )
{
    mSecondaryTcpConnectionDelay = value;
}

/** Get the secondary TCP connection delay, in ms. */
uint32_t IpConnectivityManager::getSecondaryTcpConnectionDelay()
{
    return mSecondaryTcpConnectionDelay;
}

/** Set the secondary TCP connection delay, in ms. */
void IpConnectivityManager::setFastTcpTimeoutDelay( uint32_t value )
{
    mFastTcpTimeoutDelay = value;
}

/** Get the secondary TCP connection delay, in ms. */
uint32_t IpConnectivityManager::getFastTcpTimeoutDelay()
{
    return mFastTcpTimeoutDelay;
}

/** Set the secondary TCP maximum socket count */
void IpConnectivityManager::setFastTcpTimeoutMaxCount( uint32_t value )
{
    mFastTcpTimeoutMaxCount = value;
}

/** Get the secondary TCP maximum socket count */
uint32_t IpConnectivityManager::getFastTcpTimeoutMaxCount()
{
    return mFastTcpTimeoutMaxCount;
}

/** Get the initial socket reattempt delay */
uint32_t IpConnectivityManager::getInitialSocketReattemptDelay()
{
    return mInitialSocketReattemptDelay;
}

/** Set the initial socket reattempt delay */
void IpConnectivityManager::setInitialSocketReattemptDelay( uint32_t value )
{
    mInitialSocketReattemptDelay = value;
}

/** Get the maximum socket reattempt delay */
uint32_t IpConnectivityManager::getMaximumSocketReattemptDelay()
{
    return mMaximumSocketReattemptDelay;
}

/** Set the maximum socket reattempt delay */
void IpConnectivityManager::setMaximumSocketReattemptDelay( uint32_t value )
{
    mMaximumSocketReattemptDelay = value;
}

/** Set the HTTP connection timeout value, in ms. */
void IpConnectivityManager::setHttpConnectionTimeout( uint32_t value )
{
    mHttpConnectionTimeout = value;
}

/** Get the HTTP connection timeout value, in ms. */
uint32_t IpConnectivityManager::getHttpConnectionTimeout()
{
    return mHttpConnectionTimeout;
}

/** Set the HTTP stall timeout value, in ms. */
void IpConnectivityManager::setHttpStallTimeout( uint32_t value )
{
    mHttpStallTimeout = value;
}

/** Get the HTTP stall timeout value, in ms. */
uint32_t IpConnectivityManager::getHttpStallTimeout()
{
    return mHttpStallTimeout;
}

/** Set the socket receive buffer size. */
void IpConnectivityManager::setSocketReceiveBufferSize( uint32_t value )
{
    mSocketReceiveBufferSize = value;
}

/** Get the socket receive buffer size. */
uint32_t IpConnectivityManager::getSocketReceiveBufferSize()
{
    return mSocketReceiveBufferSize;
}

/** Set the prefer IPv6 flag. */
void IpConnectivityManager::setPreferIpV6Flag( bool flag )
{
    mPreferIpV6Flag = flag;
}

/** Get the prefer IPv6 flag. */
bool IpConnectivityManager::getPreferIpV6Flag()
{
    return mPreferIpV6Flag;
}

/** Set the IP connectivity policy. */
void IpConnectivityManager::setIpConnectivityPolicy( IpConnectivityManager::Policy policy )
{
    mIpConnectivityPolicy = policy;
}

/** Get the IP connectivity policy for a destination. */
IpConnectivityManager::Policy IpConnectivityManager::getIpConnectivityPolicy()
{
    return mIpConnectivityPolicy;
}

//Set/Get the number of connection per server.
void IpConnectivityManager::setHstNumberOfConnectionPerServer( uint32_t count )
{
    mHstNumberOfConnectionPerServer = count;
}

uint32_t IpConnectivityManager::getHstNumberOfConnectionPerServer()
{
    return mHstNumberOfConnectionPerServer;
}

//Set/Get the HTTP pipelining depth.
void IpConnectivityManager::setHstHttpPipeliningDepth( uint32_t depth )
{
    mHstHttpPipeliningDepth = depth;
}

uint32_t IpConnectivityManager::getHstHttpPipeliningDepth()
{
    return mHstHttpPipeliningDepth;
}

//Set/Get the HTTP pipelining support threshold.
void IpConnectivityManager::setHstHttpPipeliningSupportThreshold( uint32_t threshold )
{
    mHstHttpPipeliningSupportThreshold = threshold;
}

uint32_t IpConnectivityManager::getHstHttpPipeliningSupportThreshold()
{
    return mHstHttpPipeliningSupportThreshold;
}

//Set/Get the HTTP connect backoff time, in ms.
void IpConnectivityManager::setHstHttpConnectBackoffTime( uint32_t time )
{
    mHstHttpConnectBackoffTime = time;
}

uint32_t IpConnectivityManager::getHstHttpConnectBackoffTime()
{
    return mHstHttpConnectBackoffTime;
}

//Set/Getg the HTTP connect failure threshold.
void IpConnectivityManager::setHstHttpConnectFailureThreshold( uint32_t threshold )
{
    mHstHttpConnectFailureThreshold = threshold;
}

uint32_t IpConnectivityManager::getHstHttpConnectFailureThreshold()
{
    return mHstHttpConnectFailureThreshold;
}

//Set/get the Traffic Shaper speed
void IpConnectivityManager::setTrafficShaperSpeed( uint32_t speed )
{
    mTrafficShaperSpeed = speed;
}

uint32_t IpConnectivityManager::getTrafficShaperSpeed()
{
    return mTrafficShaperSpeed;
}

//Set/get the Traffic Shaper min/max sleep interval
void IpConnectivityManager::setTrafficShaperSleepIntervals( std::pair<uint32_t,uint32_t> sleepIntervals )
{
    mTrafficShaperMinSleepInterval = sleepIntervals.first;
    mTrafficShaperMaxSleepInterval = sleepIntervals.second;
}

std::pair<uint32_t,uint32_t> IpConnectivityManager::getTrafficShaperSleepIntervals()
{
    return std::make_pair( mTrafficShaperMinSleepInterval, mTrafficShaperMaxSleepInterval );
}

//Set get the Traffic Shaper soft/hard capacitity
void IpConnectivityManager::setTrafficShaperCapacities( std::pair<uint32_t,uint32_t> capacities )
{
    mTrafficShaperSoftCapacityLimit = capacities.first;
    mTrafficShaperHardCapacityLimit = capacities.second;
}

std::pair<uint32_t,uint32_t> IpConnectivityManager::getTrafficShaperCapacities()
{
    return std::make_pair( mTrafficShaperSoftCapacityLimit, mTrafficShaperHardCapacityLimit );
}

void IpConnectivityManager::addIpConnectivityListener( IpConnectivityListenerPtr pListener )
{
    std::vector<IpConnectivityListenerPtr>::iterator it = std::find( mIpConnectivityListeners.begin(),
                                                                     mIpConnectivityListeners.end(),
                                                                     pListener );
    if ( it == mIpConnectivityListeners.end() )
    {
        mIpConnectivityListeners.push_back( pListener );
    }
}

void IpConnectivityManager::removeIpConnectivityListener( IpConnectivityListenerPtr pListener )
{
    std::vector<IpConnectivityListenerPtr>::iterator it = std::find( mIpConnectivityListeners.begin(),
                                                                     mIpConnectivityListeners.end(),
                                                                     pListener );
    if ( it != mIpConnectivityListeners.end() )
    {
        mIpConnectivityListeners.erase( it );
    }
}

void IpConnectivityManager::trafficShaperConfigChanged()
{
    std::vector<IpConnectivityListenerPtr>::iterator it = mIpConnectivityListeners.begin();
    for( ; it != mIpConnectivityListeners.end(); ++it )
    {
        (*it)->trafficShaperConfigChanged();
    }
}

std::string IpConnectivityManager::joinStringVector(const std::vector<std::string> & split)
{
    switch (split.size())
    {
    case 0:
        return std::string(";");
    case 1:
        return split[0];
    default:
        std::ostringstream joined;
        std::copy(split.begin(), split.end() - 1, std::ostream_iterator<std::string>(joined, ";"));
        joined << *split.rbegin();
        return joined.str();
    }
}

std::vector<std::string> IpConnectivityManager::splitString(const std::string & joined)
{
    std::vector<std::string> split;
    std::stringstream ss(joined);
    std::string item;
    while (std::getline(ss, item, ';'))
    {
        if (!item.empty())
        {
            split.push_back(item);
        }
    }
    return split;
}

} // namespace netflix::base

