/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MDX_H
#define MDX_H

#include <nrd/IMdx.h>

#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>

namespace netflix {
namespace mdx {

/**
 * @class Mdx Mdx.h
 * @brief Multi Device Experience Class.
 */
class Mdx : public IMdx
{
public:
    /** Constructor. */
    Mdx();

    /** Destructor. */
    virtual ~Mdx();

    //
    // IMdx interface implementation.
    //

    virtual MdxErrorCode mdxConfigure(int advertisingPeriod,
                                      int advertisingTTL,
                                      int advertisingPort,
                                      int listeningPort,
                                      int numSsdpReplies,
                                      int msgLimit);
    virtual MdxErrorCode mdxInit(const std::string& host,
                                 const std::string& serviceType,
                                 const std::string& USN,
                                 const std::string& friendlyName,
                                 const std::string& manufacturer,
                                 const std::string& modelName,
                                 bool asyncHttpRequests,
                                 bool enableWebSocketServer);
    virtual MdxErrorCode mdxDeinit();
    virtual MdxErrorCode mdxAddInterfaceName(const std::string &name);
    virtual MdxErrorCode SearchForMdxDevices(const std::string& serviceType, const std::vector<std::string>& headerPatterns, int mx, int numSsdpSearches);
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
    virtual void InterfaceChanged(const std::string &newInterface, bool connected, const std::string &ipAddress, const std::string &ssid);
    virtual MdxErrorCode addListener(Listener& l);
    virtual MdxErrorCode removeListener(Listener& l);
    virtual State getState() const;
    virtual AdvertisingState getAdvertisingState() const;
    virtual DiscoveryState getDiscoveryState() const;
    virtual std::string getInterface() const;
    virtual std::string getIpAddress() const;

private:
    IMdx* mdxInternal_;
    mutable Mutex controlMutex_;
};

}} // namespace netflix::mdx

#endif // MDX_H
