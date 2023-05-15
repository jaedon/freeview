/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <map>

#include <nrdbase/Base64.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Log.h>

#include "NrdApplication.h"
#include "DrmManager.h"

using namespace std;
using namespace netflix;
using namespace netflix::device;

DrmManager* DrmManager::sInstance = new DrmManager();

Mutex DrmManager::mDrmManagerMutex(DRMMANAGER_MUTEX, "DrmManager");

const std::string DrmManager::CONTENTID_FOR_SINGLE_SESSION_MODE = "contentId_for_single_session_mode";

DrmManager* DrmManager::instance()
{
    return sInstance;
}

DrmManager::DrmManager()
{
    mDrmSessionQueue.clear();
}

DrmManager::~DrmManager()
{

}

/*
 * public
 */
NFErr DrmManager::openDrmSystem(const std::string &drmSystemType)
{
    // TODO - migrate to factory to support multiple drm systems
    NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s", __func__);

    ScopedMutex lock(mDrmManagerMutex);

    NFErr err;

    NRDP_UNUSED(drmSystemType);
    mDrmSystem = nrdApp()->getDrmSystem();
    if(mDrmSystem.get()){
        err = mDrmSystem->initialize();
    } else {
        err = NFErr_NotFound;
    }
    return err;
}

NFErr DrmManager::closeDrmSystem(const std::string &drmSystemType)
{
    ScopedMutex lock(mDrmManagerMutex);

    NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s", __func__);

    NRDP_UNUSED(drmSystemType);

    if(mDrmSystem.get())
    {
        ASSERT(mDrmSystem->getDrmSystemType() == drmSystemType, "DRM system type mismatch: '%s' vs. '%s'",
               mDrmSystem->getDrmSystemType().c_str(), drmSystemType.c_str());

        // delete all drm sessions alive. If not, PlayReady will fail to
        // uninitialize itself when teardown.
        flushDrmSessions();

        NFErr err = mDrmSystem->teardown();
        mDrmSystem.reset();
        return err;
    }
    NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
    return NFErr_Uninitialized;
}

std::string DrmManager::getDrmSystemType()
{
    if(mDrmSystem.get()) {
        return mDrmSystem->getDrmSystemType();
    } else {
        return std::string();
    }
}

NFErr DrmManager::generateChallenge(string contentId,
                                    enum DrmLicenseType licenseType,
                                    const std::vector<unsigned char>& drmHeader,
                                    std::vector<unsigned char>& challenge,
                                    unsigned int& sessionId)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s, contentId:%s, licenseType:%d",
           __func__, contentId.c_str(), licenseType);

    ScopedMutex lock(mDrmManagerMutex);

    // create a DrmSession
    NFErr err;
    shared_ptr<IDrmSystem> drmSystem;
    shared_ptr<IDrmSession> drmSession;
    drmSystem = getDrmSystem();
    if(drmSystem.get()){
        err = drmSystem->createDrmSession(contentId, licenseType, drmHeader, drmSession);
        if(err == NFErr_OK) {
            err = drmSession->getChallengeData(challenge,
                                               (licenseType == LIMITED_DURATION_LICENSE)? true : false);
            if (err == NFErr_OK){
                sessionId = drmSession->getSessionId();
                enqueue(IDrmSession::LicenseAcquisitionState, drmSession);
                return err;
            }
        } else {
            // error handling for session creation failure
            NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed to createDrmSession", __func__);
            return NFErr_DRMFailed;
        }
    } else {
        // error handling for not supported DrmType
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        return NFErr_Uninitialized;
    }
    return err;
}

NFErr DrmManager::provideLicense(unsigned int sessionId,
                                 const std::vector<unsigned char> license,
                                 std::vector<unsigned char>& secureStopId)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s, sessionId %u", __func__, sessionId);

    ScopedMutex lock(mDrmManagerMutex);

    NFErr err;
    shared_ptr<IDrmSession> drmSession;
    drmSession = dequeue(IDrmSession::LicenseAcquisitionState, sessionId);
    if(drmSession.get()){
        // and then, store prefered license
        err = drmSession->storeLicenseData(license, secureStopId);
        if(err == NFErr_OK){
            // when license is stored, put it into InactiveDecryptionQ so that
            // ESPlayer later can get one from this queue for decryption
            enqueue(IDrmSession::InactiveDecryptionState, drmSession);
        } else {
            // we already dequeue drmSession from LicenseAcquisitionState,
            // but since storing license failed, we destroy this session
            shared_ptr<IDrmSystem> drmSystem;
            drmSystem = getDrmSystem();
            drmSystem->destroyDrmSession(drmSession);
            NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed to storeLicenseData", __func__);
        }
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        err = NFErr_Uninitialized;
    }
    return err;
}

NFErr DrmManager::deleteDrmSession(unsigned int sessionId)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s sessionId:%u", __func__, sessionId);

    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSystem> drmSystem;
    shared_ptr<IDrmSession> drmSession;

    drmSystem = getDrmSystem();
    if ( drmSystem.get() ) {
        std::vector< shared_ptr<device::IDrmSession> > :: iterator it = mDrmSessionQueue.begin();
        for(;it!=mDrmSessionQueue.end();){
            if((*it)->getSessionId() == sessionId){
                drmSession = *it;
                mDrmSessionQueue.erase(it);
                drmSystem->destroyDrmSession(drmSession);
                return NFErr_OK;
            } else {
                it++;
            }
        }
        return NFErr_NotFound;
    }

    NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
    return NFErr_Uninitialized;
}

NFErr DrmManager::flushDrmSessions()
{
    ScopedMutex lock(mDrmManagerMutex);

    NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s", __func__);

    shared_ptr<IDrmSystem> drmSystem;
    shared_ptr<IDrmSession> drmSession;

    drmSystem = getDrmSystem();
    if ( drmSystem.get() ){
        std::vector< shared_ptr<device::IDrmSession> > :: iterator it = mDrmSessionQueue.begin();
        while(it!=mDrmSessionQueue.end()){
            drmSession = *it;
            //drmSession->setSessionState(IDrmSession::InvalidState);
            it = mDrmSessionQueue.erase(it);
            drmSystem->destroyDrmSession(drmSession);
        }
        return NFErr_OK;
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        return NFErr_Uninitialized;
    }
}

/**
 * Secure stop intefaces
 *
 *
 */

void DrmManager::enableSecureStop(bool enable)
{
    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()){
        drmSystem->enableSecureStop(enable);
    }
}

bool DrmManager::isSecureStopEnabled()
{
    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()) {
        return drmSystem->isSecureStopEnabled();
    } else {
        // this should not happen because bridge protect against uninitialized
        // case, but if ever comes here, return false by default when not
        // initialized.
        return false;
    }
}

NFErr DrmManager::getSecureStopIds(std::vector<std::vector<unsigned char> > &secureStopIds)
{
    ScopedMutex lock(mDrmManagerMutex);

    NFErr err;
    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()) {
        err = drmSystem->getSecureStopIds(secureStopIds);
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        err = NFErr_Uninitialized;
    }
    return err;
}

NFErr DrmManager::getSecureStop(std::vector<unsigned char> secureStopId,
                                shared_ptr<ISecureStop>& secureStop)
{
    ScopedMutex lock(mDrmManagerMutex);

    NFErr err;
    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()) {
        err = drmSystem->getSecureStop(secureStopId, secureStop);
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        err = NFErr_Uninitialized;
    }
    return err;
}

void DrmManager::commitSecureStop(std::vector<unsigned char> secureStopId,
                                  std::vector<unsigned char> serverResponse)
{
    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()) {
        drmSystem->commitSecureStop(secureStopId, serverResponse);
    }
}

int DrmManager::resetSecureStops()
{
    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()) {
        return drmSystem->resetSecureStops();
    } else {
        // this should not happen because bridge protect against uninitialized
        // case, but if ever comes here, return 0 deleted by default when not
        // initialized.
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        return 0;
    }
}

/**
 * Drm store / Key store delete interfaces
 *
 *
 */
NFErr DrmManager::deleteDrmStore()
{
    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()) {
        return drmSystem->deleteDrmStore();
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        return NFErr_Uninitialized;
    }
}

NFErr DrmManager::deleteKeyStore()
{
    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()) {
        return drmSystem->deleteKeyStore();
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        return NFErr_Uninitialized;
    }
}

NFErr DrmManager::getDrmStoreHash(std::vector<unsigned char> &drmStoreHash)
{
    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()) {
        return drmSystem->getDrmStoreHash(drmStoreHash);
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        return NFErr_Uninitialized;
    }
}

NFErr DrmManager::getKeyStoreHash(std::vector<unsigned char> &keyStoreHash)
{
    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()) {
        return drmSystem->getKeyStoreHash(keyStoreHash);
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        return NFErr_Uninitialized;
    }
}

time_t DrmManager::getDrmTime()
{
    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()) {
        return drmSystem->getDrmTime();
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        return time_t(0);
    }
}

/**
 * License queuey interface for PumpingThread (for McQueen V2
 *
 */
bool DrmManager::isLicenseReceived(std::string contentId)
{
    ScopedMutex lock(mDrmManagerMutex);

    shared_ptr<IDrmSession> drmSession;
    drmSession = peekQueueByContentId(IDrmSession::InactiveDecryptionState, contentId);
    if(drmSession.get()){
        return true;
    }
    drmSession = peekQueueByContentId(IDrmSession::ActiveDecryptionState, contentId);
    if(drmSession.get()){
        return true;
    }
    return false;
}

/**
 * Session access interface for ESPlayer
 *
 *
 */
shared_ptr<IDrmSession> DrmManager::getDrmSession(std::string contentId)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s contentId %s\n", __func__, Base64::encode(contentId).c_str());
    ScopedMutex lock(mDrmManagerMutex);
    shared_ptr<IDrmSession> drmSession;
    drmSession = dequeuePrefereredDrmSession(IDrmSession::InactiveDecryptionState, contentId);
    if(drmSession.get()){
        enqueue(IDrmSession::ActiveDecryptionState, drmSession);
    } else
    {
        drmSession = dequeuePrefereredDrmSession(IDrmSession::ActiveDecryptionState, contentId);
        if(drmSession.get()){
            enqueue(IDrmSession::ActiveDecryptionState, drmSession);
        } else {
            NTRACE(TRACE_DRMSYSTEM, "DrmManager::getDrmSession no DrmSession found for contentId %s", contentId.c_str());
        }
    }
    return drmSession;
}

NFErr DrmManager::getLdlSessionsLimit(uint32_t& ldlLimit)
{
    NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s\n", __func__);
    shared_ptr<IDrmSystem> drmSystem;
    drmSystem = getDrmSystem();
    if(drmSystem.get()){
        return drmSystem->getLdlSessionsLimit(ldlLimit);
    } else {
        NTRACE(TRACE_DRMSYSTEM, "DrmManager::%s failed - no drm system", __func__);
        return NFErr_Uninitialized;
    }
}

/**
 * private methods
 */
void DrmManager::enqueue(enum IDrmSession::SessionState sessionState,
                         shared_ptr<device::IDrmSession> session)
{
    assert(session.get());

    ScopedMutex lock(mDrmManagerMutex);
    session->setSessionState(sessionState);
    mDrmSessionQueue.push_back(session);
    NTRACE(TRACE_DRMSYSTEM,
           "[DrmSessionLifeCycle] %s::IDrmSession instance with sessionId %d enqueued to state %d",
           __func__, session->getSessionId(), sessionState);
}

shared_ptr<IDrmSession> DrmManager::dequeue(enum IDrmSession::SessionState sessionState,
                                            unsigned int sessionId)
{
    ScopedMutex lock(mDrmManagerMutex);
    shared_ptr<IDrmSession> drmSession;
    std::vector< shared_ptr<device::IDrmSession> > :: iterator it = mDrmSessionQueue.begin();
    for(;it!=mDrmSessionQueue.end();){
        if( ((*it)->getSessionState() == sessionState) && (sessionId == (*it)->getSessionId()) ){
            drmSession = *it;
            mDrmSessionQueue.erase(it);
            NTRACE(TRACE_DRMSYSTEM,
                   "[DrmSessionLifeCycle] %s::IDrmSession instance for sessionId %d dequeued from state %d",
                   __func__, sessionId, sessionState);
            break;
        } else {
            it++;
        }
    }
    return drmSession;
}

shared_ptr<device::IDrmSession> DrmManager::dequeuePrefereredDrmSession(
    enum IDrmSession::SessionState sessionState,
    std::string contentId)
{
    NTRACE(TRACE_DRMSYSTEM, "[DrmSessionLifeCycle] %s searching for drmSession : contentId %s sessionState %d",
           __func__, Base64::encode(contentId).c_str(), sessionState);

    ScopedMutex lock(mDrmManagerMutex);
    enum DrmLicenseType licenseType = INVALID_LICENSE;
    shared_ptr<IDrmSession> drmSession;
    std::vector< shared_ptr<device::IDrmSession> > :: iterator it = mDrmSessionQueue.begin();
    for(;it!=mDrmSessionQueue.end();){
        // here find preferred session based on policy
        // policy is simply, longer license duration currently
        NTRACE(TRACE_DRMSYSTEM,"search from : sessionState %d, contentId %s, licenseType %d",
               (*it)->getSessionState(),
               Base64::encode((*it)->getContentId()).c_str(),
               (*it)->getLicenseType());
        if( ((*it)->getSessionState() == sessionState) &&
            ((*it)->getContentId() == contentId) &&
            ((*it)->getLicenseType() > licenseType) ){
            // if DrmSession is in desired state, and if duration is bigger
            drmSession = *it;
            mDrmSessionQueue.erase(it);
            NTRACE(TRACE_DRMSYSTEM, "[DrmSessionLifeCycle] %s::IDrmSession instance for contentId %s found",
                   __func__, Base64::encode(contentId).c_str());
            break;
        } else {
            it++;
        }
    }
    return drmSession;
}

shared_ptr<device::IDrmSession> DrmManager::dequeueUnprefereredDrmSession(
    std::string contentId,
    enum DrmLicenseType licenseType)
{
    ScopedMutex lock(mDrmManagerMutex);
    shared_ptr<IDrmSession> drmSession;
    std::vector< shared_ptr<device::IDrmSession> > :: iterator it = mDrmSessionQueue.begin();
    for(;it!=mDrmSessionQueue.end();){
        // here find preferred session based on policy
        // policy is simply, Regular license > limited duration license
        if( ((*it)->getContentId() == contentId) &&
            ((*it)->getLicenseType() < licenseType) ){
            // if DrmSession is in desired state, and if duration is bigger
            NTRACE(TRACE_DRMSYSTEM
                   , "[DrmSessionLifeCycle] %s::IDrmSession instance for contentId %s of licenseType %d found",
                   __func__, Base64::encode(contentId).c_str(), (*it)->getLicenseType());
            drmSession = *it;
            mDrmSessionQueue.erase(it);
            break;
        } else {
            it++;
        }
    }
    return drmSession;
}

shared_ptr<IDrmSession> DrmManager::peekQueue(
    enum IDrmSession::SessionState sessionState, unsigned int sessionId)
{
    ScopedMutex lock(mDrmManagerMutex);
    shared_ptr<IDrmSession> drmSession;
    std::vector< shared_ptr<IDrmSession> > :: const_iterator it = mDrmSessionQueue.begin();
    for(;it!=mDrmSessionQueue.end();it++){
        if( ((*it)->getSessionState() == sessionState) && (sessionId == (*it)->getSessionId()) ){
            drmSession = *it;
            break;
        }
    }
    return drmSession;
}

shared_ptr<IDrmSession> DrmManager::peekQueueByContentId(
    enum IDrmSession::SessionState sessionState, std::string contentId)
{
    ScopedMutex lock(mDrmManagerMutex);
    shared_ptr<IDrmSession> drmSession;
    std::vector< shared_ptr<IDrmSession> > :: const_iterator it = mDrmSessionQueue.begin();
    for(;it!=mDrmSessionQueue.end();it++){
        //NTRACE(TRACE_DRMSYSTEM, "sessionId %d, contentId %s", (*it)->getSessionId(), (*it)->getContentId());
        if( ((*it)->getSessionState() == sessionState) && (contentId == (*it)->getContentId()) ){
            drmSession = *it;
            break;
        }
    }
    return drmSession;
}

void DrmManager::getDrmSessionIds(std::map<uint32_t, uint32_t>& sessionIds)
{
    ScopedMutex lock(mDrmManagerMutex);
    shared_ptr<IDrmSession> drmSession;
    std::vector< shared_ptr<IDrmSession> > :: const_iterator it = mDrmSessionQueue.begin();
    for(;it!=mDrmSessionQueue.end();it++){
        drmSession = *it;
        NTRACE(TRACE_DRMSYSTEM
               , "DrmManager::%s sessionId %d, sessionState %d"
               , __func__, drmSession->getSessionId(), drmSession->getSessionState());
        sessionIds.insert(std::pair<uint32_t, uint32_t>
                          (drmSession->getSessionId(), static_cast<uint32_t> (drmSession->getSessionState())));
    }
}

shared_ptr<IDrmSystem> DrmManager::getDrmSystem(std::string drmType)
{
    NRDP_UNUSED(drmType);
    return mDrmSystem;
}

shared_ptr<IDrmSystem> DrmManager::getDrmSystem()
{
    return mDrmSystem;
}
