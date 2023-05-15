/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IDRMSYSTEM_H
#define IDRMSYSTEM_H

#include <vector>

#include <nrdbase/tr1.h>
#include <nrdbase/StdInt.h>
#include <nrdbase/NFErr.h>
#include <nrdase/AseCommonDataTypes.h>
#include <nrd/IDeviceError.h>
#include <nrd/ISecureStop.h>
#include <nrd/IDrmSession.h>

namespace netflix {
namespace device {

/** @file IDrmSystem.h The header file for the DRM System DPI.
 *
 */

class IDrmSystem
{
public:
    virtual ~IDrmSystem() {}

    virtual NFErr initialize() = 0;

    virtual NFErr teardown() = 0;

    /**
     *
     * Identifies the DRM System Type
     *
     */

    virtual std::string getDrmSystemType() = 0;

    /**
     * IDrmSession factory methods.
     */

    /**
     *
     *
     * Creates a new IDrmSession instance and sets a unique sessionId for
     * the IDrmSession
     *
     * @param[in] contentId
     * @param[in]
     * @param[in] drmHeader
     *
     */
    virtual NFErr createDrmSession(const std::string& contentId,
                                   enum DrmLicenseType licenseType,
                                   const std::vector<uint8_t>& drmHeader,
                                   shared_ptr<IDrmSession>& drmSession) = 0;

    virtual NFErr destroyDrmSession(shared_ptr<IDrmSession> drmSession) = 0;

    /**
     *
     * SecureStop interfaces
     *
     */

    /**
     * Returns all the secure stop ids used by the device
     *
     * @param[out] secureStopIds vector of session ids (16 byte vector)
     * @return NFErr_OK if successful;
     *         NFErr_Bad if something went wrong;
     *         NFErr_NotAllowed if secure stop is not supported
     *         (e.g. Janus and PlayReady).
     */
    virtual netflix::NFErr getSecureStopIds(
        /*out*/std::vector<std::vector<unsigned char> > &secureStopIds) = 0;

    /**
     * Provides the uncommitted secure stop tokens for all destroyed
     * DRM contexts.
     *
     * @param[in] secureStopId secureStopId of the secure stop to fetch
     * @param[out] current list of secure stops.
     * @return NFErr_OK if successful;
     *         NFErr_Bad if something went wrong;
     *         NFErr_NotAllowed if secure stop is not supported
     *         (e.g. Janus and PlayReady).
     */
    virtual netflix::NFErr getSecureStop(const std::vector<unsigned char> &secureStopId,
                                         /*out*/shared_ptr<ISecureStop> &secureStop) = 0;

    /**
     * Commits a single secure stop token. The token will no longer be
     * available from getSecureStops().
     *
     * @param[in] secureStopId secureStopId of the secure stop to commit
     */
    virtual void commitSecureStop(const std::vector<unsigned char> &secureStopId,
                                  const std::vector<unsigned char> &serverResponse) = 0;

    /**
     * Resets all persisted secure stops. This won't destroy any active playback session.
     *
     * @return number of secure stops deleted
     */
    virtual int resetSecureStops() = 0;

    /**
     * Enables/disables secure stop usage on this device
     */
    virtual void enableSecureStop(bool) = 0;

    /**
     * Queries secure stop support on this device
     */
    virtual bool isSecureStopEnabled() = 0;

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
    virtual netflix::NFErr deleteDrmStore() = 0;

     /**
     * delete key store
     * @return NFErr_OK if successful or if drm store is already empty
     *         NFErr_IOError if delete operation failed
     *         NFErr_NotImplemented if not implemented
     */
    virtual netflix::NFErr deleteKeyStore() = 0;

    /**
     * get SHA256 hash of drm store
     * @param[out] drmStoreHash vector of hash SHA256 result
     * @return NFErr_OK if successful
     *         NFErr_IOError if it can not read drm store
     *         NFErr_NotImplemented if not implemented
     */
    virtual netflix::NFErr getDrmStoreHash(std::vector<unsigned char> &drmStoreHash) = 0;

    /**
     * get SHA256 hash of key store
     * @param[out] drmStoreHash vector of hash SHA256 result
     * @return NFErr_OK if successful
     *         NFErr_IOError if it can not read drm store
     *         NFErr_NotImplemented if not implemented
     */
    virtual netflix::NFErr getKeyStoreHash(std::vector<unsigned char> &keyStoreHash) = 0;


    /**
     * get maximum LDL session counts supported by underlying drm system
     * @param[out] max number of ldl session that can be created
     * @return NFErr_Ok if successful
     *         NFErr_NotImplemented if not implemented
     *
     */
    virtual netflix::NFErr getLdlSessionsLimit(uint32_t& ldlLimit) = 0;


    /**
     * Return the current time from the DRM subsystem.
     *
     * @return the DRM subsystem time.
     */
    virtual time_t getDrmTime() const = 0;

};

}}

#endif
