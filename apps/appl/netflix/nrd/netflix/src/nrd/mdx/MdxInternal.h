/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MDX_INTERNAL_H
#define MDX_INTERNAL_H

#include "IMdx.h"
#include <nrdbase/Mutex.h>
#include <nrdbase/ReadWriteLock.h>
#include <nrdbase/Variant.h>
#include <nrdbase/Version.h>
#include "MdxListenerEvent.h"
#include "MdxController.h"
#include "MdxTarget.h"

namespace netflix {
class CurlMultiThread;
namespace mdx {

class MdxNetworkListener;

/**
 * @class MdxInternal MdxInternal.h
 * @brief Multi Device Experience Internal Class.
 */
class MdxInternal : public IMdx
{
public:
    static MdxInternal* getInstance();
    static void destroyInstance();

    virtual ~MdxInternal();

    //================================================================================
    // IMdx interface implementation.
    //================================================================================

    virtual MdxErrorCode mdxConfigure(int advertisingPeriod,
                                      int advertisingTTL,
                                      int advertisingPort,
                                      int listeningPort,
                                      int numSsdpReplies,
                                      int msgLimit);
    virtual MdxErrorCode mdxInit(const std::string& host,
                                 const std::string& targetServiceType,
                                 const std::string& uuid,
                                 const std::string& friendlyName,
                                 const std::string& manufacturer,
                                 const std::string& modelName,
                                 bool asyncHttpRequests,
                                 bool enableWebSocketServer);
    virtual MdxErrorCode mdxDeinit();
    virtual MdxErrorCode mdxAddInterfaceName(const std::string &name);
    virtual MdxErrorCode SearchForMdxDevices(const std::string& serviceType,
                                             const std::vector<std::string> &headerPatterns,
                                             int mx,
                                             int numSsdpSearches);
    virtual MdxErrorCode StopMdxDiscovery();
    virtual MdxErrorCode RevealTargetPresence();
    virtual MdxErrorCode SetDeviceReplyHeaders(const std::vector<std::string> &deviceReplyHeaders);
    virtual MdxErrorCode HideTargetPresence();
    virtual MdxErrorCode StartMdxAdvertising();
    virtual MdxErrorCode StopMdxAdvertising();
    virtual MdxErrorCode SendMdxHttpRequest(const std::string& url,
                                            int method ,
                                            llong xid,
                                            int curltimeout,
                                            const std::string& headers,
                                            const std::string& body);
    virtual MdxErrorCode SendSessionMessage(const std::string &url,
                                            int method,
                                            llong xid,
                                            int curltimeout,
                                            shared_ptr<CryptContext> context,
                                            const std::string &headers,
                                            const std::string &body,
                                            const std::string &message,
                                            const std::string &plaintext);
    virtual MdxErrorCode ProcessSessionMessage(shared_ptr<CryptContext> context,
                                               const std::string &message,
                                               const std::string &messageHmac,
                                               const std::string &ciphertext,
                                               std::string &outHmac,
                                               std::string &outPlaintext);
    virtual MdxErrorCode GetDialDeviceInfo(const std::string &url, const std::string &USN, const std::string &serviceType, int timeout);
    virtual MdxErrorCode SendWebSocketMessage(const std::string &host, ullong xid, const std::string &body);
    virtual void InterfaceChanged(const std::string &newInterface, bool connected, const std::string &ipaddress, const std::string &ssid);
    virtual MdxErrorCode addListener(Listener& l);
    virtual MdxErrorCode removeListener(Listener& l);
    virtual State getState() const { return initState_; }
    virtual AdvertisingState getAdvertisingState() const { return advertisingState_; }
    virtual DiscoveryState getDiscoveryState() const { return discoveryState_; }
    virtual std::string getInterface() const { return networkInterface_; }
    virtual std::string getIpAddress() const;
    virtual void error(MdxErrorCode error);
    /**
     * Call the listeners' notification functions for the specified listener event.
     * @param[in] event the listener event to be notified.
     */
    void callListener(const MdxListenerEvent& event);
private:

    MdxErrorCode mdxDeinitInternal();
    MdxErrorCode StopMdxDiscoveryInternal();
    MdxErrorCode StartMdxAdvertisingInternal();
    MdxErrorCode StopMdxAdvertisingInternal();

    bool startWebserver();
    void stopWebserver();
    bool webserverIsRunning() const;

    bool isMdxInterface(const std::string &name);

    enum InitIntent
    {
        DONT_INITIALIZE,
        INITIALIZE
    };

    enum AdvertisingIntent
    {
        DONT_ADVERTISE,
        ADVERTISE
    };

    enum RevealState
    {
        NOT_REVEALED,
        REVEALED
    };

    enum DiscoveryIntent
    {
        DONT_DISCOVER,
        DISCOVER
    };

    static MdxInternal*             pInstance;
    volatile State                  initState_;
    InitIntent                      initIntent_;
    AdvertisingState                advertisingState_;
    AdvertisingIntent               advertisingIntent_;
    DiscoveryState                  discoveryState_;
    DiscoveryIntent                 discoveryIntent_;
    RevealState                     revealState_;
    bool                            isInterfaceUp_;
    bool                            asyncHttpRequests_;
    bool                            enableWebSocketServer_;
    std::string                     networkInterface_;
    std::string                     targetServiceType_;
    std::vector<std::string>        discoverHeaderPatterns_;
    std::string                     uuid_;
    std::string                     friendlyName_;
    std::string			            manufacturer_;
    std::string			            modelName_;
    std::string                     ipAddress_;
    int                             advertisingPeriod_;
    int                             advertisingTTL_;
    int                             advertisingPort_;
    int                             listeningPort_;
    int                             numSsdpReplies_;
    int                             msgLimit_;
    std::string                     ssid_;
    vector<Listener*>               listeners_;
    // list of interfaces that we want to actively listen if they get set
    vector<std::string>             mdxInterfaces_;
    MdxController*                  mdxController_;
    MdxTarget*                      mdxTarget_;
    ReadWriteLock             stateRWLock_;
    Mutex                     listenerMutex_;
    CurlMultiThread * curlThread_;

    shared_ptr<mdx::MdxNetworkListener> mdxNetworkListener_;
private:

    MdxInternal();
};

}} // namespace netflix::mdx

#endif // MDX_INTERNAL_H
