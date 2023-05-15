/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NrdpBridge.h"

#include <nrd/config.h>
#include <nrd/NrdApplication.h>
#include <nrdnet/ResourceManager.h>

#include "AudioBridge.h"
#include "DeviceBridge.h"
#include "DrmSystemBridge.h"
#include "InstrumentationBridge.h"
#include "LogBridge.h"
#include "MediaRequestBridge.h"
#ifdef NRDP_ASE
#include "PlayerBridge.h"
#endif
#include "MediaSourcePlayerFactoryBridge.h"
#include "NrdpMediaSource.h"
#include "StorageBridge.h"
#include "TextToSpeechBridge.h"
#include "SystemBridge.h"
#include "NetworkBridge.h"
#include "NrdConfiguration.h"
#ifdef NRDP_HAS_PARTNER_INTEGRATION
#  include "TileBridge.h"
#endif
#ifdef NRDP_MDX_ENABLED
# include <nrd/MdxBridge.h>
#endif
#include "WebCryptoBridge.h"
#include "WebSocketBridge.h"
#ifdef BUILD_QA
# include "QATextToSpeechBridge.h"
#ifdef NRDP_ASE
#  include "QALocationMonitorBridge.h"
# endif
#endif

#include "SystemData.h"

using namespace netflix;
using namespace netflix::mediacontrol;

#define NrdpPropertyList(Property)                      \
    Property(capabilities, NfObject::ReadOnly)          \
    Property(config, NfObject::ReadOnly)                \
    Property(configList, NfObject::ReadOnly)            \
    Property(debug, NfObject::ReadOnly)                 \
    Property(hasSuspendHandler, NfObject::ReadWrite)    \
    Property(options, NfObject::ReadOnly)               \
    Property(started, NfObject::ReadOnly)               \
    Property(suspended, NfObject::ReadOnly)             \
    Property(trustStoreHash, NfObject::ReadOnly)

PropertyTable(NrdpBridge, NrdpPropertyList, properties)

#define MethodList(Method)                      \
    Method(assert)                              \
    Method(exit)                                \
    Method(locks)                               \
    Method(now)                                 \
    Method(objects)                             \
    Method(parseJSON)                           \
    Method(parseXML)                            \
    Method(ping)                                \
    Method(requestSuspend)                      \
    Method(setConfigData)                       \
    Method(setConfigurationOverrides)           \
    Method(setServerTime)                       \
    Method(setTestDriverIpAddress)              \
    Method(setTrustStore)                       \
    Method(suspendComplete)

MethodTable(NrdpBridge, MethodList)

NrdpBridge::NrdpBridge()
    : NfObject("nrdp",
               Properties::NrdpBridge::properties, Properties::NrdpBridge::PropertyCount,
               Methods::NrdpBridge::methods, Methods::NrdpBridge::MethodCount)
{
    mSuspendReason = Configuration::suspendModeOnStart();
}

void NrdpBridge::init()
{
    NTRACE(TRACE_SUSPEND, "NrdpBridge::init() Suspend reason: \"%s\"", mSuspendReason.c_str());
    shared_ptr<NfObject> that = shared_from_this();
    audioBridge.reset(new AudioBridge);
    audioBridge->setParent(that);
    deviceBridge.reset(new DeviceBridge);
    deviceBridge->init();
    deviceBridge->setParent(that);
#if defined(NRDP_ASE)
    playerBridge.reset(new PlayerBridge);
    playerBridge->setParent(that);
#endif
    textToSpeechBridge.reset(new TextToSpeechBridge);
    textToSpeechBridge->setParent(that);
#ifdef BUILD_QA
    qaBridge.reset(new NfObject("qa"));
    shared_ptr<QATextToSpeechBridge> qat2sb(new QATextToSpeechBridge);
    qat2sb->setParent(qaBridge);
#if defined(NRDP_ASE)
    shared_ptr<NfObject> locMonBridge(new QALocationMonitorBridge);
    locMonBridge->setParent(qaBridge);
#endif
    qaBridge->setParent(that);
#endif
    logBridge.reset(new LogBridge);
    logBridge->setParent(that);
    logBridge->welcome();
    drmSystemBridge.reset(new DrmSystemBridge);
    drmSystemBridge->setParent(that);
    storageBridge.reset(new StorageBridge);
    storageBridge->init();
    storageBridge->setParent(that);
    instrumentationBridge.reset(new InstrumentationBridge);
    instrumentationBridge->init();
    instrumentationBridge->setParent(that);
    webCryptoBridge.reset(new WebCryptoBridge);
    webCryptoBridge->init();
    webCryptoBridge->setParent(that);
    webSocketBridge.reset(new WebSocketBridge);
    webSocketBridge->setParent(that);
    networkBridge.reset(new NetworkBridge);
    networkBridge->setParent(that);
    systemBridge.reset(new SystemBridge);
    systemBridge->setParent(that);
#ifdef NRDP_HAS_PARTNER_INTEGRATION
    tileBridge.reset(new TileBridge);
    tileBridge->init();
    tileBridge->setParent(that);
#endif
#ifdef NRDP_MDX_ENABLED
    if (NrdConfiguration::mdxEnabled()) {
        mdxBridge.reset(new MdxBridge);
        mdxBridge->init();
        mdxBridge->setParent(that);
    }
#endif
    mediaSourceManager.reset(new NrdpMediaSourceManager);

    mediaSourcePlayerFactoryBridge.reset(new MediaSourcePlayerFactoryBridge);
    mediaSourcePlayerFactoryBridge->setParent(that);

    mediaRequestBridge.reset(new MediaRequestBridge(mediaSourceManager,
                                                    mediaSourcePlayerFactoryBridge));
    mediaRequestBridge->init();
    mediaRequestBridge->setParent(that);

    startEventListener();
}

void NrdpBridge::shutdown()
{
#ifdef NRDP_ASE
    if (playerBridge)
        playerBridge->shutdown();
#endif
    if (storageBridge)
        storageBridge->shutdown();

    if (logBridge)
        logBridge->shutdown();

    if (mediaSourcePlayerFactoryBridge)
        mediaSourcePlayerFactoryBridge->shutdown();

    if (mediaSourceManager)
        mediaSourceManager.reset();

    if (mediaRequestBridge)
        mediaRequestBridge->deinit();

    if (networkBridge)
        networkBridge->shutdown();
}

unsigned NrdpBridge::requestFlags(const Request &request) const
{
    unsigned ret = Request::None;
    switch (request.method()) {
    case Methods::NrdpBridge::assert:
    case Methods::NrdpBridge::ping:
    case Methods::NrdpBridge::now:
    case Methods::NrdpBridge::exit:
    case Methods::NrdpBridge::setServerTime:
    case Methods::NrdpBridge::setTestDriverIpAddress:
    case Methods::NrdpBridge::setTrustStore:
    case Methods::NrdpBridge::setConfigurationOverrides:
        ret |= Request::Synchronous;
        break;
    default: {
        if (request.property() == Properties::NrdpBridge::hasSuspendHandler)
            ret |= Request::Synchronous;
        break; }
    }
    return ret;
}

Variant NrdpBridge::invoke(int method, const Variant &var)
{
#define READ_VAR(type, name)                                            \
    var.mapValue<type>(name, &ok);                                      \
    do { if (!ok) { invalidArgumentError(method, name); return false; } } while(0)

    Variant ret;
    bool ok;
    switch (method) {
    case Methods::NrdpBridge::assert:
        assert(0);
        break;
    case Methods::NrdpBridge::requestSuspend:
        NTRACE(TRACE_SUSPEND, "NrdpBridge::requestSuspend(\"%s\")", var.mapValue<std::string>("reason", 0, "UI_ENGINE").c_str());
        nrdApp()->requestSuspend(var.mapValue<std::string>("reason", 0, "UI_ENGINE"));
        break;
    case Methods::NrdpBridge::suspendComplete:
        NTRACE(TRACE_SUSPEND, "NrdpBridge::suspendComplete()");
        nrdApp()->suspendComplete();
        break;
    case Methods::NrdpBridge::setConfigurationOverrides: {
        const std::string xml = READ_VAR(std::string, "xml");
        ret["success"] = Configuration::setOverrideContents(xml);
        return ret; }
    case Methods::NrdpBridge::setConfigData: {
        const std::string name = READ_VAR(std::string, "name");
        const Variant data = var.value("data");
        if (!data.isStringMap()) {
            invalidArgumentError(method, "data");
            return false;
        }
        std::map<std::string, std::string> params;
        const Variant::StringMapIterator end = data.stringMapEnd();
        for(Variant::StringMapIterator it = data.stringMapBegin(); it != end; ++it)
            params[it->first] = it->second.value<std::string>();
        nrdApp()->systemData()->setServiceParameters(name, params);

        if(name == "streaming"){
            std::map<std::string,std::string>::const_iterator it = params.find("maxNumReusedSocketConnection");
            if(it != params.end()){
                // found streaming parameter
                uint32_t numSocket =
                    strtoul(mapValue<std::string>(params, "maxNumReusedSocketConnection").c_str(), NULL, 10);
                Configuration::setMaxNumReusedSocketConnection(numSocket);
            }
        }

        break; }
    case Methods::NrdpBridge::locks: {
        const int id = READ_VAR(int, "id");
        const unsigned int flags = READ_VAR(unsigned int, "flags");
        Variant data;
        Variant locks;
        Mutex::dumpVariant(DumpInfo(), flags, &locks);
        data["id"] = id;
        data["locks"] = locks;
        sendEvent("locks", data);
        break; }
    case Methods::NrdpBridge::now:
        return Time::serverTime().ms();
    case Methods::NrdpBridge::objects: {
        const int id = READ_VAR(int, "id");
        Variant data;
        Variant objects;
        ObjectCount::dump(ObjectCount::None, std::string(), &objects);
        data["id"] = id;
        data["objects"] = objects;
        sendEvent("objects", data);
        break; }
    case Methods::NrdpBridge::exit: {
        const int code = var.mapValue<int>("code", 0, 0);
#if !defined(NRDP_EXIT)
        Log::warn(TRACE_SYSTEM, "'nrdp.exit()' called, please note that this should not be used in production UIs on this platform");
#endif
        nrdApp()->scheduleQuit(code);
        break; }
    case Methods::NrdpBridge::parseXML: {
        const int id = READ_VAR(int, "id");

        Variant::StringMapIterator it = var.find("xml");
        if (it == var.stringMapEnd()) {
            invalidArgumentError(method, "xml");
            return false;
        }

        const char *xml = it->second.c_str();
        if (!xml) {
            invalidArgumentError(method, "xml");
            return false;
        }

        Variant data;
        data["id"] = id;
        bool success;
        const Variant object = Variant::fromXML(xml, it->second.size(), &success);
        data["success"] = success;
        if (success)
            data["object"] = object;
        sendEvent("parsedXML", data);
        break; }
    case Methods::NrdpBridge::ping: {
        const int id = READ_VAR(int, "id");
        Variant data;
        data["id"] = id;
        data["received"] = Time::monoMS();
        sendEvent("pong", data);
        break; }
    case Methods::NrdpBridge::parseJSON: {
        const int id = READ_VAR(int, "id");

        Variant::StringMapIterator it = var.find("json");
        if (it == var.stringMapEnd()) {
            invalidArgumentError(method, "json");
            return false;
        }

        const char *json = it->second.c_str();
        if (!json) {
            invalidArgumentError(method, "json");
            return false;
        }

        const int length = it->second.size();
        Variant varr;
        int pos = 0;
        while (pos < length) {
            int used;
            Variant v = Variant::fromJSON(json + pos, length - pos, &ok, &used);
            if (!ok)
                break;
            pos += used;
            varr.push_back(v);
        }

        Variant data;
        data["id"] = id;
        data["object"] = varr;
        sendEvent("parsedJSON", data);
        break; }

    case Methods::NrdpBridge::setTrustStore: {
        const std::string trustStore = READ_VAR(std::string, "trustStore");
        nrdApp()->setAppTrustStore(trustStore);
        break; }
    case Methods::NrdpBridge::setServerTime: {
        const ullong time = READ_VAR(ullong, "time");
        nrdApp()->setServerTime(Time::fromSeconds(time));
        break; }
    case Methods::NrdpBridge::setTestDriverIpAddress: {
        const std::string address = READ_VAR(std::string, "address");
        nrdApp()->setTestDriverIpAddress(address);
        break; }
    default:
        return NfObject::invoke(method, var);
    }
    return ret;
}

bool NrdpBridge::setProperty(int index, const Variant &value)
{
    switch (index) {
    case Properties::NrdpBridge::hasSuspendHandler:
        NTRACE(TRACE_SUSPEND, "Setting hasSuspendHandler to %d", value.value<bool>());
        nrdApp()->setHasSuspendHandler(value.value<bool>());
        break;
    }
    return true;
}

static Variant makeCapabilitiesVariant(const Variant &variant)
{
    if(variant.isString()) {
        Variant result;
        bool ok;
        result = variant.value<int>(&ok);
        if(ok)
            return result;
        result = variant.value<double>(&ok);
        if(ok)
            return result;
        result = variant.value<bool>(&ok);
        if(ok)
            return result;
    } else if(variant.isStringMap()) {
        Variant result;
        const Variant::StringMapIterator end = variant.stringMapEnd();
        for(Variant::StringMapIterator it = variant.stringMapBegin(); it != end; ++it)
            result[(*it).first] = makeCapabilitiesVariant((*it).second);
        return result;
    } else if(variant.isNull()) {
        return std::string();
    }
    return variant;
}

bool NrdpBridge::getProperty(int index, Variant *variant) const
{
    switch (index) {
    case Properties::NrdpBridge::capabilities:
        *variant = makeCapabilitiesVariant(Configuration::configDataValues().value("capabilities"));
        return true;
    case Properties::NrdpBridge::config: {
        shared_ptr<SystemData> sysdata = nrdApp()->systemData();
        std::vector<std::string> files = sysdata->serviceFiles();
        std::vector<std::string>::iterator it;
        for (it = files.begin(); it != files.end(); ++it) {
            (*variant)[*it] = sysdata->serviceParameters(*it);
        }
        return true; }
    case Properties::NrdpBridge::configList:
        *variant = nrdApp()->systemData()->serviceFiles();
        return true;
    case Properties::NrdpBridge::debug:
#ifdef BUILD_DEBUG
        *variant = true;
#else
        *variant = false;
#endif
        return true;
    case Properties::NrdpBridge::hasSuspendHandler:
        *variant = nrdApp()->hasSuspendHandler();
        return true;
    case Properties::NrdpBridge::options: {
        const ConfigurationOptions &options = Configuration::finalOptionValues();
        for (ConfigurationOptions::const_iterator it = options.begin(); it != options.end(); ++it) {
            if (it->longOption) {
                if (!strcmp(it->longOption, "boot-url")) {
                    const Variant val = it->variantArgument();
                    variant->insert("boot_url", val.mapValue<std::string>("url"));
                    variant->insert("boot_request", val);
                    // maintain compatibility for nrdjs
                } else if (!strcmp(it->longOption, "ui-url")) {
                    const Variant val = it->variantArgument();
                    variant->insert("ui_url", val.mapValue<std::string>("url"));
                    variant->insert("ui_request", val);
                    // maintain compatibility for nrdjs
                } else {
                    variant->insert(Configuration::jsOption(it->longOption), it->variantArgument());
                }
            }
        }

        return true; }
    case Properties::NrdpBridge::suspended:
        if(mSuspendReason.empty()) //compatibility
            *variant = false;
        else
            *variant = mSuspendReason;
        return true;
    case Properties::NrdpBridge::started:
        *variant = nrdApp()->monoApplicationStarted().ms();
        return true;
    case Properties::NrdpBridge::trustStoreHash: {
        shared_ptr<TrustStore> store = nrdApp()->resourceManager()->trustStore();
        if (store) {
            *variant = store->getHash();
        }
        return true; }
    default:
        break;
    }
    return false;
}

void NrdpBridge::event(ApplicationEvent::Type type, const Variant &data)
{
    switch (type) {
    case ApplicationEvent::Event_Suspend:
        setSuspended(true, data);
        break;
    case ApplicationEvent::Event_Resume:
        setSuspended(false, data);
        break;
    default:
        break;
    }
}

void NrdpBridge::setSuspended(bool suspend, const Variant &data)
{
    Variant event;
    if(data.isStringMap())
        event = data;
    else if(data.isString())
        event["reason"] = data.value<std::string>();

    if(suspend) {
        mSuspendTime = Time::mono();
        mSuspendReason = event["reason"].value<std::string>();
        if(mSuspendReason.empty())
            mSuspendReason = "suspend";
    } else {
        if(!event.contains("duration"))
            event["duration"] = (Time::mono() - mSuspendTime).seconds();
        mSuspendTime = Time();
        mSuspendReason.clear();
    }
    event["suspend"] = suspend;
    NTRACE(TRACE_SUSPEND, "NrdpBridge::setSuspended(%s)", event.toJSON().c_str());
    propertyUpdated(Properties::NrdpBridge::suspended);
    sendEvent("suspendChanged", event);
}

void NrdpBridge::trustStoreUpdated()
{
    propertyUpdated(Properties::NrdpBridge::trustStoreHash);
}
