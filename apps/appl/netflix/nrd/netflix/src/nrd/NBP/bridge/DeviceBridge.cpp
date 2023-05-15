/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DeviceBridge.h"

#include <nrdbase/StringTokenizer.h>
#include <nrdbase/Variant.h>
#include <nrdbase/Version.h>
#include <nrdbase/Instrumentation.h>

#include <nrd/NrdApplication.h>
#include <nrd/config.h>
#include <nrd/IAudioMixer.h>
#include <nrd/ISystem.h>
#include <nrd/NrdConfiguration.h>
#include <nrd/Request.h>
#include <nrd/Response.h>

#include "SystemData.h"
#include "SecureStore.h"
#include "DiskStore.h"
#include <algorithm>

#include "DrmManager.h"

using namespace std;
using namespace netflix;
using namespace netflix::device;

#define DevicePropertyList(Property)                                    \
    Property(ESN, NfObject::ReadOnly)                                   \
    Property(ESNPrefix, NfObject::ReadOnly)                             \
    Property(SDKVersion, NfObject::ReadOnly)                            \
    Property(UILanguages, NfObject::ReadWrite)                          \
    Property(UIVersion, NfObject::ReadWrite)                            \
    Property(activeVideoOutput, NfObject::ReadOnly)                     \
    Property(availableViewModes, NfObject::ReadOnly)                    \
    Property(capability, NfObject::ReadOnly)                            \
    Property(certificationVersion, NfObject::ReadOnly)                  \
    Property(currentViewMode, NfObject::ReadOnly)                       \
    Property(deviceModel, NfObject::ReadOnly)                           \
    Property(dnslist, NfObject::ReadOnly)                               \
    Property(friendlyName, NfObject::ReadOnly)                          \
    Property(iflist, NfObject::ReadOnly)                                \
    Property(ipversions, NfObject::ReadOnly)                            \
    Property(language, NfObject::ReadOnly)                              \
    Property(maxVideoPipelines, NfObject::ReadOnly)                     \
    Property(mute, NfObject::ReadOnly)                                  \
    Property(registered, NfObject::ReadWrite)                           \
    Property(screensaverOn, NfObject::ReadOnly)                         \
    Property(softwareVersion, NfObject::ReadOnly)                       \
    Property(startupTags, NfObject::ReadOnly)                           \
    Property(supportedVideoOutput, NfObject::ReadOnly)                  \
    Property(videoOutput, NfObject::ReadOnly)                           \
    Property(videoPipelineCapabilities, NfObject::ReadOnly)             \
    Property(volumeControlType, NfObject::ReadOnly)                     \
    Property(volumeStep, NfObject::ReadOnly)

PropertyTable(DeviceBridge, DevicePropertyList, properties)

#define MethodList(Method)                      \
    Method(disableDolbyVisionELComposing)       \
    Method(factoryReset)                        \
    Method(getDisplaySize)                      \
    Method(getRemainingVideoPipelineCapability) \
    Method(getSignatures)                       \
    Method(getSystemValue)                      \
    Method(getUptime)                           \
    Method(getVolume)                           \
    Method(setViewMode)                         \
    Method(setVolume)

MethodTable(DeviceBridge, MethodList)

#define READ_VAR(type, name)                    \
    var.mapValue<type>(name, &ok);              \
    do {                                        \
        if (!ok) {                              \
            invalidArgumentError(method, name); \
            sendEvent("result", result);        \
            return false;                       \
        }                                       \
    } while (0)

DeviceBridge::DeviceBridge()
    : NfObject("device",
               Properties::DeviceBridge::properties, Properties::DeviceBridge::PropertyCount,
               Methods::DeviceBridge::methods, Methods::DeviceBridge::MethodCount)
{
    mStartupTags = nrdApp()->getSystem()->getStartupLogTags();
}

DeviceBridge::~DeviceBridge()
{
}

void DeviceBridge::init()
{
    mEventDispatcher.reset(new EventDispatcher(static_pointer_cast<DeviceBridge>(shared_from_this())));
    nrdApp()->addSystemListener(mEventDispatcher);
}

bool DeviceBridge::getProperty(int index, Variant *result) const
{
    switch (index) {
    case Properties::DeviceBridge::videoOutput: {
        uint32_t width = 1920, height = 1080;
        nrdApp()->getSystem()->getVideoOutputResolution(width, height);
        *result = Variant();
        (*result)["width"] = width;
        (*result)["height"] = height;
        break;
    }
    case Properties::DeviceBridge::activeVideoOutput: {
        std::vector<struct ISystem::VideoOutputState> supportedOutput = nrdApp()->getSystem()->getActiveVideoOutput();
        *result = Variant();
        Variant oList;
        for (std::vector<struct ISystem::VideoOutputState>::iterator iter = supportedOutput.begin(); iter != supportedOutput.end(); iter++) {
            ISystem::VideoOutputState oInfo = *iter;
            Variant oInfoVar;
            oInfoVar["videoOutput"] = oInfo.videoOutput;
            oInfoVar["hdcpVersion"] = oInfo.hdcpVersion;
            oList.push_back(oInfoVar);
        }
        (*result) = oList;
        break;
    }
    case Properties::DeviceBridge::supportedVideoOutput: {
        std::vector<struct ISystem::VideoOutputInfo> supportedOutput = nrdApp()->getSystem()->getSupportedVideoOutput();
        *result = Variant();
        Variant oList;
        for (std::vector<struct ISystem::VideoOutputInfo>::iterator iter = supportedOutput.begin(); iter != supportedOutput.end(); iter++) {
            ISystem::VideoOutputInfo oInfo = *iter;
            Variant oInfoVar;
            oInfoVar["name"] = oInfo.name;
            oInfoVar["videoOutput"] = oInfo.videoOutput;
            oInfoVar["hdcpVersion"] = oInfo.hdcpVersion;
            oList.push_back(oInfoVar);
        }
        (*result) = oList;
        break;
    }
    case Properties::DeviceBridge::capability: {
        const ISystem::Capability c = nrdApp()->getSystem()->getCapability();
        *result = Variant();
        (*result)["hasPointer"] = c.hasPointer;
        (*result)["hasTouchSupport"] = c.hasTouchSupport;
        (*result)["hasKeyboard"] = c.hasKeyboard;
        (*result)["hasSuspend"] = c.hasSuspend;
        (*result)["hasBackground"] = c.hasBackground;
        (*result)["hasOnScreenKeyboard"] = c.hasOnScreenKeyboard;
        (*result)["hasScreensaver"] = c.hasScreensaver;
        (*result)["supportFrameRateSeamlessSwitch"] = c.supportFrameRateSeamlessSwitch;
        (*result)["support2DVideoResize"] = c.support2DVideoResize;
        (*result)["support2DVideoResizeAnimation"] = c.support2DVideoResizeAnimation;
        (*result)["supportDrmPlaybackTransition"] = c.supportDrmPlaybackTransition;
        (*result)["supportDrmToDrmTransition"] = c.supportDrmToDrmTransition;
        (*result)["supportMultiSessionDrm"] = true; // SDK after 15.1 will have this true always.
        (*result)["supportMultipleDrmDecryption"] = c.supportMultipleDrmDecryption;
        (*result)["supportLimitedDurationLicenses"] = c.supportLimitedDurationLicenses;
        (*result)["supportDrmStorageDeletion"] = c.supportDrmStorageDeletion;
        (*result)["supportSecureStop"] = c.supportSecureStop;
        (*result)["supportUiSound"] = c.supportUiSound && nrdApp()->getDeviceLib()->getAudioMixer();
        (*result)["supportOnTheFlyAudioSwitch"] = c.supportOnTheFlyAudioSwitch;
        (*result)["supportOnTheFlyAudioCodecSwitch"] = c.supportOnTheFlyAudioCodecSwitch;
        (*result)["supportDolbyVisionComposerControl"] = c.supportDolbyVisionComposerControl;
        (*result)["supportDynamicVideoPipelines"] = c.supportDynamicVideoPipelines;
        (*result)["tcpReceiveBufferSize"] = (int)c.tcpReceiveBufferSize;
        (*result)["videoBufferPoolSize"] = (int)c.videoBufferPoolSize;
        (*result)["audioBufferPoolSize"] = (int)c.audioBufferPoolSize;
        (*result)["supportPlayerAudioTransition"] = ISystem::transitionProfileToString(c.supportPlayerAudioTransition);
        (*result)["displayColorDepth"] = (int)c.displayColorDepth;

        {
            Variant stringList;
            vector<ContentProfile> videoProfiles = nrdApp()->getSystem()->getCapability().videoProfiles;
            for (vector<ContentProfile>::iterator iter = videoProfiles.begin(); iter != videoProfiles.end(); iter++) {
                ContentProfile profile = *iter;
                std::string str = profileToString(profile);
                if (!str.empty())
                    stringList.push_back(str);
            }
            (*result)["videoProfiles"] = stringList;
        }
        {
            Variant stringList;
            vector<ContentProfile> audioProfiles = nrdApp()->getSystem()->getCapability().audioProfiles;
            for (vector<ContentProfile>::iterator iter = audioProfiles.begin(); iter != audioProfiles.end(); iter++) {
                ContentProfile profile = *iter;
                const std::string str = profileToString(profile);
                if (!str.empty())
                    stringList.push_back(str);
            }
            (*result)["audioProfiles"] = stringList;
        }
        {
            Variant stringList;
            vector<ContentProfile> audioProfiles = nrdApp()->getSystem()->getCapability().audioDecodeProfiles;
            for (vector<ContentProfile>::iterator iter = audioProfiles.begin(); iter != audioProfiles.end(); iter++) {
                ContentProfile profile = *iter;
                const std::string str = profileToString(profile);
                if (!str.empty())
                    stringList.push_back(str);
            }
            (*result)["audioDecodeProfiles"] = stringList;
        }
        {
            Variant stringList;
            vector<ISystem::EaseProfile> audioEaseProfiles = nrdApp()->getSystem()->getCapability().audioEaseProfiles;
            for (vector<ISystem::EaseProfile>::iterator iter = audioEaseProfiles.begin(); iter != audioEaseProfiles.end(); iter++) {
                ISystem::EaseProfile profile = *iter;
                const std::string str = ISystem::easeProfileToString(profile);
                if (!str.empty())
                    stringList.push_back(str);
            }
            (*result)["audioEaseProfiles"] = stringList;
        }

        std::string drmString;

        // drmString = DrmManager::instance()->getDrmSystemType();
        // For NRDP 4.2 native streaming engine, we only support PlayReady.
        drmString = "PlayReady";

        (*result)["drmType"] = stringToDrmType(drmString);
        break;
    }
    case Properties::DeviceBridge::UIVersion:
        *result = nrdApp()->readSystemConfiguration(SystemKeys::UIVersion);
        break;
    case Properties::DeviceBridge::UILanguages:
        *result = nrdApp()->systemData()->languagePreferences();
        break;
    case Properties::DeviceBridge::ESN:
        *result = nrdApp()->getSystem()->getEsn();
        break;
    case Properties::DeviceBridge::language:
        *result = nrdApp()->getSystem()->getLanguage();
        break;
    case Properties::DeviceBridge::ESNPrefix:
        *result = nrdApp()->esnPrefix();
        break;
    case Properties::DeviceBridge::SDKVersion:
        *result = nrdApp()->versionInfo();
        break;
    case Properties::DeviceBridge::softwareVersion:
        *result = nrdApp()->getSystem()->getSoftwareVersion();
        break;
    case Properties::DeviceBridge::friendlyName:
        *result = nrdApp()->getSystem()->getFriendlyName();
        break;
    case Properties::DeviceBridge::certificationVersion:
        *result = nrdApp()->getSystem()->getCertificationVersion();
        break;
    case Properties::DeviceBridge::deviceModel: {
        string model;
        nrdApp()->getSystem()->getDeviceModel(model);
        *result = model;
        break;
    }
    case Properties::DeviceBridge::screensaverOn:
        *result = nrdApp()->getSystem()->isScreensaverOn();
        break;
    case Properties::DeviceBridge::currentViewMode:
        *result = viewModeTypeToString(nrdApp()->getSystem()->getCurrentViewMode());
        break;
    case Properties::DeviceBridge::availableViewModes: {
        *result = Variant();
        std::vector<ViewModeType> viewModes = nrdApp()->getSystem()->getAvailableViewModes();
        for (vector<ViewModeType>::iterator iter = viewModes.begin(); iter != viewModes.end(); iter++) {
            ViewModeType viewMode = *iter;
            std::string str = viewModeTypeToString(viewMode);
            if (!str.empty())
                (*result).push_back(str);
        }
        break;
    }
    case Properties::DeviceBridge::dnslist:
        *result = nrdApp()->getSystem()->getDNSList();
        break;
    case Properties::DeviceBridge::iflist: {
        *result = Variant();
        std::vector< struct ISystem::NetworkInterface > ifaceList = nrdApp()->getSystem()->getNetworkInterfaces();
        result->resizeArray(ifaceList.size());
        int i = 0;
        vector<struct ISystem::NetworkInterface>::const_iterator it = (&ifaceList)->begin();
        const vector<struct ISystem::NetworkInterface>::const_iterator end = (&ifaceList)->end();
        for (; it != end; ++it) {
            Variant map;
            map["ifname"] = (*it).name;
            map["physicalLayerType"] = physicalLayerTypeToString((*it).physicalLayerType);
            map["physicalLayerSubtype"] = physicalLayerSubtypeToString((*it).physicalLayerSubtype);
            map["ipAddress"] = (*it).ipAddress;
            map["ipv6Addresses"] = (*it).ipv6Addresses;
            map["macAddress"] = (*it).macAddress;
            map["ssid"] = (*it).ssid;
            map["wirelessChannel"] = (*it).wirelessChannel;
            map["mobileCarrier"] = (*it).mobileCarrier;
            map["mobileCountryCode"] = (*it).mobileCountryCode;
            map["mobileNetworkCode"] = (*it).mobileNetworkCode;
            map["isDefault"] = (*it).isDefault;
            map["linkConnected"] = (*it).linkConnected;
            map["internetConnected"] = (*it).internetConnected;
            map["additionalInfo"] = (*it).additionalInfo;
            (*result)[i++] = map;
        }
        break;
    }
    case Properties::DeviceBridge::ipversions: {
        *result = Variant();
        NetworkInterfaceInfo interfaceInfo = nrdApp()->systemData()->getNetworkInterfaceInfo();
        (*result)["policy"] = interfaceInfo.ipVersionPolicy_;
        (*result)["available"] = interfaceInfo.ipVersionsAvailable_;
        (*result)["preferred"] = interfaceInfo.ipVersionPreferred_;
        break;
    }
    case Properties::DeviceBridge::volumeControlType:
        *result = nrdApp()->getSystem()->getVolumeControlType();
        break;
    case Properties::DeviceBridge::volumeStep:
        *result = nrdApp()->getSystem()->getCurrentMinimumVolumeStep();
        break;
    case Properties::DeviceBridge::mute:
        *result = nrdApp()->getSystem()->isMuted();
        break;
    case Properties::DeviceBridge::startupTags:
        *result = mStartupTags;
        break;
    case Properties::DeviceBridge::registered: {
        shared_ptr<SecureStore> sstore = nrdApp()->secureStore();
        if (sstore->has(SystemKeys::registered)) {
            *result = sstore->get<bool>(SystemKeys::registered);
        } else {
            *result = Variant();
        }
        break;
    }
    case Properties::DeviceBridge::maxVideoPipelines:
    {
        *result = nrdApp()->getSystem()->maxVideoPipelines();
        break;
    }
    case Properties::DeviceBridge::videoPipelineCapabilities: {
        vector<struct ISystem::VideoPipelineCapability> videoPipelineCapabilities
            = nrdApp()->getSystem()->getVideoPipelineCapabilities();
        *result = Variant();
        Variant oList;
        for(vector<struct ISystem::VideoPipelineCapability>::iterator iter = videoPipelineCapabilities.begin();
            iter != videoPipelineCapabilities.end();
            iter++) {
            ISystem::VideoPipelineCapability oInfo = *iter;
            Variant oInfoVar;
            oInfoVar["pipelineId"] = oInfo.pipelineId;
            oInfoVar["maxHEVCCodecProfile"] = oInfo.maxHEVCCodecProfile;
            oInfoVar["maxAVCCodecProfile"] = oInfo.maxAVCCodecProfile;
            oInfoVar["isDrmCapable"] = oInfo.isDrmCapable;
            oInfoVar["isFullPictureQuality"] = oInfo.isFullPictureQuality;
            oInfoVar["zOrder"] = oInfo.zOrder;
            oInfoVar["isFullFeaturedPipeline"] = oInfo.isFullFeaturedPipeline;
            oList.push_back(oInfoVar);
        }
        (*result) = oList;
        break;
    }
    default:
        return false;
    }
    return true;
}

Variant DeviceBridge::invoke(int method, const Variant &data)
{
    const std::string index  = data.mapValue<std::string>("idx");

    switch (method) {
    case Methods::DeviceBridge::disableDolbyVisionELComposing:
    {
        const bool disable = data.mapValue<bool>("disable");
        nrdApp()->getSystem()->disableDolbyVisionELComposing(disable);
        Variant map;
        map["idx"] = index;
        sendEvent("disableDolbyVisionELComposing", map);
        break;
    }
    case Methods::DeviceBridge::factoryReset: {
        const int properties[] = {
            Properties::DeviceBridge::UILanguages,
            Properties::DeviceBridge::UIVersion,
            Properties::DeviceBridge::registered
        };
        nrdApp()->sendEvent(ApplicationEvent::Event_FactoryReset);
        propertiesUpdated(properties, sizeof(properties) / sizeof(properties[0]));
        nrdApp()->sendResponse(Response::createEvent("factoryReset", shared_ptr<NfObject>(), Variant()));
        Variant map;
        map["idx"] = index;
        sendEvent("factoryReset", map);
        break; }
    case Methods::DeviceBridge::getDisplaySize: {
        uint32_t width, height;
        nrdApp()->getSystem()->getDisplaySize(width, height);
        Variant displaySize;
        displaySize["width"] = width;
        displaySize["height"] = height;
        Variant map;
        map["idx"] = index;
        map["data"] = displaySize;
        sendEvent("getDisplaySize", map);
        break; }
    case Methods::DeviceBridge::getSystemValue: {
        Variant map;
        const std::string key = data.mapValue<std::string>("key");
        map["idx"] = index;
        map["data"] = nrdApp()->getSystem()->getSystemValue(key);
        map["key"] = key;
        sendEvent("getSystemValue", map);
        break; }
    case Methods::DeviceBridge::getSignatures: {
        Variant map;
        map["idx"] = index;
        map["data"] = nrdApp()->getSystem()->getSignatures();
        sendEvent("getSignatures", map);
        break; }
    case Methods::DeviceBridge::getUptime: {
        uint32_t uptime, ontime;
        nrdApp()->getSystem()->getUptime(uptime, ontime);
        Variant uptimeVals;
        uptimeVals["uptime"] = uptime;
        uptimeVals["ontime"] = ontime;
        Variant map;
        map["idx"] = index;
        map["data"] = uptimeVals;
        sendEvent("getUptime", map);
        break; }
    case Methods::DeviceBridge::setViewMode: {
        const std::string viewModeStr = data.mapValue<std::string>("viewMode");
        ViewModeType viewMode = viewModeTypeFromString(viewModeStr);
        nrdApp()->getSystem()->setViewMode(viewMode);
        break; }
    case Methods::DeviceBridge::getVolume: {
        Variant map;
        map["idx"] = index;
        map["data"] = nrdApp()->getSystem()->getVolume();
        sendEvent("getVolume", map);
        break; }
    case Methods::DeviceBridge::setVolume:
        {
            if (nrdApp()->getSystem()->getVolumeControlType() != NO_VOLUME_CONTROL) {

                // check volume validity
                bool ok;
                double targetVolume = data.mapValue<double>("targetVolume", &ok);
                if (ok) {
                    if (targetVolume < 0.0) {
                        NTRACE(TRACE_NBP, "DeviceBridge::invoke(): warning, setVolume: targetVolume < 0, set to 0");
                        targetVolume = 0.0;
                    } else if (targetVolume > 1.0) {
                        NTRACE(TRACE_NBP, "DeviceBridge::invoke(): warning, setVolume: targetVolume > 1.0, set to 1.0");
                        targetVolume = 1.0;
                    }
                } else {
                    NTRACE(TRACE_NBP, "DeviceBridge::invoke(): warning, setVolume: targetVolume unspecified, set to 1.0");
                    targetVolume = 1.0;
                }

                nrdApp()->getSystem()->setVolume(targetVolume);
            }
            break;
        }
    case Methods::DeviceBridge::getRemainingVideoPipelineCapability: {
        NFErr err;
        std::vector<ISystem::VideoPipelineCapability> videoPipelineCapabilities;
        ISystem::VideoPipelineCapability remainingVideoPipelineCapability;

        /*
         * NRDJS can create array of VideoPipelineCapability
         *
         var videoPipelineCapability1 = {maxHEVCCodecProfile:9, maxAVCCodecProfile:2};
         var videoPipelineCapabilities = [videoPipelineCapability1];
         var cb = function (evt){nrdp.log.error(evt);};
         nrdp.device.getRemainingVideoPipelineCapability(videoPipelineCapabilities, cb);
        */
        bool ok;
        Variant result;
        Variant pipelines = data["videoPipelineCapabilities"];
        Variant::ArrayIterator it;
        for (it = pipelines.arrayBegin(); it != pipelines.arrayEnd(); ++it)
        {
            const Variant& var = *it;
            ISystem::VideoPipelineCapability currVideoPipelineCapability;
            currVideoPipelineCapability.pipelineId = READ_VAR(uint32_t, "pipelineId");
            currVideoPipelineCapability.maxHEVCCodecProfile
                = (enum ISystem::CodecProfile) READ_VAR(uint32_t, "maxHEVCCodecProfile");
            currVideoPipelineCapability.maxAVCCodecProfile
                = (enum ISystem::CodecProfile) READ_VAR(uint32_t, "maxAVCCodecProfile");
            currVideoPipelineCapability.isDrmCapable = READ_VAR(bool, "isDrmCapable");
            currVideoPipelineCapability.isFullPictureQuality = READ_VAR(bool, "isFullPictureQuality");
            currVideoPipelineCapability.zOrder = (enum ISystem::ZOrderCapability) READ_VAR(uint32_t, "zOrder");
            currVideoPipelineCapability.isFullFeaturedPipeline =  READ_VAR(bool, "isFullFeaturedPipeline");
            videoPipelineCapabilities.push_back(currVideoPipelineCapability);
        }

        err = nrdApp()->getSystem()->getRemainingVideoPipelineCapability(videoPipelineCapabilities,
                                                                         remainingVideoPipelineCapability);
        if(err.ok()){
            result["pipelineId"] = remainingVideoPipelineCapability.pipelineId;
            result["maxHEVCCodecProfile"] = remainingVideoPipelineCapability.maxHEVCCodecProfile;
            result["maxAVCCodecProfile"] = remainingVideoPipelineCapability.maxAVCCodecProfile;
            result["isDrmCapable"] = remainingVideoPipelineCapability.isDrmCapable;
            result["isFullPictureQuality"] = remainingVideoPipelineCapability.isFullPictureQuality;
            result["zOrder"] = remainingVideoPipelineCapability.zOrder;
            result["isFullFeaturedPipeline"] = remainingVideoPipelineCapability.isFullFeaturedPipeline;
        } else {
            result["type"] = "error";
            result["error"] = static_cast<int>(err.peekCode());
            result["stack"] = err.toVariant();
        }

        Variant map;
        map["idx"] = index;
        map["data"] = result;
        sendEvent("remainingVideoPipelineCapability", map);
    }
    default:
        return false;
    }
    return true;
}

bool DeviceBridge::setProperty(int index, const Variant &value)
{
    bool ok;
    std::string str;
    switch (index) {
    case Properties::DeviceBridge::UIVersion:
        str = value.value<std::string>(&ok);
        if (ok) {
            nrdApp()->writeSystemConfiguration(SystemKeys::UIVersion, str);
            propertyUpdated(Properties::DeviceBridge::UIVersion);
            INSTRUMENTATION_EVENT_SWITCHED("nrdp.device",Variant::StringMap("uiversion",str));
        }
        break;
    case Properties::DeviceBridge::UILanguages:
        str = value.value<std::string>(&ok);
        if (ok) {
            std::vector<std::string> languages;
            StringTokenizer::tokenize(str, languages, ",");
            nrdApp()->systemData()->setLanguagePreferences(languages);
            propertyUpdated(Properties::DeviceBridge::UILanguages);
        }
        break;
    case Properties::DeviceBridge::mute:
        if (nrdApp()->getSystem()->getVolumeControlType() != NO_VOLUME_CONTROL) {
            bool mute = value.value<bool>(&ok);
            if (ok) {
                nrdApp()->getSystem()->setMute(mute);
            }
        }
    case Properties::DeviceBridge::registered:
        nrdApp()->getSystem()->setRegistered(value.value<bool>());
        nrdApp()->writeSystemConfiguration(SystemKeys::registered,
                                           value.value<std::string>());
        propertyUpdated(Properties::DeviceBridge::registered);
        break;
    }

    return true;
}

void DeviceBridge::EventDispatcher::capabilityChanged()
{
    if (shared_ptr<DeviceBridge> bridge = mBridge.lock()){
        const int properties[] = {
            Properties::DeviceBridge::capability,
            Properties::DeviceBridge::videoPipelineCapabilities
        };

        bridge->propertiesUpdated(properties, sizeof(properties) / sizeof(properties[0]));
    }
}

void DeviceBridge::EventDispatcher::videoOutputResolutionChanged()
{
    if (shared_ptr<DeviceBridge> bridge = mBridge.lock())
        bridge->propertyUpdated(Properties::DeviceBridge::videoOutput);
}

void DeviceBridge::EventDispatcher::screensaverStateChanged()
{
    if (shared_ptr<DeviceBridge> bridge = mBridge.lock())
        bridge->propertyUpdated(Properties::DeviceBridge::screensaverOn);
}

void DeviceBridge::EventDispatcher::languageChanged()
{
    if (shared_ptr<DeviceBridge> bridge = mBridge.lock())
        bridge->propertyUpdated(Properties::DeviceBridge::language);
}

void DeviceBridge::EventDispatcher::networkChanged()
{
    if (shared_ptr<DeviceBridge> bridge = mBridge.lock())
        bridge->updateNetworkProperty();
}

void DeviceBridge::EventDispatcher::commandReceived(const std::string &command)
{
    if (shared_ptr<DeviceBridge> bridge = mBridge.lock())
        bridge->parent()->sendEvent("commandReceived", command);
}

void DeviceBridge::EventDispatcher::viewModeChanged()
{
    if (shared_ptr<DeviceBridge> bridge = mBridge.lock())
        bridge->propertyUpdated(Properties::DeviceBridge::currentViewMode);
}

void DeviceBridge::EventDispatcher::outputVolumeChanged()
{
    if (shared_ptr<DeviceBridge> bridge = mBridge.lock()) {
        const int volumeProperties[] = {
            Properties::DeviceBridge::volumeStep, Properties::DeviceBridge::mute
        };

        bridge->propertiesUpdated(volumeProperties, sizeof(volumeProperties) / sizeof(volumeProperties[0]));
    }
}

void DeviceBridge::EventDispatcher::videoOutputStatusChanged()
{
    if (shared_ptr<DeviceBridge> bridge = mBridge.lock()){
        // supportedVideoOutput actually should be static values. However, it is
        // added to be updated for QA purpose
        bridge->propertyUpdated(Properties::DeviceBridge::supportedVideoOutput);
        bridge->propertyUpdated(Properties::DeviceBridge::activeVideoOutput);
    }
}

void DeviceBridge::updateSDKVersion()
{
    propertyUpdated(Properties::DeviceBridge::SDKVersion);
}

void DeviceBridge::updateNetworkProperty()
{
    const int netProperties[] = {
        Properties::DeviceBridge::dnslist,
        Properties::DeviceBridge::iflist,
        Properties::DeviceBridge::ipversions
    };
    propertiesUpdated(netProperties, sizeof(netProperties) / sizeof(netProperties[0]));
}

