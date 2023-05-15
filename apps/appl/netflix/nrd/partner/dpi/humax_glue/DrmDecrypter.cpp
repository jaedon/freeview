/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DrmDecrypter.h"

#include <nrdbase/ScopedMutex.h>

#include <nrd/AppLog.h>

#include "DrmError.h"
#include "ESManagerNative.h"
#include "PlaybackGroupNative.h"

using namespace netflix::device::esplayer;
using namespace netflix;
using namespace netflix::device;

#if defined(PLAYREADY)
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
    NTRACE(TRACE_WMDRM,
           "outputLevelsCallback outputLevels=%p callbackType=%u data=%p",
           outputLevels, static_cast<uint32_t>(callbackType), data);

    // We only care about the play callback.
    if (callbackType != DRM_PLAY_OPL_CALLBACK)
        return DRM_SUCCESS;

    // Pull out the protection levels.
    DrmDecrypter::PlayLevels* levels = (DrmDecrypter::PlayLevels*)data;
    DRM_PLAY_OPL_EX* playLevels = (DRM_PLAY_OPL_EX*)outputLevels;
    levels->compressedDigitalVideoLevel_ = playLevels->minOPL.wCompressedDigitalVideo;
    levels->uncompressedDigitalVideoLevel_ = playLevels->minOPL.wUncompressedDigitalVideo;
    levels->analogVideoLevel_ = playLevels->minOPL.wAnalogVideo;
    levels->compressedDigitalAudioLevel_ = playLevels->minOPL.wCompressedDigitalAudio;
    levels->uncompressedDigitalAudioLevel_ = playLevels->minOPL.wUncompressedDigitalAudio;

    // At actual device, enable/disable device output protection will be needed
    // upon receiving this protection information.
    NTRACE(TRACE_WMDRM, "Output Protection Level : compressed digital %d, uncompressed digital %d, analog video %d",
           levels->compressedDigitalVideoLevel_,
           levels->uncompressedDigitalVideoLevel_,
           levels->analogVideoLevel_);

    // All done.
    return DRM_SUCCESS;
}
#endif

DrmDecrypter::DrmDecrypter(PlaybackGroupNative *playbackGroup)
{
    NRDP_UNUSED(playbackGroup);
}

DrmDecrypter::~DrmDecrypter()
{
    cleanupDecryptContext();
}

NFErr DrmDecrypter::decrypt(unsigned char* IVData,
              uint32_t IVDataSize,
              uint64_t byteOffset,
              DataSegment& dataSegment)
{
    NRDP_UNUSED(IVData);
    NRDP_UNUSED(IVDataSize);
    NRDP_UNUSED(byteOffset);
    NRDP_UNUSED(dataSegment);

#if defined(PLAYREADY)
    assert(IVDataSize > 0);
    unsigned char* data = dataSegment.data;
    uint32_t size = dataSegment.numBytes;

    if(size == 0)
    {
        return NFErr_OK;
    }

    NTRACE(TRACE_WMDRM, "decrypt data=%p length=%d", data, size);

    // If we don't yet have a decryption context, get one from the session.
    if (!decryptContext_.get()) {
        std::ostringstream ss;
        ss << "DrmDecrypter::decrypt: we don't yet have a decryption context:";
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR,
                                      refAppDeviceSpecific_NoDecryptionContext,
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
        NTRACE(TRACE_WMDRM, "DrmDecrypter::decrypt: error in Drm_Reader_InitDecrypt:"
               " 0x%08lx",static_cast<unsigned long>(err));
        std::ostringstream ss;
        ss << "DrmDecrypter::decrypt: Error in Drm_Reader_InitDecrypt:" << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR, err, errInfo));
    }

    // Decrypt.

    // The documentation indicates the AES Counter-mode context
    // pointer should be NULL for Cocktail packaged content, but
    // it is ignored so we can pass it in anyway.
    DRM_AES_COUNTER_MODE_CONTEXT ctrContext;
    if (IVData)
    {
        NETWORKBYTES_TO_QWORD(ctrContext.qwInitializationVector, IVData, 0);
    }
    else
    {
        ctrContext.qwInitializationVector = 0;
    }
    ctrContext.qwBlockOffset = byteOffset >> 4;
    ctrContext.bByteOffset = (DRM_BYTE)(byteOffset & 0xf);
    err = Drm_Reader_Decrypt(decryptContext_.get(), &ctrContext,
                             (DRM_BYTE*)dataSegment.data,
                             dataSegment.numBytes);
    if (DRM_FAILED(err))
    {
        NTRACE(TRACE_WMDRM, "DrmDecrypter::decrypt: error in Drm_Reader_Decrypt:"
               " 0x%08lx", static_cast<unsigned long>(err));
        std::ostringstream ss;
        ss << "DrmDecrypter::decrypt: Error in Drm_Reader_Decrypt:" << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR, err, errInfo));
    }
#endif
    // All good.
    return NFErr_OK;
}

NFErr DrmDecrypter::initDecryptContext()
{
#if defined(PLAYREADY)
    ScopedMutex lock(DrmManager::getDrmContextMutex());

    DRM_RESULT err;

    /*
     * Create a decrypt context and bind it with the drm context.
     */
    decryptContext_.reset(new DRM_DECRYPT_CONTEXT);
    memset(decryptContext_.get(), 0, sizeof(*decryptContext_));
    err = Drm_Reader_Bind(DrmManager::getDrmContext().get(),
                          RIGHTS,
                          sizeof(RIGHTS) / sizeof(DRM_CONST_STRING*),
                          &outputLevelsCallback, &levels_,
                          decryptContext_.get());

    if (DRM_FAILED(err))
    {
        NTRACE(TRACE_WMDRM, "DrmDecrypter::initDecryptContext: Error in Drm_Reader_Bind"
               " 0x%08lx", static_cast<unsigned long>(err));
        decryptContext_.reset();

        std::ostringstream ss;
        ss << "DrmDecrypter::initDecryptContext:Error in Drm_Reader_Bind" << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR, err, errInfo));
    }

    err = Drm_Reader_Commit(DrmManager::getDrmContext().get(), &outputLevelsCallback,
                            &levels_);
    if (DRM_FAILED(err))
    {
        NTRACE(TRACE_WMDRM, "DrmDecrypter::initDecryptContext: Error inDrm_Reader_Commit 0x%08lx",
               static_cast<unsigned long>(err));
        decryptContext_.reset();

        std::ostringstream ss;
        ss << "DrmDecrypter::initDecryptContext:Error inDrm_Reader_Commit" << std::showbase << std::hex << err;
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        return NFErr(new IDeviceError(DECRYPTION_ERROR, err, errInfo));
    }
#endif
    return NFErr_OK;
}

void DrmDecrypter::cleanupDecryptContext()
{
#if defined(PLAYREADY2TEE)
    DRM_RESULT err;
    /*
     * unbind decrypt context from drm context
     */
    err = Drm_Reader_Unbind(DrmManager::getDrmContext().get(), decryptContext_.get());
    if (DRM_FAILED(err)) {
        NTRACE(TRACE_WMDRM, "DrmManager::cleanupDecryptContext: Error in "
               "Drm_Reader_Unbind: 0x%08lx",
               static_cast<unsigned long>(err));
    }
#endif
}



