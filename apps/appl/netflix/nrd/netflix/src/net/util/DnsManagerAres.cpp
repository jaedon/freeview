/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DnsManager.h"
#include <nrdbase/config.h>
#include <nrdbase/Log.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Thread.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/Variant.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdnet/IpConnectivityManager.h>
#include <ares.h>
#include <arpa/nameser.h>
#include <algorithm>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

#ifdef NRDP_HAS_SOCKETIZER
#include <nrdsocketizer/Socketizer.h>
using namespace netflix::socketizer;
#endif

#ifdef NDEBUG
    #define DNSMANAGER_DEBUG_NTRACE(...)
#else
    #define DNSMANAGER_DEBUG_NTRACE(...) //NTRACE(TRACE_DNS_MANAGER, __VA_ARGS__)
#endif

#define EINTRWRAP(VAR, BLOCK) do { VAR = BLOCK; } while ((VAR == -1) && (errno == EINTR))

namespace netflix {
namespace DnsManager {

class Channel;
class Query;
class Record;

typedef std::list< RequestPtr > Requests;
typedef shared_ptr<Channel> ChannelPtr;
typedef std::vector< ChannelPtr > Channels;
typedef shared_ptr<Query> QueryPtr;
typedef shared_ptr< Client > DnsClientPtr;
typedef std::list< DnsClientPtr> Clients;
typedef shared_ptr<Record> RecordPtr;
typedef std::list< RecordPtr > Records;

// Channel is a logical connection to one or more DNS servers
class Channel
{
public:
    inline Channel();
    inline ~Channel();
    int create(const std::string & server);
    int create(const std::vector<std::string> & servers);

    ares_channel mAresChannel;
    std::string mName;
    bool mIsFallback;

    // stats
    unsigned int mSuccessfulResponses;
    unsigned int mErrorResponses;
    unsigned int mTimeoutResponses;
    unsigned int mWinningResponses;
    AseTimeVal mTotalResponseTime;
};

// Query encapsulates DNS query setup info and results
class Query
{
public:
    inline Query();
    inline ~Query();

    Channel * mChannel;
    Record * mRecord;
    AddressType mAddressType;
    AseTimeVal mStartTime;
    RequestResult mResult;
};

// Client represents an external user of the DnsManager
class Client
{
public:
    inline Client(const std::string & name);
    inline ~Client();
    inline void setSignal();
    inline void clearSignal();

    std::string mName;
    int mSignalPipe[2];
    Requests mRequests;
};

// Record is a DNS cache entry for a given host/address type
class Record
{
public:
    inline Record();
    inline ~Record();

    inline std::string toString() const;

    enum State { STATE_WAITING, STATE_PROCESSING, STATE_COMPLETE };
    typedef std::list< RequestPtr > Requests;
    typedef std::list< QueryPtr > Queries;

    State mState;
    RequestParams mParams;
    Requests mRequests;
    Queries mQueries;
    unsigned int mProcessingCount;
    RequestResultPtr mResult;
    RequestResultPtr mNewResult;
    Query * mFirstFallbackQuery;
    AseTimeVal mLastLookupTime;
};

// Request holds all the information for each request a Client adds
class Request
{
public:
    inline Request();
    inline ~Request();

    ClientPtr mClient;
    RequestParams mParams;
    AseTimeVal mTime;
    RequestResultPtr mResult;
    void * mUserData;
    bool mSignaled;
};

// WorkerThread asynchronously processes all the Requests, issues Queries, and gathers Results
class WorkerThread : public Thread
{
public:
    inline WorkerThread();
    inline ~WorkerThread();

    inline void setSignal();

    inline void pause();
    inline void resume();

protected:
    inline void setPaused(bool paused);
    inline bool waitForResume();
    void Run();

    enum PauseState { PS_RUNNING, PS_PAUSE_REQUESTED, PS_PAUSED, PS_RESUME_REQUESTED };

    ConditionVariable mCondition;
    volatile bool mShutdown;
    volatile PauseState mPauseState;
    Mutex mMutex;
    int mSignalPipe[2];
};

static Mutex sInstanceMutex(DNSMANAGER_INSTANCE_MUTEX, "DNSManager");
static bool sUsable;
static Configuration sConfig;
static bool sAresInitialized;
static Channels sChannels;
static Mutex sClientsMutex(DNSMANAGER_CLIENTS_MUTEX, "DNSClients");
static Clients sClients;
static Mutex sRecordsMutex(DNSMANAGER_RECORDS_MUTEX, "DNSRecords");
static Records sRecords;
static shared_ptr<WorkerThread> sWorkerThread;
static unsigned int sCacheHits;
static unsigned int sCacheMisses;
static unsigned int sConsecutiveFailures;
static std::string sGroupName;

static unsigned int sSuccessfulResponses;
static unsigned int sErrorResponses;
static unsigned int sTimeoutRespones;
static AseTimeVal sTotalResponseTime;

static bool sDown = false;
static Configuration sDownConfig;


static std::vector<std::string> aresDefaultServers();
static RecordPtr obtainRecord(const RequestParams & requestParams, RequestPtr request);
static RequestResultPtr resolveIpLiteral(const RequestParams & requestParams);
static RequestResultPtr resolveHostsFile(const RequestParams & requestParams);
static void checkConsecutiveFailures();
static void aresLookupCallback(void * arg, int status, int timeouts, unsigned char * abuf, int alen);
static AseErrorCode aresErrorToAseErrorCode(int aresError);
static void logLookupResult(const std::string & hostName, const AseTimeVal & responseTime, const Query * query, const RequestResult * result);
static void logQueryResult(const std::string & hostName, const AseTimeVal & responseTime, const Query * query);
static bool createPipe(int * pipe);
static bool clearPipe(int fd);
static inline std::string resultToString(const RequestResultPtr & result);
static inline bool operator == (const RequestParams & lhs, const RequestParams & rhs);
static inline bool operator == (const Configuration & lhs, const Configuration & rhs);

void create()
{
    NTRACE(TRACE_DNS_MANAGER, "create()");

    ScopedMutex instanceLock(sInstanceMutex);

    ASSERT(!sUsable, "already created");
    if (sUsable)
        return;

    int retVal = ARES_SUCCESS;

    // initialize c-ares library.
    if (!sAresInitialized) {
        retVal = ares_library_init(ARES_LIB_INIT_ALL);
        if (retVal == ARES_SUCCESS)
            sAresInitialized = true;
    }

    if (retVal == ARES_SUCCESS) {
        if (!sConfig.mParallelLookups) {
            NTRACE(TRACE_DNS_MANAGER, "parallel lookups disabled");
            // create channel for default servers (standard serial lookup)
            ChannelPtr channel(new Channel);
            retVal = channel->create(sConfig.mServers);
            if (retVal != ARES_SUCCESS)
                Log::error(TRACE_DNS_MANAGER, "standard serial channel creation failed: %s", ares_strerror(retVal));
            else
                sChannels.push_back(channel);
        } else {
            NTRACE(TRACE_DNS_MANAGER, "parallel lookups enabled");

            if (sConfig.mServers.empty()) {
                if (sConfig.mUseDefaultServers)
                    sConfig.mServers = aresDefaultServers();
                if (sConfig.mServers.empty())
                    retVal = ARES_ESERVFAIL;
            }

            // create channels for standard servers
            for (Configuration::DnsServers::const_iterator sit = sConfig.mServers.begin(); sit != sConfig.mServers.end(); ++sit) {
                const std::string & dnsServer = *sit;

                ChannelPtr channel(new Channel);
                retVal = channel->create(dnsServer);
                if (retVal != ARES_SUCCESS)
                    Log::error(TRACE_DNS_MANAGER, "standard channel %s creation failed: %s", dnsServer.c_str(), ares_strerror(retVal));
                else
                    sChannels.push_back(channel);
            }
        }

        // create channels for fallback servers
        IpConnectivityManager::Policy ipConnectivityPolicy = IpConnectivityManager::instance()->getIpConnectivityPolicy();
        for (Configuration::DnsServers::const_iterator sit = sConfig.mFallbackServers.begin(); sit != sConfig.mFallbackServers.end(); ++sit) {
            const std::string & dnsServer = *sit;
            NrdIpAddr nrdIpAddr(dnsServer);
            if ((NrdIpAddr::IP_V4 == nrdIpAddr.getVersion() && (ipConnectivityPolicy != IpConnectivityManager::IP_V6_ONLY)) ||
                (NrdIpAddr::IP_V6 == nrdIpAddr.getVersion() && (ipConnectivityPolicy != IpConnectivityManager::IP_V4_ONLY))) {
                if (std::find(sConfig.mServers.begin(), sConfig.mServers.end(), dnsServer) != sConfig.mServers.end()) {
                    NTRACE(TRACE_DNS_MANAGER, "fallback server '%s' already in default server list, ignoring", dnsServer.c_str());
                } else {
                    ChannelPtr channel(new Channel);
                    retVal = channel->create(dnsServer);
                    if (retVal != ARES_SUCCESS) {
                        Log::error(TRACE_DNS_MANAGER, "fallback channel %s failed: %s", dnsServer.c_str(), ares_strerror(retVal));
                    } else {
                        channel->mIsFallback = true;
                        sChannels.push_back(channel);
                    }
                }
            }
        }
    }

    if (!sAresInitialized || sChannels.empty()) {
        NTRACE(TRACE_DNS_MANAGER, "create(): unusable, ares %s, channels %s",
               sAresInitialized ? "initialized" : "not initialized", sChannels.empty() ? "empty" : "not empty");
    } else {
        sUsable = true;

        NTRACE(TRACE_DNS_MANAGER, "create(): usable");

        // start the worker thread
        if (!sWorkerThread) {
            NTRACE(TRACE_DNS_MANAGER, "create(): starting worker thread");
            sWorkerThread.reset(new WorkerThread);
            sWorkerThread->Start();
        }

        for (Configuration::Hosts:: const_iterator hit = sConfig.mPrefetchHosts.begin(); hit != sConfig.mPrefetchHosts.end(); ++hit) {
            const std::string & host = *hit;

            NTRACE(TRACE_DNS_MANAGER, "create(): prefetching %s", host.c_str());

            RequestParams requestParams;
            requestParams.mHostName = host;

#ifdef NRDP_HAS_IPV6
            IpConnectivityManager::Policy ipConnectivityPolicy = IpConnectivityManager::instance()->getIpConnectivityPolicy();
            if (ipConnectivityPolicy != IpConnectivityManager::IP_V6_ONLY) {
                // ipv4 address resolution request creation if ipv4 network is available
                requestParams.mAddressType = ADDRESSTYPE_IPV4;
                obtainRecord(requestParams, RequestPtr());
            }
            if (ipConnectivityPolicy != IpConnectivityManager::IP_V4_ONLY) {
                // ipv6 address resolution request creation if ipv6 network is available
                requestParams.mAddressType = ADDRESSTYPE_IPV6;
                obtainRecord(requestParams, RequestPtr());
            }
#else
            requestParams.mAddressType = ADDRESSTYPE_IPV4;
            obtainRecord(requestParams, RequestPtr());
#endif
        }
    }

    logConfiguration();
}

void destroy()
{
    NTRACE(TRACE_DNS_MANAGER, "destroy()");

    ScopedMutex instanceLock(sInstanceMutex);
    DNSMANAGER_DEBUG_NTRACE("killing worker thread");
    sWorkerThread.reset();
    ScopedMutex clientsLock(sClientsMutex);
    ScopedMutex recordsLock(sRecordsMutex);

    sCacheHits = 0;
    sCacheMisses = 0;
    sConsecutiveFailures = 0;

    DNSMANAGER_DEBUG_NTRACE("destroying %zu channels", sChannels.size());
    sChannels.clear();
    DNSMANAGER_DEBUG_NTRACE("destroying %zu records", sRecords.size());
    sRecords.clear();
    DNSMANAGER_DEBUG_NTRACE("destroying %zu clients", sClients.size());
    sClients.clear();
    sConfig.restoreDefaults();

    DNSMANAGER_DEBUG_NTRACE("destroying ares");
    if (sAresInitialized) {
        ares_library_cleanup();
        sAresInitialized = false;
    }

    sUsable = false;
}

void down()
{
    if (!sDown) {
        sDownConfig = sConfig;
        Configuration config;
        config.restoreDefaults();
        config.mUseDefaultServers = false;
        setConfiguration(config);
        sDown = true;
    }
}

void up()
{
    if (sDown) {
        sDown = false;
        create();
        setConfiguration(sDownConfig);
    }
}

void Configuration::restoreDefaults()
{
    mParallelLookups = false;
    mLookupTimeout = AseTimeVal::fromMS(1000);
    mLookupRetries = 4;
    mCacheMaxEntries = 16;
    mCacheMaxTime = AseTimeVal::fromMS(300000);
    mTtlMinTime = AseTimeVal::fromMS(20000);
    mServers.clear();
    mFallbackServers.clear();
    mFallbackDelay = 0;
    mLogLookups = false;
    mLogQueries = false;
    mPrefetchHosts.clear();
    mConsecutiveFailuresLimit = 3;
    mUseDefaultServers = true;
}

std::string Configuration::toString() const
{
    std::ostringstream str;
    str << "dplf: " << sConfig.mParallelLookups << ", ";
    str << "dlt: " << sConfig.mLookupTimeout.ms() << ", ";
    str << "dlr: " << sConfig.mLookupRetries << ", ";
    str << "dcel: " << sConfig.mCacheMaxEntries << ", ";
    str << "dct: " << sConfig.mCacheMaxTime.ms() << ", ";
    str << "dmt: " << sConfig.mTtlMinTime.ms() << ", ";
    str << "dllf: " << sConfig.mLogLookups << ", ";
    str << "dlqf: " << sConfig.mLogQueries << ", ";

    str << "srv: ";
    for (DnsServers::const_iterator sit = sConfig.mServers.begin(); sit != sConfig.mServers.end(); ++sit) {
        if (sit != sConfig.mServers.begin())
            str<< ";";
        str << *sit;
    }
    str << ", ";

    str << "dfs: ";
    for (DnsServers::const_iterator sit = sConfig.mFallbackServers.begin(); sit != sConfig.mFallbackServers.end(); ++sit) {
        if (sit != sConfig.mFallbackServers.begin())
            str<< ";";
        str << *sit;
    }
    str << ", ";

    str << "dfd: " << sConfig.mFallbackDelay.ms() << ", ";

    str << "dph: ";
    for (DnsServers::const_iterator sit = sConfig.mPrefetchHosts.begin(); sit != sConfig.mPrefetchHosts.end(); ++sit) {
        if (sit != sConfig.mPrefetchHosts.begin())
            str<< ";";
        str << *sit;
    }
    str << ", ";

    str << "dcfl: " << sConfig.mConsecutiveFailuresLimit;
    return str.str();
}

void setConfiguration(const Configuration & config)
{
    NTRACE(TRACE_DNS_MANAGER, "setConfiguration()");

    ScopedMutex instanceLock(sInstanceMutex);

    if (sDown) {
        sDownConfig = config;
        return;
    }

    if (config == sConfig) {
        NTRACE(TRACE_DNS_MANAGER, "identical configuration, ignoring");
    } else {
        bool needsReinit = false;
        if ((config.mParallelLookups != sConfig.mParallelLookups) ||
            (config.mLookupTimeout != sConfig.mLookupTimeout) ||
            (config.mLookupRetries != sConfig.mLookupRetries) ||
            (config.mServers != sConfig.mServers) ||
            (config.mFallbackServers != sConfig.mFallbackServers))
        {
            // these changes require reinitializing ares
            needsReinit = true;
        }

        sConfig = config;

        NTRACE(TRACE_DNS_MANAGER, "config: %s", sConfig.toString().c_str());

#ifndef NDEBUG
        //// for testing
        //sConfig.mLookupRetries = 4;
        //sConfig.mLookupTimeout = AseTimeVal::fromMS(750);
        //sConfig.mCacheMaxEntries = 0;
        //sConfig.mCacheMaxTime = 0; //AseTimeVal::fromMS(60 * 1000);
        //sConfig.mTtlMinTime = AseTimeVal::fromMS(5 * 1000);
        //sConfig.mParallelLookups = true;
        //sConfig.mServers.clear();
        //sConfig.mServers.push_back("1.1.1.1"); // bad
        //sConfig.mServers.push_back("10.2.225.176"); // Configurable test DNS
        //sConfig.mServers.push_back("10.2.128.180"); // Netflix
        //sConfig.mServers.push_back("64.102.255.44"); // Cisco
        //sConfig.mFallbackServers.clear();
        //sConfig.mFallbackServers.push_back("8.8.8.8"); // Google
        //sConfig.mFallbackServers.push_back("8.8.4.4"); // Google
        //sConfig.mFallbackServers.push_back("209.244.0.3"); // Level3
        //sConfig.mFallbackServers.push_back("209.244.0.4"); // Level3
        //sConfig.mFallbackServers.push_back("208.67.222.222"); // OpenDNS
        //sConfig.mFallbackServers.push_back("208.67.220.220"); // OpenDNS
        //sConfig.mFallbackServers.push_back("156.154.70.1"); // DNS Advantage
        //sConfig.mFallbackServers.push_back("156.154.71.1"); // DNS Advantage
        //sConfig.mFallbackServers.push_back("8.26.56.26"); // Comodo
        //sConfig.mFallbackServers.push_back("8.20.247.20"); // Comodo
        //sConfig.mFallbackServers.push_back("1.1.1.1"); // bad
        ////sConfig.mFallbackServers.push_back("1.1.1.2"); // bad
        //sConfig.mFallbackDelay = AseTimeVal::fromMS(100);
        //sConfig.mLogLookups = true;
        //sConfig.mLogQueries = true;
        //sConfig.mPrefetchHosts.clear();
        //sConfig.mConsecutiveFailuresLimit = 0;
#endif

        if (needsReinit && (sAresInitialized || !sChannels.empty())) {
            Log::warn(TRACE_DNS_MANAGER, "reinitializing due to config change");
            restartResolver();
        }
    }
}

void getConfiguration(Configuration & config)
{
    NTRACE(TRACE_DNS_MANAGER, "getConfiguration()");

    ScopedMutex instanceLock(sInstanceMutex);
    config = sConfig;
}

ClientPtr clientAdd(const std::string & name)
{
    ClientPtr client(new Client(name));

    NTRACE(TRACE_DNS_MANAGER, "clientAdd(%s/%p)", client->mName.c_str(), client.get());

    ScopedMutex clientsLock(sClientsMutex);
    sClients.push_back(client);
    return client;
}

void clientRemove(ClientPtr client)
{
    ASSERT(client, "null client");

    NTRACE(TRACE_DNS_MANAGER, "clientRemove(%s/%p)", client->mName.c_str(), client.get());

    sClientsMutex.lock();
    Clients::iterator cit = std::find(sClients.begin(), sClients.end(), client);
    ASSERT(cit != sClients.end(), "client not found");
    if (cit != sClients.end()) {
        DnsClientPtr client = *cit;
        sClients.erase(cit);
        sClientsMutex.unlock();

        while (!client->mRequests.empty()) {
            RequestPtr request = client->mRequests.front();
            requestRemove(request);
        }

        return;
    }
    sClientsMutex.unlock();

    Log::warn(TRACE_DNS_MANAGER, "warning, client %s not found", client->mName.c_str());
}

int clientGetSignalFd(ClientPtr client)
{
    ASSERT(client, "null client");

    DNSMANAGER_DEBUG_NTRACE("clientGetSignalFd(%s/%p)", client->mName.c_str(), client.get());

    return client->mSignalPipe[0];
}

void clientClearSignal(ClientPtr client)
{
    ASSERT(client, "null client");

    DNSMANAGER_DEBUG_NTRACE("clientClearSignal(%s/%p)", client->mName.c_str(), client.get());

    client->clearSignal();
}

bool clientHasPendingRequests(ClientPtr client)
{
    ASSERT(client, "null client");

    if (!client->mRequests.empty()) {
        DNSMANAGER_DEBUG_NTRACE("clientHasPendingRequests(%s/%p)", client->mName.c_str(), client.get());
        return true;
    } else {
        return false;
    }
}

RequestPtr requestAdd(ClientPtr client, const RequestParams & requestParams)
{
    ASSERT(client, "null client");

    NTRACE(TRACE_DNS_MANAGER, "%s - requestAdd() by %s/%p", requestParams.mHostName.c_str(),
           client->mName.c_str(), client.get());

    RequestPtr request;

    if (requestParams.mHostName.empty()) {
        Log::warn(TRACE_DNS_MANAGER, "empty host name request ignored");
        return request;
    }

    if (!((requestParams.mAddressType > ADDRESSTYPE_NONE) && (requestParams.mAddressType < ADDRESSTYPE_COUNT))) {
        Log::warn(TRACE_DNS_MANAGER, "empty host name request ignored");
        return request;
    }

    RequestResultPtr result = resolveIpLiteral(requestParams);
    if (!result && !sUsable)
        return request;

    request.reset(new Request);
    request->mClient = client;
    request->mParams = requestParams;
    request->mTime = AseTimeVal::now();

    if (result) {
        request->mResult = result;
    } else {
        RecordPtr record = obtainRecord(requestParams, request);

        // update stats
        if (resultIsUsable(record->mResult))
            ++sCacheHits;
        else
            ++sCacheMisses;
    }

    return request;
}

void requestRemove(RequestPtr request)
{
    ASSERT(request, "null request");

    NTRACE(TRACE_DNS_MANAGER, "%s - requestRemove() by %s/%p", request->mParams.mHostName.c_str(),
           request->mClient->mName.c_str(), request->mClient.get());

    ScopedMutex recordsLock(sRecordsMutex);

    // remove request from record
    bool recordFound = false;
    for (Records::iterator rcit = sRecords.begin(); rcit != sRecords.end(); ++rcit) {
        RecordPtr record = *rcit;
        if (record->mParams == request->mParams) {
            Requests::iterator rqit = std::find(record->mRequests.begin(), record->mRequests.end(), request);
            if (rqit != record->mRequests.end()) {
                record->mRequests.erase(rqit);
            } else {
                Log::warn(TRACE_DNS_MANAGER, "warning, request %s not found in record", request->mParams.mHostName.c_str());
            }
            recordFound = true;
            break;
        }
    }
    if (!recordFound) {
        Log::warn(TRACE_DNS_MANAGER, "warning, record not found for request %s", request->mParams.mHostName.c_str());
    }

    // remove request from client
    Requests::iterator rqit = std::find(request->mClient->mRequests.begin(), request->mClient->mRequests.end(), request);
    if (rqit != request->mClient->mRequests.end()) {
        request->mClient->mRequests.erase(rqit);
    } else {
        Log::warn(TRACE_DNS_MANAGER, "warning, request %s not found in client", request->mParams.mHostName.c_str());
    }
}

const RequestParams & requestGetParams(const RequestPtr request)
{
    ASSERT(request, "null request");

    DNSMANAGER_DEBUG_NTRACE("%s - requestGetParams() by %s/%p", request->mParams.mHostName.c_str(),
                            request->mClient->mName.c_str(), request->mClient.get());

    return request->mParams;
}

void requestSetUserData(RequestPtr request, void * userData)
{
    ASSERT(request, "null request");

    DNSMANAGER_DEBUG_NTRACE("%s - requestSetUserData() by %s/%p", request->mParams.mHostName.c_str(),
                            request->mClient->mName.c_str(), request->mClient.get());

    request->mUserData = userData;
}

void * requestGetUserData(const RequestPtr request)
{
    ASSERT(request, "null request");

    DNSMANAGER_DEBUG_NTRACE("%s - requestGetUserData() by %s/%p", request->mParams.mHostName.c_str(),
                            request->mClient->mName.c_str(), request->mClient.get());

    return request->mUserData;
}

const AseTimeVal & requestGetTime(const RequestPtr request)
{
    ASSERT(request, "null request");

    DNSMANAGER_DEBUG_NTRACE("%s - requestGetLookupTime() by %s", request->mParams.mHostName.c_str(),
                            request->mClient->mName.c_str());

    return request->mTime;
}

RequestResultPtr requestGetResult(const RequestPtr request)
{
    ASSERT(request, "null request");

    DNSMANAGER_DEBUG_NTRACE("%s - requestGetResult() by %s/%p", request->mParams.mHostName.c_str(),
                            request->mClient->mName.c_str(), request->mClient.get());

    checkConsecutiveFailures();

    return request->mResult;
}

RequestResultPtr requestTryGetResult(const RequestParams & requestParams)
{
    DNSMANAGER_DEBUG_NTRACE("%s - requestTryGetResult()", requestParams.mHostName.c_str());

    RequestResultPtr result = resolveIpLiteral(requestParams);
    if (result || !sUsable)
        return result;

    RecordPtr record = obtainRecord(requestParams, RequestPtr());

    DNSMANAGER_DEBUG_NTRACE("%s - existing result %s", requestParams.mHostName.c_str(),
                            resultIsUsable(record->mResult) ? "usable" : "not usable");

    // update stats
    if (resultIsUsable(record->mResult))
        ++sCacheHits;
    else
        ++sCacheMisses;

    checkConsecutiveFailures();

    return record->mResult;
}

bool resultIsUsable(const RequestResultPtr result)
{
    if (!result) {
        DNSMANAGER_DEBUG_NTRACE("resultIsUsable() input result is null");
        return false;
    }

    bool ret = true;

    if (result->mErrorCode != AS_NO_ERROR)
        ret = false;

    if (result->mAddresses.empty())
        ret = false;

    DNSMANAGER_DEBUG_NTRACE("resultIsUsable() returns %s", ret ? "true" : "false");

    return ret;
}

bool resultIsExpired(const RequestResultPtr result)
{
    ASSERT(result, "null result")

    //DNSMANAGER_DEBUG_NTRACE("resultIsExpired()");

    AseTimeVal timeNow(AseTimeVal::now());
    AseTimeVal timeResultExpiration(result->mTime + result->mTtl + AseTimeVal::fromSeconds(1));
    return timeNow > timeResultExpiration;
}

void setGroupName(const std::string &groupName)
{
    sGroupName = groupName;
}

void logConfiguration()
{
    NTRACE(TRACE_DNS_MANAGER, "logConfiguration()");

    ScopedMutex instanceLock(sInstanceMutex);

    Variant config;
    config["usable"] = sUsable;
    config["dplf"] = sConfig.mParallelLookups;
    config["dlt"] = sConfig.mLookupTimeout.ms();
    config["dlr"] = sConfig.mLookupRetries;
    config["dcel"] = sConfig.mCacheMaxEntries;
    config["dct"] = sConfig.mCacheMaxTime.ms();
    config["dmt"] = sConfig.mTtlMinTime.ms();
    config["dllf"] = sConfig.mLogLookups;
    config["dlqf"] = sConfig.mLogQueries;
    if (!sConfig.mServers.empty())
        config["srv"] = sConfig.mServers;
    if (!sConfig.mFallbackServers.empty())
        config["dfs"] = sConfig.mFallbackServers;
    config["dfd"] = sConfig.mFallbackDelay.ms();
    if (!sConfig.mPrefetchHosts.empty())
        config["dph"] = sConfig.mPrefetchHosts;
    config["dcfl"] = sConfig.mConsecutiveFailuresLimit;
    if (sGroupName.empty())
        config["groupname"] = "control";
    else
        config["groupname"] = sGroupName;

    Log::Message msg(TRACE_DNS_MANAGER, Log::Info, "dnsconfig");
    msg.m_message = config.toJSON(false);
    msg.m_flags |= Log::Message::Critical;
    msg.send();
}

void logStats(bool reset)
{
    DNSMANAGER_DEBUG_NTRACE("logStats()");

    ScopedMutex instanceLock(sInstanceMutex);

    Variant channels;
    for (Channels::const_iterator cit = sChannels.begin(); cit != sChannels.end(); ++cit) {
        const ChannelPtr channel = *cit;

        Variant channelStats;
        channelStats["nam"] = channel->mName;
        channelStats["sr"] = channel->mSuccessfulResponses;
        channelStats["er"] = channel->mErrorResponses;
        channelStats["tr"] = channel->mTimeoutResponses;
        channelStats["wr"] = channel->mWinningResponses;
        unsigned int totalResponses = channel->mSuccessfulResponses + channel->mErrorResponses + channel->mTimeoutResponses;
        if (totalResponses)
            channelStats["art"] = channel->mTotalResponseTime.ms() / totalResponses;
        channels.push_back(channelStats);

        if (reset) {
            channel->mSuccessfulResponses = 0;
            channel->mErrorResponses = 0;
            channel->mTimeoutResponses = 0;
            channel->mWinningResponses = 0;
            channel->mTotalResponseTime = AseTimeVal(0);
        }
    }

    Variant stats;
    stats["cah"] = sCacheHits;
    stats["cam"] = sCacheMisses;
    stats["chn"] = channels;
    stats["sr"] = sSuccessfulResponses;
    stats["er"] = sErrorResponses;
    stats["tr"] = sTimeoutRespones;
    unsigned int totalResponses = sSuccessfulResponses + sErrorResponses + sTimeoutRespones;
    if (totalResponses)
        stats["art"] = sTotalResponseTime.ms() / totalResponses;

    if (reset) {
        sCacheHits = 0;
        sCacheMisses = 0;
        sSuccessfulResponses = 0;
        sErrorResponses = 0;
        sTimeoutRespones = 0;
        sTotalResponseTime = AseTimeVal(0);
    }

    Log::Message msg(TRACE_DNS_MANAGER, Log::Info, "dnsstats");
    msg.m_message = stats.toJSON(false);
    msg.m_flags |= Log::Message::Critical;
    msg.send();
}

void restartResolver()
{
    NTRACE(TRACE_DNS_MANAGER, "restartResolver()");

    // do partial shutdown

    bool workerPaused = false;
    if (sWorkerThread) {
        sWorkerThread->pause();
        workerPaused = true;
    }

    logStats();

    ScopedMutex instanceLock(sInstanceMutex);
    ScopedMutex clientsLock(sClientsMutex);
    ScopedMutex recordsLock(sRecordsMutex);

    // signal clients of any pending requests
    for (Records::iterator rcit = sRecords.begin(); rcit != sRecords.end(); ++rcit) {
        RecordPtr record = *rcit;
        for (Requests::iterator rqit = record->mRequests.begin(); rqit != record->mRequests.end(); ++rqit) {
            RequestPtr request = *rqit;
            if (!request->mSignaled) {
                NTRACE(TRACE_DNS_MANAGER, "%s - restartResolver(), signaling client %s/%p", request->mParams.mHostName.c_str(),
                    request->mClient->mName.c_str(), request->mClient.get());
                request->mClient->setSignal();

                // update request stats
                ++sErrorResponses;
                AseTimeVal responseTime = AseTimeVal::now() - request->mTime;
                sTotalResponseTime += responseTime;
            }
        }
    }

    DNSMANAGER_DEBUG_NTRACE("destroying %zu channels", sChannels.size());
    sChannels.clear();

    // this is probably not necessary, but do it just to be safe
    DNSMANAGER_DEBUG_NTRACE("destroying ares");
    if (sAresInitialized) {
        ares_library_cleanup();
        sAresInitialized = false;
    }

    sConsecutiveFailures = 0;
    sUsable = false;

    // put all records/queries back to initial state so they will be re-resolved
    for (Records::iterator rcit = sRecords.begin(); rcit != sRecords.end(); ++rcit) {
        RecordPtr record = *rcit;
        record->mState = Record::STATE_WAITING;
        record->mProcessingCount = 0;
        record->mResult.reset();
        record->mNewResult.reset();
        record->mFirstFallbackQuery = 0;
        record->mLastLookupTime = AseTimeVal::now();
        for (Requests::iterator rqit = record->mRequests.begin(); rqit != record->mRequests.end(); ++rqit) {
            RequestPtr request = *rqit;
            request->mResult.reset();
            request->mSignaled = false;
        }
    }

    // restart
    create();

    if (workerPaused)
        sWorkerThread->resume();
}

std::vector<std::string> aresDefaultServers()
{
    DNSMANAGER_DEBUG_NTRACE("aresDefaultServers()");

    std::vector<std::string> servers;

    ares_channel aresChannel;
    if (ares_init(&aresChannel) == ARES_SUCCESS) {
        struct ares_addr_node * aresAddrNodes;
        if (ares_get_servers(aresChannel, &aresAddrNodes) == ARES_SUCCESS) {
            NTRACE(TRACE_DNS_MANAGER, "no servers configured, using defaults:");
            for (struct ares_addr_node * aresAddrNode = aresAddrNodes; aresAddrNode; aresAddrNode = aresAddrNode->next) {
                if (aresAddrNode->family == AF_INET) {
                    NrdIpAddr nrdIpAddr(aresAddrNode->addr.addr4);
                    std::string dnsServer(nrdIpAddr.getStrAddr());
                    NTRACE(TRACE_DNS_MANAGER, "%s", dnsServer.c_str());
                    servers.push_back(dnsServer);
                }
#ifdef NRDP_HAS_IPV6
                else {
                    in6_addr in6Addr;
                    memcpy(in6Addr.s6_addr, aresAddrNode->addr.addr6._S6_un._S6_u8, 16);
                    NrdIpAddr nrdIpAddr(in6Addr);
                    std::string dnsServer(nrdIpAddr.getStrAddr());
                    NTRACE(TRACE_DNS_MANAGER, "%s", dnsServer.c_str());
                    servers.push_back(dnsServer);
                }
#endif
            }

            ares_free_data(aresAddrNodes);
        }

        ares_destroy(aresChannel);
    }

    return servers;
}

RecordPtr obtainRecord(const RequestParams & requestParams, RequestPtr request)
{
    DNSMANAGER_DEBUG_NTRACE("%s - obtainRecord()", requestParams.mHostName.c_str());

    ASSERT(!resolveIpLiteral(requestParams), "attempt to create record for IP literal");

    ScopedMutex recordsLock(sRecordsMutex);

    AseTimeVal timeNow(AseTimeVal::now());

    RecordPtr record;

    // try to find matching existing record
    for (Records::iterator trit = sRecords.begin(); trit != sRecords.end(); ++trit) {
        RecordPtr tempRecord = *trit;
        if (tempRecord->mParams == requestParams) {
            record = tempRecord;
            sRecords.erase(trit); // remove temporarily, will be added back to end below (keeps sorted by LRU)
            break;
        }
    }

    if (!record) {
        // check if cache is full
        if (sRecords.size() >= sConfig.mCacheMaxEntries) {
            Records::iterator trit = sRecords.begin();
            while (trit != sRecords.end()) {
                RecordPtr tempRecord = *trit;
                if (!tempRecord->mRequests.empty() || (tempRecord->mState != Record::STATE_COMPLETE)) {
                    ++trit;
                } else {
                    NTRACE(TRACE_DNS_MANAGER, "%s - evicting from cache to make room for new entry, %zu entries, max %u",
                           tempRecord->mParams.mHostName.c_str(), sRecords.size(), sConfig.mCacheMaxEntries);
                    trit = sRecords.erase(trit);
                }

                if (sRecords.size() < sConfig.mCacheMaxEntries)
                    break;
            }
        }

        NTRACE(TRACE_DNS_MANAGER, "%s - no existing entry, creating one", requestParams.mHostName.c_str());
        record.reset(new Record);
        record->mParams = requestParams;
    }

    record->mLastLookupTime = timeNow;
    sRecords.push_back(record);

    if (request) {
        ASSERT(record->mRequests.size() < 256, "sanity check for number of requests per record exceeded");
        record->mRequests.push_back(request);
        request->mClient->mRequests.push_back(request);
    }

    // check the existing records
    if (resultIsUsable(record->mResult)) {
        NTRACE(TRACE_DNS_MANAGER, "%s - usable result available", requestParams.mHostName.c_str());
        if (request) {
            request->mResult = record->mResult;
            if (!request->mSignaled) {
                request->mSignaled = true;
                request->mClient->setSignal();

                // update request stats
                ++sSuccessfulResponses;
                AseTimeVal responseTime = AseTimeVal::now() - request->mTime;
                sTotalResponseTime += responseTime;
            }
        }

        if (resultIsExpired(record->mResult) && (record->mState == Record::STATE_COMPLETE)) {
            NTRACE(TRACE_DNS_MANAGER, "%s - result expired, signaling worker thread to renew",
                   requestParams.mHostName.c_str());
            record->mState = Record::STATE_WAITING;
            sWorkerThread->setSignal();
        }
    } else {
        NTRACE(TRACE_DNS_MANAGER, "%s - no result currently available", requestParams.mHostName.c_str());

        if (record->mState != Record::STATE_PROCESSING) {
            DNSMANAGER_DEBUG_NTRACE("%s - signaling worker thread", record->mParams.mHostName.c_str());
            record->mState = Record::STATE_WAITING;
            sWorkerThread->setSignal();
        }
    }
    return record;
}

std::vector<std::string> currentRecords()
{
    ScopedMutex recordsLock(sRecordsMutex);

    // try to find matching existing record
    std::vector<std::string> ret;
    ret.reserve(sRecords.size());
    for (Records::iterator trit = sRecords.begin(); trit != sRecords.end(); ++trit) {
        ret.push_back((*trit)->toString());
    }
    return ret;
}

RequestResultPtr resolveIpLiteral(const RequestParams & requestParams)
{
    DNSMANAGER_DEBUG_NTRACE("%s - resolveIpLiteral()", requestParams.mHostName.c_str());

    // check if the host name is an IP literal.
    NrdIpAddr nrdIpAddr(requestParams.mHostName);
    if (nrdIpAddr.valid()) {
        if ((requestParams.mAddressType == ADDRESSTYPE_IPV4) && (nrdIpAddr.getVersion() == NrdIpAddr::IP_V4)) {
            NTRACE(TRACE_DNS_MANAGER, "%s - resolved as IP literal", requestParams.mHostName.c_str());
            RequestResultPtr result(new RequestResult);
            result->mErrorCode = AS_NO_ERROR;
            result->mTime = AseTimeVal::now();
            result->mAddressType = ADDRESSTYPE_IPV4;
            result->mAddresses.push_back(nrdIpAddr);
            result->mTtl = 0;
            return result;
        }

#ifdef NRDP_HAS_IPV6
        if ((requestParams.mAddressType == ADDRESSTYPE_IPV6) && (nrdIpAddr.getVersion() == NrdIpAddr::IP_V6)) {
            NTRACE(TRACE_DNS_MANAGER, "%s - resolved as IP literal", requestParams.mHostName.c_str());
            RequestResultPtr result(new RequestResult);
            result->mErrorCode = AS_NO_ERROR;
            result->mTime = AseTimeVal::now();
            result->mAddressType = ADDRESSTYPE_IPV6;
            result->mAddresses.push_back(nrdIpAddr);
            result->mTtl = 0;
            return result;
        }
#endif
    }

    // not resolved, return null
    return RequestResultPtr();
}

RequestResultPtr resolveHostsFile(const RequestParams & requestParams)
{
    DNSMANAGER_DEBUG_NTRACE("%s - resolveHostsFile()", requestParams.mHostName.c_str());

    // use fake channel to avoid instance lock
    // ares doesn't use this for hosts file lookup, except as an initialization check
    ares_channel channel = (ares_channel)~0;

    // check if the host name can be looked up locally.
    if (requestParams.mAddressType == ADDRESSTYPE_IPV4) {
        struct hostent * hostEntry = 0;
        if (ares_gethostbyname_file(channel, requestParams.mHostName.c_str(), AF_INET, &hostEntry) == ARES_SUCCESS) {
            NTRACE(TRACE_DNS_MANAGER, "%s - resolved using hosts file", requestParams.mHostName.c_str());
            NrdIpAddr nrdIpAddr(*(struct in_addr *)(hostEntry->h_addr));
            RequestResultPtr result(new RequestResult);
            result->mErrorCode = AS_NO_ERROR;
            result->mTime = AseTimeVal::now();
            result->mAddressType = ADDRESSTYPE_IPV4;
            result->mAddresses.push_back(nrdIpAddr);
            result->mTtl = AseTimeVal(0);
            ares_free_hostent(hostEntry);
            return result;
        }
    }

#ifdef NRDP_HAS_IPV6
    if (requestParams.mAddressType == ADDRESSTYPE_IPV6) {
        struct hostent * hostEntry = 0;
        if (ares_gethostbyname_file(channel, requestParams.mHostName.c_str(), AF_INET6, &hostEntry) == ARES_SUCCESS) {
            NTRACE(TRACE_DNS_MANAGER, "%s - resolved using hosts file", requestParams.mHostName.c_str());
            NrdIpAddr nrdIpAddr(*(struct in6_addr *)hostEntry->h_addr);
            RequestResultPtr result(new RequestResult);
            result->mErrorCode = AS_NO_ERROR;
            result->mTime = AseTimeVal::now();
            result->mAddressType = ADDRESSTYPE_IPV6;
            result->mAddresses.push_back(nrdIpAddr);
            result->mTtl = AseTimeVal(0);
            ares_free_hostent(hostEntry);
            return result;
        }
    }
#endif

    // not resolved, return null
    return RequestResultPtr();
}

void checkConsecutiveFailures()
{
    if (!sConfig.mConsecutiveFailuresLimit)
        return;

    sRecordsMutex.lock();
    unsigned int recordCount = sRecords.size();
    sRecordsMutex.unlock();

    if (sConsecutiveFailures >= (sConfig.mConsecutiveFailuresLimit * recordCount)) {
        Log::error(TRACE_DNS_MANAGER, "restarting after %u consecutive failures (%u records)", sConsecutiveFailures, recordCount);
        restartResolver();
    }
}

Channel::Channel() :
    mAresChannel(0),
    mIsFallback(false),
    mSuccessfulResponses(0),
    mErrorResponses(0),
    mTimeoutResponses(0),
    mWinningResponses(0)
{
}

Channel::~Channel()
{
    DNSMANAGER_DEBUG_NTRACE("destroying channel %s", mName.c_str());
    if (mAresChannel) {
        ares_cancel(mAresChannel);
        ares_destroy(mAresChannel);
    }
}

int Channel::create(const std::string & server)
{
    std::vector<std::string> servers;
    servers.push_back(server);
    return create(servers);
}

int Channel::create(const std::vector<std::string> & servers)
{
    ASSERT(!mAresChannel, "channel already created");

    DNSMANAGER_DEBUG_NTRACE("Channel::create()");

    if (servers.empty() && !sConfig.mUseDefaultServers)
        return ARES_ESERVFAIL;

    int retVal = ARES_SUCCESS;

    // initialize c-ares resolver channel

    ares_options aresOptions;
    int aresOptionsMask = 0;

    aresOptions.flags =
        ARES_FLAG_STAYOPEN; // don't close socket when idle
    aresOptionsMask |= ARES_OPT_FLAGS;

    if (sConfig.mLookupTimeout.ms()) {
        aresOptions.timeout = sConfig.mLookupTimeout.ms();
        aresOptionsMask |= ARES_OPT_TIMEOUTMS;
    }

    if (sConfig.mLookupRetries) {
        aresOptions.tries = sConfig.mLookupRetries;
        aresOptionsMask |= ARES_OPT_TRIES;
    }

    retVal = ares_init_options(&mAresChannel, &aresOptions, aresOptionsMask);

    if (retVal != ARES_SUCCESS)
        return retVal;

    IpConnectivityManager::Policy ipConnectivityPolicy = IpConnectivityManager::instance()->getIpConnectivityPolicy();

    if (servers.empty()) {
        // no servers specified, synthesize a name for the default servers
        mName += "default(";
        std::vector<std::string> defaultServers = aresDefaultServers();
        for (std::vector<std::string>::const_iterator sit = defaultServers.begin(); sit != defaultServers.end(); ++sit) {
            std::string server = *sit;
            if (sit != defaultServers.begin())
                mName += ';';

            NrdIpAddr nrdIpAddr(*sit);
            if ((NrdIpAddr::IP_V4 == nrdIpAddr.getVersion() && (ipConnectivityPolicy != IpConnectivityManager::IP_V6_ONLY)) ||
                (NrdIpAddr::IP_V6 == nrdIpAddr.getVersion() && (ipConnectivityPolicy != IpConnectivityManager::IP_V4_ONLY))) {
                mName += server;
            }
        }
        mName += ")";
    } else {
        // one or more server specified, synthesize a name for those and tell ares to use them
        if (servers.size() > 1)
            mName += "(";

        std::vector<struct ares_addr_node > aresAddrNodes;
        aresAddrNodes.resize(servers.size());
        int i = 0;
        for (std::vector<std::string>::const_iterator sit = servers.begin(); sit != servers.end(); sit++, i++) {
            std::string server = *sit;

            if (sit != servers.begin())
                mName += ';';

            // allocate struct ares_addr_node
            NrdIpAddr nrdIpAddr(*sit);
            aresAddrNodes[i].next = (i > 0) ? &aresAddrNodes[i - 1] : 0;
            if (NrdIpAddr::IP_V4 == nrdIpAddr.getVersion() && (ipConnectivityPolicy != IpConnectivityManager::IP_V6_ONLY)) {
                // if IPv4
                DNSMANAGER_DEBUG_NTRACE("IPv4 DNS servers provided %s\n", (*sit).c_str());
                struct in_addr inAddr = nrdIpAddr.getInAddr();
                aresAddrNodes[i].family = AF_INET;
                memcpy(&(aresAddrNodes[i].addr.addr4), &inAddr, sizeof(struct in_addr));
                mName += server;
            }
#ifdef NRDP_HAS_IPV6
            else if (NrdIpAddr::IP_V6 == nrdIpAddr.getVersion() && (ipConnectivityPolicy != IpConnectivityManager::IP_V4_ONLY)) {
                // if IPv6
                DNSMANAGER_DEBUG_NTRACE("IPv6 DNS servers provided %s\n", (*sit).c_str());
                struct in6_addr in6Addr = nrdIpAddr.getIn6Addr();
                aresAddrNodes[i].family = AF_INET6;
                memcpy(&(aresAddrNodes[i].addr.addr6), &in6Addr, sizeof(struct ares_in6_addr));
                mName += server;
            }
#endif
        }
        if (servers.size() > 1)
            mName += ")";

        if (aresAddrNodes.empty()) {
            Log::error(TRACE_DNS_MANAGER, "no usable servers found (IPV4/IPVG mismatch?)");
            return ARES_ENODATA;
        }

        // ares_set_servers only when there are/is servers
        retVal = ares_set_servers(mAresChannel, &aresAddrNodes[i-1]);
    }

    return retVal;
}

Query::Query() :
    mAddressType(ADDRESSTYPE_NONE)
{
}

Query::~Query()
{
    //DNSMANAGER_DEBUG_NTRACE("%s - destroying query, server %s, addrtype %s",
    //                        mRecord ? mRecord->mParams.mHostName.c_str() : "no record",
    //                        mChannel ? mChannel->mName.c_str() : "no channel",
    //                        (mAddressType == ADDRESSTYPE_IPV4) ? "ipv4" : "ipv6");
}

Client::Client(const std::string & name) :
    mName(name)
{
    const bool val = createPipe(mSignalPipe);
    NRDP_UNUSED(val);
    ASSERT(val && (mSignalPipe[0] >= 0) && (mSignalPipe[1] >= 0), "pipe creation failed");
}

Client::~Client()
{
    DNSMANAGER_DEBUG_NTRACE("destroying client %s", mName.c_str());

    while (!mRequests.empty())
        requestRemove(mRequests.front());

    if (mSignalPipe[0] >= 0)
        close(mSignalPipe[0]);
    if (mSignalPipe[1] >= 0)
        close(mSignalPipe[1]);
}

void Client::setSignal()
{
    ssize_t ret = 0;
    if (mSignalPipe[1] >= 0) {
        DNSMANAGER_DEBUG_NTRACE("signaling client %s/%p", mName.c_str(), this);
        char signalByte = 1;
        EINTRWRAP(ret, write(mSignalPipe[1], &signalByte, sizeof(signalByte)));
    }
}

void Client::clearSignal()
{
    if (mSignalPipe[0]) {
        const bool ret = clearPipe(mSignalPipe[0]);
        DNSMANAGER_DEBUG_NTRACE("clearing signal client %s/%p/%d", mName.c_str(), this, ret);
        (void)ret;
        assert(ret && "Failure when clearing DNS Manager client pipe");
    }
}

Record::Record() :
    mState(STATE_WAITING),
    mProcessingCount(0)
{
}

Record::~Record()
{
    DNSMANAGER_DEBUG_NTRACE("%s - destroying record, requests %zu, queries %zu",
                            mParams.mHostName.c_str(), mRequests.size(), mQueries.size());
}

std::string Record::toString() const
{
    const char * state = 0;
    switch (mState) {
        case STATE_WAITING: state = "waiting"; break;
        case STATE_PROCESSING: state = "processing"; break;
        case STATE_COMPLETE: state = "complete"; break;
    }

    const char *requestType = 0;
    switch (mParams.mAddressType) {
        case ADDRESSTYPE_IPV4: requestType = "(4)"; break;
#ifdef NRDP_HAS_IPV6
        case ADDRESSTYPE_IPV6: requestType = "(6)"; break;
#endif
        case ADDRESSTYPE_NONE: requestType = ""; break;
        default: ASSERT(0, "unknown address type %d", mParams.mAddressType); break;
    }

    return StringFormatter::sformat("State: %s Host: %s%s Requests: %zu Queries: %zu Processing: %u Age: %llu Result: %s New result: %s",
                                    state, mParams.mHostName.c_str(), requestType, mRequests.size(), mQueries.size(), mProcessingCount,
                                    (AseTimeVal::now() - mLastLookupTime).ms(), resultToString(mResult).c_str(), resultToString(mNewResult).c_str());
}

Request::Request() :
    mUserData(0),
    mSignaled(false)
{
}

Request::~Request()
{
    DNSMANAGER_DEBUG_NTRACE("%s - destroying request, addrtype %s, client %s/%p",
                            mParams.mHostName.c_str(),
                            (mParams.mAddressType == ADDRESSTYPE_IPV4) ? "ipv4" : "ipv6",
                            mClient ? mClient->mName.c_str() : "no client",
                            mClient.get());
    //ASSERT(mSignaled, "request not signaled before destruction");
}

WorkerThread::WorkerThread() :
    Thread(&THREAD_DNS_MANAGER),
    mShutdown(false),
    mPauseState(PS_PAUSED),
    mMutex(ZERO_MUTEX, "DnsManager::WorkerThread::mMutex")
{
    const bool val = createPipe(mSignalPipe);
    NRDP_UNUSED(val);
    ASSERT(val && (mSignalPipe[0] >= 0) && (mSignalPipe[1] >= 0), "pipe creation failed");
}

WorkerThread::~WorkerThread()
{
    DNSMANAGER_DEBUG_NTRACE("destroying worker thread");
    {
        ScopedMutex lock(mMutex);
        mCondition.broadcast();
        mShutdown = true;
    }
    setSignal();
    Wait();

    if (mSignalPipe[0] >= 0)
        close(mSignalPipe[0]);
    if (mSignalPipe[1] >= 0)
        close(mSignalPipe[1]);
}

void WorkerThread::setSignal()
{
    ssize_t ret;
    if (mSignalPipe[1] >= 0) {
        char signalByte = 1;
        EINTRWRAP(ret, write(mSignalPipe[1], &signalByte, sizeof(signalByte)));
    }
}

void WorkerThread::pause()
{
    NTRACE(TRACE_DNS_MANAGER, "pause()");
    setPaused(true);
    NTRACE(TRACE_DNS_MANAGER, "pause complete");
}

void WorkerThread::resume()
{
    NTRACE(TRACE_DNS_MANAGER, "resume()");
    setPaused(false);
    NTRACE(TRACE_DNS_MANAGER, "resume complete");
}

void WorkerThread::setPaused(bool pause)
{
    ScopedMutex lock(mMutex);
    const PauseState targetState = pause ? PS_PAUSED : PS_RUNNING;
    if (mPauseState == targetState)
        return;
    mPauseState = pause ? PS_PAUSE_REQUESTED : PS_RESUME_REQUESTED;
    mCondition.broadcast();
    setSignal();
    while (mPauseState != targetState && !mShutdown)
        mCondition.wait(&mMutex);
}

bool WorkerThread::waitForResume()
{
    ScopedMutex lock(mMutex);
    if (mShutdown)
        return false;
    assert(mPauseState != PS_PAUSED);
    if (mPauseState != PS_PAUSE_REQUESTED)
        return true;
    mPauseState = PS_PAUSED;
    mCondition.broadcast();
    while (mPauseState != PS_RESUME_REQUESTED && !mShutdown)
        mCondition.wait(&mMutex);
    if (mShutdown)
        return false;
    mPauseState = PS_RUNNING;
    mCondition.broadcast();
    return true;
}

void WorkerThread::Run()
{
    NTRACE(TRACE_DNS_MANAGER, "Run()");

    int retVal;

    {
        ScopedMutex lock(mMutex);
        mPauseState = PS_RUNNING;
    }

    while (waitForResume()) {
        PERFORMANCE_MARKER_SCOPED("dnsmanager.loop");

        AseTimeVal nextFallbackTime(~(ullong)0);

        sRecordsMutex.lock();

        // issue queries for waiting records
        Records::iterator rcit = sRecords.begin();
        while (rcit != sRecords.end()) {
            RecordPtr record = *rcit;

            // check to see if we should use any existing fallback results
            if (record->mState == Record::STATE_PROCESSING) {
                if (!record->mNewResult && record->mFirstFallbackQuery) {
                    AseTimeVal fallbackWaited(AseTimeVal::now() - record->mFirstFallbackQuery->mResult.mTime);
                    if (fallbackWaited < sConfig.mFallbackDelay) {
                        AseTimeVal fallbackTime(sConfig.mFallbackDelay - fallbackWaited);
                        if (fallbackTime < nextFallbackTime)
                            nextFallbackTime = fallbackTime;
                    } else {
                        NTRACE(TRACE_DNS_MANAGER, "%s - using fallback result, waited %llu ms, waking %zu %s",
                               record->mParams.mHostName.c_str(), fallbackWaited.ms(),
                               record->mRequests.size(), (record->mRequests.size() == 1) ? "requestor" : "requestors");

                        record->mNewResult.reset(new RequestResult);
                        *record->mNewResult = record->mFirstFallbackQuery->mResult;
                        record->mResult = record->mNewResult;
                        ++record->mFirstFallbackQuery->mChannel->mWinningResponses;

                        for (Requests::iterator rqit = record->mRequests.begin(); rqit != record->mRequests.end(); ++rqit) {
                            RequestPtr request = *rqit;

                            request->mResult = record->mResult;
                            if (!request->mSignaled) {
                                request->mSignaled = true;
                                request->mClient->setSignal();

                                // update request stats
                                ++sSuccessfulResponses;
                                AseTimeVal responseTime = AseTimeVal::now() - request->mTime;
                                sTotalResponseTime += responseTime;
                            }
                        }
                    }
                }

                ++rcit;
                continue;
            }

            // check to see if the cache needs maintenance
            if (record->mState == Record::STATE_COMPLETE) {
                if (sRecords.size() > sConfig.mCacheMaxEntries) {
                    if (record->mRequests.empty()) {
                        NTRACE(TRACE_DNS_MANAGER, "%s - evicting from cache, %zu entries, max %u",
                               record->mParams.mHostName.c_str(), sRecords.size(), sConfig.mCacheMaxEntries);
                        rcit = sRecords.erase(rcit);
                        continue;
                    }
                } else if (!record->mResult || resultIsExpired(record->mResult)) {
                    // FIX - scan other results too
                    AseTimeVal recordAge(AseTimeVal::now() - record->mLastLookupTime);
                    if (recordAge < sConfig.mCacheMaxTime) {
                        NTRACE(TRACE_DNS_MANAGER, "%s - renewing, age is %llu seconds (max %llu)",
                               record->mParams.mHostName.c_str(), recordAge.seconds(), sConfig.mCacheMaxTime.seconds());
                        record->mState = Record::STATE_WAITING;
                    } else if (record->mRequests.empty()) {
                        NTRACE(TRACE_DNS_MANAGER, "%s - evicting from cache, age is %llu seconds (max %llu)",
                               record->mParams.mHostName.c_str(), recordAge.seconds(), sConfig.mCacheMaxTime.seconds());
                        rcit = sRecords.erase(rcit);
                        continue;
                    }
                }
            }

            if (record->mState != Record::STATE_WAITING) {
                ++rcit;
                continue;
            }

            RequestResultPtr requestResultPtr = resolveHostsFile(record->mParams);
            if (requestResultPtr) {
                NTRACE(TRACE_DNS_MANAGER, "%s - using local lookup", record->mParams.mHostName.c_str());

                record->mState = Record::STATE_COMPLETE;
                record->mResult = requestResultPtr;
                record->mNewResult = requestResultPtr;
                record->mResult = record->mNewResult;

                for (Requests::iterator rcsit = record->mRequests.begin(); rcsit != record->mRequests.end(); ++rcsit) {
                    RequestPtr request = *rcsit;

                    request->mResult = record->mResult;
                    if (!request->mSignaled) {
                        request->mSignaled = true;
                        request->mClient->setSignal();

                        // update request stats
                        ++sSuccessfulResponses;
                        AseTimeVal responseTime = AseTimeVal::now() - request->mTime;
                        sTotalResponseTime += responseTime;
                    }
                }

                ++rcit;
                continue;
            }

            DNSMANAGER_DEBUG_NTRACE("%s - initiating queries", record->mParams.mHostName.c_str());

            ASSERT(!record->mProcessingCount, "unexpected processing count %u", record->mProcessingCount);
            record->mState = Record::STATE_PROCESSING;
            record->mQueries.clear();
            record->mNewResult.reset();
            record->mFirstFallbackQuery = 0;

            for (Channels::iterator cit = sChannels.begin(); cit != sChannels.end(); ++cit) {
                ChannelPtr channel = *cit;

                QueryPtr query(new Query);
                query->mChannel = channel.get();
                query->mRecord = record.get();
                query->mAddressType = query->mResult.mAddressType = record->mParams.mAddressType;
                query->mStartTime = AseTimeVal::now();
                query->mResult.mErrorCode = AS_DNS_NO_DATA;
                record->mQueries.push_back(query);
                ++record->mProcessingCount;
            }

            for (Record::Queries::iterator qit = record->mQueries.begin(); qit != record->mQueries.end(); ++qit) {
                QueryPtr query = *qit;

                // these queries can potentially trigger the callback immediately
                if (query->mAddressType == ADDRESSTYPE_IPV4)
                    ares_query(query->mChannel->mAresChannel, record->mParams.mHostName.c_str(), C_IN, T_A, aresLookupCallback, query.get());
#ifdef NRDP_HAS_IPV6
                else if (query->mAddressType == ADDRESSTYPE_IPV6)
                    ares_query(query->mChannel->mAresChannel, record->mParams.mHostName.c_str(), C_IN, T_AAAA, aresLookupCallback, query.get());
#endif
                else {
                    ASSERT(0, "unknown address type %d", query->mAddressType);
                }
            }

            ++rcit;
        }

        sRecordsMutex.unlock();

        int nfds = -1;
        fd_set readFds;
        fd_set writeFds;
        FD_ZERO(&readFds);
        FD_ZERO(&writeFds);

        FD_SET(mSignalPipe[0], &readFds);
        nfds = mSignalPipe[0] + 1;

        for (Channels::iterator cit = sChannels.begin(); cit != sChannels.end(); ++cit) {
            ChannelPtr channel = *cit;

            retVal = ares_fds(channel->mAresChannel, &readFds, &writeFds);
            if (retVal > nfds)
                nfds = retVal;
        }

        struct timeval tv;
        tv.tv_sec = (nextFallbackTime.ms() < 1000) ? 0 : (time_t)1;
        tv.tv_usec = (nextFallbackTime.ms() < 1000) ? (nextFallbackTime.ms() * 1000) : (suseconds_t)0;

        retVal = select(nfds, &readFds, &writeFds, 0, &tv);

        {
            ScopedMutex lock(mMutex);
            if (mShutdown)
                break;
        }

        if (FD_ISSET(mSignalPipe[0], &readFds)) {
            bool ret = clearPipe(mSignalPipe[0]);
            assert(ret);
            (void)ret;
        }

        if (retVal < 0) {
            sleep(1);
        } else {
            ScopedMutex recordsLock(sRecordsMutex);

            for (Channels::iterator cit = sChannels.begin(); cit != sChannels.end(); ++cit) {
                ChannelPtr channel = *cit;

                ares_process(channel->mAresChannel, &readFds, &writeFds);
            }
        }
    }
}

static void aresLookupCallback(void * arg, int status, int timeouts, unsigned char * abuf, int alen)
{
    DNSMANAGER_DEBUG_NTRACE("aresLookupCallback()");

    NRDP_UNUSED(timeouts);

    AseTimeVal timeNow = AseTimeVal::now();

    Query * query = (Query *)arg;
    query->mResult.mTime = timeNow;
    AseTimeVal responseTime = timeNow - query->mStartTime;

    Record * record = query->mRecord;
    ASSERT(record->mProcessingCount > 0, "processing count underflow");
    --record->mProcessingCount;
    if (!record->mProcessingCount)
        record->mState = Record::STATE_COMPLETE;

    NTRACE(TRACE_DNS_MANAGER, "%s - response from %s took %llums, status %d, timeouts %d, completed %zu/%zu",
           record->mParams.mHostName.c_str(), query->mChannel->mName.c_str(), responseTime.ms(),
           status, timeouts, record->mQueries.size() - record->mProcessingCount, record->mQueries.size());

    if (status != ARES_SUCCESS) {
        NTRACE(TRACE_DNS_MANAGER, "%s - query failed with error '%s'",
               record->mParams.mHostName.c_str(), ares_strerror(status));
    } else {
        if (query->mAddressType == ADDRESSTYPE_IPV4) {
            struct hostent * host = 0;
            struct ares_addrttl addrTtls[64];
            int32_t addrCount = sizeof(addrTtls) / sizeof(addrTtls[0]);
            status = ares_parse_a_reply(abuf, alen, &host, addrTtls, &addrCount);

            if (status != ARES_SUCCESS) {
                NTRACE(TRACE_DNS_MANAGER, "%s - parse failed with error '%s'",
                       record->mParams.mHostName.c_str(), ares_strerror(status));
            } else {
                if (host->h_name) {
                    NTRACE(TRACE_DNS_MANAGER, "%s - name %s",
                           record->mParams.mHostName.c_str(), host->h_name);
                    query->mResult.mName = host->h_name;
                }

                for (unsigned int count = 0; host->h_aliases[count]; ++count) {
                    NTRACE(TRACE_DNS_MANAGER, "%s - alias %s",
                           record->mParams.mHostName.c_str(), host->h_aliases[count]);
                    query->mResult.mAliases.push_back(host->h_aliases[count]);
                }

                if (addrCount <= 0) {
                    NTRACE(TRACE_DNS_MANAGER, "%s - parsing successful, but no address found",
                           record->mParams.mHostName.c_str());
                    status = ARES_ENOTFOUND;
                } else {
                    query->mResult.mAddresses.clear();
                    unsigned int ttl = ~0U;

                    for (int32_t count = 0; count < addrCount; ++count) {
                        NrdIpAddr nrdIpAddr(addrTtls[count].ipaddr);

                        NTRACE(TRACE_DNS_MANAGER, "%s - IPv4 address %s, ttl %d",
                               record->mParams.mHostName.c_str(), nrdIpAddr.getStrAddr().c_str(),
                               addrTtls[count].ttl);

                        query->mResult.mAddresses.push_back(nrdIpAddr);
#ifdef NRDP_HAS_SOCKETIZER
                        Socketizer::instance()->setHostName(record->mParams.mHostName,
                                                            query->mResult.mName,
                                                            query->mResult.mAliases,
                                                            nrdIpAddr.getStrAddr());
#endif

                        if (ttl > (unsigned int)addrTtls[count].ttl)
                            ttl = addrTtls[count].ttl;
                    }

                    query->mResult.mTtl = AseTimeVal::fromSeconds(ttl);
                    if (query->mResult.mTtl < sConfig.mTtlMinTime) {
                        NTRACE(TRACE_DNS_MANAGER, "%s - overriding ttl with minimum of %llu",
                               record->mParams.mHostName.c_str(), sConfig.mTtlMinTime.seconds());
                        query->mResult.mTtl = sConfig.mTtlMinTime;
                    }

                    DNSMANAGER_DEBUG_NTRACE("%s - expiration %llu", record->mParams.mHostName.c_str(),
                                            (query->mResult.mTime + query->mResult.mTtl).seconds());
                }

                ares_free_hostent(host);
            }
        }
#ifdef NRDP_HAS_IPV6
        else if (query->mAddressType == ADDRESSTYPE_IPV6) {
            struct hostent * host = 0;
            struct ares_addr6ttl addrTtls[64];
            int32_t addrCount = sizeof(addrTtls) / sizeof(addrTtls[0]);
            status = ares_parse_aaaa_reply(abuf, alen, &host, addrTtls, &addrCount);

            if (status != ARES_SUCCESS) {
                NTRACE(TRACE_DNS_MANAGER, "%s - parse failed with error '%s'",
                       record->mParams.mHostName.c_str(), ares_strerror(status));
            } else {
                if (host->h_name) {
                    NTRACE(TRACE_DNS_MANAGER, "%s - name %s",
                           record->mParams.mHostName.c_str(), host->h_name);
                    query->mResult.mName = host->h_name;
                }

                for (unsigned int count = 0; host->h_aliases[count]; ++count) {
                    NTRACE(TRACE_DNS_MANAGER, "%s - alias %s",
                           record->mParams.mHostName.c_str(), host->h_aliases[count]);
                    query->mResult.mAliases.push_back(host->h_aliases[count]);
                }

                if (addrCount <= 0) {
                    NTRACE(TRACE_DNS_MANAGER, "%s - parsing successful, but no address found",
                           record->mParams.mHostName.c_str());
                    status = ARES_ENOTFOUND;
                } else {
                    query->mResult.mAddresses.clear();
                    unsigned int ttl = ~0U;

                    for (int32_t count = 0; count < addrCount; ++count) {
                        struct in6_addr in6Addr;
                        memcpy(in6Addr.s6_addr, addrTtls[count].ip6addr._S6_un._S6_u8, sizeof(in6Addr.s6_addr));
                        NrdIpAddr nrdIpAddr(in6Addr);

                        NTRACE(TRACE_DNS_MANAGER, "%s - IPV6 address %s, ttl %d",
                               record->mParams.mHostName.c_str(), nrdIpAddr.getStrAddr().c_str(),
                               addrTtls[count].ttl);

                        query->mResult.mAddresses.push_back(nrdIpAddr);
#ifdef NRDP_HAS_SOCKETIZER
                        Socketizer::instance()->setHostName(record->mParams.mHostName,
                                                            query->mResult.mName,
                                                            query->mResult.mAliases,
                                                            nrdIpAddr.getStrAddr());
#endif

                        if (ttl > (unsigned int)addrTtls[count].ttl)
                            ttl = addrTtls[count].ttl;
                    }

                    query->mResult.mTtl = AseTimeVal::fromSeconds(ttl);
                    if (query->mResult.mTtl < sConfig.mTtlMinTime) {
                        NTRACE(TRACE_DNS_MANAGER, "%s - overriding ttl with minimum of %llu",
                               record->mParams.mHostName.c_str(), sConfig.mTtlMinTime.seconds());
                        query->mResult.mTtl = sConfig.mTtlMinTime;
                    }

                    DNSMANAGER_DEBUG_NTRACE("%s - expiration %llu", record->mParams.mHostName.c_str(),
                                            (query->mResult.mTime + query->mResult.mTtl).seconds());
                }

                ares_free_hostent(host);
            }
        }
#endif
        else {
            ASSERT(0, "unknown address type %d", query->mAddressType);
            status = ARES_EBADRESP;
        }
    }

    query->mResult.mErrorCode = aresErrorToAseErrorCode(status);

    RequestResult * firstNewResult = 0;
    if (record->mNewResult) {
        NTRACE(TRACE_DNS_MANAGER, "%s - processed %llu ms after first result",
               record->mParams.mHostName.c_str(),
               (query->mResult.mTime - record->mNewResult->mTime).ms());
    } else {
        if (query->mResult.mErrorCode == AS_NO_ERROR) {
            if (!query->mChannel->mIsFallback) {
                NTRACE(TRACE_DNS_MANAGER, "%s - first valid result, waking %zu %s",
                       record->mParams.mHostName.c_str(), record->mRequests.size(),
                       (record->mRequests.size() == 1) ? "requestor" : "requestors");
                firstNewResult = &query->mResult;
                ++query->mChannel->mWinningResponses;
                NTRACEIF(sConsecutiveFailures, TRACE_DNS_MANAGER, "consecutive failures reset to 0");
                sConsecutiveFailures = 0;
            } else {
                if (record->mFirstFallbackQuery) {
                    NTRACE(TRACE_DNS_MANAGER, "%s - secondary fallback result, %llu ms after first result",
                           record->mParams.mHostName.c_str(),
                           (query->mResult.mTime - record->mFirstFallbackQuery->mResult.mTime).ms());
                } else {
                    NTRACE(TRACE_DNS_MANAGER, "%s - first fallback result",
                           record->mParams.mHostName.c_str());
                    record->mFirstFallbackQuery = query;

                    if (record->mState == Record::STATE_COMPLETE) {
                        NTRACE(TRACE_DNS_MANAGER, "%s - using fallback result, all other results failed",
                               record->mParams.mHostName.c_str());
                        firstNewResult = &query->mResult;
                        ++record->mFirstFallbackQuery->mChannel->mWinningResponses;
                    }
                }
            }
        } else if (record->mState == Record::STATE_COMPLETE) {
            // check to see if we should use any existing fallback results
            if (record->mFirstFallbackQuery) {
                NTRACE(TRACE_DNS_MANAGER, "%s - all lookups complete, using fallback result",
                       record->mParams.mHostName.c_str());
                firstNewResult = &record->mFirstFallbackQuery->mResult;
                ++record->mFirstFallbackQuery->mChannel->mWinningResponses;
            }

            if (!firstNewResult) {
                NTRACE(TRACE_DNS_MANAGER, "%s - no valid results, waking %zu %s",
                       record->mParams.mHostName.c_str(), record->mRequests.size(),
                       (record->mRequests.size() == 1) ? "requestor" : "requestors");

                firstNewResult = &query->mResult;

                ASSERT(query->mResult.mTtl == AseTimeVal(0), "aresLookupCallback() ttl default overriding unexpect value %llu\n", query->mResult.mTtl.val());
                query->mResult.mTtl = sConfig.mTtlMinTime;

                switch (status) {
                case ARES_SUCCESS:
                    ASSERT(false, "aresLookupCallback() status is ARES_SUCCES while query->mResult.mErrorCode is %d\n", query->mResult.mErrorCode);
                    break;

                case ARES_ENODATA:
                case ARES_ESERVFAIL:
                case ARES_ENOTFOUND:
                case ARES_ENOTIMP:
                case ARES_EREFUSED:
                    // failure not solvable by client
                    NTRACEIF(sConsecutiveFailures, TRACE_DNS_MANAGER, "consecutive failures reset to 0");
                    sConsecutiveFailures = 0;
                    break;

                case ARES_EFORMERR:
                case ARES_EBADQUERY:
                case ARES_EBADNAME:
                case ARES_EBADFAMILY:
                case ARES_EBADRESP:
                case ARES_ECONNREFUSED:
                case ARES_ETIMEOUT:
                case ARES_EOF:
                case ARES_EFILE:
                case ARES_ENOMEM:
                case ARES_EDESTRUCTION:
                case ARES_EBADSTR:
                case ARES_EBADFLAGS:
                case ARES_ENONAME:
                case ARES_EBADHINTS:
                case ARES_ENOTINITIALIZED:
                case ARES_ELOADIPHLPAPI:
                case ARES_EADDRGETNETWORKPARAMS:
                case ARES_ECANCELLED:
                    ++sConsecutiveFailures;
                    NTRACE(TRACE_DNS_MANAGER, "consecutive failures: %u", sConsecutiveFailures);
                    break;

                default:
                    NTRACE(TRACE_DNS_MANAGER, "warning: unhandled ares error %d", status);
                    ++sConsecutiveFailures;
                    NTRACE(TRACE_DNS_MANAGER, "consecutive failures: %u", sConsecutiveFailures);
                    break;
                }
            }
        }
    }

    if (firstNewResult) {
        // log lookups if enabled or if on transitions from success<-->error if query logging is off
        bool previousHadError = record->mResult && (record->mResult->mErrorCode != AS_NO_ERROR);
        bool newHasError = firstNewResult->mErrorCode != AS_NO_ERROR;
        bool logErrorTransition = (previousHadError != newHasError) && !sConfig.mLogQueries;

        record->mNewResult.reset(new RequestResult);
        *record->mNewResult = *firstNewResult;
        record->mResult = record->mNewResult;

        for (Requests::iterator rqit = record->mRequests.begin(); rqit != record->mRequests.end(); ++rqit) {
            RequestPtr request = *rqit;

            request->mResult = record->mResult;
            if (!request->mSignaled) {
                request->mSignaled = true;
                request->mClient->setSignal();

                // update request stats
                if (request->mResult->mErrorCode == AS_NO_ERROR)
                    ++sSuccessfulResponses;
                else if (request->mResult->mErrorCode == AS_TIMEOUT)
                    ++sTimeoutRespones;
                else
                    ++sErrorResponses;
                AseTimeVal responseTime = AseTimeVal::now() - request->mTime;
                sTotalResponseTime += responseTime;
            }
        }

        // log lookup result
        if (sConfig.mLogLookups || logErrorTransition)
            logLookupResult(record->mParams.mHostName, responseTime, query, firstNewResult);
    }

    ASSERT((record->mState != Record::STATE_COMPLETE) || record->mResult, "completed record with no result");

    // update stats
    if (status == ARES_SUCCESS)
        ++query->mChannel->mSuccessfulResponses;
    else if (status == ARES_ETIMEOUT)
        ++query->mChannel->mTimeoutResponses;
    else
        ++query->mChannel->mErrorResponses;
    query->mChannel->mTotalResponseTime += responseTime;

    // log query result
    if (sConfig.mLogQueries)
        logQueryResult(record->mParams.mHostName, responseTime, query);
}

AseErrorCode aresErrorToAseErrorCode(int aresError)
{
    switch (aresError)
    {
        case ARES_SUCCESS:               return AS_NO_ERROR;
        case ARES_ENODATA:               return AS_DNS_NO_DATA;
        case ARES_EFORMERR:              return AS_DNS_FORMAT_ERROR;
        case ARES_ESERVFAIL:             return AS_DNS_SERVER_FAILURE;
        case ARES_ENOTFOUND:             return AS_DNS_NOT_FOUND;
        case ARES_ENOTIMP:               return AS_DNS_NOT_IMPLEMENTED;
        case ARES_EREFUSED:              return AS_DNS_QUERY_REFUSED;
        case ARES_EBADQUERY:             return AS_DNS_QUERY_MALFORMED;
        case ARES_EBADNAME:              return AS_DNS_BAD_NAME;
        case ARES_EBADFAMILY:            return AS_DNS_BAD_FAMILY;
        case ARES_EBADRESP:              return AS_DNS_RESPONSE_MALFORMED;
        case ARES_ECONNREFUSED:          return AS_DNS_CONNECTION_REFUSED;
        case ARES_ETIMEOUT:              return AS_DNS_TIMEOUT;
        case ARES_EOF:                   return AS_DNS_EOF;
        case ARES_EFILE:                 return AS_DNS_FILE;
        case ARES_ENOMEM:                return AS_DNS_NO_MEMORY;
        case ARES_EDESTRUCTION:          return AS_DNS_CHANNEL_DESTROYED;
        case ARES_EBADSTR:               return AS_DNS_BAD_STRING;
        case ARES_EBADFLAGS:             return AS_DNS_BAD_FLAGS;
        case ARES_ENONAME:               return AS_DNS_NO_NAME;
        case ARES_EBADHINTS:             return AS_DNS_BAD_HINTS;
        case ARES_ENOTINITIALIZED:       return AS_DNS_NOT_INITIALIZED;
        case ARES_ELOADIPHLPAPI:         return AS_DNS_NOT_INITIALIZED;
        case ARES_EADDRGETNETWORKPARAMS: return AS_DNS_NOT_INITIALIZED;
        case ARES_ECANCELLED:            return AS_DNS_CANCELLED;

        default:
            ASSERT(0, "unknown error code %d", aresError);
            return AS_DNS_NOT_IMPLEMENTED;
    }
}

void logLookupResult(const std::string & hostName, const AseTimeVal & responseTime, const Query * query, const RequestResult * result)
{
    Variant lookup;
    lookup["hst"] = hostName;
    lookup["srv"] = query->mChannel->mName;
    lookup["adt"] = (result->mAddressType == ADDRESSTYPE_IPV4) ? "ipv4" : "ipv6";
    lookup["tim"] = responseTime.ms();
    if (result->mErrorCode != AS_NO_ERROR) {
        lookup["err"] = result->mErrorCode;
    } else {
        if (!result->mName.empty())
            lookup["nam"] = result->mName;
        if (!result->mAliases.empty())
            lookup["als"] = result->mAliases;
        lookup["ttl"] = result->mTtl.ms();
        if (!result->mAddresses.empty()) {
            std::vector<std::string> addresses;
            for (RequestResult::Addresses::const_iterator ait = result->mAddresses.begin(); ait != result->mAddresses.end(); ++ait)
                addresses.push_back(ait->getStrAddr());
            lookup["adr"] = addresses;
        }
    }

#ifndef NRDP_HAS_IPV6
    const Log::LogLevel level = (result->mErrorCode == AS_NO_ERROR) ? Log::Info : Log::Error;
#else
    const Log::LogLevel level = ((result->mErrorCode == AS_NO_ERROR) || (result->mAddressType == ADDRESSTYPE_IPV6)) ? Log::Info : Log::Error;
#endif
    Log::Message msg(TRACE_DNS_MANAGER, level, "dnslookup");
    msg.m_message = lookup.toJSON();
    msg.send();
}
    
void logQueryResult(const std::string & hostName, const AseTimeVal & responseTime, const Query * query)
{
    Variant lookup;
    lookup["hst"] = hostName;
    lookup["srv"] = query->mChannel->mName;
    lookup["adt"] = (query->mAddressType == ADDRESSTYPE_IPV4) ? "ipv4" : "ipv6";
    lookup["tim"] = responseTime.ms();
    if (query->mResult.mErrorCode != AS_NO_ERROR) {
        lookup["err"] = query->mResult.mErrorCode;
    } else {
        if (!query->mResult.mName.empty())
            lookup["nam"] = query->mResult.mName;
        if (!query->mResult.mAliases.empty())
            lookup["als"] = query->mResult.mAliases;
        lookup["ttl"] = query->mResult.mTtl.ms();
        if (!query->mResult.mAddresses.empty()) {
            std::vector<std::string> addresses;
            for (RequestResult::Addresses::const_iterator ait = query->mResult.mAddresses.begin(); ait != query->mResult.mAddresses.end(); ++ait)
                addresses.push_back(ait->getStrAddr());
            lookup["adr"] = addresses;
        }
    }

    const Log::LogLevel level = (query->mResult.mErrorCode == AS_NO_ERROR) ? Log::Info : Log::Error;
    Log::Message msg(TRACE_DNS_MANAGER, level, "dnsquery");
    msg.m_message = lookup.toJSON();
    msg.send();
}

bool createPipe(int * pipe)
{
    int ret = ::pipe(pipe);
    if (ret == -1) {
        pipe[0] = pipe[1] = -1;
        return false;
    }
    EINTRWRAP(ret, ::fcntl(pipe[0], F_GETFL, 0));

    if (ret == -1) {
        close(pipe[0]);
        close(pipe[1]);
        pipe[0] = pipe[1] = -1;
        return false;
    }

    const int flags = ret | O_NONBLOCK;

    EINTRWRAP(ret, ::fcntl(pipe[0], F_SETFL, flags));
    if (ret == -1) {
        close(pipe[0]);
        close(pipe[1]);
        pipe[0] = pipe[1] = -1;
        return false;
    }
    return true;
}

bool clearPipe(int fd)
{
    bool ok = true;
    int ret;
    do {
        char ch;
        EINTRWRAP(ret, ::read(fd, &ch, sizeof(ch)));
        if (ret == 0) {
            ok = false;
        } else if (ret == -1) {
            ok = (errno == EAGAIN || errno == EWOULDBLOCK);
        }
    } while (ret == 1);
    return ok;
}

std::string resultToString(const RequestResultPtr & result)
{
    std::ostringstream str;
    if (result) {
        str << "Error code: " << result->mErrorCode
            << " Age: " << (AseTimeVal::now() - result->mTime).ms()
            << " Name: " << result->mName
            << " Aliases: " << StringTokenizer::join(result->mAliases, ", ")
            << " Type: ";
        switch (result->mAddressType) {
            case ADDRESSTYPE_IPV4: str << "(4)"; break;
#ifdef NRDP_HAS_IPV6
            case ADDRESSTYPE_IPV6: str << "(6)"; break;
#endif
            case ADDRESSTYPE_NONE: str << "(none)"; break;
            default: ASSERT(0, "unknown address type %d", result->mAddressType); break;
        }
        str << " TTL: " << static_cast<int>(result->mTtl.ms());
        for (RequestResult::Addresses::const_iterator it = result->mAddresses.begin(); it != result->mAddresses.end(); ++it) {
            if (it != result->mAddresses.begin())
                str << ' ';
            str << it->getStrAddr();
        }
    }
    return str.str();
}

bool operator == (const RequestParams & lhs, const RequestParams & rhs)
{
    return
        (lhs.mHostName == rhs.mHostName) &&
        (lhs.mAddressType == rhs.mAddressType);
}

bool operator == (const Configuration & lhs, const Configuration & rhs)
{
    return
        (lhs.mParallelLookups == rhs.mParallelLookups) &&
        (lhs.mLookupTimeout == rhs.mLookupTimeout) &&
        (lhs.mLookupRetries == rhs.mLookupRetries) &&
        (lhs.mCacheMaxEntries == rhs.mCacheMaxEntries) &&
        (lhs.mCacheMaxTime == rhs.mCacheMaxTime) &&
        (lhs.mTtlMinTime == rhs.mTtlMinTime) &&
        (lhs.mServers == rhs.mServers) &&
        (lhs.mFallbackServers == rhs.mFallbackServers) &&
        (lhs.mFallbackDelay == rhs.mFallbackDelay) &&
        (lhs.mLogLookups == rhs.mLogLookups) &&
        (lhs.mLogQueries == rhs.mLogQueries) &&
        (lhs.mPrefetchHosts == rhs.mPrefetchHosts) && // FIX - sort before comparing
        (lhs.mConsecutiveFailuresLimit == rhs.mConsecutiveFailuresLimit) &&
        (lhs.mUseDefaultServers == rhs.mUseDefaultServers);
}

} // namespace DnsManager
} // namespace netflix
