/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>

#include "NrdApplication.h"
#include "NrdIpConnectivityManager.h"

using namespace std;

namespace netflix
{

namespace util
{
/** Constructor. */
NrdIpConnectivityManager::NrdIpConnectivityManager() :
                                mMutex(
                                    UNTRACKED_MUTEX,
                                    "NrdIpConnectivityManager" )
{
}

/** Lock the connectivity manager. */
void NrdIpConnectivityManager::lock()
{
    mMutex.lock();
}

/** Unlock the connectivity manager. */
void NrdIpConnectivityManager::unlock()
{
    mMutex.unlock();
}

/** Load the configuration parameters. */
void NrdIpConnectivityManager::load()
{
    string configurationValue;

    configurationValue = nrdApp()->readSystemConfiguration( SystemKeys::IpConnectivityParas );


    size_t beginPos = 0;

    size_t delimPos;

    while (beginPos < configurationValue.size())
    {
        string keyValue;

        delimPos = configurationValue.find( ",",  beginPos );

        if (delimPos == beginPos)
        {
            beginPos++;

            continue;
        }

        if (delimPos != string::npos)
        {
            keyValue = configurationValue.substr( beginPos, delimPos - beginPos );
        }
        else
        {
            keyValue = configurationValue.substr( beginPos, string::npos );
        }

        //Parse key value pair.
        string key;
        string value;
        uint32_t uintValue;
        size_t colonPos;

        colonPos = keyValue.find( ":", 0 );

        if ((colonPos != string::npos)
            && (colonPos > 0)
            && (colonPos < (keyValue.size() - 1)))
        {
            key = keyValue.substr( 0, colonPos );
            value = keyValue.substr( colonPos + 1, keyValue.size() - colonPos - 1 );

            if (!value.empty() && value != "-")
            {
                char *endptr;
                uintValue = strtoul( value.c_str(), &endptr, 10 );
                if ( *endptr == '\0' )
                {
                    if (key == "ds")
                    {
                        mDnsServers = splitString(value);
                    }
                    else if (key == "dlt")
                    {
                        mDnsLookupTimeout = uintValue;
                    }
                    else if (key == "dlr")
                    {
                        mDnsLookupRetryCount = uintValue;
                    }
                    else if (key == "dcel")
                    {
                        mDnsCacheEntryLimit = uintValue;
                    }
                    else if (key == "dct")
                    {
                        mDnsCacheTimeout = uintValue;
                    }
                    else if (key == "dmt")
                    {
                        mDnsMinimumTtl = uintValue;
                    }
                    else if (key == "dplf")
                    {
                        mDnsParallelLookupFlag = (uintValue == 1);
                    }
                    else if (key == "dllf")
                    {
                        mDnsLogLookupsFlag = (uintValue == 1);
                    }
                    else if (key == "dlqf")
                    {
                        mDnsLogQueriesFlag = (uintValue == 1);
                    }
                    else if (key == "dfs")
                    {
                        mDnsFallbackServers = splitString(value);
                    }
                    else if (key == "dfd")
                    {
                        mDnsFallbackDelay = uintValue;
                    }
                    else if (key == "dph")
                    {
                        mDnsPrefetchHosts = splitString(value);
                    }
                    else if (key == "dcfl")
                    {
                        mDnsConsecutiveFailuresLimit = uintValue;
                    }
                    else if (key == "stc")
                    {
                        mSecondaryTcpConnectionDelay = uintValue;
                    }
                    else if (key == "fttd")
                    {
                        mFastTcpTimeoutDelay = uintValue;
                    }
                    else if (key == "fttc")
                    {
                        mFastTcpTimeoutMaxCount = uintValue;
                    }
                    else if (key == "isrd")
                    {
                        mInitialSocketReattemptDelay = uintValue;
                    }
                    else if (key == "msrd")
                    {
                        mMaximumSocketReattemptDelay = uintValue;
                    }
                    else if (key == "hct")
                    {
                        mHttpConnectionTimeout = uintValue;
                    }
                    else if (key == "hst")
                    {
                        mHttpStallTimeout = uintValue;
                    }
                    else if (key == "srs")
                    {
                        mSocketReceiveBufferSize = uintValue;
                    }
                    else if (key == "pif")
                    {
                        mPreferIpV6Flag = (uintValue == 1);
                    }
                    else if (key == "hst_ncp")
                    {
                        mHstNumberOfConnectionPerServer = uintValue;
                    }
                    else if (key == "hst_hpd")
                    {
                        mHstHttpPipeliningDepth = uintValue;
                    }
                    else if (key == "hst_hpt")
                    {
                        mHstHttpPipeliningSupportThreshold = uintValue;
                    }
                    else if (key == "hst_hcb")
                    {
                        mHstHttpConnectBackoffTime = uintValue;
                    }
                    else if (key == "hst_hft")
                    {
                        mHstHttpConnectFailureThreshold = uintValue;
                    }
                    else if (key == "ts_s")
                    {
                        mTrafficShaperSpeed = uintValue;
                    }
                    else if (key == "ts_mins")
                    {
                        mTrafficShaperMinSleepInterval = uintValue;
                    }
                    else if (key == "ts_maxs")
                    {
                        mTrafficShaperMaxSleepInterval = uintValue;
                    }
                    else if (key == "ts_sc")
                    {
                        mTrafficShaperSoftCapacityLimit = uintValue;
                    }
                    else if (key == "ts_hc")
                    {
                        mTrafficShaperHardCapacityLimit = uintValue;
                    }
                }
            }
        }

        if (delimPos != string::npos)
        {
            beginPos = delimPos + 1;
        }
        else
        {
            break;
        }
    }

    NTRACE(
        TRACE_IP_CONNECTIVITY_MANAGER,
        "loaded the configurations: [%s] "
        "DnsServers: %s, DnsLookupTimeout: %u, DnsLookupRetryCount: %u, DnsCacheEntryLimit: %u, "
        "DnsCacheTimeout: %u, DnsMinimumTtl: %u, DnsParallelLookupFlag: %s, "
        "DnsLogLookupsFlag: %s, DnsLogQueriesFlag: %s, DnsFallbackServers: %s, DnsFallbackDelay: %u, "
        "DnsPrefetchHosts: %s, DnsConsecutiveFailuresLimit: %u, SecondaryTcpConnectDelay: %u, "
        "FastTcpTimeoutDelay: %u, FastTcpTimeoutMaxCount: %u, "
        "InitialSocketReattemptDelay: %u, MaximumSocketReattemptDelay: %u, "
        "HttpConnectTimeout: %u, HttpStallTimeout: %u, SocketReceiveBufferSize: %u, "
        "preferIpV6Flag: %s, HstNumberOfConnectionPerServer: %u, HstPipeliningDepath: %u, "
        "HstHttpPipeliningSupportThreshold: %u, HstHttpConnectBackoffTime: %u, "
        "HstHttpConnectFailureThreshold: %u, "
        "TrafficShaperSpeed: %u, TrafficShaperMinSleepInterval: %u, TrafficShaperMaxSleepInterval: %u,"
        "TrafficShaperSoftCapacityLimit: %u, TrafficShaperHardCapacityLimit: %u",
        configurationValue.c_str(),
        joinStringVector(mDnsServers).c_str(),
        mDnsLookupTimeout,
        mDnsLookupRetryCount,
        mDnsCacheEntryLimit,
        mDnsCacheTimeout,
        mDnsMinimumTtl,
        (mDnsParallelLookupFlag) ? "true" : "false",
        (mDnsLogLookupsFlag) ? "true" : "false",
        (mDnsLogQueriesFlag) ? "true" : "false",
        joinStringVector(mDnsFallbackServers).c_str(),
        mDnsFallbackDelay,
        joinStringVector(mDnsPrefetchHosts).c_str(),
        mDnsConsecutiveFailuresLimit,
        mSecondaryTcpConnectionDelay,
        mFastTcpTimeoutDelay,
        mFastTcpTimeoutMaxCount,
        mInitialSocketReattemptDelay,
        mMaximumSocketReattemptDelay,
        mHttpConnectionTimeout,
        mHttpStallTimeout,
        mSocketReceiveBufferSize,
        (mPreferIpV6Flag) ? "true" : "false",
        mHstNumberOfConnectionPerServer,
        mHstHttpPipeliningDepth,
        mHstHttpPipeliningSupportThreshold,
        mHstHttpConnectBackoffTime,
        mHstHttpConnectFailureThreshold,
        mTrafficShaperSpeed,
        mTrafficShaperMinSleepInterval,
        mTrafficShaperMaxSleepInterval,
        mTrafficShaperSoftCapacityLimit,
        mTrafficShaperHardCapacityLimit );
}

/** Store the configuration parameters. */
void NrdIpConnectivityManager::store()
{
    string configurationValue;
    char tempValue[80];

    configurationValue.clear();

    string dnsServerString(joinStringVector(mDnsServers));
    if (dnsServerString != DEFAULT_DNS_SERVERS)
    {
        configurationValue += "ds:" + dnsServerString + ",";
    }
    else
    {
        configurationValue += "ds:-,";
    }

    if (mDnsLookupTimeout != DEFAULT_DNS_LOOKUP_TIMEOUT)
    {
        snprintf(
            tempValue,
            80,
            "dlt:%u,",
            mDnsLookupTimeout );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "dlt:-,";
    }

    if (mDnsLookupRetryCount != DEFAULT_DNS_LOOKUP_RETRY_COUNT)
    {
        snprintf(
            tempValue,
            80,
            "dlr:%u,",
            mDnsLookupRetryCount );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "dlr:-,";
    }

    if (mDnsCacheEntryLimit != DEFAULT_DNS_CACHE_ENTRY_LIMIT)
    {
        snprintf(
            tempValue,
            80,
            "dcel:%u,",
            mDnsCacheEntryLimit );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "dcel:-,";
    }

    if (mDnsCacheTimeout != DEFAULT_DNS_CACHE_TIMEOUT)
    {
        snprintf(
            tempValue,
            80,
            "dct:%u,",
            mDnsCacheTimeout );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "dct:-,";
    }

    if (mDnsMinimumTtl != DEFAULT_DNS_MINIMUM_TTL)
    {
        snprintf(
            tempValue,
            80,
            "dmt:%u,",
            mDnsMinimumTtl );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "dmt:-,";
    }

    if (mDnsParallelLookupFlag != DEFAULT_DNS_PARALLEL_LOOKUP_FLAG)
    {
        snprintf(
            tempValue,
            80,
            "dplf:%u,",
            mDnsParallelLookupFlag ? 1 : 0 );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "dplf:-,";
    }

    if (mDnsLogLookupsFlag != DEFAULT_DNS_LOG_LOOKUPS_FLAG)
    {
        snprintf(
            tempValue,
            80,
            "dllf:%u,",
            mDnsLogLookupsFlag ? 1 : 0 );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "dllf:-,";
    }

    if (mDnsLogQueriesFlag != DEFAULT_DNS_LOG_QUERIES_FLAG)
    {
        snprintf(
            tempValue,
            80,
            "dlqf:%u,",
            mDnsLogQueriesFlag ? 1 : 0 );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "dlqf:-,";
    }

    string dnsFallbackServerString(joinStringVector(mDnsFallbackServers));
    if (dnsFallbackServerString != DEFAULT_DNS_FALLBACK_SERVERS)
    {
        configurationValue += "dfs:" + dnsFallbackServerString + ",";
    }
    else
    {
        configurationValue += "dfs:-,";
    }

    if (mDnsFallbackDelay != DEFAULT_DNS_FALLBACK_DELAY)
    {
        snprintf(
            tempValue,
            80,
            "dfd:%u,",
            mDnsFallbackDelay );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "dfd:-,";
    }

    string dnsPrefetchHostsString(joinStringVector(mDnsPrefetchHosts));
    if (dnsPrefetchHostsString != DEFAULT_DNS_PREFETCH_HOSTS)
    {
        configurationValue += "dph:" + dnsPrefetchHostsString + ",";
    }
    else
    {
        configurationValue += "dph:-,";
    }

    if (mDnsConsecutiveFailuresLimit != DEFAULT_DNS_CONSECUTIVE_FAILURES_LIMIT)
    {
        snprintf(
            tempValue,
            80,
            "dcfl:%u,",
            mDnsConsecutiveFailuresLimit);

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "dcfl:-,";
    }

    if (mSecondaryTcpConnectionDelay != DEFAULT_SECONDARY_TCP_CONNECTION_DELAY)
    {
        snprintf(
            tempValue,
            80,
            "stc:%u,",
            mSecondaryTcpConnectionDelay );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "stc:-,";
    }

    if (mFastTcpTimeoutDelay != DEFAULT_FAST_TCP_TIMEOUT_DELAY)
    {
        snprintf(
            tempValue,
            80,
            "fttd:%u,",
            mFastTcpTimeoutDelay );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "fttd:-,";
    }

    if (mFastTcpTimeoutMaxCount != DEFAULT_FAST_TCP_TIMEOUT_MAX_COUNT)
    {
        snprintf(
            tempValue,
            80,
            "fttc:%u,",
            mFastTcpTimeoutMaxCount );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "fttc:-,";
    }

    if (mInitialSocketReattemptDelay != DEFAULT_SOCKET_REATTEMPT_INITIAL_DELAY)
    {
        snprintf(
            tempValue,
            80,
            "isrd:%u,",
            mInitialSocketReattemptDelay );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "isrd:-,";
    }

    if (mMaximumSocketReattemptDelay != DEFAULT_SOCKET_REATTEMPT_MAXIMUM_DELAY)
    {
        snprintf(
            tempValue,
            80,
            "msrd:%u,",
            mMaximumSocketReattemptDelay );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "msrd:-,";
    }

    if (mHttpConnectionTimeout != DEFAULT_HTTP_CONNECTION_TIMEOUT)
    {
        snprintf(
            tempValue,
            80,
            "hct:%u,",
            mHttpConnectionTimeout );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "hct:-,";
    }

    if (mHttpStallTimeout != DEFAULT_HTTP_STALL_TIMEOUT)
    {
        snprintf(
            tempValue,
            80,
            "hst:%u,",
            mHttpStallTimeout );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "hst:-,";
    }

    if (mSocketReceiveBufferSize != DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE)
    {
        snprintf(
            tempValue,
            80,
            "srs:%u,",
            mSocketReceiveBufferSize );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "srs:-,";
    }

    if (mPreferIpV6Flag != DEFAULT_PREFER_IP_V6_FLAG)
    {
        snprintf(
            tempValue,
            80,
            "pif:%u,",
            mPreferIpV6Flag ? 1 : 0 );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "pif:-,";
    }

    if (mHstNumberOfConnectionPerServer != DEFAULT_HST_NUMBER_CONNECTION_PER_SERVER)
    {
        snprintf(
            tempValue,
            80,
            "hst_ncp:%u,",
            mHstNumberOfConnectionPerServer );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "hst_ncp:-,";
    }

    if (mHstHttpPipeliningDepth != DEFAULT_HST_HTTP_PIPELINING_DEPATH)
    {
        snprintf(
            tempValue,
            80,
            "hst_hpd:%u,",
            mHstHttpPipeliningDepth );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "hst_hpd:-,";
    }

    if (mHstHttpPipeliningSupportThreshold != DEFAULT_HST_HTTP_PIPELINING_SUPPORT_THRESHOLD)
    {
        snprintf(
            tempValue,
            80,
            "hst_hpt:%u,",
            mHstHttpPipeliningSupportThreshold );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "hst_hpt:-,";
    }

    if (mHstHttpConnectBackoffTime != DEFAULT_HST_HTTP_CONNECT_BACKOFF_TIME)
    {
        snprintf(
            tempValue,
            80,
            "hst_hcb:%u,",
            mHstHttpConnectBackoffTime );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "hst_hcb:-,";
    }

    if (mHstHttpConnectFailureThreshold != DEFAULT_HST_HTTP_CONNECT_FAILURE_THRESHOLD)
    {
        snprintf(
            tempValue,
            80,
            "hst_hft:%u,",
            mHstHttpConnectFailureThreshold );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "hst_hft:-,";
    }

    if (mTrafficShaperSpeed != DEFAULT_TRAFFIC_SHAPER_SPEED)
    {
        snprintf(
            tempValue,
            80,
            "ts_s:%u,",
            mTrafficShaperSpeed );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "ts_s:-,";
    }

    if (mTrafficShaperMinSleepInterval != DEFAULT_TRAFFIC_SHAPER_MIN_SLEEP_INTERVAL)
    {
        snprintf(
            tempValue,
            80,
            "ts_mins:%u,",
            mTrafficShaperMinSleepInterval );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "ts_mins:-,";
    }

    if (mTrafficShaperMaxSleepInterval != DEFAULT_TRAFFIC_SHAPER_MAX_SLEEP_INTERVAL)
    {
        snprintf(
            tempValue,
            80,
            "ts_maxs:%u,",
            mTrafficShaperMaxSleepInterval );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "ts_maxs:-,";
    }

    if (mTrafficShaperSoftCapacityLimit != DEFAULT_TRAFFIC_SHAPER_SOFT_CAPACITY)
    {
        snprintf(
            tempValue,
            80,
            "ts-sc:%u,",
            mTrafficShaperSoftCapacityLimit );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "ts-sc:-,";
    }

    if (mTrafficShaperHardCapacityLimit != DEFAULT_TRAFFIC_SHAPER_HARD_CAPACITY)
    {
        snprintf(
            tempValue,
            80,
            "ts-hc:%u",
            mTrafficShaperHardCapacityLimit );

        configurationValue += tempValue;
    }
    else
    {
        configurationValue += "ts-hc:-";
    }



    nrdApp()->writeSystemConfiguration( SystemKeys::IpConnectivityParas, configurationValue.c_str() );

    NTRACE(
        TRACE_IP_CONNECTIVITY_MANAGER,
        "stored the configurations: [%s] "
        "DnsServers: %s, DnsLookupTimeout: %u, DnsLookupRetryCount: %u, DnsCacheEntryLimit: %u, "
        "DnsCacheTimeout: %u, DnsMinimumTtl: %u, DnsParallelLookupFlag: %s, DnsLogLookupsFlag: %s, "
        "DnsLogQueriesFlag: %s, DnsFallbackServers: %s, DnsFallbackDelay: %u, DnsPrefetchHosts: %s, "
        "DnsConsecutiveFailuresLimit: %u SecondaryTcpConnectDelay: %u, FastTcpTimeoutDelay: %u, "
        "FastTcpTimeoutMaxCount: %u, "
        "InitialSocketReattemptDelay: %u, "
        "MaximumSocketReattemptDelay: %u, "
        "HttpConnectTimeout: %u, HttpStallTimeout: %u, "
        "SocketReceiveBufferSize: %u, preferIpV6Flag: %s, HstNumberOfConnectionPerServer: %u, "
        "HstPipeliningDepath: %u, HstHttpPipeliningSupportThreshold: %u, "
        "HstHttpConnectBackoffTime: %u, HstHttpConnectFailureThreshold: %u, "
        "TrafficShaperSpeed: %u, TrafficShaperMinSleepInterval: %u, TrafficShaperMaxSleepInterval: %u,"
        "TrafficShaperSoftCapacityLimit: %u, TrafficShaperHardCapacityLimit: %u",
        configurationValue.c_str(),
        joinStringVector(mDnsServers).c_str(),
        mDnsLookupTimeout,
        mDnsLookupRetryCount,
        mDnsCacheEntryLimit,
        mDnsCacheTimeout,
        mDnsMinimumTtl,
        (mDnsParallelLookupFlag) ? "true" : "false",
        (mDnsLogLookupsFlag) ? "true" : "false",
        (mDnsLogQueriesFlag) ? "true" : "false",
        joinStringVector(mDnsFallbackServers).c_str(),
        mDnsFallbackDelay,
        joinStringVector(mDnsPrefetchHosts).c_str(),
        mDnsConsecutiveFailuresLimit,
        mSecondaryTcpConnectionDelay,
        mFastTcpTimeoutDelay,
        mFastTcpTimeoutMaxCount,
        mInitialSocketReattemptDelay,
        mMaximumSocketReattemptDelay,
        mHttpConnectionTimeout,
        mHttpStallTimeout,
        mSocketReceiveBufferSize,
        (mPreferIpV6Flag) ? "true" : "false",
        mHstNumberOfConnectionPerServer,
        mHstHttpPipeliningDepth,
        mHstHttpPipeliningSupportThreshold,
        mHstHttpConnectBackoffTime,
        mHstHttpConnectFailureThreshold,
        mTrafficShaperSpeed,
        mTrafficShaperMinSleepInterval,
        mTrafficShaperMaxSleepInterval,
        mTrafficShaperSoftCapacityLimit,
        mTrafficShaperHardCapacityLimit );
}

/** Get the DNS servers */
std::vector<std::string> NrdIpConnectivityManager::dnsServers()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::dnsServers();
}

/** Get the DNS lookup timeout value, in ms. */
uint32_t NrdIpConnectivityManager::getDnsLookupTimeout()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsLookupTimeout();
}

/** Get the DNS lookup retry count. */
uint32_t NrdIpConnectivityManager::getDnsLookupRetryCount()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsLookupRetryCount();
}

/** Get the DNS cache maximum number of entries. */
uint32_t NrdIpConnectivityManager::getDnsCacheEntryLimit()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsCacheEntryLimit();
}

/** Get the DNS cache timeout value, in ms. */
uint32_t NrdIpConnectivityManager::getDnsCacheTimeout()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsCacheTimeout();
}

/** Get the DNS minimum TTL value, in ms */
uint32_t NrdIpConnectivityManager::getDnsMinimumTtl()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsMinimumTtl();
}

/** Get the DNS parallel lookup setting */
bool NrdIpConnectivityManager::getDnsParallelLookupFlag()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsParallelLookupFlag();
}

/** Get the DNS lookup logging setting */
bool NrdIpConnectivityManager::getDnsLogLookupsFlag()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsLogLookupsFlag();
}

/** Get the DNS query logging setting */
bool NrdIpConnectivityManager::getDnsLogQueriesFlag()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsLogQueriesFlag();
}

/** Get the DNS fallback servers */
std::vector<std::string> NrdIpConnectivityManager::dnsFallbackServers()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::dnsFallbackServers();
}

/** Get the DNS fallback delay, in ms */
uint32_t NrdIpConnectivityManager::getDnsFallbackDelay()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsFallbackDelay();
}

/** Get the DNS hosts to prefetch */
std::vector<std::string> NrdIpConnectivityManager::getDnsPrefetchHosts()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsPrefetchHosts();
}

/** Get the DNS consecutive failures limit */
uint32_t NrdIpConnectivityManager::getDnsConsecutiveFailuresLimit()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getDnsConsecutiveFailuresLimit();
}

/** Get the secondary TCP connection delay, in ms. */
uint32_t NrdIpConnectivityManager::getSecondaryTcpConnectionDelay()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getSecondaryTcpConnectionDelay();
}

/** Get the fast TCP timeout delay, in ms. */
uint32_t NrdIpConnectivityManager::getFastTcpTimeoutDelay()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getFastTcpTimeoutDelay();
}

/** Get the fast TCP timeout max count */
uint32_t NrdIpConnectivityManager::getFastTcpTimeoutMaxCount()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getFastTcpTimeoutMaxCount();
}

/** Get the initial socket reattempt delay */
uint32_t NrdIpConnectivityManager::getInitialSocketReattemptDelay()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getInitialSocketReattemptDelay();
}

/** Get the maximum socket reattempt delay */
uint32_t NrdIpConnectivityManager::getMaximumSocketReattemptDelay()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getMaximumSocketReattemptDelay();
}

/** Get the HTTP connection timeout value, in ms. */
uint32_t NrdIpConnectivityManager::getHttpConnectionTimeout()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getHttpConnectionTimeout();
}

/** Get the HTTP stall timeout value, in ms. */
uint32_t NrdIpConnectivityManager::getHttpStallTimeout()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getHttpStallTimeout();
}

/** Get the socket receive buffer size. */
uint32_t NrdIpConnectivityManager::getSocketReceiveBufferSize()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getSocketReceiveBufferSize();
}

/** Get the IP connectivity policy for a destination. */
IpConnectivityManager::Policy NrdIpConnectivityManager::getIpConnectivityPolicy()
{
    IpConnectivityManager::Policy retPolicy = IP_V4_ONLY;

#ifdef NRDP_HAS_IPV6

    ScopedMutex scopedMutex( mMutex );

    //Get the IP connectivity mode from DPI.

    IpConnectivityMode ipConnectivityMode;

    ipConnectivityMode = nrdApp()->getSystem()->getIpConnectivityMode();

    if (ipConnectivityMode == IP_V4_ONLY_MODE)
    {
        NTRACE(
            TRACE_IP_CONNECTIVITY_MANAGER,
            "selected IPv4 only because of DPI connectivity mode." );

        retPolicy = IP_V4_ONLY;
    }
    else if (ipConnectivityMode == IP_V6_ONLY_MODE)
    {
        NTRACE(
            TRACE_IP_CONNECTIVITY_MANAGER,
            "selected IPv6 only because of DPI connectivity mode." );

        retPolicy = IP_V6_ONLY;
    }
    else //dualstack
    {
        if (mPreferIpV6Flag)
        {
            NTRACE(
                TRACE_IP_CONNECTIVITY_MANAGER,
                "selected dual stack IPv6/IPv4 because of preference." );

            //Prefer IPv6.
            retPolicy = IP_V6_V4;
        }
        else
        {
            NTRACE(
                TRACE_IP_CONNECTIVITY_MANAGER,
                "selected dual stack IPv4/IPv6 because of preference." );

            //Dual stack, prefer IPv4.
            retPolicy = IP_V4_V6;
        }
    }

#else
    NTRACE(
        TRACE_IP_CONNECTIVITY_MANAGER,
        "selected IPv4 only because IPv6 is disabled at compilation time." );
#endif

    return retPolicy;
}

//Get the number of connection per server.
uint32_t NrdIpConnectivityManager::getHstNumberOfConnectionPerServer()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getHstNumberOfConnectionPerServer();
}

//Get the HTTP pipelining depth.
uint32_t NrdIpConnectivityManager::getHstHttpPipeliningDepth()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getHstHttpPipeliningDepth();
}

//Get the HTTP pipelining support threshold.
uint32_t NrdIpConnectivityManager::getHstHttpPipeliningSupportThreshold()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getHstHttpPipeliningSupportThreshold();
}

//Get the HTTP connect backoff time, in ms.
uint32_t NrdIpConnectivityManager::getHstHttpConnectBackoffTime()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getHstHttpConnectBackoffTime();
}

//Get the HTTP connect failure threshold.
uint32_t NrdIpConnectivityManager::getHstHttpConnectFailureThreshold()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getHstHttpConnectFailureThreshold();
}

//Get the Traffic Shaper speed
uint32_t NrdIpConnectivityManager::getTrafficShaperSpeed()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getTrafficShaperSpeed();
}

//Get the Traffic Shaper min/max sleep interval
std::pair<uint32_t,uint32_t> NrdIpConnectivityManager::getTrafficShaperSleepIntervals()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getTrafficShaperSleepIntervals();
}

//Get the Traffic Shaper soft/hard capacitity
std::pair<uint32_t,uint32_t> NrdIpConnectivityManager::getTrafficShaperCapacities()
{
    ScopedMutex scopedMutex( mMutex) ;

    return IpConnectivityManager::getTrafficShaperCapacities();
}

//Add/remove IpConnectivityListeners
void NrdIpConnectivityManager::addIpConnectivityListener( IpConnectivityListenerPtr pListener )
{
    ScopedMutex scopedMutex( mMutex );

    IpConnectivityManager::addIpConnectivityListener( pListener );
}

void NrdIpConnectivityManager::removeIpConnectivityListener( IpConnectivityListenerPtr pListener )
{
    ScopedMutex scopedMutex( mMutex );

    IpConnectivityManager::removeIpConnectivityListener( pListener );
}

//Signal a change in traffic shaper configuration
void NrdIpConnectivityManager::trafficShaperConfigChanged()
{
    ScopedMutex scopedMutex( mMutex );

    IpConnectivityManager::trafficShaperConfigChanged();
}

/** Destructor. */
NrdIpConnectivityManager::~NrdIpConnectivityManager()
{
}

}} // namespace netflix::util

