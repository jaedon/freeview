/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DRMMANAGER_H
#define DRMMANAGER_H

#include <vector>
#include <map>
#include <nrdbase/tr1.h>

#include <nrd/ISecureStop.h>
#include <nrd/IDrmSystem.h>
#include <nrd/IDrmSession.h>

namespace netflix{

class DrmManager
{
public:

    ~DrmManager();

    /** Returns the singleton instance of DrmManager */
    static DrmManager* instance();

    /*
     * initializeDrmSystems - initialize underlying drm systems
     * teardownDrmSystem    - tear down underlying drm systems
     */
    NFErr openDrmSystem(const std::string &drmSystemType);
    NFErr closeDrmSystem(const std::string &drmSystemType);

    /**
     * Interfaces to DrmSystemBridge for license handling, secures top, drm
     * storage delete.
     */

    /**
     *
     * Drm system type identification interface
     * PLAY_READY:    return "PlayReady"
     * SESSION_DH1:   return "SessionDH1"
     * WIDEVINE_CEF:  return "WidevineCEF"
     */
    std::string getDrmSystemType();

    /**
     * This is blocking call to get challenge. Called by DrmSystemBridge in NBP
     * thread. After this call, DrmManager create an instance of IDrmSession for
     * specified protectionSystemID.
     *
     * @param[in] contentId
     * @param[in] licenseType
     * @param[in] drmHeader
     * @param[out] challenge data
     * @param[out] DrmSessionId - id of IDrmSession instance
     * @return OK if challenge data is successfully generated.
     */
    NFErr generateChallenge(std::string contentId,
                            enum DrmLicenseType licenseType,
                            const std::vector<unsigned char>& drmHeader,
                            std::vector<unsigned char>& challenge,
                            unsigned int& drmSessionId);

    /**
     * This is blocking call to store license. Called by DrmSystemBridge in NBP
     * thread
     *
     * @param[in] sessionId : id of IDrmSesion instance
     * @param[in] license
     * @param[out] secureStopId
     *
     * @return NFErr_OK if license is stored, and sessionId is retrieved.
     */
    NFErr provideLicense(unsigned int sessionId,
                         const std::vector<unsigned char> license,
                         std::vector<unsigned char>& secureStopId);

    /**
     * This has to be called when there is not player using DrmSession with
     * sessionId
     */
    NFErr deleteDrmSession(unsigned int sessionId);

    /**
     * This has to be called when there is no playback is happening
     */
    NFErr flushDrmSessions();

    /**
     * Secure stop interfaces
     *
     */
    void enableSecureStop(bool);
    bool isSecureStopEnabled();

    NFErr getSecureStopIds(std::vector<std::vector<unsigned char> > &secureStopIds /*out*/);
    NFErr getSecureStop(std::vector<unsigned char> secureStopId,
                        shared_ptr<device::ISecureStop>& secureStop);
    void commitSecureStop(std::vector<unsigned char> secureStopId,
                          std::vector<unsigned char> serverResponse = std::vector<unsigned char>());
    int resetSecureStops();

    /**
     * Drm store / Key store interface
     *
     */
    NFErr deleteDrmStore();
    NFErr deleteKeyStore();
    NFErr getDrmStoreHash(std::vector<unsigned char> &drmStoreHash);
    NFErr getKeyStoreHash(std::vector<unsigned char> &keyStoreHash);

    /**
     * Drm time
     */
    time_t getDrmTime();

    /**
     * Interface to Pumping Thread (for McQueen V2)
     *
     */

    /**
     * called by pumpingThread to see if license is arrived for movieId. When
     * return true, pumpingThread is expected to feed
     */
    bool isLicenseReceived(std::string contentId);

    /**
     * Interface to IElementaryStreamPlayer. From IElementaryStreamPlayer, key
     * is movieId.
     */

    /**
     * getDrmSession
     *
     * Called by device thread. DrmManager will get one inactive decryption
     * session from InactiveDecryptionSession and it will intialize to make an
     * active decryption session, and push an active decryption session into
     * ActiveDecryptionSession queue. Always, drm session with regular license
     * duration get preference over limited duration license.
     *
     * @param[in] contentId
     * @return shared_ptr to IDrmSession instance
     */
    shared_ptr<device::IDrmSession> getDrmSession(std::string contentId);

    /**
     * query system's ldl sessions limit
     *
     */
    NFErr getLdlSessionsLimit(uint32_t& ldlLimit);

    /**
     *
     */
    void getDrmSessionIds(std::map<uint32_t, uint32_t>& sessionIds);

protected:
    DrmManager();
    static DrmManager* sInstance;

private:

    void enqueue(enum device::IDrmSession::SessionState queueType, shared_ptr<device::IDrmSession> session);

    shared_ptr<device::IDrmSession> dequeue(enum device::IDrmSession::SessionState sessionState,
                                            unsigned int sessionId);

    shared_ptr<device::IDrmSession> dequeuePrefereredDrmSession(
        enum device::IDrmSession::SessionState sessionState,
        std::string contentId);

    shared_ptr<device::IDrmSession> dequeueUnprefereredDrmSession(
        std::string contentId,
        enum DrmLicenseType licenseType);

    shared_ptr<device::IDrmSession> peekQueue(enum device::IDrmSession::SessionState sessionState,
                                              unsigned int sessionId);

    shared_ptr<device::IDrmSession> peekQueueByContentId(enum device::IDrmSession::SessionState sessionState,
                                                         std::string contentId);

private:

    /**
     * DrmManager can have multiple underlying implementation of
     * IDrmSystem. This function returns instance of IDrmSystem by drmType
     */
    shared_ptr<device::IDrmSystem> getDrmSystem(std::string drmType);

    /**
     * In this version of SDK, we assume that there is only one IDrmSystem
     * instance in the system
     */
    shared_ptr<device::IDrmSystem> getDrmSystem();

    std::vector< shared_ptr<device::IDrmSession> > mDrmSessionQueue;


    shared_ptr<device::IDrmSystem>  mDrmSystem;

    static Mutex mDrmManagerMutex;

public:
    static const std::string CONTENTID_FOR_SINGLE_SESSION_MODE;

    // UUID type for DRM system IDs.
    typedef uint8_t uuid_t[16];
};

}

#endif
