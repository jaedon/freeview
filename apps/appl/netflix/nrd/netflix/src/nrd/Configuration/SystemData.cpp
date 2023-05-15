/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "SystemData.h"

#include <nrdbase/Base64.h>
#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringCompressor.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdnet/IpConnectivityManager.h>
#include <nrdbase/DigestAlgo.h>
#include <nrdbase/Digester.h>
#include <nrdbase/Random.h>

#include "ConfigurationKeys.h"
#include "DiskStore.h"
#include "ISystem.h"
#include "NrdApplication.h"
#include "SecureStore.h"

using namespace netflix;
using namespace netflix::device;
using namespace netflix::util;

namespace { // anonymous

//////////
//
// helpers
//
//////////

ullong generateRandom()
{
    ullong id = 0;

    // upper 32 bits will be predicted server time
    uint32_t serverTime = Time::serverTime().seconds();
    id |= serverTime & 0xffffffff;
    id <<= 32;

    // lower 32 bits will be random
    uint32_t randomBits;
    Random::next((unsigned char *)(&randomBits), sizeof(randomBits));
    id |= randomBits & 0xffffffff;

    return id;
}

} // anonymous namespace

//////////
//
// SystemData
//
//////////
SystemData::SystemData()
    : mx_(SYSTEMDATA_MUTEX, "SystemData"), paramMutex_(SYSTEMDATAPARAM_MUTEX, "SystemDataParam"),
      networkInterfaceInfoIsValid_(false)
{
    NTRACE(TRACE_SYSTEM, "reset()");
    {
        ScopedMutex scopedMutex(paramMutex_);

        instrumentationEnabled_ = true;
        instrumentationEvents_.clear();

        // inflate the persisted value
        std::string raw(nrdApp()->secureStore()->get<std::string>(SystemKeys::appTrustStore));
        StringCompressor::inflateB64(/*out*/ appTrustStore_, raw);
    }
    // create a applicatoinID and a sessionID
    setAppID(generateRandom());
    resetSessionID();
}

void SystemData::addListener(const shared_ptr<SystemDataListener> &listener)
{
    assert(listener.get());
    ScopedMutex lock(mx_);
    listeners_.insert(listener);
}

void SystemData::removeListener(const shared_ptr<SystemDataListener> &listener)
{
    assert(listener);
    ScopedMutex lock(mx_);
    listeners_.erase(listener);
}

void SystemData::setAppID(ullong appID)
{
    NTRACE(TRACE_SYSTEM, "Setting appID to %lld", appID);
    ScopedMutex scopedMutex(paramMutex_);
    // appid is stored in Log
    Log::setAppId(appID);
}

ullong SystemData::resetSessionID()
{
    ullong result = generateRandom();
    NTRACE(TRACE_SYSTEM, "Setting sessionID to %lld", result);
    {
        ScopedMutex scopedMutex(paramMutex_);
        // sessoinid is stored in Log
        Log::setSessionId(result);
    }
    return result;
}

void SystemData::resetAppID(ullong appID) { setAppID(appID ? appID : generateRandom()); }

std::vector<std::string> SystemData::languagePreferences() const
{
    const std::string value = nrdApp()->secureStore()->get<std::string>(SystemKeys::UILanguages);

    std::vector<std::string> tokens;
    StringTokenizer::tokenize(value, tokens, std::string(1, ','));
    return tokens;
}

void SystemData::setLanguagePreferences(const std::vector<std::string> &languages)
{
    std::string value;
    size_t size = languages.size();
    for (size_t i = 0; i < size; ++i) {
        value += languages[i];
        if (i < size - 1)
            value += ",";
    }
    nrdApp()->secureStore()->set(SystemKeys::UILanguages, value);
    notifyListeners(SystemDataKey::UI_LANGUAGE);
}

void SystemData::setInstrumentationEnabled(bool enabled)
{
    ScopedMutex scopedMutex(paramMutex_);
    if (instrumentationEnabled_ == enabled)
        return;

    instrumentationEnabled_ = enabled;
}

bool SystemData::instrumentationEnabled() const
{
    ScopedMutex lock(paramMutex_);
    return instrumentationEnabled_;
}

void SystemData::setInstrumentationEvents(const std::string &events)
{
    ScopedMutex scopedMutex(paramMutex_);
    if (instrumentationEvents_ == events)
        return;

    instrumentationEvents_ = events;
}

std::string SystemData::instrumentationEvents() const
{
    ScopedMutex lock(paramMutex_);
    return instrumentationEvents_;
}

// parameters received from config service

void SystemData::addServiceFile(const std::string &file)
{
    ScopedMutex scopedMutex(paramMutex_);
    serviceParamsList_.push_back(file);
}

std::vector<std::string> SystemData::serviceFiles() const
{
    ScopedMutex scopedMutex(paramMutex_);
    return serviceParamsList_;
}

void SystemData::setServiceParameters(const std::string &file, const std::map<std::string, std::string> &params)
{
    {
        ScopedMutex scopedMutex(paramMutex_);
        serviceParams_[file] = params;
    }

#ifdef BUILD_DEBUG
    for (std::map<std::string, std::string>::const_iterator it = params.begin(); it != params.end(); it++) {
        NTRACE(TRACE_SYSTEM, "TRACE: Setting %s = %s in %s \n", (*it).first.c_str(), (*it).second.c_str(), file.c_str());
    }
#endif

    for (std::map<std::string, std::string>::const_iterator it = params.begin(); it != params.end(); it++) {
        if (it->first == "preferIpV6Flag") {
            NTRACE(TRACE_SYSTEM, "TRACE: Setting %s = %s in %s \n", (*it).first.c_str(), (*it).second.c_str(), file.c_str());
            nrdApp()->updateDeviceBridgeNetworkProperty();
            break;
        }
    }

    notifyListeners(SystemDataKey::SYSTEM_FILE);
}

std::map<std::string, std::string> SystemData::serviceParameters(const std::string &file)
{
    ScopedMutex scopedMutex(paramMutex_);
    return serviceParams_[file];
}

std::string SystemData::appTrustStore() const
{
    ScopedMutex lock(paramMutex_);
    return appTrustStore_;
}

void SystemData::setAppTrustStore(const std::string &trustStore)
{
    ScopedMutex lock(paramMutex_);

    appTrustStore_ = trustStore;

    // compress the std::string
    std::string encoded;
    StringCompressor::deflateB64(/*out*/ encoded, trustStore);
    // see if we fit in our budget
    uint32_t budget = appTrustStoreBudget();
    if (encoded.length() > budget) {
        Log::warn(TRACE_SECURE_STORE, "WARNING: app trust store exceeds budgeted size: %zu > %u", encoded.length(), budget);
    } else {
        nrdApp()->secureStore()->set(SystemKeys::appTrustStore, encoded);
    }
}

// Method checks whether the network interface info has been fetched from the
// device's ISystem implementation yet. If it hasn't, the method fetches the
// interface data. Since, the network interface info is updated any time the
// device's system implementation raises the interface changed event, the
// interface info returned here will always be up-to-date.
NetworkInterfaceInfo SystemData::getNetworkInterfaceInfo()
{
    ScopedMutex scopedMutex(paramMutex_);
    if (networkInterfaceInfoIsValid_ == false) {

        // Update both the interface settings and the ipversion preference and
        // policy.
        updateNetworkInterfaceInfo();
    } else {

        // Update only the ipversion preference and policy. The interface
        // settings will be updated on a the NrdApplication system event thread if they
        // change.
        updateIpVersionsPreferenceAndPolicy();
    }
    return networkInterfaceInfo_;
}

void SystemData::updateIpVersionsPreferenceAndPolicy()
{
    // ip version avaialble to network interface
    networkInterfaceInfo_.ipVersionsAvailable_ = ipConnectivityModeToString(nrdApp()->getSystem()->getIpConnectivityMode());

    // ip connection preference
    std::map<std::string, std::string> networkServiceParams = serviceParameters("networking");

    if (networkServiceParams.find("preferIpV6Flag") != networkServiceParams.end()) {
        if (networkServiceParams.find("preferIpV6Flag")->second == "true" || networkServiceParams.find("preferIpV6Flag")->second == "1") {
            networkInterfaceInfo_.ipVersionPreferred_ = "p6";
            IpConnectivityManager::instance()->setPreferIpV6Flag(true);
            // change connectivity policy based on preference
            if (IpConnectivityManager::instance()->getIpConnectivityPolicy() == IpConnectivityManager::IP_V4_V6) {
                IpConnectivityManager::instance()->setIpConnectivityPolicy(IpConnectivityManager::IP_V6_V4);
            }
        } else {
            networkInterfaceInfo_.ipVersionPreferred_ = "p4";
            IpConnectivityManager::instance()->setPreferIpV6Flag(false);
            // change connectivity policy based on preference
            if (IpConnectivityManager::instance()->getIpConnectivityPolicy() == IpConnectivityManager::IP_V6_V4) {
                IpConnectivityManager::instance()->setIpConnectivityPolicy(IpConnectivityManager::IP_V4_V6);
            }
        }
    } else {
        networkInterfaceInfo_.ipVersionPreferred_ = "default";
    }

    // policy can be 1)ipv4 only, 2)ipv6 only, 3)ipv4 -> ipv6, or 4)ipv6->ipv4
    switch (IpConnectivityManager::instance()->getIpConnectivityPolicy()) {
    case IpConnectivityManager::IP_V4_ONLY:
        networkInterfaceInfo_.ipVersionPolicy_ = "4";
        break;
    case IpConnectivityManager::IP_V4_V6:
        networkInterfaceInfo_.ipVersionPolicy_ = "p4";
        break;
    case IpConnectivityManager::IP_V6_ONLY:
        networkInterfaceInfo_.ipVersionPolicy_ = "6";
        break;
    case IpConnectivityManager::IP_V6_V4:
        networkInterfaceInfo_.ipVersionPolicy_ = "p6";
        break;
    default:
        break;
    }
}

void SystemData::updateNetworkInterfaceInfo()
{
    ScopedMutex scopedMutex(paramMutex_);
    std::stringstream countryCodeSStream;
    std::stringstream networkCodeSStream;
    std::stringstream wirelessChannelSStream;

    // Find the interface that's in use.
    std::vector<struct ISystem::NetworkInterface> interfaces = nrdApp()->getSystem()->getNetworkInterfaces();
    std::vector<struct ISystem::NetworkInterface>::const_iterator it;
    for (it = interfaces.begin(); it != interfaces.end(); it++) {
        if (it->isDefault) {
            break;
        }
    }

    // Check if there is no default interface.
    if (it == interfaces.end()) {
        // If the last call to updateNetworkInterfaceInfo didn't yield the "no
        // default interface" case, log an error and clear any interface info we
        // may have from when we were connected.
        if (networkInterfaceInfo_.defaultInterfaceName_ != "none") {
            networkInterfaceInfo_.reset();
            networkInterfaceInfo_.defaultInterfaceName_ = "none";
            scopedMutex.unlock(); // Unlock before the log since the log sinks may lock additional mutexes
            Log::error(TRACE_LOG, "SystemData::updateNetworkInterfaceInfo(): no default interface");
        }

        return;
    }

    networkInterfaceInfo_.defaultInterfaceName_ = it->name;

    updateIpVersionsPreferenceAndPolicy();

    if (it->physicalLayerType == PHYSLAYER_TYPE_WIFI) {
        wirelessChannelSStream << it->wirelessChannel;
        networkInterfaceInfo_.ssid_ = it->ssid;
    } else {
        networkInterfaceInfo_.ssid_ = "";
    }

    // The country code and network code strings should only be non-empty if
    // this is a mobile network.
    if (!it->mobileCarrier.empty()) {
        countryCodeSStream << it->mobileCountryCode;
        networkCodeSStream << it->mobileNetworkCode;
    }

    networkInterfaceInfo_.carrier_ = it->mobileCarrier;
    networkInterfaceInfo_.countryCode_ = countryCodeSStream.str();
    networkInterfaceInfo_.ssid_ = it->ssid;
    networkInterfaceInfo_.wirelessChannel_ = wirelessChannelSStream.str();
    networkInterfaceInfo_.networkCode_ = networkCodeSStream.str();
    networkInterfaceInfo_.networkType_ = physicalLayerTypeToString(it->physicalLayerType);
    networkInterfaceInfo_.networkSpec_ = physicalLayerSubtypeToString(it->physicalLayerSubtype);
    networkInterfaceInfo_.additionalInfo_ = it->additionalInfo;

    networkInterfaceInfoIsValid_ = true;
}
