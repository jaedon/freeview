/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MdxBridge_h
#define MdxBridge_h

#include <nrd/NfObject.h>
#include <nrd/Response.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ReadWriteLock.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Version.h>
#include <nrd/IMdx.h>
#include <nrd/MdxError.h>
#include <string.h>

namespace netflix {

namespace mdx {
class CryptContext;
class MdxNetworkManager;
}


class MdxBridge : public NfObject, public mdx::IMdx::Listener
{
public:
    MdxBridge();
    ~MdxBridge();

    void init();

    virtual Variant invoke(int method, const Variant &data);

    virtual bool getProperty(int index, Variant *result) const;
    virtual void DeviceFound(const std::string& location,
                             const std::string& serviceType,
                             const std::string& USN,
                             const std::string& friendlyName,
                             const ResponseHeaders &headers,
                             bool activated,
                             bool paired);
    virtual void DeviceByeBye (const std::string &serviceType, const std::string& USN);
    virtual void HttpResponse(const std::string& url, int method , llong xid, int32_t responseCode, const std::string& headers, const std::string& body);
    virtual void IncomingMessage(const std::string& url, const Variant& headers, const std::string& body);
    virtual void WebSocketMessage(const Variant& headers, const std::string& body);
    virtual void error(mdx::MdxErrorCode error);
    virtual void stateChanged(mdx::IMdx::State, const std::string& ssid, int listeningPort);
    virtual void advertisingStateChanged(mdx::IMdx::AdvertisingState state);
    virtual void discoveryStateChanged(mdx::IMdx::DiscoveryState state);
    virtual void interfaceChanged(const std::string &prevInterface, const std::string &newInterface, bool connected);
    virtual void DialDeviceInfoResponse(const std::string &url,
                                        const std::string &USN,
                                        const std::string &serviceType,
                                        const Variant &responseHeaders,
                                        const Variant &desc);

    virtual void webSocketConnect(const std::string &host);
    virtual void webSocketDisconnect(const std::string &host);
    virtual void webSocketSent(const std::string &host, ullong xid);

private:

    void sendMdxEvent(const char *type);
    void sendMdxEvent(const Variant &map);
    bool getEncodedStringArg(int method, const Variant &data, const std::string &name, std::string &arg);

    typedef std::map<std::string, shared_ptr<mdx::CryptContext> > CryptContextMap;
    shared_ptr<mdx::CryptContext> getContext(const std::string &context) const;

    shared_ptr<mdx::IMdx>              mMdx;
    shared_ptr<mdx::MdxNetworkManager> mNetworkManager;
    ReadWriteLock       mContextRWLock;
    unsigned long                      mContextIdx;
    CryptContextMap                    mContexts;
};
} // namespace netflix

#endif
