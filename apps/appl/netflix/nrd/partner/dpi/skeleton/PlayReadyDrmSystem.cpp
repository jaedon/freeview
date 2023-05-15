/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <cstring>
#include <string.h>
#include <unistd.h>

#include <nrdbase/Configuration.h>
#include <nrd/AppLog.h>
#include <nrd/IDeviceError.h>

#include "ESPlayerConstants.h"
#include "PlayReadyDrmSystem.h"
#include "PlayReadyDrmSession.h"

using namespace netflix;
using namespace netflix::device;
using namespace netflix::device::esplayer;

extern DRM_CONST_STRING g_dstrDrmPath;
DRM_WCHAR* PlayReadyDrmSystem::drmdir_;
DRM_CONST_STRING PlayReadyDrmSystem::drmStore_;
std::string PlayReadyDrmSystem::drmStoreStr_;
shared_ptr<DRM_APP_CONTEXT> PlayReadyDrmSystem::appContext_;
Mutex PlayReadyDrmSystem::drmAppContextMutex_(DRMSYSTEM_MUTEX, "DrmStore");

namespace {

// The rights we want to request.
// const DRM_WCHAR PLAY[] = { ONE_WCHAR('P', '\0'),
//                            ONE_WCHAR('l', '\0'),
//                            ONE_WCHAR('a', '\0'),
//                            ONE_WCHAR('y', '\0'),
//                            ONE_WCHAR('\0', '\0')
// };
// const DRM_CONST_STRING PLAY_RIGHT = CREATE_DRM_STRING(PLAY);
// const DRM_CONST_STRING* RIGHTS[] = { &PLAY_RIGHT };

/**
 * Creates a new DRM_WCHAR[] on the heap from the provided string.
 *
 * @param[in] s the string.
 * @return the resulting DRM_WCHAR[], NULL terminated.
 */
DRM_WCHAR* createDrmWchar(std::string const& s)
{
    DRM_WCHAR* w = new DRM_WCHAR[s.length() + 1];
    for (size_t i = 0; i < s.length(); ++i)
        w[i] = ONE_WCHAR(s[i], '\0');
    w[s.length()] = ONE_WCHAR('\0', '\0');
    return w;
}
} // namespace anonymous

PlayReadyDrmSystem::PlayReadyDrmSystem()
  :mSessionId(0)
{
    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::%s", __func__);

    appContext_.reset();
    static const std::string dataDirectory = Configuration::dataReadPath();
    static const std::string STORE_FILENAME = "drmstore";
#if defined(PLAYREADY2)
    appContextOpaqueBuffer_ = NULL;
    pbRevocationBuffer_ = NULL;
#endif
    const std::string dir = dataDirectory + "/dpi/playready";
    const std::string store = dataDirectory + "/dpi/playready/storage/" + STORE_FILENAME;

    // Create wchar strings from the arguments.
    drmdir_ = createDrmWchar(dir);

    // Initialize PlayReady directory.
    g_dstrDrmPath.pwszString = drmdir_;
    g_dstrDrmPath.cchString = dir.length();

    // Save the DRM store location for later.
    drmStore_.pwszString = createDrmWchar(store);
    drmStore_.cchString = store.length();
    drmStoreStr_ = store;

#if defined(PLAYREADY2)
    appContextOpaqueBuffer_ = new DRM_BYTE[MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE];
    pbRevocationBuffer_ = new DRM_BYTE[REVOCATION_BUFFER_SIZE];
#else
    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::PlayReadyDrmSystem is created without any DRM library");
#endif
}

PlayReadyDrmSystem::~PlayReadyDrmSystem()
{
#if defined(PLAYREADY)
    ScopedMutex lock(drmAppContextMutex_);
    if(appContext_.get() != NULL)
    {
        Drm_Reader_Commit(appContext_.get(), NULL, NULL);
        Drm_Uninitialize(appContext_.get());
        appContext_.reset();
    }

    delete [] drmdir_; drmdir_ = NULL;
    if (drmStore_.pwszString != NULL)
    {
        delete [] drmStore_.pwszString;
        drmStore_.pwszString = NULL;
        drmStore_.cchString = 0;
    }
#endif
#if defined(PLAYREADY2)
    if (appContextOpaqueBuffer_ != NULL)
    {
        delete [] appContextOpaqueBuffer_;
        appContextOpaqueBuffer_ = NULL;
    }
    if (pbRevocationBuffer_ != NULL)
    {
        delete [] pbRevocationBuffer_;
        pbRevocationBuffer_ = NULL;
    }
#endif
}

NFErr PlayReadyDrmSystem::initialize()
{
    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::%s", __func__);

    ScopedMutex lock(drmAppContextMutex_);

    DRM_RESULT err;

#if defined (PLAYREADY2)
    // DRM Platform Initialization
    err = Drm_Platform_Initialize();
    if(DRM_FAILED(err))
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::%s Error in Drm_Platform_Initialize  0x%08lx",
               __func__, static_cast<unsigned long>(err));
        appContext_.reset();
        std::ostringstream ss;
        ss << "Error in Drm_Platform_Initialize" << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_INITIALIZATION_ERROR, err, errInfo));
    }
#endif

    appContext_.reset(new DRM_APP_CONTEXT);
    memset(appContext_.get(), 0, sizeof(DRM_APP_CONTEXT));
    err  = Drm_Initialize(appContext_.get(), NULL,
#if defined (PLAYREADY2)
        appContextOpaqueBuffer_,
        MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE,
#endif
        &drmStore_);
    if(DRM_FAILED(err)) {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::initDrmContext: Error in Drm_Initialize:"
               " 0x%08lx", static_cast<unsigned long>(err));
        appContext_.reset();
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::initialize Error in Drm_Initialize:"<< std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_INITIALIZATION_ERROR, err, errInfo));
    }

#if defined (PLAYREADY2)
    err = Drm_Revocation_SetBuffer( appContext_.get(), pbRevocationBuffer_, REVOCATION_BUFFER_SIZE);
    if(DRM_FAILED(err))
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::initDrmContext: Error in Drm_Revocation_SetBuffer:"
                   " 0x%08lx", static_cast<unsigned long>(err));
        appContext_.reset();
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::initDrmContext: Error in Drm_Revocation_SetBuffer:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_INITIALIZATION_ERROR, err, errInfo));
    }
#endif

   return NFErr_OK;
}

NFErr PlayReadyDrmSystem::teardown()
{
    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::%s", __func__);

    ScopedMutex lock(drmAppContextMutex_);

    if( !appContext_.get() ){
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::%s - no app context yet", __func__);
        return NFErr_Uninitialized;
    }

    // cleanup drm store for expired or removal date license
    NFErr nferr = NFErr_OK;
    DRM_RESULT err;
    err = Drm_Reader_Commit(appContext_.get(), NULL, NULL);
    if(DRM_FAILED(err)){
        // nothing that we can do about. Just log
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::%s Drm_Reader_Commit 0x%08lx",
               __func__, static_cast<unsigned long>(err));
    }

#if !defined(__CELLOS_LV2__)
    err = Drm_StoreMgmt_CleanupStore(appContext_.get(),
#if defined (PLAYREADY2)
                                     DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES |
                                     DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES,
#endif
                                     NULL, 0, NULL);
    if(DRM_FAILED(err))
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::%s Drm_StoreMgmt_CleanupStore 0x%08lx",
               __func__, static_cast<unsigned long>(err));
        appContext_.reset();
        std::ostringstream ss;
        ss << "Drm_StoreMgmt_CleanupStore" << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        nferr.push(new IDeviceError(DRMSYSTEM_DRMSTORE_ERROR, err, errInfo));
    }
    // Uninitialize drm context
    Drm_Uninitialize(appContext_.get());
    appContext_.reset();
#else
    err = Drm_DeleteSecureStore(&drmStore_);
    if (err != DRM_SUCCESS){
        Log::warn(TRACE_DRMSYSTEM, "Drm_DeleteSecureStore returned 0x%lx", err);
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::teardown: Error in Drm_DeleteSecureStore:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        nferr.push(new IDeviceError(DRMSYSTEM_DRMSTORE_ERROR, err, errInfo));
    }

    err = Drm_DeleteKeyStore();
    if (err != DRM_SUCCESS){
        Log::warn(TRACE_DRMSYSTEM, "Drm_DeleteKeyStore returned 0x%lx", err);
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::deleteKeyStore: Error in Drm_DeleteKeyStore:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        nferr.push(new IDeviceError(DRMSYSTEM_KEYSTORE_ERROR, err, errInfo));
    }

    if(appContext_.get()){
        Drm_Uninitialize(appContext_.get());
        appContext_.reset();
    }
#endif

#if defined (PLAYREADY2)
    // Unitialize platform
    err = Drm_Platform_Uninitialize();
    if(DRM_FAILED(err))
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::%s Error in Drm_Platform_Uninitialize  0x%08lx",
               __func__, static_cast<unsigned long>(err));
        appContext_.reset();
        std::ostringstream ss;
        ss << "Error in Drm_Platform_Uninitialize" << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        nferr.push(new IDeviceError(DRMSYSTEM_INITIALIZATION_ERROR, err, errInfo));
    }
#endif
    return nferr;
}

std::string PlayReadyDrmSystem::getDrmSystemType()
{
    return "PlayReady";
}

/**
 * IDrmSession factory methods
 */
NFErr PlayReadyDrmSystem::createDrmSession(const std::string& contentId,
                                           enum DrmLicenseType licenseType,
                                           const std::vector<uint8_t>& drmHeader,
                                           shared_ptr<IDrmSession>& drmSession)
{
    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::%s", __func__);

    if(mSessionId >= UINT_MAX)
        mSessionId = 0;

    // create a PlayReadyDrmSession
    drmSession.reset(new PlayReadyDrmSession(mSessionId++,
                                             contentId,
                                             licenseType,
                                             drmHeader,
                                             shared_from_this()));

    NTRACE(TRACE_DRMSYSTEM, "[DrmSessionLifeCycle] %s for %d, use_count %ld\n",
           __func__, mSessionId - 1, drmSession.use_count());

    return NFErr_OK;
}

NFErr PlayReadyDrmSystem::destroyDrmSession(shared_ptr<IDrmSession> drmSession)
{
    NTRACE(TRACE_DRMSYSTEM,
           "[DrmSessionLifeCycle] %s for %d, use count %ld\n",
           __func__, drmSession->getSessionId(), drmSession.use_count());

    if(drmSession.get())
    {
        drmSession->finalizeDrmSession();
    }
    drmSession.reset();
    return NFErr_OK;
}

//////////
//
// SecureStop
//
//////////
class DrmSecureStop : public ISecureStop
{
private:
    std::vector<unsigned char> m_sessionID;
    std::vector<unsigned char> m_rawData;

public:
    DrmSecureStop(const std::vector<unsigned char> &sessionID, size_t dataSize)
        : m_sessionID(sessionID)
    {
        m_rawData.resize(dataSize);
    }
    virtual ~DrmSecureStop() {}

    virtual unsigned char *getRawData()
    {
        return &m_rawData[0];
    }
    virtual size_t getRawDataSize()
    {
        return m_rawData.size();
    }
    virtual void getSessionID(std::vector<unsigned char> &sessionID)
    {
        sessionID = m_sessionID;
    }
};

/**
 *
 * SecureStop interfaces
 *
 */
/**
 * Returns all the secure stop ids used by the device
 *
 * @param[out] sessionIds vector of session ids (16 byte vector)
 * @return NFErr_OK if successful;
 *         NFErr_Bad if something went wrong;
 *         NFErr_NotAllowed if secure stop is not supported
 *         (e.g. Janus and PlayReady).
 */
netflix::NFErr PlayReadyDrmSystem::getSecureStopIds(std::vector<std::vector<unsigned char> > &ids)
{
#ifndef PLAYREADY2TEE
    NRDP_UNUSED(ids);
    return NFErr_NotAllowed;
#else
    ScopedMutex lock(drmAppContextMutex_);

    // if secure stop is not supported, return NotAllowed
    DRM_BOOL supported = Drm_SupportSecureStop();
    if (supported == FALSE)
        return NFErr_NotAllowed;

    DRM_BYTE sessionIds[TEE_MAX_NUM_SECURE_STOPS][TEE_SESSION_ID_LEN];
    DRM_DWORD count = 0;
    DRM_RESULT err = Drm_GetSecureStopIds(appContext_.get(), sessionIds, &count);
    if (err != DRM_SUCCESS)
    {
        NTRACE(TRACE_DRMSYSTEM, "Drm_GetSecureStopIds returned 0x%lx", err);
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::getSecureStopIds Error in Drm_GetSecureStopIds:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_SECURESTOP_ERROR, err, errInfo));
    }

    // build up the returned ids
    ids.clear();
    for (unsigned int i=0; i<count; ++i)
    {
        std::vector<unsigned char> id;
        id.reserve(TEE_SESSION_ID_LEN);

        memcpy(&id[0], &sessionIds[i][0], TEE_SESSION_ID_LEN);

        ids.push_back(id);
    }

    return NFErr_OK;
#endif

}

/**
 * Provides the uncommitted secure stop tokens for all destroyed
 * DRM contexts.
 *
 * @param[in] sessionID sessionID of the secure stop to fetch
 * @param[out] current list of secure stops.
 * @return NFErr_OK if successful;
 *         NFErr_Bad if something went wrong;
 *         NFErr_NotAllowed if secure stop is not supported
 *         (e.g. Janus and PlayReady).
 */
netflix::NFErr PlayReadyDrmSystem::getSecureStop(const std::vector<unsigned char> &sessionID,
                                                 shared_ptr<ISecureStop> &secureStop /*out*/)
{
    ScopedMutex lock(drmAppContextMutex_);

#ifndef PLAYREADY2TEE
    NRDP_UNUSED(sessionID);
    NRDP_UNUSED(secureStop);
    return NFErr_NotAllowed;
#else
    // if secure stop is not supported, return NotAllowed
    DRM_BOOL supported = Drm_SupportSecureStop();
    if (supported == FALSE)
        return NFErr_NotAllowed;

    if(!sessionID.size()){
        NTRACE(TRACE_DRMSYSTEM, "Drm_GetSecureStop sessionID length %zu", sessionID.size());
        std::ostringstream ss;
        ss << "Drm_GetSecureStop sessionID length:" << sessionID.size();
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_SECURESTOP_ERROR,
                                      skeletonDeviceSpecific_InvalidDrmSecureStopId,
                                      errInfo));
    }

    // convert our vector to the uuid, sessionID is only supposed to be 16 bytes long
    unsigned char uuid[TEE_SESSION_ID_LEN];
    memcpy(&uuid[0], &sessionID[0], TEE_SESSION_ID_LEN);

    // call once with zero size to determine actual size of secure stop
    DRM_BYTE dummy;
    DRM_WORD rawSize = 0;
    DRM_RESULT err = Drm_GetSecureStop(appContext_.get(), uuid, &dummy, &rawSize);
    if (err != DRM_E_BUFFERTOOSMALL)
    {
        NTRACE(TRACE_DRMSYSTEM, "Drm_GetSecureStop(0) returned 0x%lx", err);
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::getSecureStop Error in Drm_GetSecureStop(0):"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_SECURESTOP_ERROR, err, errInfo));
    }

    // create a new secureStop
    shared_ptr<DrmSecureStop> drmSecureStop(new DrmSecureStop(sessionID, rawSize));

    // now get the secure stop
    err = Drm_GetSecureStop(appContext_.get(), uuid, drmSecureStop->getRawData(),
                            &rawSize);
    if (err != DRM_SUCCESS)
    {
        NTRACE(TRACE_DRMSYSTEM, "Drm_GetSeureStop(1) returned 0x%lx", err);
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::Drm_GetSecureStop(1) returned 0x%lx" << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_SECURESTOP_ERROR, err, errInfo));
    }

    // return the secure stop
    secureStop = drmSecureStop;
    return NFErr_OK;
#endif
}

/**
 * Commits a single secure stop token. The token will no longer be
 * available from getSecureStops().
 *
 * @param[in] sessionID sessionID of the secure stop to commit
 */
void PlayReadyDrmSystem::commitSecureStop(const std::vector<unsigned char> &sessionID,
                                          const std::vector<unsigned char> &serverResponse)
{
    NRDP_UNUSED(serverResponse);

#ifndef PLAYREADY2TEE
    NRDP_UNUSED(sessionID);
#else
    ScopedMutex lock(drmAppContextMutex_);

    // if secure stop is not supported, return
    DRM_BOOL supported = Drm_SupportSecureStop();
    if (supported == FALSE)
        return;

    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::commitSecureStop");

    if(!sessionID.size()){
        NTRACE(TRACE_DRMSYSTEM, "Drm_GetSecureStop sessionID length %zu", sessionID.size());
        return;
    }

    // convert our vector to the uuid, sessionID is only supposed to be 16 bytes long
    unsigned char uuid[TEE_SESSION_ID_LEN];
    memcpy(&uuid[0], &sessionID[0], TEE_SESSION_ID_LEN);

    // commit it
    DRM_RESULT err = Drm_CommitSecureStop(appContext_.get(), uuid);
    if (err != DRM_SUCCESS)
    {
        Log::warn(TRACE_DRMSYSTEM, "Drm_CommitSecureStop returned 0x%lx", err);
    }
#endif
}

/**
 * Resets all persisted secure stops. This won't destroy any active playback session.
 *
 * @return number of secure stops deleted
 */
int PlayReadyDrmSystem::resetSecureStops()
{
#ifndef PLAYREADY2TEE
    return 0;
#else
    ScopedMutex lock(drmAppContextMutex_);

    // if secure stop is not supported, return
    DRM_BOOL supported = Drm_SupportSecureStop();
    if (supported == FALSE)
        return 0;

    DRM_WORD numDeleted = 0;
    DRM_RESULT err = Drm_ResetSecureStops(appContext_.get(), &numDeleted);
    if (err != DRM_SUCCESS)
    {
        Log::warn(TRACE_DRMSYSTEM, "Drm_ResetSecureStops returned 0x%lx", err);
    }
    return numDeleted;
#endif

}

/**
 * Enables/disables secure stop usage on this device
 */
void PlayReadyDrmSystem::enableSecureStop(bool use)
{
    ScopedMutex lock(drmAppContextMutex_);

#ifndef PLAYREADY2TEE
    NRDP_UNUSED(use);
#else
    Drm_TurnSecureStop(static_cast<int>(use));
#endif

}

/**
 * Queries secure stop support on this device
 */
bool PlayReadyDrmSystem::isSecureStopEnabled()
{
    ScopedMutex lock(drmAppContextMutex_);

#ifndef PLAYREADY2TEE
    return false;
#else
    // TODO: to be changed to Drm_IsSecureStopOn();
    return static_cast<bool>(Drm_SupportSecureStop());
#endif
}

/**
 *
 * Drm Key/License storage handling interfaces
 *
 */

/**
 * delete drm store
 * @return NFErr_OK if successful or if drm store is already empty
 *         NFErr_IOError if delete operation failed
 *         NFErr_NotImplemented if not implemented
 */
NFErr PlayReadyDrmSystem::deleteDrmStore()
{
    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::deleteDrmStore");

    ScopedMutex lock(drmAppContextMutex_);

    NFErr nferr;
#ifndef PLAYREADY2_5
    nferr = NFErr_NotImplemented;
#else
    nferr = NFErr_OK;

    DRM_RESULT err = Drm_DeleteSecureStore(&drmStore_);
    if (err != DRM_SUCCESS){
        Log::warn(TRACE_DRMSYSTEM, "Drm_DeleteSecureStore returned 0x%lx", err);
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::deleteDrmStore: Error in Drm_DeleteSecureStore:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        nferr.push(new IDeviceError(DRMSYSTEM_DRMSTORE_ERROR, err, errInfo));
    }
#endif

    return nferr;

}

/**
 * delete key store
 * @return NFErr_OK if successful or if drm store is already empty
 *         NFErr_IOError if delete operation failed
 *         NFErr_NotImplemented if not implemented
 */
NFErr PlayReadyDrmSystem::deleteKeyStore()
{
    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::deleteKeyStore");

    ScopedMutex lock(drmAppContextMutex_);

    NFErr nferr;
#ifndef PLAYREADY2_5
    nferr = NFErr_NotImplemented;
#else
    nferr = NFErr_OK;

    DRM_RESULT err = Drm_DeleteKeyStore();
    if (err != DRM_SUCCESS){
        Log::warn(TRACE_DRMSYSTEM, "Drm_DeleteKeyStore returned 0x%lx", err);
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::deleteKeyStore: Error in Drm_DeleteKeyStore:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        nferr.push(new IDeviceError(DRMSYSTEM_KEYSTORE_ERROR, err, errInfo));
    }
#endif

    return nferr;

}

/**
 * get SHA256 hash of drm store
 * @param[out] drmStoreHash vector of hash SHA256 result
 * @return NFErr_OK if successful
 *         NFErr_IOError if it can not read drm store
 *         NFErr_NotImplemented if not implemented
 */
NFErr PlayReadyDrmSystem::getDrmStoreHash(std::vector<unsigned char> &drmStoreHash)
{
    ScopedMutex lock(drmAppContextMutex_);

    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::getDrmStoreHash");

    NFErr nferr;
#ifndef PLAYREADY2_5
    NRDP_UNUSED(drmStoreHash);
    nferr = NFErr_NotImplemented;
#else
    nferr = NFErr_OK;

    drmStoreHash.resize(256);
    DRM_RESULT err = Drm_GetSecureStoreHash(&drmStore_, static_cast< unsigned char * > (&drmStoreHash[0]));
    if (err != DRM_SUCCESS){
        Log::warn(TRACE_DRMSYSTEM, "Drm_GetSecureStoreHash returned 0x%lx", err);
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::getKeyStoreHash: Error in Drm_GetKeyStoreHash:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        nferr.push(new IDeviceError(DRMSYSTEM_DRMSTORE_ERROR, err, errInfo));
    }

    /*
    std::string hash(drmStoreHash.begin(), drmStoreHash.end());
    NTRACE(TRACE_DRMSYSTEM, "SHA256 hash for key: %s", hash.c_str());
    */
#endif
    return nferr;
}

/**
 * get SHA256 hash of key store
 * @param[out] drmStoreHash vector of hash SHA256 result
 * @return NFErr_OK if successful
 *         NFErr_IOError if it can not read drm store
 *         NFErr_NotImplemented if not implemented
 */
netflix::NFErr PlayReadyDrmSystem::getKeyStoreHash(std::vector<unsigned char> &keyStoreHash)
{
    ScopedMutex lock(drmAppContextMutex_);

    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::getKeyStoreHash");

    NFErr nferr;
#ifndef PLAYREADY2_5
    NRDP_UNUSED(keyStoreHash);
    nferr = NFErr_NotImplemented;
#else
    nferr = NFErr_OK;

    keyStoreHash.resize(256);

    DRM_RESULT err = Drm_GetKeyStoreHash(&keyStoreHash[0]);
    if (err != DRM_SUCCESS){
        Log::warn(TRACE_DRMSYSTEM, "Drm_DeleteKeyStore returned 0x%lx", err);
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::getKeyStoreHash: Error in Drm_GetKeyStoreHash:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        nferr.push(new IDeviceError(DRMSYSTEM_KEYSTORE_ERROR, err, errInfo));
    }

    /*
    std::string hash(keyStoreHash.begin(), keyStoreHash.end());
    NTRACE(TRACE_DRMSYSTEM, "SHA256 hash for key: %s", hash.c_str());
    */
#endif
    return nferr;
}

netflix::NFErr PlayReadyDrmSystem::getLdlSessionsLimit(uint32_t& ldlLimit)
{
    ScopedMutex lock(drmAppContextMutex_);

#if defined(PLAYREADY2_5_TEE_LDL)
    NFErr nferr;
    DRM_RESULT err = Drm_LicenseAcq_GetLdlSessionsLimit_Netflix(appContext_.get(),
                                                                &ldlLimit);
    if (err != DRM_SUCCESS){
        Log::warn(TRACE_DRMSYSTEM, "Drm_LicenseAcq_GetLdlSessionsLimit_Netflix 0x%lx", err);
        std::ostringstream ss;
        ss << "PlayReadyDrmSystem::getLdlSessionsLimit :"
           << "Error in Drm_LicenseAcq_GetLdlSessionsLimit_Netflix:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        nferr.push(new IDeviceError(DRMSYSTEM_LICENSE_ERROR, err, errInfo));
    }
    return nferr;
#else
    NRDP_UNUSED(ldlLimit);
    return NFErr_NotImplemented;
#endif

}

time_t PlayReadyDrmSystem::getDrmTime() const
{
    ScopedMutex lock(drmAppContextMutex_);

#if defined(PLAYREADY2_5_TEE_LDL)
    DRM_UINT64 utctime64;
    DRM_RESULT err = Drm_Clock_GetSystemTime(appContext_.get(), &utctime64);
    if (err != DRM_SUCCESS){
        Log::warn(TRACE_DRMSYSTEM, "Drm_Clock_GetSystemTime error 0x%lx", err);
        // return invalid time
        return (time_t) -1;
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DPI: getDrmTime %ld, time %ld\n", (time_t)utctime64, time(NULL));
        return (time_t)utctime64;
    }
#else
    // need to support broader playready library
    return time(NULL);
#endif
}

/**
 * helper functions
 */

shared_ptr<DRM_APP_CONTEXT> PlayReadyDrmSystem::getDrmAppContext()
{
    return appContext_;
}

Mutex& PlayReadyDrmSystem::getDrmAppContextMutex()
{
    return drmAppContextMutex_;
}
