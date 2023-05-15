/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Log.h>

#include "NrdIpConnectivityManager.h"
#include "NrdIpConfigurationListener.h"
#include <nrd/NrdApplication.h>


namespace netflix
{

namespace util
{

/** Constructor. */
NrdIpConfigurationListener::NrdIpConfigurationListener(
                                    NrdIpConnectivityManager* pIpConnectivityManager )
                                        : mNrdIpConnectivityManagerPtr( pIpConnectivityManager )
{
}

/** Destructor. */
NrdIpConfigurationListener::~NrdIpConfigurationListener()
{
}

/** The listener callback. */
void NrdIpConfigurationListener::datumChanged( SystemDataKey::Key key )
{
    if (key == SystemDataKey::SYSTEM_FILE)
    {
        //Load the configuration parameters.
        std::map<std::string, std::string> params = nrdApp()->serviceParameters("streaming");

        std::string dnsServers(DEFAULT_DNS_SERVERS);
        uint32_t dnsLookupTimeout = DEFAULT_DNS_LOOKUP_TIMEOUT;
        uint32_t dnsLookupRetryCount = DEFAULT_DNS_LOOKUP_RETRY_COUNT;
        uint32_t dnsCacheEntryLimit = DEFAULT_DNS_CACHE_ENTRY_LIMIT;
        uint32_t dnsCacheTimeout = DEFAULT_DNS_CACHE_TIMEOUT;
        uint32_t dnsMinimumTtl = DEFAULT_DNS_MINIMUM_TTL;
        uint32_t dnsParallelLookupFlag = DEFAULT_DNS_PARALLEL_LOOKUP_FLAG;
        uint32_t dnsLogLookupsFlag = DEFAULT_DNS_LOG_LOOKUPS_FLAG;
        uint32_t dnsLogQueriesFlag = DEFAULT_DNS_LOG_QUERIES_FLAG;
        std::string dnsFallbackServers(DEFAULT_DNS_FALLBACK_SERVERS);
        uint32_t dnsFallbackDelay(DEFAULT_DNS_FALLBACK_DELAY);
        std::string dnsPrefetchHosts(DEFAULT_DNS_PREFETCH_HOSTS);
        uint32_t dnsConsecutiveFailuresLimit = DEFAULT_DNS_CONSECUTIVE_FAILURES_LIMIT;
        uint32_t secondaryTcpConnectionDelay = DEFAULT_SECONDARY_TCP_CONNECTION_DELAY;
        uint32_t fastTcpTimeoutDelay = DEFAULT_FAST_TCP_TIMEOUT_DELAY;
        uint32_t fastTcpTimeoutMaxCount = DEFAULT_FAST_TCP_TIMEOUT_MAX_COUNT;
        uint32_t initialSocketReattemptDelay = DEFAULT_SOCKET_REATTEMPT_INITIAL_DELAY;
        uint32_t maximumSocketReattemptDelay = DEFAULT_SOCKET_REATTEMPT_MAXIMUM_DELAY;
        uint32_t httpConnectionTimeout = DEFAULT_HTTP_CONNECTION_TIMEOUT;
        uint32_t httpStallTimeout = DEFAULT_HTTP_STALL_TIMEOUT;
        uint32_t socketReceiveBufferSize = DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE;
        uint32_t preferIpV6Flag = DEFAULT_PREFER_IP_V6_FLAG;

        uint32_t hstNumberOfConnectionPerServer = DEFAULT_HST_NUMBER_CONNECTION_PER_SERVER;
        uint32_t hstHttpPipeliningDepth = DEFAULT_HST_HTTP_PIPELINING_DEPATH;
        uint32_t hstHttpPipeliningSupportThreshold = DEFAULT_HST_HTTP_PIPELINING_SUPPORT_THRESHOLD;
        uint32_t hstHttpConnectBackoffTime = DEFAULT_HST_HTTP_CONNECT_BACKOFF_TIME;
        uint32_t hstHttpConnectFailureThreshold = DEFAULT_HST_HTTP_CONNECT_FAILURE_THRESHOLD;

        uint32_t trafficShaperSpeed = DEFAULT_TRAFFIC_SHAPER_SPEED;
        uint32_t trafficShaperMinSleepInterval = DEFAULT_TRAFFIC_SHAPER_MIN_SLEEP_INTERVAL;
        uint32_t trafficShaperMaxSleepInterval = DEFAULT_TRAFFIC_SHAPER_MAX_SLEEP_INTERVAL;
        uint32_t trafficShaperSoftCapacityLimit = DEFAULT_TRAFFIC_SHAPER_SOFT_CAPACITY;
        uint32_t trafficShaperHardCapacityLimit = DEFAULT_TRAFFIC_SHAPER_HARD_CAPACITY;

        if (params.find( "dnsServers" ) != params.end())
        {
            dnsServers = params["dnsServers"];
        }

        if (params.find( "dnsLookupTimeout" ) != params.end())
        {
            dnsLookupTimeout = strtoul( params["dnsLookupTimeout"].c_str(), NULL, 10 );
        }

        if (params.find( "dnsLookupRetryCount" ) != params.end())
        {
            dnsLookupRetryCount = strtoul( params["dnsLookupRetryCount"].c_str(), NULL, 10 );
        }

        if (params.find( "dnsCacheEntryLimit" ) != params.end())
        {
            dnsCacheEntryLimit = strtoul( params["dnsCacheEntryLimit"].c_str(), NULL, 10 );
        }

        if (params.find( "dnsCacheTimeout" ) != params.end())
        {
            dnsCacheTimeout = strtoul( params["dnsCacheTimeout"].c_str(), NULL, 10 );
        }

        if (params.find( "dnsMinimumTtl" ) != params.end())
        {
            dnsMinimumTtl = strtoul( params["dnsMinimumTtl"].c_str(), NULL, 10 );
        }

        if (params.find( "dnsParallelLookupFlag" ) != params.end())
        {
            if ((params["dnsParallelLookupFlag"] == "true") || (params["dnsParallelLookupFlag"] == "1"))
            {
                dnsParallelLookupFlag = 1;
            }
            else
            {
                dnsParallelLookupFlag = 0;
            }
        }

        if (params.find( "dnsLogLookupsFlag" ) != params.end())
        {
            if ((params["dnsLogLookupsFlag"] == "true") || (params["dnsLogLookupsFlag"] == "1"))
            {
                dnsLogLookupsFlag = 1;
            }
            else
            {
                dnsLogLookupsFlag = 0;
            }
        }

        if (params.find( "dnsLogQueriesFlag" ) != params.end())
        {
            if ((params["dnsLogQueriesFlag"] == "true") || (params["dnsLogQueriesFlag"] == "1"))
            {
                dnsLogQueriesFlag = 1;
            }
            else
            {
                dnsLogQueriesFlag = 0;
            }
        }

        if (params.find( "dnsFallbackServers" ) != params.end())
        {
            dnsFallbackServers = params["dnsFallbackServers"];
        }

        if (params.find( "dnsFallbackDelay" ) != params.end())
        {
            dnsFallbackDelay = strtoul( params["dnsFallbackDelay"].c_str(), NULL, 10 );
        }

        if (params.find( "dnsPrefetchHosts" ) != params.end())
        {
            dnsPrefetchHosts = params["dnsPrefetchHosts"];
        }

        if (params.find( "dnsConsecutiveFailuresLimit" ) != params.end())
        {
            dnsConsecutiveFailuresLimit = strtoul( params["dnsConsecutiveFailuresLimit"].c_str(), NULL, 10 );
        }

        if (params.find( "secondaryTcpConnectDelay" ) != params.end())
        {
            secondaryTcpConnectionDelay = strtoul( params["secondaryTcpConnectDelay"].c_str(), NULL, 10 );
        }

        if (params.find( "fastTcpTimeoutDelay" ) != params.end())
        {
            fastTcpTimeoutDelay = strtoul( params["fastTcpTimeoutDelay"].c_str(), NULL, 10 );
        }

        if (params.find( "fastTcpTimeoutMaxCount" ) != params.end())
        {
            fastTcpTimeoutMaxCount = strtoul( params["fastTcpTimeoutMaxCount"].c_str(), NULL, 10 );
        }

        if (params.find( "initialSocketReattemptDelay" ) != params.end())
        {
            initialSocketReattemptDelay = strtoul( params["initialSocketReattemptDelay"].c_str(), NULL, 10 );
        }
        if (params.find( "maximumSocketReattemptDelay" ) != params.end())
        {
            maximumSocketReattemptDelay = strtoul( params["maximumSocketReattemptDelay"].c_str(), NULL, 10 );
        }

        if (params.find( "httpConnectTimeout" ) != params.end())
        {
            httpConnectionTimeout = strtoul( params["httpConnectTimeout"].c_str(), NULL, 10 );
        }

        if (params.find( "httpStallTimeout" ) != params.end())
        {
            httpStallTimeout = strtoul( params["httpStallTimeout"].c_str(), NULL, 10 );
        }

        if (params.find( "socketReceiveBufferSize" ) != params.end())
        {
            socketReceiveBufferSize = strtoul( params["socketReceiveBufferSize"].c_str(), NULL, 10 );
        }

        if (params.find( "preferIpV6Flag" ) != params.end())
        {
            if ((params["preferIpV6Flag"] == "true") || (params["preferIpV6Flag"] == "1"))
            {
                preferIpV6Flag = 1;
            }
            else
            {
                preferIpV6Flag = 0;
            }
        }

        if (params.find( "hstNumberOfConnectionPerServer" ) != params.end())
        {
            hstNumberOfConnectionPerServer = strtoul( params["hstNumberOfConnectionPerServer"].c_str(), NULL, 10 );
        }

        if (params.find( "hstHttpPipeliningDepth" ) != params.end())
        {
            hstHttpPipeliningDepth = strtoul( params["hstHttpPipeliningDepth"].c_str(), NULL, 10 );
        }

        if (params.find( "hstHttpPipeliningSupportThreshold" ) != params.end())
        {
            hstHttpPipeliningSupportThreshold = strtoul( params["hstHttpPipeliningSupportThreshold"].c_str(), NULL, 10 );
        }

        if (params.find( "hstHttpConnectBackoffTime" ) != params.end())
        {
            hstHttpConnectBackoffTime = strtoul( params["hstHttpConnectBackoffTime"].c_str(), NULL, 10 );
        }

        if (params.find( "hstHttpConnectFailureThreshold" ) != params.end())
        {
            hstHttpConnectFailureThreshold = strtoul( params["hstHttpConnectFailureThreshold"].c_str(), NULL, 10 );
        }

        if (params.find( "trafficShaperSpeed" ) != params.end())
        {
            trafficShaperSpeed = strtoul( params["trafficShaperSpeed"].c_str(), NULL, 10 );
        }

        if (params.find( "trafficShaperMinSleepInterval" ) != params.end())
        {
            trafficShaperMinSleepInterval = strtoul( params["trafficShaperMinSleepInterval"].c_str(), NULL, 10 );
        }

        if (params.find( "trafficShaperMaxSleepInterval" ) != params.end())
        {
            trafficShaperMaxSleepInterval = strtoul( params["trafficShaperMaxSleepInterval"].c_str(), NULL, 10 );
        }

        if (params.find( "trafficShaperSoftCapacityLimit" ) != params.end())
        {
            trafficShaperSoftCapacityLimit = strtoul( params["trafficShaperSoftCapacityLimit"].c_str(), NULL, 10 );
        }

        if (params.find( "trafficShaperHardCapacityLimit" ) != params.end())
        {
            trafficShaperHardCapacityLimit = strtoul( params["trafficShaperHardCapacityLimit"].c_str(), NULL, 10 );
        }

        NTRACE(
            TRACE_IP_CONNECTIVITY_MANAGER,
            "received the configuration changes: "
            "DnsServers: %s, DnsLookupTimeout: %u, DnsLookupRetryCount: %u, DnsCacheEntryLimit: %u, "
            "DnsCacheTimeout: %u, DnsMinimumTtl: %u, DnsParallelLookupFlag: %u, DnsLogLookupsFlag: %u, "
            "DnsLogQueriesFlag: %u, DnsFallbackServers: %s, DnsFallbackDelay: %u, DnsPrefetchHosts: %s, "
            "DnsConsecutiveFailuresLimit: %u, SecondaryTcpConnectDelay: %u, FastTcpTimeoutDelay: %u, "
            "FastTcpTimeoutMaxCount: %u, InitialSocketReattemptDelay: %u, MaximumSocketReattemptDelay: %u, "
            "HttpConnectTimeout: %u, HttpStallTimeout: %u, "
            "SocketReceiveBufferSize: %u, preferIpV6Flag: %u, HstNumberOfConnectionPerServer: %u, "
            "HstPipeliningDepath: %u,  HstHttpPipeliningSupportThreshold: %u, "
            "HstHttpConnectBackoffTime: %u, HstHttpConnectFailureThreshold: %u, "
            "TrafficShaperSpeed: %u, TrafficShaperMinSleepInterval: %u, TrafficShaperMaxSleepInterval: %u,"
            "TrafficShaperSoftCapacityLimit: %u, TrafficShaperHardCapacityLimit: %u",
            dnsServers.c_str(),
            dnsLookupTimeout,
            dnsLookupRetryCount,
            dnsCacheEntryLimit,
            dnsCacheTimeout,
            dnsMinimumTtl,
            dnsParallelLookupFlag,
            dnsLogLookupsFlag,
            dnsLogQueriesFlag,
            dnsFallbackServers.c_str(),
            dnsFallbackDelay,
            dnsPrefetchHosts.c_str(),
            dnsConsecutiveFailuresLimit,
            secondaryTcpConnectionDelay,
            fastTcpTimeoutDelay,
            fastTcpTimeoutMaxCount,
            initialSocketReattemptDelay,
            maximumSocketReattemptDelay,
            httpConnectionTimeout,
            httpStallTimeout,
            socketReceiveBufferSize,
            preferIpV6Flag,
            hstNumberOfConnectionPerServer,
            hstHttpPipeliningDepth,
            hstHttpPipeliningSupportThreshold,
            hstHttpConnectBackoffTime,
            hstHttpConnectFailureThreshold,
            trafficShaperSpeed,
            trafficShaperMinSleepInterval,
            trafficShaperMaxSleepInterval,
            trafficShaperSoftCapacityLimit,
            trafficShaperHardCapacityLimit );

        std::vector<std::string> dnsServerVector(mNrdIpConnectivityManagerPtr->splitString(dnsServers));
        std::vector<std::string> dnsFallbackServerVector(mNrdIpConnectivityManagerPtr->splitString(dnsFallbackServers));
        std::vector<std::string> dnsPrefetchHostsVector(mNrdIpConnectivityManagerPtr->splitString(dnsPrefetchHosts));

        bool bTrafficShaperChanged =
            ( mNrdIpConnectivityManagerPtr->getTrafficShaperSpeed() != trafficShaperSpeed )
        ||  ( mNrdIpConnectivityManagerPtr->getTrafficShaperSleepIntervals() != std::make_pair( trafficShaperMinSleepInterval, trafficShaperMaxSleepInterval ) )
        ||  ( mNrdIpConnectivityManagerPtr->getTrafficShaperCapacities() != std::make_pair( trafficShaperSoftCapacityLimit, trafficShaperHardCapacityLimit ) );

        mNrdIpConnectivityManagerPtr->lock();

        mNrdIpConnectivityManagerPtr->setDnsServers( dnsServerVector );
        mNrdIpConnectivityManagerPtr->setDnsLookupTimeout( dnsLookupTimeout );
        mNrdIpConnectivityManagerPtr->setDnsLookupRetryCount( dnsLookupRetryCount );
        mNrdIpConnectivityManagerPtr->setDnsCacheEntryLimit( dnsCacheEntryLimit );
        mNrdIpConnectivityManagerPtr->setDnsCacheTimeout( dnsCacheTimeout );
        mNrdIpConnectivityManagerPtr->setDnsMinimumTtl( dnsMinimumTtl );
        mNrdIpConnectivityManagerPtr->setDnsParallelLookupFlag( dnsParallelLookupFlag );
        mNrdIpConnectivityManagerPtr->setDnsLogLookupsFlag( dnsLogLookupsFlag );
        mNrdIpConnectivityManagerPtr->setDnsLogQueriesFlag( dnsLogQueriesFlag );
        mNrdIpConnectivityManagerPtr->setDnsFallbackServers( dnsFallbackServerVector );
        mNrdIpConnectivityManagerPtr->setDnsFallbackDelay( dnsFallbackDelay );
        mNrdIpConnectivityManagerPtr->setDnsPrefetchHosts( dnsPrefetchHostsVector );
        mNrdIpConnectivityManagerPtr->setDnsConsecutiveFailuresLimit( dnsConsecutiveFailuresLimit );
        mNrdIpConnectivityManagerPtr->setSecondaryTcpConnectionDelay( secondaryTcpConnectionDelay );
        mNrdIpConnectivityManagerPtr->setFastTcpTimeoutDelay( fastTcpTimeoutDelay );
        mNrdIpConnectivityManagerPtr->setFastTcpTimeoutMaxCount( fastTcpTimeoutMaxCount );
        mNrdIpConnectivityManagerPtr->setInitialSocketReattemptDelay( initialSocketReattemptDelay );
        mNrdIpConnectivityManagerPtr->setMaximumSocketReattemptDelay( maximumSocketReattemptDelay );
        mNrdIpConnectivityManagerPtr->setHttpConnectionTimeout( httpConnectionTimeout );
        mNrdIpConnectivityManagerPtr->setHttpStallTimeout( httpStallTimeout );
        mNrdIpConnectivityManagerPtr->setSocketReceiveBufferSize( socketReceiveBufferSize );
        mNrdIpConnectivityManagerPtr->setPreferIpV6Flag( preferIpV6Flag == 1 );

        mNrdIpConnectivityManagerPtr->setHstNumberOfConnectionPerServer(
                                                    hstNumberOfConnectionPerServer );
        mNrdIpConnectivityManagerPtr->setHstHttpPipeliningDepth(
                                                    hstHttpPipeliningDepth );
        mNrdIpConnectivityManagerPtr->setHstHttpPipeliningSupportThreshold(
                                                    hstHttpPipeliningSupportThreshold );
        mNrdIpConnectivityManagerPtr->setHstHttpConnectBackoffTime(
                                                    hstHttpConnectBackoffTime );
        mNrdIpConnectivityManagerPtr->setHstHttpConnectFailureThreshold(
                                                    hstHttpConnectFailureThreshold );

        mNrdIpConnectivityManagerPtr->setTrafficShaperSpeed( trafficShaperSpeed );

        mNrdIpConnectivityManagerPtr->setTrafficShaperSleepIntervals( std::make_pair( trafficShaperMinSleepInterval, trafficShaperMaxSleepInterval ) );

        mNrdIpConnectivityManagerPtr->setTrafficShaperCapacities( std::make_pair( trafficShaperSoftCapacityLimit, trafficShaperHardCapacityLimit ) );

        mNrdIpConnectivityManagerPtr->store();

        if ( bTrafficShaperChanged )
            mNrdIpConnectivityManagerPtr->trafficShaperConfigChanged();

        mNrdIpConnectivityManagerPtr->unlock();

        nrdApp()->configureDns();
    }
}

}} // namespace netflix::util

