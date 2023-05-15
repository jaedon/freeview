/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxInternal.h"
#include "MdxCryptContext.h"
#include "MdxGuard.h"
#include "MdxHttpClient.h"
#include "MdxWebSocketConnections.h"
#include "MdxTracing.h"
#include "WebSocketMdx.h"
#include "MdxLog.h"
#include "MdxVersion.h"
#include <cstring>
#include <locale>
#include <sstream>
#include <algorithm>
#include <nrd/NrdApplication.h>
#include "MdxMongooseNFLX.h"

#include <nrd/ISystem.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/Variant.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Log.h>
#include <nrdbase/Assert.h>
#include <nrdbase/Base64.h>
#include <nrdbase/Thread.h>
#include <nrdbase/UrlEncoder.h>
#include <nrdbase/Version.h>
#include <nrdnet/CurlMultiThread.h>

using namespace netflix;
using namespace netflix::device;
using namespace netflix::mdx;

static std::string getUserAgentString();

//DECLARE_THREAD(MDX_CURL_MULTI)
DEFINE_THREAD(MDX_CURL_MULTI, 0, 0);

MdxInternal* MdxInternal::pInstance = 0;

MdxInternal::MdxInternal()
    : initState_(NOT_INITIALIZED)
    , initIntent_(DONT_INITIALIZE)
    , advertisingState_(NOT_ADVERTISING)
    , advertisingIntent_(DONT_ADVERTISE)
    , discoveryState_(NOT_DISCOVERING)
    , discoveryIntent_(DONT_DISCOVER)
    , revealState_(NOT_REVEALED)
    , isInterfaceUp_(false)
    , asyncHttpRequests_(false)
    , enableWebSocketServer_(false)
    , networkInterface_("none")
    , advertisingPeriod_(30)
    , advertisingTTL_(1)
    , advertisingPort_(1900)
    , listeningPort_(9080)
    , numSsdpReplies_(-1)
    , msgLimit_(-1)
    , ssid_("")
    , mdxController_(NULL)
    , mdxTarget_(NULL)
#if NRDP_VERSION_MAJOR > 2012 || (NRDP_VERSION_MAJOR == 2012 &&  NRDP_VERSION_MINOR >= 2)
    , stateRWLock_(MDXINTERNALSTATE_MUTEX, "MdxInternalState")
    , listenerMutex_(MDXINTERNALLISTENER_MUTEX, "MdxInternalListener")
#endif
    , curlThread_(NULL)
{
    MDXTRACE("MdxInternal created");
}


MdxInternal* MdxInternal::getInstance()
{
    if(pInstance == 0){
        pInstance = new MdxInternal();
    }
    return pInstance;
}

void MdxInternal::destroyInstance()
{
    if(pInstance != NULL){
        delete pInstance;
        pInstance = NULL;
    }
}

MdxInternal::~MdxInternal()
{
    if(mdxController_) {
        mdxController_->Stop();
        MdxController::destroyInstance();
    }

    if(mdxTarget_) {
        MdxTarget::destroyInstance();
    }

    MdxGuard::shutdown();

    MDXTRACE("MdxInternal destroyed");
}

MdxErrorCode MdxInternal::mdxConfigure(int advertisingPeriod,
                                       int advertisingTTL,
                                       int advertisingPort,
                                       int listeningPort,
                                       int numSsdpReplies,
                                       int msgLimit)
{
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Write);
    MDXTRACE("MdxInternal::mdxConfigure");

    advertisingPeriod_  = advertisingPeriod;    // time between advertisements
    advertisingTTL_     = advertisingTTL;       // ttl for advertisements
    advertisingPort_    = advertisingPort;      // port for SSDP (typically 1900)
    listeningPort_      = listeningPort;        // port for mongoose to listen to (and to advertise we are listening to)
    numSsdpReplies_     = numSsdpReplies;       // number of SSDP reply packets to emit when replying to searches or when advertising
    msgLimit_           = msgLimit;             // maximum size of post data in an incoming message

    return MdxError_OK;
}

MdxErrorCode MdxInternal::mdxInit(const std::string& host,
                                  const std::string& targetServiceType,
                                  const std::string& uuid,
                                  const std::string& friendlyName,
                                  const std::string& manufacturer,
                                  const std::string& modelName,
                                  bool asyncHttpRequests,
                                  bool enableWebSocketServer)
{
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Write);
    MDXTRACE("MdxInternal::mdxInit");
    initIntent_             = INITIALIZE;
    uuid_                   = uuid;
    friendlyName_           = friendlyName;
    manufacturer_           = manufacturer;
    modelName_              = modelName;
    ipAddress_              = host;
    asyncHttpRequests_      = asyncHttpRequests;
    enableWebSocketServer_  = enableWebSocketServer;

    if (isInterfaceUp_) {
        if (initState_ != INITIALIZED) {

            if(asyncHttpRequests_)
            {
                if(!curlThread_)
                {
                    curlThread_ = new CurlMultiThread(&THREAD_MDX_CURL_MULTI);
                    if(!curlThread_)
                    {
                        return MdxError_OutOfMemory;
                    }
                }
            }

            if(!webserverIsRunning()) {
                if(!startWebserver()) {
                    MDXERROR("Error initializing mongoose");
                    error(MdxError_MongooseInitFailed);
                    return MdxError_MongooseInitFailed;
                }
                // get the port that the webserver actually started listening on, because it can be overridden
                listeningPort_ = MdxMongooseNFLX::instance()->getListeningPort();
            }

            MDXTRACE("Initializing UPnP\n"
                     "\tipaddress = %s ssdp port = %u mdx port = %u",
                     host.c_str(), advertisingPort_, listeningPort_);

            int ret = UpnpInit(host.c_str(), advertisingPort_, advertisingTTL_);

            if (ret != UPNP_E_SUCCESS)  {
                if (ret != UPNP_E_INIT)  {
                    UpnpFinish();

                    MDXERROR("Error with UpnpInit -- %d %s", ret, UpnpGetErrorMessage(ret));
                    error(MdxError_UpnpInitFailed);
                    return MdxError_UpnpInitFailed;
                }
            }
            else
            {
                mdxController_ = MdxController::getInstance();

                targetServiceType_ = targetServiceType;
                std::string USN = "uuid:" + uuid;

                mdxTarget_ = MdxTarget::getInstance(
                        advertisingPeriod_,
                        listeningPort_,
                        numSsdpReplies_,
                        USN,
                        friendlyName,
                        manufacturer,
                        modelName );

                initState_ = INITIALIZED;
            }
        }

        callListener(StateChangedEvent(initState_, ssid_, listeningPort_));
    } else {
        MDXINFO("MdxInternal::mdxInit network not available, not initializing");
    }
    return MdxError_OK;
}

MdxErrorCode MdxInternal::mdxDeinit()
{
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Write);
    MDXTRACE("MdxInternal::mdxDeinit");
    initIntent_ = DONT_INITIALIZE;

    return mdxDeinitInternal();
}

MdxErrorCode MdxInternal::mdxDeinitInternal()
{
    if(initState_ == INITIALIZED)
    {
        StopMdxDiscovery();
        StopMdxAdvertising();
        HideTargetPresence();

        mdxController_ = NULL;
        MdxController::destroyInstance();
        mdxTarget_ = NULL;
        MdxTarget::destroyInstance();

        MDXTRACE("Stopping UPnP");
        UpnpFinish();

        stopWebserver();

        delete curlThread_;
        curlThread_ = NULL;

        initState_ = NOT_INITIALIZED;
        callListener(StateChangedEvent(initState_, ssid_, listeningPort_));
    }

    return MdxError_OK;
}

MdxErrorCode MdxInternal::mdxAddInterfaceName(const std::string &name)
{
    for(vector<std::string>::const_iterator i = mdxInterfaces_.begin(); i != mdxInterfaces_.end(); ++i)
    {
        if(*i == name)
        {
            // already in the list
            return MdxError_OK;
        }
    }
    MDXTRACE("MdxInternal::mdxAddInterfaceName adding %s", name.c_str());
    mdxInterfaces_.push_back(name);

    return MdxError_OK;
}

//================================================================================
// IMdx interface implementation.
//================================================================================
MdxErrorCode MdxInternal::SearchForMdxDevices(const std::string& serviceType,
                                              const std::vector<std::string> &headerPatterns,
                                              int mx,
                                              int numSsdpSearches)
{
    int rc;
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Write);
    discoveryIntent_ = DISCOVER;

    if(initState_ != INITIALIZED)
    {
        MDXERROR("MdxInternal::SearchForMdxDevices MDX is not initialized");
        return MdxError_BadCall;
    }

    if (discoveryState_ != DISCOVERING) {
        MDXTRACE("MdxInternal::SearchForMdxDevices starting discovery");
        rc = mdxController_->Start(headerPatterns);
        if (rc != MDX_SUCCESS) {
            MDXERROR("Error starting UPnP MDX Control Point: %d", rc);

            error(MdxError_StartControlPointFailed);
            return MdxError_StartControlPointFailed;
        }
        discoveryState_ = DISCOVERING;
        callListener(DiscoveryStateChangedEvent(discoveryState_));
    }

    if(discoveryState_ == DISCOVERING) {
        rc = mdxController_->Search(serviceType, mx, numSsdpSearches);
        if(rc != MDX_SUCCESS) {
            MDXERROR("Error searching for MDX devices: %d", rc);
            return MdxError_SearchFailed;
        }
    }
    return MdxError_OK;
}

MdxErrorCode MdxInternal::StopMdxDiscovery()
{
    discoveryIntent_ = DONT_DISCOVER;
    return StopMdxDiscoveryInternal();
}

MdxErrorCode MdxInternal::StopMdxDiscoveryInternal()
{
    int rc;
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Write);
    if (discoveryState_ == DISCOVERING) {
        MDXTRACE("MdxInternal::StopMdxDiscovery stopping discovery");
        rc = mdxController_->Stop();
        if (rc != MDX_SUCCESS) {
            MDXERROR("Error stopping UPnP MDX Control Point: %d", rc);
        }
        discoveryState_ = NOT_DISCOVERING;
        callListener(DiscoveryStateChangedEvent(discoveryState_));
    }
    return MdxError_OK;
}

MdxErrorCode MdxInternal::RevealTargetPresence()
{
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Write);
    if (initState_ != INITIALIZED)
    {
        MDXERROR("MdxInternal::RevealTargetPresence MDX is not initialized");
        return MdxError_BadCall;
    }

    if (revealState_ == NOT_REVEALED)
    {
        MDXTRACE("MdxInternal::RevealTargetPresence revealing presence");
        int rc = mdxTarget_->RevealPresence(targetServiceType_);

        if (rc != MDX_SUCCESS)
        {
            MDXERROR("Error revealing presence");
            return MdxError_UpnpCallFailed;
        }

        revealState_ = REVEALED;
    }

    return MdxError_OK;
}

MdxErrorCode MdxInternal::SetDeviceReplyHeaders(const std::vector<std::string> &deviceReplyHeaders)
{
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Write);
    std::string msg = "MdxInternal::SetDeviceReplyHeaders";
    for(vector<string>::const_iterator i = deviceReplyHeaders.begin(); i != deviceReplyHeaders.end(); ++i)
    {
        msg += "\n";
        msg += i->c_str();
    }
    MDXTRACE(msg.c_str());
    if (initState_ != INITIALIZED)
    {
        MDXERROR("MdxInternal::SetDeviceReplyHeaders MDX is not initialized");
        return MdxError_BadCall;
    }

    if (revealState_ == NOT_REVEALED)
    {
        MDXERROR("Not revealed, cannot set device reply headers");
        return MdxError_BadCall;
    }

    int rc = mdxTarget_->SetDeviceReplyHeaders(deviceReplyHeaders);

    if (rc != MDX_SUCCESS)
    {
        MDXERROR("Error setting device reply headers: %d", rc);
        return MdxError_UpnpCallFailed;
    }

    return MdxError_OK;
}

MdxErrorCode MdxInternal::HideTargetPresence()
{
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Write);

    if (initState_ != INITIALIZED)
    {
        MDXERROR("MdxInternal::HideTargetPresence MDX is not initialized");
        return MdxError_BadCall;
    }

    if (revealState_ == REVEALED)
    {
        MDXTRACE("MdxInternal::HideTargetPresence hiding presence");
        int rc = mdxTarget_->HidePresence();

        if (rc != MDX_SUCCESS)
        {
            MDXERROR("Error hiding presence: %d", rc);
            return MdxError_UpnpCallFailed;
        }

        revealState_ = NOT_REVEALED;
    }

    return MdxError_OK;
}

MdxErrorCode MdxInternal::StartMdxAdvertising()
{
    advertisingIntent_ = ADVERTISE;
    return StartMdxAdvertisingInternal();
}

MdxErrorCode MdxInternal::StartMdxAdvertisingInternal()
{
    int rc;
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Write);

    if (initState_ != INITIALIZED)
    {
        MDXERROR("MdxInternal::StartMdxAdvertising MDX is not initialized");
        return MdxError_BadCall;
    }

    if (advertisingState_ == NOT_ADVERTISING) {
        MDXTRACE("MdxInternal::StartMdxAdvertising");
        rc = mdxTarget_->MdxTargetSendAdvt();
        if (rc != MDX_SUCCESS) {
            MDXERROR("Error starting Advertisements: %d", rc);

            error(MdxError_StartAdvertisementFailed);
            return MdxError_StartAdvertisementFailed;
        }
        advertisingState_ = ADVERTISING;
        callListener(AdvertisingStateChangedEvent(advertisingState_));
    }
    return MdxError_OK;
}

MdxErrorCode MdxInternal::StopMdxAdvertising()
{
    advertisingIntent_ = DONT_ADVERTISE;
    return StopMdxAdvertisingInternal();
}

MdxErrorCode MdxInternal::StopMdxAdvertisingInternal()
{
    int rc;
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Write);

    if (advertisingState_ == ADVERTISING) {
        MDXTRACE("MdxInternal::StopMdxAdvertising");
        rc = mdxTarget_->MdxTargetStopAdvt();
        if (rc != MDX_SUCCESS) {
            MDXERROR("Error stopping UPnP Advertisements: %d", rc);
        }
        advertisingState_ = NOT_ADVERTISING;
        callListener(AdvertisingStateChangedEvent(advertisingState_));
    }
    return MdxError_OK;
}

MdxErrorCode MdxInternal::SendMdxHttpRequest(const std::string& url,
                                             int method ,
                                             llong xid,
                                             int curltimeout,
                                             const std::string& headers,
                                             const std::string& body)
{
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Read);
    if (initState_ != INITIALIZED)
    {
        MDXERROR("MdxInternal::SendMdxHttpRequest MDX is not initialized, cannot send message");
        return MdxError_BadCall;
    }

    if(asyncHttpRequests_)
    {
        assert(curlThread_);
        EVENT_STEP("Message", xid, "MDX", "MdxHttpRequestAsync", NULL);
        MdxGuard::sendMdxHttpRequestAsync(curlThread_,
                                          url,
                                          headers,
                                          body,
                                          xid,
                                          curltimeout,
                                          method,
                                          getUserAgentString());
    }
    else
    {
        EVENT_STEP("Message", xid, "MDX", "MdxHttpRequest", NULL);
        MdxGuard::sendMdxHttpRequest(url,
                                     headers,
                                     body,
                                     xid,
                                     curltimeout,
                                     method,
                                     getUserAgentString());
    }

    return MdxError_OK;
}

MdxErrorCode MdxInternal::SendSessionMessage(const std::string &url,
                                             int method,
                                             llong xid,
                                             int curltimeout,
                                             shared_ptr<CryptContext> context, // context to use for crypto operations
                                             const std::string &headers,       // http headers
                                             const std::string &body,          // body of message
                                             const std::string &message,       // message for HMAC purposes (already canonicalized)
                                             const std::string &plaintext)     // plaintext to be encrypted prior to HMAC
{
    ScopedReadWriteLock scopedRWLockState(stateRWLock_, ReadWriteLock::Read);
    if (initState_ != INITIALIZED)
    {
        MDXERROR("MdxInternal::SendSessionMessage MDX is not initialized, cannot send message");
        return MdxError_BadCall;
    }

    const std::string responseHeaders = "";

    // we will look for the key "ciphertext=ciphertext", and replace it with
    // "ciphertext=<encrypted plaintext>" in both the message
    // (for HMAC calculation) and the body (for sending)
    const std::string key = "ciphertext=ciphertext";
    std::string newKey;
    std::string newBody = body;
    std::string newMessage = message;
    size_t insertPos = newBody.find(key);

    if(insertPos == std::string::npos)
    {
        // http response is the mechanism by which the results of sending mdx http requests are reported
        std::string responseBody = std::string("error ") + toString(MdxError_CiphertextInsertPos);
        MdxInternal::getInstance()->callListener(HttpResponseEvent(url, method, xid, 0, responseHeaders, responseBody));
        return MdxError_CiphertextInsertPos;
    }

    // encrypt the plaintext
    EVENT_STEP("Message", xid, "MDX", "Encrypt", NULL);

    std::vector<unsigned char> vPlaintext(plaintext.begin(), plaintext.end());
    std::vector<unsigned char> vCiphertext = context->encrypt(vPlaintext);
    std::string ciphertext(vCiphertext.begin(), vCiphertext.end());

    if(!ciphertext.length())
    {
        // http response is the mechanism by which the results of sending mdx http requests are reported
        std::string responseBody = std::string("error  ") + toString(MdxError_EncryptFailed);
        MdxInternal::getInstance()->callListener(HttpResponseEvent(url, method, xid, 0, responseHeaders, responseBody));
        return MdxError_EncryptFailed;
    }

    newKey = std::string("ciphertext=") + ciphertext;

    // replace ciphertext
    newBody.replace(insertPos, key.length(), newKey);

    insertPos = message.find(key);
    if(insertPos == std::string::npos)
    {
        // http response is the mechanism by which the results of sending mdx http requests are reported
        std::string responseBody = std::string("error ") + toString(MdxError_CiphertextInsertPos);
        MdxInternal::getInstance()->callListener(HttpResponseEvent(url, method, xid, 0, responseHeaders, responseBody));
        return MdxError_CiphertextInsertPos;
    }

    // replace the key with the encrypted plaintext
    newKey = std::string("ciphertext=") + UrlEncoder::encode(ciphertext);
    newMessage.replace(insertPos, key.length(), newKey);

    // HMAC the canonicalized message
    EVENT_STEP("Message", xid, "MDX", "HMAC", NULL);

    std::vector<unsigned char> vNewMessage(newMessage.begin(), newMessage.end());
    std::vector<unsigned char> vHmac;
    if (!context->calculateHmac(vHmac, vNewMessage) || !vHmac.size()) {
        // http response is the mechanism by which the results of sending mdx http requests are reported
        std::string responseBody = std::string("error ") + toString(MdxError_HmacFailed);
        MdxInternal::getInstance()->callListener(HttpResponseEvent(url, method, xid, 0, responseHeaders, responseBody));
        return MdxError_HmacFailed;
    }

    std::string hmac(vHmac.begin(), vHmac.end());

    // include the hmac field in the body of the message
    newBody += std::string("hmac=") + hmac + "\r\n";

    // send the message
    if(url.find("ws://") == 0) {
        // WebSocket just takes the host
        std::string host = url.substr(5, std::string::npos);
        return SendWebSocketMessage(host, xid, newBody);
    } else {
        return SendMdxHttpRequest(url, method, xid, curltimeout, headers, newBody);
    }
}

MdxErrorCode MdxInternal::ProcessSessionMessage(shared_ptr<CryptContext> context,
                                                const std::string &message,
                                                const std::string &messageHmac,
                                                const std::string &ciphertext,
                                                std::string &outHmac,
                                                std::string &outPlaintext)
{
    outHmac = "";
    outPlaintext = "";

    std::vector<unsigned char> vMessage(message.begin(), message.end());
    std::vector<unsigned char> vHmac;
    if (!context->calculateHmac(vHmac, vMessage)) {
        error(MdxError_HmacFailed);
        return MdxError_HmacFailed;
    }

    // don't perform the decrypt if the calculated hmac doesn't match the message's hmac
    // but do return the calculated hmac, in case the caller would like to report it
    outHmac = string(vHmac.begin(), vHmac.end());

    if(messageHmac != outHmac) {
        MDXTRACE("MdxInternal::ProcessSessionMessage: HMAC mismatch, not decrypting");
        return MdxError_OK;
    }

    std::vector<unsigned char> vCiphertext(ciphertext.begin(), ciphertext.end());
    std::vector<unsigned char> vPlaintext = context->decrypt(vCiphertext);

    outPlaintext = string(vPlaintext.begin(), vPlaintext.end());

    return MdxError_OK;
}

MdxErrorCode MdxInternal::GetDialDeviceInfo(const std::string &url,
                                            const std::string &USN,
                                            const std::string &serviceType,
                                            int timeout)
{
    if (initState_ != INITIALIZED)
    {
        MDXERROR("MdxInternal::GetDialDeviceInfo MDX not initialized");
        return MdxError_BadCall;
    }

    Variant responseHeaders;
    Variant desc;

    MdxHttpClient dispatch(url, "", "", timeout, 0, true, getUserAgentString());
    if (dispatch.execute() && dispatch.getResponseCode() == 200) {
        std::string responseBody = dispatch.getResponse();

        // parse response headers
        HttpHeadersToVariantMap headersFormatter;
        responseHeaders = headersFormatter.format(dispatch.getResponseHeaders());

        // parse response xml
        XmlToVariantMap descFormatter;

        desc = descFormatter.format(responseBody);

        callListener(DialDeviceInfoResponseEvent(url, USN, serviceType, responseHeaders, desc));
    } else {
        MDXERROR("Error with GetDialDeviceInfo -- %s %d %s %s",
                 url.c_str(),
                 dispatch.getResponseCode(),
                 dispatch.getLastResultString().c_str(),
                 dispatch.getResponse().c_str());

        // return empty headers and description indicates failure
        callListener(DialDeviceInfoResponseEvent(url, USN, serviceType, responseHeaders, desc));
    }

    return MdxError_OK;
}

MdxErrorCode MdxInternal::SendWebSocketMessage(const std::string &host, ullong xid, const std::string &body)
{
    EVENT_STEP("Message", xid, "MDX", "SendWebSocketMessage", NULL);
    MDXTRACE("SendWebSocketMessage %s %llu", host.c_str(), xid);

    shared_ptr<MdxServer> pServer = MdxWebSocketConnections::getConnection(host);

    if(!pServer.get())
    {
        MDXTRACE("SendWebSocketMessage %s %llu No Connection", host.c_str(), xid);
        return MdxError_NoConnection;
    }

    if(!pServer->queue(xid, body))
    {
        MDXTRACE("SendWebSocketMessage %s %llu could not queue message", host.c_str(), xid);
        return MdxError_NoConnection;
    }

    return MdxError_OK;
}

void MdxInternal::InterfaceChanged(const std::string &newInterface, bool connected, const std::string &ipaddress, const std::string &ssid)
{
    if (!isMdxInterface(newInterface)) {
        // If it is not one of allowed MDX interface, ignore this event. MDX-362
        MDXTRACE("MdxInternal::InterfaceChanged not %s is not an mdx interface, ignoring ...", newInterface.c_str());
        return;
    }

    // If we get notified of a connection to a different SSID, without having
    // been told about a disconnection from the existing SSID, perform a
    // disconnection scenario first, then carry out this connection
    if(isInterfaceUp_ && connected && ssid != ssid_) {
        InterfaceChanged(networkInterface_, false, ipAddress_, ssid_);
    }

    std::string oldInterface = networkInterface_;
    bool wasInterfaceUp      = isInterfaceUp_;
    std::string oldSsid      = ssid_;
    std::string oldIpAddress = ipAddress_;

    networkInterface_ = newInterface;
    ssid_             = ssid;
    ipAddress_        = ipaddress;

    // if we were initialized when the interface changed, we need to do things
    // like start or stop mongoose, advertising, and discovery
    if (connected && (ipaddress != "") ) {
        isInterfaceUp_ = true;
        MDXTRACE("setting allowed interface = true.");
    } else {
        isInterfaceUp_ = false;
        MDXTRACE("setting allowed interface = false.");
    }

    callListener(InterfaceChangedEvent(oldInterface, newInterface, isInterfaceUp_));

    if (isInterfaceUp_) {
        if (initIntent_ == INITIALIZE) {
            //initialize
            if  (initState_ != INITIALIZED) {
                MDXINFO("MDX was deinitialized. Initializing on interface change from (%s, %s, %s) to (%s, %s, %s)",
                        oldInterface.c_str(),
                        wasInterfaceUp ? "connected" : "disconnected",
                        oldIpAddress.c_str(),
                        newInterface.c_str(),
                        isInterfaceUp_ ? "connected" : "disconnected",
                        ipaddress.c_str());
                if ( mdxInit(
                            ipaddress,
                            targetServiceType_,
                            uuid_,
                            friendlyName_,
                            manufacturer_,
                            modelName_,
                            asyncHttpRequests_,
                            enableWebSocketServer_) == MdxError_OK ) {
                    if(advertisingIntent_ == ADVERTISE && advertisingState_ == NOT_ADVERTISING)
                    {
                        MDXTRACE("StartMdxAdvertising on interface change from (%s, %s, %s) to (%s, %s, %s)",
                                 oldInterface.c_str(),
                                 wasInterfaceUp ? "connected" : "disconnected",
                                 oldIpAddress.c_str(),
                                 newInterface.c_str(),
                                 isInterfaceUp_ ? "connected" : "disconnected",
                                 ipaddress.c_str());
                        StartMdxAdvertisingInternal();
                    }
                }
            } else if(!wasInterfaceUp) {
                // This should never happen. Do we have to re-initialize here?
                MDXWARN("MDX is already initialized on interface change from (%s, %s, %s) to (%s, %s, %s)",
                        oldInterface.c_str(),
                        wasInterfaceUp ? "connected" : "disconnected",
                        oldIpAddress.c_str(),
                        newInterface.c_str(),
                        isInterfaceUp_ ? "connected" : "disconnected",
                        ipaddress.c_str());
            }
        }
    } else {
        // de init and set states
        if(initState_ == INITIALIZED) {
            MDXINFO("MDX was initialized. DeInitializing on interface change from (%s, %s, %s) to (%s, %s, %s)",
                    oldInterface.c_str(),
                    wasInterfaceUp ? "connected" : "disconnected",
                    oldIpAddress.c_str(),
                    newInterface.c_str(),
                    isInterfaceUp_ ? "connected" : "disconnected",
                    ipaddress.c_str());

            mdxDeinitInternal();
        }
    }
}

MdxErrorCode MdxInternal::addListener(IMdx::Listener& l)
{
    ScopedMutex scopedMutexListener(listenerMutex_);
    MDXTRACE("adding listener");
    vector<IMdx::Listener*>::iterator n = find(listeners_.begin(), listeners_.end(), &l);

    if(n == listeners_.end())
        listeners_.push_back(&l);

    return MdxError_OK;
}

MdxErrorCode MdxInternal::removeListener(IMdx::Listener& l)
{
    ScopedMutex scopedMutexListener(listenerMutex_);
    vector<IMdx::Listener*>::iterator n = find(listeners_.begin(), listeners_.end(), &l);

    if(n != listeners_.end())
        listeners_.erase(n);

    return MdxError_OK;
}

string MdxInternal::getIpAddress() const
{
    return ipAddress_;
}

void MdxInternal::error(MdxErrorCode error)
{
    callListener(ErrorEvent(error));
}

void MdxInternal::callListener(const MdxListenerEvent& event)
{
    TIMER("MdxInternal::callListener", "MDX");
    ScopedMutex scopedMutexListener(listenerMutex_);

    uint32_t i;
    for(i = 0; i < listeners_.size(); i ++)
        event.fireTo(listeners_[i]);
}

bool MdxInternal::startWebserver()
{
    MdxMongooseNFLX *mdxMongoose = new MdxMongooseNFLX;
    assert(MdxMongooseNFLX::instance() == mdxMongoose);
    NRDP_UNUSED(mdxMongoose);

    MDXTRACE("Starting mongoose");
    if(msgLimit_ > 0)
    {
        MdxMongooseNFLX::instance()->setMaximumPostSize(msgLimit_);
    }
    if (!MdxMongooseNFLX::instance()->start(listeningPort_, enableWebSocketServer_)) {
        delete mdxMongoose;
        assert(!MdxMongooseNFLX::instance());
        return false;
    }
    return true;
}

void MdxInternal::stopWebserver()
{
    MdxMongooseNFLX *mdxMongoose = MdxMongooseNFLX::instance();

    if(mdxMongoose)
    {
        MDXTRACE("Stopping mongoose");
        mdxMongoose->stop();
        delete mdxMongoose;
    }
}

bool MdxInternal::webserverIsRunning() const
{
    MdxMongooseNFLX *mdxMongoose = MdxMongooseNFLX::instance();

    return mdxMongoose && mdxMongoose->isRunning();
}

bool MdxInternal::isMdxInterface(const std::string &name)
{
    for(vector<std::string>::const_iterator i = mdxInterfaces_.begin(); i != mdxInterfaces_.end(); ++i)
    {
        if(name == *i)
        {
            return true;
        }
    }

    return false;
}

std::string getUserAgentString()
{
    static std::string s_userAgent;
    if (s_userAgent.empty())
    {
        if (shared_ptr<ISystem> system = nrdApp()->getSystem()) {
            s_userAgent = StringFormatter::sformat(
                "Netflix/%s MDX/%s (DEVTYPE=%s; CERTVER=%d)",
                NRDP_VERSION_NUMBER, MDXLIB_VERSION_NUMBER,
                system->getSoftwareVersion().c_str(), system->getCertificationVersion());
        }
    }
    return s_userAgent;
}
