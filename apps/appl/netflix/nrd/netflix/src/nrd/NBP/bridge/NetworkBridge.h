/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NetworkBridge_h__
#define NetworkBridge_h__

#include <nrdbase/tr1.h>
#include <nrd/NfObject.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ConditionVariable.h>
#include <map>
#include <ares.h>

namespace netflix {

class NetworkBridge : public NfObject
{
public:
    NetworkBridge();

    virtual Variant invoke(int method, const Variant &data);
    virtual bool getProperty(int index, Variant *result) const;

    void shutdown();
private:
    void cancelDNSLookups();
    struct DnsResult {
        int result;
        std::vector<std::string> ipaddrs;
    };

    DnsResult dnsCheck(const std::string &dnsip, const std::string &url, int timeout);
    static void ares_callback(void *arg, int status, int /*timeouts*/, struct hostent *host);

    Mutex mMutex;
    ConditionVariable mCondition;
    struct ChannelData {
        int *pipe;
        bool cancelled;
    };
    bool mShutdown;
    std::map<ares_channel, ChannelData> mActive;
};

}

#endif // NetworkBridge_h__
