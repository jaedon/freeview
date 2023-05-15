/*
    * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
    * U.S. copyright and other applicable intellectual property laws and
    * may not be copied without the express permission of Netflix, Inc.,
    * which reserves all rights.  Reuse of any of this content for any
    * purpose without the permission of Netflix, Inc. is strictly
    * prohibited.
    */

#include "DnsManager.h"
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/JSONFormatter.h>
#include <nrdbase/Log.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Variant.h>
#include <algorithm>
#include <xtl.h>

#ifdef NDEBUG
#define DNSMANAGER_DEBUG_NTRACE(fmt, ...)
#else
#define DNSMANAGER_DEBUG_NTRACE(fmt, ...) //NTRACE(TRACE_DNS_MANAGER, fmt, ##__VA_ARGS__)
#endif

using namespace netflix;

static inline std::string resultToString(const DnsManager::RequestResultPtr &result)
{
    std::ostringstream str;
    if (result) {
        str << "Error code: " << result->mErrorCode
            << " Age: " << (AseTimeVal::now() - result->mTime).ms()
            << " Name: " << result->mName
            << " Aliases: " << netflix::StringTokenizer::join(result->mAliases, ", ")
            << " Type: ";
        switch (result->mAddressType) {
            case netflix::DnsManager::ADDRESSTYPE_IPV4: str << "(4)"; break;
#ifdef NRDP_HAS_IPV6
            case netflix::DnsManager::ADDRESSTYPE_IPV6: str << "(6)"; break;
#endif
            case netflix::DnsManager::ADDRESSTYPE_NONE: str << "(none)"; break;
            default: ASSERT(0, "unknown address type %d", result->mAddressType); break;
        }
        str << " TTL: " << static_cast<int>(result->mTtl.ms());
        for (netflix::DnsManager::RequestResult::Addresses::const_iterator it = result->mAddresses.begin(); it != result->mAddresses.end(); ++it) {
            if (it != result->mAddresses.begin())
                str << ' ';
            str << it->getStrAddr();
        }
    }
    return str.str();
}

namespace netflix {
namespace DnsManager {

class Query;
class Record;

typedef std::list< RequestPtr > Requests;
typedef shared_ptr<Query> QueryPtr;
typedef shared_ptr< Client > DnsClientPtr;
typedef std::list< DnsClientPtr> Clients;
typedef shared_ptr<Record> RecordPtr;
typedef std::list< RecordPtr > Records;

// Query encapsulates DNS query setup info and results
class Query
{
public:
    inline Query();
    inline ~Query();

    Record * mRecord;
    AddressType mAddressType;
    AseTimeVal mStartTime;
    RequestResult mResult;
    XNDNS * mXNDns;
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

protected:
    void Run();

    HANDLE mEvent;
    volatile bool mShutdown;
};

static Mutex sInstanceMutex(DNSMANAGER_INSTANCE_MUTEX, "DNSManager");
static bool sInitialized;
static Configuration sConfig;
static Mutex sClientsMutex(DNSMANAGER_CLIENTS_MUTEX, "DNSClients");
static Clients sClients;
static Mutex sRecordsMutex(DNSMANAGER_RECORDS_MUTEX, "DNSRecords");
static Records sRecords;
static shared_ptr<WorkerThread> sWorkerThread;
static unsigned int sCacheHits;
static unsigned int sCacheMisses;
static std::string sGroupName;

static RecordPtr obtainRecord(const RequestParams & requestParams, RequestPtr request);
static RequestResultPtr resolveIpLiteral(const RequestParams & requestParams);
static AseErrorCode xnetErrorToAseErrorCode(int xnetError);
static inline bool operator == (const RequestParams & lhs, const RequestParams & rhs);
static inline bool operator == (const Configuration & lhs, const Configuration & rhs);

void create()
{
    NTRACE(TRACE_DNS_MANAGER, "create()");

    ScopedMutex instanceLock(sInstanceMutex);

    ASSERT(!sInitialized, "already created");
    if (sInitialized)
        return;

    sInitialized = true;

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

        DnsManager::RequestParams requestParams;
        requestParams.mHostName = host;

        requestParams.mAddressType = DnsManager::ADDRESSTYPE_IPV4;
        obtainRecord(requestParams, RequestPtr());
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

    DNSMANAGER_DEBUG_NTRACE("destroying %zu records", sRecords.size());
    sRecords.clear();
    DNSMANAGER_DEBUG_NTRACE("destroying %zu clients", sClients.size());
    sClients.clear();
    sConfig.restoreDefaults();

    sInitialized = false;
}

void down()
{
    ASSERT(false, "down() not implemented");
}

void up()
{
    ASSERT(false, "up() not implemented");
}

void restartResolver()
{
    ASSERT(false, "restartResolver() not implemented");
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
    mPrefetchHosts.clear();
    mConsecutiveFailuresLimit = 5;
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

    if (config == sConfig) {
        NTRACE(TRACE_DNS_MANAGER, "identical configuration, ignoring");
    } else {
        sConfig = config;
        NTRACE(TRACE_DNS_MANAGER, "config: %s", sConfig.toString().c_str());
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

    NTRACE(TRACE_DNS_MANAGER, "warning, client %s not found", client->mName.c_str());
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

    if( !client->mRequests.empty() ) {
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
    if (!result && !sInitialized)
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
                NTRACE(TRACE_DNS_MANAGER, "warning, request %s not found in record", request->mParams.mHostName.c_str());
            }
            recordFound = true;
            break;
        }
    }
    if (!recordFound) {
        NTRACE(TRACE_DNS_MANAGER, "warning, record not found for request %s", request->mParams.mHostName.c_str());
    }

    // remove request from client
    Requests::iterator rqit = std::find(request->mClient->mRequests.begin(), request->mClient->mRequests.end(), request);
    if (rqit != request->mClient->mRequests.end()) {
        request->mClient->mRequests.erase(rqit);
    } else {
        NTRACE(TRACE_DNS_MANAGER, "warning, request %s not found in client",
            request->mParams.mHostName.c_str());
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

    return request->mResult;
}

RequestResultPtr requestTryGetResult(const RequestParams & requestParams)
{
    DNSMANAGER_DEBUG_NTRACE("%s - requestTryGetResult()", requestParams.mHostName.c_str());

    RequestResultPtr result = resolveIpLiteral(requestParams);
    if (result || !sInitialized)
        return result;

    RecordPtr record = obtainRecord(requestParams, RequestPtr());

    DNSMANAGER_DEBUG_NTRACE("%s - existing result %s", requestParams.mHostName.c_str(),
        resultIsUsable(record->mResult) ? "usable" : "not usable");

    // update stats
    if (resultIsUsable(record->mResult))
        ++sCacheHits;
    else
        ++sCacheMisses;

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
    DNSMANAGER_DEBUG_NTRACE("logConfiguration()");

    ScopedMutex instanceLock(sInstanceMutex);

    Variant config;
    config["dplf"] = sConfig.mParallelLookups;
    config["dlt"] = sConfig.mLookupTimeout.ms();
    config["dlr"] = sConfig.mLookupRetries;
    config["dcel"] = sConfig.mCacheMaxEntries;
    config["dct"] = sConfig.mCacheMaxTime.ms();
    config["dmt"] = sConfig.mTtlMinTime.ms();
    config["dllf"] = sConfig.mLogLookups;
    if (!sConfig.mServers.empty())
        config["srv"] = sConfig.mServers;
    if (!sConfig.mFallbackServers.empty())
        config["dfs"] = sConfig.mFallbackServers;
    config["dfd"] = sConfig.mFallbackDelay.ms();
    if (!sConfig.mPrefetchHosts.empty())
        config["dph"] = sConfig.mPrefetchHosts;
    config["dcfl"] = sConfig.mConsecutiveFailuresLimit;
    if(sGroupName.empty())
        config["groupname"] = "control";
    else
        config["groupname"] = sGroupName;

    JSONFormatter jsonFormatter;
    Log::Message msg(TRACE_DNS_MANAGER, Log::Info, "dnsconfig");
    msg.m_message = jsonFormatter.format(config);
    msg.m_flags |= Log::Message::Critical;
    msg.send();
}

void logStats(bool reset)
{
    DNSMANAGER_DEBUG_NTRACE("logStats()");

    ScopedMutex instanceLock(sInstanceMutex);

    Variant stats;
    stats["cah"] = sCacheHits;
    stats["cam"] = sCacheMisses;

    if (reset) {
        sCacheHits = 0;
        sCacheMisses = 0;
    }

    JSONFormatter jsonFormatter;
    Log::Message msg(TRACE_DNS_MANAGER, Log::Info, "dnsstats");
    msg.m_message = jsonFormatter.format(stats);
    msg.m_flags |= Log::Message::Critical;
    msg.send();
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

    AseTimeVal timeNow(AseTimeVal::now());
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
    }

    // not resolved, return null
    return RequestResultPtr();
}

Query::Query() :
    mAddressType(ADDRESSTYPE_NONE),
    mXNDns(0)
{
}

Query::~Query()
{
    //DNSMANAGER_DEBUG_NTRACE("%s - destroying query",
    //                        mRecord ? mRecord->mParams.mHostName.c_str() : "no record")

    if (mXNDns) {
        INT ret = XNetDnsRelease(mXNDns);
        ASSERT(!ret, "dns release failed");
        NRDP_UNUSED(ret);
    }
}

Client::Client(const std::string & name) :
    mName(name)
{
    int val;

    mSignalPipe[0] = mSignalPipe[1] = -1;
    val = pipe(mSignalPipe);
    NRDP_UNUSED(val);
    ASSERT(!val && (mSignalPipe[0] >= 0) && (mSignalPipe[1] >= 0), "pipe creation failed");
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
        ret = write(mSignalPipe[1], &signalByte, sizeof(signalByte));
        NRDP_UNUSED(ret);
    }
}

void Client::clearSignal()
{
    ssize_t ret = 0;
    if (mSignalPipe[0] >= 0) {
        DNSMANAGER_DEBUG_NTRACE("clearing signal client %s/%p", mName.c_str(), this);
        char signalByte;
        ret = read(mSignalPipe[0], &signalByte, sizeof(signalByte));
        NRDP_UNUSED(ret);
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
    const char *state = 0;
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

    return StringFormatter::sformat("State: %s Host: %s%s Requests: %d Queries: %d Processing: %d Age: %llu Result: %s New result: %s",
        state, mParams.mHostName.c_str(), requestType, mRequests.size(), mQueries.size(), mProcessingCount,
        (AseTimeVal::now() - mLastLookupTime).ms(), ::resultToString(mResult).c_str(), ::resultToString(mNewResult).c_str());
}

Request::Request() :
    mUserData(0),
    mSignaled(false)
{
}

Request::~Request()
{
    DNSMANAGER_DEBUG_NTRACE("%s - destroying request, client %s/%p",
                            mParams.mHostName.c_str(),
                            mClient ? mClient->mName.c_str() : "no client",
                            mClient.get());
    //ASSERT(mSignaled, "request not signaled before destruction");
}

WorkerThread::WorkerThread() :
    Thread(&THREAD_DNS_MANAGER),
    mEvent(0),
    mShutdown(false)
{
    mEvent = CreateEvent(NULL, false, FALSE, "dns worker");
    ASSERT(mEvent, "CreateEvent() failed");
}

WorkerThread::~WorkerThread()
{
    DNSMANAGER_DEBUG_NTRACE("destroying worker thread");

    mShutdown = true;
    setSignal();
    Wait();

    if (!CloseHandle(mEvent))
        ASSERT(false, "CloseHandle() failed");
}

void WorkerThread::setSignal()
{
    if (!SetEvent(mEvent))
        ASSERT(false, "SetEvent() failed");
}

void WorkerThread::Run()
{
    NTRACE(TRACE_DNS_MANAGER, "Run()");

    while (!mShutdown) {
        PERFORMANCE_MARKER_SCOPED("dnsmanager.loop");

        {
            PERFORMANCE_MARKER_SCOPED("dnsmanager.loop.process");

            sRecordsMutex.lock();

            // process records
            Records::iterator rcit = sRecords.begin();
            while (rcit != sRecords.end()) {
                RecordPtr record = *rcit;

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

                // issue queries for waiting records
                if (record->mState == Record::STATE_WAITING) {
                    DNSMANAGER_DEBUG_NTRACE("%s - initiating queries", record->mParams.mHostName.c_str());

                    ASSERT(!record->mProcessingCount, "unexpected processing count %u", record->mProcessingCount);
                    record->mState = Record::STATE_PROCESSING;
                    record->mQueries.clear();
                    record->mNewResult.reset();

                    QueryPtr query(new Query);
                    query->mRecord = record.get();
                    query->mAddressType = query->mResult.mAddressType = record->mParams.mAddressType;
                    query->mStartTime = AseTimeVal::now();
                    query->mResult.mErrorCode = AS_DNS_NO_DATA;
                    query->mXNDns = 0;
                    record->mQueries.push_back(query);
                    ++record->mProcessingCount;

                    for (Record::Queries::iterator qit = record->mQueries.begin(); qit != record->mQueries.end(); ++qit) {
                        QueryPtr query = *qit;

                        if (query->mAddressType == ADDRESSTYPE_IPV4) {
                            INT ret = XNetDnsLookup(query->mRecord->mParams.mHostName.c_str(), mEvent, &query->mXNDns);
                            query->mResult.mErrorCode = xnetErrorToAseErrorCode(ret);
                        } else {
                            ASSERT(0, "unknown address type %d", query->mAddressType);
                        }
                    }
                }

                // check for completion of in progress records
                if (record->mState == Record::STATE_PROCESSING) {
                    for (Record::Queries::iterator qit = record->mQueries.begin(); qit != record->mQueries.end(); ++qit) {
                        QueryPtr query = *qit;

                        bool complete = false;

                        if (query->mAddressType == ADDRESSTYPE_IPV4) {
                            INT iStatus = query->mXNDns->iStatus; // make a copy since it can change anytime

                            if (query->mResult.mErrorCode != AS_NO_ERROR) {
                                complete = true;
                            } else if (iStatus == ERROR_SUCCESS) {
                                complete = true;

                                UINT addrCount = query->mXNDns->cina;
                                if (addrCount <= 0) {
                                    NTRACE(TRACE_DNS_MANAGER, "%s - no addresses found",
                                        record->mParams.mHostName.c_str());
                                    query->mResult.mErrorCode = AS_DNS_NO_DATA;
                                } else {
                                    query->mResult.mAddresses.clear();

                                    for (UINT count = 0; count < addrCount; ++count) {
                                        NrdIpAddr nrdIpAddr(query->mXNDns->aina[count]);

                                        NTRACE(TRACE_DNS_MANAGER, "%s - IPv4 address %s",
                                            record->mParams.mHostName.c_str(), nrdIpAddr.getStrAddr().c_str());

                                        query->mResult.mAddresses.push_back(nrdIpAddr);
                                    }

                                    // no TTL available, just use default
                                    query->mResult.mTtl = sConfig.mTtlMinTime;

                                    DNSMANAGER_DEBUG_NTRACE("%s - expiration %llu", record->mParams.mHostName.c_str(),
                                        (query->mResult.mTime + query->mResult.mTtl).seconds());
                                }
                            } else if (iStatus != WSAEINPROGRESS) {
                                query->mResult.mErrorCode = xnetErrorToAseErrorCode(iStatus);
                                complete = true;
                            }
                        } else {
                            ASSERT(0, "unknown address type %d", query->mAddressType);
                        }

                        if (complete) {
                            bool gotFirstNewResult = false;

                            AseTimeVal timeNow = AseTimeVal::now();

                            query->mResult.mTime = timeNow;
                            AseTimeVal responseTime = timeNow - query->mStartTime;

                            ASSERT(record->mProcessingCount > 0, "processing count underflow");
                            --record->mProcessingCount;
                            if (!record->mProcessingCount)
                                record->mState = Record::STATE_COMPLETE;

                            if (query->mResult.mErrorCode == AS_NO_ERROR) {
                                NTRACE(TRACE_DNS_MANAGER, "%s - first valid result, waking %zu %s",
                                    record->mParams.mHostName.c_str(), record->mRequests.size(),
                                    (record->mRequests.size() == 1) ? "requestor" : "requestors");
                                gotFirstNewResult = true;
                            } else if (record->mState == Record::STATE_COMPLETE) {
                                NTRACE(TRACE_DNS_MANAGER, "%s - no valid results, waking %zu %s",
                                    record->mParams.mHostName.c_str(), record->mRequests.size(),
                                    (record->mRequests.size() == 1) ? "requestor" : "requestors");
                                gotFirstNewResult = true;
                            }

                            if (gotFirstNewResult) {
                                record->mNewResult.reset(new RequestResult);
                                *record->mNewResult = query->mResult;
                                record->mResult = record->mNewResult;

                                for (Requests::iterator rqit = record->mRequests.begin(); rqit != record->mRequests.end(); ++rqit) {
                                    RequestPtr request = *rqit;

                                    request->mResult = record->mResult;
                                    if (!request->mSignaled) {
                                        request->mSignaled = true;
                                        request->mClient->setSignal();
                                    }
                                }
                            }

                            ASSERT((record->mState != Record::STATE_COMPLETE) || record->mResult, "completed record with no result");

                            if (sConfig.mLogLookups) {
                                Variant lookup;
                                lookup["hst"] = record->mParams.mHostName;
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

                                JSONFormatter jsonFormatter;
                                Log::Message msg(TRACE_DNS_MANAGER, Log::Info, "dnslookup");
                                msg.m_message = jsonFormatter.format(lookup);
                                msg.send();
                            }
                        }
                    }
                }

                ++rcit;
            }

            sRecordsMutex.unlock();
        }

        if (WaitForSingleObject(mEvent, 1000) == WAIT_FAILED)
            ASSERT(false, "WaitForSingleObject() failed");
    }
}

AseErrorCode xnetErrorToAseErrorCode(int xnetError)
{
    switch (xnetError) {
        case ERROR_SUCCESS:     return AS_NO_ERROR;
        case WSAHOST_NOT_FOUND: return AS_DNS_NOT_FOUND;
        case WSAEREFUSED:       return AS_DNS_QUERY_REFUSED;
        case WSAETIMEDOUT:      return AS_DNS_TIMEOUT;
        case WSATRY_AGAIN:      return AS_DNS_TIMEOUT;
        case WSANO_RECOVERY:    return AS_DNS_ERROR;
        case WSANO_DATA:        return AS_DNS_NO_DATA;
        default:
            __debugbreak();
            return AS_DNS_ERROR;
    }
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
        (lhs.mPrefetchHosts == rhs.mPrefetchHosts) && // FIX - sort before comparing
        (lhs.mConsecutiveFailuresLimit == rhs.mConsecutiveFailuresLimit);
}

} // namespace DnsManager
} // namespace netflix
