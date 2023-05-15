/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "Mdx.h"

#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Log.h>
#include <nrdbase/Version.h>

#include "MdxInternal.h"

using namespace std;
using namespace netflix;
using namespace netflix::mdx;

DEFINE_TRACEAREA(MDX, mdx);
DEFINE_TRACEAREA(MDXLISTENER, mdx);
DEFINE_TRACEAREA(MDX_MONGOOSE, mdx);
DEFINE_TRACEAREA(MDX_MONGOOSE_REQUEST, mdx);

class MdxTracer
{
private:
    const char* const functionName_;

public:
    MdxTracer(const char* functionName) : functionName_(functionName)
    {
        MDXTRACE( "%s() begin", functionName_);
        NRDP_UNUSED(functionName_);
    }

    ~MdxTracer()
    {
        MDXTRACE( "%s() end", functionName_);
    }
};

Mdx::Mdx()
#if NRDP_VERSION_MAJOR > 2012 || (NRDP_VERSION_MAJOR == 2012 &&  NRDP_VERSION_MINOR >= 2)
    : controlMutex_(MDXCONTROL_MUTEX, "MdxControlMutex")
#endif
{
    mdxInternal_ = MdxInternal::getInstance();
}

Mdx::~Mdx()
{
    MdxInternal::destroyInstance();
}

MdxErrorCode Mdx::mdxConfigure(int advertisingPeriod,
                               int advertisingTTL,
                               int advertisingPort,
                               int listeningPort,
                               int numSsdpReplies,
                               int msgLimit)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("mdxConfigure");
    return mdxInternal_->mdxConfigure(advertisingPeriod,
                                      advertisingTTL,
                                      advertisingPort,
                                      listeningPort,
                                      numSsdpReplies,
                                      msgLimit);
}

MdxErrorCode Mdx::mdxInit(const std::string& host,
                          const std::string& serviceType,
                          const std::string& USN,
                          const std::string& friendlyName,
                          const std::string& manufacturer,
                          const std::string& modelName,
                          bool asyncHttpRequests,
                          bool enableWebSocketServer)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("mdxInit");
    return mdxInternal_->mdxInit(host,
                                 serviceType,
                                 USN,
                                 friendlyName,
                                 manufacturer,
                                 modelName,
                                 asyncHttpRequests,
                                 enableWebSocketServer);
}

MdxErrorCode Mdx::mdxDeinit()
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("Deinit");
    return mdxInternal_->mdxDeinit();
}

MdxErrorCode Mdx::mdxAddInterfaceName(const std::string &name)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("AddInterfaceName");
    return mdxInternal_->mdxAddInterfaceName(name);
}

MdxErrorCode Mdx::SearchForMdxDevices(const std::string& serviceType,
                                      const std::vector<std::string>& headerPatterns,
                                      int mx,
                                      int numSsdpSearches)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("SearchForMDxDevices");
    return mdxInternal_->SearchForMdxDevices(serviceType, headerPatterns, mx, numSsdpSearches);
}

MdxErrorCode Mdx::StopMdxDiscovery()
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("StopMdxDiscovery");
    return mdxInternal_->StopMdxDiscovery();
}

MdxErrorCode Mdx::RevealTargetPresence()
{
    ScopedMutex scopedMutexControL(controlMutex_);
    MdxTracer tracer("RevealTargetPresence");
    return mdxInternal_->RevealTargetPresence();
}

MdxErrorCode Mdx::SetDeviceReplyHeaders(const std::vector<std::string> &deviceReplyHeaders)
{
    ScopedMutex scopedMutexControL(controlMutex_);
    MdxTracer tracer("SetDeviceReplyHeaders");
    return mdxInternal_->SetDeviceReplyHeaders(deviceReplyHeaders);
}

MdxErrorCode Mdx::HideTargetPresence()
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("HideTargetPresence");
    return mdxInternal_->HideTargetPresence();
}

MdxErrorCode Mdx::StartMdxAdvertising()
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("StartMdxAdvertising");
    return mdxInternal_->StartMdxAdvertising();
}

MdxErrorCode Mdx::StopMdxAdvertising()
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("StopMdxAdvertising");
    return mdxInternal_->StopMdxAdvertising();
}

MdxErrorCode Mdx::SendMdxHttpRequest(const std::string& url,
                                     int method ,
                                     llong xid,
                                     int curltimeout,
                                     const std::string& headers,
                                     const std::string& body)
{
    MdxTracer tracer("SendMdxHttpRequest");
    return mdxInternal_->SendMdxHttpRequest(url, method, xid, curltimeout, headers, body);
}

MdxErrorCode Mdx::SendSessionMessage(const std::string &url,
                                     int method,
                                     llong xid,
                                     int curltimeout,
                                     shared_ptr<CryptContext> context,
                                     const std::string &headers,
                                     const std::string &body,
                                     const std::string &message,
                                     const std::string &plaintext)
{
    MdxTracer tracer("SendSessionMessage");
    return mdxInternal_->SendSessionMessage(url, method, xid, curltimeout, context, headers, body, message, plaintext);
}

MdxErrorCode Mdx::ProcessSessionMessage(shared_ptr<CryptContext> context,
                                        const std::string &message,
                                        const std::string &messageHmac,
                                        const std::string &ciphertext,
                                        std::string &outHmac,
                                        std::string &outPlaintext)
{
    MdxTracer tracer("ProcessSessionMessage");
    return mdxInternal_->ProcessSessionMessage(context, message, messageHmac, ciphertext, outHmac, outPlaintext);
}

MdxErrorCode Mdx::GetDialDeviceInfo(const std::string &url, const std::string &USN, const std::string &serviceType, int timeout)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("GetDialDeviceInfo");
    return mdxInternal_->GetDialDeviceInfo(url, USN, serviceType, timeout);
}

MdxErrorCode Mdx::SendWebSocketMessage(const std::string &host, ullong xid, const std::string &body)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("SendWebSocketMessage");
    return mdxInternal_->SendWebSocketMessage(host, xid, body);
}

void Mdx::InterfaceChanged(const std::string &newInterface, bool connected, const std::string &ipAddress, const std::string &ssid)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("InterfaceChanged");
    mdxInternal_->InterfaceChanged(newInterface, connected, ipAddress, ssid);
}

MdxErrorCode Mdx::addListener(Listener& l)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("addListener");
    return mdxInternal_->addListener(l);
}

MdxErrorCode Mdx::removeListener(Listener& l)
{
    ScopedMutex scopedMutexControl(controlMutex_);
    MdxTracer tracer("removeListener");
    return mdxInternal_->removeListener(l);
}

IMdx::State Mdx::getState() const
{
    return mdxInternal_->getState();
}

IMdx::AdvertisingState Mdx::getAdvertisingState() const
{
    return mdxInternal_->getAdvertisingState();
}

IMdx::DiscoveryState Mdx::getDiscoveryState() const
{
    return mdxInternal_->getDiscoveryState();
}

std::string Mdx::getInterface() const
{
    return mdxInternal_->getInterface();
}

std::string Mdx::getIpAddress() const
{
    return mdxInternal_->getIpAddress();
}
