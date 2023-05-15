/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxBridge.h"
#include <nrd/Request.h>
#include <nrd/Response.h>
#include <nrd/ISystem.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/Version.h>
#include <nrdbase/UrlEncoder.h>
#include "Mdx.h"
#include "MdxCryptContext.h"
#include "MdxNetworkManager.h"
#include "MdxLog.h"
#include "MdxTracing.h"
#include "MdxUtil.h"
#include "MdxVersion.h"
#include <nrd/NrdApplication.h>
#include "MdxConfiguration.h"

using namespace std;
using namespace netflix;
using namespace netflix::device;
using namespace netflix::mdx;

#define MdxBridgeDevicePropertyList(Property)       \
    Property(interfaceName, NfObject::ReadOnly)     \
    Property(localIPAddress, NfObject::ReadOnly)    \
    Property(nativeVersion, NfObject::ReadOnly)     \
    Property(state, NfObject::ReadOnly)

PropertyTable(MdxBridge, MdxBridgeDevicePropertyList, properties)

#define MdxBridgeMethodList(Method)             \
    Method(AddInterfaceName)                    \
    Method(DialGetDeviceInfo)                   \
    Method(HideTargetPresence)                  \
    Method(InterfaceChanged)                    \
    Method(MdxConfigure)                        \
    Method(MdxDeinit)                           \
    Method(MdxInit)                             \
    Method(ProcessSessionMessage)               \
    Method(RevealTargetPresence)                \
    Method(SearchForMdxDevices)                 \
    Method(SendMdxHttpRequest)                  \
    Method(SendSessionMessage)                  \
    Method(SendWebSocketMessage)                \
    Method(SetDeviceReplyHeaders)               \
    Method(StartMdxAdvertising)                 \
    Method(StopMdxAdvertising)                  \
    Method(StopMdxDiscovery)                    \
    Method(beginContext)                        \
    Method(decrypt)                             \
    Method(encrypt)                             \
    Method(endContext)                          \
    Method(hmac)

MethodTable(MdxBridge, MdxBridgeMethodList);

MdxBridge::MdxBridge()
    : NfObject("mdx", Properties::MdxBridge::properties, Methods::MdxBridge::methods),
      mMdx(new Mdx())
    , mNetworkManager(new mdx::MdxNetworkManager)
    , mContextRWLock(MDXCONTEXT_MUTEX, "MdxContextMutex")
    , mContextIdx(0)
{
}

MdxBridge::~MdxBridge()
{
    mNetworkManager->stop();

    if(mMdx.get())
    {
        mMdx->removeListener(*this);
        mMdx->mdxDeinit();
    }

    NTRACE(TRACE_MDX, "MDX shutdown complete");
}

void MdxBridge::init()
{
    NTRACE(TRACE_MDX, "Starting MDX %s", MDXLIB_VERSION_STRING);

    if(mMdx.get()) {
        if(MdxError_OK == mMdx->addListener(*this)) {
            MDXTRACE("MdxBridge added listener");
        }
    } else {
        MDXTRACE("MdxBridge failed to add listener");
    }

    MdxErrorCode result = mNetworkManager->start(mMdx);

    if(MdxError_OK != result)
    {
        MDXERROR("Failed to initialize MdxNetworkManager");
    }

    NTRACE(TRACE_MDX, "MDX startup complete");

}

void MdxBridge::sendMdxEvent(const char *type)
{
    Variant args("type", type);

    EVENT("sendMdxEvent", "MDX", &args);

    sendEvent("IMdx", args);
}

void MdxBridge::sendMdxEvent(const Variant &map)
{
#ifdef MDXLIB_HAVE_TRACING
    Variant args(map);
    EVENT("sendMdxEvent", "MDX", &args);
#endif

    sendEvent("IMdx", map);
}

Variant MdxBridge::invoke(int method, const Variant &data)
{
    Variant returnValue;
    switch (method) {
    case Methods::MdxBridge::beginContext: {
        TIMER("beginContext", "MDX");

        bool ok;
        int index = data.mapValue<int>("idx", &ok);
        if (!ok) {
            invalidArgumentError(method, "idx");
            return false;
        }

        const string sharedSecret = data.mapValue<string>("sharedSecret");
        if (sharedSecret.empty()) {
            invalidArgumentError(method, "sharedSecret");
            return false;
        }

        ScopedReadWriteLock scope(mContextRWLock, ReadWriteLock::Write);

        char buf[16];
        const int written = snprintf(buf, 16, "c%lu", ++mContextIdx);
        string newContext = string(buf, written);

        vector<unsigned char> ssVector;
        ssVector.assign(sharedSecret.begin(), sharedSecret.end());

        mContexts[newContext].reset(new CryptContext(ssVector));

        Variant returnValue; // map sent to the callback if it exists
        returnValue["data"]["context"] = newContext;
        returnValue["idx"] = index;

        sendEvent("returnValue", returnValue);

        break; }
    case Methods::MdxBridge::decrypt: {
        TIMER("decrypt", "MDX");

        bool ok;
        int index = data.mapValue<int>("idx", &ok);
        const string context = data.mapValue<string>("context");
        const string payload = data.mapValue<string>("data");

        ScopedReadWriteLock scope(mContextRWLock, ReadWriteLock::Write);

        CryptContextMap::iterator it = mContexts.find(context);
        if (it == mContexts.end()) {
            invalidArgumentError(method, "context");
            return false;
        }

        shared_ptr<CryptContext> ctxt = it->second;

        Variant returnValue; // map sent to the callback if it exists
        returnValue["idx"] = index;
        std::vector<unsigned char> dec = ctxt->decrypt(vector<unsigned char>(payload.begin(), payload.end()));
        returnValue["data"] = string(dec.begin(), dec.end());
        sendEvent("returnValue", returnValue);
        break;
    }
    case Methods::MdxBridge::encrypt: {
        TIMER("encrypt", "MDX");

        bool ok;
        int index = data.mapValue<int>("idx", &ok);
        const string context = data.mapValue<string>("context");
        const string payload = data.mapValue<string>("data");

        ScopedReadWriteLock scope(mContextRWLock, ReadWriteLock::Write);

        CryptContextMap::iterator it = mContexts.find(context);
        if (it == mContexts.end()) {
            invalidArgumentError(method, "context");
            return false;
        }

        shared_ptr<CryptContext> ctxt = it->second;

        Variant returnValue; // map sent to the callback if it exists
        returnValue["idx"] = index;
        std::vector<unsigned char> enc = ctxt->encrypt(vector<unsigned char>(payload.begin(), payload.end()));
        returnValue["data"] = string(enc.begin(), enc.end());
        sendEvent("returnValue", returnValue);
        break;
    }
    case Methods::MdxBridge::endContext: {
        TIMER("endContext", "MDX");

        bool ok;
        int index = data.mapValue<int>("idx", &ok);
        const string context = data.mapValue<string>("context");

        ScopedReadWriteLock scope(mContextRWLock, ReadWriteLock::Write);

        CryptContextMap::iterator it = mContexts.find(context);
        if (it == mContexts.end()) {
            invalidArgumentError(method, "context");
            return false;
        }
        mContexts.erase(it);

        Variant returnValue; // map sent to the callback if it exists
        returnValue["idx"] = index;

        sendEvent("returnValue", returnValue);

        break; }
    case Methods::MdxBridge::hmac: {
        TIMER("hmac", "MDX");

        bool ok;
        int index = data.mapValue<int>("idx", &ok);
        const string context = data.mapValue<string>("context");
        const string payload = data.mapValue<string>("data");

        ScopedReadWriteLock scope(mContextRWLock, ReadWriteLock::Write);

        CryptContextMap::iterator it = mContexts.find(context);
        if (it == mContexts.end()) {
            invalidArgumentError(method, "context");
            return false;
        }

        shared_ptr<CryptContext> ctxt = it->second;

        Variant returnValue; // map sent to the callback if it exists
        returnValue["idx"] = index;
        std::vector<unsigned char> hmac;
        ctxt->calculateHmac(hmac, vector<unsigned char>(payload.begin(), payload.end()));
        returnValue["data"] = string(hmac.begin(), hmac.end());
        sendEvent("returnValue", returnValue);
        break;
    }
    case Methods::MdxBridge::DialGetDeviceInfo: {
        TIMER("DialGetDeviceInfo", "MDX");

        bool ok;

        const std::string& encodedurl = data.mapValue<std::string> ("url", &ok);
        if (!ok) {
            invalidArgumentError(method, "url");
            return false;
        }
        const std::string& url = UrlEncoder::decode(encodedurl.c_str());

        const std::string &encodedUSN = data.mapValue<std::string>("USN", &ok);
        if(!ok) {
            invalidArgumentError(method, "USN");
            return false;
        }
        const std::string &USN = UrlEncoder::decode(encodedUSN.c_str());

        const std::string &encodedServiceType = data.mapValue<std::string>("serviceType", &ok);
        if(!ok) {
            invalidArgumentError(method, "serviceType");
            return false;
        }
        const std::string &serviceType = UrlEncoder::decode(encodedServiceType.c_str());

        int timeout = data.mapValue<int>("timeout", &ok);
        if (!ok) {
            timeout = 10;
        }
        const MdxErrorCode err = mMdx->GetDialDeviceInfo(url, USN, serviceType, timeout);
        if(err != MdxError_OK) { MDXTRACE("IMdx::GetDialDeviceInfo returned %s", toString(err)); }
        break; }
    case Methods::MdxBridge::MdxConfigure: {
        TIMER("MdxConfigure", "MDX");

        bool ok;
        int advtPeriod = data.mapValue<int> ("advertisingPeriod", &ok);
        if (!ok) {
            invalidArgumentError(method, "advertisingPeriod");
            return false;
        }

        int advtTTL = data.mapValue<int> ("advertisingTTL", &ok);
        if (!ok) {
            invalidArgumentError(method, "advertisingTTL");
            return false;
        }

        int advtPort = data.mapValue<int> ("advertisingPort", &ok);
        if (!ok) {
            invalidArgumentError(method, "advertisingPort");
            return false;
        }

        int listeningPort = data.mapValue<int> ("listeningPort", &ok);
        if (!ok) {
            invalidArgumentError(method, "listeningPort");
            return false;
        }

        // optional
        int numSsdpReplies = data.mapValue<int> ("numSsdpReplies", &ok);
        if(!ok) {
            numSsdpReplies = -1;
        }

        int msgLimit = data.mapValue<int> ("msgLimit", &ok);
        if(!ok) {
            msgLimit = -1;
        }

        const MdxErrorCode err = mMdx->mdxConfigure(advtPeriod,
                                                    advtTTL,
                                                    advtPort,
                                                    listeningPort,
                                                    numSsdpReplies,
                                                    msgLimit);

        if(err != MdxError_OK) { MDXTRACE("IMdx::mdxConfigure returned %s", toString(err)); }
        break; }
    case Methods::MdxBridge::MdxInit: {
        TIMER("MdxInit", "MDX");

        bool ok;
        const std::string& encodedhost = data.mapValue<std::string> ("host", &ok);
        if (!ok) {
            invalidArgumentError(method, "host");
            return false;
        }
        const std::string& host = UrlEncoder::decode(encodedhost.c_str());

        const std::string& encodedserviceType = data.mapValue<std::string> (
            "serviceType", &ok);
        if (!ok) {
            invalidArgumentError(method, "serviceType");
            return false;
        }
        const std::string& serviceType = UrlEncoder::decode(encodedserviceType.c_str());

        const std::string& encodeduuid = data.mapValue<std::string> ("uuid", &ok);
        if (!ok) {
            invalidArgumentError(method, "uuid");
            return false;
        }
        const std::string& uuid = UrlEncoder::decode(encodeduuid.c_str());

#if NRDP_VERSION_MAJOR==2012 && NRDP_VERSION_MINOR==1
        const std::string& friendlyName = nrdLib->getSystemControl()->getFriendlyName();
        const std::string& manufacturer("");
        const std::string& modelName("");
#else
        const std::string& friendlyName = nrdApp()->getSystem()->getFriendlyName();
        const std::string& manufacturer = netflix::mdx::Configuration::getManufacturer();
        const std::string& modelName = netflix::mdx::Configuration::getModelName();
#endif

        const bool async = data.mapValue<bool>("asyncHttpRequests", &ok, false);
        const bool webSocketServer = data.mapValue<bool>("webSocketServer", &ok, false);

        const MdxErrorCode err = mMdx->mdxInit(
                        host,
                        serviceType,
                        uuid,
                        friendlyName,
                        manufacturer,
                        modelName,
                        async,
                        webSocketServer);

        if(err != MdxError_OK) { MDXTRACE("IMdx::mdxInit returned %s", toString(err)); }
        break; }
    case Methods::MdxBridge::MdxDeinit: {
        TIMER("MdxDeinit", "MDX");

        const MdxErrorCode err = mMdx->mdxDeinit();
        if(err != MdxError_OK) { MDXTRACE("IMdx::mdxDeinit returned %s", toString(err)); }
        break; }
    case Methods::MdxBridge::AddInterfaceName: {
        TIMER("AddInterfaceName", "MDX");

        bool ok;
        const std::string& encodedname = data.mapValue<std::string>("name", &ok);
        std::string name = UrlEncoder::decode(encodedname.c_str());
        const MdxErrorCode err = mMdx->mdxAddInterfaceName(name);
        if(err != MdxError_OK) { MDXTRACE("IMdx::mdxAddInterfaceName returned %s", toString(err)); }
        break; }
    case Methods::MdxBridge::SearchForMdxDevices: {
        TIMER("SearchForMdxDevices", "MDX");

        bool ok;
        const std::string& encodedserviceType = data.mapValue<std::string> (
            "serviceType", &ok);
        if (!ok) {
            invalidArgumentError(method, "serviceType");
            return false;
        }
        const std::string& serviceType = UrlEncoder::decode(encodedserviceType.c_str());

        std::vector<std::string> headerPatternsVec;
        Variant headerPatterns = data.value("headerPatterns");
        if(headerPatterns.isArray()) {
            for(int i = 0; i < headerPatterns.size(); i++) {
                const std::string &encodedHeader = headerPatterns[i].value<std::string>(&ok);
                if(!ok) {
                    invalidArgumentError(method, "headerPatterns");
                    return false;
                }
                headerPatternsVec.push_back(UrlEncoder::decode(encodedHeader.c_str()));
            }
        }

        // optional
        int mx = data.mapValue<int>("mx", &ok);
        if(!ok) {
            mx = 1;
        }

        // optional
        int numSsdpSearches = data.mapValue<int>("numSsdpSearches", &ok);
        if(!ok) {
            numSsdpSearches = -1;
        }

        const MdxErrorCode err = mMdx->SearchForMdxDevices(serviceType, headerPatternsVec, mx, numSsdpSearches);

        if(err != MdxError_OK) { MDXTRACE("IMdx::SearchForMdxDevices returned %s", toString(err)); }
        break; }
    case Methods::MdxBridge::StopMdxDiscovery: {
        bool ok;
        const int index = data.mapValue<int>("idx", &ok);
        TIMER("StopMdxDiscovery", "MDX");

        mMdx->StopMdxDiscovery();
        if (ok) {
            Variant returnValue; // map sent to the callback if it exists
            returnValue["idx"] = index;

            sendEvent("returnValue", returnValue);
        }
        break; }
    case Methods::MdxBridge::RevealTargetPresence: {
        TIMER("RevealTargetPresence", "MDX");

        bool ok;
        const int index  = data.mapValue<int>("idx", &ok);
        mMdx->RevealTargetPresence();
        if (ok) {
            Variant returnValue;
            returnValue["idx"] = index;
            sendEvent("returnValue", returnValue);
        }
        break; }
    case Methods::MdxBridge::SetDeviceReplyHeaders: {
        TIMER("SetDeviceReplyHeaders", "MDX");
        bool ok;
        const int index  = data.mapValue<int>("idx", &ok);
        std::vector<std::string> deviceReplyHeadersVec;

        Variant deviceReplyHeaders = data.value("deviceReplyHeaders");
        if(deviceReplyHeaders.isArray()) {
            for (int i = 0; i < deviceReplyHeaders.size(); i++) {
                bool headerOk;
                const std::string &encodedHeader = deviceReplyHeaders[i].value<std::string>(&headerOk);
                if (!headerOk) {
                    invalidArgumentError(method, "deviceReplyHeaders");
                    return false;
                }
                deviceReplyHeadersVec.push_back(UrlEncoder::decode(encodedHeader.c_str()));
            }
        }
        const MdxErrorCode err = mMdx->SetDeviceReplyHeaders(deviceReplyHeadersVec);

        if (err != MdxError_OK) { MDXTRACE("IMdx::SetDeviceReplyHeaders returned %s", toString(err)); }

        if (ok) {
            Variant returnValue;
            returnValue["idx"] = index;
            sendEvent("returnValue", returnValue);
        }

        break; }
    case Methods::MdxBridge::HideTargetPresence: {
        bool ok;
        const int index  = data.mapValue<int>("idx", &ok);
        TIMER("HideTargetPresence", "MDX");

        mMdx->HideTargetPresence();
        if (ok) {
            Variant returnValue;
            returnValue["idx"] = index;
            sendEvent("returnValue", returnValue);
        }
        break; }
    case Methods::MdxBridge::StartMdxAdvertising: {
        bool ok;
        const int index  = data.mapValue<int>("idx", &ok);
        TIMER("StartMdxAdvertising", "MDX");

        mMdx->StartMdxAdvertising();
        if (ok) {
            Variant returnValue;
            returnValue["idx"] = index;
            sendEvent("returnValue", returnValue);
        }
        break; }
    case Methods::MdxBridge::StopMdxAdvertising: {
        bool ok;
        const int index  = data.mapValue<int>("idx", &ok);
        TIMER("StopMdxAdvertising", "MDX");

        mMdx->StopMdxAdvertising();
        if (ok) {
            Variant returnValue;
            returnValue["idx"] = index;
            sendEvent("returnValue", returnValue);
        }
        break; }
    case Methods::MdxBridge::SendMdxHttpRequest: {
        bool ok;
        const std::string& encodedurl = data.mapValue<std::string> ("url", &ok);
        if (!ok) {
            invalidArgumentError(method, "url");
            return false;
        }
        const std::string& url = UrlEncoder::decode(encodedurl.c_str());
        const bool requestType = data.mapValue<int> ("requestType", &ok);
        if (!ok) {
            invalidArgumentError(method, "requestType");
            return false;
        }

        const double xid = data.mapValue<double> ("xid", &ok);
        if (!ok) {
            invalidArgumentError(method, "xid");
            return false;
        }

        int curltimeout = data.mapValue<int> ("curltimeout", &ok);
        if (!ok) {
            invalidArgumentError(method, "curltimeout");
            return false;
        }

        const std::string& encodedrequestHeader = data.mapValue<std::string> ("requestHeader", &ok);
        if (!ok) {
            invalidArgumentError(method, "requestHeader");
            return false;
        }
        const std::string& requestHeader = UrlEncoder::decode(encodedrequestHeader.c_str());

        const std::string& encodedrequestBody = data.mapValue<std::string> ("requestBody", &ok);
        if (!ok) {
            invalidArgumentError(method, "requestBody");
            return false;
        }
        const std::string& requestBody = UrlEncoder::decode(encodedrequestBody.c_str());
        EVENT_START("Message", xid, "MDX", "begin", &data);
        const MdxErrorCode err = mMdx->SendMdxHttpRequest(url, requestType, (llong)xid, curltimeout, requestHeader, requestBody);
        if(err != MdxError_OK) { MDXTRACE("IMdx::SendMdxHttpRequest returned %s", toString(err)); }
        break; }
    case Methods::MdxBridge::SendSessionMessage: {
        std::string url;
        std::string context;
        std::string requestHeader;
        std::string requestBody;
        std::string message;
        std::string plaintext;

        if(!getEncodedStringArg(method, data, "url", url))
        {
            return false;
        }

        if(!getEncodedStringArg(method, data, "context", context))
        {
            return false;
        }

        if(!getEncodedStringArg(method, data, "requestHeader", requestHeader))
        {
            return false;
        }

        if(!getEncodedStringArg(method, data, "requestBody", requestBody))
        {
            return false;
        }

        if(!getEncodedStringArg(method, data, "message", message))
        {
            return false;
        }

        if(!getEncodedStringArg(method, data, "plaintext", plaintext))
        {
            return false;
        }

        bool ok;
        const bool requestType = data.mapValue<int> ("requestType", &ok);
        if (!ok) {
            invalidArgumentError(method, "requestType");
            return false;
        }

        const double xid = data.mapValue<double> ("xid", &ok);
        if (!ok) {
            invalidArgumentError(method, "xid");
            return false;
        }

        int curltimeout = data.mapValue<int> ("curltimeout", &ok);
        if (!ok) {
            invalidArgumentError(method, "curltimeout");
            return false;
        }

        EVENT_START("Message", xid, "MDX", "SendSessionMessage", &data);

        ScopedReadWriteLock scope(mContextRWLock, ReadWriteLock::Write);
        shared_ptr<CryptContext> ctxt = getContext(context);
        if (!ctxt.get()) {
            Log::error(TRACE_MDX, "Invalid context %s for %s", context.c_str(), methodName(method));
        } else {
            const MdxErrorCode err = mMdx->SendSessionMessage(url,
                                                              requestType,
                                                              (llong)xid,
                                                              curltimeout,
                                                              ctxt,
                                                              requestHeader,
                                                              requestBody,
                                                              message,
                                                              plaintext);

            if(err != MdxError_OK) { MDXTRACE("IMdx::SendSessionMessage returned %s", toString(err)); }
        }
        break; }
    case Methods::MdxBridge::SendWebSocketMessage: {
        TIMER("SendWebSocketMessage", "MDX");

        std::string host;
        ullong xid;
        std::string body;
        bool ok;

        if(!getEncodedStringArg(method, data, "host", host))
        {
            return false;
        }

        xid = (ullong)data.mapValue<double> ("xid", &ok);
        if (!ok) {
            invalidArgumentError(method, "xid");
            return false;
        }

        if(!getEncodedStringArg(method, data, "body", body))
        {
            return false;
        }
        EVENT_START("Message", xid, "MDX", "begin", &data);
        const MdxErrorCode err = mMdx->SendWebSocketMessage(host, xid, body);
        if(err != MdxError_OK) { MDXTRACE("IMdx::SendWebSocketMessage returned %s", toString(err)); }
        break; }
    case Methods::MdxBridge::ProcessSessionMessage: {
        TIMER("ProcessSessionMessage", "MDX");

        const string index  = data.mapValue<string>("idx");

        string context;
        string message;
        string messageHmac;
        string ciphertext;
        string outHmac;
        string outPlaintext;

        if(!getEncodedStringArg(method, data, "context", context))
        {
            return false;
        }

        if(!getEncodedStringArg(method, data, "message", message))
        {
            return false;
        }

        if(!getEncodedStringArg(method, data, "messageHmac", messageHmac))
        {
            return false;
        }

        if(!getEncodedStringArg(method, data, "ciphertext", ciphertext))
        {
            return false;
        }

        Variant returnValue; // map sent to the callback if it exists
        returnValue["idx"]               = index;
        returnValue["data"]["hmac"]      = "";  // hmac and plaintext will be
        returnValue["data"]["plaintext"] = "";  // filled in if successful below

        ScopedReadWriteLock scope(mContextRWLock, ReadWriteLock::Write);
        shared_ptr<CryptContext> ctxt = getContext(context);
        if (!ctxt.get()) {
            Log::error(TRACE_MDX, "Invalid context %s for %s", context.c_str(), methodName(method));
        } else {
            if(MdxError_OK == mMdx->ProcessSessionMessage(ctxt, message, messageHmac, ciphertext, outHmac, outPlaintext)) {
                returnValue["data"]["hmac"] = outHmac;
                returnValue["data"]["plaintext"] = outPlaintext;
            }
        }
        if(!index.empty()) {
            sendEvent("returnValue", returnValue);
        }
        break; }
    case Methods::MdxBridge::InterfaceChanged: {
        TIMER("InterfaceChanged", "MDX");

        bool ok;
        const std::string& newInterface = data.mapValue<std::string> (
            "newInterface", &ok);
        if (!ok) {
            invalidArgumentError(method, "newInterface");
            return false;
        }

        bool connected = data.mapValue<bool>("connected", &ok);
        if(!ok)
        {
            invalidArgumentError(method, "connected");
            return false;
        }

        const std::string& ipAddress = data.mapValue<std::string> ("ipaddress", &ok);
        if (!ok) {
            invalidArgumentError(method, "ipaddress");
            return false;
        }

        const std::string& ssid = data.mapValue<std::string> ("ssid", &ok);
        if (!ok) {
            invalidArgumentError(method, "ssid");
            return false;
        }

        mMdx->InterfaceChanged(newInterface, connected, ipAddress, ssid);
        break; }

    default:
        return NfObject::invoke(method, data);
    }
    return true;
}

void MdxBridge::stateChanged(IMdx::State state, const std::string& ssid, int listeningPort)
{
    propertyUpdated(Properties::MdxBridge::state);

    Variant map;
    map["type"] = "statechanged";
    map["state"] = state;
    map["ssid"] = ssid;
    map["mdxport"] = listeningPort;
    sendMdxEvent(map);
}

void MdxBridge::advertisingStateChanged(IMdx::AdvertisingState state)
{
    Variant map;
    map["type"] = "advertisingstatechanged";
    map["state"] = state;
    sendMdxEvent(map);
}

void MdxBridge::discoveryStateChanged(IMdx::DiscoveryState state)
{
    Variant map;
    map["type"] = "discoverystatechanged";
    map["state"] = state;
    sendMdxEvent(map);
}

void MdxBridge::DeviceFound(const std::string& location,
                            const std::string& serviceType,
                            const std::string& USN,
                            const std::string& friendlyName,
                            const ResponseHeaders &responseHeaders,
                            bool activated,
                            bool paired)
{
    Variant map;
    map["type"]             = "devicefound";
    map["location"]         = UrlEncoder::encode(location);
    map["serviceType"]      = UrlEncoder::encode(serviceType);
    map["USN"]              = UrlEncoder::encode(USN);
    map["friendlyName"]     = UrlEncoder::encode(friendlyName);
    Variant headerMap;
    for(ResponseHeaders::const_iterator i = responseHeaders.begin(); i != responseHeaders.end(); ++i) {
        headerMap[i->first] = UrlEncoder::encode(i->second);
    }
    map["responseHeaders"]  = headerMap;
    map["activated"]        = activated;
    map["paired"]           = paired;
    sendMdxEvent(map);
}

void MdxBridge::DeviceByeBye(const std::string &serviceType, const std::string& USN)
{
    Variant map;
    map["type"] = "devicebyebye";
    map["serviceType"] = UrlEncoder::encode(serviceType);
    map["USN"] = UrlEncoder::encode(USN);
    sendMdxEvent(map);
}

void MdxBridge::HttpResponse(const std::string& url, int method , llong xid, int32_t responseCode, const std::string& headers, const std::string& body)
{
    Variant map;
    map["type"] = "httpresponse";
    map["url"] = UrlEncoder::encode(url);
    map["method"] = method;
    map["xid"] = (double)xid;
    map["code"] = responseCode;
    map["headers"] = headers;
    map["body"] = UrlEncoder::encode(body);

    sendMdxEvent(map);

#ifdef MDXLIB_HAVE_TRACING
    Variant args(map);
    EVENT_FINISH("Message", (ullong)xid, "MDX", "HttpResponse", &args);
#endif
}

void MdxBridge::IncomingMessage(const std::string& url, const Variant& headers, const std::string& body)
{
    Variant map;
    map["type"] = "incomingmessage";
    map["url"] = UrlEncoder::encode(url);
    map["headers"] = headers;
    map["body"] =  UrlEncoder::encode(body);
    sendMdxEvent(map);
}

void MdxBridge::error(MdxErrorCode error)
{
    Variant map;
    map["type"] = "error";
    map["error"] = int(error);
    sendMdxEvent(map);
}

bool MdxBridge::getProperty(int index, Variant *result) const
{
    switch (index)
    {
    case Properties::MdxBridge::state:
        *result = mMdx->getState();
        break;
    case Properties::MdxBridge::localIPAddress:
    {
        *result = mMdx->getIpAddress();
        break;
    }
    case Properties::MdxBridge::nativeVersion:
    {    *result = MDXLIB_VERSION_STRING;
        break;
    }
    case Properties::MdxBridge::interfaceName:
        *result = mMdx->getInterface();
        break;
    default:
        return false;
    }
    return true;
}

// This sends an event to the JavaScript layer to inform it that the interface has changed
void MdxBridge::interfaceChanged(const std::string &prevInterface, const std::string &newInterface, bool connected)
{
    propertyUpdated(Properties::MdxBridge::interfaceName);
    propertyUpdated(Properties::MdxBridge::localIPAddress);

    Variant map;
    map["type"] = "interfacechanged";
    map["prevInterface"] = UrlEncoder::encode(prevInterface);
    map["newInterface"] = UrlEncoder::encode(newInterface);
    map["connected"] = connected;
    sendMdxEvent(map);
}

void MdxBridge::DialDeviceInfoResponse(const std::string &url,
                                       const std::string &USN,
                                       const std::string &serviceType,
                                       const Variant &responseHeaders,
                                       const Variant &desc)
{
    Variant map;
    map["type"] = "DialDeviceInfoResponse";
    map["url"] = url;
    map["USN"] = USN;
    map["serviceType"] = serviceType;
    map["responseHeaders"] = responseHeaders;
    map["desc"] = desc;
    sendMdxEvent(map);
}

void MdxBridge::WebSocketMessage(const Variant& headers, const std::string& body)
{
    Variant map;
    map["type"] = "WebSocketMessage";
    map["headers"] = headers;
    map["body"] =  UrlEncoder::encode(body);
    sendMdxEvent(map);
}

void MdxBridge::webSocketConnect(const std::string &host)
{
    Variant map;
    map["type"] = "WebSocketConnect";
    map["host"] = host;
    sendMdxEvent(map);
}

void MdxBridge::webSocketDisconnect(const std::string &host)
{
    Variant map;
    map["type"] = "WebSocketDisconnect";
    map["host"] = host;
    sendMdxEvent(map);
}

void MdxBridge::webSocketSent(const std::string &host, ullong xid)
{
    Variant map;
    map["type"] = "WebSocketSent";
    map["host"] = host;
    map["xid"] = xid;
    map["body"] = "";
    sendMdxEvent(map);

#ifdef MDXLIB_HAVE_TRACING
    Variant args(map);
    EVENT_FINISH("Message", (ullong)xid, "MDX", "WebSocketSent", &args);
#endif
}

bool MdxBridge::getEncodedStringArg(int method, const Variant &data, const std::string &name, std::string &arg) {
    bool ok;
    const std::string& encodedArg = data.mapValue<std::string> (name.c_str(), &ok);
    if (!ok) {
        invalidArgumentError(method, name.c_str());
        return false;
    }
    arg = UrlEncoder::decode(encodedArg.c_str());
    return true;
}

shared_ptr<CryptContext> MdxBridge::getContext(const string &context) const
{
    CryptContextMap::const_iterator it = mContexts.find(context);
    if (it != mContexts.end())
        return (*it).second;
    return shared_ptr<CryptContext>();
}
