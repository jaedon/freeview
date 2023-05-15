/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DNS_MANAGER_H
#define DNS_MANAGER_H

#include "nrdbase/tr1.h"
#include "nrdbase/AseErrorCodes.h"
#include "nrdnet/AseTimeVal.h"
#include "nrdnet/NrdIpAddr.h"
#include <list>
#include <string>
#include <vector>

namespace netflix {
namespace DnsManager {

    // Instance management

    void create();
    void destroy();

    void down();
    void up();

    void restartResolver();
    // Configuration

    class Configuration
    {
    public:
        Configuration() { restoreDefaults(); }

        void restoreDefaults();
        std::string toString() const;

        typedef std::vector<std::string> DnsServers;
        typedef std::vector<std::string> Hosts;

        bool mParallelLookups;
        AseTimeVal mLookupTimeout;
        unsigned int mLookupRetries;
        unsigned int mCacheMaxEntries;
        AseTimeVal mCacheMaxTime;
        AseTimeVal mTtlMinTime;
        DnsServers mServers;
        DnsServers mFallbackServers;
        AseTimeVal mFallbackDelay;
        bool mLogLookups;
        bool mLogQueries;
        Hosts mPrefetchHosts;
        unsigned int mConsecutiveFailuresLimit;
        bool mUseDefaultServers;
    };

    void setConfiguration(const Configuration & config);
    void getConfiguration(Configuration & config);


    // Client management

    class Client;
    typedef shared_ptr<Client> ClientPtr;

    ClientPtr clientAdd(const std::string & name);
    void clientRemove(ClientPtr client);
    int clientGetSignalFd(ClientPtr client);
    void clientClearSignal(ClientPtr client);
    bool clientHasPendingRequests(ClientPtr client);


    // Manage requests and results

    enum AddressType {
        ADDRESSTYPE_NONE,
        ADDRESSTYPE_IPV4,
#ifdef NRDP_HAS_IPV6
        ADDRESSTYPE_IPV6,
#endif
        ADDRESSTYPE_COUNT
    };

    struct RequestParams
    {
        std::string mHostName;
        AddressType mAddressType;
    };

    struct RequestResult
    {
        typedef std::vector<NrdIpAddr> Addresses;
        AseErrorCode mErrorCode;
        AseTimeVal mTime;
        std::string mName;
        std::vector<std::string> mAliases;
        AddressType mAddressType;
        Addresses mAddresses;
        AseTimeVal mTtl;
    };
    typedef shared_ptr<RequestResult> RequestResultPtr;

    class Request;
    typedef shared_ptr<Request> RequestPtr;

    RequestPtr requestAdd(ClientPtr client, const RequestParams & requestParams);
    void requestRemove(RequestPtr request);
    const RequestParams & requestGetParams(const RequestPtr request);
    void requestSetUserData(RequestPtr request, void * userData);
    void * requestGetUserData(const RequestPtr request);
    const AseTimeVal & requestGetTime(const RequestPtr request);
    RequestResultPtr requestGetResult(const RequestPtr request);
    std::vector<std::string> currentRecords();

    // Try to get a result without doing a network query (uses cache, local lookup, etc.)
    RequestResultPtr requestTryGetResult(const RequestParams & requestParams);

    // Test validity of results
    bool resultIsUsable(const RequestResultPtr result);
    bool resultIsExpired(const RequestResultPtr result);


    // logging
    void setGroupName(const std::string &groupName);
    void logConfiguration();
    void logStats(bool reset = true);

} // namespace DnsManager
} // namespace netflix

#endif // DNS_MANAGER_H
