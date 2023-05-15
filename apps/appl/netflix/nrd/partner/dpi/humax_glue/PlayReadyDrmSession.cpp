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

#include <nrd/AppLog.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>
#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/IDeviceError.h>
#include <nrdbase/Base64.h>

#include "ESPlayerConstants.h"
#include "PlayReadyDrmSystem.h"
#include "PlayReadyDrmSession.h"

#include "NativeBridge.h"

using namespace netflix;
using namespace netflix::device;
using namespace netflix::device::esplayer;

namespace {

// The rights we want to request.
const DRM_WCHAR PLAY[] = { ONE_WCHAR('P', '\0'),
                           ONE_WCHAR('l', '\0'),
                           ONE_WCHAR('a', '\0'),
                           ONE_WCHAR('y', '\0'),
                           ONE_WCHAR('\0', '\0')
};
const DRM_CONST_STRING PLAY_RIGHT = CREATE_DRM_STRING(PLAY);
const DRM_CONST_STRING* RIGHTS[] = { &PLAY_RIGHT };

} // namespace anonymous


/**
 * DRMPFNOUTPUTLEVELSCALLBACK for Drm_Reader_Bind. Sets the minimum
 * output protection levels.
 *
 * @param[in] outputLevels the output level data.
 * @param[in] callbackType the callback type.
 * @param[in] data the callback pass-thru data (DrmManager*).
 * @return DRM_SUCCESS if successful, DRM_FAILED if not.
 */
static DRM_RESULT outputLevelsCallback(const DRM_VOID *outputLevels,
                                       DRM_POLICY_CALLBACK_TYPE callbackType,
                                       const DRM_VOID *data)
{
    NRDP_UNUSED(data);
    NTRACE(TRACE_DRMSYSTEM,
           "outputLevelsCallback outputLevels=%p callbackType=%u data=%p",
           outputLevels, static_cast<uint32_t>(callbackType), data);

    // We only care about the play callback.
    if (callbackType != DRM_PLAY_OPL_CALLBACK)
        return DRM_SUCCESS;

    // Pull out the protection levels.
    PlayReadyDrmSession::PlayLevels* levels = (PlayReadyDrmSession::PlayLevels*)data;

    /*save default value of output control*/
    levels->isset =  true;
    levels->base_hdcpisOn = NativeBridge::system()->getHdcpStatus();
    levels->base_cgmsStatus = NativeBridge::system()->getCgmsStatus();
    /**/

    DRM_PLAY_OPL_EX* playLevels = (DRM_PLAY_OPL_EX*)outputLevels;
    levels->compressedDigitalVideoLevel_ = playLevels->minOPL.wCompressedDigitalVideo;
    levels->uncompressedDigitalVideoLevel_ = playLevels->minOPL.wUncompressedDigitalVideo;
    levels->analogVideoLevel_ = playLevels->minOPL.wAnalogVideo;
    levels->compressedDigitalAudioLevel_ = playLevels->minOPL.wCompressedDigitalAudio;
    levels->uncompressedDigitalAudioLevel_ = playLevels->minOPL.wUncompressedDigitalAudio;
    printf("[Netflix:DPI] uncompressedDigitalVideoLevel_ = %d\n",levels->uncompressedDigitalVideoLevel_);

    /*HDCP Output Protection*/
    if(levels->uncompressedDigitalVideoLevel_ == 300)
    {// HDCP On
            if(levels->base_hdcpisOn != true)
            {
                    printf("[Netflix:DPI:%d] set HDCP On(force)\n", __LINE__);
                    NativeBridge::system()->setHdcpStatus(true, true);
            }
    }
    else if(levels->uncompressedDigitalVideoLevel_ == 270)
    {
            if(levels->base_hdcpisOn != true)
            {
                    printf("[Netflix:DPI:%d] set HDCP On(not force)\n", __LINE__);
                    NativeBridge::system()->setHdcpStatus(true, false);
            }
    }
    else
    {
            if(levels->base_hdcpisOn != false)
            {
                    printf("[Netflix:DPI:%d] set HDCP Off()\n", __LINE__);
                    NativeBridge::system()->setHdcpStatus(false, false);
            }
    }

    /*CGMS-A Analog Copy Protection*/
    /*0~100 : Output of analog video content is permitted without restriction*/
    /*101~200 : Output of analog video content is permitted only when CGMS-A, is engaged with the CGMS copy field set to 3 (no more copies) */
    /*201~ : the receiver is not permitted to output analog video content. (Not requirement of NETFLIX)*/
    if(levels->analogVideoLevel_ <= 100)
    {
            if(levels->base_cgmsStatus != NXL_COPYRIGHT_COPY_FREE)
                    NativeBridge::system()->setCgmsStatus(NXL_COPYRIGHT_COPY_FREE);
    }
    else
    {
            if(levels->base_cgmsStatus != NXL_COPYRIGHT_COPY_NEVER)
                    NativeBridge::system()->setCgmsStatus(NXL_COPYRIGHT_COPY_NEVER);
    }

    // At actual device, enable/disable device output protection will be needed
    // upon receiving this protection information.
    NTRACE(TRACE_DRMSYSTEM, "Output Protection Level : compressed digital %d, uncompressed digital %d, analog video %d",
           levels->compressedDigitalVideoLevel_,
           levels->uncompressedDigitalVideoLevel_,
           levels->analogVideoLevel_);

    // All done.
    return DRM_SUCCESS;
}

PlayReadyDrmSession::PlayReadyDrmSession(uint32_t sessionId,
                                         std::string contentId,
                                         enum DrmLicenseType licenseType,
                                         const std::vector<uint8_t>& drmHeader,
                                         shared_ptr<PlayReadyDrmSystem> drmSystem)
  :mSessionState(InvalidState)
  ,mSessionId(sessionId)
  ,mContentId(contentId)
  ,mLicenseType(licenseType)
  ,mDrmSystem(drmSystem)
  ,mDrmDecryptContextMutex(DRMCONTEXT_MUTEX, "DrmSession")
{
    // copy drm header as session information
    mDrmHeader = drmHeader;
    NTRACE(TRACE_DRMSYSTEM, "sessionId %d, licenseType %d, contentId %s, drmHeader.size() %zu",
           sessionId, licenseType, Base64::encode(contentId).c_str(), drmHeader.size());

    // Assign app context(DRM_APP_CONTEXT) to mAppContext
    mAppContext = mDrmSystem->getDrmAppContext();
    memset(&levels_, 0, sizeof(struct PlayLevels));
    mSecureStopId.clear();
}

PlayReadyDrmSession::~PlayReadyDrmSession()
{
    NTRACE(TRACE_DRMSYSTEM, "%s sessionId %d", __func__, mSessionId);
    finalizeDrmSession();
    mDrmHeader.clear();
    mSecureStopId.clear();
}

void PlayReadyDrmSession::finalizeDrmSession()
{
    ScopedMutex decryptContextLock(mDrmDecryptContextMutex);
    {
        ScopedMutex systemLock(mDrmSystem->getDrmAppContextMutex());

        NTRACE(TRACE_DRMSYSTEM, "%s sessionId %d, licenseType %d", __func__, mSessionId, mLicenseType);

        if( getSessionState() == IDrmSession::LicenseAcquisitionState){
            // cancel license challenge
            cancelChallenge();
        } else if (getSessionState() != IDrmSession::InvalidState ){
            // cleanup any context created
            cleanupDecryptContext();
        }
        setSessionState(IDrmSession::InvalidState);
    }
}

std::string PlayReadyDrmSession::getDrmType()
{
    return "PlayReady";
}

uint32_t PlayReadyDrmSession::getSessionId()
{
    return mSessionId;
}

std::string PlayReadyDrmSession::getContentId()
{
    return mContentId;
}

enum DrmLicenseType PlayReadyDrmSession::getLicenseType()
{
    return mLicenseType;
}

void PlayReadyDrmSession::setSessionState(enum IDrmSession::SessionState sessionState)
{
    NTRACE(TRACE_DRMSYSTEM, "%s from %d to %d for sessionId %d, licenseType %d",
           __func__, mSessionState, sessionState, mSessionId, mLicenseType);
    mSessionState = sessionState;
}

enum IDrmSession::SessionState PlayReadyDrmSession::getSessionState()
{
    return mSessionState;
}


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
NFErr PlayReadyDrmSession::getChallengeData(std::vector<uint8_t>& challenge, bool isLDL)
{
    DRM_RESULT err;

    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::generateChallenge challenge=%p sessionId %d"
           , &challenge, mSessionId);

    // open scope for DRM_APP_CONTEXT mutex
    ScopedMutex systemLock(mDrmSystem->getDrmAppContextMutex());

    // sanity check for drm header
    if (mDrmHeader.size() == 0){
        return NFErr_NotAllowed;
    }

    // Application wide context check. PlayReadyDrmSystem should have initialized
    // DRM__APP_CONTEXT for application level. If this DRM_APP_CONTEXT does not
    // exist, each session should recover it by initializing DRM_APP_CONTEXT.
    if( mAppContext.get() == NULL) {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::%s: recovering application wide context", __func__);

        NTRACE(TRACE_DRMSYSTEM, "cleanup Key/Drm stores before Drm_Initialize");
        // Cleanup drm store and key store
        if (mDrmSystem->deleteDrmStore() != NFErr_OK ){
            NTRACE(TRACE_DRMSYSTEM, "deleteDrmStore failed before Drm_Initialize");
        }
        if (mDrmSystem->deleteKeyStore() != NFErr_OK){
            NTRACE(TRACE_DRMSYSTEM, "deleteKeyStore failed before Drm_Initialize");
        }

        NFErr error = mDrmSystem->initialize();
        if (error != NFErr_OK){
            return error;
        }
        // Assign app context(DRM_APP_CONTEXT) to mAppContext
        mAppContext = mDrmSystem->getDrmAppContext();
    } else {
        // reinitialze DRM_APP_CONTEXT - this is limitation of PlayReady 2.x
        err = Drm_Reinitialize(mAppContext.get());
        if(DRM_FAILED(err))
        {
            NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::initDrmContext: Error in Drm_Reinitialize:"
                   " 0x%08lx", static_cast<unsigned long>(err));
            std::ostringstream ss;
            ss << "PlayReadyDrmSession::initDrmContext: Error in Drm_Reinitialize:" << std::showbase << std::hex << err;
            Variant errInfo;
            errInfo["errorDescription"] = ss.str();
            return NFErr(new IDeviceError(DRMSYSTEM_INITIALIZATION_ERROR, err, errInfo));
        }
    }

    /*
     * Set the drm context's drm header property to the systemSpecificData.
     */
    NTRACE(TRACE_DRMSYSTEM, "Drm_Content_SetProperty : generateChallenge setting header");
    err = Drm_Content_SetProperty(mAppContext.get(),
                                  DRM_CSP_AUTODETECT_HEADER,
                                  &mDrmHeader[0],
                                  mDrmHeader.size());
    if (DRM_FAILED(err))
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::generateChallenge: Error in Drm_Content_SetProperty:"
                   " 0x%08lx", static_cast<unsigned long>(err));
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::generateChallenge: Error in Drm_Content_SetProperty:"
           << std::showbase << std::hex << err;
	Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_CHALLENGE_ERROR, err, errInfo));
    }

    // Figure out the buffer allocation sizes for the challenge.
    NTRACE(TRACE_DRMSYSTEM, "generateChallenge querying challenge size");
    DRM_DWORD challengeSize = 0;
#if defined(PLAYREADY2_5_TEE_LDL)
    mNounce.resize(TEE_SESSION_ID_LEN);
    err = Drm_LicenseAcq_GenerateChallenge_Netflix(mAppContext.get(),
                                                   RIGHTS,
                                                   sizeof(RIGHTS) / sizeof(DRM_CONST_STRING*),
                                                   NULL,
                                                   NULL, 0,
                                                   NULL, NULL,
                                                   NULL, NULL,
                                                   NULL, &challengeSize,
                                                   &mNounce[0], isLDL);

#else
    NRDP_UNUSED(isLDL);
    err = Drm_LicenseAcq_GenerateChallenge(mAppContext.get(),
                                           RIGHTS,
                                           sizeof(RIGHTS) / sizeof(DRM_CONST_STRING*),
                                           NULL,
                                           NULL, 0,
                                           NULL, NULL,
                                           NULL, NULL,
                                           NULL, &challengeSize);
#endif
    if (err != DRM_E_BUFFERTOOSMALL)
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::generateChallenge: Error in "
               " Drm_LicenseAcq_GenerateChallenge: 0x%08lx",
               static_cast<unsigned long>(err));
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::generateChallenge: Error in Drm_LicenseAcq_GenerateChallenge:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_CHALLENGE_ERROR, err, errInfo));
    }

    // Now get the challenge.
    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::generateChallenge: generating challenge size=%u",
           static_cast<uint32_t>(challengeSize));
    challenge.resize(challengeSize);

#if defined(PLAYREADY2_5_TEE_LDL)
    err = Drm_LicenseAcq_GenerateChallenge_Netflix(mAppContext.get(),
                                                   RIGHTS,
                                                   sizeof(RIGHTS) / sizeof(DRM_CONST_STRING*),
                                                   NULL,
                                                   NULL, 0,
                                                   NULL, NULL,
                                                   NULL, NULL,
                                                   (DRM_BYTE*)&challenge[0],
                                                   &challengeSize,
                                                   &mNounce[0],
                                                   isLDL);
#else
    err = Drm_LicenseAcq_GenerateChallenge(mAppContext.get(),
                                           RIGHTS,
                                           sizeof(RIGHTS) / sizeof(DRM_CONST_STRING*),
                                           NULL,
                                           NULL, 0,
                                           NULL, NULL,
                                           NULL, NULL,
                                           (DRM_BYTE*)&challenge[0],
                                           &challengeSize);
#endif
    if (DRM_FAILED(err))
    {
        NTRACE(TRACE_DRMSYSTEM, "generateChallenge: Error in Drm_LicenseAcq_GenerateChallenge: 0x%08lx",
               static_cast<unsigned long>(err));
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::generateChallenge: Error in Drm_LicenseAcq_GenerateChallenge:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_CHALLENGE_ERROR, err, errInfo));
    }

    // All done.
    challenge.resize(challengeSize);

    return NFErr_OK;
}

/**
 * Stores license and returns a session ID.  The session ID will be used to
 * identify secure stops.
 *
 * @param[in] licenseData: the license data
 * @param[in] licenseDataSize: license data size.
 * @param[out] sessionID: specifies the established DRM session.
 *
 * @return OK on success, FURTHER_STAGES_REQUIRED if this is a
 *         multi-stage challenge and additional stages are needed, or
 *         ERROR.
 */
NFErr PlayReadyDrmSession::storeLicenseData(const std::vector<uint8_t>& licenseData,
                                            std::vector<unsigned char> &secureStopId)
{
    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::storeLicense data=%p sessionId %d, licenseType %d"
           , &licenseData, mSessionId, mLicenseType);

    // open scope for DRM_APP_CONTEXT mutex
    ScopedMutex systemLock(mDrmSystem->getDrmAppContextMutex());

#ifndef PLAYREADY2TEE
    NRDP_UNUSED(secureStopId);
#else
    unsigned char uuid[TEE_SESSION_ID_LEN];
#endif

    DRM_RESULT err;
    DRM_LICENSE_RESPONSE resp;
	memset(&resp, 0, sizeof(DRM_LICENSE_RESPONSE));

    // reinitialze DRM_APP_CONTEXT and set DRM header for current session
    NFErr nferr = reinitializeForCurrentSession();
    if(!nferr.ok()){
        return nferr;
    }

	unsigned int i;
	printf("@@@@License Data(%d) = ", licenseData.size());
	for(i=0 ; i<licenseData.size() ; ++i)
	{
		printf("%c", licenseData[i]);
	}
	printf("\n@@@@End\n");

    /*
     * Store the license
     */
#if defined( __APPLE__ ) && !defined(PLAYREADY2_5_TEE)
    // apple build - it supports playready2, but no secure stop
    err = Drm_LicenseAcq_ProcessResponse(mAppContext.get(),
                                         DRM_PROCESS_LIC_RESPONSE_NO_FLAGS,
                                         //DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                         NULL, NULL,
                                         (DRM_BYTE*)&licenseData[0],
                                         (DRM_DWORD)licenseData.size(),
                                         uuid,
                                         &resp);

#elif defined(PLAYREADY2TEE) && !defined(PLAYREADY2_5_TEE_LDL)
    // playready2 and playready2tee for linux support secure stop
// SUCHOI!!!!!!!!!!!!!!!!!!!!!!MODIFIED ORIGINAL FUNCTION CALL : Drm_LicenseAcq_ProcessResponse_SecStop
    err = Drm_LicenseAcq_ProcessResponse_SecStop(mAppContext.get(),
                                                 DRM_PROCESS_LIC_RESPONSE_NO_FLAGS,
                                                 //DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                                 NULL, NULL,
                                                 (DRM_BYTE*)&licenseData[0],
                                                 (DRM_DWORD)licenseData.size(),
                                                 uuid,
                                                 &resp);

#elif defined(PLAYREADY2_5_TEE_LDL)
    // playready2.5-ss-tee which has LDL support - new function added
    err = Drm_LicenseAcq_ProcessResponse_Netflix(mAppContext.get(),
                                                 DRM_PROCESS_LIC_RESPONSE_NO_FLAGS,
                                                 //DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                                 NULL, NULL,
                                                 (DRM_BYTE*)&licenseData[0],
                                                 (DRM_DWORD)licenseData.size(),
                                                 uuid,
                                                 &resp);
#else
    // old playready 1.2 - no secure stop, no DRM_PROCESS_LIC_RESPONSE_NO_FLAGS
    err = Drm_LicenseAcq_ProcessResponse(mAppContext.get(),
                                         NULL, NULL,
                                         (DRM_BYTE*)&licenseData[0],
                                         (DRM_DWORD)licenseData.size(),
                                         &resp);
#endif
    if (DRM_FAILED(err)) {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::storeLicense: Error in "
                   "Drm_LicenseAcq_ProcessResponse: 0x%08lx",
                   static_cast<unsigned long>(err));
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::storeLicense: Error in Drm_LicenseAcq_ProcessResponse:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_LICENSE_ERROR, err, errInfo));
    }

#ifdef PLAYREADY2TEE
    // convert uuid to vector
    secureStopId.clear();
    secureStopId.resize(TEE_SESSION_ID_LEN);
    secureStopId.assign(uuid, uuid + TEE_SESSION_ID_LEN);
    mSecureStopId = secureStopId;
#endif

    // All done.
    return NFErr_OK;
}

/**
 * Clear a previously stored DRM license.
 *
 * @return OK if succeeded, otherwise ERROR.
 */
NFErr PlayReadyDrmSession::clearLicense()
{
    NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::clearLicense");

    // open scope for DRM_APP_CONTEXT mutex
    ScopedMutex systemLock(mDrmSystem->getDrmAppContextMutex());

    DRM_RESULT result;
    DRM_DWORD numDeleted;

    NFErr nferr = reinitializeForCurrentSession();
    if(!nferr.ok()){
        return nferr;
    }

    /*
     * Delete license for this KID (from drm header)
     */
    result = Drm_StoreMgmt_DeleteLicenses(mAppContext.get(), NULL, &numDeleted);
    if (DRM_FAILED(result))
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::clearLicense: Error in "
               "Drm_StoreMgmt_DeleteLicenses: 0x%08lx",
               static_cast<unsigned long>(result));
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::clearLicense: Error in Drm_StoreMgmt_DeleteLicenses:"
           << std::showbase << std::hex << result;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_LICENSE_ERROR, result, errInfo));
    }

    return NFErr_OK;
}


NFErr PlayReadyDrmSession::initDecryptContext()
{
    NTRACE(TRACE_DRMSYSTEM,
           "PlayReadyDrmSession::initDecryptContext sessionId %d, licenseType %d",
           mSessionId, mLicenseType);

    // open scope for DRM_APP_CONTEXT mutex
    ScopedMutex systemLock(mDrmSystem->getDrmAppContextMutex());

    DRM_RESULT err;

    /*
     * reinitialze DRM_APP_CONTEXT and set DRM header for current session for
     * simulataneous decryption support
     */
    NFErr nferr = reinitializeForCurrentSession();
    if(!nferr.ok()){
        return nferr;
    }

    /*
     * Create a decrypt context and bind it with the drm context.
     */
    if (decryptContext_.get()){
        // we already have initialized decrypt context.
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::initDecryptContext: decrypContext already initialized");
        return NFErr_OK;
    }
    decryptContext_.reset(new DRM_DECRYPT_CONTEXT);
    memset(decryptContext_.get(), 0, sizeof(*decryptContext_));

#if !defined(PLAYREADY2_5_TEE_LDL)
    err = Drm_Reader_Bind(mAppContext.get(),
                          RIGHTS,
                          sizeof(RIGHTS) / sizeof(DRM_CONST_STRING*),
                          &outputLevelsCallback, &levels_,
                          decryptContext_.get());
#else
    if(mSecureStopId.size() == TEE_SESSION_ID_LEN ){
        err = Drm_Reader_Bind_Netflix(mAppContext.get(),
                                      RIGHTS,
                                      sizeof(RIGHTS) / sizeof(DRM_CONST_STRING*),
                                      &outputLevelsCallback, &levels_,
                                      &mSecureStopId[0],
                                      decryptContext_.get());
    } else {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::initDecryptContext: Error securestopId not valid");
        decryptContext_.reset();
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::initDecryptContext: Error securestopId not valid";
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR, skeletonDeviceSpecific_InvalidDrmSecureStopId, errInfo));
    }
#endif

    if (DRM_FAILED(err))
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::initDecryptContext: Error in Drm_Reader_Bind"
               " 0x%08lx", static_cast<unsigned long>(err));
        decryptContext_.reset();
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::initDecryptContext:Error in Drm_Reader_Bind "
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR, err, errInfo));
    }

    err = Drm_Reader_Commit(mAppContext.get(), &outputLevelsCallback, &levels_);
    if (DRM_FAILED(err))
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::initDecryptContext: Error inDrm_Reader_Commit 0x%08lx",
               static_cast<unsigned long>(err));
        decryptContext_.reset();
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::initDecryptContext:Error inDrm_Reader_Commit"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR, err, errInfo));
    }
    return NFErr_OK;
}

NFErr PlayReadyDrmSession::decrypt(unsigned char* IVData,
                                   uint32_t IVDataSize,
                                   uint64_t byteOffset,
                                   DataSegment& dataSegment)
{
    // decrypt does not use DRM_APP_CONTEXT, so, it should be safe to use
    // different mutex between what protect DRM_APP_CONTEXT and
    // DRM_DECRYPT_CONTEXT. However, underlying secure stop related code may be
    // vulnarable to sharing those, so here, let's protect decrypt context also
    // with mutext that protect DRM_APP_CONTEXT

    // open scope for DRM_APP_CONTEXT mutex
    ScopedMutex systemLock(mDrmSystem->getDrmAppContextMutex());

    NRDP_UNUSED(IVDataSize);
    assert(IVDataSize > 0);
    unsigned char* data = dataSegment.data;
    uint32_t size = dataSegment.numBytes;
    if(size == 0){
        return NFErr_OK;
    }

    //NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::decrypt data=%p length=%d", data, size);

    // If we don't yet have a decryption context, get one from the session.
    if (!decryptContext_.get()) {
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::decrypt: we don't yet have a decryption context:";
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR,
                                      skeletonDeviceSpecific_NoDecryptionContext,
                                      errInfo));
    }

    // Initialize the decryption context for Cocktail packaged
    // content. This is a no-op for AES packaged content.
    DRM_RESULT err = DRM_SUCCESS;
    if (size <= 15)
    {
        err = Drm_Reader_InitDecrypt(decryptContext_.get(),
                                     (DRM_BYTE*)data, size);
    }
    else
    {
        err = Drm_Reader_InitDecrypt(decryptContext_.get(),
                                     (DRM_BYTE*)(data + size - 15), size);
    }
    if (DRM_FAILED(err))
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::decrypt: error in Drm_Reader_InitDecrypt:"
               " 0x%08lx",static_cast<unsigned long>(err));
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::decrypt: Error in Drm_Reader_InitDecrypt:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR, err, errInfo));
    }

    // Decrypt.

    // The documentation indicates the AES Counter-mode context
    // pointer should be NULL for Cocktail packaged content, but
    // it is ignored so we can pass it in anyway.

    //printf("IVSize %d,\t byteOffset %lld\n", IVDataSize, byteOffset);

    DRM_AES_COUNTER_MODE_CONTEXT ctrContext;
    if (IVData && IVDataSize == 8) {
        NETWORKBYTES_TO_QWORD(ctrContext.qwInitializationVector, IVData, 0);
        ctrContext.qwBlockOffset = byteOffset >> 4;
        ctrContext.bByteOffset = (DRM_BYTE)(byteOffset & 0xf);
    } else if (IVData && IVDataSize == 16) {
        NETWORKBYTES_TO_QWORD(ctrContext.qwInitializationVector, IVData, 0);
        NETWORKBYTES_TO_QWORD(byteOffset, IVData, 8);
        ctrContext.qwBlockOffset = byteOffset;
        ctrContext.bByteOffset = 0;
    } else  {
        ctrContext.qwInitializationVector = 0;
        ctrContext.qwBlockOffset = byteOffset >> 4;
        ctrContext.bByteOffset = (DRM_BYTE)(byteOffset & 0xf);
    }

    //printf("IVSize %d,\t byteOffset %lld,\t qwBlockOffset %lld,\t bByteOffset %d, numBytes %d\n",
    //       IVDataSize, byteOffset, ctrContext.qwBlockOffset, ctrContext.bByteOffset, dataSegment.numBytes);

    err = Drm_Reader_Decrypt(decryptContext_.get(), &ctrContext,
                             (DRM_BYTE*)dataSegment.data,
                             dataSegment.numBytes);
    if (DRM_FAILED(err))
    {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::decrypt: error in Drm_Reader_Decrypt:"
               " 0x%08lx", static_cast<unsigned long>(err));
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::decrypt: Error in Drm_Reader_Decrypt:" << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR, err, errInfo));
    }
    // All good.
    return NFErr_OK;
}

void PlayReadyDrmSession::cleanupDecryptContext()
{
    NTRACE(TRACE_DRMSYSTEM,
           "PlayReadyDrmSession::cleanupDecryptContext sessionId %d, licenseType %d",
           mSessionId, mLicenseType);

    // mutex is set at finalizeDrmSession

    if(levels_.isset == true)
    {
            if(levels_.base_hdcpisOn != NativeBridge::system()->getHdcpStatus()){
                    NativeBridge::system()->setHdcpStatus(levels_.base_hdcpisOn, false);
            }
            if(levels_.base_cgmsStatus != NativeBridge::system()->getCgmsStatus()){
                    NativeBridge::system()->setCgmsStatus((eNXL_CopyRight)levels_.base_cgmsStatus);
            }
            levels_.isset = false;
    }


#if defined(PLAYREADY2TEE)
    DRM_RESULT err;

    if(decryptContext_.get()){
        /*
         * unbind decrypt context from drm context
         */
        err = Drm_Reader_Unbind(mAppContext.get(), decryptContext_.get());
        if (DRM_FAILED(err)) {
            NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::cleanupDecryptContext: Error in "
                   "Drm_Reader_Unbind: 0x%08lx",
                   static_cast<unsigned long>(err));
        }
    } else {
        // This means that we created DrmSession, but never bound it to
        // DRM_DECRYPT_CONTEXT. In this case, if we have a license to bind, we
        // need to bind and unbind to create a secure stop.  This is requirement
        // from security team.

        NTRACE(TRACE_DRMSYSTEM, "DecryptContext not created: temporarily bind and unbind");

        reinitializeForCurrentSession();

        // now bind and unbind it
        //const DRM_CONST_STRING* RIGHTS[] = { &PLAY_RIGHT };
        decryptContext_.reset(new DRM_DECRYPT_CONTEXT);
        memset(decryptContext_.get(), 0, sizeof(*decryptContext_));
#if !defined(PLAYREADY2_5_TEE_LDL)
        err = Drm_Reader_Bind(mAppContext.get(),
                              RIGHTS,
                              sizeof(RIGHTS) / sizeof(DRM_CONST_STRING*),
                              &outputLevelsCallback, &levels_,
                              decryptContext_.get());
#else
        if(mSecureStopId.size() == TEE_SESSION_ID_LEN ){
            err = Drm_Reader_Bind_Netflix(mAppContext.get(),
                                          RIGHTS,
                                          sizeof(RIGHTS) / sizeof(DRM_CONST_STRING*),
                                          &outputLevelsCallback, &levels_,
                                          &mSecureStopId[0],
                                          decryptContext_.get());
        } else {
            NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::cleanupDecryptContext:Error securestopId not valid");
            decryptContext_.reset();
            return;
        }
#endif
        if (DRM_FAILED(err)) {
            // this is not happening during playback. No need to logblob
            NTRACE(TRACE_DRMSYSTEM, "Error Drm_Reader_Bind: 0x%08lx while creating temporary DRM_DECRYPT_CONTEXT",
                   static_cast<unsigned long>(err));
            decryptContext_.reset();
            return ;
        }

        err = Drm_Reader_Unbind(mAppContext.get(), decryptContext_.get());
        if (DRM_FAILED(err))
        {
            NTRACE(TRACE_DRMSYSTEM, "Error Drm_Reader_Unbind: 0x%08lx when creating temporary DRM_DECRYPT_CONTEXT",
                   static_cast<unsigned long>(err));
        }
    }

    err = Drm_Reader_Commit(mAppContext.get(), NULL, NULL);
    if(DRM_FAILED(err)){
        // nothing that we can do about. Just log
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSystem::%s Drm_Reader_Commit 0x%08lx",
               __func__, static_cast<unsigned long>(err));
    }

#endif
    decryptContext_.reset();
}

void PlayReadyDrmSession::cancelChallenge()
{
    // scoped mutex is set at finalizeDrmSession
    NTRACE(TRACE_DRMSYSTEM, "%s sessionId %d, licenseType %d", __func__, mSessionId, mLicenseType);

#if defined(PLAYREADY2_5_TEE_LDL)
    DRM_RESULT err;
    err = Drm_LicenseAcq_CancelChallenge_Netflix(mAppContext.get(), &mNounce[0]);
    if (DRM_FAILED(err)) {
        NTRACE(TRACE_DRMSYSTEM, "Error Drm_LicenseAcq_CancelChallenge_Netflix: 0x%08lx",
               static_cast<unsigned long>(err));
    }
#endif
}

NFErr PlayReadyDrmSession::reinitializeForCurrentSession()
{
    /*
     * Simultaneous decryption support - when multiple license requests were in
     * flight, and license were retrived and ready to be stored, we should
     * reinitialize app context for current license to be stored
     */

    /*
     * Do not put mutex here! Calling function is holding a mutex
     */

#if defined(PLAYREADY2_5)
    DRM_RESULT err;
    err = Drm_Reinitialize(mAppContext.get());
    if (DRM_FAILED(err)) {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::storeLicense: Error in "
               "Drm_Reinitialize: 0x%08lx",
               static_cast<unsigned long>(err));
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::" << __func__
           << " Error in Drm_Reinitialize:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_INITIALIZATION_ERROR, err, errInfo));
    }

    err = Drm_Content_SetProperty(mAppContext.get(),
                                  DRM_CSP_AUTODETECT_HEADER,
                                  &mDrmHeader[0],
                                  mDrmHeader.size());
    if (DRM_FAILED(err)) {
        NTRACE(TRACE_DRMSYSTEM, "PlayReadyDrmSession::storeLicense: Error in "
               "Drm_Content_SetProperty: 0x%08lx",
               static_cast<unsigned long>(err));
        std::ostringstream ss;
        ss << "PlayReadyDrmSession::" << __func__
           << " Error in Drm_Content_SetProperty:"
           << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DRMSYSTEM_INITIALIZATION_ERROR, err, errInfo));
    }
#endif
    return NFErr_OK;
}

Mutex& PlayReadyDrmSession::getDecryptContextMutex()
{
    return mDrmDecryptContextMutex;
}
