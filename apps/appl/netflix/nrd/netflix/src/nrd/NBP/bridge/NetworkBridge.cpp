/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NetworkBridge.h"

#include <nrdbase/Variant.h>
#include <nrdnet/NetError.h>
#include <nrd/Request.h>
#include <nrdnet/ResourceManager.h>
#include <nrd/NrdApplication.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

using namespace netflix;

#define NetworkBridgePropertyList(Property)     \
    Property(ERRORCODE, NfObject::ReadOnly)     \
    Property(ERRORGROUP, NfObject::ReadOnly)

PropertyTable(NetworkBridge, NetworkBridgePropertyList, properties)

#define NetworkBridgeMethodList(Method)         \
    Method(canceldns)                           \
    Method(checkdns)                            \
    Method(get)                                 \
    Method(useFallbackDnsOnly)

MethodTable(NetworkBridge, NetworkBridgeMethodList);

#define REQUESTVALUE(name, type)                        \
    const type name = data.mapValue<type>(#name, &ok);  \
    if (!ok) {                                          \
        invalidArgumentError(method, #name);            \
        return false;                                   \
    }


NetworkBridge::NetworkBridge()
    : NfObject("network", Properties::NetworkBridge::properties, Properties::NetworkBridge::PropertyCount,
               Methods::NetworkBridge::methods, Methods::NetworkBridge::MethodCount),
      mMutex(NETWORKBRIDGE_MUTEX, "NETWORKBRIDGE"), mShutdown(false)
{
}

void NetworkBridge::shutdown()
{
    {
        ScopedMutex lock(mMutex);
        mShutdown = true;
    }
    cancelDNSLookups();
}

class NetworkBridgeRequest : public ResourceRequest
{
public:
    NetworkBridgeRequest(const shared_ptr<NfObject> &bridge, const std::string &url)
        : ResourceRequest(0, url, Resource::Method_Get), mBridge(bridge)
    {}
    virtual void onComplete(const Resource::SharedPtr &resource, Resource::State)
    {
        if (shared_ptr<NfObject> bridge = mBridge.lock()) {
            const NetError::ErrorCode result = resource->errorCode();
            const NetError::ErrorGroup ng = NetError::mapErrorGroup(result);
            const std::vector<AseUrl> urls = resource->urls();
            assert(!urls.empty());
            Variant data;
            data["type"] = "getresult";
            data["result"] = result;
            data["errorgroup"] = ng;
            data["errorcode"] = result;
            data["nativeerrorcode"] = resource->nativeErrorCode();
            data["url"] = urls[0].str();

            const NetworkMetrics metrics = resource->networkMetrics();
            if (metrics.serverIpAddress().valid())
                data["serverIp"] = metrics.serverIpAddress().getStrAddr();
            const std::string cname = metrics.cname();
            if (!cname.empty())
                data["cname"] = cname;

            const std::string sslVersion = metrics.sslVersion();
            if (!sslVersion.empty())
                data["sslVersion"] = sslVersion;

            if (urls.size() > 1) {
                Variant urlChain;
                for (std::vector<AseUrl>::const_iterator it = urls.begin(); it != urls.end(); ++it) {
                    urlChain.push_back(it->str());
                }
                data["urlChain"] = urlChain;
            }

            NTRACE(TRACE_NBP, "NetworkBridge::get result %d (%s)", result, data.toJSON(true).c_str());
            bridge->sendEvent("INetwork", data);
        }
    }
private:
    weak_ptr<NfObject> mBridge;
};


static void state_cb(void *data, int fd, int read, int write)
{
    // suppress unused variable warnings
    NRDP_UNUSED(data);
    NRDP_UNUSED(fd);
    NRDP_UNUSED(read);
    NRDP_UNUSED(write);

    NTRACE(TRACE_NETWORK, "Change state fd %d read:%d write:%d for %s",
           fd, read, write, *reinterpret_cast<const char**>(data));
}

void NetworkBridge::ares_callback(void *arg, int status, int /*timeouts*/, struct hostent *host)
{
    NetworkBridge::DnsResult &dnsResult = *static_cast<DnsResult*>(arg);
    if (!host || status != ARES_SUCCESS) {
        NTRACE(TRACE_NETWORK, "Failed to lookup ec=%s", ares_strerror(status));
        dnsResult.result = status;
        return;
    }

    NTRACE(TRACE_NETWORK, "Found address name %s", host->h_name ? host->h_name : "Empty hostname");
    char ip[INET6_ADDRSTRLEN];

    for (int i = 0; host->h_addr_list[i]; ++i) {
        inet_ntop(host->h_addrtype, host->h_addr_list[i], ip, sizeof(ip));
        NTRACE(TRACE_NETWORK,"%s", ip);
        dnsResult.result = 0;
        dnsResult.ipaddrs.push_back(ip);
    }
}

NetworkBridge::DnsResult NetworkBridge::dnsCheck(const std::string &dnsip, const std::string &url, int timeout)
{
    DnsResult dnsResult;
    dnsResult.result = 0;
    NTRACE(TRACE_NETWORK, "dnsCheck dnsip=%s url=%s", dnsip.c_str(), url.c_str());
    /****/
    ares_channel channel;
    int status;
    struct ares_options options;
    memset(&options, '\0', sizeof(options));
    int optmask = 0;

    status = ares_library_init(ARES_LIB_INIT_ALL);
    if (status != ARES_SUCCESS){
        NTRACE(TRACE_NETWORK, "ares_library_init: %s", ares_strerror(status));
        dnsResult.result = status;
        return dnsResult;
    }
    options.sock_state_cb = state_cb;
    const char *urlCstr = url.c_str();
    options.sock_state_cb_data = &urlCstr;
    options.timeout = timeout;
    options.tries  = 1;
    optmask |= ARES_OPT_SOCK_STATE_CB;
    optmask |= ARES_OPT_TIMEOUTMS;
    optmask |= ARES_OPT_TRIES;
    status = ares_init_options(&channel, &options, optmask);
    if (status != ARES_SUCCESS) {
        NTRACE(TRACE_NETWORK, "ares_init_options: %s", ares_strerror(status));
        dnsResult.result = status;
        return dnsResult;
    }

    ares_addr_node *servers = (ares_addr_node *) malloc(1 * sizeof(*servers));
    if (!servers) {
        NTRACE(TRACE_NETWORK, "ares_init_options: ENOMEM");
        dnsResult.result = ARES_ENOMEM;
        return dnsResult;
    }

    if (inet_pton(AF_INET, dnsip.c_str(), &servers->addr.addr4) > 0) {
        servers[0].family = AF_INET;
    } else if (inet_pton(AF_INET6, dnsip.c_str(), &servers->addr.addr6) > 0) {
        servers[0].family = AF_INET6;
    }

    servers[0].next = 0;

    status = ares_set_servers(channel, servers);
    free(servers);

    if (status != ARES_SUCCESS) {
        NTRACE(TRACE_NETWORK, "ares_init_options: ares_set_servers failed");
        dnsResult.result = status;
        ares_destroy(channel);
        ares_library_cleanup();
        return dnsResult;
    }
    int pipe[2];
    if (::pipe(pipe) == -1) {
        NTRACE(TRACE_NETWORK, "Couldn't create pipe");
        dnsResult.result = -1;
        ares_destroy(channel);
        ares_library_cleanup();
        return dnsResult;
    }
    {
        ScopedMutex lock(mMutex);
        if (mShutdown) {
            NTRACE(TRACE_NETWORK, "NetworkBridge shutdown has been called. Discarding request");
            dnsResult.result = -1;
            ares_destroy(channel);
            ares_library_cleanup();
            return dnsResult;
        }
        ChannelData &data = mActive[channel];
        data.cancelled = false;
        data.pipe = pipe;
        mCondition.broadcast(); // just in case someone's actually trying to cancel right now
    }
    ares_gethostbyname(channel, url.c_str(), AF_INET, ares_callback, &dnsResult);
    while (!dnsResult.result) {
        struct timeval *tvp, tv;
        fd_set read_fds, write_fds;
        int nfds;

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        nfds = ares_fds(channel, &read_fds, &write_fds);
        if (nfds == 0) {
            break;
        }
        FD_SET(pipe[0], &read_fds);
        nfds = std::max(nfds, pipe[0] + 1);
        tvp = ares_timeout(channel, 0, &tv);
        (void)select(nfds, &read_fds, &write_fds, 0, tvp);
        ares_process(channel, &read_fds, &write_fds);
        if (FD_ISSET(pipe[0], &read_fds)) {
            break;
        }
    }
    {
        ScopedMutex lock(mMutex);
        close(pipe[0]);
        close(pipe[1]);
        mActive.erase(channel);
        mCondition.broadcast();
    }

    ares_destroy(channel);
    ares_library_cleanup();
    return dnsResult;
}

Variant NetworkBridge::invoke(int method, const Variant &data)
{
    bool ok;

    switch (method) {
    case Methods::NetworkBridge::get: {
        REQUESTVALUE(url, std::string);
        REQUESTVALUE(connectiontimeout, double);
        REQUESTVALUE(datatimeout, double);
        shared_ptr<NetworkBridgeRequest> request(new NetworkBridgeRequest(shared_from_this(), url));
        request->setRequestHeader("X-Gibbon-Cache-Control", "no-cache");
        request->setConnectTimeout((ullong)connectiontimeout);
        request->setTimeout((ullong)datatimeout);
        nrdApp()->resourceManager()->sendRequest(request);
        break; }
    case Methods::NetworkBridge::checkdns: {
        REQUESTVALUE(dnsip, std::string);
        REQUESTVALUE(url, std::string);
        DnsResult dnsResult = dnsCheck(dnsip, url, data.mapValue<int>("timeout", 0, 5000));
        Variant data;
        data["type"] = "dnsresult";
        data["result"] = dnsResult.result;
        data["ipaddrs"] = dnsResult.ipaddrs;
        data["url"] = url;
        NTRACE(TRACE_NBP, "NetworkBridge::sending result checkDNS %d", dnsResult.result);
        sendEvent("INetwork", data);
        break; }
    case Methods::NetworkBridge::useFallbackDnsOnly: {
        const bool fallback = data.mapValue<bool>("fallback");
        nrdApp()->useFalbackDnsConfig(fallback);
        break;
    }
    case Methods::NetworkBridge::canceldns:
        cancelDNSLookups();
        break;
    default:
        return false;
    }
    return true;
}

void NetworkBridge::cancelDNSLookups()
{
    ScopedMutex lock(mMutex);
    while (!mActive.empty()) {
        for (std::map<ares_channel, ChannelData>::iterator it = mActive.begin(); it != mActive.end(); ++it) {
            if (!it->second.cancelled) {
                int ret;
                do {
                    ret = write(it->second.pipe[1], "q", 1);
                } while (ret == -1 && errno == EINTR);
                assert(ret == 1);
                it->second.cancelled = true;
            }
        }
        mCondition.wait(&mMutex);
    }
}

bool NetworkBridge::getProperty(int index, Variant *result) const
{
    switch (index) {
    case Properties::NetworkBridge::ERRORGROUP: {
        Variant ret;
        for (int i=0; i<NetErrorGroup::Count; ++i) {
            ret[NetError::errorGroupToString(static_cast<NetError::ErrorGroup>(i))] = i;
        }
        *result = ret;
        return true; }
    case Properties::NetworkBridge::ERRORCODE: {
        Variant ret;
        for (int i=0; i<NetErrorCode::Count; ++i) {
            ret[NetError::errorCodeToString(static_cast<NetError::ErrorCode>(i))] = i;
        }
        *result = ret;
        return true; }
    }
    return false;
}
