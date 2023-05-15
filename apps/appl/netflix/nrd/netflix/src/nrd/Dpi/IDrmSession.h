/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IDRMSESSION_H
#define IDRMSESSION_H

#include <nrdbase/NFErr.h>
#include <nrdbase/tr1.h>
#include <nrdbase/StdInt.h>
#include <vector>
#include "IDeviceError.h"
#include <nrdase/AseCommonDataTypes.h>

namespace netflix {
namespace device {


/**
 * @file IDrmSession.h The header file for DRM Session DPI.
 */

/**
 * @class IDrmSession
 * @brief Interface for DRM sessions.
 */

class IDrmSession
{
public:
    enum SessionState {
        /**
         * `LicenseAcquisitionState` is state where DrmSession is working on license
         * acquisition. Since there can be multiple license acquisitions in
         * flight for multiple video playback, and there is nccp transaction
         * involvement that can take time, when license response arrives, we
         * need to identify which session we were working on from this queue
         */
        LicenseAcquisitionState = 0,

        /**
         * InactiveDecryptionSession holds DrmSessions which already have
         * acquired license, but not bound. It does not have decryption context
         * yet
         */
        InactiveDecryptionState,

        /**
         * ActiveDecryptionSession holds DrmSessions which is bound to license,
         * and decryption context is created
         */
        ActiveDecryptionState,

        /**
         * DrmSession in this state should not be used. For example, regular
         * duration license arrives for new DrmSession while limited duration
         * license were used, DrmSession with limited duration license should be
         * put in InvalidState so that new DrmSession can be used
         */
        InvalidState
    };

    virtual ~IDrmSession() {}

    virtual void finalizeDrmSession() = 0;

    virtual std::string getDrmType() = 0;

    virtual uint32_t getSessionId() = 0;

    virtual std::string getContentId() = 0;

    virtual enum DrmLicenseType getLicenseType()= 0;

    virtual void setSessionState(enum SessionState) = 0;

    virtual enum SessionState getSessionState() = 0;

    /**
     * Generates challenge data for a particular protection system and system
     * specific data. Indicates whether a license has already been stored for
     * this data or whether additional challenge stages are required.
     *
     * The challenge data is allocated and stored in the challengeData argument
     * and will be deallocated by the caller.
     *
     *
     * @param[in] protectionSystemID: the UUID identifying the protection system
     * @param[in] systemSpecificData: data needed to generate the challenge.
     * @param[in] systemSpecificDataLen:
     * @param[out] challengedata
     *
     * @return OK if challenge data is successfully generated.
     *         LICENSE_ALREADY_STORED if no additional license acquistion is needed.
     *         ERROR if challenge generation has failed.
     */
    virtual NFErr getChallengeData(std::vector<uint8_t>& challenge, bool isLDL) = 0;
    /**
     * Stores license and returns a session ID.  The session ID will be used to
     * identify secure stops.
     *
     * @param[in] licenseData: the license data
     * @param[in] licenseDataSize: license data size.
     * @param[out] secureStopId: specifies the established DRM session.
     *
     * @return OK on success, FURTHER_STAGES_REQUIRED if this is a
     *         multi-stage challenge and additional stages are needed, or
     *         ERROR.
     */
    virtual NFErr storeLicenseData(const std::vector<uint8_t>& licenseData,
                                   std::vector<uint8_t> &secureStopId) = 0;

    /**
     * Clear a previously stored DRM license for current session.
     *
     * @return OK if succeeded, otherwise ERROR.
     */
    virtual NFErr clearLicense() = 0;

    /**
     * Decrypts the provided data using the license stored for this
     * session. The first time this is called, if a valid license is
     * found, the minimum output protection level values are set.
     *
     * @param[in] an identifier of the encryption algorithm used to encrypt the
     *         track. 0: not encrypted, 1: AES 128-bit in CTRmode.
     * @param[in] IVData the initialization vector data for the decryption process.
     * @param[in] IVDataSize the size of initialization vector data in bytes.
     * @param[in|out] dataSegments contain the data segments that will be decrypted
     *                in place.
     * @param[in] dataSegmentOffset specifies the number of bytes that were encrypted
     *                 before the first byte in the dataSegment.
     *
     * @return NFErr_OK if decryption was successful or not necessary,
     *         NFErr_DRMFailed if the license could not be found or
     *         decryption failed.
     */
    struct DataSegment
    {
        unsigned char* data;
        uint32_t numBytes;
    };

    virtual NFErr  decrypt(unsigned char* IVData,
                           uint32_t IVDataSize,
                           uint64_t dataSegmentOffset,
                           DataSegment& dataSegment) = 0;

    virtual NFErr initDecryptContext() = 0;

    virtual void cleanupDecryptContext() = 0;

    /**
     * We need to protect decrypt context from being deleted in the middle of
     * decryption. This can happen when nrdjs call deleteDrmSession while
     * decryption is happening using LDL.
     *
     */
    virtual Mutex& getDecryptContextMutex() = 0;

};

} // device
} // netflix




#endif
