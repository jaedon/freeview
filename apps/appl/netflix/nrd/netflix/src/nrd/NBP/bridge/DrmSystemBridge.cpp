/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DrmSystemBridge.h"

#include <nrdbase/Base64.h>
#include <nrdbase/Log.h>
#include <nrd/Request.h>

#include <nrd/ISecureStop.h>
#include "DrmManager.h"
#include "NrdApplication.h"

using namespace netflix;
using namespace netflix::device;

//////////
//
// properties
//
//////////
#define DrmSystemBridgePropertyList(Property)                           \
    Property(drmType, NfObject::ReadOnly)                               \
    Property(isOpen, NfObject::ReadOnly)                                \
    Property(supportSecureStop, NfObject::ReadOnly)                     \
    Property(supportStorageDeletion, NfObject::ReadOnly)

PropertyTable(DrmSystemBridge, DrmSystemBridgePropertyList, properties)

//////////
//
// methods
//
//////////
#define DrmSystemBridgeMethodList(Method)       \
    Method(closeDrmSystem)                      \
    Method(commitSecureStop)                    \
    Method(deleteDrmSession)                    \
    Method(deleteDrmStore)                      \
    Method(deleteKeyStore)                      \
    Method(enableSecureStop)                    \
    Method(flushDrmSessions)                    \
    Method(generateChallenge)                   \
    Method(getDrmSessionIds)                    \
    Method(getDrmStoreHash)                     \
    Method(getDrmTime)                          \
    Method(getKeyStoreHash)                     \
    Method(getLdlSessionsLimit)                 \
    Method(getSecureStop)                       \
    Method(getSecureStopIds)                    \
    Method(isSecureStopEnabled)                 \
    Method(openDrmSystem)                       \
    Method(provideLicense)                      \
    Method(resetSecureStops)

MethodTable(DrmSystemBridge, DrmSystemBridgeMethodList)

unsigned DrmSystemBridge::requestFlags(const Request &request) const
{
    unsigned ret = Request::NoLogArgs;

    switch (request.method())
    {
    case Methods::DrmSystemBridge::getDrmTime:
    case Methods::DrmSystemBridge::isSecureStopEnabled:
        ret |= Request::Synchronous;
        break;
    default:
        break;
    }
    return ret;
}

//////////
//
// DrmSystemBridge
//
//////////
DrmSystemBridge::DrmSystemBridge()
  : NfObject("drmsystem",
             Properties::DrmSystemBridge::properties, Properties::DrmSystemBridge::PropertyCount,
             Methods::DrmSystemBridge::methods, Methods::DrmSystemBridge::MethodCount),
    m_drmSystemBridgeMutex(DRMSYSTEMBRIDGE_MUTEX, "DrmSystemBridgeMutex")
{
    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::DrmSystemBridge");

    m_drm = DrmManager::instance();
    m_drmSystemOpened = false;
}

DrmSystemBridge::~DrmSystemBridge()
{
    if (m_drmSystemOpened)
        m_drm->closeDrmSystem("PlayReady");
}

//////////
//
// getProperty
//
//////////
bool DrmSystemBridge::getProperty(int index, /*out*/Variant *result) const
{
    /*
     * if platform's drm system is not opened yet, return with conservative
     * values. When platform system is opened, we should update this values from
     * platform.
     */
    const ISystem::Capability c = nrdApp()->getSystem()->getCapability();
    switch (index)
    {
        case Properties::DrmSystemBridge::drmType:
        {
            *result = "PlayReady";
            break;
        }
        case Properties::DrmSystemBridge::isOpen:
        {
            *result = m_drmSystemOpened;
            break;
        }
        case Properties::DrmSystemBridge::supportStorageDeletion:
        {
            *result = c.supportDrmStorageDeletion;
            break;
        }
        case Properties::DrmSystemBridge::supportSecureStop:
        {
            *result = c.supportSecureStop;
            break;
        }
        default:
            return false;
    }

    return true;
}

//////////
//
// invoke
//
//////////
Variant DrmSystemBridge::invoke(int method, const Variant &var)
{
    ScopedMutex lock(m_drmSystemBridgeMutex);

    if ( !m_drm )
        return NfObject::invoke(method, var);

    if ( !m_drmSystemOpened &&
         (method != Methods::DrmSystemBridge::openDrmSystem) &&
         (method != Methods::DrmSystemBridge::closeDrmSystem) &&
         (method != Methods::DrmSystemBridge::deleteDrmStore) &&
         (method != Methods::DrmSystemBridge::getDrmStoreHash) &&
         (method != Methods::DrmSystemBridge::deleteKeyStore) &&
         (method != Methods::DrmSystemBridge::getKeyStoreHash) )
    {
         NFErr err = m_drm->openDrmSystem("PlayReady");
         if(err.ok()){
             m_drmSystemOpened = true;
             propertyUpdated(Properties::DrmSystemBridge::isOpen);
          } else {
             m_drmSystemOpened = false;
             propertyUpdated(Properties::DrmSystemBridge::isOpen);
             return NfObject::invoke(method, var);
         }
    }

    Variant result;

    // make sure we pass back the index of the caller
    const std::string index  = var.mapValue<std::string>("idx");
    result["idx"] = index;
    result["success"] = false; // assume failure for now

    switch (method)
    {
#define FORWARD_METHOD(METHODNAME, ...) \
        case Methods::DrmSystemBridge::METHODNAME: return METHODNAME(result, method, var);

        // Forward all the methods to the same named function
        DrmSystemBridgeMethodList(FORWARD_METHOD)
    }

    // should never get here, but pass to default handler
    return NfObject::invoke(method, var);
}

/*
 * *****************
 *  Helper functions
 *  ****************
 */

//////////
//
// getBase64String
//
//////////
static std::string getBase64String(
    const std::string &name, const Variant &var, bool &ok)
{
    if (!var.contains(name))
    {
        ok = false;
        return std::string();
    }

    ok = true;

    // If the value is a data buffer (a typed array in JS), we convert it
    // to base 64, otherwise, we assume that it is a string already in base64
    std::string str64;
    const Variant &value(var[name]);
    if (value.isDataBuffer())
        str64 = value.dataBuffer().toBase64().toString();
    else
        str64 = value.string();

    return str64;
}

//////////
//
// toB64Vector
//
//////////
static std::vector<unsigned char> toBinVector(const std::string &b64str)
{
    std::string str(Base64::decode(b64str));
    return std::vector<unsigned char>(str.begin(), str.end());
}

//////////
//
// macros
//
//////////

#define READ_VAR(type, name)                                            \
    var.mapValue<type>(name, &ok);                                      \
    do { if (!ok) {                                                     \
            invalidArgumentError(method, name);                         \
            sendEvent("result", result);                                \
            return false;                                               \
        }                                                               \
    } while(0)

#define READ_B64(name)                                                  \
    getBase64String(name, var, ok);                                     \
    do { if (!ok) {                                                     \
            invalidArgumentError(method, name);                         \
            sendEvent("result", result);                                \
            return false;                                               \
        }                                                               \
    } while(0)

#define READ_DATA(varname, name)                                        \
    DataBuffer varname;                                                 \
    do {                                                                \
        const Variant::StringMapIterator it = var.find(name);           \
        if (it == var.stringMapEnd()) {                                 \
            invalidArgumentError(method, name);                         \
            sendEvent("result", result);                                \
            return false;                                               \
        } else if (it->second.type() == Variant::Type_DataBuffer) {     \
            varname = it->second.dataBuffer();                          \
        } else if (it->second.type() == Variant::Type_String) {         \
            varname = DataBuffer::fromBase64(it->second.string());      \
        } else {                                                        \
            invalidArgumentError(method, name);                         \
            sendEvent("result", result);                                \
            return false;                                               \
        }                                                               \
    } while (0)


/*
 * ********************
 * Drm System functions
 * ********************
 */
bool DrmSystemBridge::openDrmSystem(Variant &result, int /*method*/, const Variant& var)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::openDrmSystem");

    if(m_drmSystemOpened){
        result["success"] = true;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::openDrmSystem already opened");
        sendEvent("result", result);
        return true;
    }

    bool ok;
    std::string drmType = var.mapValue<std::string>("drmType", &ok);
    if (!ok){
        drmType = "PlayReady";
    }

    NFErr err = m_drm->openDrmSystem(drmType);
    if( err == NFErr_OK ){
        result["success"] = true;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::openDrmSystem successful");
        m_drmSystemOpened = true;
        propertyUpdated(Properties::DrmSystemBridge::isOpen);
    } else {
        err.push(NFErr_DRMFailed);
        result["success"] = false;
        result["type"] = "error";
        result["error"] = static_cast<int>(err.peekCode());
        result["stack"] = err.toVariant();
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::openDrmSystem failed %s",
               err.toVariant().toJSON().c_str());
    }
    sendEvent("result", result);
    return true;
}

bool DrmSystemBridge::closeDrmSystem(Variant &result, int method, const Variant& var)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::closeDrmSystem");

    /*
     * even when openDrmSystem is failed, closeDrmSystem still should be able to
     * be called because closeDrmSystem cleanup underlying drm
     * system. Therefore, this function should be able to be called at any time
     */
    /*
    if(!m_drmSystemOpened){
        result["success"] = true;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::closeDrmSystem already closed");
        sendEvent("result", result);
        return true;
    }
    */
    bool ok;
    const std::string drmType = READ_VAR(std::string,"drmType");
    NFErr err = m_drm->closeDrmSystem(drmType);
    if( err == NFErr_OK ){
        result["success"] = true;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::closeDrmSystem successful");
    } else {
        err.push(NFErr_DRMFailed);
        result["success"] = false;
        result["type"] = "error";
        result["error"] = static_cast<int>(err.peekCode());
        result["stack"] = err.toVariant();
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::closeDrmSystem failed %s",
               err.toVariant().toJSON().c_str());
    }
    sendEvent("result", result);

    m_drmSystemOpened = false;
    propertyUpdated(Properties::DrmSystemBridge::isOpen);
    return true;
}

std::string DrmSystemBridge::getDrmType(Variant &/*result*/, int /*method*/, const Variant& /*var*/)
{
    return m_drm->getDrmSystemType();
}

/*
 * *****************************
 * secure stop related functions
 * *****************************
 */

//////////
//
// commitSecureStop
//
/////////
bool DrmSystemBridge::commitSecureStop(Variant &result, int method,
                                       const Variant &var)
{
    bool ok;

    // get params
    const std::string secureStopId = READ_B64("secureStopId");
    const std::string serverResponse = READ_B64("serverResponse");

    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::commitSecureStop %s", secureStopId.c_str());

    // commit the secure stop
    m_drm->commitSecureStop(toBinVector(secureStopId), toBinVector(serverResponse));

    result["success"] = true;
    sendEvent("result", result);

    return true;
}

//////////
//
// getSecureStop
//
//////////
bool DrmSystemBridge::getSecureStop(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const std::string secureStopId = READ_B64("secureStopId");

    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::getSecureStop %s", secureStopId.c_str());

    if(secureStopId.empty()){
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::getSecureStop secureStopId is not valid(%s)",
               secureStopId.c_str());
        result["success"] = false;
        sendEvent("result", result);
        return true;
    }

    // get that secure stop
    shared_ptr<ISecureStop> secureStop;
    NFErr err = m_drm->getSecureStop(toBinVector(secureStopId), /*out*/secureStop);
    if (!err.ok())
    {
        Log::warn(TRACE_DRMSYSTEM, "getSecureStop returned %s", err.toString().c_str());
    }
    else
    {
        // return the secure stop data
        std::string stopData((const char *)secureStop->getRawData(),
                             secureStop->getRawDataSize());
        std::string stopData64 = Base64::encode(stopData, true/*urlSafe*/);

        result["securestop"] = stopData64;
        result["success"] = true;
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// getSecureStopIds
//
//////////
bool DrmSystemBridge::getSecureStopIds(Variant &result, int /*method*/,
                                       const Variant &/*var*/)
{
    // get the ids
    std::vector<std::vector<unsigned char> > ids;
    NFErr err = m_drm->getSecureStopIds(/*out*/ids);
    if (err.ok())
    {
        Variant array(Variant::Type_Array);

        for (size_t i=0; i<ids.size(); ++i)
        {
            std::vector<unsigned char> id = Base64::encode(ids[i], true/*urlSafe*/);
            std::string idstr = std::string(id.begin(), id.end());

            Variant vmap;
            vmap["secureStopId"] = idstr;

            array.push_back(vmap);
        }

        result["secureStopIds"] = array;
        result["success"] = true;
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// resetSecureStops
//
//////////
bool DrmSystemBridge::resetSecureStops(Variant &result, int /*method*/,
                                       const Variant &/*var*/)
{
    // reset the secure stops
    int num = m_drm->resetSecureStops();

    // return the number reset
    result["numreset"] = num;
    result["success"] = true;
    sendEvent("result", result);

    return true;
}

//////////
//
// useSecureStop
//
//////////
bool DrmSystemBridge::enableSecureStop(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const bool enable = READ_VAR(bool, "enable");

    // tell drm layer
    m_drm->enableSecureStop(enable);

    // report success
    result["success"] = true;
    sendEvent("result", result);

    return true;
}

bool DrmSystemBridge::isSecureStopEnabled(Variant & /*result*/, int /*method*/, const Variant &/*var*/)
{
    // ask drm layer
    return m_drm->isSecureStopEnabled();
}

/*
 *
 * DRM Key/License store related functions
 *
 */

//////////
//
// deleteDrmStore
//
//////////
bool DrmSystemBridge::deleteDrmStore(Variant &result, int, const Variant &)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::deleteDrmStore");

    netflix::NFErr err = m_drm->deleteDrmStore();
    if (err == NFErr_OK) {
        result["success"] = true;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::deleteDrmStore success");
    } else {
        err.push(NFErr_DRMFailed);
        result["success"] = false;
        result["type"] = "error";
        result["error"] = static_cast<int>(err.peekCode());
        result["stack"] = err.toVariant();
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::deleteDrmStore failed %s",
               err.toVariant().toJSON().c_str());
    }
    sendEvent("result", result);

    return true;
}

//////////
//
// deleteKeyStore
//
//////////
bool DrmSystemBridge::deleteKeyStore(Variant &result, int, const Variant &)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::deleteKeyStore");

    netflix::NFErr err = m_drm->deleteKeyStore();
    if (err == NFErr_OK) {
        result["success"] = true;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::deleteKeyStore success");
    } else {
        err.push(NFErr_DRMFailed);
        result["success"] = false;
        result["type"] = "error";
        result["error"] = static_cast<int>(err.peekCode());
        result["stack"] = err.toVariant();
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::deleteKeyStore failed %s",
               err.toVariant().toJSON().c_str());
    }
    sendEvent("result", result);

    return true;
}

//////////
//
// getDrmStoreHash
//
//////////
bool DrmSystemBridge::getDrmStoreHash(Variant &result, int, const Variant &)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::getDrmSystem");

    std::vector<unsigned char> sha256hash;
    sha256hash.resize(256);
    NFErr err = m_drm->getDrmStoreHash(sha256hash);
    if( err == NFErr_OK ){
        std::vector<unsigned char> hash = Base64::encode(sha256hash);
        std::string hashString = std::string(hash.begin(), hash.end());
        result["success"] = true;
        result["sha256hash"] = hashString.c_str();
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::hash for drm store %s", hashString.c_str());
    } else {
        err.push(NFErr_DRMFailed);
        result["success"] = false;
        result["type"] = "error";
        result["error"] = static_cast<int>(err.peekCode());
        result["stack"] = err.toVariant();
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::hash for drm store failed %s",
               err.toVariant().toJSON().c_str());
    }
    sendEvent("result", result);

    return true;
}

//////////
//
// getKeyStoreHash
//
//////////
bool DrmSystemBridge::getKeyStoreHash(Variant &result, int /*method*/, const Variant &/*var*/)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::getKeyStoreHash");

    std::vector<unsigned char> sha256hash;
    sha256hash.resize(256);
    NFErr err = m_drm->getKeyStoreHash(sha256hash);
    if( err == NFErr_OK ){
        std::vector<unsigned char> hash = Base64::encode(sha256hash);
        std::string hashString = std::string(hash.begin(), hash.end());
        result["success"] = true;
        result["sha256hash"] = hashString.c_str();
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::hash for key store %s", hashString.c_str());
    } else {
        err.push(NFErr_DRMFailed);
        result["success"] = false;
        result["type"] = "error";
        result["error"] = static_cast<int>(err.peekCode());
        result["stack"] = err.toVariant();
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::hash for key store failed %s",
               err.toVariant().toJSON().c_str());
    }
    sendEvent("result", result);

    return true;
}

time_t DrmSystemBridge::getDrmTime(Variant &/*result*/, int /*method*/, const Variant &/*var*/)
{
    time_t currentDrmTime = m_drm->getDrmTime();
    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::getDrmTime %ld, time %ld", (long)currentDrmTime, (long)time(NULL));
    return currentDrmTime;
}

/**
 * License handling related
 *
 */

bool DrmSystemBridge::generateChallenge(Variant &result, int method, const Variant& var)
{
    // get params
    bool ok;
    const std::string drmType = READ_VAR(std::string,"drmType");
    // contentId is stringified 16bytes keyID
    READ_DATA(data, "contentId");
    const std::string contentId = data.toString();
    enum DrmLicenseType licenseType = static_cast<enum DrmLicenseType>(var.mapValue<int>("licenseType", &ok));
    DataBuffer drmHeaderBuffer = READ_VAR(DataBuffer, "drmHeader");
    std::vector<uint8_t> drmHeader = drmHeaderBuffer.toVector();

    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::generateChallenge drmType:%s, contentId:%s, licenseType:%d",
           drmType.c_str(), contentId.c_str(), licenseType);

    // create challenge holder
    std::vector<uint8_t> challenge;
    unsigned int sessionId;

    NFErr err = m_drm->generateChallenge(contentId, licenseType, drmHeader, challenge, sessionId);
    if( err == NFErr_OK ){
        std::string challengeString = std::string(challenge.begin(), challenge.end());
        result["success"] = true;
        result["type"] = "provideChallenge";
        result["drmType"] = drmType;
        result["challenge"] = Base64::encode(challengeString);
        result["sessionId"] = sessionId;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::challenge:%s sessionId(%d), licenseType(%d)",
               challengeString.c_str(), sessionId, licenseType);
    } else {
        err.push(NFErr_DRMFailed);
        result["success"] = false;
        result["type"] = "error";
        result["error"] = static_cast<int>(err.peekCode());
        result["stack"] = err.toVariant();
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::generateChallenge failed %s",
               err.toVariant().toJSON().c_str());
    }
    sendEvent("result", result);
    return true;
}

bool DrmSystemBridge::provideLicense(Variant &result, int method, const Variant& var)
{
    // get params
    bool ok;
    const unsigned int sessionId = READ_VAR(unsigned int, "sessionId");
    const std::string e = READ_VAR(std::string,"license");
    std::string d = Base64::decode(e);
    std::vector<uint8_t> license(d.begin(), d.end());

    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::provideLicense sessionId:%u", sessionId);

    NTRACE(TRACE_DRMSYSTEM, "license base64encoded:::: %s", e.c_str());
    NTRACE(TRACE_DRMSYSTEM, "license base64decoded:::: %s", d.c_str());

    // create secureStopId holder
    std::vector<unsigned char> secureStopId;

    NFErr err = m_drm->provideLicense(sessionId, license, secureStopId);
    if( err == NFErr_OK ){
        std::string secureStopIdString = std::string(secureStopId.begin(), secureStopId.end());
        result["success"] = true;
        result["type"] = "licensesession";
        //result["drmType"] = "PlayReady";
        result["session"] = Base64::encode(secureStopIdString);
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::provideLicense secureStopId %s", Base64::encode(secureStopIdString).c_str());
    } else {
        err.push(NFErr_DRMFailed);
        result["success"] = false;
        result["type"] = "error";
        result["error"] = static_cast<int>(err.peekCode());
        result["stack"] = err.toVariant();
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::provideLicense failed %s",
               err.toVariant().toJSON().c_str());
    }
    sendEvent("result", result);
    return true;
}

bool DrmSystemBridge::deleteDrmSession(Variant &result, int method, const Variant& var)
{
    // get params
    bool ok;
    const unsigned int sessionId = READ_VAR(unsigned int, "sessionId");

    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::%s, sessionId:%u", __func__, sessionId);

    // call DrmManager function
    NFErr err = m_drm->deleteDrmSession(sessionId);
    if( err == NFErr_OK ){
        result["success"] = true;
        result["sessionId"] = sessionId;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::%s for %d successful", __func__, sessionId);
    } else {
        result["success"] = false;
        result["sessionId"] = sessionId;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::%s for %d failed", __func__, sessionId);
    }
    sendEvent("result", result);
    return true;
}

bool DrmSystemBridge::flushDrmSessions(Variant &result, int method, const Variant& var)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::%s", __func__);
    NRDP_UNUSED(method);
    NRDP_UNUSED(var);

    // call DrmManager function
    NFErr err = m_drm->flushDrmSessions();
    if( err == NFErr_OK ){
        result["success"] = true;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::%s successful", __func__);
    } else {
        result["success"] = false;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::%s failed", __func__);
    }
    sendEvent("result", result);
    return true;
}

bool DrmSystemBridge::getLdlSessionsLimit(Variant &result, int method, const Variant& var)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::%s", __func__);
    NRDP_UNUSED(method);
    NRDP_UNUSED(var);

    // call DrmManager function
    uint32_t ldlLimit;
    NFErr err = m_drm->getLdlSessionsLimit(ldlLimit);
    if( err == NFErr_OK ){
        result["success"] = true;
        result["ldlSessionsLimit"] = ldlLimit;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::%s successful", __func__);
    } else {
        result["success"] = false;
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::%s failed", __func__);
    }
    sendEvent("result", result);
    return true;

}


//////////
//
// getSecureStopIds
//
//////////
bool DrmSystemBridge::getDrmSessionIds(Variant &result, int /*method*/,
                                       const Variant &/*var*/)
{
    // get the ids
    std::map<uint32_t, uint32_t> ids;
    m_drm->getDrmSessionIds(/*out*/ids);

    Variant oList;
    for (std::map<uint32_t, uint32_t>::const_iterator cit = ids.begin(); cit!=ids.end(); ++cit)
    {
        Variant vmap;
        vmap["drmSessionId"] = cit->first;
        vmap["drmSessionState"] = cit->second;
        oList.push_back(vmap);
        NTRACE(TRACE_DRMSYSTEM, "DrmSystemBridge::%s [%d] = %d", __func__, cit->first, cit->second);
    }
    result["drmSessionIds"] = oList;
    result["success"] = true;
    sendEvent("result", result);
    return true;
}

#undef FORWARD_METHOD
#undef READ_VAR
